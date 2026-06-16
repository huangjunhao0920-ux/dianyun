#ifndef DIANYUN_COMMON_CONSTANTS_H
#define DIANYUN_COMMON_CONSTANTS_H

namespace dianyun {

// ============ Physical Constants ============
const float GRAVITY = 9.81f;           // m/s^2
const float DEG_TO_RAD = 3.14159265f / 180.0f;
const float RAD_TO_DEG = 180.0f / 3.14159265f;

// ============ Sensor Parameters ============
// LiDAR
const float LIDAR_MIN_RANGE = 0.15f;   // meters
const float LIDAR_MAX_RANGE = 10.0f;   // meters
const int LIDAR_POINTS_PER_SCAN = 1024; // single-line LiDAR

// IMU
const float IMU_ACCEL_RANGE = 16.0f;   // ±16 g
const float IMU_GYRO_RANGE = 2000.0f;  // ±2000 deg/s
const int IMU_SAMPLE_RATE = 100;       // Hz

// Camera
const int CAMERA_WIDTH = 1280;
const int CAMERA_HEIGHT = 960;
const int CAMERA_FPS = 30;

// ============ Processing Parameters ============
// Time sync
const int TIME_SYNC_BUFFER_SIZE = 100;
const int TIME_SYNC_TIMEOUT_MS = 100;

// Point cloud
const int POINTCLOUD_MAX_POINTS = 100000;
const int POINTCLOUD_ACCUMULATION_FRAMES = 10;  // frames to accumulate

// Classification
const float GROUND_HEIGHT_THRESHOLD = 0.1f;     // meters
const float GROUND_SLOPE_THRESHOLD = 15.0f;     // degrees

// ============ Display Parameters ============
const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const int TARGET_FPS = 30;
const float TARGET_FRAME_TIME = 1.0f / TARGET_FPS;  // seconds

// ============ Thread Parameters ============
const int NUM_PROCESSING_THREADS = 4;
const int SENSOR_QUEUE_SIZE = 100;

} // namespace dianyun

#endif // DIANYUN_COMMON_CONSTANTS_H
