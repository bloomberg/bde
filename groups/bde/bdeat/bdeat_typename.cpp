// bdeat_typename.cpp              -*-C++-*-

#include <bdeat_typename.h>
#include <bsls_assert.h>

namespace BloombergLP {

const char bdeat_TypeName_Imp::BDEAT_NAME_BOOL[]           = "bool";
const char bdeat_TypeName_Imp::BDEAT_NAME_CHAR[]           = "char";
const char bdeat_TypeName_Imp::BDEAT_NAME_SIGNED_CHAR[]    = "signed char";
const char bdeat_TypeName_Imp::BDEAT_NAME_UNSIGNED_CHAR[]  = "unsigned char";
const char bdeat_TypeName_Imp::BDEAT_NAME_SHORT[]          = "short";
const char bdeat_TypeName_Imp::BDEAT_NAME_UNSIGNED_SHORT[] = "unsigned short";
const char bdeat_TypeName_Imp::BDEAT_NAME_INT[]            = "int";
const char bdeat_TypeName_Imp::BDEAT_NAME_UNSIGNED_INT[]   = "unsigned int";
const char bdeat_TypeName_Imp::BDEAT_NAME_LONG[]           = "long";
const char bdeat_TypeName_Imp::BDEAT_NAME_UNSIGNED_LONG[]  = "unsigned long";
const char bdeat_TypeName_Imp::BDEAT_NAME_INT64[]          = "Int64";
const char bdeat_TypeName_Imp::BDEAT_NAME_UINT64[]         = "Uint64";
const char bdeat_TypeName_Imp::BDEAT_NAME_FLOAT[]          = "float";
const char bdeat_TypeName_Imp::BDEAT_NAME_DOUBLE[]         = "double";
const char bdeat_TypeName_Imp::BDEAT_NAME_CONST_CHAR_PTR[] = "const char*";
const char bdeat_TypeName_Imp::BDEAT_NAME_CONST_SIGNED_CHAR_PTR[] =
                                                          "const signed char*";
const char bdeat_TypeName_Imp::BDEAT_NAME_CONST_UNSIGNED_CHAR_PTR[] =
                                                        "const unsigned char*";
const char bdeat_TypeName_Imp::BDEAT_NAME_STRING[]         = "string";
const char bdeat_TypeName_Imp::BDEAT_NAME_DATE[]           = "bdet_Date";
const char bdeat_TypeName_Imp::BDEAT_NAME_DATE_TZ[]        = "bdet_DateTz";
const char bdeat_TypeName_Imp::BDEAT_NAME_DATETIME[]       = "bdet_Datetime";
const char bdeat_TypeName_Imp::BDEAT_NAME_DATETIME_TZ[]    = "bdet_DatetimeTz";
const char bdeat_TypeName_Imp::BDEAT_NAME_TIME[]           = "bdet_Time";
const char bdeat_TypeName_Imp::BDEAT_NAME_TIME_TZ[]        = "bdet_TimeTz";
const char bdeat_TypeName_Imp::BDEAT_NAME_VECTOR_BEGIN[]   = "vector<";
const char bdeat_TypeName_Imp::BDEAT_NAME_VECTOR_END[]     = ">";

const char bdeat_TypeName_Imp::BDEAT_XSDNAME_BOOLEAN[]     = "boolean";
const char bdeat_TypeName_Imp::BDEAT_XSDNAME_BYTE[]        = "byte";
const char bdeat_TypeName_Imp::BDEAT_XSDNAME_UNSIGNED_BYTE[] = "unsignedByte";
const char bdeat_TypeName_Imp::BDEAT_XSDNAME_SHORT[]       = "short";
const char bdeat_TypeName_Imp::BDEAT_XSDNAME_UNSIGNED_SHORT[] =
                                                               "unsignedShort";
const char bdeat_TypeName_Imp::BDEAT_XSDNAME_INT[]         = "int";
const char bdeat_TypeName_Imp::BDEAT_XSDNAME_UNSIGNED_INT[] = "unsignedInt";
const char bdeat_TypeName_Imp::BDEAT_XSDNAME_LONG[]        = "long";
const char bdeat_TypeName_Imp::BDEAT_XSDNAME_UNSIGNED_LONG[] = "unsignedLong";
const char bdeat_TypeName_Imp::BDEAT_XSDNAME_FLOAT[]       = "float";
const char bdeat_TypeName_Imp::BDEAT_XSDNAME_DOUBLE[]      = "double";
const char bdeat_TypeName_Imp::BDEAT_XSDNAME_DECIMAL[]     = "decimal";
const char bdeat_TypeName_Imp::BDEAT_XSDNAME_STRING[]      = "string";
const char bdeat_TypeName_Imp::BDEAT_XSDNAME_BASE64_BINARY[] = "base64Binary";
const char bdeat_TypeName_Imp::BDEAT_XSDNAME_HEX_BINARY[]  = "hexBinary";
const char bdeat_TypeName_Imp::BDEAT_XSDNAME_DATE[]        = "date";
const char bdeat_TypeName_Imp::BDEAT_XSDNAME_DATETIME[]    = "dateTime";
const char bdeat_TypeName_Imp::BDEAT_XSDNAME_TIME[]        = "time";
const char bdeat_TypeName_Imp::BDEAT_XSDNAME_ANY_TYPE[]    = "anyType";

const char*
bdeat_TypeName_Imp::xsdName(const signed char *, int format)
{
    switch (format & FMode::BDEAT_TYPE_MASK) {
      case FMode::BDEAT_DEFAULT:
      case FMode::BDEAT_DEC:
        return BDEAT_XSDNAME_BYTE;

      case FMode::BDEAT_TEXT:
        return BDEAT_XSDNAME_STRING;  // UTF-8 character

      default:
        BSLS_ASSERT(0 && "Invalid format for char");

        return 0;
    }
}

const char*
bdeat_TypeName_Imp::xsdName(const unsigned char *, int format)
{
    switch (format & FMode::BDEAT_TYPE_MASK) {
      case FMode::BDEAT_DEFAULT:
      case FMode::BDEAT_DEC:
        return BDEAT_XSDNAME_UNSIGNED_BYTE;

      case FMode::BDEAT_TEXT:
        return BDEAT_XSDNAME_STRING;  // UTF-8 character

      default:
        BSLS_ASSERT(0 && "Invalid format for char");

        return 0;
    }
}

const char*
bdeat_TypeName_Imp::xsdName(const short *, int format)
{
    switch (format & FMode::BDEAT_TYPE_MASK) {
      case FMode::BDEAT_DEFAULT:
      case FMode::BDEAT_DEC:
        return BDEAT_XSDNAME_SHORT;

      case FMode::BDEAT_TEXT:
        return BDEAT_XSDNAME_STRING;  // UTF-16 character

      default:
        BSLS_ASSERT(0 && "Invalid format for short");

        return 0;
    }
}

const char*
bdeat_TypeName_Imp::xsdName(const float *, int format)
{
    switch (format & FMode::BDEAT_TYPE_MASK) {
      case FMode::BDEAT_DEFAULT:
        return BDEAT_XSDNAME_FLOAT;

      case FMode::BDEAT_DEC:
        return BDEAT_XSDNAME_DECIMAL;

      default:
        BSLS_ASSERT(0 && "Invalid format for float");

        return 0;
    }
}

const char*
bdeat_TypeName_Imp::xsdName(const double *, int format)
{
    switch (format & FMode::BDEAT_TYPE_MASK) {
      case FMode::BDEAT_DEFAULT:
        return BDEAT_XSDNAME_DOUBLE;

      case FMode::BDEAT_DEC:
        return BDEAT_XSDNAME_DECIMAL;

      default:
        BSLS_ASSERT(0 && "Invalid format for double");

        return 0;
    }
}

const char*
bdeat_TypeName_Imp::xsdName(const bsl::string *, int format)
{
    switch (format & FMode::BDEAT_TYPE_MASK) {
      case FMode::BDEAT_DEFAULT:
      case FMode::BDEAT_TEXT:
        return BDEAT_XSDNAME_STRING;

      case FMode::BDEAT_BASE64:
        return BDEAT_XSDNAME_BASE64_BINARY;

      case FMode::BDEAT_HEX:
        return BDEAT_XSDNAME_HEX_BINARY;

      default:
        BSLS_ASSERT(0 && "Invalid format for string");
        return 0;
    }
}

const char*
bdeat_TypeName_Imp::xsdName(const bsl::vector<char> *, int format)
{
    switch (format & FMode::BDEAT_TYPE_MASK) {
      case FMode::BDEAT_DEFAULT:
      case FMode::BDEAT_BASE64:
        return BDEAT_XSDNAME_BASE64_BINARY;

      case FMode::BDEAT_HEX:
        return BDEAT_XSDNAME_HEX_BINARY;

      case FMode::BDEAT_TEXT:
        return BDEAT_XSDNAME_STRING;  // UTF-8 string

      default:
        BSLS_ASSERT(0 && "Invalid format for vector<char>");
        return 0;
    }
}

const char*
bdeat_TypeName_Imp::xsdName(const bsl::vector<short> *, int format)
{
    switch (format & FMode::BDEAT_TYPE_MASK) {
      case FMode::BDEAT_TEXT:
        return BDEAT_XSDNAME_STRING;  // UTF-16 string

      default:
        BSLS_ASSERT(0 && "Invalid format for vector<short>");

        return 0;
    }
}

bool bdeat_TypeName_Imp::idempotentConcat(char       *dest,
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

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
