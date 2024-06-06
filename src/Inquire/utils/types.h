#pragma once

#include <string>
#include <functional>


enum SPECIAL_KEY{
    UP,
    DOWN,
    RIGHT,
    LEFT,
    ENTER,
    ESC,
    SPACE,
    BACKSPACE,
    UNKNOWN
};

struct KeyResult {
    SPECIAL_KEY key;
    std::string str;
    bool isKey;

    KeyResult(SPECIAL_KEY key) : key(key), isKey(true) {}
    KeyResult(std::string str) : str(str), isKey(false) {}
};
