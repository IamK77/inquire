#include <iostream>
#include "colorful.hpp"

int main() {
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
    
    return 0;
}