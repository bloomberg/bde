// bdem_schemautil.t.cpp            -*-C++-*-

#include <bdem_schemautil.h>

#include <bdem_elemref.h>                    // for testing only
#include <bdem_elemtype.h>
#include <bdem_schema.h>

#include <bdeat_formattingmode.h>            // for testing only

#include <bdet_datetime.h>                   // for testing only
#include <bdet_datetimetz.h>                 // for testing only
#include <bdet_date.h>                       // for testing only
#include <bdet_datetz.h>                     // for testing only
#include <bdet_time.h>                       // for testing only
#include <bdet_timetz.h>                     // for testing only

#include <bdetu_unset.h>                     // for testing only

#include <bslma_testallocator.h>             // for testing only
#include <bslma_testallocatorexception.h>    // for testing only

#include <bsls_assert.h>
#include <bsls_platformutil.h>               // for testing only

#include <bsl_iostream.h>
#include <bsl_string.h>

#include <bsl_cctype.h>         // isspace()
#include <bsl_cstdlib.h>        // atoi()
#include <bsl_cstring.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// [13] bool areEquivalent(const record& record1, const record& record2);
// [ 9] bool areStructurallyEquivalent(& record1, & record2);
// [10] bool areStructurallyEquivalent(& record1, & record2, int mask);
// [11] bool areSymbolicallyEquivalent(& record1, & record2);
// [12] bool areSymbolicallyEquivalent(& record1, & record2, int mask);
// [14] bool isSuperset(const schema& super, const schema& sub);
// [ 8] bool isSuperset(const record& super, const record& sub);
// [ 4] bool isStructuralSuperset(& superRecord, & subRecord);
// [ 5] bool isStructuralSuperset(& superRecord, & subRecord, int mask);
// [ 6] bool isSymbolicSuperset(& superRecord, & subRecord);
// [ 7] bool isSymbolicSuperset(& superRecord, & subRecord, int mask);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] bdem_Schema& gg(bdem_Schema *s, const char *spec) (generator function)
// [ 3] bdem_Schema g(const char *spec)
//
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

//=============================================================================
//                   GLOBAL CONSTANTS/TYPEDEFS FOR TESTING
//=============================================================================

typedef bdem_SchemaUtil          Util;  // component under test

typedef bdem_ElemRef             ERef;
typedef bdem_ElemType            EType;
typedef bdem_FieldDef            FldDef;
typedef bdem_FieldSpec           FldSpec;
typedef bdem_FieldDefAttributes  FldAttr;
typedef bdem_EnumerationDef      EnumDef;
typedef bdem_RecordDef           RecDef;
typedef RecDef::RecordType       RecType;
typedef bdem_Schema              Schema;

typedef bdeat_FormattingMode     Format;

typedef bsls_PlatformUtil::Int64 Int64;

const int MASKS[] = {
    0,
    Util::BDEM_DEFAULT_VALUE,
    Util::BDEM_FORMATTING_MODE,
    Util::BDEM_DEFAULT_VALUE | Util::BDEM_FORMATTING_MODE
};
const int NUM_MASKS = sizeof MASKS / sizeof *MASKS;

#ifndef SIZEOF_ARRAY
#define SIZEOF_ARRAY(a) ((int) (sizeof(a)/sizeof((a)[0])))
#endif

    // Create Three Distinct Exemplars For Each Scalar Element Type
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const char            A00 = 'A';
const char            B00 = 'B';
const char            N00 = bdetu_Unset<char>::unsetValue();

const short           A01 = -1;
const short           B01 = -2;
const short           N01 = bdetu_Unset<short>::unsetValue();

const int             A02 = 2;
const int             B02 = 5;
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
//                              Test apparatus
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
static inline
const char *recName(const bdem_RecordDef& record)
    // Return the name of the specified 'record'.  Note that 'record' does not
    // know its name.  Rather, 'record.schema()' accesses its parent schema,
    // which in turn has a 'recordName' method.  This helper just implements
    // less-convenient 'bdem_schema' functionality.
{
    return record.schema().recordName(record.recordIndex());
}

static inline
int strCmp(const char* lhs, const char* rhs)
    // Return 0 if the specified 'lhs' and 'rhs' null-terminated strings are
    // equal, a negative value if lhs < rhs, and a positive value if lhs > rhs.
    // Note that the behavior is well-defined for null-pointer arguments.
{
    if (0 == lhs && 0 == rhs) return 0;
    if (0 == lhs) return -1;
    if (0 == rhs) return 1;
    return bsl::strcmp(lhs, rhs);
}

static
void setDefaultValueForType(bdem_ElemRef ref, bool other = false)
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

//=============================================================================
//                        GENERATOR LANGUAGE FOR g
//=============================================================================
//
// The g function interprets the given 'spec' in order from left to right
// to build up a schema according to a custom language.  Lowercase letters
// [a ..  j] specify arbitrary (but unique) names to be used for records or
// fields.  Records and fields may be unnamed; this is specified by using the
// '*' character for the name.  Field types are indicated with uppercase
// letters [A ..  N] for unconstrained field, + for a constrained list, and #
// for a constrained table.
//
// A spec consists of 0 or more record definitions.  (An empty spec will result
// in a valid empty schema.)  Records are specified by a leading ':' and name
// character followed by 0 or more field definitions.  A field definition
// is either an unconstrained type specifier followed by a name, or a
// constrained field specifier followed by a name, and then a constraint
// specification.  A constraint specification is either the name of a
// (previously defined) constraint record within this schema, or the 0-based
// index of a record within this schema, less than or equal to the 0-based
// index of the current record.  As may or may not be obvious from this
// description, encountering a name specification character where a field type
// would have been expected indicates the start of a new record.  A ~ indicates
// a removeAll operation is to be performed on the target schema.  The ~ is
// legal anywhere a record definition could begin.  Spaces, tabs, and newlines
// are allowed in the specification but ignored; they are purely cosmetic.
//
// Annotated EBNF grammar:
//  (For those unused to Extended-BNF, parentheses indicate grouping, and *
//  means "0 or more of the preceding".  Although this grammar does not use
//  it, EBNF also defines + as meaning "1 or more of the preceding" and ? as
//  "0 or 1 of the preceding".  As in BNF, | indicates alternatives.)
//
//  Note that items following %% comment markers are not part of the grammar,
//  but comments about semantics.
//
//  Spaces, tabs, and newlines are ignored by the parser, and are not specified
//  in this grammar although they are legal anywhere in the spec.
//
//  Spec      := ('~' | RecordDef)*
//
//  RecordDef := ':' Name ['?'] FieldDef*
//                                  %% Optional '?' indicates CHOICE record.
//
//  FieldDef  := (UnconstrainedFieldType Name)
//             | (ConstrainedFieldType Name ConstraintRef)
//
//  Name      :=
//            '*'                   %% No name, the 0 char *
//           |'a'|'b'|'c'|'d'       %% "a".."d"
//           |'e'                   %% ""  <- An empty string, not "e"
//           |'f'|'g'|'h'|'i'|'j'   %% "f".."p"
//           |'k'|'l'|'m'|'n'|'o'   %% "k".."o"
//           |'p'|'q'|'r'|'s'|'t'   %% "p".."t"
//           |'u'|'v'|'w'|'x'       %% "u".."x"
//
//  UnconstrainedFieldType :=
//            'A'|'B'|'C'|'D'|'E'   %% CHAR, SHORT, INT, INT64, FLOAT,
//           |'F'|'G'|'H'|'I'|'J'   %% DOUBLE, STRING, DATETIME, DATE, TIME
//           |'K'|'L'|'M'|'N'|'O'   %% (CHAR|SHORT|INT|INT64|FLOAT)_ARRAY
//           |'P'|'Q'|'R'|'S'|'T'   %% (DOUBLE|STRING|DATETIME|DATE|TIME)_ARRAY
//           |'U'|'V'               %% LIST, TABLE,
//           |'W'|'X'|'Y'|'Z'       %% BOOL, DATETIMETZ, DATETZ, TIMETZ
//           |'a'|'b'|'c'|'d'       %% (BOOL|DATETIMETZ|DATETZ|TIMETZ)_ARRAY
//           |'e'|'f'               %% CHOICE, CHOICE_ARRAY
//
//  ConstrainedFieldType :=
//            '+'|'#'|'%'|'@'       %% LIST, TABLE, CHOICE, CHOICE_ARRAY
//
//  ConstraintRef := Name | Index
//
//  %% If a constraint is referenced by index, the index is the 0-based number
//  %% of the constraint record in the schema.  Since we're limiting this to
//  %% one digit, we can't constrain by index on any record past the 10th one.
//  %% We also allow constraining by name, so this isn't a big problem.
//  Index     :=
//         '0'|'1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'
//
// g syntax usage examples:
//  Here are some examples, and the schema which would result.
//
// Spec String      Result schema          Description
// -----------      -------------          -----------
// ""               (empty)                An empty schema,
//
// " \t\n  "        (empty)                Whitespace characters are ignored;
//                                         this is equivalent to "".
//
// ":a"             SCHEMA {               A schema containing a single empty
//                      RECORD "a" {       record named "a".
//                      }
//                  }
//
// ":j"             SCHEMA {               A schema containing a single empty
//                      RECORD "j" {       record named "j".
//                      }
//                  }
//
// ":*"             SCHEMA {               A schema containing a single empty
//                      RECORD {           unnamed record.
//                      }
//                  }
//
// ":a:b"           SCHEMA {               A schema containing two empty
//                      RECORD "a" {       records named "a" and "b".
//                      }
//                      RECORD "b" {
//                      }
//                  }
//
// ":aBc"           SCHEMA {               A schema containing one record named
//                      RECORD "a" {       "a" with one DOUBLE field named "c".
//                          DOUBLE "c";
//                      }
//                  }
//
// ":aAa"           SCHEMA {               A schema containing one record named
//                      RECORD "a" {       "a" with one INT field named "a".
//                          INT "a";       Note that fields and records have
//                      }                  separate namespaces.
//                  }
//
// ":aC*"           SCHEMA {               A schema containing one record named
//                      RECORD "a" {       "a" with one STRING field which is
//                          STRING;        unnamed.
//                      }
//                  }
//
// ":aCbDc"         SCHEMA {               A schema containing one record named
//                      RECORD "a" {       "a" with two fields, one a STRING
//                          STRING "b";    named "b" and the other a DATETIME
//                          DATETIME "c";  named "c".
//                      }
//                  }
//
// ":aCb:cDc"       SCHEMA {               A schema containing 2 records.
//                      RECORD "a" {       Record "a" has one STRING field
//                          STRING "b";    named "b".
//                      }
//                      RECORD "c" {       Record "c" has one DATETIME field
//                          DATETIME "c";  named "c".
//                      }
//                  }
//
// ":aCb :cDc"      SCHEMA {               Exactly the same as the previous
//                      RECORD "a" {       example "aCbcDc", but more readable
//                          STRING "b";    with a space inserted between the
//                      }                  record definitions
//                      RECORD "c" {
//                          DATETIME "c";
//                      }
//                  }
//
// ":*A*B* :*C*D* :*:*"
//                  SCHEMA {               Any number of records or fields can
//                      RECORD {           be unnamed.
//                          INT;
//                          DOUBLE;
//                      }
//                      RECORD {
//                          STRING;
//                          DATETIME;
//                      }
//                      RECORD {           Empty records can be defined,
//                      }                  although this is of limited
//                      RECORD {           practical use.
//                      }
//                  }
//
// ":aBc :d+ea"     SCHEMA {               A schema containing 2 records.
//                      RECORD "a" {       Record "a" has one DOUBLE field
//                          DOUBLE "c";    named "c".
//                      }
//                      RECORD "d" {       Record "d" has one constrained LIST
//                          LIST<"a">"";   field named "" constrained on record
//                      }                  "a".
//                  }
//
// ":aBc :d#ea"     SCHEMA {               A schema containing 2 records.
//                      RECORD "a" {       Record "a" has one DOUBLE field
//                          DOUBLE "c";    named "c".
//                      }
//                      RECORD "d" {       Record "d" has one constrained TABLE
//                          TABLE<"a">"";  field named "" constrained on record
//                      }                  "a".
//                  }
//
// ":aBc :d#e0"     SCHEMA {               A schema containing 2 records.
//                      RECORD "a" {       Record "a" has one DOUBLE field
//                          DOUBLE "c";    named "c".
//                      }
//                      RECORD "d" {       Record "d" has one constrained TABLE
//                          TABLE<"a">"";  field named "" constrained on record
//                      }                  "a", referring to "a" by it's index,
//                  }                      0.
//
//=============================================================================
//                bdem_Schema g LANGUAGE INTERNALS
//=============================================================================

static const EType::Type ggElemTypes[] = {
    EType::BDEM_CHAR,             //  0
    EType::BDEM_SHORT,            //  1
    EType::BDEM_INT,              //  2
    EType::BDEM_INT64,            //  3
    EType::BDEM_FLOAT,            //  4
    EType::BDEM_DOUBLE,           //  5
    EType::BDEM_STRING,           //  6
    EType::BDEM_DATETIME,         //  7
    EType::BDEM_DATE,             //  8
    EType::BDEM_TIME,             //  9
    EType::BDEM_CHAR_ARRAY,       // 10
    EType::BDEM_SHORT_ARRAY,      // 11
    EType::BDEM_INT_ARRAY,        // 12
    EType::BDEM_INT64_ARRAY,      // 13
    EType::BDEM_FLOAT_ARRAY,      // 14
    EType::BDEM_DOUBLE_ARRAY,     // 15
    EType::BDEM_STRING_ARRAY,     // 16
    EType::BDEM_DATETIME_ARRAY,   // 17
    EType::BDEM_DATE_ARRAY,       // 18
    EType::BDEM_TIME_ARRAY,       // 19
    EType::BDEM_LIST,             // 20
    EType::BDEM_TABLE,            // 21
    EType::BDEM_BOOL,             // 22
    EType::BDEM_DATETIMETZ,       // 23
    EType::BDEM_DATETZ,           // 24
    EType::BDEM_TIMETZ,           // 25
    EType::BDEM_BOOL_ARRAY,       // 26
    EType::BDEM_DATETIMETZ_ARRAY, // 27
    EType::BDEM_DATETZ_ARRAY,     // 28
    EType::BDEM_TIMETZ_ARRAY,     // 29
    EType::BDEM_CHOICE,           // 30
    EType::BDEM_CHOICE_ARRAY,     // 31
    EType::BDEM_LIST,             // 32 (constrained, but type still 'LIST')
    EType::BDEM_TABLE,            // 33 (constrained, but type still 'TABLE')
    EType::BDEM_CHOICE,           // 34 (constrained, but type still 'CHOICE')
    EType::BDEM_CHOICE_ARRAY      // 35 (constrained, but type still
                                  //     'CHOICE_ARRAY')
};
const int NUM_GG_TYPES = sizeof ggElemTypes / sizeof *ggElemTypes;

//=============================================================================
//                bdem_Schema gg HELPER DATA AND FUNCTIONS
//=============================================================================

// NOTE: 'index' is used in string.h on AIX so switched to indexStr
const char removeTilde[]    = "~";
const char name[]           = "*abcdefghijklmnopqrstuvwx";
const char indexStr[]       = "0123456789";
const char bdemType[]       = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
const char aggBdemType[]    = "+#%@";  // Note: Includes terminal '\0'
const char constrBdemType[] = "+#%@$^!/";  // Note: Includes terminal '\0'

const int NUM_GG_NAMES     = sizeof name           / sizeof(char) - 1;
const int NUM_CONSTR_TYPES = sizeof constrBdemType / sizeof(char) - 1;

bool isName(char token)
{
    return '\0' != token && 0 != bsl::strchr(name, token);
}

bool isConstrainedFieldType(char token)
{
    return '\0' != token && 0 != bsl::strchr(constrBdemType, token);
}

bool isUnconstrainedFieldType(char token)
{
    return '\0' != token && 0 != bsl::strchr(bdemType, token);
}

bool isFieldType(char token)
{
    return isUnconstrainedFieldType(token) || isConstrainedFieldType(token);
}

const char *getName(char nameCode)
    // Return the name corresponding to the specified 'nameCode'.
    //..
    //  '*'      => 0        (a null name)
    //  'a'-'d'  => "a"-"d"
    //  'e'      => ""       (an empty name)
    //  'f'-'x'  => "f"-"x"
    //..
{
    switch (nameCode) {
      case '*': return 0;    // Note that name code '*' is the null string.
      case 'a': return "a";
      case 'b': return "b";
      case 'c': return "c";
      case 'd': return "d";
      case 'e': return "";   // Note that name code 'e' is the empty string.
      case 'f': return "f";
      case 'g': return "g";
      case 'h': return "h";
      case 'i': return "i";
      case 'j': return "j";
      case 'k': return "k";
      case 'l': return "l";
      case 'm': return "m";
      case 'n': return "n";
      case 'o': return "o";
      case 'p': return "p";
      case 'q': return "q";
      case 'r': return "r";
      case 's': return "s";
      case 't': return "t";
      case 'u': return "you";
      case 'v': return "vee";
      case 'w': return "doubleU";
      case 'x': return "ex";
      default: {
        P(nameCode);  ASSERT("Invalid name used in gg script" && 0);
      } break;
    }

    return 0;
}

EType::Type getType(char typeCode)
    // Return the element type corresponding to the specified 'typeCode'.
    //..
    //  'A' => CHAR        'K' => CHAR_ARRAY
    //  'B' => SHORT       'L' => SHORT_ARRAY
    //  'C' => INT         'M' => INT_ARRAY
    //  'D' => INT64       'N' => INT64_ARRAY
    //  'E' => FLOAT       'O' => FLOAT_ARRAY
    //  'F' => DOUBLE      'P' => DOUBLE_ARRAY
    //  'G' => STRING      'Q' => STRING_ARRAY
    //  'H' => DATETIME    'R' => DATETIME_ARRAY
    //  'I' => DATE        'S' => DATE_ARRAY
    //  'J' => TIME        'T' => TIME_ARRAY
    //
    //  'U' => LIST        'V' => TABLE         (used for unconstrained fields)
    //
    //  'W' => BOOL        'a' => BOOL_ARRAY
    //  'X' => DATETIMETZ  'b' => DATETIMETZ_ARRAY
    //  'Y' => DATETZ      'c' => DATETZ_ARRAY
    //  'Z' => TIMETZ      'd' => TIMETZ_ARRAY
    //
    //  'e' => CHOICE      'f' => CHOICE_ARRAY  (used for unconstrained fields)
    //
    //  '+' => LIST        '#' => TABLE         (used for constrained fields)
    //  '%' => CHOICE      '@' => CHOICE_ARRAY  (used for constrained fields)
    //  '$' => INT         '^' => STRING        (used for enumeration fields)
    //  '!' => INT_ARRAY   '/' => STRING_ARRAY  (used for enumeration fields)
    //..
{
    switch (typeCode) {
      case 'A': return EType::BDEM_CHAR;
      case 'B': return EType::BDEM_SHORT;
      case 'C': return EType::BDEM_INT;
      case 'D': return EType::BDEM_INT64;
      case 'E': return EType::BDEM_FLOAT;
      case 'F': return EType::BDEM_DOUBLE;
      case 'G': return EType::BDEM_STRING;
      case 'H': return EType::BDEM_DATETIME;
      case 'I': return EType::BDEM_DATE;
      case 'J': return EType::BDEM_TIME;
      case 'K': return EType::BDEM_CHAR_ARRAY;
      case 'L': return EType::BDEM_SHORT_ARRAY;
      case 'M': return EType::BDEM_INT_ARRAY;
      case 'N': return EType::BDEM_INT64_ARRAY;
      case 'O': return EType::BDEM_FLOAT_ARRAY;
      case 'P': return EType::BDEM_DOUBLE_ARRAY;
      case 'Q': return EType::BDEM_STRING_ARRAY;
      case 'R': return EType::BDEM_DATETIME_ARRAY;
      case 'S': return EType::BDEM_DATE_ARRAY;
      case 'T': return EType::BDEM_TIME_ARRAY;
      case 'U': return EType::BDEM_LIST;
      case 'V': return EType::BDEM_TABLE;
      case 'W': return EType::BDEM_BOOL;
      case 'X': return EType::BDEM_DATETIMETZ;
      case 'Y': return EType::BDEM_DATETZ;
      case 'Z': return EType::BDEM_TIMETZ;
      case 'a': return EType::BDEM_BOOL_ARRAY;
      case 'b': return EType::BDEM_DATETIMETZ_ARRAY;
      case 'c': return EType::BDEM_DATETZ_ARRAY;
      case 'd': return EType::BDEM_TIMETZ_ARRAY;
      case 'e': return EType::BDEM_CHOICE;
      case 'f': return EType::BDEM_CHOICE_ARRAY;

      case '+': return EType::BDEM_LIST;
      case '#': return EType::BDEM_TABLE;
      case '%': return EType::BDEM_CHOICE;
      case '@': return EType::BDEM_CHOICE_ARRAY;
      case '$': return EType::BDEM_INT;
      case '^': return EType::BDEM_STRING;
      case '!': return EType::BDEM_INT_ARRAY;
      case '/': return EType::BDEM_STRING_ARRAY;
      default: {
        P(typeCode);  ASSERT("Invalid element type used in gg script" && 0);
      } break;
    }

    return EType::BDEM_INT;
}

const RecDef *getRecordConstraint(Schema *object, char token)
{
    if (bsl::strchr(indexStr, token)) {
        // constrained by index
        return &object->record(token - '0');
    }

    // else constrained by name
    return object->lookupRecord(getName(token));
}

const EnumDef *getEnumConstraint(Schema *object, char token)
{
    if (bsl::strchr(indexStr, token)) {
        // constrained by index
        return &object->enumeration(token - '0');
    }

    // else constrained by name
    return object->lookupEnumeration(getName(token));
}

int getFormattingMode(char fmtCode)
    // Return the formatting mode corresponding to the specified 'fmtCode'.
    //..
    //  '0' => BDEAT_DEFAULT (zero, not "oh")
    //  'B' => BDEAT_BASE64
    //  'D' => BDEAT_DEC
    //  'L' => BDEAT_LIST
    //  'N' => BDEAT_NILLABLE
    //  'T' => BDEAT_TEXT
    //  'U' => BDEAT_UNTAGGED
    //  'X' => BDEAT_HEX
    //..
{
    switch (fmtCode) {
      case '0': return Format::BDEAT_DEFAULT;
      case 'B': return Format::BDEAT_BASE64;
      case 'D': return Format::BDEAT_DEC;
      case 'L': return Format::BDEAT_LIST;
      case 'N': return Format::BDEAT_NILLABLE;
      case 'T': return Format::BDEAT_TEXT;
      case 'U': return Format::BDEAT_UNTAGGED;
      case 'X': return Format::BDEAT_HEX;

      default: {
        P(fmtCode);  ASSERT("Invalid formatting mode used in gg script" && 0);
      } break;
    }

    return 0;
}

bool getNullability(char nullCode, EType::Type type)
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
      case 'D': return false;
      case 'F': return false;
      case 'T': return true;
      default: {
        P(nullCode);  ASSERT("Invalid nullability used in gg script" && 0);
      } break;
    }

    return true;
}

const char
*addRecord(Schema* object, const char* recordName,
           RecType recordType, const char *p)
{
    RecDef *record = object->createRecord(recordName, recordType);
    ASSERT(record);

    while (isFieldType(*p)) {       // add a field
        const char fieldTypeChar = *p++;
        const EType::Type fieldType = getType(fieldTypeChar);
        ASSERT(isName(*p));
        const char *fieldName = getName(*p++);

        const RecDef *recordConstraint = 0;
        const EnumDef *enumConstraint = 0;
        if (isConstrainedFieldType(fieldTypeChar)) {
            if (EType::isAggregateType(fieldType)) {
                recordConstraint = getRecordConstraint(object, *p++);
                ASSERT(recordConstraint);
            }
            else {
                ASSERT(EnumDef::canHaveEnumConstraint(fieldType));
                enumConstraint = getEnumConstraint(object, *p++);
                ASSERT(enumConstraint);
            }
        }

        // Parse field attributes, if any.

        int  fmt           = Format::BDEAT_DEFAULT;
        bool nullable      = false;
        bool hasNoDefault  = true;
        int  dfltIndex     = -1;
        int  id            = RecDef::BDEM_NULL_FIELD_ID;

        while ('&' == *p) {
            const char attrCode  = *++p;
            const char attrValue = *++p;
            switch (attrCode) {
              case 'D': {
                  ASSERT('0' <= attrValue && attrValue <= '1');
                  dfltIndex = attrValue - '0';
                  hasNoDefault = false;
              } break;
              case 'F': {
                  fmt |= getFormattingMode(attrValue);
              } break;
              case 'I': {
                  ASSERT('0' <= attrValue && attrValue <= '9');
                  id = attrValue - '0';
              } break;
              case 'N': {
                  nullable      = getNullability(attrValue, fieldType);
              } break;
              case 'n': {
                  hasNoDefault  = getNullability(attrValue, fieldType);
              } break;
              default: {
                  P(attrCode);
                  ASSERT("Invalid attribute code used in gg script" && 0);
              } break;
            }
            ++p;
        }

        FldAttr spec(fieldType);
        spec.setFormattingMode(fmt);
        spec.setIsNullable(nullable);
        if (!hasNoDefault) {
            BSLS_ASSERT(! bdem_ElemType::isAggregateType(fieldType));
            spec.defaultValue().makeNull();
        }

        if (-1 != dfltIndex) {
            ASSERT(! EType::isAggregateType(fieldType));
            setDefaultValueForType(spec.defaultValue(), dfltIndex);
        }

        const FldDef *field = (enumConstraint) 
            ? record->appendField(spec, enumConstraint, fieldName, id)
            : record->appendField(spec, recordConstraint, fieldName, id);
       
        ASSERT(field);
    }

    return p;
}

const char *addEnumeration(Schema *object, const char* enumName, const char *p)
{
    EnumDef *enumdef = object->createEnumeration(enumName);
    ASSERT(enumdef);

    while (isName(*p)) {
        const char* enumeratorName = getName(*p++);
        ASSERT(enumeratorName);
        if ('0' <= *p && *p <= '9') {
            int enumeratorId = *p++ - '0';
            enumdef->addEnumerator(enumeratorName, enumeratorId);
        }
        else {
            enumdef->addEnumerator(enumeratorName);
        }
    }

    return p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Schema& gg(Schema *object, const char *spec)
    // Configure the specified schema 'object' according to the specified
    // 'spec' using the primary manipulators and accessors of this component
    // and return a reference to 'object.  Note that normal usage is that
    // 'object' is an empty schema, but the manipulators will be applied to
    // 'object' in whatever state it is in.
{
    enum { MAX_SPEC_LEN = 20000 };
    ASSERT(bsl::strlen(spec) < MAX_SPEC_LEN);

    char cleanSpec[MAX_SPEC_LEN];  // spec after removing all white spaces

    int i = 0;
    for (int j = 0; spec[j]; ++j) {
        if (!isspace(spec[j])) {
            cleanSpec[i++] = spec[j];
        }
    }
    cleanSpec[i] = '\0';

    const char *p = cleanSpec;
    while (*p) {
        if ('~' == *p) {                    // removeAll
            object->removeAll();
            ++p;
        }
        else {
            ASSERT(':' == *p);              // create a record
            const char N = *++p;            // record name index
            ASSERT(isName(N));
            ++p;

            if ('?' == *p) {
                ++p;
                p = addRecord(object, getName(N),
                              RecDef::BDEM_CHOICE_RECORD, p);
            }
            else if ('=' == *p) {
                ++p;
                p = addEnumeration(object, getName(N), p);
            }
            else {
                p = addRecord(object, getName(N),
                              RecDef::BDEM_SEQUENCE_RECORD, p);
            }
        }
    }

    return *object;
}

bdem_Schema g(const char *spec)
    // Return, by value, a new object corresponding to the specified spec.
{
    bool localVerbose = false;
    bdem_Schema object;

    gg(&object, spec);

    if (localVerbose) {
        P(object);
    }

    return object;
}

//=============================================================================
//    Pre-defined schema test values (as spec strings for 'gg')
//=============================================================================

const char *testSpecs[] = {
    // An empty schema.
    "~",

    // One unnamed empty record (sequence and choice).
    "~ :*",
    "~ :*?",

    // One empty record named "" (the empty string, which is a unique name).
    "~ :e",
    "~ :e?",

    // One record named "a", having one field of each unconstrained type.
    "~ :a Aa Bb Cc Dd Ee Ff Gg Hh Ii Jj K* L* M* N* O* P* Q* R* S* T* U* V*"
    "W* X* Y* Z* a* b* c* d* e* f*",

    "~ :a? Aa Bb Cc Dd Ee Ff Gg Hh Ii Jj K* L* M* N* O* P* Q* R* S* T* U* V*"
    "W* X* Y* Z* a* b* c* d* e* f*",

    // One record named "a", with self-referencing constrained types
    // using referencing by name and by number.
    "~ :a +ba #ca +d0 #e0",
    "~ :a? %ba @ca %d0 @e0",

    // A longer schema with a "representative" mix of records and fields.
    "~ "
    ":* Aa "
    ":* "
    ":b Hh Ii Jj K* L* M* N* O* P* Q* R* "
    ":c +db #ec  "
    ":d S* T* U* V* Aa Bb Cc Dd Ee Ff Gg "
    ":* +b0 "
    ":* #ac ",

    "~ "
    ":*? Aa "
    ":* "
    ":b? Hh Ii Jj K* L* M* N* O* P* Q* R* "
    ":c? %db @ec  "
    ":d S* T* U* V* Aa Bb Cc Dd Ee Ff Gg "
    ":* %b0 "
    ":* @ac ",

    // An ugly long schema.
    "~ "
    ":* Aa "
    ":* "
    ":b Hh Ii Jj K* L* M* N* Aa Bb Cc Dd Ee Ff Gg "
    ":c +bb #cc "
    ":* +*b Hh Ii Jj K* L* M* N* Aa Bb Cc Dd Ee Ff Gg +*b "
    ":* #*2 Ff Gg +*b "
    ":a A* Bb Cc Dd Ee Ff Gg +*b "
    ":d Bb Cc Dd Ee Ff Gg #*a "
    ":e Cc Dd Ee Ff Gg +*d "
    ":f Dd Ee Ff +ge "
    ":g Cg "
    ":h "
    ":i B* "
    ":j #aa +ba",
};

const int NUM_SPECS = sizeof testSpecs / sizeof *testSpecs;

//=============================================================================
//            One Last Helper That Uses a 'gg' Helper
//-----------------------------------------------------------------------------

inline
const char *ggName(int index)
    // Return the 'gg' test name corresponding to the specified 'index' if
    // index < NUM_GG_NAMES, and 0 otherwise.
{
    return index < NUM_GG_NAMES ? getName(name[index]) : 0;
}

//=============================================================================
//              Shorthand helpers for functions under test
//-----------------------------------------------------------------------------

inline
bool areEquivalent(const RecDef& r1, const RecDef& r2)
{
    return Util::areEquivalent(r1, r2);
}

inline
bool areEquivalent(const EnumDef& e1, const EnumDef& e2)
{
    return Util::areEquivalent(e1, e2);
}

inline
int
areStructurallyEquivalent(const RecDef& r1, const RecDef& r2)
{
    return Util::areStructurallyEquivalent(r1, r2);
}

inline
int
areStructurallyEquivalent(const RecDef& r1, const RecDef& r2, int mask)
{
    return Util::areStructurallyEquivalent(r1, r2, mask);
}

inline
int
areSymbolicallyEquivalent(const RecDef& r1, const RecDef& r2)
{
    return Util::areSymbolicallyEquivalent(r1, r2);
}

inline
int
areSymbolicallyEquivalent(const RecDef& r1, const RecDef& r2, int mask)
{
    return Util::areSymbolicallyEquivalent(r1, r2, mask);
}

inline
bool isSuperset(const Schema& s1, const Schema& s2)
{
    return Util::isSuperset(s1, s2);
}

inline
bool isSuperset(const RecDef& r1, const RecDef& r2)
{
    return Util::isSuperset(r1, r2);
}

inline
bool isSuperset(const EnumDef& e1, const EnumDef& e2)
{
    return Util::isSuperset(e1, e2);
}

inline
bool isStructuralSuperset(const RecDef& r1, const RecDef& r2)
{
    return Util::isStructuralSuperset(r1, r2);
}

inline
bool isStructuralSuperset(const RecDef& r1, const RecDef& r2, int mask)
{
    return Util::isStructuralSuperset(r1, r2, mask);
}

inline
bool isSymbolicSuperset(const RecDef& r1, const RecDef& r2)
{
    return Util::isSymbolicSuperset(r1, r2);
}

inline
bool isSymbolicSuperset(const RecDef& r1, const RecDef& r2, int mask)
{
    return Util::isSymbolicSuperset(r1, r2, mask);
}

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma_TestAllocator  testAllocator(veryVeryVerbose);
    bslma_Allocator     *Z = &testAllocator;

    switch (test) { case 0:
      case 15: {
        // --------------------------------------------------------------------
        // TESTING REALLY COMPLEX SCHEMAS
        //
        // Concerns:
        //   That the previous testing doesn't really exercise code on really
        //   complex schemas.
        //
        // Plan:
        //   First, build a set of schemas in 2 parts that are spliced
        //   together, possibly with a '&' sequence in between them, for
        //   testing, and test this with various masks.  The first part of the
        //   schema must always end with a record that ends with a scalar
        //   field, so it can be given a default value.  Most things in the
        //   2 schemas have to be named, so that symbolic comparisons can work.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Complex Schemas in pairs\n"
                               "================================\n";

        const int defaultMask = Util::BDEM_DEFAULT_VALUE;
        const int formatMask  = Util::BDEM_FORMATTING_MODE;
        const int enumMask    = Util::BDEM_CHECK_ENUMERATION;
        const int allMask     = defaultMask | formatMask | enumMask;

        const struct {
            int         d_lineNumber;  // line number
            const char *d_spec1;       // 'gg' spec 1
            const char *d_spec2;       // 'gg' spec 2
        } DATA[] = {
            { L_, ":aAb", " :cA*+ea" },
            { L_, ":aAb", " :cA*+ea :dAbCc+da+*c" },
            { L_, ":aAb", " :cA*+ea :dAbCc+da+*c :eCaDb+cd+*c+*a" },
            { L_, ":aAb :bDaDb", " :c+aa+bb :d+ac+bc+cb+da :e+ad" },
            { L_, ":aFaDb :bFaFb :c+a0+bbFc", " :d+ab+bc+cc :e+aa+bb+cc+dd" },
            { L_, ":aFaCb :b+aaFb", " :c?AaBbCcDd :d+ba+ab :e%ac+bd" },
            { L_, ":aFa :b+aa :c+aa+bbFc",
                                      " :d+aa+bb+cc :e+ae :f+aa+bb+cc+dd+ee" },
            { L_, ":a?A*B*C* :b%aaFb", " :cFa%ba :d+ac+bb :e+ad+bc+cb" },
            { L_, ":a?A*B*C* :b%aa :c%aaFbFc", " :d%aa+bb+cc" },
            { L_, ":aFa :b?Aa+baBc :c%abFb", " :d+aa%bb :e+dd+cc+aa%bb" },
            { L_, ":aCcDdEeFf :b?+aaCcDdEeFf :cFa", " :d+aa@bb#ca :e+dd+cc" },
            { L_, ":a=abcd :bCc$da^eaFf", " :c+bb+cc :d+ac :e+dd#eb+bb" },
            { L_, ":aCcDdUu :bUu+aaFf", " :c+aa+bb+cc#da#eb :d+ac+bc+cc" },
        };

        if (verbose) cout << "Equivalent Schemas\n"
                             "==================\n";

        for (int i = 0; i < SIZEOF_ARRAY(DATA); ++i) {
            const int LINE = DATA[i].d_lineNumber;
            string lhs     = DATA[i].d_spec1;
            string rhs     = DATA[i].d_spec2;

            string schStr  = lhs + rhs;

            if (veryVerbose) cout << "    schema: " << schStr << endl;

            Schema lSchema = g(schStr.c_str());
            Schema rSchema = g(schStr.c_str());

            const RecDef *lRec = &lSchema.record(lSchema.numRecords() - 1);
            const RecDef *rRec = &rSchema.record(rSchema.numRecords() - 1);

            LOOP_ASSERT(LINE, isSuperset(lSchema, rSchema));

            LOOP_ASSERT(LINE, areEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE, areStructurallyEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE, areStructurallyEquivalent(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE, areSymbolicallyEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE, areSymbolicallyEquivalent(*lRec, *rRec,
                                                                     allMask));

            LOOP_ASSERT(LINE, isSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE, isStructuralSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE, isStructuralSuperset(*lRec, *rRec, allMask));
            LOOP_ASSERT(LINE, isSymbolicSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE, isSymbolicSuperset(*lRec, *rRec, allMask));
        } // for equivalent schemas

        if (verbose) cout << "Differ by one default\n"
                             "=====================\n";

        for (int i = 0; i < SIZEOF_ARRAY(DATA); ++i) {
            const int LINE = DATA[i].d_lineNumber;
            string lhs     = DATA[i].d_spec1;
            string rhs     = DATA[i].d_spec2;

            string lSchStr = lhs + "&D0" + rhs;
            string rSchStr = lhs + "   " + rhs;

            if (veryVerbose) cout << "  : lSch: " << lSchStr << endl <<
                                     "    rSch: " << rSchStr << endl;

            Schema lSchema = g(lSchStr.c_str());
            Schema rSchema = g(rSchStr.c_str());

            const RecDef *lRec = &lSchema.record(lSchema.numRecords() - 1);
            const RecDef *rRec = &rSchema.record(rSchema.numRecords() - 1);

            LOOP_ASSERT(LINE, !isSuperset(lSchema, rSchema));
            LOOP_ASSERT(LINE, !isSuperset(rSchema, lSchema));

            LOOP_ASSERT(LINE, ! areEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! areEquivalent(*rRec, *lRec));
            LOOP_ASSERT(LINE,   areStructurallyEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE,   areStructurallyEquivalent(*rRec, *lRec));
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE,   areSymbolicallyEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE,   areSymbolicallyEquivalent(*rRec, *lRec));
            LOOP_ASSERT(LINE, ! areSymbolicallyEquivalent(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE, ! areSymbolicallyEquivalent(*lRec, *rRec,
                                                                     allMask));

            LOOP_ASSERT(LINE, ! isSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! isSuperset(*rRec, *lRec));
            LOOP_ASSERT(LINE,   isStructuralSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE,   isStructuralSuperset(*rRec, *lRec));
            LOOP_ASSERT(LINE, ! isStructuralSuperset(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE, ! isStructuralSuperset(*rRec, *lRec,
                                                                     allMask));
            LOOP_ASSERT(LINE,   isSymbolicSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE,   isSymbolicSuperset(*rRec, *lRec));
            LOOP_ASSERT(LINE, ! isSymbolicSuperset(*lRec, *rRec, allMask));
            LOOP_ASSERT(LINE, ! isSymbolicSuperset(*rRec, *lRec, allMask));
        } // for i, differ by a default

        if (verbose) cout << "Differ by default in unnamed field\n"
                             "==================================\n";

        for (int i = 0; i < SIZEOF_ARRAY(DATA); ++i) {
            const int LINE = DATA[i].d_lineNumber;
            string lhs     = DATA[i].d_spec1;
            string rhs     = DATA[i].d_spec2;

            string lSchStr = lhs + "F*&D0" + rhs;
            string rSchStr = lhs + "F*   " + rhs;

            if (veryVerbose) cout << "  : lSch: " << lSchStr << endl <<
                                     "    rSch: " << rSchStr << endl;

            Schema lSchema = g(lSchStr.c_str());
            Schema rSchema = g(rSchStr.c_str());

            const RecDef *lRec = &lSchema.record(lSchema.numRecords() - 1);
            const RecDef *rRec = &rSchema.record(rSchema.numRecords() - 1);

            LOOP_ASSERT(LINE, ! isSuperset(lSchema, rSchema));
            LOOP_ASSERT(LINE, ! isSuperset(rSchema, lSchema));

            LOOP_ASSERT(LINE, ! areEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! areEquivalent(*rRec, *lRec));
            LOOP_ASSERT(LINE,   areStructurallyEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE,   areStructurallyEquivalent(*rRec, *lRec));
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*rRec, *lRec,
                                                                     allMask));
            LOOP_ASSERT(LINE, areSymbolicallyEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE, areSymbolicallyEquivalent(*rRec, *lRec));
            LOOP_ASSERT(LINE, areSymbolicallyEquivalent(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE, areSymbolicallyEquivalent(*rRec, *lRec,
                                                                     allMask));

            LOOP_ASSERT(LINE, ! isSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! isSuperset(*rRec, *lRec));
            LOOP_ASSERT(LINE,   isStructuralSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE,   isStructuralSuperset(*rRec, *lRec));
            LOOP_ASSERT(LINE, ! isStructuralSuperset(*lRec, *rRec, allMask));
            LOOP_ASSERT(LINE, ! isStructuralSuperset(*rRec, *lRec, allMask));
            LOOP_ASSERT(LINE, isSymbolicSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE, isSymbolicSuperset(*rRec, *lRec));
            LOOP_ASSERT(LINE, isSymbolicSuperset(*lRec, *rRec, allMask));
            LOOP_ASSERT(LINE, isSymbolicSuperset(*rRec, *lRec, allMask));
        } // for equivalent schemas

        if (verbose) cout << "Differ by one null field\n"
                             "========================\n";

        for (int i = 0; i < SIZEOF_ARRAY(DATA); ++i) {
            const int LINE = DATA[i].d_lineNumber;
            string lhs     = DATA[i].d_spec1;
            string rhs     = DATA[i].d_spec2;

            string lSchStr = lhs + "&NF" + rhs;
            string rSchStr = lhs + "&NT" + rhs;

            if (veryVerbose) cout << "  : lSch: " << lSchStr << endl <<
                                     "    rSch: " << rSchStr << endl;

            Schema lSchema = g(lSchStr.c_str());
            Schema rSchema = g(rSchStr.c_str());

            if (veryVerbose) cout << "  : lSch: " << lSchema << endl <<
                                     "    rSch: " << rSchema << endl;

            const RecDef *lRec = &lSchema.record(lSchema.numRecords() - 1);
            const RecDef *rRec = &rSchema.record(rSchema.numRecords() - 1);

            LOOP_ASSERT(LINE, !isSuperset(lSchema, rSchema));
            LOOP_ASSERT(LINE, !isSuperset(rSchema, lSchema));

            LOOP_ASSERT(LINE, ! areEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! areEquivalent(*rRec, *lRec));
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*rRec, *lRec));

#if 0
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*lRec, *rRec,
                                                                     allMask));
#endif
            LOOP_ASSERT(LINE, ! areSymbolicallyEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! areSymbolicallyEquivalent(*rRec, *lRec));

#if 0
            LOOP_ASSERT(LINE, ! areSymbolicallyEquivalent(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE, ! areSymbolicallyEquivalent(*lRec, *rRec,
                                                                     allMask));
#endif

            LOOP_ASSERT(LINE, ! isSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! isSuperset(*rRec, *lRec));
            LOOP_ASSERT(LINE, ! isStructuralSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! isStructuralSuperset(*rRec, *lRec));

#if 0
            LOOP_ASSERT(LINE, ! isStructuralSuperset(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE, ! isStructuralSuperset(*rRec, *lRec,
                                                                     allMask));
#endif
            LOOP_ASSERT(LINE, ! isSymbolicSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! isSymbolicSuperset(*rRec, *lRec));
            LOOP_ASSERT(LINE, ! isSymbolicSuperset(*lRec, *rRec, allMask));
            LOOP_ASSERT(LINE, ! isSymbolicSuperset(*rRec, *lRec, allMask));
        } // for i, differ by a null field

        if (verbose) cout << "Differ by one null in unnamed field\n"
                             "===================================\n";

        for (int i = 0; i < SIZEOF_ARRAY(DATA); ++i) {
            const int LINE = DATA[i].d_lineNumber;
            string lhs     = DATA[i].d_spec1;
            string rhs     = DATA[i].d_spec2;

            string lSchStr = lhs + "F*&NF" + rhs;
            string rSchStr = lhs + "F*&NT" + rhs;

            if (veryVerbose) cout << "  : lSch: " << lSchStr << endl <<
                                     "    rSch: " << rSchStr << endl;

            Schema lSchema = g(lSchStr.c_str());
            Schema rSchema = g(rSchStr.c_str());

            const RecDef *lRec = &lSchema.record(lSchema.numRecords() - 1);
            const RecDef *rRec = &rSchema.record(rSchema.numRecords() - 1);

            LOOP_ASSERT(LINE, ! isSuperset(lSchema, rSchema));
            LOOP_ASSERT(LINE, ! isSuperset(rSchema, lSchema));

            LOOP_ASSERT(LINE, ! areEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! areEquivalent(*rRec, *lRec));
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*rRec, *lRec));

#if 0
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*rRec, *lRec,
                                                                     allMask));
#endif
            LOOP_ASSERT(LINE, areSymbolicallyEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE, areSymbolicallyEquivalent(*rRec, *lRec));
            LOOP_ASSERT(LINE, areSymbolicallyEquivalent(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE, areSymbolicallyEquivalent(*rRec, *lRec,
                                                                     allMask));

            LOOP_ASSERT(LINE, ! isSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! isSuperset(*rRec, *lRec));
            LOOP_ASSERT(LINE, ! isStructuralSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! isStructuralSuperset(*rRec, *lRec));
            LOOP_ASSERT(LINE, ! isStructuralSuperset(*lRec, *rRec, allMask));
            LOOP_ASSERT(LINE, ! isStructuralSuperset(*rRec, *lRec, allMask));
            LOOP_ASSERT(LINE, isSymbolicSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE, isSymbolicSuperset(*rRec, *lRec));
            LOOP_ASSERT(LINE, isSymbolicSuperset(*lRec, *rRec, allMask));
            LOOP_ASSERT(LINE, isSymbolicSuperset(*rRec, *lRec, allMask));
        } // for equivalent schemas

        if (verbose) cout << "Differ by one additional field\n"
                             "==============================\n";

        for (int i = 0; i < SIZEOF_ARRAY(DATA); ++i) {
            const int LINE = DATA[i].d_lineNumber;
            string lhs     = DATA[i].d_spec1;
            string rhs     = DATA[i].d_spec2;

            string lSchStr = lhs + "Dt" + rhs;
            string rSchStr = lhs + "  " + rhs;

            if (veryVerbose) cout << "  : lSch: " << lSchStr << endl <<
                                     "    rSch: " << rSchStr << endl;

            Schema lSchema = g(lSchStr.c_str());
            Schema rSchema = g(rSchStr.c_str());

            const RecDef *lRec = &lSchema.record(lSchema.numRecords() - 1);
            const RecDef *rRec = &rSchema.record(rSchema.numRecords() - 1);

            LOOP_ASSERT(LINE,  isSuperset(lSchema, rSchema));
            LOOP_ASSERT(LINE, !isSuperset(rSchema, lSchema));

            LOOP_ASSERT(LINE, ! areEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! areEquivalent(*rRec, *lRec));
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*rRec, *lRec));
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE, ! areSymbolicallyEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! areSymbolicallyEquivalent(*rRec, *lRec));
            LOOP_ASSERT(LINE, ! areSymbolicallyEquivalent(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE, ! areSymbolicallyEquivalent(*lRec, *rRec,
                                                                     allMask));

            LOOP_ASSERT(LINE,   isSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! isSuperset(*rRec, *lRec));
            LOOP_ASSERT(LINE,   isStructuralSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! isStructuralSuperset(*rRec, *lRec));
            LOOP_ASSERT(LINE,   isStructuralSuperset(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE, ! isStructuralSuperset(*rRec, *lRec,
                                                                     allMask));
            LOOP_ASSERT(LINE,   isSymbolicSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! isSymbolicSuperset(*rRec, *lRec));
            LOOP_ASSERT(LINE,   isSymbolicSuperset(*lRec, *rRec, allMask));
            LOOP_ASSERT(LINE, ! isSymbolicSuperset(*rRec, *lRec, allMask));
        } // for i, differ by an additional field

        if (verbose) cout << "Differ by one unnamed additional field\n"
                             "======================================\n";

        for (int i = 0; i < SIZEOF_ARRAY(DATA); ++i) {
            const int LINE = DATA[i].d_lineNumber;
            string lhs     = DATA[i].d_spec1;
            string rhs     = DATA[i].d_spec2;

            string lSchStr = lhs + "D*" + rhs;
            string rSchStr = lhs + "  " + rhs;

            if (veryVerbose) cout << "  : lSch: " << lSchStr << endl <<
                                     "    rSch: " << rSchStr << endl;

            Schema lSchema = g(lSchStr.c_str());
            Schema rSchema = g(rSchStr.c_str());

            const RecDef *lRec = &lSchema.record(lSchema.numRecords() - 1);
            const RecDef *rRec = &rSchema.record(rSchema.numRecords() - 1);

            LOOP_ASSERT(LINE,  isSuperset(lSchema, rSchema));
            LOOP_ASSERT(LINE, !isSuperset(rSchema, lSchema));

            LOOP_ASSERT(LINE, ! areEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! areEquivalent(*rRec, *lRec));
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*rRec, *lRec));
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE, ! areStructurallyEquivalent(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE,   areSymbolicallyEquivalent(*lRec, *rRec));
            LOOP_ASSERT(LINE,   areSymbolicallyEquivalent(*rRec, *lRec));
            LOOP_ASSERT(LINE,   areSymbolicallyEquivalent(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE,   areSymbolicallyEquivalent(*lRec, *rRec,
                                                                     allMask));

            LOOP_ASSERT(LINE,   isSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! isSuperset(*rRec, *lRec));
            LOOP_ASSERT(LINE,   isStructuralSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE, ! isStructuralSuperset(*rRec, *lRec));
            LOOP_ASSERT(LINE,   isStructuralSuperset(*lRec, *rRec,
                                                                     allMask));
            LOOP_ASSERT(LINE, ! isStructuralSuperset(*rRec, *lRec,
                                                                     allMask));
            LOOP_ASSERT(LINE,   isSymbolicSuperset(*lRec, *rRec));
            LOOP_ASSERT(LINE,   isSymbolicSuperset(*rRec, *lRec));
            LOOP_ASSERT(LINE,   isSymbolicSuperset(*lRec, *rRec, allMask));
            LOOP_ASSERT(LINE,   isSymbolicSuperset(*rRec, *lRec, allMask));
        } // for i, differ by an additional field
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'isSuperset' METHOD (for schemas)
        //
        // Concerns:
        //   For schema 's1' to be an exact superset of 's2', the following
        //   properties must hold.
        //     1. Every record in 's2' has a corresponding record (i.e., same
        //        index) in 's1' having the same record name (or both are
        //        unnamed).
        //     2. Every field in every record in 's2' has a corresponding field
        //        (i.e., same index) in the corresponding record in 's1', and
        //        having the same field name.
        //     3. Corresponding fields have the same type and
        //          a. no constraint, or
        //          b. the same constraint index
        //
        // Plan:
        //   We use the table-based implementation technique, where each entry
        //   consists of two schema specification strings and two expected
        //   return values (the latter technique allows for the function to be
        //   called twice on each iteration, with the difference between the
        //   two calls being the order in which 's1' and 's2' are passed).
        //
        //   Broad testing is done using test data selected via enumeration;
        //   more specific testing uses data selected on an ad hoc basis,
        //   although enumeration techniques are used within the selections.
        //
        //   On each test iteration we also perform identity tests:
        //
        //     1. 1 == isSuperset(s1, s1)
        //     2. 1 == isSuperset(s2, s2)
        //
        //   White-box considerations:
        //   -------------------------
        //   1. We know from looking at the implementation that the size of the
        //      schemas (i.e., number of records and/or fields), and the types
        //      of the fields, do not enter into the logic flow; hence, we do
        //      not test with particularly large numbers of records, fields,
        //      and/or types.
        //
        // Testing:
        //   bool isSuperset(const schema& super, const schema& sub);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing isSuperset(Schema, Schema)"
                          << endl << "=================================="
                          << endl;

        if (verbose) cout << "Varying a single property or attribute." << endl;
        {
            const struct {
                int         d_lineNumber;  // line number
                const char *d_spec1;       // 'gg' spec 1
                const char *d_spec2;       // 'gg' spec 2
                int         d_exp12;       // expected result for x1 op x2
                int         d_exp21;       // expected result for x2 op x1
            } DATA[] = {
                //line spec1              spec2                 expected
                //---- -----              -----                 --------
                //  record type
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":a?",             ":a?",                1,     1     },
                { L_,  ":a?",             ":a",                 0,     0     },

                //  record name
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":*",              ":*",                 1,     1     },
                { L_,  ":e",              ":e",                 1,     1     },
                { L_,  ":a",              ":e",                 0,     0     },
                { L_,  ":a",              ":*",                 0,     0     },
                { L_,  ":*",              ":e",                 0,     0     },

                //  field type
                { L_,  ":*A*",            ":*A*",               1,     1     },
                { L_,  ":*A*",            ":*B*",               0,     0     },

                //  field name
                { L_,  ":*Aa",            ":*Aa",               1,     1     },
                { L_,  ":*A*",            ":*A*",               1,     1     },
                { L_,  ":*Ae",            ":*Ae",               1,     1     },
                { L_,  ":*Aa",            ":*Ae",               0,     0     },
                { L_,  ":*Aa",            ":*A*",               0,     0     },
                { L_,  ":*A*",            ":*Ae",               0,     0     },

                //  field formatting mode
                { L_,  ":*A*&F0",         ":*A*",               1,     1     },
                { L_,  ":*A*&FX",         ":*A*",               0,     0     },
                { L_,  ":*A*&FX",         ":*A*&FX",            1,     1     },
                { L_,  ":*A*&FX",         ":*A*&FB",            0,     0     },

                //  field nullability ('U' == 'LIST')
                { L_,  ":*A*&ND",         ":*A*",               1,     1     },
                { L_,  ":*A*&NF",         ":*A*",               1,     1     },
                { L_,  ":*A*&NT",         ":*A*",               0,     0     },
                { L_,  ":*A*&NF",         ":*A*&NT",            0,     0     },

                { L_,  ":*U*&ND",         ":*U*",               1,     1     },
                { L_,  ":*U*&NF",         ":*U*",               1,     1     },
                { L_,  ":*U*&NT",         ":*U*",               0,     0     },
                { L_,  ":*U*&NF",         ":*U*&NT",            0,     0     },

                //  field default value
                { L_,  ":*A*&D0",         ":*A*",               0,     0     },
                { L_,  ":*A*&D0",         ":*A*&D0",            1,     1     },
                { L_,  ":*A*&D0",         ":*A*&D1",            0,     0     },

                //  field id (1-field and 2-field records)
                { L_,  ":*A*&I0",         ":*A*",               1,     1     },
                { L_,  ":*A*&I0",         ":*A*&I0",            1,     1     },
                { L_,  ":*A*&I3",         ":*A*",               0,     0     },
                { L_,  ":*A*&I3",         ":*A*&I3",            1,     1     },
                { L_,  ":*A*&I3",         ":*A*&I2",            0,     0     },
                { L_,  ":*A*&I3",         ":*A*&I0",            0,     0     },

                                                                     // DEPTH 0
                { L_,  ":*Aa   Bb   ",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 1
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb   ",       0,     0     },

                                                                     // DEPTH 2
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I2",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I2Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I2",       0,     0     },

                { L_,  ":*Aa   Bb&I0",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I3",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I3",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I2Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I2",       1,     1     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb&I1",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa   Bb   ",       0,     0     },

                                                                     // DEPTH 3
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I2Bb   ",       0,     0     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I1",       0,     0     },

                                                                     // DEPTH 4
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I1",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I2",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I2Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb&I0",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I2",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I0Bb&I2",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I3",       0,     0     },

            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_PAIRS; ++ti) {
                const int   LINE  = DATA[ti].d_lineNumber;
                const char *SPEC1 = DATA[ti].d_spec1;
                const char *SPEC2 = DATA[ti].d_spec2;
                const int   EXP12 = DATA[ti].d_exp12;
                const int   EXP21 = DATA[ti].d_exp21;

                const Schema S1(g(SPEC1));
                const Schema S2(g(SPEC2));

                LOOP3_ASSERT(LINE, SPEC1, SPEC2, EXP12 == isSuperset(S1, S2));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2, EXP21 == isSuperset(S2, S1));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2, 1     == isSuperset(S1, S1));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2, 1     == isSuperset(S2, S2));
            }
        }

        if (verbose) cout << "Testing SEQUENCE records." << endl;
        {
            const struct {
                int         d_lineNumber;
                const char *d_s1;
                const char *d_s2;
                int         d_exp1;
                int         d_exp2;
            } DATA[] = {
            //
            // Concerns 1, 2, 3a
            //
            //line spec1                spec2                 expected   DEPTH
            //---- -----                -----                 --------   D = 0
            { L_,  "",                  "",                   1,     1       },

            //line spec1                spec2
            //---- -----                -----                            D = 1
            { L_,  ":a",                "",                   1,     0       },
            { L_,  ":*",                "",                   1,     0       },

            //line spec1                spec2
            //---- -----                -----                            D = 2
            { L_,  ":a:b",              "",                   1,     0       },
            { L_,  ":*:*",              "",                   1,     0       },
            { L_,  ":aAx",              "",                   1,     0       },
            { L_,  ":*A*",              "",                   1,     0       },
            { L_,  ":a",                ":a",                 1,     1       },
            { L_,  ":a",                ":b",                 0,     0       },
            { L_,  ":e",                ":e",                 1,     1       },
            { L_,  ":*",                ":*",                 1,     1       },

            //line spec1                spec2
            //---- -----                -----                            D = 3
            { L_,  ":a:b:c",            "",                   1,     0       },
            { L_,  ":*:*:*",            "",                   1,     0       },
            { L_,  ":a:bAh",            "",                   1,     0       },
            { L_,  ":*:*A*",            "",                   1,     0       },
            { L_,  ":aAh:b",            "",                   1,     0       },
            { L_,  ":*A*:*",            "",                   1,     0       },
            { L_,  ":a:b",              ":a",                 1,     0       },
            { L_,  ":e:a",              ":e",                 1,     0       },
            { L_,  ":*:*",              ":*",                 1,     0       },
            { L_,  ":aAhBi",            "",                   1,     0       },
            { L_,  ":*A*B*",            "",                   1,     0       },

            //
            // Notation: <num records>-<num fields>
            //
            //line spec1                spec2                 expected
            //---- -----                -----                 --------   D = 4
            //     4-0                  0-0
            { L_,  ":a:b:c:d",          "",                   1,     0       },
            //     3-1                  0-0
            { L_,  ":aAh:b:c:d",        "",                   1,     0       },
            { L_,  ":a:bAh:c:d",        "",                   1,     0       },
            { L_,  ":a:b:cAh:d",        "",                   1,     0       },
            { L_,  ":a:b:c:dAh",        "",                   1,     0       },
                // 3-0                  1-0
            { L_,  ":a:b:c",            ":d",                 0,     0       },
                // 1-3                  0-0
            { L_,  ":aAhAiAj",          "",                   1,     0       },
            { L_,  ":aAhAi:b",          "",                   1,     0       },
            { L_,  ":aAh:bAh",          "",                   1,     0       },
                // 2-2                  0-0
            { L_,  ":a:bAhAi",          "",                   1,     0       },
            { L_,  ":aAh:bAh",          "",                   1,     0       },
            { L_,  ":a:bAhAi",          "",                   1,     0       },
                // 2-0                  2-0
            { L_,  ":a:b",              ":a:b",               1,     1       },
            { L_,  ":a:b",              ":b:a",               0,     0       },
            { L_,  ":a:b",              ":c:d",               0,     0       },
            { L_,  ":*:*",              ":*:*",               1,     1       },
                // 2-1                  1-0
            { L_,  ":aAh:b",            ":a",                 1,     0       },
            { L_,  ":a:bAh",            ":a",                 1,     0       },
                // 1-2                  1-0
            { L_,  ":aAhAi",            ":a",                 1,     0       },
                // 2-0                  1-1
            { L_,  ":a:b",              ":aAh",               0,     0       },
                // 1-1                  1-1
            { L_,  ":aAh",              ":aAh",               1,     1       },
            { L_,  ":aAh",              ":bAi",               0,     0       },
            { L_,  ":eAe",              ":eAe",               1,     1       },
            { L_,  ":*A*",              ":*A*",               1,     1       },
            { L_,  ":aA*",              ":*Ai",               0,     0       },
            { L_,  ":*Aa",              ":aA*",               0,     0       },
            { L_,  ":eA*",              ":*Ae",               0,     0       },

                //
                // Concerns 3a
                // Notation: 'r' = record, 'f' = field
                //
                // spec1                    spec2             expected
                // -----                    -----             --------
                // rrrff
            { L_,  ":a:b:cAaBb",            ":a",             1,     0       },
            { L_,  ":a:b:cAaBb",            ":h",             0,     0       },
            { L_,  ":a:b:cAaBb",            ":a:b",           1,     0       },
            { L_,  ":a:b:cAaBb",            ":a:i",           0,     0       },
            { L_,  ":a:b:cAaBb",            ":a:b:c",         1,     0       },
            { L_,  ":a:b:cAaBb",            ":a:b:j",         0,     0       },
            { L_,  ":a:b:cAaBb",            ":a:b:cAa",       1,     0       },
            { L_,  ":a:b:cAaBb",            ":a:b:cAh",       0,     0       },
            { L_,  ":a:b:cAaBb",            ":a:b:cAaBb",     1,     1       },
            { L_,  ":a:b:cAaBb",            ":a:b:cBb",       0,     0       },
            { L_,  ":a:b:cAaBb",            ":a:b:cBbAa",     0,     0       },
            { L_,  ":a:b:cAaBb",            ":b:cAaBb:a",     0,     0       },

                // rfffrfff
            { L_,  ":aAaBbCc:bAaBbCc",      ":a",             1,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":h",             0,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":a:b",           1,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":a:i",           0,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":aAa:b",         1,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":aAb:i",         0,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":a:bAa",         1,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":a:bAh",         0,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":aAa:bAa",       1,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":aAa:bAh",       0,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":aAaBb",         1,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":aAaBi",         0,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":aAaBb:b",       1,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":aAaBb:i",       0,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":aAaBb:bAa",     1,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":aAaBb:bAh",     0,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":aAaBb:bAaBb",   1,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":aAaBb:bAaBi",   0,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":aAaBb:bAaAb",   0,     0       },
            { L_,  ":aAaBbCc:bAaBbCc",      ":aAaBb:bAaAb",   0,     0       },

                //
                // Concerns 3b
                //
            { L_,  ":a:b:c:d#a0#b0",        ":a",             1,     0       },
            { L_,  ":a:b:c:d#a0#b0",        ":a:b",           1,     0       },
            { L_,  ":a:b:c:d#a0#b0",        ":a:b:c",         1,     0       },
            { L_,  ":a:b:c:d#a0#b0",        ":a:b:c:d",       1,     0       },
            { L_,  ":a:b:c:d#a0#b0",        ":a:b:c:d#a0",    1,     0       },
            { L_,  ":a:b:c:d#a0#b0",        ":a:b:c:d#a0#b0", 1,     1       },
            { L_,  ":a:b:c:d#a0#b1",        ":a:b:c:d#a1",    0,     0       },
            { L_,  ":a:b:c:d#a0#b1",        ":a:b:c:d#a0#b1", 1,     1       },
            { L_,  ":a:b:c:d#a2#b0",        ":a:b:c:d#a2",    1,     0       },
            { L_,  ":a:b:c:d#a0#b0",        ":a:b:c:d#a1",    0,     0       },
            { L_,  ":a:b:c:d#a0#b0",        ":a:b:c:d#a0#b2", 0,     0       },

            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int   LINE  = DATA[i].d_lineNumber;
                const char *SPEC1 = DATA[i].d_s1;
                const char *SPEC2 = DATA[i].d_s2;
                const int   EXP1  = DATA[i].d_exp1;
                const int   EXP2  = DATA[i].d_exp2;

                const Schema s1(g(SPEC1));
                const Schema s2(g(SPEC2));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2, EXP1 == isSuperset(s1, s2));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2, EXP2 == isSuperset(s2, s1));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2, 1    == isSuperset(s1, s1));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2, 1    == isSuperset(s2, s2));
            }
        }

        if (verbose) cout << "Testing CHOICE records." << endl;
        {
            const struct {
                int         d_lineNumber;
                const char *d_s1;
                const char *d_s2;
                int         d_exp1;
                int         d_exp2;
            } DATA[] = {
            //
            // Concerns 1, 2, 3a
            //
            //line spec1                spec2                 expected   DEPTH
            //---- -----                -----                 --------   D = 0
            { L_,  "",                  "",                   1,     1       },

            //line spec1                spec2
            //---- -----                -----                            D = 1
            { L_,  ":a?",               "",                   1,     0       },
            { L_,  ":*?",               "",                   1,     0       },

            //line spec1                spec2
            //---- -----                -----                            D = 2
            { L_,  ":a?:b?",            "",                   1,     0       },
            { L_,  ":*?:*?",            "",                   1,     0       },
            { L_,  ":a?Ax",             "",                   1,     0       },
            { L_,  ":*?A*",             "",                   1,     0       },
            { L_,  ":a?",               ":a?",                1,     1       },
            { L_,  ":a?",               ":b?",                0,     0       },
            { L_,  ":e?",               ":e?",                1,     1       },
            { L_,  ":*?",               ":*?",                1,     1       },

            //line spec1                spec2
            //---- -----                -----                            D = 3
            { L_,  ":a?:b?:c?",         "",                   1,     0       },
            { L_,  ":*?:*?:*?",         "",                   1,     0       },
            { L_,  ":a?:b?Ah",          "",                   1,     0       },
            { L_,  ":*?:*?A*",          "",                   1,     0       },
            { L_,  ":a?Ah:b?",          "",                   1,     0       },
            { L_,  ":*?A*:*?",          "",                   1,     0       },
            { L_,  ":a?:b?",            ":a?",                1,     0       },
            { L_,  ":e?:a?",            ":e?",                1,     0       },
            { L_,  ":*?:*?",            ":*?",                1,     0       },
            { L_,  ":a?AhBi",           "",                   1,     0       },
            { L_,  ":*?A*B*",           "",                   1,     0       },

            //
            // Notation: <num records>-<num fields>
            //
            //line spec1                spec2                 expected
            //---- ------               -----                 --------   D = 4
            //     4-0                  0-0
            { L_,  ":a?:b?:c?:d?",      "",                   1,     0       },
            //     3-1                  0-0
            { L_,  ":a?Ah:b?:c?:d?",    "",                   1,     0       },
            { L_,  ":a?:b?Ah:c?:d?",    "",                   1,     0       },
            { L_,  ":a?:b?:c?Ah:d?",    "",                   1,     0       },
            { L_,  ":a?:b?:c?:d?Ah",    "",                   1,     0       },
                // 3-0                  1-0
            { L_,  ":a?:b?:c?",         ":d?",                0,     0       },
                // 1-3                  0-0
            { L_,  ":a?AhAiAj",         "",                   1,     0       },
            { L_,  ":a?AhAi:b?",        "",                   1,     0       },
            { L_,  ":a?Ah:b?Ah",        "",                   1,     0       },
                // 2-2                  0-0
            { L_,  ":a?:b?AhAi",        "",                   1,     0       },
            { L_,  ":a?Ah:b?Ah",        "",                   1,     0       },
            { L_,  ":a?:b?AhAi",        "",                   1,     0       },
                // 2-0                  2-0
            { L_,  ":a?:b?",            ":a?:b?",             1,     1       },
            { L_,  ":a?:b?",            ":b?:a?",             0,     0       },
            { L_,  ":a?:b?",            ":c?:d?",             0,     0       },
            { L_,  ":*?:*?",            ":*?:*?",             1,     1       },
                // 2-1                  1-0
            { L_,  ":a?Ah:b?",          ":a?",                1,     0       },
            { L_,  ":a?:b?Ah",          ":a?",                1,     0       },
                // 1-2                  1-0
            { L_,  ":a?AhAi",           ":a?",                1,     0       },
                // 2-0                  1-1
            { L_,  ":a?:b?",            ":a?Ah",              0,     0       },
                // 1-1                  1-1
            { L_,  ":a?Ah",             ":a?Ah",              1,     1       },
            { L_,  ":a?Ah",             ":b?Ai",              0,     0       },
            { L_,  ":e?Ae",             ":e?Ae",              1,     1       },
            { L_,  ":*?A*",             ":*?A*",              1,     1       },
            { L_,  ":a?A*",             ":*?Ai",              0,     0       },
            { L_,  ":*?Aa",             ":a?A*",              0,     0       },
            { L_,  ":e?A*",             ":*?Ae",              0,     0       },

                //
                // Concerns 3a
                // Notation: 'r' = record, 'f' = field
                //
                // spec1                 spec2                expected
                // -----                 -----                --------
                // rrrff
            { L_,  ":a?:b?:c?AaBb",      ":a?",               1,     0       },
            { L_,  ":a?:b?:c?AaBb",      ":h?",               0,     0       },
            { L_,  ":a?:b?:c?AaBb",      ":a?:b?",            1,     0       },
            { L_,  ":a?:b?:c?AaBb",      ":a?:i?",            0,     0       },
            { L_,  ":a?:b?:c?AaBb",      ":a?:b?:c?",         1,     0       },
            { L_,  ":a?:b?:c?AaBb",      ":a?:b?:j?",         0,     0       },
            { L_,  ":a?:b?:c?AaBb",      ":a?:b?:c?Aa",       1,     0       },
            { L_,  ":a?:b?:c?AaBb",      ":a?:b?:c?Ah",       0,     0       },
            { L_,  ":a?:b?:c?AaBb",      ":a?:b?:c?AaBb",     1,     1       },
            { L_,  ":a?:b?:c?AaBb",      ":a?:b?:c?Bb",       0,     0       },
            { L_,  ":a?:b?:c?AaBb",      ":a?:b?:c?BbAa",     0,     0       },
            { L_,  ":a?:b?:c?AaBb",      ":b?:c?AaBb:a?",     0,     0       },

                // rfffrfff
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?",               1,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":h?",               0,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?:b?",            1,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?:i?",            0,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?Aa:b?",          1,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?Ab:i?",          0,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?:b?Aa",          1,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?:b?Ah",          0,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?Aa:b?Aa",        1,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?Aa:b?Ah",        0,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?AaBb",           1,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?AaBi",           0,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?AaBb:b?",        1,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?AaBb:i?",        0,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?AaBb:b?Aa",      1,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?AaBb:b?Ah",      0,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?AaBb:b?AaBb",    1,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?AaBb:b?AaBi",    0,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?AaBb:b?AaAb",    0,     0       },
            { L_,  ":a?AaBbCc:b?AaBbCc", ":a?AaBb:b?AaAb",    0,     0       },

                //
                // Concerns 3b
                //
            { L_,  ":a?:b?:c?:d?@a0@b0", ":a?",                1,    0       },
            { L_,  ":a?:b?:c?:d?@a0@b0", ":a?:b?",             1,    0       },
            { L_,  ":a?:b?:c?:d?@a0@b0", ":a?:b?:c?",          1,    0       },
            { L_,  ":a?:b?:c?:d?@a0@b0", ":a?:b?:c?:d?",       1,    0       },
            { L_,  ":a?:b?:c?:d?@a0@b0", ":a?:b?:c?:d?@a0",    1,    0       },
            { L_,  ":a?:b?:c?:d?@a0@b0", ":a?:b?:c?:d?@a0@b0", 1,    1       },
            { L_,  ":a?:b?:c?:d?@a0@b1", ":a?:b?:c?:d?@a1",    0,    0       },
            { L_,  ":a?:b?:c?:d?@a0@b1", ":a?:b?:c?:d?@a0@b1", 1,    1       },
            { L_,  ":a?:b?:c?:d?@a2@b0", ":a?:b?:c?:d?@a2",    1,    0       },
            { L_,  ":a?:b?:c?:d?@a0@b0", ":a?:b?:c?:d?@a1",    0,    0       },
            { L_,  ":a?:b?:c?:d?@a0@b0", ":a?:b?:c?:d?@a0@b2", 0,    0       },

            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int   LINE  = DATA[i].d_lineNumber;
                const char *SPEC1 = DATA[i].d_s1;
                const char *SPEC2 = DATA[i].d_s2;
                const int   EXP1  = DATA[i].d_exp1;
                const int   EXP2  = DATA[i].d_exp2;

                const Schema s1(g(SPEC1));
                const Schema s2(g(SPEC2));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2, EXP1 == isSuperset(s1, s2));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2, EXP2 == isSuperset(s2, s1));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2, 1    == isSuperset(s1, s1));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2, 1    == isSuperset(s2, s2));
            }
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'areEquivalent' METHOD (for records)
        //
        // Concerns:
        //   For a record 'r1' to be exactly equivalent to a record 'r2', the
        //   following properties must hold.
        //     1. 'r1' and 'r2' have the same length.
        //     2. Every field in 'r1' has a corresponding field (i.e., same
        //        index) in 'r2'.
        //     3. Corresponding fields have the same type, the same name (or
        //        are both unnamed), and
        //          a. no constraint, or
        //          b. an exactly equivalent constraint (as defined by the
        //             'areEquivalent' method under test (recursively)).
        //     4. Record names do not affect this comparison.
        //
        // Plan:
        //   For the methods taking records as arguments, it is convenient to
        //   construct one schema containing N records configured appropriately
        //   for the tests, and then to construct a table containing pairs of
        //   record indices and expected results.
        //
        //   The "depth-ordered enumeration" for test data is achieved in this
        //   test, to a certain extent, by the order in which records are
        //   appended to the schema.  As an additional convenience, records are
        //   appended in pairs, where the odd-index record is identical to its
        //   predecessor except that it is unnamed.  The first six records have
        //   no constrained fields, and the next six do have constrained
        //   fields.
        //
        //   Test data is tabulated as a pair of record indices (i, j)
        //   representing records (ri, rj) in the one schema value used in this
        //   test.  The expected value is tabulated for the method call on
        //   (ri, rj) and on (rj, ri).
        //
        //   Since the record name is not relevant for this comparison method,
        //   every tabulated test is repeated using the records at indices
        //   (i + 1, j) and (i, j + 1) with the same expected results, and
        //   with (i, i + 1) and (i + 1, i) with an expected result of 1
        //   always.
        //
        //   To test for alias behavior, each tabulated test is performed
        //   twice, once on records from two distinct instances of the same
        //   schema value, and once on two records from the same schema
        //   instance.  The "identity" test is also performed on the input
        //   (ri, ri) -- i.e., the exact same record for both arguments.
        //
        // Testing:
        //   bool areEquivalent(const record& record1, const record& record2);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing areEquivalent()"
                          << endl << "=======================" << endl;

        if (verbose) cout << "Varying a single property or attribute." << endl;
        {
            const struct {
                int         d_lineNumber;  // line number
                const char *d_spec1;       // 'gg' spec 1
                const char *d_spec2;       // 'gg' spec 2
                int         d_exp12;       // expected result for x1 op x2
                int         d_exp21;       // expected result for x2 op x1
            } DATA[] = {
                //line spec1              spec2                 expected
                //---- -----              -----                 --------
                //  record type
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":a?",             ":a?",                1,     1     },
                { L_,  ":a?",             ":a",                 0,     0     },

                //  record name
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":*",              ":*",                 1,     1     },
                { L_,  ":e",              ":e",                 1,     1     },
                { L_,  ":a",              ":e",                 1,     1     },
                { L_,  ":a",              ":*",                 1,     1     },
                { L_,  ":*",              ":e",                 1,     1     },

                //  field type
                { L_,  ":*A*",            ":*A*",               1,     1     },
                { L_,  ":*A*",            ":*B*",               0,     0     },

                //  field name
                { L_,  ":*Aa",            ":*Aa",               1,     1     },
                { L_,  ":*A*",            ":*A*",               1,     1     },
                { L_,  ":*Ae",            ":*Ae",               1,     1     },
                { L_,  ":*Aa",            ":*Ae",               0,     0     },
                { L_,  ":*Aa",            ":*A*",               0,     0     },
                { L_,  ":*A*",            ":*Ae",               0,     0     },

                //  field formatting mode
                { L_,  ":*A*&F0",         ":*A*",               1,     1     },
                { L_,  ":*A*&FX",         ":*A*",               0,     0     },
                { L_,  ":*A*&FX",         ":*A*&FX",            1,     1     },
                { L_,  ":*A*&FX",         ":*A*&FB",            0,     0     },

                //  field nullability ('U' == 'LIST')
                { L_,  ":*A*&ND",         ":*A*",               1,     1     },
                { L_,  ":*A*&NF",         ":*A*",               1,     1     },
                { L_,  ":*A*&NT",         ":*A*",               0,     0     },
                { L_,  ":*A*&NF",         ":*A*&NT",            0,     0     },

                { L_,  ":*U*&ND",         ":*U*",               1,     1     },
                { L_,  ":*U*&NF",         ":*U*",               1,     1     },
                { L_,  ":*U*&NT",         ":*U*",               0,     0     },
                { L_,  ":*U*&NF",         ":*U*&NT",            0,     0     },

                //  field default value
                { L_,  ":*A*&D0",         ":*A*",               0,     0     },
                { L_,  ":*A*&D0",         ":*A*&D0",            1,     1     },
                { L_,  ":*A*&D0",         ":*A*&D1",            0,     0     },

                //  field id (1-field and 2-field records)
                { L_,  ":*A*&I0",         ":*A*",               1,     1     },
                { L_,  ":*A*&I0",         ":*A*&I0",            1,     1     },
                { L_,  ":*A*&I3",         ":*A*",               0,     0     },
                { L_,  ":*A*&I3",         ":*A*&I3",            1,     1     },
                { L_,  ":*A*&I3",         ":*A*&I2",            0,     0     },
                { L_,  ":*A*&I3",         ":*A*&I0",            0,     0     },

                                                                     // DEPTH 0
                { L_,  ":*Aa   Bb   ",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 1
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb   ",       0,     0     },

                                                                     // DEPTH 2
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I2",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I2Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I2",       0,     0     },

                { L_,  ":*Aa   Bb&I0",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I3",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I3",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I2Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I2",       1,     1     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb&I1",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa   Bb   ",       0,     0     },

                                                                     // DEPTH 3
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I2Bb   ",       0,     0     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I1",       0,     0     },

                                                                     // DEPTH 4
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I1",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I2",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I2Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb&I0",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I2",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I0Bb&I2",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I3",       0,     0     },

            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_PAIRS; ++ti) {
                const int   LINE  = DATA[ti].d_lineNumber;
                const char *SPEC1 = DATA[ti].d_spec1;
                const char *SPEC2 = DATA[ti].d_spec2;
                const int   EXP12 = DATA[ti].d_exp12;
                const int   EXP21 = DATA[ti].d_exp21;

                const Schema S1(g(SPEC1));  const RecDef& R1 = S1.record(0);
                const Schema S2(g(SPEC2));  const RecDef& R2 = S2.record(0);

                LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                             EXP12 == areEquivalent(R1, R2));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                             EXP21 == areEquivalent(R2, R1));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                             1     == areEquivalent(R1, R1));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                             1     == areEquivalent(R2, R2));
            }
        }

        if (verbose) cout << "Testing SEQUENCE records." << endl;
        {
            static const char* SPEC =
                ":a                          :*"
                ":bAa                        :*Aa"
                ":cA*                        :*A*"
                ":dAaBb                      :*AaBb"
                ":eA*B*                      :*A*B*"
                ":fBbAa                      :*BbAa"
                ":gAa+bd                     :*Aa+bd"
                ":hAa+bb#cg                  :*Aa+bb#cg"
                ":iAb+a3#cg                  :*Ab+a3#cg"
                ":jA*+*3#*h                  :*A*+*3#*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       0,     0       },
                {  L_,      4,    2,       0,     0       },
                {  L_,      6,    4,       0,     0       },
                {  L_,      8,    6,       0,     0       },
                {  L_,     10,    2,       0,     0       },

                {  L_,     10,    8,       0,     0       },
                {  L_,     12,    2,       0,     0       },
                {  L_,     12,    4,       0,     0       },
                {  L_,     14,    2,       0,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       0,     0       },
                {  L_,     18,    2,       0,     0       },
                {  L_,     18,   12,       0,     0       },
                {  L_,     18,   14,       0,     0       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

                LOOP_ASSERT(LINE, EXP12 == areEquivalent(r11, r12));
                LOOP_ASSERT(LINE, EXP12 == areEquivalent(r11p1, r12));
                LOOP_ASSERT(LINE, EXP12 == areEquivalent(r11, r12p1));
                LOOP_ASSERT(LINE, EXP12 == areEquivalent(r11, r22));
                LOOP_ASSERT(LINE, EXP21 == areEquivalent(r12, r11));
                LOOP_ASSERT(LINE, 1     == areEquivalent(r11, r11p1));
                LOOP_ASSERT(LINE, 1     == areEquivalent(r12, r12p1));
                LOOP_ASSERT(LINE, 1     == areEquivalent(r11, r11));
                LOOP_ASSERT(LINE, 1     == areEquivalent(r12, r12));
            }
        }

        if (verbose) cout << "Testing CHOICE records." << endl;
        {
            static const char* SPEC =
                ":a?                          :*?"
                ":b?Aa                        :*?Aa"
                ":c?A*                        :*?A*"
                ":d?AaBb                      :*?AaBb"
                ":e?A*B*                      :*?A*B*"
                ":f?BbAa                      :*?BbAa"
                ":g?Aa%bd                     :*?Aa%bd"
                ":h?Aa%bb@cg                  :*?Aa%bb@cg"
                ":i?Ab%a3@cg                  :*?Ab%a3@cg"
                ":j?A*%*3@*h                  :*?A*%*3@*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       0,     0       },
                {  L_,      4,    2,       0,     0       },
                {  L_,      6,    4,       0,     0       },
                {  L_,      8,    6,       0,     0       },
                {  L_,     10,    2,       0,     0       },

                {  L_,     10,    8,       0,     0       },
                {  L_,     12,    2,       0,     0       },
                {  L_,     12,    4,       0,     0       },
                {  L_,     14,    2,       0,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       0,     0       },
                {  L_,     18,    2,       0,     0       },
                {  L_,     18,   12,       0,     0       },
                {  L_,     18,   14,       0,     0       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

                LOOP_ASSERT(LINE, EXP12 == areEquivalent(r11, r12));
                LOOP_ASSERT(LINE, EXP12 == areEquivalent(r11p1, r12));
                LOOP_ASSERT(LINE, EXP12 == areEquivalent(r11, r12p1));
                LOOP_ASSERT(LINE, EXP12 == areEquivalent(r11, r22));
                LOOP_ASSERT(LINE, EXP21 == areEquivalent(r12, r11));
                LOOP_ASSERT(LINE, 1     == areEquivalent(r11, r11p1));
                LOOP_ASSERT(LINE, 1     == areEquivalent(r12, r12p1));
                LOOP_ASSERT(LINE, 1     == areEquivalent(r11, r11));
                LOOP_ASSERT(LINE, 1     == areEquivalent(r12, r12));
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'areSymbolicallyEquivalent' METHOD WITH MASK (for records)
        //
        // Concerns:
        //   For a record 'r1' to be symbolically equivalent to a record 'r2',
        //   the following properties must hold.
        //     1. 'r1' and 'r2' have the same length.
        //     2. Every *named* field in 'r1' has a corresponding field (i.e.,
        //        having the same name) in 'r2'.
        //     3. Corresponding fields have the same type and
        //          a. no constraint, or
        //          b. a symbolically equivalent constraint (as defined by the
        //             'areSymbolicallyEquivalent' method under test
        //             (recursively)).
        //     4. Field *indices* and unnamed fields do not affect the
        //        comparison.
        //
        // Plan:
        //   For the methods taking records as arguments, it is convenient to
        //   construct one schema containing N records configured appropriately
        //   for the tests, and then to construct a table containing pairs of
        //   record indices and expected results.
        //
        //   The "depth-ordered enumeration" for test data is achieved in this
        //   test, to a certain extent, by the order in which records are
        //   appended to the schema.  As an additional convenience, records are
        //   appended in pairs, where the odd-index record is identical to its
        //   predecessor except that it is unnamed.  The first six records have
        //   no constrained fields, and the next six do have constrained
        //   fields.
        //
        //   Test data is tabulated as a pair of record indices (i, j)
        //   representing records (ri, rj) in the one schema value used in this
        //   test.  The expected value is tabulated for the method call on
        //   (ri, rj) and on (rj, ri).
        //
        //   Since the record name is not relevant for this comparison method,
        //   every tabulated test is repeated using the records at indices
        //   (i + 1, j) and (i, j + 1) with the same expected results, and
        //   with (i, i + 1) and (i + 1, i) with an expected result of 1
        //   always.
        //
        //   To test for alias behavior, each tabulated test is performed
        //   twice, once on records from two distinct instances of the same
        //   schema value, and once on two records from the same schema
        //   instance.  The "identity" test is also performed on the input
        //   (ri, ri) -- i.e., the exact same record for both arguments.
        //
        // Testing:
        //   bool areSymbolicallyEquivalent(& record1, & record2);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing areSymbolicallyEquivalent()"
                          << endl << "==================================="
                          << endl;

        if (verbose) cout << "Varying a single property or attribute." << endl;
        {
            const struct {
                int         d_lineNumber;  // line number
                const char *d_spec1;       // 'gg' spec 1
                const char *d_spec2;       // 'gg' spec 2
                int         d_exp12;       // expected result for x1 op x2
                int         d_exp21;       // expected result for x2 op x1
            } DATA[] = {
                //line spec1              spec2                 expected
                //---- -----              -----                 --------
                //  record type
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":a?",             ":a?",                1,     1     },
                { L_,  ":a?",             ":a",                 0,     0     },

                //  record name
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":*",              ":*",                 1,     1     },
                { L_,  ":e",              ":e",                 1,     1     },
                { L_,  ":a",              ":e",                 1,     1     },
                { L_,  ":a",              ":*",                 1,     1     },
                { L_,  ":*",              ":e",                 1,     1     },

                //  field type
                { L_,  ":*Aa",            ":*Aa",               1,     1     },
                { L_,  ":*Aa",            ":*Ba",               0,     0     },

                //  field name
                { L_,  ":*Aa",            ":*Aa",               1,     1     },
                { L_,  ":*A*",            ":*A*",               1,     1     },
                { L_,  ":*Ae",            ":*Ae",               1,     1     },
                { L_,  ":*Aa",            ":*Ae",               0,     0     },
                { L_,  ":*Aa",            ":*A*",               0,     0     },
                { L_,  ":*A*",            ":*Ae",               0,     0     },

                //  field nullability ('U' == 'LIST')
                { L_,  ":*Aa&ND",         ":*Aa",               1,     1     },
                { L_,  ":*Aa&NF",         ":*Aa",               1,     1     },
                { L_,  ":*Aa&NT",         ":*Aa",               0,     0     },
                { L_,  ":*Aa&NF",         ":*Aa&NT",            0,     0     },

                { L_,  ":*Ua&ND",         ":*Ua",               1,     1     },
                { L_,  ":*Ua&NF",         ":*Ua",               1,     1     },
                { L_,  ":*Ua&NT",         ":*Ua",               0,     0     },
                { L_,  ":*Ua&NF",         ":*Ua&NT",            0,     0     },

                //  field id (1-field and 2-field records)
                { L_,  ":*Aa&I0",         ":*Aa",               0,     0     },
                { L_,  ":*Aa&I0",         ":*Aa&I0",            1,     1     },
                { L_,  ":*Aa&I3",         ":*Aa",               0,     0     },
                { L_,  ":*Aa&I3",         ":*Aa&I3",            1,     1     },
                { L_,  ":*Aa&I3",         ":*Aa&I2",            0,     0     },
                { L_,  ":*Aa&I3",         ":*Aa&I0",            0,     0     },

                                                                     // DEPTH 0
                { L_,  ":*Aa   Bb   ",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 1
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb   ",       0,     0     },

                                                                     // DEPTH 2
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I2",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I2Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I2",       0,     0     },

                { L_,  ":*Aa   Bb&I0",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I3",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I3",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I2Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I2",       1,     1     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb&I1",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa   Bb   ",       0,     0     },

                                                                     // DEPTH 3
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I2Bb   ",       0,     0     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I1",       0,     0     },

                                                                     // DEPTH 4
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I1",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I2",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I2Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb&I0",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I2",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I0Bb&I2",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I3",       0,     0     },

            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_PAIRS; ++ti) {
                const int   LINE  = DATA[ti].d_lineNumber;
                const char *SPEC1 = DATA[ti].d_spec1;
                const char *SPEC2 = DATA[ti].d_spec2;
                const int   EXP12 = DATA[ti].d_exp12;
                const int   EXP21 = DATA[ti].d_exp21;

                const Schema S1(g(SPEC1));  const RecDef& R1 = S1.record(0);
                const Schema S2(g(SPEC2));  const RecDef& R2 = S2.record(0);

                for (int m = 0; m < NUM_MASKS; ++m) {
                    const int M = MASKS[m];

                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                EXP12 == areSymbolicallyEquivalent(R1, R2, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                EXP21 == areSymbolicallyEquivalent(R2, R1, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                1     == areSymbolicallyEquivalent(R1, R1, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                1     == areSymbolicallyEquivalent(R2, R2, M));
                }
            }
        }
        {
            const struct {
                int         d_lineNumber;  // line number
                const char *d_spec1;       // 'gg' spec 1
                const char *d_spec2;       // 'gg' spec 2
                int         d_exp[4];      // expected results per mask
            } DATA[] = {
                //line spec1             spec2                 expected
                //---- -----             -----            -----------------
                //  field formatting mode
                { L_,  ":*Aa&F0",        ":*Aa",          { 1,  1,  1,  1 }  },
                { L_,  ":*Aa&FX",        ":*Aa",          { 1,  1,  0,  0 }  },
                { L_,  ":*Aa&FX",        ":*Aa&FX",       { 1,  1,  1,  1 }  },
                { L_,  ":*Aa&FX",        ":*Aa&FB",       { 1,  1,  0,  0 }  },

                //  field default value
                { L_,  ":*Aa&D0",        ":*Aa",          { 1,  0,  1,  0 }  },
                { L_,  ":*Aa&D0",        ":*Aa&D0",       { 1,  1,  1,  1 }  },
                { L_,  ":*Aa&D0",        ":*Aa&D1",       { 1,  0,  1,  0 }  },

                //  combinations
                { L_,  ":*Aa&FL&D0",     ":*Aa&FL&D0",    { 1,  1,  1,  1 }  },
                { L_,  ":*Aa&FL&D0",     ":*Aa&FX&D0",    { 1,  1,  0,  0 }  },
                { L_,  ":*Aa&FL&D0",     ":*Aa&FL&D1",    { 1,  0,  1,  0 }  },
                { L_,  ":*Aa&FL&D0",     ":*Aa&FX&D1",    { 1,  0,  0,  0 }  },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_PAIRS; ++ti) {
                const int   LINE  = DATA[ti].d_lineNumber;
                const char *SPEC1 = DATA[ti].d_spec1;
                const char *SPEC2 = DATA[ti].d_spec2;
                const int  *EXP   = DATA[ti].d_exp;

                const Schema S1(g(SPEC1));  const RecDef& R1 = S1.record(0);
                const Schema S2(g(SPEC2));  const RecDef& R2 = S2.record(0);

                for (int m = 0; m < NUM_MASKS; ++m) {
                    const int M = MASKS[m];

                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                               EXP[m] == areSymbolicallyEquivalent(R1, R2, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                               EXP[m] == areSymbolicallyEquivalent(R2, R1, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                               1      == areSymbolicallyEquivalent(R1, R1, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                               1      == areSymbolicallyEquivalent(R2, R2, M));
                }
            }
        }

        if (verbose) cout << "Testing SEQUENCE records." << endl;
        {
            static const char* SPEC =
                ":a                          :*"
                ":bAa                        :*Aa"
                ":cA*                        :*A*"
                ":dAaBb                      :*AaBb"
                ":eA*B*                      :*A*B*"
                ":fBbAa                      :*BbAa"
                ":gAa+bd                     :*Aa+bd"
                ":hAa+bb#cg                  :*Aa+bb#cg"
                ":iAb+a3#cg                  :*Ab+a3#cg"
                ":jA*+*3#*h                  :*A*+*3#*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       0,     0       },
                {  L_,      4,    2,       0,     0       },
                {  L_,      6,    4,       0,     0       },
                {  L_,      8,    6,       0,     0       },
                {  L_,     10,    2,       0,     0       },

                {  L_,     10,    8,       0,     0       },
                {  L_,     12,    2,       0,     0       },
                {  L_,     12,    4,       0,     0       },
                {  L_,     14,    2,       0,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       0,     0       },
                {  L_,     18,    2,       0,     0       },
                {  L_,     18,   12,       0,     0       },
                {  L_,     18,   14,       0,     0       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

                for (int m = 0; m < NUM_MASKS; ++m) {
                    const int M = MASKS[m];

          //----^
          LOOP_ASSERT(LINE, EXP12 == areSymbolicallyEquivalent(r11, r12, M));
          LOOP_ASSERT(LINE, EXP12 == areSymbolicallyEquivalent(r11p1, r12, M));
          LOOP_ASSERT(LINE, EXP12 == areSymbolicallyEquivalent(r11, r12p1, M));
          LOOP_ASSERT(LINE, EXP12 == areSymbolicallyEquivalent(r11, r22, M));
          LOOP_ASSERT(LINE, EXP21 == areSymbolicallyEquivalent(r12, r11, M));
          LOOP_ASSERT(LINE, 1     == areSymbolicallyEquivalent(r11, r11p1, M));
          LOOP_ASSERT(LINE, 1     == areSymbolicallyEquivalent(r12, r12p1, M));
          LOOP_ASSERT(LINE, 1     == areSymbolicallyEquivalent(r11, r11, M));
          LOOP_ASSERT(LINE, 1     == areSymbolicallyEquivalent(r12, r12, M));
          //----v
                }
            }
        }

        if (verbose) cout << "Testing CHOICE records." << endl;
        {
            static const char* SPEC =
                ":a?                          :*?"
                ":b?Aa                        :*?Aa"
                ":c?A*                        :*?A*"
                ":d?AaBb                      :*?AaBb"
                ":e?A*B*                      :*?A*B*"
                ":f?BbAa                      :*?BbAa"
                ":g?Aa%bd                     :*?Aa%bd"
                ":h?Aa%bb@cg                  :*?Aa%bb@cg"
                ":i?Ab%a3@cg                  :*?Ab%a3@cg"
                ":j?A*%*3@*h                  :*?A*%*3@*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       0,     0       },
                {  L_,      4,    2,       0,     0       },
                {  L_,      6,    4,       0,     0       },
                {  L_,      8,    6,       0,     0       },
                {  L_,     10,    2,       0,     0       },

                {  L_,     10,    8,       0,     0       },
                {  L_,     12,    2,       0,     0       },
                {  L_,     12,    4,       0,     0       },
                {  L_,     14,    2,       0,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       0,     0       },
                {  L_,     18,    2,       0,     0       },
                {  L_,     18,   12,       0,     0       },
                {  L_,     18,   14,       0,     0       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

                for (int m = 0; m < NUM_MASKS; ++m) {
                    const int M = MASKS[m];

          //----^
          LOOP_ASSERT(LINE, EXP12 == areSymbolicallyEquivalent(r11, r12, M));
          LOOP_ASSERT(LINE, EXP12 == areSymbolicallyEquivalent(r11p1, r12, M));
          LOOP_ASSERT(LINE, EXP12 == areSymbolicallyEquivalent(r11, r12p1, M));
          LOOP_ASSERT(LINE, EXP12 == areSymbolicallyEquivalent(r11, r22, M));
          LOOP_ASSERT(LINE, EXP21 == areSymbolicallyEquivalent(r12, r11, M));
          LOOP_ASSERT(LINE, 1     == areSymbolicallyEquivalent(r11, r11p1, M));
          LOOP_ASSERT(LINE, 1     == areSymbolicallyEquivalent(r12, r12p1, M));
          LOOP_ASSERT(LINE, 1     == areSymbolicallyEquivalent(r11, r11, M));
          LOOP_ASSERT(LINE, 1     == areSymbolicallyEquivalent(r12, r12, M));
          //----v
                }
            }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'areSymbolicallyEquivalent' METHOD (for records)
        //
        // Concerns:
        //   For a record 'r1' to be symbolically equivalent to a record 'r2',
        //   the following properties must hold.
        //     1. 'r1' and 'r2' have the same length.
        //     2. Every *named* field in 'r1' has a corresponding field (i.e.,
        //        having the same name) in 'r2'.
        //     3. Corresponding fields have the same type and
        //          a. no constraint, or
        //          b. a symbolically equivalent constraint (as defined by the
        //             'areSymbolicallyEquivalent' method under test
        //             (recursively)).
        //     4. Field *indices* and unnamed fields do not affect the
        //        comparison.
        //
        // Plan:
        //   For the methods taking records as arguments, it is convenient to
        //   construct one schema containing N records configured appropriately
        //   for the tests, and then to construct a table containing pairs of
        //   record indices and expected results.
        //
        //   The "depth-ordered enumeration" for test data is achieved in this
        //   test, to a certain extent, by the order in which records are
        //   appended to the schema.  As an additional convenience, records are
        //   appended in pairs, where the odd-index record is identical to its
        //   predecessor except that it is unnamed.  The first six records have
        //   no constrained fields, and the next six do have constrained
        //   fields.
        //
        //   Test data is tabulated as a pair of record indices (i, j)
        //   representing records (ri, rj) in the one schema value used in this
        //   test.  The expected value is tabulated for the method call on
        //   (ri, rj) and on (rj, ri).
        //
        //   Since the record name is not relevant for this comparison method,
        //   every tabulated test is repeated using the records at indices
        //   (i + 1, j) and (i, j + 1) with the same expected results, and
        //   with (i, i + 1) and (i + 1, i) with an expected result of 1
        //   always.
        //
        //   To test for alias behavior, each tabulated test is performed
        //   twice, once on records from two distinct instances of the same
        //   schema value, and once on two records from the same schema
        //   instance.  The "identity" test is also performed on the input
        //   (ri, ri) -- i.e., the exact same record for both arguments.
        //
        // Testing:
        //   bool areSymbolicallyEquivalent(& record1, & record2);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing areSymbolicallyEquivalent()"
                          << endl << "==================================="
                          << endl;

        if (verbose) cout << "Varying a single property or attribute." << endl;
        {
            const struct {
                int         d_lineNumber;  // line number
                const char *d_spec1;       // 'gg' spec 1
                const char *d_spec2;       // 'gg' spec 2
                int         d_exp12;       // expected result for x1 op x2
                int         d_exp21;       // expected result for x2 op x1
            } DATA[] = {
                //line spec1              spec2                 expected
                //---- -----              -----                 --------
                //  record type
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":a?",             ":a?",                1,     1     },
                { L_,  ":a?",             ":a",                 0,     0     },

                //  record name
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":*",              ":*",                 1,     1     },
                { L_,  ":e",              ":e",                 1,     1     },
                { L_,  ":a",              ":e",                 1,     1     },
                { L_,  ":a",              ":*",                 1,     1     },
                { L_,  ":*",              ":e",                 1,     1     },

                //  field type
                { L_,  ":*Aa",            ":*Aa",               1,     1     },
                { L_,  ":*Aa",            ":*Ba",               0,     0     },

                //  field name
                { L_,  ":*Aa",            ":*Aa",               1,     1     },
                { L_,  ":*A*",            ":*A*",               1,     1     },
                { L_,  ":*Ae",            ":*Ae",               1,     1     },
                { L_,  ":*Aa",            ":*Ae",               0,     0     },
                { L_,  ":*Aa",            ":*A*",               0,     0     },
                { L_,  ":*A*",            ":*Ae",               0,     0     },

                //  field formatting mode
                { L_,  ":*Aa&F0",         ":*Aa",               1,     1     },
                { L_,  ":*Aa&FX",         ":*Aa",               1,     1     },
                { L_,  ":*Aa&FX",         ":*Aa&FX",            1,     1     },
                { L_,  ":*Aa&FX",         ":*Aa&FB",            1,     1     },

                //  field nullability ('U' == 'LIST')
                { L_,  ":*Aa&ND",         ":*Aa",               1,     1     },
                { L_,  ":*Aa&NF",         ":*Aa",               1,     1     },
                { L_,  ":*Aa&NT",         ":*Aa",               0,     0     },
                { L_,  ":*Aa&NF",         ":*Aa&NT",            0,     0     },

                { L_,  ":*Ua&ND",         ":*Ua",               1,     1     },
                { L_,  ":*Ua&NF",         ":*Ua",               1,     1     },
                { L_,  ":*Ua&NT",         ":*Ua",               0,     0     },
                { L_,  ":*Ua&NF",         ":*Ua&NT",            0,     0     },

                //  field default value
                { L_,  ":*Aa&D0",         ":*Aa",               1,     1     },
                { L_,  ":*Aa&D0",         ":*Aa&D0",            1,     1     },
                { L_,  ":*Aa&D0",         ":*Aa&D1",            1,     1     },

                //  field id (1-field and 2-field records)
                { L_,  ":*Aa&I0",         ":*Aa",               0,     0     },
                { L_,  ":*Aa&I0",         ":*Aa&I0",            1,     1     },
                { L_,  ":*Aa&I3",         ":*Aa",               0,     0     },
                { L_,  ":*Aa&I3",         ":*Aa&I3",            1,     1     },
                { L_,  ":*Aa&I3",         ":*Aa&I2",            0,     0     },
                { L_,  ":*Aa&I3",         ":*Aa&I0",            0,     0     },

                                                                     // DEPTH 0
                { L_,  ":*Aa   Bb   ",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 1
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb   ",       0,     0     },

                                                                     // DEPTH 2
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I2",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I2Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I2",       0,     0     },

                { L_,  ":*Aa   Bb&I0",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I3",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I3",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I2Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I2",       1,     1     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb&I1",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa   Bb   ",       0,     0     },

                                                                     // DEPTH 3
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I2Bb   ",       0,     0     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I1",       0,     0     },

                                                                     // DEPTH 4
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I1",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I2",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I2Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb&I0",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I2",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I0Bb&I2",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I3",       0,     0     },

            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_PAIRS; ++ti) {
                const int   LINE  = DATA[ti].d_lineNumber;
                const char *SPEC1 = DATA[ti].d_spec1;
                const char *SPEC2 = DATA[ti].d_spec2;
                const int   EXP12 = DATA[ti].d_exp12;
                const int   EXP21 = DATA[ti].d_exp21;

                const Schema S1(g(SPEC1));  const RecDef& R1 = S1.record(0);
                const Schema S2(g(SPEC2));  const RecDef& R2 = S2.record(0);

                LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                             EXP12 == areSymbolicallyEquivalent(R1, R2));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                             EXP21 == areSymbolicallyEquivalent(R2, R1));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                             1     == areSymbolicallyEquivalent(R1, R1));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                             1     == areSymbolicallyEquivalent(R2, R2));
            }
        }

        if (verbose) cout << "Testing SEQUENCE records." << endl;
        {
            static const char* SPEC =
                ":a                          :*"
                ":bAa                        :*Aa"
                ":cA*                        :*A*"
                ":dAaBb                      :*AaBb"
                ":eA*B*                      :*A*B*"
                ":fBbAa                      :*BbAa"
                ":gAa+bd                     :*Aa+bd"
                ":hAa+bb#cg                  :*Aa+bb#cg"
                ":iAb+a3#cg                  :*Ab+a3#cg"
                ":jA*+*3#*h                  :*A*+*3#*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       0,     0       },
                {  L_,      4,    2,       0,     0       },
                {  L_,      6,    4,       0,     0       },
                {  L_,      8,    6,       0,     0       },
                {  L_,     10,    2,       0,     0       },

                {  L_,     10,    8,       0,     0       },
                {  L_,     12,    2,       0,     0       },
                {  L_,     12,    4,       0,     0       },
                {  L_,     14,    2,       0,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       0,     0       },
                {  L_,     18,    2,       0,     0       },
                {  L_,     18,   12,       0,     0       },
                {  L_,     18,   14,       0,     0       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

            //--^
            LOOP_ASSERT(LINE, EXP12 == areSymbolicallyEquivalent(r11, r12));
            LOOP_ASSERT(LINE, EXP12 == areSymbolicallyEquivalent(r11p1, r12));
            LOOP_ASSERT(LINE, EXP12 == areSymbolicallyEquivalent(r11, r12p1));
            LOOP_ASSERT(LINE, EXP12 == areSymbolicallyEquivalent(r11, r22));
            LOOP_ASSERT(LINE, EXP21 == areSymbolicallyEquivalent(r12, r11));
            LOOP_ASSERT(LINE, 1     == areSymbolicallyEquivalent(r11, r11p1));
            LOOP_ASSERT(LINE, 1     == areSymbolicallyEquivalent(r12, r12p1));
            LOOP_ASSERT(LINE, 1     == areSymbolicallyEquivalent(r11, r11));
            LOOP_ASSERT(LINE, 1     == areSymbolicallyEquivalent(r12, r12));
            //--v
            }
        }

        if (verbose) cout << "Testing CHOICE records." << endl;
        {
            static const char* SPEC =
                ":a?                          :*?"
                ":b?Aa                        :*?Aa"
                ":c?A*                        :*?A*"
                ":d?AaBb                      :*?AaBb"
                ":e?A*B*                      :*?A*B*"
                ":f?BbAa                      :*?BbAa"
                ":g?Aa%bd                     :*?Aa%bd"
                ":h?Aa%bb@cg                  :*?Aa%bb@cg"
                ":i?Ab%a3@cg                  :*?Ab%a3@cg"
                ":j?A*%*3@*h                  :*?A*%*3@*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       0,     0       },
                {  L_,      4,    2,       0,     0       },
                {  L_,      6,    4,       0,     0       },
                {  L_,      8,    6,       0,     0       },
                {  L_,     10,    2,       0,     0       },

                {  L_,     10,    8,       0,     0       },
                {  L_,     12,    2,       0,     0       },
                {  L_,     12,    4,       0,     0       },
                {  L_,     14,    2,       0,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       0,     0       },
                {  L_,     18,    2,       0,     0       },
                {  L_,     18,   12,       0,     0       },
                {  L_,     18,   14,       0,     0       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

            //--^
            LOOP_ASSERT(LINE, EXP12 == areSymbolicallyEquivalent(r11, r12));
            LOOP_ASSERT(LINE, EXP12 == areSymbolicallyEquivalent(r11p1, r12));
            LOOP_ASSERT(LINE, EXP12 == areSymbolicallyEquivalent(r11, r12p1));
            LOOP_ASSERT(LINE, EXP12 == areSymbolicallyEquivalent(r11, r22));
            LOOP_ASSERT(LINE, EXP21 == areSymbolicallyEquivalent(r12, r11));
            LOOP_ASSERT(LINE, 1     == areSymbolicallyEquivalent(r11, r11p1));
            LOOP_ASSERT(LINE, 1     == areSymbolicallyEquivalent(r12, r12p1));
            LOOP_ASSERT(LINE, 1     == areSymbolicallyEquivalent(r11, r11));
            LOOP_ASSERT(LINE, 1     == areSymbolicallyEquivalent(r12, r12));
            //--v
            }
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'areStructurallyEquivalent' METHOD WITH MASK (for records)
        //
        // Concerns:
        //   For a record 'r1' to be structurally equivalent to a record 'r2',
        //   the following properties must hold.
        //     1. 'r1' and 'r2' have the same length.
        //     2. Every field in 'r1' has a corresponding field (i.e., same
        //        index) in 'r2'.
        //     3. Corresponding fields have the same type and
        //          a. no constraint, or
        //          b. a structurally equivalent constraint (as defined by the
        //             'areStructurallyEquivalent' method under test
        //             (recursively)).
        //     4. Field *names* do not affect the comparison.
        //
        // Plan:
        //   For the methods taking records as arguments, it is convenient to
        //   construct one schema containing N records configured appropriately
        //   for the tests, and then to construct a table containing pairs of
        //   record indices and expected results.
        //
        //   The "depth-ordered enumeration" for test data is achieved in this
        //   test, to a certain extent, by the order in which records are
        //   appended to the schema.  As an additional convenience, records are
        //   appended in pairs, where the odd-index record is identical to its
        //   predecessor except that it is unnamed.  The first six records have
        //   no constrained fields, and the next six do have constrained
        //   fields.
        //
        //   Test data is tabulated as a pair of record indices (i, j)
        //   representing records (ri, rj) in the one schema value used in this
        //   test.  The expected value is tabulated for the method call on
        //   (ri, rj) and on (rj, ri).
        //
        //   Since the record name is not relevant for this comparison method,
        //   every tabulated test is repeated using the records at indices
        //   (i + 1, j) and (i, j + 1) with the same expected results, and
        //   with (i, i + 1) and (i + 1, i) with an expected result of 1
        //   always.
        //
        //   To test for alias behavior, each tabulated test is performed
        //   twice, once on records from two distinct instances of the same
        //   schema value, and once on two records from the same schema
        //   instance.  The "identity" test is also performed on the input
        //   (ri, ri) -- i.e., the exact same record for both arguments.
        //
        // Testing:
        //   bool areStructurallyEquivalent(& record1, & record2);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing areStructurallyEquivalent()"
                          << endl << "==================================="
                          << endl;

        if (verbose) cout << "Varying a single property or attribute." << endl;
        {
            const struct {
                int         d_lineNumber;  // line number
                const char *d_spec1;       // 'gg' spec 1
                const char *d_spec2;       // 'gg' spec 2
                int         d_exp12;       // expected result for x1 op x2
                int         d_exp21;       // expected result for x2 op x1
            } DATA[] = {
                //line spec1              spec2                 expected
                //---- -----              -----                 --------
                //  record type
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":a?",             ":a?",                1,     1     },
                { L_,  ":a?",             ":a",                 0,     0     },

                //  record name
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":*",              ":*",                 1,     1     },
                { L_,  ":e",              ":e",                 1,     1     },
                { L_,  ":a",              ":e",                 1,     1     },
                { L_,  ":a",              ":*",                 1,     1     },
                { L_,  ":*",              ":e",                 1,     1     },

                //  field type
                { L_,  ":*A*",            ":*A*",               1,     1     },
                { L_,  ":*A*",            ":*B*",               0,     0     },

                //  field name
                { L_,  ":*Aa",            ":*Aa",               1,     1     },
                { L_,  ":*A*",            ":*A*",               1,     1     },
                { L_,  ":*Ae",            ":*Ae",               1,     1     },
                { L_,  ":*Aa",            ":*Ae",               1,     1     },
                { L_,  ":*Aa",            ":*A*",               1,     1     },
                { L_,  ":*A*",            ":*Ae",               1,     1     },

                //  field nullability ('U' == 'LIST')
                { L_,  ":*A*&ND",         ":*A*",               1,     1     },
                { L_,  ":*A*&NF",         ":*A*",               1,     1     },
                { L_,  ":*A*&NT",         ":*A*",               0,     0     },
                { L_,  ":*A*&NF",         ":*A*&NT",            0,     0     },

                { L_,  ":*U*&ND",         ":*U*",               1,     1     },
                { L_,  ":*U*&NF",         ":*U*",               1,     1     },
                { L_,  ":*U*&NT",         ":*U*",               0,     0     },
                { L_,  ":*U*&NF",         ":*U*&NT",            0,     0     },

                //  field id (1-field and 2-field records)
                { L_,  ":*A*&I0",         ":*A*",               1,     1     },
                { L_,  ":*A*&I0",         ":*A*&I0",            1,     1     },
                { L_,  ":*A*&I3",         ":*A*",               1,     1     },
                { L_,  ":*A*&I3",         ":*A*&I3",            1,     1     },
                { L_,  ":*A*&I3",         ":*A*&I2",            1,     1     },
                { L_,  ":*A*&I3",         ":*A*&I0",            1,     1     },

                                                                     // DEPTH 0
                { L_,  ":*Aa   Bb   ",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 1
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 2
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I3Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I2",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I2Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I2",       1,     1     },

                { L_,  ":*Aa   Bb&I0",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa&I3Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I3",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I1Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I3Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I3",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I2Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I2",       1,     1     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb&I1",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 3
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I1Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I2Bb   ",       1,     1     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I1",       1,     1     },

                                                                     // DEPTH 4
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I1",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I2",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I2Bb&I1",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb&I0",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb&I1",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I2",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I0Bb&I2",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I3",       1,     1     },

            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_PAIRS; ++ti) {
                const int   LINE  = DATA[ti].d_lineNumber;
                const char *SPEC1 = DATA[ti].d_spec1;
                const char *SPEC2 = DATA[ti].d_spec2;
                const int   EXP12 = DATA[ti].d_exp12;
                const int   EXP21 = DATA[ti].d_exp21;

                const Schema S1(g(SPEC1));  const RecDef& R1 = S1.record(0);
                const Schema S2(g(SPEC2));  const RecDef& R2 = S2.record(0);

                for (int m = 0; m < NUM_MASKS; ++m) {
                    const int M = MASKS[m];

                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                EXP12 == areStructurallyEquivalent(R1, R2, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                EXP21 == areStructurallyEquivalent(R2, R1, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                1     == areStructurallyEquivalent(R1, R1, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                1     == areStructurallyEquivalent(R2, R2, M));
                }
            }
        }
        {
            const struct {
                int         d_lineNumber;  // line number
                const char *d_spec1;       // 'gg' spec 1
                const char *d_spec2;       // 'gg' spec 2
                int         d_exp[4];      // expected results per mask
            } DATA[] = {
                //line spec1             spec2                 expected
                //---- -----             -----            -----------------
                //  field formatting mode
                { L_,  ":*A*&F0",        ":*A*",          { 1,  1,  1,  1 }  },
                { L_,  ":*A*&FX",        ":*A*",          { 1,  1,  0,  0 }  },
                { L_,  ":*A*&FX",        ":*A*&FX",       { 1,  1,  1,  1 }  },
                { L_,  ":*A*&FX",        ":*A*&FB",       { 1,  1,  0,  0 }  },

                //  field default value
                { L_,  ":*A*&D0",        ":*A*",          { 1,  0,  1,  0 }  },
                { L_,  ":*A*&D0",        ":*A*&D0",       { 1,  1,  1,  1 }  },
                { L_,  ":*A*&D0",        ":*A*&D1",       { 1,  0,  1,  0 }  },

                //  combinations
                { L_,  ":*A*&FL&D0",     ":*A*&FL&D0",    { 1,  1,  1,  1 }  },
                { L_,  ":*A*&FL&D0",     ":*A*&FX&D0",    { 1,  1,  0,  0 }  },
                { L_,  ":*A*&FL&D0",     ":*A*&FL&D1",    { 1,  0,  1,  0 }  },
                { L_,  ":*A*&FL&D0",     ":*A*&FX&D1",    { 1,  0,  0,  0 }  },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_PAIRS; ++ti) {
                const int   LINE  = DATA[ti].d_lineNumber;
                const char *SPEC1 = DATA[ti].d_spec1;
                const char *SPEC2 = DATA[ti].d_spec2;
                const int  *EXP   = DATA[ti].d_exp;

                const Schema S1(g(SPEC1));  const RecDef& R1 = S1.record(0);
                const Schema S2(g(SPEC2));  const RecDef& R2 = S2.record(0);

                for (int m = 0; m < NUM_MASKS; ++m) {
                    const int M = MASKS[m];

                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                               EXP[m] == areStructurallyEquivalent(R1, R2, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                               EXP[m] == areStructurallyEquivalent(R2, R1, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                               1      == areStructurallyEquivalent(R1, R1, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                               1      == areStructurallyEquivalent(R2, R2, M));
                }
            }
        }

        if (verbose) cout << "Testing SEQUENCE records." << endl;
        {
            static const char* SPEC =
                ":a                          :*"
                ":bAa                        :*Aa"
                ":cA*                        :*A*"
                ":dAaBb                      :*AaBb"
                ":eA*B*                      :*A*B*"
                ":fBbAa                      :*BbAa"
                ":gAa+bd                     :*Aa+bd"
                ":hAa+bb#cg                  :*Aa+bb#cg"
                ":iAb+a3#cg                  :*Ab+a3#cg"
                ":jA*+*3#*h                  :*A*+*3#*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       0,     0       },
                {  L_,      4,    2,       1,     1       },
                {  L_,      6,    4,       0,     0       },
                {  L_,      8,    6,       1,     1       },
                {  L_,     10,    2,       0,     0       },

                {  L_,     10,    8,       0,     0       },
                {  L_,     12,    2,       0,     0       },
                {  L_,     12,    4,       0,     0       },
                {  L_,     14,    2,       0,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       1,     1       },
                {  L_,     18,    2,       0,     0       },
                {  L_,     18,   12,       0,     0       },
                {  L_,     18,   14,       0,     0       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

                for (int m = 0; m < NUM_MASKS; ++m) {
                    const int M = MASKS[m];

          //----^
          LOOP_ASSERT(LINE, EXP12 == areStructurallyEquivalent(r11, r12, M));
          LOOP_ASSERT(LINE, EXP12 == areStructurallyEquivalent(r11p1, r12, M));
          LOOP_ASSERT(LINE, EXP12 == areStructurallyEquivalent(r11, r12p1, M));
          LOOP_ASSERT(LINE, EXP12 == areStructurallyEquivalent(r11, r22, M));
          LOOP_ASSERT(LINE, EXP21 == areStructurallyEquivalent(r12, r11, M));
          LOOP_ASSERT(LINE, 1     == areStructurallyEquivalent(r11, r11p1, M));
          LOOP_ASSERT(LINE, 1     == areStructurallyEquivalent(r12, r12p1, M));
          LOOP_ASSERT(LINE, 1     == areStructurallyEquivalent(r11, r11, M));
          LOOP_ASSERT(LINE, 1     == areStructurallyEquivalent(r12, r12, M));
          //----v
                }
            }
        }

        if (verbose) cout << "Testing CHOICE records." << endl;
        {
            static const char* SPEC =
                ":a?                          :*?"
                ":b?Aa                        :*?Aa"
                ":c?A*                        :*?A*"
                ":d?AaBb                      :*?AaBb"
                ":e?A*B*                      :*?A*B*"
                ":f?BbAa                      :*?BbAa"
                ":g?Aa%bd                     :*?Aa%bd"
                ":h?Aa%bb@cg                  :*?Aa%bb@cg"
                ":i?Ab%a3@cg                  :*?Ab%a3@cg"
                ":j?A*%*3@*h                  :*?A*%*3@*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       0,     0       },
                {  L_,      4,    2,       1,     1       },
                {  L_,      6,    4,       0,     0       },
                {  L_,      8,    6,       1,     1       },
                {  L_,     10,    2,       0,     0       },

                {  L_,     10,    8,       0,     0       },
                {  L_,     12,    2,       0,     0       },
                {  L_,     12,    4,       0,     0       },
                {  L_,     14,    2,       0,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       1,     1       },
                {  L_,     18,    2,       0,     0       },
                {  L_,     18,   12,       0,     0       },
                {  L_,     18,   14,       0,     0       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

                for (int m = 0; m < NUM_MASKS; ++m) {
                    const int M = MASKS[m];

          //----^
          LOOP_ASSERT(LINE, EXP12 == areStructurallyEquivalent(r11, r12, M));
          LOOP_ASSERT(LINE, EXP12 == areStructurallyEquivalent(r11p1, r12, M));
          LOOP_ASSERT(LINE, EXP12 == areStructurallyEquivalent(r11, r12p1, M));
          LOOP_ASSERT(LINE, EXP12 == areStructurallyEquivalent(r11, r22, M));
          LOOP_ASSERT(LINE, EXP21 == areStructurallyEquivalent(r12, r11, M));
          LOOP_ASSERT(LINE, 1     == areStructurallyEquivalent(r11, r11p1, M));
          LOOP_ASSERT(LINE, 1     == areStructurallyEquivalent(r12, r12p1, M));
          LOOP_ASSERT(LINE, 1     == areStructurallyEquivalent(r11, r11, M));
          LOOP_ASSERT(LINE, 1     == areStructurallyEquivalent(r12, r12, M));
          //----v
                }
            }
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'areStructurallyEquivalent' METHOD (for records)
        //
        // Concerns:
        //   For a record 'r1' to be structurally equivalent to a record 'r2',
        //   the following properties must hold.
        //     1. 'r1' and 'r2' have the same length.
        //     2. Every field in 'r1' has a corresponding field (i.e., same
        //        index) in 'r2'.
        //     3. Corresponding fields have the same type and
        //          a. no constraint, or
        //          b. a structurally equivalent constraint (as defined by the
        //             'areStructurallyEquivalent' method under test
        //             (recursively)).
        //     4. Field *names* do not affect the comparison.
        //
        // Plan:
        //   For the methods taking records as arguments, it is convenient to
        //   construct one schema containing N records configured appropriately
        //   for the tests, and then to construct a table containing pairs of
        //   record indices and expected results.
        //
        //   The "depth-ordered enumeration" for test data is achieved in this
        //   test, to a certain extent, by the order in which records are
        //   appended to the schema.  As an additional convenience, records are
        //   appended in pairs, where the odd-index record is identical to its
        //   predecessor except that it is unnamed.  The first six records have
        //   no constrained fields, and the next six do have constrained
        //   fields.
        //
        //   Test data is tabulated as a pair of record indices (i, j)
        //   representing records (ri, rj) in the one schema value used in this
        //   test.  The expected value is tabulated for the method call on
        //   (ri, rj) and on (rj, ri).
        //
        //   Since the record name is not relevant for this comparison method,
        //   every tabulated test is repeated using the records at indices
        //   (i + 1, j) and (i, j + 1) with the same expected results, and
        //   with (i, i + 1) and (i + 1, i) with an expected result of 1
        //   always.
        //
        //   To test for alias behavior, each tabulated test is performed
        //   twice, once on records from two distinct instances of the same
        //   schema value, and once on two records from the same schema
        //   instance.  The "identity" test is also performed on the input
        //   (ri, ri) -- i.e., the exact same record for both arguments.
        //
        // Testing:
        //   bool areStructurallyEquivalent(& record1, & record2);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing areStructurallyEquivalent()"
                          << endl << "==================================="
                          << endl;

        if (verbose) cout << "Varying a single property or attribute." << endl;
        {
            const struct {
                int         d_lineNumber;  // line number
                const char *d_spec1;       // 'gg' spec 1
                const char *d_spec2;       // 'gg' spec 2
                int         d_exp12;       // expected result for x1 op x2
                int         d_exp21;       // expected result for x2 op x1
            } DATA[] = {
                //line spec1              spec2                 expected
                //---- -----              -----                 --------
                //  record type
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":a?",             ":a?",                1,     1     },
                { L_,  ":a?",             ":a",                 0,     0     },

                //  record name
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":*",              ":*",                 1,     1     },
                { L_,  ":e",              ":e",                 1,     1     },
                { L_,  ":a",              ":e",                 1,     1     },
                { L_,  ":a",              ":*",                 1,     1     },
                { L_,  ":*",              ":e",                 1,     1     },

                //  field type
                { L_,  ":*A*",            ":*A*",               1,     1     },
                { L_,  ":*A*",            ":*B*",               0,     0     },

                //  field name
                { L_,  ":*Aa",            ":*Aa",               1,     1     },
                { L_,  ":*A*",            ":*A*",               1,     1     },
                { L_,  ":*Ae",            ":*Ae",               1,     1     },
                { L_,  ":*Aa",            ":*Ae",               1,     1     },
                { L_,  ":*Aa",            ":*A*",               1,     1     },
                { L_,  ":*A*",            ":*Ae",               1,     1     },

                //  field formatting mode
                { L_,  ":*A*&F0",         ":*A*",               1,     1     },
                { L_,  ":*A*&FX",         ":*A*",               1,     1     },
                { L_,  ":*A*&FX",         ":*A*&FX",            1,     1     },
                { L_,  ":*A*&FX",         ":*A*&FB",            1,     1     },

                //  field nullability ('U' == 'LIST')
                { L_,  ":*A*&ND",         ":*A*",               1,     1     },
                { L_,  ":*A*&NF",         ":*A*",               1,     1     },
                { L_,  ":*A*&NT",         ":*A*",               0,     0     },
                { L_,  ":*A*&NF",         ":*A*&NT",            0,     0     },

                { L_,  ":*U*&ND",         ":*U*",               1,     1     },
                { L_,  ":*U*&NF",         ":*U*",               1,     1     },
                { L_,  ":*U*&NT",         ":*U*",               0,     0     },
                { L_,  ":*U*&NF",         ":*U*&NT",            0,     0     },

                //  field default value
                { L_,  ":*A*&D0",         ":*A*",               1,     1     },
                { L_,  ":*A*&D0",         ":*A*&D0",            1,     1     },
                { L_,  ":*A*&D0",         ":*A*&D1",            1,     1     },

                //  field id (1-field and 2-field records)
                { L_,  ":*A*&I0",         ":*A*",               1,     1     },
                { L_,  ":*A*&I0",         ":*A*&I0",            1,     1     },
                { L_,  ":*A*&I3",         ":*A*",               1,     1     },
                { L_,  ":*A*&I3",         ":*A*&I3",            1,     1     },
                { L_,  ":*A*&I3",         ":*A*&I2",            1,     1     },
                { L_,  ":*A*&I3",         ":*A*&I0",            1,     1     },

                                                                     // DEPTH 0
                { L_,  ":*Aa   Bb   ",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 1
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 2
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I3Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I2",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I2Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I2",       1,     1     },

                { L_,  ":*Aa   Bb&I0",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa&I3Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I3",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I1Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I3Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I3",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I2Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I2",       1,     1     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb&I1",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 3
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I1Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I2Bb   ",       1,     1     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I1",       1,     1     },

                                                                     // DEPTH 4
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I1",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I2",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I2Bb&I1",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb&I0",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb&I1",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I2",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I0Bb&I2",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I3",       1,     1     },

            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_PAIRS; ++ti) {
                const int   LINE  = DATA[ti].d_lineNumber;
                const char *SPEC1 = DATA[ti].d_spec1;
                const char *SPEC2 = DATA[ti].d_spec2;
                const int   EXP12 = DATA[ti].d_exp12;
                const int   EXP21 = DATA[ti].d_exp21;

                const Schema S1(g(SPEC1));  const RecDef& R1 = S1.record(0);
                const Schema S2(g(SPEC2));  const RecDef& R2 = S2.record(0);
  //--------^
  LOOP3_ASSERT(LINE, SPEC1, SPEC2, EXP12 == areStructurallyEquivalent(R1, R2));
  LOOP3_ASSERT(LINE, SPEC1, SPEC2, EXP21 == areStructurallyEquivalent(R2, R1));
  LOOP3_ASSERT(LINE, SPEC1, SPEC2, 1     == areStructurallyEquivalent(R1, R1));
  LOOP3_ASSERT(LINE, SPEC1, SPEC2, 1     == areStructurallyEquivalent(R2, R2));
  //--------v
            }
        }

        if (verbose) cout << "Testing SEQUENCE records." << endl;
        {
            static const char* SPEC =
                ":a                          :*"
                ":bAa                        :*Aa"
                ":cA*                        :*A*"
                ":dAaBb                      :*AaBb"
                ":eA*B*                      :*A*B*"
                ":fBbAa                      :*BbAa"
                ":gAa+bd                     :*Aa+bd"
                ":hAa+bb#cg                  :*Aa+bb#cg"
                ":iAb+a3#cg                  :*Ab+a3#cg"
                ":jA*+*3#*h                  :*A*+*3#*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       0,     0       },
                {  L_,      4,    2,       1,     1       },
                {  L_,      6,    4,       0,     0       },
                {  L_,      8,    6,       1,     1       },
                {  L_,     10,    2,       0,     0       },

                {  L_,     10,    8,       0,     0       },
                {  L_,     12,    2,       0,     0       },
                {  L_,     12,    4,       0,     0       },
                {  L_,     14,    2,       0,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       1,     1       },
                {  L_,     18,    2,       0,     0       },
                {  L_,     18,   12,       0,     0       },
                {  L_,     18,   14,       0,     0       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

            //--^
            LOOP_ASSERT(LINE, EXP12 == areStructurallyEquivalent(r11, r12));
            LOOP_ASSERT(LINE, EXP12 == areStructurallyEquivalent(r11p1, r12));
            LOOP_ASSERT(LINE, EXP12 == areStructurallyEquivalent(r11, r12p1));
            LOOP_ASSERT(LINE, EXP12 == areStructurallyEquivalent(r11, r22));
            LOOP_ASSERT(LINE, EXP21 == areStructurallyEquivalent(r12, r11));
            LOOP_ASSERT(LINE, 1     == areStructurallyEquivalent(r11, r11p1));
            LOOP_ASSERT(LINE, 1     == areStructurallyEquivalent(r12, r12p1));
            LOOP_ASSERT(LINE, 1     == areStructurallyEquivalent(r11, r11));
            LOOP_ASSERT(LINE, 1     == areStructurallyEquivalent(r12, r12));
            //--v
            }
        }

        if (verbose) cout << "Testing CHOICE records." << endl;
        {
            static const char* SPEC =
                ":a?                          :*?"
                ":b?Aa                        :*?Aa"
                ":c?A*                        :*?A*"
                ":d?AaBb                      :*?AaBb"
                ":e?A*B*                      :*?A*B*"
                ":f?BbAa                      :*?BbAa"
                ":g?Aa%bd                     :*?Aa%bd"
                ":h?Aa%bb@cg                  :*?Aa%bb@cg"
                ":i?Ab%a3@cg                  :*?Ab%a3@cg"
                ":j?A*%*3@*h                  :*?A*%*3@*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       0,     0       },
                {  L_,      4,    2,       1,     1       },
                {  L_,      6,    4,       0,     0       },
                {  L_,      8,    6,       1,     1       },
                {  L_,     10,    2,       0,     0       },

                {  L_,     10,    8,       0,     0       },
                {  L_,     12,    2,       0,     0       },
                {  L_,     12,    4,       0,     0       },
                {  L_,     14,    2,       0,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       1,     1       },
                {  L_,     18,    2,       0,     0       },
                {  L_,     18,   12,       0,     0       },
                {  L_,     18,   14,       0,     0       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

            //--^
            LOOP_ASSERT(LINE, EXP12 == areStructurallyEquivalent(r11, r12));
            LOOP_ASSERT(LINE, EXP12 == areStructurallyEquivalent(r11p1, r12));
            LOOP_ASSERT(LINE, EXP12 == areStructurallyEquivalent(r11, r12p1));
            LOOP_ASSERT(LINE, EXP12 == areStructurallyEquivalent(r11, r22));
            LOOP_ASSERT(LINE, EXP21 == areStructurallyEquivalent(r12, r11));
            LOOP_ASSERT(LINE, 1     == areStructurallyEquivalent(r11, r11p1));
            LOOP_ASSERT(LINE, 1     == areStructurallyEquivalent(r12, r12p1));
            LOOP_ASSERT(LINE, 1     == areStructurallyEquivalent(r11, r11));
            LOOP_ASSERT(LINE, 1     == areStructurallyEquivalent(r12, r12));
            //--v
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'isSuperset' METHOD (for records)
        //
        // Concerns:
        //   For a record 'sup' to be an exact superset of a record 'sub', the
        //   following properties must hold.
        //     1. Every field in 'sub' has a corresponding field (i.e., same
        //        index) in 'sup'.
        //     2. Corresponding fields have the same type, the same name (or
        //        are both unnamed), and
        //          a. no constraint, or
        //          b. the same constraint index
        //     3. Record names do not affect this comparison.
        //
        // Plan:
        //   For the methods taking records as arguments, it is convenient to
        //   construct one schema containing N records configured appropriately
        //   for the tests, and then to construct a table containing pairs of
        //   record indices and expected results.
        //
        //   The "depth-ordered enumeration" for test data is achieved in this
        //   test, to a certain extent, by the order in which records are
        //   appended to the schema.  As an additional convenience, records are
        //   appended in pairs, where the odd-index record is identical to its
        //   predecessor except that it is unnamed.  The first six records have
        //   no constrained fields, and the next six do have constrained
        //   fields.
        //
        //   Test data is tabulated as a pair of record indices (i, j)
        //   representing records (ri, rj) in the one schema value used in this
        //   test.  The expected value is tabulated for the method call on
        //   (ri, rj) and on (rj, ri).
        //
        //   Since the record name is not relevant for this comparison method,
        //   every tabulated test is repeated using the records at indices
        //   (i + 1, j) and (i, j + 1) with the same expected results, and
        //   with (i, i + 1) and (i + 1, i) with an expected result of 1
        //   always.
        //
        //   To test for alias behavior, each tabulated test is performed
        //   twice, once on records from two distinct instances of the same
        //   schema value, and once on two records from the same schema
        //   instance.  The "identity" test is also performed on the input
        //   (ri, ri) -- i.e., the exact same record for both arguments.
        //
        // Testing:
        //   bool isSuperset(const record& super, const record& sub);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing isSuperset(Rec, Rec)"
                          << endl << "============================" << endl;

        if (verbose) cout << "Varying a single property or attribute." << endl;
        {
            const struct {
                int         d_lineNumber;  // line number
                const char *d_spec1;       // 'gg' spec 1
                const char *d_spec2;       // 'gg' spec 2
                int         d_exp12;       // expected result for x1 op x2
                int         d_exp21;       // expected result for x2 op x1
            } DATA[] = {
                //line spec1              spec2                 expected
                //---- -----              -----                 --------
                //  record type
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":a?",             ":a?",                1,     1     },
                { L_,  ":a?",             ":a",                 0,     0     },

                //  record name
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":*",              ":*",                 1,     1     },
                { L_,  ":e",              ":e",                 1,     1     },
                { L_,  ":a",              ":e",                 1,     1     },
                { L_,  ":a",              ":*",                 1,     1     },
                { L_,  ":*",              ":e",                 1,     1     },

                //  field type
                { L_,  ":*A*",            ":*A*",               1,     1     },
                { L_,  ":*A*",            ":*B*",               0,     0     },

                //  field name
                { L_,  ":*Aa",            ":*Aa",               1,     1     },
                { L_,  ":*A*",            ":*A*",               1,     1     },
                { L_,  ":*Ae",            ":*Ae",               1,     1     },
                { L_,  ":*Aa",            ":*Ae",               0,     0     },
                { L_,  ":*Aa",            ":*A*",               0,     0     },
                { L_,  ":*A*",            ":*Ae",               0,     0     },

                //  field formatting mode
                { L_,  ":*A*&F0",         ":*A*",               1,     1     },
                { L_,  ":*A*&FX",         ":*A*",               0,     0     },
                { L_,  ":*A*&FX",         ":*A*&FX",            1,     1     },
                { L_,  ":*A*&FX",         ":*A*&FB",            0,     0     },

                //  field nullability ('U' == 'LIST')
                { L_,  ":*A*&ND",         ":*A*",               1,     1     },
                { L_,  ":*A*&NF",         ":*A*",               1,     1     },
                { L_,  ":*A*&NT",         ":*A*",               0,     0     },
                { L_,  ":*A*&NF",         ":*A*&NT",            0,     0     },

                { L_,  ":*U*&ND",         ":*U*",               1,     1     },
                { L_,  ":*U*&NF",         ":*U*",               1,     1     },
                { L_,  ":*U*&NT",         ":*U*",               0,     0     },
                { L_,  ":*U*&NF",         ":*U*&NT",            0,     0     },

                //  field default value
                { L_,  ":*A*&D0",         ":*A*",               0,     0     },
                { L_,  ":*A*&D0",         ":*A*&D0",            1,     1     },
                { L_,  ":*A*&D0",         ":*A*&D1",            0,     0     },

                //  field id (1-field and 2-field records)
                { L_,  ":*A*&I0",         ":*A*",               1,     1     },
                { L_,  ":*A*&I0",         ":*A*&I0",            1,     1     },
                { L_,  ":*A*&I3",         ":*A*",               0,     0     },
                { L_,  ":*A*&I3",         ":*A*&I3",            1,     1     },
                { L_,  ":*A*&I3",         ":*A*&I2",            0,     0     },
                { L_,  ":*A*&I3",         ":*A*&I0",            0,     0     },

                                                                     // DEPTH 0
                { L_,  ":*Aa   Bb   ",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 1
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb   ",       0,     0     },

                                                                     // DEPTH 2
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I2",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I2Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I2",       0,     0     },

                { L_,  ":*Aa   Bb&I0",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I3",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I3",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I2Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I2",       1,     1     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb&I1",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa   Bb   ",       0,     0     },

                                                                     // DEPTH 3
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I2Bb   ",       0,     0     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I1",       0,     0     },

                                                                     // DEPTH 4
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I1",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I2",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I2Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb&I0",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I2",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I0Bb&I2",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I3",       0,     0     },

            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_PAIRS; ++ti) {
                const int   LINE  = DATA[ti].d_lineNumber;
                const char *SPEC1 = DATA[ti].d_spec1;
                const char *SPEC2 = DATA[ti].d_spec2;
                const int   EXP12 = DATA[ti].d_exp12;
                const int   EXP21 = DATA[ti].d_exp21;

                const Schema S1(g(SPEC1));  const RecDef& R1 = S1.record(0);
                const Schema S2(g(SPEC2));  const RecDef& R2 = S2.record(0);

                LOOP3_ASSERT(LINE, SPEC1, SPEC2, EXP12 == isSuperset(R1, R2));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2, EXP21 == isSuperset(R2, R1));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2, 1     == isSuperset(R1, R1));
                LOOP3_ASSERT(LINE, SPEC1, SPEC2, 1     == isSuperset(R2, R2));
            }
        }

        if (verbose) cout << "Testing SEQUENCE records." << endl;
        {
            static const char* SPEC =
                ":a                          :*"
                ":bAa                        :*Aa"
                ":cA*                        :*A*"
                ":dAaBb                      :*AaBb"
                ":eA*B*                      :*A*B*"
                ":fBbAa                      :*BbAa"
                ":gAa+bd                     :*Aa+bd"
                ":hAa+bb#cg                  :*Aa+bb#cg"
                ":iAb+a3#cg                  :*Ab+a3#cg"
                ":jA*+*3#*h                  :*A*+*3#*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       1,     0       },
                {  L_,      4,    2,       0,     0       },
                {  L_,      6,    4,       0,     0       },
                {  L_,      8,    6,       0,     0       },
                {  L_,     10,    2,       0,     0       },

                {  L_,     10,    8,       0,     0       },
                {  L_,     12,    2,       1,     0       },
                {  L_,     12,    4,       0,     0       },
                {  L_,     14,    2,       1,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       0,     0       },
                {  L_,     18,    2,       0,     0       },
                {  L_,     18,   12,       0,     0       },
                {  L_,     18,   14,       0,     0       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

                LOOP_ASSERT(LINE, EXP12 == isSuperset(r11, r12));
                LOOP_ASSERT(LINE, EXP12 == isSuperset(r11p1, r12));
                LOOP_ASSERT(LINE, EXP12 == isSuperset(r11, r12p1));
                LOOP_ASSERT(LINE, EXP12 == isSuperset(r11, r22));
                LOOP_ASSERT(LINE, EXP21 == isSuperset(r12, r11));
                LOOP_ASSERT(LINE, 1     == isSuperset(r11, r11p1));
                LOOP_ASSERT(LINE, 1     == isSuperset(r12, r12p1));
                LOOP_ASSERT(LINE, 1     == isSuperset(r11, r11));
                LOOP_ASSERT(LINE, 1     == isSuperset(r12, r12));
            }
        }

        if (verbose) cout << "Testing CHOICE records." << endl;
        {
            static const char* SPEC =
                ":a?                          :*?"
                ":b?Aa                        :*?Aa"
                ":c?A*                        :*?A*"
                ":d?AaBb                      :*?AaBb"
                ":e?A*B*                      :*?A*B*"
                ":f?BbAa                      :*?BbAa"
                ":g?Aa%bd                     :*?Aa%bd"
                ":h?Aa%bb@cg                  :*?Aa%bb@cg"
                ":i?Ab%a3@cg                  :*?Ab%a3@cg"
                ":j?A*%*3@*h                  :*?A*%*3@*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       1,     0       },
                {  L_,      4,    2,       0,     0       },
                {  L_,      6,    4,       0,     0       },
                {  L_,      8,    6,       0,     0       },
                {  L_,     10,    2,       0,     0       },

                {  L_,     10,    8,       0,     0       },
                {  L_,     12,    2,       1,     0       },
                {  L_,     12,    4,       0,     0       },
                {  L_,     14,    2,       1,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       0,     0       },
                {  L_,     18,    2,       0,     0       },
                {  L_,     18,   12,       0,     0       },
                {  L_,     18,   14,       0,     0       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

                LOOP_ASSERT(LINE, EXP12 == isSuperset(r11, r12));
                LOOP_ASSERT(LINE, EXP12 == isSuperset(r11p1, r12));
                LOOP_ASSERT(LINE, EXP12 == isSuperset(r11, r12p1));
                LOOP_ASSERT(LINE, EXP12 == isSuperset(r11, r22));
                LOOP_ASSERT(LINE, EXP21 == isSuperset(r12, r11));
                LOOP_ASSERT(LINE, 1     == isSuperset(r11, r11p1));
                LOOP_ASSERT(LINE, 1     == isSuperset(r12, r12p1));
                LOOP_ASSERT(LINE, 1     == isSuperset(r11, r11));
                LOOP_ASSERT(LINE, 1     == isSuperset(r12, r12));
            }
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'isSymbolicSuperset' METHOD WITH MASK (for records)
        //
        // Concerns:
        //   For a record 'sup' to be a symbolic superset of a record 'sub',
        //   the following properties must hold.
        //     2. Every *named* field in 'sub' has a corresponding field (i.e.,
        //        having the same name) in 'sup'.
        //     3. Corresponding fields have the same type and
        //          a. no constraint, or
        //          b. the same constraint index
        //     4. Field *indices* and unnamed fields do not affect the
        //        comparison.
        //
        // Plan:
        //   For the methods taking records as arguments, it is convenient to
        //   construct one schema containing N records configured appropriately
        //   for the tests, and then to construct a table containing pairs of
        //   record indices and expected results.
        //
        //   The "depth-ordered enumeration" for test data is achieved in this
        //   test, to a certain extent, by the order in which records are
        //   appended to the schema.  As an additional convenience, records are
        //   appended in pairs, where the odd-index record is identical to its
        //   predecessor except that it is unnamed.  The first six records have
        //   no constrained fields, and the next six do have constrained
        //   fields.
        //
        //   Test data is tabulated as a pair of record indices (i, j)
        //   representing records (ri, rj) in the one schema value used in this
        //   test.  The expected value is tabulated for the method call on
        //   (ri, rj) and on (rj, ri).
        //
        //   Since the record name is not relevant for this comparison method,
        //   every tabulated test is repeated using the records at indices
        //   (i + 1, j) and (i, j + 1) with the same expected results, and
        //   with (i, i + 1) and (i + 1, i) with an expected result of 1
        //   always.
        //
        //   To test for alias behavior, each tabulated test is performed
        //   twice, once on records from two distinct instances of the same
        //   schema value, and once on two records from the same schema
        //   instance.  The "identity" test is also performed on the input
        //   (ri, ri) -- i.e., the exact same record for both arguments.
        //
        // Testing:
        //   bool isSymbolicSuperset(& superRecord, & subRecord);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing isSymbolicSuperset()"
                          << endl << "============================" << endl;

        if (verbose) cout << "Varying a single property or attribute." << endl;
        {
            const struct {
                int         d_lineNumber;  // line number
                const char *d_spec1;       // 'gg' spec 1
                const char *d_spec2;       // 'gg' spec 2
                int         d_exp12;       // expected result for x1 op x2
                int         d_exp21;       // expected result for x2 op x1
            } DATA[] = {
                //line spec1              spec2                 expected
                //---- -----              -----                 --------
                //  record type
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":a?",             ":a?",                1,     1     },
                { L_,  ":a?",             ":a",                 0,     0     },

                //  record name
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":*",              ":*",                 1,     1     },
                { L_,  ":e",              ":e",                 1,     1     },
                { L_,  ":a",              ":e",                 1,     1     },
                { L_,  ":a",              ":*",                 1,     1     },
                { L_,  ":*",              ":e",                 1,     1     },
                { L_,  ":b",              ":b",                 1,     1     },

                //  field type
                { L_,  ":*Aa",            ":*Aa",               1,     1     },
                { L_,  ":*Aa",            ":*Ba",               0,     0     },

                //  field name
                { L_,  ":*Aa",            ":*Aa",               1,     1     },
                { L_,  ":*A*",            ":*A*",               1,     1     },
                { L_,  ":*Ae",            ":*Ae",               1,     1     },
                { L_,  ":*Aa",            ":*Ae",               0,     0     },
                { L_,  ":*Aa",            ":*A*",               1,     0     },
                { L_,  ":*A*",            ":*Ae",               0,     1     },
                { L_,  ":*Aa",            ":*Ab",               0,     0     },

                //  field nullability ('U' == 'LIST')
                { L_,  ":*Aa&ND",         ":*Aa",               1,     1     },
                { L_,  ":*Aa&NF",         ":*Aa",               1,     1     },
                { L_,  ":*Aa&NT",         ":*Aa",               0,     0     },
                { L_,  ":*Aa&NF",         ":*Aa&NT",            0,     0     },

                { L_,  ":*Ua&ND",         ":*Ua",               1,     1     },
                { L_,  ":*Ua&NF",         ":*Ua",               1,     1     },
                { L_,  ":*Ua&NT",         ":*Ua",               0,     0     },
                { L_,  ":*Ua&NF",         ":*Ua&NT",            0,     0     },

                //  field id (1-field and 2-field records)
                { L_,  ":*Aa&I0",         ":*Aa",               0,     0     },
                { L_,  ":*Aa&I0",         ":*Aa&I0",            1,     1     },
                { L_,  ":*Aa&I3",         ":*Aa",               0,     0     },
                { L_,  ":*Aa&I3",         ":*Aa&I3",            1,     1     },
                { L_,  ":*Aa&I3",         ":*Aa&I2",            0,     0     },
                { L_,  ":*Aa&I3",         ":*Aa&I0",            0,     0     },

                                                                     // DEPTH 0
                { L_,  ":*Aa   Bb   ",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 1
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb   ",       0,     0     },

                                                                     // DEPTH 2
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I2",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I2Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I2",       0,     0     },

                { L_,  ":*Aa   Bb&I0",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I3",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I3",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I2Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I2",       1,     1     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb&I1",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa   Bb   ",       0,     0     },

                                                                     // DEPTH 3
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I2Bb   ",       0,     0     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I1",       0,     0     },

                                                                     // DEPTH 4
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I1",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I2",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I2Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb&I0",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I2",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I0Bb&I2",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I3",       0,     0     },

            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_PAIRS; ++ti) {
                const int   LINE  = DATA[ti].d_lineNumber;
                const char *SPEC1 = DATA[ti].d_spec1;
                const char *SPEC2 = DATA[ti].d_spec2;
                const int   EXP12 = DATA[ti].d_exp12;
                const int   EXP21 = DATA[ti].d_exp21;

                const Schema S1(g(SPEC1));  const RecDef& R1 = S1.record(0);
                const Schema S2(g(SPEC2));  const RecDef& R2 = S2.record(0);

                for (int m = 0; m < NUM_MASKS; ++m) {
                    const int M = MASKS[m];

                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                 EXP12 == isSymbolicSuperset(R1, R2, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                 EXP21 == isSymbolicSuperset(R2, R1, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                 1     == isSymbolicSuperset(R1, R1, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                 1     == isSymbolicSuperset(R2, R2, M));
                }
            }
        }
        {
            const struct {
                int         d_lineNumber;  // line number
                const char *d_spec1;       // 'gg' spec 1
                const char *d_spec2;       // 'gg' spec 2
                int         d_exp[4];      // expected results per mask
            } DATA[] = {
                //line spec1             spec2                 expected
                //---- -----             -----            -----------------
                //  field formatting mode
                { L_,  ":*Aa&F0",        ":*Aa",          { 1,  1,  1,  1 }  },
                { L_,  ":*Aa&FX",        ":*Aa",          { 1,  1,  0,  0 }  },
                { L_,  ":*Aa&FX",        ":*Aa&FX",       { 1,  1,  1,  1 }  },
                { L_,  ":*Aa&FX",        ":*Aa&FB",       { 1,  1,  0,  0 }  },

                //  field default value
                { L_,  ":*Aa&D0",        ":*Aa",          { 1,  0,  1,  0 }  },
                { L_,  ":*Aa&D0",        ":*Aa&D0",       { 1,  1,  1,  1 }  },
                { L_,  ":*Aa&D0",        ":*Aa&D1",       { 1,  0,  1,  0 }  },

                //  combinations
                { L_,  ":*Aa&FL&D0",     ":*Aa&FL&D0",    { 1,  1,  1,  1 }  },
                { L_,  ":*Aa&FL&D0",     ":*Aa&FX&D0",    { 1,  1,  0,  0 }  },
                { L_,  ":*Aa&FL&D0",     ":*Aa&FL&D1",    { 1,  0,  1,  0 }  },
                { L_,  ":*Aa&FL&D0",     ":*Aa&FX&D1",    { 1,  0,  0,  0 }  },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_PAIRS; ++ti) {
                const int   LINE  = DATA[ti].d_lineNumber;
                const char *SPEC1 = DATA[ti].d_spec1;
                const char *SPEC2 = DATA[ti].d_spec2;
                const int  *EXP   = DATA[ti].d_exp;

                const Schema S1(g(SPEC1));  const RecDef& R1 = S1.record(0);
                const Schema S2(g(SPEC2));  const RecDef& R2 = S2.record(0);

                for (int m = 0; m < NUM_MASKS; ++m) {
                    const int M = MASKS[m];

                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                 EXP[m] == isSymbolicSuperset(R1, R2, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                 EXP[m] == isSymbolicSuperset(R2, R1, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                 1      == isSymbolicSuperset(R1, R1, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                 1      == isSymbolicSuperset(R2, R2, M));
                }
            }
        }

        if (verbose) cout << "Testing SEQUENCE records." << endl;
        {
            static const char* SPEC =
                ":a                          :*"
                ":bAa                        :*Aa"
                ":cA*                        :*A*"
                ":dAaBb                      :*AaBb"
                ":eA*B*                      :*A*B*"
                ":fBbAa                      :*BbAa"
                ":gAa+bd                     :*Aa+bd"
                ":hAa+bb#cg                  :*Aa+bb#cg"
                ":iAb+a3#cg                  :*Ab+a3#cg"
                ":jA*+*3#*h                  :*A*+*3#*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       1,     0       },
                {  L_,      4,    2,       0,     1       },
                {  L_,      6,    4,       1,     0       },
                {  L_,      8,    6,       0,     1       },
                {  L_,     10,    2,       1,     0       },

                {  L_,     10,    8,       1,     0       },
                {  L_,     12,    2,       1,     0       },
                {  L_,     12,    4,       1,     0       },
                {  L_,     14,    2,       1,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       0,     0       },
                {  L_,     18,    2,       0,     1       },
                {  L_,     18,   12,       0,     1       },
                {  L_,     18,   14,       0,     1       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

                for (int m = 0; m < NUM_MASKS; ++m) {
                    const int M = MASKS[m];

            //--^
            LOOP_ASSERT(LINE, EXP12 == isSymbolicSuperset(r11, r12, M));
            LOOP_ASSERT(LINE, EXP12 == isSymbolicSuperset(r11p1, r12, M));
            LOOP_ASSERT(LINE, EXP12 == isSymbolicSuperset(r11, r12p1, M));
            LOOP_ASSERT(LINE, EXP12 == isSymbolicSuperset(r11, r22, M));
            LOOP_ASSERT(LINE, EXP21 == isSymbolicSuperset(r12, r11, M));
            LOOP_ASSERT(LINE, 1     == isSymbolicSuperset(r11, r11p1, M));
            LOOP_ASSERT(LINE, 1     == isSymbolicSuperset(r12, r12p1, M));
            LOOP_ASSERT(LINE, 1     == isSymbolicSuperset(r11, r11, M));
            LOOP_ASSERT(LINE, 1     == isSymbolicSuperset(r12, r12, M));
            //--v
                }
            }
        }

        if (verbose) cout << "Testing CHOICE records." << endl;
        {
            static const char* SPEC =
                ":a?                          :*?"
                ":b?Aa                        :*?Aa"
                ":c?A*                        :*?A*"
                ":d?AaBb                      :*?AaBb"
                ":e?A*B*                      :*?A*B*"
                ":f?BbAa                      :*?BbAa"
                ":g?Aa%bd                     :*?Aa%bd"
                ":h?Aa%bb@cg                  :*?Aa%bb@cg"
                ":i?Ab%a3@cg                  :*?Ab%a3@cg"
                ":j?A*%*3@*h                  :*?A*%*3@*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       1,     0       },
                {  L_,      4,    2,       0,     1       },
                {  L_,      6,    4,       1,     0       },
                {  L_,      8,    6,       0,     1       },
                {  L_,     10,    2,       1,     0       },

                {  L_,     10,    8,       1,     0       },
                {  L_,     12,    2,       1,     0       },
                {  L_,     12,    4,       1,     0       },
                {  L_,     14,    2,       1,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       0,     0       },
                {  L_,     18,    2,       0,     1       },
                {  L_,     18,   12,       0,     1       },
                {  L_,     18,   14,       0,     1       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

                for (int m = 0; m < NUM_MASKS; ++m) {
                    const int M = MASKS[m];

            //--^
            LOOP_ASSERT(LINE, EXP12 == isSymbolicSuperset(r11, r12, M));
            LOOP_ASSERT(LINE, EXP12 == isSymbolicSuperset(r11p1, r12, M));
            LOOP_ASSERT(LINE, EXP12 == isSymbolicSuperset(r11, r12p1, M));
            LOOP_ASSERT(LINE, EXP12 == isSymbolicSuperset(r11, r22, M));
            LOOP_ASSERT(LINE, EXP21 == isSymbolicSuperset(r12, r11, M));
            LOOP_ASSERT(LINE, 1     == isSymbolicSuperset(r11, r11p1, M));
            LOOP_ASSERT(LINE, 1     == isSymbolicSuperset(r12, r12p1, M));
            LOOP_ASSERT(LINE, 1     == isSymbolicSuperset(r11, r11, M));
            LOOP_ASSERT(LINE, 1     == isSymbolicSuperset(r12, r12, M));
            //--v
                }
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'isSymbolicSuperset' METHOD (for records)
        //
        // Concerns:
        //   For a record 'sup' to be a symbolic superset of a record 'sub',
        //   the following properties must hold.
        //     2. Every *named* field in 'sub' has a corresponding field (i.e.,
        //        having the same name) in 'sup'.
        //     3. Corresponding fields have the same type and
        //          a. no constraint, or
        //          b. the same constraint index
        //     4. Field *indices* and unnamed fields do not affect the
        //        comparison.
        //
        // Plan:
        //   For the methods taking records as arguments, it is convenient to
        //   construct one schema containing N records configured appropriately
        //   for the tests, and then to construct a table containing pairs of
        //   record indices and expected results.
        //
        //   The "depth-ordered enumeration" for test data is achieved in this
        //   test, to a certain extent, by the order in which records are
        //   appended to the schema.  As an additional convenience, records are
        //   appended in pairs, where the odd-index record is identical to its
        //   predecessor except that it is unnamed.  The first six records have
        //   no constrained fields, and the next six do have constrained
        //   fields.
        //
        //   Test data is tabulated as a pair of record indices (i, j)
        //   representing records (ri, rj) in the one schema value used in this
        //   test.  The expected value is tabulated for the method call on
        //   (ri, rj) and on (rj, ri).
        //
        //   Since the record name is not relevant for this comparison method,
        //   every tabulated test is repeated using the records at indices
        //   (i + 1, j) and (i, j + 1) with the same expected results, and
        //   with (i, i + 1) and (i + 1, i) with an expected result of 1
        //   always.
        //
        //   To test for alias behavior, each tabulated test is performed
        //   twice, once on records from two distinct instances of the same
        //   schema value, and once on two records from the same schema
        //   instance.  The "identity" test is also performed on the input
        //   (ri, ri) -- i.e., the exact same record for both arguments.
        //
        // Testing:
        //   bool isSymbolicSuperset(& superRecord, & subRecord);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing isSymbolicSuperset()"
                          << endl << "============================" << endl;

        if (verbose) cout << "Varying a single property or attribute." << endl;
        {
            const struct {
                int         d_lineNumber;  // line number
                const char *d_spec1;       // 'gg' spec 1
                const char *d_spec2;       // 'gg' spec 2
                int         d_exp12;       // expected result for x1 op x2
                int         d_exp21;       // expected result for x2 op x1
            } DATA[] = {
                //line spec1              spec2                 expected
                //---- -----              -----                 --------
                //  record type
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":a?",             ":a?",                1,     1     },
                { L_,  ":a?",             ":a",                 0,     0     },

                //  record name
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":*",              ":*",                 1,     1     },
                { L_,  ":e",              ":e",                 1,     1     },
                { L_,  ":a",              ":e",                 1,     1     },
                { L_,  ":a",              ":*",                 1,     1     },
                { L_,  ":*",              ":e",                 1,     1     },
                { L_,  ":b",              ":b",                 1,     1     },

                //  field type
                { L_,  ":*Aa",            ":*Aa",               1,     1     },
                { L_,  ":*Aa",            ":*Ba",               0,     0     },

                //  field name
                { L_,  ":*Aa",            ":*Aa",               1,     1     },
                { L_,  ":*A*",            ":*A*",               1,     1     },
                { L_,  ":*Ae",            ":*Ae",               1,     1     },
                { L_,  ":*Aa",            ":*Ae",               0,     0     },
                { L_,  ":*Aa",            ":*A*",               1,     0     },
                { L_,  ":*A*",            ":*Ae",               0,     1     },
                { L_,  ":*Aa",            ":*Ab",               0,     0     },

                //  field formatting mode
                { L_,  ":*Aa&F0",         ":*Aa",               1,     1     },
                { L_,  ":*Aa&FX",         ":*Aa",               1,     1     },
                { L_,  ":*Aa&FX",         ":*Aa&FX",            1,     1     },
                { L_,  ":*Aa&FX",         ":*Aa&FB",            1,     1     },

                //  field nullability ('U' == 'LIST')
                { L_,  ":*Aa&ND",         ":*Aa",               1,     1     },
                { L_,  ":*Aa&NF",         ":*Aa",               1,     1     },
                { L_,  ":*Aa&NT",         ":*Aa",               0,     0     },
                { L_,  ":*Aa&NF",         ":*Aa&NT",            0,     0     },

                { L_,  ":*Ua&ND",         ":*Ua",               1,     1     },
                { L_,  ":*Ua&NF",         ":*Ua",               1,     1     },
                { L_,  ":*Ua&NT",         ":*Ua",               0,     0     },
                { L_,  ":*Ua&NF",         ":*Ua&NT",            0,     0     },

                //  field default value
                { L_,  ":*Aa&D0",         ":*Aa",               1,     1     },
                { L_,  ":*Aa&D0",         ":*Aa&D0",            1,     1     },
                { L_,  ":*Aa&D0",         ":*Aa&D1",            1,     1     },

                //  field id (1-field and 2-field records)
                { L_,  ":*Aa&I0",         ":*Aa",               0,     0     },
                { L_,  ":*Aa&I0",         ":*Aa&I0",            1,     1     },
                { L_,  ":*Aa&I3",         ":*Aa",               0,     0     },
                { L_,  ":*Aa&I3",         ":*Aa&I3",            1,     1     },
                { L_,  ":*Aa&I3",         ":*Aa&I2",            0,     0     },
                { L_,  ":*Aa&I3",         ":*Aa&I0",            0,     0     },

                                                                     // DEPTH 0
                { L_,  ":*Aa   Bb   ",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 1
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb   ",       0,     0     },

                                                                     // DEPTH 2
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I2",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I2Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I2",       0,     0     },

                { L_,  ":*Aa   Bb&I0",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I3",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I3Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I3",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I2Bb   ",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I2",       1,     1     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I2Bb&I1",    ":*Aa   Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa   Bb   ",       0,     0     },

                                                                     // DEPTH 3
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb   ",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I2Bb   ",       0,     0     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I0",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I1",       0,     0     },

                                                                     // DEPTH 4
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I1",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I2",       0,     0     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I2Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb&I0",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb&I1",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I2",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I0Bb&I2",       0,     0     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I3",       0,     0     },

            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_PAIRS; ++ti) {
                const int   LINE  = DATA[ti].d_lineNumber;
                const char *SPEC1 = DATA[ti].d_spec1;
                const char *SPEC2 = DATA[ti].d_spec2;
                const int   EXP12 = DATA[ti].d_exp12;
                const int   EXP21 = DATA[ti].d_exp21;

                const Schema S1(g(SPEC1));  const RecDef& R1 = S1.record(0);
                const Schema S2(g(SPEC2));  const RecDef& R2 = S2.record(0);

         //-^
         LOOP3_ASSERT(LINE, SPEC1, SPEC2, EXP12 == isSymbolicSuperset(R1, R2));
         LOOP3_ASSERT(LINE, SPEC1, SPEC2, EXP21 == isSymbolicSuperset(R2, R1));
         LOOP3_ASSERT(LINE, SPEC1, SPEC2, 1     == isSymbolicSuperset(R1, R1));
         LOOP3_ASSERT(LINE, SPEC1, SPEC2, 1     == isSymbolicSuperset(R2, R2));
         //-v
            }
        }

        if (verbose) cout << "Testing SEQUENCE records." << endl;
        {
            static const char* SPEC =
                ":a                          :*"
                ":bAa                        :*Aa"
                ":cA*                        :*A*"
                ":dAaBb                      :*AaBb"
                ":eA*B*                      :*A*B*"
                ":fBbAa                      :*BbAa"
                ":gAa+bd                     :*Aa+bd"
                ":hAa+bb#cg                  :*Aa+bb#cg"
                ":iAb+a3#cg                  :*Ab+a3#cg"
                ":jA*+*3#*h                  :*A*+*3#*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       1,     0       },
                {  L_,      4,    2,       0,     1       },
                {  L_,      6,    4,       1,     0       },
                {  L_,      8,    6,       0,     1       },
                {  L_,     10,    2,       1,     0       },

                {  L_,     10,    8,       1,     0       },
                {  L_,     12,    2,       1,     0       },
                {  L_,     12,    4,       1,     0       },
                {  L_,     14,    2,       1,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       0,     0       },
                {  L_,     18,    2,       0,     1       },
                {  L_,     18,   12,       0,     1       },
                {  L_,     18,   14,       0,     1       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

                LOOP_ASSERT(LINE, EXP12 == isSymbolicSuperset(r11, r12));
                LOOP_ASSERT(LINE, EXP12 == isSymbolicSuperset(r11p1, r12));
                LOOP_ASSERT(LINE, EXP12 == isSymbolicSuperset(r11, r12p1));
                LOOP_ASSERT(LINE, EXP12 == isSymbolicSuperset(r11, r22));
                LOOP_ASSERT(LINE, EXP21 == isSymbolicSuperset(r12, r11));
                LOOP_ASSERT(LINE, 1     == isSymbolicSuperset(r11, r11p1));
                LOOP_ASSERT(LINE, 1     == isSymbolicSuperset(r12, r12p1));
                LOOP_ASSERT(LINE, 1     == isSymbolicSuperset(r11, r11));
                LOOP_ASSERT(LINE, 1     == isSymbolicSuperset(r12, r12));
            }
        }

        if (verbose) cout << "Testing CHOICE records." << endl;
        {
            static const char* SPEC =
                ":a?                          :*?"
                ":b?Aa                        :*?Aa"
                ":c?A*                        :*?A*"
                ":d?AaBb                      :*?AaBb"
                ":e?A*B*                      :*?A*B*"
                ":f?BbAa                      :*?BbAa"
                ":g?Aa%bd                     :*?Aa%bd"
                ":h?Aa%bb@cg                  :*?Aa%bb@cg"
                ":i?Ab%a3@cg                  :*?Ab%a3@cg"
                ":j?A*%*3@*h                  :*?A*%*3@*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       1,     0       },
                {  L_,      4,    2,       0,     1       },
                {  L_,      6,    4,       1,     0       },
                {  L_,      8,    6,       0,     1       },
                {  L_,     10,    2,       1,     0       },

                {  L_,     10,    8,       1,     0       },
                {  L_,     12,    2,       1,     0       },
                {  L_,     12,    4,       1,     0       },
                {  L_,     14,    2,       1,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       0,     0       },
                {  L_,     18,    2,       0,     1       },
                {  L_,     18,   12,       0,     1       },
                {  L_,     18,   14,       0,     1       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

                LOOP_ASSERT(LINE, EXP12 == isSymbolicSuperset(r11, r12));
                LOOP_ASSERT(LINE, EXP12 == isSymbolicSuperset(r11p1, r12));
                LOOP_ASSERT(LINE, EXP12 == isSymbolicSuperset(r11, r12p1));
                LOOP_ASSERT(LINE, EXP12 == isSymbolicSuperset(r11, r22));
                LOOP_ASSERT(LINE, EXP21 == isSymbolicSuperset(r12, r11));
                LOOP_ASSERT(LINE, 1     == isSymbolicSuperset(r11, r11p1));
                LOOP_ASSERT(LINE, 1     == isSymbolicSuperset(r12, r12p1));
                LOOP_ASSERT(LINE, 1     == isSymbolicSuperset(r11, r11));
                LOOP_ASSERT(LINE, 1     == isSymbolicSuperset(r12, r12));
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'isStructuralSuperset' METHOD WITH MASK (for records)
        //
        // Concerns:
        //   For a record 'sup' to be an exact superset of a record 'sub', the
        //   following properties must hold.
        //     2. Every field in 'r1' has a corresponding field (i.e., same
        //        index) in 'r2'.
        //     3. Corresponding fields have the same type and
        //          a. no constraint, or
        //          b. the same constraint index
        //     4. Field *names* do not affect the comparison.
        //
        // Plan:
        //   For the methods taking records as arguments, it is convenient to
        //   construct one schema containing N records configured appropriately
        //   for the tests, and then to construct a table containing pairs of
        //   record indices and expected results.
        //
        //   The "depth-ordered enumeration" for test data is achieved in this
        //   test, to a certain extent, by the order in which records are
        //   appended to the schema.  As an additional convenience, records are
        //   appended in pairs, where the odd-index record is identical to its
        //   predecessor except that it is unnamed.  The first six records have
        //   no constrained fields, and the next six do have constrained
        //   fields.
        //
        //   Test data is tabulated as a pair of record indices (i, j)
        //   representing records (ri, rj) in the one schema value used in this
        //   test.  The expected value is tabulated for the method call on
        //   (ri, rj) and on (rj, ri).
        //
        //   Since the record name is not relevant for this comparison method,
        //   every tabulated test is repeated using the records at indices
        //   (i + 1, j) and (i, j + 1) with the same expected results, and
        //   with (i, i + 1) and (i + 1, i) with an expected result of 1
        //   always.
        //
        //   To test for alias behavior, each tabulated test is performed
        //   twice, once on records from two distinct instances of the same
        //   schema value, and once on two records from the same schema
        //   instance.  The "identity" test is also performed on the input
        //   (ri, ri) -- i.e., the exact same record for both arguments.
        //
        // Testing:
        //   bool isStructuralSuperset(& superRecord, & subRecord);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing isStructuralSuperset()"
                          << endl << "==============================" << endl;

        if (verbose) cout << "Varying a single property or attribute." << endl;
        {
            const struct {
                int         d_lineNumber;  // line number
                const char *d_spec1;       // 'gg' spec 1
                const char *d_spec2;       // 'gg' spec 2
                int         d_exp12;       // expected result for x1 op x2
                int         d_exp21;       // expected result for x2 op x1
            } DATA[] = {
                //line spec1              spec2                 expected
                //---- -----              -----                 --------
                //  record type
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":a?",             ":a?",                1,     1     },
                { L_,  ":a?",             ":a",                 0,     0     },

                //  record name
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":*",              ":*",                 1,     1     },
                { L_,  ":e",              ":e",                 1,     1     },
                { L_,  ":a",              ":e",                 1,     1     },
                { L_,  ":a",              ":*",                 1,     1     },
                { L_,  ":*",              ":e",                 1,     1     },

                //  field type
                { L_,  ":*A*",            ":*A*",               1,     1     },
                { L_,  ":*A*",            ":*B*",               0,     0     },

                //  field name
                { L_,  ":*Aa",            ":*Aa",               1,     1     },
                { L_,  ":*A*",            ":*A*",               1,     1     },
                { L_,  ":*Ae",            ":*Ae",               1,     1     },
                { L_,  ":*Aa",            ":*Ae",               1,     1     },
                { L_,  ":*Aa",            ":*A*",               1,     1     },
                { L_,  ":*A*",            ":*Ae",               1,     1     },

                //  field nullability ('U' == 'LIST')
                { L_,  ":*A*&ND",         ":*A*",               1,     1     },
                { L_,  ":*A*&NF",         ":*A*",               1,     1     },
                { L_,  ":*A*&NT",         ":*A*",               0,     0     },
                { L_,  ":*A*&NF",         ":*A*&NT",            0,     0     },

                { L_,  ":*U*&ND",         ":*U*",               1,     1     },
                { L_,  ":*U*&NF",         ":*U*",               1,     1     },
                { L_,  ":*U*&NT",         ":*U*",               0,     0     },
                { L_,  ":*U*&NF",         ":*U*&NT",            0,     0     },

                //  field id (1-field and 2-field records)
                { L_,  ":*A*&I0",         ":*A*",               1,     1     },
                { L_,  ":*A*&I0",         ":*A*&I0",            1,     1     },
                { L_,  ":*A*&I3",         ":*A*",               1,     1     },
                { L_,  ":*A*&I3",         ":*A*&I3",            1,     1     },
                { L_,  ":*A*&I3",         ":*A*&I2",            1,     1     },
                { L_,  ":*A*&I3",         ":*A*&I0",            1,     1     },

                                                                     // DEPTH 0
                { L_,  ":*Aa   Bb   ",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 1
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 2
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I3Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I2",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I2Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I2",       1,     1     },

                { L_,  ":*Aa   Bb&I0",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa&I3Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I3",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I1Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I3Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I3",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I2Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I2",       1,     1     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb&I1",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 3
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I1Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I2Bb   ",       1,     1     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I1",       1,     1     },

                                                                     // DEPTH 4
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I1",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I2",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I2Bb&I1",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb&I0",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb&I1",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I2",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I0Bb&I2",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I3",       1,     1     },

            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_PAIRS; ++ti) {
                const int   LINE  = DATA[ti].d_lineNumber;
                const char *SPEC1 = DATA[ti].d_spec1;
                const char *SPEC2 = DATA[ti].d_spec2;
                const int   EXP12 = DATA[ti].d_exp12;
                const int   EXP21 = DATA[ti].d_exp21;

                const Schema S1(g(SPEC1));  const RecDef& R1 = S1.record(0);
                const Schema S2(g(SPEC2));  const RecDef& R2 = S2.record(0);

                for (int m = 0; m < NUM_MASKS; ++m) {
                    const int M = MASKS[m];

                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                 EXP12 == isStructuralSuperset(R1, R2, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                 EXP21 == isStructuralSuperset(R2, R1, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                 1     == isStructuralSuperset(R1, R1, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                 1     == isStructuralSuperset(R2, R2, M));
                }
            }
        }
        {
            const struct {
                int         d_lineNumber;  // line number
                const char *d_spec1;       // 'gg' spec 1
                const char *d_spec2;       // 'gg' spec 2
                int         d_exp[4];      // expected results per mask
            } DATA[] = {
                //line spec1             spec2                 expected
                //---- -----             -----            -----------------
                //  field formatting mode
                { L_,  ":*A*&F0",        ":*A*",          { 1,  1,  1,  1 }  },
                { L_,  ":*A*&FX",        ":*A*",          { 1,  1,  0,  0 }  },
                { L_,  ":*A*&FX",        ":*A*&FX",       { 1,  1,  1,  1 }  },
                { L_,  ":*A*&FX",        ":*A*&FB",       { 1,  1,  0,  0 }  },

                //  field default value
                { L_,  ":*A*&D0",        ":*A*",          { 1,  0,  1,  0 }  },
                { L_,  ":*A*&D0",        ":*A*&D0",       { 1,  1,  1,  1 }  },
                { L_,  ":*A*&D0",        ":*A*&D1",       { 1,  0,  1,  0 }  },

                //  combinations
                { L_,  ":*A*&FL&D0",     ":*A*&FL&D0",    { 1,  1,  1,  1 }  },
                { L_,  ":*A*&FL&D0",     ":*A*&FX&D0",    { 1,  1,  0,  0 }  },
                { L_,  ":*A*&FL&D0",     ":*A*&FL&D1",    { 1,  0,  1,  0 }  },
                { L_,  ":*A*&FL&D0",     ":*A*&FX&D1",    { 1,  0,  0,  0 }  },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_PAIRS; ++ti) {
                const int   LINE  = DATA[ti].d_lineNumber;
                const char *SPEC1 = DATA[ti].d_spec1;
                const char *SPEC2 = DATA[ti].d_spec2;
                const int  *EXP   = DATA[ti].d_exp;

                const Schema S1(g(SPEC1));  const RecDef& R1 = S1.record(0);
                const Schema S2(g(SPEC2));  const RecDef& R2 = S2.record(0);

                for (int m = 0; m < NUM_MASKS; ++m) {
                    const int M = MASKS[m];

                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                 EXP[m] == isStructuralSuperset(R1, R2, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                 EXP[m] == isStructuralSuperset(R2, R1, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                 1      == isStructuralSuperset(R1, R1, M));
                    LOOP3_ASSERT(LINE, SPEC1, SPEC2,
                                 1      == isStructuralSuperset(R2, R2, M));
                }
            }
        }

        if (verbose) cout << "Testing SEQUENCE records." << endl;
        {
            static const char* SPEC =
                ":a                          :*"
                ":bAa                        :*Aa"
                ":cA*                        :*A*"
                ":dAaBb                      :*AaBb"
                ":eA*B*                      :*A*B*"
                ":fBbAa                      :*BbAa"
                ":gAa+bd                     :*Aa+bd"
                ":hAa+bb#cg                  :*Aa+bb#cg"
                ":iAb+a3#cg                  :*Ab+a3#cg"
                ":jA*+*3#*h                  :*A*+*3#*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       1,     0       },
                {  L_,      4,    2,       1,     1       },
                {  L_,      6,    4,       1,     0       },
                {  L_,      8,    6,       1,     1       },
                {  L_,     10,    2,       0,     0       },

                {  L_,     10,    8,       0,     0       },
                {  L_,     12,    2,       1,     0       },
                {  L_,     12,    4,       1,     0       },
                {  L_,     14,    2,       1,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       1,     1       },
                {  L_,     18,    2,       1,     0       },
                {  L_,     18,   12,       0,     0       },
                {  L_,     18,   14,       0,     0       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

                for (int m = 0; m < NUM_MASKS; ++m) {
                    const int M = MASKS[m];

            //--^
            LOOP_ASSERT(LINE, EXP12 == isStructuralSuperset(r11, r12, M));
            LOOP_ASSERT(LINE, EXP12 == isStructuralSuperset(r11p1, r12, M));
            LOOP_ASSERT(LINE, EXP12 == isStructuralSuperset(r11, r12p1, M));
            LOOP_ASSERT(LINE, EXP12 == isStructuralSuperset(r11, r22, M));
            LOOP_ASSERT(LINE, EXP21 == isStructuralSuperset(r12, r11, M));
            LOOP_ASSERT(LINE, 1     == isStructuralSuperset(r11, r11p1, M));
            LOOP_ASSERT(LINE, 1     == isStructuralSuperset(r12, r12p1, M));
            LOOP_ASSERT(LINE, 1     == isStructuralSuperset(r11, r11, M));
            LOOP_ASSERT(LINE, 1     == isStructuralSuperset(r12, r12, M));
            //--v
                }
            }
        }

        if (verbose) cout << "Testing CHOICE records." << endl;
        {
            static const char* SPEC =
                ":a?                          :*?"
                ":b?Aa                        :*?Aa"
                ":c?A*                        :*?A*"
                ":d?AaBb                      :*?AaBb"
                ":e?A*B*                      :*?A*B*"
                ":f?BbAa                      :*?BbAa"
                ":g?Aa%bd                     :*?Aa%bd"
                ":h?Aa%bb@cg                  :*?Aa%bb@cg"
                ":i?Ab%a3@cg                  :*?Ab%a3@cg"
                ":j?A*%*3@*h                  :*?A*%*3@*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       1,     0       },
                {  L_,      4,    2,       1,     1       },
                {  L_,      6,    4,       1,     0       },
                {  L_,      8,    6,       1,     1       },
                {  L_,     10,    2,       0,     0       },

                {  L_,     10,    8,       0,     0       },
                {  L_,     12,    2,       1,     0       },
                {  L_,     12,    4,       1,     0       },
                {  L_,     14,    2,       1,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       1,     1       },
                {  L_,     18,    2,       1,     0       },
                {  L_,     18,   12,       0,     0       },
                {  L_,     18,   14,       0,     0       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

                for (int m = 0; m < NUM_MASKS; ++m) {
                    const int M = MASKS[m];

            //--^
            LOOP_ASSERT(LINE, EXP12 == isStructuralSuperset(r11, r12, M));
            LOOP_ASSERT(LINE, EXP12 == isStructuralSuperset(r11p1, r12, M));
            LOOP_ASSERT(LINE, EXP12 == isStructuralSuperset(r11, r12p1, M));
            LOOP_ASSERT(LINE, EXP12 == isStructuralSuperset(r11, r22, M));
            LOOP_ASSERT(LINE, EXP21 == isStructuralSuperset(r12, r11, M));
            LOOP_ASSERT(LINE, 1     == isStructuralSuperset(r11, r11p1, M));
            LOOP_ASSERT(LINE, 1     == isStructuralSuperset(r12, r12p1, M));
            LOOP_ASSERT(LINE, 1     == isStructuralSuperset(r11, r11, M));
            LOOP_ASSERT(LINE, 1     == isStructuralSuperset(r12, r12, M));
            //--v
                }
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'isStructuralSuperset' METHOD (for records)
        //
        // Concerns:
        //   For a record 'sup' to be an exact superset of a record 'sub', the
        //   following properties must hold.
        //     2. Every field in 'r1' has a corresponding field (i.e., same
        //        index) in 'r2'.
        //     3. Corresponding fields have the same type and
        //          a. no constraint, or
        //          b. the same constraint index
        //     4. Field *names* do not affect the comparison.
        //
        // Plan:
        //   For the methods taking records as arguments, it is convenient to
        //   construct one schema containing N records configured appropriately
        //   for the tests, and then to construct a table containing pairs of
        //   record indices and expected results.
        //
        //   The "depth-ordered enumeration" for test data is achieved in this
        //   test, to a certain extent, by the order in which records are
        //   appended to the schema.  As an additional convenience, records are
        //   appended in pairs, where the odd-index record is identical to its
        //   predecessor except that it is unnamed.  The first six records have
        //   no constrained fields, and the next six do have constrained
        //   fields.
        //
        //   Test data is tabulated as a pair of record indices (i, j)
        //   representing records (ri, rj) in the one schema value used in this
        //   test.  The expected value is tabulated for the method call on
        //   (ri, rj) and on (rj, ri).
        //
        //   Since the record name is not relevant for this comparison method,
        //   every tabulated test is repeated using the records at indices
        //   (i + 1, j) and (i, j + 1) with the same expected results, and
        //   with (i, i + 1) and (i + 1, i) with an expected result of 1
        //   always.
        //
        //   To test for alias behavior, each tabulated test is performed
        //   twice, once on records from two distinct instances of the same
        //   schema value, and once on two records from the same schema
        //   instance.  The "identity" test is also performed on the input
        //   (ri, ri) -- i.e., the exact same record for both arguments.
        //
        // Testing:
        //   bool isStructuralSuperset(& superRecord, & subRecord);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing isStructuralSuperset()"
                          << endl << "==============================" << endl;

        if (verbose) cout << "Varying a single property or attribute." << endl;
        {
            const struct {
                int         d_lineNumber;  // line number
                const char *d_spec1;       // 'gg' spec 1
                const char *d_spec2;       // 'gg' spec 2
                int         d_exp12;       // expected result for x1 op x2
                int         d_exp21;       // expected result for x2 op x1
            } DATA[] = {
                //line spec1              spec2                 expected
                //---- -----              -----                 --------
                //  record type
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":a?",             ":a?",                1,     1     },
                { L_,  ":a?",             ":a",                 0,     0     },

                //  record name
                { L_,  ":a",              ":a",                 1,     1     },
                { L_,  ":*",              ":*",                 1,     1     },
                { L_,  ":e",              ":e",                 1,     1     },
                { L_,  ":a",              ":e",                 1,     1     },
                { L_,  ":a",              ":*",                 1,     1     },
                { L_,  ":*",              ":e",                 1,     1     },

                //  field type
                { L_,  ":*A*",            ":*A*",               1,     1     },
                { L_,  ":*A*",            ":*B*",               0,     0     },

                //  field name
                { L_,  ":*Aa",            ":*Aa",               1,     1     },
                { L_,  ":*A*",            ":*A*",               1,     1     },
                { L_,  ":*Ae",            ":*Ae",               1,     1     },
                { L_,  ":*Aa",            ":*Ae",               1,     1     },
                { L_,  ":*Aa",            ":*A*",               1,     1     },
                { L_,  ":*A*",            ":*Ae",               1,     1     },

                //  field formatting mode
                { L_,  ":*A*&F0",         ":*A*",               1,     1     },
                { L_,  ":*A*&FX",         ":*A*",               1,     1     },
                { L_,  ":*A*&FX",         ":*A*&FX",            1,     1     },
                { L_,  ":*A*&FX",         ":*A*&FB",            1,     1     },

                //  field nullability ('U' == 'LIST')
                { L_,  ":*A*&ND",         ":*A*",               1,     1     },
                { L_,  ":*A*&NF",         ":*A*",               1,     1     },
                { L_,  ":*A*&NT",         ":*A*",               0,     0     },
                { L_,  ":*A*&NF",         ":*A*&NT",            0,     0     },

                { L_,  ":*U*&ND",         ":*U*",               1,     1     },
                { L_,  ":*U*&NF",         ":*U*",               1,     1     },
                { L_,  ":*U*&NT",         ":*U*",               0,     0     },
                { L_,  ":*U*&NF",         ":*U*&NT",            0,     0     },

                //  field default value
                { L_,  ":*A*&D0",         ":*A*",               1,     1     },
                { L_,  ":*A*&D0",         ":*A*&D0",            1,     1     },
                { L_,  ":*A*&D0",         ":*A*&D1",            1,     1     },

                //  field id (1-field and 2-field records)
                { L_,  ":*A*&I0",         ":*A*",               1,     1     },
                { L_,  ":*A*&I0",         ":*A*&I0",            1,     1     },
                { L_,  ":*A*&I3",         ":*A*",               1,     1     },
                { L_,  ":*A*&I3",         ":*A*&I3",            1,     1     },
                { L_,  ":*A*&I3",         ":*A*&I2",            1,     1     },
                { L_,  ":*A*&I3",         ":*A*&I0",            1,     1     },

                                                                     // DEPTH 0
                { L_,  ":*Aa   Bb   ",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 1
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 2
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa&I3Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa&I0Bb   ",    ":*Aa   Bb&I2",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa&I2Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa&I2Bb   ",    ":*Aa   Bb&I2",       1,     1     },

                { L_,  ":*Aa   Bb&I0",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa&I3Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa   Bb&I0",    ":*Aa   Bb&I3",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I1Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa&I3Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa   Bb&I1",    ":*Aa   Bb&I3",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa&I2Bb   ",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa   Bb&I2",    ":*Aa   Bb&I2",       1,     1     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I2Bb&I1",    ":*Aa   Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa   Bb   ",       1,     1     },

                                                                     // DEPTH 3
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I1Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb   ",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I2Bb   ",       1,     1     },

                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa   Bb&I1",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I0",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa   Bb&I1",       1,     1     },

                                                                     // DEPTH 4
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I1",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I0Bb&I2",       1,     1     },
                { L_,  ":*Aa&I0Bb&I1",    ":*Aa&I2Bb&I1",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I1Bb&I0",       1,     1     },
                { L_,  ":*Aa&I1Bb&I0",    ":*Aa&I0Bb&I1",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I2",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I0Bb&I2",       1,     1     },
                { L_,  ":*Aa&I1Bb&I2",    ":*Aa&I1Bb&I3",       1,     1     },

            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_PAIRS; ++ti) {
                const int   LINE  = DATA[ti].d_lineNumber;
                const char *SPEC1 = DATA[ti].d_spec1;
                const char *SPEC2 = DATA[ti].d_spec2;
                const int   EXP12 = DATA[ti].d_exp12;
                const int   EXP21 = DATA[ti].d_exp21;

                const Schema S1(g(SPEC1));  const RecDef& R1 = S1.record(0);
                const Schema S2(g(SPEC2));  const RecDef& R2 = S2.record(0);

       //---^
       LOOP3_ASSERT(LINE, SPEC1, SPEC2, EXP12 == isStructuralSuperset(R1, R2));
       LOOP3_ASSERT(LINE, SPEC1, SPEC2, EXP21 == isStructuralSuperset(R2, R1));
       LOOP3_ASSERT(LINE, SPEC1, SPEC2, 1     == isStructuralSuperset(R1, R1));
       LOOP3_ASSERT(LINE, SPEC1, SPEC2, 1     == isStructuralSuperset(R2, R2));
       //---v
            }
        }

        if (verbose) cout << "Testing SEQUENCE records." << endl;
        {
            static const char* SPEC =
                ":a                          :*"
                ":bAa                        :*Aa"
                ":cA*                        :*A*"
                ":dAaBb                      :*AaBb"
                ":eA*B*                      :*A*B*"
                ":fBbAa                      :*BbAa"
                ":gAa+bd                     :*Aa+bd"
                ":hAa+bb#cg                  :*Aa+bb#cg"
                ":iAb+a3#cg                  :*Ab+a3#cg"
                ":jA*+*3#*h                  :*A*+*3#*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       1,     0       },
                {  L_,      4,    2,       1,     1       },
                {  L_,      6,    4,       1,     0       },
                {  L_,      8,    6,       1,     1       },
                {  L_,     10,    2,       0,     0       },

                {  L_,     10,    8,       0,     0       },
                {  L_,     12,    2,       1,     0       },
                {  L_,     12,    4,       1,     0       },
                {  L_,     14,    2,       1,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       1,     1       },
                {  L_,     18,    2,       1,     0       },
                {  L_,     18,   12,       0,     0       },
                {  L_,     18,   14,       0,     0       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

                LOOP_ASSERT(LINE, EXP12 == isStructuralSuperset(r11, r12));
                LOOP_ASSERT(LINE, EXP12 == isStructuralSuperset(r11p1, r12));
                LOOP_ASSERT(LINE, EXP12 == isStructuralSuperset(r11, r12p1));
                LOOP_ASSERT(LINE, EXP12 == isStructuralSuperset(r11, r22));
                LOOP_ASSERT(LINE, EXP21 == isStructuralSuperset(r12, r11));
                LOOP_ASSERT(LINE, 1     == isStructuralSuperset(r11, r11p1));
                LOOP_ASSERT(LINE, 1     == isStructuralSuperset(r12, r12p1));
                LOOP_ASSERT(LINE, 1     == isStructuralSuperset(r11, r11));
                LOOP_ASSERT(LINE, 1     == isStructuralSuperset(r12, r12));
            }
        }

        if (verbose) cout << "Testing CHOICE records." << endl;
        {
            static const char* SPEC =
                ":a?                          :*?"
                ":b?Aa                        :*?Aa"
                ":c?A*                        :*?A*"
                ":d?AaBb                      :*?AaBb"
                ":e?A*B*                      :*?A*B*"
                ":f?BbAa                      :*?BbAa"
                ":g?Aa%bd                     :*?Aa%bd"
                ":h?Aa%bb@cg                  :*?Aa%bb@cg"
                ":i?Ab%a3@cg                  :*?Ab%a3@cg"
                ":j?A*%*3@*h                  :*?A*%*3@*h";

            const struct {
                int d_lineNumber;
                int d_idx1;
                int d_idx2;
                int d_exp12;
                int d_exp21;
            } DATA[] = {
                //        Rec Index   expected  results
                //Line    idx1  idx2     exp12  exp21
                //----    ----  ----     -----  ----
                {  L_,      2,    0,       1,     0       },
                {  L_,      4,    2,       1,     1       },
                {  L_,      6,    4,       1,     0       },
                {  L_,      8,    6,       1,     1       },
                {  L_,     10,    2,       0,     0       },

                {  L_,     10,    8,       0,     0       },
                {  L_,     12,    2,       1,     0       },
                {  L_,     12,    4,       1,     0       },
                {  L_,     14,    2,       1,     0       },
                {  L_,     14,   12,       0,     0       },

                {  L_,     16,   12,       0,     0       },
                {  L_,     16,   14,       1,     1       },
                {  L_,     18,    2,       1,     0       },
                {  L_,     18,   12,       0,     0       },
                {  L_,     18,   14,       0,     0       },
            };
            const int NUM_PAIRS = sizeof DATA / sizeof *DATA;

            const Schema S1(g(SPEC));
            const Schema S2(S1);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                const int     LINE  = DATA[i].d_lineNumber;
                const RecDef& r11   = S1.record(DATA[i].d_idx1);
                const RecDef& r12   = S1.record(DATA[i].d_idx2);
                const RecDef& r11p1 = S1.record(DATA[i].d_idx1 + 1);
                const RecDef& r12p1 = S1.record(DATA[i].d_idx2 + 1);
                const RecDef& r22   = S2.record(DATA[i].d_idx2);
                const int     EXP12 = DATA[i].d_exp12;
                const int     EXP21 = DATA[i].d_exp21;

                LOOP_ASSERT(LINE, EXP12 == isStructuralSuperset(r11, r12));
                LOOP_ASSERT(LINE, EXP12 == isStructuralSuperset(r11p1, r12));
                LOOP_ASSERT(LINE, EXP12 == isStructuralSuperset(r11, r12p1));
                LOOP_ASSERT(LINE, EXP12 == isStructuralSuperset(r11, r22));
                LOOP_ASSERT(LINE, EXP21 == isStructuralSuperset(r12, r11));
                LOOP_ASSERT(LINE, 1     == isStructuralSuperset(r11, r11p1));
                LOOP_ASSERT(LINE, 1     == isStructuralSuperset(r12, r12p1));
                LOOP_ASSERT(LINE, 1     == isStructuralSuperset(r11, r11));
                LOOP_ASSERT(LINE, 1     == isStructuralSuperset(r12, r12));
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'g'
        //   <Test taken from 'bdem_schema.t.cpp' case 11>
        //   'g' is implemented using 'gg'.  It is sufficient to test that 'gg'
        //   and 'g' produce the same values for a few test specs.
        //
        // Plan:
        //   Test that' g' results in schemas equal to those generated by 'gg'.
        //
        // Testing
        //   bdem_Schema g(const char *spec)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting g"
                          << "\n=========" << endl;

        if (verbose) cout << "\nCompare values produced by 'g' and 'gg' on "
                          << "various inputs." << endl;

        for (int i = 0; i < NUM_SPECS; ++i) {
            const char *spec = testSpecs[i];
            if (veryVerbose) P(spec);

            Schema x(Z);  const Schema& X = x;  gg(&x, spec);
            if (veryVerbose) { PS(g(spec)); PS(X); }

            LOOP_ASSERT(i, g(spec) == X);
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'gg' GENERATOR:
        //   <Test taken from 'bdem_schema.t.cpp' case 3>
        //   The 'gg' generator is implemented as a finite state machine (FSM).
        //   All symbols must be recognized successfully, and all possible
        //   state transitions must be verified.
        //
        // Plan:
        //   Evaluate a series of test strings of increasing complexity to
        //   validate every major state and transition in the FSM underlying
        //   gg.  Add extra tests to validate less likely transitions until
        //   they are all covered.
        //
        // Testing:
        //   bdem_Schema& gg(bdem_Schema *object, const char *spec)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'gg'"
                          << "\n============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n Empty string" << endl;

        Schema x1(Z);  const Schema& X1 = x1;
        {
            gg(&x1, "");  ASSERT(0 == X1.numRecords());
        }

        const int NUM_TYPES = EType::BDEM_NUM_TYPES;

        {
            if (verbose)
                cout << "\nTesting 'gg' with all unconstrained types." << endl;

            int i;  // loop variables, declared here to keep MSVC++ happy
            for (i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :* %*"; // The % gets overwritten.
                spec[5] = bdemType[i];

                if (veryVerbose) cout << "testing 'gg' using " << spec << endl;

                gg(&x1, spec);
                LOOP_ASSERT(i, 1 == X1.numRecords());

                const RecDef *rec = &X1.record(0);

                LOOP_ASSERT(i, 0 == recName(*rec));

                LOOP_ASSERT(i, 1 == rec->numFields());
                LOOP_ASSERT(i, 0 == rec->fieldName(0));
                LOOP_ASSERT(i, ggElemTypes[i] == rec->field(0).elemType());
                LOOP_ASSERT(i, 0 == rec->field(0).recordConstraint());

                if (veryVerbose) PS(X1);
            }

            if (verbose) cout << "\nAll unconstrained types, with name c "
                              << "(\"c\")." << endl;
            for (i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :* %c"; // The % gets overwritten.
                spec[5] = bdemType[i];

                if (veryVerbose) cout << "testing 'gg' using " << spec << endl;

                gg(&x1, spec);
                LOOP_ASSERT(i, 1 == X1.numRecords());

                const RecDef *rec = &X1.record(0);

                LOOP_ASSERT(i, 0 == recName(*rec));

                LOOP_ASSERT(i, 1 == rec->numFields());
                LOOP_ASSERT(i, 0 == strCmp(rec->fieldName(0), "c"));
                LOOP_ASSERT(i, ggElemTypes[i] == rec->field(0).elemType());
                LOOP_ASSERT(i, 0 == rec->field(0).recordConstraint());

                if (veryVerbose)  PS(X1);
            }

            if (verbose) cout << "\nAll unconstrained types, with name c "
                              << "(\"c\") and record name d (\"d\")." << endl;
            for (i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :d %c"; // The % gets overwritten.
                spec[5] = bdemType[i];

                if (veryVerbose) cout << "testing 'gg' using " << spec << endl;

                gg(&x1, spec);
                LOOP_ASSERT(i, 1 == X1.numRecords());

                const RecDef *rec = &X1.record(0);

                LOOP_ASSERT(i, 0 == strCmp(recName(*rec), "d"));
                LOOP_ASSERT(i, 1 == rec->numFields());
                LOOP_ASSERT(i, 0 == strCmp(rec->fieldName(0), "c"));
                LOOP_ASSERT(i, ggElemTypes[i] == rec->field(0).elemType());
                LOOP_ASSERT(i, 0 == rec->field(0).recordConstraint());

                if (veryVerbose) PS(X1);
            }

            if (verbose)
                cout << "\nTesting 'gg' with all constrained types." << endl;
            for (i = 0; aggBdemType[i]; ++i) {
                static char spec[] = "~ :c% Ad :* %dc"; // '%' get replaced
                const char bdemType = aggBdemType[i];
                spec[4]  = '%' == bdemType || '@' == bdemType ? '?' : ' ';
                spec[12] = bdemType;

                if (veryVerbose) cout << "testing 'gg' using " << spec << endl;

                gg(&x1, spec);
                LOOP_ASSERT(i, 2 == X1.numRecords());

                const RecDef *rec = &X1.record(0);

                LOOP_ASSERT(i, 0 == strCmp(recName(*rec), "c"));
                LOOP_ASSERT(i, 1 == rec->numFields());
                LOOP_ASSERT(i, 0 == strCmp(rec->fieldName(0), "d"));
                LOOP_ASSERT(i, ggElemTypes[0] == rec->field(0).elemType());

                rec = &X1.record(1);

                LOOP_ASSERT(i, 0 == recName(*rec));
                LOOP_ASSERT(i, 1 == rec->numFields());
                LOOP_ASSERT(i, 0 == strCmp(rec->fieldName(0), "d"));
                LOOP_ASSERT(i, &X1.record(0) ==
                                           rec->field(0).recordConstraint());
                ASSERT(ggElemTypes[32 + i] == rec->field(0).elemType());

                if (veryVerbose)  PS(X1);
            }

            if (verbose) cout << "\nTesting 'gg' with a large invocation "
                              << "touching every token." << endl;
            {
                // Test all the cases, in one big 'gg' invocation, including
                // the cross-product of all possible constraints.

                bsl::string spec = "~";

                int i, j;  // loop variables declared here to keep MSVC++ happy
                for (i = 0; i < NUM_GG_NAMES; ++i) {
                    spec += "\n";
                    spec += ":";
                    spec += name[i];
                    if (i % 2) {
                        spec += "?";  // odd 'i' are CHOICE records
                    }
                    for (j = 0; j < NUM_TYPES; ++j) {
                        spec += " ";
                        spec += bdemType[j];
                        if (j < NUM_GG_NAMES) {
                            spec += name[j];
                        }
                        else {
                            // test space transition in state FLD_NAME.
                            spec += " *";
                        }
                    }

                    // Add constrained flds by name on previous record.
                    if (i > 1) {
                        if (i % 2) {
                            spec += " +*";
                            spec += name[i - 1];

                            spec += " #*";
                            spec += name[i - 1];
                        }
                        else {
                            spec += " %*";
                            spec += name[i - 1];

                            spec += " @*";
                            spec += name[i - 1];
                        }
                    }

                    // Add constrained flds by index on all previous records,
                    // and on current record.  Note that this can only
                    // reference first 10 records, since we use a single digit
                    // for reference by index.
                    for (j = 0; j <= i && j <= 9; ++j) {
                        if (j % 2) {
                            // Trailing space to exercise space transition in
                            // state CFLDCONSTR.
                            spec += " %* ";
                            spec += char(j + '0');

                            // Inner space to exercise space transition in
                            // state CFLDNAME.
                            spec += " @ * ";
                            spec += char(j + '0');
                        }
                        else {
                            spec += " +* ";
                            spec += char(j + '0');

                            spec += " # *";
                            spec += char(j + '0');
                        }
                    }
                }

                if (verbose) cout << "Testing 'gg' using " << spec << endl;

                gg(&x1, spec.c_str());
                ASSERT(X1.numRecords() == NUM_GG_NAMES);

                if (veryVerbose)  PS(X1);

                for (i = 0; i < NUM_GG_NAMES; ++i) {
                    const RecDef *R = &X1.record(i);
                    ASSERT(0 != R);
                    ASSERT(0 == strCmp(recName(*R), ggName(i)));

                    for (j = 0; j < NUM_TYPES; ++j) {
                        ASSERT(ggElemTypes[j] == R->field(j).elemType());
                        ASSERT(0 == strCmp(R->fieldName(j), ggName(j)));
                        ASSERT(0 == R->field(j).recordConstraint());
                    }

                    if (i > 1) {
                        // Two fields constrained by name on previous record.
                        int k = i % 2 ? 0 : 2;  // "+#" or "%@"
                        for (j = NUM_TYPES; j < NUM_TYPES + 1; ++j, ++k) {
                            ASSERT(getType(constrBdemType[k]) ==
                                                       R->field(j).elemType());
                            ASSERT(0 != R->field(j).recordConstraint());
                            ASSERT(0 ==
                               strCmp(recName(*R->field(j).recordConstraint()),
                                      ggName(i - 1)));
                        }
                    }

                    // Constrained fields by index on previous records and on
                    // current record (up to at most 9 previous records).
                    int f = i > 1 ? NUM_TYPES + 2 : NUM_TYPES;
                    for (j = 0; j <= i && j <= 9; ++j, f += 2) {
                        int k = j % 2 ? 2 : 0;  // "%@" or "+#"
                        ASSERT(getType(constrBdemType[k]) ==
                                                       R->field(f).elemType());
                        ASSERT(0 != R->field(f).recordConstraint());
                        ASSERT(0 ==
                             strCmp(recName(*R->field(f).recordConstraint()),
                                           ggName(j)));
                        ASSERT(getType(constrBdemType[k + 1]) ==
                                                   R->field(f + 1).elemType());
                        ASSERT(0 != R->field(f).recordConstraint());
                        ASSERT(0 ==
                           strCmp(recName(*R->field(f + 1).recordConstraint()),
                                  ggName(j)));
                    }
                }
            }

            {
                Schema x1(Z);  const Schema& X1 = x1;

                if (verbose) cout << "\nForce a test of name transition in "
                                  << "state REC_NAME." << endl;
                gg(&x1, ":a");

                ASSERT(1 == X1.numRecords());
                ASSERT(0 == strCmp(recName(X1.record(0)), "a"));

                if (verbose) cout << "\nMake sure 'gg' is additive if '~' is "
                                  << "not present, although this is NOT "
                                  << "normal usage." << endl;
                gg(&x1, ":b");

                ASSERT(2 == X1.numRecords());
                ASSERT(0 == strCmp(recName(X1.record(0)), "a"));
                ASSERT(0 == strCmp(recName(X1.record(1)), "b"));
            }

            {
                Schema x1(Z);  const Schema& X1 = x1;

                if (verbose) cout << "\nForce a test of remove transition in "
                                  << "state FLD_END." << endl;
                gg(&x1, ":aAa~:a");

                ASSERT(1 == X1.numRecords());
                ASSERT(0 == strCmp(recName(X1.record(0)), "a"));
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        // Concerns:
        //   We are concerned that the basic functionality of
        //   'bdem_schemautil' operates sanely.
        //
        // Plan:
        //   Create simple 'bdem_Schema' objects (using default constructor),
        //   append records (and fields to the records, and check that the
        //   results are as expected.
        //
        //   The exercises:
        //      1) Create schemas and test for equality/containment.
        //
        //      2) Create empty records and test for equality/containment.
        //
        //      3) Populate records and test for equality/containment.
        //
        //
        // Testing:
        //   This breathing test exercises basic functionality as a preliminary
        //   to the thorough testing done in subsequent cases.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        const RecType CHOICE = RecDef::BDEM_CHOICE_RECORD;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Exercising schema methods." << endl;

        if (verbose) cout << "    SEQUENCE records." << endl;
        {
            Schema s1, s2;
            ASSERT(1 == isSuperset(s1, s1));
            ASSERT(1 == isSuperset(s1, s2));

            RecDef *r1 = s1.createRecord("Rec");

            ASSERT(1 == isSuperset(s1, s2));

            r1->appendField(EType::BDEM_INT, "Int");

            ASSERT(1 == isSuperset(s1, s2));
            ASSERT(0 == isSuperset(s2, s1));

            RecDef *r2 = s2.createRecord("Rec");
            r2->appendField(EType::BDEM_INT, "Int");

            ASSERT(1 == isSuperset(s2, s1));
            ASSERT(1 == isSuperset(*r2, *r1));

            r2->appendField(EType::BDEM_DATE, "Date");

            ASSERT(1 == isSuperset(s2, s1));
            ASSERT(0 == isSuperset(s1, s2));
            ASSERT(1 == isSuperset(*r2, *r1));
            ASSERT(0 == isSuperset(*r1, *r2));

            r1->appendField(EType::BDEM_DATE, "Date");

            ASSERT(1 == isSuperset(s1, s2));
            ASSERT(1 == isSuperset(s2, s1));
            ASSERT(1 == isSuperset(*r2, *r1));
            ASSERT(1 == isSuperset(*r1, *r2));

            EnumDef *e1 = s1.createEnumeration("Colors");
            e1->addEnumerator("blue");
            e1->addEnumerator("green");
            e1->addEnumerator("red");

            ASSERT(1 == isSuperset(s1, s2));
            ASSERT(0 == isSuperset(s2, s1));

            EnumDef *e2 = s2.createEnumeration("Colors");
            e2->addEnumerator("blue");
            e2->addEnumerator("green");
            e2->addEnumerator("red");

            ASSERT(1 == isSuperset(s1, s2));
            ASSERT(1 == isSuperset(s2, s1));

            r1->appendField(EType::BDEM_INT, e1, "Int2");

            ASSERT(1 == isSuperset(s1, s2));
            ASSERT(0 == isSuperset(s2, s1));

            r2->appendField(EType::BDEM_INT, e2, "Int2");

            ASSERT(1 == isSuperset(s1, s2));
            ASSERT(1 == isSuperset(s2, s1));
        }

        if (verbose) cout << "    CHOICE records." << endl;
        {
            Schema s1, s2;
            ASSERT(1 == isSuperset(s1, s1));
            ASSERT(1 == isSuperset(s1, s2));

            RecDef *r1 = s1.createRecord("Rec", CHOICE);

            ASSERT(1 == isSuperset(s1, s2));

            r1->appendField(EType::BDEM_INT, "Int");

            ASSERT(1 == isSuperset(s1, s2));
            ASSERT(0 == isSuperset(s2, s1));

            RecDef *r2 = s2.createRecord("Rec", CHOICE);
            r2->appendField(EType::BDEM_INT, "Int");

            ASSERT(1 == isSuperset(s2, s1));
            ASSERT(1 == isSuperset(*r2, *r1));

            r2->appendField(EType::BDEM_DATE, "Date");

            ASSERT(1 == isSuperset(s2, s1));
            ASSERT(0 == isSuperset(s1, s2));
            ASSERT(1 == isSuperset(*r2, *r1));
            ASSERT(0 == isSuperset(*r1, *r2));

            r1->appendField(EType::BDEM_DATE, "Date");

            ASSERT(1 == isSuperset(s1, s2));
            ASSERT(1 == isSuperset(s2, s1));
            ASSERT(1 == isSuperset(*r2, *r1));
            ASSERT(1 == isSuperset(*r1, *r2));

            EnumDef *e1 = s1.createEnumeration("Colors");
            e1->addEnumerator("blue");
            e1->addEnumerator("green");
            e1->addEnumerator("red");

            ASSERT(1 == isSuperset(s1, s2));
            ASSERT(0 == isSuperset(s2, s1));

            EnumDef *e2 = s2.createEnumeration("Colors");
            e2->addEnumerator("blue");
            e2->addEnumerator("green");
            e2->addEnumerator("red");

            ASSERT(1 == isSuperset(s1, s2));
            ASSERT(1 == isSuperset(s2, s1));

            r1->appendField(EType::BDEM_INT, e1, "Int2");

            ASSERT(1 == isSuperset(s1, s2));
            ASSERT(0 == isSuperset(s2, s1));

            r2->appendField(EType::BDEM_INT, e2, "Int2");

            ASSERT(1 == isSuperset(s1, s2));
            ASSERT(1 == isSuperset(s2, s1));
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Empty records." << endl;

        if (verbose) cout << "    SEQUENCE records." << endl;
        {
            Schema s1;
            ASSERT(1 == isSuperset (s1, s1));

            Schema s2;
            ASSERT(1 == isSuperset (s1, s2));

            RecDef *r1 = s1.createRecord("Rec");
            ASSERT(1 == areEquivalent(*r1, *r1));
            ASSERT(1 == areSymbolicallyEquivalent(*r1, *r1));
            ASSERT(1 == areStructurallyEquivalent(*r1, *r1));
            ASSERT(1 == isSuperset(*r1, *r1));
            ASSERT(1 == isSymbolicSuperset(*r1, *r1));
            ASSERT(1 == isStructuralSuperset(*r1, *r1));

            RecDef *r2 = s2.createRecord("Rec");
            ASSERT(1 == areEquivalent(*r1, *r2));
            ASSERT(1 == areSymbolicallyEquivalent(*r1, *r2));
            ASSERT(1 == areStructurallyEquivalent(*r1, *r2));
            ASSERT(1 == isSuperset(*r1, *r2));
            ASSERT(1 == isSymbolicSuperset(*r1, *r2));
            ASSERT(1 == isStructuralSuperset(*r1, *r2));

            EnumDef *e1 = s1.createEnumeration("Enum");
            ASSERT(1 == areEquivalent(*e1, *e1));
            ASSERT(1 == isSuperset(*e1, *e1));

            EnumDef *e2 = s2.createEnumeration("Enum");
            ASSERT(1 == areEquivalent(*e1, *e2));
            ASSERT(1 == isSuperset(*e1, *e2));
        }

        if (verbose) cout << "    CHOICE records." << endl;
        {
            Schema s1;
            ASSERT(1 == isSuperset (s1, s1));

            Schema s2;
            ASSERT(1 == isSuperset (s1, s2));

            RecDef *r1 = s1.createRecord("Rec", CHOICE);
            ASSERT(1 == areEquivalent(*r1, *r1));
            ASSERT(1 == areSymbolicallyEquivalent(*r1, *r1));
            ASSERT(1 == areStructurallyEquivalent(*r1, *r1));
            ASSERT(1 == isSuperset(*r1, *r1));
            ASSERT(1 == isSymbolicSuperset(*r1, *r1));
            ASSERT(1 == isStructuralSuperset(*r1, *r1));

            RecDef *r2 = s2.createRecord("Rec", CHOICE);
            ASSERT(1 == areEquivalent(*r1, *r2));
            ASSERT(1 == areSymbolicallyEquivalent(*r1, *r2));
            ASSERT(1 == areStructurallyEquivalent(*r1, *r2));
            ASSERT(1 == isSuperset(*r1, *r2));
            ASSERT(1 == isSymbolicSuperset(*r1, *r2));
            ASSERT(1 == isStructuralSuperset(*r1, *r2));

            EnumDef *e1 = s1.createEnumeration("Enum");
            ASSERT(1 == areEquivalent(*e1, *e1));
            ASSERT(1 == isSuperset(*e1, *e1));

            EnumDef *e2 = s2.createEnumeration("Enum");
            ASSERT(1 == areEquivalent(*e1, *e2));
            ASSERT(1 == isSuperset(*e1, *e2));
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Populated records." << endl;

        if (verbose) cout << "    SEQUENCE records." << endl;
        {
            Schema superSet;
            Schema subSet;
            Schema symbSubSet;

            EnumDef *superEnum = superSet.createEnumeration("SuperEnum");
            superEnum->addEnumerator("Enum1", 1);
            superEnum->addEnumerator("Enum2");

            EnumDef *subEnum = subSet.createEnumeration("SubEnum");
            subEnum->addEnumerator("Enum1", 1);

            RecDef *r11  = superSet.createRecord("Rec1");
            r11->appendField(EType::BDEM_INT, "Int");
            r11->appendField(EType::BDEM_DATE, "Date");
            r11->appendField(EType::BDEM_INT, superEnum, "IntEnum1");

            RecDef *r12 = superSet.createRecord("Rec2");
            r12->appendField(EType::BDEM_INT,"Int");
            r12->appendField(EType::BDEM_DATE,"Date");
            r12->appendField(EType::BDEM_INT, superEnum, "IntEnum1");

            RecDef *r14 = superSet.createRecord("Rec4");
            r14->appendField(EType::BDEM_LIST, r11, "List");

            RecDef *r15 = superSet.createRecord("Rec5");
            r15->appendField(EType::BDEM_INT, superEnum, "IntEnum1");

            RecDef *r16 = superSet.createRecord("Rec6");
            {
                FldAttr fa(EType::BDEM_INT);
                fa.defaultValue().theModifiableInt() = 6;
                r16->appendField(fa, "Int1");
            }

            RecDef *r17 = superSet.createRecord("Rec7");
            {
                FldAttr fa(EType::BDEM_INT);
                fa.defaultValue().theModifiableInt() = 7;
                r17->appendField(fa, "Int1");
            }

            RecDef *r18 = superSet.createRecord("Rec8");
            {
                FldAttr fa(EType::BDEM_INT_ARRAY);
                fa.defaultValue().theModifiableInt() = 6;
                r18->appendField(fa, "IntArray1");
            }

            RecDef *r19 = superSet.createRecord("Rec9");
            {
                FldAttr fa(EType::BDEM_INT_ARRAY);
                fa.defaultValue().theModifiableInt() = 7;
                r19->appendField(fa, "IntArray1");
            }

            RecDef *r21 = subSet.createRecord("Rec1");
            r21->appendField(EType::BDEM_INT,"Int");

            RecDef *r22 = subSet.createRecord("Rec2");
            r22->appendField(EType::BDEM_INT,"Int");
            r22->appendField(EType::BDEM_INT, subEnum, "IntEnum1");

            RecDef *r23 = subSet.createRecord("Rec3");
            r23->appendField(EType::BDEM_INT,"Int");
            r23->appendField(EType::BDEM_DATE,"Date");
            r23->appendField(EType::BDEM_INT, subEnum, "IntEnum1");

            RecDef *r24 = subSet.createRecord("Rec4");
            r24->appendField(EType::BDEM_LIST, "List");

            // enum where we deliberately forgot to constrain
            RecDef *r25 = subSet.createRecord("Rec5");
            r25->appendField(EType::BDEM_INT, "IntEnum1");

            RecDef *r26 = subSet.createRecord("Rec6");
            r26->appendField(EType::BDEM_INT, "Int1");

            RecDef *r28 = subSet.createRecord("Rec8");
            r28->appendField(EType::BDEM_INT_ARRAY, "IntArray1");

            // TBD: Are the next two lines needed?
            //RecDef *r32 = symbSubSet.createRecord("Rec2");
            //r32->appendField(EType::BDEM_DATE,"Date");

            ASSERT(1 == areEquivalent(*r11, *r12));
            ASSERT(1 == areSymbolicallyEquivalent(*r11, *r12));
            ASSERT(1 == areStructurallyEquivalent(*r11, *r12));
            ASSERT(1 == isSuperset(*r11, *r12));
            ASSERT(1 == isSymbolicSuperset(*r11, *r12));
            ASSERT(1 == isStructuralSuperset(*r11, *r12));

            ASSERT(0 == areEquivalent(*r11, *r21));
            ASSERT(0 == areSymbolicallyEquivalent(*r11, *r21));
            ASSERT(0 == areStructurallyEquivalent(*r11, *r21));
            ASSERT(1 == isSuperset(*r11, *r21));
            ASSERT(1 == isSymbolicSuperset(*r11, *r21));
            ASSERT(1 == isStructuralSuperset(*r11, *r21));

            ASSERT(0 == areEquivalent(*r11, *r22));
            ASSERT(0 == areSymbolicallyEquivalent(*r11, *r22));
            ASSERT(0 == areStructurallyEquivalent(*r11, *r22));
            ASSERT(0 == isSuperset(*r11, *r22));
            ASSERT(1 == isSymbolicSuperset(*r11, *r22));
            ASSERT(0 == isStructuralSuperset(*r11, *r22));

            ASSERT(0 == areEquivalent(*r14, *r24));
            ASSERT(0 == areEquivalent(*r24, *r14));
            ASSERT(0 == areSymbolicallyEquivalent(*r14, *r24));
            ASSERT(0 == areSymbolicallyEquivalent(*r24, *r14));
            ASSERT(0 == areStructurallyEquivalent(*r14, *r24));
            ASSERT(0 == areStructurallyEquivalent(*r24, *r14));
            ASSERT(1 == isSuperset(*r14, *r24));
            ASSERT(0 == isSuperset(*r24, *r14));
            ASSERT(1 == isSymbolicSuperset(*r14, *r24));
            ASSERT(0 == isSymbolicSuperset(*r24, *r14));
            ASSERT(1 == isStructuralSuperset(*r14, *r24));
            ASSERT(0 == isStructuralSuperset(*r24, *r14));

            const int enumMask = Util::BDEM_CHECK_ENUMERATION;
            ASSERT(0 == areEquivalent(*r15, *r25));
            ASSERT(0 == areEquivalent(*r25, *r15));
            ASSERT(1 == areSymbolicallyEquivalent(*r15, *r25));
            ASSERT(1 == areSymbolicallyEquivalent(*r25, *r15));
            ASSERT(0 == areSymbolicallyEquivalent(*r15, *r25, enumMask));
            ASSERT(0 == areSymbolicallyEquivalent(*r25, *r15, enumMask));
            ASSERT(1 == areStructurallyEquivalent(*r15, *r25));
            ASSERT(1 == areStructurallyEquivalent(*r25, *r15));
            ASSERT(0 == areStructurallyEquivalent(*r15, *r25, enumMask));
            ASSERT(0 == areStructurallyEquivalent(*r25, *r15, enumMask));
            ASSERT(1 == isSuperset(*r15, *r25));
            ASSERT(0 == isSuperset(*r25, *r15));
            ASSERT(1 == isSymbolicSuperset(*r15, *r25));
            ASSERT(1 == isSymbolicSuperset(*r25, *r15));
            ASSERT(1 == isSymbolicSuperset(*r15, *r25, enumMask));
            ASSERT(0 == isSymbolicSuperset(*r25, *r15, enumMask));
            ASSERT(1 == isStructuralSuperset(*r15, *r25));
            ASSERT(1 == isStructuralSuperset(*r25, *r15));
            ASSERT(1 == isStructuralSuperset(*r15, *r25, enumMask));
            ASSERT(0 == isStructuralSuperset(*r25, *r15, enumMask));

            const int defaultMask = Util::BDEM_DEFAULT_VALUE;
            ASSERT(0 == areEquivalent(*r16, *r26));
            ASSERT(0 == areEquivalent(*r26, *r16));
            ASSERT(1 == areSymbolicallyEquivalent(*r16, *r26));
            ASSERT(1 == areSymbolicallyEquivalent(*r26, *r16));
            ASSERT(0 == areSymbolicallyEquivalent(*r16, *r26, defaultMask));
            ASSERT(0 == areSymbolicallyEquivalent(*r26, *r16, defaultMask));
            ASSERT(1 == areStructurallyEquivalent(*r16, *r26));
            ASSERT(1 == areStructurallyEquivalent(*r26, *r16));
            ASSERT(0 == areStructurallyEquivalent(*r16, *r26, defaultMask));
            ASSERT(0 == areStructurallyEquivalent(*r26, *r16, defaultMask));
            ASSERT(0 == isSuperset(*r16, *r26));
            ASSERT(0 == isSuperset(*r26, *r16));
            ASSERT(1 == isSymbolicSuperset(*r16, *r26));
            ASSERT(1 == isSymbolicSuperset(*r26, *r16));
            ASSERT(0 == isSymbolicSuperset(*r16, *r26, defaultMask));
            ASSERT(0 == isSymbolicSuperset(*r26, *r16, defaultMask));
            ASSERT(1 == isStructuralSuperset(*r16, *r26));
            ASSERT(1 == isStructuralSuperset(*r26, *r16));
            ASSERT(0 == isStructuralSuperset(*r16, *r26, defaultMask));
            ASSERT(0 == isStructuralSuperset(*r26, *r16, defaultMask));

            ASSERT(0 == areEquivalent(*r16, *r17));
            ASSERT(0 == areEquivalent(*r17, *r16));
            ASSERT(1 == areSymbolicallyEquivalent(*r16, *r17));
            ASSERT(1 == areSymbolicallyEquivalent(*r17, *r16));
            ASSERT(0 == areSymbolicallyEquivalent(*r16, *r17, defaultMask));
            ASSERT(0 == areSymbolicallyEquivalent(*r17, *r16, defaultMask));
            ASSERT(1 == areStructurallyEquivalent(*r16, *r17));
            ASSERT(1 == areStructurallyEquivalent(*r17, *r16));
            ASSERT(0 == areStructurallyEquivalent(*r16, *r17, defaultMask));
            ASSERT(0 == areStructurallyEquivalent(*r17, *r16, defaultMask));
            ASSERT(0 == isSuperset(*r16, *r17));
            ASSERT(0 == isSuperset(*r17, *r16));
            ASSERT(1 == isSymbolicSuperset(*r16, *r17));
            ASSERT(1 == isSymbolicSuperset(*r17, *r16));
            ASSERT(0 == isSymbolicSuperset(*r16, *r17, defaultMask));
            ASSERT(0 == isSymbolicSuperset(*r17, *r16, defaultMask));
            ASSERT(1 == isStructuralSuperset(*r16, *r17));
            ASSERT(1 == isStructuralSuperset(*r17, *r16));
            ASSERT(0 == isStructuralSuperset(*r16, *r17, defaultMask));
            ASSERT(0 == isStructuralSuperset(*r17, *r16, defaultMask));

            ASSERT(0 == areEquivalent(*r18, *r28));
            ASSERT(0 == areEquivalent(*r28, *r18));
            ASSERT(1 == areSymbolicallyEquivalent(*r18, *r28));
            ASSERT(1 == areSymbolicallyEquivalent(*r28, *r18));
            ASSERT(0 == areSymbolicallyEquivalent(*r18, *r28, defaultMask));
            ASSERT(0 == areSymbolicallyEquivalent(*r28, *r18, defaultMask));
            ASSERT(1 == areStructurallyEquivalent(*r18, *r28));
            ASSERT(1 == areStructurallyEquivalent(*r28, *r18));
            ASSERT(0 == areStructurallyEquivalent(*r18, *r28, defaultMask));
            ASSERT(0 == areStructurallyEquivalent(*r28, *r18, defaultMask));
            ASSERT(0 == isSuperset(*r18, *r28));
            ASSERT(0 == isSuperset(*r28, *r18));
            ASSERT(1 == isSymbolicSuperset(*r18, *r28));
            ASSERT(1 == isSymbolicSuperset(*r28, *r18));
            ASSERT(0 == isSymbolicSuperset(*r18, *r28, defaultMask));
            ASSERT(0 == isSymbolicSuperset(*r28, *r18, defaultMask));
            ASSERT(1 == isStructuralSuperset(*r18, *r28));
            ASSERT(1 == isStructuralSuperset(*r28, *r18));
            ASSERT(0 == isStructuralSuperset(*r18, *r28, defaultMask));
            ASSERT(0 == isStructuralSuperset(*r28, *r18, defaultMask));

            ASSERT(0 == areEquivalent(*r18, *r19));
            ASSERT(0 == areEquivalent(*r19, *r18));
            ASSERT(1 == areSymbolicallyEquivalent(*r18, *r19));
            ASSERT(1 == areSymbolicallyEquivalent(*r19, *r18));
            ASSERT(0 == areSymbolicallyEquivalent(*r18, *r19, defaultMask));
            ASSERT(0 == areSymbolicallyEquivalent(*r19, *r18, defaultMask));
            ASSERT(1 == areStructurallyEquivalent(*r18, *r19));
            ASSERT(1 == areStructurallyEquivalent(*r19, *r18));
            ASSERT(0 == areStructurallyEquivalent(*r18, *r19, defaultMask));
            ASSERT(0 == areStructurallyEquivalent(*r19, *r18, defaultMask));
            ASSERT(0 == isSuperset(*r18, *r19));
            ASSERT(0 == isSuperset(*r19, *r18));
            ASSERT(1 == isSymbolicSuperset(*r18, *r19));
            ASSERT(1 == isSymbolicSuperset(*r19, *r18));
            ASSERT(0 == isSymbolicSuperset(*r18, *r19, defaultMask));
            ASSERT(0 == isSymbolicSuperset(*r19, *r18, defaultMask));
            ASSERT(1 == isStructuralSuperset(*r18, *r19));
            ASSERT(1 == isStructuralSuperset(*r19, *r18));
            ASSERT(0 == isStructuralSuperset(*r18, *r19, defaultMask));
            ASSERT(0 == isStructuralSuperset(*r19, *r18, defaultMask));

            ASSERT(0 == areEquivalent(*r11, *r23));
            ASSERT(1 == areSymbolicallyEquivalent(*r11, *r23));
            ASSERT(1 == areStructurallyEquivalent(*r11, *r23));
            ASSERT(0 == areSymbolicallyEquivalent(*r11, *r23, enumMask));
            ASSERT(0 == areStructurallyEquivalent(*r11, *r23, enumMask));
            ASSERT(1 == isSuperset(*r11, *r23));
            ASSERT(1 == isSymbolicSuperset(*r11, *r23));
            ASSERT(1 == isStructuralSuperset(*r11, *r23));
        }

        if (verbose) cout << "    CHOICE records." << endl;
        {
            Schema superSet;
            Schema subSet;
            Schema symbSubSet;

            EnumDef *superEnum = superSet.createEnumeration("SuperEnum");
            superEnum->addEnumerator("Enum1", 1);
            superEnum->addEnumerator("Enum2");

            EnumDef *subEnum = subSet.createEnumeration("SubEnum");
            subEnum->addEnumerator("Enum1", 1);

            RecDef *r11 = superSet.createRecord("Rec1", CHOICE);
            r11->appendField(EType::BDEM_INT, "Int");
            r11->appendField(EType::BDEM_DATE, "Date");
            r11->appendField(EType::BDEM_INT, superEnum, "IntEnum1");

            RecDef *r12 = superSet.createRecord("Rec2", CHOICE);
            r12->appendField(EType::BDEM_INT,"Int");
            r12->appendField(EType::BDEM_DATE,"Date");
            r12->appendField(EType::BDEM_INT, superEnum, "IntEnum1");

            RecDef *r21 = subSet.createRecord("Rec1", CHOICE);
            r21->appendField(EType::BDEM_INT,"Int");

            RecDef *r22 = subSet.createRecord("Rec2", CHOICE);
            r22->appendField(EType::BDEM_INT,"Int");
            r22->appendField(EType::BDEM_INT, subEnum, "IntEnum1");

            RecDef *r23 = subSet.createRecord("Rec3", CHOICE);
            r23->appendField(EType::BDEM_INT,"Int");
            r23->appendField(EType::BDEM_DATE,"Date");
            r23->appendField(EType::BDEM_INT, subEnum, "IntEnum1");

            // TBD: Are the next two lines needed?
            //RecDef *r32 = symbSubSet.createRecord("Rec2", CHOICE);
            //r32->appendField(EType::BDEM_DATE,"Date");

            ASSERT(1 == areEquivalent(*r11, *r12));
            ASSERT(1 == areSymbolicallyEquivalent(*r11, *r12));
            ASSERT(1 == areStructurallyEquivalent(*r11, *r12));
            ASSERT(1 == isSuperset(*r11, *r12));
            ASSERT(1 == isSymbolicSuperset(*r11, *r12));
            ASSERT(1 == isStructuralSuperset(*r11, *r12));

            ASSERT(0 == areEquivalent(*r11, *r21));
            ASSERT(0 == areSymbolicallyEquivalent(*r11, *r21));
            ASSERT(0 == areStructurallyEquivalent(*r11, *r21));
            ASSERT(1 == isSuperset(*r11, *r21));
            ASSERT(1 == isSymbolicSuperset(*r11, *r21));
            ASSERT(1 == isStructuralSuperset(*r11, *r21));

            ASSERT(0 == areEquivalent(*r11, *r22));
            ASSERT(0 == areSymbolicallyEquivalent(*r11, *r22));
            ASSERT(0 == areStructurallyEquivalent(*r11, *r22));
            ASSERT(0 == isSuperset(*r11, *r22));
            ASSERT(1 == isSymbolicSuperset(*r11, *r22));
            ASSERT(0 == isStructuralSuperset(*r11, *r22));

            const int mask = Util::BDEM_CHECK_ENUMERATION;
            ASSERT(0 == areEquivalent(*r11, *r23));
            ASSERT(1 == areSymbolicallyEquivalent(*r11, *r23));
            ASSERT(1 == areStructurallyEquivalent(*r11, *r23));
            ASSERT(0 == areSymbolicallyEquivalent(*r11, *r23, mask));
            ASSERT(0 == areStructurallyEquivalent(*r11, *r23, mask));
            ASSERT(1 == isSuperset(*r11, *r23));
            ASSERT(1 == isSymbolicSuperset(*r11, *r23));
            ASSERT(1 == isStructuralSuperset(*r11, *r23));
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error: non-zero test status = " << testStatus << "."
             << endl;
    }
    else {
        if(verbose) cout << "(Success!)" << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
