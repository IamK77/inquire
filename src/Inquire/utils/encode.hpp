#pragma once

#ifdef _WIN32

#include <windows.h>

void ToUTF8();

void ToGBK();

#endif

#ifdef __linux__

void ToUTF8();

void ToGBK();

#endif
