# API Reference

## Sensor Driver Layer
- `SensorBase`
  - `bool start()`
  - `void stop()`
  - `const char* name() const`
- `LidarDriver`
  - `void push_scan(const LidarScan&)`
  - `bool pop_latest_scan(LidarScan&)`
- `ImuDriver`
  - `void push_sample(const ImuSample&)`
  - `bool pop_latest_sample(ImuSample&)`
- `CameraDriver`
  - `void push_frame(const CameraFrame&)`
  - `bool pop_latest_frame(CameraFrame&)`

## Time Sync Layer
- `TimeSynchronizer`
  - `void set_offset_ns(const std::string& sensor_id, int64_t offset_ns)`
  - `uint64_t align_timestamp_ns(const std::string& sensor_id, uint64_t raw_ts_ns) const`
- `ClockManager`
  - `static uint64_t now_ns()`

## Processing Layer
- `ImuProcessor`
  - `Pose integrate(const ImuSample&, double dt_sec)`
  - `const Pose& current_pose() const`
- `PointCloudStitcher`
  - `void append_scan(const LidarScan&, const Pose&)`
  - `const PointCloud& stitched_cloud() const`
  - `void clear()`
- `PointCloudClassifier`
  - `void classify(PointCloud&, float ground_threshold)`
- `PointCloudColorer`
  - `void colorize(PointCloud&, const CameraFrame&)`

## Visualization Layer
- `Visualizer`
  - `void update(const PointCloud&)`
  - `double last_fps() const`
