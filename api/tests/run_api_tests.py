# python script to run unitests for api using subprocess

import subprocess
import os
import sys
from colorama import init, Fore, Style
import platform

# Initialize colorama
init()

# Get the current path of this Python script
current_path = os.path.dirname(os.path.abspath(__file__))


def get_extension_by_platform() -> str:
	if platform.system() == 'Windows':
		return '.dll'
	elif platform.system() == 'Darwin':
		return '.dylib'
	else:
		return '.so'


# Define a function to run a script and print its output
def run_script(script_path, is_unittest=False):
	# Print the name of the script in green
	print(f'{Fore.CYAN}Running script: {script_path}{Fore.RESET}')
	
	# Determine the command to run the script
	python_command = 'python' if platform.system() == 'Windows' else 'python3'
	command = [python_command, '-m', 'unittest', script_path] if is_unittest else [python_command, script_path]
	
	# Get the directory of the script file
	script_dir = os.path.dirname(os.path.abspath(script_path))
	
	# Run the script and capture its output
	process = subprocess.Popen(command, cwd=script_dir, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
	
	# Print the script's output in real time
	while process.poll() is None:
		print(process.stdout.readline(), end='')
		print(process.stderr.readline(), file=sys.stderr, end='')
	
	# If the script failed, raise an error
	if process.returncode != 0:
		raise subprocess.CalledProcessError(process.returncode, command)


# --------------------------------------------

# --------------------------------------------

# sanity tests

# --------------------------------------------

# run python3.11->openjdk tests

print(f'{Fore.MAGENTA}Testing Sanity Python3.11 -> OpenJDK{Fore.RESET} - {Fore.YELLOW}RUNNING{Fore.RESET}')

# Define the paths to the scripts to be run
build_sanity_openjdk_script_path = os.path.join(current_path, 'sanity', 'openjdk', 'build_java_code.py')
test_sanity_openjdk_path = os.path.join(current_path, 'sanity', 'openjdk', 'openjdk_test.py')

# Run the scripts
run_script(build_sanity_openjdk_script_path)
run_script(test_sanity_openjdk_path, is_unittest=True)

print(f'{Fore.MAGENTA}Testing Sanity Python3.11 -> OpenJDK{Fore.RESET} - {Fore.GREEN}PASSED{Fore.RESET}')

# --------------------------------------------

# run python3.11->Go tests

print(f'{Fore.MAGENTA}Testing Sanity Python3.11 -> Go{Fore.RESET} - {Fore.YELLOW}RUNNING{Fore.RESET}')

# Define the paths to the scripts to be run
build_sanity_go_script_path = os.path.join(current_path, 'sanity', 'go', 'build_metaffi.py')
test_sanity_go_path = os.path.join(current_path, 'sanity', 'go', 'go_test.py')

# Run the scripts
run_script(build_sanity_go_script_path)
run_script(test_sanity_go_path, is_unittest=True)

print(f'{Fore.MAGENTA}Testing Sanity Python3.11 -> Go{Fore.RESET} - {Fore.GREEN}PASSED{Fore.RESET}')
os.remove(os.path.join(current_path, 'sanity', 'go', f'TestRuntime_MetaFFIGuest{get_extension_by_platform()}'))

# --------------------------------------------

# extended tests

# --------------------------------------------

# run python3.11->OpenJDK tests

print(f'{Fore.MAGENTA}Testing Extended Python3.11 -> OpenJDK{Fore.RESET} - {Fore.YELLOW}RUNNING{Fore.RESET}')

# Define the path to the unittest script
test_extended_openjdk_path = os.path.join(current_path, 'extended', 'openjdk', 'log4j', 'log4j_test.py')

# Run the script
run_script(test_extended_openjdk_path, is_unittest=True)

print(f'{Fore.MAGENTA}Testing Extended Python3.11 -> OpenJDK{Fore.RESET} - {Fore.GREEN}PASSED{Fore.RESET}')

# --------------------------------------------

# run python3.11->Go tests

print(f'{Fore.MAGENTA}Testing Extended Python3.11 -> Go{Fore.RESET} - {Fore.YELLOW}RUNNING{Fore.RESET}')

# Define the paths to the scripts to be run
build_extended_go_script_path = os.path.join(current_path, 'extended', 'go', 'gomcache', 'build_metaffi.py')
test_extended_go_path = os.path.join(current_path, 'extended', 'go', 'gomcache', 'gomcache_test.py')

# Run the scripts
run_script(build_extended_go_script_path)
run_script(test_extended_go_path, is_unittest=True)

print(f'{Fore.MAGENTA}Testing Extended Python3.11 -> Go{Fore.RESET} - {Fore.GREEN}PASSED{Fore.RESET}')
os.remove(os.path.join(current_path, 'extended', 'go', 'gomcache', f'mcache_MetaFFIGuest{get_extension_by_platform()}'))
