#define INQUIRE_IMPLEMENTATION
#include "inquire.hpp"

#include <iostream>

int main() {
    using namespace Inquire;

    try {
        std::vector<std::string> options = {
            "Apple", "Banana", "Cherry", "Durian", "Elderberry",
            "Fig", "Grape", "Honeydew", "Kiwi", "Lemon",
            "Mango", "Nectarine", "Orange", "Papaya", "Quince"
        };

        std::string fruit = Select("Pick a fruit", options)
            .page_size(6)
            .help_message("type to fuzzy-filter, ↑↓ to move, enter to select")
            .prompt();
        std::cout << "fruit: " << fruit << std::endl;

        auto picks = MultiSelect("Pick toppings", options)
            .page_size(6)
            .min_selected(1)
            .max_selected(3)
            .default_indices({0, 2})
            .prompt();
        std::cout << "picks: ";
        for (size_t i = 0; i < picks.size(); ++i)
            std::cout << (i ? ", " : "") << picks[i];
        std::cout << std::endl;

        std::string name = Text("Your name?")
            .default_value("anonymous")
            .add_validator(validators::min_length(2))
            .prompt();
        std::cout << "name: " << name << std::endl;

        std::string pwd = Password("Set a password")
            .add_validator(validators::min_length(6, "password too short"))
            .prompt();
        std::cout << "password length: " << pwd.size() << std::endl;

        bool ok = Confirm("All good?").default_value(true).prompt();
        std::cout << "ok: " << (ok ? "yes" : "no") << std::endl;
    } catch (const CancelledError&) {
        std::cout << "cancelled" << std::endl;
        return 1;
    } catch (const InquireError& e) {
        std::cout << "error: " << e.what() << std::endl;
        return 2;
    }
    return 0;
}
