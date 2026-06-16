#pragma once

#include <cstdint>

namespace dianyun {

class ClockManager {
public:
    static uint64_t now_ns();
};

}  // namespace dianyun
