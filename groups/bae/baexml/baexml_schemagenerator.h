// baexml_schemagenerator.h                  -*-C++-*-
#ifndef INCLUDED_BAEXML_SCHEMAGENERATOR
#define INCLUDED_BAEXML_SCHEMAGENERATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an XML schema (XSD) generating utility.
//
//@CLASSES:
//  baexml_SchemaGenerator: XML schema generator
//
//@SEE_ALSO: baexml_schemaparser, bdem_schema
//
//@AUTHOR: Xiheng Xu (xxu)
//
//@DESCRIPTION: This component provides a class, 'baexml_SchemaGenerator' that
// may be used to generate an XML schema (XSD) corresponding to a
// 'bdem_Schema' object supplied by the user.  The generated XSD
// conforms to the W3 XML Schema, the namespace for which is described at the
// following URI:
//..
//   http://www.w3.org/TR/xmlschema-0/
//..
///XSD Generation Rules
///--------------------
// The generated XSD begins with boiler-plate code consisting of a '<schema>'
// definition containing declarations for a number of important namespaces
// (including the namespace for the generated schema itself), specifying that
// elements should be qualified ('elementFormDefault="qualified"') and
// declaring the target namespace.  The '<schema>' header is followed by a
// number of boilerplate '<simpleType>' definitions.  Some of this boilerplate
// may be removed in a later release if these definitions can be made
// transparent to users of the 'baexml' package.
//
// Each 'bdem_EnumerationDef' in the 'bdem_Schema' is mapped to a global
// (schema-level) '<simpleType>' in the output.  The '<simpleType>' defines an
// enumeration restriction on the built-in XML 'string' type.  The name of the
// '<simpleType>' is taken from the name of its corresponding enumeration
// definition in the 'bdem_Schema'.  If the enumeration is anonymous (i.e.,
// unnamed), a name of the form "ENUMERATION_ddd" is generated, where *ddd*
// indicates the index of the enumeration in its containing 'bdem_Schema'.
//
// Each 'bdem_RecordDef' in the 'bdem_schema' is mapped to a global
// (schema-level) '<complexType>' in the output.  The '<complexType>' contains
// a '<sequence>' or '<choice>' construct which, in turn, contains a local
// '<element>' for each field in the record definition.  The name of the
// '<complexType>' is taken from the name of its corresponding record in the
// 'bdem_Schema'.  If the record is anonymous (i.e., unnamed), a name of the
// form "RECORD_ddd" is generated, where *ddd* indicates the index of the
// record in its containing schema.  A global '<element>' definition is also
// generated for each 'bdem_RecordDef'.  This global '<element>' effectively
// makes the corresponding '<complexType>' available to documents that import
// the generated schema.
//
// As discussed above, a local '<element>' definition is generated for each
// field in a 'bdem_RecordDef' except that a local '<attribute>' definition is
// generated instead of an '<element>' definition if a field within a
// *sequence* record has the 'IS_ATTRIBUTE' bit set in its formatting mode.
// Except in the case of untagged fields (see below), an '<element>' or
// '<attribute>' generated for a field takes its name from the name of the
// field in the 'bdem_Schema'.  If the field is anonymous, a name of the form
// "FIELD_ddd" is generated, where *ddd* indicates the index of the field in
// its containing record.
//
// Each 'bdem' scalar field is represented by an '<element>' or '<attribute>'
// element with the "type" attribute set to the equivalent XML built-in type,
// as listed below:
//..
//       'bdem' Element Type     XML Type
//       -------------------     --------
//       CHAR                    xs:byte
//       SHORT                   xs:short
//       INT                     xs:int
//       INT64                   xs:long
//       FLOAT                   xs:float
//       DOUBLE                  xs:double
//       STRING                  xs:string
//       DATETIME                xs:dateTime
//       DATE                    xs:date
//       TIME                    xs:time
//       BOOL                    xs:boolean
//       DATETIMETZ              xs:dateTime
//       DATETZ                  xs:date
//       TIMETZ                  xs:time
//..
// Each 'bdem' array of scalar field is represented by an '<element>' with the
// type attribute set to the XML equivalent for the corresponding *scalar*
// type, and with the additional attribute, 'maxOccurs="unbounded"'.  Note
// that array types cannot be used to generate '<attribute>'s.  The generator
// will return an error if it encounters an array with the 'IS_ATTRIBUTE'
// formatting mode.
//
// Each 'bdem' field of aggregate type (i.e., 'LIST', 'CHOICE', 'TABLE', or
// 'CHOICE_ARRAY') is translated into an '<element>' with the "type" attribute
// set to the name of the field's record constraint (or the "RECORD_ddd"
// generated name if the record constraint was unnamed).  In the case of
// 'TABLE' and 'CHOICE_ARRAY', the 'maxOccurs="unbounded"' attribute is also
// generated.  An aggregate field that does not have a record constraint is
// not encodable in XSD and will result in an error being returned.  Note that
// aggregate types cannot be used to generate '<attribute>'s.  The generator
// will return an error if it encounters an aggregate with the 'IS_ATTRIBUTE'
// formatting mode.
//
// The translation from 'bdem' type to XML type can be altered through the use
// of formatting modes in the field definition.  Bdem fields of type
// 'CHAR_ARRAY' will be translated to the XML types 'base64binary' or
// 'hexBinary' by specifying the 'BASE64' or 'HEX' formatting mode,
// respectively.  Bdem fields of type 'FLOAT' or 'DOUBLE' will translated to
// the XML type 'decimal' if the 'DEC' formatting mode is set in the field
// specification.  Array types are mapped to list types if the
// 'bdeat_FormattingMode::LIST' bit is set, as per the following table:
//..
//       'bdem' Element Type     XML list Type
//       -------------------     -------------
//       CHAR_ARRAY              <simpleType name="CHAR_LIST_TYPE">
//       SHORT_ARRAY             <simpleType name="SHORT_LIST_TYPE">
//       INT_ARRAY               <simpleType name="INT_LIST_TYPE">
//       INT64_ARRAY             <simpleType name="INT64_LIST_TYPE">
//       FLOAT_ARRAY             <simpleType name="FLOAT_LIST_TYPE">
//       DOUBLE_ARRAY            <simpleType name="DOUBLE_LIST_TYPE">
//       STRING_ARRAY            <simpleType name="STRING_LIST_TYPE">
//       DATETIME_ARRAY          <simpleType name="DATETIME_LIST_TYPE">
//       DATE_ARRAY              <simpleType name="DATE_LIST_TYPE">
//       TIME_ARRAY              <simpleType name="TIME_LIST_TYPE">
//       BOOL_ARRAY              <simpleType name="BOOL_LIST_TYPE">
//       DATETIMETZ_ARRAY        <simpleType name="DATETIMETZ_LIST_TYPE">
//       DATETZ_ARRAY            <simpleType name="DATETZ_LIST_TYPE">
//       TIMETZ_ARRAY            <simpleType name="TIMETZ_LIST_TYPE">
//..
// Each XML list type is an abbreviation for an XML list of the
// specified type.  For example, 'INT_LIST_TYPE' is declared this way in the
// boilerplate code that is part of every generated schema:
//..
//    <xs:simpleType name="INT_LIST_TYPE">
//         <xs:list itemType="xs:int"/>
//    </xs:simpleType>
//..
// An '<element>' that is generated from a field that is nullable
// ('isNullable()' returns true), has a default value, or has array type will
// have the additional attribute, 'minOccurs="0"'.  An '<attribute>' that is
// generated from a non-nullable field with no default value will have the
// additional attribute, 'use="required"'.  If a field has a default value,
// then a "default" attribute is added to the generated '<element>' or
// '<attribute>'.  If the field has the 'IS_NILLABLE' mode flag, then the
// 'nillable="true"' attribute is added to the generated '<element>'
// definition.  Note that 'IS_NILLABLE' is ignored when combined with
// 'IS_ATTRIBUTE'.
//
///Untagged Aggregate Fields
///-------------------------
// If an aggregate field has the 'IS_UNTAGGED' formatting mode, then, instead
// of generating a named '<element>', the schema generator generates a nested,
// anonymous '<choice>' or '<sequence>' construct.  If the field is nullable,
// then the '<choice>' or '<sequence>' will have 'minOccurs="0"'.  In the case
// of 'CHOICE_ARRAY' or 'TABLE', the '<choice>' or '<sequence>' will have
// 'maxOccurs="unbounded"'.  As a special case, a *named* sequence record that
// comprises a single, untagged, aggregate field will have the top-level
// '<sequence>' stripped off.  Thus, instead of generating:
//..
//  <xs:complexType name="myRecord">
//      <xs:sequence>
//          <xs:choice minOccurs="0">
//              <xs:element name="chocolate" type="xs:int"/>
//              <xs:element name="vanilla" type="xs:string"/>
//          </xs:choice>
//      </xs:sequence>
//  </xs:complexType>
//..
// the schema generator removes the '<xs:sequence>' layer and generates:
//..
//  <xs:complexType name="myRecord">
//      <xs:choice minOccurs="0">
//          <xs:element name="chocolate" type="xs:int"/>
//          <xs:element name="vanilla" type="xs:string"/>
//      </xs:choice>
//  </xs:complexType>
//..
// This special case produces a schema that recognizes the same input, but
// reverses the addition of an extra nesting level produced by the schema
// parser.  The extra level of nesting is necessary in 'bdem_Schema' because
// there is no other way to represent a global nullable 'LIST' or 'CHOICE' or
// a global 'CHOICE_ARRAY' or 'TABLE'.  The extra nesting level is not
// necessary in XSD and produces a cluttered schema.
//
// The schema generator assumes that if a 'bdem_RecordDef' has no name and is
// used to constrain only untagged fields, then there is no desire to
// reference that record directly from the generated schema.  No global
// '<complexType>' or '<element>' constructs are generated for such anonymous
// records.
//
///Usage
///-----
// This component has a very simple interface.  The following illustrates
// typical usage.
//
// First we create a 'bdem_Schema' with the following structure:
//..
//  {
//      SEQUENCE RECORD "CrossReferenceRecord" {
//          INT "ID" { nullable 0x0 10 }
//          STRING "description" { nullable 0x0 }
//          LIST<"CrossReferenceRecord"> "selfReference" { nullable 0x0 }
//      }
//      SEQUENCE RECORD "EventRecord" {
//          DATETIME_ARRAY "eventDate" { nullable 0x0 }
//          SHORT "variance" { nullable 0x0 -1 }
//          INT ENUM<"EventType"> "type" { nullable 0x0 1 }
//          TABLE<"EventRecord"> { nullable 0x0 }
//      }
//      SEQUENCE RECORD {
//          STRING { nullable 0x0 }
//          STRING_ARRAY { nullable 0x0 }
//          STRING ENUM<"EventType"> { nullable 0x0 CORP_DINNER }
//          TIME "clock" { nullable 0x0 00:01:02.003 }
//      }
//      SEQUENCE RECORD "Configuration" {
//          CHAR "grade" { !nullable 0x20000 A }
//          SHORT "count" { nullable 0x0 -1 }
//          INT64 "grains" { nullable 0x0 -100 }
//          DOUBLE "price" { !nullable 0x0 }
//          STRING "name" { nullable 0x0 one }
//          DATETIME "timestamp" { nullable 0x0 01JAN2000_00:01:02.003 }
//          DATE "deadline" { nullable 0x0 01JAN2000 }
//          TIME "dailyAlarm" { nullable 0x0 00:01:02.003 }
//          CHAR_ARRAY "sequence" { nullable 0x0 }
//          LIST<"CrossReferenceRecord"> "crossReference" { !nullable 0x0 }
//          TABLE<"EventRecord"> "history" { nullable 0x0 }
//          TABLE[2] "namelessTable" { nullable 0x0 }
//      }
//      ENUMERATION "EventType" {
//          "CORP_LUNCH" = 0
//          "CORP_DINNER" = 1
//      }
//  }
//..
// The above schema illustrates the following:
//..
//   - Basic 'bdem' types (both scalar and array of scalar).
//   - Nullable and non-nullable fields.
//   - Scalar's constrained by enumerations.
//   - Default values
//   - Constrained 'LIST' and 'TABLE' types.
//   - Recursive record references.
//   - Anonymous records and fields.
//   - Attribute fields
//..
// The above schema is generated by the following code:
//..
//      bdem_Schema schema;
//
//      bdem_EnumerationDef *et = schema.createEnumeration("EventType");
//      et->addEnumerator("CORP_LUNCH");
//      et->addEnumerator("CORP_DINNER");
//
//      bdem_RecordDef *crr = schema.createRecord("CrossReferenceRecord");
//      bdem_FieldSpec fieldSpec(bdem_ElemType::BDEM_INT);
//      fieldSpec.defaultValue().theModifiableInt() = 10;
//      crr->appendField(fieldSpec, "ID");
//      crr->appendField(bdem_ElemType::BDEM_STRING, "description");
//      crr->appendField(bdem_ElemType::BDEM_LIST, crr, "selfReference");
//
//      bdem_RecordDef *er = schema.createRecord("EventRecord");
//      er->appendField(bdem_ElemType::BDEM_DATETIME_ARRAY, "eventDate");
//      fieldSpec.reset(bdem_ElemType::BDEM_SHORT);
//      fieldSpec.defaultValue().theModifiableShort() = -1;
//      er->appendField(fieldSpec, "variance");
//      fieldSpec.reset(bdem_ElemType::BDEM_INT, et);
//      fieldSpec.defaultValue().theModifiableInt() = 1;
//      er->appendField(fieldSpec, "type");
//      er->appendField(bdem_ElemType::BDEM_TABLE, er);
//
//      bdem_RecordDef *record2 = schema.createRecord();
//      record2->appendField(bdem_ElemType::BDEM_STRING);
//      record2->appendField(bdem_ElemType::BDEM_STRING_ARRAY);
//      fieldSpec.reset(bdem_ElemType::BDEM_STRING, et);
//      fieldSpec.defaultValue().theModifiableString() = "CORP_DINNER";
//      record2->appendField(fieldSpec);
//      fieldSpec.reset(bdem_ElemType::BDEM_TIME);
//      fieldSpec.defaultValue().theModifiableTime() = bdet_Time(0, 1, 2, 3);
//      record2->appendField(fieldSpec, "clock");
//
//      bdem_RecordDef *config = schema.createRecord("Configuration");
//      fieldSpec.reset(bdem_ElemType::BDEM_CHAR);
//      fieldSpec.defaultValue().theModifiableChar() = 'A';
//      fieldSpec.setIsNullable(false);
//      fieldSpec.setFormattingMode(bdeat_FormattingMode::BDEAT_ATTRIBUTE);
//      config->appendField(fieldSpec, "grade");
//      fieldSpec.reset(bdem_ElemType::BDEM_SHORT);
//      fieldSpec.defaultValue().theModifiableShort() = -1;
//      config->appendField(fieldSpec, "count");
//      fieldSpec.reset(bdem_ElemType::BDEM_INT64);
//      fieldSpec.defaultValue().theModifiableInt64() = -100;
//      config->appendField(fieldSpec, "grains");
//      fieldSpec.reset(bdem_ElemType::BDEM_DOUBLE);
//      fieldSpec.setIsNullable(false);
//      config->appendField(fieldSpec, "price");
//      fieldSpec.reset(bdem_ElemType::BDEM_STRING);
//      fieldSpec.defaultValue().theModifiableString() = "one";
//      config->appendField(fieldSpec, "name");
//      fieldSpec.reset(bdem_ElemType::BDEM_DATETIME);
//      fieldSpec.defaultValue().theModifiableDatetime() =
//                                       bdet_Datetime(2000, 1, 1, 0, 1, 2, 3);
//      config->appendField(fieldSpec, "timestamp");
//      fieldSpec.reset(bdem_ElemType::BDEM_DATE);
//      fieldSpec.defaultValue().theModifiableDate() = bdet_Date(2000, 1, 1);
//      config->appendField(fieldSpec, "deadline");
//      fieldSpec.reset(bdem_ElemType::BDEM_TIME);
//      fieldSpec.defaultValue().theModifiableTime() = bdet_Time(0, 1, 2, 3);
//      config->appendField(fieldSpec, "dailyAlarm");
//      config->appendField(bdem_ElemType::BDEM_CHAR_ARRAY, "sequence");
//      config->appendField(bdem_ElemType::BDEM_LIST, crr, "crossReference");
//      config->appendField(bdem_ElemType::BDEM_TABLE, er, "history");
//      config->appendField(bdem_ElemType::BDEM_TABLE, record2,
//                          "namelessTable");
//..
// We generate the XSD for this 'bdem_Schema' object by creating a generator
// object and an output stream, then calling the 'generateXsd' method.  In
// addition to the output stream and the input schema, the 'generateXsd'
// method takes an optional target namespace, initial indent-level, spaces per
// indent-level, and wrap column:
//..
//      baexml_SchemaGenerator generator;
//      generator.generateXsd(bsl::cout, schema,
//                            "http://bloomberg.com/schemas/configuration",
//                            0, 4, 68);
//..
// The following is printed to 'bsl::cout':
//..
//  <?xml version="1.0" encoding="UTF-8" ?>
//  <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema"
//       xmlns:bdem="http://bloomberg.com/schemas/bdem"
//       targetNamespace="http://bloomberg.com/schemas/configuration"
//       xmlns="http://bloomberg.com/schemas/configuration"
//       elementFormDefault="qualified">
//
//      <xs:simpleType name="BYTE_LIST_TYPE">
//          <xs:list itemType="xs:byte"/>
//      </xs:simpleType>
//      <xs:simpleType name="SHORT_LIST_TYPE">
//          <xs:list itemType="xs:short"/>
//      </xs:simpleType>
//      <xs:simpleType name="INT_LIST_TYPE">
//          <xs:list itemType="xs:int"/>
//      </xs:simpleType>
//      <xs:simpleType name="INT64_LIST_TYPE">
//          <xs:list itemType="xs:long"/>
//      </xs:simpleType>
//      <xs:simpleType name="FLOAT_LIST_TYPE">
//          <xs:list itemType="xs:float"/>
//      </xs:simpleType>
//      <xs:simpleType name="DOUBLE_LIST_TYPE">
//          <xs:list itemType="xs:double"/>
//      </xs:simpleType>
//      <xs:simpleType name="DECIMAL_LIST_TYPE">
//          <xs:list itemType="xs:decimal"/>
//      </xs:simpleType>
//      <xs:simpleType name="STRING_LIST_TYPE">
//          <xs:list itemType="xs:string"/>
//      </xs:simpleType>
//      <xs:simpleType name="DATETIME_LIST_TYPE">
//          <xs:list itemType="xs:dateTime"/>
//      </xs:simpleType>
//      <xs:simpleType name="DATE_LIST_TYPE">
//          <xs:list itemType="xs:date"/>
//      </xs:simpleType>
//      <xs:simpleType name="TIME_LIST_TYPE">
//          <xs:list itemType="xs:time"/>
//      </xs:simpleType>
//      <xs:simpleType name="BOOL_LIST_TYPE">
//          <xs:list itemType="xs:boolean"/>
//      </xs:simpleType>
//      <xs:simpleType name="DATETIMETZ_LIST_TYPE">
//          <xs:list itemType="xs:dateTime"/>
//      </xs:simpleType>
//      <xs:simpleType name="DATETZ_LIST_TYPE">
//          <xs:list itemType="xs:date"/>
//      </xs:simpleType>
//      <xs:simpleType name="TIMETZ_LIST_TYPE">
//          <xs:list itemType="xs:time"/>
//      </xs:simpleType>
//
//      <xs:simpleType name="EventType">
//          <xs:restriction base="xs:string">
//              <xs:enumeration value="CORP_LUNCH" bdem:id="0"/>
//              <xs:enumeration value="CORP_DINNER" bdem:id="1"/>
//          </xs:restriction>
//      </xs:simpleType>
//
//      <xs:complexType name="CrossReferenceRecord">
//          <xs:sequence>
//              <xs:element name="ID" type="xs:int" default="10"
//                   minOccurs="0"/>
//              <xs:element name="description" type="xs:string"
//                   minOccurs="0"/>
//              <xs:element name="selfReference"
//                   type="CrossReferenceRecord" minOccurs="0"/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name="EventRecord">
//          <xs:sequence>
//              <xs:element name="eventDate" type="xs:dateTime"
//                   minOccurs="0" maxOccurs="unbounded"/>
//              <xs:element name="variance" type="xs:short"
//                   default="-1" minOccurs="0"/>
//              <xs:element name="type" type="EventType"
//                   default="CORP_DINNER" minOccurs="0"/>
//              <xs:element name="FIELD_3" type="EventRecord"
//                   minOccurs="0" maxOccurs="unbounded"/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name="RECORD_2">
//          <xs:sequence>
//              <xs:element name="FIELD_0" type="xs:string"
//                   minOccurs="0"/>
//              <xs:element name="FIELD_1" type="xs:string"
//                   minOccurs="0" maxOccurs="unbounded"/>
//              <xs:element name="FIELD_2" type="EventType"
//                   default="CORP_DINNER" minOccurs="0"/>
//              <xs:element name="clock" type="xs:time"
//                   default="00:01:02.003" minOccurs="0"/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name="Configuration">
//          <xs:attribute name="grade" type="xs:byte" default="65"/>
//          <xs:sequence>
//              <xs:element name="count" type="xs:short" default="-1"
//                   minOccurs="0"/>
//              <xs:element name="grains" type="xs:long"
//                   default="-100" minOccurs="0"/>
//              <xs:element name="price" type="xs:double"/>
//              <xs:element name="name" type="xs:string"
//                   default="one" minOccurs="0"/>
//              <xs:element name="timestamp" type="xs:dateTime"
//                   default="2000-01-01T00:01:02.003" minOccurs="0"/>
//              <xs:element name="deadline" type="xs:date"
//                   default="2000-01-01" minOccurs="0"/>
//              <xs:element name="dailyAlarm" type="xs:time"
//                   default="00:01:02.003" minOccurs="0"/>
//              <xs:element name="sequence" type="xs:byte"
//                   minOccurs="0" maxOccurs="unbounded"/>
//              <xs:element name="crossReference"
//                   type="CrossReferenceRecord"/>
//              <xs:element name="history" type="EventRecord"
//                   minOccurs="0" maxOccurs="unbounded"/>
//              <xs:element name="namelessTable" type="RECORD_2"
//                   minOccurs="0" maxOccurs="unbounded"/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:element name="CrossReferenceRecord"
//           type="CrossReferenceRecord"/>
//
//      <xs:element name="EventRecord" type="EventRecord"/>
//
//      <xs:element name="RECORD_2" type="RECORD_2"/>
//
//      <xs:element name="Configuration" type="Configuration"/>
//
//  </xs:schema>;
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEXML_FORMATTER
#include <baexml_formatter.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

class bdem_Schema;

                        // ============================
                        // class baexml_SchemaGenerator
                        // ============================

class baexml_SchemaGenerator {
    // The overloaded 'generateXsd' method provided by this class generates an
    // XML schema (XSD) that is equivalent to a 'bdem_Schema' supplied to the
    // method.  The generated XSD is streamed to a 'bsl::ostream' or
    // 'bsl::streambuf' that is also supplied to the 'generateXsd' method.
    // Other arguments passed to 'generateXsd' affect the formatting of the
    // generated schema.  'baexml_SchemaGenerator' objects are stateless.

  private:
    // NOT IMPLEMENTED
    baexml_SchemaGenerator(const baexml_SchemaGenerator& original);
    baexml_SchemaGenerator& operator=(const baexml_SchemaGenerator& rhs);

    // At present, this class has no state and could be implemented as a
    // utility class.  However, this may change in the future.  For example,
    // this class may eventually hold generation options like extra namespace
    // declarations, style (e.g., whether the default namespace should be used
    // for the W3 Schema namespace or the target namespace), etc.  To preserve
    // these future possibilities, this class is implemented as a mechanism
    // class with non-static member functions.

  public:
    // CREATORS
    baexml_SchemaGenerator();
        // Construct a XML schema generator object.

    // ~baexml_SchemaGenerator();
        // Destroy this XML schema generator object.  Note that this trivial
        // destructor is generated by the compiler.

    // ACCESSORS
    int generateXsd(baexml_Formatter&   formatter,
                    const bdem_Schema&  schema,
                    const bsl::string&  targetNamespace) const;
        // Format an XML schema (XSD) equivalent to the specified 'bdem'
        // 'schema' to the specified 'formatter'.  The generated XSD declares
        // the specified 'targetNamespace' (if not empty).  Return 0 upon
        // success, and a non-zero value if any field defined in the 'schema'
        // is of unconstrained choice, choice array, list, or table type or if
        // any non-scalar field has the 'IS_ATTRIBUTE' formatting mode.  Note
        // that the generated XSD declares the default namespace to be
        // 'targetNamespace' and 'elementFormDefault' to be "qualified".  (See
        // 'http://www.w3.org/TR/xmlschema-0/' for further information on the
        // XML schema definition language.)

    int generateXsd(bsl::ostream&      stream,
                    const bdem_Schema& schema,
                    const bsl::string& targetNamespace,
                    int                initialIndentLevel = 0,
                    int                spacesPerLevel = 4,
                    int                wrapColumn = 80) const;

    int generateXsd(bsl::streambuf     *streamBuf,
                    const bdem_Schema&  schema,
                    const bsl::string&  targetNamespace,
                    int                 initialIndentLevel = 0,
                    int                 spacesPerLevel = 4,
                    int                 wrapColumn = 80) const;
        // Format an XML schema (XSD) equivalent to the specified 'bdem'
        // 'schema' to the specified 'stream' ('streamBuf').  The generated
        // XSD declares the specified 'targetNamespace' (if not empty).
        // Optionally specify the 'initialIndentLevel', 'spacesPerLevel', and
        // 'wrapColumn' to affect the format of the generated schema.  Return
        // 0 upon success, and a non-zero value if any field defined in the
        // 'schema' is of unconstrained choice, choice array, list, or table
        // type or if any non-scalar field has the 'IS_ATTRIBUTE' formatting
        // mode.  Note that the generated XSD declares the default namespace
        // to be 'targetNamespace' and 'elementFormDefault' to be "qualified".
        // (See 'http://www.w3.org/TR/xmlschema-0/' for further information on
        // the XML schema definition language.)
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CREATORS
inline
baexml_SchemaGenerator::baexml_SchemaGenerator()
{
}

// ACCESSORS
inline
int baexml_SchemaGenerator::generateXsd(bsl::ostream&      stream,
                                        const bdem_Schema& schema,
                                        const bsl::string& targetNamespace,
                                        int                initialIndentLevel,
                                        int                spacesPerLevel,
                                        int                wrapColumn) const
{
    baexml_Formatter formatter(stream,
                               initialIndentLevel,
                               spacesPerLevel,
                               wrapColumn);

    return generateXsd(formatter, schema, targetNamespace);
}

inline
int baexml_SchemaGenerator::generateXsd(bsl::streambuf    *streamBuf,
                                        const bdem_Schema& schema,
                                        const bsl::string& targetNamespace,
                                        int                initialIndentLevel,
                                        int                spacesPerLevel,
                                        int                wrapColumn) const
{
    baexml_Formatter formatter(streamBuf,
                               initialIndentLevel,
                               spacesPerLevel,
                               wrapColumn);

    return generateXsd(formatter, schema, targetNamespace);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
