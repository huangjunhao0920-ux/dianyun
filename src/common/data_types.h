#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace dianyun {

struct LidarScan {
    uint64_t timestamp_ns{0};
    float angle_min_rad{-3.1415926f};
    float angle_increment_rad{0.0174533f};
    std::vector<float> ranges_m;
};

struct ImuSample {
    uint64_t timestamp_ns{0};
    float ax{0.0f};
    float ay{0.0f};
    float az{0.0f};
    float gx{0.0f};
    float gy{0.0f};
    float gz{0.0f};
};

struct CameraFrame {
    uint64_t timestamp_ns{0};
    uint32_t width{0};
    uint32_t height{0};
    std::vector<uint8_t> rgb;
};

struct Pose {
    uint64_t timestamp_ns{0};
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
    float roll{0.0f};
    float pitch{0.0f};
    float yaw{0.0f};
};

enum class PointClass : uint8_t {
    Ground = 0,
    Obstacle = 1,
    Unknown = 255
};

struct PointXYZIC {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
    float intensity{0.0f};
    PointClass cls{PointClass::Unknown};
    std::array<uint8_t, 3> color{{0, 0, 0}};
};

struct PointCloud {
    uint64_t timestamp_ns{0};
    std::vector<PointXYZIC> points;
};

}  // namespace dianyun
