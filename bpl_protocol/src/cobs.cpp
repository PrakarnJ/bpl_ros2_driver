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
#include "bpl_protocol/cobs.hpp"

namespace bpl_protocol
{
uint8_t Cobs::cobs_encode(ByteArray & input, uint8_t length, ByteArray & output)
{
  uint8_t read_index = 0;
  uint8_t write_index = 1;
  uint8_t code_index = 0;
  uint8_t code = 1;

  while (read_index < length) {
    if (input.at(read_index) == 0) {
      output.at(code_index) = code;
      code = 1;
      code_index = write_index++;
      read_index++;
    } else {
      output.at(write_index++) = input.at(read_index++);
      code++;
      if (code == 0xFF) {
        output.at(code_index) = code;
        code = 1;
        code_index = write_index++;
      }
    }
  }
  output.at(code_index) = code;
  return write_index;
}

uint8_t Cobs::cobs_decode(ByteArray & input, uint8_t length, ByteArray & output)
{
  uint8_t read_index = 0;
  uint8_t write_index = 0;
  uint8_t code;
  uint8_t i;

  while (read_index < length) {
    code = input.at(read_index);
    if (read_index + code > length && code != 1) return 0;
    read_index++;
    for (i = 1; i < code; i++) output.at(write_index++) = input.at(read_index++);
    if (code != 0xFF && read_index != length) output.at(write_index++) = '\0';
  }
  return write_index;
}
}  // namespace bpl_protocol