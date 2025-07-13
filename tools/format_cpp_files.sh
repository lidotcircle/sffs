#!/bin/bash

# exclude build directory and third_party directory
find include test benchmark -name "*.cpp" -o -name "*.h" -o -name "*.c" -o -name "*.hpp" -o -name "*.hxx" | xargs clang-format -i
