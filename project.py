#!/usr/bin/env python3
import os
import json
import zipfile
import platform
import shlex
import sys
import time
import stat
from glob import glob
from urllib.request import urlretrieve
from urllib.parse import urlparse
import shutil
import subprocess
import hashlib
import argparse
from pathlib import Path

workingDir = os.getcwd()

projectFile = os.path.join(workingDir, 'project.json')
vendorRootDir = os.path.join(workingDir, 'vendor')
tempDir = os.path.join(vendorRootDir, 'tmp')
projectHashFile = os.path.join(vendorRootDir, 'project_hash')

buildModes = ['Debug', 'Release']

profiler_data = {}

parser = argparse.ArgumentParser(description='Liquid project dependency manager')

parser.add_argument('-m', '--mode', nargs='+', help='Mode', choices=['debug', 'release'], default=['debug', 'release'])
parser.add_argument('-p', '--packages', help='Specify which package to install', nargs='+', default=None)

args = parser.parse_args()

buildModes = [x.capitalize() for x in args.mode]
projectNames = args.packages

#
# Error handler for rmtree
#
# Needed to delete readonly files on Windows
#
def on_rm_error( func, path, exc_info):
    # path contains the path of the file that couldn't be removed
    # let's just assume that it's read-only and unlink it.
    os.chmod( path, stat.S_IWRITE )
    os.unlink( path )

#
# Deletes existing directory and
# creates a new one
#
def clean_make_dir(path):
    if os.path.exists(path):
        shutil.rmtree(path, onerror=on_rm_error)
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
        x['archivePath'] = os.path.join(tempDir, filename)
        x['archiveContentPath'] = os.path.join(tempDir, x['name'])
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
def fetch_dependencies(dependencies):
    for x in dependencies:
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
def cmd_copy(cmdLine, x):
    parts = shlex.split(cmdLine)
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
# Command to remove files
#
def cmd_rm(cmdLine, x):
    parts = shlex.split(cmdLine)
    if len(parts) < 2:
        print('RM command must have at least one argument')
        return

    mainParts = parts[1:len(parts)]
    includedFiles = []
    for p in mainParts:
        includedFiles.extend(glob(f'{x["sourceDir"]}/{p}'))

    includedFiles = [os.path.normpath(p) for p in includedFiles]
    for file in includedFiles:
        if os.path.isdir(file):
            shutil.rmtree(file, onerror=on_rm_error)
        elif os.path.isfile(file):
            os.remove(file)
        else:
            print(f'[COPY] Path is not a file or a directory')

#
# Command to make directory
#
def cmd_mkdir(cmdLine, x):
    parts = shlex.split(cmdLine)
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
        'VENDOR_ROOT_DIR': params['VENDOR_ROOT_DIR'],
        'VENDOR_INSTALL_DIR': params['VENDOR_INSTALL_DIR'],
        'VENDOR_PROJECTS_DIR': params['VENDOR_PROJECTS_DIR'],
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

project = open_project(projectFile)
dependencies = project['dependencies']
cleanAll = True

if projectNames is not None:
    dependencies = [x for x in dependencies if x['name'] in projectNames]
    cleanAll = False
elif project_hash_matches(projectFile, projectHashFile):
    print('Dependencies are up to date!')
    sys.exit(0)

if cleanAll:
    clean_make_dir(vendorRootDir)

    for buildMode in buildModes:
        vendorModeDir = os.path.join(vendorRootDir, buildMode)
        clean_make_dir(vendorModeDir)
        clean_make_dir(os.path.join(vendorModeDir, 'include'))
        clean_make_dir(os.path.join(vendorModeDir, 'lib'))
        clean_make_dir(os.path.join(vendorModeDir, 'bin'))

    clean_make_dir(os.path.join(vendorRootDir, 'projects'))

clean_make_dir(os.path.join(tempDir))
fetch_dependencies(dependencies)

for buildMode in buildModes:
    for x in dependencies: 
        print(f'Building {x["name"]}...')
        start = time.time()
        params = {
            'VENDOR_ROOT_DIR': vendorRootDir,
            'VENDOR_INSTALL_DIR': os.path.join(vendorRootDir, buildMode),
            'VENDOR_PROJECTS_DIR': os.path.join(vendorRootDir, 'projects'),
            'SOURCE_DIR': x['sourceDir'],
            'BUILD_MODE': buildMode
        }
        
        for cmdLine in x['cmd']:
            if isinstance(cmdLine, str):
                parsedCmdLine = parse_placeholders(cmdLine, params)
                printedCmdLine = parse_placeholders(cmdLine, {
                    'VENDOR_ROOT_DIR': 'vendor/',
                    'VENDOR_INSTALL_DIR': f'vendor/{buildMode}',
                    'VENDOR_PROJECTS_DIR': 'vendor/projects',
                    'SOURCE_DIR': f'{x["name"]}/',
                    'BUILD_MODE': buildMode
                })
                print('\t', printedCmdLine)

                if parsedCmdLine.startswith('{COPY}'):
                    cmd_copy(parsedCmdLine, x)
                elif parsedCmdLine.startswith('{MKDIR}'):
                    cmd_mkdir(parsedCmdLine, x)
                elif parsedCmdLine.startswith('{RM}'):
                    cmd_rm(parsedCmdLine, x)
                else:
                    run_process(parsedCmdLine, x['sourceDir'])
            elif cmdLine['type'] == 'cmake':
                print('CMake build')
                options = {
                    'CMAKE_INSTALL_PREFIX': parse_placeholders('{{VENDOR_INSTALL_DIR}}', params)
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

shutil.rmtree(tempDir, onerror=on_rm_error)

print(f'{"Name":<14} {"Fetch":<8} {"Build (Debug)":<12} {"Build (Release)":<12}')
for k, v in profiler_data.items():
    fixed_fetch = f'{v["fetch"]:.4f}'
    fixed_build_debug = f'{v["build-Debug"]:.4f}' if 'build-Debug' in v else 0
    fixed_build_release = f'{v["build-Release"]:.4f}' if 'build-Release' in v else 0

    print(f'{k:<14} {fixed_fetch:<8} {fixed_build_debug:<12} {fixed_build_release:<12}')

create_project_hash_file(projectFile, projectHashFile)
