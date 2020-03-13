cmake_minimum_required(VERSION 3.10)
project(robotics CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(protobuf REQUIRED)
find_package(pigpio REQUIRED)
find_package(libevdev REQUIRED)
find_package(yaml-cpp REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/lib/include)
include_directories(${CMAKE_SOURCE_DIR}/proto/cpp)
include_directories(${pigpio_INCLUDE_DIR})

# --- Handles --- #

add_library(GPIOGuard lib/src/GPIOGuard.cpp)
target_link_libraries(GPIOGuard ${pigpio_LIBRARY})

add_library(I2CHandle lib/src/I2CHandle.cpp)
target_link_libraries(I2CHandle ${pigpio_LIBRARY} fmt)

# --- Hardware --- #

add_library(L298N lib/src/L298N.cpp)
target_link_libraries(L298N ${pigpio_LIBRARY} m)

add_library(LED lib/src/LED.cpp)
target_link_libraries(GPIOGuard ${pigpio_LIBRARY})

add_library(Servo lib/src/Servo.cpp)
target_link_libraries(Servo ${pigpio_LIBRARY} m)

add_library(PCF8591 lib/src/PCF8591.cpp)
target_link_libraries(PCF8591 I2CHandle fmt)

add_library(Encoder lib/src/Encoder.cpp)
target_link_libraries(Encoder ${pigpio_LIBRARY})

# --- Systems --- #

add_library(PID lib/src/PID.cpp)
target_link_libraries(PID ${pigpio_LIBRARY})

add_library(Wheel lib/src/Wheel.cpp)
target_link_libraries(Wheel L298N Encoder stdc++)

add_library(Diagnostics lib/src/Diagnostics.cpp)
target_link_libraries(Diagnostics GPIOGuard L298N LED Servo PCF8591 Encoder PID Wheel)

# --- Communication --- #

add_library(Protocol proto/cpp/Protocol.pb.cc)
target_link_libraries(Protocol protobuf::libprotobuf)

add_library(UDPTransceiver lib/src/UDPTransceiver.cpp)
target_link_libraries(UDPTransceiver fmt)

# --- User --- #

# Rename to controller
add_library(Joy lib/src/Joy.cpp)
target_include_directories(Joy INTERFACE ${LIBEVDEV_INCLUDE_DIR})
target_link_libraries(Joy fmt ${LIBEVDEV_LIBRARY} stdc++fs)

add_library(TickManager lib/src/TickManager.cpp)
target_link_libraries(TickManager ${pigpio_LIBRARY})

# --- Apps --- #

add_executable(emitUDP apps/cpp/emitUDP.cpp)
target_link_libraries(emitUDP Protocol UDPTransceiver)

add_executable(kitek apps/cpp/kitek.cpp)
target_link_libraries(kitek pthread Diagnostics Protocol UDPTransceiver TickManager yaml-cpp)

add_executable(base apps/cpp/base.cpp)
target_link_libraries(base UDPTransceiver Joy Protocol TickManager)