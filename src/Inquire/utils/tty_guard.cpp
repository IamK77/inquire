#include "tty_guard.hpp"

#ifdef __linux__

#include <atomic>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <termios.h>
#include <unistd.h>

namespace Inquire {

namespace {
    termios g_saved_termios{};
    std::atomic<bool> g_saved_ok{false};
    std::atomic<bool> g_save_done{false};
    std::atomic<bool> g_signal_installed{false};

    void save_once() {
        bool expected = false;
        if (!g_save_done.compare_exchange_strong(expected, true)) return;
        bool ok = (tcgetattr(STDIN_FILENO, &g_saved_termios) == 0);
        g_saved_ok.store(ok);
    }

    void on_fatal_signal(int sig) {
        if (g_saved_ok.load()) {
            tcsetattr(STDIN_FILENO, TCSANOW, &g_saved_termios);
        }
        const char show[] = "\033[?25h";
        ssize_t n = write(STDOUT_FILENO, show, sizeof(show) - 1);
        (void)n;
        std::signal(sig, SIG_DFL);
        std::raise(sig);
    }
}

TtyGuard::TtyGuard() {
    save_once();
    install_signal_handler();
    saved_ok_ = g_saved_ok.load();
}

TtyGuard::~TtyGuard() {
    restore();
}

void TtyGuard::enter_raw() {
    if (!saved_ok_ || active_) return;
    struct termios raw = g_saved_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0) {
        active_ = true;
    }
}

void TtyGuard::restore() {
    if (!saved_ok_ || !active_) return;
    tcsetattr(STDIN_FILENO, TCSANOW, &g_saved_termios);
    active_ = false;
}

void TtyGuard::install_signal_handler() {
    bool expected = false;
    if (!g_signal_installed.compare_exchange_strong(expected, true)) return;
    std::signal(SIGINT,  on_fatal_signal);
    std::signal(SIGTERM, on_fatal_signal);
    std::signal(SIGSEGV, on_fatal_signal);
}

}

#endif
