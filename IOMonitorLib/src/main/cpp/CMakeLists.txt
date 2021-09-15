cmake_minimum_required(VERSION 3.0.0)
project(io_monitor VERSION 0.1.0)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

include_directories(xhook)
find_library(log-lib log)
add_library(io_monitor SHARED io_monitor_jni.cpp io_monitor.cpp utils.cpp)
add_library(xhook SHARED IMPORTED)
set_target_properties(xhook PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/xhook/${ANDROID_ABI}/libxhook.so)
target_link_libraries(io_monitor ${log-lib} xhook)

add_library(x_log SHARED log/x_log.cpp log/x_log_buffer.cpp log/x_log_jni.cpp log/x_log_buffer_header.cpp)
target_link_libraries(x_log ${log-lib})


