# Contributing to Inquire

Thanks for taking the time to contribute! This document explains how to get
set up, what we expect from a patch, and where things live.

## Code of Conduct

This project adopts the [Contributor Covenant](CODE_OF_CONDUCT.md). By
participating you agree to abide by it.

## Quick start

```sh
# clone
git clone https://github.com/IamK77/inquire.git
cd inquire

# build & run tests via CMake
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
ctest --test-dir build --output-on-failure

# or with the Makefile
make lib && make test
```

For local sanitizer runs:

```sh
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug \
  -DINQUIRE_ENABLE_ASAN=ON -DINQUIRE_ENABLE_UBSAN=ON
cmake --build build-asan -j
ctest --test-dir build-asan --output-on-failure
```

## Repository layout

| Path                     | Contents                                          |
|--------------------------|---------------------------------------------------|
| `include/inquire.hpp`    | Generated single-header. **Do not edit by hand.** |
| `src/Inquire/`           | Modular sources — edit these.                     |
| `src/Inquire/utils/`     | Internal helpers (terminal, key input, renderer). |
| `tools/amalgamate.sh`    | Regenerates `include/inquire.hpp`.                |
| `tests/test.cpp`         | Compile-only unit tests — non-interactive.        |
| `example/example.cpp`    | Runs all five interactive prompts.                |
| `cmake/`                 | CMake config templates.                           |
| `.github/workflows/`     | CI definitions.                                   |

## Workflow

1. **Fork** the repo and create a topic branch off `main`:
   `git switch -c feat/short-description`.
2. **Make changes** in `src/`. Don't edit `include/inquire.hpp` directly —
   regenerate it (see below).
3. **Add or update tests** in `tests/test.cpp`. New behavior without a test
   will be asked for one in review.
4. **Regenerate the single header**: `bash tools/amalgamate.sh`. CI will
   reject PRs where this is out of date.
5. **Run the full check** locally:
   ```sh
   cmake -S . -B build && cmake --build build -j && ctest --test-dir build
   ```
6. **Open a PR**. CI must pass.

## Coding conventions

- C++11 only. No newer features in shippable code (we still target older
  toolchains). It's fine to use `<regex>`, `<atomic>`, `<thread>`.
- Run `clang-format` on touched files; `.clang-format` is in the repo root.
- Build cleanly under `-Wall -Wextra -Wpedantic`. CI uses
  `-DINQUIRE_WARNINGS_AS_ERRORS=ON`.
- Naming:
  - Public types in `Inquire::` (PascalCase classes, `snake_case` members).
  - Internal helpers go in anonymous namespaces inside `.cpp` files.
- Errors:
  - Throw `Inquire::ValidationError` from validators.
  - Throw `Inquire::CancelledError` / `InterruptedError` only from the
    prompt I/O loop.
  - User-facing prompts must restore the terminal on every code path
    (RAII, not `try`/`finally` patterns).

## Adding a new prompt

A new prompt should live next to the existing ones in `src/Inquire/inquire.{hpp,cpp}`:

1. Declare the class in `inquire.hpp` with builder-style chained setters
   that return `*this`.
2. Implement in `inquire.cpp`. Build a `Renderer`, a draw lambda, and a key
   loop modelled on `Text::do_prompt_()`.
3. Throw `CancelledError` on Esc / Ctrl+C, `InterruptedError` on Ctrl+D.
4. Provide both `prompt()` (throwing) and `try_prompt()` (returning
   `Result<T>`).
5. Add unit tests covering builder chaining and any pure helpers.
6. Document the public API in `README.md` and `Docs/README.zh.md`.
7. Regenerate the single header.

## Reporting bugs

Open an issue using the bug report template. Include:

- OS, terminal emulator, and compiler version.
- A minimal reproduction (≤ 30 lines) and the actual vs. expected output.
- For rendering bugs, an `asciinema` recording or screenshot helps.

## Security

See [SECURITY.md](SECURITY.md). Don't open public issues for vulnerabilities.

## License

By contributing you agree your patches will be released under the project's
[MIT license](LICENSE).
