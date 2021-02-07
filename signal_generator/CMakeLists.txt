cmake_minimum_required(VERSION 3.13)

project(actors CXX)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra  -Wno-attributes)

set(Boost_USE_STATIC_LIBS        OFF)  # only find static libs
set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(pigpio REQUIRED)


find_package(Boost 1.75.0 COMPONENTS date_time filesystem system program_options PATHS /home/pi/lib/boost_1_75_0/*)
if(Boost_FOUND)
  message("boost found " ${Boost_INCLUDE_DIRS} )
  include_directories(${Boost_INCLUDE_DIRS}  )


  add_subdirectory(sig-gen)

endif()

