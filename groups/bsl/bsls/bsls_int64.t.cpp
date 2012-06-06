// bsls_int64.t.cpp                                                   -*-C++-*-

#include <bsls_int64.h>

#include <bsls_platform.h>  // for testing only

#include <cstddef>     // offsetof()
#include <cstdlib>     // atoi()
#include <cstring>     // memset(), memcmp(), strlen()
#include <iostream>
#include <stdio.h>     // sprintf(), snprintf() [NOT <cstdio>, which does not
                       // include 'snprintf']

#if defined(BSLS_PLATFORM__CMP_IBM) && !defined(BSLS_PLATFORM__CPU_64_BIT)
    #define BSLS_INT64_TEST__NO_64_BIT_CONSTANTS 1
#endif

#if defined(BSLS_PLATFORM__CMP_MSVC)
#define snprintf _snprintf
#endif

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This component implements platform-specific patches in order to provide
// platform-independent 64-bit integer functionality.
//-----------------------------------------------------------------------------
// [ 2] Int64
// [ 2] Uint64
// [ 4] operator<<(ostream&, const bsls::PlatformUtil::Uint64&);
// [ 3] operator<<(ostream&, const bsls::PlatformUtil::Int64&);
//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define TAB cout << '\t';                     // output the tab character.
#define L_ __LINE__                           // current Line number

//==========================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
//--------------------------------------------------------------------------

#if defined(BSLS_PLATFORM__CMP_GNU)
    #define BSLS_INT64_TEST__NO_64_BIT_CONSTANTS 1
#endif

#if defined(BSLS_PLATFORM__CMP_MSVC)
#define INT64_FMT_STR  "0x%I64X"
#else
#define INT64_FMT_STR  "0x%llX"
#endif

char *hex64(char *buffer, bsls_Int64::Uint64 value)
    // Convert the specified 64-bit 'value' to a hexadecimal string.
{
    sprintf(buffer, INT64_FMT_STR, value);
    return buffer;
}

void printBits(bsls_Int64::Uint64 value)
    // Print the individual bits of the specified 64-bit 'value'.
{
    char *bytes = reinterpret_cast<char *>(&value);
    for (int i = 0; i < 8; ++i) {
        cout << !!(bytes[i] & 0x80)
             << !!(bytes[i] & 0x40)
             << !!(bytes[i] & 0x20)
             << !!(bytes[i] & 0x10)
             << !!(bytes[i] & 0x08)
             << !!(bytes[i] & 0x04)
             << !!(bytes[i] & 0x02)
             << !!(bytes[i] & 0x01)
             << ' ';
    }
    cout << endl;
}

#undef INT64_FMT_STR

//--------------------------------------------------------------------------

//==========================================================================
//                   SUPPORTING FUNCTIONS USED FOR TESTING
//--------------------------------------------------------------------------

#if 0
static bool isBigEndian()
    // Return 'true' if this machine is observed to be big endian, and 'false'
    // otherwise.  Internally, assert that this machine is observed to be
    // either big endian or little endian.
{
    union U {
        int d_int;
        char d_char[sizeof(int)];
    } u;

    u.d_int = 0x1;

    if (u.d_char[0] == 0x1) {
        return false;
    }
    else if (u.d_char[sizeof(int) - 1] == 0x1) {
        return true;
    }
    else {
        ASSERT(0);      // Neither big endian nor little endian!
    }
    return false;
}

//--------------------------------------------------------------------------

static bool isLittleEndian()
    // Return 'true' if this machine is observed to be little endian, and
    // 'false' otherwise.  Internally, assert that this machine is observed to
    // be either big endian or little endian.
{
    union U {
        int d_int;
        char d_char[sizeof(int)];
    } u;

    u.d_int = 0x1;

    if (u.d_char[0] == 0x1) {
        return true;
    }
    else if (u.d_char[sizeof(int) - 1] == 0x1) {
        return false;
    }
    else {
        ASSERT(0);      // Neither big endian nor little endian!
    }
    return false;
}
#endif

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //   Since the output operator is layered on basic accessors, it is
        //   sufficient to test only output *format* (and to ensure that no
        //   additional characters are written past the terminating null).
        //
        // Plan:
        //   For each of a small representative set of object values, ordered
        //   by increasing length, use 'ostrstream' to write that object's
        //   value to two separate character buffers each with different
        //   initial values.  Compare the contents of these buffers with the
        //   literal expected output format and verify that the characters
        //   beyond the null characters are unaffected in both buffers.
        //   On platforms that support 64-bit integer constants, test those
        //   constants directly.  Additionally generated the same 64-bit
        //   constants by logically combining two 32-bit constants and test
        //   those as well.
        //
        // Testing:
        //   operator<<(ostream&, const bsls_Int64::Uint64&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing 64-bit unsigned integer streaming support." << endl
            << "==================================================" << endl;

        typedef bsls_Int64::Uint64 T;
        typedef bsls_Int64::Int64  S;

        static const struct {
            int         d_lineNum;    // source line number
            T           d_spec;       // specification string
            const char *d_fmt_p;      // expected output format
        } DATA[] = {
            //line  spec               output format
            //----  -----------------  -------------------------
            { L_,    0,                 "0"                    },
            { L_,    1,                 "1"                    },
            { L_,   0x7F,               "127"                  },
            { L_,   0x80,               "128"                  },
            { L_,   0xFF,               "255"                  },
            { L_,   0x100,              "256"                  },
            { L_,   0x7FFF,             "32767"                },
            { L_,   0x8000,             "32768"                },
            { L_,   0xFFFF,             "65535"                },
            { L_,   0x10000,            "65536"                },
            { L_,   0x7FFFFFFF,         "2147483647"           },
            { L_,   0x80000000,         "2147483648"           },
            { L_,   0xFFFFFFFF,         "4294967295"           },
#if !defined(BSLS_INT64_TEST__NO_64_BIT_CONSTANTS)
            { L_,   0x100000000,        "4294967296"           },
            { L_,   0x7FFFFFFFFFFFFFFF, "9223372036854775807"  },
            { L_,   0x8000000000000000, "9223372036854775808"  },
            { L_,   0xFFFFFFFFFFFFFFFF, "18446744073709551615" },  // unsigned
#endif
            { L_,   (T) (S) -1,         "18446744073709551615" },  // unsigned
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int SIZE = 1000;
        const char Z1 = (char) 0xFF;
        const char Z2 = 0x00;

        char buf64[24];
        char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
        char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
        const char *CTRL_BUF1 = mCtrlBuf1;
        const char *CTRL_BUF2 = mCtrlBuf2;

        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE        = DATA[ti].d_lineNum;
                const T SPEC          = DATA[ti].d_spec;
                const char *const FMT = DATA[ti].d_fmt_p;

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE);  // Preset buf1 to Z1 values
                memcpy(buf2, CTRL_BUF2, SIZE);  // Preset buf1 to Z2 values

                if (verbose) cout << "\tSpec = \t\t      \""
                                  << hex64(buf64, SPEC) << '"' << endl;
                if (veryVerbose)
                    cout << "\tEXPECTED FORMAT     : " << FMT << endl;

                #if defined(BSLS_PLATFORM__CMP_MSVC)
                snprintf(buf1, SIZE, "%I64u", SPEC);
                snprintf(buf2, SIZE, "%I64u", SPEC);
                #else
                snprintf(buf1, SIZE, "%llu", SPEC);
                snprintf(buf2, SIZE, "%llu", SPEC);
                #endif

                if (veryVerbose)
                    cout << "\tACTUAL FORMAT (buf1): " << buf1 << endl
                         << "\tACTUAL FORMAT (buf2): " << buf2 << endl << endl;

                const int SZ = (int) strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf1 + SZ,
                                              CTRL_BUF1 + SZ,
                                              REST));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2 + SZ,
                                              CTRL_BUF2 + SZ,
                                              REST));
            }
        }

        if (verbose) cout << "\nTesting bit-shifted values" << endl;

        static const struct {
            int         d_lineNum;
            T           d_hi;
            T           d_lo;
            const char *d_fmt_p;
        } DATA2[] = {
            //line  spec_hi     spec_lo          output format
            //----  ----------  ----------  -------------------------
            { L_,   0x00000001, 0x00000000, "4294967296"           },
            { L_,   0x7FFFFFFF, 0xFFFFFFFF, "9223372036854775807"  },
            { L_,   0x80000000, 0x00000000, "9223372036854775808"  },
            { L_,   0xFFFFFFFF, 0xFFFFFFFF, "18446744073709551615" },
        };

        const int NUM_DATA2 = sizeof DATA2 / sizeof *DATA2;

        {
            for (int ti = 0; ti < NUM_DATA2; ++ti) {
                const int LINE         = DATA2[ti].d_lineNum;
                const T HI             = DATA2[ti].d_hi;
                const T LO             = DATA2[ti].d_lo;
                const char *const FMT  = DATA2[ti].d_fmt_p;
                const T SPEC = (HI << 32) | LO;

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE);  // Preset buf1 to Z1 values
                memcpy(buf2, CTRL_BUF2, SIZE);  // Preset buf1 to Z2 values

                if (verbose) cout << "\tSpec = \t\t      \""
                                  << hex64(buf64, SPEC) << '"' << endl;
                if (veryVerbose)
                    cout << "\tEXPECTED FORMAT     : " << FMT << endl;

                #if defined(BSLS_PLATFORM__CMP_MSVC)
                snprintf(buf1, SIZE, "%I64u", SPEC);
                snprintf(buf2, SIZE, "%I64u", SPEC);
                #else
                snprintf(buf1, SIZE, "%llu", SPEC);
                snprintf(buf2, SIZE, "%llu", SPEC);
                #endif

                if (veryVerbose)
                    cout << "\tACTUAL FORMAT (buf1): " << buf1 << endl
                         << "\tACTUAL FORMAT (buf2): " << buf2 << endl << endl;

                const int SZ = (int) strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf1 + SZ,
                                              CTRL_BUF1 + SZ,
                                              REST));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //   Since the output operator is layered on basic accessors, it is
        //   sufficient to test only output *format* (and to ensure that no
        //   additional characters are written past the terminating null).
        //
        // Plan:
        //   For each of a small representative set of object values, ordered
        //   by increasing length, use 'ostrstream' to write that object's
        //   value to two separate character buffers each with different
        //   initial values.  Compare the contents of these buffers with the
        //   literal expected output format and verify that the characters
        //   beyond the null characters are unaffected in both buffers.
        //   On platforms that support 64-bit integer constants, test those
        //   constants directly.  Additionally generated the same 64-bit
        //   constants by logically combining two 32-bit constants and test
        //   those as well.
        //
        // Testing:
        //   operator<<(ostream&, const bsls_Int64::Int64&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing 64-bit signed integer streaming support." << endl
            << "================================================" << endl;

        typedef bsls_Int64::Int64 T;

        static const struct {
            int         d_lineNum;    // source line number
            T           d_spec;       // specification string
            const char *d_fmt_p;      // expected output format
        } DATA[] = {
            //line  spec               output format
            //----  -----------------  -------------------------
            { L_,    0,                 "0"                    },
            { L_,    1,                 "1"                    },
            { L_,   0x7F,               "127"                  },
            { L_,   0x80,               "128"                  },
            { L_,   0xFF,               "255"                  },
            { L_,   0x100,              "256"                  },
            { L_,   0x7FFF,             "32767"                },
            { L_,   0x8000,             "32768"                },
            { L_,   0xFFFF,             "65535"                },
            { L_,   0x10000,            "65536"                },
            { L_,   0x7FFFFFFF,         "2147483647"           },
            { L_,   0x80000000,         "2147483648"           },
            { L_,   0xFFFFFFFF,         "4294967295"           },
#if !defined(BSLS_INT64_TEST__NO_64_BIT_CONSTANTS)
            { L_,   0x100000000,        "4294967296"           },
            { L_,   0x7FFFFFFFFFFFFFFF, "9223372036854775807"  },
            { L_,   0x8000000000000000, "-9223372036854775808" },
            { L_,   0xFFFFFFFFFFFFFFFF, "-1"                   },  // signed
#endif
            { L_,   -1,                 "-1"                   },  // signed
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int SIZE = 1000;
        const char Z1 = (char) 0xFF;
        const char Z2 = 0x00;

        char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
        char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
        const char *CTRL_BUF1 = mCtrlBuf1;
        const char *CTRL_BUF2 = mCtrlBuf2;

        int ti;
        for (ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE        = DATA[ti].d_lineNum;
            const T SPEC          = DATA[ti].d_spec;
            const char *const FMT = DATA[ti].d_fmt_p;

            char buf64[24];
            char buf1[SIZE], buf2[SIZE];
            memcpy(buf1, CTRL_BUF1, SIZE);  // Preset buf1 to Z1 values
            memcpy(buf2, CTRL_BUF2, SIZE);  // Preset buf1 to Z2 values

            if (verbose)
                cout << "\tSpec = \"" << hex64(buf64, SPEC) << '"' << endl;
            if (veryVerbose)
                cout << "EXPECTED FORMAT:" << endl << FMT << endl;

            #if defined(BSLS_PLATFORM__CMP_MSVC)
            snprintf(buf1, SIZE, "%I64d", SPEC);
            snprintf(buf2, SIZE, "%I64d", SPEC);
            #else
            snprintf(buf1, SIZE, "%lld", SPEC);
            snprintf(buf2, SIZE, "%lld", SPEC);
            #endif

            if (veryVerbose)
                cout << "ACTUAL FORMAT:" << endl << buf1 << endl;

            const int SZ = (int) strlen(FMT) + 1;
            const int REST = SIZE - SZ;
            LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
            LOOP_ASSERT(LINE, Z1 == buf1[SIZE - 1]);  // Check for overrun.
            LOOP_ASSERT(LINE, Z2 == buf2[SIZE - 1]);  // Check for overrun.
            LOOP_ASSERT(LINE, 0 == memcmp(buf1, FMT, SZ));
            LOOP_ASSERT(LINE, 0 == memcmp(buf2, FMT, SZ));
            LOOP_ASSERT(LINE, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
            LOOP_ASSERT(LINE, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
        }

        if (verbose) cout << "\nTesting generated values" << endl;

        static const struct {
            int         d_lineNum;
            T           d_hi;
            T           d_lo;
            const char *d_fmt_p;
        } DATA2[] = {
            //line  spec_hi     spec_lo          output format
            //----  ----------  ----------  ------------------------
            { L_,   0x00000001, 0x00000000, "4294967296"           },
            { L_,   0x7FFFFFFF, 0xFFFFFFFF, "9223372036854775807"  },
            { L_,   0x80000000, 0x00000000, "-9223372036854775808" },
            { L_,   0xFFFFFFFF, 0xFFFFFFFF, "-1"                   },
        };

        const int NUM_DATA2 = sizeof DATA2 / sizeof *DATA2;

        for (ti = 0; ti < NUM_DATA2; ++ti) {
            const int LINE         = DATA2[ti].d_lineNum;
            const T HI             = DATA2[ti].d_hi;
            const T LO             = DATA2[ti].d_lo;
            const char *const FMT  = DATA2[ti].d_fmt_p;
            T SPEC = HI << 32 | LO;

            char buf64[24];
            char buf1[SIZE], buf2[SIZE];
            memcpy(buf1, CTRL_BUF1, SIZE);  // Preset buf1 to Z1 values
            memcpy(buf2, CTRL_BUF2, SIZE);  // Preset buf1 to Z2 values

            if (verbose) cout << "\tSpec = \t\t      \""
                              << hex64(buf64, SPEC) << '"' << endl;
            if (veryVerbose)
                cout << "\tSpec = \t\t      " << SPEC << endl
                     << "\tEXPECTED FORMAT     : " << FMT << endl;

            #if defined(BSLS_PLATFORM__CMP_MSVC)
            snprintf(buf1, SIZE, "%I64d", SPEC);
            snprintf(buf2, SIZE, "%I64d", SPEC);
            #else
            snprintf(buf1, SIZE, "%lld", SPEC);
            snprintf(buf2, SIZE, "%lld", SPEC);
            #endif

            if (veryVerbose) {
                cout << "\tACTUAL FORMAT (buf1): " << buf1 << endl;
                cout << "\tACTUAL FORMAT (buf2): " << buf2 << endl;
            }

            const int SZ = (int) strlen(FMT) + 1;
            const int REST = SIZE - SZ;
            LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
            LOOP_ASSERT(LINE, Z1 == buf1[SIZE - 1]);  // Check for overrun.
            LOOP_ASSERT(LINE, Z2 == buf2[SIZE - 1]);  // Check for overrun.
            LOOP_ASSERT(LINE, 0 == memcmp(buf1, FMT, SZ));
            LOOP_ASSERT(LINE, 0 == memcmp(buf2, FMT, SZ));
            LOOP_ASSERT(LINE, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TYPEDEFS FOR 64-BIT FUNDAMENTAL DATA TYPES:
        //   Concerns:
        //     Since the Int64 and Uint64 types are generated, it is
        //     possible that the typedef statements concerned for those types
        //     were wrong.  We must ensure that:
        //     - both Int64 and Uint64 are both 64-bits wide.
        //     - the sign bit is interpreted properly for both Int64 and
        //       Uint64.
        // Plan:
        //   First measure the size of each of the types, ensuring that both
        //   Int64 and Uint64 are 64-bits wide.
        //
        //   Next, ensure the sign-bit is interpreted correctly by assigning
        //   a negative number to a variable of each type and ensuring that
        //   the value stored in the Int64 variable is negative and that the
        //   value stored in the Uint64 variable is positive.
        //
        // Testing:
        //   Int64
        //   Uint64
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing Int64 & Uint64" << endl
            << "======================" << endl;

        typedef bsls_Int64 I64;

        // Must be right size.
        ASSERT(8 == sizeof(I64::Int64));
        ASSERT(8 == sizeof(I64::Uint64));

        // Must be types.
        I64::Int64  int64  = -1;
        I64::Uint64 uint64 = (I64::Uint64) (I64::Int64) -1;

        // Must interpret sign bit correctly.
        ASSERT(int64 < (I64::Int64) 0); // cast addresses dgintel compiler bug
        ASSERT(uint64 > 0);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING
        //   Concerns:
        //   Plan:
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing ." << endl
            << "====================================================" << endl;

        typedef bsls_Int64 I64;

        if (verbose) cout << " " << endl;

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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
