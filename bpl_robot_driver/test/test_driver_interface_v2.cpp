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
#include <bpl_robot_driver/bpl_robot_driver_interface.hpp>
#include <chrono>
#include <memory>
#include <thread>

using namespace bpl_robot_driver;
using namespace bpl_protocol;

void joint_command(
  std::shared_ptr<RobotDriverInterface> node, std::vector<double> pos1, std::vector<double> pos2)
{
  while (true) {
    node->write_joint_command(pos1, POSITION);
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));
    node->write_joint_command(pos2, POSITION);
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));
  }
}

void rx_read(std::shared_ptr<RobotDriverInterface> node)
{
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    node->bpl_interface_->rx_receive();
  }
}

int main(int argc, char const * argv[])
{
  std::string ip = "192.168.2.3";
  int port = 6789;
  auto node = std::make_shared<RobotDriverInterface>(ip, port);

  std::vector<double> pos1{2.5807, 2.567, 0.15453, 3.09729, 0.12522, 0.6};
  std::vector<double> pos2{1.4980, 3.1000, 0.15452, 3.0972, 0.05199, 0.1};

  std::thread t1(joint_command, node, pos1, pos2);
  t1.detach();

  std::thread t2(rx_read, node);
  t2.detach();
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    // std::cout << "In loop" << endl;
    // node->bpl_interface_->rx_receive();
    node->sent_read_joint_states();
    std::cout << "joint state: ";
    for (int i = 0; i < node->bpl_interface_->joint_states_.size(); i++) {
      std::cout << node->bpl_interface_->joint_states_.at(i) << " ";
    }
    std::cout << std::endl;
  }

  return 0;
}
