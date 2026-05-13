/**
 * Copyright 2026 Prakarn Jaroonsorn <prakarn.jrs@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef BPL_ROBOT_DRIVER__BPL_ROBOT_HARDWARE_INTERFACE_HPP_
#define BPL_ROBOT_DRIVER__BPL_ROBOT_HARDWARE_INTERFACE_HPP_

// System
#include <memory>
#include <string>
#include <vector>

// Ros2 control hardware interface
#include "bpl_robot_driver/bpl_robot_driver_interface.hpp"
#include "bpl_robot_driver/visibility_control.h"
#include "hardware_interface/handle.hpp"
#include "hardware_interface/hardware_info.hpp"
#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
#include "rclcpp/macros.hpp"
#include "rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp"
#include "rclcpp_lifecycle/state.hpp"

// ros
#include "rclcpp/macros.hpp"
#include "sensor_msgs/msg/joint_state.hpp"

namespace bpl_robot_driver
{
class BPLRobotHardwareInterface : public hardware_interface::SystemInterface
{
public:
  RCLCPP_SHARED_PTR_DEFINITIONS(BPLRobotHardwareInterface);

  BPL_ROS2_CONTROL_HARDWARE_PUBLIC
  hardware_interface::CallbackReturn on_init(
    const hardware_interface::HardwareInfo & info) override;

  BPL_ROS2_CONTROL_HARDWARE_PUBLIC
  hardware_interface::CallbackReturn on_configure(
    const rclcpp_lifecycle::State & previous_state) override;

  BPL_ROS2_CONTROL_HARDWARE_PUBLIC
  std::vector<hardware_interface::StateInterface> export_state_interfaces() override;

  BPL_ROS2_CONTROL_HARDWARE_PUBLIC
  std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;

  BPL_ROS2_CONTROL_HARDWARE_PUBLIC
  hardware_interface::CallbackReturn on_activate(
    const rclcpp_lifecycle::State & previous_state) override;

  BPL_ROS2_CONTROL_HARDWARE_PUBLIC
  hardware_interface::CallbackReturn on_deactivate(
    const rclcpp_lifecycle::State & previous_state) override;

  BPL_ROS2_CONTROL_HARDWARE_PUBLIC
  hardware_interface::return_type read(
    const rclcpp::Time & time, const rclcpp::Duration & period) override;

  BPL_ROS2_CONTROL_HARDWARE_PUBLIC
  hardware_interface::return_type write(
    const rclcpp::Time & time, const rclcpp::Duration & period) override;

  static const size_t number_of_joints = 6;

private:
  std::unique_ptr<RobotDriverInterface> robot_;

  // Parameters
  double hw_start_sec_;
  double hw_stop_sec_;
  double hw_slowdown_;

  bool first_pass_;
  bool initialized_;

  // Store the command for the simulated robot
  std::vector<double> hw_commands_;
  std::vector<double> hw_commands_old_;
  std::vector<double> hw_states_;
};

}  // namespace bpl_robot_driver

#endif /* BPL_ROBOT_DRIVER__BPL_ROBOT_HARDWARE_INTERFACE_HPP_ */
