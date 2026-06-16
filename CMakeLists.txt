cmake_minimum_required(VERSION 3.10)
project(dianyun VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required packages
find_package(OpenCV REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(Boost REQUIRED SYSTEM FILESYSTEM)
find_package(PCL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(GLEW REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${OPENCV_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${PCL_INCLUDE_DIRS})
include_directories(${GLFW3_INCLUDE_DIRS})

# Add link directories
link_directories(${PCL_LIBRARY_DIRS})

# Source files
set(COMMON_SOURCES
    src/common/data_types.h
    src/common/constants.h
)

set(SENSOR_DRIVER_SOURCES
    src/sensor_driver/sensor_base.h
    src/sensor_driver/lidar_driver.h
    src/sensor_driver/imu_driver.h
    src/sensor_driver/camera_driver.h
)

set(TIME_SYNC_SOURCES
    src/time_sync/time_sync.h
    src/time_sync/time_sync.cpp
    src/time_sync/clock_manager.h
    src/time_sync/clock_manager.cpp
)

set(PROCESSING_SOURCES
    src/processing/imu_processor.h
    src/processing/imu_processor.cpp
    src/processing/camera_processor.h
    src/processing/camera_processor.cpp
    src/processing/pointcloud_stitcher.h
    src/processing/pointcloud_stitcher.cpp
    src/processing/pointcloud_classifier.h
    src/processing/pointcloud_classifier.cpp
    src/processing/pointcloud_colorer.h
    src/processing/pointcloud_colorer.cpp
)

set(VISUALIZATION_SOURCES
    src/visualization/visualizer.h
    src/visualization/visualizer.cpp
    src/visualization/camera.h
    src/visualization/camera.cpp
    src/visualization/shader.h
    src/visualization/shader.cpp
)

set(UTILS_SOURCES
    src/utils/logger.h
    src/utils/logger.cpp
    src/utils/math_utils.h
    src/utils/math_utils.cpp
    src/utils/timer.h
    src/utils/timer.cpp
)

# Create main executable
add_executable(mdc_pointcloud_app
    src/main.cpp
    ${COMMON_SOURCES}
    ${SENSOR_DRIVER_SOURCES}
    ${TIME_SYNC_SOURCES}
    ${PROCESSING_SOURCES}
    ${VISUALIZATION_SOURCES}
    ${UTILS_SOURCES}
)

# Link libraries
target_link_libraries(mdc_pointcloud_app
    ${OpenCV_LIBS}
    Eigen3::Eigen
    ${Boost_LIBRARIES}
    ${PCL_LIBRARIES}
    ${OPENGL_LIBRARIES}
    GLFW::glfw
    GLEW::GLEW
)

# Testing
enable_testing()

# Test executables
add_executable(test_time_sync
    test/test_time_sync.cpp
    src/time_sync/time_sync.cpp
    src/time_sync/clock_manager.cpp
    src/utils/logger.cpp
    src/utils/timer.cpp
)

target_link_libraries(test_time_sync
    ${OpenCV_LIBS}
    Eigen3::Eigen
    ${Boost_LIBRARIES}
)

add_test(NAME TimeSync COMMAND test_time_sync)

add_executable(test_pointcloud
    test/test_pointcloud.cpp
    src/processing/pointcloud_stitcher.cpp
    src/processing/imu_processor.cpp
    src/utils/logger.cpp
    src/utils/math_utils.cpp
)

target_link_libraries(test_pointcloud
    ${OpenCV_LIBS}
    Eigen3::Eigen
    ${PCL_LIBRARIES}
)

add_test(NAME PointCloud COMMAND test_pointcloud)

add_executable(test_imu_processor
    test/test_imu_processor.cpp
    src/processing/imu_processor.cpp
    src/utils/logger.cpp
    src/utils/math_utils.cpp
)

target_link_libraries(test_imu_processor
    Eigen3::Eigen
)

add_test(NAME IMUProcessor COMMAND test_imu_processor)

add_executable(test_coloring
    test/test_coloring.cpp
    src/processing/pointcloud_colorer.cpp
    src/utils/logger.cpp
    src/utils/math_utils.cpp
)

target_link_libraries(test_coloring
    ${OpenCV_LIBS}
    Eigen3::Eigen
    ${PCL_LIBRARIES}
)

add_test(NAME Coloring COMMAND test_coloring)

# Print build info
message(STATUS "========== Build Configuration ==========")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "OpenCV: ${OpenCV_VERSION}")
message(STATUS "Eigen3: Found")
message(STATUS "PCL: Found")
message(STATUS "==========================================")
