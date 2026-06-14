#!/bin/sh

rm -rf coverage
mkdir coverage

make clean
make coverage
make test

python3 test_suite.py
gcov -o build/coverage src/*.c
gcovr --html-details -o coverage/index.html

firefox coverage/index.html