cmake_minimum_required(VERSION 2.8.3)
project(wpi_controller)

add_compile_options(-std=c++11)

find_package(Eigen3 REQUIRED)

# Eigen 3.2 (Wily) only provides EIGEN3_INCLUDE_DIR, not EIGEN3_INCLUDE_DIRS
if(NOT EIGEN3_INCLUDE_DIRS)
  set(EIGEN3_INCLUDE_DIRS ${EIGEN3_INCLUDE_DIR})
endif()

find_package(catkin REQUIRED
  COMPONENTS
    interactive_markers
    moveit_core
    moveit_visual_tools
    moveit_ros_planning
    moveit_ros_planning_interface
    pluginlib
    geometric_shapes
    pcl_ros
    pcl_conversions
    rosbag
    tf2_ros
    tf2_eigen
    tf2_geometry_msgs
    #ur5e_workstation
    #robotiq_2f_gripper_control
    #gpd
)

find_package(Boost REQUIRED system filesystem date_time thread)

#set(THIS_PACKAGE_INCLUDE_DIRS
#  doc/interactivity/include
#)

catkin_package(
  LIBRARIES
    interactivity_utils
  INCLUDE_DIRS
    ${THIS_PACKAGE_INCLUDE_DIRS}
  CATKIN_DEPENDS
    moveit_core
    moveit_visual_tools
    moveit_ros_planning_interface
    interactive_markers
  DEPENDS
    EIGEN3
)

###########
## Build ##
###########

include_directories(SYSTEM ${THIS_PACKAGE_INCLUDE_DIRS} ${Boost_INCLUDE_DIR} ${EIGEN3_INCLUDE_DIRS})

#I'm so sorry
include_directories(../../ur5e_workstation/include)

include_directories(include ${catkin_INCLUDE_DIRS})
link_directories(${catkin_LIBRARY_DIRS})

add_executable(wpi_controller src/wpi_controller.cpp)
target_link_libraries(wpi_controller ${catkin_LIBRARIES} ${Boost_LIBRARIES})
#install(TARGETS wpi_controller DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})

add_executable(groupstate_generator src/groupstate_generator.cpp)
target_link_libraries(groupstate_generator ${catkin_LIBRARIES} ${Boost_LIBRARIES})
