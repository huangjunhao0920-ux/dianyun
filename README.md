# Dianyun - MDC Real-time Point Cloud Fusion System

Dianyun is a C++14 project scaffold for a Huawei MDC-oriented multi-sensor real-time point cloud fusion pipeline.

## Hardware Inputs
- Four-module panoramic camera (360°)
- IMU
- Single-line LiDAR

## Core Capabilities (Scaffold)
1. Multi-sensor data collection interfaces (`lidar_driver`, `imu_driver`, `camera_driver`)
2. Time synchronization (`time_sync`, `clock_manager`)
3. Point cloud processing (`imu_processor`, `pointcloud_stitcher`, `pointcloud_classifier`, `pointcloud_colorer`)
4. Real-time visualization interface (`visualizer`)
5. MDC-ready processing flow with <33ms design target and 30+ FPS rendering target

## Quick Start
```bash
cmake -S . -B build
cmake --build build -j
ctest --test-dir build --output-on-failure
```

### Optional full dependency mode
The default scaffold mode does not require external SDK dependencies, so the structure can be built immediately.

To validate package discovery for MDC/full deployment mode:
```bash
cmake -S . -B build -DDIANYUN_ENABLE_EXTERNAL_DEPS=ON
```

Expected dependencies in full mode:
- OpenCV
- Eigen3
- PCL
- Boost
- OpenGL / GLFW3 / GLEW

## Project Structure
```text
dianyun/
├── CMakeLists.txt
├── MDC_Architecture_Design.md
├── README.md
├── docs/
│   ├── API_Reference.md
│   └── Tuning_Guide.md
├── config/
│   ├── sensor_config.yaml
│   └── calibration.yaml
├── src/
│   ├── main.cpp
│   ├── common/
│   ├── sensor_driver/
│   ├── time_sync/
│   ├── processing/
│   ├── visualization/
│   └── utils/
└── test/
```

## Development Roadmap
- Phase 1: Framework and interfaces
- Phase 2: Sensor adapters on MDC SDK APIs
- Phase 3: Time sync + IMU pose + LiDAR stitching
- Phase 4: Classification + coloring
- Phase 5: OpenGL rendering and MDC display output
- Phase 6: Performance tuning and deployment

## MDC Integration Notes
- Message queue / thread-ready modular APIs are defined for adapter implementation.
- Data structures are centralized in `src/common/data_types.h`.
- Output interfaces are prepared for real-time display module integration.
