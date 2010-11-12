// bdem_elemtype.t.cpp          -*-C++-*-

#include <bdem_elemtype.h>

#include <bdex_testinstream.h>           // for testing only
#include <bdex_testinstreamexception.h>  // for testing only
#include <bdex_testoutstream.h>          // for testing only

#include <bsl_iostream.h>
#include <bsl_strstream.h>

#include <bsl_cstdlib.h>   // atoi()
#include <bsl_cstring.h>   // strcmp(), memcmp() memcpy()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Standard enumeration test plan.
//-----------------------------------------------------------------------------
// 'bdem_ElemType' public interface:
// [ 1] enum Type { ... };
// [ 1] enum { BDEM_NUM_TYPES = ... };
// [ 1] char *toAscii(Type value);
// [ 2] int maxSupportedBdexVersion
// [ 3] bdexStreamOut(bdex_OutStream& s, bdem_ElemType::Type rhs, int ver);
// [ 3] bdexStreamIn(bdex_InStream& s, bdem_ElemType::Type& rhs, int ver);
// [ 3] bdexStreamOut(bdex_OutStream& s, bdem_ElemType::Type rhs);
// [ 3] bdexStreamIn(bdex_InStream& s, bdem_ElemType::Type& rhs);
// [ 3] operator<<(bdex_OutStream&, bdem_ElemType::Type rhs);
// [ 3] operator>>(bdex_InStream&, bdem_ElemType::Type& rhs);
// [ 4] bdem_ElemType::Type toArrayType(bdem_ElemType::Type value);
// [ 4] bdem_ElemType::Type fromArrayType(bdem_ElemType::Type value);
// [ 5] bool isArrayType(bdem_ElemType::Type value);
// [ 5] bool isAggregateType(bdem_ElemType::Type value);
// [ 5] bool isChoiceType(bdem_ElemType::Type value);
// [ 5] bool isScalarType(bdem_ElemType::Type value);
// [ 6] USAGE EXAMPLE
//
// 'bdem_ElemType' private methods (tested indirectly):
// [ 1] print(bsl::ostream& stream, bdem_ElemType::Type rhs);

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { P_(I) P(J) aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { P_(I) P_(J) P(K) aSsErT(1, #X, __LINE__); } }

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

typedef bdem_ElemType      Et;
typedef Et::Type           Enum;
typedef bdex_TestInStream  In;
typedef bdex_TestOutStream Out;
const int                  NUM_REAL_TYPES   = Et::BDEM_NUM_TYPES;
const int                  NUM_PSEUDO_TYPES = Et::BDEM_NUM_PSEUDO_TYPES;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:
      case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  Suppress
        //   all 'cout' statements in non-verbose mode, and add streaming to
        //   a buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Examples"
                          << "\n======================" << endl;

// The following snippets of code provide a simple illustration of
// 'bdem_ElemType' operation.
//
// First, create a variable 'elemType' of type 'bdem_ElemType::Type' and
// initialize it to the value 'bdem_ElemType::BDEM_DATETIME':
//..
  bdem_ElemType::Type elemType = bdem_ElemType::BDEM_DATETIME;
//..
// Next, store its string representation in a variable named 'rep' of type
// 'const char *':
//..
  const char *rep = bdem_ElemType::toAscii(bdem_ElemType::BDEM_DATETIME);
  ASSERT(0 == bsl::strcmp(rep, "DATETIME"));
//..
// Finally, print the string representation of 'elemType' to 'bsl::cout':
//..
  bsl::cout << elemType;
//..
// This statement produces the following output on 'stdout':
//..
//  DATETIME
//..
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'IS' FUNCTIONS
        // Concerns:
        //   The three is functions, 'isArrayType', 'isAggregateType' and
        //   'isScalarType', return the correct bool value for each possible
        //   'bdem_ElemType::Type' enumerator.  Integral values that do not
        //   correspond to any value in the 'bdem_ElemType::Type' enumeration
        //   should return false
        //
        // Plan:
        //   Define a data array with a test vector corresponding to each
        //   value in the 'bdem_ElemType::Type' enumeration.  Exercise
        //   'isArrayType' 'isAggregateType' and 'isScalarType' on each test
        //   vector.  Also test out-of-band values (both too small and too
        //   large).
        //
        // Testing:
        //   bool isArrayType(bdem_ElemType::Type value);
        //   bool isAggregateType(bdem_ElemType::Type value);
        //   bool isChoiceType(bdem_ElemType::Type value);
        //   bool isScalarType(bdem_ElemType::Type value);
        // --------------------------------------------------------------------

        if (verbose) {
          cout << endl << "Testing 'is***Type' Functions" << endl
                       << "=============================" << endl;
        }

        static const struct {
            Enum d_enum;             // enumerator value
            bool d_isArrayType;      // is array type
            bool d_isAggregateType;  // is aggregate type
            bool d_isChoiceType;     // is choice type
            bool d_isScalarType;     // is scalar type
        } DATA[] = {
// enum                    isArrayType  isAggType isChoiceType isScalarType
// ----                    -----------  --------- ------------ ------------
{ Et::BDEM_CHOICE_ARRAY_ITEM,  false,       true,      true,       false },
{ Et::BDEM_ROW,                false,       true,      false,      false },
{ Et::BDEM_VOID,               false,       false,     false,      false },

{ Et::BDEM_BOOL,               false,       false,     false,      true  },
{ Et::BDEM_CHAR,               false,       false,     false,      true  },
{ Et::BDEM_SHORT,              false,       false,     false,      true  },
{ Et::BDEM_INT,                false,       false,     false,      true  },
{ Et::BDEM_INT64,              false,       false,     false,      true  },
{ Et::BDEM_FLOAT,              false,       false,     false,      true  },
{ Et::BDEM_DOUBLE,             false,       false,     false,      true  },
{ Et::BDEM_STRING,             false,       false,     false,      true  },
{ Et::BDEM_DATETIME,           false,       false,     false,      true  },
{ Et::BDEM_DATE,               false,       false,     false,      true  },
{ Et::BDEM_TIME,               false,       false,     false,      true  },
{ Et::BDEM_DATETIMETZ,         false,       false,     false,      true  },
{ Et::BDEM_DATETZ,             false,       false,     false,      true  },
{ Et::BDEM_TIMETZ,             false,       false,     false,      true  },

{ Et::BDEM_BOOL_ARRAY,         true,        false,     false,      false },
{ Et::BDEM_CHAR_ARRAY,         true,        false,     false,      false },
{ Et::BDEM_SHORT_ARRAY,        true,        false,     false,      false },
{ Et::BDEM_INT_ARRAY,          true,        false,     false,      false },
{ Et::BDEM_INT64_ARRAY,        true,        false,     false,      false },
{ Et::BDEM_FLOAT_ARRAY,        true,        false,     false,      false },
{ Et::BDEM_DOUBLE_ARRAY,       true,        false,     false,      false },
{ Et::BDEM_STRING_ARRAY,       true,        false,     false,      false },
{ Et::BDEM_DATETIME_ARRAY,     true,        false,     false,      false },
{ Et::BDEM_DATE_ARRAY,         true,        false,     false,      false },
{ Et::BDEM_TIME_ARRAY,         true,        false,     false,      false },
{ Et::BDEM_DATETIMETZ_ARRAY,   true,        false,     false,      false },
{ Et::BDEM_DATETZ_ARRAY,       true,        false,     false,      false },
{ Et::BDEM_TIMETZ_ARRAY,       true,        false,     false,      false },

{ Et::BDEM_CHOICE,             false,       true,      true,       false },
{ Et::BDEM_CHOICE_ARRAY,       true,        true,      true,       false },
{ Et::BDEM_LIST,               false,       true,      false,      false },
{ Et::BDEM_TABLE,              true,        true,      false,      false },
          };

        const int DATA_LENGTH = sizeof DATA / sizeof *DATA;

        if (verbose) {
            cout << "\nVerify that table length is correct." << endl;
        }

        ASSERT(DATA_LENGTH == NUM_REAL_TYPES + NUM_PSEUDO_TYPES);

        if (verbose) {
            cout << "\nTest in-of-band values." << endl;
        }

        for (int i = 0; i < DATA_LENGTH; ++i) {
            Enum ENUM            = DATA[i].d_enum;
            bool IS_ARRAY_TYPE   = DATA[i].d_isArrayType;
            bool IS_AGG_TYPE     = DATA[i].d_isAggregateType;
            bool IS_CHOICE_TYPE  = DATA[i].d_isChoiceType;
            bool IS_SCALAR_TYPE  = DATA[i].d_isScalarType;

            LOOP3_ASSERT(ENUM, Et::isArrayType(ENUM), IS_ARRAY_TYPE,
                         IS_ARRAY_TYPE == Et::isArrayType(ENUM));
            LOOP3_ASSERT(ENUM, Et::isAggregateType(ENUM), IS_AGG_TYPE,
                         IS_AGG_TYPE == Et::isAggregateType(ENUM));
            LOOP3_ASSERT(ENUM, Et::isChoiceType(ENUM), IS_CHOICE_TYPE,
                         IS_CHOICE_TYPE == Et::isChoiceType(ENUM));
            LOOP3_ASSERT(ENUM, Et::isScalarType(ENUM), IS_SCALAR_TYPE,
                         IS_SCALAR_TYPE == Et::isScalarType(ENUM));
        }

        if (verbose) {
            cout << "\nTest out-of-band values." << endl;
        }

        const int TOO_SMALL = -(NUM_PSEUDO_TYPES + 1);
        LOOP_ASSERT(Et::isArrayType(Enum(TOO_SMALL)),
                    0 == Et::isArrayType(Enum(TOO_SMALL)));
        LOOP_ASSERT(Et::isAggregateType(Enum(TOO_SMALL)),
                    0 == Et::isAggregateType(Enum(TOO_SMALL)));
        LOOP_ASSERT(Et::isChoiceType(Enum(TOO_SMALL)),
                    0 == Et::isChoiceType(Enum(TOO_SMALL)));
        LOOP_ASSERT(Et::isScalarType(Enum(TOO_SMALL)),
                    0 == Et::isScalarType(Enum(TOO_SMALL)));

        // Can't test TOO_LARGE because 32 would be outside of the range of the
        // enum, defined in the standard as the smallest bit field that can
        // store all the enumerated values.

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING TYPE-MAPPING FUNCTIONS
        // Concerns:
        //   The two type-mapping functions, 'toArrayType' and
        //   'fromArrayType', return the correct value for each possible
        //   'bdem_ElemType::Type' enumerator.  Integral values that do not
        //   correspond to any value in the 'bdem_ElemType::Type' enumeration
        //   should map to 'bdem_ElemType::BDEM_VOID'.
        //
        // Plan:
        //   Define a data array with a test vector corresponding to each value
        //   in the 'bdem_ElemType::Type' enumeration.  Exercise
        //   'toArrayType' and 'fromArrayType' on each test vector.  Also test
        //   out-of-band values (both too small and too large).
        //
        // Testing:
        //   bdem_ElemType::Type toArrayType(bdem_ElemType::Type value);
        //   bdem_ElemType::Type fromArrayType(bdem_ElemType::Type value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Type Mapping Functions" << endl
                                  << "==============================" << endl;

        static const struct {
            Enum d_enum;       // enumerator value
            Enum d_arrayType;  // corresponding array type
            Enum d_baseType;   // corresponding base type
        } DATA[] = {
// enumerator value  toArrayType              fromArraytype
// ----------------  -----------              -------------
{ Et::BDEM_CHOICE_ARRAY_ITEM, Et::BDEM_CHOICE_ARRAY,    Et::BDEM_VOID       },
{ Et::BDEM_ROW,               Et::BDEM_TABLE,           Et::BDEM_VOID       },
{ Et::BDEM_VOID,              Et::BDEM_VOID,            Et::BDEM_VOID       },

{ Et::BDEM_BOOL,              Et::BDEM_BOOL_ARRAY,      Et::BDEM_VOID       },
{ Et::BDEM_CHAR,              Et::BDEM_CHAR_ARRAY,      Et::BDEM_VOID       },
{ Et::BDEM_SHORT,             Et::BDEM_SHORT_ARRAY,     Et::BDEM_VOID       },
{ Et::BDEM_INT,               Et::BDEM_INT_ARRAY,       Et::BDEM_VOID       },
{ Et::BDEM_INT64,             Et::BDEM_INT64_ARRAY,     Et::BDEM_VOID       },
{ Et::BDEM_FLOAT,             Et::BDEM_FLOAT_ARRAY,     Et::BDEM_VOID       },
{ Et::BDEM_DOUBLE,            Et::BDEM_DOUBLE_ARRAY,    Et::BDEM_VOID       },
{ Et::BDEM_STRING,            Et::BDEM_STRING_ARRAY,    Et::BDEM_VOID       },
{ Et::BDEM_DATETIME,          Et::BDEM_DATETIME_ARRAY,  Et::BDEM_VOID       },
{ Et::BDEM_DATE,              Et::BDEM_DATE_ARRAY,      Et::BDEM_VOID       },
{ Et::BDEM_TIME,              Et::BDEM_TIME_ARRAY,      Et::BDEM_VOID       },
{ Et::BDEM_DATETIMETZ,        Et::BDEM_DATETIMETZ_ARRAY,Et::BDEM_VOID       },
{ Et::BDEM_DATETZ,            Et::BDEM_DATETZ_ARRAY,    Et::BDEM_VOID       },
{ Et::BDEM_TIMETZ,            Et::BDEM_TIMETZ_ARRAY,    Et::BDEM_VOID       },

{ Et::BDEM_BOOL_ARRAY,        Et::BDEM_VOID,            Et::BDEM_BOOL       },
{ Et::BDEM_CHAR_ARRAY,        Et::BDEM_VOID,            Et::BDEM_CHAR       },
{ Et::BDEM_SHORT_ARRAY,       Et::BDEM_VOID,            Et::BDEM_SHORT      },
{ Et::BDEM_INT_ARRAY,         Et::BDEM_VOID,            Et::BDEM_INT        },
{ Et::BDEM_INT64_ARRAY,       Et::BDEM_VOID,            Et::BDEM_INT64      },
{ Et::BDEM_FLOAT_ARRAY,       Et::BDEM_VOID,            Et::BDEM_FLOAT      },
{ Et::BDEM_DOUBLE_ARRAY,      Et::BDEM_VOID,            Et::BDEM_DOUBLE     },
{ Et::BDEM_STRING_ARRAY,      Et::BDEM_VOID,            Et::BDEM_STRING     },
{ Et::BDEM_DATETIME_ARRAY,    Et::BDEM_VOID,            Et::BDEM_DATETIME   },
{ Et::BDEM_DATE_ARRAY,        Et::BDEM_VOID,            Et::BDEM_DATE       },
{ Et::BDEM_TIME_ARRAY,        Et::BDEM_VOID,            Et::BDEM_TIME       },
{ Et::BDEM_DATETIMETZ_ARRAY,  Et::BDEM_VOID,            Et::BDEM_DATETIMETZ },
{ Et::BDEM_DATETZ_ARRAY,      Et::BDEM_VOID,            Et::BDEM_DATETZ     },
{ Et::BDEM_TIMETZ_ARRAY,      Et::BDEM_VOID,            Et::BDEM_TIMETZ     },

{ Et::BDEM_CHOICE,            Et::BDEM_CHOICE_ARRAY,    Et::BDEM_VOID       },
{ Et::BDEM_CHOICE_ARRAY,      Et::BDEM_VOID,            Et::BDEM_CHOICE     },
{ Et::BDEM_LIST,              Et::BDEM_TABLE,           Et::BDEM_VOID       },
{ Et::BDEM_TABLE,             Et::BDEM_VOID,            Et::BDEM_LIST       }
        };

        const int DATA_LENGTH = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nVerify that table length is correct." << endl;

        ASSERT(DATA_LENGTH == NUM_REAL_TYPES + NUM_PSEUDO_TYPES);

        if (verbose) cout <<
                    "\nVerify toArrayType & fromArrayType functions." << endl;

        for (int i = 0; i < DATA_LENGTH; ++i) {
            Enum ENUM       = DATA[i].d_enum;
            Enum ARRAY_TYPE = DATA[i].d_arrayType;
            Enum BASE_TYPE  = DATA[i].d_baseType;

            LOOP_ASSERT(i, ARRAY_TYPE == Et::toArrayType(ENUM));
            LOOP_ASSERT(i, BASE_TYPE  == Et::fromArrayType(ENUM));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING STREAMING OPERATORS
        // Concerns:
        //   Our concerns here are to test the standard operations on streams
        //   that are convertible to 'bdex' streams.  We thoroughly test
        //   "normal" functionality using the 'bdexStreamOut' and
        //   'bdexStreamIn' methods.  We next step through the sequence of
        //   possible "abnormal" stream states (empty, invalid, incomplete,
        //   and corrupted), appropriately selecting data sets as described
        //   below.  In all cases, exception neutrality is confirmed using the
        //   specially instrumented 'bdex_TestInStream' and a pair of standard
        //   macros, 'BEGIN_BDEX_EXCEPTION_TEST' and
        //   'END_BDEX_EXCEPTION_TEST', which configure the
        //   'bdex_TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   Let L represents the number of valid enumerator values.
        //   Let S represent the set of consecutive integers { 0 .. L - 1 }
        //   Let T represent the set of consecutive integers { -1 .. L }
        //
        //   VALID STREAMS
        //     Verify that each valid enumerator value in S can be written to
        //     and successfully read from a valid 'bdex' data stream into an
        //     instance of the enumeration with any initial value in T leaving
        //     the stream in a valid state.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each valid and invalid initial enumerator value in T,
        //     create an instance of the enumeration and attempt to stream
        //     into it from an empty and then invalid stream.  Verify that the
        //     instance has its initial value, and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 identical valid enumerator values to an output stream
        //     buffer, which will then be of total length N.  For each partial
        //     stream length from 0 to N - 1, construct an input stream and
        //     attempt to read into enumerator instances initialized with 3
        //     other identical values.  Verify values of instances that are
        //     successfully modified, partially modified (and therefore reset
        //     to the default value), or left entirely unmodified.  Also verify
        //     that the stream becomes invalid immediately after the first
        //     incomplete read.
        //
        //   CORRUPTED DATA
        //     Use the underlying stream package to simulate an instance of a
        //     typical valid (control) stream and verify that it can be
        //     streamed in successfully.  Then for each of the two data fields
        //     in the stream (beginning with the version number), provide two
        //     similar tests with the data field corrupted ("too small" and
        //     "too large").  After each test, verify the instance has the
        //     default value, and that the input stream has gone invalid.
        //
        //   WHITE-BOX CONSIDERATIONS
        //   ------------------------
        //   Of the streaming methods being tested here, we know from looking
        //   at the implementation that the 'bdexStreamOut' and 'bdexStreamIn'
        //   methods which *require* a 'version' parameter are the methods
        //   which do the main bulk of processing (the remaining methods are
        //   lightweight wrappers).  Therefore, only these two are
        //   significantly tested - for the remaining ones we only perform some
        //   basic tests with valid data.
        //
        // Testing:
        //   bdexStreamOut(bdex_OutStream& s,bdem_ElemType::Type rhs, int ver);
        //   bdexStreamIn(bdex_InStream& s, bdem_ElemType::Type& rhs, int ver);
        //   bdexStreamIn(bdex_InStream& s, bdem_ElemType::Type& rhs);
        //   bdexStreamOut(bdex_OutStream& s, bdem_ElemType::Type rhs);
        //   operator>>(bdex_InStream&, bdem_ElemType::Type& rhs);
        //   operator<<(bdex_OutStream&, bdem_ElemType::Type rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Streaming" << endl
                                  << "=================" << endl;

        // --------------------------------------------------------------------

        if (verbose) cout << "\nCORE FUNCTIONS." << endl << endl;

        // --------------------------------------------------------------------

        if (verbose) cout << "\tOk data." << endl;
        {
            for (int i = 0; i < NUM_REAL_TYPES; ++i) {
                Out out;
                const Enum X = Enum(i);  if (veryVerbose) { P_(i);  P(X); }
                const int version = Et::maxSupportedBdexVersion();
                out.putVersion(version);

                Et::bdexStreamOut(out, X, version);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value
                // and that the input stream is emptied, but remains valid.
                for (int j = 0; j < NUM_REAL_TYPES; ++j) {
                    int ver;
                    In in(OD, LOD);
                    in.setSuppressVersionCheck(1);
                    In &testInStream = in;
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());
                    Enum t = Enum(j);

                    BEGIN_BDEX_EXCEPTION_TEST {
                      in.reset();
                      LOOP2_ASSERT(i, j, X == t == (i == j));
                      in.getVersion(ver);
                      ASSERT(version == ver);
                      Et::bdexStreamIn(in, t, ver);
                    } END_BDEX_EXCEPTION_TEST

                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            Out out;
            const int version = 1;
            const char *const  OD = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = -1; i <= NUM_REAL_TYPES; ++i) {
                In in(OD, LOD);
                In &testInStream = in;
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged if
                // it was initially valid.
                const Enum X = Enum(i);
                Enum t(X);
                LOOP_ASSERT(i, X == t);

                BEGIN_BDEX_EXCEPTION_TEST {
                    in.reset();
                    Et::bdexStreamIn(in, t, version);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == t ||
                                ((i == -1 || i == NUM_REAL_TYPES) && 0 == t));
                    Et::bdexStreamIn(in, t, version);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == t ||
                                ((i == -1 || i == NUM_REAL_TYPES) && 0 == t));
                } END_BDEX_EXCEPTION_TEST

            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
             "\t\tOn incomplete, but otherwise valid, data." << endl;
        {
            const Enum W1 = Enum(0), X1 = Enum(1), Y1 = Enum(2);
            const Enum W2 = Enum(3), X2 = Enum(4), Y2 = Enum(5);
            const Enum W3 = Enum(6), X3 = Enum(7), Y3 = Enum(8);

            Out out;
            const int version = 1;
            Et::bdexStreamOut(out, X1, version);
            const int LOD1 = out.length();
            Et::bdexStreamOut(out, X2, version);
            const int LOD2 = out.length();
            Et::bdexStreamOut(out, X3, version);
            const int LOD  = out.length();
            const char *const     OD   = out.data();

            for (int i = 0; i < LOD; ++i) {
                In in(OD, i);  In &testInStream = in;

                BEGIN_BDEX_EXCEPTION_TEST {
                    in.reset();
                    LOOP_ASSERT(i, in);
                    LOOP_ASSERT(i, !i == in.isEmpty());
                    Enum t1(W1), t2(W2), t3(W3);

                    if (i < LOD1) {
                        Et::bdexStreamIn(in, t1, version);
                        LOOP_ASSERT(i, !in);
                        if (0 == i) LOOP_ASSERT(i, W1 == t1);
                        Et::bdexStreamIn(in, t2, version);
                        LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W2 == t2);
                        Et::bdexStreamIn(in, t3, version);
                        LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W3 == t3);
                    }

                    else if (i < LOD2) {
                        Et::bdexStreamIn(in, t1, version);
                        LOOP_ASSERT(i,  in);   LOOP_ASSERT(i, X1 == t1);
                        Et::bdexStreamIn(in, t2, version);
                        LOOP_ASSERT(i, !in);
                        if (LOD1 == i) LOOP_ASSERT(i, W2 == t2);
                        Et::bdexStreamIn(in, t3, version);
                        LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W3 == t3);
                    }

                    else {
                        Et::bdexStreamIn(in, t1, version);
                        LOOP_ASSERT(i,  in);    LOOP_ASSERT(i, X1 == t1);
                        Et::bdexStreamIn(in, t2, version);
                        LOOP_ASSERT(i,  in);    LOOP_ASSERT(i, X2 == t2);
                        Et::bdexStreamIn(in, t3, version);
                        LOOP_ASSERT(i, !in);
                        if (LOD2 == i) LOOP_ASSERT(i, W3 == t3);
                    }

                    LOOP_ASSERT(i, Y1 != t1);
                    t1 = Y1;
                    LOOP_ASSERT(i, Y1 == t1);    LOOP_ASSERT(i, Y2 != t2);
                    t2 = Y2;
                    LOOP_ASSERT(i, Y2 == t2);    LOOP_ASSERT(i, Y3 != t3);
                    t3 = Y3;
                    LOOP_ASSERT(i, Y3 == t3);
                } END_BDEX_EXCEPTION_TEST
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Enum W = Enum(0), X = Enum(1), Y = Enum(2);
        ASSERT(NUM_REAL_TYPES > Y);

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            const int version = 1;
            Out out;
            Et::bdexStreamOut(out, Y, version);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            Enum t(X);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
            In in(OD, LOD);
            ASSERT(in);
            Et::bdexStreamIn(in, t, version);
            ASSERT(in);
            ASSERT(W != t);    ASSERT(X != t);    ASSERT(Y == t);
         }

        // -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

        if (verbose) cout << "\t\tBad version number." << endl;

        {
            const int version     = 0;          // BAD: too small
            const Enum enumerator = Enum(Y);    // BAD: too large
            Out out;
            Et::bdexStreamOut(out, enumerator, version);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            Enum t(X);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);
            Et::bdexStreamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
        }
        {
            const int version     = 5;           // BAD: too large
            const Enum enumerator = Enum(Y);     // BAD: too large
            Out out;
            Et::bdexStreamOut(out, enumerator, version);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            Enum t(X);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);
            Et::bdexStreamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
        }

        // -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

        if (verbose) cout << "\t\tBad enumerator value." << endl;

        {
            const int version     = 1;
            const Enum enumerator = Enum(-1);              // BAD: too small
            Out out;
            Et::bdexStreamOut(out, enumerator, version);
            const char *const OD =  out.data();
            const int         LOD = out.length();
            Enum t(X);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);
            Et::bdexStreamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
        }
        {
            const int version     = 1;
            const Enum enumerator = Enum(NUM_REAL_TYPES);  // BAD: too large
            Out out;
            Et::bdexStreamOut(out, enumerator, version);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            Enum t(X);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
            In in(OD, LOD);
            ASSERT(in);
            Et::bdexStreamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\n\nWRAPPER FUNCTIONS." << endl << endl;

        // --------------------------------------------------------------------

        if (verbose) cout << "\tTesting bdexStreamIn/Out." << endl;
        {
            for (int i = 0; i < NUM_REAL_TYPES; ++i) {
                Out out;
                const Enum X = Enum(i);
                if (veryVerbose) { P_(i);  P(X); }
                bdex_OutStreamFunctions::streamOut(
                                             out,
                                             X,
                                             Et::maxSupportedBdexVersion());
                const char *const OD = out.data();
                const int LOD = out.length();

                // Verify that each new value overwrites every old value
                // and that the input stream is emptied, but remains valid.
                for (int j = 0; j < NUM_REAL_TYPES; ++j) {
                    In in(OD, LOD);
                    in.setSuppressVersionCheck(1);
                    In &testInStream = in;
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());
                    Enum t = Enum(j);

                    BEGIN_BDEX_EXCEPTION_TEST {
                      in.reset();
                      LOOP2_ASSERT(i, j, X == t == (i == j));
                      bdex_InStreamFunctions::streamIn(
                                             in,
                                             t,
                                             Et::maxSupportedBdexVersion());
                    } END_BDEX_EXCEPTION_TEST

                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\tTesting '<<' and '>>'." << endl;
        {
            for (int i = 0; i < NUM_REAL_TYPES; ++i) {
                const Enum X = Enum(i);
                if (veryVerbose) { P_(i);  P(X); }
                Out out;
                bdex_OutStreamFunctions::streamOut(
                                             out,
                                             X,
                                             Et::maxSupportedBdexVersion());
                const char *const OD  = out.data();
                const int         LOD = out.length();

                for (int j = 0; j < NUM_REAL_TYPES; ++j) {
                    In in(OD, LOD);  In &testInStream = in;
                    in.setSuppressVersionCheck(1);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());
                    Enum t = Enum(j);
                    BEGIN_BDEX_EXCEPTION_TEST {
                        in.reset();
                        LOOP2_ASSERT(i, j, X == t == (i == j));
                        bdex_InStreamFunctions::streamIn(
                                             in,
                                             t,
                                             Et::maxSupportedBdexVersion());
                    } END_BDEX_EXCEPTION_TEST
                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING MAX-SUPPORTED-VERSION
        // Concerns:
        //   Our concern is that the correct value is returned.
        //
        // Plan:
        //   Retrieve the version number and ensure that it is the correct
        //   value.  Note that this version number is hard-coded into the .h,
        //   and therefore this test case needs to be updated whenever that
        //   number changes.
        //
        // Testing:
        //   int maxSupportedBdexVersion();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting maxSupportedBdexVersion"
                          << "\n===========================" << endl;

        const int EXPECTED_VERSION = 1;
        int currentVersion = Et::maxSupportedBdexVersion();
        ASSERT(currentVersion == EXPECTED_VERSION);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // VALUE TEST
        // Concerns:
        //
        // Plan:
        //   Ensure enumerator values are consecutive integers in the range
        //   [0 .. NUM_TYPES - 1] and that all names are unique.  Verify that
        //   the 'toAscii' function produces strings that are identical to
        //   their respective enumerator symbols.  Verify that the output
        //   operator produces the same respective string values that would be
        //   produced by 'toAscii' (note that this is testing streams
        //   convertible to standard 'ostream' streams and the 'print' method).
        //   Also verify the ascii representation and 'ostream' output for
        //   invalid enumerator values.
        //
        // Testing:
        //   enum Type { ... }
        //   enum { NUM_TYPES = ... }
        //   char *toAscii(Type value);
        //  ^print(bsl::ostream& stream, bdem_ElemType::Type rhs);
        //
        //   Note: '^' indicates a private method, which is tested indirectly
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "VALUE TEST" << endl
                                  << "==========" << endl;

        const char *const UNKNOWN_FMT = "(* UNKNOWN *)";

        static const struct {
            Enum         d_enum;   // Enumerated Value
            const char  *d_ascii;  // String Representation
        } DATA[] = {
            // Enumerated Value                 String Representation
            // ----------------                 ----------------------
            { Et::BDEM_CHOICE_ARRAY_ITEM,         "CHOICE_ARRAY_ITEM" },
            { Et::BDEM_ROW,                       "ROW"               },
            { Et::BDEM_VOID,                      "VOID"              },
            { Et::BDEM_CHAR,                      "CHAR"              },
            { Et::BDEM_SHORT,                     "SHORT"             },
            { Et::BDEM_INT,                       "INT"               },
            { Et::BDEM_INT64,                     "INT64"             },
            { Et::BDEM_FLOAT,                     "FLOAT"             },
            { Et::BDEM_DOUBLE,                    "DOUBLE"            },
            { Et::BDEM_STRING,                    "STRING"            },
            { Et::BDEM_DATETIME,                  "DATETIME"          },
            { Et::BDEM_DATE,                      "DATE"              },
            { Et::BDEM_TIME,                      "TIME"              },
            { Et::BDEM_CHAR_ARRAY,                "CHAR_ARRAY"        },
            { Et::BDEM_SHORT_ARRAY,               "SHORT_ARRAY"       },
            { Et::BDEM_INT_ARRAY,                 "INT_ARRAY"         },
            { Et::BDEM_INT64_ARRAY,               "INT64_ARRAY"       },
            { Et::BDEM_FLOAT_ARRAY,               "FLOAT_ARRAY"       },
            { Et::BDEM_DOUBLE_ARRAY,              "DOUBLE_ARRAY"      },
            { Et::BDEM_STRING_ARRAY,              "STRING_ARRAY"      },
            { Et::BDEM_DATETIME_ARRAY,            "DATETIME_ARRAY"    },
            { Et::BDEM_DATE_ARRAY,                "DATE_ARRAY"        },
            { Et::BDEM_TIME_ARRAY,                "TIME_ARRAY"        },
            { Et::BDEM_LIST,                      "LIST"              },
            { Et::BDEM_TABLE,                     "TABLE"             },
            { Et::BDEM_BOOL,                      "BOOL"              },
            { Et::BDEM_DATETIMETZ,                "DATETIMETZ"        },
            { Et::BDEM_DATETZ,                    "DATETZ"            },
            { Et::BDEM_TIMETZ,                    "TIMETZ"            },
            { Et::BDEM_BOOL_ARRAY,                "BOOL_ARRAY"        },
            { Et::BDEM_DATETIMETZ_ARRAY,          "DATETIMETZ_ARRAY"  },
            { Et::BDEM_DATETZ_ARRAY,              "DATETZ_ARRAY"      },
            { Et::BDEM_TIMETZ_ARRAY,              "TIMETZ_ARRAY"      },
            { Et::BDEM_CHOICE,                    "CHOICE"            },
            { Et::BDEM_CHOICE_ARRAY,              "CHOICE_ARRAY"      },
        };

        const int DATA_LENGTH = sizeof DATA / sizeof *DATA;

        int i; // loop index variable -- keeps MS compiler from complaining

        if (verbose) cout << "\nVerify table length is correct." << endl;

        ASSERT(DATA_LENGTH >= NUM_REAL_TYPES + NUM_PSEUDO_TYPES);

        if (verbose) cout <<
                         "\nVerify enumerator values are sequential." << endl;

        for (i = 0; i < NUM_REAL_TYPES + NUM_PSEUDO_TYPES; ++i) {
            LOOP_ASSERT(i, DATA[i].d_enum == i - NUM_PSEUDO_TYPES);
        }

        if (verbose) cout << "\nVerify the toAscii function." << endl;

        for (i = 0; i < DATA_LENGTH; ++i) {
            const Enum        ENUM = DATA[i].d_enum;
            const char *const FMT  = DATA[i].d_ascii;

            if (veryVerbose) cout << "EXPECTED FORMAT: " << FMT << endl;
            const char *const ACT = Et::toAscii(ENUM);
            if (veryVerbose) cout << "  ACTUAL FORMAT: " << ACT << endl <<endl;

            LOOP_ASSERT(i, 0 == strcmp(FMT, ACT));
            for (int j = 0; j < i          // Make sure ALL strings are unique.
                         && j < NUM_REAL_TYPES + NUM_PSEUDO_TYPES; ++j) {
                LOOP2_ASSERT(i, j, 0 != strcmp(DATA[j].d_ascii, FMT));
            }
        }

        if (verbose) cout << "\nVerify the output (<<) operator." << endl;

        const int SIZE = 1000;   // Must be big enough to hold output string.
        const char XX = (char) 0xff;  // Value used to represent an unset char.
        char mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);
        const char *CTRL_BUF = mCtrlBuf; // Used to check for extra characters.

        for (i = 0; i < DATA_LENGTH; ++i) {
            char buf[SIZE];
            memcpy(buf, CTRL_BUF, SIZE);  // Preset buf to 'unset' char values.

            const Enum        ENUM = DATA[i].d_enum;
            const char *const FMT  = DATA[i].d_ascii;

            if (veryVerbose) cout << "EXPECTED FORMAT: " << FMT << endl;
            ostrstream out(buf, sizeof buf); out << ENUM << ends;
            if (veryVerbose) cout << "  ACTUAL FORMAT: " << buf << endl <<endl;

            const int SZ = strlen(FMT) + 1;
            LOOP_ASSERT(i, SZ < SIZE);  // Check buffer is large enough.
            LOOP_ASSERT(i, XX == buf[SIZE - 1]);  // Check for overrun.
            LOOP_ASSERT(i, 0 == memcmp(buf, FMT, SZ));
            LOOP_ASSERT(i, 0 == memcmp(buf + SZ, CTRL_BUF + SZ, SIZE - SZ));
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
