// bael_record.t.cpp         -*-C++-*-

#include <bael_record.h>

#include <bael_severity.h>                      // for testing only

#include <bdem_row.h>                           // for testing only
#include <bdem_table.h>                         // for testing only

#include <bdetu_datetime.h>                     // for testing only
#include <bdetu_unset.h>                        // for testing only

#include <bdex_instreamfunctions.h>             // for testing only
#include <bdex_outstreamfunctions.h>            // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only
#include <bdex_testoutstream.h>                 // for testing only

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only

#include <bsls_platform.h>                      // for testing only
#include <bsls_platformutil.h>                  // for testing only

#include <bsl_iostream.h>
#include <bsl_new.h>          // placement 'new' syntax
#include <bsl_string.h>
#include <bsl_strstream.h>
#include <bsl_vector.h>

#include <bsl_cstdlib.h>      // atoi()
#include <bsl_cstring.h>      // strlen(), memset(), memcpy(), memcmp()
#ifdef BSLS_PLATFORM_OS_UNIX
#include <unistd.h>     // getpid()
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test has value-semantic and contains value-semantic
// members.  In addition to standard value-semantic methods, this component
// also implements a method to provide a (tight) upper bound to the dynamic
// memory required to hold the current instance, which is implemented with a
// tested component (bael_countingallocator).  Our concerns regarding the
// implementation of this component are that (1) all the standard
// value-semantic methods are implemented correctly (2) the bookkeeping of
// the dynamically allocated memory are done correctly and (3) the bookkeeping
// is done correctly when exception are thrown.
//
// Note that places where test drivers in this family are likely to require
// adjustment are indicated by the tag: "ADJ".
//-----------------------------------------------------------------------------
// [ 9] static int maxSupportedBdexVersion();
// [ 2] bael_Record(bslma_Allocator *ba = 0);
// [ 2] bael_Record(const bael_RecordAttributes&, const bdem_List&, *ba = 0);
// [ 7] bael_Record(const bael_Record& original, *ba = 0);
// [ 2] ~bael_Record();
// [ 8] bael_Record& operator=(const bael_Record& rhs);
// [ 1] bael_RecordAttributes& fixedFields();
// [ 2] void setFixedFields(const bael_RecordAttributes& fixedFields);
// [ 2] void setUserFields(const bdem_List& userFields);
// [ 1] bdem_List& userFields();
// [ 9] STREAM& bdexStreamIn(STREAM& stream, int version);
// [ 4] const bael_RecordAttributes& fixedFields() const;
// [ 4] const bdem_List& userFields() const;
// [10] int numAllocatedBytes() const;
// [  ] bsl::ostream& print(bsl::ostream& stream, int level, int spl) const;
// [ 9] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 6] bool operator==(const bael_Record& lhs, const bael_Record& rhs);
// [ 6] operator!=(const bael_Record& lhs, const bael_Record& rhs);
// [ 9] STREAM& operator>>(STREAM& stream, bael_Record& rhs);
// [ 9] STREAM& operator<<(STREAM& stream, const bael_Record& rhs);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] TESTING GENERATOR FUNCTIONS 'GG' AND 'GGG' ('bdem_List')
// [11] USAGE EXAMPLE 1
// [12] USAGE EXAMPLE 2
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
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bael_Record Obj;
typedef bael_RecordAttributes Record_Attr;
typedef bdem_List List;
typedef bdem_Row Row;
typedef bsls_PlatformUtil::Int64 Int64;
typedef bsl::vector<bdem_ElemType::Type> my_ElemTypeArray;
typedef bdex_TestInStream In;
typedef bdex_TestOutStream Out;

const int NUM_TYPES = bdem_ElemType::BDEM_NUM_TYPES;

void getElemTypes(my_ElemTypeArray *types, const bdem_Row& row)
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

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
//=============================================================================
//          HELPER FUNCTIONS OPERATING ON 'g' AND 'h' 'spec' CHARACTERS
//
// These functions identify the 'type' and 'value' characters of the 'g' and
// 'h' languages, and convert to and from 'char' (string) and 'int' (index)
// representations.  Some of these are used by 'ggg' and/or 'hhh' directly, but
// the complete set is made available for general use.
//-----------------------------------------------------------------------------

static int isTypeChar(char c)
    // Return 1 if the specified character 'c' is in "ABCDEFGHIJKLMNOPQRSTUV",
    // and 0 otherwise.
{
    static const char TYPE_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUV";
    return 0 != c && 0 != strchr(TYPE_CHARS, c);
}

static int isValChar(char c)
    // Return 1 if the specified character, 'c', is in "xyu", and 0 otherwise.
{
    return 'x' == c || 'y' == c || 'u' == c;
}

static int typeCharToIndex(char type)
    // Return the integer index of the specified 'type' character.  The
    // behavior is undefined unless 'type' is one of "ABCDEFGHIJKLMNOPQRSTUV".
{
    ASSERT(isTypeChar(type));
    static const char TYPE_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUV";
    return strchr(TYPE_CHARS, type) - TYPE_CHARS;
}

static int valCharToIndex(char val)
    // Return the integer index of the specified 'val' character.  The behavior
    // is undefined unless 'val' is one of "xyu".
{
    ASSERT(isValChar(val));
    static const char VAL_CHARS[] = "xyu";
    return strchr(VAL_CHARS, val) - VAL_CHARS;
}

static int getPid()
{
#ifdef BSLS_PLATFORM_OS_UNIX
    return getpid();
#else
    return 0;
#endif
}
//=============================================================================
//                        TEST DATA POINTER ARRAYS
//
// This set of 66 pointers (22 array-of-3 types) are defined here at global
// scope because they are used by the 'ggg' helper 'appendElement'.  They are
// populated in 'main' after the definition of the specific test values, but
// are available as needed to various additional helper functions.
//-----------------------------------------------------------------------------

static const char                      *G_VALUES_A[3];  // { &XA, &YA, &UA }
static const short                     *G_VALUES_B[3];  // { &XB, &YB, &UB }
static const int                       *G_VALUES_C[3];  // { &XC, &YC, &UC }
static const Int64                     *G_VALUES_D[3];  // ...
static const float                     *G_VALUES_E[3];
static const double                    *G_VALUES_F[3];
static const bsl::string               *G_VALUES_G[3];
static const bdet_Datetime             *G_VALUES_H[3];
static const bdet_Date                 *G_VALUES_I[3];
static const bdet_Time                 *G_VALUES_J[3];
static const bsl::vector<char>          *G_VALUES_K[3];
static const bsl::vector<short>         *G_VALUES_L[3];
static const bsl::vector<int>           *G_VALUES_M[3];
static const bsl::vector<Int64>         *G_VALUES_N[3];
static const bsl::vector<float>         *G_VALUES_O[3];
static const bsl::vector<double>        *G_VALUES_P[3];
static const bsl::vector<bsl::string>   *G_VALUES_Q[3];
static const bsl::vector<bdet_Datetime> *G_VALUES_R[3];
static const bsl::vector<bdet_Date>     *G_VALUES_S[3];
static const bsl::vector<bdet_Time>     *G_VALUES_T[3];
static const bdem_List                 *G_VALUES_U[3];
static const bdem_Table                *G_VALUES_V[3];  // { &XV, &YV, &UV }

//=============================================================================
//                     HELPER FUNCTIONS FOR 'ggg', 'hhh'
//
// These functions are used by 'ggg' and/or 'hhh' directly.  They are also
// available as general-purpose helpers.
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

static void appendElement(List *address, char val, char type)
    // Append an element of the 'bdem_ElemType::Type' corresponding to the
    // specified 'type' character and having the value corresponding to the
    // specified 'val' character to the 'bdem_List' at the specified 'address'.
{
    ASSERT('x' == val ||'y' == val ||'u' == val)
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

      default:  P(val);  P(type);  ASSERT(!"Bad type character");
    }
    return;
}

static void appendToElemTypeArray(my_ElemTypeArray *e, const char *spec)
    // Append to the specified 'bdet_ElemTypeArray' 'e' the sequence of types
    // in the specified 'spec'.
{
    for (; *spec; ++spec) {
        e->push_back(bdem_ElemType::Type(typeCharToIndex(*spec)));
    }
    return;
}

//=============================================================================
//       GENERATOR FUNCTIONS 'g', 'gg', AND 'ggg' FOR TESTING LISTS
//-----------------------------------------------------------------------------
// The 'g' family of functions generate a 'bdem_List' object for testing.
// They, along with their helpers, interpret a given 'spec' (from left to
// right) to configure the list according to a custom language.  The 22
// characters [A-V] represent the 22 'bdem_ElemType' types.  'x', 'y', and
// 'u' respectively represent two arbitrary but unique values and the 'unset'
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
//                   'S' | 'T' | 'U' | 'V' |    //the 22 'bdem_ElemType' types
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

static int ggg(List *address, const char *spec, int showErrorFlag)
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

List& gg(List *address, const char *spec)
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

List g(const char *spec)
    // Return by value a new 'bdem_List' object corresponding to the specified
    // 'spec'.
{
    List object;
    return gg(&object, spec);
}

//=============================================================================
//              Miscellaneous helpers following the general pattern
//-----------------------------------------------------------------------------

static int areEqual(const Row& a, int i, const Row& b, int j)
    // Return 1 if the two elements in the two specified rows 'a' and 'b' at
    // the specified indices 'i, and 'j', respectively, have the same type and
    // value, and zero otherwise.
{
    ASSERT(0 <= i);
    ASSERT(i < a.length());
    ASSERT(0 <= j);
    ASSERT(j < b.length());

    if (a.elemType(i) != b.elemType(j)) {
        return 0;
    }

    typedef bdem_ElemType T;

    switch(a.elemType(i)) {
      case T::BDEM_CHAR:
        return a.theChar(i)          == b.theChar(j);
      case T::BDEM_SHORT:
        return a.theShort(i)         == b.theShort(j);
      case T::BDEM_INT:
        return a.theInt(i)           == b.theInt(j);
      case T::BDEM_INT64:
        return a.theInt64(i)         == b.theInt64(j);
      case T::BDEM_FLOAT:
        return a.theFloat(i)         == b.theFloat(j);
      case T::BDEM_DOUBLE:
        return a.theDouble(i)        == b.theDouble(j);
      case T::BDEM_STRING:
        return a.theString(i)        == b.theString(j);
      case T::BDEM_DATETIME:
        return a.theDatetime(i)      == b.theDatetime(j);
      case T::BDEM_DATE:
        return a.theDate(i)          == b.theDate(j);
      case T::BDEM_TIME:
        return a.theTime(i)          == b.theTime(j);
      case T::BDEM_CHAR_ARRAY:
        return a.theCharArray(i)     == b.theCharArray(j);
      case T::BDEM_SHORT_ARRAY:
        return a.theShortArray(i)    == b.theShortArray(j);
      case T::BDEM_INT_ARRAY:
        return a.theIntArray(i)      == b.theIntArray(j);
      case T::BDEM_INT64_ARRAY:
        return a.theInt64Array(i)    == b.theInt64Array(j);
      case T::BDEM_FLOAT_ARRAY:
        return a.theFloatArray(i)    == b.theFloatArray(j);
      case T::BDEM_DOUBLE_ARRAY:
        return a.theDoubleArray(i)   == b.theDoubleArray(j);
      case T::BDEM_STRING_ARRAY:
        return a.theStringArray(i)   == b.theStringArray(j);
      case T::BDEM_DATETIME_ARRAY:
        return a.theDatetimeArray(i) == b.theDatetimeArray(j);
      case T::BDEM_DATE_ARRAY:
        return a.theDateArray(i)     == b.theDateArray(j);
      case T::BDEM_TIME_ARRAY:
        return a.theTimeArray(i)     == b.theTimeArray(j);
      case T::BDEM_LIST:
        return a.theList(i)          == b.theList(j);
      case T::BDEM_TABLE:
        return a.theTable(i)         == b.theTable(j);

      default:
        ASSERT(!"ERROR");
        return 0;
    }
}

//=============================================================================
//                              USAGE EXAMPLE 2
//-----------------------------------------------------------------------------
class Information
{
  private:
    bsl::string d_heading;
    bsl::string d_contents;

  public:
    Information(const char *heading, const char *contents);
    const bsl::string& heading() const;
    const bsl::string& contents() const;
};

Information::Information(const char *heading, const char *contents)
: d_heading(heading)
, d_contents(contents)
{
}

const bsl::string& Information::heading() const
{
    return d_heading;
}

const bsl::string& Information::contents() const
{
    return d_contents;
}

bsl::ostream& operator<<(bsl::ostream& stream, const Information& information)
{
    stream << information.heading() << endl;
    stream << '\t';
    stream << information.contents() << endl;
    return stream;
}

void logInformation(ostream& logStream,
                    const Information& information,
                    bael_Severity::Level severity,
                    const char *category,
                    const char* fileName,
                    int lineNumber)
{
    bael_Record record;

    // get the modifiable reference to the fixed fields
    bael_RecordAttributes& attributes = record.fixedFields();

    // set various attributes
    bdet_Datetime now;
    bdetu_Datetime::convertFromTimeT(&now, time(0));
    attributes.setTimestamp(now);

    attributes.setProcessID(getPid());

    attributes.setThreadID(0);

    attributes.setFileName(fileName);

    attributes.setLineNumber(lineNumber);

    attributes.setCategory(category);

    attributes.setSeverity(severity);

    // create an 'ostream' from message stream buffer
    ostream os(&attributes.messageStreamBuf());

    // now stream the information object into the created ostream, this will
    // set the message attribute of 'attributes' to the streamed contents.
    os << information;

    // finally log the record into the log stream
    logStream << record;
}

//=============================================================================
//                              USAGE EXAMPLE 1
//-----------------------------------------------------------------------------

static bdem_ElemType::Type listSchema[5] = { bdem_ElemType::BDEM_STRING,
                                             bdem_ElemType::BDEM_DOUBLE,
                                             bdem_ElemType::BDEM_DOUBLE,
                                             bdem_ElemType::BDEM_DOUBLE,
                                             bdem_ElemType::BDEM_DOUBLE
};

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    // ------------------------------------------------------------------------
    // The following "global constants for testing" are in 'main' because the
    // test allocator (defined in 'main') is used in the constructors of the
    // test lists and tables.
    //
    // Make three sets of useful constants for testing: Ui, Xi, Yi
    // ------------------------------------------------------------------------

    bslma_TestAllocator  testAllocator(veryVeryVerbose);
    // TBD:
    testAllocator.setQuiet(1);
    bslma_Allocator     *Z = &testAllocator;

    const char                       UA = bdetu_Unset<char>::unsetValue();
    const short                      UB = bdetu_Unset<short>::unsetValue();
    const int                        UC = bdetu_Unset<int>::unsetValue();
    const Int64                      UD = bdetu_Unset<Int64>::unsetValue();
    const float                      UE = bdetu_Unset<float>::unsetValue();
    const double                     UF = bdetu_Unset<double>::unsetValue();
    const bsl::string                UG;
    const bdet_Datetime              UH;
    const bdet_Date                  UI;
    const bdet_Time                  UJ;

    // Empty arrays, lists, and tables make fine null values, so no special
    // provisions are necessary.
    const bsl::vector<char>           UK;
    const bsl::vector<short>          UL;
    const bsl::vector<int>            UM;
    const bsl::vector<Int64>          UN;
    const bsl::vector<float>          UO;
    const bsl::vector<double>         UP;
    const bsl::vector<bsl::string>    UQ;
    const bsl::vector<bdet_Datetime>  UR;
    const bsl::vector<bdet_Date>      US;
    const bsl::vector<bdet_Time>      UT;

    const bdem_List                  UU;
    const bdem_Table                 UV;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    const char                       XA = 'x';   // = 120 decimal (ascii only)
    const short                      XB = 12;
    const int                        XC = 103;
    const Int64                      XD = 10004;
    const float                      XE = 105.5;
    const double                     XF = 106.006;
    const bsl::string                XG = "one-seven---";   // not in-place
    const bdet_Datetime              XH(bdet_Date(108, 8, 8),
                                        bdet_Time(8, 8, 8, 108));
    const bdet_Date                  XI(109, 9, 9);
    const bdet_Time                  XJ(10, 10, 10, 110);

    bsl::vector<char>                 XK_;
    const bsl::vector<char>&          XK = XK_;
    XK_.push_back(XA);
    XK_.push_back('N');
    XK_.push_back(XA);
    XK_.push_back('N');
    XK_.push_back(XA);

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

    my_ElemTypeArray e;
    getElemTypes(&e, XU.row());
    bdem_Table                       XV_(e.begin(), e.size(), Z);
    const bdem_Table&                XV = XV_;
    XV_.appendNullRow();
    XV_.theModifiableRow(0)[0].theModifiableInt()    = XC;
    XV_.theModifiableRow(0)[1].theModifiableString() = XG;
    XV_.appendNullRow();
    XV_.theModifiableRow(1)[0].theModifiableInt()    = UC;
    XV_.theModifiableRow(1)[1].theModifiableString() = UG;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    const char                       YA = 'y';    // = 121 decimal (ascii only)
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

    getElemTypes(&e, YU.row());
    bdem_Table                       YV_(e.begin(), e.size(), Z);
    const bdem_Table&                YV = YV_;
    YV_.appendNullRow();
    YV_.theModifiableRow(0)[0].theModifiableInt()    = YC;
    YV_.theModifiableRow(0)[1].theModifiableString() = YG;
    YV_.appendNullRow();
    YV_.theModifiableRow(1)[0].theModifiableInt()    = UC;
    YV_.theModifiableRow(1)[1].theModifiableString() = UG;

    //-------------------------------------------------------------------------
    // Two more convenient test objects populated with the 22 bdem types
    //-------------------------------------------------------------------------

    List LX_(Z);  const List& LX = LX_;
    LX_.appendChar(XA);
    LX_.appendShort(XB);
    LX_.appendInt(XC);
    LX_.appendInt64(XD);
    LX_.appendFloat(XE);
    LX_.appendDouble(XF);
    LX_.appendString(XG);
    LX_.appendDatetime(XH);
    LX_.appendDate(XI);
    LX_.appendTime(XJ);
    LX_.appendCharArray(XK);
    LX_.appendShortArray(XL);
    LX_.appendIntArray(XM);
    LX_.appendInt64Array(XN);
    LX_.appendFloatArray(XO);
    LX_.appendDoubleArray(XP);
    LX_.appendStringArray(XQ);
    LX_.appendDatetimeArray(XR);
    LX_.appendDateArray(XS);
    LX_.appendTimeArray(XT);
    LX_.appendList(XU);
    LX_.appendTable(XV);

    getElemTypes(&e, LX.row());
    const my_ElemTypeArray BDEM_TYPES = e;

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

    struct {
        int d_pid;  int d_tid;   int d_lineNum;   int d_severity;
        int d_year; int d_month; int d_day;
        const char *d_fileName;
        const char *d_category;
        const char *d_message;
    } ATTR_VALUES[] = {
///pid     tid     line sev year  month day  file name   category  message
///------- ------- ---- --- ----  ----  ---  ----------  --------  ----------
 { 1,      0,      0,   0,  2004, 1,    1,   "",         "",       ""        },
 { 0,      1,      0,   0,  2004, 1,    1,   "",         "",       ""        },
 { 0,      0,      1,   0,  2004, 1,    1,   "",         "",       ""        },
 { 0,      0,      0,   1,  2004, 1,    1,   "",         "",       ""        },
 { 0,      0,      0,   0,  2000, 1,    1,   "",         "",       ""        },
 { 0,      0,      0,   0,  2004, 2,    1,   "",         "",       ""        },
 { 0,      0,      0,   0,  2004, 1,    2,   "",         "",       ""        },
 { 0,      0,      0,   0,  2004, 1,    1,   "FILE1",    "",       ""        },
 { 0,      0,      0,   0,  2004, 1,    1,   "",         "CATE1",  ""        },
 { 0,      0,      0,   0,  2004, 1,    1,   "",         "",       "MSG1"    }
        };

    const int NUM_ATTRS = sizeof ATTR_VALUES / sizeof ATTR_VALUES[0];
    Record_Attr REC_ATTRS[10];
    for (int jj = 0; jj < NUM_ATTRS; ++jj) {
        const int PID        = ATTR_VALUES[jj].d_pid;
        const int TID        = ATTR_VALUES[jj].d_tid;
        const int YEAR       = ATTR_VALUES[jj].d_year;
        const int MONTH      = ATTR_VALUES[jj].d_month;
        const int DAY        = ATTR_VALUES[jj].d_day;
        const int LINE       = ATTR_VALUES[jj].d_lineNum;
        const int SEVERITY   = ATTR_VALUES[jj].d_severity;
        const char *FILENAME = ATTR_VALUES[jj].d_fileName;
        const char *CATEGORY = ATTR_VALUES[jj].d_category;
        const char *MESSAGE  = ATTR_VALUES[jj].d_message;
        Record_Attr mY(
                bdet_Datetime(YEAR, MONTH, DAY),
                PID,
                TID,
                FILENAME,
                LINE,
                CATEGORY,
                SEVERITY,
                MESSAGE);

        REC_ATTRS[jj] = mY;
    }

    const Record_Attr AA = REC_ATTRS[0];
    const Record_Attr AB = REC_ATTRS[1];
    const Record_Attr AC = REC_ATTRS[2];
    const Record_Attr AD = REC_ATTRS[3];
    const Record_Attr AE = REC_ATTRS[4];
    const Record_Attr AF = REC_ATTRS[5];
    const Record_Attr AG = REC_ATTRS[6];
    const Record_Attr AH = REC_ATTRS[7];
    const Record_Attr AI = REC_ATTRS[8];
    const Record_Attr AJ = REC_ATTRS[9];
    const Record_Attr AU;   // default value

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 2
        //
        // Concerns:
        //   The usage example-2 provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example-2 from header into driver, remove
        //   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Usage Example-2" << endl
                                  << "=======================" << endl;

        Information info("MY-HEADING", "MY-CONTENTS");
        if (verbose) {
            logInformation(cout,
                           info,
                           bael_Severity::BAEL_INFO,
                           "my-category",
                           __FILE__,
                           __LINE__);
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1:
        //
        // Concerns:
        //   The usage example-1 provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example-1 from header into driver, remove
        //   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Usage Example-1" << endl
                                  << "=======================" << endl;

        {
            static bael_RecordAttributes fixedFields(bdet_Datetime(),
                                                     getPid(),
                                                     0,  // threadID
                                                     __FILE__, __LINE__,
                                                     "EQUITY.NASD",
                                                     bael_Severity::BAEL_INFO,
                                                     "Ticker Summary");

            bdet_Datetime now;
            bdetu_Datetime::convertFromTimeT(&now, time(0));
            fixedFields.setTimestamp(now);

            static bdem_List userFields(listSchema, 5);

            userFields.theModifiableString(0) = "SUNW";   // ticker
            userFields.theModifiableDouble(1) = 5.43;     // high
            userFields.theModifiableDouble(2) = 5.17;     // low
            userFields.theModifiableDouble(3) = 5.25;     // open
            userFields.theModifiableDouble(4) = 5.35;     // close

            bael_Record message(fixedFields, userFields);
            ostream &os = cout;
            if (veryVerbose) {
                os << message << endl;
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING FUNCTION numAllocatedBytes()
        //   Verify the memory usage report is correct.
        //
        // Plan:
        //   Create a sequence of objects with a test allocator.  Call the
        //   method under testing and compare the results with those from
        //   the test allocator.
        //
        // Testing:
        //   int numAllocatedBytes() const;
        // --------------------------------------------------------------------

        if (verbose) cout <<
            "\nTesting 'numAllocatedBytes'." << endl
          << "=======================================================" << endl;

        const Obj VU(AU, UU);
        const Obj VA(AA, XU);
        const Obj VB(AB, YU);
        const Obj VC(AC, LX);
        const Obj VD(AD, UU);
        const Obj VE(AE, XU);
        const Obj VF(AF, YU);
        const Obj VG(AG, LX);

        const int NUM_VALUES = 8;
        const Obj VALUES[NUM_VALUES] = { VU, VA, VB, VC, VD, VE, VF, VG };
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        bslma_TestAllocator localTestAllocator;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj X(VALUES[i], &localTestAllocator);
            const int memUsage =
                X.numAllocatedBytes();
            const int memAlloc = localTestAllocator.numBytesInUse();

            if (veryVerbose) { P_(memUsage); P(memAlloc); }
            LOOP_ASSERT(i, memUsage == memAlloc);
        }

      } break;
      case 9: {
// TBD:
#if 0
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //   The 'bdex' streaming concerns for this component are absolutely
        //   standard.  We first probe the member functions 'outStream' and
        //   'inStream' in the manner of a "breathing test" to verify basic
        //   functionality, then we thoroughly test that functionality using
        //   the available bdex stream functions, which forward
        //   appropriate calls to the member functions.  We next step through
        //   the sequence of possible stream states (valid, empty, invalid,
        //   incomplete, and corrupted), appropriately selecting data sets as
        //   described below.  In all cases, exception neutrality is confirmed
        //   using the specially instrumented 'bdex_TestInStream' and a pair of
        //   standard macros, 'BEGIN_BDEX_EXCEPTION_TEST' and
        //   'END_BDEX_EXCEPTION_TEST', which configure the
        //   'bdex_TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   PRELIMINARY MEMBER FUNCTION TEST
        //     First perform a trivial direct test of the 'outStream' and
        //     'inStream' methods (the rest of the testing will use the stream
        //     operators).
        //
        //   VALID STREAMS
        //     For the set S of globally-defined test values, use all
        //     combinations (u, v) in the cross product S X S, stream the
        //     value of v into (a temporary copy of) u and assert u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each u in S, create a copy and attempt to stream into it
        //     from an empty and then invalid stream.  Verify after each try
        //     that the object is unchanged and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct an input stream and attempt to read into objects
        //     initialized with distinct values.  Verify values of objects
        //     that are either successfully modified or left entirely
        //     unmodified, and that the stream became invalid immediately after
        //     the first incomplete read.  Finally ensure that each object
        //     streamed into is in some valid state by assigning it a distinct
        //     new value and testing for equality.
        //
        //   CORRUPTED DATA
        //     Use the underlying stream package to simulate an instance of a
        //     typical valid (control) stream and verify that it can be
        //     streamed in successfully.  Then for each data field in the
        //     stream (beginning with the version number), provide one or more
        //     similar tests with that data field corrupted.  After each test,
        //     verify that the object is in some valid state after streaming,
        //     and that the input stream has gone invalid.
        //
        //   Finally, tests of the explicit wire format will be performed.
        //
        // Testing:
        //   int maxSupportedBdexVersion() const;
        //   bdex_InStream& streamIn(bdex_InStream& stream);
        //   bdex_InStream& streamIn(bdex_InStream& stream, int version);
        //   bdex_OutStream& streamOut(bdex_OutStream& stream) const;
        //   bdex_OutStream& bdexStreamOut(bdex_OutStream& stream, int) const;
        // --------------------------------------------------------------------

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // scalar and array object values for various stream tests
        const Obj VU(AU, UU);
        const Obj VA(AA, XU);
        const Obj VB(AB, YU);
        const Obj VC(AC, LX);
        const Obj VD(AD, UU);
        const Obj VE(AE, XU);
        const Obj VF(AF, YU);
        const Obj VG(AG, LX);

        const int NUM_VALUES = 8;
        const Obj VALUES[NUM_VALUES] = { VU, VA, VB, VC, VD, VE, VF, VG };
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
            "\nTesting 'streamOut' and (valid) 'streamIn' functionality."
                          << endl;

        const int VERSION = Obj::maxSupportedBdexVersion();
        if (verbose) cout << "\nDirect initial trial of 'streamOut' and"
                             " (valid) 'streamIn' functionality." << endl;
        {
            const Obj X(VC);
            Out out;
            bdex_OutStreamFunctions::streamOut(out, X, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();
            In in(OD, LOD);
            in.setSuppressVersionCheck(1);
            ASSERT(in);                                 ASSERT(!in.isEmpty());

            Obj t(VA);                                  ASSERT(X != t);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }

        if (verbose) cout <<
            "\nThorough test of stream operators ('<<' and '>>')." << endl;
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);
                Out out;
                bdex_OutStreamFunctions::streamOut(out, X, VERSION);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value
                // and that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    In in(OD, LOD);  In &testInStream = in;
                    in.setSuppressVersionCheck(1);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, !in.isEmpty());

                    Obj t(VALUES[j]);
                    const Obj tt = t;   // control
                  BEGIN_BDEX_EXCEPTION_TEST {
                    in.reset();
                    LOOP2_ASSERT(i, j, X == tt == (i == j));
                    bdex_InStreamFunctions::streamIn(in, t, VERSION);
                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, in.isEmpty());
                  } END_BDEX_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout <<
            "\nTesting streamIn functionality via operator ('>>')." << endl;

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            Out out;
            const char *const  OD = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);  In &testInStream = in;
                in.setSuppressVersionCheck(1);
                LOOP_ASSERT(i, in);           LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                const Obj X(VALUES[i]);  Obj t(X);  LOOP_ASSERT(i, X == t);
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
              } END_BDEX_EXCEPTION_TEST
            }
        }

        if (verbose) cout <<
            "\tOn incomplete (but otherwise valid) data." << endl;
        {
            const Obj W1 = VA, X1 = VB, Y1 = VC;
            const Obj W2 = VB, X2 = VC, Y2 = VD;
            const Obj W3 = VC, X3 = VD, Y3 = VE;

            Out out;
            bdex_OutStreamFunctions::streamOut(out, X1, VERSION);
            const int LOD1 = out.length();
            bdex_OutStreamFunctions::streamOut(out, X2, VERSION);
            const int LOD2 = out.length();
            bdex_OutStreamFunctions::streamOut(out, X3, VERSION);
            const int LOD  = out.length();

            if (veryVerbose) { P_(LOD1); P_(LOD2); P(LOD); }

            const char *const OD = out.data();

            for (int i = 0; i < LOD; ++i) {
                In in(OD, i);  In &testInStream = in;
                in.setSuppressVersionCheck(1);
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
                Obj t1(W1), t2(W2), t3(W3);

                if (i < LOD1) {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i, !in);
                                       if (0 == i) { LOOP_ASSERT(i, W1 == t1);}
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                }
                else if (i < LOD2) {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);
                                 if (LOD1 == i)    { LOOP_ASSERT(i, W2 == t2);}
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                }
                else {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);
                                 if (LOD2 == i)    { LOOP_ASSERT(i, W3 == t3);}
                }

                                LOOP_ASSERT(i, Y1 != t1);
                t1 = Y1;        LOOP_ASSERT(i, Y1 == t1);

                                LOOP_ASSERT(i, Y2 != t2);
                t2 = Y2;        LOOP_ASSERT(i, Y2 == t2);

                                LOOP_ASSERT(i, Y3 != t3);
                t3 = Y3;        LOOP_ASSERT(i, Y3 == t3);
              } END_BDEX_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Obj W;               // default value
        const Obj X(AA, XU);       // original (control) value
        const Obj Y(AB, YU);       // new (streamed-out) value

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            Out out;
            Y.bdexStreamOut(out, 1);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(in);
                             ASSERT(W != t);  ASSERT(X != t);  ASSERT(Y == t);
        }

        if (verbose) cout << "\t\tBad version." << endl;
        {
            const int version = 0; // too small ('version' must be >= 1)

            Out out;
            Y.bdexStreamOut(out, 1);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        {
            const int version = 5 ; // too large (current Date version is 1)

            Out out;
            Y.bdexStreamOut(out, 1);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion' method."
                          << endl;
        {
            if (verbose) cout << "\tusing object syntax:" << endl;
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion());
            if (verbose) cout << "\tusing class method syntax:" << endl;
            ASSERT(1 == Obj::maxSupportedBdexVersion());
        }

#endif // TBD:
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S x S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   bdet_Date& operator=(const bdet_Date& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Assignment Operator"
                          << "\n===========================" << endl;
        {
            const int NUM_RECATTRS = sizeof REC_ATTRS / sizeof REC_ATTRS[0];
            const int NUM_LISTS = sizeof G_VALUES_U / sizeof G_VALUES_U[0];

            int i;
            for (i = 0; i < NUM_RECATTRS; ++i) {
                Obj v;  const Obj& V = v;
                int ii = i % NUM_LISTS;
                v.setFixedFields(REC_ATTRS[i]);
                v.setUserFields(*G_VALUES_U[ii]);
                for (int j = 0; j < NUM_RECATTRS; ++j) {
                    Obj u;  const Obj& U = u;
                    int jj = j % NUM_LISTS;
                    u.setFixedFields(REC_ATTRS[j]);
                    u.setUserFields(*G_VALUES_U[jj]);
                    if (veryVerbose) { T_();  P_(V);  P_(U); }
                    Obj w(V);  const Obj &W = w;          // control
                    u = V;
                    if (veryVerbose) P(U);
                    LOOP2_ASSERT(i, j, W == U);
                    LOOP2_ASSERT(i, j, W == V);
                }
            }

            if (verbose) cout << "\nTesting assignment u = u (Aliasing)."
                              << endl;

            for (i = 0; i < NUM_RECATTRS; ++i) {
                Obj u;  const Obj& U = u;
                int ii = i % NUM_LISTS;
                u.setFixedFields(REC_ATTRS[i]);
                u.setUserFields(*G_VALUES_U[ii]);
                Obj w(U);  const Obj &W = w;              // control
                u = u;
                if (veryVerbose) { T_();  P_(U);  P(W); }
                LOOP_ASSERT(i, W == U);
            }
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using tested
        //   methods.  Then copy construct and object y from x, and use the
        //   equality operator to assert that both x and y have the same value
        //   as w.
        //
        // Testing:
        //   bael_Record(const bael_Record&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;
        {
            const int NUM_RECATTRS = sizeof REC_ATTRS / sizeof REC_ATTRS[0];
            const int NUM_LISTS = sizeof G_VALUES_U / sizeof G_VALUES_U[0];
            for (int i = 0; i < NUM_RECATTRS; ++i) {
                Obj w;  const Obj& W = w;           // control
                int j = i % NUM_LISTS;
                w.setFixedFields(REC_ATTRS[i]);
                w.setUserFields(*G_VALUES_U[j]);

                Obj x;  const Obj& X = x;
                x.setFixedFields(REC_ATTRS[i]);
                x.setUserFields(*G_VALUES_U[j]);

                Obj y(X);  const Obj &Y = y;
                if (veryVerbose) { T_();  P_(W);  P_(X);  P(Y); }
                LOOP_ASSERT(i, X == W);  LOOP_ASSERT(i, Y == W);
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //   Any subtle variation in value must be detected by the equality
        //   operators.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product
        //   S X S
        //
        // Testing:
        //   operator==(const bael_Record&, const bael_Record&);
        //   operator!=(const bael_Record&, const bael_Record&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Equality Operators"
                          << "\n==========================" << endl;
        if (verbose) cout <<
            "\nCompare each pair of values (u, v) in S X S." << endl;
        {

            for (int i = 0; i < 3; ++i) {
                Obj u;  const Obj& U = u;
                u.setFixedFields(REC_ATTRS[i]);
                u.setUserFields(*G_VALUES_U[i]);
                for (int j = 0; j < 3; ++j) {
                    Obj v;  const Obj& V = v;
                    v.setFixedFields(REC_ATTRS[j]);
                    v.setUserFields(*G_VALUES_U[j]);
                    int isSame = i == j;
                    if (veryVerbose) { T_();  P_(i);  P_(j);  P_(U);  P(V); }
                    LOOP2_ASSERT(i, j,  isSame == (U == V));
                    LOOP2_ASSERT(i, j, !isSame == (U != V));
                    LOOP2_ASSERT(i, j,  isSame == (V == U));
                    LOOP2_ASSERT(i, j, !isSame == (V != U));
                }
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        //
        // Plan:  BUG
        //
        // Testing:
        //   operator<<(ostream&, const bael_Record&);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing Output (<<) Operator" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //
        //   We want to verify that each individual field is returned
        //   correctly.
        //
        // Plan:
        //   For a sequence of unique object values, verify that each of the
        //   basic accessors returns the correct value.
        //
        // Testing:
        //   const bael_RecordAttributes& fixedFields();
        //   const bdem_List& userFields();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing Basic Accessors" << endl
            << "=======================" << endl;

        const char *SPECS[]  = {
            "xA", "xAyA", "xAxB", "xAxU", "xUxA", "xAxBxC", "xGxHxI",
            "xAxBxCxD", "xAxDxCxBxE", "xAxBxCxDxExF",
            "xMxNxOxPxQxRxSxTxUxVxAxBxCxDxExFxGxHxIxJxKxL"
        };
        const int NUM_SPECS  = sizeof(SPECS) / sizeof(*SPECS);

        for (int ii = 0; ii < NUM_SPECS; ++ii) {
            List mX;  const List& X = mX;  // control
            gg(&mX, SPECS[ii]);

            if (verbose) { cout << "\t"; P_(SPECS[ii]); P(X); }
            for (int jj = 0; jj < NUM_ATTRS; ++jj) {

                const Record_Attr& Y = REC_ATTRS[jj];

                Obj mT(Y, mX, &testAllocator);

                if (veryVeryVerbose) { P(mT); }
                LOOP2_ASSERT(ii, jj, mT.fixedFields() == Y);
                LOOP2_ASSERT(ii, jj, mT.userFields()  == X);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS 'GG' AND 'GGG' ('bdem_List')
        // Plan:
        //   Test the behavior of 'gg' and 'ggg'.
        //   * Verify that all test values are distinct.
        //   * Show 'gg' returns a correct reference to its first argument.
        //   * Enumerate representative input, ordered by length.
        //   * Show the correct accessing of all 66 unique values (of 22
        //     unique types) that are a part of the 'gg' language.
        //   * Test large legal input strings.
        //   * Test 'ggg' syntax checking with syntactically illegal inputs.
        //
        // Testing:
        //   bdem_List& gg(bdem_List *address, const char *spec);
        //   int ggg(bdem_List *address, const char *spec, int showErrorFlag);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
            << "Testing 'gg' and 'ggg' generator functions" << endl
            << "==========================================" << endl;

        if (verbose) cout <<
            "\nTesting that all test values are distinct." << endl;

        ASSERT(0 == (XA == UA));
        ASSERT(0 == (XA == YA));
        ASSERT(0 == (UA == YA));
        ASSERT(0 == (XB == UB));
        ASSERT(0 == (XB == YB));
        ASSERT(0 == (UB == YB));
        ASSERT(0 == (XC == UC));
        ASSERT(0 == (XC == YC));
        ASSERT(0 == (UC == YC));
        ASSERT(0 == (XD == UD));
        ASSERT(0 == (XD == YD));
        ASSERT(0 == (UD == YD));
        ASSERT(0 == (XE == UE));
        ASSERT(0 == (XE == YE));
        ASSERT(0 == (UE == YE));
        ASSERT(0 == (XF == UF));
        ASSERT(0 == (XF == YF));
        ASSERT(0 == (UF == YF));
        ASSERT(0 == (XG == UG));
        ASSERT(0 == (XG == YG));
        ASSERT(0 == (UG == YG));
        ASSERT(0 == (XH == UH));
        ASSERT(0 == (XH == YH));
        ASSERT(0 == (UH == YH));
        ASSERT(0 == (XI == UI));
        ASSERT(0 == (XI == YI));
        ASSERT(0 == (UI == YI));
        ASSERT(0 == (XJ == UJ));
        ASSERT(0 == (XJ == YJ));
        ASSERT(0 == (UJ == YJ));
        ASSERT(0 == (XK == UK));
        ASSERT(0 == (XK == YK));
        ASSERT(0 == (UK == YK));
        ASSERT(0 == (XL == UL));
        ASSERT(0 == (XL == YL));
        ASSERT(0 == (UL == YL));
        ASSERT(0 == (XM == UM));
        ASSERT(0 == (XM == YM));
        ASSERT(0 == (UM == YM));
        ASSERT(0 == (XN == UN));
        ASSERT(0 == (XN == YN));
        ASSERT(0 == (UN == YN));
        ASSERT(0 == (XO == UO));
        ASSERT(0 == (XO == YO));
        ASSERT(0 == (UO == YO));
        ASSERT(0 == (XP == UP));
        ASSERT(0 == (XP == YP));
        ASSERT(0 == (UP == YP));
        ASSERT(0 == (XQ == UQ));
        ASSERT(0 == (XQ == YQ));
        ASSERT(0 == (UQ == YQ));
        ASSERT(0 == (XR == UR));
        ASSERT(0 == (XR == YR));
        ASSERT(0 == (UR == YR));
        ASSERT(0 == (XS == US));
        ASSERT(0 == (XS == YS));
        ASSERT(0 == (US == YS));
        ASSERT(0 == (XT == UT));
        ASSERT(0 == (XT == YT));
        ASSERT(0 == (UT == YT));
        ASSERT(0 == (XU == UU));
        ASSERT(0 == (XU == YU));
        ASSERT(0 == (UU == YU));
        ASSERT(0 == (XV == UV));
        ASSERT(0 == (XV == YV));
        ASSERT(0 == (UV == YV));

        if (verbose) cout <<
            "\nTesting 'gg' (list) generator function:" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec;     // input 'spec' string for 'gg'
            const char *d_types;    // expected element types (encoded)
            int         d_length;   // expected length of returned list
        } DATA[] = {
            ///line          spec    elemType  length
            ///----          ----    --------  ------
            {   L_,            "",      "",    0, },
            {   L_,           "~",      "",    0, },
            {   L_,          "~~",      "",    0, },

            {   L_,          "xA",     "A",    1, },
            {   L_,          "yB",     "B",    1, },
            {   L_,          "uC",     "C",    1, },
            {   L_,          "xD",     "D",    1, },
            {   L_,          "yE",     "E",    1, },
            {   L_,          "uF",     "F",    1, },
            {   L_,          "xG",     "G",    1, },
            {   L_,          "yH",     "H",    1, },
            {   L_,          "uI",     "I",    1, },
            {   L_,          "xJ",     "J",    1, },
            {   L_,          "yK",     "K",    1, },
            {   L_,          "uL",     "L",    1, },
            {   L_,          "xM",     "M",    1, },
            {   L_,          "yN",     "N",    1, },
            {   L_,          "uO",     "O",    1, },
            {   L_,          "xP",     "P",    1, },
            {   L_,          "yQ",     "Q",    1, },
            {   L_,          "uR",     "R",    1, },
            {   L_,          "xS",     "S",    1, },
            {   L_,          "yT",     "T",    1, },
            {   L_,          "uU",     "U",    1, },
            {   L_,          "xV",     "V",    1, },

            {   L_,         "xB~",      "",    0, },
            {   L_,         "~uC",     "C",    1, },
            {   L_,         "~xD",     "D",    1, },

            {   L_,        "~~yF",     "F",    1, },
            {   L_,        "uG~~",      "",    0, },
            {   L_,        "yHuI",    "HI",    2, },

            {   L_,       "~yUuV",    "UV",    2, },
            {   L_,       "xB~uA",     "A",    1, },
            {   L_,       "yBuC~",      "",    0, },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\tEnumerated 'spec' ordered by length." << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE = DATA[i].d_lineNum;
            List mL(Z);
            const List& RL = gg(&mL, DATA[i].d_spec);
            my_ElemTypeArray e;  appendToElemTypeArray(&e, DATA[i].d_types);
            LOOP_ASSERT(LINE, &RL == &mL);    // Test 'gg' return value
            my_ElemTypeArray e1;
            getElemTypes(&e1, mL.row());
            LOOP_ASSERT(LINE, e == e1);
            LOOP_ASSERT(LINE, mL.length() == DATA[i].d_length);
        }

        // Testing all possible one-element lists (that 'gg' can generate),
        // Note that this tests type as well as value.
        if (verbose) cout << "\tAll available one-element lists." << endl;
        List mL(Z);
        ASSERT( gg(&mL, "~xA").theChar(0) == XA );
        ASSERT( gg(&mL, "~yA").theChar(0) == YA );
        ASSERT( gg(&mL, "~uA").theChar(0) == UA );

        ASSERT( gg(&mL, "~xB").theShort(0) == XB );
        ASSERT( gg(&mL, "~yB").theShort(0) == YB );
        ASSERT( gg(&mL, "~uB").theShort(0) == UB );

        ASSERT( gg(&mL, "~xC").theInt(0) == XC );
        ASSERT( gg(&mL, "~yC").theInt(0) == YC );
        ASSERT( gg(&mL, "~uC").theInt(0) == UC );

        ASSERT( gg(&mL, "~xD").theInt64(0) == XD );
        ASSERT( gg(&mL, "~yD").theInt64(0) == YD );
        ASSERT( gg(&mL, "~uD").theInt64(0) == UD );

        ASSERT( gg(&mL, "~xE").theFloat(0) == XE );
        ASSERT( gg(&mL, "~yE").theFloat(0) == YE );
        ASSERT( gg(&mL, "~uE").theFloat(0) == UE );

        ASSERT( gg(&mL, "~xF").theDouble(0) == XF );
        ASSERT( gg(&mL, "~yF").theDouble(0) == YF );
        ASSERT( gg(&mL, "~uF").theDouble(0) == UF );

        ASSERT( gg(&mL, "~xG").theString(0) == XG );
        ASSERT( gg(&mL, "~yG").theString(0) == YG );
        ASSERT( gg(&mL, "~uG").theString(0) == UG );

        ASSERT( gg(&mL, "~xH").theDatetime(0) == XH );
        ASSERT( gg(&mL, "~yH").theDatetime(0) == YH );
        ASSERT( gg(&mL, "~uH").theDatetime(0) == UH );

        ASSERT( gg(&mL, "~xI").theDate(0) == XI );
        ASSERT( gg(&mL, "~yI").theDate(0) == YI );
        ASSERT( gg(&mL, "~uI").theDate(0) == UI );

        ASSERT( gg(&mL, "~xJ").theTime(0) == XJ );
        ASSERT( gg(&mL, "~yJ").theTime(0) == YJ );
        ASSERT( gg(&mL, "~uJ").theTime(0) == UJ );

        ASSERT( gg(&mL, "~xK").theCharArray(0) == XK );
        ASSERT( gg(&mL, "~yK").theCharArray(0) == YK );
        ASSERT( gg(&mL, "~uK").theCharArray(0) == UK );

        ASSERT( gg(&mL, "~xL").theShortArray(0) == XL );
        ASSERT( gg(&mL, "~yL").theShortArray(0) == YL );
        ASSERT( gg(&mL, "~uL").theShortArray(0) == UL );

        ASSERT( gg(&mL, "~xM").theIntArray(0) == XM );
        ASSERT( gg(&mL, "~yM").theIntArray(0) == YM );
        ASSERT( gg(&mL, "~uM").theIntArray(0) == UM );

        ASSERT( gg(&mL, "~xN").theInt64Array(0) == XN );
        ASSERT( gg(&mL, "~yN").theInt64Array(0) == YN );
        ASSERT( gg(&mL, "~uN").theInt64Array(0) == UN );

        ASSERT( gg(&mL, "~xO").theFloatArray(0) == XO );
        ASSERT( gg(&mL, "~yO").theFloatArray(0) == YO );
        ASSERT( gg(&mL, "~uO").theFloatArray(0) == UO );

        ASSERT( gg(&mL, "~xP").theDoubleArray(0) == XP );
        ASSERT( gg(&mL, "~yP").theDoubleArray(0) == YP );
        ASSERT( gg(&mL, "~uP").theDoubleArray(0) == UP );

        ASSERT( gg(&mL, "~xQ").theStringArray(0) == XQ );
        ASSERT( gg(&mL, "~yQ").theStringArray(0) == YQ );
        ASSERT( gg(&mL, "~uQ").theStringArray(0) == UQ );

        ASSERT( gg(&mL, "~xR").theDatetimeArray(0) == XR );
        ASSERT( gg(&mL, "~yR").theDatetimeArray(0) == YR );
        ASSERT( gg(&mL, "~uR").theDatetimeArray(0) == UR );

        ASSERT( gg(&mL, "~xS").theDateArray(0) == XS );
        ASSERT( gg(&mL, "~yS").theDateArray(0) == YS );
        ASSERT( gg(&mL, "~uS").theDateArray(0) == US );

        ASSERT( gg(&mL, "~xT").theTimeArray(0) == XT );
        ASSERT( gg(&mL, "~yT").theTimeArray(0) == YT );
        ASSERT( gg(&mL, "~uT").theTimeArray(0) == UT );

        ASSERT( gg(&mL, "~xU").theList(0) == XU );
        ASSERT( gg(&mL, "~yU").theList(0) == YU );
        ASSERT( gg(&mL, "~uU").theList(0) == UU );

        ASSERT( gg(&mL, "~xV").theTable(0) == XV );
        ASSERT( gg(&mL, "~yV").theTable(0) == YV );
        ASSERT( gg(&mL, "~uV").theTable(0) == UV );

        if (verbose) cout << "\tLarge 'spec'." << endl;
        const int NUM_PASSES = 10;
        const int SPEC_SIZE  = 44 * NUM_PASSES + 31;
        char bigSpec[SPEC_SIZE] = "~~xDxCxBxA~~yDyCyByA~uDuCuBuA~";
        for (int j = 0; j < NUM_PASSES; ++j) {
            strcat(bigSpec, "xAxBxCxDxExFxGxHxIxJxKxLxMxNxOxPxQxRxSxTxUxV");
            gg(&mL, bigSpec);
            LOOP_ASSERT( j, 22 * (j + 1) == mL.length() );
            LOOP_ASSERT( j, XA  == mL.theChar(22 * j) );
            LOOP_ASSERT( j, XV  == mL.theTable(22 * (j + 1) - 1) );
        }

        if (verbose) cout << "\nTesting error reporting in 'ggg'." << endl;
        static const struct {
            int         d_lineNum;   // source line number
            const char *d_spec;      // input 'spec' string for 'gg'
            int         d_errorCode; // expected 'enum' error code
        } E_DATA[] = {
            // line    spec                error code
            // ----    ----                ----------
            {   L_,    "."             ,   GGG_INAPPROPRIATE_CHARACTER, },
            {   L_,    ","             ,   GGG_INAPPROPRIATE_CHARACTER, },
            {   L_,    "z"             ,   GGG_INAPPROPRIATE_CHARACTER, },
            {   L_,    ";"             ,   GGG_INAPPROPRIATE_CHARACTER, },
            {   L_,    "A"             ,   GGG_INAPPROPRIATE_CHARACTER, },

            {   L_,    "Ax"            ,   GGG_INAPPROPRIATE_CHARACTER, },
            {   L_,    "A~"            ,   GGG_INAPPROPRIATE_CHARACTER, },

            {   L_,    "Ax;"           ,   GGG_INAPPROPRIATE_CHARACTER, },
            {   L_,    "Axx"           ,   GGG_INAPPROPRIATE_CHARACTER, },

            {   L_,    "x~"            ,   GGG_EXPECTED_TYPE_CHARACTER, },
            {   L_,    "x;"            ,   GGG_EXPECTED_TYPE_CHARACTER, },
            {   L_,    "xx"            ,   GGG_EXPECTED_TYPE_CHARACTER, },

            {   L_,    "xAx~"          ,   GGG_EXPECTED_TYPE_CHARACTER, },
            {   L_,    "xBxx"          ,   GGG_EXPECTED_TYPE_CHARACTER, },
            {   L_,    "xxxC"          ,   GGG_EXPECTED_TYPE_CHARACTER, },
            {   L_,    "x;xD"          ,   GGG_EXPECTED_TYPE_CHARACTER, },

            {   L_,    ""              ,   GGG_SUCCESS,                 },
            {   L_,    "~xAyB~xCyDuE~" ,   GGG_SUCCESS,                 }
        };
        int numEData = sizeof E_DATA / sizeof E_DATA[0]; // E_DATA[] length
        for (int k = 0; k < numEData; ++k) {
            const int LINE = E_DATA[k].d_lineNum;
            List mL(Z);
            const int SHOW_ERRORS = veryVerbose;
            const int ERROR_CODE  = ggg(&mL, E_DATA[k].d_spec, SHOW_ERRORS);
            LOOP_ASSERT(LINE, ERROR_CODE == E_DATA[k].d_errorCode);
        }

        // verify the operation of the areEqual helper function.

        if (verbose) cout << "\nTesting 'areEqual' helper function:" << endl;
        static const struct {
            int          d_lineNum;  // source line number
            const char  *d_lspec;    // lhs spec
            int          d_lpos;     // lhs position
            const char  *d_rspec;    // rhs spec
            int          d_rpos;     // rhs position
            int          d_result;   // result
        } Q_DATA[] = {
            {  L_,  "xA",         0,     "xA",          0,     1     },
            {  L_,  "xA",         0,     "yA",          0,     0     },
            {  L_,  "xA",         0,     "uA",          0,     0     },
            {  L_,  "uA",         0,     "xA",          0,     0     },
            {  L_,  "xA",         0,     "xB",          0,     0     },
            {  L_,  "xB",         0,     "xB",          0,     1     },
            {  L_,  "xC",         0,     "xC",          0,     1     },
            {  L_,  "xD",         0,     "xD",          0,     1     },
            {  L_,  "xE",         0,     "xE",          0,     1     },
            {  L_,  "xAxA",       0,     "xAxB",        0,     1     },
            {  L_,  "xAxA",       0,     "xAxB",        1,     0     },
            {  L_,  "xAxA",       1,     "xAxB",        0,     1     },
            {  L_,  "xAxA",       1,     "xAxB",        1,     0     },
            {  L_,  "xBxA",       0,     "xAxB",        0,     0     },
            {  L_,  "xBxA",       0,     "xAxB",        1,     1     },
            {  L_,  "xBxA",       1,     "xAxB",        0,     1     },
            {  L_,  "xBxA",       1,     "xAxB",        1,     0     },
            {  L_,  "xAxA",       0,     "xAyA",        0,     1     },
            {  L_,  "xAxA",       0,     "xAyA",        1,     0     },
            {  L_,  "xAxA",       1,     "xAyA",        0,     1     },
            {  L_,  "xAxA",       1,     "xAyA",        1,     0     },
            {  L_,  "yAxA",       0,     "xAyA",        0,     0     },
            {  L_,  "yAxA",       0,     "xAyA",        1,     1     },
            {  L_,  "yAxA",       1,     "xAyA",        0,     1     },
            {  L_,  "yAxA",       1,     "xAyA",        1,     0     },
            {  L_,  "xAxBxC",     0,     "xCxAxB",      0,     0     },
            {  L_,  "xAxBxC",     0,     "xCxAxB",      1,     1     },
            {  L_,  "xAxBxC",     2,     "xCxAxB",      0,     1     },
            {  L_,  "xAxBxC",     1,     "xCxAxB",      2,     1     },
            {  L_,  "xAxBxC",     2,     "xCxAxB",      2,     0     },
        };

        int numQData = sizeof Q_DATA / sizeof Q_DATA[0]; // Q_DATA[] length
        for (int m = 0; m < numQData; ++m) {
            const int LINE = Q_DATA[m].d_lineNum;

            List mL(Z);
            gg(&mL, Q_DATA[m].d_lspec);
            List mR(Z);
            gg(&mR, Q_DATA[m].d_rspec);

            LOOP_ASSERT(LINE, areEqual (mL.row(), Q_DATA[m].d_lpos,
                                        mR.row(), Q_DATA[m].d_rpos) ==
                                        Q_DATA[m].d_result);
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //
        //   We want to verify the constructor and the primary manipulators set
        //   the member fields correctly.
        //
        // Plan:
        //   Create three bael_RecordAttributes and bdem_List objects with
        //   different values - one unset value and two distinct values.
        //   Construct bael_Record objects with these values and with default
        //   constructor, verify the values with the basic accessors, verify
        //   the equality and inequality of these objects.  Use the primary
        //   manipulators to set values, verify the values are correctly set.
        //   The destructor is exercised as the objects being tested leave
        //   scope.
        //
        // Testing:
        //   bael_Record(const bael_RecordAttributes &, const bdem_List &);
        //   bael_Record();
        //   void setFixedFields(const bael_RecordAttributes &);
        //   void setUserFields(const bdem_List &);
        //   ~bael_Record();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Primary Manipulators" << endl
                                  << "============================" << endl;

        Record_Attr FA;
        Record_Attr FB(bdet_Datetime(2004, 1, 21, 12, 30, 25, 150),
                       getPid(),
                       0,  // threadID
                       __FILE__,
                       __LINE__,
                       "MyCategory1",
                       bael_Severity::BAEL_INFO,
                       "Distinct Message 1");
        Record_Attr FC(bdet_Datetime(2000, 2, 29, 10, 13, 55, 111),
                       getPid(),
                       0,  // threadID
                       __FILE__,
                       __LINE__,
                       "MyCategory2",
                       bael_Severity::BAEL_ERROR,
                       "Distinct Message 2");

        if (verbose) cout << "\nTesting default ctor (thoroughly)." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Obj X((bslma_Allocator *)0);
            if (veryVerbose) { cout << "\t\t"; P(X); }
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
#if !defined(BSLS_PLATFORM_CMP_MSVC) || defined(BDE_BUILD_TARGET_OPT)
          BEGIN_BSLMA_EXCEPTION_TEST {
#endif
            if (veryVerbose) cout <<
                "\tTesting Exceptions In Default Ctor" << endl;
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
#if !defined(BSLS_PLATFORM_CMP_MSVC) || defined(BDE_BUILD_TARGET_OPT)
          } END_BSLMA_EXCEPTION_TEST
#endif
        }

        if (verbose) cout << "\nTesting ctor (thoroughly)." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Obj X(FB, XU, (bslma_Allocator *)0);
            if (veryVerbose) { cout << "\t\t"; P(X); }
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Obj X(FB, XU, &testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
#if !defined(BSLS_PLATFORM_CMP_MSVC) || defined(BDE_BUILD_TARGET_OPT)
          BEGIN_BSLMA_EXCEPTION_TEST {
#endif
            if (veryVerbose) cout <<
                "\tTesting Exceptions In Ctor" << endl;
            const Obj X(FB, XU, &testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
#if !defined(BSLS_PLATFORM_CMP_MSVC) || defined(BDE_BUILD_TARGET_OPT)
          } END_BSLMA_EXCEPTION_TEST
#endif
        }
        if (verbose) cout << "\nTesting primary manipulator." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            Obj mX;
            Obj mY(FB, XU);
            Obj mZ(FC, YU);

            if (veryVeryVerbose) { P(mX); P(mY); P(mZ); }

            ASSERT(FA != FB); ASSERT(FA != FC); ASSERT(FC != FB);
            ASSERT(0 == (mX == mY)); ASSERT(0 == (mX == mZ));
            ASSERT(0 == (mY == mZ));

            ASSERT(mX.fixedFields() == FA);    ASSERT(mX.userFields()  == UU);
            ASSERT(mY.fixedFields() == FB);    ASSERT(mY.userFields()  == XU);
            ASSERT(mZ.fixedFields() == FC);    ASSERT(mZ.userFields()  == YU);

            if (veryVerbose) cout << "\tSetting mX with mY's initializer."
                            << endl;
            mX.setFixedFields(FB);             ASSERT(mX.fixedFields() == FB);
            mX.setUserFields(XU);              ASSERT(mX.userFields() == XU);
            ASSERT(mX == mY);

            if (veryVerbose) cout << "\tSetting mX with mZ's initializer."
                            << endl;
            mX.setFixedFields(FC);             ASSERT(mX.fixedFields() == FC);
            mX.setUserFields(YU);              ASSERT(mX.userFields()  == YU);
            ASSERT(mX == mZ);
        }

        if (verbose) cout << "\tWith an allocator." << endl;
        if (verbose) cout << "\t\tWithout exceptions." << endl;
        {
            bslma_TestAllocator testAllocatorX(veryVeryVerbose);
            bslma_TestAllocator testAllocatorY(veryVeryVerbose);
            bslma_TestAllocator testAllocatorZ(veryVeryVerbose);
            Obj mX(&testAllocatorX);
            Obj mY(FB, XU, &testAllocatorY);
            Obj mZ(FC, YU, &testAllocatorZ);

            if (veryVeryVerbose) { P(mX); P(mY); P(mZ); }

            ASSERT(FA != FB); ASSERT(FA != FC); ASSERT(FC != FB);
            ASSERT(0 == (mX == mY)); ASSERT(0 == (mX == mZ));
            ASSERT(0 == (mY == mZ));

            ASSERT(mX.fixedFields() == FA);    ASSERT(mX.userFields()  == UU);
            ASSERT(mY.fixedFields() == FB);    ASSERT(mY.userFields()  == XU);
            ASSERT(mZ.fixedFields() == FC);    ASSERT(mZ.userFields()  == YU);

            if (veryVerbose) cout << "\tSetting mX with mY's initializer."
                            << endl;
            mX.setFixedFields(FB);             ASSERT(mX.fixedFields() == FB);
            mX.setUserFields(XU);              ASSERT(mX.userFields() == XU);
            ASSERT(mX == mY);

            if (veryVerbose) cout << "\tSetting mX with mZ's initializer."
                            << endl;
            mX.setFixedFields(FC);             ASSERT(mX.fixedFields() == FC);
            mX.setUserFields(YU);              ASSERT(mX.userFields()  == YU);
            ASSERT(mX == mZ);
        }
        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            bslma_TestAllocator testAllocatorX(veryVeryVerbose);
            bslma_TestAllocator testAllocatorY(veryVeryVerbose);
            bslma_TestAllocator testAllocatorZ(veryVeryVerbose);
#if !defined(BSLS_PLATFORM_CMP_MSVC) || defined(BDE_BUILD_TARGET_OPT)
          BEGIN_BSLMA_EXCEPTION_TEST {
#endif
            if (veryVerbose) cout <<
                "\tTesting Exceptions In Primary Manipulator" << endl;
            Obj mX(&testAllocatorX);
            Obj mY(FB, XU, &testAllocatorY);
            Obj mZ(FC, YU, &testAllocatorZ);

            if (veryVeryVerbose) { P(mX); P(mY); P(mZ); }

            ASSERT(FA != FB); ASSERT(FA != FC); ASSERT(FC != FB);
            ASSERT(0 == (mX == mY)); ASSERT(0 == (mX == mZ));
            ASSERT(0 == (mY == mZ));

            ASSERT(mX.fixedFields() == FA);    ASSERT(mX.userFields()  == UU);
            ASSERT(mY.fixedFields() == FB);    ASSERT(mY.userFields()  == XU);
            ASSERT(mZ.fixedFields() == FC);    ASSERT(mZ.userFields()  == YU);

            if (veryVerbose) cout << "\tSetting mX with mY's initializer."
                            << endl;
            mX.setFixedFields(FB);             ASSERT(mX.fixedFields() == FB);
            mX.setUserFields(XU);              ASSERT(mX.userFields() == XU);
            ASSERT(mX == mY);

            if (veryVerbose) cout << "\tSetting mX with mZ's initializer."
                            << endl;
            mX.setFixedFields(FC);             ASSERT(mX.fixedFields() == FC);
            mX.setUserFields(YU);              ASSERT(mX.userFields()  == YU);
            ASSERT(mX == mZ);
#if !defined(BSLS_PLATFORM_CMP_MSVC) || defined(BDE_BUILD_TARGET_OPT)
          } END_BSLMA_EXCEPTION_TEST
#endif
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   We want to exercise basic value-semantic functionality.  In
        //   particular, we want to demonstrate a base-line level of correct
        //   operation of the following methods and operators:
        //     - constructor/destructor
        //     - assignment operator
        //     - userFields()/fixedFields() (non-const versions)
        //     - userFields()/fixedFields() (const versions)
        //     - setUserFields()/setFixedFields()
        //     - numAllocatedBytes()
        //     - equality/inequality operator (== and !=)
        //     - etc.
        //
        // Plan:
        //   Create list FA and FB with different schemas.  Create record
        //   attributes UA and UB with different values.  Instantiate test
        //   objects RA from FA and UA and RB from FB and UB.  Use accessors
        //   to verify the correct values.  Copy construct RC from RA, verify
        //   their equality.  Use manipulators to set RA to values hold by RB,
        //   verify RA's inequality with RC, and equality with RB.  Reset RA
        //   original values.  Get the dynamic memory usage.  To test
        //   non-const versions of the 'userFields' and 'fixedFields' methods,
        //   create a record RA from FA and XU, get modifiables references
        //   to fixed field and user field and assign them FB and YU and
        //   finally verify that record is appropriately modified.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.

        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        Record_Attr FA(bdet_Datetime(),
                       getPid(),
                       0,  // threadID
                       __FILE__, __LINE__,
                       "EQUITY.NASD",
                       bael_Severity::BAEL_INFO,
                       "Ticker Summary");
        bdet_Datetime now;
        bdetu_Datetime::convertFromTimeT(&now, time(0));
        FA.setTimestamp(now);

        if (verbose) { P(FA) ; }

        Obj RA(FA, XU);
        ASSERT(FA == RA.fixedFields()); ASSERT(XU == RA.userFields());

        Record_Attr FB(bdet_Datetime(),
                       getPid(),
                       0,  // threadID
                       __FILE__, __LINE__,
                       "USER_SESSION",
                       bael_Severity::BAEL_TRACE,
                       "User Session Info");

        bdetu_Datetime::convertFromTimeT(&now, time(0));
        FB.setTimestamp(now);

        if (verbose) { P(FB) ; }

        Obj RB(FB, YU);
        ASSERT(FB == RB.fixedFields()); ASSERT(YU == RB.userFields());

        Obj RC(RA);
        ASSERT(RC == RA);               ASSERT(RC != RB);

        RA.setFixedFields(FB);
        RA.setUserFields(YU);
        ASSERT(FB == RA.fixedFields()); ASSERT(YU == RA.userFields());
        ASSERT(RA == RB);               ASSERT(RA != RC);

        RA.setFixedFields(FA);
        RA.setUserFields(XU);
        ASSERT(FA == RA.fixedFields()); ASSERT(XU == RA.userFields());
        ASSERT(RA != RB);               ASSERT(RA == RC);

        RC = RB;
        ASSERT(FB == RC.fixedFields()); ASSERT(YU == RC.userFields());
        ASSERT(RA != RC);               ASSERT(RB == RC);

        const int sizea = RA.numAllocatedBytes();
        const int sizeb = RB.numAllocatedBytes();
        if (verbose) { P_(sizea); P(sizeb); }
        ASSERT(0 < sizea);              ASSERT(0 < sizeb);

        {
            if (verbose) {
                cout << "testing const versions of 'fixedFields'"
                     << "and 'userFields'"
                     << endl;
            }

            Obj RA(FA, XU);
            Record_Attr &ff = RA.fixedFields();
            ff = FB;
            List &uf = RA.userFields();
            uf = YU;
            const Obj &CRA = RA;
            LOOP_ASSERT(CRA.fixedFields(), CRA.fixedFields() == FB);
            LOOP_ASSERT(CRA.userFields(), CRA.userFields() == YU);
            if (verbose) {
                cout << "tested const versions of 'fixedFields'"
                     << "and 'userFields'"
                     << endl;
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
