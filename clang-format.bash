#!/usr/bin/env bash
find ./include ./src ./samples -type f \( -iname \*.cpp -o -iname \*.hpp \) | xargs clang-format -style=llvm -i
find ./test -type f \( -iname \*.cpp \) | xargs clang-format -style=llvm -i
