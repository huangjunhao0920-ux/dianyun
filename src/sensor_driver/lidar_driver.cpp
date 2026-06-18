#include "lidar_driver.h"
#include "../utils/logger.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>

namespace dianyun {

LiDARDriver::LiDARDriver(const std::string& name)
    : SensorBase(name, SENSOR_LIDAR), serial_fd_(-1) {
    // Default configuration
    config_.port = "/dev/ttyUSB0";
    config_.baudrate = 230400;
    config_.sample_rate = 100;
    config_.min_range = 0.15f;
    config_.max_range = 10.0f;
    config_.num_points_per_scan = 1024;
}

LiDARDriver::~LiDARDriver() {
    if (is_running_) {
        stop();
    }
    if (serial_fd_ >= 0) {
        close_serial_port();
    }
}

bool LiDARDriver::init(const std::string& config_file) {
    LOG_INFO("Initializing LiDAR driver...");
    
    // TODO: Parse config file and set config_
    // For now, using default values
    
    if (!open_serial_port()) {
        LOG_ERROR("Failed to open serial port: " << config_.port);
        status_ = STATUS_ERROR;
        return false;
    }
    
    status_ = STATUS_CONNECTED;
    LOG_INFO("LiDAR driver initialized successfully.");
    return true;
}

bool LiDARDriver::start() {
    if (is_running_) {
        LOG_WARN("LiDAR driver is already running.");
        return true;
    }
    
    if (status_ != STATUS_CONNECTED) {
        LOG_ERROR("LiDAR driver is not properly initialized.");
        return false;
    }
    
    LOG_INFO("Starting LiDAR driver...");
    is_running_ = true;
    should_exit_ = false;
    
    // Start reading thread
    read_thread_ = std::thread(&LiDARDriver::read_thread_function, this);
    
    LOG_INFO("LiDAR driver started successfully.");
    return true;
}

bool LiDARDriver::stop() {
    if (!is_running_) {
        LOG_WARN("LiDAR driver is not running.");
        return true;
    }
    
    LOG_INFO("Stopping LiDAR driver...");
    is_running_ = false;
    should_exit_ = true;
    
    // Signal the condition variable to wake up the reading thread
    queue_cv_.notify_all();
    
    // Wait for reading thread to finish
    if (read_thread_.joinable()) {
        read_thread_.join();
    }
    
    LOG_INFO("LiDAR driver stopped successfully.");
    return true;
}

bool LiDARDriver::shutdown() {
    if (is_running_) {
        stop();
    }
    
    if (serial_fd_ >= 0) {
        close_serial_port();
    }
    
    status_ = STATUS_IDLE;
    LOG_INFO("LiDAR driver shutdown complete.");
    return true;
}

bool LiDARDriver::get_scan(LiDARScan& scan, int timeout_ms) {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    
    // Wait for data with timeout
    auto timeout = std::chrono::milliseconds(timeout_ms);
    if (!queue_cv_.wait_for(lock, timeout, [this] { return !scan_queue_.empty(); })) {
        return false;
    }
    
    if (scan_queue_.empty()) {
        return false;
    }
    
    scan = scan_queue_.front();
    scan_queue_.pop();
    
    return true;
}

bool LiDARDriver::has_data() const {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    return !scan_queue_.empty();
}

size_t LiDARDriver::get_queue_size() const {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    return scan_queue_.size();
}

bool LiDARDriver::open_serial_port() {
    serial_fd_ = ::open(config_.port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (serial_fd_ < 0) {
        LOG_ERROR("Failed to open serial port: " << config_.port);
        return false;
    }
    
    // Configure serial port
    struct termios options;
    tcgetattr(serial_fd_, &options);
    
    // Set baud rate
    cfsetispeed(&options, B230400);
    cfsetospeed(&options, B230400);
    
    // Set data bits, stop bits, parity
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;        // 8 data bits
    options.c_cflag &= ~PARENB;    // No parity
    options.c_cflag &= ~CSTOPB;    // 1 stop bit
    options.c_cflag |= CREAD;      // Enable receiver
    
    // Set raw mode
    options.c_lflag &= ~(ICANON | ECHO);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;
    
    tcsetattr(serial_fd_, TCSANOW, &options);
    
    LOG_INFO("Serial port opened: " << config_.port);
    return true;
}

void LiDARDriver::close_serial_port() {
    if (serial_fd_ >= 0) {
        ::close(serial_fd_);
        serial_fd_ = -1;
        LOG_INFO("Serial port closed.");
    }
}

void LiDARDriver::read_thread_function() {
    LOG_INFO("LiDAR read thread started.");
    
    std::vector<uint8_t> buffer(4096);
    
    while (!should_exit_ && is_running_) {
        // Read raw data from serial port
        if (!read_from_serial(buffer, 100)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        // Parse LiDAR data
        LiDARScan scan;
        if (parse_lidar_data(buffer, scan)) {
            // Add to queue
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                
                // Keep queue size limited
                if (scan_queue_.size() >= MAX_QUEUE_SIZE) {
                    scan_queue_.pop();
                }
                
                scan_queue_.push(scan);
            }
            queue_cv_.notify_one();
        }
    }
    
    LOG_INFO("LiDAR read thread exited.");
}

bool LiDARDriver::parse_lidar_data(const std::vector<uint8_t>& data, LiDARScan& scan) {
    // TODO: Implement actual LiDAR protocol parsing
    // This is a placeholder implementation
    
    scan.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    scan.num_points = config_.num_points_per_scan;
    scan.distances.clear();
    scan.angles.clear();
    scan.intensity.clear();
    
    // Generate synthetic data for testing
    static float angle_offset = 0.0f;
    for (int i = 0; i < config_.num_points_per_scan; ++i) {
        float angle = (i / static_cast<float>(config_.num_points_per_scan)) * 2 * 3.14159f;
        float distance = 2.0f + 0.5f * std::sin(angle + angle_offset);
        
        scan.angles.push_back(angle);
        scan.distances.push_back(distance);
        scan.intensity.push_back(static_cast<uint8_t>(128 + 127 * std::sin(angle)));
    }
    angle_offset += 0.1f;
    
    return true;
}

bool LiDARDriver::write_to_serial(const std::vector<uint8_t>& data) {
    if (serial_fd_ < 0) {
        return false;
    }
    
    ssize_t bytes_written = ::write(serial_fd_, data.data(), data.size());
    return bytes_written == static_cast<ssize_t>(data.size());
}

bool LiDARDriver::read_from_serial(std::vector<uint8_t>& buffer, int timeout_ms) {
    if (serial_fd_ < 0) {
        return false;
    }
    
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(serial_fd_, &readfds);
    
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    
    int ret = select(serial_fd_ + 1, &readfds, nullptr, nullptr, &tv);
    if (ret <= 0) {
        return false;
    }
    
    ssize_t bytes_read = ::read(serial_fd_, buffer.data(), buffer.size());
    return bytes_read > 0;
}

} // namespace dianyun
