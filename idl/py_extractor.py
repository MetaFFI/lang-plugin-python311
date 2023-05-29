import builtins
from inspect import *
import importlib
import os
import sys
from typing import List
import types
import re
import none_python_impl_definitions

ignored_builtins = { 'False', 'Ellipsis', 'None', 'True', 'NotImplemented', 'super'}

class variable_info:
	name: str
	type: str
	is_getter: bool
	is_setter: bool

	def __init__(self):
		self.is_getter = True
		self.is_setter = True


class parameter_info:
	name: str
	type: str
	is_default_value: bool
	is_optional: bool
	kind: str

	def __init__(self):
		self.is_default_value = False
		self.is_optional = False
		self.kind = ''


class function_info:
	name: str
	comment: str
	parameters: List[parameter_info]
	return_values: List[str]

	def __init__(self):
		self.parameters = list()
		self.return_values = list()


class class_info:
	name: str
	comment: str
	fields: List[variable_info]
	methods: List[function_info]

	def __init__(self):
		self.fields = list()
		self.methods = list()


class py_info:
	globals: List[variable_info]
	functions: List[function_info]
	classes: List[class_info]

	def __init__(self):
		self.globals = list()
		self.functions = list()
		self.classes = list()


class py_extractor:
	filename: str
	mod: types.ModuleType

	def __init__(self, filename: str):
		filename = filename.replace('\\', '/')  # replace win-style slash to *nix style (which is supported in win)
		self.filename = filename

		if not self.in_site_package(filename):
			sys.path.append(os.path.dirname(filename))
			module_name = os.path.basename(filename)
			module_name = os.path.splitext(module_name)[0]

		else:  # if installed in site-package
			module_name = re.sub('.*site-packages/', '', filename)
			module_name = module_name.replace('.py', '')
			module_name = module_name.replace('/', '.')

		print('Py IDL Extractor: Going to parse ' + self.filename)
		print('Py IDL Extractor: Importing '+module_name)
		self.mod = importlib.import_module(module_name)

	def in_site_package(self, path: str) -> bool:
		return '/site-packages' in path

	def extract(self) -> py_info:
		info = py_info()
		info.globals = self._extract_globals()
		info.functions = self._extract_functions()
		info.classes = self._extract_classes()

		return info

	def _is_variable(self, obj):
		is_not_var = isfunction(obj) or isclass(obj) or ismodule(obj)
		return not is_not_var

	def _extract_globals(self) -> List[variable_info]:
		all_members = getmembers(self.mod)
		global_vars = []
		for m in all_members:
			if self.mod.__name__ == 'builtins' and m[0] in ignored_builtins:  # if builtin module, skip special cases
				continue

			if not self._is_variable(m[1]):
				continue

			if m[0].startswith('_'):
				continue

			global_vars.append(self._extract_field(m, True, True))

		return global_vars

	def _extract_functions(self) -> List[function_info]:
		functions_members = getmembers(self.mod, isfunction)
		res = []

		for f in functions_members:
			if f[0].startswith('_'):
				continue

			res.append(self._extract_function(f, None))

		return res

	def _extract_classes(self) -> List[class_info]:
		global ignored_builtins

		res = []

		for c in getmembers(self.mod, isclass):
			clsdata = class_info()
			clsdata.name = c[0]
			clsdata.comment = getcomments(c[1])
			if clsdata.comment is not None:
				clsdata.comment = clsdata.comment.replace('#', '', 1).strip()

			constructor_found = False
			for member in getmembers(c[1]):
				if member[0] == '__annotations__' and not isgetsetdescriptor(member[1]):  # fields

					for k, v in member[1].items():
						if k.startswith('_'):
							continue

						clsdata.fields.append(self._extract_field([k, v], True, True))

				elif ismethoddescriptor(member[1]):

					# check if method is in "non_python_method_definitions"
					method_data = none_python_impl_definitions.get_method_definition(self.mod.__name__, clsdata.name, member[0])
					if method_data is None:
						print(f'Skipping {self.mod.__name__}.{clsdata.name}.{member[0]} as it is not implemented in python, and definition not found in non_python_method_definitions')
						continue

					if member[0] == '__init__':
						constructor_found = True

					clsdata.methods.append(self._extract_function((member[0], method_data), clsdata.name))

				elif isfunction(member[1]):
					# if member[0].startswith('_') and member[0] != '__init__':
					# 	continue

					if member[0] == '__init__':
						constructor_found = True

					clsdata.methods.append(self._extract_function(member, clsdata.name))
				elif isinstance(member[1], builtins.property):
					if member[1].fget is not None or member[1].fset is not None:
						clsdata.fields.append(self._extract_field(member, member[1].fget is not None, member[1].fset is not None))
				else:
					if member[0].startswith('_'):
						continue
					else:
						print('Skipping {} of type {}'.format('{}.{}'.format(clsdata.name, member[0]), type(member[1]).__name__))
						continue

			# make sure class has a constructor, if not, add the default one
			if not constructor_found:
				clsdata.methods.append(self._default_constructor(clsdata.name))

			res.append(clsdata)

		return res

	def _extract_field(self, m, is_getter, is_setter) -> variable_info:
		v = variable_info()
		v.name = m[0]

		if isinstance(m[1], builtins.property):
			if m[1].fget is not None:
				sig = signature(m[1].fget)
				if sig.return_annotation is None:
					v.type = 'any'
				else:
					v.type = sig.return_annotation.__name__
			elif m[1].fset is not None:
				sig = signature(m[1].fset)
				if sig.return_annotation is None:
					v.type = 'any'
				else:
					v.type = sig.return_annotation.__name__
			else:
				raise ValueError('property {} does not have a getter nor a setter'.format(m[0]))
		else:
			v.type = type(m[1]).__name__.strip()

		if v.type == '_empty':
			v.type = 'any'

		v.is_getter = is_getter
		v.is_setter = is_setter

		return v

	def _default_constructor(self, clsname: str) -> function_info:
		if clsname is None:
			raise ValueError('No class name for default constructor')

		func_info = function_info()
		func_info.name = '__init__'
		func_info.comment = 'Default Constructor'
		func_info.return_values.append(clsname)

		return func_info

	def _extract_function(self, f, clsname: str) -> function_info:
		func_info = function_info()
		func_info.name = f[0]

		func_info.comment = getcomments(f[1])
		if func_info.comment is not None:
			func_info.comment = func_info.comment.replace('#', '', 1).strip()

		sig = signature(f[1])

		# parse parameters
		print('{}('.format(f[0]), end=' ')
		for name, param in sig.parameters.items():

			pdata = parameter_info()
			pdata.name = name

			if param.annotation != param.empty:
				if isinstance(param.annotation, str):
					pdata.type = param.annotation
				elif isinstance(param.annotation, types.UnionType):
					pdata.type = 'any'
				else:
					pdata.type = param.annotation.__name__
			else:
				pdata.type = 'any'

			if pdata.type == 'typing.Any' or pdata.type == 'Any':
				pdata.type = 'any'

			pdata.is_default_value = param.default != param.empty
			pdata.is_optional = False
			pdata.kind = param.kind.name

			# cleanup the name
			pdata.name = pdata.name.replace('*', '')

			print('{} {},'.format(pdata.type, pdata.name), end=' ')
			func_info.parameters.append(pdata)
		print(')')

		# parse return value
		if func_info.name == '__init__':
			if clsname is None:
				raise ValueError('No class name for constructor')

			func_info.return_values.append(clsname)
		else:
			if sig.return_annotation is not None:
				if isinstance(sig.return_annotation, str):
					rettype = sig.return_annotation
				elif sig.empty == sig.return_annotation:
					rettype = 'any'
				else:
					rettype = sig.return_annotation.__name__.strip()
					if rettype == '_empty':
						raise RuntimeError('Shouldnt reach here! what sig.return_annotation type: {}'.format(type(sig.return_annotation)))

				if rettype == 'typing.Any' or rettype == 'Any':
					rettype = 'any'

				# print('{} return type is {}'.format(f[0], rettype))
				func_info.return_values.append(rettype)

		return func_info

if '__main__' == __name__:
	ext = py_extractor('collections')
	ext.extract()
