#include "types.h"
#include <iostream>
#include <vector>
#include <string>

#ifdef _WIN32

#include <windows.h>

extern HANDLE hConsole ;

#endif


KeyResult key_catch();
// void clean(const std::size_t &size, std::function<void()> func);
// void reflush_input(const std::string &input, bool is_backspace = false);
// void reflush_result(const std::size_t &size_option, std::string &input);

