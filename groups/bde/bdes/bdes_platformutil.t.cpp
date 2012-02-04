// bdes_platformutil.t.cpp         -*-C++-*-

#include <bdes_platformutil.h>

#include <bsls_platform.h>     // for testing only

#include <bsl_iostream.h>

#include <bsl_cstddef.h>     // offsetof(), std::ptrdiff_t
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memset(), memcmp(), strlen()
#include <bsl_c_stdio.h>     // sprintf(), snprintf() [NOT <cstdio>, which does
                             // not include 'snprintf']

#if defined(BSLS_PLATFORM__CMP_MSVC)
#define snprintf _snprintf
#endif

#ifdef BSLS_PLATFORM__OS_UNIX
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif

using namespace BloombergLP;
using namespace bsl;

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
// [ 1] static int isBigEndian() const;
// [ 1] static int isLittleEndian() const;
// [ 3] static int roundUpToMaximalAlignment(int size);
// [ 1] BDES_PLATFORMUTIL__IS_LITTLE_ENDIAN
// [ 1] BDES_PLATFORMUTIL__IS_BIG_ENDIAN
// [ 5] BDES_PLATFORMUTIL__NO_64_BIT_CONSTANTS
// [ 8] BDES_PLATFORMUTIL__HTONS
// [ 8] BDES_PLATFORMUTIL__HTONL
// [ 8] BDES_PLATFORMUTIL__NTOHS
// [ 8] BDES_PLATFORMUTIL__NTOHL
//-----------------------------------------------------------------------------
// [ 7] operator<<(ostream&, const bdes_PlatformUtil::Uint64&);
// [ 6] operator<<(ostream&, const bdes_PlatformUtil::Int64&);
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

char *hex64(char *buffer, bdes_PlatformUtil::Uint64 value)
    // Convert the specified 64-bit 'value' to a hexadecimal string.
{
    sprintf(buffer, INT64_FMT_STR, value);
    return buffer;
}

void printBits(bdes_PlatformUtil::Uint64 value)
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

struct Test8BytesAlignedType
{
    long long d_dummy __attribute__((__aligned__(8)));
};
#endif

//--------------------------------------------------------------------------

//==========================================================================
//                   SUPPORTING FUNCTIONS USED FOR TESTING
//--------------------------------------------------------------------------

static int isBigEndian()
    // Return 1 if this machine is observed to be big endian, and 0 otherwise.
    // Internally, assert that this machine is observed to be either big
    // endian or little endian.
{
    union U {
        int d_int;
        char d_char[sizeof(int)];
    } u;

    u.d_int = 0x1;

    if (u.d_char[0] == 0x1) {
        return 0;
    }
    else if (u.d_char[sizeof(int) - 1] == 0x1) {
        return 1;
    }
    else {
        ASSERT(0);      // Neither big endian nor little endian!
    }
    return 0;
}

//--------------------------------------------------------------------------

static int isLittleEndian()
    // Return 1 if this machine is observed to be little endian, and 0
    // otherwise.  Internally, assert that this machine is observed to be
    // either big endian or little endian.
{
    union U {
        int d_int;
        char d_char[sizeof(int)];
    } u;

    u.d_int = 0x1;

    if (u.d_char[0] == 0x1) {
        return 1;
    }
    else if (u.d_char[sizeof(int) - 1] == 0x1) {
        return 0;
    }
    else {
        ASSERT(0);      // Neither big endian nor little endian!
    }
    return 0;
}

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
      case 8: {
        // --------------------------------------------------------------------
        // TESTING BDES_PLATFORMUTIL_{H,S}TOH{L,S}
        //
        // Concerns:
        //  Each byte in a small (2 byte) and a long (4 bytes) integer are
        //  properly shifted if necessary.
        //
        // Plan:
        //  By using values with bits set on each byte, we will verify
        //  that the output of the macros are the same as the platform
        //  provided macros.
        //
        // Testing:
        //   BDES_PLATFORMUTIL__HTONS
        //   BDES_PLATFORMUTIL__NTOHS
        //   BDES_PLATFORMUTIL__HTONS
        //   BDES_PLATFORMUTIL__NTOHL
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing BDES_PLATFORMUTIL__HTONS and BDES_PLATFORMUTIL__NTOHS"
            << endl
            << "============================================================="
            << endl;
        {
            static const struct {
                int             d_lineNum;    // source line number
                unsigned short  d_value;
            } DATA[] = {
                //line  value
                //----  --------
                { L_,        0, },
                { L_,   0xff00, },
                { L_,   0x00ff, },
                { L_,   0xffff, },
                { L_,   0x8000, },
                { L_,   0x0100, },
                { L_,   0x0080, },
                { L_,   0x0001, },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE    = DATA[ti].d_lineNum;
                const unsigned short VALUE = DATA[ti].d_value;

                {
                    const unsigned short oracle = htons(VALUE);
                    const unsigned short newValue
                                             = BDES_PLATFORMUTIL__HTONS(VALUE);

                    if (veryVerbose) {
                        TAB P_(ti); P_(VALUE); P_(newValue); P(oracle)
                    }

                    LOOP_ASSERT(LINE, newValue == oracle);
                }
                {
                    const unsigned short oracle = ntohs(VALUE);
                    const unsigned short newValue
                                             = BDES_PLATFORMUTIL__NTOHS(VALUE);

                    if (veryVerbose) {
                        TAB P_(ti); P_(VALUE); P_(newValue); P(oracle)
                    }

                    LOOP_ASSERT(LINE, newValue == oracle);
                }
            }
        }

        if (verbose) cout << endl
            << "Testing BDES_PLATFORMUTIL__HTONL and BDES_PLATFORMUTIL__NTOHL"
            << endl
            << "============================================================="
            << endl;
        {
            static const struct {
                int   d_lineNum;    // source line number
                int   d_value;
            } DATA[] = {
                //line  value
                //----  ------------
                { L_,            0, },
                { L_,   0xff000000, },
                { L_,   0x00ff0000, },
                { L_,   0x0000ff00, },
                { L_,   0x000000ff, },
                { L_,   0xffffffff, },
                { L_,   0x80000000, },
                { L_,   0x01000000, },
                { L_,   0x00800000, },
                { L_,   0x00010000, },
                { L_,   0x00008000, },
                { L_,   0x00000100, },
                { L_,   0x00000080, },
                { L_,   0x00000001, },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE    = DATA[ti].d_lineNum;
                const int VALUE = DATA[ti].d_value;

                {
                    const int oracle = htonl(VALUE);
                    const int newValue = BDES_PLATFORMUTIL__HTONL(VALUE);

                    if (veryVerbose) {
                        TAB P_(ti); P_(VALUE); P_(newValue); P(oracle)
                    }

                    LOOP_ASSERT(LINE, newValue == oracle);
                }
                {
                    const int oracle = ntohl(VALUE);
                    const int newValue = BDES_PLATFORMUTIL__NTOHL(VALUE);

                    if (veryVerbose) {
                        TAB P_(ti); P_(VALUE); P_(newValue); P(oracle)
                    }

                    LOOP_ASSERT(LINE, newValue == oracle);
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR FOR UNSIGNED INT64 TYPE:
        //   The output operator is fully tested in 'bdes_int64'.  The test is
        //   repeated here explicitly using 'bdes_PlatformUtil::Uint64' to
        //   insure that the 'typedef'ed Uint64 behaves properly.
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
        //   'operator<<' with const bdes_PlatformUtil::Uint64&
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing 64-bit unsigned integer streaming support." << endl
            << "==================================================" << endl;

        typedef bdes_PlatformUtil::Uint64 T;
        typedef bdes_PlatformUtil::Int64  S;

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
#if !defined(BDES_PLATFORMUTIL__NO_64_BIT_CONSTANTS)
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
      case 6: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR FOR SIGNED INT64 TYPE:
        //   The output operator is fully tested in 'bdes_int64'.  The test is
        //   repeated here explicitly using 'bdes_PlatformUtil::Int64' to
        //   insure that the 'typedef'ed Int64 behaves properly.
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
        //   'operator<<' with const bdes_PlatformUtil::Int64&
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing 64-bit signed integer streaming support." << endl
            << "================================================" << endl;

        typedef bdes_PlatformUtil::Int64 T;

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
#if !defined(BDES_PLATFORMUTIL__NO_64_BIT_CONSTANTS)
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
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 64-BIT CONSTANTS:
        //   Concerns:
        //     Since the actual flag indicates the lack of support for
        //     64-bit integer constants, the only way to test the flag is
        //     for the compiler to fail.  Therefore the test will check for
        //     the absence of the flag and attempt to assign 64-bit constants
        //     to a variable, ensuring the compile-time macro for support of
        //     64-bit integer constants agrees with the capability of the
        //     compiler.
        // Plan:
        //   - Assign both signed and unsigned 64-bit integer constants
        //     to variables of each type.
        //   - Verify that the compiler does not truncate the assignment or
        //     the constant by splitting the constant into 2 32-bit constants
        //     and combining them using logical operations into another
        //     64-bit value.
        //   - Verify the constructed value is equal to the 64-bit value
        //     directly assigned.
        //   - Verify no truncation is occurring by logically masking
        //     and shifting the 64-bit value with the 32-bit lo and hi words.
        // Testing:
        //   BDES_PLATFORMUTIL__NO_64_BIT_CONSTANT
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing 64-bit integer constant support." << endl
            << "========================================" << endl;

#if defined(BDES_PLATFORMUTIL__NO_64_BIT_CONSTANTS)
        if (veryVerbose) cout << "No 64-bit integer constants." << endl;
#else
        if (veryVerbose) cout << "64-bit integer constants supported." << endl;
        typedef bdes_PlatformUtil::Int64 T;
        typedef bdes_PlatformUtil::Uint64 U;

        T  i, iHi, iLo, iTest;
        U u, uHi, uLo, uTest;

        // the following lines should compile
        i = 9223372036854775807;        // 0x7FFFFFFFFFFFFFFF
        u = 9223372036854775809;        // 0x8000000000000001

        ASSERT(i == 0x7FFFFFFFFFFFFFFF);
        ASSERT(u == 0x8000000000000001);

        // generate test values in 32-bit parts
        iHi = 0x7FFFFFFF; iLo = 0xFFFFFFFF;
        iTest = iHi << 32 | iLo;
        ASSERT(i == iTest);
        ASSERT((i & 0xFFFFFFFF) == iLo);
        ASSERT(i >> 32 == iHi);

        uHi = 0x80000000; uLo = 0x00000001;
        uTest = uHi << 32 | uLo;
        ASSERT(u == uTest);
        ASSERT((u & 0x0FFFFFFFF) == uLo);
        ASSERT(u >> 32 == uHi);
#endif
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
        //   MaxAlign
        //   static int roundUpToMaximalAlignment(int size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "MAXIMUM ALIGNMENT TEST" << endl
                                  << "======================" << endl;

        typedef bdes_PlatformUtil U;

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

#if BSL_LEGACY == 0
    typedef bsls_AlignmentUtil::MaxAlignedType LegacyMaxAlign;
#else
    typedef U::MaxAlign                        LegacyMaxAlign;
#endif

        struct MaxAlignAlign    { char c; LegacyMaxAlign d_maxAlign;};

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
                                d_8BytesAlignedType)       - 1)
#endif
                );

        const int maxAlignment = offsetof(MaxAlignAlign, d_maxAlign);

        if (veryVerbose) {
            TAB; P_(maxAlignment); P_(sizeof(LegacyMaxAlign)); P(EXP);
        }

        ASSERT(EXP == maxAlignment);
        ASSERT(EXP == sizeof(LegacyMaxAlign));

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

        typedef bdes_PlatformUtil Util;

        // Must be at least as wide as an int.
        ASSERT(sizeof(Util::size_type) >= sizeof(int));

        // Must be convertible (without error or warning) from a difference of
        // two pointers.  Use some arbitrary values for this.
        Util::size_type zero     = bsl::ptrdiff_t(0);
        Util::size_type minusOne = bsl::ptrdiff_t(-1);
        Util::size_type posValue = bsl::ptrdiff_t(1048576);

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

        typedef bdes_PlatformUtil Util;

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
        //     1. The macros BDES_PLATFORMUTIL__IS_BIG_ENDIAN and
        //        BSLS_PLATFORM__IS_LITTLE_ENDIAN must have boolean values.
        //     2. The macros BDES_PLATFORMUTIL__IS_BIG_ENDIAN and
        //        BSLS_PLATFORM__IS_LITTLE_ENDIAN are assigned at compile
        //        time based on the platform (see overview above).  If any
        //        one of the flags or inferences is wrong, the "endian-ness"
        //        of a given platform could be wrong.  Similarly, the
        //        'isBigEndian' and 'isLittleEndian' member functions could
        //        also be wrong since those functions do nothing more than
        //        return the//        value of the macros.  Fortunately it
        //        is possible to make run-time determination of a platform's
        //        "endian-ness" by using a union.  Unfortunately such a test
        //        is more expensive than checking a flag.  However, such a
        //        function is perfect for a test driver.
        //   Plan:
        //     First ensure the values for the endian macros and their
        //     corresponding function return boolean values.
        //
        //     Next, ensure the compile-time macros and inline functions agree
        //     with the values calculated at runtime.
        // Testing:
        //   static int isBigEndian();
        //   static int isLittleEndian();
        //   BDES_PLATFORMUTIL__IS_LITTLE_ENDIAN
        //   BDES_PLATFORMUTIL__IS_BIG_ENDIAN
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing little-endian & big-endian functions/macros." << endl
            << "====================================================" << endl;

        typedef bdes_PlatformUtil Util;

        if (verbose) cout << "This platform is " <<
            (bdes_PlatformUtil::isBigEndian() ? "BIG ENDIAN" : "LITTLE ENDIAN")
            << '.' << endl;

        // Must be boolean values,
        ASSERT(Util::isBigEndian() == !!Util::isBigEndian());
        ASSERT(Util::isLittleEndian() == !!Util::isLittleEndian());

        // Must be same as control functions.
        ASSERT(::isBigEndian() == Util::isBigEndian());
        ASSERT(::isBigEndian() != Util::isLittleEndian());
        ASSERT(::isLittleEndian() == Util::isLittleEndian());
        ASSERT(::isLittleEndian() != Util::isBigEndian());

        // Must be same as preprocessor MACROS.
#if defined(BDES_PLATFORMUTIL__IS_BIG_ENDIAN)
        ASSERT(BDES_PLATFORMUTIL__IS_BIG_ENDIAN == Util::isBigEndian());
#else
        ASSERT(BDES_PLATFORMUTIL__IS_LITTLE_ENDIAN == Util::isLittleEndian());
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
