#include "console.hpp"

#ifdef _WIN32

#include <windows.h>
#include <iostream>

Cursor::Cursor() {
    get_screen_x_y();
    
}

void Cursor::get_screen_x_y() {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    screen_x = csbi.dwSize.X;
    screen_y = csbi.dwSize.Y;
}

void Cursor::get_cursor_x_y() {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    cursor_x = csbi.dwCursorPosition.X;
    cursor_y = csbi.dwCursorPosition.Y;
}

std::pair<int, int> Cursor::now_screen_x_y() {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    screen_x = csbi.dwSize.X;
    screen_y = csbi.dwSize.Y;
    return std::make_pair(screen_x, screen_y);
}

std::pair<int, int> Cursor::now_cursor_x_y() {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    cursor_x = csbi.dwCursorPosition.X;
    cursor_y = csbi.dwCursorPosition.Y;
    return std::make_pair(cursor_x, cursor_y);
}

void Cursor::set_cursor_Pos(int x, int y) {
    COORD coordScreen = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
    SetConsoleCursorPosition(hConsole, coordScreen);
}


void Cursor::clsline(int line, int count) {
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

void Cursor::clsfront(int front, int PosX, int PosY) {
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

void Cursor::clsback(int back, int PosX, int PosY) {
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

void Cursor::fill(int x, int y, int width, int height, char ch) {
    COORD coordScreen = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
    DWORD cCharsWritten;
    if (!FillConsoleOutputCharacter(hConsole, (TCHAR) ch, width * height, coordScreen, &cCharsWritten)) {
        return;
    }
    SetConsoleCursorPosition(hConsole, coordScreen);
}

void Cursor::cursor_move(const int &x, const int &y) {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    COORD cursorPos = csbi.dwCursorPosition;
    cursorPos.X += x;
    cursorPos.Y += y;
    SetConsoleCursorPosition(hConsole, cursorPos);
}

void Cursor::printcursorPos() {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    std::cout << "X: " << csbi.dwCursorPosition.X << " Y: " << csbi.dwCursorPosition.Y;
}

void Cursor::printscreensize() {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    std::cout << "X: " << csbi.dwSize.X << " Y: " << csbi.dwSize.Y << std::endl;
}

void Cursor::coutxy(int x, int y, std::string msg) {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    COORD coordScreen = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
    SetConsoleCursorPosition(hConsole, coordScreen);
    std::cout << msg;
}

void Cursor::debug_in_last_line(std::string msg) {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    for (std::string::size_type i = 0; i < msg.size(); ++i) {
        fill(i, csbi.dwSize.Y, 1, 1, msg[i]);
    }
}

#endif


#ifdef __linux__

#include <iostream>
#include <string>
#include <utility>
#include <unistd.h>
#include <termios.h>

Cursor::Cursor() {
    // get_screen_x_y();
}

void Cursor::get_screen_x_y() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    screen_x = w.ws_col;
    screen_y = w.ws_row;
    
}

void Cursor::get_cursor_x_y() {
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

    cursor_x = x;
    cursor_y = y;
}

std::pair<int, int> Cursor::now_screen_x_y() {
    get_screen_x_y();
    return std::make_pair(screen_x, screen_y);
}

std::pair<int, int> Cursor::now_cursor_x_y() {
    get_cursor_x_y();
    return std::make_pair(cursor_x, cursor_y);
}

void Cursor::set_cursor_Pos(int x, int y) {
    std::cout << "\033[" << y << ";" << x << "H";
}

void Cursor::clsline(int line, int count) {
    std::cout << "\033[" << line << ";0H";
    for (int i = 0; i < count; ++i) {
        std::cout << "\033[K";
    }
}

void Cursor::clsfront(int front, int PosX, int PosY) {
    std::cout << "\033[" << PosY << ";" << PosX - front << "H";
    for (int i = 0; i < front; ++i) {
        std::cout << " ";
    }
}

void Cursor::clsback(int back, int PosX, int PosY) {
    std::cout << "\033[" << PosY << ";" << PosX << "H";
    for (int i = 0; i < back; ++i) {
        std::cout << " ";
    }
}

void Cursor::fill(int x, int y, int width, int height, char ch) {
    std::cout << "\033[" << y << ";" << x << "H";
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            std::cout << ch;
        }
    }
}

void Cursor::cursor_move(const int &x, const int &y) {
    std::cout << "\033[" << y << ";" << x << "H";
}

void Cursor::printcursorPos() {
    std::cout << "\033[6n";
}

void Cursor::printscreensize() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    std::cout << "X: " << w.ws_col << " Y: " << w.ws_row << std::endl;
}

void Cursor::coutxy(int x, int y, std::string msg) {
    std::cout << "\033[" << y << ";" << x << "H";
    std::cout << msg;
}

void Cursor::debug_in_last_line(std::string msg) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    for (std::string::size_type i = 0; i < msg.size(); ++i) {
        fill(i, w.ws_row, 1, 1, msg[i]);
    }
}

#endif


// #ifdef TEST

// int main() {
//     // printscreensize();
//     // printcursorPos();
//     Cursor cursor;
//     CONSOLE_SCREEN_BUFFER_INFO csbi;
//     GetConsoleScreenBufferInfo(hConsole, &csbi);
//     int width = csbi.dwSize.X;
//     int height = csbi.dwSize.Y;
//     cursor.fill(0, 0, width, height, '*');

//     cursor.set_cursor_Pos(10, 15);

//     // cursor.clsline(5);
//     cursor.clsfront(5, 4, 15);
//     cursor.clsback(5, 4, 15);
//     cursor.set_cursor_Pos(10, 15);
//     while (true) {}
//     return 0;
// }

// #endif