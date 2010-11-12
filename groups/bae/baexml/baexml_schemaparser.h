// baexml_schemaparser.h                  -*-C++-*-
#ifndef INCLUDED_BAEXML_SCHEMAPARSER
#define INCLUDED_BAEXML_SCHEMAPARSER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an XML schema parser that produces a 'bdem_Schema' object.
//
//@CLASSES:
//  baexml_SchemaParser: XML schema parser
//
//@SEE_ALSO: bdem_schema, baexml_schemagenerator, baexml_decoder
//
//@AUTHOR: Pablo Halpern (phalpern), Alexander Libman (alibman1)
//
//@DESCRIPTION: The XML format for structured text can effectively represent
// the concepts embodied in the 'bdem' family of in-core data structures:
// 'bdem_Choice', 'bdem_ChoiceArray', 'bdem_List', 'bdem_Table', and
// 'bdem_Schema'.  An XML schema (XSD) document describes and constrains XML
// instance documents in much the same way that a 'bdem_Schema' object
// describes and constrains 'bdem' aggregates ('bdem_Choice',
// 'bdem_ChoiceArray', 'bdem_List', and 'bdem_Table').
//
// This component translates an XSD document (conforming to a subset of the XML
// Schema specification) into an isomorphic 'bdem_Schema' object.  A companion
// component, 'DataParser', translates an XML data document into a 'bdem_List'
// object.  Given an XSD document and an XML document such that the XSD
// document describes and validates the the XML document, the two parsers can
// be used to construct a ('bdem_Schema', 'bdem_List') pair such that the
// 'bdem_Schema' describes and validates the 'bdem_List'.
//
///XML Schema Format
///-----------------
// The format of XML schemas is described by the XML schema structures
// specification (http://www.w3.org/TR/xmlschema-1/).  An XSD document is
// itself an XML document and consists primarily of three constructs: <schema>,
// <element>, and <complexType>.  The following simple XSD document illustrates
// the use of these basic constructs:
//..
//  <?xml version="1.0" encoding="UTF-8"?>
//
//  <schema xmlns="http://www.w3.org/2001/XMLSchema"
//          xmlns:bb="http://bloomberg.com/schemas/memo"
//          targetNamespace="http://bloomberg.com/schemas/memo"
//          elementFormDefault="qualified">
//
//      <element name="memo">
//          <complexType>
//              <sequence>
//                  <element name="to" type="string"/>
//                  <element name="from" type="string"/>
//                  <element name="date" type="date"/>
//                  <element name="contents" maxOccurs="unbounded">
//                      <complexType>
//                          <sequence>
//                              <element name="indent" type="int"/>
//                              <element name="text" type="string"/>
//                          </sequence>
//                      </complexType>
//                  </element>
//              </sequence>
//          </complexType>
//      </element>
//
//  </schema>
//..
// Note that element names and types may have identical names without creating
// a conflict.  Hence, we can have an element with the name, "date", and the
// same (or different) element with the type, "date".  A 'baexml_SchemaParser'
// object would construct the following 'bdem_Schema' object from the above
// XSD document:
//..
//  {
//      RECORD {
//          INT "indent";
//          STRING "text";
//      }
//
//      RECORD "memo" {
//          STRING "to";
//          STRING "from";
//          DATE "date";
//          TABLE[0] "contents";
//      }
//  }
//..
// Essentially the same structure can be expressed in an XSD file this way:
//..
//  <?xml version="1.0" encoding="UTF-8"?>
//
//  <schema xmlns="http://www.w3.org/2001/XMLSchema"
//          targetNamespace="http://bloomberg.com/schemas/memo"
//          xmlns:xs="http://www.w3.org/2001/XMLSchema"
//          xmlns:bb="http://bloomberg.com/schemas/memo"
//          elementFormDefault="qualified">
//
//      <complexType name="memoType">
//          <sequence>
//              <element name="to" type="string"/>
//              <element name="from" type="string"/>
//              <element name="date" type="date"/>
//              <element name="contents" type="bb:paragraph"
//                       maxOccurs="unbounded"/>
//          </sequence>
//      </complexType>
//
//      <complexType name="paragraph">
//          <sequence>
//              <element name="indent" type="int"/>
//              <element name="text" type="string"/>
//          </sequence>
//      </complexType>
//
//      <element name="memo" type="bb:memoType"/>
//
//  </schema>
//..
// When the above XML schema is parsed, the resulting 'bdem_Schema' object is
// the same as in the previous example except that the first record is given a
// name, "paragraph", and is referenced in the "memo" record as follows:
//..
//   TABLE<"paragraph">
//..
// Here is the complete 'bdem_Schema' generated for the above XSD file:
//..
//  {
//      RECORD "paragraph" {
//          INT "indent";
//          STRING "text";
//      }
//
//      RECORD "memo" {
//          STRING "to";
//          STRING "from";
//          DATE "date";
//          TABLE<"paragraph"> "contents";
//      }
//  }
//..
// Both schemas describe the same set of legal XML data files of which one
// possible file is shown below:
//..
//  <?xml version="1.0" encoding="UTF-8"?>
//  <memo xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
//        xsi:schemaLocation="http://bloomberg.com/schemas/memo docEx1.xsd"
//        xmlns="http://bloomberg.com/schemas/memo">
//      <to>Ben Franklin</to>
//      <from>Thomas Jefferson</from>
//      <date>1776-07-03</date>
//      <contents>
//          <indent>5</indent>
//          <text>Hey, Ben, I think I've figured the problem passage:</text>
//      </contents>
//      <contents>
//          <indent>10</indent>
//          <text>
//               We hold these truths to be self-evident, that all men are
//               created equal, that they are endowed by their creator, with
//               certain inalienable rights...
//          </text>
//      </contents>
//  </memo>
//..
///Translation of XML Schema Constructs
///------------------------------------
// The 'baexml_SchemaParser' class recognizes and translates a specific subset
// of the XML schema structures specification.  An XML schema is itself an XML
// document, described using XML constructs ('<element>', '<simpleType>',
// '<complexType>', etc.), attributes ('name', 'type', etc.), and text data.
// Any *attribute* or *data* *value* that cannot be represented in
// 'bdem_Schema' is ignored.  Any *schema* *construct* that is not recognized
// or cannot be translated into 'bdem_Schema' is either ignored or results in
// an error, as listed below.
//
// The parser translates a '<schema>' construct into a 'bdem_Schema' object, a
// '<complexType>' construct within a '<schema>' into a 'bdem_RecordDef' object
// within the 'bdem_Schema', and an '<element>' construct within a
// '<complexType>' into a 'bdem_FieldDef' object within the 'bdem_RecordDef'.
// Other XML schema constructs modify the parse in various ways, as described
// below.  The term "schema-level" is used to describe a construct nested
// immediately within the schema document's '<schema>' construct.
//
// Each XSD construct is translated as follows:
//..
// <schema [targetNamespace="http://bloomberg.com/schemas/targetNs"] ...>
//..
//    Always the root element of an XSD document, this construct describes the
//    entire 'bdem_Schema' object.  It may contain attributes that define
//    namespaces, including the target namespace for XML elements that are
//    being defined in this schema.
//..
// <complexType [name="name"]>
//..
//    Describes a 'bdem_RecordDef' object.  A schema-level '<complexType>' must
//    have a name, which is assigned to its 'bdem_RecordDef'.  Only
//    schema-level '<complexType>' constructs may have names.
//
//    Only those '<complexType>' constructs that are directly or indirectly
//    "reachable" from a "root" element (see '<element>' below) will result
//    in the creation of 'bdem_RecordDef' objects.
//..
// <element [name="name"] [type="type"] [minOccurs="0"] [maxOccurs="num"]
//          [default="defaultValue"] [nillable="false"] [bdem:id="3"]
//          [ref="otherElement"]/>
//..
//    The '<element>' construct declares an XML data element.  An '<element>'
//    construct must always have either a 'name' attribute or a 'ref' attribute
//    and must have a type.  Its type can be specified with a 'type' attribute,
//    a 'ref' attribute, or a nested '<complexType>' or '<simpleType>'
//    construct.
//
//    An '<element>' construct can appear in two different types of locations
//    in an XSD file.  An '<element>' construct nested within a
//    '<complexType>' construct (see '<sequence>' and '<choice>', below)
//    describes a 'bdem_FieldDef'.  An '<element>' construct appearing at the
//    schema-level identifies a "root" element in conforming XML data
//    documents.  In the above example, "memo" is the root element.  The
//    'bdem_RecordDef' corresponding to the '<complexType>' directly
//    referenced by a schema-level '<element>' construct inherits its name
//    from that '<element>', overriding any name explicitly given to the
//    '<complexType>' construct itself.  In our example, the schema-level
//    '<element>' name "memo" overrides the '<complexType>' name "memoType" so
//    that the latter does not show up in the populated 'bdem_Schema' object
//    for the corresponding 'bdem_RecordDef'.
//
//    If the 'maxOccurs' attribute is present for an '<element>' construct
//    within a '<complexType>' construct, and it has a value of "unbounded" or
//    a numeric value greater than "1", then the corresponding 'bdem_FieldDef'
//    for the '<element>' construct is treated as an array or a table,
//    depending on whether the '<element>' construct's type is a primitive
//    type (or choice) or complex type, respectively.  The schema parser
//    treats all values greater than one as if they were "unbounded", but the
//    exact constraint can be enforced by using a validating data parser and
//    providing the original XSD file.  (See "Does not Preserve Constraints,"
//    below.)
//
//    If the 'minOccurs' attribute is present for an '<element>' construct
//    within a '<complexType>' construct, and it has a value of zero, then the
//    corresponding 'bdem_FieldDef' generated from the '<element>' construct
//    is treated as nullable (i.e., 'isNullable' will return true) if and only
//    if 'maxOccurs' has value 1 (or is absent) AND there is no 'default'
//    attribute.  The schema parser treats all non-zero values as if they were
//    "1", but the exact constraint can be enforced by using a validating data
//    parser and providing the original XSD file.  (See "Does not Preserve
//    Constraints," below.)
//
//    The 'default' attribute is used to set the default value for the field
//    definition.  It is useful only simple types.  If 'maxOccurs' is
//    "unbounded" or greater than zero, then the default value applies to
//    every element of the resulting array.
//
//    If the 'nillable' attribute is present for an '<element>' construct,
//    then its value is used to set the 'IS_NILLABLE' formatting flag for the
//    field.  A field that is nillable but not nullable is represented
//    differently in XML data.
//
//    If the 'bdem:id' attribute is present, then the field is assigned the
//    specified ID within its containing record definition.  By default, each
//    field is has an ID that is equal to its field index.  This attribute is
//    useful for schema evolution; e.g., to remove a field without changing
//    the IDs of subsequent fields.
//
//    The 'ref' attribute may not be specified in combination with 'name',
//    'type', or 'default'.  If supplied, these attributes are copied from
//    the referenced element, which must be a root element in the schema.  A
//    root element may not have a 'ref' attribute.
//..
// <all>
// <choice>
// <sequence>
//..
//    Within a 'complexType', contains the list of elements that describe the
//    fields of the constructed 'bdem_RecordDef' object.  The type of the
//    'bdem_RecordDef' created for a 'complexType' containing a '<choice>'
//    child element will be 'bdem_RecordDef::BDEM_CHOICE_RECORD'.  In the case
//    of '<all>' and '<sequence>', the type of the corresponding
//    'bdem_RecordDef' will be bdem_RecordDef::BDEM_SEQUENCE_RECORD.  Although
//    the '<all>' and '<sequence>' constructs generate the same
//    'bdem_RecordDef' output, they have different semantics when used by the
//    data parser for validating XML data input.  The '<all>' construct allows
//    the fields in a conforming XML file to be specified in any order but does
//    not permit any of the fields to have a 'maxOccurs' value greater than 1
//    (e.g., no tables).  The '<sequence>' construct mandates the order of
//    fields, but allows any or all of them to be repeated.
//
//    Note that the XML Schema standard allows '<choice>' to be nested within
//    '<sequence>' or '<all>' to represent an "anonymous" choice, analogous to
//    an anonymous union in C++.  Anonymous choices are not supported by the
//    schema parser at this time.
//..
// <simpleType>
// <restriction>
//..
//    Symbolic constructs used to create synonyms for basic types and
//    constrained versions of basic types.
//..
// <enumeration name="name" [bdem:id="2"]>
//..
//    A '<simpleType>' construct containing at least one '<enumeration>'
//    construct will describe a 'bdem_EnumerationDef' object within the
//    'bdem_Schema'.  Each '<enumeration>' (which must be nested within a
//    '<restriction>' construct) describes one enumerator with the specified
//    name.  If 'bdem:id' is present, its value is used to set the numeric ID
//    for the enumerator.  If none of the '<enumeration>' constructs within a
//    given '<simpleType>' contain the 'bdem:id' attribute, then IDs are
//    assigned automatically in alphabetical order.  Assigning IDs
//    alphabetically is consistent with the ASN.1 formula for assigning
//    integer IDs to enumerators.
//..
// <list itemType="type">
//..
//    Nested within a 'simpleType', specifies that the type is an array.  For
//    example, '<list itemType="int"/>' defines a (possibly empty) array of
//    integers.
//..
// <include schemaLocation="location">
// <import namespace="extNamespace" [schemaLocation="location"]>
//..
//    These constructs allow external XSD documents to be merged into this
//    schema as per the XML Schema standard.  The '<include>' construct is
//    roughly like '#include' in C.  The '<import>' construct is more
//    sophisticated and allows inclusion of schema constructs from other
//    namespaces.  See http://www.w3.org/TR/xmlschema-1/#element-include and
//    http://www.w3.org/TR/xmlschema-1/#element-import for more information on
//    '<include>' and '<import>', respectively.
//
//    To assist the schema parser in finding the included or imported
//    documents, the user should call 'setSchemaResolver', passing it a
//    functor that will resolve a schema given a location and a namespace.
//
//    WARNING: Support for '<import>' is currently incomplete.  In particular,
//    the namespace is ignored for constructs that are imported from other
//    namespaces and an error will result if there are any name collisions
//    after namespace prefixes are discarded.
//..
// <complexContent>
// <simpleContent>
//..
//    Syntactic constructs needed by the XML parser, but having no observable
//    effect on the constructed 'bdem_Schema'.
//..
// <annotation>
// <appinfo>
// <attribute>
// <attributeGroup>
// <documentation>
// <field>
// <fractionDigits>
// <key>
// <keyref>
// <length>
// <maxExclusive>
// <maxInclusive>
// <maxLength>
// <minExclusive>
// <minInclusive>
// <minLength>
// <notation>
// <pattern>
// <selector>
// <totalDigits>
// <unique>
// <whiteSpace>
//..
//    The above constructs are skipped by 'baexml_SchemaParser' and have no
//    effect on the constructed 'bdem_Schema'.  Any nested constructs are also
//    skipped.  Note that syntactic and semantic correctness may be enforced
//    on some of these constructs, even though they don't change the resulting
//    value.
//..
// <any>
// <anyAttribute>
// <extension>
// <group>
// <redefine>
// <union>
//..
//    'baexml_SchemaParser' will produce an error on any of the above XML
//    schema elements.
//
///Data Type Translations
///----------------------
// There are 46 built-in types in the XML Schema specification (see
// http://www.w3.org/TR/xmlschema-2/#built-in-datatypes) but only 14 scalar
// types defined in the 'bdem' package.  When translating an XML schema into a
// bdem schema, the schema parser maps XML data types into bdem data types
// using a many-to-one mapping, as follows:
//..
//  XML Data Type         bdem_ElementType  bdeat_FormattingMode
//  -------------         ----------------  --------------------
//  ENTITIES              STRING_ARRAY      LIST
//  ENTITY                STRING
//  ID                    STRING
//  IDREF                 STRING
//  IDREFS                STRING_ARRAY      LIST
//  NCName                STRING
//  NMTOKEN               STRING
//  NMTOKENS              STRING_ARRAY      LIST
//  QName                 STRING
//  anyURI                STRING
//  base64Binary          CHAR_ARRAY        BASE64
//  boolean               BOOL
//  byte                  CHAR              DEC
//  date                  DATETZ
//  dateTime              DATETIMETZ
//  decimal               DOUBLE
//  double                DOUBLE
//  float                 FLOAT
//  hexBinary             CHAR_ARRAY        HEX
//  int                   INT
//  integer               INT64
//  language              STRING
//  long                  INT64
//  name                  STRING
//  negativeInteger       INT64
//  nonNegativeInteger    INT64
//  nonPositiveInteger    INT64
//  normalizedString      STRING
//  positiveInteger       INT64
//  short                 SHORT
//  string                STRING
//  time                  TIMETZ
//  token                 STRING
//  unsignedByte          SHORT
//  unsignedInt           INT64
//  unsignedLong          INT64
//  unsignedShort         INT
//..
// The following XML data types are not supported by the schema parser and
// will result in a parse error if encountered in an XSD file:
//..
//  NOTATION
//  anySimpleType
//  anyType
//  duration
//  gDay
//  gMonth
//  gMonthDay
//  gYear
//  gYearMonth
//..
///Special Type and Element Names
///------------------------------
// If a '<complexType>' or a schema-level '<element>' has the name 'RECORD_##',
// where '##' represents a sequence of one or more decimal digits ('0' to
// '9'), then the name is assumed to be a synthetic placeholder for an
// anonymous record and the constructed 'bdem_RecordDef' is not given a name.
// No attempt is made to position the 'bdem_RecordDef' such that its index
// within the 'bdem_Schema' corresponds to the value of '##' (see
// "Limitations" below).
//
// If a '<simpleType>' has the name 'ENUMERATION_##' where '##' represents a
// sequence of one or more decimal digits ('0' to '9'), then the name is
// assumed to be a synthetic placeholder for an anonymous enumeration and the
// constructed 'bdem_EnumerationDef' is not given a name.  No attempt is made
// to position the 'bdem_EnumerationDef' such that its index within the
// 'bdem_Schema' corresponds to the value of '##'.
//
// If an '<element>' within a '<complexType>' has the name 'FIELD_##', where
// '##' represents a sequence of one or more decimal digits ('0' to '9'), then
// the name is assumed to be a synthetic placeholder for an anonymous field and
// the constructed 'bdem_FieldDef' is not given a name.  No attempt is made to
// position the 'bdem_FieldDef' such that its index within its parent
// 'bdem_RecordDef' corresponds to the value of '##' (see "Limitations" below).
// However, fields are added to records in order, so if '<element>' 'FIELD_n'
// appears at position 'n' within the list of elements in the '<complexType>'
// (counting from zero), then its index within the 'bdem_RecordDef' object
// *will* correspond to 'n'.  For example, if 'FIELD_2' is the third element in
// a 'complexType', then a 'bdem_FieldDef' will be constructed at position 2
// within its parent 'bdem_RecordDef'.
//
///Limitations
///-----------
// XSD is an expressive format for defining and constraining the contents of a
// wide variety of XML documents.  'baexml_SchemaParser' translates only a
// *limited* *subset* of the entire XML schema specification.  Some elements
// of a valid XML schema result in errors when attempting to translate to
// 'bdem_Schema'; others are simply ignored, as listed above.  Below are some
// specific limitations in the handling of XML schema (XSD) documents.
//
///Does not Validate Fully
///- - - - - - - - - - - -
// 'baexml_SchemaParser' does not fully check its input stream to ensure that
// it conforms to valid XSD syntax.  For example, the following is not legal
// XML schema (it has too many '<sequence>' elements), but will not produce an
// error when read by 'baexml_SchemaParser':
//..
//  <complexType>
//      <sequence>
//          <element name="a" type="int"/>
//      <sequence>
//      </sequence>
//          <element name="b" type="string"/>
//      </sequence>
//  <complexType>
//..
// The above schema will be interpreted as if it were written:
//..
//  <complexType>
//      <sequence>
//          <element name="a" type="int"/>
//          <element name="b" type="string"/>
//      </sequence>
//  <complexType>
//..
// If, however, the same XSD text is used by a validating reader to validate
// XML data, the schema syntax will be fully validated.  Furthermore,
// additional validity checks are being added continuously, so it would be
// unwise to depend on the schema parser to ignore certain errors.
//
///Does not Preserve Constraints
/// - - - - - - - - -- - - - - -
// The 'bdem_FieldDef' class does not have provisions for constraints on
// values (except for structural constraints on choice, choice array, list,
// table values, and enumeration constraints on strings).
// 'baexml_SchemaParser' must therefore ignore useful information in the XML
// schema when constructing the 'bdem_Schema' object.  However, if the XML
// schema text is supplied to the XML decoder along with a validating reader,
// then the XML parse phase will enforce these constraints even though
// 'bdem_Schema' does not.
//
///No Support for Groups
///- - - - - - - - - - -
// The '<group>' construct is not supported at this time, although there is
// an obvious mapping from the XML Schema concept to 'bdem_Schema' concepts.
// This construct may be supported in a future release.
//
///No Support for Unconstrained Choices, Choice Arrays, Lists, and Tables
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A 'bdem_FieldDef' with type 'CHOICE', 'CHOICE_ARRAY', 'LIST', or 'TABLE' is
// not required to be constrained by a 'bdem_RecordDef' describing the
// structure of the respective aggregate.  However, the schema parser does not
// support any XSD representation of such unconstrained aggregates.  This is
// deliberate; the use of unconstrained aggregates in 'bdem_Schema' is
// discouraged.
//
///XML Attributes are Limited to Sequence Records
/// - - - - - - - - - - - - - - - - - - - - - - -
// The '<attribute>' construct in XML schema defines an XML attribute.  XML
// attributes are typically used to specify meta-data about the data within an
// XML element (e.g., its ID).  There are no 'bdem' constructs with similar
// meaning to XML attributes.  This schema parser will translate '<attribute>'
// constructs as additional fields within a sequence record, using the
// 'IS_ATTRIBUTE' formatting mode to flag the field for converting the
// 'bdem_Schema' back to XSD format.  This works well for '<complexType>'
// constructs containing a '<sequence>' or '<all>', but does not work for
// '<simpleType>' or '<choice>', as there is no place in the 'bdem_Schema'
// data structure to store these attribute fields in a scalar or choice
// record.  Any attempt to add attributes to a scalar or choice will result in
// an error.  Also, an attribute name cannot conflict with an element name
// within the same complex type or else an error will be generated.
//
///Generate and Parse are not Fully Symmetric
/// - - - - - - - - - - - - - - - - - - - - -
// If the 'bdem_Schema' object constructed by 'baexml_SchemaParser' is fed to
// 'SchemaGenerator', the resulting XML schema is unlikely to match the
// original input stream.  There are a number of reasons for this.
//
// First, since XML schema is much richer than 'bdem_Schema' (e.g., in its
// support for data types and constraints), many constraint attributes and
// constructs in the original XML schema are discarded when translating to
// 'bdem_Schema'.  All annotations, comments, documentation and whitespace are
// similarly discarded.
//
// Second, in XML schema, there are several ways to represent the same 'bdem'
// concept.  For example, a '<complexType>' can be nested directly within an
// '<element>' or can appear at schema-level and be referenced by the
// '<element>'.  The 'SchemaGenerator' must choose one representation, which
// may be different than the original.  The worst of these differences is in
// the representation of arrays.  The following two elements are both
// translated into 'INT_ARRAY' types in 'bdem_FieldDef':
//..
//  <element name="a1" type="int" maxOccurs="unbounded"/>
//  <element name="a2">
//      <simpleType>
//          <list itemType="int"/>
//      </simpleType>
//  </element>
//..
// In 'bdem', these two elements have the same type, but XML data that will
// validate against them is quite different:
//..
//  <a1>4</a1>
//  <a1>5</a1>
//  <a1>6</a1>
//  <a2>0 1 2 3</a2>
//..
// The 'SchemaGenerator' class will always generate the first style for arrays
// of string and the second style for all other arrays.
//
// Third, 'bdem_Schema' supports unnamed records and unnamed fields which can
// be referenced by index.  XML has no concept of an unnamed element.
// 'SchemaGenerator' generates XML names for these unnamed entities by
// appending their index to "RECORD_" or "FIELD_".  As stated previously,
// 'baexml_SchemaParser' does not attempt to preserve the indexes of these
// unnamed entities, although it does correctly discard the name when
// constructing the 'bdem_Schema' object.
//
// Finally, the ordering of '<complexType>' constructs in the XML schema is not
// preserved when constructing the 'bdem_RecordDef' objects in the
// 'bdem_Schema'.
//
// All this having been said, translating back-and-forth between XML schema
// and 'bdem_Schema' will produce byte-for-byte matches after four
// translations.  I.e, starting with a file, 'xml1.xsd' and successively
// translating it back and forth to 'bdem_Schema':
//..
//  xml1.xsd -> bdemSchema1 -> xml2.xsd -> bdemSchema2 -> xml3.xsd
//..
// 'bdem_Schema1' will compare equal to 'bdemSchema2' (by 'operator==') and
// 'xml2.xsd' will have the same contents as 'xml3.xsd'.
//
///Usage
///-----
// Frequently, the schema for a 'bdem_List' can be hard-wired into the
// program in the form of an string constant containing XSD data.  The
// following string describes a simple schema containing two records,
// "mailOrder" and "item":
//..
//  const char XML_SCHEMA[] =
//      "<?xml version='1.0' encoding='UTF-8'?>\n"
//      "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
//      "           xmlns:bb='http://bloomberg.com/schemas/mailOrder'\n"
//      "           xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
//      "           xmlns='http://www.w3.org/2001/XMLSchema'\n"
//      "           targetNamespace='http://bloomberg.com/schemas/mailOrder'\n"
//      "           elementFormDefault='qualified'>\n"
//      "\n"
//      "    <element name='mailOrder'>\n"
//      "        <complexType>\n"
//      "            <sequence>\n"
//      "                <element name='orderDate' type='date'/>\n"
//      "                <element name='customerName' type='string'/>\n"
//      "                <element name='items' type='bb:item'\n"
//      "                         maxOccurs='unbounded'/>\n"
//      "            </sequence>\n"
//      "        </complexType>\n"
//      "    </element> \n"
//      "\n"
//      "    <simpleType name='ids'>"
//      "        <restriction base='string'>"
//      "            <enumeration value='REAR_SETS' bdem:id='0'/>"
//      "            <enumeration value='DAMPER' bdem:id='1'/>"
//      "            <enumeration value='POWER_COMMANDER' bdem:id='2'/>"
//      "        </restriction>"
//      "    </simpleType>"
//      "\n"
//      "    <complexType name='item'>\n"
//      "        <sequence>\n"
//      "            <element name='itemName' type='string'/>\n"
//      "            <element name='id' type='bb:ids'/>\n"
//      "            <element name='size' type='double'/>\n"
//      "        </sequence>\n"
//      "    </complexType>\n"
//      "\n"
//      "</xs:schema>\n";
//..
// In order to parse the XML schema, we first need to construct an
// 'baexml_SchemaParser' object and a 'bdem_Schema' object:
//..
//  baexml_MiniReader reader;
//  baexml_ErrorInfo  errorInfo;
//..
// Given 'reader', we can now create the following 'parser'; we also supply a
// 'baexml_ErrorInfo' and diagnostic stream, both of which are optional.  In
// our case, we use 'bsl::cerr' for the diagnostic stream:
//..
//  baexml_SchemaParser parser(&reader, &errorInfo, &bsl::cerr);
//  bdem_Schema         schema;
//..
// The parser expects an 'istream' or 'streambuf' argument, so we must create
// a stream to read from the static XSD character array:
//..
//  bsl::istringstream xmlStream(XML_SCHEMA);
//..
// Now we call the 'parse' method to construct the 'bdem_Schema' object from
// the XSD input text.  We invent a file name, "mailOrder.xsd" to be used in
// error messages, and pass the address of variables in which to capture error
// information:
//..
//  bsl::string targetNamespace;
//  int result = parser.parse(xmlStream, &schema, &targetNamespace,
//                            "mailOrder.xsd");
//  assert(0 == result);
//  assert("http://bloomberg.com/schemas/mailOrder" == targetNamespace);
//..
// Display errors, if any:
//..
//  if (0 != result) {
//      bsl::cerr << "mailOrder.xsd:" << errorInfo << bsl::endl;
//  }
//..
// Confirm that the 'bdem_Schema' object has the desired structure:
//..
//  const bdem_RecordDef *mailOrderRec = schema.lookupRecord("mailOrder");
//  assert(0 != mailOrderRec);
//  const int mailOrderRecLen = (mailOrderRec ? mailOrderRec->numFields() : 0);
//  assert(3 == mailOrderRecLen);
//
//  const bdem_RecordDef *itemRec = schema.lookupRecord("item");
//  assert(0 != itemRec);
//  const int itemRecLen = (itemRec ? itemRec->numFields() : 0);
//  assert(3 == itemRecLen);
//
//  const bdem_EnumerationDef *idsEnum = schema.lookupEnumeration("ids");
//  assert(0 != idsEnum);
//  const int idsEnumLen = (idsEnum ? idsEnum->numEnumerators() : 0);
//  assert(3 == idsEnumLen);
//
//  const bdem_FieldDef *dateField = mailOrderRec->lookupField("orderDate");
//  assert(0 != dateField);
//  if (0 < mailOrderRecLen) assert(&mailOrderRec->field(0) == dateField);
//  if (0 != dateField)
//      assert(bdem_ElemType::BDEM_DATETZ == dateField->elemType());
//
//  const bdem_FieldDef *nameField = mailOrderRec->lookupField("customerName");
//  assert(0 != nameField);
//  if (1 < mailOrderRecLen) assert(&mailOrderRec->field(1) == nameField);
//  if (0 != nameField)
//      assert(bdem_ElemType::BDEM_STRING == nameField->elemType());
//
//  const bdem_FieldDef *itemsField = mailOrderRec->lookupField("items");
//  assert(0 != itemsField);
//  if (2 < mailOrderRecLen) assert(&mailOrderRec->field(2) == itemsField);
//  if (0 != itemsField) {
//      assert(bdem_ElemType::BDEM_TABLE == itemsField->elemType());
//      assert(itemRec == itemsField->constraint());
//  }
//
//  const bdem_FieldDef *itemNameField = itemRec->lookupField("itemName");
//  assert(0 != itemNameField);
//  if (0 < itemRecLen) assert(&itemRec->field(0) == itemNameField);
//  if (0 != itemNameField) {
//      assert(bdem_ElemType::BDEM_STRING == itemNameField->elemType());
//  }
//
//  const bdem_FieldDef *idField = itemRec->lookupField("id");
//  assert(0 != idField);
//  if (1 < itemRecLen) assert(&itemRec->field(1) == idField);
//  if (0 != idField) {
//      assert(bdem_ElemType::BDEM_STRING == itemNameField->elemType());
//  }
//
//  const bdem_FieldDef *sizeField = itemRec->lookupField("size");
//  assert(0 != sizeField);
//  if (2 < itemRecLen) assert(&itemRec->field(2) == sizeField);
//  if (0 != sizeField)
//      assert(bdem_ElemType::BDEM_DOUBLE == sizeField->elemType());
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEXML_READER
#include <baexml_reader.h>
#endif

#ifndef INCLUDED_BDEUT_STRINGREF
#include <bdeut_stringref.h>
#endif

#ifndef INCLUDED_BSL_ISTREAM
#include <bsl_istream.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

class bdem_Schema;

                        // =========================
                        // class baexml_SchemaParser
                        // =========================

class baexml_SchemaParser {
    // Translate an XML schema document into an isomorphic 'bdem_Schema'
    // object.

  public:
    // TYPES
    typedef bsl::map<bsl::string, bsl::string> SchemaElementAttributes;
        // 'SchemaElementAttributes' is an alias for a map storing the
        // attributes of the top level 'schema' element as name-value pairs.

  private:
    // DATA
    baexml_Reader    *d_reader;        // for parsing XML (held, not owned)
    baexml_ErrorInfo *d_errorInfo;     // for reporting error (held, not owned)
    bsl::ostream     *d_verboseStream; // for verbose output (held, not owned)

    // NOT IMPLEMENTED
    baexml_SchemaParser(const baexml_SchemaParser&);
    baexml_SchemaParser& operator=(const baexml_SchemaParser&);

    // PRIVATE MANIPULATORS
    int parse(bsl::streambuf          *inputStream,
              bdem_Schema             *schema,
              bsl::string             *targetNamespace,
              SchemaElementAttributes *attributes,
              const bdeut_StringRef&   inputId);
        // Parse the XML schema text in 'inputStream' and load the translated
        // contents into the specified 'schema' object; return a non-negative
        // value on success and a negative value on error.  If the specified
        // 'targetNamespace' is non-null, fill in the value of the target
        // namespace from the XML schema text.  If the specified
        // 'attributes' is non-null, fill in the value of the attributes of the
        // schema element from the XML schema text.  If the specified 'inputId'
        // string is provided, it is used for error handling only and
        // represents the name (URI or file name) of the input stream.  The
        // previous contents of '*schema' are destroyed.

  public:
    // CREATORS
    baexml_SchemaParser(baexml_Reader    *reader,
                        baexml_ErrorInfo *errorInfo = 0,
                        bsl::ostream     *verboseStream = 0);
        // Construct a schema parser object using the specified 'reader' to
        // perform the XML-level parsing.  If 'errorInfo' is non-null, it is
        // used to store a message string, line number, and column number in
        // case of a parse error and are unmodified otherwise.

    // ~baexml_SchemaParser();
        // Destroy this schema parser object.  Note that this trivial
        // destructor is generated by the compiler.

    // MANIPULATORS
    int parse(bsl::istream&           inputStream,
              bdem_Schema            *schema,
              bsl::string            *targetNamespace = 0,
              const bdeut_StringRef&  inputId = bdeut_StringRef());

    int parse(bsl::streambuf         *inputStream,
              bdem_Schema            *schema,
              bsl::string            *targetNamespace = 0,
              const bdeut_StringRef&  inputId = bdeut_StringRef());
        // Parse the XML schema text in 'inputStream' and load the translated
        // contents into the specified 'schema' object; return a non-negative
        // value on success and a negative value on error.  If the specified
        // 'targetNamespace' is non-null, fill in the value of the target
        // namespace from the XML schema text.  If the specified 'inputId'
        // string is provided, it is used for error handling only and
        // represents the name (URI or file name) of the input stream.  The
        // previous contents of '*schema' are destroyed.

    int parse(bsl::istream&            inputStream,
              bdem_Schema             *schema,
              SchemaElementAttributes *attributes,
              const bdeut_StringRef&   inputId = bdeut_StringRef());

    int parse(bsl::streambuf          *inputStream,
              bdem_Schema             *schema,
              SchemaElementAttributes *attributes,
              const bdeut_StringRef&   inputId = bdeut_StringRef());
        // Parse the XML schema text in 'inputStream', load the translated
        // contents into the specified 'schema' object, and load the attributes
        // of the 'schema' element as name/value pairs into the specified
        // 'attributes' map; return a non-negative value on success and
        // a negative value on error.  If the specified 'inputId' string is
        // provided, it is used for error handling only and represents the
        // name (URI or file name) of the input stream.  The previous contents
        // of '*schema' are destroyed.

    void setErrorInfo(baexml_ErrorInfo *errorInfo);
        // Set the specified 'errorInfo' object as the place where error
        // messages are stored.  If 'errorInfo' is null, discard error
        // messages.

    void setVerboseStream(bsl::ostream *verboseStream);
        // Set the output stream for verbose/debug output to the specified
        // 'verboseStream'.  If 'verboseStream' is null, then no verbose
        // output will be generated.

    // ACCESSORS
    baexml_Reader *reader() const;
        // Return the 'baexml_Reader' used by this schema parser.

    baexml_ErrorInfo *errorInfo() const;
        // Return the object in which error messages are put or a zero pointer
        // if error messages are being discarded.

    bsl::ostream *verboseStream() const;
        // Return the stream used for verbose output or a zero pointer if
        // verbose output is disabled.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // -------------------------
                        // class baexml_SchemaParser
                        // -------------------------

// CREATORS
inline
baexml_SchemaParser::baexml_SchemaParser(baexml_Reader    *reader,
                                         baexml_ErrorInfo *errorInfo,
                                         bsl::ostream     *verboseStream)
: d_reader       (reader)
, d_errorInfo    (errorInfo)
, d_verboseStream(verboseStream)
{
}

// MANIPULATORS
inline
int baexml_SchemaParser::parse(bsl::istream&           inputStream,
                               bdem_Schema            *schema,
                               bsl::string            *targetNamespace,
                               const bdeut_StringRef&  inputId)
{
    return parse(inputStream.rdbuf(), schema, targetNamespace, 0, inputId);
}

inline
int baexml_SchemaParser::parse(bsl::istream&            inputStream,
                               bdem_Schema             *schema,
                               SchemaElementAttributes *attributes,
                               const bdeut_StringRef&   inputId)
{
    return parse(inputStream.rdbuf(), schema, 0, attributes, inputId);
}

inline
int baexml_SchemaParser::parse(bsl::streambuf         *inputStream,
                               bdem_Schema            *schema,
                               bsl::string            *targetNamespace,
                               const bdeut_StringRef&  inputId)
{
    return parse(inputStream, schema, targetNamespace, 0, inputId);
}

inline
int baexml_SchemaParser::parse(bsl::streambuf          *inputStream,
                               bdem_Schema             *schema,
                               SchemaElementAttributes *attributes,
                               const bdeut_StringRef&   inputId)
{
    return parse(inputStream, schema, 0, attributes, inputId);
}

inline
void baexml_SchemaParser::setVerboseStream(bsl::ostream *verboseStream)
{
    d_verboseStream = verboseStream;
}

inline
baexml_ErrorInfo* baexml_SchemaParser::errorInfo() const
{
    return d_errorInfo;
}

inline
baexml_Reader* baexml_SchemaParser::reader() const
{
    return d_reader;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
