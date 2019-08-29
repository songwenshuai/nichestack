/**
 * @file str.h
 * @brief String manipulation helper functions
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

#ifndef _STR_H
#define _STR_H

//Dependencies
#include "os_port.h"
#include "error.h"

//C++ guard
#ifdef __cplusplus
   extern "C" {
#endif

//String manipulation helper functions
char_t *strDuplicate(const char_t *s);
char_t *strTrimWhitespace(char_t *s);
void strRemoveTrailingSpace(char_t *s);
void strReplaceChar(char_t *s, char_t oldChar, char_t newChar);

error_t strSafeCopy(char_t *dest, const char_t *src, size_t destSize);

//C++ guard
#ifdef __cplusplus
   }
#endif

#endif
