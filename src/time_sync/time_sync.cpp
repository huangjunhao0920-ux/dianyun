#include "time_sync/time_sync.h"

namespace dianyun {

void TimeSynchronizer::set_offset_ns(const std::string& sensor_id, int64_t offset_ns) {
    offsets_ns_[sensor_id] = offset_ns;
}

uint64_t TimeSynchronizer::align_timestamp_ns(const std::string& sensor_id, uint64_t raw_ts_ns) const {
    const auto it = offsets_ns_.find(sensor_id);
    if (it == offsets_ns_.end()) {
        return raw_ts_ns;
    }

    const int64_t aligned = static_cast<int64_t>(raw_ts_ns) + it->second;
    return aligned > 0 ? static_cast<uint64_t>(aligned) : 0ULL;
}

}  // namespace dianyun
