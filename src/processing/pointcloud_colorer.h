#pragma once

#include "common/data_types.h"

namespace dianyun {

class PointCloudColorer {
public:
    void colorize(PointCloud& cloud, const CameraFrame& frame) const;
};

}  // namespace dianyun
