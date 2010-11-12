// bdempu_aggregate.cpp                                               -*-C++-*-
#include <bdempu_aggregate.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdempu_aggregate_cpp,"$Id$ $CSID$")

#include <bdempu_elemtype.h>
#include <bdepu_ntypesparser.h>
#include <bdem_aggregate.h>
#include <bsls_assert.h>

#include <bsl_vector.h>

namespace BloombergLP {

struct bdempu_Aggregate_Assertions
{
    // an 'int' array is used to store 'bdem_ElemType::Type' values
    char assertion1[sizeof(int) == sizeof(bdem_ElemType::Type)];
};

                     // =======================
                     // struct bdempu_Aggregate
                     // =======================

enum { BDEMPU_SUCCESS = 0, BDEMPU_FAILURE = 1 };

                     // *** PARSE FUNCTIONS ***

int bdempu_Aggregate::parseList(const char **endPos,
                                bdem_List   *result,
                                const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(endPos, inputString);
    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '{')) {
        if (0 == bdepu_ParserImpUtil::parseNull(endPos, *endPos)) {
            result->removeAll();
            return BDEMPU_SUCCESS;
        }
        return BDEMPU_FAILURE;
    }

    bdem_ElemType::Type resType;
    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    switch(**endPos) {
      case '\0': {
        return BDEMPU_FAILURE;
      }
      case '}': {
        // It is an empty list
        result->removeAll();
        ++*endPos;
        return BDEMPU_SUCCESS;
      }
      default: {
        result->removeAll();
        if (bdempu_ElemType::parseElemType(endPos, &resType, *endPos)) {
            return BDEMPU_FAILURE;
        }
        bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
        if (parseListElement(endPos, result, resType, *endPos)) {
            return BDEMPU_FAILURE;
        }
      }
    }

    while (1) {
        bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
        switch(**endPos) {
          case '}': {
            ++*endPos;
            return BDEMPU_SUCCESS;
          }
          case '\0': {
            return BDEMPU_FAILURE;
          }
          default: {
            if (',' == **endPos) {  // handle *optional* ',' separator
                ++*endPos;
                bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
            }
            if (bdempu_ElemType::parseElemType(endPos, &resType, *endPos)) {
                return BDEMPU_FAILURE;
            }
            bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
            if (parseListElement(endPos, result, resType, *endPos)) {
                return BDEMPU_FAILURE;
            }
          } break;
        }
    }
}

int bdempu_Aggregate::parseListElement(
                                    const char                 **endPos,
                                    bdem_List                   *result,
                                    const bdem_ElemType::Type&   elemType,
                                    const char                  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    *endPos = inputString;
    switch (elemType) {
      case bdem_ElemType::BDEM_CHAR:
      {
        char res;
        if (bdepu_NTypesParser::parseChar(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendChar(res);
      } break;
      case bdem_ElemType::BDEM_SHORT:
      {
        short res = 0;
        if (bdepu_NTypesParser::parseShort(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendShort(res);
      } break;
      case bdem_ElemType::BDEM_INT:
      {
        int res = 0;
        if (bdepu_NTypesParser::parseInt(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendInt(res);
      } break;
      case bdem_ElemType::BDEM_INT64:
      {
        bsls_PlatformUtil::Int64 res;
        if (bdepu_NTypesParser::parseInt64(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendInt64(res);
      } break;
      case bdem_ElemType::BDEM_FLOAT:
      {
        float res;
        if (bdepu_NTypesParser::parseFloat(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendFloat(res);
      } break;
      case bdem_ElemType::BDEM_DOUBLE:
      {
        double res;
        if (bdepu_NTypesParser::parseDouble(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendDouble(res);
      } break;
      case bdem_ElemType::BDEM_STRING:
      {
        bsl::string res;
        if (bdepu_NTypesParser::parseString(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendString(bsl::string(res.c_str()));  // TBD efficiency
      } break;
      case bdem_ElemType::BDEM_DATE:
      {
        bdet_Date res;
        if (bdepu_NTypesParser::parseDate(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendDate(res);
      } break;
      case bdem_ElemType::BDEM_DATETIME:
      {
        bdet_Datetime res;
        if (bdepu_NTypesParser::parseDatetime(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendDatetime(res);
      } break;
      case bdem_ElemType::BDEM_TIME:
      {
        bdet_Time res;
        if (bdepu_NTypesParser::parseTime(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendTime(res);
      } break;
      case bdem_ElemType::BDEM_CHAR_ARRAY:
      {
        bsl::vector<char> res;
        if (bdepu_NTypesParser::parseCharArray(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendCharArray(res);
      } break;
      case bdem_ElemType::BDEM_SHORT_ARRAY:
      {
        bsl::vector<short> res;
        if (bdepu_NTypesParser::parseShortArray(endPos, &res, *endPos) != 0) {
            return BDEMPU_FAILURE;
        }
        result->appendShortArray(res);
      } break;
      case bdem_ElemType::BDEM_INT_ARRAY:
      {
        bsl::vector<int> res;
        if (bdepu_NTypesParser::parseIntArray(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendIntArray(res);
      } break;
      case bdem_ElemType::BDEM_INT64_ARRAY:
      {
        bsl::vector<bsls_PlatformUtil::Int64> res;
        if (bdepu_NTypesParser::parseInt64Array(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendInt64Array(res);
      } break;
      case bdem_ElemType::BDEM_FLOAT_ARRAY:
      {
        bsl::vector<float> res;
        if (bdepu_NTypesParser::parseFloatArray(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendFloatArray(res);
      } break;
      case bdem_ElemType::BDEM_DOUBLE_ARRAY:
      {
        bsl::vector<double> res;
        if (bdepu_NTypesParser::parseDoubleArray(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendDoubleArray(res);
      } break;
      case bdem_ElemType::BDEM_STRING_ARRAY:
      {
        bsl::vector<bsl::string> res;
        if (bdepu_NTypesParser::parseStringArray(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendStringArray(res);
      } break;
      case bdem_ElemType::BDEM_DATETIME_ARRAY:
      {
        bsl::vector<bdet_Datetime> res;
        if (bdepu_NTypesParser::parseDatetimeArray(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendDatetimeArray(res);
      } break;
      case bdem_ElemType::BDEM_DATE_ARRAY:
      {
        bsl::vector<bdet_Date> res;
        if (bdepu_NTypesParser::parseDateArray(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendDateArray(res);
      } break;
      case bdem_ElemType::BDEM_TIME_ARRAY:
      {
        bsl::vector<bdet_Time> res;
        if (bdepu_NTypesParser::parseTimeArray(endPos, &res, *endPos)) {
            return BDEMPU_FAILURE;
        }
        result->appendTimeArray(res);
      } break;
      case bdem_ElemType::BDEM_LIST:
      {
        bdem_List res;
        if (parseList(endPos, &res, *endPos) != 0) {
            return BDEMPU_FAILURE;
        }
        result->appendList(res);
      } break;
      case bdem_ElemType::BDEM_TABLE:
      {
        bdem_Table res;
        if (parseTable(endPos, &res, *endPos) != 0) {
            return BDEMPU_FAILURE;
        }
        result->appendTable(res);
      } break;
      default:
        return BDEMPU_FAILURE;
    }
    return BDEMPU_SUCCESS;
}

int bdempu_Aggregate::parseTable(const char **endPos,
                                 bdem_Table  *result,
                                 const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(endPos, inputString);
    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '{')) {
        if (0 == bdepu_ParserImpUtil::parseNull(endPos, *endPos)) {
            result->removeAll();
            return BDEMPU_SUCCESS;
        }
        return BDEMPU_FAILURE;
    }
    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);

    // Parse the COLUMN_TYPES
    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '[')) {
        return BDEMPU_FAILURE;
    }
    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    result->removeAll();

    switch(**endPos) {
      case '\0': {
        return BDEMPU_FAILURE;
      }
      case ']': {
        // It is an empty table
        ++*endPos;

        bdem_List list;
        bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
        while (bdepu_ParserImpUtil::skipRequiredToken(
                                  endPos, *endPos, '{') == 0) {
            bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
            if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '}')) {
                return BDEMPU_FAILURE;
            }
            result->appendRow(list);
        } //end while

        if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '}')) {
            return BDEMPU_FAILURE;
        }
        return BDEMPU_SUCCESS;
      }
    }

    // If we are here, then the table has at least one column

    // Parse list of element types

    bsl::vector<int> columnTypeArray;  // using 'int' for 'bdem_ElemType::Type'
    bdem_ElemType::Type colType;

    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    if (bdempu_ElemType::parseElemType(endPos, &colType, *endPos)) {
        return BDEMPU_FAILURE;
    }
    columnTypeArray.push_back(colType);

    int done = 0;
    while (!done) {
        bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
        switch(**endPos) {
          case ']': {
            // End of element type list
            ++*endPos;
            done = 1;
          } break;
          case '\0': {
            return BDEMPU_FAILURE;
          }
          default: {
            // Add element
            bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
            if (bdempu_ElemType::parseElemType(endPos, &colType, *endPos)) {
                return BDEMPU_FAILURE;
            }
            columnTypeArray.push_back((int)colType);
          } break;
        }
    }

    result->reset((bdem_ElemType::Type *)&columnTypeArray.front(),
                   columnTypeArray.size());
    int len = columnTypeArray.size();

    bdem_List list;
    // Parse ROW
    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    while (bdepu_ParserImpUtil::skipRequiredToken(
                                  endPos, *endPos, '{') == 0) {
        list.removeAll();
        bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
        if (parseListElement(endPos,
                             &list,
                             (bdem_ElemType::Type)columnTypeArray[0],
                             *endPos)) {
            return BDEMPU_FAILURE;
        }
        for (int i = 1; i < len; i++) {
            bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
            if (parseListElement(endPos,
                                 &list,
                                 (bdem_ElemType::Type)columnTypeArray[i],
                                 *endPos)) {
                return BDEMPU_FAILURE;
            }
        }
        bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
        if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '}')) {
            return BDEMPU_FAILURE;
        }
        bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
        result->appendRow(list);
    }

    // Parse end of table
    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '}')) {
        return BDEMPU_FAILURE;
    }
    return BDEMPU_SUCCESS;
}

                     // *** GENERATE FUNCTIONS ***

void bdempu_Aggregate::generateListRaw(bsl::vector<char> *buffer,
                                       const bdem_List&   value,
                                       int                level,
                                       int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,level, spacesPerLevel);
    }
    else {
        level *= -1;
    }

    buffer->push_back('{');
    buffer->push_back('\n');

    const bdem_Row& row = value.row();
    int len = value.length();
    if (!len) {
        bdepu_ParserImpUtil::generateIndentation(buffer,level, spacesPerLevel);
        buffer->push_back('}');
        return;
    }

    int eltLevel = level + 1;
    bdempu_ElemType::generateElemTypeRaw(buffer, value.elemType(0), eltLevel,
                                                               spacesPerLevel);
    buffer->push_back(' ');
    generateRowElement(buffer, row, 0, value.elemType(0), eltLevel * -1,
                                                               spacesPerLevel);
    for (int i = 1; i < len; i++) {
        buffer->push_back(',');
        buffer->push_back('\n');
        bdempu_ElemType::generateElemTypeRaw(buffer, value.elemType(i),
                                                     eltLevel, spacesPerLevel);
        buffer->push_back(' ');
        generateRowElement(buffer, row, i, value.elemType(i), eltLevel * -1,
                                                               spacesPerLevel);
    }

    buffer->push_back('\n');
    bdepu_ParserImpUtil::generateIndentation(buffer, level, spacesPerLevel);
    buffer->push_back('}');
}

void bdempu_Aggregate::generateRowElement(
                                  bsl::vector<char>          *buffer,
                                  const bdem_Row&             value,
                                  int                         index,
                                  const bdem_ElemType::Type&  elemType,
                                  int                         level,
                                  int                         spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    switch (elemType) {
      case bdem_ElemType::BDEM_CHAR: {
          bdepu_NTypesParser::generateCharRaw(buffer, value.theChar(index),
                                                        level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_SHORT: {
          bdepu_NTypesParser::generateShortRaw(buffer, value.theShort(index),
                                                        level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_INT: {
          bdepu_NTypesParser::generateIntRaw(buffer, value.theInt(index),
                                                        level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_INT64: {
          bdepu_NTypesParser::generateInt64Raw(buffer, value.theInt64(index),
                                                        level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_FLOAT: {
          bdepu_NTypesParser::generateFloatRaw(buffer, value.theFloat(index),
                                                        level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_DOUBLE: {
          bdepu_NTypesParser::generateDoubleRaw(buffer,value.theDouble(index),
                                                        level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_STRING: {
          bdepu_NTypesParser::generateStringRaw(buffer,
                                                value.theString(index).data(),
                                                level,
                                                spacesPerLevel);
          buffer->push_back(' ');
      } break;
      case bdem_ElemType::BDEM_DATETIME: {
          bdepu_NTypesParser::generateDatetimeRaw(buffer,
                              value.theDatetime(index), level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_DATE: {
          bdepu_NTypesParser::generateDateRaw(buffer, value.theDate(index),
                                                        level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_TIME: {
          bdepu_NTypesParser::generateTimeRaw(buffer, value.theTime(index),
                                                        level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_CHAR_ARRAY: {
          bdepu_NTypesParser::generateCharArrayRaw(buffer,
                             value.theCharArray(index), level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_SHORT_ARRAY: {
          bdepu_NTypesParser::generateShortArrayRaw(buffer,
                            value.theShortArray(index), level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_INT_ARRAY: {
          bdepu_NTypesParser::generateIntArrayRaw(buffer,
                              value.theIntArray(index), level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_INT64_ARRAY: {
          bdepu_NTypesParser::generateInt64ArrayRaw(buffer,
                            value.theInt64Array(index), level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_FLOAT_ARRAY: {
          bdepu_NTypesParser::generateFloatArrayRaw(buffer,
                            value.theFloatArray(index), level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_DOUBLE_ARRAY: {
          bdepu_NTypesParser::generateDoubleArrayRaw(buffer,
                           value.theDoubleArray(index), level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_STRING_ARRAY: {
          bdepu_NTypesParser::generateStringArrayRaw(buffer,
                           value.theStringArray(index), level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_DATETIME_ARRAY: {
          bdepu_NTypesParser::generateDatetimeArrayRaw(buffer,
                         value.theDatetimeArray(index), level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_DATE_ARRAY: {
          bdepu_NTypesParser::generateDateArrayRaw(buffer,
                             value.theDateArray(index), level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_TIME_ARRAY: {
          bdepu_NTypesParser::generateTimeArrayRaw(buffer,
                             value.theTimeArray(index), level, spacesPerLevel);
      } break;
      case bdem_ElemType::BDEM_LIST: {
          const bdem_List &theList = value.theList(index);
          if (0 == theList.length()) {
              bdepu_ParserImpUtil::generateIndentation(buffer, level,
                                                               spacesPerLevel);
              bdepu_ParserImpUtil::generateNull(buffer);
          }
          else {
              generateListRaw(buffer, value.theList(index), level,
                                                               spacesPerLevel);
          }
      } break;
      case bdem_ElemType::BDEM_TABLE: {
          const bdem_Table &theTable = value.theTable(index);
          if ((0 == theTable.numColumns()) && (0 == theTable.numColumns())) {
              bdepu_ParserImpUtil::generateIndentation(buffer, level,
                                                               spacesPerLevel);
              bdepu_ParserImpUtil::generateNull(buffer);
          }
          else {
              generateTableRaw(buffer, theTable, level, spacesPerLevel);
          }
      } break;
      // TBD
      // The following catches 'BOOL', 'DATETIMETZ', etc.  This package is
      // deprecated, so it is unlikely to be extended to support the new types.
      default: {
        BSLS_ASSERT(0);  // Should *not* reach here!
      } break;
    }
}

void bdempu_Aggregate::generateTableRaw(bsl::vector<char>  *buffer,
                                        const bdem_Table&   value,
                                        int                 level,
                                        int                 spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,level, spacesPerLevel);
    }
    else {
        level *= -1;
    }

    buffer->push_back('{');
    buffer->push_back('\n');
    int innerLevel = level + 1;
    bdepu_ParserImpUtil::generateIndentation(buffer,innerLevel,spacesPerLevel);
    buffer->push_back('[');
    int eltLevel = innerLevel + 1;

    int len = value.numColumns();
    if (len) {
        buffer->push_back('\n');
        bdempu_ElemType::generateElemTypeRaw(buffer, value.columnType(0),
                                                     eltLevel, spacesPerLevel);
    }
    for (int i = 1; i < len; i++) {
        buffer->push_back('\n');
        bdempu_ElemType::generateElemTypeRaw(buffer, value.columnType(i),
                                                     eltLevel, spacesPerLevel);
    }

    // Append closing square brace for element type list
    buffer->push_back('\n');
    bdepu_ParserImpUtil::generateIndentation(buffer,innerLevel,spacesPerLevel);
    buffer->push_back(']');
    buffer->push_back('\n');

    // Append rows
    int numRows = value.numRows();
    for (int i = 0; i < numRows; i++) {
        bdepu_ParserImpUtil::generateIndentation(buffer, innerLevel,
                                                               spacesPerLevel);
        buffer->push_back('{');
        buffer->push_back('\n');
        const bdem_Row& row = value[i];

        // Append first element in the row
        generateRowElement(buffer, row, 0, value.columnType(0), eltLevel,
                                                               spacesPerLevel);

        for (int j = 1; j < len; j++) {
            buffer->push_back('\n');
            // Generate the rest of the row elements
            generateRowElement(buffer, row, j, value.columnType(j), eltLevel,
                                                               spacesPerLevel);
        }
        buffer->push_back('\n');
        bdepu_ParserImpUtil::generateIndentation(buffer, innerLevel,
                                                               spacesPerLevel);
        buffer->push_back('}');
        buffer->push_back('\n');
    }

    bdepu_ParserImpUtil::generateIndentation(buffer, level, spacesPerLevel);
    buffer->push_back('}');
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
