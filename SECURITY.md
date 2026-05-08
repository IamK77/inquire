# Security Policy

## Supported versions

Inquire is in `0.x`. Only the most recent minor release receives security
fixes. Older `0.x` lines may be patched at the maintainers' discretion.

| Version  | Status     |
|----------|------------|
| 0.2.x    | Supported  |
| < 0.2    | End-of-life |

## Reporting a vulnerability

**Please do not open a public issue for vulnerabilities.**

Use one of the following private channels instead:

1. GitHub's [private vulnerability reporting](https://github.com/IamK77/inquire/security/advisories/new)
   (preferred — sends a draft advisory to the maintainers).
2. Open a regular issue without details that says "I have a security
   report" and a maintainer will reach out for a private channel.

When reporting, please include:

- A description of the issue and its impact.
- Reproduction steps or a proof-of-concept.
- The affected version(s) (`git rev-parse HEAD` or release tag).
- Your platform (OS, terminal, compiler).
- Whether you have a suggested fix.

## Process & timelines

- **Acknowledgement**: within 5 business days.
- **Initial assessment**: within 14 days, including a planned fix
  timeline.
- **Patch & disclosure**: aim for ≤ 90 days from acknowledgement.
  Coordinated disclosure is preferred.
- **Credit**: reporters who would like to be credited will be named in
  the release notes and the security advisory.

## Out of scope

The following are not considered security issues for this project:

- Visual misrendering on terminals that do not implement ANSI VT100
  control sequences.
- Behavior when stdin or stdout is not a TTY (the prompts are designed
  to be interactive).
- Crashes induced by sending crafted input via `tcsetattr`, `ptrace`,
  or other privileged interfaces — these require a local foothold that
  is itself the security boundary.

## Hardening notes for users

- Always check return values of `try_prompt()` if you cannot use
  exceptions.
- Treat input from `Text` / `Password` as untrusted; validate before
  using it in shell commands, SQL, or filesystem paths.
- For password fields prefer `Password` over `Text` so the value never
  touches the screen buffer.
