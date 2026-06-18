#ifndef DIANYUN_SENSOR_DRIVER_CAMERA_DRIVER_H
#define DIANYUN_SENSOR_DRIVER_CAMERA_DRIVER_H

#include "sensor_base.h"
#include "../common/data_types.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <opencv2/opencv.hpp>

namespace dianyun {

/**
 * @brief Panoramic camera driver
 * 
 * This driver handles communication with a four-module panoramic camera.
 * It captures and provides RGB images in real-time.
 */
class CameraDriver : public SensorBase {
public:
    explicit CameraDriver(const std::string& name = "Camera");
    ~CameraDriver() override;
    
    // Sensor interface implementation
    bool init(const std::string& config_file) override;
    bool start() override;
    bool stop() override;
    bool shutdown() override;
    
    /**
     * @brief Get the latest camera frame
     * @param frame Output camera frame
     * @param timeout_ms Timeout in milliseconds
     * @return true if frame is available
     */
    bool get_frame(CameraFrame& frame, int timeout_ms = 100);
    
    /**
     * @brief Check if frame is available
     * @return true if there is frame in queue
     */
    bool has_data() const;
    
    /**
     * @brief Get number of pending frames in queue
     * @return Number of frames
     */
    size_t get_queue_size() const;
    
    /**
     * @brief Get latest frame without blocking
     * @param frame Output frame
     * @return true if frame available
     */
    bool get_latest_frame(CameraFrame& frame);
    
private:
    // Configuration parameters
    struct Config {
        std::string device;            // Camera device (e.g., "/dev/video0")
        int width;                     // Image width in pixels
        int height;                    // Image height in pixels
        int fps;                       // Frames per second
        std::string camera_id;         // Camera identifier
    } config_;
    
    // OpenCV capture
    cv::VideoCapture cap_;
    
    // Data queue
    mutable std::mutex queue_mutex_;
    std::queue<CameraFrame> frame_queue_;
    CameraFrame latest_frame_;         // Latest frame for quick access
    std::condition_variable queue_cv_;
    static constexpr size_t MAX_QUEUE_SIZE = 10;
    
    // Capture thread
    std::thread capture_thread_;
    std::atomic<bool> should_exit_{false};
    
    // Methods
    bool open_camera();
    void close_camera();
    void capture_thread_function();
};

} // namespace dianyun

#endif // DIANYUN_SENSOR_DRIVER_CAMERA_DRIVER_H
