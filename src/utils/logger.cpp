#include "utils/logger.h"

#include <iostream>

namespace dianyun {

void Logger::info(const std::string& message) {
    std::cout << "[dianyun] " << message << std::endl;
}

}  // namespace dianyun
