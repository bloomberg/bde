// bsls_byteorderutil_impl.t.cpp                                      -*-C++-*-

#include <bsls_byteorderutil_impl.h>

#include <bsls_bsltestutil.h>    // for testing only

#include <cstdlib>
#include <cstdio>

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// [3] TESTING CONSISTENCY OF COMPILE-TIME FLAGS
// [2] TESTING 'mySwapBytes*'
// [1] TESTING 'BSLS_BYTEORDERUTIL_IMPL_GENERIC_SWAP*' & SOUNDNESS OF TABLE
//-----------------------------------------------------------------------------
// [3] 'BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_*'
// [2] 'mySwapBytes[16,32,64}'
// [1] 'BSLS_BYTEORDERUTIL_IMPL_GENERIC_SWAP{16,32,64}'
//-----------------------------------------------------------------------------

using namespace BloombergLP;
using std::printf;
using std::fprintf;

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

#define PHEX(X)  printf(#X " = 0x%llx\n", (Uint64) (X));
#define PHEX_(X) printf(#X " = 0x%llx\t", (Uint64) (X));                      \
                 bsls::BslTestUtil::flush();


#if defined(BSLS_PLATFORM_OS_WINDOWS) ||                                      \
    (defined(BSLS_PLATFORM_OS_AIX) && defined(BSLS_PLATFORM_CPU_32_BIT))
#define BYTEORDERUTIL_SIZEOF_WCHAR_T 2
#else
#define BYTEORDERUTIL_SIZEOF_WCHAR_T 4
#endif

#if defined(BSLS_PLATFORM_CPU_64_BIT) && defined(BSLS_PLATFORM_OS_UNIX)
#define BYTEORDERUTIL_SIZEOF_LONG 8
#else
#define BYTEORDERUTIL_SIZEOF_LONG 4
#endif

//=============================================================================
//                         GLOBAL TYPEDEF FOR TESTING
//-----------------------------------------------------------------------------

typedef BloombergLP::bsls::Types::Int64  Int64;
typedef BloombergLP::bsls::Types::Uint64 Uint64;

//=============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <class TYPE>
void swapBytesInPlace(TYPE *value)
    // Swap the byte order of the specified integral object '*value'.
{
    char *pc = reinterpret_cast<char *>(value);

    for (int h = 0, t = sizeof(*value) - 1; h < t; ++h, --t) {
        char tmp = pc[h];
        pc[h] = pc[t];
        pc[t] = tmp;
    }
}

inline
unsigned short
mySwapBytes16(unsigned short x)
    // Return the value of the specified 'x' with the byte order reversed.
{
    // These macros all return a value of type 'unsigned short'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16( unsigned short, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16(unsigned short, &x);
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_16(unsigned short, x);        // RETURN
#endif
}

inline
unsigned int
mySwapBytes32(unsigned int x)
    // Return the value of the specified 'x' with the byte order reversed.
{
    // These macros all return a value of type 'unsigned int'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32( unsigned int, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32(unsigned int, &x);
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_32(unsigned int, x);
#endif
}

inline
bsls::Types::Uint64
mySwapBytes64(bsls::Types::Uint64 x)
    // Return the value of the specified 'x' with the byte order reversed.
{
    // These macros all return a value of type 'bsls::Types::Uint64'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64( bsls::Types::Uint64, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64(bsls::Types::Uint64, &x);
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_64(bsls::Types::Uint64, x);
#endif
}

inline
unsigned short
myGenericSwap16(unsigned short x)
    // Return the value of the specified 'x' with the byte order reversed.
{
    // This macro will return a value of type 'unsigned short'.

    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_16(unsigned short, x);        // RETURN
}

inline
unsigned int
myGenericSwap32(unsigned int x)
    // Return the value of the specified 'x' with the byte order reversed.
{
    // This macro will return a value of type 'unsigned int'.

    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_32(unsigned int, x);          // RETURN
}

inline
bsls::Types::Uint64
myGenericSwap64(bsls::Types::Uint64 x)
    // Return the value of the specified 'x' with the byte order reversed.
{
    // This macro will return a value of type 'bsls::Types::Uint64'.

    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_64(bsls::Types::Uint64, x);   // RETURN
}

//=============================================================================
//                                TABLE DATA
//-----------------------------------------------------------------------------

static const struct {
    int            d_lineNum;  // source line number
    unsigned short d_value;
    unsigned short d_swapped;
} data16[] = {
    //LINE  VALUE   EXPECTED
    //----  ------  ------

    { L_,   0x0000, 0x0000 },
    { L_,   0xff00, 0x00ff },
    { L_,   0x00ff, 0xff00 },
    { L_,   0xffff, 0xffff },
    { L_,   0x8000, 0x0080 },
    { L_,   0x0100, 0x0001 },
    { L_,   0x0080, 0x8000 },
    { L_,   0x0001, 0x0100 },
    { L_,   0x1001, 0x0110 },
    { L_,   0x2002, 0x0220 },
    { L_,   0xc00d, 0x0dc0 },
    { L_,   0x1234, 0x3412 },
    { L_,   0x3412, 0x1234 },
    { L_,   0x1000, 0x0010 },
    { L_,   0x0800, 0x0008 },
    { L_,   0x7fff, 0xff7f },
    { L_,   0xfeff, 0xfffe },
    { L_,   0xefff, 0xffef },
    { L_,   0xf7ff, 0xfff7 },
};
enum { k_NUM_DATA16 = sizeof data16 / sizeof *data16 };

static const struct {
    int            d_lineNum;    // source line number
    unsigned int   d_value;
    unsigned int   d_swapped;
} data32[] = {
    //LINE  VALUE        EXPECTED
    //----  ----------   ----------

    { L_,            0,           0 },
    { L_,   0xff000000,  0x000000ff },
    { L_,   0x00ff0000,  0x0000ff00 },
    { L_,   0x0000ff00,  0x00ff0000 },
    { L_,   0x000000ff,  0xff000000 },
    { L_,   0xffffffff,  0xffffffff },
    { L_,   0x80000000,  0x00000080 },
    { L_,   0x01000000,  0x00000001 },
    { L_,   0x00800000,  0x00008000 },
    { L_,   0x00010000,  0x00000100 },
    { L_,   0x00008000,  0x00800000 },
    { L_,   0x11000011,  0x11000011 },
    { L_,   0x10000001,  0x01000010 },
    { L_,   0xab0000cd,  0xcd0000ab },
    { L_,   0x00abcd00,  0x00cdab00 },
    { L_,   0xabcd0000,  0x0000cdab },
    { L_,   0x00000100,  0x00010000 },
    { L_,   0x00000080,  0x80000000 },
    { L_,   0x00000001,  0x01000000 },
    { L_,   0x12345678,  0x78563412 },
    { L_,   0x78563412,  0x12345678 },
    { L_,   0x10000000,  0x00000010 },
    { L_,   0x08000000,  0x00000008 },
    { L_,   0x7fffffff,  0xffffff7f },
    { L_,   0xfeffffff,  0xfffffffe },
    { L_,   0xefffffff,  0xffffffef },
    { L_,   0xf7ffffff,  0xfffffff7 },
};
enum { k_NUM_DATA32 = sizeof data32 / sizeof *data32 };

static const struct {
    int    d_lineNum;
    Uint64 d_value;
    Uint64 d_swapped;
} data64[] = {
    //LINE  VALUE                  EXPECTED
    //----  ---------------------  ---------------------

    { L_,                       0,                     0 },
    { L_,   0xff00000000000000ULL, 0x00000000000000ffULL },
    { L_,   0x00ff000000000000ULL, 0x000000000000ff00ULL },
    { L_,   0x0000ff0000000000ULL, 0x0000000000ff0000ULL },
    { L_,   0x000000ff00000000ULL, 0x00000000ff000000ULL },
    { L_,   0x00000000ff000000ULL, 0x000000ff00000000ULL },
    { L_,   0x0000000000ff0000ULL, 0x0000ff0000000000ULL },
    { L_,   0x000000000000ff00ULL, 0x00ff000000000000ULL },
    { L_,   0x00000000000000ffULL, 0xff00000000000000ULL },
    { L_,   0xffffffffffffffffULL, 0xffffffffffffffffULL },
    { L_,   0x8000000000000000ULL, 0x0000000000000080ULL },
    { L_,   0x8000000000000008ULL, 0x0800000000000080ULL },
    { L_,   0x1000000000000001ULL, 0x0100000000000010ULL },
    { L_,   0x1100000000000011ULL, 0x1100000000000011ULL },
    { L_,   0x0100000000000000ULL, 0x0000000000000001ULL },
    { L_,   0x0080000000000000ULL, 0x0000000000008000ULL },
    { L_,   0x0001000000000000ULL, 0x0000000000000100ULL },
    { L_,   0x0000800000000000ULL, 0x0000000000800000ULL },
    { L_,   0x0000010000000000ULL, 0x0000000000010000ULL },
    { L_,   0x0000008000000000ULL, 0x0000000080000000ULL },
    { L_,   0x0000000100000000ULL, 0x0000000001000000ULL },
    { L_,   0x0000000080000000ULL, 0x0000008000000000ULL },
    { L_,   0x0000000001000000ULL, 0x0000000100000000ULL },
    { L_,   0x0000000000800000ULL, 0x0000800000000000ULL },
    { L_,   0x0000000000010000ULL, 0x0000010000000000ULL },
    { L_,   0x0000000000008000ULL, 0x0080000000000000ULL },
    { L_,   0x0000000000000100ULL, 0x0001000000000000ULL },
    { L_,   0x0000000000000080ULL, 0x8000000000000000ULL },
    { L_,   0x0000000000000001ULL, 0x0100000000000000ULL },
    { L_,   0x1234567890abcdefULL, 0xefcdab9078563412ULL },
    { L_,   0xefcdab9078563412ULL, 0x1234567890abcdefULL },
    { L_,   0x8000000000000000ULL, 0x0000000000000080ULL },
    { L_,   0x1000000000000000ULL, 0x0000000000000010ULL },
    { L_,   0x0800000000000000ULL, 0x0000000000000008ULL },
    { L_,   0x0100000000000000ULL, 0x0000000000000001ULL },
    { L_,   0x7fffffffffffffffULL, 0xffffffffffffff7fULL },
    { L_,   0xfeffffffffffffffULL, 0xfffffffffffffffeULL },
    { L_,   0xefffffffffffffffULL, 0xffffffffffffffefULL },
    { L_,   0xf7ffffffffffffffULL, 0xfffffffffffffff7ULL },
};
enum { k_NUM_DATA64 = sizeof data64 / sizeof *data64 };

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONSISTENCY OF COMPILE-TIME FLAGS
        //
        // Concerns:
        //: 1 When run in 'veryVerbose' mode, display which compile-time flags
        //:   are defined and which are not.
        //:
        //: 2 Verify that '*_CUSTOMSWAP_NN' and '_CUSTOMSWAPP_NN' are never
        //:   both set for any value of 'NN' on any platform (note that it is
        //:   permissible for neither to be set).
        //
        // Plan:
        //: 1 Do '#ifdef'-driven output (C-1).
        //:
        //: 2 Construct an '#if' statement to check the consistency of the
        //:   macros being set.
        //
        // Testing:
        //   'BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_*'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONSISTENCY OF COMPILE-TIME FLAGS\n"
                              "=========================================\n");

        if (veryVerbose) {
#ifdef BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16 defined);
#else
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16 not defined);
#endif

#ifdef BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16 defined);
#else
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16 not defined);
#endif

#ifdef BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32 defined);
#else
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32 not defined);
#endif

#ifdef BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32 defined);
#else
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32 not defined);
#endif

#ifdef BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64 defined);
#else
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64 not defined);
#endif

#ifdef BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64 defined);
#else
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64 not defined);
#endif
        }

        // Check that no two custom flags are simultaneously set for how to
        // implement swapping for the same word width.

#if    (defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16) &&                     \
        defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16))                      \
    || (defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32) &&                     \
        defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32))                      \
    || (defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64) &&                     \
        defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64))
#error inconsistent compiler flags
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'mySwapBytes*'
        //
        // Concerns:
        //: 1 That all incarnations of '*_CUSTOMSWAP_NN', '*_CUSTOMSWAP_PNN',
        //:   and '_GENERICSWAP_NN' work.
        //
        // Plan:
        //: 1 Create the functions 'mySwapBytesNN' (before 'main', above),
        //:   which, if called on all platforms will result in all incarnations
        //:   of the functions under tests being tested.
        //:
        //: 2 Use the tables 'data16', 'data32', and 'data64', defined at file
        //:   scope above, to provide test data and its swapped values.
        //:
        //: 3 For word widths 16, 32, and 64
        //:   A Test swapping signed and unsigned types with 'mySwapBytesNN'.
        //:   B Test swapping the unsigned type with 'myGenericSwapNN'
        //:   C Use '#ifdef's to determine if 'long' or 'wchar_t' have the
        //:     designated word width under test, and if so, test it.
        //
        // Testing:
        //   'mySwapBytes[16,32,64}'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'mySwapBytes*'\n"
                              "======================\n");

        bool wchar_t_tested = false;
        bool long_tested    = false;

        if (verbose) printf("Testing 16 Bit\n");
        {
            for (int ti = 0; ti < k_NUM_DATA16; ++ti) {
                const int            line     = data16[ti].d_lineNum;
                const unsigned short uValue   = data16[ti].d_value;
                const unsigned short uSwapped = data16[ti].d_swapped;
                const short          iValue   = data16[ti].d_value;
                const short          iSwapped = data16[ti].d_swapped;

                if (veryVerbose) { PHEX_(uValue); PHEX(uSwapped); }

                LOOP_ASSERT(line, uSwapped == mySwapBytes16(uValue));
                LOOP_ASSERT(line, uSwapped == myGenericSwap16(uValue));
                LOOP_ASSERT(line, static_cast<unsigned short>(iSwapped) ==
                                                        mySwapBytes16(iValue));
#if 2 == BYTEORDERUTIL_SIZEOF_WCHAR_T
                ASSERT(2 == sizeof(wchar_t));

                const wchar_t wValue   = (wchar_t) uValue;
                const wchar_t wSwapped = (wchar_t) uSwapped;

                LOOP_ASSERT(line, wSwapped == mySwapBytes16(wValue));

                wchar_t_tested = true;
#endif
            }
        }

        if (verbose) printf("Testing 32 Bit\n");
        {
            for (int ti = 0; ti < k_NUM_DATA32; ++ti) {
                const int          line     = data32[ti].d_lineNum;
                const unsigned int uValue   = data32[ti].d_value;
                const unsigned int uSwapped = data32[ti].d_swapped;
                const int          iValue   = data32[ti].d_value;
                const int          iSwapped = data32[ti].d_swapped;

                if (veryVerbose) { PHEX_(uValue); PHEX(uSwapped); }

                LOOP_ASSERT(line, uSwapped == mySwapBytes32(uValue));
                LOOP_ASSERT(line, uSwapped == myGenericSwap32(uValue));
                LOOP_ASSERT(line, iSwapped == (int) mySwapBytes32(iValue));

#if 4 == BYTEORDERUTIL_SIZEOF_WCHAR_T
                ASSERT(4 == sizeof(wchar_t));

                const wchar_t wValue   = (wchar_t) uValue;
                const wchar_t wSwapped = (wchar_t) uSwapped;

                LOOP_ASSERT(line, wSwapped ==
                                          (wchar_t) mySwapBytes32(wValue));

                wchar_t_tested = true;
#endif

#if 4 == BYTEORDERUTIL_SIZEOF_LONG
                ASSERT(4 == sizeof(long));

                const long          iLValue   = (long) uValue;
                const long          iLSwapped = (long) uSwapped;

                const unsigned long uLValue   = (unsigned long) uValue;
                const unsigned long uLSwapped = (unsigned long) uSwapped;

                LOOP_ASSERT(line, iLSwapped == (long) mySwapBytes32(iLValue));
                LOOP_ASSERT(line, iLValue  == (long) mySwapBytes32(iLSwapped));

                LOOP_ASSERT(line, uLSwapped == (unsigned long)
                                                       mySwapBytes32(uLValue));
                LOOP_ASSERT(line, uLValue   == (unsigned long)
                                                     mySwapBytes32(uLSwapped));

                long_tested = true;
#endif
            }
        }

        ASSERT(wchar_t_tested);

        if (verbose) printf("Testing 64 Bit\n");
        {
            for (int ti = 0; ti < k_NUM_DATA64; ++ti) {
                const int    line     = data64[ti].d_lineNum;
                const Uint64 uValue   = data64[ti].d_value;
                const Uint64 uSwapped = data64[ti].d_swapped;
                const Int64  iValue   = data64[ti].d_value;
                const Int64  iSwapped = data64[ti].d_swapped;

                if (veryVerbose) { PHEX_(uValue); PHEX(uSwapped); }

                LOOP_ASSERT(line, uSwapped == mySwapBytes64(uValue));
                LOOP_ASSERT(line, uSwapped == myGenericSwap64(uValue));
                LOOP_ASSERT(line, iSwapped == (Int64) mySwapBytes64(iValue));

#if 8 == BYTEORDERUTIL_SIZEOF_LONG
                ASSERT(8 == sizeof(long));

                const long          iLValue   = (long) uValue;
                const long          iLSwapped = (long) uSwapped;

                const unsigned long uLValue   = (unsigned long) uValue;
                const unsigned long uLSwapped = (unsigned long) uSwapped;

                LOOP_ASSERT(line, iLSwapped == (long) mySwapBytes64(iLValue));
                LOOP_ASSERT(line, iLValue  == (long) mySwapBytes64(iLSwapped));

                LOOP_ASSERT(line, uLSwapped == (unsigned long)
                                                       mySwapBytes64(uLValue));
                LOOP_ASSERT(line, uLValue   == (unsigned long)
                                                     mySwapBytes64(uLSwapped));

                long_tested = true;
#endif
            }
        }

        ASSERT(long_tested);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'myGenericSwap*' & SOUNDNESS OF TABLES
        //
        // Concerns:
        //: 1 Test that the tables 'data16', 'data32', and 'data64', accurately
        //:   provide swapped values for their types.
        //:
        //: 2 Test that the tables are of significant length (> 10 entries).
        //:
        //: 3 Test that 'BSLS_BYTEORDERUTIL_IMPL_GENERIC_SWAPNN' works
        //:   correctly.
        //
        // Plan:
        //: 1 Create a template function 'swapBytesInPlace' that, given a
        //:   pointer to an object of any integral type, will reverse the byte
        //:   order of the object.
        //:
        //: 2 For each table:
        //:   A Assert that the table length is >10.
        //:   B Traverse the table, apply 'swapBytesInPlace' to verify the
        //:     expected swapped value matches the swapped value.
        //:   6 Call 'myGenericSwapNN' on signed and unsigned values and
        //:     confirm the result matches the expected value.
        //
        // Testing:
        //   'BSLS_BYTEORDERUTIL_IMPL_GENERIC_SWAP{16,32,64}'
        // --------------------------------------------------------------------

        if (verbose) printf(
                         "\nTESTING '*_GENERICSWAP_*' & SOUNDNESS OF TABLE\n"
                           "==============================================\n");

        if (verbose) printf("Testing 16 Bit\n");
        {
            ASSERT(k_NUM_DATA16 > 10);

            for (int ti = 0; ti < k_NUM_DATA16; ++ti) {
                const int            line     = data16[ti].d_lineNum;
                const unsigned short uValue   = data16[ti].d_value;
                const unsigned short uSwapped = data16[ti].d_swapped;
                const short          iValue   = data16[ti].d_value;
                const short          iSwapped = data16[ti].d_swapped;

                if (veryVerbose) { PHEX_(uValue); PHEX(uSwapped); }

                unsigned short uVerify = uValue;
                swapBytesInPlace(&uVerify);
                ASSERT(uSwapped == uVerify);

                LOOP_ASSERT(line, uSwapped == myGenericSwap16(uValue));
                LOOP_ASSERT(line, static_cast<unsigned short>(iSwapped) ==
                                                      myGenericSwap16(iValue));
            }
        }

        if (verbose) printf("Testing 32 Bit\n");
        {
            ASSERT(k_NUM_DATA32 > 10);

            for (int ti = 0; ti < k_NUM_DATA32; ++ti) {
                const int          line     = data32[ti].d_lineNum;
                const unsigned int uValue   = data32[ti].d_value;
                const unsigned int uSwapped = data32[ti].d_swapped;
                const int          iValue   = data32[ti].d_value;
                const int          iSwapped = data32[ti].d_swapped;

                if (veryVerbose) { PHEX_(uValue); PHEX(uSwapped); }

                unsigned int uVerify = uValue;
                swapBytesInPlace(&uVerify);
                ASSERT(uSwapped == uVerify);

                LOOP_ASSERT(line, uSwapped == myGenericSwap32(uValue));
                LOOP_ASSERT(line, iSwapped == (int) myGenericSwap32(iValue));
            }
        }

        if (verbose) printf("Testing 64 Bit\n");
        {
            ASSERT(k_NUM_DATA64 > 10);

            for (int ti = 0; ti < k_NUM_DATA64; ++ti) {
                const int    line     = data64[ti].d_lineNum;
                const Uint64 uValue   = data64[ti].d_value;
                const Uint64 uSwapped = data64[ti].d_swapped;
                const Int64  iValue   = data64[ti].d_value;
                const Int64  iSwapped = data64[ti].d_swapped;

                if (veryVerbose) { PHEX_(uValue); PHEX(uSwapped); }

                Uint64 uVerify = uValue;
                swapBytesInPlace(&uVerify);
                ASSERT(uSwapped == uVerify);

                LOOP_ASSERT(line, uSwapped == myGenericSwap64(uValue));
                LOOP_ASSERT(line, iSwapped == (Int64) myGenericSwap64(iValue));
            }
        }
      } break;
      default: {
        std::fprintf(stderr, "WARNING: CASE '$d' NOT FOUND.\n");
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        std::fprintf(stderr, "Error, non-zero test status = %d.\n",testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
