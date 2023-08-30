from pathlib import Path
import re
import sys
import os

FA_HEADER_FILE = Path(os.path.dirname(__file__), '..', 'editor', 'src', 'liquidator', 'ui', 'FontAwesome.h')

FORMAT_PATTERN_FA_VAR = '\$fa-var-(\S+)\: (\S+);'
FORMAT_PATTERN_ICON = '\"(\S+)\"\:'

fontPath = sys.argv[1]

if not fontPath:
    print("You must provide a font path to fontawesome variables SASS file")

iconVars = {}
icons = []

fontLookup = False

with open(fontPath, 'r', encoding = 'utf-8') as f:
    contents = f.read()
    lines = contents.splitlines()
    for line in lines:
        matches = re.search(FORMAT_PATTERN_FA_VAR, line)
        if matches is not None:
            name = matches.group(1)
            if name.isdigit():
                continue

            iconVars[name] = matches.group(2)

        if line.startswith('$fa-icons'):
            fontLookup = True
            continue

        if line.startswith(')'):
            fontLookup = False

        if fontLookup:
            matches = re.search(FORMAT_PATTERN_ICON, line)
            if matches is not None:
                name = matches.group(1)
                if name not in iconVars:
                    continue

                code = iconVars[name]
                icons.append((name, code))

def convertKebabCaseToPascalCase(str):
    parts = str.split('-')
    return ''.join([f'{x[0].upper()}{x[1:]}' for x in parts])

def convertUnicodeToCppCode(code):
    HEX_REPR = '\\x'
    hexCode = chr(int(code[1:], 16)).encode('utf-8').hex('_')

    return f'{HEX_REPR}{hexCode.replace("_", HEX_REPR)}'

lines = []

for (name, code) in icons:
    varName = convertKebabCaseToPascalCase(name)
    cppCode = convertUnicodeToCppCode(code)
    lines.append(f'static constexpr const char *{varName} = "{cppCode}";')

fileContents = """#pragma once

namespace quollator::fa {

$ICONS

} // namespace quollator::fa
""".replace('$ICONS', '\n'.join(lines))

with open(FA_HEADER_FILE, 'w') as f:
    f.write(fileContents)

print(f'{FA_HEADER_FILE} generated! Run clang-format to format the document!')
