// bael_predicate.t.cpp    -*-C++-*-
#include <bael_predicate.h>

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bdex_instreamfunctions.h>             // for testing only
#include <bdex_outstreamfunctions.h>            // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only
#include <bdex_testoutstream.h>                 // for testing only

#include <bsl_climits.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a standard value-semantic component.  For the
// standard 10-step test procedure, we choose the constructor that takes a
// name and a value of type 'bael_Attribute::Value' as the primary
// manipulator, and the 'name' and 'value' methods as the basic accessors.
// Our concerns regarding the implementation of this component are that (1)
// all supported value-semantic methods are implemented correctly; (2) the
// hash values must be calculated correctly and must be re-calculated after
// the objects have been modified.
//-----------------------------------------------------------------------------
// [13] static int hash(const bael_Predicate&, int size);
// [11] bael_Predicate(const char *n, int v, bdema_Alct *ba);
// [11] bael_Predicate(const char *n, Int64 v, bdema_Alct *ba);
// [11] bael_Predicate(const char *n, const char *v, bdema_Alct *ba);
// [ 2] bael_Predicate(const char *n, const Value& v, bdema_Alct *ba);
// [ 7] bael_Predicate(const bael_Predicate&, bdema_Alct *ba);
// [ 2] ~bael_Predicate();
// [ 9] bael_Predicate& operator=(const bael_Predicate& rhs);
// [12] void setName(const char *n);
// [12] void setValue(const Value& value);
// [ 4] const char *name() const;
// [ 4] const Value& value() const;
// [ 5] bsl::ostream& print(bsl::ostream& stream, int lvl, int spl) const;
// [ 6] operator==(const bael_Predicate&, const bael_Predicate&);
// [ 6] operator!=(const bael_Predicate&, const bael_Predicate&);
// [ 5] bsl::ostream& operator<<(bsl::ostream&, const bael_Predicate&)
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] Value createValue(int type, int v1, Int64 v2, const char *v3);
// [ 8] UNUSED
// [10] UNUSED
// [14] USAGE EXAMPLE
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

#define LOOP2_ASSERT(I,J,X) {                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t"               \
                     << #J << ": " << J << "\t"               \
                     << #K << ": " << K << "\n";              \
                     aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t"               \
                     << #J << ": " << J << "\t"               \
                     << #K << ": " << K << "\n"               \
                     << #L << ": " << L << "\n";              \
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

typedef bael_Predicate Obj;
typedef bael_Attribute::Value Value;
typedef bsls_PlatformUtil::Int64 Int64;
typedef bdex_TestInStream  In;
typedef bdex_TestOutStream Out;

#define VA_NAME   ""
#define VA_VALUE  0
#define VB_NAME   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define VB_VALUE  1111111111
#define VC_NAME   "abcdefghijklmnopqrstuvwxyz"
#define VC_VALUE  (Int64)1111111111
#define VD_NAME   "1234567890"
#define VD_VALUE  "1234567890"

#define VA VA_NAME, VA_VALUE
#define VB VB_NAME, VB_VALUE
#define VC VC_NAME, VC_VALUE
#define VD VD_NAME, VD_VALUE

const struct {
    int         d_line;     // line number
    const char *d_name;     // predicate name
} NAMES[] = {
    // line     name
    // ----     ----
    {  L_,      ""                                             },
    {  L_,      "A"                                            },
    {  L_,      "B"                                            },
    {  L_,      "a"                                            },
    {  L_,      "AA"                                           },
    {  L_,      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"                   },
    {  L_,      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"                   },
};

const int NUM_NAMES = sizeof NAMES / sizeof *NAMES;

const struct {
    int         d_line;     // line number

    int         d_type;     // type of predicate value: 0 - int
                            //                          1 - Int64
                            //                          2 - string

    Int64       d_ivalue;   // integer value - used when d_type == 0
                            //                        or d_type == 1

    const char *d_svalue;   // string value  - used when d_type == 2
} VALUES[] = {
    ///line  type   ivalue         svalue
    ///----  ----   ------         ------
    {  L_,   0,     0,             0                             },
    {  L_,   0,     1,             0                             },
    {  L_,   0,     -1,            0                             },
    {  L_,   0,     INT_MAX,       0                             },
    {  L_,   0,     INT_MIN,       0                             },

    {  L_,   1,     0,             0                             },
    {  L_,   1,     1,             0                             },
    {  L_,   1,     -1,            0                             },
    {  L_,   1,     INT_MAX,       0                             },
    {  L_,   1,     INT_MIN,       0                             },
    {  L_,   1,     (Int64)INT_MAX + 1,  0                       },
    {  L_,   1,     (Int64)INT_MIN - 1,  0                       },
    {  L_,   1,     LLONG_MAX,     0                             },
    {  L_,   1,     LLONG_MIN,     0                             },

    {  L_,   2,     0,             ""                            },
    {  L_,   2,     0,             "0"                           },
    {  L_,   2,     0,             "A"                           },
    {  L_,   2,     0,             "B"                           },
    {  L_,   2,     0,             "a"                           },
    {  L_,   2,     0,             "AA"                          },
    {  L_,   2,     0,             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"  },
};

const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

Value createValue(int type, int v1, Int64 v2, const char *v3)
    // Create an predicate value from one of specified 'v1', 'v2', or 'v3'
    // based on the specified 'type'.
{
    Value variant;
    switch (type) {
      case 0:
        variant.assign<int>(v1);
        break;
      case 1:
        variant.assign<Int64>(v2);
        break;
      case 2:
        variant.assign<string>(v3);
        break;
    }
    return variant;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bslma_TestAllocator  testAllocator(veryVeryVerbose);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 14: {
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

        if (verbose) cout << "\nTesting usage example"
                          << "\n====================="
                          << endl;

        bael_Predicate p1("uuid", 4044457);
        bael_Predicate p2("name", "Gang Chen");

        ASSERT(0 == bsl::strcmp("uuid", p1.name()));
        ASSERT(0 == bsl::strcmp("name", p2.name()));

        ASSERT(true        == p1.value().is<int>());
        ASSERT(4044457     == p1.value().the<int>());
        ASSERT(true        == p2.value().is<bsl::string>());
        ASSERT("Gang Chen" == p2.value().the<bsl::string>());

        char buffer[] = "Hello";
        bael_Predicate p3(buffer, 1);
        bsl::strcpy(buffer, "World");
        ASSERT(0 == bsl::strcmp("Hello", p3.name()));

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING HASH FUNCTION (VALUE):
        //   Verify the hash return value is constant across all platforms for
        //   a given input.
        //
        // Plan:
        //   Specifying a set of test vectors and verify the return value.
        //
        //   Note that 7 is the smallest hash table size and 1610612741 is
        //   largest size (that can fit into an int) used by stlport
        //   hashtable.
        //
        // Testing:
        //   static int hash(const bael_Predicate&, int size);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting hash function"
                          << "\n====================="
                          << endl;

        static const struct {
            int                d_line;       // line number
            const char        *d_name;       // attribute name
            int                d_type;       // type of attribute value
            bsls_Types::Int64  d_ivalue;     // integer attribute value
            const char        *d_svalue;     // string attribute value
            int                d_hashSize;   // hashtable size
            int                d_hashValue;  // expected hash value
        } HDATA[] = {
            ///line  name  type  ivalue   svalue  hsize       hash value
            ///----  ----  ----  ------   ------  -----       ----------
            {  L_,   "",   0,    0,       0,      256,        246          },
            {  L_,   "A",  0,    0,       0,      256,        54           },
            {  L_,   "A",  0,    1,       0,      256,        35           },
            {  L_,   "A",  0,    INT_MAX, 0,      256,        194          },
            {  L_,   "A",  0,    INT_MIN, 0,      256,        82           },
            {  L_,   "",   1,    0,       0,      256,        72           },
            {  L_,   "A",  1,    0,       0,      256,        136          },
            {  L_,   "A",  1,    1,       0,      256,        34           },
            {  L_,   "A",  1,    INT_MAX, 0,      256,        122          },
            {  L_,   "A",  1,    INT_MIN, 0,      256,        50           },
            {  L_,   "A",  1,    LLONG_MAX, 0,    256,        15           },
            {  L_,   "A",  1,    LLONG_MIN, 0,    256,        10           },
            {  L_,   "",   2,    0,       "",     256,        26           },
            {  L_,   "A",  2,    0,       "",     256,        90           },
            {  L_,   "A",  2,    0,       "A",    256,        154          },
            {  L_,   "",   2,    0,       "ABCD", 256,        162          },
            {  L_,   "A",  2,    0,       "ABCD", 256,        226          },
            {  L_,   "",   0,    0,       0,      65536,      36086        },
            {  L_,   "A",  0,    0,       0,      65536,      1846         },
            {  L_,   "A",  0,    1,       0,      65536,      55843        },
            {  L_,   "A",  0,    INT_MAX, 0,      65536,      4290         },
            {  L_,   "A",  0,    INT_MIN, 0,      65536,      26706        },
            {  L_,   "",   1,    0,       0,      65536,      45128        },
            {  L_,   "A",  1,    0,       0,      65536,      10888        },
            {  L_,   "A",  1,    1,       0,      65536,      40738        },
            {  L_,   "A",  1,    INT_MAX, 0,      65536,      20346        },
            {  L_,   "A",  1,    INT_MIN, 0,      65536,      17970        },
            {  L_,   "A",  1,    LLONG_MAX, 0,    65536,      61711        },
            {  L_,   "A",  1,    LLONG_MIN, 0,    65536,      10506        },
            {  L_,   "",   2,    0,       "",     65536,      41498        },
            {  L_,   "A",  2,    0,       "",     65536,      7258         },
            {  L_,   "A",  2,    0,       "A",    65536,      38554        },
            {  L_,   "",   2,    0,       "ABCD", 65536,      52898        },
            {  L_,   "A",  2,    0,       "ABCD", 65536,      18658        },
            {  L_,   "",   0,    0,       0,      7,          1            },
            {  L_,   "A",  0,    0,       0,      7,          4            },
            {  L_,   "A",  0,    1,       0,      7,          0            },
            {  L_,   "A",  0,    INT_MAX, 0,      7,          0            },
            {  L_,   "A",  0,    INT_MIN, 0,      7,          5            },
            {  L_,   "",   1,    0,       0,      7,          5            },
            {  L_,   "A",  1,    0,       0,      7,          5            },
            {  L_,   "A",  1,    1,       0,      7,          1            },
            {  L_,   "A",  1,    INT_MAX, 0,      7,          6            },
            {  L_,   "A",  1,    INT_MIN, 0,      7,          0            },
            {  L_,   "A",  1,    LLONG_MAX, 0,    7,          6            },
            {  L_,   "A",  1,    LLONG_MIN, 0,    7,          0            },
            {  L_,   "",   2,    0,       "",     7,          5            },
            {  L_,   "A",  2,    0,       "",     7,          1            },
            {  L_,   "A",  2,    0,       "A",    7,          4            },
            {  L_,   "",   2,    0,       "ABCD", 7,          0            },
            {  L_,   "A",  2,    0,       "ABCD", 7,          3            },
            {  L_,   "",   0,    0,       0,      1610612741, 1185910006   },
            {  L_,   "A",  0,    0,       0,      1610612741, 717686582    },
            {  L_,   "A",  0,    1,       0,      1610612741, 1358289443   },
            {  L_,   "A",  0,    INT_MAX, 0,      1610612741, 981602493    },
            {  L_,   "A",  0,    INT_MIN, 0,      1610612741, 388327501    },
            {  L_,   "",   1,    0,       0,      1610612741, 327790664    },
            {  L_,   "A",  1,    0,       0,      1610612741, 933309054    },
            {  L_,   "A",  1,    1,       0,      1610612741, 221028130    },
            {  L_,   "A",  1,    INT_MAX, 0,      1610612741, 371216250    },
            {  L_,   "A",  1,    INT_MIN, 0,      1610612741, 929711661    },
            {  L_,   "A",  1,    LLONG_MAX, 0,    1610612741, 1138749706   },
            {  L_,   "A",  1,    LLONG_MIN, 0,    1610612741, 60893445     },
            {  L_,   "",   2,    0,       "",     1610612741, 445882901    },
            {  L_,   "A",  2,    0,       "",     1610612741, 1588272218   },
            {  L_,   "A",  2,    0,       "A",    1610612741, 1120048794   },
            {  L_,   "",   2,    0,       "ABCD", 1610612741, 427544216    },
            {  L_,   "A",  2,    0,       "ABCD", 1610612741, 1569933533   },
        };

        const int NUM_HDATA = sizeof HDATA / sizeof *HDATA;

        for (int i = 0; i < NUM_HDATA; ++i) {
            int LINE = HDATA[i].d_line;

            Obj mX(HDATA[i].d_name,
                   createValue(HDATA[i].d_type,
                               (int)HDATA[i].d_ivalue,
                               HDATA[i].d_ivalue,
                               HDATA[i].d_svalue));
            const Obj& X = mX;

            int hash = Obj::hash(X, HDATA[i].d_hashSize);
            if (veryVerbose) {
                cout <<  X  << " ---> " << hash << endl;
            }
            LOOP_ASSERT(LINE, 0 <= hash);
            LOOP_ASSERT(LINE, hash < HDATA[i].d_hashSize);
            LOOP2_ASSERT(LINE, hash, HDATA[i].d_hashValue == hash);
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING NAME/VALUE MANIPULATORS
        //   The 'setName' and 'setValue' method should set the corresponding
        //   fields correctly.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each pair (u, v) in the cross product
        //   S X S, construct u using the primary constructor, and then change
        //   its name and value using v's name and value.  Verify that two
        //   objects have the same value.
        //
        //   We also in this test case verify that hash values are correct
        //   after objects have been modified.
        //
        // Testing:
        //   void setName(const char *n);
        //   void setValue(const Value& value);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Name/Value Manipulators"
                          << "\n===============================" << endl;

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;

            Value value1 = createValue(VALUES[j].d_type,
                                       VALUES[j].d_ivalue,
                                       VALUES[j].d_ivalue,
                                       VALUES[j].d_svalue);

            Obj v(NAMES[i].d_name, value1);  const Obj& V = v;

            for (int k = 0; k < NUM_NAMES; ++k) {
            for (int l = 0; l < NUM_VALUES; ++l) {
                int LINE3 = NAMES[k].d_line;
                int LINE4 = VALUES[l].d_line;

                Value value2 = createValue(VALUES[l].d_type,
                                           VALUES[l].d_ivalue,
                                           VALUES[l].d_ivalue,
                                           VALUES[l].d_svalue);

                Obj u(NAMES[k].d_name, value2);  const Obj& U = u;
                if (veryVerbose) {
                    cout << "\t";
                    P_(U);
                    P(V);
                }

                bool isSame = i == k && j == l;

                Obj mW1(V); const Obj& W1 = mW1;

                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4, W1 == V);
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4, W1 == U == isSame);
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                             Obj::hash(W1, 65536) == Obj::hash(V, 65536));

                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                             W1.value() == V.value());
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                             0 == strcmp(W1.name(), V.name()));

                mW1.setName(U.name());
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                             W1.value() == V.value());
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                             0 == strcmp(W1.name(), U.name()));

                mW1.setValue(U.value());
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                             W1.value() == U.value());
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                             0 == strcmp(W1.name(), U.name()));

                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4, W1 == U);
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4, W1 == V == isSame);
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                             Obj::hash(W1, 65536) == Obj::hash(U, 65536));

                Obj mW2(V); const Obj& W2 = mW2;

                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4, W2 == V);
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4, W2 == U == isSame);
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                             Obj::hash(W2, 65536) == Obj::hash(V, 65536));

                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                             W2.value() == V.value());
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                             0 == strcmp(W2.name(), V.name()));

                mW2.setValue(U.value());
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                             W2.value() == U.value());
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                             0 == strcmp(W2.name(), V.name()));

                mW2.setName(U.name());
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                             W2.value() == U.value());
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                             0 == strcmp(W2.name(), U.name()));

                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4, W2 == U);
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4, W2 == V == isSame);
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                             Obj::hash(W2, 65536) == Obj::hash(U, 65536));

            }
            }
        }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING NAME/VALUE CONSTRUCTORS
        //   The name/value constructors must initialize members correctly.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct an object
        //   x using the primary constructor, and another object y using the
        //   corresponding name/value constructor.  Use the equality operator
        //   to assert that both x and y have the same value as w.
        //
        // Testing:
        //   bael_Predicate(const char *n, int v, bdema_Alct *ba);
        //   bael_Predicate(const char *n, Int64 v, bdema_Alct *ba);
        //   bael_Predicate(const char *n, const char *v, bdema_Alct *ba);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Name/Value Constructors"
                          << "\n===============================" << endl;

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;

            const char *name = NAMES[i].d_name;
            Value value = createValue(VALUES[j].d_type,
                                      VALUES[j].d_ivalue,
                                      VALUES[j].d_ivalue,
                                      VALUES[j].d_svalue);

            Obj y(name, value); const Obj& Y = y;

            switch (VALUES[j].d_type) {
              case 0: {
                const Obj X1(name, (int)VALUES[j].d_ivalue);
                LOOP2_ASSERT(LINE1, LINE2, X1 == Y);
                const Obj X2(name, (int)VALUES[j].d_ivalue, &testAllocator);
                LOOP2_ASSERT(LINE1, LINE2, X2 == Y);
                BEGIN_BSLMA_EXCEPTION_TEST {
                    const Obj X3(name,
                                 (int)VALUES[j].d_ivalue,
                                 &testAllocator);
                    LOOP2_ASSERT(LINE1, LINE2, X3 == Y);
                } END_BSLMA_EXCEPTION_TEST
              } break;
              case 1: {
                const Obj X1(name, VALUES[j].d_ivalue);
                LOOP2_ASSERT(LINE1, LINE2, X1 == Y);
                const Obj X2(name, VALUES[j].d_ivalue, &testAllocator);
                LOOP2_ASSERT(LINE1, LINE2, X2 == Y);
                BEGIN_BSLMA_EXCEPTION_TEST {
                    const Obj X3(name,
                                 VALUES[j].d_ivalue,
                                 &testAllocator);
                    LOOP2_ASSERT(LINE1, LINE2, X3 == Y);
                } END_BSLMA_EXCEPTION_TEST
              } break;
              case 2: {
                const Obj X1(name, VALUES[j].d_svalue);
                LOOP2_ASSERT(LINE1, LINE2, X1 == Y);
                const Obj X2(name, VALUES[j].d_svalue, &testAllocator);
                LOOP2_ASSERT(LINE1, LINE2, X2 == Y);
                BEGIN_BSLMA_EXCEPTION_TEST {
                    const Obj X3(name,
                                 VALUES[j].d_svalue,
                                 &testAllocator);
                    LOOP2_ASSERT(LINE1, LINE2, X3 == Y);
                } END_BSLMA_EXCEPTION_TEST
              } break;
            }
        }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //
        // Concerns:
        //   The 'bdex' streaming concerns for this component are standard.
        //   We first test the class method 'maxSupportedBdexVersion' and then
        //   use that method to probe the member functions 'outStream' and
        //   'inStream' in the manner of a "breathing test" to verify basic
        //   functionality.  We then thoroughly test streaming functionality
        //   relying on the available bdex stream functions provided by the
        //   concrete streams, which forward appropriate calls to the member
        //   functions of this component.  We next step through the sequence
        //   of possible stream states (valid, empty, invalid, incomplete, and
        //   corrupted), appropriately selecting data sets as described below.
        //   In all cases, exception neutrality is confirmed using the
        //   specially instrumented 'bdex_TestInStream' and a pair of standard
        //   macros, 'BEGIN_BDEX_EXCEPTION_TEST' and
        //   'END_BDEX_EXCEPTION_TEST', which configure the
        //   'bdex_TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   PRELIMINARY MEMBER FUNCTION TEST
        //     First test 'maxSupportedBdexVersion' explicitly, and then
        //     perform a trivial direct test of the 'outStream' and 'inStream'
        //     methods (the rest of the testing will use the stream
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
        //     unmodified, and that the stream became invalid immediately
        //     after the first incomplete read.  Finally ensure that each
        //     object streamed into is in some valid state by assigning it a
        //     distinct new value and testing for equality.
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
        // Testing:
        //   static int maxSupportedBdexVersion();
        //   template <class STREAM> STREAM& bdexStreamIn(STREAM&, int)
        //   template <class STREAM> STREAM& bdexStreamOut(STREAM&, int) const
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'bdex' Streaming Functionality"
                          << "\n======================================"
                          << endl;

        if (verbose) cout << "\n  Testing 'maxSupportedBdexVersion' method."
                          << endl;
        {
            const Obj X(VA_NAME, VA_VALUE);
            ASSERT(1 == X.maxSupportedBdexVersion());
            ASSERT(1 == Obj::maxSupportedBdexVersion());
        }

        const Obj V1(VA_NAME, VA_VALUE);
        const Obj V2(VB_NAME, VA_VALUE);
        const Obj V3(VB_NAME, VB_VALUE);
        const Obj V4(VB_NAME, VC_VALUE);
        const Obj V5(VB_NAME, VD_VALUE);
        const Obj V6(VC_NAME, VA_VALUE);
        const Obj V7(VC_NAME, VB_VALUE);
        const Obj V8(VC_NAME, VC_VALUE);
        const Obj V9(VC_NAME, VD_VALUE);

        const Obj VALUES[] = { V1, V2, V3, V4, V5, V6, V7, V8, V9 };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        const int VERSION = Obj::maxSupportedBdexVersion();
        if (verbose) cout << "\nDirect inital trial of 'streamOut' and"
                             " 'streamIn'." << endl;
        {
            const Obj A(V1);
            Out       out;

            A.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();
            In                in(OD, LOD);
            ASSERT(in);
            ASSERT(!in.isEmpty());
            Obj B(V2);
            ASSERT(B != A);

            in.setSuppressVersionCheck(1);
            B.bdexStreamIn(in, VERSION);
            ASSERT(B == A);
            ASSERT(in);
            ASSERT(in.isEmpty());
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
                LOOP_ASSERT(i, in);        LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                Obj t(VALUES[i]);
                const Obj tt = t;    // control
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, tt == t);
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, tt == t);
              } END_BDEX_EXCEPTION_TEST
            }
        }

        if (verbose) cout <<
            "\tOn incomplete (but otherwise valid) data." << endl;
        {
            const Obj W1 = V1, X1 = V2, Y1 = V3;
            const Obj W2 = V4, X2 = V5, Y2 = V6;
            const Obj W3 = V7, X3 = V8, Y3 = V9;

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

        const Obj W(VA_NAME, VA_VALUE);    // default value
        const Obj X(VB_NAME, VB_VALUE);    // original (control) value
        const Obj Y(VC_NAME, VC_VALUE);    // new (streamed-out) value

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            Out out;
            Y.bdexStreamOut(out, VERSION);
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
            Y.bdexStreamOut(out, VERSION);

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
            const int version = 5 ; // too large (current version is 1)

            Out out;
            Y.bdexStreamOut(out, VERSION);

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
        if (verbose) cout << "\t\tBad value type." << endl;
        {
            Out out;
            out.putString(VB_NAME);     // predicate name
            out.putUint8(1);            // version number for predicate value
            out.putInt32(-1);           // value type should be 0, 1, or 2
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

      } break;
      case 9: {
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
        //   bael_Predicate& operator=(const bael_Predicate& rhs);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Assignment Operator"
                          << "\n===========================" << endl;

        if (verbose) cout << "\nTesting assignment u = v.";

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;

            Value value1 = createValue(VALUES[j].d_type,
                                       VALUES[j].d_ivalue,
                                       VALUES[j].d_ivalue,
                                       VALUES[j].d_svalue);

            Obj v(NAMES[i].d_name, value1);  const Obj& V = v;

            for (int k = 0; k < NUM_NAMES; ++k) {
            for (int l = 0; l < NUM_VALUES; ++l) {
                int LINE3 = NAMES[k].d_line;
                int LINE4 = VALUES[l].d_line;

                  Value value2 = createValue(VALUES[l].d_type,
                                             VALUES[l].d_ivalue,
                                             VALUES[l].d_ivalue,
                                             VALUES[l].d_svalue);

                  Obj u(NAMES[k].d_name, value2);  const Obj& U = u;
                  if (veryVerbose) {
                      cout << "\t";
                      P_(U);
                      P(V);
                  }

                  Obj w(V); const Obj& W = w;      // control
                  u = V;
                  LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4, W == U);
                  LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4, W == V);
            }
            }
        }
        }

        if (verbose) cout << "\nTesting assignment u = u (Aliasing).";

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;
            Value value1 = createValue(VALUES[j].d_type,
                                       VALUES[j].d_ivalue,
                                       VALUES[j].d_ivalue,
                                       VALUES[j].d_svalue);

            Obj u(NAMES[i].d_name, value1);  const Obj& U = u;
            Obj w(U); const Obj& W = w;                         // control
            u = u;

            if (veryVerbose) { T_(); P_(U); P_(W); }
            LOOP2_ASSERT(LINE1, LINE2, U == W);
        }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SECONDARY TEST APPARATUS:
        //   Void for 'bael_Predicate'.
        // --------------------------------------------------------------------

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
        //   bael_Predicate(const bael_Predicate&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;

            const char *name = NAMES[i].d_name;
            Value value = createValue(VALUES[j].d_type,
                                      VALUES[j].d_ivalue,
                                      VALUES[j].d_ivalue,
                                      VALUES[j].d_svalue);

            Obj w(name, value); const Obj& W = w;  // control
            Obj x(name, value); const Obj& X = x;
            Obj y(X);           const Obj& Y = y;

            if (veryVerbose) { T_(); P_(W); P_(X); P(Y); }

            LOOP2_ASSERT(LINE1, LINE2, X == W);
            LOOP2_ASSERT(LINE1, LINE2, Y == W);
            LOOP2_ASSERT(LINE1, LINE2, Y == X);
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
        //   Specify a set S of unique names and a set T of unique value.
        //   Construct a set W of unique bael_Predicate objects using every
        //   element of the cross product S X T.  Verify the correctness of
        //   'operator==' and 'operator!=' for all elements (u, v) of the
        //   cross product W X W.  Next for each element in W, make a copy of
        //   of the predicate, and then use that copy along with the same
        //   predicate value to create another bael_Predicate object to verify
        //   that is the same as that created directly from the name/value
        //   pair.
        //
        // Testing:
        //   operator==(const bael_Predicate&, const bael_Predicate&);
        //   operator!=(const bael_Predicate&, const bael_Predicate&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Equality Operators"
                          << "\n==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of values (u, v) in W X W." << endl;
        {
            for (int i = 0; i < NUM_NAMES; ++i) {
            for (int j = 0; j < NUM_VALUES; ++j) {
                int LINE1 = NAMES[i].d_line;
                int LINE2 = VALUES[j].d_line;

                Value value1 = createValue(VALUES[j].d_type,
                                           VALUES[j].d_ivalue,
                                           VALUES[j].d_ivalue,
                                           VALUES[j].d_svalue);

                Obj mX(NAMES[i].d_name, value1);  const Obj& X = mX;

                for (int k = 0; k < NUM_NAMES; ++k) {
                for (int l = 0; l < NUM_VALUES; ++l) {
                    int LINE3 = NAMES[k].d_line;
                    int LINE4 = VALUES[l].d_line;

                    Value value2 = createValue(VALUES[l].d_type,
                                               VALUES[l].d_ivalue,
                                               VALUES[l].d_ivalue,
                                               VALUES[l].d_svalue);

                    Obj mY(NAMES[k].d_name, value2);  const Obj& Y = mY;
                    if (veryVerbose) {
                        cout << "\t";
                        P_(X);
                        P(Y);
                    }

                    bool isSame = i == k && j == l;
                    LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                                 isSame == (X == Y));
                    LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                                 !isSame == (X != Y));

                }
                }
            }
            }
        }

        if (verbose) cout <<
            "\nVerify that predicate names are compared by content." << endl;
        {
            for (int i = 0; i < NUM_NAMES; ++i) {
            for (int j = 0; j < NUM_VALUES; ++j) {
                int LINE1 = NAMES[i].d_line;
                int LINE2 = VALUES[j].d_line;

                Value value1 = createValue(VALUES[j].d_type,
                                           VALUES[j].d_ivalue,
                                           VALUES[j].d_ivalue,
                                           VALUES[j].d_svalue);

                Obj mX(NAMES[i].d_name, value1);  const Obj& X = mX;

                string name(NAMES[i].d_name);
                Obj mY(name.c_str(), value1);  const Obj& Y = mY;
                LOOP2_ASSERT(LINE1, LINE2, X == Y);
                LOOP2_ASSERT(LINE1, LINE2, !(X != Y));
            }
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //   The output operator is trivially implemented using the
        //   'bsl::ostream' output operators; a very few test vectors can
        //   sufficiently test wc this functionality.
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   bsl::ostream& operator<<(bsl::ostream&, const bael_Predicate&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Output (<<) Operator" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;

        static const struct {
            int         d_line;            // line number
            const char *d_name;            // attribute name
            int         d_type;            // type of predicate value
            int         d_ivalue;          // integer predicate value
            const char *d_svalue;          // string attribute value
            const char *d_output;          // expected output format
        } DATA[] = {
            // line name type ivalue svalue expected
            // ---- ---- ---- ------ ------ --------
            {  L_,  "",  0,   0,     0,    "[  = 0 ]"   },
            {  L_,  "",  1,   0,     0,    "[  = 0 ]"   },
            {  L_,  "",  2 ,  0,     "0",  "[  = 0 ]"   },
            {  L_,  "A", 0,   1,     0,    "[ A = 1 ]"  },
            {  L_,  "A", 2,   0,     "1",  "[ A = 1 ]"  },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            int LINE = DATA[i].d_line;
            Value value = createValue(DATA[i].d_type,
                                      DATA[i].d_ivalue,
                                      DATA[i].d_ivalue,
                                      DATA[i].d_svalue);

            Obj mX(DATA[i].d_name, value);  const Obj& X = mX;

            ostringstream os;
            os << X;

            if (veryVerbose) {
                cout << "\t";
                P_(X);
                P_(DATA[i].d_output);
                P(os.str());
            }
            LOOP_ASSERT(LINE, os.str() == DATA[i].d_output);
        }

        static const struct {
            int         d_line;            // line number
            const char *d_name;            // attribute name
            const char *d_svalue;          // string attribute value
            int         d_level;           // tab level
            int         d_spacesPerLevel;  // spaces per level
            const char *d_output;          // expected output format
        } PDATA[] = {
            // line name svalue level space expected
            // ---- ---- ------ ----- ----- -----------------------
            {  L_,  "A", "1",   0,    -1,   "[ A = 1 ]"            },
            {  L_,  "A", "1",   1,    2,    "  [ A = 1 ]"          },
            {  L_,  "A", "1",   -1,   -2,   "[ A = 1 ]"            },
        };

        const int NUM_PDATA = sizeof PDATA / sizeof *PDATA;

        if (verbose) cout << "\nTesting 'print'." << endl;

        for (int i = 0; i < NUM_PDATA; ++i) {
            int LINE = PDATA[i].d_line;
            Value value = createValue(2, 0, 0, PDATA[i].d_svalue);

            Obj mX(PDATA[i].d_name, value);  const Obj& X = mX;

            ostringstream os;
            X.print(os, PDATA[i].d_level, PDATA[i].d_spacesPerLevel);

            if (veryVerbose) {
                cout << "\t";
                P_(X);
                P_(PDATA[i].d_output);
                P(os.str());
            }
            LOOP_ASSERT(LINE, os.str() == PDATA[i].d_output);
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   We want to verify that each individual field is returned
        //   correctly.
        //
        // Plan:
        //   Specify a set S of unique names and a set T of unique value.
        //   Construct one bael_Predicate object for every element of the
        //   cross product S X T using the primary constructor.  Verify that
        //   each of the basic accessors returns the correct value.
        //
        // Testing:
        //   const char *name();
        //   const Value& value();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Basic Accessors" << endl
                          << "=======================" << endl;

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;

            Value value = createValue(VALUES[j].d_type,
                                      VALUES[j].d_ivalue,
                                      VALUES[j].d_ivalue,
                                      VALUES[j].d_svalue);

            Obj mX(NAMES[i].d_name, value);  const Obj& X = mX;
            if (veryVerbose) {
                cout << "\t";
                P_(NAMES[i].d_name);
                P_(value);
                P(X);
            }
            LOOP2_ASSERT(LINE1,
                         LINE2,
                         0 == strcmp(X.name(), NAMES[i].d_name));
            LOOP2_ASSERT(LINE1, LINE2, X.value() == value);
        }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE TEST APPARATUS:
        //   The 'createValue' method must correctly create a 'bdeut_Variant'
        //   object having the specified type and value.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in type and value.  For each element in S, construct
        //   a 'bdeut_Variant' object using the 'createValue' method, and
        //   verify that the resultant has the specified type and value.
        //
        // Testing:
        //   Value createValue(int type, int v1, Int64 v2, const char *v3);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primitive Test Apparatus"
                          << "\n================================" << endl;

        if (verbose) cout << "\nTesting 'createValue'." << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            int LINE = VALUES[i].d_line;

            Value value = createValue(VALUES[i].d_type,
                                      VALUES[i].d_ivalue,
                                      VALUES[i].d_ivalue,
                                      VALUES[i].d_svalue);

            if (veryVerbose) { cout << "\t"; P(value); }

            switch (VALUES[i].d_type) {
              case 0: {
                LOOP_ASSERT(LINE, value.is<int>());
                LOOP_ASSERT(LINE, VALUES[i].d_ivalue == value.the<int>());
              } break;
              case 1: {
                LOOP_ASSERT(LINE, value.is<Int64>());
                LOOP_ASSERT(LINE, VALUES[i].d_ivalue == value.the<Int64>());
              } break;
              case 2: {
                LOOP_ASSERT(LINE, value.is<string>());
                LOOP_ASSERT(LINE, VALUES[i].d_svalue == value.the<string>());
              } break;
              default:
                LOOP_ASSERT(LINE, 0);
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //   We want to verify the primary manipulators set the member fields
        //   correctly.
        //
        // Plan:
        //   Construct bael_Predicate objects with distinct name and value
        //   pairs, verify the values with the basic accessors, verify the
        //   equality and inequality of these objects.  The destructor is
        //   exercised as the objects being tested leave scope.
        //
        // Testing:
        //   bael_Predicate(const char *n, const Value& v, bdema_Alct *ba);
        //   ~bael_Predicate();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulators"
                          << "\n============================" << endl;

        if (verbose) cout << "\nTesting primary manipulators (thoroughly)."
                          << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            Value mV;  const Value& V = mV;

            mV.assign<int>(VA_VALUE);
            const Obj X(VA_NAME, V);
            ASSERT(0        == strcmp(X.name(), VA_NAME));
            ASSERT(true     == X.value().is<int>());
            ASSERT(VA_VALUE == X.value().the<int>());

            mV.assign<int>(VB_VALUE);
            const Obj Y(VB_NAME, V);
            ASSERT(0        == strcmp(Y.name(), VB_NAME));
            ASSERT(true     == Y.value().is<int>());
            ASSERT(VB_VALUE == Y.value().the<int>());

            mV.assign<Int64>(VC_VALUE);
            const Obj Z(VC_NAME, V);
            ASSERT(0        == strcmp(Z.name(), VC_NAME));
            ASSERT(true     == Z.value().is<Int64>());
            ASSERT(VC_VALUE == Z.value().the<Int64>());

            mV.assign<string>(VD_VALUE);
            const Obj W(VD_NAME, V);
            ASSERT(0        == strcmp(W.name(), VD_NAME));
            ASSERT(true     == W.value().is<string>());
            ASSERT(VD_VALUE == W.value().the<string>());

            if (veryVerbose) { cout << "\t\t"; P_(X); P_(Y); P_(Z); P(W); }
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            Value mV(&testAllocator);  const Value& V = mV;

            mV.assign<int>(VA_VALUE);
            const Obj X(VA_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(X.name(), VA_NAME));
            ASSERT(true     == X.value().is<int>());
            ASSERT(VA_VALUE == X.value().the<int>());

            mV.assign<int>(VB_VALUE);
            const Obj Y(VB_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(Y.name(), VB_NAME));
            ASSERT(true     == Y.value().is<int>());
            ASSERT(VB_VALUE == Y.value().the<int>());

            mV.assign<Int64>(VC_VALUE);
            const Obj Z(VC_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(Z.name(), VC_NAME));
            ASSERT(true     == Z.value().is<Int64>());
            ASSERT(VC_VALUE == Z.value().the<Int64>());

            mV.assign<string>(VD_VALUE);
            const Obj W(VD_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(W.name(), VD_NAME));
            ASSERT(true     == W.value().is<string>());
            ASSERT(VD_VALUE == W.value().the<string>());

            if (veryVerbose) { cout << "\t\t"; P_(X); P_(Y); P_(Z); P(W); }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
          BEGIN_BSLMA_EXCEPTION_TEST {
            Value mV(&testAllocator);  const Value& V = mV;

            mV.assign<int>(VA_VALUE);
            const Obj X(VA_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(X.name(), VA_NAME));
            ASSERT(true     == X.value().is<int>());
            ASSERT(VA_VALUE == X.value().the<int>());

            mV.assign<int>(VB_VALUE);
            const Obj Y(VB_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(Y.name(), VB_NAME));
            ASSERT(true     == Y.value().is<int>());
            ASSERT(VB_VALUE == Y.value().the<int>());

            mV.assign<Int64>(VC_VALUE);
            const Obj Z(VC_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(Z.name(), VC_NAME));
            ASSERT(true     == Z.value().is<Int64>());
            ASSERT(VC_VALUE == Z.value().the<Int64>());

            mV.assign<string>(VD_VALUE);
            const Obj W(VD_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(W.name(), VD_NAME));
            ASSERT(true     == W.value().is<string>());
            ASSERT(VD_VALUE == W.value().the<string>());

            if (veryVerbose) { cout << "\t\t"; P_(X); P_(Y); P_(Z); P(W); }
          } END_BSLMA_EXCEPTION_TEST
        }

      } break;
      case 1: {
       // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //   Create four test objects by using the initializing and copy
        //   constructors.  Exercise the basic value-semantic methods and the
        //   equality operators using these test objects.  Invoke the primary
        //   manipulator [3, 6], copy constructor [2, 8], and assignment
        //   operator without [9, 10] and with [11] aliasing.  Use the direct
        //   accessors to verify the expected results.  Display object values
        //   frequently in verbose mode.  Note that 'VA', 'VB', 'VC', and 'VD'
        //   denote unique, but otherwise arbitrary, object values.
        //
        // 1.  Create an object x1 using VA.        { x1:VA }
        // 2.  Create an object x2 (copy from x1).  { x1:VA x2:VA }
        // 3.  Set x1 to VB.                        { x1:VB x2:VA }
        // 4.  Set x2 to VB.                        { x1:VB x2:VB }
        // 5.  Set x2 to VC.                        { x1:VB x2:VC }
        // 6.  Set x1 to VA.                        { x1:VA x2:VC }
        // 7.  Create an object x3 (with value VD). { x1:VA x2:VC x3:VD }
        // 8.  Create an object x4 (copy from x1).  { x1:VA x2:VC x3:VD x4:VA }
        // 9.  Assign x2 = x1.                      { x1:VA x2:VA x3:VD x4:VA }
        // 10. Assign x2 = x3.                      { x1:VA x2:VD x3:VD x4:VA }
        // 11. Assign x1 = x1 (aliasing).           { x1:VA x2:VD x3:VD x4:VA }
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        if (verbose) cout << "\n 1. Create an object x1 using VA." << endl;
        Obj mX1(VA);  const Obj& X1 = mX1;
        ASSERT(0 == bsl::strcmp(VA_NAME, X1.name()));
        ASSERT(VA_VALUE == X1.value().the<int>());

        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                          << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        ASSERT(0 == bsl::strcmp(VA_NAME, X2.name()));
        ASSERT(VA_VALUE == X2.value().the<int>());

        if (verbose) cout << "\n 3. Set x1 to VB." << endl;
        mX1.setName(VB_NAME);
        mX1.setValue(createValue(0, VB_VALUE, 0, 0));
        ASSERT(0 == bsl::strcmp(VB_NAME, X1.name()));
        ASSERT(VB_VALUE == X1.value().the<int>());
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 4. Set x2 to VB." << endl;
        mX2.setName(VB_NAME);
        mX2.setValue(createValue(0, VB_VALUE, 0, 0));
        ASSERT(0 == bsl::strcmp(VB_NAME, X2.name()));
        ASSERT(VB_VALUE == X2.value().the<int>());
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X1 == X2));        ASSERT(0 == (X1 != X2));

        if (verbose) cout << "\n 5. Set x2 to VC." << endl;
        mX2.setName(VC_NAME);
        mX2.setValue(createValue(1, 0, VC_VALUE, 0));
        ASSERT(0 == bsl::strcmp(VC_NAME, X2.name()));
        ASSERT(VC_VALUE == X2.value().the<Int64>());
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 6. Set x1 to VA." << endl;
        mX1.setName(VA_NAME);
        mX1.setValue(createValue(0, VA_VALUE, 0, 0));
        ASSERT(0 == bsl::strcmp(VA_NAME, X1.name()));
        ASSERT(VA_VALUE == X1.value().the<int>());
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 7. Create an object x3 (with value VD)."
                           << endl;
        Obj mX3(VD);  const Obj& X3 = mX3;
        ASSERT(0 == bsl::strcmp(VD_NAME, X3.name()));
        ASSERT(VD_VALUE == X3.value().the<string>());
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));

        if (verbose) cout << "\n 8. Create an object x4 (copy from x1)."
                           << endl;
        Obj mX4(X1);  const Obj& X4 = mX4;
        ASSERT(0 == bsl::strcmp(VA_NAME, X4.name()));
        ASSERT(VA_VALUE == X4.value().the<int>());
        ASSERT(1 == (X4 == X1));        ASSERT(0 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(0 == (X4 == X3));        ASSERT(1 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        if (verbose) cout << "\n 9. Assign x2 = x1." << endl;
        mX2 = X1;
        ASSERT(0 == bsl::strcmp(VA_NAME, X2.name()));
        ASSERT(VA_VALUE == X2.value().the<int>());
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(1 == (X2 == X4));        ASSERT(0 == (X2 != X4));

        if (verbose) cout << "\n 10. Assign x2 = x3." << endl;
        mX2 = X3;
        ASSERT(0 == bsl::strcmp(VD_NAME, X2.name()));
        ASSERT(VD_VALUE == X2.value().the<string>());
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        if (verbose) cout << "\n 11. Assign x1 = x1 (aliasing)." << endl;
        mX1 = X1;
        ASSERT(0 == bsl::strcmp(VA_NAME, X1.name()));
        ASSERT(VA_VALUE == X1.value().the<int>());
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));
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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
