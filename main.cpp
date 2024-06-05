#include "inquire.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "encode.hpp"

int main() {
    ToUTF8();

    std::vector<std::string> options = {"Option 111", "Option 112", "Option 223", "Option 224"};

    std::string result = Select("Select an option", options).prompt();
    std::cout << "You selected: " << result << std::endl;

    // std::string text = Text("Enter some text").prompt();
    // std::cout << "You entered: " << text << std::endl;

    // std::string password = Password("Enter a password").prompt();
    // std::cout << "You entered: " << password << std::endl;

    // bool confirm = Confirm("Are you suuuuuuuuuuuuuuuuuuuure?").prompt();
    // std::cout << "You confirmed: " << confirm << std::endl;

    return 0;
}