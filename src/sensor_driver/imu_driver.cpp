#include "imu_driver.h"
#include "../utils/logger.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <cmath>

namespace dianyun {

IMUDriver::IMUDriver(const std::string& name)
    : SensorBase(name, SENSOR_IMU), serial_fd_(-1) {
    // Default configuration
    config_.port = "/dev/ttyUSB1";
    config_.baudrate = 115200;
    config_.sample_rate = 100;
    config_.accel_range = 16.0f;  // ±16g
    config_.gyro_range = 2000.0f; // ±2000 deg/s
    
    // Initialize calibration (default: no bias, scale = 1)
    calibration_.accel_bias = Eigen::Vector3f::Zero();
    calibration_.gyro_bias = Eigen::Vector3f::Zero();
    calibration_.accel_scale = Eigen::Vector3f::Ones();
    calibration_.gyro_scale = Eigen::Vector3f::Ones();
    
    // Initialize latest data
    latest_data_.timestamp = 0;
    latest_data_.accel = Eigen::Vector3f::Zero();
    latest_data_.gyro = Eigen::Vector3f::Zero();
    latest_data_.mag = Eigen::Vector3f::Zero();
    latest_data_.temperature = 25.0f;
}

IMUDriver::~IMUDriver() {
    if (is_running_) {
        stop();
    }
    if (serial_fd_ >= 0) {
        close_serial_port();
    }
}

bool IMUDriver::init(const std::string& config_file) {
    LOG_INFO("Initializing IMU driver...");
    
    // TODO: Parse config file and set config_ and calibration_
    // For now, using default values
    
    if (!open_serial_port()) {
        LOG_ERROR("Failed to open serial port: " << config_.port);
        status_ = STATUS_ERROR;
        return false;
    }
    
    status_ = STATUS_CONNECTED;
    LOG_INFO("IMU driver initialized successfully.");
    return true;
}

bool IMUDriver::start() {
    if (is_running_) {
        LOG_WARN("IMU driver is already running.");
        return true;
    }
    
    if (status_ != STATUS_CONNECTED) {
        LOG_ERROR("IMU driver is not properly initialized.");
        return false;
    }
    
    LOG_INFO("Starting IMU driver...");
    is_running_ = true;
    should_exit_ = false;
    
    // Start reading thread
    read_thread_ = std::thread(&IMUDriver::read_thread_function, this);
    
    LOG_INFO("IMU driver started successfully.");
    return true;
}

bool IMUDriver::stop() {
    if (!is_running_) {
        LOG_WARN("IMU driver is not running.");
        return true;
    }
    
    LOG_INFO("Stopping IMU driver...");
    is_running_ = false;
    should_exit_ = true;
    
    // Signal the condition variable
    queue_cv_.notify_all();
    
    // Wait for reading thread to finish
    if (read_thread_.joinable()) {
        read_thread_.join();
    }
    
    LOG_INFO("IMU driver stopped successfully.");
    return true;
}

bool IMUDriver::shutdown() {
    if (is_running_) {
        stop();
    }
    
    if (serial_fd_ >= 0) {
        close_serial_port();
    }
    
    status_ = STATUS_IDLE;
    LOG_INFO("IMU driver shutdown complete.");
    return true;
}

bool IMUDriver::get_imu_data(IMUData& data, int timeout_ms) {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    
    // Wait for data with timeout
    auto timeout = std::chrono::milliseconds(timeout_ms);
    if (!queue_cv_.wait_for(lock, timeout, [this] { return !data_queue_.empty(); })) {
        return false;
    }
    
    if (data_queue_.empty()) {
        return false;
    }
    
    data = data_queue_.front();
    data_queue_.pop();
    
    return true;
}

bool IMUDriver::has_data() const {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    return !data_queue_.empty();
}

size_t IMUDriver::get_queue_size() const {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    return data_queue_.size();
}

bool IMUDriver::get_latest_imu_data(IMUData& data) {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    if (latest_data_.timestamp == 0) {
        return false;
    }
    data = latest_data_;
    return true;
}

bool IMUDriver::open_serial_port() {
    serial_fd_ = ::open(config_.port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (serial_fd_ < 0) {
        LOG_ERROR("Failed to open serial port: " << config_.port);
        return false;
    }
    
    // Configure serial port
    struct termios options;
    tcgetattr(serial_fd_, &options);
    
    // Set baud rate to 115200
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    
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

void IMUDriver::close_serial_port() {
    if (serial_fd_ >= 0) {
        ::close(serial_fd_);
        serial_fd_ = -1;
        LOG_INFO("Serial port closed.");
    }
}

void IMUDriver::read_thread_function() {
    LOG_INFO("IMU read thread started.");
    
    std::vector<uint8_t> buffer(1024);
    
    while (!should_exit_ && is_running_) {
        // Read raw data from serial port
        if (!read_from_serial(buffer, 100)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        // Parse IMU data
        IMUData imu_data;
        if (parse_imu_data(buffer, imu_data)) {
            // Apply calibration
            apply_calibration(imu_data);
            
            // Update latest data and queue
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                latest_data_ = imu_data;  // Always update latest
                
                // Add to queue with size limit
                if (data_queue_.size() >= MAX_QUEUE_SIZE) {
                    data_queue_.pop();
                }
                data_queue_.push(imu_data);
            }
            queue_cv_.notify_one();
        }
    }
    
    LOG_INFO("IMU read thread exited.");
}

bool IMUDriver::parse_imu_data(const std::vector<uint8_t>& data, IMUData& imu_data) {
    // TODO: Implement actual IMU protocol parsing (e.g., AHRS, XSENS, etc.)
    // This is a placeholder implementation
    
    imu_data.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    
    // Generate synthetic IMU data for testing
    static float time_offset = 0.0f;
    float t = time_offset;
    
    // Simulate motion: gentle rotation and acceleration
    imu_data.accel[0] = 0.5f * std::sin(t);
    imu_data.accel[1] = 0.3f * std::cos(t);
    imu_data.accel[2] = 9.81f + 0.2f * std::sin(2*t);  // Add gravity
    
    imu_data.gyro[0] = 0.1f * std::sin(t);
    imu_data.gyro[1] = 0.05f * std::cos(t);
    imu_data.gyro[2] = 0.15f * std::sin(t);
    
    imu_data.mag[0] = 20.0f + 2.0f * std::sin(t);
    imu_data.mag[1] = 30.0f + 2.0f * std::cos(t);
    imu_data.mag[2] = 40.0f;
    
    imu_data.temperature = 25.0f + 2.0f * std::sin(t);
    
    time_offset += 0.01f;
    
    return true;
}

void IMUDriver::apply_calibration(IMUData& data) {
    // Apply bias and scale corrections
    data.accel = (data.accel - calibration_.accel_bias).cwiseProduct(calibration_.accel_scale);
    data.gyro = (data.gyro - calibration_.gyro_bias).cwiseProduct(calibration_.gyro_scale);
}

bool IMUDriver::write_to_serial(const std::vector<uint8_t>& data) {
    if (serial_fd_ < 0) {
        return false;
    }
    
    ssize_t bytes_written = ::write(serial_fd_, data.data(), data.size());
    return bytes_written == static_cast<ssize_t>(data.size());
}

bool IMUDriver::read_from_serial(std::vector<uint8_t>& buffer, int timeout_ms) {
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
