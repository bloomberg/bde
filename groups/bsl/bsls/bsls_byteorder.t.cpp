// bsls_byteorder.t.cpp                                               -*-C++-*-

#include <bsls_byteorder.h>

#include <bsls_platform.h>

#include <bsls_types.h>

#include <stdlib.h>     // atoi()
#include <stdio.h>      // printf()
#include <iostream>

#ifdef BSLS_PLATFORM_OS_UNIX   // htons(), htonl(), ntohs(), ntohl()
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif

// Warning: the following 'using' declarations interfere with the testing of
// the macros defined in this component.  Please do not uncomment them.
// using namespace BloombergLP;
// using namespace std;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//-----------------------------------------------------------------------------
// [ 1] BSLS_BYTEORDER_HTONS
// [ 1] BSLS_BYTEORDER_HTONL
// [ 1] BSLS_BYTEORDER_HTONLL
// [ 1] BSLS_BYTEORDER_NTOHS
// [ 1] BSLS_BYTEORDER_NTOHL
// [ 1] BSLS_BYTEORDER_NTOHLL
// [ 1] BSLS_BYTEORDER_HTONS_CONSTANT
// [ 1] BSLS_BYTEORDER_HTONL_CONSTANT
// [ 1] BSLS_BYTEORDER_HTONLL_CONSTANT
// [ 1] BSLS_BYTEORDER_NTOHS_CONSTANT
// [ 1] BSLS_BYTEORDER_NTOHL_CONSTANT
// [ 1] BSLS_BYTEORDER_NTOHLL_CONSTANT
// [ 2] BSLS_BYTEORDER_LE_U16_TO_HOST
// [ 2] BSLS_BYTEORDER_LE_U32_TO_HOST
// [ 2] BSLS_BYTEORDER_LE_U64_TO_HOST
// [ 2] BSLS_BYTEORDER_BE_U16_TO_HOST
// [ 2] BSLS_BYTEORDER_BE_U32_TO_HOST
// [ 2] BSLS_BYTEORDER_BE_U64_TO_HOST
// [ 2] BSLS_BYTEORDER_HOST_U16_TO_LE
// [ 2] BSLS_BYTEORDER_HOST_U32_TO_LE
// [ 2] BSLS_BYTEORDER_HOST_U64_TO_LE
// [ 2] BSLS_BYTEORDER_HOST_U16_TO_BE
// [ 2] BSLS_BYTEORDER_HOST_U32_TO_BE
// [ 2] BSLS_BYTEORDER_HOST_U64_TO_BE
//-----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
// [ 3] MACRO SAFETY
//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        std::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << std::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
  if (!(X)) { std::cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) std::cout << #X " = " << (X) << std::endl; // Print identifier
                                                        // and value.

#define Q(X) std::cout << "<| " #X " |>" << std::endl;  // Quote identifier
                                                        // literally.

#define P_(X) std::cout << #X " = " << (X) << ", "<<std::flush; // P(X) without
                                                                // '\n'

#define T_ std::cout << "\t" << std::flush;             // Print tab w/o
                                                        // newline

#define L_ __LINE__                                     // current Line number

//=============================================================================
//                         GLOBAL TYPEDEF FOR TESTING
//-----------------------------------------------------------------------------

typedef BloombergLP::bsls::Types::Int64  Int64;
typedef BloombergLP::bsls::Types::Uint64 Uint64;

//=============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void printHex(const char * c, int size)
    // Print the specified character array 'c' with the specified 'size' in
    // hex.
{
    const char *hex = "0123456789abcdef";
    for (int i = 0; i < size; ++i) {

        std::cout << hex[(c[i] >> 4) & 0xf]
                  << hex[ c[i]       & 0xf];
    }
}

template <class T>
void printHex(T x)
    // Print the specified object 'x' of parameterized type 'T' in hex.
{
    printHex((const char*)&x, sizeof x);
}

#define PH(X) std::cout << #X " = "; printHex(X); std::cout << std::endl;
#define PH_(X) std::cout << #X " = "; printHex(X);                            \
               std::cout << ", " << std::flush;

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    std::cout << "TEST " << __FILE__ << " CASE " << test << std::endl;

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
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

        if (verbose) std::cout << "\nTesting Usage Example"
                               << "\n=====================" << std::endl;

// For example, to use the little-endian / big-endian to host-endian macros:
//..
    short x = static_cast<short>(0xabcd);
    int   y = 0xabcdef12;
    Int64 z = 0xabcdef1234567890LL;

    if (veryVerbose) {  // added to keep output of test driver clean

    // Note the use of macros within the calls to 'printHex'.

    printf("\nLE to Host(x): ");
    printHex(BSLS_BYTEORDER_LE_U16_TO_HOST(x));

    printf("\nLE to Host(y): ");
    printHex(BSLS_BYTEORDER_LE_U32_TO_HOST(y));

    printf("\nLE to Host(z): ");
    printHex(BSLS_BYTEORDER_LE_U64_TO_HOST(z));

    printf("\nBE to Host(x): ");
    printHex(BSLS_BYTEORDER_BE_U16_TO_HOST(x));

    printf("\nBE to Host(y): ");
    printHex(BSLS_BYTEORDER_BE_U32_TO_HOST(y));

    printf("\nBE to Host(z): ");
    printHex(BSLS_BYTEORDER_BE_U64_TO_HOST(z));

    }  // added to keep output of test driver clean
//..
// On little-endian machines (e.g., x86, IA64), this will print the following
// to 'stdout':
//..
//  LE to Host(x): abcd
//  LE to Host(y): abcdef12
//  LE to Host(z): abcdef1234567890
//  BE to Host(x): cdab
//  BE to Host(y): 12efcdab
//  BE to Host(z): 9078563412efcdab
//..
// On big-endian machines (e.g., sparc, powerpc), the following will be printed
// instead:
//..
//  LE to Host(x): cdab
//  LE to Host(y): 12efcdab
//  LE to Host(z): 9078563412efcdab
//  BE to Host(x): abcd
//  BE to Host(y): abcdef12
//  BE to Host(z): abcdef1234567890
//..
// The other macros can be used in a similar manner.

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING MACRO SAFETY
        //
        // Concerns:
        //   1. The macros should be usable outside 'namespace' 'BloombergLP'
        //      and 'namespace' 'std'.
        //
        //   2. The macros should be usable within conditional statements
        //      that are not fully bracketed ('{}'-enclosed).
        //
        // Plan:
        //   The 'using' declarations at the top of the file are specifically
        //   commented out to test concern 1.  For concern 2, the macros are
        //   used within an 'if' statement that is not fully bracketed,
        //   followed by an 'else' statement.  If either concerns are violated,
        //   the test driver should fail to compile.
        //
        // Testing:
        //   MACRO SAFETY
        // --------------------------------------------------------------------

        if (verbose) std::cout << std::endl
                               << "TESTING MACRO SAFETY" << std::endl
                               << "====================" << std::endl;

        if (verbose) std::cout <<
            "\nTesting macro in unbracketed context and absence of using"
            "namespace BloombergLP" << std::endl;

        // Input values
        const short INPUT16 = static_cast<short>(0xabcd);
        const int   INPUT32 = 0xabcdef12;
        const Int64 INPUT64 = 0xabcdef1234567890LL;

#if BSLS_PLATFORM_IS_BIG_ENDIAN
        // Big Endian to host expected results
        const short BEEXP16 = static_cast<short>(0xabcd);
        const int   BEEXP32 = 0xabcdef12;
        const Int64 BEEXP64 = 0xabcdef1234567890LL;

        // Little Endian to host expected results
        const short LEEXP16 = static_cast<short>(0xcdab);
        const int   LEEXP32 = 0x12efcdab;
        const Int64 LEEXP64 = 0x9078563412efcdabLL;
#else
        // Big Endian to host expected results
        const short BEEXP16 = static_cast<short>(0xcdab);
        const int   BEEXP32 = 0x12efcdab;
        const Int64 BEEXP64 = 0x9078563412efcdabLL;

        // Little Endian to host expected results
        const short LEEXP16 = static_cast<short>(0xabcd);
        const int   LEEXP32 = 0xabcdef12;
        const Int64 LEEXP64 = 0xabcdef1234567890LL;
#endif

        int unbracketedLoggingFlag = 1;

        {
            if (veryVerbose) std::cout << "BSLS_BYTEORDER_LE_U16_TO_HOST"
                                       << std::endl;

            short result = 0;
            if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                result = BSLS_BYTEORDER_LE_U16_TO_HOST(INPUT16);
            else
                ++unbracketedLoggingFlag;
            ASSERT(LEEXP16 == result);
        }

        {
            if (veryVerbose) std::cout << "BSLS_BYTEORDER_LE_U32_TO_HOST"
                                       << std::endl;

            int result = 0;
            if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                result = BSLS_BYTEORDER_LE_U32_TO_HOST(INPUT32);
            else
                ++unbracketedLoggingFlag;
            ASSERT(LEEXP32 == result);
        }

        {
            if (veryVerbose) std::cout << "BSLS_BYTEORDER_LE_U64_TO_HOST"
                                       << std::endl;

            Int64 result = 0;
            if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                result = BSLS_BYTEORDER_LE_U64_TO_HOST(INPUT64);
            else
                ++unbracketedLoggingFlag;
            ASSERT(LEEXP64 == result);
        }

        {
            if (veryVerbose) std::cout << "BSLS_BYTEORDER_BE_U16_TO_HOST"
                                       << std::endl;

            short result = 0;
            if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                result = BSLS_BYTEORDER_BE_U16_TO_HOST(INPUT16);
            else
                ++unbracketedLoggingFlag;
            ASSERT(BEEXP16 == result);
        }

        {
            if (veryVerbose) std::cout << "BSLS_BYTEORDER_BE_U32_TO_HOST"
                                       << std::endl;

            int result = 0;
            if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                result = BSLS_BYTEORDER_BE_U32_TO_HOST(INPUT32);
            else
                ++unbracketedLoggingFlag;
            ASSERT(BEEXP32 == result);
        }

        {
            if (veryVerbose) std::cout << "BSLS_BYTEORDER_BE_U64_TO_HOST"
                                       << std::endl;

            Int64 result = 0;
            if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                result = BSLS_BYTEORDER_BE_U64_TO_HOST(INPUT64);
            else
                ++unbracketedLoggingFlag;
            ASSERT(BEEXP64 == result);
        }

        {
            if (veryVerbose) std::cout << "BSLS_BYTEORDER_HOST_U16_TO_LE"
                                       << std::endl;

            short result = 0;
            if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                result = BSLS_BYTEORDER_HOST_U16_TO_LE(INPUT16);
            else
                ++unbracketedLoggingFlag;
            ASSERT(LEEXP16 == result);
        }

        {
            if (veryVerbose) std::cout << "BSLS_BYTEORDER_HOST_U32_TO_LE"
                                       << std::endl;

            int result = 0;
            if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                result = BSLS_BYTEORDER_HOST_U32_TO_LE(INPUT32);
            else
                ++unbracketedLoggingFlag;
            ASSERT(LEEXP32 == result);
        }

        {
            if (veryVerbose) std::cout << "BSLS_BYTEORDER_HOST_U64_TO_LE"
                                       << std::endl;

            Int64 result = 0;
            if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                result = BSLS_BYTEORDER_HOST_U64_TO_LE(INPUT64);
            else
                ++unbracketedLoggingFlag;
            ASSERT(LEEXP64 == result);
        }

        {
            if (veryVerbose) std::cout << "BSLS_BYTEORDER_HOST_U16_TO_BE"
                                       << std::endl;

            short result = 0;
            if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                result = BSLS_BYTEORDER_HOST_U16_TO_BE(INPUT16);
            else
                ++unbracketedLoggingFlag;
            ASSERT(BEEXP16 == result);
        }

        {
            if (veryVerbose) std::cout << "BSLS_BYTEORDER_HOST_U32_TO_BE"
                                       << std::endl;

            int result = 0;
            if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                result = BSLS_BYTEORDER_HOST_U32_TO_BE(INPUT32);
            else
                ++unbracketedLoggingFlag;
            ASSERT(BEEXP32 == result);
        }

        {
            if (veryVerbose) std::cout << "BSLS_BYTEORDER_HOST_U64_TO_BE"
                                       << std::endl;

            Int64 result = 0;
            if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                result = BSLS_BYTEORDER_HOST_U64_TO_BE(INPUT64);
            else
                ++unbracketedLoggingFlag;
            ASSERT(BEEXP64 == result);
        }

        {
            if (veryVerbose) std::cout << "BSLS_BYTEORDER_HTONS" << std::endl;

            short result = 0;
            if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                result = BSLS_BYTEORDER_HTONS(INPUT16);
            else
                ++unbracketedLoggingFlag;
            ASSERT(BEEXP16 == result);
        }

        {
            if (veryVerbose) std::cout << "BSLS_BYTEORDER_HTONL" << std::endl;

            int result = 0;
            if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                result = BSLS_BYTEORDER_HTONL(INPUT32);
            else
                ++unbracketedLoggingFlag;
            ASSERT(BEEXP32 == result);
        }

        {
            if (veryVerbose) std::cout << "BSLS_BYTEORDER_HTONLL" << std::endl;

            Int64 result = 0;
            if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                result = BSLS_BYTEORDER_HTONLL(INPUT64);
            else
                ++unbracketedLoggingFlag;
            ASSERT(BEEXP64 == result);
        }

        {
            if (veryVerbose) std::cout << "BSLS_BYTEORDER_NTOHS" << std::endl;

            short result = 0;
            if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                result = BSLS_BYTEORDER_NTOHS(INPUT16);
            else
                ++unbracketedLoggingFlag;
            ASSERT(BEEXP16 == result);
        }

        {
            if (veryVerbose) std::cout << "BSLS_BYTEORDER_NTOHL" << std::endl;

            int result = 0;
            if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                result = BSLS_BYTEORDER_NTOHL(INPUT32);
            else
                ++unbracketedLoggingFlag;
            ASSERT(BEEXP32 == result);
        }

        {
            if (veryVerbose) std::cout << "BSLS_BYTEORDER_NTOHLL" << std::endl;

            Int64 result = 0;
            if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                result = BSLS_BYTEORDER_NTOHLL(INPUT64);
            else
                ++unbracketedLoggingFlag;
            ASSERT(BEEXP64 == result);
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BSLS_BYTEORDER_{LE,BE/HOST}_U{16,32,64}_TO_{HOST/LE,BE}
        //
        // Concerns:
        //   1) Bytes are properly swapped when host endian is not the same as
        //      the endian specified by the macro.
        //
        //      a) All bytes are 0.
        //      b) All bytes are 1.
        //      c) Only the MSB of a byte set / unset.
        //      d) Only the LSB of a byte set / unset.
        //
        //   2) Bytes are unchanged when host endian is the same as the endian
        //      specified by the macro.
        //
        // Plan:
        //   Using the table-driven technique, we will specify test vectors
        //   that address the test concerns, pass the value to the conversion
        //   macros and verify the result.
        //
        // Testing:
        //   BSLS_BYTEORDER_LE_U16_TO_HOST
        //   BSLS_BYTEORDER_LE_U32_TO_HOST
        //   BSLS_BYTEORDER_LE_U64_TO_HOST
        //   BSLS_BYTEORDER_BE_U16_TO_HOST
        //   BSLS_BYTEORDER_BE_U32_TO_HOST
        //   BSLS_BYTEORDER_BE_U64_TO_HOST
        //   BSLS_BYTEORDER_HOST_U16_TO_LE
        //   BSLS_BYTEORDER_HOST_U32_TO_LE
        //   BSLS_BYTEORDER_HOST_U64_TO_LE
        //   BSLS_BYTEORDER_HOST_U16_TO_BE
        //   BSLS_BYTEORDER_HOST_U32_TO_BE
        //   BSLS_BYTEORDER_HOST_U64_TO_BE
        // --------------------------------------------------------------------

        if (verbose) std::cout << std::endl
           << "TESTING BSLS_BYTEORDER_{LE,BE/HOST}_U{16,32,64}_TO_{HOST/LE,BE}"
           << std::endl
           << "==============================================================="
           << std::endl;

        if (verbose) std::cout
            << "\nTesting BSLS_BYTEORDER_{LE,BE/HOST}_U16_TO_{HOST/LE,BE}"
            << std::endl;
        {
            static const struct {
                int   d_lineNum;            // line number
                unsigned short d_input;     // test vector input
                unsigned short d_expected;  // expected output
            } DATA[] = {
                //LINE  INPUT                 EXPECTED
                //----  ----------            ----------
                { L_,   0x0000,               0x0000               },
                { L_,   0xFFFF,               0xFFFF               },
                { L_,   0x1234,               0x3412               },
                { L_,   0x3412,               0x1234               },
                { L_,   0x8000,               0x0080               },
                { L_,   0x1000,               0x0010               },
                { L_,   0x0800,               0x0008               },
                { L_,   0x0100,               0x0001               },
                { L_,   0x7FFF,               0xFF7F               },
                { L_,   0xFEFF,               0xFFFE               },
                { L_,   0xEFFF,               0xFFEF               },
                { L_,   0xF7FF,               0xFFF7               },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE     = DATA[ti].d_lineNum;
                const unsigned short INPUT    = DATA[ti].d_input;
                const unsigned short EXPECTED = DATA[ti].d_expected;

                if (veryVerbose) { T_ P_(LINE) PH(INPUT) }

                {

                    const unsigned short LEToHost =
                                          BSLS_BYTEORDER_LE_U16_TO_HOST(INPUT);
                    const unsigned short hostToLE =
                                          BSLS_BYTEORDER_HOST_U16_TO_LE(INPUT);

                    if (veryVeryVerbose) { T_ T_ PH_(LEToHost) PH_(hostToLE) }

#if BSLS_PLATFORM_IS_BIG_ENDIAN
                    const unsigned short EXP = EXPECTED;
#else
                    const unsigned short EXP = INPUT;
#endif
                    if (veryVeryVerbose) { PH(EXP) }

                    LOOP_ASSERT(LINE, EXP == LEToHost);
                    LOOP_ASSERT(LINE, EXP == hostToLE);

                }

                {

                    unsigned short BEToHost =
                                          BSLS_BYTEORDER_BE_U16_TO_HOST(INPUT);
                    unsigned short hostToBE =
                                          BSLS_BYTEORDER_HOST_U16_TO_BE(INPUT);

                    if (veryVeryVerbose) { T_ T_ PH_(BEToHost) PH_(hostToBE) }

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
                    const unsigned short EXP = EXPECTED;
#else
                    const unsigned short EXP = INPUT;
#endif
                    if (veryVeryVerbose) { PH(EXP) }

                    LOOP_ASSERT(LINE, EXP == BEToHost);
                    LOOP_ASSERT(LINE, EXP == hostToBE);
                }
            }
        }

        if (verbose) std::cout
            << "\nTesting BSLS_BYTEORDER_{LE,BE/HOST}_U32_TO_{HOST/LE,BE}"
            << std::endl;
        {
            static const struct {
                int d_lineNum;            // line number
                unsigned int d_input;     // test vector input
                unsigned int d_expected;  // expected output
            } DATA[] = {
                //LINE  INPUT                 EXPECTED
                //----  ----------            ----------
                { L_,   0x00000000,           0x00000000           },
                { L_,   0xFFFFFFFF,           0xFFFFFFFF           },
                { L_,   0x12345678,           0x78563412           },
                { L_,   0x78563412,           0x12345678           },
                { L_,   0x80000000,           0x00000080           },
                { L_,   0x10000000,           0x00000010           },
                { L_,   0x08000000,           0x00000008           },
                { L_,   0x01000000,           0x00000001           },
                { L_,   0x7FFFFFFF,           0xFFFFFF7F           },
                { L_,   0xFEFFFFFF,           0xFFFFFFFE           },
                { L_,   0xEFFFFFFF,           0xFFFFFFEF           },
                { L_,   0xF7FFFFFF,           0xFFFFFFF7           },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE              = DATA[ti].d_lineNum;
                const unsigned int INPUT    = DATA[ti].d_input;
                const unsigned int EXPECTED = DATA[ti].d_expected;

                if (veryVerbose) { T_ P_(LINE) PH(INPUT) }

                {
                    unsigned int LEToHost =
                                          BSLS_BYTEORDER_LE_U32_TO_HOST(INPUT);
                    unsigned int hostToLE =
                                          BSLS_BYTEORDER_HOST_U32_TO_LE(INPUT);

                    if (veryVeryVerbose) { T_ T_ PH_(LEToHost) PH_(hostToLE) }

#if BSLS_PLATFORM_IS_BIG_ENDIAN
                    const unsigned int EXP = EXPECTED;
#else
                    const unsigned int EXP = INPUT;
#endif
                    if (veryVeryVerbose) { PH(EXP) }

                    LOOP_ASSERT(LINE, EXP == LEToHost);
                    LOOP_ASSERT(LINE, EXP == hostToLE);
                }

                {

                    unsigned int BEToHost =
                                          BSLS_BYTEORDER_BE_U32_TO_HOST(INPUT);
                    unsigned int hostToBE =
                                          BSLS_BYTEORDER_HOST_U32_TO_BE(INPUT);

                    if (veryVeryVerbose) { T_ T_ PH_(BEToHost) PH_(hostToBE) }

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
                    const unsigned int EXP = EXPECTED;
#else
                    const unsigned int EXP = INPUT;
#endif
                    if (veryVeryVerbose) { PH(EXP) }

                    LOOP_ASSERT(LINE, EXP == BEToHost);
                    LOOP_ASSERT(LINE, EXP == hostToBE);
                }
            }
        }

        if (verbose) std::cout
            << "\nTesting BSLS_BYTEORDER_{LE,BE/HOST}_U64_TO_{HOST/LE,BE}"
            << std::endl;
        {
            static const struct {
                int    d_lineNum;   // line number
                Uint64 d_input;     // test vector input
                Uint64 d_expected;  // expected output
            } DATA[] = {
                //LINE  INPUT                 EXPECTED
                //----  ----------            ----------
                { L_,   0x0000000000000000LL, 0x0000000000000000LL },
                { L_,   0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL },
                { L_,   0x1234567890ABCDEFLL, 0xEFCDAB9078563412LL },
                { L_,   0xEFCDAB9078563412LL, 0x1234567890ABCDEFLL },
                { L_,   0x8000000000000000LL, 0x0000000000000080LL },
                { L_,   0x1000000000000000LL, 0x0000000000000010LL },
                { L_,   0x0800000000000000LL, 0x0000000000000008LL },
                { L_,   0x0100000000000000LL, 0x0000000000000001LL },
                { L_,   0x7FFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFF7FLL },
                { L_,   0xFEFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFELL },
                { L_,   0xEFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFEFLL },
                { L_,   0xF7FFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFF7LL },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE     = DATA[ti].d_lineNum;
                const Uint64 INPUT    = DATA[ti].d_input;
                const Uint64 EXPECTED = DATA[ti].d_expected;

                if (veryVerbose) { T_ P_(LINE) PH(INPUT) }

                {
                    Uint64 LEToHost = BSLS_BYTEORDER_LE_U64_TO_HOST(INPUT);
                    Uint64 hostToLE = BSLS_BYTEORDER_HOST_U64_TO_LE(INPUT);

                    if (veryVeryVerbose) { T_ T_ PH_(LEToHost) PH_(hostToLE) }

#if BSLS_PLATFORM_IS_BIG_ENDIAN
                    const Uint64 EXP = EXPECTED;
#else
                    const Uint64 EXP = INPUT;
#endif
                    if (veryVeryVerbose) { PH(EXP) }

                    LOOP_ASSERT(LINE, EXP == LEToHost);
                    LOOP_ASSERT(LINE, EXP == hostToLE);
                }

                {
                    Uint64 BEToHost = BSLS_BYTEORDER_BE_U64_TO_HOST(INPUT);
                    Uint64 hostToBE = BSLS_BYTEORDER_HOST_U64_TO_BE(INPUT);

                    if (veryVeryVerbose) { T_ T_ PH_(BEToHost) PH_(hostToBE) }

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
                    const Uint64 EXP = EXPECTED;
#else
                    const Uint64 EXP = INPUT;
#endif
                    if (veryVeryVerbose) { PH(EXP) }

                    LOOP_ASSERT(LINE, EXP == BEToHost);
                    LOOP_ASSERT(LINE, EXP == hostToBE);
                }
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING HTON{S,L,LL} AND HTON{S,L,LL}
        //
        // Concerns:
        //  1. That each byte in a small (2 byte), a long (4 bytes), and a long
        //     long (8 bytes) integer are properly shifted if necessary.
        //
        //  2. That the "CONSTANT" versions of these macros can be used to
        //     initialize static values.
        //
        // Plan:
        //  1. By using values with bits set on each byte, we will verify
        //     that the output of the macros are the same as the platform
        //     provided macros.
        //
        //  2. Verify that there is no compile error when initializing an
        //     'enum' with the CONSTANT versions of the macros.
        //
        // Testing:
        //   BSLS_BYTEORDER_HTONS
        //   BSLS_BYTEORDER_HTONL
        //   BSLS_BYTEORDER_HTONLL
        //   BSLS_BYTEORDER_NTOHS
        //   BSLS_BYTEORDER_NTOHL
        //   BSLS_BYTEORDER_NTOHLL
        //   BSLS_BYTEORDER_HTONS_CONSTANT
        //   BSLS_BYTEORDER_HTONL_CONSTANT
        //   BSLS_BYTEORDER_HTONLL_CONSTANT
        //   BSLS_BYTEORDER_NTOHS_CONSTANT
        //   BSLS_BYTEORDER_NTOHL_CONSTANT
        //   BSLS_BYTEORDER_NTOHLL_CONSTANT
        // --------------------------------------------------------------------

        if (verbose) std::cout << std::endl
                       << "TESTING HTON{S,L,LL} AND HTON{S,L,LL}" << std::endl
                       << "=====================================" << std::endl;

        if (verbose) std::cout << std::endl
                  << "Testing BSLS_BYTEORDER_HTONS and BSLS_BYTEORDER_NTOHS"
                  << std::endl
                  << "====================================================="
                  << std::endl;
        {
            static const struct {
                int            d_lineNum;  // source line number
                unsigned short d_value;
            } DATA[] = {
                //LINE  VALUE
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
                const int            LINE  = DATA[ti].d_lineNum;
                const unsigned short VALUE = DATA[ti].d_value;

                if (veryVerbose) { T_ P_(LINE) P(VALUE) }
                {
                    const short ORACLE    = htons(VALUE);
                    const short NEWVALUE  = BSLS_BYTEORDER_HTONS(VALUE);
                    const short NEWVALUE2 =
                                          BSLS_BYTEORDER_HTONS_CONSTANT(VALUE);

                    if (veryVeryVerbose) { T_ T_ P_(NEWVALUE) P_(NEWVALUE2)
                                                                    P(ORACLE) }

                    LOOP_ASSERT(LINE, ORACLE == NEWVALUE);
                    LOOP_ASSERT(LINE, ORACLE == NEWVALUE2);
                }
                {
                    const short ORACLE    = ntohs(VALUE);
                    const short NEWVALUE  = BSLS_BYTEORDER_NTOHS(VALUE);
                    const short NEWVALUE2 =
                                          BSLS_BYTEORDER_NTOHS_CONSTANT(VALUE);

                    if (veryVeryVerbose) { T_ T_ P_(NEWVALUE) P_(NEWVALUE2)
                                                                    P(ORACLE) }

                    LOOP_ASSERT(LINE, ORACLE == NEWVALUE);
                    LOOP_ASSERT(LINE, ORACLE == NEWVALUE2);
                }
            }
        }

        if (verbose) std::cout << std::endl
                  << "Testing BSLS_BYTEORDER_HTONL and BSLS_BYTEORDER_NTOHL"
                  << std::endl
                  << "====================================================="
                  << std::endl;
        {
            static const struct {
                int   d_lineNum;    // source line number
                int   d_value;
            } DATA[] = {
                //LINE  VALUE
                //----  ------------
                { L_,            0, },
                { L_,   (int) 0xff000000, },
                { L_,         0x00ff0000, },
                { L_,         0x0000ff00, },
                { L_,         0x000000ff, },
                { L_,   (int) 0xffffffff, },
                { L_,   (int) 0x80000000, },
                { L_,         0x01000000, },
                { L_,         0x00800000, },
                { L_,         0x00010000, },
                { L_,         0x00008000, },
                { L_,         0x00000100, },
                { L_,         0x00000080, },
                { L_,         0x00000001, },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_lineNum;
                const int VALUE = DATA[ti].d_value;

                if (veryVerbose) { T_ P_(LINE) P(VALUE) }

                {
                    const int ORACLE    = htonl(VALUE);
                    const int NEWVALUE  = BSLS_BYTEORDER_HTONL(VALUE);
                    const int NEWVALUE2 = BSLS_BYTEORDER_HTONL_CONSTANT(VALUE);

                    if (veryVeryVerbose) { T_ T_ P_(NEWVALUE) P_(NEWVALUE2)
                                                                    P(ORACLE) }

                    LOOP_ASSERT(LINE, ORACLE == NEWVALUE);
                    LOOP_ASSERT(LINE, ORACLE == NEWVALUE2);
                }
                {
                    const int ORACLE    = ntohl(VALUE);
                    const int NEWVALUE  = BSLS_BYTEORDER_NTOHL(VALUE);
                    const int NEWVALUE2 = BSLS_BYTEORDER_NTOHL_CONSTANT(VALUE);

                    if (veryVeryVerbose) { T_ T_ P_(NEWVALUE) P_(NEWVALUE2)
                                                                    P(ORACLE) }

                    LOOP_ASSERT(LINE, ORACLE == NEWVALUE);
                    LOOP_ASSERT(LINE, ORACLE == NEWVALUE2);
                }
            }
        }

        if (verbose) std::cout << std::endl
                  << "Testing BSLS_BYTEORDER_HTONLL and BSLS_BYTEORDER_NTOHLL"
                  << std::endl
                  << "======================================================="
                  << std::endl;
        {
            static const struct {
                int    d_lineNum;
                Uint64 d_value;
                Uint64 d_expected;  // needed because there is no oracle
            } DATA[] = {
                //LINE  VALUE                 EXPECTED
                //----  --------------------  --------------------
                { L_,                      0,                    0 },
                { L_,   0xff00000000000000LL, 0x00000000000000ffLL },
                { L_,   0x00ff000000000000LL, 0x000000000000ff00LL },
                { L_,   0x0000ff0000000000LL, 0x0000000000ff0000LL },
                { L_,   0x000000ff00000000LL, 0x00000000ff000000LL },
                { L_,   0x00000000ff000000LL, 0x000000ff00000000LL },
                { L_,   0x0000000000ff0000LL, 0x0000ff0000000000LL },
                { L_,   0x000000000000ff00LL, 0x00ff000000000000LL },
                { L_,   0x00000000000000ffLL, 0xff00000000000000LL },
                { L_,   0xffffffffffffffffLL, 0xffffffffffffffffLL },
                { L_,   0x8000000000000000LL, 0x0000000000000080LL },
                { L_,   0x0100000000000000LL, 0x0000000000000001LL },
                { L_,   0x0080000000000000LL, 0x0000000000008000LL },
                { L_,   0x0001000000000000LL, 0x0000000000000100LL },
                { L_,   0x0000800000000000LL, 0x0000000000800000LL },
                { L_,   0x0000010000000000LL, 0x0000000000010000LL },
                { L_,   0x0000008000000000LL, 0x0000000080000000LL },
                { L_,   0x0000000100000000LL, 0x0000000001000000LL },
                { L_,   0x0000000080000000LL, 0x0000008000000000LL },
                { L_,   0x0000000001000000LL, 0x0000000100000000LL },
                { L_,   0x0000000000800000LL, 0x0000800000000000LL },
                { L_,   0x0000000000010000LL, 0x0000010000000000LL },
                { L_,   0x0000000000008000LL, 0x0080000000000000LL },
                { L_,   0x0000000000000100LL, 0x0001000000000000LL },
                { L_,   0x0000000000000080LL, 0x8000000000000000LL },
                { L_,   0x0000000000000001LL, 0x0100000000000000LL },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE     = DATA[ti].d_lineNum;
                const Uint64 VALUE    = DATA[ti].d_value;
                const Uint64 EXPECTED = DATA[ti].d_expected;

                if (veryVerbose) { T_ P_(LINE) P_(VALUE) P(EXPECTED) }

#if defined (BSLS_PLATFORM_IS_BIG_ENDIAN)
                {
                    const Uint64 NEWVALUE  = BSLS_BYTEORDER_HTONLL(VALUE);
                    const Uint64 NEWVALUE2 =
                                         BSLS_BYTEORDER_HTONLL_CONSTANT(VALUE);

                    if (veryVeryVerbose) { T_ T_ P_(NEWVALUE) P(NEWVALUE2) }
                    LOOP_ASSERT(LINE, VALUE == NEWVALUE);
                    LOOP_ASSERT(LINE, VALUE == NEWVALUE2);
                }
                {
                    const Uint64 NEWVALUE  = BSLS_BYTEORDER_NTOHLL(VALUE);
                    const Uint64 NEWVALUE2 =
                                         BSLS_BYTEORDER_NTOHLL_CONSTANT(VALUE);

                    if (veryVeryVerbose) { T_ T_ P_(NEWVALUE) P(NEWVALUE2) }
                    LOOP_ASSERT(LINE, VALUE == NEWVALUE);
                    LOOP_ASSERT(LINE, VALUE == NEWVALUE2);
                }
#else
                {
                    const Uint64 NEWVALUE  = BSLS_BYTEORDER_HTONLL(VALUE);
                    const Uint64 NEWVALUE2 =
                                         BSLS_BYTEORDER_HTONLL_CONSTANT(VALUE);

                    if (veryVeryVerbose) { T_ T_ P_(NEWVALUE) P(NEWVALUE2) }
                    LOOP_ASSERT(LINE, EXPECTED == NEWVALUE);
                    LOOP_ASSERT(LINE, EXPECTED == NEWVALUE2);
                }
                {
                    const Uint64 NEWVALUE  = BSLS_BYTEORDER_NTOHLL(VALUE);
                    const Uint64 NEWVALUE2 =
                                         BSLS_BYTEORDER_NTOHLL_CONSTANT(VALUE);

                    if (veryVeryVerbose) { T_ T_ P_(NEWVALUE) P(NEWVALUE2) }
                    LOOP_ASSERT(LINE, EXPECTED == NEWVALUE);
                    LOOP_ASSERT(LINE, EXPECTED == NEWVALUE2);
                }
#endif
            }
        }

        {
            if (verbose) std::cout <<
                     std::endl << "Testing compile-time initialization" <<
                     std::endl << "===================================" <<
                     std::endl;

            enum { TEST1 = BSLS_BYTEORDER_HTONS_CONSTANT(1),
                   TEST2 = BSLS_BYTEORDER_HTONL_CONSTANT(1),
                   TEST3 = BSLS_BYTEORDER_HTONLL_CONSTANT(1LL),
                   TEST4 = BSLS_BYTEORDER_NTOHS_CONSTANT(1),
                   TEST5 = BSLS_BYTEORDER_NTOHL_CONSTANT(1),
                   TEST6 = BSLS_BYTEORDER_NTOHLL_CONSTANT(1LL) };
        }

      } break;
      default: {
        std::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << std::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        std::cerr << "Error, non-zero test status = " << testStatus << "."
                  << std::endl;
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
