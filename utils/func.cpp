#include "func.h"

#include <iostream>
#include <vector>
#include <string>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>

// HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void debug(const std::string &msg) {
    std::cout << msg << std::endl;
}

#endif

#ifdef __linux__

#include <termios.h>
#include <unistd.h>

int getch() {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}


#endif

KeyResult key_catch() {
    while (true) {
        // int ch = _getch();
        // if (ch >= 32 && ch <= 126) { // 如果 ch 是一个可打印字符的 ASCII 值
        //     return KeyResult(std::string(1, static_cast<char>(ch)));
        // } else {
        //     if (ch == 0xE0) {
        //         int ch2 = _getch();
        //         switch (ch2) {
        //             case 72:
        //                 return KeyResult(SPECIAL_KEY::UP);
        //             case 80:
        //                 return KeyResult(SPECIAL_KEY::DOWN);
        //             case 77:
        //                 return KeyResult(SPECIAL_KEY::RIGHT);
        //             case 75:
        //                 return KeyResult(SPECIAL_KEY::LEFT);
        //             default:
        //                 return KeyResult(SPECIAL_KEY::UNKNOWN);
        //         }
        //     } else {
        //         switch (ch) {
        //             case 13:
        //                 return KeyResult(SPECIAL_KEY::ENTER);
        //             case 27:
        //                 return KeyResult(SPECIAL_KEY::ESC);
        //             case 32:
        //                 return KeyResult(SPECIAL_KEY::SPACE);
        //             case 8:
        //                 return KeyResult(SPECIAL_KEY::BACKSPACE);
        //             default:
        //                 return KeyResult(SPECIAL_KEY::UNKNOWN);
        //         }
        //     }
        // }
    }
}