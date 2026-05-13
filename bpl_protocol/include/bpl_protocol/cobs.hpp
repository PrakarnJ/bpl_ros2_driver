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
#ifndef BPL_PROTOCOL__COBS_HPP_
#define BPL_PROTOCOL__COBS_HPP_

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

namespace bpl_protocol
{
typedef std::vector<uint8_t> ByteArray;
class Cobs
{
public:
  static uint8_t cobs_encode(ByteArray & input, uint8_t length, ByteArray & output);
  static uint8_t cobs_decode(ByteArray & input, uint8_t length, ByteArray & output);
};
}  // namespace bpl_protocol

#endif /* BPL_PROTOCOL__COBS_HPP_ */
