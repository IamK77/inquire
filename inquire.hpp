#pragma once

#include "utils/func.h"
#include "utils/console.hpp"
#include "utils/colorful.hpp"
#include <vector>


class Select {
    private:
        std::vector<std::string> options;
        std::string prompt_str;
        int MAX_OPTION_LINE;
    private:
        Cursor cursor;

        std::pair<int, int> ori_cursorPos;

        std::string input = "";
        int option_size;
        int selected = 0;
        int cursorPos = 0;
        int preselected = 0;
        
        #ifdef __linux__
            int temp = 0;
        #endif
        
    public:
        Select(std::string prompt_str, std::vector<std::string> options, int MAX_OPTION_LINE = 7);
        std::string prompt();
    private:
        void flush_display(bool is_clear = false);
        void input_insert(std::string ch);
        void flush_result(std::string result);
        void flush_input();
        void up(); 
        void down();
        void right();
        void left();
};


class Text {
    private:
        std::string prompt_str;
        Cursor cursor;
        std::string input;
    public:
        Text(std::string prompt_str);
        std::string prompt();
};


class Password {
    private:
        std::string prompt_str;
        Cursor cursor;
    public:
        Password(std::string prompt_str);
        std::string prompt();
};


class Confirm {
    private:
        std::string prompt_str;
        Cursor cursor;
    public:
        Confirm(std::string prompt_str);
        bool prompt();
};


class MultiSelect {
    private:
        std::vector<std::string> options;
        std::string prompt_str;
        int MAX_OPTION_LINE;
    private:
        Cursor cursor;

        std::pair<int, int> ori_cursorPos;

        std::vector<std::string> input;
        int option_size;
        std::vector<int> selected;
        int cursorPos = 0;
        int preselected = 0;
    public:
        MultiSelect(std::string prompt_str, std::vector<std::string> options, int MAX_OPTION_LINE = 10);
        std::vector<std::string> prompt();
    private:
        void flush_display(bool is_clear = false);
        void input_insert(std::string ch);
        void flush_result(std::vector<std::string> result);
        void flush_input();
        void up(); 
        void down();
        void right();
        void left();
};