#include "inquire.hpp"

#include "utils/renderer.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

namespace Inquire {

namespace {

constexpr const char* QUESTION_MARK = "?";
constexpr const char* CURSOR_MARK   = ">";
constexpr const char* SEP_MARK      = "›";
constexpr const char* CHECK_ON      = "[x]";
constexpr const char* CHECK_OFF     = "[ ]";

std::string format_question(const std::string& q) {
    return style::green(QUESTION_MARK) + " " + style::bold(q);
}

std::string format_help(const std::string& help) {
    return style::dim(help);
}

std::string format_error(const std::string& err) {
    return style::red("✗ " + err);
}

bool default_filter_match(const std::string& option, const std::string& input,
                          bool case_sensitive) {
    if (input.empty()) return true;
    if (case_sensitive) return option.find(input) != std::string::npos;
    return icontains(option, input);
}

void utf8_erase_before(std::string& s, std::size_t& cursor) {
    if (cursor == 0) return;
    std::size_t prev = utf8_prev_char_start(s, cursor);
    s.erase(prev, cursor - prev);
    cursor = prev;
}

void utf8_erase_at(std::string& s, std::size_t cursor) {
    if (cursor >= s.size()) return;
    std::size_t len = utf8_char_size(s, cursor);
    s.erase(cursor, len);
}

void utf8_move_left(const std::string& s, std::size_t& cursor) {
    if (cursor > 0) cursor = utf8_prev_char_start(s, cursor);
}

void utf8_move_right(const std::string& s, std::size_t& cursor) {
    if (cursor < s.size()) cursor += utf8_char_size(s, cursor);
}

std::size_t header_prefix_width(const std::string& question) {
    return utf8_display_width(QUESTION_MARK) + 1
         + utf8_display_width(question) + 1
         + utf8_display_width(SEP_MARK) + 1;
}

int compute_scroll(int cursor, int scroll_top, int matches_size, int viewport) {
    if (matches_size <= viewport) return 0;
    if (cursor < scroll_top) scroll_top = cursor;
    if (cursor >= scroll_top + viewport) scroll_top = cursor - viewport + 1;
    if (scroll_top < 0) scroll_top = 0;
    if (scroll_top > matches_size - viewport) scroll_top = matches_size - viewport;
    return scroll_top;
}

}

/* =============================== Select =============================== */

Select::Select(std::string question, std::vector<std::string> options)
    : question_(std::move(question)), options_(std::move(options)) {
    Terminal::init();
    ToUTF8();
}

Select& Select::page_size(int n)         { page_size_ = n > 0 ? n : 1; return *this; }
Select& Select::default_index(int i)     { default_index_ = i; return *this; }
Select& Select::help_message(std::string m) { help_ = std::move(m); return *this; }
Select& Select::case_sensitive(bool on)  { case_sensitive_ = on; return *this; }
Select& Select::filter(std::function<bool(const std::string&, const std::string&)> fn) {
    filter_ = std::move(fn);
    return *this;
}

int Select::do_prompt_() {
    if (options_.empty()) throw EmptyOptionsError();

    Renderer renderer;

    auto match_one = [&](const std::string& option, const std::string& input) {
        return filter_ ? filter_(option, input)
                       : default_filter_match(option, input, case_sensitive_);
    };
    auto recompute_matches = [&](const std::string& input) {
        std::vector<int> m;
        m.reserve(options_.size());
        for (std::size_t i = 0; i < options_.size(); ++i) {
            if (match_one(options_[i], input)) m.push_back(static_cast<int>(i));
        }
        return m;
    };

    std::string input;
    auto matches = recompute_matches(input);

    int cursor = 0;
    if (default_index_ >= 0 && default_index_ < static_cast<int>(options_.size())) {
        for (std::size_t i = 0; i < matches.size(); ++i) {
            if (matches[i] == default_index_) { cursor = static_cast<int>(i); break; }
        }
    }
    int initial_viewport = std::min(page_size_, static_cast<int>(matches.size()));
    int scroll_top = compute_scroll(cursor, 0, static_cast<int>(matches.size()), initial_viewport);

    auto draw = [&]() {
        auto term = Terminal::size();
        std::size_t max_width = term.first > 4 ? static_cast<std::size_t>(term.first - 4) : 1;

        std::vector<std::string> lines;
        lines.push_back(format_question(question_) + " " + style::dim(SEP_MARK) + " " + input);

        int viewport = std::min(page_size_, static_cast<int>(matches.size()));
        if (viewport == 0) {
            lines.push_back(style::dim("  (no matches)"));
        } else {
            for (int i = 0; i < viewport; ++i) {
                int idx = scroll_top + i;
                if (idx >= static_cast<int>(matches.size())) break;
                std::string truncated = truncate_display(options_[matches[idx]], max_width);
                if (idx == cursor) {
                    lines.push_back(style::cyan(std::string(CURSOR_MARK) + " " + truncated));
                } else {
                    lines.push_back("  " + truncated);
                }
            }
        }

        std::ostringstream hint;
        int total = static_cast<int>(matches.size());
        if (total > viewport && viewport > 0) {
            hint << "[" << (cursor + 1) << "/" << total << "]  ";
        }
        hint << (help_.empty()
                 ? "↑↓ navigate · type to filter · enter · esc"
                 : help_);
        lines.push_back(format_help(hint.str()));

        int header_col = static_cast<int>(header_prefix_width(question_)
                                          + utf8_display_width(input)) + 1;
        renderer.draw(lines, /*cursor_line=*/0, /*cursor_col=*/header_col);
    };

    draw();

    while (true) {
        KeyEvent ev = key_catch();
        int viewport = std::min(page_size_, static_cast<int>(matches.size()));
        switch (ev.key) {
            case Key::Up:
                if (!matches.empty()) {
                    cursor = (cursor - 1 + static_cast<int>(matches.size()))
                             % static_cast<int>(matches.size());
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::Down:
                if (!matches.empty()) {
                    cursor = (cursor + 1) % static_cast<int>(matches.size());
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::PageUp:
                if (!matches.empty()) {
                    cursor = std::max(0, cursor - page_size_);
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::PageDown:
                if (!matches.empty()) {
                    cursor = std::min(static_cast<int>(matches.size()) - 1, cursor + page_size_);
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::Home:
                cursor = 0; scroll_top = 0; break;
            case Key::End:
                if (!matches.empty()) {
                    cursor = static_cast<int>(matches.size()) - 1;
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::Backspace: {
                if (!input.empty()) {
                    std::size_t pos = input.size();
                    utf8_erase_before(input, pos);
                    matches = recompute_matches(input);
                    if (cursor >= static_cast<int>(matches.size())) cursor = 0;
                    int vp = std::min(page_size_, static_cast<int>(matches.size()));
                    scroll_top = compute_scroll(cursor, 0, static_cast<int>(matches.size()), vp);
                }
                break;
            }
            case Key::CtrlU:
                if (!input.empty()) {
                    input.clear();
                    matches = recompute_matches(input);
                    cursor = 0; scroll_top = 0;
                }
                break;
            case Key::Enter:
                if (matches.empty()) break;
                renderer.finalize(format_question(question_) + " " +
                                  style::cyan(options_[matches[cursor]]));
                return matches[cursor];
            case Key::Escape:
            case Key::CtrlC:
                renderer.finalize(format_question(question_) + " " + style::red("<cancelled>"));
                throw CancelledError();
            case Key::CtrlD:
                renderer.finalize(format_question(question_) + " " + style::red("<interrupted>"));
                throw InterruptedError();
            case Key::Char:
            case Key::Space: {
                input += (ev.key == Key::Space) ? std::string(1, ' ') : ev.text;
                matches = recompute_matches(input);
                if (cursor >= static_cast<int>(matches.size())) cursor = 0;
                int vp = std::min(page_size_, static_cast<int>(matches.size()));
                scroll_top = compute_scroll(cursor, 0, static_cast<int>(matches.size()), vp);
                break;
            }
            default:
                break;
        }
        draw();
    }
}

std::string Select::prompt() { return options_[do_prompt_()]; }
int Select::prompt_index()   { return do_prompt_(); }

Result<std::string> Select::try_prompt() {
    try { return Result<std::string>::ok(prompt()); }
    catch (const InquireError& e) {
        return Result<std::string>::err(e.code(), e.what());
    }
}

/* ============================ MultiSelect ============================ */

MultiSelect::MultiSelect(std::string question, std::vector<std::string> options)
    : question_(std::move(question)), options_(std::move(options)) {
    Terminal::init();
    ToUTF8();
}

MultiSelect& MultiSelect::page_size(int n)         { page_size_ = n > 0 ? n : 1; return *this; }
MultiSelect& MultiSelect::min_selected(int n)      { min_selected_ = n; return *this; }
MultiSelect& MultiSelect::max_selected(int n)      { max_selected_ = n; return *this; }
MultiSelect& MultiSelect::case_sensitive(bool on)  { case_sensitive_ = on; return *this; }
MultiSelect& MultiSelect::default_indices(std::vector<int> idx) {
    defaults_ = std::move(idx);
    return *this;
}
MultiSelect& MultiSelect::help_message(std::string m) { help_ = std::move(m); return *this; }

std::vector<int> MultiSelect::do_prompt_() {
    if (options_.empty()) throw EmptyOptionsError();

    Renderer renderer;
    std::vector<bool> selected(options_.size(), false);
    for (int i : defaults_) {
        if (i >= 0 && i < static_cast<int>(options_.size())) selected[i] = true;
    }

    auto recompute_matches = [&](const std::string& input) {
        std::vector<int> m;
        m.reserve(options_.size());
        for (std::size_t i = 0; i < options_.size(); ++i) {
            if (default_filter_match(options_[i], input, case_sensitive_)) {
                m.push_back(static_cast<int>(i));
            }
        }
        return m;
    };

    auto count_selected = [&]() {
        int n = 0; for (bool b : selected) if (b) ++n; return n;
    };

    std::string input;
    std::string error;
    auto matches = recompute_matches(input);
    int cursor = 0;
    int scroll_top = 0;

    auto draw = [&](const std::string& err) {
        auto term = Terminal::size();
        std::size_t max_width = term.first > 8 ? static_cast<std::size_t>(term.first - 8) : 1;

        std::vector<std::string> lines;
        lines.push_back(format_question(question_) + " " + style::dim(SEP_MARK) + " " + input);

        int viewport = std::min(page_size_, static_cast<int>(matches.size()));
        if (viewport == 0) {
            lines.push_back(style::dim("  (no matches)"));
        } else {
            for (int i = 0; i < viewport; ++i) {
                int idx = scroll_top + i;
                if (idx >= static_cast<int>(matches.size())) break;
                int orig = matches[idx];
                std::string box = selected[orig] ? style::green(CHECK_ON) : std::string(CHECK_OFF);
                std::string truncated = truncate_display(options_[orig], max_width);
                std::string line;
                if (idx == cursor) {
                    line = style::cyan(std::string(CURSOR_MARK) + " ") + box + " "
                           + style::cyan(truncated);
                } else {
                    line = "  " + box + " " + truncated;
                }
                lines.push_back(line);
            }
        }

        std::ostringstream hint;
        int total = static_cast<int>(matches.size());
        if (total > viewport && viewport > 0) {
            hint << "[" << (cursor + 1) << "/" << total << "]  ";
        }
        hint << "selected: " << count_selected();
        if (max_selected_ >= 0) hint << "/" << max_selected_;
        hint << "  ";
        hint << (help_.empty()
                 ? "↑↓ · space toggle · enter · esc"
                 : help_);
        lines.push_back(format_help(hint.str()));

        if (!err.empty()) lines.push_back(format_error(err));

        int header_col = static_cast<int>(header_prefix_width(question_)
                                          + utf8_display_width(input)) + 1;
        renderer.draw(lines, /*cursor_line=*/0, /*cursor_col=*/header_col);
    };

    draw(error);

    while (true) {
        KeyEvent ev = key_catch();
        int viewport = std::min(page_size_, static_cast<int>(matches.size()));
        switch (ev.key) {
            case Key::Up:
                if (!matches.empty()) {
                    cursor = (cursor - 1 + static_cast<int>(matches.size()))
                             % static_cast<int>(matches.size());
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::Down:
                if (!matches.empty()) {
                    cursor = (cursor + 1) % static_cast<int>(matches.size());
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::PageUp:
                if (!matches.empty()) {
                    cursor = std::max(0, cursor - page_size_);
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::PageDown:
                if (!matches.empty()) {
                    cursor = std::min(static_cast<int>(matches.size()) - 1, cursor + page_size_);
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::Home: cursor = 0; scroll_top = 0; break;
            case Key::End:
                if (!matches.empty()) {
                    cursor = static_cast<int>(matches.size()) - 1;
                    scroll_top = compute_scroll(cursor, scroll_top,
                                                static_cast<int>(matches.size()), viewport);
                }
                break;
            case Key::Space:
                if (!matches.empty()) {
                    int orig = matches[cursor];
                    if (!selected[orig] && max_selected_ >= 0
                        && count_selected() >= max_selected_) {
                        error = "cannot select more than " + std::to_string(max_selected_);
                    } else {
                        selected[orig] = !selected[orig];
                        error.clear();
                    }
                }
                break;
            case Key::Backspace: {
                if (!input.empty()) {
                    std::size_t pos = input.size();
                    utf8_erase_before(input, pos);
                    matches = recompute_matches(input);
                    if (cursor >= static_cast<int>(matches.size())) cursor = 0;
                    int vp = std::min(page_size_, static_cast<int>(matches.size()));
                    scroll_top = compute_scroll(cursor, 0, static_cast<int>(matches.size()), vp);
                }
                break;
            }
            case Key::CtrlU:
                if (!input.empty()) {
                    input.clear();
                    matches = recompute_matches(input);
                    cursor = 0; scroll_top = 0;
                }
                break;
            case Key::Enter: {
                int n = count_selected();
                if (n < min_selected_) {
                    error = "select at least " + std::to_string(min_selected_) + " option(s)";
                    break;
                }
                std::vector<int> result;
                for (std::size_t i = 0; i < selected.size(); ++i)
                    if (selected[i]) result.push_back(static_cast<int>(i));
                std::ostringstream summary;
                summary << format_question(question_) << " ";
                if (result.empty()) {
                    summary << style::dim("(none)");
                } else {
                    for (std::size_t i = 0; i < result.size(); ++i) {
                        if (i) summary << style::dim(", ");
                        summary << style::cyan(options_[result[i]]);
                    }
                }
                renderer.finalize(summary.str());
                return result;
            }
            case Key::Escape:
            case Key::CtrlC:
                renderer.finalize(format_question(question_) + " " + style::red("<cancelled>"));
                throw CancelledError();
            case Key::CtrlD:
                renderer.finalize(format_question(question_) + " " + style::red("<interrupted>"));
                throw InterruptedError();
            case Key::Char: {
                input += ev.text;
                matches = recompute_matches(input);
                if (cursor >= static_cast<int>(matches.size())) cursor = 0;
                int vp = std::min(page_size_, static_cast<int>(matches.size()));
                scroll_top = compute_scroll(cursor, 0, static_cast<int>(matches.size()), vp);
                break;
            }
            default: break;
        }
        draw(error);
    }
}

std::vector<std::string> MultiSelect::prompt() {
    auto idx = do_prompt_();
    std::vector<std::string> out;
    out.reserve(idx.size());
    for (int i : idx) out.push_back(options_[i]);
    return out;
}

std::vector<int> MultiSelect::prompt_indices() { return do_prompt_(); }

Result<std::vector<std::string>> MultiSelect::try_prompt() {
    try { return Result<std::vector<std::string>>::ok(prompt()); }
    catch (const InquireError& e) {
        return Result<std::vector<std::string>>::err(e.code(), e.what());
    }
}

/* =============================== Text =============================== */

Text::Text(std::string question) : question_(std::move(question)) {
    Terminal::init();
    ToUTF8();
}

Text& Text::default_value(std::string v)   { default_ = std::move(v); return *this; }
Text& Text::placeholder(std::string p)     { placeholder_ = std::move(p); return *this; }
Text& Text::help_message(std::string m)    { help_ = std::move(m); return *this; }
Text& Text::add_validator(Validator v)     { validators_.push_back(std::move(v)); return *this; }

std::string Text::do_prompt_() {
    Renderer renderer;
    std::string input;
    std::size_t cursor_byte = 0;
    std::string error;

    auto draw = [&]() {
        std::vector<std::string> lines;
        std::string body;
        std::size_t cursor_offset = 0;
        if (input.empty() && !default_.empty()) {
            body = style::dim("(" + default_ + ")");
        } else if (input.empty() && !placeholder_.empty()) {
            body = style::dim(placeholder_);
        } else {
            body = input;
            cursor_offset = utf8_display_width(input.substr(0, cursor_byte));
        }
        lines.push_back(format_question(question_) + " " + style::dim(SEP_MARK) + " " + body);
        if (!help_.empty()) lines.push_back(format_help(help_));
        if (!error.empty()) lines.push_back(format_error(error));

        int header_col = static_cast<int>(header_prefix_width(question_) + cursor_offset) + 1;
        renderer.draw(lines, /*cursor_line=*/0, /*cursor_col=*/header_col);
    };

    draw();

    while (true) {
        KeyEvent ev = key_catch();
        switch (ev.key) {
            case Key::Char:
                input.insert(cursor_byte, ev.text);
                cursor_byte += ev.text.size();
                error.clear();
                break;
            case Key::Space:
                input.insert(cursor_byte, 1, ' ');
                cursor_byte += 1;
                error.clear();
                break;
            case Key::Backspace:
                utf8_erase_before(input, cursor_byte);
                error.clear();
                break;
            case Key::Delete:
                utf8_erase_at(input, cursor_byte);
                error.clear();
                break;
            case Key::Left:
                utf8_move_left(input, cursor_byte);
                break;
            case Key::Right:
                utf8_move_right(input, cursor_byte);
                break;
            case Key::Home:
            case Key::CtrlA:
                cursor_byte = 0;
                break;
            case Key::End:
            case Key::CtrlE:
                cursor_byte = input.size();
                break;
            case Key::CtrlU:
                input.clear();
                cursor_byte = 0;
                error.clear();
                break;
            case Key::Enter: {
                std::string val = input.empty() ? default_ : input;
                try {
                    for (auto& v : validators_) v(val);
                } catch (const ValidationError& e) {
                    error = e.what();
                    draw();
                    continue;
                }
                renderer.finalize(format_question(question_) + " " +
                                  (val.empty() ? style::dim("(empty)") : style::cyan(val)));
                return val;
            }
            case Key::Escape:
            case Key::CtrlC:
                renderer.finalize(format_question(question_) + " " + style::red("<cancelled>"));
                throw CancelledError();
            case Key::CtrlD:
                renderer.finalize(format_question(question_) + " " + style::red("<interrupted>"));
                throw InterruptedError();
            default:
                break;
        }
        draw();
    }
}

std::string Text::prompt() { return do_prompt_(); }

Result<std::string> Text::try_prompt() {
    try { return Result<std::string>::ok(prompt()); }
    catch (const InquireError& e) {
        return Result<std::string>::err(e.code(), e.what());
    }
}

/* ============================== Password ============================== */

Password::Password(std::string question) : question_(std::move(question)) {
    Terminal::init();
    ToUTF8();
}

Password& Password::mask_char(char c)             { mask_ = c; return *this; }
Password& Password::show_typing(bool on)          { show_ = on; return *this; }
Password& Password::help_message(std::string m)   { help_ = std::move(m); return *this; }
Password& Password::add_validator(Validator v)    { validators_.push_back(std::move(v)); return *this; }

std::string Password::do_prompt_() {
    Renderer renderer;
    std::string input;
    std::string error;

    auto draw = [&]() {
        std::vector<std::string> lines;
        std::string visible;
        std::size_t cursor_offset;
        if (show_) {
            visible = input;
            cursor_offset = utf8_display_width(input);
        } else {
            std::size_t chars = utf8_char_count(input);
            visible.assign(chars, mask_);
            cursor_offset = chars;
        }
        lines.push_back(format_question(question_) + " " + style::dim(SEP_MARK) + " " + visible);
        if (!help_.empty()) lines.push_back(format_help(help_));
        if (!error.empty()) lines.push_back(format_error(error));

        int header_col = static_cast<int>(header_prefix_width(question_) + cursor_offset) + 1;
        renderer.draw(lines, /*cursor_line=*/0, /*cursor_col=*/header_col);
    };

    draw();

    while (true) {
        KeyEvent ev = key_catch();
        switch (ev.key) {
            case Key::Char:
                input += ev.text;
                error.clear();
                break;
            case Key::Space:
                input.push_back(' ');
                error.clear();
                break;
            case Key::Backspace: {
                std::size_t pos = input.size();
                utf8_erase_before(input, pos);
                error.clear();
                break;
            }
            case Key::CtrlU:
                input.clear();
                error.clear();
                break;
            case Key::Enter: {
                try {
                    for (auto& v : validators_) v(input);
                } catch (const ValidationError& e) {
                    error = e.what();
                    draw();
                    continue;
                }
                std::string final_mask(8, mask_);
                renderer.finalize(format_question(question_) + " " + style::cyan(final_mask));
                return input;
            }
            case Key::Escape:
            case Key::CtrlC:
                renderer.finalize(format_question(question_) + " " + style::red("<cancelled>"));
                throw CancelledError();
            case Key::CtrlD:
                renderer.finalize(format_question(question_) + " " + style::red("<interrupted>"));
                throw InterruptedError();
            default:
                break;
        }
        draw();
    }
}

std::string Password::prompt() { return do_prompt_(); }

Result<std::string> Password::try_prompt() {
    try { return Result<std::string>::ok(prompt()); }
    catch (const InquireError& e) {
        return Result<std::string>::err(e.code(), e.what());
    }
}

/* ============================== Confirm ============================== */

Confirm::Confirm(std::string question) : question_(std::move(question)) {
    Terminal::init();
    ToUTF8();
}

Confirm& Confirm::default_value(bool b) {
    default_ = b;
    has_default_ = true;
    return *this;
}

Confirm& Confirm::help_message(std::string m) { help_ = std::move(m); return *this; }

bool Confirm::prompt() {
    Renderer renderer;

    auto draw = [&]() {
        std::vector<std::string> lines;
        std::string yn = has_default_ ? (default_ ? "(Y/n)" : "(y/N)") : "(y/n)";
        lines.push_back(format_question(question_) + " " + style::dim(yn));
        if (!help_.empty()) lines.push_back(format_help(help_));
        renderer.draw(lines);
    };

    draw();

    while (true) {
        KeyEvent ev = key_catch();
        if (ev.key == Key::Enter) {
            if (!has_default_) continue;
            renderer.finalize(format_question(question_) + " " +
                              (default_ ? style::cyan("Yes") : style::red("No")));
            return default_;
        }
        if (ev.key == Key::Escape || ev.key == Key::CtrlC) {
            renderer.finalize(format_question(question_) + " " + style::red("<cancelled>"));
            throw CancelledError();
        }
        if (ev.key == Key::CtrlD) {
            renderer.finalize(format_question(question_) + " " + style::red("<interrupted>"));
            throw InterruptedError();
        }
        if (ev.is_char()) {
            const std::string& s = ev.text;
            if (s == "y" || s == "Y") {
                renderer.finalize(format_question(question_) + " " + style::cyan("Yes"));
                return true;
            }
            if (s == "n" || s == "N") {
                renderer.finalize(format_question(question_) + " " + style::red("No"));
                return false;
            }
        }
    }
}

Result<bool> Confirm::try_prompt() {
    try { return Result<bool>::ok(prompt()); }
    catch (const InquireError& e) {
        return Result<bool>::err(e.code(), e.what());
    }
}

}
