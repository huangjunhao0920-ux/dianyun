#ifndef DIANYUN_COMMON_DATA_TYPES_H
#define DIANYUN_COMMON_DATA_TYPES_H

#include <vector>
#include <cstdint>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>

namespace dianyun {

// ============ Basic Data Types ============

struct Point3D {
    float x, y, z;
    Point3D() : x(0), y(0), z(0) {}
    Point3D(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

struct PointXYZRGB {
    float x, y, z;
    uint8_t r, g, b;
    PointXYZRGB() : x(0), y(0), z(0), r(0), g(0), b(0) {}
};

struct PointXYZIRC {
    float x, y, z;
    uint8_t intensity;
    uint8_t r, g, b;
    uint32_t classification;
    PointXYZIRC() : x(0), y(0), z(0), intensity(0), r(0), g(0), b(0), classification(0) {}
};

// ============ Sensor Data ============

struct IMUData {
    uint64_t timestamp;        // nanoseconds
    Eigen::Vector3f accel;     // m/s^2
    Eigen::Vector3f gyro;      // rad/s
    Eigen::Vector3f mag;       // Tesla
    float temperature;         // Celsius
};

struct LiDARScan {
    uint64_t timestamp;
    std::vector<float> distances;      // meters
    std::vector<float> angles;         // radians
    std::vector<uint8_t> intensity;    // 0-255
    uint32_t num_points;
};

struct CameraFrame {
    uint64_t timestamp;
    cv::Mat image;             // BGR format
    uint32_t width;
    uint32_t height;
    std::string camera_id;
};

// ============ Processed Data ============

struct Pose {
    Eigen::Vector3f position;           // x, y, z
    Eigen::Quaternionf orientation;    // quaternion
    uint64_t timestamp;
    
    Eigen::Matrix3f get_rotation_matrix() const {
        return orientation.toRotationMatrix();
    }
};

struct PointCloud {
    uint64_t timestamp;
    std::vector<PointXYZIRC> points;
    uint32_t width;  // number of points
    uint32_t height; // usually 1 for unorganized
    bool is_dense;   // no NaN or Inf values
    
    size_t size() const { return points.size(); }
    void clear() { points.clear(); }
};

struct SyncedSensorData {
    uint64_t sync_timestamp;
    IMUData imu;
    LiDARScan lidar;
    CameraFrame camera;
    bool is_valid;
};

// ============ Configuration ============

struct SensorCalibration {
    // Camera intrinsics
    float fx, fy, cx, cy;  // focal length and principal point
    float k1, k2, p1, p2;  // distortion coefficients
    
    // LiDAR to camera extrinsics
    Eigen::Matrix4f lidar_to_camera;  // 4x4 transformation matrix
    
    // IMU to camera extrinsics
    Eigen::Matrix4f imu_to_camera;
};

} // namespace dianyun

#endif // DIANYUN_COMMON_DATA_TYPES_H
