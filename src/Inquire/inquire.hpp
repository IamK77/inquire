#pragma once

#include "error.hpp"
#include "validator.hpp"
#include "utils/colorful.hpp"
#include "utils/console.hpp"
#include "utils/encode.hpp"
#include "utils/func.h"
#include "utils/types.h"

#include <functional>
#include <string>
#include <vector>

namespace Inquire {

class Select {
public:
    Select(std::string question, std::vector<std::string> options);

    Select& page_size(int n);
    Select& default_index(int i);
    Select& help_message(std::string msg);
    Select& filter(std::function<bool(const std::string&, const std::string&)> fn);
    Select& case_sensitive(bool on);

    std::string prompt();
    Result<std::string> try_prompt();
    int prompt_index();

private:
    std::string question_;
    std::vector<std::string> options_;
    int page_size_ = 7;
    int default_index_ = 0;
    bool case_sensitive_ = false;
    std::string help_;
    std::function<bool(const std::string&, const std::string&)> filter_;

    int do_prompt_();
};

class MultiSelect {
public:
    MultiSelect(std::string question, std::vector<std::string> options);

    MultiSelect& page_size(int n);
    MultiSelect& default_indices(std::vector<int> idx);
    MultiSelect& help_message(std::string msg);
    MultiSelect& min_selected(int n);
    MultiSelect& max_selected(int n);
    MultiSelect& case_sensitive(bool on);

    std::vector<std::string> prompt();
    Result<std::vector<std::string>> try_prompt();
    std::vector<int> prompt_indices();

private:
    std::string question_;
    std::vector<std::string> options_;
    int page_size_ = 10;
    int min_selected_ = 0;
    int max_selected_ = -1;
    bool case_sensitive_ = false;
    std::vector<int> defaults_;
    std::string help_;

    std::vector<int> do_prompt_();
};

class Text {
public:
    explicit Text(std::string question);

    Text& default_value(std::string v);
    Text& placeholder(std::string p);
    Text& help_message(std::string m);
    Text& add_validator(Validator v);

    std::string prompt();
    Result<std::string> try_prompt();

private:
    std::string question_;
    std::string default_;
    std::string placeholder_;
    std::string help_;
    std::vector<Validator> validators_;

    std::string do_prompt_();
};

class Password {
public:
    explicit Password(std::string question);

    Password& mask_char(char c);
    Password& show_typing(bool on);
    Password& help_message(std::string m);
    Password& add_validator(Validator v);

    std::string prompt();
    Result<std::string> try_prompt();

private:
    std::string question_;
    char mask_ = '*';
    bool show_ = false;
    std::string help_;
    std::vector<Validator> validators_;

    std::string do_prompt_();
};

class Confirm {
public:
    explicit Confirm(std::string question);

    Confirm& default_value(bool b);
    Confirm& help_message(std::string m);

    bool prompt();
    Result<bool> try_prompt();

private:
    std::string question_;
    bool default_ = true;
    bool has_default_ = false;
    std::string help_;
};

}
