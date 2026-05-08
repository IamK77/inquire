// Compile-only tests for the single-header library: exercise the public API
// surface without performing interactive I/O. Built with `make test`.

#define INQUIRE_IMPLEMENTATION
#include "inquire.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace Inquire;

static int passed = 0;
static int failed = 0;

#define CHECK(expr) do { \
    if (expr) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expr " (line " << __LINE__ << ")\n"; } \
} while (0)

static void test_utf8() {
    CHECK(utf8_char_count("hello") == 5);
    CHECK(utf8_char_count("中文") == 2);
    CHECK(utf8_char_count("a中b") == 3);
    CHECK(utf8_display_width("hello") == 5);
    CHECK(utf8_display_width("中文") == 4);
    CHECK(utf8_display_width("a中b") == 4);

    std::string s = "中文";
    CHECK(utf8_char_size(s, 0) == 3);
    CHECK(utf8_prev_char_start(s, 6) == 3);
    CHECK(utf8_prev_char_start(s, 3) == 0);
}

static void test_icontains() {
    CHECK(icontains("Hello World", "hello"));
    CHECK(icontains("Hello World", "WORLD"));
    CHECK(icontains("abc", ""));
    CHECK(!icontains("abc", "xyz"));
    CHECK(!icontains("ab", "abc"));
}

static void test_validators() {
    bool threw = false;
    try { validators::non_empty()(""); }
    catch (const ValidationError&) { threw = true; }
    CHECK(threw);

    threw = false;
    try { validators::non_empty()("hi"); }
    catch (const ValidationError&) { threw = true; }
    CHECK(!threw);

    threw = false;
    try { validators::min_length(3)("ab"); }
    catch (const ValidationError&) { threw = true; }
    CHECK(threw);

    threw = false;
    try { validators::max_length(2)("abc"); }
    catch (const ValidationError&) { threw = true; }
    CHECK(threw);

    threw = false;
    try { validators::integer()("abc"); }
    catch (const ValidationError&) { threw = true; }
    CHECK(threw);

    threw = false;
    try { validators::integer()("-42"); }
    catch (const ValidationError&) { threw = true; }
    CHECK(!threw);

    threw = false;
    try { validators::matches("[0-9]+")("abc"); }
    catch (const ValidationError&) { threw = true; }
    CHECK(threw);

    threw = false;
    try { validators::all_of({validators::non_empty(), validators::min_length(3)})("ab"); }
    catch (const ValidationError&) { threw = true; }
    CHECK(threw);
}

static void test_result() {
    auto ok = Result<int>::ok(42);
    CHECK(ok.ok());
    CHECK(ok.value() == 42);
    CHECK(ok.value_or(0) == 42);

    auto err = Result<int>::err(ErrorCode::Cancelled, "nope");
    CHECK(!err.ok());
    CHECK(err.error_code() == ErrorCode::Cancelled);
    CHECK(err.error_message() == "nope");
    CHECK(err.value_or(7) == 7);
}

static void test_error_hierarchy() {
    CancelledError c;
    CHECK(c.code() == ErrorCode::Cancelled);
    CHECK(dynamic_cast<const InquireError*>(&c) != nullptr);

    ValidationError v("bad");
    CHECK(v.code() == ErrorCode::InvalidInput);
    CHECK(std::string(v.what()) == "bad");

    EmptyOptionsError e;
    CHECK(e.code() == ErrorCode::EmptyOptions);
}

static void test_truncate() {
    CHECK(truncate_display("hello", 10) == "hello");
    CHECK(truncate_display("hello world", 5) == "hell\xe2\x80\xa6");
    CHECK(truncate_display("中文测试", 4) == "中\xe2\x80\xa6");
    CHECK(truncate_display("中文", 4) == "中文");
    CHECK(truncate_display("ab", 5) == "ab");
}

static void test_select_empty_throws() {
    bool threw = false;
    try {
        Select("question", std::vector<std::string>{}).prompt();
    } catch (const EmptyOptionsError&) {
        threw = true;
    }
    CHECK(threw);
}

static void test_multiselect_empty_throws() {
    bool threw = false;
    try {
        MultiSelect("question", std::vector<std::string>{}).prompt();
    } catch (const EmptyOptionsError&) {
        threw = true;
    }
    CHECK(threw);
}

static void test_builder_chain() {
    Select s("q", {"a", "b"});
    s.page_size(3).default_index(1).help_message("h").case_sensitive(true);
    CHECK(true);

    MultiSelect m("q", {"a", "b"});
    m.page_size(3).min_selected(1).max_selected(2).default_indices({0});
    CHECK(true);

    Text t("q");
    t.default_value("v").placeholder("p").help_message("h")
     .add_validator(validators::non_empty());
    CHECK(true);

    Password p("q");
    p.mask_char('#').show_typing(true).help_message("h")
     .add_validator(validators::min_length(1));
    CHECK(true);

    Confirm c("q");
    c.default_value(false).help_message("h");
    CHECK(true);
}

static void test_utf8_edge_cases() {
    CHECK(utf8_char_count("") == 0);
    CHECK(utf8_display_width("") == 0);
    CHECK(utf8_char_size("", 0) == 0);
    CHECK(utf8_char_size("abc", 5) == 0);
    CHECK(utf8_prev_char_start("", 0) == 0);
    CHECK(utf8_prev_char_start("abc", 0) == 0);

    // 4-byte emoji (musical note U+1D11E and rocket U+1F680).
    std::string emoji = "\xF0\x9D\x84\x9E";
    CHECK(utf8_char_count(emoji) == 1);
    CHECK(utf8_char_size(emoji, 0) == 4);

    std::string rocket = "\xF0\x9F\x9A\x80";
    CHECK(utf8_char_count(rocket) == 1);
    CHECK(utf8_display_width(rocket) == 2);

    // Mixed: ascii + 2-byte + 3-byte + 4-byte
    std::string mixed = std::string("a") + "\xC3\xA9" + "中" + rocket;
    CHECK(utf8_char_count(mixed) == 4);
    CHECK(utf8_display_width(mixed) == 1 + 1 + 2 + 2);
}

static void test_validators_edge_cases() {
    bool threw = false;
    try { validators::integer()(""); }
    catch (const ValidationError&) { threw = true; }
    CHECK(threw);

    threw = false;
    try { validators::integer()("+"); }
    catch (const ValidationError&) { threw = true; }
    CHECK(threw);

    threw = false;
    try { validators::integer()("+0"); }
    catch (const ValidationError&) { threw = true; }
    CHECK(!threw);

    threw = false;
    try { validators::length_between(2, 4)("a"); }
    catch (const ValidationError&) { threw = true; }
    CHECK(threw);

    threw = false;
    try { validators::length_between(2, 4)("abcde"); }
    catch (const ValidationError&) { threw = true; }
    CHECK(threw);

    threw = false;
    try { validators::length_between(2, 4)("abc"); }
    catch (const ValidationError&) { threw = true; }
    CHECK(!threw);

    // Custom validator via lambda
    Validator forbid_x = [](const std::string& s) {
        if (s.find('x') != std::string::npos) throw ValidationError("no x allowed");
    };
    threw = false;
    try { forbid_x("axe"); }
    catch (const ValidationError& e) {
        threw = true;
        CHECK(std::string(e.what()) == "no x allowed");
    }
    CHECK(threw);
}

static void test_result_throws_on_value() {
    auto err = Result<std::string>::err(ErrorCode::Cancelled, "cancelled");
    bool threw = false;
    try { (void)err.value(); }
    catch (const InquireError& e) {
        threw = true;
        CHECK(e.code() == ErrorCode::Cancelled);
    }
    CHECK(threw);

    auto ok = Result<std::string>::ok("hi");
    CHECK(ok.value() == "hi");
    CHECK(static_cast<bool>(ok));
    CHECK(!static_cast<bool>(err));
}

static void test_truncate_edge_cases() {
    CHECK(truncate_display("", 5) == "");
    CHECK(truncate_display("hello", 0) == "");
    CHECK(truncate_display("a", 1) == "a");
    // Width-aware truncation never produces output wider than max_width.
    std::string out = truncate_display("中文测试", 3);
    CHECK(utf8_display_width(out) <= 3);
    out = truncate_display("hello world!!!!!", 7);
    CHECK(utf8_display_width(out) <= 7);
}

static void test_icontains_edge_cases() {
    CHECK(icontains("", ""));
    CHECK(!icontains("", "a"));
    CHECK(icontains("aaa", "a"));
    CHECK(icontains("AbCdEf", "cde"));
    CHECK(!icontains("Foo Bar", "ofb"));
    // Non-ASCII bytes pass through verbatim (no Unicode case folding,
    // by design — stays predictable across locales).
    CHECK(icontains("中文测试", "文测"));
}

static void test_color_toggle() {
    style::set_enabled(false);
    std::string plain = style::red("hello");
    CHECK(plain == "hello");
    style::set_enabled(true);
    std::string colored = style::red("hello");
    CHECK(colored != "hello");
    CHECK(colored.find("hello") != std::string::npos);
}

static void test_error_codes_distinct() {
    CHECK(static_cast<int>(ErrorCode::Cancelled)   != static_cast<int>(ErrorCode::Interrupted));
    CHECK(static_cast<int>(ErrorCode::InvalidInput) != static_cast<int>(ErrorCode::EmptyOptions));
    CHECK(static_cast<int>(ErrorCode::Io)           != static_cast<int>(ErrorCode::UnsupportedTerminal));

    // Each typed error reports the matching code.
    CancelledError c;          CHECK(c.code() == ErrorCode::Cancelled);
    InterruptedError i;        CHECK(i.code() == ErrorCode::Interrupted);
    ValidationError v("x");    CHECK(v.code() == ErrorCode::InvalidInput);
    EmptyOptionsError e;       CHECK(e.code() == ErrorCode::EmptyOptions);
    IoError io("io");          CHECK(io.code() == ErrorCode::Io);
}

int main() {
    test_utf8();
    test_icontains();
    test_validators();
    test_result();
    test_error_hierarchy();
    test_truncate();
    test_select_empty_throws();
    test_multiselect_empty_throws();
    test_builder_chain();
    test_utf8_edge_cases();
    test_validators_edge_cases();
    test_result_throws_on_value();
    test_truncate_edge_cases();
    test_icontains_edge_cases();
    test_color_toggle();
    test_error_codes_distinct();

    std::cout << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
