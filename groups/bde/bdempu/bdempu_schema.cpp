// bdempu_schema.cpp                                                  -*-C++-*-
#include <bdempu_schema.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdempu_schema_cpp,"$Id$ $CSID$")

#include <bdempu_elemtype.h>

#include <bdepu_parserimputil.h>
#include <bdepu_typesparser.h>
#include <bdepu_typesparserimputil.h>

#include <bdem_schema.h>

#include <bsls_assert.h>

#include <bsl_cstddef.h>

namespace BloombergLP {

enum { BDEMPU_SUCCESS = 0, BDEMPU_FAILURE = 1 };

static
bool isIdentifier(const bsl::string& result) {
    // Check first character
    char nextChar = result[0];
    if (!((nextChar >= 'A' && nextChar <= 'Z') ||
          (nextChar >= 'a' && nextChar <= 'z') || nextChar == '_'))
        return false;

    // And then all the rest
    for (bsl::size_t i = 0; i < result.length(); ++i) {
        nextChar = result[i];
        if (! ((nextChar >= 'A' && nextChar <= 'Z') ||
               (nextChar >= 'a' && nextChar <= 'z') ||
               (nextChar >= '0' && nextChar <= '9') ||
                nextChar == '_')) {
            return false;
        }
    }
    return true;
}

                    // ====================
                    // struct bdempu_Schema
                    // ====================

// PRIVATE CLASS METHODS
int bdempu_Schema::parseField(const char         **endPos,
                              bdem_RecordDef      *result,
                              const char          *inputString,
                              const bdem_Schema&   schema)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    // This 'if' block completely handles all Elemtypes other than
    // (constrained) List and Table (i.e., RECORD and RECORD_ARRAY)
    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, inputString,
                                               "RECORD")) {
        // This could be <ELEMENT_TYPE> <OPTIONAL_NAME> ";"
        bdem_ElemType::Type fieldType;
        if (bdempu_ElemType::parseElemType(endPos, &fieldType, *endPos)) {
            return BDEMPU_FAILURE;
        }

        bdem_FieldDefAttributes  fieldAttr(fieldType);
        bsl::string              fieldName;
        const char              *fieldNameCStr = 0;

        // Set the field's nullability according to pre-BDE 1.19 semantics for
        // compatibility with legacy applications.
        fieldAttr.setIsNullable(bdem_ElemType::BDEM_LIST == fieldType ? false
                                                                      : true);

        bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
        if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, ';')) {
            // This could be the field that has a name
            if (bdepu_TypesParserImpUtil::parseQuotedString(endPos,
                                 &fieldName, *endPos)) {
                if (bdepu_ParserImpUtil::parseIdentifier(endPos, &fieldName,
                                                         *endPos)) {
                    return BDEMPU_FAILURE;
                }
            }

            bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
            if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, ';')) {
                return BDEMPU_FAILURE;
            }
            fieldNameCStr = fieldName.c_str();
        }

        result->appendField(fieldAttr, fieldNameCStr);
        return BDEMPU_SUCCESS;
    }

    // If we got this far, we are parsing either a RECORD or a RECORD_ARRAY.
    bdem_ElemType::Type fieldType;
    if ('_' == **endPos) {  // This could be "RECORD_ARRAY"
        ++*endPos;
        if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, "ARRAY")) {
            return BDEMPU_FAILURE;
        }
        fieldType = bdem_ElemType::BDEM_TABLE;
        bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    }
    else {
        fieldType = bdem_ElemType::BDEM_LIST;
    }

    // Inside a field def, both RECORD and RECORD_ARRAY *must* be constrained
    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '<')) {
        return BDEMPU_FAILURE;
    }

    // Find either an integer locator, or a string locator & convert to int.
    int integerLocator = -1;
    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    if (bdepu_TypesParser::parseIntegerLocator(endPos, &integerLocator,
                                                     *endPos)) {
        bsl::string stringLocator;
        if (bdepu_TypesParserImpUtil::parseQuotedString(endPos, &stringLocator,
                                                                    *endPos)) {
            if (bdepu_ParserImpUtil::parseIdentifier(endPos, &stringLocator,
                                                                    *endPos)) {
                return BDEMPU_FAILURE;
            }
        }
        integerLocator = schema.recordIndex(stringLocator.c_str());
    }

    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '>')) {
        return BDEMPU_FAILURE;
    }
    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);

    // See if the field has been named
    bsl::string locatorName;
    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, ';')) {
        if (bdepu_TypesParserImpUtil::parseQuotedString(endPos,
                                        &locatorName, *endPos)) {
            if (bdepu_ParserImpUtil::parseIdentifier(endPos, &locatorName,
                                                     *endPos)) {
                return BDEMPU_FAILURE;
            }
        }

        bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
        if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, ';')) {
            return BDEMPU_FAILURE;
        }
    }

    // Now insert the table or list.
    bdem_FieldDefAttributes fieldAttr(fieldType);

    // Set the field's nullability according to pre-BDE 1.19 semantics for
    // compatibility with legacy applications.
    fieldAttr.setIsNullable(bdem_ElemType::BDEM_LIST == fieldType ? false
                                                                  : true);

    const char *fieldName = locatorName.empty() ? 0 : locatorName.c_str();
    if (!result->appendField(fieldAttr,
                             &schema.record(integerLocator),
                             fieldName)) {
        return BDEMPU_FAILURE;
    }

    return BDEMPU_SUCCESS;
}

int bdempu_Schema::parseRecord(const char  **endPos,
                               bdem_Schema  *result,
                               const char   *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, inputString,
                                               "RECORD")) {
        return BDEMPU_FAILURE;
    }

    bdem_RecordDef *recordDef;
    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '{')) {
        // parse record name
        bsl::string recordName;
        if (bdepu_TypesParserImpUtil::parseQuotedString(endPos, &recordName,
                                                                 *endPos)) {
            if (bdepu_ParserImpUtil::parseIdentifier(endPos, &recordName,
                                                                 *endPos)) {
                return BDEMPU_FAILURE;
            }
        }
        bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
        if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '{')) {
            return BDEMPU_FAILURE;
        }
        if (! (recordDef = result->createRecord(recordName.c_str()))) {
            return BDEMPU_FAILURE;
        }
    }
    else {
        if (!(recordDef = result->createRecord())) {
            return BDEMPU_FAILURE;
        }
    }

    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    while (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '}')) {
        if (parseField(endPos, recordDef, *endPos, *result)) {
            return BDEMPU_FAILURE;
        }
        bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    }

    return BDEMPU_SUCCESS;
}

void bdempu_Schema::generateRecordDef(bsl::vector<char>  *buffer,
                                      const bdem_Schema&  value,
                                      int                 index,
                                      int                 level,
                                      int                 spacesPerLevel)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= level);
    BSLS_ASSERT(0 <= spacesPerLevel);

    const char *recordName = value.recordName(index);
    const bdem_RecordDef *recordDef = &value.record(index);

    bdepu_ParserImpUtil::generateIndentation(buffer, level, spacesPerLevel);

    buffer->push_back('R');
    buffer->push_back('E');
    buffer->push_back('C');
    buffer->push_back('O');
    buffer->push_back('R');
    buffer->push_back('D');
    buffer->push_back(' ');

    if (recordName) {
        int hasNonIdentifierCharacters = !isIdentifier(recordName);
        if (hasNonIdentifierCharacters) {
            buffer->push_back('"');
        }
        const int len = bsl::strlen(recordName);
        for (int i = 0; i < len; ++i) {
            buffer->push_back(recordName[i]);
        }
        if (hasNonIdentifierCharacters) {
            buffer->push_back('"');
        }
        buffer->push_back(' ');
    }

    buffer->push_back('{');
    buffer->push_back('\n');

    int n = recordDef->numFields();  // Number of fields in the record

    for (int i = 0; i < n; ++i) {
        const bdem_FieldDef *fldDef  = &recordDef->field(i);
        bdem_ElemType::Type  type    = fldDef->elemType();
        const char          *fldName = recordDef->fieldName(i);

        bdepu_ParserImpUtil::
                        generateIndentation(buffer, level + 1, spacesPerLevel);

        const char *asciiElemType;
        switch (type) {
          case bdem_ElemType::BDEM_LIST: {
            asciiElemType = "RECORD ";
          } break;
          case bdem_ElemType::BDEM_TABLE: {
            asciiElemType = "RECORD_ARRAY ";
          } break;
          default: {
            asciiElemType = bdem_ElemType::toAscii(type);
          } break;
        }

        const int len = bsl::strlen(asciiElemType);
        for (int j = 0; j < len; ++j) {
            buffer->push_back(asciiElemType[j]);
        }

        const bdem_RecordDef *constraint = fldDef->recordConstraint();

        if (constraint) {
            BSLS_ASSERT(type == bdem_ElemType::BDEM_LIST
                        || type == bdem_ElemType::BDEM_TABLE);
            buffer->push_back('<');
            buffer->push_back(' ');
            buffer->push_back('{');
            bdepu_TypesParserImpUtil::generateInt(buffer,
                                                  constraint->recordIndex());
            buffer->push_back('}');
            buffer->push_back(' ');
            buffer->push_back('>');
        }
        if (fldName) {
            buffer->push_back(' ');
            int hasNonIdentifierCharacters = !isIdentifier(fldName);
            if (hasNonIdentifierCharacters) {
                buffer->push_back('"');
            }

            const int lenFldName = bsl::strlen(fldName);
            for (int j = 0; j < lenFldName; ++j) {
                buffer->push_back(fldName[j]);
            }

            if (hasNonIdentifierCharacters) {
                buffer->push_back('"');
            }
        }
        buffer->push_back(' ');
        buffer->push_back(';');
        buffer->push_back('\n');
    }
    bdepu_ParserImpUtil::generateIndentation(buffer, level, spacesPerLevel);
    buffer->push_back('}');
    buffer->push_back('\n');
}

// CLASS METHODS
int bdempu_Schema::parseSchema(const char  **endPos,
                               bdem_Schema  *result,
                               const char   *inputString)
{
    bdepu_ParserImpUtil::skipWhiteSpace(endPos, inputString);
    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '{')) {
        return BDEMPU_FAILURE;
    }

    result->removeAll();
    while (1) {
        bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
        switch (**endPos) {
          case '}': {
            // Schema parsed successfully
            ++*endPos;
            return BDEMPU_SUCCESS;
          }
          case 'R': {
            if (parseRecord(endPos, result, *endPos)) {
                return BDEMPU_FAILURE;
            }
          } break;
          default: {  // Parsing failed
            return BDEMPU_FAILURE;
          }
        }
    }
}

void bdempu_Schema::generateSchema(bsl::vector<char>   *buffer,
                                   const bdem_Schema&  value,
                                   int                 level,
                                   int                 spacesPerLevel)
{
    bdepu_NTypesParser::stripNull(buffer);
    generateSchemaRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

void bdempu_Schema::generateSchemaRaw(bsl::vector<char>   *buffer,
                                      const bdem_Schema&  value,
                                      int                 level,
                                      int                 spacesPerLevel)
{
    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer, level,
                                                 spacesPerLevel);
    }
    else {
        level = -level;
    }

    buffer->push_back('{');
    buffer->push_back('\n');

    int len = value.numRecords();

    for (int i = 0; i < len; ++i) {
        generateRecordDef(buffer, value, i, level + 1, spacesPerLevel);
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
