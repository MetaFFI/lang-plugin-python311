import shutil
import subprocess
import time
import os
from typing import final
import re

# Change directory to the current directory of the __file__
current_dir = os.getcwd()
os.chdir(os.path.dirname(os.path.abspath(__file__)))

try:
	shutil.move("SConscript_api-python3", '..')

	if os.path.exists('./tests') and (not os.path.exists('../tests') or not os.listdir('../tests')):
		shutil.rmtree('../tests', ignore_errors=True)
		shutil.move('./tests', '..')

	if os.path.exists('./unittest') and (not os.path.exists('../unittest') or not os.listdir('../unittest')):
		shutil.rmtree('../unittest', ignore_errors=True)
		shutil.move('./unittest', '..')

	# * ---- Update the package version ----
	# Read the current version from the file
	with open("./metaffi/__init__.py", "r") as file:
		content = file.read()

	# Extract the current version number using regex
	pattern = r"__version__ = \"(\d+\.\d+\.)(\d+)\""
	match = re.search(pattern, content)
	if match:
		major_minor = match.group(1)
		patch = int(match.group(2))
		new_patch = patch + 1
		new_version = f"{major_minor}{new_patch}"
		# Replace the old version with the new version using regex
		content = re.sub(pattern, f"__version__ = \"{new_version}\"", content)

	# Write the modified content back to the file
	with open("./metaffi/__init__.py", "w") as file:
		file.write(content)
	
	# * Git commit the code for publishing to pypi
	subprocess.run(['git', 'add', '*'], check=True)
	subprocess.run(['git', 'commit', '-m', '.'], check=True)

	# * Publish to pypi
	subprocess.run(['flit', 'publish', '--repository', 'pypi', '--pypirc', os.path.expanduser("~")+'/.pyirc'], check=True)

	# Move back the "tests" directory from parent directory to current directory
	shutil.move('../tests', '.')
	shutil.move('../unittest', '.')

	subprocess.run(['git', 'add', '*'], check=True)
	subprocess.run(['git', 'commit', '-m', '.'], check=True)
	subprocess.run(['git', 'push'], check=True)

	# wait for pypi to update
	print("waiting 5 seconds for pypi to update")
	time.sleep(5)

	# Update metaffi-api pip package
	subprocess.run(['py', '-m', 'pip', 'install', 'metaffi-api', '--upgrade'], check=True)

	# Change back to the previous current directory
	os.chdir(current_dir)

	print("done updating package")

finally:
	shutil.move("../SConscript_api-python3", '.')

	# Change back to the previous current directory
	os.chdir(current_dir)