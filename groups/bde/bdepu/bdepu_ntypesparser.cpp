// bdepu_ntypesparser.cpp                                             -*-C++-*-
#include <bdepu_ntypesparser.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdepu_ntypesparser_cpp,"$Id$ $CSID$")

#include <bdepu_typesparser.h>

#include <bsls_assert.h>

namespace BloombergLP {

enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = 1 };

                        // -------------------------
                        // struct bdepu_NTypesParser
                        // -------------------------

// CLASS METHODS

                       // *** PARSE FUNCTIONS ***

int bdepu_NTypesParser::parseChar(const char **endPos,
                                  char        *result,
                                  const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == bdepu_ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdetu_Unset<char>::unsetValue();
        return 0;
    }

    return bdepu_TypesParserImpUtil::parseChar(endPos, result, inputString);
}

int bdepu_NTypesParser::parseDate(const char **endPos,
                                  bdet_Date   *result,
                                  const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == bdepu_ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdetu_Unset<bdet_Date>::unsetValue();
        return 0;
    }

    return bdepu_TypesParserImpUtil::parseDate(endPos, result, inputString);
}

int bdepu_NTypesParser::parseDatetime(const char    **endPos,
                                      bdet_Datetime  *result,
                                      const char     *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == bdepu_ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdetu_Unset<bdet_Datetime>::unsetValue();
        return 0;
    }

    return bdepu_TypesParserImpUtil::parseDatetime(endPos,
                                                   result,
                                                   inputString);
}

int bdepu_NTypesParser::parseDatetimeTz(const char      **endPos,
                                        bdet_DatetimeTz  *result,
                                        const char       *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == bdepu_ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdetu_Unset<bdet_DatetimeTz>::unsetValue();
        return 0;
    }

    return bdepu_TypesParserImpUtil::parseDatetimeTz(endPos,
                                                     result,
                                                     inputString);
}

int bdepu_NTypesParser::parseDateTz(const char  **endPos,
                                    bdet_DateTz  *result,
                                    const char   *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == bdepu_ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdetu_Unset<bdet_DateTz>::unsetValue();
        return 0;
    }

    return bdepu_TypesParserImpUtil::parseDateTz(endPos, result, inputString);
}

int bdepu_NTypesParser::parseDouble(const char **endPos,
                                    double      *result,
                                    const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == bdepu_ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdetu_Unset<double>::unsetValue();
        return 0;
    }

    return bdepu_TypesParserImpUtil::parseDouble(endPos, result, inputString);
}

int bdepu_NTypesParser::parseFloat(const char **endPos,
                                   float       *result,
                                   const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == bdepu_ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdetu_Unset<float>::unsetValue();
        return 0;
    }

    return bdepu_TypesParserImpUtil::parseFloat(endPos, result, inputString);
}

int bdepu_NTypesParser::parseInt(const char **endPos,
                                 int         *result,
                                 const char  *inputString,
                                 int          base)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == bdepu_ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdetu_Unset<int>::unsetValue();
        return 0;
    }

    return bdepu_TypesParserImpUtil::parseInt(endPos,
                                              result,
                                              inputString,
                                              base);
}

int bdepu_NTypesParser::parseInt64(const char               **endPos,
                                   bsls_PlatformUtil::Int64  *result,
                                   const char                *inputString,
                                   int                        base)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == bdepu_ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdetu_Unset<bsls_PlatformUtil::Int64>::unsetValue();
        return 0;
    }

    return bdepu_TypesParserImpUtil::parseInt64(endPos,
                                                result,
                                                inputString,
                                                base);
}

int bdepu_NTypesParser::parseShort(const char **endPos,
                                   short       *result,
                                   const char  *inputString,
                                   int          base)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == bdepu_ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdetu_Unset<short>::unsetValue();
        return 0;
    }

    return bdepu_TypesParserImpUtil::parseShort(endPos,
                                                result,
                                                inputString,
                                                base);
}

int bdepu_NTypesParser::parseString(const char  **endPos,
                                    bsl::string  *result,
                                    const char   *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == bdepu_ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdetu_Unset<bsl::string>::unsetValue();
        return 0;
    }

    return bdepu_TypesParserImpUtil::parseString(endPos, result, inputString);
}

int bdepu_NTypesParser::parseQuotedString(const char  **endPos,
                                          bsl::string  *result,
                                          const char   *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == bdepu_ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdetu_Unset<bsl::string>::unsetValue();
        return 0;
    }

    return bdepu_TypesParserImpUtil::parseQuotedString(endPos,
                                                       result,
                                                       inputString);
}

int bdepu_NTypesParser::parseUnquotedString(const char  **endPos,
                                            bsl::string  *result,
                                            const char   *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == bdepu_ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdetu_Unset<bsl::string>::unsetValue();
        return 0;
    }

    return bdepu_TypesParserImpUtil::parseUnquotedString(endPos,
                                                         result,
                                                         inputString);
}

int bdepu_NTypesParser::parseSpaceDelimitedString(const char  **endPos,
                                                  bsl::string  *result,
                                                  const char   *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == bdepu_ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdetu_Unset<bsl::string>::unsetValue();
        return 0;
    }

    return bdepu_TypesParserImpUtil::parseSpaceDelimitedString(endPos,
                                                               result,
                                                               inputString);
}

int bdepu_NTypesParser::parseTime(const char **endPos,
                                  bdet_Time   *result,
                                  const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == bdepu_ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdetu_Unset<bdet_Time>::unsetValue();
        return 0;
    }

    return bdepu_TypesParserImpUtil::parseTime(endPos, result, inputString);
}

int bdepu_NTypesParser::parseTimeTz(const char  **endPos,
                                    bdet_TimeTz  *result,
                                    const char   *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == bdepu_ParserImpUtil::parseNull(endPos, inputString)) {
        *result = bdetu_Unset<bdet_TimeTz>::unsetValue();
        return 0;
    }

    return bdepu_TypesParserImpUtil::parseTimeTz(endPos, result, inputString);
}

#define PARSE_FUNC_ELEM(BASE) parse##BASE
#define PARSE_FUNC_ARRAY(BASE) parse##BASE##Array

#define PARSE_ARRAY(FUNC_BASE, ELEM_TYPE)                                     \
int bdepu_NTypesParser::PARSE_FUNC_ARRAY(FUNC_BASE)(                          \
                                    const char             **endPos,          \
                                    bsl::vector<ELEM_TYPE>  *result,          \
                                    const char              *inputString)     \
{                                                                             \
    BSLS_ASSERT(0 != endPos);                                                 \
    BSLS_ASSERT(0 != result);                                                 \
    BSLS_ASSERT(0 != inputString);                                            \
                                                                              \
    result->clear();                                                          \
    bdepu_ParserImpUtil::skipWhiteSpace(endPos, inputString);                 \
                                                                              \
    if ('[' != **endPos) {                                                    \
        return BDEPU_FAILURE;                                                 \
    }                                                                         \
    ++*endPos;                                                                \
                                                                              \
    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);                     \
                                                                              \
    ELEM_TYPE value;                                                          \
    while (']' != **endPos) {                                                 \
        inputString = *endPos;                                                \
        if (0 == bdepu_ParserImpUtil::parseNull(endPos, *endPos)) {           \
            value = bdetu_Unset<ELEM_TYPE>::unsetValue();                     \
        }                                                                     \
        else if (bdepu_TypesParserImpUtil::PARSE_FUNC_ELEM(FUNC_BASE)(        \
                                                              endPos,         \
                                                              &value,         \
                                                              inputString)) { \
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
PARSE_ARRAY(DateTz, bdet_DateTz)
PARSE_ARRAY(Datetime, bdet_Datetime)
PARSE_ARRAY(DatetimeTz, bdet_DatetimeTz)
PARSE_ARRAY(Double, double)
PARSE_ARRAY(Float, float)
PARSE_ARRAY(Int, int)
PARSE_ARRAY(Int64, bsls_PlatformUtil::Int64)
PARSE_ARRAY(Short, short)
PARSE_ARRAY(String, bsl::string)
PARSE_ARRAY(Time, bdet_Time)
PARSE_ARRAY(TimeTz, bdet_TimeTz)

                       // *** GENERATE FUNCTIONS ***

void bdepu_NTypesParser::generateCharRaw(bsl::vector<char> *buffer,
                                         char               value,
                                         int                level,
                                         int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdetu_Unset<char>::unsetValue()) {
        bdepu_ParserImpUtil::generateNull(buffer);
    }
    else {
        bdepu_TypesParserImpUtil::generateChar(buffer, value);
    }
}

void bdepu_NTypesParser::generateShortRaw(bsl::vector<char> *buffer,
                                          short              value,
                                          int                level,
                                          int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdetu_Unset<short>::unsetValue()) {
        bdepu_ParserImpUtil::generateNull(buffer);
    }
    else {
        bdepu_TypesParserImpUtil::generateShort(buffer, value);
    }
}

void bdepu_NTypesParser::generateIntRaw(bsl::vector<char> *buffer,
                                        int                value,
                                        int                level,
                                        int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdetu_Unset<int>::unsetValue()) {
        bdepu_ParserImpUtil::generateNull(buffer);
    }
    else {
        bdepu_TypesParserImpUtil::generateInt(buffer, value);
    }
}

void bdepu_NTypesParser::generateInt64Raw(
                                      bsl::vector<char>        *buffer,
                                      bsls_PlatformUtil::Int64  value,
                                      int                       level,
                                      int                       spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdetu_Unset<bsls_PlatformUtil::Int64>::unsetValue()) {
        bdepu_ParserImpUtil::generateNull(buffer);
    }
    else {
        bdepu_TypesParserImpUtil::generateInt64(buffer, value);
    }
}

void bdepu_NTypesParser::generateFloatRaw(bsl::vector<char> *buffer,
                                          float              value,
                                          int                level,
                                          int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdetu_Unset<float>::unsetValue()) {
        bdepu_ParserImpUtil::generateNull(buffer);
    }
    else {
        bdepu_TypesParserImpUtil::generateFloat(buffer, value);
    }
}

void bdepu_NTypesParser::generateDoubleRaw(bsl::vector<char> *buffer,
                                           double             value,
                                           int                level,
                                           int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdetu_Unset<double>::unsetValue()) {
        bdepu_ParserImpUtil::generateNull(buffer);
    }
    else {
        bdepu_TypesParserImpUtil::generateDouble(buffer, value);
    }
}

void bdepu_NTypesParser::generateStringRaw(bsl::vector<char> *buffer,
                                           const char        *value,
                                           int                level,
                                           int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(value);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdetu_Unset<bsl::string>::unsetValue()) {
        bdepu_ParserImpUtil::generateNull(buffer);
    }
    else {
        bdepu_TypesParserImpUtil::generateString(buffer, value);
    }
}

void bdepu_NTypesParser::generateDateRaw(bsl::vector<char> *buffer,
                                         const bdet_Date&   value,
                                         int                level,
                                         int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdetu_Unset<bdet_Date>::unsetValue()) {
        bdepu_ParserImpUtil::generateNull(buffer);
    }
    else {
        bdepu_TypesParserImpUtil::generateDate(buffer, value);
    }
}

void bdepu_NTypesParser::generateDateTzRaw(bsl::vector<char>  *buffer,
                                           const bdet_DateTz&  value,
                                           int                 level,
                                           int                 spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdetu_Unset<bdet_DateTz>::unsetValue()) {
        bdepu_ParserImpUtil::generateNull(buffer);
    }
    else {
        bdepu_TypesParserImpUtil::generateDateTz(buffer, value);
    }
}

void bdepu_NTypesParser::generateTimeRaw(bsl::vector<char> *buffer,
                                         const bdet_Time&   value,
                                         int                level,
                                         int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdetu_Unset<bdet_Time>::unsetValue()) {
        bdepu_ParserImpUtil::generateNull(buffer);
    }
    else {
        bdepu_TypesParserImpUtil::generateTime(buffer, value);
    }
}

void bdepu_NTypesParser::generateTimeTzRaw(bsl::vector<char>  *buffer,
                                           const bdet_TimeTz&  value,
                                           int                 level,
                                           int                 spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdetu_Unset<bdet_TimeTz>::unsetValue()) {
        bdepu_ParserImpUtil::generateNull(buffer);
    }
    else {
        bdepu_TypesParserImpUtil::generateTimeTz(buffer, value);
    }
}

void bdepu_NTypesParser::generateDatetimeRaw(
                                          bsl::vector<char>    *buffer,
                                          const bdet_Datetime&  value,
                                          int                   level,
                                          int                   spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdetu_Unset<bdet_Datetime>::unsetValue()) {
        bdepu_ParserImpUtil::generateNull(buffer);
    }
    else {
        bdepu_TypesParserImpUtil::generateDatetime(buffer, value);
    }
}

void bdepu_NTypesParser::generateDatetimeTzRaw(
                                        bsl::vector<char>      *buffer,
                                        const bdet_DatetimeTz&  value,
                                        int                     level,
                                        int                     spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    if (value == bdetu_Unset<bdet_DatetimeTz>::unsetValue()) {
        bdepu_ParserImpUtil::generateNull(buffer);
    }
    else {
        bdepu_TypesParserImpUtil::generateDatetimeTz(buffer, value);
    }
}

#define GEN_FUNC_ELEM(BASE) generate##BASE
#define GEN_FUNC_ARRAY(BASE) generate##BASE##ArrayRaw

#define GEN_ARRAY_RAW(FUNC_BASE, ELEM_TYPE)                                   \
void bdepu_NTypesParser::GEN_FUNC_ARRAY(FUNC_BASE)(                           \
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
        if (bdetu_Unset<ELEM_TYPE>::isUnset(value[i])) {                      \
            bdepu_ParserImpUtil::generateNull(buffer);                        \
        }                                                                     \
        else {                                                                \
            bdepu_TypesParserImpUtil::GEN_FUNC_ELEM(FUNC_BASE)(buffer,        \
                                                               value[i]);     \
        }                                                                     \
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

void bdepu_NTypesParser::generateChar(bsl::vector<char> *buffer,
                                      char               value,
                                      int                level,
                                      int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateCharRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdepu_NTypesParser::generateInt(bsl::vector<char> *buffer,
                                     int                value,
                                     int                level,
                                     int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateIntRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdepu_NTypesParser::generateInt64(
                                      bsl::vector<char>        *buffer,
                                      bsls_PlatformUtil::Int64  value,
                                      int                       level,
                                      int                       spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateInt64Raw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdepu_NTypesParser::generateFloat(bsl::vector<char> *buffer,
                                       float              value,
                                       int                level,
                                       int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateFloatRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdepu_NTypesParser::generateDouble(bsl::vector<char> *buffer,
                                        double             value,
                                        int                level,
                                        int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDoubleRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdepu_NTypesParser::generateString(bsl::vector<char> *buffer,
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

void bdepu_NTypesParser::generateDate(bsl::vector<char> *buffer,
                                      const bdet_Date&   value,
                                      int                level,
                                      int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDateRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdepu_NTypesParser::generateDateTz(bsl::vector<char>  *buffer,
                                        const bdet_DateTz&  value,
                                        int                 level,
                                        int                 spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDateTzRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdepu_NTypesParser::generateTime(bsl::vector<char> *buffer,
                                      const bdet_Time&   value,
                                      int                level,
                                      int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateTimeRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdepu_NTypesParser::generateTimeTz(bsl::vector<char>  *buffer,
                                        const bdet_TimeTz&  value,
                                        int                 level,
                                        int                 spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateTimeTzRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdepu_NTypesParser::generateDatetime(bsl::vector<char>    *buffer,
                                          const bdet_Datetime&  value,
                                          int                   level,
                                          int                   spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDatetimeRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdepu_NTypesParser::generateDatetimeTz(
                                        bsl::vector<char>      *buffer,
                                        const bdet_DatetimeTz&  value,
                                        int                     level,
                                        int                     spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDatetimeTzRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdepu_NTypesParser::generateCharArray(
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

void bdepu_NTypesParser::generateShort(bsl::vector<char> *buffer,
                                       short              value,
                                       int                level,
                                       int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateShortRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdepu_NTypesParser::generateShortArray(
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

void bdepu_NTypesParser::generateIntArray(
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

void bdepu_NTypesParser::generateInt64Array(
                  bsl::vector<char>                            *buffer,
                  const bsl::vector<bsls_PlatformUtil::Int64>&  value,
                  int                                           level,
                  int                                           spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateInt64ArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdepu_NTypesParser::generateFloatArray(
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

void bdepu_NTypesParser::generateDoubleArray(
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

void bdepu_NTypesParser::generateStringArray(
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

void bdepu_NTypesParser::generateDateArray(
                                 bsl::vector<char>             *buffer,
                                 const bsl::vector<bdet_Date>&  value,
                                 int                            level,
                                 int                            spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDateArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdepu_NTypesParser::generateDateTzArray(
                               bsl::vector<char>               *buffer,
                               const bsl::vector<bdet_DateTz>&  value,
                               int                              level,
                               int                              spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDateTzArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdepu_NTypesParser::generateTimeArray(
                                 bsl::vector<char>             *buffer,
                                 const bsl::vector<bdet_Time>&  value,
                                 int                            level,
                                 int                            spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateTimeArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdepu_NTypesParser::generateTimeTzArray(
                               bsl::vector<char>               *buffer,
                               const bsl::vector<bdet_TimeTz>&  value,
                               int                              level,
                               int                              spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateTimeTzArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdepu_NTypesParser::generateDatetimeArray(
                            bsl::vector<char>                 *buffer,
                            const bsl::vector<bdet_Datetime>&  value,
                            int                                level,
                            int                                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDatetimeArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdepu_NTypesParser::generateDatetimeTzArray(
                          bsl::vector<char>                   *buffer,
                          const bsl::vector<bdet_DatetimeTz>&  value,
                          int                                  level,
                          int                                  spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    stripNull(buffer);
    generateDatetimeTzArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
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
