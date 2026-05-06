#include "encode.hpp"
#include "console.hpp"

#ifdef _WIN32
    #include <windows.h>
#endif

namespace Inquire {

void ToUTF8() {
    Terminal::init();
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

void ToGBK() {
#ifdef _WIN32
    SetConsoleOutputCP(936);
    SetConsoleCP(936);
#endif
}

}
