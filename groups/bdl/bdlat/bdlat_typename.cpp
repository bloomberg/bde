// bdlat_typename.cpp                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlat_typename.h>
#include <bsls_assert.h>

namespace BloombergLP {

const char bdlat_TypeName_Imp::BDLAT_NAME_BOOL[]           = "bool";
const char bdlat_TypeName_Imp::BDLAT_NAME_CHAR[]           = "char";
const char bdlat_TypeName_Imp::BDLAT_NAME_SIGNED_CHAR[]    = "signed char";
const char bdlat_TypeName_Imp::BDLAT_NAME_UNSIGNED_CHAR[]  = "unsigned char";
const char bdlat_TypeName_Imp::BDLAT_NAME_SHORT[]          = "short";
const char bdlat_TypeName_Imp::BDLAT_NAME_UNSIGNED_SHORT[] = "unsigned short";
const char bdlat_TypeName_Imp::BDLAT_NAME_INT[]            = "int";
const char bdlat_TypeName_Imp::BDLAT_NAME_UNSIGNED_INT[]   = "unsigned int";
const char bdlat_TypeName_Imp::BDLAT_NAME_LONG[]           = "long";
const char bdlat_TypeName_Imp::BDLAT_NAME_UNSIGNED_LONG[]  = "unsigned long";
const char bdlat_TypeName_Imp::BDLAT_NAME_INT64[]          = "Int64";
const char bdlat_TypeName_Imp::BDLAT_NAME_UINT64[]         = "Uint64";
const char bdlat_TypeName_Imp::BDLAT_NAME_FLOAT[]          = "float";
const char bdlat_TypeName_Imp::BDLAT_NAME_DOUBLE[]         = "double";
const char bdlat_TypeName_Imp::BDLAT_NAME_CONST_CHAR_PTR[] = "const char*";
const char bdlat_TypeName_Imp::BDLAT_NAME_CONST_SIGNED_CHAR_PTR[] =
                                                          "const signed char*";
const char bdlat_TypeName_Imp::BDLAT_NAME_CONST_UNSIGNED_CHAR_PTR[] =
                                                        "const unsigned char*";
const char bdlat_TypeName_Imp::BDLAT_NAME_STRING[]         = "string";
const char bdlat_TypeName_Imp::BDLAT_NAME_DATE[]           = "bdlt::Date";
const char bdlat_TypeName_Imp::BDLAT_NAME_DATE_TZ[]        = "bdlt::DateTz";
const char bdlat_TypeName_Imp::BDLAT_NAME_DATETIME[]       = "bdlt::Datetime";
const char bdlat_TypeName_Imp::BDLAT_NAME_DATETIME_TZ[]    =
                                                            "bdlt::DatetimeTz";
const char bdlat_TypeName_Imp::BDLAT_NAME_TIME[]           = "bdlt::Time";
const char bdlat_TypeName_Imp::BDLAT_NAME_TIME_TZ[]        = "bdlt::TimeTz";
const char bdlat_TypeName_Imp::BDLAT_NAME_VECTOR_BEGIN[]   = "vector<";
const char bdlat_TypeName_Imp::BDLAT_NAME_VECTOR_END[]     = ">";

const char bdlat_TypeName_Imp::BDLAT_XSDNAME_BOOLEAN[]     = "boolean";
const char bdlat_TypeName_Imp::BDLAT_XSDNAME_BYTE[]        = "byte";
const char bdlat_TypeName_Imp::BDLAT_XSDNAME_UNSIGNED_BYTE[] = "unsignedByte";
const char bdlat_TypeName_Imp::BDLAT_XSDNAME_SHORT[]       = "short";
const char bdlat_TypeName_Imp::BDLAT_XSDNAME_UNSIGNED_SHORT[] =
                                                               "unsignedShort";
const char bdlat_TypeName_Imp::BDLAT_XSDNAME_INT[]         = "int";
const char bdlat_TypeName_Imp::BDLAT_XSDNAME_UNSIGNED_INT[] = "unsignedInt";
const char bdlat_TypeName_Imp::BDLAT_XSDNAME_LONG[]        = "long";
const char bdlat_TypeName_Imp::BDLAT_XSDNAME_UNSIGNED_LONG[] = "unsignedLong";
const char bdlat_TypeName_Imp::BDLAT_XSDNAME_FLOAT[]       = "float";
const char bdlat_TypeName_Imp::BDLAT_XSDNAME_DOUBLE[]      = "double";
const char bdlat_TypeName_Imp::BDLAT_XSDNAME_DECIMAL[]     = "decimal";
const char bdlat_TypeName_Imp::BDLAT_XSDNAME_STRING[]      = "string";
const char bdlat_TypeName_Imp::BDLAT_XSDNAME_BASE64_BINARY[] = "base64Binary";
const char bdlat_TypeName_Imp::BDLAT_XSDNAME_HEX_BINARY[]  = "hexBinary";
const char bdlat_TypeName_Imp::BDLAT_XSDNAME_DATE[]        = "date";
const char bdlat_TypeName_Imp::BDLAT_XSDNAME_DATETIME[]    = "dateTime";
const char bdlat_TypeName_Imp::BDLAT_XSDNAME_TIME[]        = "time";
const char bdlat_TypeName_Imp::BDLAT_XSDNAME_ANY_TYPE[]    = "anyType";

const char*
bdlat_TypeName_Imp::xsdName(const signed char *, int format)
{
    switch (format & FMode::e_TYPE_MASK) {
      case FMode::e_DEFAULT:
      case FMode::e_DEC:
        return BDLAT_XSDNAME_BYTE;                                    // RETURN

      case FMode::e_TEXT:
        return BDLAT_XSDNAME_STRING;  // UTF-8 character              // RETURN

      default:
        BSLS_ASSERT(0 && "Invalid format for char");

        return 0;                                                     // RETURN
    }
}

const char*
bdlat_TypeName_Imp::xsdName(const unsigned char *, int format)
{
    switch (format & FMode::e_TYPE_MASK) {
      case FMode::e_DEFAULT:
      case FMode::e_DEC:
        return BDLAT_XSDNAME_UNSIGNED_BYTE;                           // RETURN

      case FMode::e_TEXT:
        return BDLAT_XSDNAME_STRING;  // UTF-8 character              // RETURN

      default:
        BSLS_ASSERT(0 && "Invalid format for char");

        return 0;                                                     // RETURN
    }
}

const char*
bdlat_TypeName_Imp::xsdName(const short *, int format)
{
    switch (format & FMode::e_TYPE_MASK) {
      case FMode::e_DEFAULT:
      case FMode::e_DEC:
        return BDLAT_XSDNAME_SHORT;                                   // RETURN

      case FMode::e_TEXT:
        return BDLAT_XSDNAME_STRING;  // UTF-16 character             // RETURN

      default:
        BSLS_ASSERT(0 && "Invalid format for short");

        return 0;                                                     // RETURN
    }
}

const char*
bdlat_TypeName_Imp::xsdName(const float *, int format)
{
    switch (format & FMode::e_TYPE_MASK) {
      case FMode::e_DEFAULT:
        return BDLAT_XSDNAME_FLOAT;

      case FMode::e_DEC:
        return BDLAT_XSDNAME_DECIMAL;

      default:
        BSLS_ASSERT(0 && "Invalid format for float");

        return 0;                                                     // RETURN
    }
}

const char*
bdlat_TypeName_Imp::xsdName(const double *, int format)
{
    switch (format & FMode::e_TYPE_MASK) {
      case FMode::e_DEFAULT:
        return BDLAT_XSDNAME_DOUBLE;

      case FMode::e_DEC:
        return BDLAT_XSDNAME_DECIMAL;

      default:
        BSLS_ASSERT(0 && "Invalid format for double");

        return 0;                                                     // RETURN
    }
}

const char*
bdlat_TypeName_Imp::xsdName(const bsl::string *, int format)
{
    switch (format & FMode::e_TYPE_MASK) {
      case FMode::e_DEFAULT:
      case FMode::e_TEXT:
        return BDLAT_XSDNAME_STRING;                                  // RETURN

      case FMode::e_BASE64:
        return BDLAT_XSDNAME_BASE64_BINARY;                           // RETURN

      case FMode::e_HEX:
        return BDLAT_XSDNAME_HEX_BINARY;                              // RETURN

      default:
        BSLS_ASSERT(0 && "Invalid format for string");
        return 0;                                                     // RETURN
    }
}

const char*
bdlat_TypeName_Imp::xsdName(const bsl::vector<char> *, int format)
{
    switch (format & FMode::e_TYPE_MASK) {
      case FMode::e_DEFAULT:
      case FMode::e_BASE64:
        return BDLAT_XSDNAME_BASE64_BINARY;                           // RETURN

      case FMode::e_HEX:
        return BDLAT_XSDNAME_HEX_BINARY;                              // RETURN

      case FMode::e_TEXT:
        return BDLAT_XSDNAME_STRING;  // UTF-8 string                 // RETURN

      default:
        BSLS_ASSERT(0 && "Invalid format for vector<char>");
        return 0;                                                     // RETURN
    }
}

const char*
bdlat_TypeName_Imp::xsdName(const bsl::vector<short> *, int format)
{
    switch (format & FMode::e_TYPE_MASK) {
      case FMode::e_TEXT:
        return BDLAT_XSDNAME_STRING;  // UTF-16 string

      default:
        BSLS_ASSERT(0 && "Invalid format for vector<short>");

        return 0;                                                     // RETURN
    }
}

bool bdlat_TypeName_Imp::idempotentConcat(char       *dest,
                                          int         destSize,
                                          const char *segments[],
                                          int         numSegments)
{
    // Avoid any operation that writes a byte (including a null terminator)
    // and then overwrites that byte with a different value.  Thus, we cannot
    // use bsl::strcat, bsl::strncat, etc.

    char *p = dest;
    int remaining = destSize - 1;
    for (int i = 0; i < numSegments; ++i) {
        const char *segment = segments[i];
        int segmentLen = bsl::strlen(segment);
        if (segmentLen > remaining) {
            segmentLen = remaining;
        }
        bsl::memcpy(p, segment, segmentLen);
        remaining -= segmentLen;
        p += segmentLen;
    }

    *p = '\0'; // write only the very last null terminator

    return true;
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
