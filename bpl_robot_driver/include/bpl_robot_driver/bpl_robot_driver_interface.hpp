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
#ifndef BPL_ROBOT_DRIVER__BPL_ROBOT_DRIVER_INTERFACE_HPP_
#define BPL_ROBOT_DRIVER__BPL_ROBOT_DRIVER_INTERFACE_HPP_

// System
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// bpl
#include "bpl_protocol/bplprotocol.hpp"

using namespace bpl_protocol;
using namespace std;
namespace bpl_robot_driver
{
class RobotDriverInterface
{
public:
  RobotDriverInterface(const std::string & robot_ip, const int port);

  bool write_joint_command(const std::vector<double> & positions, uint8_t control_mode);
  bool write_joint_command(const double position, uint8_t control_mode);
  bool write(const std::vector<double> & positions, ByteArray joint_id, uint8_t control_mode);
  bool write_keep_alive();
  void sent_read_joint_states();

  std::vector<double> read_j_states();
  std::vector<double> j_states_;
  double read_g_state();
  double g_state_;
  std::vector<double> zero_vel_;
  std::unique_ptr<BPLProtocol> bpl_interface_;

private:
  std::string ip_;
  int port_;
  ByteArray gripper_id_{0x01};
  ByteArray joint_id_{0x07, 0x06, 0x05, 0x04, 0x03, 0x02};
  std::mutex read_mutex_;
  std::mutex write_mutex_;
};
}  // namespace bpl_robot_driver
#endif /* BPL_ROBOT_DRIVER__BPL_ROBOT_DRIVER_INTERFACE_HPP_ */
