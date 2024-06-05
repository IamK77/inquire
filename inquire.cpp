#include "inquire.hpp"


#ifdef __linux__

#include <time.h>

struct timespec ts;

void sleep(int ms) {
    ts.tv_sec = 0;
    ts.tv_nsec = ms * 1000000; // Convert milliseconds to nanoseconds
    nanosleep(&ts, NULL);
}

#endif



/* ---------------------------Select-------------------------------*/

Select::Select(std::string prompt_str, std::vector<std::string> options, int MAX_OPTION_LINE) : prompt_str(prompt_str), options(options), option_size(options.size()), preselected(options.size()) {

    #ifdef _WIN32
        if (((cursor.now_screen_x_y().second - 1) - cursor.now_cursor_x_y().second) < (options.size() + 1)) {
            for (int i = 0; i < (options.size() + 1); ++i) {
                std::cout << std::endl;
            }

            std::pair<int, int> xy = cursor.now_cursor_x_y();
            cursor.set_cursor_Pos(xy.first, xy.second - (options.size() + 1));
        }
    #endif

    #ifdef __linux__
        if (((cursor.now_screen_x_y().second - 1) - cursor.now_cursor_x_y().second) < (options.size() + 1)) {
            std::cout << "\033[s";
            for (int i = 0; i < (options.size() + 1); ++i) {
                std::cout << std::endl;
            }
            std::cout << "\033[u";

            std::pair<int, int> xy = cursor.now_cursor_x_y();
            cursor.set_cursor_Pos(xy.first, xy.second - (options.size() + 2));
        }
    #endif

    std::cout << green("? ") << prompt_str << " : " << std::flush;

    // #ifdef __linux__
    //     std::pair<int, int> xy = cursor.now_cursor_x_y();
    //     cursor.coutxy(xy.first, xy.second + 10, "X: " + std::to_string(xy.first) + "Y: " + std::to_string(xy.second));
    //     cursor.set_cursor_Pos(xy.first, xy.second);
    // #endif

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

        #ifdef __linux
            std::cout << std::endl;
        #endif

        cursor.clsline(ori_cursorPos.second + 1, option_size + 1);

        std::vector<std::string> predisplays;

        for (int i = 0; i < options.size(); ++i) {
            if (options[i].find(input) != std::string::npos) {
                predisplays.push_back(options[i]);
            }
        }

        preselected = predisplays.size();

        if (selected >= preselected) {
            selected = 0;
        }

        for (int i = 0; i < predisplays.size(); ++i) {
            if (i == selected) {
                std::cout << cyan("> ") << cyan(predisplays[i]) << std::endl;
            } else {
                std::cout << "  " << predisplays[i] << std::endl;
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

        #ifdef _WIN32
            Sleep(100 / input.size());
        #endif

        #ifdef __linux__
            ts.tv_sec = 0;
            ts.tv_nsec = (100 / (input.size())) * 1000000; // Convert milliseconds to nanoseconds
            nanosleep(&ts, NULL);
        #endif

        }
    }
    cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
    for (int i = 0; i < result.size(); ++i) {
        std::cout << result[i];
        if (result.size() < 20 && (result.size() - i) > 0) {
            #ifdef _WIN32
                Sleep(100 / (result.size() - i));
            #endif

            #ifdef __linux__
                ts.tv_sec = 0;
                ts.tv_nsec = (100 / (result.size() - i)) * 1000000; // Convert milliseconds to nanoseconds
                nanosleep(&ts, NULL);
            #endif

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


/* ---------------------------Text-------------------------------*/

Text::Text(std::string prompt_str) : prompt_str(prompt_str) {
    std::cout << green("? ") << prompt_str << " : ";
}

std::string Text::prompt() {
    std::string input;
    std::pair<int, int> ori_cursorPos = cursor.now_cursor_x_y();
    std::getline(std::cin, input);
    cursor.clsback(input.size() + 1, ori_cursorPos.first, ori_cursorPos.second);
    cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
    std::cout << cyan(input) << std::endl;
    return input;
}


/* ---------------------------Password-------------------------------*/

Password::Password(std::string prompt_str) : prompt_str(prompt_str) {
    std::cout << green("? ") << prompt_str << " : ";
}

std::string Password::prompt() {
    std::string input;
    std::pair<int, int> ori_cursorPos = cursor.now_cursor_x_y();
    while (true) {
        KeyResult key = key_catch();
        if (key.isKey) {
            switch (key.key) {
                case SPECIAL_KEY::ENTER:
                    cursor.clsback(input.size() + 1, ori_cursorPos.first, ori_cursorPos.second);
                    cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
                    std::cout << cyan("********") << std::endl;
                    return input;
                case SPECIAL_KEY::BACKSPACE:
                    if (input.size() > 0) {
                        input.pop_back();
                        cursor.clsback(1, cursor.now_cursor_x_y().first, cursor.now_cursor_x_y().second);
                    }
                    break;
                case SPECIAL_KEY::ESC:
                    cursor.clsback(input.size() + 1, ori_cursorPos.first, ori_cursorPos.second);
                    cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
                    std::cout << red("<canceled>") << std::endl;
                    return "";
                default:
                    break;
            }
        } else {
            input += key.str;
            std::cout << "*";
        }
    }
    return input;

}


/* ---------------------------Confirm-------------------------------*/

Confirm::Confirm(std::string prompt_str) : prompt_str(prompt_str) {
    std::cout << green("? ") << prompt_str << " [Enter/ESC] ";
}

bool Confirm::prompt() {
    std::pair<int, int> ori_cursorPos = cursor.now_cursor_x_y();
    std::cout << gray("Enter: Yes, ESC: No");
    cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
    while (true) {
        KeyResult key = key_catch();
        if (key.isKey) {
            switch (key.key) {
                case SPECIAL_KEY::ENTER:
                    cursor.clsback(19, ori_cursorPos.first, ori_cursorPos.second);
                    cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
                    cursor.clsback(19, cursor.now_cursor_x_y().first, cursor.now_cursor_x_y().second);
                    std::cout << cyan("Yes") << std::endl;
                    return true;
                case SPECIAL_KEY::ESC:
                    cursor.clsback(1, ori_cursorPos.first, ori_cursorPos.second);
                    cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
                    std::cout << red("No") << std::endl;
                    return false;
                default:
                    break;
            }
        }
    }
    return false;
}


/* ---------------------------MultiSelect-------------------------------*/




