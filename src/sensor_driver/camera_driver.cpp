#include "camera_driver.h"
#include "../utils/logger.h"
#include <chrono>

namespace dianyun {

CameraDriver::CameraDriver(const std::string& name)
    : SensorBase(name, SENSOR_CAMERA) {
    // Default configuration
    config_.device = "/dev/video0";
    config_.width = 1280;
    config_.height = 960;
    config_.fps = 30;
    config_.camera_id = "panoramic_camera_0";
    
    // Initialize latest frame
    latest_frame_.timestamp = 0;
    latest_frame_.width = config_.width;
    latest_frame_.height = config_.height;
    latest_frame_.camera_id = config_.camera_id;
}

CameraDriver::~CameraDriver() {
    if (is_running_) {
        stop();
    }
    close_camera();
}

bool CameraDriver::init(const std::string& config_file) {
    LOG_INFO("Initializing camera driver...");
    
    // TODO: Parse config file and set config_
    // For now, using default values
    
    if (!open_camera()) {
        LOG_ERROR("Failed to open camera: " << config_.device);
        status_ = STATUS_ERROR;
        return false;
    }
    
    status_ = STATUS_CONNECTED;
    LOG_INFO("Camera driver initialized successfully.");
    return true;
}

bool CameraDriver::start() {
    if (is_running_) {
        LOG_WARN("Camera driver is already running.");
        return true;
    }
    
    if (status_ != STATUS_CONNECTED) {
        LOG_ERROR("Camera driver is not properly initialized.");
        return false;
    }
    
    LOG_INFO("Starting camera driver...");
    is_running_ = true;
    should_exit_ = false;
    
    // Start capture thread
    capture_thread_ = std::thread(&CameraDriver::capture_thread_function, this);
    
    LOG_INFO("Camera driver started successfully.");
    return true;
}

bool CameraDriver::stop() {
    if (!is_running_) {
        LOG_WARN("Camera driver is not running.");
        return true;
    }
    
    LOG_INFO("Stopping camera driver...");
    is_running_ = false;
    should_exit_ = true;
    
    // Signal the condition variable
    queue_cv_.notify_all();
    
    // Wait for capture thread to finish
    if (capture_thread_.joinable()) {
        capture_thread_.join();
    }
    
    LOG_INFO("Camera driver stopped successfully.");
    return true;
}

bool CameraDriver::shutdown() {
    if (is_running_) {
        stop();
    }
    
    close_camera();
    
    status_ = STATUS_IDLE;
    LOG_INFO("Camera driver shutdown complete.");
    return true;
}

bool CameraDriver::get_frame(CameraFrame& frame, int timeout_ms) {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    
    // Wait for frame with timeout
    auto timeout = std::chrono::milliseconds(timeout_ms);
    if (!queue_cv_.wait_for(lock, timeout, [this] { return !frame_queue_.empty(); })) {
        return false;
    }
    
    if (frame_queue_.empty()) {
        return false;
    }
    
    frame = frame_queue_.front();
    frame_queue_.pop();
    
    return true;
}

bool CameraDriver::has_data() const {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    return !frame_queue_.empty();
}

size_t CameraDriver::get_queue_size() const {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    return frame_queue_.size();
}

bool CameraDriver::get_latest_frame(CameraFrame& frame) {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    if (latest_frame_.timestamp == 0) {
        return false;
    }
    frame = latest_frame_;
    return true;
}

bool CameraDriver::open_camera() {
    // Try to open camera device
    // First try as device path
    int device_id = 0;
    try {
        device_id = std::stoi(config_.device.substr(config_.device.find_last_of('/') + 1));
    } catch (...) {
        device_id = 0;
    }
    
    cap_.open(device_id);
    if (!cap_.isOpened()) {
        LOG_ERROR("Failed to open camera device: " << config_.device);
        return false;
    }
    
    // Set camera properties
    cap_.set(cv::CAP_PROP_FRAME_WIDTH, config_.width);
    cap_.set(cv::CAP_PROP_FRAME_HEIGHT, config_.height);
    cap_.set(cv::CAP_PROP_FPS, config_.fps);
    cap_.set(cv::CAP_PROP_BUFFERSIZE, 1);  // Single frame buffer
    
    // Verify settings
    int actual_width = static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_WIDTH));
    int actual_height = static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_HEIGHT));
    double actual_fps = cap_.get(cv::CAP_PROP_FPS);
    
    LOG_INFO("Camera opened: " << actual_width << "x" << actual_height << " @ " << actual_fps << "fps");
    return true;
}

void CameraDriver::close_camera() {
    if (cap_.isOpened()) {
        cap_.release();
        LOG_INFO("Camera closed.");
    }
}

void CameraDriver::capture_thread_function() {
    LOG_INFO("Camera capture thread started.");
    
    cv::Mat frame;
    
    while (!should_exit_ && is_running_) {
        // Capture frame from camera
        if (!cap_.read(frame)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        // Convert to BGR if necessary (OpenCV default is BGR)
        if (frame.empty()) {
            continue;
        }
        
        // Create camera frame
        CameraFrame camera_frame;
        camera_frame.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        camera_frame.image = frame.clone();  // Deep copy
        camera_frame.width = frame.cols;
        camera_frame.height = frame.rows;
        camera_frame.camera_id = config_.camera_id;
        
        // Update latest frame and queue
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            latest_frame_ = camera_frame;  // Always update latest
            
            // Add to queue with size limit
            if (frame_queue_.size() >= MAX_QUEUE_SIZE) {
                frame_queue_.pop();
            }
            frame_queue_.push(camera_frame);
        }
        queue_cv_.notify_one();
    }
    
    LOG_INFO("Camera capture thread exited.");
}

} // namespace dianyun
