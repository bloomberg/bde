// baexml_schemaparser.t.cpp                  -*-C++-*-

#include <baexml_schemaparser.h>
#include <baexml_errorinfo.h>

// bde_build.pl is not smart enough to know that
// XML2 is not defined.  But it's also not smart
// enough to realize this is not a real "#if 0"
#if 0 || defined(XML2)
#include <a_xml2_reader.h>
typedef BloombergLP::a_xml2::Reader   Reader;
#else
#include <baexml_minireader.h>
typedef BloombergLP::baexml_MiniReader   Reader;
#endif

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

#include <bdesb_fixedmeminstreambuf.h>       // for testing only

#include <bslma_default.h>                   // for testing only
#include <bslma_testallocator.h>             // for testing only

#include <bsls_platformutil.h>               // for testing only

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_cctype.h>

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
// Although 'baexml_SchemaParser' has very few public functions, there is a
// lot to test.  The 'parse' function can take a wide range of inputs
// and produce a wide range of results.  Most of the tests involve
// creating an XML Schema (XSD) document in memory, feeding it to the
// 'parse' function, and comparing the resulting 'bdem_Schema' object
// to the expected value.
//
// Because XSD documents can be many lines long, a compact script is
// used to represent the input test cases.  The test driver includes a
// function, 'gXsd' to convert this compact script into a full XSD
// document.  Additionally, the test driver includes a function,
// 'gBdemSchema', to construct a 'bdem_Schema' object from a different
// compact script language.
//
// Test cases were selected to exercise branches and loops in the code, as
// determined by a white-box review of 'baexml_schemaparser.cpp'.  Each XSD
// construct is tested and error conditions are checked.
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
                          << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\n";           \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J    \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\t" << #M << ": " << M << "\n";         \
               aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J     \
                         << "\t" << #K << ": " << K << "\t" << #L << ": "  \
                         << L << "\t" << #M << ": " << M << "\t" << #N     \
                         << ": " << N << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl; // Print ID and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;  // Quote ID literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << flush; // P(X) w/o '\n'
#define L_ __LINE__                                // current Line number
#define T_ bsl::cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPES AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

typedef baexml_SchemaParser Obj;
typedef baexml_ErrorInfo    ErrInfo;

typedef bdem_ElemRef             ERef;
typedef bdem_ElemType            EType;
typedef bdem_FieldSpec           FldSpec;
typedef bdem_FieldDefAttributes  FldAttr;
typedef bdem_RecordDef           RecDef;
typedef bdem_EnumerationDef      EnumDef;
typedef RecDef::RecordType       RecType;
typedef bsls_PlatformUtil::Int64 Int64;

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;
int veryVeryVeryVerbose = 0;

struct TestData {
    int         d_line;
    const char *d_xmlSchemaScript;
    const char *d_bdemSchemaScript;
};

struct TestData2 {
    int         d_line;
    const char *d_xmlSchemaScript1;
    const char *d_xmlSchemaScript2;
    const char *d_bdemSchemaScript;
};

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

const bdet_DatetimeTz A23(A07, -7);
const bdet_DatetimeTz B23(B07, 6);
const bdet_DatetimeTz N23 = bdetu_Unset<bdet_DatetimeTz>::unsetValue();

const bdet_DateTz     A24(A08, -5);
const bdet_DateTz     B24(B08, -4);
const bdet_DateTz     N24 = bdetu_Unset<bdet_DateTz>::unsetValue();

const bdet_TimeTz     A25(A09, 3);
const bdet_TimeTz     B25(B09, 11);
const bdet_TimeTz     N25 = bdetu_Unset<bdet_TimeTz>::unsetValue();

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Must be a macro, not a const array, to be expanded within string literals.
#define XML_TEST_NAMESPACE "http://bloomberg.com/schemas/test"

const char *const ELEMENTS[] = {
    "<?xml version='1.0' encoding='UTF-8'?>",                             // 0

    "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"            // 1
    "        xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
    "        targetNamespace='" XML_TEST_NAMESPACE "'\n"
    "        xmlns:bb='" XML_TEST_NAMESPACE "'\n"
    "        xmlns='" XML_TEST_NAMESPACE "'\n"
    "        elementFormDefault='qualified'>",

    "<xs:complexType>",                                                   // 2
    "<xs:complexType name='%1'>",                                         // 3
    "<xs:element name='%1'%2%3%4>",                                       // 4
    "<xs:element name='%1' type='%2'%3%4%5>",                             // 5
    "<xs:element name='%1' type='%2'%3%4%5%6/>",                          // 6
    "<xs:element ref='%1'%2%3/>",                                         // 7
    "<xs:all>",                                                           // 8
    "<xs:sequence%1%2%3>",                                                // 9
    "<xs:complexContent>",                                                // 10
    "<xs:simpleContent>",                                                 // 11
    "<xs:simpleType%2%3%4%5>",                                            // 12
    "<xs:simpleType name='%1'%2%3%4%5>",                                  // 13
    "<xs:extension base='%1'>",                                           // 14
    "<xs:restriction base='%1'%2%3%4%5>",                                 // 15
    "<xs:list itemType='%1'%2%3%4%5/>",                                   // 16
    "<xs:length value='20'/>",                                            // 17
    "<xs:choice%1%2%3>",                                                  // 18

    "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"            // 19
    "        xmlns='" XML_TEST_NAMESPACE "'\n"
    "        targetNamespace='" XML_TEST_NAMESPACE "'\n"
    "       %1%2%3%4>\n",

    "<xs:include schemaLocation='location1'/>",                           // 20

    "<xs:attribute name='%1'%2%3%4>",                                     // 21
    "<xs:attribute name='%1' type='%2'%3%4%5/>",                          // 22
    "<xs:attribute ref='%1'%2%3%4%5/>",                                   // 23
    "<xs:enumeration value='%1'%2%3%4%5/>",                               // 24
};

const int NUM_ELEMENTS = sizeof ELEMENTS / sizeof *ELEMENTS;

const char *const ATTRIBUTES[] = {
    "",                                                                   // 0
    " minOccurs='0'",                                                     // 1
    " minOccurs='1'",                                                     // 2
    " maxOccurs='1'",                                                     // 3
    " maxOccurs='2'",                                                     // 4
    " maxOccurs='unbounded'",                                             // 5
    " default='20'",                                                      // 6
    " default='one'",                                                     // 7
    " default='2005-01-18T19:22:00'",                                     // 8
    " xmlns:xs='http://www.w3.org/2001/XMLSchema'",                       // 9
    " xmlns:bb='" XML_TEST_NAMESPACE "'",                                 // 10
    " elementFormDefault='qualified'",                                    // 11
    " use='optional'",                                                    // 12
    " use='required'",                                                    // 13
    " use='prohibited'",                                                  // 14
    " fixed='20'",                                                        // 15
    " nillable='false'",                                                  // 16
    " bdem:id='0'",                                                       // 17
    " bdem:id='1'",                                                       // 18
    " bdem:id='2'",                                                       // 19
    " bdem:id='3'",                                                       // 20
    " default='false'",                                                   // 21
    " bdem:preserveEnumOrder='true'",                                     // 22
    " nillable='true'",                                                   // 23
};

const int NUM_ATTRIBUTES = sizeof ATTRIBUTES / sizeof *ATTRIBUTES;

const char *const NAMES[] = {
    "bb:Apple",         // A
    "bb:Banana",        // B
    "bb:Catapult",      // C
    "bb:Daddy",         // D
    "bb:Effervescent",  // E
    "bb:Ferocious",     // F
    "bb:Good",          // G
    "bb:Holiday",       // H
    "bb:Interesting",   // I
    "bb:Jump",          // J
    "bb:Kangaroo",      // K
    "bb:Largo",         // L
    "bb:Mandatory",     // M
    "bb:Nemo",          // N
    "bb:Opulent",       // O
    "bb:Perk",          // P
    "bb:Query",         // Q
    "bb:Radar",         // R
    "bb:Snow",          // S
    "bb:RECORD_0",      // T
    "bb:RECORD_1",      // U
    "bb:RECORD_99",     // V
    "bb:FIELD_0",       // W
    "bb:FIELD_1",       // X
    "bb:FIELD_2",       // Y
    "bb:FIELD_99"       // Z
};

const int NUM_NAMES = sizeof NAMES / sizeof *NAMES;

const char *const XMLTYPES[] = {
    "xs:boolean",             // a
    "xs:byte",                // b
    "xs:date",                // c
    "xs:dateTime",            // d
    "xs:decimal",             // e
    "xs:double",              // f
    "xs:float",               // g
    "xs:int",                 // h
    "xs:integer",             // i
    "xs:long",                // j
    "xs:negativeInteger",     // k
    "xs:nonNegativeInteger",  // l
    "xs:nonPositiveInteger",  // m
    "xs:normalizedString",    // n
    "xs:positiveInteger",     // o
    "xs:short",               // p
    "xs:string",              // q
    "xs:time",                // r
    "xs:unsignedByte",        // s
    "xs:unsignedInt",         // t
    "xs:unsignedLong",        // u
    "xs:unsignedShort",       // v
    "xs:base64Binary",        // w
    "xs:hexBinary",           // x
    "xs:Name"                 // y
};

const int NUM_XMLTYPES = sizeof XMLTYPES / sizeof *XMLTYPES;

EType::Type gType(char typeCode)
    // Return the 'bdem' element type corresponding to the specified
    // 'typeCode'.  Note that 'DATETIME', 'DATE', 'TIME', 'DATETIME_ARRAY',
    // 'DATE_ARRAY', and 'TIME_ARRAY' are not used by this component, but are
    // retained here to remain consistent with other 'baexml' test drivers.
    //..
    //  'a' => CHAR        'k' => CHAR_ARRAY
    //  'b' => SHORT       'l' => SHORT_ARRAY
    //  'c' => INT         'm' => INT_ARRAY
    //  'd' => INT64       'n' => INT64_ARRAY
    //  'e' => FLOAT       'o' => FLOAT_ARRAY
    //  'f' => DOUBLE      'p' => DOUBLE_ARRAY
    //  'g' => STRING      'q' => STRING_ARRAY
    //  'h' => DATETIME    'r' => DATETIME_ARRAY  ( not used )
    //  'i' => DATE        's' => DATE_ARRAY      ( not used )
    //  'j' => TIME        't' => TIME_ARRAY      ( not used )
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

char gBdemChar(EType::Type type)
{
    switch (type) {
      case EType::BDEM_CHAR: return 'a';
      case EType::BDEM_SHORT: return 'b';
      case EType::BDEM_INT: return 'c';
      case EType::BDEM_INT64: return 'd';
      case EType::BDEM_FLOAT: return 'e';
      case EType::BDEM_DOUBLE: return 'f';
      case EType::BDEM_STRING: return 'g';
      case EType::BDEM_DATETIME: return 'h';
      case EType::BDEM_DATE: return 'i';
      case EType::BDEM_TIME: return 'j';
      case EType::BDEM_CHAR_ARRAY: return 'k';
      case EType::BDEM_SHORT_ARRAY: return 'l';
      case EType::BDEM_INT_ARRAY: return 'm';
      case EType::BDEM_INT64_ARRAY: return 'n';
      case EType::BDEM_FLOAT_ARRAY: return 'o';
      case EType::BDEM_DOUBLE_ARRAY: return 'p';
      case EType::BDEM_STRING_ARRAY: return 'q';
      case EType::BDEM_DATETIME_ARRAY: return 'r';
      case EType::BDEM_DATE_ARRAY: return 's';
      case EType::BDEM_TIME_ARRAY: return 't';
      case EType::BDEM_LIST: return 'u';
      case EType::BDEM_TABLE: return 'v';
      case EType::BDEM_BOOL: return 'w';
      case EType::BDEM_DATETIMETZ: return 'x';
      case EType::BDEM_DATETZ: return 'y';
      case EType::BDEM_TIMETZ: return 'z';
      case EType::BDEM_BOOL_ARRAY: return 'A';
      case EType::BDEM_DATETIMETZ_ARRAY: return 'B';
      case EType::BDEM_DATETZ_ARRAY: return 'C';
      case EType::BDEM_TIMETZ_ARRAY: return 'D';
      case EType::BDEM_CHOICE: return 'E';
      case EType::BDEM_CHOICE_ARRAY: return 'F';

      default: {
        P(type);  ASSERT("Invalid bdem type used in 'gBdemType'" && 0);
      } break;
    }

    return 0;
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
    //  'D' => false
    //  'F' => false
    //  'T' => true
    //..
{
    switch (nullCode) {
      case 'D': return false;
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
                          int                          *fieldId,
                          EType::Type                   fieldType,
                          bsl::string::const_iterator&  iter)
    // Parse field attributes for the specified 'fieldType' from the specified
    // 'iter'.  Note that 'iter' is passed by reference and is advanced by this
    // function!
{
    // Reset attributes to their default values.

    *defaultIndex   = -1;
    *formattingMode = bdeat_FormattingMode::BDEAT_DEFAULT;
    *isNullable     = (EType::BDEM_LIST == fieldType ||
                       EType::isArrayType(fieldType)) ? false : true;
    *fieldId        = bdem_RecordDef::BDEM_NULL_FIELD_ID;

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
          case 'I': {
            ASSERT(bsl::isdigit((unsigned) attributeValue));
            *fieldId = attributeValue - '0';
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
    return NAMES[nameIndex] + offset;
}

static const int MAX_POSITIONAL_PARAMETERS = 9;

const char *
lookupParameters(const char *positionalParams[MAX_POSITIONAL_PARAMETERS],
                 const char *start,
                 const char *finish)
    // Given the substring '[start, finish)', parse it as an element lookup
    // and return the element pattern from the 'ELEMENTS' array.  Given an
    // example input substring of "13A:Bc@3", the parts are interpreted as
    // follows: The initial integer (13 in our example), is used as the index
    // in the 'ELEMENTS' array corresponding to the desired element pattern to
    // be returned.  The portion of the substitution string after the initial
    // integer encodes the string fragments to be returned for slot in the
    // 'positionalParams' array.  An upper-case letter ("A", in our example),
    // indexes an element in the 'NAMES' array and strips off the leading
    // "bb:".  A colon followed by an upper-case letter (":B") also indexes
    // the 'NAMES' array but includes the leading "bb:".  A lower-case letter
    // ("c") indexes the 'XMLTYPES' array, and an at-symbol followed by a
    // decimal integer ("@3") indexes the 'ATTRIBUTES' array.  If there are
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
            // Parameter is a name code, including "bb:" prefix.
            ++start;    // skip ':'
            ASSERT(bsl::isupper(*start));
            positionalParams[numParams++] = gName(*start++, true);
        }
        else if (bsl::isupper(*start)) {
            // Parameter is a name code, excluding "bb:" prefix.
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
    bsl::string::size_type savePos = 0;

    for (const char *p = spec; *p; ) {

        bsl::size_t span = bsl::strcspn(p, "[%{}^");
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
            savePos = result->length();

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
          case '{': {
            // Beginning of an open element

            // Save the position and length of the first tag in the
            // expanded string.

            ASSERT(p >= spec + 2);
            ASSERT(p[-1] == ']' || (p[-1] == '^' && p[-2] == ']'));
            ASSERT('<' == (*result)[savePos]);

            ++savePos;  // Advance past the '<' character
            size_type saveLen = result->find_first_of(" >", savePos) - savePos;
            ASSERT(savePos + saveLen < result->length());
            tagStack.push(subrange(savePos, saveLen));
            ++indentLevel;

            ++p;
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
                // If current position is just after one or more newlines, put
                // the caret at the first of the newlines.
                caret = result->find_last_not_of('\n', result->length()) + 1;
            }
            ++p;
          } break;
        }
    }

    return caret;
}

struct LineAndColumn {
    // A line and column position.
    int d_line;
    int d_column;
};

bsl::ostream& operator<<(bsl::ostream& os, const LineAndColumn& pos)
{
    return os << "line " << pos.d_line
              << ", column " << pos.d_column;
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
// Each element within the 'ELEMENTS' array must contain either a single,
// unclosed XML tag, or a complete XML element (including closing tag),
// possibly with nested elements.  An element reference for an unclosed tag
// must be followed by an open curly brace ({).  The closing curly brace (})
// is replaced by the corresponding closing tag.
//
// A caret does not generate any XML.  However, the position of the caret
// (line number and column number) in the generated XML is remembered and
// return from 'gXsd'.  This is useful for testing the error handling logic,
// where a specific line number is expected in the error state.
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
// [7:A]                <xs:element ref="bb:Apple"/>
//
// [2]{[9]{[6Bc]}}      <xs:complexType>
//                          <xs:sequence>
//                              <xs:element name="Banana" type="date"/>
//                          </xs:sequence>
//                      </xs:complexType>

LineAndColumn gXsd(bsl::string *xmlStr, const bsl::string& spec)
{
    xmlStr->clear();

    // process the string as if it were an element substitution with no
    // parameters.
    int caret = expandXmlEncoding(xmlStr, spec.c_str(), 0, 0);

    LineAndColumn ret = { -1, -1 };
    if (caret >= 0) {
        ret.d_line = bsl::count(xmlStr->begin(), xmlStr->begin() + caret,
                                '\n') + 1;
        ret.d_column = caret + 1;  // Column is one-based, not zero-based.
        if (ret.d_line > 1) {
            // Subtract position of last newline
            ret.d_column -= xmlStr->rfind('\n', caret - 1);
        }
    }

    return ret;
}

// Generate a 'bdem_Schema' from a compact specification string.
//
// Each record in the schema is specified using an upper-case letter, which
// corresponds to a name in the 'NAMES' array.  An '*' is used to specify an
// unnamed record.  The record name code is followed by an open curly brace
// ({) followed by zero or more sets of two or three-letter field
// definitions.
//
// Each field definition consists of a capital letter, again referring to the
// NAMES array, with '*' used for nameless fields.  The field name code is
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
//    @FL : Set IS_LIST formatting mode
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
//..
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
//..
void gBdemSchema(bdem_Schema *schema, const bsl::string& spec)
{
    schema->removeAll();

    enum Expectation { RECNAME, OPENCURLY, FIELDNAME, FIELDTYPE, CONSTRAINT,
                       ENUM_ID };
    enum Pass { CREATE_RECORDS = 0, APPEND_FIELDS = 1 };

    // The 'schema' must be constructed in two passes to accommodate forward
    // references to records.  Empty records are created in the first pass.
    // The records are populated in the second pass.

    for (int currentPass = CREATE_RECORDS; currentPass <= APPEND_FIELDS;
                                                               ++currentPass) {

        Expectation expected = RECNAME;
        RecDef *currRecord = 0;
        RecType recordType;
        EnumDef *currEnum = 0;
        bool isEnum = false;
        bdem_FieldDef *field = 0;
        const char *fieldName = 0;
        int recordIndex = 0;  // needed for '*' records in 'APPEND_FIELDS' pass
        int enumIndex = 0;
        EType::Type fieldType;
        int defaultIndex;
        int formattingMode;
        bool isNullable;
        int fieldId;

        for (bsl::string::const_iterator i = spec.begin(); i != spec.end();
                                                                         ++i) {
            switch (expected) {
              case RECNAME: {
                ASSERT(bsl::isupper(*i) || '*' == *i);
                const char *recordName = '*' == *i ? 0 : gName(*i);
                isEnum = false;
                if (*(i + 1) == '=') {
                    isEnum = true;
                    ++i;
                }
                else if (*(i + 1) == '?') {
                    recordType = RecDef::BDEM_CHOICE_RECORD;
                    ++i;
                }
                else {
                    recordType = RecDef::BDEM_SEQUENCE_RECORD;
                }

                expected = OPENCURLY;
                if (isEnum) {
                    if (CREATE_RECORDS == currentPass) {
                        currEnum = schema->createEnumeration(recordName);
                        ASSERT(currEnum);
                        ++enumIndex;
                    }
                }
                else {
                    if (CREATE_RECORDS == currentPass) {
                        currRecord = schema->createRecord(recordName,
                                                          recordType);
                    }
                    else {
                        const RecDef *constRecord;
                        if (recordName) {
                            constRecord = schema->lookupRecord(recordName);
                        }
                        else {  // unnamed record
                            constRecord = &schema->record(recordIndex);
                        }
                        currRecord = const_cast<RecDef *>(constRecord);
                    }
                    ASSERT(currRecord);
                    ++recordIndex;
                }
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
                expected = isEnum ? ENUM_ID : FIELDTYPE;
              } break;
              case FIELDTYPE: {
                fieldType = gType(*i);
                if (EType::isAggregateType(fieldType)) {
                    expected = CONSTRAINT;
                }
                else if ('=' == *(i + 1)) {
                    ++i;
                    expected = CONSTRAINT;
                }
                else {
                    parseFieldAttributes(&defaultIndex,
                                         &formattingMode,
                                         &isNullable,
                                         &fieldId,
                                         fieldType,
                                         i);

                    if (APPEND_FIELDS == currentPass) {
                        ASSERT(currRecord);

                        FldAttr fieldAttr(fieldType,
                                          isNullable, 
                                          formattingMode);
                        if (-1 != defaultIndex) {
                            setDefaultValueForType(fieldAttr.defaultValue(),
                                                   defaultIndex);
                        }

                        currRecord->appendField(fieldAttr, fieldName, fieldId);
                    }
                    expected = FIELDNAME;
                }
              } break;
              case CONSTRAINT: {
                ASSERT(bsl::isupper(*i) || bsl::isdigit(*i));
                const char constraintChar = *i;

                parseFieldAttributes(&defaultIndex,
                                     &formattingMode,
                                     &isNullable,
                                     &fieldId,
                                     fieldType,
                                     i);

                if (APPEND_FIELDS == currentPass) {
                    if (EType::isAggregateType(fieldType)) {
                        const RecDef *constraint = 0;
                        if (bsl::isupper(constraintChar)) {
                            constraint =
                                schema->lookupRecord(gName(constraintChar));
                        }
                        else {  // '0' .. '9'
                            constraint = &schema->record(constraintChar - '0');
                        }
                        ASSERT(constraint);

                        
                        FldAttr fieldAttr(fieldType, 
                                          isNullable,
                                          formattingMode);


                        currRecord->appendField(fieldAttr,
                                                constraint, 
                                                fieldName,
                                                fieldId);

                    }
                    else {
                        const EnumDef *constraint = 0;
                        if (bsl::isupper(constraintChar)) {
                            constraint =
                              schema->lookupEnumeration(gName(constraintChar));
                        }
                        else {  // '0' .. '9'
                            constraint =
                                &schema->enumeration(constraintChar - '0');
                        }
                        ASSERT(constraint);

                        FldAttr fieldAttr(fieldType,
                                          isNullable,
                                          formattingMode);
                        currRecord->appendField(fieldAttr, 
                                                constraint, 
                                                fieldName, 
                                                fieldId);
                    }
                }
                expected = FIELDNAME;
              } break;
              case ENUM_ID: {
                ASSERT(fieldName);
                if (CREATE_RECORDS == currentPass) {
                    if (bsl::isdigit(*i)) {
                        currEnum->addEnumerator(fieldName, *i - '0');
                    }
                    else {
                        currEnum->addEnumerator(fieldName);
                    }
                }

                if (! bsl::isdigit(*i)) {
                    --i; // Rescan next character
                }
                expected = FIELDNAME;
              } break;
              default: {
                ASSERT(0);
              } break;
            }
        }
    }
}

void runTest(int         line,
             const char *xmlSchemaScript,
             const char *bdemSchemaScript)
    // use 'gXsd' to generate an XSD document from 'xmlSchemaScript' and use
    // 'gBdemSchema' to generate a 'bdem_Schema' from 'SchemaScript'.  Create
    // a 'baexml_SchemaParser' object and parse the XSD document.  Assert that
    // the parse succeeds and that the 'bdem_Schema' constructed from the
    // parse matches the 'bdem_Schema' specified in the 'TestData' row.  Also
    // assert that 'errInfo' is not modified.
{
    bsl::string xsd;
    gXsd(&xsd, xmlSchemaScript);
    bsl::istringstream input(xsd);

    if (veryVerbose) bsl::cout << "XSD Input = \n" << xsd;

    bdem_Schema expected;
    gBdemSchema(&expected, bdemSchemaScript);

    if (veryVerbose) bsl::cout << "Expected = \n" << expected;

    Reader reader;
    ErrInfo  errInfo;
    Obj    parser(&reader, &errInfo);
    bdem_Schema result;
    bsl::string targetNamespace = "unknown";

    if (veryVeryVerbose) parser.setVerboseStream(&bsl::cout);

    int retval = parser.parse(input,
                              &result,
                              &targetNamespace,
                              "test.xsd");

    if (veryVerbose) bsl::cout << "Actual = \n" << result;

    LOOP2_ASSERT(line, retval, 0 <= retval);
    LOOP3_ASSERT(line, result, expected, result == expected);
    LOOP2_ASSERT(line, targetNamespace, XML_TEST_NAMESPACE == targetNamespace);
    LOOP2_ASSERT(line, errInfo,
                 errInfo.severity() == ErrInfo::BAEXML_NO_ERROR);
    LOOP_ASSERT(line, errInfo.message().empty());
    LOOP_ASSERT(line, errInfo.lineNumber() == 0);
    LOOP_ASSERT(line, errInfo.columnNumber() == 0);
}

class Resolver
{
    // This resolver can resolve only two schemaLocation values:
    // "location1"  and "location2"
  private:
    const char * d_schema1;
    const char * d_schema2;

  public:
    Resolver (const char * schema1 = 0, const char * schema2 = 0)
        : d_schema1 (schema1)
        , d_schema2 (schema2)
    {}

    typedef baexml_Reader::StreamBufPtr StreamBufPtr;
    typedef bdema_ManagedPtr < bsl::istringstream >  InpStrStreamPtr;

    StreamBufPtr  operator()(const char *schemaLocation,
                             const char *targetNs)
    {
        const char * ptr = 0;
        if (bsl::strcmp(schemaLocation, "location1") == 0)
            ptr = d_schema1;
        else if (bsl::strcmp(schemaLocation, "location2") == 0)
            ptr = d_schema2;

        if (ptr == 0)
            ptr = "";

        bsl::string buffer(ptr);

        bslma_Allocator * allocator = bslma_Default::defaultAllocator();
        InpStrStreamPtr issPtr (
            new (*allocator) bsl::istringstream(buffer));

        StreamBufPtr ret;
        ret.loadAlias(issPtr, issPtr->rdbuf());

        return ret;
    }
};

bool compareSchema(const bdem_Schema& lhs, const bdem_Schema& rhs)
{
    const int numRecords = lhs.numRecords();
    const int numEnums = lhs.numEnumerations();
    if (numRecords != rhs.numRecords()
     || numEnums != rhs.numEnumerations()) {  // Do likely-to-fail test first.
        return false;                                                 // RETURN
    }

    if (&lhs == &rhs) {  // "Identically equal"; cheap but less likely.
        return true;                                                  // RETURN
    }

    for (int i = 0; i < numRecords; ++i) {
        const bdem_RecordDef& lhsRecord = lhs.record(i);
        const bdem_RecordDef& rhsRecord = rhs.record(i);

        const int numFields = lhsRecord.numFields();
        if (numFields != rhsRecord.numFields()
         || lhsRecord.recordType() != rhsRecord.recordType()) {
            return false;                                             // RETURN
        }

        const char *lhsRecordName = lhs.recordName(lhsRecord.recordIndex());
        const char *rhsRecordName = rhs.recordName(rhsRecord.recordIndex());

        if (lhsRecordName) {
            if (!rhsRecordName || bsl::strcmp(lhsRecordName, rhsRecordName)) {
                return false;                                         // RETURN
            }
        }
        else {
            if (rhsRecordName) {
                return false;                                         // RETURN
            }
        }

        for (int j = 0; j < numFields; ++j) {
            const bdem_FieldDef& lhsField   = lhsRecord.field(j);
            const bdem_FieldDef& rhsField   = rhsRecord.field(j);

            const int            lhsFieldId = lhsRecord.fieldId(j);
            const int            rhsFieldId = rhsRecord.fieldId(j);

            const bdem_ElemType::Type type = lhsField.elemType();

            if (type                      != rhsField.elemType()
             || lhsField.formattingMode() != rhsField.formattingMode()
             || lhsField.isNullable()     != rhsField.isNullable()
             || lhsFieldId                != rhsFieldId) {
                return false;                                         // RETURN
            }

            if (lhsField.hasDefaultValue() != rhsField.hasDefaultValue()
             || (lhsField.hasDefaultValue() &&
                         lhsField.defaultValue() != rhsField.defaultValue())) {
                return false;                                         // RETURN
            }

            const int lhsRecConstraintIdx = lhsField.recordConstraint()
                         ? lhsField.recordConstraint()->recordIndex()
                         : -1;
            const int rhsRecConstraintIdx = rhsField.recordConstraint()
                         ? rhsField.recordConstraint()->recordIndex()
                         : -1;

            const int lhsEnumConstraintIdx = lhsField.enumerationConstraint()
                         ? lhsField.enumerationConstraint()->enumerationIndex()
                         : -1;
            const int rhsEnumConstraintIdx = rhsField.enumerationConstraint()
                         ? rhsField.enumerationConstraint()->enumerationIndex()
                         : -1;

            if (lhsRecConstraintIdx  != rhsRecConstraintIdx
             || lhsEnumConstraintIdx != rhsEnumConstraintIdx) {
                return false;                                         // RETURN
            }

            const char *const lhsFieldName = lhsRecord.fieldName(j);
            const char *const rhsFieldName = rhsRecord.fieldName(j);

            if (lhsFieldName) {
                if (!rhsFieldName || bsl::strcmp(lhsFieldName, rhsFieldName)) {
                    return false;                                     // RETURN
                }
            }
            else {
                if (rhsFieldName) {
                    return false;                                     // RETURN
                }
            }
        }
    }

    for (int i = 0; i < numEnums; ++i) {
        const bdem_EnumerationDef& lhsEnum = lhs.enumeration(i);
        const bdem_EnumerationDef& rhsEnum = rhs.enumeration(i);

        const char *lhsEnumName = lhs.enumerationName(i);
        const char *rhsEnumName = rhs.enumerationName(i);

        if (lhsEnumName) {
            if (!rhsEnumName || bsl::strcmp(lhsEnumName, rhsEnumName)) {
                return false;                                         // RETURN
            }
        }
        else {
            if (rhsEnumName) {
                return false;                                         // RETURN
            }
        }

        if (! bdem_EnumerationDef::areEquivalent(lhsEnum, rhsEnum)) {
            return false;                                             // RETURN
        }
    }

    return true;
}

void runTestInc(int         line,
                const char *xmlTopSchema,
                const char *xmlSchemaScript,
                const char *bdemSchemaScript)
    // use 'gXsd' to generate an XSD document from 'xmlSchemaScript' and use
    // 'gBdemSchema' to generate a 'bdem_Schema' from 'SchemaScript'.  Create
    // a 'baexml_SchemaParser' object and parse the XSD document.  Assert that
    // the parse succeeds and that the 'bdem_Schema' constructed from the
    // parse matches the 'bdem_Schema' specified in the 'TestData' row.  Also
    // assert that 'errInfo', is not modified.
{
    bsl::string xsdTop;
    bsl::string xsdInc;

    gXsd(&xsdTop, xmlTopSchema);
    gXsd(&xsdInc, xmlSchemaScript);

    if (veryVerbose) {
        bsl::cout << "XSD Top = " << xsdTop << bsl::endl;
        bsl::cout << "XSD Inc = " << xsdInc << bsl::endl;
    }

    Resolver resolver(xsdInc.c_str(), 0);

    bsl::istringstream input(xsdTop);

    bdem_Schema expected;
    gBdemSchema(&expected, bdemSchemaScript);

    if (veryVerbose) bsl::cout << "Expected = \n" << expected;

    Reader reader;
    ErrInfo  errInfo;
    Obj    parser(&reader, &errInfo);
    bdem_Schema result;
    Obj::SchemaElementAttributes attributes;
    bsl::string targetNamespace = "unknown";

    if (veryVeryVerbose) parser.setVerboseStream(&bsl::cout);

    reader.setResolver(resolver);

    int retval = parser.parse(input,
                              &result,
                              &attributes,
                              "test.xsd");

    if (veryVerbose) bsl::cout << "Actual = \n" << result;

    LOOP2_ASSERT(line, retval, 0 <= retval);
    LOOP2_ASSERT(line, result, result == expected);
    LOOP2_ASSERT(line, targetNamespace,
                 XML_TEST_NAMESPACE == attributes["targetNamespace"]);
    LOOP2_ASSERT(line, errInfo,
                 errInfo.severity () == ErrInfo::BAEXML_NO_ERROR);
    LOOP_ASSERT(line, errInfo.message().empty());
    LOOP_ASSERT(line, errInfo.lineNumber() == 0);
    LOOP_ASSERT(line, errInfo.columnNumber() == 0);
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

const char        SCHEMA1[]   =
"<?xml version='1.0' encoding='UTF-8'?>\n"
"<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
"           targetNamespace='" XML_TEST_NAMESPACE "'\n"
"           xmlns:bb='" XML_TEST_NAMESPACE "'"
"           elementFormDefault='qualified'>\n"
"\n"
"    <xs:simpleType name='CHAR_ARRAY'>\n"
"        <xs:list itemType='xs:byte'/>\n"
"    </xs:simpleType>\n"
"\n"
"    <xs:complexType name='CrossReferenceRecord'>\n"
"        <xs:sequence>\n"
"            <xs:element name='ID'          type='xs:int'/>\n"
"            <xs:element name='description' type='xs:string'/>\n"
"        </xs:sequence>\n"
"    </xs:complexType>\n"
"\n"
"    <xs:complexType name='EventRecord'>\n"
"        <xs:sequence>\n"
"            <xs:element name='eventDate' type='xs:date'/>\n"
"            <xs:element name='variance'  type='xs:short'/>\n"
"        </xs:sequence>\n"
"    </xs:complexType>\n"
"\n"
"    <xs:complexType name='RECORD_2'>  <!-- Auto-generated name -->\n"
"        <xs:sequence>\n"
"            <xs:element name='FIELD_0' type='xs:string'/>"
"  <!-- Auto name -->\n"
"            <xs:element name='FIELD_1' type='xs:double'/>"
"  <!-- Auto name -->\n"
"        </xs:sequence>\n"
"    </xs:complexType>\n"
"\n"
"    <xs:complexType name='Configuration'>\n"
"        <xs:sequence>\n"
"            <xs:element name='grade'          type='xs:byte'/>\n"
"            <xs:element name='count'          type='xs:short'/>\n"
"            <xs:element name='tolerance'      type='xs:int'/>\n"
"            <xs:element name='grains'         type='xs:long'/>\n"
"            <xs:element name='ratio'          type='xs:float'/>\n"
"            <xs:element name='price'          type='xs:double'/>\n"
"            <xs:element name='name'           type='xs:string'/>\n"
"            <xs:element name='timestamp'      type='xs:dateTime'\n"
"                nillable='1'/>\n"
"            <xs:element name='deadline'       type='xs:date'/>\n"
"            <xs:element name='dailyAlarm'     type='xs:time'/>\n"
"            <xs:element name='sequence'       type='bb:CHAR_ARRAY'\n"
"                maxOccurs='unbounded'/>\n"
"            <xs:element name='nillableArray'  type='bb:CHAR_ARRAY'\n"
"                nillable='true' maxOccurs='unbounded'/>\n"
"            <xs:element name='crossReference'"
" type='bb:CrossReferenceRecord'/>\n"
"            <xs:element name='history'        type='bb:EventRecord'\n"
"                maxOccurs='unbounded'/>\n"
"            <xs:element name='anonymous'      type='bb:RECORD_2'/>\n"
"            <xs:element name='NillableAnon'   type='bb:RECORD_2'\n"
"                nillable='1'/>\n"
"        </xs:sequence>\n"
"    </xs:complexType>\n"
"\n"
"    <xs:element name='Configuration' type='bb:Configuration'/>\n"
"\n"
"</xs:schema>\n";

const bsl::size_t SCHEMA1_LEN = sizeof(SCHEMA1) - 1;

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    bslma_TestAllocator testAllocator(veryVeryVeryVerbose);
    bslma_Default::setDefaultAllocatorRaw(&testAllocator);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 20: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                               << "\n=============" << bsl::endl;

// Frequently, the schema for a 'bdem_List' can be hard-wired into the
// program in the form of an string constant containing XSD data.  The
// following string describes a simple schema containing two records,
// "mailOrder" and "item":
//..
    const char XML_SCHEMA[] =
        "<?xml version='1.0' encoding='UTF-8'?>\n"
        "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
        "           xmlns:bb='http://bloomberg.com/schemas/mailOrder'\n"
        "           bdem:requestType='SctpsvcRequest'\n"
        "           bdem:responseType='SctpsvcResponse'\n"
        "           xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
        "           xmlns='http://www.w3.org/2001/XMLSchema'\n"
        "           targetNamespace='http://bloomberg.com/schemas/mailOrder'\n"
        "           elementFormDefault='qualified'>\n"
        "\n"
        "    <xs:element name='mailOrder'>\n"
        "        <xs:complexType>\n"
        "            <xs:sequence>\n"
        "                <xs:element name='orderDate' type='xs:date'/>\n"
        "                <xs:element name='customerName' type='xs:string'/>\n"
        "                <xs:element name='items' type='bb:item'\n"
        "                         maxOccurs='unbounded'/>\n"
        "            </xs:sequence>\n"
        "        </xs:complexType>\n"
        "    </xs:element> \n"
        "\n"
        "    <xs:simpleType name='ids'>"
        "        <xs:restriction base='xs:string'>"
        "            <xs:enumeration value='REAR_SETS' bdem:id='0'/>"
        "            <xs:enumeration value='DAMPER' bdem:id='1'/>"
        "            <xs:enumeration value='POWER_COMMANDER' bdem:id='2'/>"
        "        </xs:restriction>"
        "    </xs:simpleType>"
        "\n"
        "    <xs:complexType name='item'>\n"
        "        <xs:sequence>\n"
        "            <xs:element name='itemName' type='xs:string'/>\n"
        "            <xs:element name='id' type='bb:ids'/>\n"
        "            <xs:element name='size' type='xs:double'/>\n"
        "        </xs:sequence>\n"
        "    </xs:complexType>\n"
        "\n"
        "</xs:schema>\n";
//..
// In order to parse the XML schema, we first need to construct namespace
// registry, prefix stack, XML reader, error info, 'baexml_SchemaParser' and a
// 'bdem_Schema' objects:
//..
    baexml_NamespaceRegistry namespaces;
    baexml_PrefixStack       prefixes(&namespaces);
    baexml_MiniReader        reader;
    reader.setPrefixStack(&prefixes);
    baexml_ErrorInfo         errInfo;
    baexml_SchemaParser      parser(&reader, &errInfo);
    bdem_Schema              schema;

    if (veryVeryVerbose) parser.setVerboseStream(&bsl::cout);

//..
// The parser expects an 'istream' or 'streambuf' argument, so we must create
// a stream to read from the static XSD character array:
//..
    bsl::istringstream xmlStream(XML_SCHEMA);
//..
// Now we call the 'parse' method to construct the 'bdem_Schema' object from
// the XSD input text.  We invent a file name, "mailOrder.xsd" to be used in
// error messages, and pass the address of variables in which to capture error
// information:
//..
    bsl::string targetNamespace;
    int result = parser.parse(xmlStream,
                              &schema,
                              &targetNamespace,
                              "mailOrder.xsd");
    ASSERT(0 == result);
    ASSERT("http://bloomberg.com/schemas/mailOrder" == targetNamespace);
//..
// Display errors, if any:
//..
    if (0 != result) {
        bsl::cerr << "mailOrder.xsd:" << errInfo << bsl::endl;
    }
//..
// Confirm that the 'bdem_Schema' object has the desired structure:
//..
    const bdem_RecordDef *mailOrderRec = schema.lookupRecord("mailOrder");
    ASSERT(0 != mailOrderRec);
    const int mailOrderRecLen = (mailOrderRec ? mailOrderRec->numFields() : 0);
    ASSERT(3 == mailOrderRecLen);

    const bdem_RecordDef *itemRec = schema.lookupRecord("item");
    ASSERT(0 != itemRec);
    const int itemRecLen = (itemRec ? itemRec->numFields() : 0);
    ASSERT(3 == itemRecLen);

    const bdem_EnumerationDef *idsEnum = schema.lookupEnumeration("ids");
    ASSERT(0 != idsEnum);
    const int idsEnumLen = (idsEnum ? idsEnum->numEnumerators() : 0);
    ASSERT(3 == idsEnumLen);
    ASSERT(0 == idsEnum->lookupId("REAR_SETS"));
    ASSERT(1 == idsEnum->lookupId("DAMPER"));
    ASSERT(2 == idsEnum->lookupId("POWER_COMMANDER"));

    const bdem_FieldDef *dateField = mailOrderRec->lookupField("orderDate");
    ASSERT(0 != dateField);
    if (0 < mailOrderRecLen) ASSERT(&mailOrderRec->field(0) == dateField);
    if (0 != dateField) ASSERT(bdem_ElemType::BDEM_DATETZ
                               == dateField->elemType());

    const bdem_FieldDef *nameField = mailOrderRec->lookupField("customerName");
    ASSERT(0 != nameField);
    if (1 < mailOrderRecLen) ASSERT(&mailOrderRec->field(1) == nameField);
    if (0 != nameField) ASSERT(bdem_ElemType::BDEM_STRING
                               == nameField->elemType());

    const bdem_FieldDef *itemsField = mailOrderRec->lookupField("items");
    ASSERT(0 != itemsField);
    if (2 < mailOrderRecLen) ASSERT(&mailOrderRec->field(2) == itemsField);
    if (0 != itemsField) {
        ASSERT(bdem_ElemType::BDEM_TABLE == itemsField->elemType());
        ASSERT(itemRec == itemsField->recordConstraint());
    }

    const bdem_FieldDef *itemNameField = itemRec->lookupField("itemName");
    ASSERT(0 != itemNameField);
    if (0 < itemRecLen) ASSERT(&itemRec->field(0) == itemNameField);
    if (0 != itemNameField) {
        ASSERT(bdem_ElemType::BDEM_STRING == itemNameField->elemType());
    }

    const bdem_FieldDef *idField = itemRec->lookupField("id");
    ASSERT(0 != idField);
    if (1 < itemRecLen) ASSERT(&itemRec->field(1) == idField);
    if (0 != idField) {
        ASSERT(bdem_ElemType::BDEM_STRING == itemNameField->elemType());
    }

    const bdem_FieldDef *sizeField = itemRec->lookupField("size");
    ASSERT(0 != sizeField);
    if (2 < itemRecLen) ASSERT(&itemRec->field(2) == sizeField);
    if (0 != sizeField) ASSERT(bdem_ElemType::BDEM_DOUBLE
                               == sizeField->elemType());
//..

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING INVALID DEFAULT VALUES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING INVALID DEFAULT VALUES"
                               << "\n=============================="
                               << bsl::endl;

        const struct {
            const int   d_line;
            const char *d_schema_p;
            bool        d_parseSuccess;
        } DATA[] = {
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='BoolFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:boolean'"
         "                            default=\"true\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         true
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='BoolFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:boolean'"
         "                            default=\"TRUE\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         true
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='BoolFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:boolean'"
         "                            default=\"false\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         true
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='BoolFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:boolean'"
         "                            default=\"FALSE\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         true
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='BoolFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:boolean'"
         "                            default=\"0\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         true
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='BoolFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:boolean'"
         "                            default=\"1\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         true
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='BoolFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:boolean'"
         "                            default=\"Invalid\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         false
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='BoolFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:boolean'"
         "                            default=\"Bad Flag\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         false
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='BoolFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:boolean'"
         "                            default=\"2\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         false
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='BoolFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:boolean'"
         "                            default=\"-1\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         false
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='CharFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:byte'"
         "                            default=\"1\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         true
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='ShortFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:short'"
         "                            default=\"123\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         true
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='ShortFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:short'"
         "                            default=\"123\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         true
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='IntFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:int'"
         "                            default=\"12AB\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         false
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='IntFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:int'"
         "                            default=\"123\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         true
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='IntFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:int'"
         "                            default=\"12AB\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         false
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='FloatFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:float'"
         "                            default=\"123.345\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         true
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='FloatFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:float'"
         "                            default=\"12AB.123\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         false
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='DoubleFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:double'"
         "                            default=\"123.345\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         true
            },
            {
         L_,
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
         "    <xs:element name='DoubleFlag'>\n"
         "        <xs:complexType>\n"
         "            <xs:sequence>\n"
         "                <xs:element name='Flag' type='xs:double'"
         "                            default=\"12AB.123\"/>\n"
         "            </xs:sequence>\n"
         "        </xs:complexType>\n"
         "    </xs:element> \n"
         "</xs:schema>",
         false
            },
        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE          = DATA[i].d_line;
            const char *SCHEMA        = DATA[i].d_schema_p;
            const bool  PARSE_SUCCESS = DATA[i].d_parseSuccess;

            baexml_NamespaceRegistry namespaces;
            baexml_PrefixStack       prefixes(&namespaces);
            baexml_MiniReader        reader;
            reader.setPrefixStack(&prefixes);
            baexml_ErrorInfo         errInfo;
            baexml_SchemaParser      parser(&reader, &errInfo);
            bdem_Schema              schema;

            if (veryVerbose) {
                parser.setVerboseStream(&bsl::cout);
            }

            bsl::istringstream xmlStream(SCHEMA);
            bsl::string targetNamespace;
            int result = parser.parse(xmlStream,
                                      &schema,
                                      &targetNamespace);
            if (PARSE_SUCCESS) {
                LOOP2_ASSERT(LINE, result, !result);
            }
            else {
                LOOP2_ASSERT(LINE, result, result < 0);
            }
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // ACCEPTING EMPTY ROOT ELEMENT
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nACCEPTING EMPTY ROOT ELEMENT"
                               << "\n============================"
                               << bsl::endl;

        const char XML_SCHEMA1[] =
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'/>";
        const char XML_SCHEMA2[] =
         "<?xml version='1.0' encoding='UTF-8'?>\n"
         "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'></xs:schema>";

        baexml_NamespaceRegistry namespaces1, namespaces2;
        baexml_PrefixStack       prefixes1(&namespaces1),
                                 prefixes2(&namespaces2);
        baexml_MiniReader        reader1, reader2;
        reader1.setPrefixStack(&prefixes1);
        reader2.setPrefixStack(&prefixes2);
        baexml_ErrorInfo         errInfo1, errInfo2;
        baexml_SchemaParser      parser1(&reader1, &errInfo1);
        baexml_SchemaParser      parser2(&reader2, &errInfo2);
        bdem_Schema              schema1, schema2;

        if (veryVerbose) {
            parser1.setVerboseStream(&bsl::cout);
            parser2.setVerboseStream(&bsl::cout);
        }

        bsl::istringstream xmlStream1(XML_SCHEMA1);
        bsl::istringstream xmlStream2(XML_SCHEMA2);
        bsl::string targetNamespace1, targetNamespace2;
        int result = parser1.parse(xmlStream1,
                                   &schema1,
                                   &targetNamespace1);
        ASSERT(0 == result);
        result = parser2.parse(xmlStream2,
                               &schema2,
                               &targetNamespace2);
        ASSERT(0 == result);
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'parse' RETURNING SCHEMA ATTRIBUTES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   int parse(bsl::istream&            inputStream,
        //             bdem_Schema             *schema,
        //             SchemaElementAttributes *attributes,
        //             const bdeut_StringRef&   inputId = bdeut_StringRef());
        //   int parse(bsl::streambuf          *inputStream,
        //             bdem_Schema             *schema,
        //             SchemaElementAttributes *attributes,
        //             const bdeut_StringRef&   inputId = bdeut_StringRef());
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << "\nTESTING 'parse' RETURNING SCHEMA ATTRIBUTES"
                      << "\n==========================================="
                      << bsl::endl;

#define BODY                                                                  \
    "    <xs:element name='mailOrder'>\n"                                     \
    "        <xs:complexType>\n"                                              \
    "            <xs:sequence>\n"                                             \
    "                <xs:element name='orderDate' type='xs:date'/>\n"         \
    "                <xs:element name='customerName' type='xs:string'/>\n"    \
    "                <xs:element name='items' type='bb:item'\n"               \
    "                         maxOccurs='unbounded'/>\n"                      \
    "            </xs:sequence>\n"                                            \
    "        </xs:complexType>\n"                                             \
    "    </xs:element> \n"                                                    \
    "\n"                                                                      \
    "    <xs:simpleType name='ids'>"                                          \
    "        <xs:restriction base='xs:string'>"                               \
    "            <xs:enumeration value='REAR_SETS'/>"                         \
    "            <xs:enumeration value='DAMPER'/>"                            \
    "            <xs:enumeration value='POWER_COMMANDER'/>"                   \
    "        </xs:restriction>"                                               \
    "    </xs:simpleType>"                                                    \
    "\n"                                                                      \
    "    <xs:complexType name='item'>\n"                                      \
    "        <xs:sequence>\n"                                                 \
    "            <xs:element name='itemName' type='xs:string'/>\n"            \
    "            <xs:element name='id' type='bb:ids'/>\n"                     \
    "            <xs:element name='size' type='xs:double'/>\n"                \
    "        </xs:sequence>\n"                                                \
    "    </xs:complexType>\n"                                                 \
    "\n"

        const int MAX_NUM_ATTRIBUTES = 10;
        typedef struct {
            const char *d_name_p;
            const char *d_value_p;
        } Attribute;

        const struct {
            int         d_line;
            const char *d_input_p;
            int         d_numAttributes;
            Attribute   d_attributes[MAX_NUM_ATTRIBUTES];
        } DATA[] = {
            {
                L_,
        "<?xml version='1.0' encoding='UTF-8'?>\n"
        "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>\n"
        "\n"
        BODY
        "</xs:schema>\n",
                1,
                { "xmlns:xs", "http://www.w3.org/2001/XMLSchema" },
            },
            {
                L_,
        "<?xml version='1.0' encoding='UTF-8'?>\n"
        "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
        "           xmlns:bb='http://bloomberg.com/schemas/mailOrder'\n"
        "           bdem:requestType='SctpsvcRequest'\n"
        "           bdem:responseType='SctpsvcResponse'\n"
        "           xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
        "           xmlns='http://www.w3.org/2001/XMLSchema'\n"
        "           targetNamespace='http://bloomberg.com/schemas/mailOrder'\n"
        "           elementFormDefault='qualified'>\n"
        "\n"
        BODY
        "</xs:schema>\n",
                8,
                {
            { "xmlns:xs", "http://www.w3.org/2001/XMLSchema" },
            { "xmlns:bb", "http://bloomberg.com/schemas/mailOrder" },
            { "bdem:requestType", "SctpsvcRequest" },
            { "bdem:responseType", "SctpsvcResponse" },
            { "xmlns:bdem", "http://bloomberg.com/schemas/bdem" },
            { "xmlns", "http://www.w3.org/2001/XMLSchema" },
            { "targetNamespace", "http://bloomberg.com/schemas/mailOrder" },
            { "elementFormDefault", "qualified" },
                }
            }

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        typedef Obj::SchemaElementAttributes SchemaAttributes;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE       = DATA[i].d_line;
            const char *XML_SCHEMA = DATA[i].d_input_p;
            const int   NUM_ATTRS  = DATA[i].d_numAttributes;

            SchemaAttributes expected;
            for (int j = 0; j < NUM_ATTRS; ++j) {
                const Attribute  ATTR  = DATA[i].d_attributes[j];
                const char      *NAME  = ATTR.d_name_p;
                const char      *VALUE = ATTR.d_value_p;
                expected[NAME] = VALUE;
            }

            baexml_NamespaceRegistry namespaces;
            baexml_PrefixStack       prefixes(&namespaces);
            baexml_MiniReader        reader;
            reader.setPrefixStack(&prefixes);
            baexml_ErrorInfo         errInfo;
            baexml_SchemaParser      parser(&reader, &errInfo);
            bdem_Schema              schema;

            if (veryVeryVerbose) parser.setVerboseStream(&bsl::cout);

            bsl::istringstream xmlStream(XML_SCHEMA);

            SchemaAttributes attributes;
            int result = parser.parse(xmlStream,
                                      &schema,
                                      &attributes,
                                      "mailOrder.xsd");
            ASSERT(0 == result);
            ASSERT(NUM_ATTRS == attributes.size());

            if (verbose) {
                schema.print(bsl::cout);
            }

            for (SchemaAttributes::const_iterator iter = attributes.begin();
                 iter != attributes.end();
                 ++iter) {
                LOOP4_ASSERT(LINE, iter->first, iter->second,
                             expected[iter->first],
                             expected[iter->first] == iter->second);
            }

            if (0 != result) {
                bsl::cerr << "mailOrder.xsd:" << errInfo << bsl::endl;
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING ANONYMOUS CHOICE AND SEQUENCE
        //
        // Concerns:
        //  - An anonymous <sequence> or <choice> construct nested within a
        //    <sequence> or <choice> will result in an unnamed LIST or CHOICE
        //    field with the 'IS_UNTAGGED' format flag, constrained by an
        //    unnamed record.
        //  - If the anonymous <sequence> or <choice> has a 'maxOccurs'
        //    attribute greater than one, then the resulting unnamed field is
        //    of type TABLE or CHOICE_ARRAY, respectively.
        //  - Anonymous elements can be nested.
        //  - Special-case, if a <complexType> directly contains a <sequence>
        //    or <choice> construct with a 'minOccurs' of zero and/or a
        //    'maxOccurs' greater than one, then an extra level of nesting is
        //    generated, yielding a 'bdem_RecordDef' with a single, untagged
        //    field of type TABLE or CHOICE_ARRAY.  (This extra level is
        //    removed by the schema generator.)
        //  - An <element> with name 'FIELD_xxxx' with "type="RECORD_yyyy"
        //    will generate an unnamed aggregate field constrained by an
        //    unnamed record, but the field will not have the 'IS_UNTAGGED'
        //    formatting flag.
        //
        // Plan:
        //  - For each of the above concerns, create an XSD document
        //  - Generate the 'bdem_Schema' for each XML schema and compare with
        //    the expected results.
        //  - White-box note: The order of generated 'bdem_RecordDef's in the
        //    generated 'bdem_Schema' is not guaranteed by this component.
        //    However, this test driver must assume an ordering in the
        //    specification of the expected results.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "TEST ANONYMOUS CHOICE AND SEQUENCE"
                               << bsl::endl;

        const TestData DATA[] = {
            // line xml script,  (1st n lines)
            //      bdem script  (last line)
            // ---- ------------

            // Anonymous <sequence> and <choice>
            { L_, "[0][1]{"
                  "[3C]{[9]{[6Dh][9]{[6Ab][6Bd]}[6Eq]}}"
                  "[6CC]}",
                  "C{Dc@NF*u1@FU@NFEg@NF}*{Aa@FD@NFBx@NF}" },
            { L_, "[0][1]{"
                  "[3C]{[9]{[6Dh][18]{[6Ab][6Bd]}[6Eq]}}"
                  "[6CC]}",
                  "C{Dc@NF*E1@FU@NFEg@NF}*?{Aa@FD@NFBx@NF}" },

            // EMPTY anonymous <sequence> and <choice>
            { L_, "[0][1]{"
                  "[3C]{[9]{[6Dh][9]{}[6Eq]}}"
                  "[6CC]}",
                  "C{Dc@NF*u1@FU@NFEg@NF}*{}" },
            { L_, "[0][1]{"
                  "[3C]{[9]{[6Dh][18]{}[6Eq]}}"
                  "[6CC]}",
                  "C{Dc@NF*E1@FU@NFEg@NF}*?{}" },

            // Anonymous <sequence> and <choice> w/minOccurs='0'
            { L_, "[0][1]{"
                  "[3C]{[9]{[6Dh][9@1]{[6Ab][6Bd]}[6Eq]}}"
                  "[6CC]}",
                  "C{Dc@NF*u1@FU@NTEg@NF}*{Aa@FD@NFBx@NF}" },
            { L_, "[0][1]{"
                  "[3C]{[9]{[6Dh][18@1]{[6Ab][6Bd]}[6Eq]}}"
                  "[6CC]}",
                  "C{Dc@NF*E1@FU@NTEg@NF}*?{Aa@FD@NFBx@NF}" },

            // Anonymous <sequence> and <choice> w/maxOccurs='unbounded'
            { L_, "[0][1]{"
                  "[3C]{[9]{[6Dh][9@5]{[6Ab][6Bd]}[6Eq]}}"
                  "[6CC]}",
                  "C{Dc@NF*v1@FU@NFEg@NF}*{Aa@FD@NFBx@NF}" },
            { L_, "[0][1]{"
                  "[3C]{[9]{[6Dh][18@5]{[6Ab][6Bd]}[6Eq]}}"
                  "[6CC]}",
                  "C{Dc@NF*F1@FU@NFEg@NF}*?{Aa@FD@NFBx@NF}" },

            // Anonymous <sequence> nested within anonymous <choice> element
            { L_, "[0][1]{"
                  "[3C]{[9]{[18]{[9]{[6Ab][6Bd]}[6Dh]}[6Eq]}}"
                  "[6CC]}",
                  "C{*E1@FU@NFEg@NF}*?{*u2@FU@NFDc@NF}*{Aa@FD@NFBx@NF}" },

            // Anonymous <choice> nested within anonymous <sequence> element
            { L_, "[0][1]{"
                  "[3C]{[9]{[6Eq][9]{[6Dh][18]{[6Ab][6Bd]}}}}"
                  "[6CC]}",
                  "C{Eg@NF*u1@FU@NF}*{Dc@NF*E2@FU@NF}*?{Aa@FD@NFBx@NF}" },

            // Anonymous <choice> w/ minOccurs='0' and maxOccurs='unbounded'
            // nested within anonymous <sequence> element
            { L_, "[0][1]{"
                  "[3C]{[9]{[6Eq][9]{[18@1@5]{[6Ab][6Bd]}}}}"
                  "[6CC]}",
                  "C{Eg@NF*u1@FU@NF}*{*F2@FU}*?{Aa@FD@NFBx@NF}" },

            // Special case:
            // <complexType> directly containing a <sequence> or <choice>
            // construct with a 'minOccurs' of zero and/or a 'maxOccurs'
            // greater than one, causing an extra level of nesting to be
            // generated.
            { L_, "[0][1]{"
                  "[3A]{[9@1]{[6Ab][6Bd]}}"
                  "[3C]{[9]{[6Eq][6AA]}}"
                  "[6AA]"
                  "[6CC]}",
                  "A{*u1@NT@FU}*{Aa@FD@NFBx@NF}C{Eg@NFAuA@NF}" },
            { L_, "[0][1]{"
                  "[3A]{[18@1]{[6Ab][6Bd]}}"
                  "[3C]{[9]{[6Eq][6AA]}}"
                  "[6AA]"
                  "[6CC]}",
                  "A{*E1@FU}*?{Aa@FD@NFBx@NF}C{Eg@NFAuA@NF}" },
            { L_, "[0][1]{"
                  "[3A]{[9@5]{[6Ab][6Bd]}}"
                  "[3C]{[9]{[6Eq][6AA]}}"
                  "[6AA]"
                  "[6CC]}",
                  "A{*v1@FU@NF}*{Aa@FD@NFBx@NF}C{Eg@NFAuA@NF}" },
            { L_, "[0][1]{"
                  "[3A]{[18@5]{[6Ab][6Bd]}}"
                  "[3C]{[9]{[6Eq][6AA]}}"
                  "[6AA]"
                  "[6CC]}",
                  "A{*F1@FU@NF}*?{Aa@FD@NFBx@NF}C{Eg@NFAuA@NF}" },

            // <element> with name "FIELD_xxxx" having "type='RECORD_yyyy'"
            { L_, "[0][1]{"
                  "[3A]{[9]{[6WT@1]}}"
                  "[3T]{[9]{[6Ab][6Bd]}}"
                  "[6AA]}",
                  "A{*u1@NT}*{Aa@FD@NFBx@NF}" },
            { L_, "[0][1]{"
                  "[3A]{[9]{[6WT]}}"
                  "[3T]{[18]{[6Ab][6Bd]}}"
                  "[6AA]}",
                  "A{*E1@NF}*?{Aa@FD@NFBx@NF}" },
            { L_, "[0][1]{"
                  "[3A]{[9]{[6WT@5]}}"
                  "[3T]{[9]{[6Ab][6Bd]}}"
                  "[6AA]}",
                  "A{*v1@NF}*{Aa@FD@NFBx@NF}" },
            { L_, "[0][1]{"
                  "[3A]{[9]{[6WT@5]}}"
                  "[3T]{[18]{[6Ab][6Bd]}}"
                  "[6AA]}",
                  "A{*F1@NF}*?{Aa@FD@NFBx@NF}" },
        };

        const int DATA_SIZE = sizeof DATA / sizeof DATA[0];

        const char TARGET_NS[] = "http://bloomberg.com/schemas/test";

        for (int i = 0; i < DATA_SIZE; ++i) {
            const int LINE                = DATA[i].d_line;
            const char *const XSD_SCRIPT  = DATA[i].d_xmlSchemaScript;
            const char *const BDEM_SCRIPT = DATA[i].d_bdemSchemaScript;

            if (verbose) {
                bsl::cout << "Test vector at line " << LINE << bsl::endl;
            }

            bdem_Schema expectedBdemSchema;
            gBdemSchema(&expectedBdemSchema, BDEM_SCRIPT);

            bsl::string xsdString;
            gXsd(&xsdString, XSD_SCRIPT);

            if (veryVerbose) {
                P(xsdString);
                P(expectedBdemSchema);
            }

            Reader reader;
            ErrInfo errInfo;
            Obj    parser(&reader, &errInfo);
            bdem_Schema result;
            bsl::string targetNamespace;

            if (veryVeryVerbose) parser.setVerboseStream(&bsl::cout);

            bsl::istringstream xsdStream(xsdString);
            int retval = parser.parse(xsdStream,
                                      &result,
                                      &targetNamespace,
                                      "test.xsd");

            LOOP3_ASSERT(LINE, retval, errInfo, 0 == retval);
            LOOP2_ASSERT(LINE, targetNamespace, TARGET_NS == targetNamespace);

            if (veryVerbose) { P(result); }

            LOOP4_ASSERT(LINE, xsdString, expectedBdemSchema, result,
                         expectedBdemSchema == result);
        }

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING MISCELLANEOUS FEATURES
        //
        // Concerns:
        //   - Both constructor calls produce the same result.
        //   - If the 'bdem_Schema' object is non-empty when 'parse' is called,
        //     the old contents is discarded.
        //   - The presence or absence of the <?xml?> header has no affect
        //     unless a character set other than UTF-8 is specified.
        //
        // Plan:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING MISCELLANEOUS FEATURES"
                               << "\n=============================="
                               << bsl::endl;

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING setVerboseStream
        //
        // Concerns:
        //   - If a verbose stream is set via 'setVerboseStream', then all
        //     manner of stuff is written to it when parse() is called.
        //
        // Plan:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING setVerboseStream"
                               << "\n==================" << bsl::endl;

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING ERROR DETECTION
        //
        // Concerns:
        //   - When an error is detected, it is correctly returned to the
        //     caller in the form of a negative return value.
        //   - For all errors, an error message is returned.
        //   - For most errors, a line and column number is returned
        //     indicating a point in the input stream reasonably close to the
        //     point of the error.
        //   - XML syntax errors detectable by the underlying Xerces parser
        //     are reported to the caller.
        //   - For a variety of errors detectable by the 'baexml_SchemaParser'
        //     component, the errors are reported to the caller.
        //   - If 'parse' is called with null pointer values for any of the
        //     'errorString', 'errorLine', or 'errorColumn' arguments, then
        //     those values are not reported.  Correct values are returned
        //     through any non-null pointers.
        //
        // Plan:
        //   - Construct a set of XSD inputs with known errors.
        //   - Parse with the input strings.
        //   - Verify that the position and type of error is as expected.
        //
        // The following errors conditions are tested:
        // 1. An XSD input that contains schema elements incorrectly nested
        //    within other schema elements will produce an error, even if the
        //    inner or outer schema element would be ignored for the purposes
        //    of generating the bdem schema.
        // 2. TBD: More error tests
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING ERROR DETECTION"
                               << "\n=======================" << bsl::endl;

        static const bsl::string::size_type npos = bsl::string::npos;

        static const struct {
            int         d_line;
            const char *d_xsdSpec;
            const char *d_expErrMsg;
                // Expected error message or fragment thereof.
                // 'd_expErrMsg' is not a regular expression, but we do
                // recognize '^' as beginning-of-string and '$' as
                // end-of-string.
        } DATA[] = {
//                                                                        Error
//Line XSD Spec                                 Expected Error            Test
//==== ======================================   =======================   =====
{ L_, "[0]<xs:any xmlns:xs='http://www.w3.org/2001/XMLSchema'>^</xs:any>"
                                              , "Expected <schema>.$"   }, // 1
{ L_, "[0][1]{[4A]{[2]{[8]{[18]^{[6Bh]}}}}}"  , "tation> or <element>.$"}, // 1
{ L_, "[0][1]{[4A]{[2]{[18]{[8]^{[6Bh]}}}}}"  , "valid within <choice>."}, // 1
{ L_, "[0][1]{[3A]{[6Bh]^}[6AA]}"             , ", or <simpleContent>.$"}, // 1
{ L_, "[0][1]{[4A]{[12]{[9]^{[6Bh]}}}}"       , "within <simpleType>."  }, // 1
        };

        static const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE       = DATA[i].d_line;
            const char *const XSD_SPEC   = DATA[i].d_xsdSpec;
            const char *const EXP_ERRMSG = DATA[i].d_expErrMsg;

            bsl::string xsd;
            LineAndColumn expErrPos = gXsd(&xsd, XSD_SPEC);

            Reader reader;
            ErrInfo errInfo;
            Obj    parser(&reader, &errInfo);
            bdem_Schema result;
            bsl::string targetNamespace;

            if (veryVeryVerbose) parser.setVerboseStream(&bsl::cout);

            bsl::istringstream input(xsd);
            int retval = parser.parse(input,
                                      &result,
                                      &targetNamespace,
                                      "test.xsd");

            if (veryVerbose) { P(xsd); P(errInfo); }

            LOOP_ASSERT(LINE, 0 != retval);
            if (0 == retval) {
                // parse was unexpectedly successful.  Display result.
                P(result);
            }
            else {
                // Don't test these conditions if parse succeeded.  They would
                // all fail and clutter the output.

                // EXP_ERRMSG is not a regular expression, but we still
                // recognize '^' as beginning-of-string and '$' as
                // end-of-string.
                bsl::string errMessage = "^$";
                errMessage.insert(1, errInfo.message());

                LOOP_ASSERT(LINE, errInfo.isError());
                LOOP4_ASSERT(LINE, expErrPos,
                             errInfo.lineNumber(), errInfo.columnNumber(),
                             expErrPos.d_line == errInfo.lineNumber() &&
                             expErrPos.d_column >= errInfo.columnNumber());
                LOOP3_ASSERT(LINE, EXP_ERRMSG, errMessage,
                             npos != errMessage.find(EXP_ERRMSG));
                LOOP2_ASSERT(LINE, errInfo.source(),
                             "test.xsd" == errInfo.source());
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING NAMESPACE HANDLING
        //
        // Concerns:
        //   An XSD must use a namespace prefix to qualify the schema
        //   construct tags, the types and elements referenced from within
        //   other types and elements, or both.  We need to test that:
        //   - If the default namespace equals targetNamespace, then types and
        //     elements need not be namespace-qualified.
        //   - If the default namespace equals the W3 Schema namespace, then
        //     schema constructs (e.g., <element>) and attributes
        //     (e.g., maxOccurs) need not be namespace-qualified.
        //   - Whether the default namespace equals targetNamespace or W3
        //     Schema namespace or neither, schema constructs and/or type and
        //     element references are permitted to be namespace-qualified.
        //
        // Plan:
        //   - Construct otherwise identical XSD documents using each
        //     combination of namespace abbreviations.
        //   - Parse each XSD document.
        //   - Verify that they all produce the same, expected 'bdem_Schema'.
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING NAMESPACE HANDLING"
                               << "\n==========================" << bsl::endl;

        static const char *const XSD_DOCS[] = {
            // default namespace == W3 schema namespace
            "<schema xmlns='http://www.w3.org/2001/XMLSchema'\n"
            "        xmlns:bb='http://bloomberg.com/schemas/test'\n"
            "        targetNamespace='http://bloomberg.com/schemas/test'\n"
            "        elementFormDefault='qualified'>\n"
            "    <element name='Apple' type='bb:Apple' minOccurs='0'/>\n"
            "    <complexType name='Apple'>\n"
            "        <sequence>\n"
            "            <element name='Banana' type='short' minOccurs='0'/>\n"
            "        </sequence>\n"
            "    </complexType>\n"
            "</schema>\n",

            // Default namespace == targetNamespace
            "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
            "        xmlns='http://bloomberg.com/schemas/test'\n"
            "        targetNamespace='http://bloomberg.com/schemas/test'\n"
            "        elementFormDefault='qualified'>\n"
            "    <xs:element name='Apple' type='Apple' minOccurs='0'/>\n"
            "    <xs:complexType name='Apple'>\n"
            "        <xs:sequence>\n"
            "            <xs:element name='Banana' type='xs:short' "
            "minOccurs='0'/>\n"
            "        </xs:sequence>\n"
            "    </xs:complexType>\n"
            "</xs:schema>\n",

            // No default namespace
            "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
            "        xmlns:bb='http://bloomberg.com/schemas/test'\n"
            "        targetNamespace='http://bloomberg.com/schemas/test'\n"
            "        elementFormDefault='qualified'>\n"
            "    <xs:element name='Apple' type='bb:Apple' minOccurs='0'/>\n"
            "    <xs:complexType name='Apple'>\n"
            "        <xs:sequence>\n"
            "            <xs:element name='Banana' type='xs:short' "
            "minOccurs='0'/>\n"
            "        </xs:sequence>\n"
            "    </xs:complexType>\n"
            "</xs:schema>\n",

            // Qualified elements even though default namespace == W3 schema
            // namespace.
            "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
            "        xmlns:bb='http://bloomberg.com/schemas/test'\n"
            "        xmlns='http://www.w3.org/2001/XMLSchema'\n"
            "        targetNamespace='http://bloomberg.com/schemas/test'\n"
            "        elementFormDefault='qualified'>\n"
            "    <xs:element name='Apple' type='bb:Apple' minOccurs='0'/>\n"
            "    <xs:complexType name='Apple'>\n"
            "        <xs:sequence>\n"
            "            <xs:element name='Banana' type='xs:short' "
            "minOccurs='0'/>\n"
            "        </xs:sequence>\n"
            "    </xs:complexType>\n"
            "</xs:schema>\n",

            // Qualified names even though default namespace == targetNamespace
            "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
            "        xmlns:bb='http://bloomberg.com/schemas/test'\n"
            "        xmlns='http://bloomberg.com/schemas/test'\n"
            "        targetNamespace='http://bloomberg.com/schemas/test'\n"
            "        elementFormDefault='qualified'>\n"
            "    <xs:element name='Apple' type='bb:Apple' minOccurs='0'/>\n"
            "    <xs:complexType name='Apple'>\n"
            "        <xs:sequence>\n"
            "            <xs:element name='Banana' type='xs:short' "
            "minOccurs='0'/>\n"
            "        </xs:sequence>\n"
            "    </xs:complexType>\n"
            "</xs:schema>\n"
        };

        static const int XSD_DOCS_LEN = sizeof XSD_DOCS / sizeof *XSD_DOCS;

        bdem_Schema expected;
        gBdemSchema(&expected, "A{Bb}");

        if (veryVeryVerbose) P(expected);

        for (int i = 0; i < XSD_DOCS_LEN; ++i) {

            bsl::istringstream input(XSD_DOCS[i]);

            Reader reader;
            ErrInfo errInfo;
            Obj    parser(&reader, &errInfo);
            bdem_Schema result;
            bsl::string targetNamespace;

            if (veryVeryVerbose) parser.setVerboseStream(&bsl::cout);

            int retval = parser.parse(input,
                                      &result,
                                      &targetNamespace,
                                      "test.xsd");

            if (veryVeryVerbose) {
                bsl::cout << "Actual = \n" << result;
            }

            LOOP_ASSERT(i, 0 <= retval);
            LOOP_ASSERT(i, result == expected);
            LOOP_ASSERT(i, errInfo.severity () == ErrInfo::BAEXML_NO_ERROR);
            LOOP_ASSERT(i, errInfo.message().empty());
            LOOP_ASSERT(i, errInfo.lineNumber() == 0);
            LOOP_ASSERT(i, errInfo.columnNumber() == 0);
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING UNSUPPORTED CONSTRUCTS
        //
        // Concerns:
        //   - Constructs that always result in an error (as documented in the
        //     component header file) will produce that error even when used
        //     correctly according to the XSD standard.
        //
        // Plan:
        //   - For each unsupported construct, create a pair of XSD scripts
        //     that are identical except for the presence or absence of the
        //     construct being tested.
        //   - Parse each XSD document and verify that version containing the
        //     unsupported construct fails with a predictable error, whereas
        //     the other script produces the expected result.
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING UNSUPPORTED CONSTRUCTS"
                               << "\n=================" << bsl::endl;
      } break;
      case 10: {

        // --------------------------------------------------------------------
        // TESTING <INCLUDE> CONSTRUCTS
        //
        // Concerns:
        //
        // Plan:
        //
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTESTING <include>"
                               << "\n=================" << bsl::endl;

        static const TestData2 DATA[] = {
{ L_, "[1]{[20][4B]{[2]{[9]{[6Ab@1]}}}}",
      "[1]{[4A]{[2]{[9]{[6Ah@1][6Bq@1]}}}}",
                                                      "A{AcBg}B{Aa@FD}" }, // 1
{ L_, "[1]{[20][4B]{[2]{[18]{[6Ab@1]}}}}",
      "[1]{[4A]{[2]{[18]{[6Ah@1][6Bq@1]}}}}",
                                                    "A?{AcBg}B?{Aa@FD}" }, // 1
{ L_, "[1]{[20][4A]{[2]{[9]{[6Bh@1][6Xq@1][6Zb@1]}}}}",
      "[1]{}",                                           "A{Bc*g*a@FD}" }, // 2
{ L_, "[1]{[20][4A]{[2]{[18]{[6Bh@1][6Xq@1][6Zb@1]}}}}",
      "[1]{}",                                          "A?{Bc*g*a@FD}" }, // 2
{ L_, "[1]{[20][3B]{[9]{[6Cc@1]}}}",
      "[1]{[6A:B]}",                                        "A{Cy}"     }, // 3
{ L_, "[1]{[20][3B]{[18]{[6Cc@1]}}}",
      "[1]{[6A:B]}",                                        "A?{Cy}"    }, // 3
{ L_, "[1]{[20][3A]{[9]{[6Df@1]}}[3B]{[9]{[6Cc@1]}}}",
      "[1]{[6A:B][6B:A]}",
                                                           "A{Cy}B{Df}" }, // 3
{ L_, "[1]{[20][3A]{[18]{[6Df@1]}}[3B]{[18]{[6Cc@1]}}}",
      "[1]{[6A:B][6B:A]}",
                                                         "A?{Cy}B?{Df}" }, // 3
{ L_, "[1]{[20][4A]{[2]{[9]{[6Bb@1]}}}}",
      "[1]{[4C]{[2]{[9]{[7:A]}}}}",
                                                       "A{Ba@FD}C{AuA}" }, // 4
{ L_, "[1]{[20][4A]{[2]{[18]{[6Bb@1]}}}}",
      "[1]{[4C]{[2]{[18]{[7:A]}}}}",
                                                  "A?{Ba@FD}C?{AEA@NF}" }, // 4
{ L_, "[1]{[20][4C]{[2]{[9]{[7:A@1@5]}}}}",
      "[1]{[6Ad@1]}",             "C{AB}"      }, // 5
{ L_, "[1]{[20][4C]{[2]{[18]{[7:A@1@5]}}}}",
      "[1]{[6Ad@1]}",            "C?{AB}"     }, // 5
{ L_, "[1]{[20][4C]{[2]{[9]{[7:A@1@5]}}}}",
      "[1]{[4A]{[2]{[9]{[6Bb@1]}}}}",
                                                    "A{Ba@FD}C{AvA@NF}" }, // 5
{ L_, "[1]{[20][4C]{[2]{[18]{[7:A@1@5]}}}}",
      "[1]{[4A]{[2]{[18]{[6Bb@1]}}}}",
                                                  "A?{Ba@FD}C?{AFA@NF}" }, // 5
{ L_, "[1]{[20][3B]{[9]{[6Cc@1]}}}",
      "[1]{[6A:B][6D:B]}",               "A{Cy}D{Cy}" }, // 6
{ L_, "[1]{[20][3B]{[18]{[6Cc@1]}}}",
      "[1]{[6A:B][6D:B]}",            "A?{Cy}D?{Cy}" }, // 6
        };
        static const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const TestData2 *p = &DATA[i];

            // now test as included schemas
            runTestInc(p->d_line,
                       p->d_xmlSchemaScript1,
                       p->d_xmlSchemaScript2,
                       p->d_bdemSchemaScript);
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING SKIPPED CONSTRUCTS
        //
        // Concerns:
        //   - Constructs which are legal XSD but skipped by this component
        //     (as documented in the component header file) do
        //     not affect the result of the parse.
        //
        // Plan:
        //   - For each skipped construct, create a pair of XSD scripts that
        //     are identical except for the presence or absence of the
        //     construct being tested.
        //   - Parse each XSD document and verify that both scripts produce
        //     the same, expected result.
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING SKIPPED CONSTRUCTS"
                               << "\n==========================" << bsl::endl;

        struct SkipTestData {
            int         d_line;
            const char *d_skippedElement;
            const char *d_xmlSchemaScript;
            const char *d_bdemSchemaScript;
        };

        const SkipTestData DATA[] = {
//
//Line Skipped Element/
//     XML Script                                          bdem Script
//==== ==========                                          ===========
{ L_, "<xs:annotation><xs:documentation>hello world</xs:documentation>"
      "</xs:annotation>",
      "[1]{[4A]{?[2]{[9]{[6Bp@1]}}}}",                       "A{Bb}" },
//{ L_, "<xs:anyAttribute/>",
//      "[1]{[4A]{[2]{[9]{[6Bp@1]}?}}}",                       "A{Bb}" },
{ L_, "<xs:annotation><xs:appInfo>hello world</xs:appInfo></xs:annotation>",
      "[1]{[4A]{?[2]{[9]{[6Bp@1]}}}}",                       "A{Bb}" },
{ L_, "<xs:attributeGroup name='X'><xs:attribute name='Y' type='xs:int'/>"
      "</xs:attributeGroup>",
      "[1]{?[4A]{[2]{[9]{[6Bp@1]}}}}",                       "A{Bb}" }
#if 0
// TBD: Finish this test
{ L_, "<xs:field>"
{ L_, "<xs:fractionDigits>"
{ L_, "<xs:key>"
{ L_, "<xs:keyref>"
{ L_, "<xs:length>"
{ L_, "<xs:maxExclusive>"
{ L_, "<xs:maxInclusive>"
{ L_, "<xs:maxLength>"
{ L_, "<xs:minExclusive>"
{ L_, "<xs:minInclusive>"
{ L_, "<xs:minLength>"
{ L_, "<xs:notation>"
{ L_, "<xs:pattern>"
{ L_, "<xs:selector>"
{ L_, "<xs:totalDigits>"
{ L_, "<xs:unique>"
{ L_, "<xs:whiteSpace>"
#endif
        };

        static const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const SkipTestData *p = &DATA[i];

            bsl::string xsdScript = p->d_xmlSchemaScript;
            int line = p->d_line;
            bsl::string::size_type marker = xsdScript.find('?');
            ASSERT(marker < xsdScript.length());
            xsdScript.erase(marker, 1);

            bsl::string xsd;
            gXsd(&xsd, xsdScript);
            bsl::istringstream input(xsd);

            bdem_Schema expected;
            gBdemSchema(&expected, p->d_bdemSchemaScript);
            if (veryVeryVerbose) bsl::cout << "Expected = \n" << expected;

            Reader reader;
            ErrInfo errInfo;
            Obj    parser(&reader, &errInfo);
            bdem_Schema plainResult;
            bsl::string targetNamespace;

            if (veryVeryVerbose) parser.setVerboseStream(&bsl::cout);
            if (veryVeryVerbose) bsl::cout << "Plain XSD = \n" << xsd;

            int retval = parser.parse(input,
                               &plainResult,
                               &targetNamespace,
                               "test.xsd");

            if (veryVeryVerbose) bsl::cout << "Plain = \n" << plainResult;

            LOOP2_ASSERT(line, retval, 0 <= retval);
            LOOP2_ASSERT(line, plainResult, plainResult == expected);
            LOOP2_ASSERT(line, targetNamespace,
                         XML_TEST_NAMESPACE == targetNamespace);
            LOOP2_ASSERT(line, errInfo,
                         errInfo.severity() == ErrInfo::BAEXML_NO_ERROR);
            LOOP_ASSERT(line, errInfo.message().empty());
            LOOP_ASSERT(line, errInfo.lineNumber() == 0);
            LOOP_ASSERT(line, errInfo.columnNumber() == 0);

            // Now insert skipped element into XSD.
            xsdScript.insert(marker, p->d_skippedElement);
            gXsd(&xsd, xsdScript);
            input.str(xsd);
            bdem_Schema skipResult;
            if (veryVeryVerbose) bsl::cout << "Skip XSD = \n" << xsd;
            retval = parser.parse(input,
                                  &skipResult,
                                  &targetNamespace,
                                  "test.xsd");

            if (veryVeryVerbose) bsl::cout << "Skip = \n" << skipResult;

            LOOP2_ASSERT(line, retval, 0 <= retval);
            LOOP3_ASSERT(line, expected, skipResult, skipResult == expected);
            LOOP2_ASSERT(line, plainResult,  skipResult == plainResult);
            LOOP2_ASSERT(line, targetNamespace,
                         XML_TEST_NAMESPACE == targetNamespace);
            LOOP2_ASSERT(line, errInfo,
                         errInfo.severity() == ErrInfo::BAEXML_NO_ERROR);
            LOOP_ASSERT(line, errInfo.message().empty());
            LOOP_ASSERT(line, errInfo.lineNumber() == 0);
            LOOP_ASSERT(line, errInfo.columnNumber() == 0);
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING <attribute>
        //
        // Concerns:
        //    1 The same attribute name can be used to name different
        //      fields at different levels of hierarchy.
        //    2 Non-root <attribute>s with names beginning with "FIELD_" are
        //      mapped to unnamed fields in the resulting 'bdem_RecordDef'.
        //    3 The 'default' attribute of an '<attribute>' construct sets the
        //      default value of the field definition.
        //    4 The 'fixed' attribute of an '<attribute>' works exactly like
        //      'default' for our purposes here.
        //    5 The field is set to nullable if and only if both of the
        //      following hold:
        //      a. 'use' is absent, "optional", or "prohibited"
        //      b. 'default' and 'fixed' are both absent
        //      Otherwise, the field is set to non-nullable.
        //    6 An attribute may get its name and type from another element
        //      using the 'ref' attribute.
        //    7 An attribute may use the 'ref' attribute and 'use' to
        //      create a required or optional field.
        //    8 The 'bdem:id' attribute of an '<attribute>' construct forces
        //      the field ID to be the specified value.
        //    9 Multiple '<attribute>' constructs can appear in the same
        //      record
        //    10 Multiple combinations of 'default', 'fixed', 'use', 'ref, and
        //       'bdem:id' can be used together on one '<attribute>'
        //    11 Simple error conditions are detected.
        //    Note that testing the '<attribute>' construct with different
        //    values for the 'type' attribute was already accomplished in test
        //    case 3.
        //
        // Plan:
        //    Create a set of XSD schemas with corresponding BDEM schemas
        //    testing each of the above concern except error handling.  Parse
        //    the XSD and verify that the resulting 'bdem_Schema' matches the
        //    expected value.
        //
        //    Create a set of XSD schemas with specific errors in the
        //    '<attribute>' constructs.  Test that the schema parser returns
        //    an appropriate error for each.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING <attribute>"
                               << "\n===================" << bsl::endl;

        static const TestData DATA[] = {
//
//Line XML Script                                          bdem Script  Concern
//==== ==========                                          ===========      ===
{ L_, "[1]{[4A]{[2]{[22Ah@13][9]{[6Bq]}}}[4B]{[2]{[9]{[6Bb]}[22Ab@12]}}}",
                               "A{Ac@FA@NFBg@NF}B{Aa@FA@FD@NTBa@NF@FD}" }, // 1
{ L_, "[1]{[4A]{[2]{[22Wh@13][9]{[6Bq]}}}}",         "A{*c@FA@NFBg@NF}" }, // 2
{ L_, "[1]{[4A]{[2]{[22Ah@6][9]{[6Bq]}}}}",       "A{Ac@FA@D1@NFBg@NF}" }, // 3
{ L_, "[1]{[4A]{[2]{[22Aq@7][9]{[6Bq]}}}}",       "A{Ag@FA@D0@NFBg@NF}" }, // 3
{ L_, "[1]{[4A]{[2]{[22Ah@15][9]{[6Bq]}}}}",      "A{Ac@FA@D1@NFBg@NF}" }, // 4
{ L_, "[1]{[4A]{[2]{[22Ah][9]{[6Bq]}}}}",            "A{Ac@FA@NTBg@NF}" }, // 5
{ L_, "[1]{[4A]{[2]{[22Ah@12][9]{[6Bq]}}}}",         "A{Ac@FA@NTBg@NF}" }, // 5
{ L_, "[1]{[4A]{[2]{[22Ah@13][9]{[6Bq]}}}}",         "A{Ac@FA@NFBg@NF}" }, // 5
{ L_, "[1]{[4A]{[2]{[22Ah@14][9]{[6Bq]}}}}",         "A{Ac@FA@NTBg@NF}" }, // 5
{ L_, "[1]{[4A]{[2]{[23C][9]{[6Bq]}}}[22Ch]}",       "A{Cc@FA@NTBg@NF}" }, // 6
{ L_, "[1]{[4A]{[2]{[23C@13][9]{[6Bq]}}}[22Ch]}",    "A{Cc@FA@NFBg@NF}" }, // 7
{ L_, "[1]{[4A]{[2]{[9]{[6Bq@18]}[22Ah@17]}}}","A{Ac@FA@NT@I0Bg@NF@I1}" }, // 8
{ L_, "[1]{[4A]{[2]{[22Ah][9]{[6Bq]}[22Cg]}}}",    "A{Ac@FACe@FABg@NF}" }, // 9
{ L_, "[1]{[4A]{[2]{[22Ah@6@12@18][9]{[6Bq@17]}}}}",
                                           "A{Ac@FA@D1@NF@I1Bg@NF@I0}" }, // 10
        };

        static const int NUM_DATA = sizeof DATA / sizeof *DATA;

        static const char mainSchema[] = "[1]{[20]}";

        for (int i = 0; i < NUM_DATA; ++i) {
            const TestData& d = DATA[i];
            runTest(d.d_line, d.d_xmlSchemaScript, d.d_bdemSchemaScript);

            // now test as included schemas
            runTestInc(d.d_line, mainSchema, d.d_xmlSchemaScript,
                       d.d_bdemSchemaScript);
        }

        static const struct {
            int         d_line;
            const char *d_xmlSchemaScript;
            const char *d_error;  // Fragment of expected error message
        } ERROR_DATA[] = {
//
//Line XML Script                              Expected Error Message
//==== ==========                              ======================
{ L_, "[1]{[4A]{[2]{[22Ah][9]{[6Aq]}}}}",      "defined twice"        },
{ L_, "[1]{[4A]{[2]{[9]{[6Aq]}[22Ah]}}}",      "defined twice"        },
{ L_, "[1]{[22Ah][23A]}",                      "\"ref\""              },
{ L_, "[1]{[4A]{[2]{[23B][9]{}}}}",            "No type"              },
{ L_, "[1]{[4A]{[2]{[22Ah@6@15][9]{[6Bq]}}}}", "Mutually-exclusive"   },
{ L_, "[1]{[4A]{[2]{[18]{[6Bq]}[22Ah]}}}",     "only on <sequence>"   },
{ L_, "[1]{[4A]{[2]{[22Ah][18]{[6Bq]}}}}",     "previously-defined"   },
{ L_, "[1]{[4A]{[13B]{[15g]{}[22Ah]}}}",       "nly top-level types"  },
{ L_, "[1]{[4A]{[12]{[15g]{[22Ah]}}}}",        "only on <sequence>"   },
//{ L_, "[1]{[4A]{[12]{[22Ah][15g]{}}}}",        "only on <sequence>"  },
        };

        static const int NUM_ERROR_DATA =
            sizeof ERROR_DATA / sizeof *ERROR_DATA;

        for (int i = 0; i < NUM_ERROR_DATA; ++i) {
            const int LINE               = ERROR_DATA[i].d_line;
            const char *const XSD_SCRIPT = ERROR_DATA[i].d_xmlSchemaScript;
            const char *const ERROR_MSG  = ERROR_DATA[i].d_error;

            bsl::string xsd;
            gXsd(&xsd, XSD_SCRIPT);
            bsl::istringstream input(xsd);

            if (veryVerbose) {
                P(xsd);
                bsl::cout << "Expected error = " << ERROR_MSG << bsl::endl;
            }

            Reader reader;
            ErrInfo errInfo;
            Obj    parser(&reader, &errInfo);
            if (veryVeryVerbose) parser.setVerboseStream(&bsl::cout);

            bdem_Schema result;
            bsl::string targetNamespace = "unknown";
            int retval = parser.parse(input, &result, &targetNamespace,
                                      "test.xsd");

            static const bsl::string::size_type NPOS = bsl::string::npos;
            LOOP2_ASSERT(LINE, result, 0 != retval);
            LOOP3_ASSERT(LINE, errInfo.message(), ERROR_MSG,
                         errInfo.message().find(ERROR_MSG) != NPOS);
            LOOP2_ASSERT(LINE, errInfo.lineNumber(),
                         0 <= errInfo.lineNumber());
            LOOP2_ASSERT(LINE, errInfo.columnNumber(),
                         0 <= errInfo.columnNumber());
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING <element>
        //
        // Concerns:
        //    1 The same element name can be used to name different
        //      fields at different levels of hierarchy.
        //    2 Non-root <element>s with names beginning with "FIELD_" are
        //      mapped to unnamed fields in the resulting 'bdem_RecordDef'.
        //    3 The <complexType> used by a root-level element takes its name
        //      from that element, overriding any name given it via its own
        //      name attribute.  Two <complexTypes> may end up with each
        //      other's original names by this mechanism.
        //    4 An element may get its name and type from another element
        //      using the 'ref' attribute.
        //    5 An <element> of <simpleType> with maxOccurs > 1 constructs a
        //      field of array type.
        //    6 Duplicate 'bdem_RecordDef's are created for all root elements
        //      sharing the same <complexType>.
        //    7 An <element> of <complexType> with maxOccurs > 1 constructs a
        //      constrained TABLE field in the 'bdem_Schema'.
        //    8 An element may use the 'ref' attribute and 'maxOccurs' to
        //      create an array or table.
        //    9 The field is set to nullable if and only if all of the
        //      following hold:
        //      a. 'minOccurs' is zero
        //      b. 'maxOccurs' is one
        //      c. 'default' and 'fixed' are both absent
        //      Otherwise, the field is set to non-nullable.
        //
        // Plan:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING <element>"
                               << "\n=================" << bsl::endl;

        static const TestData DATA[] = {
//
//Line XML Script                                          bdem Script  Concern
//==== ==========                                          ===========      ===
{ L_, "[1]{[4A]{[2]{[9]{[6Ah@1][6Bq@1]}}}[4B]{[2]{[9]{[6Ab@1]}}}}",
                                                      "A{AcBg}B{Aa@FD}" }, // 1
{ L_, "[1]{[4A]{[2]{[18]{[6Ah@1][6Bq@1]}}}[4B]{[2]{[18]{[6Ab@1]}}}}",
                                                    "A?{AcBg}B?{Aa@FD}" }, // 1
{ L_, "[1]{[4A]{[2]{[9]{[6Bh@1][6Xq@1][6Zb@1]}}}}",      "A{Bc*g*a@FD}" }, // 2
{ L_, "[1]{[4A]{[2]{[18]{[6Bh@1][6Xq@1][6Zb@1]}}}}",    "A?{Bc*g*a@FD}" }, // 2
{ L_, "[1]{[6A:B][3B]{[9]{[6Cc@1]}}}",                     "A{Cy}"      }, // 3
{ L_, "[1]{[6A:B][3B]{[18]{[6Cc@1]}}}",                    "A?{Cy}"     }, // 3
{ L_, "[1]{[6A:B][6B:A][3A]{[9]{[6Df@1]}}[3B]{[9]{[6Cc@1]}}}",
                                                           "A{Cy}B{Df}" }, // 3
{ L_, "[1]{[6A:B][6B:A][3A]{[18]{[6Df@1]}}[3B]{[18]{[6Cc@1]}}}",
                                                         "A?{Cy}B?{Df}" }, // 3
{ L_, "[1]{[4A]{[2]{[9]{[6Bb@1]}}}[4C]{[2]{[9]{[7:A]}}}}",
                                                       "A{Ba@FD}C{AuA}" }, // 4
{ L_, "[1]{[4A]{[2]{[18]{[6Bb@1]}}}[4C]{[2]{[18]{[7:A]}}}}",
                                                  "A?{Ba@FD}C?{AEA@NF}" }, // 4
{ L_, "[1]{[4A]{[2]{[9]{[6Bh@5]}}}}",                           "A{Bm}" }, // 5
{ L_, "[1]{[6A:B][6D:B][3B]{[9]{[6Cc@1]}}}",               "A{Cy}D{Cy}" }, // 6
{ L_, "[1]{[6A:B][6D:B][3B]{[18]{[6Cc@1]}}}",            "A?{Cy}D?{Cy}" }, // 6
{ L_, "[1]{[6Ad@1][4C]{[2]{[9]{[7:A@1@5]}}}}",             "C{AB}"      }, // 8
{ L_, "[1]{[6Ad@1][4C]{[2]{[18]{[7:A@1@5]}}}}",            "C?{AB}"     }, // 8
{ L_, "[1]{[4A]{[2]{[9]{[6Bb@1]}}}[4C]{[2]{[9]{[7:A@1@5]}}}}",
                                                    "A{Ba@FD}C{AvA@NF}" }, // 8
{ L_, "[1]{[4A]{[2]{[18]{[6Bb@1]}}}[4C]{[2]{[18]{[7:A@1@5]}}}}",
                                                  "A?{Ba@FD}C?{AFA@NF}" }, // 8
{ L_, "[1]{[4A]{[2]{[9]{[6Bh@1]}}}}",                        "A{Bc@NT}" }, // 9
{ L_, "[1]{[4A]{[2]{[9]{[6Bh@1@5]}}}}",                      "A{Bm@NF}" }, // 9
{ L_, "[1]{[4A]{[2]{[9]{[6Bh@1@6]}}}}",                   "A{Bc@D1@NF}" }, // 9
{ L_, "[1]{[4A]{[2]{[9]{[6Bh@1@5@6]}}}}",                 "A{Bm@D1@NF}" }, // 9
        };
        static const int NUM_DATA = sizeof DATA / sizeof *DATA;

        static const char mainSchema[] = "[1]{[20]}";

        for (int i = 0; i < NUM_DATA; ++i) {
            const TestData& d = DATA[i];
            runTest(d.d_line, d.d_xmlSchemaScript, d.d_bdemSchemaScript);

            // now test as included schemas
            runTestInc(d.d_line, mainSchema, d.d_xmlSchemaScript,
                       d.d_bdemSchemaScript);
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING <complexType>
        //
        // Concerns:
        //    1 A <complexType> can be empty (have no elements).
        //    2 A named, schema-level <complexType> can be referenced by a
        //      schema-level element.
        //    3 A <complexType> can have any mixture of array and non-array
        //      elements.
        //    4 A named, schema-level <complexType> can be referenced by an
        //      <element> within another <complexType>, to produce a
        //      'bdem_FieldDef' of type 'LIST'.
        //    5 A named <complexType> can be referenced before it is defined.
        //    6 A named, schema-level <complexType> can be referenced by an
        //      <element> within another <complexType>, to produce a
        //      'bdem_FieldDef' of type 'TABLE' by specifying a 'maxOccurs' of
        //      greater than 1.
        //    7 A named, schema-level <complexType> can have an element that
        //      refers to the <complexType> recursively.
        //    8 A <complexType> may be nested within an <element> within
        //      another <complexType> to an arbitrary depth.
        //    9 A named <complexType> with a name starting with "RECORD_" will
        //      be mapped to an unnamed record in the resulting 'bdem_Schema'.
        //   10 It makes no difference to this component whether <all> or
        //      <sequence> is used to enclose the list of elements in a
        //      <complexType>.
        //
        // Plan:
        //   - For each of the above concerns, create an XSD document
        //     containing the construct in question.
        //   - Parse each XSD document and compare the constructed
        //     'bdem_Schema' with the expected result.
        //   - For concern 8 (nesting to arbitrary depth), create XSD in a
        //     loop with ever deeper nesting.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING <complexType>"
                               << "\n=====================" << bsl::endl;

        static const TestData DATA[] = {
//                                                                      Concern
//Line XML Script                                       bdem Script          |
//==== ==========                                       ===========         ===
{ L_, "[1]{[4A]{[2]{}}}",                               "A{}"            }, //1
{ L_, "[1]{[4A]{[2]{[9]{}}}}",                          "A{}"            }, //1
{ L_, "[1]{[4A]{[2]{[18]{}}}}",                         "A?{}"           }, //1
{ L_, "[1]{[3A]{[9]{[6Ca@1]}}[6B:A@1]}",                "B{Cw}"          }, //2
{ L_, "[1]{[3A]{[18]{[6Ca@1]}}[6B:A@1]}",               "B?{Cw}"         }, //2
{ L_, "[1]{[4A]{[2]{[9]{[6Bb@1][6Cc@1@5][4D]{[12]{[16d]}}}}}}",
                                                     "A{Ba@FDCCDB@FL@NF}"}, //3
{ L_, "[1]{[4A]{[2]{[18]{[6Bb@1][6Cc@1@5][4D]{[12]{[16d]}}}}}}",
                                                    "A?{Ba@FDCCDB@FL@NF}"}, //3
{ L_, "[1]{[4A]{[2]{[9]{[6Bb@1][6Cc@1@5][4D@1]{[12]{[16d]}}}}}}",
                                                     "A{Ba@FDCCDB@FL@NT}"}, //3
{ L_, "[1]{[3A]{[9]{[6Bh@1]}}[4C]{[2]{[9]{[6D:A]}}}}",  "C{DuA}A{Bc}"    }, //4
{ L_, "[1]{[3A]{[18]{[6Bh@1]}}[4C]{[2]{[9]{[6D:A]}}}}", "C{DEA@NF}A?{Bc}"}, //4
{ L_, "[1]{[4C]{[2]{[9]{[6D:A]}}}[3A]{[9]{[6Bh@1]}}}",  "C{DuA}A{Bc}"    }, //5
{ L_, "[1]{[4C]{[2]{[18]{[6D:A]}}}[3A]{[18]{[6Bh@1]}}}",
                                                      "C?{DEA@NF}A?{Bc}" }, //5
{ L_, "[1]{[3A]{[9]{[6Bh@1]}}[4C]{[2]{[9]{[6D:A@1@5]}}}}",
                                                        "C{DvA@NF}A{Bc}" }, //6
{ L_, "[1]{[3A]{[18]{[6Bh@1]}}[4C]{[2]{[18]{[6D:A@1@5]}}}}",
                                                      "C?{DFA@NF}A?{Bc}" }, //6
{ L_, "[1]{[3A]{[9]{[6Bj@1][6C:A@1]}}[6A:A@1]}",        "A{BdCuA}"       }, //7
{ L_, "[1]{[3A]{[18]{[6Bj@1][6C:A@1]}}[6A:A@1]}",       "A?{BdCEA}"      }, //7
{ L_, "[1]{[3A]{[9]{[6Bj@1][6C:A@1@5]}}[6A:A@1]}",      "A{BdCvA@NF}"    }, //7
{ L_, "[1]{[3A]{[18]{[6Bj@1][6C:A@1@5]}}[6A:A@1]}",     "A?{BdCFA@NF}"   }, //7
{ L_, "[1]{[3A]{[9]{[6Bb@1][4C]{[2]{[9]{[6Dd@1]"
      "[4E]{[2]{[9]{[6Ff@1][6Gg@1]}}}}}}}}[6A:A@1]}",
                                             "A{Ba@FDCu1}*{DxEu2}*{FfGe}"}, //8
{ L_, "[1]{[3A]{[9]{[6Bb@1][4C@5]{[2]{[9]{[6Dd@1]"
      "[4E]{[2]{[9]{[6Ff@1][6Gg@1]}}}}}}}}[6A:A@1]}",
                                             "A{Ba@FDCv1}*{DxEu2}*{FfGe}"}, //8
{ L_, "[1]{[3A]{[18]{[6Bb@1][4C]{[2]{[18]{[6Dd@1]"
      "[4E]{[2]{[18]{[6Ff@1][6Gg@1]}}}}}}}}[6A:A@1]}",
                                    "A?{Ba@FDCE1@NF}*?{DxEE2@NF}*?{FfGe}"}, //8
{ L_, "[1]{[3A]{[18]{[6Bb@1][4C@5]{[2]{[18]{[6Dd@1]"
      "[4E]{[2]{[18]{[6Ff@1][6Gg@1]}}}}}}}}[6A:A@1]}",
                                    "A?{Ba@FDCF1@NF}*?{DxEE2@NF}*?{FfGe}"}, //8
{ L_, "[1]{[3T]{[9]{[6Aa@1]}}[4C]{[2]{[9]{[6B:T]}}}}",  "C{Bu1}*{Aw}"    }, //9
{ L_, "[1]{[3T]{[18]{[6Aa@1]}}[4C]{[2]{[18]{[6B:T]}}}}",
                                                      "C?{BE1@NF}*?{Aw}" }, //9
{ L_, "[1]{[3V]{[9]{[6Aa@1]}}[4C]{[2]{[9]{[6B:V]}}}}",  "C{Bu1}*{Aw}"    },//10
{ L_, "[1]{[3V]{[18]{[6Aa@1]}}[4C]{[2]{[18]{[6B:V]}}}}",
                                                       "C?{BE1@NF}*?{Aw}"},//10
{ L_, "[1]{[3A]{[8]{[6Bb@1][4C]{[2]{[9]{[6Dd@1]"
      "[4E]{[2]{[8]{[6Ff@1][6Gg@1]}}}}}}}}[6A:A]}",
                                             "A{Ba@FDCu1}*{DxEu2}*{FfGe}"},//11
{ L_, "[1]{[3A]{[8]{[6Bb@1][4C]{[2]{[18]{[6Dd@1]"
      "[4E]{[2]{[8]{[6Ff@1][6Gg@1]}}}}}}}}[6A:A]}",
                                         "A{Ba@FDCE1@NF}*?{DxEu2}*{FfGe}"},//11
{ L_, "[1]{[3A]{[8]{[6Ba@21]}}[6A:A]}",                    "A{Bw@NF@D1}"},
{ L_, "[1]{[4A]{[2]{[9]{[6Bb@1][6Cc@1@5][4D]{[12]{[16d]}}}}}}",
                                                     "A{Ba@FDCCDB@FL@NF}"}, //3
        };
        static const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const TestData& d = DATA[i];
            runTest(d.d_line, d.d_xmlSchemaScript, d.d_bdemSchemaScript);
        }

        // TBD Nesting test.
        // NOTE: gBdemSchema cannot handle more than 10 unnamed records, so
        // schema must be built using 'bdem_Schema' calls directly.

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING <simpleType>
        //
        // Concerns:
        //   1 A schema-level <simpleType> can be given a name and used to
        //     define a synonym for a built-in type with constraints.  (The
        //     constraints are currently ignored.)
        //   2 A <simpleType> within an <element> can be used to specify a
        //     constrained primitive type for an element with constraints.
        //     (The constraints are currently ignored.)
        //   3 A <simpleType> can refine another <simpleType>.
        //   4 A <simpleType> can define an array
        //   5 In a chain of <simpleType> definitions such that each
        //     <simpleType> refines the previous one, then an array type in
        //     the middle of the chain will propagate its "array-ness" to all
        //     subsequent types.
        //   6 A <simpleType> can define an enumeration with alphabetically-
        //     assigned integer IDs, manually-assigned IDs (using the
        //     'bdem:id' attribute) and declaration-order IDs (using the
        //     'bdem:preserveEnumOrder' attribute).
        //
        // Plan:
        //   - For each of the above concerns, create an XSD document
        //     containing the construct in question.
        //   - Parse each XSD document and compare the constructed
        //     'bdem_Schema' with the expected result.
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING <simpleType>"
                               << "\n====================" << bsl::endl;

        static const TestData DATA[] = {
//                                                                      Concern
//Line XML Script                                             bdem Script    |
//==== ==========                                             ===========   ===
{ L_,  "[1]{[13A]{[15c@1]{[17]}}[4B]{[2]{[9]{[6C:A@1]}}}}",   "B{Cy}"    }, //1
{ L_,  "[1]{[4B]{[2]{[9]{[4C@1]{[12]{[15c@1]{[17]}}}}}}}",    "B{Cy}"    }, //2
{ L_,  "[1]{[13A]{[15c@1]{[17]}}[13D]{[15:A@1]{}}[4B]{[2]{[9]{[6C:D@1]}}}}",
                                                              "B{Cy@NT}" }, //3
{ L_,  "[1]{[13A]{[16d]}[4B]{[2]{[9]{[6C:A@1]}}}}",        "B{CB@FL@NT}" }, //4
{ L_,  "[1]{[13A]{[15d@1]{[17]}}[13D]{[16:A]}[4B]{[2]{[9]{[6C:D@1]}}}}",
                                                           "B{CB@FL@NT}" }, //5
{ L_,  "[1]{[13A]{[15d@1]{[17]}}[13D]{[16:A]}[13E]{[15:D@1]{}}"
       "[4B]{[2]{[9]{[6C:E]}}}}",                          "B{CB@FL@NF}" }, //5
{ L_,  "[1]{[13E]{[15q]{[24B][24C][24A]}}[4D]{[2]{[9]{[6FE]}}}}",
                                                     "E={ABC}D{Fg=E@NF}" }, //6
{ L_,  "[1]{[13E]{[15q]{[24B@17][24C@19][24A@20]}}[4D]{[2]{[9]{[6FE]}}}}",
                                                    "E={BC2A}D{Fg=E@NF}" }, //6
{ L_,  "[1]{[13E@22]{[15q]{[24B][24C][24A]}}[4D]{[2]{[9]{[6FE]}}}}",
                                                     "E={BCA}D{Fg=E@NF}" }, //6
        };
        static const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const TestData& d = DATA[i];
            runTest(d.d_line, d.d_xmlSchemaScript, d.d_bdemSchemaScript);
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ARRAY ELEMENTS
        //
        // Concerns:
        //   - The schema parser recognizes arrays specified with the
        //     following XSD notations:
        //     * A '<list>' with a primitive 'baseType' attribute.
        //     * An '<element>' with 'maxOccurs' greater than 1.
        //     * An '<element>' with 'maxOccurs' set to "unbounded".
        //   - Each of the above notations will result in an array element
        //     within the constructed 'bdem_Schema'.
        //   - Each of the above notations will work with each primitive type.
        //   - Default value will produce a field definition with a
        //     scalar default value (which applies to each array item).
        //
        // Plan:
        //   - Create a simple XSD string for each of the above notations for
        //     specifying an element containing an array.
        //   - For each primitive XML type, create a variant of each XSD
        //     string.
        //   - Parse each XSD string into a 'bdem_Schema' and compare the
        //     actual result with the expected result.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING ARRAY ELEMENTS"
                               << "\n======================" << bsl::endl;

        // Express the mapping from XML types to bdem types:
        static const char XMLTYPECHARS[]  = "abcdefghijklmnopqrstuvy";
        static const char BDEMTYPECHARS[] = "AkCBppomnnnnnqnlqDlnnmq";

        if (verbose) bsl::cout << "Testing <sequence>." << bsl::endl;
        {
            // The following XML schema scripts all produce identical
            // 'bdem_Schema' output.  They differ in the way the array-ness of
            // an element is expressed in the XSD.  The '?' character indicates
            // the position of the element type within the script.  The loops
            // below will substitute each built-in element type for the '?'.
            // The scripts are non-const to make it easy to replace the '?'.
            // '9' == <sequence>.
            char xsd1[]="[0][1]{[4A@1]{[2]{[9]{[4B]{[12]{[16?]}}}}}}";
                                                               // <list...>
            char xsd2[]="[0][1]{[4A@1]{[2]{[9]{[6B?@1@4]}}}}"; // maxOccurs='2'
            char xsd3[]="[0][1]{[4A@1]{[2]{[9]{[6B?@1@5]}}}}"; // ='unbounded'
            char xsd4[]="[0][1]{[4A@1]{[2]{[9]{[6B?@1@5@23]}}}}"; // nillable
            char *xsdScripts[] = { xsd1, xsd2, xsd3, xsd4 };

            const int NUM_XSD_SCRIPTS = sizeof xsdScripts / sizeof *xsdScripts;

            for (int j = 0; j < NUM_XSD_SCRIPTS; ++j) {
                char *xsdScript = xsdScripts[j];
                char *xsdTypeCharPtr = bsl::strchr(xsdScript, '?');
                ASSERT(xsdTypeCharPtr);

                for (int i = 0; i < sizeof XMLTYPECHARS - 1; ++i) {
                    *xsdTypeCharPtr = XMLTYPECHARS[i];

                    bsl::string xmlSchema;
                    gXsd(&xmlSchema, xsdScript);

                    if (verbose) bsl::cout << "  Testing "
                                           << XMLTYPES[*xsdTypeCharPtr - 'a']
                                           << bsl::endl;

                    // The gBdemSchema script is identical in each iteration
                    // except for the type of the single field (and possibly
                    // the formatting mode attribute).

                    char        bdemTypeChar = BDEMTYPECHARS[i];
                    EType::Type elemType     = gType(bdemTypeChar);
                    bsl::string bdemScript   = "A{B";  // SEQUENCE record
                    if (3 == j) {
                        bdemScript += "v1";
                    }
                    else {
                        bdemScript += bdemTypeChar;
                    }
                    bdemScript += "@NF";
                    if (0 == j) {  // <list>
                        bdemScript += "@FL";  // 'LIST'
                    }
                    if ('b' == *xsdTypeCharPtr ||   // xs:byte
                        'e' == *xsdTypeCharPtr) {   // xs:decimal
                        bdemScript += "@FD";  // 'DEC'
                    }
                    if (3 == j) {
                        bdemScript += "@FN}";  // 'NILLABLE'
                        char bdemChar = gBdemChar(EType::fromArrayType(
                                                                    elemType));
                        bdemScript += "*{*";
                        bdemScript += bdemChar;
                        bdemScript += "@NF@FN";
                    }

                    bdemScript += '}';

                    bdem_Schema bdemSchema;
                    gBdemSchema(&bdemSchema, bdemScript);

                    if (veryVerbose) {
                        bsl::cout << "    XML Schema = \n" << xmlSchema
                                  << "    Expected bdem_Schema = \n"
                                  << bdemSchema;
                    }

                    Reader reader;
                    Obj    parser(&reader);
                    if (veryVeryVerbose) parser.setVerboseStream(&bsl::cout);

                    bsl::istringstream input(xmlSchema);
                    bdem_Schema outputSchema;
                    bsl::string targetNamespace;

                    int result = parser.parse(input, &outputSchema,
                                              &targetNamespace);

                    if (veryVerbose) bsl::cout << "    Actual bdem_Schema = \n"
                                               << outputSchema;

                    LOOP3_ASSERT(j, *xsdTypeCharPtr,
                                 BDEMTYPECHARS[i], 0 == result);
                    LOOP3_ASSERT(j, *xsdTypeCharPtr, BDEMTYPECHARS[i],
                                 XML_TEST_NAMESPACE == targetNamespace);
                    LOOP3_ASSERT(j, *xsdTypeCharPtr, BDEMTYPECHARS[i],
                                 bdemSchema == outputSchema);
                }
            }
        }

        if (verbose) bsl::cout << "Testing <choice>." << bsl::endl;
        {
            // The following XML schema scripts all produce identical
            // 'bdem_Schema' output.  They differ in the way the array-ness of
            // an element is expressed in the XSD.  The '?' character indicates
            // the position of the element type within the script.  The loops
            // below will substitute each built-in element type for the '?'.
            // The scripts are non-const to make it easy to replace the '?'.
            // '18' == <choice>.
            char xsd1[]="[0][1]{[4A@1]{[2]{[18]{[4B]{[12]{[16?]}}}}}}";
                                                               // <list...>
            char xsd2[]="[0][1]{[4A@1]{[2]{[18]{[6B?@1@4]}}}}";// maxOccurs='2'
            char xsd3[]="[0][1]{[4A@1]{[2]{[18]{[6B?@1@5]}}}}";// ='unbounded'
            char xsd4[]="[0][1]{[4A@1]{[2]{[18]{[6B?@1@5@23]}}}}"; // nillable
            char *xsdScripts[] = { xsd1, xsd2, xsd3, xsd4 };

            const int NUM_XSD_SCRIPTS = sizeof xsdScripts / sizeof *xsdScripts;

            for (int j = 0; j < NUM_XSD_SCRIPTS; ++j) {
                char *xsdScript = xsdScripts[j];
                char *xsdTypeCharPtr = bsl::strchr(xsdScript, '?');
                ASSERT(xsdTypeCharPtr);

                for (int i = 0; i < sizeof XMLTYPECHARS - 1; ++i) {
                    *xsdTypeCharPtr = XMLTYPECHARS[i];

                    bsl::string xmlSchema;
                    gXsd(&xmlSchema, xsdScript);

                    if (verbose) bsl::cout << "  Testing "
                                           << XMLTYPES[*xsdTypeCharPtr - 'a']
                                           << bsl::endl;

                    // The gBdemSchema script is identical in each iteration
                    // except for the type of the single field (and possibly
                    // the formatting mode attribute).

                    char        bdemTypeChar = BDEMTYPECHARS[i];
                    EType::Type elemType     = gType(bdemTypeChar);
                    bsl::string bdemScript   = "A?{B";  // CHOICE record
                    if (3 == j) {
                        bdemScript += "v1";
                    }
                    else {
                        bdemScript += bdemTypeChar;
                    }

                    bdemScript += "@NF";
                    if (0 == j) {  // <list>
                        bdemScript += "@FL";  // 'LIST'
                    }
                    if ('b' == *xsdTypeCharPtr ||   // xs:byte
                        'e' == *xsdTypeCharPtr) {   // xs:decimal
                        bdemScript += "@FD";  // 'DEC'
                    }
                    if (3 == j) {
                        bdemScript += "@FN}";  // 'NILLABLE'
                        char bdemChar = gBdemChar(EType::fromArrayType(
                                                                    elemType));
                        bdemScript += "*{*";
                        bdemScript += bdemChar;
                        bdemScript += "@NF@FN";
                    }
                    bdemScript += '}';

                    bdem_Schema bdemSchema;
                    gBdemSchema(&bdemSchema, bdemScript);

                    if (veryVerbose) {
                        bsl::cout << "    XML Schema = \n" << xmlSchema
                                  << "    Expected bdem_Schema = \n"
                                  << bdemSchema;
                    }

                    Reader reader;
                    Obj    parser(&reader);
                    if (veryVeryVerbose) parser.setVerboseStream(&bsl::cout);

                    bsl::istringstream input(xmlSchema);
                    bdem_Schema outputSchema;
                    bsl::string targetNamespace;
                    int result = parser.parse(input, &outputSchema,
                                              &targetNamespace);

                    if (veryVerbose) bsl::cout << "    Actual bdem_Schema = \n"
                                               << outputSchema;

                    LOOP3_ASSERT(j, *xsdTypeCharPtr,
                                 BDEMTYPECHARS[i], 0 == result);
                    LOOP3_ASSERT(j, *xsdTypeCharPtr, BDEMTYPECHARS[i],
                                 XML_TEST_NAMESPACE == targetNamespace);
                    LOOP3_ASSERT(j, *xsdTypeCharPtr, BDEMTYPECHARS[i],
                                 bdemSchema == outputSchema);
                }
            }
        }
        if (verbose) bsl::cout << "Testing default value." << bsl::endl;
        {
            // The following XML schema scripts all produce identical
            // 'bdem_Schema' output.  They differ in the way the array-ness of
            // an element is expressed in the XSD.  The array of 'xs:int'
            // is given a default value of '20' for each item.
            // Note: <list> elements cannot have default values.
            char xsd1[]="[0][1]{[4A@1]{[2]{[9]{[6Bh@1@4@6]}}}}";
            char xsd2[]="[0][1]{[4A@1]{[2]{[9]{[6Bh@1@5@6]}}}}";
            char xsd3[]="[0][1]{[4A@1]{[2]{[9]{[6Bh@1@5@6@23]}}}}";
            char *xsdScripts[] = { xsd1, xsd2, xsd3 };

            const int NUM_XSD_SCRIPTS = sizeof xsdScripts / sizeof *xsdScripts;

            for (int j = 0; j < NUM_XSD_SCRIPTS; ++j) {
                char *xsdScript = xsdScripts[j];

                bsl::string xmlSchema;
                gXsd(&xmlSchema, xsdScript);

                // The gBdemSchema script is identical in each iteration
                // except possibly for the formatting mode attribute.

                bsl::string bdemScript = "A{Bm@D1";  // default='20'
                if (2 == j) {
                    bdemScript += "@FN";
                }
                bdemScript += '}';

                bdem_Schema bdemSchema;
                gBdemSchema(&bdemSchema, bdemScript);

                if (veryVerbose) {
                    bsl::cout << "    XML Schema = \n" << xmlSchema
                              << "    Expected bdem_Schema = \n"
                              << bdemSchema;
                }

                Reader reader;
                Obj    parser(&reader);
                if (veryVeryVerbose) parser.setVerboseStream(&bsl::cout);

                bsl::istringstream input(xmlSchema);
                bdem_Schema outputSchema;
                bsl::string targetNamespace;
                int result = parser.parse(input, &outputSchema,
                                          &targetNamespace);

                if (veryVerbose) bsl::cout << "    Actual bdem_Schema = \n"
                                           << outputSchema;

                ASSERT(0 == result);
                ASSERT(XML_TEST_NAMESPACE == targetNamespace);
                ASSERT(bdemSchema == outputSchema);
            }
        }

        if (verbose) bsl::cout << "Testing enumerations." << bsl::endl;
        {
            static const TestData DATA[] = {
//
//Line XML Script                                          bdem Script
//==== ==========                                          ===========
 { L_, "[1]{[13E]{[15q]{[24B][24C][24A]}}[4D]{[2]{[9]{[6FE]}}}}",
                                                         "E={ABC}D{Fg=E@NF}" },
 { L_, "[1]{[13E]{[15q]{[24B][24C][24A]}}[4D]{[2]{[9]{[6FE@4]}}}}",
                                                         "E={ABC}D{Fq=E@NF}" },
 { L_, "[1]{[13E]{[15q]{[24B][24C][24A]}}[4D]{[2]{[9]{[6FE@1@4]}}}}",
                                                         "E={ABC}D{Fq=E@NF}" },
 { L_, "[1]{[13E]{[15q]{[24B][24C][24A]}}[4D]{[2]{[9]{[6FE@5]}}}}",
                                                         "E={ABC}D{Fq=E@NF}" },
 { L_, "[1]{[13E]{[15q]{[24B][24C][24A]}}[4D]{[2]{[9]{[6FE@1@5]}}}}",
                                                         "E={ABC}D{Fq=E@NF}" },

 { L_, "[1]{[13E]{[15h]{[24B@18][24C@19][24A@17]}}[4D]{[2]{[9]{[6FE]}}}}",
                                                                  "D{Fc@NF}" },
 { L_, "[1]{[13E]{[15h]{[24B@18][24C@19][24A@17]}}[4D]{[2]{[9]{[6FE@4]}}}}",
                                                                "D{Fm@NF}" },
 { L_, "[1]{[13E]{[15h]{[24B@18][24C@19][24A@17]}}[4D]{[2]{[9]{[6FE@1@4]}}}}",
                                                                "D{Fm@NF}" },
 { L_, "[1]{[13E]{[15h]{[24B@18][24C@19][24A@17]}}[4D]{[2]{[9]{[6FE@5]}}}}",
                                                                "D{Fm@NF}" },
 { L_, "[1]{[13E]{[15h]{[24B@18][24C@19][24A@17]}}[4D]{[2]{[9]{[6FE@1@5]}}}}",
                                                                "D{Fm@NF}" },

 { L_, "[1]{[13E]{[15q]{[24B][24C][24A]}}[4D]{[2]{[9]{[6FE@23]}}}}",
                                                      "E={ABC}D{Fg=E@NF@FN}" },
 { L_, "[1]{[13E]{[15q]{[24B][24C][24A]}}[4D]{[2]{[9]{[6FE@4@23]}}}}",
                                          "E={ABC}D{Fv1@NF@FN}*{*g=E@NF@FN}" },
 { L_, "[1]{[13E]{[15q]{[24B][24C][24A]}}[4D]{[2]{[9]{[6FE@1@4@23]}}}}",
                                          "E={ABC}D{Fv1@NF@FN}*{*g=E@NF@FN}" },
 { L_, "[1]{[13E]{[15q]{[24B][24C][24A]}}[4D]{[2]{[9]{[6FE@5@23]}}}}",
                                          "E={ABC}D{Fv1@NF@FN}*{*g=E@NF@FN}" },
 { L_, "[1]{[13E]{[15q]{[24B][24C][24A]}}[4D]{[2]{[9]{[6FE@1@5@23]}}}}",
                                          "E={ABC}D{Fv1@NF@FN}*{*g=E@NF@FN}" },

 { L_, "[1]{[13E]{[15h]{[24B@18][24C@19][24A@17]}}[4D]{[2]{[9]{[6FE@23]}}}}",
                                                               "D{Fc@NF@FN}" },
 { L_,
   "[1]{[13E]{[15h]{[24B@18][24C@19][24A@17]}}[4D]{[2]{[9]{[6FE@4@23]}}}}",
                                                   "D{Fv1@NF@FN}*{*c@NF@FN}" },
 { L_,
   "[1]{[13E]{[15h]{[24B@18][24C@19][24A@17]}}[4D]{[2]{[9]{[6FE@1@4@23]}}}}",
                                                   "D{Fv1@NF@FN}*{*c@NF@FN}" },
 { L_,
   "[1]{[13E]{[15h]{[24B@18][24C@19][24A@17]}}[4D]{[2]{[9]{[6FE@5@23]}}}}",
                                                   "D{Fv1@NF@FN}*{*c@NF@FN}" },
 { L_,
   "[1]{[13E]{[15h]{[24B@18][24C@19][24A@17]}}[4D]{[2]{[9]{[6FE@1@5@23]}}}}",
                                                   "D{Fv1@NF@FN}*{*c@NF@FN}" },
            };
            static const int NUM_DATA = sizeof DATA / sizeof *DATA;

            static const char mainSchema[] = "[1]{[20]}";

            for (int i = 0; i < NUM_DATA; ++i) {
                const TestData& d = DATA[i];
                runTest(d.d_line, d.d_xmlSchemaScript, d.d_bdemSchemaScript);

                // now test as included schemas
                runTestInc(d.d_line, mainSchema, d.d_xmlSchemaScript,
                           d.d_bdemSchemaScript);
            }
        }

        if (verbose) bsl::cout << "Testing aggregate array types."
                               << bsl::endl;
        {
            static const TestData DATA[] = {
//
//Line XML Script                                          bdem Script
//==== ==========                                          ===========
 { L_, "[0][1]{[3A]{[9]{[6Bh]}}[3D]{[9]{[6BA]}}[6CD]}",
                                                  "C{BuA@NF}A{Bc@NF}"},
 { L_, "[0][1]{[3A]{[9]{[6Bh]}}[3D]{[9]{[6BA@1]}}[6CD]}",
                                                  "C{BuA@NT}A{Bc@NF}"},
 { L_, "[0][1]{[3A]{[9]{[6Bh]}}[3D]{[9]{[6BA@5]}}[6CD]}",
                                                  "C{BvA@NF}A{Bc@NF}"},

 { L_, "[0][1]{[3A]{[9]{[6Bh]}}[3D]{[9]{[6BA@23]}}[6CD]}",
                                              "C{BuA@NF@FN}A{Bc@NF}"},
 { L_, "[0][1]{[3A]{[9]{[6Bh]}}[3D]{[9]{[6BA@23@1]}}[6CD]}",
                                              "C{BuA@NT@FN}A{Bc@NF}"},
 { L_, "[0][1]{[3A]{[9]{[6Bh]}}[3D]{[9]{[6BA@23@5]}}[6CD]}",
                                              "C{BvA@NF@FN}A{Bc@NF}"},
            };
            static const int NUM_DATA = sizeof DATA / sizeof *DATA;

            static const char mainSchema[] = "[1]{[20]}";

            for (int i = 0; i < NUM_DATA; ++i) {
                const TestData& d = DATA[i];
                runTest(d.d_line, d.d_xmlSchemaScript, d.d_bdemSchemaScript);

                // now test as included schemas
                runTestInc(d.d_line, mainSchema, d.d_xmlSchemaScript,
                           d.d_bdemSchemaScript);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING BASIC ELEMENTS AND ATTRIBUTES
        //
        // Concerns:
        //   - Each basic XML schema element type gets mapped to the
        //     appropriate bdem field type.
        //
        // Plan:
        //   - Construct minimal XML schema strings, each containing an element
        //     of a different basic XML schema type.
        //   - Parse each XML schema string into a 'bdem_Schema' and compare
        //     with the expected output.
        //   - Construct minimal XML schema strings, each containing an element
        //     with an attribute of a different basic XML schema type.
        //   - Parse each XML schema string into a 'bdem_Schema' and compare
        //     with the expected output.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING BASIC ELEMENTS AND ATTRIBUTES"
                               << "\n====================================="
                               << bsl::endl;

        // Express the mapping from XML types to bdem types.  Each character
        // is a code used to index the 'XMLTYPES' array and 'gType' function,
        // respectively.
        static const char XMLTYPECHARS[]  = "abcdefghijklmnopqrstuvwxy";
        static const char BDEMTYPECHARS[] = "wayxffecdddddgdbgzbddckkg";

        // The XSD script in each test vector has a '?' character that
        // indicates the position of the XML type code.  Each type can then be
        // substituted in turn within the loop in order to test each type with
        // each construction.
        static const struct {
            int         d_numAttributes;
            bool        d_isChoice;
            bool        d_isNillable;
            const char* d_xsdScript;
        } DATA[] = {
            { 0, false, false, "[0][1]{[4A]{[2]{[9]{[6B?@1][6Ch@1]}}}}"  },
            { 2, false, false, "[0][1]{[4A]{[2]{[22D?][9]{[6Ch@1]}[22Ba]}}}" },
            { 0, true, false, "[0][1]{[4A]{[2]{[18]{[6B?@1][6Ch@1]}}}}" },
            { 0, false, true, "[0][1]{[4A]{[2]{[9]{[6B?@1@23][6Ch@1]}}}}" },
            { 0, true, true, "[0][1]{[4A]{[2]{[18]{[6B?@1@23][6Ch@1]}}}}" },
        };

        static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

        for (int d = 0; d < NUM_DATA; ++d) {
            const int  NUM_ATTRIBUTES  = DATA[d].d_numAttributes;
            const bool IS_CHOICE       = DATA[d].d_isChoice;
            const bool IS_NILLABLE     = DATA[d].d_isNillable;
            const char *const REC_TYPE = IS_CHOICE ? "choice" : "sequence";
            const char *const FLD_TYPE = NUM_ATTRIBUTES
                                       ? "attribute":"element";
            char xsdScript[100];
            bsl::strcpy(xsdScript, DATA[d].d_xsdScript);

            if (verbose) bsl::cout << "Testing <" << FLD_TYPE << "> types "
                                   << "within <" << REC_TYPE << "> constructs."
                                   << bsl::endl;

            // The gXsd script is identical for all types except for the
            // actual type of the element.  The '?' character is a placeholder
            // for the XML type code in the script and should be replaced by
            // the actual type class in each inner loop iteration.
            char *xsdTypeCharPtr = bsl::strchr(xsdScript, '?');

            for (int i = 0; i < sizeof XMLTYPECHARS - 1; ++i) {

                char xmlTypeChar  = XMLTYPECHARS[i];
                char bdemTypeChar = BDEMTYPECHARS[i];
                EType::Type elemType = gType(bdemTypeChar);
                bool isNillableArray = false;
                if (IS_NILLABLE && EType::isArrayType(elemType)) {
                    bdemTypeChar = 'v';
                    isNillableArray = true;
                }

                // Replace '?' in 'gXsd' script with type code at index 'i'.
                *xsdTypeCharPtr = xmlTypeChar;

                if (verbose) bsl::cout << "  Testing "
                                       << XMLTYPES[*xsdTypeCharPtr - 'a']
                                       << bsl::endl;

                bsl::string xmlSchema;
                gXsd(&xmlSchema, xsdScript);

                // The gBdemSchema script is identical in each iteration except
                // for the type of the single field (and possibly the
                // formatting mode attribute).

                bsl::string bdemScript = (IS_CHOICE ? "A?{" : "A{");
                if (NUM_ATTRIBUTES) {
//                     bdemScript += "Bw@FAD";
                    bdemScript += "D";
                    bdemScript += bdemTypeChar;
                    bdemScript += "@FA";
                }
                else {
                    bdemScript += 'B';
                    bdemScript += bdemTypeChar;
                    if (isNillableArray) {
                        bdemScript += '1';
                    }
                }

                switch (xmlTypeChar) {
                  case 'b': bdemScript += "@FD"; break; // byte (as
                                                        // decimal)
                  case 'e': bdemScript += "@FD"; break; // decimal
                  case 'w': bdemScript += "@FB@NT"; break; // base64Binary
                  case 'x': bdemScript += "@FX@NT"; break; // hexBinary
                }

                if (NUM_ATTRIBUTES) {
                    bdemScript += "Bw@FA";
                }

                if (IS_NILLABLE) {
                    if (isNillableArray
                     && 'w' != xmlTypeChar
                     && 'x' != xmlTypeChar) {
                        bdemScript += "@NF";
                    }
                    bdemScript += "@FN";
                }
                bdemScript += "Cc}";

                if (isNillableArray) {
                    char bdemChar = gBdemChar(EType::fromArrayType(elemType));
                    bdemScript += "*{*";
                    bdemScript += bdemChar;
                    bdemScript += "@NF@FN}";
                }

                if (veryVerbose) {
                    P(bdemScript);
                }

                bdem_Schema bdemSchema;
                gBdemSchema(&bdemSchema, bdemScript);

                if (veryVerbose) {
                    bsl::cout << "    XML Schema =\n"           << xmlSchema
                              << "    Expected bdem_Schema =\n" << bdemSchema;
                }

                Reader reader;
                ErrInfo errInfo;
                Obj    parser(&reader, &errInfo);
                if (veryVeryVerbose) parser.setVerboseStream(&bsl::cout);

                bsl::istringstream input(xmlSchema);
                bdem_Schema outputSchema;
                int result = parser.parse(input, &outputSchema);
                if (veryVerbose) bsl::cout << "    Actual bdem_Schema = \n"
                                           << outputSchema;

                bool equal = compareSchema(bdemSchema, outputSchema);
                LOOP5_ASSERT(REC_TYPE, FLD_TYPE, xmlTypeChar, bdemTypeChar,
                             errInfo, 0 == result);
                LOOP4_ASSERT(REC_TYPE, FLD_TYPE, xmlTypeChar, bdemTypeChar,
                             bdemSchema == outputSchema);
            }
        } // End for (each test type, d)

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING gXsd AND gBdemSchema
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING gXsd AND gBdemSchema"
                               << "\n============================"
                               << bsl::endl;

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBREATHING TEST"
                               << "\n==============" << bsl::endl;

        Reader reader;
        ErrInfo errInfo;
        Obj    parser(&reader, &errInfo);
        if (veryVeryVerbose) parser.setVerboseStream(&bsl::cout);

        bdesb_FixedMemInStreamBuf input(SCHEMA1, SCHEMA1_LEN);
        bdem_Schema schema;
        bsl::string targetNamespace;
        int result = parser.parse(&input,
                                  &schema,
                                  &targetNamespace,
                                  "test.xsd");
        ASSERT(0 == result);
        ASSERT(XML_TEST_NAMESPACE == targetNamespace);
        if (0 != result) {
            bsl::cout << "test.xsd:" << errInfo << bsl::endl;
        }

        if (verbose) {
            schema.print(bsl::cout);
        }

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // baexml_SchemaParser INTERACTIVE TEST
        //
        // Invoke this test program with argument -1 and redirect input to an
        // xml schema file.  The program will construct the bdem_Schema and
        // print it out.
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nINTERACTIVE TEST"
                               << "\n================" << bsl::endl;

        Reader reader;
        ErrInfo errInfo;
        Obj    parser(&reader, &errInfo);
        if (veryVeryVerbose) parser.setVerboseStream(&bsl::cout);

        bdem_Schema schema;
        bsl::string targetNamespace;
        int result = parser.parse(bsl::cin,
                                  &schema,
                                  &targetNamespace,
                                  "test.xsd");

        bsl::cerr << errInfo << bsl::endl;
        ASSERT (0 == result);

        bsl::cout << "targetNamespace: " << targetNamespace << bsl::endl;
        schema.print(bsl::cout);

      } break;
      case -2: {
        // --------------------------------------------------------------------
        // gXsd INTERACTIVE TEST
        //
        // Invoke this test program with argument -2 and interactively enter
        // gXsd strings, one per line.  The results of gXsd are printed.  End
        // by typing ^D.
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nINTERACTIVE gXsd TEST"
                               << "\n=====================" << bsl::endl;

        while (bsl::cin) {
            bsl::cout << "> ";
            bsl::string line;
            bsl::getline(bsl::cin, line);
            bsl::string xmlOut;
            LineAndColumn caret = gXsd(&xmlOut, line);
            bsl::cout << xmlOut;
            bsl::cout << "caret at " << caret << bsl::endl;
        }
      } break;
      case -3: {
        // --------------------------------------------------------------------
        // gBdemSchema INTERACTIVE TEST
        //
        // Invoke this test program with argument -2 and interactively enter
        // gBdemSchema strings, one per line.  The results of 'gBdemSchema'
        // are printed.  End by typing ^D.
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nINTERACTIVE gBdemSchema TEST"
                               << "\n============================"
                               << bsl::endl;

        while (bsl::cin) {
            bsl::cout << "> ";
            bsl::string line;
            bsl::getline(bsl::cin, line);
            bdem_Schema schemaOut;
            gBdemSchema(&schemaOut, line);
            bsl::cout << schemaOut;
        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
