import shutil
import subprocess
import time
import os
from typing import final

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

	subprocess.run(['git', 'add', '*'], check=True)
	subprocess.run(['git', 'commit', '-m', '.'], check=True)

	# Publish to pypi
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