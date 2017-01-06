cmake_minimum_required(VERSION 3.1.0)
project(client CXX)
set_target_properties(${TARGET} PROPERTIES LINKER_LANGUAGE CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

include_directories(include)
include_directories(src)

link_directories(lib/linux)
 
add_executable(client 
  src/client.cpp
  src/camera.cpp
  src/calibration.cpp
)

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
