#pragma once

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
