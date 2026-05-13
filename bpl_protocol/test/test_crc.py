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

from crcmod import crcmod

if __name__ == '__main__':

    crc32_func = crcmod.mkCrcFun(0x14D, initCrc=0xFF, xorOut=0xFF)

    text = b'\x00\x00\x00\x03\x05\x08'

    encoded = crc32_func(text)
    print(hex(encoded))
