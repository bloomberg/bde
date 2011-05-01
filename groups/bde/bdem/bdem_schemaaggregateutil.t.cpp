// bdem_schemaaggregateutil.t.cpp                                     -*-C++-*-

#include <bdem_schemaaggregateutil.h>

#include <bdem_aggregate.h>
#include <bdem_elemattrlookup.h>             // for testing only
#include <bdem_elemref.h>                    // for testing only
#include <bdem_elemtype.h>
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

#include <bsls_assert.h>
#include <bsls_types.h>                      // for testing only

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_cctype.h>       // isspace()
#include <bsl_cstdlib.h>      // atoi

#include <bsl_c_assert.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                   TEST PLAN
//-----------------------------------------------------------------------------
// [11] int canSatisfyRecord(const bdem_Row&, bdem_RecordDef&);
// [11] int canSatisfyRecord(const bdem_List&, bdem_RecordDef&);
// [12] int canSatisfyRecord(const bdem_Table&, bdem_RecordDef&);
// [13] int canSatisfyRecord(const bdem_Choice&, const bdem_RecordDef&);
// [14] int canSatisfyRecord(const bdem_ChoiceArray&, recDef);
// [13] int canSatisfyRecord(const bdem_ChoiceArrayItem&, recDef);
// [ 3] initRowShallow(bdem_Row *row, const bdem_RecordDef& record);
// [ 6] initRowDeep(bdem_Row *row, const bdem_RecordDef& record);
// [ 3] initListAllNull(bdem_List *list, const bdem_RecordDef& record);
// [ 3] initListShallow(bdem_List *list, const bdem_RecordDef& record);
// [ 6] initListDeep(bdem_List *list, const bdem_RecordDef& record);
// [ 4] initTable(bdem_Table *table, const bdem_RecordDef& record);
// [ 5] initChoice(bdem_Choice *choice, const bdem_RecordDef& record);
// [ 5] initChoiceArray(bdem_ChoiceArray*, const bdem_RecordDef& record);
// [ 7] bool isRowConformant(const bdem_Row&, bdem_RecordDef&);
// [ 7] bool isRowShallowConformant(const bdem_Row&, bdem_RecordDef&);
// [ 7] bool isRowDeepConformant(const bdem_Row&, bdem_RecordDef&);
// [ 7] bool isListConformant(const bdem_List&, bdem_RecordDef&);
// [ 7] bool isListShallowConformant(const bdem_List&, bdem_RecordDef&);
// [ 7] bool isListDeepConformant(const bdem_List&, bdem_RecordDef&);
// [ 8] bool isTableConformant(const bdem_Table& table, recDef);
// [ 8] bool isTableShallowConformant(const bdem_Table& table, recDef);
// [ 8] bool isTableDeepConformant(const bdem_Table& table, recDef);
// [ 9] bool isChoiceConformant(const bdem_Choice&, recDef);
// [ 9] bool isChoiceShallowConformant(const bdem_Choice&, recDef);
// [ 9] bool isChoiceDeepConformant(const bdem_Choice&, recDef);
// [10] bool isChoiceArrayConformant(const bdem_ChoiceArray&, recDef);
// [10] bool isChoiceArrayShallowConformant(const ChoiceArray&, recDef);
// [10] bool isChoiceArrayDeepConformant(const ChoiceArray&, recDef);
// [15] bsl::ostream& print(bsl::ostream&             stream,
//                          const bdem_ConstElemRef&  item,
//                          const bdem_RecordDef     *constraint     = 0,
//                          int                       level          = 0,
//                          int                       spacesPerLevel = 4);
// [15] bsl::ostream& print(bsl::ostream&         stream,
//                          const bdem_Row&       item,
//                          const bdem_RecordDef& record,
//                          int                   level          = 0,
//                          int                   spacesPerLevel = 4);
// [15] bsl::ostream& print(bsl::ostream&         stream,
//                          const bdem_List&      item,
//                          const bdem_RecordDef& record,
//                          int                   level          = 0,
//                          int                   spacesPerLevel = 4);
// [15] bsl::ostream& print(bsl::ostream&               stream,
//                          const bdem_ChoiceArrayItem& item,
//                          const bdem_RecordDef&       record,
//                          int                         level          = 0,
//                          int                         spacesPerLevel = 4);
// [15] bsl::ostream& print(bsl::ostream&         stream,
//                          const bdem_Choice&    item,
//                          const bdem_RecordDef& record,
//                          int                   level          = 0,
//                          int                   spacesPerLevel = 4);
// [15] bsl::ostream& print(bsl::ostream&         stream,
//                          const bdem_Table&     item,
//                          const bdem_RecordDef& record,
//                          int                   level          = 0,
//                          int                   spacesPerLevel = 4);
// [15] bsl::ostream& print(bsl::ostream&           stream,
//                          const bdem_ChoiceArray& item,
//                          const bdem_RecordDef&   record,
//                          int                     level          = 0,
//                          int                     spacesPerLevel = 4);
//-----------------------------------------------------------------------------
// [ 2] bdem_Schema g(const char *spec);
// [ 1] bdem_Schema& gg(bdem_Schema *object, const char *spec);
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

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

static int verbose         = 0;
static int veryVerbose     = 0;
static int veryVeryVerbose = 0;

//=============================================================================
//                   GLOBAL CONSTANTS/TYPEDEFS FOR TESTING
//=============================================================================

typedef bdem_SchemaAggregateUtil Util;
typedef bdem_ElemRef             ERef;
typedef bdem_ElemType            EType;
typedef bdem_Schema              Schema;
typedef bdem_RecordDef           RecDef;
typedef RecDef::RecordType       RecType;
typedef bdem_EnumerationDef      EnumDef;
typedef bdem_FieldDef            FldDef;
typedef bdem_FieldSpec           FldSpec;
typedef bdem_FieldDefAttributes  FldAttr;
typedef bdem_Row                 Row;
typedef bdem_List                List;
typedef bdem_Table               Table;
typedef bdem_Choice              Choice;
typedef bdem_ChoiceArray         Array;
typedef bdem_ChoiceArrayItem     ChoiceItem;

typedef bdeat_FormattingMode     Format;

typedef bsls_Types::Int64        Int64;

const int NUM_TYPES = EType::BDEM_NUM_TYPES;

#ifndef SIZEOF_ARRAY
#define SIZEOF_ARRAY(a) (int)(sizeof(a)/sizeof((a)[0]))
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
const Int64           N03 = bdetu_Unset<Int64>::unsetValue();

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
                                  // 'CHOICE_ARRAY')
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
const char bdemAggType[]    = "UVef";
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
      case 'D': return EType::BDEM_LIST == type ? false : true;
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
        LOOP2_ASSERT(fieldTypeChar, *p, isName(*p));
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
        bool nullable      = ! bsl::strchr("+#%@", fieldTypeChar);
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
//            One Last Helper That Uses a 'gg' Helper
//-----------------------------------------------------------------------------
static inline
const char *ggName(int index)
    // Return the 'gg' test name corresponding to the specified 'index' if
    // index < NUM_GG_NAMES, and 0 otherwise.
{
    return index < NUM_GG_NAMES ? getName(name[index]) : 0;
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
    ":h "
    ":i B* "
    ":j #aa +ba",
};

const int NUM_SPECS = sizeof testSpecs / sizeof *testSpecs;

//=============================================================================
//                          Helper functions
//-----------------------------------------------------------------------------

static
bool containsAggregate(const RecDef& rec)
{
    int length = rec.numFields();

    for (int i = 0; i < length; ++i) {
        if (bdem_ElemType::isAggregateType(rec.field(i).elemType())) {
            return true;
        }
    }

    return false;
}

static
bool totallyCorrect(const bdem_Row& row, const bdem_RecordDef& record) {
    return Util::canSatisfyRecord(row, record) &&
           Util::isRowConformant(row, record) &&
           Util::isRowShallowConformant(row, record) &&
           Util::isRowDeepConformant(row, record);
}

static
bool totallyCorrect(const bdem_List& list, const bdem_RecordDef& record) {
    return Util::canSatisfyRecord(list, record) &&
           Util::isListConformant(list, record) &&
           Util::isListShallowConformant(list, record) &&
           Util::isListDeepConformant(list, record);
}

struct Assertions {
    char assertion1[sizeof(int) == sizeof(bdem_ElemType::Type)];
};

void getElemTypes(bsl::vector<bdem_ElemType::Type> *types, const bdem_Row& row)
    // Load the specified 'types' with the element types from the specified
    // non-modifiable 'row'.
{
    const int numTypes = row.length();

    types->clear();
    types->reserve(numTypes);
    for (int i = 0; i < numTypes; ++i) {
        types->push_back(row.elemType(i));
    }
}

inline
void getElemTypes(bsl::vector<bdem_ElemType::Type> *types,
                  const bdem_List&                  list)
    // Load the specified 'types' with the element types from the specified
    // non-modifiable 'list'.
{
    getElemTypes(types, list.row());
}

void getElemTypes(bsl::vector<bdem_ElemType::Type> *types,
                  const bdem_Table&                 table)
    // Load the specified 'types' with the element types of the columns from
    // the specified non-modifiable 'table'.
{
    const int numTypes = table.numColumns();

    types->clear();
    types->reserve(numTypes);
    for (int i = 0; i < numTypes; ++i) {
        types->push_back(table.columnType(i));
    }
}

void getElemTypes(bsl::vector<bdem_ElemType::Type> *types,
                  const bdem_RecordDef&             record)
    // Load the specified 'types' with the field types of the specified
    // non-modifiable 'record'.
{
    const int numTypes = record.numFields();

    types->clear();
    types->reserve(numTypes);
    for (int i = 0; i < numTypes; ++i) {
        types->push_back(record.field(i).elemType());
    }
}

void getElemTypes(bsl::vector<bdem_ElemType::Type> *types,
                  const bdem_Choice&                choice)
    // Load the specified 'types' with the element types from the type
    // catalog of the specified non-modifiable 'choice'.
{
    const int numTypes = choice.numSelections();

    types->clear();
    types->reserve(numTypes);
    for (int i = 0; i < numTypes; ++i) {
        types->push_back(choice.selectionType(i));
    }
}

void getElemTypes(bsl::vector<bdem_ElemType::Type> *types,
                  const bdem_ChoiceArray&           array)
    // Load the specified 'types' with the element types from the type
    // catalog of the specified choice non-modifiable 'array'.
{
    const int numTypes = array.numSelections();

    types->clear();
    types->reserve(numTypes);
    for (int i = 0; i < numTypes; ++i) {
        types->push_back(array.selectionType(i));
    }
}

//-----------------------------------------------------------------------------
static
int verifyDeepInitList(const List& list, const RecDef& recDef)
    // Verify:
    //    1. the specified 'list' has the element types of the specified
    //       'recDef';
    //    2. a scalar element is set to its default value
    //    3. an aggregate or array element is unset if it is unconstrained;
    //    4. for a constrained list, if the constraint is 'recDef' itself,
    //       then the list element is unset, otherwise recursively call
    //       'verifyDeepInitList'.
    //    5. a constrained table has the required column types and 0 rows;
    // Return 1 ("true") on success, and 0 ("false") otherwise.
{
    bsl::vector<bdem_ElemType::Type> elemTypes;
    getElemTypes(&elemTypes, recDef);

    bsl::vector<bdem_ElemType::Type> listElemTypes;
    getElemTypes(&listElemTypes, list);
    if (listElemTypes != elemTypes) {
        cout << "list.elemTypes() != elemTypes" << endl;
        return 0;
    }

    for (int i = 0; i < list.length(); ++i) {
        const FldDef *pFld = &recDef.field(i);
        const RecDef *pConstraint = pFld->recordConstraint();
        const bdem_ElemType::Type fldType = pFld->elemType();

        if (bdem_ElemType::isScalarType(fldType)) {
            //    2. a scalar element is set to its default value
            if (!pFld->hasDefaultValue()) {
                if (pFld->isNullable()) {
                    if (!list[i].isNull()) {
                        cout << "schema is null, element is not" << endl;
                        P_(list[i]);
                        return 0;
                    }
                }
                else if (list[i].isNonNull()) {
                    cout << "schema has no default value, list is set" << endl;
                    return 0;
                }
            }
            else {
                if (list[i] != pFld->defaultValue()) {
                    cout << "scalar element is not set to default value" <<
                                                                          endl;
                    P(list[i]); P(pFld->defaultValue());
                    return 0;
                }
            }
        }
        else if (bdem_ElemType::isAggregateType(fldType)) {
            if (pFld->isNullable()) {
                if (!list[i].isNull()) {
                    cout << "schema is null, element is not" << endl;
                    P_(list[i]);
                    return 0;
                }

                continue;
            }

            if (! pConstraint) {
                //    3. an aggregate or array element is unset if it is
                //       unconstrained;
                if (list[i].isNonNull()) {
                    cout << "unconstrained sub-list not unset" << endl;
                    return 0;
                }

                continue;                                           // CONTINUE
            }

            // constrained aggregate

            switch (fldType) {
              case EType::BDEM_LIST: {
                //    4. for a constrained list, if the constraint is 'recDef'
                //       itself, then the list element is unset, otherwise
                //       recursively call 'verifyDeepInitList'.
                if(pConstraint == &recDef) {
                    if (list[i].isNonNull()) {
                        cout << "self-constrained sub-list not unset" << endl;
                        return 0;
                    }
                }
                else {
                    if (!verifyDeepInitList(list.theList(i), *pConstraint)) {
                        cout << "sub-list not deep-conformant" << endl;
                        return 0;
                    }
                }
              } break;
              case EType::BDEM_TABLE: {
                //    5. a constrained table has the required column types and
                //       0 rows;
                bsl::vector<bdem_ElemType::Type> constraintTypes;
                bsl::vector<bdem_ElemType::Type> tableTypes;
                getElemTypes(&constraintTypes, *pConstraint);
                list.theTable(i).columnTypes(&tableTypes);
                if (constraintTypes != tableTypes) {
                    cout << "sub-table does not have proper column types"
                         << endl;
                    P(list.theTable(i)); P(bdem_Table(constraintTypes));
                    return 0;
                }
                if (0 != list.theTable(i).numRows()) {
                    cout << "sub-table does not have zero rows" << endl;
                    P(list.theTable(i));
                    return 0;
                }
              } break;
              case EType::BDEM_CHOICE: {
                //    6. a constrained choice has the required type catalog and
                //       is null;
                bsl::vector<bdem_ElemType::Type> constraintTypes;
                bsl::vector<bdem_ElemType::Type> catalogTypes;
                getElemTypes(&constraintTypes, *pConstraint);
                list.theChoice(i).selectionTypes(&catalogTypes);
                if (constraintTypes != catalogTypes) {
                    cout << "sub-choice does not have the proper type catalog"
                         << endl;
                    P(list.theChoice(i)); P(bdem_Choice(constraintTypes));
                    return 0;
                }
                if (-1 != list.theChoice(i).selector()) {
                    cout << "sub-choice is not null" << endl;
                    P(list.theChoice(i));
                    return 0;
                }
              } break;
              case EType::BDEM_CHOICE_ARRAY: {
                //    7. a constrained choice array has the required type
                //       catalog and contains no items;
                bsl::vector<bdem_ElemType::Type> constraintTypes;
                bsl::vector<bdem_ElemType::Type> catalogTypes;
                getElemTypes(&constraintTypes, *pConstraint);
                list.theChoiceArray(i).selectionTypes(&catalogTypes);
                if (constraintTypes != catalogTypes) {
                    cout <<
                       "sub-choice-array does not have the proper type catalog"
                         << endl;
                    P(list.theChoiceArray(i));
                    P(bdem_ChoiceArray(constraintTypes));
                    return 0;
                }
                if (0 != list.theChoiceArray(i).length()) {
                    cout << "sub-choice-array is not empty" << endl;
                    P(list.theChoiceArray(i));
                    return 0;
                }
              } break;
              default: {
                BSLS_ASSERT(0);  // Should *not* reach here!
              } break;
            } // switch
        } // if scalar else if aggregate
    } // for elements in list

    return 1;
}

//-----------------------------------------------------------------------------

// The functions 'buildListDeep', 'buildTableDeep', 'buildChoiceDeep', and
// 'buildChoiceArrayDeep' recursively build an aggregate from the specified
// record.  These functions are intended for testing conformance predicates
// and printing.

// forward declarations

static
void buildElemRefDeep(const bdem_ElemRef&  ref,
                      const RecDef        *pConstraint,
                      bool                 rich = false);

static
void buildChoiceDeep(Choice *choice, const RecDef& recDef, bool rich = false)
    // Reset the type catalog of the specified 'choice' to the element types of
    // the field types in the specified 'recDef'.  If 'recDef' is not empty,
    // set 'choice' to have a selector of 0.
{
    bsl::vector<EType::Type> types;

    getElemTypes(&types, recDef);
    choice->reset(types);

    if (!types.empty()) {
        const int SELECTION = 0;

        choice->makeSelection(SELECTION);
        const FldDef& fld = recDef.field(SELECTION);
        const RecDef *pConstraint = fld.recordConstraint();

        if (pConstraint != &recDef) {
            buildElemRefDeep(choice->selection(), pConstraint, rich);
        }
    }
}

static
void buildChoiceArrayDeep(Array *array, const RecDef& recDef,
                          bool rich = false)
    // Reset the type catalog of the specified choice 'array' to the element
    // types of the field types in the specified 'recDef' and append 2 unset
    // items to the choice array.  If 'recDef' is not empty, set each item in
    // the choice 'array' to have a selector of 0.  If 'rich' is true, then
    // the array is constructed with more complexity: having an item for each
    // selector.
{
    bsl::vector<EType::Type> types;

    getElemTypes(&types, recDef);
    array->reset(types);

    int numSelections = types.size();

    if (rich) {
        // Append an item for each possible selector, but not fewer than 2.
        array->appendNullItems(numSelections < 2 ? 2 : numSelections);
    }
    else {
        array->appendNullItems(2);
    }

    if (0 != numSelections) {
        for (int i = 0; i < array->length(); ++i) {
            const int SELECTION = rich ? i % numSelections : 0;
            const FldDef& fld = recDef.field(SELECTION);
            const RecDef *pConstraint = fld.recordConstraint();
            (*array).theModifiableItem(i).makeSelection(SELECTION);

            if (pConstraint != &recDef) {
                buildElemRefDeep((*array).theModifiableItem(i).selection(),
                                 pConstraint, rich);
            }
        }
    }
}

static
void buildTableDeep(Table* table, const RecDef& recDef, bool rich = false)
    // Reset the element types of the columns of the specified 'table' to
    // the element types of the field types in the specified 'recDef' and
    // append 2 unset rows to the table.  For each constrained field in
    // 'recDef', call 'buildList' or 'buildTable' with sub-list or
    // sub-table and the constraining recordDef.
{
    Util::initTable(table, recDef);
    table->appendNullRow();

    const int nFields = recDef.numFields();
    for (int f = 0; f < nFields; ++f) {
        const FldDef& fld = recDef.field(f);
        const RecDef *pConstraint = fld.recordConstraint();
        if (pConstraint != &recDef) {
            buildElemRefDeep((*table).theModifiableRow(0)[f],
                             pConstraint, rich);
        }
    }

    // Append a second row identical to the first.
    table->appendRow(*table, 0);
}

static
void buildListDeep(List* list, const RecDef& recDef, bool rich = false)
    // Reset the element types of the specified 'list' to the element types
    // of the field types in the specified 'recDef' and  For each constrained
    // field in 'recDef', call 'buildList' or 'buildTable' with sub-list or
    // sub-table and the constraining recordDef.
{
    Util::initListShallow(list, recDef);

    int nFields = recDef.numFields();
    for (int f = 0; f < nFields; ++f) {
        const FldDef& fld = recDef.field(f);
        const RecDef *pConstraint = fld.recordConstraint();
        if (pConstraint != &recDef) {
            buildElemRefDeep((*list)[f], pConstraint, rich);
        }
    }
}

static
void buildElemRefDeep(const bdem_ElemRef&  ref,
                      const RecDef        *pConstraint,
                      bool                 rich)
{
    switch (ref.type()) {
      case EType::BDEM_CHAR: {
          ref.theModifiableChar()       = B00;
      } break;
      case EType::BDEM_SHORT: {
          ref.theModifiableShort()      = B01;
      } break;
      case EType::BDEM_INT: {
          ref.theModifiableInt()        = B02;
      } break;
      case EType::BDEM_INT64: {
          ref.theModifiableInt64()      = B03;
      } break;
      case EType::BDEM_FLOAT: {
          ref.theModifiableFloat()      = B04;
      } break;
      case EType::BDEM_DOUBLE: {
          ref.theModifiableDouble()     = B05;
      } break;
      case EType::BDEM_STRING: {
          ref.theModifiableString()     = B06;
      } break;
      case EType::BDEM_DATETIME: {
          ref.theModifiableDatetime()   = B07;
      } break;
      case EType::BDEM_DATE: {
          ref.theModifiableDate()       = B08;
      } break;
      case EType::BDEM_TIME: {
          ref.theModifiableTime()       = B09;
      } break;
      case EType::BDEM_BOOL: {
          ref.theModifiableBool()       = B22;
      } break;
      case EType::BDEM_DATETIMETZ: {
          ref.theModifiableDatetimeTz() = B23;
      } break;
      case EType::BDEM_DATETZ: {
          ref.theModifiableDateTz()     = B24;
      } break;
      case EType::BDEM_TIMETZ: {
          ref.theModifiableTimeTz()     = B25;
      } break;

      case EType::BDEM_CHAR_ARRAY: {
          ref.theModifiableCharArray().push_back(A00);
          ref.theModifiableCharArray().push_back(B00);
      } break;
      case EType::BDEM_SHORT_ARRAY: {
          ref.theModifiableShortArray().push_back(A01);
          ref.theModifiableShortArray().push_back(B01);
      } break;
      case EType::BDEM_INT_ARRAY: {
          ref.theModifiableIntArray().push_back(A02);
          ref.theModifiableIntArray().push_back(B02);
      } break;
      case EType::BDEM_INT64_ARRAY: {
          ref.theModifiableInt64Array().push_back(A03);
          ref.theModifiableInt64Array().push_back(B03);
      } break;
      case EType::BDEM_FLOAT_ARRAY: {
          ref.theModifiableFloatArray().push_back(A04);
          ref.theModifiableFloatArray().push_back(B04);
      } break;
      case EType::BDEM_DOUBLE_ARRAY: {
          ref.theModifiableDoubleArray().push_back(A05);
          ref.theModifiableDoubleArray().push_back(B05);
      } break;
      case EType::BDEM_STRING_ARRAY: {
          ref.theModifiableStringArray().push_back(A06);
          ref.theModifiableStringArray().push_back(B06);
      } break;
      case EType::BDEM_DATETIME_ARRAY: {
          ref.theModifiableDatetimeArray().push_back(A07);
          ref.theModifiableDatetimeArray().push_back(B07);
      } break;
      case EType::BDEM_DATE_ARRAY: {
          ref.theModifiableDateArray().push_back(A08);
          ref.theModifiableDateArray().push_back(B08);
      } break;
      case EType::BDEM_TIME_ARRAY: {
          ref.theModifiableTimeArray().push_back(A09);
          ref.theModifiableTimeArray().push_back(B09);
      } break;
      case EType::BDEM_BOOL_ARRAY: {
          ref.theModifiableBoolArray().push_back(A22);
          ref.theModifiableBoolArray().push_back(B22);
      } break;
      case EType::BDEM_DATETIMETZ_ARRAY: {
          ref.theModifiableDatetimeTzArray().push_back(A23);
          ref.theModifiableDatetimeTzArray().push_back(B23);
      } break;
      case EType::BDEM_DATETZ_ARRAY: {
          ref.theModifiableDateTzArray().push_back(A24);
          ref.theModifiableDateTzArray().push_back(B24);
      } break;
      case EType::BDEM_TIMETZ_ARRAY: {
          ref.theModifiableTimeTzArray().push_back(A25);
          ref.theModifiableTimeTzArray().push_back(B25);
      } break;

      case EType::BDEM_LIST: {
          if (pConstraint) {
              buildListDeep(&ref.theModifiableList(), *pConstraint, rich);
          }
      } break;
      case EType::BDEM_TABLE: {
          if (pConstraint) {
              buildTableDeep(&ref.theModifiableTable(), *pConstraint, rich);
          }
      } break;
      case EType::BDEM_CHOICE: {
          if (pConstraint) {
              buildChoiceDeep(&ref.theModifiableChoice(), *pConstraint, rich);
          }
      } break;
      case EType::BDEM_CHOICE_ARRAY: {
          if (pConstraint) {
              buildChoiceArrayDeep(&ref.theModifiableChoiceArray(),
                                                           *pConstraint, rich);
          }
      } break;

      default: {
          BSLS_ASSERT(0);  // Should *not* reach here!
      } break;
    }
}

// Data for testing indentation in 'print' functions
static const struct {
    int         d_level;                // Indentation level
    int         d_spacesPerLevel;       // Spaces per level
    const char* d_exp1stIndent;         // Expected 1st-line indentation
    const char* d_exp2ndIndent;         // Expected 2ne-line indentation
    const char* d_expLastIndent;        // Expected last-line indentation
} INDENT_DATA[] = {
    // level    spaces  1st Indent      2nd Indent      last Indent
    // -----    ------  -------------   ------------    -----------
    {  0,        0,     "",             "",             ""      },
    {  0,        4,     "",             "    ",         ""      },
    {  0,       -4,     "",             0,              0       },
    {  1,        0,     "",             "",             ""      },
    {  1,        4,     "    ",         "        ",     "    "  },
    {  1,       -4,     "    ",         0,              0       },
    { -1,        0,     "",             "",             ""      },
    { -1,        3,     "",             "      ",       "   "   },
    { -1,       -5,     "",             0,              0       },
};

static const int NUM_INDENT_DATA = SIZEOF_ARRAY(INDENT_DATA);

template <typename AGG_TYPE>
void printTest(int                   LINE,
               const AGG_TYPE&       agg,
               EType::Type           type,
               const bdem_RecordDef& constraint)
    // Using a variety of 'level' and 'spacesPerLevel' values, construct a
    // stringstream, 'stream', and call
    // 'Util::print(stream, agg, constraint, level, spacesPerLevel)', then
    // validate that the resulting output has the correct indentation on the
    // first, second, and last lines.  Finally, package 'agg' into a
    // 'bdem_ConstElemRef' and call 'Util::print' for the elemref, then
    // validate that the output is identical to the output for calling 'print'
    // on 'agg'.  Specifically, this function validates the following concerns:
    //..
    // - The first line is indented by 'abs(level * spacesPerLevel)'
    //   unless 'level' is negative.
    // - The second line is indented by 'abs((level+1) * spacesPerLevel)'
    //   unless 'spacesPerLevel' is negative or unless the output is
    //   fewer than three lines.
    // - The last line is indented by 'abs(level * spacesPerLevel)'
    //   unless 'spacesPerLevel' is negative or unless the output is
    //   fewer than two lines.
    // - The first line is not indented if 'level' is negative.
    // - There are no newlines in the output if 'spacesPerLevel' is
    //   negative.
    // - The result of calling 'print' directly on an aggregate is
    //   the same as calling 'print' through an ElemRef to that aggregate.
    //
    // Plan:
    // - 'printTest' uses a vector of various 'level' and 'spacesPerLevel'
    //   values can calls 'bdem_SchemaAggregateUtil::print' for each
    //   combination.
    // - For each 'print' output, 'printTest' validates the concerns
    //   listed above.
    // - Finally, 'printTest' construct a 'bdem_ConstElemRef' from the
    //   aggregate and verifies that the output of 'print' for the ElemRef
    //   is the same as for the raw aggregate.
    //..
{
    typedef bsl::string::size_type strpos_t;
    static const strpos_t npos = bsl::string::npos;

    bsl::ostringstream stream;

    for (int i = 0; i < NUM_INDENT_DATA; ++i) {
        const int         LEVEL              = INDENT_DATA[i].d_level;
        const int         SPACES_PER_LEVEL   = INDENT_DATA[i].d_spacesPerLevel;
        const char *const EXP_1ST_INDENT     = INDENT_DATA[i].d_exp1stIndent;
        const char *const EXP_2ND_INDENT     = INDENT_DATA[i].d_exp2ndIndent;
        const char *const EXP_LAST_INDENT    = INDENT_DATA[i].d_expLastIndent;
        const int         EXP_1ST_INDENT_LEN = bsl::strlen(EXP_1ST_INDENT);
        const int         EXP_2ND_INDENT_LEN = (EXP_2ND_INDENT ?
                                                bsl::strlen(EXP_2ND_INDENT) :
                                                0);
        const int         EXP_LAST_INDENT_LEN= (EXP_LAST_INDENT ?
                                                bsl::strlen(EXP_LAST_INDENT) :
                                                0);

        stream.str("");
        Util::print(stream, agg, constraint, LEVEL, SPACES_PER_LEVEL);

        bsl::string result = stream.str();
        LOOP3_ASSERT(LINE, LEVEL, SPACES_PER_LEVEL, ! result.empty());

        if (veryVeryVerbose) {
            P_(LEVEL); P(SPACES_PER_LEVEL);
            P(result);
        }

        LOOP3_ASSERT(LINE, LEVEL, SPACES_PER_LEVEL,
                     EXP_1ST_INDENT == result.substr(0, EXP_1ST_INDENT_LEN));

        // Find start of second line, if not empty.
        strpos_t secondLine = result.find("\n");
        if (result.length() - 1 == secondLine) {
            secondLine = npos;  // second line is empty
        }
        if (npos != secondLine) {
            ++secondLine;
        }

        // Find start of last non-empty line
        strpos_t lastLine = result.rfind("\n");
        if (result.length() - 1 == lastLine) {
            lastLine = result.rfind("\n", lastLine - 1);
        }
        if (npos != lastLine) {
            ++lastLine;
        }

        // Either secondLine and lastLine are both npos, or neither of them is
        // npos:
        LOOP5_ASSERT(LINE, LEVEL, SPACES_PER_LEVEL, secondLine, lastLine,
                     (npos == secondLine) == (npos == lastLine));

        // Test that indentation of second line is as expected, but only if
        // second line is not the last line.
        if (! EXP_2ND_INDENT) {
            LOOP3_ASSERT(LINE, LEVEL, SPACES_PER_LEVEL, npos == secondLine);
        }
        else if (secondLine != lastLine) {
            LOOP3_ASSERT(LINE, LEVEL, SPACES_PER_LEVEL,
                         EXP_2ND_INDENT == result.substr(secondLine,
                                                         EXP_2ND_INDENT_LEN));
            LOOP3_ASSERT(LINE, LEVEL, SPACES_PER_LEVEL,
                         ' ' != result[secondLine + EXP_2ND_INDENT_LEN]);
        }

        // Test that indentation of last line is as expected.
        if (! EXP_LAST_INDENT) {
            LOOP3_ASSERT(LINE, LEVEL, SPACES_PER_LEVEL, npos == lastLine);
        }
        else if (npos != lastLine) {
            LOOP3_ASSERT(LINE, LEVEL, SPACES_PER_LEVEL,
                         EXP_LAST_INDENT == result.substr(lastLine,
                                                         EXP_LAST_INDENT_LEN));
            LOOP3_ASSERT(LINE, LEVEL, SPACES_PER_LEVEL,
                         ' ' != result[lastLine + EXP_LAST_INDENT_LEN]);
        }

        // Print again through an elemref.  Results should be the same.
        bdem_ConstElemRef ref(&agg, bdem_ElemAttrLookup::lookupTable()[type]);
        stream.str("");
        Util::print(stream, ref, &constraint, LEVEL, SPACES_PER_LEVEL);
        LOOP3_ASSERT(LINE, LEVEL, SPACES_PER_LEVEL, stream.str() == result);
    }
}
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test        = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose         = argc > 2;
    veryVerbose     = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 26: {
        // ====================================================================
        // Usage Example
        //
        // Concerns:
        //   Need a real world usage example that illustrates, constrained
        //   aggregates, constrained enumerations, and default values.
        //
        // Plan:
        //   Build a usage example based on mortgage bonds.  In mortgage bonds,
        //   something like 1000 home loans are packaged together in a "deal",
        //   which is a family of bonds.  As homeowners pay off their
        //   mortgages, the money flows into the deal, and then payments are
        //   made to several different bonds.  Some bonds in the deal are
        //   paid before others, the bonds paid last take the most risk.  The
        //   first bonds to be paid are "senior" and have good ratings.  The
        //   "junior" bonds are riskier, have lower ratings, but pay higher
        //   interest.  We will have two bonds from the same deal, "ACME-04",
        //   a senior bond (the "A" tranche) and a junior bond (the "J"
        //   tranche).
        // ====================================================================

        if (verbose) cout << "Usage Example\n";

        // pseudo code describing the schema
        // - - - - - - - - - - - - - - - - -
        //
        // bdem_EnumerationDef rating = { {0, "AAA"}, {1, "AA"}, {2, "A"},
        //                                {3, "BBB"}, {4, "BB"}, {5, "B"},
        //                                {6, "CCC"}, {7, "CC"}, {8, "C"},
        //                                {9, "D"},   {10, "NR"} };
        // bdem_RecordDef(sequence) location = {
        //   String "state";
        //   Double "percent";    // percent of homes in the deal in this state
        // };
        //
        // bdem_RecordDef(sequence) delinquency = {
        //   Double "percentDelinq60Plus";  // % homeowners over 60 days behind
        //   Double "percentDelinq90Plus";  // % homeowners over 90 days behind
        //   Double "percentForeclosed";    // % homeowners over foreclosed
        // };
        //
        // bdem_RecordDef(sequence) bond {
        //   String "name";
        //   String "rating" constrained by enum "rating" default "AAA";
        //   Table  "locationOfHomes" constrained by record "location";
        //   List   "delinquency" constrained by record "delinquency";
        //   Int    "avgFICOScore";    // avge FICO score of homeowners in deal
        //   Double "percentInterest"; // % interest the bond pays
        // };

        typedef bdem_SchemaAggregateUtil Util;

        bdem_Schema mS;    const bdem_Schema& S = mS;

        // First create the "RATING" enumeration constraint:

        bdem_EnumerationDef *rating = mS.createEnumeration("RATING");
        const char *ratings[] = { "AAA", "AA", "A", "BBB", "BB", "B",
                                  "CCC", "CC", "C", "D", "NR" };
        const int NUM_RATINGS = sizeof ratings / sizeof *ratings;
        for (int i = 0; i < NUM_RATINGS; ++i) {
            rating->addEnumerator(ratings[i]);
        }

        // Now create the "LOCATION" sequence record definition:

        bdem_RecordDef *location = mS.createRecord("LOCATION");
        location->appendField(bdem_ElemType::BDEM_STRING, "state");
        location->appendField(bdem_ElemType::BDEM_DOUBLE, "percent");

        const int stateIndex = location->fieldIndex("state");
        const int percentIndex = location->fieldIndex("percent");

        // Next, the "DELINQUENCY" sequence record definition:

        bdem_RecordDef *delinquency = mS.createRecord("DELINQUENCY");
        delinquency->appendField(bdem_ElemType::BDEM_DOUBLE,
                                 "percentDelinq60Plus");
        delinquency->appendField(bdem_ElemType::BDEM_DOUBLE,
                                 "percentDelinq90Plus");
        delinquency->appendField(bdem_ElemType::BDEM_DOUBLE,
                                 "percentForeclosed");

        const int sixtyIndex = delinquency->fieldIndex("percentDelinq60Plus");
        const int ninetyIndex = delinquency->fieldIndex("percentDelinq90Plus");
        const int forecloseIndex =delinquency->fieldIndex("percentForeclosed");

        // Then, the "BOND" sequence record definition:

        bdem_RecordDef *bond = mS.createRecord("BOND");
        bond->appendField(bdem_ElemType::BDEM_STRING, "name");
        {
            bdem_FieldDefAttributes fieldAttr(bdem_ElemType::BDEM_STRING);
            fieldAttr.defaultValue().theModifiableString() = "AAA";
            bond->appendField(fieldAttr, rating, "rating");
        }
        bond->appendField(bdem_ElemType::BDEM_TABLE,
                          location,
                          "locationOfHomes");
        ASSERT(0 == bond->lookupField("locationOfHomes")->isNullable());
        bond->appendField(bdem_ElemType::BDEM_LIST,
                          delinquency,
                          "delinquency");
        ASSERT(0 == bond->lookupField("delinquency")->isNullable());
        bond->appendField(bdem_ElemType::BDEM_INT, "avgFICOScore");
        bond->appendField(bdem_ElemType::BDEM_DOUBLE, "percentInterest");

        const int nameIndex = bond->fieldIndex("name");
        const int ratingIndex = bond->fieldIndex("rating");
        const int locationIndex = bond->fieldIndex("locationOfHomes");
        const int delinquencyIndex = bond->fieldIndex("delinquency");
        const int ficoIndex = bond->fieldIndex("avgFICOScore");
        const int interestIndex = bond->fieldIndex("percentInterest");

        // We're done with the definitions, now let's start initializing some
        // data:

        bdem_List acme04DA;

        // 'initListAllNull' will satisfy all forms of conformance and
        // satisfaction except for deep conformance:

        Util::initListAllNull(&acme04DA, *bond);
        ASSERT(1 == Util::isListShallowConformant(acme04DA, *bond));
        ASSERT(1 == Util::isListConformant(       acme04DA, *bond));
        ASSERT(0 == Util::isListDeepConformant(   acme04DA, *bond));
        ASSERT(1 == Util::canSatisfyRecord(       acme04DA, *bond));

        // Verify that the types of a few of the fields are correct:

        ASSERT(acme04DA[ratingIndex].type() == bdem_ElemType::BDEM_STRING);
        ASSERT(acme04DA[ratingIndex].isNull());
        ASSERT(acme04DA[locationIndex].type() == bdem_ElemType::BDEM_TABLE);
        ASSERT(acme04DA[locationIndex].isNull());
        ASSERT(acme04DA[delinquencyIndex].type() == bdem_ElemType::BDEM_LIST);
        ASSERT(acme04DA[delinquencyIndex].isNull());

        // Note that 'initListAllNull' did not initialize contained aggregates:

        ASSERT(0 == Util::isTableShallowConformant(
                               acme04DA[locationIndex].theTable(), *location));
        ASSERT(0 == Util::isListShallowConformant(
                          acme04DA[delinquencyIndex].theList(), *delinquency));

        // 'initListShallow' is like 'initListAllNull' except that it
        // initializes default values:

        Util::initListShallow(&acme04DA, *bond);
        ASSERT(1 == Util::isListShallowConformant(acme04DA, *bond));
        ASSERT(1 == Util::isListConformant(       acme04DA, *bond));
        ASSERT(0 == Util::isListDeepConformant(   acme04DA, *bond));
        ASSERT(1 == Util::canSatisfyRecord(       acme04DA, *bond));

        // The types are as before, except this time the rating has been
        // initialized to its default value:

        ASSERT(acme04DA[ratingIndex].type() == bdem_ElemType::BDEM_STRING);
        ASSERT(acme04DA[ratingIndex].theString() == "AAA");    // default value
        ASSERT(acme04DA[locationIndex].type() == bdem_ElemType::BDEM_TABLE);
        ASSERT(acme04DA[locationIndex].isNull());
        ASSERT(acme04DA[delinquencyIndex].type() == bdem_ElemType::BDEM_LIST);
        ASSERT(acme04DA[delinquencyIndex].isNull());

        // Note 'initListShallow' did not initialize sub aggregates:

        ASSERT(0 == Util::isTableShallowConformant(
                               acme04DA[locationIndex].theTable(), *location));
        ASSERT(0 == Util::isListShallowConformant(
                          acme04DA[delinquencyIndex].theList(), *delinquency));

        // 'initListDeep' will deeply initialize sub-aggregates:

        Util::initListDeep(&acme04DA, *bond);
        ASSERT(1 == Util::isListShallowConformant(acme04DA, *bond));
        ASSERT(1 == Util::isListConformant(       acme04DA, *bond));
        ASSERT(1 == Util::isListDeepConformant(   acme04DA, *bond));
        ASSERT(1 == Util::canSatisfyRecord(       acme04DA, *bond));

        ASSERT(acme04DA[ratingIndex].type() == bdem_ElemType::BDEM_STRING);
        ASSERT(acme04DA[ratingIndex].theString() == "AAA");    // default value
        ASSERT(acme04DA[locationIndex].type() == bdem_ElemType::BDEM_TABLE);
        ASSERT(0 == acme04DA[locationIndex].isNull());
        ASSERT(acme04DA[delinquencyIndex].type() ==
                                                     bdem_ElemType::BDEM_LIST);
        ASSERT(! acme04DA[delinquencyIndex].isNull());

        bdem_Table *table = &acme04DA[locationIndex].theModifiableTable();
        bdem_List  *list =&acme04DA[delinquencyIndex].theModifiableList();

        ASSERT(1 == Util::isTableShallowConformant(*table, *location));
        ASSERT(1 == Util::isTableConformant(       *table, *location));
        ASSERT(1 == Util::isTableDeepConformant(   *table, *location));
        ASSERT(1 == Util::canSatisfyRecord(        *table, *location));

        ASSERT(1 == Util::isListShallowConformant(*list, *delinquency));
        ASSERT(1 == Util::isListConformant(       *list, *delinquency));
        ASSERT(1 == Util::isListDeepConformant(   *list, *delinquency));
        ASSERT(1 == Util::canSatisfyRecord(       *list, *delinquency));

        // Now populate the new bond with some actual data:

        acme04DA[nameIndex].theModifiableString() = "ACME-04-D A";
        acme04DA[ratingIndex].theModifiableString() = "AA";

        {
            bdem_List locationList;
            Util::initListShallow(&locationList, *location);
            locationList[stateIndex].theModifiableString() = "NJ";
            locationList[percentIndex].theModifiableDouble() = 35.7;

            ASSERT(1 == Util::isListDeepConformant(locationList, *location));
            table->appendRow(locationList);

            locationList[stateIndex].theModifiableString() = "NY";
            locationList[percentIndex].theModifiableDouble() = 21.3;
            table->appendRow(locationList);

            locationList[stateIndex].theModifiableString() = "PA";
            locationList[percentIndex].theModifiableDouble() = 12.6;
            table->appendRow(locationList);

            ASSERT(1 == Util::isTableDeepConformant(*table, *location));
        }

        // Populate the delinquency list:

        (*list)[sixtyIndex].theModifiableDouble()     = 5.7;
        (*list)[ninetyIndex].theModifiableDouble()    = 3.2;
        (*list)[forecloseIndex].theModifiableDouble() = 2.1;

        acme04DA[ficoIndex].theModifiableInt() = 703;
        acme04DA[interestIndex].theModifiableDouble() = 5.5;

        ASSERT(1 == Util::isListShallowConformant(acme04DA, *bond));
        ASSERT(1 == Util::isListConformant(       acme04DA, *bond));
        ASSERT(1 == Util::isListDeepConformant(   acme04DA, *bond));
        ASSERT(1 == Util::canSatisfyRecord(       acme04DA, *bond));

        // Print out the bond:

        if (verbose) cout << "ACME-04-D A:" << acme04DA << endl;

        // Which prints out ACME-04-D A:{ STRING ACME-04-D A STRING AA
        // TABLE { Column Types: [ STRING DOUBLE ] Row 0: { NJ 35.7 }
        // Row 1: { NY 21.3 } Row 2: { PA 12.6 } } LIST { DOUBLE 5.7
        // DOUBLE 3.2 DOUBLE 2.1 } INT 703 DOUBLE 5.5 } all on one line.

        // Create another bond, "ACME-04-D J" - it is from the same "deal" or
        // family of bonds, so much of the data will be the same, but it is of
        // a lower tranche, so its rating will be lower and it will pay better
        // interest:

        bdem_List acme04DJ = acme04DA;

        acme04DJ[nameIndex].theModifiableString() = "ACME-04-D J";
        acme04DJ[ratingIndex].theModifiableString() = "C";
        acme04DJ[interestIndex].theModifiableDouble() = 6.1;

        ASSERT(Util::isListDeepConformant(acme04DJ, *bond));

        if (verbose) cout << "ACME-04-D J:" << acme04DJ << endl;

        // Which prints out ACME-04-D J:{ STRING ACME-04-D J STRING C
        // TABLE { Column Types: [ STRING DOUBLE ] Row 0: { NJ 35.7 }
        // Row 1: { NY 21.3 } Row 2: { PA 12.6 } } LIST { DOUBLE 5.7
        // DOUBLE 3.2 DOUBLE 2.1 } INT 703 DOUBLE 6.1 } all on one line.

        // Now we create another bond that we will manipulate to test various
        // types of conformance and satisfaction:

        bdem_List testBond(acme04DJ);
        ASSERT(1 == Util::isListDeepConformant(   testBond, *bond));

        // Note that appending a stray element breaks all forms of conformance,
        // but not satisfaction:

        testBond.appendInt(5);
        ASSERT(0 == Util::isListShallowConformant(testBond, *bond));
        ASSERT(0 == Util::isListConformant(       testBond, *bond));
        ASSERT(0 == Util::isListDeepConformant(   testBond, *bond));
        ASSERT(1 == Util::canSatisfyRecord(       testBond, *bond));
        testBond.removeElement(testBond.length() - 1);
        ASSERT(1 == Util::isListDeepConformant(   testBond, *bond));

        // Setting the rating (which is constrained by an enum) to an invalid
        // value is caught by everything but shallow conformance:

        testBond[ratingIndex].theModifiableString() = "Z";
        ASSERT(1 == Util::isListShallowConformant(testBond, *bond));
        ASSERT(0 == Util::isListConformant(       testBond, *bond));
        ASSERT(0 == Util::isListDeepConformant(   testBond, *bond));
        ASSERT(0 == Util::canSatisfyRecord(       testBond, *bond));
        testBond[ratingIndex].theModifiableString() = "AA";

        ASSERT(1 == Util::isListDeepConformant(   testBond, *bond));

        // Changing the type of the last element breaks all conformance and
        // satisfaction:

        ASSERT(testBond.length() - 1 == interestIndex);
        ASSERT(testBond[interestIndex].type() == bdem_ElemType::BDEM_DOUBLE);
        testBond.removeElement(interestIndex);
        testBond.appendNullElement(bdem_ElemType::BDEM_INT);
        ASSERT(0 == Util::isListShallowConformant(testBond, *bond));
        ASSERT(0 == Util::isListConformant(       testBond, *bond));
        ASSERT(0 == Util::isListDeepConformant(   testBond, *bond));
        ASSERT(0 == Util::canSatisfyRecord(       testBond, *bond));

        testBond.removeElement(interestIndex);
        testBond.appendNullElement(bdem_ElemType::BDEM_DOUBLE);
        ASSERT(1 == Util::isListDeepConformant(   testBond, *bond));

        // Setting the delinquency sublist to null only breaks deep
        // conformance:

        testBond[delinquencyIndex].makeNull();
        ASSERT(1 == Util::isListShallowConformant(testBond, *bond));
        ASSERT(1 == Util::isListConformant(       testBond, *bond));
        ASSERT(0 == Util::isListDeepConformant(   testBond, *bond));
        ASSERT(1 == Util::canSatisfyRecord(       testBond, *bond));
      } break;
      case 25: {
        // ====================================================================
        // VERIFY THAT UNCONSTRAINED AGGREGATES ARE DEEP CONFORMANT
        //
        // Concerns:
        //   Verify that unconstrained aggregates initialized to null are
        //   deep conformant.
        //
        // Plan:
        //   Create a list with all forms of unconstrained aggregates in it,
        //   initialized to null, and verify that they are deep conformant.
        // ====================================================================

        if (verbose) cout <<
                           "Shallow conformance of unconstrained aggregates\n";

        bslma_TestAllocator ta(veryVeryVerbose), *Z = &ta;

        const char *schemaStrs[] = { ":*U*&NF", ":*V*&NF",
                                     ":*e*&NF", ":*f*&NF" };
        const int NUM_SCHEMAS = sizeof schemaStrs / sizeof *schemaStrs;

        for (int i = 0; i < NUM_SCHEMAS; ++i) {
            Schema s(Z);    gg(&s, schemaStrs[i]);

            const RecDef& recInit = s.record(0);

            List list(Z);
            Util::initListShallow(&list, recInit);

            ASSERT(Util::isRowShallowConformant(list.row(), recInit));
            ASSERT(Util::isRowConformant(       list.row(), recInit));
            ASSERT(Util::isRowDeepConformant(   list.row(), recInit));

            ASSERT(Util::isListShallowConformant(list,      recInit));
            ASSERT(Util::isListConformant(       list,      recInit));
            ASSERT(Util::isListDeepConformant(   list,      recInit));
        }
      } break;
      case 24: {
        // ====================================================================
        // REPRODUCE BUG IN CONFORMANCE TESTING
        //
        // Concerns:
        //   A bug was discoved in the conformance data testing code where a
        //   record is pronounced conformant when the first unset element was
        //   found.  The code should have skipped the element and continued
        //   checking.
        //
        // Plan:
        //   Create two similar schema record definitions 'recA' and 'recB'.
        //   Create a list conformant to 'recA' and with its first element
        //   null.  Have the list be shallow conformant but not otherwise
        //   conformant to 'recB'.  Verify that the list is not plain or deep
        //   conformant to 'recB'.
        // ====================================================================

        bslma_TestAllocator ta(veryVeryVerbose), *Z = &ta;

        Schema s(Z);    gg(&s, ":cA* :dB* :aA*+*c :bA*+*d");

        const RecDef& recA = *s.lookupRecord("a");
        const RecDef& recB = *s.lookupRecord("b");

        List list(Z);    const List& LIST = list;
        Util::initListDeep(&list, recA);

        ASSERT(2 == LIST.length());
        ASSERT(bdem_ElemType::BDEM_CHAR == LIST[0].type());
        ASSERT(bdem_ElemType::BDEM_LIST == LIST[1].type());
        ASSERT(bdem_ElemType::BDEM_CHAR == LIST[1].theList()[0].type());
        ASSERT(1 == LIST[1].theList().length());
        ASSERT(1 == LIST[0].isNull());
        ASSERT(! LIST[1].isNull());
        ASSERT(1 == LIST[1].theList()[0].isNull());

        ASSERT(1 == Util::isListShallowConformant(LIST, recA));
        ASSERT(1 == Util::canSatisfyRecord(LIST, recA));
        ASSERT(1 == Util::isListConformant(LIST, recA));
        ASSERT(1 == Util::isListDeepConformant(LIST, recA));

        ASSERT(1 == Util::isListShallowConformant(LIST, recB));
        ASSERT(0 == Util::canSatisfyRecord(LIST, recB));
        ASSERT(0 == Util::isListConformant(LIST, recB));
        ASSERT(0 == Util::isListDeepConformant(LIST, recB));
      } break;
      case 23: {
        // ====================================================================
        // TEST UNSET STATE OF VARIABLE WITH DEFAULT VALUE
        //
        // Concerns:
        //   That objects are not properly initialized to the unset state
        //   by init functions.
        //
        // Plan:
        //   For all scalar types, have two schemas: schema A, which has the
        //   hasNoDefaultValue bit cleared but nothing has been done to the
        //   default value (it should be appropriately unset), and schema B,
        //   which is identical except it has a non-unset default value for
        //   the type.  Make sure the initListShallow, initListDeep,
        //   initRowShallow, and initRowDeep all do the right thing.
        // ====================================================================

        if (verbose) {
            cout << "Testing has default value flag, unset default value\n"
                    "===================================================\n";
        }

        {
            char schemaStrA[] = { ":*]*&nF&NF" };  // ']' is replaced by
                                                   // scalar or array type char
            char schemaStrB[] = { ":*]*&D0&NF" };

            // for all unconstrained bdem types
            for (const char *pc = bdemType; *pc; ++pc) {
                // skip aggregates
                if (strchr(bdemAggType, *pc)) {
                    continue;
                }

                schemaStrA[2] = *pc;
                schemaStrB[2] = *pc;

                Schema sA = g(schemaStrA);
                const RecDef &recA = sA.record(sA.numRecords() - 1);
                Schema sB = g(schemaStrB);
                const RecDef &recB = sB.record(sB.numRecords() - 1);

                bdem_ElemType::Type fieldType = recA.field(0).elemType();
                bdem_ElemType::Type defaultType =
                                           recA.field(0).defaultValue().type();

                if (veryVerbose) {
                    cout <<
                        "  : SchemaStrA: " << schemaStrA << ", array: " <<
                        bdem_ElemType::isArrayType(fieldType) <<
                                                            ", defScalar:  " <<
                        bdem_ElemType::isScalarType(defaultType) <<
                        "\n  : SchemaStrB: " << schemaStrB << endl;
                    P(sA) P(sB)
                }

                LOOP_ASSERT(*pc, ! recA.field(0).isNullable());
                LOOP_ASSERT(*pc, ! recA.field(0).hasDefaultValue());
                LOOP_ASSERT(*pc,   recA.field(0).defaultValue().isNull());

                LOOP_ASSERT(*pc, ! recB.field(0).isNullable());
                LOOP_ASSERT(*pc,   recB.field(0).hasDefaultValue());
                LOOP_ASSERT(*pc, ! recB.field(0).defaultValue().isNull());

                {
                    List list;
                    Util::initListShallow(&list, recA);
                    if (EType::isScalarType(recA.field(0).elemType())) {
                        LOOP2_ASSERT(*pc, list[0], list[0].isNull());
                    }
                    else {
                        LOOP2_ASSERT(*pc, list[0], list[0].isNull());
                    }
                    LOOP_ASSERT(*pc,  list[0].isNull());

                    Util::initListShallow(&list, recB);
                    if (EType::isScalarType(recA.field(0).elemType())) {
                        LOOP2_ASSERT(*pc, list[0], !list[0].isNull());
                    }
                    else {
                        LOOP2_ASSERT(*pc, list[0], list[0].isNull());
                    }
                    LOOP_ASSERT(*pc,  list[0].isNull() ==
                                                EType::isArrayType(fieldType));

                    Util::initRowShallow(&list.row(), recA);
                    if (EType::isScalarType(recA.field(0).elemType())) {
                        LOOP2_ASSERT(*pc, list[0], list[0].isNull());
                    }
                    else {
                        LOOP2_ASSERT(*pc, list[0], list[0].isNull());
                    }
                    LOOP_ASSERT(*pc,  list[0].isNull());
                }

                {
                    List list;
                    Util::initListDeep(&list, recA);
                    if (EType::isScalarType(recA.field(0).elemType())) {
                        LOOP2_ASSERT(*pc, list[0], list[0].isNull());
                    }
                    else {
                        LOOP2_ASSERT(*pc, list[0], list[0].isNull());
                    }
                    LOOP_ASSERT(*pc,  list[0].isNull());

                    Util::initListDeep(&list, recB);
                    if (EType::isScalarType(recA.field(0).elemType())) {
                        LOOP2_ASSERT(*pc, list[0], !list[0].isNull());
                    }
                    else {
                        LOOP2_ASSERT(*pc, list[0], list[0].isNull());
                    }
                    LOOP_ASSERT(*pc,  list[0].isNull() ==
                                                EType::isArrayType(fieldType));

                    Util::initRowDeep(&list.row(), recA);
                    if (EType::isScalarType(recA.field(0).elemType())) {
                        LOOP2_ASSERT(*pc, list[0], list[0].isNull());
                    }
                    else {
                        LOOP2_ASSERT(*pc, list[0], list[0].isNull());
                    }
                    LOOP_ASSERT(*pc,  list[0].isNull());
                }
            } // for '*pc' represents all unconstrained types
        }

        {
            char schemaStrA[] = { ":*]*&NT&nT" };      // ']' is replaced by
                                                       // scalar or array type
                                                       // char
            char schemaStrB[] = { ":*]*&NT&nT&D0" };

            // for all unconstrained bdem types
            for (const char *pc = bdemType; *pc; ++pc) {
                // skip aggregates
                if (strchr(bdemAggType, *pc)) {
                    continue;
                }

                schemaStrA[2] = *pc;
                schemaStrB[2] = *pc;

                Schema sA = g(schemaStrA);
                const RecDef &recA = sA.record(sA.numRecords() - 1);
                Schema sB = g(schemaStrB);
                const RecDef &recB = sB.record(sB.numRecords() - 1);

                bdem_ElemType::Type fieldType = recA.field(0).elemType();
                bool isArrayType = EType::isArrayType(fieldType);
                bdem_ElemType::Type defaultType =
                                           recA.field(0).defaultValue().type();

                ASSERT(recA.field(0).defaultValue().type() ==
                                          recB.field(0).defaultValue().type());

                if (veryVerbose) cout <<
                    "  : SchemaStrA: " << schemaStrA << ", array: " <<
                       bdem_ElemType::isArrayType(fieldType) <<
                                                            ", defScalar:  " <<
                                    bdem_ElemType::isScalarType(defaultType) <<
                  "\n  : SchemaStrB: " << schemaStrB << endl;

                LOOP_ASSERT(*pc,   recA.field(0).isNullable());
                LOOP_ASSERT(*pc, ! recA.field(0).hasDefaultValue());
                LOOP_ASSERT(*pc,   recA.field(0).defaultValue().isNull());

                LOOP_ASSERT(*pc,   recB.field(0).isNullable());
                LOOP_ASSERT(*pc,   recB.field(0).hasDefaultValue());
                LOOP_ASSERT(*pc, ! recB.field(0).defaultValue().isNull());

                {
                    List list;
                    Util::initListShallow(&list, recA);
                    LOOP_ASSERT(*pc, list[0].isNull());

                    Util::initListShallow(&list, recB);
                    LOOP_ASSERT(*pc, list[0].isNull() == isArrayType);

                    Util::initRowShallow(&list.row(), recA);
                    LOOP_ASSERT(*pc, list[0].isNull());
                }

                {
                    List list;
                    Util::initListDeep(&list, recA);
                    LOOP_ASSERT(*pc, list[0].isNull());

                    Util::initListDeep(&list, recB);
                    LOOP_ASSERT(*pc, list[0].isNull()  == isArrayType);

                    Util::initRowDeep(&list.row(), recA);
                    LOOP_ASSERT(*pc, list[0].isNull());
                }
            } // for *pc represents all unconstrained types
        }
      } break;
      case 22: {
        // ====================================================================
        // TEST RECURSIVE SCHEMA
        //
        // Concerns:
        //   That 'init' and checking routines can handle a recursive schema.
        //
        // Plan:
        //   gg() can't make a recursive schema, so we have to build them by
        //   hand.
        // ====================================================================

        if (verbose) cout << "Testing recursive schema\n"
                             "========================\n";

        Schema s;

        RecDef *a = s.createRecord("a");
        RecDef *b = s.createRecord("b");
        RecDef *c = s.createRecord("c");

        {
            a->appendField(EType::BDEM_LIST, b);
        }
        {
            b->appendField(EType::BDEM_LIST, c);
        }
        {
            FldAttr fa(EType::BDEM_LIST, true, 0);
            c->appendField(fa, a);
        }

        List list;
        Util::initListAllNull(&list, *a);

        Util::initRowDeep(&list.row(), *a);

        if (veryVerbose) P(list);

        Util::initListAllNull(&list, *a);

        Util::initListDeep(&list, *a);

        if (veryVerbose) P(list);
      } break;
      case 21: {
        // ====================================================================
        // TEST INITIALIZATION
        //
        // Concerns:
        //   That fields get initialized to null when the schema specifies
        //   it.
        //   This is only meaningful in the cases of the object being directly
        //   within a list, or within a nested list.
        //
        // Plan:
        //   Create schemas that specify objects are to be null and verify
        //   after creating lists, that the fields are indeed null.
        // ====================================================================

        bslma_TestAllocator  testAllocator(veryVeryVerbose);
        bslma_Allocator     *Z = &testAllocator;

        { // directly within a list
            // for all scalar and array types
            for (const char *pType = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
                                                             *pType; ++pType) {
                if (EType::isAggregateType(getType(*pType))) {
                    continue;
                }

                char schema[] = { ":m[*&NT&nT" };  // '[' will be replaced
                                                // by *pType
                schema[2] = *pType;

                Schema s(Z);                gg(&s, schema);
                const RecDef& RM = *s.lookupRecord("m"); // m for main

                {
                    List list(Z);

                    Util::initListShallow(&list, RM);
                    ASSERT(1 == list[0].isNull());
                }

                {
                    List list(Z);

                    Util::initListDeep(&list, RM);
                    ASSERT(1 == list[0].isNull());
                }
            } // for types
        } // directly within list

        { // within nested list
            // for all scalar and array types
            for (const char *pType = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
                                                             *pType; ++pType) {
                if (EType::isAggregateType(getType(*pType))) {
                    continue;
                }

                char schema[] = { ":a[*&NT&nTA*B* :m+*a" };   // '[' will be
                                                           // replaced by
                                                           // *pType
                schema[2] = *pType;

                Schema s(Z);                gg(&s, schema);
                List list(Z);

                const RecDef& RM = *s.lookupRecord("m"); // m for main

                Util::initListDeep(&list, RM);
                ASSERT(1 == list[0].theModifiableList()[0].isNull());
            }
        }
      } break;
      case 20: {
        // ====================================================================
        // TEST NULL CHECKING
        //
        // Concerns:
        //   That the init and checking functions in this component check nulls
        //   appropriately.
        //
        // Plan:
        //   create schemas that have non-nullable objects of all scalar
        //   and array types, within a list and within all 4 types of
        //   aggregates within a list.  Verify that all checks pass when
        //   the object is unset, and that deep checks fail when the object
        //   is null.
        // ====================================================================

        bslma_TestAllocator  testAllocator(veryVeryVerbose);
        bslma_Allocator     *Z = &testAllocator;

        { // directly within a list
            // for all scalar and array types
            for (const char *pType = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
                                                             *pType; ++pType) {
                if (EType::isAggregateType(getType(*pType))) {
                    continue;
                }

                char schema[] = { ":m[*&NF" };  // '[' will be replaced
                                                // by *pType
                schema[2] = *pType;

                Schema s(Z);                gg(&s, schema);
                List list(Z);

                const RecDef& RM = *s.lookupRecord("m"); // m for main

                Util::initListAllNull(&list, RM);
                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));

                list[0].makeNull();
                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));
            }
        }

        { // within a nested list
            // for all scalar and array types
            for (const char *pType = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
                                                             *pType; ++pType) {
                if (EType::isAggregateType(getType(*pType))) {
                    continue;
                }

                char schema[] = { ":a[*&NFA*&NTB*&NT :m+*a" };   // '[' will be
                                                           // replaced by
                                                           // *pType
                schema[2] = *pType;

                Schema s(Z);                gg(&s, schema);
                List list(Z);

                const RecDef& RM = *s.lookupRecord("m"); // m for main

                Util::initListDeep(&list, RM);
                LOOP_ASSERT(list[0], list[0].theList()[0].isNull());
                ASSERT(1 == list[0].theList()[1].isNull());
                ASSERT(1 == list[0].theList()[2].isNull());

                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));

                list[0].theModifiableList()[0].makeNull();
                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));

                list[0].theModifiableList()[0].makeNull();
                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));
            }
        }

        { // within a table
            // for all scalar and array types
            for (const char *pType = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
                                                             *pType; ++pType) {
                if (EType::isAggregateType(getType(*pType))) {
                    continue;
                }

                char schema[] = { ":a[*&NFA*&NTB*&NT :m#*a" };   // '[' will be
                                                           // replaced by
                                                           // *pType
                schema[2] = *pType;

                Schema s(Z);                gg(&s, schema);
                List list(Z);

                const RecDef& RM = *s.lookupRecord("m"); // m for main

                Util::initListDeep(&list, RM);
                bdem_Table& table = list[0].theModifiableTable();
                table.appendNullRow();
                table.theModifiableRow(0)[1].makeNull();
                table.theModifiableRow(0)[2].makeNull();

                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));

                table.theModifiableRow(0)[0].makeNull();
                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));

                table.theModifiableRow(0)[0].makeNull();
                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));

                table.theModifiableRow(0)[1].makeNull();
                table.theModifiableRow(0)[2].makeNull();
                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));
            }
        }

        { // within a choice
            // for all scalar and array types
            for (const char *pType = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
                                                             *pType; ++pType) {
                if (EType::isAggregateType(getType(*pType))) {
                    continue;
                }

                char schema[] = { ":a?[*&NFB* :m%*a" };  // '[' will be
                                                         // replaced by
                                                         // *pType
                schema[3] = *pType;

                Schema s(Z);                gg(&s, schema);
                List list(Z);

                const RecDef& RM = *s.lookupRecord("m"); // m for main

                Util::initListDeep(&list, RM);
                bdem_Choice& choice = list[0].theModifiableChoice();
                ASSERT(-1 == choice.selector());
                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));

                choice.makeSelection(0);
                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));

                choice.selection().makeNull();
                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));

                choice.selection().makeNull();
                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));

                choice.makeSelection(1);
                ASSERT(choice.selection().isNull());
                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));
            }
        }

        { // within a choiceArray
            // for all scalar and array types
            for (const char *pType = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
                                                             *pType; ++pType) {
                if (EType::isAggregateType(getType(*pType))) {
                    continue;
                }

                char schema[] = { ":a?[*&NFA*&NTB*&NT :m@*a" };  // '[' will be
                                                           // replaced by
                                                           // *pType
                schema[3] = *pType;

                Schema s(Z);                gg(&s, schema);
                List list(Z);

                const RecDef& RM = *s.lookupRecord("m"); // m for main

                Util::initListDeep(&list, RM);
                bdem_ChoiceArray& choiceArray =
                                            list[0].theModifiableChoiceArray();
                choiceArray.appendNullItems(1);

                ASSERT(-1 == choiceArray[0].selector());
                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));

                choiceArray.theModifiableItem(0).makeSelection(0);
                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));

                choiceArray.theModifiableItem(0).selection().makeNull();
                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));

                choiceArray.theModifiableItem(0).selection().makeNull();
                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));

                choiceArray.theModifiableItem(0).makeSelection(1);
                choiceArray.theModifiableItem(0).selection().makeNull();
                ASSERT(choiceArray[0].selection().isNull());
                LOOP_ASSERT(*pType, Util::isListShallowConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListConformant(list, RM));
                LOOP_ASSERT(*pType, Util::isListDeepConformant(list, RM));
                LOOP_ASSERT(*pType, Util::canSatisfyRecord(list, RM));
            }
        }
      } break;
      case 19: {
        // ====================================================================
        // TEST ENUMERATED CONSTRAINTS CHECKING WITHIN AGGREGATES
        //
        // Concerns:
        //   That objects constrained by enumerations that are nested within
        //   aggregates are being checked properly.
        //
        // Plan:
        //   For every enumerated type:
        //     For every aggregate type:
        //       - Build a schema of the enumerated type constrained and held
        //         within the aggregate type.
        //       - If the enumerated type is a scalar
        //         - verify that when the scalar is unset it passes checks
        //         - verify that when the scalar has a value that does not
        //           match constraints it fails deep checks
        //         - verify that when the scalar has a non-unset value that
        //           matches constraints is passes deep checks
        //       - If the enumerated type is an array
        //         - verify that when the array is empty it passes checks
        //         - verify that when the array contains a single unset element
        //           it passes checks.
        //         - verify that when the array contains an element that does
        //           not match the constraint, if fails deep checks.
        //         - verify that when the array contains only an unset element
        //           and one element that matches constraints it passes checks.
        // ====================================================================

        bslma_TestAllocator  testAllocator(veryVeryVerbose);
        bslma_Allocator     *Z = &testAllocator;

        { // enumerated ints
            { // within list
                Schema s(Z);    s = g(":a=a6b7c8 :b$*a&NT :m+*b");
                const RecDef& RM = *s.lookupRecord("m"); // m for main
                List list(Z);

                Util::initListDeep(&list, RM);
                List& nested = list[0].theModifiableList();
                ASSERT(nested[0].isNull());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                nested[0].theModifiableInt() = 20;
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(0 == Util::isListConformant(list, RM));
                ASSERT(0 == Util::isListDeepConformant(list, RM));
                ASSERT(0 == Util::canSatisfyRecord(list, RM));

                nested[0].theModifiableInt() = 7;
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));
            }

            { // within table
                Schema s(Z);    s = g(":a=a6b7c8 :b$*a :m#*b");
                const RecDef& RM = *s.lookupRecord("m"); // m for main
                List list(Z);

                Util::initListDeep(&list, RM);
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bdem_Table& table = list[0].theModifiableTable();

                table.appendNullRow();
                ASSERT(1 == table[0][0].isNull());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                table.theModifiableRow(0)[0].theModifiableInt() = 20;
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(0 == Util::isListConformant(list, RM));
                ASSERT(0 == Util::isListDeepConformant(list, RM));
                ASSERT(0 == Util::canSatisfyRecord(list, RM));

                table.theModifiableRow(0)[0].theModifiableInt() = 7;
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));
            }

            { // within choice
                Schema s(Z);    s = g(":a=a6b7c8 :b?$*a :m%*b");
                const RecDef& RM = *s.lookupRecord("m"); // m for main
                List list(Z);

                Util::initListDeep(&list, RM);
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bdem_Choice& choice = list[0].theModifiableChoice();
                choice.makeSelection(0);
                ASSERT(choice.selection().isNull());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                choice.selection().theModifiableInt() = 20;
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(0 == Util::isListConformant(list, RM));
                ASSERT(0 == Util::isListDeepConformant(list, RM));
                ASSERT(0 == Util::canSatisfyRecord(list, RM));

                choice.selection().theModifiableInt() = 7;
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));
            }

            { // within choice array
                Schema s(Z);    s = g(":a=a6b7c8 :b?$*a :m@*b&NF");
                const RecDef& RM = *s.lookupRecord("m"); // m for main
                List list(Z);

                Util::initListDeep(&list, RM);
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bdem_ChoiceArray& choiceArray =
                                            list[0].theModifiableChoiceArray();
                choiceArray.appendNullItems(1);
                choiceArray.theModifiableItem(0).makeSelection(0);
                ASSERT(choiceArray[0].selection().isNull());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                choiceArray.theModifiableItem(0).selection().
                                                       theModifiableInt() = 20;
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(0 == Util::isListConformant(list, RM));
                ASSERT(0 == Util::isListDeepConformant(list, RM));
                ASSERT(0 == Util::canSatisfyRecord(list, RM));

                choiceArray.theModifiableItem(0).selection().
                                                       theModifiableInt() = 7;
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));
            }
        } // enumerated ints

        { // enumerated strings
            { // within list
                Schema s(Z);    s = g(":a=a6b7c8 :b^*a&NT :m+*b");
                const RecDef& RM = *s.lookupRecord("m"); // m for main
                List list(Z);

                Util::initListDeep(&list, RM);
                List& nested = list[0].theModifiableList();
                ASSERT(nested[0].isNull());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                nested[0].theModifiableString() = "woof";
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(0 == Util::isListConformant(list, RM));
                ASSERT(0 == Util::isListDeepConformant(list, RM));
                ASSERT(0 == Util::canSatisfyRecord(list, RM));

                nested[0].theModifiableString() = "b";
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));
            }

            { // within table
                Schema s(Z);    s = g(":a=a6b7c8 :b^*a :m#*b");
                const RecDef& RM = *s.lookupRecord("m"); // m for main
                List list(Z);

                Util::initListDeep(&list, RM);
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bdem_Table& table = list[0].theModifiableTable();

                table.appendNullRow();
                ASSERT(1 == table[0][0].isNull());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                table.theModifiableRow(0)[0].theModifiableString() = "woof";
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(0 == Util::isListConformant(list, RM));
                ASSERT(0 == Util::isListDeepConformant(list, RM));
                ASSERT(0 == Util::canSatisfyRecord(list, RM));

                table.theModifiableRow(0)[0].theModifiableString() = "b";
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));
            }

            { // within choice
                Schema s(Z);    s = g(":a=a6b7c8 :b?^*a :m%*b");
                const RecDef& RM = *s.lookupRecord("m"); // m for main
                List list(Z);

                Util::initListDeep(&list, RM);
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bdem_Choice& choice = list[0].theModifiableChoice();
                choice.makeSelection(0);
                ASSERT(choice.selection().isNull());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                choice.selection().theModifiableString() = "woof";
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(0 == Util::isListConformant(list, RM));
                ASSERT(0 == Util::isListDeepConformant(list, RM));
                ASSERT(0 == Util::canSatisfyRecord(list, RM));

                choice.selection().theModifiableString() = "b";
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));
            }

            { // within choice array
                Schema s(Z);    s = g(":a=a6b7c8 :b?^*a :m@*b");
                const RecDef& RM = *s.lookupRecord("m"); // m for main
                List list(Z);

                Util::initListDeep(&list, RM);
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bdem_ChoiceArray& choiceArray =
                                            list[0].theModifiableChoiceArray();
                choiceArray.appendNullItems(1);
                choiceArray.theModifiableItem(0).makeSelection(0);
                ASSERT(choiceArray[0].selection().isNull());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                choiceArray.theModifiableItem(0).selection().
                                                theModifiableString() = "woof";
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(0 == Util::isListConformant(list, RM));
                ASSERT(0 == Util::isListDeepConformant(list, RM));
                ASSERT(0 == Util::canSatisfyRecord(list, RM));

                choiceArray.theModifiableItem(0).selection().
                                                   theModifiableString() = "b";
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));
            }
        } // enumerated strings

        { // enumerated int arrays
            { // within list
                Schema s(Z);    s = g(":a=a6b7c8 :b!*a&NT :m+*b");
                const RecDef& RM = *s.lookupRecord("m"); // m for main
                List list(Z);

                Util::initListDeep(&list, RM);
                List& nested = list[0].theModifiableList();
                ASSERT(nested[0].isNull());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bsl::vector<int>& intArray = nested[0].theModifiableIntArray();
                intArray.push_back(bdetu_Unset<int>::unsetValue());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                intArray.push_back(20);
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(0 == Util::isListConformant(list, RM));
                ASSERT(0 == Util::isListDeepConformant(list, RM));
                ASSERT(0 == Util::canSatisfyRecord(list, RM));

                intArray[1] = 7;
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));
            }

            { // within table
                Schema s(Z);    s = g(":a=a6b7c8 :b!*a&NT :m#*b");
                const RecDef& RM = *s.lookupRecord("m"); // m for main
                List list(Z);

                Util::initListDeep(&list, RM);
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bdem_Table& table = list[0].theModifiableTable();

                table.appendNullRow();
                ASSERT(1 == table[0][0].isNull());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bsl::vector<int>& intArray =
                          table.theModifiableRow(0)[0].theModifiableIntArray();
                intArray.push_back(bdetu_Unset<int>::unsetValue());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                intArray.push_back(20);
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(0 == Util::isListConformant(list, RM));
                ASSERT(0 == Util::isListDeepConformant(list, RM));
                ASSERT(0 == Util::canSatisfyRecord(list, RM));

                intArray[1] = 7;
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));
            }

            { // within choice
                Schema s(Z);    s = g(":a=a6b7c8 :b?!*a :m%*b");
                const RecDef& RM = *s.lookupRecord("m"); // m for main
                List list(Z);

                Util::initListDeep(&list, RM);
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bdem_Choice& choice = list[0].theModifiableChoice();
                choice.makeSelection(0);
                ASSERT(choice.selection().isNull());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bsl::vector<int>& intArray =
                                    choice.selection().theModifiableIntArray();
                intArray.push_back(bdetu_Unset<int>::unsetValue());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                intArray.push_back(20);
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(0 == Util::isListConformant(list, RM));
                ASSERT(0 == Util::isListDeepConformant(list, RM));
                ASSERT(0 == Util::canSatisfyRecord(list, RM));

                intArray[1] = 7;
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));
            }

            { // within choice array
                Schema s(Z);    s = g(":a=a6b7c8 :b?!*a :m@*b");
                const RecDef& RM = *s.lookupRecord("m"); // m for main
                List list(Z);

                Util::initListDeep(&list, RM);
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bdem_ChoiceArray& choiceArray =
                                            list[0].theModifiableChoiceArray();
                choiceArray.appendNullItems(1);
                choiceArray.theModifiableItem(0).makeSelection(0);
                ASSERT(choiceArray[0].selection().isNull());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bsl::vector<int>& intArray =
                                  choiceArray.theModifiableItem(0).selection().
                                                       theModifiableIntArray();
                intArray.push_back(bdetu_Unset<int>::unsetValue());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                intArray.push_back(20);
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(0 == Util::isListConformant(list, RM));
                ASSERT(0 == Util::isListDeepConformant(list, RM));
                ASSERT(0 == Util::canSatisfyRecord(list, RM));

                intArray[1] = 7;
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));
            }
        } // enumerated int arrays

        { // enumerated string arrays
            { // within list
                Schema s(Z);    s = g(":a=a6b7c8 :b/*a&NT :m+*b");
                const RecDef& RM = *s.lookupRecord("m"); // m for main
                List list(Z);

                Util::initListDeep(&list, RM);
                List& nested = list[0].theModifiableList();
                ASSERT(nested[0].isNull());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bsl::vector<bsl::string>& stringArray =
                                          nested[0].theModifiableStringArray();
                stringArray.push_back("");
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                stringArray.push_back("woof");
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(0 == Util::isListConformant(list, RM));
                ASSERT(0 == Util::isListDeepConformant(list, RM));
                ASSERT(0 == Util::canSatisfyRecord(list, RM));

                stringArray[1] = "b";
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));
            }

            { // within table
                Schema s(Z);    s = g(":a=a6b7c8 :b/*a :m#*b");
                const RecDef& RM = *s.lookupRecord("m"); // m for main
                List list(Z);

                Util::initListDeep(&list, RM);
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bdem_Table& table = list[0].theModifiableTable();

                table.appendNullRow();
                ASSERT(1 == table[0][0].isNull());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bsl::vector<bsl::string>& stringArray =
                       table.theModifiableRow(0)[0].theModifiableStringArray();
                stringArray.push_back("");
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                stringArray.push_back("woof");
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(0 == Util::isListConformant(list, RM));
                ASSERT(0 == Util::isListDeepConformant(list, RM));
                ASSERT(0 == Util::canSatisfyRecord(list, RM));

                stringArray[1] = "b";
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));
            }

            { // within choice
                Schema s(Z);    s = g(":a=a6b7c8 :b?/*a :m%*b");
                const RecDef& RM = *s.lookupRecord("m"); // m for main
                List list(Z);

                Util::initListDeep(&list, RM);
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bdem_Choice& choice = list[0].theModifiableChoice();
                choice.makeSelection(0);
                ASSERT(choice.selection().isNull());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bsl::vector<bsl::string>& stringArray =
                                 choice.selection().theModifiableStringArray();
                stringArray.push_back("");
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                stringArray.push_back("woof");
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(0 == Util::isListConformant(list, RM));
                ASSERT(0 == Util::isListDeepConformant(list, RM));
                ASSERT(0 == Util::canSatisfyRecord(list, RM));

                stringArray[1] = "b";
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));
            }

            { // within choice array
                Schema s(Z);    s = g(":a=a6b7c8 :b?/*a :m@*b");
                const RecDef& RM = *s.lookupRecord("m"); // m for main
                List list(Z);

                Util::initListDeep(&list, RM);
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bdem_ChoiceArray& choiceArray =
                                            list[0].theModifiableChoiceArray();
                choiceArray.appendNullItems(1);
                choiceArray.theModifiableItem(0).makeSelection(0);
                ASSERT(choiceArray[0].selection().isNull());
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                bsl::vector<bsl::string>& stringArray =
                                  choiceArray.theModifiableItem(0).selection().
                                                    theModifiableStringArray();
                stringArray.push_back("");
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));

                stringArray.push_back("woof");
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(0 == Util::isListConformant(list, RM));
                ASSERT(0 == Util::isListDeepConformant(list, RM));
                ASSERT(0 == Util::canSatisfyRecord(list, RM));

                stringArray[1] = "b";
                ASSERT(1 == Util::isListShallowConformant(list, RM));
                ASSERT(1 == Util::isListConformant(list, RM));
                ASSERT(1 == Util::isListDeepConformant(list, RM));
                ASSERT(1 == Util::canSatisfyRecord(list, RM));
            }
        } // enumerated string arrays
      } break;
      case 18: {
        // ====================================================================
        // TEST COMPLEX SCHEMAS WITHIN INITIALIZATION & CHECKING IN LISTS AND
        // ROWS
        //
        // Concerns:
        //   Given a deep, complex schema, initShallow should only satisfy
        //   is*ShallowConformant and fail canSatisfyRecord, isConformant,
        //   and isDeepConformant.
        //       Note that it is OK for a constrained list to be null, but
        //       anything with columns or a catalog will not be properly
        //       initialized by an init shallow.
        //
        // Plan:
        //   Specify a bunch of complex schemas using gg and verify for all
        //   of them that initShallow and initDeep have the proper functioning.
        //   Record "m" will always be the "main" record of the schema.
        // ====================================================================

        bslma_TestAllocator  testAllocator(veryVeryVerbose);
        bslma_Allocator     *Z = &testAllocator;

        struct {
            int         d_lineNum;
            const char *d_schema;
        } schemas[] = {
            { L_, ":aA*B*C*D*E*F*  :m#*a" },         // &NF means nulls are
            { L_, ":a?A*B*C*D*E*F* :m%*a" },         // not allowed
            { L_, ":a?A*B*C*D*E*F* :m@*a" },
            { L_, ":a=a2b3c4 :b=f4g5h6 :cA*B* :m$*a&D0$*b&D1#*c" },
            { L_, ":a=a2b3c4 :b$*a&D0A*B* :m#*b" },
            { L_, ":a=a2b3c4 :b?$*a&D0A*B* :m%*b" },
            { L_, ":a=a2b3c4 :b?$*a&D0A*B* :m@*b" },
        };

        enum { NUM_SCHEMAS = sizeof schemas / sizeof *schemas };

        for (int i = 0; i < NUM_SCHEMAS; ++i) {
            const char *SCHEMA = schemas[i].d_schema;

            Schema s = g(SCHEMA);

            if (veryVerbose) P(s);

            const RecDef& R = *s.lookupRecord("m"); // m for main
            bool ca = containsAggregate(R);

            List list(Z);

            Util::initListAllNull(&list, R);
            LOOP_ASSERT(SCHEMA,        Util::isListShallowConformant(list, R));
            LOOP_ASSERT(SCHEMA,        Util::isListConformant(list, R));
            LOOP_ASSERT(SCHEMA, ca == !Util::isListDeepConformant(list, R));
            LOOP_ASSERT(SCHEMA,        Util::canSatisfyRecord(list, R));

            list.removeAll();
            Util::initListAllNull(&list, R);
            LOOP_ASSERT(SCHEMA,        Util::isListShallowConformant(list, R));
            LOOP_ASSERT(SCHEMA,        Util::isListConformant(list, R));
            LOOP_ASSERT(SCHEMA, ca == !Util::isListDeepConformant(list, R));
            LOOP_ASSERT(SCHEMA,        Util::canSatisfyRecord(list, R));

            list.removeAll();
            Util::initListShallow(&list, R);
            LOOP_ASSERT(SCHEMA,        Util::isListShallowConformant(list, R));
            LOOP_ASSERT(SCHEMA,        Util::isListConformant(list, R));
            LOOP_ASSERT(SCHEMA, ca == !Util::isListDeepConformant(list, R));
            LOOP_ASSERT(SCHEMA,        Util::canSatisfyRecord(list, R));

            list.removeAll();
            Util::initListDeep(&list, R);
            LOOP_ASSERT(SCHEMA,        Util::isListShallowConformant(list, R));
            LOOP_ASSERT(SCHEMA,        Util::isListConformant(list, R));
            LOOP_ASSERT(SCHEMA,        Util::isListDeepConformant(list, R));
            LOOP_ASSERT(SCHEMA,        Util::canSatisfyRecord(list, R));

            list.removeAll();
            Util::initListAllNull(&list, R);
            Row *row = &list.row();
            Util::initRowShallow(row, R);
            LOOP_ASSERT(SCHEMA,        Util::isRowShallowConformant(*row, R));
            LOOP_ASSERT(SCHEMA,        Util::isRowConformant(*row, R));
            LOOP_ASSERT(SCHEMA, ca == !Util::isRowDeepConformant(*row, R));
            LOOP_ASSERT(SCHEMA,        Util::canSatisfyRecord(*row, R));

            list.removeAll();
            Util::initListAllNull(&list, R);
            row = &list.row();
            Util::initRowDeep(row, R);
            LOOP_ASSERT(SCHEMA,        Util::isRowShallowConformant(*row, R));
            LOOP_ASSERT(SCHEMA,        Util::isRowConformant(*row, R));
            LOOP_ASSERT(SCHEMA,        Util::isRowDeepConformant(*row, R));
            LOOP_ASSERT(SCHEMA,        Util::canSatisfyRecord(*row, R));
        }
      } break;
      case 17: {
        // ====================================================================
        // TEST ENUMERATED CONSTRAINTS
        //
        // Concerns:
        // - That enumerated constraints aren't being properly checked.
        //
        // Plan:
        // a: Make a schema involving all 4 enumerated constraint types,
        //    and verify in all cases that tests pass given
        //    - a value listed among the values in the EnumerationDef
        //    - an unset value, or
        //    - a null value
        // b: Verify that canSatisfyRecord, is*Conformant, is*DeepConformant
        //    all return false if an enumerated type has a value not listed
        //    under a: above.
        // c: Verify that is*ShallowConformant still returns true if the
        //    enumerated type has the wrong value, since that routine only
        //    checks for matching type information.
        // ====================================================================

        bslma_TestAllocator  testAllocator(veryVeryVerbose);
        bslma_Allocator     *Z = &testAllocator;
        int                  fmt = Format::BDEAT_DEFAULT;

        Schema s(Z);
        EnumDef *e = s.createEnumeration("a");
        e->addEnumerator("book", 4);
        e->addEnumerator("paper", 5);
        e->addEnumerator("pencil", 6);
        e->addEnumerator("pen", 7);

        RecDef& nested = *s.createRecord("nested");
        {
            FldAttr attr(EType::BDEM_INT, true, fmt);
            attr.defaultValue().theModifiableInt() = 5;
            nested.appendField(attr, e);
        }
        {
            FldAttr attr(EType::BDEM_STRING, true, fmt);
            attr.defaultValue().theModifiableString() = "pencil";
            nested.appendField(attr, e);
        }
        {
            FldAttr attr(EType::BDEM_INT_ARRAY, true, fmt);
            nested.appendField(attr, e);
        }
        {
            FldAttr attr(EType::BDEM_STRING_ARRAY, true, fmt);
            nested.appendField(attr, e);
        }

        RecDef& record = *s.createRecord("b");
        {
            FldAttr attr(EType::BDEM_INT, true, fmt);
            attr.defaultValue().theModifiableInt() = 5;
            record.appendField(attr, e);
        }
        {
            FldAttr attr(EType::BDEM_STRING, true, fmt);
            attr.defaultValue().theModifiableString() = "pencil";
            record.appendField(attr, e);
        }
        {
            FldAttr attr(EType::BDEM_INT_ARRAY, true, fmt);
            record.appendField(attr, e);
        }
        {
            FldAttr attr(EType::BDEM_STRING_ARRAY, true, fmt);
            record.appendField(attr, e);
        }
        {
            FldAttr attr(EType::BDEM_LIST, true, fmt);
            record.appendField(attr, &nested);
        }

        if (verbose) s.print(bsl::cout);

        {
            if (verbose) bsl::cout << "Testing Lists\n";

            List ll(Z);
            Util::initListDeep(&ll, record);
            {
                bsl::vector<int>& intArray = ll[2].theModifiableIntArray();
                intArray.push_back(4);
                intArray.push_back(5);
                intArray.push_back(6);
            }
            {
                bsl::vector<bsl::string>& stringArray =
                                              ll[3].theModifiableStringArray();
                stringArray.push_back("book");
                stringArray.push_back("pencil");
                stringArray.push_back("paper");
            }

            if (verbose) P(ll);
            ASSERT(totallyCorrect(ll, record));

            ll[0].makeNull();
            ll[1].makeNull();

            const int unsetInt =            ll[0].theInt();
            const bsl::string unsetString = ll[1].theString();

            ll[2].theModifiableIntArray()[0]    = unsetInt;
            ll[3].theModifiableStringArray()[0] = unsetString;

            if (veryVerbose) P(ll);
            ASSERT(totallyCorrect(ll, record));

            ll[0].makeNull();
            ll[1].makeNull();

            if (veryVerbose) P(ll);
            ASSERT(totallyCorrect(ll, record));

            ll[0].theModifiableInt() = 20;

            if (veryVerbose) P(ll);
            ASSERT(0 == Util::canSatisfyRecord(ll, record));
            ASSERT(0 == Util::isListConformant(ll, record));
            ASSERT(Util::isListShallowConformant(ll, record));
            ASSERT(0 == Util::isListDeepConformant(ll, record));

            ll[0].theModifiableInt() = 6;

            if (veryVerbose) P(ll);
            ASSERT(totallyCorrect(ll, record));

            ll[1].theModifiableString() = "woof";

            if (veryVerbose) P(ll);
            ASSERT(0 == Util::canSatisfyRecord(ll, record));
            ASSERT(0 == Util::isListConformant(ll, record));
            ASSERT(Util::isListShallowConformant(ll, record));
            ASSERT(0 == Util::isListDeepConformant(ll, record));

            ll[1].theModifiableString() = "book";

            if (veryVerbose) P(ll);
            ASSERT(totallyCorrect(ll, record));

            ll[2].theModifiableIntArray()[0] = 20;

            if (veryVerbose) P(ll);
            ASSERT(0 == Util::canSatisfyRecord(ll, record));
            ASSERT(0 == Util::isListConformant(ll, record));
            ASSERT(Util::isListShallowConformant(ll, record));
            ASSERT(0 == Util::isListDeepConformant(ll, record));

            ll[2].theModifiableIntArray()[0] = 4;

            if (veryVerbose) P(ll);
            ASSERT(totallyCorrect(ll, record));

            ll[3].theModifiableStringArray()[0] = "woof";

            if (veryVerbose) P(ll);
            ASSERT(0 == Util::canSatisfyRecord(ll, record));
            ASSERT(0 == Util::isListConformant(ll, record));
            ASSERT(Util::isListShallowConformant(ll, record));
            ASSERT(0 == Util::isListDeepConformant(ll, record));

            ll[3].theModifiableStringArray()[0] = "book";

            if (veryVerbose) P(ll);
            ASSERT(totallyCorrect(ll, record));

            ll[2].makeNull();
            ll[3].makeNull();

            if (veryVerbose) P(ll);
            ASSERT(totallyCorrect(ll, record));

            ll.removeAll();
            Util::initListAllNull(&ll, record);

            if (veryVerbose) P(ll);
            ASSERT(totallyCorrect(ll, record));

            ll.removeAll();
            Util::initListAllNull(&ll, record);

            if (veryVerbose) P(ll);
            ASSERT(totallyCorrect(ll, record));

            ll[4].theModifiableList().appendDouble(0.5);

            if (veryVerbose) P(ll);
            ASSERT(0 == Util::canSatisfyRecord(ll, record));
            ASSERT(0 == Util::isListConformant(ll, record));
            ASSERT(1 == Util::isListShallowConformant(ll, record));
            ASSERT(0 == Util::isListDeepConformant(ll, record));

            ll.removeAll();
            Util::initListShallow(&ll, record);

            ASSERT(totallyCorrect(ll, record));
        }

        {
            if (verbose) bsl::cout << "Testing Rows\n";

            List containsRow(Z);
            Util::initListAllNull(&containsRow, record);
            Row& row = containsRow.row();
            Util::initRowDeep(&row, record);
            {
                bsl::vector<int>& intArray = row[2].theModifiableIntArray();
                intArray.push_back(4);
                intArray.push_back(5);
                intArray.push_back(6);
            }
            {
                bsl::vector<bsl::string>& stringArray =
                                             row[3].theModifiableStringArray();
                stringArray.push_back("book");
                stringArray.push_back("pencil");
                stringArray.push_back("paper");
            }

            if (verbose) P(row);
            ASSERT(totallyCorrect(row, record));

            row[0].makeNull();
            row[1].makeNull();

            const int unsetInt =            row[0].theInt();
            const bsl::string unsetString = row[1].theString();

            row[2].theModifiableIntArray()[0]    = unsetInt;
            row[3].theModifiableStringArray()[0] = unsetString;

            if (veryVerbose) P(row);
            ASSERT(totallyCorrect(row, record));

            row[0].makeNull();
            row[1].makeNull();

            if (veryVerbose) P(row);
            ASSERT(totallyCorrect(row, record));

            row[0].theModifiableInt() = 20;

            if (veryVerbose) P(row);
            ASSERT(0 == Util::canSatisfyRecord(row, record));
            ASSERT(0 == Util::isRowConformant(row, record));
            ASSERT(Util::isRowShallowConformant(row, record));
            ASSERT(0 == Util::isRowDeepConformant(row, record));

            row[0].theModifiableInt() = 6;

            if (veryVerbose) P(row);
            ASSERT(totallyCorrect(row, record));

            row[1].theModifiableString() = "woof";

            if (veryVerbose) P(row);
            ASSERT(0 == Util::canSatisfyRecord(row, record));
            ASSERT(0 == Util::isRowConformant(row, record));
            ASSERT(Util::isRowShallowConformant(row, record));
            ASSERT(0 == Util::isRowDeepConformant(row, record));

            row[1].theModifiableString() = "book";

            if (veryVerbose) P(row);
            ASSERT(totallyCorrect(row, record));

            row[2].theModifiableIntArray()[0] = 20;

            if (veryVerbose) P(row);
            ASSERT(0 == Util::canSatisfyRecord(row, record));
            ASSERT(0 == Util::isRowConformant(row, record));
            ASSERT(Util::isRowShallowConformant(row, record));
            ASSERT(0 == Util::isRowDeepConformant(row, record));

            row[2].theModifiableIntArray()[0] = 4;

            if (veryVerbose) P(row);
            ASSERT(totallyCorrect(row, record));

            row[3].theModifiableStringArray()[0] = "woof";

            if (veryVerbose) P(row);
            ASSERT(0 == Util::canSatisfyRecord(row, record));
            ASSERT(0 == Util::isRowConformant(row, record));
            ASSERT(Util::isRowShallowConformant(row, record));
            ASSERT(0 == Util::isRowDeepConformant(row, record));

            row[3].theModifiableStringArray()[0] = "book";

            if (veryVerbose) P(row);
            ASSERT(totallyCorrect(row, record));

            row[2].makeNull();
            row[3].makeNull();

            if (veryVerbose) P(row);
            ASSERT(totallyCorrect(row, record));
        }

        {
            bdem_Table table(Z);

            Util::initTable(&table, record);
            table.appendNullRow();
            table.theModifiableRow(0)[0].theModifiableInt() = 5;
            table.theModifiableRow(0)[1].theModifiableString() = "pencil";
            table.theModifiableRow(0)[2].theModifiableIntArray().push_back(5);
            table.theModifiableRow(0)[3].theModifiableStringArray().
                                                           push_back("pencil");
            // leave the list [0][4] unset

            ASSERT(1 == Util::isTableShallowConformant(table, record));
            ASSERT(1 == Util::canSatisfyRecord(table, record));
            ASSERT(1 == Util::isTableConformant(table, record));
            ASSERT(1 == Util::isTableDeepConformant(table, record));

            table.theModifiableRow(0)[0].theModifiableInt() = 20;

            ASSERT(1 == Util::isTableShallowConformant(table, record));
            ASSERT(0 == Util::canSatisfyRecord(table, record));
            ASSERT(0 == Util::isTableConformant(table, record));
            ASSERT(0 == Util::isTableDeepConformant(table, record));

            table.theModifiableRow(0)[0].makeNull();

            ASSERT(1 == Util::isTableShallowConformant(table, record));
            ASSERT(1 == Util::canSatisfyRecord(table, record));
            ASSERT(1 == Util::isTableConformant(table, record));
            ASSERT(1 == Util::isTableDeepConformant(table, record));

            table.theModifiableRow(0)[1].theModifiableString() = "woof";

            ASSERT(1 == Util::isTableShallowConformant(table, record));
            ASSERT(0 == Util::canSatisfyRecord(table, record));
            ASSERT(0 == Util::isTableConformant(table, record));
            ASSERT(0 == Util::isTableDeepConformant(table, record));

            table.theModifiableRow(0)[1].makeNull();

            ASSERT(1 == Util::isTableShallowConformant(table, record));
            ASSERT(1 == Util::canSatisfyRecord(table, record));
            ASSERT(1 == Util::isTableConformant(table, record));
            ASSERT(1 == Util::isTableDeepConformant(table, record));

            table.theModifiableRow(0)[2].theModifiableIntArray()[0] = 20;

            ASSERT(1 == Util::isTableShallowConformant(table, record));
            ASSERT(0 == Util::canSatisfyRecord(table, record));
            ASSERT(0 == Util::isTableConformant(table, record));
            ASSERT(0 == Util::isTableDeepConformant(table, record));

            table.theModifiableRow(0)[2].theModifiableIntArray()[0] = 5;

            ASSERT(1 == Util::isTableShallowConformant(table, record));
            ASSERT(1 == Util::canSatisfyRecord(table, record));
            ASSERT(1 == Util::isTableConformant(table, record));
            ASSERT(1 == Util::isTableDeepConformant(table, record));

            table.theModifiableRow(0)[3].theModifiableStringArray()[0] =
                                                                        "woof";

            ASSERT(1 == Util::isTableShallowConformant(table, record));
            ASSERT(0 == Util::canSatisfyRecord(table, record));
            ASSERT(0 == Util::isTableConformant(table, record));
            ASSERT(0 == Util::isTableDeepConformant(table, record));
        }
      } break;
      case 16: {
        // ====================================================================
        // TEST PRINT
        //
        // Concerns:
        // - 'print' can be called on a variety of schemas and objects.
        // - The first line is indented by 'abs(level * spacesPerLevel)'
        //   unless 'level' is negative.
        // - The second line is indented by 'abs((level+1) * spacesPerLevel)'
        //   unless 'spacesPerLevel' is negative or unless the output is
        //   fewer than three lines.
        // - The last line is indented by 'abs(level * spacesPerLevel)'
        //   unless 'spacesPerLevel' is negative or unless the output is
        //   fewer than two lines.
        // - The first line is not indented if 'level' is negative.
        // - There are no newlines in the output if 'spacesPerLevel' is
        //   negative.
        // - The result of calling 'print' directly on an aggregate is
        //   the same as calling 'print' through an ElemRef to that aggregate.
        // - We are not concerned about the exact output format, except
        //   for indentation.
        //
        // Plan:
        // - Using a table of schema specifications, generate lists, rows,
        //   tables, choices, choice array items, and choice arrays conforming
        //   to different schemas.
        // - Call a template function, 'printTest', passing it the aggregate
        //   object and the schema.
        // - 'printTest' uses a vector of various 'level' and 'spacesPerLevel'
        //   values can calls 'bdem_SchemaAggregateUtil::print' for each
        //   combination.
        // - For each 'print' output, 'printTest' validates the concerns
        //   listed above.
        // - Finally, 'printTest' construct a 'bdem_ConstElemRef' from the
        //   aggregate and verifies that the output of 'print' for the ElemRef
        //   is the same as for the raw aggregate.
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream&             stream,
        //                       const bdem_ConstElemRef&  item,
        //                       const bdem_RecordDef     *constraint     = 0,
        //                       int                       level          = 0,
        //                       int                       spacesPerLevel = 4);
        //   bsl::ostream& print(bsl::ostream&         stream,
        //                       const bdem_Row&       item,
        //                       const bdem_RecordDef& record,
        //                       int                   level          = 0,
        //                       int                   spacesPerLevel = 4);
        //   bsl::ostream& print(bsl::ostream&         stream,
        //                       const bdem_List&      item,
        //                       const bdem_RecordDef& record,
        //                       int                   level          = 0,
        //                       int                   spacesPerLevel = 4);
        //   bsl::ostream& print(bsl::ostream&               stream,
        //                       const bdem_ChoiceArrayItem& item,
        //                       const bdem_RecordDef&       record,
        //                       int                         level         = 0,
        //                       int                         spacesPerLevel=4);
        //   bsl::ostream& print(bsl::ostream&         stream,
        //                       const bdem_Choice&    item,
        //                       const bdem_RecordDef& record,
        //                       int                   level          = 0,
        //                       int                   spacesPerLevel = 4);
        //   bsl::ostream& print(bsl::ostream&         stream,
        //                       const bdem_Table&     item,
        //                       const bdem_RecordDef& record,
        //                       int                   level          = 0,
        //                       int                   spacesPerLevel = 4);
        //   bsl::ostream& print(bsl::ostream&           stream,
        //                       const bdem_ChoiceArray& item,
        //                       const bdem_RecordDef&   record,
        //                       int                     level          = 0,
        //                       int                     spacesPerLevel = 4);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING PRINT"
                          << "\n=============" << endl;

        bsl::ostringstream stream;

        if (verbose) cout << "\tTesting list, row and table" << endl;
        {
            static const struct {
                int         d_lineNumber;
                const char *d_dataSpec;
            } DATA[] = {
                //line  data spec
                //----  ----------------------------
                { L_,   ":a"                         },
                { L_,   ":aAa"                       },
                { L_,   ":*Aa"                       },
                { L_,   ":aA*"                       },
                { L_,   ":*A*"                       },
                { L_,   ":aUa"                       },
                { L_,   ":aVa"                       },
                { L_,   ":aAa"                       },
                { L_,   ":aAaBb"                     },
                { L_,   ":aAaBbCcDdEeFfGgHhIiJjUuVv" },
                { L_,   ":aA*BbC*DdEeFfG*HhIiJjUuVv" },
                { L_,   ":aWaXbYcZdaebfcgdheifj"     },
                { L_,   ":aAa :b+a0"                 },
                { L_,   ":aAa :b+*0"                 },
                { L_,   ":aAa :b#a0"                 },
                { L_,   ":aAa :b#*0"                 },
                { L_,   ":a?Aa :b%a0"                },
                { L_,   ":a?Aa :b%*0"                },
                { L_,   ":a?AaBbCc :b%*0"            },
                { L_,   ":a?Aa :b@a0"                },
                { L_,   ":a?Aa :b@*0"                },
                { L_,   ":a?AaBbCc :b@*0"            },
                { L_,   ":aAa :b+a0Bb"               },
                { L_,   ":aAa :b+a0#b0"              },
                { L_,   ":a?Aa :b%a0Bb"              },
                { L_,   ":a?Aa :b%a0@b0"             },
                { L_,   ":a?AaBbCc :b%a0@b0"         },
                { L_,   ":aAaBb :b+a0"               },
                { L_,   ":aAa :b+a0 :c+a1"           },
                { L_,   ":aAa :b+a0Bb :c+a1"         },
                { L_,   ":aAaBb :b+a0 :c+a1"         },
                { L_,   ":aAa :b+a0 :c+a1 :d+a2"     },
                { L_,   ":aAaBb :b+a0 :c+a1 :d+a2"   },
                { L_,   ":aAa :b+a0Bb :c+a1 :d+a2"   },
                { L_,   ":aAa :b+a0 :c+a1Bb :d+a2"   },
                { L_,   ":aAa :b+a0 :c+a1 :d+a2Bb"   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE      = DATA[i].d_lineNumber;
                const char *DATA_SPEC = DATA[i].d_dataSpec;

                Schema ds;  gg(&ds, DATA_SPEC);      // used to build list
                int dataIndex = ds.numRecords() - 1; // idx of last rec in 'ds'
                const RecDef& dr = ds.record(dataIndex);

                if (veryVerbose) { P(ds); P(DATA_SPEC); }

                List list;
                buildListDeep(&list, dr, true);
                if (veryVeryVerbose) P(list);
                printTest(LINE, list, EType::BDEM_LIST, dr);
                printTest(LINE, list.row(), EType::BDEM_ROW, dr);

                Table table;
                buildTableDeep(&table, dr, true);
                if (veryVeryVerbose) P(table);
                printTest(LINE, table, EType::BDEM_TABLE, dr);
            }
        }

        if (verbose) {
            cout << "\tTesting choice, choice array item, and choice array"
                 << endl;
        }

        {
            static const struct {
                int         d_lineNumber;
                const char *d_dataSpec;
            } DATA[] = {
                //line  data spec
                //----  -------------------------------
                { L_,   ":a?"                           },
                { L_,   ":a?Aa"                         },
                { L_,   ":*?Aa"                         },
                { L_,   ":a?A*"                         },
                { L_,   ":*?A*"                         },
                { L_,   ":a?Ua"                         },
                { L_,   ":a?Va"                         },
                { L_,   ":a?Aa"                         },
                { L_,   ":a?AaBb"                       },
                { L_,   ":a?AaBbCcDdEeFfGgHhIiJjUuVv"   },
                { L_,   ":a?A*BbC*DdEeFfG*HhIiJjUuVv"   },
                { L_,   ":a?WaXbYcZdaebfcgdheifj"       },
                { L_,   ":a?Aa :b?%a0"                  },
                { L_,   ":a?Aa :b?%*0"                  },
                { L_,   ":a?AaBbCc :b?%*0"              },
                { L_,   ":a?Aa :b?@a0"                  },
                { L_,   ":a?Aa :b?@*0"                  },
                { L_,   ":a?AaBbCc :b?@*0"              },
                { L_,   ":aAa :b?+a0"                   },
                { L_,   ":aAa :b?+*0"                   },
                { L_,   ":aAa :b?#a0"                   },
                { L_,   ":aAa :b?#*0"                   },
                { L_,   ":a?Aa :b?@a0Bb"                },
                { L_,   ":a?AaBbCc :b?@a0Bb"            },
                { L_,   ":a?Aa :b?%a0@b0"               },
                { L_,   ":a?AaBbCc :b?%a0@b0"           },
                { L_,   ":aAa :b?+a0Bb"                 },
                { L_,   ":aAa :b?+a0#b0"                },
                { L_,   ":a?AaBb :b?@a0"                },
                { L_,   ":a?Aa :b?@a0 :c?@a1"           },
                { L_,   ":a?Aa :b?@a0Bb :c?@a1"         },
                { L_,   ":a?AaBbCc :b?@a0Bb :c?@a1"     },
                { L_,   ":a?AaBb :b?@a0 :c?@a1"         },
                { L_,   ":a?Aa :b?@a0 :c?@a1 :d?@a2"    },
                { L_,   ":a?AaBb :b?@a0 :c?@a1 :d?@a2"  },
                { L_,   ":a?Aa :b?@a0Bb :c?@a1 :d?@a2"  },
                { L_,   ":a?Aa :b?@a0 :c?@a1Bb :d?@a2"  },
                { L_,   ":a?Aa :b?@a0 :c?@a1 :d?@a2Bb"  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE      = DATA[i].d_lineNumber;
                const char *DATA_SPEC = DATA[i].d_dataSpec;

                Schema ds;  gg(&ds, DATA_SPEC);      // used to build list
                int dataIndex = ds.numRecords() - 1; // idx of last rec in 'ds'
                const RecDef& dr = ds.record(dataIndex);

                if (veryVerbose) { P(ds); P(DATA_SPEC); }

                Choice choice;
                buildChoiceDeep(&choice, dr, true);
                if (veryVeryVerbose) P(choice);
                printTest(LINE, choice, EType::BDEM_CHOICE, dr);
                printTest(LINE,
                          choice.item(), EType::BDEM_CHOICE_ARRAY_ITEM, dr);

                Array choiceArray;
                buildChoiceArrayDeep(&choiceArray, dr, true);
                if (veryVeryVerbose) P(choiceArray);
                printTest(LINE, choiceArray, EType::BDEM_CHOICE_ARRAY, dr);
            }
        }

      } break;
      case 15: {
        // ====================================================================
        // TEST CHOICE ARRAY CAN-SATISFY FUNCTION
        //
        // Testing:
        //   int canSatisfyRecord(const bdem_ChoiceArray&, recDef);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "TESTING CHOICE ARRAY CAN-SATISFY FUNCTION" << endl
            << "=========================================" << endl;

        if (verbose) cout << "\t Testing choice arrays" << endl;

        static const struct {
            int         d_lineNumber;
            const char *d_dataSpec;
            const char *d_testSpec;
            int         d_exp;
        } DATA[] = {
//--------------^
//line  data spec                        test spec                        exp.
//----  -----------------------------    ------------------------------   ---
{ L_,   ":a?",                           ":b?",                            1 },

{ L_,   ":a?Aa",                         ":b?",                            1 },
{ L_,   ":a?Ua",                         ":b?",                            1 },
{ L_,   ":a?Va",                         ":b?",                            1 },
{ L_,   ":a?",                           ":b?Aa",                          0 },
{ L_,   ":a?",                           ":b?Ua",                          0 },
{ L_,   ":a?",                           ":b?Va",                          0 },

{ L_,   ":a?Aa",                         ":b?Ab",                          1 },
{ L_,   ":a?Aa",                         ":b?Bb",                          0 },
{ L_,   ":a?AaBb",                       ":b?Aa",                          1 },
{ L_,   ":a?AaBb",                       ":b?BbAa",                        0 },

{ L_,   ":a?AaBbCcDdEeFfGgHhIiJjUuVv",   ":a?AaBbCcDdEeFfGgHhIiJjUuVv",    1 },
{ L_,   ":a?AaBbCcDdEeFfGgHhIiJjUuVv",   ":a?AaBbCcDdEeFfGgHhIiJjVvUu",    0 },
{ L_,   ":a?WaXbYcZdaebfcgdheifj",       ":a?WaXbYcZdaebfcgdheifj",        1 },
{ L_,   ":a?WiXjYkZlaabbccddeeff",       ":a?WiXjYkZlaabbccddffee",        0 },

{ L_,   ":a?Aa :b?%a0",                  ":a?Aa :b?%a0",                   1 },
{ L_,   ":a?Aa :b?%a0",                  ":a?Aa :b?@a0",                   0 },
{ L_,   ":a?Aa :b?@a0",                  ":a?Aa :b?%a0",                   0 },
{ L_,   ":a?Aa :b?@a0",                  ":a?Aa :b?@a0",                   1 },

{ L_,   ":aAa :b?+a0",                   ":aAa :b?+a0",                    1 },
{ L_,   ":aAa :b?+a0",                   ":aAa :b?#a0",                    0 },
{ L_,   ":aAa :b?#a0",                   ":aAa :b?+a0",                    0 },
{ L_,   ":aAa :b?#a0",                   ":aAa :b?#a0",                    1 },

{ L_,   ":a?Aa :b?@a0Bb",                ":a?Aa :b?@a0",                   1 },
{ L_,   ":a?Aa :b?Aa@b0",                ":a?Aa :b?@a0",                   0 },
{ L_,   ":a?Aa :b?@a0",                  ":a?Aa :b?@a0Bb",                 0 },
{ L_,   ":a?Aa :b?%a0@b0",               ":a?Aa :b?%a0@b0",                1 },

{ L_,   ":aAa :b?+a0Bb",                 ":aAa :b?+a0",                    1 },
{ L_,   ":aAa :b?Aa+b0",                 ":aAa :b?+a0",                    0 },
{ L_,   ":aAa :b?+a0",                   ":aAa :b?+a0Bb",                  0 },
{ L_,   ":aAa :b?+a0#b0",                ":aAa :b?+a0#b0",                 1 },

{ L_,   ":a?Aa :b?@a0",                  ":a?Bb :b?@a0",                   0 },
{ L_,   ":a?AaBb :b?@a0",                ":a?Aa :b?@a0",                   1 },
{ L_,   ":a?Aa :b?@a0",                  ":a?AaBb :b?@a0",                 0 },
{ L_,   ":a?BbAa :b?@a0",                ":a?Aa :b?@a0",                   0 },

{ L_,   ":a?Aa :b?@a0 :c?@a1",           ":a?Aa :b?@a0 :c?@a1",            1 },
{ L_,   ":a?Bb :b?@a0 :c?@a1",           ":a?Aa :b?@a0 :c?@a1",            0 },
{ L_,   ":a?Aa :b?@a0Bb :c?@a1",         ":a?Aa :b?@a0 :c?@a1",            1 },
{ L_,   ":a?Aa :b?@a0 :c?@a1",           ":a?Aa :b?@a0Bb :c?@a1",          0 },
{ L_,   ":a?AaBb :b?@a0 :c?@a1",         ":a?Aa :b?@a0 :c?@a1",            1 },
{ L_,   ":a?Aa :b?@a0 :c?@a1",           ":a?AaBb :b?@a0 :c?@a1",          0 },

{ L_,   ":a?Aa :b?@a0 :c?@a1 :d?@a2",    ":a?Aa :b?@a0 :c?@a1 :d?@a2",     1 },
{ L_,   ":a?Bb :b?@a0 :c?@a1 :d?@a2",    ":a?Aa :b?@a0 :c?@a1 :d?@a2",     0 },
{ L_,   ":a?AaBb :b?@a0 :c?@a1 :d?@a2",  ":a?Aa :b?@a0 :c?@a1 :d?@a2",     1 },
{ L_,   ":a?Aa :b?@a0Bb :c?@a1 :d?@a2",  ":a?Aa :b?@a0 :c?@a1 :d?@a2",     1 },
{ L_,   ":a?Aa :b?@a0 :c?@a1Bb :d?@a2",  ":a?Aa :b?@a0 :c?@a1 :d?@a2",     1 },
{ L_,   ":a?Aa :b?@a0 :c?@a1 :d?@a2Bb",  ":a?Aa :b?@a0 :c?@a1 :d?@a2",     1 },
{ L_,   ":a?Aa :b?@a0 :c?@a1 :d?@a2",    ":a?AaBb :b?@a0 :c?@a1 :d?@a2",   0 },
{ L_,   ":a?Aa :b?@a0 :c?@a1 :d?@a2",    ":a?Aa :b?@a0Bb :c?@a1 :d?@a2",   0 },
{ L_,   ":a?Aa :b?@a0 :c?@a1 :d?@a2",    ":a?Aa :b?@a0 :c?@a1Bb :d?@a2",   0 },
{ L_,   ":a?Aa :b?@a0 :c?@a1 :d?@a2",    ":a?Aa :b?@a0 :c?@a1 :d?@a2Bb",   0 },

//--------------v
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE      = DATA[i].d_lineNumber;
            const char *DATA_SPEC = DATA[i].d_dataSpec;
            const char *TEST_SPEC = DATA[i].d_testSpec;
            int         EXP       = DATA[i].d_exp;

            if (veryVerbose) { P(DATA_SPEC);  P(TEST_SPEC);  P(EXP); }

            Schema ds;  gg(&ds, DATA_SPEC);   // used to build choice array
            Schema ts;  gg(&ts, TEST_SPEC);   // used in method under test

            int dataIndex = ds.numRecords() - 1;  // idx of last rec in 'ds'
            int testIndex = ts.numRecords() - 1;  // idx of last rec in 'ts'

            const RecDef& dr = ds.record(dataIndex);
            const RecDef& tr = ts.record(testIndex);

            Array array;
            buildChoiceArrayDeep(&array, dr);
            if (veryVeryVerbose) P(array);

            LOOP_ASSERT(LINE, EXP == Util::canSatisfyRecord(array, tr));
        }

        if (verbose) cout << "\t Testing invalid record types" << endl;
        {
            const Schema X(g(":*?A*"));
            Array mA;  const Array& A = mA;
            buildChoiceArrayDeep(&mA, X.record(0));

            static const struct {
                int         d_lineNumber;
                const char *d_spec;
                int         d_exp;
            } DATA[] = {
                //line   data spec   exp.
                //----   ---------   ----
                { L_,    ":*?A*",    1    },
                { L_,    ":*A*",     0    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_lineNumber;
                const char *SPEC = DATA[i].d_spec;
                int         EXP  = DATA[i].d_exp;

                if (veryVerbose) { P_(LINE);  P_(SPEC);  P(EXP); }

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);

                LOOP_ASSERT(LINE, EXP == Util::canSatisfyRecord(A, RD));
            }
        }

      } break;
      case 14: {
        // ====================================================================
        // TEST CHOICE AND CHOICE ARRAY ITEM CAN-SATISFY FUNCTIONS
        //
        // Testing:
        //   int canSatisfyRecord(const bdem_Choice&, const bdem_RecordDef&);
        //   int canSatisfyRecord(const bdem_ChoiceArrayItem&, recDef);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "TESTING CHOICE, CHOICE ARRAY ITEM CAN-SATISFY FUNCTIONS"
            << endl
            << "======================================================="
            << endl;

        if (verbose) cout << "\t Testing choices and choice array items"
                          << endl;

        static const struct {
            int         d_lineNumber;
            const char *d_dataSpec;
            const char *d_testSpec;
            int         d_exp;
        } DATA[] = {
//--------------^
//line  data spec                        test spec                        exp.
//----  -----------------------------    ------------------------------   ---
{ L_,   ":a?",                           ":b?",                            1 },

{ L_,   ":a?Aa",                         ":b?",                            1 },
{ L_,   ":a?Ua",                         ":b?",                            1 },
{ L_,   ":a?Va",                         ":b?",                            1 },
{ L_,   ":a?",                           ":b?Aa",                          0 },
{ L_,   ":a?",                           ":b?Ua",                          0 },
{ L_,   ":a?",                           ":b?Va",                          0 },

{ L_,   ":a?Aa",                         ":b?Ab",                          1 },
{ L_,   ":a?Aa",                         ":b?Bb",                          0 },
{ L_,   ":a?AaBb",                       ":b?Aa",                          1 },
{ L_,   ":a?AaBb",                       ":b?BbAa",                        0 },

{ L_,   ":a?AaBbCcDdEeFfGgHhIiJjUuVv",   ":a?AaBbCcDdEeFfGgHhIiJjUuVv",    1 },
{ L_,   ":a?AaBbCcDdEeFfGgHhIiJjUuVv",   ":a?AaBbCcDdEeFfGgHhIiJjVvUu",    0 },
{ L_,   ":a?WaXbYcZdaebfcgdheifj",       ":a?WaXbYcZdaebfcgdheifj",        1 },
{ L_,   ":a?WiXjYkZlaabbccddeeff",       ":a?WiXjYkZlaabbccddffee",        0 },

{ L_,   ":a?Aa :b?%a0",                  ":a?Aa :b?%a0",                   1 },
{ L_,   ":a?Aa :b?%a0",                  ":a?Aa :b?@a0",                   0 },
{ L_,   ":a?Aa :b?@a0",                  ":a?Aa :b?%a0",                   0 },
{ L_,   ":a?Aa :b?@a0",                  ":a?Aa :b?@a0",                   1 },

{ L_,   ":aAa :b?+a0",                   ":aAa :b?+a0",                    1 },
{ L_,   ":aAa :b?+a0",                   ":aAa :b?#a0",                    0 },
{ L_,   ":aAa :b?#a0",                   ":aAa :b?+a0",                    0 },
{ L_,   ":aAa :b?#a0",                   ":aAa :b?#a0",                    1 },

{ L_,   ":a?Aa :b?%a0Bb",                ":a?Aa :b?%a0",                   1 },
{ L_,   ":a?Aa :b?Aa%b0",                ":a?Aa :b?%a0",                   0 },
{ L_,   ":a?Aa :b?%a0",                  ":a?Aa :b?%a0Bb",                 0 },
{ L_,   ":a?Aa :b?%a0@b0",               ":a?Aa :b?%a0@b0",                1 },

{ L_,   ":aAa :b?+a0Bb",                 ":aAa :b?+a0",                    1 },
{ L_,   ":aAa :b?Aa+b0",                 ":aAa :b?+a0",                    0 },
{ L_,   ":aAa :b?+a0",                   ":aAa :b?+a0Bb",                  0 },
{ L_,   ":aAa :b?+a0#b0",                ":aAa :b?+a0#b0",                 1 },

{ L_,   ":a?Aa :b?%a0",                  ":a?Bb :b?%a0",                   0 },
{ L_,   ":a?AaBb :b?%a0",                ":a?Aa :b?%a0",                   1 },
{ L_,   ":a?Aa :b?%a0",                  ":a?AaBb :b?%a0",                 0 },
{ L_,   ":a?BbAa :b?%a0",                ":a?Aa :b?%a0",                   0 },

{ L_,   ":a?Aa :b?%a0 :c?%a1",           ":a?Aa :b?%a0 :c?%a1",            1 },
{ L_,   ":a?Bb :b?%a0 :c?%a1",           ":a?Aa :b?%a0 :c?%a1",            0 },
{ L_,   ":a?Aa :b?%a0Bb :c?%a1",         ":a?Aa :b?%a0 :c?%a1",            1 },
{ L_,   ":a?Aa :b?%a0 :c?%a1",           ":a?Aa :b?%a0Bb :c?%a1",          0 },
{ L_,   ":a?AaBb :b?%a0 :c?%a1",         ":a?Aa :b?%a0 :c?%a1",            1 },
{ L_,   ":a?Aa :b?%a0 :c?%a1",           ":a?AaBb :b?%a0 :c?%a1",          0 },

{ L_,   ":a?Aa :b?%a0 :c?%a1 :d?%a2",    ":a?Aa :b?%a0 :c?%a1 :d?%a2",     1 },
{ L_,   ":a?Bb :b?%a0 :c?%a1 :d?%a2",    ":a?Aa :b?%a0 :c?%a1 :d?%a2",     0 },
{ L_,   ":a?AaBb :b?%a0 :c?%a1 :d?%a2",  ":a?Aa :b?%a0 :c?%a1 :d?%a2",     1 },
{ L_,   ":a?Aa :b?%a0Bb :c?%a1 :d?%a2",  ":a?Aa :b?%a0 :c?%a1 :d?%a2",     1 },
{ L_,   ":a?Aa :b?%a0 :c?%a1Bb :d?%a2",  ":a?Aa :b?%a0 :c?%a1 :d?%a2",     1 },
{ L_,   ":a?Aa :b?%a0 :c?%a1 :d?%a2Bb",  ":a?Aa :b?%a0 :c?%a1 :d?%a2",     1 },
{ L_,   ":a?Aa :b?%a0 :c?%a1 :d?%a2",    ":a?AaBb :b?%a0 :c?%a1 :d?%a2",   0 },
{ L_,   ":a?Aa :b?%a0 :c?%a1 :d?%a2",    ":a?Aa :b?%a0Bb :c?%a1 :d?%a2",   0 },
{ L_,   ":a?Aa :b?%a0 :c?%a1 :d?%a2",    ":a?Aa :b?%a0 :c?%a1Bb :d?%a2",   0 },
{ L_,   ":a?Aa :b?%a0 :c?%a1 :d?%a2",    ":a?Aa :b?%a0 :c?%a1 :d?%a2Bb",   0 },

//--------------v
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE      = DATA[i].d_lineNumber;
            const char *DATA_SPEC = DATA[i].d_dataSpec;
            const char *TEST_SPEC = DATA[i].d_testSpec;
            int         EXP       = DATA[i].d_exp;

            if (veryVerbose) { P(DATA_SPEC);  P(TEST_SPEC);  P(EXP); }

            Schema ds;  gg(&ds, DATA_SPEC);   // used to build choice
            Schema ts;  gg(&ts, TEST_SPEC);   // used in method under test

            int dataIndex = ds.numRecords() - 1;  // idx of last rec in 'ds'
            int testIndex = ts.numRecords() - 1;  // idx of last rec in 'ts'

            const RecDef& dr = ds.record(dataIndex);
            const RecDef& tr = ts.record(testIndex);

            Choice choice;
            buildChoiceDeep(&choice, dr);
            if (veryVeryVerbose) P(choice);

//          LOOP_ASSERT(LINE, EXP == Util::canSatisfyRecord(choice.item(),
//                                                          tr));
            LOOP_ASSERT(LINE, EXP == Util::canSatisfyRecord(choice, tr));
        }

        if (verbose) cout << "\t Testing invalid record types" << endl;
        {
            const Schema X(g(":*?A*"));
            Choice mC;  const Choice& C = mC;
            buildChoiceDeep(&mC, X.record(0));

            static const struct {
                int         d_lineNumber;
                const char *d_spec;
                int         d_exp;
            } DATA[] = {
                //line   data spec   exp.
                //----   ---------   ----
                { L_,    ":*?A*",    1    },
                { L_,    ":*A*",     0    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_lineNumber;
                const char *SPEC = DATA[i].d_spec;
                int         EXP  = DATA[i].d_exp;

                if (veryVerbose) { P_(LINE);  P_(SPEC);  P(EXP); }

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);

                LOOP_ASSERT(LINE, EXP == Util::canSatisfyRecord(C, RD));
//              LOOP_ASSERT(LINE, EXP == Util::canSatisfyRecord(C.item(), RD));
            }
        }

      } break;
      case 13: {
        // ====================================================================
        // TEST TABLE CAN-SATISFY FUNCTION
        //
        // Concerns:
        //   Building upon the concerns of the previous test case that tests
        //   methods taking a list or a row as its data argument, the method
        //   under test operating on a table argument adds two related
        //   concerns.  The first concern is that, since a table can have zero
        //   or more rows, for any given table structure, we must perform our
        //   test for tables having differing numbers of rows; each row can
        //   affect the method's return value.  The second "concern" is really
        //   a test-method concern, since we do not have convenient and
        //   *tested* machinery to generate tables of like structure and with
        //   varying numbers of rows, arbitrarily nested in other aggregate
        //   structures.  Nevertheless, this consideration will influence our
        //   testing strategy.
        //
        // Plan:
        //   We will first perform a set of tests very much similar to the
        //   previous test case for the row and list methods.  The difference
        //   in technique is that 'initTable' does not have a "deep" variant,
        //   and so we use the "data record" to configure the top-level table
        //   with 'initTable', then configure a list from the same record, and
        //   append the list's row to the table.  Note that this technique is
        //   a good start but not sufficient, since 'initListDeep' still
        //   cannot initialize table elements deeply, so a second set of tests
        //   will be needed.
        //
        //   The second step is done as an ad hoc sketch; the method can be
        //   generalized to a table-based test, but that is left for
        //   Development to finish.  The idea is to build a table containing a
        //   list element and a table element, and a record defining the
        //   structure of the sub-list and subtable.   First put a "conformant"
        //   row in the table, then put an "altered" row, and show that
        //   'canSatisfyRecord' fails.
        //
        // Testing:
        //   int canSatisfyRecord(const bdem_Table&, bdem_RecordDef&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "TESTING TABLE CAN-SATISFY FUNCTION" << endl
            << "==================================" << endl;

        if (verbose) cout << "\t Testing tables without nested tables" << endl;

        static const struct {
            int         d_lineNumber;
            const char *d_dataSpec;
            const char *d_testSpec;
            int         d_exp;
        } DATA[] = {
//--------------^
//line  data spec                      test spec                       exp.
//----  -----------------------------  ------------------------------  ---
{ L_,   ":a",                          ":b",                            1 },

{ L_,   ":aAa",                        ":b",                            1 },
{ L_,   ":aUa",                        ":b",                            1 },
{ L_,   ":aVa",                        ":b",                            1 },
{ L_,   ":a",                          ":bAa",                          0 },
{ L_,   ":a",                          ":bUa",                          0 },
{ L_,   ":a",                          ":bVa",                          0 },

{ L_,   ":aAa",                        ":bAb",                          1 },
{ L_,   ":aAa",                        ":bBb",                          0 },
{ L_,   ":aAaBb",                      ":bAa",                          1 },
{ L_,   ":aAaBb",                      ":bBbAa",                        0 },

{ L_,   ":aAaBbCcDdEeFfGgHhIiJjUuVv",  ":aAaBbCcDdEeFfGgHhIiJjUuVv",    1 },
{ L_,   ":aAaBbCcDdEeFfGgHhIiJjUuVv",  ":aAaBbCcDdEeFfGgHhIiJjVvUu",    0 },
{ L_,   ":aWaXbYcZdaebfcgdheifj",      ":aWaXbYcZdaebfcgdheifj",        1 },
{ L_,   ":aWiXjYkZlaabbccddeeff",      ":aWiXjYkZlaabbccddffee",        0 },

{ L_,   ":aAa :b+a0",                  ":aAa :b+a0",                    1 },
{ L_,   ":aAa :b+a0",                  ":aAa :b#a0",                    0 },
{ L_,   ":aAa :b#a0",                  ":aAa :b+a0",                    0 },
{ L_,   ":aAa :b#a0",                  ":aAa :b#a0",                    1 },

{ L_,   ":a?Aa :b%a0",                 ":a?Aa :b%a0",                   1 },
{ L_,   ":a?Aa :b%a0",                 ":a?Aa :b@a0",                   0 },
{ L_,   ":a?Aa :b@a0",                 ":a?Aa :b%a0",                   0 },
{ L_,   ":a?Aa :b@a0",                 ":a?Aa :b@a0",                   1 },

{ L_,   ":aAa :b+a0Bb",                ":aAa :b+a0",                    1 },
{ L_,   ":aAa :bAa+b0",                ":aAa :b+a0",                    0 },
{ L_,   ":aAa :b+a0",                  ":aAa :b+a0Bb",                  0 },
{ L_,   ":aAa :b+a0#b0",               ":aAa :b+a0#b0",                 1 },

{ L_,   ":a?Aa :b%a0Bb",               ":a?Aa :b%a0",                   1 },
{ L_,   ":a?Aa :bAa%b0",               ":a?Aa :b%a0",                   0 },
{ L_,   ":a?Aa :b%a0",                 ":a?Aa :b%a0Bb",                 0 },
{ L_,   ":a?Aa :b%a0@b0",              ":a?Aa :b%a0@b0",                1 },

{ L_,   ":aAa :b#a0",                  ":aBb :b#a0",                    0 },
{ L_,   ":aAaBb :b#a0",                ":aAa :b#a0",                    1 },
{ L_,   ":aAa :b#a0",                  ":aAaBb :b#a0",                  0 },
{ L_,   ":aBbAa :b#a0",                ":aAa :b#a0",                    0 },

{ L_,   ":aAa :b#a0 :c#a1",            ":aAa :b#a0 :c#a1",              1 },
{ L_,   ":aBb :b#a0 :c#a1",            ":aAa :b#a0 :c#a1",              0 },
{ L_,   ":aAa :b#a0Bb :c#a1",          ":aAa :b#a0 :c#a1",              1 },
{ L_,   ":aAa :b#a0 :c#a1",            ":aAa :b#a0Bb :c#a1",            0 },
{ L_,   ":aAaBb :b#a0 :c#a1",          ":aAa :b#a0 :c#a1",              1 },
{ L_,   ":aAa :b#a0 :c#a1",            ":aAaBb :b#a0 :c#a1",            0 },

{ L_,   ":aAa :b+a0 :c+a1 :d+a2",      ":aAa :b+a0 :c+a1 :d+a2",        1 },
{ L_,   ":aBb :b+a0 :c+a1 :d+a2",      ":aAa :b+a0 :c+a1 :d+a2",        0 },
{ L_,   ":aAaBb :b+a0 :c+a1 :d+a2",    ":aAa :b+a0 :c+a1 :d+a2",        1 },
{ L_,   ":aAa :b+a0Bb :c+a1 :d+a2",    ":aAa :b+a0 :c+a1 :d+a2",        1 },
{ L_,   ":aAa :b+a0 :c+a1Bb :d+a2",    ":aAa :b+a0 :c+a1 :d+a2",        1 },
{ L_,   ":aAa :b+a0 :c+a1 :d+a2Bb",    ":aAa :b+a0 :c+a1 :d+a2",        1 },
{ L_,   ":aAa :b+a0 :c+a1 :d+a2",      ":aAaBb :b+a0 :c+a1 :d+a2",      0 },
{ L_,   ":aAa :b+a0 :c+a1 :d+a2",      ":aAa :b+a0Bb :c+a1 :d+a2",      0 },
{ L_,   ":aAa :b+a0 :c+a1 :d+a2",      ":aAa :b+a0 :c+a1Bb :d+a2",      0 },
{ L_,   ":aAa :b+a0 :c+a1 :d+a2",      ":aAa :b+a0 :c+a1 :d+a2Bb",      0 },

//--------------v
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE      = DATA[i].d_lineNumber;
            const char *DATA_SPEC = DATA[i].d_dataSpec;
            const char *TEST_SPEC = DATA[i].d_testSpec;
            int         EXP       = DATA[i].d_exp;

            if (veryVerbose) { P(DATA_SPEC);  P(TEST_SPEC);  P(EXP); }

            Schema ds;  gg(&ds, DATA_SPEC);   // used to build table
            Schema ts;  gg(&ts, TEST_SPEC);   // used in method under test

            int dataIndex = ds.numRecords() - 1;  // idx of last rec in 'ds'
            int testIndex = ts.numRecords() - 1;  // idx of last rec in 'ts'

            const RecDef& dr = ds.record(dataIndex);
            const RecDef& tr = ts.record(testIndex);

            Table table;
            List  list;
            Util::initTable(&table, dr);
            buildListDeep(&list, dr);

            table.appendRow(list);  if (veryVerbose) { P(table); }

            LOOP_ASSERT(LINE, EXP == Util::canSatisfyRecord(table, tr));

            table.appendRow(list);  if (veryVerbose) { P(table); }

            LOOP_ASSERT(LINE, EXP == Util::canSatisfyRecord(table, tr));
        }

        if (verbose) cout << "\t Testing tables by hand." << endl;
        {
            const char *dataSpec    = ":aAaBbCcUuVv"; // char short int lst tbl
            const char *alteredSpec = ":aAaAbCcUuVv"; // char char  int lst tbl
            const char *testSpec    = ":aAaBbCcUuVv :bAaBbCc+u0#v0";

            Schema dataSchema;     gg(&dataSchema, dataSpec);
            Schema testSchema;     gg(&testSchema, testSpec);
            Schema alteredSchema;  gg(&alteredSchema, alteredSpec);

            const RecDef& dr = dataSchema.record(0);
            const RecDef& tr = testSchema.record(1);
            const RecDef& ar = alteredSchema.record(0);

            List  referenceList;
            Table table;

            Util::initTable(&table, dr);
            Table subTable(table);

            Util::initListDeep(&referenceList, dr);
            List subList(referenceList);

            referenceList.theModifiableList(3)  = subList;
            referenceList.theModifiableTable(4) = subTable;

            table.appendRow(referenceList);
            ASSERT(1 == Util::canSatisfyRecord(table, tr));

            // Now make an altered list having an altered sub-list
            List alteredList(referenceList);
            List alteredSubList;
            Util::initListDeep(&alteredSubList, ar);

            // Place an altered sub-list in the altered-list's list
            alteredList.theModifiableList(3)  = alteredSubList;

            // Place an altered sub-table in the altered-list's table
            alteredList.theModifiableTable(4) = subTable;

            // Append the row containing an altered list element to the table.
            table.appendRow(alteredList);
            ASSERT(0 == Util::canSatisfyRecord(table, tr));
        }

        if (verbose) cout << "\t Testing invalid record types" << endl;
        {
            const Schema X(g(":*A*"));
            Table mT;  const Table& T = mT;
            buildTableDeep(&mT, X.record(0));

            static const struct {
                int         d_lineNumber;
                const char *d_spec;
                int         d_exp;
            } DATA[] = {
                //line   data spec   exp.
                //----   ---------   ----
                { L_,    ":*A*",     1    },
                { L_,    ":*?A*",    0    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_lineNumber;
                const char *SPEC = DATA[i].d_spec;
                int         EXP  = DATA[i].d_exp;

                if (veryVerbose) { P_(LINE);  P_(SPEC);  P(EXP); }

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);

                LOOP_ASSERT(LINE, EXP == Util::canSatisfyRecord(T, RD));
            }
        }

      } break;
      case 12: {
        // ====================================================================
        // TEST ROW AND LIST CAN-SATISFY FUNCTIONS
        //
        // Concerns:
        //   The methods under test must verify that every field in the
        //   specified record, and every sub-field within every constrained
        //   field, has a corresponding field or sub-field, to all depths of
        //   recursion, in the specified row or list.  A complication arises
        //   when a sub-field is a constrained table, since these methods
        //   recursively call the as-yet-untested 'canSatisfyRecord' method
        //   that takes a table.  We will therefore make limited use of
        //   constrained tables in this test case, postponing full testing of
        //   nested tables until the next test case above.
        //
        //   We de-emphasize the "black box concern" of testing many "subtle"
        //   differences of type in otherwise-similar structures, since the
        //   code under test for that concern is a single line.  Rather, we are
        //   concerned that excess fields in the row or list do not cause a
        //   zero return value, while excess fields in the primary record (and
        //   especially in constraint records) do cause a zero return value.
        //   Only a few representative type-difference cases need be
        //   considered, since we are more concerned with structural issues.
        //
        // Plan:
        //   We tabulate specifications for two schemas using the 'bdem_Schema'
        //   'gg' language; the last record in each schema is the "active
        //   record" of interest for the test.  The first schema produces the
        //   "data record", which is used to configure a list (using the tested
        //   'initListDeep' method) that provides the first argument for the
        //   method under test; the second schema produces the "test record",
        //   which provides the second argument.  We also tabulate the expected
        //   result, based on the two schema specs.
        //
        // Testing:
        //   int canSatisfyRecord(const bdem_Row&, bdem_RecordDef&);
        //   int canSatisfyRecord(const bdem_List&, bdem_RecordDef&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "TESTING ROW AND LIST CAN-SATISFY FUNCTIONS" << endl
            << "==========================================" << endl;

        if (verbose) cout << "\t Testing rows and lists" << endl;

        static const struct {
            int         d_lineNumber;
            const char *d_dataSpec;
            const char *d_testSpec;
            int         d_exp;
        } DATA[] = {
//--------------^
//line  data spec                      test spec                       exp.
//----  -----------------------------  ------------------------------  ---
{ L_,   ":a",                          ":b",                            1 },

{ L_,   ":aAa",                        ":b",                            1 },
{ L_,   ":aUa",                        ":b",                            1 },
{ L_,   ":aVa",                        ":b",                            1 },
{ L_,   ":a",                          ":bAa",                          0 },
{ L_,   ":a",                          ":bUa",                          0 },
{ L_,   ":a",                          ":bVa",                          0 },

{ L_,   ":aAa",                        ":bAb",                          1 },
{ L_,   ":aAa",                        ":bBb",                          0 },
{ L_,   ":aAaBb",                      ":bAa",                          1 },
{ L_,   ":aAaBb",                      ":bBbAa",                        0 },

{ L_,   ":aAaBbCcDdEeFfGgHhIiJjUuVv",  ":aAaBbCcDdEeFfGgHhIiJjUuVv",    1 },
{ L_,   ":aAaBbCcDdEeFfGgHhIiJjUuVv",  ":aAaBbCcDdEeFfGgHhIiJjVvUu",    0 },
{ L_,   ":aWaXbYcZdaebfcgdheifj",      ":aWaXbYcZdaebfcgdheifj",        1 },
{ L_,   ":aWiXjYkZlaabbccddeeff",      ":aWiXjYkZlaabbccddffee",        0 },

{ L_,   ":aAa :b+a0",                  ":aAa :b+a0",                    1 },
{ L_,   ":aAa :b+a0",                  ":aAa :b#a0",                    0 },
{ L_,   ":aAa :b#a0",                  ":aAa :b+a0",                    0 },
{ L_,   ":aAa :b#a0",                  ":aAa :b#a0",                    1 },

{ L_,   ":a?Aa :b%a0",                 ":a?Aa :b%a0",                   1 },
{ L_,   ":a?Aa :b%a0",                 ":a?Aa :b@a0",                   0 },
{ L_,   ":a?Aa :b@a0",                 ":a?Aa :b%a0",                   0 },
{ L_,   ":a?Aa :b@a0",                 ":a?Aa :b@a0",                   1 },

{ L_,   ":aAa :b+a0Bb",                ":aAa :b+a0",                    1 },
{ L_,   ":aAa :bAa+b0",                ":aAa :b+a0",                    0 },
{ L_,   ":aAa :b+a0",                  ":aAa :b+a0Bb",                  0 },
{ L_,   ":aAa :b+a0#b0",               ":aAa :b+a0#b0",                 1 },

{ L_,   ":a?Aa :b%a0Bb",               ":a?Aa :b%a0",                   1 },
{ L_,   ":a?Aa :bAa%b0",               ":a?Aa :b%a0",                   0 },
{ L_,   ":a?Aa :b%a0",                 ":a?Aa :b%a0Bb",                 0 },
{ L_,   ":a?Aa :b%a0@b0",              ":a?Aa :b%a0@b0",                1 },

{ L_,   ":aAa :b+a0",                  ":aBb :b+a0",                    0 },
{ L_,   ":aAaBb :b+a0",                ":aAa :b+a0",                    1 },
{ L_,   ":aAa :b+a0",                  ":aAaBb :b+a0",                  0 },
{ L_,   ":aBbAa :b+a0",                ":aAa :b+a0",                    0 },

{ L_,   ":aAa :b+a0 :c+a1",            ":aAa :b+a0 :c+a1",              1 },
{ L_,   ":aBb :b+a0 :c+a1",            ":aAa :b+a0 :c+a1",              0 },
{ L_,   ":aAa :b+a0Bb :c+a1",          ":aAa :b+a0 :c+a1",              1 },
{ L_,   ":aAa :b+a0 :c+a1",            ":aAa :b+a0Bb :c+a1",            0 },
{ L_,   ":aAaBb :b+a0 :c+a1",          ":aAa :b+a0 :c+a1",              1 },
{ L_,   ":aAa :b+a0 :c+a1",            ":aAaBb :b+a0 :c+a1",            0 },

{ L_,   ":aAa :b+a0 :c+a1 :d+a2",      ":aAa :b+a0 :c+a1 :d+a2",        1 },
{ L_,   ":aBb :b+a0 :c+a1 :d+a2",      ":aAa :b+a0 :c+a1 :d+a2",        0 },
{ L_,   ":aAaBb :b+a0 :c+a1 :d+a2",    ":aAa :b+a0 :c+a1 :d+a2",        1 },
{ L_,   ":aAa :b+a0Bb :c+a1 :d+a2",    ":aAa :b+a0 :c+a1 :d+a2",        1 },
{ L_,   ":aAa :b+a0 :c+a1Bb :d+a2",    ":aAa :b+a0 :c+a1 :d+a2",        1 },
{ L_,   ":aAa :b+a0 :c+a1 :d+a2Bb",    ":aAa :b+a0 :c+a1 :d+a2",        1 },
{ L_,   ":aAa :b+a0 :c+a1 :d+a2",      ":aAaBb :b+a0 :c+a1 :d+a2",      0 },
{ L_,   ":aAa :b+a0 :c+a1 :d+a2",      ":aAa :b+a0Bb :c+a1 :d+a2",      0 },
{ L_,   ":aAa :b+a0 :c+a1 :d+a2",      ":aAa :b+a0 :c+a1Bb :d+a2",      0 },
{ L_,   ":aAa :b+a0 :c+a1 :d+a2",      ":aAa :b+a0 :c+a1 :d+a2Bb",      0 },

//--------------v
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE      = DATA[i].d_lineNumber;
            const char *DATA_SPEC = DATA[i].d_dataSpec;
            const char *TEST_SPEC = DATA[i].d_testSpec;
            int         EXP       = DATA[i].d_exp;

            if (veryVerbose) { P(DATA_SPEC);  P(TEST_SPEC);  P(EXP); }

            Schema ds;  gg(&ds, DATA_SPEC);   // used to build list
            Schema ts;  gg(&ts, TEST_SPEC);   // used in method under test

            int dataIndex = ds.numRecords() - 1;  // idx of last rec in 'ds'
            int testIndex = ts.numRecords() - 1;  // idx of last rec in 'ts'

            const RecDef& dr = ds.record(dataIndex);
            const RecDef& tr = ts.record(testIndex);

            List list;
            buildListDeep(&list, dr);
            if (veryVeryVerbose) P(list);

            LOOP_ASSERT(LINE, EXP == Util::canSatisfyRecord(list.row(), tr));
            LOOP_ASSERT(LINE, EXP == Util::canSatisfyRecord(list, tr));
        }

        if (verbose) cout << "\t Testing invalid record types" << endl;
        {
            const Schema X(g(":*A*"));
            List mL;  const List& L = mL;
            buildListDeep(&mL, X.record(0));

            static const struct {
                int         d_lineNumber;
                const char *d_spec;
                int         d_exp;
            } DATA[] = {
                //line   data spec   exp.
                //----   ---------   ----
                { L_,    ":*A*",     1    },
                { L_,    ":*?A*",    0    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_lineNumber;
                const char *SPEC = DATA[i].d_spec;
                int         EXP  = DATA[i].d_exp;

                if (veryVerbose) { P_(LINE);  P_(SPEC);  P(EXP); }

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);

                LOOP_ASSERT(LINE, EXP == Util::canSatisfyRecord(L.row(), RD));
                LOOP_ASSERT(LINE, EXP == Util::canSatisfyRecord(L, RD));
            }
        }

      } break;
      case 11: {
        // ====================================================================
        // TEST CHOICE ARRAY ITEM CONFORMANT FUNCTIONS
        //
        // Testing:
        //   bool canSatisfyRecord(const bdem_ChoiceArrayItem&, recDef);
        //   bool isChoiceConformant(const bdem_ChoiceArrayItem&, recDef);
        //   bool isChoiceShallowConformant(const bdem_ChoiceArrayItem&,
        //                                                             recDef);
        //   bool isChoiceDeepConformant(const bdem_ChoiceArrayItem&, recDef);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CHOICE CONFORMANT FUNCTIONS" << endl
                          << "===================================" << endl;

        if (verbose) cout << "\t Testing deep-conformant choices" << endl;
        {
            const char *SPEC[] = {
            //-^
            ":*?",
            ":*?A*", ":*?B*", ":*?C*", ":*?D*", ":*?E*", ":*?F*", ":*?G*",
            ":*?H*", ":*?I*", ":*?J*", ":*?K*", ":*?L*", ":*?M*", ":*?N*",
            ":*?O*", ":*?P*", ":*?Q*", ":*?R*", ":*?S*", ":*?T*", ":*?U*",
            ":*?V*", ":*?W*", ":*?X*", ":*?Y*", ":*?Z*", ":*?a*", ":*?b*",
            ":*?c*", ":*?d*", ":*?e*", ":*?f*", ":*?A*B*",
            ":*?A*B* :*?%*0", ":*?A*B* :*?@*0", ":*?A*B* :*?%*0 :*?@*1",
            ":*?C*D* :*?%*0@*0 :*E*F* :*?%*1@*1+*2#*2",
    ":*?A*B*C*D*E*F*G*H*I*J*K*L*M*N*O*P*Q*R*S*T*U*V*W*X*Y*Z*a*b*c*d*e*f*"
    ":*?A*B*C*D*E*F*G*H*I*J*K*L*M*N*O*P*Q*R*S*T*U*V*W*X*Y*Z*a*b*c*d*e*f*%*0@*0"
            //-v
            };

            for (int i = 0; i < SIZEOF_ARRAY(SPEC); ++i) {
                const Schema  S(g(SPEC[i]));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mC;  const Choice& C = mC;

//              bool isConformant = bsl::strpbrk(SPEC[i], "UVef")
//                                ? false : true;
                bool isConformant = true;
                if (veryVerbose) { P_(i);  P(SPEC[i]) }
                buildChoiceDeep(&mC, RD);
                const ChoiceItem& CI = C.item();

                LOOP_ASSERT(i, isConformant == Util::canSatisfyRecord(CI, RD));
                LOOP_ASSERT(i, isConformant ==
                                Util::isChoiceArrayItemDeepConformant(CI, RD));
                LOOP_ASSERT(i, isConformant ==
                                    Util::isChoiceArrayItemConformant(CI, RD));
                LOOP_ASSERT(i, 1 ==
                             Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC.addSelection(bdem_ElemType::BDEM_INT);
                LOOP_ASSERT(i, isConformant == Util::canSatisfyRecord(CI, RD));
            }
        }

        if (verbose) cout
            << "\t Testing conformant but not deep-conformant choices"
            << endl;
        {
            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?%*1@*1+*2#*2";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                const ChoiceItem& CI = C.item();
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC.theModifiableChoice().reset();    // unset, not null
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC = CHOICE;
                mC.theModifiableChoice().theModifiableChoice().reset();
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));
            }

            {
                const char *SPEC = ":*?A* :*?@*0%*0 :*B* :*?%*1@*1+*2#*2";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                const ChoiceItem& CI = C.item();
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC.theModifiableChoice().reset();    // unset, not null
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC = CHOICE;
                mC.theModifiableChoice().theModifiableChoiceArray().
                                                                   removeAll();
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC = CHOICE;
                mC.theModifiableChoice().theModifiableChoiceArray().
                                                  theModifiableItem(0).reset();
                if (veryVerbose) P(C);
                ASSERT(1 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC = CHOICE;
                mC.theModifiableChoice().theModifiableChoiceArray().
                                                  theModifiableItem(1).reset();
                if (veryVerbose) P(C);
                ASSERT(1 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));
            }

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?@*1+*2#*2%*1";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                const ChoiceItem& CI = C.item();
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC.theModifiableChoiceArray().removeAll();    // unset, !null
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(0).
                                                 theModifiableChoice().reset();
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(1).
                                                 theModifiableChoice().reset();
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));
            }

            {
                const char *SPEC = ":*?A* :*?@*0%*0 :*B* :*?@*1+*2#*2%*1";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                const ChoiceItem& CI = C.item();
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC.theModifiableChoiceArray().removeAll();
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(0).
                                        theModifiableChoiceArray().removeAll();
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(1).
                                        theModifiableChoiceArray().removeAll();
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(0).
                       theModifiableChoiceArray().theModifiableItem(1).reset();
                if (veryVerbose) P(C);
                ASSERT(1 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(1).
                       theModifiableChoiceArray().theModifiableItem(0).reset();
                if (veryVerbose) P(C);
                ASSERT(1 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));
            }

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?+*2#*2%*1@*1";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                const ChoiceItem& CI = C.item();
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC.theModifiableList().removeAll();    // unset, not null
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));
            }

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?#*2%*1@*1+*2";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                const ChoiceItem& CI = C.item();
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC.theModifiableTable().removeAll();
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));
            }
        }

        if (verbose) cout
            << "\t Testing shallow-conformant but not conformant choices"
            << endl;
        {
            List   mJUNK_LIST;    const List&   JUNK_LIST   = mJUNK_LIST;
            Table  mJUNK_TABLE;   const Table&  JUNK_TABLE  = mJUNK_TABLE;
            Choice mJUNK_CHOICE;  const Choice& JUNK_CHOICE = mJUNK_CHOICE;
            Array  mJUNK_ARRAY;   const Array&  JUNK_ARRAY  = mJUNK_ARRAY;

            mJUNK_LIST.appendNullDatetime();
            bsl::vector<bdem_ElemType::Type> junkListElemTypes;
            getElemTypes(&junkListElemTypes, JUNK_LIST);
            mJUNK_TABLE.reset(junkListElemTypes);
            mJUNK_CHOICE.reset(junkListElemTypes);
            mJUNK_ARRAY.reset(junkListElemTypes);

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?%*1@*1+*2#*2";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                const ChoiceItem& CI = C.item();
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC.theModifiableChoice() = JUNK_CHOICE;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC = CHOICE;
                mC.theModifiableChoice().theModifiableChoice() = JUNK_CHOICE;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));
            }

            {
                const char *SPEC = ":*?A* :*?@*0%*0 :*B* :*?%*1@*1+*2#*2";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                const ChoiceItem& CI = C.item();
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC.theModifiableChoice() = JUNK_CHOICE;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC = CHOICE;
                mC.theModifiableChoice().theModifiableChoiceArray() =
                                                                    JUNK_ARRAY;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));
            }

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?@*1+*2#*2%*1";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                const ChoiceItem& CI = C.item();
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC.theModifiableChoiceArray() = JUNK_ARRAY;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(0).
                                           theModifiableChoice() = JUNK_CHOICE;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(1).
                                           theModifiableChoice() = JUNK_CHOICE;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));
            }

            {
                const char *SPEC = ":*?A* :*?@*0%*0 :*B* :*?@*1+*2#*2%*1";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                const ChoiceItem& CI = C.item();
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC.theModifiableChoiceArray() = JUNK_ARRAY;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(0).
                                       theModifiableChoiceArray() = JUNK_ARRAY;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(1).
                                       theModifiableChoiceArray() = JUNK_ARRAY;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));
            }

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?+*2#*2%*1@*1";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                const ChoiceItem& CI = C.item();
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC.theModifiableList() = JUNK_LIST;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));
            }

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?#*2%*1@*1+*2";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                const ChoiceItem& CI = C.item();
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC.theModifiableTable() = JUNK_TABLE;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));
            }
        }

        if (verbose) cout
            << "\t Testing plain-conformant but not deep conformant choices"
            << endl;
        {
            List   mJUNK_LIST;    const List&   JUNK_LIST   = mJUNK_LIST;
            Table  mJUNK_TABLE;   const Table&  JUNK_TABLE  = mJUNK_TABLE;
            Choice mJUNK_CHOICE;  const Choice& JUNK_CHOICE = mJUNK_CHOICE;
            Array  mJUNK_ARRAY;   const Array&  JUNK_ARRAY  = mJUNK_ARRAY;

            mJUNK_LIST.appendNullDatetime();
            bsl::vector<bdem_ElemType::Type> junkListElemTypes;
            getElemTypes(&junkListElemTypes, JUNK_LIST);
            mJUNK_TABLE.reset(junkListElemTypes);
            mJUNK_CHOICE.reset(junkListElemTypes);
            mJUNK_ARRAY.reset(junkListElemTypes);

            {
                const char *SPEC = ":a=a2 :b$*a :c?+*b&NFA*B*C*";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);

                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                const ChoiceItem& CI = C.item();

                buildChoiceDeep(&mCHOICE, RD);
                mCHOICE.selection().theModifiableList()[0].makeNull();

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC.selection().makeNull();
                ASSERT(C.selection().isNull());
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC = CHOICE;
                mC.theModifiableList()[0].theModifiableInt() = 27;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(0 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(0 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));

                mC.theModifiableList()[0].theModifiableInt() = 2;
                ASSERT(1 == Util::isChoiceArrayItemDeepConformant(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemConformant(CI, RD));
                ASSERT(1 == Util::canSatisfyRecord(CI, RD));
                ASSERT(1 == Util::isChoiceArrayItemShallowConformant(CI, RD));
            }
        }

        if (verbose) cout << "\t Testing not shallow-conformant choices"
                          << endl;
        {
            const char *SPEC[] = {
            //-^
            ":*?",
            ":*?A*", ":*?B*", ":*?C*", ":*?D*", ":*?E*", ":*?F*", ":*?G*",
            ":*?H*", ":*?I*", ":*?J*", ":*?K*", ":*?L*", ":*?M*", ":*?N*",
            ":*?O*", ":*?P*", ":*?Q*", ":*?R*", ":*?S*", ":*?T*", ":*?U*",
            ":*?V*", ":*?W*", ":*?X*", ":*?Y*", ":*?Z*", ":*?a*", ":*?b*",
            ":*?c*", ":*?d*", ":*?e*", ":*?f*",
            ":*?A*B*", ":*?B*A*", ":*?A*A*", ":*?A*B*C*"
            //-v
            };

            for (int i = 0; i < SIZEOF_ARRAY(SPEC); ++i) {
                const Schema  SI(g(SPEC[i]));
                const RecDef& RI = SI.record(SI.numRecords() - 1);

                for (int j = 0; j < SIZEOF_ARRAY(SPEC); ++j) {
                    const Schema  SJ(g(SPEC[j]));
                    const RecDef& RJ = SJ.record(SJ.numRecords() - 1);
                    Choice mC;  const Choice& C = mC;
                    const ChoiceItem& CI = C.item();
                    buildChoiceDeep(&mC, RJ);

//                  bool isConformant = bsl::strpbrk(SPEC[j], "UVef")
//                                    ? false : true;
                    bool isConformant = true;

                    if (i != j) {
                        LOOP2_ASSERT(i, j, 0 ==
                                Util::isChoiceArrayItemDeepConformant(CI, RI));
                        LOOP2_ASSERT(i, j, 0 ==
                                    Util::isChoiceArrayItemConformant(CI, RI));
                        LOOP2_ASSERT(i, j, 0 ==
                             Util::isChoiceArrayItemShallowConformant(CI, RI));
                    }
                    else {
                        LOOP4_ASSERT(i, j, SPEC[i], SPEC[j], isConformant ==
                                Util::isChoiceArrayItemDeepConformant(CI, RI));
                        LOOP4_ASSERT(i, j, SPEC[i], SPEC[j], isConformant ==
                                    Util::isChoiceArrayItemConformant(CI, RI));
                        LOOP4_ASSERT(i, j, SPEC[i], SPEC[j], isConformant ==
                                               Util::canSatisfyRecord(CI, RI));
                        LOOP4_ASSERT(i, j, SPEC[i], SPEC[j], 1 ==
                             Util::isChoiceArrayItemShallowConformant(CI, RI));
                    }
                }
            }
        }

        if (verbose) cout << "\t Testing invalid record types" << endl;
        {
            const Schema X(g(":*?A*"));
            Choice mC;  const Choice& C = mC;
                    const ChoiceItem& CI = C.item();
            buildChoiceDeep(&mC, X.record(0));

            static const struct {
                int         d_lineNumber;
                const char *d_spec;
                int         d_exp;
            } DATA[] = {
                //line   data spec   exp.
                //----   ---------   ----
                { L_,    ":*?A*",    1    },
                { L_,    ":*A*",     0    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_lineNumber;
                const char *SPEC = DATA[i].d_spec;
                int         EXP  = DATA[i].d_exp;

                if (veryVerbose) { P_(LINE);  P_(SPEC);  P(EXP); }

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);

                LOOP_ASSERT(LINE, EXP ==
                                Util::isChoiceArrayItemDeepConformant(CI, RD));
                LOOP_ASSERT(LINE, EXP ==
                                    Util::isChoiceArrayItemConformant(CI, RD));
                LOOP_ASSERT(LINE, EXP == Util::canSatisfyRecord(CI, RD));
                LOOP_ASSERT(LINE, EXP ==
                             Util::isChoiceArrayItemShallowConformant(CI, RD));
            }
        }
      } break;
      case 10: {
        // ====================================================================
        // TEST CHOICE ARRAY CONFORMANT FUNCTIONS
        //
        // Testing:
        //   bool isChoiceArrayConformant(const bdem_ChoiceArray&, recDef);
        //   bool isChoiceArrayShallowConformant(const ChoiceArray&, recDef);
        //   bool isChoiceArrayDeepConformant(const ChoiceArray&, recDef);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING CHOICE ARRAY CONFORMANT FUNCTIONS" << endl
                 << "=========================================" << endl;

        if (verbose) cout << "\t Testing deep-conformant choice arrays"
                          << endl;
        {
            const char *SPEC[] = {
            //-^
            ":*?",
            ":*?A*", ":*?B*", ":*?C*", ":*?D*", ":*?E*", ":*?F*", ":*?G*",
            ":*?H*", ":*?I*", ":*?J*", ":*?K*", ":*?L*", ":*?M*", ":*?N*",
            ":*?O*", ":*?P*", ":*?Q*", ":*?R*", ":*?S*", ":*?T*", ":*?U*",
            ":*?V*", ":*?W*", ":*?X*", ":*?Y*", ":*?Z*", ":*?a*", ":*?b*",
            ":*?c*", ":*?d*", ":*?e*", ":*?f*", ":*?A*B*",
            ":*?A*B* :*?%*0", ":*?A*B* :*?@*0", ":*?A*B* :*?%*0 :*?@*1",
            ":*?C*D* :*?%*0@*0 :*E*F* :*?%*1@*1+*2#*2",
    ":*?A*B*C*D*E*F*G*H*I*J*K*L*M*N*O*P*Q*R*S*T*U*V*W*X*Y*Z*a*b*c*d*e*f*"
    ":*?A*B*C*D*E*F*G*H*I*J*K*L*M*N*O*P*Q*R*S*T*U*V*W*X*Y*Z*a*b*c*d*e*f*%*0@*0"
            //-v
            };

            for (int i = 0; i < SIZEOF_ARRAY(SPEC); ++i) {
                const Schema  S(g(SPEC[i]));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Array         mA;  const Array& A = mA;

                if (veryVerbose) { P_(i);  P(SPEC[i]) }
                buildChoiceArrayDeep(&mA, RD);

                LOOP_ASSERT(i, 1 == Util::isChoiceArrayDeepConformant(A, RD));
                LOOP_ASSERT(i, 1 == Util::isChoiceArrayConformant(A, RD));
                LOOP_ASSERT(i, 1 == Util::isChoiceArrayShallowConformant(A,
                                                                         RD));
            }
        }

        if (verbose) cout
            << "\t Testing conformant but not deep-conformant choice arrays"
            << endl;
        {
            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?%*1@*1+*2#*2";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Array         mARRAY;  const Array& ARRAY = mARRAY;
                Array         mA;      const Array& A     = mA;
                buildChoiceArrayDeep(&mARRAY, RD);

                mA = ARRAY;  // control
                ASSERT(1 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA.theModifiableItem(0).theModifiableChoice().reset();
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableChoice().reset();
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(0).theModifiableChoice().
                                                 theModifiableChoice().reset();
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableChoice().
                                                 theModifiableChoice().reset();
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));
            }

            {
                const char *SPEC = ":*?A* :*?@*0%*0 :*B* :*?%*1@*1+*2#*2";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Array         mARRAY;  const Array& ARRAY = mARRAY;
                Array         mA;      const Array& A     = mA;
                buildChoiceArrayDeep(&mARRAY, RD);

                mA = ARRAY;  // control
                ASSERT(1 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA.theModifiableItem(0).theModifiableChoice().reset();
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableChoice().reset();
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(0).theModifiableChoice().
                                        theModifiableChoiceArray().removeAll();
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(0).theModifiableChoice().
                       theModifiableChoiceArray().theModifiableItem(1).reset();
                if (veryVerbose) P(A);
                ASSERT(1 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableChoice().
                       theModifiableChoiceArray().theModifiableItem(0).reset();
                if (veryVerbose) P(A);
                ASSERT(1 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));
            }

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?@*1+*2#*2%*1";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Array         mARRAY;  const Array& ARRAY = mARRAY;
                Array         mA;      const Array& A     = mA;
                buildChoiceArrayDeep(&mARRAY, RD);

                mA = ARRAY;  // control
                ASSERT(1 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA.theModifiableItem(0).theModifiableChoiceArray().removeAll();
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableChoiceArray().removeAll();
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(0).theModifiableChoiceArray().
                                                  theModifiableItem(1).reset();
                if (veryVerbose) P(A);
                ASSERT(1 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableChoiceArray().
                                                  theModifiableItem(0).reset();
                if (veryVerbose) P(A);
                ASSERT(1 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));
            }

            {
                const char *SPEC = ":*?A* :*?@*0@*0 :*B* :*?@*1+*2#*2%*1";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Array         mARRAY;  const Array& ARRAY = mARRAY;
                Array         mA;      const Array& A     = mA;
                buildChoiceArrayDeep(&mARRAY, RD);

                mA = ARRAY;  // control
                ASSERT(1 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA.theModifiableItem(0).theModifiableChoiceArray().removeAll();
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableChoiceArray().removeAll();
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(0).theModifiableChoiceArray().
                                        theModifiableItem(1).
                                        theModifiableChoiceArray().removeAll();
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableChoiceArray().
                                        theModifiableItem(0).
                                        theModifiableChoiceArray().removeAll();
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(0).theModifiableChoiceArray().
                                        theModifiableItem(1).
                                        theModifiableChoiceArray().
                                        theModifiableItem(0).reset();
                if (veryVerbose) P(A);
                ASSERT(1 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableChoiceArray().
                               theModifiableItem(0).theModifiableChoiceArray().
                                                  theModifiableItem(1).reset();
                if (veryVerbose) P(A);
                ASSERT(1 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));
            }

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?+*2#*2%*1@*1";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Array         mARRAY;  const Array& ARRAY = mARRAY;
                Array         mA;      const Array& A     = mA;
                buildChoiceArrayDeep(&mARRAY, RD);

                mA = ARRAY;  // control
                ASSERT(1 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA.theModifiableItem(0).theModifiableList().removeAll();
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableList().removeAll();
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));
            }

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?#*2%*1@*1+*2";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Array         mARRAY;  const Array& ARRAY = mARRAY;
                Array         mA;      const Array& A     = mA;
                buildChoiceArrayDeep(&mARRAY, RD);

                mA = ARRAY;  // control
                ASSERT(1 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA.theModifiableItem(0).theModifiableTable().removeAll();
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableTable().removeAll();
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));
            }
        }

        if (verbose) cout
            << "\t Testing shallow-conformant but not conformant choice arrays"
            << endl;
        {
            List   mJUNK_LIST;    const List&   JUNK_LIST   = mJUNK_LIST;
            Table  mJUNK_TABLE;   const Table&  JUNK_TABLE  = mJUNK_TABLE;
            Choice mJUNK_CHOICE;  const Choice& JUNK_CHOICE = mJUNK_CHOICE;
            Array  mJUNK_ARRAY;   const Array&  JUNK_ARRAY  = mJUNK_ARRAY;

            mJUNK_LIST.appendNullDatetime();
            bsl::vector<bdem_ElemType::Type> junkListElemTypes;
            getElemTypes(&junkListElemTypes, JUNK_LIST);
            mJUNK_TABLE.reset(junkListElemTypes);
            mJUNK_CHOICE.reset(junkListElemTypes);
            mJUNK_ARRAY.reset(junkListElemTypes);

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?%*1@*1+*2#*2";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Array         mARRAY;  const Array& ARRAY = mARRAY;
                Array         mA;      const Array& A     = mA;
                buildChoiceArrayDeep(&mARRAY, RD);

                mA = ARRAY;  // control
                ASSERT(1 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA.theModifiableItem(0).theModifiableChoice() = JUNK_CHOICE;
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableChoice() = JUNK_CHOICE;
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(0).theModifiableChoice().
                                           theModifiableChoice() = JUNK_CHOICE;
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableChoice().
                                           theModifiableChoice() = JUNK_CHOICE;
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));
            }

            {
                const char *SPEC = ":*?A* :*?@*0%*0 :*B* :*?%*1@*1+*2#*2";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Array         mARRAY;  const Array& ARRAY = mARRAY;
                Array         mA;      const Array& A     = mA;
                buildChoiceArrayDeep(&mARRAY, RD);

                mA = ARRAY;  // control
                ASSERT(1 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA.theModifiableItem(0).theModifiableChoice() = JUNK_CHOICE;
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableChoice() = JUNK_CHOICE;
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(0).theModifiableChoice().
                                       theModifiableChoiceArray() = JUNK_ARRAY;
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableChoice().
                                       theModifiableChoiceArray() = JUNK_ARRAY;
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));
            }

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?@*1+*2#*2%*1";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Array         mARRAY;  const Array& ARRAY = mARRAY;
                Array         mA;      const Array& A     = mA;
                buildChoiceArrayDeep(&mARRAY, RD);

                mA = ARRAY;  // control
                ASSERT(1 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA.theModifiableItem(0).theModifiableChoiceArray() =
                                                                    JUNK_ARRAY;
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableChoiceArray() =
                                                                    JUNK_ARRAY;
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(0).theModifiableChoiceArray().
                      theModifiableItem(1).theModifiableChoice() = JUNK_CHOICE;
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableChoiceArray().
                                        theModifiableItem(0).
                                        theModifiableChoice() = JUNK_CHOICE;
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));
            }

            {
                const char *SPEC = ":*?A* :*?@*0@*0 :*B* :*?@*1+*2#*2%*1";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Array         mARRAY;  const Array& ARRAY = mARRAY;
                Array         mA;      const Array& A     = mA;
                buildChoiceArrayDeep(&mARRAY, RD);

                mA = ARRAY;  // control
                ASSERT(1 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA.theModifiableItem(0).theModifiableChoiceArray() =
                                                                    JUNK_ARRAY;
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableChoiceArray() =
                                                                    JUNK_ARRAY;
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));
            }

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?+*2#*2%*1@*1";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Array         mARRAY;  const Array& ARRAY = mARRAY;
                Array         mA;      const Array& A     = mA;
                buildChoiceArrayDeep(&mARRAY, RD);

                mA = ARRAY;  // control
                ASSERT(1 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA.theModifiableItem(0).theModifiableList() = JUNK_LIST;
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableList() = JUNK_LIST;
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));
            }

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?#*2%*1@*1+*2";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Array         mARRAY;  const Array& ARRAY = mARRAY;
                Array         mA;      const Array& A     = mA;
                buildChoiceArrayDeep(&mARRAY, RD);

                mA = ARRAY;  // control
                ASSERT(1 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA.theModifiableItem(0).theModifiableTable() = JUNK_TABLE;
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));

                mA = ARRAY;
                mA.theModifiableItem(1).theModifiableTable() = JUNK_TABLE;
                if (veryVerbose) P(A);
                ASSERT(0 == Util::isChoiceArrayDeepConformant(A, RD));
                ASSERT(0 == Util::isChoiceArrayConformant(A, RD));
                ASSERT(1 == Util::isChoiceArrayShallowConformant(A, RD));
            }
        }

        if (verbose) cout << "\t Testing not shallow-conformant choice arrays"
                          << endl;
        {
            const char *SPEC[] = {
            //-^
            ":*?",
            ":*?A*", ":*?B*", ":*?C*", ":*?D*", ":*?E*", ":*?F*", ":*?G*",
            ":*?H*", ":*?I*", ":*?J*", ":*?K*", ":*?L*", ":*?M*", ":*?N*",
            ":*?O*", ":*?P*", ":*?Q*", ":*?R*", ":*?S*", ":*?T*", ":*?U*",
            ":*?V*", ":*?W*", ":*?X*", ":*?Y*", ":*?Z*", ":*?a*", ":*?b*",
            ":*?c*", ":*?d*", ":*?e*", ":*?f*",
            ":*?A*B*", ":*?B*A*", ":*?A*A*", ":*?A*B*C*"
            //-v
            };

            for (int i = 0; i < SIZEOF_ARRAY(SPEC); ++i) {
                const Schema  SI(g(SPEC[i]));
                const RecDef& RI = SI.record(SI.numRecords() - 1);

                for (int j = 0; j < SIZEOF_ARRAY(SPEC); ++j) {
                    const Schema  SJ(g(SPEC[j]));
                    const RecDef& RJ = SJ.record(SJ.numRecords() - 1);
                    Array mA;  const Array& A = mA;
                    buildChoiceArrayDeep(&mA, RJ);

                    if (i != j) {
                        LOOP2_ASSERT(i, j,
                             0 == Util::isChoiceArrayDeepConformant(A, RI));
                        LOOP2_ASSERT(i, j,
                             0 == Util::isChoiceArrayConformant(A, RI));
                        LOOP2_ASSERT(i, j,
                             0 == Util::isChoiceArrayShallowConformant(A, RI));
                    }
                    else {
                        LOOP2_ASSERT(i, j,
                             1 == Util::isChoiceArrayDeepConformant(A, RI));
                        LOOP2_ASSERT(i, j,
                             1 == Util::isChoiceArrayConformant(A, RI));
                        LOOP2_ASSERT(i, j,
                             1 == Util::isChoiceArrayShallowConformant(A, RI));
                    }
                }
            }
        }

        if (verbose) cout << "\t Testing invalid record types" << endl;
        {
            const Schema X(g(":*?A*"));
            Array mA;  const Array& A = mA;
            buildChoiceArrayDeep(&mA, X.record(0));

            static const struct {
                int         d_lineNumber;
                const char *d_spec;
                int         d_exp;
            } DATA[] = {
                //line   data spec   exp.
                //----   ---------   ----
                { L_,    ":*?A*",    1    },
                { L_,    ":*A*",     0    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_lineNumber;
                const char *SPEC = DATA[i].d_spec;
                int         EXP  = DATA[i].d_exp;

                if (veryVerbose) { P_(LINE);  P_(SPEC);  P(EXP); }

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);

                LOOP_ASSERT(LINE,
                           EXP == Util::isChoiceArrayDeepConformant(A, RD));
                LOOP_ASSERT(LINE, EXP == Util::isChoiceArrayConformant(A, RD));
                LOOP_ASSERT(LINE,
                           EXP == Util::isChoiceArrayShallowConformant(A, RD));
            }
        }

      } break;
      case 9: {
        // ====================================================================
        // TEST CHOICE CONFORMANT FUNCTIONS
        //
        // Testing:
        //   bool isChoiceConformant(const bdem_Choice&, recDef);
        //   bool isChoiceShallowConformant(const bdem_Choice&, recDef);
        //   bool isChoiceDeepConformant(const bdem_Choice&, recDef);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CHOICE CONFORMANT FUNCTIONS" << endl
                          << "===================================" << endl;

        if (verbose) cout << "\t Testing deep-conformant choices" << endl;
        {
            const char *SPEC[] = {
            //-^
            ":*?",
            ":*?A*", ":*?B*", ":*?C*", ":*?D*", ":*?E*", ":*?F*", ":*?G*",
            ":*?H*", ":*?I*", ":*?J*", ":*?K*", ":*?L*", ":*?M*", ":*?N*",
            ":*?O*", ":*?P*", ":*?Q*", ":*?R*", ":*?S*", ":*?T*", ":*?U*",
            ":*?V*", ":*?W*", ":*?X*", ":*?Y*", ":*?Z*", ":*?a*", ":*?b*",
            ":*?c*", ":*?d*", ":*?e*", ":*?f*", ":*?A*B*",
            ":*?A*B* :*?%*0", ":*?A*B* :*?@*0", ":*?A*B* :*?%*0 :*?@*1",
            ":*?C*D* :*?%*0@*0 :*E*F* :*?%*1@*1+*2#*2",
    ":*?A*B*C*D*E*F*G*H*I*J*K*L*M*N*O*P*Q*R*S*T*U*V*W*X*Y*Z*a*b*c*d*e*f*"
    ":*?A*B*C*D*E*F*G*H*I*J*K*L*M*N*O*P*Q*R*S*T*U*V*W*X*Y*Z*a*b*c*d*e*f*%*0@*0"
            //-v
            };

            for (int i = 0; i < SIZEOF_ARRAY(SPEC); ++i) {
                const Schema  S(g(SPEC[i]));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mC;  const Choice& C = mC;

//              bool isConformant = bsl::strpbrk(SPEC[i], "UVef")
//                                ? false : true;
                bool isConformant = true;
                if (veryVerbose) { P_(i);  P(SPEC[i]) }
                buildChoiceDeep(&mC, RD);

                LOOP_ASSERT(i,
                          isConformant == Util::isChoiceDeepConformant(C, RD));
                LOOP_ASSERT(i,isConformant == Util::isChoiceConformant(C, RD));
                LOOP_ASSERT(i, 1 == Util::isChoiceShallowConformant(C, RD));
            }
        }

        if (verbose) cout
            << "\t Testing conformant but not deep-conformant choices"
            << endl;
        {
            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?%*1@*1+*2#*2";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(1 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC.theModifiableChoice().reset();    // unset, not null
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC = CHOICE;
                mC.theModifiableChoice().theModifiableChoice().reset();
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));
            }

            {
                const char *SPEC = ":*?A* :*?@*0%*0 :*B* :*?%*1@*1+*2#*2";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(1 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC.theModifiableChoice().reset();    // unset, not null
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC = CHOICE;
                mC.theModifiableChoice().theModifiableChoiceArray().
                                                                   removeAll();
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC = CHOICE;
                mC.theModifiableChoice().theModifiableChoiceArray().
                                                  theModifiableItem(0).reset();
                if (veryVerbose) P(C);
                ASSERT(1 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(1 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC = CHOICE;
                mC.theModifiableChoice().theModifiableChoiceArray().
                                                  theModifiableItem(1).reset();
                if (veryVerbose) P(C);
                ASSERT(1 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(1 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));
            }

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?@*1+*2#*2%*1";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(1 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC.theModifiableChoiceArray().removeAll();    // unset, !null
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(0).
                                                 theModifiableChoice().reset();
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(1).
                                                 theModifiableChoice().reset();
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));
            }

            {
                const char *SPEC = ":*?A* :*?@*0%*0 :*B* :*?@*1+*2#*2%*1";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(1 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC.theModifiableChoiceArray().removeAll();
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(0).
                                        theModifiableChoiceArray().removeAll();
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(1).
                                        theModifiableChoiceArray().removeAll();
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(0).
                       theModifiableChoiceArray().theModifiableItem(1).reset();
                if (veryVerbose) P(C);
                ASSERT(1 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(1 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(1).
                       theModifiableChoiceArray().theModifiableItem(0).reset();
                if (veryVerbose) P(C);
                ASSERT(1 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(1 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));
            }

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?+*2#*2%*1@*1";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(1 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC.theModifiableList().removeAll();    // unset, not null
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));
            }

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?#*2%*1@*1+*2";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(1 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC.theModifiableTable().removeAll();
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));
            }
        }

        if (verbose) cout
            << "\t Testing shallow-conformant but not conformant choices"
            << endl;
        {
            List   mJUNK_LIST;    const List&   JUNK_LIST   = mJUNK_LIST;
            Table  mJUNK_TABLE;   const Table&  JUNK_TABLE  = mJUNK_TABLE;
            Choice mJUNK_CHOICE;  const Choice& JUNK_CHOICE = mJUNK_CHOICE;
            Array  mJUNK_ARRAY;   const Array&  JUNK_ARRAY  = mJUNK_ARRAY;

            mJUNK_LIST.appendNullDatetime();
            bsl::vector<bdem_ElemType::Type> junkListElemTypes;
            getElemTypes(&junkListElemTypes, JUNK_LIST);
            mJUNK_TABLE.reset(junkListElemTypes);
            mJUNK_CHOICE.reset(junkListElemTypes);
            mJUNK_ARRAY.reset(junkListElemTypes);

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?%*1@*1+*2#*2";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(1 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC.theModifiableChoice() = JUNK_CHOICE;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC = CHOICE;
                mC.theModifiableChoice().theModifiableChoice() = JUNK_CHOICE;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));
            }

            {
                const char *SPEC = ":*?A* :*?@*0%*0 :*B* :*?%*1@*1+*2#*2";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(1 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC.theModifiableChoice() = JUNK_CHOICE;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC = CHOICE;
                mC.theModifiableChoice().theModifiableChoiceArray() =
                                                                    JUNK_ARRAY;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));
            }

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?@*1+*2#*2%*1";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(1 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC.theModifiableChoiceArray() = JUNK_ARRAY;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(0).
                                           theModifiableChoice() = JUNK_CHOICE;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(1).
                                           theModifiableChoice() = JUNK_CHOICE;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));
            }

            {
                const char *SPEC = ":*?A* :*?@*0%*0 :*B* :*?@*1+*2#*2%*1";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(1 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC.theModifiableChoiceArray() = JUNK_ARRAY;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(0).
                                       theModifiableChoiceArray() = JUNK_ARRAY;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC = CHOICE;
                mC.theModifiableChoiceArray().theModifiableItem(1).
                                       theModifiableChoiceArray() = JUNK_ARRAY;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));
            }

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?+*2#*2%*1@*1";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(1 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC.theModifiableList() = JUNK_LIST;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));
            }

            {
                const char *SPEC = ":*?A* :*?%*0@*0 :*B* :*?#*2%*1@*1+*2";

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Choice        mCHOICE;  const Choice& CHOICE = mCHOICE;
                Choice        mC;       const Choice& C      = mC;
                buildChoiceDeep(&mCHOICE, RD);

                mC = CHOICE;  // control
                ASSERT(1 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(1 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));

                mC.theModifiableTable() = JUNK_TABLE;
                if (veryVerbose) P(C);
                ASSERT(0 == Util::isChoiceDeepConformant(C, RD));
                ASSERT(0 == Util::isChoiceConformant(C, RD));
                ASSERT(1 == Util::isChoiceShallowConformant(C, RD));
            }
        }

        if (verbose) cout << "\t Testing not shallow-conformant choices"
                          << endl;
        {
            const char *SPEC[] = {
            //-^
            ":*?",
            ":*?A*", ":*?B*", ":*?C*", ":*?D*", ":*?E*", ":*?F*", ":*?G*",
            ":*?H*", ":*?I*", ":*?J*", ":*?K*", ":*?L*", ":*?M*", ":*?N*",
            ":*?O*", ":*?P*", ":*?Q*", ":*?R*", ":*?S*", ":*?T*", ":*?U*",
            ":*?V*", ":*?W*", ":*?X*", ":*?Y*", ":*?Z*", ":*?a*", ":*?b*",
            ":*?c*", ":*?d*", ":*?e*", ":*?f*",
            ":*?A*B*", ":*?B*A*", ":*?A*A*", ":*?A*B*C*"
            //-v
            };

            for (int i = 0; i < SIZEOF_ARRAY(SPEC); ++i) {
                const Schema  SI(g(SPEC[i]));
                const RecDef& RI = SI.record(SI.numRecords() - 1);

                for (int j = 0; j < SIZEOF_ARRAY(SPEC); ++j) {
                    const Schema  SJ(g(SPEC[j]));
                    const RecDef& RJ = SJ.record(SJ.numRecords() - 1);
                    Choice mC;  const Choice& C = mC;
                    buildChoiceDeep(&mC, RJ);

//                  bool isConformant = bsl::strpbrk(SPEC[j], "UVef")
//                                    ? false : true;
                    bool isConformant = true;

                    if (i != j) {
                        LOOP2_ASSERT(i, j,
                                  0 == Util::isChoiceDeepConformant(C, RI));
                        LOOP2_ASSERT(i, j,
                                  0 == Util::isChoiceConformant(C, RI));
                        LOOP2_ASSERT(i, j,
                                  0 == Util::isChoiceShallowConformant(C, RI));
                    }
                    else {
                        LOOP4_ASSERT(i, j, SPEC[i], SPEC[j],
                          isConformant == Util::isChoiceDeepConformant(C, RI));
                        LOOP4_ASSERT(i, j, SPEC[i], SPEC[j],
                            isConformant == Util::isChoiceConformant(C, RI));
                        LOOP4_ASSERT(i, j, SPEC[i], SPEC[j],
                                  1 == Util::isChoiceShallowConformant(C, RI));
                    }
                }
            }
        }

        if (verbose) cout << "\t Testing invalid record types" << endl;
        {
            const Schema X(g(":*?A*"));
            Choice mC;  const Choice& C = mC;
            buildChoiceDeep(&mC, X.record(0));

            static const struct {
                int         d_lineNumber;
                const char *d_spec;
                int         d_exp;
            } DATA[] = {
                //line   data spec   exp.
                //----   ---------   ----
                { L_,    ":*?A*",    1    },
                { L_,    ":*A*",     0    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_lineNumber;
                const char *SPEC = DATA[i].d_spec;
                int         EXP  = DATA[i].d_exp;

                if (veryVerbose) { P_(LINE);  P_(SPEC);  P(EXP); }

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);

                LOOP_ASSERT(LINE, EXP == Util::isChoiceDeepConformant(C, RD));
                LOOP_ASSERT(LINE, EXP == Util::isChoiceConformant(C, RD));
                LOOP_ASSERT(LINE, EXP == Util::isChoiceShallowConformant(C,
                                                                         RD));
            }
        }

      } break;
      case 8: {
        // ====================================================================
        // TEST TABLE CONFORMANT FUNCTIONS
        //
        // Testing:
        //   bool isTableConformant(const bdem_Table& table, recDef);
        //   bool isTableShallowConformant(const bdem_Table& table, recDef);
        //   bool isTableDeepConformant(const bdem_Table&, bdem_RecordDef&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "TESTING TABLE CONFORMANT FUNCTIONS" << endl
            << "==================================" << endl;

        if (verbose) cout << "\t Testing deep-conformant tables" << endl;
        {
            const char *SPEC[] = {
        //-----^
        ":*e* :*+*0 :*+*1",
        ":*f* :*+*0 :*+*1",
        ":*",
        ":*A*", ":*B*", ":*C*", ":*D*", ":*E*", ":*F*", ":*G*", ":*H*", ":*I*",
        ":*J*", ":*K*", ":*L*", ":*M*", ":*N*", ":*O*", ":*P*", ":*Q*", ":*R*",
        ":*S*", ":*T*", ":*U*", ":*V*", ":*W*", ":*X*", ":*Y*", ":*Z*", ":*a*",
        ":*b*", ":*c*", ":*d*", ":*e*", ":*f*",
        ":*A*B*", ":*A*B* :*+*0", ":*A*B* :*#*0", ":*A*B* :*+*0 :*#*1",
        ":*C*D* :*+*0#*0 :*?E*F* :*+*1#*1%*2@*2",
        ":*A*B*C*D*E*F*G*H*I*J*K*L*M*N*O*P*Q*R*S*T*U*V*W*X*Y*Z*a*b*c*d*e*f*"
        ":*A*B*C*D*E*F*G*H*I*J*K*L*M*N*O*P*Q*R*S*T*U*V*W*X*Y*Z*a*b*c*d*e*f*"
        "+*0#*0"
        //-----v
            };

            for (int i = 0; i < SIZEOF_ARRAY(SPEC); ++i) {
                const Schema  S(g(SPEC[i]));
                const RecDef& RD = S.record(S.numRecords() - 1);
                Table         mT;  const Table& T = mT;

                if (veryVerbose) { P_(i);  P(SPEC[i]) P(S) }
                buildTableDeep(&mT, RD);

//                 bool isConformant = bsl::strpbrk(SPEC[i], "e")
//                                   ? false : true;

                LOOP2_ASSERT(i, T,
                            1 == Util::isTableDeepConformant(T, RD));
                LOOP_ASSERT(i,
                            1 == Util::isTableConformant(T, RD));
                LOOP_ASSERT(i, 1 == Util::isTableShallowConformant(T, RD));
            }
        }

        if (verbose) cout
            << "\t Testing conformant but not deep-conformant tables" << endl;
        {
            const char *SPEC = ":*A* :*+*0&NT#*0 :*?B* :*+*1&NT#*1%*2@*2";

            const Schema  s(g(SPEC));
            const RecDef& RD = s.record(s.numRecords() - 1);
            Table         TABLE;
            Table         mT;     const Table& T = mT;
            buildTableDeep(&TABLE, RD);

            if (veryVerbose) P(TABLE);

            mT = TABLE;  // control
            ASSERT(1 == Util::isTableDeepConformant(T, RD));
            ASSERT(1 == Util::isTableConformant(T, RD));
            ASSERT(1 == Util::isTableShallowConformant(T, RD));

            mT.theModifiableRow(0)[0].makeNull();
            ASSERT(1 == Util::isTableDeepConformant(T, RD));
            ASSERT(1 == Util::isTableConformant(T, RD));
            ASSERT(1 == Util::isTableShallowConformant(T, RD));

            mT = TABLE;
            mT.theModifiableRow(1)[1].makeNull();
            ASSERT(0 == Util::isTableDeepConformant(T, RD));
            ASSERT(1 == Util::isTableConformant(T, RD));
            ASSERT(1 == Util::isTableShallowConformant(T, RD));

            mT = TABLE;
            mT.theModifiableRow(1)[0].theModifiableList()[0].makeNull();
            ASSERT(1 == Util::isTableDeepConformant(T, RD));
            ASSERT(1 == Util::isTableConformant(T, RD));
            ASSERT(1 == Util::isTableShallowConformant(T, RD));

            mT = TABLE;
            mT.theModifiableRow(0)[1].theModifiableTable().
                                             theModifiableRow(0)[0].makeNull();
            ASSERT(1 == Util::isTableDeepConformant(T, RD));
            ASSERT(1 == Util::isTableConformant(T, RD));
            ASSERT(1 == Util::isTableShallowConformant(T, RD));

            mT = TABLE;
            mT.theModifiableRow(0)[2].makeNull();
            ASSERT(0 == Util::isTableDeepConformant(T, RD));
            ASSERT(1 == Util::isTableConformant(T, RD));
            ASSERT(1 == Util::isTableShallowConformant(T, RD));

            mT = TABLE;
            mT.theModifiableRow(1)[3].makeNull();
            ASSERT(0 == Util::isTableDeepConformant(T, RD));
            ASSERT(1 == Util::isTableConformant(T, RD));
            ASSERT(1 == Util::isTableShallowConformant(T, RD));

            mT = TABLE;
            mT.theModifiableRow(1)[3].theModifiableChoiceArray().
                                                  theModifiableItem(0).reset();
            ASSERT(1 == Util::isTableDeepConformant(T, RD));
            ASSERT(1 == Util::isTableConformant(T, RD));
            ASSERT(1 == Util::isTableShallowConformant(T, RD));

            mT = TABLE;
            mT.theModifiableRow(1)[3].theModifiableChoiceArray().
                                                  theModifiableItem(1).reset();
            ASSERT(1 == Util::isTableDeepConformant(T, RD));
            ASSERT(1 == Util::isTableConformant(T, RD));
            ASSERT(1 == Util::isTableShallowConformant(T, RD));
        }

        if (verbose) cout
            << "\t Testing shallow-conformant but not conformant tables"
            << endl;
        {
            const char *SPEC = ":*A* :*+*0#*0 :*?B* :*+*1#*1%*2@*2";

            const Schema  s(g(SPEC));
            const RecDef& RD = s.record(s.numRecords() - 1);

            Table  mTABLE;        const Table&  TABLE       = mTABLE;
            Table  mT;            const Table&  T           = mT;
            List   mJUNK_LIST;    const List&   JUNK_LIST   = mJUNK_LIST;
            Table  mJUNK_TABLE;   const Table&  JUNK_TABLE  = mJUNK_TABLE;
            Choice mJUNK_CHOICE;  const Choice& JUNK_CHOICE = mJUNK_CHOICE;
            Array  mJUNK_ARRAY;   const Array&  JUNK_ARRAY  = mJUNK_ARRAY;

            buildTableDeep(&mTABLE, RD);
            mJUNK_LIST.appendNullDatetime();
            bsl::vector<bdem_ElemType::Type> junkListElemTypes;
            getElemTypes(&junkListElemTypes, mJUNK_LIST);
            mJUNK_TABLE.reset(junkListElemTypes);
            mJUNK_CHOICE.reset(junkListElemTypes);
            mJUNK_ARRAY.reset(junkListElemTypes);

            mT = TABLE;  // control
            ASSERT(1 == Util::isTableDeepConformant(T, RD));
            ASSERT(1 == Util::isTableConformant(T, RD));
            ASSERT(1 == Util::isTableShallowConformant(T, RD));

            mT.theModifiableRow(0)[0].theModifiableList() = JUNK_LIST;
            ASSERT(0 == Util::isTableDeepConformant(T, RD));
            ASSERT(0 == Util::isTableConformant(T, RD));
            ASSERT(1 == Util::isTableShallowConformant(T, RD));

            mT = TABLE;
            mT.theModifiableRow(1)[1].theModifiableTable() = JUNK_TABLE;
            ASSERT(0 == Util::isTableDeepConformant(T, RD));
            ASSERT(0 == Util::isTableConformant(T, RD));
            ASSERT(1 == Util::isTableShallowConformant(T, RD));

            mT = TABLE;
            mT.theModifiableRow(0)[0].theModifiableList().
                                              theModifiableList(0) = JUNK_LIST;
            ASSERT(0 == Util::isTableDeepConformant(T, RD));
            ASSERT(0 == Util::isTableConformant(T, RD));
            ASSERT(1 == Util::isTableShallowConformant(T, RD));

            mT = TABLE;
            mT.theModifiableRow(0)[0].theModifiableList().
                                            theModifiableTable(1) = JUNK_TABLE;
            ASSERT(0 == Util::isTableDeepConformant(T, RD));
            ASSERT(0 == Util::isTableConformant(T, RD));
            ASSERT(1 == Util::isTableShallowConformant(T, RD));

            mT = TABLE;
            mT.theModifiableRow(1)[1].theModifiableTable().
                                      theModifiableRow(1)[0].
                                      theModifiableList() = JUNK_LIST;
            ASSERT(0 == Util::isTableDeepConformant(T, RD));
            ASSERT(0 == Util::isTableConformant(T, RD));
            ASSERT(1 == Util::isTableShallowConformant(T, RD));

            mT = TABLE;
            mT.theModifiableRow(1)[1].theModifiableTable().
                                      theModifiableRow(0)[1].
                                      theModifiableTable() = JUNK_TABLE;
            ASSERT(0 == Util::isTableDeepConformant(T, RD));
            ASSERT(0 == Util::isTableConformant(T, RD));
            ASSERT(1 == Util::isTableShallowConformant(T, RD));

            mT = TABLE;
            mT.theModifiableRow(0)[2].theModifiableChoice() = JUNK_CHOICE;
            ASSERT(0 == Util::isTableDeepConformant(T, RD));
            ASSERT(0 == Util::isTableConformant(T, RD));
            ASSERT(1 == Util::isTableShallowConformant(T, RD));

            mT = TABLE;
            mT.theModifiableRow(1)[3].theModifiableChoiceArray() = JUNK_ARRAY;
            ASSERT(0 == Util::isTableDeepConformant(T, RD));
            ASSERT(0 == Util::isTableConformant(T, RD));
            ASSERT(1 == Util::isTableShallowConformant(T, RD));
        }

        if (verbose) cout << "\t Testing not shallow-conformant tables"
                          << endl;
        {
            const char *SPEC[] = {
            //-^
            ":*",
            ":*A*", ":*B*", ":*C*", ":*D*", ":*E*", ":*F*", ":*G*", ":*H*",
            ":*I*", ":*J*", ":*K*", ":*L*", ":*M*", ":*N*", ":*O*", ":*P*",
            ":*Q*", ":*R*", ":*S*", ":*T*", ":*U*", ":*V*", ":*W*", ":*X*",
            ":*Y*", ":*Z*", ":*a*", ":*b*", ":*c*", ":*d*", ":*e*", ":*f*",
            ":*A*B*", ":*B*A*", ":*A*A*", ":*A*B*C*"
            //-v
            };

            for (int i = 0; i < SIZEOF_ARRAY(SPEC); ++i) {
                const Schema  SI(g(SPEC[i]));
                const RecDef& RI = SI.record(SI.numRecords() - 1);

                for (int j = 0; j < SIZEOF_ARRAY(SPEC); ++j) {
                    const Schema  SJ(g(SPEC[j]));
                    const RecDef& RJ = SJ.record(SJ.numRecords() - 1);
                    Table mT;  const Table& T = mT;
                    buildTableDeep(&mT, RJ);

//                  bool isConformant = bsl::strpbrk(SPEC[j], "ef")
//                                    ? false : true;
                    bool isConformant = true;
                    if (i != j) {
                        LOOP2_ASSERT(i, j,
                                   0 == Util::isTableDeepConformant(T, RI));
                        LOOP2_ASSERT(i, j,
                                   0 == Util::isTableConformant(T, RI));
                        LOOP2_ASSERT(i, j,
                                   0 == Util::isTableShallowConformant(T, RI));
                    }
                    else {
                        LOOP2_ASSERT(i, j,
                           isConformant == Util::isTableDeepConformant(T, RI));
                        LOOP2_ASSERT(i, j,
                             isConformant == Util::isTableConformant(T, RI));
                        LOOP2_ASSERT(i, j,
                                   1 == Util::isTableShallowConformant(T, RI));
                    }
                }
            }
        }

        if (verbose) cout << "\t Testing invalid record types" << endl;
        {
            const Schema X(g(":*A*"));
            Table mT;  const Table& T = mT;
            buildTableDeep(&mT, X.record(0));

            static const struct {
                int         d_lineNumber;
                const char *d_spec;
                int         d_exp;
            } DATA[] = {
                //line   data spec   exp.
                //----   ---------   ----
                { L_,    ":*A*",     1    },
                { L_,    ":*?A*",    0    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_lineNumber;
                const char *SPEC = DATA[i].d_spec;
                int         EXP  = DATA[i].d_exp;

                if (veryVerbose) { P_(LINE);  P_(SPEC);  P(EXP); }

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);

                LOOP_ASSERT(LINE, EXP == Util::isTableDeepConformant(T, RD));
                LOOP_ASSERT(LINE, EXP == Util::isTableConformant(T, RD));
                LOOP_ASSERT(LINE, EXP == Util::isTableShallowConformant(T,
                                                                        RD));
            }
        }

      } break;
      case 7: {
        // ====================================================================
        // TEST LIST AND ROW CONFORMANT FUNCTIONS
        //
        // Testing:
        //   bool isRowConformant(const bdem_Row&, bdem_RecordDef&);
        //   bool isRowShallowConformant(const bdem_Row&, bdem_RecordDef&);
        //   bool isRowDeepConformant(const bdem_Row&, bdem_RecordDef&);
        //
        //   bool isListConformant(const bdem_List&, bdem_RecordDef&);
        //   bool isListShallowConformant(const bdem_List&, bdem_RecordDef&);
        //   bool isListDeepConformant(const bdem_List&, bdem_RecordDef&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
           << "'Test ROW and LIST CONFORMANT FUNCTIONS" << endl
           << "=======================================" << endl;

        if (verbose) cout << "\t Testing deep-conformant lists" << endl;
        {
            const char *SPEC[] = {
        //-----^
        ":*",
        ":*A*", ":*B*", ":*C*", ":*D*", ":*E*", ":*F*", ":*G*", ":*H*", ":*I*",
        ":*J*", ":*K*", ":*L*", ":*M*", ":*N*", ":*O*", ":*P*", ":*Q*", ":*R*",
        ":*S*", ":*T*", ":*U*", ":*V*", ":*W*", ":*X*", ":*Y*", ":*Z*", ":*a*",
        ":*b*", ":*c*", ":*d*", ":*e*", ":*f*",
        ":*A*B*", ":*A*B* :*+*0", ":*A*B* :*#*0", ":*A*B* :*+*0 :*#*1",
        ":*C*D* :*+*0#*0 :*?E*F* :*+*1#*1%*2@*2",
        ":*A*B*C*D*E*F*G*H*I*J*K*L*M*N*O*P*Q*R*S*T*U*V*W*X*Y*Z*a*b*c*d*e*f*"
     ":*A*B*C*D*E*F*G*H*I*J*K*L*M*N*O*P*Q*R*S*T*U*V*W*X*Y*Z*a*b*c*d*e*f*+*0#*0"
        //-----v
            };

            for (int i = 0; i < SIZEOF_ARRAY(SPEC); ++i) {
                const Schema  S(g(SPEC[i]));
                const RecDef& RD = S.record(S.numRecords() - 1);
                List          mL;  const List& L = mL;

                if (veryVerbose) { P_(i);  P(SPEC[i]) }
                buildListDeep(&mL, RD);

//              bool isConformant =
//                                bsl::strpbrk(SPEC[i], "UVef") ? false : true;
                bool isConformant = true;
                LOOP3_ASSERT(i, L.row(), SPEC[i],
                       isConformant == Util::isRowDeepConformant(L.row(), RD));
                LOOP3_ASSERT(i, L.row(), SPEC[i],
                           isConformant == Util::isRowConformant(L.row(), RD));
                LOOP2_ASSERT(i, SPEC[i],
                               1 == Util::isRowShallowConformant(L.row(), RD));

                LOOP3_ASSERT(i, L, SPEC[i],
                            isConformant == Util::isListDeepConformant(L, RD));
                LOOP3_ASSERT(i, L, SPEC[i],
                                isConformant == Util::isListConformant(L, RD));
                LOOP2_ASSERT(i, SPEC[i],
                                    1 == Util::isListShallowConformant(L, RD));
            }
        }

        if (verbose) cout
            << "\t Testing conformant but not deep-conformant lists"
            << endl;
        {
            const char *SPEC = ":*A* :*+*0&NT#*0 :*?B* :*+*1&NT#*1%*2@*2";

            const Schema  S(g(SPEC));
            const RecDef& RD = S.record(S.numRecords() - 1);
            List          mLIST;  const List& LIST = mLIST;
            List          mL;     const List& L    = mL;
            buildListDeep(&mLIST, RD);

            if (veryVerbose) P(LIST);

            mL = LIST;  // control
            ASSERT(1 == Util::isRowDeepConformant(L.row(), RD));
            ASSERT(1 == Util::isRowConformant(L.row(), RD));
            ASSERT(1 == Util::isRowShallowConformant(L.row(), RD));

            ASSERT(1 == Util::isListDeepConformant(L, RD));
            ASSERT(1 == Util::isListConformant(L, RD));
            ASSERT(1 == Util::isListShallowConformant(L, RD));

            mL[0].makeNull();

            ASSERT(1 == Util::isRowDeepConformant(L.row(), RD));
            ASSERT(1 == Util::isRowConformant(L.row(), RD));
            ASSERT(1 == Util::isRowShallowConformant(L.row(), RD));

            ASSERT(1 == Util::isListDeepConformant(L, RD));
            ASSERT(1 == Util::isListConformant(L, RD));
            ASSERT(1 == Util::isListShallowConformant(L, RD));

            mL = LIST;
            mL[1].makeNull();

            ASSERT(0 == Util::isRowDeepConformant(L.row(), RD));
            ASSERT(1 == Util::isRowConformant(L.row(), RD));
            ASSERT(1 == Util::isRowShallowConformant(L.row(), RD));

            ASSERT(0 == Util::isListDeepConformant(L, RD));
            ASSERT(1 == Util::isListConformant(L, RD));
            ASSERT(1 == Util::isListShallowConformant(L, RD));

            mL = LIST;
            mL.theModifiableList(0)[0].makeNull();
            ASSERT(1 == Util::isRowDeepConformant(L.row(), RD));
            ASSERT(1 == Util::isRowConformant(L.row(), RD));
            ASSERT(1 == Util::isRowShallowConformant(L.row(), RD));

            ASSERT(1 == Util::isListDeepConformant(L, RD));
            ASSERT(1 == Util::isListConformant(L, RD));
            ASSERT(1 == Util::isListShallowConformant(L, RD));

            mL = LIST;
            mL.theModifiableTable(1).theModifiableRow(0)[0].makeNull();
            ASSERT(1 == Util::isRowDeepConformant(L.row(), RD));
            ASSERT(1 == Util::isRowConformant(L.row(), RD));
            ASSERT(1 == Util::isRowShallowConformant(L.row(), RD));

            ASSERT(1 == Util::isListDeepConformant(L, RD));
            ASSERT(1 == Util::isListConformant(L, RD));
            ASSERT(1 == Util::isListShallowConformant(L, RD));

            mL = LIST;
            mL.theModifiableTable(1).theModifiableRow(1)[1].makeNull();
            ASSERT(0 == Util::isRowDeepConformant(L.row(), RD));
            ASSERT(1 == Util::isRowConformant(L.row(), RD));
            ASSERT(1 == Util::isRowShallowConformant(L.row(), RD));

            ASSERT(0 == Util::isListDeepConformant(L, RD));
            ASSERT(1 == Util::isListConformant(L, RD));
            ASSERT(1 == Util::isListShallowConformant(L, RD));

            mL = LIST;
            mL.theModifiableChoice(2).reset();    // unset but not null
            ASSERT(0 == Util::isRowDeepConformant(L.row(), RD));
            ASSERT(0 == Util::isRowConformant(L.row(), RD));
            ASSERT(1 == Util::isRowShallowConformant(L.row(), RD));

            ASSERT(0 == Util::isListDeepConformant(L, RD));
            ASSERT(0 == Util::isListConformant(L, RD));
            ASSERT(1 == Util::isListShallowConformant(L, RD));

            mL = LIST;
            mL.theModifiableChoiceArray(3).theModifiableItem(0).reset();
                                        // unset but not null - catalog intact
            ASSERT(1 == Util::isRowDeepConformant(L.row(), RD));
            ASSERT(1 == Util::isRowConformant(L.row(), RD));
            ASSERT(1 == Util::isRowShallowConformant(L.row(), RD));

            ASSERT(1 == Util::isListDeepConformant(L, RD));
            ASSERT(1 == Util::isListConformant(L, RD));
            ASSERT(1 == Util::isListShallowConformant(L, RD));

            mL = LIST;
            mL.theModifiableChoiceArray(3).theModifiableItem(1).reset();
            ASSERT(1 == Util::isRowDeepConformant(L.row(), RD));
            ASSERT(1 == Util::isRowConformant(L.row(), RD));
            ASSERT(1 == Util::isRowShallowConformant(L.row(), RD));

            ASSERT(1 == Util::isListDeepConformant(L, RD));
            ASSERT(1 == Util::isListConformant(L, RD));
            ASSERT(1 == Util::isListShallowConformant(L, RD));
        }

        if (verbose) cout
            << "\t Testing shallow-conformant but not conformant lists"
            << endl;
        {
            const char *SPEC = ":*A* :*+*0#*0 :*?B* :*+*1#*1%*2@*2";

            const Schema  s(g(SPEC));
            const RecDef& RD = s.record(s.numRecords() - 1);

            List   mLIST;         const List&   LIST        = mLIST;
            List   mL;            const List&   L           = mL;
            List   mJUNK_LIST;    const List&   JUNK_LIST   = mJUNK_LIST;
            Table  mJUNK_TABLE;   const Table&  JUNK_TABLE  = mJUNK_TABLE;
            Choice mJUNK_CHOICE;  const Choice& JUNK_CHOICE = mJUNK_CHOICE;
            Array  mJUNK_ARRAY;   const Array&  JUNK_ARRAY  = mJUNK_ARRAY;

            buildListDeep(&mLIST, RD);
            mJUNK_LIST.appendNullDatetime();
            bsl::vector<bdem_ElemType::Type> junkListElemTypes;
            getElemTypes(&junkListElemTypes, JUNK_LIST);
            mJUNK_TABLE.reset(junkListElemTypes);
            mJUNK_CHOICE.reset(junkListElemTypes);
            mJUNK_ARRAY.reset(junkListElemTypes);

            mL = LIST;  // control
            ASSERT(1 == Util::isRowDeepConformant(L.row(), RD));
            ASSERT(1 == Util::isRowConformant(L.row(), RD));
            ASSERT(1 == Util::isRowShallowConformant(L.row(), RD));

            ASSERT(1 == Util::isListDeepConformant(L, RD));
            ASSERT(1 == Util::isListConformant(L, RD));
            ASSERT(1 == Util::isListShallowConformant(L, RD));

            mL.theModifiableList(0) = JUNK_LIST;
            if (veryVerbose) P(L);
            ASSERT(0 == Util::isRowDeepConformant(L.row(), RD));
            ASSERT(0 == Util::isRowConformant(L.row(), RD));
            ASSERT(1 == Util::isRowShallowConformant(L.row(), RD));

            ASSERT(0 == Util::isListDeepConformant(L, RD));
            ASSERT(0 == Util::isListConformant(L, RD));
            ASSERT(1 == Util::isListShallowConformant(L, RD));

            mL = LIST;
            mL.theModifiableTable(1) = JUNK_TABLE;
            if (veryVerbose) P(L);
            ASSERT(0 == Util::isRowDeepConformant(L.row(), RD));
            ASSERT(0 == Util::isRowConformant(L.row(), RD));
            ASSERT(1 == Util::isRowShallowConformant(L.row(), RD));

            ASSERT(0 == Util::isListDeepConformant(L, RD));
            ASSERT(0 == Util::isListConformant(L, RD));
            ASSERT(1 == Util::isListShallowConformant(L, RD));

            mL = LIST;
            mL.theModifiableList(0).theModifiableList(0) = JUNK_LIST;
            if (veryVerbose) P(L);
            ASSERT(0 == Util::isRowDeepConformant(L.row(), RD));
            ASSERT(0 == Util::isRowConformant(L.row(), RD));
            ASSERT(1 == Util::isRowShallowConformant(L.row(), RD));

            ASSERT(0 == Util::isListDeepConformant(L, RD));
            ASSERT(0 == Util::isListConformant(L, RD));
            ASSERT(1 == Util::isListShallowConformant(L, RD));

            mL = LIST;
            mL.theModifiableList(0).theModifiableTable(1) = JUNK_TABLE;
            if (veryVerbose) P(L);
            ASSERT(0 == Util::isRowDeepConformant(L.row(), RD));
            ASSERT(0 == Util::isRowConformant(L.row(), RD));
            ASSERT(1 == Util::isRowShallowConformant(L.row(), RD));

            ASSERT(0 == Util::isListDeepConformant(L, RD));
            ASSERT(0 == Util::isListConformant(L, RD));
            ASSERT(1 == Util::isListShallowConformant(L, RD));

            mL = LIST;
            mL.theModifiableTable(1).theModifiableRow(0)[0].
                                               theModifiableList() = JUNK_LIST;
            if (veryVerbose) P(L);
            ASSERT(0 == Util::isRowDeepConformant(L.row(), RD));
            ASSERT(0 == Util::isRowConformant(L.row(), RD));
            ASSERT(1 == Util::isRowShallowConformant(L.row(), RD));

            ASSERT(0 == Util::isListDeepConformant(L, RD));
            ASSERT(0 == Util::isListConformant(L, RD));
            ASSERT(1 == Util::isListShallowConformant(L, RD));

            mL = LIST;
            mL.theModifiableTable(1).theModifiableRow(1)[1].
                                            theModifiableTable() = JUNK_TABLE;
            if (veryVerbose) P(L);
            ASSERT(0 == Util::isRowDeepConformant(L.row(), RD));
            ASSERT(0 == Util::isRowConformant(L.row(), RD));
            ASSERT(1 == Util::isRowShallowConformant(L.row(), RD));

            ASSERT(0 == Util::isListDeepConformant(L, RD));
            ASSERT(0 == Util::isListConformant(L, RD));
            ASSERT(1 == Util::isListShallowConformant(L, RD));

            mL = LIST;
            mL.theModifiableChoice(2) = JUNK_CHOICE;
            if (veryVerbose) P(L);
            ASSERT(0 == Util::isRowDeepConformant(L.row(), RD));
            ASSERT(0 == Util::isRowConformant(L.row(), RD));
            ASSERT(1 == Util::isRowShallowConformant(L.row(), RD));

            ASSERT(0 == Util::isListDeepConformant(L, RD));
            ASSERT(0 == Util::isListConformant(L, RD));
            ASSERT(1 == Util::isListShallowConformant(L, RD));

            mL = LIST;
            mL.theModifiableChoiceArray(3) = JUNK_ARRAY;
            if (veryVerbose) P(L);
            ASSERT(0 == Util::isRowDeepConformant(L.row(), RD));
            ASSERT(0 == Util::isRowConformant(L.row(), RD));
            ASSERT(1 == Util::isRowShallowConformant(L.row(), RD));

            ASSERT(0 == Util::isListDeepConformant(L, RD));
            ASSERT(0 == Util::isListConformant(L, RD));
            ASSERT(1 == Util::isListShallowConformant(L, RD));
        }

        if (verbose) cout << "\t Testing not shallow-conformant lists" << endl;
        {
            const char *SPEC[] = {
            //-^
            ":*",
            ":*A*", ":*B*", ":*C*", ":*D*", ":*E*", ":*F*", ":*G*", ":*H*",
            ":*I*", ":*J*", ":*K*", ":*L*", ":*M*", ":*N*", ":*O*", ":*P*",
            ":*Q*", ":*R*", ":*S*", ":*T*", ":*U*", ":*V*", ":*W*", ":*X*",
            ":*Y*", ":*Z*", ":*a*", ":*b*", ":*c*", ":*d*", ":*e*", ":*f*",
            ":*A*B*", ":*B*A*", ":*A*A*", ":*A*B*C*"
            //-v
            };

            for (int i = 0; i < SIZEOF_ARRAY(SPEC); ++i) {
                const Schema  SI(g(SPEC[i]));
                const RecDef& RI = SI.record(SI.numRecords() - 1);

                for (int j = 0; j < SIZEOF_ARRAY(SPEC); ++j) {
                    const Schema  SJ(g(SPEC[j]));
                    const RecDef& RJ = SJ.record(SJ.numRecords() - 1);
                    List mL;  const List& L = mL;
                    buildListDeep(&mL, RJ);

//                  bool isConformant = bsl::strpbrk(SPEC[j], "UVef")
//                                    ? false : true;
                    bool isConformant = true;
                    if (i != j) {
                        LOOP2_ASSERT(i, j,
                               0 == Util::isRowDeepConformant(L.row(), RI));
                        LOOP2_ASSERT(i, j,
                               0 == Util::isRowConformant(L.row(), RI));
                        LOOP2_ASSERT(i, j,
                               0 == Util::isRowShallowConformant(L.row(), RI));

                        LOOP2_ASSERT(i, j,
                               0 == Util::isListDeepConformant(L, RI));
                        LOOP2_ASSERT(i, j,
                               0 == Util::isListConformant(L, RI));
                        LOOP2_ASSERT(i, j,
                               0 == Util::isListShallowConformant(L, RI));
                    }
                    else {
                        LOOP2_ASSERT(i, j,
                          isConformant == Util::isRowDeepConformant(L.row(),
                                                                    RI));
                        LOOP2_ASSERT(i, j,
                          isConformant == Util::isRowConformant(L.row(), RI));
                        LOOP2_ASSERT(i, j,
                                  1 == Util::isRowShallowConformant(L.row(),
                                                                    RI));

                        LOOP2_ASSERT(i, j,
                               isConformant == Util::isListDeepConformant(L,
                                                                          RI));
                        LOOP2_ASSERT(i, j,
                                isConformant == Util::isListConformant(L, RI));
                        LOOP2_ASSERT(i, j,
                                     1 == Util::isListShallowConformant(L,
                                                                        RI));
                    }
                }
            }
        }

        if (verbose) cout << "\t Testing invalid record types" << endl;
        {
            const Schema X(g(":*A*"));
            List mL;  const List& L = mL;
            buildListDeep(&mL, X.record(0));

            static const struct {
                int         d_lineNumber;
                const char *d_spec;
                int         d_exp;
            } DATA[] = {
                //line   data spec   exp.
                //----   ---------   ----
                { L_,    ":*A*",     1    },
                { L_,    ":*?A*",    0    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_lineNumber;
                const char *SPEC = DATA[i].d_spec;
                int         EXP  = DATA[i].d_exp;

                if (veryVerbose) { P_(LINE);  P_(SPEC);  P(EXP); }

                const Schema  S(g(SPEC));
                const RecDef& RD = S.record(S.numRecords() - 1);

                LOOP_ASSERT(LINE, EXP == Util::isRowDeepConformant(L.row(),
                                                                   RD));
                LOOP_ASSERT(LINE, EXP == Util::isRowConformant(L.row(), RD));
                LOOP_ASSERT(LINE, EXP == Util::isRowShallowConformant(L.row(),
                                                                      RD));

                LOOP_ASSERT(LINE, EXP == Util::isListDeepConformant(L, RD));
                LOOP_ASSERT(LINE, EXP == Util::isListConformant(L, RD));
                LOOP_ASSERT(LINE, EXP == Util::isListShallowConformant(L, RD));
            }
        }

      } break;
      case 6: {
        // ====================================================================
        // TEST 'initRowDeep' AND 'initListDeep'
        //   For each schema SPEC in a set, create a corresponding schema and
        //   get the last recordDef of the schema.  Then, using this recordDef,
        //   deep-init a list and verify correctness of the result.
        //
        // Testing:
        //   initRowDeep(bdem_Row *row, const bdem_RecordDef& record);
        //   initListDeep(bdem_List *list, const bdem_RecordDef& record);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing initListDeep:" << endl
                                  << "=====================" << endl;

        const char *SPEC[] = {
        //-^
        ":*",
        ":*A*", ":*B*", ":*C*", ":*D*", ":*E*", ":*F*", ":*G*", ":*H*", ":*I*",
        ":*J*", ":*K*", ":*L*", ":*M*", ":*N*", ":*O*", ":*P*", ":*Q*", ":*R*",
        ":*S*", ":*T*", ":*U*", ":*V*", ":*W*", ":*X*", ":*Y*", ":*Z*", ":*a*",
        ":*b*", ":*c*", ":*d*", ":*e*", ":*f*", ":*+*0", ":*#*0", ":*A*B*",
        ":*A*B* :*+*0", ":*A*B* :*#*0", ":*?A*B* :*%*0", ":*?A*B* :*@*0",
        ":*A*B*C*D*E*F*G*H*I*J*K*L*M*N*O*P*Q*R*S*T*U*V*W*X*Y*Z*a*b*c*d*e*f*"
                                                                      "+*0#*0"
        ":*A*B*C*D*E*F*G*H*I*J*K*L*M*N*O*P*Q*R*S*T*U*V*W*X*Y*Z*a*b*c*d*e*f*"
                                                                      "+*0#*0",
        ":aA*B*C* :bD*E*F* :c+*a+*b :d+*c+*c"
        //-v
        };

        if (verbose) cout << "\t Testing 'initListDeep'" << endl;
        {
            for (int i = 0; i < SIZEOF_ARRAY(SPEC); ++i) {
                const Schema  s(g(SPEC[i]));
                const RecDef& RD = s.record(s.numRecords() - 1);
                List          list;

                if (veryVerbose) { P_(i);  P(SPEC[i]) }
                Util::initListDeep(&list, RD);
                if (veryVeryVerbose) cout << list << endl;
                LOOP2_ASSERT(i, SPEC[i], verifyDeepInitList(list, RD));
            }
        }

        if (verbose) cout << "\t Testing 'initRowDeep'" << endl;
        {
            for (int i = 0; i < SIZEOF_ARRAY(SPEC); ++i) {
                const Schema  s(g(SPEC[i]));
                const RecDef& RD = s.record(s.numRecords() - 1);

                bsl::vector<bdem_ElemType::Type> elemTypes;
                getElemTypes(&elemTypes, RD);

                if (veryVerbose) { P_(i);  P(SPEC[i]) }

                List list(elemTypes);
                Util::initRowDeep(&list.row(), RD);

                if (veryVeryVerbose) cout << list << endl;
                LOOP2_ASSERT(i, SPEC[i], verifyDeepInitList(list, RD));
            }
        }

        if (verbose) cout << "\t Testing default values" << endl;
        {
            Schema s;  const Schema& S = s;
            RecDef *record = s.createRecord();  ASSERT(record);

            for (int i = 0; i < NUM_TYPES; ++i) {
                const EType::Type type = (EType::Type)i;
                if (EType::isScalarType(type)) {
                    FldAttr mFA(type);  const FldAttr& FA = mFA;
                    setDefaultValueForType(mFA.defaultValue());
                    const FldDef *field = record->appendField(FA);
                    ASSERT(field);
                }
            }

            const RecDef& RD1 = S.record(0);
            const int NUM_SCALAR_TYPES = 14;
            ASSERT(NUM_SCALAR_TYPES == RD1.numFields());

            record = s.createRecord();  ASSERT(record);
            const FldDef *field = record->appendField(EType::BDEM_LIST, &RD1);
            ASSERT(field);

            const RecDef& RD2 = S.record(1);
            ASSERT(1 == RD2.numFields());

            {
                List list;
                Util::initListDeep(&list, RD1);

                ASSERT(verifyDeepInitList(list, RD1));
            }

            {
                List list;
                Util::initListDeep(&list, RD2);

                ASSERT(verifyDeepInitList(list, RD2));
            }

            {
                bsl::vector<bdem_ElemType::Type> elemTypes;
                getElemTypes(&elemTypes, RD1);

                List list(elemTypes);
                Util::initRowDeep(&list.row(), RD1);

                ASSERT(verifyDeepInitList(list, RD1));
            }

            {
                bsl::vector<bdem_ElemType::Type> elemTypes;
                getElemTypes(&elemTypes, RD2);

                List list(elemTypes);
                Util::initRowDeep(&list.row(), RD2);

                ASSERT(verifyDeepInitList(list, RD2));
            }
        }

      } break;
      case 5: {
        // ====================================================================
        // TEST 'initChoice' AND 'initChoiceArray'
        //   For each schema SPEC in a set, create a corresponding schema and
        //   get the last recordDef of the schema.  Then, using this recordDef,
        //   init a choice and a choice array and verify correctness of the
        //   result.
        //
        // Testing:
        //   initChoice(bdem_Choice *choice, const bdem_RecordDef& record);
        //   initChoiceArray(*choiceArray, const bdem_RecordDef& record);
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CHOICE[ARRAY] INITIALIZATION:" << endl
                          << "=====================================" << endl;

        const char *SPEC[] = {
        //-^
        ":*?",
        ":*?A*", ":*?B*", ":*?C*", ":*?D*", ":*?E*", ":*?F*", ":*?G*", ":*?H*",
        ":*?I*", ":*?J*", ":*?K*", ":*?L*", ":*?M*", ":*?N*", ":*?O*", ":*?P*",
        ":*?Q*", ":*?R*", ":*?S*", ":*?T*", ":*?U*", ":*?V*", ":*?W*", ":*?X*",
        ":*?Y*", ":*?Z*", ":*?a*", ":*?b*", ":*?c*", ":*?d*", ":*?e*", ":*?f*",
        ":*?%*0", ":*?@*0", ":*?A*B*", ":*A*B* :*?+*0", ":*A*B* :*?#*0",
    ":*?A*B*C*D*E*F*G*H*I*J*K*L*M*N*O*P*Q*R*S*T*U*V*W*X*Y*Z*a*b*c*d*e*f*%*0@*0"
    ":*?A*B*C*D*E*F*G*H*I*J*K*L*M*N*O*P*Q*R*S*T*U*V*W*X*Y*Z*a*b*c*d*e*f*%*0@*0"
        //-v
        };

        if (verbose) cout << "\t Testing 'initChoice'" << endl;
        {
            for (int i = 0; i < SIZEOF_ARRAY(SPEC); ++i) {
                const Schema  s(g(SPEC[i]));
                const RecDef& RD = s.record(s.numRecords() - 1);
                Choice        choice;
                Util::initChoice(&choice, RD);

                bsl::vector<bdem_ElemType::Type> elemTypes;
                choice.selectionTypes(&elemTypes);
                if (choice != Choice(elemTypes)) {
                    P(choice);
                    P(Choice(elemTypes));
                }
                LOOP_ASSERT(i, choice == Choice(elemTypes));
            }
        }

        if (verbose) cout << "\t Testing 'initChoiceArray'" << endl;
        {
            for (int i = 0; i < SIZEOF_ARRAY(SPEC); ++i) {
                const Schema  s(g(SPEC[i]));
                const RecDef& RD = s.record(s.numRecords() - 1);
                Array         array;
                Util::initChoiceArray(&array, RD);

                bsl::vector<bdem_ElemType::Type> elemTypes;
                array.selectionTypes(&elemTypes);
                LOOP_ASSERT(i, array == Array(elemTypes));
            }
        }

      } break;
      case 4: {
        // ====================================================================
        // TEST 'initTable'
        //   For each schema SPEC in a set, create a corresponding schema and
        //   get the last recordDef of the schema.  Then, using this recordDef,
        //   init a table and verify correctness.
        //
        // Testing:
        //   initTable(bdem_Table *table, const bdem_RecordDef& record);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing initTable:" << endl
                                  << "==================" << endl;

        const char *SPEC[] = {
        //-^
        ":*",
        ":*A*", ":*B*", ":*C*", ":*D*", ":*E*", ":*F*", ":*G*", ":*H*", ":*I*",
        ":*J*", ":*K*", ":*L*", ":*M*", ":*N*", ":*O*", ":*P*", ":*Q*", ":*R*",
        ":*S*", ":*T*", ":*U*", ":*V*", ":*W*", ":*X*", ":*Y*", ":*Z*", ":*a*",
        ":*b*", ":*c*", ":*d*", ":*e*", ":*f*", ":*+*0", ":*#*0", ":*A*B*",
        ":*A*B* :*+*0", ":*A*B* :*#*0", ":*?A*B* :*%*0", ":*?A*B* :*@*0",
     ":*A*B*C*D*E*F*G*H*I*J*K*L*M*N*O*P*Q*R*S*T*U*V*W*X*Y*Z*a*b*c*d*e*f*+*0#*0"
     ":*A*B*C*D*E*F*G*H*I*J*K*L*M*N*O*P*Q*R*S*T*U*V*W*X*Y*Z*a*b*c*d*e*f*+*0#*0"
        //-v
        };

        for (int i = 0; i < SIZEOF_ARRAY(SPEC); ++i) {
            const Schema  s(g(SPEC[i]));
            const RecDef& RD = s.record(s.numRecords() - 1);
            Table         table;

            if (veryVerbose) { P_(i);  P(SPEC[i]) }
            Util::initTable(&table, RD);

            bsl::vector<bdem_ElemType::Type> elemTypes;
            table.columnTypes(&elemTypes);
            LOOP_ASSERT(i, table == Table(elemTypes));
        }

      } break;
      case 3: {
        // ====================================================================
        // TEST 'initRowShallow', 'initListShallow', AND 'initListAllNull'
        //   For each schema SPEC in a set, create a corresponding schema and
        //   get the last recordDef of the schema.  Then, using this recordDef,
        //   init a list and a row and verify correctness.
        //
        // Testing:
        //   initRowShallow(bdem_Row *row, const bdem_RecordDef& record);
        //   initListAllNull(bdem_List *list, const bdem_RecordDef& record);
        //   initListShallow(bdem_List *list, const bdem_RecordDef& record);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing single-level list initialization:" << endl
            << "=========================================" << endl;

        const char *SPEC[] = {
        //-^
        ":*",
        ":*A*", ":*B*", ":*C*", ":*D*", ":*E*", ":*F*", ":*G*", ":*H*", ":*I*",
        ":*J*", ":*K*", ":*L*", ":*M*", ":*N*", ":*O*", ":*P*", ":*Q*", ":*R*",
        ":*S*", ":*T*", ":*U*", ":*V*", ":*W*", ":*X*", ":*Y*", ":*Z*", ":*a*",
        ":*b*", ":*c*", ":*d*", ":*e*", ":*f*", ":*+*0", ":*#*0", ":*A*B*",
        ":*A*B* :*+*0", ":*A*B* :*#*0", ":*?A*B* :*%*0", ":*?A*B* :*@*0",
     ":*A*B*C*D*E*F*G*H*I*J*K*L*M*N*O*P*Q*R*S*T*U*V*W*X*Y*Z*a*b*c*d*e*f*+*0#*0"
     ":*A*B*C*D*E*F*G*H*I*J*K*L*M*N*O*P*Q*R*S*T*U*V*W*X*Y*Z*a*b*c*d*e*f*+*0#*0"
        //-v
        };

        if (verbose) cout << "\t Testing 'initListShallow'" << endl;
        {
            for (int i = 0; i < SIZEOF_ARRAY(SPEC); ++i) {
                const Schema  s(g(SPEC[i]));
                const RecDef& RD = s.record(s.numRecords() - 1);
                List          list;

                if (veryVerbose) P(i);
                Util::initListShallow(&list, RD);

                bsl::vector<bdem_ElemType::Type> elemTypes;
                list.elemTypes(&elemTypes);

                if (veryVerbose) P(list);

                bool noAggregates = true;
                for (int j = 0; j < list.length(); ++j) {
                    if (EType::isAggregateType(elemTypes[j])) {
                        noAggregates = false;
                        break;
                    }
                }

                if (noAggregates) {
                    List newList(elemTypes);
                    LOOP4_ASSERT(i, list, newList, SPEC[i], list == newList);
                }
            }
        }

        if (verbose) cout << "\t Testing 'initRowShallow'" << endl;
        {
            for (int i = 0; i < SIZEOF_ARRAY(SPEC); ++i) {
                const Schema  s(g(SPEC[i]));
                const RecDef& RD = s.record(s.numRecords() - 1);

                bsl::vector<bdem_ElemType::Type> elemTypes;
                getElemTypes(&elemTypes, RD);

                bool noAggregates = true;
                for (int j = 0; j < (int) elemTypes.size(); ++j) {
                    if (EType::isAggregateType(elemTypes[j])) {
                        noAggregates = false;
                        break;
                    }
                }

                if (noAggregates) {
                    List list(elemTypes);
                    Util::initRowShallow(&list.row(), RD);

                    List newList(elemTypes);
                    LOOP3_ASSERT(i, list, newList, list == newList);
                }
            }
        }

        if (verbose) cout << "\t Testing 'initListAllNull'" << endl;
        {
            for (int i = 0; i < SIZEOF_ARRAY(SPEC); ++i) {
                const Schema  s(g(SPEC[i]));
                const RecDef& RD = s.record(s.numRecords() - 1);
                List          list;
                Util::initListAllNull(&list, RD);

                bsl::vector<bdem_ElemType::Type> elemTypes;
                list.elemTypes(&elemTypes);
                List L(elemTypes);
                L.makeAllNull();
                LOOP_ASSERT(i, list == L);
            }
        }

        if (verbose) cout << "\t Testing default values" << endl;
        {
            Schema s;  const Schema& S = s;
            RecDef *record = s.createRecord();  ASSERT(record);

            for (int i = 0; i < NUM_TYPES; ++i) {
                const EType::Type type = (EType::Type)i;
                if (EType::isScalarType(type)) {
                    FldAttr mFA(type);  const FldAttr& FA = mFA;
                    setDefaultValueForType(mFA.defaultValue());
                    const FldDef *field = record->appendField(FA);
                    ASSERT(field);
                }
            }

            const RecDef& RD = S.record(S.numRecords() - 1);
            const int NUM_SCALAR_TYPES = 14;
            ASSERT(NUM_SCALAR_TYPES == RD.numFields());

            {
                List list;
                Util::initListShallow(&list, RD);

                ASSERT(verifyDeepInitList(list, RD));
            }

            {
                bsl::vector<bdem_ElemType::Type> elemTypes;
                getElemTypes(&elemTypes, RD);

                List list(elemTypes);
                Util::initRowShallow(&list.row(), RD);

                ASSERT(verifyDeepInitList(list, RD));
            }

            {
                List list;
                Util::initListAllNull(&list, RD);

                bsl::vector<bdem_ElemType::Type> elemTypes;
                list.elemTypes(&elemTypes);
                List L(elemTypes);
                L.makeAllNull();
                ASSERT(list == L);
            }
        }

      } break;
      case 2: {
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

        bslma_TestAllocator  testAllocator(veryVeryVerbose);
        bslma_Allocator     *Z = &testAllocator;

        for (int i = 0; i < NUM_SPECS; ++i) {
            const char *spec = testSpecs[i];
            if (veryVerbose) P(spec);

            Schema x(Z);  const Schema& X = x;  gg(&x, spec);
            if (veryVerbose) { PS(g(spec)); PS(X); }

            LOOP_ASSERT(i, g(spec) == X);
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'gg' GENERATOR
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

        bslma_TestAllocator  testAllocator(veryVeryVerbose);
        bslma_Allocator     *Z = &testAllocator;

        Schema x1(Z);  const Schema& X1 = x1;

        {
            if (verbose)  cout << "\nTesting 'gg' with empty string." << endl;
            gg(&x1, "");  ASSERT(0 == X1.numRecords());
        }

        {
            if (verbose)
                cout << "\nTesting 'gg' with all unconstrained types." << endl;

            int i;  // loop variables, declared here to keep MSVC++ happy
            for (i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :* ]*"; // The ] gets overwritten.
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

                if (veryVerbose) PS(x1);
            }

            if (verbose) cout << "\nAll unconstrained types, with name c "
                              << "(\"c\")." << endl;
            for (i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :* ]c"; // The ] gets overwritten.
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

                if (veryVerbose)  PS(x1);
            }

            if (verbose) cout << "\nAll unconstrained types, with name c "
                              << "(\"c\") and record name d (\"d\")." << endl;
            for (i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :d ]c"; // The ] gets overwritten.
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

                if (veryVerbose) PS(x1);
            }

            if (verbose)
                cout << "\nTesting 'gg' with LIST/TABLE constrained types."
                     << endl;
            static char constrTypes[] = "+#";
            for (i = 0; constrTypes[i]; ++i) {
                static char spec[] = "~ :c Ad :* ]dc"; // ']' gets replaced
                spec[11] = constrTypes[i];

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

                if (veryVerbose)  PS(x1);
            }

            if (verbose)
                cout << "\nTesting 'gg' with CHOICE/CHOICE_ARRAY "
                        "constrained types." << endl;
            static const char constrChoiceTypes[] = "%@";
            for (i = 0; constrChoiceTypes[i]; ++i) {
                static char spec[] = "~ :c? Ad :* ]dc"; // ']' gets replaced
                spec[12] = constrChoiceTypes[i];

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
                ASSERT(ggElemTypes[34 + i] == rec->field(0).elemType());

                if (veryVerbose)  PS(x1);
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
                        spec += '?';  // odd 'i' are CHOICE records
                    }
                    for (j = 0; j < NUM_TYPES; ++j) {
                        spec += ' ';
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

                gg(&x1, spec.data());
                ASSERT(X1.numRecords() == NUM_GG_NAMES);

                if (veryVerbose)  PS(x1);

                for (i = 0; i < NUM_GG_NAMES; ++i) {
                    const RecDef *R = &X1.record(i);
                    ASSERT(0 != R);
                    ASSERT(0 == strCmp(recName(*R), ggName(i)));

                    for (j = 0; j < NUM_TYPES; ++j) {
                        LOOP_ASSERT(j, ggElemTypes[j] ==
                                           R->field(j).elemType());
                        LOOP_ASSERT(j, 0 ==
                                           strCmp(R->fieldName(j), ggName(j)));
                        LOOP_ASSERT(j, 0 == R->field(j).recordConstraint());
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

            {
                Schema x1(Z);  const Schema& X1 = x1;

                if (verbose) cout << "\nTest hasDefaultValue configuration\n";

                gg(&x1, ":aAaBb&nTCc&nF");

                if (veryVerbose) {
                    P(x1);
                }

                const bdem_RecordDef& record = X1.record(0);
                ASSERT(1 == record.field(0).defaultValue().isNull());
                ASSERT(1 == record.field(1).defaultValue().isNull());
                ASSERT(1 == record.field(2).defaultValue().isNull());
            }
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    else {
        if (verbose) cout << "(success)" << endl;
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
