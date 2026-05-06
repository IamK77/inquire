#pragma once

#include "error.hpp"

#include <functional>
#include <regex>
#include <string>
#include <vector>

namespace Inquire {

using Validator = std::function<void(const std::string&)>;

namespace validators {

inline Validator non_empty(std::string msg = "input cannot be empty") {
    return [msg](const std::string& s) {
        if (s.empty()) throw ValidationError(msg);
    };
}

inline Validator min_length(std::size_t n, std::string msg = "") {
    return [n, msg](const std::string& s) {
        if (s.size() < n) {
            throw ValidationError(msg.empty()
                ? ("input must be at least " + std::to_string(n) + " characters")
                : msg);
        }
    };
}

inline Validator max_length(std::size_t n, std::string msg = "") {
    return [n, msg](const std::string& s) {
        if (s.size() > n) {
            throw ValidationError(msg.empty()
                ? ("input must be at most " + std::to_string(n) + " characters")
                : msg);
        }
    };
}

inline Validator length_between(std::size_t lo, std::size_t hi, std::string msg = "") {
    return [lo, hi, msg](const std::string& s) {
        if (s.size() < lo || s.size() > hi) {
            throw ValidationError(msg.empty()
                ? ("input length must be between " + std::to_string(lo)
                   + " and " + std::to_string(hi))
                : msg);
        }
    };
}

inline Validator matches(std::string pattern, std::string msg = "") {
    std::regex re(pattern);
    return [re, msg, pattern](const std::string& s) {
        if (!std::regex_match(s, re)) {
            throw ValidationError(msg.empty()
                ? ("input does not match pattern: " + pattern)
                : msg);
        }
    };
}

inline Validator integer(std::string msg = "input must be an integer") {
    return [msg](const std::string& s) {
        if (s.empty()) throw ValidationError(msg);
        std::size_t i = (s[0] == '-' || s[0] == '+') ? 1 : 0;
        if (i == s.size()) throw ValidationError(msg);
        for (; i < s.size(); ++i) {
            if (s[i] < '0' || s[i] > '9') throw ValidationError(msg);
        }
    };
}

inline Validator all_of(std::vector<Validator> vs) {
    return [vs](const std::string& s) {
        for (const auto& v : vs) v(s);
    };
}

}
}
