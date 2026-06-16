#include <cassert>

#include "sensor_driver/camera_driver.h"
#include "sensor_driver/imu_driver.h"
#include "sensor_driver/lidar_driver.h"

int main() {
    dianyun::LidarDriver lidar;
    dianyun::ImuDriver imu;
    dianyun::CameraDriver camera;

    assert(lidar.start());
    assert(imu.start());
    assert(camera.start());

    dianyun::LidarScan scan;
    scan.ranges_m = {1.0f};
    lidar.push_scan(scan);
    dianyun::LidarScan out_scan;
    assert(lidar.pop_latest_scan(out_scan));

    dianyun::ImuSample imu_sample;
    imu.push_sample(imu_sample);
    dianyun::ImuSample out_imu;
    assert(imu.pop_latest_sample(out_imu));

    dianyun::CameraFrame frame;
    frame.rgb = {1, 2, 3};
    camera.push_frame(frame);
    dianyun::CameraFrame out_frame;
    assert(camera.pop_latest_frame(out_frame));

    return 0;
}
