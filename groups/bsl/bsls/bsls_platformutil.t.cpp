// bsls_platformutil.t.cpp                                            -*-C++-*-

#include <bsls_platformutil.h>

#include <bsls_alignmentutil.h> // for testing only
#include <bsls_platform.h>      // for testing only

#include <cstddef>     // offsetof(), std::ptrdiff_t
#include <cstdlib>     // atoi()
#include <cstring>     // memset(), memcmp(), strlen()
#include <iostream>
#include <stdio.h>     // sprintf(), snprintf() [NOT <cstdio>, which does not
                       // include 'snprintf']

#if defined(BSLS_PLATFORM__CMP_MSVC)
#define snprintf _snprintf
#endif

#ifdef BSLS_PLATFORM__OS_UNIX
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
// This component implements several utilities for a platform.  A platform is
// defined as the unique combination of compiler, operating system, and
// processor (CPU).  The utilities defined are: types, macros, and functions.
// We will test each of the utilities, verifying that they have been properly
// implemented for a given platform.
//-----------------------------------------------------------------------------
// [ 2] Int64
// [ 2] Uint64
// [ 3] size_type
// [ 4] MaxAlign
// [ 1] static bool isBigEndian() const;
// [ 1] static bool isLittleEndian() const;
// [ 4] static int roundUpToMaximalAlignment(int size);
// [ 1] BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
// [ 1] BSLS_PLATFORMUTIL__IS_BIG_ENDIAN
//-----------------------------------------------------------------------------
// [ 5] operator<<(ostream&, const bsls::PlatformUtil::Int64&);
// [ 6] operator<<(ostream&, const bsls::PlatformUtil::Uint64&);
// [ 7] USAGE EXAMPLE
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

#if defined(BSLS_PLATFORM__CMP_MSVC)
#define INT64_FMT_STR  "0x%I64X"
#else
#define INT64_FMT_STR  "0x%llX"
#endif

char *hex64(char *buffer, bsls::PlatformUtil::Uint64 value)
    // Convert the specified 64-bit 'value' to a hexadecimal string.
{
    sprintf(buffer, INT64_FMT_STR, value);
    return buffer;
}

void printBits(bsls::PlatformUtil::Uint64 value)
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

#if defined(BSLS_PLATFORM__CPU_X86) && defined(BSLS_PLATFORM__CMP_GNU)
// On Linux x86, no natural type is aligned on a 64-bit boundary, but we need
// such a type to implement low-level constructs (e.g 64-bit atomic types).

struct Test8BytesAlignedType {
    long long d_dummy __attribute__((__aligned__(8)));
};
#endif

//==========================================================================
//                   SUPPORTING FUNCTIONS USED FOR TESTING
//--------------------------------------------------------------------------

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

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

// 'bsls::PlatformUtil::size_type' identifies the preferred integral type
// denoting the number of elements in a container, and the number of bytes in
// a single block of memory supplied by an allocator.  For example, a typical
// use is as a 'typedef' in an STL container:
//..
    class vector {

        // ...

      public:
        typedef bsls::PlatformUtil::size_type size_type;

        // ...
    };
//..
//
///Functions and Macros
/// - - - - - - - - - -
// The functions:
//..
//  bool bsls::PlatformUtil::isLittleEndian();
//  bool bsls::PlatformUtil::isBigEndian();
//..
// encapsulate the capability of determining whether a machine is big- or
// little-endian across all supported platforms.  In addition, certain
// compile-time constants are also provided as preprocessor macros to
// facilitate conditional compilation:
//..
//  BSLS_PLATFORMUTIL__IS_BIG_ENDIAN
//  BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
//..
// These functions and macros are useful for writing platform-independent code,
// such as a function that converts the bytes in a 'short' to network byte
// order (which is in big-endian):
//..
    short convertToNetworkByteOrder(short input)
        // Return the specified 'input' in network byte order.
    {
    #ifdef BSLS_PLATFORMUTIL__IS_BIG_ENDIAN
        return input;
    #else
        return ((input >> 8) & 0xFF) | ((input & 0xFF) << 8);
    #endif
    }
//..
// Note that in the above usage example, either the macros or the functions
// can be used to test whether a platform is big- or little-endian.

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
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE TEST" << endl
                                  << "==================" << endl;

///Usage
///-----
// The following illustrates how some of the types and functions supplied
// by this component might be used.
//
///Types
///- - -
// 'bsls::PlatformUtil::Int64' and 'bsls::PlatformUtil::Uint64' identify the
// preferred fundamental types denoting signed and unsigned 64-bit integers,
// respectively:
//..
    bsls::PlatformUtil::Uint64 stimulus = 787000000000ULL;
//..
// Clients can use these types in the same way as an 'int'.  Clients can also
// mix usage with other fundamental integral types:
//..
    bsls::PlatformUtil::Uint64 nationalDebt = 1000000000000ULL;
    nationalDebt += stimulus;

    unsigned int deficitReduction = 1000000000;
    nationalDebt -= deficitReduction;

if (verbose)
    std::cout << "National Debt Level: " << nationalDebt << std::endl;
//..

        // use 'convertToNetworkByteOrder' from Usage example

        const short x      = 0x1001;
        const short xPrime = 0x0110;

        if (bsls::PlatformUtil::isBigEndian()) {
            ASSERT(x      == convertToNetworkByteOrder(x));
        }
        else {
            ASSERT(xPrime == convertToNetworkByteOrder(x));
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR FOR UNSIGNED INT64 TYPE:
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
        //   'operator<<' with const bsls::PlatformUtil::Uint64&
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing 64-bit unsigned integer streaming support." << endl
            << "==================================================" << endl;

        typedef bsls::PlatformUtil::Uint64 T;
        typedef bsls::PlatformUtil::Int64  S;

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
#if !defined(BSLS_PLATFORM__NO_64_BIT_LITERALS)
            { L_,   0x100000000,        "4294967296"           },
            { L_,   0x7FFFFFFFFFFFFFFF, "9223372036854775807"  },
            { L_,   0x8000000000000000, "9223372036854775808"  },
            { L_,   0xFFFFFFFFFFFFFFFF, "18446744073709551615" },  // unsigned
#endif
            { L_,   (T) (S) -1,         "18446744073709551615" },  // unsigned
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
                LOOP_ASSERT(LINE, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ,
                                              REST));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ,
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
                LOOP_ASSERT(LINE, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ,
                                              REST));
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR FOR SIGNED INT64 TYPE:
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
        //   'operator<<' with const bsls::PlatformUtil::Int64&
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing 64-bit signed integer streaming support." << endl
            << "================================================" << endl;

        typedef bsls::PlatformUtil::Int64 T;

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
#if !defined(BSLS_PLATFORM__NO_64_BIT_LITERALS)
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
      case 4: {
        // --------------------------------------------------------------------
        // TESTING MAXIMUM ALIGNMENT:
        //   Memory management and allocation functions must be able to return
        //   the address of a block of memory into which any type of data can
        //   be stored.  Such a block of memory is called maximally aligned.
        //
        // Concerns:
        //   At compile time, a series of conditional compilation
        //   statements determine what the maximally-aligned type is
        //   based on macros set for the platform.  It is possible that
        //   the logic could be flawed or a new platform (combination
        //   of compiler, OS, and CPU) introduced.  While it is possible
        //   programmatically to determine the maximally-aligned type, the
        //   calculation is not efficient.  However, such functionality is
        //   well suited for a test driver.
        //
        // Plan:
        //   First, ensure that 'MaxAlign' is the maximally aligned type.
        //
        //   Next, ensure that the size of the maximally aligned type is
        //   equal to its alignment offset.
        //
        //   Finally, ensure that 'roundUpToMaximalAlignment' rounds its
        //   argument up to the nearest multiple of 'sizeof(MaxAlign)'.
        //
        // Testing:
        //   static int roundUpToMaximalAlignment(int size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "MAXIMUM ALIGNMENT TEST" << endl
                                  << "======================" << endl;

        typedef bsls::PlatformUtil U;

        struct ShortAlign       { char c; short  d_short;           };
        struct IntAlign         { char c; int    d_int;             };
        struct LongAlign        { char c; long   d_long;            };
        struct Int64Align       { char c; U::Int64 d_int64;         };
        struct FloatAlign       { char c; float  d_float;           };
        struct DoubleAlign      { char c; double d_double;          };
        struct LongDoubleAlign  { char c; long double d_longDouble; };
        struct VoidPtrAlign     { char c; void  *d_voidPtr;         };
#if defined(BSLS_PLATFORM__CPU_X86) && defined(BSLS_PLATFORM__CMP_GNU)
        struct Test8bytesAlign  { char c; Test8BytesAlignedType
                                               d_8BytesAlignedType; };
#endif
        struct MaxAlignAlign    { char c;
                       bsls::AlignmentUtil::MaxAlignedType d_maxAlign; };

        const int EXP =
            1 + ( (1                                       - 1) // char
                | (offsetof(ShortAlign, d_short)           - 1)
                | (offsetof(IntAlign, d_int)               - 1)
                | (offsetof(LongAlign, d_long)             - 1)
                | (offsetof(Int64Align, d_int64)           - 1)
                | (offsetof(FloatAlign, d_float)           - 1)
                | (offsetof(DoubleAlign, d_double)         - 1)
                | (offsetof(LongDoubleAlign, d_longDouble) - 1)
                | (offsetof(VoidPtrAlign, d_voidPtr)       - 1)
#if defined(BSLS_PLATFORM__CPU_X86) && defined(BSLS_PLATFORM__CMP_GNU)
                | (offsetof(Test8bytesAlign,
                            d_8BytesAlignedType)           - 1)
#endif
                );

        const int maxAlignment = offsetof(MaxAlignAlign, d_maxAlign);
        if (veryVerbose) {
            TAB; P_(maxAlignment);
            P_(sizeof(bsls::AlignmentUtil::MaxAlignedType)); P(EXP);
        }

        ASSERT(EXP == maxAlignment);
        ASSERT(EXP == sizeof(bsls::AlignmentUtil::MaxAlignedType));

        ASSERT(0 == U::roundUpToMaximalAlignment(0));

        int i;
        for (i = 1; i <= maxAlignment; ++i) {
            ASSERT(    maxAlignment == U::roundUpToMaximalAlignment(i));
        }
        for (i = maxAlignment + 1; i <= 2 * maxAlignment; ++i) {
            ASSERT(2 * maxAlignment == U::roundUpToMaximalAlignment(i));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING TYPEDEFS FOR SIZE TYPE:
        //   Concerns:
        //     Since the size_type is generated, it is possible that the
        //     typedef statements concerned for those types were wrong.  We
        //     must ensure that:
        //     - 'size_type' is a signed integer type
        //     - a size_type is at least as wide as an int.
        //     - a size_type can contain the difference between two pointers.
        //
        // Plan:
        //   First measure the size of the size type, ensuring that it is at
        //   least as wide as an int.  Then cast a few values of different
        //   signs to it, and assert that the sign is correct.
        //
        // Testing:
        //   size_type
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing size_type" << endl
            << "=================" << endl;

        typedef bsls::PlatformUtil Util;

        // Must be at least as wide as an int.
#ifdef BSLS_PLATFORM__CPU_64_BIT
        ASSERT(sizeof(Util::size_type) >= sizeof(long int));
#else
        ASSERT(sizeof(Util::size_type) >= sizeof(int));
#endif

        // Must be convertible (without error or warning) from a difference of
        // two pointers.  Use some arbitrary values for this.
        Util::size_type zero     = std::ptrdiff_t(0);
        Util::size_type minusOne = std::ptrdiff_t(-1);
        Util::size_type posValue = std::ptrdiff_t(1048576);

        // Must be signed.
        ASSERT(0 == zero);
        ASSERT(minusOne < 0);
        ASSERT(0 <  posValue);

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

        typedef bsls::PlatformUtil Util;

        // Must be right size.
        ASSERT(8 == sizeof(Util::Int64));
        ASSERT(8 == sizeof(Util::Uint64));

        // Must be types.
        Util::Int64  int64  = -1;
        Util::Uint64 uint64 = (Util::Uint64) (Util::Int64) -1;

        // Must interpret sign bit correctly.
        ASSERT(int64 < (Util::Int64) 0); // cast addresses dgintel compiler bug
        ASSERT(uint64 > 0);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING BIG ENDIAN and LITTLE ENDIAN:
        //   Concerns:
        //     1. The macros BSLS_PLATFORMUTIL__IS_BIG_ENDIAN and
        //        BSLS_PLATFORM__IS_LITTLE_ENDIAN must have boolean values.
        //     2. The macros BSLS_PLATFORMUTIL__IS_BIG_ENDIAN and
        //        BSLS_PLATFORM__IS_LITTLE_ENDIAN are assigned at compile
        //        time based on the platform (see overview above).  If any
        //        one of the flags or inferences is wrong, the "endian-ness"
        //        of a given platform could be wrong.  Similarly, the
        //        'isBigEndian' and 'isLittleEndian' member functions could
        //        also be wrong since those functions do nothing more than
        //        return the value of the macros.  Fortunately it is possible
        //        to make run-time determination of a platform's "endian-ness"
        //        by using a union.  Unfortunately such a test is more
        //        expensive than checking a flag.  However, such a function is
        //        perfect for a test driver.
        //   Plan:
        //     First ensure the values for the endian macros and their
        //     corresponding function return boolean values.
        //
        //     Next, ensure the compile-time macros and inline functions agree
        //     with the values calculated at runtime.
        // Testing:
        //   static bool isBigEndian();
        //   static bool isLittleEndian();
        //   BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
        //   BSLS_PLATFORMUTIL__IS_BIG_ENDIAN
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing little-endian & big-endian functions/macros." << endl
            << "====================================================" << endl;

        typedef bsls::PlatformUtil Util;

        if (verbose) cout << "This platform is " <<
           (bsls::PlatformUtil::isBigEndian() ? "BIG ENDIAN" : "LITTLE ENDIAN")
            << '.' << endl;

        // Must be boolean values
        ASSERT(Util::isBigEndian() == !!Util::isBigEndian());
        ASSERT(Util::isLittleEndian() == !!Util::isLittleEndian());

        // Must be same as control functions.
        ASSERT(::isBigEndian() == Util::isBigEndian());
        ASSERT(::isBigEndian() != Util::isLittleEndian());
        ASSERT(::isLittleEndian() == Util::isLittleEndian());
        ASSERT(::isLittleEndian() != Util::isBigEndian());

        // Must be same as preprocessor MACROS.
#if defined(BSLS_PLATFORMUTIL__IS_BIG_ENDIAN)
        ASSERT(BSLS_PLATFORMUTIL__IS_BIG_ENDIAN == Util::isBigEndian());
#else
        ASSERT(BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN == Util::isLittleEndian());
#endif
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
