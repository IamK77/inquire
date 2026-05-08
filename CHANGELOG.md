# Changelog

All notable changes to this project are documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/).

## [0.2.0] — 2026-05-08

A ground-up rewrite for production use. The 0.1.x API is **not** backwards
compatible with this release; pin to v0.1.0 if you need the old behavior.

### Breaking changes

- `prompt()` now **throws** on cancellation (`Inquire::CancelledError`)
  instead of returning an empty string. Use `try_prompt()` for the
  non-throwing variant returning `Result<T>`.
- `Inquire::SPECIAL_KEY` and `Inquire::KeyResult` are gone, replaced by
  `Inquire::Key` (scoped enum) and `Inquire::KeyEvent`.
- The `Cursor` class and Windows globals (`hConsole`, `csbi`, `hStdin`,
  ...) are no longer in the public surface. Use `Inquire::Terminal` for
  size queries.
- The single-header library now requires `#define INQUIRE_IMPLEMENTATION`
  in exactly one translation unit (stb-style).

### Added

- `MultiSelect` is fully implemented — was previously declared but unused.
- Builder API on every prompt: `page_size`, `default_value`/`default_index`,
  `help_message`, `add_validator`, etc.
- `Inquire::validators` namespace: `non_empty`, `min_length`, `max_length`,
  `length_between`, `matches` (regex), `integer`, `all_of`.
- Typed error hierarchy: `InquireError`, `CancelledError`,
  `InterruptedError`, `ValidationError`, `EmptyOptionsError`, `IoError`,
  each carrying an `ErrorCode`.
- Non-throwing `try_prompt()` returning `Result<T>`.
- Pagination & scrolling for `Select`/`MultiSelect` (`page_size` is now
  honored — it was ignored in 0.1.x).
- Case-insensitive substring filter by default; opt-in case sensitivity;
  custom matcher via `Select::filter(...)`.
- Inline cursor editing in `Text`: `←/→`, `Home/End`, `Ctrl+A`/`Ctrl+E`,
  `Backspace`/`Delete`, `Ctrl+U`.
- `Confirm` accepts `y/Y/n/N` directly; `Enter` applies the default only
  when one is set.
- UTF-8 multi-byte input on Linux & Windows (was previously truncated to
  ASCII / 3 bytes).
- Display-width-aware truncation for option labels (correct CJK handling).

### Reliability

- Linux raw-mode is RAII-managed; the terminal is restored on every
  return path, including exceptions.
- `SIGINT`, `SIGTERM`, and `SIGSEGV` handlers restore the terminal and
  re-raise with the default action — no more "garbled shell" after
  Ctrl+C.
- Signal-handler-safe storage of the original `termios` (no dangling
  pointers).
- UTF-8 continuation-byte validation when reading multi-byte input.
- Renderer tracks both lines-above and lines-below the cursor so
  multi-line redraws and finalization always clear the prior frame
  cleanly.

### Build & tooling

- **CMake**: `Inquire::header` (INTERFACE) and `Inquire::inquire`
  (STATIC) targets, install rules, CMake package config + version
  file, pkg-config (`inquire.pc`). Toggles for tests, examples, ASan,
  UBSan, coverage, and warnings-as-errors.
- **Makefile**: `lib | demo | test | example | single-header | asan |
  ubsan | tsan | coverage | tidy | format | docs | clean`.
- **CI**: GitHub Actions matrix (Ubuntu gcc/clang, macOS, Windows
  MSVC) with `-Werror`; amalgamation drift check, ASan+UBSan job, lcov
  coverage upload, tag-driven release with single-header asset and
  CHANGELOG-extracted notes; Doxygen → GitHub Pages workflow.
- **Static analysis**: `.clang-tidy` enabling bugprone, cert,
  cppcoreguidelines, hicpp, modernize, performance, portability, and
  readability families with project-tuned suppressions.
- **Editor / VCS**: `.clang-format`, `.editorconfig`, `.gitattributes`
  (LF normalization, single-header marked `linguist-generated`),
  expanded `.gitignore`.

### Testing

- 93 unit tests covering UTF-8 helpers (incl. 4-byte / mixed
  sequences), validators (boundary cases, custom callables),
  `Result<T>` (incl. throw on `value()`), error hierarchy, truncation
  display-width invariants, ANSI toggle, error-code distinctness,
  builder chaining, empty-options behavior.

### Community

- `CONTRIBUTING.md`, `CODE_OF_CONDUCT.md` (Contributor Covenant 2.1),
  `SECURITY.md`, `.github/ISSUE_TEMPLATE/{bug_report,feature_request,config}`,
  `.github/PULL_REQUEST_TEMPLATE.md`.

## [0.1.0]

Initial release.

[0.2.0]: https://github.com/IamK77/inquire/releases/tag/v0.2.0
[0.1.0]: https://github.com/IamK77/inquire/releases/tag/v0.1.0
