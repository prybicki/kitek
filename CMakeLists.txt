cmake_minimum_required(VERSION 3.10)
project(robotics CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(protobuf REQUIRED)
find_package(pigpio REQUIRED)
# find_package(libevdev REQUIRED)
find_package(yaml-cpp REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/lib/cpp)
include_directories(${CMAKE_SOURCE_DIR}/proto/cpp)
include_directories(${pigpio_INCLUDE_DIR})

# --- Handles --- #

add_library(GPIOHandle lib/cpp/GPIOHandle.cpp)
target_link_libraries(GPIOHandle pigpio)

add_library(I2CHandle lib/cpp/I2CHandle.cpp)
target_link_libraries(I2CHandle pigpio fmt)

# --- Hardware --- #

add_library(L298N lib/cpp/L298N.cpp)
target_link_libraries(L298N pigpio m)

add_library(LED lib/cpp/LED.cpp)
target_link_libraries(GPIOHandle pigpio)

add_library(Servo lib/cpp/Servo.cpp)
target_link_libraries(Servo pigpio m)

add_library(PCF8591 lib/cpp/PCF8591.cpp)
target_link_libraries(PCF8591 I2CHandle fmt)

add_library(Encoder lib/cpp/Encoder.cpp)
target_link_libraries(Encoder pigpio)

# --- Systems --- #

add_library(PID lib/cpp/PID.cpp)
target_link_libraries(PID pigpio)

add_library(Wheel lib/cpp/Wheel.cpp)
target_link_libraries(Wheel L298N Encoder stdc++)

add_library(diagnostics lib/cpp/diagnostics.cpp)
target_link_libraries(diagnostics GPIOHandle L298N LED Servo PCF8591 Encoder PID Wheel)

# --- Communication --- #

add_library(Protocol proto/cpp/Protocol.pb.cc)
target_link_libraries(Protocol protobuf::libprotobuf)

add_library(UDPTransceiver lib/cpp/UDPTransceiver.cpp)
target_link_libraries(UDPTransceiver fmt)

# --- User --- #

# add_library(Joy lib/cpp/Joy.cpp)
# target_include_directories(Joy INTERFACE ${LIBEVDEV_INCLUDE_DIR})
# target_link_libraries(Joy fmt ${LIBEVDEV_LIBRARY})

# --- Apps --- #

# add_executable(emitUDP apps/emitUDP.cpp)
# target_link_libraries(emitUDP Protocol UDPTransceiver)

add_executable(kitek apps/cpp/kitek.cpp)
target_link_libraries(kitek pthread diagnostics Protocol UDPTransceiver yaml-cpp)

# add_executable(base apps/base.cpp)
# target_link_libraries(base UDPTransceiver Controller)