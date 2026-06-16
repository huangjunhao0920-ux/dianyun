#pragma once

#include <mutex>

#include "common/data_types.h"
#include "sensor_driver/sensor_base.h"

namespace dianyun {

class LidarDriver : public SensorBase {
public:
    bool start() override { running_ = true; return true; }
    void stop() override { running_ = false; }
    const char* name() const override { return "lidar_driver"; }

    void push_scan(const LidarScan& scan) {
        std::lock_guard<std::mutex> lock(mu_);
        latest_scan_ = scan;
        has_scan_ = true;
    }

    bool pop_latest_scan(LidarScan& out) {
        std::lock_guard<std::mutex> lock(mu_);
        if (!has_scan_) {
            return false;
        }
        out = latest_scan_;
        return true;
    }

private:
    bool running_{false};
    bool has_scan_{false};
    LidarScan latest_scan_;
    std::mutex mu_;
};

}  // namespace dianyun
