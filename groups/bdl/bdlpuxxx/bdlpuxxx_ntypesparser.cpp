// bdlpuxxx_ntypesparser.cpp                                             -*-C++-*-
#include <bdlpuxxx_ntypesparser.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlpuxxx_ntypesparser_cpp,"$Id$ $CSID$")

#include <bdlpuxxx_typesparser.h>

#include <bsls_assert.h>

namespace BloombergLP {

enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = 1 };

namespace bdlpuxxx {
                        // -------------------------
                        // struct NTypesParser
                        // -------------------------

// CLASS METHODS

                       // *** PARSE FUNCTIONS ***

int NTypesParser::parseChar(const char **endPos,
                                  char        *result,
                                  const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdltuxxx::Unset<char>::unsetValue();
        return 0;
    }

    return TypesParserImpUtil::parseChar(endPos, result, inputString);
}

int NTypesParser::parseDate(const char **endPos,
                                  bdlt::Date   *result,
                                  const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdltuxxx::Unset<bdlt::Date>::unsetValue();
        return 0;
    }

    return TypesParserImpUtil::parseDate(endPos, result, inputString);
}

int NTypesParser::parseDatetime(const char    **endPos,
                                      bdlt::Datetime  *result,
                                      const char     *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdltuxxx::Unset<bdlt::Datetime>::unsetValue();
        return 0;
    }

    return TypesParserImpUtil::parseDatetime(endPos,
                                                   result,
                                                   inputString);
}

int NTypesParser::parseDatetimeTz(const char      **endPos,
                                        bdlt::DatetimeTz  *result,
                                        const char       *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdltuxxx::Unset<bdlt::DatetimeTz>::unsetValue();
        return 0;
    }

    return TypesParserImpUtil::parseDatetimeTz(endPos,
                                                     result,
                                                     inputString);
}

int NTypesParser::parseDateTz(const char  **endPos,
                                    bdlt::DateTz  *result,
                                    const char   *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdltuxxx::Unset<bdlt::DateTz>::unsetValue();
        return 0;
    }

    return TypesParserImpUtil::parseDateTz(endPos, result, inputString);
}

int NTypesParser::parseDouble(const char **endPos,
                                    double      *result,
                                    const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdltuxxx::Unset<double>::unsetValue();
        return 0;
    }

    return TypesParserImpUtil::parseDouble(endPos, result, inputString);
}

int NTypesParser::parseFloat(const char **endPos,
                                   float       *result,
                                   const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdltuxxx::Unset<float>::unsetValue();
        return 0;
    }

    return TypesParserImpUtil::parseFloat(endPos, result, inputString);
}

int NTypesParser::parseInt(const char **endPos,
                                 int         *result,
                                 const char  *inputString,
                                 int          base)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdltuxxx::Unset<int>::unsetValue();
        return 0;
    }

    return TypesParserImpUtil::parseInt(endPos,
                                              result,
                                              inputString,
                                              base);
}

int NTypesParser::parseInt64(const char         **endPos,
                                   bsls::Types::Int64  *result,
                                   const char          *inputString,
                                   int                  base)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdltuxxx::Unset<bsls::Types::Int64>::unsetValue();
        return 0;
    }

    return TypesParserImpUtil::parseInt64(endPos,
                                                result,
                                                inputString,
                                                base);
}

int NTypesParser::parseShort(const char **endPos,
                                   short       *result,
                                   const char  *inputString,
                                   int          base)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdltuxxx::Unset<short>::unsetValue();
        return 0;
    }

    return TypesParserImpUtil::parseShort(endPos,
                                                result,
                                                inputString,
                                                base);
}

int NTypesParser::parseString(const char  **endPos,
                                    bsl::string  *result,
                                    const char   *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdltuxxx::Unset<bsl::string>::unsetValue();
        return 0;
    }

    return TypesParserImpUtil::parseString(endPos, result, inputString);
}

int NTypesParser::parseQuotedString(const char  **endPos,
                                          bsl::string  *result,
                                          const char   *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdltuxxx::Unset<bsl::string>::unsetValue();
        return 0;
    }

    return TypesParserImpUtil::parseQuotedString(endPos,
                                                       result,
                                                       inputString);
}

int NTypesParser::parseUnquotedString(const char  **endPos,
                                            bsl::string  *result,
                                            const char   *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdltuxxx::Unset<bsl::string>::unsetValue();
        return 0;
    }

    return TypesParserImpUtil::parseUnquotedString(endPos,
                                                         result,
                                                         inputString);
}

int NTypesParser::parseSpaceDelimitedString(const char  **endPos,
                                                  bsl::string  *result,
                                                  const char   *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdltuxxx::Unset<bsl::string>::unsetValue();
        return 0;
    }

    return TypesParserImpUtil::parseSpaceDelimitedString(endPos,
                                                               result,
                                                               inputString);
}

int NTypesParser::parseTime(const char **endPos,
                                  bdlt::Time   *result,
                                  const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdltuxxx::Unset<bdlt::Time>::unsetValue();
        return 0;
    }

    return TypesParserImpUtil::parseTime(endPos, result, inputString);
}

int NTypesParser::parseTimeTz(const char  **endPos,
                                    bdlt::TimeTz  *result,
                                    const char   *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdltuxxx::Unset<bdlt::TimeTz>::unsetValue();
        return 0;
    }

    return TypesParserImpUtil::parseTimeTz(endPos, result, inputString);
}
}  // close package namespace

#define PARSE_FUNC_ELEM(BASE) parse##BASE
#define PARSE_FUNC_ARRAY(BASE) parse##BASE##Array

#define PARSE_ARRAY(FUNC_BASE, ELEM_TYPE)                                     \
int bdlpuxxx::NTypesParser::PARSE_FUNC_ARRAY(FUNC_BASE)(                          \
                                    const char             **endPos,          \
                                    bsl::vector<ELEM_TYPE>  *result,          \
                                    const char              *inputString)     \
{                                                                             \
    BSLS_ASSERT(0 != endPos);                                                 \
    BSLS_ASSERT(0 != result);                                                 \
    BSLS_ASSERT(0 != inputString);                                            \
                                                                              \
    result->clear();                                                          \
    bdlpuxxx::ParserImpUtil::skipWhiteSpace(endPos, inputString);                 \
                                                                              \
    if ('[' != **endPos) {                                                    \
        return BDEPU_FAILURE;                                                 \
    }                                                                         \
    ++*endPos;                                                                \
                                                                              \
    bdlpuxxx::ParserImpUtil::skipWhiteSpace(endPos, *endPos);                     \
                                                                              \
    ELEM_TYPE value;                                                          \
    while (']' != **endPos) {                                                 \
        inputString = *endPos;                                                \
        if (0 == bdlpuxxx::ParserImpUtil::parseNull(endPos, *endPos)) {           \
            value = bdltuxxx::Unset<ELEM_TYPE>::unsetValue();                     \
        }                                                                     \
        else if (bdlpuxxx::TypesParserImpUtil::PARSE_FUNC_ELEM(FUNC_BASE)(        \
                                                              endPos,         \
                                                              &value,         \
                                                              inputString)) { \
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

namespace bdlpuxxx {
PARSE_ARRAY(Char, char)
PARSE_ARRAY(Date, bdlt::Date)
PARSE_ARRAY(DateTz, bdlt::DateTz)
PARSE_ARRAY(Datetime, bdlt::Datetime)
PARSE_ARRAY(DatetimeTz, bdlt::DatetimeTz)
PARSE_ARRAY(Double, double)
PARSE_ARRAY(Float, float)
PARSE_ARRAY(Int, int)
PARSE_ARRAY(Int64, bsls::Types::Int64)
PARSE_ARRAY(Short, short)
PARSE_ARRAY(String, bsl::string)
PARSE_ARRAY(Time, bdlt::Time)
PARSE_ARRAY(TimeTz, bdlt::TimeTz)

                       // *** GENERATE FUNCTIONS ***

void NTypesParser::generateCharRaw(bsl::vector<char> *buffer,
                                         char               value,
                                         int                level,
                                         int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdltuxxx::Unset<char>::unsetValue()) {
        ParserImpUtil::generateNull(buffer);
    }
    else {
        TypesParserImpUtil::generateChar(buffer, value);
    }
}

void NTypesParser::generateShortRaw(bsl::vector<char> *buffer,
                                          short              value,
                                          int                level,
                                          int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdltuxxx::Unset<short>::unsetValue()) {
        ParserImpUtil::generateNull(buffer);
    }
    else {
        TypesParserImpUtil::generateShort(buffer, value);
    }
}

void NTypesParser::generateIntRaw(bsl::vector<char> *buffer,
                                        int                value,
                                        int                level,
                                        int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdltuxxx::Unset<int>::unsetValue()) {
        ParserImpUtil::generateNull(buffer);
    }
    else {
        TypesParserImpUtil::generateInt(buffer, value);
    }
}

void NTypesParser::generateInt64Raw(bsl::vector<char>  *buffer,
                                          bsls::Types::Int64  value,
                                          int                 level,
                                          int                 spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdltuxxx::Unset<bsls::Types::Int64>::unsetValue()) {
        ParserImpUtil::generateNull(buffer);
    }
    else {
        TypesParserImpUtil::generateInt64(buffer, value);
    }
}

void NTypesParser::generateFloatRaw(bsl::vector<char> *buffer,
                                          float              value,
                                          int                level,
                                          int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdltuxxx::Unset<float>::unsetValue()) {
        ParserImpUtil::generateNull(buffer);
    }
    else {
        TypesParserImpUtil::generateFloat(buffer, value);
    }
}

void NTypesParser::generateDoubleRaw(bsl::vector<char> *buffer,
                                           double             value,
                                           int                level,
                                           int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdltuxxx::Unset<double>::unsetValue()) {
        ParserImpUtil::generateNull(buffer);
    }
    else {
        TypesParserImpUtil::generateDouble(buffer, value);
    }
}

void NTypesParser::generateStringRaw(bsl::vector<char> *buffer,
                                           const char        *value,
                                           int                level,
                                           int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(value);

    if (level > 0) {
        ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdltuxxx::Unset<bsl::string>::unsetValue()) {
        ParserImpUtil::generateNull(buffer);
    }
    else {
        TypesParserImpUtil::generateString(buffer, value);
    }
}

void NTypesParser::generateDateRaw(bsl::vector<char> *buffer,
                                         const bdlt::Date&   value,
                                         int                level,
                                         int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdltuxxx::Unset<bdlt::Date>::unsetValue()) {
        ParserImpUtil::generateNull(buffer);
    }
    else {
        TypesParserImpUtil::generateDate(buffer, value);
    }
}

void NTypesParser::generateDateTzRaw(bsl::vector<char>  *buffer,
                                           const bdlt::DateTz&  value,
                                           int                 level,
                                           int                 spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdltuxxx::Unset<bdlt::DateTz>::unsetValue()) {
        ParserImpUtil::generateNull(buffer);
    }
    else {
        TypesParserImpUtil::generateDateTz(buffer, value);
    }
}

void NTypesParser::generateTimeRaw(bsl::vector<char> *buffer,
                                         const bdlt::Time&   value,
                                         int                level,
                                         int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdltuxxx::Unset<bdlt::Time>::unsetValue()) {
        ParserImpUtil::generateNull(buffer);
    }
    else {
        TypesParserImpUtil::generateTime(buffer, value);
    }
}

void NTypesParser::generateTimeTzRaw(bsl::vector<char>  *buffer,
                                           const bdlt::TimeTz&  value,
                                           int                 level,
                                           int                 spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdltuxxx::Unset<bdlt::TimeTz>::unsetValue()) {
        ParserImpUtil::generateNull(buffer);
    }
    else {
        TypesParserImpUtil::generateTimeTz(buffer, value);
    }
}

void NTypesParser::generateDatetimeRaw(
                                          bsl::vector<char>    *buffer,
                                          const bdlt::Datetime&  value,
                                          int                   level,
                                          int                   spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdltuxxx::Unset<bdlt::Datetime>::unsetValue()) {
        ParserImpUtil::generateNull(buffer);
    }
    else {
        TypesParserImpUtil::generateDatetime(buffer, value);
    }
}

void NTypesParser::generateDatetimeTzRaw(
                                        bsl::vector<char>      *buffer,
                                        const bdlt::DatetimeTz&  value,
                                        int                     level,
                                        int                     spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdltuxxx::Unset<bdlt::DatetimeTz>::unsetValue()) {
        ParserImpUtil::generateNull(buffer);
    }
    else {
        TypesParserImpUtil::generateDatetimeTz(buffer, value);
    }
}
}  // close package namespace

#define GEN_FUNC_ELEM(BASE) generate##BASE
#define GEN_FUNC_ARRAY(BASE) generate##BASE##ArrayRaw

#define GEN_ARRAY_RAW(FUNC_BASE, ELEM_TYPE)                                   \
void bdlpuxxx::NTypesParser::GEN_FUNC_ARRAY(FUNC_BASE)(                           \
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
        if (bdltuxxx::Unset<ELEM_TYPE>::isUnset(value[i])) {                      \
            bdlpuxxx::ParserImpUtil::generateNull(buffer);                        \
        }                                                                     \
        else {                                                                \
            bdlpuxxx::TypesParserImpUtil::GEN_FUNC_ELEM(FUNC_BASE)(buffer,        \
                                                               value[i]);     \
        }                                                                     \
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

void NTypesParser::generateChar(bsl::vector<char> *buffer,
                                      char               value,
                                      int                level,
                                      int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateCharRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateInt(bsl::vector<char> *buffer,
                                     int                value,
                                     int                level,
                                     int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateIntRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateInt64(bsl::vector<char>  *buffer,
                                       bsls::Types::Int64  value,
                                       int                 level,
                                       int                 spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateInt64Raw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateFloat(bsl::vector<char> *buffer,
                                       float              value,
                                       int                level,
                                       int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateFloatRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateDouble(bsl::vector<char> *buffer,
                                        double             value,
                                        int                level,
                                        int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDoubleRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateString(bsl::vector<char> *buffer,
                                        const char        *value,
                                        int                level,
                                        int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(value);

    stripNull(buffer);
    generateStringRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateDate(bsl::vector<char> *buffer,
                                      const bdlt::Date&   value,
                                      int                level,
                                      int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDateRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateDateTz(bsl::vector<char>  *buffer,
                                        const bdlt::DateTz&  value,
                                        int                 level,
                                        int                 spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDateTzRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateTime(bsl::vector<char> *buffer,
                                      const bdlt::Time&   value,
                                      int                level,
                                      int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateTimeRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateTimeTz(bsl::vector<char>  *buffer,
                                        const bdlt::TimeTz&  value,
                                        int                 level,
                                        int                 spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateTimeTzRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateDatetime(bsl::vector<char>    *buffer,
                                          const bdlt::Datetime&  value,
                                          int                   level,
                                          int                   spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDatetimeRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateDatetimeTz(
                                        bsl::vector<char>      *buffer,
                                        const bdlt::DatetimeTz&  value,
                                        int                     level,
                                        int                     spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDatetimeTzRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateCharArray(
                                      bsl::vector<char>        *buffer,
                                      const bsl::vector<char>&  value,
                                      int                       level,
                                      int                       spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateCharArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateShort(bsl::vector<char> *buffer,
                                       short              value,
                                       int                level,
                                       int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateShortRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateShortArray(
                                     bsl::vector<char>         *buffer,
                                     const bsl::vector<short>&  value,
                                     int                        level,
                                     int                        spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateShortArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateIntArray(
                                       bsl::vector<char>       *buffer,
                                       const bsl::vector<int>&  value,
                                       int                      level,
                                       int                      spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateIntArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateInt64Array(
                        bsl::vector<char>                      *buffer,
                        const bsl::vector<bsls::Types::Int64>&  value,
                        int                                     level,
                        int                                     spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateInt64ArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateFloatArray(
                                     bsl::vector<char>         *buffer,
                                     const bsl::vector<float>&  value,
                                     int                        level,
                                     int                        spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateFloatArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateDoubleArray(
                                    bsl::vector<char>          *buffer,
                                    const bsl::vector<double>&  value,
                                    int                         level,
                                    int                         spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDoubleArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateStringArray(
                               bsl::vector<char>               *buffer,
                               const bsl::vector<bsl::string>&  value,
                               int                              level,
                               int                              spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateStringArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateDateArray(
                                 bsl::vector<char>             *buffer,
                                 const bsl::vector<bdlt::Date>&  value,
                                 int                            level,
                                 int                            spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDateArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateDateTzArray(
                               bsl::vector<char>               *buffer,
                               const bsl::vector<bdlt::DateTz>&  value,
                               int                              level,
                               int                              spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDateTzArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateTimeArray(
                                 bsl::vector<char>             *buffer,
                                 const bsl::vector<bdlt::Time>&  value,
                                 int                            level,
                                 int                            spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateTimeArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateTimeTzArray(
                               bsl::vector<char>               *buffer,
                               const bsl::vector<bdlt::TimeTz>&  value,
                               int                              level,
                               int                              spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateTimeTzArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateDatetimeArray(
                            bsl::vector<char>                 *buffer,
                            const bsl::vector<bdlt::Datetime>&  value,
                            int                                level,
                            int                                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDatetimeArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void NTypesParser::generateDatetimeTzArray(
                          bsl::vector<char>                   *buffer,
                          const bsl::vector<bdlt::DatetimeTz>&  value,
                          int                                  level,
                          int                                  spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDatetimeTzArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
