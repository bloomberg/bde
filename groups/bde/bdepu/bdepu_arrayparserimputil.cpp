// bdepu_arrayparserimputil.cpp                                       -*-C++-*-
#include <bdepu_arrayparserimputil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdepu_arrayparserimputil_cpp,"$Id$ $CSID$")

#include <bdepu_parserimputil.h>
#include <bdepu_realparserimputil.h>      // for testing only
#include <bdepu_typesparserimputil.h>

#include <bsls_assert.h>

namespace BloombergLP {

enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = 1 };

                          // ------------------------
                          // bdepu_ArrayParserImpUtil
                          // ------------------------

// CLASS METHODS

                     // *** GENERATING FUNCTIONS ***

#define GEN_FUNC_ELEM(BASE) generate##BASE
#define GEN_FUNC_ARRAY(BASE) generate##BASE##Array

#define GEN_ARRAY(FUNC_NAME, ELEM_TYPE)                                       \
void bdepu_ArrayParserImpUtil::                                               \
         GEN_FUNC_ARRAY(FUNC_NAME)(bsl::vector<char>             *buffer,     \
                                   const bsl::vector<ELEM_TYPE>&  value)      \
{                                                                             \
    BSLS_ASSERT(0 != buffer);                                              \
                                                                              \
    const int len = value.size();                                             \
    buffer->push_back('[');                                                   \
    buffer->push_back(' ');                                                   \
    for (int i = 0; i < len; ++i) {                                           \
        bdepu_TypesParserImpUtil::GEN_FUNC_ELEM(FUNC_NAME)(buffer, value[i]); \
        buffer->push_back(' ');                                               \
    }                                                                         \
    buffer->push_back(']');                                                   \
}

GEN_ARRAY(Char, char)
GEN_ARRAY(Date, bdet_Date)
GEN_ARRAY(Datetime, bdet_Datetime)
GEN_ARRAY(DatetimeTz, bdet_DatetimeTz)
GEN_ARRAY(DateTz, bdet_DateTz)
GEN_ARRAY(Double, double)
GEN_ARRAY(Float, float)
GEN_ARRAY(Int, int)
GEN_ARRAY(Int64, bsls_PlatformUtil::Int64)
GEN_ARRAY(Short, short)
GEN_ARRAY(String, bsl::string)
GEN_ARRAY(Time, bdet_Time)
GEN_ARRAY(TimeTz, bdet_TimeTz)

                     // *** PARSING FUNCTIONS ***

#define PARSE_FUNC_ELEM(BASE) parse##BASE
#define PARSE_FUNC_ARRAY(BASE) parse##BASE##Array

#define PARSE_ARRAY(FUNC_NAME, ELEM_TYPE)                                     \
int bdepu_ArrayParserImpUtil::PARSE_FUNC_ARRAY(FUNC_NAME)(                    \
                                      const char             **endPos,        \
                                      bsl::vector<ELEM_TYPE>  *result,        \
                                      const char              *inputString)   \
{                                                                             \
    BSLS_ASSERT(0 != endPos);                                                 \
    BSLS_ASSERT(0 != result);                                                 \
    BSLS_ASSERT(0 != inputString);                                            \
                                                                              \
    result->clear();                                                          \
                                                                              \
    if ('[' != *inputString) {                                                \
        *endPos = inputString;                                                \
        return BDEPU_FAILURE;                                                 \
    }                                                                         \
    ++inputString;                                                            \
                                                                              \
    bdepu_ParserImpUtil::skipWhiteSpace(endPos, inputString);                 \
                                                                              \
    ELEM_TYPE value;                                                          \
    while (']' != **endPos) {                                                 \
        if (bdepu_TypesParserImpUtil::PARSE_FUNC_ELEM(FUNC_NAME)(             \
                                                       endPos,                \
                                                       &value,                \
                                                       *endPos)) {            \
            return BDEPU_FAILURE;                                             \
        }                                                                     \
        result->push_back(value);                                             \
        inputString = *endPos;                                                \
        bdepu_ParserImpUtil::skipWhiteSpace(endPos, inputString);             \
        if (inputString == *endPos && ']' != **endPos) {                      \
            return BDEPU_FAILURE;                                             \
        }                                                                     \
    }                                                                         \
    ++*endPos;                                                                \
    return BDEPU_SUCCESS;                                                     \
}

PARSE_ARRAY(Char, char)
PARSE_ARRAY(Date, bdet_Date)
PARSE_ARRAY(Datetime, bdet_Datetime)
PARSE_ARRAY(DatetimeTz, bdet_DatetimeTz)
PARSE_ARRAY(DateTz, bdet_DateTz)
PARSE_ARRAY(Double, double)
PARSE_ARRAY(Float, float)
PARSE_ARRAY(Int, int)
PARSE_ARRAY(Int64, bsls_PlatformUtil::Int64)
PARSE_ARRAY(Short, short)
PARSE_ARRAY(String, bsl::string)
PARSE_ARRAY(Time, bdet_Time)
PARSE_ARRAY(TimeTz, bdet_TimeTz)

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
