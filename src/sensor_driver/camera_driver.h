#pragma once

#include <mutex>

#include "common/data_types.h"
#include "sensor_driver/sensor_base.h"

namespace dianyun {

class CameraDriver : public SensorBase {
public:
    bool start() override { running_ = true; return true; }
    void stop() override { running_ = false; }
    const char* name() const override { return "camera_driver"; }

    void push_frame(const CameraFrame& frame) {
        std::lock_guard<std::mutex> lock(mu_);
        latest_frame_ = frame;
        has_frame_ = true;
    }

    bool pop_latest_frame(CameraFrame& out) {
        std::lock_guard<std::mutex> lock(mu_);
        if (!has_frame_) {
            return false;
        }
        out = latest_frame_;
        return true;
    }

private:
    bool running_{false};
    bool has_frame_{false};
    CameraFrame latest_frame_;
    std::mutex mu_;
};

}  // namespace dianyun
