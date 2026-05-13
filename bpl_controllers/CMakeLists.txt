cmake_minimum_required(VERSION 3.5)
project(bpl_controllers)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

################ BUILD DEPENDENCIES  #################
find_package(ament_cmake REQUIRED)          # c++    (optional)
find_package(rclcpp REQUIRED)               # c++    (optional)
find_package(ament_cmake_python REQUIRED)   # python (optional)
find_package(rclpy REQUIRED)                # python (optional)
######################################################
################ FIND DEPENDENCIES  ##################
find_package(std_msgs REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(moveit_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(trajectory_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(control_msgs REQUIRED)
find_package(moveit_ros_planning_interface REQUIRED)
find_package(moveit_ros_move_group REQUIRED)
find_package(moveit_ros_planning REQUIRED)
find_package(moveit_ros_perception REQUIRED)
find_package(moveit_core REQUIRED)
find_package(moveit_common REQUIRED)
find_package(control_toolbox REQUIRED)
moveit_package()
find_package(rosidl_default_runtime REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(PCL REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(rosidl_default_runtime REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(pj_moveit2_msgs REQUIRED)
######################################################
set(DEPENDDENCIES
  rclcpp
  std_msgs
  rclcpp_action
  control_msgs
)

set(THIS_PACKAGE_INCLUDE_DEPENDS
  rclcpp
  rclcpp_components
  std_msgs
  std_srvs
  sensor_msgs
  control_msgs
  control_toolbox
  moveit_msgs
  geometry_msgs
  tf2_eigen
  trajectory_msgs
  moveit_ros_planning
  moveit_core
  pj_moveit2_msgs
)

#######################################################
include_directories(include)

######################################################
################### INSTALL C++  #####################
set(BPL_SERVO_CONTROLLER_INPUT bravo_7_servo_controller_input)
add_library(${BPL_SERVO_CONTROLLER_INPUT} SHARED src/joystick_bpl_control.cpp)
ament_target_dependencies(${BPL_SERVO_CONTROLLER_INPUT} ${THIS_PACKAGE_INCLUDE_DEPENDS})
rclcpp_components_register_nodes(${BPL_SERVO_CONTROLLER_INPUT} "moveit_servo::BplJoyToServoPub")

# Install Libraries
install(
  TARGETS
    ${BPL_SERVO_CONTROLLER_INPUT}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  DESTINATION lib/${PROJECT_NAME})

####################### END ##########################
################ INSTALL LAUNCH, ETC #################
install(DIRECTORY
#   launch
  config
  DESTINATION share/${PROJECT_NAME})
####################### END ##########################

ament_package() # always required
