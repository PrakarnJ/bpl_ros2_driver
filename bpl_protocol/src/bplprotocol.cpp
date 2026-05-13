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

#include "bpl_protocol/bplprotocol.hpp"

namespace bpl_protocol
{

BPLProtocol::BPLProtocol(const std::string & robot_ip, const int port)
: ip_(robot_ip), port_(port), controller_connected_flag_(true)
{
  if (!this->Connect(ip_.data(), port_)) {
    controller_connected_flag_ = false;
  } else {
    std::cout << "========= Connect Success =================" << std::endl;
    controller_connected_flag_ = true;
  }

  joint_states_.resize(ARM_DOF, std::numeric_limits<double>::quiet_NaN());
  encode_all.resize(MAX_PACKET_LENGTH, 0);
}

BPLProtocol::~BPLProtocol() { shutdown(); }

void BPLProtocol::shutdown()
{
  keep_running_ = false;
  if (receive_thread_->joinable()) {
    receive_thread_->join();
  }
}

void BPLProtocol::receive()
{
  while (keep_running_) {
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    rx_receive();
  }
}

void BPLProtocol::initialize_receive()
{
  keep_running_ = true;
  receive_thread_ = std::make_unique<std::thread>(&BPLProtocol::receive, this);
}

bool BPLProtocol::get_data(uint8_t device_id, uint8_t packet_id, ByteArray & data)
{
  struct Packet packet;
  packet.device_id = device_id;
  packet.packet_id = packet_id;
  packet.data_length = data.size();
  packet.data.insert(packet.data.end(), data.begin(), data.begin() + packet.data_length);

  ByteArray encode_package;
  encode_package.resize(MAX_PACKET_LENGTH, 0);

  size_t packet_length = encode_packet(encode_package, &packet);
  std::string req(encode_package.begin(), encode_package.end());
  if (!this->Send(req)) {
    int eno = errno;
    LOG_ERROR << "sent failed, errno=" << eno << " " << strerror(eno);
    return 0;
  } else {
    // std::cout << "send success" << std::endl;
    return 1;
  }
}

bool BPLProtocol::write_joint_command(
  ByteArray device_id, uint8_t packet_id, const std::vector<double> & data)
{
  prepare_encoded_array(device_id, packet_id, data);
  return tx_transmit();
}

void BPLProtocol::prepare_encoded_array(
  ByteArray device_id, uint8_t packet_id, const std::vector<double> & data)
{
  {
    std::unique_lock<std::mutex> lock(tx_mutex_);
    for (int i = 0; i < device_id.size(); i++) {
      ByteArray encoded_float;
      struct Packet packet;
      packet.data.resize(MAX_PACKET_LENGTH, 0);
      size_t data_length = encode_float(encoded_float, data[i]);

      packet.device_id = device_id.at(i);
      packet.packet_id = packet_id;

      std::copy(encoded_float.begin(), encoded_float.begin() + data_length, packet.data.begin());
      packet.data_length = data_length;
      // encode the packet.
      ByteArray encode_package;
      encode_package.resize(MAX_PACKET_LENGTH, 0);
      size_t packetLength = encode_packet(encode_package, &packet);

      encode_all.insert(
        encode_all.end(), encode_package.begin(), encode_package.begin() + packetLength + 1);
    }
  }
  buf_tx_queue_.push(encode_all);
  encode_all.clear();
  encode_all.resize(MAX_PACKET_LENGTH, 0);
}

bool BPLProtocol::tx_transmit()
{
  std::cout << "buff tx queue size: " << buf_tx_queue_.size() << std::endl;
  if (!buf_tx_queue_.empty()) {
    ByteArray br = buf_tx_queue_.front();
    buf_tx_queue_.pop();
    if (controller_connected_flag_) {
      std::string req(br.begin(), br.end());
      if (!this->Send(req)) {
        int eno = errno;
        LOG_ERROR << "sent failed, errno=" << eno << " " << strerror(eno);
        return 0;
      } else {
        // std::cout << "send success" << std::endl;
        return 1;
      }
    }
  } else {
    std::cout << "no tx queue" << std::endl;
  }
}

void BPLProtocol::rx_receive()
{
  evpp::sock::SetTimeout(this->sockfd(), 1000);

  size_t buf_size = 4096;  // The UDP max payload size
  evpp::udp::MessagePtr msg(new evpp::udp::Message(this->sockfd(), buf_size));
  socklen_t addrLen = sizeof(struct sockaddr);
  int readn = ::recvfrom(
    this->sockfd(), msg->WriteBegin(), buf_size, 0, msg->mutable_remote_addr(), &addrLen);
  int err = errno;
  if (readn >= 0) {
    // std::cout << "rx recv data" << std::endl;
    msg->WriteBytes(readn);
    std::string resp = std::string(msg->data(), msg->size());
    ByteArray recv_data(resp.begin(), resp.end());
    int result = decode_packet(&packet_recv, recv_data, recv_data.size());
    buf_rx_queue_.push(packet_recv);
    read_joint_states();
  } else {
    LOG_ERROR << "errno=" << err << " " << strerror(err) << " recvfrom return -1";
  }
}

void BPLProtocol::read_joint_states()
{
  if (!buf_rx_queue_.empty()) {
    // std::cout << "buff rx queue size: " << buf_rx_queue_.size() << std::endl;
    struct Packet packet = buf_rx_queue_.front();
    buf_rx_queue_.pop();
    if (controller_connected_flag_) {
      if (packet.packet_id == POSITION) {
        if (std::count(joint_id.begin(), joint_id.end(), packet.device_id)) {
          int index = static_cast<int>(packet.device_id);
          if (index == 1) {
            g_state_ = decode_float(packet.data) * 0.001;
          } else {
            joint_states_[7 - index] = decode_float(packet.data);
          }
          // std::cout << "Element found: " << static_cast<int>(packet.device_id) << std::endl;
        } else {
          std::cout << "Element not found" << std::endl;
        }
      }
    }
  } else {
    std::cout << "no rx joint state queue" << std::endl;
  }
}

size_t BPLProtocol::encode_float(ByteArray & output_buffer, float f)
{
  uint32_t as_int = *((uint32_t *)&f);
  int i;
  uint8_t encoded_float_data[4];
  for (i = 0; i < 4; i++) {
    encoded_float_data[i] = (as_int >> 8 * i) & 0xFF;
    output_buffer.push_back(encoded_float_data[i]);
  }
  return 4;
}

size_t BPLProtocol::encode_floats(ByteArray & output_buffer, float * float_list, size_t length)
{
  uint8_t encoded_float_data[4 * length];
  for (int i = 0; i < length; i++) {
    float f = float_list[i];
    uint32_t as_int = *((uint32_t *)&f);
    for (int j = 0; j < 4; j++) {
      encoded_float_data[i * 4 + j] = (as_int >> 8 * j) & 0xFF;
    }
  }
  for (int i = 0; i != length * sizeof(uint8_t) * 4; i++) {
    output_buffer.push_back(encoded_float_data[i]);
  }
  return length * sizeof(uint8_t) * 4;
}

float BPLProtocol::decode_float(ByteArray & input_buffer)
{
  uint32_t as_int = 0;
  for (int i = 0; i < 4; i++) {
    as_int = as_int | (input_buffer.at(i) << 8 * i);
  }
  float f = *((float *)&as_int);
  return f;
}

size_t BPLProtocol::decode_floats(
  float * output_float_buffer, ByteArray & input_buffer, size_t input_buffer_length)
{
  size_t float_length = input_buffer_length / 4;
  uint8_t * input_buffer_pointer;
  uint32_t as_int;
  for (int i = 0; i < float_length; i++) {
    as_int = 0;
    input_buffer_pointer = &input_buffer[i * 4];

    for (int j = 0; j < 4; j++) {
      as_int = as_int | (input_buffer_pointer[j] << 8 * j);
    }

    output_float_buffer[i] = *((float *)&as_int);
  }

  return float_length;
}

size_t BPLProtocol::encode_packet_bare(
  ByteArray & packet_buffer, uint8_t device_id, uint8_t packet_id, ByteArray & data, size_t length)
{
  ByteArray temp_buffer;
  temp_buffer.resize(MAX_PACKET_LENGTH, 0);
  size_t total_length = length + HEADER_SIZE;
  std::copy(data.begin(), data.begin() + length, temp_buffer.begin());
  temp_buffer.at(total_length - 4) = packet_id;
  temp_buffer.at(total_length - 3) = device_id;
  temp_buffer.at(total_length - 2) = total_length;
  temp_buffer.at(total_length - 1) = CRC8::crc8(0xff, &temp_buffer.front(), total_length - 1);
  size_t packet_length = Cobs::cobs_encode(temp_buffer, total_length, packet_buffer);
  return (size_t)packet_length;
}

size_t BPLProtocol::encode_packet(ByteArray & packet_buffer, struct Packet * packet)
{
  ByteArray temp_buffer;
  temp_buffer.resize(MAX_PACKET_LENGTH, 0);
  size_t length = packet->data_length;

  size_t total_length = length + HEADER_SIZE;
  std::copy(packet->data.begin(), packet->data.begin() + length, temp_buffer.begin());
  temp_buffer[total_length - 4] = packet->packet_id;
  temp_buffer[total_length - 3] = packet->device_id;
  temp_buffer[total_length - 2] = total_length;
  temp_buffer[total_length - 1] = CRC8::crc8(0xff, &temp_buffer.front(), total_length - 1);
  size_t packet_length = Cobs::cobs_encode(temp_buffer, total_length, packet_buffer);
  return packet_length;
}

int BPLProtocol::decode_packet(
  struct Packet * packet, ByteArray & input_data, size_t input_data_length)
{
  if (input_data_length > MAX_PACKET_LENGTH) {
    return -1;
  }

  size_t packet_end_index = 0;
  while (input_data.at(packet_end_index) != 0 && packet_end_index < input_data_length) {
    packet_end_index++;
  }

  uint8_t packet_length = input_data.at(packet_end_index - 2);

  if (packet_length < 4 || packet_length >= input_data_length) {
    // Length is incoreect
    packet->packet_id = 0;
    packet->device_id = 0;
    return -2;
  }

  ByteArray unwraped_buffer;
  unwraped_buffer.resize(MAX_PACKET_LENGTH, 0);
  std::copy(input_data.begin(), input_data.begin() + packet_end_index, unwraped_buffer.begin());

  unwraped_buffer[packet_end_index] = 0xFF;

  ByteArray decoded_data;
  decoded_data.resize(MAX_PACKET_LENGTH, 0);

  Cobs::cobs_decode(unwraped_buffer, packet_length + 1, decoded_data);

  uint8_t crc = decoded_data[packet_length - 1];
  uint8_t deviceID = decoded_data[packet_length - 3];
  uint8_t packetID = decoded_data[packet_length - 4];

  uint8_t crc_check = CRC8::crc8(0xff, &decoded_data.front(), packet_length - 1);

  if (crc != crc_check) {
    // CRC is not correct
    return -3;
  }

  packet->data_length = packet_length - 4;
  packet->device_id = deviceID;
  packet->packet_id = packetID;
  packet->data.resize(MAX_PACKET_LENGTH, 0);
  std::copy(decoded_data.begin(), decoded_data.begin() + packet_length - 4, packet->data.begin());

  return 1;
}
}  // namespace bpl_protocol