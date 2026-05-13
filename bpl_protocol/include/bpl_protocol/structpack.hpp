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

#ifndef FADAB282_454E_453E_96BB_B2806D121A07
#define FADAB282_454E_453E_96BB_B2806D121A07
#ifndef BPL_PROTOCOL__STRUCT_HPP_
#define BPL_PROTOCOL__STRUCT_HPP_

#include <stdarg.h>
#include <stdint.h>

namespace bpl_protocol
{
class StructPack
{
  uint8_t * buffer;
  uint32_t pos, maxLen;

  const char * format;
  va_list arg;

public:
  StructPack(const char * format, va_list inArg);
  StructPack(const char * format, va_list inArg, void * buffer, uint32_t maxLen);
  ~StructPack();

  int doPack();
  int doUnpack();

  static int pack(void * buffer, uint32_t maxLen, const char * format, ...);
  static int unpack(const void * buffer, uint32_t maxLen, const char * format, ...);

private:
  void pack_uint8(uint8_t val);
  void pack_uint32(uint32_t val);
  void pack_float(float val);
  void pack_string(const char * str, int len, int width);
  void emit(uint8_t byte);

  void unpack_uint8(uint8_t * val);
  void unpack_uint32(uint32_t * val);
  void unpack_float(float * val);
  void unpack_string(char * str, int len, int width);
  uint8_t fetch();
};

}  // namespace bpl_protocol

#endif /* BPL_PROTOCOL__STRUCT_HPP_ */

#endif /* FADAB282_454E_453E_96BB_B2806D121A07 */
