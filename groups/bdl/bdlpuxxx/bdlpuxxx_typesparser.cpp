// bdlpuxxx_typesparser.cpp                                           -*-C++-*-
#include <bdlpuxxx_typesparser.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlpuxxx_typesparser_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_types.h>

namespace BloombergLP {

enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = 1 };

                        // ------------------------
                        // struct bdlpuxxx::TypesParser
                        // ------------------------

// CLASS METHODS

                     // *** GENERATING FUNCTIONS ***

#define GEN_FUNC_ELEM(BASE) generate##BASE
#define GEN_FUNC_ARRAY(BASE) generate##BASE##ArrayRaw

#define GEN_ARRAY_RAW(FUNC_BASE, ELEM_TYPE)                                   \
void bdlpuxxx::TypesParser::GEN_FUNC_ARRAY(FUNC_BASE)(                            \
                           bsl::vector<char>             *buffer,             \
                           const bsl::vector<ELEM_TYPE>&  value,              \
                           int                            level,              \
                           int                            spacesPerLevel)     \
{                                                                             \
    BSLS_ASSERT(0 != buffer);                                                 \
                                                                              \
    if (level >= 0) {                                                         \
        bdlpuxxx::ParserImpUtil::generateIndentation(buffer,                      \
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
        bdlpuxxx::ParserImpUtil::generateIndentation(buffer,                      \
                                                 elemLevel,                   \
                                                 spacesPerLevel);             \
        bdlpuxxx::TypesParserImpUtil::GEN_FUNC_ELEM(FUNC_BASE)(buffer, value[i]); \
    }                                                                         \
                                                                              \
    buffer->push_back('\n');                                                  \
    bdlpuxxx::ParserImpUtil::generateIndentation(buffer, level, spacesPerLevel);  \
    buffer->push_back(']');                                                   \
}

namespace bdlpuxxx {
GEN_ARRAY_RAW(Char, char)
GEN_ARRAY_RAW(Date, bdlt::Date)
GEN_ARRAY_RAW(DateTz, bdlt::DateTz)
GEN_ARRAY_RAW(Datetime, bdlt::Datetime)
GEN_ARRAY_RAW(DatetimeTz, bdlt::DatetimeTz)
GEN_ARRAY_RAW(Double, double)
GEN_ARRAY_RAW(Float, float)
GEN_ARRAY_RAW(Int, int)
GEN_ARRAY_RAW(Int64, bsls::Types::Int64)
GEN_ARRAY_RAW(Short, short)
GEN_ARRAY_RAW(String, bsl::string)
GEN_ARRAY_RAW(Time, bdlt::Time)
GEN_ARRAY_RAW(TimeTz, bdlt::TimeTz)

                     // *** PARSING FUNCTIONS ***

int TypesParser::parseIntegerLocator(const char **endPos,
                                           int         *result,
                                           const char  *inputString)
{
    BSLS_ASSERT(0 != endPos);
    BSLS_ASSERT(0 != result);
    BSLS_ASSERT(0 != inputString);

    ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if ('{' != *inputString) {
        *endPos = inputString;
        return BDEPU_FAILURE;                                         // RETURN
    }
    ++inputString;

    ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    bsls::Types::Uint64 tmpResult;
    if (ParserImpUtil::parseUnsignedInteger(endPos,
                                                  &tmpResult,
                                                  inputString,
                                                  10,
                                                  0xFFFFFFFFuLL)) {
        *endPos = inputString;
        return BDEPU_FAILURE;                                         // RETURN
    }

    ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    if ('}' != **endPos) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    (*endPos)++;
    *result = (int)tmpResult;
    return BDEPU_SUCCESS;
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
