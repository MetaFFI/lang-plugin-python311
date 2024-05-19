
# Move ./tests directory to parent directory
Move-Item -Path .\tests -Destination ..
Move-Item -Path .\unittest -Destination ..

git add *; git commit -m ".";

# Publish to pypi
flit publish --repository pypi --pypirc C:\Users\green\.pyirc

# Move back the "tests" directory from parent directory to current directory
Move-Item -Path ..\tests -Destination .
Move-Item -Path ..\unittest -Destination .

git add *; git commit -m "."; git push;

# wait for pypi to update
Write-Host "waiting 15 seconds for pypi to update"
Start-Sleep -Seconds 15

# Update metaffi-api pip package
py -m pip install metaffi-api --upgrade

Write-Host "done"