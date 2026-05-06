// =========================================================================
// Inquire — single-header interactive CLI library for C++11
//
//   #include "inquire.hpp"
// In EXACTLY ONE translation unit, do:
//   #define INQUIRE_IMPLEMENTATION
//   #include "inquire.hpp"
//
// Project:  https://github.com/IamK77/inquire
// License:  MIT
// =========================================================================

#ifndef INQUIRE_HPP_
#define INQUIRE_HPP_

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <functional>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
    #include <io.h>
#endif

#ifdef __linux__
    #include <cerrno>
    #include <csignal>
    #include <fcntl.h>
    #include <sys/ioctl.h>
    #include <termios.h>
    #include <unistd.h>
#endif

// ----- declarations -----

// ===== src/Inquire/utils/types.h =====

#include <string>

namespace Inquire {

enum class Key {
    None,
    Char,
    Up, Down, Left, Right,
    Enter, Escape, Space, Backspace, Delete, Tab,
    Home, End, PageUp, PageDown,
    CtrlC, CtrlD, CtrlA, CtrlE, CtrlU, CtrlW,
    Unknown
};

struct KeyEvent {
    Key key = Key::None;
    std::string text;

    KeyEvent() = default;
    explicit KeyEvent(Key k) : key(k) {}
    explicit KeyEvent(std::string t) : key(Key::Char), text(std::move(t)) {}

    bool is_char() const noexcept { return key == Key::Char; }
};

}

// ===== src/Inquire/utils/colorful.hpp =====

#include <string>

namespace Inquire {

namespace style {

std::string red(const std::string& s);
std::string green(const std::string& s);
std::string yellow(const std::string& s);
std::string blue(const std::string& s);
std::string magenta(const std::string& s);
std::string cyan(const std::string& s);
std::string white(const std::string& s);
std::string black(const std::string& s);
std::string gray(const std::string& s);
std::string bold(const std::string& s);
std::string dim(const std::string& s);
std::string italic(const std::string& s);
std::string underline(const std::string& s);
std::string reverse(const std::string& s);

void set_enabled(bool on);
bool enabled();

}

inline std::string red(const std::string& s)       { return style::red(s); }
inline std::string green(const std::string& s)     { return style::green(s); }
inline std::string yellow(const std::string& s)    { return style::yellow(s); }
inline std::string blue(const std::string& s)      { return style::blue(s); }
inline std::string magenta(const std::string& s)   { return style::magenta(s); }
inline std::string cyan(const std::string& s)      { return style::cyan(s); }
inline std::string white(const std::string& s)     { return style::white(s); }
inline std::string black(const std::string& s)     { return style::black(s); }
inline std::string gray(const std::string& s)      { return style::gray(s); }
inline std::string bold(const std::string& s)      { return style::bold(s); }
inline std::string underline(const std::string& s) { return style::underline(s); }

}

// ===== src/Inquire/utils/console.hpp =====

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

// ===== src/Inquire/utils/encode.hpp =====

namespace Inquire {

void ToUTF8();
void ToGBK();

}

// ===== src/Inquire/utils/func.h =====

#include <string>

namespace Inquire {

KeyEvent key_catch();

std::size_t utf8_display_width(const std::string& s) noexcept;
std::size_t utf8_char_count(const std::string& s) noexcept;
std::size_t utf8_char_size(const std::string& s, std::size_t byte_pos) noexcept;
std::size_t utf8_prev_char_start(const std::string& s, std::size_t byte_pos) noexcept;

bool icontains(const std::string& haystack, const std::string& needle) noexcept;

}

// ===== src/Inquire/utils/renderer.hpp =====

#include <string>
#include <vector>

namespace Inquire {

class Renderer {
public:
    void clear();
    void draw(const std::vector<std::string>& lines,
              int cursor_line = -1, int cursor_col_1based = -1);
    void finalize(const std::string& final_line);

private:
    int lines_above_ = 0;
    int lines_below_ = 0;
};

std::string truncate_display(const std::string& s, std::size_t max_width);

}

// ===== src/Inquire/utils/tty_guard.hpp =====

#ifdef __linux__

#include <termios.h>

namespace Inquire {

class TtyGuard {
public:
    TtyGuard();
    ~TtyGuard();

    TtyGuard(const TtyGuard&) = delete;
    TtyGuard& operator=(const TtyGuard&) = delete;

    void enter_raw();
    void restore();

    static void install_signal_handler();

private:
    bool active_ = false;
    bool saved_ok_ = false;
};

}

#endif

// ===== src/Inquire/error.hpp =====

#include <stdexcept>
#include <string>

namespace Inquire {

enum class ErrorCode {
    Cancelled,
    Interrupted,
    InvalidInput,
    EmptyOptions,
    Io,
    UnsupportedTerminal
};

class InquireError : public std::runtime_error {
public:
    InquireError(ErrorCode code, const std::string& msg)
        : std::runtime_error(msg), code_(code) {}
    ErrorCode code() const noexcept { return code_; }
private:
    ErrorCode code_;
};

class CancelledError : public InquireError {
public:
    CancelledError()
        : InquireError(ErrorCode::Cancelled, "prompt was cancelled by user") {}
};

class InterruptedError : public InquireError {
public:
    InterruptedError()
        : InquireError(ErrorCode::Interrupted, "prompt was interrupted") {}
};

class ValidationError : public InquireError {
public:
    explicit ValidationError(const std::string& msg)
        : InquireError(ErrorCode::InvalidInput, msg) {}
};

class EmptyOptionsError : public InquireError {
public:
    EmptyOptionsError()
        : InquireError(ErrorCode::EmptyOptions, "no options were provided") {}
};

class IoError : public InquireError {
public:
    explicit IoError(const std::string& msg)
        : InquireError(ErrorCode::Io, msg) {}
};

template <typename T>
class Result {
public:
    static Result<T> ok(T value) {
        Result<T> r;
        r.has_value_ = true;
        r.value_ = std::move(value);
        return r;
    }
    static Result<T> err(ErrorCode code, std::string msg) {
        Result<T> r;
        r.has_value_ = false;
        r.code_ = code;
        r.error_ = std::move(msg);
        return r;
    }

    bool ok() const noexcept { return has_value_; }
    explicit operator bool() const noexcept { return has_value_; }

    const T& value() const& {
        if (!has_value_) throw InquireError(code_, error_);
        return value_;
    }
    T& value() & {
        if (!has_value_) throw InquireError(code_, error_);
        return value_;
    }
    T value_or(T fallback) const { return has_value_ ? value_ : fallback; }

    ErrorCode error_code() const noexcept { return code_; }
    const std::string& error_message() const noexcept { return error_; }

private:
    Result() = default;
    bool has_value_ = false;
    T value_{};
    ErrorCode code_ = ErrorCode::Io;
    std::string error_;
};

}

// ===== src/Inquire/validator.hpp =====


#include <functional>
#include <regex>
#include <string>
#include <vector>

namespace Inquire {

using Validator = std::function<void(const std::string&)>;

namespace validators {

inline Validator non_empty(std::string msg = "input cannot be empty") {
    return [msg](const std::string& s) {
        if (s.empty()) throw ValidationError(msg);
    };
}

inline Validator min_length(std::size_t n, std::string msg = "") {
    return [n, msg](const std::string& s) {
        if (s.size() < n) {
            throw ValidationError(msg.empty()
                ? ("input must be at least " + std::to_string(n) + " characters")
                : msg);
        }
    };
}

inline Validator max_length(std::size_t n, std::string msg = "") {
    return [n, msg](const std::string& s) {
        if (s.size() > n) {
            throw ValidationError(msg.empty()
                ? ("input must be at most " + std::to_string(n) + " characters")
                : msg);
        }
    };
}

inline Validator length_between(std::size_t lo, std::size_t hi, std::string msg = "") {
    return [lo, hi, msg](const std::string& s) {
        if (s.size() < lo || s.size() > hi) {
            throw ValidationError(msg.empty()
                ? ("input length must be between " + std::to_string(lo)
                   + " and " + std::to_string(hi))
                : msg);
        }
    };
}

inline Validator matches(std::string pattern, std::string msg = "") {
    std::regex re(pattern);
    return [re, msg, pattern](const std::string& s) {
        if (!std::regex_match(s, re)) {
            throw ValidationError(msg.empty()
                ? ("input does not match pattern: " + pattern)
                : msg);
        }
    };
}

inline Validator integer(std::string msg = "input must be an integer") {
    return [msg](const std::string& s) {
        if (s.empty()) throw ValidationError(msg);
        std::size_t i = (s[0] == '-' || s[0] == '+') ? 1 : 0;
        if (i == s.size()) throw ValidationError(msg);
        for (; i < s.size(); ++i) {
            if (s[i] < '0' || s[i] > '9') throw ValidationError(msg);
        }
    };
}

inline Validator all_of(std::vector<Validator> vs) {
    return [vs](const std::string& s) {
        for (const auto& v : vs) v(s);
    };
}

}
}

// ===== src/Inquire/inquire.hpp =====


#include <functional>
#include <string>
#include <vector>

namespace Inquire {

class Select {
public:
    Select(std::string question, std::vector<std::string> options);

    Select& page_size(int n);
    Select& default_index(int i);
    Select& help_message(std::string msg);
    Select& filter(std::function<bool(const std::string&, const std::string&)> fn);
    Select& case_sensitive(bool on);

    std::string prompt();
    Result<std::string> try_prompt();
    int prompt_index();

private:
    std::string question_;
    std::vector<std::string> options_;
    int page_size_ = 7;
    int default_index_ = 0;
    bool case_sensitive_ = false;
    std::string help_;
    std::function<bool(const std::string&, const std::string&)> filter_;

    int do_prompt_();
};

class MultiSelect {
public:
    MultiSelect(std::string question, std::vector<std::string> options);

    MultiSelect& page_size(int n);
    MultiSelect& default_indices(std::vector<int> idx);
    MultiSelect& help_message(std::string msg);
    MultiSelect& min_selected(int n);
    MultiSelect& max_selected(int n);
    MultiSelect& case_sensitive(bool on);

    std::vector<std::string> prompt();
    Result<std::vector<std::string>> try_prompt();
    std::vector<int> prompt_indices();

private:
    std::string question_;
    std::vector<std::string> options_;
    int page_size_ = 10;
    int min_selected_ = 0;
    int max_selected_ = -1;
    bool case_sensitive_ = false;
    std::vector<int> defaults_;
    std::string help_;

    std::vector<int> do_prompt_();
};

class Text {
public:
    explicit Text(std::string question);

    Text& default_value(std::string v);
    Text& placeholder(std::string p);
    Text& help_message(std::string m);
    Text& add_validator(Validator v);

    std::string prompt();
    Result<std::string> try_prompt();

private:
    std::string question_;
    std::string default_;
    std::string placeholder_;
    std::string help_;
    std::vector<Validator> validators_;

    std::string do_prompt_();
};

class Password {
public:
    explicit Password(std::string question);

    Password& mask_char(char c);
    Password& show_typing(bool on);
    Password& help_message(std::string m);
    Password& add_validator(Validator v);

    std::string prompt();
    Result<std::string> try_prompt();

private:
    std::string question_;
    char mask_ = '*';
    bool show_ = false;
    std::string help_;
    std::vector<Validator> validators_;

    std::string do_prompt_();
};

class Confirm {
public:
    explicit Confirm(std::string question);

    Confirm& default_value(bool b);
    Confirm& help_message(std::string m);

    bool prompt();
    Result<bool> try_prompt();

private:
    std::string question_;
    bool default_ = true;
    bool has_default_ = false;
    std::string help_;
};

}

#endif // INQUIRE_HPP_

#ifdef INQUIRE_IMPLEMENTATION
#ifndef INQUIRE_IMPL_DEFINED_
#define INQUIRE_IMPL_DEFINED_


// ===== src/Inquire/utils/colorful.cpp =====

#include <atomic>

namespace Inquire {
namespace style {

namespace {
    std::atomic<bool> g_enabled{true};

    std::string wrap(const char* code, const std::string& s) {
        if (!g_enabled.load()) return s;
        std::string out;
        out.reserve(s.size() + 9);
        out.append("\033[").append(code).append("m").append(s).append("\033[0m");
        return out;
    }
}

void set_enabled(bool on) { g_enabled.store(on); }
bool enabled()            { return g_enabled.load(); }

std::string red(const std::string& s)       { return wrap("31", s); }
std::string green(const std::string& s)     { return wrap("32", s); }
std::string yellow(const std::string& s)    { return wrap("33", s); }
std::string blue(const std::string& s)      { return wrap("34", s); }
std::string magenta(const std::string& s)   { return wrap("35", s); }
std::string cyan(const std::string& s)      { return wrap("36", s); }
std::string white(const std::string& s)     { return wrap("37", s); }
std::string black(const std::string& s)     { return wrap("30", s); }
std::string gray(const std::string& s)      { return wrap("90", s); }
std::string bold(const std::string& s)      { return wrap("1",  s); }
std::string dim(const std::string& s)       { return wrap("2",  s); }
std::string italic(const std::string& s)    { return wrap("3",  s); }
std::string underline(const std::string& s) { return wrap("4",  s); }
std::string reverse(const std::string& s)   { return wrap("7",  s); }

}
}

// ===== src/Inquire/utils/console.cpp =====

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

// ===== src/Inquire/utils/encode.cpp =====

#ifdef _WIN32
    #include <windows.h>
#endif

namespace Inquire {

void ToUTF8() {
    Terminal::init();
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

void ToGBK() {
#ifdef _WIN32
    SetConsoleOutputCP(936);
    SetConsoleCP(936);
#endif
}

}

// ===== src/Inquire/utils/func.cpp =====

#include <cstdint>
#include <string>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#endif

#ifdef __linux__
    #include <cerrno>
    #include <fcntl.h>
    #include <termios.h>
    #include <unistd.h>
#endif

namespace Inquire {

namespace {
    bool is_ascii_letter(char c) {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    }
    char to_lower_ascii(char c) {
        return (c >= 'A' && c <= 'Z') ? static_cast<char>(c + 32) : c;
    }
}

bool icontains(const std::string& haystack, const std::string& needle) noexcept {
    if (needle.empty()) return true;
    if (needle.size() > haystack.size()) return false;
    for (std::size_t i = 0; i + needle.size() <= haystack.size(); ++i) {
        std::size_t j = 0;
        for (; j < needle.size(); ++j) {
            char a = haystack[i + j];
            char b = needle[j];
            if (is_ascii_letter(a)) a = to_lower_ascii(a);
            if (is_ascii_letter(b)) b = to_lower_ascii(b);
            if (a != b) break;
        }
        if (j == needle.size()) return true;
    }
    return false;
}

std::size_t utf8_char_size(const std::string& s, std::size_t byte_pos) noexcept {
    if (byte_pos >= s.size()) return 0;
    unsigned char c = static_cast<unsigned char>(s[byte_pos]);
    if (c < 0x80) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

std::size_t utf8_prev_char_start(const std::string& s, std::size_t byte_pos) noexcept {
    if (byte_pos == 0) return 0;
    std::size_t p = byte_pos - 1;
    while (p > 0) {
        unsigned char c = static_cast<unsigned char>(s[p]);
        if ((c & 0xC0) != 0x80) break;
        --p;
    }
    return p;
}

std::size_t utf8_char_count(const std::string& s) noexcept {
    std::size_t count = 0;
    std::size_t i = 0;
    while (i < s.size()) {
        i += utf8_char_size(s, i);
        ++count;
    }
    return count;
}

namespace {
    int codepoint_width(std::uint32_t cp) noexcept {
        if (cp == 0) return 0;
        if (cp < 0x20 || (cp >= 0x7F && cp < 0xA0)) return 0;
        if ((cp >= 0x1100 && cp <= 0x115F) ||
            (cp >= 0x2E80 && cp <= 0x303E) ||
            (cp >= 0x3041 && cp <= 0x33FF) ||
            (cp >= 0x3400 && cp <= 0x4DBF) ||
            (cp >= 0x4E00 && cp <= 0x9FFF) ||
            (cp >= 0xA000 && cp <= 0xA4CF) ||
            (cp >= 0xAC00 && cp <= 0xD7A3) ||
            (cp >= 0xF900 && cp <= 0xFAFF) ||
            (cp >= 0xFE30 && cp <= 0xFE4F) ||
            (cp >= 0xFF00 && cp <= 0xFF60) ||
            (cp >= 0xFFE0 && cp <= 0xFFE6) ||
            (cp >= 0x1F300 && cp <= 0x1F9FF) ||
            (cp >= 0x20000 && cp <= 0x2FFFD)) {
            return 2;
        }
        return 1;
    }

    std::uint32_t decode_utf8(const std::string& s, std::size_t pos, std::size_t len) noexcept {
        if (len == 1) return static_cast<unsigned char>(s[pos]);
        std::uint32_t cp = 0;
        if (len == 2) {
            cp = (static_cast<unsigned char>(s[pos]) & 0x1F) << 6;
            cp |= static_cast<unsigned char>(s[pos + 1]) & 0x3F;
        } else if (len == 3) {
            cp = (static_cast<unsigned char>(s[pos]) & 0x0F) << 12;
            cp |= (static_cast<unsigned char>(s[pos + 1]) & 0x3F) << 6;
            cp |= static_cast<unsigned char>(s[pos + 2]) & 0x3F;
        } else if (len == 4) {
            cp = (static_cast<unsigned char>(s[pos]) & 0x07) << 18;
            cp |= (static_cast<unsigned char>(s[pos + 1]) & 0x3F) << 12;
            cp |= (static_cast<unsigned char>(s[pos + 2]) & 0x3F) << 6;
            cp |= static_cast<unsigned char>(s[pos + 3]) & 0x3F;
        }
        return cp;
    }
}

std::size_t utf8_display_width(const std::string& s) noexcept {
    std::size_t width = 0;
    std::size_t i = 0;
    while (i < s.size()) {
        std::size_t len = utf8_char_size(s, i);
        if (len == 0 || i + len > s.size()) break;
        width += codepoint_width(decode_utf8(s, i, len));
        i += len;
    }
    return width;
}

}

#ifdef _WIN32

namespace Inquire {

namespace {
    std::string utf16_to_utf8(wchar_t wc) {
        wchar_t buf[2] = { wc, 0 };
        int n = WideCharToMultiByte(CP_UTF8, 0, buf, 1, nullptr, 0, nullptr, nullptr);
        if (n <= 0) return std::string();
        std::string out(static_cast<std::size_t>(n), '\0');
        WideCharToMultiByte(CP_UTF8, 0, buf, 1, &out[0], n, nullptr, nullptr);
        return out;
    }
}

KeyEvent key_catch() {
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD rec;
    DWORD read = 0;
    while (true) {
        if (!ReadConsoleInputW(h, &rec, 1, &read) || read == 0) {
            return KeyEvent(Key::Unknown);
        }
        if (rec.EventType != KEY_EVENT || !rec.Event.KeyEvent.bKeyDown) continue;

        const KEY_EVENT_RECORD& k = rec.Event.KeyEvent;
        WORD vk = k.wVirtualKeyCode;
        bool ctrl = (k.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0;

        switch (vk) {
            case VK_UP:     return KeyEvent(Key::Up);
            case VK_DOWN:   return KeyEvent(Key::Down);
            case VK_LEFT:   return KeyEvent(Key::Left);
            case VK_RIGHT:  return KeyEvent(Key::Right);
            case VK_RETURN: return KeyEvent(Key::Enter);
            case VK_ESCAPE: return KeyEvent(Key::Escape);
            case VK_BACK:   return KeyEvent(Key::Backspace);
            case VK_DELETE: return KeyEvent(Key::Delete);
            case VK_TAB:    return KeyEvent(Key::Tab);
            case VK_HOME:   return KeyEvent(Key::Home);
            case VK_END:    return KeyEvent(Key::End);
            case VK_PRIOR:  return KeyEvent(Key::PageUp);
            case VK_NEXT:   return KeyEvent(Key::PageDown);
            case VK_SPACE:  return KeyEvent(Key::Space);
        }

        if (ctrl) {
            switch (vk) {
                case 'C': return KeyEvent(Key::CtrlC);
                case 'D': return KeyEvent(Key::CtrlD);
                case 'A': return KeyEvent(Key::CtrlA);
                case 'E': return KeyEvent(Key::CtrlE);
                case 'U': return KeyEvent(Key::CtrlU);
                case 'W': return KeyEvent(Key::CtrlW);
            }
        }

        wchar_t wc = k.uChar.UnicodeChar;
        if (wc != 0) {
            std::string utf8 = utf16_to_utf8(wc);
            if (!utf8.empty() && static_cast<unsigned char>(utf8[0]) >= 0x20) {
                return KeyEvent(std::move(utf8));
            }
        }
    }
}

}

#endif

#ifdef __linux__

namespace Inquire {

namespace {
    int read_byte_blocking() {
        unsigned char c;
        ssize_t n;
        do { n = ::read(STDIN_FILENO, &c, 1); } while (n == -1 && errno == EINTR);
        if (n <= 0) return -1;
        return c;
    }

    int read_byte_nonblocking() {
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
        unsigned char c;
        ssize_t n = ::read(STDIN_FILENO, &c, 1);
        fcntl(STDIN_FILENO, F_SETFL, flags);
        if (n <= 0) return -1;
        return c;
    }

    KeyEvent parse_csi() {
        std::string params;
        int b;
        while ((b = read_byte_nonblocking()) != -1) {
            char c = static_cast<char>(b);
            if (c >= '0' && c <= '9') { params += c; continue; }
            if (c == ';') { params += c; continue; }
            switch (c) {
                case 'A': return KeyEvent(Key::Up);
                case 'B': return KeyEvent(Key::Down);
                case 'C': return KeyEvent(Key::Right);
                case 'D': return KeyEvent(Key::Left);
                case 'H': return KeyEvent(Key::Home);
                case 'F': return KeyEvent(Key::End);
                case '~': {
                    if (params == "1" || params == "7") return KeyEvent(Key::Home);
                    if (params == "4" || params == "8") return KeyEvent(Key::End);
                    if (params == "3") return KeyEvent(Key::Delete);
                    if (params == "5") return KeyEvent(Key::PageUp);
                    if (params == "6") return KeyEvent(Key::PageDown);
                    return KeyEvent(Key::Unknown);
                }
                default: return KeyEvent(Key::Unknown);
            }
        }
        return KeyEvent(Key::Unknown);
    }
}

KeyEvent key_catch() {
    TtyGuard guard;
    guard.enter_raw();

    int first = read_byte_blocking();
    if (first == -1) return KeyEvent(Key::Unknown);

    if (first == 27) {
        int b = read_byte_nonblocking();
        if (b == -1) return KeyEvent(Key::Escape);
        if (b == '[' || b == 'O') return parse_csi();
        return KeyEvent(Key::Escape);
    }
    if (first == 10 || first == 13) return KeyEvent(Key::Enter);
    if (first == 127 || first == 8) return KeyEvent(Key::Backspace);
    if (first == 9)  return KeyEvent(Key::Tab);
    if (first == 32) return KeyEvent(std::string(1, ' '));
    if (first == 3)  return KeyEvent(Key::CtrlC);
    if (first == 4)  return KeyEvent(Key::CtrlD);
    if (first == 1)  return KeyEvent(Key::CtrlA);
    if (first == 5)  return KeyEvent(Key::CtrlE);
    if (first == 21) return KeyEvent(Key::CtrlU);
    if (first == 23) return KeyEvent(Key::CtrlW);
    if (first < 32)  return KeyEvent(Key::Unknown);
    if (first < 0x80) return KeyEvent(std::string(1, static_cast<char>(first)));

    std::size_t need;
    if ((first & 0xE0) == 0xC0) need = 1;
    else if ((first & 0xF0) == 0xE0) need = 2;
    else if ((first & 0xF8) == 0xF0) need = 3;
    else return KeyEvent(Key::Unknown);

    std::string buf(1, static_cast<char>(first));
    for (std::size_t i = 0; i < need; ++i) {
        int nb = read_byte_blocking();
        if (nb == -1) return KeyEvent(Key::Unknown);
        if ((nb & 0xC0) != 0x80) return KeyEvent(Key::Unknown);
        buf.push_back(static_cast<char>(nb));
    }
    return KeyEvent(std::move(buf));
}

}

#endif

// ===== src/Inquire/utils/renderer.cpp =====


#include <cstdint>
#include <iostream>

namespace Inquire {

void Renderer::clear() {
    if (lines_above_ > 0) Terminal::cursor_up(lines_above_);
    std::cout << "\r";
    Terminal::clear_below();
    lines_above_ = 0;
    lines_below_ = 0;
}

void Renderer::draw(const std::vector<std::string>& lines,
                    int cursor_line, int cursor_col_1based) {
    clear();
    if (lines.empty()) {
        std::cout.flush();
        return;
    }

    for (std::size_t i = 0; i < lines.size(); ++i) {
        std::cout << lines[i];
        if (i + 1 < lines.size()) std::cout << "\n";
    }

    int last_line = static_cast<int>(lines.size()) - 1;
    if (cursor_line >= 0 && cursor_line <= last_line) {
        int up = last_line - cursor_line;
        if (up > 0) Terminal::cursor_up(up);
        std::cout << "\r";
        if (cursor_col_1based >= 1) Terminal::cursor_to_column(cursor_col_1based);
        lines_above_ = cursor_line;
        lines_below_ = last_line - cursor_line;
    } else {
        lines_above_ = last_line;
        lines_below_ = 0;
    }
    std::cout.flush();
}

void Renderer::finalize(const std::string& final_line) {
    clear();
    std::cout << final_line << "\n";
    std::cout.flush();
}

namespace {
    std::size_t cp_width(std::uint32_t cp) {
        if (cp == 0) return 0;
        if (cp < 0x20 || (cp >= 0x7F && cp < 0xA0)) return 0;
        if ((cp >= 0x1100 && cp <= 0x115F) ||
            (cp >= 0x2E80 && cp <= 0x303E) ||
            (cp >= 0x3041 && cp <= 0x33FF) ||
            (cp >= 0x3400 && cp <= 0x4DBF) ||
            (cp >= 0x4E00 && cp <= 0x9FFF) ||
            (cp >= 0xA000 && cp <= 0xA4CF) ||
            (cp >= 0xAC00 && cp <= 0xD7A3) ||
            (cp >= 0xF900 && cp <= 0xFAFF) ||
            (cp >= 0xFE30 && cp <= 0xFE4F) ||
            (cp >= 0xFF00 && cp <= 0xFF60) ||
            (cp >= 0xFFE0 && cp <= 0xFFE6) ||
            (cp >= 0x1F300 && cp <= 0x1F9FF) ||
            (cp >= 0x20000 && cp <= 0x2FFFD)) return 2;
        return 1;
    }

    std::uint32_t decode_cp(const std::string& s, std::size_t pos, std::size_t len) {
        if (len == 1) return static_cast<unsigned char>(s[pos]);
        std::uint32_t cp = 0;
        if (len == 2) {
            cp = (static_cast<unsigned char>(s[pos]) & 0x1F) << 6;
            cp |= static_cast<unsigned char>(s[pos + 1]) & 0x3F;
        } else if (len == 3) {
            cp = (static_cast<unsigned char>(s[pos]) & 0x0F) << 12;
            cp |= (static_cast<unsigned char>(s[pos + 1]) & 0x3F) << 6;
            cp |= static_cast<unsigned char>(s[pos + 2]) & 0x3F;
        } else if (len == 4) {
            cp = (static_cast<unsigned char>(s[pos]) & 0x07) << 18;
            cp |= (static_cast<unsigned char>(s[pos + 1]) & 0x3F) << 12;
            cp |= (static_cast<unsigned char>(s[pos + 2]) & 0x3F) << 6;
            cp |= static_cast<unsigned char>(s[pos + 3]) & 0x3F;
        }
        return cp;
    }
}

std::string truncate_display(const std::string& s, std::size_t max_width) {
    if (max_width == 0) return std::string();

    std::size_t total = 0;
    {
        std::size_t i = 0;
        while (i < s.size()) {
            std::size_t len = utf8_char_size(s, i);
            if (len == 0 || i + len > s.size()) break;
            total += cp_width(decode_cp(s, i, len));
            i += len;
        }
    }
    if (total <= max_width) return s;

    std::size_t budget = max_width - 1;
    std::size_t width = 0;
    std::size_t i = 0;
    while (i < s.size()) {
        std::size_t len = utf8_char_size(s, i);
        if (len == 0 || i + len > s.size()) break;
        std::size_t w = cp_width(decode_cp(s, i, len));
        if (width + w > budget) {
            return s.substr(0, i) + "…";
        }
        width += w;
        i += len;
    }
    return s;
}

}

// ===== src/Inquire/utils/tty_guard.cpp =====

#ifdef __linux__

#include <atomic>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <termios.h>
#include <unistd.h>

namespace Inquire {

namespace {
    termios g_saved_termios{};
    std::atomic<bool> g_saved_ok{false};
    std::atomic<bool> g_save_done{false};
    std::atomic<bool> g_signal_installed{false};

    void save_once() {
        bool expected = false;
        if (!g_save_done.compare_exchange_strong(expected, true)) return;
        bool ok = (tcgetattr(STDIN_FILENO, &g_saved_termios) == 0);
        g_saved_ok.store(ok);
    }

    void on_fatal_signal(int sig) {
        if (g_saved_ok.load()) {
            tcsetattr(STDIN_FILENO, TCSANOW, &g_saved_termios);
        }
        const char show[] = "\033[?25h";
        ssize_t n = write(STDOUT_FILENO, show, sizeof(show) - 1);
        (void)n;
        std::signal(sig, SIG_DFL);
        std::raise(sig);
    }
}

TtyGuard::TtyGuard() {
    save_once();
    install_signal_handler();
    saved_ok_ = g_saved_ok.load();
}

TtyGuard::~TtyGuard() {
    restore();
}

void TtyGuard::enter_raw() {
    if (!saved_ok_ || active_) return;
    struct termios raw = g_saved_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0) {
        active_ = true;
    }
}

void TtyGuard::restore() {
    if (!saved_ok_ || !active_) return;
    tcsetattr(STDIN_FILENO, TCSANOW, &g_saved_termios);
    active_ = false;
}

void TtyGuard::install_signal_handler() {
    bool expected = false;
    if (!g_signal_installed.compare_exchange_strong(expected, true)) return;
    std::signal(SIGINT,  on_fatal_signal);
    std::signal(SIGTERM, on_fatal_signal);
    std::signal(SIGSEGV, on_fatal_signal);
}

}

#endif

// ===== src/Inquire/inquire.cpp =====


#include <algorithm>
#include <iostream>
#include <sstream>

namespace Inquire {

namespace {

constexpr const char* QUESTION_MARK = "?";
constexpr const char* CURSOR_MARK   = ">";
constexpr const char* SEP_MARK      = "›";
constexpr const char* CHECK_ON      = "[x]";
constexpr const char* CHECK_OFF     = "[ ]";

std::string format_question(const std::string& q) {
    return style::green(QUESTION_MARK) + " " + style::bold(q);
}

std::string format_help(const std::string& help) {
    return style::dim(help);
}

std::string format_error(const std::string& err) {
    return style::red("✗ " + err);
}

bool default_filter_match(const std::string& option, const std::string& input,
                          bool case_sensitive) {
    if (input.empty()) return true;
    if (case_sensitive) return option.find(input) != std::string::npos;
    return icontains(option, input);
}

void utf8_erase_before(std::string& s, std::size_t& cursor) {
    if (cursor == 0) return;
    std::size_t prev = utf8_prev_char_start(s, cursor);
    s.erase(prev, cursor - prev);
    cursor = prev;
}

void utf8_erase_at(std::string& s, std::size_t cursor) {
    if (cursor >= s.size()) return;
    std::size_t len = utf8_char_size(s, cursor);
    s.erase(cursor, len);
}

void utf8_move_left(const std::string& s, std::size_t& cursor) {
    if (cursor > 0) cursor = utf8_prev_char_start(s, cursor);
}

void utf8_move_right(const std::string& s, std::size_t& cursor) {
    if (cursor < s.size()) cursor += utf8_char_size(s, cursor);
}

std::size_t header_prefix_width(const std::string& question) {
    return utf8_display_width(QUESTION_MARK) + 1
         + utf8_display_width(question) + 1
         + utf8_display_width(SEP_MARK) + 1;
}

int compute_scroll(int cursor, int scroll_top, int matches_size, int viewport) {
    if (matches_size <= viewport) return 0;
    if (cursor < scroll_top) scroll_top = cursor;
    if (cursor >= scroll_top + viewport) scroll_top = cursor - viewport + 1;
    if (scroll_top < 0) scroll_top = 0;
    if (scroll_top > matches_size - viewport) scroll_top = matches_size - viewport;
    return scroll_top;
}

}

/* =============================== Select =============================== */

Select::Select(std::string question, std::vector<std::string> options)
    : question_(std::move(question)), options_(std::move(options)) {
    Terminal::init();
    ToUTF8();
}

Select& Select::page_size(int n)         { page_size_ = n > 0 ? n : 1; return *this; }
Select& Select::default_index(int i)     { default_index_ = i; return *this; }
Select& Select::help_message(std::string m) { help_ = std::move(m); return *this; }
Select& Select::case_sensitive(bool on)  { case_sensitive_ = on; return *this; }
Select& Select::filter(std::function<bool(const std::string&, const std::string&)> fn) {
    filter_ = std::move(fn);
    return *this;
}

int Select::do_prompt_() {
    if (options_.empty()) throw EmptyOptionsError();

    Renderer renderer;

    auto match_one = [&](const std::string& option, const std::string& input) {
        return filter_ ? filter_(option, input)
                       : default_filter_match(option, input, case_sensitive_);
    };
    auto recompute_matches = [&](const std::string& input) {
        std::vector<int> m;
        m.reserve(options_.size());
        for (std::size_t i = 0; i < options_.size(); ++i) {
            if (match_one(options_[i], input)) m.push_back(static_cast<int>(i));
        }
        return m;
    };

    std::string input;
    auto matches = recompute_matches(input);

    int cursor = 0;
    if (default_index_ >= 0 && default_index_ < static_cast<int>(options_.size())) {
        for (std::size_t i = 0; i < matches.size(); ++i) {
            if (matches[i] == default_index_) { cursor = static_cast<int>(i); break; }
        }
    }
    int initial_viewport = std::min(page_size_, static_cast<int>(matches.size()));
    int scroll_top = compute_scroll(cursor, 0, static_cast<int>(matches.size()), initial_viewport);

    auto draw = [&]() {
        auto term = Terminal::size();
        std::size_t max_width = term.first > 4 ? static_cast<std::size_t>(term.first - 4) : 1;

        std::vector<std::string> lines;
        lines.push_back(format_question(question_) + " " + style::dim(SEP_MARK) + " " + input);

        int viewport = std::min(page_size_, static_cast<int>(matches.size()));
        if (viewport == 0) {
            lines.push_back(style::dim("  (no matches)"));
        } else {
            for (int i = 0; i < viewport; ++i) {
                int idx = scroll_top + i;
                if (idx >= static_cast<int>(matches.size())) break;
                std::string truncated = truncate_display(options_[matches[idx]], max_width);
                if (idx == cursor) {
                    lines.push_back(style::cyan(std::string(CURSOR_MARK) + " " + truncated));
                } else {
                    lines.push_back("  " + truncated);
                }
            }
        }

        std::ostringstream hint;
        int total = static_cast<int>(matches.size());
        if (total > viewport && viewport > 0) {
            hint << "[" << (cursor + 1) << "/" << total << "]  ";
        }
        hint << (help_.empty()
                 ? "↑↓ navigate · type to filter · enter · esc"
                 : help_);
        lines.push_back(format_help(hint.str()));

        int header_col = static_cast<int>(header_prefix_width(question_)
                                          + utf8_display_width(input)) + 1;
        renderer.draw(lines, /*cursor_line=*/0, /*cursor_col=*/header_col);
    };

    draw();

    while (true) {
        KeyEvent ev = key_catch();
        int viewport = std::min(page_size_, static_cast<int>(matches.size()));
        switch (ev.key) {
            case Key::Up:
                if (!matches.empty()) {
                    cursor = (cursor - 1 + static_cast<int>(matches.size()))
                             % static_cast<int>(matches.size());
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::Down:
                if (!matches.empty()) {
                    cursor = (cursor + 1) % static_cast<int>(matches.size());
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::PageUp:
                if (!matches.empty()) {
                    cursor = std::max(0, cursor - page_size_);
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::PageDown:
                if (!matches.empty()) {
                    cursor = std::min(static_cast<int>(matches.size()) - 1, cursor + page_size_);
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::Home:
                cursor = 0; scroll_top = 0; break;
            case Key::End:
                if (!matches.empty()) {
                    cursor = static_cast<int>(matches.size()) - 1;
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::Backspace: {
                if (!input.empty()) {
                    std::size_t pos = input.size();
                    utf8_erase_before(input, pos);
                    matches = recompute_matches(input);
                    if (cursor >= static_cast<int>(matches.size())) cursor = 0;
                    int vp = std::min(page_size_, static_cast<int>(matches.size()));
                    scroll_top = compute_scroll(cursor, 0, static_cast<int>(matches.size()), vp);
                }
                break;
            }
            case Key::CtrlU:
                if (!input.empty()) {
                    input.clear();
                    matches = recompute_matches(input);
                    cursor = 0; scroll_top = 0;
                }
                break;
            case Key::Enter:
                if (matches.empty()) break;
                renderer.finalize(format_question(question_) + " " +
                                  style::cyan(options_[matches[cursor]]));
                return matches[cursor];
            case Key::Escape:
            case Key::CtrlC:
                renderer.finalize(format_question(question_) + " " + style::red("<cancelled>"));
                throw CancelledError();
            case Key::CtrlD:
                renderer.finalize(format_question(question_) + " " + style::red("<interrupted>"));
                throw InterruptedError();
            case Key::Char:
            case Key::Space: {
                input += (ev.key == Key::Space) ? std::string(1, ' ') : ev.text;
                matches = recompute_matches(input);
                if (cursor >= static_cast<int>(matches.size())) cursor = 0;
                int vp = std::min(page_size_, static_cast<int>(matches.size()));
                scroll_top = compute_scroll(cursor, 0, static_cast<int>(matches.size()), vp);
                break;
            }
            default:
                break;
        }
        draw();
    }
}

std::string Select::prompt() { return options_[do_prompt_()]; }
int Select::prompt_index()   { return do_prompt_(); }

Result<std::string> Select::try_prompt() {
    try { return Result<std::string>::ok(prompt()); }
    catch (const InquireError& e) {
        return Result<std::string>::err(e.code(), e.what());
    }
}

/* ============================ MultiSelect ============================ */

MultiSelect::MultiSelect(std::string question, std::vector<std::string> options)
    : question_(std::move(question)), options_(std::move(options)) {
    Terminal::init();
    ToUTF8();
}

MultiSelect& MultiSelect::page_size(int n)         { page_size_ = n > 0 ? n : 1; return *this; }
MultiSelect& MultiSelect::min_selected(int n)      { min_selected_ = n; return *this; }
MultiSelect& MultiSelect::max_selected(int n)      { max_selected_ = n; return *this; }
MultiSelect& MultiSelect::case_sensitive(bool on)  { case_sensitive_ = on; return *this; }
MultiSelect& MultiSelect::default_indices(std::vector<int> idx) {
    defaults_ = std::move(idx);
    return *this;
}
MultiSelect& MultiSelect::help_message(std::string m) { help_ = std::move(m); return *this; }

std::vector<int> MultiSelect::do_prompt_() {
    if (options_.empty()) throw EmptyOptionsError();

    Renderer renderer;
    std::vector<bool> selected(options_.size(), false);
    for (int i : defaults_) {
        if (i >= 0 && i < static_cast<int>(options_.size())) selected[i] = true;
    }

    auto recompute_matches = [&](const std::string& input) {
        std::vector<int> m;
        m.reserve(options_.size());
        for (std::size_t i = 0; i < options_.size(); ++i) {
            if (default_filter_match(options_[i], input, case_sensitive_)) {
                m.push_back(static_cast<int>(i));
            }
        }
        return m;
    };

    auto count_selected = [&]() {
        int n = 0; for (bool b : selected) if (b) ++n; return n;
    };

    std::string input;
    std::string error;
    auto matches = recompute_matches(input);
    int cursor = 0;
    int scroll_top = 0;

    auto draw = [&](const std::string& err) {
        auto term = Terminal::size();
        std::size_t max_width = term.first > 8 ? static_cast<std::size_t>(term.first - 8) : 1;

        std::vector<std::string> lines;
        lines.push_back(format_question(question_) + " " + style::dim(SEP_MARK) + " " + input);

        int viewport = std::min(page_size_, static_cast<int>(matches.size()));
        if (viewport == 0) {
            lines.push_back(style::dim("  (no matches)"));
        } else {
            for (int i = 0; i < viewport; ++i) {
                int idx = scroll_top + i;
                if (idx >= static_cast<int>(matches.size())) break;
                int orig = matches[idx];
                std::string box = selected[orig] ? style::green(CHECK_ON) : std::string(CHECK_OFF);
                std::string truncated = truncate_display(options_[orig], max_width);
                std::string line;
                if (idx == cursor) {
                    line = style::cyan(std::string(CURSOR_MARK) + " ") + box + " "
                           + style::cyan(truncated);
                } else {
                    line = "  " + box + " " + truncated;
                }
                lines.push_back(line);
            }
        }

        std::ostringstream hint;
        int total = static_cast<int>(matches.size());
        if (total > viewport && viewport > 0) {
            hint << "[" << (cursor + 1) << "/" << total << "]  ";
        }
        hint << "selected: " << count_selected();
        if (max_selected_ >= 0) hint << "/" << max_selected_;
        hint << "  ";
        hint << (help_.empty()
                 ? "↑↓ · space toggle · enter · esc"
                 : help_);
        lines.push_back(format_help(hint.str()));

        if (!err.empty()) lines.push_back(format_error(err));

        int header_col = static_cast<int>(header_prefix_width(question_)
                                          + utf8_display_width(input)) + 1;
        renderer.draw(lines, /*cursor_line=*/0, /*cursor_col=*/header_col);
    };

    draw(error);

    while (true) {
        KeyEvent ev = key_catch();
        int viewport = std::min(page_size_, static_cast<int>(matches.size()));
        switch (ev.key) {
            case Key::Up:
                if (!matches.empty()) {
                    cursor = (cursor - 1 + static_cast<int>(matches.size()))
                             % static_cast<int>(matches.size());
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::Down:
                if (!matches.empty()) {
                    cursor = (cursor + 1) % static_cast<int>(matches.size());
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::PageUp:
                if (!matches.empty()) {
                    cursor = std::max(0, cursor - page_size_);
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::PageDown:
                if (!matches.empty()) {
                    cursor = std::min(static_cast<int>(matches.size()) - 1, cursor + page_size_);
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::Home: cursor = 0; scroll_top = 0; break;
            case Key::End:
                if (!matches.empty()) {
                    cursor = static_cast<int>(matches.size()) - 1;
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::Space:
                if (!matches.empty()) {
                    int orig = matches[cursor];
                    if (!selected[orig] && max_selected_ >= 0
                        && count_selected() >= max_selected_) {
                        error = "cannot select more than " + std::to_string(max_selected_);
                    } else {
                        selected[orig] = !selected[orig];
                        error.clear();
                    }
                }
                break;
            case Key::Backspace: {
                if (!input.empty()) {
                    std::size_t pos = input.size();
                    utf8_erase_before(input, pos);
                    matches = recompute_matches(input);
                    if (cursor >= static_cast<int>(matches.size())) cursor = 0;
                    int vp = std::min(page_size_, static_cast<int>(matches.size()));
                    scroll_top = compute_scroll(cursor, 0, static_cast<int>(matches.size()), vp);
                }
                break;
            }
            case Key::CtrlU:
                if (!input.empty()) {
                    input.clear();
                    matches = recompute_matches(input);
                    cursor = 0; scroll_top = 0;
                }
                break;
            case Key::Enter: {
                int n = count_selected();
                if (n < min_selected_) {
                    error = "select at least " + std::to_string(min_selected_) + " option(s)";
                    break;
                }
                std::vector<int> result;
                for (std::size_t i = 0; i < selected.size(); ++i)
                    if (selected[i]) result.push_back(static_cast<int>(i));
                std::ostringstream summary;
                summary << format_question(question_) << " ";
                if (result.empty()) {
                    summary << style::dim("(none)");
                } else {
                    for (std::size_t i = 0; i < result.size(); ++i) {
                        if (i) summary << style::dim(", ");
                        summary << style::cyan(options_[result[i]]);
                    }
                }
                renderer.finalize(summary.str());
                return result;
            }
            case Key::Escape:
            case Key::CtrlC:
                renderer.finalize(format_question(question_) + " " + style::red("<cancelled>"));
                throw CancelledError();
            case Key::CtrlD:
                renderer.finalize(format_question(question_) + " " + style::red("<interrupted>"));
                throw InterruptedError();
            case Key::Char: {
                input += ev.text;
                matches = recompute_matches(input);
                if (cursor >= static_cast<int>(matches.size())) cursor = 0;
                int vp = std::min(page_size_, static_cast<int>(matches.size()));
                scroll_top = compute_scroll(cursor, 0, static_cast<int>(matches.size()), vp);
                break;
            }
            default: break;
        }
        draw(error);
    }
}

std::vector<std::string> MultiSelect::prompt() {
    auto idx = do_prompt_();
    std::vector<std::string> out;
    out.reserve(idx.size());
    for (int i : idx) out.push_back(options_[i]);
    return out;
}

std::vector<int> MultiSelect::prompt_indices() { return do_prompt_(); }

Result<std::vector<std::string>> MultiSelect::try_prompt() {
    try { return Result<std::vector<std::string>>::ok(prompt()); }
    catch (const InquireError& e) {
        return Result<std::vector<std::string>>::err(e.code(), e.what());
    }
}

/* =============================== Text =============================== */

Text::Text(std::string question) : question_(std::move(question)) {
    Terminal::init();
    ToUTF8();
}

Text& Text::default_value(std::string v)   { default_ = std::move(v); return *this; }
Text& Text::placeholder(std::string p)     { placeholder_ = std::move(p); return *this; }
Text& Text::help_message(std::string m)    { help_ = std::move(m); return *this; }
Text& Text::add_validator(Validator v)     { validators_.push_back(std::move(v)); return *this; }

std::string Text::do_prompt_() {
    Renderer renderer;
    std::string input;
    std::size_t cursor_byte = 0;
    std::string error;

    auto draw = [&]() {
        std::vector<std::string> lines;
        std::string body;
        std::size_t cursor_offset = 0;
        if (input.empty() && !default_.empty()) {
            body = style::dim("(" + default_ + ")");
        } else if (input.empty() && !placeholder_.empty()) {
            body = style::dim(placeholder_);
        } else {
            body = input;
            cursor_offset = utf8_display_width(input.substr(0, cursor_byte));
        }
        lines.push_back(format_question(question_) + " " + style::dim(SEP_MARK) + " " + body);
        if (!help_.empty()) lines.push_back(format_help(help_));
        if (!error.empty()) lines.push_back(format_error(error));

        int header_col = static_cast<int>(header_prefix_width(question_) + cursor_offset) + 1;
        renderer.draw(lines, /*cursor_line=*/0, /*cursor_col=*/header_col);
    };

    draw();

    while (true) {
        KeyEvent ev = key_catch();
        switch (ev.key) {
            case Key::Char:
                input.insert(cursor_byte, ev.text);
                cursor_byte += ev.text.size();
                error.clear();
                break;
            case Key::Space:
                input.insert(cursor_byte, 1, ' ');
                cursor_byte += 1;
                error.clear();
                break;
            case Key::Backspace:
                utf8_erase_before(input, cursor_byte);
                error.clear();
                break;
            case Key::Delete:
                utf8_erase_at(input, cursor_byte);
                error.clear();
                break;
            case Key::Left:
                utf8_move_left(input, cursor_byte);
                break;
            case Key::Right:
                utf8_move_right(input, cursor_byte);
                break;
            case Key::Home:
            case Key::CtrlA:
                cursor_byte = 0;
                break;
            case Key::End:
            case Key::CtrlE:
                cursor_byte = input.size();
                break;
            case Key::CtrlU:
                input.clear();
                cursor_byte = 0;
                error.clear();
                break;
            case Key::Enter: {
                std::string val = input.empty() ? default_ : input;
                try {
                    for (auto& v : validators_) v(val);
                } catch (const ValidationError& e) {
                    error = e.what();
                    draw();
                    continue;
                }
                renderer.finalize(format_question(question_) + " " +
                                  (val.empty() ? style::dim("(empty)") : style::cyan(val)));
                return val;
            }
            case Key::Escape:
            case Key::CtrlC:
                renderer.finalize(format_question(question_) + " " + style::red("<cancelled>"));
                throw CancelledError();
            case Key::CtrlD:
                renderer.finalize(format_question(question_) + " " + style::red("<interrupted>"));
                throw InterruptedError();
            default:
                break;
        }
        draw();
    }
}

std::string Text::prompt() { return do_prompt_(); }

Result<std::string> Text::try_prompt() {
    try { return Result<std::string>::ok(prompt()); }
    catch (const InquireError& e) {
        return Result<std::string>::err(e.code(), e.what());
    }
}

/* ============================== Password ============================== */

Password::Password(std::string question) : question_(std::move(question)) {
    Terminal::init();
    ToUTF8();
}

Password& Password::mask_char(char c)             { mask_ = c; return *this; }
Password& Password::show_typing(bool on)          { show_ = on; return *this; }
Password& Password::help_message(std::string m)   { help_ = std::move(m); return *this; }
Password& Password::add_validator(Validator v)    { validators_.push_back(std::move(v)); return *this; }

std::string Password::do_prompt_() {
    Renderer renderer;
    std::string input;
    std::string error;

    auto draw = [&]() {
        std::vector<std::string> lines;
        std::string visible;
        std::size_t cursor_offset;
        if (show_) {
            visible = input;
            cursor_offset = utf8_display_width(input);
        } else {
            std::size_t chars = utf8_char_count(input);
            visible.assign(chars, mask_);
            cursor_offset = chars;
        }
        lines.push_back(format_question(question_) + " " + style::dim(SEP_MARK) + " " + visible);
        if (!help_.empty()) lines.push_back(format_help(help_));
        if (!error.empty()) lines.push_back(format_error(error));

        int header_col = static_cast<int>(header_prefix_width(question_) + cursor_offset) + 1;
        renderer.draw(lines, /*cursor_line=*/0, /*cursor_col=*/header_col);
    };

    draw();

    while (true) {
        KeyEvent ev = key_catch();
        switch (ev.key) {
            case Key::Char:
                input += ev.text;
                error.clear();
                break;
            case Key::Space:
                input.push_back(' ');
                error.clear();
                break;
            case Key::Backspace: {
                std::size_t pos = input.size();
                utf8_erase_before(input, pos);
                error.clear();
                break;
            }
            case Key::CtrlU:
                input.clear();
                error.clear();
                break;
            case Key::Enter: {
                try {
                    for (auto& v : validators_) v(input);
                } catch (const ValidationError& e) {
                    error = e.what();
                    draw();
                    continue;
                }
                std::string final_mask(8, mask_);
                renderer.finalize(format_question(question_) + " " + style::cyan(final_mask));
                return input;
            }
            case Key::Escape:
            case Key::CtrlC:
                renderer.finalize(format_question(question_) + " " + style::red("<cancelled>"));
                throw CancelledError();
            case Key::CtrlD:
                renderer.finalize(format_question(question_) + " " + style::red("<interrupted>"));
                throw InterruptedError();
            default:
                break;
        }
        draw();
    }
}

std::string Password::prompt() { return do_prompt_(); }

Result<std::string> Password::try_prompt() {
    try { return Result<std::string>::ok(prompt()); }
    catch (const InquireError& e) {
        return Result<std::string>::err(e.code(), e.what());
    }
}

/* ============================== Confirm ============================== */

Confirm::Confirm(std::string question) : question_(std::move(question)) {
    Terminal::init();
    ToUTF8();
}

Confirm& Confirm::default_value(bool b) {
    default_ = b;
    has_default_ = true;
    return *this;
}

Confirm& Confirm::help_message(std::string m) { help_ = std::move(m); return *this; }

bool Confirm::prompt() {
    Renderer renderer;

    auto draw = [&]() {
        std::vector<std::string> lines;
        std::string yn = has_default_ ? (default_ ? "(Y/n)" : "(y/N)") : "(y/n)";
        lines.push_back(format_question(question_) + " " + style::dim(yn));
        if (!help_.empty()) lines.push_back(format_help(help_));
        renderer.draw(lines);
    };

    draw();

    while (true) {
        KeyEvent ev = key_catch();
        if (ev.key == Key::Enter) {
            if (!has_default_) continue;
            renderer.finalize(format_question(question_) + " " +
                              (default_ ? style::cyan("Yes") : style::red("No")));
            return default_;
        }
        if (ev.key == Key::Escape || ev.key == Key::CtrlC) {
            renderer.finalize(format_question(question_) + " " + style::red("<cancelled>"));
            throw CancelledError();
        }
        if (ev.key == Key::CtrlD) {
            renderer.finalize(format_question(question_) + " " + style::red("<interrupted>"));
            throw InterruptedError();
        }
        if (ev.is_char()) {
            const std::string& s = ev.text;
            if (s == "y" || s == "Y") {
                renderer.finalize(format_question(question_) + " " + style::cyan("Yes"));
                return true;
            }
            if (s == "n" || s == "N") {
                renderer.finalize(format_question(question_) + " " + style::red("No"));
                return false;
            }
        }
    }
}

Result<bool> Confirm::try_prompt() {
    try { return Result<bool>::ok(prompt()); }
    catch (const InquireError& e) {
        return Result<bool>::err(e.code(), e.what());
    }
}

}

#endif // INQUIRE_IMPL_DEFINED_
#endif // INQUIRE_IMPLEMENTATION
