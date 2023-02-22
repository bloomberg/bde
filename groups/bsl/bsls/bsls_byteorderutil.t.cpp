// bsls_byteorderutil.t.cpp                                           -*-C++-*-

#include <bsls_byteorderutil.h>

#include <bsls_assert.h>         // for testing only
#include <bsls_bsltestutil.h>    // for testing only
#include <bsls_stopwatch.h>      // for testing only

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <limits>

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test contains a utility 'class' containing a collection
// of single-argument, passed by value, static functions that return their
// results.  There are two classes of functions:
//
//: o 'TYPE swapBytes(TYPE)' where 'TYPE' is any integral type
//: o 'TYPE swapBytes{16,32,64}(TYPE) where 'TYPE' is an unsigned integral type
//:   whose word width in bits is indicated by the number at the end of the
//:   function name.
//
// The 'swapBytes(TYPE)' methods themselves fall into two categories
//: o functions that take a single-byte argument, which is returned without
//:   modification
//: o functions that take a multi-byte argument, which is return with the byte
//:   order reversed.
//
// Tables are created in this component with 2, 4, and 8-byte integral values,
// along with the same value with byte order reversed.  For every integral
// type, the type is loaded with the unswapped value, then this is fed into
// function under test, and the result compared to the swapped value.
//
// Some types vary in size across different platforms, and accommodation is
// made for this, plus tests to make sure that all types are in fact tested.
//
// Single byte values are tested for all possible value in the 'singleByteTest'
// template function.
//
// [5] USAGE EXAMPLE
// [4] TESTING SINGLE BYTE OBJECTS
// [3] TESTING TYPE MATCHING
// [2] TESTING 'swapBytes', 'swapBytes{16,32,64}'
// [1] BREATHING TEST
//-----------------------------------------------------------------------------
// [ 5] USAGE
// [ 4] static TYPE swapBytes(TYPE) for all single-byte 'TYPE'
// [ 3] static TYPE swapBytes(TYPE) for all integral 'TYPE'
// [ 2] static TYPE swapBytes(TYPE) for all multi-byte integral 'TYPE'
// [ 2] static unsigned short swapBytes16(unsigned short)
// [ 2] static unsigned int swapBytes32(unsigned int)
// [ 2] static bsls::Uint64 swapBytes32(bsls::Uint64)
// [ 1] static TYPE swapBytes(TYPE) for all multi-byte integral 'TYPE'
// [ 1] static unsigned short swapBytes16(unsigned short)
// [ 1] static unsigned int swapBytes32(unsigned int)
// [ 1] static bsls::Uint64 swapBytes32(bsls::Uint64)
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
# define BYTEORDERUTIL_SIZEOF_WCHAR_T 2
#else
# define BYTEORDERUTIL_SIZEOF_WCHAR_T 4
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

typedef BloombergLP::bsls::ByteOrderUtil Util;

//=============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

template <class LHS_TYPE, class RHS_TYPE>
bool isSameType(const LHS_TYPE&, const RHS_TYPE&)
    // Return 'false' because 'LHS_TYPE' and 'RHS_TYPE' are different.
{
    return false;
}

template <class TYPE>
bool isSameType(const TYPE&, const TYPE&)
    // Return 'true' because both of the arguments are of the same type.
{
    return true;
}

template <class TYPE>
void singleByteTest()
    // Verify that 'Util::swapBytes' returns the value passed without
    // modification for all possible values passed to it when taking an arg of
    // single byte type 'TYPE'.
{
    int counter = 0;

    TYPE x = std::numeric_limits<TYPE>::min();
    ASSERT(1 == sizeof(x));
    while (true) {
        ASSERT(Util::swapBytes(x) == x);

        ++counter;
        if (std::numeric_limits<TYPE>::max() == x) {
            break;
        }
        ++x;
    }

    ASSERT(256 == counter);
}

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
myGenericSwap16(unsigned short x)
{
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_16(unsigned short, x);
}

inline
unsigned int
myGenericSwap32(unsigned int x)
{
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_32(unsigned int, x);
}

inline
bsls::Types::Uint64
myGenericSwap64(bsls::Types::Uint64 x)
{
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_64(bsls::Types::Uint64, x);
}

}  // close unnamed namespace

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

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE\n"
                              "=============\n");

// In this example we demonstrate the use of different overloads of the
// 'swapBytes' function.
//
// First we 'typedef' a shorthand to the utility 'class':
//..
    typedef bsls::ByteOrderUtil Util;
//..
// Then, we demonstrate reversing the bytes of an 'unsigned short':
//..
    unsigned short us = 0x1234;
    ASSERT(0x3412 == Util::swapBytes(us));
//..
// Next, we do a signed 'short:
//..
    short ss = 0x4321;
    ASSERT(0x2143 == Util::swapBytes(ss));
//..
// Then, we reverse an 'unsigned int':
//..
    unsigned int ui = 0x01020304;
    ASSERT(0x04030201 == Util::swapBytes(ui));
//..
// Next, we reverse the bytes of a 32-bit signed integer:
//..
    int si = 0x11223344;
    ASSERT(0x44332211 == Util::swapBytes(si));
//..
// Now, we perform the transform on a 64-bit unsigned:
//..
    bsls::Types::Uint64 ui64 = 0x0102030405060708ULL;
    ASSERT(0x0807060504030201ULL == Util::swapBytes(ui64));
//..
// Finally, we do a 64-bit signed integer:
//..
    bsls::Types::Int64 i64 = 0x0a0b0c0d0e0f0102LL;
    ASSERT(0x02010f0e0d0c0b0aLL == Util::swapBytes(i64));
//..
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING SINGLE BYTE OBJECTS
        //
        // Concerns:
        //: 1 That 'swapBytes', when called on single byte objects, returns the
        //:   value passed without modification.
        //
        // Plan:
        //: 1 For 'bool', just test both possible values by hand.
        //:
        //: 2 For 'char' types, use the 'singleByteTest' template function
        //:   (defined in this file) to test that 'swapBytes' is the identify
        //:   function for all possible values of the char type.
        //
        // Testing:
        //   static TYPE swapBytes(TYPE) for all single-byte 'TYPE'
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING SINGLE BYTE OBJECTS\n"
                            "===========================\n");

        ASSERT(true  == Util::swapBytes(true));
        ASSERT(false == Util::swapBytes(false));

        singleByteTest<char>();
        singleByteTest<unsigned char>();
        singleByteTest<signed char>();
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        singleByteTest<char8_t>();
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING TYPE MATCHING
        //
        // Concerns:
        //: 1 That the set of types supported by 'swapBytes' are
        //:   distinguishable when passed by value, and that none of them
        //:   evaluate to the same type (i.e., 'char' and 'signed char').
        //
        // Plan:
        //: 1 Use the template function 'isSameType' (defined in this file)
        //:   to compare types passed to it, and to compare the return value
        //:   type of calls to 'swapBytes'.
        //
        // Testing:
        //   static TYPE swapBytes(TYPE) for all integral 'TYPE'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TYPE MATCHING\n"
                              "=====================\n");

        bool b = true;
        ASSERT( isSameType(b,   b));
        ASSERT( isSameType(b,   Util::swapBytes(b)));

        char c = 'a';
        ASSERT(!isSameType(b,   Util::swapBytes(c)));
        ASSERT(!isSameType(b,   c));
        ASSERT( isSameType(c,   c));
        ASSERT( isSameType(c,   Util::swapBytes(c)));

        unsigned char uc = 'a';
        ASSERT(!isSameType(b,   Util::swapBytes(uc)));
        ASSERT(!isSameType(c,   Util::swapBytes(uc)));
        ASSERT(!isSameType(c,   uc));
        ASSERT( isSameType(uc,  uc));
        ASSERT( isSameType(uc,  Util::swapBytes(uc)));

        signed char sc = 'a';
        ASSERT(!isSameType(b,   Util::swapBytes(sc)));
        ASSERT(!isSameType(c,   Util::swapBytes(sc)));
        ASSERT(!isSameType(uc,  Util::swapBytes(sc)));
        ASSERT(!isSameType(uc,  sc));
        ASSERT( isSameType(sc,  sc));
        ASSERT( isSameType(sc,  Util::swapBytes(sc)));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        char8_t c8 = 'a';
        ASSERT(!isSameType(b,   Util::swapBytes(c8)));
        ASSERT(!isSameType(c,   Util::swapBytes(c8)));
        ASSERT(!isSameType(uc,  Util::swapBytes(c8)));
        ASSERT(!isSameType(sc,  Util::swapBytes(c8)));
        ASSERT(!isSameType(sc,  c8));
        ASSERT( isSameType(c8,  c8));
        ASSERT( isSameType(c8,  Util::swapBytes(c8)));
#endif

        wchar_t w = 'w';
        ASSERT(!isSameType(b,   Util::swapBytes(w)));
        ASSERT(!isSameType(c,   Util::swapBytes(w)));
        ASSERT(!isSameType(uc,  Util::swapBytes(w)));
        ASSERT(!isSameType(sc,  Util::swapBytes(w)));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        ASSERT(!isSameType(c8,  Util::swapBytes(w)));
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        ASSERT(!isSameType(c8,  w));
#endif
        ASSERT( isSameType(w,   w));
        ASSERT( isSameType(w,   Util::swapBytes(w)));

        short s = 12345;
        ASSERT(!isSameType(b,   Util::swapBytes(s)));
        ASSERT(!isSameType(c,   Util::swapBytes(s)));
        ASSERT(!isSameType(uc,  Util::swapBytes(s)));
        ASSERT(!isSameType(sc,  Util::swapBytes(s)));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        ASSERT(!isSameType(c8,  Util::swapBytes(s)));
#endif
        ASSERT(!isSameType(w,   Util::swapBytes(s)));
        ASSERT(!isSameType(w,   s));
        ASSERT( isSameType(s,   s));
        ASSERT( isSameType(s,   Util::swapBytes(s)));

        unsigned short us = 23456;
        ASSERT(!isSameType(b,   Util::swapBytes(us)));
        ASSERT(!isSameType(c,   Util::swapBytes(us)));
        ASSERT(!isSameType(uc,  Util::swapBytes(us)));
        ASSERT(!isSameType(sc,  Util::swapBytes(us)));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        ASSERT(!isSameType(c8,  Util::swapBytes(us)));
#endif
        ASSERT(!isSameType(w,   Util::swapBytes(us)));
        ASSERT(!isSameType(s,   Util::swapBytes(us)));
        ASSERT(!isSameType(s,   us));
        ASSERT( isSameType(us,  us));
        ASSERT( isSameType(us,  Util::swapBytes(us)));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        char16_t c6 = 12345;
        ASSERT(!isSameType(b,   Util::swapBytes(c6)));
        ASSERT(!isSameType(c,   Util::swapBytes(c6)));
        ASSERT(!isSameType(uc,  Util::swapBytes(c6)));
        ASSERT(!isSameType(sc,  Util::swapBytes(c6)));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        ASSERT(!isSameType(c8,  Util::swapBytes(c6)));
#endif
        ASSERT(!isSameType(w,   Util::swapBytes(c6)));
        ASSERT(!isSameType(s,   Util::swapBytes(c6)));
        ASSERT(!isSameType(us,  Util::swapBytes(c6)));
        ASSERT(!isSameType(us,  c6));
        ASSERT( isSameType(c6,  c6));
        ASSERT( isSameType(c6,  Util::swapBytes(c6)));

        char32_t c2 = 1234567890;
        ASSERT(!isSameType(b,   Util::swapBytes(c2)));
        ASSERT(!isSameType(c,   Util::swapBytes(c2)));
        ASSERT(!isSameType(uc,  Util::swapBytes(c2)));
        ASSERT(!isSameType(sc,  Util::swapBytes(c2)));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        ASSERT(!isSameType(c8,  Util::swapBytes(c2)));
#endif
        ASSERT(!isSameType(w,   Util::swapBytes(c2)));
        ASSERT(!isSameType(s,   Util::swapBytes(c2)));
        ASSERT(!isSameType(us,  Util::swapBytes(c2)));
        ASSERT(!isSameType(c6,  Util::swapBytes(c2)));
        ASSERT(!isSameType(c6,  c2));
        ASSERT( isSameType(c2,  c2));
        ASSERT( isSameType(c2,  Util::swapBytes(c2)));
#endif

        int ii = 1234567890;
        ASSERT(!isSameType(b,   Util::swapBytes(ii)));
        ASSERT(!isSameType(c,   Util::swapBytes(ii)));
        ASSERT(!isSameType(uc,  Util::swapBytes(ii)));
        ASSERT(!isSameType(sc,  Util::swapBytes(ii)));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        ASSERT(!isSameType(c8,  Util::swapBytes(ii)));
#endif
        ASSERT(!isSameType(w,   Util::swapBytes(ii)));
        ASSERT(!isSameType(s,   Util::swapBytes(ii)));
        ASSERT(!isSameType(us,  Util::swapBytes(ii)));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        ASSERT(!isSameType(c6,  Util::swapBytes(ii)));
        ASSERT(!isSameType(c2,  Util::swapBytes(ii)));
        ASSERT(!isSameType(c2,  ii));
#endif
        ASSERT(!isSameType(us,  ii));
        ASSERT( isSameType(ii,  ii));
        ASSERT( isSameType(ii,  Util::swapBytes(ii)));

        unsigned int ui = 2345678901U;
        ASSERT(!isSameType(b,   Util::swapBytes(ui)));
        ASSERT(!isSameType(c,   Util::swapBytes(ui)));
        ASSERT(!isSameType(uc,  Util::swapBytes(ui)));
        ASSERT(!isSameType(sc,  Util::swapBytes(ui)));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        ASSERT(!isSameType(c8,  Util::swapBytes(ui)));
#endif
        ASSERT(!isSameType(w,   Util::swapBytes(ui)));
        ASSERT(!isSameType(s,   Util::swapBytes(ui)));
        ASSERT(!isSameType(us,  Util::swapBytes(ui)));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        ASSERT(!isSameType(c6,  Util::swapBytes(ui)));
        ASSERT(!isSameType(c2,  Util::swapBytes(ui)));
        ASSERT(!isSameType(c2,  ui));
#endif
        ASSERT(!isSameType(ii,  Util::swapBytes(ui)));
        ASSERT(!isSameType(ii,  ui));
        ASSERT( isSameType(ui,  ui));
        ASSERT( isSameType(ui,  Util::swapBytes(ui)));

        long ll = 1234567891;
        ASSERT(!isSameType(b,   Util::swapBytes(ll)));
        ASSERT(!isSameType(c,   Util::swapBytes(ll)));
        ASSERT(!isSameType(uc,  Util::swapBytes(ll)));
        ASSERT(!isSameType(sc,  Util::swapBytes(ll)));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        ASSERT(!isSameType(c8,  Util::swapBytes(ll)));
#endif
        ASSERT(!isSameType(w,   Util::swapBytes(ll)));
        ASSERT(!isSameType(s,   Util::swapBytes(ll)));
        ASSERT(!isSameType(us,  Util::swapBytes(ll)));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        ASSERT(!isSameType(c6,  Util::swapBytes(ll)));
        ASSERT(!isSameType(c2,  Util::swapBytes(ll)));
        ASSERT(!isSameType(c2,  ll));
#endif
        ASSERT(!isSameType(ii,  Util::swapBytes(ll)));
        ASSERT(!isSameType(ui,  Util::swapBytes(ll)));
        ASSERT(!isSameType(ui,  ll));
        ASSERT( isSameType(ll,  ll));
        ASSERT( isSameType(ll,  Util::swapBytes(ll)));

        unsigned long ul = 2345678902U;
        ASSERT(!isSameType(b,   Util::swapBytes(ul)));
        ASSERT(!isSameType(c,   Util::swapBytes(ul)));
        ASSERT(!isSameType(uc,  Util::swapBytes(ul)));
        ASSERT(!isSameType(sc,  Util::swapBytes(ul)));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        ASSERT(!isSameType(c8,  Util::swapBytes(ul)));
#endif
        ASSERT(!isSameType(w,   Util::swapBytes(ul)));
        ASSERT(!isSameType(s,   Util::swapBytes(ul)));
        ASSERT(!isSameType(us,  Util::swapBytes(ul)));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        ASSERT(!isSameType(c6,  Util::swapBytes(ul)));
        ASSERT(!isSameType(c2,  Util::swapBytes(ul)));
        ASSERT(!isSameType(c2,  ul));
#endif
        ASSERT(!isSameType(ii,  Util::swapBytes(ul)));
        ASSERT(!isSameType(ui,  Util::swapBytes(ul)));
        ASSERT(!isSameType(ll,  Util::swapBytes(ul)));
        ASSERT(!isSameType(ll,  ul));
        ASSERT( isSameType(ul,  ul));
        ASSERT( isSameType(ul,  Util::swapBytes(ul)));

        Uint64 ull = 234567890;
        ASSERT(!isSameType(b,   Util::swapBytes(ull)));
        ASSERT(!isSameType(c,   Util::swapBytes(ull)));
        ASSERT(!isSameType(uc,  Util::swapBytes(ull)));
        ASSERT(!isSameType(sc,  Util::swapBytes(ull)));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        ASSERT(!isSameType(c8,  Util::swapBytes(ull)));
#endif
        ASSERT(!isSameType(w,   Util::swapBytes(ull)));
        ASSERT(!isSameType(s,   Util::swapBytes(ull)));
        ASSERT(!isSameType(us,  Util::swapBytes(ull)));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        ASSERT(!isSameType(c6,  Util::swapBytes(ull)));
        ASSERT(!isSameType(c2,  Util::swapBytes(ull)));
        ASSERT(!isSameType(c2,  ull));
#endif
        ASSERT(!isSameType(ii,  Util::swapBytes(ull)));
        ASSERT(!isSameType(ui,  Util::swapBytes(ull)));
        ASSERT(!isSameType(ll,  Util::swapBytes(ull)));
        ASSERT(!isSameType(ul,  Util::swapBytes(ull)));
        ASSERT(!isSameType(ul,  ull));
        ASSERT( isSameType(ull, ull));
        ASSERT( isSameType(ull, Util::swapBytes(ull)));

        Int64 sll = 123456789;
        ASSERT(!isSameType(b,   Util::swapBytes(sll)));
        ASSERT(!isSameType(c,   Util::swapBytes(sll)));
        ASSERT(!isSameType(uc,  Util::swapBytes(sll)));
        ASSERT(!isSameType(sc,  Util::swapBytes(sll)));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        ASSERT(!isSameType(c8,  Util::swapBytes(sll)));
#endif
        ASSERT(!isSameType(w,   Util::swapBytes(sll)));
        ASSERT(!isSameType(s,   Util::swapBytes(sll)));
        ASSERT(!isSameType(us,  Util::swapBytes(sll)));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        ASSERT(!isSameType(c6,  Util::swapBytes(sll)));
        ASSERT(!isSameType(c2,  Util::swapBytes(sll)));
        ASSERT(!isSameType(c2,  sll));
#endif
        ASSERT(!isSameType(ii,  Util::swapBytes(sll)));
        ASSERT(!isSameType(ui,  Util::swapBytes(sll)));
        ASSERT(!isSameType(ll,  Util::swapBytes(sll)));
        ASSERT(!isSameType(ul,  Util::swapBytes(sll)));
        ASSERT(!isSameType(ull, Util::swapBytes(sll)));
        ASSERT(!isSameType(ull, sll));
        ASSERT( isSameType(sll, sll));
        ASSERT( isSameType(sll, Util::swapBytes(sll)));
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'swapBytes', 'swapBytes{16,32,64}'
        //
        // Concerns:
        //: 1 That 'swapBytes' and 'swapBytes{16,32,64}' return the proper
        //:   values.
        //
        // Plan:
        //: o Traverse the 'data16', 'data32', and 'data64' tables.
        //:   1 Use the 'swapBytesInPlace' oracle function (defined in this to
        //:     verify that the 'd_swapped' field of the entry is reversed
        //:     version of the 'd_value' entry.
        //:
        //:   2 Call 'isSameType' to compare the types of different variables
        //:     and calls to 'swapBytes' and 'swapBytesNN', verifying that
        //:     the types match or do not as expected.
        //:
        //:   3 Call 'swapBytesNN' and 'swapBytes' on the table values and
        //:     verify the results are as expected.
        //
        // Testing:
        //   static TYPE swapBytes(TYPE) for all multi-byte integral 'TYPE'
        //   static unsigned short swapBytes16(unsigned short)
        //   static unsigned int swapBytes32(unsigned int)
        //   static bsls::Uint64 swapBytes32(bsls::Uint64)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'swapBytes', 'swapBytes{16,32,64}'\n"
                              "==========================================\n");

        bool wchar_t_tested = false;
        bool long_tested    = false;

        if (verbose) printf("Testing 16 Bit\n");
        {
            for (int ti = 0; ti < k_NUM_DATA16; ++ti) {
                const unsigned short uValue   = data16[ti].d_value;
                const unsigned short uSwapped = data16[ti].d_swapped;
                const short          iValue   = data16[ti].d_value;
                const short          iSwapped = data16[ti].d_swapped;

                if (veryVerbose) { PHEX_(uValue); PHEX(uSwapped); }

                unsigned short verifySwapped = uValue;
                swapBytesInPlace(&verifySwapped);
                ASSERT(uSwapped == verifySwapped);

                ASSERT( isSameType(uValue, Util::swapBytes16(uValue)));
                ASSERT( isSameType(uValue, Util::swapBytes16(iValue)));
                ASSERT(!isSameType(iValue, Util::swapBytes16(iValue)));

                ASSERT( isSameType(uValue, Util::swapBytes(uValue)));
                ASSERT( isSameType(iValue, Util::swapBytes(iValue)));

                ASSERT(uSwapped == Util::swapBytes16(uValue));
                ASSERT(uSwapped == Util::swapBytes16(iValue));

                ASSERT(uValue   == Util::swapBytes16(iSwapped));
                ASSERT(uValue   == Util::swapBytes16(uSwapped));

                ASSERT(uSwapped == Util::swapBytes(uValue));
                ASSERT(iSwapped == Util::swapBytes(iValue));

                ASSERT(iValue   == Util::swapBytes(iSwapped));
                ASSERT(uValue   == Util::swapBytes(uSwapped));

#if 2 == BYTEORDERUTIL_SIZEOF_WCHAR_T
                ASSERT(2 == sizeof(wchar_t));

                const wchar_t wValue   = (wchar_t) uValue;
                const wchar_t wSwapped = (wchar_t) uSwapped;

                ASSERT(wSwapped == (wchar_t) Util::swapBytes16(uValue));

                ASSERT(wSwapped == Util::swapBytes(uValue));
                ASSERT(wValue   == Util::swapBytes(uSwapped));

                ASSERT(isSameType(wValue, Util::swapBytes(wValue)));

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

                unsigned int verifySwapped = uValue;
                swapBytesInPlace(&verifySwapped);
                ASSERT(uSwapped == verifySwapped);

                ASSERT( isSameType(uValue, Util::swapBytes32(uValue)));
                ASSERT( isSameType(uValue, Util::swapBytes32(iValue)));
                ASSERT(!isSameType(iValue, Util::swapBytes32(iValue)));

                ASSERT( isSameType(uValue, Util::swapBytes(uValue)));
                ASSERT( isSameType(iValue, Util::swapBytes(iValue)));

                ASSERT(uSwapped == Util::swapBytes32(uValue));

                ASSERT(uSwapped == Util::swapBytes(uValue));
                ASSERT(iSwapped == Util::swapBytes(iValue));

                ASSERT(iValue   == Util::swapBytes(iSwapped));
                ASSERT(uValue   == Util::swapBytes(uSwapped));

#if 4 == BYTEORDERUTIL_SIZEOF_WCHAR_T
                ASSERT(4 == sizeof(wchar_t));

                const wchar_t wValue   = (wchar_t) uValue;
                const wchar_t wSwapped = (wchar_t) uSwapped;

                ASSERT(wSwapped == (wchar_t) Util::swapBytes32(uValue));

                ASSERT(wSwapped == Util::swapBytes(wValue));
                ASSERT(wValue   == Util::swapBytes(wSwapped));

                ASSERT(isSameType(wValue, Util::swapBytes(wValue)));

                wchar_t_tested = true;
#endif

#if 4 == BYTEORDERUTIL_SIZEOF_LONG
                ASSERT(4 == sizeof(long));

                const long          iLValue   = (long) uValue;
                const long          iLSwapped = (long) uSwapped;

                const unsigned long uLValue   = (unsigned long) uValue;
                const unsigned long uLSwapped = (unsigned long) uSwapped;

                LOOP_ASSERT(line, iLSwapped == Util::swapBytes(iLValue));
                LOOP_ASSERT(line, iLValue   == Util::swapBytes(iLSwapped));

                LOOP_ASSERT(line, uLSwapped == Util::swapBytes(uLValue));
                LOOP_ASSERT(line, uLValue   == Util::swapBytes(uLSwapped));

                LOOP_ASSERT(line, isSameType(iLValue,
                                             Util::swapBytes(iLValue)));
                LOOP_ASSERT(line, isSameType(uLValue,
                                             Util::swapBytes(uLValue)));

                long_tested = true;
#else
                (void) line;
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

                Uint64 verifySwapped = uValue;
                swapBytesInPlace(&verifySwapped);
                ASSERT(uSwapped == verifySwapped);

                ASSERT( isSameType(uValue, Util::swapBytes64(uValue)));
                ASSERT( isSameType(uValue, Util::swapBytes64(iValue)));
                ASSERT(!isSameType(iValue, Util::swapBytes64(iValue)));

                ASSERT( isSameType(uValue, Util::swapBytes(uValue)));
                ASSERT( isSameType(iValue, Util::swapBytes(iValue)));

                ASSERT(uSwapped == Util::swapBytes64(uValue));
                ASSERT(uSwapped == Util::swapBytes64(iValue));

                ASSERT(uValue   == Util::swapBytes64(iSwapped));
                ASSERT(uValue   == Util::swapBytes64(uSwapped));

                ASSERT(uSwapped == Util::swapBytes(uValue));
                ASSERT(iSwapped == Util::swapBytes(iValue));

                ASSERT(iValue   == Util::swapBytes(iSwapped));
                ASSERT(uValue   == Util::swapBytes(uSwapped));

#if 8 == BYTEORDERUTIL_SIZEOF_LONG
                ASSERT(8 == sizeof(long));

                const long          iLValue   = (long) uValue;
                const long          iLSwapped = (long) uSwapped;

                const unsigned long uLValue   = (unsigned long) uValue;
                const unsigned long uLSwapped = (unsigned long) uSwapped;

                LOOP_ASSERT(line, iLSwapped == Util::swapBytes(iLValue));
                LOOP_ASSERT(line, iLValue   == Util::swapBytes(iLSwapped));

                LOOP_ASSERT(line, uLSwapped == Util::swapBytes(uLValue));
                LOOP_ASSERT(line, uLValue   == Util::swapBytes(uLSwapped));

                LOOP_ASSERT(line, isSameType(iLValue,
                                             Util::swapBytes(iLValue)));
                LOOP_ASSERT(line, isSameType(uLValue,
                                             Util::swapBytes(uLValue)));

                long_tested = true;
#else
                (void) line;
#endif
            }
        }

        ASSERT(long_tested);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 Test the swapping functions for all multibyte swap functions in
        //:   this component.
        //
        // Plan:
        //   The tests are table driven, based on tables 'data16', 'data32',
        //   and 'data64' declared outside 'main', that supply test values of
        //   different word widths and their swapped values.
        //
        //: 1 Test 16 bit swaps.
        //:   A Test 'swapBytes16'
        //:   B Test 'swapBytes' on 'short' and 'unsigned short'
        //:   C If '2 == sizeof(wchar_t)', test 'swapBytes(wchar_t)'
        //:
        //: 2 Test 32 bit swaps.
        //:   A Test 'swapBytes32'
        //:   B Test 'swapBytes' on 'int' and 'unsigned int'
        //:   C If '4 == sizeof(wchar_t)', test 'swapBytes(wchar_t)'
        //:   D If '4 == sizeof(long)', test 'swapBytes(long)'
        //:
        //: 3 Test 64 bit swaps.
        //:   A Test 'swapBytes64'
        //:   B Test 'swapBytes' on 'Types::Int64' and 'Types::Uint64'
        //:   C If '8 == sizeof(long)', test 'swapBytes(long)'
        //:
        //: 4 Check 'bool's set when testing 'wchar_t' and 'long' to make
        //:   sure the #ifdef logic resulted in both types being tested.
        //
        // Testing:
        //   static TYPE swapBytes(TYPE) for all multi-byte integral 'TYPE'
        //   static unsigned short swapBytes16(unsigned short)
        //   static unsigned int swapBytes32(unsigned int)
        //   static bsls::Uint64 swapBytes32(bsls::Uint64)
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST\n"
                              "==============\n");

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

                LOOP_ASSERT(line, uSwapped == Util::swapBytes16(uValue));
                LOOP_ASSERT(line, static_cast<unsigned short>(iSwapped) ==
                                                    Util::swapBytes16(iValue));
#if 2 == BYTEORDERUTIL_SIZEOF_WCHAR_T
                ASSERT(2 == sizeof(wchar_t));

                const wchar_t wValue   = (wchar_t) uValue;
                const wchar_t wSwapped = (wchar_t) uSwapped;

                LOOP_ASSERT(line, wSwapped == Util::swapBytes16(wValue));

                LOOP_ASSERT(line, wSwapped == Util::swapBytes(wValue));
                LOOP_ASSERT(line, wValue   == Util::swapBytes(wSwapped));

                LOOP_ASSERT(line, isSameType(wValue,
                                             Util::swapBytes(wValue)));
                LOOP_ASSERT(line, !isSameType(wValue,
                                              Util::swapBytes16(wValue)));

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

                LOOP_ASSERT(line, uSwapped == Util::swapBytes32(uValue));
                LOOP_ASSERT(line, iSwapped == (int) Util::swapBytes32(iValue));

#if 4 == BYTEORDERUTIL_SIZEOF_WCHAR_T
                ASSERT(4 == sizeof(wchar_t));

                const wchar_t wValue   = (wchar_t) uValue;
                const wchar_t wSwapped = (wchar_t) uSwapped;

                LOOP_ASSERT(line, wSwapped ==
                                          (wchar_t) Util::swapBytes32(wValue));

                LOOP_ASSERT(line, wSwapped == Util::swapBytes(wValue));
                LOOP_ASSERT(line, wValue   == Util::swapBytes(wSwapped));

                LOOP_ASSERT(line, isSameType(wValue,
                                             Util::swapBytes(wValue)));
                LOOP_ASSERT(line, !isSameType(wValue,
                                              Util::swapBytes32(wValue)));

                wchar_t_tested = true;
#endif

#if 4 == BYTEORDERUTIL_SIZEOF_LONG
                ASSERT(4 == sizeof(long));

                const long          iLValue   = (long) uValue;
                const long          iLSwapped = (long) uSwapped;

                const unsigned long uLValue   = (unsigned long) uValue;
                const unsigned long uLSwapped = (unsigned long) uSwapped;

                LOOP_ASSERT(line, iLSwapped == Util::swapBytes(iLValue));
                LOOP_ASSERT(line, iLValue   == Util::swapBytes(iLSwapped));

                LOOP_ASSERT(line, uLSwapped == Util::swapBytes(uLValue));
                LOOP_ASSERT(line, uLValue   == Util::swapBytes(uLSwapped));

                LOOP_ASSERT(line, isSameType(iLValue,
                                             Util::swapBytes(iLValue)));
                LOOP_ASSERT(line, isSameType(uLValue,
                                             Util::swapBytes(uLValue)));

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

                LOOP_ASSERT(line, uSwapped == Util::swapBytes64(uValue));
                LOOP_ASSERT(line, iSwapped ==
                                            (Int64) Util::swapBytes64(iValue));

                LOOP_ASSERT(line, uSwapped == Util::swapBytes(uValue));
                LOOP_ASSERT(line, iSwapped == Util::swapBytes(iValue));

#if 8 == BYTEORDERUTIL_SIZEOF_LONG
                ASSERT(8 == sizeof(long));

                const long          iLValue   = (long) uValue;
                const long          iLSwapped = (long) uSwapped;

                const unsigned long uLValue   = (unsigned long) uValue;
                const unsigned long uLSwapped = (unsigned long) uSwapped;

                LOOP_ASSERT(line, iLSwapped == Util::swapBytes(iLValue));
                LOOP_ASSERT(line, iLValue   == Util::swapBytes(iLSwapped));

                LOOP_ASSERT(line, uLSwapped == Util::swapBytes(uLValue));
                LOOP_ASSERT(line, uLValue   == Util::swapBytes(uLSwapped));

                LOOP_ASSERT(line, isSameType(iLValue,
                                             Util::swapBytes(iLValue)));
                LOOP_ASSERT(line, isSameType(uLValue,
                                             Util::swapBytes(uLValue)));

                long_tested = true;
#endif
            }
        }

        ASSERT(long_tested);
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE SPEED TRIALS
        //
        // Concerns:
        //   Evaluate the performance of the swap functions.
        //
        // Plan:
        //   Repeatedly evaluate the 'swapBytes' function, and use 'StopWatch'
        //   to evaluate how quickly they run.
        // --------------------------------------------------------------------

        if (verbose) std::printf("\nPERFORMANCE SPEED TRIALS\n"
                                   "========================\n");

        unsigned int intTotal = 0;
        bsls::Stopwatch sw;

        // Dummy loops

        Uint64 startIteration = (1 << 24);

        while (true) {
            P(startIteration);

            sw.reset();
            sw.start(true);
            unsigned short shortSrc = 0;
            for (Uint64 ti = startIteration; ti > 0; --ti, --shortSrc) {
                intTotal += shortSrc ^ 0x1234;
            }
            sw.stop();

            if (sw.accumulatedUserTime() >= 2.0) {
                break;
            }

            startIteration <<= 1;
            BSLS_ASSERT_OPT(0 != startIteration);
        }

        double dummy16Time = sw.accumulatedUserTime();

        ASSERT(dummy16Time > 0);

        sw.reset();
        sw.start(true);
        unsigned short shortSrc = 0;
        for (Uint64 ti = startIteration; ti > 0; --ti, --shortSrc) {
            intTotal += Util::swapBytes(shortSrc) ^ 0x1234;
        }
        sw.stop();

        double shortTime = sw.accumulatedUserTime() - dummy16Time;

        LOOP_ASSERT(shortTime, shortTime > 0);

        sw.reset();
        sw.start(true);
        shortSrc = -1;
        for (Uint64 ti = startIteration; ti > 0; --ti, --shortSrc) {
            intTotal += myGenericSwap16(shortSrc) ^ 0x1234;
        }
        sw.stop();

        double genericShortTime = sw.accumulatedUserTime() - dummy16Time;

        LOOP_ASSERT(genericShortTime, genericShortTime > 0);
        genericShortTime = std::max(genericShortTime, 1e-20);
        printf("16: custom/generic: %g\n", shortTime / genericShortTime);
        P_(shortTime); P(genericShortTime);

        sw.reset();
        sw.start(true);
        unsigned int intSrc = 0;
        for (Uint64 ti = startIteration; ti > 0; --ti, --intSrc) {
            intTotal += intSrc ^ 0x1234;
        }
        sw.stop();

        double dummy32Time = sw.accumulatedUserTime();

        ASSERT(dummy32Time > 0);

        sw.reset();
        sw.start(true);
        intSrc = 0;
        for (Uint64 ti = startIteration; ti > 0; --ti, --intSrc) {
            intTotal += Util::swapBytes(intSrc) ^ 0x1234;
        }
        sw.stop();

        double intTime = sw.accumulatedUserTime() - dummy32Time;
        LOOP_ASSERT(intTime, intTime > 0);

        sw.reset();
        sw.start(true);
        intSrc = 0;
        for (Uint64 ti = startIteration; ti > 0; --ti, --intSrc) {
            intTotal += myGenericSwap32(intSrc) ^ 0x1234;
        }
        sw.stop();

        double genericIntTime = sw.accumulatedUserTime() - dummy32Time;

        LOOP_ASSERT(genericIntTime, genericIntTime > 0);
        genericIntTime = std::max(genericIntTime, 1e-20);
        printf("32: custom/generic: %g\n", intTime / genericIntTime);
        P_(intTime); P(genericIntTime);

        Uint64 int64Total = intTotal;

        sw.reset();
        sw.start(true);
        for (Uint64 ti = startIteration; ti > 0; --ti) {
            int64Total += ti ^ 0x1234;
        }
        sw.stop();

        double dummy64Time = sw.accumulatedUserTime();

        LOOP_ASSERT(sw.accumulatedUserTime(), dummy64Time > 0);

        sw.reset();
        sw.start(true);
        for (Uint64 ti = startIteration; ti > 0; --ti) {
            int64Total += Util::swapBytes(ti) ^ 0x1234;
        }
        sw.stop();

        double int64Time = sw.accumulatedUserTime() - dummy64Time;

        LOOP_ASSERT(int64Time, int64Time > 0);

        sw.reset();
        sw.start(true);
        for (Uint64 ti = startIteration; ti > 0; --ti) {
            int64Total += myGenericSwap64(ti) ^ 0x1234;
        }
        sw.stop();

        double genericInt64Time = sw.accumulatedUserTime() - dummy64Time;

        LOOP_ASSERT(genericInt64Time, genericInt64Time > 0);
        genericInt64Time = std::max(genericInt64Time, 1e-20);
        printf("64: custom/generic: %g\n", int64Time / genericInt64Time);
        P_(int64Time); P(genericInt64Time);

        // Output 'int64Total' to make sure it's observed and prevent
        // optimizers from optimizing loops out of existence.

        P(int64Total);
      } break;
      default: {
        std::fprintf(stderr, "WARNING: CASE '$d' NOT FOUND.\n");
        testStatus = -1;
      } break;
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
