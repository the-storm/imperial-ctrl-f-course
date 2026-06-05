#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")"

if [ "${1:-}" = "clean" ]; then
    rm -rf build simple_crash
    exit 0
fi

CXX_COMPILER="${CMAKE_CXX_COMPILER:-}"
if [ -z "$CXX_COMPILER" ]; then
    if command -v clang++ >/dev/null 2>&1; then
        CXX_COMPILER="$(command -v clang++)"
    else
        CXX_COMPILER="${CXX:-c++}"
    fi
fi

mkdir -p build
cmake -S . -B build \
    -DCMAKE_CXX_COMPILER="$CXX_COMPILER" \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_CXX_FLAGS="-g -O1 ${CMAKE_CXX_FLAGS:-}"
cmake --build build --target simple_crash
cp build/simple_crash ./simple_crash
chmod +x ./simple_crash
