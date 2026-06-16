#include <cassert>

#include "processing/imu_processor.h"

int main() {
    dianyun::ImuProcessor processor;
    dianyun::ImuSample imu;
    imu.gx = 1.0f;
    imu.timestamp_ns = 100;

    const auto pose = processor.integrate(imu, 0.5);
    assert(pose.roll > 0.49f && pose.roll < 0.51f);
    assert(pose.timestamp_ns == 100);
    return 0;
}
