// baejsn_parserutil.t.cpp                                            -*-C++-*-
#include <baejsn_parserutil.h>

#include <bsl_sstream.h>
#include <bsl_cfloat.h>
#include <bsl_climits.h>
#include <bsl_limits.h>
#include <bsl_iostream.h>
#include <bdepu_typesparser.h>

#include <bdesb_memoutstreambuf.h>            // for testing only
#include <bdesb_fixedmemoutstreambuf.h>       // for testing only
#include <bdesb_fixedmeminstreambuf.h>        // for testing only

using namespace BloombergLP;
using namespace bsl;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define LOOP0_ASSERT ASSERT

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP1_ASSERT LOOP_ASSERT

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// The 'BSLS_BSLTESTUTIL_EXPAND' macro is required to workaround a
// pre-proccessor issue on windows that prevents __VA_ARGS__ to be expanded in
// the definition of 'BSLS_BSLTESTUTIL_NUM_ARGS'
#define EXPAND(X)                                            \
    X

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)        \
    N

#define NUM_ARGS(...)                                        \
    EXPAND(NUM_ARGS_IMPL( __VA_ARGS__, 5, 4, 3, 2, 1, 0, ""))

#define LOOPN_ASSERT_IMPL(N, ...)                            \
    EXPAND(LOOP ## N ## _ASSERT(__VA_ARGS__))

#define LOOPN_ASSERT(N, ...)                                 \
    LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...)                                         \
    LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

enum { SUCCESS = 0, FAILURE = -1 };

typedef baejsn_ParserUtil Util;

bool areBuffersEqual(const char *lhs, const char *rhs)
    // Compare the data written to the specified 'lhs' with the data in the
    // specified 'rhs' ignoring whitespace in the specified 'lhs'.  Return
    // 'true' if they are equal, and 'false' otherwise.
{
    while (*lhs) {
        if (' ' == *lhs) {
            ++lhs;
            continue;
        }
        if (*lhs != *rhs) {
           return false;
        }
        ++lhs;
        ++rhs;
    }
    return true;
}

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

typedef bsls_PlatformUtil::Int64  Int64;
typedef bsls_PlatformUtil::Uint64 Uint64;

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for double values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout
                     << "\nTESTING 'putValue' and 'getValue' for double"
                     << "\n==========================================="
                     << bsl::endl;
        {
            typedef double Type;

            Type posInf = bsl::numeric_limits<Type>::infinity();
            Type qNaN   = bsl::numeric_limits<Type>::quiet_NaN();
            Type sNaN   = bsl::numeric_limits<Type>::signaling_NaN();

            static const struct {
                int         d_line;   // line number
                Type        d_value;  // unsigned char value
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                // line   value       exp
                // ----   -----       ---
                { L_,     -1.0,          "-1"                            },
                { L_,     -0.1,          "-0.1"                          },
//                 { L_,     -0.1234567,    "-0.1234567"                    },
                { L_,     -1.234567e-35,  "-1.234567e-35"                 },
                { L_,     0.0,           "0"                             },
                { L_,     0.1,           "0.1"                           },
                { L_,     1.0,           "1"                             },
                { L_,     1234567.0,     "1234567"                       },
//                 { L_,     1.234567e35,    "1.234567e+35"                  },
//                 { L_,     posInf,         "+INF"                          },
//                 { L_,    -posInf,         "-INF"                          },
//                 { L_,     sNaN,           "NaN"                           },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE = DATA[i].d_line;
                const Type   VAL  = DATA[i].d_value;
                const string EXP  = DATA[i].d_exp;
                      Type   val;

                if (veryVerbose) { cout << "\nTesting double values" << endl; }
                {
//                     bdesb_MemOutStreamBuf osb;
//                     LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, VAL));
//                     LOOP_ASSERT(LINE, LEN     == osb.length());
//                     LOOP_ASSERT(LINE, areBuffersEqual(EXP, osb.data()));

//                     if (veryVerbose) {
//                         cout << "ACT: ";
//                         P(EXP)
//                     }

                    bdesb_FixedMemInStreamBuf isb(EXP.data(), EXP.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &val));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, val == VAL);
                }
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for float values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout
                     << "\nTESTING 'putValue' and 'getValue' for float"
                     << "\n==========================================="
                     << bsl::endl;
        {
            typedef float Type;

            Type posInf = bsl::numeric_limits<Type>::infinity();
            Type qNaN   = bsl::numeric_limits<Type>::quiet_NaN();
            Type sNaN   = bsl::numeric_limits<Type>::signaling_NaN();

            static const struct {
                int         d_line;   // line number
                Type        d_value;  // unsigned char value
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                // line   value       exp
                // ----   -----       ---
                { L_,     -1.0f,          "-1"                            },
                { L_,     -0.1f,          "-0.1"                          },
                { L_,     -0.1234567f,    "-0.1234567"                    },
                { L_,     -1.234567e-35,  "-1.234567e-35"                 },
                { L_,     0.0f,           "0"                             },
                { L_,     0.1f,           "0.1"                           },
                { L_,     1.0f,           "1"                             },
                { L_,     1234567.0f,     "1234567"                       },
                { L_,     1.234567e35,    "1.234567e+35"                  },
//                 { L_,     posInf,         "+INF"                          },
//                 { L_,    -posInf,         "-INF"                          },
//                 { L_,     sNaN,           "NaN"                           },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE = DATA[i].d_line;
                const Type   VAL  = DATA[i].d_value;
                const string EXP  = DATA[i].d_exp;
                      Type   val;

                if (veryVerbose) { cout << "\nTesting float values" << endl; }
                {
//                     bdesb_MemOutStreamBuf osb;
//                     LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, VAL));
//                     LOOP_ASSERT(LINE, LEN     == osb.length());
//                     LOOP_ASSERT(LINE, areBuffersEqual(EXP, osb.data()));

//                     if (veryVerbose) {
//                         cout << "ACT: ";
//                         P(EXP)
//                     }

                    bdesb_FixedMemInStreamBuf isb(EXP.data(), EXP.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &val));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, val == VAL);
                }
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for Uint64 values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout
                     << "\nTESTING 'putValue' and 'getValue' for Uint64"
                     << "\n============================================"
                     << bsl::endl;
        {
            typedef Uint64 Type;

            static const struct {
                int         d_line;   // line number
                Type        d_value;  // unsigned char value
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                // line   value       exp
                // ----   -----       ---
                {  L_,       0,         "0"       },
                {  L_,       1,         "1"       },
                {  L_,      95,        "95"       },
                {  L_,     127,       "127"       },
                {  L_,     128,       "128"       },
                {  L_,     200,       "200"       },
                {  L_,     255,       "255"       },
                {  L_,   32767,       "32767"       },
                {  L_,   32768,       "32768"       },
                {  L_,   65534,       "65534"       },
                {  L_,   65535,       "65535"       },
                {  L_,   8388607,      "8388607"                },
                {  L_,   8388608,      "8388608"             },
                {  L_,   2147483646,   "2147483646"             },
                {  L_,   2147483647,   "2147483647"             },
                {  L_,   4294967294LL,   "4294967294"             },
                {  L_,   4294967295LL,   "4294967295"             },

                {  L_,       1,         "1.0"     },
                {  L_,       1,         "1.5"     },
                {  L_,       1,         "1.9"     },

                {  L_,       1,         "1e0"      },
                {  L_,       1,         "1E0"      },
                {  L_,       1,         "1e+0"     },
                {  L_,       1,         "1E+0"     },
                {  L_,       1,         "1e-0"     },
                {  L_,       1,         "1E-0"     },

                {  L_,       10,         "1e1"      },
                {  L_,       10,         "1E1"      },
                {  L_,       10,         "1e+1"     },
                {  L_,       10,         "1E+1"     },
                {  L_,       0,          "1e-1"     },
                {  L_,       0,          "1E-1"     },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE = DATA[i].d_line;
                const Type   VAL  = DATA[i].d_value;
                const string EXP  = DATA[i].d_exp;
                      Type   val;

                if (veryVerbose) { cout << "\nTesting Uint64 values" << endl; }
                {
//                     bdesb_MemOutStreamBuf osb;
//                     LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, VAL));
//                     LOOP_ASSERT(LINE, LEN     == osb.length());
//                     LOOP_ASSERT(LINE, areBuffersEqual(EXP, osb.data()));

//                     if (veryVerbose) {
//                         cout << "ACT: ";
//                         P(EXP)
//                     }

                    bdesb_FixedMemInStreamBuf isb(EXP.data(), EXP.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &val));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, val == VAL);
                }
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for Int64 values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout
                     << "\nTESTING 'putValue' and 'getValue' for Int64"
                     << "\n==========================================="
                     << bsl::endl;
        {
            typedef Int64 Type;

            static const struct {
                int         d_line;   // line number
                Type        d_value;  // unsigned char value
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                // line   value       exp
                // ----   -----       ---
                {  L_,       0,         "0"       },
                {  L_,       1,         "1"       },
                {  L_,      95,        "95"       },
                {  L_,     127,       "127"       },
                {  L_,     128,       "128"       },
                {  L_,    -127,       "-127"      },
                {  L_,    -128,       "-128"      },
                {  L_,    -129,       "-129"      },
                {  L_,     200,       "200"       },
                {  L_,     255,       "255"       },
                {  L_,   32767,       "32767"       },
                {  L_,   32768,       "32768"       },
                {  L_,   -32767,       "-32767"       },
                {  L_,   -32768,       "-32768"       },
                {  L_,   -32769,       "-32769"       },
                {  L_,   65534,       "65534"       },
                {  L_,   65535,       "65535"       },
                {  L_,   -65534,       "-65534"       },
                {  L_,   -65535,       "-65535"       },
                {  L_,   -65536,       "-65536"       },
                {  L_,   8388607,      "8388607"                },
                {  L_,   8388608,      "8388608"             },
                {  L_,   -8388608,     "-8388608"                },
                {  L_,   -8388609,     "-8388609"             },
                {  L_,   2147483646,   "2147483646"             },
                {  L_,   2147483647,   "2147483647"             },
                {  L_,   -2147483647,  "-2147483647"             },
                {  L_,   -2147483648LL,  "-2147483648"             },
                {  L_,   4294967294LL,   "4294967294"             },
                {  L_,   4294967295LL,   "4294967295"             },
                {  L_,   -4294967295LL,   "-4294967295"             },
                {  L_,   -4294967296LL,   "-4294967296"             },

                {  L_,       1,         "1.0"     },
                {  L_,       1,         "1.5"     },
                {  L_,       1,         "1.9"     },

                {  L_,       1,         "1e0"      },
                {  L_,       1,         "1E0"      },
                {  L_,       1,         "1e+0"     },
                {  L_,       1,         "1E+0"     },
                {  L_,       1,         "1e-0"     },
                {  L_,       1,         "1E-0"     },

                {  L_,       10,         "1e1"      },
                {  L_,       10,         "1E1"      },
                {  L_,       10,         "1e+1"     },
                {  L_,       10,         "1E+1"     },
                {  L_,       0,          "1e-1"     },
                {  L_,       0,          "1E-1"     },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE = DATA[i].d_line;
                const Type   VAL  = DATA[i].d_value;
                const string EXP  = DATA[i].d_exp;
                      Type   val;

                if (veryVerbose) { cout << "\nTesting Int64 values" << endl; }
                {
//                     bdesb_MemOutStreamBuf osb;
//                     LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, VAL));
//                     LOOP_ASSERT(LINE, LEN     == osb.length());
//                     LOOP_ASSERT(LINE, areBuffersEqual(EXP, osb.data()));

//                     if (veryVerbose) {
//                         cout << "ACT: ";
//                         P(EXP)
//                     }

                    bdesb_FixedMemInStreamBuf isb(EXP.data(), EXP.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &val));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, val == VAL);
                }
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for unsigned int values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout
                     << "\nTESTING 'putValue' and 'getValue' for unsigned int"
                     << "\n=================================================="
                     << bsl::endl;
        {
            typedef unsigned int Type;

            static const struct {
                int         d_line;   // line number
                Type        d_value;  // unsigned char value
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                // line   value       exp
                // ----   -----       ---
                {  L_,       0,         "0"       },
                {  L_,       1,         "1"       },
                {  L_,      95,        "95"       },
                {  L_,     127,       "127"       },
                {  L_,     128,       "128"       },
                {  L_,     200,       "200"       },
                {  L_,     255,       "255"       },
                {  L_,   32767,       "32767"       },
                {  L_,   32768,       "32768"       },
                {  L_,   65534,       "65534"       },
                {  L_,   65535,       "65535"       },
                {  L_,   8388607,      "8388607"                },
                {  L_,   8388608,      "8388608"             },
                {  L_,   2147483646,   "2147483646"             },
                {  L_,   2147483647,   "2147483647"             },
                {  L_,   4294967294,   "4294967294"             },
                {  L_,   4294967295,   "4294967295"             },

                {  L_,       1,         "1.0"     },
                {  L_,       1,         "1.5"     },
                {  L_,       1,         "1.9"     },

                {  L_,       1,         "1e0"      },
                {  L_,       1,         "1E0"      },
                {  L_,       1,         "1e+0"     },
                {  L_,       1,         "1E+0"     },
                {  L_,       1,         "1e-0"     },
                {  L_,       1,         "1E-0"     },

                {  L_,       10,         "1e1"      },
                {  L_,       10,         "1E1"      },
                {  L_,       10,         "1e+1"     },
                {  L_,       10,         "1E+1"     },
                {  L_,       0,          "1e-1"     },
                {  L_,       0,          "1E-1"     },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE = DATA[i].d_line;
                const Type   VAL  = DATA[i].d_value;
                const string EXP  = DATA[i].d_exp;
                      Type   val;

                if (veryVerbose) { cout << "\nTesting unsigned int values"
                                        << endl; }
                {
//                     bdesb_MemOutStreamBuf osb;
//                     LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, VAL));
//                     LOOP_ASSERT(LINE, LEN     == osb.length());
//                     LOOP_ASSERT(LINE, areBuffersEqual(EXP, osb.data()));

//                     if (veryVerbose) {
//                         cout << "ACT: ";
//                         P(EXP)
//                     }

                    bdesb_FixedMemInStreamBuf isb(EXP.data(), EXP.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &val));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, val == VAL);
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for int values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout
                     << "\nTESTING 'putValue' and 'getValue' for int"
                     << "\n========================================="
                     << bsl::endl;
        {
            typedef int Type;

            static const struct {
                int         d_line;   // line number
                Type        d_value;  // unsigned char value
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                // line   value       exp
                // ----   -----       ---
                {  L_,       0,         "0"       },
                {  L_,       1,         "1"       },
                {  L_,      95,        "95"       },
                {  L_,     127,       "127"       },
                {  L_,     128,       "128"       },
                {  L_,    -127,       "-127"      },
                {  L_,    -128,       "-128"      },
                {  L_,    -129,       "-129"      },
                {  L_,     200,       "200"       },
                {  L_,     255,       "255"       },
                {  L_,   32767,       "32767"       },
                {  L_,   32768,       "32768"       },
                {  L_,   -32767,       "-32767"       },
                {  L_,   -32768,       "-32768"       },
                {  L_,   -32769,       "-32769"       },
                {  L_,   65534,       "65534"       },
                {  L_,   65535,       "65535"       },
                {  L_,   -65534,       "-65534"       },
                {  L_,   -65535,       "-65535"       },
                {  L_,   -65536,       "-65536"       },
                {  L_,   8388607,      "8388607"                },
                {  L_,   8388608,      "8388608"             },
                {  L_,   -8388608,     "-8388608"                },
                {  L_,   -8388609,     "-8388609"             },
                {  L_,   2147483646,   "2147483646"             },
                {  L_,   2147483647,   "2147483647"             },
                {  L_,   -2147483647,  "-2147483647"             },
                {  L_,   -2147483648,  "-2147483648"             },

                {  L_,       1,         "1.0"     },
                {  L_,       1,         "1.5"     },
                {  L_,       1,         "1.9"     },

                {  L_,       1,         "1e0"      },
                {  L_,       1,         "1E0"      },
                {  L_,       1,         "1e+0"     },
                {  L_,       1,         "1E+0"     },
                {  L_,       1,         "1e-0"     },
                {  L_,       1,         "1E-0"     },

                {  L_,       10,         "1e1"      },
                {  L_,       10,         "1E1"      },
                {  L_,       10,         "1e+1"     },
                {  L_,       10,         "1E+1"     },
                {  L_,       0,          "1e-1"     },
                {  L_,       0,          "1E-1"     },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE = DATA[i].d_line;
                const Type   VAL  = DATA[i].d_value;
                const string EXP  = DATA[i].d_exp;
                      Type   val;

                if (veryVerbose) { cout << "\nTesting int values" << endl; }
                {
//                     bdesb_MemOutStreamBuf osb;
//                     LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, VAL));
//                     LOOP_ASSERT(LINE, LEN     == osb.length());
//                     LOOP_ASSERT(LINE, areBuffersEqual(EXP, osb.data()));

//                     if (veryVerbose) {
//                         cout << "ACT: ";
//                         P(EXP)
//                     }

                    bdesb_FixedMemInStreamBuf isb(EXP.data(), EXP.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &val));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, val == VAL);
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for unsigned short values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout
                    << "\nTESTING 'putValue' and 'getValue' for unsigned short"
                    << "\n===================================================="
                    << bsl::endl;
        {
            typedef unsigned short Type;

            static const struct {
                int         d_line;   // line number
                Type        d_value;  // unsigned char value
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                // line   value       exp
                // ----   -----       ---
                {  L_,       0,         "0"       },
                {  L_,       1,         "1"       },
                {  L_,      95,        "95"       },
                {  L_,     127,       "127"       },
                {  L_,     128,       "128"       },
                {  L_,     200,       "200"       },
                {  L_,     255,       "255"       },
                {  L_,   32767,       "32767"       },
                {  L_,   32768,       "32768"       },
                {  L_,   65534,       "65534"       },
                {  L_,   65535,       "65535"       },

                {  L_,       1,         "1.0"     },
                {  L_,       1,         "1.5"     },
                {  L_,       1,         "1.9"     },

                {  L_,       1,         "1e0"      },
                {  L_,       1,         "1E0"      },
                {  L_,       1,         "1e+0"     },
                {  L_,       1,         "1E+0"     },
                {  L_,       1,         "1e-0"     },
                {  L_,       1,         "1E-0"     },

                {  L_,       10,         "1e1"      },
                {  L_,       10,         "1E1"      },
                {  L_,       10,         "1e+1"     },
                {  L_,       10,         "1E+1"     },
                {  L_,       0,          "1e-1"     },
                {  L_,       0,          "1E-1"     },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE = DATA[i].d_line;
                const Type   VAL  = DATA[i].d_value;
                const string EXP  = DATA[i].d_exp;
                      Type   val;

                if (veryVerbose) { cout << "\nTesting short values" << endl; }
                {
//                     bdesb_MemOutStreamBuf osb;
//                     LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, VAL));
//                     LOOP_ASSERT(LINE, LEN     == osb.length());
//                     LOOP_ASSERT(LINE, areBuffersEqual(EXP, osb.data()));

//                     if (veryVerbose) {
//                         cout << "ACT: ";
//                         P(EXP)
//                     }

                    bdesb_FixedMemInStreamBuf isb(EXP.data(), EXP.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &val));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, val == VAL);
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for short values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout
                     << "\nTESTING 'putValue' and 'getValue' for short"
                     << "\n==========================================="
                     << bsl::endl;
        {
            typedef short Type;

            static const struct {
                int         d_line;   // line number
                Type        d_value;  // unsigned char value
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                // line   value       exp
                // ----   -----       ---
                {  L_,       0,         "0"       },
                {  L_,       1,         "1"       },
                {  L_,      95,        "95"       },
                {  L_,     127,       "127"       },
                {  L_,     128,       "128"       },
                {  L_,    -127,       "-127"      },
                {  L_,    -128,       "-128"      },
                {  L_,    -129,       "-129"      },
                {  L_,     200,       "200"       },
                {  L_,     255,       "255"       },
                {  L_,   32767,       "32767"       },
                {  L_,   32768,       "32768"       },
                {  L_,   -32767,       "-32767"       },

                {  L_,       1,         "1.0"     },
                {  L_,       1,         "1.5"     },
                {  L_,       1,         "1.9"     },

                {  L_,       1,         "1e0"      },
                {  L_,       1,         "1E0"      },
                {  L_,       1,         "1e+0"     },
                {  L_,       1,         "1E+0"     },
                {  L_,       1,         "1e-0"     },
                {  L_,       1,         "1E-0"     },

                {  L_,       10,         "1e1"      },
                {  L_,       10,         "1E1"      },
                {  L_,       10,         "1e+1"     },
                {  L_,       10,         "1E+1"     },
                {  L_,       0,          "1e-1"     },
                {  L_,       0,          "1E-1"     },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE = DATA[i].d_line;
                const Type   VAL  = DATA[i].d_value;
                const string EXP  = DATA[i].d_exp;
                      Type   val;

                if (veryVerbose) { cout << "\nTesting short values" << endl; }
                {
//                     bdesb_MemOutStreamBuf osb;
//                     LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, VAL));
//                     LOOP_ASSERT(LINE, LEN     == osb.length());
//                     LOOP_ASSERT(LINE, areBuffersEqual(EXP, osb.data()));

//                     if (veryVerbose) {
//                         cout << "ACT: ";
//                         P(EXP)
//                     }

                    bdesb_FixedMemInStreamBuf isb(EXP.data(), EXP.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &val));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, val == VAL);
                }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for unsigned char values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout
                     << "\nTESTING 'putValue' and 'getValue' for unsigned char"
                     << "\n==================================================="
                     << bsl::endl;
        {
            typedef unsigned char Type;

            static const struct {
                int         d_line;   // line number
                Type        d_value;  // unsigned char value
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                // line   value       exp
                // ----   -----       ---
                {  L_,       0,         "0"       },
                {  L_,       1,         "1"       },
                {  L_,      95,        "95"       },
                {  L_,     127,       "127"       },
                {  L_,     128,       "128"       },
                {  L_,     200,       "200"       },
                {  L_,     255,       "255"       },

                {  L_,       1,         "1.0"     },
                {  L_,       1,         "1.5"     },
                {  L_,       1,         "1.9"     },

                {  L_,       1,         "1e0"      },
                {  L_,       1,         "1E0"      },
                {  L_,       1,         "1e+0"     },
                {  L_,       1,         "1E+0"     },
                {  L_,       1,         "1e-0"     },
                {  L_,       1,         "1E-0"     },

                {  L_,       10,         "1e1"      },
                {  L_,       10,         "1E1"      },
                {  L_,       10,         "1e+1"     },
                {  L_,       10,         "1E+1"     },
                {  L_,       0,          "1e-1"     },
                {  L_,       0,          "1E-1"     },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE = DATA[i].d_line;
                const Type   VAL  = DATA[i].d_value;
                const string EXP  = DATA[i].d_exp;
                      Type   val;

                if (veryVerbose) { cout << "\nTesting unsigned char values"
                                        << endl; }
                {
//                     bdesb_MemOutStreamBuf osb;
//                     LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, VAL));
//                     LOOP_ASSERT(LINE, LEN     == osb.length());
//                     LOOP_ASSERT(LINE, areBuffersEqual(EXP, osb.data()));

//                     if (veryVerbose) {
//                         cout << "ACT: ";
//                         P(EXP)
//                     }

                    bdesb_FixedMemInStreamBuf isb(EXP.data(), EXP.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &val));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, val == VAL);
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for signed char values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout
                     << "\nTESTING 'putValue' and 'getValue' for signed char"
                     << "\n================================================="
                     << bsl::endl;

        {
            static const struct {
                int         d_line;   // line number
                char        d_value;  // char value
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                // line   value       exp
                // ----   -----       ---
                {  L_,      '0',      "\"0\""   },
                {  L_,      '1',      "\"1\""   },
                {  L_,      'A',      "\"A\""   },
                {  L_,      'Z',      "\"Z\""   },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int            LINE = DATA[i].d_line;
                const char           C    = DATA[i].d_value;
                const signed char    SC   = (signed char) DATA[i].d_value;
                const string         EXP  = DATA[i].d_exp;
                      char           c;
                      signed char    sc;

                if (veryVerbose) { cout << "\nTesting char values" << endl; }
                {
//                     bdesb_MemOutStreamBuf osb;
//                     LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, C));
//                     LOOP_ASSERT(LINE, LEN     == osb.length());
//                     LOOP_ASSERT(LINE, areBuffersEqual(EXP, osb.data()));

//                     if (veryVerbose) {
//                         cout << "ACT: ";
//                         P(EXP)
//                     }

                    bdesb_FixedMemInStreamBuf isb(EXP.data(), EXP.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &c));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, c == C);
                }

                if (veryVerbose) { cout << "\nTesting signed char values"
                                        << endl; }
                {
//                     bdesb_MemOutStreamBuf osb;
//                     LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, SC));
//                     LOOP_ASSERT(LINE, LEN     == osb.length());
//                     LOOP_ASSERT(LINE, 0       ==
//                                               compareBuffers(osb.data(), EXP));

//                     if (veryVerbose) {
//                         P(EXP)
//                         cout << "ACT: ";
//                         printBuffer(osb.data(), osb.length());
//                     }

                    bdesb_FixedMemInStreamBuf isb(EXP.data(), EXP.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &sc));
                    LOOP_ASSERT(LINE, 0  == isb.length());
                    LOOP_ASSERT(LINE, sc == SC);
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for bool values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout
                         << "\nTESTING 'putValue' and 'getValue' for bool"
                         << "\n=========================================="
                         << bsl::endl;

        {
            typedef bool Type;

            const Type XA1 = true;  Type XA2; const string EA = "true";
            const Type XB1 = false; Type XB2; const string EB = "false";

            {
//                 bdesb_MemOutStreamBuf osb;
//                 ASSERT(SUCCESS  == Util::putValue(&osb, XA1));
//                 ASSERT(4        == osb.length());
//                 ASSERT(areBuffersEqual(EA, osb.data()));

//                 if (veryVerbose) {
//                     P(osb.str());
//                 }

                bdesb_FixedMemInStreamBuf isb(EA.data(), EA.length());
                ASSERT(SUCCESS == Util::getValue(&isb, &XA2));
                ASSERT(0       == isb.length());
                ASSERT(XA1     == XA2);
            }

            {
//                 bdesb_MemOutStreamBuf osb;
//                 ASSERT(SUCCESS  == Util::putValue(&osb, XA1));
//                 ASSERT(4        == osb.length());
//                 ASSERT(areBuffersEqual(EA, osb.data()));

//                 if (veryVerbose) {
//                     P(osb.str());
//                 }

                bdesb_FixedMemInStreamBuf isb(EB.data(), EB.length());
                ASSERT(SUCCESS == Util::getValue(&isb, &XB2));
                ASSERT(0       == isb.length());
                ASSERT(XB1     == XB2);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const struct {
            int         d_line;
            const char* d_string;
            bool        d_validFlag;
            double      d_result;
        } DATA[] = {
            //LINE        STRING    FLAG      RESULTS
            //----        ------    ----      -------

            { L_,   "-1.625e308",   true,  -1.625e308 },
            { L_,    "-3.125e38",   true,   -3.125e38 },
            { L_,         "-1.0",   true,        -1.0 },
            { L_,         "-0.0",   true,        -0.0 },
            { L_,            "0",   true,         0.0 },
            { L_,          "0.0",   true,         0.0 },
            { L_,          "1.0",   true,         1.0 },
            { L_,     "3.125e38",   true,    3.125e38 },
            { L_,    "1.625e308",   true,   1.625e308 },
            { L_,          "0e0",   true,         0.0 },
            { L_,          "1e0",   true,         1.0 },
            { L_,         "-1.5",   true,        -1.5 },
            { L_,     "-1.25e-3",   true,    -1.25e-3 },
            { L_,     "9.25e+10",   true,     9.25e10 },
            { L_,           ".1",  false,         0.0 },
            { L_,           "1.",  false,         0.0 },
            { L_,           "+1",  false,         0.0 },
            { L_,          "--1",  false,         0.0 },
            { L_,        "1e+-1",  false,         0.0 }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const STRING = DATA[ti].d_string;
            const bool        FLAG   = DATA[ti].d_validFlag;
            const double      EXP    = DATA[ti].d_result;

            bsl::istringstream iss(STRING);

            double result;
            ASSERTV(LINE, FLAG == (0 == Util::getNumber(iss.rdbuf(),
                                                        &result)));

            if (FLAG) {
                ASSERTV(LINE, result, EXP, result == EXP);
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
