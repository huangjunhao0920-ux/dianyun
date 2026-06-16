#pragma once

#include "common/data_types.h"

namespace dianyun {

class Visualizer {
public:
    void update(const PointCloud& cloud);
    double last_fps() const { return fps_; }
    const PointCloud& latest_cloud() const { return latest_cloud_; }

private:
    PointCloud latest_cloud_{};
    uint64_t last_timestamp_ns_{0};
    double fps_{0.0};
};

}  // namespace dianyun
