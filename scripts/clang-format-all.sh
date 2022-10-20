#!/bin/sh

if [ -z $1 ]; then
    CMD=clang-format
else
    CMD=clang-format-$1
fi

if [ "$2" = "verify" ]; then
    ARGS="-n -Werror"
else
    ARGS="-i"
fi

find engine editor runtime -type f -name "*.h" -o -name "*.cpp" -o -name "*.frag" -o -name "*.vert" | xargs $CMD -style=file $ARGS

exit $?
