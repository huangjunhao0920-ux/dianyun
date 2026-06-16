#include "processing/pointcloud_stitcher.h"

#include <cmath>

namespace dianyun {

void PointCloudStitcher::append_scan(const LidarScan& scan, const Pose& pose) {
    cloud_.timestamp_ns = scan.timestamp_ns;
    for (size_t i = 0; i < scan.ranges_m.size(); ++i) {
        const float angle = scan.angle_min_rad + scan.angle_increment_rad * static_cast<float>(i);
        const float r = scan.ranges_m[i];

        PointXYZIC p;
        p.x = pose.x + r * std::cos(angle + pose.yaw);
        p.y = pose.y + r * std::sin(angle + pose.yaw);
        p.z = pose.z;
        p.intensity = 1.0f;
        cloud_.points.push_back(p);
    }
}

}  // namespace dianyun
