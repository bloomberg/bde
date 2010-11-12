// baexml_schemagenerator.t.cpp                  -*-C++-*-

#include <baexml_schemagenerator.h>

#include <bdem_elemref.h>                    // for testing only
#include <bdem_elemtype.h>                   // for testing only
#include <bdem_schema.h>

#include <bdeat_formattingmode.h>

#include <bdetu_unset.h>                     // for testing only

#include <bdet_datetime.h>                   // for testing only
#include <bdet_datetimetz.h>                 // for testing only
#include <bdet_date.h>                       // for testing only
#include <bdet_datetz.h>                     // for testing only
#include <bdet_time.h>                       // for testing only
#include <bdet_timetz.h>                     // for testing only

#include <bslma_testallocator.h>             // for testing only

#include <bsls_platformutil.h>               // for testing only

#include <bsl_cctype.h>
#include <bsl_climits.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_stack.h>
#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component has only one public interface (with one variation) function
// that takes a bdem_Schema as the input and generates an XML Schema (XSD)
// output into an ostream.  The test cases are broken down such that
// generating/formatting of major building blocks of a bdem_Schema and
// therefore an XSD document is tested and compared.  The test uses the
// substitution language as implemented in the helper functions from
// baexml_schemaparser.t.cpp.  The substitution language helps simplify the
// generation of bdem_Schema object and the parallel generation of the
// expected XSD output.
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
               aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { \
              bsl::cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { \
       bsl::cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { \
       bsl::cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { \
       bsl::cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
    // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
    // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", "<< bsl::flush;
    // P(X) without '\n'
#define L_ __LINE__                           // current Line number

namespace {
//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef baexml_SchemaGenerator Obj;

typedef bdem_ElemRef             ERef;
typedef bdem_ElemType            EType;
typedef bdem_FieldSpec           FldSpec;
typedef bdem_FieldDefAttributes  FldAttr;
typedef bdem_RecordDef           RecDef;
typedef RecDef::RecordType       RecType;
typedef bsls_PlatformUtil::Int64 Int64;

const char TESTNAMESPACE[] = "http://bloomberg.com/schemas/test";

    // Create Three Distinct Exemplars For Each Scalar Element Type
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const char            A00 = 'A';
const char            B00 = 'B';
const char            N00 = bdetu_Unset<char>::unsetValue();

const short           A01 = -1;
const short           B01 = -2;
const short           N01 = bdetu_Unset<short>::unsetValue();

const int             A02 = 10;
const int             B02 = 20;
const int             N02 = bdetu_Unset<int>::unsetValue();

const Int64           A03 = -100;
const Int64           B03 = -200;
const Int64           N03 =
                           bdetu_Unset<bsls_PlatformUtil::Int64>::unsetValue();

const float           A04 = -1.5;
const float           B04 = -2.5;
const float           N04 = bdetu_Unset<float>::unsetValue();

const double          A05 = 10.5;
const double          B05 = 20.5;
const double          N05 = bdetu_Unset<double>::unsetValue();

const bsl::string     A06 = "one";
const bsl::string     B06 = "two";
const bsl::string     N06 = bdetu_Unset<bsl::string>::unsetValue();

// Note: bdet_Datetime x07 implemented in terms of x08 and x09.

const bdet_Date       A08(2000,  1, 1);
const bdet_Date       B08(9999, 12,31);
const bdet_Date       N08 = bdetu_Unset<bdet_Date>::unsetValue();

const bdet_Time       A09(0, 1, 2, 3);
const bdet_Time       B09(4, 5, 6, 789);
const bdet_Time       N09 = bdetu_Unset<bdet_Time>::unsetValue();

const bdet_Datetime   A07(A08, A09);
const bdet_Datetime   B07(B08, B09);
const bdet_Datetime   N07 = bdetu_Unset<bdet_Datetime>::unsetValue();

const bool            A22 = true;
const bool            B22 = false;
const bool            N22 = bdetu_Unset<bool>::unsetValue();

const bdet_DatetimeTz A23(A07, -420);
const bdet_DatetimeTz B23(B07, 360);
const bdet_DatetimeTz N23 = bdetu_Unset<bdet_DatetimeTz>::unsetValue();

const bdet_DateTz     A24(A08, -300);
const bdet_DateTz     B24(B08, -240);
const bdet_DateTz     N24 = bdetu_Unset<bdet_DateTz>::unsetValue();

const bdet_TimeTz     A25(A09, 180);
const bdet_TimeTz     B25(B09, 660);
const bdet_TimeTz     N25 = bdetu_Unset<bdet_TimeTz>::unsetValue();

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------
//
// Substitution language
//
// All following helper functions and constants assume an baexml_Formatter of
//     initial indentation level = 0
//     spaces per indentation level = 4
//     wrap column = infinity (INT_MAX actually)
//
// These are copied from the helper functions in baexml_schemaparser.t.cpp.
// Only the constant character data is slightly modified.
// Original Author: Pablo Halpern (phalpern@bloomberg.net)
//
const char *const ELEMENTS[] = {
    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>",                  // 0

    "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\""      // 1
    " xmlns:bdem=\"http://bloomberg.com/schemas/bdem\""
    " targetNamespace=\"http://bloomberg.com/schemas/test\""
    " xmlns=\"http://bloomberg.com/schemas/test\""
    " elementFormDefault=\"qualified\">\n",

    "<xs:simpleType name=\"BYTE_LIST_TYPE\">\n"                     // 2
    "        <xs:list itemType=\"xs:byte\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"SHORT_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:short\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"INT_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:int\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"INT64_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:long\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"FLOAT_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:float\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"DOUBLE_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:double\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"DECIMAL_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:decimal\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"STRING_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:string\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"DATETIME_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:dateTime\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"DATE_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:date\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"TIME_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:time\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"BOOL_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:boolean\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"DATETIMETZ_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:dateTime\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"DATETZ_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:date\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"TIMETZ_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:time\"/>\n"
    "    </xs:simpleType>",

    "<xs:complexType>",                                             // 3
    "<xs:complexType name=\"%1\">",                                 // 4
    "<xs:element name=\"%1\"%2%3%4>",                               // 5
    "<xs:element name=\"%1\" type=\"%2\"%3%4%5>",                   // 6
    "<xs:element name=\"%1\" type=\"%2\"%3%4%5/>",                  // 7
    "<xs:element ref=\"%1\"%2%3/>",                                 // 8
    "<xs:all>",                                                     // 9
    "<xs:sequence%1%2%3>",                                          // 10
    "<xs:complexContent>",                                          // 11
    "<xs:simpleContent>",                                           // 12
    "<xs:simpleType>",                                              // 13
    "<xs:simpleType name=\"%1\">",                                  // 14
    "<xs:extension>",                                               // 15
    "<xs:restriction>",                                             // 16
    "<xs:list itemType=\"xs:%1\"/>",                                // 17
    "<xs:choice%1%2%3>",                                            // 18
    "",                                                             // 19
    "<xs:attribute ref=\"%1\"%2%3%4%5/>",                           // 20
    "<xs:attribute name=\"%1\"%2%3%4>",                             // 21
    "<xs:attribute name=\"%1\" type=\"%2\"%3%4%5/>",                // 22
};

const int NUM_ELEMENTS = sizeof ELEMENTS / sizeof *ELEMENTS;

const char *const ATTRIBUTES[] = {
    "",                                            // 0
    " minOccurs=\"0\"",                            // 1
    " maxOccurs=\"unbounded\"",                    // 2
    " minOccurs=\"1\"",                            // 3
    " default=\"65\"",                             // 4  : byte
    " default=\"-1\"",                             // 5  : short
    " default=\"10\"",                             // 6  : int
    " default=\"-100\"",                           // 7  : long
    " default=\"-1.5\"",                           // 8  : float
    " default=\"10.5\"",                           // 9  : double
    " default=\"one\"",                            // 10 : string
    " default=\"2000-01-01T00:01:02.003\"",        // 11 : dateTime (no TZ)
    " default=\"2000-01-01\"",                     // 12 : date     (no TZ)
    " default=\"00:01:02.003\"",                   // 13 : time     (no TZ)
    " default=\"true\"",                           // 14 : boolean
    " default=\"2000-01-01T00:01:02.003-07:00\"",  // 15 : dateTime (with TZ)
    " default=\"2000-01-01-05:00\"",               // 16 : date     (with TZ)
    " default=\"00:01:02.003+03:00\"",             // 17 : time     (with TZ)
    " nillable=\"false\"",                         // 18
    " use=\"required\"",                           // 19
    " bdem:id=\"0\"",                              // 20
    " bdem:id=\"1\"",                              // 21
};

const int NUM_ATTRIBUTES = sizeof ATTRIBUTES / sizeof *ATTRIBUTES;

int gDefaultIndex(char typeCode)
    // Return the index into the 'ATTRIBUTES' array of the default value
    // corresponding to the specified 'bdem' scalar 'typeCode'.
{
    enum { DEFAULT_OFFSET = 4 };  // offset in 'ATTRIBUTES' of first value

    switch (typeCode) {
      case EType::BDEM_CHAR:       return DEFAULT_OFFSET + 0;
      case EType::BDEM_SHORT:      return DEFAULT_OFFSET + 1;
      case EType::BDEM_INT:        return DEFAULT_OFFSET + 2;
      case EType::BDEM_INT64:      return DEFAULT_OFFSET + 3;
      case EType::BDEM_FLOAT:      return DEFAULT_OFFSET + 4;
      case EType::BDEM_DOUBLE:     return DEFAULT_OFFSET + 5;
      case EType::BDEM_STRING:     return DEFAULT_OFFSET + 6;
      case EType::BDEM_DATETIME:   return DEFAULT_OFFSET + 7;
      case EType::BDEM_DATE:       return DEFAULT_OFFSET + 8;
      case EType::BDEM_TIME:       return DEFAULT_OFFSET + 9;

      case EType::BDEM_BOOL:       return DEFAULT_OFFSET + 10;
      case EType::BDEM_DATETIMETZ: return DEFAULT_OFFSET + 11;
      case EType::BDEM_DATETZ:     return DEFAULT_OFFSET + 12;
      case EType::BDEM_TIMETZ:     return DEFAULT_OFFSET + 13;

      default: {
        P(typeCode);
        ASSERT("Invalid element type used in 'gDefaultIndex'" && 0);
      } break;
    }

    return 0;
}

const char *const NAMES[] = {
    "Apple",         // A
    "Banana",        // B
    "Catapult",      // C
    "Daddy",         // D
    "Effervescent",  // E
    "FIELD_0",       // F
    "FIELD_1",       // G
    "FIELD_2",       // H
    "FIELD_3",       // I
    "FIELD_4",       // J
    "FIELD_5",       // K
    "FIELD_6",       // L
    "FIELD_7",       // M
    "FIELD_8",       // N
    "FIELD_9",       // O
    "FIELD_10",      // P
    "FIELD_11",      // Q
    "RECORD_0",      // R
    "RECORD_1",      // S
    "RECORD_2",      // T
    "RECORD_3",      // U
    "RECORD_4",      // V
    "RECORD_5",      // W
    "RECORD_6",      // X
    "RECORD_7",      // Y
    "RECORD_8",      // Z
};

const int MAXNUM_NAMES = 5;
const int MAXNUM_ANONYMOUS_FIELDS = 12;
const int MAXNUM_ANONYMOUS_RECORDS = 9;

const int NUM_NAMES = sizeof NAMES / sizeof *NAMES;

const char *const XMLTYPES[] = {
    "xs:byte",                      // a
    "xs:short",                     // b
    "xs:int",                       // c
    "xs:long",                      // d
    "xs:float",                     // e
    "xs:double",                    // f
    "xs:string",                    // g
    "xs:dateTime",                  // h
    "xs:date",                      // i
    "xs:time",                      // j
    "BYTE_LIST_TYPE",               // k
    "SHORT_LIST_TYPE",              // l
    "INT_LIST_TYPE",                // m
    "INT64_LIST_TYPE",              // n
    "FLOAT_LIST_TYPE",              // o
    "DOUBLE_LIST_TYPE",             // p
    "STRING_LIST_TYPE",             // q
    "DATETIME_LIST_TYPE",           // r
    "DATE_LIST_TYPE",               // s
    "TIME_LIST_TYPE",               // t
    "xs:base64Binary",              // u
    "xs:hexBinary",                 // v
    "xs:boolean",                   // w
    "BOOL_LIST_TYPE",               // x
    "DATETIMETZ_LIST_TYPE",         // y
    "DATETZ_LIST_TYPE",             // z
// TBD problem: expectation is that all XMLTYPE codes are lower-case
//    "TIMETZ_ARRAY_TYPE",            // A
};

const int NUM_XMLTYPES = sizeof XMLTYPES / sizeof *XMLTYPES;

EType::Type gType(char typeCode)
    // Return the 'bdem' element type corresponding to the specified
    // 'typeCode'.
    //..
    //  'a' => CHAR        'k' => CHAR_ARRAY
    //  'b' => SHORT       'l' => SHORT_ARRAY
    //  'c' => INT         'm' => INT_ARRAY
    //  'd' => INT64       'n' => INT64_ARRAY
    //  'e' => FLOAT       'o' => FLOAT_ARRAY
    //  'f' => DOUBLE      'p' => DOUBLE_ARRAY
    //  'g' => STRING      'q' => STRING_ARRAY
    //  'h' => DATETIME    'r' => DATETIME_ARRAY
    //  'i' => DATE        's' => DATE_ARRAY
    //  'j' => TIME        't' => TIME_ARRAY
    //
    //  'u' => LIST        'v' => TABLE
    //
    //  'w' => BOOL        'A' => BOOL_ARRAY
    //  'x' => DATETIMETZ  'B' => DATETIMETZ_ARRAY
    //  'y' => DATETZ      'C' => DATETZ_ARRAY
    //  'z' => TIMETZ      'D' => TIMETZ_ARRAY
    //
    //  'E' => CHOICE      'F' => CHOICE_ARRAY
    //..
{
    switch (typeCode) {
      case 'a': return EType::BDEM_CHAR;
      case 'b': return EType::BDEM_SHORT;
      case 'c': return EType::BDEM_INT;
      case 'd': return EType::BDEM_INT64;
      case 'e': return EType::BDEM_FLOAT;
      case 'f': return EType::BDEM_DOUBLE;
      case 'g': return EType::BDEM_STRING;
      case 'h': return EType::BDEM_DATETIME;
      case 'i': return EType::BDEM_DATE;
      case 'j': return EType::BDEM_TIME;
      case 'k': return EType::BDEM_CHAR_ARRAY;
      case 'l': return EType::BDEM_SHORT_ARRAY;
      case 'm': return EType::BDEM_INT_ARRAY;
      case 'n': return EType::BDEM_INT64_ARRAY;
      case 'o': return EType::BDEM_FLOAT_ARRAY;
      case 'p': return EType::BDEM_DOUBLE_ARRAY;
      case 'q': return EType::BDEM_STRING_ARRAY;
      case 'r': return EType::BDEM_DATETIME_ARRAY;
      case 's': return EType::BDEM_DATE_ARRAY;
      case 't': return EType::BDEM_TIME_ARRAY;
      case 'u': return EType::BDEM_LIST;
      case 'v': return EType::BDEM_TABLE;
      case 'w': return EType::BDEM_BOOL;
      case 'x': return EType::BDEM_DATETIMETZ;
      case 'y': return EType::BDEM_DATETZ;
      case 'z': return EType::BDEM_TIMETZ;
      case 'A': return EType::BDEM_BOOL_ARRAY;
      case 'B': return EType::BDEM_DATETIMETZ_ARRAY;
      case 'C': return EType::BDEM_DATETZ_ARRAY;
      case 'D': return EType::BDEM_TIMETZ_ARRAY;
      case 'E': return EType::BDEM_CHOICE;
      case 'F': return EType::BDEM_CHOICE_ARRAY;

      default: {
        P(typeCode);  ASSERT("Invalid element type used in 'gType'" && 0);
      } break;
    }

    return EType::BDEM_INT;
}

void gFormattingMode(int *formattingMode, char code)
    // Bitwise-or into the specified 'bdeat' 'formattingMode' the value
    // corresponding to the specified 'code'.  Note that this function is used
    // by 'gBdemSchema'.
{
    switch (code) {
      case '0': {  // zero (e.g., "off" or no explicit mode specified)
        *formattingMode |= bdeat_FormattingMode::BDEAT_DEFAULT;
      } break;
      case 'A': {
        *formattingMode |= bdeat_FormattingMode::BDEAT_ATTRIBUTE;
      } break;
      case 'B': {
        *formattingMode |= bdeat_FormattingMode::BDEAT_BASE64;
      } break;
      case 'D': {
        *formattingMode |= bdeat_FormattingMode::BDEAT_DEC;
      } break;
      case 'L': {
        *formattingMode |= bdeat_FormattingMode::BDEAT_LIST;
      } break;
      case 'N':
      case 'n': {
        *formattingMode |= bdeat_FormattingMode::BDEAT_NILLABLE;
      } break;
      case 'S': {
        *formattingMode |= bdeat_FormattingMode::BDEAT_SIMPLE_CONTENT;
      } break;
      case 'T': {
        *formattingMode |= bdeat_FormattingMode::BDEAT_TEXT;
      } break;
      case 'U': {
        *formattingMode |= bdeat_FormattingMode::BDEAT_UNTAGGED;
      } break;
      case 'X': {
        *formattingMode |= bdeat_FormattingMode::BDEAT_HEX;
      } break;
      default: {
        ASSERT("Invalid code passed to 'gFormattingMode'" && 0);
      } break;
    }
}

bool gNullability(char nullCode, EType::Type type)
    // Return the nullability value corresponding to the specified 'nullCode'
    // and 'bdem' element 'type'.  Note that 'type' is ignored unless the
    // default nullability is specified.
    //..
    //  'D' => default for 'type'
    //  'F' => false
    //  'T' => true
    //..
{
    switch (nullCode) {
      case 'D': return EType::BDEM_LIST == type ? false : true;
      case 'F': return false;
      case 'T': return true;
      default: {
        P(nullCode);
        ASSERT("Invalid code passed to 'gNullability'" && 0);
      } break;
    }

    return true;
}

void parseFieldAttributes(int                          *defaultIndex,
                          int                          *formattingMode,
                          bool                         *isNullable,
                          EType::Type                   fieldType,
                          bsl::string::const_iterator&  iter)
    // Parse field attributes for the specified 'fieldType' from the specified
    // 'iter'.  Note that 'iter' is passed by reference and is advanced by this
    // function!
{
    // Reset attributes to their default values.

    *defaultIndex   = -1;
    *formattingMode = bdeat_FormattingMode::BDEAT_DEFAULT;
    *isNullable     = EType::BDEM_LIST == fieldType ? false : true;

    // Each field attribute specification is of the form '@av' where 'a'
    // denotes the attribute and 'v' encodes its value.

    while ('@' == *(iter + 1)) {
        ++iter;  // advance to '@'
        ++iter;  // advance to attribute code
        const char attributeCode = *iter;
        ++iter;  // advance to value
        const char attributeValue = *iter;
        switch (attributeCode) {
          case 'D': {
            ASSERT('0' <= attributeValue && attributeValue <= '1');
            *defaultIndex = attributeValue - '0';
          } break;
          case 'F': {
            gFormattingMode(formattingMode, attributeValue);
          } break;
          case 'N': {
            *isNullable = gNullability(attributeValue, fieldType);
          } break;
          default: {
            P(attributeCode);
            ASSERT("Invalid code passed to 'parseFieldAttributes'" && 0);
          } break;
        }
    }
}

void setDefaultValueForType(ERef ref, bool other = false)
    // Set the value of the element referenced by 'ref' to a non-null value
    // appropriate for its type.  If the optionally-specified 'other' flag is
    // 'true', set the value of the element referenced by 'ref' to an alternate
    // non-null value.  The behavior is undefined unless the type of the
    // element referenced by 'ref' is a scalar 'bdem' type.
{
    switch (ref.type()) {
      case EType::BDEM_CHAR: {
        ref.theModifiableChar()       = other ? B00 : A00;
      } break;
      case EType::BDEM_SHORT: {
        ref.theModifiableShort()      = other ? B01 : A01;
      } break;
      case EType::BDEM_INT: {
        ref.theModifiableInt()        = other ? B02 : A02;
      } break;
      case EType::BDEM_INT64: {
        ref.theModifiableInt64()      = other ? B03 : A03;
      } break;
      case EType::BDEM_FLOAT: {
        ref.theModifiableFloat()      = other ? B04 : A04;
      } break;
      case EType::BDEM_DOUBLE: {
        ref.theModifiableDouble()     = other ? B05 : A05;
      } break;
      case EType::BDEM_STRING: {
        ref.theModifiableString()     = other ? B06 : A06;
      } break;
      case EType::BDEM_DATETIME: {
        ref.theModifiableDatetime()   = other ? B07 : A07;
      } break;
      case EType::BDEM_DATE: {
        ref.theModifiableDate()       = other ? B08 : A08;
      } break;
      case EType::BDEM_TIME: {
        ref.theModifiableTime()       = other ? B09 : A09;
      } break;
      case EType::BDEM_BOOL: {
        ref.theModifiableBool()       = other ? B22 : A22;
      } break;
      case EType::BDEM_DATETIMETZ: {
        ref.theModifiableDatetimeTz() = other ? B23 : A23;
      } break;
      case EType::BDEM_DATETZ: {
        ref.theModifiableDateTz()     = other ? B24 : A24;
      } break;
      case EType::BDEM_TIMETZ: {
        ref.theModifiableTimeTz()     = other ? B25 : A25;
      } break;
      default: {
        ASSERT("Invalid element passed to 'setDefaultValueForType'" && 0);
      } break;
    }
}

const char *gName(char abbreviation, bool prefix = false)
    // Look up a name in 'NAMES' array based on its one-character
    // 'abbreviation'.  If 'prefix' is 'true', prefix the name with "bb:".
{
    int offset = (prefix ? 0 : 3);  // If 'prefix' is false, skip "bb:"
    int nameIndex = abbreviation - 'A';
    ASSERT(NUM_NAMES > nameIndex);
    if (NAMES[nameIndex][2] != ':') {
        offset = 0;
    }
    return NAMES[nameIndex] + offset;
}

const int MAX_POSITIONAL_PARAMETERS = 9;

const char *
lookupParameters(const char *positionalParams[MAX_POSITIONAL_PARAMETERS],
                 const char *start,
                 const char *finish)
    // Given the substring '[start, finish)', parse it as an element lookup
    // and return the element pattern from the 'ELEMENTS' array.  Given an
    // example input substring of "13A:Bc@2", the parts are interpreted as
    // follows: The initial integer (13 in our example), is used as the index
    // in the 'ELEMENTS' array corresponding to the desired element pattern to
    // be returned.  The portion of the substitution string after the initial
    // integer encodes the string fragments to be returned for slot in the
    // 'positionalParams' array.  An upper-case letter ("A", in our example),
    // indexes an element in the 'NAMES' array and strips off the leading
    // "bb:", if any.  A colon followed by an upper-case letter (":B") also
    // indexes the 'NAMES' array but includes the leading "bb:", if present in
    // the names array.  (NOTE: current implementation does not generate "bb:"
    // prefix.  Test logic remains in case that changes.)  A lower-case letter
    // ("c") indexes the 'XMLTYPES' array, and an at-symbol followed by a
    // decimal integer ("@2") indexes the 'ATTRIBUTES' array.  If there are
    // fewer than 'MAX_POSITIONAL_PARAMETERS' specified, then the rest of
    // 'positionalParams' is filled with null pointers.
{
    int numParams = 0;

    // Convert digits at start to an integer index within 'ELEMENTS' array.
    char *endDigits = 0;
    int elementIndex = bsl::strtol(start, &endDigits, 10);
    start = endDigits;

    // Look up element string in 'ELEMENTS' array.
    ASSERT(elementIndex < NUM_ELEMENTS);
    const char *element = ELEMENTS[elementIndex];

    // Loop through rest of substring, looking up strings for positional
    // parameters.
    while (start != finish) {
        ASSERT(MAX_POSITIONAL_PARAMETERS > numParams);

        if ('@' ==  *start) {
            // Parameter is an 'ATTRIBUTES' index in decimal format.
            ++start;    // skip '@'
            ASSERT(bsl::isdigit(*start));
            int attrIndex = bsl::strtol(start, &endDigits, 10);
            start = endDigits;

            ASSERT(NUM_ATTRIBUTES > attrIndex);
            positionalParams[numParams++] = ATTRIBUTES[attrIndex];
        }
        else if (':' == *start) {
            // Parameter is a name code, including "bb:" prefix, if any.
            ++start;    // skip ':'
            ASSERT(bsl::isupper(*start));
            positionalParams[numParams++] = gName(*start++, true);
        }
        else if (bsl::isupper(*start)) {
            // Parameter is a name code, excluding "bb:" prefix, if any.
            positionalParams[numParams++] = gName(*start++, false);
        }
        else {
            // Parameter is an XML type code
            ASSERT(bsl::islower(*start));
            int xmlTypeIndex = *start++ - 'a';

            ASSERT(NUM_XMLTYPES > xmlTypeIndex);
            positionalParams[numParams++] = XMLTYPES[xmlTypeIndex];
        }
    }

    // Set unspecified positional parameters to null.
    for ( ; numParams < MAX_POSITIONAL_PARAMETERS; ++numParams) {
        positionalParams[numParams] = 0;
    }

    return element;
}

int expandXmlEncoding(bsl::string  *result,
                      const char   *spec,
                      const char  **positionalParams,
                      int           indentLevel)
    // Expand an XML Schema string, compactly encoded in 'spec', store the
    // result in the string pointed to by 'result' and return the line number
    // in 'result' corresponding to the caret (^) character, if any in
    // 'spec'.  The 'spec' string is copied verbatim into '*result' except for
    // the following special sequences:
    //
    //   [xxx]          is replaced by an element-tag lookup as described in
    //                  the 'lookupParameters' function, above.
    //   [xxx]{...}     is handled the same as [xxx] except that a closing tag
    //                  is automatically generated for the closing curly
    //                  brace (}).
    //   %n             where 'n' is a digit from 1 to 9, is replaced by the
    //                  corresponding string in 'positionalParams[n - 1]'
    //   ^              is not copied to '*result' but its logical position
    //                  in '*result' is remembered and converted to the line
    //                  returned from this function.
{
    static const char spaces[] =
        "                                                            ";

    int caret = -1;
    bool isStartOfLine = true;

    typedef bsl::string::size_type size_type;
    typedef bsl::pair<size_type, size_type> subrange;

    bsl::stack<subrange> tagStack;

    for (const char *p = spec; *p; ) {

        bsl::size_t span = bsl::strcspn(p, "[%}^");
        if (0 < span) {
            // Copy verbatim characters.
            if (isStartOfLine) {
                result->append(spaces, 4 * indentLevel);
            }
            result->append(p, span);
            isStartOfLine = false;
        }

        if ('\0' == p[span]) {
            // Found no further substitutions
            return caret;
        }

        p += span;
        switch (*p) {
          case '[': {
            // Nested substitution

            // Remember start of substitution within output string.
            bsl::string::size_type savePos = result->length();

            // Find closing bracket
            const char *startBracket = p;
            p = bsl::strchr(startBracket, ']');
            ASSERT(p);

            // Lookup element string and positional parameters
            const char *positionalParams[MAX_POSITIONAL_PARAMETERS];
            const char *elementString = lookupParameters(positionalParams,
                                                         startBracket + 1, p);
            ++p;

            // Recursively expand the element string
            int newCaret = expandXmlEncoding(result, elementString,
                                             positionalParams, indentLevel);

            if (0 > caret) {
                caret = newCaret;
            }

            // Advance saved position past any new space characters.
            savePos = result->find_first_not_of(" ", savePos);
            if (result->length() < savePos) {
                savePos = result->length();
            }

            // At the end of an element tag, always append newline.
            result->append("\n");
            isStartOfLine = true;

            if ('{' == *p) {
                // Beginning of an open element.

                // Save the position and length of the first tag in the
                // expanded string.
                ASSERT('<' == (*result)[savePos]);

                ++savePos;  // Advance past the '<' character
                size_type saveLen = result->find_first_of(" >",
                                                          savePos) - savePos;
                ASSERT(savePos + saveLen < result->length());
                tagStack.push(subrange(savePos, saveLen));
                ++indentLevel;

                ++p;
            }
          } break;

          case '%': {
            // Parameter substitution
            ASSERT(bsl::isdigit(p[1]));
            int positionalIndex = p[1] - '1';
            ASSERT(0 <= positionalIndex);

            if (positionalParams && positionalParams[positionalIndex]) {
                result->append(positionalParams[positionalIndex]);
                isStartOfLine = false;
            }

            p += 2;
          } break;

          case '}': {
            // End of open element
            ASSERT(! tagStack.empty());

            --indentLevel;
            if (isStartOfLine) {
                result->append(spaces, 4 * indentLevel);
                isStartOfLine = false;
            }
            result->append("</");
            // Append a substring of result onto itself.  The substring
            // contains the name of the tag we are matching.
            result->append(*result,
                           tagStack.top().first, tagStack.top().second);
            result->append(">\n");
            isStartOfLine = true;
            tagStack.pop();

            ++p;
          } break;

          case '^': {
            // Remember our position in the generation of the string.
            if (caret < 0) {
                caret = result->length();
                if (0 < caret && '\n' == (*result)[caret - 1]) {
                    --caret;
                }
            }
            ++p;
          } break;
        }
    }

    return caret;
}

// Generate an XML string from a compact specification.
//
// The XML generator takes a compact input string and decodes it into an XML
// string.  The specification string consists of normal text with embedded
// codes, which are expanded into canned sequences of XML elements.  The
// process is recursive: the result of expanding a code is scanned again for
// additional codes.
//
// There are two kinds of codes used within an input string: element
// references and parameter references.
//
// An element reference consists of a square bracket ([) followed by a decimal
// number, followed by zero or more single-letter parameters, followed by a
// close bracket (]), with no intervening spaces.  The decimal number indexes
// an element in the 'ELEMENTS' array, which replaces the element reference
// during expansion.  Each letter after the decimal number refers to a
// positional parameter within the element.  A lower-case letter refers to an
// XML type in the 'XMLTYPES' array.  An upper-case letter refers to a name in
// the 'NAMES' array.  Each positional parameter is used to expand a parameter
// reference within the element string.
//
// A positional parameter consists of a percent character (%) followed by a
// digit from 1 to 9. When an element reference is expanded, the positional
// references within the expanded element correspond to the positional
// parameters within the element reference.  The %1 code is replaced by the
// first positional parameter, %2 code by the second positional parameter,
// etc.
//
// Each element within the 'ELEMENTS' array must contain either an single,
// unclosed XML tag, or a complete XML element (including closing tag),
// possibly with nested elements.  An element reference for an unclosed tag
// must be followed by an open curly brace ({).  The closing curly brace (})
// is replaced by the corresponding closing tag.
//
// A caret does not generate any XML.  However, the position of the caret in
// the generated XML is remembered and return from 'gXML'.  This is useful for
// testing the error handling logic, where a specific line number is expected
// in the error state.
//
// Synopsis:
//
// [#A:Bc]     Replace with element number # using positional parameters
//             name A, name :B, and XML type c.
// %#          Replace with positional parameter # (single-digit #)
// [#Ab]{...}  Replace with unclosed element #.  Insert closing tag at '}'.
// ^           Remember line number at this position
//
// Examples (refer to ELEMENTS, XMLTYPES, and NAMES arrays, above):
//
// Input spec           Output xmlStr
// ----------           -------------
// [7:A]                <xs:element ref="Apple"/>
//
// [2]{[9]{[6Bc]}}      <xs:complexType>
//                          <xs:sequence>
//                              <xs:element name="Banana" type="date"/>
//                          <xs:/sequence>
//                      <xs:/complexType>

int gXML(bsl::string *xmlStr, const bsl::string& spec)
{
    xmlStr->clear();

    // process the string as if it were an element substitution with no
    // parameters.
    int caret = expandXmlEncoding(xmlStr, spec.c_str(), 0, 0);

    if (caret < 0) {
        return -1;
    }
    else {
        return bsl::count(xmlStr->begin(), xmlStr->begin() + caret, '\n') + 1;
    }
}

// Generate a 'bdem_Schema' from a compact specification string.
//
// Each record in the schema is specified using an upper-case letter, which
// corresponds to a name in the 'NAMES' array.  An '*' is used to specify an
// unnamed record.  The record name code is followed by an open curly brace
// ({) followed by zero or more sets of two or three-letter field
// definitions.  If question mark ('?') appears between the name and the open
// curly-brace, then the record is a choice record; otherwise it is a sequence
// record.
//
// Each field definition consists of a capital letter, again referring to the
// 'NAMES' array, with '*' used for nameless fields.  The field name code is
// followed by a lower-case or upper-case letter that specifies one of the 32
// types as per the 'gType' function.  If the type code is 'u' (LIST), 'v'
// (TABLE), 'E' (CHOICE), or 'F' (CHOICE_ARRAY), then it must be followed by
// another upper-case letter specifying the name of the constraint record, or a
// digit (0-9) specifying the index of an unnamed record.
//
// The field name and constraint may be optionally followed by zero or more
// additional attributes:
//..
//  Default value:
//    @D0 : default value 0 corresponding to the field value
//    @D1 : default value 1 corresponding to the field value
//
//  Formatting mode:
//    @F0 : Set DEFAULT formatting mode
//    @FA : Set IS_ATTRIBUTE formatting mode
//    @FB : Set BASE64 formatting mode
//    @FD : Set DEC formatting mode
//    @FL : Set LIST formatting mode
//    @FN : Set IS_NILLABLE formatting mode
//    @FT : Set TEXT formatting mode
//    @FU : Set IS_UNTAGGED formatting mode
//    @FX : Set HEX formatting mode
//
//  Nullability:
//    @NF : Set nullability to FALSE
//    @NT : Set nullability to TRUE
//    @ND : Set nullability to its default value
//
//  Field Id:
//    @I0 ... @I9 : Set field ID to value 0 ... 9
//..
// The following examples refer to the 'ELEMENTS' and 'NAMES' arrays, and the
// 'gType' function defined above:
//
// Input spec           Output bdem_Schema
// ----------           ------------------
// A{BaDl}S{WvAKb}      {
//                          RECORD "Apple" {
//                              CHAR "Banana";
//                              SHORT_ARRAY "Daddy";
//                          }
//
//                          RECORD "Snow" {
//                              TABLE<"Apple"> "Walrus";
//                              SHORT "Kangaroo";
//                          }
//                      }
void gBdemSchema(bdem_Schema *schema, const bsl::string& spec)
{
    schema->removeAll();

    enum Expectation { RECNAME, OPENCURLY, FIELDNAME, FIELDTYPE, CONSTRAINT };

    Expectation expected = RECNAME;

    RecDef *currRecord = 0;
    RecType recordType;
    const char *fieldName = 0;
    EType::Type fieldType;
    int defaultIndex;
    int formattingMode;
    bool isNullable;

    for (bsl::string::const_iterator i = spec.begin(); i != spec.end(); ++i) {
        switch (expected) {
          case RECNAME: {
            ASSERT(bsl::isupper(*i) || '*' == *i);
            const char *recordName = '*' == *i ? 0 : gName(*i);
            if (*(i + 1) == '?') {
                recordType = RecDef::BDEM_CHOICE_RECORD;
                ++i;
            }
            else {
                recordType = RecDef::BDEM_SEQUENCE_RECORD;
            }
            currRecord = schema->createRecord(recordName, recordType);
            expected = OPENCURLY;
          } break;
          case OPENCURLY: {
            ASSERT('{' == *i);
            expected = FIELDNAME;
          } break;
          case FIELDNAME: {
            if ('}' == *i) {
                expected = RECNAME;
                break;
            }
            ASSERT(bsl::isupper(*i) || '*' == *i);
            fieldName = '*' == *i ? 0 : gName(*i);
            expected = FIELDTYPE;
          } break;
          case FIELDTYPE: {
            fieldType = gType(*i);
            if (EType::isAggregateType(fieldType)) {
                expected = CONSTRAINT;
            }
            else {
                ASSERT(currRecord);

                parseFieldAttributes(&defaultIndex,
                                     &formattingMode,
                                     &isNullable,
                                     fieldType,
                                     i);

                FldAttr fieldAttr(fieldType, isNullable, formattingMode);
                if (-1 != defaultIndex
                 && EType::isScalarType(fieldType)) {
                    setDefaultValueForType(fieldAttr.defaultValue(),
                                           defaultIndex);
                }

                currRecord->appendField(fieldAttr, fieldName);
                expected = FIELDNAME;
            }
          } break;
          case CONSTRAINT: {
            ASSERT(bsl::isupper(*i) || bsl::isdigit(*i));
            const RecDef *constraint = 0;
            if (bsl::isupper(*i)) {
                constraint = schema->lookupRecord(gName(*i));
            }
            else {
                constraint = &schema->record(*i - '0');
            }

            ASSERT(constraint);
            ASSERT(EType::isAggregateType(fieldType));
            ASSERT(currRecord);

            parseFieldAttributes(&defaultIndex,
                                 &formattingMode,
                                 &isNullable,
                                 fieldType,
                                 i);

            FldAttr fieldAttr(fieldType,
                              isNullable,
                              formattingMode);

            currRecord->appendField(fieldAttr, constraint, fieldName);

            expected = FIELDNAME;
          } break;
          default: {
            ASSERT(0);
          } break;
        }
    }
}

} // close anonymous namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    struct TestVector {
        int         d_line;
        const char *d_bdemScript;
        const char *d_xsdScript;
    };

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
        // --------------------------------------------------------------------
        // TESTING ANONYMOUS CHOICE AND SEQUENCE
        //
        // Concerns:
        //  - A LIST, CHOICE, TABLE, or CHOICE_ARRAY field with formatting
        //    mode 'IS_UNTAGGED' will generate a nested, anonymous <choice> or
        //    anonymous <sequence> element.
        //  - The 'IS_UNTAGGED' formatting mode is ignored for non-aggregate
        //    fields.
        //  - The field name is ignored for untagged aggregate fields.
        //  - The name of the constraint record is ignored for untagged
        //    aggregate fields, but the constraint record is still generated
        //    at the global level.
        //  - Anonymous elements can be nested.
        //  - Special-case, if an anonymous choice or sequence is the only
        //    field within a named sequence, then the extra level of nesting
        //    is removed.
        //
        // Plan:
        //  - For each of the above concerns, create a bdem_Schema.
        //  - Generate the XSD for each schema and compare with the expected
        //    results.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "TEST ANONYMOUS CHOICE AND SEQUENCE"
                               << bsl::endl;

        const TestVector DATA[] = {
            // line bdem script, (first line)
            //      xml script   (2nd+ line(s))
            // ---- ------------

            // Test LIST, CHOICE, TABLE, and CHOICE_ARRAY.
            { L_, "*{Aa@NFBh@NF}C{Dc@NF*u0@FU@NFEg@NF}",
                  "[0][1]{[2][19]"
                  "[4C]{[10]{[7Dc][10]{[7Aa][7Bh]}[7Eg]}}[19]"
                  "[7CC][19]}" },
            { L_, "*?{Aa@NFBh@NF}C{Dc@NF*E0@FU@NFEg@NF}",
                  "[0][1]{[2][19]"
                  "[4C]{[10]{[7Dc][18]{[7Aa][7Bh]}[7Eg]}}[19]"
                  "[7CC][19]}" },
            { L_, "*{Aa@NFBh@NF}C{Dc@NF*v0@FUEg@NF}",
                  "[0][1]{[2][19]"
                  "[4C]{[10]{[7Dc][10@1@2]{[7Aa][7Bh]}[7Eg]}}[19]"
                  "[7CC][19]}" },
            { L_, "*?{Aa@NFBh@NF}C{Dc@NF*F0@FUEg@NF}",
                  "[0][1]{[2][19]"
                  "[4C]{[10]{[7Dc][18@1@2]{[7Aa][7Bh]}[7Eg]}}[19]"
                  "[7CC][19]}" },

            // Test that IS_UNTAGGED is ignored for non-aggregate fields.
            { L_, "C{Dc@NF*d@FU@NFEg@NF}",
                  "[0][1]{[2][19]"
                  "[4C]{[10]{[7Dc][7Gd][7Eg]}}[19]"
                  "[7CC][19]}" },
            { L_, "C{Dc@NFAd@FU@NFEg@NF}",
                  "[0][1]{[2][19]"
                  "[4C]{[10]{[7Dc][7Ad][7Eg]}}[19]"
                  "[7CC][19]}" },

            // Test that field name is ignored
            { L_, "*?{Aa@NFBh@NF}C{Dc@NFCE0@FU@NFEg@NF}",
                  "[0][1]{[2][19]"
                  "[4C]{[10]{[7Dc][18]{[7Aa][7Bh]}[7Eg]}}[19]"
                  "[7CC][19]}" },

            // Test that constraint record name is ignored in nested type.
            // (However, the named record is added separately to the output.)
            { L_, "A?{Aa@NFBh@NF}C{Dc@NF*E0@FU@NFEg@NF}",
                  "[0][1]{[2][19]"
                  "[4A]{[18]{[7Aa][7Bh]}}[19]"
                  "[4C]{[10]{[7Dc][18]{[7Aa][7Bh]}[7Eg]}}[19]"
                  "[7AA][19]"
                  "[7CC][19]}" },
            { L_, "A?{Aa@NFBh@NF}C{Dc@NFCE0@FU@NFEg@NF}",
                  "[0][1]{[2][19]"
                  "[4A]{[18]{[7Aa][7Bh]}}[19]"
                  "[4C]{[10]{[7Dc][18]{[7Aa][7Bh]}[7Eg]}}[19]"
                  "[7AA][19]"
                  "[7CC][19]}" },

            // Anonymous sequence nested within anonymous choice element
            { L_, "*{Aa@NFBh@NF}*?{Cu0@FU@NFDc@NF}C{*E1@FU@NFEg@NF}",
                  "[0][1]{[2][19]"
                  "[4C]{[10]{[18]{[10]{[7Aa][7Bh]}[7Dc]}[7Eg]}}[19]"
                  "[7CC][19]}" },

            // Anonymous choice nested within anonymous sequence element
            { L_, "*?{Aa@NFBh@NF}*{Dc@NFCE0@FU@NF}C{Eg@NF*u1@FU@NF}",
                  "[0][1]{[2][19]"
                  "[4C]{[10]{[7Eg][10]{[7Dc][18]{[7Aa][7Bh]}}}}[19]"
                  "[7CC][19]}" },

            // Anonymous choice array nested within anonymous sequence element
            { L_, "*?{Aa@NFBh@NF}*{*F0@FU}C{Eg@NF*u1@FU@NF}",
                  "[0][1]{[2][19]"
                  "[4C]{[10]{[7Eg][10]{[18@1@2]{[7Aa][7Bh]}}}}[19]"
                  "[7CC][19]}" },

            // Special case:
            // Single, anonymous field nested within named sequence element.
            { L_, "*{Aa@NFBh@NF}A{*u0@NT@FU}C{Eg@NFAuA@NF}",
                  "[0][1]{[2][19]"
                  "[4A]{[10@1]{[7Aa][7Bh]}}[19]"
                  "[4C]{[10]{[7Eg][7AA]}}[19]"
                  "[7AA][19]"
                  "[7CC][19]}" },
            { L_, "*?{Aa@NFBh@NF}A{*E0@FU}C{Eg@NFAuA@NF}",
                  "[0][1]{[2][19]"
                  "[4A]{[18@1]{[7Aa][7Bh]}}[19]"
                  "[4C]{[10]{[7Eg][7AA]}}[19]"
                  "[7AA][19]"
                  "[7CC][19]}" },
            { L_, "*{Aa@NFBh@NF}A{*v0@FU}C{Eg@NFAuA@NF}",
                  "[0][1]{[2][19]"
                  "[4A]{[10@1@2]{[7Aa][7Bh]}}[19]"
                  "[4C]{[10]{[7Eg][7AA]}}[19]"
                  "[7AA][19]"
                  "[7CC][19]}" },
            { L_, "*?{Aa@NFBh@NF}A{*F0@FU}C{Eg@NFAuA@NF}",
                  "[0][1]{[2][19]"
                  "[4A]{[18@1@2]{[7Aa][7Bh]}}[19]"
                  "[4C]{[10]{[7Eg][7AA]}}[19]"
                  "[7AA][19]"
                  "[7CC][19]}" },

            // Normal case: anonymous field, but not only field
            { L_, "*{Aa@NFBh@NF}A{*u0@NT@FUCc@NF}",
                  "[0][1]{[2][19]"
                  "[4A]{[10]{[10@1]{[7Aa][7Bh]}[7Cc]}}[19]"
                  "[7AA][19]}" },
            { L_, "*?{Aa@NFBh@NF}A{*E0@FUCc@NF}",
                  "[0][1]{[2][19]"
                  "[4A]{[10]{[18@1]{[7Aa][7Bh]}[7Cc]}}[19]"
                  "[7AA][19]}" },

            // Normal case: unnamed field, but not untagged (anonymous)
            { L_, "*{Aa@NFBh@NF}A{*u0@NT}",
                  "[0][1]{[2][19]"
                  "[4R]{[10]{[7Aa][7Bh]}}[19]"
                  "[4A]{[10]{[7FR@1]}}[19]"
                  "[7RR][19]"
                  "[7AA][19]}" },
            { L_, "*?{Aa@NFBh@NF}A{*E0}",
                  "[0][1]{[2][19]"
                  "[4R]{[18]{[7Aa][7Bh]}}[19]"
                  "[4A]{[10]{[7FR@1]}}[19]"
                  "[7RR][19]"
                  "[7AA][19]}" },
        };

        const int DATA_SIZE = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < DATA_SIZE; ++i) {
            const int LINE                = DATA[i].d_line;
            const char *const BDEM_SCRIPT = DATA[i].d_bdemScript;
            const char *const XSD_SCRIPT  = DATA[i].d_xsdScript;

            if (verbose) {
                bsl::cout << "Test vector at line " << LINE << bsl::endl;
            }

            bdem_Schema bdemSchema;
            gBdemSchema(&bdemSchema, BDEM_SCRIPT);

            bsl::string expectedXsd;
            gXML(&expectedXsd, XSD_SCRIPT);

            if (veryVerbose) {
                P(bdemSchema);
                P(expectedXsd);
            }

            Obj generator;
            bsl::ostringstream schemaStream;
            int status = generator.generateXsd(schemaStream,
                                               bdemSchema,
                                               TESTNAMESPACE,
                                               0, 4, INT_MAX);
            LOOP2_ASSERT(LINE, bdemSchema, 0 == status);

            if (veryVerbose) { P(schemaStream.str()); }

            LOOP4_ASSERT(LINE, bdemSchema, expectedXsd, schemaStream.str(),
                         schemaStream.str() == expectedXsd);
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING Unconstrained LIST and TABLE fields in bdem_Schema result
        // in failure.  Minimal test for now.
        //
        // Plans:
        //   Constructed a bdem_Schema with one record that has an
        //   unconstrained LIST field and another bdem_Schema with one record
        //   that has an unconstrained TABLE field.  The generateXsd() should
        //   fail.
        // --------------------------------------------------------------------

        if (verbose)
              bsl::cout << "TEST input with unconstrained LIST&TABLE fields"
                        << bsl::endl;

        {
            bdem_Schema badSchema;
            RecDef *record0 = badSchema.createRecord("zero");
            record0->appendField(EType::BDEM_LIST, "unconstrainedListField");

            Obj generator;
            bsl::ostringstream oss;
            ASSERT(0 != generator.generateXsd(oss, badSchema, TESTNAMESPACE));
        }
        {
            bdem_Schema badSchema;
            RecDef *record0 = badSchema.createRecord("zero");
            record0->appendField(EType::BDEM_TABLE, "unconstrainedTableField");

            Obj generator;
            bsl::ostringstream oss;
            ASSERT(0 != generator.generateXsd(oss, badSchema, TESTNAMESPACE));
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING combination for element generation of anonymous fields,
        //   type generation of anonymous records, and LIST and TABLE fields
        //   that are constrained to itself and/or other records.
        //
        // Concerns:
        //   Combination of case 7, 6, 5, 4, 3, 2 works
        //
        // Plan:
        //   Construct a bdem_Schema with records both named and anonymous with
        //   fields both named and anonymous.  The records self and cross
        //   reference each other.  Check for generated naming for local
        //   elements, naming for global elements, naming for complexTypes,
        //   naming for constrained fields and type of such fields.
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << "TEST combination field and record generation"
                      << bsl::endl;

        char bdemScript[] =
            "*{*a@D0}"
            "A{*b@D0Ac@D0*u0}"
            "*{*v1*d@D0*e@D0*f@D0*u2}"
            "B{*g@D0*u2*h@D0*i@D0Bj@D0*vB}"
            "*{*k*l@FL*mCn@NFDv3*o}"
            "C{*p*q@FL@NFEu4*v4*r*s*t@FL}";
        char xsdScript [] =
            "[0][1]{"
            "[2][19]"
            "[4R]{[10]{[7Fa@4@1]}}[19]"
            "[4A]{[10]{[7Fb@5@1][7Ac@6@1][7H:R]}}[19]"
            "[4T]{[10]{[7F:A@1@2][7Gd@7@1][7He@8@1][7If@9@1][7J:T@1]}}[19]"
            "[4B]{[10]{[7Fg@10@1][7G:T][7Hh@11@1][7Ii@12@1][7Bj@13@1]"
                      "[7K:B@1@2]}}[19]"
            "[4V]{[10]{[7Fa@1@2][7Gl@1][7Hc@1@2][7Cd@1@2][7D:B@1@2]"
                      "[7Ke@1@2]}}[19]"
            "[4C]{[10]{[7Ff@1@2][7Gq][7E:V][7I:V@1@2][7Jh@1@2][7Ki@1@2]"
                      "[7Lt@1]}}[19]"
            "[7R:R][19][7A:A][19][7T:T][19][7B:B][19][7V:V][19][7C:C][19]"
            "}";

        bdem_Schema bdemSchema;
        gBdemSchema(&bdemSchema, bdemScript);

        Obj generator;
        bsl::ostringstream schemaStream;
        int status = generator.generateXsd(schemaStream,
                                           bdemSchema,
                                           TESTNAMESPACE,
                                           0, 4, INT_MAX);
        ASSERT(0 == status);

        bsl::string expectedXsd;
        gXML(&expectedXsd, xsdScript);

        if (veryVerbose) {
            P(bdemSchema);
            Q(schemaStream.str());
            bsl::cout << schemaStream.str();
            Q(schemaStream.str());
            Q(expectedXsd);
            bsl::cout << expectedXsd;
            Q(expectedXsd);
        }

        LOOP2_ASSERT(expectedXsd, schemaStream.str(),
                     schemaStream.str() == expectedXsd);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING self-reference constrained list and table fields
        //
        // Concerns:
        //   LIST and TABLE fields that are constrained onto the their
        //   enclosing record are formatted correctly.  The name and the type.
        //
        // Plans:
        //   Construct a bdem_Schema that has one record, in which a LIST and
        //   a TABLE fields reference back to the record.  Check for the
        //   element names and element types.
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << "TEST self-reference constrained fields" << bsl::endl;

        const char bdemScript[] = "A{BuACvA}";
        const char xsdScript[] =
            "[0][1]{"       // header <xs:schema>
            "[2][19]"       // boilerplate
            "[4A]{[10]{"    // <xs:complexType name="A"><xs:sequence>
            "[7B:A@1]"      // <xs:element name="B" type="bb:A" minOccurs="0"/>
            "[7C:A@1@2]"    // <xs:element name="C" type="bb:A" minOccurs="0"
                            // maxOccurs="unbounded"/>
            "}}[19]"        // </xs:sequence></xs:complexType>\n
            "[7A:A][19]}";  // <xs:element name="A" type="bb:A"/>

        bdem_Schema bdemSchema;
        gBdemSchema(&bdemSchema, bdemScript);

        Obj generator;
        bsl::ostringstream schemaStream;
        int status = generator.generateXsd(schemaStream,
                                           bdemSchema,
                                           TESTNAMESPACE,
                                           0, 4, INT_MAX);
        ASSERT(0 == status);

        bsl::string expectedXsd;
        gXML(&expectedXsd, xsdScript);

        if (veryVerbose) {
            P(bdemSchema);
            Q(schemaStream.str());
            bsl::cout << schemaStream.str();
            Q(schemaStream.str());
            Q(expectedXsd);
            bsl::cout << expectedXsd;
            Q(expectedXsd);
        }

        ASSERT(schemaStream.str() == expectedXsd);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING constrained tables and choice arrays
        //
        // Concerns:
        //   Naming of constrained tables and choice arrays is correct both as
        //   element name and element type in XSD schema.
        //
        // Plan:
        //   Construct a bdem_Schema where each record except the first one
        //   has a TABLE (or CHOICE_ARRAY) field constrained to the previous
        //   record.  Check for the element names and types.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "TEST constrained tables." << bsl::endl;
        {
            bsl::string bdemScript = "*{*a@D0}";  // record 0 is unnamed
            bsl::string xsdScript =
                "[0][1]{"       // header <xs:schema>
                "[2][19]"       // boilerplate
                "[4R]{[10]{[7Fa@4@1]}}[19]";  // record 0 has one simple field

            char previousRecordNameChar = 'R';
            for (int record = 0; record < MAXNUM_NAMES; ++record) {
                bdemScript += 'A' + record;  // SEQUENCE record name
                bdemScript += '{';
                bdemScript += 'A' + record;  // constrained TABLE field name
                bdemScript += 'v';           // constrained TABLE
                bdemScript += '0' + record;  // constrained TABLE field type
                bdemScript += '}';

                xsdScript += "[4";
                xsdScript += 'A' + record; // <xs:complexType name="A">
                xsdScript += "]{[10]{[7";  // <xs:sequence><xs:element
                xsdScript += 'A' + record; // name="A"
                xsdScript += ':';
                xsdScript += previousRecordNameChar; // type="bb:prev-Record"
                xsdScript += "@1@2]";      // minOccurs="0"
                                           // maxOccurs="unbounded"/>
                xsdScript += "}}[19]";     // </xs:sequence></xs:complexType>\n

                previousRecordNameChar = 'A' + record;
            }
            xsdScript += "[7R:R][19]";     // <xs:element name="R" type="R"/>\n
            for (int record = 0; record < MAXNUM_NAMES; ++record) {
                xsdScript += "[7";         // <xs:element
                xsdScript += 'A' + record; //     name="A"
                xsdScript += ':';
                xsdScript += 'A' + record; //     type="bb:A"
                xsdScript += "][19]";      // />\n
            }
            xsdScript += "}";

            bdem_Schema bdemSchema;
            gBdemSchema(&bdemSchema, bdemScript);

            Obj generator;
            bsl::ostringstream schemaStream;
            int status = generator.generateXsd(schemaStream,
                                               bdemSchema,
                                               TESTNAMESPACE,
                                               0, 4, INT_MAX);
            ASSERT(0 == status);

            bsl::string expectedXsd;
            gXML(&expectedXsd, xsdScript);

            if (veryVerbose) {
                P(bdemSchema);
                Q(schemaStream.str());
                bsl::cout << schemaStream.str();
                Q(schemaStream.str());
                Q(expectedXsd);
                bsl::cout << expectedXsd;
                Q(expectedXsd);
            }

            ASSERT(schemaStream.str() == expectedXsd);
        }

        if (verbose)
            bsl::cout << "TEST constrained choice arrays." << bsl::endl;
        {
            bsl::string bdemScript = "*?{*a@D0}";  // record 0 is unnamed
            bsl::string xsdScript =
                "[0][1]{"       // header <xs:schema>
                "[2][19]"       // boilerplate
                "[4R]{[18]{[7Fa@4@1]}}[19]";  // record 0 has one simple field

            char previousRecordNameChar = 'R';
            for (int record = 0; record < MAXNUM_NAMES; ++record) {
                bdemScript += 'A' + record;  // record name
                bdemScript += '?';           // CHOICE record
                bdemScript += '{';
                bdemScript += 'A' + record;  // constrained CHOICE_ARRAY name
                bdemScript += 'F';           // constrained CHOICE_ARRAY
                bdemScript += '0' + record;  // constrained CHOICE_ARRAY type
                bdemScript += '}';

                xsdScript += "[4";
                xsdScript += 'A' + record; // <xs:complexType name="A">
                xsdScript += "]{[18]{[7";  // <xs:choice><xs:element
                xsdScript += 'A' + record; // name="A"
                xsdScript += ':';
                xsdScript += previousRecordNameChar; // type="bb:prev-Record"
                xsdScript += "@1@2]";      // minOccurs="0"
                                           // maxOccurs="unbounded"/>
                xsdScript += "}}[19]";     // </xs:choice></xs:complexType>\n

                previousRecordNameChar = 'A' + record;
            }
            xsdScript += "[7R:R][19]";     // <xs:element name="R" type="R"/>\n
            for (int record = 0; record < MAXNUM_NAMES; ++record) {
                xsdScript += "[7";         // <xs:element
                xsdScript += 'A' + record; // name="A"
                xsdScript += ':';
                xsdScript += 'A' + record; // type="bb:A"
                xsdScript += "][19]";      // />\n
            }
            xsdScript += "}";

            bdem_Schema bdemSchema;
            gBdemSchema(&bdemSchema, bdemScript);

            Obj generator;
            bsl::ostringstream schemaStream;
            int status = generator.generateXsd(schemaStream,
                                               bdemSchema,
                                               TESTNAMESPACE,
                                               0, 4, INT_MAX);
            ASSERT(0 == status);

            bsl::string expectedXsd;
            gXML(&expectedXsd, xsdScript);

            if (veryVerbose) {
                P(bdemSchema);
                Q(schemaStream.str());
                bsl::cout << schemaStream.str();
                Q(schemaStream.str());
                Q(expectedXsd);
                bsl::cout << expectedXsd;
                Q(expectedXsd);
            }

            ASSERT(schemaStream.str() == expectedXsd);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING constrained lists and choices
        //
        // Concerns:
        //   Naming of constrained lists and choices is correct both as element
        //   name and element type in XSD schema.
        //
        // Plan:
        //   Construct a bdem_Schema where each record except the first one
        //   has a LIST (or CHOICE) field constrained to the previous record.
        //   Check for the element names and types.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "TEST constrained lists." << bsl::endl;
        {
            bsl::string bdemScript = "*{*a@D0}";  // record 0 is unnamed
            bsl::string xsdScript =
                "[0][1]{"                     // header <xs:schema>
                "[2][19]"                     // boilerplate
                "[4R]{[10]{[7Fa@4@1]}}[19]";  // record 0 has one simple field

            char previousRecordNameChar = 'R';
            for (int record = 0; record < MAXNUM_NAMES; ++record) {
                bdemScript += 'A' + record;  // SEQUENCE record name
                bdemScript += '{';
                bdemScript += 'A' + record;  // constrained LIST field name
                bdemScript += 'u';           // constrained LIST
                bdemScript += '0' + record;  // constrained LIST field type
                bdemScript += '}';

                xsdScript += "[4";
                xsdScript += 'A' + record;  // <xs:complexType name="A">
                xsdScript += "]{[10]{[7";   // <xs:sequence><xs:element
                xsdScript += 'A' + record;  // name="A"
                xsdScript += ':';
                xsdScript += previousRecordNameChar; // type="bb:prev-Record"
//                xsdScript += "@3]";       // minOccurs="1"/>
                xsdScript += "]";           // />
                xsdScript += "}}[19]";      // </xs:sequence> </xs:complexType>

                previousRecordNameChar = 'A' + record;
            }
            xsdScript += "[7R:R][19]";      // <xs:element name="R" type="R"/>
            for (int record = 0; record < MAXNUM_NAMES; ++record) {
                xsdScript += "[7";          // <xs:element
                xsdScript += 'A' + record;  // name="A"
                xsdScript += ':';
                xsdScript += 'A' + record;  // type="bb:A"
                xsdScript += "][19]";       // />\n
            }
            xsdScript += "}";

            bdem_Schema bdemSchema;
            gBdemSchema(&bdemSchema, bdemScript);

            Obj generator;
            bsl::ostringstream schemaStream;
            int status = generator.generateXsd(schemaStream,
                                               bdemSchema,
                                               TESTNAMESPACE,
                                               0, 4, INT_MAX);
            ASSERT(0 == status);

            bsl::string expectedXsd;
            gXML(&expectedXsd, xsdScript);

            if (veryVerbose) {
                P(bdemSchema);
                Q(schemaStream.str());
                bsl::cout << schemaStream.str();
                Q(schemaStream.str());
                Q(expectedXsd);
                bsl::cout << expectedXsd;
                Q(expectedXsd); }
            ASSERT(schemaStream.str() == expectedXsd);
        }

        if (verbose) bsl::cout << "TEST constrained choices." << bsl::endl;
        {
            bsl::string bdemScript = "*?{*a@D0}";  // record 0 is unnamed
            bsl::string xsdScript =
                "[0][1]{"                     // header <xs:schema>
                "[2][19]"                     // boilerplate
                "[4R]{[18]{[7Fa@4@1]}}[19]";  // record 0 has one simple field

            char previousRecordNameChar = 'R';
            for (int record = 0; record < MAXNUM_NAMES; ++record) {
                bdemScript += 'A' + record;  // record name
                bdemScript += '?';           // CHOICE
                bdemScript += '{';
                bdemScript += 'A' + record;  // constrained CHOICE field name
                bdemScript += 'E';           // constrained CHOICE
                bdemScript += '0' + record;  // constrained CHOICE field type
                bdemScript += '}';

                xsdScript += "[4";
                xsdScript += 'A' + record;  // <xs:complexType name="A">
                xsdScript += "]{[18]{[7";   // <xs:choice><xs:element
                xsdScript += 'A' + record;  // name="A"
                xsdScript += ':';
                xsdScript += previousRecordNameChar; // type="bb:prev-Record"
                xsdScript += "@1]";         // minOccurs="0"/>
                xsdScript += "}}[19]";      // </xs:choice></xs:complexType>\n

                previousRecordNameChar = 'A' + record;
            }
            xsdScript += "[7R:R][19]";      // <xs:element name="R" type="R"/>
            for (int record = 0; record < MAXNUM_NAMES; ++record) {
                xsdScript += "[7";          // <xs:element
                xsdScript += 'A' + record;  // name="A"
                xsdScript += ':';
                xsdScript += 'A' + record;  // type="bb:A"
                xsdScript += "][19]";       // />\n
            }
            xsdScript += "}";

            bdem_Schema bdemSchema;
            gBdemSchema(&bdemSchema, bdemScript);

            Obj generator;
            bsl::ostringstream schemaStream;
            int status = generator.generateXsd(schemaStream,
                                               bdemSchema,
                                               TESTNAMESPACE,
                                               0, 4, INT_MAX);
            ASSERT(0 == status);

            bsl::string expectedXsd;
            gXML(&expectedXsd, xsdScript);

            if (veryVerbose) {
                P(bdemSchema);
                Q(schemaStream.str());
                bsl::cout << schemaStream.str();
                Q(schemaStream.str());
                Q(expectedXsd);
                bsl::cout << expectedXsd;
                Q(expectedXsd);
            }

            ASSERT(schemaStream.str() == expectedXsd);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING complexType generation of anonymous records
        //
        // Concerns:
        //   Naming of anonymous records in XSD schema is correct
        //
        // Plan:
        //   Construct a bdem_Schema schema with MAXNUM_ANONYMOUS_RECORDS
        //   anonymous records that contains only one simple field, without
        //   cross-reference (i.e., constrained fields).  Check for correct
        //   naming of complexType's and global elements
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "TEST anonymous records" << bsl::endl;

        if (verbose) bsl::cout << "Testing <xs:sequence>." << bsl::endl;
        {
            bsl::string bdemScript;
            bsl::string xsdScript =
                "[0][1]{"     // header <xs:schema>
                "[2][19]";    // boilerplate

            for (int record = 0; record < MAXNUM_ANONYMOUS_RECORDS; ++record) {
                bdemScript += "*{Aa}";      // SEQUENCE record
                xsdScript += "[4";
                xsdScript += 'R' + record;  // <xs:complexType name="RECORD_#"
                xsdScript += "]{[10]{";     // <xs:sequence>
                xsdScript += "[7Aa@1]}}";   // <xs:element name="A"
                                            //             type="a" .../>
                                            // </xs:sequence> </xs:complexType>
                xsdScript += "[19]";        // \n
            }

            for (int record = 0; record < MAXNUM_ANONYMOUS_RECORDS; ++record) {
                xsdScript += "[7";          // global <xs:element
                xsdScript += 'R' + record;  // name="RECORD_#"
                xsdScript += ':';           // type="bb:RECORD_#"
                xsdScript += 'R' + record;
                xsdScript += "][19]";       // \n
            }
            xsdScript += '}';               // </xs:schema>

            bdem_Schema bdemSchema;
            bsl::string expectedXsd;

            gBdemSchema(&bdemSchema, bdemScript);
            gXML(&expectedXsd, xsdScript);

            if (veryVerbose) {
                P(bdemScript);
                P(xsdScript);
                P(bdemSchema);
                Q(expectedXsd);
                bsl::cout << expectedXsd;
                Q(expectedXsd);
            }

            bsl::ostringstream schemaStream;

            Obj generator;

            int status = generator.generateXsd(schemaStream,
                                               bdemSchema,
                                               TESTNAMESPACE,
                                               0, 4, INT_MAX);
            ASSERT(0 == status);
            LOOP2_ASSERT(schemaStream.str(), expectedXsd,
                         schemaStream.str() == expectedXsd);
            if (veryVerbose) {
                Q(schemaStream.str());
                bsl::cout << schemaStream.str();
                Q(schemaStream.str());
            }
        }

        if (verbose) bsl::cout << "Testing <xs:choice>." << bsl::endl;
        {
            bsl::string bdemScript;
            bsl::string xsdScript =
                "[0][1]{"     // header <xs:schema>
                "[2][19]";    // boilerplate

            for (int record = 0; record < MAXNUM_ANONYMOUS_RECORDS; ++record) {
                bdemScript += "*?{Aa}";     // CHOICE record
                xsdScript += "[4";
                xsdScript += 'R' + record;  // <xs:complexType name="RECORD_#"
                xsdScript += "]{[18]{";     // <xs:choice>
                xsdScript += "[7Aa@1]}}";   // <xs:element name="A"
                                            //             type="a".../>
                                            // </xs:choice> </xs:complexType>
                xsdScript += "[19]";        // \n
            }

            for (int record = 0; record < MAXNUM_ANONYMOUS_RECORDS; ++record) {
                xsdScript += "[7";          // global <xs:element
                xsdScript += 'R' + record;  // name="RECORD_#"
                xsdScript += ':';           // type="bb:RECORD_#"
                xsdScript += 'R' + record;
                xsdScript += "][19]";       // \n
            }
            xsdScript += '}';               // </xs:schema>

            bdem_Schema bdemSchema;
            bsl::string expectedXsd;

            gBdemSchema(&bdemSchema, bdemScript);
            gXML(&expectedXsd, xsdScript);

            if (veryVerbose) {
                P(bdemScript);
                P(xsdScript);
                P(bdemSchema);
                Q(expectedXsd);
                bsl::cout << expectedXsd;
                Q(expectedXsd);
            }

            bsl::ostringstream schemaStream;

            Obj generator;

            int status = generator.generateXsd(schemaStream,
                                               bdemSchema,
                                               TESTNAMESPACE,
                                               0, 4, INT_MAX);
            ASSERT(0 == status);
            LOOP2_ASSERT(schemaStream.str(), expectedXsd,
                         schemaStream.str() == expectedXsd);
            if (veryVerbose) {
                Q(schemaStream.str());
                bsl::cout << schemaStream.str();
                Q(schemaStream.str());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING element generation of anonymous fields for basic types
        //
        // Concerns:
        //   Naming of anonymous fields in XSD schema is correct.  Naming of
        //   anonymous attributes is also correct.  There can be any mix of
        //   anonymous fields and attributes.
        //
        // Plan:
        //   Construct a bdem_Schema schema with one record that
        //   contains only anonymous fields (as many as
        //   MAXNUM_ANONYMOUS_FIELDS allows).  Enumerate this record with all
        //   basic (non-LIST/TABLE) types.  Check for namespace prefixing,
        //   correct naming.
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << "TEST anonymous fields for basic types" << bsl::endl;

        static const char RECNAMECHARS[]  = "ABCDEFG";
        static const char BDEMTYPECHARS[] = "abcdefghijklmnopqrstwxyzABC";
        static const char XMLTYPECHARS[]  = "abcdefghijabcdefghijwhijwhi";
        ASSERT(sizeof BDEMTYPECHARS == sizeof XMLTYPECHARS);

        if (verbose) bsl::cout << "Testing <xs:sequence>." << bsl::endl;
        for (unsigned i = 0; i < sizeof BDEMTYPECHARS - 1; ++i) {
            EType::Type type = gType(BDEMTYPECHARS[i]);
            ASSERT(! EType::isAggregateType(type));

            // We will create a number of records, each with a different
            // number of attributes.  If type is not scalar, then create only
            // one record with no attributes.
            const int numRecs = EType::isScalarType(type) ?
                sizeof RECNAMECHARS - 1 : 1;

            bsl::string bdemScript;
            bsl::string xsdScript =
                "[0][1]{"                        // header <xs:schema>
                "[2][19]";                       // boilerplate

            for (int numAttr = 0; numAttr < numRecs; ++numAttr) {
                // For each value of 'numAttr', create a record with that
                // number of unnamed attributes.  The rest of the unnamed
                // fields will be elements.
                bdemScript += RECNAMECHARS[numAttr]; // Begin record
                bdemScript += '{';                   // SEQUENCE record

                xsdScript += "[4";                   // <xs:complexType
                xsdScript += RECNAMECHARS[numAttr];  //     name="A"
                xsdScript += "]{";                   // >

                bool isAttr = true;
                for (int field = 0; field < MAXNUM_ANONYMOUS_FIELDS; ++field) {

                    if (field == numAttr) {
                        // Start sequence after all attributes are done.
                        xsdScript += "[10]{"; // <xs:sequence>
                        isAttr = false;
                    }

                    bdemScript += '*'; // an anonymous field
                    bdemScript += BDEMTYPECHARS[i];

                    if (isAttr) {
                        xsdScript += "[22";       // <xs:attribute
                    }
                    else {
                        xsdScript += "[7";        // <xs:element
                    }
                    xsdScript += 'F' + field;     // name="FIELD_#"
                    xsdScript += XMLTYPECHARS[i]; // type="xml type"

                    if (EType::isScalarType(type)) {  // has default value
                        const int DEFAULT_INDEX = gDefaultIndex(type);
                        xsdScript += '@';
                        bsl::ostringstream index;
                        index << DEFAULT_INDEX;
                        xsdScript += index.str();  // default value

                        bdemScript += "@D0";
                    }

                    if (isAttr) {
                        bdemScript += "@FA";   // IS_ATTRIBUTE formatting
                    }

                    if (! isAttr) {
                        xsdScript += "@1"; // minOccurs
                    }

                    if (EType::isArrayType(type)) {
                        xsdScript += "@2"; // maxOccurs;
                    }

                    xsdScript += ']'; // />
                }
                bdemScript += '}';       // End SEQUENCE RECORD
                xsdScript  += "}}[19]";  // </xs:sequence></xs:complexType>\n

            } // end for (numAttr)

            for (int rec = 0; rec < numRecs; ++rec) {
                xsdScript += "[7";              // <xs:element
                xsdScript += RECNAMECHARS[rec]; //     name="A"
                xsdScript += ':';
                xsdScript += RECNAMECHARS[rec]; //     type="bb:A"
                xsdScript += "][19]";           // />
            }
            xsdScript += "}";                   // </xs:schema>

            bdem_Schema bdemSchema;
            bsl::string expectedXsd;

            gBdemSchema(&bdemSchema, bdemScript);
            gXML(&expectedXsd, xsdScript);

            if (veryVerbose) {
                P(i);
                P(bdemScript);
                P(xsdScript);
                P(bdemSchema);
                Q(expectedXsd);
                bsl::cout << expectedXsd;
                Q(expectedXsd);
            }

            bsl::ostringstream schemaStream;

            Obj generator;

            int status = generator.generateXsd(schemaStream,
                                               bdemSchema,
                                               TESTNAMESPACE,
                                               0, 4, INT_MAX);
            LOOP_ASSERT(BDEMTYPECHARS[i], 0 == status);
            LOOP3_ASSERT(BDEMTYPECHARS[i], schemaStream.str(), expectedXsd,
                         schemaStream.str() == expectedXsd);

            if (veryVerbose) {
                Q(schemaStream.str());
                bsl::cout << schemaStream.str();
                Q(schemaStream.str());
            }
        } // end for (each type)

        if (verbose) bsl::cout << "Testing <xs:choice>." << bsl::endl;
        for (unsigned i = 0; i < sizeof BDEMTYPECHARS - 1; ++i) {
            EType::Type type = gType(BDEMTYPECHARS[i]);
            ASSERT(! EType::isAggregateType(type));

            bsl::string bdemScript = "A?{";  // CHOICE record
            bsl::string xsdScript =
                "[0][1]{"      // header <xs:schema>
                "[2][19]"      // boilerplate
                "[4A]{[18]{";  // <xs:complexType name="A"><xs:choice>

            for (int field = 0; field < MAXNUM_ANONYMOUS_FIELDS; ++field) {
                bdemScript += '*'; // an anonymous field
                bdemScript += BDEMTYPECHARS[i];

                xsdScript += "[7";            // <xs:element
                xsdScript += 'F' + field;     // name="FIELD_#"
                xsdScript += XMLTYPECHARS[i]; // type="xml type"

                if (EType::isScalarType(type)) {  // has default value
                    const int DEFAULT_INDEX = gDefaultIndex(type);
                    xsdScript += '@';
                    bsl::ostringstream index;
                    index << DEFAULT_INDEX;
                    xsdScript += index.str();  // default value

                    bdemScript += "@D0";
                }
                xsdScript += "@1";  // minOccurs

                if (EType::isArrayType(type)) {
                    xsdScript += "@2";  // maxOccurs;
                }

                xsdScript += ']'; // />
            }
            bdemScript += '}';
            xsdScript +=
                "}}[19]"      // </xs:choice></xs:complexType>\n
                "[7A:A][19]"  // global element definition
                "}";          // </xs:schema>

            bdem_Schema bdemSchema;
            bsl::string expectedXsd;

            gBdemSchema(&bdemSchema, bdemScript);
            gXML(&expectedXsd, xsdScript);

            if (veryVerbose) {
                P(i);
                P(bdemScript);
                P(xsdScript);
                P(bdemSchema);
                Q(expectedXsd);
                bsl::cout << expectedXsd;
                Q(expectedXsd);
            }

            bsl::ostringstream schemaStream;

            Obj generator;

            int status = generator.generateXsd(schemaStream,
                                               bdemSchema,
                                               TESTNAMESPACE,
                                               0, 4, INT_MAX);
            LOOP_ASSERT(BDEMTYPECHARS[i], 0 == status);
            LOOP3_ASSERT(BDEMTYPECHARS[i], schemaStream.str(), expectedXsd,
                         schemaStream.str() == expectedXsd);

            if (veryVerbose) {
                Q(schemaStream.str());
                bsl::cout << schemaStream.str();
                Q(schemaStream.str());
            }
        } // end for
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING element generation for fields of basic bdem_ElemType's
        // (except aggregate types)
        //
        // Concerns:
        //   1. In a bdem_RecordDef, fields of the following types are
        //      translated into XML elements:
        //    a. All basic bdem_ElemType::XXX with built-in XML equivalents
        //    b. All bdem_ElemType::XXX_ARRAY types
        //   2. Default values are generated.
        //   3. The translation is affected by the following formatting modes:
        //
        //      Type            Formatting    XML type
        //      ----            ----------    --------
        //      CHAR            TEXT          string of length 1
        //      CHAR            DEC           byte
        //      STRING          HEX           hexBinary
        //      STRING          BASE64        base64Binary
        //      STRING          TEXT          string
        //      CHAR_ARRAY      HEX           hexBinary
        //      CHAR_ARRAY      BASE64        base64Binary
        //      CHAR_ARRAY      TEXT          string
        //      x_ARRAY         DEFAULT       x with maxOccurs="unbounded"
        //      x_ARRAY         LIST          list with itemType="x"
        //   TBD: Not all of the above combinations are tested.
        //
        //   4. Records are generated as the global element
        //
        // Plan:
        //   Use a simple record that has field of one basic bdem_ElemType,
        //   and another simple record that has a field that enumerates all
        //   basic bdem_ElemType's.
        //   check the generated XML schema that the field is mapped to
        //   XML element of the root element with an expected XML type:
        //   XML built-in type or type with bb namespace prefix
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << "TEST field and global element generation"
                      << bsl::endl;

        static const char BDEMTYPECHARS[] =
            "abcdefghijklmnopqrstklmnopqrstwxyzABC";
        static const char XMLTYPECHARS[]  =
            "abcdefghijabcdefghijklmnopqrstwhijxyz";
        ASSERT(sizeof BDEMTYPECHARS == sizeof XMLTYPECHARS);

        if (verbose) bsl::cout << "Testing <xs:sequence>." << bsl::endl;
        {
            char xsdScript[] =
                "[0][1]{"
                "[2][19]"
                "[4A]{[10]{[7B?]}}[19]"    // complexType for record 0
                "[4C]{[10]{[7Dd@1]}}[19]"  // complexType for record 1
                "[7A:A][19][7C:C][19]"     // global elements for records 0 & 1
                "}";
            // ? holds place for:
            //   - xml type
            //   - default value, minOccurs, maxOccurs attributes
            // '10' == '<xs:sequence>'
            char *xsdTypeCharPtr = bsl::strchr(xsdScript, '?');

            for (unsigned i = 0; i < sizeof BDEMTYPECHARS - 1; ++i) {
                EType::Type type = gType(BDEMTYPECHARS[i]);
                ASSERT(! EType::isAggregateType(type));

                bsl::string xsdAttributeScript;
                char xsdTypeChar = XMLTYPECHARS[i]; // xml type:
                bool isList = ('k' <= xsdTypeChar && xsdTypeChar <= 't')
                           || ('x' <= xsdTypeChar && xsdTypeChar <= 'z');

                xsdAttributeScript += xsdTypeChar;

                if (EType::isScalarType(type)) {  // has default value
                    const int DEFAULT_INDEX = gDefaultIndex(type);
                    xsdAttributeScript += '@';
                    bsl::ostringstream index;
                    index << DEFAULT_INDEX;
                    xsdAttributeScript += index.str();  // default value
                }

                xsdAttributeScript += "@1";  // minOccurs

                if (EType::isArrayType(type) && ! isList) {
                    // Set maxOccurs='unbounded', but not for LIST formatting
                    xsdAttributeScript += "@2";  // maxOccurs;
                }

                // The gBdemSchema script is identical in each iteration except
                // for the type of the single field (and possibly the
                // formatting mode attribute).

                bsl::string bdemScript = "A{B";  // SEQUENCE record
                bdemScript += BDEMTYPECHARS[i];
                if (isList) {
                    bdemScript += "@FL";
                }
                bdemScript += "@D0}C{Dd}";

                // expand xsd script
                bsl::string xsdScriptWithAttributes(
                                                   xsdScript,
                                                   xsdTypeCharPtr - xsdScript);
                xsdScriptWithAttributes.append(xsdAttributeScript);
                xsdScriptWithAttributes.append(
                                             xsdTypeCharPtr + 1,
                                             bsl::strlen(xsdScript) -
                                             (xsdTypeCharPtr - xsdScript + 1));
                bdem_Schema bdemSchema;
                bsl::string expectedXsd;

                gBdemSchema(&bdemSchema, bdemScript);
                gXML(&expectedXsd, xsdScriptWithAttributes);

                if (veryVerbose) {
                    P(i);
                    P(bdemScript);
                    P(xsdScriptWithAttributes);
                    P(bdemSchema);
                    Q(expectedXsd);
                    bsl::cout << expectedXsd;
                    Q(expectedXsd);
                }

                bsl::ostringstream schemaStream;

                Obj generator;

                int status = generator.generateXsd(schemaStream,
                                                   bdemSchema,
                                                   TESTNAMESPACE,
                                                   0, 4, INT_MAX);

                LOOP_ASSERT(BDEMTYPECHARS[i], 0 == status);
                LOOP3_ASSERT(bdemSchema, schemaStream.str(), expectedXsd,
                             schemaStream.str() == expectedXsd);

                if (veryVerbose) {
                    Q(schemaStream.str());
                    bsl::cout << schemaStream.str();
                    Q(schemaStream.str());
                }
            }
        }

        if (verbose) bsl::cout << "Testing <xs:choice>." << bsl::endl;
        {
            char xsdScript[] =
                "[0][1]{"
                "[2][19]"
                "[4A]{[18]{[7B?]}}[19]"    // complexType for record 0
                "[4C]{[18]{[7Dd@1]}}[19]"  // complexType for record 1
                "[7A:A][19][7C:C][19]"     // global elements for records 0 & 1
                "}";
            // ? holds place for:
            //   - xml type
            //   - default value, minOccurs, maxOccurs attributes
            // '18' == '<xs:choice>'
            char *xsdTypeCharPtr = bsl::strchr(xsdScript, '?');

            for (unsigned i = 0; i < sizeof BDEMTYPECHARS - 1; ++i) {
                EType::Type type = gType(BDEMTYPECHARS[i]);
                ASSERT(! EType::isAggregateType(type));

                bsl::string xsdAttributeScript;
                char xsdTypeChar = XMLTYPECHARS[i]; // xml type:
                bool isList = ('k' <= xsdTypeChar && xsdTypeChar <= 't')
                           || ('x' <= xsdTypeChar && xsdTypeChar <= 'z');

                xsdAttributeScript += xsdTypeChar;

                if (EType::isScalarType(type)) {  // has default value
                    const int DEFAULT_INDEX = gDefaultIndex(type);
                    xsdAttributeScript += '@';
                    bsl::ostringstream index;
                    index << DEFAULT_INDEX;
                    xsdAttributeScript += index.str();  // default value
                }

                xsdAttributeScript += "@1";  // minOccurs

                if (EType::isArrayType(type) && ! isList) {
                    // Set maxOccurs='unbounded', but not for LIST formatting
                    xsdAttributeScript += "@2";  // maxOccurs;
                }

                // The gBdemSchema script is identical in each iteration except
                // for the type of the single field (and possibly the
                // formatting mode attribute).

                bsl::string bdemScript = "A?{B";  // CHOICE record
                bdemScript += BDEMTYPECHARS[i];
                if (isList) {
                    bdemScript += "@FL";
                }
                bdemScript += "@D0}C?{Dd}";

                // expand xsd script
                bsl::string xsdScriptWithAttributes(
                                                   xsdScript,
                                                   xsdTypeCharPtr - xsdScript);
                xsdScriptWithAttributes.append(xsdAttributeScript);
                xsdScriptWithAttributes.append(
                                             xsdTypeCharPtr + 1,
                                             bsl::strlen(xsdScript) -
                                             (xsdTypeCharPtr - xsdScript + 1));
                bdem_Schema bdemSchema;
                bsl::string expectedXsd;

                gBdemSchema(&bdemSchema, bdemScript);
                gXML(&expectedXsd, xsdScriptWithAttributes);

                if (veryVerbose) {
                    P(i);
                    P(bdemScript);
                    P(xsdScriptWithAttributes);
                    P(bdemSchema);
                    Q(expectedXsd);
                    bsl::cout << expectedXsd;
                    Q(expectedXsd);
                }

                bsl::ostringstream schemaStream;

                Obj generator;

                int status = generator.generateXsd(schemaStream,
                                                   bdemSchema,
                                                   TESTNAMESPACE,
                                                   0, 4, INT_MAX);

                LOOP_ASSERT(BDEMTYPECHARS[i], 0 == status);
                LOOP3_ASSERT(bdemSchema, schemaStream.str(), expectedXsd,
                             schemaStream.str() == expectedXsd);

                if (veryVerbose) {
                    Q(schemaStream.str());
                    bsl::cout << schemaStream.str();
                    Q(schemaStream.str());
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBREATHING/USAGE TEST"
                               << "\n====================" << bsl::endl;

        // Redirect standard out to a stringstream.
        bsl::ostringstream xsdOutput;
        bsl::streambuf *coutStreamBuf = bsl::cout.rdbuf();
        bsl::cout.rdbuf(xsdOutput.rdbuf());

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
        bdem_Schema schema;

        bdem_EnumerationDef *et = schema.createEnumeration("EventType");
        et->addEnumerator("CORP_LUNCH");
        et->addEnumerator("CORP_DINNER");

        bdem_RecordDef *crr = schema.createRecord("CrossReferenceRecord");
        bdem_FieldDefAttributes fieldAttr(bdem_ElemType::BDEM_INT);
        fieldAttr.defaultValue().theModifiableInt() = 10;
        crr->appendField(fieldAttr, "ID");
        crr->appendField(bdem_ElemType::BDEM_STRING, "description");
        crr->appendField(bdem_ElemType::BDEM_LIST, crr, "selfReference");

        bdem_RecordDef *er = schema.createRecord("EventRecord");
        er->appendField(bdem_ElemType::BDEM_DATETIME_ARRAY, "eventDate");
        fieldAttr.reset(bdem_ElemType::BDEM_SHORT);
        fieldAttr.defaultValue().theModifiableShort() = -1;
        er->appendField(fieldAttr, "variance");
        fieldAttr.reset(bdem_ElemType::BDEM_INT);
        fieldAttr.defaultValue().theModifiableInt() = 1;
        er->appendField(fieldAttr, et, "type");
        er->appendField(bdem_ElemType::BDEM_TABLE, er);

        bdem_RecordDef *record2 = schema.createRecord();
        record2->appendField(bdem_ElemType::BDEM_STRING);
        record2->appendField(bdem_ElemType::BDEM_STRING_ARRAY);
        fieldAttr.reset(bdem_ElemType::BDEM_STRING);
        fieldAttr.defaultValue().theModifiableString() = "CORP_DINNER";
        record2->appendField(fieldAttr, et);
        fieldAttr.reset(bdem_ElemType::BDEM_TIME);
        fieldAttr.defaultValue().theModifiableTime() = bdet_Time(0, 1, 2, 3);
        record2->appendField(fieldAttr, "clock");

        bdem_RecordDef *config = schema.createRecord("Configuration");
        fieldAttr.reset(bdem_ElemType::BDEM_CHAR);
        fieldAttr.defaultValue().theModifiableChar() = 'A';
        fieldAttr.setIsNullable(false);
        fieldAttr.setFormattingMode(bdeat_FormattingMode::BDEAT_ATTRIBUTE);
        config->appendField(fieldAttr, "grade");
        fieldAttr.reset(bdem_ElemType::BDEM_SHORT);
        fieldAttr.defaultValue().theModifiableShort() = -1;
        config->appendField(fieldAttr, "count");
        fieldAttr.reset(bdem_ElemType::BDEM_INT64);
        fieldAttr.defaultValue().theModifiableInt64() = -100;
        config->appendField(fieldAttr, "grains");
        fieldAttr.reset(bdem_ElemType::BDEM_DOUBLE);
        fieldAttr.setIsNullable(false);
        config->appendField(fieldAttr, "price");
        fieldAttr.reset(bdem_ElemType::BDEM_STRING);
        fieldAttr.defaultValue().theModifiableString() = "one";
        config->appendField(fieldAttr, "name");
        fieldAttr.reset(bdem_ElemType::BDEM_DATETIME);
        fieldAttr.defaultValue().theModifiableDatetime() =
                                         bdet_Datetime(2000, 1, 1, 0, 1, 2, 3);
        config->appendField(fieldAttr, "timestamp");
        fieldAttr.reset(bdem_ElemType::BDEM_DATE);
        fieldAttr.defaultValue().theModifiableDate() = bdet_Date(2000, 1, 1);
        config->appendField(fieldAttr, "deadline");
        fieldAttr.reset(bdem_ElemType::BDEM_TIME);
        fieldAttr.defaultValue().theModifiableTime() = bdet_Time(0, 1, 2, 3);
        config->appendField(fieldAttr, "dailyAlarm");
        config->appendField(bdem_ElemType::BDEM_CHAR_ARRAY, "sequence");
        config->appendField(bdem_ElemType::BDEM_LIST, crr, "crossReference");
        config->appendField(bdem_ElemType::BDEM_TABLE, er, "history");
        config->appendField(bdem_ElemType::BDEM_TABLE,
                            record2,
                            "namelessTable");
//..
// We generate the XSD for this 'bdem_Schema' object by creating a generator
// object and an output stream, then calling the 'generateXsd' method.  In
// addition to the output stream and the input schema, the 'generateXsd'
// method takes an optional target namespace, initial indent-level, spaces per
// indent-level, and wrap column:
//..
        baexml_SchemaGenerator generator;
        generator.generateXsd(bsl::cout, schema,
                              "http://bloomberg.com/schemas/configuration",
                              0, 4, 68);
//..
// The following is printed to 'bsl::cout':
//..
        const char EXPECTED_OUTPUT[] =
    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
    "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"\n"
    "    xmlns:bdem=\"http://bloomberg.com/schemas/bdem\"\n"
    "    targetNamespace=\"http://bloomberg.com/schemas/configuration\"\n"
    "    xmlns=\"http://bloomberg.com/schemas/configuration\"\n"
    "    elementFormDefault=\"qualified\">\n"
    "\n"
    "    <xs:simpleType name=\"BYTE_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:byte\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"SHORT_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:short\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"INT_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:int\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"INT64_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:long\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"FLOAT_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:float\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"DOUBLE_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:double\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"DECIMAL_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:decimal\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"STRING_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:string\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"DATETIME_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:dateTime\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"DATE_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:date\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"TIME_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:time\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"BOOL_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:boolean\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"DATETIMETZ_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:dateTime\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"DATETZ_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:date\"/>\n"
    "    </xs:simpleType>\n"
    "    <xs:simpleType name=\"TIMETZ_LIST_TYPE\">\n"
    "        <xs:list itemType=\"xs:time\"/>\n"
    "    </xs:simpleType>\n"
    "\n"
    "    <xs:simpleType name=\"EventType\">\n"
    "        <xs:restriction base=\"xs:string\">\n"
    "            <xs:enumeration value=\"CORP_LUNCH\" bdem:id=\"0\"/>\n"
    "            <xs:enumeration value=\"CORP_DINNER\" bdem:id=\"1\"/>\n"
    "        </xs:restriction>\n"
    "    </xs:simpleType>\n"
    "\n"
    "    <xs:complexType name=\"CrossReferenceRecord\">\n"
    "        <xs:sequence>\n"
    "            <xs:element name=\"ID\" type=\"xs:int\" default=\"10\"\n"
    "                minOccurs=\"0\"/>\n"
    "            <xs:element name=\"description\" type=\"xs:string\"/>\n"
    "            <xs:element name=\"selfReference\"\n"
    "                type=\"CrossReferenceRecord\" minOccurs=\"0\"/>\n"
    "        </xs:sequence>\n"
    "    </xs:complexType>\n"
    "\n"
    "    <xs:complexType name=\"EventRecord\">\n"
    "        <xs:sequence>\n"
    "            <xs:element name=\"eventDate\" type=\"xs:dateTime\"\n"
    "                minOccurs=\"0\" maxOccurs=\"unbounded\"/>\n"
    "            <xs:element name=\"variance\" type=\"xs:short\"\n"
    "                default=\"-1\" minOccurs=\"0\"/>\n"
    "            <xs:element name=\"type\" type=\"EventType\"\n"
    "                default=\"CORP_DINNER\" minOccurs=\"0\"/>\n"
    "            <xs:element name=\"FIELD_3\" type=\"EventRecord\"\n"
    "                minOccurs=\"0\" maxOccurs=\"unbounded\"/>\n"
    "        </xs:sequence>\n"
    "    </xs:complexType>\n"
    "\n"
    "    <xs:complexType name=\"RECORD_2\">\n"
    "        <xs:sequence>\n"
    "            <xs:element name=\"FIELD_0\" type=\"xs:string\"/>\n"
    "            <xs:element name=\"FIELD_1\" type=\"xs:string\"\n"
    "                minOccurs=\"0\" maxOccurs=\"unbounded\"/>\n"
    "            <xs:element name=\"FIELD_2\" type=\"EventType\"\n"
    "                default=\"CORP_DINNER\" minOccurs=\"0\"/>\n"
    "            <xs:element name=\"clock\" type=\"xs:time\"\n"
    "                default=\"00:01:02.003\" minOccurs=\"0\"/>\n"
    "        </xs:sequence>\n"
    "    </xs:complexType>\n"
    "\n"
    "    <xs:complexType name=\"Configuration\">\n"
    "        <xs:attribute name=\"grade\" type=\"xs:byte\" default=\"65\"/>\n"
    "        <xs:sequence>\n"
    "            <xs:element name=\"count\" type=\"xs:short\" default=\"-1\"\n"
    "                minOccurs=\"0\"/>\n"
    "            <xs:element name=\"grains\" type=\"xs:long\"\n"
    "                default=\"-100\" minOccurs=\"0\"/>\n"
    "            <xs:element name=\"price\" type=\"xs:double\"/>\n"
    "            <xs:element name=\"name\" type=\"xs:string\"\n"
    "                default=\"one\" minOccurs=\"0\"/>\n"
    "            <xs:element name=\"timestamp\" type=\"xs:dateTime\"\n"
    "                default=\"2000-01-01T00:01:02.003\" minOccurs=\"0\"/>\n"
    "            <xs:element name=\"deadline\" type=\"xs:date\"\n"
    "                default=\"2000-01-01\" minOccurs=\"0\"/>\n"
    "            <xs:element name=\"dailyAlarm\" type=\"xs:time\"\n"
    "                default=\"00:01:02.003\" minOccurs=\"0\"/>\n"
    "            <xs:element name=\"sequence\" type=\"xs:byte\"\n"
    "                minOccurs=\"0\" maxOccurs=\"unbounded\"/>\n"
    "            <xs:element name=\"crossReference\"\n"
    "                type=\"CrossReferenceRecord\"/>\n"
    "            <xs:element name=\"history\" type=\"EventRecord\"\n"
    "                minOccurs=\"0\" maxOccurs=\"unbounded\"/>\n"
    "            <xs:element name=\"namelessTable\" type=\"RECORD_2\"\n"
    "                minOccurs=\"0\" maxOccurs=\"unbounded\"/>\n"
    "        </xs:sequence>\n"
    "    </xs:complexType>\n"
    "\n"
    "    <xs:element name=\"CrossReferenceRecord\"\n"
    "        type=\"CrossReferenceRecord\"/>\n"
    "\n"
    "    <xs:element name=\"EventRecord\" type=\"EventRecord\"/>\n"
    "\n"
    "    <xs:element name=\"RECORD_2\" type=\"RECORD_2\"/>\n"
    "\n"
    "    <xs:element name=\"Configuration\" type=\"Configuration\"/>\n"
    "\n"
    "</xs:schema>\n";
//..

        bsl::cout.rdbuf(coutStreamBuf);  // End redirection
        bsl::string xsdOutputStr(xsdOutput.str());
        if (veryVerbose) {
            P(schema);
            P(xsdOutputStr.length());
            P(xsdOutputStr);
            P(bsl::strlen(EXPECTED_OUTPUT));
            P(EXPECTED_OUTPUT);
        }

        LOOP2_ASSERT(xsdOutputStr, EXPECTED_OUTPUT,
                     xsdOutputStr == EXPECTED_OUTPUT);
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = "
                  << testStatus << "." << bsl::endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
