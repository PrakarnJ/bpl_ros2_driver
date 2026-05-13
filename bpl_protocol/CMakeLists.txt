cmake_minimum_required(VERSION 3.5)
project(bpl_protocol)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#########################################################################
include_directories(include)

################ BUILD DEPENDENCIES  #################
find_package(ament_cmake REQUIRED)          # c++    (optional)
find_package(rclcpp REQUIRED)               # c++    (optional)
find_package(ament_cmake_python REQUIRED)   # python (optional)
find_package(rclpy REQUIRED)                # python (optional)
######################################################
################ FIND DEPENDENCIES  ##################
find_package(rclcpp_action REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(hardware_interface REQUIRED)
find_package(pluginlib REQUIRED)
find_package(GTest REQUIRED)
# find_package(pthread REQUIRED)

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
add_library(bpl_protocol
    SHARED 
    src/bplprotocol.cpp 
    src/cobs.cpp 
    src/crc.cpp
)
target_link_libraries(bpl_protocol evpp)

add_executable(test_crc test/test_crc.cpp)

add_executable(struct_main src/structpack.cpp test/struct_main.cpp)
target_link_libraries(struct_main gtest pthread)

add_executable(udp_socket_client_main test/udp_socket_client_main.cpp)
target_link_libraries(udp_socket_client_main evpp)

add_executable(floattohex_main test/floattohex_main.cpp)

add_executable(test_protocol test/test_protocol.cpp)  
target_link_libraries(test_protocol bpl_protocol)

add_executable(joint_control_evpp src/example/joint_control_evpp.cpp)
target_link_libraries(joint_control_evpp bpl_protocol)

add_executable(simultaneous_joint_control_evpp src/example/simultaneous_joint_control_evpp.cpp)
target_link_libraries(simultaneous_joint_control_evpp bpl_protocol)

add_executable(joint_information_evpp src/example/joint_information_evpp.cpp)
target_link_libraries(joint_information_evpp bpl_protocol pthread)

add_executable(bpl_protocol_node src/bpl_protocol_node.cpp)
target_link_libraries(bpl_protocol_node bpl_protocol pthread)
####################### END ##########################
install(TARGETS
    test_crc
    struct_main
    udp_socket_client_main
    floattohex_main
    test_protocol
    joint_control_evpp
    simultaneous_joint_control_evpp
    joint_information_evpp
    bpl_protocol_node
  RUNTIME
  DESTINATION lib/${PROJECT_NAME})
# Install Libraries
install(
  TARGETS
    bpl_protocol
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY include/${PROJECT_NAME}
DESTINATION include)

#############
ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
ament_package() # always required
