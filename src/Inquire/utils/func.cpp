#include "func.h"

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
    #include "tty_guard.hpp"
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
