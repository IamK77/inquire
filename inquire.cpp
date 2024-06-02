#include "inquire.hpp"

/* ---------------------------Select-------------------------------*/

Select::Select(std::string prompt_str, std::vector<std::string> options, int MAX_OPTION_LINE) : prompt_str(prompt_str), options(options), option_size(options.size()) {

    if (((cursor.now_screen_x_y().second - 1) - cursor.now_cursor_x_y().second) < (options.size() + 1)) {
        for (int i = 0; i < (options.size() + 1); ++i) {
            std::cout << std::endl;

        }
        std::pair<int, int> xy = cursor.now_cursor_x_y();
        cursor.set_cursor_Pos(xy.first, xy.second - (options.size() + 1));
    }
    std::cout << green("? ") << prompt_str << " : ";

    ori_cursorPos = cursor.now_cursor_x_y();

    if (options.size() == 0) {
        std::cout << red("No options available") << std::endl;
        return;
    }
}

std::string Select::prompt() {
    flush_display();
    while (true) {
        KeyResult key = key_catch();
        if (key.isKey) {
            switch (key.key) {
                case SPECIAL_KEY::UP:
                    up(); 
                    break;
                case SPECIAL_KEY::DOWN:
                    down();
                    break;
                case SPECIAL_KEY::RIGHT:
                    right();
                    break;
                case SPECIAL_KEY::LEFT:
                    left();
                    break;
                case SPECIAL_KEY::ENTER:
                    flush_display(true);
                    flush_result(cyan(options[selected]));
                    return options[selected];
                case SPECIAL_KEY::BACKSPACE:
                    if (cursorPos > 0) {
                        cursorPos--;
                        input.erase(cursorPos, 1);
                        flush_input();
                        flush_display();
                    }
                    break;
                case SPECIAL_KEY::ESC:
                    flush_display(true);
                    flush_result(red("<canceled>"));
                    return "";
                default:
                    break;
            }
        } else {
            input_insert(key.str);
            flush_input();
            flush_display();
    }}
};

void Select::flush_display(bool is_clear) {
    if (is_clear) {
        cursor.clsline(ori_cursorPos.second + 1, option_size + 1);
    } else {

        cursor.clsline(ori_cursorPos.second + 1, option_size + 1);

        for (int i = 0; i < options.size(); ++i) {
        if (options[i].find(input) != std::string::npos) { // 如果菜单项包含用户的输入
            if (i == selected) {
                std::cout << cyan("> ") << cyan(options[i]) << std::endl; // 高亮显示当前选中的菜单项
            } else {
                std::cout << "  " << options[i] << std::endl; // 不高亮显示其他菜单项
            }
        }
        }
        std::cout << cyan("[↑↓ to move, enter to select, type to filter]");

        cursor.set_cursor_Pos(ori_cursorPos.first + cursorPos, ori_cursorPos.second);
    }

}

void Select::flush_input() {
    cursor.clsback(input.size() + 1, ori_cursorPos.first, ori_cursorPos.second);
    cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
    std::cout << input;
    cursor.cursor_move(cursorPos - input.size(), 0);
}

void Select::flush_result(std::string result) {
    while (input.size() > 0) {
        input.pop_back();
        flush_input();
        if (input.size() > 0 && input.size() < 10) {
        Sleep(100 / input.size());
        }
    }
    cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
    for (int i = 0; i < result.size(); ++i) {
        std::cout << result[i];
        if (result.size() < 20 && (result.size() - i) > 0) {
            Sleep(100 / (result.size() - i));
        }
    }
    std::cout << std::endl;
}

void Select::input_insert(std::string ch) {
    input.insert(cursorPos, ch);
    cursorPos++;
}

void Select::up() {
    selected = (selected - 1 + option_size) % option_size;
    flush_display();
}

void Select::down() {
    selected = (selected + 1) % option_size;
    flush_display();
}

void Select::right() {
    if (cursorPos < input.size()) {
        cursorPos++;
        cursor.cursor_move(1, 0);
    }

}

void Select::left() {
    if (cursorPos > 0) {
        cursorPos--;
        cursor.cursor_move(-1, 0);
    }
}