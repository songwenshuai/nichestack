/**
 * @file cpu_endian.c
 * @brief Byte order conversion
 *
 * @section License
 *
 * Copyright (C) 2010-2019 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneTCP Eval.
 *
 * This software is provided in source form for a short-term evaluation only. The
 * evaluation license expires 90 days after the date you first download the software.
 *
 * If you plan to use this software in a commercial product, you are required to
 * purchase a commercial license from Oryx Embedded SARL.
 *
 * After the 90-day evaluation period, you agree to either purchase a commercial
 * license or delete all copies of this software. If you wish to extend the
 * evaluation period, you must contact sales@oryx-embedded.com.
 *
 * This evaluation software is provided "as is" without warranty of any kind.
 * Technical support is available as an option during the evaluation period.
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.9.4
 **/

//Dependencies
#include "cpu_endian.h"


/**
 * @brief Reverse the byte order of a 16-bit word
 * @param[in] value 16-bit value
 * @return 16-bit value with byte order swapped
 **/

uint16_t swapInt16(uint16_t value)
{
   return SWAPINT16(value);
}


/**
 * @brief Reverse the byte order of a 32-bit word
 * @param[in] value 32-bit value
 * @return 32-bit value with byte order swapped
 **/

uint32_t swapInt32(uint32_t value)
{
   return SWAPINT32(value);
}


/**
 * @brief Reverse the byte order of a 64-bit word
 * @param[in] value 64-bit value
 * @return 64-bit value with byte order swapped
 **/

uint64_t swapInt64(uint64_t value)
{
   return SWAPINT64(value);
}


/**
 * @brief Reverse bit order in a 4-bit word
 * @param[in] value 4-bit value
 * @return 4-bit value with bit order reversed
 **/

uint8_t reverseInt4(uint8_t value)
{
   value = ((value & 0x0C) >> 2) | ((value & 0x03) << 2);
   value = ((value & 0x0A) >> 1) | ((value & 0x05) << 1);

   return value;
}


/**
 * @brief Reverse bit order in a byte
 * @param[in] value 8-bit value
 * @return 8-bit value with bit order reversed
 **/

uint8_t reverseInt8(uint8_t value)
{
   value = ((value & 0xF0) >> 4) | ((value & 0x0F) << 4);
   value = ((value & 0xCC) >> 2) | ((value & 0x33) << 2);
   value = ((value & 0xAA) >> 1) | ((value & 0x55) << 1);

   return value;
}


/**
 * @brief Reverse bit order in a 16-bit word
 * @param[in] value 16-bit value
 * @return 16-bit value with bit order reversed
 **/

uint16_t reverseInt16(uint16_t value)
{
   value = ((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8);
   value = ((value & 0xF0F0) >> 4) | ((value & 0x0F0F) << 4);
   value = ((value & 0xCCCC) >> 2) | ((value & 0x3333) << 2);
   value = ((value & 0xAAAA) >> 1) | ((value & 0x5555) << 1);

   return value;
}


/**
 * @brief Reverse bit order in a 32-bit word
 * @param[in] value 32-bit value
 * @return 32-bit value with bit order reversed
 **/

uint32_t reverseInt32(uint32_t value)
{
   value = ((value & 0xFFFF0000UL) >> 16) | ((value & 0x0000FFFFUL) << 16);
   value = ((value & 0xFF00FF00UL) >> 8) | ((value & 0x00FF00FFUL) << 8);
   value = ((value & 0xF0F0F0F0UL) >> 4) | ((value & 0x0F0F0F0FUL) << 4);
   value = ((value & 0xCCCCCCCCUL) >> 2) | ((value & 0x33333333UL) << 2);
   value = ((value & 0xAAAAAAAAUL) >> 1) | ((value & 0x55555555UL) << 1);

   return value;
}


/**
 * @brief Reverse bit order in a 64-bit word
 * @param[in] value 64-bit value
 * @return 64-bit value with bit order reversed
 **/

uint64_t reverseInt64(uint64_t value)
{
   value = ((value & 0xFFFFFFFF00000000ULL) >> 32) | ((value & 0x00000000FFFFFFFFULL) << 32);
   value = ((value & 0xFFFF0000FFFF0000ULL) >> 16) | ((value & 0x0000FFFF0000FFFFULL) << 16);
   value = ((value & 0xFF00FF00FF00FF00ULL) >> 8) | ((value & 0x00FF00FF00FF00FFULL) << 8);
   value = ((value & 0xF0F0F0F0F0F0F0F0ULL) >> 4) | ((value & 0x0F0F0F0F0F0F0F0FULL) << 4);
   value = ((value & 0xCCCCCCCCCCCCCCCCULL) >> 2) | ((value & 0x3333333333333333ULL) << 2);
   value = ((value & 0xAAAAAAAAAAAAAAAAULL) >> 1) | ((value & 0x5555555555555555ULL) << 1);

   return value;
}
