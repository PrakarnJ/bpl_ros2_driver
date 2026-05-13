# Copyright 2026 Prakarn Jaroonsorn <prakarn.jrs@gmail.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from bplprotocol import BPLProtocol, PacketID
import time
from cobs import cobs
import socket

if __name__ == '__main__':
    device_id = 0x02  # joint wrist

    MANIPULATOR_ID_ADDRESS = '192.168.2.5'
    MANIPULATOR_PORT = 6789
    manipulator_address = (MANIPULATOR_ID_ADDRESS, MANIPULATOR_PORT)

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # text = BPLProtocol.encode_packet(device_id, PacketID.POSITION, BPLProtocol.encode_floats([0.3]))
    # text = b'\t\xcd\xcc\xcc=\x03\x02\x08\xe3\x00'
    # text = b'\t\x9a\x99\x99>\x03\x02\x08\x8e\x00'
    # sock.sendto(text, manipulator_address)
    # print(text)

    text2 = BPLProtocol.encode_floats([0.8])
    print(text2.hex())
    # print(bytes.fromhex('3f000000'))

    # tx_packet = bytes(BPLProtocol.encode_floats([0.5]))
    # print(tx_packet)
    # tx_packet += bytes([PacketID.POSITION, device_id, len(tx_packet)+4])
    # print(tx_packet)
    # tx_packet += bytes([BPLProtocol.CRC8_FUNC(tx_packet)])
    # print(tx_packet)
    # encoded = cobs.encode(bytes(tx_packet)) + b'\x00'
    # print(encoded)

    # text = [0x00, 0x00, 0x00, 0x03, 0x05, 0x08, 0xaf]
    # encoded = cobs.encode(bytes(text)) + b'\x00'
    # print(encoded)

    # text = [0x00, 0x00, 0x00, 0x03, 0x05, 0x08]
    # encoded = BPLProtocol.CRC8_FUNC(bytes(text))
    # print(hex(encoded))
