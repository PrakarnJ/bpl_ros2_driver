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

#include <evpp/udp/sync_udp_client.h>
#include <evpp/udp/udp_server.h>

#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char * argv[])
{
  static uint64_t g_timeout_ms = 1000;
  int port = 6789;
  evpp::udp::sync::Client c;
  std::vector<uint8_t> vec1{0x09, 0x9A, 0x99, 0x99, 0x3E, 0x03, 0x02, 0x08, 0x8E, 0x00};
  std::string req1(vec1.begin(), vec1.end());

  std::vector<uint8_t> vec2{0x09, 0xCD, 0xCC, 0xCC, 0x3D, 0x03, 0x02, 0x08, 0xE3, 0x00};
  std::string req2(vec2.begin(), vec2.end());

  std::string resp1 = c.DoRequest("192.168.2.3", port, req1, g_timeout_ms);
  cout << resp1 << endl;
  sleep(2);
  std::string resp2 = c.DoRequest("192.168.2.3", port, req2, g_timeout_ms);
  cout << resp2 << endl;
  return 0;
}