import shutil
import subprocess
import time
import os
from typing import final
import re

# Change directory to the current directory of the __file__
current_dir = os.getcwd()
os.chdir(os.path.dirname(os.path.abspath(__file__)))

git_changed = False

try:
	if os.path.exists('./tests') and len(os.listdir('./tests')) > 0 and (not os.path.exists('../tests') or len(os.listdir('../tests')) == 0):
		shutil.rmtree('../tests', ignore_errors=True)
		shutil.move('./tests', '..')

	if os.path.exists('./unittest') and len(os.listdir('./unittest')) > 0 and (not os.path.exists('../unittest') or len(os.listdir('../unittest')) == 0):
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

	git_changed = True

	# * Publish to pypi
	subprocess.run(['flit', 'publish', '--repository', 'pypi', '--pypirc', os.path.expanduser("~")+'/.pyirc'], check=True)

	# wait for pypi to update
	print("waiting 5 seconds for pypi to update")
	time.sleep(5)

	# Update metaffi-api pip package
	subprocess.run(['py', '-m', 'pip', 'install', 'metaffi-api', '--upgrade'], check=True)

	# Change back to the previous current directory
	os.chdir(current_dir)

	print("done updating package")

finally:
	if os.path.exists('./tests') and len(os.listdir('./tests')) == 0:
		shutil.rmtree('./tests')
	shutil.move('../tests', '.')

	if os.path.exists('./unittest') and len(os.listdir('./unittest')) == 0:
		shutil.rmtree('./unittest')

	shutil.move('../unittest', '.')

	if git_changed:
		subprocess.run(['git', 'add', '*'], check=True)
		subprocess.run(['git', 'commit', '-m', '.'], check=True)

	# Change back to the previous current directory
	os.chdir(current_dir)