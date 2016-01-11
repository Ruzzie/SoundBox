cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME FunSoundBox)
project(${PROJECT_NAME})

#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries/SPI)
#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries/SD)
#set(SD_RECURSE True)

set(${CMAKE_PROJECT_NAME}_SKETCH FunSoundBox.ino)
generate_arduino_firmware(${CMAKE_PROJECT_NAME})