// bdempu_schemaaggregate.cpp    -*-C++-*-

#include <bdempu_schemaaggregate.h>

#include <bdempu_aggregate.h>
#include <bdempu_elemtype.h>
#include <bdempu_schema.h>

#include <bdepu_ntypesparser.h>
#include <bdepu_parserimputil.h>
#include <bdepu_typesparser.h>
#include <bdepu_typesparserimputil.h>

#include <bdem_aggregate.h>
#include <bdem_schema.h>

#include <bdes_assert.h>

namespace BloombergLP {

struct bdempu_SchemaAggregate_Assertions
{
    // an 'int' array is used to store 'bdem_ElemType::Type' values
    char assertion1[sizeof(int) == sizeof(bdem_ElemType::Type)];
};

enum { SUCCESS = 0, FAILURE = 1 };

                    // ======================
                    // local helper functions
                    // ======================

int isIdentifier(const char *result) {

    BDE_ASSERT_CPP(result);

    // Check first character
    char nextChar = result[0];
    if (!((nextChar >= 'A' && nextChar <= 'Z') ||
          (nextChar >= 'a' && nextChar <= 'z') || nextChar == '_'))
        return 0;

    // And then all the rest
    for (std::size_t i = 1; i < std::strlen(result); ++i) {
        nextChar = result[i];
        if (! ((nextChar >= 'A' && nextChar <= 'Z') ||
               (nextChar >= 'a' && nextChar <= 'z') ||
               (nextChar >= '0' && nextChar <= '9') ||
                nextChar == '_')) {
            return 0;
        }
    }
    return 1;
}

static void generateValuePrefix(std::vector<char>      *buffer,
                                const bdem_RecordDef&  constraint,
                                int                    index,
                                int                    level,
                                int                    spacesPerLevel)
{
    BDE_ASSERT_CPP(buffer);

    // generate element name
    const char *name = constraint.fieldName(index);
    if (name) {
        bdepu_ParserImpUtil::generateIndentation(buffer,level, spacesPerLevel);
        int hasNonIdentifierChars = !isIdentifier(name);
        if (hasNonIdentifierChars) {
            buffer->push_back('\"');
        }
        const int len = std::strlen(name);
        for (int i = 0; i < len; ++i) {
            buffer->push_back(name[i]);
        }
        if (hasNonIdentifierChars) {
            buffer->push_back('\"');
        }
    }
    else {
        bdepu_TypesParser::generateIntegerLocatorRaw(buffer, index, level,
                                                               spacesPerLevel);
    }
    buffer->push_back(' ');
    buffer->push_back('=');
    buffer->push_back(' ');
}

                    // =============================
                    // struct bdempu_SchemaAggregate
                    // =============================

                    // *** PARSING FUNCTIONS ***

int bdempu_SchemaAggregate::parseConstrainedTable(
                                         const char            **endPos,
                                         bdem_Table             *result,
                                         const char             *inputString,
                                         const bdem_RecordDef&   constraint)
{
    BDE_ASSERT_CPP(endPos);
    BDE_ASSERT_CPP(result);
    BDE_ASSERT_CPP(inputString);

    result->removeAll();

    // Initialize the table with types matching the constraint
    std::vector<bdem_ElemType::Type> columnTypeArray;
    int len = constraint.length();
    for (int i = 0; i < len; ++i) {
        columnTypeArray.push_back(constraint.field(i).elemType());
    }
    result->reset(columnTypeArray);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == bdepu_ParserImpUtil::parseNull(endPos, inputString)) {
        return SUCCESS;
    }

    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '{')) {
        return FAILURE;
    }

    // Parse <COMPLIANT_RECORD_ARRAY>
    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    switch (**endPos) {
      case '{': {
        while ('{' == **endPos ) {
            // parse <COMPLIANT_RECORD>*
            bdem_List res;
            res.reset(columnTypeArray);

            bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
            if (parseCompliantRecord(endPos, &res, *endPos, constraint)) {
                return FAILURE;
            }
            result->appendRow(res);
            bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
        }
      } break;
      case '[': {
        // parse <COLUMN_FORMAT>

        // parse <COLUMN_MAP>
        if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '[')) {
            return FAILURE;
        }

        std::vector<int> indexArray;
        bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
        if (0 == bdepu_ParserImpUtil::skipRequiredToken(endPos,
                                                        *endPos, ']')) {
            // <COLUMN_MAP> is empty
        }
        else {
            while (1) {
                int index;
                if (parseNameOrIntegerLocator(endPos, &index, constraint,
                                                              *endPos)) {
                    return FAILURE;
                }
                indexArray.push_back(index);

                bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
                if (']' == **endPos) {
                    ++*endPos;
                    break;
                } // end if
            }  // end while(1)
        } // end else

        int len = indexArray.size();
        // Parse <DATA_ROW>*
        while (1) {
            // parse <DATA_ROW>
            bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
            if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '{')) {
                break;
            }
            bdem_List res;
            res.reset(columnTypeArray);
            for (int i = 0; i < len - 1; ++i) {
                // Parse <VALUE> and insert it in its place in the row
                if (parseTableRowElement(endPos, &res, indexArray[i],
                                         constraint, *endPos)) {
                    return FAILURE;
                }
            }

            // parse the last element
            if (parseTableRowElement(endPos, &res, indexArray[len - 1],
                                            constraint, *endPos)) {
                return FAILURE;
            }

            result->appendRow(res);

            bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
            if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '}')) {
                return FAILURE;
            }
        }
      } break;
      case '}': {
        // Empty table
        ++*endPos;
        return SUCCESS;
      }
      default: {
        return FAILURE;
      }
    }

    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '}')) {
        return FAILURE;
    }

    return SUCCESS;
}

int bdempu_SchemaAggregate::parseNameOrIntegerLocator(
                                           const char          **endPos,
                                           int                  *index,
                                           const bdem_RecordDef& constraint,
                                           const char           *inputString)
{
    BDE_ASSERT_CPP(endPos);
    BDE_ASSERT_CPP(index);
    BDE_ASSERT_CPP(inputString);

    if (bdepu_TypesParser::parseIntegerLocator(endPos, index, inputString)) {
        // This could be <NAME>
        std::string stringLocator;

        const char *startPos = *endPos;

        if (bdepu_TypesParserImpUtil::parseQuotedString(endPos, &stringLocator,
                                                                   startPos)) {
           if (bdepu_ParserImpUtil::parseIdentifier(endPos, &stringLocator,
                                                                   startPos)) {
                return FAILURE;
           }
        }

        *index = constraint.fieldIndex(stringLocator.c_str());
        if (0 > *index) {
            --*endPos;
            return FAILURE;
        }
    }
    else {
        if ((*index < 0) || (*index >= constraint.length())) {
            --*endPos;
            return FAILURE;
        }
    }
    return SUCCESS;
}

int bdempu_SchemaAggregate::parseNameOrIntegerLocator(
                                              const char       **endPos,
                                              int               *index,
                                              const bdem_Schema& constraint,
                                              const char        *inputString)
{
    BDE_ASSERT_CPP(endPos);
    BDE_ASSERT_CPP(index);
    BDE_ASSERT_CPP(inputString);

    if (bdepu_TypesParser::parseIntegerLocator(endPos, index, inputString)) {
        // This could be <NAME>
        std::string stringLocator;
        const char *startPos = *endPos;
        if (bdepu_TypesParserImpUtil::parseQuotedString(endPos, &stringLocator,
                                                                   startPos)) {
           if (bdepu_ParserImpUtil::parseIdentifier(endPos, &stringLocator,
                                                                   startPos)) {
                return FAILURE;
           }
        }

        *index = constraint.recordIndex(stringLocator.c_str());
        if (0 > *index) {
            return FAILURE;
        }
    }
    else {
        if ((*index < 0) || (*index >= constraint.length())) {
            return FAILURE;
        }
    }
    return SUCCESS;
}

int bdempu_SchemaAggregate::parseTableRowElement(
                                           const char          **endPos,
                                           bdem_List            *result,
                                           int                   index,
                                           const bdem_RecordDef& constraint,
                                           const char           *inputString)
{
    BDE_ASSERT_CPP(endPos);
    BDE_ASSERT_CPP(result);
    BDE_ASSERT_CPP(inputString);

    *endPos = inputString;
    switch (constraint.field(index).elemType()) {
      case bdem_ElemType::CHAR:
      {
        return( bdepu_NTypesParser::parseChar(endPos,
                &result->theChar(index), *endPos ));
      }
      case bdem_ElemType::SHORT:
      {
        return( bdepu_NTypesParser::parseShort(endPos,
                &result->theShort(index), *endPos ));
      }
      case bdem_ElemType::INT:
      {
        return( bdepu_NTypesParser::parseInt(endPos,
                &result->theInt(index), *endPos ));
      }
      case bdem_ElemType::INT64:
      {
        return( bdepu_NTypesParser::parseInt64(endPos,
                &result->theInt64(index), *endPos ));
      }
      case bdem_ElemType::FLOAT:
      {
        return( bdepu_NTypesParser::parseFloat(endPos,
                &result->theFloat(index), *endPos ));
      }
      case bdem_ElemType::DOUBLE:
      {
        return( bdepu_NTypesParser::parseDouble(endPos,
                &result->theDouble(index), *endPos ));
      }
      case bdem_ElemType::STRING:
      {
        return( bdepu_NTypesParser::parseString(endPos,
                &result->theString(index), *endPos ));
      }
      case bdem_ElemType::DATE:
      {
        return( bdepu_NTypesParser::parseDate(endPos,
                &result->theDate(index), *endPos ));
      }
      case bdem_ElemType::DATETIME:
      {
        return( bdepu_NTypesParser::parseDatetime(endPos,
                &result->theDatetime(index), *endPos ));
      }
      case bdem_ElemType::TIME:
      {
        return( bdepu_NTypesParser::parseTime(endPos,
                &result->theTime(index), *endPos ));
      }
      case bdem_ElemType::CHAR_ARRAY:
      {
        return( bdepu_NTypesParser::parseCharArray(endPos,
                &result->theCharArray(index), *endPos ));
      }
      case bdem_ElemType::SHORT_ARRAY:
      {
        return( bdepu_NTypesParser::parseShortArray(endPos,
                &result->theShortArray(index), *endPos ));
      }
      case bdem_ElemType::INT_ARRAY:
      {
        return( bdepu_NTypesParser::parseIntArray(endPos,
                &result->theIntArray(index), *endPos ));
      }
      case bdem_ElemType::INT64_ARRAY:
      {
        return( bdepu_NTypesParser::parseInt64Array(endPos,
                &result->theInt64Array(index), *endPos ));
      }
      case bdem_ElemType::FLOAT_ARRAY:
      {
        return( bdepu_NTypesParser::parseFloatArray(endPos,
                &result->theFloatArray(index), *endPos ));
      }
      case bdem_ElemType::DOUBLE_ARRAY:
      {
        return( bdepu_NTypesParser::parseDoubleArray(endPos,
                &result->theDoubleArray(index), *endPos ));
      }
      case bdem_ElemType::STRING_ARRAY:
      {
        return( bdepu_NTypesParser::parseStringArray(endPos,
                &result->theStrArray(index), *endPos ));
      }
      case bdem_ElemType::DATETIME_ARRAY:
      {
        return( bdepu_NTypesParser::parseDatetimeArray(endPos,
                &result->theDatetimeArray(index), *endPos ));
      }
      case bdem_ElemType::DATE_ARRAY:
      {
        return( bdepu_NTypesParser::parseDateArray(endPos,
                &result->theDateArray(index), *endPos ));
      }
      case bdem_ElemType::TIME_ARRAY:
      {
        return( bdepu_NTypesParser::parseTimeArray(endPos,
                &result->theTimeArray(index), *endPos ));
      }
      case bdem_ElemType::LIST:
      {
        const bdem_RecordDef *listConstraint =
                                  constraint.field(index).constraint();

        if (listConstraint) {
            if (bdempu_SchemaAggregate::parseConstrainedList(endPos,
                    &result->theList(index), *endPos, *listConstraint)) {
                return FAILURE;
            }
        }
        else {
            if (bdempu_Aggregate::parseList(endPos,
                    &result->theList(index), *endPos) != 0) {
                return FAILURE;
            }
        }
      } break;
      case bdem_ElemType::TABLE:
      {
        const bdem_RecordDef *tableConstraint =
                                   constraint.field(index).constraint();

        if (tableConstraint) {
            if (bdempu_SchemaAggregate::parseConstrainedTable(endPos,
                    &result->theTable(index), *endPos, *tableConstraint)) {
                return FAILURE;
            }
        }
        else {
            if (bdempu_Aggregate::parseTable(endPos,
                    &result->theTable(index), *endPos) != 0) {
                return FAILURE;
            }
        }
      } break;
      default:
        return FAILURE;
    }
    return SUCCESS;
}

                       // *** GENERATING FUNCTIONS ***

void bdempu_SchemaAggregate::generateConstrainedRowElement(
    std::vector<char>     *buffer,
    const bdem_Row&       value,
    const bdem_RecordDef& constraint,
    int                   index,
    int                   level,
    int                   spacesPerLevel)
{
    BDE_ASSERT_CPP(buffer);

    int negLevel = level * -1;

    switch (value.elemType(index)) {
      case bdem_ElemType::CHAR: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint,index, level, spacesPerLevel);
          bdepu_TypesParser::generateCharRaw(buffer, value.theChar(index),
                                                     negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::SHORT: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateShortRaw(buffer, value.theShort(index),
                                                     negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::INT: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateIntRaw(buffer, value.theInt(index),
                                                     negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::INT64: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateInt64Raw(buffer, value.theInt64(index),
                                                     negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::FLOAT: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateFloatRaw(buffer, value.theFloat(index),
                                                     negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::DOUBLE: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateDoubleRaw(buffer, value.theDouble(index),
                                                     negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::STRING: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateStringRaw(buffer,
                                               value.theString(index).data(),
                                               negLevel,
                                               spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::DATETIME: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateDatetimeRaw(buffer,
                           value.theDatetime(index), negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::DATE: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateDateRaw(buffer, value.theDate(index),
                                                     negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::TIME: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateTimeRaw(buffer, value.theTime(index),
                                                     negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::CHAR_ARRAY: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateCharArrayRaw(buffer,
                          value.theCharArray(index), negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::SHORT_ARRAY: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateShortArrayRaw(buffer,
                         value.theShortArray(index), negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::INT_ARRAY: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateIntArrayRaw(buffer,
                           value.theIntArray(index), negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::INT64_ARRAY: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateInt64ArrayRaw(buffer,
                         value.theInt64Array(index), negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::FLOAT_ARRAY: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateFloatArrayRaw(buffer,
                         value.theFloatArray(index), negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::DOUBLE_ARRAY: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateDoubleArrayRaw(buffer,
                        value.theDoubleArray(index), negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::STRING_ARRAY: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateStringArrayRaw(buffer,
              value.theStrArray(index), negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::DATETIME_ARRAY: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateDatetimeArrayRaw(buffer,
              value.theDatetimeArray(index), negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::DATE_ARRAY: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateDateArrayRaw(buffer,
              value.theDateArray(index), negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::TIME_ARRAY: {
          if (value[index].isUnset()) {
              return;
          }
          generateValuePrefix(buffer, constraint, index,level, spacesPerLevel);
          bdepu_TypesParser::generateTimeArrayRaw(buffer,
              value.theTimeArray(index), negLevel, spacesPerLevel);
          buffer->push_back('\n');
      } break;
      case bdem_ElemType::LIST: {
        // Don't generate "NULL" for fields in empty constrained lists
        if (constraint.field(index).constraint()) {
            // This is a constrained list
              const int len0 = buffer->size(); // If list is empy, we use it
                                               // undo the changes to the
                                               // buffer
              generateValuePrefix(buffer, constraint, index, level,
                                                               spacesPerLevel);
              const int len = buffer->size();  // We use it to detect if the
                                               // list was generated
              bdempu_SchemaAggregate::generateConstrainedRow(buffer,
                  value.theList(index).row(),
                  *constraint.field(index).constraint(),
                  negLevel, spacesPerLevel);

              if (len == (int)buffer->size()) {
                  // List wasn't generated, rollback the prefix
                  buffer->resize(len0);
              }
              else {
                  buffer->push_back('\n');
              }
          }
          else {
              const bdem_List& theList = value.theList(index);
              if (theList.length()) {
                  generateValuePrefix(buffer, constraint, index, level,
                                                               spacesPerLevel);

                  bdempu_Aggregate::generateListRaw(buffer,
                               value.theList(index), negLevel, spacesPerLevel);
                  buffer->push_back('\n');
              }
          }
      } break;
      case bdem_ElemType::TABLE: {
        // Don't generate "NULL" for fields in empty constrained table
        if (constraint.field(index).constraint()) {
            // This is a constrained table
              const int len0 = buffer->size(); // If table is empy, we use it
                                               // undo the changes to the
                                               // buffer
              generateValuePrefix(buffer, constraint, index, level,
                                                               spacesPerLevel);
              const int len = buffer->size();  // We use it to detect if the
                                               // table was generated
              bdempu_SchemaAggregate::generateConstrainedTableRaw(
                  buffer,
                  value.theTable(index),
                  *constraint.field(index).constraint(),
                  negLevel,
                  spacesPerLevel);

              if (len == (int)buffer->size()) {
                  // List wasn't generated, rollback the prefix
                  buffer->resize(len0);
              }
              else {
                  buffer->push_back('\n');
              }
          }
          else {
              const bdem_Table& theTable = value.theTable(index);
              if ((0 != theTable.numColumns()) ||
                  (0 != theTable.numRows())) {
                  generateValuePrefix(buffer, constraint, index, level,
                                                               spacesPerLevel);
                  bdempu_Aggregate::generateTableRaw(buffer, theTable,
                                                     negLevel, spacesPerLevel);
                  buffer->push_back('\n');
              }
          }
      } break;
      // TBD
      // The following catches 'BOOL', 'DATETIMETZ', etc.  This package is
      // deprecated, so it is unlikely to be extended to support the new types.
      default: {
        BDE_ASSERT_CPP(0);  // Should *not* reach here!
      } break;
    }
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
