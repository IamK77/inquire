#include "colorful.hpp"

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
