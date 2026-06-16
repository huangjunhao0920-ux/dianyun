#include "visualization/visualizer.h"

namespace dianyun {

void Visualizer::update(const PointCloud& cloud) {
    if (last_timestamp_ns_ != 0 && cloud.timestamp_ns > last_timestamp_ns_) {
        const double delta_ns = static_cast<double>(cloud.timestamp_ns - last_timestamp_ns_);
        fps_ = 1e9 / delta_ns;
    }
    last_timestamp_ns_ = cloud.timestamp_ns;
    latest_cloud_ = cloud;
}

}  // namespace dianyun
