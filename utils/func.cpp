#include "func.h"

#include <iostream>
#include <vector>
#include <string>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>

// HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

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

#endif

#ifdef __linux__

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

int _getch() {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);  // 设置为非阻塞模式

    char c[3];
    int nread = read(STDIN_FILENO, c, sizeof(c));

    if (nread == 1 && c[0] == 27) {
        return 27; // ESC 键
    } else if (nread > 1 && c[0] == 27 && c[1] == '[') {
        switch (c[2]) {
            case 'A': return 1000; // 上箭头
            case 'B': return 1001; // 下箭头
            case 'C': return 1002; // 右箭头
            case 'D': return 1003; // 左箭头
        }
    } else if (nread == 1) {
        return c[0]; // 返回读取到的普通字符
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    fcntl(STDIN_FILENO, F_SETFL, 0);  // 恢复为阻塞模式
    return -1;
}

KeyResult key_catch() {
    while (true) {
        int ch = _getch();
        if (ch >= 32 && ch <= 126) {
            return KeyResult(std::string(1, static_cast<char>(ch)));
        } else {
            switch (ch) {
                case 1000:
                    return KeyResult(SPECIAL_KEY::UP);
                case 1001:
                    return KeyResult(SPECIAL_KEY::DOWN);
                case 1002:
                    return KeyResult(SPECIAL_KEY::RIGHT);
                case 1003:
                    return KeyResult(SPECIAL_KEY::LEFT);
                case 27:
                    return KeyResult(SPECIAL_KEY::ESC);
                case 10:
                    return KeyResult(SPECIAL_KEY::ENTER);
                case 32:
                    return KeyResult(SPECIAL_KEY::SPACE);
                case 127:
                    return KeyResult(SPECIAL_KEY::BACKSPACE);
                default:
                    return KeyResult(SPECIAL_KEY::UNKNOWN);
            }
        }
    }
}


#endif

void debug(const std::string &msg) {
    std::cout << msg << std::endl;
}




#ifdef TEST

int main() {
    while (true) {
        KeyResult key = key_catch();
        if (key.isKey) {
            switch (key.key) {
                case SPECIAL_KEY::UP:
                    debug("UP");
                    break;
                case SPECIAL_KEY::DOWN:
                    debug("DOWN");
                    break;
                case SPECIAL_KEY::RIGHT:
                    debug("RIGHT");
                    break;
                case SPECIAL_KEY::LEFT:
                    debug("LEFT");
                    break;
                case SPECIAL_KEY::ENTER:
                    debug("ENTER");
                    break;
                case SPECIAL_KEY::ESC:
                    debug("ESC");
                    break;
                case SPECIAL_KEY::SPACE:
                    debug("SPACE");
                    break;
                case SPECIAL_KEY::BACKSPACE:
                    debug("BACKSPACE");
                    break;
                default:
                    // debug("UNKNOWN");
                    break;
            }
        } else {
            debug(key.str);
        }
    }
    return 0;
}

#endif