#!/usr/bin/env python3
import os
import json
import zipfile
from glob import glob
from urllib.request import urlretrieve
from urllib.parse import urlparse
import shutil
import subprocess

workingDir = os.getcwd()

projectFile = os.path.join(workingDir, 'project.json')
vendorDir = os.path.join(workingDir, 'vendor')
tempDir = os.path.join(vendorDir, 'tmp')

with open(projectFile, 'r') as projectFile:
    fileContents = projectFile.read()

# Delete vendor directory if exists
if os.path.exists(vendorDir):
    shutil.rmtree(vendorDir)

# Create vendor directory
os.mkdir(path=vendorDir)

# Delete temporary directory if exists
if os.path.exists(tempDir):
    shutil.rmtree(tempDir)

# Create temporary directory
os.mkdir(path=tempDir)

data = json.loads(fileContents);

for x in data['dependencies']:
    print(f'Fetching {x["name"]}...')
    parsedURL = urlparse(x['url'])
    filename = f'{x["name"]}-{os.path.basename(parsedURL.path)}'
    filenameNoExt = os.path.splitext(filename)[0]
    archivePath = os.path.join(tempDir, filename)
    archiveContentPath = os.path.join(tempDir, filenameNoExt)

    x['sourceDir'] = os.path.join(archiveContentPath, x['buildSource'])
    
    urlretrieve(x['url'], archivePath)

    with zipfile.ZipFile(archivePath, 'r') as zipRef:
        zipRef.extractall(archiveContentPath)

    os.unlink(archivePath)

for x in data['dependencies']:
    print(f'Building {x["name"]}...')
    
    for cmdLine in x['cmd']:
        parsedCmdLine = cmdLine.replace('{{VENDOR_DIR}}', vendorDir)
        parsedCmdLine = parsedCmdLine.replace('{{VENDOR_INCLUDE_DIR}}', os.path.join(vendorDir, 'include'))
        parsedCmdLine = parsedCmdLine.replace('{{SOURCE_DIR}}', x['sourceDir'])
        print('\t', parsedCmdLine)

        if parsedCmdLine.startswith('{COPY}'):
            parts = parsedCmdLine.split(' ')
            if len(parts) < 3:
                print('COPY command must have at least two arguments')
                continue

            mainParts = parts[1:len(parts) - 1]
            destination = parts[-1]
            includedFiles = []
            for p in mainParts:
                includedFiles.extend(glob(f'{x["sourceDir"]}/{p}'))

            for file in includedFiles:            
                shutil.copy2(file, destination)
        
        else:
            subprocess.run(parsedCmdLine.split(' '), cwd=x['sourceDir'])

shutil.rmtree(tempDir)
