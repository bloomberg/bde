// bsls_types.t.cpp                                                   -*-C++-*-

#include <bsls_types.h>

#include <bsls_platform.h>     // for testing only

#include <cstddef>     // offsetof(), std::ptrdiff_t
#include <cstdlib>     // atoi()
#include <cstring>     // memset(), memcmp(), strlen()
#include <iostream>
#include <limits>
#include <stdio.h>     // sprintf(), snprintf() [NOT <cstdio>, which does not
                       // include 'snprintf']

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf
#endif

#ifdef BSLS_PLATFORM_OS_UNIX
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This component implements several types for a platform.  A platform is
// defined as the unique combination of compiler, operating system, and
// processor (CPU).  We will test each of the types, verifying that their
// 'typedef' has been properly implemented for a given platform.
//-----------------------------------------------------------------------------
// [ 1] Int64
// [ 1] Uint64
// [ 2] size_type
//-----------------------------------------------------------------------------
// [ 3] 'snprintf' with const bsls::Types::Int64&
// [ 4] 'snprintf' with const bsls::Types::Uint64&
//-----------------------------------------------------------------------------
// [ 5] USAGE EXAMPLE
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

//=============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define INT64_FMT_STR  "0x%I64X"
#else
#define INT64_FMT_STR  "0x%llX"
#endif

char *hex64(char *buffer, bsls::Types::Uint64 value)
    // Convert the specified 64-bit 'value' to a hexadecimal string.
{
    sprintf(buffer, INT64_FMT_STR, value);
    return buffer;
}

#undef INT64_FMT_STR

#if defined(BSLS_PLATFORM_CPU_X86)                                            \
 && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))
// On Linux x86, no natural type is aligned on a 64-bit boundary, but we need
// such a type to implement low-level constructs (e.g 64-bit atomic types).

struct Test8BytesAlignedType
{
    long long d_dummy __attribute__((__aligned__(8)));
};
#endif

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating how one might use the types in
        //   'bsls::Types'.
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// The following illustrates how some of the types supplied by this component
// might be used.
//
// 'bsls::Types::Int64' and 'bsls::Types::Uint64' identify the preferred
// fundamental types denoting signed and unsigned 64-bit integers,
// respectively:
//..
    bsls::Types::Uint64 stimulus = 787000000000ULL;
//..
// Clients can use these types in the same way as an 'int'.  Clients can also
// mix usage with other fundamental integral types:
//..
    bsls::Types::Uint64 nationalDebt = 1000000000000ULL;
    nationalDebt += stimulus;

    unsigned int deficitReduction = 1000000000;
    nationalDebt -= deficitReduction;

    if (veryVerbose)
        std::cout << "National Debt Level: " << nationalDebt << std::endl;
//..
// 'bsls::Types::size_type' identifies the preferred integral type
// denoting the number of elements in a container, and the number of bytes in a
// single block of memory supplied by an allocator.  For example, a typical use
// is as a 'typedef' in an STL container:
//..
    class vector {

        // ...

      public:
        typedef bsls::Types::size_type size_type;

        // ...
    };
//..
// Since 'bsls::Types' are integers, the Standard Library facilities that work
// with numeric types can be used with 'bsls::Types' as well.  For example, the
// following code finds out some facts about 'bsls::Types::Int64' in a
// platform-independent way:
//..
    if (veryVerbose)
       std::cout << "Min Int64 value: "
                 << std::numeric_limits<bsls::Types::Int64>::min() << std::endl
                 << "Max Int64 value: "
                << std::numeric_limits<bsls::Types::Int64>::max() << std::endl;
//..

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT OPERATOR FOR UNSIGNED INT64 TYPE:
        //   The test is performed to insure that the 'typedef'd Uint64 behaves
        //   properly.
        //
        // Plan:
        //   For each of a small representative set of object values, ordered
        //   by increasing length, use 'snprintf' to write that object's value
        //   to two separate character buffers each with different initial
        //   values.  Compare the contents of these buffers with the literal
        //   expected output format and verify that the characters beyond the
        //   null characters are unaffected in both buffers.  On platforms that
        //   support 64-bit integer constants, test those constants directly.
        //   Additionally generate the same 64-bit constants by logically
        //   combining two 32-bit constants and test those as well.
        //
        // Testing:
        //   'snprintf' with const bsls::Types::Uint64&
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing 64-bit unsigned integer streaming support." << endl
            << "==================================================" << endl;

        typedef bsls::Types::Uint64 T;
        typedef bsls::Types::Int64  S;

        static const struct {
            int         d_lineNum;    // source line number
            T           d_spec;       // specification string
            const char *d_fmt_p;      // expected output format
        } DATA[] = {
            //line  spec                  output format
            //----  --------------------  --------------------------
            { L_,    0,                    "0"                    },
            { L_,    1,                    "1"                    },
            { L_,   0x7F,                  "127"                  },
            { L_,   0x80,                  "128"                  },
            { L_,   0xFF,                  "255"                  },
            { L_,   0x100,                 "256"                  },
            { L_,   0x7FFF,                "32767"                },
            { L_,   0x8000,                "32768"                },
            { L_,   0xFFFF,                "65535"                },
            { L_,   0x10000,               "65536"                },
            { L_,   0x7FFFFFFF,            "2147483647"           },
            { L_,   0x80000000,            "2147483648"           },
            { L_,   0xFFFFFFFF,            "4294967295"           },
#if !defined(BSLS_PLATFORM_NO_64_BIT_LITERALS)
            { L_,   0x100000000,           "4294967296"           },
            { L_,   0x7FFFFFFFFFFFFFFF,    "9223372036854775807"  },
            { L_,   0x8000000000000000,    "9223372036854775808"  },
            { L_,   0xFFFFFFFFFFFFFFFF,    "18446744073709551615" },// unsigned
#else
            { L_,   0x100000000uLL,        "4294967296"           },
            { L_,   0x7FFFFFFFFFFFFFFFuLL, "9223372036854775807"  },
            { L_,   0x8000000000000000uLL, "9223372036854775808"  },
            { L_,   0xFFFFFFFFFFFFFFFFuLL, "18446744073709551615" },// unsigned
#endif
            { L_,   (T) (S) -1,            "18446744073709551615" },// unsigned
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int SIZE = 1000;
        const char Z1  = (char) 0xFF;
        const char Z2  = 0x00;

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

                #if defined(BSLS_PLATFORM_CMP_MSVC)
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

                #if defined(BSLS_PLATFORM_CMP_MSVC)
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
                LOOP_ASSERT(LINE, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ,
                                              REST));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT OPERATOR FOR SIGNED INT64 TYPE:
        //   The test is performed to insure that the 'typedef'd Int64 behaves
        //   properly.
        //
        // Plan:
        //   For each of a small representative set of object values, ordered
        //   by increasing length, use 'snprintf' to write that object's value
        //   to two separate character buffers each with different initial
        //   values.  Compare the contents of these buffers with the literal
        //   expected output format and verify that the characters beyond the
        //   null characters are unaffected in both buffers.  On platforms that
        //   support 64-bit integer constants, test those constants directly.
        //   Additionally generate the same 64-bit constants by logically
        //   combining two 32-bit constants and test those as well.
        //
        // Testing:
        //   'snprintf' with const bsls::Types::Int64&
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing 64-bit signed integer streaming support." << endl
            << "================================================" << endl;

        typedef bsls::Types::Int64 T;

        static const struct {
            int         d_lineNum;    // source line number
            T           d_spec;       // specification string
            const char *d_fmt_p;      // expected output format
        } DATA[] = {
            //line  spec                  output format
            //----  --------------------  -------------------------
            { L_,    0,                   "0"                    },
            { L_,    1,                   "1"                    },
            { L_,   0x7F,                 "127"                  },
            { L_,   0x80,                 "128"                  },
            { L_,   0xFF,                 "255"                  },
            { L_,   0x100,                "256"                  },
            { L_,   0x7FFF,               "32767"                },
            { L_,   0x8000,               "32768"                },
            { L_,   0xFFFF,               "65535"                },
            { L_,   0x10000,              "65536"                },
            { L_,   0x7FFFFFFF,           "2147483647"           },
            { L_,   0x80000000,           "2147483648"           },
            { L_,   0xFFFFFFFF,           "4294967295"           },
#if !defined(BSLS_PLATFORM_NO_64_BIT_LITERALS)
            { L_,   0x100000000,          "4294967296"           },
            { L_,   0x7FFFFFFFFFFFFFFF,   "9223372036854775807"  },
            { L_,   T(0x8000000000000000), "-9223372036854775808" },
            { L_,   T(0xFFFFFFFFFFFFFFFF), "-1"                   },  // signed
#else
            { L_,   0x100000000LL,        "4294967296"           },
            { L_,   0x7FFFFFFFFFFFFFFFLL, "9223372036854775807"  },
            { L_,   0x8000000000000000LL, "-9223372036854775808" },
            { L_,   0xFFFFFFFFFFFFFFFFLL, "-1"                   },  // signed
#endif
            { L_,   -1,                   "-1"                   },  // signed
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

            #if defined(BSLS_PLATFORM_CMP_MSVC)
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

        if (verbose) cout << "\nTesting shifted values" << endl;

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

            #if defined(BSLS_PLATFORM_CMP_MSVC)
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
        // TESTING TYPEDEFS FOR SIZE TYPE:
        //   Concerns:
        //     Since the size_type is generated, it is possible that the
        //     typedef statements concerned for those types were wrong.  We
        //     must ensure that:
        //     - a size_type is at least as wide as an int.
        //     - 'size_type' is an unsigned integer type.
        //
        // Plan:
        //   First measure the size of the size type, ensuring that it is at
        //   least as wide as an int.  Then case a few values of different
        //   signs to it, and assert that the sign is correct.
        //
        // Testing:
        //   size_type
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing size_type" << endl
            << "=================" << endl;

        typedef bsls::Types Types;

        // Must be at least as wide as a 'long int' on 64 bit architectures,
        // as an 'int' otherwise.

#ifdef BSLS_PLATFORM_CPU_64_BIT
        ASSERT(sizeof(Types::size_type) >= sizeof(long int));
#else
        ASSERT(sizeof(Types::size_type) >= sizeof(int));
#endif

        // Must be unsigned
        Types::size_type zero(0);
        Types::size_type complementOfZero(~0);
        ASSERT(0 == zero);
        ASSERT(complementOfZero > 0);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING TYPEDEFS FOR 64-BIT FUNDAMENTAL DATA TYPES:
        //   Concerns:
        //     Since the Int64 and Uint64 types are generated, it is
        //     possible that the typedef statements concerned for those types
        //     were wrong.  We must ensure that:
        //     - both Int64 and Uint64 are both 64-bits wide.
        //     - the sign bit is interpreted properly for both Int64 and
        //       Uint64.
        //
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

        typedef bsls::Types Types;

        // Must be right size.
        ASSERT(8 == sizeof(Types::Int64));
        ASSERT(8 == sizeof(Types::Uint64));

        // Must be types.
        Types::Int64  int64  = -1;
        Types::Uint64 uint64 = (Types::Uint64) (Types::Int64) -1;

        // Must interpret sign bit correctly.
        ASSERT(int64 < (Types::Int64) 0); // cast solves dgintel compiler bug
        ASSERT(uint64 > 0);

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

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
