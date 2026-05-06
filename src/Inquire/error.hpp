#pragma once

#include <stdexcept>
#include <string>

namespace Inquire {

enum class ErrorCode {
    Cancelled,
    Interrupted,
    InvalidInput,
    EmptyOptions,
    Io,
    UnsupportedTerminal
};

class InquireError : public std::runtime_error {
public:
    InquireError(ErrorCode code, const std::string& msg)
        : std::runtime_error(msg), code_(code) {}
    ErrorCode code() const noexcept { return code_; }
private:
    ErrorCode code_;
};

class CancelledError : public InquireError {
public:
    CancelledError()
        : InquireError(ErrorCode::Cancelled, "prompt was cancelled by user") {}
};

class InterruptedError : public InquireError {
public:
    InterruptedError()
        : InquireError(ErrorCode::Interrupted, "prompt was interrupted") {}
};

class ValidationError : public InquireError {
public:
    explicit ValidationError(const std::string& msg)
        : InquireError(ErrorCode::InvalidInput, msg) {}
};

class EmptyOptionsError : public InquireError {
public:
    EmptyOptionsError()
        : InquireError(ErrorCode::EmptyOptions, "no options were provided") {}
};

class IoError : public InquireError {
public:
    explicit IoError(const std::string& msg)
        : InquireError(ErrorCode::Io, msg) {}
};

template <typename T>
class Result {
public:
    static Result<T> ok(T value) {
        Result<T> r;
        r.has_value_ = true;
        r.value_ = std::move(value);
        return r;
    }
    static Result<T> err(ErrorCode code, std::string msg) {
        Result<T> r;
        r.has_value_ = false;
        r.code_ = code;
        r.error_ = std::move(msg);
        return r;
    }

    bool ok() const noexcept { return has_value_; }
    explicit operator bool() const noexcept { return has_value_; }

    const T& value() const& {
        if (!has_value_) throw InquireError(code_, error_);
        return value_;
    }
    T& value() & {
        if (!has_value_) throw InquireError(code_, error_);
        return value_;
    }
    T value_or(T fallback) const { return has_value_ ? value_ : fallback; }

    ErrorCode error_code() const noexcept { return code_; }
    const std::string& error_message() const noexcept { return error_; }

private:
    Result() = default;
    bool has_value_ = false;
    T value_{};
    ErrorCode code_ = ErrorCode::Io;
    std::string error_;
};

}
