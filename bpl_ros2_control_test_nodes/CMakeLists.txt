cmake_minimum_required(VERSION 3.5)
project(bpl_ros2_control_test_nodes)

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
find_package(control_msgs REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(trajectory_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(control_msgs REQUIRED)
find_package(rosidl_default_runtime REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

#######################################################
include_directories(include)

################## INSTALL Python  ###################
# Install Python executables
install(PROGRAMS
  scripts/publisher_joint_trajectory_controller.py
  DESTINATION lib/${PROJECT_NAME}
)
####################### END ##########################

ament_package() # always required
