cmake_minimum_required(VERSION 3.1.0)
project(client CXX)
set_target_properties(${TARGET} PROPERTIES 
  LINKER_LANGUAGE CXX
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

include_directories(include)
include_directories(src)

if (WIN32)
  link_directories(lib/windows)
elseif (APPLE)
  link_directories(lib/mac)
elseif (UNIX)
  link_directories(lib/linux)
endif()
 
add_executable(client 
  src/client.cpp
  src/camera.cpp
  src/calibration.cpp
)

if (WIN32)
target_link_libraries(client
  socketio
  libboost_system-vc120-mt-gd-1_63
  libboost_thread-vc120-mt-gd-1_63
  freenect2
  opencv_core320d
  opencv_highgui320d
  opencv_imgcodecs320d
  opencv_aruco320d
  opencv_imgproc320d
  opencv_calib3d320d
)
else ()
target_link_libraries(client
  socketio
  boost_system
  boost_thread
  pthread
  freenect2
  opencv_core
  opencv_highgui
  opencv_imgcodecs
  opencv_aruco
  opencv_imgproc
  opencv_calib3d
)
endif ()


