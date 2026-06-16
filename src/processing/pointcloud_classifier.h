#pragma once

#include "common/data_types.h"

namespace dianyun {

class PointCloudClassifier {
public:
    void classify(PointCloud& cloud, float ground_threshold_m = 0.15f) const;
};

}  // namespace dianyun
