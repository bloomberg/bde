// bslstl_bitset.t.cpp                                                -*-C++-*-
#include <bslstl_bitset.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bsls_cpp11.h>
#include <bsls_bsltestutil.h>
#include <bsls_types.h>

#include <cstdlib>
#include <iostream>

#include <cmath>  // native_std::sqrt

#include <stdio.h>
#include <stdlib.h>      // 'atoi'
#include <string.h>      // 'strcmp', 'strcpy'

using namespace BloombergLP;
using namespace std;    // still using iostream

// Pragmas to disable noisiest bde_verify warnings

// BDE_VERIFY pragma:-IND01 // Test blocks not indenting after if (printf(""))
// BDE_VERIFY pragma:-IND04 // Many violations of aligned variable declarations
// BDE_VERIFY pragma:-TP03  // Many functions not yet tested  *NEEDS ATTENTION*
//
// BDE_VERIFY pragma:-MN01  // public data in test tables; cannot be avoided
// BDE_VERIFY pragma:-MN04  // pointer values in test tables
//=============================================================================
//                                 TEST  PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test implements a single (value-semantic) class.  The
// Primary Manipulators are 'operator[]' and 'bitset(bsl::string), and the
// Basic Accessors are 'operator[] const' and 'size'.
//
// Primary Manipulators:
//: o 'operator[]'
//
// Basic Accessors:
//: o 'operator[]'
//: o 'size'
//
// This particular attribute class also provides a value constructor capable of
// creating an object in any state relevant for thorough testing, obviating the
// primitive generator function, 'gg', normally used for this purpose.  We will
// therefore follow our standard 10-case approach to testing value-semantic
// types except that we will test the value constructor in case 3 (in lieu of
// the generator function), with the default constructor and primary
// manipulators tested fully in case 2.
//
// Certain standard value-semantic-type test cases are omitted:
//: o [10] -- BSLX streaming is not (yet) implemented for this class.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o ACCESSOR methods are 'const' thread-safe.
//
// ----------------------------------------------------------------------------
// CREATORS:
// [ 2] constexpr bitset() noexcept;
// [ 3] constexpr bitset(unsigned long) noexcept;
// [ 4] bitset(native_std::basic_string, size_type, size_type, char, char);
// [ 4] bitset(bsl::basic_string, size_type, size_type, char, char);
// [ 2] ~bitset();
//
// MANIPULATORS:
// [ 2] reference operator[](std::size_t pos);
// [  ] bitset& operator&=(const bitset &lhs);
// [  ] bitset& operator|=(const bitset &lhs);
// [  ] bitset& operator^=(const bitset &lhs);
// [11] bitset& operator<<=(std::size_t pos);
// [11] bitset& operator>>=(std::size_t pos);
// [  ] bitset& flip() noexcept;
// [  ] bitset& flip(std::size_t pos)
// [  ] bitset& reset() noexcept;
// [  ] bitset& reset(std::size_t pos)
// [  ] bitset& set() noexcept;
// [  ] bitset& set(std::size_t pos, int val = true)
//
// ACCESSORS:
// [  ] bitset operator<<(std::size_t pos) const
// [  ] bitset operator>>(std::size_t pos) const
// [  ] bitset operator~() const
// [ 4] bsl::string to_string(char, char) const;
// [ 3] constexpr bool operator[](std::size_t pos) const;
// [  ] bool operator==(std::size_t pos) const noexcept;
// [  ] bool operator!=(std::size_t pos) const noexcept;
// [ 2] bool all() const noexcept;
// [ 2] bool any() const noexcept;
// [ 2] bool none() const noexcept;
// [ 2] constexpr std::size_t size() const noexcept;
// [ 2] std::size_t count() const noexcept;
// [  ] bool test(std::size_t) const;
// [  ] unsigned long to_ulong() const noexcept;
//
//
// FREE OPERATORS:
// [  ] bitset<N> operator|(const bitset<N> &lhs, const bitset<N> &rhs);
// [  ] bitset<N> operator&(const bitset<N> &lhs, const bitset<N> &rhs);
// [  ] bitset<N> operator^(const bitset<N> &lhs, const bitset<N> &rhs);
// [  ] operator>>(std::istream &is, bitset<N>& x);
// [  ] operator<<(std::ostream &os, const bitset<N>& x);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE
// [13] CONCERN: Methods qualifed 'noexcept' in standard are so implemented.
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

//=============================================================================
//              PLATFORM DEFECT TRACKING MACROS TO SUPPORT TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_SUN)
    // Last tested with the 12.3 compiler chain, and applies only to the
    // (default) Rogue Wave library
# define BSLSTL_BITSET_NO_REBIND_IN_NATIVE_ALLOCATOR
#endif
//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
namespace bsl {
template <size_t N>
void debugprint(const bitset<N>& rhs) {
    size_t i = N;
    while(i) {
        if (rhs[--i]) {
            bsls::debugprint('1');
        }
        else {
            bsls::debugprint('0');
        }
    }
}
}

template <class StringType>
void changeBitString(StringType* str, char zeroChar, char oneChar)
{
    StringType& strRef = *str;
    for (size_t i = 0; i < strRef.size(); ++i) {
        char newChar = strRef[i] == '0' ? zeroChar : oneChar;
        strRef[i] = newChar;
    }
}

template <size_t N>
bool verifyBitset(const bsl::bitset<N> obj, const char *expected)
    // Return true if the specified bitset 'obj' has the bit pattern described
    // by the first 'N' characters of the specified 'expected' string, where
    // the 'char' value '0' corresponds to an unset bit, and the char value '1'
    // corresponds to a set bit, and return false otherwise.  The behavior is
    // undefined unless 'expected' has at least 'N' characters.
{
    for (unsigned int i = 0; i < N; ++i) {
        ASSERT(expected[i] == '1' || expected[i] == '0');
        if ((expected[i] == '1') != obj[N - i - 1]) {
            return false;                                             // RETURN
        }
    }
    return true;
}

template <size_t N>
bool verifyBitset(const bsl::bitset<N> obj,
                  size_t               expected,
                  bool                 verbose)
{
    for (size_t bitIndex = 0; bitIndex < N; ++bitIndex) {
        size_t expectedBit = 0;

        if (bitIndex < sizeof(unsigned long) * CHAR_BIT) {
            expectedBit = ((expected >> bitIndex) & 1);
        }

        if (static_cast<bool>(expectedBit) != obj[bitIndex]) {
            ASSERTV(bitIndex, expectedBit, obj[bitIndex],
                    static_cast<bool>(expectedBit) == obj[bitIndex]);

            return false;                                             // RETURN
        }
    }

    return true;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)

template <size_t N>
constexpr bool verifyBitConstexpr(size_t                bitIndex,
                                  const bsl::bitset<N>& obj,
                                  unsigned long         expected)
{
    return bitIndex < sizeof(unsigned long) * CHAR_BIT
        ? (((expected >> bitIndex) & 1)) == obj[bitIndex]
        : 0 == obj[bitIndex];
}

template <size_t BIT_INDEX, size_t N>
struct VerifyBitsetHelper {
    constexpr VerifyBitsetHelper() {}

    constexpr
    bool operator()(const bsl::bitset<N>& obj, unsigned long expected) const
    {
        return verifyBitConstexpr(BIT_INDEX, obj, expected) &&
            VerifyBitsetHelper<BIT_INDEX - 1, N>()(obj, expected);
    }
};

template <size_t N>
struct VerifyBitsetHelper<0, N> {
    constexpr VerifyBitsetHelper() {}

    constexpr
    bool operator()(const bsl::bitset<N>& obj, unsigned long expected) const
    {
        return verifyBitConstexpr(0, obj, expected);
    }
};

template <size_t N>
constexpr
bool verifyBitsetConstexpr(const bsl::bitset<N>& obj, unsigned long value)
{
    return VerifyBitsetHelper<N - 1, N>()(obj, value);
}

#endif


//=============================================================================
//                      TEST CASE SUPPORT FUNCTIONS
//-----------------------------------------------------------------------------

namespace {

template <size_t TESTSIZE>
void testCase2(bool verbose, bool veryVerbose, bool veryVeryVerbose)
{
    (void)veryVerbose;
    (void)veryVeryVerbose;

    if (verbose) printf("\tCheck bitset<" ZU ">\n", TESTSIZE);
    {   // Runtime test
        bsl::bitset<TESTSIZE> v;

        ASSERT(TESTSIZE == v.size());

        ASSERT(0 == v.count());

        ASSERT(v.none());
        ASSERT(!v.any());
        ASSERT(0 == TESTSIZE || !v.all());

        if (0 == TESTSIZE) {
            return;
        }

        v[0] = 1;

        ASSERT(1 == v.count());
        ASSERT(0 == TESTSIZE || !v.none());
        ASSERT(v.any());
        ASSERT(0 == TESTSIZE || TESTSIZE > 1 || v.all());

        v[0] = 0;

        ASSERT(0 == v.count());
        ASSERT(v.none());
        ASSERT(!v.any());
        ASSERT(0 == TESTSIZE || !v.all());

        v[TESTSIZE - 1] = 1;

        ASSERT(1 == v.count());
        ASSERT(!v.none());
        ASSERT(v.any());
        ASSERT(0 == TESTSIZE || TESTSIZE > 1 || v.all());

        v[TESTSIZE - 1] = 0;

        ASSERT(0 == v.count());
        ASSERT(v.none());
        ASSERT(!v.any());
        ASSERT(0 == TESTSIZE || !v.all());

        for (int i = 0; i < TESTSIZE; ++i) {
            v[i] = 1;
            ASSERT(i + 1 == v.count());
        }

        ASSERT(v.all());
        ASSERT(TESTSIZE == v.count());

        for (int i = 1; i < TESTSIZE; ++i) {
            LOOP2_ASSERT(i, TESTSIZE, v.all());
            v[i] = 0;
            LOOP2_ASSERT(i, TESTSIZE, !v.all());
            v[i] = 1;
            LOOP2_ASSERT(i, TESTSIZE, v.all());
        }
    }
#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
    {   // constexpr test
        constexpr bsl::bitset<TESTSIZE> v;
        ASSERT(TESTSIZE == v.size());
        ASSERT(v.none());
        ASSERT(!v.any());
        BSLMF_ASSERT(TESTSIZE == v.size());
        BSLMF_ASSERT(0 == TESTSIZE || 0 == v[0]);
    }
#endif
}

template <unsigned int LINE, size_t TESTSIZE, unsigned long VALUE>
void testCase3Imp(bool verbose, bool veryVerbose)
{
    typedef bsl::bitset<TESTSIZE> Obj;
    if (veryVerbose) {
        T_ P_(TESTSIZE) P_(LINE) P(VALUE);
    }

    Obj mX(VALUE);
    ASSERT(verifyBitset(mX, VALUE, verbose));

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
    BSLS_CPP11_CONSTEXPR Obj mX2(VALUE);
    ASSERT(verifyBitset(mX2, VALUE, verbose));
    BSLMF_ASSERT(mX2.size() == TESTSIZE);
    BSLMF_ASSERT(verifyBitsetConstexpr(mX2, VALUE));
#endif
};

template <size_t TESTSIZE>
void testCase3(bool verbose, bool veryVerbose, bool /* veryVeryVerbose */)
{
    if (verbose) printf("Testing bitset<" ZU ">(unsigned long) constructor\n",
                        TESTSIZE);
                          //LINE  VALUE
                          //----  -------------------
    testCase3Imp<L_, TESTSIZE,   0                  >(verbose, veryVerbose);
    testCase3Imp<L_, TESTSIZE,   0x10101010         >(verbose, veryVerbose);
    testCase3Imp<L_, TESTSIZE,   0xabcdef01         >(verbose, veryVerbose);
    testCase3Imp<L_, TESTSIZE,   0x12345678         >(verbose, veryVerbose);
    testCase3Imp<L_, TESTSIZE,   0xffffffff         >(verbose, veryVerbose);
    testCase3Imp<L_, TESTSIZE,   0x87654321         >(verbose, veryVerbose);
#if defined(BSLS_PLATFORM_CPU_64_BIT) && !defined(BSLS_PLATFORM_OS_WINDOWS)
    testCase3Imp<L_, TESTSIZE,   0x1010101010101010 >(verbose, veryVerbose);
    testCase3Imp<L_, TESTSIZE,   0xabcdef01abcdef01 >(verbose, veryVerbose);
    testCase3Imp<L_, TESTSIZE,   0x1234567812345678 >(verbose, veryVerbose);
    testCase3Imp<L_, TESTSIZE,   0xffffffffffffffff >(verbose, veryVerbose);
    testCase3Imp<L_, TESTSIZE,   0x8765432187654321 >(verbose, veryVerbose);
#endif

}

void testCase13()
{
    // ------------------------------------------------------------------------
    // 'noexcept' SPECIFICATION
    //
    // Concerns:
    //: 1 The 'noexcept' specification has been applied to all class interfaces
    //:   required by the standard.
    //
    // Plan:
    //: 1 Apply the uniary 'noexcept' operator to expressions that mimic those
    //:   appearing in the standard and confirm that calculated boolean value
    //:   matches the expected value.
    //:
    //: 2 Since the 'noexcept' specification does not vary with the 'TYPE'
    //:   of the container, we need test for just one general type and any
    //:   'TYPE' specializations.
    //
    // Testing:
    //   CONCERN: Methods qualifed 'noexcept' in standard are so implemented.
    // ------------------------------------------------------------------------

    // N4594: 20.8 Class template bitset

    // page 556: 20.8.4 bitset operators
    //..
    //  template <size_t N>
    //          bitset<N> operator&(const bitset<N>&, const bitset<N>&)
    //                                                                noexcept;
    //  template <size_t N>
    //          bitset<N> operator|(const bitset<N>&, const bitset<N>&)
    //                                                                noexcept;
    //  template <size_t N>
    //          bitset<N> operator^(const bitset<N>&, const bitset<N>&)
    //                                                                noexcept;
    //..
    {
        bsl::bitset<32> lhs;
        bsl::bitset<32> rhs;

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(lhs & rhs));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(lhs | rhs));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(lhs ^ rhs));
    }

    // page 557
    //..
    //  class reference {
    //    friend class bitset;
    //    reference() noexcept;
    //  public:
    //    ~reference() noexcept;
    //    reference& operator=(bool x) noexcept;           // for b[i] = x;
    //    reference& operator=(const reference&) noexcept; // for b[i] = b[j];
    //    bool operator~() const noexcept;                 // flips the bit
    //    operator bool() const noexcept;                  // for x = b[i];
    //    reference& flip() noexcept;                      // for b[i].flip();
    //  }
    //..
    {
        bsl::bitset<32> b;
        size_t          i = 0;
        size_t          j = 0;
        bool            x = true;

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b[i]));
                                                     // 'reference()' (private)

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(~b[i]));
                                                              // '~reference()'

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b[i] = x   ));
                                                           // 'operator=(bool)'

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b[i] = b[j]));
                                               // 'operator=(const reference&)'

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(~b));       // 'operator~'
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(x = b[i])); // 'operator()'
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b[i].flip())); // 'flip()'
    }

    // page 557: 20.8.1 constructors:
    //..
    //  constexpr bitset() noexcept;
    //  constexpr bitset(unsigned long long val) noexcept;
    //..
    {
        unsigned long long val = 1;

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(bsl::bitset<32>()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(bsl::bitset<32>(val)));
    }

    // page 557: 20.8.2 bitset operations:
    //..
    //  bitset<N>& operator&=(const bitset<N>& rhs) noexcept;
    //  bitset<N>& operator|=(const bitset<N>& rhs) noexcept;
    //  bitset<N>& operator^=(const bitset<N>& rhs) noexcept;
    //  bitset<N>& operator<<=(size_t pos) noexcept;
    //  bitset<N>& operator>>=(size_t pos) noexcept;
    //  bitset<N>& set() noexcept;
    //  bitset<N>& reset() noexcept;
    //  bitset<N> operator~() const noexcept;
    //  bitset<N>& flip() noexcept;
    //..
    {
        bsl::bitset<32> b;
        bsl::bitset<32> rhs;
        size_t          pos;

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b.operator&=(rhs)));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b.operator|=(rhs)));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b.operator^=(rhs)));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b.operator<<=(pos)));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b.operator>>=(pos)));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b.set()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b.reset()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b.operator~()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b.flip()));
    }

    // page 557-558: element access:
    //..
    // size_t count() const noexcept;
    // constexpr size_t size() const noexcept;
    //
    // bool operator==(const bitset<N>& rhs) const noexcept;
    // bool operator!=(const bitset<N>& rhs) const noexcept;
    // bool all() const noexcept;
    // bool any() const noexcept;
    // bool none() const noexcept;
    // bitset<N> operator<<(size_t pos) const noexcept;
    // bitset<N> operator>>(size_t pos) const noexcept;
    //..
    {
        bsl::bitset<32> b;
        bsl::bitset<32> rhs;
        size_t          pos;

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b.count()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b.size()));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b.operator==(rhs)));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b.operator!=(rhs)));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b.all()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b.any()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b.none()));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b.operator<<(pos)));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(b.operator>>(pos)));
    }
}

}  // close unnamed namespace

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Example 1: Determining if a number is prime (Sieve of Eratosthenes)
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to write a function to determine whether or not a given
// number is prime.  One way to implement this function is by using what's
// called the Sieve of Eratosthenes.  The basic idea of this algorithm is to
// repeatedly walk the sequence of integer values and mark any numbers up to
// and including the particular value of interest that are integer multiples of
// first 2, then 3, then 5 etc., (skipping 4 because it was previously marked
// when we walked the sequence by 2's).  Once we have walked the sequence with
// all primes up to and including the square root of the number of interest, we
// check to see if that number has been marked: If it has, it's composite;
// otherwise it's prime.
//
// When implementing this classic algorithm, we need an efficient way of
// representing a flag for each potential prime number.  The following
// illustrates how we can use 'bsl::bitset' to accomplish this result, provided
// we know an upper bound on supplied candidate values at compile time.
//
// First, we begin to define a function template that will determine whether or
// not a given candidate value is prime:
//..
template <unsigned int MAX_VALUE>
bool isPrime(int candidate)
    // Return 'true' if the specified 'candidate' value is a prime number, and
    // 'false' otherwise.  The behavior is undefined unless
    // '2 <= candidate <= MAX_VALUE'
{
    BSLMF_ASSERT(2 <= MAX_VALUE);
    BSLS_ASSERT(2 <= candidate);
    BSLS_ASSERT((unsigned int) candidate <= MAX_VALUE);
//..
// Then, we declare a 'bsl::bitset', 'compositeFlags', that will contain flags
// indicating whether a value corresponding to a given index is known to be
// composite ('true') or is still potentially prime ('false') up to and
// including the compile-time constant template parameter, 'MAX_VALUE'.
//..
    // Candidate primes in the '[2 .. MAX_VALUE]' range.

    bsl::bitset<MAX_VALUE + 1> compositeFlags;
//..
// Next, we observe that a default-constructed 'bsl::bitset' has no flags set,
// which we can verify by asserting that the 'none' method returns true, by
// asserting that the 'any' method returns false, or by asserting that the
// 'count' of set bits is 0:
//..
    ASSERT(true  == compositeFlags.none());
    ASSERT(false == compositeFlags.any());
    ASSERT(0     == compositeFlags.count());
//..
// Then, we note that a 'bsl::bitset' has a fixed 'size' (the set can't be
// grown or shrunk) and verify that 'size' is the same as the template argument
// used to create the 'bsl::bitset':
//..
   ASSERT(MAX_VALUE + 1 == compositeFlags.size());
//..
// Next, we compute 'sqrt(candidate)', which is as far as we need to look:
//..
    // We need to cast the 'sqrt' argument to avoid an overload ambiguity.
    const int sqrtOfCandidate = std::sqrt(static_cast<double>(candidate))
                                + 0.01;  // fudge factor
//..
// Now, we loop from 2 to 'sqrtOfCandidate', and use the sieve algorithm to
// eliminate non-primes:
//..
    // Note that we treat 'false' values as potential primes, since that is how
    // 'bsl::bitset' is default-initialized.

    for (int i = 2; i <= sqrtOfCandidate; ++i) {
        if (compositeFlags[i]) {
            continue;  // Skip this value: it's flagged as composite, so all
                       // of its multiples are already flagged as composite as
                       // well.
        }

        for (int flagValue = i;
             flagValue <= candidate;
             flagValue += i) {
            compositeFlags[flagValue] = true;
        }

        if (true == compositeFlags[candidate]) {
            return false;                                             // RETURN
        }
    }

    BSLS_ASSERT(false == compositeFlags[candidate]);

    return true;
}
//..
// Notice that if we don't return 'false' from the loop, none of the lower
// numbers evenly divided the candidate value; hence, it is a prime number.


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());


    bslma::TestAllocator ta("general", veryVeryVeryVerbose);

    bsls::Types::Int64 numDeallocations;
    bsls::Types::Int64 numAllocations;
    bsls::Types::Int64 numDeletes = 0;

    bsls::Types::Int64 numDefaultDeallocations =
                                           defaultAllocator.numDeallocations();
    bsls::Types::Int64 numDefaultAllocations =
                                             defaultAllocator.numAllocations();
    switch (test) { case 0:  // Zero is always the leading case.
      case 34: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "'noexcept' SPECIFICATION" "\n"
                                 "========================" "\n");

        testCase13();

      } break;
    case 12: {
      // --------------------------------------------------------------------
      // USAGE EXAMPLE TEST
      //
      // Finally, we can exercise our 'isPrime' function:
      // --------------------------------------------------------------------

      // Finally, we can exercise our 'isPrime' function with an upper bound
      // of 10,000:
      //..
      enum { UPPER_BOUND = 10000 };

      ASSERT(1 == isPrime<UPPER_BOUND>(2));
      ASSERT(1 == isPrime<UPPER_BOUND>(3));
      ASSERT(0 == isPrime<UPPER_BOUND>(4));
      ASSERT(1 == isPrime<UPPER_BOUND>(5));
      ASSERT(0 == isPrime<UPPER_BOUND>(6));
      ASSERT(1 == isPrime<UPPER_BOUND>(7));
      ASSERT(0 == isPrime<UPPER_BOUND>(8));
      ASSERT(0 == isPrime<UPPER_BOUND>(9));
      ASSERT(0 == isPrime<UPPER_BOUND>(10));
      // ...
      ASSERT(1 == isPrime<UPPER_BOUND>(9973));
      ASSERT(0 == isPrime<UPPER_BOUND>(9975));
      ASSERT(0 == isPrime<UPPER_BOUND>(10000));
      //..
    } break;

    case 11: {
      // --------------------------------------------------------------------
      // TESTING SHIFT OPERATORS
      //
      // Concerns:
      //   1. That a bitset can be shifted across word boundaries.
      //
      //   2. That a bitset get filled by 0s for the most significant 'pos'
      //      bits if shifted right, or the least significant 'pos' bits if
      //      shifted left.
      //
      // Plan:
      //   Using the table-driven technique, construct a bitset.  Then shift
      //   the bitset and verify the value is as expected.
      //
      // Testing:
      //   bitset& operator<<=(std::size_t pos);
      //   bitset& operator>>=(std::size_t pos);
      // --------------------------------------------------------------------

      if (verbose) printf("\nTESTING SHIFT OPERATORS"
                          "\n=======================\n");

      {

      static const struct {
          unsigned int d_lineNum;    // source line number
          const char*  d_string;     // bitset string
          size_t       d_pos;        // amount to shift by
          const char*  d_shiftlexp;  // expected string after left shift
          const char*  d_shiftrexp;  // expected string after right shift
      } DATA[] = {
   //LINE  VALUE                POS SHIFTLEXP            SHIFTREXP
   //----  -------------------  --- -------------------  ------------------
   { L_,   "00000000000000000", 0,  "00000000000000000", "00000000000000000"},
   { L_,   "00000000000000000", 1,  "00000000000000000", "00000000000000000"},
   { L_,   "00000000000000000", 8,  "00000000000000000", "00000000000000000"},
   { L_,   "00000000000000000", 15, "00000000000000000", "00000000000000000"},
   { L_,   "00000000000000000", 16, "00000000000000000", "00000000000000000"},
   { L_,   "00000000000000000", 17, "00000000000000000", "00000000000000000"},

   { L_,   "11111111111111111", 0,  "11111111111111111", "11111111111111111"},
   { L_,   "11111111111111111", 1,  "11111111111111110", "01111111111111111"},
   { L_,   "11111111111111111", 8,  "11111111100000000", "00000000111111111"},
   { L_,   "11111111111111111", 15, "11000000000000000", "00000000000000011"},
   { L_,   "11111111111111111", 16, "10000000000000000", "00000000000000001"},
   { L_,   "11111111111111111", 17, "00000000000000000", "00000000000000000"},

   { L_,   "10000000000000001", 0,  "10000000000000001", "10000000000000001"},
   { L_,   "10000000000000001", 1,  "00000000000000010", "01000000000000000"},
   { L_,   "10000000000000001", 8,  "00000000100000000", "00000000100000000"},
   { L_,   "10000000000000001", 15, "01000000000000000", "00000000000000010"},
   { L_,   "10000000000000001", 16, "10000000000000000", "00000000000000001"},
   { L_,   "10000000000000001", 17, "00000000000000000", "00000000000000000"},

   { L_,   "00001000000010000", 0,  "00001000000010000", "00001000000010000"},
   { L_,   "00001000000010000", 1,  "00010000000100000", "00000100000001000"},
   { L_,   "00001000000010000", 8,  "00001000000000000", "00000000000010000"},
   { L_,   "00001000000010000", 15, "00000000000000000", "00000000000000000"},
   { L_,   "00001000000010000", 16, "00000000000000000", "00000000000000000"},
   { L_,   "00001000000010000", 17, "00000000000000000", "00000000000000000"},
      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int TESTSIZE = 17;  // num bits of char + 1
      if (verbose) printf("\nTesting shift operators with bitset<%d>\n",
                          TESTSIZE);

      typedef bsl::bitset<TESTSIZE> Obj;

      for (int ti = 0; ti < NUM_DATA; ++ti) {
          const unsigned int  LINE      = DATA[ti].d_lineNum;
          const char         *VALUE     = DATA[ti].d_string;
          size_t              POS       = DATA[ti].d_pos;
          const char         *SHIFTLEXP = DATA[ti].d_shiftlexp;
          const char         *SHIFTREXP = DATA[ti].d_shiftrexp;

          if (veryVerbose) {
              T_ P_(LINE) P_(VALUE) P_(POS) P_(SHIFTLEXP) P(SHIFTREXP);
          }

          bsl::string value(VALUE);
          bsl::string lexpected(SHIFTLEXP);
          bsl::string rexpected(SHIFTREXP);

          Obj mX1(value);    const Obj& X1 = mX1;  // shift left
          Obj mX2(value);    const Obj& X2 = mX2;  // shift right
          Obj mY(lexpected); const Obj& Y  = mY;
          Obj mZ(rexpected); const Obj& Z  = mZ;

          Obj mX3 = mX1 << POS; const Obj& X3 = mX3;
          mX1 <<= POS;

          Obj mX4 = mX2 >> POS; const Obj& X4 = mX4;
          mX2 >>= POS;

          if (veryVeryVerbose) {
              T_ T_ P_(X1) P_(X2) P_(X3) P_(X4) P_(Y) P(Z);
          }

          LOOP_ASSERT(LINE, X1 == Y);
          LOOP_ASSERT(LINE, X3 == Y);
          LOOP_ASSERT(LINE, X2 == Z);
          LOOP_ASSERT(LINE, X4 == Z);
      }

      }

      {

      static const struct {
          unsigned int d_lineNum;    // source line number
          const char*  d_string;     // bitset string
          size_t       d_pos;        // amount to shift by
          const char*  d_shiftlexp;  // expected string after left shift
          const char*  d_shiftrexp;  // expected string after right shift
      } DATA[] = {
   //LINE  VALUE                                POS SHIFTLEXP     SHIFTREXP
   //----  -----------------------------------  --- ------------  ------------
   { L_,   "000000000000000000000000000000000", 0,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},
   { L_,   "000000000000000000000000000000000", 1,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},
   { L_,   "000000000000000000000000000000000", 16,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},
   { L_,   "000000000000000000000000000000000", 31,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},
   { L_,   "000000000000000000000000000000000", 32,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},
   { L_,   "000000000000000000000000000000000", 33,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},

   { L_,   "111111111111111111111111111111111", 0,
                                          "111111111111111111111111111111111",
                                          "111111111111111111111111111111111"},
   { L_,   "111111111111111111111111111111111", 1,
                                          "111111111111111111111111111111110",
                                          "011111111111111111111111111111111"},
   { L_,   "111111111111111111111111111111111", 16,
                                          "111111111111111110000000000000000",
                                          "000000000000000011111111111111111"},
   { L_,   "111111111111111111111111111111111", 31,
                                          "110000000000000000000000000000000",
                                          "000000000000000000000000000000011"},
   { L_,   "111111111111111111111111111111111", 32,
                                          "100000000000000000000000000000000",
                                          "000000000000000000000000000000001"},
   { L_,   "111111111111111111111111111111111", 33,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},

   { L_,   "100000000000000000000000000000001", 0,
                                          "100000000000000000000000000000001",
                                          "100000000000000000000000000000001"},
   { L_,   "100000000000000000000000000000001", 1,
                                          "000000000000000000000000000000010",
                                          "010000000000000000000000000000000"},
   { L_,   "100000000000000000000000000000001", 16,
                                          "000000000000000010000000000000000",
                                          "000000000000000010000000000000000"},
   { L_,   "100000000000000000000000000000001", 31,
                                          "010000000000000000000000000000000",
                                          "000000000000000000000000000000010"},
   { L_,   "100000000000000000000000000000001", 32,
                                          "100000000000000000000000000000000",
                                          "000000000000000000000000000000001"},
   { L_,   "100000000000000000000000000000001", 33,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},

   { L_,   "000010000100000000000001000010000", 0,
                                          "000010000100000000000001000010000",
                                          "000010000100000000000001000010000"},
   { L_,   "000010000100000000000001000010000", 1,
                                          "000100001000000000000010000100000",
                                          "000001000010000000000000100001000"},
   { L_,   "000010000100000000000001000010000", 16,
                                          "000000010000100000000000000000000",
                                          "000000000000000000001000010000000"},
   { L_,   "000010000100000000000001000010000", 31,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},
   { L_,   "000010000100000000000001000010000", 32,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},
   { L_,   "000010000100000000000001000010000", 33,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},
      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      const int TESTSIZE = 33;  // num bits of char + 1
      if (verbose) printf("\nTesting shift operators with bitset<%d>\n",
                          TESTSIZE);

      typedef bsl::bitset<TESTSIZE> Obj;

      for (int ti = 0; ti < NUM_DATA; ++ti) {
          const unsigned int  LINE      = DATA[ti].d_lineNum;
          const char         *VALUE     = DATA[ti].d_string;
          size_t              POS       = DATA[ti].d_pos;
          const char         *SHIFTLEXP = DATA[ti].d_shiftlexp;
          const char         *SHIFTREXP = DATA[ti].d_shiftrexp;

          if (veryVerbose) {
              T_ P_(LINE) P_(VALUE) P_(POS) P_(SHIFTLEXP) P(SHIFTREXP);
          }

          bsl::string value(VALUE);
          bsl::string lexpected(SHIFTLEXP);
          bsl::string rexpected(SHIFTREXP);

          Obj mX1(value);    const Obj& X1 = mX1;  // shift left
          Obj mX2(value);    const Obj& X2 = mX2;  // shift right
          Obj mY(lexpected); const Obj& Y  = mY;
          Obj mZ(rexpected); const Obj& Z  = mZ;

          Obj mX3 = mX1 << POS; const Obj& X3 = mX3;
          mX1 <<= POS;

          Obj mX4 = mX2 >> POS; const Obj& X4 = mX4;
          mX2 >>= POS;

          if (veryVeryVerbose) {
              T_ T_ P_(X1) P_(X2) P_(X3) P_(X4) P_(Y) P(Z);
          }

          LOOP_ASSERT(LINE, X1 == Y);
          LOOP_ASSERT(LINE, X3 == Y);
          LOOP_ASSERT(LINE, X2 == Z);
          LOOP_ASSERT(LINE, X4 == Z);
      }

      }

    } break;

    case 10: {
    } break;

    case 9: {
    } break;

    case 8: {
    } break;

    case 7: {
    } break;

    case 6: {
    } break;

    case 5: {
    } break;

    case 4: {
      // --------------------------------------------------------------------
      // TESTING STRING CONSTRUCTORS
      //
      // Concerns:
      //   Ensure that a bitset can be constructed from both native strings
      //   and bslstl strings.
      //
      // Plan:
      //   Using the table-driven technique, construct a bitset, 'X', from both
      //   native and bslstl strings.  Verify the value of the constructed
      //   bitset is as expected. Then transform those strings so that they
      //   use different characters to represent '0' and '1'. For each
      //   transformed string construct a bitset 'Y' and verify its value.
      //
      // Testing:
      //  bitset(native_std::basic_string, size_type, size_type, char, char);
      //  bitset(bsl::basic_string, size_type, size_type, char, char);
      //  bsl::string to_string(char, char) const;
      // --------------------------------------------------------------------

      if (verbose) printf("\nTESTING STRING CONSTRUCTORS"
                          "\n===========================\n");

      const int TESTSIZE = 32;  // 'bitset' size.
      typedef bsl::bitset<TESTSIZE> Obj;

      static const struct {
          unsigned int d_lineNum;  // source line number
          unsigned int d_value;    // bitset value
          const char*  d_string;   // bitset string
      } DATA[] = {
          //LINE  VALUE         STRING
          //----  ----------    -----------------------------------
          { L_,   0,            "00000000000000000000000000000000" },
          { L_,   0x10101010,   "00010000000100000001000000010000" },
          { L_,   0xabcdef01,   "10101011110011011110111100000001" },
          { L_,   0x12345678,   "00010010001101000101011001111000" },
          { L_,   0xffffffff,   "11111111111111111111111111111111" },
          { L_,   0x87654321,   "10000111011001010100001100100001" },
      };
      const int NUM_DATA = sizeof DATA / sizeof *DATA;

      static const struct {
          unsigned int d_lineNum;
          char d_zeroChar;
          char d_oneChar;
      } TRANSLATION_DATA[] = {
            { L_, '0', '1'},
            { L_, '1', '0'},
            { L_, '!', 'A'}
      };
      const int NUM_TRANSLATION_DATA =
              sizeof TRANSLATION_DATA / sizeof *TRANSLATION_DATA;

      if (verbose) printf("\nTesting constructor with native string\n");

      for (int ti = 0; ti < NUM_DATA; ++ti) {
          const unsigned int LINE   = DATA[ti].d_lineNum;
          const unsigned int VALUE  = DATA[ti].d_value;
          const char *       STRING = DATA[ti].d_string;

          if (veryVerbose) { T_ P_(LINE) P_(VALUE) P(STRING) }

          const native_std::string s(STRING);
          Obj mX(s);
          const Obj& X = mX;
          if (veryVeryVerbose) { T_ T_ P(X) }

          LOOP_ASSERT(LINE, verifyBitset(mX, STRING));
          LOOP3_ASSERT(LINE, VALUE, X.to_ulong(), VALUE == X.to_ulong());
#if !defined(BSLSTL_BITSET_NO_REBIND_IN_NATIVE_ALLOCATOR)
          LOOP_ASSERT(LINE, (X.to_string<char, native_std::char_traits<char>,
                             native_std::allocator<char> >() == s));
#endif

          for (int ui = 0; ui < NUM_TRANSLATION_DATA; ++ui) {
              const unsigned int LINE2     = TRANSLATION_DATA[ui].d_lineNum;
              const char         ZERO_CHAR = TRANSLATION_DATA[ui].d_zeroChar;
              const char         ONE_CHAR  = TRANSLATION_DATA[ui].d_oneChar;

              native_std::string sY(STRING);
              changeBitString(&sY, ZERO_CHAR, ONE_CHAR);
              Obj mY(sY, 0, native_std::string::npos, ZERO_CHAR, ONE_CHAR);
              const Obj& Y = mY;
              if (veryVeryVerbose) { T_ T_ P(Y) }

              LOOP_ASSERT(LINE2, verifyBitset(mY, STRING));
              LOOP3_ASSERT(LINE2, VALUE, Y.to_ulong(), VALUE == Y.to_ulong());
#if !defined(BSLSTL_BITSET_NO_REBIND_IN_NATIVE_ALLOCATOR)
              LOOP_ASSERT(LINE2, (Y.to_string<char,
                      native_std::string::traits_type,
                      native_std::string::allocator_type>() == s));
              LOOP_ASSERT(LINE2, (X.to_string<char,
                      native_std::string::traits_type,
                      native_std::string::allocator_type>(
                          ZERO_CHAR, ONE_CHAR) == sY));
#endif

#if __cplusplus >= 201103L
              LOOP_ASSERT(LINE2, (X.to_string<>(ZERO_CHAR, ONE_CHAR) == sY));
#endif
              LOOP2_ASSERT(LINE, LINE2, X == Y);
          }
      }

      if (verbose) printf("\nTesting constructor with bslstl string\n");

      for (int ti = 0; ti < NUM_DATA; ++ti) {
          const unsigned int LINE   = DATA[ti].d_lineNum;
          const unsigned int VALUE  = DATA[ti].d_value;
          const char *       STRING = DATA[ti].d_string;

          const bsl::string s(STRING);
          Obj mX(s);
          const Obj& X = mX;
          if (veryVeryVerbose) { T_ T_ P(X) }

          // Shorten these type names so that an explicit call to 'to_string'
          // can fit on a single 79-char line.
          typedef bsl::string::traits_type    TraitsType;
          typedef bsl::string::allocator_type AllocType;

          LOOP_ASSERT(LINE, verifyBitset(mX, STRING));
          LOOP3_ASSERT(LINE, VALUE, X.to_ulong(), VALUE == X.to_ulong());
          LOOP_ASSERT(LINE, (X.to_string<char, TraitsType, AllocType>() == s));

          for (int ui = 0; ui < NUM_TRANSLATION_DATA; ++ui) {
              const unsigned int LINE2     = TRANSLATION_DATA[ui].d_lineNum;
              const char         ZERO_CHAR = TRANSLATION_DATA[ui].d_zeroChar;
              const char         ONE_CHAR  = TRANSLATION_DATA[ui].d_oneChar;

              bsl::string sY(STRING);
              changeBitString(&sY, ZERO_CHAR, ONE_CHAR);
              Obj mY(sY, 0, bsl::string::npos, ZERO_CHAR, ONE_CHAR);
              const Obj& Y = mY;
              if (veryVeryVerbose) { T_ T_ P(Y) }

              LOOP_ASSERT(LINE2, verifyBitset(mY, STRING));
              LOOP3_ASSERT(LINE2, VALUE, Y.to_ulong(), VALUE == Y.to_ulong());
              LOOP_ASSERT(LINE2, (Y.to_string<char, bsl::string::traits_type,
                     bsl::string::allocator_type>() == s));
              LOOP_ASSERT(LINE2, (X.to_string<char, bsl::string::traits_type,
                     bsl::string::allocator_type>(ZERO_CHAR, ONE_CHAR) == sY));
#if __cplusplus >= 201103L
              LOOP_ASSERT(LINE2, (X.to_string<>(ZERO_CHAR, ONE_CHAR) == sY));
#endif
              LOOP2_ASSERT(LINE, LINE2, X == Y);
          }
      }

    } break;

    case 3: {
      // --------------------------------------------------------------------
      // TESTING UNSIGNED LONG CONSTRUCTOR
      //   Ensure that the unsigned long constructor leaves the object in the
      //   expected state for different initial sizes.
      //
      // Concerns:
      //: 1 All 'N' bits in a 'bitset<N>(k)'-constructed bitset match the N
      //:   low bits of 'k'.
      //:
      //: 2 'bitset<N>(k)' can be used in a constant expression. (C++11 only).
      //
      // Plan:
      //   Construct bitsets of different sizes with different unsigned long
      //   arguments 'k', and check that all 'N' bits in the bitset match the
      //   'N' lowest bits of 'k'. Then repeat the same process except that
      //   the construction and checks are done at compile time (C++11 only).
      //
      // Testing:
      //  constexpr bitset(unsigned long) noexcept;
      //  constexpr bool operator[](std::size_t pos) const;
      //---------------------------------------------------------------------

      if (verbose) printf("\nTESTING UNSIGNED LONG CONSTRUCTOR"
                          "\n=================================\n");

      testCase3<1>(verbose, veryVerbose, veryVeryVerbose);
      testCase3<2>(verbose, veryVerbose, veryVeryVerbose);
      testCase3<3>(verbose, veryVerbose, veryVeryVerbose);
      testCase3<4>(verbose, veryVerbose, veryVeryVerbose);
      testCase3<7>(verbose, veryVerbose, veryVeryVerbose);
      testCase3<8>(verbose, veryVerbose, veryVeryVerbose);
      testCase3<9>(verbose, veryVerbose, veryVeryVerbose);
      testCase3<15>(verbose, veryVerbose, veryVeryVerbose);
      testCase3<16>(verbose, veryVerbose, veryVeryVerbose);
      testCase3<17>(verbose, veryVerbose, veryVeryVerbose);
      testCase3<23>(verbose, veryVerbose, veryVeryVerbose);
      testCase3<24>(verbose, veryVerbose, veryVeryVerbose);
      testCase3<25>(verbose, veryVerbose, veryVeryVerbose);
      testCase3<31>(verbose, veryVerbose, veryVeryVerbose);
      testCase3<32>(verbose, veryVerbose, veryVeryVerbose);
      testCase3<33>(verbose, veryVerbose, veryVeryVerbose);
      testCase3<63>(verbose, veryVerbose, veryVeryVerbose);
      testCase3<64>(verbose, veryVerbose, veryVeryVerbose);
      testCase3<65>(verbose, veryVerbose, veryVeryVerbose);
    } break;

    case 2: {
      // --------------------------------------------------------------------
      // TESTING DEFAULT CONSTRUCTOR
      //   Ensure that the default constructor leaves the object in the
      //   expected state for different initial sizes.
      //
      // Concerns:
      //: 1 All 'N' bits in a default-constructed 'bitset<N>' are initially 0.
      //:
      //: 2 'bitset<N>' implies 'N == size()'.
      //:
      //: 3 'bitset<N>' has a constexpr default constructor.
      //
      // Plan:
      //   Default construct bitsets of different sizes and check 'size',
      //   'none', and 'any', then modify the first and last bits back and
      //   forth and make sure 'none' and 'any' report the expected results.
      //   Also default construct the bitsets as constant expressions and use
      //   the constexpr members 'size()' and 'operator[0]' to check the
      //   invariants in a static_assert.
      //
      // Testing:
      //  constexpr bitset() noexcept;
      //  ~bitset();
      //  reference operator[](std::size_t pos);
      //  constexpr std::size_t size() const noexcept;
      //  bool any() const noexcept;
      //  bool none() const noexcept;
      //---------------------------------------------------------------------

      if (verbose) printf("\nTESTING DEFAULT CONSTRUCTOR"
                          "\n===========================\n");

      testCase2<0>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<1>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<2>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<3>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<4>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<7>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<8>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<9>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<15>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<16>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<17>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<23>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<24>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<25>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<31>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<32>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<33>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<63>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<64>(verbose, veryVerbose, veryVeryVerbose);
      testCase2<65>(verbose, veryVerbose, veryVeryVerbose);

    } break;

    case 1: {
      // --------------------------------------------------------------------
      // BREATHING TEST
      //
      // Concerns:
      //   We want to demonstrate a base-line level of correct operation of
      //   the following methods and operators:
      //     - default and value constructors.
      //     - equality operators: 'operator==' and 'operator!='.
      //     - primary manipulators: 'reset' and 'operator|='.
      //     - basic accessors: 'operator[]'.
      //
      // Plan:
      //   Create four test objects using the default, initializing, and copy
      //   constructors.  Exercise the basic value-semantic methods and the
      //   equality operators using the test objects.  Invoke the primary
      //   manipulator [5, 6, 7].  Use the basic accessors to verify the
      //   expected results.  Display object values frequently in verbose
      //   mode.  Note that 'VA', 'VB' and 'VC' denote unique, but otherwise
      //   arbitrary, object values, while 'U' denotes the valid, but
      //   "unknown", default object value.
      //
      //    1. Create an object x1 (init to VA)   { x1:VA       }
      //    3. Create an object x3 (default ctor) { x1:VA x3:U  }
      //    5. Set x3 using 'update' (set to VB)  { x1:VA x3:VB }
      //    6. Change x1 using 'reset'            { x1:U  x3:VB }
      //    7. Change x1 ('update', set to VC)    { x1:VC x3:VB }
      //
      // Testing:
      //   BREATHING TEST
      // --------------------------------------------------------------------

      if (verbose) printf("\nBREATHING TEST"
                          "\n==============\n");

      const int TESTSIZE = 32;  // 'bitset' size.
      typedef bsl::bitset<TESTSIZE> Obj;

      static const struct {
          int         d_lineNum;  // source line number
          int         d_value;    // bitset value
          const char* d_string;   // bitset string
      } DATA[] = {
          //LINE        VALUE         STRING
          //----        ----------    -----------------------------------
          { L_,         0,            "00000000000000000000000000000000" },
          { L_,         0x10101010,   "00010000000100000001000000010000" },
          { L_,   (int) 0xabcdef01,   "10101011110011011110111100000001" },
          { L_,         0x12345678,   "00010010001101000101011001111000" },
      };

      const int    SA = DATA[1].d_value,
                   SB = DATA[2].d_value,
                   SC = DATA[3].d_value;
      const char  *VA = DATA[1].d_string,
                  *VB = DATA[2].d_string,
                  *VC = DATA[3].d_string,
                  *VU = DATA[0].d_string;

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      if (verbose) printf(
                     "\n 1. Create an object x1 (init to SA).\t\t{ x1:SA }\n");
      Obj mX1(SA);
      const Obj& X1 = mX1;
      if (verbose) { T_ P(X1); }

      if (verbose) printf("\ta. Check initial state of x1.\n");
      ASSERT(verifyBitset(X1, VA));

      if (verbose) printf("\tb. Try equality operators: x1 <op> x1.\n");

      ASSERT(1 == (X1 == X1));
      ASSERT(0 == (X1 != X1));

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      if (verbose) printf("\n 3. Create an object x3 (default ctor)."
                          "\t\t{ x1:SA x2:SA x3:U }\n");
      Obj mX3;
      const Obj& X3 = mX3;
      if (verbose) { T_ P(X3); }

      if (verbose) printf("\ta. Check initial state of x3.\n");
      ASSERT(verifyBitset(X3, VU));

      if (verbose) printf("\tb. Try equality operators: "
                          "x3 <op> x1, x2, x3.\n");
      ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
      ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      if (verbose) printf("\n 5. Set x3 using primary manip (set to VB)."
                          "\t\t{ x1:SA x2:SA x3:SB x4:U }\n");
      mX3.reset();
      mX3 |= Obj(SB);
      if (verbose) { T_ P(X3); }

      if (verbose) printf("\ta. Check new state of x3.\n");
      ASSERT(verifyBitset(X3, VB));

      if (verbose) printf(
                     "\tb. Try equality operators: x3 <op> x1, x2, x3, x4.\n");
      ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
      ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      if (verbose) printf(
              "\n 6. Change x1 using 'reset'.\t\t{ x1:U x2:SA x3:SB x4:U }\n");
      mX1.reset();
      if (verbose) { T_ P(X1); }

      if (verbose) printf("\ta. Check new state of x1.\n");
      ASSERT(verifyBitset(X1, VU));

      if (verbose) printf(
                     "\tb. Try equality operators: x1 <op> x1, x2, x3, x4.\n");
      ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
      ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      if (verbose) printf("\n 7. Change x1 (primary manip, set to VC)."
                          "\t\t{ x1:SC x2:SA x3:SB x4:U }\n");
      mX1.reset();
      mX1 |= Obj(SC);
      if (verbose) { T_ P(X1); }

      if (verbose) printf("\ta. Check new state of x1.\n");
      ASSERT(verifyBitset(X1, VC));

      if (verbose) printf(
                     "\tb. Try equality operators: x1 <op> x1, x2, x3, x4.\n");
      ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
      ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));


    } break;

    case -1: {
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
        // This test case tends to break compilers, consuming too much virtual
        // memory for a 32-build of the compiler
        printf("2147483647 is prime? %s\n",
               isPrime<2147483647>(2147483647) ? "true" : "false");
#endif
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
