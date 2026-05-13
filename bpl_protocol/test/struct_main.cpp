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

#include <gtest/gtest.h>

#include "bpl_protocol/structpack.hpp"

using namespace bpl_protocol;
using namespace std;

#define ASSERT_EQ_ARRAY(a1, a2, len)             \
  {                                              \
    for (int i = 0; i < len; i++) {              \
      ASSERT_EQ((uint8_t)a2[i], (uint8_t)a1[i]); \
    }                                            \
  }

uint8_t buf[100];

TEST(structpack, test_bpl)
{
  float c[] = {0.8};
  int r = StructPack::pack(buf, 100, "1f", c);

  ASSERT_EQ(r, 4);
  ASSERT_EQ_ARRAY(buf, "\xcd\xccL?", r);
}

int main(int argc, char ** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}