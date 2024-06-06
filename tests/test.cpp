// #include "inquire.hpp"
#include "../include/inquire.hpp"

int main() {
    

    std::vector<std::string> options = {"Option 111", "Option 112", "Option 223", "Option 224"};

    std::string result = Inquire::Select("Select an option", options).prompt();
    std::cout << "You selected: " << result << std::endl;

    std::string text = Inquire::Text("Enter some text").prompt();
    std::cout << "You entered: " << text << std::endl;

    std::string password = Inquire::Password("Enter a password").prompt();
    std::cout << "You entered: " << password << std::endl;

    bool confirm = Inquire::Confirm("Are you suuuuuure?").prompt();
    std::cout << "You confirmed: " << confirm << std::endl;

    return 0;
}
