# Dianyun - MDC平台实时点云融合系统

![License](https://img.shields.io/badge/license-MIT-blue)
![C++](https://img.shields.io/badge/language-C%2B%2B-brightgreen)
![Status](https://img.shields.io/badge/status-Development-yellow)

## 项目概述

**Dianyun** 是一个基于华为MDC（Mobile Data Center）平台的实时多传感器点云融合系统。该项目整合四模组全景相机、IMU和单线扫描LiDAR的数据，实现：

- ✅ **实时数据采集** - 多传感器同步读取
- ✅ **时间同步** - 多传感器时间戳对齐
- ✅ **点云拼接** - 单线LiDAR转3D点云
- ✅ **点云分类** - 地面、障碍物等智能分割
- ✅ **点云赋色** - 全景相机图像投影着色
- ✅ **实时显示** - OpenGL 3D可视化输出

## 系统架构

### 核心数据流

```
传感器数据 → 采集驱动 → 时间同步 → 数据处理 → 多传感器融合 → 实时显示
(相机/IMU/  (原始数据)  (时间戳    (去噪、滤波)  (点云拼接、   (OpenGL
 LiDAR)               对齐)                    赋色、分类)   渲染)
```

### 系统模块

| 模块 | 功能 | 状态 |
|---|---|---|
| 传感器驱动层 | LiDAR/IMU/相机驱动 | 📋 规划中 |
| 时间同步层 | 多传感器时间戳对齐 | 📋 规划中 |
| 数据处理层 | IMU处理、相机处理 | 📋 规划中 |
| 融合处理层 | 点云拼接、分类、赋色 | 📋 规划中 |
| 显示层 | OpenGL可视化 | 📋 规划中 |

## 快速开始

### 系统要求

- **操作系统**：Ubuntu 18.04 / 20.04 (推荐)
- **编译器**：GCC 7.0+ 或 Clang 6.0+
- **CMake**：3.10+
- **C++**：C++14 或更高

### 依赖库安装

```bash
# 基础依赖
sudo apt-get install build-essential cmake git

# 核心库
sudo apt-get install libopencv-dev libeigen3-dev libboost-all-dev

# 点云处理
sudo apt-get install libpcl-dev

# 显示库
sudo apt-get install freeglut3-dev libglfw3-dev libglew-dev
```

### 编译和运行

```bash
# 克隆仓库
git clone https://github.com/huangjunhao0920-ux/dianyun.git
cd dianyun

# 创建构建目录
mkdir build && cd build

# 编译
cmake ..
make -j4

# 运行测试
./bin/test_time_sync
./bin/test_pointcloud

# 运行主程序
./bin/mdc_pointcloud_app
```

## 项目结构

```
dianyun/
├── CMakeLists.txt                    # CMake构建配置
├── README.md                         # 项目说明文档
├── MDC_Architecture_Design.md        # 详细架构设计文档
│
├── src/
│   ├── main.cpp                      # MDC应用主程序
│   ├── common/
│   │   ├── data_types.h              # 数据结构定义
│   │   └── constants.h               # 常量定义
│   │
│   ├── sensor_driver/                # 传感器驱动模块
│   │   ├── sensor_base.h
│   │   ├── lidar_driver.h/cpp
│   │   ├── imu_driver.h/cpp
│   │   └── camera_driver.h/cpp
│   │
│   ├── time_sync/                    # 时间同步模块
│   │   ├── time_sync.h/cpp
│   │   └── clock_manager.h/cpp
│   │
│   ├── processing/                   # 数据处理模块
│   │   ├── imu_processor.h/cpp
│   │   ├── camera_processor.h/cpp
│   │   ├── pointcloud_stitcher.h/cpp
│   │   ├── pointcloud_classifier.h/cpp
│   │   └── pointcloud_colorer.h/cpp
│   │
│   ├── visualization/                # 可视化显示模块
│   │   ├── visualizer.h/cpp
│   │   ├── camera.h/cpp
│   │   ├── shader.h
│   │   └── shaders/
│   │       ├── point.vs
│   │       └── point.fs
│   │
│   ├── config/                       # 配置文件
│   │   ├── sensor_config.yaml
│   │   ├── calibration.yaml
│   │   └── process_config.yaml
│   │
│   └── utils/                        # 工具函数
│       ├── logger.h/cpp
│       ├── math_utils.h/cpp
│       └── timer.h/cpp
│
├── test/                             # 单元测试
│   ├── CMakeLists.txt
│   ├── test_time_sync.cpp
│   ├── test_pointcloud.cpp
│   ├── test_imu_processor.cpp
│   └── test_coloring.cpp
│
├── docs/                             # 文档
│   ├── MDC_Architecture_Design.md
│   ├── API_Reference.md
│   ├── Tuning_Guide.md
│   ├── Calibration_Guide.md
│   └── Troubleshooting.md
│
├── scripts/                          # 辅助脚本
│   ├── download_dependencies.sh
│   ├── calibrate_sensors.py
│   └── visualize_data.py
│
├── data/                             # 示例数据
│   ├── sample_imu.bin
│   ├── sample_lidar.bin
│   ├── sample_camera.jpg
│   └── calibration_results.yaml
│
└── .gitignore
```

## 开发路线图

### Phase 1: 基础框架 ⏳
- [x] 项目结构初始化
- [x] 架构设计文档完成
- [ ] CMake构建系统
- [ ] 数据结构定义
- [ ] 依赖库集成

### Phase 2: 传感器驱动 📋
- [ ] LiDAR驱动实现
- [ ] IMU驱动实现
- [ ] 相机驱动实现
- [ ] 驱动测试

### Phase 3: 数据处理核心 📋
- [ ] 时间同步算法
- [ ] IMU位姿估计
- [ ] 点云拼接实现
- [ ] 点云分类算法

### Phase 4: 多传感器融合 📋
- [ ] 点云赋色实现
- [ ] 融合质量评估
- [ ] 边界情况处理

### Phase 5: 实时显示与集成 📋
- [ ] OpenGL显示模块
- [ ] MDC框架集成
- [ ] 性能优化

### Phase 6: 测试与部署 📋
- [ ] 单元测试
- [ ] 集成测试
- [ ] 性能测试
- [ ] 硬件部署

## 关键技术点

### 1. 多传感器时间同步
- **挑战**：相机、IMU、LiDAR的时钟不同步
- **方案**：时间戳修正 + 硬件同步信号
- **精度**：±10-50ms

### 2. IMU位姿估计
- **算法**：IMU预积分 + EKF/UKF融合
- **输出**：位置和方向（四元数）
- **用途**：点云坐标变换

### 3. 单线LiDAR点云生成
- **方式**：单线扫描 + IMU位姿 → 3D点云
- **关键**：准确的IMU估计和时间对齐

### 4. 点云赋色
- **方法**：全景相机图像投影到3D点云
- **技术**：相机内参投影 + 双线性插值
- **标定**：相机-LiDAR外参标定

### 5. 实时显示
- **技术**：OpenGL点云渲染
- **目标帧率**：30-60 FPS
- **优化**：GPU加速、点云降采样

## 使用示例

```cpp
// 初始化系统
PointCloudFusionSystem system;
system.initialize("config/sensor_config.yaml");

// 启动采集
system.start_sensor_drivers();

// 处理循环
while (system.is_running()) {
    auto data = system.get_synchronized_data();
    
    if (data.is_valid()) {
        auto cloud = system.process_pointcloud(data);
        cloud = system.color_pointcloud(cloud, data.camera_frame);
        system.visualize(cloud);
    }
}

system.shutdown();
```

## 性能指标

| 指标 | 目标值 |
|---|---|
| 点云处理帧率 | 30 FPS |
| 时间同步精度 | ±20ms |
| 点云密度 | 1000+ 点/帧 |
| 端到端延迟 | < 100ms |
| 内存占用 | < 500MB |
| CPU使用率 | < 70% |

## 参考资源

- [PCL官方文档](http://pointclouds.org/)
- [OpenCV官方文档](https://docs.opencv.org/)
- [Eigen文档](https://eigen.tuxfamily.org/)

## 常见问题 (FAQ)

### Q: 如何标定多个传感器？
A: 详见 `docs/Calibration_Guide.md`

### Q: 为什么点云质量不好？
A: 检查以下几点：
1. 时间同步是否准确
2. IMU标定参数是否正确
3. 相机-LiDAR外参标定

### Q: 如何提高处理帧率？
A: 参考 `docs/Tuning_Guide.md` 中的性能优化章节

## 贡献指南

欢迎提交Issue和Pull Request！

## 许可证

MIT License

## 作者

**黄俊浩** - 初始项目创建

---

**最后更新**: 2026年6月16日
**项目状态**: 🚀 开发中
