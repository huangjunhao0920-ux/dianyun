#include "processing/imu_processor.h"

namespace dianyun {

Pose ImuProcessor::integrate(const ImuSample& sample, double dt_sec) {
    pose_.timestamp_ns = sample.timestamp_ns;
    pose_.roll += static_cast<float>(sample.gx * dt_sec);
    pose_.pitch += static_cast<float>(sample.gy * dt_sec);
    pose_.yaw += static_cast<float>(sample.gz * dt_sec);
    pose_.x += static_cast<float>(sample.ax * dt_sec * dt_sec * 0.5);
    pose_.y += static_cast<float>(sample.ay * dt_sec * dt_sec * 0.5);
    pose_.z += static_cast<float>(sample.az * dt_sec * dt_sec * 0.5);
    return pose_;
}

}  // namespace dianyun
