#pragma once

#include <mutex>

#include "common/data_types.h"
#include "sensor_driver/sensor_base.h"

namespace dianyun {

class ImuDriver : public SensorBase {
public:
    bool start() override { running_ = true; return true; }
    void stop() override { running_ = false; }
    const char* name() const override { return "imu_driver"; }

    void push_sample(const ImuSample& sample) {
        std::lock_guard<std::mutex> lock(mu_);
        latest_sample_ = sample;
        has_sample_ = true;
    }

    bool pop_latest_sample(ImuSample& out) {
        std::lock_guard<std::mutex> lock(mu_);
        if (!has_sample_) {
            return false;
        }
        out = latest_sample_;
        return true;
    }

private:
    bool running_{false};
    bool has_sample_{false};
    ImuSample latest_sample_;
    std::mutex mu_;
};

}  // namespace dianyun
