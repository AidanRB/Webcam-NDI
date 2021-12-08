cmake_minimum_required(VERSION 3.0.0)
project(Webcam-NDI VERSION 0.1.0)

enable_testing()

add_executable(Webcam-NDI main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# OpenCV
set(OpenCV_DIR ${PROJECT_SOURCE_DIR}/opencv-build)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(Webcam-NDI ${OpenCV_LIBS})

# NDI SDK
include_directories(${PROJECT_SOURCE_DIR}/ndi_sdk/include)
add_library(ndi SHARED IMPORTED)
target_link_libraries(Webcam-NDI ${PROJECT_SOURCE_DIR}/ndi_sdk/lib/x86_64-linux-gnu/libndi.so)

# Fix VS Code
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)