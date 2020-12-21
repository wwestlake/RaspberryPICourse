cmake_minimum_required(VERSION 3.13)

project(blink CXX)

add_compile_options(-Wall -Wextra -Wno-attribute)

find_package(pigpio)

add_executable(blink blink.cpp)
target_link_libraries(blink pigpio)

add_executable(button button.cpp)
target_link_libraries(button pigpio)

