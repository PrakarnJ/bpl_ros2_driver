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

#include <algorithm>          // for std::for_each
#include <boost/crc.hpp>      // for boost::crc_basic, boost::crc_optimal
#include <boost/cstdint.hpp>  // for boost::uint16_t
#include <cassert>            // for assert
#include <cstddef>            // for std::size_t
#include <iostream>           // for std::cout
#include <ostream>            // for std::endl

// Main function
int main()
{
  // This is "123456789" in ASCII
  unsigned char const data[] = {0x00, 0x00, 0x00, 0x03, 0x05, 0x08};
  std::size_t const data_len = sizeof(data) / sizeof(data[0]);

  // The expected CRC for the given data
  boost::uint16_t const expected = 0x5D;

  // Repeat with the optimal version (assuming a 16-bit type exists)
  boost::crc_optimal<8, 0x4D, 0x00, 0xFF, true, true> crc_ccitt2;
  crc_ccitt2 = std::for_each(data, data + data_len, crc_ccitt2);
  assert(crc_ccitt2() == expected);

  std::cout << "All tests passed." << std::endl;
  return 0;
}