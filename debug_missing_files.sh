#!/bin/bash
# Debug script to identify missing source files

echo "=== Project Structure Analysis ==="
echo "Current directory: $(pwd)"
echo

echo "=== Looking for CMakeLists.txt ==="
find . -name "CMakeLists.txt" -type f

echo
echo "=== Checking src/os/ directory ==="
if [ -d "src/os" ]; then
    echo "src/os directory exists:"
    ls -la src/os/
else
    echo "src/os directory does NOT exist"
fi

echo
echo "=== Looking for any os_unix files ==="
find . -name "*os_unix*" -type f

echo
echo "=== Looking for similar OS-related files ==="
find . -name "*unix*" -o -name "*os*" -type f

echo
echo "=== Checking CMakeLists.txt content around line 25 ==="
if [ -f "CMakeLists.txt" ]; then
    echo "Lines 20-30 of CMakeLists.txt:"
    sed -n '20,30p' CMakeLists.txt
else
    echo "CMakeLists.txt not found in current directory"
fi

echo
echo "=== All .c files in project ==="
find . -name "*.c" -type f

echo
echo "=== All .cpp files in project ==="
find . -name "*.cpp" -type f