# MDC平台实时点云融合与显示系统

## 项目目标
将多传感器数据（四模组全景相机、IMU、单线扫描LiDAR）在MDC平台上实现：
1. **实时数据采集**：相机、IMU、LiDAR数据获取
2. **数据处理**：时间同步、点云拼接、点云分类、点云赋色
3. **实时显示**：处理后的3D点云在显示屏实时输出

---

## 系统架构

### 整体数据流

```
┌─────────────────────────────────────────────────────────────────┐
│                    硬件传感器层                                    │
├─────────────┬──────────────┬─────────────────────────────────────┤
│  四模组相机  │     IMU      │   单线扫描LiDAR                     │
└──────┬──────┴──────┬───────┴──────────────┬──────────────────────┘
       │            │                      │
       ▼            ▼                      ▼
┌─────────────────────────────────────────────────────────────────┐
│                   数据采集模块 (Sensor Driver)                   │
│  - 相机驱动    - IMU驱动      - LiDAR驱动                       │
│  - 原始数据读取  - 频率管理     - 串口/网络通信                   │
└────────────────────┬────────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────────────┐
│                   时间同步模块 (Time Sync)                       │
│  - 多传感器时间戳对齐                                             │
│  - 系统时钟与传感器时钟同步                                       │
└────────────────────┬────────────────────────────────────────────┘
                     │
        ┌────────────┼────────────┐
        │            │            │
        ▼            ▼            ▼
   ┌────────┐ ┌─────────┐ ┌──────────┐
   │相机处理 │ │IMU处理  │ │LiDAR处理 │
   │模块    │ │模块    │ │模块     │
   │- 去畸变 │ │- 滤波  │ │- 原始点  │
   │- 缩放  │ │- 姿态  │ │  云生成  │
   │- ROI  │ │  估计  │ │         │
   └────┬───┘ └──┬──────┘ └────┬─────┘
        │        │             │
        └────────┼─────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│              点云融合处理模块 (Fusion & Processing)              │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ 子模块1: 点云拼接 (Point Cloud Stitching)                │  │
│  │  - IMU位姿估计用于坐标变换                               │  │
│  │  - 单线扫描转3D点云                                      │  │
│  │  - 多帧点云累积与对齐                                    │  │
│  └──────────────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ 子模块2: 点云分类 (Point Cloud Classification)           │  │
│  │  - 地面/障碍物分割                                        │  │
│  │  - 基于几何/强度的分类                                    │  │
│  └──────────────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ 子模块3: 点云赋色 (Point Cloud Coloring)                 │  │
│  │  - 全景相机图像投影到3D点云                               │  │
│  │  - 双线性插值颜色赋值                                     │  │
│  │  - 处理遮挡和无效点                                       │  │
│  └──────────────────────────────────────────────────────────┘  │
└────────────────────┬────────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────────────┐
│              显示输出模块 (Visualization & Output)               │
│  - OpenGL/Vulkan 3D点云渲染                                     │
│  - 实时帧率控制 (30FPS/60FPS)                                   │
│  - 点云数据流传输到显示屏                                        │
│  - 质量检测与监控                                               │
└─────────────────────────────────────────────────────────────────┘
```

---

## 模块详细设计

### 1. 数据采集模块 (Sensor Driver)

**主要职责：**
- 与硬件通信，读取原始数据
- 维护数据缓冲队列
- 时间戳管理

**技术要点：**
```cpp
// 伪代码示例
struct SensorData {
    uint64_t timestamp;      // 纳秒级时间戳
    uint32_t sensor_id;      // 传感器ID
    std::vector<uint8_t> raw_data;
};

class LiDARDriver {
    void read_lidar_data();
    std::queue<SensorData> lidar_buffer;
};

class IMUDriver {
    void read_imu_data();
    IMUData get_latest_imu();
};

class CameraDriver {
    void read_camera_frame();
    cv::Mat get_frame();
};
```

---

### 2. 时间同步模块 (Time Synchronization)

**问题：** 多个传感器时钟不同步
**解决方案：**
- 建立主时钟参考
- 时间戳修正算法
- 数据缓冲与对齐

**关键公式：**
```
同步后时间戳 = 原始时间戳 + 时间偏移量 + 时钟漂移修正
```

---

### 3. IMU位姿估计模块

**输入：** IMU原始数据（加速度、角速度）
**输出：** 位置和姿态信息

**主要算法：**
- 惯性测量单元积分
- 死亡漂移补偿
- 可选：与LiDAR/相机融合的EKF/UKF

```cpp
struct Pose {
    Eigen::Vector3f position;      // x, y, z
    Eigen::Quaternionf rotation;   // 四元数表示姿态
    uint64_t timestamp;
};

class IMUProcessor {
    Pose estimate_pose(const IMUData& imu);
    Eigen::Matrix3f get_rotation_matrix();
};
```

---

### 4. 点云拼接模块 (Point Cloud Stitching)

**单线扫描转3D点云：**
```cpp
struct PointCloud {
    std::vector<Eigen::Vector3f> points;     // 3D坐标
    std::vector<uint8_t> intensity;          // 反射强度
    std::vector<uint32_t> classification;    // 分类标签
    std::vector<cv::Vec3b> color;            // RGB颜色
    uint64_t timestamp;
};

class PointCloudStitcher {
    // 输入：单线扫描点 + IMU位姿 + 时间戳
    void add_lidar_scan(const LiDARScan& scan, const Pose& pose);
    
    // 输出：3D点云
    PointCloud get_stitched_cloud();
    
    // 内部：点云累积和变换
    std::deque<PointCloud> frame_buffer;
};
```

**关键步骤：**
1. 单线LiDAR扫描 → 单线2D点集
2. 结合IMU位姿信息 → 3D点坐标
3. 累积多帧 → 完整3D点云

---

### 5. 点云分类模块 (Classification)

**分类目标：** 地面/障碍物/动态物体等

**方法：**
- 基于高度的简单分割
- 基于强度/法向量的更复杂分割
- 可选：神经网络（如PointNet）

```cpp
enum PointClass {
    GROUND = 0,
    OBSTACLE = 1,
    DYNAMIC = 2,
    UNKNOWN = 255
};

class PointCloudClassifier {
    void classify(PointCloud& cloud);
    void segment_ground(PointCloud& cloud);
    void classify_obstacle(PointCloud& cloud);
};
```

---

### 6. 点云赋色模块 (Point Cloud Coloring)

**核心思路：** 将全景相机图像投影到3D点云

```cpp
class PointCloudColorer {
    // 相机内参和外参
    Eigen::Matrix3f camera_intrinsic;
    Eigen::Matrix4f camera_extrinsic;
    
    // 处理步骤：
    // 1. 点云点投影到相机图像平面
    // 2. 双线性插值获取颜色
    // 3. 处理投影外的点
    void color_point_cloud(
        PointCloud& cloud,
        const cv::Mat& camera_image
    );
    
    cv::Vec3b get_interpolated_color(
        const cv::Mat& image,
        float u, float v  // 图像坐标
    );
};
```

**投影公式：**
```
[u]   [fx  0 cx] [x/z]
[v] = [0  fy cy] [y/z]
[1]   [0  0  1 ] [1  ]
```

---

### 7. 实时显示模块 (Visualization)

**显示需求：**
- 实时3D点云渲染
- 60 FPS或更高
- 颜色与分类可视化

**技术选择：**
- **OpenGL** (跨平台，MDC兼容)
- **VTK** (开源，支持点云可视化)
- **Open3D** (简单易用)

```cpp
class PointCloudVisualizer {
    void init_opengl_context();
    void update_point_cloud(const PointCloud& cloud);
    void render_frame();
    
    // 可视化选项
    bool show_intensity = true;
    bool show_classification = true;
    bool show_raw_points = false;
};
```

---

## 实现路线图

### Phase 1: 基础框架 (第1-2周)
- [ ] 项目结构设置 (CMake, 依赖管理)
- [ ] 创建各模块的接口和数据结构
- [ ] 搭建MDC应用框架
- [ ] 集成依赖库 (OpenCV, Eigen, PCL等)

### Phase 2: 传感器驱动 (第2-3周)
- [ ] LiDAR驱动实现
- [ ] IMU驱动实现
- [ ] 相机驱动实现 (使用OpenCV或原生驱动)
- [ ] 测试数据采集

### Phase 3: 数据处理核心 (第3-5周)
- [ ] 时间同步算法
- [ ] IMU位姿估计
- [ ] 点云拼接与生成
- [ ] 点云分类算法

### Phase 4: 多传感器融合 (第5-6周)
- [ ] 点云赋色实现
- [ ] 质量评估与优化
- [ ] 处理异常和边界情况

### Phase 5: 实时显示与MDC集成 (第6-7周)
- [ ] 显示模块开发
- [ ] MDC框架集成
- [ ] 性能优化与实时性保障
- [ ] 测试与调优

### Phase 6: 优化与部署 (第7-8周)
- [ ] 内存与CPU优化
- [ ] 算法参数调优
- [ ] 完整测试与验证
- [ ] 部署到MDC硬件

---

## 核心库与依赖

| 库 | 用途 | 版本 |
|---|---|---|
| OpenCV | 图像处理、相机标定 | 4.5+ |
| Eigen | 线性代数、矩阵运算 | 3.3+ |
| PCL | 点云处理 | 1.11+ |
| Boost | 通用C++库 | 1.70+ |
| OpenGL/GLFW | 3D渲染显示 | 4.5+/3.3+ |
| Protobuf | 数据序列化 | 3.0+ |

---

## MDC特定考虑

### 1. 时间精度
- MDC系统时钟：通常精度到毫秒
- 需要的同步精度：10-50毫秒
- 解决方案：硬件时间戳 + 软件校准

### 2. 实时性
- 目标帧率：30 FPS (单线LiDAR)
- 处理延迟预算：< 33ms
- 线程模型：MDC的消息队列 + 优先级调度

### 3. 资源限制
- 内存限制：需要评估MDC硬件规格
- CPU限制：合理分配线程数
- 性能优化：使用SIMD指令集优化计算密集部分

### 4. MDC通信接口
- 数据输入：传感器驱动集成
- 数据输出：显示接口或网络传输
- 参考MDC SDK文档

---

## 测试与验证

### 单元测试
- 各模块功能测试
- 数据格式验证

### 集成测试
- 端到端数据流测试
- 时间同步验证
- 显示质量评估

### 性能测试
- 帧率统计
- 延迟测量
- 内存占用监控
- CPU使用率分析

---

## 文件结构规划

```
dianyun/
├── CMakeLists.txt                 # 构建配置
├── docs/                          # 文档
│   ├── MDC_Architecture_Design.md
│   ├── API_Reference.md
│   └── Tuning_Guide.md
├── src/
│   ├── main.cpp                   # MDC应用主程序
│   ├── sensor_driver/             # 传感器驱动模块
│   │   ├── lidar_driver.h/cpp
│   │   ├── imu_driver.h/cpp
│   │   └── camera_driver.h/cpp
│   ├── time_sync/                 # 时间同步模块
│   │   ├── time_sync.h/cpp
│   │   └── clock_manager.h/cpp
│   ├── processing/                # 数据处理模块
│   │   ├── imu_processor.h/cpp
│   │   ├── pointcloud_stitcher.h/cpp
│   │   ├── pointcloud_classifier.h/cpp
│   │   └── pointcloud_colorer.h/cpp
│   ├── visualization/             # 显示模块
│   │   ├── visualizer.h/cpp
│   │   └── shader.glsl
│   ├── config/                    # 配置文件
│   │   ├── sensor_config.yaml
│   │   └── process_config.yaml
│   └── utils/                     # 工具函数
│       ├── logger.h/cpp
│       └── math_utils.h/cpp
├── test/                          # 单元测试
│   ├── test_time_sync.cpp
│   ├── test_pointcloud.cpp
│   └── test_processing.cpp
├── data/                          # 示例数据
│   ├── sample_imu.bin
│   └── sample_lidar.bin
└── README.md
```

---

## 下一步

1. **确认MDC SDK版本和硬件规格**
2. **搭建开发环境** (CMake + 依赖库编译)
3. **开始Phase 1：基础框架搭建**
4. **根据硬件接口文档定制传感器驱动**

---

## 关键参考链接

- MDC官方文档（需向华为获取）
- PCL官方：http://pointclouds.org/
- OpenCV官方：https://opencv.org/
- Eigen官方：https://eigen.tuxfamily.org/

---

**作者备注：** 这份设计文档是基于标准的多传感器融合系统设计。实际实现需要根据：
- 您的MDC硬件版本
- 传感器的具体型号和接口协议
- 性能和功能的具体需求

来进行定制化调整。建议在开始编码前，先完成传感器接口协议的学习和测试。
