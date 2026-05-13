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
#include "bpl_protocol/crc.hpp"

namespace bpl_protocol
{
unsigned CRC8::crc8(unsigned crc, unsigned char const * data, size_t len)
{
  if (data == NULL) return 0;
  crc &= 0xff;
  unsigned char const * end = data + len;
  while (data < end) crc = crc8_table[crc ^ *data++];
  return crc;
}
}  // namespace bpl_protocol