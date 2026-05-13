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
#ifndef BPL_PROTOCOL__BPLMETATYPE_HPP_
#define BPL_PROTOCOL__BPLMETATYPE_HPP_

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

#define MAX_PACKET_LENGTH 64
#define HEADER_SIZE 4
#define ARM_DOF 6

/**
 * @brief BPL Protocol Packet IDs.
 *
 */
typedef enum packetID
{
  // 1 byte - Describes the Mode of a device
  MODE = 0x01,
  // 1 float - Describes the velocity of a device. Radians/s for angular joints. mm/s for linear joints.
  VELOCITY = 0X02,
  // 1 float - Describes the position of a device. In radians or mm
  POSITION = 0x03,
  // 1 float - Describes the current drawn by a device in mA
  CURRENT = 0x05,
  // 1 float - When sent sets the relative position of actuator.
  // The actuator will move from its current position the amount specified in the data.
  RELATIVE_POSITION = 0x0E,
  // 1 float - On first receiving indexed position an offset is created between the indexed position demand received and the
  // current position.
  // New indexed positions packets then move the actuators relative to the initial indexed position.
  INDEXED_POSITION = 0x0D,
  // bytes - Request a packet ID. On receiving the command, the device will send the packet corresponding to the packet IDs in the
  // data field.
  REQUEST = 0x60,
  // 1 float - The unique serial number of the device
  SERIAL_NUMBER = 0x61,
  // 1 float - The model number of the device
  MODEL_NUMBER = 0x62,
  // 1 float - The internal temperature in Celsius
  TEMPERATURE = 0x66,
  // 3 bytes - The software version on the device
  SOFTWARE_VERSION = 0x6C,
  // 6 floats - Request the current end effector position. (X, Y, Z, Y, P, R) in mm and radians. Only for kinematic enabled arms.
  KM_END_POS = 0xA1,
  // 6 floats - Demand the end effector velocity (XYZ, RZ, RY, RX) in mm/s and rads/s.
  // Only for kinematic enabled arm. Rotation commands (RZ, RY, RX) is only available for 7 function arms.
  KM_END_VEL = 0xA2,
  // 6 floats - Demand the end effector velocity relative to the end effector. (XYZ, RZ, RY, RX) in mm/s and rads/s.
  // Only fora kinematic enabled arm. Rotation commands (RZ, RY, RX) is only available for 7 function arms.
  KM_END_VEL_LOCAL = 0xCB,
  // 6 floats - (X1, Y1, Z1, X2, Y2, Z2) mm. Box obstacle defined by 2 opposite corners of a rectangular prism.
  KM_BOX_OBSTACLE_02 = 0xA5,
  KM_BOX_OBSTACLE_03 = 0xA5,
  KM_BOX_OBSTACLE_04 = 0xA5,
  KM_BOX_OBSTACLE_05 = 0xA5,
  // 7 floats - (X1, Y1, Z1, X2, Y2, Z2, R) mm. Cylinder obstacle defined by 2 opposite centers of a cylinder. R defining the
  // radius of the cylinder
  KM_CYLINDER_OBSTACLE_02 = 0xAB,
  KM_CYLINDER_OBSTACLE_03 = 0xAC,
  KM_CYLINDER_OBSTACLE_04 = 0xAD,
  KM_CYLINDER_OBSTACLE_05 = 0xAE,

  // 1 float - The supply voltage in Volts
  VOLTAGE = 0x90,

  // 1 byte - Send this to save user configurable settings on a device
  SAVE = 0x50,

  // 1 byte - set the frequency of a packet to be sent from a device.
  HEARTBEAT_FREQUENCY = 0x92,

  // 10 bytes - Specify the Packet IDs to be sent via heartbeat.
  HEARTBEAT_SET = 0x91,

  // 2 floats - Maximum and Minimum positions of the device
  POSITION_LIMITS = 0x10,
  // 2 floats - Maximum and Minimum velocities of the device
  VELOCITY_LIMITS = 0x11,
  // 2 floats - Maximum and Minimum currents of the device
  CURRENT_LIMITS = 0x12,

  // 6 floats - Read force in N and Torque in Nm from the Force torque sensor.
  // (FX, FY, FZ, TX, TY, TZ). Send this packet to the FT Sensor to Tare it
  ATI_FT_READING = 0xD8,
  BOOTLOADER = 0xFF,

  // 4 floats - Parameters to define the voltage threshold parameters. Enabled (0 or 1), V_Min (V), V_Max (V), time (seconds).
  VOLTAGE_THRESHOLD_PARAMETERS = 0x99,

};

/**
 * @brief BPL Packet Structure
 *
 */
typedef std::vector<uint8_t> ByteArray;
struct Packet
{
  uint8_t packet_id;
  uint8_t device_id;
  ByteArray data;
  uint8_t data_length;
};

#endif /* BPL_PROTOCOL__BPLMETATYPE_HPP_ */
