#pragma once

#include "types.h"
#include <string>

namespace Inquire {

KeyEvent key_catch();

std::size_t utf8_display_width(const std::string& s) noexcept;
std::size_t utf8_char_count(const std::string& s) noexcept;
std::size_t utf8_char_size(const std::string& s, std::size_t byte_pos) noexcept;
std::size_t utf8_prev_char_start(const std::string& s, std::size_t byte_pos) noexcept;

bool icontains(const std::string& haystack, const std::string& needle) noexcept;

}
