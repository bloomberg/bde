// bdem_schema.t.cpp            -*-C++-*-

#include <bdem_schema.h>

#include <bdem_descriptor.h>
#include <bdem_elemattrlookup.h>
#include <bdem_elemref.h>
#include <bdem_elemtype.h>

#include <bdeat_formattingmode.h>

#include <bdetu_unset.h>

#include <bdet_datetime.h>
#include <bdet_datetimetz.h>
#include <bdet_date.h>
#include <bdet_datetz.h>
#include <bdet_time.h>
#include <bdet_timetz.h>

#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testoutstream.h>                 // for testing only
#include <bdex_testinstreamexception.h>         // for testing only
#include <bdema_bufferedsequentialallocator.h>  // for testing only

#include <bslma_defaultallocatorguard.h>     // for testing only
#include <bslma_testallocator.h>             // for testing only
#include <bslma_testallocatorexception.h>    // for testing only

#include <bsls_platform.h>                   // for testing only
#include <bsls_platformutil.h>               // for testing only

#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>
#include <bsl_vector.h>

#include <bsl_cctype.h>                      // isspace()
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This component exposes three classes, 'bdem_FieldDef', 'bdem_RecordDef', and
// 'bdem_Schema', to implement the "schema" (logical) object.  Since only
// 'bdem_Schema' has public constructors, the test strategy is similar to that
// for a single value-semantic object with a rich, non-standard set of
// manipulators and accessors, except that special care must be taken when
// the subordinate objects are used (both explicitly and implicitly).
//
// In light of the above considerations, we modify the normal test sequence for
// testing a value-semantic object.  The general order of the usual first ten
// test cases is preserved, but the canonical "case 4" ("Basic Accessors") is
// expanded to four actual test cases (4 - 7), because the logical schema
// entity has many accessors.  The sequence of testing accessors goes from
// highest-level (true 'bdem_Schema' methods) in case 4 to lowest-level
// ('bdem_FieldDef' methods) in case 8 because, given a schema (the only
// publicly-created type), one needs the higher-level accessors to access the
// lower-level objects before the lower-level accessors can be invoked.
//
// The primary manipulators are:
//     'bdem_Schema::createRecord'
//     'bdem_RecordDef::appendField'
// Because of the way that 'bdem_Schema' is implemented, there is no (untested,
// independent) white-box state, and so no formal testing of a white-box
// primary manipulator is required.  Nevertheless, the 'removeAll' method is
// tested in its "usual" manner and location in case 2 with the primary
// manipulators.  This choice is made for both convenience and thoroughness.
//
// As an expedience, chosen because of the complexity of the schema object
// (which is substantially an array of record definitions, each of which is an
// array of field definitions), this test driver defines a global set of test
// objects (in the form of an array of 'testSpecs' for 'gg') that are used in
// several test cases.  As a result, the test data selection for any one case
// may not be "optimal", but the test driver as a whole is easier to write and
// to maintain.  Careful attention to the concerns in each case ensures that
// thoroughness is not compromised, although a small run-time penalty may
// result.
//
//-----------------------------------------------------------------------------
// 'bdem_FieldSpec' public interface:
// [16] bdem_FieldSpec(EType type, const bdem_RecordDef *c=0, *ba=0);
// [17] bdem_FieldSpec(EType type, *c, formattingMode, isNullable, *ba=0);
// [22] bdem_FieldSpec(const bdem_FieldSpec& original, *ba=0);
// [16] ~bdem_FieldSpec();
//
// [20] bdem_FieldSpec& operator=(const bdem_FieldSpec& rhs);
// [16] bdem_ElemRef defaultValue();
// [21] int reset(EType type, const bdem_RecordDef *c=0);
// [21] int reset(EType type, *c, formattingMode, isNullable);
// [21] int setConstraint(const bdem_RecordDef *constraint);
// [21] int setFormattingMode(int formattingMode);
// [21] int setIsNullable(bool isNullable);
//
// [16] const bdem_RecordDef *recordConstraint() const;
// [16] bdem_ConstElemRef defaultValue() const;
// [16] EType elemType() const;
// [17] int formattingMode() const;
// [17] bool isNullable() const;
// [16] bool hasDefaultValue() const;
// [23] ostream& print(ostream& stream, level, spl) const;
//
// [19] operator==(const bdem_FieldSpec& lhs, const bdem_FieldSpec& rhs);
// [19] operator!=(const bdem_FieldSpec& lhs, const bdem_FieldSpec& rhs);
//-----------------------------------------------------------------------------
// 'bdem_FieldDef' private methods (tested indirectly):
// [ 2] bdem_FieldDef(*name, int id, const bdem_FieldSpec&, *ba);
// [ 2] ~bdem_FieldDef();
// [ ?] int setIsNullable(bool isNullable);
// [25] int fieldId() const;
// [ 7] const char *fieldName() const;
//
// 'bdem_FieldDef' public interface:
// [ 9] const bdem_RecordDef *recordConstraint() const;
// [22] bdem_ConstElemRef defaultValue() const;
// [ 9] EType elemType() const;
// [24] const bdem_FieldSpec& fieldSpec() const;
// [24] int formattingMode() const;
// [24] bool hasDefaultValue() const;
// [24] bool isNullable() const;
//
// [ 9] operator==(const bdem_FieldDef& lhs, const bdem_FieldDef& rhs);
// [ 9] operator!=(const bdem_FieldDef& lhs, const bdem_FieldDef& rhs);
//-----------------------------------------------------------------------------
// 'bdem_EnumerationDef' private methods (tested indirectly):
// [ 2] bdem_EnumerationDef(Schema *, index, *name, *writeOnceAllocator);
// [ 2] ~bdem_EnumerationDef();
// [ ?] STREAM& bdexStreamIn(STREAM& stream, int detailedVersion);
// [ 2] void setEnumerationName(const char *name);
// [ ?] STREAM& bdexStreamOut(STREAM& stream, int detailedVersion) const;
// [ 4] const char *enumerationName() const;
// [ 2] bslma_Allocator *writeOnceAllocator() const;
//
// 'bdem_EnumerationDef' public interface:
// [30] static bool areEquivalent(EnumerationDef& lhs, EnumerationDef& rhs);
// [31] static bool isSuperset(EnumerationDef& super, EnumerationDef& sub);
// [ 6] static bool canHaveEnumConstraint(bdem_ElemType::Type type);
// [ 2] int addEnumerator(const char *name);
// [ 2] int addEnumerator(const char *name, int id);
// [ 2] void assignAlphabeticalIDs(int firstId = 0);
// [ 6] int enumerationIndex() const;
// [ 8] const char *lookupName(int id) const;
// [ 8] const char *lookupName(const char* name) const;
// [ 8] int lookupId(const char *name) const;
// [ 6] int numEnumerators() const;
// [ 6] int maxId() const;
// [ 6] bsl::pair<const char *, int> nextLargerName(const char* name) const;
// [ 6] bsl::pair<const char *, int> nextLargerId(int id) const;
// [10] bsl::ostream& print(ostream& stream, level, spacesPerLevel) const;
// [ 6] const bdem_Schema& schema() const;
//
//-----------------------------------------------------------------------------
// 'bdem_RecordDef' private methods (tested indirectly):
// [ 2] bdem_RecordDef(const Schema *s, index, *name, type, *pool, *ba);
// [ 2] ~bdem_RecordDef();                // purify
// [  ] STREAM& bdexStreamIn(STREAM& stream, int detailedVersion);
// [ 2] void setRecordName(const char *name);
// [15] bdexStreamOut(STREAM& stream, int version, int minorVersion) const;
// [ 4] const char *recordName() const;
// [ 2] bslma_Allocator *writeOnceAllocator() const;
//
// 'bdem_RecordDef' public interface:
// [ 2] const FldDef *appendField(EType, const RecDef& c, *name=0);
// [24] const FldDef *appendField(const FldSpec&, *name=0, id=Null);
// [ 2] const FldDef *appendField(EType type, *name=0, id=Null);
// [ 2] const FldDef *appendField(type, *c, *name=0, id=Null);
// [ 7] const bdem_FieldDef& field(int index) const;
// [25] int fieldId(int index) const;
// [ 7] int fieldIndex(const char *name) const;
// [25] int fieldIndex(int id) const;
// [32] int fieldIndexExtended(const char *name) const;
// [ 7] const char *fieldName(int index) const;
// [25] bool hasFieldIds() const;
// [ 5] int length() const;
// [ 7] const bdem_FieldDef *lookupField(const char *name) const;
// [25] const bdem_FieldDef *lookupField(int id) const;
// [32] int numAnonymousFields() const;
// [10] ostream& print(ostream& stream, level, spl) const;
// [ 5] int recordIndex() const;
// [26] RecordType recordType() const;
// [ 5] const bdem_Schema& schema() const;
//-----------------------------------------------------------------------------
// 'bdem_Schema' private interface (tested indirectly):
// [15] streamInRecordDef(STREAM& stream, *schema, *ba, major, minor);
// [15] int bdexMinorVersion() const;
//
// 'bdem_Schema' public interface:
// [15] static int maxSupportedBdexVersion();
// [ 2] bdem_Schema(bslma_Allocator *ba=0);
// [12] bdem_Schema(const bdem_Schema& original, *ba=0);
// [ 2] ~bdem_Schema();                // purify
// [14] bdem_Schema& operator=(const bdem_Schema& rhs);
// [26] bdem_RecordDef *createRecord(bdem_RecordDef::RecordType type);
// [ 2] bdem_RecordDef *createRecord(*name=0, type=BDEM_SEQUENCE_RECORD);
// [ 2] bdem_EnumerationDef *createEnumeration(const char *name = 0);
// [ 2] void removeAll();
// [26] int setRecordName(bdem_RecordDef *record, const char *name);
// [27] int setEnumerationName(EnumerationDef *enumeration, const char *name);
// [15] bdexStreamIn(STREAM& stream, int version);
// [ 4] int numRecords() const;
// [ 4] int numEnumerations() const;
// [ 4] const bdem_RecordDef *lookupRecord(const char *name) const;
// [ 4] const bdem_RecordDef& record(int index) const;
// [ 4] int recordIndex(const char *name) const;
// [ 4] const char *recordName(int index) const;
// [ 4] const bdem_EnumerationDef *lookupEnumeration(const char *name) const;
// [ 4] const bdem_EnumerationDef& enumeration(int index) const;
// [ 4] int enumerationIndex(const char *name) const;
// [ 4] const char *enumerationName(int index) const;
// [10] ostream& print(ostream& stream, level, spl) const;
// [15] ostream& streamOut(ostream& stream) const;
// [15] bdexStreamOut(STREAM& stream, int version) const;
//
// [11] bool operator==(const bdem_Schema& lhs, const bdem_Schema& rhs);
// [11] bool operator!=(const bdem_Schema& lhs, const bdem_Schema& rhs);
// [10] stream& operator<<(ostream& stream, const bdem_Schema& schema);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] bdem_Schema& gg(bdem_Schema *s, const char *spec); (generator function)
// [ 5] const char *recName(const bdem_RecordDef&);      (test helper function)
// [11] bdem_Schema g(const char *spec);                   (generator function)
// [29] FORWARD REFERENCES
// [33] USAGE

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
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);\
             }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
                          << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                          << "\t" << #K << ": " << K << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                          << "\t" << #K << ": " << K << "\t" << #L << ": " \
                          << L << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\t" << #M << ": " << M << "\n"; \
               aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\t" << #M << ": " << M << "\t" << #N \
                         << ": " << N << "\n"; \
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
#define L_ __LINE__                              // current Line number
#define PS(X) bsl::cout << #X " = \n" << (X) << bsl::endl;
                                                 // Print identifier and value.
#define T_()  bsl::cout << "\t" << bsl::flush;   // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdem_Schema              Obj;
typedef bdem_Schema              Schema;

typedef bdem_ElemRef             ERef;
typedef bdem_ElemType            EType;
typedef bdem_FieldDef            FldDef;
typedef bdem_FieldSpec           FldSpec;
typedef bdem_FieldDefAttributes  FldAttr;
typedef bdem_RecordDef           RecDef;
typedef bdem_EnumerationDef      EnumDef;
typedef RecDef::RecordType       RecType;
typedef bdem_ConstElemRef        CERef;

typedef bdeat_FormattingMode     Format;

typedef bdex_TestInStream        In;
typedef bdex_TestOutStream       Out;

typedef bsls_PlatformUtil::Int64 Int64;

const int NUM_TYPES = EType::BDEM_NUM_TYPES;

// 'bdem_FieldSpec' defaults
const int FORMAT = 0;  // formatting mode

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
const bsl::string     B06 = "two--plus-some-extra-stuff-to-force-allocation";
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

namespace {

const char *recName(const RecDef& record)
    // Return the name of the specified 'record'.  Note that 'record' does not
    // know its name.  Rather, 'record.schema()' accesses its parent schema,
    // which in turn has a 'recordName' method.  This helper just implements
    // less-convenient 'bdem_schema' functionality.
{
    return record.schema().recordName(record.recordIndex());
}

const char *enumName(const EnumDef& en)
    // Return the name of the specified 'en'.  Note that 'en' does not know
    // its name.  Rather, 'en.schema()' accesses its parent schema, which in
    // turn has an 'enumerationName' method.  This helper just implements
    // less-convenient 'bdem_schema' functionality.
{
    return en.schema().enumerationName(en.enumerationIndex());
}

int strCmp(const char* lhs, const char* rhs)
    // Return 0 if the specified 'lhs' and 'rhs' null-terminated strings are
    // equal, a negative value if lhs < rhs, and a positive value if lhs > rhs.
    // Note that the behavior is well-defined for null-pointer arguments.
{
    if (lhs == 0 && rhs == 0) return 0;
    if (lhs == 0) return -1;
    if (rhs == 0) return 1;
    return bsl::strcmp(lhs, rhs);
}

void makeDefaultValueUnset(const bdem_ElemRef& elemRef)
{
    switch (elemRef.type()) {
      case bdem_ElemType::BDEM_CHAR: {
        elemRef.theModifiableChar()        = N00;
      } break;
      case bdem_ElemType::BDEM_SHORT: {
        elemRef.theModifiableShort()       = N01;
      } break;
      case bdem_ElemType::BDEM_INT: {
        elemRef.theModifiableInt()         = N02;
      } break;
      case bdem_ElemType::BDEM_INT64: {
        elemRef.theModifiableInt64()       = N03;
      } break;
      case bdem_ElemType::BDEM_FLOAT: {
        elemRef.theModifiableFloat()       = N04;
      } break;
      case bdem_ElemType::BDEM_DOUBLE: {
        elemRef.theModifiableDouble()      = N05;
      } break;
      case bdem_ElemType::BDEM_STRING: {
        elemRef.theModifiableString()      = N06;
      } break;
      case bdem_ElemType::BDEM_DATETIME: {
        elemRef.theModifiableDatetime()    = N07;
      } break;
      case bdem_ElemType::BDEM_DATE: {
        elemRef.theModifiableDate()        = N08;
      } break;
      case bdem_ElemType::BDEM_TIME: {
        elemRef.theModifiableTime()        = N09;
      } break;
      case bdem_ElemType::BDEM_BOOL: {
        elemRef.theModifiableBool()        = N22;
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ: {
        elemRef.theModifiableDatetimeTz()  = N23;
      } break;
      case bdem_ElemType::BDEM_DATETZ: {
        elemRef.theModifiableDateTz()      = N24;
      } break;
      case bdem_ElemType::BDEM_TIMETZ: {
        elemRef.theModifiableTimeTz()      = N25;
      } break;
      default: {
        ASSERT("Shouldn't get here" == 0);
      } break;
    }
}

//=============================================================================
//                        GENERATOR LANGUAGE FOR gg
//=============================================================================
//
// The gg function interprets a given 'spec' in order from left to right to
// build up a schema according to a custom language.  Lowercase letters
// [a ..  x] specify arbitrary (but unique) names to be used for records or
// fields.  Records and fields may be unnamed; this is specified by using the
// '*' character for the name.  Field types are indicated with uppercase and
// lowercase letters [A ..  Z, a ..  f] for unconstrained fields, '+' for a
// constrained list, '#' for a constrained table, '%' for a constrained choice,
// and '@' for a constrained choice array.
//
// 'spec' consists of 0 or more record definitions.  (An empty spec will result
// in a valid empty schema.)  Records are specified by a leading ':' and name
// character followed by 0 or more field definitions.  A field definition
// is either an unconstrained type specifier followed by a name and 0 or more
// field attribute specifications, or a constrained field specifier followed by
// a name, a constraint specification, and 0 or more field attribute
// specifications.  A constraint specification is either the name of a
// (previously defined) record within this schema, or the 0-based index of a
// record within this schema, less than or equal to the 0-based index of the
// current record.  A field attribute specification is a 3-character sequence
// of the form "&av" where 'a' denotes a field attribute and 'v' denotes a
// value for that attribute.
//
// As may be obvious from this description, encountering a name specification
// character where a field type would have been expected indicates the start of
// a new record.  A '~' indicates that 'removeAll' is performed on the target
// schema.  The '~' is legal anywhere that a record definition could begin.
// Spaces, tabs, and newlines are allowed in the specification, but are
// ignored; they are purely cosmetic.
//
// Annotated EBNF grammar:
//  (For those unfamiliar with Extended-BNF, parentheses indicate grouping,
//  and "*" means "0 or more of the preceding".  Although this grammar does
//  not use it, EBNF also defines "+" as meaning "1 or more of the preceding"
//  and "?" as "0 or 1 of the preceding".  As in BNF, "|" separate
//  alternatives.)
//
//  Note that items following %% comment markers are not part of the grammar,
//  but are comments about semantics.
//
//  Spaces, tabs, and newlines are ignored by the parser, and are not specified
//  in this grammar although they are legal anywhere in the spec.
//
//  Spec      := ('~' | SeqRecordDef | ChoiceRecordDef | EnumerationDef)*
//
//  SeqRecordDef := ':' Name FieldDef*
//
//  ChoiceRecordDef := ':' Name '?' FieldDef*
//
//  EnumerationDef := ':' Name '=' Enumerator*
//
//  FieldDef  := (UnconstrainedFieldType Name AttrDef*)
//             | (ConstrainedFieldType Name ConstraintRef AttrDef*)
//
//  Enumerator := Name [ Index ]
//
//  Name      :=
//            '*'                   %% No name, the 0 char *
//           |'a'|'b'|'c'|'d'       %% "a".."d"
//           |'e'                   %% ""  <- An empty string, not "e"
//           |'f'|'g'|'h'|'i'|'j'   %% "f".."j"
//           |'k'|'l'|'m'|'n'|'o'   %% "k".."o"
//           |'p'|'q'|'r'|'s'|'t'   %% "p".."t"
//           |'u'|'v'|'w'|'x'       %% "u".."x"
//
//  UnconstrainedFieldType :=
//            'A'|'B'|'C'|'D'|'E'   %% CHAR, SHORT, INT, INT64, FLOAT
//           |'F'|'G'|'H'|'I'|'J'   %% DOUBLE, STRING, DATETIME, DATE, TIME
//           |'K'|'L'|'M'|'N'|'O'   %% (CHAR|SHORT|INT|INT64|FLOAT)_ARRAY
//           |'P'|'Q'|'R'|'S'|'T'   %% (DOUBLE|STRING|DATETIME|DATE|TIME)_ARRAY
//           |'U'|'V'               %% LIST, TABLE
//           |'W'|'X'|'Y'|'Z'       %% BOOL, DATETIMETZ, DATETZ, TIMETZ
//           |'a'|'b'|'c'|'d'       %% (BOOL|DATETIMETZ|DATETZ|TIMETZ)_ARRAY
//           |'e'|'f'               %% CHOICE, CHOICE_ARRAY
//
//  ConstrainedFieldType :=
//            '+'|'#'|'%'|'@'       %% LIST, TABLE, CHOICE, CHOICE_ARRAY
//           |'$'|'^'               %% INT enumeration, STRING enumeration
//           |'!'|'/'               %% INT_ARRAY enum, STRING_ARRAY enum
//
//  ConstraintRef := Name | Index
//
//  %% If a constraint is referenced by index, the index is the 0-based number
//  %% of the constraint record in the schema.  Since we're limiting this to
//  %% one digit, we can't constrain by index on any record past the 10th one.
//  %% We also allow constraining by name, so this isn't a big problem.
//  Index     :=
//            '0'|'1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'
//
//  AttrDef   := '&' AttrKind AttrValue
//
//  AttrKind  :=
//            'D'                   %% Field default value
//           |'F'                   %% Field formatting mode
//           |'I'                   %% Field id
//           |'N'                   %% Field nullability
//
//  AttrValue :=
//             '0'|'1'                           %% AttrKind == 'D'
//            |'0'|'B'|'D'|'L'|'N'|'T'|'U'|'X'   %% AttrKind == 'F'
//            |Index                             %% AttrKind == 'I'
//            |'D'|'F'|'T'                       %% AttrKind == 'N'
//
// 'gg' syntax usage examples:
//  Here are some examples, and the schema which would result when applied to
//  an empty schema.  Since our 'gg' language defines '~' for 'removeAll', all
//  'spec' strings should begin with '~' to insure expected behavior.
//
// Spec String      Result schema          Description
// -----------      -------------          -----------
// ""               (empty)                An empty string is a legal spec,
//                                         but has no effect.
//
// " \t\n  "        (empty)                Whitespace characters are ignored;
//                                         this is equivalent to "".
//
// ":a"             SCHEMA {               A schema containing a single empty
//                      RECORD "a" {       record named "a".
//                      }
//                  }
//
// ":a~"            SCHEMA {               An empty schema, since ~ performs
//                  }                      a removeAll().
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
// ":aFc"           SCHEMA {               A schema containing one record named
//                      RECORD "a" {       "a" with one DOUBLE field named "c".
//                          DOUBLE "c";
//                      }
//                  }
//
// ":aCa"           SCHEMA {               A schema containing one record named
//                      RECORD "a" {       "a" with one INT field named "a".
//                          INT "a";       Note that fields and records have
//                      }                  separate namespaces.
//                  }
//
// ":aG*"           SCHEMA {               A schema containing one record named
//                      RECORD "a" {       "a" with one STRING field which is
//                          STRING;        unnamed.
//                      }
//                  }
//
// ":aGbHc"         SCHEMA {               A schema containing one record named
//                      RECORD "a" {       "a" with two fields, one a STRING
//                          STRING "b";    named "b" and the other a DATETIME
//                          DATETIME "c";  named "c".
//                      }
//                  }
//
// ":aGb:cHc"       SCHEMA {               A schema containing two records.
//                      RECORD "a" {       Record "a" has one STRING field
//                          STRING "b";    named "b".
//                      }
//                      RECORD "c" {       Record "c" has one DATETIME field
//                          DATETIME "c";  named "c".
//                      }
//                  }
//
// ":aGb :cHc"      SCHEMA {               Exactly the same as the previous
//                      RECORD "a" {       example ":aGb:cHc", but more
//                          STRING "b";    readable with a space inserted
//                      }                  between the record definitions
//                      RECORD "c" {
//                          DATETIME "c";
//                      }
//                  }
//
// ":*C*F* :*G*H* :*:*"
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
// ":aFc :d+ea"     SCHEMA {               A schema containing 2 records.
//                      RECORD "a" {       Record "a" has one DOUBLE field
//                          DOUBLE "c";    named "c".
//                      }
//                      RECORD "d" {       Record "d" has one constrained LIST
//                          LIST<"a"> "";  field named "" constrained on record
//                      }                  "a".
//                  }
//
// ":aFc :d#ea"     SCHEMA {               A schema containing 2 records.
//                      RECORD "a" {       Record "a" has one DOUBLE field
//                          DOUBLE "c";    named "c".
//                      }
//                      RECORD "d" {       Record "d" has one constrained TABLE
//                          TABLE<"a"> ""; field named "" constrained on record
//                      }                  "a".
//                  }
//
// ":aFc :d#e0"     SCHEMA {               A schema containing 2 records.
//                      RECORD "a" {       Record "a" has one DOUBLE field
//                          DOUBLE "c";    named "c".
//                      }
//                      RECORD "d" {       Record "d" has one constrained TABLE
//                          TABLE<"a"> ""; field named "" constrained on record
//                      }                  "a", referring to "a" by it's index,
//                  }                      0.
//
// ":a~:a~:a~:a~"   SCHEMA {               Equivalent to "" except for
//                  }                      white-box implications.
//
// ":a=uv3 :k$fa/g0"
//     SCHEMA {                            A schema containing one enumeration
//         ENUMERATION "a" {               and one record.  Enumeration "a"
//             "you" => 0;                 has two enumerators "you" and "vee",
//             "vee" => 3;                 where "vee" has an explicit ID of 3.
//         }
//         RECORD "k" {                    Record "k" has one INT enumeration
//             INT ENUM<"a"> "f";          field named "f" and one STRING_ARRAY
//             STRING_ARRAY ENUM<"a"> "g"; enumeration field named "g", both
//         }                               of which are constrained by
//     }                                   enumeration "a".
//
// ":a=bc3 :e$fa"   SCHEMA {               A schema containing one enumeration
//                      ENUMERATION "a" {  and one record.  Enumeration "a"
//                          "b" = 0;     has two enumerators "b" and "c",
//                          "c" = 3;       where "c" has an explicit ID of 3.
//                      }
//                      RECORD "e" {           Record "e" has one INT
//                          INT ENUM<"a"> "f"; enumeration field named "f",
//                      }                      which is constrained by
//                  }                          enumeration "a".
//
//=============================================================================

const EType::Type ggElemTypes[] = {
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
    EType::BDEM_CHOICE_ARRAY,     // 35 (constrained, but type still
                                  //     'CHOICE_ARRAY')
    EType::BDEM_INT,              // 36 (enumeration, but type still 'INT')
    EType::BDEM_STRING,           // 37 (enumeration, but type still 'STRING')
    EType::BDEM_INT_ARRAY,        // 38 (enumeration, but type still
                                  //     'INT_ARRAY')
    EType::BDEM_STRING_ARRAY      // 39 (enumeration, but type still
                                  //     'STRING_ARRAY')
};

const int NUM_GG_TYPES = sizeof ggElemTypes / sizeof *ggElemTypes;

static const char *ggElemTypeNames[] = {
    "bdem_ElemType::BDEM_CHAR",                       //  0
    "bdem_ElemType::BDEM_SHORT",                      //  1
    "bdem_ElemType::BDEM_INT",                        //  2
    "bdem_ElemType::BDEM_INT64",                      //  3
    "bdem_ElemType::BDEM_FLOAT",                      //  4
    "bdem_ElemType::BDEM_DOUBLE",                     //  5
    "bdem_ElemType::BDEM_STRING",                     //  6
    "bdem_ElemType::BDEM_DATETIME",                   //  7
    "bdem_ElemType::BDEM_DATE",                       //  8
    "bdem_ElemType::BDEM_TIME",                       //  9
    "bdem_ElemType::BDEM_CHAR_ARRAY",                 // 10
    "bdem_ElemType::BDEM_SHORT_ARRAY",                // 11
    "bdem_ElemType::BDEM_INT_ARRAY",                  // 12
    "bdem_ElemType::BDEM_INT64_ARRAY",                // 13
    "bdem_ElemType::BDEM_FLOAT_ARRAY",                // 14
    "bdem_ElemType::BDEM_DOUBLE_ARRAY",               // 15
    "bdem_ElemType::BDEM_STRING_ARRAY",               // 16
    "bdem_ElemType::BDEM_DATETIME_ARRAY",             // 17
    "bdem_ElemType::BDEM_DATE_ARRAY",                 // 18
    "bdem_ElemType::BDEM_TIME_ARRAY",                 // 19
    "bdem_ElemType::BDEM_LIST",                       // 20
    "bdem_ElemType::BDEM_TABLE",                      // 21
    "bdem_ElemType::BDEM_BOOL",                       // 22
    "bdem_ElemType::BDEM_DATETIMETZ",                 // 23
    "bdem_ElemType::BDEM_DATETZ",                     // 24
    "bdem_ElemType::BDEM_TIMETZ",                     // 25
    "bdem_ElemType::BDEM_BOOL_ARRAY",                 // 26
    "bdem_ElemType::BDEM_DATETIMETZ_ARRAY",           // 27
    "bdem_ElemType::BDEM_DATETZ_ARRAY",               // 28
    "bdem_ElemType::BDEM_TIMETZ_ARRAY",               // 29
    "bdem_ElemType::BDEM_CHOICE",                     // 30
    "bdem_ElemType::BDEM_CHOICE_ARRAY",               // 31
    "bdem_ElemType::BDEM_LIST (constrained)",         // 32
    "bdem_ElemType::BDEM_TABLE (constrainted)",       // 33
    "bdem_ElemType::BDEM_CHOICE (constrained)",       // 34
    "bdem_ElemType::BDEM_CHOICE_ARRAY (constrainted)",// 35
    "bdem_ElemType::BDEM_INT (enumeration)",          // 36
    "bdem_ElemType::BDEM_STRING (enumeration)",       // 37
    "bdem_ElemType::BDEM_INT_ARRAY (enumeration)",    // 38
    "bdem_ElemType::BDEM_STRING_ARRAY (enumeration)"  // 39
};

struct Assertions {
    char typeNameAssertion[
             NUM_GG_TYPES == sizeof ggElemTypeNames / sizeof *ggElemTypeNames];
};

//=============================================================================
//                bdem_Schema gg HELPER DATA AND FUNCTIONS
//=============================================================================

// NOTE: 'index' is used in string.h on AIX so switched to indexStr
const char removeTilde[]    = "~";
const char name[]           = "*abcdefghijklmnopqrstuvwx";
const char indexStr[]       = "0123456789";
const char bdemType[]       = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
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

void setDefaultValueForType(ERef ref, bool other = false)
    // Set the value of the element referenced by the specified 'ref' to a
    // non-null value appropriate for its type.  If the optionally-specified
    // 'other' flag is 'true', set the value of the element referenced by
    // 'ref' to an alternate non-null value.  The behavior is undefined unless
    // the type of the element referenced by 'ref' is a scalar 'bdem' type or
    // array of scalar 'bdem' types.
{
    EType::Type elemType = ref.type();
    if (EType::isArrayType(elemType)) {
        elemType = EType::fromArrayType(elemType);
    }

    switch (elemType) {
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

bool verifyDefaultValueForType(CERef ref, bool other = false)
    // Return 'true' if the value of the non-modifiable element referenced by
    // the specified 'ref' has the first non-null exemplar value, or the
    // optionally-specified 'other' flag is 'true' and the value of the element
    // referenced by 'ref' is of the alternate non-null exemplar value; return
    // 'false' otherwise.  The behavior is undefined unless the type of the
    // element referenced by 'ref' is a scalar 'bdem' type.
{
    EType::Type elemType = ref.type();
    if (EType::isArrayType(elemType)) {
        elemType = EType::fromArrayType(elemType);
    }

    switch (elemType) {
      case EType::BDEM_CHAR: {
        return (other ? B00 : A00) == ref.theChar();
      } break;
      case EType::BDEM_SHORT: {
        return (other ? B01 : A01) == ref.theShort();
      } break;
      case EType::BDEM_INT: {
        return (other ? B02 : A02) == ref.theInt();
      } break;
      case EType::BDEM_INT64: {
        return (other ? B03 : A03) == ref.theInt64();
      } break;
      case EType::BDEM_FLOAT: {
        return (other ? B04 : A04) == ref.theFloat();
      } break;
      case EType::BDEM_DOUBLE: {
        return (other ? B05 : A05) == ref.theDouble();
      } break;
      case EType::BDEM_STRING: {
        return (other ? B06 : A06) == ref.theString();
      } break;
      case EType::BDEM_DATETIME: {
        return (other ? B07 : A07) == ref.theDatetime();
      } break;
      case EType::BDEM_DATE: {
        return (other ? B08 : A08) == ref.theDate();
      } break;
      case EType::BDEM_TIME: {
        return (other ? B09 : A09) == ref.theTime();
      } break;
      case EType::BDEM_BOOL: {
        return (other ? B22 : A22) == ref.theBool();
      } break;
      case EType::BDEM_DATETIMETZ: {
        return (other ? B23 : A23) == ref.theDatetimeTz();
      } break;
      case EType::BDEM_DATETZ: {
        return (other ? B24 : A24) == ref.theDateTz();
      } break;
      case EType::BDEM_TIMETZ: {
        return (other ? B25 : A25) == ref.theTimeTz();
      } break;
      default: {
        ASSERT("Invalid element passed to 'verifyDefaultValueForType'" && 0);
      } break;
    }

    return false;
}

bool isUnset(const CERef& ref)
    // Return 'true' if the specified element 'ref' references an element that
    // has the "unset" value for its type, and 'false' otherwise.
{
    return ref.descriptor()->isUnset(ref.data());
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

        int  fmt       = Format::BDEAT_DEFAULT;
        bool nullable  = EType::BDEM_LIST == fieldType ? false : true;
        int  dfltIndex = -1;
        int  id        = RecDef::BDEM_NULL_FIELD_ID;

        while ('&' == *p) {
            const char attrCode  = *++p;
            const char attrValue = *++p;
            switch (attrCode) {
              case 'D': {
                  ASSERT('0' <= attrValue && attrValue <= '1');
                  dfltIndex = attrValue - '0';
              } break;
              case 'F': {
                  fmt |= getFormattingMode(attrValue);
              } break;
              case 'I': {
                  ASSERT('0' <= attrValue && attrValue <= '9');
                  id = attrValue - '0';
              } break;
              case 'N': {
                  nullable = getNullability(attrValue, fieldType);
              } break;
              default: {
                  P(attrCode);
                  ASSERT("Invalid attribute code used in gg script" && 0);
              } break;
            }
            ++p;
        }

        FldAttr fldAttr(fieldType, nullable, fmt);
        if (-1 != dfltIndex) {
            ASSERT(! EType::isAggregateType(fieldType));
            setDefaultValueForType(fldAttr.defaultValue(), dfltIndex);
        }
        const FldDef *field = (enumConstraint)
            ? record->appendField(fldAttr, enumConstraint, fieldName, id)
            : record->appendField(fldAttr, recordConstraint, fieldName, id);
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

void addColor(EnumDef& enumeration, const char c, int i) {
    if (c == 'b') enumeration.addEnumerator("blue", i);
    if (c == 'g') enumeration.addEnumerator("green", i);
    if (c == 'r') enumeration.addEnumerator("red", i);
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

            RecDef *record = 0;
            EnumDef *enumdef = 0;
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

Obj g(const char *spec)
    // Return, by value, a new object corresponding to the specified spec.
{
    Obj object;
    return gg(&object, spec);
}

//=============================================================================
//    Pre-defined schema test values (as spec strings for 'gg')
//=============================================================================

static const struct {
    int         d_lineNum;    // source line number
    const char *d_spec;       // 'gg' spec
    int         d_bdexMinor;  // bdex minor version number (0, 1, or 2)
} testSpecs[] = {
//    line  'gg' spec                                 bdex minor version
//    ----  ---------                                 ------------------
    // An empty schema.
    { L_,   "~",                                                       0   },

    // One unnamed empty record.
    { L_,   "~ :*",                                                    0   },
    { L_,   "~ :*?",                                                   1   },

    // One empty record named "" (the empty string, which is a unique name).
    { L_,   "~ :e",                                                    0   },
    { L_,   "~ :e?",                                                   1   },

    // One record named "a", having one field of each unconstrained type.
    { L_,
      "~ :a Aa Bb Cc Dd Ee Ff Gg Hh Ii Jj K* L* M* N* O* P* Q* R* S* T* U* V*"
      "W* X* Y* Z* a* b* c* d* e* f*",
                                                                       0   },
    { L_,
      "~ :a? Aa Bb Cc Dd Ee Ff Gg Hh Ii Jj K* L* M* N* O* P* Q* R* S* T* U* V*"
      "W* X* Y* Z* a* b* c* d* e* f*",
                                                                       1   },

    // One record named "a", with self-referencing constrained types
    // using referencing by name and by number.
    { L_,   "~ :a +ba #ca +d0 #e0",                                    0   },
    { L_,   "~ :a? %ba @ca %d0 @e0",                                   1   },

    // A longer schema with a "representative" mix of records and fields.
    { L_,   "~ "
            ":* Aa "
            ":* "
            ":b Hh Ii Jj K* L* M* N* O* P* Q* R* "
            ":c +db #ec  "
            ":d S* T* U* V* Aa Bb Cc Dd Ee Ff Gg "
            ":* +b0 "
            ":* #ac ",                                                 0   },
    { L_,   "~ "
            ":*? Aa "
            ":* "
            ":b? Hh Ii Jj K* L* M* N* O* P*&D1&NF Q* R* "
            ":c? %db&I5 @ec&I3  "
            ":d S* T* U*&NT V* Aa&FX Bb Cc Dd Ee Ff Gg "
            ":* %b0 "
            ":f=tuv "
            ":g $af ^bf !cf /df "
            ":* @ac ",                                                 2   },

    // An ugly long schema.
    { L_,   "~ "
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
            ":j #aa +ba",                                              0   },
    { L_,   "~ "
            ":* Aa "
            ":* "
            ":b Hh Ii Jj K* L* M* N* Aa Bb Cc Dd Ee Ff Gg "
            ":c +bb #cc "
            ":* +*b Hh Ii Jj K* L* M*&D1 N* Aa Bb Cc Dd Ee Ff Gg +*b "
            ":* #*2 Ff Gg +*b "
            ":a A* Bb Cc Dd Ee Ff Gg +*b "
            ":d Bb Cc Dd Ee Ff Gg #*a "
            ":e Cc Dd Ee Ff Gg +*d "
            ":f Dd Ee Ff +ge "
            ":g Cg "
            ":h "
            ":i B* "
            ":j #aa +ba",                                              1   },

    // One unnamed, empty enumeration definition.
    { L_,   ":*=",                                                     2   },

    // One record containing one enumeration definition.
    { L_,   ":c=abc :a  $b0 Ad",                                       2   },
    { L_,   ":c=abc :a  ^b0 Ad",                                       2   },
    { L_,   ":c=abc :a? $b0 Ad",                                       2   },
    { L_,   ":c=abc :a? ^b0 Ad",                                       2   },
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

}  // close unnamed namespace

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

    bslma_TestAllocator  testAllocator(veryVeryVerbose);
    bslma_Allocator     *Z = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
      case 34: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  Suppress
        //   all 'bsl::cout' statements in non-verbose mode, and add streaming
        //   to a buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Usage Examples"
                               << "\n======================" << bsl::endl;

///Usage
///-----
// The following examples illustrate how to create and use a
// 'bdem_Schema'.  Note that using "ALL_CAPITALS' for record names, and
// "mixedCase" for field names, is a convention, and not required by
// 'bdem_schema'.
//

        if (verbose) bsl::cout << "\nUSAGE (Part 1)." << bsl::endl;
{

///Example 1: Creating a Trivial Schema
///- - - - - - - - - - - - - - - - - -
// In this example we create a simple schema having a single record
// definition that describes a record representing a sale.  A sale is a record
// containing an integer product id, a customer name, a floating point price,
// and a date.
//..
//  .---------------------------------------.
//  |{                                      |
//  |    RECORD "SALE" {                    |
//  |        INT           "productId";     |
//  |        STRING        "customerName";  |
//  |        DOUBLE        "price";         |
//  |        DATE          "date";          |
//  |    }                                  |
//  |}                                      |
//  `---------------------------------------'
//..
// We start by creating an empty schema:
//..
    bslma_Allocator *allocator = bslma_Default::allocator();
    bdem_Schema schema(allocator);
//..
// We create a new record definition "SALE" of type 'BDEM_SEQUENCE_RECORD',
// which is the default:
//..
    bdem_RecordDef *saleRecDef = schema.createRecord("SALE");
//..
// A record definition of type 'BDEM_SEQUENCE_RECORD' means that records
// created with that definition will be a *sequence* of all the defined
// fields (similar to a C++ 'struct'), rather than *choice* between them
// (similar to a C++ 'union').   Creating a *choice* record definition
// requires an explicit 'bdem_RecordDef::BDEM_CHOICE_RECORD' argument to the
// 'createRecord' method (see example 2).
//
// Next we append the fields to this record:
//..
    const bdem_FieldDef *productIdFldDef =
           saleRecDef->appendField(bdem_ElemType::BDEM_INT,    "productId");
    const bdem_FieldDef *customerNameFldDef =
           saleRecDef->appendField(bdem_ElemType::BDEM_STRING, "customerName");
    const bdem_FieldDef *priceFldDef =
           saleRecDef->appendField(bdem_ElemType::BDEM_DOUBLE, "price");
    const bdem_FieldDef *dateFldDef =
           saleRecDef->appendField(bdem_ElemType::BDEM_DATE,   "date");
//..
// We verify the properties of a single field.  By default, a field will not
// be nullable, it will not have a default value, and its formatting
// mode will be 'bdeat_FormattingMode::BDEAT_DEFAULT':
//..
    ASSERT(0 == bsl::strcmp("date", dateFldDef->fieldName()));
    ASSERT(bdem_ElemType::BDEM_DATE == dateFldDef->elemType());
    ASSERT(false                    == dateFldDef->hasDefaultValue());
    ASSERT(false                    == dateFldDef->isNullable());
    ASSERT(bdeat_FormattingMode::BDEAT_DEFAULT ==
                                       dateFldDef->formattingMode());
//..
// We can write this schema to the console:
//..
if (veryVerbose)
    schema.print(bsl::cout, 1, 3);
//..
// The resulting output will be:
//..
//  {
//     SEQUENCE RECORD "SALE" {
//        INT "productId" { !nullable 0x0 }
//        STRING "customerName" { !nullable 0x0 }
//        DOUBLE "price" { !nullable 0x0 }
//        DATE "date" { !nullable 0x0 }
//     }
//  }
//..

}
        if (verbose) bsl::cout << "\nUSAGE (Part 2)." << bsl::endl;
{

//
///Example 2: Using Choices, Constraints, and Default Values
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we create a more complex schema for describing a sale.  The
// schema, illustrated below, supports sales performed using either a check, a
// credit card, or cash:
//..
//  .---------------------------------------------------------.
//  |{                                                        |
//  |    ENUMERATION "CREDIT_CARD_TYPE" {                     |
//  |        "MASTERCARD"                                     |
//  |        "VISA"                                           |
//  |        "AMEX"                                           |
//  |    }                                                    |
//  |                                                         |
//  |    RECORD "CREDIT_CARD_PAYMENT" {                       |
//  |        STRING<"CREDIT_CARD_TYPE">  "cardType";          |
//  |        STRING                      "name";              |
//  |        INT                         "cardNumber";        |
//  |    }                                                    |
//  |                                                         |
//  |    RECORD "CHECK_PAYMENT" {                             |
//  |        STRING  "bankName";                              |
//  |        INT     "routingNumber";                         |
//  |    }                                                    |
//  |                                                         |
//  |    CHOICE RECORD "PAYMENT_INFO" {                       |
//  |        LIST<"CREDIT_CARD_PAYMENT"> "creditCardPayment"; |
//  |        LIST<"CHECK_PAYMENT">       "checkPayment";      |
//  |        BOOL                        "cashPayment";       |
//  |    }                                                    |
//  |                                                         |
//  |    RECORD "SALE" {                                      |
//  |        INT                    "productId";              |
//  |        INT                    "quantity" [default: 1];  |
//  |        STRING                 "customerName";           |
//  |        DOUBLE                 "price"                   |
//  |        BDET_DATE              "date";                   |
//  |        CHOICE<"PAYMENT_INFO"> "paymentInfo";            |
//  |    }                                                    |
//  |}                                                        |
//  `---------------------------------------------------------'
//..
// The selection among three different payment options (check, credit-card,
// and cash) is described by the choice record "PAYMENT_INFO".  Choice record
// definitions (i.e., record definitions of type 'BDEM_CHOICE') indicate a
// selection from among its contained fields.  A choice record definition can
// be used to constrain fields of type 'CHOICE' and 'CHOICE_ARRAY'.  The choice
// record definition "PAYMENT_INFO" is used to constraint the choice field,
// "paymentInfo", in the "SALE" record definition.
//
// By default, record definitions are of type 'BDEM_SEQUENCE_RECORD'.
// A *sequence* record-definition indicates that the record value is a
// *sequence* of the indicated field values (rather than a *choice* among
// field values).  Sequence record-definitions can be used to constraint fields
// definitions of type 'LIST' and 'TABLE'.  In this example, the *sequence*
// record-definitions "CREDIT_CARD_PAYMENT" and "CHECK_PAYMENT" are used to
// constraint the values in the lists "creditCardPayment" and "cashPayment"
// in the "PAYMENT_INFO' *choice* record-definition.
//
// Finally, note that "SALE" has a "quantity" whose default value is 1.
//
// Here are a couple hypothetical record values that this schema might
// describe:
//..
//  .---------------------------------------.
//  | {                                     |
//  |     productId:    386                 |
//  |     quantity:     4                   |
//  |     customerName: Jeff                |
//  |     price:        29.99               |
//  |     date:         April 22, 2010      |
//  |     paymentInfo:  {                   |
//  |         cashPayment: { true }         |
//  |     }                                 |
//  | }                                     |
//  | {                                     |
//  |     productId:    486                 |
//  |     customerName: Sally               |
//  |     price:        48.50               |
//  |     date:         May 1, 2010         |
//  |     paymentInfo:  {                   |
//  |         creditCardPayment:  {         |
//  |             cardType:   "VISA"        |
//  |             name:       Sally Jenkins |
//  |             cardNumber: 1111222223333 |
//  |         }                             |
//  |     }                                 |
//  | }                                     |
//  `---------------------------------------'
//..
// Notice that the second record does not specify 'quantity', so it is
// interpreted as having the schema's default 'quantity' value of 1.
//
//Creating a Schema:
//- - - - - - - - -
// We now examine how to create a 'bdem_Schema' for the schema described in
// the previous section.  We start by creating a 'bdem_Schema' object, and
// adding an enumeration named "CREDIT_CARD_TYPE":
//..
    bslma_Allocator *allocator = bslma_Default::allocator();
    bdem_Schema schema(allocator);

    bdem_EnumerationDef *ccTypeEnumDef =
                                 schema.createEnumeration("CREDIT_CARD_TYPE");
//..
// Next we add the individual of enumerator values, which by default take on
// sequential integer values starting with 0:
//..
    int id1 = ccTypeEnumDef->addEnumerator("MASTERCARD");
    int id2 = ccTypeEnumDef->addEnumerator("VISA");
    int id3 = ccTypeEnumDef->addEnumerator("AMEX");
    ASSERT(0 == id1);  ASSERT(1 == id2);  ASSERT(2 == id3);
//..
// Then we create the *sequential* record definition "CREDIT_CARD_PAYMENT",
// and append its fields.  When we add the field definition "cardType", we
// supply the enumeration definition 'ccTypeEnumDef' as an enumeration
// constraint for the values of that field:
//..
    bdem_RecordDef *ccPaymentRecDef = schema.createRecord(
                                                        "CREDIT_CARD_PAYMENT");
    ccPaymentRecDef->appendField(bdem_ElemType::BDEM_STRING,
                                 ccTypeEnumDef,
                                 "cardType");
    ccPaymentRecDef->appendField(bdem_ElemType::BDEM_STRING,
                                 "name");
    ccPaymentRecDef->appendField(bdem_ElemType::BDEM_INT,
                                 "cardNumber");
//..
// Next we create the *sequential* record definition for "CHECK_PAYMENT", and
// append its fields:
//..
    bdem_RecordDef *checkPaymentRecDef = schema.createRecord("CHECK_PAYMENT");
    checkPaymentRecDef->appendField(bdem_ElemType::BDEM_STRING,
                                    "bankName");
    checkPaymentRecDef->appendField(bdem_ElemType::BDEM_INT,
                                    "routingNumber");
//..
// Then we create the *choice* record definition for "PAYMENT_INFO".  We
// indicate that the new record definition should be a choice by supplying
// 'bdem_RecordDef::BDEM_CHOICE_RECORD' to 'createRecord':
//..
    bdem_RecordDef *paymentInfoRecDef = schema.createRecord(
                                           "PAYMENT_INFO",
                                           bdem_RecordDef::BDEM_CHOICE_RECORD);
//..
// Next we append the fields to the "PAYMENT_INFO" record.  When we add the
// field definition "creditCardPayment" we provide the record definition
// 'ccPaymentRecDef', as a record constraint for the values of that field.
// Similarly, we provide the record definition 'checkPaymentRecDef' as a
// constraint for the values of the "checkPayment" field:
//..
    paymentInfoRecDef->appendField(bdem_ElemType::BDEM_LIST,
                                   ccPaymentRecDef,
                                   "creditCardPayment");
    paymentInfoRecDef->appendField(bdem_ElemType::BDEM_LIST,
                                   checkPaymentRecDef,
                                   "checkPayment");
    paymentInfoRecDef->appendField(bdem_ElemType::BDEM_BOOL,
                                   "cashPayment");
//..
// Then we create a record for "SALE", and append its first field "productId":
//..
    bdem_RecordDef *saleRecDef = schema.createRecord("SALE");
    saleRecDef->appendField(bdem_ElemType::BDEM_INT, "productId");
//..
// For the "quantity" field, we create a 'bdem_FieldDefAttributes' so we can
// provide it a default value of 1.  We then append the "quantity" field, and
// the remaining fields to the "SALE" record definition:
//..
    bdem_FieldDefAttributes quantityType(bdem_ElemType::BDEM_INT);
    quantityType.defaultValue().theModifiableInt() = 1;

    saleRecDef->appendField(quantityType,               "quantity");
    saleRecDef->appendField(bdem_ElemType::BDEM_INT,    "productId");
    saleRecDef->appendField(bdem_ElemType::BDEM_STRING, "customerName");
    saleRecDef->appendField(bdem_ElemType::BDEM_DOUBLE, "price");
    saleRecDef->appendField(bdem_ElemType::BDEM_DATE,   "date");
    saleRecDef->appendField(bdem_ElemType::BDEM_LIST,
                            paymentInfoRecDef,
                            "paymentInfo");
//..
// Finally, we can write a description of the created schema to the console:
//..
if (veryVerbose)
    schema.print(bsl::cout, 1, 3);
//..
// The resulting console output looks like:
//..
//  {
//     SEQUENCE RECORD "CREDIT_CARD_PAYMENT" {
//        STRING ENUM<"CREDIT_CARD_TYPE"> "cardType" { !nullable 0x0 }
//        STRING "name" { !nullable 0x0 }
//        INT "cardNumber" { !nullable 0x0 }
//     }
//     SEQUENCE RECORD "CHECK_PAYMENT" {
//        STRING "bankName" { !nullable 0x0 }
//        INT "routingNumber" { !nullable 0x0 }
//     }
//     CHOICE RECORD "PAYMENT_INFO" {
//        LIST<"CREDIT_CARD_PAYMENT"> "creditCardPayment" { !nullable 0x0 }
//        LIST<"CHECK_CARD_PAYMENT"> "checkPayment" { !nullable 0x0 }
//        BOOL "cashPayment" { !nullable 0x0 }
//     }
//     SEQUENCE RECORD "SALE" {
//        INT "productId" { !nullable 0x0 }
//        INT "quantity" { !nullable 0x0 1 }
//        STRING "customerName" { !nullable 0x0 }
//        DOUBLE "price" { !nullable 0x0 }
//        DATE "date" { !nullable 0x0 }
//     }
//     ENUMERATION "CREDIT_CARD_TYPE" {
//        "MASTERCARD" = 0
//        "VISA" = 1
//        "AMEX" = 2
//     }
//  }
//..

}
        if (verbose) bsl::cout << "\nUSAGE (Part 3)." << bsl::endl;
{

///Example 3: Creating a Recursive Schema
/// - - - - - - - - - - - - - - - - - - -
// Fields of type 'LIST', 'TABLE', 'CHOICE', and 'CHOICE_ARRAY' are known as
// aggregate fields because they contain (or aggregate) a sequence of
// sub-fields (see 'bdem_ElemType::isAggregateType').  Field definitions of
// aggregate field types may be constrained by another record definition,
// indicating a constraint on the values of the sub-fields of the aggregate
// field.  It is possible for such a constraint to be recursive, i.e., a
// record definition, "A", contains an field definition for an aggregate field
// that is constrained by record "A".  Such a recursive schema allows the
// construction of recursive data structures such as lists and trees.
//
// In this example we illustrates how to construct a schema for a binary tree.
// The finished schema will have the following textual representation of
// its structure.
//..
//  .------------------------------.
//  |{                             |
//  |    RECORD "TREE" {           |
//  |        STRING       "key";   |
//  |        LIST<"TREE"> "left";  |
//  |        LIST<"TREE"> "right"; |
//  |    }                         |
//  |}                             |
//  `------------------------------'
//..
// First we create an empty 'bdem_Schema' object, 'schema':
//..
    bdem_Schema schema;
//..
// Next we append a record definition named "TREE" to 'schema':
//..
    bdem_RecordDef *treeRecDef = schema.createRecord("TREE");
//..
// Then we append to "TREE" a field definition of type 'STRING' named "key",
// followed by two field definitions, each of type 'LIST', respectively named
// "left" and "right", and each constrained by the record definition "TREE":
//..
    treeRecDef->appendField(bdem_ElemType::BDEM_STRING, "key");
    treeRecDef->appendField(bdem_ElemType::BDEM_LIST, treeRecDef, "left");
    treeRecDef->appendField(bdem_ElemType::BDEM_LIST, treeRecDef, "right");
//..
// It's also possible to create records with mutually recursive constraints --
// e.g., a schema describing a graph, consisting of mutually referential nodes
// and edges.  Note that both self-referential and mutually self-referential
// constraints are viable only if underlying data structures support null
// fields.

}

      } break;
      case 33: {
        // --------------------------------------------------------------------
        // TESTING CASES WHERE DEFAULT VALUE IS VALID BUT UNSET
        //
        // Concerns:
        //   For testing working code with null values, it is most important
        //   to test the case where a default value is valid but unset.
        //
        // Plan:
        //   Create a record, verify that we can add fields to it with unset
        //   but not null default values.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting valid but unset default values\n"
                                    "======================================\n";

        const double unsetBool   = bdetu_Unset<bool>::unsetValue();
        const double unsetInt    = bdetu_Unset<int>::unsetValue();
        const double unsetDouble = bdetu_Unset<double>::unsetValue();

        {
            Schema mS(Z);

            RecDef *mR = mS.createRecord("Rec"); const RecDef& R = *mR;
            ASSERT(mR);

            {
                FldAttr fs(EType::BDEM_BOOL);
                ASSERT(EType::BDEM_BOOL == fs.defaultValue().type());
                fs.defaultValue().theModifiableBool() = unsetBool;
                mR->appendField(fs);
            }
            {
                FldAttr fs(EType::BDEM_INT);
                ASSERT(EType::BDEM_INT == fs.defaultValue().type());
                fs.defaultValue().theModifiableInt() = unsetInt;
                mR->appendField(fs);
            }
            {
                FldAttr fs(EType::BDEM_DOUBLE);
                ASSERT(EType::BDEM_DOUBLE == fs.defaultValue().type());
                fs.defaultValue().theModifiableDouble() = unsetDouble;
                mR->appendField(fs);
            }
            {
                FldAttr fs(EType::BDEM_BOOL_ARRAY);
                ASSERT(EType::BDEM_BOOL == fs.defaultValue().type());
                fs.defaultValue().theModifiableBool() = unsetBool;
                mR->appendField(fs);
            }
            {
                FldAttr fs(EType::BDEM_INT_ARRAY);
                ASSERT(EType::BDEM_INT == fs.defaultValue().type());
                fs.defaultValue().theModifiableInt() = unsetInt;
                mR->appendField(fs);
            }
            {
                FldAttr fs(EType::BDEM_DOUBLE_ARRAY);
                ASSERT(EType::BDEM_DOUBLE == fs.defaultValue().type());
                fs.defaultValue().theModifiableDouble() = unsetDouble;
                mR->appendField(fs);
            }

            for (int i = 0; i < 6; ++i) {
                ASSERT(  R.field(i).hasDefaultValue());
                ASSERT(  isUnset(R.field(i).defaultValue()));
                ASSERT(! R.field(i).defaultValue().isNull());
                if (i < 3) {
                    ASSERT(EType::isScalarType(R.field(i).elemType()));
                    ASSERT(R.field(i).elemType() ==
                                             R.field(i).defaultValue().type());
                }
                else {
                    ASSERT(EType::isArrayType(R.field(i).elemType()));
                    EType::Type scalarType = EType::fromArrayType(
                                                        R.field(i).elemType());
                    ASSERT(R.field(i).defaultValue().type() == scalarType);
                }
            }
        }

        for (int i = 0; i < NUM_TYPES; ++i) {
            EType::Type type = static_cast<EType::Type>(i);

            if (EType::isAggregateType(type)) {
                continue;
            }

            Schema mS(Z);

            RecDef *mR = mS.createRecord("Rec"); const RecDef& R = *mR;
            ASSERT(mR);

            FldAttr fs(type);
            makeDefaultValueUnset(fs.defaultValue());
            const FldDef *fld = mR->appendField(fs);

            if (EType::isScalarType(type)) {

                ASSERT(type == R.field(0).elemType());
                ASSERT(fld->defaultValue().type() == type);

                ASSERT(  fld->hasDefaultValue());
                ASSERT(  isUnset(fld->defaultValue()));
                ASSERT(! fld->defaultValue().isNull());
            }
            else {
                ASSERT(EType::isArrayType(type));

                ASSERT(type == fld->elemType());
                EType::Type scalarType = EType::fromArrayType(type);
                ASSERT(fld->defaultValue().type() == scalarType);
                ASSERT(fld->defaultValue().type() != type);

                ASSERT(  fld->hasDefaultValue());
                ASSERT(  isUnset(fld->defaultValue()));
                ASSERT(! fld->defaultValue().isNull());
            }
        }
      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING EXTENDED FIELD LOOKUP
        //
        // Concerns:
        //   1 'fieldIndexExtended' returns the same as 'fieldIndex' if the
        //     record definition contains no unnamed fields or if the name is
        //     found at the top level.
        //   2 'fieldIndexExtended' returns -1 if the record definition
        //     contains one or more unnamed fields that don't have the
        //     specified name.
        //   3 If the record definition contains an unnamed field containing
        //     the specified name, then 'fieldIndexExtended' returns the index
        //     of that field.
        //   4 If the record definition contains an unnamed field containing
        //     a nested unnamed field containing the specified field, then
        //     'fieldIndexExtended' returns the index of the first-level
        //     unnamed field.
        //   5 Unnamed fields of non-aggregate type are treated as normal
        //     fields.
        //   6 All of the above apply when there exist multiple unnamed fields
        //     at any level and when the field is in the first, middle, or last
        //     unnamed field.
        //   7 All of the above apply when the unnamed field is a LIST, TABLE,
        //     CHOICE, or CHOICE_ARRAY.
        //   8 All of the above apply when the found field is of scalar, array,
        //     or aggregate type.
        //   9 'numAnonymousFields' returns the number of unnamed fields
        //     in the record.
        //
        // Plan:
        //   - Construct a set of test vectors where each vector consists of a
        //     schema script and the index where the unnamed field should be
        //     found.
        //   - Each schema will have a top-level record definition named "a"
        //     and a the program will look for a field named "b" in "a".
        //   - For each test vector, construct the bdem_Schema.
        //   - Call fieldIndexExtended("A") and confirm that the return value
        //     matches the the expected index.
        //   - Call lookupFieldExtended("A") and confirm that the return value
        //     matches the field at the expected index.
        //   - Call 'fieldName' on each field in the record and count the ones
        //     for which it returns 0.  Confirm that 'numAnonymousFields'
        //     returns a value matching the computed count of unnamed fields.
        //
        // Testing:
        //    int fieldIndexExtended(const char *name) const;
        //    const bdem_FieldDef *lookupFieldExtended(const char *name) const;
        //    int numAnonymousFields() const;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING EXTENDED FIELD LOOKUP"
                               << "\n=============================\n"
                               << bsl::endl;

        static const struct {
            int         d_line;
            const char *d_spec;
            int         d_expectedIdx; // -1 for failure
        } DATA[] = {

            //line Spec                                         Index  Concerns
            //---- -------------------------------------------- -----  --------
            { L_, ":a",                                            -1 }, // 1
            { L_, ":aGc",                                          -1 }, // 1
            { L_, ":aCb",                                           0 }, // 1
            { L_, ":aGcCb",                                         1 }, // 1

            { L_, ":*Gc :a+*0&FU",                                 -1 }, // 2
            { L_, ":*FaGcMd :a+*0&FU",                             -1 }, // 2
            { L_, ":*Gc :aFa+*0&FUMd",                             -1 }, // 2
            { L_, ":*GcXe :aFa+*0&FUMd",                           -1 }, // 2

            { L_, ":*Cb :a+*0&FU",                                  0 }, // 3
            { L_, ":*Cb :a+*0FaMd",                                 0 }, // 3
            { L_, ":*Cb :aFa+*0&FUMd",                              1 }, // 3
            { L_, ":*Cb :aFaMd+*0&FU",                              2 }, // 3
            { L_, ":*FaCbMd :a+*0&FU",                              0 }, // 3
            { L_, ":*CbXe :aFa+*0&FUMd",                            1 }, // 3
            { L_, ":*XeCb :aFa+*0&FUMd",                            1 }, // 3

            { L_, ":*Cb :*#*0&FU :a+*1&FU",                         0 }, // 4
            { L_, ":*FaCbMd :*#*0&FU :a+*1&FU",                     0 }, // 4
            { L_, ":*Cb :*#*0&FU :aFa+*1&FUMd",                     1 }, // 4
            { L_, ":*Cb :*Fa#*0&FUMd :a+*1&FU",                     0 }, // 4
            { L_, ":*Gc :*Fa#*0&FUCb :a+*1&FU",                     0 }, // 4

            { L_, ":aCb&FU",                                        0 }, // 5
            { L_, ":*Cb :aG*&FU+*0&FU",                             1 }, // 5
            { L_, ":*Cb :aG*&FU+*0&FUDb",                           2 }, // 5
            { L_, ":*Cb :aCb&FU+*0&FU",                             0 }, // 5

            { L_, ":*Cg :*?Gc :*?Ad :a+*0&FUBf%*1&FU@*2&FU",       -1 }, // 2,6

            { L_, ":*Cb :*?Gc :*?Ad :a+*0&FUBf%*1&FU@*2&FU",        0 }, // 3,6
            { L_, ":*Cc :*?Gb :*?Ad :a+*0&FUBf%*1&FU@*2&FU",        2 }, // 3,6
            { L_, ":*Cd :*?Gc :*?Ab :a+*0&FUBf%*1&FU@*2&FU",        3 }, // 3,6

            { L_, ":*Cb :*?Gc :*?Ad :*+*0&FUBf%*1&FU@*2&FU :a#*3&FU",  0 },
                                                                         // 4,6
            { L_, ":*Cc :*?Gb :*?Ad :*+*0&FUBf%*1&FU@*2&FU :a#*3&FU",  0 },
                                                                         // 4,6
            { L_, ":*Cd :*?Gc :*?Ab :*+*0&FUBf%*1&FU@*2&FU :a#*3&FU",  0 },
                                                                         // 4,6
            { L_, ":*Cg :*?Gc :*?Ad :*+*0&FUBf%*1&FU@*2&FU :a#*3&FU", -1 },
                                                                       // 2,4,6

            { L_, ":*Gc :a#*0&FU",                                 -1 }, // 2,7
            { L_, ":*?Gc :a%*0&FU",                                -1 }, // 2,7
            { L_, ":*?Gc :a@*0&FU",                                -1 }, // 2,7
            { L_, ":*Cb :aFa#*0&FUMd",                              1 }, // 3,7
            { L_, ":*?Cb :aFa%*0&FUMd",                             1 }, // 3,7
            { L_, ":*?Cb :aFa@*0&FUMd",                             1 }, // 3,7
            { L_, ":*Cb :*+*0&FU :a+*1&FU",                         0 }, // 4,7
            { L_, ":*Cb :*#*0&FU :a+*1&FU",                         0 }, // 4,7
            { L_, ":*?Cb :*%*0&FU :a+*1&FU",                        0 }, // 4,7
            { L_, ":*?Cb :*@*0&FU :a+*1&FU",                        0 }, // 4,7
            { L_, ":*Cb :*+*0&FU :a#*1&FU",                         0 }, // 4,7
            { L_, ":*Cb :*?+*0&FU :a%*1&FU",                        0 }, // 4,7
            { L_, ":*Cb :*?+*0&FU :a@*1&FU",                        0 }, // 4,7
            { L_, ":*?Cb :*?@*0&FU :a@*1&FU",                       0 }, // 4,7
            { L_, ":*Cb :aG*&FU#*0&FU",                             1 }, // 5,7
            { L_, ":*?Cb :aG*&FU%*0&FU",                            1 }, // 5,7
            { L_, ":*?Cb :aG*&FU@*0&FU",                            1 }, // 5,7

            { L_, ":*XeSb :aFa+*0&FUMd",                            1 }, // 3,8
            { L_, ":g :*Xe+bg :aFa+*1&FUMd",                        1 }, // 3,8
            { L_, ":g? :*Xe@bg :aFa+*1&FUMd",                       1 }, // 3,8
            { L_, ":*FaQbMd :*#*0&FU :a+*1&FU",                     0 }, // 4,8
            { L_, ":g :*Fa#bgMd :*#*1&FU :a+*2&FU",                 0 }, // 4,8
            { L_, ":g? :*Fa%bgMd :*#*1&FU :a+*2&FU",                0 }, // 4,8
            { L_, ":aSb&FU",                                        0 }, // 5,8
        };

        static const int DATA_SIZE = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < DATA_SIZE; ++i) {
            const int         LINE         = DATA[i].d_line;
            const char *const SPEC         = DATA[i].d_spec;
            const int         EXPECTED_IDX = DATA[i].d_expectedIdx;

            bdem_Schema schema;
            gg(&schema, SPEC);

            if (veryVerbose) { P_(LINE) P(schema) }

            const bdem_RecordDef *recA = schema.lookupRecord("a");
            ASSERT(0 != recA);

            const bdem_FieldDef *EXPECTED_FLD = (EXPECTED_IDX < 0 ? 0 :
                                                 &recA->field(EXPECTED_IDX));

            // Count number of anonymous fields at top level.
            int numAnonymous = 0;
            for (int i = 0; i < recA->numFields(); ++i) {
                if (0 == recA->fieldName(i)) {
                    ++numAnonymous;
                }
            }

            ASSERT(recA->numAnonymousFields() == numAnonymous);

            int idxB = recA->fieldIndexExtended("b");

            if (veryVerbose) { P_(EXPECTED_IDX) P(idxB) }

            LOOP4_ASSERT(LINE, schema, EXPECTED_IDX, idxB,
                         EXPECTED_IDX == idxB);
        }

      } break;
      case 31: {
        // --------------------------------------------------------------------
        // TESTING ISSUPERSET
        //
        // Concerns:
        //    - That 'isSuperset' returns true, if for each enumerator in 'sub'
        //      there exists an enumerator in 'super' with the same name and
        //      ID), and false otherwise.
        //    - That 'maxId()', 'enumerationIndex()' and this definition's
        //      name are not considered in this comparison.
        //
        // Plan:
        //    Create different combinations of enumerations ('super' and
        //    'sub') and test 'isSuperset' on them to confirm correct behavior.
        //    Also test that 'maxId' 'enumerationIndex()' and this
        //    definition's name are not considered in the equivalent
        //    comparison.
        //
        // Testing:
        //   isSuperset(const bdem_EnumerationDef& super,
        //              const bdem_EnumerationDef& sub);
        // --------------------------------------------------------------------
        const struct {
          int  d_line;
          char d_subColor0;
          char d_subColor1;
          char d_subColor2;
          char d_superColor0;
          char d_superColor1;
          char d_superColor2;
          bool d_isSuper;
        } DATA[] = {
        //             sub                 super
        //     -------------------- --------------------
        //LINE color1 color2 color3 color1 color2 color3 isSuper
          // LEVEL 0
          { L_, ' ',    ' ',   ' ',   ' ',   ' ',   ' ',  true  },

          // LEVEL 1
          { L_, ' ',    ' ',   ' ',   'b',   ' ',   ' ',  true  },
          { L_, ' ',    ' ',   ' ',   ' ',   'b',   ' ',  true  },
          { L_, ' ',    ' ',   ' ',   ' ',   ' ',   'b',  true  },

          { L_, 'b',    ' ',   ' ',   ' ',   ' ',   ' ',  false },
          { L_, ' ',    'b',   ' ',   ' ',   ' ',   ' ',  false },
          { L_, ' ',    ' ',   'b',   ' ',   ' ',   ' ',  false },

          // LEVEL 2
          { L_, 'b',    ' ',   ' ',   'b',   ' ',   ' ',  true  },
          { L_, 'b',    ' ',   ' ',   'g',   ' ',   ' ',  false },
          { L_, ' ',    'b',   ' ',   'b',   ' ',   ' ',  false },
          { L_, ' ',    'b',   ' ',   'g',   ' ',   ' ',  false },
          { L_, ' ',    ' ',   'b',   'b',   ' ',   ' ',  false },
          { L_, ' ',    ' ',   'b',   'g',   ' ',   ' ',  false },
          { L_, 'b',    ' ',   ' ',   ' ',   'b',   ' ',  false },
          { L_, 'b',    ' ',   ' ',   ' ',   'g',   ' ',  false },
          { L_, ' ',    'b',   ' ',   ' ',   'b',   ' ',  true  },
          { L_, ' ',    'b',   ' ',   ' ',   'g',   ' ',  false },
          { L_, ' ',    ' ',   'b',   ' ',   'b',   ' ',  false },
          { L_, ' ',    ' ',   'b',   ' ',   'g',   ' ',  false },
          { L_, 'b',    ' ',   ' ',   ' ',   ' ',   'b',  false },
          { L_, 'b',    ' ',   ' ',   ' ',   ' ',   'g',  false },
          { L_, ' ',    'b',   ' ',   ' ',   ' ',   'b',  false },
          { L_, ' ',    'b',   ' ',   ' ',   ' ',   'g',  false },
          { L_, ' ',    ' ',   'b',   ' ',   ' ',   'b',  true  },
          { L_, ' ',    ' ',   'b',   ' ',   ' ',   'g',  false },
          { L_, ' ',    ' ',   ' ',   'b',   'g',   ' ',  true  },
          { L_, 'b',    'g',   ' ',   ' ',   ' ',   ' ',  false },

          // LEVEL 3
          { L_, 'b',    ' ',   ' ',   'b',   'g',   ' ',  true  },
          { L_, 'b',    ' ',   ' ',   'g',   'b',   ' ',  false },
          { L_, 'b',    ' ',   ' ',   'g',   'r',   ' ',  false },
          { L_, ' ',    'b',   ' ',   'b',   'g',   ' ',  false },
          { L_, ' ',    'b',   ' ',   'g',   'b',   ' ',  true  },
          { L_, ' ',    'b',   ' ',   'g',   'r',   ' ',  false },
          { L_, ' ',    ' ',   'b',   'b',   'g',   ' ',  false },
          { L_, ' ',    ' ',   'b',   'g',   'b',   ' ',  false },
          { L_, ' ',    ' ',   'b',   'g',   'r',   ' ',  false },
          { L_, ' ',    ' ',   ' ',   'b',   'g',   'r',  true  },
          { L_, 'b',    'g',   'r',   ' ',   ' ',   ' ',  false },

          { L_, 'b',    ' ',   ' ',   ' ',   'b',   'g',  false },
          { L_, 'b',    ' ',   ' ',   ' ',   'g',   'b',  false },
          { L_, 'b',    ' ',   ' ',   ' ',   'g',   'r',  false },
          { L_, ' ',    'b',   ' ',   ' ',   'b',   'g',  true  },
          { L_, ' ',    'b',   ' ',   ' ',   'g',   'b',  false },
          { L_, ' ',    'b',   ' ',   ' ',   'g',   'r',  false },
          { L_, ' ',    ' ',   'b',   ' ',   'b',   'g',  false },
          { L_, ' ',    ' ',   'b',   ' ',   'g',   'b',  true  },
          { L_, ' ',    ' ',   'b',   ' ',   'g',   'r',  false },

          { L_, 'b',    'g',   ' ',   'b',   ' ',   ' ',  false },
          { L_, ' ',    'b',   'g',   'b',   ' ',   ' ',  false },

          // LEVEL 4
          { L_, 'b',    ' ',   ' ',   'b',   'g',   'r',  true  },
          { L_, 'b',    ' ',   ' ',   'r',   'b',   'g',  false },
          { L_, 'b',    ' ',   ' ',   'r',   'g',   'b',  false },
          { L_, ' ',    'b',   ' ',   'b',   'g',   'r',  false },
          { L_, ' ',    'b',   ' ',   'r',   'b',   'g',  true  },
          { L_, ' ',    'b',   ' ',   'r',   'g',   'b',  false },
          { L_, ' ',    ' ',   'b',   'b',   'g',   'r',  false },
          { L_, ' ',    ' ',   'b',   'r',   'b',   'g',  false },
          { L_, ' ',    ' ',   'b',   'r',   'g',   'b',  true  },

          { L_, 'b',    'g',   ' ',   'b',   'g',   ' ',  true  },
          { L_, 'b',    'g',   ' ',   'g',   'b',   ' ',  false },
          { L_, 'b',    'g',   ' ',   'b',   'r',   ' ',  false },
          { L_, 'b',    'g',   ' ',   'r',   'b',   ' ',  false },
          { L_, 'b',    'g',   ' ',   'g',   'r',   ' ',  false },
          { L_, 'b',    'g',   ' ',   'r',   'g',   ' ',  false },
          { L_, ' ',    'b',   'g',   'b',   'g',   ' ',  false },
          { L_, ' ',    'b',   'g',   'g',   'b',   ' ',  false },
          { L_, ' ',    'b',   'g',   'b',   'r',   ' ',  false },
          { L_, ' ',    'b',   'g',   'r',   'b',   ' ',  false },
          { L_, ' ',    'b',   'g',   'g',   'r',   ' ',  false },
          { L_, ' ',    'b',   'g',   'r',   'g',   ' ',  false },

          { L_, 'b',    'g',   'r',   'b',   ' ',   ' ',  false },
          { L_, 'b',    'g',   'r',   'g',   ' ',   ' ',  false },
          { L_, 'b',    'g',   'r',   'r',   ' ',   ' ',  false },

          // LEVEL 5
          { L_, 'b',    'g',   ' ',   'b',   'g',   'r',  true  },
          { L_, 'b',    'g',   ' ',   'b',   'r',   'g',  false },
          { L_, 'b',    'g',   ' ',   'r',   'b',   'g',  false },
          { L_, 'b',    'g',   ' ',   'r',   'g',   'b',  false },
          { L_, 'b',    'g',   ' ',   'g',   'r',   'b',  false },
          { L_, 'b',    'g',   ' ',   'g',   'b',   'r',  false },
          { L_, ' ',    'b',   'g',   'b',   'g',   'r',  false },
          { L_, ' ',    'b',   'g',   'b',   'r',   'g',  false },
          { L_, ' ',    'b',   'g',   'r',   'b',   'g',  true  },
          { L_, ' ',    'b',   'g',   'r',   'g',   'b',  false },
          { L_, ' ',    'b',   'g',   'g',   'r',   'b',  false },
          { L_, ' ',    'b',   'g',   'g',   'b',   'r',  false },

          { L_, 'b',    'g',   'r',   'b',   'g',   ' ',  false },
          { L_, 'b',    'g',   'r',   'g',   'b',   ' ',  false },
          { L_, 'b',    'g',   'r',   'b',   'r',   ' ',  false },
          { L_, 'b',    'g',   'r',   'r',   'b',   ' ',  false },
          { L_, 'b',    'g',   'r',   'g',   'r',   ' ',  false },
          { L_, 'b',    'g',   'r',   'r',   'g',   ' ',  false },

          // LEVEL 6
          { L_, 'b',    'g',   'r',   'b',   'g',   'r',  true  },
          { L_, 'b',    'g',   'r',   'b',   'r',   'g',  false },
          { L_, 'b',    'g',   'r',   'r',   'b',   'g',  false },
          { L_, 'b',    'g',   'r',   'r',   'g',   'b',  false },
          { L_, 'b',    'g',   'r',   'g',   'r',   'b',  false },
          { L_, 'b',    'g',   'r',   'g',   'b',   'r',  false }
        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        if (verbose) bsl::cout << "\nTesting isSuperset:"
                               << "\n===================" << bsl::endl;

        for (int ii = 0; ii < NUM_DATA; ++ii) {
          const int  LINE  = DATA[ii].d_line;
          const char SUBC0 = DATA[ii].d_subColor0;
          const char SUBC1 = DATA[ii].d_subColor1;
          const char SUBC2 = DATA[ii].d_subColor2;
          const char SUPERC0 = DATA[ii].d_superColor0;
          const char SUPERC1 = DATA[ii].d_superColor1;
          const char SUPERC2 = DATA[ii].d_superColor2;
          const bool ISSUPER = DATA[ii].d_isSuper;

          Obj x1(Z);  const Obj& X1 = x1;
          ASSERT(0  == X1.numRecords());
          ASSERT(0  == X1.numEnumerations());
          ASSERT(X1 == X1);
          ASSERT(!(X1 != X1));

          EnumDef *superColors = x1.createEnumeration("COLORS");
          addColor(*superColors, SUPERC0, 0);
          addColor(*superColors, SUPERC1, 1);
          addColor(*superColors, SUPERC2, 2);

          Obj y1(Z);  const Obj& Y1 = y1;
          ASSERT(0  == Y1.numRecords());
          ASSERT(0  == Y1.numEnumerations());
          ASSERT(Y1 == Y1);
          ASSERT(!(Y1 != Y1));

          EnumDef *subColors = y1.createEnumeration("COLORS");
          addColor(*subColors, SUBC0, 0);
          addColor(*subColors, SUBC1, 1);
          addColor(*subColors, SUBC2, 2);

          if (veryVerbose) {
            bsl::cout << "\nsuperColors = \n";
            superColors->print(bsl::cout);
            bsl::cout << "subColors = \n";
            subColors->print(bsl::cout);
          }
          bool isSuper = EnumDef::isSuperset(*superColors, *subColors);
          LOOP_ASSERT(LINE, ISSUPER == isSuper);

          if (veryVerbose) bsl::cout << "\n===================" << bsl::endl;
        }

        // Test with pointers to same object.
        {
          Obj x1(Z);  const Obj& X1 = x1;
          ASSERT(0  == X1.numRecords());
          ASSERT(0  == X1.numEnumerations());
          ASSERT(X1 == X1);
          ASSERT(!(X1 != X1));

          EnumDef *superColors = x1.createEnumeration("COLORS");
          superColors->addEnumerator("green");
          superColors->addEnumerator("blue");
          superColors->addEnumerator("red");

          EnumDef *subColors = superColors;

          if (veryVerbose) {
            bsl::cout << "\nsuperColors = \n";
            superColors->print(bsl::cout);
            bsl::cout << "subColors = \n";
            subColors->print(bsl::cout);
          }
          bool isSuper = EnumDef::isSuperset(*superColors, *subColors);
          ASSERT(isSuper);
        }

        {
          if (veryVerbose) bsl::cout << "\n===================" << bsl::endl;

          Obj x1(Z);  const Obj& X1 = x1;
          ASSERT(0  == X1.numRecords());
          ASSERT(0  == X1.numEnumerations());
          ASSERT(X1 == X1);
          ASSERT(!(X1 != X1));

          EnumDef *superColors = x1.createEnumeration("COLORS");

          EnumDef *subColors = superColors;

          if (veryVerbose) {
            bsl::cout << "\nsuperColors = \n";
            superColors->print(bsl::cout);
            bsl::cout << "subColors = \n";
            subColors->print(bsl::cout);
          }
          bool isSuper = EnumDef::isSuperset(*superColors, *subColors);
          ASSERT(isSuper);
        }

        // Test same enumeration different names, index, and maxId
        {
          if (veryVerbose) bsl::cout << "\n===================" << bsl::endl;

          Obj x1(Z);  const Obj& X1 = x1;
          ASSERT(0  == X1.numRecords());
          ASSERT(0  == X1.numEnumerations());
          ASSERT(X1 == X1);
          ASSERT(!(X1 != X1));

          // maxId will be 10
          EnumDef *superColors = x1.createEnumeration("COLORS");
          superColors->addEnumerator("green", 0);
          superColors->addEnumerator("blue", 5);
          superColors->addEnumerator("red", 10);

          // maxId will be 10
          EnumDef *subColors = x1.createEnumeration("COLORS2");
          subColors->addEnumerator("green", 0);
          subColors->addEnumerator("red", 10);
          subColors->addEnumerator("blue", 5);

          if (veryVerbose) {
            bsl::cout << "\nsuperColors = \n";
            superColors->print(bsl::cout);
            bsl::cout << "subColors = \n";
            subColors->print(bsl::cout);
          }
          bool isSuper = EnumDef::isSuperset(*superColors, *subColors);
          ASSERT(isSuper);

          LOOP_ASSERT(superColors->maxId(), 10 == superColors->maxId());
          LOOP_ASSERT(superColors->maxId(), 10 == subColors->maxId());

          int superIndex = superColors->enumerationIndex();
          int subIndex = subColors->enumerationIndex();
          LOOP2_ASSERT(superIndex, subIndex, superIndex != subIndex);

          const char *superName = X1.enumerationName(superIndex);
          const char *subName   = X1.enumerationName(subIndex);
          LOOP2_ASSERT(superName, subName, 0 != strCmp(superName, subName));
        }
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING AREEQUIVALENT
        //
        // Concerns:
        //   - That areEquivalent returns true, if each enumerator
        //     has the same name and ID) and 'false' otherwise.
        //   - That 'maxId()', 'enumerationIndex()' and this definition's
        //     name are not considered in this comparison.
        //
        // Plan:
        //   Verify that enumerations having the same enumerators
        //   (names and ids) are considered equivalent and that enumerations
        //   having the different enumerators are not considered equivalent.
        //   Also test that 'maxId' 'enumerationIndex()' and this definition's
        //   name are not considered in the equivalent comparison.
        //
        // Testing:
        //   areEquivalent(const bdem_EnumerationDef& lhs,
        //                 const bdem_EnumerationDef& rhs);
        // --------------------------------------------------------------------
        const struct {
          int         d_line;
          const char *d_spec;
          bool        d_areEquivalent;
        } DATA[] = {
          //LINE spec               is equivalent
          //---- ------------------ ----------
          // LEVEL 0
          { L_,  ":a= :b=",         true  },

          // LEVEL 1
          { L_,  ":a=a0 :b=",       false },

          // LEVEL 2
          { L_,  ":a=a0 :b=a0",     true  },
          { L_,  ":a=w1 :b=w1",     true  }, // longer name 'doubleU'

          { L_,  ":a=a0 :b=a1",     false },
          { L_,  ":a=w0 :b=w1",     false }, // longer name 'doubleU'

          { L_,  ":a=a0 :b=c0",     false },
          { L_,  ":a=a0 :b=w0",     false }, // longer name 'doubleU'

          // LEVEL 3
          { L_,  ":a=a0 :b=a0c1",   false },
          { L_,  ":a=a0 :b=a0w1",   false }, // longer name 'doubleU'

          { L_,  ":a=a0 :b=c0a1",   false },
          { L_,  ":a=a0 :b=a0w1",   false }, // longer name 'doubleU'

          { L_,  ":a=c0 :b=a0c1",   false },
          { L_,  ":a=w0 :b=a0w1",   false }, // longer name 'doubleU'

          { L_,  ":a=a1 :b=a0c1",   false },
          { L_,  ":a=a1 :b=a0w1",   false }, // longer name 'doubleU'

          { L_,  ":a=c1 :b=a0c1",   false },
          { L_,  ":a=w1 :b=a0w1",   false }, // longer name 'doubleU'

          // LEVEL 4
          { L_,  ":a=a0c1 :b=a0c1", true  },
          { L_,  ":a=a0w1 :b=a0w1", true  }, // longer name 'doubleU'

          { L_,  ":a=a1c0 :b=a0c1", false },
          { L_,  ":a=a1w0 :b=a0w1", false }, // longer name 'doubleU'

          { L_,  ":a=c0a1 :b=c0a1", true  },
          { L_,  ":a=w0a1 :b=w0a1", true  }, // longer name 'doubleU'

          { L_,  ":a=c1a0 :b=c0a1", false },
          { L_,  ":a=w1a0 :b=w0a1", false }, // longer name 'doubleU'

          { L_,  ":a=a0c1 :b=c0a1", false },
          { L_,  ":a=a0w1 :b=w0a1", false }, // longer name 'doubleU'

          { L_,  ":a=a1c0 :b=c0a1", true  },
          { L_,  ":a=a1w0 :b=w0a1", true  }  // longer name 'doubleU'
        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        if (verbose) bsl::cout << "\nTesting areEquivalent:"
                               << "\n======================" << bsl::endl;

        for (int ii = 0; ii < NUM_DATA; ++ii) {
          const int   LINE      = DATA[ii].d_line;
          const char *SPEC      = DATA[ii].d_spec;
          const bool EQUIVALENT = DATA[ii].d_areEquivalent;

          if (veryVerbose) { T_();  P(SPEC); }

          Obj x1(Z);  const Obj& X1 = x1;
          gg(&x1, SPEC);

          ASSERT(0  == X1.numRecords());
          ASSERT(2  == X1.numEnumerations());
          ASSERT(X1 == X1);
          ASSERT(!(X1 != X1));

          if (veryVeryVerbose) x1.print(bsl::cout);

          const EnumDef& E1 = X1.enumeration(0);
          const EnumDef& E2 = X1.enumeration(1);

          LOOP_ASSERT(LINE, EQUIVALENT == EnumDef::areEquivalent(E1, E2));
          LOOP_ASSERT(LINE, EQUIVALENT == EnumDef::areEquivalent(E2, E1));
        }

        // Test same enumeration different names, index
        {
          if (veryVerbose) bsl::cout << "\n===================" << bsl::endl;

          Obj x1(Z);  const Obj& X1 = x1;
          ASSERT(0  == X1.numRecords());
          ASSERT(0  == X1.numEnumerations());
          ASSERT(X1 == X1);
          ASSERT(!(X1 != X1));

          // maxId will be 10
          EnumDef *CE1 = x1.createEnumeration("CE1");
          CE1->addEnumerator("green", 0);
          CE1->addEnumerator("blue", 5);
          CE1->addEnumerator("red", 10);

          // maxId will be 10
          EnumDef *CE2 = x1.createEnumeration("CE2");
          CE2->addEnumerator("green", 0);
          CE2->addEnumerator("red", 10);
          CE2->addEnumerator("blue", 5);

          if (veryVerbose) {
            bsl::cout << "\nCE1 = \n";
            CE1->print(bsl::cout);
            bsl::cout << "CE2 = \n";
            CE2->print(bsl::cout);
          }
          bool areEquivalent = EnumDef::areEquivalent(*CE1, *CE2);
          ASSERT(areEquivalent);

          int CE1NId = CE1->maxId();
          int CE2NId = CE2->maxId();
          LOOP2_ASSERT(CE1NId, CE2NId, CE1NId == CE2NId);

          int CE1Index = CE1->enumerationIndex();
          int CE2Index = CE2->enumerationIndex();
          LOOP2_ASSERT(CE1Index, CE2Index, CE1Index != CE2Index);

          const char *CE1Name = X1.enumerationName(CE1Index);
          const char *CE2Name = X1.enumerationName(CE2Index);
          LOOP2_ASSERT(CE1Name, CE2Name, 0 != strCmp(CE1Name, CE2Name));
        }
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING FORWARD REFERENCES
        //
        // Concerns:
        //   - A field of aggregate type can be constrained by a record with a
        //     higher index than that of the containing record.
        //   - Schemas containing fields having such forward references are
        //     copyable, assignable, and 'bdex'-streamable.
        //
        // Plan:
        //   Verify that a schema having fields constrained by "forward"
        //   records may be constructed, copied, and assigned.  Further verify
        //   that the schema can be 'bdex' streamed correctly.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Forward References"
                               << "\n==========================" << bsl::endl;

        bslma_TestAllocator         guardedAlloc(veryVeryVerbose);
        bslma_DefaultAllocatorGuard guard(&guardedAlloc);

        const int NUM_REC = 3;
        {
            Obj mS(Z);  const Obj& S = mS;

            RecDef *R[NUM_REC] = { 0, 0, 0 };

            for (int i = 0; i < NUM_REC; ++i) {       // create records
                R[i] = mS.createRecord(getName(i + 'a'));
                ASSERT(R[i]);
            }

            for (int i = 0; i < NUM_REC; ++i) {       // append fields
                for (int j = 0; j < NUM_REC; ++j) {
                    const FldDef  *FD = R[i]->appendField(EType::BDEM_LIST,
                                                          R[j],
                                                          getName(j + 'a'),
                                                          j);
                    ASSERT(R[j] == FD->recordConstraint());
                }
            }
            ASSERT(0 == guardedAlloc.numBytesInUse());

            {                                         // copy
                Obj mT(S, Z);  const Obj& T = mT;
                ASSERT(S == T);
                ASSERT(0 == guardedAlloc.numBytesInUse());
            }

            {                                        // assign
                Obj mT(Z);  const Obj& T = mT;
                mT = S;
                ASSERT(S == T);
                ASSERT(0 == guardedAlloc.numBytesInUse());
            }

            {                                        // 'bdex' stream
                const int VERSION = 1;

                Out out(Z);  S.bdexStreamOut(out, VERSION);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                In in(OD, LOD, Z);  ASSERT(in);  ASSERT(!in.isEmpty());
                in.setSuppressVersionCheck(1);  // needed for 1.1

                Obj t(Z);

                ASSERT(S != t);

                t.bdexStreamIn(in, VERSION);
                                    ASSERT(in);  ASSERT(in.isEmpty());

                ASSERT(S == t);
                ASSERT(0 == guardedAlloc.numBytesInUse());
            }
        }

      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING VERSION 1.1
        //
        // Concerns:
        //   1. The (free) streaming operators '<<' and '>>' are implemented
        //      using the respective member functions 'bdexStreamOut' and
        //      'bdexStreamIn'.
        //   2. Streaming must be neutral to exceptions thrown as a result of
        //      either allocating memory or streaming in values.
        //   3. Ensure that streaming works for any sequence of valid values.
        //
        // Plan:
        //   First perform a trivial direct (breathing) test of the
        //   'bdexStreamOut' and 'bdexStreamIn' methods (to address concern 1).
        //   Test with a schema that has a 'CHOICE' record to force streaming
        //   using version 1.1.  Note that the rest of the testing will use the
        //   stream operators.
        //
        //   Next, specify a set S of unique object values with substantial
        //   and varied differences.  Ensure that each non-empty schema object
        //   in S contains a 'CHOICE' record to force streaming using version
        //   1.1.  For each value in S, construct an object x.
        //
        //   Note that, for performance reasons, the 'bdex' and 'bdema'
        //   exception testing are done serially rather than nested.  The
        //   code is substantially repeated for the 'bdema' exception test,
        //   but it is *not* necessary to repeat the 'ASSERT' statements.
        //
        //   VALID STREAMS (and exceptions)
        //     Using all combinations of (u, v) in S x S, stream out the value
        //     of u into a buffer and stream it back into (an independent
        //     instance of) v, and assert that u == v.
        //
        // Testing:
        //  ^bdem_RecordDef::bdexStreamOut(STREAM& stream, major, minor) const;
        //  ^streamInRecordDef(STREAM& stream, *schema, *ba, major, minor);
        //   bdexStreamIn(STREAM& stream, int version);
        //   bdexStreamOut(STREAM& stream, int version) const;
        //
        // Note: '^' indicates a private method, which is tested indirectly
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting 'bdex' Streaming Version 1.1"
                                  "\n===================================="
                               << bsl::endl;

        const int VERSION = 1;

        if (verbose) bsl::cout << "\nDirect initial trial of 'streamOut' and"
                                  " (valid) 'streamIn' functionality."
                               << bsl::endl;
        {
            const Obj X(g(":a? Aa :b Bb%ca"), &testAllocator);
            if (veryVerbose) {
                bsl::cout << "  Value being streamed:\n"; PS(X);
            }

            Out out;
            out.putVersion(VERSION);
            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            In in(OD, LOD);  ASSERT(in);  ASSERT(!in.isEmpty());

            Obj t(g(":d DdEe"), &testAllocator);

            if (veryVerbose) {
                bsl::cout << "  Value being overwritten:\n"; PS(t);
            }
            ASSERT(X != t);

            int version;
            in.getVersion(version);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(in);  ASSERT(in.isEmpty());

            if (veryVerbose) {
                bsl::cout << "  Value after overwrite:\n"; PS(t);
            }
            ASSERT(X == t);
        }
        {
            const Obj X(g(":a? Aa :b Bb@ca"), &testAllocator);
            if (veryVerbose) {
                bsl::cout << "  Value being streamed:\n"; PS(X);
            }

            Out out;  X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            In in(OD, LOD);  ASSERT(in);  ASSERT(!in.isEmpty());
            in.setSuppressVersionCheck(1);  // needed for 1.1

            Obj t(g(":d DdEe"), &testAllocator);

            if (veryVerbose) {
                bsl::cout << "  Value being overwritten:\n"; PS(t);
            }
            ASSERT(X != t);

            t.bdexStreamIn(in, VERSION);     ASSERT(in);  ASSERT(in.isEmpty());

            if (veryVerbose) {
                bsl::cout << "  Value after overwrite:\n"; PS(t);
            }
            ASSERT(X == t);
        }

        if (verbose)
            bsl::cout << "\nTesting stream operators ('<<' and '>>')."
                      << bsl::endl;

        if (verbose)
            bsl::cout << "\tOn valid, non-empty stream data." << bsl::endl;
        {
            // Create an empty CHOICE record in each schema (except empty ones)
            // to force bdex 1.1.

            for (int i = 0; i < NUM_SPECS; ++i) {
                const int         LINE   = testSpecs[i].d_lineNum;
                const char *const U_SPEC = testSpecs[i].d_spec;
                Obj uu = g(U_SPEC);  const Obj& UU = uu;             // control
                if (0 != i) {  // leave empty schema empty
                    uu.createRecord(RecDef::BDEM_CHOICE_RECORD);
                }

                Obj u(Z);  const Obj& U = u;  gg(&u, U_SPEC);
                if (0 != i) {  // leave empty schema empty
                    u.createRecord(RecDef::BDEM_CHOICE_RECORD);
                }

                LOOP_ASSERT(LINE, UU == U);

                Out out;
                U.bdexStreamOut(out, VERSION);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Must reset stream for each iteration of inner loop.
                In testInStream(OD, LOD);  In& in = testInStream;
                in.setSuppressVersionCheck(1);  // needed for 1.1
                LOOP_ASSERT(LINE, in);
                LOOP_ASSERT(LINE, !in.isEmpty());

                for (int j = 0; j < NUM_SPECS; ++j) {
                    const int         JLINE  = testSpecs[j].d_lineNum;
                    const char *const V_SPEC = testSpecs[j].d_spec;
                    Obj vv = g(V_SPEC);  const Obj& VV = vv;         // control
                    const int iEQj = i == j;

                    if (0 != j) {  // leave empty schema empty
                        vv.createRecord(RecDef::BDEM_CHOICE_RECORD);
                    }

                    if (veryVeryVerbose) { T_();  T_();  P(V_SPEC); }

                    BEGIN_BDEX_EXCEPTION_TEST {
                      in.reset();
                      LOOP2_ASSERT(LINE, JLINE, in);
                      LOOP2_ASSERT(LINE, JLINE, !in.isEmpty());

                      Obj v(Z);  const Obj& V = v;  gg(&v, V_SPEC);
                      if (0 != j) {  // leave empty schema empty
                          v.createRecord(RecDef::BDEM_CHOICE_RECORD);
                      }

                      LOOP2_ASSERT(LINE, JLINE,   UU == U);
                      LOOP2_ASSERT(LINE, JLINE,   VV == V);
                      LOOP2_ASSERT(LINE, JLINE, iEQj == (U == V));

                      // test stream-in operator here
                      bdex_InStreamFunctions::streamIn(in, v, VERSION);

                      LOOP2_ASSERT(LINE, JLINE, UU == U);
                      LOOP4_ASSERT(LINE, JLINE, UU, V, UU == V);
                      LOOP2_ASSERT(LINE, JLINE,  U == V);

                    } END_BDEX_EXCEPTION_TEST

                    // Repeat the above streaming with 'bdema' exception-test.
                    // Note that it is *not* necessary to 'ASSERT' anything!
                    BEGIN_BSLMA_EXCEPTION_TEST {
                      in.reset();
                      const int AL = testAllocator.allocationLimit();
                      testAllocator.setAllocationLimit(-1);
                      Obj v(Z);  gg(&v, V_SPEC);

                      testAllocator.setAllocationLimit(AL);

                      // testing 'bdema' exceptions only!
                      bdex_InStreamFunctions::streamIn(in, v, VERSION);

                    } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING ENUMERATIONS NAMES AND SET ENUMERATION NAME
        //
        // Concerns:
        //   - The 'createEnumeration' and 'setEnumerationName' methods work as
        //     expected.
        //   - All memory comes from the supplied allocator.
        //
        // Plan:
        //   Use 'createEnumeration' to create various enumerations.  Verify
        //   that the enumerations have the expected name.  Test the use of
        //   'setEnumerationName' to set the name of unnamed enumerations.
        //   Verify that 'createEnumeration' and 'setEnumerationName' both fail
        //   on attempts to reuse a name.  Test allocators and a default
        //   allocator guard are used to verify that all memory is allocated
        //   through the supplied allocator.
        //
        // Testing:
        //   bdem_EnumerationDef *createEnumeration(const char *name = 0);
        //   int setEnumerationName(bdem_EnumerationDef *e, const char *name);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Enumeration Names"
                                  "\n========================="
                               << bsl::endl;

        bslma_TestAllocator         guardedAlloc(veryVeryVerbose);
        bslma_DefaultAllocatorGuard guard(&guardedAlloc);

        const char ENUMNAMES[] = "*ea";
        const int NUM_ENUMNAMES = sizeof ENUMNAMES - 1;

        // default name and default record type
        {
            Obj mS(Z);  const Obj& S = mS;
            ASSERT(0 == S.numEnumerations());

            const EnumDef *pE = mS.createEnumeration();
            ASSERT(pE);
            ASSERT(1 == S.numEnumerations());
            ASSERT(0 == S.enumerationName(0));
            ASSERT(0 == guardedAlloc.numBytesInUse());
        }

        // explicit name and default record type
        {
            for (int tn = 0; tn < NUM_ENUMNAMES; ++tn) {
                const char NAME = ENUMNAMES[tn];

                Obj mS(Z);  const Obj& S = mS;
                ASSERT(0 == S.numEnumerations());

                const EnumDef *pE = mS.createEnumeration(getName(NAME));
                ASSERT(pE);
                ASSERT(1 == S.numEnumerations());
                ASSERT(0 == strCmp(getName(NAME), S.enumerationName(0)));

                if ('*' != NAME) {  // 'pE' above is not an unnamed enum.
                    const EnumDef *pE = mS.createEnumeration(getName(NAME));
                    ASSERT(!pE);
                    ASSERT(1 == S.numEnumerations());
                }
                else {
                    const EnumDef *pE = mS.createEnumeration(getName(NAME));
                    ASSERT(pE);
                    ASSERT(2 == S.numEnumerations());
                    ASSERT(0 == strCmp(getName(NAME), S.enumerationName(1)));
                }
                ASSERT(0 == guardedAlloc.numBytesInUse());
            }
        }

      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING RECORD TYPES AND RECORD NAMES
        //
        // Concerns:
        //   - The 'createRecord' and 'setRecordName' methods work as expected.
        //   - All memory comes from the supplied allocator.
        //
        // Plan:
        //   Use 'createRecord' to create various records.  Verify that the
        //   records have the expected type and name.  Test the use of
        //   'setRecordName' to set the name of unnamed records.  Verify that
        //   'createRecord' and 'setRecordName' both fail on attempts to reuse
        //   a name.  Test allocators and a default allocator guard are used to
        //   verify that all memory is allocated through the supplied
        //   allocator.
        //
        // Testing:
        //   RecordType bdem_RecordDef::recordType() const;
        //   bdem_RecordDef *createRecord(bdem_RecordDef::RecordType type);
        //   bdem_RecordDef *createRecord(*name=0, type=BDEM_SEQUENCE_RECORD);
        //   int setRecordName(bdem_RecordDef *record, const char *name);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Record Types and Record Names"
                                  "\n====================================="
                               << bsl::endl;

        bslma_TestAllocator         guardedAlloc(veryVeryVerbose);
        bslma_DefaultAllocatorGuard guard(&guardedAlloc);

        const RecType RECTYPES[] = {
            RecDef::BDEM_SEQUENCE_RECORD,
            RecDef::BDEM_CHOICE_RECORD
        };
        const int NUM_RECTYPES = sizeof RECTYPES / sizeof *RECTYPES;

        const char RECNAMES[] = "*ea";
        const int NUM_RECNAMES = sizeof RECNAMES - 1;

        if (verbose) bsl::cout << "\nTesting 1-argument 'createRecord'."
                               << bsl::endl;
        {
            for (int tt = 0; tt < NUM_RECTYPES; ++tt) {
                const RecType TYPE = RECTYPES[tt];

                Obj mS(Z);  const Obj& S = mS;
                ASSERT(0 == S.numRecords());
                ASSERT(0 == S.numEnumerations());

                const RecDef *pR = mS.createRecord(TYPE);
                ASSERT(pR);
                ASSERT(1    == S.numRecords());
                ASSERT(TYPE == pR->recordType());
                ASSERT(0    == S.recordName(0));
                ASSERT(0    == guardedAlloc.numBytesInUse());
            }
        }

        if (verbose) bsl::cout << "\nTesting 2-argument 'createRecord'."
                               << bsl::endl;

        // [1] default name and default record type
        {
            Obj mS(Z);  const Obj& S = mS;
            ASSERT(0 == S.numRecords());

            const RecDef *pR = mS.createRecord();
            ASSERT(pR);
            ASSERT(1 == S.numRecords());
            ASSERT(RecDef::BDEM_SEQUENCE_RECORD == pR->recordType());
            ASSERT(0 == S.recordName(0));
            ASSERT(0 == guardedAlloc.numBytesInUse());
        }

        // [2] explicit name and default record type
        {
            for (int tn = 0; tn < NUM_RECNAMES; ++tn) {
                const char NAME = RECNAMES[tn];

                Obj mS(Z);  const Obj& S = mS;
                ASSERT(0 == S.numRecords());

                const RecDef *pR = mS.createRecord(getName(NAME));
                ASSERT(pR);
                ASSERT(1 == S.numRecords());
                ASSERT(RecDef::BDEM_SEQUENCE_RECORD == pR->recordType());
                ASSERT(0 == strCmp(getName(NAME), S.recordName(0)));

                if ('*' != NAME) {  // 'pR' above is not an unnamed record.
                    const RecDef *pR = mS.createRecord(getName(NAME));
                    ASSERT(0 == pR);
                    ASSERT(1 == S.numRecords());
                }
                else {
                    const RecDef *pR = mS.createRecord(getName(NAME));
                    ASSERT(pR);
                    ASSERT(2 == S.numRecords());
                    ASSERT(RecDef::BDEM_SEQUENCE_RECORD == pR->recordType());
                    ASSERT(0 == strCmp(getName(NAME), S.recordName(1)));
                }
                ASSERT(0 == guardedAlloc.numBytesInUse());
            }
        }

        // [3] explicit name and explicit record type
        {
            for (int tt = 0; tt < NUM_RECTYPES; ++tt) {
                const RecType TYPE = RECTYPES[tt];

                for (int tn = 0; tn < NUM_RECNAMES; ++tn) {
                    const char NAME = RECNAMES[tn];

                    Obj mS(Z);  const Obj& S = mS;
                    ASSERT(0 == S.numRecords());

                    const RecDef *pR = mS.createRecord(getName(NAME), TYPE);
                    ASSERT(pR);
                    ASSERT(1    == S.numRecords());
                    ASSERT(TYPE == pR->recordType());
                    ASSERT(0    == strCmp(getName(NAME), S.recordName(0)));

                    if ('*' != NAME) {  // 'pR' above is not an unnamed record.
                        const RecDef *pR = mS.createRecord(getName(NAME),
                                                           TYPE);
                        ASSERT(0 == pR);
                        ASSERT(1 == S.numRecords());
                    }
                    else {
                        const RecDef *pR = mS.createRecord(getName(NAME),
                                                           TYPE);
                        ASSERT(pR);
                        ASSERT(2    == S.numRecords());
                        ASSERT(TYPE == pR->recordType());
                        ASSERT(0    == strCmp(getName(NAME), S.recordName(1)));
                    }
                    ASSERT(0 == guardedAlloc.numBytesInUse());
                }
            }
        }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING FIELD IDS AND FIELD NAMES
        //
        // Plan:
        //   - The 'appendField' methods work as expected.
        //   - The id-related accessors work as expected.
        //   - All memory comes from the supplied allocator.
        //
        // Plan:
        //   Use the 'appendField' methods to create fields having varied
        //   combinations of name and id.  Verify that the created fields have
        //   the expected names and ids.  Verify that 'appendField' fails on
        //   attempts to reuse a name or an id.  Test allocators and a default
        //   allocator guard are used to verify that all memory is allocated
        //   through the supplied allocator.
        //
        // Testing:
        //  ^int bdem_FieldDef::fieldId() const;
        //   const FldDef *appendField(const FldSpec&, *name=0, id=Null);
        //   const FldDef *appendField(EType type, *name=0, id=Null);
        //   const FldDef *appendField(type, *c, *name=0, id=Null);
        //   int fieldId(int index) const;
        //   int fieldIndex(int id) const;
        //   bool hasFieldIds() const;
        //   const bdem_FieldDef *lookupField(int id) const;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Field Ids"
                                  "\n=================" << bsl::endl;

        bslma_TestAllocator         guardedAlloc(veryVeryVerbose);
        bslma_DefaultAllocatorGuard guard(&guardedAlloc);

        const EType::Type TYPE = EType::BDEM_INT;
        const RecDef *CONSTRAINT = 0;

        // [1] default name and default field id
        {
            // appendField(field-spec)
            {
                Obj mS(Z);  const Obj& S = mS;

                RecDef *pR = mS.createRecord();
                ASSERT(1 == S.numRecords());
                ASSERT(0 == pR->numFields());

                const FldAttr FA(TYPE);

                const FldDef *pFD = pR->appendField(FA);
                ASSERT(pFD);
                ASSERT(false == pR->hasFieldIds());
                ASSERT(0     == pR->fieldId(0));
                ASSERT(0     == pR->fieldIndex(0));
                ASSERT(pFD   == pR->lookupField(0));

                const FldDef *pFD2 = pR->appendField(FA);
                ASSERT(pFD2);
                ASSERT(false == pR->hasFieldIds());
                ASSERT(0     == pR->fieldId(0));
                ASSERT(1     == pR->fieldId(1));
                ASSERT(0     == pR->fieldIndex(0));
                ASSERT(1     == pR->fieldIndex(1));
                ASSERT(pFD   == pR->lookupField(0));
                ASSERT(pFD2  == pR->lookupField(1));

                ASSERT(0     == guardedAlloc.numBytesInUse());
            }

            // appendField(type)
            {
                Obj mS(Z);  const Obj& S = mS;

                RecDef *pR = mS.createRecord();
                ASSERT(1 == S.numRecords());
                ASSERT(0 == pR->numFields());

                const FldDef *pFD = pR->appendField(TYPE);
                ASSERT(pFD);
                ASSERT(false == pR->hasFieldIds());
                ASSERT(0     == pR->fieldId(0));
                ASSERT(0     == pR->fieldIndex(0));
                ASSERT(pFD   == pR->lookupField(0));

                const FldDef *pFD2 = pR->appendField(TYPE);
                ASSERT(pFD2);
                ASSERT(false == pR->hasFieldIds());
                ASSERT(0     == pR->fieldId(0));
                ASSERT(1     == pR->fieldId(1));
                ASSERT(0     == pR->fieldIndex(0));
                ASSERT(1     == pR->fieldIndex(1));
                ASSERT(pFD   == pR->lookupField(0));
                ASSERT(pFD2  == pR->lookupField(1));

                ASSERT(0     == guardedAlloc.numBytesInUse());
            }

            // appendField(type, constraint)
            {
                Obj mS(Z);  const Obj& S = mS;

                RecDef *pR = mS.createRecord();
                ASSERT(1 == S.numRecords());
                ASSERT(0 == pR->numFields());

                const FldDef *pFD = pR->appendField(TYPE, CONSTRAINT);
                ASSERT(pFD);
                ASSERT(false == pR->hasFieldIds());
                ASSERT(0     == pR->fieldId(0));
                ASSERT(0     == pR->fieldIndex(0));
                ASSERT(pFD   == pR->lookupField(0));

                const FldDef *pFD2 = pR->appendField(TYPE, CONSTRAINT);
                ASSERT(pFD2);
                ASSERT(false == pR->hasFieldIds());
                ASSERT(0     == pR->fieldId(0));
                ASSERT(1     == pR->fieldId(1));
                ASSERT(0     == pR->fieldIndex(0));
                ASSERT(1     == pR->fieldIndex(1));
                ASSERT(pFD   == pR->lookupField(0));
                ASSERT(pFD2  == pR->lookupField(1));

                ASSERT(0     == guardedAlloc.numBytesInUse());
            }
        }

        const char FLDNAMES[] = "*ea";
        const int NUM_FLDNAMES = sizeof FLDNAMES - 1;

        const int FLDIDS[] = {
            RecDef::BDEM_NULL_FIELD_ID, -2, 0, 1, 7
        };
        const int NUM_FLDIDS = sizeof FLDIDS / sizeof *FLDIDS;

        const char XNAME = 'b';  // name not in 'FLDNAMES'
        const int  XID   = 33;   // id not in 'FLDIDS'

        // [2] explicit name and default field id
        {
            for (int tn = 0; tn < NUM_FLDNAMES; ++tn) {
                const char NAME = FLDNAMES[tn];

                // appendField(field-spec, name)
                {
                    Obj mS(Z);  const Obj& S = mS;

                    RecDef *pR = mS.createRecord();
                    ASSERT(1 == S.numRecords());
                    ASSERT(0 == pR->numFields());

                    const FldAttr FA(TYPE);

                    const FldDef *pFD = pR->appendField(FA, getName(NAME));
                    ASSERT(pFD);
                    ASSERT(false == pR->hasFieldIds());
                    ASSERT(0     == pR->fieldId(0));
                    ASSERT(0     == pR->fieldIndex(0));
                    ASSERT(pFD   == pR->lookupField(0));

                    const FldDef *pFD2 = pR->appendField(FA, getName(NAME));
                    if ('*' == NAME) {
                        ASSERT(pFD2);
                        ASSERT(1    == pR->fieldId(1));
                        ASSERT(1    == pR->fieldIndex(1));
                        ASSERT(pFD2 == pR->lookupField(1));
                    }
                    else {
                        ASSERT(!pFD2);
                    }
                    ASSERT(false == pR->hasFieldIds());
                    ASSERT(0     == pR->fieldId(0));
                    ASSERT(0     == pR->fieldIndex(0));
                    ASSERT(pFD   == pR->lookupField(0));

                    const int NEXT = pR->numFields();
                    const FldDef *pFD3 = pR->appendField(FA, getName(XNAME));
                    ASSERT(pFD3);
                    ASSERT(false == pR->hasFieldIds());
                    ASSERT(NEXT  == pR->fieldId(NEXT));
                    ASSERT(NEXT  == pR->fieldIndex(NEXT));
                    ASSERT(pFD3  == pR->lookupField(NEXT));

                    ASSERT(0     == guardedAlloc.numBytesInUse());
                }

                // appendField(type, name)
                {
                    Obj mS(Z);  const Obj& S = mS;

                    RecDef *pR = mS.createRecord();
                    ASSERT(1 == S.numRecords());
                    ASSERT(0 == pR->numFields());

                    const FldDef *pFD = pR->appendField(TYPE, getName(NAME));
                    ASSERT(pFD);
                    ASSERT(false == pR->hasFieldIds());
                    ASSERT(0     == pR->fieldId(0));
                    ASSERT(0     == pR->fieldIndex(0));
                    ASSERT(pFD   == pR->lookupField(0));

                    const FldDef *pFD2 = pR->appendField(TYPE, getName(NAME));
                    if ('*' == NAME) {
                        ASSERT(pFD2);
                        ASSERT(1    == pR->fieldId(1));
                        ASSERT(1    == pR->fieldIndex(1));
                        ASSERT(pFD2 == pR->lookupField(1));
                    }
                    else {
                        ASSERT(!pFD2);
                    }
                    ASSERT(false == pR->hasFieldIds());
                    ASSERT(0     == pR->fieldId(0));
                    ASSERT(0     == pR->fieldIndex(0));
                    ASSERT(pFD   == pR->lookupField(0));

                    const int NEXT = pR->numFields();
                    const FldDef *pFD3 = pR->appendField(TYPE, getName(XNAME));
                    ASSERT(pFD3);
                    ASSERT(false == pR->hasFieldIds());
                    ASSERT(NEXT  == pR->fieldId(NEXT));
                    ASSERT(NEXT  == pR->fieldIndex(NEXT));
                    ASSERT(pFD3  == pR->lookupField(NEXT));

                    ASSERT(0     == guardedAlloc.numBytesInUse());
                }

                // appendField(type, constraint, name)
                {
                    Obj mS(Z);  const Obj& S = mS;

                    RecDef *pR = mS.createRecord();
                    ASSERT(1 == S.numRecords());
                    ASSERT(0 == pR->numFields());

                    const FldDef *pFD =
                              pR->appendField(TYPE, CONSTRAINT, getName(NAME));
                    ASSERT(pFD);
                    ASSERT(false == pR->hasFieldIds());
                    ASSERT(0     == pR->fieldId(0));
                    ASSERT(0     == pR->fieldIndex(0));
                    ASSERT(pFD   == pR->lookupField(0));

                    const FldDef *pFD2 =
                              pR->appendField(TYPE, CONSTRAINT, getName(NAME));
                    if ('*' == NAME) {
                        ASSERT(pFD2);
                        ASSERT(1    == pR->fieldId(1));
                        ASSERT(1    == pR->fieldIndex(1));
                        ASSERT(pFD2 == pR->lookupField(1));
                    }
                    else {
                        ASSERT(!pFD2);
                    }
                    ASSERT(false == pR->hasFieldIds());
                    ASSERT(0     == pR->fieldId(0));
                    ASSERT(0     == pR->fieldIndex(0));
                    ASSERT(pFD   == pR->lookupField(0));

                    const int NEXT = pR->numFields();
                    const FldDef *pFD3 = pR->appendField(TYPE,
                                                         CONSTRAINT,
                                                         getName(XNAME));
                    ASSERT(pFD3);
                    ASSERT(false == pR->hasFieldIds());
                    ASSERT(NEXT  == pR->fieldId(NEXT));
                    ASSERT(NEXT  == pR->fieldIndex(NEXT));
                    ASSERT(pFD3  == pR->lookupField(NEXT));

                    ASSERT(0     == guardedAlloc.numBytesInUse());
                }
            }
        }

        // [3] explicit name and explicit field id
        {
            for (int ti = 0; ti < NUM_FLDIDS; ++ti) {
                const int ID = FLDIDS[ti];

                for (int tn = 0; tn < NUM_FLDNAMES; ++tn) {
                    const char NAME = FLDNAMES[tn];

                    // appendField(field-spec, name, id)
                    {
                        Obj mS(Z);  const Obj& S = mS;

                        RecDef *pR = mS.createRecord();
                        ASSERT(1 == S.numRecords());
                        ASSERT(0 == pR->numFields());

                        const FldAttr FA(TYPE);

                        const FldDef *pFD = pR->appendField(FA,
                                                            getName(NAME),
                                                            ID);
                        ASSERT(pFD);

                        const bool HASNAME = '*' != NAME;
                        const bool HASID   = RecDef::BDEM_NULL_FIELD_ID != ID;

                        const FldDef *pFD2 = pR->appendField(FA,
                                                             getName(NAME),
                                                             ID);
                        const FldDef *pFD3 = pR->appendField(FA,
                                                             getName(XNAME),
                                                             ID);
                        const FldDef *pFD4 = pR->appendField(FA,
                                                             getName(NAME),
                                                             XID);

                        if (! HASNAME && ! HASID) {
                            ASSERT( pFD2);
                            ASSERT( pFD3);
                            ASSERT( pFD4);
                        }
                        if (  HASNAME && ! HASID) {
                            ASSERT(!pFD2);
                            ASSERT( pFD3);
                            ASSERT(!pFD4);
                        }
                        if (! HASNAME &&   HASID) {
                            ASSERT(!pFD2);
                            ASSERT(!pFD3);
                            ASSERT( pFD4);
                        }
                        if (  HASNAME &&   HASID) {
                            ASSERT(!pFD2);
                            ASSERT(!pFD3);
                            ASSERT(!pFD4);
                        }

                        ASSERT(0 == guardedAlloc.numBytesInUse());
                    }

                    // appendField(type, name, id)
                    {
                        Obj mS(Z);  const Obj& S = mS;

                        RecDef *pR = mS.createRecord();
                        ASSERT(1 == S.numRecords());
                        ASSERT(0 == pR->numFields());

                        const FldDef *pFD = pR->appendField(TYPE,
                                                            getName(NAME),
                                                            ID);
                        ASSERT(pFD);

                        const bool HASNAME = '*' != NAME;
                        const bool HASID   = RecDef::BDEM_NULL_FIELD_ID != ID;

                        const FldDef *pFD2 = pR->appendField(TYPE,
                                                             getName(NAME),
                                                             ID);
                        const FldDef *pFD3 = pR->appendField(TYPE,
                                                             getName(XNAME),
                                                             ID);
                        const FldDef *pFD4 = pR->appendField(TYPE,
                                                             getName(NAME),
                                                             XID);

                        if (! HASNAME && ! HASID) {
                            ASSERT( pFD2);
                            ASSERT( pFD3);
                            ASSERT( pFD4);
                        }
                        if (  HASNAME && ! HASID) {
                            ASSERT(!pFD2);
                            ASSERT( pFD3);
                            ASSERT(!pFD4);
                        }
                        if (! HASNAME &&   HASID) {
                            ASSERT(!pFD2);
                            ASSERT(!pFD3);
                            ASSERT( pFD4);
                        }
                        if (  HASNAME &&   HASID) {
                            ASSERT(!pFD2);
                            ASSERT(!pFD3);
                            ASSERT(!pFD4);
                        }

                        ASSERT(0 == guardedAlloc.numBytesInUse());
                    }

                    // appendField(type, constraint, name, id)
                    {
                        Obj mS(Z);  const Obj& S = mS;

                        RecDef *pR = mS.createRecord();
                        ASSERT(1 == S.numRecords());
                        ASSERT(0 == pR->numFields());

                        const FldDef *pFD = pR->appendField(TYPE,
                                                            CONSTRAINT,
                                                            getName(NAME),
                                                            ID);
                        ASSERT(pFD);

                        const bool HASNAME = '*' != NAME;
                        const bool HASID   = RecDef::BDEM_NULL_FIELD_ID != ID;

                        const FldDef *pFD2 = pR->appendField(TYPE,
                                                             CONSTRAINT,
                                                             getName(NAME),
                                                             ID);
                        const FldDef *pFD3 = pR->appendField(TYPE,
                                                             CONSTRAINT,
                                                             getName(XNAME),
                                                             ID);
                        const FldDef *pFD4 = pR->appendField(TYPE,
                                                             CONSTRAINT,
                                                             getName(NAME),
                                                             XID);

                        if (! HASNAME && ! HASID) {
                            ASSERT( pFD2);
                            ASSERT( pFD3);
                            ASSERT( pFD4);
                        }
                        if (  HASNAME && ! HASID) {
                            ASSERT(!pFD2);
                            ASSERT( pFD3);
                            ASSERT(!pFD4);
                        }
                        if (! HASNAME &&   HASID) {
                            ASSERT(!pFD2);
                            ASSERT(!pFD3);
                            ASSERT( pFD4);
                        }
                        if (  HASNAME &&   HASID) {
                            ASSERT(!pFD2);
                            ASSERT(!pFD3);
                            ASSERT(!pFD4);
                        }

                        ASSERT(0 == guardedAlloc.numBytesInUse());
                    }
                }
            }
        }

        // TBD fields across multiple records

      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING 'bdem_FieldDef' ACCESSORS
        //
        // Concerns:
        //   The 'bdem_FieldDef' accessors that have analogous accessors in
        //   'bdem_FieldSpec' return the correct result.
        //
        // Plan:
        //   Using the 'appendField(const FldSpec&)' method, create fields from
        //   various field specs.  Verify that the 'bdem_FieldDef' accessors
        //   under test return the expected result for the created fields.  As
        //   an additional test, use test allocators and a default allocator
        //   guard to verify that all memory is allocated through the supplied
        //   allocator.
        //
        // Testing:
        //   const FldDef *appendField(const FldSpec&, *name=0, id=Null);
        //   bdem_ConstElemRef defaultValue() const;
        //   const bdem_FieldSpec& fieldSpec() const;
        //   int formattingMode() const;
        //   bool hasDefaultValue() const;
        //   bool isNullable() const;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting 'bdem_FieldDef' Accessors"
                                  "\n================================="
                               << bsl::endl;

        bslma_TestAllocator         guardedAlloc(veryVeryVerbose);
        bslma_DefaultAllocatorGuard guard(&guardedAlloc);

        if (verbose) bsl::cout << "\nTesting 'bdem' scalar and array types."
                               << bsl::endl;
        {
            const int  FMT = 55;
            const bool N   = false;

            for (int ti = 0; ti < EType::BDEM_NUM_TYPES; ++ti) {
                const EType::Type TYPE = (EType::Type)ti;
                if (EType::isAggregateType(TYPE)) {
                    continue;
                }

                FldAttr mFA(TYPE, N, FMT, Z);  const FldAttr& FA = mFA;
                LOOP_ASSERT(TYPE, TYPE  == FA.elemType());
                LOOP_ASSERT(TYPE, FMT   == FA.formattingMode());
                LOOP_ASSERT(TYPE, N     == FA.isNullable());
                LOOP_ASSERT(TYPE, false == FA.hasDefaultValue());

                Obj mS(Z);  const Obj& S = mS;
                RecDef *pR = mS.createRecord();

                const FldDef *pFD = pR->appendField(FA);
                LOOP_ASSERT(TYPE, 1 == pR->numFields());
                LOOP_ASSERT(TYPE, pFD);

                LOOP_ASSERT(TYPE, TYPE  == pFD->elemType());
                LOOP_ASSERT(TYPE, 0     == pFD->recordConstraint());
                LOOP_ASSERT(TYPE, FMT   == pFD->formattingMode());
                LOOP_ASSERT(TYPE, N     == pFD->isNullable());
                LOOP_ASSERT(TYPE, false == pFD->hasDefaultValue());
                LOOP_ASSERT(TYPE, 0     == guardedAlloc.numBytesInUse());
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdem' aggregate types."
                               << bsl::endl;
        {
            Obj mS(Z);  const Obj& S = mS;
            gg(&mS, ":a? :b");  ASSERT(2 == S.numRecords());

            const RecDef& RC = S.record(0);
                    ASSERT(RecDef::BDEM_CHOICE_RECORD   == RC.recordType());
            const RecDef& RS = S.record(1);
                    ASSERT(RecDef::BDEM_SEQUENCE_RECORD == RS.recordType());

            const int FMT = 55;

            static const struct {
                int           d_lineNum;  // source line number
                EType::Type   d_type;     // 'bdem' type
            } DATA[] = {
                //line  'bdem' type
                //----  -----------
                { L_,   EType::BDEM_LIST             },
                { L_,   EType::BDEM_TABLE            },
                { L_,   EType::BDEM_CHOICE           },
                { L_,   EType::BDEM_CHOICE_ARRAY     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const EType::Type TYPE = DATA[ti].d_type;

                const bool N = EType::BDEM_LIST == TYPE ? true : false;

                Obj mT(S, Z);  const Obj& T = mT;
                ASSERT(2 == T.numRecords());
                RecDef *pR = mT.createRecord();

                // Null constraint.
                {
                    const RecDef *CONSTR = 0;

                          FldAttr  mFA(TYPE, N, FMT, Z);
                    const FldAttr&  FA = mFA;
                    LOOP_ASSERT(LINE, TYPE   == FA.elemType());
                    LOOP_ASSERT(LINE, FMT    == FA.formattingMode());
                    LOOP_ASSERT(LINE, N      == FA.isNullable());
                    LOOP_ASSERT(LINE, false  == FA.hasDefaultValue());

                    const FldDef *pFD = pR->appendField(FA);
                    LOOP_ASSERT(TYPE, 1 == pR->numFields());
                    LOOP_ASSERT(TYPE, pFD);

                    LOOP_ASSERT(TYPE, TYPE   == pFD->elemType());
                    LOOP_ASSERT(TYPE, CONSTR == pFD->recordConstraint());
                    LOOP_ASSERT(TYPE, FMT    == pFD->formattingMode());
                    LOOP_ASSERT(TYPE, N      == pFD->isNullable());
                    LOOP_ASSERT(TYPE, false  == pFD->hasDefaultValue());

                    LOOP_ASSERT(TYPE, 0      == guardedAlloc.numBytesInUse());
                }

                // Non-null constraint.
                {
                    const RecDef *CONSTR = EType::BDEM_CHOICE       == TYPE
                                        || EType::BDEM_CHOICE_ARRAY == TYPE
                                         ? &T.record(0) : &T.record(1);

                          FldAttr  mFA(TYPE, N, FMT, Z);
                    const FldAttr&  FA = mFA;
                    LOOP_ASSERT(LINE, TYPE   == FA.elemType());
                    LOOP_ASSERT(LINE, FMT    == FA.formattingMode());
                    LOOP_ASSERT(LINE, N      == FA.isNullable());
                    LOOP_ASSERT(LINE, false  == FA.hasDefaultValue());

                    const FldDef *pFD = pR->appendField(FA, CONSTR);
                    LOOP_ASSERT(TYPE, 2 == pR->numFields());
                    LOOP_ASSERT(TYPE, pFD);

                    LOOP_ASSERT(TYPE, TYPE   == pFD->elemType());
                    LOOP_ASSERT(TYPE, CONSTR == pFD->recordConstraint());
                    LOOP_ASSERT(TYPE, FMT    == pFD->formattingMode());
                    LOOP_ASSERT(TYPE, N      == pFD->isNullable());
                    LOOP_ASSERT(TYPE, false  == pFD->hasDefaultValue());
                    LOOP_ASSERT(TYPE, 0      == guardedAlloc.numBytesInUse());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting default values." << bsl::endl;
        {
            const int  FMT = 55;
            const bool N   = false;

            for (int ti = 0; ti < EType::BDEM_NUM_TYPES; ++ti) {
                const EType::Type TYPE = (EType::Type)ti;
                if (EType::isAggregateType(TYPE)) {
                    continue;
                }

                Obj mS(Z);  const Obj& S = mS;

                RecDef *pR = mS.createRecord();  LOOP_ASSERT(TYPE, pR);
                LOOP_ASSERT(TYPE, 0 == pR->numFields());

                FldAttr mFA(TYPE, N, FMT, Z);  const FldAttr& FA = mFA;
                setDefaultValueForType(mFA.defaultValue());

                const FldDef *pFD = pR->appendField(FA);
                LOOP_ASSERT(TYPE, pFD);
                LOOP_ASSERT(TYPE, 1 == pR->numFields());

                LOOP_ASSERT(TYPE, true == pFD->hasDefaultValue());

                verifyDefaultValueForType(pFD->defaultValue());

                LOOP_ASSERT(TYPE, 0    == guardedAlloc.numBytesInUse());
            }
        }

      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING 'bdem_FieldSpec' PRINT METHOD
        //
        // Concerns:
        //   The 'print' method formats the value of a field spec as expected.
        //
        // Plan:
        //   Define a set S of varied field spec test values.  For each u in S,
        //   print u to a 'char' buffer and compare the contents of the buffer
        //   with the expected output format.
        //
        // Testing:
        //   ostream& print(ostream& stream, level, spl) const;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting 'bdem_FieldSpec' 'print' Method"
                               << "\n======================================="
                               << bsl::endl;

        {
            Obj mS(Z);  const Obj& S = mS;
            gg(&mS, ":a? :* :v=");
            ASSERT(2 == S.numRecords());
            ASSERT(1 == S.numEnumerations());

            const RecDef& RC = S.record(0);
            ASSERT(RecDef::BDEM_CHOICE_RECORD == RC.recordType());

            const RecDef& RS = S.record(1);
            ASSERT(RecDef::BDEM_SEQUENCE_RECORD == RS.recordType());

            const EnumDef& E = S.enumeration(0);

            static const struct {
                int           d_lineNum;         // source line number
                EType::Type   d_type;            // 'bdem' type
                const void   *d_constraint;      // constraint
                int           d_formattingMode;  // formatting mode
                bool          d_isNullable;      // nullable?
                bool          d_defaultValue;    // supply a default value?
                const char   *d_expected;        // expected output
            } DATA[] = {
                //line 'bdem' type     RecDef   fmt  null    dflt  expected
                //---- -----------     ------   ---  ----    ----  --------
                { L_,  EType::BDEM_INT,     0,        0,  false,  false,
                     "{ INT !nullable 0x0 }"                                 },
                { L_,  EType::BDEM_INT64,   0,        6,  true,   true,
                     "{ INT64 nullable 0x6 -100 }"                           },
                { L_,  EType::BDEM_DATETZ,  0,        0,  false,  true,
                     "{ DATETZ !nullable 0x0 01JAN2000-0005 }"               },
                { L_,  EType::BDEM_STRING,  0,      256,  false,  true,
                     "{ STRING !nullable 0x100 one }"                        },
                { L_,  EType::BDEM_LIST,    0,        0,  true,   false,
                     "{ LIST nullable 0x0 }"                                 },
                { L_,  EType::BDEM_LIST,    &RS,      5,  false,  false,
                     "{ LIST[RECORD CONSTRAINT] !nullable 0x5 }"             },
                { L_,  EType::BDEM_CHOICE,  0,        5,  true,   false,
                     "{ CHOICE nullable 0x5 }"                               },
                { L_,  EType::BDEM_CHOICE,  &RC,     77,  true,   false,
                     "{ CHOICE[RECORD CONSTRAINT] nullable 0x4d }"           },
                { L_,  EType::BDEM_INT,     &E,       0,  true,   false,
                     "{ INT[ENUM CONSTRAINT] nullable 0x0 }"                 },
                { L_,  EType::BDEM_STRING,  &E,     256,  false,  false,
                     "{ STRING[ENUM CONSTRAINT] !nullable 0x100 }"           },
                { L_,  EType::BDEM_INT_ARRAY,0,       0,  true,   false,
                     "{ INT_ARRAY nullable 0x0 }"                            },
                { L_,  EType::BDEM_INT_ARRAY,0,       0,  false,  true,
                     "{ INT_ARRAY !nullable 0x0 10 }"                        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE     = DATA[ti].d_lineNum;
                const EType::Type  TYPE     = DATA[ti].d_type;
                const void        *CONSTR   = DATA[ti].d_constraint;
                const int          FORMAT   = DATA[ti].d_formattingMode;
                const bool         ISNULL   = DATA[ti].d_isNullable;
                const bool         HASDFLT  = DATA[ti].d_defaultValue;
                const char        *EXPECTED = DATA[ti].d_expected;

                const RecDef* recConstr = EType::isAggregateType(TYPE) ?
                    static_cast<const RecDef*>(CONSTR) : 0;
                const EnumDef* enumConstr = EType::isAggregateType(TYPE) ?
                    0 : static_cast<const EnumDef*>(CONSTR);

                FldSpec        mFR(TYPE, recConstr, FORMAT, ISNULL, Z);
                FldSpec        mFE(TYPE, enumConstr, FORMAT, ISNULL, Z);
                FldSpec&       mF = recConstr ? mFR : mFE;
                FldSpec const&  F = mF;

                if (HASDFLT) {
                    ASSERT(! EType::isAggregateType(TYPE));
                    setDefaultValueForType(mF.defaultValue());
                }

                char buf[1500];
                bsl::memset(buf, 0xff, sizeof buf);     // Scribble on 'buf'.
                bsl::ostrstream out(buf, sizeof buf);

                LOOP_ASSERT(LINE, sizeof buf > bsl::strlen(EXPECTED));

                F.print(out, 0, -1);
                out << bsl::ends;

                LOOP3_ASSERT(LINE, EXPECTED, buf,
                             0 == bsl::strcmp(EXPECTED, buf));

                if (veryVerbose) {
                    bsl::cout << "EXPECTED: \'" << EXPECTED << "\'"
                              << bsl::endl;
                    bsl::cout << "ACTUAL  : \'" << buf << "\'" << bsl::endl;
                }
            }
        }

      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING 'bdem_FieldSpec' COPY CONSTRUCTOR
        //
        // Concerns:
        //   - The object constructed is identical in value to the object
        //     supplied as an argument.
        //   - The memory allocator is hooked up correctly.
        //
        // Plan:
        //   For each field spec F of a set of varied field specs, copy
        //   construct a field spec G from F.  Verify that the values of F and
        //   G are identical.  Using test allocators and a default allocator
        //   guard, verify that all memory is allocated through the supplied
        //   allocator.
        //
        // Testing:
        //   bdem_FieldSpec(const bdem_FieldSpec& original, *ba=0);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting 'bdem_FieldSpec' Copy Constructor"
                               << "\n========================================="
                               << bsl::endl;

        if (verbose) bsl::cout << "\nTesting 'bdem' scalar and array types."
                               << bsl::endl;
        {
            const int  FMT = 55;
            const bool N   = false;

            bslma_TestAllocator         guardedAlloc(veryVeryVerbose);
            bslma_DefaultAllocatorGuard guard(&guardedAlloc);

            for (int ti = 0; ti < EType::BDEM_NUM_TYPES; ++ti) {
                const EType::Type TYPE = (EType::Type)ti;

                if (EType::isAggregateType(TYPE)) {
                    continue;
                }

                {
                    FldSpec mF(TYPE, 0, FMT, N, Z);  const FldSpec& F = mF;
                    FldSpec mG(F, Z);                const FldSpec& G = mG;

                    LOOP2_ASSERT(TYPE,
                                 (F.print(bsl::cout), G.print(bsl::cout), 0),
                                 F == G);
                    LOOP_ASSERT(TYPE, !(F != G));
                    LOOP_ASSERT(TYPE, 0 == guardedAlloc.numBytesInUse());
                }

                {
                    FldSpec mF(TYPE, 0, FMT, N, Z);  const FldSpec& F = mF;
                    setDefaultValueForType(mF.defaultValue());
                    FldSpec mG(F, Z);                const FldSpec& G = mG;

                    LOOP_ASSERT(TYPE, F == G);
                    LOOP_ASSERT(TYPE, !(F != G));
                    if (TYPE != EType::BDEM_BOOL
                     && TYPE != EType::BDEM_BOOL_ARRAY) {
                        // Bool type has only one non-null value.
                        setDefaultValueForType(mF.defaultValue(), 1);
                        LOOP_ASSERT(TYPE, !(F== G));
                        LOOP_ASSERT(TYPE, F != G);
                    }
                    LOOP_ASSERT(TYPE, 0 == guardedAlloc.numBytesInUse());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdem' aggregate types."
                               << bsl::endl;
        {
            Obj mS(Z);  const Obj& S = mS;
            gg(&mS, ":a? :b");  ASSERT(2 == S.numRecords());

            const RecDef& RC = S.record(0);
            ASSERT(RecDef::BDEM_CHOICE_RECORD   == RC.recordType());
            const RecDef& RS = S.record(1);
            ASSERT(RecDef::BDEM_SEQUENCE_RECORD == RS.recordType());

            const int FMT = 55;

            static const struct {
                int           d_lineNum;     // source line number
                EType::Type   d_type;        // 'bdem' type
                const RecDef *d_constraint;  // constraint
            } DATA[] = {
                //line  'bdem' type            constraint
                //----  -----------            ----------
                { L_,   EType::BDEM_LIST,           0                 },
                { L_,   EType::BDEM_LIST,           &RS               },
                { L_,   EType::BDEM_TABLE,          0                 },
                { L_,   EType::BDEM_TABLE,          &RS               },
                { L_,   EType::BDEM_CHOICE,         0                 },
                { L_,   EType::BDEM_CHOICE,         &RC               },
                { L_,   EType::BDEM_CHOICE_ARRAY,   0                 },
                { L_,   EType::BDEM_CHOICE_ARRAY,   &RC               },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE   = DATA[ti].d_lineNum;
                const EType::Type  TYPE   = DATA[ti].d_type;
                const RecDef      *CONSTR = DATA[ti].d_constraint;

                const bool N = EType::BDEM_LIST == TYPE ? true : false;

                FldSpec mF(TYPE, CONSTR, FMT, N, Z);  const FldSpec& F = mF;
                FldSpec mG(F, Z);                     const FldSpec& G = mG;

                LOOP_ASSERT(LINE, F == G);
                LOOP_ASSERT(LINE, !(F != G));
            }
        }

      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING 'bdem_FieldSpec' RESET AND SETTER METHODS
        //
        // Concerns:
        //   - The 'reset' and "setter" methods set the attributes of a field
        //     spec to the expected values.
        //   - The 'reset' methods set the default value (for scalar types) to
        //     the null value.
        //   - The 'reset' and "setter" methods return the expected status
        //     value.
        //   - If the methods fail, the field specs are left in their original
        //     state.
        //
        // Plan:
        //   The 'reset' and "setter" methods, supplied with various valid and
        //   invalid arguments, are applied to varied field specs.  Valid and
        //   invalid cases are tested separately.  Field spec accessors are
        //   used extensively to verify that the methods under test set the
        //   attributes to the expected value.  Also verified is that the
        //   methods have no effect upon failure.
        //
        //   Note that the only "setter" method that can fail is
        //   'setConstraint', and the 'reset' methods can fail only if the
        //   specified constraint is invalid for the specified element type.
        //
        // Testing:
        //   int reset(EType type, const bdem_RecordDef *c=0);
        //   int reset(EType type, *c, formattingMode, isNullable);
        //   int setConstraint(const bdem_RecordDef *constraint);
        //   int setFormattingMode(int formattingMode);
        //   int setIsNullable(bool isNullable);
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << "\nTesting 'bdem_FieldSpec' 'reset' & Setter Methods"
                      << "\n================================================="
                      << bsl::endl;

        Obj mS(Z);  const Obj& S = mS;
        gg(&mS, ":a? :b? :c :d :e= :f=");
        ASSERT(4 == S.numRecords());
        ASSERT(2 == S.numEnumerations());

        const RecDef& RC1 = S.record(0);
        const RecDef& RC2 = S.record(1);
        ASSERT(RecDef::BDEM_CHOICE_RECORD == RC1.recordType());
        ASSERT(RecDef::BDEM_CHOICE_RECORD == RC2.recordType());
        ASSERT(&RC1 != &RC2);

        const RecDef& RS1 = S.record(2);
        const RecDef& RS2 = S.record(3);
        ASSERT(RecDef::BDEM_SEQUENCE_RECORD == RS1.recordType());
        ASSERT(RecDef::BDEM_SEQUENCE_RECORD == RS2.recordType());
        ASSERT(&RS1 != &RS2);

        const EnumDef& E1 = S.enumeration(0);
        const EnumDef& E2 = S.enumeration(1);
        ASSERT(&E1 != &E2)

        const int FMT = 55;

        if (verbose)
            bsl::cout << "\nTesting 2-argument 'reset' method (valid)."
                      << bsl::endl;
        {
            for (int ti = 0; ti < EType::BDEM_NUM_TYPES; ++ti) {

                const EType::Type ITYPE = (EType::Type)ti;

                const RecDef  *IRCNSTR = 0;
                const EnumDef *IECNSTR = 0;
                if (EType::isAggregateType(ITYPE)) {
                    IRCNSTR = EType::BDEM_CHOICE       == ITYPE
                           || EType::BDEM_CHOICE_ARRAY == ITYPE ? &RC1 : &RS1;
                }
                else if (EnumDef::canHaveEnumConstraint(ITYPE)) {
                    IECNSTR = &E1;
                }

                for (int tj = 0; tj < EType::BDEM_NUM_TYPES; ++tj) {
                    const EType::Type JTYPE = (EType::Type)tj;

                    const RecDef  *JRCNSTR = 0;
                    const EnumDef *JECNSTR = 0;
                    if (EType::isAggregateType(JTYPE)) {
                        JRCNSTR = EType::BDEM_CHOICE       == JTYPE
                               || EType::BDEM_CHOICE_ARRAY == JTYPE ? &RC2
                                                                    : &RS2;
                    }
                    else if (EnumDef::canHaveEnumConstraint(JTYPE)) {
                        JECNSTR = &E2;
                    }

                    const int  IFMTMD = 55;
                    const bool INULL = false;

                    FldSpec  mF(IRCNSTR ?
                                FldSpec(ITYPE, IRCNSTR, IFMTMD, INULL, Z) :
                                FldSpec(ITYPE, IECNSTR, IFMTMD, INULL, Z) );
                    const FldSpec&  F = mF;
                    LOOP2_ASSERT(ITYPE, JTYPE,
                                 ITYPE   == F.elemType());
                    LOOP2_ASSERT(ITYPE, JTYPE,
                                 IRCNSTR == F.recordConstraint());
                    LOOP2_ASSERT(ITYPE, JTYPE,
                                 IECNSTR == F.enumerationConstraint());
                    LOOP2_ASSERT(ITYPE, JTYPE,
                                 IFMTMD  == F.formattingMode());
                    LOOP2_ASSERT(ITYPE, JTYPE,
                                 INULL   == F.isNullable());

                    FldSpec  mG(IRCNSTR ?
                                FldSpec(ITYPE, IRCNSTR, IFMTMD, INULL, Z) :
                                FldSpec(ITYPE, IECNSTR, IFMTMD, INULL, Z) );
                    const FldSpec&  G = mG;
                    LOOP2_ASSERT(ITYPE, JTYPE,
                                 ITYPE   == G.elemType());
                    LOOP2_ASSERT(ITYPE, JTYPE,
                                 IRCNSTR == G.recordConstraint());
                    LOOP2_ASSERT(ITYPE, JTYPE,
                                 IECNSTR == G.enumerationConstraint());
                    LOOP2_ASSERT(ITYPE, JTYPE,
                                 IFMTMD  == G.formattingMode());
                    LOOP2_ASSERT(ITYPE, JTYPE,
                                 INULL   == G.isNullable());

                    if (! EType::isAggregateType(ITYPE)) {
                        setDefaultValueForType(mF.defaultValue());
                        setDefaultValueForType(mG.defaultValue(), true);
                        LOOP2_ASSERT(ITYPE, JTYPE,
                                 true == F.hasDefaultValue());
                        LOOP2_ASSERT(ITYPE, JTYPE,
                                 true == G.hasDefaultValue());
                    }

                    LOOP_ASSERT(ITYPE, 0      == mF.reset(JTYPE));
                    LOOP_ASSERT(ITYPE, JTYPE  == F.elemType());
                    LOOP_ASSERT(ITYPE, 0      == F.recordConstraint());
                    LOOP_ASSERT(ITYPE, 0      == F.formattingMode());
                    LOOP_ASSERT(ITYPE, INULL  == F.isNullable());

                    int resetRet = -99;
                    if (JRCNSTR) {
                        resetRet = mG.reset(JTYPE, JRCNSTR);
                    }
                    else {
                        resetRet = mG.reset(JTYPE, JECNSTR);
                    }
                    LOOP_ASSERT(ITYPE, 0       == resetRet);
                    LOOP_ASSERT(ITYPE, JTYPE   == G.elemType());
                    LOOP_ASSERT(ITYPE, JRCNSTR == G.recordConstraint());
                    LOOP_ASSERT(ITYPE, JECNSTR == G.enumerationConstraint());
                    LOOP_ASSERT(ITYPE, 0       == G.formattingMode());
                    LOOP_ASSERT(ITYPE, INULL   == G.isNullable());

                    if (EType::isScalarType(JTYPE)) {
                        LOOP2_ASSERT(ITYPE, JTYPE,
                                 false == F.hasDefaultValue());
                        LOOP2_ASSERT(ITYPE, JTYPE,
                                 false == G.hasDefaultValue());
                    }
                }
            }
        }

        if (verbose)
            bsl::cout << "\nTesting 4-argument 'reset' method (valid)."
                      << bsl::endl;
        {
            for (int ti = 0; ti < EType::BDEM_NUM_TYPES; ++ti) {

                const EType::Type ITYPE = (EType::Type)ti;

                const RecDef *ICNSTR;
                if (EType::isAggregateType(ITYPE)) {
                    ICNSTR = EType::BDEM_CHOICE       == ITYPE
                          || EType::BDEM_CHOICE_ARRAY == ITYPE ? &RC1 : &RS1;
                }
                else {
                    ICNSTR = 0;
                }

                for (int tj = 0; tj < EType::BDEM_NUM_TYPES; ++tj) {
                    const EType::Type JTYPE = (EType::Type)tj;

                    const RecDef *JCNSTR;
                    if (EType::isAggregateType(JTYPE)) {
                        JCNSTR = EType::BDEM_CHOICE       == JTYPE
                              || EType::BDEM_CHOICE_ARRAY == JTYPE ? &RC2
                                                                   : &RS2;
                    }
                    else {
                        JCNSTR = 0;
                    }

                    const int  IFMTMD = 55;
                    const bool INULL = EType::BDEM_LIST == JTYPE ? true
                                                                 : false;

                    const int  JFMT  = 44;
                    const bool JNULL = true;

                          FldSpec  mF(ITYPE, ICNSTR, IFMTMD, INULL, Z);
                    const FldSpec&  F = mF;
                    LOOP2_ASSERT(ITYPE, JTYPE,
                                 ITYPE  == F.elemType());
                    LOOP2_ASSERT(ITYPE, JTYPE,
                                 ICNSTR == F.recordConstraint());
                    LOOP2_ASSERT(ITYPE, JTYPE,
                                 IFMTMD == F.formattingMode());
                    LOOP2_ASSERT(ITYPE, JTYPE,
                                 INULL  == F.isNullable());

                    if (! EType::isAggregateType(ITYPE)) {
                        setDefaultValueForType(mF.defaultValue());
                        LOOP2_ASSERT(ITYPE, JTYPE,
                                 true == F.hasDefaultValue());
                    }

                    LOOP_ASSERT(ITYPE,
                                0 == mF.reset(JTYPE, JCNSTR, JFMT, JNULL));
                    LOOP_ASSERT(ITYPE, JTYPE  == F.elemType());
                    LOOP_ASSERT(ITYPE, JCNSTR == F.recordConstraint());
                    LOOP_ASSERT(ITYPE, JFMT   == F.formattingMode());
                    LOOP_ASSERT(ITYPE, JNULL  == F.isNullable());

                    if (! EType::isAggregateType(ITYPE)) {
                        LOOP2_ASSERT(ITYPE, JTYPE,
                                 false == F.hasDefaultValue());
                    }
                }
            }
        }

        if (verbose)
            bsl::cout << "\nTesting 'reset' methods (invalid)." << bsl::endl;
        {
            for (int ti = 0; ti < EType::BDEM_NUM_TYPES; ++ti) {

                const EType::Type ITYPE = (EType::Type)ti;

                const RecDef *ICNSTR;
                if (EType::isAggregateType(ITYPE)) {
                    ICNSTR = EType::BDEM_CHOICE       == ITYPE
                          || EType::BDEM_CHOICE_ARRAY == ITYPE ? &RC1 : &RS1;
                }
                else {
                    ICNSTR = 0;
                }

                const int  IFMTMD = 55;
                const bool INULL = true;

                      FldSpec  mF(ITYPE, ICNSTR, IFMTMD, INULL, Z);
                const FldSpec&  F = mF;
                LOOP_ASSERT(ITYPE, ITYPE  == F.elemType());
                LOOP_ASSERT(ITYPE, ICNSTR == F.recordConstraint());
                LOOP_ASSERT(ITYPE, IFMTMD == F.formattingMode());
                LOOP_ASSERT(ITYPE, INULL  == F.isNullable());

                if (! EType::isAggregateType(ITYPE)) {
                    setDefaultValueForType(mF.defaultValue());
                    LOOP_ASSERT(ITYPE, true == F.hasDefaultValue());
                }

                FldSpec mG(mF, Z);  const FldSpec&  G = mG;  // control
                ASSERT(F == G);

                for (int tj = 0; tj < EType::BDEM_NUM_TYPES; ++tj) {
                    const EType::Type JTYPE = (EType::Type)tj;

                    const RecDef *JCNSTR;
                    if (EType::isAggregateType(JTYPE)) {
                        // Constraints can be set for aggregate types.

                        continue;
                    }
                    else {
                        JCNSTR = &RC2;  // non-null for non-aggregate type
                    }

                    const int  JFMT  = 44;
                    const bool JNULL = !INULL;

                    LOOP3_ASSERT(ITYPE, JTYPE, JCNSTR,
                                 0 != mF.reset(JTYPE, JCNSTR));
                    ASSERT(F == G);

                    LOOP2_ASSERT(ti, tj,
                                 0 != mF.reset(JTYPE, JCNSTR, JFMT, JNULL));
                    ASSERT(F == G);
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting setters." << bsl::endl;

        if (verbose) bsl::cout << "\nTesting 'bdem' scalar and array types."
                               << bsl::endl;
        {
            const bool N = false;

            for (int ti = 0; ti < EType::BDEM_NUM_TYPES; ++ti) {
                const EType::Type TYPE = (EType::Type)ti;
                if (EType::isAggregateType(TYPE)) {
                    continue;
                }

                bool hasDflt = false;

                FldSpec mF(TYPE, 0, FMT, N, Z);  const FldSpec& F = mF;
                LOOP_ASSERT(TYPE, TYPE    == F.elemType());
                LOOP_ASSERT(TYPE, 0       == F.recordConstraint());
                LOOP_ASSERT(TYPE, FMT     == F.formattingMode());
                LOOP_ASSERT(TYPE, N       == F.isNullable());

                if (! EType::isAggregateType(TYPE)) {
                    setDefaultValueForType(mF.defaultValue());
                    hasDflt = true;
                }
                LOOP_ASSERT(TYPE, hasDflt == F.hasDefaultValue());

                LOOP_ASSERT(TYPE, 0       == mF.setConstraint(0));
                mF.setFormattingMode(-FMT);
                mF.setIsNullable(!N);

                LOOP_ASSERT(TYPE, TYPE    == F.elemType());
                LOOP_ASSERT(TYPE, 0       == F.recordConstraint());
                LOOP_ASSERT(TYPE, -FMT    == F.formattingMode());
                LOOP_ASSERT(TYPE, !N      == F.isNullable());
                LOOP_ASSERT(TYPE, hasDflt == F.hasDefaultValue());

                mF.setFormattingMode(FMT);
                mF.setIsNullable(N);

                LOOP_ASSERT(TYPE, TYPE    == F.elemType());
                LOOP_ASSERT(TYPE, 0       == F.recordConstraint());
                LOOP_ASSERT(TYPE, FMT     == F.formattingMode());
                LOOP_ASSERT(TYPE, N       == F.isNullable());
                LOOP_ASSERT(TYPE, hasDflt == F.hasDefaultValue());

                LOOP_ASSERT(TYPE, 0       != mF.setConstraint(&RS1));
                LOOP_ASSERT(TYPE, TYPE    == F.elemType());
                LOOP_ASSERT(TYPE, 0       == F.recordConstraint());
                LOOP_ASSERT(TYPE, FMT     == F.formattingMode());
                LOOP_ASSERT(TYPE, N       == F.isNullable());
                LOOP_ASSERT(TYPE, hasDflt == F.hasDefaultValue());

                LOOP_ASSERT(TYPE, 0       != mF.setConstraint(&RC1));
                LOOP_ASSERT(TYPE, TYPE    == F.elemType());
                LOOP_ASSERT(TYPE, 0       == F.recordConstraint());
                LOOP_ASSERT(TYPE, FMT     == F.formattingMode());
                LOOP_ASSERT(TYPE, N       == F.isNullable());
                LOOP_ASSERT(TYPE, hasDflt == F.hasDefaultValue());
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdem' aggregate types (valid)."
                               << bsl::endl;
        {
            static const struct {
                int           d_lineNum;            // source line number
                EType::Type   d_type;               // 'bdem' type
                const RecDef *d_orignalConstraint;  // original constraint
                const RecDef *d_newConstraint;      // new constraint
            } DATA[] = {
                //                            <- constraint ->
                //line  'bdem' type           original     new
                //----  -----------           --------     ---
                { L_,   EType::BDEM_LIST,          0,           &RS1         },
                { L_,   EType::BDEM_LIST,          &RS1,        &RS2         },
                { L_,   EType::BDEM_LIST,          &RS2,        0            },
                { L_,   EType::BDEM_TABLE,         0,           &RS1         },
                { L_,   EType::BDEM_TABLE,         &RS1,        &RS2         },
                { L_,   EType::BDEM_TABLE,         &RS2,        0            },
                { L_,   EType::BDEM_CHOICE,        0,           &RC1         },
                { L_,   EType::BDEM_CHOICE,        &RC1,        &RC2         },
                { L_,   EType::BDEM_CHOICE,        &RC2,        0            },
                { L_,   EType::BDEM_CHOICE_ARRAY,  0,           &RC1         },
                { L_,   EType::BDEM_CHOICE_ARRAY,  &RC1,        &RC2         },
                { L_,   EType::BDEM_CHOICE_ARRAY,  &RC2,        0            },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE   = DATA[ti].d_lineNum;
                const EType::Type  TYPE   = DATA[ti].d_type;
                const RecDef      *OCNSTR = DATA[ti].d_orignalConstraint;
                const RecDef      *NCNSTR = DATA[ti].d_newConstraint;

                const bool N = EType::BDEM_LIST == TYPE ? true : false;

                FldSpec mF(TYPE, OCNSTR, FMT, N, Z);  const FldSpec& F = mF;
                LOOP_ASSERT(LINE, TYPE   == F.elemType());
                LOOP_ASSERT(LINE, OCNSTR == F.recordConstraint());
                LOOP_ASSERT(LINE, FMT    == F.formattingMode());
                LOOP_ASSERT(LINE, N      == F.isNullable());

                LOOP_ASSERT(LINE, 0      == mF.setConstraint(NCNSTR));
                mF.setFormattingMode(-FMT);
                mF.setIsNullable(!N);

                LOOP_ASSERT(LINE, TYPE   == F.elemType());
                LOOP_ASSERT(LINE, NCNSTR == F.recordConstraint());
                LOOP_ASSERT(LINE, -FMT   == F.formattingMode());
                LOOP_ASSERT(LINE, !N     == F.isNullable());

                LOOP_ASSERT(LINE, 0      == mF.setConstraint(OCNSTR));
                mF.setFormattingMode(FMT);
                mF.setIsNullable(N);

                LOOP_ASSERT(LINE, TYPE   == F.elemType());
                LOOP_ASSERT(LINE, OCNSTR == F.recordConstraint());
                LOOP_ASSERT(LINE, FMT    == F.formattingMode());
                LOOP_ASSERT(LINE, N      == F.isNullable());
            }
        }

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'bdem_FieldSpec' ASSIGNMENT OPERATOR
        //
        // Concerns:
        //   We are concerned that any field spec value can be assigned to any
        //   other field spec value.
        //
        // Plan:
        //   Define a set S of varied field spec test values.  For each (u, v)
        //   in S x S, verify that u = v produces the expected results.
        //   Exception neutrality is tested; this is relevant to 'STRING'
        //   default values.
        //
        // Testing:
        //   bdem_FieldSpec& operator=(const bdem_FieldSpec& rhs);
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << "\nTesting 'bdem_FieldSpec' Assignment Operator"
                      << "\n============================================"
                      << bsl::endl;

        {
            Obj mS(Z);  const Obj& S = mS;
            gg(&mS, ":a? :*");  ASSERT(2 == S.numRecords());

            const RecDef& RC = S.record(0);
            ASSERT(RecDef::BDEM_CHOICE_RECORD == RC.recordType());

            const RecDef& RS = S.record(1);
            ASSERT(RecDef::BDEM_SEQUENCE_RECORD == RS.recordType());

            static const struct {
                int           d_lineNum;         // source line number
                EType::Type   d_type;            // 'bdem' type
                const RecDef *d_constraint;      // constraint
                int           d_formattingMode;  // formatting mode
                bool          d_isNullable;      // nullable?
                bool          d_defaultValue;    // supply a default value?
            } DATA[] = {
                //line 'bdem' type     RecDef  fmt  null    has dflt val
                //---- -----------     ------  ---  ----    --------------
                { L_,  EType::BDEM_INT,     0,       0,  true,   false     },
                { L_,  EType::BDEM_INT,     0,       4,  false,  true      },

                { L_,  EType::BDEM_DATE,    0,       0,  true,   true      },
                { L_,  EType::BDEM_DATETZ,  0,       0,  true,   true      },

                { L_,  EType::BDEM_STRING,  0,       5,  false,  false     },
                { L_,  EType::BDEM_STRING,  0,      -1,  true,   true      },

                { L_,  EType::BDEM_SHORT_ARRAY,0,    0,  true,   false     },
                { L_,  EType::BDEM_SHORT_ARRAY,0,    4,  false,  true      },

                { L_,  EType::BDEM_LIST,    &RS,     0,  false,  false     },
                { L_,  EType::BDEM_TABLE,   0,       3,  true,   false     },

                { L_,  EType::BDEM_CHOICE,  0,       5,  true,   false     },
                { L_,  EType::BDEM_CHOICE,  &RC,    77,  false,  false     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          ILINE    = DATA[ti].d_lineNum;
                const EType::Type  ITYPE    = DATA[ti].d_type;
                const RecDef      *ICONSTR  = DATA[ti].d_constraint;
                const int          IFORMAT  = DATA[ti].d_formattingMode;
                const bool         IISNULL  = DATA[ti].d_isNullable;
                const bool         IHASDFLT = DATA[ti].d_defaultValue;

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int          JLINE    = DATA[tj].d_lineNum;
                    const EType::Type  JTYPE    = DATA[tj].d_type;
                    const RecDef      *JCONSTR  = DATA[tj].d_constraint;
                    const int          JFORMAT  = DATA[tj].d_formattingMode;
                    const bool         JISNULL  = DATA[tj].d_isNullable;
                    const bool         JHASDFLT = DATA[tj].d_defaultValue;

                          FldSpec  mF(ITYPE, ICONSTR, IFORMAT, IISNULL, Z);
                    const FldSpec&  F = mF;

                          FldSpec  mW(ITYPE, ICONSTR, IFORMAT, IISNULL, Z);
                    const FldSpec&  W = mW;  // control

                    if (IHASDFLT) {
                        ASSERT(! EType::isAggregateType(ITYPE));
                        setDefaultValueForType(mF.defaultValue());
                        setDefaultValueForType(mW.defaultValue());
                    }

                    LOOP2_ASSERT(ILINE, JLINE,  1 == (F == W));
                    LOOP2_ASSERT(ILINE, JLINE,  0 == (F != W));

                    FldSpec        mG(JTYPE, JCONSTR, JFORMAT, JISNULL, Z);
                    FldSpec const&  G = mG;

                    if (JHASDFLT) {
                        ASSERT(! EType::isAggregateType(JTYPE));
                        setDefaultValueForType(mG.defaultValue(), true);
                    }

                    const bool fgAreSame = ti == tj && !JHASDFLT;
                    LOOP2_ASSERT(ILINE, JLINE,  fgAreSame == (F == G));
                    LOOP2_ASSERT(ILINE, JLINE, !fgAreSame == (F != G));

                    BEGIN_BSLMA_EXCEPTION_TEST {
                      mF = G;
                    } END_BSLMA_EXCEPTION_TEST

                    LOOP2_ASSERT(ILINE, JLINE,          1 == (F == G));
                    LOOP2_ASSERT(ILINE, JLINE,          0 == (F != G));

                    LOOP2_ASSERT(ILINE, JLINE,  fgAreSame == (F == W));
                    LOOP2_ASSERT(ILINE, JLINE, !fgAreSame == (F != W));
                }
            }
        }

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'bdem_FieldSpec' EQUALITY OPERATORS
        //
        // Concerns:
        //   The equality operators return the correct results.
        //
        // Plan:
        //   Specify a set S of field specs containing two "baseline" values
        //   (one constrained and one unconstrained) and all necessary
        //   perturbations of those values (plus a few extras) such that each
        //   value in S is unique.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product S x S.
        //
        // Testing:
        //   operator==(const bdem_FieldSpec& lhs, const bdem_FieldSpec& rhs);
        //   operator!=(const bdem_FieldSpec& lhs, const bdem_FieldSpec& rhs);
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << "\nTesting 'bdem_FieldSpec' Equality Operators"
                      << "\n==========================================="
                      << bsl::endl;

        {
            Obj mS(Z);  const Obj& S = mS;
            gg(&mS, ":a? :*");  ASSERT(2 == S.numRecords());

            const RecDef& RC = S.record(0);
            ASSERT(RecDef::BDEM_CHOICE_RECORD == RC.recordType());

            const RecDef& RS = S.record(1);
            ASSERT(RecDef::BDEM_SEQUENCE_RECORD == RS.recordType());

            static const struct {
                int           d_lineNum;         // source line number
                EType::Type   d_type;            // 'bdem' type
                const RecDef *d_constraint;      // constraint
                int           d_formattingMode;  // formatting mode
                bool          d_isNullable;      // nullable?
                bool          d_defaultValue;    // supply a default value?
            } DATA[] = {
                //line 'bdem' type     RecDef  fmt  null    has dflt val
                //---- -----------     ------  ---  ----    --------------
                { L_,  EType::BDEM_INT,     0,       0,  false,  false       },
                { L_,  EType::BDEM_FLOAT,   0,       0,  false,  false       },
                { L_,  EType::BDEM_INT,     0,       4,  false,  false       },
                { L_,  EType::BDEM_INT,     0,       0,  true,   false       },
                { L_,  EType::BDEM_INT,     0,       0,  false,  true        },

                { L_,  EType::BDEM_INT_ARRAY,0,      0,  true,   false       },
                { L_,  EType::BDEM_INT_ARRAY,0,      0,  false,  true        },

                { L_,  EType::BDEM_LIST,    &RS,     0,  false,  false       },
                { L_,  EType::BDEM_TABLE,   &RS,     0,  false,  false       },
                { L_,  EType::BDEM_LIST,    0,       0,  false,  false       },
                { L_,  EType::BDEM_LIST,    &RS,     4,  false,  false       },
                { L_,  EType::BDEM_LIST,    &RS,     0,  true,   false       },

                { L_,  EType::BDEM_DATETZ,  0,       0,  true,   true        },
                { L_,  EType::BDEM_STRING,  0,      -1,  false,  true        },

                { L_,  EType::BDEM_CHOICE,  0,       5,  true,   false       },
                { L_,  EType::BDEM_CHOICE,  &RC,    77,  true,   false       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          ILINE    = DATA[ti].d_lineNum;
                const EType::Type  ITYPE    = DATA[ti].d_type;
                const RecDef      *ICONSTR  = DATA[ti].d_constraint;
                const int          IFORMAT  = DATA[ti].d_formattingMode;
                const bool         IISNULL  = DATA[ti].d_isNullable;
                const bool         IHASDFLT = DATA[ti].d_defaultValue;

                      FldSpec  mF(ITYPE, ICONSTR, IFORMAT, IISNULL, Z);
                const FldSpec&  F = mF;

                if (IHASDFLT) {
                    ASSERT(! EType::isAggregateType(ITYPE));
                    setDefaultValueForType(mF.defaultValue());
                }

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int          JLINE    = DATA[tj].d_lineNum;
                    const EType::Type  JTYPE    = DATA[tj].d_type;
                    const RecDef      *JCONSTR  = DATA[tj].d_constraint;
                    const int          JFORMAT  = DATA[tj].d_formattingMode;
                    const bool         JISNULL  = DATA[tj].d_isNullable;
                    const bool         JHASDFLT = DATA[tj].d_defaultValue;

                          FldSpec  mG(JTYPE, JCONSTR, JFORMAT, JISNULL, Z);
                    const FldSpec&  G = mG;

                          FldSpec  mH(JTYPE, JCONSTR, JFORMAT, JISNULL, Z);
                    const FldSpec&  H = mH;

                    if (JHASDFLT) {
                        ASSERT(! EType::isAggregateType(JTYPE));
                        setDefaultValueForType(mG.defaultValue());
                        setDefaultValueForType(mH.defaultValue(), true);
                    }

                    const bool fgAreSame = ti == tj;
                    LOOP2_ASSERT(ILINE, JLINE,  fgAreSame == (F == G));
                    LOOP2_ASSERT(ILINE, JLINE, !fgAreSame == (F != G));

                    const bool fhAreSame = ti == tj && !JHASDFLT;
                    LOOP2_ASSERT(ILINE, JLINE,  fhAreSame == (F == H));
                    LOOP2_ASSERT(ILINE, JLINE, !fhAreSame == (F != H));

                    const bool ghAreSame = !JHASDFLT;
                    LOOP2_ASSERT(ILINE, JLINE,  ghAreSame == (G == H));
                    LOOP2_ASSERT(ILINE, JLINE, !ghAreSame == (G != H));
                }
            }
        }

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'bdem_FieldSpec' DEFAULT VALUES
        //
        // Concerns:
        //   For each 'bdem' scalar type, T:
        //     - Upon construction, a field spec for T has the null default
        //       value.
        //     - The field spec for T can be modified to hold non-null default
        //       values.
        //     - The field spec for T can be reset to hold the null default
        //       value.
        //
        // Plan:
        //   For each 'bdem' scalar type, T:
        //     - Construct a field spec for T.
        //     - Verify that the default value is null upon construction.
        //     - Alternately set the default value to a couple of
        //       representative non-null values.
        //     - Reset the default value to null.
        //     - Verify that the default value held by the field spec in each
        //       step is the value that is expected.
        //     - Using test allocators and a default allocator guard, verify
        //       that all memory is allocated through the supplied allocator.
        //
        // Testing:
        //   bdem_ElemRef defaultValue();
        //   bdem_ConstElemRef defaultValue() const;
        //   bool hasDefaultValue() const;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting 'bdem_FieldSpec' Default Values"
                               << "\n======================================="
                               << bsl::endl;

        {
            const int NUM_NON_AGGREGATES = 28;
            int numNonAggregatesTested  = 0;

            bslma_TestAllocator         guardedAlloc(veryVeryVerbose);
            bslma_DefaultAllocatorGuard guard(&guardedAlloc);

            for (int ti = 0; ti < EType::BDEM_NUM_TYPES; ++ti) {
                const EType::Type TYPE = (EType::Type)ti;
                if (EType::isAggregateType(TYPE)) {
                    continue;
                }
                ++numNonAggregatesTested;

                FldSpec mF(TYPE, 0, Z);  const FldSpec& F = mF;
                LOOP_ASSERT(TYPE, TYPE  == F.elemType());
                LOOP_ASSERT(TYPE, false == F.hasDefaultValue());

                CERef CER =  F.defaultValue();
                ERef  ER  = mF.defaultValue();

                LOOP_ASSERT(TYPE, CER.type() == ER.type());
                LOOP_ASSERT(TYPE, EType::isScalarType(CER.type()));
                LOOP2_ASSERT(TYPE, CER.type(), CER.type() == TYPE
                          || CER.type() == EType::fromArrayType(TYPE));

                LOOP_ASSERT(TYPE, 1     == isUnset(CER));

                setDefaultValueForType(ER);
                LOOP_ASSERT(TYPE, true  == F.hasDefaultValue());
                LOOP_ASSERT(TYPE, true  == verifyDefaultValueForType(CER));
                LOOP_ASSERT(TYPE, 0     == isUnset(CER));
                LOOP_ASSERT(TYPE, 0     == guardedAlloc.numBytesInUse());

                setDefaultValueForType(ER, true);
                LOOP_ASSERT(TYPE, true  == F.hasDefaultValue());
                LOOP_ASSERT(TYPE, true  == verifyDefaultValueForType(CER,
                                                                     true));
                const bool isBool = EType::BDEM_BOOL       == TYPE
                                 || EType::BDEM_BOOL_ARRAY == TYPE;
                LOOP_ASSERT(TYPE, isBool == isUnset(CER));
                LOOP_ASSERT(TYPE, 0      == guardedAlloc.numBytesInUse());

                ER.makeNull();
                LOOP_ASSERT(TYPE, false == F.hasDefaultValue());
                LOOP_ASSERT(TYPE, 1     == isUnset(CER));
            }
            ASSERT(NUM_NON_AGGREGATES == numNonAggregatesTested);
        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'bdem_FieldSpec' FIVE-ARGUMENT CONSTRUCTOR
        //
        // Concerns:
        //   - Field specs created using the 5-argument constructor have
        //     attributes with the expected value.
        //   - The memory allocator is hooked up correctly.
        //   - The destructor frees all memory internally allocated by the
        //     object.
        //
        // Plan:
        //   Create field specs having varied 'bdem' types, formatting modes,
        //   and nullabilities.  For aggregate types, test both unconstrained
        //   and constrained variants.  Assert that the attributes of the field
        //   specs have the expected values.  In particular, the formatting
        //   mode and nullability attributes should have the supplied values.
        //   Provide an explicit test, using test allocators and a default
        //   allocator guard, to verify that the memory allocator is hooked up
        //   correctly and that the destructor deallocates all memory.  This
        //   latter test is white-box: only 'STRING' is tested because it is
        //   the only 'bdem' type which uses the allocator supplied at
        //   construction.
        //
        // Testing:
        //   bdem_FieldSpec(EType type, *c, formattingMode, isNullable, *ba=0);
        //   int formattingMode() const;
        //   bool isNullable() const;
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << "\nTesting 'bdem_FieldSpec' 5-Argument Constructor"
                      << "\n==============================================="
                      << bsl::endl;

        if (verbose) bsl::cout << "\nTesting unconstrained 'bdem' types."
                               << bsl::endl;
        {
            for (int ti = 0; ti < EType::BDEM_NUM_TYPES; ++ti) {
                const EType::Type TYPE = (EType::Type)ti;

                const int FMT1 = 55;
                const int FMT2 = 66;

                const bool NT = true;
                const bool NF = !NT;  // 'false'

                {
                    FldSpec mF(TYPE, 0, FMT1, NT, Z);  const FldSpec& F = mF;
                    LOOP_ASSERT(TYPE, TYPE  == F.elemType());
                    LOOP_ASSERT(TYPE, 0     == F.recordConstraint());
                    LOOP_ASSERT(TYPE, FMT1  == F.formattingMode());
                    LOOP_ASSERT(TYPE, NT    == F.isNullable());
                    LOOP_ASSERT(TYPE, false == F.hasDefaultValue());
                }

                {
                    FldSpec mF(TYPE, 0, FMT2, NF, Z);  const FldSpec& F = mF;
                    LOOP_ASSERT(TYPE, TYPE  == F.elemType());
                    LOOP_ASSERT(TYPE, 0     == F.recordConstraint());
                    LOOP_ASSERT(TYPE, FMT2  == F.formattingMode());
                    LOOP_ASSERT(TYPE, NF    == F.isNullable());
                    LOOP_ASSERT(TYPE, false == F.hasDefaultValue());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting constrained 'bdem' types."
                               << bsl::endl;
        {
            Obj mS(Z);  const Obj& S = mS;
            gg(&mS, ":a? :b");  ASSERT(2 == S.numRecords());

            const RecDef& RC = S.record(0);
            ASSERT(RecDef::BDEM_CHOICE_RECORD   == RC.recordType());

            const RecDef& RS = S.record(1);
            ASSERT(RecDef::BDEM_SEQUENCE_RECORD == RS.recordType());

            static const struct {
                int           d_lineNum;     // source line number
                EType::Type   d_type;        // 'bdem' type
                const RecDef *d_constraint;  // constraint
            } DATA[] = {
                //line  'bdem' type            constraint
                //----  -----------            ----------
                { L_,   EType::BDEM_LIST,           0                 },
                { L_,   EType::BDEM_LIST,           &RS               },
                { L_,   EType::BDEM_TABLE,          0                 },
                { L_,   EType::BDEM_TABLE,          &RS               },
                { L_,   EType::BDEM_CHOICE,         0                 },
                { L_,   EType::BDEM_CHOICE,         &RC               },
                { L_,   EType::BDEM_CHOICE_ARRAY,   0                 },
                { L_,   EType::BDEM_CHOICE_ARRAY,   &RC               },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE   = DATA[ti].d_lineNum;
                const EType::Type  TYPE   = DATA[ti].d_type;
                const RecDef      *CONSTR = DATA[ti].d_constraint;

                const int  FMT = 55;
                const bool N   = EType::BDEM_LIST == TYPE ? true : false;

                FldSpec mF(TYPE, CONSTR, FMT, N, Z);  const FldSpec& F = mF;
                LOOP_ASSERT(LINE, TYPE   == F.elemType());
                LOOP_ASSERT(LINE, CONSTR == F.recordConstraint());
                LOOP_ASSERT(LINE, FMT    == F.formattingMode());
                LOOP_ASSERT(LINE, N      == F.isNullable());
                LOOP_ASSERT(LINE, false  == F.hasDefaultValue());
            }
        }

        if (verbose)
            bsl::cout << "\nTesting that allocator is hooked up correctly."
                      << bsl::endl;
        {
            bslma_TestAllocator         guardedAlloc(veryVeryVerbose);
            bslma_DefaultAllocatorGuard guard(&guardedAlloc);

                  FldSpec mF(EType::BDEM_STRING, 0, 0, false, Z);
            const FldSpec& F = mF;
            ASSERT(0 == guardedAlloc.numBytesInUse());

                  FldSpec  mG(EType::BDEM_STRING, 0, 0, false, Z);
            const FldSpec& G = mG;
            setDefaultValueForType(mG.defaultValue());
            ASSERT(0 == guardedAlloc.numBytesInUse());
        }

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'bdem_FieldSpec' THREE-ARGUMENT CONSTRUCTOR
        //
        // Concerns:
        //   - Field specs created using the 3-argument constructor have
        //     attributes with the expected value.
        //   - The memory allocator is hooked up correctly.
        //   - The destructor frees all memory internally allocated by the
        //     object.
        //
        // Plan:
        //   Create field specs having varied 'bdem' types.  For aggregate
        //   types, test both unconstrained and constrained variants.  Assert
        //   that the attributes of the field specs have the expected values.
        //   In particular, the formatting mode and nullability attributes
        //   should have the default values.  Provide an explicit test, using
        //   test allocators and a default allocator guard, to verify that the
        //   memory allocator is hooked up correctly and that the destructor
        //   deallocates all memory.  This latter test is white-box: only
        //   'STRING' is tested because it is the only 'bdem' type which uses
        //   the allocator supplied at construction.
        //
        // Testing:
        //   bdem_FieldSpec(EType type, const bdem_RecordDef *c=0, *ba=0);
        //   ~bdem_FieldSpec();
        //   const bdem_RecordDef *recordConstraint() const;
        //   EType elemType() const;
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << "\nTesting 'bdem_FieldSpec' 3-Argument Constructor"
                      << "\n==============================================="
                      << bsl::endl;

        if (verbose) bsl::cout << "\nTesting unconstrained 'bdem' types."
                               << bsl::endl;
        {
            for (int ti = 0; ti < EType::BDEM_NUM_TYPES; ++ti) {
                const EType::Type TYPE = (EType::Type)ti;

                {
                    FldSpec mF(TYPE);  const FldSpec& F = mF;
                    LOOP_ASSERT(TYPE, TYPE   == F.elemType());
                    LOOP_ASSERT(TYPE, 0      == F.recordConstraint());
                    LOOP_ASSERT(TYPE, FORMAT == F.formattingMode());
                    LOOP_ASSERT(TYPE, 0      == F.isNullable());
                    LOOP_ASSERT(TYPE, false  == F.hasDefaultValue());
                }
                {
                    FldSpec mF(TYPE, 0, Z);  const FldSpec& F = mF;
                    LOOP_ASSERT(TYPE, TYPE   == F.elemType());
                    LOOP_ASSERT(TYPE, 0      == F.recordConstraint());
                    LOOP_ASSERT(TYPE, FORMAT == F.formattingMode());
                    LOOP_ASSERT(TYPE, 0      == F.isNullable());
                    LOOP_ASSERT(TYPE, false  == F.hasDefaultValue());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting constrained 'bdem' types."
                               << bsl::endl;
        {
            Obj mS(Z);  const Obj& S = mS;
            gg(&mS, ":a? :b");  ASSERT(2 == S.numRecords());

            const RecDef& RC = S.record(0);
            ASSERT(RecDef::BDEM_CHOICE_RECORD   == RC.recordType());

            const RecDef& RS = S.record(1);
            ASSERT(RecDef::BDEM_SEQUENCE_RECORD == RS.recordType());

            static const struct {
                int           d_lineNum;     // source line number
                EType::Type   d_type;        // 'bdem' type
                const RecDef *d_constraint;  // constraint
            } DATA[] = {
                //line  'bdem' type            constraint
                //----  -----------            ----------
                { L_,   EType::BDEM_LIST,           0                 },
                { L_,   EType::BDEM_LIST,           &RS               },
                { L_,   EType::BDEM_TABLE,          0                 },
                { L_,   EType::BDEM_TABLE,          &RS               },
                { L_,   EType::BDEM_CHOICE,         0                 },
                { L_,   EType::BDEM_CHOICE,         &RC               },
                { L_,   EType::BDEM_CHOICE_ARRAY,   0                 },
                { L_,   EType::BDEM_CHOICE_ARRAY,   &RC               },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE   = DATA[ti].d_lineNum;
                const EType::Type  TYPE   = DATA[ti].d_type;
                const RecDef      *CONSTR = DATA[ti].d_constraint;

                const bool N = EType::BDEM_LIST == TYPE ? false : true;

                {
                    FldSpec mF(TYPE, CONSTR, Z);  const FldSpec& F = mF;
                    LOOP_ASSERT(LINE, TYPE   == F.elemType());
                    LOOP_ASSERT(LINE, CONSTR == F.recordConstraint());
                    LOOP_ASSERT(LINE, FORMAT == F.formattingMode());
                    LOOP_ASSERT(LINE, 0      == F.isNullable());
                    LOOP_ASSERT(LINE, false  == F.hasDefaultValue());
                }
            }
        }

        if (verbose)
            bsl::cout << "\nTesting that allocator is hooked up correctly."
                      << bsl::endl;
        {
            bslma_TestAllocator         guardedAlloc(veryVeryVerbose);
            bslma_DefaultAllocatorGuard guard(&guardedAlloc);

            FldSpec mF(EType::BDEM_STRING, 0, Z);  const FldSpec& F = mF;
            ASSERT(0 == guardedAlloc.numBytesInUse());

            FldSpec mG(EType::BDEM_STRING, 0, Z);  const FldSpec& G = mG;
            setDefaultValueForType(mG.defaultValue());
            ASSERT(0 == guardedAlloc.numBytesInUse());
        }

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING VERSION 1.0
        //
        // Concerns:
        //   1. The (free) streaming operators '<<' and '>>' are implemented
        //      using the respective member functions 'bdexStreamOut' and
        //      'bdexStreamIn'.
        //   2. Streaming must be neutral to exceptions thrown as a result of
        //      either allocating memory or streaming in values.
        //   3. Ensure that streaming works under the following conditions:
        //       VALID      - may contain any sequence of valid values.
        //       EMPTY      - valid, but contains no data.
        //       INVALID    - may or may not be empty.
        //       INCOMPLETE - the stream is truncated, but otherwise valid.
        //       CORRUPTED  - the data contains explicitly inconsistent fields.
        //   4. The 1.0 'bdex' streaming wire format is preserved for backward
        //      compatibility.
        //
        // Plan:
        //   First perform a trivial direct (breathing) test of the
        //   'bdexStreamOut' and 'bdexStreamIn' methods (to address concern 1).
        //   Note that the rest of the testing will use the stream operators.
        //
        //   Next, specify a set S of unique object values with substantial
        //   and varied differences.  For each value in S, construct an
        //   object x.
        //
        //   Note that, for performance reasons, the 'bdex' and 'bdema'
        //   exception testing are done serially rather than nested.  The
        //   code is substantially repeated for the 'bdema' exception test,
        //   but it is *not* necessary to repeat the 'ASSERT' statements.
        //
        //   VALID STREAMS (and exceptions)
        //     Using all combinations of (u, v) in S x S, stream out the value
        //     of u into a buffer and stream it back into (an independent
        //     instance of) v, and assert that u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each x in S, attempt to stream into (a temporary copy of) x
        //     from an empty and then invalid stream.  Verify after each try
        //     that the object is unchanged and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct a truncated input stream and attempt to read into
        //     objects initialized with distinct values.  Verify values of
        //     objects that are either successfully modified or left entirely
        //     unmodified,  and that the stream becomes invalid immediately
        //     after the first incomplete read.  Finally ensure that each
        //     object streamed into is in some valid state by creating a copy
        //     and then assigning a known value to that copy; allow the
        //     original instance to leave scope without further modification,
        //     so that the destructor asserts internal object invariants
        //     appropriately.
        //
        //   CORRUPTED DATA
        //     We will assume that the incomplete test fails every field,
        //     including a char (multi-byte representation) hence we need
        //     only to produce values that are inconsistent with a valid
        //     value and verify that they are detected.  Use the underlying
        //     stream package to simulate an instance of a typical valid
        //     (control) stream and verify that it can be streamed in
        //     successfully.  Then for each data field in the stream (beginning
        //     with the version number), provide one or more similar tests with
        //     that data field corrupted.  After each test, verify that the
        //     object is in some valid state after streaming, and that the
        //     input stream has gone invalid.
        //
        //     Note that it is not possible to test corrupted lengths (i.e.,
        //     record and field counts, and lengths of record and field names).
        //     A corrupted positive length cannot be distinguished from a
        //     non-corrupted positive length.  A negative length, a definitive
        //     corruption, cannot be simulated since the bdex 'getLength'
        //     methods always return non-negative values.
        //
        //   Concern 4 is addressed with an explicit regimen of 1.0
        //   compatibility tests.
        //
        // Testing:
        //  ^bdem_RecordDef::bdexStreamOut(STREAM& stream, major, minor) const;
        //  ^streamInRecordDef(STREAM& stream, *schema, *ba, major, minor);
        //   static int maxSupportedBdexVersion();
        //   bdexStreamIn(STREAM& stream, int version);
        //   bdexStreamOut(STREAM& stream, int version) const;
        //   operator>>(STREAM& stream, bdem_Schema& schema);
        //   operator<<(STREAM& stream, const bdem_Schema& schema);
        //
        // Note: '^' indicates a private method, which is tested indirectly
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting 'bdex' Streaming Version 1.0"
                                  "\n===================================="
                               << bsl::endl;

        const int VERSION = 1;

        if (verbose) bsl::cout << "\nDirect initial trial of 'bdexStreamOut' "
                                  " and (valid) 'bdexStreamIn' functionality."
                               << bsl::endl;
        {
            const Obj X(g(":a Aa :b Bb+ca"), &testAllocator);
            if (veryVerbose) {
                bsl::cout << "  Value being streamed:\n"; PS(X);
            }

            Out out;  bdex_OutStreamFunctions::streamOut(out, X, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            In in(OD, LOD);  ASSERT(in);  ASSERT(!in.isEmpty());
            in.setSuppressVersionCheck(1);

            Obj t(g(":d DdEe"), &testAllocator);

            if (veryVerbose) {
                bsl::cout << "  Value being overwritten:\n"; PS(t);
            }
            ASSERT(X != t);

            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(in);  ASSERT(in.isEmpty());

            if (veryVerbose) {
                bsl::cout << "  Value after overwrite:\n"; PS(t);
            }
            ASSERT(X == t);
        }
        {
            const Obj X(g(":a Aa :b Bb+ca"), &testAllocator);
            if (veryVerbose) {
                bsl::cout << "  Value being streamed:\n"; PS(X);
            }

            Out out;  X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            In in(OD, LOD);  ASSERT(in);  ASSERT(!in.isEmpty());

            Obj t(g(":d DdEe"), &testAllocator);

            if (veryVerbose) {
                bsl::cout << "  Value being overwritten:\n"; PS(t);
            }
            ASSERT(X != t);

            t.bdexStreamIn(in, VERSION);     ASSERT(in);  ASSERT(in.isEmpty());

            if (veryVerbose) {
                bsl::cout << "  Value after overwrite:\n"; PS(t);
            }
            ASSERT(X == t);
        }

        if (verbose)
            bsl::cout << "\nTesting stream operators ('<<' and '>>')."
                      << bsl::endl;

        if (verbose)
            bsl::cout << "\tOn valid, non-empty stream data." << bsl::endl;
        {
            for (int i = 0; i < NUM_SPECS; ++i) {
                const int         LINE   = testSpecs[i].d_lineNum;
                const char *const U_SPEC = testSpecs[i].d_spec;
                const Obj         UU     = g(U_SPEC);               // control

                Obj u(Z);  const Obj& U = u;  gg(&u, U_SPEC);

                Out out;
                U.bdexStreamOut(out, VERSION);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Must reset stream for each iteration of inner loop.
                In testInStream(OD, LOD);  In& in = testInStream;
                testInStream.setSuppressVersionCheck(1);
                LOOP_ASSERT(LINE, in);
                LOOP_ASSERT(LINE, !in.isEmpty());

                for (int j = 0; j < NUM_SPECS; ++j) {
                    const int         JLINE  = testSpecs[j].d_lineNum;
                    const char *const V_SPEC = testSpecs[j].d_spec;
                    const Obj         VV     = g(V_SPEC);            // control
                    const int         iEQj   = i == j;

                    if (veryVeryVerbose) { T_();  T_();  P(V_SPEC); }

                    BEGIN_BDEX_EXCEPTION_TEST {
                      in.reset();
                      LOOP2_ASSERT(LINE, JLINE, in);
                      LOOP2_ASSERT(LINE, JLINE, !in.isEmpty());

                      Obj v(Z);  const Obj& V = v;  gg(&v, V_SPEC);

                      LOOP2_ASSERT(LINE, JLINE,   UU == U);
                      LOOP2_ASSERT(LINE, JLINE,   VV == V);
                      LOOP4_ASSERT(LINE, JLINE, U, V, iEQj == (U == V));

                      // test stream-in operator here
                      bdex_InStreamFunctions::streamIn(in, v, VERSION);

                      LOOP2_ASSERT(LINE, JLINE, UU == U);
                      LOOP2_ASSERT(LINE, JLINE, UU == V);
                      LOOP4_ASSERT(LINE, JLINE, U, V, U == V);

                    } END_BDEX_EXCEPTION_TEST

                    // Repeat the above streaming with 'bdema' exception-test.
                    // Note that it is *not* necessary to 'ASSERT' anything!
                    BEGIN_BSLMA_EXCEPTION_TEST {

                      in.reset();
                      const int AL = testAllocator.allocationLimit();
                      testAllocator.setAllocationLimit(-1);
                      Obj v(Z);  gg(&v, V_SPEC);

                      testAllocator.setAllocationLimit(AL);
                      // testing 'bdema' exceptions only!
                      bdex_InStreamFunctions::streamIn(in, v, VERSION);
                    } END_BSLMA_EXCEPTION_TEST

                }
            }
        }

        if (verbose)
            bsl::cout << "\tOn incomplete (but otherwise valid) data."
                      << bsl::endl;
        {
            const Obj X1 =
                        g(":aAaBb"),     Y1 = g(":bCc"), Z1 = g(":cDd :eEe");
            const Obj X2 =
                        g(":*Ff:gGgHh"), Y2 = g(":i"),   Z2 = g(":jJjK*L*M*");
            const Obj X3 =
                        g(":c:b:aNa"),   Y3 = g(""),     Z3 = g(":dOaPb:*Q*");
            const Obj E(Z);  // Reference empty schema

            Out out;
            const int LOD0 = out.length();
            Y1.bdexStreamOut(out, VERSION);
            const int LOD1 = out.length();
            Y2.bdexStreamOut(out, VERSION);
            const int LOD2 = out.length();
            Y3.bdexStreamOut(out, VERSION);
            const int LOD  = out.length();
            const char *const OD = out.data();

            for (int i = 0; i < LOD; ++i) {
                In testInStream(OD, i);  In& in = testInStream;
                testInStream.setSuppressVersionCheck(1);
                LOOP_ASSERT(i, in);      LOOP_ASSERT(i, !i == in.isEmpty());

                if (veryVerbose) { bsl::cout << "\t\t"; P(i); }

                Obj t1(X1, Z), t2(X2, Z), t3(X3, Z);

                if (i < LOD1 - LOD0) {
                  BEGIN_BDEX_EXCEPTION_TEST {
                    in.reset();
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;  t2 = X2;  t3 = X3;

                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i, !in);
                    if (LOD0 == i) LOOP_ASSERT(i,  E == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
                }
                else if (i < LOD2 - LOD0) {
                  BEGIN_BDEX_EXCEPTION_TEST {
                    in.reset();
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;  t2 = X2;  t3 = X3;

                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);
                    if (LOD1 == i) LOOP_ASSERT(i,  E == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
                }
                else {
                  BEGIN_BDEX_EXCEPTION_TEST {
                    in.reset();
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;  t2 = X2;  t3 = X3;

                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);
                    if (LOD2 == i) LOOP_ASSERT(i,  E == t3);
                  } END_BDEX_EXCEPTION_TEST
                }

                // Repeat the above streaming with 'bdema' exception-test.
                // Note that it is *not* necessary to 'ASSERT' anything!
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    in.reset();
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    t1 = X1;  t2 = X2;  t3 = X3;
                    testAllocator.setAllocationLimit(AL);
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    ASSERT(!in);
                  } END_BSLMA_EXCEPTION_TEST
                }

                Obj w1(t1), w2(t2), w3(t3);  // make copies to be sure we can

                                LOOP_ASSERT(i, Z1 != w1);
                w1 = Z1;        LOOP_ASSERT(i, Z1 == w1);

                                LOOP_ASSERT(i, Z2 != w2);
                w2 = Z2;        LOOP_ASSERT(i, Z2 == w2);

                                LOOP_ASSERT(i, Z3 != w3);
                w3 = Z3;        LOOP_ASSERT(i, Z3 == w3);
            }
        }

        if (verbose) bsl::cout << "\tOn corrupted data." << bsl::endl;
        {
            const Obj W = g("");      // default value
            const Obj X = g(":cDd");  // original value
            const Obj Y = g(":aFb");  // new value

            if (verbose) bsl::cout << "\t\tGood stream (for control)."
                                   << bsl::endl;
            {
                Out out;
                out.putVersion(VERSION);

                out.putLength(1);                // one record
                const char *recordName = "a";
                int len = bsl::strlen(recordName) + 1;
                out.putLength(len);
                out.putArrayInt8(recordName, len - 1);

                out.putLength(1);                // one field
                const char *fieldName = "b";
                len = bsl::strlen(fieldName) + 1;
                out.putLength(len);
                out.putArrayInt8(fieldName, len - 1);
                out.putInt8((char)EType::BDEM_DOUBLE);

                out.putInt32(-1L);               // no constraint

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);   ASSERT(W != t);    ASSERT(X == t);  ASSERT(Y != t);
                In in(OD, LOD); ASSERT(in);
                int version;
                in.getVersion(version);
                ASSERT(VERSION == version);
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                ASSERT(in);
                ASSERT(W != t);    ASSERT(X != t);  ASSERT(Y == t);
            }

            if (veryVerbose) bsl::cout << "\t\tEmpty stream test."
                                       << bsl::endl;
            {
                Obj s(Z);  gg(&s, ":aAa"); ASSERT(0 != s.numRecords());
                In in;                     ASSERT(in);
                in.setSuppressVersionCheck(1);
                bdex_InStreamFunctions::streamIn(in, s, VERSION);
                ASSERT(!in);
            }

            if (veryVerbose) bsl::cout << "\t\tJust version stream test."
                                       << bsl::endl;
            {
                Obj s(Z);  gg(&s, ":aAa");  ASSERT(0 != s.numRecords());
                Out out;
                out.putVersion(VERSION);
                In in(out.data(), out.length());
                ASSERT(in);
                in.setSuppressVersionCheck(1);
                int version;
                in.getVersion(version);
                bdex_InStreamFunctions::streamIn(in, s, version);
                ASSERT(!in);
                ASSERT(0 == s.numRecords());
            }

            if (verbose) bsl::cout << "\t\tBad version." << bsl::endl;
            {
                for (char i = -1; i <= 0; ++i) {
                    Obj s(Z);  gg(&s, ":aAa");  ASSERT(0 != s.numRecords());
                    const Obj& S = s;
                    Out out;
                    out.putVersion(i);  // too small
                    S.bdexStreamOut(out, VERSION);

                    // Verify that stream data following bad version is good.
                    {
                        int version;
                        Obj t(Z);  const Obj& T = t;
                        In in(out.data(), out.length());
                        in.setSuppressVersionCheck(1);
                        in.getVersion(version);     ASSERT(VERSION != version);
                        t.bdexStreamIn(in, VERSION);
                        ASSERT(in);
                        ASSERT(S == T);
                    }

                    In in(out.data(), out.length());
                    in.setSuppressVersionCheck(1);  // needed for 1.1
                    int version;
                    in.getVersion(version);
                    LOOP_ASSERT(i, in);
                    bdex_InStreamFunctions::streamIn(in, s, version);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, 0 == s.numRecords());
                }

                {
                    const int LARGE = 3;
                    Obj s(Z);  gg(&s, ":aAa");  ASSERT(0 != s.numRecords());
                    const Obj& S = s;
                    Out out;
                    out.putVersion(LARGE);  // too large
                    S.bdexStreamOut(out, VERSION);

                    // Verify that stream data following bad version is good.
                    {
                        int version;
                        Obj t(Z);  const Obj& T = t;
                        In in(out.data(), out.length());
                        in.setSuppressVersionCheck(1);  // needed for 1.1
                        in.getVersion(version);       ASSERT(LARGE == version);
                        t.bdexStreamIn(in, VERSION);  ASSERT(in);
                                                      ASSERT(S == T);
                    }

                    In in(out.data(), out.length());
                    in.setSuppressVersionCheck(1);  // needed for 1.1
                    LOOP_ASSERT(LARGE, in);
                    int version;
                    in.getVersion(version);
                    bdex_InStreamFunctions::streamIn(in, s, version);
                    LOOP_ASSERT(LARGE, !in);
                    ASSERT(0 == s.numRecords());
                }
            }

            if (verbose) bsl::cout << "\t\tBad record data." << bsl::endl;
            {
                if (verbose) bsl::cout << "\t\t\tbad ElemType" << bsl::endl;
                {
                    Obj s(Z);  gg(&s, "");     ASSERT(1 != s.numRecords());
                    Out out;
                    out.putVersion(VERSION);  // version
                    out.putLength(1);         // schema length
                    out.putLength(0);         // record name length
                    out.putLength(1);         // field count
                    out.putLength(0);         // field name length
                    out.putInt8((char)-1);    // field type

                    In in(out.data(), out.length());   ASSERT(in);
                    in.setSuppressVersionCheck(1);  // needed for 1.1
                    int version;
                    in.getVersion(version);
                    bdex_InStreamFunctions::streamIn(in, s, VERSION);
                    ASSERT(!in);

                    ASSERT(1 == s.numRecords());
                    ASSERT(0 == s.record(0).numFields());
                }

                {
                    Obj s(Z);  gg(&s, "");     ASSERT(1 != s.numRecords());
                    Out out;
                    out.putVersion(VERSION);  // version
                    out.putLength(1);         // schema length
                    out.putLength(0);         // record name length
                    out.putLength(1);         // field count
                    out.putLength(0);         // field name length
                    out.putInt8((char)33);    // field type

                    In in(out.data(), out.length());   ASSERT(in);
                    in.setSuppressVersionCheck(1);  // needed for 1.1
                    int version;
                    in.getVersion(version);
                    bdex_InStreamFunctions::streamIn(in, s, VERSION);
                    ASSERT(!in);

                    ASSERT(1 == s.numRecords());
                    ASSERT(0 == s.record(0).numFields());
                }

                if (verbose) bsl::cout << "\t\t\tbad constraint indices"
                                       << bsl::endl;
                {
                    Obj s(Z);  gg(&s, "");     ASSERT(1 != s.numRecords());
                    Out out;
                    out.putVersion(VERSION);      // version
                    out.putLength(1);             // schema length
                    out.putLength(0);             // record name length
                    out.putLength(1);             // field count
                    out.putLength(0);             // field name length
                    bdem_ElemType::bdexStreamOut(out, EType::BDEM_INT, 1);
                                                  // field type
                    out.putInt32(-2);             // constraint

                    In in(out.data(), out.length());   ASSERT(in);
                    in.setSuppressVersionCheck(1);  // needed for 1.1
                    int version;
                    in.getVersion(version);
                    bdex_InStreamFunctions::streamIn(in, s, VERSION);
                    ASSERT(!in);

                    ASSERT(1 == s.numRecords());
                    ASSERT(1 == s.record(0).numFields());
                }

                {
                    Obj s(Z);  gg(&s, "");     ASSERT(1 != s.numRecords());
                    Out out;
                    out.putVersion(VERSION);    // version
                    out.putLength(1);           // schema length
                    out.putLength(0);           // record name length
                    out.putLength(1);           // field count
                    out.putLength(0);           // field name length
                    bdem_ElemType::bdexStreamOut(out, EType::BDEM_INT, 1);
                                                // field type
                    out.putInt32(1);            // constraint

                    In in(out.data(), out.length());   ASSERT(in);
                    in.setSuppressVersionCheck(1);  // needed for 1.1
                    int version;
                    in.getVersion(version);
                    bdex_InStreamFunctions::streamIn(in, s, VERSION);
                    ASSERT(!in);

                    ASSERT(1 == s.numRecords());
                    ASSERT(1 == s.record(0).numFields());
                }

                if (verbose) bsl::cout << "\t\t\tinvalid field types"
                                       << bsl::endl;
                {
                    Obj s(Z);  gg(&s, "");     ASSERT(1 != s.numRecords());
                    Out out;
                    out.putVersion(VERSION);  // version
                    out.putLength(1);         // schema length
                    out.putLength(0);         // record name length
                    out.putLength(1);         // field count
                    out.putLength(0);         // field name length
                    bdem_ElemType::bdexStreamOut(out, EType::BDEM_INT, 1);
                                              // field type
                    out.putInt32(0);          // constraint invalid for INT

                    In in(out.data(), out.length());   ASSERT(in);
                    in.setSuppressVersionCheck(1);  // needed for 1.1
                    int version;
                    in.getVersion(version);
                    bdex_InStreamFunctions::streamIn(in, s, VERSION);
                    ASSERT(!in);

                    ASSERT(1 == s.numRecords());
                    ASSERT(1 == s.record(0).numFields());
                }
            }
        }

        if (verbose) bsl::cout << "\tVersion 1.0 wire format." << bsl::endl;
        {
            const int VERSION = 1;

            // Use *no* 1.1 features in the following specs (e.g., BOOL,
            // CHOICE, TIMETZ).

            const char *testSpecs[] = {
                // An empty schema.
                "~",

                // One unnamed empty record.
                "~ :*",

                // One empty record named "" (the empty string, which is a
                // unique name).
                "~ :e",

                // One record named "a", having one field of each unconstrained
                // type.
      "~ :a Aa Bb Cc Dd Ee Ff Gg Hh Ii Jj K* L* M* N* O* P* Q* R* S* T* U* V*",

                // One record named "a", with self-referencing constrained
                // types using referencing by name and by number.
                "~ :a +ba #ca +d0 #e0",

                // A longer schema with a "representative" mix of records and
                // fields.
                "~ "
                ":* Aa "
                ":* "
                ":b Hh Ii Jj K* L* M* N* O* P* Q* R* "
                ":c +db #ec  "
                ":d S* T* U* V* Aa Bb Cc Dd Ee Ff Gg "
                ":* +b0 "
                ":* #ac ",

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

            static const struct {
                int         d_length;  // length of 'bdex' encoding
                const char *d_format;  // 'bdex' encoding version 1.0
            } DATA[] = {
                {   2, "\xe0\x00"                                            },
                {   6, "\xe0\x01\xe0\x00\xe0\x00"                            },
                {  11, "\xe0\x01\xe0\x01\xe0\x00\x00\x00\x00\xe0"
                       "\x00"                                                },
                { 269, "\xe0\x01\xe0\x02\xe0\x00\x00\x00\x01\x61"
                       "\xe0\x16\xe0\x02\xe0\x00\x00\x00\x01\x61"
                       "\xe0\x00\xe6\xff\xff\xff\xff\xe0\x02\xe0"
                       "\x00\x00\x00\x01\x62\xe0\x01\xe6\xff\xff"
                       "\xff\xff\xe0\x02\xe0\x00\x00\x00\x01\x63"
                       "\xe0\x02\xe6\xff\xff\xff\xff\xe0\x02\xe0"
                       "\x00\x00\x00\x01\x64\xe0\x03\xe6\xff\xff"
                       "\xff\xff\xe0\x01\xe0\x00\x00\x00\x00\xe0"
                       "\x04\xe6\xff\xff\xff\xff\xe0\x02\xe0\x00"
                       "\x00\x00\x01\x66\xe0\x05\xe6\xff\xff\xff"
                       "\xff\xe0\x02\xe0\x00\x00\x00\x01\x67\xe0"
                       "\x06\xe6\xff\xff\xff\xff\xe0\x02\xe0\x00"
                       "\x00\x00\x01\x68\xe0\x07\xe6\xff\xff\xff"
                       "\xff\xe0\x02\xe0\x00\x00\x00\x01\x69\xe0"
                       "\x08\xe6\xff\xff\xff\xff\xe0\x02\xe0\x00"
                       "\x00\x00\x01\x6a\xe0\x09\xe6\xff\xff\xff"
                       "\xff\xe0\x00\xe0\x0a\xe6\xff\xff\xff\xff"
                       "\xe0\x00\xe0\x0b\xe6\xff\xff\xff\xff\xe0"
                       "\x00\xe0\x0c\xe6\xff\xff\xff\xff\xe0\x00"
                       "\xe0\x0d\xe6\xff\xff\xff\xff\xe0\x00\xe0"
                       "\x0e\xe6\xff\xff\xff\xff\xe0\x00\xe0\x0f"
                       "\xe6\xff\xff\xff\xff\xe0\x00\xe0\x10\xe6"
                       "\xff\xff\xff\xff\xe0\x00\xe0\x11\xe6\xff"
                       "\xff\xff\xff\xe0\x00\xe0\x12\xe6\xff\xff"
                       "\xff\xff\xe0\x00\xe0\x13\xe6\xff\xff\xff"
                       "\xff\xe0\x00\xe0\x14\xe6\xff\xff\xff\xff"
                       "\xe0\x00\xe0\x15\xe6\xff\xff\xff\xff"                },
                {  71, "\xe0\x01\xe0\x02\xe0\x00\x00\x00\x01\x61"
                       "\xe0\x04\xe0\x02\xe0\x00\x00\x00\x01\x62"
                       "\xe0\x14\xe6\x00\x00\x00\x00\xe0\x02\xe0"
                       "\x00\x00\x00\x01\x63\xe0\x15\xe6\x00\x00"
                       "\x00\x00\xe0\x02\xe0\x00\x00\x00\x01\x64"
                       "\xe0\x14\xe6\x00\x00\x00\x00\xe0\x01\xe0"
                       "\x00\x00\x00\x00\xe0\x15\xe6\x00\x00\x00"
                       "\x00"                                                },
                { 379, "\xe0\x07\xe0\x00\xe0\x01\xe0\x02\xe0\x00"
                       "\x00\x00\x01\x61\xe0\x00\xe6\xff\xff\xff"
                       "\xff\xe0\x00\xe0\x00\xe0\x02\xe0\x00\x00"
                       "\x00\x01\x62\xe0\x0b\xe0\x02\xe0\x00\x00"
                       "\x00\x01\x68\xe0\x07\xe6\xff\xff\xff\xff"
                       "\xe0\x02\xe0\x00\x00\x00\x01\x69\xe0\x08"
                       "\xe6\xff\xff\xff\xff\xe0\x02\xe0\x00\x00"
                       "\x00\x01\x6a\xe0\x09\xe6\xff\xff\xff\xff"
                       "\xe0\x00\xe0\x0a\xe6\xff\xff\xff\xff\xe0"
                       "\x00\xe0\x0b\xe6\xff\xff\xff\xff\xe0\x00"
                       "\xe0\x0c\xe6\xff\xff\xff\xff\xe0\x00\xe0"
                       "\x0d\xe6\xff\xff\xff\xff\xe0\x00\xe0\x0e"
                       "\xe6\xff\xff\xff\xff\xe0\x00\xe0\x0f\xe6"
                       "\xff\xff\xff\xff\xe0\x00\xe0\x10\xe6\xff"
                       "\xff\xff\xff\xe0\x00\xe0\x11\xe6\xff\xff"
                       "\xff\xff\xe0\x02\xe0\x00\x00\x00\x01\x63"
                       "\xe0\x02\xe0\x02\xe0\x00\x00\x00\x01\x64"
                       "\xe0\x14\xe6\x00\x00\x00\x02\xe0\x01\xe0"
                       "\x00\x00\x00\x00\xe0\x15\xe6\x00\x00\x00"
                       "\x03\xe0\x02\xe0\x00\x00\x00\x01\x64\xe0"
                       "\x0b\xe0\x00\xe0\x12\xe6\xff\xff\xff\xff"
                       "\xe0\x00\xe0\x13\xe6\xff\xff\xff\xff\xe0"
                       "\x00\xe0\x14\xe6\xff\xff\xff\xff\xe0\x00"
                       "\xe0\x15\xe6\xff\xff\xff\xff\xe0\x02\xe0"
                       "\x00\x00\x00\x01\x61\xe0\x00\xe6\xff\xff"
                       "\xff\xff\xe0\x02\xe0\x00\x00\x00\x01\x62"
                       "\xe0\x01\xe6\xff\xff\xff\xff\xe0\x02\xe0"
                       "\x00\x00\x00\x01\x63\xe0\x02\xe6\xff\xff"
                       "\xff\xff\xe0\x02\xe0\x00\x00\x00\x01\x64"
                       "\xe0\x03\xe6\xff\xff\xff\xff\xe0\x01\xe0"
                       "\x00\x00\x00\x00\xe0\x04\xe6\xff\xff\xff"
                       "\xff\xe0\x02\xe0\x00\x00\x00\x01\x66\xe0"
                       "\x05\xe6\xff\xff\xff\xff\xe0\x02\xe0\x00"
                       "\x00\x00\x01\x67\xe0\x06\xe6\xff\xff\xff"
                       "\xff\xe0\x00\xe0\x01\xe0\x02\xe0\x00\x00"
                       "\x00\x01\x62\xe0\x14\xe6\x00\x00\x00\x00"
                       "\xe0\x00\xe0\x01\xe0\x02\xe0\x00\x00\x00"
                       "\x01\x61\xe0\x15\xe6\x00\x00\x00\x03"                },
                { 999, "\xe0\x0e\xe0\x00\xe0\x01\xe0\x02\xe0\x00"
                       "\x00\x00\x01\x61\xe0\x00\xe6\xff\xff\xff"
                       "\xff\xe0\x00\xe0\x00\xe0\x02\xe0\x00\x00"
                       "\x00\x01\x62\xe0\x0e\xe0\x02\xe0\x00\x00"
                       "\x00\x01\x68\xe0\x07\xe6\xff\xff\xff\xff"
                       "\xe0\x02\xe0\x00\x00\x00\x01\x69\xe0\x08"
                       "\xe6\xff\xff\xff\xff\xe0\x02\xe0\x00\x00"
                       "\x00\x01\x6a\xe0\x09\xe6\xff\xff\xff\xff"
                       "\xe0\x00\xe0\x0a\xe6\xff\xff\xff\xff\xe0"
                       "\x00\xe0\x0b\xe6\xff\xff\xff\xff\xe0\x00"
                       "\xe0\x0c\xe6\xff\xff\xff\xff\xe0\x00\xe0"
                       "\x0d\xe6\xff\xff\xff\xff\xe0\x02\xe0\x00"
                       "\x00\x00\x01\x61\xe0\x00\xe6\xff\xff\xff"
                       "\xff\xe0\x02\xe0\x00\x00\x00\x01\x62\xe0"
                       "\x01\xe6\xff\xff\xff\xff\xe0\x02\xe0\x00"
                       "\x00\x00\x01\x63\xe0\x02\xe6\xff\xff\xff"
                       "\xff\xe0\x02\xe0\x00\x00\x00\x01\x64\xe0"
                       "\x03\xe6\xff\xff\xff\xff\xe0\x01\xe0\x00"
                       "\x00\x00\x00\xe0\x04\xe6\xff\xff\xff\xff"
                       "\xe0\x02\xe0\x00\x00\x00\x01\x66\xe0\x05"
                       "\xe6\xff\xff\xff\xff\xe0\x02\xe0\x00\x00"
                       "\x00\x01\x67\xe0\x06\xe6\xff\xff\xff\xff"
                       "\xe0\x02\xe0\x00\x00\x00\x01\x63\xe0\x02"
                       "\xe0\x02\xe0\x00\x00\x00\x01\x62\xe0\x14"
                       "\xe6\x00\x00\x00\x02\xe0\x02\xe0\x00\x00"
                       "\x00\x01\x63\xe0\x15\xe6\x00\x00\x00\x03"
                       "\xe0\x00\xe0\x10\xe0\x00\xe0\x14\xe6\x00"
                       "\x00\x00\x02\xe0\x02\xe0\x00\x00\x00\x01"
                       "\x68\xe0\x07\xe6\xff\xff\xff\xff\xe0\x02"
                       "\xe0\x00\x00\x00\x01\x69\xe0\x08\xe6\xff"
                       "\xff\xff\xff\xe0\x02\xe0\x00\x00\x00\x01"
                       "\x6a\xe0\x09\xe6\xff\xff\xff\xff\xe0\x00"
                       "\xe0\x0a\xe6\xff\xff\xff\xff\xe0\x00\xe0"
                       "\x0b\xe6\xff\xff\xff\xff\xe0\x00\xe0\x0c"
                       "\xe6\xff\xff\xff\xff\xe0\x00\xe0\x0d\xe6"
                       "\xff\xff\xff\xff\xe0\x02\xe0\x00\x00\x00"
                       "\x01\x61\xe0\x00\xe6\xff\xff\xff\xff\xe0"
                       "\x02\xe0\x00\x00\x00\x01\x62\xe0\x01\xe6"
                       "\xff\xff\xff\xff\xe0\x02\xe0\x00\x00\x00"
                       "\x01\x63\xe0\x02\xe6\xff\xff\xff\xff\xe0"
                       "\x02\xe0\x00\x00\x00\x01\x64\xe0\x03\xe6"
                       "\xff\xff\xff\xff\xe0\x01\xe0\x00\x00\x00"
                       "\x00\xe0\x04\xe6\xff\xff\xff\xff\xe0\x02"
                       "\xe0\x00\x00\x00\x01\x66\xe0\x05\xe6\xff"
                       "\xff\xff\xff\xe0\x02\xe0\x00\x00\x00\x01"
                       "\x67\xe0\x06\xe6\xff\xff\xff\xff\xe0\x00"
                       "\xe0\x14\xe6\x00\x00\x00\x02\xe0\x00\xe0"
                       "\x04\xe0\x00\xe0\x15\xe6\x00\x00\x00\x02"
                       "\xe0\x02\xe0\x00\x00\x00\x01\x66\xe0\x05"
                       "\xe6\xff\xff\xff\xff\xe0\x02\xe0\x00\x00"
                       "\x00\x01\x67\xe0\x06\xe6\xff\xff\xff\xff"
                       "\xe0\x00\xe0\x14\xe6\x00\x00\x00\x02\xe0"
                       "\x02\xe0\x00\x00\x00\x01\x61\xe0\x08\xe0"
                       "\x00\xe0\x00\xe6\xff\xff\xff\xff\xe0\x02"
                       "\xe0\x00\x00\x00\x01\x62\xe0\x01\xe6\xff"
                       "\xff\xff\xff\xe0\x02\xe0\x00\x00\x00\x01"
                       "\x63\xe0\x02\xe6\xff\xff\xff\xff\xe0\x02"
                       "\xe0\x00\x00\x00\x01\x64\xe0\x03\xe6\xff"
                       "\xff\xff\xff\xe0\x01\xe0\x00\x00\x00\x00"
                       "\xe0\x04\xe6\xff\xff\xff\xff\xe0\x02\xe0"
                       "\x00\x00\x00\x01\x66\xe0\x05\xe6\xff\xff"
                       "\xff\xff\xe0\x02\xe0\x00\x00\x00\x01\x67"
                       "\xe0\x06\xe6\xff\xff\xff\xff\xe0\x00\xe0"
                       "\x14\xe6\x00\x00\x00\x02\xe0\x02\xe0\x00"
                       "\x00\x00\x01\x64\xe0\x07\xe0\x02\xe0\x00"
                       "\x00\x00\x01\x62\xe0\x01\xe6\xff\xff\xff"
                       "\xff\xe0\x02\xe0\x00\x00\x00\x01\x63\xe0"
                       "\x02\xe6\xff\xff\xff\xff\xe0\x02\xe0\x00"
                       "\x00\x00\x01\x64\xe0\x03\xe6\xff\xff\xff"
                       "\xff\xe0\x01\xe0\x00\x00\x00\x00\xe0\x04"
                       "\xe6\xff\xff\xff\xff\xe0\x02\xe0\x00\x00"
                       "\x00\x01\x66\xe0\x05\xe6\xff\xff\xff\xff"
                       "\xe0\x02\xe0\x00\x00\x00\x01\x67\xe0\x06"
                       "\xe6\xff\xff\xff\xff\xe0\x00\xe0\x15\xe6"
                       "\x00\x00\x00\x06\xe0\x01\xe0\x00\x00\x00"
                       "\x00\xe0\x06\xe0\x02\xe0\x00\x00\x00\x01"
                       "\x63\xe0\x02\xe6\xff\xff\xff\xff\xe0\x02"
                       "\xe0\x00\x00\x00\x01\x64\xe0\x03\xe6\xff"
                       "\xff\xff\xff\xe0\x01\xe0\x00\x00\x00\x00"
                       "\xe0\x04\xe6\xff\xff\xff\xff\xe0\x02\xe0"
                       "\x00\x00\x00\x01\x66\xe0\x05\xe6\xff\xff"
                       "\xff\xff\xe0\x02\xe0\x00\x00\x00\x01\x67"
                       "\xe0\x06\xe6\xff\xff\xff\xff\xe0\x00\xe0"
                       "\x14\xe6\x00\x00\x00\x07\xe0\x02\xe0\x00"
                       "\x00\x00\x01\x66\xe0\x04\xe0\x02\xe0\x00"
                       "\x00\x00\x01\x64\xe0\x03\xe6\xff\xff\xff"
                       "\xff\xe0\x01\xe0\x00\x00\x00\x00\xe0\x04"
                       "\xe6\xff\xff\xff\xff\xe0\x02\xe0\x00\x00"
                       "\x00\x01\x66\xe0\x05\xe6\xff\xff\xff\xff"
                       "\xe0\x02\xe0\x00\x00\x00\x01\x67\xe0\x14"
                       "\xe6\x00\x00\x00\x08\xe0\x02\xe0\x00\x00"
                       "\x00\x01\x67\xe0\x01\xe0\x02\xe0\x00\x00"
                       "\x00\x01\x67\xe0\x02\xe6\xff\xff\xff\xff"
                       "\xe0\x02\xe0\x00\x00\x00\x01\x68\xe0\x00"
                       "\xe0\x02\xe0\x00\x00\x00\x01\x69\xe0\x01"
                       "\xe0\x00\xe0\x01\xe6\xff\xff\xff\xff\xe0"
                       "\x02\xe0\x00\x00\x00\x01\x6a\xe0\x02\xe0"
                       "\x02\xe0\x00\x00\x00\x01\x61\xe0\x15\xe6"
                       "\x00\x00\x00\x06\xe0\x02\xe0\x00\x00\x00"
                       "\x01\x62\xe0\x14\xe6\x00\x00\x00\x06"                },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            ASSERT(NUM_DATA == NUM_SPECS);

            for (int i = 0; i < NUM_SPECS; ++i) {
                const char *const U_SPEC = testSpecs[i];
                const Obj         UU     = g(U_SPEC);           // control

                const int         LENGTH = DATA[i].d_length;
                const char *const FORMAT = DATA[i].d_format;

                Obj u(Z);  const Obj& U = u;  gg(&u, U_SPEC);

                Out out;
                U.bdexStreamOut(out, VERSION);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                LOOP3_ASSERT(i, LOD, LENGTH, LOD == LENGTH);
                LOOP_ASSERT(i, 0 == bsl::memcmp(OD, FORMAT, LOD));

                // Must reset stream for each iteration of inner loop.
                In in(OD, LOD);
                in.setSuppressVersionCheck(1);  // needed for 1.1
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, !in.isEmpty());

                const char *const V_SPEC =
                                 testSpecs[i + 1 >= NUM_SPECS - 1 ? 0 : i + 1];
                const Obj         VV     = g(V_SPEC);           // control

                if (veryVeryVerbose) { T_();  T_();  P(V_SPEC); }

                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, !in.isEmpty());

                Obj v(Z);  const Obj& V = v;  gg(&v, V_SPEC);

                LOOP_ASSERT(i, UU == U);
                LOOP_ASSERT(i, VV == V);
                LOOP_ASSERT(i,  U != V);

                v.bdexStreamIn(in, VERSION);
                                   ASSERT(in);  ASSERT(in.isEmpty());

                LOOP_ASSERT(i,  UU == U);
                LOOP_ASSERT(i,  UU == V);
                LOOP_ASSERT(i,   U == V);
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Schema' ASSIGNMENT OPERATOR
        //
        // Concerns:
        //   We are concerned that any schema value can be assigned to any
        //   other schema value.  Since 'operator=' is implemented using the
        //   same helper function as the (already-tested) copy constructor, our
        //   concerns are lessened.  Nevertheless, we employ a comparable test
        //   strategy, except that each value is used for the lhs and the rhs
        //   schema, and we are not concerned with the allocator, since it is
        //   not affected by assignment.  Exception neutrality is tested.
        //
        // Plan:
        //   Use the set S of pre-defined schema test values.  For each (u, v)
        //   in S x S, verify that u = v produces the expected results.
        //
        // Testing:
        //   bdem_Schema& operator=(const bdem_Schema& rhs);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting 'bdem_Schema' 'operator='"
                               << "\n================================="
                               << bsl::endl;

        if (verbose)
            bsl::cout << "\nVerify u = v works for all (u, v) in S x S"
                      << bsl::endl;

        for (int i = 0; i < NUM_SPECS; ++i) {
            const int  LINE   = testSpecs[i].d_lineNum;
            const char *ySpec = testSpecs[i].d_spec;
            for (int j = 0; j < NUM_SPECS; ++j) {
              const int  JLINE  = testSpecs[j].d_lineNum;
              const char *xSpec = testSpecs[j].d_spec;
              if (veryVeryVerbose) bsl::cout << "Testing specs: '" << xSpec
                                             << "' and '" << ySpec
                                             << bsl::endl;

              Obj x(g(xSpec), Z);  const Obj& X = x;
              Obj y(g(ySpec), Z);  const Obj& Y = y;

              BEGIN_BSLMA_EXCEPTION_TEST {
                y = X;

                LOOP2_ASSERT(LINE, JLINE, X == Y);
                LOOP2_ASSERT(LINE, JLINE, !(X != Y));
              } END_BSLMA_EXCEPTION_TEST
            }
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'g'
        //
        // Concerns:
        //   'g' is implemented using 'gg'.  It is sufficient to test that 'gg'
        //   and 'g' produce the same values for a few test specs.
        //
        // Plan:
        //   Test that 'g' results in schemas equal to those generated by 'gg'.
        //
        // Testing:
        //   bdem_Schema g(const char *spec);              (generator function)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting g"
                               << "\n=========" << bsl::endl;

        if (verbose)
            bsl::cout << "\nCompare values produced by 'g' and 'gg' on "
                      << "various inputs." << bsl::endl;

        for (int i = 0; i < NUM_SPECS; ++i) {
            const int         LINE = testSpecs[i].d_lineNum;
            const char *const spec = testSpecs[i].d_spec;
            if (veryVerbose) P(spec);

            Obj x(Z);  const Obj& X = x;  gg(&x, spec);
            if (veryVerbose) { PS(g(spec)); PS(X); }

            LOOP_ASSERT(LINE, X == g(spec));
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Schema' COPY CONSTRUCTOR
        //
        // Concerns:
        //   We are concerned that any schema value can be copy constructed to
        //   a new instance.  Although the implementation is reasonably
        //   complex, it is sufficient to make sure that the schema test values
        //   include the following properties:
        //     * a schema with at least two record definitions (tests 'for').
        //     * a schema with at least one enumeration definition.
        //     * a record with at least two field definitions (tests 'for').
        //     * a field definition indicating a constrained list.
        //     * a field definition indicating a constrained table.
        //     * a field definition indicating any other type.
        //   Since the copy constructor optionally takes an allocator, we want
        //   to confirm that any (or no) specified allocator works properly.
        //   In addition, the 'bdema' exception tests must be performed to test
        //   the (local) proctor object.  All other complexity involves only
        //   tested functionality.
        //
        // Plan:
        //   1) As an initial "sanity check", confirm that the default (empty)
        //      schema may be copied to a new instance.
        //   2) Use the pre-defined test schema to confirm that any schema
        //      value may be copied to a new instance.
        //
        // Testing:
        //   bdem_Schema(const bdem_Schema& original, *ba=0);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting 'bdem_Schema' Copy C'tor"
                                  "\n================================"
                               << bsl::endl;

        if (verbose) bsl::cout << "\nTest with empty schema." << bsl::endl;
        {
          BEGIN_BSLMA_EXCEPTION_TEST {
            Obj x(Z);     const Obj& X = x;
            Obj y(X, Z);  const Obj& Y = y;

            ASSERT(1 == (X == Y));
            ASSERT(0 == (X != Y));
          } END_BSLMA_EXCEPTION_TEST
        }

        if (verbose) bsl::cout << "\nTest with \"standard\" test schemas."
                               << bsl::endl;
        {
            for (int i = 0; i < NUM_SPECS; ++i) {
              Obj r(Z);  const Obj& R = r;             // "reference object"
              const int LINE   = testSpecs[i].d_lineNum;
              gg(&r, testSpecs[i].d_spec);

              const Obj X1(R);                         // No allocator.
              LOOP_ASSERT(LINE, X1 == R);
              LOOP_ASSERT(LINE, !(X1 != R));

              const Obj X2(R, (bslma_Allocator *) 0);  // Null allocator.
              LOOP_ASSERT(LINE, X2 == R);
              LOOP_ASSERT(LINE, !(X2 != R));

              BEGIN_BSLMA_EXCEPTION_TEST {             // Test allocator.
                bslma_TestAllocator testAllocator;
#ifdef BDE_BUILD_TARGET_EXC
                testAllocator.setAllocationLimit(bslmaExceptionCounter);
#endif
                const Obj X3(R, &testAllocator);

                LOOP_ASSERT(LINE, X3 == R);
                LOOP_ASSERT(LINE, !(X3 != R));
              } END_BSLMA_EXCEPTION_TEST
            }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Schema' EQUALITY OPERATORS
        //
        // Concerns:
        //   'operator==' for 'bdem_Schema' is relatively complex, since a
        //   schema value is basically an array of possibly-named records, each
        //   of which is an array of possibly-named, possibly-constrained
        //   fields.  Specifically, there are 14 'return' paths and two loop
        //   controls that must be probed.  Symmetry (i.e., switching the 'lhs'
        //   and 'rhs' inputs) reduces the number of test vectors needed to
        //   cover all return paths from 14 to 11, while testing the boundaries
        //   of the two loops adds two test vectors.  The "alias" case is
        //   tested separately, and so 12 test vectors are sufficient.  The
        //   empty schema and an arbitrary larger schema are used in addition
        //   the baseline and its perturbations, bringing the total to 14.
        //
        // Plan:
        //   1) Use the pre-defined test schema to confirm that any schema is
        //   equal to itself.
        //
        //   2) Specify a set S of schemas containing a "baseline" value and
        //   all necessary perturbations of that value (plus two extra values)
        //   such that each value in S is unique.  Verify the correctness of
        //   'operator==' and 'operator!=' using all elements (u, v) of the
        //   cross product S x S.
        //
        // Testing:
        //   bool operator==(const bdem_Schema& lhs, const bdem_Schema& rhs);
        //   bool operator!=(const bdem_Schema& lhs, const bdem_Schema& rhs);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting equality operators"
                               << "\n==========================" << bsl::endl;

        if (verbose) bsl::cout << "\nTesting \"alias\" equality (X == X)"
                               << bsl::endl;
        {
            for (int i = 0; i < NUM_SPECS; ++i) {
                Obj x(Z);  const Obj& X = x;
                const int LINE   = testSpecs[i].d_lineNum;
                gg(&x, testSpecs[i].d_spec);

                LOOP_ASSERT(LINE, 1 == (X == X));
                LOOP_ASSERT(LINE, 0 == (X != X));
            }
        }

        if (verbose) bsl::cout << "\nCompare each pair (u, v) in S x S."
                               << bsl::endl;
        {
            static const char *SPECS[] = {
                ":a Aa     :b Bb+ca"   ,  // baseline
                ":a Aa"                ,  // different schema length
                ":a AaEe   :b Bb+ca"   ,  // 1st record different length
                ":a Aa     :b Bb+caEe" ,  // last record different length
                ":a Aa     :* Bb+ca"   ,  // rhs (lhs) record unnamed
                ":a Aa     :e Bb+ca"   ,  // record differs in name
                ":a Aa     :b B*+ca"   ,  // 1st field (of 2nd record) unnamed
                ":a Aa     :b Bb+*a"   ,  // last field (of 2nd record) unnamed
                ":a Aa     :b Be+ca"   ,  // one different field name
                ":a Aa     :b Cb+ca"   ,  // one different field type
                ":a Aa     :b BbUc"    ,  // rhs (lhs) list unconstrained
                ":a Aa     :b Bb+cb"   ,  // one different (list) constraint
                "~"                    ,  // an empty schema
                ":a="                  ,  // schema with empty enum
                ":*="                  ,  // schema with empty enum unnamed
                ":a=bc3"               ,  // schema with non-empty enum
                ":*=bc3"               ,  // schema with non-empty enum unnamed
                ":a=bc3    :b$fa"      ,  // schema with enum and record
                ":a=bc3    :*$fa"      ,  // schema with enum and record
                ":*=bc3    :* Bb"      ,  // schema with enum and record
                ":*=bc3    :b Bb"      ,  // schema with enum and record

                ":* Aa       :b HhIiJjK*L*M*N*O*P*Q*R*S*T*U*V*AaBbCcDdEeFfGg"
                ":c +db#ec   :* +b0"   ,  // a schema with four records

            0}; // Null string required as last element.

            for (int i = 0; SPECS[i]; ++i) {
                const char *const U_SPEC = SPECS[i];

                Obj mU(&testAllocator); gg(&mU, U_SPEC); const Obj& U = mU;

                if (veryVerbose) { P_(i); P(U_SPEC); PS(U); }

                for (int j = 0; SPECS[j]; ++j) {
                    const char *const V_SPEC = SPECS[j];
                    Obj mV(&testAllocator); gg(&mV, V_SPEC); const Obj& V = mV;

                    if (veryVerbose) { T_(); P_(j); P(V_SPEC); PS(V);}
                    const bool isSame = i == j;
                    LOOP2_ASSERT(i, j,  isSame == (U == V));
                    LOOP2_ASSERT(i, j, !isSame == (U != V));
                }
            }
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING TO 'ostream'
        //
        // Concerns:
        //   The 'print' method must calculate indentations properly, and print
        //   named and unnamed records containing named and unnamed fields that
        //   are unconstrained, constrained by name, or constrained by index.
        //   'operator<<' is implemented using 'print' and the default
        //   indentation, and so the testing concerns are just that the
        //   arguments are correctly forwarded.
        //
        // Plan:
        //   For a set of schema values having named and unnamed records, and
        //   named and unnamed fields of every possible field type, including
        //   lists and tables constrained by name and by index, stream the
        //   schema value to a buffer and compare with the expected results.
        //   For each schema value, test 'operator<<' (with the default
        //   indentation), and test 'print' with different indentations.
        //
        // Testing:
        //  ^bdem_RecordDef::print(ostream& stream, int level, int spl) const;
        //  ^bdem_EnumerationDef::print(ostream&
        //                              stream, level, spacesPerLevel) const;
        //   bdem_Schema::print(ostream& stream, int level, int spl) const;
        //   operator<<(ostream& stream, const bdem_Schema& schema);
        //
        // Note: '^' indicates a private method, which is tested indirectly
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << "\nTesting 'bdem_Schema' 'operator<<' and 'print'"
                      << "\n=============================================="
                      << bsl::endl;

        const char SPEC4[] = "~:a Aa Bb Cc Dd Ee Ff Gg Hh Ii Jj K* L* M* N* "
                            " :b Dd Ee #fa +g1 :* #aa +bb +*2 #*2 +c2 #d2";

        if (verbose) bsl::cout << "Testing 'ostream' streaming." << bsl::endl;
        {
            static const struct {
                int         d_lineNumber;
                const char *d_spec;
                int         d_nTabs;
                int         d_spacesPerTab;
                const char *d_fmt;  // output format
            } DATA[] = {
                //line  spec  tabs  sp/tab  output format
                //----  ----  ----  ------  ---------------------------------
                { L_,   "~",    0,    4,    "{"                    // default
                                            "}"
                },

                { L_,   "~",    3,    2,    "      {\n"
                                            "      }\n"
                },

                { L_,   "~",   -3,    2,    "{\n"
                                            "      }\n"
                },

                { L_,   "~ :*",  0,    4,   "{"                    // default
                                            " SEQUENCE RECORD {"
                                            " } "
                                            "}"
                },

                { L_,   "~ :*",  3,    2,   "      {\n"
                                            "        SEQUENCE RECORD {\n"
                                            "        }\n"
                                            "      }\n"
                },

                { L_,   "~ :*", -3,    2,   "{\n"
                                            "        SEQUENCE RECORD {\n"
                                            "        }\n"
                                            "      }\n"
                },

                { L_,   "~ :*=", 0,    4,   "{"                    // default
                                            " ENUMERATION {"
                                            " } "
                                            "}"
                },

                { L_,   "~ :*=", 3,     2,  "      {\n"
                                            "        ENUMERATION {\n"
                                            "        }\n"
                                            "      }\n"
                },

                { L_,   "~ :*=",-3,     2,  "{\n"
                                            "        ENUMERATION {\n"
                                            "        }\n"
                                            "      }\n"
                },

                { L_,   "~:v",   0,    4,   "{"                    // default
                                            " SEQUENCE RECORD \"vee\" {"
                                            " } "
                                            "}"
                },

                { L_,   "~:u",   3,    2, "      {\n"
                                          "        SEQUENCE RECORD \"you\" {\n"
                                          "        }\n"
                                          "      }\n"
                },

                { L_,   "~:x",   -3,    2, "{\n"
                                           "        SEQUENCE RECORD \"ex\" {\n"
                                           "        }\n"
                                           "      }\n"
                },

                { L_,   "~:v=",  0,    4,   "{"                    // default
                                            " ENUMERATION \"vee\" {"
                                            " } "
                                            "}"
                },

                { L_,   "~:u=",  3,    2,   "      {\n"
                                            "        ENUMERATION \"you\" {\n"
                                            "        }\n"
                                            "      }\n"
                },

                { L_,   "~:x=",  -3,    2,  "{\n"
                                            "        ENUMERATION \"ex\" {\n"
                                            "        }\n"
                                            "      }\n"
                },

                { L_,   SPEC4,   0,    4,
                             "{"                                   // default
                             " SEQUENCE RECORD \"a\" {"
                             " CHAR \"a\" { nullable 0x0 }"
                             " SHORT \"b\" { nullable 0x0 }"
                             " INT \"c\" { nullable 0x0 }"
                             " INT64 \"d\" { nullable 0x0 }"
                             " FLOAT \"\" { nullable 0x0 }"
                             " DOUBLE \"f\" { nullable 0x0 }"
                             " STRING \"g\" { nullable 0x0 }"
                             " DATETIME \"h\" { nullable 0x0 }"
                             " DATE \"i\" { nullable 0x0 }"
                             " TIME \"j\" { nullable 0x0 }"
                             " CHAR_ARRAY { nullable 0x0 }"
                             " SHORT_ARRAY { nullable 0x0 }"
                             " INT_ARRAY { nullable 0x0 }"
                             " INT64_ARRAY { nullable 0x0 }"
                             " } "
                             " SEQUENCE RECORD \"b\" {"
                             " INT64 \"d\" { nullable 0x0 }"
                             " FLOAT \"\" { nullable 0x0 }"
                             " TABLE<\"a\"> \"f\" { nullable 0x0 }"
                             " LIST<\"b\"> \"g\" { nullable 0x0 }"
                             " } "
                             " SEQUENCE RECORD {"
                             " TABLE<\"a\"> \"a\" { nullable 0x0 }"
                             " LIST<\"b\"> \"b\" { !nullable 0x0 }"
                             " LIST[2] { nullable 0x0 }"
                             " TABLE[2] { nullable 0x0 }"
                             " LIST[2] \"c\" { nullable 0x0 }"
                             " TABLE[2] \"d\" { nullable 0x0 }"
                             " } "
                             "}"
                },

                { L_,  SPEC4,    3,   2,
                             "      {\n"
                             "        SEQUENCE RECORD \"a\" {\n"
                             "          CHAR \"a\" { nullable 0x0 }\n"
                             "          SHORT \"b\" { nullable 0x0 }\n"
                             "          INT \"c\" { nullable 0x0 }\n"
                             "          INT64 \"d\" { nullable 0x0 }\n"
                             "          FLOAT \"\" { nullable 0x0 }\n"
                             "          DOUBLE \"f\" { nullable 0x0 }\n"
                             "          STRING \"g\" { nullable 0x0 }\n"
                             "          DATETIME \"h\" { nullable 0x0 }\n"
                             "          DATE \"i\" { nullable 0x0 }\n"
                             "          TIME \"j\" { nullable 0x0 }\n"
                             "          CHAR_ARRAY { nullable 0x0 }\n"
                             "          SHORT_ARRAY { nullable 0x0 }\n"
                             "          INT_ARRAY { nullable 0x0 }\n"
                             "          INT64_ARRAY { nullable 0x0 }\n"
                             "        }\n"
                             "        SEQUENCE RECORD \"b\" {\n"
                             "          INT64 \"d\" { nullable 0x0 }\n"
                             "          FLOAT \"\" { nullable 0x0 }\n"
                             "          TABLE<\"a\"> \"f\" { nullable 0x0 }\n"
                             "          LIST<\"b\"> \"g\" { nullable 0x0 }\n"
                             "        }\n"
                             "        SEQUENCE RECORD {\n"
                             "          TABLE<\"a\"> \"a\" { nullable 0x0 }\n"
                             "          LIST<\"b\"> \"b\" { !nullable 0x0 }\n"
                             "          LIST[2] { nullable 0x0 }\n"
                             "          TABLE[2] { nullable 0x0 }\n"
                             "          LIST[2] \"c\" { nullable 0x0 }\n"
                             "          TABLE[2] \"d\" { nullable 0x0 }\n"
                             "        }\n"
                             "      }\n"
                },

                { L_,  SPEC4,   -3,    2,
                             "{\n"
                             "        SEQUENCE RECORD \"a\" {\n"
                             "          CHAR \"a\" { nullable 0x0 }\n"
                             "          SHORT \"b\" { nullable 0x0 }\n"
                             "          INT \"c\" { nullable 0x0 }\n"
                             "          INT64 \"d\" { nullable 0x0 }\n"
                             "          FLOAT \"\" { nullable 0x0 }\n"
                             "          DOUBLE \"f\" { nullable 0x0 }\n"
                             "          STRING \"g\" { nullable 0x0 }\n"
                             "          DATETIME \"h\" { nullable 0x0 }\n"
                             "          DATE \"i\" { nullable 0x0 }\n"
                             "          TIME \"j\" { nullable 0x0 }\n"
                             "          CHAR_ARRAY { nullable 0x0 }\n"
                             "          SHORT_ARRAY { nullable 0x0 }\n"
                             "          INT_ARRAY { nullable 0x0 }\n"
                             "          INT64_ARRAY { nullable 0x0 }\n"
                             "        }\n"
                             "        SEQUENCE RECORD \"b\" {\n"
                             "          INT64 \"d\" { nullable 0x0 }\n"
                             "          FLOAT \"\" { nullable 0x0 }\n"
                             "          TABLE<\"a\"> \"f\" { nullable 0x0 }\n"
                             "          LIST<\"b\"> \"g\" { nullable 0x0 }\n"
                             "        }\n"
                             "        SEQUENCE RECORD {\n"
                             "          TABLE<\"a\"> \"a\" { nullable 0x0 }\n"
                             "          LIST<\"b\"> \"b\" { !nullable 0x0 }\n"
                             "          LIST[2] { nullable 0x0 }\n"
                             "          TABLE[2] { nullable 0x0 }\n"
                             "          LIST[2] \"c\" { nullable 0x0 }\n"
                             "          TABLE[2] \"d\" { nullable 0x0 }\n"
                             "        }\n"
                             "      }\n"
                }
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                char buf[1500];
                bsl::memset(buf, 0xff, sizeof buf);
                                                          // Scribble on 'buf'.
                bsl::ostrstream out(buf, sizeof buf);

                const int   LINE     = DATA[i].d_lineNumber;
                const char *const EXPECTED = DATA[i].d_fmt;

                LOOP_ASSERT(LINE, sizeof(buf) > bsl::strlen(EXPECTED));
                Obj x(Z);  const Obj& X = x;  gg(&x, DATA[i].d_spec);

                if (0 == i % 3) {    // "default" test data; use 'operator<<'
                    if (verbose) bsl::cout << "\tTesting 'operator<<'"
                                           << bsl::endl;
                    out << X << bsl::ends;
                }
                else {
                    if (verbose) bsl::cout << "\tTesting 'print'" << bsl::endl;
                    X.print(out, DATA[i].d_nTabs, DATA[i].d_spacesPerTab);
                    out << bsl::ends;
                }

                LOOP_ASSERT(LINE, 0 == bsl::strcmp(EXPECTED, buf));

                if (veryVerbose) {
                    bsl::cout << "EXPECTED: \'" << EXPECTED << "\'"
                              << bsl::endl;
                    bsl::cout << "ACTUAL  : \'" << buf << "\'" << bsl::endl;
                }
            }
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'bdem_FieldDef' ACCESSORS CONSTRAINT & ELEMTYPE
        //
        // Concerns:
        //   The 'bdem_FieldDef' accessors are straightforward, given a valid
        //   field definition.  We are concerned only that they return the
        //   appropriate 'bdem_FieldDef' data member.  The ('friend') equality
        //   (free) operators similarly need only to access and appropriately
        //   compare the relevant data members correctly.  Therefore,
        //   relatively minimal test data is sufficient for a thorough test.
        //
        // Plan:
        //   Specify a single schema X having N records, each containing M
        //   fields of varying, representative constrained and unconstrained
        //   types.  Extract the set of N * M fields F using already-tested
        //   schema and record definition accessors.
        //   1) Verify that the 'bdem_FieldDef' accessors return the expected
        //      values for each field f in F.
        //   2) Verify that the equality operators give the expected results
        //      for selected pairs of fields (f1, f2) in F.
        //
        // Testing:
        //   const bdem_RecordDef *recordConstraint() const;
        //   EType elemType() const;
        //   operator==(const bdem_FieldDef& lhs, const bdem_FieldDef& rhs);
        //   operator!=(const bdem_FieldDef& lhs, const bdem_FieldDef& rhs);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting 'bdem_FieldDef' Accessors"
                                  "\n================================="
                               << bsl::endl;

        // Specify six fields [f0..f5] in three records [r0..r2]
        //                  |  r0     |    r1      |    r2      |
        //                  |  f0 f1  |    f2 f3   |    f4  f5  |
        const char spec[] = ":a  Aa A*  :b  Ua +*a  :c  +ca #*a ";

        const int NUM_RECS     = 3;
        const int FLDS_PER_REC = 2;
        const int NUM_FLDS     = NUM_RECS * FLDS_PER_REC;

        Obj x(Z);  const Obj& X = x;  gg(&x, spec);
        if (veryVerbose) { P(spec);  PS(X); }

        ASSERT(NUM_RECS     == X.numRecords()); // minimal sanity check on 'x'
        ASSERT(FLDS_PER_REC == X.record(0).numFields());
        ASSERT(FLDS_PER_REC == X.record(1).numFields());
        ASSERT(FLDS_PER_REC == X.record(2).numFields());

        const RecDef *R[NUM_RECS];
        const FldDef *F[NUM_FLDS];

        for (int i = 0; i < NUM_RECS; ++i) {
            R[i] = &X.record(i);
            if (veryVerbose) { P_(i); P(R[i]->numFields()); }
            for (int j = 0; j < FLDS_PER_REC; ++j) {
                F[FLDS_PER_REC * i + j] = &R[i]->field(j);
                if (veryVerbose) { T_(); P_(j); P(R[i]->field(j).elemType()); }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'elemType'." << bsl::endl;
        {
            ASSERT(EType::BDEM_CHAR  == F[0]->elemType());
            ASSERT(EType::BDEM_CHAR  == F[1]->elemType());
            ASSERT(EType::BDEM_LIST  == F[2]->elemType());
            ASSERT(EType::BDEM_LIST  == F[3]->elemType());
            ASSERT(EType::BDEM_LIST  == F[4]->elemType());
            ASSERT(EType::BDEM_TABLE == F[5]->elemType());
        }

        if (verbose) bsl::cout << "\nTesting 'constraint'." << bsl::endl;
        {
            ASSERT(0    == F[0]->recordConstraint());
            ASSERT(0    == F[1]->recordConstraint());
            ASSERT(0    == F[2]->recordConstraint());
            ASSERT(R[0] == F[3]->recordConstraint());
            ASSERT(R[0] == F[4]->recordConstraint());
            ASSERT(R[0] == F[5]->recordConstraint());
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING ENUMERATOR-RELATED 'bdem_EnumerationDef' ACCESSORS
        //
        // Concerns:
        //   'lookupName' and 'lookupId' must always returns a valid (possibly
        //   null) name/id.
        //
        // Plan:
        //   1) Specify a schema having a single enumeration definition having
        //      several enumerator.  In a loop, use the loop index and the
        //      'NAMES' array to test the expected results of 'lookupName' and
        //      'lookupId'.
        //
        //   2) Test expected results of 'lookupName' and 'lookupId' for
        //      non-existent enumerator.
        //
        // Testing:
        //   const char *lookupName(int id) const;
        //   const char *lookupName(const char* name) const;
        //   int lookupId(const char *name) const;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
            << "Testing Enumerator-Related 'bdem_EnumerationDef' Accessors"
            << bsl::endl
            << "=========================================================="
            << bsl::endl;

        if (verbose) bsl::cout << "\nTesting 'lookupName' and 'lookupId'."
                               << bsl::endl;
        {
            const char *NAMES[] = { "a", "b", "longer name", "c", "d" };

            const int NUM_ENUMS = sizeof NAMES / sizeof(char *);

            Obj x(Z);

            EnumDef *e = x.createEnumeration();  const EnumDef& E = *e;

            // First build up the whole record definition before testing.
            int i;
            for (i = 0; i < NUM_ENUMS; ++i) {
                int returnedId = e->addEnumerator(NAMES[i]);
                if (veryVerbose) { T_();  T_();  P(returnedId); }

                LOOP_ASSERT(i, i == returnedId);
            }

            for (i = 0; i < NUM_ENUMS; ++i) {
                if (veryVerbose) { T_();  P_(i);  P(E.lookupName(i)); }
                LOOP_ASSERT(i, 0 == strCmp(NAMES[i], E.lookupName(NAMES[i])));
                LOOP_ASSERT(i, 0 == strCmp(NAMES[i], E.lookupName(i)));
                LOOP_ASSERT(i, i == E.lookupId(NAMES[i]));
            }

            if (verbose) bsl::cout << "\n\twith non-existent names:"
                                   << bsl::endl;

            ASSERT(0 == E.lookupName("foo"));
            ASSERT(0 == E.lookupName("bar"));
            ASSERT(bdetu_Unset<int>::unsetValue() == E.lookupId("foo"));
            ASSERT(bdetu_Unset<int>::unsetValue() == E.lookupId("bar"));

            if (verbose) bsl::cout << "\n\twith non-existent id:"
                                   << bsl::endl;

            ASSERT(0 == E.lookupName(INT_MIN));
            ASSERT(0 == E.lookupName(INT_MAX));
            ASSERT(0 == E.lookupName(10));
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING FIELD-RELATED 'bdem_RecordDef' ACCESSORS
        //
        // Concerns:
        //   This test case is substantially a copy of case 4, with the roles
        //   of (schema, record definition) replaced by the respective roles of
        //   (record definition, field definition).  The methods are logically
        //   identical, except that "Record" is replaced with "Field" (and that
        //   'bdem_RecordDef::numFields()' has already been tested in case 5).
        //
        //   'fieldName' must forward its argument to the correct method of
        //   the correct data member, and always returns a valid (possibly
        //   null) name.  'fieldIndex' makes a similar call, except the
        //   specific behavior is more complicated: names not in the record
        //   definition must return a negative value.
        //
        //   'field' and 'lookupField' are substantially parallel to
        //   'fieldName' and 'fieldIndex', except for the return type, and
        //   that 'lookupField' actually implements the branch to return a
        //   null pointer for a non-existent name, which must be tested
        //   explicitly.
        //
        // Plan:
        //   1) Specify a schema having a single record definition having
        //      several fields named according to the 'gg' convention.  In a
        //      loop, use the loop index and the (implicitly tested) 'gg'
        //      helpers to independently generate the expected results of
        //      'fieldName' and 'fieldIndex'
        //   2) Because there is no independent access to the field definitions
        //      of a record definition except via the accessors under test,
        //      'gg' cannot be conveniently used to test 'lookupField' and
        //      'field' (which return a 'bdem_FieldDef' pointer and
        //      reference, respectively).  Instead, the return values from the
        //      append methods are retained for use as the expected results
        //      as the record definition is built up before testing the
        //      accessors.
        //
        // Testing:
        //   const bdem_FieldDef& field(int index) const;
        //   const bdem_FieldDef *lookupField(const char *name) const;
        //   int fieldIndex(const char *name) const;
        //   const char *fieldName(int index) const;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
            << "Testing Field-Related 'bdem_RecordDef' Accessors" << bsl::endl
            << "================================================" << bsl::endl;

        if (verbose) bsl::cout << "\nTesting 'fieldIndex' and 'fieldName'"
                               << bsl::endl;
        {
            const char SPEC[] = ":*  V* Aa Bb Cc Dd Ee Ff Gg Hh Ii Jj";
            if (veryVerbose) { T_();  P(SPEC); }

            Obj x(Z);  const Obj& X = x;  gg(&x, SPEC);

            ASSERT(1 == X.numRecords());  // sanity check on 'SPEC'

            const RecDef& R          = X.record(0);
            const int     NUM_FIELDS = R.numFields();

            if (verbose) bsl::cout << "\n\twith non-null names:" << bsl::endl;
            for (int i = 1; i < NUM_FIELDS; ++i) {
                const char *expName = ggName(i);
                if (veryVerbose) { T_();  T_();  P_(i);  P(expName); }
                LOOP_ASSERT(i, 0 == strCmp(expName, R.fieldName(i)));
                LOOP_ASSERT(i, i == R.fieldIndex(expName));
            }

            if (verbose) bsl::cout << "\n\twith a null name:" << bsl::endl;

            // Note: Right now, a null 'const char *' is "Undefined Behavior"
            // ASSERT(0 > R.fieldIndex(0));
            ASSERT(0 == R.fieldName(0));

            if (verbose) bsl::cout << "\n\twith non-existent names:"
                                   << bsl::endl;

            ASSERT(0 > R.fieldIndex("foo"));
            ASSERT(0 > R.fieldIndex("bar"));
        }

        if (verbose) bsl::cout << "\nTesting 'field' and 'lookupField'."
                               << bsl::endl;
        {
            const char *NAMES[] = { 0, "", "a", "b", "longer name" };

            const int NUM_FLD_NAMES = sizeof NAMES / sizeof(char *);

            Obj x(Z);

            RecDef *r = x.createRecord();  const RecDef& R = *r;
            const FldDef *F[NUM_FLD_NAMES];

            // First build up the whole record definition before testing.
            int i;
            for (i = 0; i < NUM_FLD_NAMES; ++i) {
                F[i] = r->appendField(ggElemTypes[i % NUM_TYPES], NAMES[i]);
                if (veryVerbose) { T_();  T_();  P(R.numFields()); }

                LOOP_ASSERT(i, i + 1 == R.numFields());
            }

            if (verbose) bsl::cout << "\n\twith non-null names:" << bsl::endl;
            for (i = 1; i < NUM_FLD_NAMES; ++i) {
                if (veryVerbose) { T_();  P_(i);  P(R.fieldName(i)); }
                LOOP_ASSERT(i, F[i] == R.lookupField(NAMES[i]));
                LOOP_ASSERT(i, F[i] == &R.field(i));
            }

            if (verbose) bsl::cout << "\n\twith a null name:" << bsl::endl;

            // Note: Right now, a null 'const char *' is "Undefined Behavior"
            //ASSERT(   0 ==  R.lookupField(0));
            ASSERT(F[0] == &R.field(0));

            if (verbose) bsl::cout << "\n\twith non-existent names:"
                                   << bsl::endl;

            ASSERT(0 == R.lookupField("foo"));
            ASSERT(0 == R.lookupField("bar"));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING ENUMERATION-RELATED 'bdem_EnumerationDef' ACCESSORS
        //
        // Concerns:
        //   Each of the accessors under test directly returns either a data
        //   member, or a single operation or method on a data member, and so
        //   our concerns are minimal.  The 'enumName' helper returns a simple
        //   composition of tested functionality (some of which is tested in
        //   this case).  Therefore, a few varied test records will suffice
        //   to confirm correct behavior.
        //
        // Plan:
        //   Specify a single schema value having N named and unnamed
        //   enumerations of various lengths.  In a single loop over the N
        //   enumerations, verify that each accessor under test returns the
        //   expected result.  Note that, as an extra sanity test of the
        //   'schema' method, two independent schema objects x and y are
        //   created, so that 'schema' can exhibit two independent return
        //   values.
        //
        // Testing:
        //   int numEnumerators() const;
        //   int enumerationIndex() const;
        //   const bdem_Schema& schema() const;
        //   const char *enumName(const bdem_EnumerationDef&);
        //                                               (test helper function)
        //   static bool canHaveEnumConstraint(bdem_ElemType::Type type);
        //   int maxId() const;
        //   pair<const char *, int> nextLargerName(const char* name) const;
        //   pair<const char *, int> nextLargerId(int id) const;
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "Testing Enumeration-related 'bdem_EnumerationDef' "
                      "Accessors"
                      << bsl::endl
                      << "=================================================="
                      "========="
                      << bsl::endl;
        {
          if (verbose)
              bsl::cout << "\nTesting , 'numEnumerations', 'enumerationIndex',"
                           " 'schema', and the helper function 'enumName'."
                        << bsl::endl;

          const int   NUM_ENUMS = 5;
          const char  SPEC[]   =  ":*= :a=a :w=ab :c=bcd :d=fghijkl";
          const int   expLen[NUM_ENUMS]  = { 0, 1, 2, 3, 7 };
          const char *expName[NUM_ENUMS] = { 0, "a", "doubleU", "c", "d" };

          Obj x(Z);  const Obj& X = x;  gg(&x, SPEC);
          Obj y(Z);  const Obj& Y = y;  gg(&y, SPEC);  // independent "copy"

          ASSERT(&x != &y);  // sanity check for 'schema()' test

          if (veryVeryVerbose) X.print(bsl::cout);

          for (int i = 0; i < NUM_ENUMS; ++i) {
              LOOP_ASSERT(i, expLen[i] == X.enumeration(i).numEnumerators());
              LOOP_ASSERT(i,         i == X.enumeration(i).enumerationIndex());
              LOOP_ASSERT(i,        &x == &X.enumeration(i).schema());
              LOOP_ASSERT(i,        &y == &Y.enumeration(i).schema());

              LOOP_ASSERT(i,
                          0 == strCmp(expName[i], enumName(X.enumeration(i))));
          }
        }

        {
          if (verbose)
              bsl::cout << "\nTesting , 'canHaveEnumConstraint'." << bsl::endl;

          for (int ii = 0; ii < NUM_TYPES; ++ii) {
            EType::Type type = ggElemTypes[ii];

            if (veryVerbose) EType::toAscii(type);

            if (EType::BDEM_INT          == type
             || EType::BDEM_INT_ARRAY    == type
             || EType::BDEM_STRING       == type
             || EType::BDEM_STRING_ARRAY == type) {
                LOOP_ASSERT(ii, EnumDef::canHaveEnumConstraint(type));
            } else {
                LOOP_ASSERT(ii, !EnumDef::canHaveEnumConstraint(type));
            }
          }
        }

        {
          if (verbose)
              bsl::cout << "\nTesting , 'maxId', 'nextLargerName',"
                           " and 'nextLargerId'."
                        << bsl::endl;

          Obj x(Z);  const Obj& X = x;
          EnumDef *e = x.createEnumeration();
          ASSERT(e);

          // test non-empty enumeration
          const struct {
            int         d_line;
            const char *d_name;
            int         d_id;
            int         d_maxId;
          } DATA[] = {
          // Line  Name  Id            Max Id
          // ====  ====  ===========   ================
            { L_,  "a",  INT_MIN + 1,   INT_MIN + 1     },
            { L_,  "b",  INT_MIN / 2,   INT_MIN / 2     },
            { L_,  "c",      -1,           -1           },
            { L_,  "d",      -5,           -1           },
            { L_,  "e",       0,            0           },
            { L_,  "f",       1,            1,          },
            { L_,  "g",  INT_MAX / 2,   INT_MAX / 2     },
          };

          const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

          for (int ii = 0; ii < NUM_DATA; ++ii) {
            const int         LINE = DATA[ii].d_line;
            const char *const NAME = DATA[ii].d_name;
            const int         ID   = DATA[ii].d_id;
            const int         MID  = DATA[ii].d_maxId;

            // Added enumerators as described in test vector.
            int returnedId = e->addEnumerator(NAME, ID);
            LOOP_ASSERT(LINE, returnedId == ID);
            LOOP_ASSERT(LINE, e->maxId() == MID);
          }

          // Test that if a id (x) was add that is less then the last id (y)
          // added that the maxId will be x + 1 and not y + 1.  Last id
          // added to e was INT_MAX, so add an enumerator at INT_MAX / 4
          // maxId should be INT_MAX / 4 + 1
          int returnedId = e->addEnumerator("h", INT_MAX / 4);
          LOOP2_ASSERT(returnedId, INT_MAX / 4, returnedId  == INT_MAX / 4);
          ASSERT(e->maxId() == INT_MAX / 2);

          // test 'nextLargerName' and 'nextLargerId' on an empty enumeration
          EnumDef *e1 = x.createEnumeration();
          ASSERT(e1);

          bsl::pair<const char *, int> namePair = e1->nextLargerName("A");
          ASSERT(0 == namePair.first);
          ASSERT(bdetu_Unset<int>::unsetValue() == namePair.second);

          bsl::pair<const char *, int> idPair = e1->nextLargerId(0);
          ASSERT(0 == idPair.first);
          ASSERT(bdetu_Unset<int>::unsetValue() == idPair.second);

          // built up new enumeration from 'DATA'
          for (int ii = 0; ii < NUM_DATA; ++ii) {
            const int         LINE = DATA[ii].d_line;
            const char *const NAME = DATA[ii].d_name;
            const int         ID   = DATA[ii].d_id;

            // Added enumerators as described in test vector.
            int returnedId = e1->addEnumerator(NAME, ID);
            LOOP_ASSERT(LINE, returnedId == ID);
          }

          namePair = e1->nextLargerName(0);
          idPair   = e1->nextLargerId(INT_MIN);
          // build up new enumeration from 'DATA'
          for (int ii = 0; ii < NUM_DATA; ++ii) {
            const int         LINE = DATA[ii].d_line;
            const char *const NAME = DATA[ii].d_name;
            const int         ID   = DATA[ii].d_id;

            if (veryVerbose) {
                T_(); bsl::cout << "EXPECTED: "; P(NAME);
                T_(); bsl::cout << "ACTUAL  : "; P(namePair.first);
            }
            // Added enumerators as described in test vector.
            LOOP_ASSERT(LINE, 0 == strCmp(namePair.first, NAME));

            if (veryVerbose) {
                T_(); bsl::cout << "EXPECTED: "; P(ID);
                T_(); bsl::cout << "ACTUAL  : "; P(namePair.second);
            }
            LOOP_ASSERT(LINE, namePair.second == ID);

            namePair = e1->nextLargerName(NAME);
            idPair   = e1->nextLargerId(ID);
          }

          // Test 'name' and 'id' > then last enumerator, which was set in the
          // last loop iteration above.
          ASSERT(0 == namePair.first);
          ASSERT(bdetu_Unset<int>::unsetValue() == namePair.second);

          ASSERT(0 == idPair.first);
          ASSERT(bdetu_Unset<int>::unsetValue() == idPair.second);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING RECORD-RELATED 'bdem_RecordDef' ACCESSORS
        //
        // Concerns:
        //   Each of the accessors under test directly returns either a data
        //   member, or a single operation or method on a data member, and so
        //   our concerns are minimal.  The 'recName' helper returns a simple
        //   composition of tested functionality (some of which is tested in
        //   this case).  Therefore, a few varied test records will suffice
        //   to confirm correct behavior.
        //
        // Plan:
        //   Specify a single schema value having N named and unnamed records
        //   of various lengths.  In a single loop over the N records, verify
        //   that each accessor under test returns the expected result.  Note
        //   that, as an extra sanity test of the 'schema' method, two
        //   independent schema objects x and y are created, so that 'schema'
        //   can exhibit two independent return values.
        //
        // Testing:
        //   int numFields() const;
        //   int recordIndex() const;
        //   const bdem_Schema& schema() const;
        //   const char *recName(const bdem_RecordDef&); (test helper function)
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "Testing Record-related 'bdem_RecordDef' Accessors"
                      << bsl::endl
                      << "================================================="
                      << bsl::endl;

        if (verbose)
            bsl::cout << "\nTesting 'length', 'recordIndex', 'schema', "
                         "and the helper function 'recName'." << bsl::endl;

        const int   NUM_RECS = 6;
        const char  SPEC[]   =  ":* :e :a :bAa :cBbCcDd :dEeFfGgHhIiJj+*b";
        const int   expLen[NUM_RECS]  = { 0, 0, 0, 1, 3, 7 };
        const char *expName[NUM_RECS] = { 0, "", "a", "b", "c", "d" };

        Obj x(Z);  const Obj& X = x;  gg(&x, SPEC);
        Obj y(Z);  const Obj& Y = y;  gg(&y, SPEC);  // independent "copy"

        ASSERT(&x != &y);  // sanity check for 'schema()' test

        for (int i = 0; i < NUM_RECS; ++i) {
            LOOP_ASSERT(i, expLen[i] ==  X.record(i).numFields());
            LOOP_ASSERT(i,         i ==  X.record(i).recordIndex());
            LOOP_ASSERT(i,        &x == &X.record(i).schema());
            LOOP_ASSERT(i,        &y == &Y.record(i).schema());

            LOOP_ASSERT(i, 0 == strCmp(expName[i], recName(X.record(i))));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Schema' ACCESSORS
        //
        // Concerns:
        //   For convenience, the (logical) schema accessors are tested in four
        //   test cases.  The 'bdem_Schema' accessors are tested first, since
        //   they represent the most "shallow" probing of the structure of a
        //   schema.
        //
        //   'length' must call the 'length' method of the appropriate data
        //   member; light probing with a few test schema is sufficient.
        //
        //   'recordName' must forward its argument to the correct method of
        //   the correct data member, and always returns a valid (possibly
        //   null) name.  'recordIndex'  makes a similar call, except the
        //   specific behavior is more complicated: a null name and names
        //   not in the schema must return a negative value.
        //
        //   'record' and 'lookupRecord' are substantially parallel to
        //   'recordName' and 'recordIndex', except for the return type, and
        //   that 'lookupRecord' actually implements the branch to return a
        //   null pointer for a null or non-existent name, which must be
        //   tested explicitly.
        //
        // Plan:
        //   1) Specify a set S of schema of various lengths and confirm that
        //      'length' returns the expected values.
        //   2) Specify a single schema having several records sequentially
        //      named according to the 'gg' convention.  In a loop, use the
        //      loop index and the (implicitly tested) 'gg' helpers to
        //      independently generate the expected results of 'recordName' and
        //      'recordIndex'
        //   3) Because there is no independent access to the record
        //      definitions of a schema except via the accessors under test,
        //      'gg' cannot be conveniently used to test 'lookupRecord' and
        //      'record' (which return a 'bdem_RecordDef' pointer and
        //      reference, respectively).  Instead, the return values from
        //      'createRecord' are retained for use as the expected results
        //      as the schema is built up before testing the accessors.
        //
        // Testing:
        //   int numRecords() const;
        //   int numEnumerations() const;
        //   const bdem_RecordDef *lookupRecord(const char *name) const;
        //   const bdem_RecordDef& record(int index) const;
        //   int recordIndex(const char *name) const;
        //   const char *recordName(int index) const;
        //   const EnumerationDef *lookupEnumeration(const char *name) const;
        //   const bdem_EnumerationDef& enumeration(int index) const;
        //   int enumerationIndex(const char *name) const;
        //   const char *enumerationName(int index) const;
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << "\nTesting 'bdem_Schema' Accessors"
                      << "\n===============================" << bsl::endl;

        if (verbose) bsl::cout << "\nTesting bdem_Schema::numRecords()."
                               << bsl::endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // schema spec
                int         d_numRecords;   // number of records specified
            } DATA[] = {
                //line  spec                                          num rec
                //----  -----------------------------------------      ------
                { L_,   "",                                             0    },
                { L_,   ":a",                                           1    },
                { L_,   ":a AaBb",                                      1    },
                { L_,   ":a :b",                                        2    },
                { L_,   ":a AaBb :b CcDdEe",                            2    },
                { L_,   ":a AaBb :b CcDdEe :c FfG*H* :d I*J*K* :e :f",  6    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const char *const SPEC   = DATA[i].d_spec;
                const int   NUMRECORDS = DATA[i].d_numRecords;
                if (veryVerbose) { T_();  P_(NUMRECORDS);  P(SPEC); }

                Obj x(Z);  const Obj& X = x;  gg(&x, SPEC);

                LOOP_ASSERT(LINE, NUMRECORDS == X.numRecords());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdem_Schema::numEnumerations()."
                               << bsl::endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // schema spec
                int         d_numEnumerations;   // number of enums specified
            } DATA[] = {
                //line  spec                                          num enum
                //----  -----------------------------------------     ------
                { L_,   "",                                            0    },
                { L_,   ":a=",                                         1    },
                { L_,   ":a= ab",                                      1    },
                { L_,   ":a= :b=",                                     2    },
                { L_,   ":a= ab :b= cd",                               2    },
                { L_,   ":a= ab :b= cd :c= fg :d= hi :e= jk :f= lm",   6    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE         = DATA[i].d_lineNum;
                const char *const SPEC         = DATA[i].d_spec;
                const int      NUMENUMERATIONS = DATA[i].d_numEnumerations;
                if (veryVerbose) { T_();  P_(NUMENUMERATIONS);  P(SPEC); }

                Obj x(Z);  const Obj& X = x;  gg(&x, SPEC);

                LOOP_ASSERT(LINE, NUMENUMERATIONS == X.numEnumerations());
            }
        }

        if (verbose)
            bsl::cout << "\nTesting 'recordIndex' and 'recordName'"
                      << bsl::endl;
        {
            const char SPEC[] =
                      ":*V* :aAa :bBb :cCc :dDd :eEe :fFf :gGg :hHh :iIi :jJj";
            if (veryVerbose) { T_();  P(SPEC); }

            Obj x(Z);  const Obj& X = x;  gg(&x, SPEC);

            const int NUM_RECORDS = X.numRecords();

            if (verbose) bsl::cout << "\n\twith non-null names:" << bsl::endl;
            for (int i = 1; i < NUM_RECORDS; ++i) {
                const char *expName = ggName(i);
                if (veryVerbose) { T_();  T_();  P_(i);  P(expName); }
                LOOP_ASSERT(i, 0 == strCmp(expName, X.recordName(i)));
                LOOP_ASSERT(i, i == X.recordIndex(expName));
            }

            if (verbose) bsl::cout << "\n\twith a null name:" << bsl::endl;

            // Note: Right now, a null 'const char *' is "Undefined Behavior"
            // ASSERT(0 > X.recordIndex(0));
            ASSERT(0 == X.recordName(0));

            if (verbose) bsl::cout << "\n\twith non-existent names:"
                                   << bsl::endl;

            ASSERT(0 > X.recordIndex("foo"));
            ASSERT(0 > X.recordIndex("bar"));
        }

        if (verbose)
            bsl::cout << "\nTesting 'enumerationIndex' and 'enumerationName'"
                      << bsl::endl;
        {
            const char SPEC[] =
                      ":*=v :a=a :b=b :c=c :d=d :f=f :g=g :h=h :i=i :j=j :k=k";
            const char OUT[] = "*abcdfghijk";
            if (veryVerbose) { T_();  P(SPEC); T_(); P(OUT); }

            Obj x(Z);  const Obj& X = x;  gg(&x, SPEC);

            const int NUM_ENUMERATIONS = X.numEnumerations();

            if (verbose) bsl::cout << "\n\twith non-null names:" << bsl::endl;
            for (int i = 1; i < NUM_ENUMERATIONS; ++i) {
                const char *expName = getName(OUT[i]);
                if (veryVerbose) { T_();  T_();  P_(i);  P(expName); }
                LOOP_ASSERT(i, 0 == strCmp(expName, X.enumerationName(i)));
                LOOP_ASSERT(i, i == X.enumerationIndex(expName));
            }

            if (verbose) bsl::cout << "\n\twith a null name:" << bsl::endl;

            // Note: Right now, a null 'const char *' is "Undefined Behavior"
            // ASSERT(0 > X.enumerationIndex(0));
            ASSERT(0 == X.enumerationName(0));

            if (verbose) bsl::cout << "\n\twith non-existent names:"
                                   << bsl::endl;

            ASSERT(0 > X.enumerationIndex("foo"));
            ASSERT(0 > X.enumerationIndex("bar"));
        }

        if (verbose) bsl::cout << "\nTesting 'record' and 'lookupRecord'."
                               << bsl::endl;
        {
            const char *NAMES[] = { 0, "", "a", "b", "longer name" };

            const int NUM_REC_NAMES = sizeof NAMES / sizeof(char *);

            Obj x(Z);  const Obj& X = x;

            const RecDef *R[NUM_REC_NAMES];

            // First build up the whole schema before testing
            {
                for (int i = 0; i < NUM_REC_NAMES; ++i) {
                    R[i] = x.createRecord(NAMES[i]);
                    if (veryVerbose) { T_();  T_();  P(X.numRecords()); }

                    LOOP_ASSERT(i, i + 1 == X.numRecords());
                }
            }

            if (verbose) bsl::cout << "\n\twith non-null names:" << bsl::endl;
            {
                for (int i = 1; i < NUM_REC_NAMES; ++i) {
                    if (veryVerbose) { T_();  P_(i);  P(X.recordName(i)); }
                    LOOP_ASSERT(i, R[i] == X.lookupRecord(NAMES[i]));
                    LOOP_ASSERT(i, R[i] == &X.record(i));
                }
            }

            if (verbose) bsl::cout << "\n\twith a null name:" << bsl::endl;

            // Note: Right now, a null 'const char *' is "Undefined Behavior"
            //ASSERT(   0 ==  X.lookupRecord(0));
            ASSERT(R[0] == &X.record(0));

            if (verbose) bsl::cout << "\n\twith non-existent names:"
                                   << bsl::endl;

            ASSERT(0 == X.lookupRecord("foo"));
            ASSERT(0 == X.lookupRecord("bar"));
        }

        if (verbose)
            bsl::cout << "\nTesting 'enumeration' and 'lookupEnumeration'."
                      << bsl::endl;
        {
            const char *NAMES[] = { 0, "", "a", "b", "longer name" };

            const int NUM_ENUM_NAMES = sizeof NAMES / sizeof(char *);

            Obj x(Z);  const Obj& X = x;

            const EnumDef *E[NUM_ENUM_NAMES];

            // First build up the whole schema before testing
            {
                for (int i = 0; i < NUM_ENUM_NAMES; ++i) {
                    E[i] = x.createEnumeration(NAMES[i]);
                    if (veryVerbose) { T_();  T_();  P(X.numEnumerations()); }

                    LOOP_ASSERT(i, i + 1 == X.numEnumerations());
                }
            }

            if (verbose) bsl::cout << "\n\twith non-null names:" << bsl::endl;
            {
                for (int i = 1; i < NUM_ENUM_NAMES; ++i) {
                    if (veryVerbose) { T_(); P_(i); P(X.enumerationName(i)); }
                    LOOP_ASSERT(i, E[i] == X.lookupEnumeration(NAMES[i]));
                    LOOP_ASSERT(i, E[i] == &X.enumeration(i));
                }
            }

            if (verbose) bsl::cout << "\n\twith a null name:" << bsl::endl;

            // Note: Right now, a null 'const char *' is "Undefined Behavior"
            //ASSERT(0 ==  X.lookupRecord(0));
            ASSERT(E[0] == &X.enumeration(0));

            if (verbose) bsl::cout << "\n\twith non-existent names:"
                                   << bsl::endl;

            ASSERT(0 == X.lookupEnumeration("foo"));
            ASSERT(0 == X.lookupEnumeration("bar"));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'gg' GENERATOR
        //
        // Concerns:
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
        //   bdem_Schema& gg(bdem_Schema *s, const char *spec); (generator fnc)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting 'gg'"
                               << "\n============" << bsl::endl;

        Obj x1(Z);  const Obj& X1 = x1;

        {
            if (verbose) bsl::cout << "\nTesting 'gg' with empty string."
                                   << bsl::endl;
            gg(&x1, "");
            ASSERT(0 == X1.numRecords());
            ASSERT(0 == X1.numEnumerations());
        }

        {
            if (verbose)
                bsl::cout << "\nTesting 'gg' with all unconstrained types."
                          << bsl::endl;

            int i;  // loop variables, declared here to keep MSVC++ happy
            for (i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :* %*"; // The % gets overwritten.
                spec[5] = bdemType[i];

                if (veryVerbose) bsl::cout << "testing 'gg' using " << spec
                                           << bsl::endl;

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

            if (verbose) bsl::cout << "\nAll unconstrained types, with name c "
                                   << "(\"c\")." << bsl::endl;
            for (i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :* %c"; // The % gets overwritten.
                spec[5] = bdemType[i];

                if (veryVerbose) bsl::cout << "testing 'gg' using " << spec
                                           << bsl::endl;

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

            if (verbose) bsl::cout << "\nAll unconstrained types, with name c "
                                   << "(\"c\") and record name d (\"d\")."
                                   << bsl::endl;
            for (i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :d %c"; // The % gets overwritten.
                spec[5] = bdemType[i];

                if (veryVerbose) bsl::cout << "testing 'gg' using " << spec
                                           << bsl::endl;

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
                bsl::cout << "\nTesting 'gg' with all constrained types."
                          << bsl::endl;
            for (i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // '%' gets replaced in following specs:
                static const char recSpec[]  = "~ :c% Ad :* %dc";
                static const char enumSpec[] = "~ :c= a2 :* %dc";
                char spec[50];
                const char bdemType = constrBdemType[i];
                if ('$' == bdemType || '^' == bdemType ||
                    '!' == bdemType || '/' == bdemType) {
                    // Enumeration specification
                    bsl::strcpy(spec, enumSpec);
                }
                else {
                    // Record (sequence or choice) specification
                    bsl::strcpy(spec, recSpec);
                    spec[4]  = '%' == bdemType || '@' == bdemType ? '?' : ' ';
                }
                spec[12] = bdemType;

                if (veryVerbose) bsl::cout << "testing 'gg' using " << spec
                                           << bsl::endl;

                gg(&x1, spec);

                const RecDef *rec = 0;
                if ('$' == bdemType || '^' == bdemType ||
                    '!' == bdemType || '/' == bdemType) {

                    LOOP_ASSERT(i, 1 == X1.numRecords());
                    LOOP_ASSERT(i, 1 == X1.numEnumerations());

                    const EnumDef *en = &X1.enumeration(0);

                    LOOP_ASSERT(i, 0 == strCmp(enumName(*en), "c"));
                    LOOP_ASSERT(i, 1 == en->numEnumerators());
                    LOOP_ASSERT(i, 0 == strCmp(en->lookupName(2), "a"));

                    rec = &X1.record(0);
                    LOOP_ASSERT(i, &X1.enumeration(0) ==
                                rec->field(0).enumerationConstraint());
                }
                else {
                    LOOP_ASSERT(i, 2 == X1.numRecords());
                    LOOP_ASSERT(i, 0 == X1.numEnumerations());

                    rec = &X1.record(0);

                    LOOP_ASSERT(i, 0 == strCmp(recName(*rec), "c"));
                    LOOP_ASSERT(i, 1 == rec->numFields());
                    LOOP_ASSERT(i, 0 == strCmp(rec->fieldName(0), "d"));
                    LOOP_ASSERT(i, ggElemTypes[0] == rec->field(0).elemType());

                    rec = &X1.record(1);
                    LOOP_ASSERT(i, &X1.record(0) ==
                                rec->field(0).recordConstraint());
                }
                LOOP_ASSERT(i, 0 == recName(*rec));
                LOOP_ASSERT(i, 1 == rec->numFields());
                LOOP_ASSERT(i, 0 == strCmp(rec->fieldName(0), "d"));
                ASSERT(ggElemTypes[32 + i]   == rec->field(0).elemType());

                if (veryVerbose)  PS(X1);
            }

            if (verbose) bsl::cout << "\nTesting 'gg' with a large invocation "
                                   << "touching every token." << bsl::endl;
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

                    // Add constrained fields by name on previous record.
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

                    // Add constrained fields by index on all previous records,
                    // and on current record.  Note that this can only
                    // reference the first 10 records, since we use a single
                    // digit for reference by index.
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

                if (verbose) bsl::cout << "Testing 'gg' using " << spec
                                       << bsl::endl;

                gg(&x1, spec.data());
                ASSERT(NUM_GG_NAMES == X1.numRecords());

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
                Obj x1(Z);  const Obj& X1 = x1;

                if (verbose)
                        bsl::cout << "\nForce a test of name transition in "
                                  << "state REC_NAME." << bsl::endl;
                gg(&x1, ":a");

                ASSERT(1 == X1.numRecords());
                ASSERT(0 == strCmp(recName(X1.record(0)), "a"));

                if (verbose)
                    bsl::cout << "\nMake sure 'gg' is additive if '~' is "
                              << "not present, although this is NOT "
                              << "normal usage." << bsl::endl;
                gg(&x1, ":b");

                ASSERT(2 == X1.numRecords());
                ASSERT(0 == strCmp(recName(X1.record(0)), "a"));
                ASSERT(0 == strCmp(recName(X1.record(1)), "b"));
            }

            {
                Obj x1(Z);  const Obj& X1 = x1;

                if (verbose)
                    bsl::cout << "\nForce a test of remove transition in "
                              << "state FLD_END." << bsl::endl;
                gg(&x1, ":aAa~:a");

                ASSERT(1 == X1.numRecords());
                ASSERT(0 == strCmp(recName(X1.record(0)), "a"));
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //
        // Concerns:
        //   Since the logical "schema" object is implemented using three
        //   classes within this component, our concerns are just a bit broader
        //   than for the primary manipulators of a "standard" value-semantic
        //   component.
        //
        //   The first concern is that the default 'bdem_Schema' constructor
        //   work properly, with any (or no) valid allocator supplied.  The
        //   destructor is tested (implicitly) in each of the several blocks
        //   that comprise this test case.
        //
        //   Next, the true 'bdem_Schema' manipulator 'createRecord', with or
        //   without a supplied record name, must produce the expected results
        //   *and* must return a pointer to the newly (privately) created
        //   record definition.  That record definition must in turn be
        //   successfully augmented using the 'bdem_RecordDef' and
        //   'bdem_EnumerationDef manipulators.  Finally, 'removeAll' should
        //   produce the expected (empty) schema (although no subsequent
        //   white-box testing is needed).
        //
        //   All of the above tests should succeed in the presence of 'bdema'
        //   exceptions.
        //
        // Plan:
        //   1) Create a schema using the default constructor:
        //    - With and without passing in an allocator.
        //    - In the presence of exceptions during memory allocations using
        //      a 'bslma_TestAllocator' and varying its *allocation* *limit*.
        //    - Where the object is constructed entirely in static memory
        //      (using a 'bslma_BufferAllocator') and never destroyed.
        //
        //   2) Create an empty schema, add an unnamed and a named record and
        //      enumeration, and let the schema go out of scope.
        //
        //   3) Create an empty schema, add many records and enumerations,
        //      and remove them.
        //
        //   4) Create a schema and, in a loop of two iterations:
        //      a) append several new records and enumerations.
        //      b) to each newly-created record, append many and varied
        //         fields using, in turn, each of the 'bdem_RecordDef' and
        //         'bdem_EnumerationDef' manipulators.  In particular, attempt
        //         to add both fields (with and with out enumeration
        //         constraints to records) and records/enumeration (to the
        //         schema) having duplicate names, and confirm that these
        //         attempts do not succeed, but otherwise leave the schema
        //         valid.
        //      c) call 'removeAll' (and return to the top of the loop, thus
        //         testing the 'removeAll' method by "reusing" the schema).
        //
        // Testing:
        //  ^bdem_FieldDef(*name, int id, const bdem_FieldSpec&, *ba);
        //  ^~bdem_FieldDef();                 // purify
        //
        //  ^bdem_RecordDef(const Schema *s, index, *name, type, *pool, *ba);
        //  ^~bdem_RecordDef();                // purify
        //  ^void setRecordName(const char *name);
        //   const FldDef *appendField(BDEM_LIST, const RecDef&, *name=0);
        //   const FldDef *appendField(BDEM_TABLE, const RecDef&, *name=0);
        //   const FldDef *appendField(EType type, *name=0, id=Null);
        //   const FldDef *appendField(type, *c, *name=0, id=Null);
        //   const FldDef *appendField(BDEM_INT, *enum, *name=0);
        //   const FldDef *appendField(BDEM_STRING, *enum, *name=0);
        //   const FldDef *appendField(BDEM_STRING_ARRAY, *enum, *name=0);
        //   const FldDef *appendField(BDEM_INT_ARRAY, *enum, *name=0);
        //   const FldDef *appendField(type, *enum, *name=0);
        //
        //  ^bdem_EnumerationDef(const Schema *s, index, *name, woa);
        //  ^~bdem_EnumerationDef();
        //  ^setEnumerationName();
        //   int addEnumerator(const char *name);
        //   int addEnumerator(const char *name, int id);
        //   void assignAlphabeticalIDs(int firstId = 0);
        //
        //   bdem_Schema(bslma_Allocator *ba=0);
        //   ~bdem_Schema();                   // purify
        //   bdem_RecordDef *createRecord(*name=0, type=BDEM_SEQUENCE_RECORD);
        //   bdem_EnumerationDef *createEnumeration(const char *name = 0);
        //   void removeAll();
        //
        // Note: '^' indicates a private method, which is tested indirectly
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Primary Manipulators"
                               << "\n============================"
                               << bsl::endl;

        if (verbose) bsl::cout << "\nTesting 'bdem_Schema()' Constructor."
                               << bsl::endl;
        if (verbose) bsl::cout << "\tWithout passing in an allocator."
                               << bsl::endl;
        {
            const Obj X((bslma_Allocator *)0);
            if (veryVerbose) { T_(); T_(); PS(X); }
            ASSERT(0 == X.numRecords());
            ASSERT(0 == X.numEnumerations());
        }

        if (verbose) bsl::cout << "\tPassing in an allocator." << bsl::endl;
        if (verbose) bsl::cout << "\t\tWith no exceptions." << bsl::endl;
        {
            const Obj X(Z);
            if (veryVerbose) { T_(); T_(); PS(X); }
            ASSERT(0 == X.numRecords());
            ASSERT(0 == X.numEnumerations());
        }

        if (verbose) bsl::cout << "\t\tWith exceptions." << bsl::endl;
        {
          BEGIN_BSLMA_EXCEPTION_TEST {
            if (veryVerbose)
                bsl::cout << "\tTesting Exceptions In Default Ctor"
                          << bsl::endl;
            const Obj X(&testAllocator);
            if (veryVerbose) { T_(); T_(); PS(X); }
            ASSERT(0 == X.numRecords());
            ASSERT(0 == X.numEnumerations());
          } END_BSLMA_EXCEPTION_TEST
        }

        if (verbose) bsl::cout << "\tIn place using a buffer allocator."
                               << bsl::endl;
        {
            char memory[1024];
            bdema_BufferedSequentialAllocator a(memory, sizeof memory);
            void *doNotDelete = new(a.allocate(sizeof(Obj))) Obj(&a);
            ASSERT(doNotDelete);

            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }

        // --------------------------------------------------------------------

        if (verbose)
                bsl::cout << "\nTesting 'createRecord' and 'bdem_RecordDef' "
                          << "constructor." << bsl::endl;
        {
          BEGIN_BSLMA_EXCEPTION_TEST {
            Obj x1(Z);  const Obj& X1 = x1;
            ASSERT(0 == X1.numRecords());
            ASSERT(0 == X1.numEnumerations());

            RecDef *rAnon = x1.createRecord();
            ASSERT(1     == X1.numRecords());
            ASSERT(0     == X1.numEnumerations());
            ASSERT(rAnon == &x1.record(0));
            ASSERT(0     == recName(*rAnon));
            ASSERT(0      > X1.recordIndex("A"));

            RecDef *rA = x1.createRecord("A");
            ASSERT(2     == X1.numRecords());
            ASSERT(0     == X1.numEnumerations());
            ASSERT(1     == X1.recordIndex("A"));
            ASSERT(rA    == &X1.record(1));
            ASSERT(rAnon == &X1.record(0));
            ASSERT(0     == strCmp(recName(*rA), "A"));

            x1.removeAll();
            ASSERT(0 == X1.numRecords());
            ASSERT(0 == X1.numEnumerations());
            ASSERT(0  > X1.recordIndex("A"));
          } END_BSLMA_EXCEPTION_TEST
        }

        {
          BEGIN_BSLMA_EXCEPTION_TEST {
            Obj x1(Z);  const Obj& X1 = x1;
            ASSERT(0 == x1.numRecords());
            ASSERT(0 == X1.numEnumerations());

            RecDef *rA = x1.createRecord("A");
            ASSERT(1  == X1.numRecords());
            ASSERT(0  == X1.numEnumerations());
            ASSERT(0  == X1.recordIndex("A"));
            ASSERT(rA == &X1.record(0));
            ASSERT(0  == bsl::strcmp(recName(*rA), "A"));

            for (int i = 0; i < 10; ++i) {
                RecDef *rAnon = x1.createRecord();
                LOOP_ASSERT(i, i + 2 == X1.numRecords());
                LOOP_ASSERT(i, 0     == X1.numEnumerations());
                LOOP_ASSERT(i, 0     == X1.recordIndex("A"));
                LOOP_ASSERT(i, rA    == &X1.record(0));
                LOOP_ASSERT(i, rAnon == &X1.record(i + 1));
                LOOP_ASSERT(i, 0     == recName(*rAnon));
            }

            RecDef *rB = x1.createRecord("B");
            ASSERT(12 == X1.numRecords());
            ASSERT(0  == X1.numEnumerations());
            ASSERT(11 == X1.recordIndex("B"));
            ASSERT(0  == X1.recordIndex("A"));
            ASSERT(rB == &X1.record(11));
            ASSERT(rA == &X1.record(0));
            ASSERT(0  == bsl::strcmp(recName(*rB), "B"));

            if (veryVerbose) PS(X1);

            // Can not add a enum with the same name as a previously add record
            EnumDef *eA = x1.createEnumeration("A");
            ASSERT(!eA);

            x1.removeAll();
            ASSERT(0 == X1.numRecords());
            ASSERT(0 == X1.numEnumerations());
            ASSERT(0  > X1.recordIndex("A"));
            ASSERT(0  > X1.recordIndex("B"));
            ASSERT(0  > X1.recordIndex(""));
          } END_BSLMA_EXCEPTION_TEST
        }

        if (verbose)
            bsl::cout << "\nTesting 'createEnumeration' and "
                      << "'bdem_EnumerationDef' constructor."
                      << bsl::endl;
        {
          BEGIN_BSLMA_EXCEPTION_TEST {
            Obj x1(Z);  const Obj& X1 = x1;
            ASSERT(0 == X1.numRecords());
            ASSERT(0 == X1.numEnumerations());

            EnumDef *eAnon = x1.createEnumeration();
            ASSERT(0     == X1.numRecords());
            ASSERT(1     == X1.numEnumerations());
            ASSERT(eAnon == &x1.enumeration(0));
            ASSERT(0     == enumName(*eAnon));
            ASSERT(0      > X1.enumerationIndex("A"));

            EnumDef *eA = x1.createEnumeration("A");
            ASSERT(0     == X1.numRecords());
            ASSERT(2     == X1.numEnumerations());
            ASSERT(1     == X1.enumerationIndex("A"));
            ASSERT(eA    == &X1.enumeration(1));
            ASSERT(eAnon == &X1.enumeration(0));
            ASSERT(0     == strCmp(enumName(*eA), "A"));

            x1.removeAll();
            ASSERT(0 == X1.numRecords());
            ASSERT(0 == X1.numEnumerations());
            ASSERT(0  > X1.enumerationIndex("A"));

            // Can create more the one unnamed enums
            EnumDef *eAnon1 = x1.createEnumeration();
            ASSERT(eAnon1);

          } END_BSLMA_EXCEPTION_TEST
        }

        {
          BEGIN_BSLMA_EXCEPTION_TEST {
            Obj x1(Z);  const Obj& X1 = x1;
            ASSERT(0 == x1.numRecords());
            ASSERT(0 == X1.numEnumerations());

            EnumDef *eA = x1.createEnumeration("A");
            ASSERT(0  == X1.numRecords());
            ASSERT(1  == X1.numEnumerations());
            ASSERT(0  == X1.enumerationIndex("A"));
            ASSERT(eA == &X1.enumeration(0));
            ASSERT(0  == bsl::strcmp(enumName(*eA), "A"));

            for (int i = 0; i < 10; ++i) {
                EnumDef *eAnon = x1.createEnumeration();
                LOOP_ASSERT(i, 0     == X1.numRecords());
                LOOP_ASSERT(i, i + 2 == X1.numEnumerations());
                LOOP_ASSERT(i, 0     == X1.enumerationIndex("A"));
                LOOP_ASSERT(i, eA    == &X1.enumeration(0));
                LOOP_ASSERT(i, eAnon == &X1.enumeration(i + 1));
                LOOP_ASSERT(i, 0     == enumName(*eAnon));
            }

            EnumDef *eB = x1.createEnumeration("B");
            ASSERT(0  == X1.numRecords());
            ASSERT(12 == X1.numEnumerations());
            ASSERT(11 == X1.enumerationIndex("B"));
            ASSERT(0  == X1.enumerationIndex("A"));
            ASSERT(eB == &X1.enumeration(11));
            ASSERT(eA == &X1.enumeration(0));
            ASSERT(0  == bsl::strcmp(enumName(*eB), "B"));

            if (veryVerbose) PS(X1);

            EnumDef *eBEE = x1.createEnumeration("B");
            ASSERT(0 == eBEE);

            x1.removeAll();
            ASSERT(0 == X1.numRecords());
            ASSERT(0 == X1.numEnumerations());
            ASSERT(0  > X1.recordIndex("A"));
            ASSERT(0  > X1.recordIndex("B"));
            ASSERT(0  > X1.recordIndex(""));
          } END_BSLMA_EXCEPTION_TEST
        }

        if (verbose)
            bsl::cout << "\nTesting addEnumeration and "
                      << "assignAlphabeticalIDs." << bsl::endl;
        {
          BEGIN_BSLMA_EXCEPTION_TEST {
            Obj x1(Z);  const Obj& X1 = x1;
            // On the first loop, we operate on the new object.  On the
            // second loop, we apply the same operations after a removeAll,
            // which should give the same programmatic results.
            for (int loop = 0; loop < 2; ++loop) {
                LOOP_ASSERT(loop, 0 == X1.numRecords());
                LOOP_ASSERT(loop, 0 == X1.numEnumerations());

                EnumDef *eA = x1.createEnumeration("A");
                LOOP_ASSERT(loop, 0  == X1.numRecords());
                LOOP_ASSERT(loop, 1  == X1.numEnumerations());
                LOOP_ASSERT(loop, 0  >  X1.recordIndex("A"));
                LOOP_ASSERT(loop, 0  == X1.enumerationIndex("A"));
                LOOP_ASSERT(loop, eA == &X1.enumeration(0));
                LOOP_ASSERT(loop, eA == X1.lookupEnumeration("A"));
                LOOP_ASSERT(loop, 0  == bsl::strcmp(enumName(*eA), "A"));

                // Try to create an enumerator with null name; it should fail.
                int returnedId = eA->addEnumerator(0);
                LOOP_ASSERT(loop, 0 > returnedId);

                // Add enumerator's a - d, getName of 'e' passes back an empty
                // string.
                for (int cc = 'a'; cc < 'e'; ++cc) {
                    const char *enumName = getName((char)cc);
                    int enumId = cc - 'a';

                    returnedId = eA->addEnumerator(enumName);
                    LOOP2_ASSERT(loop, enumName, 0 == X1.numRecords());
                    LOOP2_ASSERT(loop, enumName, 1 == X1.numEnumerations());
                    LOOP2_ASSERT(loop, enumName, returnedId == enumId);
                    LOOP2_ASSERT(loop, enumName,
                                 !bsl::strcmp(enumName,
                                              eA->lookupName(enumId)));
                    LOOP2_ASSERT(loop, enumId,
                                 enumId == eA->lookupId(enumName));
                }

                // Try to create enumerators with like names.  All of these
                // should fail.
                for (int cc = 'a'; cc < 'e'; ++cc) {
                    const char *enumName = getName((char)cc);
                    int enumId = cc - 'a';

                    returnedId = eA->addEnumerator(enumName);
                    LOOP2_ASSERT(loop, enumName, 0 == X1.numRecords());
                    LOOP2_ASSERT(loop, enumName, 1 == X1.numEnumerations());
                    LOOP2_ASSERT(loop, enumName, 0 > returnedId);
                }

                // Try to create enumerators with like names and different
                // ids.  All of these should fail.
                for (int cc = 'a'; cc < 'e'; ++cc) {
                    const char *enumName = getName((char)cc);
                    int enumId = eA->maxId();

                    returnedId = eA->addEnumerator(enumName, enumId  + 1);
                    LOOP2_ASSERT(loop, enumName, 0 == X1.numRecords());
                    LOOP2_ASSERT(loop, enumName, 1 == X1.numEnumerations());
                    LOOP2_ASSERT(loop, enumName, 0 > returnedId);
                }

                // Try to create enumerators with different names and
                // same ids.  All of these should fail.
                for (int cc = 'f'; cc < 'j'; ++cc) {
                    const char *enumName = getName((char)cc);
                    int enumId = cc - 'f';

                    returnedId = eA->addEnumerator(enumName, enumId);
                    LOOP2_ASSERT(loop, enumName, 0 == X1.numRecords());
                    LOOP2_ASSERT(loop, enumName, 1 == X1.numEnumerations());
                    LOOP2_ASSERT(loop, enumName, 0 > returnedId);
                }

                // Try to create enumerators with different names and
                // ids.  All should succeed.
                for (int cc = 'f'; cc < 'j'; ++cc) {
                    const char *enumName = getName((char)cc);
                    int enumId = eA->maxId() + 1;

                    returnedId = eA->addEnumerator(enumName, enumId);
                    LOOP2_ASSERT(loop, enumName, 0 == X1.numRecords());
                    LOOP2_ASSERT(loop, enumName, 1 == X1.numEnumerations());
                    LOOP2_ASSERT(loop, enumName, returnedId == enumId);
                    LOOP2_ASSERT(loop, enumName,
                                 !bsl::strcmp(enumName,
                                              eA->lookupName(enumId)));
                    LOOP2_ASSERT(loop, enumId,
                                 enumId == eA->lookupId(enumName));
                }

                // Try to append a enumerator, rA.  Should
                // succeed.
                int maxId = eA->maxId();

                {
                    returnedId = eA->addEnumerator("Late Addition");
                    LOOP_ASSERT(loop, 0 == X1.numRecords());
                    LOOP_ASSERT(loop, 1 == X1.numEnumerations());
                    LOOP_ASSERT(loop, returnedId == maxId + 1);
                    LOOP_ASSERT(loop, !bsl::strcmp("Late Addition",
                                                   eA->lookupName(maxId + 1)));
                    LOOP_ASSERT(loop,
                                maxId + 1 == eA->lookupId("Late Addition"));
                }

                // Add enumerator at boundaries.
                {
                  EnumDef *ePtr = x1.createEnumeration();
                  ASSERT(ePtr);

                  // Try to create an enumerator with an empty name; it should
                  // succeed.
                  returnedId = ePtr->addEnumerator("");
                  LOOP_ASSERT(loop, 0 == returnedId);

                  // The function-level documentation of addEnumerator states:
                  // The function will fail (return
                  // 'bdetu_Unset<int>::unsetValue()') unless 'name' is unique
                  // among the enumerators in this enumeration definition,
                  // 'id != bdetu_Unset<int>::unsetValue()', and 'id' is
                  // distinct from any integer ID currently used by an
                  // enumerator in this enumeration.  Here we use 'INT_MIN'
                  // instead of 'bdetu_Unset<int>::unsetValue()' so we must
                  // assert 'bdetu_Unset<int>::unsetValue()' == 'INT_MIN'

                  ASSERT(bdetu_Unset<int>::unsetValue() == INT_MIN);
                  int unsetInt = INT_MAX;
                  ++unsetInt;
                  ASSERT(bdetu_Unset<int>::unsetValue() == unsetInt);

                  returnedId = ePtr->addEnumerator("A", INT_MIN);
                  ASSERT(INT_MIN == returnedId);
                }

                {
                  EnumDef *ePtr = x1.createEnumeration();
                  LOOP_ASSERT(loop, ePtr);

                  const struct {
                    int         d_line;
                    const char *d_name;
                    int         d_enumStartId;
                  } DATA[] = {
                  // Line      Name      StartIndex
                    { L_,       "a",     INT_MIN + 1  },
                    { L_,       "b",     INT_MIN / 2 },
                    { L_,       "c",         -3      },
                    { L_,       "d",         -2      },
                    { L_,       "e",         -1      },
                    { L_,       "f",          0      },
                    { L_,       "g",          1      },
                    { L_,       "h",          2      },
                    { L_,       "i",          3      },
                    { L_,       "j",     INT_MAX / 2 },
                    { L_,       "k",       INT_MAX   }
                  };

                  const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

                  for (int ii = 0; ii < NUM_DATA; ++ii) {
                    const int         LINE = DATA[ii].d_line;
                    const char *const NAME = DATA[ii].d_name;
                    const int         ID   = DATA[ii].d_enumStartId;

                    // Added enumerators as described in test vector.
                    returnedId = ePtr->addEnumerator(NAME, ID);
                    LOOP2_ASSERT(loop, LINE, returnedId == ID);
                    LOOP2_ASSERT(loop, LINE,
                                 ePtr->lookupId(NAME) == returnedId);
                    LOOP2_ASSERT(loop, LINE, ePtr->lookupId(NAME) == ID);
                  }

                  // Test that adding an enumerator when the the ID of the
                  //previous most-recently-added enumerator was INT_MAX fails.
                  ASSERT(bdetu_Unset<int>::unsetValue() ==
                      ePtr->addEnumerator("should fail"));
                }

                // Testing assignAlphabeticalIDs
                Obj x2(Z);  const Obj& X2 = x2;

                const struct {
                  int   d_line;
                  int   d_enumStartId;
                  char  d_direction;
                } DATA[] = {
                // Line    StartIndex   Direction (inc/dec)
                  { L_,        0,          '+' },
                  { L_,        0,          '-' },
                  { L_,        25,         '-' },
                  { L_,       -25,         '+' },
                  { L_,        1,          '+' },
                  { L_,       -1,          '-' },
                  { L_,        26,         '-' },
                  { L_,       -26,         '+' },
                  { L_,        13,         '-' },
                  { L_,       -12,         '+' }
                };

                const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

                for (int ii = 0; ii < NUM_DATA; ++ii) {
                  const int  LINE      = DATA[ii].d_line;
                  const int  ID        = DATA[ii].d_enumStartId;
                  const char DIRECTION = DATA[ii].d_direction;

                  EnumDef *ePtr = x2.createEnumeration();
                  LOOP2_ASSERT(loop, LINE, 0  == X2.numRecords());
                  LOOP2_ASSERT(loop, LINE, ii + 1 == X2.numEnumerations());
                  LOOP2_ASSERT(loop, LINE, ePtr == &X2.enumeration(ii));

                  // Added enumerators as described in test vector.
                  int id = ID;
                  for (int cc = 'a'; cc <= 'z'; ++cc) {
                    char s[2]; s[0] = (char)cc; s[1] = '\0';
                    ePtr->addEnumerator(s, id);
                    id = (DIRECTION == '+') ? (id + 1) : (id - 1);
                  }

                  // Re-assign ids.
                  ePtr->alphabetize();

                  // Test to ensure enumeration starts at 0 and increments
                  // correctly.
                  int expectedId = 0;
                  for (int cc = 'a'; cc <= 'z'; ++cc) {
                    char s[2]; s[0] = (char)cc; s[1] = '\0';
                    id = ePtr->lookupId(s);
                    LOOP2_ASSERT(loop, LINE, id == expectedId);
                    ++expectedId;
                  }
                }

                x1.removeAll();
                LOOP_ASSERT(loop, 0 == X1.numRecords());
                LOOP_ASSERT(loop, 0 == X1.numEnumerations());
                LOOP_ASSERT(loop, 0  > X1.enumerationIndex("A"));

                x2.removeAll();
                LOOP_ASSERT(loop, 0 == X2.numRecords());
                LOOP_ASSERT(loop, 0 == X2.numEnumerations());
                LOOP_ASSERT(loop, 0  > X2.enumerationIndex("A"));

            }
          } END_BSLMA_EXCEPTION_TEST
        }
        break;
        if (verbose)
            bsl::cout << "\nTesting appendField and "
                      << "bdem_FieldDef constructor." << bsl::endl;
        {
          BEGIN_BSLMA_EXCEPTION_TEST {
            Obj x1(Z);  const Obj& X1 = x1;
            // On the first loop, we operate on the new object.  On the
            // second loop, we apply the same operations after a removeAll,
            // which should give the same programmatic results.
            for (int loop = 0; loop < 2; ++loop) {
                LOOP_ASSERT(loop, 0 == X1.numRecords());

                RecDef *rA = x1.createRecord("A");
                LOOP_ASSERT(loop, 1  == X1.numRecords());
                LOOP_ASSERT(loop, 0  == X1.recordIndex("A"));
                LOOP_ASSERT(loop, rA == &X1.record(0));
                LOOP_ASSERT(loop, rA == X1.lookupRecord("A"));
                LOOP_ASSERT(loop, 0  == bsl::strcmp(recName(*rA), "A"));

                // Note: Using only unconstrained field types in this test.

                // Create named fields of every possible unconstrained type.
                {
                    for (int i = 0; i < NUM_TYPES; ++i) {
                        const FldDef *fld = rA->appendField(ggElemTypes[i],
                                                           ggElemTypeNames[i]);
                        LOOP2_ASSERT(loop, i, 0 != fld);
                        LOOP2_ASSERT(loop,i,0 == bsl::strcmp(rA->fieldName(i),
                                                          ggElemTypeNames[i]));
                        LOOP2_ASSERT(loop, i, fld == &rA->field(i));
                        LOOP2_ASSERT(loop, i, fld ==
                                          rA->lookupField(ggElemTypeNames[i]));
                        LOOP2_ASSERT(loop, i, ggElemTypes[i] ==
                                                      rA->field(i).elemType());
                        LOOP2_ASSERT(loop, i, i + 1 == rA->numFields());
                    }
                }

                int savedLength = rA->numFields();

                // Try to create fields with like names.  All of these should
                // fail.
                {
                    for (int i = 0; i < NUM_TYPES; ++i) {
                        const FldDef *fld = rA->appendField(ggElemTypes[i],
                                                           ggElemTypeNames[i]);
                        LOOP2_ASSERT(loop, i, 0           == fld);
                        LOOP2_ASSERT(loop, i, savedLength == rA->numFields());
                    }
                }

                // Try to create anonymous fields.  All should succeed.
                {
                    for (int i = 0; i < NUM_TYPES; ++i) {
                        const FldDef *fld = rA->appendField(ggElemTypes[i]);
                        int fieldIndex = savedLength + i;
                        LOOP2_ASSERT(loop, i, 0 != fld);
                        LOOP2_ASSERT(loop, i, fld == &rA->field(fieldIndex));
                        LOOP2_ASSERT(loop, i, ggElemTypes[i]
                                         == rA->field(fieldIndex).elemType());
                        LOOP2_ASSERT(loop, i,
                                           fieldIndex + 1 == rA->numFields());
                    }
                }

                savedLength = rA->numFields();

                // Try to create the same sequence of anonymous fields.
                // All should succeed.
                {
                    for (int i = 0; i < NUM_TYPES; ++i) {
                        const FldDef *fld = rA->appendField(ggElemTypes[i]);
                        int fieldIndex = savedLength + i;
                        LOOP2_ASSERT(loop, i, 0 != fld);
                        LOOP2_ASSERT(loop, i, fld == &rA->field(fieldIndex));
                        LOOP2_ASSERT(loop, i, ggElemTypes[i]
                                         == rA->field(fieldIndex).elemType());
                        LOOP2_ASSERT(loop, i,
                                           fieldIndex + 1 == rA->numFields());
                    }
                }

                savedLength = rA->numFields();

                // Try to create a named field.  Should succeed.
                {
                    const FldDef *fld = rA->appendField(ggElemTypes[0],"");
                    int fieldIndex = savedLength;
                    LOOP_ASSERT(loop, 0   != fld);
                    LOOP_ASSERT(loop, 0   == strCmp(rA->fieldName(fieldIndex),
                                                    ""));
                    LOOP_ASSERT(loop, fld == &rA->field(fieldIndex));
                    LOOP_ASSERT(loop, fld == rA->lookupField(""));
                    LOOP_ASSERT(loop, ggElemTypes[0]
                                          == rA->field(fieldIndex).elemType());
                    LOOP_ASSERT(loop, savedLength + 1 == rA->numFields());
                }

                savedLength = rA->numFields();

                // Try again to create a named field.  Should fail.
                {
                    const FldDef *fld = rA->appendField(ggElemTypes[0],"");
                    LOOP_ASSERT(loop, 0           == fld);
                    LOOP_ASSERT(loop, savedLength == rA->numFields());
                }

                // Create an anonymous record.
                RecDef *rAnon = x1.createRecord();
                LOOP_ASSERT(loop, 0 != rAnon);
                LOOP_ASSERT(loop, 2 == X1.numRecords());
                LOOP_ASSERT(loop, 0 == X1.numEnumerations());
                LOOP_ASSERT(loop, 0 == recName(*rAnon));
                LOOP_ASSERT(loop, 0 == rAnon->numFields());

                // Try to append a field to the first record, rA.  Should
                // succeed.
                savedLength = rA->numFields();

                {
                    const FldDef *added = rA->appendField(ggElemTypes[0],
                                                          "Late Addition");
                    LOOP_ASSERT(loop, 0 != added);
                    LOOP_ASSERT(loop, savedLength + 1 == rA->numFields());
                }

                // Create a named enumeration.
                EnumDef *eA = x1.createEnumeration("a");
                LOOP_ASSERT(loop, 0 != eA);
                LOOP_ASSERT(loop, 2 == X1.numRecords());
                LOOP_ASSERT(loop, 1 == X1.numEnumerations());

                // Try to append a field to the first record, rA, constrained
                // on the second record, rAnon.  The original version of
                // 'bdem_schema' rejected such forward-use of records.
                // However, this restriction has been relaxed.
                savedLength = rA->numFields();

                {
                    const FldDef *notAdded = rA->appendField(EType::BDEM_LIST,
                                                             rAnon);
                    LOOP_ASSERT(loop, notAdded);
                    LOOP_ASSERT(loop, savedLength + 1 == rA->numFields());

                    notAdded = rA->appendField(EType::BDEM_TABLE, rAnon);
                    LOOP_ASSERT(loop, notAdded);
                    LOOP_ASSERT(loop, savedLength + 2 == rA->numFields());
                }

                // Append a field to the first record, rA, constrained
                // on the named enumeration, eA.
                savedLength = rA->numFields();

                {
                  for (int ii = 0; ii < NUM_TYPES; ++ii) {
                    EType::Type type = ggElemTypes[ii];
                    const FldDef *fld = rA->appendField(type, eA,
                                                        ggElemTypeNames[ii]);
                    if (EType::BDEM_INT          == type
                     || EType::BDEM_INT_ARRAY    == type
                     || EType::BDEM_STRING       == type
                     || EType::BDEM_STRING_ARRAY == type) {
                        LOOP2_ASSERT(loop, ii, 0 != fld);
                    } else {
                      LOOP2_ASSERT(loop, ii, 0 == fld);
                    }
                  }
                  savedLength = rA->numFields();
                }

                // Append a named constrained list.
                const char *listFieldName = "Constrained List";
                const FldDef *listFld = rAnon->appendField(EType::BDEM_LIST,
                                                           rA, listFieldName);
                LOOP_ASSERT(loop, 0       != listFld);
                LOOP_ASSERT(loop, 1       == rAnon->numFields());
                LOOP_ASSERT(loop, 0       == bsl::strcmp(rAnon->fieldName(0),
                                                         listFieldName));
                LOOP_ASSERT(loop, listFld == &rAnon->field(0));
                LOOP_ASSERT(loop, listFld ==
                                            rAnon->lookupField(listFieldName));
                LOOP_ASSERT(loop,
                            EType::BDEM_LIST == rAnon->field(0).elemType());

                // Append a named constrained table.
                const char *tableFieldName = "Constrained Table";
                const FldDef *tableFld = rAnon->appendField(EType::BDEM_TABLE,
                                                            rA,
                                                            tableFieldName);
                LOOP_ASSERT(loop, 0        != tableFld);
                LOOP_ASSERT(loop, 2        == rAnon->numFields());
                LOOP_ASSERT(loop, 0        == bsl::strcmp(rAnon->fieldName(1),
                                                          tableFieldName));
                LOOP_ASSERT(loop, tableFld == &rAnon->field(1));
                LOOP_ASSERT(loop, tableFld ==
                                           rAnon->lookupField(tableFieldName));
                LOOP_ASSERT(loop,
                            EType::BDEM_TABLE == rAnon->field(1).elemType());

                // Re-append a constrained list with same name (should fail).
                const FldDef *listFld2 = rAnon->appendField(EType::BDEM_LIST,
                                                            rA,
                                                            listFieldName);
                LOOP_ASSERT(loop, 0 == listFld2);
                LOOP_ASSERT(loop, 2 == rAnon->numFields());

                // Re-append a constrained table with same name (should fail).
                const FldDef *tableFld2 = rAnon->appendField(EType::BDEM_TABLE,
                                                             rA,
                                                             tableFieldName);
                LOOP_ASSERT(loop, 0 == tableFld2);
                LOOP_ASSERT(loop, 2 == rAnon->numFields());

                // Append an anonymous constrained list.
                const FldDef *listFld3 = rAnon->appendField(EType::BDEM_LIST,
                                                            rA);
                LOOP_ASSERT(loop, 0           != listFld3);
                LOOP_ASSERT(loop, 3           == rAnon->numFields());
                LOOP_ASSERT(loop, 0           == rAnon->fieldName(2));
                LOOP_ASSERT(loop, listFld3    == &rAnon->field(2));
                LOOP_ASSERT(loop,
                            EType::BDEM_LIST == rAnon->field(2).elemType());

                // Append an anonymous constrained table.
                const FldDef *tableFld3 =
                                     rAnon->appendField(EType::BDEM_TABLE, rA);
                LOOP_ASSERT(loop, 0            != tableFld3);
                LOOP_ASSERT(loop, 4            == rAnon->numFields());
                LOOP_ASSERT(loop, 0            == rAnon->fieldName(3));
                LOOP_ASSERT(loop, tableFld3    == &rAnon->field(3));
                LOOP_ASSERT(loop,
                            EType::BDEM_TABLE == rAnon->field(3).elemType());

                {
                    Obj x2(Z);

                    const RecDef *x2Rec = x2.createRecord();

                    LOOP_ASSERT(loop, 0 != x2Rec);

                    // Append an anonymous constrained table, constrained on
                    // a record in a different schema.  Should fail.
                    const FldDef *failedAddField1 =
                                          rAnon->appendField(EType::BDEM_TABLE,
                                                             x2Rec);

                    LOOP_ASSERT(loop, 0 == failedAddField1);

                    // Append an anonymous constrained list, constrained on
                    // a record in a different schema.  Should fail.
                    const FldDef *failedAddField2 =
                                           rAnon->appendField(EType::BDEM_LIST,
                                                              x2Rec);

                    LOOP_ASSERT(loop, 0 == failedAddField2);
                }

                // Create more named fields, re-using names used in another
                // record.  Should work.

                savedLength = rAnon->numFields();

                int i;  // loop index for MSVC++
                for (i = 0; i < NUM_TYPES; ++i) {
                    const FldDef *fld = rAnon->appendField(ggElemTypes[i],
                                                           ggElemTypeNames[i]);

                    int fieldIndex = savedLength + i;

                    LOOP2_ASSERT(loop, i, 0 != fld);
                    LOOP2_ASSERT(loop, i, 0 ==
                                  bsl::strcmp(rAnon->fieldName(fieldIndex),
                                                          ggElemTypeNames[i]));
                    LOOP2_ASSERT(loop, i, fld == &rAnon->field(fieldIndex));
                    LOOP2_ASSERT(loop, i, fld ==
                                       rAnon->lookupField(ggElemTypeNames[i]));
                    LOOP2_ASSERT(loop, i, ggElemTypes[i] ==
                                        rAnon->field(fieldIndex).elemType());
                    LOOP2_ASSERT(loop, i,
                                       fieldIndex + 1 == rAnon->numFields());
                }

                savedLength = rAnon->numFields();

                // Try to create fields with same names.  All of these should
                // fail.
                for (i = 0; i < NUM_TYPES; ++i) {
                    const FldDef *fld = rAnon->appendField(ggElemTypes[i],
                                                           ggElemTypeNames[i]);
                    LOOP2_ASSERT(loop, i, 0           == fld);
                    LOOP2_ASSERT(loop, i, savedLength == rAnon->numFields());
                }

                // Try again to append a field to the first record, rA.
                // This should fail, since name "Late Addition" is already
                // in the record.
                savedLength = rA->numFields();

                {
                    const FldDef *notAdded =rA->appendField(ggElemTypes[0],
                                                            "Late Addition");
                    LOOP_ASSERT(loop, 0           == notAdded);
                    LOOP_ASSERT(loop, savedLength == rA->numFields());
                }

                if (0 == loop && veryVerbose)  PS(X1);

                x1.removeAll();
                LOOP_ASSERT(loop, 0 == X1.numRecords());
                LOOP_ASSERT(loop, 0  > X1.recordIndex("A"));
            }
          } END_BSLMA_EXCEPTION_TEST
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   We are concerned that the basic (value-semantic) functionality of
        //   'bdem_schema' works properly.  We are also concerned that the
        //   somewhat customized proctor objects implemented in the '.cpp' of
        //   this component do their job, and so we perform 'bdema' exception
        //   testing in this Breathing Test.
        //
        // Plan:
        //   We exercise basic functionality in 12 steps, inspired by the
        //   'bsl::vector<double>' test driver's 11-step breathing test, to
        //   which we add a 12th step to exercise basic streaming
        //   functionality.
        //
        //   The test cases for 'bdem_Schema' are slightly more complex
        //   than those for 'bsl::vector<double>', since we need to exercise
        //   'bdem_RecordDef' and 'bdem_FieldDef' as well, and neither of
        //   these types can be tested outside of 'bdem_Schema'.
        //
        //   We will use 4 objects for our tests, x1, x2, x3, and x4.
        //
        //   After each step, we will confirm that:
        //      a) the object we just modified is in the expected state (by
        //         checking 'length' and other accessors).
        //      b) the object is equal/not equal to the other existing objects,
        //         and equal to itself.
        //
        //   The exercises:
        //      1) Build x1 using default constructor
        //          a) 0  == x1.numRecords()
        //             0  == x1.numEnumerations()
        //          b) x1 == x1
        //      2) Build x2 using copy c'tor from x1
        //          a) 0  == x2.numRecords()
        //             0  == x2.numEnumerations()
        //          b) x2 == x1
        //      3) Append a record with some fields to x1, and then a
        //         second record containing constrained fields and
        //         enumerations
        //          a) 2  == x1.numRecords()
        //             1  == x1.numEnumerations()
        //          b) x1 != x2
        //      4) Append same records to x2
        //          a) 2  == x2.numRecords()
        //             1  == x2.numEnumerations()
        //          b) x1 == x2
        //      5) x1.removeAll()
        //          a) 0  == x1.numRecords()
        //             0  == x1.numEnumerations()
        //          b) x1 != x2
        //      6) Re-append same records to x1
        //          a) 2  == x1.numRecords()
        //             1  == x1.numEnumerations()
        //          b) x1 == x2
        //      7) Empty x1, and then create empty x3 using default c'tor
        //          a) 0  == x3.numRecords()
        //             0  == x3.numEnumerations()
        //          b) x3 == x1
        //             x3 != x2
        //      8) Create bdem_Schema x4(x2)
        //          a) 2  == x4.numRecords()
        //             1  == x4.numEnumerations()
        //          b) x4 == x2
        //             x4 != x1
        //             x4 != x3
        //      9) x1=x2, trying assignment of longer schema to shorter
        //          a) 2  == x1.numRecords()
        //             1  == x1.numEnumerations()
        //          b) x1 == x2
        //             x1 == x4
        //             x1 != x3
        //     10) x4=x3, trying assignment of shorter schema to longer
        //          a) 0  == x4.numRecords()
        //             0  == x4.numEnumerations()
        //          b) x4 == x3
        //             x4 != x1
        //             x4 != x2
        //     11) x2=x2, trying self-assignment
        //          a) 2  == x2.numRecords()
        //             1  == x2.numEnumerations()
        //          b) x2 == x2
        //             x2 == x1
        //             x2 != x3
        //             x2 != x4
        //     12) stream x2 out, and then stream same data back in to x4
        //          a) 2  == x4.numRecords()
        //             1  == x4.numEnumerations()
        //          b) x4 == x2
        //             x4 == x1
        //             x4 != x3
        //             x4 == x4
        // Testing:
        //   This breathing test exercises basic functionality as a preliminary
        //   to the thorough testing done in subsequent cases.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBreathing Test:"
                               << "\n===============" << bsl::endl;

        BEGIN_BSLMA_EXCEPTION_TEST {
          if (verbose) bsl::cout << "\n 1. Create an object x1 (default ctor)."
                                    "\t\t{ x1: }" << bsl::endl;
          Obj x1(Z);  const Obj& X1 = x1;
          ASSERT(0  == X1.numRecords());
          ASSERT(0  == X1.numEnumerations());
          ASSERT(X1 == X1);      ASSERT(!(X1 != X1));

          if (veryVerbose) PS(X1);

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
          if (verbose)
              bsl::cout << "\n 2. Create a second object x2 (copy from x1)."
                           "\t{ x1:  x2: }" << bsl::endl;
          Obj x2(X1, Z);  const Obj& X2 = x2;
          ASSERT(0  == X2.numRecords());
          ASSERT(X1 == X1);      ASSERT(!(X1 != X1));
          ASSERT(X1 == X2);      ASSERT(!(X1 != X2));

          if (veryVerbose) PS(X2);

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
          if (verbose) bsl::cout << "\n 3. Append records to x1."
                                    "\t\t\t{ x1:V x2: }" << bsl::endl;
          RecDef *re = x1.createRecord("EMPLOYEE");
          ASSERT(re);
          ASSERT(0 == re->recordIndex());
          ASSERT(0 == bsl::strcmp(X1.recordName(0), "EMPLOYEE"));
          ASSERT(1 == X1.numRecords());

          const FldDef *fld = re->appendField(EType::BDEM_STRING, "Name");
          ASSERT(fld);
          ASSERT(0             == bsl::strcmp(re->fieldName(0), "Name"));
          ASSERT(0             == re->fieldIndex("Name"));
          ASSERT(EType::BDEM_STRING == fld->elemType());
          ASSERT(0             == fld->recordConstraint());
          ASSERT(1             == re->numFields());

          fld = re->appendField(EType::BDEM_STRING, "Group");
          ASSERT(fld);
          ASSERT(0             == bsl::strcmp(re->fieldName(1), "Group"));
          ASSERT(1             == re->fieldIndex("Group"));
          ASSERT(EType::BDEM_STRING == fld->elemType());
          ASSERT(0             == fld->recordConstraint());
          ASSERT(2             == re->numFields());

          fld = re->appendField(EType::BDEM_DATE, "Hire Date");
          ASSERT(fld);
          ASSERT(0           == bsl::strcmp(re->fieldName(2), "Hire Date"));
          ASSERT(2           == re->fieldIndex("Hire Date"));
          ASSERT(EType::BDEM_DATE == fld->elemType());
          ASSERT(0           == fld->recordConstraint());
          ASSERT(3           == re->numFields());

          bdem_EnumerationDef *colors = x1.createEnumeration("COLORS");
          ASSERT(&X1 == &colors->schema());
          ASSERT(0 == colors->enumerationIndex());
          ASSERT(1 == X1.numEnumerations());
          ASSERT(0 == colors->numEnumerators());

          const char *greenConst = "green";
          int greenId = colors->addEnumerator(greenConst);
          ASSERT(1 == colors->numEnumerators());
          ASSERT(0 == greenId);
          ASSERT(greenId == colors->maxId());
          ASSERT(greenId == colors->lookupId(greenConst));
          ASSERT(greenConst != colors->lookupName(greenId));
          ASSERT(0 == bsl::strcmp(greenConst, colors->lookupName(greenId)));

          int blueId = colors->addEnumerator("blue");
          ASSERT(2 == colors->numEnumerators());
          ASSERT(1 == blueId);
          ASSERT(blueId  == colors->maxId());
          ASSERT(greenId == colors->lookupId("green"));
          ASSERT(0 == bsl::strcmp("green", colors->lookupName(greenId)));
          ASSERT(blueId == colors->lookupId("blue"));
          ASSERT(0 == bsl::strcmp("blue", colors->lookupName(blueId)));

          int redId = colors->addEnumerator("red", 5);
          ASSERT(3 == colors->numEnumerators());
          ASSERT(5 == redId);
          ASSERT(redId   == colors->maxId());
          ASSERT(greenId == colors->lookupId("green"));
          ASSERT(0 == bsl::strcmp("green", colors->lookupName(greenId)));
          ASSERT(0 == bsl::strcmp("green", colors->lookupName("green")));
          ASSERT(blueId == colors->lookupId("blue"));
          ASSERT(0 == bsl::strcmp("blue", colors->lookupName(blueId)));
          ASSERT(0 == bsl::strcmp("blue", colors->lookupName("blue")));
          ASSERT(redId == colors->lookupId("red"));
          ASSERT(0 == bsl::strcmp("red", colors->lookupName(redId)));
          ASSERT(0 == bsl::strcmp("red", colors->lookupName("red")));

          ASSERT(0 == colors->lookupName(7));
          ASSERT(0 == colors->lookupName("yellow"));
          ASSERT(bdetu_Unset<int>::unsetValue() == colors->lookupId("yellow"));

          RecDef *rt = x1.createRecord("TEAM");
          ASSERT(rt);
          ASSERT(0 == re->recordIndex());
          ASSERT(1 == rt->recordIndex());
          ASSERT(0 == bsl::strcmp(X1.recordName(1), "TEAM"));
          ASSERT(2 == X1.numRecords());

          fld = rt->appendField(EType::BDEM_LIST, re, "Sue");
          ASSERT(fld);
          ASSERT(0           == bsl::strcmp(rt->fieldName(0), "Sue"));
          ASSERT(0           == rt->fieldIndex("Sue"));
          ASSERT(EType::BDEM_LIST == fld->elemType());
          ASSERT(re          == fld->recordConstraint());
          ASSERT(0           == fld->enumerationConstraint());
          ASSERT(1           == rt->numFields());

          fld = rt->appendField(EType::BDEM_LIST, re, "Ted");
          ASSERT(fld);
          ASSERT(0           == bsl::strcmp(rt->fieldName(1), "Ted"));
          ASSERT(1           == rt->fieldIndex("Ted"));
          ASSERT(EType::BDEM_LIST == fld->elemType());
          ASSERT(re          == fld->recordConstraint());
          ASSERT(0           == fld->enumerationConstraint());
          ASSERT(2           == rt->numFields());

          fld = rt->appendField(EType::BDEM_DATE);
          ASSERT(fld);
          ASSERT(0           == rt->fieldName(2));
          ASSERT(EType::BDEM_DATE == fld->elemType());
          ASSERT(0           == fld->recordConstraint());
          ASSERT(0           == fld->enumerationConstraint());
          ASSERT(3           == rt->numFields());

          fld = rt->appendField(EType::BDEM_DATE, "");
          ASSERT(fld);
          ASSERT(0           == bsl::strcmp(rt->fieldName(3), ""));
          ASSERT(3           == rt->fieldIndex(""));
          ASSERT(EType::BDEM_DATE == fld->elemType());
          ASSERT(0           == fld->recordConstraint());
          ASSERT(0           == fld->enumerationConstraint());
          ASSERT(4           == rt->numFields());

          fld = rt->appendField(EType::BDEM_INT, colors, "TeamColor");
          ASSERT(fld);
          ASSERT(0           == bsl::strcmp(rt->fieldName(4), "TeamColor"));
          ASSERT(EType::BDEM_INT  == fld->elemType());
          ASSERT(0           == fld->recordConstraint());
          ASSERT(colors      == fld->enumerationConstraint());
          ASSERT(5           == rt->numFields());

          fld = rt->appendField(EType::BDEM_STRING, colors);
          ASSERT(fld);
          ASSERT(0           == rt->fieldName(5));
          ASSERT(EType::BDEM_STRING == fld->elemType());
          ASSERT(0           == fld->recordConstraint());
          ASSERT(colors      == fld->enumerationConstraint());
          ASSERT(6           == rt->numFields());

          fld = rt->appendField(EType::BDEM_INT_ARRAY, colors, "FlagColors");
          ASSERT(fld);
          ASSERT(0           == bsl::strcmp(rt->fieldName(6), "FlagColors"));
          ASSERT(EType::BDEM_INT_ARRAY == fld->elemType());
          ASSERT(0           == fld->recordConstraint());
          ASSERT(colors      == fld->enumerationConstraint());
          ASSERT(7           == rt->numFields());

          fld = rt->appendField(EType::BDEM_STRING_ARRAY,
                                colors,
                                "ColorHistory");
          ASSERT(fld);
          ASSERT(0           == bsl::strcmp(rt->fieldName(7), "ColorHistory"));
          ASSERT(EType::BDEM_STRING_ARRAY == fld->elemType());
          ASSERT(0           == fld->recordConstraint());
          ASSERT(colors      == fld->enumerationConstraint());
          ASSERT(8           == rt->numFields());

          ASSERT(0  >  rt->fieldIndex("Ghost"));
          ASSERT(0  >  re->fieldIndex("Bank"));

          ASSERT(0  == X1.lookupRecord("EMPLOYEE")->recordIndex());
          ASSERT(1  == X1.lookupRecord("TEAM")->recordIndex());
          ASSERT(0  == X1.lookupRecord("COUNTRY"));
          ASSERT(0  == X1.lookupEnumeration("COLORS")->enumerationIndex());

          ASSERT(2  == X1.numRecords());
          ASSERT(1  == X1.numEnumerations());
          ASSERT(X1 == X1);      ASSERT(!(X1 != X1));
          ASSERT(!(X1 == X2));   ASSERT(X1 != X2);

          if (veryVerbose) PS(X1);

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
          if (verbose) bsl::cout << "\n 4. Append the same records to x2."
                                    "\t\t{ x1:V x2:V }" << bsl::endl;
          re = x2.createRecord("EMPLOYEE");
          fld = re->appendField(EType::BDEM_STRING, "Name");
          fld = re->appendField(EType::BDEM_STRING, "Group");
          fld = re->appendField(EType::BDEM_DATE, "Hire Date");

          colors = x2.createEnumeration("COLORS");
          colors->addEnumerator("green");
          colors->addEnumerator("blue");
          colors->addEnumerator("red", 5);
          bdem_EnumerationDef *colors2 = colors;  // Save for later

          rt = x2.createRecord("TEAM");
          fld = rt->appendField(EType::BDEM_LIST, re, "Sue");
          fld = rt->appendField(EType::BDEM_LIST, re, "Ted");
          fld = rt->appendField(EType::BDEM_DATE);
          fld = rt->appendField(EType::BDEM_DATE, "");
          fld = rt->appendField(EType::BDEM_INT, colors, "TeamColor");
          fld = rt->appendField(EType::BDEM_STRING, colors);
          fld = rt->appendField(EType::BDEM_INT_ARRAY, colors, "FlagColors");
          fld = rt->appendField(EType::BDEM_STRING_ARRAY,
                                colors,
                                "ColorHistory");

          ASSERT(0  > rt->fieldIndex("Ghost"));
          ASSERT(0  > re->fieldIndex("Bank"));

          ASSERT(0  == X2.lookupRecord("EMPLOYEE")->recordIndex());
          ASSERT(1  == X2.lookupRecord("TEAM")->recordIndex());
          ASSERT(0  == X2.lookupRecord("COUNTRY"));
          ASSERT(0  == X2.lookupEnumeration("COLORS")->enumerationIndex());

          ASSERT(2  == X2.numRecords());
          ASSERT(1  == X2.numEnumerations());
          ASSERT(X2 == X1);        ASSERT(!(X2 != X1));
          ASSERT(X2 == X2);        ASSERT(!(X2 != X2));

          if (veryVerbose) PS(X2);

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
          if (verbose) bsl::cout << "\n 5. Empty x1 using 'removeAll'."
                                    "\t\t\t{ x1:  x2:V }" << bsl::endl;
          x1.removeAll();

          ASSERT(0  == X1.numRecords());
          ASSERT(0  == X1.numEnumerations());

          ASSERT(X1 == X1);      ASSERT(!(X1 != X1));
          ASSERT(!(X1 == X2));   ASSERT(X1 != X2);

          if (veryVerbose) PS(X1);

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
          if (verbose) bsl::cout << "\n 6. Append the same records back to x1."
                                    "\t\t{ x1:V x2:V }" << bsl::endl;
          re = x1.createRecord("EMPLOYEE");
          ASSERT(re);
          ASSERT(0 == re->recordIndex());
          ASSERT(0 == bsl::strcmp(X1.recordName(0), "EMPLOYEE"));
          ASSERT(1 == x1.numRecords());

          fld = re->appendField(EType::BDEM_STRING, "Name");
          ASSERT(fld);
          ASSERT(0             == bsl::strcmp(re->fieldName(0), "Name"));
          ASSERT(0             == re->fieldIndex("Name"));
          ASSERT(EType::BDEM_STRING == fld->elemType());
          ASSERT(0             == fld->recordConstraint());
          ASSERT(1             == re->numFields());

          fld = re->appendField(EType::BDEM_STRING, "Group");
          ASSERT(fld);
          ASSERT(0             == bsl::strcmp(re->fieldName(1), "Group"));
          ASSERT(1             == re->fieldIndex("Group"));
          ASSERT(EType::BDEM_STRING == fld->elemType());
          ASSERT(0             == fld->recordConstraint());
          ASSERT(2             == re->numFields());

          fld = re->appendField(EType::BDEM_DATE, "Hire Date");
          ASSERT(fld);
          ASSERT(0           == bsl::strcmp(re->fieldName(2), "Hire Date"));
          ASSERT(2           == re->fieldIndex("Hire Date"));
          ASSERT(EType::BDEM_DATE == fld->elemType());
          ASSERT(0           == fld->recordConstraint());
          ASSERT(3           == re->numFields());

          colors = x1.createEnumeration("COLORS");
          ASSERT(&X1 == &colors->schema());
          ASSERT(0 == colors->enumerationIndex());
          ASSERT(1 == X1.numEnumerations());
          ASSERT(0 == colors->numEnumerators());

          greenId = colors->addEnumerator("green");
          ASSERT(1 == colors->numEnumerators());
          ASSERT(0 == greenId);
          ASSERT(greenId == colors->maxId());
          ASSERT(greenId == colors->lookupId("green"));
          ASSERT(0 == bsl::strcmp("green", colors->lookupName(greenId)));

          blueId = colors->addEnumerator("blue");
          ASSERT(2 == colors->numEnumerators());
          ASSERT(1 == blueId);
          ASSERT(blueId == colors->maxId());
          ASSERT(greenId == colors->lookupId("green"));
          ASSERT(0 == bsl::strcmp("green", colors->lookupName(greenId)));
          ASSERT(blueId == colors->lookupId("blue"));
          ASSERT(0 == bsl::strcmp("blue", colors->lookupName(blueId)));

          redId = colors->addEnumerator("red", 5);
          ASSERT(3 == colors->numEnumerators());
          ASSERT(5 == redId);
          ASSERT(redId == colors->maxId());
          ASSERT(greenId == colors->lookupId("green"));
          ASSERT(0 == bsl::strcmp("green", colors->lookupName(greenId)));
          ASSERT(blueId == colors->lookupId("blue"));
          ASSERT(0 == bsl::strcmp("blue", colors->lookupName(blueId)));
          ASSERT(redId == colors->lookupId("red"));
          ASSERT(0 == bsl::strcmp("red", colors->lookupName(redId)));

          ASSERT(0 == colors->lookupName(7));
          ASSERT(0 == colors->lookupName("yellow"));
          ASSERT(0 == colors->lookupName((char*) 0));
          ASSERT(bdetu_Unset<int>::unsetValue() == colors->lookupId("yellow"));
          ASSERT(bdetu_Unset<int>::unsetValue() == colors->lookupId(0));

          rt = x1.createRecord("TEAM");
          ASSERT(rt);
          ASSERT(0 == re->recordIndex());
          ASSERT(1 == rt->recordIndex());
          ASSERT(0 == bsl::strcmp(X1.recordName(1), "TEAM"));
          ASSERT(2 == X1.numRecords());

          fld = rt->appendField(EType::BDEM_LIST, re, "Sue");
          ASSERT(fld);
          ASSERT(0           == bsl::strcmp(rt->fieldName(0), "Sue"));
          ASSERT(0           == rt->fieldIndex("Sue"));
          ASSERT(EType::BDEM_LIST == fld->elemType());
          ASSERT(re          == fld->recordConstraint());
          ASSERT(0           == fld->enumerationConstraint());
          ASSERT(1           == rt->numFields());

          fld = rt->appendField(EType::BDEM_LIST, re, "Ted");
          ASSERT(fld);
          ASSERT(0           == bsl::strcmp(rt->fieldName(1), "Ted"));
          ASSERT(1           == rt->fieldIndex("Ted"));
          ASSERT(EType::BDEM_LIST == fld->elemType());
          ASSERT(re          == fld->recordConstraint());
          ASSERT(0           == fld->enumerationConstraint());
          ASSERT(2           == rt->numFields());

          fld = rt->appendField(EType::BDEM_DATE);
          ASSERT(fld);
          ASSERT(0           == rt->fieldName(2));
          ASSERT(EType::BDEM_DATE == fld->elemType());
          ASSERT(0           == fld->recordConstraint());
          ASSERT(0           == fld->enumerationConstraint());
          ASSERT(3           == rt->numFields());

          fld = rt->appendField(EType::BDEM_DATE, "");
          ASSERT(fld);
          ASSERT(0           == bsl::strcmp(rt->fieldName(3), ""));
          ASSERT(3           == rt->fieldIndex(""));
          ASSERT(EType::BDEM_DATE == fld->elemType());
          ASSERT(0           == fld->recordConstraint());
          ASSERT(0           == fld->enumerationConstraint());
          ASSERT(4           == rt->numFields());

          fld = rt->appendField(EType::BDEM_INT, colors, "TeamColor");
          ASSERT(fld);
          ASSERT(0           == bsl::strcmp(rt->fieldName(4), "TeamColor"));
          ASSERT(EType::BDEM_INT  == fld->elemType());
          ASSERT(0           == fld->recordConstraint());
          ASSERT(colors      == fld->enumerationConstraint());
          ASSERT(5           == rt->numFields());

          fld = rt->appendField(EType::BDEM_STRING, colors);
          ASSERT(fld);
          ASSERT(0           == rt->fieldName(5));
          ASSERT(EType::BDEM_STRING == fld->elemType());
          ASSERT(0           == fld->recordConstraint());
          ASSERT(colors      == fld->enumerationConstraint());
          ASSERT(6           == rt->numFields());

          fld = rt->appendField(EType::BDEM_INT_ARRAY, colors, "FlagColors");
          ASSERT(fld);
          ASSERT(0           == bsl::strcmp(rt->fieldName(6), "FlagColors"));
          ASSERT(EType::BDEM_INT_ARRAY == fld->elemType());
          ASSERT(0           == fld->recordConstraint());
          ASSERT(colors      == fld->enumerationConstraint());
          ASSERT(7           == rt->numFields());

          fld = rt->appendField(EType::BDEM_STRING_ARRAY,
                                colors,
                                "ColorHistory");
          ASSERT(fld);
          ASSERT(0           == bsl::strcmp(rt->fieldName(7), "ColorHistory"));
          ASSERT(EType::BDEM_STRING_ARRAY == fld->elemType());
          ASSERT(0           == fld->recordConstraint());
          ASSERT(colors      == fld->enumerationConstraint());
          ASSERT(8           == rt->numFields());

          ASSERT(0  >  rt->fieldIndex("Ghost"));
          ASSERT(0  >  re->fieldIndex("Bank"));

          ASSERT(2  == X1.numRecords());
          ASSERT(1  == X1.numEnumerations());

          ASSERT(0  == X1.lookupRecord("EMPLOYEE")->recordIndex());
          ASSERT(1  == X1.lookupRecord("TEAM")->recordIndex());
          ASSERT(0  == X1.lookupRecord("COUNTRY"));
          ASSERT(0  == X1.lookupEnumeration("COLORS")->enumerationIndex());

          ASSERT(x1 == x1);      ASSERT(!(x1 != x1));
          ASSERT(x1 == x2);      ASSERT(!(x1 != x2));

          if (veryVerbose) PS(X1);

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
          if (verbose) bsl::cout << "\n 7. Empty x1 and create (empty) x3."
                                    "\t\t{ x1:  x2:V x3: }" << bsl::endl;
          x1.removeAll();
          ASSERT(0  == X1.numRecords());
          ASSERT(0  == X1.numEnumerations());

          Obj x3(Z);  const Obj& X3 = x3;

          ASSERT(0  == X3.numRecords());
          ASSERT(0  == X3.numEnumerations());

          ASSERT(X3 == X1);      ASSERT(!(X3 != X1));
          ASSERT(!(X3 == X2));   ASSERT(X3 != X2);
          ASSERT(X3 == X3);      ASSERT(!(X3 != X3));

          if (veryVerbose) { PS(X1); PS(X3); }

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
          if (verbose) bsl::cout << "\n 8. Create x4 (copy of x2)."
                                    "\t\t\t{ x1:  x2:V x3:  x4:V }"
                                 << bsl::endl;
          Obj x4(X2, Z);
          const Obj& X4 = x4;  if (veryVerbose) PS(X4);

          ASSERT(2  == X4.numRecords());
          ASSERT(1  == X4.numEnumerations());

          ASSERT(0  == X4.lookupRecord("EMPLOYEE")->recordIndex());
          ASSERT(1  == X4.lookupRecord("TEAM")->recordIndex());
          ASSERT(0  == X4.lookupRecord("COUNTRY"));
          ASSERT(0  == X4.lookupEnumeration("COLORS")->enumerationIndex());

          ASSERT(!(X4 == X1));   ASSERT(X4 != X1);
          ASSERT(X4 == X2);      ASSERT(!(X4 != X2));
          ASSERT(!(X4 == X3));   ASSERT(X4 != X3);
          ASSERT(X4 == X4);      ASSERT(!(X4 != X4));

          if (veryVerbose) PS(X4);

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
          if (verbose)
              bsl::cout << "\n 9. Assign x1 = x2 (empty becomes non-empty)."
                           "\t{ x1:V x2:V x3:  x4:V }" << bsl::endl;
          x1 = X2;
          ASSERT(2  == X1.numRecords());
          ASSERT(1  == X1.numEnumerations());

          ASSERT(0  == X1.lookupRecord("EMPLOYEE")->recordIndex());
          ASSERT(1  == X1.lookupRecord("TEAM")->recordIndex());
          ASSERT(0  == X1.lookupRecord("COUNTRY"));
          ASSERT(0  == X1.lookupEnumeration("COLORS")->enumerationIndex());

          ASSERT(X1 == X1);          ASSERT(!(X1 != X1));
          ASSERT(X1 == X2);          ASSERT(!(X1 != X2));
          ASSERT(!(X1 == X3));       ASSERT(X1 != X3);
          ASSERT(X1 == X4);          ASSERT(!(X1 != X4));

          if (veryVerbose) PS(X1);

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
          if (verbose)
              bsl::cout << "\n10. Assign x4 = x3 (non-empty becomes empty)."
                           "\t{ x1:V x2:V x3:  x4: }" << bsl::endl;
          x4 = X3;
          ASSERT(0  == X4.numRecords());
          ASSERT(0  == X4.numEnumerations());

          ASSERT(0  == X4.lookupRecord("EMPLOYEE"));
          ASSERT(0  == X4.lookupRecord("TEAM"));
          ASSERT(0  == X4.lookupRecord("COUNTRY"));
          ASSERT(0  == X4.lookupEnumeration("COLORS"));

          ASSERT(!(X4 == X1));      ASSERT(X4 != X1);
          ASSERT(!(X4 == X2));      ASSERT(X4 != X2);
          ASSERT(X4 == X3);         ASSERT(!(X4 != X3));
          ASSERT(X4 == X4);         ASSERT(!(X4 != X4));

          if (veryVerbose) PS(X4);

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
          if (verbose) bsl::cout << "\n11. Assign x2 = x2 (aliasing)."
                                    "\t\t\t{ x1:V x2:V x3:  x4: }"
                                 << bsl::endl;
          x2 = X2;
          ASSERT(2  == X2.numRecords());
          ASSERT(1  == X2.numEnumerations());

          ASSERT(0  == X2.lookupRecord("EMPLOYEE")->recordIndex());
          ASSERT(1  == X2.lookupRecord("TEAM")->recordIndex());
          ASSERT(0  == X2.lookupRecord("COUNTRY"));
          ASSERT(0  == X2.lookupEnumeration("COLORS")->enumerationIndex());
          ASSERT(colors2 == X2.lookupEnumeration("COLORS"));

          ASSERT(X2 == X1);         ASSERT(!(X2 != X1));
          ASSERT(X2 == X2);         ASSERT(!(X2 != X2));
          ASSERT(!(X2 == X3));      ASSERT(X2 != X3);
          ASSERT(!(X2 == X4));      ASSERT(X2 != X4);

          if (veryVerbose) PS(X2);

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
          const int VERSION = 1;

          if (verbose) {
              bsl::cout << "\n12. Stream out x2 then stream value into x4."
                           "\t{ x1:V x2:V x3:  x4:V }" << bsl::endl;
              bsl::cout << "    (Using 'bdex' streaming version *"
                        << VERSION << "*.)" << bsl::endl;
          }

          Out out;
          out.putVersion(VERSION);
          X2.bdexStreamOut(out, VERSION);

          if (veryVerbose) PS(X2);

          const char *OD  = out.data();
          const int   LOD = out.length();

          In testInStream(OD, LOD);  In& in = testInStream;  ASSERT(in);
          int version;
          in.getVersion(version);        ASSERT(VERSION == version);
          x4.bdexStreamIn(in, version);  ASSERT(in);

          if (veryVerbose) PS(X4);

          ASSERT(2  == X4.numRecords());
          ASSERT(1  == X4.numEnumerations());

          ASSERT(0  == X4.lookupRecord("EMPLOYEE")->recordIndex());
          ASSERT(1  == X4.lookupRecord("TEAM")->recordIndex());
          ASSERT(0  == X4.lookupRecord("COUNTRY"));
          ASSERT(0  == X4.lookupEnumeration("COLORS")->enumerationIndex());

          ASSERT(X4 == X1);         ASSERT(!(X4 != X1));
          ASSERT(X4 == X2);         ASSERT(!(X4 != X2));
          ASSERT(!(X4 == X3));      ASSERT(X4 != X3);
          ASSERT(X4 == X4);         ASSERT(!(X4 != X4));

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
          if (verbose) bsl::cout << "\n13. Alphabetize x2.COLORS" << bsl::endl;

          const bdem_EnumerationDef* x2colors = X2.lookupEnumeration("COLORS");
          ASSERT(x2colors && 0 == x2colors->enumerationIndex());
          ASSERT(x2colors == colors2);

          // Iterate through names
          const char *nameIter = x2colors->nextLargerName(0).first;
          ASSERT(0 == bsl::strcmp(nameIter, "blue"));
          ASSERT(x2colors->lookupName(nameIter) == nameIter);
          nameIter = x2colors->nextLargerName(nameIter).first;
          ASSERT(0 == bsl::strcmp(nameIter, "green"));
          ASSERT(x2colors->lookupName(nameIter) == nameIter);
          nameIter = x2colors->nextLargerName(nameIter).first;
          ASSERT(0 == bsl::strcmp(nameIter, "red"));
          ASSERT(x2colors->lookupName(nameIter) == nameIter);
          nameIter = x2colors->nextLargerName(nameIter).first;
          ASSERT(0 == nameIter);
          ASSERT(x2colors->lookupName(nameIter) == nameIter);

          // Iterate through IDs.
          int idIter = x2colors->nextLargerId(INT_MIN).second;
          ASSERT(greenId == idIter);
          idIter = x2colors->nextLargerId(idIter).second;
          ASSERT(blueId == idIter);
          idIter = x2colors->nextLargerId(idIter).second;
          ASSERT(redId == idIter);
          idIter = x2colors->nextLargerId(idIter).second;
          ASSERT(bdetu_Unset<int>::unsetValue() == idIter);

          // Lookup enumerators
          ASSERT(greenId == x2colors->lookupId("green"));
          ASSERT(0 == bsl::strcmp("green", x2colors->lookupName(greenId)));
          ASSERT(blueId == x2colors->lookupId("blue"));
          ASSERT(0 == bsl::strcmp("blue", x2colors->lookupName(blueId)));
          ASSERT(redId == x2colors->lookupId("red"));
          ASSERT(0 == bsl::strcmp("red", x2colors->lookupName(redId)));
          ASSERT(redId == x2colors->maxId());

          colors2->alphabetize(-1);

          // Iterate through names
          nameIter = x2colors->nextLargerName(0).first;
          ASSERT(0 == bsl::strcmp(nameIter, "blue"));
          nameIter = x2colors->nextLargerName(nameIter).first;
          ASSERT(0 == bsl::strcmp(nameIter, "green"));
          nameIter = x2colors->nextLargerName(nameIter).first;
          ASSERT(0 == bsl::strcmp(nameIter, "red"));
          nameIter = x2colors->nextLargerName(nameIter).first;
          ASSERT(0 == nameIter);

          // Iterate through IDs.
          idIter = x2colors->nextLargerId(INT_MIN).second;
          ASSERT(-1 == idIter);
          idIter = x2colors->nextLargerId(idIter).second;
          ASSERT(0 == idIter);
          idIter = x2colors->nextLargerId(idIter).second;
          ASSERT(1 == idIter);
          idIter = x2colors->nextLargerId(idIter).second;
          ASSERT(bdetu_Unset<int>::unsetValue() == idIter);

          // Lookup enumerators
          ASSERT(-1 == x2colors->lookupId("blue"));
          ASSERT(0 == bsl::strcmp("blue", x2colors->lookupName(-1)));
          ASSERT(0 == x2colors->lookupId("green"));
          ASSERT(0 == bsl::strcmp("green", x2colors->lookupName(0)));
          ASSERT(1 == x2colors->lookupId("red"));
          ASSERT(0 == bsl::strcmp("red", x2colors->lookupName(1)));
          ASSERT(1 == x2colors->maxId());

          ASSERT(!(X2 == X1));      ASSERT(X2 != X1);
          ASSERT(X2 == X2);         ASSERT(!(X2 != X2));
          ASSERT(!(X2 == X3));      ASSERT(X2 != X3);
          ASSERT(!(X2 == X4));      ASSERT(X2 != X4);

          if (veryVerbose) PS(X2);

        } END_BSLMA_EXCEPTION_TEST

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error: non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
