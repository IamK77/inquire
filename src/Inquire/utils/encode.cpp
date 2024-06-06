#include "encode.hpp"

#ifdef _WIN32

void ToUTF8() {
    SetConsoleOutputCP(65001);
}

void ToGBK() {
    SetConsoleOutputCP(936);
}

#endif


#ifdef __linux__

void ToUTF8() {
    // do nothing
}

void ToGBK() {
    // do nothing
}

#endif