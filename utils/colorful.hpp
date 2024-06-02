#pragma once

#include <iostream>
#include <string>

std::string to_string(const std::string &msg);

std::string to_string(const char &msg);

template <typename T>
std::string red(const T &msg) {
    return "\033[31m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string green(const T &msg) {
    return "\033[32m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string yellow(const T &msg) {
    return "\033[33m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string blue(const T &msg) {
    return "\033[34m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string magenta(const T &msg) {
    return "\033[35m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string cyan(const T &msg) {
    return "\033[36m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string white(const T &msg) {
    return "\033[37m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string black(const T &msg) {
    return "\033[30m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string bold(const T &msg) {
    return "\033[1m" + to_string(msg) + "\033[0m";
}

template <typename T>
std::string underline(const T &msg) {
    return "\033[4m" + to_string(msg) + "\033[0m";
}