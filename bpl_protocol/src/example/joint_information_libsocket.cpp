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

#include <thread>

#include "bpl_protocol/bplprotocol.hpp"

using namespace bpl_protocol;
using namespace std;

void request_joint_information(std::shared_ptr<libsocket::inet_dgram_client> sock)
{
  string port = "6789";
  int port_ = 6789;
  string ip = "192.168.2.3";
  BPLProtocol bb(ip, port_);
  // Encoding data with the following information
  ByteArray deviceID{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
  uint8_t packetID = REQUEST;
  ByteArray data{POSITION};

  struct Packet packet;
  while (true) {
    for (int i = 0; i < deviceID.size(); i++) {
      ByteArray encoded_float;
      ByteArray encode_package;
      encode_package.resize(MAX_PACKET_LENGTH, 0);

      packet.device_id = deviceID.at(i);
      packet.packet_id = packetID;
      packet.data_length = data.size();
      packet.data.insert(packet.data.end(), data.begin(), data.begin() + packet.data_length);

      size_t packetLength = bb.encode_packet(encode_package, &packet);

      string req(encode_package.begin(), encode_package.end());
      sock->sndto(req, ip, port);
      sleep(1);
      cout << "senddd" << endl;
    }
  }
}

void response_joint_information(
  std::shared_ptr<libsocket::inet_dgram_client> sock, std::vector<float> joint_state)
{
  cout << "receive" << endl;
  string port = "6789";
  int port_ = 6789;
  string ip = "192.168.2.3";
  BPLProtocol bb(ip, port_);
  // Encoding data with the following information
  ByteArray deviceID{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
  uint8_t packetID = REQUEST;
  ByteArray data{POSITION};

  string from1;
  string from2;
  string resp;
  resp.resize(MAX_PACKET_LENGTH);

  while (true) {
    sock->rcvfrom(resp, from1, from2);
    // cout << resp.size() << endl;

    ByteArray recv_data(resp.begin(), resp.end());
    // cout << "Packet: ";
    // for (int i = 0; i <= recv_data.size(); i++) {
    // cout << static_cast<int>(recv_data[i]) << " ";
    // }
    // cout << endl;

    struct Packet packet2;
    int result = bb.decode_packet(&packet2, recv_data, recv_data.size());
    cout << static_cast<int>(packet2.device_id) << endl;
    float f = bb.decode_float(packet2.data);
    int num = static_cast<int>(packet2.device_id);
    if (num == 1 && f > 0.0001) {
      joint_state[num] = f * 0.001;
    } else if (num != 1 && f > 0.0001) {
      joint_state[num] = f;
    }

    for (int i = 1; i <= joint_state.size(); i++) {
      cout << "joint " << i << ": " << joint_state[i] << endl;
    }
  }
}
int main(int argc, char const * argv[])
{
  auto w = std::make_shared<libsocket::inet_dgram_client>(LIBSOCKET_IPv4);
  std::vector<float> joint_state{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  std::thread req(request_joint_information, w);
  std::thread resp(response_joint_information, w, joint_state);
  req.join();
  resp.join();
  return 0;
}
