cmake_minimum_required(VERSION 3.5)
project(bpl_robot_driver)

if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
  add_compile_options(-Wall -Wextra)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#########################################################################
################# BUILD SPHINX .HTML DOCUMENTATION   ####################
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/docs" ${CMAKE_MODULE_PATH})
add_subdirectory("docs")

################ BUILD DEPENDENCIES  #################
find_package(ament_cmake REQUIRED)          # c++    (optional)
find_package(rclcpp REQUIRED)               # c++    (optional)
find_package(rclcpp_lifecycle REQUIRED)
find_package(ament_cmake_python REQUIRED)   # python (optional)
find_package(rclpy REQUIRED)                # python (optional)
######################################################
################ FIND DEPENDENCIES  ##################
find_package(rclcpp_action REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(hardware_interface REQUIRED)
find_package(pluginlib REQUIRED)
find_package(bpl_protocol REQUIRED)

set(DEPENDDENCIES
  rclcpp
  std_msgs
  sensor_msgs
  rclcpp_action
  control_msgs
)
######################################################
include_directories(
    include
)
######################################################
################### INSTALL C++  #####################
add_library(bpl_robot_driver_interface SHARED src/bpl_robot_driver_interface.cpp)
target_link_libraries(bpl_robot_driver_interface evpp)
ament_target_dependencies(bpl_robot_driver_interface bpl_protocol)

################### Robot Hardware ##################
add_library(bpl_hardware
    SHARED 
    src/bpl_robot_hardware_interface.cpp
)
target_include_directories(
    bpl_hardware
    PRIVATE
    include
)
target_compile_features(bpl_hardware PUBLIC cxx_std_17)
target_link_libraries(bpl_hardware evpp bpl_robot_driver_interface pthread)
ament_target_dependencies(
    bpl_hardware
    hardware_interface
    pluginlib
    rclcpp_lifecycle
    ${DEPENDDENCIES}
)
pluginlib_export_plugin_description_file(hardware_interface bpl_hardware.xml)

################### Gripper Hardware ##################
add_library(bpl_gripper
    SHARED 
    src/bpl_gripper_hardware_interface.cpp
)
target_include_directories(
    bpl_gripper
    PRIVATE
    include
)
target_link_libraries(bpl_gripper evpp bpl_robot_driver_interface pthread)
ament_target_dependencies(
    bpl_gripper
    hardware_interface
    pluginlib
    ${DEPENDDENCIES}
)
pluginlib_export_plugin_description_file(hardware_interface bpl_gripper.xml)

add_executable(test_driver_interface test/test_driver_interface.cpp)
target_link_libraries(test_driver_interface bpl_robot_driver_interface pthread)

add_executable(test_driver_interface_v2 test/test_driver_interface_v2.cpp)
target_link_libraries(test_driver_interface_v2 bpl_robot_driver_interface pthread)

###################### END ##########################
install(
  TARGETS
    test_driver_interface
    test_driver_interface_v2
  DESTINATION lib/${PROJECT_NAME})
# INSTALL
install(
  TARGETS 
    bpl_robot_driver_interface
    bpl_hardware
    bpl_gripper
  DESTINATION lib
)
install(
  DIRECTORY include/
  DESTINATION include
)
#############
## EXPORTS
ament_export_include_directories(
  include
)
ament_export_libraries(
    bpl_hardware
    bpl_gripper
)
ament_export_dependencies(
  hardware_interface
  pluginlib
  rclcpp
  rclcpp_lifecycle
)
ament_package() # always required
