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
from urllib.request import urlretrieve, Request, urlopen
from urllib.parse import urlparse
import shutil
import subprocess
import hashlib
import argparse
from pathlib import Path

workingDir = os.getcwd()

projectFile = os.path.join(workingDir, 'project.json')
vendorRootDir = os.path.join(workingDir, 'vendor')
archivesDir = os.path.join(vendorRootDir, 'archives')
tempDir = os.path.join(vendorRootDir, 'tmp')
projectHashFile = os.path.join(vendorRootDir, 'project_hash')

buildModes = ['Debug', 'Release']

profiler_data = {}

parser = argparse.ArgumentParser(description='Quoll project dependency manager')

parser.add_argument('-m', '--mode', nargs='+', help='Mode', choices=['debug', 'release'], default=['debug', 'release'])
parser.add_argument('-p', '--packages', help='Specify which package to install', nargs='+', default=None)
parser.add_argument('--build', action=argparse.BooleanOptionalAction, default=True)
parser.add_argument('--fetch', action=argparse.BooleanOptionalAction, default=True)

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
def open_project(path, projectNames):
    with open(path, 'r') as projectFile:
        fileContents = projectFile.read()

    project = json.loads(fileContents);

    if projectNames is not None:
        project['dependencies'] = [x for x in project['dependencies'] if x['name'] in projectNames]

    for x in project['dependencies']:
        x['archivePath'] = os.path.join(archivesDir, x['name'])
        x['archiveContentPath'] = os.path.join(tempDir, x['name'])
        x['sourceDir'] = os.path.normpath(os.path.join(x['archiveContentPath'], x['buildSource']))
        profiler_data[x['name']] = { 'fetch': 0, 'build': 0 }

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
# Check if dependency alread fetched
#
def is_dependency_fetched(dependency):
    pathEtag = Path(dependency['archivePath']).with_suffix('.etag')
    if not os.path.exists(dependency['archivePath']) or not os.path.exists(pathEtag):
       return False

    remoteEtag = None
    request = Request(dependency['url'], method='HEAD')
    try:
        with urlopen(request) as response:
            remoteEtag = response.headers['ETag'].strip('"')
    except:
        pass

    localEtag = None
    try:
        with open(pathEtag) as f:
            localEtag = f.read()
    except:
        pass

    if remoteEtag == None:
        return False
    
    return remoteEtag == localEtag

#
# Fetches dependencies of a project
#
def fetch_dependencies(dependencies):
    for x in dependencies:
        print(f'Fetching {x["name"]}...', end=' ')
        if not is_dependency_fetched(x):
            start = time.time()
            (_, headers) = urlretrieve(x['url'], x['archivePath'])
            profiler_data[x['name']]['fetch'] = time.time() - start

            print('Downloaded')

            etag = headers['ETag'].strip('"')
            pathEtag = Path(x['archivePath']).with_suffix('.etag')

            with open(pathEtag, 'w') as f:
                f.write(etag)
        else:
            print('Local used')


#
# Extracts dependencies of a project
#
def extract_dependencies(dependencies):
    for x in dependencies:
        with zipfile.ZipFile(x['archivePath'], 'r') as zipRef:
            for info in zipRef.infolist():
                extracted_path = zipRef.extract(info, x['archiveContentPath'])

                # Unix System == 3
                if info.create_system == 3:
                    unix_attributes = info.external_attr >> 16
                    if unix_attributes:
                        os.chmod(extracted_path, unix_attributes)        
     

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
    placeholders = params | {
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

project = open_project(projectFile, projectNames)
dependencies = project['dependencies']
cleanAll = True

if projectNames is not None:
    cleanAll = False

if not os.path.exists(vendorRootDir):
    os.mkdir(path=vendorRootDir)

if not os.path.exists(archivesDir):
    os.mkdir(path=archivesDir)

if args.fetch:
    print('Fetching dependencies')
    fetch_dependencies(dependencies)

if args.build:
    print('Building dependencies')

    clean_make_dir(tempDir)
    extract_dependencies(dependencies)

    for buildMode in buildModes:
        if project_hash_matches(projectFile, projectHashFile + '.' + buildMode.lower()):
            print('Dependencies already built for', buildMode)
            continue

        if cleanAll:
            clean_make_dir(os.path.join(vendorRootDir, 'projects'))
            vendorModeDir = os.path.join(vendorRootDir, buildMode)
            clean_make_dir(vendorModeDir)
            clean_make_dir(os.path.join(vendorModeDir, 'include'))
            clean_make_dir(os.path.join(vendorModeDir, 'lib'))
            clean_make_dir(os.path.join(vendorModeDir, 'bin'))

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
                    params = params | { 'CMAKE_IS_DEBUG': 'ON' if buildMode == 'Debug' else 'OFF' }
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
        create_project_hash_file(projectFile, projectHashFile + '.' + buildMode.lower())

    shutil.rmtree(tempDir, onerror=on_rm_error)

print(f'{"Name":<14} {"Fetch":<8} {"Build (Debug)":<12} {"Build (Release)":<12}')
for k, v in profiler_data.items():
    fixed_fetch = f'{v["fetch"]:.4f}'
    fixed_build_debug = f'{v["build-Debug"]:.4f}' if 'build-Debug' in v else 0
    fixed_build_release = f'{v["build-Release"]:.4f}' if 'build-Release' in v else 0

    print(f'{k:<14} {fixed_fetch:<8} {fixed_build_debug:<12} {fixed_build_release:<12}')

