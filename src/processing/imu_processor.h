#pragma once

#include "common/data_types.h"

namespace dianyun {

class ImuProcessor {
public:
    Pose integrate(const ImuSample& sample, double dt_sec);
    const Pose& current_pose() const { return pose_; }

private:
    Pose pose_{};
};

}  // namespace dianyun
