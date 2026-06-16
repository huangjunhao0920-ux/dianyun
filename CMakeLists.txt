cmake_minimum_required(VERSION 3.10)
project(dianyun VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(DIANYUN_ENABLE_EXTERNAL_DEPS "Enable OpenCV/Eigen/PCL/Boost/OpenGL stack" OFF)

add_library(dianyun_pipeline
    src/time_sync/time_sync.cpp
    src/time_sync/clock_manager.cpp
    src/processing/imu_processor.cpp
    src/processing/pointcloud_stitcher.cpp
    src/processing/pointcloud_classifier.cpp
    src/processing/pointcloud_colorer.cpp
    src/visualization/visualizer.cpp
    src/utils/logger.cpp
)

target_include_directories(dianyun_pipeline PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_compile_options(dianyun_pipeline PRIVATE -Wall -Wextra -Wpedantic)

if(DIANYUN_ENABLE_EXTERNAL_DEPS)
    find_package(OpenCV REQUIRED)
    find_package(Eigen3 3.3 REQUIRED NO_MODULE)
    find_package(Boost REQUIRED)
    find_package(PCL REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(GLFW3 REQUIRED)
    find_package(GLEW REQUIRED)

    target_link_libraries(dianyun_pipeline PUBLIC
        ${OpenCV_LIBS}
        Eigen3::Eigen
        ${Boost_LIBRARIES}
        ${PCL_LIBRARIES}
        ${OPENGL_LIBRARIES}
        GLFW::glfw
        GLEW::GLEW
    )
endif()

add_executable(dianyun_app src/main.cpp)
target_link_libraries(dianyun_app PRIVATE dianyun_pipeline)

enable_testing()

add_executable(test_time_sync test/test_time_sync.cpp)
target_link_libraries(test_time_sync PRIVATE dianyun_pipeline)
add_test(NAME test_time_sync COMMAND test_time_sync)

add_executable(test_sensor_drivers test/test_sensor_drivers.cpp)
target_link_libraries(test_sensor_drivers PRIVATE dianyun_pipeline)
add_test(NAME test_sensor_drivers COMMAND test_sensor_drivers)

add_executable(test_imu_processor test/test_imu_processor.cpp)
target_link_libraries(test_imu_processor PRIVATE dianyun_pipeline)
add_test(NAME test_imu_processor COMMAND test_imu_processor)

add_executable(test_pointcloud_pipeline test/test_pointcloud_pipeline.cpp)
target_link_libraries(test_pointcloud_pipeline PRIVATE dianyun_pipeline)
add_test(NAME test_pointcloud_pipeline COMMAND test_pointcloud_pipeline)

add_executable(test_visualization test/test_visualization.cpp)
target_link_libraries(test_visualization PRIVATE dianyun_pipeline)
add_test(NAME test_visualization COMMAND test_visualization)

message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "External deps enabled: ${DIANYUN_ENABLE_EXTERNAL_DEPS}")
