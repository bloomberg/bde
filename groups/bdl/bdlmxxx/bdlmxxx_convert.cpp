// bdlmxxx_convert.cpp                                                   -*-C++-*-
#include <bdlmxxx_convert.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_convert_cpp,"$Id$ $CSID$")

#include <bdlmxxx_choice.h>
#include <bdlmxxx_choicearray.h>
#include <bdlmxxx_choicearrayitem.h>
#include <bdlmxxx_list.h>
#include <bdlmxxx_row.h>
#include <bdlmxxx_table.h>

#include <bdlpuxxx_iso8601.h>
#include <bdlpuxxx_typesparser.h>
#include <bdlpuxxx_typesparserimputil.h>

#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_date.h>
#include <bdlt_datetz.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bsls_types.h>

#include <bsl_cctype.h>
#include <bsl_cstdlib.h>

namespace BloombergLP {

// LOCAL CONSTANTS
enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };
    // This enumeration defines the status values returned by methods in this
    // file.

// STATIC HELPER FUNCTIONS
static
int getWord(const char **strPtr)
    // Advance '*strPtr' to the first non-whitespace character then return the
    // length of the non-whitespace portion of the string starting from the
    // new value of '*strPtr'.
{
    BSLS_ASSERT(strPtr);

    const char *newPtr = *strPtr;

    // Skip whitespace.
    while (*newPtr && bsl::isspace(static_cast<unsigned char>(*newPtr))) {
        ++newPtr;
    }
    *strPtr = newPtr;

    // Find next whitespace.
    while (*newPtr && ! bsl::isspace(static_cast<unsigned char>(*newPtr))) {
        ++newPtr;
    }

    return newPtr - *strPtr;
}

namespace bdlmxxx {
                        // ------------------
                        // class Convert
                        // ------------------

// PRIVATE CLASS METHODS
int Convert::fromString(bool *dstAddr, const char *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    const int wordLen = getWord(&srcValue);

    switch (wordLen) {
      case 1: {
        if ('1' == srcValue[0]) {
            *dstAddr = true;
            return BDEM_SUCCESS;                                      // RETURN
        }
        else if ('0' == srcValue[0]) {
            *dstAddr = false;
            return BDEM_SUCCESS;                                      // RETURN
        }
      } break;
      case 4: {
        if (('t' == srcValue[0] || 'T' == srcValue[0])
         && ('r' == srcValue[1] || 'R' == srcValue[1])
         && ('u' == srcValue[2] || 'U' == srcValue[2])
         && ('e' == srcValue[3] || 'E' == srcValue[3])) {
            *dstAddr = true;
            return BDEM_SUCCESS;                                      // RETURN
        }
      } break;
      case 5: {
        if (('f' == srcValue[0] || 'F' == srcValue[0])
         && ('a' == srcValue[1] || 'A' == srcValue[1])
         && ('l' == srcValue[2] || 'L' == srcValue[2])
         && ('s' == srcValue[3] || 'S' == srcValue[3])
         && ('e' == srcValue[4] || 'E' == srcValue[4])) {
            *dstAddr = false;
            return BDEM_SUCCESS;                                      // RETURN
        }
      } break;
    }

    return BDEM_FAILURE;
}

int Convert::fromString(char *dstAddr, const char *srcValue)
{
    BSLS_ASSERT(dstAddr);

    *dstAddr = srcValue ? *srcValue : 0;

    return BDEM_SUCCESS;
}

int Convert::fromString(short *dstAddr, const char *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    char *endPtr = 0;
    int i = (int) bsl::strtol(srcValue, &endPtr, 10);
    if (endPtr == srcValue) {
        return BDEM_FAILURE;                                          // RETURN
    }

    *dstAddr = (short) i;
    return BDEM_SUCCESS;
}

int Convert::fromString(int *dstAddr, const char *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    char *endPtr = 0;
    int i = (int) bsl::strtol(srcValue, &endPtr, 10);
    if (endPtr == srcValue) {
        return BDEM_FAILURE;                                          // RETURN
    }

    *dstAddr = i;
    return BDEM_SUCCESS;
}

int Convert::fromString(bsls::Types::Int64 *dstAddr, const char *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    const char *endPtr = 0;
    Int64       result = 0;

    if (bdlpuxxx::TypesParser::parseInt64(&endPtr, &result, srcValue, 10)) {
        return BDEM_FAILURE;                                          // RETURN
    }

    *dstAddr = result;
    return BDEM_SUCCESS;
}

int Convert::fromString(float *dstAddr, const char *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    char *endPtr = 0;
    double d = bsl::strtod(srcValue, &endPtr);
    if (endPtr == srcValue) {
        return BDEM_FAILURE;                                          // RETURN
    }

    *dstAddr = static_cast<float>(d);
    return BDEM_SUCCESS;
}

int Convert::fromString(double *dstAddr, const char *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    char *endPtr = 0;
    double d = bsl::strtod(srcValue, &endPtr);
    if (endPtr == srcValue) {
        return BDEM_FAILURE;                                          // RETURN
    }

    *dstAddr = d;
    return BDEM_SUCCESS;
}

int Convert::fromString(bdlt::Datetime *dstAddr, const char *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    const int wordLen = getWord(&srcValue);
    return bdlpuxxx::Iso8601::parse(dstAddr, srcValue, wordLen);
}

int Convert::fromString(bdlt::Date *dstAddr, const char *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    const int wordLen = getWord(&srcValue);
    return bdlpuxxx::Iso8601::parse(dstAddr, srcValue, wordLen);
}

int Convert::fromString(bdlt::Time *dstAddr, const char *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    const int wordLen = getWord(&srcValue);
    return bdlpuxxx::Iso8601::parse(dstAddr, srcValue, wordLen);
}

int Convert::fromString(bdlt::DatetimeTz *dstAddr, const char *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    const int wordLen = getWord(&srcValue);
    return bdlpuxxx::Iso8601::parse(dstAddr, srcValue, wordLen);
}

int Convert::fromString(bdlt::DateTz *dstAddr, const char *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    const int wordLen = getWord(&srcValue);
    return bdlpuxxx::Iso8601::parse(dstAddr, srcValue, wordLen);
}

int Convert::fromString(bdlt::TimeTz *dstAddr, const char *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    const int wordLen = getWord(&srcValue);
    return bdlpuxxx::Iso8601::parse(dstAddr, srcValue, wordLen);
}

// CLASS METHODS
int Convert::convert(ElemRef             *dstAddr,
                          const ConstElemRef&  srcValue)
{
    BSLS_ASSERT(dstAddr);

    const int rc = Convert::fromBdemType(dstAddr,
                                              srcValue.data(),
                                              srcValue.type());

    if (!rc && srcValue.isBound() && srcValue.isNull()) {
        // Make destination null only if the conversion succeeds.
        dstAddr->makeNull();
    }

    return rc;
}

int Convert::convert(ElemRef        *dstAddr,
                          const ElemRef&  srcValue)
{
    BSLS_ASSERT(dstAddr);

    const int nullnessWord = srcValue.isBound() && srcValue.isNull();
    ConstElemRef elemRef(srcValue.dataRaw(),
                              srcValue.descriptor(),
                              &nullnessWord,
                              0);

    return Convert::convert(dstAddr, elemRef);
}

int Convert::convert(bsl::string *dstAddr, char srcValue)
{
    BSLS_ASSERT(dstAddr);

    if ('\0' == srcValue) {
        dstAddr->clear();
    }
    else {
        dstAddr->assign(&srcValue, 1);
    }
    return BDEM_SUCCESS;
}

int Convert::convert(bsl::string *dstAddr, short srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdltuxxx::Unset<short>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }
    char buffer[bdlpuxxx::TypesParserImpUtil::BDEPU_MAX_SHORT_STRLEN10 + 1];
    int length = bdlpuxxx::TypesParserImpUtil::generateIntRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int Convert::convert(bsl::string *dstAddr, int srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdltuxxx::Unset<int>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdlpuxxx::TypesParserImpUtil::BDEPU_MAX_INT_STRLEN10 + 1];
    int length = bdlpuxxx::TypesParserImpUtil::generateIntRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int Convert::convert(bsl::string *dstAddr, bsls::Types::Int64 srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdltuxxx::Unset<Int64>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdlpuxxx::TypesParserImpUtil::BDEPU_MAX_INT64_STRLEN10 + 1];
    int length = bdlpuxxx::TypesParserImpUtil::generateInt64Raw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int Convert::convert(bsl::string *dstAddr, float srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdltuxxx::Unset<float>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdlpuxxx::TypesParserImpUtil::BDEPU_MAX_FLOAT_STRLEN10 + 1];
    int length = bdlpuxxx::TypesParserImpUtil::generateFloatRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int Convert::convert(bsl::string *dstAddr, double srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdltuxxx::Unset<double>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdlpuxxx::TypesParserImpUtil::BDEPU_MAX_DOUBLE_STRLEN10 + 1];
    int length = bdlpuxxx::TypesParserImpUtil::generateDoubleRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int Convert::convert(bsl::string *dstAddr, long double srcValue)
{
    BSLS_ASSERT(dstAddr);

    char buffer[bdlpuxxx::TypesParserImpUtil::BDEPU_MAX_LONGDOUBLE_STRLEN10 + 1];
    int length = bdlpuxxx::TypesParserImpUtil::generateLongDoubleRaw(buffer,
                                                                 srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int Convert::convert(bsl::string *dstAddr, const bdlt::Datetime& srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdltuxxx::Unset<bdlt::Datetime>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdlpuxxx::Iso8601::BDEPU_MAX_DATETIME_STRLEN + 1];
    int  length = bdlpuxxx::Iso8601::generateRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int Convert::convert(bsl::string *dstAddr, const bdlt::Date& srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdltuxxx::Unset<bdlt::Date>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdlpuxxx::Iso8601::BDEPU_MAX_DATETIME_STRLEN + 1];
    int  length = bdlpuxxx::Iso8601::generateRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int Convert::convert(bsl::string *dstAddr, const bdlt::Time& srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdltuxxx::Unset<bdlt::Time>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdlpuxxx::Iso8601::BDEPU_MAX_DATETIME_STRLEN + 1];
    int  length = bdlpuxxx::Iso8601::generateRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int Convert::convert(bsl::string            *dstAddr,
                          const bdlt::DatetimeTz&  srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdltuxxx::Unset<bdlt::DatetimeTz>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdlpuxxx::Iso8601::BDEPU_MAX_DATETIME_STRLEN + 1];
    int  length = bdlpuxxx::Iso8601::generateRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int Convert::convert(bsl::string *dstAddr, const bdlt::DateTz& srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdltuxxx::Unset<bdlt::DateTz>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdlpuxxx::Iso8601::BDEPU_MAX_DATETIME_STRLEN + 1];
    int  length = bdlpuxxx::Iso8601::generateRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int Convert::convert(bsl::string *dstAddr, const bdlt::TimeTz& srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdltuxxx::Unset<bdlt::TimeTz>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdlpuxxx::Iso8601::BDEPU_MAX_DATETIME_STRLEN + 1];
    int  length = bdlpuxxx::Iso8601::generateRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

// Note that the following three methods for converting "non-TZ" to "TZ" were
// added in response to DRQS 13447607.

int Convert::convert(bdlt::DateTz *dstAddr, const bdlt::Date& srcValue)
{
    BSLS_ASSERT(dstAddr);

    *dstAddr = bdlt::DateTz(srcValue, 0);
    return 0;
}

int Convert::convert(bdlt::TimeTz *dstAddr, const bdlt::Time& srcValue)
{
    BSLS_ASSERT(dstAddr);

    *dstAddr = bdlt::TimeTz(srcValue, 0);
    return 0;
}

int Convert::convert(bdlt::DatetimeTz      *dstAddr,
                          const bdlt::Datetime&  srcValue)
{
    BSLS_ASSERT(dstAddr);

    *dstAddr = bdlt::DatetimeTz(srcValue, 0);
    return 0;
}

int Convert::convert(List *dstAddr, const Row& srcValue)
{
    BSLS_ASSERT(dstAddr);

    // Convert a 'Row' to a 'List'.
    *dstAddr = srcValue;
    return BDEM_SUCCESS;
}

int Convert::convert(Row *dstAddr, const Row& srcValue)
    // Specialization: 'Row' does not have a public assignment operator,
    // so this special conversion logic applies.
{
    BSLS_ASSERT(dstAddr);

    // Ensure that rows have identical element types, else fail conversion.

    const int srcLen = srcValue.length();
    if (dstAddr->length() != srcLen) {
        return BDEM_FAILURE;                                          // RETURN
    }

    for (int i = 0; i < srcLen; ++i) {
        if (dstAddr->elemType(i) != srcValue.elemType(i)) {
            return BDEM_FAILURE;                                      // RETURN
        }
    }

    dstAddr->replaceValues(srcValue);
    return BDEM_SUCCESS;
}

int Convert::convert(Row *dstAddr, const List& srcValue)
{
    BSLS_ASSERT(dstAddr);

    // Convert a 'List' to a 'Row'.  Conversion will fail unless the
    // list and row have the same number and types of elements.

    return Convert::convert(dstAddr, srcValue.row());
}

int Convert::convert(Choice                 *dstAddr,
                          const ChoiceArrayItem&  srcValue)
{
    BSLS_ASSERT(dstAddr);

    // Convert a 'ChoiceArrayItem' to a 'Choice'.
    *dstAddr = srcValue;
    return BDEM_SUCCESS;
}

int Convert::convert(ChoiceArrayItem        *dstAddr,
                          const ChoiceArrayItem&  srcValue)
    // Specialization: 'ChoiceArrayItem' does not have a public
    // assignment operator, so this special conversion logic applies.
{
    BSLS_ASSERT(dstAddr);

    // Ensure that choice items are compatible, else fail conversion.
    if (dstAddr->numSelections() <= srcValue.selector()
     || dstAddr->selectionType(srcValue.selector()) !=
        srcValue.selectionType()) {
        return BDEM_FAILURE;                                          // RETURN
    }

    dstAddr->makeSelection(srcValue.selector()).replaceValue(
                                                         srcValue.selection());
    return BDEM_SUCCESS;
}

int Convert::convert(ChoiceArrayItem *dstAddr,
                          const Choice&    srcValue)
{
    BSLS_ASSERT(dstAddr);

    // Convert a 'Choice' to a 'ChoiceArrayItem'.  Conversion will
    // fail unless the 'Choice' and 'ChoiceArrayItem' have the same
    // types catalog.

    return Convert::convert(dstAddr, srcValue.item());
}

int Convert::toBdemType(void                *dstAddr,
                             ElemType::Type  dstType,
                             const ElemRef&  srcValue)
{
    BSLS_ASSERT(dstAddr || ElemType::BDEM_VOID == dstType);

    return Convert::convertBdemTypes(dstAddr, dstType,
                                          srcValue.dataRaw(), srcValue.type());
}

int Convert::fromBdemType(ElemRef        *dstAddr,
                               const void          *srcAddr,
                               ElemType::Type  srcType)
{
    BSLS_ASSERT(dstAddr);
    BSLS_ASSERT(srcAddr || ElemType::BDEM_VOID == srcType);

    const bool                 isDstNull = dstAddr->isBound()
                                        && dstAddr->isNull();
    const ElemType::Type  dstType   = dstAddr->type();

    // 'dstType' is 'ElemType::BDEM_VOID' if '*dstAddr' is unbound.

    void *dstValuePtr;

    switch (dstType) {
      case ElemType::BDEM_CHAR: {
        dstValuePtr = (void *) &dstAddr->theModifiableChar();
      } break;
      case ElemType::BDEM_SHORT: {
        dstValuePtr = (void *) &dstAddr->theModifiableShort();
      } break;
      case ElemType::BDEM_INT: {
        dstValuePtr = (void *) &dstAddr->theModifiableInt();
      } break;
      case ElemType::BDEM_INT64: {
        dstValuePtr = (void *) &dstAddr->theModifiableInt64();
      } break;
      case ElemType::BDEM_FLOAT: {
        dstValuePtr = (void *) &dstAddr->theModifiableFloat();
      } break;
      case ElemType::BDEM_DOUBLE: {
        dstValuePtr = (void *) &dstAddr->theModifiableDouble();
      } break;
      case ElemType::BDEM_STRING: {
        dstValuePtr = (void *) &dstAddr->theModifiableString();
      } break;
      case ElemType::BDEM_DATETIME: {
        dstValuePtr = (void *) &dstAddr->theModifiableDatetime();
      } break;
      case ElemType::BDEM_DATE: {
        dstValuePtr = (void *) &dstAddr->theModifiableDate();
      } break;
      case ElemType::BDEM_TIME: {
        dstValuePtr = (void *) &dstAddr->theModifiableTime();
      } break;
      case ElemType::BDEM_CHAR_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableCharArray();
      } break;
      case ElemType::BDEM_SHORT_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableShortArray();
      } break;
      case ElemType::BDEM_INT_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableIntArray();
      } break;
      case ElemType::BDEM_INT64_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableInt64Array();
      } break;
      case ElemType::BDEM_FLOAT_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableFloatArray();
      } break;
      case ElemType::BDEM_DOUBLE_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableDoubleArray();
      } break;
      case ElemType::BDEM_STRING_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableStringArray();
      } break;
      case ElemType::BDEM_DATETIME_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableDatetimeArray();
      } break;
      case ElemType::BDEM_DATE_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableDateArray();
      } break;
      case ElemType::BDEM_TIME_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableTimeArray();
      } break;
      case ElemType::BDEM_LIST: {
        dstValuePtr = (void *) &dstAddr->theModifiableList();
      } break;
      case ElemType::BDEM_TABLE: {
        dstValuePtr = (void *) &dstAddr->theModifiableTable();
      } break;
      case ElemType::BDEM_BOOL: {
        dstValuePtr = (void *) &dstAddr->theModifiableBool();
      } break;
      case ElemType::BDEM_DATETIMETZ: {
        dstValuePtr = (void *) &dstAddr->theModifiableDatetimeTz();
      } break;
      case ElemType::BDEM_DATETZ: {
        dstValuePtr = (void *) &dstAddr->theModifiableDateTz();
      } break;
      case ElemType::BDEM_TIMETZ: {
        dstValuePtr = (void *) &dstAddr->theModifiableTimeTz();
      } break;
      case ElemType::BDEM_BOOL_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableBoolArray();
      } break;
      case ElemType::BDEM_DATETIMETZ_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableDatetimeTzArray();
      } break;
      case ElemType::BDEM_DATETZ_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableDateTzArray();
      } break;
      case ElemType::BDEM_TIMETZ_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableTimeTzArray();
      } break;
      case ElemType::BDEM_CHOICE: {
        dstValuePtr = (void *) &dstAddr->theModifiableChoice();
      } break;
      case ElemType::BDEM_CHOICE_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableChoiceArray();
      } break;
      case ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        dstValuePtr = (void *) &dstAddr->theModifiableChoiceArrayItem();
      } break;
      case ElemType::BDEM_ROW: {
        dstValuePtr = (void *) &dstAddr->theModifiableRow();
      } break;
      case ElemType::BDEM_VOID: {
        // Void-to-void conversion is legal, but void to anything else is not.
        return ElemType::BDEM_VOID == srcType
               ? BDEM_SUCCESS
               : BDEM_FAILURE;
      } break;
      default: {
        return BDEM_FAILURE;
      } break;
    }

    int rc = convertBdemTypes(dstValuePtr, dstType, srcAddr, srcType);

    if (rc && isDstNull) {
        // If the destination was originally null and the conversion failed
        // then make it null again.
        dstAddr->makeNull();
    }

    return rc;
}

int Convert::convertBdemTypes(void                *dstAddr,
                                   ElemType::Type  dstType,
                                   const void          *srcAddr,
                                   ElemType::Type  srcType)
{
    BSLS_ASSERT(dstAddr || ElemType::BDEM_VOID == dstType);
    BSLS_ASSERT(srcAddr || ElemType::BDEM_VOID == srcType);

    int result;

    switch (srcType) {
      case ElemType::BDEM_CHAR: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const char*) srcAddr);
      } break;
      case ElemType::BDEM_SHORT: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const short*) srcAddr);
      } break;
      case ElemType::BDEM_INT: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const int*) srcAddr);
      } break;
      case ElemType::BDEM_INT64: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const Int64*) srcAddr);
      } break;
      case ElemType::BDEM_FLOAT: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const float*) srcAddr);
      } break;
      case ElemType::BDEM_DOUBLE: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const double*) srcAddr);
      } break;
      case ElemType::BDEM_STRING: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const bsl::string*) srcAddr);
      } break;
      case ElemType::BDEM_DATETIME: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const bdlt::Datetime*) srcAddr);
      } break;
      case ElemType::BDEM_DATE: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const bdlt::Date*) srcAddr);
      } break;
      case ElemType::BDEM_TIME: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const bdlt::Time*) srcAddr);
      } break;
      case ElemType::BDEM_CHAR_ARRAY: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const bsl::vector<char>*) srcAddr);
      } break;
      case ElemType::BDEM_SHORT_ARRAY: {
        result = Convert::toBdemType(
                                         dstAddr,
                                         dstType,
                                         *(const bsl::vector<short>*) srcAddr);
      } break;
      case ElemType::BDEM_INT_ARRAY: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const bsl::vector<int>*) srcAddr);
      } break;
      case ElemType::BDEM_INT64_ARRAY: {
        result = Convert::toBdemType(
                                         dstAddr,
                                         dstType,
                                         *(const bsl::vector<Int64>*) srcAddr);
      } break;
      case ElemType::BDEM_FLOAT_ARRAY: {
        result = Convert::toBdemType(
                                         dstAddr,
                                         dstType,
                                         *(const bsl::vector<float>*) srcAddr);
      } break;
      case ElemType::BDEM_DOUBLE_ARRAY: {
        result = Convert::toBdemType(
                                        dstAddr,
                                        dstType,
                                        *(const bsl::vector<double>*) srcAddr);
      } break;
      case ElemType::BDEM_STRING_ARRAY: {
        result = Convert::toBdemType(
                                    dstAddr,
                                    dstType,
                                    *(const bsl::vector<bsl::string>*) srcAddr);
      } break;
      case ElemType::BDEM_DATETIME_ARRAY: {
        result = Convert::toBdemType(
                                 dstAddr,
                                 dstType,
                                 *(const bsl::vector<bdlt::Datetime>*) srcAddr);
      } break;
      case ElemType::BDEM_DATE_ARRAY: {
        result = Convert::toBdemType(
                                     dstAddr,
                                     dstType,
                                     *(const bsl::vector<bdlt::Date>*) srcAddr);
      } break;
      case ElemType::BDEM_TIME_ARRAY: {
        result = Convert::toBdemType(
                                     dstAddr,
                                     dstType,
                                     *(const bsl::vector<bdlt::Time>*) srcAddr);
      } break;
      case ElemType::BDEM_LIST: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const List*) srcAddr);
      } break;
      case ElemType::BDEM_TABLE: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const Table*) srcAddr);
      } break;
      case ElemType::BDEM_BOOL: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const bool*) srcAddr);
      } break;
      case ElemType::BDEM_DATETIMETZ: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const bdlt::DatetimeTz*) srcAddr);
      } break;
      case ElemType::BDEM_DATETZ: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const bdlt::DateTz*) srcAddr);
      } break;
      case ElemType::BDEM_TIMETZ: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const bdlt::TimeTz*) srcAddr);
      } break;
      case ElemType::BDEM_BOOL_ARRAY: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const bsl::vector<bool>*) srcAddr);
      } break;
      case ElemType::BDEM_DATETIMETZ_ARRAY: {
        result = Convert::toBdemType(
                               dstAddr,
                               dstType,
                               *(const bsl::vector<bdlt::DatetimeTz>*) srcAddr);
      } break;
      case ElemType::BDEM_DATETZ_ARRAY: {
        result = Convert::toBdemType(
                                   dstAddr,
                                   dstType,
                                   *(const bsl::vector<bdlt::DateTz>*) srcAddr);
      } break;
      case ElemType::BDEM_TIMETZ_ARRAY: {
        result = Convert::toBdemType(
                                   dstAddr,
                                   dstType,
                                   *(const bsl::vector<bdlt::TimeTz>*) srcAddr);
      } break;
      case ElemType::BDEM_CHOICE: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const Choice*) srcAddr);
      } break;
      case ElemType::BDEM_CHOICE_ARRAY: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const ChoiceArray*) srcAddr);
      } break;
      case ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        result = Convert::toBdemType(
                                       dstAddr,
                                       dstType,
                                       *(const ChoiceArrayItem*) srcAddr);
      } break;
      case ElemType::BDEM_ROW: {
        result = Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(const Row*) srcAddr);
      } break;
      case ElemType::BDEM_VOID: {
        // Void-to-void conversion is legal, but void to anything else is not.
        result = ElemType::BDEM_VOID == dstType
                 ? BDEM_SUCCESS
                 : BDEM_FAILURE;
      } break;
      default: {
        result = BDEM_FAILURE;
      } break;
    }

    return result;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
