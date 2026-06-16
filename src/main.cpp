#include "processing/imu_processor.h"
#include "processing/pointcloud_classifier.h"
#include "processing/pointcloud_colorer.h"
#include "processing/pointcloud_stitcher.h"
#include "utils/logger.h"
#include "visualization/visualizer.h"

int main() {
    dianyun::ImuProcessor imu_processor;
    dianyun::PointCloudStitcher stitcher;
    dianyun::PointCloudClassifier classifier;
    dianyun::PointCloudColorer colorer;
    dianyun::Visualizer visualizer;

    dianyun::ImuSample imu{};
    imu.timestamp_ns = 1000000;
    imu.gz = 0.1f;
    const auto pose = imu_processor.integrate(imu, 0.01);

    dianyun::LidarScan scan{};
    scan.timestamp_ns = 2000000;
    scan.ranges_m = {1.0f, 1.2f, 1.1f};
    stitcher.append_scan(scan, pose);

    auto cloud = stitcher.stitched_cloud();
    classifier.classify(cloud);

    dianyun::CameraFrame frame{};
    frame.rgb = {40, 120, 220};
    colorer.colorize(cloud, frame);

    visualizer.update(cloud);
    dianyun::Logger::info("pipeline initialized with " + std::to_string(cloud.points.size()) + " points");
    return 0;
}
