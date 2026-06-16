#include <cassert>

#include "processing/pointcloud_classifier.h"
#include "processing/pointcloud_colorer.h"
#include "processing/pointcloud_stitcher.h"

int main() {
    dianyun::PointCloudStitcher stitcher;
    dianyun::LidarScan scan;
    scan.timestamp_ns = 10;
    scan.ranges_m = {1.0f, 2.0f};

    dianyun::Pose pose;
    pose.z = 0.05f;
    stitcher.append_scan(scan, pose);

    auto cloud = stitcher.stitched_cloud();
    assert(cloud.points.size() == 2);

    dianyun::PointCloudClassifier classifier;
    classifier.classify(cloud, 0.1f);
    assert(cloud.points[0].cls == dianyun::PointClass::Ground);

    dianyun::CameraFrame frame;
    frame.rgb = {10, 20, 30};
    dianyun::PointCloudColorer colorer;
    colorer.colorize(cloud, frame);
    assert(cloud.points[0].color[0] == 10);
    return 0;
}
