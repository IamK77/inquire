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

    std::cout << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
