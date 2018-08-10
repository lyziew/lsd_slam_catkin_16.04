cmake_minimum_required(VERSION 2.4.6)
project(lsd_slam_viewer)

# Set the build type. Options are:
#  Coverage : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug : w/ debug symbols, w/o optimization
#  Release : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel : w/o debug symbols, w/ optimization, stripped binaries
set(CMAKE_BUILD_TYPE Release)

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/thirdparty/Sophus)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  dynamic_reconfigure
  sensor_msgs
  roscpp
  rosbag
  message_generation
  roslib
  cmake_modules
)

find_package(Pangolin REQUIRED)
find_package(Eigen REQUIRED)
set(OpenCV_DIR /usr/local/share/OpenCV)
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread)

include_directories(
		    ${catkin_INCLUDE_DIRS} 
		    ${EIGEN_INCLUDE_DIR}
        ${Pangolin_INCLUDE_DIRS})

# SSE flags
set(CMAKE_CXX_FLAGS
   "${CMAKE_CXX_FLAGS} -march=native -Wall -std=c++0x"
)

add_message_files(DIRECTORY msg FILES keyframeMsg.msg keyframeGraphMsg.msg)
generate_messages(DEPENDENCIES)

generate_dynamic_reconfigure_options(
  cfg/LSDSLAMViewerParams.cfg
)


# Sources files
set(SOURCE_FILES         
  src/Viewer.cpp
  src/KeyFrameDisplay.cpp
  src/KeyFrameGraphDisplay.cpp
  src/Settings.cpp
)

set(HEADER_FILES     
  src/Viewer.h
  src/KeyFrameDisplay.h
  src/KeyFrameGraphDisplay.h
  src/Settings.h
)


include_directories(
  ${PROJECT_SOURCE_DIR}/thirdparty/Sophus
)  

add_executable(viewer src/main_viewer.cpp ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(viewer 
			     ${catkin_LIBRARIES}
			     ${Boost_LIBRARIES}
           ${Pangolin_LIBRARIES}
		     )

add_dependencies(viewer lsd_slam_viewer_generate_messages_cpp)
