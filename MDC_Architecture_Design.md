# MDC Architecture Design - Dianyun Real-time Point Cloud Fusion

## 1. System Goal
Build a Huawei MDC-compatible real-time pipeline that ingests panoramic camera + IMU + single-line LiDAR data and outputs classified, colorized 3D point clouds for display.

## 2. End-to-end Data Flow
1. **Sensor Driver Layer**
   - `lidar_driver`: LiDAR scan ingestion
   - `imu_driver`: high-rate IMU ingestion
   - `camera_driver`: panoramic image ingestion
2. **Time Synchronization Layer**
   - Timestamp alignment to a shared reference clock
3. **Processing Layer**
   - IMU integration -> pose estimation
   - LiDAR stitching with pose -> incremental 3D point cloud
   - Semantic class assignment (ground/obstacle/...)
   - Camera projection-based colorization
4. **Visualization Layer**
   - Real-time renderer interface (OpenGL-ready)
   - 30+ FPS target, <33 ms frame latency budget

## 3. Module Layout
- `src/sensor_driver/`
- `src/time_sync/`
- `src/processing/`
- `src/visualization/`
- `src/utils/`
- `config/` for sensor and calibration parameters
- `test/` for unit and integration tests

## 4. Timing and Thread Model (MDC-ready)
- Dedicated ingest threads for LiDAR, IMU, and camera
- Time-sync queue aligns messages by corrected timestamp
- Processing thread executes fusion stages in deterministic order
- Visualization/output thread decoupled from ingest to avoid sensor backpressure

## 5. Real-time Constraints
- **Target FPS**: >= 30
- **Latency budget**: < 33 ms
- **Suggested split**:
  - Ingest + sync: 5 ms
  - Pose + stitching: 10 ms
  - Classification + coloring: 8 ms
  - Visualization/output: 10 ms

## 6. Build and Dependencies
- Language: C++14+
- Build: CMake 3.10+
- Full-stack dependencies (deployment mode): OpenCV, Eigen3, PCL, Boost, OpenGL, GLFW3, GLEW
- Scaffold mode keeps interfaces buildable without SDK install friction.

## 7. Phase Roadmap
- **Phase 1**: Project bootstrap, interfaces, CMake
- **Phase 2**: MDC sensor adapter implementation
- **Phase 3**: Time sync + IMU pose + scan stitching
- **Phase 4**: Classification + camera colorization
- **Phase 5**: Renderer integration and display output
- **Phase 6**: Profiling, tuning, and on-device deployment

## 8. Key Deliverables in This Initialization
- Base project structure and module skeleton
- Build system and tests scaffold
- Documentation set (README, architecture, API, tuning)
- Example configs (`sensor_config.yaml`, `calibration.yaml`)
