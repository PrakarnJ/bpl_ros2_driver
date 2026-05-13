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
#include <time.h>

#include <cstdint>
#include <iostream>

#include "bpl_protocol/cobs.hpp"

using namespace bpl_protocol;

int main(int argc, char * argv[])
{
  ByteArray raw_data = ByteArray({0x00, 0x00, 0x00, 0x03, 0x05, 0x08, 0xaf});
  ByteArray encoded_data;
  Cobs::encode(raw_data, encoded_data);
  if (ByteArray({0x01, 0x01, 0x01, 0x05, 0x03, 0x05, 0x08, 0xaf, 0x00}) == encoded_data) {
    std::cout << "encoded matched" << std::endl;
  } else {
    std::cout << "encoded does't matched" << std::endl;
  }
  ByteArray decoded_data;
  Cobs::decode(encoded_data, decoded_data);
  if (raw_data == decoded_data) {
    std::cout << "decoded matched" << std::endl;
  } else {
    std::cout << "decoded does't matched" << std::endl;
  }
  //  Cobs::print_vector(encoded_data);

  return 0;
}