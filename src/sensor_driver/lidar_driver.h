#ifndef DIANYUN_SENSOR_DRIVER_LIDAR_DRIVER_H
#define DIANYUN_SENSOR_DRIVER_LIDAR_DRIVER_H

#include "sensor_base.h"
#include "../common/data_types.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace dianyun {

/**
 * @brief Single-line scanning LiDAR driver
 * 
 * This driver handles communication with a single-line scanning LiDAR sensor.
 * It reads raw distance and intensity data and provides it in a queue.
 */
class LiDARDriver : public SensorBase {
public:
    explicit LiDARDriver(const std::string& name = "LiDAR");
    ~LiDARDriver() override;
    
    // Sensor interface implementation
    bool init(const std::string& config_file) override;
    bool start() override;
    bool stop() override;
    bool shutdown() override;
    
    /**
     * @brief Get the latest LiDAR scan
     * @param scan Output LiDAR scan data
     * @param timeout_ms Timeout in milliseconds
     * @return true if scan data is available
     */
    bool get_scan(LiDARScan& scan, int timeout_ms = 100);
    
    /**
     * @brief Check if scan data is available
     * @return true if there is data in queue
     */
    bool has_data() const;
    
    /**
     * @brief Get number of pending scans in queue
     * @return Number of scans
     */
    size_t get_queue_size() const;
    
private:
    // Configuration parameters
    struct Config {
        std::string port;              // Serial port (e.g., "/dev/ttyUSB0")
        int baudrate;                  // Baud rate (typically 230400)
        int sample_rate;               // Sample rate in Hz
        float min_range;               // Minimum detection range in meters
        float max_range;               // Maximum detection range in meters
        int num_points_per_scan;       // Points per scan
    } config_;
    
    // Serial communication
    int serial_fd_;                    // Serial port file descriptor
    
    // Data queue
    mutable std::mutex queue_mutex_;
    std::queue<LiDARScan> scan_queue_;
    std::condition_variable queue_cv_;
    static constexpr size_t MAX_QUEUE_SIZE = 20;
    
    // Reading thread
    std::thread read_thread_;
    std::atomic<bool> should_exit_{false};
    
    // Methods
    bool open_serial_port();
    void close_serial_port();
    void read_thread_function();
    bool parse_lidar_data(const std::vector<uint8_t>& data, LiDARScan& scan);
    
    // Serial communication helpers
    bool write_to_serial(const std::vector<uint8_t>& data);
    bool read_from_serial(std::vector<uint8_t>& buffer, int timeout_ms);
};

} // namespace dianyun

#endif // DIANYUN_SENSOR_DRIVER_LIDAR_DRIVER_H
