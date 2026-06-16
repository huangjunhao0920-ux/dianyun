#pragma once

#include "common/data_types.h"

namespace dianyun {

class PointCloudStitcher {
public:
    void append_scan(const LidarScan& scan, const Pose& pose);
    const PointCloud& stitched_cloud() const { return cloud_; }
    void clear() { cloud_ = PointCloud{}; }

private:
    PointCloud cloud_{};
};

}  // namespace dianyun
