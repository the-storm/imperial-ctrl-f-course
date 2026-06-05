#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")"

if [ "${1:-}" = "clean" ]; then
    rm -rf build audio_meta
    exit 0
fi

CXX_COMPILER="${CMAKE_CXX_COMPILER:-}"
if [ -z "$CXX_COMPILER" ]; then
    if command -v afl-clang-fast++ >/dev/null 2>&1; then
        CXX_COMPILER="$(command -v afl-clang-fast++)"
    elif [ -x /AFLplusplus/afl-clang-fast++ ]; then
        CXX_COMPILER="/AFLplusplus/afl-clang-fast++"
    else
        CXX_COMPILER="${CXX:-c++}"
    fi
fi

export AFL_USE_ASAN="${AFL_USE_ASAN:-1}"

mkdir -p build
cmake -S . -B build \
    -DCMAKE_CXX_COMPILER="$CXX_COMPILER" \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_CXX_FLAGS="-g -O1 -fno-omit-frame-pointer -fsanitize=address ${CMAKE_CXX_FLAGS:-}"
cmake --build build --target audio_meta
cp build/audio_meta ./audio_meta
chmod +x ./audio_meta
