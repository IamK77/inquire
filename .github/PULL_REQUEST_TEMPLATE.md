<!--
Thanks for opening a pull request! Please fill out the sections below.
See CONTRIBUTING.md for the full workflow.
-->

## Summary

<!-- One paragraph: what changes and why. -->

## Type of change

- [ ] Bug fix (non-breaking)
- [ ] New feature (non-breaking)
- [ ] Breaking change
- [ ] Documentation only
- [ ] Build / CI / tooling

## Checklist

- [ ] Source changes are in `src/` (not in the generated `include/inquire.hpp`)
- [ ] `bash tools/amalgamate.sh` was run; `include/inquire.hpp` is up to date
- [ ] Added or updated tests in `tests/test.cpp`
- [ ] `cmake -S . -B build && cmake --build build && ctest --test-dir build` passes locally
- [ ] Builds cleanly under `-Wall -Wextra -Wpedantic` (CI uses `-Werror`)
- [ ] Public API change → updated `README.md` and `Docs/README.zh.md`
- [ ] Behavior change → added an entry to `CHANGELOG.md`

## Related issues

<!-- e.g. Fixes #123, Refs #456 -->
