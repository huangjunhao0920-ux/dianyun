#include <cassert>

#include "visualization/visualizer.h"

int main() {
    dianyun::Visualizer viz;

    dianyun::PointCloud cloud_a;
    cloud_a.timestamp_ns = 1000;
    cloud_a.points.resize(1);
    viz.update(cloud_a);

    dianyun::PointCloud cloud_b;
    cloud_b.timestamp_ns = 2000;
    cloud_b.points.resize(2);
    viz.update(cloud_b);

    assert(viz.latest_cloud().points.size() == 2);
    assert(viz.last_fps() > 0.0);
    return 0;
}
