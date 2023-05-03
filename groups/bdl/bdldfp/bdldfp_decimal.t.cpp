// bdldfp_decimal.t.cpp                                               -*-C++-*-
#include <bdldfp_decimal.h>
#include <bdldfp_uint128.h>

#include <bdlb_nullablevalue.h>
#include <bdlb_randomdevice.h>

#include <bdlsb_fixedmemoutstreambuf.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bslx_testinstream.h>
#include <bslx_testoutstream.h>
#include <bslx_versionfunctions.h>

#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_climits.h> // CHAR_BIT
#include <bsl_limits.h>
#include <bsl_utility.h>

#include <typeinfo>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;
using bsl::atoi;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test provides a set of value-semantic classes which
// represent a floating point numbers in the decimal base, where each class has
// a different precision.
// ----------------------------------------------------------------------------
// CREATORS
//: o 'Decimal_Type32()'
//: o 'Decimal_Type32(DecimalImplUtil::ValueType32)'
//: o 'Decimal_Type32(Decimal64)'
//: o 'Decimal_Type32(Decimal128)'
//: o 'Decimal_Type32(float)'
//: o 'Decimal_Type32(double)'
//: o 'Decimal_Type32(long double)'
//: o 'Decimal_Type32(int)'
//: o 'Decimal_Type32(unsigned int)'
//: o 'Decimal_Type32(long int)'
//: o 'Decimal_Type32(unsigned long int)'
//: o 'Decimal_Type32(long long)'
//: o 'Decimal_Type32(unsigned long long)'
//: o 'Decimal_Type64()'
//: o 'Decimal_Type64(DecimalImplUtil::ValueType64)'
//: o 'Decimal_Type64(Decimal32)'
//: o 'Decimal_Type64(Decimal128)'
//: o 'Decimal_Type64(float)'
//: o 'Decimal_Type64(double)'
//: o 'Decimal_Type64(long double)'
//: o 'Decimal_Type64(int)'
//: o 'Decimal_Type64(unsigned int)'
//: o 'Decimal_Type64(long)'
//: o 'Decimal_Type64(unsigned long)'
//: o 'Decimal_Type64(long long)'
//: o 'Decimal_Type64(unsigned long long)'
//: o 'Decimal_Type128()'
//: o 'Decimal_Type128(DecimalImplUtil::ValueType128)'
//: o 'Decimal_Type128(Decimal32)'
//: o 'Decimal_Type128(Decimal64)'
//: o 'Decimal_Type128(float)'
//: o 'Decimal_Type128(double)'
//: o 'Decimal_Type128(long double)'
//: o 'Decimal_Type128(int)'
//: o 'Decimal_Type128(unsigned int)'
//: o 'Decimal_Type128(long)'
//: o 'Decimal_Type128(unsigned long)'
//: o 'Decimal_Type128(long long)'
//: o 'Decimal_Type128(unsigned long long)'
//
// MANIPULATORS
//: o 'operator='
//: o 'operator++'
//: o 'operator--'
//: o 'operator+='
//: o 'operator-='
//: o 'operator*='
//: o 'operator/='
//: o 'data'
//
// ACCESSORS
//: o 'data'
//: o 'value'
//: o 'print'
//
// FREE OPERATORS
//: o 'operator+' -- Unary
//: o 'operator++'
//: o 'operator--'
//: o 'operator+'
//: o 'operator-'
//: o 'operator*'
//: o 'operator/'
//: o 'operator=='
//: o 'operator!='
//: o 'operator<'
//: o 'operator>'
//: o 'operator<='
//: o 'operator>='
//: o 'operator>> (bsl::basic_istream<CHARTYPE, TRAITS>& stream...'
//: o 'operator>> (bsl::basic_ostream<CHARTYPE, TRAITS>& stream...'
//: o 'operator"" _d32 (const char *)
//: o 'operator"" _d64 (const char *)
//: o 'operator"" _d128(const char *)
//: o 'operator"" _d32 (const char *, size_t)
//: o 'operator"" _d64 (const char *, size_t)
//: o 'operator"" _d128(const char *, size_t)
//
// FREE FUNCTIONS
//: o void hashAppend(HASHALG& hashAlg, const Decimal32& object);
//: o void hashAppend(HASHALG& hashAlg, const Decimal64& object);
//: o void hashAppend(HASHALG& hashAlg, const Decimal128& object);
//
// ----------------------------------------------------------------------------
// [ 1] Decimal32 Type
// [ 2] Decimal64 Type
// [ 3] Decimal128 Type
// [ 9] REGRESSIONS
// [10] USAGE EXAMPLE
// ----------------------------------------------------------------------------


// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

static int                test;
static int             verbose;
static int         veryVerbose;
static int     veryVeryVerbose;
static int veryVeryVeryVerbose;
static bslma::TestAllocator *pa;

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace BDEC = BloombergLP::bdldfp;

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace UsageExample {
  // TBD
}  // close namespace UsageExample

//=============================================================================
//              GLOBAL HELPER FUNCTIONS AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------

                 // stringstream helpers - not thread safe!

void getStringFromStream(bsl::ostringstream &o, bsl::string  *out)
{
    bslma::TestAllocator osa("osstream");
    bslma::DefaultAllocatorGuard g(&osa);
    *out = o.str();
}

void getStringFromStream(bsl::wostringstream &o, bsl::wstring *out)
{
    bslma::TestAllocator osa("osstream");
    bslma::DefaultAllocatorGuard g(&osa);
    *out = o.str();
}

 // String compare for decimal floating point numbers needs 'e'/'E' conversion

bsl::string& decLower(bsl::string& s)
{
    for (size_t i = 0; i < s.length(); ++i) if ('E' == s[i]) s[i] = 'e';
    return s;
}

bsl::wstring& decLower(bsl::wstring& s)
{
    for (size_t i = 0; i < s.length(); ++i) if (L'E' == s[i]) s[i] = L'e';
    return s;
}

//-----------------------------------------------------------------------------


template <class EXPECT, class RECEIVED>
void checkType(const RECEIVED&)
{
    ASSERT(typeid(EXPECT) == typeid(RECEIVED));
}

                          // Stream buffer helpers

template <int SIZE>
struct BufferBuf : bsl::streambuf {
    BufferBuf() { reset(); }
    const char *str() { *this->pptr() =0; return this->pbase(); }
    void reset() { this->setp(this->d_buf, this->d_buf + SIZE); }
    char d_buf[SIZE + 1];
};

struct PtrInputBuf : bsl::streambuf {
    PtrInputBuf(const char *s) {
        char *x = const_cast<char *>(s);
        this->setg(x, x, x + strlen(x));
    }
};

struct NulBuf : bsl::streambuf {
    char d_dummy[64];
    virtual int overflow(int c) {
        setp( d_dummy, d_dummy + sizeof(d_dummy));
        return traits_type::not_eof(c);
    }
};

long long int clipValue(long long int value,
                        long long int min,
                        long long int max)
    // Clip the specified 'value' to fit segment between the specified 'min'
    // and the specified 'max'.
{
    return static_cast<long long int>(
           (static_cast<unsigned long long int>(value)%(max - min + 1)) + min);
}

bdldfp::Decimal32 randomDecimal32()
    // Return randomly generated Decimal32 object having finite value on
    // success and infinity value otherwise.
{
    int significand = 0;
    int exponent = 0;

    int rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
                                reinterpret_cast<unsigned char*>(&significand),
                                sizeof(int));
    ASSERTV(rc, 0 == rc);

    significand = static_cast<int>(clipValue( significand,
                                             -9999999,
                                              9999999));

    rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
                                   reinterpret_cast<unsigned char*>(&exponent),
                                   sizeof(int));
    ASSERTV(rc, 0 == rc);

    exponent = static_cast<int>(clipValue(exponent, -101, 90));

    return BDEC::DecimalImpUtil::makeDecimalRaw32(significand, exponent);
}

bdldfp::Decimal64 randomDecimal64()
    // Return randomly generated Decimal64 object having finite value on
    // success and infinity value otherwise.
{
    long long int significand = 0;
    int           exponent = 0;

    int rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
                                reinterpret_cast<unsigned char*>(&significand),
                                sizeof(long long int));
    ASSERTV(rc, 0 == rc);

    significand = clipValue( significand,
                            -9999999999999999LL,
                             9999999999999999LL);

    rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
                                   reinterpret_cast<unsigned char*>(&exponent),
                                   sizeof(int));
    ASSERTV(rc, 0 == rc);

    exponent = static_cast<int>(clipValue(exponent, -398, 369));

    return BDEC::DecimalImpUtil::makeDecimalRaw64(significand, exponent);
}

bdldfp::Decimal128 randomDecimal128()
    // Return randomly generated Decimal128 object having finite value on
    // success and infinity value otherwise.
{
    long long int significand = 0;
    int           exponent = 0;

    int rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
                                reinterpret_cast<unsigned char*>(&significand),
                                sizeof(long long int));
    ASSERTV(rc, 0 == rc);

    rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
                                   reinterpret_cast<unsigned char*>(&exponent),
                                   sizeof(int));
    ASSERTV(rc, 0 == rc);

    exponent = static_cast<int>(clipValue(exponent, -6176, 6111));

    return BDEC::DecimalImpUtil::makeDecimalRaw128(significand, exponent);
}

template <class DECIMAL>
bool nanEqual(DECIMAL lhs, DECIMAL rhs)
    // Return true if the specified 'lhs' and 'rhs' are the same value, even in
    // the case of 'NaN'.  Two 'DECIMAL' objects are considered equal if either
    // 'lhs' and 'rhs' are the same value, or both 'lhs' and 'rhs' are not
    // equal to themselves (implying them both to be 'NaN').
{
    return lhs == rhs || (lhs != lhs && rhs != rhs);
}

//=============================================================================
//                      TEST DRIVER NAMESPACE CLASS
//-----------------------------------------------------------------------------

struct TestDriver {
    // This class provides a namespace for the various test-case functions in
    // the test driver main program.  This class is necessitated by
    // compile-time performance issues on some platforms.

    static void testCase10();
    static void testCase9();
    static void testCase8();
    static void testCase7();
    static void testCase6();
    static void testCase5();
    static void testCase4();
    static void testCase3();
    static void testCase2();
    static void testCase1();

};

void TestDriver::testCase10()
{
    // ------------------------------------------------------------------------
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
    // ------------------------------------------------------------------------

    if (verbose) bsl::cout << bsl::endl
                           << "Testing Usage Example" << bsl::endl
                           << "=====================" << bsl::endl;

    if (veryVerbose) bsl::cout << bsl::endl
                               << "Portable initialization of "
                               << "non-integer, constant values"
                               << bsl::endl;
    {
        // If your compiler does not support the C Decimal TR, it does not
        // support decimal floating point literals, only binary floating
        // point literals.  The problem with binary floating point literals
        // is the same as with binary floating point numbers in general:
        // they cannot represent the decimal numbers we care about.  To
        // solve this problem there are 3 macros provided by this component
        // that can be used to initialize decimal floating point types with
        // non-integer values, precisely.  These macros will evaluate to
        // real, C language literals where those are supported and to a
        // runtime-parsed solution otherwise.  The following code
        // demonstrates the use of these macros as well as mixed-type
        // arithmetics and comparisons:
        //..
        bdldfp::Decimal32  d32( BDLDFP_DECIMAL_DF(0.1));
        bdldfp::Decimal64  d64( BDLDFP_DECIMAL_DD(0.2));
        bdldfp::Decimal128 d128(BDLDFP_DECIMAL_DL(0.3));

        ASSERT(d32 + d64 == d128);
        ASSERT(bdldfp::Decimal64(d32)  * 10 == bdldfp::Decimal64(1));
        ASSERT(d64  * 10 == bdldfp::Decimal64(2));
        ASSERT(d128 * 10 == bdldfp::Decimal128(3));
    }

    if (veryVerbose) bsl::cout << bsl::endl
                               << "Precise calculations with decimal "
                               << "values" << bsl::endl;
    {
        //..
        // Suppose we need to add two (decimal) numbers and then tell if
        // the result is a particular decimal number or not.  That can get
        // difficult with binary floating-point, but easy with decimal:
        //..
        if (std::numeric_limits<double>::radix == 2) {
           ASSERT(.1 + .2 != .3);
        }
        ASSERT(BDLDFP_DECIMAL_DD(0.1) +  BDLDFP_DECIMAL_DD(0.2) ==
               BDLDFP_DECIMAL_DD(0.3));
        //..
    }
}

void TestDriver::testCase9()
{
    // ------------------------------------------------------------------------
    // REGRESSIONS
    //
    // Concerns:
    //: 1 'bdlb::NullableValue<Decimal128>' type compiles and behaves as
    //:   expected. See {DRQS 171486531} - bdlb::NullableValue<Dec128>
    //:   compile fails on GCC 32-bit.
    //
    // Plan:
    //: 1 Create a variable of the offending type, verify it is empty, add
    //:   a value, verify it is there. (C-1)
    //
    // Testing:
    //   REGRESSIONS
    // ------------------------------------------------------------------------

    if (verbose) cout << "\nREGRESSIONS"
                      << "\n===========\n";

    bdlb::NullableValue<bdldfp::Decimal128> nvDec128;
    ASSERT(nvDec128.isNull());
    nvDec128.makeValueInplace(BDLDFP_DECIMAL_DL(42.));
    ASSERT(!nvDec128.isNull() && nvDec128.value() == BDLDFP_DECIMAL_DL(42.));
}

void TestDriver::testCase8()
{
    // ------------------------------------------------------------------------
    // TESTING 'operator""'
    //
    // Concerns:
    //: 1 That call of user-defined literal operators are properly forwarded to
    //:   appropriate implementation.
    //:
    //: 2 That if argument value has an absolute value that exceeds maximum
    //:   value supported by tested type then the value of the macro 'ERANGE'
    //:   is stored into 'errno' and the resultant value is initialized to
    //:   infinity with the same sign as argument.
    //:
    //: 3 That if argument value has an absolute value that is less than the
    //:   smallest value supported by tested type then the value of the macro
    //:   'ERANGE' is stored into 'errno' and the resultant value is
    //:   initialized to zero with the same sign as argument.
    //:
    //: 4 That the quanta of the resultant value equals the number of decimal
    //:   places in input string and does exceed the maximum digits necessary
    //:   to differentiate all values of tested decimal type.
    //:
    //
    // Plan:
    //: 1 A set of representative values for 32, 64, and 128-bit types will be
    //:   created using user-defined literal operators and each compared
    //:   against expected value.  (C-1..3)
    //
    // Testing:
    //   bdldfp::Decimal32  operator ""  _d32(const char *);
    //   bdldfp::Decimal64  operator ""  _d64(const char *);
    //   bdldfp::Decimal128 operator "" _d128(const char *);
    //   bdldfp::Decimal32  operator ""  _d32(const char *, size_t);
    //   bdldfp::Decimal64  operator ""  _d64(const char *, size_t);
    //   bdldfp::Decimal128 operator "" _d128(const char *, size_t);
    // ------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)  && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS)
    if (verbose) bsl::cout
                     << bsl::endl
                     << "TESTING DECIMAL USER-DEFINED LITERALS" << bsl::endl
                     << "=====================================" << bsl::endl;
    {
#define DFP(X) BDLDFP_DECIMAL_DF(X)
        using namespace bdldfp;
        using namespace DecimalLiterals;

        typedef Decimal32 Tested;

        const Tested  NaN = BDEC::DecimalImpUtil::quietNaN32();
        const Tested sNaN = BDEC::DecimalImpUtil::signalingNaN32();
        const Tested  Inf = BDEC::DecimalImpUtil::infinity32();

        struct {
            int          d_line;
            Tested       d_x;
            Tested       d_expected;
        } DATA[] = {
            //-------------------------------------------
            // LINE |     X            | EXPECTED
            //-------------------------------------------
            //-------------------------------------------
            // Fixed notation
            { L_,     0_d32,              DFP( 0.0)      },
            { L_,    +0_d32,              DFP( 0.0)      },
            { L_,    -0_d32,              DFP( 0.0)      },
            { L_,   "0"_d32,              DFP( 0.0)      },
            { L_,  "+0"_d32,              DFP( 0.0)      },
            { L_,  "-0"_d32,              DFP( 0.0)      },

            { L_,     4.2_d32,            DFP( 4.2)      },
            { L_,   -42.0_d32,            DFP(-42.0)     },
            { L_,    -0.42_d32,           DFP(-0.42)     },
            { L_,     1.23456789_d32,     DFP( 1.234568) },
            { L_,    "4.2"_d32,           DFP( 4.2)      },
            { L_,  "-42.0"_d32,           DFP(-42.0)     },
            { L_,   "-0.42"_d32,          DFP(-0.42)     },
            { L_,    "1.23456789"_d32,    DFP( 1.234568) },
            //-------------------------------------------
            // Scientific notation
            { L_,     4.2e+0_d32,         DFP( 4.2)      },
            { L_,    -4.2e+1_d32,         DFP(-42.0)     },
            { L_,    -4.2e-1_d32,         DFP(-0.42)     },
            { L_,     1.23456789e+0_d32,  DFP( 1.234568) },
            { L_,    "4.2e+0"_d32,        DFP( 4.2)      },
            { L_,   "-4.2e+1"_d32,        DFP(-42.0)     },
            { L_,   "-4.2e-1"_d32,        DFP(-0.42)     },
            { L_,    "1.23456789e+0"_d32, DFP( 1.234568) },
            //-------------------------------------------
            // Special values
            { L_,     1e+200_d32,         Inf            },
            { L_,    -1e+200_d32,        -Inf            },
            { L_,     1e-200_d32,         DFP( 0.0)      },
            { L_,    -1e-200_d32,         DFP(-0.0)      },
            { L_,    "1e+200"_d32,        Inf            },
            { L_,   -"1e+200"_d32,       -Inf            },
            { L_,    "1e-200"_d32,        DFP( 0.0)      },
            { L_,   -"1e-200"_d32,        DFP(-0.0)      },

            { L_,      "nan"_d32,         NaN            },
            { L_,     "-nan"_d32,        -NaN            },
            { L_,     -"nan"_d32,        -NaN            },

            { L_,     "snan"_d32,        sNaN            },
            { L_,    "-snan"_d32,       -sNaN            },
            { L_,    -"snan"_d32,       -sNaN            },

            { L_,      "inf"_d32,         Inf            },
            { L_,     "-inf"_d32,        -Inf            },
            { L_,     -"inf"_d32,        -Inf            },

            { L_,     "ABCDEF"_d32,       NaN            },
            //
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE     = DATA[ti].d_line;
            const Tested& X        = DATA[ti].d_x;
            const Tested& EXPECTED = DATA[ti].d_expected;

            LOOP3_ASSERT(LINE, X, EXPECTED, nanEqual(X, EXPECTED));
        }

        { // C-2
            errno = 0;
            const Tested  X1       = 1.0e+200_d32;
            const Tested &EXPECTED = Inf;

            LOOP3_ASSERT(L_, X1, EXPECTED, X1 == EXPECTED);
            LOOP3_ASSERT(L_, errno, ERANGE, errno == ERANGE);
        }
        { // C-2
            errno = 0;
            const Tested& EXPECTED = -Inf;
            const Tested  X1       = -1.0e+200_d32;

            LOOP3_ASSERT(L_, X1, EXPECTED, X1 == EXPECTED);
            LOOP3_ASSERT(L_, errno, ERANGE, errno == ERANGE);
        }
        { // C-3
            errno = 0;
            const Tested  X1       = 1.0e-200_d32;
            const Tested& EXPECTED = DFP(0.0);

            LOOP3_ASSERT(L_, X1, EXPECTED, X1 == EXPECTED);
            LOOP3_ASSERT(L_, errno, ERANGE, errno == ERANGE);
        }
        { // C-3
            errno = 0;
            const Tested  X1       = -1.0e-200_d32;
            const Tested& EXPECTED = DFP(-0.0);

            LOOP3_ASSERT(L_, X1, EXPECTED, X1 == EXPECTED);
            LOOP3_ASSERT(L_, errno, ERANGE, errno == ERANGE);
        }
        { // C-4
            const Tested  X1 = -1.000_d32;
            const Tested  X2 = "2.000e-4"_d32;

            int           SIGN;
            unsigned int  SIGNIFICAND;
            int           EXPONENT;
            int           CLASS = BDEC::DecimalImpUtil::decompose(&SIGN,
                                                                  &SIGNIFICAND,
                                                                  &EXPONENT,
                                                                  X1.value());

            ASSERTV(L_, CLASS,       FP_NORMAL == CLASS);
            ASSERTV(L_, SIGN,               -1 == SIGN);
            ASSERTV(L_, SIGNIFICAND,      1000 == SIGNIFICAND);
            ASSERTV(L_, EXPONENT,           -3 == EXPONENT);

            CLASS = BDEC::DecimalImpUtil::decompose(&SIGN,
                                                    &SIGNIFICAND,
                                                    &EXPONENT,
                                                    X2.value());

            ASSERTV(L_, CLASS,       FP_NORMAL == CLASS);
            ASSERTV(L_, SIGN,                1 == SIGN);
            ASSERTV(L_, SIGNIFICAND,      2000 == SIGNIFICAND);
            ASSERTV(L_, EXPONENT,           -7 == EXPONENT);
        }

#undef DFP
    }
    {
#define DFP(X) BDLDFP_DECIMAL_DD(X)
        using namespace bdldfp;
        using namespace DecimalLiterals;

        typedef Decimal64 Tested;

        const Tested  NaN = BDEC::DecimalImpUtil::quietNaN64();
        const Tested sNaN = BDEC::DecimalImpUtil::signalingNaN64();
        const Tested  Inf = BDEC::DecimalImpUtil::infinity64();

        struct {
            int          d_line;
            Tested       d_x;
            Tested       d_expected;
        } DATA[] = {
            //------------------------------------------------------------
            // LINE |     X                   | EXPECTED
            //------------------------------------------------------------
            //------------------------------------------------------------
            // Fixed notation
            { L_,     0.0_d64,                  DFP( 0.0)                },
            { L_,    -0.0_d64,                  DFP(-0.0)                },
            { L_,    +0.0_d64,                  DFP( 0.0)                },
            { L_,     "0"_d64,                  DFP( 0.0)                },
            { L_,    "+0"_d64,                  DFP( 0.0)                },
            { L_,    "-0"_d64,                  DFP( 0.0)                },

            { L_,     4.2_d64,                  DFP( 4.2)                },
            { L_,   -42.0_d64,                  DFP(-42.0)               },
            { L_,   -0.42_d64,                  DFP(-0.42)               },
            { L_,    1.2345678901234567_d64,    DFP( 1.234567890123457)  },
            { L_,   "4.2"_d64,                  DFP( 4.2)                },
            { L_, "-42.0"_d64,                  DFP(-42.0)               },
            { L_, "-0.42"_d64,                  DFP(-0.42)               },
            { L_,   "1.2345678901234567"_d64,   DFP( 1.234567890123457)  },
            //------------------------------------------------------------
            // Scientific notation
            { L_,   4.2e+0_d64,                 DFP( 4.2)                },
            { L_,  -4.2e+1_d64,                 DFP(-42.0)               },
            { L_,  -4.2e-1_d64,                 DFP(-0.42)               },
            { L_,   1.2345678901234567e+0_d64,  DFP( 1.2345678901234567) },
            { L_,  "4.2e+0"_d64,                DFP( 4.2)                },
            { L_, "-4.2e+1"_d64,                DFP(-42.0)               },
            { L_, "-4.2e-1"_d64,                DFP(-0.42)               },
            { L_,  "1.2345678901234567e+0"_d64, DFP( 1.2345678901234567) },
            //------------------------------------------------------------
            // Special values
            { L_,    1e+400_d64,                Inf                      },
            { L_,   -1e+400_d64,               -Inf                      },
            { L_,    1e-400_d64,                DFP( 0.0)                },
            { L_,   -1e-400_d64,                DFP( 0.0)                },
            { L_,   "1e+400"_d64,               Inf                      },
            { L_,  -"1e+400"_d64,              -Inf                      },
            { L_,   "1e-400"_d64,               DFP( 0.0)                },
            { L_,  -"1e-400"_d64,               DFP(-0.0)                },

            { L_,     "nan"_d64,                NaN                      },
            { L_,    "-nan"_d64,               -NaN                      },
            { L_,    -"nan"_d64,               -NaN                      },

            { L_,    "snan"_d64,               sNaN                      },
            { L_,   "-snan"_d64,              -sNaN                      },
            { L_,   -"snan"_d64,              -sNaN                      },

            { L_,     "inf"_d64,                Inf                      },
            { L_,    "-inf"_d64,               -Inf                      },
            { L_,    -"inf"_d64,               -Inf                      },

            { L_,    "ABCDEF"_d64,              NaN                      }
        };

        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE     = DATA[ti].d_line;
            const Tested& X        = DATA[ti].d_x;
            const Tested& EXPECTED = DATA[ti].d_expected;

            LOOP3_ASSERT(LINE, X, EXPECTED, nanEqual(X, EXPECTED));
        }

        { // C-2
            errno = 0;
            const Tested& X1       = 1.0e+400_d64;
            const Tested& EXPECTED = Inf;

            LOOP3_ASSERT(L_, X1, EXPECTED, X1 == EXPECTED);
            LOOP3_ASSERT(L_, errno, ERANGE, errno == ERANGE);
        }
        { // C-2
            errno = 0;
            const Tested  X1       = -1.0e+400_d64;
            const Tested& EXPECTED = -Inf;

            LOOP3_ASSERT(L_, X1, EXPECTED, X1 == EXPECTED);
            LOOP3_ASSERT(L_, errno, ERANGE, errno == ERANGE);
        }
        { // C-3
            errno = 0;
            const Tested  X1       = 1.0e-400_d64;
            const Tested& EXPECTED = DFP(0.0);

            LOOP3_ASSERT(L_, X1, EXPECTED, X1 == EXPECTED);
            LOOP3_ASSERT(L_, errno, ERANGE, errno == ERANGE);
        }
        { // C-3
            errno = 0;
            const Tested  X1       = -1.0e-400_d64;
            const Tested& EXPECTED = DFP(-0.0);

            LOOP3_ASSERT(L_, X1, EXPECTED, X1 == EXPECTED);
            LOOP3_ASSERT(L_, errno, ERANGE, errno == ERANGE);
        }
        { // C-4
            const Tested  X1 = -1.000_d64;
            const Tested  X2 = "2.000e-4"_d64;

            int                 SIGN;
            bsls::Types::Uint64 SIGNIFICAND;
            int                 EXPONENT;

            int CLASS = BDEC::DecimalImpUtil::decompose(&SIGN,
                                                        &SIGNIFICAND,
                                                        &EXPONENT,
                                                        X1.value());

            ASSERTV(L_, CLASS,       FP_NORMAL == CLASS);
            ASSERTV(L_, SIGN,               -1 == SIGN);
            ASSERTV(L_, SIGNIFICAND,      1000 == SIGNIFICAND);
            ASSERTV(L_, EXPONENT,           -3 == EXPONENT);

            CLASS = BDEC::DecimalImpUtil::decompose(&SIGN,
                                                    &SIGNIFICAND,
                                                    &EXPONENT,
                                                    X2.value());

            ASSERTV(L_, CLASS,       FP_NORMAL == CLASS);
            ASSERTV(L_, SIGN,                1 == SIGN);
            ASSERTV(L_, SIGNIFICAND,      2000 == SIGNIFICAND);
            ASSERTV(L_, EXPONENT,           -7 == EXPONENT);
        }

#undef DFP
    }
    {
#define DFP(X) BDLDFP_DECIMAL_DL(X)
        using namespace bdldfp;
        using namespace DecimalLiterals;

        typedef Decimal128 Tested;

        const Tested  NaN = BDEC::DecimalImpUtil::quietNaN128();
        const Tested sNaN = BDEC::DecimalImpUtil::signalingNaN128();
        const Tested  Inf = BDEC::DecimalImpUtil::infinity128();

        struct {
            int          d_line;
            Tested       d_x;
            Tested       d_expected;
        } DATA[] = {
            //-----------------------------------------------------------
            // LINE |     X                   | EXPECTED
            //-----------------------------------------------------------
            // Fixed notation
            { L_,     0.0_d128,                DFP( 0.0)                },
            { L_,    -0.0_d128,                DFP(-0.0)                },
            { L_,    +0.0_d128,                DFP( 0.0)                },
            { L_,     "0"_d128,                 DFP( 0.0)               },
            { L_,    "+0"_d128,                 DFP( 0.0)               },
            { L_,    "-0"_d128,                 DFP( 0.0)               },

            { L_,     4.2_d128,                DFP( 4.2)                },
            { L_,   -42.0_d128,                DFP(-42.0)               },
            { L_,   -0.42_d128,                DFP(-0.42)               },
            { L_,    1.234567890123456789012345678901234567_d128,
                             DFP( 1.234567890123456789012345678901235)  },
            { L_,   "4.2"_d128,                 DFP( 4.2)               },
            { L_, "-42.0"_d128,                 DFP(-42.0)              },
            { L_, "-0.42"_d128,                 DFP(-0.42)              },
            { L_,  "1.234567890123456789012345678901234567"_d128,
                             DFP( 1.234567890123456789012345678901235)  },
            //-----------------------------------------------------------
            // Scientific notation
            { L_,   4.2e+0_d128,               DFP( 4.2)                },
            { L_,  -4.2e+1_d128,               DFP(-42.0)               },
            { L_,  -4.2e-1_d128,               DFP(-0.42)               },
            { L_,   1.234567890123456789012345678901234567e+0_d128,
                              DFP( 1.234567890123456789012345678901235) },
            //-----------------------------------------------------------
            // Special values
            { L_,    1e+7000_d128,             Inf                      },
            { L_,   -1e+7000_d128,            -Inf                      },
            { L_,    1e-7000_d128,             DFP( 0.0)                },
            { L_,   -1e-7000_d128,             DFP(-0.0)                },
            { L_,   "1e+7000"_d128,            Inf                      },
            { L_,  -"1e+7000"_d128,           -Inf                      },
            { L_,   "1e-7000"_d128,            DFP( 0.0)                },
            { L_,  -"1e-7000"_d128,            DFP(-0.0)                },

            { L_,      "nan"_d128,             NaN                      },
            { L_,     "-nan"_d128,            -NaN                      },
            { L_,     -"nan"_d128,            -NaN                      },

            { L_,     "snan"_d128,            sNaN                      },
            { L_,    "-snan"_d128,           -sNaN                      },
            { L_,    -"snan"_d128,           -sNaN                      },

            { L_,      "inf"_d128,             Inf                      },
            { L_,     "-inf"_d128,            -Inf                      },
            { L_,     -"inf"_d128,            -Inf                      },

            { L_,     "ABCDEF"_d128,           NaN                      }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE     = DATA[ti].d_line;
            const Tested& X        = DATA[ti].d_x;
            const Tested& EXPECTED = DATA[ti].d_expected;

            LOOP3_ASSERT(LINE, X, EXPECTED, nanEqual(X, EXPECTED));
        }

        { // C-2
            errno = 0;
            const Tested& X1       = 1.0e+7000_d128;
            const Tested& EXPECTED = Inf;

            LOOP3_ASSERT(L_, X1, EXPECTED, X1 == EXPECTED);
            LOOP3_ASSERT(L_, errno, ERANGE, errno == ERANGE);
        }
        { // C-2
            errno = 0;
            const Tested  X1       = -1.0e+7000_d128;
            const Tested& EXPECTED = -Inf;

            LOOP3_ASSERT(L_, X1, EXPECTED, X1 == EXPECTED);
            LOOP3_ASSERT(L_, errno, ERANGE, errno == ERANGE);
        }
        { // C-3
            errno = 0;
            const Tested  X1       = 1.0e-7000_d128;
            const Tested& EXPECTED = DFP(0.0);

            LOOP3_ASSERT(L_, X1, EXPECTED, X1 == EXPECTED);
            LOOP3_ASSERT(L_, errno, ERANGE, errno == ERANGE);
        }
        { // C-3
            errno = 0;
            const Tested  X1       = -1.0e-7000_d128;
            const Tested& EXPECTED = DFP(-0.0);

            LOOP3_ASSERT(L_, X1, EXPECTED, X1 == EXPECTED);
            LOOP3_ASSERT(L_, errno, ERANGE, errno == ERANGE);
        }
        { // C-4
            const Tested  X1 = -1.000_d128;
            const Tested  X2 = "2.000e-4"_d128;

            int            SIGN;
            BDEC::Uint128  SIGNIFICAND;
            int            EXPONENT;

            int CLASS = BDEC::DecimalImpUtil::decompose(&SIGN,
                                                        &SIGNIFICAND,
                                                        &EXPONENT,
                                                        X1.value());

            ASSERTV(L_, CLASS,             FP_NORMAL == CLASS);
            ASSERTV(L_, SIGN,                     -1 == SIGN);
            ASSERTV(L_, SIGNIFICAND.low(),      1000 == SIGNIFICAND);
            ASSERTV(L_, EXPONENT,                 -3 == EXPONENT);

            CLASS = BDEC::DecimalImpUtil::decompose(&SIGN,
                                                    &SIGNIFICAND,
                                                    &EXPONENT,
                                                    X2.value());

            ASSERTV(L_, CLASS,             FP_NORMAL == CLASS);
            ASSERTV(L_, SIGN,                      1 == SIGN);
            ASSERTV(L_, SIGNIFICAND.low(),      2000 == SIGNIFICAND);
            ASSERTV(L_, EXPONENT,                 -7 == EXPONENT);
        }
#undef DFP
    }
#endif
}

void TestDriver::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING 'hashAppend'
    //
    // Concerns:
    //: 1 The 'hashAppend' function hashes objects with different values
    //:   differently.
    //:
    //: 2 The 'hashAppend' function hashes objects with the same values
    //:   identically irregardless their representations.
    //:
    //: 3 The 'hashAppend' function hashes 'const' and non-'const' objects.
    //
    // Plan:
    //: 1 Brute force test of a several hand picked and randomly generated
    //:   values, ensuring that hashes of equivalent objects match and hashes
    //:   of unequal objects do not.  (C-1..3)
    //
    // Testing:
    //   void hashAppend(HASHALG& hashAlg, const Decimal32& object);
    //   void hashAppend(HASHALG& hashAlg, const Decimal64& object);
    //   void hashAppend(HASHALG& hashAlg, const Decimal128& object);
    // ------------------------------------------------------------------------

    if (verbose) bsl::cout << bsl::endl
                           << "Testing 'hashAppend'" << bsl::endl
                           << "====================" << bsl::endl;

    typedef ::BloombergLP::bslh::Hash<> Hasher;
    typedef Hasher::result_type         HashType;

    Hasher hasher;

    bslma::TestAllocator va("vector", veryVeryVeryVerbose);

    if (verbose) bsl::cout << "\tTesting Decimal32" << bsl::endl;
    {
        typedef BDEC::Decimal32                      Obj;
        typedef bsl::numeric_limits<BDEC::Decimal32> d32_limits;
        typedef bsl::pair<Obj, HashType>             TestDataPair;
        typedef bsl::vector<TestDataPair>            TestDataVector;

        TestDataVector testData(&va);

        // Adding boundary and special values.

        const Obj ZERO_P        = BDLDFP_DECIMAL_DF(0.00);
        const Obj MIN_P         = d32_limits::min();
        const Obj MAX_P         = d32_limits::max();
        const Obj EPSILON_P     = d32_limits::epsilon();
        const Obj ROUND_ERROR_P = d32_limits::round_error();
        const Obj INFINITE_P    = d32_limits::infinity();
        const Obj DENORM_MIN_P  = d32_limits::denorm_min();
        const Obj ZERO_N        = -ZERO_P;
        const Obj MIN_N         = -MIN_P;
        const Obj MAX_N         = -MAX_P;
        const Obj EPSILON_N     = -EPSILON_P;
        const Obj ROUND_ERROR_N = -ROUND_ERROR_P;
        const Obj INFINITE_N    = -INFINITE_P;
        const Obj DENORM_MIN_N  = -DENORM_MIN_P;

        const Obj Q_NAN      = d32_limits::quiet_NaN();
        HashType  Q_NAN_HASH = hasher(Q_NAN);
        const Obj S_NAN      = d32_limits::signaling_NaN();
        HashType  S_NAN_HASH = hasher(S_NAN);

        testData.push_back(TestDataPair(ZERO_P       , hasher(ZERO_P       )));
        testData.push_back(TestDataPair(MIN_P        , hasher(MIN_P        )));
        testData.push_back(TestDataPair(MAX_P        , hasher(MAX_P        )));
        testData.push_back(TestDataPair(EPSILON_P    , hasher(EPSILON_P    )));
        testData.push_back(TestDataPair(ROUND_ERROR_P, hasher(ROUND_ERROR_P)));
        testData.push_back(TestDataPair(INFINITE_P   , hasher(INFINITE_P   )));
        testData.push_back(TestDataPair(DENORM_MIN_P , hasher(DENORM_MIN_P )));
        testData.push_back(TestDataPair(ZERO_N       , hasher(ZERO_N       )));
        testData.push_back(TestDataPair(MIN_N        , hasher(MIN_N        )));
        testData.push_back(TestDataPair(MAX_N        , hasher(MAX_N        )));
        testData.push_back(TestDataPair(EPSILON_N    , hasher(EPSILON_N    )));
        testData.push_back(TestDataPair(ROUND_ERROR_N, hasher(ROUND_ERROR_N)));
        testData.push_back(TestDataPair(INFINITE_N   , hasher(INFINITE_N   )));
        testData.push_back(TestDataPair(DENORM_MIN_N , hasher(DENORM_MIN_N )));

        // Adding identical values, having different representations:
        // 1e+3
        // 10e+2
        // ...
        // 100000e-2
        // 1000000e-3

        int       cloneSignificand = 1;
        int       cloneExponent    = 3;
        const Obj ORIGINAL         = BDEC::DecimalImpUtil::makeDecimalRaw32(
                                                              cloneSignificand,
                                                              cloneExponent);
        testData.push_back(TestDataPair(ORIGINAL, hasher(ORIGINAL)));

        for (int i = 0; i < 6; ++i) {
            --cloneExponent;
            cloneSignificand *= 10;
            const Obj CLONE = BDEC::DecimalImpUtil::makeDecimalRaw32(
                                                              cloneSignificand,
                                                              cloneExponent);
            ASSERTV(ORIGINAL, CLONE, ORIGINAL == CLONE);
            testData.push_back(TestDataPair(CLONE, hasher(CLONE)));
        }

        // Adding zero values, having different representations.

        const Obj ZERO1 = BDLDFP_DECIMAL_DF(-0.00);
        const Obj ZERO2 = BDEC::DecimalImpUtil::makeDecimalRaw32(0, 1);
        const Obj ZERO3 = BDEC::DecimalImpUtil::makeDecimalRaw32(0, 10);
        const Obj ZERO4 = BDEC::DecimalImpUtil::makeDecimalRaw32(0, 90);

        testData.push_back(TestDataPair(ZERO1, hasher(ZERO1)));
        testData.push_back(TestDataPair(ZERO2, hasher(ZERO2)));
        testData.push_back(TestDataPair(ZERO3, hasher(ZERO3)));
        testData.push_back(TestDataPair(ZERO4, hasher(ZERO4)));

        // Adding random values.

        const int RANDOM_VALUES_NUM = 100;
        for (int i = 0; i < RANDOM_VALUES_NUM; ++i) {
            Obj randomValue = randomDecimal32();
            testData.push_back(TestDataPair(randomValue,
                                            hasher(randomValue)));
        }

        if (veryVerbose) {
            T_ T_ P(testData.size());
        }

        // Testing 'hashAppend' function.

        TestDataVector::iterator iter1 = testData.begin();

        while (iter1 != testData.end()) {
            const Obj&               VALUE1 = iter1->first;
            const HashType&          HASH1  = iter1->second;
            TestDataVector::iterator iter2  = testData.begin();

            if (veryVerbose) {
                T_ T_ P_(VALUE1) P(HASH1);
            }

            while (iter2 != testData.end()) {
                const Obj&      VALUE2 = iter2->first;
                const HashType& HASH2  = iter2->second;
                if (VALUE1 == VALUE2) {
                    ASSERTV(VALUE1, VALUE2, HASH1, HASH2, HASH1 == HASH2);
                } else {
                    ASSERTV(VALUE1, VALUE2, HASH1, HASH2, HASH1 != HASH2);
                }
                ++iter2;
            }
            ASSERT(Q_NAN_HASH != HASH1);
            ASSERT(S_NAN_HASH != HASH1);
            ++iter1;
        }
    }

    if (verbose) bsl::cout << "\tTesting Decimal64" << bsl::endl;
    {
        typedef BDEC::Decimal64                      Obj;
        typedef bsl::numeric_limits<BDEC::Decimal32> d64_limits;
        typedef bsl::pair<Obj, HashType>             TestDataPair;
        typedef bsl::vector<TestDataPair>            TestDataVector;

        TestDataVector       testData(&va);

        // Adding boundary and special values.

        const Obj ZERO_P        = BDLDFP_DECIMAL_DD(0.00);
        const Obj MIN_P         = d64_limits::min();
        const Obj MAX_P         = d64_limits::max();
        const Obj EPSILON_P     = d64_limits::epsilon();
        const Obj ROUND_ERROR_P = d64_limits::round_error();
        const Obj INFINITE_P    = d64_limits::infinity();
        const Obj DENORM_MIN_P  = d64_limits::denorm_min();
        const Obj ZERO_N        = -ZERO_P;
        const Obj MIN_N         = -MIN_P;
        const Obj MAX_N         = -MAX_P;
        const Obj EPSILON_N     = -EPSILON_P;
        const Obj ROUND_ERROR_N = -ROUND_ERROR_P;
        const Obj INFINITE_N    = -INFINITE_P;
        const Obj DENORM_MIN_N  = -DENORM_MIN_P;

        const Obj Q_NAN      = d64_limits::quiet_NaN();
        HashType  Q_NAN_HASH = hasher(Q_NAN);
        const Obj S_NAN      = d64_limits::signaling_NaN();
        HashType  S_NAN_HASH = hasher(S_NAN);

        testData.push_back(TestDataPair(ZERO_P       , hasher(ZERO_P       )));
        testData.push_back(TestDataPair(MIN_P        , hasher(MIN_P        )));
        testData.push_back(TestDataPair(MAX_P        , hasher(MAX_P        )));
        testData.push_back(TestDataPair(EPSILON_P    , hasher(EPSILON_P    )));
        testData.push_back(TestDataPair(ROUND_ERROR_P, hasher(ROUND_ERROR_P)));
        testData.push_back(TestDataPair(INFINITE_P   , hasher(INFINITE_P   )));
        testData.push_back(TestDataPair(DENORM_MIN_P , hasher(DENORM_MIN_P )));
        testData.push_back(TestDataPair(ZERO_N       , hasher(ZERO_N       )));
        testData.push_back(TestDataPair(MIN_N        , hasher(MIN_N        )));
        testData.push_back(TestDataPair(MAX_N        , hasher(MAX_N        )));
        testData.push_back(TestDataPair(EPSILON_N    , hasher(EPSILON_N    )));
        testData.push_back(TestDataPair(ROUND_ERROR_N, hasher(ROUND_ERROR_N)));
        testData.push_back(TestDataPair(INFINITE_N   , hasher(INFINITE_N   )));
        testData.push_back(TestDataPair(DENORM_MIN_N , hasher(DENORM_MIN_N )));

        // Adding identical values, having different representations:
        // 1e+7
        // 10e+6
        // ...
        // 100000000000000e-7
        // 1000000000000000e-8

        long long cloneSignificand = 1;
        int       cloneExponent    = 7;
        const Obj ORIGINAL         = BDEC::DecimalImpUtil::makeDecimalRaw64(
                                                              cloneSignificand,
                                                              cloneExponent);
        testData.push_back(TestDataPair(ORIGINAL, hasher(ORIGINAL)));

        for (int i = 0; i < 15; ++i) {
            --cloneExponent;
            cloneSignificand *= 10;
            const Obj CLONE = BDEC::DecimalImpUtil::makeDecimalRaw64(
                                                              cloneSignificand,
                                                              cloneExponent);
            ASSERTV(ORIGINAL, CLONE, ORIGINAL == CLONE);
            testData.push_back(TestDataPair(CLONE, hasher(CLONE)));
        }

        // Adding zero values, having different representations.

        const Obj ZERO1 = BDLDFP_DECIMAL_DD(-0.00);
        const Obj ZERO2 = BDEC::DecimalImpUtil::makeDecimalRaw64(0, 1);
        const Obj ZERO3 = BDEC::DecimalImpUtil::makeDecimalRaw64(0, 10);
        const Obj ZERO4 = BDEC::DecimalImpUtil::makeDecimalRaw64(0, 100);
        const Obj ZERO5 = BDEC::DecimalImpUtil::makeDecimalRaw64(0, 369);

        testData.push_back(TestDataPair(ZERO1, hasher(ZERO1)));
        testData.push_back(TestDataPair(ZERO2, hasher(ZERO2)));
        testData.push_back(TestDataPair(ZERO3, hasher(ZERO3)));
        testData.push_back(TestDataPair(ZERO4, hasher(ZERO4)));
        testData.push_back(TestDataPair(ZERO5, hasher(ZERO5)));

        // Adding random values.

        const int RANDOM_VALUES_NUM = 100;
        for (int i = 0; i < RANDOM_VALUES_NUM; ++i) {
            const Obj randomValue = randomDecimal64();
            testData.push_back(TestDataPair(randomValue,
                                            hasher(randomValue)));
        }

        if (veryVerbose) {
            T_ T_ P(testData.size());
        }

        // Testing 'hashAppend' function.

        TestDataVector::iterator iter1 = testData.begin();

        while (iter1 != testData.end()) {
            const Obj&               VALUE1 = iter1->first;
            const HashType&          HASH1  = iter1->second;
            TestDataVector::iterator iter2  = testData.begin();

            if (veryVerbose) {
                T_ T_ P_(VALUE1) P(HASH1);
            }

            while (iter2 != testData.end()) {
                const Obj&      VALUE2 = iter2->first;
                const HashType& HASH2  = iter2->second;
                if (VALUE1 == VALUE2) {
                    ASSERTV(VALUE1, VALUE2, HASH1, HASH2, HASH1 == HASH2);
                } else {
                    ASSERTV(VALUE1, VALUE2, HASH1, HASH2, HASH1 != HASH2);
                }
                ++iter2;
            }
            ASSERT(Q_NAN_HASH != HASH1);
            ASSERT(S_NAN_HASH != HASH1);
            ++iter1;
        }
    }

    if (verbose) bsl::cout << "\tTesting Decimal128" << bsl::endl;
    {
        typedef BDEC::Decimal128                     Obj;
        typedef bsl::numeric_limits<BDEC::Decimal32> d128_limits;
        typedef bsl::pair<Obj, HashType>             TestDataPair;
        typedef bsl::vector<TestDataPair>            TestDataVector;

        TestDataVector       testData(&va);

        // Adding boundary and special values.

        const Obj ZERO_P        = BDLDFP_DECIMAL_DL(0.00);
        const Obj MIN_P         = d128_limits::min();
        const Obj MAX_P         = d128_limits::max();
        const Obj EPSILON_P     = d128_limits::epsilon();
        const Obj ROUND_ERROR_P = d128_limits::round_error();
        const Obj INFINITE_P    = d128_limits::infinity();
        const Obj DENORM_MIN_P  = d128_limits::denorm_min();
        const Obj ZERO_N        = -ZERO_P;
        const Obj MIN_N         = -MIN_P;
        const Obj MAX_N         = -MAX_P;
        const Obj EPSILON_N     = -EPSILON_P;
        const Obj ROUND_ERROR_N = -ROUND_ERROR_P;
        const Obj INFINITE_N    = -INFINITE_P;
        const Obj DENORM_MIN_N  = -DENORM_MIN_P;

        const Obj Q_NAN      = d128_limits::quiet_NaN();
        HashType  Q_NAN_HASH = hasher(Q_NAN);
        const Obj S_NAN      = d128_limits::signaling_NaN();
        HashType  S_NAN_HASH = hasher(S_NAN);

        testData.push_back(TestDataPair(ZERO_P       , hasher(ZERO_P       )));
        testData.push_back(TestDataPair(MIN_P        , hasher(MIN_P        )));
        testData.push_back(TestDataPair(MAX_P        , hasher(MAX_P        )));
        testData.push_back(TestDataPair(EPSILON_P    , hasher(EPSILON_P    )));
        testData.push_back(TestDataPair(ROUND_ERROR_P, hasher(ROUND_ERROR_P)));
        testData.push_back(TestDataPair(INFINITE_P   , hasher(INFINITE_P   )));
        testData.push_back(TestDataPair(DENORM_MIN_P , hasher(DENORM_MIN_P )));
        testData.push_back(TestDataPair(ZERO_N       , hasher(ZERO_N       )));
        testData.push_back(TestDataPair(MIN_N        , hasher(MIN_N        )));
        testData.push_back(TestDataPair(MAX_N        , hasher(MAX_N        )));
        testData.push_back(TestDataPair(EPSILON_N    , hasher(EPSILON_N    )));
        testData.push_back(TestDataPair(ROUND_ERROR_N, hasher(ROUND_ERROR_N)));
        testData.push_back(TestDataPair(INFINITE_N   , hasher(INFINITE_N   )));
        testData.push_back(TestDataPair(DENORM_MIN_N , hasher(DENORM_MIN_N )));

        // Adding identical values, having different representations:
        // 1e+9
        // 10e+8
        // ...
        // 100000000000000000e-8
        // 1000000000000000000e-9

        long long cloneSignificand = 1;
        int       cloneExponent    = 9;
        const Obj ORIGINAL         = BDEC::DecimalImpUtil::makeDecimalRaw128(
                                                              cloneSignificand,
                                                              cloneExponent);
        testData.push_back(TestDataPair(ORIGINAL, hasher(ORIGINAL)));

        for (int i = 0; i < 18; ++i) {
            --cloneExponent;
            cloneSignificand *= 10;
            const Obj CLONE = BDEC::DecimalImpUtil::makeDecimalRaw128(
                                                              cloneSignificand,
                                                              cloneExponent);
            ASSERTV(ORIGINAL, CLONE, ORIGINAL == CLONE);
            testData.push_back(TestDataPair(CLONE, hasher(CLONE)));
        }

        // Adding zero values, having different representations.

        const Obj ZERO1 = BDLDFP_DECIMAL_DL(-0.00);
        const Obj ZERO2 = BDEC::DecimalImpUtil::makeDecimalRaw128(0, 1);
        const Obj ZERO3 = BDEC::DecimalImpUtil::makeDecimalRaw128(0, 10);
        const Obj ZERO4 = BDEC::DecimalImpUtil::makeDecimalRaw128(0, 100);
        const Obj ZERO5 = BDEC::DecimalImpUtil::makeDecimalRaw128(0, 1000);
        const Obj ZERO6 = BDEC::DecimalImpUtil::makeDecimalRaw128(0, 6111);

        testData.push_back(TestDataPair(ZERO1, hasher(ZERO1)));
        testData.push_back(TestDataPair(ZERO2, hasher(ZERO2)));
        testData.push_back(TestDataPair(ZERO3, hasher(ZERO3)));
        testData.push_back(TestDataPair(ZERO4, hasher(ZERO4)));
        testData.push_back(TestDataPair(ZERO5, hasher(ZERO5)));
        testData.push_back(TestDataPair(ZERO6, hasher(ZERO6)));

        // Adding random values.

        const int RANDOM_VALUES_NUM = 100;
        for (int i = 0; i < RANDOM_VALUES_NUM; ++i) {
            const Obj randomValue = randomDecimal128();
            testData.push_back(TestDataPair(randomValue,
                                            hasher(randomValue)));
        }

        if (veryVerbose) {
            T_ T_ P(testData.size());
        }

        // Testing 'hashAppend' function.

        TestDataVector::iterator iter1 = testData.begin();

        while (iter1 != testData.end()) {
            const Obj&               VALUE1 = iter1->first;
            const HashType&          HASH1  = iter1->second;
            TestDataVector::iterator iter2  = testData.begin();

            if (veryVerbose) {
                T_ T_ P_(VALUE1) P(HASH1);
            }

            while (iter2 != testData.end()) {
                const Obj&      VALUE2 = iter2->first;
                const HashType& HASH2  = iter2->second;
                if (VALUE1 == VALUE2) {
                    ASSERTV(VALUE1, VALUE2, HASH1, HASH2, HASH1 == HASH2);
                } else {
                    ASSERTV(VALUE1, VALUE2, HASH1, HASH2, HASH1 != HASH2);
                }
                ++iter2;
            }
            ASSERT(Q_NAN_HASH != HASH1);
            ASSERT(S_NAN_HASH != HASH1);
            ++iter1;
        }
    }
}

void TestDriver::testCase6()
{
    // ------------------------------------------------------------------------
    // TESTING BDEX STREAMING
    //   Verify the BDEX streaming implementation works correctly.  Specific
    //   concerns include wire format, handling of stream states (valid, empty,
    //   invalid, incomplete, and corrupted), and exception neutrality.
    //
    // Concerns:
    //: 1 The class method 'maxSupportedBdexVersion' returns the correct
    //:   version to be used for the specified 'versionSelector'.
    //:
    //: 2 The 'bdexStreamOut' method is callable on a reference providing only
    //:   non-modifiable access.
    //:
    //: 3 For valid streams, externalization and unexternalization are inverse
    //:   operations.
    //:
    //: 4 For invalid streams, externalization leaves the stream invalid and
    //:   unexternalization does not alter the value of the object and leaves
    //:   the stream invalid.
    //:
    //: 5 Unexternalizing of incomplete, invalid, or corrupted data results in
    //:   a valid object of unspecified value and an invalidated stream.
    //:
    //: 6 The wire format of the object is as expected.
    //:
    //: 7 All methods are exception neutral.
    //:
    //: 8 The 'bdexStreamIn' and 'bdexStreamOut' methods return a reference to
    //:   the provided stream in all situations.
    //:
    //: 9 The initial value of the object has no affect on unexternalization.
    //:
    //:10 Streaming version 1 provides the expected compatibility between
    //:   the two calendar modes.
    //
    // Testing:
    //   static int maxSupportedBdexVersion(int versionSelector);
    //   static int maxSupportedBdexVersion();
    //   STREAM& bdexStreamIn(STREAM& stream, int version);
    //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TESTING BDEX STREAMING" << endl
                      << "======================" << endl;
    {
        // Allocator to use instead of the default allocator.
        bslma::TestAllocator allocator("bslx", veryVeryVeryVerbose);

        // Scalar obj
        // ect values used in various stream tests.

        typedef BDEC::Decimal32     Obj;
        typedef bslx::TestInStream  In;
        typedef bslx::TestOutStream Out;

        using namespace bdldfp;

        const Obj VA(BDLDFP_DECIMAL_DF(1.12));
        const Obj VB(BDLDFP_DECIMAL_DF(2.55));
        const Obj VC(BDLDFP_DECIMAL_DF(3.67));
        const Obj VD(BDLDFP_DECIMAL_DF(5.17));
        const Obj VE(BDEC::DecimalImpUtil::parse32("Inf"));
        const Obj VF(BDEC::DecimalImpUtil::parse32("-Inf"));
        const Obj VG(BDLDFP_DECIMAL_DF(-7.64));
        const Obj VH = bsl::numeric_limits<Obj>::min();
        const Obj VI = bsl::numeric_limits<Obj>::max();

        // Array object used in various stream tests.
        const Obj VALUES[]   = { VA, VB, VC, VD, VE, VF, VG, VH, VI };
        const int NUM_VALUES =
            static_cast<int>(sizeof VALUES / sizeof *VALUES);
        const int VERSION_SELECTOR = 20151026;

        // Testing maxSupportedBDexVersion
        {
            ASSERT(1 == Obj::maxSupportedBdexVersion());
            ASSERT(1 == Obj::maxSupportedBdexVersion(0));
            ASSERT(1 == Obj::maxSupportedBdexVersion(
                       VERSION_SELECTOR));

            using bslx::VersionFunctions::maxSupportedBdexVersion;
            ASSERT(1 == maxSupportedBdexVersion(
                       reinterpret_cast<Obj *>(0), 0));
            ASSERT(1 == maxSupportedBdexVersion(
                       reinterpret_cast<Obj *>(0), VERSION_SELECTOR));
        }

        // Direct initial trial of 'bdexStreamOut' and (valid) 'bdexStreamIn'
        // functionality.
        const int VERSION = Obj::maxSupportedBdexVersion(0);
        {
            const Obj X(VC);
            Out out(VERSION_SELECTOR, &allocator);

            Out& rvOut = X.bdexStreamOut(out, VERSION);
            ASSERT(&out == &rvOut);
            ASSERT(out);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            In in(OD, LOD);
            ASSERT(in);
            ASSERT(!in.isEmpty());

            Obj mT(VA);  const Obj& T = mT;
            ASSERT(X != T);

            In& rvIn = mT.bdexStreamIn(in, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(X == T);
            ASSERT(in);
            ASSERT(in.isEmpty());
        }

        // Testing valid streams
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);

                Out out(VERSION_SELECTOR, &allocator);

                using bslx::OutStreamFunctions::bdexStreamOut;
                using bslx::InStreamFunctions::bdexStreamIn;

                Out& rvOut = bdexStreamOut(out, X, VERSION);
                LOOP_ASSERT(i, &out == &rvOut);
                ASSERT(out);
                const char *const OD  = out.data();
                const int         LOD = static_cast<int>(out.length());

                // Verify that each new value overwrites every old value and
                // that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    In in(OD, LOD);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());

                    Obj mT(VALUES[j]);  const Obj& T = mT;
                    LOOP2_ASSERT(i, j, (X == T) == (i == j));

                    BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                        in.reset();
                        In& rvIn = bdexStreamIn(in, mT, VERSION);
                        LOOP2_ASSERT(i, j, &in == &rvIn);
                    } BSLX_TESTINSTREAM_EXCEPTION_TEST_END;

                    LOOP2_ASSERT(i, j, X == T);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        // Testing empty streams (valid and invalid).
        {
            Out               out(VERSION_SELECTOR, &allocator);
            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                using bslx::InStreamFunctions::bdexStreamIn;

                const Obj  X(VALUES[i]);
                Obj        mT(X);
                const Obj& T = mT;
                LOOP_ASSERT(i, X == T);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();

                    // Stream is valid.
                    In& rvIn1 = bdexStreamIn(in, mT, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn1);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);

                    // Stream is invalid.
                    In& rvIn2 = bdexStreamIn(in, mT, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn2);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END;
            }
        }

        // Testing non-empty, invalid streams.

        {
            Out out(VERSION_SELECTOR, &allocator);

            using bslx::OutStreamFunctions::bdexStreamOut;
            Out& rvOut = bdexStreamOut(out, Obj(), VERSION);
            ASSERT(&out == &rvOut);
            ASSERT(out);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            ASSERT(0 < LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                in.invalidate();
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, !in.isEmpty());

                // Ensure that reading from a non-empty, invalid input stream
                // leaves the stream invalid and the target object unchanged.

                using bslx::InStreamFunctions::bdexStreamIn;

                const Obj  X(VALUES[i]);
                Obj        mT(X);
                const Obj& T = mT;
                LOOP_ASSERT(i, X == T);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();
                    in.invalidate();
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, !in.isEmpty());

                    In& rvIn = bdexStreamIn(in, mT, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END;
            }
        }

        // Test wire-format
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);
                bslx::ByteOutStream out(VERSION_SELECTOR, &allocator);

                using bslx::OutStreamFunctions::bdexStreamOut;
                using bslx::InStreamFunctions::bdexStreamIn;

                bdexStreamOut(out, X, VERSION);
                const char *const OD  = out.data();
                const int         LOD = static_cast<int>(out.length());

                bdldfp::DecimalStorage::Type32 bidVal =
                                                       VALUES[i].value().d_raw;

                bsl::uint32_t expectedValue = BSLS_BYTEORDER_HTONL(bidVal);

                ASSERTV(i, X, LOD == 4);
                ASSERT(memcmp(OD, &expectedValue, 4) == 0);
            }
        }
    }

    {
        // Allocator to use instead of the default allocator.
        bslma::TestAllocator allocator("bslx", veryVeryVeryVerbose);

        // Scalar obj
        // ect values used in various stream tests.

        typedef BDEC::Decimal64 Obj;
        typedef bslx::TestInStream In;
        typedef bslx::TestOutStream Out;

        const Obj VA(BDLDFP_DECIMAL_DD(1.12));
        const Obj VB(BDLDFP_DECIMAL_DD(2.55));
        const Obj VC(BDLDFP_DECIMAL_DD(3.67));
        const Obj VD(BDLDFP_DECIMAL_DD(5.17));
        const Obj VE(BDEC::DecimalImpUtil::parse64("Inf"));
        const Obj VF(BDEC::DecimalImpUtil::parse64("-Inf"));
        const Obj VG(BDLDFP_DECIMAL_DD(-7.64));
        const Obj VH = bsl::numeric_limits<Obj>::min();
        const Obj VI = bsl::numeric_limits<Obj>::max();

        // Array object used in various stream tests.
        const Obj VALUES[]   = { VA, VB, VC, VD, VE, VF, VG, VH, VI };
        const int NUM_VALUES =
            static_cast<int>(sizeof VALUES / sizeof *VALUES);
        const int VERSION_SELECTOR = 20151026;

        // Testing maxSupportedBDexVersion
        {
            ASSERT(1 == Obj::maxSupportedBdexVersion());
            ASSERT(1 == Obj::maxSupportedBdexVersion(0));
            ASSERT(1 == Obj::maxSupportedBdexVersion(
                       VERSION_SELECTOR));

            using bslx::VersionFunctions::maxSupportedBdexVersion;
            ASSERT(1 == maxSupportedBdexVersion(
                       reinterpret_cast<Obj *>(0), 0));
            ASSERT(1 == maxSupportedBdexVersion(
                       reinterpret_cast<Obj *>(0), VERSION_SELECTOR));
        }

        // Direct initial trial of 'bdexStreamOut' and (valid) 'bdexStreamIn'
        // functionality.
        const int VERSION = Obj::maxSupportedBdexVersion(0);
        {
            const Obj X(VC);
            Out out(VERSION_SELECTOR, &allocator);

            Out& rvOut = X.bdexStreamOut(out, VERSION);
            ASSERT(&out == &rvOut);
            ASSERT(out);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            In in(OD, LOD);
            ASSERT(in);
            ASSERT(!in.isEmpty());

            Obj mT(VA);  const Obj& T = mT;
            ASSERT(X != T);

            In& rvIn = mT.bdexStreamIn(in, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(X == T);
            ASSERT(in);
            ASSERT(in.isEmpty());
        }

        // Testing valid streams
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);

                Out out(VERSION_SELECTOR, &allocator);

                using bslx::OutStreamFunctions::bdexStreamOut;
                using bslx::InStreamFunctions::bdexStreamIn;

                Out& rvOut = bdexStreamOut(out, X, VERSION);
                LOOP_ASSERT(i, &out == &rvOut);
                LOOP_ASSERT(i, out);
                const char *const OD  = out.data();
                const int         LOD = static_cast<int>(out.length());

                // Verify that each new value overwrites every old value and
                // that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    In in(OD, LOD);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());

                    Obj mT(VALUES[j]);  const Obj& T = mT;
                    LOOP2_ASSERT(i, j, (X == T) == (i == j));

                    BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                        in.reset();
                        In& rvIn = bdexStreamIn(in, mT, VERSION);
                        LOOP2_ASSERT(i, j, &in == &rvIn);
                    } BSLX_TESTINSTREAM_EXCEPTION_TEST_END;

                    LOOP2_ASSERT(i, j, X == T);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        // Testing empty streams (valid and invalid).
        {
            Out               out(VERSION_SELECTOR, &allocator);
            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                using bslx::InStreamFunctions::bdexStreamIn;

                const Obj  X(VALUES[i]);
                Obj        mT(X);
                const Obj& T = mT;
                LOOP_ASSERT(i, X == T);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();

                    // Stream is valid.
                    In& rvIn1 = bdexStreamIn(in, mT, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn1);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);

                    // Stream is invalid.
                    In& rvIn2 = bdexStreamIn(in, mT, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn2);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END;
            }
        }

        // Testing non-empty, invalid streams.

        {
            Out out(VERSION_SELECTOR, &allocator);

            using bslx::OutStreamFunctions::bdexStreamOut;
            Out& rvOut = bdexStreamOut(out, Obj(), VERSION);
            ASSERT(&out == &rvOut);
            ASSERT(out);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            ASSERT(0 < LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                in.invalidate();
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, !in.isEmpty());

                // Ensure that reading from a non-empty, invalid input stream
                // leaves the stream invalid and the target object unchanged.

                using bslx::InStreamFunctions::bdexStreamIn;

                const Obj  X(VALUES[i]);
                Obj        mT(X);
                const Obj& T = mT;
                LOOP_ASSERT(i, X == T);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();
                    in.invalidate();
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, !in.isEmpty());

                    In& rvIn = bdexStreamIn(in, mT, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END;
            }
        }

        // Test wire-format
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);
                bslx::ByteOutStream out(VERSION_SELECTOR, &allocator);

                using bslx::OutStreamFunctions::bdexStreamOut;
                using bslx::InStreamFunctions::bdexStreamIn;

                bdexStreamOut(out, X, VERSION);
                ASSERT(out);

                const char *const OD  = out.data();
                const int         LOD = static_cast<int>(out.length());

                bdldfp::DecimalStorage::Type64 bidVal =
                                                       VALUES[i].value().d_raw;
                bsls::Types::Uint64 expectedValue =
                                                 BSLS_BYTEORDER_HTONLL(bidVal);

                ASSERTV(i, X, LOD == 8);
                ASSERT(memcmp(OD, &expectedValue, 8) == 0);
            }
        }
    }

    {
        // Allocator to use instead of the default allocator.
        bslma::TestAllocator allocator("bslx", veryVeryVeryVerbose);

        // Scalar obj
        // ect values used in various stream tests.

        typedef BDEC::Decimal128    Obj;
        typedef bslx::TestInStream  In;
        typedef bslx::TestOutStream Out;

        const Obj VA(BDLDFP_DECIMAL_DL(1.12));
        const Obj VB(BDLDFP_DECIMAL_DL(2.55));
        const Obj VC(BDLDFP_DECIMAL_DL(3.67));
        const Obj VD(BDLDFP_DECIMAL_DL(5.17));
        const Obj VE(BDEC::DecimalImpUtil::parse128("Inf"));
        const Obj VF(BDEC::DecimalImpUtil::parse128("-Inf"));
        const Obj VG(BDLDFP_DECIMAL_DL(-7.64));
        const Obj VH = bsl::numeric_limits<Obj>::min();
        const Obj VI = bsl::numeric_limits<Obj>::max();

        // Array object used in various stream tests.
        const Obj VALUES[]   = { VA, VB, VC, VD, VE, VF, VG, VH, VI };
        const int NUM_VALUES =
            static_cast<int>(sizeof VALUES / sizeof *VALUES);
        const int VERSION_SELECTOR = 20151026;

        // Testing maxSupportedBDexVersion
        {
            ASSERT(1 == Obj::maxSupportedBdexVersion());
            ASSERT(1 == Obj::maxSupportedBdexVersion(0));
            ASSERT(1 == Obj::maxSupportedBdexVersion(
                       VERSION_SELECTOR));

            using bslx::VersionFunctions::maxSupportedBdexVersion;
            ASSERT(1 == maxSupportedBdexVersion(
                       reinterpret_cast<Obj *>(0), 0));
            ASSERT(1 == maxSupportedBdexVersion(
                       reinterpret_cast<Obj *>(0), VERSION_SELECTOR));
        }

        // Direct initial trial of 'bdexStreamOut' and (valid) 'bdexStreamIn'
        // functionality.
        const int VERSION = Obj::maxSupportedBdexVersion(0);
        {
            const Obj X(VC);
            Out out(VERSION_SELECTOR, &allocator);

            Out& rvOut = X.bdexStreamOut(out, VERSION);
            ASSERT(&out == &rvOut);
            ASSERT(out);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            In in(OD, LOD);
            ASSERT(in);
            ASSERT(!in.isEmpty());

            Obj mT(VA);  const Obj& T = mT;
            ASSERT(X != T);

            In& rvIn = mT.bdexStreamIn(in, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(X == T);
            ASSERT(in);
            ASSERT(in.isEmpty());
        }

        // Testing valid streams
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);

                Out out(VERSION_SELECTOR, &allocator);

                using bslx::OutStreamFunctions::bdexStreamOut;
                using bslx::InStreamFunctions::bdexStreamIn;

                Out& rvOut = bdexStreamOut(out, X, VERSION);
                LOOP_ASSERT(i, &out == &rvOut);
                LOOP_ASSERT(i, out);
                const char *const OD  = out.data();
                const int         LOD = static_cast<int>(out.length());

                // Verify that each new value overwrites every old value and
                // that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    In in(OD, LOD);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());

                    Obj mT(VALUES[j]);  const Obj& T = mT;
                    LOOP2_ASSERT(i, j, (X == T) == (i == j));

                    BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                        in.reset();
                        In& rvIn = bdexStreamIn(in, mT, VERSION);
                        LOOP2_ASSERT(i, j, &in == &rvIn);
                    } BSLX_TESTINSTREAM_EXCEPTION_TEST_END;

                    LOOP2_ASSERT(i, j, X == T);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        // Testing empty streams (valid and invalid).
        {
            Out               out(VERSION_SELECTOR, &allocator);
            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                using bslx::InStreamFunctions::bdexStreamIn;

                const Obj  X(VALUES[i]);
                Obj        mT(X);
                const Obj& T = mT;
                LOOP_ASSERT(i, X == T);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();

                    // Stream is valid.
                    In& rvIn1 = bdexStreamIn(in, mT, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn1);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);

                    // Stream is invalid.
                    In& rvIn2 = bdexStreamIn(in, mT, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn2);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END;
            }
        }

        // Testing non-empty, invalid streams.

        {
            Out out(VERSION_SELECTOR, &allocator);

            using bslx::OutStreamFunctions::bdexStreamOut;
            Out& rvOut = bdexStreamOut(out, Obj(), VERSION);
            ASSERT(&out == &rvOut);
            ASSERT(out);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            ASSERT(0 < LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                in.invalidate();
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, !in.isEmpty());

                // Ensure that reading from a non-empty, invalid input stream
                // leaves the stream invalid and the target object unchanged.

                using bslx::InStreamFunctions::bdexStreamIn;

                const Obj  X(VALUES[i]);
                Obj        mT(X);
                const Obj& T = mT;
                LOOP_ASSERT(i, X == T);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();
                    in.invalidate();
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, !in.isEmpty());

                    In& rvIn = bdexStreamIn(in, mT, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END;
            }
        }

        // Test wire-format
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);
                bslx::ByteOutStream out(VERSION_SELECTOR, &allocator);

                using bslx::OutStreamFunctions::bdexStreamOut;
                using bslx::InStreamFunctions::bdexStreamIn;

                bdexStreamOut(out, X, VERSION);
                ASSERT(out);
                const char *const OD  = out.data();
                const int         LOD = static_cast<int>(out.length());

                bdldfp::DecimalStorage::Type128 bidVal =
                                                       VALUES[i].value().d_raw;

                const int LEN = static_cast<int>(
                                          sizeof(BDEC::DecimalStorage::Type128)
                                          / sizeof(unsigned char));

                ASSERTV(i,   X,   LOD == 16);
                ASSERTV(LOD, LEN, LOD == LEN);

                unsigned char        expected[LEN];
                const unsigned char *value_p =
                                     reinterpret_cast<unsigned char*>(&bidVal);

#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
                for (int i(0); i < LEN; ++i) {
                    expected[i] = *(value_p + i);
                }
#elif defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
                for (int i(0); i < LEN; ++i) {
                    expected[i] = *(value_p + (LEN - 1) - i);
                }
#endif
                ASSERT(memcmp(OD, expected, 16) == 0);
            }
        }
    }

}

void TestDriver::testCase5()
{
    // ------------------------------------------------------------------------
    // TESTING PRINT METHOD
    //
    // Concerns:
    //: 1 The 'print' method writes the value to the specified 'ostream'.
    //:
    //: 2 The 'print' method write the value in the intended format.
    //:
    //: 3 The 'print' method uses the same underlying formatting function as
    //:   'operator<<'.
    //:
    //: 4 The 'print' method's signature and return type are standard.
    //:
    //: 5 The 'print' method returns the supplied 'ostream'.
    //:
    //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
    //:   correct default values (0 and 4, respectively).
    //
    // Testing:
    //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
    // ------------------------------------------------------------------------

    if (verbose) bsl::cout << "\nTESTING PRINT METHOD"
                           << "\n===================="
                           << bsl::endl;

    // Verify that the signatures and return types are standard.
    {
        typedef bsl::ostream& (BDEC::Decimal64::*funcPtr)(bsl::ostream&,
                                                          int, int) const;

        funcPtr     printMember = &BDEC::Decimal64::print;

        (void)printMember;
    }

#define DFP(X) BDLDFP_DECIMAL_DD(X)
    static const struct {
        int              d_line;
        BDEC::Decimal64  d_decimalValue;
        int              d_level;
        int              d_spacesPerLevel;
        const char      *d_expected_p;
    } DATA[] = {

        // -8 implies using default values

        //LINE  NUMBER     LEVEL  SPL  EXPECTED
        //----  ---------  -----  ---  --------
        { L_,  DFP(4.25),     0,   0,  "4.25\n" },
        { L_,  DFP(4.25),     0,   1,  "4.25\n" },
        { L_,  DFP(4.25),     0,  -1,  "4.25"   },
        { L_,  DFP(4.25),     0,  -8,  "4.25\n" },

        { L_,  DFP(4.25),     3,   0,  "4.25\n" },
        { L_,  DFP(4.25),     3,   2,  "      4.25\n" },
        { L_,  DFP(4.25),     3,  -2,  "      4.25" },
        { L_,  DFP(4.25),     3,  -8,  "            4.25\n" },
        { L_,  DFP(4.25),    -3,   0,  "4.25\n" },
        { L_,  DFP(4.25),    -3,   2,  "4.25\n" },
        { L_,  DFP(4.25),    -3,  -2,  "4.25" },
        { L_,  DFP(4.25),    -3,  -8,  "4.25\n" },

        { L_,  DFP(4.25),    -8,  -8,  "4.25\n" },
    };

    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int             LINE     = DATA[ti].d_line;
        const BDEC::Decimal64 VALUE    = DATA[ti].d_decimalValue;
        const int             LEVEL    = DATA[ti].d_level;
        const int             SPL      = DATA[ti].d_spacesPerLevel;
        const char*           EXPECTED = DATA[ti].d_expected_p;

        bsl::stringstream outdec;

        outdec << bsl::setprecision(2) << bsl::fixed;
        if (-8 != SPL) {
            VALUE.print(outdec, LEVEL, SPL);
        }
        else if (-8 != LEVEL) {
            VALUE.print(outdec, LEVEL);
        }
        else {
            VALUE.print(outdec);
        }
        bsl::string ACTUAL = outdec.str();

        ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
    }
#undef DFP
}

void TestDriver::testCase4()
{
    // ------------------------------------------------------------------------
    // TESTING IOSTREAM OPERATORS
    //
    // Concerns:
    //: 1 Calling 'operator<<' on a 'Decimal32', 'Decimal64', 'Decimal128' type
    //:   renders the decimals value to the appropriate stream.
    //:
    //: 2 That 'operator<<' renders a simple decimal value in a fixed point
    //:   and scientific format using the correct digits of precision.
    //:
    //: 3 That 'operator<<' correctly renders infinity, and negative infinity.
    //:
    //: 4 That 'operator<<' correctly renders quiet and signaling NaN.
    //:
    //: 5 That 'operator<<' correctly handles a set width.
    //:
    //: 6 That 'operator<<' correctly handles 'bsl::fixed' manipulator.
    //:
    //: 7 That 'operator<<' correctly handles 'bsl::scientific' manipulator.
    //:
    //: 8 That 'operator<<' correctly handles 'bsl::setprecision' manipulator.
    //:
    //: 9 That 'operator<<' correctly handles 'bsl::showpos' manipulator.
    //:
    //:10 That 'operator<<' correctly handles a set width with either a left,
    //:   internal, or right justification.
    //:
    //:11 That 'operator<<' correctly handles 'bsl::uppercase'.
    //:
    //:12 That 'operator<<' correctly handles 'bsl::fill' manipulator.
    //:
    //:13 That if a precision provided by 'bsl::setprecision' manipulator is
    //:   less than precision of a decimal value then the number being written
    //:   is "rounded-half-up" to fewer digits than it initially contains.
    //:
    //:14 That if a precision provided by 'bsl::setprecision' manipulator is
    //:   bigger than precision of a decimal value then the number being
    //:   written is filled up with trailing 0.
    //:
    //:15 That the 'print' method writes the value to the specified 'ostream'.
    //:
    //:16 That the 'print' method write the vlaue in the intended format.
    //:
    //:17 That the output using 's << obj' is the same as
    //:   'obj.print(s, 0, -1)'.
    //:
    //:18 That 'operator<<' sets the fail and errors bit if the memory buffer
    //:   in the supplied output stream is not large enough.
    //:
    //:19 That 'operator<<' correctly outputs decimal values to wide stream.
    //:
    //:20 That 'operator<<' outputs decimal values in natural notation by
    //:   default.
    //
    // Plan:
    //  1 Create a test table, where each element contains a decimal value
    //    (including infinity, signaling and quiet NaN values), a set of
    //    formatting flags, and an expected output.  Iterate over the test
    //    table for 'Decimal32', 'Decimal64', and 'Decimal128' types, and
    //    ensure the streamed output matches the expected value.
    //
    //  2 Create a test table, where each element contains max, min or
    //    denormalized decimal value, a set of formatting flags and a length of
    //    expected output.  Make sure that length of the resultant string
    //    equals to the expected one.  Make sure that a decimal value restored
    //    from the output equals to the tested value.
    //
    //  3 Stream out a value to an output stream with a fixed memory buffer
    //    that is not large enough.  Make sure that the bad and fail bits are
    //    set in the output stream.
    //
    //  4 Stream out arbitrary decimal values to wide stream.  Ensure the
    //    streamed output matches the expected value.  (C-19)
    //
    // Testing:
    //   bsl::basic_ostream& operator<<(bsl::basic_ostream& , Decimal32 );
    //   bsl::basic_ostream& operator<<(bsl::basic_ostream& , Decimal64 );
    //   bsl::basic_ostream& operator<<(bsl::basic_ostream& , Decimal128 );
    // ------------------------------------------------------------------------

#define DFP(X) BDLDFP_DECIMAL_DF(X)

    if (verbose) bsl::cout << "\nTESTING IOSTREAM OPERATORS"
                           << "\n=========================="
                           << bsl::endl;

    if (veryVerbose) bsl::cout << "\tTesting decimal formatting in fixed "
                               << "notation"
                               << bsl::endl;
    {
        static const struct {
            int              d_line;
            BDEC::Decimal32  d_decimalValue;
            int              d_precision;
            int              d_width;
            char             d_fill;
            bool             d_showPos;
            char             d_justification;
            const char      *d_expected;
        } DATA[] = {
            //-----------------------------------------------------------------
            // L  |NUMBER    |PRS |WIDTH |FILL |SHOW_POS |JUST |EXPECTED
            //-----------------------------------------------------------------
            //-----------------------------------------------------------------
            // Test negative and positive decimal rendering with all possible
            // justification values in fixed notation.
            // C-10
            {  L_, DFP( 4.25),  2,    8,   ' ',  false,   'l',  "4.25    "  },
            {  L_, DFP( 4.25),  2,    8,   ' ',  false,   'i',  "    4.25"  },
            {  L_, DFP( 4.25),  2,    8,   ' ',  false,   'r',  "    4.25"  },

            {  L_, DFP(-4.25),  2,    8,   ' ',  false,   'l',  "-4.25   "  },
            {  L_, DFP(-4.25),  2,    8,   ' ',  false,   'i',  "-   4.25"  },
            {  L_, DFP(-4.25),  2,    8,   ' ',  false,   'r',  "   -4.25"  },

            //-----------------------------------------------------------------
            // Update 'd_showPos' attribute to test 'bsl::showpos' manipulator
            // with all possible justification values.
            // C-9
            {  L_, DFP( 4.25),  2,    8,   ' ',  true,    'l',  "+4.25   "  },
            {  L_, DFP( 4.25),  2,    8,   ' ',  true,    'i',  "+   4.25"  },
            {  L_, DFP( 4.25),  2,    8,   ' ',  true,    'r',  "   +4.25"  },

            {  L_, DFP(-4.25),  2,    8,   ' ',  true,    'l',  "-4.25   "  },
            {  L_, DFP(-4.25),  2,    8,   ' ',  true,    'i',  "-   4.25"  },
            {  L_, DFP(-4.25),  2,    8,   ' ',  true,    'r',  "   -4.25"  },

            //-----------------------------------------------------------------
            // Update 'd_fill' attribute to test 'bsl::fill' manipulator with
            // combination of 'showpos' and justification manipulators.
            // C-12
            {  L_, DFP( 4.25),  2,    8,   '_',  false,   'l',  "4.25____"  },
            {  L_, DFP( 4.25),  2,    8,   '_',  false,   'i',  "____4.25"  },
            {  L_, DFP( 4.25),  2,    8,   '_',  false,   'r',  "____4.25"  },

            {  L_, DFP(-4.25),  2,    8,   '_',  false,   'l',  "-4.25___"  },
            {  L_, DFP(-4.25),  2,    8,   '_',  false,   'i',  "-___4.25"  },
            {  L_, DFP(-4.25),  2,    8,   '_',  false,   'r',  "___-4.25"  },

            {  L_, DFP( 4.25),  2,    8,   '_',  true,    'l',  "+4.25___"  },
            {  L_, DFP( 4.25),  2,    8,   '_',  true,    'i',  "+___4.25"  },
            {  L_, DFP( 4.25),  2,    8,   '_',  true,    'r',  "___+4.25"  },

            {  L_, DFP(-4.25),  2,    8,   '_',  true,    'l',  "-4.25___"  },
            {  L_, DFP(-4.25),  2,    8,   '_',  true,    'i',  "-___4.25"  },
            {  L_, DFP(-4.25),  2,    8,   '_',  true,    'r',  "___-4.25"  },

            //-----------------------------------------------------------------
            // Update 'd_width' attribute to test 'bsl::width' manipulator with
            // combination of 'fill', 'showpos' and justification manipulators.
            // C-5
            {  L_, DFP( 4.25),  2,    9,   ' ',  false,   'l',  "4.25     " },
            {  L_, DFP( 4.25),  2,    9,   ' ',  false,   'i',  "     4.25" },
            {  L_, DFP( 4.25),  2,    9,   ' ',  false,   'r',  "     4.25" },

            {  L_, DFP(-4.25),  2,    9,   ' ',  false,   'l',  "-4.25    " },
            {  L_, DFP(-4.25),  2,    9,   ' ',  false,   'i',  "-    4.25" },
            {  L_, DFP(-4.25),  2,    9,   ' ',  false,   'r',  "    -4.25" },

            {  L_, DFP( 4.25),  2,    9,   ' ',  true,    'l',  "+4.25    " },
            {  L_, DFP( 4.25),  2,    9,   ' ',  true,    'i',  "+    4.25" },
            {  L_, DFP( 4.25),  2,    9,   ' ',  true,    'r',  "    +4.25" },

            {  L_, DFP(-4.25),  2,    9,   ' ',  true,    'l',  "-4.25    " },
            {  L_, DFP(-4.25),  2,    9,   ' ',  true,    'i',  "-    4.25" },
            {  L_, DFP(-4.25),  2,    9,   ' ',  true,    'r',  "    -4.25" },

            {  L_, DFP( 4.25),  2,    9,   '_',  false,   'l',  "4.25_____" },
            {  L_, DFP( 4.25),  2,    9,   '_',  false,   'i',  "_____4.25" },
            {  L_, DFP( 4.25),  2,    9,   '_',  false,   'r',  "_____4.25" },

            {  L_, DFP(-4.25),  2,    9,   '_',  false,   'l',  "-4.25____" },
            {  L_, DFP(-4.25),  2,    9,   '_',  false,   'i',  "-____4.25" },
            {  L_, DFP(-4.25),  2,    9,   '_',  false,   'r',  "____-4.25" },

            {  L_, DFP( 4.25),  2,    9,   '_',  true,    'l',  "+4.25____" },
            {  L_, DFP( 4.25),  2,    9,   '_',  true,    'i',  "+____4.25" },
            {  L_, DFP( 4.25),  2,    9,   '_',  true,    'r',  "____+4.25" },

            {  L_, DFP(-4.25),  2,    9,   '_',  true,    'l',  "-4.25____" },
            {  L_, DFP(-4.25),  2,    9,   '_',  true,    'i',  "-____4.25" },
            {  L_, DFP(-4.25),  2,    9,   '_',  true,    'r',  "____-4.25" },

            //-----------------------------------------------------------------
            // Update 'd_precision' attribute to test 'bsl::setprecision'
            // manipulator  with combination of 'width', 'fill', 'showpos' and
            // justification manipulators.
            // C-8
            {  L_, DFP( 4.25),  3,    8,   ' ',  false,   'l',  "4.250   "  },
            {  L_, DFP( 4.25),  3,    8,   ' ',  false,   'i',  "   4.250"  },
            {  L_, DFP( 4.25),  3,    8,   ' ',  false,   'r',  "   4.250"  },

            {  L_, DFP(-4.25),  3,    8,   ' ',  false,   'l',  "-4.250  "  },
            {  L_, DFP(-4.25),  3,    8,   ' ',  false,   'i',  "-  4.250"  },
            {  L_, DFP(-4.25),  3,    8,   ' ',  false,   'r',  "  -4.250"  },

            {  L_, DFP( 4.25),  3,    8,   ' ',  true,    'l',  "+4.250  "  },
            {  L_, DFP( 4.25),  3,    8,   ' ',  true,    'i',  "+  4.250"  },
            {  L_, DFP( 4.25),  3,    8,   ' ',  true,    'r',  "  +4.250"  },

            {  L_, DFP(-4.25),  3,    8,   ' ',  true,    'l',  "-4.250  "  },
            {  L_, DFP(-4.25),  3,    8,   ' ',  true,    'i',  "-  4.250"  },
            {  L_, DFP(-4.25),  3,    8,   ' ',  true,    'r',  "  -4.250"  },

            {  L_, DFP( 4.25),  3,    8,   '_',  false,   'l',  "4.250___"  },
            {  L_, DFP( 4.25),  3,    8,   '_',  false,   'i',  "___4.250"  },
            {  L_, DFP( 4.25),  3,    8,   '_',  false,   'r',  "___4.250"  },

            {  L_, DFP(-4.25),  3,    8,   '_',  false,   'l',  "-4.250__"  },
            {  L_, DFP(-4.25),  3,    8,   '_',  false,   'i',  "-__4.250"  },
            {  L_, DFP(-4.25),  3,    8,   '_',  false,   'r',  "__-4.250"  },

            {  L_, DFP( 4.25),  3,    8,   '_',  true,    'l',  "+4.250__"  },
            {  L_, DFP( 4.25),  3,    8,   '_',  true,    'i',  "+__4.250"  },
            {  L_, DFP( 4.25),  3,    8,   '_',  true,    'r',  "__+4.250"  },

            {  L_, DFP(-4.25),  3,    8,   '_',  true,    'l',  "-4.250__"  },
            {  L_, DFP(-4.25),  3,    8,   '_',  true,    'i',  "-__4.250"  },
            {  L_, DFP(-4.25),  3,    8,   '_',  true,    'r',  "__-4.250"  },

            {  L_, DFP( 4.25),  3,    9,   ' ',  false,   'l',  "4.250    " },
            {  L_, DFP( 4.25),  3,    9,   ' ',  false,   'i',  "    4.250" },
            {  L_, DFP( 4.25),  3,    9,   ' ',  false,   'r',  "    4.250" },

            {  L_, DFP(-4.25),  3,    9,   ' ',  false,   'l',  "-4.250   " },
            {  L_, DFP(-4.25),  3,    9,   ' ',  false,   'i',  "-   4.250" },
            {  L_, DFP(-4.25),  3,    9,   ' ',  false,   'r',  "   -4.250" },

            {  L_, DFP( 4.25),  3,    9,   ' ',  true,    'l',  "+4.250   " },
            {  L_, DFP( 4.25),  3,    9,   ' ',  true,    'i',  "+   4.250" },
            {  L_, DFP( 4.25),  3,    9,   ' ',  true,    'r',  "   +4.250" },

            {  L_, DFP(-4.25),  3,    9,   ' ',  true,    'l',  "-4.250   " },
            {  L_, DFP(-4.25),  3,    9,   ' ',  true,    'i',  "-   4.250" },
            {  L_, DFP(-4.25),  3,    9,   ' ',  true,    'r',  "   -4.250" },

            {  L_, DFP( 4.25),  3,    9,   '_',  false,   'l',  "4.250____" },
            {  L_, DFP( 4.25),  3,    9,   '_',  false,   'i',  "____4.250" },
            {  L_, DFP( 4.25),  3,    9,   '_',  false,   'r',  "____4.250" },

            {  L_, DFP(-4.25),  3,    9,   '_',  false,   'l',  "-4.250___" },
            {  L_, DFP(-4.25),  3,    9,   '_',  false,   'i',  "-___4.250" },
            {  L_, DFP(-4.25),  3,    9,   '_',  false,   'r',  "___-4.250" },

            {  L_, DFP( 4.25),  3,    9,   '_',  true,    'l',  "+4.250___" },
            {  L_, DFP( 4.25),  3,    9,   '_',  true,    'i',  "+___4.250" },
            {  L_, DFP( 4.25),  3,    9,   '_',  true,    'r',  "___+4.250" },

            {  L_, DFP(-4.25),  3,    9,   '_',  true,    'l',  "-4.250___" },
            {  L_, DFP(-4.25),  3,    9,   '_',  true,    'i',  "-___4.250" },
            {  L_, DFP(-4.25),  3,    9,   '_',  true,    'r',  "___-4.250" },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        const bool UPPERCASE[] = { false, true };

        const int NUM_UPPERCASE = static_cast<int>(sizeof UPPERCASE
                                                   / sizeof *UPPERCASE);

        typedef BDEC::Decimal32 Type32;

        for (int tj = 0; tj < NUM_UPPERCASE; ++tj) {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int     LINE       = DATA[ti].d_line;
                const Type32  DECIMAL32  = DATA[ti].d_decimalValue;
                const int     PRECISION  = DATA[ti].d_precision;
                const int     WIDTH      = DATA[ti].d_width;
                const char    FILL       = DATA[ti].d_fill;
                const bool    SHOW_POS   = DATA[ti].d_showPos;
                const bool    LEFT       = (DATA[ti].d_justification == 'l');
                const bool    INTERNAL   = (DATA[ti].d_justification == 'i');
                const bool    RIGHT      = (DATA[ti].d_justification == 'r');
                const char   *EXPECTED   = DATA[ti].d_expected;

                if (veryVeryVerbose) {
                    P_(LINE); P_(EXPECTED); P(DECIMAL32);
                }

                // Test with Decimal32.  C-1
                {
                    const BDEC::Decimal32 VALUE(DECIMAL32);

                    bsl::ostringstream outdec(pa);

                    if (UPPERCASE[tj]) {
                        // C-11
                        outdec << bsl::uppercase;
                    }

                    outdec << bsl::fixed;
                    outdec << bsl::setprecision(PRECISION);
                    outdec.width(WIDTH);
                    outdec.fill(FILL);

                    if (SHOW_POS) { outdec << bsl::showpos;  }
                    if (LEFT)     { outdec << bsl::left;     }
                    if (INTERNAL) { outdec << bsl::internal; }
                    if (RIGHT)    { outdec << bsl::right;    }
                    outdec << VALUE;

                    bsl::string ACTUAL(pa);
                    getStringFromStream(outdec, &ACTUAL);

                    ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
                    ASSERTV(outdec.good());
                }

                // Test with Decimal64.  C-1
                {
                    const BDEC::Decimal64 VALUE(DECIMAL32);

                    bsl::ostringstream outdec(pa);

                    if (UPPERCASE[tj]) {
                        // C-11
                        outdec << bsl::uppercase;
                    }

                    outdec << bsl::fixed;
                    outdec << bsl::setprecision(PRECISION);
                    outdec.width(WIDTH);
                    outdec.fill(FILL);

                    if (SHOW_POS) { outdec << bsl::showpos;  }
                    if (LEFT)     { outdec << bsl::left;     }
                    if (INTERNAL) { outdec << bsl::internal; }
                    if (RIGHT)    { outdec << bsl::right;    }
                    outdec << VALUE;

                    bsl::string ACTUAL(pa);
                    getStringFromStream(outdec, &ACTUAL);

                    ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
                    ASSERTV(outdec.good());
                }

                // Test with print.  C-15..17
                {
                    const BDEC::Decimal64 VALUE(DECIMAL32);

                    bsl::ostringstream outdec(pa);

                    if (UPPERCASE[tj]) {
                        // C-11
                        outdec << bsl::uppercase;
                    }

                    outdec << bsl::fixed;
                    outdec << bsl::setprecision(PRECISION);
                    outdec.width(WIDTH);
                    outdec.fill(FILL);

                    if (SHOW_POS) { outdec << bsl::showpos;  }
                    if (LEFT)     { outdec << bsl::left;     }
                    if (INTERNAL) { outdec << bsl::internal; }
                    if (RIGHT)    { outdec << bsl::right;    }
                    VALUE.print(outdec, 0, -1);

                    bsl::string ACTUAL(pa);
                    getStringFromStream(outdec, &ACTUAL);

                    ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
                }

                // Test with Decimal128.  C-1
                {
                    const BDEC::Decimal128 VALUE(DECIMAL32);

                    bsl::ostringstream outdec(pa);

                    outdec << bsl::fixed;
                    outdec << bsl::setprecision(PRECISION);
                    outdec.width(WIDTH);
                    outdec.fill(FILL);

                    if (SHOW_POS) { outdec << bsl::showpos;  }
                    if (LEFT)     { outdec << bsl::left;     }
                    if (INTERNAL) { outdec << bsl::internal; }
                    if (RIGHT)    { outdec << bsl::right;    }
                    outdec << VALUE;

                    bsl::string ACTUAL(pa);
                    getStringFromStream(outdec, &ACTUAL);

                    ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
                    ASSERTV(outdec.good());
                }
            }
        }
    }
    {
        static const struct {
            int              d_line;
            BDEC::Decimal32  d_decimalValue;
            int              d_precision;
            int              d_width;
            char             d_fill;
            bool             d_showPos;
            bool             d_uppercase;
            char             d_justification;
            const char      *d_expected;
        } DATA[] = {
     //------------------------------------------------------------------------
     // L  |NUMBER    |PRS |WIDTH |FILL |SHOW_POS |UPPER |JUST |EXPECTED
     //------------------------------------------------------------------------
     //------------------------------------------------------------------------
     // Test negative and positive decimal rendering with all possible
     // justification values in scientific notation.
     // C-10
     //------------------------------------------------------------------------
     {  L_, DFP( 0.42),  1,    8,   ' ',  false,   false,  'l',  "4.2e-01 "  },
     {  L_, DFP( 0.42),  1,    8,   ' ',  false,   false,  'i',  " 4.2e-01"  },
     {  L_, DFP( 0.42),  1,    8,   ' ',  false,   false,  'r',  " 4.2e-01"  },

     {  L_, DFP(-0.42),  1,    8,   ' ',  false,   false,  'l',  "-4.2e-01"  },
     {  L_, DFP(-0.42),  1,    8,   ' ',  false,   false,  'i',  "-4.2e-01"  },
     {  L_, DFP(-0.42),  1,    8,   ' ',  false,   false,  'r',  "-4.2e-01"  },
     //------------------------------------------------------------------------
     // Update 'd_uppercase' attribute to test 'bsl::uppercase' manipulator
     // with all possible justification values.
     // C-9
     //------------------------------------------------------------------------
     {  L_, DFP( 0.42),  1,    8,   ' ',  false,   true,   'l',  "4.2E-01 "  },
     {  L_, DFP( 0.42),  1,    8,   ' ',  false,   true,   'i',  " 4.2E-01"  },
     {  L_, DFP( 0.42),  1,    8,   ' ',  false,   true,   'r',  " 4.2E-01"  },

     {  L_, DFP(-0.42),  1,    8,   ' ',  false,   true,   'l',  "-4.2E-01"  },
     {  L_, DFP(-0.42),  1,    8,   ' ',  false,   true,   'i',  "-4.2E-01"  },
     {  L_, DFP(-0.42),  1,    8,   ' ',  false,   true,   'r',  "-4.2E-01"  },
     //------------------------------------------------------------------------
     // Update 'd_showPos' attribute to test 'bsl::showpos' manipulator
     // with all possible justification values and 'uppercase' manipulator.
     // C-9
     //------------------------------------------------------------------------
     {  L_, DFP( 0.42),  1,    8,   ' ',  true,    false,  'l',  "+4.2e-01"  },
     {  L_, DFP( 0.42),  1,    8,   ' ',  true,    false,  'i',  "+4.2e-01"  },
     {  L_, DFP( 0.42),  1,    8,   ' ',  true,    false,  'r',  "+4.2e-01"  },

     {  L_, DFP(-0.42),  1,    8,   ' ',  true,    false,  'l',  "-4.2e-01"  },
     {  L_, DFP(-0.42),  1,    8,   ' ',  true,    false,  'i',  "-4.2e-01"  },
     {  L_, DFP(-0.42),  1,    8,   ' ',  true,    false,  'r',  "-4.2e-01"  },

     {  L_, DFP( 0.42),  1,    8,   ' ',  true,    true,   'l',  "+4.2E-01"  },
     {  L_, DFP( 0.42),  1,    8,   ' ',  true,    true,   'i',  "+4.2E-01"  },
     {  L_, DFP( 0.42),  1,    8,   ' ',  true,    true,   'r',  "+4.2E-01"  },

     {  L_, DFP(-0.42),  1,    8,   ' ',  true,    true,   'l',  "-4.2E-01"  },
     {  L_, DFP(-0.42),  1,    8,   ' ',  true,    true,   'i',  "-4.2E-01"  },
     {  L_, DFP(-0.42),  1,    8,   ' ',  true,    true,   'r',  "-4.2E-01"  },
     //------------------------------------------------------------------------
     // Update 'd_fill' attribute to test 'bsl::fill' manipulator with
     // combination of 'showpos', 'uppercase' and justification manipulators.
     // C-12
     //------------------------------------------------------------------------
     {  L_, DFP( 0.42),  1,    8,   '_',  false,   false,  'l',  "4.2e-01_"  },
     {  L_, DFP( 0.42),  1,    8,   '_',  false,   false,  'i',  "_4.2e-01"  },
     {  L_, DFP( 0.42),  1,    8,   '_',  false,   false,  'r',  "_4.2e-01"  },

     {  L_, DFP(-0.42),  1,    8,   '_',  false,   false,  'l',  "-4.2e-01"  },
     {  L_, DFP(-0.42),  1,    8,   '_',  false,   false,  'i',  "-4.2e-01"  },
     {  L_, DFP(-0.42),  1,    8,   '_',  false,   false,  'r',  "-4.2e-01"  },

     {  L_, DFP( 0.42),  1,    8,   '_',  false,   true,   'l',  "4.2E-01_"  },
     {  L_, DFP( 0.42),  1,    8,   '_',  false,   true,   'i',  "_4.2E-01"  },
     {  L_, DFP( 0.42),  1,    8,   '_',  false,   true,   'r',  "_4.2E-01"  },

     {  L_, DFP(-0.42),  1,    8,   '_',  false,   true,   'l',  "-4.2E-01"  },
     {  L_, DFP(-0.42),  1,    8,   '_',  false,   true,   'i',  "-4.2E-01"  },
     {  L_, DFP(-0.42),  1,    8,   '_',  false,   true,   'r',  "-4.2E-01"  },

     {  L_, DFP( 0.42),  1,    8,   '_',  true,    false,  'l',  "+4.2e-01"  },
     {  L_, DFP( 0.42),  1,    8,   '_',  true,    false,  'i',  "+4.2e-01"  },
     {  L_, DFP( 0.42),  1,    8,   '_',  true,    false,  'r',  "+4.2e-01"  },

     {  L_, DFP(-0.42),  1,    8,   '_',  true,    false,  'l',  "-4.2e-01"  },
     {  L_, DFP(-0.42),  1,    8,   '_',  true,    false,  'i',  "-4.2e-01"  },
     {  L_, DFP(-0.42),  1,    8,   '_',  true,    false,  'r',  "-4.2e-01"  },

     {  L_, DFP( 0.42),  1,    8,   '_',  true,    true,   'l',  "+4.2E-01"  },
     {  L_, DFP( 0.42),  1,    8,   '_',  true,    true,   'i',  "+4.2E-01"  },
     {  L_, DFP( 0.42),  1,    8,   '_',  true,    true,   'r',  "+4.2E-01"  },

     {  L_, DFP(-0.42),  1,    8,   '_',  true,    true,   'l',  "-4.2E-01"  },
     {  L_, DFP(-0.42),  1,    8,   '_',  true,    true,   'i',  "-4.2E-01"  },
     {  L_, DFP(-0.42),  1,    8,   '_',  true,    true,   'r',  "-4.2E-01"  },
     //------------------------------------------------------------------------
     // Update 'd_width' attribute to test 'bsl::width' manipulator with
     // combination of 'fill', 'showpos', 'uppercase' and justification
     // manipulators.  C-5
     //------------------------------------------------------------------------
     {  L_, DFP( 0.42),  1,    9,   ' ',  false,   false,  'l',  "4.2e-01  " },
     {  L_, DFP( 0.42),  1,    9,   ' ',  false,   false,  'i',  "  4.2e-01" },
     {  L_, DFP( 0.42),  1,    9,   ' ',  false,   false,  'r',  "  4.2e-01" },

     {  L_, DFP(-0.42),  1,    9,   ' ',  false,   false,  'l',  "-4.2e-01 " },
     {  L_, DFP(-0.42),  1,    9,   ' ',  false,   false,  'i',  "- 4.2e-01" },
     {  L_, DFP(-0.42),  1,    9,   ' ',  false,   false,  'r',  " -4.2e-01" },

     {  L_, DFP( 0.42),  1,    9,   ' ',  false,   true,   'l',  "4.2E-01  " },
     {  L_, DFP( 0.42),  1,    9,   ' ',  false,   true,   'i',  "  4.2E-01" },
     {  L_, DFP( 0.42),  1,    9,   ' ',  false,   true,   'r',  "  4.2E-01" },

     {  L_, DFP(-0.42),  1,    9,   ' ',  false,   true,   'l',  "-4.2E-01 " },
     {  L_, DFP(-0.42),  1,    9,   ' ',  false,   true,   'i',  "- 4.2E-01" },
     {  L_, DFP(-0.42),  1,    9,   ' ',  false,   true,   'r',  " -4.2E-01" },

     {  L_, DFP( 0.42),  1,    9,   ' ',  true,    false,  'l',  "+4.2e-01 " },
     {  L_, DFP( 0.42),  1,    9,   ' ',  true,    false,  'i',  "+ 4.2e-01" },
     {  L_, DFP( 0.42),  1,    9,   ' ',  true,    false,  'r',  " +4.2e-01" },

     {  L_, DFP(-0.42),  1,    9,   ' ',  true,    false,  'l',  "-4.2e-01 " },
     {  L_, DFP(-0.42),  1,    9,   ' ',  true,    false,  'i',  "- 4.2e-01" },
     {  L_, DFP(-0.42),  1,    9,   ' ',  true,    false,  'r',  " -4.2e-01" },

     {  L_, DFP( 0.42),  1,    9,   ' ',  true,    true,   'l',  "+4.2E-01 " },
     {  L_, DFP( 0.42),  1,    9,   ' ',  true,    true,   'i',  "+ 4.2E-01" },
     {  L_, DFP( 0.42),  1,    9,   ' ',  true,    true,   'r',  " +4.2E-01" },

     {  L_, DFP(-0.42),  1,    9,   ' ',  true,    true,   'l',  "-4.2E-01 " },
     {  L_, DFP(-0.42),  1,    9,   ' ',  true,    true,   'i',  "- 4.2E-01" },
     {  L_, DFP(-0.42),  1,    9,   ' ',  true,    true,   'r',  " -4.2E-01" },

     {  L_, DFP( 0.42),  1,    9,   'x',  false,   false,  'l',  "4.2e-01xx" },
     {  L_, DFP( 0.42),  1,    9,   'x',  false,   false,  'i',  "xx4.2e-01" },
     {  L_, DFP( 0.42),  1,    9,   'x',  false,   false,  'r',  "xx4.2e-01" },

     {  L_, DFP(-0.42),  1,    9,   'x',  false,   false,  'l',  "-4.2e-01x" },
     {  L_, DFP(-0.42),  1,    9,   'x',  false,   false,  'i',  "-x4.2e-01" },
     {  L_, DFP(-0.42),  1,    9,   'x',  false,   false,  'r',  "x-4.2e-01" },

     {  L_, DFP( 0.42),  1,    9,   'x',  false,   true,   'l',  "4.2E-01xx" },
     {  L_, DFP( 0.42),  1,    9,   'x',  false,   true,   'i',  "xx4.2E-01" },
     {  L_, DFP( 0.42),  1,    9,   'x',  false,   true,   'r',  "xx4.2E-01" },

     {  L_, DFP(-0.42),  1,    9,   'x',  false,   true,   'l',  "-4.2E-01x" },
     {  L_, DFP(-0.42),  1,    9,   'x',  false,   true,   'i',  "-x4.2E-01" },
     {  L_, DFP(-0.42),  1,    9,   'x',  false,   true,   'r',  "x-4.2E-01" },

     {  L_, DFP( 0.42),  1,    9,   'x',  true,    false,  'l',  "+4.2e-01x" },
     {  L_, DFP( 0.42),  1,    9,   'x',  true,    false,  'i',  "+x4.2e-01" },
     {  L_, DFP( 0.42),  1,    9,   'x',  true,    false,  'r',  "x+4.2e-01" },

     {  L_, DFP(-0.42),  1,    9,   'x',  true,    false,  'l',  "-4.2e-01x" },
     {  L_, DFP(-0.42),  1,    9,   'x',  true,    false,  'i',  "-x4.2e-01" },
     {  L_, DFP(-0.42),  1,    9,   'x',  true,    false,  'r',  "x-4.2e-01" },

     {  L_, DFP( 0.42),  1,    9,   'x',  true,    true,   'l',  "+4.2E-01x" },
     {  L_, DFP( 0.42),  1,    9,   'x',  true,    true,   'i',  "+x4.2E-01" },
     {  L_, DFP( 0.42),  1,    9,   'x',  true,    true,   'r',  "x+4.2E-01" },

     {  L_, DFP(-0.42),  1,    9,   'x',  true,    true,   'l',  "-4.2E-01x" },
     {  L_, DFP(-0.42),  1,    9,   'x',  true,    true,   'i',  "-x4.2E-01" },
     {  L_, DFP(-0.42),  1,    9,   'x',  true,    true,   'r',  "x-4.2E-01" },
     //------------------------------------------------------------------------
     // Update 'd_precision' attribute to test 'bsl::setprecision'
     // manipulator  with combination of 'width', 'fill', 'showpos',
     // 'uppercase' and justification manipulators.
     // C-8
     //------------------------------------------------------------------------
     {  L_, DFP( 0.42),  2,    9,   ' ',  false,   false,  'l', "4.20e-01 "  },
     {  L_, DFP( 0.42),  2,    9,   ' ',  false,   false,  'i', " 4.20e-01"  },
     {  L_, DFP( 0.42),  2,    9,   ' ',  false,   false,  'r', " 4.20e-01"  },

     {  L_, DFP(-0.42),  2,    9,   ' ',  false,   false,  'l', "-4.20e-01"  },
     {  L_, DFP(-0.42),  2,    9,   ' ',  false,   false,  'i', "-4.20e-01"  },
     {  L_, DFP(-0.42),  2,    9,   ' ',  false,   false,  'r', "-4.20e-01"  },

     {  L_, DFP( 0.42),  2,    9,   ' ',  false,   true,   'l', "4.20E-01 "  },
     {  L_, DFP( 0.42),  2,    9,   ' ',  false,   true,   'i', " 4.20E-01"  },
     {  L_, DFP( 0.42),  2,    9,   ' ',  false,   true,   'r', " 4.20E-01"  },

     {  L_, DFP(-0.42),  2,    9,   ' ',  false,   true,   'l', "-4.20E-01"  },
     {  L_, DFP(-0.42),  2,    9,   ' ',  false,   true,   'i', "-4.20E-01"  },
     {  L_, DFP(-0.42),  2,    9,   ' ',  false,   true,   'r', "-4.20E-01"  },

     {  L_, DFP( 0.42),  2,    9,   ' ',  true,    false,  'l', "+4.20e-01"  },
     {  L_, DFP( 0.42),  2,    9,   ' ',  true,    false,  'i', "+4.20e-01"  },
     {  L_, DFP( 0.42),  2,    9,   ' ',  true,    false,  'r', "+4.20e-01"  },

     {  L_, DFP(-0.42),  2,    9,   ' ',  true,    false,  'l', "-4.20e-01"  },
     {  L_, DFP(-0.42),  2,    9,   ' ',  true,    false,  'i', "-4.20e-01"  },
     {  L_, DFP(-0.42),  2,    9,   ' ',  true,    false,  'r', "-4.20e-01"  },

     {  L_, DFP( 0.42),  2,    9,   ' ',  true,    true,   'l', "+4.20E-01"  },
     {  L_, DFP( 0.42),  2,    9,   ' ',  true,    true,   'i', "+4.20E-01"  },
     {  L_, DFP( 0.42),  2,    9,   ' ',  true,    true,   'r', "+4.20E-01"  },

     {  L_, DFP(-0.42),  2,    9,   ' ',  true,    true,   'l', "-4.20E-01"  },
     {  L_, DFP(-0.42),  2,    9,   ' ',  true,    true,   'i', "-4.20E-01"  },
     {  L_, DFP(-0.42),  2,    9,   ' ',  true,    true,   'r', "-4.20E-01"  },

     {  L_, DFP( 0.42),  2,    9,   '_',  false,   false,  'l', "4.20e-01_"  },
     {  L_, DFP( 0.42),  2,    9,   '_',  false,   false,  'i', "_4.20e-01"  },
     {  L_, DFP( 0.42),  2,    9,   '_',  false,   false,  'r', "_4.20e-01"  },

     {  L_, DFP(-0.42),  2,    9,   '_',  false,   false,  'l', "-4.20e-01"  },
     {  L_, DFP(-0.42),  2,    9,   '_',  false,   false,  'i', "-4.20e-01"  },
     {  L_, DFP(-0.42),  2,    9,   '_',  false,   false,  'r', "-4.20e-01"  },

     {  L_, DFP( 0.42),  2,    9,   '_',  false,   true,   'l', "4.20E-01_"  },
     {  L_, DFP( 0.42),  2,    9,   '_',  false,   true,   'i', "_4.20E-01"  },
     {  L_, DFP( 0.42),  2,    9,   '_',  false,   true,   'r', "_4.20E-01"  },

     {  L_, DFP(-0.42),  2,    9,   '_',  false,   true,   'l', "-4.20E-01"  },
     {  L_, DFP(-0.42),  2,    9,   '_',  false,   true,   'i', "-4.20E-01"  },
     {  L_, DFP(-0.42),  2,    9,   '_',  false,   true,   'r', "-4.20E-01"  },

     {  L_, DFP( 0.42),  2,    9,   '_',  true,    false,  'l', "+4.20e-01"  },
     {  L_, DFP( 0.42),  2,    9,   '_',  true,    false,  'i', "+4.20e-01"  },
     {  L_, DFP( 0.42),  2,    9,   '_',  true,    false,  'r', "+4.20e-01"  },

     {  L_, DFP(-0.42),  2,    9,   '_',  true,    false,  'l', "-4.20e-01"  },
     {  L_, DFP(-0.42),  2,    9,   '_',  true,    false,  'i', "-4.20e-01"  },
     {  L_, DFP(-0.42),  2,    9,   '_',  true,    false,  'r', "-4.20e-01"  },

     {  L_, DFP( 0.42),  2,    9,   '_',  true,    true,   'l', "+4.20E-01"  },
     {  L_, DFP( 0.42),  2,    9,   '_',  true,    true,   'i', "+4.20E-01"  },
     {  L_, DFP( 0.42),  2,    9,   '_',  true,    true,   'r', "+4.20E-01"  },

     {  L_, DFP(-0.42),  2,    9,   '_',  true,    true,   'l', "-4.20E-01"  },
     {  L_, DFP(-0.42),  2,    9,   '_',  true,    true,   'i', "-4.20E-01"  },
     {  L_, DFP(-0.42),  2,    9,   '_',  true,    true,   'r', "-4.20E-01"  },

     {  L_, DFP( 0.42),  2,   10,   ' ',  false,   false,  'l', "4.20e-01  " },
     {  L_, DFP( 0.42),  2,   10,   ' ',  false,   false,  'i', "  4.20e-01" },
     {  L_, DFP( 0.42),  2,   10,   ' ',  false,   false,  'r', "  4.20e-01" },

     {  L_, DFP(-0.42),  2,   10,   ' ',  false,   false,  'l', "-4.20e-01 " },
     {  L_, DFP(-0.42),  2,   10,   ' ',  false,   false,  'i', "- 4.20e-01" },
     {  L_, DFP(-0.42),  2,   10,   ' ',  false,   false,  'r', " -4.20e-01" },

     {  L_, DFP( 0.42),  2,   10,   ' ',  false,   true,   'l', "4.20E-01  " },
     {  L_, DFP( 0.42),  2,   10,   ' ',  false,   true,   'i', "  4.20E-01" },
     {  L_, DFP( 0.42),  2,   10,   ' ',  false,   true,   'r', "  4.20E-01" },

     {  L_, DFP(-0.42),  2,   10,   ' ',  false,   true,   'l', "-4.20E-01 " },
     {  L_, DFP(-0.42),  2,   10,   ' ',  false,   true,   'i', "- 4.20E-01" },
     {  L_, DFP(-0.42),  2,   10,   ' ',  false,   true,   'r', " -4.20E-01" },

     {  L_, DFP( 0.42),  2,   10,   ' ',  true,    false,  'l', "+4.20e-01 " },
     {  L_, DFP( 0.42),  2,   10,   ' ',  true,    false,  'i', "+ 4.20e-01" },
     {  L_, DFP( 0.42),  2,   10,   ' ',  true,    false,  'r', " +4.20e-01" },

     {  L_, DFP(-0.42),  2,   10,   ' ',  true,    false,  'l', "-4.20e-01 " },
     {  L_, DFP(-0.42),  2,   10,   ' ',  true,    false,  'i', "- 4.20e-01" },
     {  L_, DFP(-0.42),  2,   10,   ' ',  true,    false,  'r', " -4.20e-01" },

     {  L_, DFP( 0.42),  2,   10,   ' ',  true,    true,   'l', "+4.20E-01 " },
     {  L_, DFP( 0.42),  2,   10,   ' ',  true,    true,   'i', "+ 4.20E-01" },
     {  L_, DFP( 0.42),  2,   10,   ' ',  true,    true,   'r', " +4.20E-01" },

     {  L_, DFP(-0.42),  2,   10,   ' ',  true,    true,   'l', "-4.20E-01 " },
     {  L_, DFP(-0.42),  2,   10,   ' ',  true,    true,   'i', "- 4.20E-01" },
     {  L_, DFP(-0.42),  2,   10,   ' ',  true,    true,   'r', " -4.20E-01" },

     {  L_, DFP( 0.42),  2,   10,   'x',  false,   false,  'l', "4.20e-01xx" },
     {  L_, DFP( 0.42),  2,   10,   'x',  false,   false,  'i', "xx4.20e-01" },
     {  L_, DFP( 0.42),  2,   10,   'x',  false,   false,  'r', "xx4.20e-01" },

     {  L_, DFP(-0.42),  2,   10,   'x',  false,   false,  'l', "-4.20e-01x" },
     {  L_, DFP(-0.42),  2,   10,   'x',  false,   false,  'i', "-x4.20e-01" },
     {  L_, DFP(-0.42),  2,   10,   'x',  false,   false,  'r', "x-4.20e-01" },

     {  L_, DFP( 0.42),  2,   10,   'x',  false,   true,   'l', "4.20E-01xx" },
     {  L_, DFP( 0.42),  2,   10,   'x',  false,   true,   'i', "xx4.20E-01" },
     {  L_, DFP( 0.42),  2,   10,   'x',  false,   true,   'r', "xx4.20E-01" },

     {  L_, DFP(-0.42),  2,   10,   'x',  false,   true,   'l', "-4.20E-01x" },
     {  L_, DFP(-0.42),  2,   10,   'x',  false,   true,   'i', "-x4.20E-01" },
     {  L_, DFP(-0.42),  2,   10,   'x',  false,   true,   'r', "x-4.20E-01" },

     {  L_, DFP( 0.42),  2,   10,   'x',  true,    false,  'l', "+4.20e-01x" },
     {  L_, DFP( 0.42),  2,   10,   'x',  true,    false,  'i', "+x4.20e-01" },
     {  L_, DFP( 0.42),  2,   10,   'x',  true,    false,  'r', "x+4.20e-01" },

     {  L_, DFP(-0.42),  2,   10,   'x',  true,    false,  'l', "-4.20e-01x" },
     {  L_, DFP(-0.42),  2,   10,   'x',  true,    false,  'i', "-x4.20e-01" },
     {  L_, DFP(-0.42),  2,   10,   'x',  true,    false,  'r', "x-4.20e-01" },

     {  L_, DFP( 0.42),  2,   10,   'x',  true,    true,   'l', "+4.20E-01x" },
     {  L_, DFP( 0.42),  2,   10,   'x',  true,    true,   'i', "+x4.20E-01" },
     {  L_, DFP( 0.42),  2,   10,   'x',  true,    true,   'r', "x+4.20E-01" },

     {  L_, DFP(-0.42),  2,   10,   'x',  true,    true,   'l', "-4.20E-01x" },
     {  L_, DFP(-0.42),  2,   10,   'x',  true,    true,   'i', "-x4.20E-01" },
     {  L_, DFP(-0.42),  2,   10,   'x',  true,    true,   'r', "x-4.20E-01" },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        typedef BDEC::Decimal32 Type32;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE       = DATA[ti].d_line;
            const Type32  DECIMAL32  = DATA[ti].d_decimalValue;
            const int     PRECISION  = DATA[ti].d_precision;
            const int     WIDTH      = DATA[ti].d_width;
            const char    FILL       = DATA[ti].d_fill;
            const bool    SHOW_POS   = DATA[ti].d_showPos;
            const bool    UPPERCASE  = DATA[ti].d_uppercase;
            const bool    LEFT       = (DATA[ti].d_justification == 'l');
            const bool    INTERNAL   = (DATA[ti].d_justification == 'i');
            const bool    RIGHT      = (DATA[ti].d_justification == 'r');
            const char   *EXPECTED   = DATA[ti].d_expected;

            if (veryVeryVerbose) {
                P_(LINE); P_(EXPECTED); P(DECIMAL32);
            }

            // Test with Decimal32.  C-1
            {
                const BDEC::Decimal32 VALUE(DECIMAL32);

                bsl::ostringstream outdec(pa);

                outdec << bsl::scientific
                       << bsl::setprecision(PRECISION);
                outdec.width(WIDTH);
                outdec.fill(FILL);

                if (SHOW_POS)  { outdec << bsl::showpos;   }
                if (UPPERCASE) { outdec << bsl::uppercase; }
                if (LEFT)      { outdec << bsl::left;      }
                if (INTERNAL)  { outdec << bsl::internal;  }
                if (RIGHT)     { outdec << bsl::right;     }
                outdec << VALUE;

                bsl::string ACTUAL(pa);
                getStringFromStream(outdec, &ACTUAL);

                ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
                ASSERTV(outdec.good());
            }

            // Test with Decimal64.  C-1
            {
                const BDEC::Decimal64 VALUE(DECIMAL32);

                bsl::ostringstream outdec(pa);

                outdec << bsl::scientific
                       << bsl::setprecision(PRECISION);
                outdec.width(WIDTH);
                outdec.fill(FILL);

                if (SHOW_POS)  { outdec << bsl::showpos;  }
                if (UPPERCASE) { outdec << bsl::uppercase; }
                if (LEFT)      { outdec << bsl::left;     }
                if (INTERNAL)  { outdec << bsl::internal; }
                if (RIGHT)     { outdec << bsl::right;    }
                outdec << VALUE;

                bsl::string ACTUAL(pa);
                getStringFromStream(outdec, &ACTUAL);

                ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
                ASSERTV(outdec.good());
            }

            // Test with print.  C-15..17
            {
                const BDEC::Decimal64 VALUE(DECIMAL32);

                bsl::ostringstream outdec(pa);

                outdec << bsl::scientific
                       << bsl::setprecision(PRECISION);
                outdec.width(WIDTH);
                outdec.fill(FILL);

                if (SHOW_POS)  { outdec << bsl::showpos;   }
                if (UPPERCASE) { outdec << bsl::uppercase; }
                if (LEFT)      { outdec << bsl::left;      }
                if (INTERNAL)  { outdec << bsl::internal;  }
                if (RIGHT)     { outdec << bsl::right;     }
                VALUE.print(outdec, 0, -1);

                bsl::string ACTUAL(pa);
                getStringFromStream(outdec, &ACTUAL);

                ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
            }

            // Test with Decimal128.  C-1
            {
                const BDEC::Decimal128 VALUE(DECIMAL32);

                bsl::ostringstream outdec(pa);

                outdec << bsl::scientific
                       << bsl::setprecision(PRECISION);
                outdec.width(WIDTH);
                outdec.fill(FILL);

                if (SHOW_POS)  { outdec << bsl::showpos;   }
                if (UPPERCASE) { outdec << bsl::uppercase; }
                if (LEFT)      { outdec << bsl::left;      }
                if (INTERNAL)  { outdec << bsl::internal;  }
                if (RIGHT)     { outdec << bsl::right;     }
                outdec << VALUE;

                bsl::string ACTUAL(pa);
                getStringFromStream(outdec, &ACTUAL);

                ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
                ASSERTV(outdec.good());
            }
        }
    }
    {
        static const struct {
            int              d_line;
            BDEC::Decimal32  d_decimalValue;
            char             d_style;
            int              d_precision;
            int              d_width;
            const char      *d_expected;
            int              d_showpoint;
        } DATA[] = {
            //-----------------------------------------------------------------
            // L | NUMBER      | STYLE | PRS | WIDTH | EXPECTED
            //-----------------------------------------------------------------
            //-----------------------------------------------------------------
            // Test that rendering decimal with combination of
            // 'bsl::setprecision' and 'bsl::width' manipulators produces
            // expected output.  C-5,8,13,14
            //-----------------------------------------------------------------
            // Fixed notation.
            //-----------------------------------------------------------------
            {  L_, DFP( 23.456),  'F',    0,    0,    "23",          0 },
            {  L_, DFP( 23.456),  'F',    0,    0,   "23.",          1 },
            {  L_, DFP( 23.456),  'F',    0,    1,    "23",          0 },
            {  L_, DFP( 23.456),  'F',    0,    1,   "23.",          1 },
            {  L_, DFP( 23.456),  'F',    0,    2,    "23",          0 },
            {  L_, DFP( 23.456),  'F',    0,    2,   "23.",          1 },
            {  L_, DFP( 23.456),  'F',    0,    3,   " 23",          0 },
            {  L_, DFP( 23.456),  'F',    0,    3,   "23.",          1 },
            {  L_, DFP( 23.456),  'F',    0,    4,  "  23",          0 },
            {  L_, DFP( 23.456),  'F',    0,    4,  " 23.",          1 },

            {  L_, DFP( 23.456),  'F',    1,    0,    "23.5",        0 },
            {  L_, DFP( 23.456),  'F',    1,    1,    "23.5",        0 },
            {  L_, DFP( 23.456),  'F',    1,    2,    "23.5",        0 },
            {  L_, DFP( 23.456),  'F',    1,    3,    "23.5",        0 },
            {  L_, DFP( 23.456),  'F',    1,    4,    "23.5",        0 },
            {  L_, DFP( 23.456),  'F',    1,    5,   " 23.5",        0 },
            {  L_, DFP( 23.456),  'F',    1,    6,  "  23.5",        0 },

            {  L_, DFP( 23.456),  'F',    2,    0,    "23.46",       0 },
            {  L_, DFP( 23.456),  'F',    2,    1,    "23.46",       0 },
            {  L_, DFP( 23.456),  'F',    2,    2,    "23.46",       0 },
            {  L_, DFP( 23.456),  'F',    2,    3,    "23.46",       0 },
            {  L_, DFP( 23.456),  'F',    2,    4,    "23.46",       0 },
            {  L_, DFP( 23.456),  'F',    2,    5,    "23.46",       0 },
            {  L_, DFP( 23.456),  'F',    2,    6,   " 23.46",       0 },
            {  L_, DFP( 23.456),  'F',    2,    7,  "  23.46",       0 },

            {  L_, DFP( 23.456),  'F',    3,    0,    "23.456",      0 },
            {  L_, DFP( 23.456),  'F',    3,    1,    "23.456",      0 },
            {  L_, DFP( 23.456),  'F',    3,    2,    "23.456",      0 },
            {  L_, DFP( 23.456),  'F',    3,    3,    "23.456",      0 },
            {  L_, DFP( 23.456),  'F',    3,    4,    "23.456",      0 },
            {  L_, DFP( 23.456),  'F',    3,    5,    "23.456",      0 },
            {  L_, DFP( 23.456),  'F',    3,    6,    "23.456",      0 },
            {  L_, DFP( 23.456),  'F',    3,    7,   " 23.456",      0 },
            {  L_, DFP( 23.456),  'F',    3,    8,  "  23.456",      0 },

            {  L_, DFP( 23.456),  'F',    4,    0,    "23.4560",     0 },
            {  L_, DFP( 23.456),  'F',    4,    1,    "23.4560",     0 },
            {  L_, DFP( 23.456),  'F',    4,    2,    "23.4560",     0 },
            {  L_, DFP( 23.456),  'F',    4,    3,    "23.4560",     0 },
            {  L_, DFP( 23.456),  'F',    4,    4,    "23.4560",     0 },
            {  L_, DFP( 23.456),  'F',    4,    5,    "23.4560",     0 },
            {  L_, DFP( 23.456),  'F',    4,    6,    "23.4560",     0 },
            {  L_, DFP( 23.456),  'F',    4,    7,    "23.4560",     0 },
            {  L_, DFP( 23.456),  'F',    4,    8,   " 23.4560",     0 },
            {  L_, DFP( 23.456),  'F',    4,    9,  "  23.4560",     0 },

            {  L_, DFP( 23.456),  'F',    5,    0,    "23.45600",    0 },
            {  L_, DFP( 23.456),  'F',    5,    1,    "23.45600",    0 },
            {  L_, DFP( 23.456),  'F',    5,    2,    "23.45600",    0 },
            {  L_, DFP( 23.456),  'F',    5,    3,    "23.45600",    0 },
            {  L_, DFP( 23.456),  'F',    5,    4,    "23.45600",    0 },
            {  L_, DFP( 23.456),  'F',    5,    5,    "23.45600",    0 },
            {  L_, DFP( 23.456),  'F',    5,    6,    "23.45600",    0 },
            {  L_, DFP( 23.456),  'F',    5,    7,    "23.45600",    0 },
            {  L_, DFP( 23.456),  'F',    5,    8,    "23.45600",    0 },
            {  L_, DFP( 23.456),  'F',    5,    9,   " 23.45600",    0 },
            {  L_, DFP( 23.456),  'F',    5,   10,  "  23.45600",    0 },
            //-----------------------------------------------------------------
            // Scientific notation.
            //-----------------------------------------------------------------
            {  L_, DFP( 23.456),  'S',    0,    0,    "2e+01",       0 },
            {  L_, DFP( 23.456),  'S',    0,    0,   "2.e+01",       1 },
            {  L_, DFP( 23.456),  'S',    0,    1,    "2e+01",       0 },
            {  L_, DFP( 23.456),  'S',    0,    1,   "2.e+01",       1 },
            {  L_, DFP( 23.456),  'S',    0,    2,    "2e+01",       0 },
            {  L_, DFP( 23.456),  'S',    0,    2,   "2.e+01",       1 },
            {  L_, DFP( 23.456),  'S',    0,    3,    "2e+01",       0 },
            {  L_, DFP( 23.456),  'S',    0,    3,   "2.e+01",       1 },
            {  L_, DFP( 23.456),  'S',    0,    4,    "2e+01",       0 },
            {  L_, DFP( 23.456),  'S',    0,    4,   "2.e+01",       1 },
            {  L_, DFP( 23.456),  'S',    0,    5,    "2e+01",       0 },
            {  L_, DFP( 23.456),  'S',    0,    5,   "2.e+01",       1 },
            {  L_, DFP( 23.456),  'S',    0,    6,   " 2e+01",       0 },
            {  L_, DFP( 23.456),  'S',    0,    6,   "2.e+01",       1 },
            {  L_, DFP( 23.456),  'S',    0,    7,  "  2e+01",       0 },
            {  L_, DFP( 23.456),  'S',    0,    7,  " 2.e+01",       1 },

            {  L_, DFP( 23.456),  'S',    1,    0,    "2.3e+01",     0 },
            {  L_, DFP( 23.456),  'S',    1,    1,    "2.3e+01",     0 },
            {  L_, DFP( 23.456),  'S',    1,    2,    "2.3e+01",     0 },
            {  L_, DFP( 23.456),  'S',    1,    3,    "2.3e+01",     0 },
            {  L_, DFP( 23.456),  'S',    1,    4,    "2.3e+01",     0 },
            {  L_, DFP( 23.456),  'S',    1,    5,    "2.3e+01",     0 },
            {  L_, DFP( 23.456),  'S',    1,    6,    "2.3e+01",     0 },
            {  L_, DFP( 23.456),  'S',    1,    7,    "2.3e+01",     0 },
            {  L_, DFP( 23.456),  'S',    1,    8,   " 2.3e+01",     0 },
            {  L_, DFP( 23.456),  'S',    1,    9,  "  2.3e+01",     0 },

            {  L_, DFP( 23.456),  'S',    2,    0,    "2.35e+01",    0 },
            {  L_, DFP( 23.456),  'S',    2,    1,    "2.35e+01",    0 },
            {  L_, DFP( 23.456),  'S',    2,    2,    "2.35e+01",    0 },
            {  L_, DFP( 23.456),  'S',    2,    3,    "2.35e+01",    0 },
            {  L_, DFP( 23.456),  'S',    2,    4,    "2.35e+01",    0 },
            {  L_, DFP( 23.456),  'S',    2,    5,    "2.35e+01",    0 },
            {  L_, DFP( 23.456),  'S',    2,    6,    "2.35e+01",    0 },
            {  L_, DFP( 23.456),  'S',    2,    7,    "2.35e+01",    0 },
            {  L_, DFP( 23.456),  'S',    2,    8,    "2.35e+01",    0 },
            {  L_, DFP( 23.456),  'S',    2,    9,   " 2.35e+01",    0 },
            {  L_, DFP( 23.456),  'S',    2,   10,  "  2.35e+01",    0 },

            {  L_, DFP( 23.456),  'S',    3,    0,    "2.346e+01",   0 },
            {  L_, DFP( 23.456),  'S',    3,    1,    "2.346e+01",   0 },
            {  L_, DFP( 23.456),  'S',    3,    2,    "2.346e+01",   0 },
            {  L_, DFP( 23.456),  'S',    3,    3,    "2.346e+01",   0 },
            {  L_, DFP( 23.456),  'S',    3,    4,    "2.346e+01",   0 },
            {  L_, DFP( 23.456),  'S',    3,    5,    "2.346e+01",   0 },
            {  L_, DFP( 23.456),  'S',    3,    6,    "2.346e+01",   0 },
            {  L_, DFP( 23.456),  'S',    3,    7,    "2.346e+01",   0 },
            {  L_, DFP( 23.456),  'S',    3,    8,    "2.346e+01",   0 },
            {  L_, DFP( 23.456),  'S',    3,    9,    "2.346e+01",   0 },
            {  L_, DFP( 23.456),  'S',    3,   10,   " 2.346e+01",   0 },
            {  L_, DFP( 23.456),  'S',    3,   11,  "  2.346e+01",   0 },

            {  L_, DFP( 23.456),  'S',    4,    0,    "2.3456e+01",  0 },
            {  L_, DFP( 23.456),  'S',    4,    1,    "2.3456e+01",  0 },
            {  L_, DFP( 23.456),  'S',    4,    2,    "2.3456e+01",  0 },
            {  L_, DFP( 23.456),  'S',    4,    3,    "2.3456e+01",  0 },
            {  L_, DFP( 23.456),  'S',    4,    4,    "2.3456e+01",  0 },
            {  L_, DFP( 23.456),  'S',    4,    5,    "2.3456e+01",  0 },
            {  L_, DFP( 23.456),  'S',    4,    6,    "2.3456e+01",  0 },
            {  L_, DFP( 23.456),  'S',    4,    7,    "2.3456e+01",  0 },
            {  L_, DFP( 23.456),  'S',    4,    8,    "2.3456e+01",  0 },
            {  L_, DFP( 23.456),  'S',    4,    9,    "2.3456e+01",  0 },
            {  L_, DFP( 23.456),  'S',    4,   10,    "2.3456e+01",  0 },
            {  L_, DFP( 23.456),  'S',    4,   11,   " 2.3456e+01",  0 },
            {  L_, DFP( 23.456),  'S',    4,   12,  "  2.3456e+01",  0 },

            {  L_, DFP( 23.456),  'S',    5,    0,    "2.34560e+01", 0 },
            {  L_, DFP( 23.456),  'S',    5,    1,    "2.34560e+01", 0 },
            {  L_, DFP( 23.456),  'S',    5,    2,    "2.34560e+01", 0 },
            {  L_, DFP( 23.456),  'S',    5,    3,    "2.34560e+01", 0 },
            {  L_, DFP( 23.456),  'S',    5,    4,    "2.34560e+01", 0 },
            {  L_, DFP( 23.456),  'S',    5,    5,    "2.34560e+01", 0 },
            {  L_, DFP( 23.456),  'S',    5,    6,    "2.34560e+01", 0 },
            {  L_, DFP( 23.456),  'S',    5,    7,    "2.34560e+01", 0 },
            {  L_, DFP( 23.456),  'S',    5,    8,    "2.34560e+01", 0 },
            {  L_, DFP( 23.456),  'S',    5,    9,    "2.34560e+01", 0 },
            {  L_, DFP( 23.456),  'S',    5,   10,    "2.34560e+01", 0 },
            {  L_, DFP( 23.456),  'S',    5,   11,    "2.34560e+01", 0 },
            {  L_, DFP( 23.456),  'S',    5,   12,   " 2.34560e+01", 0 },
            {  L_, DFP( 23.456),  'S',    5,   13,  "  2.34560e+01", 0 },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int             LINE       = DATA[ti].d_line;
            const BDEC::Decimal32 DECIMAL32  = DATA[ti].d_decimalValue;
            const char            STYLE      = DATA[ti].d_style;
            const int             PRECISION  = DATA[ti].d_precision;
            const int             WIDTH      = DATA[ti].d_width;
            const char           *EXPECTED   = DATA[ti].d_expected;
            const bool            SHOWPOINT  = DATA[ti].d_showpoint;

            if (veryVeryVerbose) {
                P_(LINE); P_(EXPECTED); P(DECIMAL32);
            }

            // Test with Decimal32.
            {
                const BDEC::Decimal32 VALUE(DECIMAL32);

                bsl::ostringstream outdec(pa);

                if ('F' == STYLE) { outdec << bsl::fixed;      }
                if ('S' == STYLE) { outdec << bsl::scientific; }
                if (SHOWPOINT)    { outdec << bsl::showpoint;  }

                outdec.width(WIDTH);
                outdec << bsl::setprecision(PRECISION)
                       << VALUE;

                bsl::string ACTUAL(pa);
                getStringFromStream(outdec, &ACTUAL);

                ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
                ASSERTV(outdec.good());
            }

            // Test with Decimal64.
            {
                const BDEC::Decimal64 VALUE(DECIMAL32);

                bsl::ostringstream outdec(pa);

                if ('F' == STYLE) { outdec << bsl::fixed;      }
                if ('S' == STYLE) { outdec << bsl::scientific; }
                if (SHOWPOINT)    { outdec << bsl::showpoint;  }

                outdec.width(WIDTH);
                outdec << bsl::setprecision(PRECISION)
                       << VALUE;

                bsl::string ACTUAL(pa);
                getStringFromStream(outdec, &ACTUAL);

                ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
                ASSERTV(outdec.good());
            }

            // Test with print.
            {
                const BDEC::Decimal64 VALUE(DECIMAL32);

                bsl::ostringstream outdec(pa);

                if ('F' == STYLE) { outdec << bsl::fixed;      }
                if ('S' == STYLE) { outdec << bsl::scientific; }
                if (SHOWPOINT)    { outdec << bsl::showpoint;  }

                outdec.width(WIDTH);
                outdec << bsl::setprecision(PRECISION);
                VALUE.print(outdec, 0, -1);

                bsl::string ACTUAL(pa);
                getStringFromStream(outdec, &ACTUAL);

                ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
                ASSERTV(outdec.good());
            }

            // Test with Decimal128.
            {
                const BDEC::Decimal128 VALUE(DECIMAL32);

                bsl::ostringstream outdec(pa);

                if ('F' == STYLE) { outdec << bsl::fixed;      }
                if ('S' == STYLE) { outdec << bsl::scientific; }
                if (SHOWPOINT)    { outdec << bsl::showpoint;  }

                outdec.width(WIDTH);
                outdec << bsl::setprecision(PRECISION)
                       << VALUE;

                bsl::string ACTUAL(pa);
                getStringFromStream(outdec, &ACTUAL);

                ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
                ASSERTV(outdec.good());
            }
        }
    }
#undef DFP
    {
        const char *INF_P  =   "inf";
        const char *INF_N  =  "-inf";
        const char *QNAN_P =   "nan";
        const char *QNAN_N =  "-nan";
        const char *SNAN_P =  "snan";
        const char *SNAN_N = "-snan";

        static const struct {
            int         d_line;
            const char *d_decimalStr;
            int         d_width;
            char        d_fill;
            bool        d_uppercase;
            bool        d_showPos;
            char        d_justification;
            const char *d_expected;
        } DATA[] = {
            //-----------------------------------------------------------------
            // L |NUMBER |WIDTH |FILL |UPPER |SHOW_POS |JUST |EXPECTED
            //-----------------------------------------------------------------
            //-----------------------------------------------------------------
            // Test Infinity value.  C-3
            //-----------------------------------------------------------------
            {  L_, INF_P,    6,   ' ', false, false,   'l',   "inf   "  },
            {  L_, INF_P,    6,   ' ', false, false,   'i',   "   inf"  },
            {  L_, INF_P,    6,   ' ', false, false,   'r',   "   inf"  },

            {  L_, INF_P,    6,   ' ', false, true,    'l',   "+inf  "  },
            {  L_, INF_P,    6,   ' ', false, true,    'i',   "+  inf"  },
            {  L_, INF_P,    6,   ' ', false, true,    'r',   "  +inf"  },

            {  L_, INF_P,    6,   ' ', true,  false,   'l',   "INF   "  },
            {  L_, INF_P,    6,   ' ', true,  false,   'i',   "   INF"  },
            {  L_, INF_P,    6,   ' ', true,  false,   'r',   "   INF"  },

            {  L_, INF_P,    6,   ' ', true,  true,    'l',   "+INF  "  },
            {  L_, INF_P,    6,   ' ', true,  true,    'i',   "+  INF"  },
            {  L_, INF_P,    6,   ' ', true,  true,    'r',   "  +INF"  },

            {  L_, INF_P,    6,   'x', false, false,   'l',   "infxxx"  },
            {  L_, INF_P,    6,   'x', false, false,   'i',   "xxxinf"  },
            {  L_, INF_P,    6,   'x', false, false,   'r',   "xxxinf"  },

            {  L_, INF_P,    6,   'x', false, true,    'l',   "+infxx"  },
            {  L_, INF_P,    6,   'x', false, true,    'i',   "+xxinf"  },
            {  L_, INF_P,    6,   'x', false, true,    'r',   "xx+inf"  },

            {  L_, INF_P,    6,   'x', true,  false,   'l',   "INFxxx"  },
            {  L_, INF_P,    6,   'x', true,  false,   'i',   "xxxINF"  },
            {  L_, INF_P,    6,   'x', true,  false,   'r',   "xxxINF"  },

            {  L_, INF_P,    6,   'x', true,  true,    'l',   "+INFxx"  },
            {  L_, INF_P,    6,   'x', true,  true,    'i',   "+xxINF"  },
            {  L_, INF_P,    6,   'x', true,  true,    'r',   "xx+INF"  },

            {  L_, INF_N,    7,   ' ', false, false,   'l',   "-inf   "  },
            {  L_, INF_N,    7,   ' ', false, false,   'i',   "-   inf"  },
            {  L_, INF_N,    7,   ' ', false, false,   'r',   "   -inf"  },

            {  L_, INF_N,    7,   ' ', false, true,    'l',   "-inf   "  },
            {  L_, INF_N,    7,   ' ', false, true,    'i',   "-   inf"  },
            {  L_, INF_N,    7,   ' ', false, true,    'r',   "   -inf"  },

            {  L_, INF_N,    7,   ' ', true,  false,   'l',   "-INF   "  },
            {  L_, INF_N,    7,   ' ', true,  false,   'i',   "-   INF"  },
            {  L_, INF_N,    7,   ' ', true,  false,   'r',   "   -INF"  },

            {  L_, INF_N,    7,   ' ', true,  true,    'l',   "-INF   "  },
            {  L_, INF_N,    7,   ' ', true,  true,    'i',   "-   INF"  },
            {  L_, INF_N,    7,   ' ', true,  true,    'r',   "   -INF"  },

            {  L_, INF_N,    7,   '_', false, false,   'l',   "-inf___"  },
            {  L_, INF_N,    7,   '_', false, false,   'i',   "-___inf"  },
            {  L_, INF_N,    7,   '_', false, false,   'r',   "___-inf"  },

            {  L_, INF_N,    7,   '_', false, true,    'l',   "-inf___"  },
            {  L_, INF_N,    7,   '_', false, true,    'i',   "-___inf"  },
            {  L_, INF_N,    7,   '_', false, true,    'r',   "___-inf"  },

            {  L_, INF_N,    7,   '_', true,  false,   'l',   "-INF___"  },
            {  L_, INF_N,    7,   '_', true,  false,   'i',   "-___INF"  },
            {  L_, INF_N,    7,   '_', true,  false,   'r',   "___-INF"  },

            {  L_, INF_N,    7,   '_', true,  true,    'l',   "-INF___"  },
            {  L_, INF_N,    7,   '_', true,  true,    'i',   "-___INF"  },
            {  L_, INF_N,    7,   '_', true,  true,    'r',   "___-INF"  },
            //-----------------------------------------------------------------
            // Test quiet NaN values.  C-4
            //-----------------------------------------------------------------
            {  L_, QNAN_P,    6,   ' ', false, false,   'l',   "nan   "  },
            {  L_, QNAN_P,    6,   ' ', false, false,   'i',   "   nan"  },
            {  L_, QNAN_P,    6,   ' ', false, false,   'r',   "   nan"  },

            {  L_, QNAN_P,    6,   ' ', false, true,    'l',   "+nan  "  },
            {  L_, QNAN_P,    6,   ' ', false, true,    'i',   "+  nan"  },
            {  L_, QNAN_P,    6,   ' ', false, true,    'r',   "  +nan"  },

            {  L_, QNAN_P,    6,   ' ', true,  false,   'l',   "NAN   "  },
            {  L_, QNAN_P,    6,   ' ', true,  false,   'i',   "   NAN"  },
            {  L_, QNAN_P,    6,   ' ', true,  false,   'r',   "   NAN"  },

            {  L_, QNAN_P,    6,   ' ', true,  true,    'l',   "+NAN  "  },
            {  L_, QNAN_P,    6,   ' ', true,  true,    'i',   "+  NAN"  },
            {  L_, QNAN_P,    6,   ' ', true,  true,    'r',   "  +NAN"  },

            {  L_, QNAN_P,    6,   'x', false, false,   'l',   "nanxxx"  },
            {  L_, QNAN_P,    6,   'x', false, false,   'i',   "xxxnan"  },
            {  L_, QNAN_P,    6,   'x', false, false,   'r',   "xxxnan"  },

            {  L_, QNAN_P,    6,   'x', false, true,    'l',   "+nanxx"  },
            {  L_, QNAN_P,    6,   'x', false, true,    'i',   "+xxnan"  },
            {  L_, QNAN_P,    6,   'x', false, true,    'r',   "xx+nan"  },

            {  L_, QNAN_P,    6,   'x', true,  false,   'l',   "NANxxx"  },
            {  L_, QNAN_P,    6,   'x', true,  false,   'i',   "xxxNAN"  },
            {  L_, QNAN_P,    6,   'x', true,  false,   'r',   "xxxNAN"  },

            {  L_, QNAN_P,    6,   'x', true,  true,    'l',   "+NANxx"  },
            {  L_, QNAN_P,    6,   'x', true,  true,    'i',   "+xxNAN"  },
            {  L_, QNAN_P,    6,   'x', true,  true,    'r',   "xx+NAN"  },

            {  L_, QNAN_N,    7,   ' ', false, false,   'l',   "-nan   "  },
            {  L_, QNAN_N,    7,   ' ', false, false,   'i',   "-   nan"  },
            {  L_, QNAN_N,    7,   ' ', false, false,   'r',   "   -nan"  },

            {  L_, QNAN_N,    7,   ' ', false, true,    'l',   "-nan   "  },
            {  L_, QNAN_N,    7,   ' ', false, true,    'i',   "-   nan"  },
            {  L_, QNAN_N,    7,   ' ', false, true,    'r',   "   -nan"  },

            {  L_, QNAN_N,    7,   ' ', true,  false,   'l',   "-NAN   "  },
            {  L_, QNAN_N,    7,   ' ', true,  false,   'i',   "-   NAN"  },
            {  L_, QNAN_N,    7,   ' ', true,  false,   'r',   "   -NAN"  },

            {  L_, QNAN_N,    7,   ' ', true,  true,    'l',   "-NAN   "  },
            {  L_, QNAN_N,    7,   ' ', true,  true,    'i',   "-   NAN"  },
            {  L_, QNAN_N,    7,   ' ', true,  true,    'r',   "   -NAN"  },

            {  L_, QNAN_N,    7,   '_', false, false,   'l',   "-nan___"  },
            {  L_, QNAN_N,    7,   '_', false, false,   'i',   "-___nan"  },
            {  L_, QNAN_N,    7,   '_', false, false,   'r',   "___-nan"  },

            {  L_, QNAN_N,    7,   '_', false, true,    'l',   "-nan___"  },
            {  L_, QNAN_N,    7,   '_', false, true,    'i',   "-___nan"  },
            {  L_, QNAN_N,    7,   '_', false, true,    'r',   "___-nan"  },

            {  L_, QNAN_N,    7,   '_', true,  false,   'l',   "-NAN___"  },
            {  L_, QNAN_N,    7,   '_', true,  false,   'i',   "-___NAN"  },
            {  L_, QNAN_N,    7,   '_', true,  false,   'r',   "___-NAN"  },

            {  L_, QNAN_N,    7,   '_', true,  true,    'l',   "-NAN___"  },
            {  L_, QNAN_N,    7,   '_', true,  true,    'i',   "-___NAN"  },
            {  L_, QNAN_N,    7,   '_', true,  true,    'r',   "___-NAN"  },
            //-----------------------------------------------------------------
            // Test signaling NaN values.  C-4
            //-----------------------------------------------------------------
            {  L_, SNAN_P,    7,   ' ', false, false,   'l',   "snan   "  },
            {  L_, SNAN_P,    7,   ' ', false, false,   'i',   "   snan"  },
            {  L_, SNAN_P,    7,   ' ', false, false,   'r',   "   snan"  },

            {  L_, SNAN_P,    7,   ' ', false, true,    'l',   "+snan  "  },
            {  L_, SNAN_P,    7,   ' ', false, true,    'i',   "+  snan"  },
            {  L_, SNAN_P,    7,   ' ', false, true,    'r',   "  +snan"  },

            {  L_, SNAN_P,    7,   ' ', true,  false,   'l',   "SNAN   "  },
            {  L_, SNAN_P,    7,   ' ', true,  false,   'i',   "   SNAN"  },
            {  L_, SNAN_P,    7,   ' ', true,  false,   'r',   "   SNAN"  },

            {  L_, SNAN_P,    7,   ' ', true,  true,    'l',   "+SNAN  "  },
            {  L_, SNAN_P,    7,   ' ', true,  true,    'i',   "+  SNAN"  },
            {  L_, SNAN_P,    7,   ' ', true,  true,    'r',   "  +SNAN"  },

            {  L_, SNAN_P,    7,   'x', false, false,   'l',   "snanxxx"  },
            {  L_, SNAN_P,    7,   'x', false, false,   'i',   "xxxsnan"  },
            {  L_, SNAN_P,    7,   'x', false, false,   'r',   "xxxsnan"  },

            {  L_, SNAN_P,    7,   'x', false, true,    'l',   "+snanxx"  },
            {  L_, SNAN_P,    7,   'x', false, true,    'i',   "+xxsnan"  },
            {  L_, SNAN_P,    7,   'x', false, true,    'r',   "xx+snan"  },

            {  L_, SNAN_P,    7,   'x', true,  false,   'l',   "SNANxxx"  },
            {  L_, SNAN_P,    7,   'x', true,  false,   'i',   "xxxSNAN"  },
            {  L_, SNAN_P,    7,   'x', true,  false,   'r',   "xxxSNAN"  },

            {  L_, SNAN_P,    7,   'x', true,  true,    'l',   "+SNANxx"  },
            {  L_, SNAN_P,    7,   'x', true,  true,    'i',   "+xxSNAN"  },
            {  L_, SNAN_P,    7,   'x', true,  true,    'r',   "xx+SNAN"  },

            {  L_, SNAN_N,    8,   ' ', false, false,   'l',   "-snan   "  },
            {  L_, SNAN_N,    8,   ' ', false, false,   'i',   "-   snan"  },
            {  L_, SNAN_N,    8,   ' ', false, false,   'r',   "   -snan"  },

            {  L_, SNAN_N,    8,   ' ', false, true,    'l',   "-snan   "  },
            {  L_, SNAN_N,    8,   ' ', false, true,    'i',   "-   snan"  },
            {  L_, SNAN_N,    8,   ' ', false, true,    'r',   "   -snan"  },

            {  L_, SNAN_N,    8,   ' ', true,  false,   'l',   "-SNAN   "  },
            {  L_, SNAN_N,    8,   ' ', true,  false,   'i',   "-   SNAN"  },
            {  L_, SNAN_N,    8,   ' ', true,  false,   'r',   "   -SNAN"  },

            {  L_, SNAN_N,    8,   ' ', true,  true,    'l',   "-SNAN   "  },
            {  L_, SNAN_N,    8,   ' ', true,  true,    'i',   "-   SNAN"  },
            {  L_, SNAN_N,    8,   ' ', true,  true,    'r',   "   -SNAN"  },

            {  L_, SNAN_N,    8,   '_', false, false,   'l',   "-snan___"  },
            {  L_, SNAN_N,    8,   '_', false, false,   'i',   "-___snan"  },
            {  L_, SNAN_N,    8,   '_', false, false,   'r',   "___-snan"  },

            {  L_, SNAN_N,    8,   '_', false, true,    'l',   "-snan___"  },
            {  L_, SNAN_N,    8,   '_', false, true,    'i',   "-___snan"  },
            {  L_, SNAN_N,    8,   '_', false, true,    'r',   "___-snan"  },

            {  L_, SNAN_N,    8,   '_', true,  false,   'l',   "-SNAN___"  },
            {  L_, SNAN_N,    8,   '_', true,  false,   'i',   "-___SNAN"  },
            {  L_, SNAN_N,    8,   '_', true,  false,   'r',   "___-SNAN"  },

            {  L_, SNAN_N,    8,   '_', true,  true,    'l',   "-SNAN___"  },
            {  L_, SNAN_N,    8,   '_', true,  true,    'i',   "-___SNAN"  },
            {  L_, SNAN_N,    8,   '_', true,  true,    'r',   "___-SNAN"  },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        const bool FIXED_STYLE[] = { false, true };

        const int NUM_FIXED_STYLE = static_cast<int>(sizeof FIXED_STYLE
                                                   / sizeof *FIXED_STYLE);

        for (int tj = 0; tj < NUM_FIXED_STYLE; ++tj) {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int     LINE       = DATA[ti].d_line;
                const int     WIDTH      = DATA[ti].d_width;
                const char    FILL       = DATA[ti].d_fill;
                const bool    SHOW_POS   = DATA[ti].d_showPos;
                const bool    UPPERCASE  = DATA[ti].d_uppercase;
                const bool    LEFT       = (DATA[ti].d_justification == 'l');
                const bool    INTERNAL   = (DATA[ti].d_justification == 'i');
                const bool    RIGHT      = (DATA[ti].d_justification == 'r');
                const char   *EXPECTED   = DATA[ti].d_expected;

                // Test with Decimal32.
                {
                    const BDEC::Decimal32 VALUE =
                          BDEC::DecimalImpUtil::parse32(DATA[ti].d_decimalStr);

                    bsl::ostringstream outdec(pa);

                    if (FIXED_STYLE[tj]) {
                        outdec << bsl::fixed;
                    }
                    else {
                        outdec << bsl::scientific;
                    }

                    if (UPPERCASE) { outdec << bsl::uppercase; }
                    if (SHOW_POS)  { outdec << bsl::showpos;   }
                    if (LEFT)      { outdec << bsl::left;      }
                    if (INTERNAL)  { outdec << bsl::internal;  }
                    if (RIGHT)     { outdec << bsl::right;     }

                    outdec.width(WIDTH);
                    outdec.fill(FILL);
                    outdec << bsl::fixed
                           << VALUE;

                    bsl::string ACTUAL(pa);
                    getStringFromStream(outdec, &ACTUAL);

                    ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
                    ASSERTV(outdec.good());
                }
                // Test with Decimal64.
                {
                    const BDEC::Decimal64 VALUE =
                          BDEC::DecimalImpUtil::parse64(DATA[ti].d_decimalStr);

                    bsl::ostringstream outdec(pa);

                    if (FIXED_STYLE[tj]) {
                        outdec << bsl::fixed;
                    }
                    else {
                        outdec << bsl::scientific;
                    }

                    if (UPPERCASE) { outdec << bsl::uppercase; }
                    if (SHOW_POS)  { outdec << bsl::showpos;   }
                    if (LEFT)      { outdec << bsl::left;      }
                    if (INTERNAL)  { outdec << bsl::internal;  }
                    if (RIGHT)     { outdec << bsl::right;     }

                    outdec.width(WIDTH);
                    outdec.fill(FILL);
                    outdec << bsl::fixed
                           << VALUE;

                    bsl::string ACTUAL(pa);
                    getStringFromStream(outdec, &ACTUAL);

                    ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
                    ASSERTV(outdec.good());
                }
                // Test with print.
                {
                    const BDEC::Decimal64 VALUE =
                          BDEC::DecimalImpUtil::parse64(DATA[ti].d_decimalStr);

                    bsl::ostringstream outdec(pa);

                    if (FIXED_STYLE[tj]) {
                        outdec << bsl::fixed;
                    }
                    else {
                        outdec << bsl::scientific;
                    }

                    if (UPPERCASE) { outdec << bsl::uppercase; }
                    if (SHOW_POS)  { outdec << bsl::showpos;   }
                    if (LEFT)      { outdec << bsl::left;      }
                    if (INTERNAL)  { outdec << bsl::internal;  }
                    if (RIGHT)     { outdec << bsl::right;     }

                    outdec.width(WIDTH);
                    outdec.fill(FILL);
                    outdec << bsl::fixed;
                    VALUE.print(outdec, 0, -1);

                    bsl::string ACTUAL(pa);
                    getStringFromStream(outdec, &ACTUAL);

                    ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
                    ASSERTV(outdec.good());
                }
                // Test with Decimal128.
                {
                    const BDEC::Decimal128 VALUE =
                          BDEC::DecimalImpUtil::parse128(DATA[ti].d_decimalStr);

                    bsl::ostringstream outdec(pa);

                    if (FIXED_STYLE[tj]) {
                        outdec << bsl::fixed;
                    }
                    else {
                        outdec << bsl::scientific;
                    }

                    if (UPPERCASE) { outdec << bsl::uppercase; }
                    if (SHOW_POS)  { outdec << bsl::showpos;   }
                    if (LEFT)      { outdec << bsl::left;      }
                    if (INTERNAL)  { outdec << bsl::internal;  }
                    if (RIGHT)     { outdec << bsl::right;     }

                    outdec.width(WIDTH);
                    outdec.fill(FILL);
                    outdec << bsl::fixed
                           << VALUE;

                    bsl::string ACTUAL(pa);
                    getStringFromStream(outdec, &ACTUAL);

                    ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
                    ASSERTV(outdec.good());
                }
            }
        }
    }
    {
        // Test max, min and denormalized Decimal32 values.
        typedef BDEC::Decimal32 Tested;

        typedef bsl::numeric_limits<Tested> Limits;

        const Tested MAX_P =  BDEC::DecimalImpUtil::max32();
        const Tested MAX_N = -BDEC::DecimalImpUtil::max32();
        const Tested MIN_P =  BDEC::DecimalImpUtil::min32();
        const Tested MIN_N = -BDEC::DecimalImpUtil::min32();
        const Tested DEN_P =  BDEC::DecimalImpUtil::denormMin32();
        const Tested DEN_N = -BDEC::DecimalImpUtil::denormMin32();
        const int MAX_PRS  =  Limits::max_precision;

        static const struct {
            int    d_line;
            Tested d_decimalValue;
            char   d_style;
            int    d_precision;
            int    d_expectedLength;
        } DATA[] = {
            //------------------------------------------------------
            // L | NUMBER | STYLE | PRECISION  | EXPECTED LENGTH
            //------------------------------------------------------
            //------------------------------------------------------
            // Fixed notation
            //------------------------------------------------------
            {  L_, MAX_P,    'F',   0,            97               },
            {  L_, MAX_P,    'F',   1,            99               },
            {  L_, MAX_P,    'F',   2,           100               },
            {  L_, MAX_P,    'F',   MAX_PRS,      98 + MAX_PRS     },
            {  L_, MAX_P,    'F',   MAX_PRS + 1,  98 + MAX_PRS + 1 },
            {  L_, MAX_N,    'F',   0,            98               },
            {  L_, MAX_N,    'F',   1,           100               },
            {  L_, MAX_N,    'F',   2,           101               },
            {  L_, MAX_N,    'F',   MAX_PRS,      99 + MAX_PRS     },
            {  L_, MAX_N,    'F',   MAX_PRS + 1,  99 + MAX_PRS + 1 },

            {  L_, MIN_P,    'F',   95,           97               },
            {  L_, MIN_P,    'F',   96,           98               },
            {  L_, MIN_N,    'F',   95,           98               },
            {  L_, MIN_N,    'F',   96,           99               },

            {  L_, DEN_P,    'F',   MAX_PRS,       2 + MAX_PRS     },
            {  L_, DEN_P,    'F',   MAX_PRS + 1,   2 + MAX_PRS + 1 },
            {  L_, DEN_N,    'F',   MAX_PRS,       3 + MAX_PRS     },
            {  L_, DEN_N,    'F',   MAX_PRS + 1,   3 + MAX_PRS + 1 },
            //------------------------------------------------------
            // Scientific notation
            //------------------------------------------------------
            {  L_, MAX_P,    'S',   6,            12               },
            {  L_, MAX_P,    'S',   7,            13               },
            {  L_, MAX_P,    'S',   MAX_PRS,       2 + MAX_PRS + 4 },
            {  L_, MAX_P,    'S',   MAX_PRS + 1,   2 + MAX_PRS + 5 },
            {  L_, MAX_N,    'S',   6,            13               },
            {  L_, MAX_N,    'S',   7,            14               },
            {  L_, MAX_N,    'S',   MAX_PRS,       3 + MAX_PRS + 4 },
            {  L_, MAX_N,    'S',   MAX_PRS + 1,   3 + MAX_PRS + 5 },

            {  L_, MIN_P,    'S',   0,             5               },
            {  L_, MIN_P,    'S',   1,             7               },
            {  L_, MIN_P,    'S',   2,             8               },
            {  L_, MIN_N,    'S',   0,             6               },
            {  L_, MIN_N,    'S',   1,             8               },
            {  L_, MIN_N,    'S',   2,             9               },

            {  L_, DEN_P,    'S',   0,             6               },
            {  L_, DEN_P,    'S',   1,             8               },
            {  L_, DEN_P,    'S',   2,             9               },
            {  L_, DEN_N,    'S',   0,             7               },
            {  L_, DEN_N,    'S',   1,             9               },
            {  L_, DEN_N,    'S',   2,            10               },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE      = DATA[ti].d_line;
            const Tested VALUE     = DATA[ti].d_decimalValue;
            const char   STYLE     = DATA[ti].d_style;
            const int    PRECISION = DATA[ti].d_precision;
            const int    EXPECTED  = DATA[ti].d_expectedLength;

            bsl::ostringstream outdec(pa);

            if ('F' == STYLE) { outdec << bsl::fixed;      }
            if ('S' == STYLE) { outdec << bsl::scientific; }

            outdec << bsl::setprecision(PRECISION) << VALUE;

            bsl::string ACTUAL(pa);
            getStringFromStream(outdec, &ACTUAL);
            int ACTUAL_LEN = static_cast<int>(ACTUAL.length());

            ASSERTV(LINE, ACTUAL_LEN, EXPECTED, ACTUAL_LEN == EXPECTED);
            ASSERTV(outdec.good());

            Tested ACTUAL_VALUE = BDEC::DecimalImpUtil::parse32(
                                                               ACTUAL.c_str());
            ASSERTV(LINE, ACTUAL_VALUE, VALUE, ACTUAL_VALUE == VALUE);
        }
    }
    {
        // Test max, min and denormalized Decimal64 values.

        typedef BDEC::Decimal64 Tested;

        typedef bsl::numeric_limits<Tested> Limits;

        const Tested MAX_P =  BDEC::DecimalImpUtil::max64();
        const Tested MAX_N = -BDEC::DecimalImpUtil::max64();
        const Tested MIN_P =  BDEC::DecimalImpUtil::min64();
        const Tested MIN_N = -BDEC::DecimalImpUtil::min64();
        const Tested DEN_P =  BDEC::DecimalImpUtil::denormMin64();
        const Tested DEN_N = -BDEC::DecimalImpUtil::denormMin64();
        const int MAX_PRS  =  Limits::max_precision;

        static const struct {
            int    d_line;
            Tested d_decimalValue;
            char   d_style;
            int    d_precision;
            int    d_expectedLength;
        } DATA[] = {
            //------------------------------------------------------
            // L | NUMBER | STYLE | PRECISION  | EXPECTED LENGTH
            //------------------------------------------------------
            //------------------------------------------------------
            // Fixed notation
            //------------------------------------------------------
            {  L_, MAX_P,    'F',   0,           385               },
            {  L_, MAX_P,    'F',   1,           387               },
            {  L_, MAX_P,    'F',   2,           388               },
            {  L_, MAX_P,    'F',   MAX_PRS,     386 + MAX_PRS     },
            {  L_, MAX_P,    'F',   MAX_PRS + 1, 386 + MAX_PRS + 1 },
            {  L_, MAX_N,    'F',   0,           386               },
            {  L_, MAX_N,    'F',   1,           388               },
            {  L_, MAX_N,    'F',   2,           389               },
            {  L_, MAX_N,    'F',   MAX_PRS,     387 + MAX_PRS     },
            {  L_, MAX_N,    'F',   MAX_PRS + 1, 387 + MAX_PRS + 1 },

            {  L_, MIN_P,    'F',   383,         385               },
            {  L_, MIN_P,    'F',   384,         386               },
            {  L_, MIN_N,    'F',   383,         386               },
            {  L_, MIN_N,    'F',   384,         387               },

            {  L_, DEN_P,    'F',   MAX_PRS,       2 + MAX_PRS     },
            {  L_, DEN_N,    'F',   MAX_PRS,       3 + MAX_PRS     },
            //------------------------------------------------------
            // Scientific notation
            //------------------------------------------------------
            {  L_, MAX_P,    'S',   15,           22               },
            {  L_, MAX_P,    'S',   16,           23               },
            {  L_, MAX_P,    'S',   MAX_PRS,       2 + MAX_PRS + 5 },
            {  L_, MAX_P,    'S',   MAX_PRS + 1,   2 + MAX_PRS + 6 },

            {  L_, MAX_N,    'S',   15,           23               },
            {  L_, MAX_N,    'S',   16,           24               },
            {  L_, MAX_N,    'S',   MAX_PRS,       3 + MAX_PRS + 5 },
            {  L_, MAX_N,    'S',   MAX_PRS + 1,   3 + MAX_PRS + 6 },

            {  L_, MIN_P,    'S',   0,             6               },
            {  L_, MIN_P,    'S',   1,             8               },
            {  L_, MIN_P,    'S',   2,             9               },

            {  L_, MIN_N,    'S',   0,             7               },
            {  L_, MIN_N,    'S',   1,             9               },
            {  L_, MIN_N,    'S',   2,            10               },

            {  L_, DEN_P,    'S',   0,             6               },
            {  L_, DEN_P,    'S',   1,             8               },
            {  L_, DEN_P,    'S',   2,             9               },

            {  L_, DEN_N,    'S',   0,             7               },
            {  L_, DEN_N,    'S',   1,             9               },
            {  L_, DEN_N,    'S',   2,            10               },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE      = DATA[ti].d_line;
            const Tested VALUE     = DATA[ti].d_decimalValue;
            const char   STYLE     = DATA[ti].d_style;
            const int    PRECISION = DATA[ti].d_precision;
            const int    EXPECTED  = DATA[ti].d_expectedLength;

            bsl::ostringstream outdec(pa);

            if ('F' == STYLE) { outdec << bsl::fixed;      }
            if ('S' == STYLE) { outdec << bsl::scientific; }

            outdec << bsl::setprecision(PRECISION) << VALUE;

            bsl::string ACTUAL(pa);
            getStringFromStream(outdec, &ACTUAL);
            int ACTUAL_LEN = static_cast<int>(ACTUAL.length());

            ASSERTV(LINE, ACTUAL_LEN, EXPECTED, ACTUAL_LEN == EXPECTED);
            ASSERTV(outdec.good());

            Tested ACTUAL_VALUE = BDEC::DecimalImpUtil::parse64(
                                                               ACTUAL.c_str());
            ASSERTV(LINE, ACTUAL, ACTUAL_VALUE, VALUE, ACTUAL_VALUE == VALUE);

        }
    }
    {
        // Test max, min and denormalized Decimal128 values.

        typedef BDEC::Decimal128 Tested;

        typedef bsl::numeric_limits<Tested> Limits;

        const Tested MAX_P =  BDEC::DecimalImpUtil::max128();
        const Tested MAX_N = -BDEC::DecimalImpUtil::max128();
        const Tested MIN_P =  BDEC::DecimalImpUtil::min128();
        const Tested MIN_N = -BDEC::DecimalImpUtil::min128();
        const Tested DEN_P =  BDEC::DecimalImpUtil::denormMin128();
        const Tested DEN_N = -BDEC::DecimalImpUtil::denormMin128();
        const int MAX_PRS  =  Limits::max_precision;

        static const struct {
            int    d_line;
            Tested d_decimalValue;
            char   d_style;
            int    d_precision;
            int    d_expectedLength;
        } DATA[] = {
            //-------------------------------------------------------
            // L | NUMBER | STYLE | PRECISION  | EXPECTED LENGTH
            //-------------------------------------------------------
            //-------------------------------------------------------
            // Fixed notation
            //-------------------------------------------------------
            {  L_, MAX_P,    'F',   0,           6145               },
            {  L_, MAX_P,    'F',   1,           6147               },
            {  L_, MAX_P,    'F',   2,           6148               },
            {  L_, MAX_P,    'F',   MAX_PRS,     6146 + MAX_PRS     },
            {  L_, MAX_P,    'F',   MAX_PRS + 1, 6146 + MAX_PRS + 1 },
            {  L_, MAX_N,    'F',   0,           6146               },
            {  L_, MAX_N,    'F',   1,           6148               },
            {  L_, MAX_N,    'F',   2,           6149               },
            {  L_, MAX_N,    'F',   MAX_PRS,     6147 + MAX_PRS     },
            {  L_, MAX_N,    'F',   MAX_PRS + 1, 6147 + MAX_PRS + 1 },

            {  L_, MIN_P,    'F',   6143,        6145               },
            {  L_, MIN_P,    'F',   6144,        6146               },
            {  L_, MIN_N,    'F',   6143,        6146               },
            {  L_, MIN_N,    'F',   6144,        6147               },

            {  L_, DEN_P,    'F',   MAX_PRS,        2 + MAX_PRS     },
            {  L_, DEN_N,    'F',   MAX_PRS,        3 + MAX_PRS     },
            //-------------------------------------------------------
            // Scientific notation
            //-------------------------------------------------------
            {  L_, MAX_P,    'S',   33,            41               },
            {  L_, MAX_P,    'S',   34,            42               },
            {  L_, MAX_P,    'S',   MAX_PRS,        2 + MAX_PRS + 6 },
            {  L_, MAX_P,    'S',   MAX_PRS + 1,    2 + MAX_PRS + 7 },

            {  L_, MAX_N,    'S',   33,            42               },
            {  L_, MAX_N,    'S',   34,            43               },
            {  L_, MAX_N,    'S',   MAX_PRS,        3 + MAX_PRS + 6 },
            {  L_, MAX_N,    'S',   MAX_PRS + 1,    3 + MAX_PRS + 7 },

            {  L_, MIN_P,    'S',   0,              7               },
            {  L_, MIN_P,    'S',   1,              9               },
            {  L_, MIN_P,    'S',   2,             10               },

            {  L_, MIN_N,    'S',   0,              8               },
            {  L_, MIN_N,    'S',   1,             10               },
            {  L_, MIN_N,    'S',   2,             11               },

            {  L_, DEN_P,    'S',   0,              7               },
            {  L_, DEN_P,    'S',   1,              9               },
            {  L_, DEN_P,    'S',   2,             10               },

            {  L_, DEN_N,    'S',   0,              8               },
            {  L_, DEN_N,    'S',   1,             10               },
            {  L_, DEN_N,    'S',   2,             11               },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE      = DATA[ti].d_line;
            const Tested VALUE     = DATA[ti].d_decimalValue;
            const char   STYLE     = DATA[ti].d_style;
            const int    PRECISION = DATA[ti].d_precision;
            const int    EXPECTED  = DATA[ti].d_expectedLength;

            bsl::ostringstream outdec(pa);

            if ('F' == STYLE) { outdec << bsl::fixed;      }
            if ('S' == STYLE) { outdec << bsl::scientific; }

            outdec << bsl::setprecision(PRECISION) << VALUE;

            bsl::string ACTUAL(pa);
            getStringFromStream(outdec, &ACTUAL);
            int ACTUAL_LEN = static_cast<int>(ACTUAL.length());

            ASSERTV(LINE, ACTUAL_LEN, EXPECTED, ACTUAL_LEN == EXPECTED);
            ASSERTV(outdec.good());

            Tested ACTUAL_VALUE = BDEC::DecimalImpUtil::parse128(
                                                               ACTUAL.c_str());
            ASSERTV(LINE, ACTUAL, ACTUAL_VALUE, VALUE, ACTUAL_VALUE == VALUE);

        }
    }
#define DFP(X) BDLDFP_DECIMAL_DL(X)
    { // C-20
        typedef BDEC::Decimal128 Tested;

        static const struct {
            int         d_line;
            Tested      d_decimalValue;
            const char *d_expected;
        } DATA[] = {
            //----------------------------------------------
            // L  | NUMBER             | EXPECTED
            //----------------------------------------------
            {  L_,  DFP(123456789E+2),  "1.23456789e+10"   },
            {  L_,  DFP(123456789E+1),  "1.23456789e+09"   },
            {  L_,  DFP(123456789E-0),  "123456789"        },
            {  L_,  DFP(123456789E-1),  "12345678.9"       },
            {  L_,  DFP(123456789E-2),  "1234567.89"       },
            {  L_,  DFP(123456789E-3),  "123456.789"       },
            {  L_,  DFP(123456789E-4),  "12345.6789"       },
            {  L_,  DFP(123456789E-5),  "1234.56789"       },
            {  L_,  DFP(123456789E-6),  "123.456789"       },
            {  L_,  DFP(123456789E-7),  "12.3456789"       },
            {  L_,  DFP(123456789E-8),  "1.23456789"       },
            {  L_,  DFP(123456789E-9),  "0.123456789"      },
            {  L_,  DFP(123456789E-10), "0.0123456789"     },
            {  L_,  DFP(123456789E-11), "0.00123456789"    },
            {  L_,  DFP(123456789E-12), "0.000123456789"   },
            {  L_,  DFP(123456789E-13), "0.0000123456789"  },
            {  L_,  DFP(123456789E-14), "0.00000123456789" },
            {  L_,  DFP(123456789E-15), "1.23456789e-07"   },
            {  L_,  DFP(123456789E-16), "1.23456789e-08"   },

            {  L_,  DFP(0.042000000),   "0.042000000"      },
            {  L_,  DFP(0.04200000),    "0.04200000"       },
            {  L_,  DFP(0.0420000),     "0.0420000"        },
            {  L_,  DFP(0.042000),      "0.042000"         },
            {  L_,  DFP(0.04200),       "0.04200"          },
            {  L_,  DFP(0.0420),        "0.0420"           },
            {  L_,  DFP(0.042),         "0.042"            },
            {  L_,  DFP(0.42),          "0.42"             },
            {  L_,  DFP(4.2),           "4.2"              },
            {  L_,  DFP(42E+0),         "42"               },
            {  L_,  DFP(42.0),          "42.0"             },
            {  L_,  DFP(42.00),         "42.00"            },
            {  L_,  DFP(42.000),        "42.000"           },
            {  L_,  DFP(42.0000),       "42.0000"          },
            {  L_,  DFP(42.00000),      "42.00000"         },
            {  L_,  DFP(42.000000),     "42.000000"        },
            {  L_,  DFP(42.0000000),    "42.0000000"       },
            {  L_,  DFP(42.00000000),   "42.00000000"      },
            {  L_,  DFP(42.000000000),  "42.000000000"     },

            {  L_,  DFP(0.0),           "0.0"              },
            {  L_,  DFP(0.00),          "0.00"             },
            {  L_,  DFP(0.000),         "0.000"            },
            {  L_,  DFP(0.0000),        "0.0000"           },
            {  L_,  DFP(0.00000),       "0.00000"          },
            {  L_,  DFP(0.000000),      "0.000000"         },
            {  L_,  DFP(0.0000000),     "0e-07"            },
            {  L_,  DFP(0.00000000),    "0e-08"            },
        };

        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE   = DATA[ti].d_line;
            const Tested       VALUE  = DATA[ti].d_decimalValue;
            const bsl::string  EXPECTED(DATA[ti].d_expected, pa);
            bsl::ostringstream outdec(pa);

            outdec << VALUE;

            bsl::string ACTUAL(pa);
            getStringFromStream(outdec, &ACTUAL);

            ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
            ASSERTV(outdec.good());

            if (veryVerbose) {
                P_(ACTUAL) P(EXPECTED);
            }
        }
    }
#undef DFP
    // Bug in Studio Studio's C++ standard library: 'ostreambuf_iterator'
    // doesn't set the 'failed' attribute when the iterator reaches the end of
    // EOF of the 'streambuf'.  Therefore, 'operator<<' for the decimal types
    // does not set the 'fail' and 'bad' bits when streaming to an 'ostream'
    // with a 'streambuf' that is not large enough.  This is consistent with
    // the behavior for 'int' and 'double'.  Note that the bug does not exist
    // when using stlport.

#if (!defined(BSLS_PLATFORM_OS_SUNOS) &&                                      \
     !defined(BSLS_PLATFORM_OS_SOLARIS)) ||                                   \
    defined(BDE_BUILD_TARGET_STLPORT)

    {
        {
            char buffer[4];
            bdlsb::FixedMemOutStreamBuf obuf(buffer, 4);
            bsl::ostream out(&obuf);
            BDEC::Decimal64 value = BDLDFP_DECIMAL_DD(-1.0);
            out << bsl::setprecision(1) << bsl::fixed << value;
            ASSERTV(out.fail(), !out.fail());
            ASSERTV(out.bad(), !out.bad());
        }
        {
            char buffer[4];
            bdlsb::FixedMemOutStreamBuf obuf(buffer, 3);
            bsl::ostream out(&obuf);
            BDEC::Decimal32 value = BDLDFP_DECIMAL_DF(-1.0);
            out << bsl::setprecision(1) << bsl::fixed << value;
            ASSERTV(out.fail(), out.fail());
            ASSERTV(out.bad(), out.bad());
        }
        {
            char buffer[4];
            bdlsb::FixedMemOutStreamBuf obuf(buffer, 3);
            bsl::ostream out(&obuf);
            BDEC::Decimal64 value = BDLDFP_DECIMAL_DD(-1.0);
            out << bsl::setprecision(1) << bsl::fixed << value;
            ASSERTV(out.fail(), out.fail());
            ASSERTV(out.bad(), out.bad());
        }
        {
            char buffer[4];
            bdlsb::FixedMemOutStreamBuf obuf(buffer, 3);
            bsl::ostream out(&obuf);
            BDEC::Decimal128 value = BDLDFP_DECIMAL_DL(-1.0);
            out << bsl::setprecision(1) << bsl::fixed << value;
            ASSERTV(out.fail(), out.fail());
            ASSERTV(out.bad(), out.bad());
        }
    }
#endif

#define DFP(X) BDLDFP_DECIMAL_DF(X)
    {
        // Test output to wide stream.  (P-4)

        typedef BDEC::Decimal32      Tested;
        typedef BDEC::DecimalImpUtil Util;

        const Tested  MAX_P =  Util::max32();
        const Tested  MAX_N = -Util::max32();
        const Tested  MIN_P =  Util::min32();
        const Tested  MIN_N = -Util::min32();
        const Tested  DEN_P =  Util::denormMin32();
        const Tested  DEN_N = -Util::denormMin32();
        const Tested  INF_P =  Util::infinity32();
        const Tested  INF_N = -Util::infinity32();
        const Tested  NAN_P =  Util::quietNaN32();
        const Tested  NAN_N = -Util::quietNaN32();
        const Tested SNAN_P =  Util::signalingNaN32();
        const Tested SNAN_N = -Util::signalingNaN32();

        static const struct {
            int            d_line;
            Tested         d_decimalValue;
            char           d_style;
            int            d_precision;
            const wchar_t *d_expected;
        } DATA[] = {
            //---------------------------------------------------------
            // L | NUMBER     | STYLE | PRECISION  | EXPECTED
            //---------------------------------------------------------
            //---------------------------------------------------------
            // Fixed notation
            //---------------------------------------------------------
            {  L_, DFP(42.5),    'F',      0,        L"43"            },
            {  L_, DFP(42.5),    'F',      1,        L"42.5"          },
            {  L_, DFP(42.5),    'F',      2,        L"42.50"         },
            {  L_, DFP(-0.0425), 'F',      3,        L"-0.043"        },
            {  L_, DFP(-0.0425), 'F',      4,        L"-0.0425"       },
            {  L_, DFP(-0.0425), 'F',      5,        L"-0.04250"      },

            {  L_,  INF_P,       'F',      0,         L"inf"          },
            {  L_,  INF_N,       'F',      0,        L"-inf"          },
            {  L_,  NAN_P,       'F',      0,         L"nan"          },
            {  L_,  NAN_N,       'F',      0,        L"-nan"          },
            {  L_, SNAN_P,       'F',      0,         L"snan"         },
            {  L_, SNAN_N,       'F',      0,        L"-snan"         },
            //---------------------------------------------------------
            // Scientific notation
            //---------------------------------------------------------
            {  L_, DFP(42.5),    'S',      0,        L"4e+01"         },
            {  L_, DFP(42.5),    'S',      1,        L"4.3e+01"       },
            {  L_, DFP(42.5),    'S',      2,        L"4.25e+01"      },
            {  L_, DFP(42.5),    'S',      3,        L"4.250e+01"     },
            {  L_, DFP(-0.0425), 'S',      0,        L"-4e-02"        },
            {  L_, DFP(-0.0425), 'S',      1,        L"-4.3e-02"      },
            {  L_, DFP(-0.0425), 'S',      2,        L"-4.25e-02"     },
            {  L_, DFP(-0.0425), 'S',      3,        L"-4.250e-02"    },

            {  L_,  MAX_P,       'S',      6,         L"9.999999e+96" },
            {  L_,  MAX_N,       'S',      6,        L"-9.999999e+96" },
            {  L_,  MIN_P,       'S',      6,         L"1.000000e-95" },
            {  L_,  MIN_N,       'S',      6,        L"-1.000000e-95" },
            {  L_,  DEN_P,       'S',      0,         L"1e-101"       },
            {  L_,  DEN_N,       'S',      0,        L"-1e-101"       },

            {  L_,  INF_P,       'S',      0,         L"inf"          },
            {  L_,  INF_N,       'S',      0,        L"-inf"          },
            {  L_,  NAN_P,       'S',      0,         L"nan"          },
            {  L_,  NAN_N,       'S',      0,        L"-nan"          },
            {  L_, SNAN_P,       'S',      0,         L"snan"         },
            {  L_, SNAN_N,       'S',      0,        L"-snan"         },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE      = DATA[ti].d_line;
            const Tested       VALUE     = DATA[ti].d_decimalValue;
            const char         STYLE     = DATA[ti].d_style;
            const int          PRECISION = DATA[ti].d_precision;
            const bsl::wstring EXPECTED(DATA[ti].d_expected, pa);

            bsl::wostringstream outdec(pa);

            if ('F' == STYLE) { outdec << bsl::fixed;      }
            if ('S' == STYLE) { outdec << bsl::scientific; }

            outdec << bsl::setprecision(PRECISION) << VALUE;

            bsl::wstring ACTUAL(pa);
            getStringFromStream(outdec, &ACTUAL);

            ASSERTV(LINE, ACTUAL == EXPECTED);
            ASSERTV(outdec.good());
        }
    }
#undef DFP

}

void TestDriver::testCase3()
{
    // ------------------------------------------------------------------------
    // TESTING 'Decimal128'
    //   Ensure that 'Decimal128' functions correctly
    //
    // Concerns:
    //: 1 'Decimal128' operations are properly forwarded to implementation
    //: 2 All possible operand combinations work correctly.
    //
    // Plan:
    //: 1 Expected value testing will be used.
    //:
    //: 2 All forms of LHS and RHS combinations for all free operators will
    //:   be tested.
    //:
    //: 3 Every member operator will be tested
    //
    // Testing:
    //   'class Decimal128'
    // ------------------------------------------------------------------------

    if (verbose) bsl::cout << "\nTesting class Decimal128"
                            << "\n========================" << bsl::endl;

    if (veryVerbose) bsl::cout << "Constructors" << bsl::endl;

    if (veryVeryVerbose) bsl::cout << "Copy/convert" << bsl::endl;
    {
        const BDEC::Decimal32  c32  = BDEC::Decimal32(32);
        const BDEC::Decimal64  c64  = BDEC::Decimal64(64);
        const BDEC::Decimal128 c128 = BDEC::Decimal128(128);

        ASSERT(BDLDFP_DECIMAL_DL( 32.0) == BDEC::Decimal128(c32));
        ASSERT(BDLDFP_DECIMAL_DL( 64.0) == BDEC::Decimal128(c64));
        ASSERT(BDLDFP_DECIMAL_DL(128.0) == BDEC::Decimal128(c128));
    }

    if (veryVeryVerbose) bsl::cout << "Integral" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DL(  0.0) == BDEC::Decimal128());  // default
    ASSERT(BDLDFP_DECIMAL_DL(-42.0) == BDEC::Decimal128(-42)); // int
    ASSERT(BDLDFP_DECIMAL_DL( 42.0) == BDEC::Decimal128(42u)); // unsigned
    ASSERT(BDLDFP_DECIMAL_DL(-42.0) == BDEC::Decimal128(-42l)); // long
    ASSERT(BDLDFP_DECIMAL_DL( 42.0) == BDEC::Decimal128(42ul)); // ulong
    ASSERT(BDLDFP_DECIMAL_DL(-42.0) == BDEC::Decimal128(-42ll)); //longlong
    ASSERT(BDLDFP_DECIMAL_DL( 42.0) == BDEC::Decimal128(42ull)); // ulongl

    BDEC::Decimal128 cDefault;
    BDEC::Decimal128 cExpectedDefault = BDLDFP_DECIMAL_DL(0e-6176);
    ASSERTV(cDefault, cExpectedDefault,
            0 == memcmp(&cDefault, &cExpectedDefault, sizeof(cDefault)));

    BDEC::Decimal128 cZero(0);
    BDEC::Decimal128 cExpectedZero = BDLDFP_DECIMAL_DL(0e0);
    ASSERTV(cZero, cExpectedZero,
            0 == memcmp(&cZero, &cExpectedZero, sizeof(cZero)));

    ASSERTV(cDefault, cZero, 0 != memcmp(&cDefault, &cZero, sizeof(cDefault)));

    if (veryVeryVerbose) bsl::cout << "Binary FP" << bsl::endl;

    // Note that to test binary-float taking constructors I use numbers
    // that can be represented exactly in both binary and decimal FP.

    ASSERT(BDLDFP_DECIMAL_DL(4.25) == BDEC::Decimal128(4.25f)); // float
    ASSERT(BDLDFP_DECIMAL_DL(4.25) == BDEC::Decimal128(4.25)); // double

    if (veryVeryVerbose) bsl::cout << "Decimal FP" << bsl::endl;

    LOOP2_ASSERT(BDLDFP_DECIMAL_DL(-42.0), BDLDFP_DECIMAL_DF(-42.0),
                 BDLDFP_DECIMAL_DL(-42.0) == BDLDFP_DECIMAL_DF(-42.0));
                                                              // Decimal32
    ASSERT(BDLDFP_DECIMAL_DL(42.0) == BDLDFP_DECIMAL_DF(42.0));
    ASSERT(BDLDFP_DECIMAL_DL(4.2) == BDLDFP_DECIMAL_DF(4.2));
    ASSERT(BDLDFP_DECIMAL_DL(4.2e9) == BDLDFP_DECIMAL_DF(4.2e9));

    ASSERT(BDLDFP_DECIMAL_DL(-42.0) == BDLDFP_DECIMAL_DD(-42.0));// Dec64
    ASSERT(BDLDFP_DECIMAL_DL(42.0) == BDLDFP_DECIMAL_DD(42.0));
    ASSERT(BDLDFP_DECIMAL_DL(4.2) == BDLDFP_DECIMAL_DD(4.2));
    ASSERT(BDLDFP_DECIMAL_DL(4.2e9) == BDLDFP_DECIMAL_DD(4.2e9));

    if (veryVerbose) bsl::cout << "Propriatery accessors" << bsl::endl;
    {

        BDEC::Decimal128 d128(42);
        ASSERT((void*)d128.data() == (void*)&d128);

    // XLC versions prior to 12.0 incorrectly pass decimal128 values in
    // some contexts (0x0c00 -> 12.00)
#if defined(BSLS_PLATFORM_CMP_IBM) && (BSLS_PLATFORM_CMP_VERSION >= 0x0c00)
        ASSERTV(BDEC::Decimal128(d128.value()) == BDEC::Decimal128(42));
#endif
    }

    if (veryVerbose) bsl::cout << "Operator==" << bsl::endl;

    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) == BDLDFP_DECIMAL_DL(5.0)));
    ASSERT(BDLDFP_DECIMAL_DL(4.0) == BDLDFP_DECIMAL_DL(4.0));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) == BDLDFP_DECIMAL_DL(-9.345e27));

    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) == BDLDFP_DECIMAL_DD(5.0)));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) == BDLDFP_DECIMAL_DD(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) == BDLDFP_DECIMAL_DL(5.0)));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) == BDLDFP_DECIMAL_DL(-9.345e27));

    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) == BDLDFP_DECIMAL_DF(5.0)));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) == BDLDFP_DECIMAL_DF(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) == BDLDFP_DECIMAL_DL(5.0)));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) == BDLDFP_DECIMAL_DL(-9.345e27));

    if (veryVerbose) bsl::cout << "Operator!=" << bsl::endl;

    LOOP2_ASSERT(BDLDFP_DECIMAL_DL(4.0), BDLDFP_DECIMAL_DL(5.0),
                 BDLDFP_DECIMAL_DL(4.0) != BDLDFP_DECIMAL_DL(5.0));
    LOOP2_ASSERT(BDLDFP_DECIMAL_DL(7.0), BDLDFP_DECIMAL_DL(5.0),
                 BDLDFP_DECIMAL_DL(7.0) != BDLDFP_DECIMAL_DL(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DL(-9.345e27) !=
              BDLDFP_DECIMAL_DL(-9.345e27)));

    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) == BDLDFP_DECIMAL_DD(5.0)));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) == BDLDFP_DECIMAL_DD(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) == BDLDFP_DECIMAL_DL(5.0)));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) == BDLDFP_DECIMAL_DL(-9.345e27));

    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) == BDLDFP_DECIMAL_DF(5.0)));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) == BDLDFP_DECIMAL_DF(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) == BDLDFP_DECIMAL_DL(5.0)));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) == BDLDFP_DECIMAL_DL(-9.345e27));

    if (veryVerbose) bsl::cout << "Operator<" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DL(4.0) < BDLDFP_DECIMAL_DL(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DL(-9.345e27) <
              BDLDFP_DECIMAL_DL(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DL(5.0) < BDLDFP_DECIMAL_DL(4.0)));

    ASSERT(BDLDFP_DECIMAL_DL(4.0) < BDLDFP_DECIMAL_DD(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DL(-9.345e27) <
              BDLDFP_DECIMAL_DD(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DL(5.0) < BDLDFP_DECIMAL_DD(4.0)));
    ASSERT(BDLDFP_DECIMAL_DD(4.0) < BDLDFP_DECIMAL_DL(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DD(-9.345e27) <
              BDLDFP_DECIMAL_DL(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DD(5.0) < BDLDFP_DECIMAL_DL(4.0)));

    ASSERT(BDLDFP_DECIMAL_DL(4.0) < BDLDFP_DECIMAL_DF(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DL(-9.345e27) <
              BDLDFP_DECIMAL_DF(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DL(5.0) < BDLDFP_DECIMAL_DF(4.0)));
    ASSERT(BDLDFP_DECIMAL_DF(4.0) < BDLDFP_DECIMAL_DL(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DF(-9.345e27) <
              BDLDFP_DECIMAL_DL(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DF(5.0) < BDLDFP_DECIMAL_DL(4.0)));

    if (veryVerbose) bsl::cout << "Operator>" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DL(5.0) > BDLDFP_DECIMAL_DL(4.0));
    ASSERT(! (BDLDFP_DECIMAL_DL(-9.345e27) >
              BDLDFP_DECIMAL_DL(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) > BDLDFP_DECIMAL_DL(5.0)));

    ASSERT(BDLDFP_DECIMAL_DL(5.0) > BDLDFP_DECIMAL_DD(4.0));
    ASSERT(! (BDLDFP_DECIMAL_DL(-9.345e27) >
              BDLDFP_DECIMAL_DD(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) > BDLDFP_DECIMAL_DD(5.0)));
    ASSERT(BDLDFP_DECIMAL_DD(5.0) > BDLDFP_DECIMAL_DL(4.0));
    ASSERT(! (BDLDFP_DECIMAL_DD(-9.345e27) >
              BDLDFP_DECIMAL_DL(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) > BDLDFP_DECIMAL_DL(5.0)));

    ASSERT(BDLDFP_DECIMAL_DL(5.0) > BDLDFP_DECIMAL_DF(4.0));
    ASSERT(! (BDLDFP_DECIMAL_DL(-9.345e27) >
              BDLDFP_DECIMAL_DF(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) > BDLDFP_DECIMAL_DF(5.0)));
    ASSERT(BDLDFP_DECIMAL_DF(5.0) > BDLDFP_DECIMAL_DL(4.0));
    ASSERT(! (BDLDFP_DECIMAL_DF(-9.345e27) >
              BDLDFP_DECIMAL_DL(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) > BDLDFP_DECIMAL_DL(5.0)));

    if (veryVerbose) bsl::cout << "Operator<=" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DL(4.0) <= BDLDFP_DECIMAL_DL(5.0));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) <= BDLDFP_DECIMAL_DL(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DL(5.0) <= BDLDFP_DECIMAL_DL(4.0)));

    ASSERT(BDLDFP_DECIMAL_DL(4.0) <= BDLDFP_DECIMAL_DD(5.0));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) <= BDLDFP_DECIMAL_DD(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DL(5.0) <= BDLDFP_DECIMAL_DD(4.0)));
    ASSERT(BDLDFP_DECIMAL_DD(4.0) <= BDLDFP_DECIMAL_DL(5.0));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) <= BDLDFP_DECIMAL_DL(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DD(5.0) <= BDLDFP_DECIMAL_DL(4.0)));

    ASSERT(BDLDFP_DECIMAL_DL(4.0) <= BDLDFP_DECIMAL_DF(5.0));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) <= BDLDFP_DECIMAL_DF(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DL(5.0) <= BDLDFP_DECIMAL_DF(4.0)));
    ASSERT(BDLDFP_DECIMAL_DF(4.0) <= BDLDFP_DECIMAL_DL(5.0));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) <= BDLDFP_DECIMAL_DL(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DF(5.0) <= BDLDFP_DECIMAL_DL(4.0)));

    if (veryVerbose) bsl::cout << "Operator>=" << bsl::endl;

    LOOP2_ASSERT(BDLDFP_DECIMAL_DL(5.0),   BDLDFP_DECIMAL_DL(4.0),
                 BDLDFP_DECIMAL_DL(5.0) >= BDLDFP_DECIMAL_DL(4.0));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) >= BDLDFP_DECIMAL_DL(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) >= BDLDFP_DECIMAL_DL(5.0)));

    ASSERT(BDLDFP_DECIMAL_DL(5.0) >= BDLDFP_DECIMAL_DD(4.0));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) >= BDLDFP_DECIMAL_DD(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) >= BDLDFP_DECIMAL_DD(5.0)));
    ASSERT(BDLDFP_DECIMAL_DD(5.0) >= BDLDFP_DECIMAL_DL(4.0));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) >= BDLDFP_DECIMAL_DL(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) >= BDLDFP_DECIMAL_DL(5.0)));

    ASSERT(BDLDFP_DECIMAL_DL(5.0) >= BDLDFP_DECIMAL_DF(4.0));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) >= BDLDFP_DECIMAL_DF(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) >= BDLDFP_DECIMAL_DF(5.0)));
    ASSERT(BDLDFP_DECIMAL_DF(5.0) >= BDLDFP_DECIMAL_DL(4.0));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) >= BDLDFP_DECIMAL_DL(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) >= BDLDFP_DECIMAL_DL(5.0)));

    // TODO TBD: Note that I am testing with the maximum number of
    // significant digits and I actually expect stream output to do no
    // rounding.  That is wrong (see later why), but necessary to
    // remote-test the 'format' function and make sure it does not lose
    // digits or does some unwanted rounding.  When 'format' will be in the
    // lower utility, it will be tested directly.
    //
    // Expecting to see all digits is wrong because that is not how the
    // stream output should behave: it should print with the default
    // precision as it would for binary floating point *or* with the
    // implied precision of the cohort if that is larger.  AFAIU

    if (veryVerbose) bsl::cout << "Test stream out" << bsl::endl;
    {
        bsl::ostringstream  out(pa);

        BDEC::Decimal128 d1(BDLDFP_DECIMAL_DL(
                                -1.234567890123456789012345678901234e-24));
        out << bsl::setprecision(33) << d1;
        bsl::string s(pa);
        getStringFromStream(out, &s);
        LOOP2_ASSERT(
                decLower(s),
                "-1.234567890123456789012345678901234e-24",
                decLower(s) == "-1.234567890123456789012345678901234e-24");
    }

    if (veryVerbose) bsl::cout << "Test stream in" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("-1.234567890123456789012345678901234e-24", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 ==
              BDLDFP_DECIMAL_DL(-1.234567890123456789012345678901234e-24));
    }
    {
        bsl::istringstream  in(pa);
        bsl::string ins("-123456789012345678901234567890123.4", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 ==
              BDLDFP_DECIMAL_DL(-123456789012345678901234567890123.4));
    }

    // bdldfp does not know how to parse128("NaN") etc.
    if (veryVerbose) bsl::cout << "Test stream in NaN" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("NaN", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 != d1);
    }
    {
        bsl::istringstream  in(pa);
        bsl::string ins("nan", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 != d1);
    }

    if (veryVerbose) bsl::cout << "Test stream in Inf" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("Inf", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 > bsl::numeric_limits<BDEC::Decimal128>::max());
    }
    {
        bsl::istringstream  in(pa);
        bsl::string ins("inf", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 > bsl::numeric_limits<BDEC::Decimal128>::max());
    }

    if (veryVerbose) bsl::cout << "Test stream in -Inf" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("-Inf", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 < -bsl::numeric_limits<BDEC::Decimal128>::max());
    }
    {
        bsl::istringstream  in(pa);
        bsl::string ins("-inf", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 < -bsl::numeric_limits<BDEC::Decimal128>::max());
    }

    if (veryVerbose) bsl::cout << "Test stream in NaNa (bad)" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("NaNa", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(in.fail() == true);
    }

    if (veryVerbose) bsl::cout << "Test stream in Infinity" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("Infinity", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 > bsl::numeric_limits<BDEC::Decimal128>::max());
    }

    if (veryVerbose) bsl::cout << "Test stream in -Infinity" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("-Infinity", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 < -bsl::numeric_limits<BDEC::Decimal128>::max());
    }

    if (veryVerbose) bsl::cout << "Test stream in Infin (bad)" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("-Infin", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(in.fail() == true);
    }

    if (veryVerbose) bsl::cout << "Test wide stream out" << bsl::endl;
    {
        bsl::wostringstream  out(pa);
        BDEC::Decimal128 d1(BDLDFP_DECIMAL_DL(
                                -1.234567890123456789012345678901234e-24));
        out << bsl::setprecision(33) << d1;
        bsl::wstring s(pa);
        getStringFromStream(out, &s);
        ASSERT(decLower(s) == L"-1.234567890123456789012345678901234e-24");
    }

    if (veryVerbose) bsl::cout << "Test wide stream in" << bsl::endl;
    {
        bsl::wistringstream  in(pa);
        bsl::wstring ins(L"-1.234567890123456789012345678901234e-24", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 ==
              BDLDFP_DECIMAL_DL(-1.234567890123456789012345678901234e-24));
    }

    if (veryVerbose) bsl::cout << "Operator++" << bsl::endl;
    {
        BDEC::Decimal128 d1(BDLDFP_DECIMAL_DL(-5.0));
        BDEC::Decimal128 d2 = d1++;
        ASSERT(BDLDFP_DECIMAL_DL(-5.0) == d2);
        LOOP_ASSERT(d1, BDLDFP_DECIMAL_DL(-4.0) == d1);
        BDEC::Decimal128 d3 = ++d2;
        LOOP_ASSERT(d2, BDLDFP_DECIMAL_DL(-4.0) == d2);
        LOOP_ASSERT(d3, BDLDFP_DECIMAL_DL(-4.0) == d3);
    }

    if (veryVerbose) bsl::cout << "Operator--" << bsl::endl;
    {
        BDEC::Decimal128 d1(BDLDFP_DECIMAL_DL(-5.0));
        BDEC::Decimal128 d2 = d1--;
        ASSERT(BDLDFP_DECIMAL_DL(-5.0) == d2);
        LOOP_ASSERT(d1, BDLDFP_DECIMAL_DL(-6.0) == d1);
        BDEC::Decimal128 d3 = --d2;
        LOOP_ASSERT(d2, BDLDFP_DECIMAL_DL(-6.0) == d2);
        LOOP_ASSERT(d3, BDLDFP_DECIMAL_DL(-6.0) == d3);
    }

    if (veryVerbose) bsl::cout << "Unary-" << bsl::endl;
    {
        BDEC::Decimal128 d1(BDLDFP_DECIMAL_DL(-5.0));
        BDEC::Decimal128 d2(BDLDFP_DECIMAL_DL( 5.0));
        ASSERT(BDLDFP_DECIMAL_DL( 5.0) == -d1);
        ASSERT(BDLDFP_DECIMAL_DL(-5.0) == -d2);

        // Unary - must make +0 into -0

        bsl::ostringstream out(pa);
        out << -BDLDFP_DECIMAL_DL(0.0);
        bsl::string s(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, s[0] == '-'); // it is negative
        ASSERT(-BDLDFP_DECIMAL_DL(0.0) == BDEC::Decimal128(0)); // and 0
    }

    if (veryVerbose) bsl::cout << "Unary+" << bsl::endl;
    {
        BDEC::Decimal128 d1(BDLDFP_DECIMAL_DL(-5.0));
        BDEC::Decimal128 d2(BDLDFP_DECIMAL_DL( 5.0));
        ASSERT(BDLDFP_DECIMAL_DL(-5.0) == +d1);
        ASSERT(BDLDFP_DECIMAL_DL( 5.0) == +d2);

        // Unary + must make -0 into +0

        BDEC::Decimal128 negzero(-BDLDFP_DECIMAL_DL(0.0));
        bsl::ostringstream out(pa);
        out << +negzero;
        bsl::string s(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, s[0] != '+'); // it is positive
        ASSERT(-BDLDFP_DECIMAL_DL(0.0) == BDEC::Decimal128(0)); // and 0
    }

    if (veryVerbose) bsl::cout << "+=" << bsl::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) bsl::cout << "+=(int)" << bsl::endl;
        d += 1;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-4.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(unsigned int)" << bsl::endl;
        d += 2u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-2.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(long int)" << bsl::endl;
        d += -1l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-3.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(unsigned long int)"
                                       << bsl::endl;
        d += 5ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(2.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(long long int)" << bsl::endl;
        d += -2ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(0.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(unsigned long long int)"
                                << bsl::endl;
        d += 42ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(42.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(Decimal32)" << bsl::endl;
        d += BDLDFP_DECIMAL_DF(-22.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(20.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(Decimal64)" << bsl::endl;
        d += BDLDFP_DECIMAL_DD(3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(23.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(Decimal128)" << bsl::endl;
        d += BDLDFP_DECIMAL_DL(9.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(32.0) == d);

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode addition" << bsl::endl;
        if (veryVeryVerbose) { T_ bsl::cout << "+=(Decimal32)" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal32  rhs( BDLDFP_DECIMAL_DF(1.000001));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(2.0000010000000001));
            d128 += rhs;
            LOOP_ASSERT(d128, EXP == d128);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "+=(Decimal64)" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal64  rhs( BDLDFP_DECIMAL_DD(1.000000000000001));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(2.0000000000000011));
            d128 += rhs;
            LOOP_ASSERT(d128, EXP == d128);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "+=(Decimal128)" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal128 rhs( BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(2.0000000000000002));
            d128 += rhs;
            LOOP_ASSERT(d128, EXP == d128);
        }
    }

    if (veryVerbose) bsl::cout << "-=" << bsl::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) bsl::cout << "-=(int)" << bsl::endl;
        d -= 1;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-6.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(unsigned int)" << bsl::endl;
        d -= 2u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-8.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(long int)" << bsl::endl;
        d -= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(2.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(unsigned long int)"
                                       << bsl::endl;
        d -= 10ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-8.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(long long int)" << bsl::endl;
        d -= -8ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(0.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(unsigned long long int)"
                                << bsl::endl;
        d -= 42ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-42.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(Decimal32)" << bsl::endl;
        d -= BDLDFP_DECIMAL_DF(-22.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-20.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(Decimal64)" << bsl::endl;
        d -= BDLDFP_DECIMAL_DD(3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-23.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(Decimal128)" << bsl::endl;
        d -= BDLDFP_DECIMAL_DL(9.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-32.0) == d);

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode subtraction"
                      << bsl::endl;
        if (veryVeryVerbose) { T_ bsl::cout << "-=(Decimal32)" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(2.0000010000000001));
            BDEC::Decimal32  rhs( BDLDFP_DECIMAL_DF(1.000001));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(1.0000000000000001));
            d128 -= rhs;
            LOOP_ASSERT(d128, EXP == d128);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "-=(Decimal64)" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(2.0000000000000011));
            BDEC::Decimal64  rhs( BDLDFP_DECIMAL_DD(1.000000000000001));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(1.0000000000000001));
            d128 -= rhs;
            LOOP_ASSERT(d128, EXP == d128);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "-=(Decimal128)" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(2.0000000000000002));
            BDEC::Decimal128 rhs( BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(1.0000000000000001));
            d128 -= rhs;
            LOOP_ASSERT(d128, EXP == d128);
        }
    }

    if (veryVerbose) bsl::cout << "*=" << bsl::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) bsl::cout << "*=(int)" << bsl::endl;
        d *= -2;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(10.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(unsigned int)" << bsl::endl;
        d *= 2000u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(20000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(long int)" << bsl::endl;
        d *= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-200000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(unsigned long int)"
                                       << bsl::endl;
        d *= 3ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-600000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(long long int)" << bsl::endl;
        d *= -1ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(600000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(unsigned long long int)"
                                << bsl::endl;
        d *= 5ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(3000000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(Decimal32)" << bsl::endl;
        d *= BDLDFP_DECIMAL_DF(1e-5);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(30.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(Decimal64)" << bsl::endl;
        d *= BDLDFP_DECIMAL_DD(-3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-90.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(Decimal128)" << bsl::endl;
        d *= BDLDFP_DECIMAL_DL(2.4e-134);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-2.16e-132) == d);

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode multiplication"
                      << bsl::endl;
        if (veryVeryVerbose) { T_ bsl::cout << "*=(Decimal32)" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal32  rhs( BDLDFP_DECIMAL_DF(3.0));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(3.0000000000000003));
            d128 *= rhs;
            LOOP_ASSERT(d128, EXP == d128);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "*=(Decimal64)" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal64  rhs( BDLDFP_DECIMAL_DD(3.0));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(3.0000000000000003));
            d128 *= rhs;
            LOOP_ASSERT(d128, EXP == d128);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "*=(Decimal128)" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal128 rhs( BDLDFP_DECIMAL_DL(3.0));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(3.0000000000000003));
            d128 *= rhs;
            LOOP_ASSERT(d128, EXP == d128);
        }
    }

    if (veryVerbose) bsl::cout << "/=" << bsl::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) bsl::cout << "/=(int)" << bsl::endl;
        d /= -2;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(2.5) == d);

        if (veryVeryVerbose) bsl::cout << "/=(unsigned int)" << bsl::endl;
        d /= 2000u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(0.00125) == d);

        if (veryVeryVerbose) bsl::cout << "/=(long int)" << bsl::endl;
        d /= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-0.000125) == d);

        if (veryVeryVerbose) bsl::cout << "/=(unsigned long int)"
                                       << bsl::endl;
        d /= 5ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-0.000025) == d);

        if (veryVeryVerbose) bsl::cout << "/=(long long int)" << bsl::endl;
        d /= -5ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(0.000005) == d);

        if (veryVeryVerbose) bsl::cout << "/=(unsigned long long int)"
                                << bsl::endl;
        d /= 5ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(0.000001) == d);

        if (veryVeryVerbose) bsl::cout << "/=(Decimal32)" << bsl::endl;
        d /= BDLDFP_DECIMAL_DF(1e-5);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(0.1) == d);

        if (veryVeryVerbose) bsl::cout << "/=(Decimal64)" << bsl::endl;
        d /= BDLDFP_DECIMAL_DD(-5.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-0.02) == d);

        if (veryVeryVerbose) bsl::cout << "/=(Decimal128)" << bsl::endl;
        d /= BDLDFP_DECIMAL_DL(-2.5e-134);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(8.e131) == d);

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode division" << bsl::endl;
        if (veryVeryVerbose) { T_ bsl::cout << "/=(Decimal32)" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal32  rhs( BDLDFP_DECIMAL_DF(0.5));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(2.0000000000000002));
            d128 /= rhs;
            LOOP_ASSERT(d128, EXP == d128);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "/=(Decimal64)" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal64  rhs( BDLDFP_DECIMAL_DD(0.5));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(2.0000000000000002));
            d128 /= rhs;
            LOOP_ASSERT(d128, EXP == d128);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "/=(Decimal128)" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal128 rhs( BDLDFP_DECIMAL_DL(0.5));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(2.0000000000000002));
            d128 /= rhs;
            LOOP_ASSERT(d128, EXP == d128);
        }
    }

    if (veryVerbose) bsl::cout << "operator+" << bsl::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) bsl::cout << "dec + int" << bsl::endl;
        LOOP_ASSERT(d + 1, d + 1 == BDLDFP_DECIMAL_DL(-4.0));
        LOOP_ASSERT(1 + d, 1 + d == BDLDFP_DECIMAL_DL(-4.0));

        if (veryVeryVerbose) bsl::cout << "dec + unsigned int"
                                       << bsl::endl;
        LOOP_ASSERT(d + 10u, d + 10u == BDLDFP_DECIMAL_DL(5.0));
        LOOP_ASSERT(10u + d, 10u + d == BDLDFP_DECIMAL_DL(5.0));

        if (veryVeryVerbose) bsl::cout << "dec + long int"
                                       << bsl::endl;
        LOOP_ASSERT(d + 1l, d + 1l == BDLDFP_DECIMAL_DL(-4.0));
        LOOP_ASSERT(1l + d, 1l + d == BDLDFP_DECIMAL_DL(-4.0));

        if (veryVeryVerbose) bsl::cout << "dec + unsigned long int"
                                       << bsl::endl;
        LOOP_ASSERT(d + 10ul, d + 10ul == BDLDFP_DECIMAL_DL(5.0));
        LOOP_ASSERT(10ul + d, 10ul + d == BDLDFP_DECIMAL_DL(5.0));

        if (veryVeryVerbose) bsl::cout << "dec + long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d + 1ll, d + 1ll == BDLDFP_DECIMAL_DL(-4.0));
        LOOP_ASSERT(1ll + d, 1ll + d == BDLDFP_DECIMAL_DL(-4.0));

        if (veryVeryVerbose) bsl::cout << "dec + unsigned long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d + 10ull, d + 10ull == BDLDFP_DECIMAL_DL(5.0));
        LOOP_ASSERT(10ull + d, 10ull + d == BDLDFP_DECIMAL_DL(5.0));

        if (veryVeryVerbose) bsl::cout << "dec + dec128" << bsl::endl;
        LOOP_ASSERT(d + BDLDFP_DECIMAL_DL(-3.0),
                    d + BDLDFP_DECIMAL_DL(-3.0) ==
                    BDLDFP_DECIMAL_DL(-8.0));

        if (veryVeryVerbose) bsl::cout << "dec + dec32" << bsl::endl;
        LOOP_ASSERT(d + BDLDFP_DECIMAL_DF(-3.0),
                    d + BDLDFP_DECIMAL_DF(-3.0) ==
                    BDLDFP_DECIMAL_DL(-8.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-3.0) + d,
                    BDLDFP_DECIMAL_DF(-3.0) + d ==
                    BDLDFP_DECIMAL_DL(-8.0));

        if (veryVeryVerbose) bsl::cout << "dec + dec64" << bsl::endl;
        LOOP_ASSERT(d + BDLDFP_DECIMAL_DD(-3.0),
                    d + BDLDFP_DECIMAL_DD(-3.0) ==
                    BDLDFP_DECIMAL_DL(-8.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DD(-3.0) + d,
                    BDLDFP_DECIMAL_DD(-3.0) + d ==
                    BDLDFP_DECIMAL_DL(-8.0));

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode addition" << bsl::endl;
        if (veryVeryVerbose) { T_ bsl::cout << "dec + dec32" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal32  d32( BDLDFP_DECIMAL_DF(1.000001));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(2.0000010000000001));
            LOOP_ASSERT(d128 + d32, EXP == d128 + d32);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec32 + dec" << bsl::endl; }
        {
            BDEC::Decimal32  d32 (BDLDFP_DECIMAL_DF(1.000001));
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(2.0000010000000001));
            LOOP_ASSERT(d32 + d128, EXP == d32 + d128);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec + dec64" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal64  d64( BDLDFP_DECIMAL_DD(1.000000000000001));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(2.0000000000000011));
            LOOP_ASSERT(d128 + d64, EXP == d128 + d64);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec64 + dec" << bsl::endl; }
        {
            BDEC::Decimal64  d64( BDLDFP_DECIMAL_DD(1.000000000000001));
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(2.0000000000000011));
            LOOP_ASSERT(d64 + d128, EXP == d64 + d128);
        }
    }

    if (veryVerbose) bsl::cout << "operator-" << bsl::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) bsl::cout << "dec - int" << bsl::endl;
        LOOP_ASSERT(d - 1, d - 1 == BDLDFP_DECIMAL_DL(-6.0));
        LOOP_ASSERT(1 - d, 1 - d == BDLDFP_DECIMAL_DL( 6.0));

        if (veryVeryVerbose) bsl::cout << "dec - unsigned int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 10u, d - 10u == BDLDFP_DECIMAL_DL(-15.0));
        LOOP_ASSERT(10u - d, 10u - d == BDLDFP_DECIMAL_DL( 15.0));

        if (veryVeryVerbose) bsl::cout << "dec - long int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 1l, d - 1l == BDLDFP_DECIMAL_DL(-6.0));
        LOOP_ASSERT(1l - d, 1l - d == BDLDFP_DECIMAL_DL( 6.0));

        if (veryVeryVerbose) bsl::cout << "dec - unsigned long int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 10ul, d - 10ul == BDLDFP_DECIMAL_DL(-15.0));
        LOOP_ASSERT(10ul - d, 10ul - d == BDLDFP_DECIMAL_DL( 15.0));

        if (veryVeryVerbose) bsl::cout << "dec - long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 1ll, d - 1ll == BDLDFP_DECIMAL_DL(-6.0));
        LOOP_ASSERT(1ll - d, 1ll - d == BDLDFP_DECIMAL_DL( 6.0));

        if (veryVeryVerbose) bsl::cout << "dec - unsigned long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 10ull, d - 10ull == BDLDFP_DECIMAL_DL(-15.0));
        LOOP_ASSERT(10ull - d, 10ull - d == BDLDFP_DECIMAL_DL( 15.0));

        if (veryVeryVerbose) bsl::cout << "dec - dec128" << bsl::endl;
        LOOP_ASSERT(d - BDLDFP_DECIMAL_DL(-3.0),
                    d - BDLDFP_DECIMAL_DL(-3.0) ==
                    BDLDFP_DECIMAL_DL(-2.0));

        if (veryVeryVerbose) bsl::cout << "dec - dec32" << bsl::endl;
        LOOP_ASSERT(d - BDLDFP_DECIMAL_DF(-3.0),
                    d - BDLDFP_DECIMAL_DF(-3.0) ==
                    BDLDFP_DECIMAL_DL(-2.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-3.0) - d,
                    BDLDFP_DECIMAL_DF(-3.0) - d ==
                    BDLDFP_DECIMAL_DL( 2.0));

        if (veryVeryVerbose) bsl::cout << "dec - dec64" << bsl::endl;
        LOOP_ASSERT(d - BDLDFP_DECIMAL_DD(-3.0),
                    d - BDLDFP_DECIMAL_DD(-3.0) ==
                    BDLDFP_DECIMAL_DL(-2.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DD(-3.0) - d,
                    BDLDFP_DECIMAL_DD(-3.0) - d ==
                    BDLDFP_DECIMAL_DL( 2.0));

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode subtraction"
                      << bsl::endl;
        if (veryVeryVerbose) { T_ bsl::cout << "dec - dec32" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(3.0000020000000001));
            BDEC::Decimal32  d32( BDLDFP_DECIMAL_DF(1.000001));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(2.0000010000000001));
            LOOP_ASSERT(d128 - d32, EXP == d128 - d32);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec32 - dec" << bsl::endl; }
        {
            BDEC::Decimal32  d32 (BDLDFP_DECIMAL_DF(3.000001));
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(2.0000009999999999));
            LOOP_ASSERT(d32 - d128, EXP == d32 - d128);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec - dec64" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(3.0000000000000021));
            BDEC::Decimal64  d64( BDLDFP_DECIMAL_DD(1.000000000000001));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(2.0000000000000011));
            LOOP_ASSERT(d128 - d64, EXP == d128 - d64);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec64 - dec" << bsl::endl; }
        {
            BDEC::Decimal64  d64( BDLDFP_DECIMAL_DD(3.000000000000001));
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(2.0000000000000009));
            LOOP_ASSERT(d64 - d128, EXP == d64 - d128);
        }
    }

    if (veryVerbose) bsl::cout << "operator*" << bsl::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) bsl::cout << "dec * int" << bsl::endl;
        LOOP_ASSERT(d * -2, d * -2 == BDLDFP_DECIMAL_DD(10.0));
        LOOP_ASSERT(-2 * d, -2 * d == BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose) bsl::cout << "dec * unsigned int"
                                       << bsl::endl;
        LOOP_ASSERT(d * 10u, d * 10u == BDLDFP_DECIMAL_DD(-50.0));
        LOOP_ASSERT(10u * d, 10u * d == BDLDFP_DECIMAL_DD(-50.0));

        if (veryVeryVerbose) bsl::cout << "dec * long int" << bsl::endl;
        LOOP_ASSERT(d * -2l, d * -2l == BDLDFP_DECIMAL_DD(10.0));
        LOOP_ASSERT(-2l * d, -2l * d == BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose) bsl::cout << "dec * unsigned long int"
                                       << bsl::endl;
        LOOP_ASSERT(d * 10ul, d * 10ul == BDLDFP_DECIMAL_DD(-50.0));
        LOOP_ASSERT(10ul * d, 10ul * d == BDLDFP_DECIMAL_DD(-50.0));

        if (veryVeryVerbose) bsl::cout << "dec * long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d * -2ll, d * -2ll == BDLDFP_DECIMAL_DD(10.0));
        LOOP_ASSERT(-2ll * d, -2ll * d == BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose) bsl::cout << "dec * unsigned long long int"
                                << bsl::endl;
        LOOP_ASSERT(d * 10ull, d * 10ull == BDLDFP_DECIMAL_DD(-50.0));
        LOOP_ASSERT(10ull * d, 10ull * d == BDLDFP_DECIMAL_DD(-50.0));

        if (veryVeryVerbose) bsl::cout << "dec * dec64" << bsl::endl;
        LOOP_ASSERT(d * BDLDFP_DECIMAL_DD(-3.0),
                    d * BDLDFP_DECIMAL_DD(-3.0) ==
                    BDLDFP_DECIMAL_DD(15.0));

        if (veryVeryVerbose) bsl::cout << "dec * dec32" << bsl::endl;
        LOOP_ASSERT(d * BDLDFP_DECIMAL_DF(-3.0),
                    d * BDLDFP_DECIMAL_DF(-3.0) ==
                    BDLDFP_DECIMAL_DD(15.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-3.0) * d,
                    BDLDFP_DECIMAL_DF(-3.0) * d ==
                    BDLDFP_DECIMAL_DD(15.0));

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode multiplication"
                      << bsl::endl;
        if (veryVeryVerbose) { T_ bsl::cout << "dec * dec32" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal32  d32( BDLDFP_DECIMAL_DF(3.0));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(3.0000000000000003));
            LOOP_ASSERT(d128 * d32, EXP == d128 * d32);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec32 * dec" << bsl::endl; }
        {
            BDEC::Decimal32  d32 (BDLDFP_DECIMAL_DF(3.0));
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(3.0000000000000003));
            LOOP_ASSERT(d32 * d128, EXP == d32 * d128);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec * dec64" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal64  d64( BDLDFP_DECIMAL_DD(3.0));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(3.0000000000000003));
            LOOP_ASSERT(d128 * d64, EXP == d128 * d64);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec64 * dec" << bsl::endl; }
        {
            BDEC::Decimal64  d64( BDLDFP_DECIMAL_DD(3.0));
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(3.0000000000000003));
            LOOP_ASSERT(d64 * d128, EXP == d64 * d128);
        }
    }

    if (veryVerbose) bsl::cout << "operator/" << bsl::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) bsl::cout << "dec / int" << bsl::endl;
        LOOP_ASSERT(d / -2, d / -2 == BDLDFP_DECIMAL_DL(2.5));
        LOOP_ASSERT(-2 / d, -2 / d == BDLDFP_DECIMAL_DL(0.4));

        if (veryVeryVerbose) bsl::cout << "dec / unsigned int"
                                       << bsl::endl;
        LOOP_ASSERT(d / 100u, d / 100u == BDLDFP_DECIMAL_DL( -0.05));
        LOOP_ASSERT(100u / d, 100u / d == BDLDFP_DECIMAL_DL(-20.0));

        if (veryVeryVerbose) bsl::cout << "dec / long int" << bsl::endl;
        LOOP_ASSERT(d / -2l, d / -2l == BDLDFP_DECIMAL_DL(2.5));
        LOOP_ASSERT(-2l / d, -2l / d == BDLDFP_DECIMAL_DL(0.4));

        if (veryVeryVerbose) bsl::cout << "dec / unsigned long int"
                                       << bsl::endl;
        LOOP_ASSERT(d / 100ul, d / 100ul == BDLDFP_DECIMAL_DL( -0.05));
        LOOP_ASSERT(100ul / d, 100ul / d == BDLDFP_DECIMAL_DL(-20.0));

        if (veryVeryVerbose) bsl::cout << "dec / long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d / -2ll, d / -2ll == BDLDFP_DECIMAL_DL(2.5));
        LOOP_ASSERT(-2ll / d, -2ll / d == BDLDFP_DECIMAL_DL(0.4));

        if (veryVeryVerbose) bsl::cout << "dec / unsigned long long int"
                                << bsl::endl;
        LOOP_ASSERT(d / 100ull, d / 100ull == BDLDFP_DECIMAL_DL( -0.05));
        LOOP_ASSERT(100ull / d, 100ull / d == BDLDFP_DECIMAL_DL(-20.0));

        if (veryVeryVerbose) bsl::cout << "dec / dec128" << bsl::endl;
        LOOP_ASSERT(d / BDLDFP_DECIMAL_DL(-50.0),
                    d / BDLDFP_DECIMAL_DL(-50.0) ==
                    BDLDFP_DECIMAL_DL(0.1));

        if (veryVeryVerbose) bsl::cout << "dec / dec32" << bsl::endl;
        LOOP_ASSERT(d / BDLDFP_DECIMAL_DF(-50.0),
                    d / BDLDFP_DECIMAL_DF(-50.0) ==
                    BDLDFP_DECIMAL_DL(0.1));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-50.0) / d,
                    BDLDFP_DECIMAL_DF(-50.0) / d ==
                    BDLDFP_DECIMAL_DL(10.0));

        if (veryVeryVerbose) bsl::cout << "dec / dec64" << bsl::endl;
        LOOP_ASSERT(d / BDLDFP_DECIMAL_DD(-50.0),
                    d / BDLDFP_DECIMAL_DD(-50.0) ==
                    BDLDFP_DECIMAL_DL(0.1));
        LOOP_ASSERT(BDLDFP_DECIMAL_DD(-50.0) / d,
                    BDLDFP_DECIMAL_DD(-50.0) / d ==
                    BDLDFP_DECIMAL_DL(10.0));

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode division"
                      << bsl::endl;
        if (veryVeryVerbose) { T_ bsl::cout << "dec / dec32" << bsl::endl; }
        {
            BDEC::Decimal128 d128(BDLDFP_DECIMAL_DL(1.0000000000000001));
            BDEC::Decimal32  d32( BDLDFP_DECIMAL_DF(0.5));
            BDEC::Decimal128 EXP( BDLDFP_DECIMAL_DL(2.0000000000000002));
            LOOP_ASSERT(d128 / d32, EXP == d128 / d32);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec32 / dec" << bsl::endl; }
        {
            BDEC::Decimal32  d32 (BDLDFP_DECIMAL_DF(2.0));
            BDEC::Decimal128 d128(
                       BDLDFP_DECIMAL_DL(1.000000000000000000000000000000001));
            BDEC::Decimal128 EXP(
                       BDLDFP_DECIMAL_DL(1.999999999999999999999999999999998));
            LOOP_ASSERT(d32 / d128, EXP == d32 / d128);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec / dec64" << bsl::endl; }
        {
            BDEC::Decimal128 d128(
                       BDLDFP_DECIMAL_DL(2.000000000000000000000000000000001));
            BDEC::Decimal64  d64(
                       BDLDFP_DECIMAL_DD(1.000000000000001));
            BDEC::Decimal128 EXP(
                       BDLDFP_DECIMAL_DL(1.999999999999998000000000000002001));
            LOOP_ASSERT(d128 / d64, EXP == d128 / d64);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec64 / dec" << bsl::endl; }
        {
            BDEC::Decimal64  d64(
                       BDLDFP_DECIMAL_DD(1.000000000000001));
            BDEC::Decimal128 d128(
                       BDLDFP_DECIMAL_DL(1.000000000000000000000000000000001));
            BDEC::Decimal128 EXP(
                       BDLDFP_DECIMAL_DL(1.000000000000000999999999999999999));
            LOOP_ASSERT(d64 / d128, EXP == d64 / d128);
        }
    }

    if (veryVerbose) bsl::cout << "Create test objects" << bsl::endl;

    BDEC::Decimal32        d32  = BDEC::Decimal32();
    BDEC::Decimal128       d128 = BDEC::Decimal128();
    const BDEC::Decimal128 c128 = BDEC::Decimal128();

    if (veryVerbose) bsl::cout << "Check return types" << bsl::endl;

    checkType<BDEC::Decimal128&>(++d128);
    checkType<BDEC::Decimal128>(d128++);
    checkType<BDEC::Decimal128&>(--d128);
    checkType<BDEC::Decimal128>(d128--);
    checkType<BDEC::Decimal128&>(d128 += static_cast<char>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<unsigned char>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<signed char>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<short>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<unsigned short>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<int>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<unsigned int>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<long>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<unsigned long>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<long long>(1));
    checkType<BDEC::Decimal128&>(d128 +=
                                 static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<char>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<signed char>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<short>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<int>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<long>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<long long>(1));
    checkType<BDEC::Decimal128&>(d128 -=
                                 static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<char>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<signed char>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<short>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<int>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<long>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<long long>(1));
    checkType<BDEC::Decimal128&>(d128 *=
                                 static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<char>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<signed char>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<short>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<int>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<long>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<long long>(1));
    checkType<BDEC::Decimal128&>(d128 /=
                                 static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<BDEC::Decimal128>(1));

    checkType<BDEC::Decimal128>(+d128);
    checkType<BDEC::Decimal128>(-d128);

    checkType<BDEC::Decimal128>(d128 + d128);
    checkType<BDEC::Decimal128>(d128 - d128);
    checkType<BDEC::Decimal128>(d128 * d128);
    checkType<BDEC::Decimal128>(d128 / d128);
    checkType<bool>(d128 == d128);
    checkType<bool>(d32 == d128);
    checkType<bool>(d128 == d32);
    checkType<bool>(d128 != d128);
    checkType<bool>(d32 != d128);
    checkType<bool>(d128 != d32);
    checkType<bool>(d128 < d128);
    checkType<bool>(d32 < d128);
    checkType<bool>(d128 < d32);
    checkType<bool>(d128 <= d128);
    checkType<bool>(d32 <= d128);
    checkType<bool>(d128 <= d32);
    checkType<bool>(d128 > d128);
    checkType<bool>(d32 > d128);
    checkType<bool>(d128 > d32);
    checkType<bool>(d128 >= d128);
    checkType<bool>(d32 >= d128);
    checkType<bool>(d128 >= d32);

    {
        bsl::istringstream  in(pa);
        bsl::wistringstream win(pa);
        bsl::ostringstream  out(pa);
        bsl::wostringstream wout(pa);

        checkType<bsl::istream&>(in >> d128);
        checkType<bsl::wistream&>(win >> d128);
        checkType<bsl::ostream&>(out << c128);
        checkType<bsl::wostream&>(wout << c128);
    }


    typedef bsl::numeric_limits<BDEC::Decimal128> d128_limits;
    checkType<bool>(d128_limits::is_specialized);
    checkType<BDEC::Decimal128>(d128_limits::min());
    checkType<BDEC::Decimal128>(d128_limits::max());
    checkType<int>(d128_limits::digits);
    checkType<int>(d128_limits::digits10);
    checkType<int>(d128_limits::max_digits10);
    checkType<bool>(d128_limits::is_signed);
    checkType<bool>(d128_limits::is_integer);
    checkType<bool>(d128_limits::is_exact);
    checkType<int>(d128_limits::radix);
    checkType<BDEC::Decimal128>(d128_limits::epsilon());
    checkType<BDEC::Decimal128>(d128_limits::round_error());
    checkType<int>(d128_limits::min_exponent);
    checkType<int>(d128_limits::min_exponent10);
    checkType<int>(d128_limits::max_exponent);
    checkType<int>(d128_limits::max_exponent10);
    checkType<bool>(d128_limits::has_infinity);
    checkType<bool>(d128_limits::has_quiet_NaN);
    checkType<bool>(d128_limits::has_signaling_NaN);
    checkType<bsl::float_denorm_style>(d128_limits::has_denorm);
    checkType<bool>(d128_limits::has_denorm_loss);
    checkType<BDEC::Decimal128>(d128_limits::infinity());
    checkType<BDEC::Decimal128>(d128_limits::quiet_NaN());
    checkType<BDEC::Decimal128>(d128_limits::signaling_NaN());
    checkType<BDEC::Decimal128>(d128_limits::denorm_min());
    checkType<bool>(d128_limits::is_iec559);
    checkType<bool>(d128_limits::is_bounded);
    checkType<bool>(d128_limits::is_modulo);
    checkType<bool>(d128_limits::traps);
    checkType<bool>(d128_limits::tinyness_before);
    checkType<bsl::float_round_style>(d128_limits::round_style);
}

void TestDriver::testCase2()
{
    // ------------------------------------------------------------------------
    // TESTING 'Decimal64'
    //   Ensure that 'Decimal64' functions correctly
    //
    // Concerns:
    //: 1 'Decimal64' operations are properly forwarded to implementation
    //: 2 All possible operand combinations work correctly.
    //
    // Plan:
    //: 1 Expected value testing will be used.
    //:
    //: 2 All forms of LHS and RHS combinations for all free operators will be
    //:   tested.
    //:
    //: 3 Every member operator will be tested
    //
    // Testing:
    //   'class Decimal64'
    // ------------------------------------------------------------------------

    if (verbose) bsl::cout << "\nTesting class Decimal64"
                           << "\n=======================" << bsl::endl;

    if (veryVerbose) bsl::cout << "Constructors" << bsl::endl;

    if (veryVeryVerbose) bsl::cout << "Copy/convert" << bsl::endl;
    {
        const BDEC::Decimal32  c32  = BDEC::Decimal32(32);
        const BDEC::Decimal64  c64  = BDEC::Decimal64(64);
        const BDEC::Decimal128 c128 = BDEC::Decimal128(128);

        LOOP2_ASSERT(BDLDFP_DECIMAL_DD( 32.0),   BDEC::Decimal64(c32),
                     BDLDFP_DECIMAL_DD( 32.0) == BDEC::Decimal64(c32));
        LOOP2_ASSERT(BDLDFP_DECIMAL_DD( 64.0),   BDEC::Decimal64(c64),
                     BDLDFP_DECIMAL_DD( 64.0) == BDEC::Decimal64(c64));
        LOOP2_ASSERT(BDLDFP_DECIMAL_DD(128.0),   BDEC::Decimal64(c128),
                     BDLDFP_DECIMAL_DD(128.0) == BDEC::Decimal64(c128));
    }

    if (veryVeryVerbose) bsl::cout << "Integral" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DD(  0.0) == BDEC::Decimal64());  // default
    ASSERT(BDLDFP_DECIMAL_DD(-42.0) == BDEC::Decimal64(-42)); // int
    ASSERT(BDLDFP_DECIMAL_DD( 42.0) == BDEC::Decimal64(42u)); // unsigned
    ASSERT(BDLDFP_DECIMAL_DD(-42.0) == BDEC::Decimal64(-42l)); // long
    ASSERT(BDLDFP_DECIMAL_DD( 42.0) == BDEC::Decimal64(42ul)); // ulong
    ASSERT(BDLDFP_DECIMAL_DD(-42.0) == BDEC::Decimal64(-42ll)); // longlong
    ASSERT(BDLDFP_DECIMAL_DD( 42.0) == BDEC::Decimal64(42ull)); // ulongl

    BDEC::Decimal64 cDefault;
    BDEC::Decimal64 cExpectedDefault = BDLDFP_DECIMAL_DD(0e-398);
    ASSERTV(cDefault, cExpectedDefault,
            0 == memcmp(&cDefault, &cExpectedDefault, sizeof(cDefault)));

    BDEC::Decimal64 cZero(0);
    BDEC::Decimal64 cExpectedZero = BDLDFP_DECIMAL_DD(0e0);
    ASSERTV(cZero, cExpectedZero,
            0 == memcmp(&cZero, &cExpectedZero, sizeof(cZero)));

    ASSERTV(cDefault, cZero, 0 != memcmp(&cDefault, &cZero, sizeof(cDefault)));

    if (veryVeryVerbose) bsl::cout << "Binary FP" << bsl::endl;

    // Note that to test binary-float taking constructors I use numbers
    // that can be represented exactly in both binary and decimal FP.

    ASSERT(BDLDFP_DECIMAL_DD(4.25) == BDEC::Decimal64(4.25f)); // float
    ASSERT(BDLDFP_DECIMAL_DD(4.25) == BDEC::Decimal64(4.25)); // double

    if (veryVeryVerbose) bsl::cout << "Decimal FP" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DD(-42.0) ==
           BDLDFP_DECIMAL_DF(-42.0));// Decimal342
    ASSERT(BDLDFP_DECIMAL_DD(42.0) == BDLDFP_DECIMAL_DF(42.0));
    ASSERT(BDLDFP_DECIMAL_DD(4.2) == BDLDFP_DECIMAL_DF(4.2));
    ASSERT(BDLDFP_DECIMAL_DD(4.2e9) == BDLDFP_DECIMAL_DF(4.2e9));

    ASSERT(BDLDFP_DECIMAL_DD(-42.0) == BDLDFP_DECIMAL_DL(-42.0));// Dec128
    ASSERT(BDLDFP_DECIMAL_DD(42.0) == BDLDFP_DECIMAL_DL(42.0));
    ASSERT(BDLDFP_DECIMAL_DD(4.2) == BDLDFP_DECIMAL_DL(4.2));
    ASSERT(BDLDFP_DECIMAL_DD(4.2e9) == BDLDFP_DECIMAL_DL(4.2e9));

    if (veryVerbose) bsl::cout << "Propriatery accessors" << bsl::endl;
    {
        BDEC::Decimal64 d64(42);
        ASSERT((void*)d64.data() == (void*)&d64);

        const BDEC::Decimal64 cd64(42);
        ASSERT((const void*)d64.data() == (const void*)&d64);

        ASSERT(BDEC::Decimal64(cd64.value()) == BDEC::Decimal64(42));
    }

    if (veryVerbose) bsl::cout << "Operator==" << bsl::endl;

    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) == BDLDFP_DECIMAL_DD(5.0)));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) == BDLDFP_DECIMAL_DD(-9.345e27));

    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) == BDLDFP_DECIMAL_DF(5.0)));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) == BDLDFP_DECIMAL_DF(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) == BDLDFP_DECIMAL_DD(5.0)));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) == BDLDFP_DECIMAL_DD(-9.345e27));

    if (veryVerbose) bsl::cout << "Operator!=" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DD(4.0) != BDLDFP_DECIMAL_DD(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DD(-9.345e27) !=
              BDLDFP_DECIMAL_DD(-9.345e27)));

    ASSERT(BDLDFP_DECIMAL_DD(4.0) != BDLDFP_DECIMAL_DF(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DD(-9.345e27) !=
              BDLDFP_DECIMAL_DF(-9.345e27)));
    ASSERT(BDLDFP_DECIMAL_DF(4.0) != BDLDFP_DECIMAL_DD(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DF(-9.345e27) !=
              BDLDFP_DECIMAL_DD(-9.345e27)));

    if (veryVerbose) bsl::cout << "Operator<" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DD(4.0) < BDLDFP_DECIMAL_DD(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DD(-9.345e27) <
              BDLDFP_DECIMAL_DD(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DD(5.0) < BDLDFP_DECIMAL_DD(4.0)));

    ASSERT(BDLDFP_DECIMAL_DD(4.0) < BDLDFP_DECIMAL_DF(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DD(-9.345e27) <
              BDLDFP_DECIMAL_DF(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DD(5.0) < BDLDFP_DECIMAL_DF(4.0)));
    ASSERT(BDLDFP_DECIMAL_DF(4.0) < BDLDFP_DECIMAL_DD(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DF(-9.345e27) <
              BDLDFP_DECIMAL_DD(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DF(5.0) < BDLDFP_DECIMAL_DD(4.0)));

    if (veryVerbose) bsl::cout << "Operator>" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DD(5.0) > BDLDFP_DECIMAL_DD(4.0));
    ASSERT(! (BDLDFP_DECIMAL_DD(-9.345e27) >
              BDLDFP_DECIMAL_DD(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) > BDLDFP_DECIMAL_DD(5.0)));

    ASSERT(BDLDFP_DECIMAL_DD(5.0) > BDLDFP_DECIMAL_DF(4.0));
    ASSERT(! (BDLDFP_DECIMAL_DD(-9.345e27) >
              BDLDFP_DECIMAL_DF(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) > BDLDFP_DECIMAL_DF(5.0)));
    ASSERT(BDLDFP_DECIMAL_DF(5.0) > BDLDFP_DECIMAL_DD(4.0));
    ASSERT(! (BDLDFP_DECIMAL_DF(-9.345e27) >
              BDLDFP_DECIMAL_DD(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) > BDLDFP_DECIMAL_DD(5.0)));

    if (veryVerbose) bsl::cout << "Operator<=" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DD(4.0) <= BDLDFP_DECIMAL_DD(5.0));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) <= BDLDFP_DECIMAL_DD(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DD(5.0) <= BDLDFP_DECIMAL_DD(4.0)));

    ASSERT(BDLDFP_DECIMAL_DD(4.0) <= BDLDFP_DECIMAL_DF(5.0));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) <= BDLDFP_DECIMAL_DF(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DD(5.0) <= BDLDFP_DECIMAL_DF(4.0)));
    ASSERT(BDLDFP_DECIMAL_DF(4.0) <= BDLDFP_DECIMAL_DD(5.0));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) <= BDLDFP_DECIMAL_DD(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DF(5.0) <= BDLDFP_DECIMAL_DD(4.0)));

    if (veryVerbose) bsl::cout << "Operator>=" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DD(5.0) >= BDLDFP_DECIMAL_DD(4.0));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) >= BDLDFP_DECIMAL_DD(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) >= BDLDFP_DECIMAL_DD(5.0)));

    ASSERT(BDLDFP_DECIMAL_DD(5.0) >= BDLDFP_DECIMAL_DF(4.0));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) >= BDLDFP_DECIMAL_DF(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) >= BDLDFP_DECIMAL_DF(5.0)));
    ASSERT(BDLDFP_DECIMAL_DF(5.0) >= BDLDFP_DECIMAL_DD(4.0));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) >= BDLDFP_DECIMAL_DD(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) >= BDLDFP_DECIMAL_DD(5.0)));

    // TODO TBD: Note that I am testing with the maximum number of
    // significant digits and I actually expect stream output to do no
    // rounding.  That is wrong (see later why), but necessary to
    // remote-test the 'format' function and make sure it does not lose
    // digits or does some unwanted rounding.  When 'format' will be in the
    // lower utility, it will be tested directly.
    //
    // Expecting to see all digits is wrong because that is not how the
    // stream output should behave: it should print with the default
    // precision as it would for binary floating point *or* with the
    // implied precision of the cohort if that is larger.  AFAIU

    if (veryVerbose) bsl::cout << "Test stream out" << bsl::endl;
    {
        bsl::ostringstream  out(pa);

        BDEC::Decimal64 d1(BDLDFP_DECIMAL_DD(-1.234567890123456e-24));
        out << bsl::setprecision(15) << d1;
        bsl::string s(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, decLower(s) == "-1.234567890123456e-24");
    }

    if (veryVerbose) bsl::cout << "Test stream in" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("-1.234567890123456e-24", pa);
        in.str(ins);

        BDEC::Decimal64 d1;
        in >> d1;
        ASSERT(d1 == BDLDFP_DECIMAL_DD(-1.234567890123456e-24));
    }

    {
        bsl::istringstream  in(pa);
        bsl::string ins("-123456789012345.6", pa);
        in.str(ins);

        BDEC::Decimal64 d1;
        in >> d1;
        ASSERT(d1 == BDLDFP_DECIMAL_DD(-123456789012345.6));
    }

    if (veryVerbose) bsl::cout << "Test stream in NaN" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("NaN", pa);
        in.str(ins);

        BDEC::Decimal64 d1;
        in >> d1;
        ASSERT(d1 != d1);
    }

    if (veryVerbose) bsl::cout << "Test stream in Infinity" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("Inf", pa);
        in.str(ins);

        BDEC::Decimal64 d1;
        in >> d1;
        ASSERT(d1 > bsl::numeric_limits<BDEC::Decimal64>::max());
    }

    if (veryVerbose) bsl::cout << "Test stream in -Infinity" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("-Inf", pa);
        in.str(ins);

        BDEC::Decimal64 d1;
        in >> d1;
        ASSERT(d1 < -bsl::numeric_limits<BDEC::Decimal64>::max());
    }

    if (veryVerbose) bsl::cout << "Test wide stream out" << bsl::endl;
    {
        bsl::wostringstream  out(pa);
        BDEC::Decimal64 d1(BDLDFP_DECIMAL_DD(-1.234567890123456e-24));
        out << bsl::setprecision(15) << d1;
        bsl::wstring s(pa);
        getStringFromStream(out, &s);
        ASSERT(decLower(s) == L"-1.234567890123456e-24");
    }

    if (veryVerbose) bsl::cout << "Test wide stream in" << bsl::endl;
    {
        bsl::wistringstream  in(pa);
        bsl::wstring ins(L"-1.234567890123456e-24", pa);
        in.str(ins);

        BDEC::Decimal64 d1;
        in >> d1;
        ASSERT(d1 == BDLDFP_DECIMAL_DD(-1.234567890123456e-24));
    }

    if (veryVerbose) bsl::cout << "Operator++" << bsl::endl;
    {
        BDEC::Decimal64 d1(BDLDFP_DECIMAL_DD(-5.0));
        BDEC::Decimal64 d2 = d1++;
        ASSERT(BDLDFP_DECIMAL_DD(-5.0) == d2);
        LOOP_ASSERT(d1, BDLDFP_DECIMAL_DD(-4.0) == d1);
        BDEC::Decimal64 d3 = ++d2;
        LOOP_ASSERT(d2, BDLDFP_DECIMAL_DD(-4.0) == d2);
        LOOP_ASSERT(d3, BDLDFP_DECIMAL_DD(-4.0) == d3);
    }

    if (veryVerbose) bsl::cout << "Operator--" << bsl::endl;
    {
        BDEC::Decimal64 d1(BDLDFP_DECIMAL_DD(-5.0));
        BDEC::Decimal64 d2 = d1--;
        ASSERT(BDLDFP_DECIMAL_DD(-5.0) == d2);
        LOOP_ASSERT(d1, BDLDFP_DECIMAL_DD(-6.0) == d1);
        BDEC::Decimal64 d3 = --d2;
        LOOP_ASSERT(d2, BDLDFP_DECIMAL_DD(-6.0) == d2);
        LOOP_ASSERT(d3, BDLDFP_DECIMAL_DD(-6.0) == d3);
    }

    if (veryVerbose) bsl::cout << "Unary-" << bsl::endl;
    {
        BDEC::Decimal64 d1(BDLDFP_DECIMAL_DD(-5.0));
        BDEC::Decimal64 d2(BDLDFP_DECIMAL_DD( 5.0));
        ASSERT(BDLDFP_DECIMAL_DD( 5.0) == -d1);
        ASSERT(BDLDFP_DECIMAL_DD(-5.0) == -d2);

        // Unary - must make +0 into -0

        bsl::ostringstream out(pa);
        out << -BDLDFP_DECIMAL_DD(0.0);
        bsl::string s(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, s[0] == '-'); // it is negative
        ASSERT(-BDLDFP_DECIMAL_DD(0.0) == BDEC::Decimal64(0)) // and 0

        BDEC::Decimal64  dd =  BDLDFP_DECIMAL_DD(0.0);
        BDEC::Decimal64 ndd = -BDLDFP_DECIMAL_DD(0.0);
        LOOP2_ASSERT( dd, ndd,  bsl::memcmp(&ndd, &dd, sizeof(dd)));
        dd= -dd;
        LOOP2_ASSERT(dd, ndd, !bsl::memcmp(&ndd, &dd, sizeof(dd)));
    }

    if (veryVerbose) bsl::cout << "Unary+" << bsl::endl;
    {
        BDEC::Decimal64 d1(BDLDFP_DECIMAL_DD(-5.0));
        BDEC::Decimal64 d2(BDLDFP_DECIMAL_DD( 5.0));
        ASSERT(BDLDFP_DECIMAL_DD(-5.0) == +d1);
        ASSERT(BDLDFP_DECIMAL_DD( 5.0) == +d2);

        // Unary + must make -0 into +0

        BDEC::Decimal128 negzero(-BDLDFP_DECIMAL_DD(0.0));
        bsl::ostringstream out(pa);
        out << +negzero;
        bsl::string s(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, s[0] != '+'); // it is positive
        ASSERT(-BDLDFP_DECIMAL_DD(0.0) == BDEC::Decimal64(0)) // and 0
    }

    if (veryVerbose) bsl::cout << "+=" << bsl::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) bsl::cout << "+=(int)" << bsl::endl;
        d += 1;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-4.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(unsigned int)" << bsl::endl;
        d += 2u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-2.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(long int)" << bsl::endl;
        d += -1l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-3.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(unsigned long int)"
                                       << bsl::endl;
        d += 5ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(2.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(long long int)" << bsl::endl;
        d += -2ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(0.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(unsigned long long int)"
                                       << bsl::endl;
        d += 42ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(42.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(Decimal32)" << bsl::endl;
        d += BDLDFP_DECIMAL_DF(-22.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(20.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(Decimal64)" << bsl::endl;
        d += BDLDFP_DECIMAL_DD(3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(23.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(Decimal128)" << bsl::endl;
        d += BDLDFP_DECIMAL_DL(9.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(32.0) == d);

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode addition" << bsl::endl;
        if (veryVeryVerbose) bsl::cout << "+=(Decimal64)" << bsl::endl;
        {
            BDEC::Decimal64 d64(     BDLDFP_DECIMAL_DD(0.9999999999999980));
            BDEC::Decimal64 rhs(     BDLDFP_DECIMAL_DD(1.000000000000001));
            BDEC::Decimal64 EXPECTED(BDLDFP_DECIMAL_DD(1.999999999999999));
            d64 += rhs;
            LOOP_ASSERT(d64, EXPECTED == d64);
        }
        if (veryVeryVerbose) bsl::cout << "+=(Decimal128)" << bsl::endl;
        {
           BDEC::Decimal64  d64(     BDLDFP_DECIMAL_DD(0.9999999999999985));
           BDEC::Decimal128 rhs(     BDLDFP_DECIMAL_DL(1.0000000000000001));
           BDEC::Decimal64  EXPECTED(BDLDFP_DECIMAL_DD(1.9999999999999990));
           d64 += rhs;
           LOOP_ASSERT(d64, EXPECTED == d64);
        }
        if (veryVeryVerbose) bsl::cout << "+=(long long)" << bsl::endl;
        {
           BDEC::Decimal64 d64(     BDLDFP_DECIMAL_DD( 9999999999999985.0));
           long long       rhs(                       10000000000000001ll);
           BDEC::Decimal64 EXPECTED(BDLDFP_DECIMAL_DD(19999999999999990.0));
           d64 += rhs;
           LOOP_ASSERT(d64, EXPECTED == d64);
        }
        if (veryVeryVerbose)
            bsl::cout << "+=(unsigned long long)" << bsl::endl;
        {
           BDEC::Decimal64 d64(     BDLDFP_DECIMAL_DD( 9999999999999985.0));
           long long       rhs(                       10000000000000001ull);
           BDEC::Decimal64 EXPECTED(BDLDFP_DECIMAL_DD(19999999999999990.0));
           d64 += rhs;
           LOOP_ASSERT(d64, EXPECTED == d64);
        }
    }

    if (veryVerbose) bsl::cout << "-=" << bsl::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) bsl::cout << "-=(int)" << bsl::endl;
        d -= 1;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-6.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(unsigned int)" << bsl::endl;
        d -= 2u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-8.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(long int)" << bsl::endl;
        d -= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(2.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(unsigned long int)"
                                       << bsl::endl;
        d -= 10ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-8.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(long long int)" << bsl::endl;
        d -= -8ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(0.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(unsigned long long int)"
                                       << bsl::endl;
        d -= 42ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-42.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(Decimal32)" << bsl::endl;
        d -= BDLDFP_DECIMAL_DF(-22.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-20.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(Decimal64)" << bsl::endl;
        d -= BDLDFP_DECIMAL_DD(3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-23.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(Decimal128)" << bsl::endl;
        d -= BDLDFP_DECIMAL_DL(9.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-32.0) == d);

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode subtraction"
                      << bsl::endl;
        if (veryVeryVerbose) bsl::cout << "-=(Decimal64)" << bsl::endl;
        {
            BDEC::Decimal64 d64(BDLDFP_DECIMAL_DD(    3.0              ));
            BDEC::Decimal64 rhs(BDLDFP_DECIMAL_DD(    1.000000000000001));
            d64 -= rhs;
            LOOP_ASSERT(d64, d64 == BDLDFP_DECIMAL_DD(1.999999999999999));
        }
        if (veryVeryVerbose) bsl::cout << "-=(Decimal128)" << bsl::endl;
        {
            BDEC::Decimal64  d64(BDLDFP_DECIMAL_DD(   2.0               ));
            BDEC::Decimal128 rhs(BDLDFP_DECIMAL_DL(   1.0000000000000001));
            d64 -= rhs;
            LOOP_ASSERT(d64, d64 == BDLDFP_DECIMAL_DD(0.9999999999999999));
        }
        if (veryVeryVerbose) bsl::cout << "-=(long long)" << bsl::endl;
        {
            BDEC::Decimal64 d64(BDLDFP_DECIMAL_DD(   20000000000000000.0));
            long long       rhs(                     10000000000000001ll    );
            d64 -= rhs;
            LOOP_ASSERT(d64, d64 == BDLDFP_DECIMAL_DD(9999999999999999.0));
        }
        if (veryVeryVerbose)
            bsl::cout << "-=(unsigned long long)" << bsl::endl;
        {
            BDEC::Decimal64 d64(BDLDFP_DECIMAL_DD(   20000000000000000.0));
            long long       rhs(                     10000000000000001ull   );
            d64 -= rhs;
            LOOP_ASSERT(d64, d64 == BDLDFP_DECIMAL_DD(9999999999999999.0));
        }
    }

    if (veryVerbose) bsl::cout << "*=" << bsl::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) bsl::cout << "*=(int)" << bsl::endl;
        d *= -2;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(10.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(unsigned int)" << bsl::endl;
        d *= 2000u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(20000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(long int)" << bsl::endl;
        d *= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-200000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(unsigned long int)"
                                       << bsl::endl;
        d *= 3ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-600000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(long long int)" << bsl::endl;
        d *= -1ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(600000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(unsigned long long int)"
                                       << bsl::endl;
        d *= 5ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(3000000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(Decimal32)" << bsl::endl;
        d *= BDLDFP_DECIMAL_DF(1e-5);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(30.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(Decimal64)" << bsl::endl;
        d *= BDLDFP_DECIMAL_DD(-3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-90.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(Decimal128)" << bsl::endl;
        d *= BDLDFP_DECIMAL_DL(2.4e-134);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-2.16e-132) == d);
        if (veryVeryVerbose)
            bsl::cout << "Test precision in mix-mode multiplication"
                      << bsl::endl;
        if (veryVeryVerbose) bsl::cout << "*=(Decimal64)" << bsl::endl;
        {
            BDEC::Decimal64 d64(     BDLDFP_DECIMAL_DD(9.0));
            BDEC::Decimal64 rhs(     BDLDFP_DECIMAL_DD(1.000000000000003));
            BDEC::Decimal64 EXPECTED(BDLDFP_DECIMAL_DD(9.000000000000027));
            d64 *= rhs;
            LOOP_ASSERT(d64, EXPECTED == d64);
        }
        if (veryVeryVerbose) bsl::cout << "*=(Decimal128)" << bsl::endl;
        {
            BDEC::Decimal64  d64(   BDLDFP_DECIMAL_DD(9.0));
            BDEC::Decimal128 rhs(   BDLDFP_DECIMAL_DL(1.0000000000000003));
            d64 *= rhs;
            LOOP_ASSERT(d64, d64 == BDLDFP_DECIMAL_DD(9.000000000000003));
        }
        if (veryVeryVerbose) bsl::cout << "*=(long long)" << bsl::endl;
        {
            BDEC::Decimal64 d64(BDLDFP_DECIMAL_DD(                    9.0));
            long long       rhs(                      10000000000000003ll);
            d64 *= rhs;
            LOOP_ASSERT(d64, d64 == BDLDFP_DECIMAL_DD(90000000000000030.0));
        }
        if (veryVeryVerbose)
            bsl::cout << "*=(unsigned long long)" << bsl::endl;
        {
            BDEC::Decimal64    d64(BDLDFP_DECIMAL_DD(                 9.0));
            unsigned long long rhs(                   10000000000000003ull);
            d64 *= rhs;
            LOOP_ASSERT(d64, d64 == BDLDFP_DECIMAL_DD(90000000000000030.0));
        }
    }

    if (veryVerbose) bsl::cout << "/=" << bsl::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) bsl::cout << "/=(int)" << bsl::endl;
        d /= -2;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(2.5) == d);

        if (veryVeryVerbose) bsl::cout << "/=(unsigned int)" << bsl::endl;
        d /= 2000u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(0.00125) == d);

        if (veryVeryVerbose) bsl::cout << "/=(long int)" << bsl::endl;
        d /= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-0.000125) == d);

        if (veryVeryVerbose) bsl::cout << "/=(unsigned long int)"
                                       << bsl::endl;
        d /= 5ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-0.000025) == d);

        if (veryVeryVerbose) bsl::cout << "/=(long long int)" << bsl::endl;
        d /= -5ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(0.000005) == d);

        if (veryVeryVerbose) bsl::cout << "/=(unsigned long long int)"
                                       << bsl::endl;
        d /= 5ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(0.000001) == d);

        if (veryVeryVerbose) bsl::cout << "/=(Decimal32)" << bsl::endl;
        d /= BDLDFP_DECIMAL_DF(1e-5);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(0.1) == d);

        if (veryVeryVerbose) bsl::cout << "/=(Decimal64)" << bsl::endl;
        d /= BDLDFP_DECIMAL_DD(-5.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-0.02) == d);

        if (veryVeryVerbose) bsl::cout << "/=(Decimal128)" << bsl::endl;
        d /= BDLDFP_DECIMAL_DL(-2.5e-134);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(8.e131) == d);

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mix-mode division" << bsl::endl;
        if (veryVeryVerbose) bsl::cout << "/=(Decimal64)" << bsl::endl;
        {
            BDEC::Decimal64 d64(     BDLDFP_DECIMAL_DD(2.0));
            BDEC::Decimal64 rhs(     BDLDFP_DECIMAL_DD(1.000000000000001));
            BDEC::Decimal64 EXPECTED(BDLDFP_DECIMAL_DD(1.999999999999998));
            d64 /= rhs;
            LOOP_ASSERT(d64, EXPECTED == d64);
        }
        if (veryVeryVerbose) bsl::cout << "/=(Decimal128)" << bsl::endl;
        {
            BDEC::Decimal64  d64(BDLDFP_DECIMAL_DD(1.0));
            BDEC::Decimal128 rhs(BDLDFP_DECIMAL_DL(1.0000000000000001));
            d64 /= rhs;
            LOOP_ASSERT(d64, BDLDFP_DECIMAL_DD(0.9999999999999999) == d64);
        }
        if (veryVeryVerbose) bsl::cout << "/=(long long)" << bsl::endl;
        {
            BDEC::Decimal64  d64(BDLDFP_DECIMAL_DD(10000000000000000.0));
            long long        rhs(                  10000000000000001ll);
            d64 /= rhs;
            LOOP_ASSERT(d64, BDLDFP_DECIMAL_DD(0.9999999999999999) == d64);
        }
        if (veryVeryVerbose)
            bsl::cout << "/=(unsigned long long)" << bsl::endl;
        {
            BDEC::Decimal64    d64(BDLDFP_DECIMAL_DD(10000000000000000.0));
            unsigned long long rhs(                  10000000000000001ull);
            d64 /= rhs;
            LOOP_ASSERT(d64, BDLDFP_DECIMAL_DD(0.9999999999999999) == d64);
        }
    }

    if (veryVerbose) bsl::cout << "operator+" << bsl::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) { T_ bsl::cout << "dec + int" << bsl::endl; }
        LOOP_ASSERT(d + 1, d + 1 == BDLDFP_DECIMAL_DD(-4.0));
        LOOP_ASSERT(1 + d, 1 + d == BDLDFP_DECIMAL_DD(-4.0));

        if (veryVeryVerbose) {
            T_ bsl::cout << "dec + unsigned int" << bsl::endl;
        }
        LOOP_ASSERT(d + 10u, d + 10u == BDLDFP_DECIMAL_DD(5.0));
        LOOP_ASSERT(10u + d, 10u + d == BDLDFP_DECIMAL_DD(5.0));

        if (veryVeryVerbose) { T_ bsl::cout << "dec + long int" << bsl::endl; }
        LOOP_ASSERT(d + 1l, d + 1l == BDLDFP_DECIMAL_DD(-4.0));
        LOOP_ASSERT(1l + d, 1l + d == BDLDFP_DECIMAL_DD(-4.0));

        if (veryVeryVerbose) {
            T_ bsl::cout << "dec + unsigned long int" << bsl::endl;
        }
        LOOP_ASSERT(d + 10ul, d + 10ul == BDLDFP_DECIMAL_DD(5.0));
        LOOP_ASSERT(10ul + d, 10ul + d == BDLDFP_DECIMAL_DD(5.0));

        if (veryVeryVerbose) {
            T_ bsl::cout << "dec + long long int" << bsl::endl;
        }
        LOOP_ASSERT(d + 1ll, d + 1ll == BDLDFP_DECIMAL_DD(-4.0));
        LOOP_ASSERT(1ll + d, 1ll + d == BDLDFP_DECIMAL_DD(-4.0));

        if (veryVeryVerbose) {
            T_ bsl::cout << "dec + unsigned long long int" << bsl::endl;
        }
        LOOP_ASSERT(d + 10ull, d + 10ull == BDLDFP_DECIMAL_DD(5.0));
        LOOP_ASSERT(10ull + d, 10ull + d == BDLDFP_DECIMAL_DD(5.0));

        if (veryVeryVerbose) { T_ bsl::cout << "dec + dec64" << bsl::endl; }
        LOOP_ASSERT(d + BDLDFP_DECIMAL_DD(-3.0),
                    d + BDLDFP_DECIMAL_DD(-3.0) ==
                    BDLDFP_DECIMAL_DD(-8.0));

        if (veryVeryVerbose) { T_ bsl::cout << "dec + dec32" << bsl::endl; }
        LOOP_ASSERT(d + BDLDFP_DECIMAL_DF(-3.0),
                    d + BDLDFP_DECIMAL_DF(-3.0) ==
                    BDLDFP_DECIMAL_DD(-8.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-3.0) + d,
                    BDLDFP_DECIMAL_DF(-3.0) + d ==
                    BDLDFP_DECIMAL_DD(-8.0));
        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode addition" << bsl::endl;
        if (veryVeryVerbose) { T_ bsl::cout << "dec + dec32" << bsl::endl; }
        {
            BDEC::Decimal32 lhs(     BDLDFP_DECIMAL_DF(0.9999999));
            BDEC::Decimal64 rhs(     BDLDFP_DECIMAL_DD(1.000000000000001));
            BDEC::Decimal64 EXPECTED(BDLDFP_DECIMAL_DD(1.999999900000001));
            LOOP_ASSERT(lhs + rhs, EXPECTED == lhs + rhs);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec32 + dec" << bsl::endl; }
        {
            BDEC::Decimal64 lhs(     BDLDFP_DECIMAL_DD(1.000000000000001));
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(0.9999999));
            BDEC::Decimal64 EXPECTED(BDLDFP_DECIMAL_DD(1.999999900000001));
            LOOP_ASSERT(lhs + rhs, EXPECTED == lhs + rhs);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec + dec64" << bsl::endl; }
        {
            BDEC::Decimal64 lhs(     BDLDFP_DECIMAL_DD(0.9999999999999980));
            BDEC::Decimal64 rhs(     BDLDFP_DECIMAL_DD(1.000000000000001));
            BDEC::Decimal64 EXPECTED(BDLDFP_DECIMAL_DD(1.999999999999999));
            LOOP_ASSERT(lhs + rhs, EXPECTED == lhs + rhs);
        }
        if (veryVeryVerbose) {
            T_ bsl::cout << "dec + long long" << bsl::endl;
        }
        {
            BDEC::Decimal64  lhs(    BDLDFP_DECIMAL_DD( 9999999999999985.0));
            long long        rhs(                      10000000000000001ll  );
            LOOP_ASSERT(lhs + rhs,
                        lhs + rhs == BDLDFP_DECIMAL_DD(19999999999999990.0));
        }
        if (veryVeryVerbose) {
            T_ bsl::cout << "dec + unsigned long long" << bsl::endl;
        }
        {
            BDEC::Decimal64     lhs( BDLDFP_DECIMAL_DD( 9999999999999985.0));
            unsigned long long  rhs(                   10000000000000001ull );
            LOOP_ASSERT(lhs + rhs,
                        lhs + rhs == BDLDFP_DECIMAL_DD(19999999999999990.0));
        }
        if (veryVeryVerbose) {
            T_ bsl::cout << "long long + dec" << bsl::endl;
        }
        {
            long long        lhs(                      10000000000000001ll  );
            BDEC::Decimal64  rhs(    BDLDFP_DECIMAL_DD( 9999999999999985.0));
            LOOP_ASSERT(lhs + rhs,
                        lhs + rhs == BDLDFP_DECIMAL_DD(19999999999999990.0));
        }
        if (veryVeryVerbose) {
            T_ bsl::cout << "unsigned long long + dec" << bsl::endl;
        }
        {
            unsigned long long  lhs(                   10000000000000001ull);
            BDEC::Decimal64     rhs( BDLDFP_DECIMAL_DD( 9999999999999985.0));
            LOOP_ASSERT(lhs + rhs,
                        lhs + rhs == BDLDFP_DECIMAL_DD(19999999999999990.0));
        }
    }

    if (veryVerbose) bsl::cout << "operator-" << bsl::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) bsl::cout << "dec - int" << bsl::endl;
        LOOP_ASSERT(d - 1, d - 1 == BDLDFP_DECIMAL_DD(-6.0));
        LOOP_ASSERT(1 - d, 1 - d == BDLDFP_DECIMAL_DD( 6.0));

        if (veryVeryVerbose) bsl::cout << "dec - unsigned int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 10u, d - 10u == BDLDFP_DECIMAL_DD(-15.0));
        LOOP_ASSERT(10u - d, 10u - d == BDLDFP_DECIMAL_DD( 15.0));

        if (veryVeryVerbose) bsl::cout << "dec - long int" << bsl::endl;
        LOOP_ASSERT(d - 1l, d - 1l == BDLDFP_DECIMAL_DD(-6.0));
        LOOP_ASSERT(1l - d, 1l - d == BDLDFP_DECIMAL_DD( 6.0));

        if (veryVeryVerbose) bsl::cout << "dec - unsigned long int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 10ul, d - 10ul == BDLDFP_DECIMAL_DD(-15.0));
        LOOP_ASSERT(10ul - d, 10ul - d == BDLDFP_DECIMAL_DD( 15.0));

        if (veryVeryVerbose) bsl::cout << "dec - long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 1ll, d - 1ll == BDLDFP_DECIMAL_DD(-6.0));
        LOOP_ASSERT(1ll - d, 1ll - d == BDLDFP_DECIMAL_DD( 6.0));

        if (veryVeryVerbose) bsl::cout << "dec - unsigned long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 10ull, d - 10ull == BDLDFP_DECIMAL_DD(-15.0));
        LOOP_ASSERT(10ull - d, 10ull - d == BDLDFP_DECIMAL_DD( 15.0));

        if (veryVeryVerbose) bsl::cout << "dec - dec64" << bsl::endl;
        LOOP_ASSERT(d - BDLDFP_DECIMAL_DD(-3.0),
                    d - BDLDFP_DECIMAL_DD(-3.0) ==
                    BDLDFP_DECIMAL_DD(-2.0));

        if (veryVeryVerbose) bsl::cout << "dec - dec32" << bsl::endl;
        LOOP_ASSERT(d - BDLDFP_DECIMAL_DF(-3.0),
                    d - BDLDFP_DECIMAL_DF(-3.0) ==
                    BDLDFP_DECIMAL_DD(-2.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-3.0) - d,
                    BDLDFP_DECIMAL_DF(-3.0) - d ==
                    BDLDFP_DECIMAL_DD( 2.0));

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode subtraction"
                      << bsl::endl;
        if (veryVeryVerbose) { T_ bsl::cout << "dec - dec32" << bsl::endl; }
        {
            BDEC::Decimal64 lhs(BDLDFP_DECIMAL_DD(3.000000000000001));
            BDEC::Decimal32 rhs(BDLDFP_DECIMAL_DF(1.000001));
            LOOP_ASSERT(lhs - rhs,
                        lhs - rhs == BDLDFP_DECIMAL_DD(1.999999000000001));
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec32 - dec" << bsl::endl; }
        {
            BDEC::Decimal32 lhs(BDLDFP_DECIMAL_DF(3.000001));
            BDEC::Decimal64 rhs(BDLDFP_DECIMAL_DD(1.000000000000001));
            LOOP_ASSERT(lhs - rhs,
                        lhs - rhs == BDLDFP_DECIMAL_DD(2.000000999999999));
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec - dec64" << bsl::endl; }
        {
            BDEC::Decimal64 lhs(BDLDFP_DECIMAL_DD(3.0));
            BDEC::Decimal64 rhs(BDLDFP_DECIMAL_DD(1.000000000000001));
            LOOP_ASSERT(lhs - rhs,
                        lhs - rhs == BDLDFP_DECIMAL_DD(1.999999999999999));
        }
        if (veryVeryVerbose) {
            T_ bsl::cout << "dec - long long" << bsl::endl;
        }
        {
            BDEC::Decimal64  lhs(BDLDFP_DECIMAL_DD(   20000000000000000.0));
            long long        rhs(                     10000000000000001ll);
            LOOP_ASSERT(lhs - rhs,
                        lhs - rhs == BDLDFP_DECIMAL_DD(9999999999999999.0));
        }
        if (veryVeryVerbose) {
            T_ bsl::cout << "dec - unsigned long long" << bsl::endl;
        }
        {
            BDEC::Decimal64    lhs(BDLDFP_DECIMAL_DD( 20000000000000000.0));
            unsigned long long rhs(                   10000000000000001ull);
            LOOP_ASSERT(lhs - rhs,
                        lhs - rhs == BDLDFP_DECIMAL_DD(9999999999999999.0));
        }
        if (veryVeryVerbose) {
            T_ bsl::cout << "long long - dec" << bsl::endl;
        }
        {
            long long        lhs(                  10000000000000001ll);
            BDEC::Decimal64  rhs(BDLDFP_DECIMAL_DD(20000000000000000.0));
            BDEC::Decimal64  EXP(BDLDFP_DECIMAL_DD(-9999999999999999.0));
            LOOP_ASSERT(lhs - rhs, lhs - rhs == EXP);

        }
        if (veryVeryVerbose) {
            T_ bsl::cout << "unsigned long long - dec" << bsl::endl;
        }
        {
            unsigned long long lhs(                  10000000000000001ull);
            BDEC::Decimal64    rhs(BDLDFP_DECIMAL_DD(20000000000000000.0));
            BDEC::Decimal64    EXP(BDLDFP_DECIMAL_DD(-9999999999999999.0));
            LOOP_ASSERT(lhs - rhs, lhs - rhs == EXP);
        }
    }

    if (veryVerbose) bsl::cout << "operator*" << bsl::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) { T_ bsl::cout << "dec * int" << bsl::endl; }
        LOOP_ASSERT(d * -2, d * -2 == BDLDFP_DECIMAL_DD(10.0));
        LOOP_ASSERT(-2 * d, -2 * d == BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose) { T_ bsl::cout << "dec * unsigned int"
                                            << bsl::endl; }
        LOOP_ASSERT(d * 10u, d * 10u == BDLDFP_DECIMAL_DD(-50.0));
        LOOP_ASSERT(10u * d, 10u * d == BDLDFP_DECIMAL_DD(-50.0));

        if (veryVeryVerbose) { T_ bsl::cout << "dec * long int" << bsl::endl; }
        LOOP_ASSERT(d * -2l, d * -2l == BDLDFP_DECIMAL_DD(10.0));
        LOOP_ASSERT(-2l * d, -2l * d == BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose) { T_ bsl::cout << "dec * unsigned long int"
                                            << bsl::endl; }
        LOOP_ASSERT(d * 10ul, d * 10ul == BDLDFP_DECIMAL_DD(-50.0));
        LOOP_ASSERT(10ul * d, 10ul * d == BDLDFP_DECIMAL_DD(-50.0));

        if (veryVeryVerbose) { T_ bsl::cout << "dec * long long int"
                                            << bsl::endl; }
        LOOP_ASSERT(d * -2ll, d * -2ll == BDLDFP_DECIMAL_DD(10.0));
        LOOP_ASSERT(-2ll * d, -2ll * d == BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose) { T_ bsl::cout << "dec * unsigned long long int"
                                       << bsl::endl; }
        LOOP_ASSERT(d * 10ull, d * 10ull == BDLDFP_DECIMAL_DD(-50.0));
        LOOP_ASSERT(10ull * d, 10ull * d == BDLDFP_DECIMAL_DD(-50.0));

        if (veryVeryVerbose) { T_ bsl::cout << "dec * dec64" << bsl::endl; }
        LOOP_ASSERT(d * BDLDFP_DECIMAL_DD(-3.0),
                    d * BDLDFP_DECIMAL_DD(-3.0) ==
                    BDLDFP_DECIMAL_DD(15.0));

        if (veryVeryVerbose) { T_ bsl::cout << "dec * dec32" << bsl::endl; }
        LOOP_ASSERT(d * BDLDFP_DECIMAL_DF(-3.0),
                    d * BDLDFP_DECIMAL_DF(-3.0) ==
                    BDLDFP_DECIMAL_DD(15.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-3.0) * d,
                    BDLDFP_DECIMAL_DF(-3.0) * d ==
                    BDLDFP_DECIMAL_DD(15.0));
        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode multiplication"
                      << bsl::endl;
        if (veryVeryVerbose) { T_ bsl::cout << "dec * dec32" << bsl::endl; }
        {
            BDEC::Decimal64 lhs(     BDLDFP_DECIMAL_DD(3.000000000000001));
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(3.0));
            BDEC::Decimal64 EXPECTED(BDLDFP_DECIMAL_DD(9.000000000000003));
            LOOP_ASSERT(lhs * rhs, EXPECTED == lhs * rhs);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec32 * dec" << bsl::endl; }
        {
            BDEC::Decimal32 lhs(     BDLDFP_DECIMAL_DF(3.0));
            BDEC::Decimal64 rhs(     BDLDFP_DECIMAL_DD(3.000000000000001));
            BDEC::Decimal64 EXPECTED(BDLDFP_DECIMAL_DD(9.000000000000003));
            LOOP_ASSERT(lhs * rhs, EXPECTED == lhs * rhs);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec * dec64" << bsl::endl; }
        {
            BDEC::Decimal64 lhs(     BDLDFP_DECIMAL_DD(9.0));
            BDEC::Decimal64 rhs(     BDLDFP_DECIMAL_DD(1.000000000000003));
            BDEC::Decimal64 EXPECTED(BDLDFP_DECIMAL_DD(9.000000000000027));
            LOOP_ASSERT(lhs * rhs, EXPECTED == lhs * rhs);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec * long long"
                                            << bsl::endl; }
        {
            BDEC::Decimal64  lhs(BDLDFP_DECIMAL_DD(                9.0));
            long long        rhs(                  10000000000000003ll);
            BDEC::Decimal64  EXP(BDLDFP_DECIMAL_DD(90000000000000030.0));
            LOOP_ASSERT(lhs * rhs, EXP == lhs * rhs);
        }
        if (veryVeryVerbose)
            { T_ bsl::cout << "dec * unsigned long long" << bsl::endl; }
        {
            BDEC::Decimal64    lhs(BDLDFP_DECIMAL_DD(                9.0));
            unsigned long long rhs(                  10000000000000003ull);
            BDEC::Decimal64    EXP(BDLDFP_DECIMAL_DD(90000000000000030.0));
            LOOP_ASSERT(lhs * rhs, EXP == lhs * rhs);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "long long * dec"
                                            << bsl::endl; }
        {
            long long        lhs(                  10000000000000003ll);
            BDEC::Decimal64  rhs(BDLDFP_DECIMAL_DD(                9.0));
            BDEC::Decimal64  EXP(BDLDFP_DECIMAL_DD(90000000000000030.0));
            LOOP_ASSERT(lhs * rhs, EXP == lhs * rhs);
        }
        if (veryVeryVerbose)
            { T_ bsl::cout << "unsigned long long * dec" << bsl::endl; }
        {
            unsigned long long lhs(                  10000000000000003ull);
            BDEC::Decimal64    rhs(BDLDFP_DECIMAL_DD(                9.0));
            BDEC::Decimal64    EXP(BDLDFP_DECIMAL_DD(90000000000000030.0));
            LOOP_ASSERT(lhs * rhs, EXP == lhs * rhs);
        }
    }

    if (veryVerbose) bsl::cout << "operator/" << bsl::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) { T_ bsl::cout << "dec / int" << bsl::endl; }
        LOOP_ASSERT(d / -2, d / -2 == BDLDFP_DECIMAL_DD(2.5));
        LOOP_ASSERT(-2 / d, -2 / d == BDLDFP_DECIMAL_DD(0.4));

        if (veryVeryVerbose) { T_ bsl::cout << "dec / unsigned int"
                                       << bsl::endl; }
        LOOP_ASSERT(d / 100u, d / 100u == BDLDFP_DECIMAL_DD( -0.05));
        LOOP_ASSERT(100u / d, 100u / d == BDLDFP_DECIMAL_DD(-20.0));

        if (veryVeryVerbose) { T_ bsl::cout << "dec / long int" << bsl::endl; }
        LOOP_ASSERT(d / -2l, d / -2l == BDLDFP_DECIMAL_DD(2.5));
        LOOP_ASSERT(-2l / d, -2l / d == BDLDFP_DECIMAL_DD(0.4));

        if (veryVeryVerbose) { T_ bsl::cout << "dec / unsigned long int"
                                       << bsl::endl; }
        LOOP_ASSERT(d / 100ul, d / 100ul == BDLDFP_DECIMAL_DD( -0.05));
        LOOP_ASSERT(100ul / d, 100ul / d == BDLDFP_DECIMAL_DD(-20.0));

        if (veryVeryVerbose) { T_ bsl::cout << "dec / long long int"
                                       << bsl::endl; }
        LOOP_ASSERT(d / -2ll, d / -2ll == BDLDFP_DECIMAL_DD(2.5));
        LOOP_ASSERT(-2ll / d, -2ll / d == BDLDFP_DECIMAL_DD(0.4));

        if (veryVeryVerbose) { T_ bsl::cout << "dec / unsigned long long int"
                                       << bsl::endl; }
        LOOP_ASSERT(d / 100ull, d / 100ull == BDLDFP_DECIMAL_DD( -0.05));
        LOOP_ASSERT(100ull / d, 100ull / d == BDLDFP_DECIMAL_DD(-20.0));

        if (veryVeryVerbose) { T_ bsl::cout << "dec / dec64" << bsl::endl; }
        LOOP_ASSERT(d / BDLDFP_DECIMAL_DD(-50.0),
                    d / BDLDFP_DECIMAL_DD(-50.0) ==
                    BDLDFP_DECIMAL_DD(0.1));

        if (veryVeryVerbose) { T_ bsl::cout << "dec / dec32" << bsl::endl; }
        LOOP_ASSERT(d / BDLDFP_DECIMAL_DF(-50.0),
                    d / BDLDFP_DECIMAL_DF(-50.0) ==
                    BDLDFP_DECIMAL_DD(0.1));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-50.0) / d,
                    BDLDFP_DECIMAL_DF(-50.0) / d ==
                    BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode division"
                      << bsl::endl;
        if (veryVeryVerbose) { T_ bsl::cout << "dec / dec32" << bsl::endl; }
        {
            BDEC::Decimal64 lhs(BDLDFP_DECIMAL_DD(2.000000000000002));
            BDEC::Decimal32 rhs(BDLDFP_DECIMAL_DF(2.0));
            BDEC::Decimal64 EXP(BDLDFP_DECIMAL_DD(1.000000000000001));
            LOOP_ASSERT(lhs / rhs, EXP == lhs / rhs);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec32 / dec" << bsl::endl; }
        {
            BDEC::Decimal32 lhs(BDLDFP_DECIMAL_DF(1.0));
            BDEC::Decimal64 rhs(BDLDFP_DECIMAL_DD(1.000000000000001));
            BDEC::Decimal64 EXP(BDLDFP_DECIMAL_DD(0.999999999999999));
            LOOP_ASSERT(lhs / rhs, EXP == lhs / rhs);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec / dec64" << bsl::endl; }
        {
            BDEC::Decimal64 lhs(BDLDFP_DECIMAL_DD(2.0));
            BDEC::Decimal64 rhs(BDLDFP_DECIMAL_DD(1.000000000000001));
            BDEC::Decimal64 EXP(BDLDFP_DECIMAL_DD(1.999999999999998));
            LOOP_ASSERT(lhs / rhs, EXP == lhs / rhs);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "dec / long long"
                                            << bsl::endl; }
        {
            BDEC::Decimal64 lhs(BDLDFP_DECIMAL_DD(10000000000000000.0));
            long long       rhs(                  10000000000000001ll);
            BDEC::Decimal64 EXP(BDLDFP_DECIMAL_DD(0.9999999999999999));
            LOOP_ASSERT(lhs / rhs, EXP == lhs / rhs);
        }
        if (veryVeryVerbose)
            { T_ bsl::cout << "dec / unsigned long long" << bsl::endl; }
        {
            BDEC::Decimal64    lhs(BDLDFP_DECIMAL_DD(10000000000000000.0));
            unsigned long long rhs(                  10000000000000001ull);
            BDEC::Decimal64    EXP(BDLDFP_DECIMAL_DD(0.9999999999999999));
            LOOP_ASSERT(lhs / rhs, EXP == lhs / rhs);
        }
        if (veryVeryVerbose) { T_ bsl::cout << "long long / dec"
                                            << bsl::endl; }
        {
            long long       lhs(                  20000000000000011ll);
            BDEC::Decimal64 rhs(BDLDFP_DECIMAL_DD(                2.0));
            BDEC::Decimal64 EXP(BDLDFP_DECIMAL_DD(10000000000000010.0));
            LOOP_ASSERT(lhs / rhs, EXP == lhs / rhs);
        }
        if (veryVeryVerbose)
            { T_ bsl::cout << "unsigned long long / dec" << bsl::endl; }
        {
            unsigned long long lhs(                  20000000000000011ull);
            BDEC::Decimal64    rhs(BDLDFP_DECIMAL_DD(                2.0));
            BDEC::Decimal64    EXP(BDLDFP_DECIMAL_DD(10000000000000010.0));
            LOOP_ASSERT(lhs / rhs, EXP == lhs / rhs);
        }
    }

    if (veryVerbose) bsl::cout << "Create test objects" << bsl::endl;

    BDEC::Decimal32        d32  = BDEC::Decimal32();
    BDEC::Decimal64        d64  = BDEC::Decimal64();
    const BDEC::Decimal64  c64  = BDEC::Decimal64();

    if (veryVerbose) bsl::cout << "Check return types" << bsl::endl;

    checkType<BDEC::Decimal64&>(++d64);
    checkType<BDEC::Decimal64>(d64++);
    checkType<BDEC::Decimal64&>(--d64);
    checkType<BDEC::Decimal64>(d64--);
    checkType<BDEC::Decimal64&>(d64 += static_cast<char>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<unsigned char>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<signed char>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<short>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<unsigned short>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<int>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<unsigned int>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<long>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<unsigned long>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<long long>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<char>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<signed char>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<short>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<int>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<long>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<long long>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<char>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<signed char>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<short>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<int>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<long>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<long long>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<char>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<signed char>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<short>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<int>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<long>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<long long>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<BDEC::Decimal128>(1));

    checkType<BDEC::Decimal64>(+d64);
    checkType<BDEC::Decimal64>(-d64);

    checkType<BDEC::Decimal64>(d64 + d64);
    checkType<BDEC::Decimal64>(d64 - d64);
    checkType<BDEC::Decimal64>(d64 * d64);
    checkType<BDEC::Decimal64>(d64 / d64);
    checkType<bool>(d64 == d64);
    checkType<bool>(d32 == d64);
    checkType<bool>(d64 == d32);
    checkType<bool>(d64 != d64);
    checkType<bool>(d32 != d64);
    checkType<bool>(d64 != d32);
    checkType<bool>(d64 < d64);
    checkType<bool>(d32 < d64);
    checkType<bool>(d64 < d32);
    checkType<bool>(d64 <= d64);
    checkType<bool>(d32 <= d64);
    checkType<bool>(d64 <= d32);
    checkType<bool>(d64 > d64);
    checkType<bool>(d32 > d64);
    checkType<bool>(d64 > d32);
    checkType<bool>(d64 >= d64);
    checkType<bool>(d32 >= d64);
    checkType<bool>(d64 >= d32);

    {
        bsl::istringstream  in(pa);
        bsl::wistringstream win(pa);
        bsl::ostringstream  out(pa);
        bsl::wostringstream wout(pa);

        checkType<bsl::istream&>(in >> d64);
        checkType<bsl::wistream&>(win >> d64);
        checkType<bsl::ostream&>(out << c64);
        checkType<bsl::wostream&>(wout << c64);
    }

    typedef bsl::numeric_limits<BDEC::Decimal64> d64_limits;
    checkType<bool>(d64_limits::is_specialized);
    checkType<BDEC::Decimal64>(d64_limits::min());
    checkType<BDEC::Decimal64>(d64_limits::max());
    checkType<int>(d64_limits::digits);
    checkType<int>(d64_limits::digits10);
    checkType<int>(d64_limits::max_digits10);
    checkType<bool>(d64_limits::is_signed);
    checkType<bool>(d64_limits::is_integer);
    checkType<bool>(d64_limits::is_exact);
    checkType<int>(d64_limits::radix);
    checkType<BDEC::Decimal64>(d64_limits::epsilon());
    checkType<BDEC::Decimal64>(d64_limits::round_error());
    checkType<int>(d64_limits::min_exponent);
    checkType<int>(d64_limits::min_exponent10);
    checkType<int>(d64_limits::max_exponent);
    checkType<int>(d64_limits::max_exponent10);
    checkType<bool>(d64_limits::has_infinity);
    checkType<bool>(d64_limits::has_quiet_NaN);
    checkType<bool>(d64_limits::has_signaling_NaN);
    checkType<bsl::float_denorm_style>(d64_limits::has_denorm);
    checkType<bool>(d64_limits::has_denorm_loss);
    checkType<BDEC::Decimal64>(d64_limits::infinity());
    checkType<BDEC::Decimal64>(d64_limits::quiet_NaN());
    checkType<BDEC::Decimal64>(d64_limits::signaling_NaN());
    checkType<BDEC::Decimal64>(d64_limits::denorm_min());
    checkType<bool>(d64_limits::is_iec559);
    checkType<bool>(d64_limits::is_bounded);
    checkType<bool>(d64_limits::is_modulo);
    checkType<bool>(d64_limits::traps);
    checkType<bool>(d64_limits::tinyness_before);
    checkType<bsl::float_round_style>(d64_limits::round_style);
}

void TestDriver::testCase1()
{
    // ------------------------------------------------------------------------
    // TESTING 'Decimal32'
    //   Ensure that 'Decimal32' functions correctly
    //
    // Concerns:
    //: 1 'Decimal32' operations are properly forwarded to implementation
    //:
    //: 2 All possible operand combinations work correctly.
    //:
    //: 3 That the result of arithmetic operation fits Decimal32 precision when
    //:   operands exceed it to ensure that operation is performed in higher
    //:   precision.
    //
    // Plan:
    //: 1 Expected value testing will be used.
    //:
    //: 2 All forms of LHS and RHS combinations for all free operators will be
    //:   tested.
    //:
    //: 3 Every member operator will be tested
    //
    // Testing:
    //   'class Decimal32'
    // ------------------------------------------------------------------------
    if (verbose) bsl::cout << bsl::endl
                           << "Testing class Decimal32" << bsl::endl
                           << "=======================" << bsl::endl;

    if (veryVerbose) bsl::cout << "Constructors" << bsl::endl;

    if (veryVeryVerbose) bsl::cout << "Copy/convert" << bsl::endl;
    {
        const BDEC::Decimal32  c32  = BDEC::Decimal32(  32);
        const BDEC::Decimal64  c64  = BDEC::Decimal64(  64);
        const BDEC::Decimal128 c128 = BDEC::Decimal128(128);

        LOOP2_ASSERT(BDLDFP_DECIMAL_DF( 32.0),   BDEC::Decimal32( c32),
                     BDLDFP_DECIMAL_DF( 32.0) == BDEC::Decimal32( c32));
        LOOP2_ASSERT(BDLDFP_DECIMAL_DF( 64.0),   BDEC::Decimal32( c64),
                     BDLDFP_DECIMAL_DF( 64.0) == BDEC::Decimal32( c64));
        LOOP2_ASSERT(BDLDFP_DECIMAL_DF(128.0),   BDEC::Decimal32(c128),
                     BDLDFP_DECIMAL_DF(128.0) == BDEC::Decimal32(c128));
    }

    if (veryVeryVerbose) bsl::cout << "Integral" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DF(  0.0) == BDEC::Decimal32());  // default
    ASSERT(BDLDFP_DECIMAL_DF(-42.0) == BDEC::Decimal32(-42)); // int
    ASSERT(BDLDFP_DECIMAL_DF( 42.0) == BDEC::Decimal32(42u)); // unsigned
    ASSERT(BDLDFP_DECIMAL_DF(-42.0) == BDEC::Decimal32(-42l)); // long
    ASSERT(BDLDFP_DECIMAL_DF( 42.0) == BDEC::Decimal32(42ul)); // ulong
    ASSERT(BDLDFP_DECIMAL_DF(-42.0) == BDEC::Decimal32(-42ll)); // longlong
    ASSERT(BDLDFP_DECIMAL_DF( 42.0) == BDEC::Decimal32(42ull)); // ulongl

    BDEC::Decimal32 cDefault;
    BDEC::Decimal32 cExpectedDefault = BDLDFP_DECIMAL_DF(0e-101);
    ASSERTV(cDefault, cExpectedDefault,
            0 == memcmp(&cDefault, &cExpectedDefault, sizeof(cDefault)));

    BDEC::Decimal32 cZero(0);
    BDEC::Decimal32 cExpectedZero = BDLDFP_DECIMAL_DF(0e0);
    ASSERTV(cZero, cExpectedZero,
            0 == memcmp(&cZero, &cExpectedZero, sizeof(cZero)));

    ASSERTV(cDefault, cZero, 0 != memcmp(&cDefault, &cZero, sizeof(cDefault)));

    if (veryVeryVerbose) bsl::cout << "Binary FP" << bsl::endl;

    // Note that to test binary-float taking constructors I use numbers
    // that can be represented exactly in both binary and decimal FP.

    ASSERT(BDLDFP_DECIMAL_DF(4.25) == BDEC::Decimal32(4.25f)); // float
    ASSERT(BDLDFP_DECIMAL_DF(4.25) == BDEC::Decimal32(4.25)); // double

    if (veryVeryVerbose) bsl::cout << "Decimal FP" << bsl::endl;

    // Decimal64
    ASSERT(BDLDFP_DECIMAL_DF(-42.0) == BDLDFP_DECIMAL_DD(-42.0));
    ASSERT(BDLDFP_DECIMAL_DF(42.0) == BDLDFP_DECIMAL_DD(42.0));
    ASSERT(BDLDFP_DECIMAL_DF(4.2) == BDLDFP_DECIMAL_DD(4.2));
    ASSERT(BDLDFP_DECIMAL_DF(4.2e9) == BDLDFP_DECIMAL_DD(4.2e9));

    // Dec128
    ASSERT(BDLDFP_DECIMAL_DF(-42.0) == BDLDFP_DECIMAL_DL(-42.0));
    ASSERT(BDLDFP_DECIMAL_DF(42.0) == BDLDFP_DECIMAL_DL(42.0));
    ASSERT(BDLDFP_DECIMAL_DF(4.2) == BDLDFP_DECIMAL_DL(4.2));
    ASSERT(BDLDFP_DECIMAL_DF(4.2e9) == BDLDFP_DECIMAL_DL(4.2e9));

    if (veryVerbose) bsl::cout << "Propriatery accessors" << bsl::endl;
    {
        BDEC::Decimal32 d32(42);
        ASSERT((void*)d32.data() == (void*)&d32);

        const BDEC::Decimal32 cd32(42);
        ASSERT((const void*)d32.data() == (const void*)&d32);

        ASSERT(BDEC::Decimal32(cd32.value()) == BDEC::Decimal32(42));
    }

    if (veryVerbose) bsl::cout << "Operator==" << bsl::endl;

    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) == BDLDFP_DECIMAL_DF(5.0)));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) == BDLDFP_DECIMAL_DF(-9.345e27));

    if (veryVerbose) bsl::cout << "Operator!=" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DF(4.0) != BDLDFP_DECIMAL_DF(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DF(-9.345e27) !=
              BDLDFP_DECIMAL_DF(-9.345e27)));

    if (veryVerbose) bsl::cout << "Operator<" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DF(4.0) <
           BDLDFP_DECIMAL_DF(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DF(-9.345e27) <
              BDLDFP_DECIMAL_DF(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DF(5.0) < BDLDFP_DECIMAL_DF(4.0)));

    if (veryVerbose) bsl::cout << "Operator>" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DF(5.0) > BDLDFP_DECIMAL_DF(4.0));
    ASSERT(! (BDLDFP_DECIMAL_DF(-9.345e27) >
              BDLDFP_DECIMAL_DF(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) > BDLDFP_DECIMAL_DF(5.0)));

    if (veryVerbose) bsl::cout << "Operator<=" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DF(4.0) <= BDLDFP_DECIMAL_DF(5.0));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) <= BDLDFP_DECIMAL_DF(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DF(5.0) <= BDLDFP_DECIMAL_DF(4.0)));

    if (veryVerbose) bsl::cout << "Operator>=" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DF(5.0) >= BDLDFP_DECIMAL_DF(4.0));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) >= BDLDFP_DECIMAL_DF(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) >= BDLDFP_DECIMAL_DF(5.0)));

    // TODO TBD: Note that I am testing with the maximum number of
    // significant digits and I actually expect stream output to do no
    // rounding.  That is wrong (see later why), but necessary to
    // remote-test the 'format' function and make sure it does not lose
    // digits or does some unwanted rounding.  When 'format' will be in the
    // lower utility, it will be tested directly.  Expecting to see all
    // digits is wrong because that is not how the stream output should
    // behave: it should print with the default precision as it would for
    // binary floating point *or* with the implied precision of the cohort
    // if that is larger.  AFAIU

    if (veryVerbose) bsl::cout << "Test stream out" << bsl::endl;
    {
        bsl::ostringstream out(pa);

        BDEC::Decimal32 d1(BDLDFP_DECIMAL_DF(-8.327457e-24));
        out << d1;
        bsl::string s(pa);
        getStringFromStream(out, &s);
        LOOP2_ASSERT(s, decLower(s), decLower(s) == "-8.327457e-24");
    }

    if (veryVerbose) bsl::cout << "Test stream in" << bsl::endl;
    {
        bsl::istringstream in(pa);
        bsl::string ins("-8.327457e-24", pa);
        in.str(ins);

        BDEC::Decimal32 d1;
        in >> d1;
        ASSERT(d1 == BDLDFP_DECIMAL_DF(-8.327457e-24));
    }
    {
        bsl::istringstream in(pa);
        bsl::string ins("-832745.7", pa);
        in.str(ins);

        BDEC::Decimal32 d1;
        in >> d1;
        ASSERT(d1 == BDLDFP_DECIMAL_DF(-832745.7));
    }

    if (veryVerbose) bsl::cout << "Test stream in NaN" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("NaN", pa);
        in.str(ins);

        BDEC::Decimal32 d1;
        in >> d1;
        ASSERT(d1 != d1);
    }

    if (veryVerbose) bsl::cout << "Test stream in Infinity" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("Inf", pa);
        in.str(ins);

        BDEC::Decimal32 d1;
        in >> d1;
        ASSERT(d1 > bsl::numeric_limits<BDEC::Decimal32>::max());
    }

    if (veryVerbose) bsl::cout << "Test stream in -Infinity" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("-Inf", pa);
        in.str(ins);

        BDEC::Decimal32 d1;
        in >> d1;
        ASSERT(d1 < -bsl::numeric_limits<BDEC::Decimal32>::max());
    }

    if (veryVerbose) bsl::cout << "Test wide stream out" << bsl::endl;
    {
        bsl::wostringstream  out(pa);
        BDEC::Decimal32 d1(BDLDFP_DECIMAL_DF(-8.327457e-24));
        out << d1;
        bsl::wstring s(pa);
        getStringFromStream(out, &s);
        ASSERT(decLower(s) == L"-8.327457e-24");
    }

    if (veryVerbose) bsl::cout << "Test wide stream in" << bsl::endl;
    {
        bsl::wistringstream  in(pa);
        bsl::wstring ins(L"-8.327457e-24", pa);
        in.str(ins);

        BDEC::Decimal32 d1;
        in >> d1;
        ASSERT(d1 == BDLDFP_DECIMAL_DF(-8.327457e-24));
    }

    if (veryVerbose) bsl::cout << "Operator++" << bsl::endl;
    {
        BDEC::Decimal32 d1(BDLDFP_DECIMAL_DF(-5.0));
        BDEC::Decimal32 d2 = d1++;
        ASSERT(BDLDFP_DECIMAL_DF(-5.0) == d2);
        LOOP_ASSERT(d1, BDLDFP_DECIMAL_DF(-4.0) == d1);
        BDEC::Decimal32 d3 = ++d2;
        LOOP_ASSERT(d2, BDLDFP_DECIMAL_DF(-4.0) == d2);
        LOOP_ASSERT(d3, BDLDFP_DECIMAL_DF(-4.0) == d3);
    }

    if (veryVerbose) bsl::cout << "Operator--" << bsl::endl;
    {
        BDEC::Decimal32 d1(BDLDFP_DECIMAL_DF(-5.0));
        BDEC::Decimal32 d2 = d1--;
        ASSERT(BDLDFP_DECIMAL_DF(-5.0) == d2);
        LOOP_ASSERT(d1, BDLDFP_DECIMAL_DF(-6.0) == d1);
        BDEC::Decimal32 d3 = --d2;
        LOOP_ASSERT(d2, BDLDFP_DECIMAL_DF(-6.0) == d2);
        LOOP_ASSERT(d3, BDLDFP_DECIMAL_DF(-6.0) == d3);
    }

    if (veryVerbose) bsl::cout << "Unary-" << bsl::endl;
    {
        BDEC::Decimal32 d1(BDLDFP_DECIMAL_DF(-5.0));
        BDEC::Decimal32 d2(BDLDFP_DECIMAL_DF( 5.0));
        ASSERT(BDLDFP_DECIMAL_DF( 5.0) == -d1);
        ASSERT(BDLDFP_DECIMAL_DF(-5.0) == -d2);

        // Unary - must make +0 into -0

        bsl::ostringstream out(pa);
        out << -BDLDFP_DECIMAL_DF(0.0);
        bsl::string s(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, s[0] == '-'); // it is negative
        ASSERT(-BDLDFP_DECIMAL_DF(0.0) == BDEC::Decimal32(0)) // and 0
    }

    if (veryVerbose) bsl::cout << "Unary+" << bsl::endl;
    {
        BDEC::Decimal32 d1(BDLDFP_DECIMAL_DF(-5.0));
        BDEC::Decimal32 d2(BDLDFP_DECIMAL_DF( 5.0));
        ASSERT(BDLDFP_DECIMAL_DF(-5.0) == +d1);
        ASSERT(BDLDFP_DECIMAL_DF( 5.0) == +d2);

        // Unary + must make -0 into +0

        BDEC::Decimal128 negzero(-BDLDFP_DECIMAL_DF(0.0));
        bsl::ostringstream out(pa);
        out << +negzero;
        bsl::string s(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, s[0] != '+'); // it is positive
        ASSERT(-BDLDFP_DECIMAL_DF(0.0) == BDEC::Decimal32(0)) // and 0
    }

    if (veryVerbose) bsl::cout << "+=" << bsl::endl;
    {
        BDEC::Decimal32 d(BDLDFP_DECIMAL_DF(-5.0));

        if (veryVeryVerbose) bsl::cout << "+=(int)" << bsl::endl;
        d += 1;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-4.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(unsigned int)" << bsl::endl;
        d += 2u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-2.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(long int)" << bsl::endl;
        d += -1l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-3.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(unsigned long int)"
                                       << bsl::endl;
        d += 5ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(2.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(long long int)" << bsl::endl;
        d += -49ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-47.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(unsigned long long int)"
                                       << bsl::endl;
        d += 42ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-5.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(Decimal32)" << bsl::endl;
        d += BDLDFP_DECIMAL_DF(-22.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-27.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(Decimal64)" << bsl::endl;
        d += BDLDFP_DECIMAL_DD(3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-24.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(Decimal128)" << bsl::endl;
        d += BDLDFP_DECIMAL_DL(9.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-15.0) == d);

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode addition" << bsl::endl;
        if (veryVeryVerbose) bsl::cout << "+=(Decimal32)" << bsl::endl;
        {
            BDEC::Decimal32 d32(     BDLDFP_DECIMAL_DF(0.9999980e+0));
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(1.000001e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(1.999999e+0));
            d32 += rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "+=(Decimal64)" << bsl::endl;
        {
            BDEC::Decimal32 d32(     BDLDFP_DECIMAL_DF(0.9999985e+0));
            BDEC::Decimal64 rhs(     BDLDFP_DECIMAL_DD(1.0000001e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(1.999999e+0));
            d32 += rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "+=(Decimal128)" << bsl::endl;
        {
            BDEC::Decimal32  d32(     BDLDFP_DECIMAL_DF(0.9999985e+0));
            BDEC::Decimal128 rhs(     BDLDFP_DECIMAL_DL(1.0000001e+0));
            BDEC::Decimal32  EXPECTED(BDLDFP_DECIMAL_DF(1.999999e+0));
            d32 += rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "+=(int)" << bsl::endl;
        {
            BDEC::Decimal32  d32(     BDLDFP_DECIMAL_DF( 9999985.0e+0));
            int              rhs(                       10000001      );
            BDEC::Decimal32  EXPECTED(BDLDFP_DECIMAL_DF(19999990.0e+0));
            d32 += rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "+=(unsigned int)" << bsl::endl;
        {
            BDEC::Decimal32  d32(     BDLDFP_DECIMAL_DF( 9999985.0e+0));
            unsigned int     rhs(                       10000001u     );
            BDEC::Decimal32  EXPECTED(BDLDFP_DECIMAL_DF(19999990.0e+0));
            d32 += rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "+=(long)" << bsl::endl;
        {
            BDEC::Decimal32  d32(     BDLDFP_DECIMAL_DF( 9999985.0e+0));
            long             rhs(                       10000001l     );
            BDEC::Decimal32  EXPECTED(BDLDFP_DECIMAL_DF(19999990.0e+0));
            d32 += rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "+=(unsigned long)" << bsl::endl;
        {
            BDEC::Decimal32  d32(     BDLDFP_DECIMAL_DF( 9999985.0e+0));
            unsigned long    rhs(                       10000001ul    );
            BDEC::Decimal32  EXPECTED(BDLDFP_DECIMAL_DF(19999990.0e+0));
            d32 += rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "+=(long long)" << bsl::endl;
        {
            BDEC::Decimal32  d32(     BDLDFP_DECIMAL_DF( 9999985.0e+0));
            long long        rhs(                       10000001ll    );
            BDEC::Decimal32  EXPECTED(BDLDFP_DECIMAL_DF(19999990.0e+0));
            d32 += rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose)
            bsl::cout << "+=(unsigned long long)" << bsl::endl;
        {
            BDEC::Decimal32    d32(     BDLDFP_DECIMAL_DF( 9999985.0e+0));
            unsigned long long rhs(                       10000001ull   );
            BDEC::Decimal32    EXPECTED(BDLDFP_DECIMAL_DF(19999990.0e+0));
            d32 += rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
    }

    if (veryVerbose) bsl::cout << "-=" << bsl::endl;
    {
        BDEC::Decimal32 d(BDLDFP_DECIMAL_DF(-5.0));

        if (veryVeryVerbose) bsl::cout << "-=(int)" << bsl::endl;
        d -= 1;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-6.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(unsigned int)" << bsl::endl;
        d -= 2u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-8.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(long int)" << bsl::endl;
        d -= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(2.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(unsigned long int)"
                                       << bsl::endl;
        d -= 10ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-8.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(long long int)" << bsl::endl;
        d -= -8ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(0.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(unsigned long long int)"
                                << bsl::endl;
        d -= 42ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-42.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(Decimal32)" << bsl::endl;
        d -= BDLDFP_DECIMAL_DF(-22.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-20.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(Decimal64)" << bsl::endl;
        d -= BDLDFP_DECIMAL_DD(3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-23.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(Decimal128)" << bsl::endl;
        d -= BDLDFP_DECIMAL_DL(9.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-32.0) == d);

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode subtraction" << bsl::endl;
        if (veryVeryVerbose) bsl::cout << "-=(Decimal32)" << bsl::endl;
        {
            BDEC::Decimal32 d32(BDLDFP_DECIMAL_DF(2.0e+0));
            BDEC::Decimal32 rhs(BDLDFP_DECIMAL_DF(1.000001e+0));
            d32 -= rhs;
            LOOP_ASSERT(d32, BDLDFP_DECIMAL_DF(0.999999) == d32);
        }
        if (veryVeryVerbose) bsl::cout << "-=(Decimal64)" << bsl::endl;
        {
            BDEC::Decimal32 d32(BDLDFP_DECIMAL_DF(2.0e+0));
            BDEC::Decimal64 rhs(BDLDFP_DECIMAL_DD(1.0000001e+0));
            d32 -= rhs;
            LOOP_ASSERT(d32, BDLDFP_DECIMAL_DF(0.9999999e+0) == d32);
        }
        if (veryVeryVerbose) bsl::cout << "-=(Decimal128)" << bsl::endl;
        {
            BDEC::Decimal32  d32(BDLDFP_DECIMAL_DF(2.0e+0));
            BDEC::Decimal128 rhs(BDLDFP_DECIMAL_DL(1.0000001e+0));
            d32 -= rhs;
            LOOP_ASSERT(d32, BDLDFP_DECIMAL_DF(0.9999999e+0) == d32);
        }
        if (veryVeryVerbose) bsl::cout << "-=(int)" << bsl::endl;
        {
            BDEC::Decimal32 d32(BDLDFP_DECIMAL_DF(20000000.0e+0));
            int             rhs(                  10000001      );
            d32 -= rhs;
            LOOP_ASSERT(d32, BDLDFP_DECIMAL_DF(9999999.0e+0) == d32);
        }
        if (veryVeryVerbose) bsl::cout << "-=(unsigned int)" << bsl::endl;
        {
            BDEC::Decimal32 d32(BDLDFP_DECIMAL_DF(20000000.0e+0));
            unsigned int    rhs(                  10000001u     );
            d32 -= rhs;
            LOOP_ASSERT(d32, BDLDFP_DECIMAL_DF(9999999.0e+0) == d32);
        }
        if (veryVeryVerbose) bsl::cout << "-=(long)" << bsl::endl;
        {
            BDEC::Decimal32 d32(BDLDFP_DECIMAL_DF(20000000.0e+0));
            long            rhs(                  10000001l     );
            d32 -= rhs;
            LOOP_ASSERT(d32, BDLDFP_DECIMAL_DF(9999999.0e+0) == d32);
        }
        if (veryVeryVerbose) bsl::cout << "-=(unsigned long)" << bsl::endl;
        {
            BDEC::Decimal32 d32(BDLDFP_DECIMAL_DF(20000000.0e+0));
            unsigned long   rhs(                  10000001ul     );
            d32 -= rhs;
            LOOP_ASSERT(d32, BDLDFP_DECIMAL_DF(9999999.0e+0) == d32);
        }
        if (veryVeryVerbose) bsl::cout << "-=(long long)" << bsl::endl;
        {
            BDEC::Decimal32 d32(BDLDFP_DECIMAL_DF(20000000.0e+0));
            long long       rhs(                  10000001ll    );
            d32 -= rhs;
            LOOP_ASSERT(d32, BDLDFP_DECIMAL_DF(9999999.0e+0) == d32);
        }
        if (veryVeryVerbose)
            bsl::cout << "-=(unsigned long long)" << bsl::endl;
        {
            BDEC::Decimal32    d32(BDLDFP_DECIMAL_DF(20000000.0e+0));
            unsigned long long rhs(                  10000001ull   );
            d32 -= rhs;
            LOOP_ASSERT(d32, BDLDFP_DECIMAL_DF(9999999.0e+0) == d32);
        }
    }

    if (veryVerbose) bsl::cout << "*=" << bsl::endl;
    {
        BDEC::Decimal32 d(BDLDFP_DECIMAL_DF(-5.0));

        if (veryVeryVerbose) bsl::cout << "*=(int)" << bsl::endl;
        d *= -2;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(10.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(unsigned int)" << bsl::endl;
        d *= 2000u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(20000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(long int)" << bsl::endl;
        d *= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-200000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(unsigned long int)"
                                       << bsl::endl;
        d *= 3ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-600000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(long long int)" << bsl::endl;
        d *= -1ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(600000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(unsigned long long int)"
                                       << bsl::endl;
        d *= 5ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(3000000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(Decimal32)" << bsl::endl;
        d *= BDLDFP_DECIMAL_DF(1e-5);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(30.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(Decimal64)" << bsl::endl;
        d *= BDLDFP_DECIMAL_DD(-3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-90.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(Decimal128)" << bsl::endl;
        d *= BDLDFP_DECIMAL_DL(2.4e-101);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-2.16e-99) == d);

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mix-mode multiplication"
                      << bsl::endl;
        if (veryVeryVerbose) bsl::cout << "*=(Decimal32)" << bsl::endl;
        {
            BDEC::Decimal32 d32(     BDLDFP_DECIMAL_DF(9.0));
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(1.000003));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(9.000027));
            d32 *= rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "*=(Decimal64)" << bsl::endl;
        {
            BDEC::Decimal32 d32(     BDLDFP_DECIMAL_DF(9.0e+0));
            BDEC::Decimal64 rhs(     BDLDFP_DECIMAL_DD(1.0000003e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(9.000003e+0));
            d32 *= rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "*=(Decimal128)" << bsl::endl;
        {
            BDEC::Decimal32  d32(     BDLDFP_DECIMAL_DF(9.0e+0));
            BDEC::Decimal128 rhs(     BDLDFP_DECIMAL_DL(1.0000003e+0));
            BDEC::Decimal32  EXPECTED(BDLDFP_DECIMAL_DF(9.000003e+0));
            d32 *= rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "*=(int)" << bsl::endl;
        {
            BDEC::Decimal32 d32(     BDLDFP_DECIMAL_DF(       9.0e+0));
            int             rhs(                       10000003      );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(90000030.0e+0));
            d32 *= rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "*=(unsigned int)" << bsl::endl;
        {
            BDEC::Decimal32 d32(     BDLDFP_DECIMAL_DF(       9.0e+0));
            unsigned int    rhs(                       10000003u     );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(90000030.0e+0));
            d32 *= rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "*=(long)" << bsl::endl;
        {
            BDEC::Decimal32 d32(     BDLDFP_DECIMAL_DF(       9.0e+0));
            long            rhs(                       10000003l     );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(90000030.0e+0));
            d32 *= rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "*=(unsigned long)" << bsl::endl;
        {
            BDEC::Decimal32 d32(     BDLDFP_DECIMAL_DF(       9.0e+0));
            unsigned long   rhs(                       10000003ul    );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(90000030.0e+0));
            d32 *= rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "*=(long long)" << bsl::endl;
        {
            BDEC::Decimal32 d32(     BDLDFP_DECIMAL_DF(       9.0e+0));
            long long       rhs(                       10000003ll     );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(90000030.0e+0));
            d32 *= rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose)
            bsl::cout << "*=(unsigned long long)" << bsl::endl;
        {
            BDEC::Decimal32    d32(     BDLDFP_DECIMAL_DF(       9.0e+0));
            unsigned long long rhs(                       10000003ull   );
            BDEC::Decimal32    EXPECTED(BDLDFP_DECIMAL_DF(90000030.0e+0));
            d32 *= rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
    }

    if (veryVerbose) bsl::cout << "/=" << bsl::endl;
    {
        BDEC::Decimal32 d(BDLDFP_DECIMAL_DF(-5.0));

        if (veryVeryVerbose) bsl::cout << "/=(int)" << bsl::endl;
        d /= -2;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(2.5) == d);

        if (veryVeryVerbose) bsl::cout << "/=(unsigned int)" << bsl::endl;
        d /= 2000u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(0.00125) == d);

        if (veryVeryVerbose) bsl::cout << "/=(long int)" << bsl::endl;
        d /= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-0.000125) == d);

        if (veryVeryVerbose) bsl::cout << "/=(unsigned long int)"
                                       << bsl::endl;
        d /= 5ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-0.000025) == d);

        if (veryVeryVerbose) bsl::cout << "/=(long long int)" << bsl::endl;
        d /= -5ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(0.000005) == d);

        if (veryVeryVerbose) bsl::cout << "/=(unsigned long long int)"
                                       << bsl::endl;
        d /= 5ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(0.000001) == d);

        if (veryVeryVerbose) bsl::cout << "/=(Decimal32)" << bsl::endl;
        d /= BDLDFP_DECIMAL_DF(1e-5);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(0.1) == d);

        if (veryVeryVerbose) bsl::cout << "/=(Decimal64)" << bsl::endl;
        d /= BDLDFP_DECIMAL_DD(-5.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DF(-0.02) == d);

        BDEC::Decimal32 d32(BDLDFP_DECIMAL_DF(-5e-100));
        if (veryVeryVerbose) bsl::cout << "/=(Decimal128)" << bsl::endl;
        d32 /= BDLDFP_DECIMAL_DL(-2.5e-134);
                            LOOP_ASSERT(d32, BDLDFP_DECIMAL_DF(2e34) == d32);

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mix-mode division" << bsl::endl;
        if (veryVeryVerbose) bsl::cout << "/=(Decimal32)" << bsl::endl;
        {
            BDEC::Decimal32 d32(     BDLDFP_DECIMAL_DF(1.0));
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(1.000001));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(0.999999));
            d32 /= rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "/=(Decimal64)" << bsl::endl;
        {
            BDEC::Decimal32 d32(     BDLDFP_DECIMAL_DF(1.0e+0));
            BDEC::Decimal64 rhs(     BDLDFP_DECIMAL_DD(1.0000001e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(0.9999999e+0));
            d32 /= rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "/=(int)" << bsl::endl;
        {
            BDEC::Decimal32 d32(BDLDFP_DECIMAL_DF(10000000.0e+0      ));
            int             rhs(                  10000001            );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(  0.9999999e+0));
            d32 /= rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "/=(unsigned int)" << bsl::endl;
        {
            BDEC::Decimal32 d32(BDLDFP_DECIMAL_DF(10000000.0e+0      ));
            unsigned int    rhs(                  10000001u            );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(  0.9999999e+0));
            d32 /= rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "/=(long)" << bsl::endl;
        {
            BDEC::Decimal32 d32(BDLDFP_DECIMAL_DF(10000000.0e+0      ));
            long            rhs(                  10000001l           );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(  0.9999999e+0));
            d32 /= rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "/=(unsigned long)" << bsl::endl;
        {
            BDEC::Decimal32 d32(BDLDFP_DECIMAL_DF(10000000.0e+0      ));
            unsigned long   rhs(                  10000001ul           );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(  0.9999999e+0));
            d32 /= rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose) bsl::cout << "/=(long long)" << bsl::endl;
        {
            BDEC::Decimal32 d32(BDLDFP_DECIMAL_DF(10000000.0e+0      ));
            long long       rhs(                  10000001ll          );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(  0.9999999e+0));
            d32 /= rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
        if (veryVeryVerbose)
            bsl::cout << "/=(unsigned long long)" << bsl::endl;
        {
            BDEC::Decimal32    d32(    BDLDFP_DECIMAL_DF(10000000.0e+0     ));
            unsigned long long rhs(                      10000001ull         );
            BDEC::Decimal32    EXPECTED(BDLDFP_DECIMAL_DF(      0.9999999e+0));
            d32 /= rhs;
            LOOP_ASSERT(d32, EXPECTED == d32);
        }
    }

    if (veryVerbose) bsl::cout << "operator+" << bsl::endl;
    {
        BDEC::Decimal32 d(BDLDFP_DECIMAL_DF(-5.0));

        if (veryVeryVerbose) bsl::cout << "dec + int" << bsl::endl;
        LOOP_ASSERT(d + 1, d + 1 == BDLDFP_DECIMAL_DF(-4.0));
        LOOP_ASSERT(1 + d, 1 + d == BDLDFP_DECIMAL_DF(-4.0));

        if (veryVeryVerbose) bsl::cout << "dec + unsigned int"
                                       << bsl::endl;
        LOOP_ASSERT(d + 10u, d + 10u == BDLDFP_DECIMAL_DF(5.0));
        LOOP_ASSERT(10u + d, 10u + d == BDLDFP_DECIMAL_DF(5.0));

        if (veryVeryVerbose) bsl::cout << "dec + long int" << bsl::endl;
        LOOP_ASSERT(d + 1l, d + 1l == BDLDFP_DECIMAL_DF(-4.0));
        LOOP_ASSERT(1l + d, 1l + d == BDLDFP_DECIMAL_DF(-4.0));

        if (veryVeryVerbose) bsl::cout << "dec + unsigned long int"
                                       << bsl::endl;
        LOOP_ASSERT(d + 10ul, d + 10ul == BDLDFP_DECIMAL_DF(5.0));
        LOOP_ASSERT(10ul + d, 10ul + d == BDLDFP_DECIMAL_DF(5.0));

        if (veryVeryVerbose) bsl::cout << "dec + long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d + 1ll, d + 1ll == BDLDFP_DECIMAL_DF(-4.0));
        LOOP_ASSERT(1ll + d, 1ll + d == BDLDFP_DECIMAL_DF(-4.0));

        if (veryVeryVerbose) bsl::cout << "dec + unsigned long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d + 10ull, d + 10ull == BDLDFP_DECIMAL_DF(5.0));
        LOOP_ASSERT(10ull + d, 10ull + d == BDLDFP_DECIMAL_DF(5.0));

        if (veryVeryVerbose) bsl::cout << "dec + dec32" << bsl::endl;
        LOOP_ASSERT(d + BDLDFP_DECIMAL_DF(-3.0),
                    d + BDLDFP_DECIMAL_DF(-3.0) ==
                    BDLDFP_DECIMAL_DF(-8.0));


        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode addition" << bsl::endl;
        if (veryVeryVerbose) bsl::cout << "dec + dec32" << bsl::endl;
        {
            BDEC::Decimal32 lhs(     BDLDFP_DECIMAL_DF(0.9999980));
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(1.000001));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(1.999999));
            LOOP_ASSERT(lhs + rhs, EXPECTED == lhs + rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec + int" << bsl::endl;
        {
            BDEC::Decimal32  lhs(     BDLDFP_DECIMAL_DF( 9999985.0e+0));
            int              rhs(                       10000001      );
            BDEC::Decimal32  EXPECTED(BDLDFP_DECIMAL_DF(19999990.0e+0));
            LOOP_ASSERT(lhs + rhs, EXPECTED == lhs + rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec + unsigned int" << bsl::endl;
        {
            BDEC::Decimal32  lhs(     BDLDFP_DECIMAL_DF( 9999985.0e+0));
            unsigned int     rhs(                       10000001u     );
            BDEC::Decimal32  EXPECTED(BDLDFP_DECIMAL_DF(19999990.0e+0));
            LOOP_ASSERT(lhs + rhs, EXPECTED == lhs + rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec + long" << bsl::endl;
        {
            BDEC::Decimal32  lhs(     BDLDFP_DECIMAL_DF( 9999985.0e+0));
            long             rhs(                       10000001l     );
            BDEC::Decimal32  EXPECTED(BDLDFP_DECIMAL_DF(19999990.0e+0));
            LOOP_ASSERT(lhs + rhs, EXPECTED == lhs + rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec + unsigned long" << bsl::endl;
        {
            BDEC::Decimal32  lhs(     BDLDFP_DECIMAL_DF( 9999985.0e+0));
            unsigned long    rhs(                       10000001ul    );
            BDEC::Decimal32  EXPECTED(BDLDFP_DECIMAL_DF(19999990.0e+0));
            LOOP_ASSERT(lhs + rhs, EXPECTED == lhs + rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec + long long" << bsl::endl;
        {
            BDEC::Decimal32  lhs(     BDLDFP_DECIMAL_DF( 9999985.0e+0));
            long long        rhs(                       10000001ll    );
            BDEC::Decimal32  EXPECTED(BDLDFP_DECIMAL_DF(19999990.0e+0));
            LOOP_ASSERT(lhs + rhs, EXPECTED == lhs + rhs);
        }
        if (veryVeryVerbose)
            bsl::cout << "dec + unsigned long long" << bsl::endl;
        {
            BDEC::Decimal32     lhs(     BDLDFP_DECIMAL_DF( 9999985.0e+0));
            unsigned long long  rhs(                       10000001ull    );
            BDEC::Decimal32     EXPECTED(BDLDFP_DECIMAL_DF(19999990.0e+0));
            LOOP_ASSERT(lhs + rhs, EXPECTED == lhs + rhs);
        }
        if (veryVeryVerbose) bsl::cout << "int + dec" << bsl::endl;
        {
            int              lhs(                       10000001      );
            BDEC::Decimal32  rhs(     BDLDFP_DECIMAL_DF( 9999985.0e+0));
            BDEC::Decimal32  EXPECTED(BDLDFP_DECIMAL_DF(19999990.0e+0));
            LOOP_ASSERT(lhs + rhs, EXPECTED == lhs + rhs);
        }
        if (veryVeryVerbose) bsl::cout << "unsigned int + dec" << bsl::endl;
        {
            unsigned int     lhs(                       10000001u     );
            BDEC::Decimal32  rhs(     BDLDFP_DECIMAL_DF( 9999985.0e+0));
            BDEC::Decimal32  EXPECTED(BDLDFP_DECIMAL_DF(19999990.0e+0));
            LOOP_ASSERT(lhs + rhs, EXPECTED == lhs + rhs);
        }
        if (veryVeryVerbose) bsl::cout << "long + dec" << bsl::endl;
        {
            long             lhs(                       10000001l     );
            BDEC::Decimal32  rhs(     BDLDFP_DECIMAL_DF( 9999985.0e+0));
            BDEC::Decimal32  EXPECTED(BDLDFP_DECIMAL_DF(19999990.0e+0));
            LOOP_ASSERT(lhs + rhs, EXPECTED == lhs + rhs);
        }
        if (veryVeryVerbose) bsl::cout << "unsigned long + dec" << bsl::endl;
        {
            unsigned long    lhs(                       10000001ul    );
            BDEC::Decimal32  rhs(     BDLDFP_DECIMAL_DF( 9999985.0e+0));
            BDEC::Decimal32  EXPECTED(BDLDFP_DECIMAL_DF(19999990.0e+0));
            LOOP_ASSERT(lhs + rhs, EXPECTED == lhs + rhs);
        }
        if (veryVeryVerbose) bsl::cout << "long long + dec" << bsl::endl;
        {
            long long        lhs(                       10000001ll    );
            BDEC::Decimal32  rhs(     BDLDFP_DECIMAL_DF( 9999985.0e+0));
            BDEC::Decimal32  EXPECTED(BDLDFP_DECIMAL_DF(19999990.0e+0));
            LOOP_ASSERT(lhs + rhs, EXPECTED == lhs + rhs);
        }
        if (veryVeryVerbose)
            bsl::cout << "unsigned long long + dec" << bsl::endl;
        {
            unsigned long long  lhs(                       10000001ull    );
            BDEC::Decimal32     rhs(     BDLDFP_DECIMAL_DF( 9999985.0e+0));
            BDEC::Decimal32     EXPECTED(BDLDFP_DECIMAL_DF(19999990.0e+0));
            LOOP_ASSERT(lhs + rhs, EXPECTED == lhs + rhs);
        }
    }

    if (veryVerbose) bsl::cout << "operator-" << bsl::endl;
    {
        BDEC::Decimal32 d(BDLDFP_DECIMAL_DF(-5.0));

        if (veryVeryVerbose) bsl::cout << "dec - int" << bsl::endl;
        LOOP_ASSERT(d - 1, d - 1 == BDLDFP_DECIMAL_DF(-6.0));
        LOOP_ASSERT(1 - d, 1 - d == BDLDFP_DECIMAL_DF( 6.0));

        if (veryVeryVerbose) bsl::cout << "dec - unsigned int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 10u, d - 10u == BDLDFP_DECIMAL_DF(-15.0));
        LOOP_ASSERT(10u - d, 10u - d == BDLDFP_DECIMAL_DF( 15.0));

        if (veryVeryVerbose) bsl::cout << "dec - long int" << bsl::endl;
        LOOP_ASSERT(d - 1l, d - 1l == BDLDFP_DECIMAL_DF(-6.0));
        LOOP_ASSERT(1l - d, 1l - d == BDLDFP_DECIMAL_DF( 6.0));

        if (veryVeryVerbose) bsl::cout << "dec - unsigned long int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 10ul, d - 10ul == BDLDFP_DECIMAL_DF(-15.0));
        LOOP_ASSERT(10ul - d, 10ul - d == BDLDFP_DECIMAL_DF( 15.0));

        if (veryVeryVerbose) bsl::cout << "dec - long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 1ll, d - 1ll == BDLDFP_DECIMAL_DF(-6.0));
        LOOP_ASSERT(1ll - d, 1ll - d == BDLDFP_DECIMAL_DF( 6.0));

        if (veryVeryVerbose) bsl::cout << "dec - unsigned long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 10ull, d - 10ull == BDLDFP_DECIMAL_DF(-15.0));
        LOOP_ASSERT(10ull - d, 10ull - d == BDLDFP_DECIMAL_DF( 15.0));

        if (veryVeryVerbose) bsl::cout << "dec - dec32" << bsl::endl;
        LOOP_ASSERT(d - BDLDFP_DECIMAL_DF(-3.0),
                    d - BDLDFP_DECIMAL_DF(-3.0) ==
                    BDLDFP_DECIMAL_DF(-2.0));

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode subtraction"
                      << bsl::endl;
        if (veryVeryVerbose) bsl::cout << "dec - dec32" << bsl::endl;
        {
            BDEC::Decimal32 lhs(     BDLDFP_DECIMAL_DF(2.0));
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(1.000001));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(0.999999));
            LOOP_ASSERT(lhs - rhs, EXPECTED == lhs - rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec - int" << bsl::endl;
        {
            BDEC::Decimal32 lhs(     BDLDFP_DECIMAL_DF(20000000.0e+0));
            int             rhs(                       10000001      );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF( 9999999.0e+0));
            LOOP_ASSERT(lhs - rhs, EXPECTED == lhs - rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec - unsigned int" << bsl::endl;
        {
            BDEC::Decimal32 lhs(     BDLDFP_DECIMAL_DF(20000000.0e+0));
            unsigned int    rhs(                       10000001u     );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF( 9999999.0e+0));
            LOOP_ASSERT(lhs - rhs, EXPECTED == lhs - rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec - long" << bsl::endl;
        {
            BDEC::Decimal32 lhs(     BDLDFP_DECIMAL_DF(20000000.0e+0));
            long            rhs(                       10000001l     );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF( 9999999.0e+0));
            LOOP_ASSERT(lhs - rhs, EXPECTED == lhs - rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec - unsigned long" << bsl::endl;
        {
            BDEC::Decimal32 lhs(     BDLDFP_DECIMAL_DF(20000000.0e+0));
            unsigned long   rhs(                       10000001ul    );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF( 9999999.0e+0));
            LOOP_ASSERT(lhs - rhs, EXPECTED == lhs - rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec - long long" << bsl::endl;
        {
            BDEC::Decimal32 lhs(     BDLDFP_DECIMAL_DF(20000000.0e+0));
            long long       rhs(                       10000001ll    );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF( 9999999.0e+0));
            LOOP_ASSERT(lhs - rhs, EXPECTED == lhs - rhs);
        }
        if (veryVeryVerbose)
            bsl::cout << "dec - unsigned long long" << bsl::endl;
        {
            BDEC::Decimal32    lhs(     BDLDFP_DECIMAL_DF(20000000.0e+0));
            unsigned long long rhs(                       10000001ull   );
            BDEC::Decimal32    EXPECTED(BDLDFP_DECIMAL_DF( 9999999.0e+0));
            LOOP_ASSERT(lhs - rhs, EXPECTED == lhs - rhs);
        }
        if (veryVeryVerbose) bsl::cout << "int - dec" << bsl::endl;
        {
            int             lhs(                       10000001      );
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(20000000.0e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(-9999999.0e+0));
            LOOP_ASSERT(lhs - rhs, EXPECTED == lhs - rhs);
        }
        if (veryVeryVerbose) bsl::cout << "unsigned int - dec" << bsl::endl;
        {
            unsigned int    lhs(                       10000001u     );
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(20000000.0e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(-9999999.0e+0));
            LOOP_ASSERT(lhs - rhs, EXPECTED == lhs - rhs);
        }
        if (veryVeryVerbose) bsl::cout << "long - dec" << bsl::endl;
        {
            long            lhs(                       10000001l     );
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(20000000.0e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(-9999999.0e+0));
            LOOP_ASSERT(lhs - rhs, EXPECTED == lhs - rhs);
        }
        if (veryVeryVerbose) bsl::cout << "unsigned long - dec" << bsl::endl;
        {
            unsigned long   lhs(                       10000001ul    );
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(20000000.0e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(-9999999.0e+0));
            LOOP_ASSERT(lhs - rhs, EXPECTED == lhs - rhs);
        }
        if (veryVeryVerbose) bsl::cout << "long long - dec" << bsl::endl;
        {
            long long       lhs(                       10000001ll    );
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(20000000.0e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(-9999999.0e+0));
            LOOP_ASSERT(lhs - rhs, EXPECTED == lhs - rhs);
        }
        if (veryVeryVerbose)
            bsl::cout << "unsigned long long - dec" << bsl::endl;
        {
            unsigned long long lhs(                       10000001ull   );
            BDEC::Decimal32    rhs(     BDLDFP_DECIMAL_DF(20000000.0e+0));
            BDEC::Decimal32    EXPECTED(BDLDFP_DECIMAL_DF(-9999999.0e+0));
            LOOP_ASSERT(lhs - rhs, EXPECTED == lhs - rhs);
        }
    }

    if (veryVerbose) bsl::cout << "operator*" << bsl::endl;
    {
        BDEC::Decimal32 d(BDLDFP_DECIMAL_DF(-5.0));

        if (veryVeryVerbose) bsl::cout << "dec * int" << bsl::endl;
        LOOP_ASSERT(d * -2, d * -2 == BDLDFP_DECIMAL_DF(10.0));
        LOOP_ASSERT(-2 * d, -2 * d == BDLDFP_DECIMAL_DF(10.0));

        if (veryVeryVerbose) bsl::cout << "dec * unsigned int"
                                       << bsl::endl;
        LOOP_ASSERT(d * 10u, d * 10u == BDLDFP_DECIMAL_DF(-50.0));
        LOOP_ASSERT(10u * d, 10u * d == BDLDFP_DECIMAL_DF(-50.0));

        if (veryVeryVerbose) bsl::cout << "dec * long int" << bsl::endl;
        LOOP_ASSERT(d * -2l, d * -2l == BDLDFP_DECIMAL_DF(10.0));
        LOOP_ASSERT(-2l * d, -2l * d == BDLDFP_DECIMAL_DF(10.0));

        if (veryVeryVerbose) bsl::cout << "dec * unsigned long int"
                                       << bsl::endl;
        LOOP_ASSERT(d * 10ul, d * 10ul == BDLDFP_DECIMAL_DF(-50.0));
        LOOP_ASSERT(10ul * d, 10ul * d == BDLDFP_DECIMAL_DF(-50.0));

        if (veryVeryVerbose) bsl::cout << "dec * long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d * -2ll, d * -2ll == BDLDFP_DECIMAL_DF(10.0));
        LOOP_ASSERT(-2ll * d, -2ll * d == BDLDFP_DECIMAL_DF(10.0));

        if (veryVeryVerbose) bsl::cout << "dec * unsigned long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d * 10ull, d * 10ull == BDLDFP_DECIMAL_DF(-50.0));
        LOOP_ASSERT(10ull * d, 10ull * d == BDLDFP_DECIMAL_DF(-50.0));

        if (veryVeryVerbose) bsl::cout << "dec * dec32" << bsl::endl;
        LOOP_ASSERT(d * BDLDFP_DECIMAL_DF(-3.0),
                    d * BDLDFP_DECIMAL_DF(-3.0) ==
                    BDLDFP_DECIMAL_DF(15.0));

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode multiplication"
                      << bsl::endl;
        if (veryVeryVerbose) bsl::cout << "dec * dec32" << bsl::endl;
        {
            BDEC::Decimal32 lhs(     BDLDFP_DECIMAL_DF(9.0));
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(1.000003));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(9.000027));
            LOOP_ASSERT(lhs * rhs, EXPECTED == lhs * rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec * int" << bsl::endl;
        {
            BDEC::Decimal32 lhs(     BDLDFP_DECIMAL_DF(       9.0e+0));
            int             rhs(                       10000003      );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(90000030.0e+0));
            LOOP_ASSERT(lhs * rhs, EXPECTED == lhs * rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec * unsigned int" << bsl::endl;
        {
            BDEC::Decimal32 lhs(     BDLDFP_DECIMAL_DF(       9.0e+0));
            unsigned int    rhs(                       10000003u     );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(90000030.0e+0));
            LOOP_ASSERT(lhs * rhs, EXPECTED == lhs * rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec * long" << bsl::endl;
        {
            BDEC::Decimal32 lhs(     BDLDFP_DECIMAL_DF(       9.0e+0));
            long            rhs(                       10000003l     );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(90000030.0e+0));
            LOOP_ASSERT(lhs * rhs, EXPECTED == lhs * rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec * unsigned long" << bsl::endl;
        {
            BDEC::Decimal32 lhs(     BDLDFP_DECIMAL_DF(       9.0e+0));
            unsigned long   rhs(                       10000003ul    );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(90000030.0e+0));
            LOOP_ASSERT(lhs * rhs, EXPECTED == lhs * rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec * long long" << bsl::endl;
        {
            BDEC::Decimal32 lhs(     BDLDFP_DECIMAL_DF(       9.0e+0));
            long            rhs(                       10000003ll    );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(90000030.0e+0));
            LOOP_ASSERT(lhs * rhs, EXPECTED == lhs * rhs);
        }
        if (veryVeryVerbose)
            bsl::cout << "dec * unsigned long long" << bsl::endl;
        {
            BDEC::Decimal32 lhs(     BDLDFP_DECIMAL_DF(       9.0e+0));
            unsigned long   rhs(                       10000003ull   );
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(90000030.0e+0));
            LOOP_ASSERT(lhs * rhs, EXPECTED == lhs * rhs);
        }
        if (veryVeryVerbose) bsl::cout << "int * dec" << bsl::endl;
        {
            int             lhs(                       10000003      );
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(       9.0e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(90000030.0e+0));
            LOOP_ASSERT(lhs * rhs, EXPECTED == lhs * rhs);
        }
        if (veryVeryVerbose) bsl::cout << "unsigned int * dec" << bsl::endl;
        {
            unsigned int    lhs(                       10000003u     );
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(       9.0e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(90000030.0e+0));
            LOOP_ASSERT(lhs * rhs, EXPECTED == lhs * rhs);
        }
        if (veryVeryVerbose) bsl::cout << "long * dec" << bsl::endl;
        {
            long            lhs(                       10000003l     );
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(       9.0e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(90000030.0e+0));
            LOOP_ASSERT(lhs * rhs, EXPECTED == lhs * rhs);
        }
        if (veryVeryVerbose) bsl::cout << "unsigned long * dec" << bsl::endl;
        {
            unsigned long   lhs(                       10000003ul    );
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(       9.0e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(90000030.0e+0));
            LOOP_ASSERT(lhs * rhs, EXPECTED == lhs * rhs);
        }
        if (veryVeryVerbose) bsl::cout << "long long * dec" << bsl::endl;
        {
            long            lhs(                       10000003ll    );
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(       9.0e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(90000030.0e+0));
            LOOP_ASSERT(lhs * rhs, EXPECTED == lhs * rhs);
        }
        if (veryVeryVerbose)
            bsl::cout << "unsigned long long * dec" << bsl::endl;
        {
            unsigned long   lhs(                       10000003ull   );
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(       9.0e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(90000030.0e+0));
            LOOP_ASSERT(lhs * rhs, EXPECTED == lhs * rhs);
        }
    }

    if (veryVerbose) bsl::cout << "operator/" << bsl::endl;
    {
        BDEC::Decimal32 d(BDLDFP_DECIMAL_DF(-5.0));

        if (veryVeryVerbose) bsl::cout << "dec / int" << bsl::endl;
        LOOP_ASSERT(d / -2, d / -2 == BDLDFP_DECIMAL_DF(2.5));
        LOOP_ASSERT(-2 / d, -2 / d == BDLDFP_DECIMAL_DF(0.4));

        if (veryVeryVerbose) bsl::cout << "dec / unsigned int"
                                       << bsl::endl;
        LOOP_ASSERT(d / 100u, d / 100u == BDLDFP_DECIMAL_DF( -0.05));
        LOOP_ASSERT(100u / d, 100u / d == BDLDFP_DECIMAL_DF(-20.0));

        if (veryVeryVerbose) bsl::cout << "dec / long int" << bsl::endl;
        LOOP_ASSERT(d / -2l, d / -2l == BDLDFP_DECIMAL_DF(2.5));
        LOOP_ASSERT(-2l / d, -2l / d == BDLDFP_DECIMAL_DF(0.4));

        if (veryVeryVerbose) bsl::cout << "dec / unsigned long int"
                                       << bsl::endl;
        LOOP_ASSERT(d / 100ul, d / 100ul == BDLDFP_DECIMAL_DF( -0.05));
        LOOP_ASSERT(100ul / d, 100ul / d == BDLDFP_DECIMAL_DF(-20.0));

        if (veryVeryVerbose) bsl::cout << "dec / long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d / -2ll, d / -2ll == BDLDFP_DECIMAL_DF(2.5));
        LOOP_ASSERT(-2ll / d, -2ll / d == BDLDFP_DECIMAL_DF(0.4));

        if (veryVeryVerbose) bsl::cout << "dec / unsigned long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d / 100ull, d / 100ull == BDLDFP_DECIMAL_DF( -0.05));
        LOOP_ASSERT(100ull / d, 100ull / d == BDLDFP_DECIMAL_DF(-20.0));

        if (veryVeryVerbose) bsl::cout << "dec / dec2" << bsl::endl;
        LOOP_ASSERT(d / BDLDFP_DECIMAL_DF(-50.0),
                    d / BDLDFP_DECIMAL_DF(-50.0) ==
                    BDLDFP_DECIMAL_DF(0.1));

        if (veryVeryVerbose)
            bsl::cout << "Test precision in mixed-mode division"
                      << bsl::endl;
        if (veryVeryVerbose) bsl::cout << "dec / dec32" << bsl::endl;
        {
            BDEC::Decimal32 lhs(     BDLDFP_DECIMAL_DF(1.0));
            BDEC::Decimal32 rhs(     BDLDFP_DECIMAL_DF(1.000001));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(0.999999));
            LOOP_ASSERT(lhs / rhs, EXPECTED == lhs / rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec / int" << bsl::endl;
        {
            BDEC::Decimal32 lhs(BDLDFP_DECIMAL_DF(10000000.0e+0      ));
            int             rhs(                  10000001            );
            BDEC::Decimal32 EXPECTED(  BDLDFP_DECIMAL_DF(0.9999999e+0));
            LOOP_ASSERT(lhs / rhs, EXPECTED == lhs / rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec / unsigned int" << bsl::endl;
        {
            BDEC::Decimal32 lhs(BDLDFP_DECIMAL_DF(10000000.0e+0      ));
            int             rhs(                  10000001u           );
            BDEC::Decimal32 EXPECTED(  BDLDFP_DECIMAL_DF(0.9999999e+0));
            LOOP_ASSERT(lhs / rhs, EXPECTED == lhs / rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec / long" << bsl::endl;
        {
            BDEC::Decimal32 lhs(BDLDFP_DECIMAL_DF(10000000.0e+0      ));
            long            rhs(                  10000001l           );
            BDEC::Decimal32 EXPECTED(  BDLDFP_DECIMAL_DF(0.9999999e+0));
            LOOP_ASSERT(lhs / rhs, EXPECTED == lhs / rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec / unsigned long" << bsl::endl;
        {
            BDEC::Decimal32 lhs(BDLDFP_DECIMAL_DF(10000000.0e+0      ));
            unsigned long   rhs(                  10000001ul          );
            BDEC::Decimal32 EXPECTED(  BDLDFP_DECIMAL_DF(0.9999999e+0));
            LOOP_ASSERT(lhs / rhs, EXPECTED == lhs / rhs);
        }
        if (veryVeryVerbose) bsl::cout << "dec / long long" << bsl::endl;
        {
            BDEC::Decimal32 lhs(BDLDFP_DECIMAL_DF(10000000.0e+0      ));
            long long       rhs(                  10000001ll          );
            BDEC::Decimal32 EXPECTED(  BDLDFP_DECIMAL_DF(0.9999999e+0));
            LOOP_ASSERT(lhs / rhs, EXPECTED == lhs / rhs);
        }
        if (veryVeryVerbose)
            bsl::cout << "dec / unsigned long long" << bsl::endl;
        {
            BDEC::Decimal32    lhs(BDLDFP_DECIMAL_DF(10000000.0e+0      ));
            unsigned long long rhs(                  10000001ull         );
            BDEC::Decimal32    EXPECTED(  BDLDFP_DECIMAL_DF(0.9999999e+0));
            LOOP_ASSERT(lhs / rhs, EXPECTED == lhs / rhs);
        }
        if (veryVeryVerbose) bsl::cout << "int / dec" << bsl::endl;
        {
            BDEC::Decimal32 d32l(BDLDFP_DECIMAL_DF(20000011.0e+0));
            BDEC::Decimal32 d32r(BDLDFP_DECIMAL_DF(       2.0e+0));
            BDEC::Decimal32 EXP( BDLDFP_DECIMAL_DF(10000000.0e+0));
            LOOP_ASSERT(d32l / d32r, EXP == d32l / d32r);

            int             lhs(                       20000011      );
            BDEC::Decimal32 rhs(BDLDFP_DECIMAL_DF(            2.0e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(10000010.0e+0));
            LOOP_ASSERT(lhs / rhs, EXPECTED == lhs / rhs);
        }
        if (veryVeryVerbose) bsl::cout << "unsigned int / dec" << bsl::endl;
        {
            unsigned int    lhs(                       20000011u     );
            BDEC::Decimal32 rhs(BDLDFP_DECIMAL_DF(            2.0e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(10000010.0e+0));
            LOOP_ASSERT(lhs / rhs, EXPECTED == lhs / rhs);
        }
        if (veryVeryVerbose) bsl::cout << "long / dec" << bsl::endl;
        {
            long            lhs(                       20000011l     );
            BDEC::Decimal32 rhs(BDLDFP_DECIMAL_DF(            2.0e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(10000010.0e+0));
            LOOP_ASSERT(lhs / rhs, EXPECTED == lhs / rhs);
        }
        if (veryVeryVerbose) bsl::cout << "unsigned long / dec" << bsl::endl;
        {
            unsigned long   lhs(                       20000011ul    );
            BDEC::Decimal32 rhs(BDLDFP_DECIMAL_DF(            2.0e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(10000010.0e+0));
            LOOP_ASSERT(lhs / rhs, EXPECTED == lhs / rhs);
        }
        if (veryVeryVerbose) bsl::cout << "long long / dec" << bsl::endl;
        {
            long long       lhs(                       20000011ll    );
            BDEC::Decimal32 rhs(BDLDFP_DECIMAL_DF(            2.0e+0));
            BDEC::Decimal32 EXPECTED(BDLDFP_DECIMAL_DF(10000010.0e+0));
            LOOP_ASSERT(lhs / rhs, EXPECTED == lhs / rhs);
        }
        if (veryVeryVerbose) bsl::cout << "unsigned long / dec" << bsl::endl;
        {
            unsigned long long lhs(                       20000011ull   );
            BDEC::Decimal32    rhs(BDLDFP_DECIMAL_DF(            2.0e+0));
            BDEC::Decimal32    EXPECTED(BDLDFP_DECIMAL_DF(10000010.0e+0));
            LOOP_ASSERT(lhs / rhs, EXPECTED == lhs / rhs);
        }
    }

    if (veryVerbose) bsl::cout << "Create test objects" << bsl::endl;

    BDEC::Decimal32        d32  = BDEC::Decimal32();
    const BDEC::Decimal32  c32  = BDEC::Decimal32();

    if (veryVerbose) bsl::cout << "Check return types" << bsl::endl;

    checkType<BDEC::Decimal32&>(++d32);
    checkType<BDEC::Decimal32>(d32++);
    checkType<BDEC::Decimal32&>(--d32);
    checkType<BDEC::Decimal32>(d32--);
    checkType<BDEC::Decimal32&>(d32 += static_cast<char>(1));
    checkType<BDEC::Decimal32&>(d32 += static_cast<unsigned char>(1));
    checkType<BDEC::Decimal32&>(d32 += static_cast<signed char>(1));
    checkType<BDEC::Decimal32&>(d32 += static_cast<short>(1));
    checkType<BDEC::Decimal32&>(d32 += static_cast<unsigned short>(1));
    checkType<BDEC::Decimal32&>(d32 += static_cast<int>(1));
    checkType<BDEC::Decimal32&>(d32 += static_cast<unsigned int>(1));
    checkType<BDEC::Decimal32&>(d32 += static_cast<long>(1));
    checkType<BDEC::Decimal32&>(d32 += static_cast<unsigned long>(1));
    checkType<BDEC::Decimal32&>(d32 += static_cast<long long>(1));
    checkType<BDEC::Decimal32&>(d32 += static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal32&>(d32 += static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal32&>(d32 += static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal32&>(d32 += static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal32&>(d32 -= static_cast<char>(1));
    checkType<BDEC::Decimal32&>(d32 -= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal32&>(d32 -= static_cast<signed char>(1));
    checkType<BDEC::Decimal32&>(d32 -= static_cast<short>(1));
    checkType<BDEC::Decimal32&>(d32 -= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal32&>(d32 -= static_cast<int>(1));
    checkType<BDEC::Decimal32&>(d32 -= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal32&>(d32 -= static_cast<long>(1));
    checkType<BDEC::Decimal32&>(d32 -= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal32&>(d32 -= static_cast<long long>(1));
    checkType<BDEC::Decimal32&>(d32 -= static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal32&>(d32 -= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal32&>(d32 -= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal32&>(d32 -= static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal32&>(d32 *= static_cast<char>(1));
    checkType<BDEC::Decimal32&>(d32 *= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal32&>(d32 *= static_cast<signed char>(1));
    checkType<BDEC::Decimal32&>(d32 *= static_cast<short>(1));
    checkType<BDEC::Decimal32&>(d32 *= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal32&>(d32 *= static_cast<int>(1));
    checkType<BDEC::Decimal32&>(d32 *= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal32&>(d32 *= static_cast<long>(1));
    checkType<BDEC::Decimal32&>(d32 *= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal32&>(d32 *= static_cast<long long>(1));
    checkType<BDEC::Decimal32&>(d32 *= static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal32&>(d32 *= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal32&>(d32 *= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal32&>(d32 *= static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal32&>(d32 /= static_cast<char>(1));
    checkType<BDEC::Decimal32&>(d32 /= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal32&>(d32 /= static_cast<signed char>(1));
    checkType<BDEC::Decimal32&>(d32 /= static_cast<short>(1));
    checkType<BDEC::Decimal32&>(d32 /= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal32&>(d32 /= static_cast<int>(1));
    checkType<BDEC::Decimal32&>(d32 /= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal32&>(d32 /= static_cast<long>(1));
    checkType<BDEC::Decimal32&>(d32 /= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal32&>(d32 /= static_cast<long long>(1));
    checkType<BDEC::Decimal32&>(d32 /= static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal32&>(d32 /= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal32&>(d32 /= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal32&>(d32 /= static_cast<BDEC::Decimal128>(1));

    checkType<bool>(d32 == d32);
    checkType<bool>(d32 != d32);
    checkType<bool>(d32 < d32);
    checkType<bool>(d32 <= d32);
    checkType<bool>(d32 > d32);
    checkType<bool>(d32 >= d32);

    checkType<BDEC::Decimal32>(+d32);
    checkType<BDEC::Decimal32>(-d32);

    checkType<BDEC::Decimal32>(d32 + d32);
    checkType<BDEC::Decimal32>(d32 - d32);
    checkType<BDEC::Decimal32>(d32 * d32);
    checkType<BDEC::Decimal32>(d32 / d32);

    {
        bsl::istringstream  in(pa);
        bsl::ostringstream  out(pa);
        bsl::wistringstream win(pa);
        bsl::wostringstream wout(pa);

        checkType<bsl::istream&>(in >> d32);
        checkType<bsl::wistream&>(win >> d32);
        checkType<bsl::ostream&>(out << c32);
        checkType<bsl::wostream&>(wout << c32);
    }

    typedef bsl::numeric_limits<BDEC::Decimal32> d32_limits;
    checkType<bool>(d32_limits::is_specialized);
    checkType<BDEC::Decimal32>(d32_limits::min());
    checkType<BDEC::Decimal32>(d32_limits::max());
    checkType<int>(d32_limits::digits);
    checkType<int>(d32_limits::digits10);
    checkType<int>(d32_limits::max_digits10);
    checkType<bool>(d32_limits::is_signed);
    checkType<bool>(d32_limits::is_integer);
    checkType<bool>(d32_limits::is_exact);
    checkType<int>(d32_limits::radix);
    checkType<BDEC::Decimal32>(d32_limits::epsilon());
    checkType<BDEC::Decimal32>(d32_limits::round_error());
    checkType<int>(d32_limits::min_exponent);
    checkType<int>(d32_limits::min_exponent10);
    checkType<int>(d32_limits::max_exponent);
    checkType<int>(d32_limits::max_exponent10);
    checkType<bool>(d32_limits::has_infinity);
    checkType<bool>(d32_limits::has_quiet_NaN);
    checkType<bool>(d32_limits::has_signaling_NaN);
    checkType<bsl::float_denorm_style>(d32_limits::has_denorm);
    checkType<bool>(d32_limits::has_denorm_loss);
    checkType<BDEC::Decimal32>(d32_limits::infinity());
    checkType<BDEC::Decimal32>(d32_limits::quiet_NaN());
    checkType<BDEC::Decimal32>(d32_limits::signaling_NaN());
    checkType<BDEC::Decimal32>(d32_limits::denorm_min());
    checkType<bool>(d32_limits::is_iec559);
    checkType<bool>(d32_limits::is_bounded);
    checkType<bool>(d32_limits::is_modulo);
    checkType<bool>(d32_limits::traps);
    checkType<bool>(d32_limits::tinyness_before);
    checkType<bsl::float_round_style>(d32_limits::round_style);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
                   test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;  // always the last

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator  ta(veryVeryVeryVerbose);
    pa = &ta;

    cout.precision(35);

    switch (test) { case 0:
      case 10: {
        TestDriver::testCase10();
      } break;
      case 9: {
        TestDriver::testCase9();
      } break;
      case 8: {
        TestDriver::testCase8();
      } break;
      case 7: {
        TestDriver::testCase7();
      } break;
      case 6: {
        TestDriver::testCase6();
      } break;
      case 5: {
        TestDriver::testCase5();
      } break;
      case 4: {
        TestDriver::testCase4();
      } break;
      case 3: {
        TestDriver::testCase3();
      } break;
      case 2: {
        TestDriver::testCase2();
      } break;
      case 1: {
        TestDriver::testCase1();
      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: No memory came from the global allocator.

    LOOP2_ASSERT(test, globalAllocator.numBlocksTotal(),
                 0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
