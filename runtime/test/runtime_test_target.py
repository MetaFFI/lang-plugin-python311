import sys
import time

def hello_world()->None:
	print('Hello World, from Python3')


def returns_an_error()->None:
	print('going to throw exception')
	raise Exception('Error')


def div_integers(x:int, y:int)->float:
	print('dividing {}/{}'.format(x, y))
	return x/ y


def join_strings(arr)->str:
	res = ','.join(arr)
	return res

five_seconds = 5
def wait_a_bit(secs : int):
	time.sleep(secs)
	return None

class testmap:
	def __init__(self):
		self.curdict = dict()
		self.name = 'name1'


	def set(self, k: str, v):
		self.curdict[k] = v


	def get(self, k:str):
		v = self.curdict[k]
		return v


	def contains(self, k:str):
		return k in self.curdict


class SomeClass:
	def print(self):
		print("Hello from inner class")


def get_some_classes():
	return [SomeClass() for _ in range(3)]


def expect_three_some_classes(arr):
	if len(arr) != 3:
		raise ValueError("Array length is not 3")


def expect_three_buffers(buffers):
	if len(buffers) != 3:
		raise ValueError("Buffers length is not 3")


def get_three_buffers():
	buffers = []
	for _ in range(3):
		buffers.append(bytes([1, 2, 3]))
	return buffers
