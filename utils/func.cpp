#include "func.h"

#ifdef _WIN32
#include <conio.h>
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <variant>

#include <windows.h>

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void debug(const std::string &msg) {
    std::cout << msg << std::endl;
}

KeyResult key_catch() {
    while (true) {
        int ch = _getch();
        if (ch >= 32 && ch <= 126) { // 如果 ch 是一个可打印字符的 ASCII 值
            return KeyResult(std::string(1, static_cast<char>(ch)));
        } else {
            if (ch == 0xE0) {
                int ch2 = _getch();
                switch (ch2) {
                    case 72:
                        return KeyResult(SPECIAL_KEY::UP);
                    case 80:
                        return KeyResult(SPECIAL_KEY::DOWN);
                    case 77:
                        return KeyResult(SPECIAL_KEY::RIGHT);
                    case 75:
                        return KeyResult(SPECIAL_KEY::LEFT);
                    default:
                        return KeyResult(SPECIAL_KEY::UNKNOWN);
                }
            } else {
                switch (ch) {
                    case 13:
                        return KeyResult(SPECIAL_KEY::ENTER);
                    case 27:
                        return KeyResult(SPECIAL_KEY::ESC);
                    case 32:
                        return KeyResult(SPECIAL_KEY::SPACE);
                    case 8:
                        return KeyResult(SPECIAL_KEY::BACKSPACE);
                    default:
                        return KeyResult(SPECIAL_KEY::UNKNOWN);
                }
            }
        }
    }
}

#endif

