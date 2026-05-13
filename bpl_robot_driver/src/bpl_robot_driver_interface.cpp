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

#include "bpl_robot_driver/bpl_robot_driver_interface.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <memory>
#include <mutex>
#include <vector>

namespace bpl_robot_driver
{

RobotDriverInterface::RobotDriverInterface(const std::string & robot_ip, const int port)
: ip_(robot_ip), port_(port)
{
  j_states_.resize(ARM_DOF, std::numeric_limits<double>::quiet_NaN());
  zero_vel_.resize(ARM_DOF, 0.0);
  g_state_ = std::numeric_limits<double>::quiet_NaN();

  bpl_interface_ = std::make_unique<BPLProtocol>(ip_, port_);
}

bool RobotDriverInterface::write_joint_command(
  const std::vector<double> & positions, uint8_t control_mode)
{
  std::lock_guard<std::mutex> lock(write_mutex_);
  return write(positions, joint_id_, control_mode);
}

bool RobotDriverInterface::write_joint_command(const double position, uint8_t control_mode)
{
  std::lock_guard<std::mutex> lock(write_mutex_);
  std::vector<double> pos{position * 1000.0};
  return write(pos, gripper_id_, control_mode);
}

bool RobotDriverInterface::write_keep_alive()
{
  return 1;
  // uint8_t packet_id = VELOCITY;
  // return write(zero_vel_, packet_id);
}

bool RobotDriverInterface::write(
  const std::vector<double> & positions, ByteArray joint_id, uint8_t control_mode)
{
  return bpl_interface_->write_joint_command(joint_id, control_mode, positions);
}

void RobotDriverInterface::sent_read_joint_states()
{
  uint8_t device_id = 0xFF;
  uint8_t packet_id = REQUEST;
  ByteArray data{POSITION};
  bpl_interface_->get_data(device_id, packet_id, data);
  // std::cout << "sent " << std::endl;
}

std::vector<double> RobotDriverInterface::read_j_states()
{
  std::lock_guard<std::mutex> lock(read_mutex_);
  j_states_ = bpl_interface_->joint_states_;
  // std::reverse(j_states_.begin(), j_states_.end());
  return j_states_;
}

double RobotDriverInterface::read_g_state()
{
  std::lock_guard<std::mutex> lock(read_mutex_);
  g_state_ = bpl_interface_->g_state_;
  // std::reverse(j_states_.begin(), j_states_.end());
  return g_state_;
}

}  // namespace bpl_robot_driver