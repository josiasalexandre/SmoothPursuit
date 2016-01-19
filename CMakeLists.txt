project(dsp)

cmake_minimum_required(VERSION 2.6)

find_package(OpenCV REQUIRED)
find_package(Qt4 REQUIRED)

add_executable(dsp main.cpp)

include_directories(
    include/
    include/MatrixT/
    include/CircularBuffer/
    include/Devices/
    include/Devices/Video/
    include/Devices/Image/
    include/Devices/Math/
    include/Devices/Filters/
    include/Devices/Interpolation//
    include/ListT/
)

set(DSP_LIB_SOURCES
    src/OpticalFlowCPU.cpp
    src/ImageMotionModel.cpp
)

## Declare a C++ library
add_library(dsp_libs ${DSP_LIB_SOURCES})

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(dsp ${OpenCV_LIBS})
target_link_libraries(dsp ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
target_link_libraries(dsp ${OpenCV_LIBS})
target_link_libraries(dsp dsp_libs)

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
