from genericpath import isdir
import platform
from typing import List, Tuple, Dict
import glob
import os
from colorama import Fore

# ret[0] - win files, ret[1] - ubuntu files

def get_files(win_metaffi_home: str, ubuntu_metaffi_home: str) -> Tuple[Dict[str, str], Dict[str, str]]:
	# get all files from $METAFFI_HOME/go - the installed dir of this project recursively
	# don't continue recursively if the directory starts with '__'

	pluginname = 'python311'
	
	win_metaffi_home = win_metaffi_home.replace('\\', '/')
	ubuntu_metaffi_home = ubuntu_metaffi_home.replace('\\', '/')

	win_files = {}
	for file in glob.glob(win_metaffi_home + f'/{pluginname}/**', recursive=True):		
		if os.path.isfile(file) and '__' not in file:
			file = file.replace('\\', '/')
			win_files[file.removeprefix(win_metaffi_home+f'/{pluginname}/')] = file

	assert len(win_files) > 0, f'No files found in {win_metaffi_home}/{pluginname}'

	ubuntu_files = {}
	for file in glob.glob(ubuntu_metaffi_home + f'/{pluginname}/**', recursive=True):
		if os.path.isfile(file) and '__' not in file:
			file = file.replace('\\', '/')
			ubuntu_files[file.removeprefix(ubuntu_metaffi_home+f'/{pluginname}/')] = file

	assert len(ubuntu_files) > 0, f'No files found in {ubuntu_metaffi_home}/{pluginname}'

	# * copy the api tests
	current_script_dir = os.path.dirname(os.path.abspath(__file__))
	api_tests_files = glob.glob(f'{current_script_dir}/api/tests/**', recursive=True)
	for file in api_tests_files:
		if '__pycache__' in file:
			continue

		if os.path.isfile(file):
			target = file.replace('\\', '/').removeprefix(current_script_dir.replace('\\', '/')+'/api/')
			win_files[target] = file
			ubuntu_files[target] = file

	# * uninstaller
	win_files['uninstall_plugin.py'] = os.path.dirname(os.path.abspath(__file__))+'/uninstall_plugin.py'
	ubuntu_files['uninstall_plugin.py'] = os.path.dirname(os.path.abspath(__file__))+'/uninstall_plugin.py'

	return win_files, ubuntu_files


def setup_environment():
	pass


def check_prerequisites() -> bool:

	if platform.system() == 'Windows':
		import shutil		
		# first check if python3 is installed (use the PATH environment variable to search for python.exe or python3.exe)
		python3_path = shutil.which('python3')
		python_path = shutil.which('python')

		if python3_path is None and python_path is None:
			print('Python3 is not installed')
			return False

	elif platform.system() == 'Linux':
		import shutil
		python3_path = shutil.which('python3')
		
		if python3_path is None:
			print('Python3 is not installed')
			return False
	else:
		print(f'Unsupported platform: {platform.system()}')
		return False

	return True

def print_prerequisites():
	print("""Prerequisites:\n\tPython3.11 (for ubuntu python3.11-dev package is also required)""")


def get_version():
	return '0.3.0'

