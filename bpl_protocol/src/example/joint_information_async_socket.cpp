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

#include "async-sockets/udpsocket.hpp"
#include "bpl_protocol/bplprotocol.hpp"

using namespace bpl_protocol;
using namespace std;

int main(int argc, char const * argv[])
{
  static uint64_t g_timeout_ms = 1000;
  const string IP = "192.168.2.3";
  const uint16_t PORT = 6789;
  BPLProtocol bb(IP, PORT);
  // const string IP = "localhost";
  // const uint16_t PORT = 8888;

  // Encoding data with the following information
  ByteArray deviceID{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
  uint8_t packetID = REQUEST;
  ByteArray data{POSITION};
  std::vector<float> joint_state{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  struct Packet packet;

  UDPSocket udpSocket(true);  // "true" to use Connection on UDP. Default is "false".

  while (true) {
    cout << "" << endl;
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
      // Initialize socket.

      udpSocket.Connect(IP, PORT);

      // Send String:
      udpSocket.Send(req);
      // udpSocket.SendTo(req, IP, PORT); // If you want to connectless

      udpSocket.onMessageReceived = [&](string message, string ipv4, uint16_t port) {
        cout << ipv4 << ":" << port << " => " << message << endl;
        bb.buf_ = message;
      };
      ByteArray recv_data(bb.buf_.begin(), bb.buf_.end());
      // cout << "Packet: ";
      // for (int i = 0; i <= recv_data.size(); i++) {
      // cout << static_cast<int>(recv_data[i]) << " ";
      // }
      // cout << endl;

      struct Packet packet2;
      int result = bb.decode_packet(&packet2, recv_data, recv_data.size());

      float f = bb.decode_float(packet2.data);
      if (i == 0) {
        joint_state.at(i) = f * 0.001;
      } else {
        joint_state.at(i) = f;
      }

      cout << "joint " << i + 1 << ": " << joint_state.at(i) << endl;
      // cout << "Decoded Floats: ";

      // for (int i = 0; i < float_length; i++) {
      //     cout << float_list[i] << " ";
      // }
      // cout << endl;
    }
  }
  return 0;
}
