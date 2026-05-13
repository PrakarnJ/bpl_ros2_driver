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

#include <stdio.h>

#include "bpl_protocol/bplprotocol.hpp"

using namespace bpl_protocol;
using namespace std;

int main(int argc, char const * argv[])
{
  float f = 0.8f;
  ByteArray a_data;
  cout << "encode_float: ";
  size_t length_a = BPLProtocol::encode_float(a_data, f);
  for (auto i : a_data) cout << static_cast<int>(i) << " ";
  cout << endl;

  cout << "encode_floats: ";
  ByteArray b_data;
  float fl[5] = {1.2, 2.3, 3.4, 4.5, 5.6};
  size_t length_b = BPLProtocol::encode_floats(b_data, fl, sizeof(fl) / sizeof(fl[0]));
  for (auto i : b_data) cout << static_cast<int>(i) << " ";
  cout << endl;

  ByteArray encode_package;
  encode_package.resize(MAX_PACKET_LENGTH, 0);
  uint8_t device_id = 0x01;
  uint8_t packet_id = MODE;
  ByteArray data{5};  // Int packet
  // encode the packet.
  size_t packet_length =
    BPLProtocol::encode_packet_bare(encode_package, device_id, packet_id, data, data.size());
  // Print the encoded packet to stdio.
  cout << "Encoded Packet: ";
  for (int i = 0; i <= packet_length; i++) {
    cout << static_cast<int>(encode_package[i]) << " ";
  }
  cout << endl;

  ByteArray encode_package2;
  encode_package2.resize(MAX_PACKET_LENGTH, 0);
  // Encoding data with the following information
  uint8_t deviceID = 0x01;
  uint8_t packetID = MODE;
  ByteArray data2{1, 2, 3, 4, 5};  // Int packet
  struct Packet packet;
  packet.device_id = deviceID;
  packet.packet_id = packetID;
  packet.data.resize(MAX_PACKET_LENGTH, 0);
  std::copy(data2.begin(), data2.begin() + data2.size(), packet.data.begin());
  packet.data_length = data2.size();
  // encode the packet.
  size_t packetLength = BPLProtocol::encode_packet(encode_package2, &packet);
  // Print the encoded packet to stdio.
  cout << "Encoded Packet: ";
  for (int i = 0; i <= packetLength; i++) {
    cout << static_cast<int>(encode_package2[i]) << " ";
  }
  cout << endl;

  // create a buffer of bytes for your packet to be filled in with.
  ByteArray encoded_packet3{13, 154, 153, 153, 63, 51, 51,  19, 64, 154, 153, 89,  64,
                            1,  11,  144, 64,  51, 51, 179, 64, 3,  1,   24,  145, 0};

  // Encoding data with the following information
  struct Packet packet2;

  // devode the packet.
  int result = BPLProtocol::decode_packet(&packet2, encoded_packet3, encoded_packet3.size());

  if (result == -1) {
    cout << "decodePacket ERROR: LENGTH: " << encoded_packet3.size()
         << "is longer than Maximum Packet Length " << MAX_PACKET_LENGTH << endl;
    return 0;
  } else if (result == -2) {
    cout << "decodePacket ERROR: Read Packet Length is invalid." << endl;
    return 0;
  }

  else if (result == -3) {
    cout << "decodePacket ERROR: CRC Check did not Match." << endl;
    return 0;
  } else if (result < 0) {
    cout << "decodePacket ERROR: Unknown Error." << endl;
    return 0;
  }

  cout << "Decoded packet: " << endl;
  cout << "Device ID: " << static_cast<int>(packet2.device_id) << endl;
  cout << "Packet ID: " << static_cast<int>(packet2.packet_id) << endl;
  cout << "Packet data: ";
  for (int i = 0; i < packet2.data_length; i++) {
    cout << static_cast<int>(packet2.data[i]) << " ";
  }
  cout << endl;
  cout << "Decoding data to floats" << endl;

  float float_list[MAX_PACKET_LENGTH / 4];

  size_t float_length = BPLProtocol::decode_floats(float_list, packet2.data, packet2.data_length);

  cout << "Decoded Floats: ";

  for (int i = 0; i < float_length; i++) {
    cout << float_list[i] << " ";
  }
  cout << endl;
  return 0;
}
