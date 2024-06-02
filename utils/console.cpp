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
    for (int i = 0; i < msg.size(); ++i) {
        fill(i, csbi.dwSize.Y, 1, 1, msg[i]);
    }
}

#endif


#ifdef TEST

int main() {
    // printscreensize();
    // printcursorPos();
    Cursor cursor;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int width = csbi.dwSize.X;
    int height = csbi.dwSize.Y;
    cursor.fill(0, 0, width, height, '*');

    cursor.set_cursor_Pos(10, 15);

    // cursor.clsline(5);
    cursor.clsfront(5, 4, 15);
    cursor.clsback(5, 4, 15);
    cursor.set_cursor_Pos(10, 15);
    while (true) {}
    return 0;
}

#endif