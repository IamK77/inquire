#include "renderer.hpp"

#include "console.hpp"
#include "func.h"

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
