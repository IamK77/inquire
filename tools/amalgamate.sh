#!/usr/bin/env bash
# Amalgamate all sources into a single-header library at include/inquire.hpp.
# Usage:  bash tools/amalgamate.sh
# Users:  #include "inquire.hpp"
#         in EXACTLY ONE TU define INQUIRE_IMPLEMENTATION first.

set -euo pipefail
cd "$(dirname "$0")/.."

OUT=include/inquire.hpp
mkdir -p include

strip() {
    # remove internal includes (indented or not), pragma once
    sed -E -e '/^[[:space:]]*#include "[^>]+"[[:space:]]*$/d' \
           -e '/^[[:space:]]*#pragma once[[:space:]]*$/d' "$1"
}

HEADERS=(
    src/Inquire/utils/types.h
    src/Inquire/utils/colorful.hpp
    src/Inquire/utils/console.hpp
    src/Inquire/utils/encode.hpp
    src/Inquire/utils/func.h
    src/Inquire/utils/renderer.hpp
    src/Inquire/utils/tty_guard.hpp
    src/Inquire/error.hpp
    src/Inquire/validator.hpp
    src/Inquire/inquire.hpp
)

SOURCES=(
    src/Inquire/utils/colorful.cpp
    src/Inquire/utils/console.cpp
    src/Inquire/utils/encode.cpp
    src/Inquire/utils/func.cpp
    src/Inquire/utils/renderer.cpp
    src/Inquire/utils/tty_guard.cpp
    src/Inquire/inquire.cpp
)

{
cat <<'BANNER'
// =========================================================================
// Inquire — single-header interactive CLI library for C++11
//
//   #include "inquire.hpp"
// In EXACTLY ONE translation unit, do:
//   #define INQUIRE_IMPLEMENTATION
//   #include "inquire.hpp"
//
// Project:  https://github.com/IamK77/inquire
// License:  MIT
// =========================================================================

#ifndef INQUIRE_HPP_
#define INQUIRE_HPP_

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <functional>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
    #include <io.h>
#endif

#ifdef __linux__
    #include <cerrno>
    #include <csignal>
    #include <fcntl.h>
    #include <sys/ioctl.h>
    #include <termios.h>
    #include <unistd.h>
#endif

BANNER

echo "// ----- declarations -----"
for h in "${HEADERS[@]}"; do
    echo
    echo "// ===== $h ====="
    strip "$h"
done

echo
echo "#endif // INQUIRE_HPP_"
echo
echo "#ifdef INQUIRE_IMPLEMENTATION"
echo "#ifndef INQUIRE_IMPL_DEFINED_"
echo "#define INQUIRE_IMPL_DEFINED_"
echo

for s in "${SOURCES[@]}"; do
    echo
    echo "// ===== $s ====="
    strip "$s"
done

echo
echo "#endif // INQUIRE_IMPL_DEFINED_"
echo "#endif // INQUIRE_IMPLEMENTATION"
} > "$OUT"

echo "wrote $OUT ($(wc -l < "$OUT") lines)"
