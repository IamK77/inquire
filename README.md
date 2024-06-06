# Inquire

[中文](Docs/README.zh.md) | English

Inquire is a simple, interactive command-line library for C++ developers, making it easier to create interactive command-line applications.

## Background

When writing some simple scripts, I wanted to use an interactive command line for convenience. Searching on GitHub, I found Rust has `inquire` and Python has `bullet`, but there wasn't a library in C++ that met my expectations. Hence, this project was born.

This library emulates the effects of [inquire](https://github.com/mikaelmello/inquire).

## Features

- [`Select`](#select) - Choose an option from a list of selections.
- [`Text`](#text) - Enter text.
- [`Confirm`](#confirm) - Confirmation prompt.
- [`Password`](#password) - Enter a password.

## Example Usage

```shell
make example
```

## Usage

Place `inquire.hpp` from the `include` directory into your project

Create an `inquire` folder in your directory for third-party libraries, and select the appropriate command to run based on your situation.

```shell
mkdir inquire
cd inquire
```

Download the `inquire.hpp` file using wget:

```shell
wget -O inquire.hpp https://github.com/IamK77/inquire/releases/download/v0.1.0/inquire.hpp
```

Alternatively, download the `inquire.hpp` file using curl:

```shell
curl -L -o inquire.hpp https://github.com/IamK77/inquire/releases/download/v0.1.0/inquire.hpp
```

To download the `inquire.hpp` file using a proxy with curl, replace `7890` with your proxy port:

```shell
curl -x 127.0.0.1:7890 -L -o inquire.hpp https://github.com/IamK77/inquire/releases/download/v0.1.0/inquire.hpp
```

use as follows:

```cpp
#include "inquire.hpp"

int main() {
    std::vector<std::string> options = {"Option 1", "Option 2", "Option 3", "Option 4"};

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
```

Using it is similar to `inquire`.

## Select

In `Select`, use arrow keys to choose an option, type to filter, Enter to confirm, and ESC to cancel.

```cpp
std::vector<std::string> options = {"Option 1", "Option 2", "Option 3", "Option 4"};

std::string result = Inquire::Select("Select an option", options).prompt();

if (!result.empty()) {
    switch (result) {
        case "Option 1":
            // do something
            break;
        case "Option 2":
            // do something
            break;
        case "Option 3":
            // do something
            break;
        case "Option 4":
            // do something
            break;
    }
}
```

*class Select*
----------------
- `Select(param_question, param_options)` Constructor
    - `param_question` : std::string The question
    - `param_options` : std::vector\<std::string> List of options

- `prompt()` : std::string Display the question and return the selected option
    - `return` : std::string The selected option


## Text

In `Text`, type text and press Enter to confirm, ESC to cancel.

```cpp
std::string text = Inquire::Text("Enter some text").prompt();
```

*class Text*
----------------
- `Text(param_question)` Constructor
    - `param_question` : std::string The question

- `prompt()` : std::string Display the question and return the entered text
    - `return` : std::string The entered text


## Confirm

In `Confirm`, press Enter to confirm, ESC to cancel.

```cpp
bool confirm = Inquire::Confirm("Are you suuuuuure?").prompt();
```

*class Confirm*
----------------
- `Confirm(param_question)` Constructor
    - `param_question` : std::string The question

- `prompt()` : bool Display the question and return the user's choice
    - `return` : bool The user's choice


## Password

In `Password`, type a password and press Enter to confirm, ESC to cancel.

```cpp
std::string password = Inquire::Password("Enter a password").prompt();
```

*class Password*
----------------
- `Password(param_question)` Constructor
    - `param_question` : std::string The question

- `prompt()` : std::string Display the question and return the entered password
    - `return` : std::string The entered password


## Related Repositories

- [inquire](https://github.com/mikaelmello/inquire) - Rust
