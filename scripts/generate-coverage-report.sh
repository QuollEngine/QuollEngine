#!/bin/sh

rm -rf coverage/
mkdir -p coverage/
lcov -c --directory workspace -o coverage/report.info
lcov -r coverage/report.info "*vendor*" "/usr*" "*engine/tests*" "*editor/tests*" -o coverage/report.info

if [ $1 = 'html' ]; then
    genhtml coverage/report.info --output-directory coverage/ --prefix $(pwd)
fi
