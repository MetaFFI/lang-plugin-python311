from inspect import *
import importlib
import os
import sys
from typing import List
import types
import site
import re

class variable_info:
	name: str
	type: str


class parameter_info:
	name: str
	type: str
	default_val: str


class function_info:
	name: str
	comment: str
	parameters: List[variable_info]
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
			if not self._is_variable(m[1]):
				continue

			if m[0].startswith('_'):
				continue

			global_vars.append(self._extract_field(m))

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
		res = []

		for c in getmembers(self.mod, isclass):
			clsdata = class_info()
			clsdata.name = c[0]
			clsdata.comment = getcomments(c[1])
			if clsdata.comment is not None:
				clsdata.comment = clsdata.comment.replace('#', '', 1).strip()

			constructor_found = False
			for member in getmembers(c[1]):
				if member[0] == '__annotations__':
					for k, v in member[1].items():
						if k.startswith('_'):
							continue

						clsdata.fields.append(self._extract_field([k, v]))

				elif isfunction(member[1]):
					if member[0].startswith('_') and member[0] != '__init__':
						continue

					if member[0] == '__init__':
						constructor_found = True

					clsdata.methods.append(self._extract_function(member, clsdata.name))
				else:
					if member[0].startswith('_'):
						continue
					else:
						print('Skipping {} of type {}'.format(member[0], type(member[1]).__name__))
						continue

			# make sure class has a constructor, if not, add the default one
			if not constructor_found:
				clsdata.methods.append(self._default_constructor(clsdata.name))

			res.append(clsdata)

		return res

	def _extract_field(self, m) -> variable_info:
		v = variable_info()
		v.name = m[0]

		if hasattr(m[1], '__name__'):
			v.type = m[1].__name__.strip()
		else:
			v.type = type(m[1]).__name__.strip()

		if v.type == '_empty':
			v.type = 'any'
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
		for name, param in sig.parameters.items():
			typedata = str(param).split(':', -1)

			pdata = parameter_info()
			if len(typedata) != 2:
				# no type hint
				pdata.name = typedata[0].strip()
				pdata.type = 'any'
			else:
				# type hint
				pdata.name = typedata[0].strip()
				pdata.type = typedata[1].strip()

			if '=' in pdata.name:  # default value
				name_and_val = pdata.name.split('=')
				pdata.name = name_and_val[0]
				pdata.default_val = name_and_val[1]

			if '=' in pdata.type:  # default value
				type_and_val = pdata.type.split('=')
				pdata.type = type_and_val[0].strip()
				pdata.default_val = type_and_val[1]

			func_info.parameters.append(pdata)

		# parse return value
		if func_info.name == '__init__':
			if clsname is None:
				raise ValueError('No class name for constructor')

			func_info.return_values.append(clsname)
		else:
			if sig.return_annotation is not None:
				rettype = sig.return_annotation.__name__.strip()
				if rettype == '_empty':
					func_info.return_values.append('any')
				else:
					func_info.return_values.append(rettype)

		return func_info
