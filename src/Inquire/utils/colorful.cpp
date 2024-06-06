#include "colorful.hpp"
#include <string>
#include "encode.hpp"



std::string to_string(const std::string &msg) {
    return msg;
}

std::string to_string(const char &msg) {
    return std::string(1, msg);
}



#ifdef TEST

int main() {
    ToUTF8();

    std::string msg = "Hello, World!";
    
    std::cout << "Red: " << red(msg) << std::endl;
    std::cout << "Green: " << green(msg) << std::endl;
    std::cout << "Yellow: " << yellow(msg) << std::endl;
    std::cout << "Blue: " << blue(msg) << std::endl;
    std::cout << "Magenta: " << magenta(msg) << std::endl;
    std::cout << "Cyan: " << cyan(msg) << std::endl;
    std::cout << "White: " << white(msg) << std::endl;
    std::cout << "Black: " << black(msg) << std::endl;
    std::cout << "Bold: " << bold(msg) << std::endl;
    std::cout << "Underline: " << underline(msg) << std::endl;
    std::cout << "Gray: " << gray(msg) << std::endl;

    std::cout << "🍖" << std::endl;
    std::cout << "中文测试" << std::endl;
    
    return 0;
}

#endif



