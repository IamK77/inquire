#include "encode.hpp"

#ifdef _WIN32

void ToUTF8() {
    SetConsoleOutputCP(65001);
}

void ToGBK() {
    SetConsoleOutputCP(936);
}

#endif