#pragma once

#ifdef __linux__

#include <termios.h>

namespace Inquire {

class TtyGuard {
public:
    TtyGuard();
    ~TtyGuard();

    TtyGuard(const TtyGuard&) = delete;
    TtyGuard& operator=(const TtyGuard&) = delete;

    void enter_raw();
    void restore();

    static void install_signal_handler();

private:
    bool active_ = false;
    bool saved_ok_ = false;
};

}

#endif
