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
    public:
        Select(std::string prompt_str, std::vector<std::string> options, int MAX_OPTION_LINE = 10);
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