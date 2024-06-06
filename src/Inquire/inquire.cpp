#include "inquire.hpp"


/* ---------------------------Select-------------------------------*/

Inquire::Select::Select(std::string prompt_str, std::vector<std::string> options, int MAX_OPTION_LINE) : prompt_str(prompt_str), options(options), option_size(options.size()), preselected(options.size()) {

    ToUTF8();

    #ifdef _WIN32
        if (((cursor.now_screen_x_y().second - 1) - cursor.now_cursor_x_y().second) < (options.size() + 1)) {
            for (int i = 0; i < (options.size() + 1); ++i) {
                std::cout << std::endl;
            }

            std::pair<int, int> xy = cursor.now_cursor_x_y();
            cursor.set_cursor_Pos(xy.first, xy.second - (options.size() + 1));
        }
    #endif

    std::cout << green("? ") << prompt_str << " : " << std::flush;

    ori_cursorPos = cursor.now_cursor_x_y();

    if (options.size() == 0) {
        std::cout << red("No options available") << std::endl;
        return;
    }
}

std::string Inquire::Select::prompt() {
    flush_display();
    while (true) {
        KeyResult key = Inquire::key_catch();
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
                        #ifdef __linux__
                            temp = cursorPos;
                        #endif

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

void Inquire::Select::flush_display(bool is_clear) {
    if (is_clear) {
        #ifdef _WIN32
            cursor.clsline(ori_cursorPos.second + 1, option_size + 1);
        #endif
        #ifdef __linux__
            std::cout << std::endl;
            for(int i = 0; i < option_size; ++i) {
                std::cout << "\033[K" << std::endl;
            }
            std::cout << "\033[K" << std::flush;
            cursor.cursor_move(prompt_str.size() + 5, - option_size - 1);
        #endif

    } else {

        #ifdef __linux
            std::cout << std::endl;
        #endif

        #ifdef _WIN32
            cursor.clsline(ori_cursorPos.second + 1, option_size + 1);
        #endif

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

        #ifdef __linux__
            for(int i = 0; i < option_size; ++i) {
                std::cout << "\033[K" << std::endl;
            }
            std::cout << "\033[K" << std::flush;
            cursor.cursor_move(0, - option_size);
        #endif

        for (int i = 0; i < predisplays.size(); ++i) {
            if (i == selected) {
                std::cout << cyan("> ") << cyan(predisplays[i]) << std::endl;
            } else {
                std::cout << "  " << predisplays[i] << std::endl;
            }
        }

        std::cout << cyan("[↑↓ to move, enter to select, type to filter]");

        #ifdef _WIN32
            cursor.set_cursor_Pos(ori_cursorPos.first + cursorPos, ori_cursorPos.second);
        #endif

        #ifdef __linux__
            std::cout << "\r" << std::flush;
            cursor.cursor_move(ori_cursorPos.first + cursorPos, -preselected - 1);
        #endif
    }

}

void Inquire::Select::flush_input() {
    #ifdef _WIN32
        cursor.clsback(input.size() + 1, ori_cursorPos.first, ori_cursorPos.second);
        cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
        std::cout << input;
        cursor.cursor_move(cursorPos - input.size(), 0);
    #endif

    #ifdef __linux__
        cursor.cursor_move(-(temp), 0);
        for(int i = 0; i <= input.size(); ++i) {
            std::cout << " ";
        }
        cursor.cursor_move(-input.size() - 1, 0);
        std::cout << input << std::flush;
        cursor.cursor_move(cursorPos - input.size(), 0);
    #endif
}

void Inquire::Select::flush_result(std::string result) {

    #ifdef _WIN32
        while (input.size() > 0) {
            input.pop_back();
            flush_input();
            if (input.size() > 0 && input.size() < 10) {
                Sleep(100 / input.size());
            }
        }
        cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
        
    #endif

    #ifdef __linux__
        for(int i = 0; i <= input.size(); ++i) {
            std::cout << " ";
        }
        cursor.cursor_move(- input.size() - 1, 0);
    #endif

    for (int i = 0; i < result.size(); ++i) {
        std::cout << result[i];
        if (result.size() < 20 && (result.size() - i) > 0) {
            #ifdef _WIN32
                Sleep(100 / (result.size() - i));
            #endif
        }
    }
    std::cout << std::endl;
}

void Inquire::Select::input_insert(std::string ch) {
    input.insert(cursorPos, ch);
    #ifdef __linux__
        temp = cursorPos;
    #endif

    cursorPos++;
}

void Inquire::Select::up() {
    selected = (selected - 1 + option_size) % option_size;
    flush_display();
}

void Inquire::Select::down() {
    selected = (selected + 1) % option_size;
    flush_display();
}

void Inquire::Select::right() {
    if (cursorPos < input.size()) {
        cursorPos++;
        cursor.cursor_move(1, 0);
    }
}

void Inquire::Select::left() {
    if (cursorPos > 0) {
        cursorPos--;
        cursor.cursor_move(-1, 0);
    }
}


/* ---------------------------Text-------------------------------*/

Inquire::Text::Text(std::string prompt_str) : prompt_str(prompt_str) {

    ToUTF8();

    std::cout << green("? ") << prompt_str << " : ";
}

std::string Inquire::Text::prompt() {

    #ifdef _WIN32
        std::pair<int, int> ori_cursorPos = cursor.now_cursor_x_y();
        std::getline(std::cin, input);
        cursor.clsback(input.size() + 1, ori_cursorPos.first, ori_cursorPos.second);
        cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
    #endif

    #ifdef __linux__
        if (std::cin.peek() == '\n') { 
            std::cin.ignore();
        }
        std::getline(std::cin, input);
        cursor.cursor_move((prompt_str.size() + 5), -1);
        for(int i = 0; i <= input.size(); ++i) { std::cout << " "; }
        cursor.cursor_move(-input.size() - 1, 0);
    #endif

    std::cout << cyan(input) << std::endl;
    return input;
}


/* ---------------------------Password-------------------------------*/

Inquire::Password::Password(std::string prompt_str) : prompt_str(prompt_str) {

    ToUTF8();

    std::cout << green("? ") << prompt_str << " : ";
}

std::string Inquire::Password::prompt() {
    std::string input;

    #ifdef _WIN32

    std::pair<int, int> ori_cursorPos = cursor.now_cursor_x_y();
    while (true) {
        KeyResult key = Inquire::key_catch();
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

    #endif


    #ifdef __linux__

    while (true) {
        std::cout << std::flush;
        KeyResult key = Inquire::key_catch();
        if (key.isKey) {
            switch (key.key) {
                case SPECIAL_KEY::ENTER:
                    cursor.cursor_move(-input.size(), 0);
                    for(int i = 0; i <= input.size(); ++i) { std::cout << " "; }
                    cursor.cursor_move(-input.size() - 1, 0);
                    std::cout << cyan("********") << std::endl;
                    return input;
                case SPECIAL_KEY::BACKSPACE:
                    if (input.size() > 0) {
                        input.pop_back();
                        cursor.cursor_move(-1, 0);
                        std::cout << " ";
                        cursor.cursor_move(-1, 0);
                    }
                    break;
                case SPECIAL_KEY::ESC:
                    cursor.cursor_move(-input.size(), 0);
                    for(int i = 0; i <= input.size(); ++i) { std::cout << " "; }
                    cursor.cursor_move(-input.size() - 1, 0);
                    std::cout << red("<canceled>") << std::endl;
                    return "";
                default:
                    break;
            }
        } else {
            input += key.str;
            std::cout << "*" << std::flush;
        }
    }

    #endif

    return input;

}



/* ---------------------------Confirm-------------------------------*/

Inquire::Confirm::Confirm(std::string prompt_str) : prompt_str(prompt_str) {

    ToUTF8();

    std::cout << green("? ") << prompt_str << " [Enter/ESC] ";
}

bool Inquire::Confirm::prompt() {

    #ifdef _WIN32
    std::pair<int, int> ori_cursorPos = cursor.now_cursor_x_y();
    std::cout << gray("Enter: Yes, ESC: No");
    cursor.set_cursor_Pos(ori_cursorPos.first, ori_cursorPos.second);
    while (true) {
        KeyResult key = Inquire::key_catch();
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
    #endif


    #ifdef __linux__

    std::string help = "Enter: Yes, ESC: No";
    std::cout << gray(help);
    cursor.cursor_move(-help.size(), 0);

    while (true) {
        KeyResult key = Inquire::key_catch();
        if (key.isKey) {
            switch (key.key) {
                case SPECIAL_KEY::ENTER:
                    for (int i = 0; i < help.size(); ++i) {
                        std::cout << " ";
                    }
                    cursor.cursor_move(-help.size(), 0);
                    std::cout << cyan("Yes") << std::endl;
                    return true;
                case SPECIAL_KEY::ESC:
                    for (int i = 0; i < help.size(); ++i) {
                        std::cout << " ";
                    }
                    cursor.cursor_move(-help.size(), 0);
                    std::cout << red("No") << std::endl;
                    return false;
                default:
                    break;
            }
        }
    }

    #endif

    return false;
}


/* ---------------------------MultiSelect-------------------------------*/




