// bdem_convert.cpp                                                   -*-C++-*-
#include <bdem_convert.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_convert_cpp,"$Id$ $CSID$")

#include <bdem_choice.h>
#include <bdem_choicearray.h>
#include <bdem_choicearrayitem.h>
#include <bdem_list.h>
#include <bdem_row.h>
#include <bdem_table.h>

#include <bdepu_iso8601.h>
#include <bdepu_typesparser.h>
#include <bdepu_typesparserimputil.h>

#include <bdet_datetime.h>
#include <bdet_datetimetz.h>
#include <bdet_date.h>
#include <bdet_datetz.h>
#include <bdet_time.h>
#include <bdet_timetz.h>

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

                        // ------------------
                        // class bdem_Convert
                        // ------------------

// PRIVATE CLASS METHODS
int bdem_Convert::fromString(bool *dstAddr, const char *srcValue)
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

int bdem_Convert::fromString(char *dstAddr, const char *srcValue)
{
    BSLS_ASSERT(dstAddr);

    *dstAddr = srcValue ? *srcValue : 0;

    return BDEM_SUCCESS;
}

int bdem_Convert::fromString(short *dstAddr, const char *srcValue)
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

int bdem_Convert::fromString(int *dstAddr, const char *srcValue)
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

int bdem_Convert::fromString(bsls_PlatformUtil::Int64 *dstAddr,
                             const char               *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    const char *endPtr = 0;
    Int64       result = 0;

    if (bdepu_TypesParser::parseInt64(&endPtr, &result, srcValue, 10)) {
        return BDEM_FAILURE;                                          // RETURN
    }

    *dstAddr = result;
    return BDEM_SUCCESS;
}

int bdem_Convert::fromString(float *dstAddr, const char *srcValue)
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

int bdem_Convert::fromString(double *dstAddr, const char *srcValue)
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

int bdem_Convert::fromString(bdet_Datetime *dstAddr, const char *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    const int wordLen = getWord(&srcValue);
    return bdepu_Iso8601::parse(dstAddr, srcValue, wordLen);
}

int bdem_Convert::fromString(bdet_Date *dstAddr, const char *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    const int wordLen = getWord(&srcValue);
    return bdepu_Iso8601::parse(dstAddr, srcValue, wordLen);
}

int bdem_Convert::fromString(bdet_Time *dstAddr, const char *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    const int wordLen = getWord(&srcValue);
    return bdepu_Iso8601::parse(dstAddr, srcValue, wordLen);
}

int bdem_Convert::fromString(bdet_DatetimeTz *dstAddr, const char *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    const int wordLen = getWord(&srcValue);
    return bdepu_Iso8601::parse(dstAddr, srcValue, wordLen);
}

int bdem_Convert::fromString(bdet_DateTz *dstAddr, const char *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    const int wordLen = getWord(&srcValue);
    return bdepu_Iso8601::parse(dstAddr, srcValue, wordLen);
}

int bdem_Convert::fromString(bdet_TimeTz *dstAddr, const char *srcValue)
{
    if (0 == srcValue || '\0' == srcValue[0]) {
        // Null or empty string.  Do nothing.
        return BDEM_SUCCESS;                                          // RETURN
    }

    BSLS_ASSERT(dstAddr);

    const int wordLen = getWord(&srcValue);
    return bdepu_Iso8601::parse(dstAddr, srcValue, wordLen);
}

// CLASS METHODS
int bdem_Convert::convert(bdem_ElemRef             *dstAddr,
                          const bdem_ConstElemRef&  srcValue)
{
    BSLS_ASSERT(dstAddr);

    const int rc = bdem_Convert::fromBdemType(dstAddr,
                                              srcValue.data(),
                                              srcValue.type());

    if (!rc && srcValue.isBound() && srcValue.isNull()) {
        // Make destination null only if the conversion succeeds.
        dstAddr->makeNull();
    }

    return rc;
}

int bdem_Convert::convert(bdem_ElemRef        *dstAddr,
                          const bdem_ElemRef&  srcValue)
{
    BSLS_ASSERT(dstAddr);

    const int nullnessWord = srcValue.isBound() && srcValue.isNull();
    bdem_ConstElemRef elemRef(srcValue.dataRaw(),
                              srcValue.descriptor(),
                              &nullnessWord,
                              0);

    return bdem_Convert::convert(dstAddr, elemRef);
}

int bdem_Convert::convert(bsl::string *dstAddr, char srcValue)
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

int bdem_Convert::convert(bsl::string *dstAddr, short srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdetu_Unset<short>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }
    char buffer[bdepu_TypesParserImpUtil::BDEPU_MAX_SHORT_STRLEN10 + 1];
    int length = bdepu_TypesParserImpUtil::generateIntRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int bdem_Convert::convert(bsl::string *dstAddr, int srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdetu_Unset<int>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdepu_TypesParserImpUtil::BDEPU_MAX_INT_STRLEN10 + 1];
    int length = bdepu_TypesParserImpUtil::generateIntRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int bdem_Convert::convert(bsl::string              *dstAddr,
                          bsls_PlatformUtil::Int64  srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdetu_Unset<Int64>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdepu_TypesParserImpUtil::BDEPU_MAX_INT64_STRLEN10 + 1];
    int length = bdepu_TypesParserImpUtil::generateInt64Raw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int bdem_Convert::convert(bsl::string *dstAddr, float srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdetu_Unset<float>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdepu_TypesParserImpUtil::BDEPU_MAX_FLOAT_STRLEN10 + 1];
    int length = bdepu_TypesParserImpUtil::generateFloatRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int bdem_Convert::convert(bsl::string *dstAddr, double srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdetu_Unset<double>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdepu_TypesParserImpUtil::BDEPU_MAX_DOUBLE_STRLEN10 + 1];
    int length = bdepu_TypesParserImpUtil::generateDoubleRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int bdem_Convert::convert(bsl::string *dstAddr, long double srcValue)
{
    BSLS_ASSERT(dstAddr);

    char buffer[bdepu_TypesParserImpUtil::BDEPU_MAX_LONGDOUBLE_STRLEN10 + 1];
    int length = bdepu_TypesParserImpUtil::generateLongDoubleRaw(buffer,
                                                                 srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int bdem_Convert::convert(bsl::string *dstAddr, const bdet_Datetime& srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdetu_Unset<bdet_Datetime>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdepu_Iso8601::BDEPU_MAX_DATETIME_STRLEN + 1];
    int  length = bdepu_Iso8601::generateRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int bdem_Convert::convert(bsl::string *dstAddr, const bdet_Date& srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdetu_Unset<bdet_Date>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdepu_Iso8601::BDEPU_MAX_DATETIME_STRLEN + 1];
    int  length = bdepu_Iso8601::generateRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int bdem_Convert::convert(bsl::string *dstAddr, const bdet_Time& srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdetu_Unset<bdet_Time>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdepu_Iso8601::BDEPU_MAX_DATETIME_STRLEN + 1];
    int  length = bdepu_Iso8601::generateRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int bdem_Convert::convert(bsl::string            *dstAddr,
                          const bdet_DatetimeTz&  srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdetu_Unset<bdet_DatetimeTz>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdepu_Iso8601::BDEPU_MAX_DATETIME_STRLEN + 1];
    int  length = bdepu_Iso8601::generateRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int bdem_Convert::convert(bsl::string *dstAddr, const bdet_DateTz& srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdetu_Unset<bdet_DateTz>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdepu_Iso8601::BDEPU_MAX_DATETIME_STRLEN + 1];
    int  length = bdepu_Iso8601::generateRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

int bdem_Convert::convert(bsl::string *dstAddr, const bdet_TimeTz& srcValue)
{
    BSLS_ASSERT(dstAddr);

    if (bdetu_Unset<bdet_TimeTz>::isUnset(srcValue)) {
        dstAddr->clear();
        return BDEM_SUCCESS;                                          // RETURN
    }

    char buffer[bdepu_Iso8601::BDEPU_MAX_DATETIME_STRLEN + 1];
    int  length = bdepu_Iso8601::generateRaw(buffer, srcValue);
    dstAddr->assign(buffer, length);
    return BDEM_SUCCESS;
}

// Note that the following three methods for converting "non-TZ" to "TZ" were
// added in response to DRQS 13447607.

int bdem_Convert::convert(bdet_DateTz *dstAddr, const bdet_Date& srcValue)
{
    BSLS_ASSERT(dstAddr);

    *dstAddr = bdet_DateTz(srcValue, 0);
    return 0;
}

int bdem_Convert::convert(bdet_TimeTz *dstAddr, const bdet_Time& srcValue)
{
    BSLS_ASSERT(dstAddr);

    *dstAddr = bdet_TimeTz(srcValue, 0);
    return 0;
}

int bdem_Convert::convert(bdet_DatetimeTz      *dstAddr,
                          const bdet_Datetime&  srcValue)
{
    BSLS_ASSERT(dstAddr);

    *dstAddr = bdet_DatetimeTz(srcValue, 0);
    return 0;
}

int bdem_Convert::convert(bdem_List *dstAddr, const bdem_Row& srcValue)
{
    BSLS_ASSERT(dstAddr);

    // Convert a 'bdem_Row' to a 'bdem_List'.
    *dstAddr = srcValue;
    return BDEM_SUCCESS;
}

int bdem_Convert::convert(bdem_Row *dstAddr, const bdem_Row& srcValue)
    // Specialization: 'bdem_Row' does not have a public assignment operator,
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

int bdem_Convert::convert(bdem_Row *dstAddr, const bdem_List& srcValue)
{
    BSLS_ASSERT(dstAddr);

    // Convert a 'bdem_List' to a 'bdem_Row'.  Conversion will fail unless the
    // list and row have the same number and types of elements.

    return bdem_Convert::convert(dstAddr, srcValue.row());
}

int bdem_Convert::convert(bdem_Choice                 *dstAddr,
                          const bdem_ChoiceArrayItem&  srcValue)
{
    BSLS_ASSERT(dstAddr);

    // Convert a 'bdem_ChoiceArrayItem' to a 'bdem_Choice'.
    *dstAddr = srcValue;
    return BDEM_SUCCESS;
}

int bdem_Convert::convert(bdem_ChoiceArrayItem        *dstAddr,
                          const bdem_ChoiceArrayItem&  srcValue)
    // Specialization: 'bdem_ChoiceArrayItem' does not have a public
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

int bdem_Convert::convert(bdem_ChoiceArrayItem *dstAddr,
                          const bdem_Choice&    srcValue)
{
    BSLS_ASSERT(dstAddr);

    // Convert a 'bdem_Choice' to a 'bdem_ChoiceArrayItem'.  Conversion will
    // fail unless the 'bdem_Choice' and 'bdem_ChoiceArrayItem' have the same
    // types catalog.

    return bdem_Convert::convert(dstAddr, srcValue.item());
}

int bdem_Convert::toBdemType(void                *dstAddr,
                             bdem_ElemType::Type  dstType,
                             const bdem_ElemRef&  srcValue)
{
    BSLS_ASSERT(dstAddr || bdem_ElemType::BDEM_VOID == dstType);

    return bdem_Convert::convertBdemTypes(dstAddr, dstType,
                                          srcValue.dataRaw(), srcValue.type());
}

int bdem_Convert::fromBdemType(bdem_ElemRef        *dstAddr,
                               const void          *srcAddr,
                               bdem_ElemType::Type  srcType)
{
    BSLS_ASSERT(dstAddr);
    BSLS_ASSERT(srcAddr || bdem_ElemType::BDEM_VOID == srcType);

    const bool                 isDstNull = dstAddr->isBound()
                                        && dstAddr->isNull();
    const bdem_ElemType::Type  dstType   = dstAddr->type();

    // 'dstType' is 'bdem_ElemType::BDEM_VOID' if '*dstAddr' is unbound.

    void *dstValuePtr;

    switch (dstType) {
      case bdem_ElemType::BDEM_CHAR: {
        dstValuePtr = (void *) &dstAddr->theModifiableChar();
      } break;
      case bdem_ElemType::BDEM_SHORT: {
        dstValuePtr = (void *) &dstAddr->theModifiableShort();
      } break;
      case bdem_ElemType::BDEM_INT: {
        dstValuePtr = (void *) &dstAddr->theModifiableInt();
      } break;
      case bdem_ElemType::BDEM_INT64: {
        dstValuePtr = (void *) &dstAddr->theModifiableInt64();
      } break;
      case bdem_ElemType::BDEM_FLOAT: {
        dstValuePtr = (void *) &dstAddr->theModifiableFloat();
      } break;
      case bdem_ElemType::BDEM_DOUBLE: {
        dstValuePtr = (void *) &dstAddr->theModifiableDouble();
      } break;
      case bdem_ElemType::BDEM_STRING: {
        dstValuePtr = (void *) &dstAddr->theModifiableString();
      } break;
      case bdem_ElemType::BDEM_DATETIME: {
        dstValuePtr = (void *) &dstAddr->theModifiableDatetime();
      } break;
      case bdem_ElemType::BDEM_DATE: {
        dstValuePtr = (void *) &dstAddr->theModifiableDate();
      } break;
      case bdem_ElemType::BDEM_TIME: {
        dstValuePtr = (void *) &dstAddr->theModifiableTime();
      } break;
      case bdem_ElemType::BDEM_CHAR_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableCharArray();
      } break;
      case bdem_ElemType::BDEM_SHORT_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableShortArray();
      } break;
      case bdem_ElemType::BDEM_INT_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableIntArray();
      } break;
      case bdem_ElemType::BDEM_INT64_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableInt64Array();
      } break;
      case bdem_ElemType::BDEM_FLOAT_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableFloatArray();
      } break;
      case bdem_ElemType::BDEM_DOUBLE_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableDoubleArray();
      } break;
      case bdem_ElemType::BDEM_STRING_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableStringArray();
      } break;
      case bdem_ElemType::BDEM_DATETIME_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableDatetimeArray();
      } break;
      case bdem_ElemType::BDEM_DATE_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableDateArray();
      } break;
      case bdem_ElemType::BDEM_TIME_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableTimeArray();
      } break;
      case bdem_ElemType::BDEM_LIST: {
        dstValuePtr = (void *) &dstAddr->theModifiableList();
      } break;
      case bdem_ElemType::BDEM_TABLE: {
        dstValuePtr = (void *) &dstAddr->theModifiableTable();
      } break;
      case bdem_ElemType::BDEM_BOOL: {
        dstValuePtr = (void *) &dstAddr->theModifiableBool();
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ: {
        dstValuePtr = (void *) &dstAddr->theModifiableDatetimeTz();
      } break;
      case bdem_ElemType::BDEM_DATETZ: {
        dstValuePtr = (void *) &dstAddr->theModifiableDateTz();
      } break;
      case bdem_ElemType::BDEM_TIMETZ: {
        dstValuePtr = (void *) &dstAddr->theModifiableTimeTz();
      } break;
      case bdem_ElemType::BDEM_BOOL_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableBoolArray();
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableDatetimeTzArray();
      } break;
      case bdem_ElemType::BDEM_DATETZ_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableDateTzArray();
      } break;
      case bdem_ElemType::BDEM_TIMETZ_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableTimeTzArray();
      } break;
      case bdem_ElemType::BDEM_CHOICE: {
        dstValuePtr = (void *) &dstAddr->theModifiableChoice();
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        dstValuePtr = (void *) &dstAddr->theModifiableChoiceArray();
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        dstValuePtr = (void *) &dstAddr->theModifiableChoiceArrayItem();
      } break;
      case bdem_ElemType::BDEM_ROW: {
        dstValuePtr = (void *) &dstAddr->theModifiableRow();
      } break;
      case bdem_ElemType::BDEM_VOID: {
        // Void-to-void conversion is legal, but void to anything else is not.
        return bdem_ElemType::BDEM_VOID == srcType
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

int bdem_Convert::convertBdemTypes(void                *dstAddr,
                                   bdem_ElemType::Type  dstType,
                                   const void          *srcAddr,
                                   bdem_ElemType::Type  srcType)
{
    BSLS_ASSERT(dstAddr || bdem_ElemType::BDEM_VOID == dstType);
    BSLS_ASSERT(srcAddr || bdem_ElemType::BDEM_VOID == srcType);

    int result;

    switch (srcType) {
      case bdem_ElemType::BDEM_CHAR: {
        result = bdem_Convert::toBdemType(dstAddr, dstType, *(char*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_SHORT: {
        result = bdem_Convert::toBdemType(dstAddr, dstType, *(short*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_INT: {
        result = bdem_Convert::toBdemType(dstAddr, dstType, *(int*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_INT64: {
        result = bdem_Convert::toBdemType(dstAddr, dstType, *(Int64*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_FLOAT: {
        result = bdem_Convert::toBdemType(dstAddr, dstType, *(float*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DOUBLE: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(double*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_STRING: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bsl::string*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DATETIME: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bdet_Datetime*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DATE: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bdet_Date*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_TIME: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bdet_Time*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_CHAR_ARRAY: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bsl::vector<char>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_SHORT_ARRAY: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bsl::vector<short>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_INT_ARRAY: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bsl::vector<int>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_INT64_ARRAY: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bsl::vector<Int64>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_FLOAT_ARRAY: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bsl::vector<float>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DOUBLE_ARRAY: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bsl::vector<double>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_STRING_ARRAY: {
        result = bdem_Convert::toBdemType(
                                         dstAddr,
                                         dstType,
                                         *(bsl::vector<bsl::string>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DATETIME_ARRAY: {
        result = bdem_Convert::toBdemType(
                                      dstAddr,
                                      dstType,
                                      *(bsl::vector<bdet_Datetime>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DATE_ARRAY: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bsl::vector<bdet_Date>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_TIME_ARRAY: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bsl::vector<bdet_Time>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_LIST: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bdem_List*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_TABLE: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bdem_Table*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_BOOL: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bool*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bdet_DatetimeTz*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DATETZ: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bdet_DateTz*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_TIMETZ: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bdet_TimeTz*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_BOOL_ARRAY: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bsl::vector<bool>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ_ARRAY: {
        result = bdem_Convert::toBdemType(
                                    dstAddr,
                                    dstType,
                                    *(bsl::vector<bdet_DatetimeTz>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DATETZ_ARRAY: {
        result = bdem_Convert::toBdemType(
                                        dstAddr,
                                        dstType,
                                        *(bsl::vector<bdet_DateTz>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_TIMETZ_ARRAY: {
        result = bdem_Convert::toBdemType(
                                        dstAddr,
                                        dstType,
                                        *(bsl::vector<bdet_TimeTz>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_CHOICE: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bdem_Choice*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bdem_ChoiceArray*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bdem_ChoiceArrayItem*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_ROW: {
        result = bdem_Convert::toBdemType(dstAddr,
                                          dstType,
                                          *(bdem_Row*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_VOID: {
        // Void-to-void conversion is legal, but void to anything else is not.
        result = bdem_ElemType::BDEM_VOID == dstType
                 ? BDEM_SUCCESS
                 : BDEM_FAILURE;
      } break;
      default: {
        result = BDEM_FAILURE;
      } break;
    }

    return result;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
