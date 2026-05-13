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
#ifndef BPL_PROTOCOL__BPL_PROTOCOL_HPP_
#define BPL_PROTOCOL__BPL_PROTOCOL_HPP_

#include <evpp/udp/sync_udp_client.h>
#include <evpp/udp/udp_server.h>

#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "bpl_protocol/bplmetatype.hpp"
#include "bpl_protocol/cobs.hpp"
#include "bpl_protocol/crc.hpp"
namespace bpl_protocol
{
typedef std::vector<uint8_t> ByteArray;
class BPLProtocol : public evpp::udp::sync::Client
{
public:
  BPLProtocol(const std::string & robot_ip, const int port);
  virtual ~BPLProtocol();

  void shutdown();

  /**
   * @brief Encodes a single float into 4 Bytes.
   *
   * @param output_buffer Output buffer to encoded bytes to fill.
   * @param f Float to encode.
   * @return size_t Length of encoded floats (4).
   */
  static size_t encode_float(ByteArray & output_buffer, float f);
  /**
   * @brief Encodes a list of floats.
   *
   * @param output_buffer Output buffer to be filled.
   * @param float_list Pointer to a list of floats.
   * @param length Length of the list of floats.
   * @return size_t Length of the encoded bytes. (4 * length).
   */
  static size_t encode_floats(ByteArray & output_buffer, float * float_list, size_t length);
  /**
   * @brief Decode a single float
   *
   * @param input_buffer Pointer to the bytes list (4 bytes)
   * @return float Single decoded float.
   */
  static float decode_float(ByteArray & input_buffer);
  /**
   * @brief Decode Bytes into floats.
   *
   * @param output_float_buffer Pointer to where to decode the floats into.
   * @param input_buffer Pointer to the list of bytes of the floats.
   * @param input_buffer_length Length of the bytes buffer (should be divisable by 4)
   * @return size_t Amount of floats decoded. (inputBufferLength/4)
   */
  static size_t decode_floats(
    float * output_float_buffer, ByteArray & input_buffer, size_t input_buffer_length);
  /**
   * @brief Encode a packet
   *
   * @param packet_buffer Buffer for packet to be filled into.
   * @param device_id Device ID to Encode
   * @param packet_id Packet ID to Encode
   * @param data data buffer to Encode
   * @param length length of data buffer
   * @return * size_t The size of the packet
   */
  static size_t encode_packet_bare(
    ByteArray & packet_buffer, uint8_t device_id, uint8_t packet_id, ByteArray & data,
    size_t length);
  /**
   * @brief Encode a Packet using a packet struct.
   *
   * @param packet_buffer Buffer for the packet to be filled into.
   * @param packet Packet Structure to Encode.
   * @return size_t Length of the packet.
   */
  static size_t encode_packet(ByteArray & packet_buffer, struct Packet * packet);
  /**
   * @brief Devcodes bytes into a packet.
   *
   * @param packet Packet to fill data into.
   * @param input_data Pointer to the list of bytes.
   * @param input_data_length Length of the bytes.
   * @return int Status of the Decoding. Less than zero is a failure to Decode.
   */
  static int decode_packet(
    struct Packet * packet, ByteArray & input_data, size_t input_data_length);

public:
  bool get_data(uint8_t device_id, uint8_t packet_id, ByteArray & data);
  bool write_joint_command(
    ByteArray device_id, uint8_t packet_id, const std::vector<double> & data);
  bool tx_transmit();
  void rx_receive();

  void initialize_receive();
  void read_joint_states();
  void prepare_encoded_array(
    ByteArray device_id, uint8_t packet_id, const std::vector<double> & data);

  std::string buf_;
  struct Packet packet_recv;

  std::vector<double> joint_states_{};
  double g_state_;

private:
  std::string ip_;
  int port_;
  int buffer_size_;
  bool controller_connected_flag_;
  ByteArray joint_id{0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};

  std::mutex tx_mutex_;

  std::queue<ByteArray> buf_tx_queue_;
  std::queue<Packet> buf_rx_queue_;
  ByteArray encode_all;

  std::unique_ptr<std::thread> receive_thread_;
  std::unique_ptr<std::thread> send_pos_thread_;
  bool keep_running_;
  void receive();
};
}  // namespace bpl_protocol

#endif /* BPL_PROTOCOL__BPL_PROTOCOL_HPP_ */
