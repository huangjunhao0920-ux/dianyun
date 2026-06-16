#ifndef DIANYUN_UTILS_LOGGER_H
#define DIANYUN_UTILS_LOGGER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <memory>

namespace dianyun {

class Logger {
public:
    enum LogLevel {
        DEBUG = 0,
        INFO = 1,
        WARN = 2,
        ERROR = 3
    };
    
    static Logger& instance() {
        static Logger logger;
        return logger;
    }
    
    void set_log_level(LogLevel level) {
        log_level_ = level;
    }
    
    void log(LogLevel level, const std::string& message) {
        if (level >= log_level_) {
            std::string level_str;
            switch (level) {
                case DEBUG: level_str = "[DEBUG]"; break;
                case INFO:  level_str = "[INFO] "; break;
                case WARN:  level_str = "[WARN] "; break;
                case ERROR: level_str = "[ERROR]"; break;
            }
            
            std::string timestamp = get_timestamp();
            std::string output = timestamp + " " + level_str + " " + message;
            
            std::cout << output << std::endl;
            
            if (log_file_.is_open()) {
                log_file_ << output << std::endl;
                log_file_.flush();
            }
        }
    }
    
private:
    Logger() : log_level_(INFO) {}
    
    std::string get_timestamp() {
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    LogLevel log_level_;
    std::ofstream log_file_;
};

} // namespace dianyun

#define LOG_DEBUG(msg) { std::ostringstream oss; oss << msg; dianyun::Logger::instance().log(dianyun::Logger::DEBUG, oss.str()); }
#define LOG_INFO(msg)  { std::ostringstream oss; oss << msg; dianyun::Logger::instance().log(dianyun::Logger::INFO, oss.str()); }
#define LOG_WARN(msg)  { std::ostringstream oss; oss << msg; dianyun::Logger::instance().log(dianyun::Logger::WARN, oss.str()); }
#define LOG_ERROR(msg) { std::ostringstream oss; oss << msg; dianyun::Logger::instance().log(dianyun::Logger::ERROR, oss.str()); }

#endif // DIANYUN_UTILS_LOGGER_H
