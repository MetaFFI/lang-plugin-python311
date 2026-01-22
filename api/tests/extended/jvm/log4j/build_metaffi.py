import platform
import os
import subprocess
import shlex
import sys


def run_command(command: str):
	
	print(f'{command}')
	
	try:
		command_split = shlex.split(os.path.expanduser(os.path.expandvars(command)))
		output = subprocess.run(command_split, capture_output=True, text=True)
	except subprocess.CalledProcessError as e:
		print(f'Failed running "{command}" with exit code {e.returncode}. Output:\n{str(e.stdout)}{str(e.stderr)}')
		exit(1)
	except FileNotFoundError as e:
		print(f'Failed running {command} with {e.strerror}.\nfile: {e.filename}')
		exit(1)
	
	if output.returncode != 0:
		raise Exception(f'Failed to run {command}. Exit code: {output.returncode}. Output:\n{str(output.stdout).strip()}{str(output.stderr).strip()}')
	
	all_stdout = output.stdout
	
	# if the return code is not zero, raise an exception
	return str(all_stdout).strip()


def main():
	os.chdir(os.path.dirname(os.path.abspath(__file__)))
	
	try:
		run_command(f'metaffi -c --idl-plugin jvm --idl log4j-api-2.21.1.jar;log4j-core-2.21.1.jar -h python3')
		print("Successfully generated Python code from log4j JAR files using JVM IDL plugin")
	except Exception as e:
		print(f"Error: Failed to generate Python code: {e}")
		print("   This indicates a problem with the JVM IDL plugin or Python311 host compiler.")
		print("   Please check the IDL plugin and host compiler implementations.")
		exit(1)


if __name__ == '__main__':
	main() 
