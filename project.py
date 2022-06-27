#!/usr/bin/env python3
import os
import json
import zipfile
import platform
import shlex
import sys
import time
from glob import glob
from urllib.request import urlretrieve
from urllib.parse import urlparse
import shutil
import subprocess
import hashlib
from pathlib import Path

workingDir = os.getcwd()

projectFile = os.path.join(workingDir, 'project.json')
vendorDir = os.path.join(workingDir, 'vendor')
tempDir = os.path.join(vendorDir, 'tmp')
projectHashFile = os.path.join(vendorDir, 'project_hash')
buildModes = ['Debug', 'Release']

profiler_data = {}

#
# Deletes existing directory and
# creates a new one
#
def clean_make_dir(path):
    if os.path.exists(path):
        shutil.rmtree(path)
    os.mkdir(path=path)

#
# Opens project JSON file, loads it, and parses it
#
def open_project(path):
    with open(path, 'r') as projectFile:
        fileContents = projectFile.read()

    project = json.loads(fileContents);

    for x in project['dependencies']:
        parsedURL = urlparse(x['url'])
        filename = f'{x["name"]}-{os.path.basename(parsedURL.path)}'
        filenameNoExt = os.path.splitext(filename)[0]
        x['archivePath'] = os.path.join(tempDir, filename)
        x['archiveContentPath'] = os.path.join(tempDir, filenameNoExt)
        x['sourceDir'] = os.path.normpath(os.path.join(x['archiveContentPath'], x['buildSource']))

    return project

#
# Creates sha384 hash of the project json file
#
def get_project_hash(projectFilePath):
    with open(projectFilePath, 'r') as projectFile:
        fileContents = projectFile.read()
        return hashlib.sha384(fileContents.encode('utf-8')).hexdigest()

#
# Create project hash file 
#
def create_project_hash_file(projectFilePath, hashFilePath):
    hash = get_project_hash(projectFilePath)
    with open(hashFilePath, 'w') as hashFile:
        hashFile.write(hash)

#
# Check if hashes match
#
def project_hash_matches(projectFilePath, hashFilePath):
    hash = get_project_hash(projectFilePath)

    try:
        with open(hashFilePath, 'r') as hashFile:
            fileContents = hashFile.read()
            return fileContents == hash
    except:
        return False

#
# Fetches and extracts dependencies of a project
#
def fetch_dependencies(project):
    for x in project['dependencies']:
        profiler_data[x['name']] = { 'fetch': 0, 'build': 0 }

        print(f'Fetching {x["name"]}...')
        start = time.time()
        urlretrieve(x['url'], x['archivePath'])
        with zipfile.ZipFile(x['archivePath'], 'r') as zipRef:
            for info in zipRef.infolist():
                extracted_path = zipRef.extract(info, x['archiveContentPath'])

                # Unix System == 3
                if info.create_system == 3:
                    unix_attributes = info.external_attr >> 16
                    if unix_attributes:
                        os.chmod(extracted_path, unix_attributes)

        os.unlink(x['archivePath'])
        profiler_data[x['name']]['fetch'] = time.time() - start

#
# Command to copy files to directory
#
def cmd_copy(cmdLine):
    parts = cmdLine.split(' ')
    if len(parts) < 3:
        print('COPY command must have at least two arguments')
        return

    mainParts = parts[1:len(parts) - 1]
    destination = os.path.normpath(parts[-1])
    includedFiles = []
    for p in mainParts:
        includedFiles.extend(glob(f'{x["sourceDir"]}/{p}'))

    includedFiles = [os.path.normpath(p) for p in includedFiles]
    for file in includedFiles:
        if os.path.isdir(file):
            shutil.copytree(file, destination, dirs_exist_ok=True)
        elif os.path.isfile(file):       
            shutil.copy2(file, destination)
        else:
            print(f'[COPY] Path is not a file or a directory')

#
# Command to make directory
#
def cmd_mkdir(cmdLine):
    parts = cmdLine.split(' ')
    if len(parts) < 2:
        print('MKDIR command must have at least one argument')
        return
    
    mainParts = parts[1:len(parts)]
    includedDirs = [os.path.normpath(p) for p in mainParts]
    for directory in includedDirs:
        os.makedirs(directory, exist_ok=True)


#
# Parse placeholders
#
def parse_placeholders(cmdLine, params):
    placeholders = {
        'VENDOR_DIR': params['VENDOR_DIR'],
        'SOURCE_DIR': params['SOURCE_DIR'],
        'BUILD_MODE': params['BUILD_MODE'],
        'PLATFORM': platform.system().lower(),
        'OS_NAME': os.name,
        'ARCHITECTURE': platform.machine()
    }

    line = cmdLine
    for k, v in placeholders.items():
        line = line.replace('{{' + k + '}}', v)

    return Path(line).as_posix()

#
# Runs process by splitting
# it the command line into an array
#
def run_process(cmdLine, cwd):
    subprocess.run(shlex.split(cmdLine), shell=platform.system() == 'windows', cwd=cwd).check_returncode()

#
# Check if object is a dictionary
#
def is_dict(obj, key):
    return key in obj and type(obj[key]) is dict

if project_hash_matches(projectFile, projectHashFile):
    print('Dependencies are up to date!')
    sys.exit(0)

project = open_project(projectFile)
clean_make_dir(vendorDir)
clean_make_dir(os.path.join(tempDir))
clean_make_dir(os.path.join(vendorDir, 'include'))
clean_make_dir(os.path.join(vendorDir, 'projects'))
clean_make_dir(os.path.join(vendorDir, 'lib'))
clean_make_dir(os.path.join(vendorDir, 'bin'))
create_project_hash_file(projectFile, projectHashFile)
fetch_dependencies(project)

for buildMode in buildModes:
    for x in project['dependencies']:
        print(f'Building {x["name"]}...')
        start = time.time()
        
        for cmdLine in x['cmd']:
            params = {
                'VENDOR_DIR': vendorDir,
                'SOURCE_DIR': x['sourceDir'],
                'BUILD_MODE': buildMode
            }
            if isinstance(cmdLine, str):
                parsedCmdLine = parse_placeholders(cmdLine, params)
                printedCmdLine = parse_placeholders(cmdLine, {
                    'VENDOR_DIR': 'vendor/',
                    'SOURCE_DIR': f'{x["name"]}/',
                    'BUILD_MODE': buildMode
                })
                print('\t', printedCmdLine)

                if parsedCmdLine.startswith('{COPY}'):
                    cmd_copy(parsedCmdLine)
                elif parsedCmdLine.startswith('{MKDIR}'):
                    cmd_mkdir(parsedCmdLine)
                else:
                    run_process(parsedCmdLine, x['sourceDir'])
            elif cmdLine['type'] == 'cmake':
                print('CMake build')
                options = {
                    'CMAKE_INSTALL_PREFIX': parse_placeholders('{{VENDOR_DIR}}', params)
                }
                source = '.'
                if 'source' in cmdLine:
                    source = cmdLine['source']

                if is_dict(cmdLine, 'options'):
                    if is_dict(cmdLine['options'], 'common'):
                        for k, v in cmdLine['options']['common'].items():
                            options[k] = parse_placeholders(v, params)

                    if sys.platform == 'linux' and is_dict(cmdLine['options'], 'linux'):
                        for k, v in cmdLine['options']['linux'].items():
                            options[k] = parse_placeholders(v, params)
                    elif sys.platform == 'win32' and is_dict(cmdLine['options'], 'windows'):
                        for k, v in cmdLine['options']['windows'].items():
                            options[k] = parse_placeholders(v, params)
                    elif sys.platform == 'darwin' and is_dict(cmdLine['options'], 'macos'):
                        for k, v in cmdLine['options']['macos'].items():
                            options[k] = parse_placeholders(v, params)

                print("The following options are being used")
                for k, v in options.items():
                    print(f'{k:<40} {v}')

                cmdOptions = ''
                for k, v in options.items():
                    cmdOptions = f'{cmdOptions} -D{k}={v}'

                run_process(f'cmake {source} -B build {cmdOptions}', x['sourceDir'])
                run_process(f'cmake --build build --config {buildMode}', x['sourceDir'])
                run_process(f'cmake --install build --config {buildMode}', x['sourceDir'])

        profiler_data[x['name']][f'build-{buildMode}'] = time.time() - start

shutil.rmtree(tempDir)

print(f'{"Name":<14} {"Fetch":<8} {"Build (Debug)":<12} {"Build (Release)":<12}')
for k, v in profiler_data.items():
    fixed_fetch = f'{v["fetch"]:.4f}'
    fixed_build_debug = f'{v["build-Debug"]:.4f}'
    fixed_build_release = f'{v["build-Release"]:.4f}'

    print(f'{k:<14} {fixed_fetch:<8} {fixed_build_debug:<12} {fixed_build_release:<12}')
