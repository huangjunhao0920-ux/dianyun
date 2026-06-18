#ifndef DIANYUN_SENSOR_DRIVER_SENSOR_BASE_H
#define DIANYUN_SENSOR_DRIVER_SENSOR_BASE_H

#include <string>
#include <cstdint>
#include <memory>

namespace dianyun {

/**
 * @brief Base class for all sensor drivers
 * 
 * This abstract class defines the interface that all sensor drivers must implement.
 * It provides common functionality like initialization, data reading, and shutdown.
 */
class SensorBase {
public:
    enum SensorType {
        SENSOR_LIDAR,
        SENSOR_IMU,
        SENSOR_CAMERA
    };
    
    enum SensorStatus {
        STATUS_IDLE,        // Not initialized
        STATUS_CONNECTING,  // In connection process
        STATUS_CONNECTED,   // Successfully connected
        STATUS_ERROR        // Connection or reading error
    };
    
    explicit SensorBase(const std::string& sensor_name, SensorType type)
        : sensor_name_(sensor_name), sensor_type_(type), 
          status_(STATUS_IDLE), is_running_(false) {}
    
    virtual ~SensorBase() = default;
    
    // Delete copy operations
    SensorBase(const SensorBase&) = delete;
    SensorBase& operator=(const SensorBase&) = delete;
    
    /**
     * @brief Initialize the sensor
     * @param config_file Configuration file path
     * @return true if initialization successful
     */
    virtual bool init(const std::string& config_file) = 0;
    
    /**
     * @brief Start reading sensor data
     * @return true if start successful
     */
    virtual bool start() = 0;
    
    /**
     * @brief Stop reading sensor data
     * @return true if stop successful
     */
    virtual bool stop() = 0;
    
    /**
     * @brief Shutdown the sensor
     * @return true if shutdown successful
     */
    virtual bool shutdown() = 0;
    
    /**
     * @brief Check if sensor is running
     * @return true if sensor is active
     */
    bool is_running() const { return is_running_; }
    
    /**
     * @brief Get current sensor status
     * @return Current status
     */
    SensorStatus get_status() const { return status_; }
    
    /**
     * @brief Get sensor name
     * @return Sensor name
     */
    const std::string& get_name() const { return sensor_name_; }
    
    /**
     * @brief Get sensor type
     * @return Sensor type
     */
    SensorType get_type() const { return sensor_type_; }
    
protected:
    std::string sensor_name_;
    SensorType sensor_type_;
    SensorStatus status_;
    bool is_running_;
};

using SensorBasePtr = std::shared_ptr<SensorBase>;

} // namespace dianyun

#endif // DIANYUN_SENSOR_DRIVER_SENSOR_BASE_H
