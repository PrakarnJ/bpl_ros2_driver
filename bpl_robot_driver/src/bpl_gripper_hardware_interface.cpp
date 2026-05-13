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

#include "bpl_robot_driver/bpl_gripper_hardware_interface.hpp"

#include <chrono>
#include <cmath>
#include <limits>
#include <memory>
#include <vector>

#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "rclcpp/rclcpp.hpp"

namespace bpl_robot_driver
{

hardware_interface::CallbackReturn BPLGripperHardwareInterface::on_init(
  const hardware_interface::HardwareInfo & info)
{
  robot_ = std::make_unique<RobotDriverInterface>("192.168.2.3", 6789);

  if (
    hardware_interface::ActuatorInterface::on_init(info) !=
    hardware_interface::CallbackReturn::SUCCESS) {
    return hardware_interface::CallbackReturn::ERROR;
  }
  hw_start_sec_ = stod(info_.hardware_parameters["param_hw_start_duration_sec"]);
  hw_stop_sec_ = stod(info_.hardware_parameters["param_hw_stop_duration_sec"]);
  hw_slowdown_ = stod(info_.hardware_parameters["param_hw_slowdown"]);

  hw_states_ = std::numeric_limits<double>::quiet_NaN();
  hw_commands_ = std::numeric_limits<double>::quiet_NaN();
  hw_commands_old_ = std::numeric_limits<double>::quiet_NaN();

  first_pass_ = true;
  initialized_ = false;

  for (const hardware_interface::ComponentInfo & joint : info_.joints) {
    // RRBotSystemPositionOnly has exactly one state and command interface on each joint
    if (joint.command_interfaces.size() != 1) {
      RCLCPP_FATAL(
        rclcpp::get_logger("BPLGripperHardwareInterface"),
        "Joint '%s' has %zu command interfaces found. 1 expected.", joint.name.c_str(),
        joint.command_interfaces.size());
      return hardware_interface::CallbackReturn::ERROR;
    }

    if (joint.command_interfaces[0].name != hardware_interface::HW_IF_POSITION) {
      RCLCPP_FATAL(
        rclcpp::get_logger("BPLGripperHardwareInterface"),
        "Joint '%s' have %s command interfaces found. '%s' expected.", joint.name.c_str(),
        joint.command_interfaces[0].name.c_str(), hardware_interface::HW_IF_POSITION);
      return hardware_interface::CallbackReturn::ERROR;
    }

    if (joint.state_interfaces.size() != 1) {
      RCLCPP_FATAL(
        rclcpp::get_logger("BPLGripperHardwareInterface"),
        "Joint '%s' has %zu state interface. 1 expected.", joint.name.c_str(),
        joint.state_interfaces.size());
      return hardware_interface::CallbackReturn::ERROR;
    }

    if (joint.state_interfaces[0].name != hardware_interface::HW_IF_POSITION) {
      RCLCPP_FATAL(
        rclcpp::get_logger("BPLGripperHardwareInterface"),
        "Joint '%s' have %s state interface. '%s' expected.", joint.name.c_str(),
        joint.state_interfaces[0].name.c_str(), hardware_interface::HW_IF_POSITION);
      return hardware_interface::CallbackReturn::ERROR;
    }
  }

  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn BPLGripperHardwareInterface::on_configure(
  const rclcpp_lifecycle::State & /*previous_state*/)
{
  RCLCPP_INFO(rclcpp::get_logger("BPLGripperHardwareInterface"), "Configuring ...please wait...");

  for (int i = 0; i < hw_start_sec_; i++) {
    rclcpp::sleep_for(std::chrono::seconds(1));
    RCLCPP_INFO(
      rclcpp::get_logger("BPLGripperHardwareInterface"), "%.1f seconds left...", hw_start_sec_ - i);
  }

  // Set some default values when starting the first time
  hw_states_ = 0;
  hw_commands_ = 0;

  RCLCPP_INFO(rclcpp::get_logger("BPLGripperHardwareInterface"), "Successfully configured!");

  return hardware_interface::CallbackReturn::SUCCESS;
}

std::vector<hardware_interface::StateInterface>
BPLGripperHardwareInterface::export_state_interfaces()
{
  std::vector<hardware_interface::StateInterface> state_interfaces;

  state_interfaces.emplace_back(hardware_interface::StateInterface(
    info_.joints[0].name, hardware_interface::HW_IF_POSITION, &hw_states_));

  return state_interfaces;
}

std::vector<hardware_interface::CommandInterface>
BPLGripperHardwareInterface::export_command_interfaces()
{
  std::vector<hardware_interface::CommandInterface> command_interfaces;

  command_interfaces.emplace_back(hardware_interface::CommandInterface(
    info_.joints[0].name, hardware_interface::HW_IF_POSITION, &hw_commands_));

  return command_interfaces;
}

hardware_interface::CallbackReturn BPLGripperHardwareInterface::on_activate(
  const rclcpp_lifecycle::State & /*previous_state*/)
{
  RCLCPP_INFO(rclcpp::get_logger("BPLGripperHardwareInterface"), "Activating ...please wait...");

  for (int i = 0; i < hw_start_sec_; i++) {
    rclcpp::sleep_for(std::chrono::seconds(1));
    RCLCPP_INFO(
      rclcpp::get_logger("BPLGripperHardwareInterface"), "%.1f seconds left...", hw_start_sec_ - i);
  }

  // command and state should be equal when starting
  if (std::isnan(hw_states_)) {
    hw_states_ = 0;
    hw_commands_ = 0;
  } else {
    hw_commands_ = hw_states_;
  }

  RCLCPP_INFO(rclcpp::get_logger("BPLGripperHardwareInterface"), "Successfully activated!");

  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn BPLGripperHardwareInterface::on_deactivate(
  const rclcpp_lifecycle::State & /*previous_state*/)
{
  RCLCPP_INFO(rclcpp::get_logger("BPLGripperHardwareInterface"), "Deactivating ...please wait...");

  for (int i = 0; i < hw_stop_sec_; i++) {
    rclcpp::sleep_for(std::chrono::seconds(1));
    RCLCPP_INFO(
      rclcpp::get_logger("BPLGripperHardwareInterface"), "%.1f seconds left...", hw_stop_sec_ - i);
  }

  RCLCPP_INFO(rclcpp::get_logger("BPLGripperHardwareInterface"), "Successfully deactivated!");

  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::return_type BPLGripperHardwareInterface::read(
  const rclcpp::Time & /*time*/, const rclcpp::Duration & /*period*/)
{
  //   RCLCPP_INFO(rclcpp::get_logger("BPLGripperHardwareInterface"), "Reading...");
  //   robot_->sent_read_joint_states();
  //   const auto p_state = robot_->read_g_state();
  //   for (int i = 0; i < p_state.size(); i++)
  //   {
  //       RCLCPP_INFO(rclcpp::get_logger("BPLGripperHardwareInterface"), "joint %d : %f", i+1, p_state.at(i));
  //   }

  hw_states_ = hw_states_ + (hw_commands_ - hw_states_) / hw_slowdown_;
  if (first_pass_ && !initialized_) {
    hw_commands_ = hw_commands_old_ = hw_states_;
    initialized_ = true;
  }

  return hardware_interface::return_type::OK;
}

hardware_interface::return_type BPLGripperHardwareInterface::write(
  const rclcpp::Time & /*time*/, const rclcpp::Duration & /*period*/)
{
  if (not std::isfinite(hw_commands_)) {
    return hardware_interface::return_type::ERROR;
  } else {
    bool new_data_available = false;

    // create a position differece vector
    double pos_diff = std::abs(hw_commands_ - hw_commands_old_);

    if (pos_diff != 0) {
      new_data_available = true;
    }

    if (new_data_available) {
      robot_->write_joint_command(hw_commands_, POSITION);
      hw_commands_old_ = hw_commands_;
    } else {
      robot_->write_keep_alive();
    }

    // remember old values
    hw_commands_old_ = hw_commands_;

    return hardware_interface::return_type::OK;
  }
}

}  // namespace bpl_robot_driver

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(
  bpl_robot_driver::BPLGripperHardwareInterface, hardware_interface::ActuatorInterface)