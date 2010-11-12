// baexml_schemagenerator.cpp                  -*-C++-*-
#include <baexml_schemagenerator.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_schemagenerator_cpp,"$Id$ $CSID$")

#include <baexml_typesprintutil.h>

#include <bdem_elemtype.h>
#include <bdem_schema.h>

#include <bdetu_unset.h>

#include <bdet_datetime.h>
#include <bdet_datetimetz.h>
#include <bdet_date.h>
#include <bdet_datetz.h>
#include <bdet_time.h>
#include <bdet_timetz.h>

#include <bdeat_formattingmode.h>

#include <bsls_assert.h>
#include <bsls_platformutil.h>

#include <bsl_cstring.h>     // 'strchr', etc.
#include <bsl_cstdio.h>      // 'sprintf'
#include <bsl_iomanip.h>
#include <bsl_sstream.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>

namespace BloombergLP {

namespace {

const char *bdemTypeToDefaultXmlType[bdem_ElemType::BDEM_NUM_TYPES] = {
    // Map all 'bdem' element types to the name of their corresponding default
    // XML type.  Note that the order of the strings in the following table
    // must correspond to the 'bdem_ElemType::Type' enumeration.
    //
    // XML schema type           'bdem' element type
    // ---------------           -------------------
    "xs:byte",                   // bdem_ElemType::BDEM_CHAR
    "xs:short",                  // bdem_ElemType::BDEM_SHORT
    "xs:int",                    // bdem_ElemType::BDEM_INT
    "xs:long",                   // bdem_ElemType::BDEM_INT64
    "xs:float",                  // bdem_ElemType::BDEM_FLOAT
    "xs:double",                 // bdem_ElemType::BDEM_DOUBLE
    "xs:string",                 // bdem_ElemType::BDEM_STRING
    "xs:dateTime",               // bdem_ElemType::BDEM_DATETIME
    "xs:date",                   // bdem_ElemType::BDEM_DATE
    "xs:time",                   // bdem_ElemType::BDEM_TIME
    "BYTE_LIST_TYPE",            // bdem_ElemType::BDEM_CHAR_ARRAY
    "SHORT_LIST_TYPE",           // bdem_ElemType::BDEM_SHORT_ARRAY
    "INT_LIST_TYPE",             // bdem_ElemType::BDEM_INT_ARRAY
    "INT64_LIST_TYPE",           // bdem_ElemType::BDEM_INT64_ARRAY
    "FLOAT_LIST_TYPE",           // bdem_ElemType::BDEM_FLOAT_ARRAY
    "DOUBLE_LIST_TYPE",          // bdem_ElemType::BDEM_DOUBLE_ARRAY
    "STRING_LIST_TYPE",          // bdem_ElemType::BDEM_STRING_ARRAY
    "DATETIME_LIST_TYPE",        // bdem_ElemType::BDEM_DATETIME_ARRAY
    "DATE_LIST_TYPE",            // bdem_ElemType::BDEM_DATE_ARRAY
    "TIME_LIST_TYPE",            // bdem_ElemType::BDEM_TIME_ARRAY
    "LIST",                      // bdem_ElemType::BDEM_LIST
    "LIST",                      // bdem_ElemType::BDEM_TABLE

    "xs:boolean",                // bdem_ElemType::BDEM_BOOL
    "xs:dateTime",               // bdem_ElemType::BDEM_DATETIMETZ
    "xs:date",                   // bdem_ElemType::BDEM_DATETZ
    "xs:time",                   // bdem_ElemType::BDEM_TIMETZ
    "BOOL_LIST_TYPE",            // bdem_ElemType::BDEM_BOOL_ARRAY
    "DATETIMETZ_LIST_TYPE",      // bdem_ElemType::BDEM_DATETIMETZ_ARRAY
    "DATETZ_LIST_TYPE",          // bdem_ElemType::BDEM_DATETZ_ARRAY
    "TIMETZ_LIST_TYPE",          // bdem_ElemType::BDEM_TIMETZ_ARRAY
    "CHOICE",                    // bdem_ElemType::BDEM_CHOICE
    "CHOICE"                     // bdem_ElemType::BDEM_CHOICE_ARRAY
};

const char *const BDEM_URI = "http://bloomberg.com/schemas/bdem";

// STATIC HELPER FUNCTIONS

enum { BAEXML_FAILURE = -1, BAEXML_SUCCESS = 0 };

char *generateName(char *buf, const char *prefix, int id)
    // Generate a name into the specified 'buf' by appending the decimal
    // representation of the specified 'id' to the contents of the specified
    // 'prefix' and return 'buf'.  The behavior is undefined unless 'buf' has
    // a capacity at least 10 characters longer than 'prefix' and 'id' is
    // non-negative.
{
    BSLS_ASSERT(buf);
    BSLS_ASSERT(id >= 0);

    bsl::sprintf(buf, "%s%d", prefix, id);
    return buf;
}

void putBoilerPlateSimpleTypes(baexml_Formatter& formatter)
    // Write boiler-plate XML that declares a 'simpleType' for each 'bdem'
    // array type to the specified 'stream'.
{
    static const struct {
        const char *d_simpleTypeName;
        const char *d_itemType;
    } BOILER_PLATE_DATA[] = {
        { "BYTE_LIST_TYPE",       "xs:byte"      },
        { "SHORT_LIST_TYPE",      "xs:short"     },
        { "INT_LIST_TYPE",        "xs:int"       },
        { "INT64_LIST_TYPE",      "xs:long"      },
        { "FLOAT_LIST_TYPE",      "xs:float"     },
        { "DOUBLE_LIST_TYPE",     "xs:double"    },
        { "DECIMAL_LIST_TYPE",    "xs:decimal"   },
        { "STRING_LIST_TYPE",     "xs:string"    },
        { "DATETIME_LIST_TYPE",   "xs:dateTime"  },
        { "DATE_LIST_TYPE",       "xs:date"      },
        { "TIME_LIST_TYPE",       "xs:time"      },
        { "BOOL_LIST_TYPE",       "xs:boolean"   },
        { "DATETIMETZ_LIST_TYPE", "xs:dateTime"  },
        { "DATETZ_LIST_TYPE",     "xs:date"      },
        { "TIMETZ_LIST_TYPE",     "xs:time"      },
    };

    static const int BOILER_PLATE_DATA_LEN =
        sizeof BOILER_PLATE_DATA / sizeof BOILER_PLATE_DATA[0];

    // For each boiler-plate item, generate the following XSD:
    //..
    //     <xs:simpleType name=\"XYZ_LIST_TYPE\">\n"
    //         <xs:list itemType=\"xs:xyz\"/>\n"
    //     </xs:simpleType>\n"
    //..

    formatter.addBlankLine();
    for (int i = 0; i < BOILER_PLATE_DATA_LEN; ++i) {
        const char *simpleTypeName = BOILER_PLATE_DATA[i].d_simpleTypeName;
        const char *itemType       = BOILER_PLATE_DATA[i].d_itemType;

        formatter.openElement("xs:simpleType");
        formatter.addAttribute("name", simpleTypeName);
        formatter.openElement("xs:list");
        formatter.addAttribute("itemType", itemType);
        formatter.closeElement("xs:list");
        formatter.closeElement("xs:simpleType");
    }
    formatter.addBlankLine();
}

void getTypeName(bsl::string *typeName, const bdem_FieldDef& field)
    // Load into the specified 'typeName' the name of the XML type
    // corresponding to the specified 'bdem' scalar 'field'.   The behavior is
    // undefined unless 'typeName' is non-null.  Note that the formatting mode
    // of the 'field' can override the default 'bdem'-type-to-XML-type mapping.
{
    bdem_ElemType::Type elemType = field.elemType();

    // Modify type according to formatting mode
    const int fieldFormat = field.formattingMode();
    const int typeFormat  =
                           fieldFormat & bdeat_FormattingMode::BDEAT_TYPE_MASK;
    switch (elemType) {
      case bdem_ElemType::BDEM_CHAR_ARRAY:
        if (bdeat_FormattingMode::BDEAT_BASE64 == typeFormat) {
            *typeName = "xs:base64Binary";
            return;
        }
        else if (bdeat_FormattingMode::BDEAT_HEX == typeFormat) {
            *typeName = "xs:hexBinary";
            return;
        }
        break;

      case bdem_ElemType::BDEM_FLOAT:
      case bdem_ElemType::BDEM_DOUBLE:
        if (bdeat_FormattingMode::BDEAT_DEC == typeFormat) {
            *typeName = "xs:decimal";
            return;
        }
        break;

      case bdem_ElemType::BDEM_FLOAT_ARRAY:
      case bdem_ElemType::BDEM_DOUBLE_ARRAY:
        if (bdeat_FormattingMode::BDEAT_DEC == typeFormat) {
            *typeName = "DECIMAL_LIST_TYPE";
            return;
        }
        break;

      default:
        break;
    }

    if (bdem_ElemType::isArrayType(elemType) &&
        ! (fieldFormat & bdeat_FormattingMode::BDEAT_LIST)) {
        // For arrays that are not formatted as lists, the type
        // is the same as the scalar type (but maxOccurs will be different).
        elemType = bdem_ElemType::fromArrayType(elemType);
    }

    *typeName = bdemTypeToDefaultXmlType[elemType];
}

void getDefaultValue(bsl::string *defaultValue, const bdem_FieldDef& field)
    // Load into the specified 'defaultValue' the string representation of the
    // default value for the specified 'bdem' scalar 'field'.  If 'type' is
    // non-scalar, 'defaultValue' is set to the empty string ("").  The
    // behavior is undefined unless 'defaultValue' is non-null.
{
    bsl::ostringstream stream;

    const bool hasDefaultValue = field.hasDefaultValue();
    const int  formattingMode  = bdeat_FormattingMode::BDEAT_DEFAULT;

    if (! hasDefaultValue) {
        defaultValue->clear();
        return;
    }

    switch (field.elemType()) {
      case bdem_ElemType::BDEM_CHAR: {
        const char value = field.defaultValue().theChar();
        stream << (int)(signed char)value;
      } break;
      case bdem_ElemType::BDEM_SHORT: {
        baexml_TypesPrintUtil::print(stream,
                                     field.defaultValue().theShort(),
                                     formattingMode);
      } break;
      case bdem_ElemType::BDEM_INT: {
        const bdem_EnumerationDef *enumeration =
            field.enumerationConstraint();
        if (enumeration) {
            baexml_TypesPrintUtil::print(
                stream,
                enumeration->lookupName(field.defaultValue().theInt()),
                formattingMode);
        }
        else {
            baexml_TypesPrintUtil::print(stream,
                                         field.defaultValue().theInt(),
                                         formattingMode);
        }
      } break;
      case bdem_ElemType::BDEM_INT64: {
        typedef bsls_PlatformUtil::Int64 Int64;
        baexml_TypesPrintUtil::print(stream,
                                     field.defaultValue().theInt64(),
                                     formattingMode);
      } break;
      case bdem_ElemType::BDEM_FLOAT: {
        baexml_TypesPrintUtil::print(stream,
                                     field.defaultValue().theFloat(),
                                     formattingMode);
      } break;
      case bdem_ElemType::BDEM_DOUBLE: {
        baexml_TypesPrintUtil::print(stream,
                                     field.defaultValue().theDouble(),
                                     formattingMode);
      } break;
      case bdem_ElemType::BDEM_STRING: {
        baexml_TypesPrintUtil::print(stream,
                                     field.defaultValue().theString(),
                                     formattingMode);
      } break;
      case bdem_ElemType::BDEM_DATETIME: {
        baexml_TypesPrintUtil::print(stream,
                                     field.defaultValue().theDatetime(),
                                     formattingMode);
      } break;
      case bdem_ElemType::BDEM_DATE: {
        baexml_TypesPrintUtil::print(stream,
                                     field.defaultValue().theDate(),
                                     formattingMode);
      } break;
      case bdem_ElemType::BDEM_TIME: {
        baexml_TypesPrintUtil::print(stream,
                                     field.defaultValue().theTime(),
                                     formattingMode);
      } break;
      case bdem_ElemType::BDEM_BOOL: {
        baexml_TypesPrintUtil::print(stream,
                                     field.defaultValue().theBool(),
                                     formattingMode);
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ: {
        baexml_TypesPrintUtil::print(stream,
                                     field.defaultValue().theDatetimeTz(),
                                     formattingMode);
      } break;
      case bdem_ElemType::BDEM_DATETZ: {
        baexml_TypesPrintUtil::print(stream,
                                     field.defaultValue().theDateTz(),
                                     formattingMode);
      } break;
      case bdem_ElemType::BDEM_TIMETZ: {
        baexml_TypesPrintUtil::print(stream,
                                     field.defaultValue().theTimeTz(),
                                     formattingMode);
      } break;
      default: {
        stream << "";
      } break;
    }

    *defaultValue = stream.str();
}

void getTypeAndValue(bsl::string          *typeName,
                     bsl::string          *defaultValue,
                     const bdem_FieldDef&  field)
    // Load into the specified 'typeName' and 'defaultValue' the name of the
    // XML 'complexType' and the string representation of the default value,
    // respectively, for the specified 'bdem' 'field' definition.  The behavior
    // is undefined unless 'typeName' and 'defaultValue' are non-null.
{
    const bdem_RecordDef *record = field.recordConstraint();
    const bdem_EnumerationDef *enumeration = field.enumerationConstraint();

    if (record) {  // constrained choice, choice array, list, or table
        const int recordIndex  = record->recordIndex();
        const char *recordName = record->schema().recordName(recordIndex);

        char nameBuffer[25];
        if (! recordName) {
            recordName = generateName(nameBuffer, "RECORD_", recordIndex);
        }

        *typeName = recordName;
        // A constrained choice, choice array, list or table cannot have
        // a default value.
    }
    else if (enumeration) {
        const int enumerationIndex  = enumeration->enumerationIndex();
        const char *enumerationName =
            enumeration->schema().enumerationName(enumerationIndex);

        char nameBuffer[25];
        if (! enumerationName) {
            enumerationName = generateName(nameBuffer, "ENUMERATION_",
                                           enumerationIndex);
        }

        *typeName = enumerationName;
        getDefaultValue(defaultValue, field);
    }
    else {  // unconstrained 'bdem' type
        BSLS_ASSERT(! bdem_ElemType::isAggregateType(field.elemType()));

        getTypeName(typeName, field);
        getDefaultValue(defaultValue, field);
    }
}

void formatEnumerationAsType(baexml_Formatter           *formatter,
                            const bdem_EnumerationDef&  enumeration)
    // Format the specified 'enumeration' as an XML 'simpleType' definition
    // using the specified 'formatter'.
{
    static const int endEnumId = bdetu_Unset<int>::unsetValue();

    const bdem_Schema& schema = enumeration.schema();

    const int enumerationIndex = enumeration.enumerationIndex();
    const char *enumerationName = schema.enumerationName(enumerationIndex);

    char nameBuffer[25];
    if (! enumerationName) {
        enumerationName = generateName(nameBuffer, "ENUMERATION_",
                                       enumerationIndex);
    }

    formatter->openElement("xs:simpleType");
    formatter->addAttribute("name", enumerationName);
    formatter->openElement("xs:restriction");
    formatter->addAttribute("base", "xs:string");

    // Enumerations can optionally have user defined ids associated with them,
    // e.g., <enumeration value="ENUM1" bdem::id="1"/>.  If ids are present,
    // maintaining the value to id correspondence described in the xml is
    // required.
    bool useIds = false;
    int followId = -1;
    const char *followStr = "";
    for (bsl::pair<const char *, int> enumerator =
         enumeration.nextLargerId(INT_MIN);
         enumerator.second != endEnumId;
         enumerator = enumeration.nextLargerId(enumerator.second)) {

        if (1 > bsl::strcmp(enumerator.first, followStr) ||
            ++followId != enumerator.second) {
            // Enumerators are not alphabetically ordered or ids are not
            // sequentially incremented.
            useIds = true;
            break;
        }
        followStr = enumerator.first;
    }

    // Second pass actually adds the enumerators to the formatter.
    for (bsl::pair<const char *, int> enumerator =
         enumeration.nextLargerId(INT_MIN);
         enumerator.second != endEnumId;
         enumerator = enumeration.nextLargerId(enumerator.second)) {

        formatter->openElement("xs:enumeration");
        formatter->addAttribute("value", enumerator.first);
        if (useIds) {
            formatter->addAttribute("bdem:id", enumerator.second);
        }
        formatter->closeElement("xs:enumeration");
    }

    formatter->closeElement("xs:restriction");
    formatter->closeElement("xs:simpleType");
}

int formatRecordAttributes(baexml_Formatter      *formatter,
                           const bdem_RecordDef&  record)
{
    const bool recordHasFieldIds = record.hasFieldIds();
    const int numFields = record.numFields();

    // Loop through the fields and generate '<attribute>' for each one for
    // which the 'IS_ATTRIBUTE' bit is set.  This is done BEFORE the
    // '<sequence>' or '<choice>' attribute is started.  The remaining
    // (non-attribute) fields will be handled in a second pass, below.
    // The 'IS_ATTRIBUTE' bit is ignored for fields in choice records.
    for (int fieldIndex = 0; fieldIndex < numFields; ++fieldIndex)
    {
        const bdem_FieldDef&      field    = record.field(fieldIndex);
        const bdem_ElemType::Type elemType = field.elemType();

        if (! (field.formattingMode() &
               bdeat_FormattingMode::BDEAT_ATTRIBUTE)) {
            continue;  // Skip non-attributes
        }

        // Attributes must be scalar types
        if (!bdem_ElemType::isScalarType(elemType)) {
            return BAEXML_FAILURE;
        }

        formatter->openElement("xs:attribute");

        const char *name = record.fieldName(fieldIndex);
        char nameBuffer[25];
        if (! name) {
            name = generateName(nameBuffer, "FIELD_", fieldIndex);
        }

        formatter->addAttribute("name", name);

        bsl::string typeName, defaultValue;
        getTypeAndValue(&typeName, &defaultValue, field);
        formatter->addAttribute("type", typeName);
        if (! defaultValue.empty()) {
            formatter->addAttribute("default", defaultValue);
        }

        if (! field.isNullable() && ! field.hasDefaultValue()) {
            formatter->addAttribute("use", "required");
        }

        if (recordHasFieldIds) {  // Emit "explicit" field ids only.
            const int id = record.fieldId(fieldIndex);
            if (bdem_RecordDef::BDEM_NULL_FIELD_ID != id) {
                formatter->addAttribute("bdem:id", id);
            }
        }

        formatter->closeElement("xs:attribute");

    } // End for each attribute

    return BAEXML_SUCCESS;
}

int formatRecordElements(baexml_Formatter      *formatter,
                         const bdem_RecordDef&  record)
{
    const bool recordHasFieldIds = record.hasFieldIds();
    const int numFields = record.numFields();
    bool isChoice = bdem_RecordDef::BDEM_CHOICE_RECORD == record.recordType();

    // Loop through the fields and generate '<elements>' for each field for
    // which the 'IS_ATTRIBUTE' bit is not set.
    for (int fieldIndex = 0; fieldIndex < numFields; ++fieldIndex) {
        const bdem_FieldDef&      field     = record.field(fieldIndex);
        const bdem_ElemType::Type elemType  = field.elemType();
        const int fieldFormat = field.formattingMode();
        const bdem_RecordDef *fieldConstraint = field.recordConstraint();

        if (!isChoice
         && (fieldFormat & bdeat_FormattingMode::BDEAT_ATTRIBUTE)) {
            continue;  // Skip attributes in non-choice records
        }

        if (bdem_ElemType::isAggregateType(elemType) && ! fieldConstraint) {
        // All 'bdem' aggregate types must be constrained.
            return BAEXML_FAILURE;
        }

        const char *untaggedFieldKind = 0;
        if (fieldFormat & bdeat_FormattingMode::BDEAT_UNTAGGED
         && fieldConstraint) {
            // Field is either an untagged (anonymous) <choice> or <sequence>.
            untaggedFieldKind = (fieldConstraint->recordType() ==
                                 bdem_RecordDef::BDEM_CHOICE_RECORD ?
                                 "xs:choice" : "xs:sequence");
        }

        if (untaggedFieldKind) {
            // Untagged (anonymous) <choice> or <sequence>.
            formatter->openElement(untaggedFieldKind);
        }
        else { // if normal element
            formatter->openElement("xs:element");

            const char *name = record.fieldName(fieldIndex);
            char nameBuffer[25];
            if (! name) {
                name = generateName(nameBuffer, "FIELD_", fieldIndex);
            }

            formatter->addAttribute("name", name);

            bsl::string typeName, defaultValue;
            getTypeAndValue(&typeName, &defaultValue, field);
            formatter->addAttribute("type", typeName);
            if (defaultValue != "") {
                formatter->addAttribute("default", defaultValue);
            }
        } // end else if normal element

        const int typeFormat =
                           fieldFormat & bdeat_FormattingMode::BDEAT_TYPE_MASK;
        const bool isRepeatedElem =
            (bdem_ElemType::isArrayType(elemType)           &&
             typeFormat     != bdeat_FormattingMode::BDEAT_BASE64 &&
             typeFormat     != bdeat_FormattingMode::BDEAT_HEX    &&
             ! (fieldFormat &  bdeat_FormattingMode::BDEAT_LIST));

        if (field.isNullable() || isRepeatedElem || field.hasDefaultValue()) {
            formatter->addAttribute("minOccurs", 0);
        }

        if (isRepeatedElem) {
            formatter->addAttribute("maxOccurs", "unbounded");
        }

        if (fieldFormat & bdeat_FormattingMode::BDEAT_NILLABLE) {
            formatter->addAttribute("nillable", "true");
        }

        if (recordHasFieldIds) {  // Emit "explicit" field ids only.
            const int id = record.fieldId(fieldIndex);
            if (bdem_RecordDef::BDEM_NULL_FIELD_ID != id) {
                formatter->addAttribute("bdem:id", id);
            }
        }

        if (untaggedFieldKind) {
            formatRecordElements(formatter, *fieldConstraint);
            formatter->closeElement(untaggedFieldKind);
        }
        else {
            formatter->closeElement("xs:element");
        }
    }

    return BAEXML_SUCCESS;
}

int formatRecordAsType(baexml_Formatter      *formatter,
                       const bdem_RecordDef&  record)
    // Format the specified 'record' as an XML 'complexType' definition using
    // the specified 'formatter'.  Return 0 on success, and a non-zero value if
    // 'record' contains a field of unconstrained choice, choice array, list,
    // or table type.
{
    const int recordIndex  = record.recordIndex();
    const char *recordName = record.recordName();

    char nameBuffer[25];
    if (! recordName) {
        recordName = generateName(nameBuffer, "RECORD_", recordIndex);
    }

    formatter->openElement("xs:complexType");
    formatter->addAttribute("name", recordName);

    // For sequence records, generate the attributes.  Extract those fields
    // with the 'IS_ATTRIBUTE' formatting bit set and generate <attribute>
    // constructs.
    if (bdem_RecordDef::BDEM_SEQUENCE_RECORD == record.recordType() &&
        formatRecordAttributes(formatter, record)) {
        return BAEXML_FAILURE;
    }

    // Now generate the <sequence> or <choice> element.  Extract those fields
    // without the 'IS_ATTRIBUTE'  formatting bit set and generate nested
    // <element> constructs.
    bool isSeq = bdem_RecordDef::BDEM_SEQUENCE_RECORD == record.recordType();
    const char *elementKind = isSeq ? "xs:sequence" : "xs:choice";

    // Special case: If record is a named sequence and comprises a single,
    // untagged field, then remove the outermost nesting level.
    bool unNest = (isSeq && record.recordName() && 1 == record.numFields() &&
                   (record.field(0).formattingMode() &
                    bdeat_FormattingMode::BDEAT_UNTAGGED));

    int ret = BAEXML_SUCCESS;
    if (unNest) {
        // Skip outermost nesting level and generate the single, untagged,
        // <choice> or <sequence> field directly.
        ret = formatRecordElements(formatter, record);
    }
    else {
        formatter->openElement(elementKind);  // <choice> or <sequence>
        ret = formatRecordElements(formatter, record);
        formatter->closeElement(elementKind);
    }

    formatter->closeElement("xs:complexType");
    return ret;
}

void formatRecordAsElement(baexml_Formatter      *formatter,
                           const bdem_RecordDef&  record)
    // Format the specified 'record' as a top-level XML element using the
    // specified 'formatter'.
{
    const int recordIndex  = record.recordIndex();
    const char *recordName = record.schema().recordName(recordIndex);

    char nameBuffer[25];
    if (! recordName) {
        recordName = generateName(nameBuffer, "RECORD_", recordIndex);
    }

    formatter->openElement("xs:element");
    formatter->addAttribute("name", recordName);
    formatter->addAttribute("type", recordName);
    formatter->closeElement("xs:element");
}

}  // close unnamed namespace

// ACCESSORS

int
baexml_SchemaGenerator::generateXsd(baexml_Formatter&  formatter,
                                    const bdem_Schema& schema,
                                    const bsl::string& targetNamespace) const
{
    formatter.addHeader();

    formatter.openElement("xs:schema");
    formatter.addAttribute("xmlns:xs", "http://www.w3.org/2001/XMLSchema");
    formatter.addAttribute("xmlns:bdem", BDEM_URI);
    if (! targetNamespace.empty()) {
        formatter.addAttribute("targetNamespace", targetNamespace);
        formatter.addAttribute("xmlns", targetNamespace);
    }
    formatter.addAttribute("elementFormDefault", "qualified");
    formatter.addNewline();

    // Write boilerplate XML for 'bdem' array types.

    putBoilerPlateSimpleTypes(formatter);

    const int numEnumerations = schema.numEnumerations();
    for (int enumerationIndex = 0;
         enumerationIndex < numEnumerations;
         ++enumerationIndex) {

        const bdem_EnumerationDef& enumeration =
            schema.enumeration(enumerationIndex);
        formatEnumerationAsType(&formatter, enumeration);
        formatter.addBlankLine();
    }

    const int numRecords = schema.numRecords();

    // For each record, determine whether it is used by name, in an untagged
    // field, both, or neither.
    //
    // A record is "named" if either:
    // - The record has a name within the schema OR
    // - A field refers to the record and the field is not untagged.
    // A record is "untagged" if:
    // - An untagged field refers to the record.
    //
    // If a field has no named uses and at least one untagged use, then it is
    // assumed to exist only for use as an untagged nested type.
    enum {
        REC_UNUSED = 0, REC_NAMED = 1, REC_UNTAGGED = 2, REC_NAMED_UNTAGGED = 3
    };
    bsl::vector<char> recordUses(numRecords, REC_UNUSED);
    for (int recordIndex = 0; recordIndex < numRecords; ++recordIndex) {
        const bdem_RecordDef& record = schema.record(recordIndex);
        if (record.recordName()) {
            recordUses[recordIndex] |= REC_NAMED;
        }

        const int numFields = record.numFields();
        for (int fieldIndex = 0; fieldIndex < numFields; ++fieldIndex) {
            const bdem_FieldDef& field = record.field(fieldIndex);
            const bdem_RecordDef *recordConstraint = field.recordConstraint();
            if (recordConstraint) {
                const int constraintIndex = recordConstraint->recordIndex();
                if (field.formattingMode() &
                    bdeat_FormattingMode::BDEAT_UNTAGGED) {
                    recordUses[constraintIndex] |= REC_UNTAGGED;
                }
                else {
                    recordUses[constraintIndex] |= REC_NAMED;
                }
            }
        }
    }

    for (int recordIndex = 0; recordIndex < numRecords; ++recordIndex) {
        // Don't generate a schema-level complex type for a record that has
        // only untagged uses.
        if (recordUses[recordIndex] != REC_UNTAGGED) {
            const bdem_RecordDef& record = schema.record(recordIndex);
            if (formatRecordAsType(&formatter, record)) {
                return BAEXML_FAILURE;
            }
            formatter.addBlankLine();
        }
    }

    // Format all records as elements.
    for (int recordIndex = 0; recordIndex < numRecords; ++recordIndex) {
        // Don't generate a schema-level element for a record that has only
        // untagged uses.
        if (recordUses[recordIndex] != REC_UNTAGGED) {
            formatRecordAsElement(&formatter, schema.record(recordIndex));
            formatter.addBlankLine();
        }
    }

    formatter.closeElement("xs:schema");

    return BAEXML_SUCCESS;
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
