// bdepu_typesparser.cpp                                              -*-C++-*-
#include <bdepu_typesparser.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdepu_typesparser_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_platformutil.h>

namespace BloombergLP {

enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = 1 };

                        // ------------------------
                        // struct bdepu_TypesParser
                        // ------------------------

// CLASS METHODS

                     // *** GENERATING FUNCTIONS ***

#define GEN_FUNC_ELEM(BASE) generate##BASE
#define GEN_FUNC_ARRAY(BASE) generate##BASE##ArrayRaw

#define GEN_ARRAY_RAW(FUNC_BASE, ELEM_TYPE)                                   \
void bdepu_TypesParser::GEN_FUNC_ARRAY(FUNC_BASE)(                            \
                           bsl::vector<char>             *buffer,             \
                           const bsl::vector<ELEM_TYPE>&  value,              \
                           int                            level,              \
                           int                            spacesPerLevel)     \
{                                                                             \
    BSLS_ASSERT(0 != buffer);                                                 \
                                                                              \
    if (level >= 0) {                                                         \
        bdepu_ParserImpUtil::generateIndentation(buffer,                      \
                                                 level,                       \
                                                 spacesPerLevel);             \
    }                                                                         \
    else {                                                                    \
        level *= -1;                                                          \
    }                                                                         \
                                                                              \
    buffer->push_back('[');                                                   \
                                                                              \
    const int len = value.size();                                             \
    const int elemLevel = level + 1;                                          \
    for (int i = 0; i < len; ++i) {                                           \
        buffer->push_back('\n');                                              \
        bdepu_ParserImpUtil::generateIndentation(buffer,                      \
                                                 elemLevel,                   \
                                                 spacesPerLevel);             \
        bdepu_TypesParserImpUtil::GEN_FUNC_ELEM(FUNC_BASE)(buffer, value[i]); \
    }                                                                         \
                                                                              \
    buffer->push_back('\n');                                                  \
    bdepu_ParserImpUtil::generateIndentation(buffer, level, spacesPerLevel);  \
    buffer->push_back(']');                                                   \
}

GEN_ARRAY_RAW(Char, char)
GEN_ARRAY_RAW(Date, bdet_Date)
GEN_ARRAY_RAW(DateTz, bdet_DateTz)
GEN_ARRAY_RAW(Datetime, bdet_Datetime)
GEN_ARRAY_RAW(DatetimeTz, bdet_DatetimeTz)
GEN_ARRAY_RAW(Double, double)
GEN_ARRAY_RAW(Float, float)
GEN_ARRAY_RAW(Int, int)
GEN_ARRAY_RAW(Int64, bsls_PlatformUtil::Int64)
GEN_ARRAY_RAW(Short, short)
GEN_ARRAY_RAW(String, bsl::string)
GEN_ARRAY_RAW(Time, bdet_Time)
GEN_ARRAY_RAW(TimeTz, bdet_TimeTz)

                     // *** PARSING FUNCTIONS ***

int bdepu_TypesParser::parseIntegerLocator(const char **endPos,
                                           int         *result,
                                           const char  *inputString)
{
    BSLS_ASSERT(0 != endPos);
    BSLS_ASSERT(0 != result);
    BSLS_ASSERT(0 != inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if ('{' != *inputString) {
        *endPos = inputString;
        return BDEPU_FAILURE;
    }
    ++inputString;

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    bsls_PlatformUtil::Uint64 tmpResult;
    if (bdepu_ParserImpUtil::parseUnsignedInteger(endPos,
                                                  &tmpResult,
                                                  inputString,
                                                  10,
                                                  0xFFFFFFFFuLL)) {
        *endPos = inputString;
        return BDEPU_FAILURE;
    }

    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    if ('}' != **endPos) {
        return BDEPU_FAILURE;
    }

    (*endPos)++;
    *result = (int)tmpResult;
    return BDEPU_SUCCESS;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
