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

#include <iostream>
#include <memory>
#include <vector>

using namespace std;

int main(int argc, const char ** argv)
{
  float f = 0.8f;
  std::vector<uint8_t> arr;
  std::vector<uint8_t> v{0x00, 0x00, 0x00, 0x03, 0x05, 0x08, 0xaf};

  unsigned char const * p = reinterpret_cast<unsigned char const *>(&f);

  for (std::size_t i = 0; i != sizeof(float); ++i) {
    std::printf("The byte #%zu is 0x%02X\n", i, p[i]);
    arr.push_back(p[i]);
  }
  for (auto i : arr) cout << static_cast<int>(i) << " ";
  cout << endl;
  for (std::size_t i = 0; i != v.size(); ++i) {
    std::printf("The byte #%zu is 0x%02X\n", i, v[i]);
  }
  for (auto i : v) cout << static_cast<int>(i) << " ";
  return 0;
}