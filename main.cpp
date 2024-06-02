#include "inquire.hpp"
#include <iostream>
#include <vector>
#include <string>

int main() {
    std::vector<std::string> options = {"Option 1", "Option 2", "Option 3", "Option 4"};

    std::string result = Select("Select an option", options).prompt();
    std::cout << "You selected: " << result << std::endl;

    return 0;
}