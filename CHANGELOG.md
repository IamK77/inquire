# Changelog

## 0.2.0 — 2026-05-06

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

### New

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
- `Confirm` now accepts `y/Y/n/N` directly; `Enter` only applies when a
  default is set.
- UTF-8 multi-byte input on Linux & Windows (was previously truncated to
  ASCII / 3 bytes).
- Display-width-aware truncation for option labels (correct CJK handling).

### Reliability

- Linux raw-mode is now RAII-managed; the terminal is restored on every
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

### Build

- Cleaner Makefile with `lib`, `demo`, `test`, `example`,
  `single-header`, `clean` targets.
- `tools/amalgamate.sh` regenerates `include/inquire.hpp` from the
  modular sources.
- Static library output `build/libinquire.a`.
- Builds with `-Wall -Wextra -Wpedantic` clean.

### Testing

- 46 unit tests covering UTF-8 helpers, validators, `Result<T>`, error
  hierarchy, truncation, builder chains, and empty-input safety.

## 0.1.0

Initial release.
