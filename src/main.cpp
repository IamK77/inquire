#include "Inquire/inquire.hpp"

#include <iostream>
#include <vector>

int main() {
    using namespace Inquire;

    try {
        std::vector<std::string> options = {
            "Apple", "Banana", "Cherry", "Durian", "Elderberry",
            "Fig", "Grape", "Honeydew", "Kiwi", "Lemon"
        };

        std::string fruit = Select("Pick a fruit", options)
            .page_size(5)
            .help_message("type to filter, ↑↓ to move")
            .prompt();
        std::cout << "selected: " << fruit << "\n";

        std::vector<std::string> picks = MultiSelect("Pick toppings", options)
            .page_size(5)
            .min_selected(1)
            .max_selected(3)
            .prompt();
        std::cout << "picks: " << picks.size() << "\n";

        std::string name = Text("Your name?")
            .default_value("anonymous")
            .add_validator(validators::min_length(2))
            .prompt();
        std::cout << "name: " << name << "\n";

        std::string pwd = Password("Set a password")
            .add_validator(validators::min_length(6, "password too short"))
            .prompt();
        std::cout << "password length: " << pwd.size() << "\n";

        bool sure = Confirm("Are you sure?")
            .default_value(true)
            .prompt();
        std::cout << "confirmed: " << (sure ? "yes" : "no") << "\n";

    } catch (const CancelledError&) {
        std::cout << "user cancelled\n";
        return 1;
    } catch (const InquireError& e) {
        std::cout << "error: " << e.what() << "\n";
        return 2;
    }
    return 0;
}
