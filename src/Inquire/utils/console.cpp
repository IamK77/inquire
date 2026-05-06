#include "console.hpp"

#include <atomic>
#include <iostream>

#ifdef _WIN32
    #include <io.h>
    #include <windows.h>
#endif

#ifdef __linux__
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

namespace Inquire {

namespace {
    std::atomic<bool> g_initialized{false};

#ifdef _WIN32
    void enable_vt_mode() {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) return;
        DWORD mode = 0;
        if (!GetConsoleMode(hOut, &mode)) return;
        SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT);

        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        DWORD inMode = 0;
        if (GetConsoleMode(hIn, &inMode)) {
            SetConsoleMode(hIn, inMode | ENABLE_VIRTUAL_TERMINAL_INPUT);
        }
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    }
#endif
}

void Terminal::init() {
    bool expected = false;
    if (!g_initialized.compare_exchange_strong(expected, true)) return;
#ifdef _WIN32
    enable_vt_mode();
#endif
}

std::pair<int, int> Terminal::size() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleScreenBufferInfo(h, &csbi)) {
        int cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        return { cols, rows };
    }
    return { 80, 24 };
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0) {
        return { static_cast<int>(w.ws_col), static_cast<int>(w.ws_row) };
    }
    return { 80, 24 };
#endif
}

bool Terminal::is_tty() {
#ifdef _WIN32
    return _isatty(_fileno(stdout)) != 0;
#else
    return isatty(STDOUT_FILENO) != 0;
#endif
}

std::ostream& Terminal::out() { return std::cout; }

void Terminal::hide_cursor()  { std::cout << "\033[?25l" << std::flush; }
void Terminal::show_cursor()  { std::cout << "\033[?25h" << std::flush; }

void Terminal::cursor_up(int n)    { if (n > 0) std::cout << "\033[" << n << "A"; }
void Terminal::cursor_down(int n)  { if (n > 0) std::cout << "\033[" << n << "B"; }
void Terminal::cursor_left(int n)  { if (n > 0) std::cout << "\033[" << n << "D"; }
void Terminal::cursor_right(int n) { if (n > 0) std::cout << "\033[" << n << "C"; }
void Terminal::cursor_to_column(int col) { std::cout << "\033[" << col << "G"; }

void Terminal::clear_line()    { std::cout << "\r\033[2K"; }
void Terminal::clear_to_eol()  { std::cout << "\033[K"; }
void Terminal::clear_below()   { std::cout << "\033[J"; }

void Terminal::newline() { std::cout << "\n"; }
void Terminal::flush()   { std::cout.flush(); }

CursorHider::CursorHider() { Terminal::hide_cursor(); }
CursorHider::~CursorHider() { Terminal::show_cursor(); }

}
