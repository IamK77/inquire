#pragma once

#ifdef _WIN32

#include <windows.h>
#endif
#include <utility>
#include <string>   

// HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#ifdef _WIN32
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

