#ifndef DIANYUN_SENSOR_DRIVER_IMU_DRIVER_H
#define DIANYUN_SENSOR_DRIVER_IMU_DRIVER_H

#include "sensor_base.h"
#include "../common/data_types.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace dianyun {

/**
 * @brief IMU (Inertial Measurement Unit) driver
 * 
 * This driver handles communication with an IMU sensor.
 * It reads acceleration, angular velocity, and magnetic field data.
 */
class IMUDriver : public SensorBase {
public:
    explicit IMUDriver(const std::string& name = "IMU");
    ~IMUDriver() override;
    
    // Sensor interface implementation
    bool init(const std::string& config_file) override;
    bool start() override;
    bool stop() override;
    bool shutdown() override;
    
    /**
     * @brief Get the latest IMU data
     * @param data Output IMU data
     * @param timeout_ms Timeout in milliseconds
     * @return true if IMU data is available
     */
    bool get_imu_data(IMUData& data, int timeout_ms = 100);
    
    /**
     * @brief Check if IMU data is available
     * @return true if there is data in queue
     */
    bool has_data() const;
    
    /**
     * @brief Get number of pending IMU readings in queue
     * @return Number of readings
     */
    size_t get_queue_size() const;
    
    /**
     * @brief Get latest IMU data without blocking
     * @param data Output IMU data
     * @return true if data available
     */
    bool get_latest_imu_data(IMUData& data);
    
private:
    // Configuration parameters
    struct Config {
        std::string port;              // Serial port
        int baudrate;                  // Baud rate
        int sample_rate;               // Sample rate in Hz
        float accel_range;             // Accelerometer range in g
        float gyro_range;              // Gyroscope range in deg/s
    } config_;
    
    // Calibration parameters
    struct Calibration {
        Eigen::Vector3f accel_bias;    // Accelerometer bias
        Eigen::Vector3f gyro_bias;     // Gyroscope bias
        Eigen::Vector3f accel_scale;   // Accelerometer scale
        Eigen::Vector3f gyro_scale;    // Gyroscope scale
    } calibration_;
    
    // Serial communication
    int serial_fd_;
    
    // Data queue
    mutable std::mutex queue_mutex_;
    std::queue<IMUData> data_queue_;
    IMUData latest_data_;              // Latest data for quick access
    std::condition_variable queue_cv_;
    static constexpr size_t MAX_QUEUE_SIZE = 100;
    
    // Reading thread
    std::thread read_thread_;
    std::atomic<bool> should_exit_{false};
    
    // Methods
    bool open_serial_port();
    void close_serial_port();
    void read_thread_function();
    bool parse_imu_data(const std::vector<uint8_t>& data, IMUData& imu_data);
    void apply_calibration(IMUData& data);
    
    // Serial communication helpers
    bool write_to_serial(const std::vector<uint8_t>& data);
    bool read_from_serial(std::vector<uint8_t>& buffer, int timeout_ms);
};

} // namespace dianyun

#endif // DIANYUN_SENSOR_DRIVER_IMU_DRIVER_H
