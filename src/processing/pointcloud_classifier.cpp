#include "processing/pointcloud_classifier.h"

namespace dianyun {

void PointCloudClassifier::classify(PointCloud& cloud, float ground_threshold_m) const {
    for (auto& point : cloud.points) {
        point.cls = (point.z <= ground_threshold_m) ? PointClass::Ground : PointClass::Obstacle;
    }
}

}  // namespace dianyun
