#pragma once

#include <iosfwd>
#include <string>
#include <utility>

namespace Inquire {

class Terminal {
public:
    static void init();
    static std::pair<int, int> size();
    static bool is_tty();

    static std::ostream& out();

    static void hide_cursor();
    static void show_cursor();

    static void cursor_up(int n = 1);
    static void cursor_down(int n = 1);
    static void cursor_left(int n = 1);
    static void cursor_right(int n = 1);
    static void cursor_to_column(int col);

    static void clear_line();
    static void clear_to_eol();
    static void clear_below();

    static void newline();
    static void flush();
};

class CursorHider {
public:
    CursorHider();
    ~CursorHider();
    CursorHider(const CursorHider&) = delete;
    CursorHider& operator=(const CursorHider&) = delete;
};

}
