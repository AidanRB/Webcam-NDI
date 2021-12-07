cmake_minimum_required(VERSION 3.0.0)
project(Webcam-NDI VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(Webcam-NDI main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# NDI SDK
include_directories(${PROJECT_SOURCE_DIR}/ndi_sdk/include)
add_library(ndi SHARED IMPORTED)
target_link_libraries(Webcam-NDI ${PROJECT_SOURCE_DIR}/ndi_sdk/lib/x86_64-linux-gnu/libndi.so)

# fix vs code
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)