cmake_minimum_required(VERSION 3.2)
project(ATTINY85_projects_root)

execute_process(COMMAND ${CMAKE_HOME_DIRECTORY}/configure ${CMAKE_HOME_DIRECTORY})
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(HID-Emulator)
