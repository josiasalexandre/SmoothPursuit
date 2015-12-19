cmake_minimum_required(VERSION 2.8.11)

project(dsp)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(OpenCV REQUIRED)
find_package(Qt5Widgets REQUIRED)

add_executable(dsp main.cpp)

target_link_libraries(dsp Qt5::Widgets)

include_directories(
    include/MatrixT/
    include/CircularBuffer/
    include/Devices/
    include/Devices/Video/
    include/ListT/
)

target_link_libraries(dsp ${OpenCV_LIBS})

install(TARGETS dsp RUNTIME DESTINATION bin)

include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
