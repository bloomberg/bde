// bdlpuxxx_arrayparserimputil.cpp                                    -*-C++-*-
#include <bdlpuxxx_arrayparserimputil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlpuxxx_arrayparserimputil_cpp,"$Id$ $CSID$")

#include <bdlpuxxx_parserimputil.h>
#include <bdlpuxxx_realparserimputil.h>      // for testing only
#include <bdlpuxxx_typesparserimputil.h>

#include <bsls_assert.h>

namespace BloombergLP {

enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = 1 };

                          // ------------------------
                          // bdlpuxxx::ArrayParserImpUtil
                          // ------------------------

// CLASS METHODS

                     // *** GENERATING FUNCTIONS ***

#define GEN_FUNC_ELEM(BASE) generate##BASE
#define GEN_FUNC_ARRAY(BASE) generate##BASE##Array

#define GEN_ARRAY(FUNC_NAME, ELEM_TYPE)                                       \
void bdlpuxxx::ArrayParserImpUtil::                                               \
         GEN_FUNC_ARRAY(FUNC_NAME)(bsl::vector<char>             *buffer,     \
                                   const bsl::vector<ELEM_TYPE>&  value)      \
{                                                                             \
    BSLS_ASSERT(0 != buffer);                                              \
                                                                              \
    const int len = value.size();                                             \
    buffer->push_back('[');                                                   \
    buffer->push_back(' ');                                                   \
    for (int i = 0; i < len; ++i) {                                           \
        bdlpuxxx::TypesParserImpUtil::GEN_FUNC_ELEM(FUNC_NAME)(buffer, value[i]); \
        buffer->push_back(' ');                                               \
    }                                                                         \
    buffer->push_back(']');                                                   \
}

GEN_ARRAY(Char, char)
GEN_ARRAY(Date, bdlt::Date)
GEN_ARRAY(Datetime, bdlt::Datetime)
GEN_ARRAY(DatetimeTz, bdlt::DatetimeTz)
GEN_ARRAY(DateTz, bdlt::DateTz)
GEN_ARRAY(Double, double)
GEN_ARRAY(Float, float)
GEN_ARRAY(Int, int)
GEN_ARRAY(Int64, bsls::Types::Int64)
GEN_ARRAY(Short, short)
GEN_ARRAY(String, bsl::string)
GEN_ARRAY(Time, bdlt::Time)
GEN_ARRAY(TimeTz, bdlt::TimeTz)

                     // *** PARSING FUNCTIONS ***

#define PARSE_FUNC_ELEM(BASE) parse##BASE
#define PARSE_FUNC_ARRAY(BASE) parse##BASE##Array

#define PARSE_ARRAY(FUNC_NAME, ELEM_TYPE)                                     \
int bdlpuxxx::ArrayParserImpUtil::PARSE_FUNC_ARRAY(FUNC_NAME)(                    \
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
    bdlpuxxx::ParserImpUtil::skipWhiteSpace(endPos, inputString);                 \
                                                                              \
    ELEM_TYPE value;                                                          \
    while (']' != **endPos) {                                                 \
        if (bdlpuxxx::TypesParserImpUtil::PARSE_FUNC_ELEM(FUNC_NAME)(             \
                                                       endPos,                \
                                                       &value,                \
                                                       *endPos)) {            \
            return BDEPU_FAILURE;                                             \
        }                                                                     \
        result->push_back(value);                                             \
        inputString = *endPos;                                                \
        bdlpuxxx::ParserImpUtil::skipWhiteSpace(endPos, inputString);             \
        if (inputString == *endPos && ']' != **endPos) {                      \
            return BDEPU_FAILURE;                                             \
        }                                                                     \
    }                                                                         \
    ++*endPos;                                                                \
    return BDEPU_SUCCESS;                                                     \
}

PARSE_ARRAY(Char, char)                                               // RETURN
PARSE_ARRAY(Date, bdlt::Date)                                         // RETURN
PARSE_ARRAY(Datetime, bdlt::Datetime)                                 // RETURN
PARSE_ARRAY(DatetimeTz, bdlt::DatetimeTz)                             // RETURN
PARSE_ARRAY(DateTz, bdlt::DateTz)                                     // RETURN
PARSE_ARRAY(Double, double)                                           // RETURN
PARSE_ARRAY(Float, float)                                             // RETURN
PARSE_ARRAY(Int, int)                                                 // RETURN
PARSE_ARRAY(Int64, bsls::Types::Int64)                                // RETURN
PARSE_ARRAY(Short, short)                                             // RETURN
PARSE_ARRAY(String, bsl::string)                                      // RETURN
PARSE_ARRAY(Time, bdlt::Time)                                         // RETURN
PARSE_ARRAY(TimeTz, bdlt::TimeTz)                                     // RETURN

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
