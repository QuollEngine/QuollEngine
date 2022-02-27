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
from pathlib import Path

workingDir = os.getcwd()

projectFile = os.path.join(workingDir, 'project.json')
vendorDir = os.path.join(workingDir, 'vendor')
tempDir = os.path.join(vendorDir, 'tmp')
buildMode = 'Debug'

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
            shutil.copytree(file, destination)
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
        os.makedirs(directory)


#
# Parse placeholders
#
def parse_placeholders(cmdLine, params):
    placeholders = {
        'VENDOR_DIR': params['VENDOR_DIR'],
        'SOURCE_DIR': params['SOURCE_DIR'],
        'BUILD_MODE': buildMode,
        'PLATFORM': platform.system().lower(),
        'OS_NAME': os.name,
        'ARCHITECTURE': platform.machine()
    }

    line = cmdLine
    for k, v in placeholders.items():
        line = line.replace('{{' + k + '}}', v)

    return Path(line).as_posix()

def run_process(cmdLine, cwd):
    subprocess.run(shlex.split(cmdLine), shell=platform.system() == 'windows', cwd=cwd).check_returncode()

project = open_project(projectFile)
clean_make_dir(vendorDir)
clean_make_dir(os.path.join(tempDir))
clean_make_dir(os.path.join(vendorDir, 'include'))
clean_make_dir(os.path.join(vendorDir, 'projects'))
clean_make_dir(os.path.join(vendorDir, 'lib'))
clean_make_dir(os.path.join(vendorDir, 'bin'))
fetch_dependencies(project)

for x in project['dependencies']:
    print(f'Building {x["name"]}...')
    start = time.time()
    
    for cmdLine in x['cmd']:
        params = {
            'VENDOR_DIR': vendorDir,
            'SOURCE_DIR': x['sourceDir']
        }
        if isinstance(cmdLine, str):
            parsedCmdLine = parse_placeholders(cmdLine, params)
            printedCmdLine = parse_placeholders(cmdLine, {
                'VENDOR_DIR': 'vendor/',
                'SOURCE_DIR': f'{x["name"]}/'
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
            options = {}
            for k, v in cmdLine['options']['common'].items():
                options[k] = parse_placeholders(v, params)

            if sys.platform == 'linux' and 'linux' in cmdLine['options']:
                for k, v in cmdLine['options']['linux'].items():
                    options[k] = parse_placeholders(v, params)
            elif sys.platform == 'win32' and 'windows' in cmdLine['options']:
                for k, v in cmdLine['options']['windows'].items():
                    options[k] = parse_placeholders(v, params)
            elif sys.platform == 'darwin' and 'macos' in cmdLine['options']:
                for k, v in cmdLine['options']['macos'].items():
                    options[k] = parse_placeholders(v, params)

            print("The following options are being used")
            for k, v in options.items():
                print(f'{k:<40} {v}')

            cmdOptions = ''
            for k, v in options.items():
                cmdOptions = f'{cmdOptions} -D{k}={v}'

            run_process(f'cmake {cmdLine["source"]} -B build {cmdOptions}', x['sourceDir'])
            run_process(f'cmake --build build --config {buildMode}', x['sourceDir'])
            run_process(f'cmake --install build --config {buildMode}', x['sourceDir'])

    profiler_data[x['name']]['build'] = time.time() - start

shutil.rmtree(tempDir)

profiler_table = [[k, v['fetch'], v['build']] for k,v in profiler_data.items()]

print(f'{"Name":<14} {"Fetch":<8} {"Build":<8}')
for k, v in profiler_data.items():
    fixed_fetch = f'{v["fetch"]:.4f}'
    fixed_build = f'{v["build"]:.4f}'

    print(f'{k:<14} {fixed_fetch:<8} {fixed_build:<8}')