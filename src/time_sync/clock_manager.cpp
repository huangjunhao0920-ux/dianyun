#include "time_sync/clock_manager.h"

#include <chrono>

namespace dianyun {

uint64_t ClockManager::now_ns() {
    const auto now = std::chrono::steady_clock::now().time_since_epoch();
    return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(now).count());
}

}  // namespace dianyun
