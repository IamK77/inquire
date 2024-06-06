#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <functional>

#ifdef _WIN32

#include <windows.h>
#include <conio.h>

inline void ToUTF8() {
    SetConsoleOutputCP(65001);
}

inline void ToGBK() {
    SetConsoleOutputCP(936);
}

#endif

#ifdef __linux__

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <utility>
#include <sys/ioctl.h>

void ToUTF8() {
    // do nothing
}

void ToGBK() {
    // do nothing
}

#endif

namespace Inquire {

enum SPECIAL_KEY{
    UP,
    DOWN,
    RIGHT,
    LEFT,
    ENTER,
    ESC,
    SPACE,
    BACKSPACE,
    UNKNOWN
};

struct KeyResult {
    SPECIAL_KEY key;
    std::string str;
    bool isKey;

    KeyResult(SPECIAL_KEY key) : key(key), isKey(true) {}
    KeyResult(std::string str) : str(str), isKey(false) {}
};

#ifdef _WIN32

extern HANDLE hStdin;
extern INPUT_RECORD irInputRecord;
extern DWORD dwEventsRead;
extern KEY_EVENT_RECORD key;

#endif

KeyResult key_catch();


#ifdef _WIN32

HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); 
INPUT_RECORD irInputRecord;
DWORD dwEventsRead;
KEY_EVENT_RECORD key;

KeyResult key_catch() {
    while (true) {
        ReadConsoleInput(hStdin, &irInputRecord, 1, &dwEventsRead);
        if (irInputRecord.EventType == KEY_EVENT && irInputRecord.Event.KeyEvent.bKeyDown) {
            key = irInputRecord.Event.KeyEvent;
            if (key.uChar.AsciiChar != '\0') {
                if (key.wVirtualKeyCode == VK_BACK) {
                    return KeyResult(SPECIAL_KEY::BACKSPACE);
                } else if (key.wVirtualKeyCode == VK_RETURN) {
                    return KeyResult(SPECIAL_KEY::ENTER);
                } else if (key.wVirtualKeyCode == VK_ESCAPE) {
                    return KeyResult(SPECIAL_KEY::ESC);
                } else if (key.wVirtualKeyCode == VK_SPACE) {
                    return KeyResult(SPECIAL_KEY::SPACE);
                } else {
                    return KeyResult(std::string(1, static_cast<char>(key.uChar.AsciiChar)));
                }
            } else {
                switch (key.wVirtualKeyCode) {
                    case VK_UP: return KeyResult(SPECIAL_KEY::UP);
                    case VK_DOWN: return KeyResult(SPECIAL_KEY::DOWN);
                    case VK_LEFT: return KeyResult(SPECIAL_KEY::LEFT);
                    case VK_RIGHT: return KeyResult(SPECIAL_KEY::RIGHT);
                    default: return KeyResult(SPECIAL_KEY::UNKNOWN);
                }
            }
        }
    }
}


#endif

#ifdef __linux__



inline int _getch() {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);  // 设置为非阻塞模式

    char c[3];
    int nread = read(STDIN_FILENO, c, sizeof(c));

    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    fcntl(STDIN_FILENO, F_SETFL, 0);  // 恢复为阻塞模式

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
        return c[0]; 
    }


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


/*------------------------colorful---------------------------*/

std::string to_string(const std::string &msg);

std::string to_string(const char &msg);

template <typename T>
std::string red(const T &msg) {
    return "\033[31m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string green(const T &msg) {
    return "\033[32m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string yellow(const T &msg) {
    return "\033[33m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string blue(const T &msg) {
    return "\033[34m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string magenta(const T &msg) {
    return "\033[35m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string cyan(const T &msg) {
    return "\033[36m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string white(const T &msg) {
    return "\033[37m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string black(const T &msg) {
    return "\033[30m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string bold(const T &msg) {
    return "\033[1m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string underline(const T &msg) {
    return "\033[4m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string gray(const T &msg) {
    return "\033[90m" + to_string(msg) + "\033[0m";
}

inline std::string to_string(const std::string &msg) {
    return msg;
}

inline std::string to_string(const char &msg) {
    return std::string(1, msg);
}




/*------------------------console---------------------------*/


#ifdef _WIN32
    extern HANDLE hConsole;
    extern CONSOLE_SCREEN_BUFFER_INFO csbi;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
#endif

class Cursor {
    public:
        Cursor();

    private:
        int screen_x;
        int screen_y;

        int cursor_x;
        int cursor_y;

    public:
        void get_screen_x_y();
        void get_cursor_x_y();

        std::pair<int, int> now_screen_x_y();
        std::pair<int, int> now_cursor_x_y();

        void set_cursor_Pos(int x, int y);

        void clsline(int line, int count = 1);

        void clsfront(int front, int PosX, int PosY);

        void clsback(int back, int PosX, int PosY);

        void fill(int x, int y, int width, int height, char ch);

        void cursor_move(const int &x, const int &y);

        void printcursorPos();

        void printscreensize();

        void coutxy(int x, int y, std::string msg);

        void debug_in_last_line(std::string msg);

};

#ifdef _WIN32

Inquire::Cursor::Cursor() {
    get_screen_x_y();
    
}

inline void Inquire::Cursor::get_screen_x_y() {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    screen_x = csbi.dwSize.X;
    screen_y = csbi.dwSize.Y;
}

inline void Inquire::Cursor::get_cursor_x_y() {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    cursor_x = csbi.dwCursorPosition.X;
    cursor_y = csbi.dwCursorPosition.Y;
}

inline std::pair<int, int> Inquire::Cursor::now_screen_x_y() {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    screen_x = csbi.dwSize.X;
    screen_y = csbi.dwSize.Y;
    return std::make_pair(screen_x, screen_y);
}

inline std::pair<int, int> Inquire::Cursor::now_cursor_x_y() {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    cursor_x = csbi.dwCursorPosition.X;
    cursor_y = csbi.dwCursorPosition.Y;
    return std::make_pair(cursor_x, cursor_y);
}

inline void Inquire::Cursor::set_cursor_Pos(int x, int y) {
    COORD coordScreen = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
    SetConsoleCursorPosition(hConsole, coordScreen);
}


inline void Inquire::Cursor::clsline(int line, int count) {
    COORD coordScreen = {0, static_cast<SHORT>(line)};
    DWORD cCharsWritten;
    DWORD dwConSize;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        return;
    }
    dwConSize = csbi.dwSize.X * count;
    if (!FillConsoleOutputCharacter(hConsole, (TCHAR) ' ', dwConSize, coordScreen, &cCharsWritten)) {
        return;
    }
    if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten)) {
        return;
    }
    SetConsoleCursorPosition(hConsole, coordScreen);
}

inline void Inquire::Cursor::clsfront(int front, int PosX, int PosY) {
    if (front > PosX) {
        front = PosX;
    }
    COORD coordScreen = {static_cast<SHORT>(PosX - front), static_cast<SHORT>(PosY)};
    DWORD cCharsWritten;
    if (!FillConsoleOutputCharacter(hConsole, (TCHAR) ' ', front, coordScreen, &cCharsWritten)) {
        return;
    }
    if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, front, coordScreen, &cCharsWritten)) {
        return;
    }
    SetConsoleCursorPosition(hConsole, coordScreen);
}

inline void Inquire::Cursor::clsback(int back, int PosX, int PosY) {
    if (back > csbi.dwSize.X - PosX) {
        back = csbi.dwSize.X - PosX;
    }
    COORD coordScreen = {static_cast<SHORT>(PosX), static_cast<SHORT>(PosY)};
    DWORD cCharsWritten;
    if (!FillConsoleOutputCharacter(hConsole, (TCHAR) ' ', back, coordScreen, &cCharsWritten)) {
        return;
    }
    if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, back, coordScreen, &cCharsWritten)) {
        return;
    }
    SetConsoleCursorPosition(hConsole, coordScreen);
}

void Inquire::Cursor::fill(int x, int y, int width, int height, char ch) {
    COORD coordScreen = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
    DWORD cCharsWritten;
    if (!FillConsoleOutputCharacter(hConsole, (TCHAR) ch, width * height, coordScreen, &cCharsWritten)) {
        return;
    }
    SetConsoleCursorPosition(hConsole, coordScreen);
}

inline void Inquire::Cursor::cursor_move(const int &x, const int &y) {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    COORD cursorPos = csbi.dwCursorPosition;
    cursorPos.X += x;
    cursorPos.Y += y;
    SetConsoleCursorPosition(hConsole, cursorPos);
}

void Inquire::Cursor::printcursorPos() {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    std::cout << "X: " << csbi.dwCursorPosition.X << " Y: " << csbi.dwCursorPosition.Y;
}

void Inquire::Cursor::printscreensize() {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    std::cout << "X: " << csbi.dwSize.X << " Y: " << csbi.dwSize.Y << std::endl;
}

void Inquire::Cursor::coutxy(int x, int y, std::string msg) {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    COORD coordScreen = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
    SetConsoleCursorPosition(hConsole, coordScreen);
    std::cout << msg;
}

void Inquire::Cursor::debug_in_last_line(std::string msg) {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    for (std::string::size_type i = 0; i < msg.size(); ++i) {
        fill(i, csbi.dwSize.Y, 1, 1, msg[i]);
    }
}

#endif


#ifdef __linux__



Inquire::Cursor::Cursor() {
    get_screen_x_y();
}

inline void Inquire::Cursor::get_screen_x_y() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    screen_x = w.ws_col;
    screen_y = w.ws_row;
}

inline void Inquire::Cursor::get_cursor_x_y() {
    // Save the termios settings
    struct termios saved_settings;
    tcgetattr(STDIN_FILENO, &saved_settings);

    // Set the termios settings to allow us to read the cursor position
    struct termios settings = saved_settings;
    settings.c_lflag &= ~ICANON;
    settings.c_lflag &= ~ECHO;
    settings.c_cc[VMIN] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &settings);

    // Write the escape code to the terminal
    std::cout << "\033[6n";

    // Read the cursor position from the terminal
    int x = 0, y = 0;
    scanf("\033[%d;%dR", &y, &x);

    // Restore the termios settings
    tcsetattr(STDIN_FILENO, TCSANOW, &saved_settings);

    cursor_x = x - 1;
    cursor_y = y - 1;
}

inline std::pair<int, int> Inquire::Cursor::now_screen_x_y() {
    get_screen_x_y();
    return std::make_pair(screen_x, screen_y);
}

inline std::pair<int, int> Inquire::Cursor::now_cursor_x_y() {
    get_cursor_x_y();
    return std::make_pair(cursor_x, cursor_y);
}

inline void Inquire::Cursor::set_cursor_Pos(int x, int y) {
    std::cout << "\033[" << y + 1 << ";" << x + 1 << "H" << std::flush;
}

inline void Inquire::Cursor::clsline(int line, int count) {
    // std::cout << "\033[s";
    // std::pair<int, int> xy = now_cursor_x_y();
    set_cursor_Pos(0, line);
    for (int i = 0; i < count; ++i) {
        std::cout << "\033[K" << std::endl;
    }
    cursor_move(0, -count);
    // std::cout << "\033[u" << std::flush;
    // set_cursor_Pos(xy.first, xy.second);
}

void Inquire::Cursor::clsfront(int front, int PosX, int PosY) {
    std::pair<int, int> xy = now_cursor_x_y();
    std::cout << "\033[" << PosY + 1 << ";" << PosX - front + 1 << "H";
    for (int i = 0; i < front; ++i) {
        std::cout << " ";
    }
    std::cout << std::flush;
    set_cursor_Pos(xy.first, xy.second);
}

void Inquire::Cursor::clsback(int back, int PosX, int PosY) {
    std::pair<int, int> xy = now_cursor_x_y();
    std::cout << "\033[" << PosY + 1 << ";" << PosX + 1 << "H";
    for (int i = 0; i < back; ++i) {
        std::cout << " ";
    }
    std::cout << std::flush;
    set_cursor_Pos(xy.first, xy.second);
}

void Inquire::Cursor::fill(int x, int y, int width, int height, char ch) {
    std::cout << "\033[" << y << ";" << x << "H" << std::flush;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            std::cout << ch;
        }
    }
}

inline void Inquire::Cursor::cursor_move(const int &x, const int &y) {
    if (x > 0) {
        std::cout << "\033[" << x << "C" << std::flush;  // 向右移动
    } else if (x < 0) {
        std::cout << "\033[" << -x << "D" << std::flush;  // 向左移动
    }

    if (y > 0) {
        std::cout << "\033[" << y << "B" << std::flush;  // 向下移动
    } else if (y < 0) {
        std::cout << "\033[" << -y << "A" << std::flush;  // 向上移动
    }
}

void Inquire::Cursor::printcursorPos() {
    std::cout << "\033[6n" << std::flush;
}

void Inquire::Cursor::printscreensize() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    std::cout << "X: " << w.ws_col << " Y: " << w.ws_row << std::endl;
}

void Inquire::Cursor::coutxy(int x, int y, std::string msg) {
    std::cout << "\033[" << y + 1 << ";" << x + 1 << "H" << std::flush;
    std::cout << msg;
}

void Inquire::Cursor::debug_in_last_line(std::string msg) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    for (std::string::size_type i = 0; i < msg.size(); ++i) {
        fill(i, w.ws_row, 1, 1, msg[i]);
    }
}

#endif





/*-----------------------------inquire-------------------------------*/



class Select {
    private:
        std::vector<std::string> options;
        std::string prompt_str;
        int MAX_OPTION_LINE;
    private:
        Inquire::Cursor cursor;

        std::pair<int, int> ori_cursorPos;

        std::string input = "";
        int option_size;
        int selected = 0;
        int cursorPos = 0;
        int preselected = 0;
        
        #ifdef __linux__
            int temp = 0;
        #endif
        
    public:
        Select(std::string prompt_str, std::vector<std::string> options, int MAX_OPTION_LINE = 7);
        std::string prompt();
    private:
        void flush_display(bool is_clear = false);
        void input_insert(std::string ch);
        void flush_result(std::string result);
        void flush_input();
        void up(); 
        void down();
        void right();
        void left();
};


class Text {
    private:
        std::string prompt_str;
        Inquire::Cursor cursor;
        std::string input;
    public:
        Text(std::string prompt_str);
        std::string prompt();
};


class Password {
    private:
        std::string prompt_str;
        Inquire::Cursor cursor;
    public:
        Password(std::string prompt_str);
        std::string prompt();
};


class Confirm {
    private:
        std::string prompt_str;
        Inquire::Cursor cursor;
    public:
        Confirm(std::string prompt_str);
        bool prompt();
};


class MultiSelect {
    private:
        std::vector<std::string> options;
        std::string prompt_str;
        int MAX_OPTION_LINE;
    private:
        Inquire::Cursor cursor;

        std::pair<int, int> ori_cursorPos;

        std::vector<std::string> input;
        int option_size;
        std::vector<int> selected;
        int cursorPos = 0;
        int preselected = 0;
    public:
        MultiSelect(std::string prompt_str, std::vector<std::string> options, int MAX_OPTION_LINE = 10);
        std::vector<std::string> prompt();
    private:
        void flush_display(bool is_clear = false);
        void input_insert(std::string ch);
        void flush_result(std::vector<std::string> result);
        void flush_input();
        void up(); 
        void down();
        void right();
        void left();
};





/* ---------------------------Select-------------------------------*/

Inquire::Select::Select(std::string prompt_str, std::vector<std::string> options, int MAX_OPTION_LINE) : prompt_str(prompt_str), options(options), option_size(options.size()), preselected(options.size()) {

    ToUTF8();

    #ifdef _WIN32
        if (((cursor.now_screen_x_y().second - 1) - cursor.now_cursor_x_y().second) < (options.size() + 1)) {
            for (int i = 0; i < (options.size() + 1); ++i) {
                std::cout << std::endl;
            }

            std::pair<int, int> xy = cursor.now_cursor_x_y();
            cursor.set_cursor_Pos(xy.first, xy.second - (options.size() + 1));
        }
    #endif

    std::cout << green("? ") << prompt_str << " : " << std::flush;

    ori_cursorPos = cursor.now_cursor_x_y();

    if (options.size() == 0) {
        std::cout << red("No options available") << std::endl;
        return;
    }
}

std::string Inquire::Select::prompt() {
    flush_display();
    while (true) {
        KeyResult key = Inquire::key_catch();
        if (key.isKey) {
            switch (key.key) {
                case SPECIAL_KEY::UP:
                    up(); 
                    break;
                case SPECIAL_KEY::DOWN:
                    down();
                    break;
                case SPECIAL_KEY::RIGHT:
                    right();
                    break;
                case SPECIAL_KEY::LEFT:
                    left();
                    break;
                case SPECIAL_KEY::ENTER:
                    flush_display(true);
                    flush_result(cyan(options[selected]));
                    return options[selected];
                case SPECIAL_KEY::BACKSPACE:
                    if (cursorPos > 0) {
                        #ifdef __linux__
                            temp = cursorPos;
                        #endif

                        cursorPos--;
                        input.erase(cursorPos, 1);
                        flush_input();
                        flush_display();
                    }
                    break;
                case SPECIAL_KEY::ESC:
                    flush_display(true);
                    flush_result(red("<canceled>"));
                    return "";
                default:
                    break;
            }
        } else {
            input_insert(key.str);
            flush_input();
            flush_display();
    }}
};

void Inquire::Select::flush_display(bool is_clear) {
    if (is_clear) {
        #ifdef _WIN32
            cursor.clsline(ori_cursorPos.second + 1, option_size + 1);
        #endif
        #ifdef __linux__
            std::cout << std::endl;
            for(int i = 0; i < option_size; ++i) {
                std::cout << "\033[K" << std::endl;
            }
            std::cout << "\033[K" << std::flush;
            cursor.cursor_move(prompt_str.size() + 5, - option_size - 1);
        #endif

    } else {

        #ifdef __linux
            std::cout << std::endl;
        #endif

        #ifdef _WIN32
            cursor.clsline(ori_cursorPos.second + 1, option_size + 1);
        #endif

        std::vector<std::string> predisplays;

        for (int i = 0; i < options.size(); ++i) {
            if (options[i].find(input) != std::string::npos) {
                predisplays.push_back(options[i]);
            }
        }

        preselected = predisplays.size();

        if (selected >= preselected) {
            selected = 0;
        }

        #ifdef __linux__
            for(int i = 0; i < option_size; ++i) {
                std::cout << "\033[K" << std::endl;
            }
            std::cout << "\033[K" << std::flush;
            cursor.cursor_move(0, - option_size);
        #endif

        for (int i = 0; i < predisplays.size(); ++i) {
            if (i == selected) {
                std::cout << cyan("> ") << cyan(predisplays[i]) << std::endl;
            } else {
                std::cout << "  " << predisplays[i] << std::endl;
            }
        }

        std::cout << cyan("[↑↓ to move, enter to select, type to filter]");

        #ifdef _WIN32
            cursor.set_cursor_Pos(ori_cursorPos.first + cursorPos, ori_cursorPos.second);
        #endif

        #ifdef __linux__
            std::cout << "\r" << std::flush;
            cursor.cursor_move(ori_cursorPos.first + cursorPos, -preselected - 1);
        #endif
    }

}

inline void Inquire::Select::flush_input() {
    #ifdef _WIN32
        cursor.clsback(input.size() + 1, ori_cursorPos.first, ori_cursorPos.second);
        cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
        std::cout << input;
        cursor.cursor_move(cursorPos - input.size(), 0);
    #endif

    #ifdef __linux__
        cursor.cursor_move(-(temp), 0);
        for(int i = 0; i <= input.size(); ++i) {
            std::cout << " ";
        }
        cursor.cursor_move(-input.size() - 1, 0);
        std::cout << input << std::flush;
        cursor.cursor_move(cursorPos - input.size(), 0);
    #endif
}

inline void Inquire::Select::flush_result(std::string result) {

    #ifdef _WIN32
        while (input.size() > 0) {
            input.pop_back();
            flush_input();
            if (input.size() > 0 && input.size() < 10) {
                Sleep(100 / input.size());
            }
        }
        cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
        
    #endif

    #ifdef __linux__
        for(int i = 0; i <= input.size(); ++i) {
            std::cout << " ";
        }
        cursor.cursor_move(- input.size() - 1, 0);
    #endif

    for (int i = 0; i < result.size(); ++i) {
        std::cout << result[i];
        if (result.size() < 20 && (result.size() - i) > 0) {
            #ifdef _WIN32
                Sleep(100 / (result.size() - i));
            #endif
        }
    }
    std::cout << std::endl;
}

inline void Inquire::Select::input_insert(std::string ch) {
    input.insert(cursorPos, ch);
    #ifdef __linux__
        temp = cursorPos;
    #endif

    cursorPos++;
}

inline void Inquire::Select::up() {
    selected = (selected - 1 + option_size) % option_size;
    flush_display();
}

inline void Inquire::Select::down() {
    selected = (selected + 1) % option_size;
    flush_display();
}

inline void Inquire::Select::right() {
    if (cursorPos < input.size()) {
        cursorPos++;
        cursor.cursor_move(1, 0);
    }
}

inline void Inquire::Select::left() {
    if (cursorPos > 0) {
        cursorPos--;
        cursor.cursor_move(-1, 0);
    }
}


/* ---------------------------Text-------------------------------*/

Inquire::Text::Text(std::string prompt_str) : prompt_str(prompt_str) {

    ToUTF8();

    std::cout << green("? ") << prompt_str << " : ";
}

std::string Inquire::Text::prompt() {

    #ifdef _WIN32
        std::pair<int, int> ori_cursorPos = cursor.now_cursor_x_y();
        std::getline(std::cin, input);
        cursor.clsback(input.size() + 1, ori_cursorPos.first, ori_cursorPos.second);
        cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
    #endif

    #ifdef __linux__
        if (std::cin.peek() == '\n') { 
            std::cin.ignore();
        }
        std::getline(std::cin, input);
        cursor.cursor_move((prompt_str.size() + 5), -1);
        for(int i = 0; i <= input.size(); ++i) { std::cout << " "; }
        cursor.cursor_move(-input.size() - 1, 0);
    #endif

    std::cout << cyan(input) << std::endl;
    return input;
}


/* ---------------------------Password-------------------------------*/

Inquire::Password::Password(std::string prompt_str) : prompt_str(prompt_str) {

    ToUTF8();

    std::cout << green("? ") << prompt_str << " : ";
}

std::string Inquire::Password::prompt() {
    std::string input;

    #ifdef _WIN32

    std::pair<int, int> ori_cursorPos = cursor.now_cursor_x_y();
    while (true) {
        KeyResult key = Inquire::key_catch();
        if (key.isKey) {
            switch (key.key) {
                case SPECIAL_KEY::ENTER:
                    cursor.clsback(input.size() + 1, ori_cursorPos.first, ori_cursorPos.second);
                    cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
                    std::cout << cyan("********") << std::endl;
                    return input;
                case SPECIAL_KEY::BACKSPACE:
                    if (input.size() > 0) {
                        input.pop_back();
                        cursor.clsback(1, cursor.now_cursor_x_y().first, cursor.now_cursor_x_y().second);
                    }
                    break;
                case SPECIAL_KEY::ESC:
                    cursor.clsback(input.size() + 1, ori_cursorPos.first, ori_cursorPos.second);
                    cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
                    std::cout << red("<canceled>") << std::endl;
                    return "";
                default:
                    break;
            }
        } else {
            input += key.str;
            std::cout << "*";
        }
    }

    #endif


    #ifdef __linux__

    while (true) {
        std::cout << std::flush;
        KeyResult key = Inquire::key_catch();
        if (key.isKey) {
            switch (key.key) {
                case SPECIAL_KEY::ENTER:
                    cursor.cursor_move(-input.size(), 0);
                    for(int i = 0; i <= input.size(); ++i) { std::cout << " "; }
                    cursor.cursor_move(-input.size() - 1, 0);
                    std::cout << cyan("********") << std::endl;
                    return input;
                case SPECIAL_KEY::BACKSPACE:
                    if (input.size() > 0) {
                        input.pop_back();
                        cursor.cursor_move(-1, 0);
                        std::cout << " ";
                        cursor.cursor_move(-1, 0);
                    }
                    break;
                case SPECIAL_KEY::ESC:
                    cursor.cursor_move(-input.size(), 0);
                    for(int i = 0; i <= input.size(); ++i) { std::cout << " "; }
                    cursor.cursor_move(-input.size() - 1, 0);
                    std::cout << red("<canceled>") << std::endl;
                    return "";
                default:
                    break;
            }
        } else {
            input += key.str;
            std::cout << "*" << std::flush;
        }
    }

    #endif

    return input;

}



/* ---------------------------Confirm-------------------------------*/

Inquire::Confirm::Confirm(std::string prompt_str) : prompt_str(prompt_str) {

    ToUTF8();

    std::cout << green("? ") << prompt_str << " [Enter/ESC] ";
}

bool Inquire::Confirm::prompt() {

    #ifdef _WIN32
    std::pair<int, int> ori_cursorPos = cursor.now_cursor_x_y();
    std::cout << gray("Enter: Yes, ESC: No");
    cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
    while (true) {
        KeyResult key = Inquire::key_catch();
        if (key.isKey) {
            switch (key.key) {
                case SPECIAL_KEY::ENTER:
                    cursor.clsback(19, ori_cursorPos.first, ori_cursorPos.second);
                    cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
                    cursor.clsback(19, cursor.now_cursor_x_y().first, cursor.now_cursor_x_y().second);
                    std::cout << cyan("Yes") << std::endl;
                    return true;
                case SPECIAL_KEY::ESC:
                    cursor.clsback(1, ori_cursorPos.first, ori_cursorPos.second);
                    cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
                    std::cout << red("No") << std::endl;
                    return false;
                default:
                    break;
            }
        }
    }
    #endif


    #ifdef __linux__

    std::string help = "Enter: Yes, ESC: No";
    std::cout << gray(help);
    cursor.cursor_move(-help.size(), 0);

    while (true) {
        KeyResult key = Inquire::key_catch();
        if (key.isKey) {
            switch (key.key) {
                case SPECIAL_KEY::ENTER:
                    for (int i = 0; i < help.size(); ++i) {
                        std::cout << " ";
                    }
                    cursor.cursor_move(-help.size(), 0);
                    std::cout << cyan("Yes") << std::endl;
                    return true;
                case SPECIAL_KEY::ESC:
                    for (int i = 0; i < help.size(); ++i) {
                        std::cout << " ";
                    }
                    cursor.cursor_move(-help.size(), 0);
                    std::cout << red("No") << std::endl;
                    return false;
                default:
                    break;
            }
        }
    }

    #endif

    return false;
}


}