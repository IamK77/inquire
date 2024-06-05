#include <windows.h>
#include <iostream>

enum class SPECIAL_KEY {
    UP, DOWN, LEFT, RIGHT, ENTER, ESC, SPACE, BACKSPACE, UNKNOWN
};

SPECIAL_KEY key_catch() {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); 
    INPUT_RECORD irInputRecord;
    DWORD dwEventsRead;
    KEY_EVENT_RECORD key;

    while (true) {
        ReadConsoleInput(hStdin, &irInputRecord, 1, &dwEventsRead);
        if (irInputRecord.EventType == KEY_EVENT && irInputRecord.Event.KeyEvent.bKeyDown) {
            key = irInputRecord.Event.KeyEvent;
            switch (key.wVirtualKeyCode) {
                case VK_UP: return SPECIAL_KEY::UP;
                case VK_DOWN: return SPECIAL_KEY::DOWN;
                case VK_LEFT: return SPECIAL_KEY::LEFT;
                case VK_RIGHT: return SPECIAL_KEY::RIGHT;
                case VK_RETURN: return SPECIAL_KEY::ENTER;
                case VK_ESCAPE: return SPECIAL_KEY::ESC;
                case VK_SPACE: return SPECIAL_KEY::SPACE;
                case VK_BACK: return SPECIAL_KEY::BACKSPACE;
                default: return SPECIAL_KEY::UNKNOWN;
            }
        }
    }
}

int main() {
    SPECIAL_KEY result = key_catch();
    std::cout << "Key captured!" << std::endl;
    switch (result)
    {
    case SPECIAL_KEY::UP:
        std::cout << "UP" << std::endl;
        break;
    case SPECIAL_KEY::DOWN:
        std::cout << "DOWN" << std::endl;
        break;
    case SPECIAL_KEY::LEFT:
        std::cout << "LEFT" << std::endl;
        break;
    case SPECIAL_KEY::RIGHT:
        std::cout << "RIGHT" << std::endl;
        break;
    case SPECIAL_KEY::ENTER:
        std::cout << "ENTER" << std::endl;
        break;
    case SPECIAL_KEY::ESC:
        std::cout << "ESC" << std::endl;
        break;
    case SPECIAL_KEY::SPACE:
        std::cout << "SPACE" << std::endl;
        break;
    case SPECIAL_KEY::BACKSPACE:
        std::cout << "BACKSPACE" << std::endl;
        break;
    }
    return 0;
}
