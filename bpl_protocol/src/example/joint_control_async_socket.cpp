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

#include <iostream>

#include "async-sockets/udpsocket.hpp"
#include "bpl_protocol/bplprotocol.hpp"

using namespace std;
using namespace bpl_protocol;

int main()
{
  const string IP = "192.168.2.3";
  const uint16_t PORT = 6789;
  BPLProtocol b(IP, PORT);
  // Encoding data with the following information
  uint8_t deviceID = 0x02;  // Joint B
  uint8_t packetID = POSITION;
  float f = 0.1;

  ByteArray encoded_float;
  size_t data_length = b.encode_float(encoded_float, f);

  ByteArray encode_package;
  encode_package.resize(MAX_PACKET_LENGTH, 0);

  struct Packet packet;
  packet.device_id = deviceID;
  packet.packet_id = packetID;
  packet.data.resize(MAX_PACKET_LENGTH, 0);
  std::copy(encoded_float.begin(), encoded_float.begin() + data_length, packet.data.begin());
  packet.data_length = data_length;
  // encode the packet.
  size_t packetLength = b.encode_packet(encode_package, &packet);
  // Print the encoded packet to stdio.
  cout << "Encoded Packet: ";
  for (int i = 0; i <= packetLength; i++) {
    cout << static_cast<int>(encode_package[i]) << " ";
  }
  cout << endl;
  string req(encode_package.begin(), encode_package.end());
  cout << req << endl;
  // Our constants:

  // Initialize socket.
  UDPSocket udpSocket(true);  // "true" to use Connection on UDP. Default is "false".

  udpSocket.SendTo(req, IP, PORT);  // If you want to connectless

  udpSocket.onRawMessageReceived = [&](
                                     const char * message, int length, string ipv4, uint16_t port) {
    cout << ipv4 << ":" << port << " => " << message << "(" << length << ")" << endl;
  };

  // Close the socket.
  udpSocket.Close();

  return 0;
}