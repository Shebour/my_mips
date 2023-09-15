#!/bin/sh

gcov -o build/bin src/*.c
gcovr --html-details -o index.html

