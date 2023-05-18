from typing import Any, Callable, DefaultDict, Deque, Dict, Iterable, List, Optional, Tuple
import inspect
import sys

class __collection__deque:
	def __init__(self, iterable: Optional[Iterable] = None, maxlen: Optional[int] = None):
		pass

	def append(self, x: Any) -> None:
		pass

	def appendleft(self, x: Any) -> None:
		pass

	def clear(self) -> None:
		pass

	def count(self, x: Any) -> int:
		pass

	def extend(self, iterable: Iterable) -> None:
		pass

	def extendleft(self, iterable: Iterable) -> None:
		pass

	def pop(self) -> Any:
		pass

	def popleft(self) -> Any:
		pass

	def remove(self, value: Any) -> None:
		pass

	def reverse(self) -> None:
		pass

	def rotate(self, n: int = 1) -> None:
		pass


class __collection__defaultdict:
	def __init__(self, default_factory: Optional[Callable] = None):
		pass

	def __missing__(self, key: Any) -> Any:
		pass


class __collection__Counter:
	def __init__(self, iterable: Optional[Iterable] = None):
		pass

	def elements(self) -> Iterable:
		pass

	def most_common(self, n: Optional[int] = None) -> List[Tuple[Any, int]]:
		pass

	def subtract(self, iterable: Optional[Iterable] = None):
		pass


class __collection__OrderedDict:
	def __init__(self):
		pass


def get_method_definition(module_name: str, class_name: str, method_name:str):
	modified_class_name = '__{}__{}'.format(module_name, class_name)
	for c in inspect.getmembers(sys.modules[__name__], inspect.isclass):
		if c[0] == modified_class_name:
			for m in inspect.getmembers(c[1], inspect.isfunction):
				if m[0] == method_name:
					return m[1]

			#print('found {}.{} in "none python implementations definitions", but not the method {}'.format(module_name, class_name, method_name))
			return None

	#print('did not find class {}.{} in "none python implementations definitions"'.format(module_name, class_name))
	return None
