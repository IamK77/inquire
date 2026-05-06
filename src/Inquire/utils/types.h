#pragma once

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
