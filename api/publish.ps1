
# Move ./tests directory to parent directory
Move-Item -Path .\tests -Destination ..

# Publish to pypi
flit publish --repository pypi --pypirc C:\Users\green\.pyirc

# Move back the "tests" directory from parent directory to current directory
Move-Item -Path ..\tests -Destination .

# Update metaffi-api pip package
py -m pip install metaffi-api --upgrade