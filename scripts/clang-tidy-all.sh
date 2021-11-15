#!/bin/sh

if [ -z $1 ]; then
    CMD=clang-tidy
else
    CMD=clang-tidy-$1
fi

find engine/src editor/src -type f -name "*.cpp" -o -name "*.frag" -o -name "*.vert" | xargs -I{} $CMD -header-filter=.* --p=file --quiet {} -- --std=c++17 -isystem./vendor/include -isystem./engine/src -isystem./editor/src/ -isystem./platform-tools/include -isystem/usr/local/include
