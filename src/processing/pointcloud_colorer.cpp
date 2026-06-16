#include "processing/pointcloud_colorer.h"

namespace dianyun {

void PointCloudColorer::colorize(PointCloud& cloud, const CameraFrame& frame) const {
    for (auto& p : cloud.points) {
        if (frame.rgb.size() >= 3) {
            p.color = {{frame.rgb[0], frame.rgb[1], frame.rgb[2]}};
            continue;
        }

        if (p.cls == PointClass::Ground) {
            p.color = {{80, 180, 80}};
        } else if (p.cls == PointClass::Obstacle) {
            p.color = {{220, 50, 50}};
        } else {
            p.color = {{128, 128, 128}};
        }
    }
}

}  // namespace dianyun
