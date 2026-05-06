#pragma once

#include <string>
#include <vector>

namespace Inquire {

class Renderer {
public:
    void clear();
    void draw(const std::vector<std::string>& lines,
              int cursor_line = -1, int cursor_col_1based = -1);
    void finalize(const std::string& final_line);

private:
    int lines_above_ = 0;
    int lines_below_ = 0;
};

std::string truncate_display(const std::string& s, std::size_t max_width);

}
