#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

namespace dianyun {

class TimeSynchronizer {
public:
    void set_offset_ns(const std::string& sensor_id, int64_t offset_ns);
    uint64_t align_timestamp_ns(const std::string& sensor_id, uint64_t raw_ts_ns) const;

private:
    std::unordered_map<std::string, int64_t> offsets_ns_;
};

}  // namespace dianyun
