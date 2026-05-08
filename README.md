# Inquire

[中文](Docs/README.zh.md) | English

A small, production-quality, single-header library of interactive command-line
prompts for C++11. Inspired by Rust's [inquire](https://github.com/mikaelmello/inquire).

```cpp
#define INQUIRE_IMPLEMENTATION
#include "inquire.hpp"

int main() {
    auto fruit = Inquire::Select("Pick a fruit", {"apple", "banana", "cherry"})
        .page_size(5)
        .prompt();
}
```

[![CI](https://github.com/IamK77/inquire/actions/workflows/ci.yml/badge.svg)](https://github.com/IamK77/inquire/actions/workflows/ci.yml)
![C++](https://img.shields.io/badge/c%2B%2B-11-blue)
![Header-only or static](https://img.shields.io/badge/single--header-yes-blueviolet)
![Platforms](https://img.shields.io/badge/platforms-linux%20%7C%20macOS%20%7C%20windows-lightgrey)
[![License: MIT](https://img.shields.io/badge/license-MIT-green)](LICENSE)
![Tests](https://img.shields.io/badge/tests-93%20passing-success)

> **Status**: 0.2.x is the first production-grade line. APIs may still
> evolve before 1.0; pin to a tag for stability. CI runs on Linux
> (gcc + clang), macOS (clang), and Windows (MSVC) with
> `-Werror`/`/WX`, plus ASan, UBSan, and coverage jobs.

## Features

| Prompt          | Returns                          | Highlights                                         |
|-----------------|----------------------------------|----------------------------------------------------|
| `Select`        | `std::string` / `int`            | filter, pagination, default index, custom matcher  |
| `MultiSelect`   | `std::vector<std::string>` / idx | min/max selected, defaults, pagination             |
| `Text`          | `std::string`                    | inline cursor edit, default, placeholder, validators |
| `Password`      | `std::string`                    | masking, optional show-typing, validators          |
| `Confirm`       | `bool`                           | y/n keys, optional default, Enter shortcut         |

Cross-cutting:

- **Typed errors**: `CancelledError`, `ValidationError`, `EmptyOptionsError`,
  `InterruptedError`, `IoError`. All derive from `InquireError` and carry an
  `ErrorCode`.
- **Non-throwing API**: every prompt also exposes `try_prompt()` that returns
  a `Result<T>`.
- **UTF-8 aware**: multi-byte input on Linux & Windows; double-width CJK glyphs
  in option labels are display-width truncated, not byte-truncated.
- **Signal-safe terminal**: SIGINT, SIGTERM, and SIGSEGV restore the terminal
  state before re-raising. No more "garbled shell" after Ctrl+C.
- **ANSI on every platform**: enables Virtual Terminal mode on Windows 10+ for
  uniform rendering.
- **Builder-style configuration**: chainable `prompt.option().option()` calls.

## Install

Drop `include/inquire.hpp` into your project. In **exactly one** translation
unit, define `INQUIRE_IMPLEMENTATION` before the include:

```cpp
#define INQUIRE_IMPLEMENTATION
#include "inquire.hpp"
```

Other translation units include the header normally.

Or, build from source as a static library:

```shell
make lib       # produces build/libinquire.a
make demo      # builds demo binary
make test      # runs unit tests
make example   # builds & runs the interactive example
```

## Quick start

```cpp
#define INQUIRE_IMPLEMENTATION
#include "inquire.hpp"

#include <iostream>

int main() {
    using namespace Inquire;
    try {
        std::string fruit = Select("Pick a fruit", {
                "apple", "banana", "cherry", "durian", "elderberry"
            })
            .page_size(3)
            .default_index(1)
            .help_message("type to filter, ↑↓ to move, enter to confirm")
            .prompt();

        std::vector<std::string> picks = MultiSelect("Toppings", {
                "cheese", "olives", "mushroom", "bacon", "spinach"
            })
            .min_selected(1)
            .max_selected(3)
            .prompt();

        std::string name = Text("Name?")
            .default_value("anonymous")
            .add_validator(validators::min_length(2))
            .prompt();

        std::string pwd = Password("Set a password")
            .add_validator(validators::all_of({
                validators::min_length(8),
                validators::matches(".*[0-9].*", "must contain a digit")
            }))
            .prompt();

        bool ok = Confirm("Looks good?").default_value(true).prompt();

        std::cout << name << " picked " << fruit << "; ok=" << ok << "\n";
    } catch (const Inquire::CancelledError&) {
        std::cerr << "cancelled\n";
        return 1;
    } catch (const Inquire::InquireError& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 2;
    }
}
```

## Error model

All prompts throw on cancellation. The base type is:

```cpp
class InquireError : public std::runtime_error {
public:
    ErrorCode code() const noexcept;
};
```

Specific subclasses, all of which you can catch:

| Type                  | When                                              |
|-----------------------|---------------------------------------------------|
| `CancelledError`      | user pressed ESC or Ctrl+C                        |
| `InterruptedError`    | user pressed Ctrl+D                               |
| `ValidationError`     | a validator rejected the input (re-prompted)      |
| `EmptyOptionsError`   | `Select`/`MultiSelect` was given an empty vector  |
| `IoError`             | terminal I/O failure                              |

If you prefer not to use exceptions:

```cpp
auto r = Inquire::Select("pick", {"a", "b"}).try_prompt();
if (r.ok()) {
    std::cout << r.value() << "\n";
} else if (r.error_code() == Inquire::ErrorCode::Cancelled) {
    std::cout << "user cancelled\n";
}
```

## Validators

Built-in validators in `Inquire::validators`:

```cpp
validators::non_empty()
validators::min_length(n)
validators::max_length(n)
validators::length_between(lo, hi)
validators::matches("[a-z0-9]+")
validators::integer()
validators::all_of({a, b, c})
```

A validator is just `std::function<void(const std::string&)>`; throw
`ValidationError("...")` from any callable to reject input. Failed validation
does **not** throw out of `prompt()`; it shows the error and re-prompts.

```cpp
Inquire::Text("port?")
    .add_validator([](const std::string& s) {
        if (std::stoi(s) > 65535) throw Inquire::ValidationError("> 65535");
    })
    .prompt();
```

## Reference

### `Select`

```cpp
Select(std::string question, std::vector<std::string> options);
Select& page_size(int n);
Select& default_index(int i);
Select& help_message(std::string m);
Select& case_sensitive(bool on);
Select& filter(std::function<bool(const std::string& option,
                                  const std::string& input)> fn);

std::string             prompt();        // throws CancelledError on cancel
int                     prompt_index();  // returns index instead of value
Result<std::string>     try_prompt();    // non-throwing
```

Keys: `↑/↓` move, `PgUp/PgDn`, `Home/End`, type to filter (case-insensitive
substring by default), `Enter` confirm, `Esc` or `Ctrl+C` cancel.

### `MultiSelect`

```cpp
MultiSelect(std::string question, std::vector<std::string> options);
MultiSelect& page_size(int n);
MultiSelect& default_indices(std::vector<int> idx);
MultiSelect& min_selected(int n);
MultiSelect& max_selected(int n);
MultiSelect& help_message(std::string m);
MultiSelect& case_sensitive(bool on);

std::vector<std::string>             prompt();
std::vector<int>                     prompt_indices();
Result<std::vector<std::string>>     try_prompt();
```

Keys: `↑/↓`, `PgUp/PgDn`, `Home/End`, `Space` toggles, `Enter` confirms.

### `Text`

```cpp
Text(std::string question);
Text& default_value(std::string v);
Text& placeholder(std::string p);
Text& help_message(std::string m);
Text& add_validator(Validator v);

std::string          prompt();
Result<std::string>  try_prompt();
```

Keys: `←/→`, `Home/End` (or `Ctrl+A`/`Ctrl+E`), `Backspace`/`Delete`,
`Ctrl+U` to clear, `Enter` confirm.

### `Password`

```cpp
Password(std::string question);
Password& mask_char(char c);          // default '*'
Password& show_typing(bool on);
Password& help_message(std::string m);
Password& add_validator(Validator v);

std::string          prompt();
Result<std::string>  try_prompt();
```

### `Confirm`

```cpp
Confirm(std::string question);
Confirm& default_value(bool b);
Confirm& help_message(std::string m);

bool          prompt();
Result<bool>  try_prompt();
```

Keys: `y/Y` → true, `n/N` → false, `Enter` accepts the default if set.

## Repository layout

```
include/inquire.hpp        amalgamated single header (regenerate via
                           `make single-header`)
src/Inquire/
├── inquire.hpp / .cpp     public API and prompt implementations
├── error.hpp              error hierarchy + Result<T>
├── validator.hpp          validators (header-only)
├── utils/
│   ├── colorful.hpp/.cpp  ANSI styling
│   ├── console.hpp/.cpp   Terminal abstraction (size, cursor, ANSI)
│   ├── encode.hpp/.cpp    UTF-8 console mode toggle
│   ├── func.h/.cpp        key event capture + UTF-8 helpers
│   ├── renderer.hpp/.cpp  multi-line stable redraw + width truncation
│   └── tty_guard.hpp/.cpp Linux RAII raw-mode + signal-safe restore
example/example.cpp        runs all five prompts
tests/test.cpp             46 unit tests covering the non-interactive surface
tools/amalgamate.sh        regenerates include/inquire.hpp from sources
```

## Platforms

- Linux: any terminal that speaks ANSI.
- Windows 10+: VT processing is enabled automatically.
- macOS: untested but should behave like Linux (POSIX termios).

## Versioning

Semantic versioning. The `0.x` line may break compatibility between minor
releases as the API stabilizes. Single-header users should pin a tagged
version.

## License

MIT. See `LICENSE`.
