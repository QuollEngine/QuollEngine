#!/bin/sh

rm -rf coverage/
mkdir -p coverage/
lcov -c --directory workspace -o coverage/report.info --gcov-tool $(pwd)/scripts/llvm-cov.sh
lcov -r coverage/report.info "*vendor*" "*vcpkg_installed*" "/usr*" "*engine/tests*" "*editor/tests*" -o coverage/report.info

if [ $1 = 'html' ]; then
    genhtml coverage/report.info --output-directory coverage/ --prefix $(pwd)
fi
