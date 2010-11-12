// bdem_aggregateutil.t.cpp     -*-C++-*-

#include <bdem_aggregateutil.h>
#include <bdem_aggregate.h>
#include <bdem_elemref.h>

#include <bdet_datetime.h>
#include <bdet_date.h>
#include <bdet_time.h>
#include <bdet_datetimetz.h>
#include <bdet_datetz.h>
#include <bdet_timetz.h>

#include <bslma_default.h>                      // for testing only
#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only

#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

#include <bsl_cstdlib.h>    // atoi()
#include <bsl_cstring.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//  The component under test is a utility consisting of pure procedures.  Each
//  must perform as advertised.  The methods under test are logically of the
//  general form of 'operator=='.
//
//  There are there functions that need to be tested;
//  'areRowsApproximatelyEqual', 'areChoicesApproximatelyEqual', and
//  'areChoiceArraysApproximatelyEqual'.  These methods take two references
//  (Rows, Choices, and ChoiceArrays) and four tolerances.  The overloaded
//  versions of these methods are either inline forwards relatively trivial
//  loops extracting one row at a time from a table (and forwarding the
//  function call appropriately).  Nevertheless, the overall Test Plan is
//  structured more "incrementally".
//
//  After a brief Breathing Test, the methods are tested for sets of lists,
//  tables, choices and choice arrays with minor variations that are, in
//  general, independent of FLOAT and DOUBLE values, and with the default
//  tolerances.  These tests are substantially taken from 'bdem_aggregate'.
//  Then, in a separate series of test cases, the "fuzzy" functionality is
//  tested.  Here, the test is grouped into three cases: (1) using the default
//  tolerances, (2) specifying relative tolerance only, and (3) specifying
//  relative and absolute tolerances.  This pattern follows the tests in
//  'bdeimp_fuzzy'.  To adequately test the various parameter-passing
//  mechanisms and also the unique control-flow paths of the one non-trivial
//  function, each of these three cases varies one 'float' or 'double' value
//  in each possible "contained" position within a list, table, choice, or
//  choice array.  Note that the tests of varied 'double' values explicitly
//  pass two, three, or four arguments, while tests of 'float' values pass
//  two, five, or six values.
// ----------------------------------------------------------------------------
// [ 4] bool areElemRefsApproximatelyEqual(lhs, rhs);
// [ 5] bool areElemRefsApproximatelyEqual(lhs, rhs, dRel);
// [ 6] bool areElemRefsApproximatelyEqual(lhs, rhs, dRel, dAbs);
// [ 7] bool areElemRefsApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel);
// [ 6] bool areElemRefsApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel, fAbs);
//
// [ 4] bool areRowsApproximatelyEqual(lhs, rhs);
// [ 5] bool areRowsApproximatelyEqual(lhs, rhs, dRel);
// [ 6] bool areRowsApproximatelyEqual(lhs, rhs, dRel, dAbs);
// [ 7] bool areRowsApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel);
// [ 6] bool areRowsApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel, fAbs);
//
// [ 4] bool areListsApproximatelyEqual(lhs, rhs);
// [ 5] bool areListsApproximatelyEqual(lhs, rhs, dRel);
// [ 6] bool areListsApproximatelyEqual(lhs, rhs, dRel, dAbs);
// [ 7] bool areListsApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel);
// [ 6] bool areListsApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel, fAbs);
//
// [ 4] bool areTablesApproximatelyEqual(lhs, rhs);
// [ 5] bool areTablesApproximatelyEqual(lhs, rhs, dRel);
// [ 6] bool areTablesApproximatelyEqual(lhs, rhs, dRel, dAbs);
// [ 7] bool areTablesApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel);
// [ 6] bool areTablesApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel, fAbs);
//
// [ 4] bool areChoicesApproximatelyEqual(lhs, rhs);
// [ 5] bool areChoicesApproximatelyEqual(lhs, rhs, dRel);
// [ 6] bool areChoicesApproximatelyEqual(lhs, rhs, dRel, dAbs);
// [ 7] bool areChoicesApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel);
// [ 6] bool areChoicesApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel, fAbs);
//
// [ 4] bool areChoiceArraysApproximatelyEqual(lhs, rhs);
// [ 5] bool areChoiceArraysApproximatelyEqual(lhs, rhs, dRel);
// [ 6] bool areChoiceArraysApproximatelyEqual(lhs, rhs, dRel, dAbs);
// [ 7] bool areChoiceArraysApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel);
// [ 6] bool areChoiceArraysApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel,
//                                                                  fAbs);
//
// [ 2] bool         areRowsApproximatelyEqual(lhs, rhs, 0, 0, 0, 0);
// [ 2] bool        areListsApproximatelyEqual(lhs, rhs, 0, 0, 0, 0);
// [ 3] bool       areTablesApproximatelyEqual(lhs, rhs, 0, 0, 0, 0);
// [ 2] bool      areChoicesApproximatelyEqual(lhs, rhs, 0, 0, 0, 0);
// [ 2] bool areChoiceArraysApproximatelyEqual(lhs, rhs, 0, 0, 0, 0);

// ----------------------------------------------------------------------------
// [ 8] USAGE TEST
// [ 1] BREATHING TEST
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

//=============================================================================
//                              Test apparatus
//-----------------------------------------------------------------------------

#define NL "\n"

//==========================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//--------------------------------------------------------------------------

typedef bdem_AggregateUtil Util;
typedef bdem_ConstElemRef  ConstElemRef;
typedef bdem_Row           Row;
typedef bdem_List          List;
typedef bdem_Table         Table;
typedef bdem_Choice        Choice;
typedef bdem_ChoiceArray   ChoiceArray;

typedef bsls_PlatformUtil::Int64 Int64;

const double RELDEF = 1e-12;  // 2/28/03: 'double' and 'float' have the same
const double ABSDEF = 1e-24;  // Rel & Abs Tolerances.  KEEP IN SYNC w/.cpp

//=============================================================================
//          HELPER FUNCTIONS OPERATING ON 'g' AND 'h' 'spec' CHARACTERS
//
// These functions identify the 'type' and 'value' characters of the 'g' and
// 'h' languages, and convert to and from 'char' (string) and 'int' (index)
// representations.  Some of these are used by 'ggg' and/or 'hhh' directly, but
// the complete set is made available for general use.
//-----------------------------------------------------------------------------

static const char TYPE_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
    // letters that correspond to the bdem_ElemTypes, retrieval in
    // appendElement is based off these chars

static bool isTypeChar(char c)
    // Return 'true' if the specified character 'c' is in TYPE_CHARS
    // and 'false' otherwise.
{
    return 0 != c && 0 != strchr(TYPE_CHARS, c);
}

static bool isValChar(char c)
    // Return 'true' if the specified character 'c' is in "xyu", and 'false'
    // otherwise.
{
    return 'x' == c || 'y' == c || 'u' == c;
}

static int typeCharToIndex(char type)
    // Return the integer index of the specified 'type' character.  The
    // behavior is undefined unless 'type' is one of TYPE_CHARS
{
    ASSERT(isTypeChar(type));
    int space = 0;
    if ('a' <= type) {
        space = 'a' - '[';
    }
    return strchr(TYPE_CHARS, type) - TYPE_CHARS - space;
}

static int valCharToIndex(char val)
    // Return the integer index of the specified 'val' character.  The behavior
    // is undefined unless 'val' is one of "xyu".
{
    ASSERT(isValChar(val));
    static const char VAL_CHARS[] = "xyu";
    return strchr(VAL_CHARS, val) - VAL_CHARS;
}

static char indexToTypeChar(int index)
    // Return the type character that corresponds to the specified 'index'.
    // The behavior is undefined unless
    // '0 <= index < bdem_ElemType::BDEM_NUM_TYPES'.
{
    ASSERT(0 <= index && index < bdem_ElemType::BDEM_NUM_TYPES);
    return TYPE_CHARS[index];
}

#if 0
static char indexToValChar(int index)
    // Return the value character that corresponds to the specified 'index'.
    // The behavior is undefined unless 0 <= index < 3.
{
    ASSERT(0 <= index && index < 3);
    static const char VAL_CHARS[] = "xyu";
    return VAL_CHARS[index];
}
#endif

//=============================================================================
//                        TEST DATA POINTER ARRAYS
//
// This set of 96 pointers (32 array-of-3 types) are defined here at global
// scope because they are used by the 'ggg' helper 'appendElement'.  They are
// populated in 'main' after the definition of the specific test values, but
// are available as needed to various additional helper functions.
//-----------------------------------------------------------------------------

static const char                         *G_VALUES_A[3];  // { &XA, &YA, &UA }
static const short                        *G_VALUES_B[3];  // { &XB, &YB, &UB }
static const int                          *G_VALUES_C[3];  // { &XC, &YC, &UC }
static const Int64                        *G_VALUES_D[3];  // ...
static const float                        *G_VALUES_E[3];
static const double                       *G_VALUES_F[3];
static const bsl::string                  *G_VALUES_G[3];
static const bdet_Datetime                *G_VALUES_H[3];
static const bdet_Date                    *G_VALUES_I[3];
static const bdet_Time                    *G_VALUES_J[3];
static const bsl::vector<char>            *G_VALUES_K[3];
static const bsl::vector<short>           *G_VALUES_L[3];
static const bsl::vector<int>             *G_VALUES_M[3];
static const bsl::vector<Int64>           *G_VALUES_N[3];
static const bsl::vector<float>           *G_VALUES_O[3];
static const bsl::vector<double>          *G_VALUES_P[3];
static const bsl::vector<bsl::string>     *G_VALUES_Q[3];
static const bsl::vector<bdet_Datetime>   *G_VALUES_R[3];
static const bsl::vector<bdet_Date>       *G_VALUES_S[3];
static const bsl::vector<bdet_Time>       *G_VALUES_T[3];
static const bdem_List                    *G_VALUES_U[3];
static const bdem_Table                   *G_VALUES_V[3];
static const bool                         *G_VALUES_W[3];
static const bsl::vector<bool>            *G_VALUES_X[3];
static const bdem_Choice                  *G_VALUES_Y[3];
static const bdem_ChoiceArray             *G_VALUES_Z[3];  // { &XZ, &YZ, &UZ }
static const bdet_DatetimeTz              *G_VALUES_a[3];  // { &Xa, &Ya, &Ua }
static const bdet_DateTz                  *G_VALUES_b[3];  // { &Xb, &Yb, &Ub }
static const bdet_TimeTz                  *G_VALUES_c[3];  // { &Xc, &Yc, &Uc }
static const bsl::vector<bdet_DatetimeTz> *G_VALUES_d[3];
static const bsl::vector<bdet_DateTz>     *G_VALUES_e[3];
static const bsl::vector<bdet_TimeTz>     *G_VALUES_f[3];

//=============================================================================
//                     HELPER FUNCTIONS FOR 'ggg', 'hhh'
//-----------------------------------------------------------------------------

static void
bug(const char *spec, int position, const char *description, int showErrorFlag)
    // Write the specified 'spec' and the specified error 'description' to
    // 'cout', identifying the specified 'position' of the errant character in
    // 'spec', only if the specified 'showErrorFlag' is non-zero.
{
    if (showErrorFlag) {
        cout << "spec : " << spec << endl;
        cout << description << " at position " << position;
        if ('\0' != spec[position]) {
            cout << " ('" << spec[position] << "')." << endl;
        }
        else {
            cout << " ('')." << endl;
        }
    }
    return;
}

static void appendElement(bdem_List *address, char val, char type)
    // Append an element of the 'bdem_ElemType::Type' corresponding to the
    // specified 'type' character and having the value corresponding to the
    // specified 'val' character to the 'bdem_List' at the specified 'address'.
{
    ASSERT(isValChar(val));
    int i = valCharToIndex(val);

    switch (type) {
      case 'A': address->appendChar(*G_VALUES_A[i]);             break;
      case 'B': address->appendShort(*G_VALUES_B[i]);            break;
      case 'C': address->appendInt(*G_VALUES_C[i]);              break;
      case 'D': address->appendInt64(*G_VALUES_D[i]);            break;
      case 'E': address->appendFloat(*G_VALUES_E[i]);            break;
      case 'F': address->appendDouble(*G_VALUES_F[i]);           break;
      case 'G': address->appendString(*G_VALUES_G[i]);           break;
      case 'H': address->appendDatetime(*G_VALUES_H[i]);         break;
      case 'I': address->appendDate(*G_VALUES_I[i]);             break;
      case 'J': address->appendTime(*G_VALUES_J[i]);             break;

      case 'K': address->appendCharArray(*G_VALUES_K[i]);        break;
      case 'L': address->appendShortArray(*G_VALUES_L[i]);       break;
      case 'M': address->appendIntArray(*G_VALUES_M[i]);         break;
      case 'N': address->appendInt64Array(*G_VALUES_N[i]);       break;
      case 'O': address->appendFloatArray(*G_VALUES_O[i]);       break;
      case 'P': address->appendDoubleArray(*G_VALUES_P[i]);      break;
      case 'Q': address->appendStringArray(*G_VALUES_Q[i]);      break;
      case 'R': address->appendDatetimeArray(*G_VALUES_R[i]);    break;
      case 'S': address->appendDateArray(*G_VALUES_S[i]);        break;
      case 'T': address->appendTimeArray(*G_VALUES_T[i]);        break;

      case 'U': address->appendList(*G_VALUES_U[i]);             break;
      case 'V': address->appendTable(*G_VALUES_V[i]);            break;

      case 'W': address->appendBool(*G_VALUES_W[i]);             break;
      case 'X': address->appendBoolArray(*G_VALUES_X[i]);        break;
      case 'Y': address->appendChoice(*G_VALUES_Y[i]);           break;
      case 'Z': address->appendChoiceArray(*G_VALUES_Z[i]);      break;

      case 'a': address->appendDatetimeTz(*G_VALUES_a[i]);       break;
      case 'b': address->appendDateTz(*G_VALUES_b[i]);           break;
      case 'c': address->appendTimeTz(*G_VALUES_c[i]);           break;
      case 'd': address->appendDatetimeTzArray(*G_VALUES_d[i]);  break;
      case 'e': address->appendDateTzArray(*G_VALUES_e[i]);      break;
      case 'f': address->appendTimeTzArray(*G_VALUES_f[i]);      break;

      default:  P(val);  P(type);  ASSERT(!"Bad type character");
    }
    return;
}

#if 0
static void appendElement(bdem_List *address, int typeIdx, int valIdx)
{
    appendElement(address, indexToValChar(valIdx), indexToTypeChar(typeIdx));
    return;
}
#endif

//-----------------------------------------------------------------------------

struct Assertions {
    char assertion1[sizeof(int) == sizeof(bdem_ElemType::Type)];
};

void getElemTypes(bsl::vector<bdem_ElemType::Type> *types, const bdem_Row& row)
    // Fill types with element types from row
{
    int numTypes = row.length();
    types->clear();
    types->reserve(numTypes);
    for (int i = 0; i < numTypes; ++i)
    {
        types->push_back(row.elemType(i));
    }
}

inline
void getElemTypes(bsl::vector<bdem_ElemType::Type> *types,
                  const bdem_List&                  list)
    // Fill types with element types from list
{
    getElemTypes(types, list.row());
}

void getElemTypes(bsl::vector<bdem_ElemType::Type> *types,
                  const bdem_Table&                 table)
    // Fill types with element types from table
{
    int numTypes = table.numColumns();
    types->clear();
    types->reserve(numTypes);
    for (int i = 0; i < numTypes; ++i)
    {
        types->push_back(table.columnType(i));
    }
}

void choiceArrayFromList(ChoiceArray& choiceArray, const Row& row)
    // populate choiceArray with the elements from the row
{
    bsl::vector<bdem_ElemType::Type> elemTypes;
    getElemTypes(&elemTypes, row);

    choiceArray.reset(elemTypes);

    int length = row.length();
    choiceArray.appendNullItems(length);

    for (int i = 0; i < length; i++) {
        bdem_ConstElemRef ref = row[i];
        choiceArray.theModifiableItem(i).makeSelection(i);
        choiceArray.theModifiableItem(i).selection().replaceValue(ref);
    }

}

void choiceArrayFromList(ChoiceArray& choiceArray, const List& list)
    // populate choiceArray with the elements from the list
{
    choiceArrayFromList(choiceArray, list.row());
}

//=============================================================================
//       GENERATOR FUNCTIONS 'g', 'gg', AND 'ggg' FOR TESTING LISTS
//-----------------------------------------------------------------------------
// The 'g' family of functions generate a 'bdem_List' object for testing.
// They, along with their helpers, interpret a given 'spec' (from left to
// right) to configure the list according to a custom language.  The 22
// characters [A-V] represent the 22 'bdem_ElemType' types.  'x', 'y', and
// 'u', respectively, represent two arbitrary but unique values and the 'unset'
// value (of the appropriate type).  Value and type characters must occur in
// pairs and generate the appropriate 'append'.  A tilde ('~') removes all
// elements, leaving the list in the logical (empty and unset) default creation
// state.  The full language specification follows.
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>        ::= <INSTRUCTION> | <INSTRUCTION><SPEC>
//
// <INSTRUCTION> ::= <VALUE><TYPE> | <REMOVE_ALL> | <EMPTY>
//
// <VALUE>       ::= = 'x' | 'y' | 'u'  // two arbitrary values plus "unset"
//
// <TYPE>        ::= 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' |
//                   'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' |
//                   'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z' | 'a' |
//                   'b' | 'c' | 'd' | 'e' | 'f'
//                                         // the 32 'bdem_ElemType' types
//
// <REMOVE_ALL>  ::= '~'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object unaltered.
// "xC"         Produces: LIST { INT 103 }
// "xCyF"       Produces: LIST { INT 103 DOUBLE 206.006 }
// "xCyFuG"     Produces: LIST { INT 103 DOUBLE 206.006 STRING NULL }
// "xCyFuG~"    Produces: LIST { INT 103 DOUBLE 206.006 STRING NULL }, and then
//              removes all elements.
//-----------------------------------------------------------------------------

enum {
    // 'enum' of parse errors for 'ggg'.
    GGG_SUCCESS = 0,
    GGG_EXPECTED_TYPE_CHARACTER,
    GGG_INAPPROPRIATE_CHARACTER
};

static int ggg(bdem_List *address, const char *spec, int showErrorFlag)
    // Configure the modifiable 'bdem_List' at the specified 'address'
    // according to the specified 'spec'.  Report errors to 'cout' only if the
    // specified 'showErrorFlag' is non-zero.  Return 0 on success, and a
    // unique non-zero global error code otherwise.  Note that this function
    // uses only the primary manipulators of the test plan.  Note also that
    // neither pointer argument may be null.
{
    for (const char *p = spec; *p; ++p) {
        if ('~' == *p) {
            address->removeAll();
            continue;
        }
        if (isValChar(*p)) {
            char valChar = *p++;
            if (isTypeChar(*p)) {
                appendElement(address, valChar, *p);
                continue;
            }
            bug(spec, p - spec, "Expected type character", showErrorFlag);
            return GGG_EXPECTED_TYPE_CHARACTER;
        }
        bug(spec, p - spec, "Inappropriate character", showErrorFlag);
        return GGG_INAPPROPRIATE_CHARACTER;
    }
    return GGG_SUCCESS;
}

bdem_List& gg(bdem_List *address, const char *spec)
    // Configure the modifiable 'bdem_List' object at the specified 'address'
    // according to the specified 'spec', and return a reference to the
    // modifiable list.  Note that this function uses only the primary
    // manipulators of the test plan.  Note also that neither argument may be
    // null.
{
    enum { SHOW_ERROR_FLAG = 1 };
    int status = ggg(address, spec, SHOW_ERROR_FLAG);
    ASSERT(GGG_SUCCESS == status);
    return *address;
}

bdem_List g(const char *spec)
    // Return by value a new 'bdem_List' object corresponding to the specified
    // 'spec'.
{
    bdem_List object;
    return gg(&object, spec);
}

//=============================================================================
//              GENERATOR FUNCTIONS 'h', 'hh', AND 'hhh' FOR TESTING
//-----------------------------------------------------------------------------
// The 'h' family of functions generate a 'bdem_Table' object for testing.
// They, along with their helpers, interpret a given 'spec' (from left to
// right) to configure the table according to a custom language.  The 22
// characters [A-V] represent the 22 'bdem_ElemType' types.  'x', 'y', and
// 'u', respectively, represent two arbitrary but unique values and the 'unset'
// value (of the appropriate type).  A semicolon (';') terminates each type
// field and each value field.  Any value field must have the same number of
// entries as the current type field.  The initial type field corresponds to
// the 'columnTypes' of the input 'bdem_Table'.  When a complete type field is
// recognized, the 'columnTypes' of the table are reset to the appropriate
// values.  When a complete, valid value field is recognized, an appropriate
// row is appended to the table.  A tilde ('~') resets the table to its empty
// (and unset) logical state.  A bang ('!') removes all rows (but does not
// alter the 'columnTypes').  Note that '~' and '!' may not occur within
// (incomplete) type or value fields.  The full language specification follows.
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>        ::= <SEQ> | <EMPTY>
//
// <SEQ>         ::= <ROWDEF><ROWS> | <REMOVE>
//
// <ROWDEF>      ::= <TYPE><DELIM>
//
// <TYPE>        ::= <T> | <T><TYPE>
//
// <T>           ::= 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' |
//                   'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' |
//                   'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z' | 'a' |
//                   'b' | 'c' | 'd' | 'e' | 'f'
//                                         // the 32 'bdem_ElemType' types
//
// <DELIM>       ::= ';'
//
// <ROWS>        ::= <ROW> | <ROW><ROWS> | ^
//
// <ROW>         ::= <VAL><DELIM>
//
// <VAL>         ::= <V> | <V><VAL>
//
// <V>           ::= = 'x' | 'y' | 'u'  // two arbitrary values plus "unset"
//
// <REMOVE>      ::= <REMOVE_ROWS> | <REMOVE_ALL>
//
// <REMOVE_ROWS> ::= '!'
//
// <REMOVE_ALL>  ::= '~'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object empty.
// "CFG;"       Sets the columnTypes to "INT DOUBLE STRING"
// "CF;xx;"     Sets the columnTypes to "INT DOUBLE" and appends a row of
//              appropriate 'x' values
// "CF;xx;yy;"  Sets the columnTypes to "INT DOUBLE", appends a row of
//              appropriate 'x' values, and then appends a row of appropriate
//              'y' values.
// "CF;xx;yy;!" Sets the columnTypes to "INT DOUBLE", appends a row of
//              appropriate 'x' values, appends a row of appropriate 'y'
//              values, and then removes the rows (leaving the column types
//              intact).
// "CF;xx;yy;~" Sets the columnTypes to "INT DOUBLE", appends a row of
//              appropriate 'x' values, appends a row of appropriate 'y'
//              values, and then removes the rows and resets column types
//              to the initial empty state.
//-----------------------------------------------------------------------------

enum {
    // 'enum' of parse errors for 'hhh'.
    HHH_SUCCESS = 0,
    HHH_TYPEFIELD_UNTERMINATED,
    HHH_VALFIELD_UNTERMINATED,
    HHH_MISSING_EXPECTED_VALUE,
    HHH_ILLEGAL_CHARACTER
};

static int hhh(bdem_Table *address, const char *spec, int showErrorFlag)
    // Configure the modifiable 'bdem_Table' at the specified 'address'
    // according to the specified 'spec'.  Report errors to 'cout' only if the
    // specified 'showErrorFlag' is non-zero.  Return 0 on success, and a
    // unique non-zero global error code otherwise.  Note that this function
    // uses only the primary manipulators of the test plan.  Note also that
    // neither pointer argument may be null.
{
    ASSERT(address);
    ASSERT(spec);

    const int   MAX     = 1000;     // maximum number of columns permitted
    char        buf[MAX * 2 + 1];   // buffer to accumulate types and values
    int         numCols;            // count of types (columns); 0 = "empty"

    // Note that this implementation parses the 'hhh' spec to build up a
    // sequence of 'ggg' specs, each of which is in turn used with 'gg' to
    // configure a temporary list from which the row is extracted to
    // configure the specified table.

    // Note that 'numCols' and the null-terminated 'buf' are 'for'-loop
    // invariants that preserve column-type information.  'buf[2 * i + 1]'
    // holds the column type character for each column index 'i' such that
    // 0 < i < numCols, and buf[2 * numCols] = '\0'.

    numCols = address->numColumns();
    for (int i = 0; i < numCols; ++i) {
        buf[2 * i + 1] = indexToTypeChar(address->columnType(i));
    }
    buf[2 * numCols] = '\0';

    for (const char *p = spec; *p; ++p) {
        if ('!' == *p) {
            address->removeAllRows();
        }
        else if ('~' == *p) {
            address->removeAll();
            numCols = 0;  buf[0] = '\0';     // reset column state to "empty"
        }
        else if (isTypeChar(*p)) {
            for (numCols = 0; isTypeChar(*p) && numCols < MAX; ++numCols) {
                buf[2 * numCols + 0] = 'u';
                buf[2 * numCols + 1] = *p++; // rewrite "stateful" 'buf'
            }
            buf[2 * numCols] = '\0';
            if (';' != *p) {
                bug(spec, p - spec, "Missing ';' after last valid type",
                    showErrorFlag);
                return HHH_TYPEFIELD_UNTERMINATED;
            }
            bdem_List tempList;
            bsl::vector<bdem_ElemType::Type> tempTypeArray;
            getElemTypes(&tempTypeArray, gg(&tempList,buf));
            address->reset(tempTypeArray);
        }
        else if (isValChar(*p) || ';' == *p) {
            int numVals;
            for (numVals = 0; isValChar(*p) && numVals < numCols; ++numVals) {
                buf[2 * numVals] = *p++;    // relies on type chars from above
            }
            if (';' != *p) {
                bug(spec, p - spec, "Missing ';' after last valid value",
                    showErrorFlag);
                return HHH_VALFIELD_UNTERMINATED;
            }
            if (numVals < numCols) {
                bug(spec, p - spec, "Missing expected value", showErrorFlag);
                return HHH_MISSING_EXPECTED_VALUE;
            }
            bdem_List tempList;
            address->appendRow(gg(&tempList, buf));
        }
        else {
            bug(spec, p - spec, "Illegal character", showErrorFlag);
            return HHH_ILLEGAL_CHARACTER;
        }
    }
    return HHH_SUCCESS;
}

bdem_Table& hh(bdem_Table *address, const char *spec)
    // Configure the modifiable 'bdem_Table' at the specified 'address'
    // according to the specified 'spec'.  Return a reference to '*address'.
    // Note that this function uses only the primary manipulators of the test
    // plan.  Note also that neither argument may be null.
{
    enum { SHOW_ERROR_FLAG = 1 };
    int status = hhh(address, spec, SHOW_ERROR_FLAG);
    ASSERT(HHH_SUCCESS == status);
    return *address;
}

bdem_Table h(const char *spec)
    // Return by value a new 'bdem_Table' object corresponding to the specified
    // 'spec'.
{
    bdem_Table object;
    return hh(&object, spec);
}

//=============================================================================
//              "Shorthand" Aliases For Functions Under Test
//-----------------------------------------------------------------------------
// These functions are convenient aliases for the four (overloaded)
// functions under test.  The 'Exact' functions forward four 0.0
// tolerances to emulate an expensive 'operator==', while the 'Approx'
// functions forward various tester-supplied arguments to the appropriate
// 'bdem_AggregateUtil' functions under test.

bool eExact(const ConstElemRef& lhs, const ConstElemRef& rhs)
{
    return Util::areElemRefsApproximatelyEqual(lhs, rhs, 0.0, 0.0, 0.0, 0.0);
}

bool elExact(const bdem_List& lhs, const bdem_List& rhs)
{
    for (int idx = 0; idx < lhs.length(); ++idx) {
        if (!eExact(lhs[idx], rhs[idx])) {
            return false;
        }
    }
    return true;
}

bool ecExact(const bdem_Choice& lhs, const bdem_Choice& rhs)
{
    return eExact(lhs.selection(), rhs.selection());
}

bool etExact(const bdem_Table& lhs, const bdem_Table& rhs)
{
    for (int rIdx = 0; rIdx < lhs.numRows(); ++rIdx) {
        for (int cIdx = 0; cIdx < lhs.numColumns(); ++cIdx) {
            if (!eExact(lhs[rIdx][cIdx], rhs[rIdx][cIdx])) {
                return false;
            }
        }
    }
    return true;
}

bool ecaExact(const bdem_ChoiceArray& lhs, const bdem_ChoiceArray& rhs)
{
    for (int idx = 0; idx < lhs.length(); ++idx) {
        if (!eExact(lhs[idx].selection(), rhs[idx].selection())) {
            return false;
        }
    }
    return true;
}

bool rExact(const Row& lhs, const Row& rhs)
{
    return Util::areRowsApproximatelyEqual(lhs, rhs, 0.0, 0.0, 0.0, 0.0);
}

bool lExact(const List& lhs, const List& rhs)
{
    return Util::areListsApproximatelyEqual(lhs, rhs, 0.0, 0.0, 0.0, 0.0);
}

bool tExact(const Table& lhs, const Table& rhs)
{
    return Util::areTablesApproximatelyEqual(lhs, rhs, 0.0, 0.0, 0.0, 0.0);
}

bool caExact(const ChoiceArray& lhs, const ChoiceArray& rhs)
{
    return Util::areChoiceArraysApproximatelyEqual(lhs,
                                                   rhs,
                                                   0.0,
                                                   0.0,
                                                   0.0,
                                                   0.0);
}

bool cExact(const Choice& lhs, const Choice& rhs)
{
    return Util::areChoicesApproximatelyEqual(lhs, rhs, 0.0, 0.0, 0.0, 0.0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool eApprox2(const ConstElemRef& lhs, const ConstElemRef& rhs)
{
    return Util::areElemRefsApproximatelyEqual(lhs, rhs);
}

bool elApprox2(const bdem_List& lhs, const bdem_List& rhs)
{
    for (int idx = 0; idx < lhs.length(); ++idx) {
        if (!eApprox2(lhs[idx], rhs[idx])) {
            return false;
        }
    }
    return true;
}

bool ecApprox2(const bdem_Choice& lhs, const bdem_Choice& rhs)
{
    return eApprox2(lhs.selection(), rhs.selection());
}

bool etApprox2(const bdem_Table& lhs, const bdem_Table& rhs)
{
    for (int rIdx = 0; rIdx < lhs.numRows(); ++rIdx) {
        for (int cIdx = 0; cIdx < lhs.numColumns(); ++cIdx) {
            if (!eApprox2(lhs[rIdx][cIdx], rhs[rIdx][cIdx])) {
                return false;
            }
        }
    }
    return true;
}

bool ecaApprox2(const bdem_ChoiceArray& lhs, const bdem_ChoiceArray& rhs)
{
    for (int idx = 0; idx < lhs.length(); ++idx) {
        if (!eApprox2(lhs[idx].selection(), rhs[idx].selection())) {
            return false;
        }
    }
    return true;
}

bool rApprox2(const Row& lhs, const Row& rhs)
{
    return Util::areRowsApproximatelyEqual(lhs, rhs);
}

bool lApprox2(const List& lhs, const List& rhs)
{
    return Util::areListsApproximatelyEqual(lhs, rhs);
}

bool tApprox2(const Table& lhs, const Table& rhs)
{
    return Util::areTablesApproximatelyEqual(lhs, rhs);
}

bool caApprox2(const ChoiceArray& lhs, const ChoiceArray& rhs)
{
    return Util::areChoiceArraysApproximatelyEqual(lhs, rhs);
}

bool cApprox2(const Choice& lhs, const Choice& rhs)
{
    return Util::areChoicesApproximatelyEqual(lhs, rhs);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool eApprox3(const ConstElemRef& lhs, const ConstElemRef& rhs, double dRel)
{
    return Util::areElemRefsApproximatelyEqual(lhs, rhs, dRel);
}

bool elApprox3(const bdem_List& lhs, const bdem_List& rhs, double dRel)
{
    const int lhsLen = lhs.length();
    if (lhsLen != rhs.length()) {
        return false;
    }

    for (int idx = 0; idx < lhsLen; ++idx) {
        if (!eApprox3(lhs[idx], rhs[idx], dRel)) {
            return false;
        }
    }
    return true;
}

bool ecApprox3(const bdem_Choice& lhs,
               const bdem_Choice& rhs,
               double             dRel)
{
    return eApprox3(lhs.selection(), rhs.selection(), dRel);
}

bool etApprox3(const bdem_Table& lhs, const bdem_Table& rhs, double dRel)
{
    for (int rIdx = 0; rIdx < lhs.numRows(); ++rIdx) {
        for (int cIdx = 0; cIdx < lhs.numColumns(); ++cIdx) {
            if (!eApprox3(lhs[rIdx][cIdx], rhs[rIdx][cIdx], dRel)) {
                return false;
            }
        }
    }
    return true;
}

bool ecaApprox3(const bdem_ChoiceArray& lhs,
                const bdem_ChoiceArray& rhs,
                double                  dRel)
{
    for (int idx = 0; idx < lhs.length(); ++idx) {
        if (!eApprox3(lhs[idx].selection(), rhs[idx].selection(), dRel)) {
            return false;
        }
    }
    return true;
}

bool rApprox3(const Row& lhs, const Row& rhs, double dRel)
{
    return Util::areRowsApproximatelyEqual(lhs, rhs, dRel);
}

bool lApprox3(const List& lhs, const List& rhs, double dRel)
{
    return Util::areListsApproximatelyEqual(lhs, rhs, dRel);
}

bool tApprox3(const Table& lhs, const Table& rhs, double dRel)
{
    return Util::areTablesApproximatelyEqual(lhs, rhs, dRel);
}

bool caApprox3(const ChoiceArray& lhs, const ChoiceArray& rhs, double dRel)
{
    return Util::areChoiceArraysApproximatelyEqual(lhs, rhs, dRel);
}

bool cApprox3(const Choice& lhs, const Choice& rhs, double dRel)
{
    return Util::areChoicesApproximatelyEqual(lhs, rhs, dRel);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool eApprox4(const ConstElemRef& lhs,
              const ConstElemRef& rhs,
              double              dRel,
              double              dAbs)
{
    return Util::areElemRefsApproximatelyEqual(lhs, rhs, dRel, dAbs);
}

bool elApprox4(const bdem_List& lhs,
               const bdem_List& rhs,
               double           dRel,
               double           dAbs)
{
    for (int idx = 0; idx < lhs.length(); ++idx) {
        if (!eApprox4(lhs[idx], rhs[idx], dRel, dAbs)) {
            return false;
        }
    }
    return true;
}

bool ecApprox4(const bdem_Choice& lhs,
               const bdem_Choice& rhs,
               double             dRel,
               double             dAbs)
{
    return eApprox4(lhs.selection(), rhs.selection(), dRel, dAbs);
}

bool etApprox4(const bdem_Table& lhs,
               const bdem_Table& rhs,
               double            dRel,
               double            dAbs)
{
    for (int rIdx = 0; rIdx < lhs.numRows(); ++rIdx) {
        for (int cIdx = 0; cIdx < lhs.numColumns(); ++cIdx) {
            if (!eApprox4(lhs[rIdx][cIdx], rhs[rIdx][cIdx], dRel, dAbs)) {
                return false;
            }
        }
    }
    return true;
}

bool ecaApprox4(const bdem_ChoiceArray& lhs,
                const bdem_ChoiceArray& rhs,
                double                  dRel,
                double                  dAbs)
{
    for (int idx = 0; idx < lhs.length(); ++idx) {
        if (!eApprox4(lhs[idx].selection(), rhs[idx].selection(),
                      dRel, dAbs)) {
            return false;
        }
    }
    return true;
}

bool rApprox4(const Row& lhs, const Row& rhs, double dRel, double dAbs)
{
    return Util::areRowsApproximatelyEqual(lhs, rhs, dRel, dAbs);
}

bool lApprox4(const List& lhs, const List& rhs, double dRel, double dAbs)
{
    return Util::areListsApproximatelyEqual(lhs, rhs, dRel, dAbs);
}

bool tApprox4(const Table& lhs, const Table& rhs, double dRel, double dAbs)
{
    return Util::areTablesApproximatelyEqual(lhs, rhs, dRel, dAbs);
}

bool caApprox4(const ChoiceArray& lhs, const ChoiceArray& rhs, double dRel,
                                                              double dAbs)
{
    return Util::areChoiceArraysApproximatelyEqual(lhs, rhs, dRel, dAbs);
}

bool cApprox4(const Choice& lhs, const Choice& rhs, double dRel,
                                                   double dAbs)
{
    return Util::areChoicesApproximatelyEqual(lhs, rhs, dRel, dAbs);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool eApprox5(const ConstElemRef& lhs,
              const ConstElemRef& rhs,
              double              dRel,
              double              dAbs,
              double              fRel)
{
    return Util::areElemRefsApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel);
}

bool elApprox5(const bdem_List& lhs,
               const bdem_List& rhs,
               double           dRel,
               double           dAbs,
               double           fRel)
{
    for (int idx = 0; idx < lhs.length(); ++idx) {
        if (!eApprox5(lhs[idx], rhs[idx], dRel, dAbs, fRel)) {
            return false;
        }
    }
    return true;
}

bool ecApprox5(const bdem_Choice& lhs,
               const bdem_Choice& rhs,
               double             dRel,
               double             dAbs,
               double             fRel)
{
    return eApprox5(lhs.selection(), rhs.selection(), dRel, dAbs, fRel);
}

bool etApprox5(const bdem_Table& lhs,
               const bdem_Table& rhs,
               double            dRel,
               double            dAbs,
               double            fRel)
{
    for (int rIdx = 0; rIdx < lhs.numRows(); ++rIdx) {
        for (int cIdx = 0; cIdx < lhs.numColumns(); ++cIdx) {
            if (!eApprox5(lhs[rIdx][cIdx], rhs[rIdx][cIdx],
                          dRel, dAbs, fRel)) {
                return false;
            }
        }
    }
    return true;
}

bool ecaApprox5(const bdem_ChoiceArray& lhs,
                const bdem_ChoiceArray& rhs,
                double                  dRel,
                double                  dAbs,
                double                  fRel)
{
    for (int idx = 0; idx < lhs.length(); ++idx) {
        if (!eApprox5(lhs[idx].selection(), rhs[idx].selection(),
                      dRel, dAbs, fRel)) {
            return false;
        }
    }
    return true;
}

bool rApprox5(const Row& lhs, const Row& rhs, double dRel, double dAbs,
                                             double fRel)
{
    return Util::areRowsApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel);
}

bool lApprox5(const List& lhs, const List& rhs, double dRel, double dAbs,
                                               double fRel)
{
    return Util::areListsApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel);
}

bool tApprox5(const Table& lhs, const Table& rhs, double dRel, double dAbs,
                                                 double fRel)
{
    return Util::areTablesApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel);
}

bool caApprox5(const ChoiceArray& lhs, const ChoiceArray& rhs, double dRel,
                                                              double dAbs,
                                                              double fRel)
{
    return Util::areChoiceArraysApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel);
}

bool cApprox5(const Choice& lhs, const Choice& rhs, double dRel,
                                                   double dAbs,
                                                   double fRel)
{
    return Util::areChoicesApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool eApprox6(const ConstElemRef& lhs,
              const ConstElemRef& rhs,
              double              dRel,
              double              dAbs,
              double              fRel,
              double              fAbs)
{
    return Util::areElemRefsApproximatelyEqual(lhs, rhs, dRel,
                                               dAbs, fRel, fAbs);
}

bool elApprox6(const bdem_List& lhs,
               const bdem_List& rhs,
               double           dRel,
               double           dAbs,
               double           fRel,
               double           fAbs)
{
    for (int idx = 0; idx < lhs.length(); ++idx) {
        if (!eApprox6(lhs[idx], rhs[idx], dRel, dAbs, fRel, fAbs)) {
            return false;
        }
    }
    return true;
}

bool ecApprox6(const bdem_Choice& lhs,
               const bdem_Choice& rhs,
               double             dRel,
               double             dAbs,
               double             fRel,
               double             fAbs)
{
    return eApprox6(lhs.selection(), rhs.selection(), dRel, dAbs, fRel, fAbs);
}

bool etApprox6(const bdem_Table& lhs,
               const bdem_Table& rhs,
               double            dRel,
               double            dAbs,
               double            fRel,
               double            fAbs)
{
    for (int rIdx = 0; rIdx < lhs.numRows(); ++rIdx) {
        for (int cIdx = 0; cIdx < lhs.numColumns(); ++cIdx) {
            if (!eApprox6(lhs[rIdx][cIdx], rhs[rIdx][cIdx],
                          dRel, dAbs, fRel, fAbs)) {
                return false;
            }
        }
    }
    return true;
}

bool ecaApprox6(const bdem_ChoiceArray& lhs,
                const bdem_ChoiceArray& rhs,
                double                  dRel,
                double                  dAbs,
                double                  fRel,
                double                  fAbs)
{
    for (int idx = 0; idx < lhs.length(); ++idx) {
        if (!eApprox6(lhs[idx].selection(), rhs[idx].selection(),
                      dRel, dAbs, fRel, fAbs)) {
            return false;
        }
    }
    return true;
}

bool rApprox6(const Row& lhs, const Row& rhs, double dRel, double dAbs,
                                             double fRel, double fAbs)
{
    return Util::areRowsApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel, fAbs);
}

bool lApprox6(const List& lhs, const List& rhs, double dRel, double dAbs,
                                               double fRel, double fAbs)
{
    return Util::areListsApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel, fAbs);
}

bool tApprox6(const Table& lhs, const Table& rhs, double dRel, double dAbs,
                                                 double fRel, double fAbs)
{
    return Util::areTablesApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel, fAbs);
}

bool caApprox6(const ChoiceArray& lhs, const ChoiceArray& rhs, double dRel,
                                                              double dAbs,
                                                              double fRel,
                                                              double fAbs)
{
    return Util::areChoiceArraysApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel,
                                                                         fAbs);
}

bool cApprox6(const Choice& lhs,
              const Choice& rhs,
              double dRel,
              double dAbs,
              double fRel,
              double fAbs)
{
    return Util::areChoicesApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel,
                                                                    fAbs);
}

//==========================================================================
//                               USAGE EXAMPLE
//--------------------------------------------------------------------------

///Usage
///-----
// As part of a transaction processing system consider a price book class that
// stores pricing quotes from various sources on a variety of fixed income
// securities.  For each security identified by its CUSIP number, the book
// stores the price, yield to maturity, coupon, and maturity date for multiple
// pricing sources.  Each pricing source is identified by a unique source id.
//
// The interface of the price book class allows adding securities, updating
// the pricing information of currently stored securities, and removing
// securities.  Additionally, the price book can be queried to find the
// pricing source that most closely matches a client's purchase price and
// acceptable tolerance levels.  Note that for illustration purposes the
// interface is intentionally kept very short.
//
// The following is a class definition for the price book:
//..
    class PriceBook {
        // This class stores the set of pricing information received from
        // various pricing sources, and additionally provides a function that
        // allows retrieval of the pricing source that supports the best match
        // for the client's pricing request.

        // TYPES
        typedef bsl::pair<int, bdem_List>                        SourceQuote;
        typedef bsl::map<bsl::string, bsl::vector<SourceQuote> > QuoteBook;

        // DATA
        QuoteBook d_quoteBook;

      public:
        // CREATORS
        PriceBook(bslma_Allocator *basicAllocator = 0);
            // Construct a price book.  Optionally specify a 'basicAllocator'
            // used to supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.

        ~PriceBook();
            // Destroy this price book.

        // MANIPULATORS
        void addSecurity(const bsl::string& cusip,
                         int                sourceId,
                         const bdem_List&   pricingInfo);
            // Add the security having the specified 'cusip' being sold by the
            // pricing source indicated by the specified 'sourceId' with the
            // specified 'pricingInfo'.

        int updateSecurity(const bsl::string& cusip,
                           int                sourceId,
                           const bdem_List&   pricingInfo);
            // Update the security having the specified 'cusip' being sold by
            // the pricing source indicated by the specified 'sourceId' to have
            // the specified 'pricingInfo'.  Return 0 on success, and a
            // non-zero value otherwise.

        int removeSecurity(const bsl::string& cusip, int sourceId);
            // Remove the security having the specified 'cusip' being sold by
            // the pricing source indicated by the specified 'sourceId'.
            // Return 0 on success, and a non-zero value otherwise.

        int loadMatchingSourceId(int                *sourceId,
                                 const bsl::string&  cusip,
                                 const bdem_List&    requestedPrice,
                                 double              relTolerance,
                                 double              absTolerance);
            // Load into the specified 'sourceId' the id of the pricing source
            // that matches the specified 'requestedPrice' for the specified
            // 'cusip' within the specified 'relTolerance' and 'absTolerance'
            // levels.
    };
//..
// The function definitions for the price book class are provided below:
//..
    // CREATORS
    PriceBook::PriceBook(bslma_Allocator *basicAllocator)
    : d_quoteBook(basicAllocator)
    {
    }

    PriceBook::~PriceBook()
    {
    }

    // MANIPULATORS
    void PriceBook::addSecurity(const bsl::string& cusip,
                                int                sourceId,
                                const bdem_List&   pricingInfo)
    {
        QuoteBook::iterator iter = d_quoteBook.find(cusip);
        if (iter == d_quoteBook.end()) {
            bsl::vector<SourceQuote> v;
            v.push_back(bsl::make_pair(sourceId, pricingInfo));
            d_quoteBook[cusip] = v;
        }
        else {
            iter->second.push_back(bsl::make_pair(sourceId, pricingInfo));
        }
    }

    int PriceBook::updateSecurity(const bsl::string& cusip,
                                  int                sourceId,
                                  const bdem_List&   pricingInfo)
    {
        enum { FAILURE = -1, SUCCESS };

        QuoteBook::iterator mapIter = d_quoteBook.find(cusip);
        if (mapIter == d_quoteBook.end()) {
            return FAILURE;                                           // RETURN
        }

        bsl::vector<SourceQuote>& priceVec  = mapIter->second;
        const int                 numPrices = priceVec.size();
        for (int i = 0; i < numPrices; ++i) {
            if (sourceId == priceVec[0].first) {
                priceVec[0].second = pricingInfo;
            }
        }
        return SUCCESS;
    }

    int PriceBook::removeSecurity(const bsl::string& cusip, int sourceId)
    {
        enum { FAILURE = -1, SUCCESS };

        QuoteBook::iterator mapIter = d_quoteBook.find(cusip);
        if (mapIter == d_quoteBook.end()) {
            return FAILURE;                                           // RETURN
        }

        bsl::vector<SourceQuote>& priceVec  = mapIter->second;
        const int                 numPrices = priceVec.size();
        for (int i = 0; i < numPrices; ++i) {
            if (sourceId == priceVec[0].first) {
                priceVec.erase(priceVec.begin() + i);
            }
        }
        return SUCCESS;
    }

    int PriceBook::loadMatchingSourceId(int                *sourceId,
                                        const bsl::string&  cusip,
                                        const bdem_List&    requestedPrice,
                                        double              relTolerance,
                                        double              absTolerance)
    {
        enum { FAILURE = -1, SUCCESS };

        QuoteBook::const_iterator iter = d_quoteBook.find(cusip);
        const bsl::vector<SourceQuote>& priceVec = iter->second;
        const int numPrices = priceVec.size();
        for (int i = 0; i < numPrices; ++i) {
            if (bdem_AggregateUtil::areListsApproximatelyEqual(
                                                            requestedPrice,
                                                            priceVec[i].second,
                                                            relTolerance,
                                                            absTolerance)) {
                *sourceId = priceVec[i].first;
                return SUCCESS;
            }
        }
        return FAILURE;
    }

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? atoi(argv[1]) : 0;
    int verbose         = argc > 2;
    int veryVerbose     = argc > 3;
    int veryVeryVerbose = argc > 4;

    bslma_TestAllocator  testAllocator(veryVeryVerbose);
    bslma_Allocator     *Z = &testAllocator;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // ------------------------------------------------------------------------
    // The following "global constants for testing" are in 'main' because the
    // test allocator (defined in 'main') is used in the constructors of the
    // test lists and tables.
    //
    // For each of the 22 'bdem_ElemType' types 't', make a set of three
    // useful constants for testing: Ut, Xt, Yt
    // ------------------------------------------------------------------------

    const char                       UA = char();
    const short                      UB = short();
    const int                        UC = int();
    const Int64                      UD = Int64();
    const float                      UE = float();
    const double                     UF = double();
    const bsl::string                UG;
    const bdet_Datetime              UH;
    const bdet_Date                  UI;
    const bdet_Time                  UJ;

    const bsl::vector<char>           UK;  // Empty arrays, lists, and tables
    const bsl::vector<short>          UL;  // make fine null values, so no
    const bsl::vector<int>            UM;  // special provisions are necessary.
    const bsl::vector<Int64>          UN;
    const bsl::vector<float>          UO;
    const bsl::vector<double>         UP;
    const bsl::vector<bsl::string>    UQ;
    const bsl::vector<bdet_Datetime>  UR;
    const bsl::vector<bdet_Date>      US;
    const bsl::vector<bdet_Time>      UT;

    const bdem_List                  UU;
    const bdem_Table                 UV;

    const bool                       UW = bool();
    const bsl::vector<bool>          UX;
    const bdem_Choice                UY;
    const bdem_ChoiceArray           UZ;

    const bdet_DatetimeTz               Ua;
    const bdet_DateTz                   Ub;
    const bdet_TimeTz                   Uc;
    const bsl::vector<bdet_DatetimeTz>  Ud;
    const bsl::vector<bdet_DateTz>      Ue;
    const bsl::vector<bdet_TimeTz>      Uf;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    const char                       XA = 'x';   // = 120 decimal
    const short                      XB = 12;
    const int                        XC = 103;
    const Int64                      XD = 10004;
    const float                      XE = 105.5;
    const double                     XF = 106.006;
    const bsl::string                XG = "one-seven---";
    const bdet_Datetime              XH(bdet_Date(108, 8, 8),
                                        bdet_Time(8, 8, 8, 108));
    const bdet_Date                  XI(109, 9, 9);
    const bdet_Time                  XJ(10, 10, 10, 110);

    bsl::vector<char>                 XK_;
    const bsl::vector<char>&          XK = XK_;
    XK_.push_back(XA);
    XK_.push_back('N');

    bsl::vector<short>                XL_;
    const bsl::vector<short>&         XL = XL_;
    XL_.push_back(XB);
    XL_.push_back(UB);

    bsl::vector<int>                  XM_;
    const bsl::vector<int>&           XM = XM_;
    XM_.push_back(XC);
    XM_.push_back(UC);

    bsl::vector<Int64>                XN_;
    const bsl::vector<Int64>&         XN = XN_;
    XN_.push_back(XD);
    XN_.push_back(UD);

    bsl::vector<float>                XO_;
    const bsl::vector<float>&         XO = XO_;
    XO_.push_back(XE);
    XO_.push_back(UE);

    bsl::vector<double>               XP_;
    const bsl::vector<double>&        XP = XP_;
    XP_.push_back(XF);
    XP_.push_back(UF);

    bsl::vector<bsl::string>          XQ_;
    const bsl::vector<bsl::string>&   XQ = XQ_;
    XQ_.push_back(XG);
    XQ_.push_back(UG);

    bsl::vector<bdet_Datetime>        XR_;
    const bsl::vector<bdet_Datetime>& XR = XR_;
    XR_.push_back(XH);
    XR_.push_back(UH);

    bsl::vector<bdet_Date>            XS_;
    const bsl::vector<bdet_Date>&     XS = XS_;
    XS_.push_back(XI);
    XS_.push_back(UI);

    bsl::vector<bdet_Time>            XT_;
    const bsl::vector<bdet_Time>&     XT = XT_;
    XT_.push_back(XJ);
    XT_.push_back(UJ);

    bdem_List                        XU_(Z);
    const bdem_List&                 XU = XU_;
    XU_.appendInt(XC);
    XU_.appendString(XG);

    bsl::vector<bdem_ElemType::Type> XUElemTypes;
    getElemTypes(&XUElemTypes, XU);
    bdem_Table XV_(XUElemTypes, Z);
    const bdem_Table& XV = XV_;
    XV_.appendNullRow();
    XV_.theModifiableRow(0)[0].theModifiableInt()    = XC;
    XV_.theModifiableRow(0)[1].theModifiableString() = XG;
    XV_.appendNullRow();
    XV_.theModifiableRow(1)[0].theModifiableInt()    = UC;
    XV_.theModifiableRow(1)[1].theModifiableString() = UG;

    const bool                       XW = true;

    bsl::vector<bool>                XX_;
    const bsl::vector<bool>&         XX = XX_;
    XX_.push_back(XW);
    XX_.push_back(false);

    bsl::vector<bdem_ElemType::Type> XYElemTypes;
    getElemTypes(&XYElemTypes, XU);
    bdem_Choice XY_(XYElemTypes, Z);
    const bdem_Choice& XY = XY_;
    XY_.makeSelection(0).theModifiableInt() = XC;

    bsl::vector<bdem_ElemType::Type> XZElemTypes;
    getElemTypes(&XZElemTypes, XU);
    bdem_ChoiceArray XZ_(XZElemTypes, Z);
    const bdem_ChoiceArray& XZ = XZ_;
    XZ_.appendNullItems(2);
    XZ_.theModifiableItem(0).makeSelection(0).theModifiableInt()    = XC;
    XZ_.theModifiableItem(1).makeSelection(1).theModifiableString() = XG;

    const bdet_DatetimeTz            Xa(XH, 10);
    const bdet_DateTz                Xb(XI, 10);
    const bdet_TimeTz                Xc(XJ, 10);

    bsl::vector<bdet_DatetimeTz>        Xd_;
    const bsl::vector<bdet_DatetimeTz>& Xd = Xd_;
    Xd_.push_back(Xa);
    Xd_.push_back(Ua);

    bsl::vector<bdet_DateTz>            Xe_;
    const bsl::vector<bdet_DateTz>&     Xe = Xe_;
    Xe_.push_back(Xb);
    Xe_.push_back(Ub);

    bsl::vector<bdet_TimeTz>            Xf_;
    const bsl::vector<bdet_TimeTz>&     Xf = Xf_;
    Xf_.push_back(Xc);
    Xf_.push_back(Uc);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    const char                       YA = 'y';    // = 121 decimal
    const short                      YB = 22;
    const int                        YC = 203;
    const Int64                      YD = 20004;
    const float                      YE = 205.5;
    const double                     YF = 206.006;
    const bsl::string                YG = "two-seven";
    const bdet_Datetime              YH(bdet_Date(208, 8, 8),
                                        bdet_Time(8, 8, 8, 208));
    const bdet_Date                  YI(209, 9, 9);
    const bdet_Time                  YJ(10, 10, 10, 210);

    bsl::vector<char>                 YK_;
    const bsl::vector<char>&          YK = YK_;
    YK_.push_back(YA);
    YK_.push_back('n');

    bsl::vector<short>                YL_;
    const bsl::vector<short>&         YL = YL_;
    YL_.push_back(YB);
    YL_.push_back(UB);

    bsl::vector<int>                  YM_;
    const bsl::vector<int>&           YM = YM_;
    YM_.push_back(YC);
    YM_.push_back(UC);

    bsl::vector<Int64>                YN_;
    const bsl::vector<Int64>&         YN = YN_;
    YN_.push_back(YD);
    YN_.push_back(UD);

    bsl::vector<float>                YO_;
    const bsl::vector<float>&         YO = YO_;
    YO_.push_back(YE);
    YO_.push_back(UE);

    bsl::vector<double>               YP_;
    const bsl::vector<double>&        YP = YP_;
    YP_.push_back(YF);
    YP_.push_back(UF);

    bsl::vector<bsl::string>          YQ_;
    const bsl::vector<bsl::string>&   YQ = YQ_;
    YQ_.push_back(YG);
    YQ_.push_back(UG);

    bsl::vector<bdet_Datetime>        YR_;
    const bsl::vector<bdet_Datetime>& YR = YR_;
    YR_.push_back(YH);
    YR_.push_back(UH);

    bsl::vector<bdet_Date>            YS_;
    const bsl::vector<bdet_Date>&     YS = YS_;
    YS_.push_back(YI);
    YS_.push_back(UI);

    bsl::vector<bdet_Time>            YT_;
    const bsl::vector<bdet_Time>&     YT = YT_;
    YT_.push_back(YJ);
    YT_.push_back(UJ);

    bdem_List                        YU_(Z);
    const bdem_List&                 YU = YU_;
    YU_.appendInt(YC);
    YU_.appendString(YG);

    bsl::vector<bdem_ElemType::Type> YUElemTypes;
    getElemTypes(&YUElemTypes, YU);
    bdem_Table YV_(&YUElemTypes[0],
                   YUElemTypes.size(),
                   Z);
    const bdem_Table&                YV = YV_;
    YV_.appendNullRow();
    YV_.theModifiableRow(0)[0].theModifiableInt()    = YC;
    YV_.theModifiableRow(0)[1].theModifiableString() = YG;
    YV_.appendNullRow();
    YV_.theModifiableRow(1)[0].theModifiableInt()    = UC;
    YV_.theModifiableRow(1)[1].theModifiableString() = UG;

    static bool                      YW = false;

    bsl::vector<bool>                YX_;
    const bsl::vector<bool>&         YX = YX_;
    YX_.push_back(YW);
    YX_.push_back(true);

    bsl::vector<bdem_ElemType::Type> YYElemTypes;
    getElemTypes(&YYElemTypes, YU);
    bdem_Choice YY_(YYElemTypes, Z);
    const bdem_Choice& YY = YY_;
    YY_.makeSelection(1).theModifiableString() = YG;

    bsl::vector<bdem_ElemType::Type> YZElemTypes;
    getElemTypes(&YZElemTypes, YU);
    bdem_ChoiceArray YZ_(YZElemTypes, Z);
    const bdem_ChoiceArray& YZ = YZ_;
    YZ_.appendNullItems(2);
    YZ_.theModifiableItem(0).makeSelection(1).theModifiableString() = YG;
    YZ_.theModifiableItem(1).makeSelection(0).theModifiableInt()    = YC;

    const bdet_DatetimeTz            Ya(YH, 10);
    const bdet_DateTz                Yb(YI, 10);
    const bdet_TimeTz                Yc(YJ, 10);

    bsl::vector<bdet_DatetimeTz>        Yd_;
    const bsl::vector<bdet_DatetimeTz>& Yd = Yd_;
    Yd_.push_back(Ya);
    Yd_.push_back(Ua);

    bsl::vector<bdet_DateTz>            Ye_;
    const bsl::vector<bdet_DateTz>&     Ye = Ye_;
    Ye_.push_back(Yb);
    Ye_.push_back(Ub);

    bsl::vector<bdet_TimeTz>            Yf_;
    const bsl::vector<bdet_TimeTz>&     Yf = Yf_;
    Yf_.push_back(Yc);
    Yf_.push_back(Uc);

    // ------------------------------------------------------------------------
    // populate global element pointers for generator function g("...")

    G_VALUES_A[0] = &XA;    G_VALUES_A[1] = &YA;    G_VALUES_A[2] = &UA;
    G_VALUES_B[0] = &XB;    G_VALUES_B[1] = &YB;    G_VALUES_B[2] = &UB;
    G_VALUES_C[0] = &XC;    G_VALUES_C[1] = &YC;    G_VALUES_C[2] = &UC;
    G_VALUES_D[0] = &XD;    G_VALUES_D[1] = &YD;    G_VALUES_D[2] = &UD;
    G_VALUES_E[0] = &XE;    G_VALUES_E[1] = &YE;    G_VALUES_E[2] = &UE;
    G_VALUES_F[0] = &XF;    G_VALUES_F[1] = &YF;    G_VALUES_F[2] = &UF;
    G_VALUES_G[0] = &XG;    G_VALUES_G[1] = &YG;    G_VALUES_G[2] = &UG;
    G_VALUES_H[0] = &XH;    G_VALUES_H[1] = &YH;    G_VALUES_H[2] = &UH;
    G_VALUES_I[0] = &XI;    G_VALUES_I[1] = &YI;    G_VALUES_I[2] = &UI;
    G_VALUES_J[0] = &XJ;    G_VALUES_J[1] = &YJ;    G_VALUES_J[2] = &UJ;
    G_VALUES_K[0] = &XK;    G_VALUES_K[1] = &YK;    G_VALUES_K[2] = &UK;
    G_VALUES_L[0] = &XL;    G_VALUES_L[1] = &YL;    G_VALUES_L[2] = &UL;
    G_VALUES_M[0] = &XM;    G_VALUES_M[1] = &YM;    G_VALUES_M[2] = &UM;
    G_VALUES_N[0] = &XN;    G_VALUES_N[1] = &YN;    G_VALUES_N[2] = &UN;
    G_VALUES_O[0] = &XO;    G_VALUES_O[1] = &YO;    G_VALUES_O[2] = &UO;
    G_VALUES_P[0] = &XP;    G_VALUES_P[1] = &YP;    G_VALUES_P[2] = &UP;
    G_VALUES_Q[0] = &XQ;    G_VALUES_Q[1] = &YQ;    G_VALUES_Q[2] = &UQ;
    G_VALUES_R[0] = &XR;    G_VALUES_R[1] = &YR;    G_VALUES_R[2] = &UR;
    G_VALUES_S[0] = &XS;    G_VALUES_S[1] = &YS;    G_VALUES_S[2] = &US;
    G_VALUES_T[0] = &XT;    G_VALUES_T[1] = &YT;    G_VALUES_T[2] = &UT;
    G_VALUES_U[0] = &XU;    G_VALUES_U[1] = &YU;    G_VALUES_U[2] = &UU;
    G_VALUES_V[0] = &XV;    G_VALUES_V[1] = &YV;    G_VALUES_V[2] = &UV;
    G_VALUES_W[0] = &XW;    G_VALUES_W[1] = &YW;    G_VALUES_W[2] = &UW;
    G_VALUES_X[0] = &XX;    G_VALUES_X[1] = &YX;    G_VALUES_X[2] = &UX;
    G_VALUES_Y[0] = &XY;    G_VALUES_Y[1] = &YY;    G_VALUES_Y[2] = &UY;
    G_VALUES_Z[0] = &XZ;    G_VALUES_Z[1] = &YZ;    G_VALUES_Z[2] = &UZ;
    G_VALUES_a[0] = &Xa;    G_VALUES_a[1] = &Ya;    G_VALUES_a[2] = &Ua;
    G_VALUES_b[0] = &Xb;    G_VALUES_b[1] = &Yb;    G_VALUES_b[2] = &Ub;
    G_VALUES_c[0] = &Xc;    G_VALUES_c[1] = &Yc;    G_VALUES_c[2] = &Uc;
    G_VALUES_d[0] = &Xd;    G_VALUES_d[1] = &Yd;    G_VALUES_d[2] = &Ud;
    G_VALUES_e[0] = &Xe;    G_VALUES_e[1] = &Ye;    G_VALUES_e[2] = &Ue;
    G_VALUES_f[0] = &Xf;    G_VALUES_f[1] = &Yf;    G_VALUES_f[2] = &Uf;

    //-------------------------------------------------------------------------
    // Convenient test objects for cases 4 - 6
    //-------------------------------------------------------------------------

    // "Helper" sub-values for the test list and table
    bsl::vector<float>  mFA;  mFA.push_back(0.0);  mFA.push_back(0.0);
    bsl::vector<double> mDA;  mDA.push_back(0.0);  mDA.push_back(0.0);
    List               subL(g("xExF"));
    Table              subT(h("EF;xx;"));

    // Create and prepare modifiable test aggregate values used to create the
    // desired 'const' test objects for cases 4 - 6.

    List  testList(g(
      "xAxBxCxDxExFxGxHxIxJxKxLxMxNxOxPxQxRxSxTxUxVxWxXxYxZxaxbxcxdxexf"));
    Table testTable(h("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef;"
                      "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx;"
                      "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx;"));

    testList.theModifiableList(20)        = subL;
    testList.theModifiableTable(21)       = subT;
    testTable.theModifiableRow(1)[20].theModifiableList()  = subL;
    testTable.theModifiableRow(1)[21].theModifiableTable() = subT;

    // Now create the 'const' test values used to initialize and re-set the
    // actual arguments for the 'areApproximatelyEqual' methods.

    const List  TEST_LIST(testList);
    const Table TEST_TABLE(testTable);

#define R1 L1.row()
#define R2 L2.row()

    // ========================================================================
    //                      MAIN TEST SWITCH BLOCKS
    // ========================================================================

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
//..
// Clients can use the price book class as follows:
//..
     PriceBook                        priceBook;
     bsl::vector<bdem_ElemType::Type> priceTypes;

     priceTypes.push_back(bdem_ElemType::BDEM_DOUBLE);  // price
     priceTypes.push_back(bdem_ElemType::BDEM_DOUBLE);  // yield to maturity
     priceTypes.push_back(bdem_ElemType::BDEM_DOUBLE);  // coupon
     priceTypes.push_back(bdem_ElemType::BDEM_INT);     // payment frequency
     priceTypes.push_back(bdem_ElemType::BDEM_DATE);    // maturity date

     const bsl::string CUSIP = "SL2S6Q3Y";

     const int srcId1 = 1, srcId2 = 2;
     bsl::vector<int> sourceIds;
     bdem_List        quote1(priceTypes), quote2(priceTypes);

     quote1[0].theModifiableDouble() = 101.53;
     quote1[1].theModifiableDouble() = .06;
     quote1[2].theModifiableDouble() = 70;
     quote1[3].theModifiableDate()   = bdet_Date(2012, 12, 31);

     quote2[0].theModifiableDouble() = 101.93;
     quote2[1].theModifiableDouble() = .057;
     quote2[2].theModifiableDouble() = 70;
     quote2[3].theModifiableDate()   = bdet_Date(2012, 12, 31);

     priceBook.addSecurity(CUSIP, srcId1, quote1);
     priceBook.addSecurity(CUSIP, srcId2, quote2);

     int srcId = -1;
     bdem_List requestedQuote(priceTypes);
     requestedQuote[0].theModifiableDouble() = 102;
     requestedQuote[1].theModifiableDouble() = .055;
     requestedQuote[2].theModifiableDouble() = 70;
     requestedQuote[3].theModifiableDate()   = bdet_Date(2012, 12, 31);
     int rc = priceBook.loadMatchingSourceId(&srcId,
                                             CUSIP,
                                             requestedQuote,
                                             0,
                                             0.1);
     ASSERT(0 == rc);
     ASSERT(2 == srcId);

     rc = priceBook.loadMatchingSourceId(&srcId,
                                         CUSIP,
                                         requestedQuote,
                                         0,
                                         0.5);
     ASSERT(0 == rc);
     ASSERT(1 == srcId);
//..
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING APPROXIMATE-EQUALITY FUNCTIONS FOR AGGREGATES
        // Concerns:
        //   The main underlying functionality has already been tested in cases
        //   2 and 3.  Our primary concern here is that the
        //   'areXXXApproximatelyEqual' functions correctly forward their
        //   arguments when default values are used for float absolute
        //   tolerances and non-default values for everything else.
        //
        // Plan:
        //   In a table-driven test, confirm the correct operation of the
        //   'areXXXApproximatelyEqual' functions taking a relative tolerance
        //   and using the default absolute tolerance (for both the 'double'
        //   and 'float' position specifications).
        //
        //   Construct a pair of identical lists and a pair of identical
        //   tables.  In a sequence of blocks, for each pair of objects, vary
        //   exactly one corresponding pair of 'float' or 'double' values:
        //      (1) in one FLOAT element
        //      (2) in one DOUBLE element
        //      (3) in one position in one FLOATARRAY element
        //      (4) in one position in one DOUBLEARRAY element
        //      (5) in one FLOAT element of one (contained) LIST element
        //      (6) in one DOUBLE element of one (contained) LIST element
        //      (7) in one FLOAT element of one (contained) TABLE element
        //      (8) in one DOUBLE element of one (contained) TABLE element
        //
        // Testing:
        //   bool areElemRefsApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel);
        //   bool areRowsApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel);
        //   bool areListsApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel);
        //   bool areTablesApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel);
        //   bool areChoicesApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel);
        //   bool areChoiceArraysApproximatelyEqual(lhs, rhs, dRel, dAbs,
        //                                                               fRel);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "TESTING 'areXXXApproximatelyEqual' w/o FLOAT ABS TOLERANCE"
            << endl
            << "=========================================================="
            << endl;

        if (verbose) cout << "\nTesting 'areXXXApproximatelyEqual' "
                             "without float absolute tolerance input." << endl;

        // Variables and data common to all four tests in this case:

        static const struct {       // Test input/expected output
            int    d_line;          //   line
            float  d_a;             //   rhs value to vary in tests
            float  d_b;             //   lhs value to vary in tests
            double d_rel;           //   relative tolerance
            double d_abs;           //   absolute tolerance
            int    d_eq;            //   expected return value
        } DATA[] = {
            // line  d_a        d_b         rel      abs       Expected
            // ---- --------   --------    ------   ------    --------
            {  L_,  1.0     ,  1.0     ,   1e-6,    1e-3,     1       },
            {  L_,  1.0     ,  0.9995  ,   1e-6,    1e-3,     1       },
            {  L_,  1.0     ,  0.995   ,   1e-6,    1e-3,     0       },
            {  L_,  0.9995  ,  1.0     ,   1e-6,    1e-3,     1       },
            {  L_,  0.995   ,  1.0     ,   1e-6,    1e-3,     0       },
            {  L_,  1.0     ,  0.9995  ,   1e-3,    0   ,     1       },
            {  L_,  1.0     ,  0.995   ,   1e-3,    0   ,     0       },
            {  L_,  0.9995  ,  1.0     ,   1e-3,    0   ,     1       },
            {  L_,  0.995   ,  1.0     ,   1e-3,    0   ,     0       }
        };
        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        // Create the pairs of test aggregates used as arguments to the
        // 'areApproximatelyEqual' methods during testing.

        List  mL1(TEST_LIST);   const List&  L1 = mL1;
        List  mL2(L1);          const List&  L2 = mL2;
        Table mT1(TEST_TABLE);  const Table& T1 = mT1;
        Table mT2(T1);          const Table& T2 = mT2;

        int i;

        if (verbose) cout << "\tVarying only a FLOAT." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const int    LINE = DATA[i].d_line;
            const float  FA   = DATA[i].d_a;
            const float  FB   = DATA[i].d_b;
            const double REL  = DATA[i].d_rel;
            const double ABS  = DATA[i].d_abs;
            const bool   RC   = DATA[i].d_eq;

            mL1.theModifiableFloat(4)                       = FA;
            mL2.theModifiableFloat(4)                       = FB;
            mT1.theModifiableRow(1)[4].theModifiableFloat() = FA;
            mT2.theModifiableRow(1)[4].theModifiableFloat() = FB;

            LOOP_ASSERT(LINE, RC == rApprox5(R1, R2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == lApprox5(L1, L2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == elApprox5(L1, L2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == tApprox5(T1, T2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == etApprox5(T1, T2, 0, ABS, REL));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(LINE, RC == caApprox5(CA1, CA2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == ecaApprox5(CA1, CA2, 0, ABS, REL));

            Choice mC1(mCA1[4]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[4]);  const Choice& C2 = mC2;

            LOOP_ASSERT(LINE, RC == cApprox5(C1, C2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == ecApprox5(C1, C2, 0, ABS, REL));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a DOUBLE." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const int    LINE = DATA[i].d_line;
            const float  FA   = DATA[i].d_a;
            const float  FB   = DATA[i].d_b;
            const double REL  = DATA[i].d_rel;
            const double ABS  = DATA[i].d_abs;
            const bool   RC   = DATA[i].d_eq;

            mL1.theModifiableDouble(5)                       = FA;
            mL2.theModifiableDouble(5)                       = FB;
            mT1.theModifiableRow(1)[5].theModifiableDouble() = FA;
            mT2.theModifiableRow(1)[5].theModifiableDouble() = FB;

            LOOP_ASSERT(LINE, RC == rApprox5(R1, R2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == lApprox5(L1, L2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == elApprox5(L1, L2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == tApprox5(T1, T2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == etApprox5(T1, T2, REL, ABS, 0));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(LINE, RC == caApprox5(CA1, CA2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == ecaApprox5(CA1, CA2, REL, ABS, 0));

            Choice mC1(mCA1[5]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[5]);  const Choice& C2 = mC2;

            LOOP_ASSERT(LINE, RC == cApprox5(C1, C2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == ecApprox5(C1, C2, REL, ABS, 0));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a FLOATARRAY." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const int    LINE = DATA[i].d_line;
            const float  FA   = DATA[i].d_a;
            const float  FB   = DATA[i].d_b;
            const double REL  = DATA[i].d_rel;
            const double ABS  = DATA[i].d_abs;
            const bool   RC   = DATA[i].d_eq;

            mL1.theModifiableFloatArray(14)[1]      = FA;
            mL2.theModifiableFloatArray(14)[1]      = FB;
            mT1.theModifiableRow(1)[14].theModifiableFloatArray()[1]
                                                    = FA;
            mT2.theModifiableRow(1)[14].theModifiableFloatArray()[1]
                                                    = FB;

            LOOP_ASSERT(LINE, RC == rApprox5(R1, R2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == lApprox5(L1, L2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == elApprox5(L1, L2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == tApprox5(T1, T2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == etApprox5(T1, T2, 0, ABS, REL));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(LINE, RC == caApprox5(CA1, CA2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == ecaApprox5(CA1, CA2, 0, ABS, REL));

            Choice mC1(mCA1[14]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[14]);  const Choice& C2 = mC2;

            LOOP_ASSERT(LINE, RC == cApprox5(C1, C2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == ecApprox5(C1, C2, 0, ABS, REL));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a DOUBLEARRAY." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const int    LINE = DATA[i].d_line;
            const float  FA   = DATA[i].d_a;
            const float  FB   = DATA[i].d_b;
            const double REL  = DATA[i].d_rel;
            const double ABS  = DATA[i].d_abs;
            const bool   RC   = DATA[i].d_eq;

            mL1.theModifiableDoubleArray(15)[1]      = FA;
            mL2.theModifiableDoubleArray(15)[1]      = FB;
            mT1.theModifiableRow(1)[15].theModifiableDoubleArray()[1]
                                                     = FA;
            mT2.theModifiableRow(1)[15].theModifiableDoubleArray()[1]
                                                     = FB;

            LOOP_ASSERT(LINE, RC == rApprox5(R1, R2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == lApprox5(L1, L2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == elApprox5(L1, L2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == tApprox5(T1, T2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == etApprox5(T1, T2, REL, ABS, 0));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(LINE, RC == caApprox5(CA1, CA2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == ecaApprox5(CA1, CA2, REL, ABS, 0));

            Choice mC1(mCA1[15]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[15]);  const Choice& C2 = mC2;

            LOOP_ASSERT(LINE, RC == cApprox5(C1, C2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == ecApprox5(C1, C2, REL, ABS, 0));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a FLOAT in a sub-LIST." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const int    LINE = DATA[i].d_line;
            const float  FA   = DATA[i].d_a;
            const float  FB   = DATA[i].d_b;
            const double REL  = DATA[i].d_rel;
            const double ABS  = DATA[i].d_abs;
            const bool   RC   = DATA[i].d_eq;

            mL1.theModifiableList(20).theModifiableFloat(0)    = FA;
            mL2.theModifiableList(20).theModifiableFloat(0)    = FB;
            mT1.theModifiableRow(1)[20].theModifiableList().
                                         theModifiableFloat(0) = FA;
            mT2.theModifiableRow(1)[20].theModifiableList().
                                         theModifiableFloat(0) = FB;

            LOOP_ASSERT(LINE, RC == rApprox5(R1, R2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == lApprox5(L1, L2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == elApprox5(L1, L2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == tApprox5(T1, T2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == etApprox5(T1, T2, 0, ABS, REL));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(LINE, RC == caApprox5(CA1, CA2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == ecaApprox5(CA1, CA2, 0, ABS, REL));

            Choice mC1(mCA1[20]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[20]);  const Choice& C2 = mC2;

            LOOP_ASSERT(LINE, RC == cApprox5(C1, C2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == ecApprox5(C1, C2, 0, ABS, REL));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a DOUBLE in a sub-LIST." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const int    LINE = DATA[i].d_line;
            const float  FA   = DATA[i].d_a;
            const float  FB   = DATA[i].d_b;
            const double REL  = DATA[i].d_rel;
            const double ABS  = DATA[i].d_abs;
            const bool   RC   = DATA[i].d_eq;

            mL1.theModifiableList(20).theModifiableDouble(1) = FA;
            mL2.theModifiableList(20).theModifiableDouble(1) = FB;
            mT1.theModifiableRow(1)[20].theModifiableList().
                                      theModifiableDouble(1) = FA;
            mT2.theModifiableRow(1)[20].theModifiableList().
                                      theModifiableDouble(1) = FB;

            LOOP_ASSERT(LINE, RC == rApprox5(R1, R2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == lApprox5(L1, L2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == elApprox5(L1, L2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == tApprox5(T1, T2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == etApprox5(T1, T2, REL, ABS, 0));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(LINE, RC == caApprox5(CA1, CA2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == ecaApprox5(CA1, CA2, REL, ABS, 0));

            Choice mC1(mCA1[20]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[20]);  const Choice& C2 = mC2;

            LOOP_ASSERT(LINE, RC == cApprox5(C1, C2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == ecApprox5(C1, C2, REL, ABS, 0));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a FLOAT in a sub-TABLE." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const int    LINE = DATA[i].d_line;
            const float  FA   = DATA[i].d_a;
            const float  FB   = DATA[i].d_b;
            const double REL  = DATA[i].d_rel;
            const double ABS  = DATA[i].d_abs;
            const bool   RC   = DATA[i].d_eq;

            mL1.theModifiableTable(21).theModifiableRow(0)[0].
                                          theModifiableFloat() = FA;
            mL2.theModifiableTable(21).theModifiableRow(0)[0].
                                          theModifiableFloat() = FB;
            mT1.theModifiableRow(1)[21].theModifiableTable().
                   theModifiableRow(0)[0].theModifiableFloat() = FA;
            mT2.theModifiableRow(1)[21].theModifiableTable().
                   theModifiableRow(0)[0].theModifiableFloat() = FB;

            LOOP_ASSERT(LINE, RC == rApprox5(R1, R2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == lApprox5(L1, L2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == elApprox5(L1, L2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == tApprox5(T1, T2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == etApprox5(T1, T2, 0, ABS, REL));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(LINE, RC == caApprox5(CA1, CA2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == ecaApprox5(CA1, CA2, 0, ABS, REL));

            Choice mC1(mCA1[21]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[21]);  const Choice& C2 = mC2;

            LOOP_ASSERT(LINE, RC == cApprox5(C1, C2, 0, ABS, REL));
            LOOP_ASSERT(LINE, RC == ecApprox5(C1, C2, 0, ABS, REL));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a DOUBLE in a sub-TABLE." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const int    LINE = DATA[i].d_line;
            const float  FA   = DATA[i].d_a;
            const float  FB   = DATA[i].d_b;
            const double REL  = DATA[i].d_rel;
            const double ABS  = DATA[i].d_abs;
            const bool   RC   = DATA[i].d_eq;

            mL1.theModifiableTable(21).theModifiableRow(0)[1].
                                         theModifiableDouble() = FA;
            mL2.theModifiableTable(21).theModifiableRow(0)[1].
                                         theModifiableDouble() = FB;
            mT1.theModifiableRow(1)[21].theModifiableTable().
                  theModifiableRow(0)[1].theModifiableDouble() = FA;
            mT2.theModifiableRow(1)[21].theModifiableTable().
                  theModifiableRow(0)[1].theModifiableDouble() = FB;

            LOOP_ASSERT(LINE, RC == rApprox5(R1, R2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == lApprox5(L1, L2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == elApprox5(L1, L2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == tApprox5(T1, T2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == etApprox5(T1, T2, REL, ABS, 0));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(LINE, RC == caApprox5(CA1, CA2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == ecaApprox5(CA1, CA2, REL, ABS, 0));

            Choice mC1(mCA1[21]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[21]);  const Choice& C2 = mC2;

            LOOP_ASSERT(LINE, RC == cApprox5(C1, C2, REL, ABS, 0));
            LOOP_ASSERT(LINE, RC == ecApprox5(C1, C2, REL, ABS, 0));
        }
      }  break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING APPROXIMATE-EQUALITY FUNCTIONS
        // Concerns:
        //   The main underlying functionality has already been tested in cases
        //   2 and 3.  Our primary concern here is that the
        //   'areXXXApproximatelyEqual' functions correctly forward their
        //   arguments when non-default values are used for both relative
        //   tolerances and absolute tolerances.
        //
        // Plan:
        //   In a table-driven test, confirm the correct operation of the
        //   'areXXXApproximatelyEqual' functions taking a relative tolerance
        //   and an absolute tolerance (for both the 'double' and the 'float'
        //   position specifications).
        //
        //   Construct a pair of identical lists and a pair of identical
        //   tables.  In a sequence of blocks, for each pair of objects, vary
        //   exactly one corresponding pair of 'float' or 'double' values:
        //      (1) in one FLOAT element
        //      (2) in one DOUBLE element
        //      (3) in one position in one FLOATARRAY element
        //      (4) in one position in one DOUBLEARRAY element
        //      (5) in one FLOAT element of one (contained) LIST element
        //      (6) in one DOUBLE element of one (contained) LIST element
        //      (7) in one FLOAT element of one (contained) TABLE element
        //      (8) in one DOUBLE element of one (contained) TABLE element
        //
        // Testing:
        //   bool areElemRefsApproximatelyEqual(lhs, rhs, dRel, dAbs);
        //   bool areElemRefsApproximatelyEqual(lhs, rhs, dRel,
        //                                      dAbs, fRel, fAbs);
        //
        //   bool areRowsApproximatelyEqual(lhs, rhs, dRel, dAbs);
        //   bool areRowsApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel, fAbs);
        //
        //   bool areListsApproximatelyEqual(lhs, rhs, dRel, dAbs);
        //   bool areListsApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel, fAbs);
        //
        //   bool areTablesApproximatelyEqual(lhs, rhs, dRel, dAbs);
        //   bool areTablesApproximatelyEqual(lhs, rhs, dRel, dAbs, fRel,
        //                                                               fAbs);
        //
        //   bool areChoicesApproximatelyEqual(lhs, rhs, dRel, dAbs);
        //   bool areChoicesApproximatelyEqual(lhs, rhs, dRel, dAbs,
        //                                                         fRel, fAbs);
        //
        //   bool areChoiceArraysApproximatelyEqual(lhs, rhs, dRel, dAbs);
        //   bool areChoiceArraysApproximatelyEqual(lhs, rhs, dRel, dAbs,
        //                                                         fRel, fAbs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "TESTING 'areXXXApproximatelyEqual' w/INPUT TOLERANCES" << endl
            << "=====================================================" << endl;

        if (verbose) cout << endl
            << "Testing 'areXXXApproximatelyEqual' w/ both tolerances input."
            << endl;

        // Variables and data common to all four tests in this case:

        static const struct {       // Test input/expected output
            float  d_a;             //   rhs value to vary in tests
            float  d_b;             //   lhs value to vary in tests
            double d_rel;           //   relative tolerance
            double d_abs;           //   absolute tolerance
            int    d_eq;            //   expected return value
        } DATA[] = {
            // d_a        d_b         rel      abs       Expected
            // --------   --------    ------   ------    --------
            {  1.0f    ,  1.0f    ,   1e-6,    1e-3,     1       },
            {  1.0f    ,  0.9995f ,   1e-6,    1e-3,     1       },
            {  1.0f    ,  0.995f  ,   1e-6,    1e-3,     0       },
            {  0.9995f ,  1.0f    ,   1e-6,    1e-3,     1       },
            {  0.995f  ,  1.0f    ,   1e-6,    1e-3,     0       },
            {  1.0f    ,  0.9995f ,   1e-3,    0   ,     1       },
            {  1.0f    ,  0.995f  ,   1e-3,    0   ,     0       },
            {  0.9995f ,  1.0f    ,   1e-3,    0   ,     1       },
            {  0.995f  ,  1.0f    ,   1e-3,    0   ,     0       }
        };
        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        // Create the pairs of test aggregates used as arguments to the
        // 'areApproximatelyEqual' methods during testing.

        List  mL1(TEST_LIST);   const List&  L1 = mL1;
        List  mL2(L1);          const List&  L2 = mL2;
        Table mT1(TEST_TABLE);  const Table& T1 = mT1;
        Table mT2(T1);          const Table& T2 = mT2;

        int i;

        if (verbose) cout << "\tVarying only a FLOAT." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const float  FA  = DATA[i].d_a;
            const float  FB  = DATA[i].d_b;
            const double REL = DATA[i].d_rel;
            const double ABS = DATA[i].d_abs;
            const bool   RC  = DATA[i].d_eq;

            mL1.theModifiableFloat(4)      = FA;
            mL2.theModifiableFloat(4)      = FB;
            mT1.theModifiableRow(1)[4].theModifiableFloat() = FA;
            mT2.theModifiableRow(1)[4].theModifiableFloat() = FB;

            LOOP_ASSERT(i, RC == rApprox6(R1, R2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == lApprox6(L1, L2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == elApprox6(L1, L2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == tApprox6(T1, T2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == etApprox6(T1, T2, 0, 0, REL, ABS));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(i, RC == caApprox6(CA1, CA2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == ecaApprox6(CA1, CA2, 0, 0, REL, ABS));

            Choice mC1(mCA1[4]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[4]);  const Choice& C2 = mC2;

            LOOP_ASSERT(i, RC == cApprox6(C1, C2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == ecApprox6(C1, C2, 0, 0, REL, ABS));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a DOUBLE." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const float  FA  = DATA[i].d_a;
            const float  FB  = DATA[i].d_b;
            const double REL = DATA[i].d_rel;
            const double ABS = DATA[i].d_abs;
            const bool   RC  = DATA[i].d_eq;

            mL1.theModifiableDouble(5)      = FA;
            mL2.theModifiableDouble(5)      = FB;
            mT1.theModifiableRow(1)[5].theModifiableDouble() = FA;
            mT2.theModifiableRow(1)[5].theModifiableDouble() = FB;

            LOOP_ASSERT(i, RC == rApprox4(R1, R2, REL, ABS));
            LOOP_ASSERT(i, RC == lApprox4(L1, L2, REL, ABS));
            LOOP_ASSERT(i, RC == elApprox4(L1, L2, REL, ABS));
            LOOP_ASSERT(i, RC == tApprox4(T1, T2, REL, ABS));
            LOOP_ASSERT(i, RC == etApprox4(T1, T2, REL, ABS));

            LOOP_ASSERT(i, RC == rApprox6(R1, R2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == lApprox6(L1, L2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == elApprox6(L1, L2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == tApprox6(T1, T2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == etApprox6(T1, T2, REL, ABS, 0, 0));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(i, RC == caApprox4(CA1, CA2, REL, ABS));
            LOOP_ASSERT(i, RC == ecaApprox4(CA1, CA2, REL, ABS));
            LOOP_ASSERT(i, RC == caApprox6(CA1, CA2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == ecaApprox6(CA1, CA2, REL, ABS, 0, 0));

            Choice mC1(mCA1[5]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[5]);  const Choice& C2 = mC2;

            LOOP_ASSERT(i, RC == cApprox4(C1, C2, REL, ABS));
            LOOP_ASSERT(i, RC == ecApprox4(C1, C2, REL, ABS));
            LOOP_ASSERT(i, RC == cApprox6(C1, C2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == ecApprox6(C1, C2, REL, ABS, 0, 0));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a FLOATARRAY." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const float  FA  = DATA[i].d_a;
            const float  FB  = DATA[i].d_b;
            const double REL = DATA[i].d_rel;
            const double ABS = DATA[i].d_abs;
            const bool   RC  = DATA[i].d_eq;

            mL1.theModifiableFloatArray(14)[1]      = FA;
            mL2.theModifiableFloatArray(14)[1]      = FB;
            mT1.theModifiableRow(1)[14].theModifiableFloatArray()[1]
                                                    = FA;
            mT2.theModifiableRow(1)[14].theModifiableFloatArray()[1]
                                                    = FB;

            LOOP_ASSERT(i, RC == rApprox6(R1, R2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == lApprox6(L1, L2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == elApprox6(L1, L2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == tApprox6(T1, T2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == etApprox6(T1, T2, 0, 0, REL, ABS));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(i, RC == caApprox6(CA1, CA2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == ecaApprox6(CA1, CA2, 0, 0, REL, ABS));

            Choice mC1(mCA1[14]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[14]);  const Choice& C2 = mC2;

            LOOP_ASSERT(i, RC == cApprox6(C1, C2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == ecApprox6(C1, C2, 0, 0, REL, ABS));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a DOUBLEARRAY." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const float  FA  = DATA[i].d_a;
            const float  FB  = DATA[i].d_b;
            const double REL = DATA[i].d_rel;
            const double ABS = DATA[i].d_abs;
            const bool   RC  = DATA[i].d_eq;

            mL1.theModifiableDoubleArray(15)[1] = FA;
            mL2.theModifiableDoubleArray(15)[1] = FB;
            mT1.theModifiableRow(1)[15].
                  theModifiableDoubleArray()[1] = FA;
            mT2.theModifiableRow(1)[15].
                  theModifiableDoubleArray()[1] = FB;

            LOOP_ASSERT(i, RC == rApprox4(R1, R2, REL, ABS));
            LOOP_ASSERT(i, RC == lApprox4(L1, L2, REL, ABS));
            LOOP_ASSERT(i, RC == elApprox4(L1, L2, REL, ABS));
            LOOP_ASSERT(i, RC == tApprox4(T1, T2, REL, ABS));
            LOOP_ASSERT(i, RC == etApprox4(T1, T2, REL, ABS));

            LOOP_ASSERT(i, RC == rApprox6(R1, R2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == lApprox6(L1, L2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == elApprox6(L1, L2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == tApprox6(T1, T2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == etApprox6(T1, T2, REL, ABS, 0, 0));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(i, RC == caApprox4(CA1, CA2, REL, ABS));
            LOOP_ASSERT(i, RC == ecaApprox4(CA1, CA2, REL, ABS));
            LOOP_ASSERT(i, RC == caApprox6(CA1, CA2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == ecaApprox6(CA1, CA2, REL, ABS, 0, 0));

            Choice mC1(mCA1[15]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[15]);  const Choice& C2 = mC2;

            LOOP_ASSERT(i, RC == cApprox4(C1, C2, REL, ABS));
            LOOP_ASSERT(i, RC == ecApprox4(C1, C2, REL, ABS));
            LOOP_ASSERT(i, RC == cApprox6(C1, C2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == ecApprox6(C1, C2, REL, ABS, 0, 0));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a FLOAT in a sub-LIST." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const float  FA  = DATA[i].d_a;
            const float  FB  = DATA[i].d_b;
            const double REL = DATA[i].d_rel;
            const double ABS = DATA[i].d_abs;
            const bool   RC  = DATA[i].d_eq;

            mL1.theModifiableList(20).theModifiableFloat(0) = FA;
            mL2.theModifiableList(20).theModifiableFloat(0) = FB;
            mT1.theModifiableRow(1)[20].theModifiableList().
                                      theModifiableFloat(0) = FA;
            mT2.theModifiableRow(1)[20].theModifiableList().
                                      theModifiableFloat(0) = FB;

            LOOP_ASSERT(i, RC == rApprox6(R1, R2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == lApprox6(L1, L2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == elApprox6(L1, L2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == tApprox6(T1, T2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == etApprox6(T1, T2, 0, 0, REL, ABS));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(i, RC == caApprox6(CA1, CA2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == ecaApprox6(CA1, CA2, 0, 0, REL, ABS));

            Choice mC1(mCA1[20]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[20]);  const Choice& C2 = mC2;

            LOOP_ASSERT(i, RC == cApprox6(C1, C2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == ecApprox6(C1, C2, 0, 0, REL, ABS));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a DOUBLE in a sub-LIST." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const float  FA  = DATA[i].d_a;
            const float  FB  = DATA[i].d_b;
            const double REL = DATA[i].d_rel;
            const double ABS = DATA[i].d_abs;
            const bool   RC  = DATA[i].d_eq;

            mL1.theModifiableList(20).theModifiableDouble(1)   = FA;
            mL2.theModifiableList(20).theModifiableDouble(1)   = FB;
            mT1.theModifiableRow(1)[20].theModifiableList().
                                        theModifiableDouble(1) = FA;
            mT2.theModifiableRow(1)[20].theModifiableList().
                                        theModifiableDouble(1) = FB;

            LOOP_ASSERT(i, RC == rApprox4(R1, R2, REL, ABS));
            LOOP_ASSERT(i, RC == lApprox4(L1, L2, REL, ABS));
            LOOP_ASSERT(i, RC == elApprox4(L1, L2, REL, ABS));
            LOOP_ASSERT(i, RC == tApprox4(T1, T2, REL, ABS));
            LOOP_ASSERT(i, RC == etApprox4(T1, T2, REL, ABS));

            LOOP_ASSERT(i, RC == rApprox6(R1, R2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == lApprox6(L1, L2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == elApprox6(L1, L2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == tApprox6(T1, T2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == etApprox6(T1, T2, REL, ABS, 0, 0));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(i, RC == caApprox4(CA1, CA2, REL, ABS));
            LOOP_ASSERT(i, RC == ecaApprox4(CA1, CA2, REL, ABS));
            LOOP_ASSERT(i, RC == caApprox6(CA1, CA2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == ecaApprox6(CA1, CA2, REL, ABS, 0, 0));

            Choice mC1(mCA1[20]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[20]);  const Choice& C2 = mC2;

            LOOP_ASSERT(i, RC == cApprox4(C1, C2, REL, ABS));
            LOOP_ASSERT(i, RC == ecApprox4(C1, C2, REL, ABS));
            LOOP_ASSERT(i, RC == cApprox6(C1, C2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == ecApprox6(C1, C2, REL, ABS, 0, 0));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a FLOAT in a sub-TABLE." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const float  FA  = DATA[i].d_a;
            const float  FB  = DATA[i].d_b;
            const double REL = DATA[i].d_rel;
            const double ABS = DATA[i].d_abs;
            const bool   RC  = DATA[i].d_eq;

            mL1.theModifiableTable(21).theModifiableRow(0)[0].
                                          theModifiableFloat() = FA;
            mL2.theModifiableTable(21).theModifiableRow(0)[0].
                                          theModifiableFloat() = FB;
            mT1.theModifiableRow(1)[21].theModifiableTable().
                   theModifiableRow(0)[0].theModifiableFloat() = FA;
            mT2.theModifiableRow(1)[21].theModifiableTable().
                   theModifiableRow(0)[0].theModifiableFloat() = FB;

            LOOP_ASSERT(i, RC == rApprox6(R1, R2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == lApprox6(L1, L2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == elApprox6(L1, L2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == tApprox6(T1, T2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == etApprox6(T1, T2, 0, 0, REL, ABS));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(i, RC == caApprox6(CA1, CA2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == ecaApprox6(CA1, CA2, 0, 0, REL, ABS));

            Choice mC1(mCA1[21]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[21]);  const Choice& C2 = mC2;

            LOOP_ASSERT(i, RC == cApprox6(C1, C2, 0, 0, REL, ABS));
            LOOP_ASSERT(i, RC == ecApprox6(C1, C2, 0, 0, REL, ABS));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a DOUBLE in a sub-TABLE." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const float  FA  = DATA[i].d_a;
            const float  FB  = DATA[i].d_b;
            const double REL = DATA[i].d_rel;
            const double ABS = DATA[i].d_abs;
            const bool   RC  = DATA[i].d_eq;

            mL1.theModifiableTable(21).theModifiableRow(0)[1].
                                           theModifiableDouble() = FA;
            mL2.theModifiableTable(21).theModifiableRow(0)[1].
                                           theModifiableDouble() = FB;
            mT1.theModifiableRow(1)[21].theModifiableTable().
                                 theModifiableRow(0)[1].theModifiableDouble() =
                                                                   FA;
            mT2.theModifiableRow(1)[21].theModifiableTable().
                                 theModifiableRow(0)[1].theModifiableDouble() =
                                                                   FB;

            LOOP_ASSERT(i, RC == rApprox4(R1, R2, REL, ABS));
            LOOP_ASSERT(i, RC == lApprox4(L1, L2, REL, ABS));
            LOOP_ASSERT(i, RC == elApprox4(L1, L2, REL, ABS));
            LOOP_ASSERT(i, RC == tApprox4(T1, T2, REL, ABS));
            LOOP_ASSERT(i, RC == etApprox4(T1, T2, REL, ABS));

            LOOP_ASSERT(i, RC == rApprox6(R1, R2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == lApprox6(L1, L2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == tApprox6(T1, T2, REL, ABS, 0, 0));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(i, RC == caApprox4(CA1, CA2, REL, ABS));
            LOOP_ASSERT(i, RC == ecaApprox4(CA1, CA2, REL, ABS));
            LOOP_ASSERT(i, RC == caApprox6(CA1, CA2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == ecaApprox6(CA1, CA2, REL, ABS, 0, 0));

            Choice mC1(mCA1[21]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[21]);  const Choice& C2 = mC2;

            LOOP_ASSERT(i, RC == cApprox4(C1, C2, REL, ABS));
            LOOP_ASSERT(i, RC == ecApprox4(C1, C2, REL, ABS));
            LOOP_ASSERT(i, RC == cApprox6(C1, C2, REL, ABS, 0, 0));
            LOOP_ASSERT(i, RC == ecApprox6(C1, C2, REL, ABS, 0, 0));
        }
      }  break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING APPROXIMATE-EQUALITY FUNCTIONS FOR AGGREGATES
        // Concerns:
        //   The main underlying functionality has already been tested in cases
        //   2 and 3.  Our primary concern here is that the
        //   'areXXXApproximatelyEqual' functions correctly forward their
        //   arguments when non-default values are used for relative
        //   tolerances and default values are used for absolute tolerances.
        //
        // Plan:
        //   In a table-driven test, confirm the correct operation of the
        //   'areXXXApproximatelyEqual' functions taking a relative tolerance
        //   and using the default absolute tolerance (for both the 'double'
        //   and 'float' position specifications).
        //
        //   Construct a pair of identical lists and a pair of identical
        //   tables.  In a sequence of blocks, for each pair of objects, vary
        //   exactly one corresponding pair of 'float' or 'double' values:
        //      (1) in one FLOAT element
        //      (2) in one DOUBLE element
        //      (3) in one position in one FLOATARRAY element
        //      (4) in one position in one DOUBLEARRAY element
        //      (5) in one FLOAT element of one (contained) LIST element
        //      (6) in one DOUBLE element of one (contained) LIST element
        //      (7) in one FLOAT element of one (contained) TABLE element
        //      (8) in one DOUBLE element of one (contained) TABLE element
        //
        // Testing:
        //   bool areElemRefsApproximatelyEqual(lhs, rhs, dRel);
        //   bool areRowsApproximatelyEqual(lhs, rhs, dRel);
        //   bool areListsApproximatelyEqual(lhs, rhs, dRel);
        //   bool areTablesApproximatelyEqual(lhs, rhs, dRel);
        //   bool areChoicesApproximatelyEqual(lhs, rhs, dRel);
        //   bool areChoiceArraysApproximatelyEqual(lhs, rhs, dRel);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "TESTING 'areXXXApproximatelyEqual' w/RELATIVE TOLERANCE"
            << endl
            << "======================================================="
            << endl;

        if (verbose) cout << "\nTesting 'areXXXApproximatelyEqual' "
                             "with relative tolerance input." << endl;

        // Variables and data common to all four tests in this case:

        double rel;                 // temporary to hold relative tolerance

        static const struct {       // Test input/expected output
            int    d_line;          //   line number
            float  d_a;             //   rhs value to vary in tests
            float  d_b;             //   lhs value to vary in tests
            double d_rel;           //   relative tolerance
            int    d_eq;            //   expected return value
        } DATA[] = {
            // line     d_a        d_b         rel       Expected
            // -----   --------   --------    ------    --------
            {  L_,    1.0f    ,  1.0f    ,   1e-3,     1       },
            {  L_,    1.0f    ,  0.9999f ,   1e-3,     1       },
            {  L_,    1.0f    ,  0.9995f ,   1e-3,     1       },
            {  L_,    1.0f    ,  0.995f  ,   1e-3,     0       },
            {  L_,    1.0f    ,  0.99f   ,   1e-3,     0       },
            {  L_,    1.0f    ,  1.0f    ,   0   ,     1       },
            {  L_,    0.9f    ,  1.0f    ,   0   ,     0       },

            {  L_,    1e-25f  ,  1e-26f  ,   0   ,     1       },
            {  L_,    1e-24f  ,  1e-25f  ,   0   ,     1       },
            {  L_,    1e-23f  ,  1e-24f  ,   0   ,     0       }
        };
        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        // Create the pairs of test aggregates used as arguments to the
        // 'areApproximatelyEqual' methods during testing.

        List  mL1(TEST_LIST);   const List&  L1 = mL1;
        List  mL2(L1);          const List&  L2 = mL2;
        Table mT1(TEST_TABLE);  const Table& T1 = mT1;
        Table mT2(T1);          const Table& T2 = mT2;

        int i;

        if (verbose) cout << "\tVarying only a DOUBLE." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const int    LINE = DATA[i].d_line;
            const float  FA   = DATA[i].d_a;
            const float  FB   = DATA[i].d_b;
            const double REL  = DATA[i].d_rel;
            const bool   RC   = DATA[i].d_eq;

            mL1.theModifiableDouble(5)                       = FA;
            mL2.theModifiableDouble(5)                       = FB;
            mT1.theModifiableRow(1)[5].theModifiableDouble() = FA;
            mT2.theModifiableRow(1)[5].theModifiableDouble() = FB;

            LOOP_ASSERT(LINE, RC == rApprox3(R1, R2, REL));
            LOOP_ASSERT(LINE, RC == lApprox3(L1, L2, REL));
            LOOP_ASSERT(LINE, RC == elApprox3(L1, L2, REL));
            LOOP_ASSERT(LINE, RC == tApprox3(T1, T2, REL));
            LOOP_ASSERT(LINE, RC == etApprox3(T1, T2, REL));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(LINE, RC == caApprox3(CA1, CA2, REL));
            LOOP_ASSERT(LINE, RC == ecaApprox3(CA1, CA2, REL));

            Choice mC1(mCA1[5]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[5]);  const Choice& C2 = mC2;

            LOOP_ASSERT(LINE, RC == cApprox3(C1, C2, REL));
            LOOP_ASSERT(LINE, RC == ecApprox3(C1, C2, REL));
            LOOP_ASSERT(LINE, RC == cApprox5(C1, C2, REL, ABSDEF, 0));
            LOOP_ASSERT(LINE, RC == ecApprox5(C1, C2, REL, ABSDEF, 0));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a DOUBLEARRAY." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const int    LINE = DATA[i].d_line;
            const float  FA   = DATA[i].d_a;
            const float  FB   = DATA[i].d_b;
            const double REL  = DATA[i].d_rel;
            const bool   RC   = DATA[i].d_eq;

            mL1.theModifiableDoubleArray(15)[1]      = FA;
            mL2.theModifiableDoubleArray(15)[1]      = FB;
            mT1.theModifiableRow(1)[15].theModifiableDoubleArray()[1]
                                                     = FA;
            mT2.theModifiableRow(1)[15].theModifiableDoubleArray()[1]
                                                     = FB;

            LOOP_ASSERT(LINE, RC == rApprox3(R1, R2, REL));
            LOOP_ASSERT(LINE, RC == lApprox3(L1, L2, REL));
            LOOP_ASSERT(LINE, RC == elApprox3(L1, L2, REL));
            LOOP_ASSERT(LINE, RC == tApprox3(T1, T2, REL));
            LOOP_ASSERT(LINE, RC == etApprox3(T1, T2, REL));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(LINE, RC == caApprox3(CA1, CA2, REL));
            LOOP_ASSERT(LINE, RC == ecaApprox3(CA1, CA2, REL));

            Choice mC1(mCA1[15]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[15]);  const Choice& C2 = mC2;

            LOOP_ASSERT(LINE, RC == cApprox3(C1, C2, REL));
            LOOP_ASSERT(LINE, RC == ecApprox3(C1, C2, REL));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a DOUBLE in a sub-LIST." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const int    LINE = DATA[i].d_line;
            const float  FA   = DATA[i].d_a;
            const float  FB   = DATA[i].d_b;
            const double REL  = DATA[i].d_rel;
            const bool   RC   = DATA[i].d_eq;

            mL1.theModifiableList(20).theModifiableDouble(1) = FA;
            mL2.theModifiableList(20).theModifiableDouble(1) = FB;
            mT1.theModifiableRow(1)[20].theModifiableList().
                                      theModifiableDouble(1) = FA;
            mT2.theModifiableRow(1)[20].theModifiableList().
                                      theModifiableDouble(1) = FB;

            LOOP_ASSERT(LINE, RC == rApprox3(R1, R2, REL));
            LOOP_ASSERT(LINE, RC == lApprox3(L1, L2, REL));
            LOOP_ASSERT(LINE, RC == elApprox3(L1, L2, REL));
            LOOP_ASSERT(LINE, RC == tApprox3(T1, T2, REL));
            LOOP_ASSERT(LINE, RC == etApprox3(T1, T2, REL));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(LINE, RC == caApprox3(CA1, CA2, REL));
            LOOP_ASSERT(LINE, RC == ecaApprox3(CA1, CA2, REL));

            Choice mC1(mCA1[20]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[20]);  const Choice& C2 = mC2;

            LOOP_ASSERT(LINE, RC == cApprox3(C1, C2, REL));
            LOOP_ASSERT(LINE, RC == ecApprox3(C1, C2, REL));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a DOUBLE in a sub-TABLE." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const int    LINE = DATA[i].d_line;
            const float  FA   = DATA[i].d_a;
            const float  FB   = DATA[i].d_b;
            const double REL  = DATA[i].d_rel;
            const bool   RC   = DATA[i].d_eq;

            mL1.theModifiableTable(21).theModifiableRow(0)[1].
                                         theModifiableDouble() = FA;
            mL2.theModifiableTable(21).theModifiableRow(0)[1].
                                         theModifiableDouble() = FB;
            mT1.theModifiableRow(1)[21].theModifiableTable().
                  theModifiableRow(0)[1].theModifiableDouble() = FA;
            mT2.theModifiableRow(1)[21].theModifiableTable().
                  theModifiableRow(0)[1].theModifiableDouble() = FB;

            LOOP_ASSERT(LINE, RC == rApprox3(R1, R2, REL));
            LOOP_ASSERT(LINE, RC == lApprox3(L1, L2, REL));
            LOOP_ASSERT(LINE, RC == elApprox3(L1, L2, REL));
            LOOP_ASSERT(LINE, RC == tApprox3(T1, T2, REL));
            LOOP_ASSERT(LINE, RC == etApprox3(T1, T2, REL));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(LINE, RC == caApprox3(CA1, CA2, REL));
            LOOP_ASSERT(LINE, RC == ecaApprox3(CA1, CA2, REL));

            Choice mC1(mCA1[21]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[21]);  const Choice& C2 = mC2;

            LOOP_ASSERT(LINE, RC == cApprox3(C1, C2, REL));
            LOOP_ASSERT(LINE, RC == ecApprox3(C1, C2, REL));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING APPROXIMATE-EQUALITY FUNCTIONS
        // Concerns:
        //   The main underlying functionality has already been tested in cases
        //   2 and 3.  Our primary concern here is that the
        //   'areXXXApproximatelyEqual' functions correctly forward their
        //   arguments when default values are used for both relative
        //   tolerances and absolute tolerances.
        //
        // Plan:
        //   In a table-driven test, confirm the correct operation of the
        //   'areXXXApproximatelyEqual' functions using the default values for
        //   the relative tolerance and the absolute tolerance.
        //
        //   Construct a pair of identical lists and a pair of identical
        //   tables.  In a sequence of blocks, for each pair of objects, vary
        //   exactly one corresponding pair of 'float' or 'double' values:
        //      (1) in one FLOAT element
        //      (2) in one DOUBLE element
        //      (3) in one position in one FLOATARRAY element
        //      (4) in one position in one DOUBLEARRAY element
        //      (5) in one FLOAT element of one (contained) LIST element
        //      (6) in one DOUBLE element of one (contained) LIST element
        //      (7) in one FLOAT element of one (contained) TABLE element
        //      (8) in one DOUBLE element of one (contained) TABLE element
        //
        // Testing:
        //   bool     areElemRefsApproximatelyEqual(lhs, rhs);
        //   bool         areRowsApproximatelyEqual(lhs, rhs);
        //   bool        areListsApproximatelyEqual(lhs, rhs);
        //   bool       areTablesApproximatelyEqual(lhs, rhs);
        //   bool      areChoicesApproximatelyEqual(lhs, rhs);
        //   bool areChoiceArraysApproximatelyEqual(lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "TESTING 'areXXXApproximatelyEqual' w/DEFAULT TOLERANCES"
            << endl
            << "======================================================="
            << endl;

        if (verbose) cout << "\nTesting 'areXXXApproximatelyEqual' "
                             "with both default tolerances." << endl;

        // Variables and data common to all four tests in this case:

        static const struct {       // Test input/expected output
            float d_a;              //   rhs value to vary in tests
            float d_b;              //   lhs value to vary in tests
            int   d_eq;             //   expected return value
        } DATA[] = {
            // d_a        d_b             Expected
            // --------   --------        --------
            {  1.0     ,  1.0     ,       1       },
            {  1.0     ,  0.9999  ,       0       },
            {  1.0     ,  0.9995  ,       0       },
            {  1.0     ,  0.999   ,       0       },
            {  0.9999  ,  1.0     ,       0       },
            {  0.9995  ,  1.0     ,       0       },
            {  0.999   ,  1.0     ,       0       },
            {  1e-23   ,  1e-24   ,       0       },
            {  1e-24   ,  1e-25   ,       1       },
            {  1e-25   ,  1e-26   ,       1       }
        };
        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        // Create the pairs of test aggregates used as arguments to the
        // 'areApproximatelyEqual' methods during testing.

        List  mL1(TEST_LIST);   const List&  L1 = mL1;
        List  mL2(L1);          const List&  L2 = mL2;
        Table mT1(TEST_TABLE);  const Table& T1 = mT1;
        Table mT2(T1);          const Table& T2 = mT2;

        int i;

        if (verbose) cout << "\tVarying only a FLOAT." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const float FA = DATA[i].d_a;
            const float FB = DATA[i].d_b;
            const bool  RC = DATA[i].d_eq;

            mL1.theModifiableFloat(4)                       = FA;
            mL2.theModifiableFloat(4)                       = FB;
            mT1.theModifiableRow(1)[4].theModifiableFloat() = FA;
            mT2.theModifiableRow(1)[4].theModifiableFloat() = FB;

            LOOP_ASSERT(i, RC == rApprox2(R1, R2));
            LOOP_ASSERT(i, RC == lApprox2(L1, L2));
            LOOP_ASSERT(i, RC == elApprox2(L1, L2));
            LOOP_ASSERT(i, RC == tApprox2(T1, T2));
            LOOP_ASSERT(i, RC == etApprox2(T1, T2));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(i, RC == caApprox2(CA1, CA2));
            LOOP_ASSERT(i, RC == ecaApprox2(CA1, CA2));

            Choice mC1(mCA1[4]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[4]);  const Choice& C2 = mC2;

            LOOP_ASSERT(i, RC == cApprox2(C1, C2));
            LOOP_ASSERT(i, RC == ecApprox2(C1, C2));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a DOUBLE." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const float FA = DATA[i].d_a;
            const float FB = DATA[i].d_b;
            const bool  RC = DATA[i].d_eq;

            mL1.theModifiableDouble(5)                       = FA;
            mL2.theModifiableDouble(5)                       = FB;
            mT1.theModifiableRow(1)[5].theModifiableDouble() = FA;
            mT2.theModifiableRow(1)[5].theModifiableDouble() = FB;

            LOOP_ASSERT(i, RC == rApprox2(R1, R2));
            LOOP_ASSERT(i, RC == lApprox2(L1, L2));
            LOOP_ASSERT(i, RC == elApprox2(L1, L2));
            LOOP_ASSERT(i, RC == tApprox2(T1, T2));
            LOOP_ASSERT(i, RC == etApprox2(T1, T2));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(i, RC == caApprox2(CA1, CA2));
            LOOP_ASSERT(i, RC == ecaApprox2(CA1, CA2));

            Choice mC1(mCA1[5]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[5]);  const Choice& C2 = mC2;

            LOOP_ASSERT(i, RC == cApprox2(C1, C2));
            LOOP_ASSERT(i, RC == ecApprox2(C1, C2));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a FLOATARRAY." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const float FA = DATA[i].d_a;
            const float FB = DATA[i].d_b;
            const bool  RC = DATA[i].d_eq;

            mL1.theModifiableFloatArray(14)[1]      = FA;
            mL2.theModifiableFloatArray(14)[1]      = FB;
            mT1.theModifiableRow(1)[14].theModifiableFloatArray()[1]
                                                    = FA;
            mT2.theModifiableRow(1)[14].theModifiableFloatArray()[1]
                                                    = FB;

            LOOP_ASSERT(i, RC == rApprox2(R1, R2));
            LOOP_ASSERT(i, RC == lApprox2(L1, L2));
            LOOP_ASSERT(i, RC == elApprox2(L1, L2));
            LOOP_ASSERT(i, RC == tApprox2(T1, T2));
            LOOP_ASSERT(i, RC == etApprox2(T1, T2));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(i, RC == caApprox2(CA1, CA2));
            LOOP_ASSERT(i, RC == ecaApprox2(CA1, CA2));

            Choice mC1(mCA1[14]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[14]);  const Choice& C2 = mC2;

            LOOP_ASSERT(i, RC == cApprox2(C1, C2));
            LOOP_ASSERT(i, RC == ecApprox2(C1, C2));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a DOUBLEARRAY." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const float FA = DATA[i].d_a;
            const float FB = DATA[i].d_b;
            const int   RC = DATA[i].d_eq;

            mL1.theModifiableDoubleArray(15)[1] = FA;
            mL2.theModifiableDoubleArray(15)[1] = FB;
            mT1.theModifiableRow(1)[15].theModifiableDoubleArray()[1]
                                                = FA;
            mT2.theModifiableRow(1)[15].theModifiableDoubleArray()[1]
                                                = FB;

            LOOP_ASSERT(i, RC == rApprox2(R1, R2));
            LOOP_ASSERT(i, RC == lApprox2(L1, L2));
            LOOP_ASSERT(i, RC == elApprox2(L1, L2));
            LOOP_ASSERT(i, RC == tApprox2(T1, T2));
            LOOP_ASSERT(i, RC == etApprox2(T1, T2));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(i, RC == caApprox2(CA1, CA2));
            LOOP_ASSERT(i, RC == ecaApprox2(CA1, CA2));

            Choice mC1(mCA1[15]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[15]);  const Choice& C2 = mC2;

            LOOP_ASSERT(i, RC == cApprox2(C1, C2));
            LOOP_ASSERT(i, RC == ecApprox2(C1, C2));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a FLOAT in a sub-LIST." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const float FA = DATA[i].d_a;
            const float FB = DATA[i].d_b;
            const bool  RC = DATA[i].d_eq;

            mL1.theModifiableList(20).theModifiableFloat(0) = FA;
            mL2.theModifiableList(20).theModifiableFloat(0) = FB;
            mT1.theModifiableRow(1)[20].theModifiableList().
                                      theModifiableFloat(0) = FA;
            mT2.theModifiableRow(1)[20].theModifiableList().
                                      theModifiableFloat(0) = FB;

            LOOP_ASSERT(i, RC == rApprox2(R1, R2));
            LOOP_ASSERT(i, RC == lApprox2(L1, L2));
            LOOP_ASSERT(i, RC == elApprox2(L1, L2));
            LOOP_ASSERT(i, RC == tApprox2(T1, T2));
            LOOP_ASSERT(i, RC == etApprox2(T1, T2));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(i, RC == caApprox2(CA1, CA2));
            LOOP_ASSERT(i, RC == ecaApprox2(CA1, CA2));

            Choice mC1(mCA1[20]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[20]);  const Choice& C2 = mC2;

            LOOP_ASSERT(i, RC == cApprox2(C1, C2));
            LOOP_ASSERT(i, RC == ecApprox2(C1, C2));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a DOUBLE in a sub-LIST." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const float FA = DATA[i].d_a;
            const float FB = DATA[i].d_b;
            const bool  RC = DATA[i].d_eq;

            mL1.theModifiableList(20).theModifiableDouble(1) = FA;
            mL2.theModifiableList(20).theModifiableDouble(1) = FB;
            mT1.theModifiableRow(1)[20].theModifiableList().
                                      theModifiableDouble(1) = FA;
            mT2.theModifiableRow(1)[20].theModifiableList().
                                      theModifiableDouble(1) = FB;

            LOOP_ASSERT(i, RC == rApprox2(R1, R2));
            LOOP_ASSERT(i, RC == lApprox2(L1, L2));
            LOOP_ASSERT(i, RC == elApprox2(L1, L2));
            LOOP_ASSERT(i, RC == tApprox2(T1, T2));
            LOOP_ASSERT(i, RC == etApprox2(T1, T2));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(i, RC == caApprox2(CA1, CA2));
            LOOP_ASSERT(i, RC == ecaApprox2(CA1, CA2));

            Choice mC1(mCA1[20]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[20]);  const Choice& C2 = mC2;

            LOOP_ASSERT(i, RC == cApprox2(C1, C2));
            LOOP_ASSERT(i, RC == ecApprox2(C1, C2));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a FLOAT in a sub-TABLE." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const float FA = DATA[i].d_a;
            const float FB = DATA[i].d_b;
            const bool  RC = DATA[i].d_eq;

            mL1.theModifiableTable(21).theModifiableRow(0)[0].
                                           theModifiableFloat() = FA;
            mL2.theModifiableTable(21).theModifiableRow(0)[0].
                                           theModifiableFloat() = FB;
            mT1.theModifiableRow(1)[21].theModifiableTable().
                    theModifiableRow(0)[0].theModifiableFloat() = FA;
            mT2.theModifiableRow(1)[21].theModifiableTable().
                    theModifiableRow(0)[0].theModifiableFloat() = FB;

            LOOP_ASSERT(i, RC == rApprox2(R1, R2));
            LOOP_ASSERT(i, RC == lApprox2(L1, L2));
            LOOP_ASSERT(i, RC == elApprox2(L1, L2));
            LOOP_ASSERT(i, RC == tApprox2(T1, T2));
            LOOP_ASSERT(i, RC == etApprox2(T1, T2));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(i, RC == caApprox2(CA1, CA2));
            LOOP_ASSERT(i, RC == ecaApprox2(CA1, CA2));

            Choice mC1(mCA1[21]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[21]);  const Choice& C2 = mC2;

            LOOP_ASSERT(i, RC == cApprox2(C1, C2));
            LOOP_ASSERT(i, RC == ecApprox2(C1, C2));
        }

        mL1 = TEST_LIST;   mL2 = mL1;
        mT1 = TEST_TABLE;  mT2 = mT1;

        if (verbose) cout << "\tVarying only a DOUBLE in a sub-TABLE." << endl;
        for (i = 0; i < NUM_DATA; ++i) {
            const float FA = DATA[i].d_a;
            const float FB = DATA[i].d_b;
            const bool  RC = DATA[i].d_eq;

            mL1.theModifiableTable(21).theModifiableRow(0)[1].
                                           theModifiableDouble() = FA;
            mL2.theModifiableTable(21).theModifiableRow(0)[1].
                                           theModifiableDouble() = FB;
            mT1.theModifiableRow(1)[21].theModifiableTable().
                    theModifiableRow(0)[1].theModifiableDouble() = FA;
            mT2.theModifiableRow(1)[21].theModifiableTable().
                    theModifiableRow(0)[1].theModifiableDouble() = FB;

            LOOP_ASSERT(i, RC == rApprox2(R1, R2));
            LOOP_ASSERT(i, RC == lApprox2(L1, L2));
            LOOP_ASSERT(i, RC == elApprox2(L1, L2));
            LOOP_ASSERT(i, RC == tApprox2(T1, T2));
            LOOP_ASSERT(i, RC == etApprox2(T1, T2));

            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            choiceArrayFromList(mCA1, L1);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            choiceArrayFromList(mCA2, L2);

            LOOP_ASSERT(i, RC == caApprox2(CA1, CA2));
            LOOP_ASSERT(i, RC == ecaApprox2(CA1, CA2));

            Choice mC1(mCA1[21]);  const Choice& C1 = mC1;
            Choice mC2(mCA2[21]);  const Choice& C2 = mC2;

            LOOP_ASSERT(i, RC == cApprox2(C1, C2));
            LOOP_ASSERT(i, RC == ecApprox2(C1, C2));
        }

      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING APPROXIMATE-EQUALITY FUNCTION FOR TABLE WHEN EXACTLY EQUAL
        // Concerns:
        //   The main underlying functionality has already been tested in case
        //   2.  Only the relatively trivial loop extracting rows from a table
        //   and the inline-forwarding needs to be tested.  When all tolerances
        //   are passed as zeros, the functions must act exactly as
        //   'operator==', and so the 'bdem_aggregate' test is borrowed.
        //
        // Plan:
        //   Thoroughly test the ability of the 'areTablesApproximatelyEqual'
        //   function to detect exact equality (i.e., when 'relativeTolerance'
        //   and 'absoluteTolerance' are both set to zero) of two tables that
        //   are either exactly equal or slightly different.
        //
        //   Specify a set S of unique tables with various minor or subtle
        //   differences.  Verify the correctness of the method under test,
        //   with all tolerances passed as zero, using all elements (u,v) of
        //   the cross product S X S.
        //
        // Testing:
        //   bool areTablesApproximatelyEqual(lhs, rhs, 0, 0, 0, 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "TESTING 'areTablesApproximatelyEqual' FUNCTION" << endl
            << "==============================================" << endl;

        if (verbose) cout <<
            "\nTesting 'areTablesApproximatelyEqual(X, Y, 0, 0, 0, 0)'."
            << endl;

        if (verbose) cout <<
            "\tCompare each pair of similar values (u, v) in S X S." << endl;
        static const char *SPECS[] = {
            "A;",          "B;",           "C;",
            "",            ";",
            "A;x;",        "B;x;",         "C;x;",
            "A;u;",        "B;u;",         ";;",
            "A;x;y;",      "B;x;y;",       "C;x;y;",
            "ABCDE;xxxxx;yxxxx;",          "DEFGHI;xxxxxx;yyyyyy;",
            "FGHITUV;xyxyxyx;yxyxyxy;",
            "AB;xy;xy;xy;yx;",
            "AB;xy;xy;xy;yy;",
        0}; // Null string required as last element.

        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const U_SPEC = SPECS[ti];

            Table        u(&testAllocator); hh(&u, U_SPEC);
            const Table& U = u;

            if (veryVerbose) { P_(ti); P_(U_SPEC); P(U); }

            for (int tj = 0; SPECS[tj]; ++tj) {
                const char *const V_SPEC = SPECS[tj];
                Table        v(&testAllocator); hh(&v, V_SPEC);
                const Table& V = v;

                if (veryVerbose) { cout << "  "; P_(tj); P_(V_SPEC); P(V); }
                const bool isSame = ti == tj;
                LOOP2_ASSERT(ti, tj, isSame == tExact(U, V));
                if (isSame) {
                    LOOP2_ASSERT(ti, tj, isSame == etExact(U, V));
                }
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING APPROXIMATE-EQUALITY FUNCTIONS
        // Concerns:
        //   There is only one non-trivial (i.e., non-inline-forward) function
        //   being tested; When all tolerances are passed as zeros, the
        //   functions must act exactly as 'operator==', and so the
        //   'bdem_aggregate' test is borrowed.  An extra test varies each
        //   type and each value separately.
        //
        // Plan:
        //   Thoroughly test the ability of the 'areXXXApproximatelyEqual'
        //   functions to detect exact equality (i.e., when
        //   'relativeTolerance' and 'absoluteTolerance' are set equal to
        //   zero) of aggregates that are either exactly equal or slightly
        //   different.
        //
        //   1) Specify a set S of unique aggregates with various minor or
        //      subtle differences.  Verify the correctness of the methods
        //      under test, with all tolerances passed as zero, using all
        //      elements (u,v) of the cross product S X S.
        //   2) Test a set of aggregate pairs (u, v) where u and v are
        //      relatively long aggregates with minor differences.
        //   3) "uW" is left out of the spec on purpose because there are
        //      only two values for bool (0 and 1) these values are tested
        //      in "xW" and "yW".
        //
        // Testing:
        //   bool     areElemRefsApproximatelyEqual(lhs, rhs, 0, 0, 0, 0);
        //   bool         areRowsApproximatelyEqual(lhs, rhs, 0, 0, 0, 0);
        //   bool        areListsApproximatelyEqual(lhs, rhs, 0, 0, 0, 0);
        //   bool      areChoicesApproximatelyEqual(lhs, rhs, 0, 0, 0, 0);
        //   bool areChoiceArraysApproximatelyEqual(lhs, rhs, 0, 0, 0, 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'areXXXApproximatelyEqual'" << endl
                          << "==================================" << endl;

        if (verbose) cout <<
            "\nTesting 'areXXXApproximatelyEqual(X, Y, 0, 0, 0, 0)'." << endl;

        if (verbose) cout <<
            "\tCompare each pair of similar values (u, v) in S X S." << endl;
        {
            static const char *SPECS[] = {
                "",
                "xA", "xB", "xC", "xD", "xE", "xF", "xG", "xH", "xI", "xJ",
                "xK", "xL", "xM", "xN", "xO", "xP", "xQ", "xR", "xS", "xT",
                "xU", "xV", "xW", "xX", "xY", "xZ", "xa", "xb", "xc", "xd",
                "xe", "xf",
                "yA", "yB", "yC", "yD", "yE", "yF", "yG", "yH", "yI", "yJ",
                "yK", "yL", "yM", "yN", "yO", "yP", "yQ", "yR", "yS", "yT",
                "yU", "yV", "yW", "yX", "yY", "yZ", "ya", "yb", "yc", "yd",
                "ye", "yf",
                "uA", "uB", "uC", "uD", "uE", "uF", "uG", "uH", "uI", "uJ",
                "uK", "uL", "uM", "uN", "uO", "uP", "uQ", "uR", "uS", "uT",
                "uU", "uV", "uX", "uY", "uZ", "ua", "ub", "uc", "ud", "ue",
                "uf",
                "uAuB",                  "uBuA",
                "uBuB",                  "uBuU",
                "uUuB",                  "uUuU",
                "xAxA",                  "xAxB",
                "xAyB",                  "xBxA",
                "xBxB",                  "xBxU",
                "xUxB",                  "xUxU",
                "yAyA",                  "yAyB",
                "yBxA",                  "yByA",
                "yByB",                  "yByU",
                "yUyB",                  "yUyU",
                "uAuAuA",                "uAuAuB",
                "uAuBuA",                "uBuAuA",
                "xAxAxA",                "xAxAxB",
                "xAxAyB",                "xAxBxA",
                "xAyBxA",                "xBxAxA",
                "yAyAyA",                "yAyAyB",
                "yAyByA",                "yBxAxA",
                "yByAyA",                "xAxAxAxA",
                "xAxAxBxA",              "xAxBxAxA",
                "xBxAxAxA",              "xAxAxAxAxA",
                "xAxAxBxAxA",            "xAxBxAxAxA",
                "xBxAxAxAxA",            "xAxAxAxAxAxA",
                "xAxAxAxBxAxA",          "xAxAxBxAxAxA",
                "xBxAxAxAxAxA",          "xAxAxAxAxAxAxA",
                "xAxAxAxAxAxBxA",        "xBxAxAxAxAxAxA",
                "xAxAxAxAxAxAxAxA",      "xAxAxAxAxAxBxAxA",
                "xAxBxAxAxAxAxAxA",      "xAxAxAxAxAxAxAxAxA",
                "xAxAxAxAxAxBxAxAxA",    "xAxAxBxAxAxAxAxAxA",
                "xAxAxAxAxAxAxAxAxAxA",  "xAxAxAxAxAxBxAxAxAxA",
          "xAxBxCxDxExFxGxHxIxJxKxLxMxNxOxPxQxRxSxTxUxVxWxXxYxZxaxbxcxdxexf",
            0}; // Null string required as last element.

            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const U_SPEC = SPECS[ti];

                List u(&testAllocator); gg(&u, U_SPEC); const List& U = u;
                const Row& RU = U.row();

                ChoiceArray cau;  const ChoiceArray& CAU = cau;
                choiceArrayFromList(cau, U);

                if (veryVerbose) {
                    P_(ti); P_(U_SPEC); P(U); P(CAU);
                }

                Choice cu;  const Choice& CU = cu;
                if (1 == CAU.length()) {
                    cu = CAU[0];
                }

                for (int tj = 0; SPECS[tj]; ++tj) {
                    const char *const V_SPEC = SPECS[tj];
                    List v(&testAllocator); gg(&v, V_SPEC); const List& V = v;
                    const Row& RV = V.row();

                    ChoiceArray cav;  const ChoiceArray& CAV = cav;
                    choiceArrayFromList(cav, V);

                    if (veryVerbose) {
                        cout << "  "; P_(tj); P_(V_SPEC); P(V); P(CAV);
                    }

                    Choice cv;  const Choice& CV = cv;
                    if (1 == CAV.length()) {
                        cv = CAV[0];
                    }

                    const bool isSame = ti == tj;
                    LOOP2_ASSERT(ti, tj, isSame == rExact(RU, RV));
                    LOOP2_ASSERT(ti, tj, isSame == lExact(U, V));
                    LOOP2_ASSERT(ti, tj, isSame == caExact(CAU, CAV));
                    if (isSame) {
                        LOOP2_ASSERT(ti, tj, isSame == elExact(U, V));
                        LOOP2_ASSERT(ti, tj, isSame == ecaExact(CAU, CAV));
                    }

                    if (1 == CU.numSelections() && 1 == CV.numSelections()) {
                        LOOP2_ASSERT(ti, tj, isSame == ecExact(CU, CV));
                        LOOP2_ASSERT(ti, tj, isSame == cExact(CU, CV));
                    }
                }
            }
        }

        if (verbose)
            cout << "\tCompare long lists with minor differences." << endl;
        {
            // TBD: need to add xZ back into spec after bug is fixed.
            char spec[] =
          "xAxBxCxDxExFxGxHxIxJxKxLxMxNxOxPxQxRxSxTxUxVxVxWxXxYxZxaxbxcxdxexf"
          "xAxBxCxDxExFxGxHxIxJxKxLxMxNxOxPxQxRxSxTxUxVxVxWxXxYxZxaxbxcxdxexf";
            const int NUM_SPEC_CHARS = (int) strlen(spec);
            int i;

            List x; const List& X = x;
            gg(&x,spec);
            ASSERT(elExact(X, X) == 1);
            ASSERT(lExact(X, X) == 1);

            ChoiceArray cax;  const ChoiceArray& CAX = cax;
            choiceArrayFromList(cax, X);

            if (veryVerbose) {
                cout << "  "; P(CAX);
            }

            ASSERT(1 == ecaExact(CAX, CAX));
            ASSERT(1 == caExact(CAX, CAX));

            // Change value at every position
            for (i = 0; i < NUM_SPEC_CHARS; i += 2) {
                spec[i] = 'y';
                List y; const List& Y = y;
                gg(&y, spec);
                spec[i] = 'x';  // restore the original value

                LOOP_ASSERT(i, 0 == rExact(X.row(), Y.row()));
                LOOP_ASSERT(i, 0 == elExact(X, Y));
                LOOP_ASSERT(i, 0 == lExact(X, Y));

                ChoiceArray cay;  const ChoiceArray& CAY = cay;
                choiceArrayFromList(cay, Y);

                if (veryVerbose) {
                    cout << "  "; P(CAY);
                }

                LOOP_ASSERT(i, 0 == ecaExact(CAX, CAY));
                LOOP_ASSERT(i, 0 == caExact(CAX, CAY));
            }

            // Change type at every position
            for (i = 1; i < NUM_SPEC_CHARS; i += 2) {
                char orgType = spec[i];
                spec[i] = indexToTypeChar((typeCharToIndex(spec[i]) + 1) % 21);
                List y; const List& Y = y;
                gg(&y, spec);
                spec[i] = orgType;  // restore the original type

                LOOP_ASSERT(i, 0 == rExact(X.row(), Y.row()));
                LOOP_ASSERT(i, 0 == elExact(X, Y));
                LOOP_ASSERT(i, 0 == lExact(X, Y));

                ChoiceArray cay;  const ChoiceArray& CAY = cay;
                choiceArrayFromList(cay, Y);

                if (veryVerbose) {
                    cout << "  "; P(CAY);
                }

                LOOP_ASSERT(i, 0 == ecaExact(CAX, CAY));
                LOOP_ASSERT(i, 0 == caExact(CAX, CAY));
            }
        }
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // Concerns:
        //   We wish to exercise the basic functionality of
        //   'bdem_AggregateUtil' using lists, tables, choices and choice
        //   arrays containing values of type 'float' and 'double', and
        //   non-empty arrays of those types.
        //
        // Plan:
        //   Test basic use of 'areXXXApproximatelyEqual' methods for elem
        //   refs, rows, lists, tables, choices and choice arrays.
        //
        // Testing:
        //   Exercising basic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "BREATHING TEST: Exercising 'areXXXApproximatelyEqual'" << endl
            << "=====================================================" << endl;

        {
            if (verbose)
                cout << "\nExercising 'areXXXApproximatelyEqual'." << endl;

            double rel;                 // temporary for relative tolerance
            double abs;                 // temporary for absolute tolerance
            int    exp;                 // temporary for expected value

            static const struct {       // Test input/expected output
                float  d_a;             //   rhs value to vary in tests
                float  d_b;             //   lhs value to vary in tests
                double d_rel;           //   relative tolerance
                double d_abs;           //   absolute tolerance
                int    d_eq;            //   expected return value
            } DATA[] = {
                // d_a        d_b         rel      abs       Expected
                // --------   --------    ------   ------    --------
                {  1.0     ,  1.0     ,   1e-6,    1e-3,     1       },
                {  1.0     ,  0.9995  ,   1e-6,    1e-3,     1       },
                {  1.0     ,  0.995   ,   1e-6,    1e-3,     0       },
                {  0.9995  ,  1.0     ,   1e-6,    1e-3,     1       },
                {  0.995   ,  1.0     ,   1e-6,    1e-3,     0       },
                {  1.0     ,  0.9995  ,   1e-3,    0   ,     1       },
                {  1.0     ,  0.995   ,   1e-3,    0   ,     0       },
                {  0.9995  ,  1.0     ,   1e-3,    0   ,     1       },
                {  0.995   ,  1.0     ,   1e-3,    0   ,     0       }
            };
            const int NUM_DATA = sizeof DATA / sizeof DATA[0];

            if (verbose)
                cout << "\tFor aggregates containing 'double's." << endl;

            int i;
            for (i = 0; i < NUM_DATA; ++i) {
                const float  FA  = DATA[i].d_a;
                const float  FB  = DATA[i].d_b;
                const double REL = DATA[i].d_rel;
                const double ABS = DATA[i].d_abs;
                const bool   RC  = DATA[i].d_eq;

                bsl::vector<double> mA1;  mA1.resize(2);
                bsl::vector<double> mA2;  mA2.resize(2);

                List mL1(g("xFxP"));  const List& L1 = mL1;
                List mL2(g("xFxP"));  const List& L2 = mL2;

                Table mT1(h("FP;xx;"));  const Table& T1 = mT1;
                Table mT2(h("FP;xx;"));  const Table& T2 = mT2;

                mA1[0] = FA;  mA1[1] = FA;
                mA2[0] = FB;  mA2[1] = FB;

                mL1.theModifiableDouble(0)      = FA;
                mL1.theModifiableDoubleArray(1) = mA1;
                mL2.theModifiableDouble(0)      = FB;
                mL2.theModifiableDoubleArray(1) = mA2;

                mT1.theModifiableRow(0)[0].theModifiableDouble()      = FA;
                mT1.theModifiableRow(0)[1].theModifiableDoubleArray() = mA1;
                mT2.theModifiableRow(0)[0].theModifiableDouble()      = FB;
                mT2.theModifiableRow(0)[1].theModifiableDoubleArray() = mA2;

                LOOP_ASSERT(i, RC == elApprox4(L1, L2, REL, ABS));
                LOOP_ASSERT(i, RC == etApprox4(T1, T2, REL, ABS));
                LOOP_ASSERT(i, RC == rApprox4(L1.row(), L2.row(), REL, ABS));
                LOOP_ASSERT(i, RC == lApprox4(L1, L2, REL, ABS));
                LOOP_ASSERT(i, RC == tApprox4(T1, T2, REL, ABS));

                ChoiceArray mCA1;  const ChoiceArray& CA1 = mCA1;
                choiceArrayFromList(mCA1, mL1);

                ChoiceArray mCA2;  const ChoiceArray& CA2 = mCA2;
                choiceArrayFromList(mCA2, mL2);

                Choice mC10(mCA1[0]);  const Choice& C10 = mC10;
                Choice mC20(mCA2[0]);  const Choice& C20 = mC20;
                Choice mC11(mCA1[1]);  const Choice& C11 = mC11;
                Choice mC21(mCA2[1]);  const Choice& C21 = mC21;

                if (veryVerbose) {
                  cout << "    "; P(CA1);
                  cout << "    "; P(CA2);
                  cout << "    "; P_(REL); P_(ABS); P(RC);
                }

                LOOP_ASSERT(i, RC == caApprox4(CA1, CA2, REL, ABS));
                LOOP_ASSERT(i, RC == cApprox4(C10, C20, REL, ABS));
                LOOP_ASSERT(i, RC == cApprox4(C11, C21, REL, ABS));
                LOOP_ASSERT(i, RC == ecApprox4(C10, C20, REL, ABS));
                LOOP_ASSERT(i, RC == ecApprox4(C11, C21, REL, ABS));
                LOOP_ASSERT(i, RC == ecaApprox4(CA1, CA2, REL, ABS));

                if (veryVerbose) {
                  cout << endl;
                }
            }

            if (verbose)
                cout << "\tFor aggregates containing 'float's." << endl;
            for (i = 0; i < NUM_DATA; ++i) {
                const float  FA  = DATA[i].d_a;
                const float  FB  = DATA[i].d_b;
                const double REL = DATA[i].d_rel;
                const double ABS = DATA[i].d_abs;
                const bool   RC  = DATA[i].d_eq;

                bsl::vector<float> mA1;  mA1.resize(2);
                bsl::vector<float> mA2;  mA2.resize(2);

                List mL1(g("xExO"));     const List& L1 = mL1;
                List mL2(g("xExO"));     const List& L2 = mL2;

                Table mT1(h("EO;xx;"));  const Table& T1 = mT1;
                Table mT2(h("EO;xx;"));  const Table& T2 = mT2;

                mA1[0] = FA;  mA1[1] = FA;
                mA2[0] = FB;  mA2[1] = FB;

                mL1.theModifiableFloat(0)      = FA;
                mL1.theModifiableFloatArray(1) = mA1;
                mL2.theModifiableFloat(0)      = FB;
                mL2.theModifiableFloatArray(1) = mA2;

                mT1.theModifiableRow(0)[0].theModifiableFloat()      = FA;
                mT1.theModifiableRow(0)[1].theModifiableFloatArray() = mA1;
                mT2.theModifiableRow(0)[0].theModifiableFloat()      = FB;
                mT2.theModifiableRow(0)[1].theModifiableFloatArray() = mA2;

                LOOP_ASSERT(i, RC == eApprox6(L1[0], L2[0], 0, 0, REL, ABS));
                LOOP_ASSERT(i, RC == eApprox6(L1[1], L2[1], 0, 0, REL, ABS));
                LOOP_ASSERT(i, RC == rApprox6(L1.row(),
                                              L2.row(),
                                              0, 0, REL, ABS));
                LOOP_ASSERT(i, RC == lApprox6(L1, L2, 0, 0, REL, ABS));
                LOOP_ASSERT(i, RC == tApprox6(T1, T2, 0, 0, REL, ABS));

                ChoiceArray mCA1;  const ChoiceArray& CA1 = mCA1;
                choiceArrayFromList(mCA1, mL1);

                ChoiceArray mCA2;  const ChoiceArray& CA2 = mCA2;
                choiceArrayFromList(mCA2, mL2);

                Choice mC10(mCA1[0]);  const Choice& C10 = mC10;
                Choice mC20(mCA2[0]);  const Choice& C20 = mC20;
                Choice mC11(mCA1[1]);  const Choice& C11 = mC11;
                Choice mC21(mCA2[1]);  const Choice& C21 = mC21;

                if (veryVerbose) {
                  cout << "    "; P(CA1);
                  cout << "    "; P(CA2);
                  cout << "    "; P_(REL); P_(ABS); P(RC);
                }

                LOOP_ASSERT(i, RC == caApprox6(CA1, CA2, 0, 0, REL, ABS));
                LOOP_ASSERT(i, RC == cApprox6(C10, C20, 0, 0, REL, ABS));
                LOOP_ASSERT(i, RC == cApprox6(C11, C21, 0, 0, REL, ABS));

                if (veryVerbose) {
                  cout << endl;
                }
            }
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error: non-zero test status = " << testStatus << "." << endl;
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
