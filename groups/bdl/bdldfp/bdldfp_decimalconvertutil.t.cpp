// bdldfp_decimalconvertutil.t.cpp                                    -*-C++-*-

#include <bdldfp_decimalconvertutil.h>

#include <bdls_testutil.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_iostream.h>
#include <bsl_iomanip.h>
#include <bsl_sstream.h>
#include <bsl_cstdlib.h>
#include <bsl_climits.h>
#include <bsl_limits.h>
#include <bsl_cmath.h>
#include <bsl_cfloat.h>
#include <bsl_cstring.h>
#include <bsl_algorithm.h>

#include <typeinfo>

using namespace BloombergLP;
using namespace BloombergLP::bdldfp;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// TBD:
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 3] Decimal64 decimal64FromMultiWidthEncoding(*buffer, size);
// [ 3] Decimal64 decimal64FromMultiWidthEncodingRaw(*buffer, size);
// [ 2] size_type decimal64ToMultiWidthEncoding(*buffer, decimal);
// [ 2] size_type decimal64ToMultiWidthEncodingRaw(*buffer, decimal);
// [ 4] unsigned char *decimal64FromVariableWidthEncoding(*decimal, *buffer);
// [ 4] unsigned char *decimal64ToVariableWidthEncoding(*buffer, value);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE
// ----------------------------------------------------------------------------


//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BDLS_TESTUTIL_ASSERT
#define LOOP_ASSERT  BDLS_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BDLS_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BDLS_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BDLS_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BDLS_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BDLS_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BDLS_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BDLS_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BDLS_TESTUTIL_ASSERTV

#define Q   BDLS_TESTUTIL_Q   // Quote identifier literally.
#define P   BDLS_TESTUTIL_P   // Print identifier and value.
#define P_  BDLS_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BDLS_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BDLS_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace BDEC = BloombergLP::bdldfp;

typedef bdldfp::DecimalConvertUtil Util;

#define PARSEDECIMAL(p, nn)                                                   \
        BDEC::Decimal##nn(BDEC::DecimalImpUtil::parse##nn(p))
#define PARSEDEC32(p) PARSEDECIMAL((p), 32)
#define PARSEDEC64(p) PARSEDECIMAL((p), 64)
#define PARSEDEC128(p) PARSEDECIMAL((p), 128)

struct DecBinTestCase {
    int         d_line;
    unsigned    d_decimalType;
    const char *d_decimalLiteral;
    long double d_ld;
    double      d_d;
    float       d_f;

                        // Decimal test case checking functions

    bool doD32() const
        // Return true, if this test case is for 'Decimal32', and false
        // otherwise.
    {
        return d_decimalType & 32;
    }

    bool doD64() const
        // Return true, if this test case is for 'Decimal64', and false
        // otherwise.
    {
        return d_decimalType & 64;
    }

    bool doD128() const
        // Return true, if this test case is for 'Decimal128', and false
        // otherwise.
    {
        return d_decimalType & 128;
    }

                        // Decmial construction functions

    BDEC::Decimal32 d32() const
        // Return a 'Decimal32' value for use in a test case, if applicable,
        // and '0' otherwise.
    {
        return doD32()?PARSEDEC32(d_decimalLiteral):BDEC::Decimal32(0);
    }

    BDEC::Decimal64 d64() const
        // Return a 'Decimal32' value for use in a test case, if applicable,
        // and '0' otherwise.
    {
        return doD64()?PARSEDEC64(d_decimalLiteral):BDEC::Decimal64(0);
    }

    BDEC::Decimal128 d128() const
        // Return a 'Decimal32' value for use in a test case, if applicable,
        // and '0' otherwise.
    {
        // workaround for IBM compiler bug
        typedef BDEC::DecimalImpUtil::ValueType128 Vt128;
        Vt128 x(BDEC::DecimalImpUtil::parse128(d_decimalLiteral));
        return doD128()?BDEC::Decimal128(x):BDEC::Decimal128(0);      // RETURN
        // END - workaround for IBM compiler bug

        // Restore this when IBM bugfix is in production
        return doD128()?PARSEDEC128(d_decimalLiteral):BDEC::Decimal128(0);
    }
};

static const float fInf = bsl::numeric_limits<float>::infinity();
static const float fNegInf = -fInf;

static const double dInf = bsl::numeric_limits<double>::infinity();
static const double dNegInf = -dInf;

static const long double lInf = bsl::numeric_limits<long double>::infinity();
static const long double lNegInf = -lInf;

static const DecBinTestCase DEC2BIN_DATA[] = {

    // Basic values

    { L_, 32|64|128,  "0",  0.0l,  0.0,  0.0f },
    { L_, 32|64|128, "-0", -0.0l, -0.0, -0.0f },
    { L_, 32|64|128,  "1",  1.0l,  1.0,  1.0f },
    { L_, 32|64|128, "-1", -1.0l, -1.0, -1.0f },

    // Large values (significant digits)

    { L_, 32|64|128,  "1234567.0",  1234567.0l,  1234567.0,  1234567.0f },
    { L_, 32|64|128, "-1234567.0", -1234567.0l, -1234567.0, -1234567.0f },

    { L_, 64|128,   "1234567890123456.0",
                     1234567890123456.0l,
                     1234567890123456.0,
                     1234567890123456.0f },
    { L_, 64|128,   "-1234567890123456.0",
                     -1234567890123456.0l,
                     -1234567890123456.0,
                     -1234567890123456.0f },

    { L_, 128,   "1234567890123456789012345678901234.0",
                 1234567890123456789012345678901234.0l,
                 1234567890123456789012345678901234.0,
                 1234567890123456789012345678901234.0f },
    { L_, 128,   "-1234567890123456789012345678901234.0",
                  -1234567890123456789012345678901234.0l,
                  -1234567890123456789012345678901234.0,
                  -1234567890123456789012345678901234.0f },

    // Fractions

    { L_, 32|64|128,  "0.1",  0.1l,  0.1,  0.1f },
    { L_, 32|64|128,  "0.1234567",  0.1234567l,  0.1234567,  0.1234567f },

    // Exponents

    { L_, 32|64|128,  "1.234567e96",  1.234567e96l,  1.234567e96, fInf    },
    { L_, 32|64|128, "-1.234567e96", -1.234567e96l, -1.234567e96, fNegInf },

    { L_, 32|64|128,  "1.234567e-95",  1.234567e-95l,  1.234567e-95, 0.0f },
    { L_, 32|64|128, "-1.234567e-95", -1.234567e-95l, -1.234567e-95, 0.0f },

    { L_, 64|128,  "1.234567890123456e385", lInf, dInf, fInf },
};

static const int DEC2BIN_DATA_COUNT =
                                sizeof(DEC2BIN_DATA) / sizeof(DEC2BIN_DATA[0]);

                        // Reverse Memory

static void memrev(void *buffer, size_t count)
    // Reverse the order of the first specified 'count' bytes, at the beginning
    // of the specified 'buffer'.  'count % 2' must be zero.
{
    unsigned char *b = static_cast<unsigned char *>(buffer);
    bsl::reverse(b, b + count);
}

                        // Mem copy with reversal functions

unsigned char *memReverseIfNeeded(void *buffer, size_t count)
    // Reverse the first specified 'count' bytes from the specified 'buffer`,
    // if the host endian is different from network endian, and return the
    // address computed from 'static_cast<unsigned char *>(buffer) + count'.
{
#ifdef BDLDFP_DECIMALPLATFORM_LITTLE_ENDIAN
    // little endian, needs to do some byte juggling
    memrev(buffer, count);
#endif
    return static_cast<unsigned char*>(buffer) + count;
}

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace UsageExample {
  // TBD
}  // close UsageExample namespace

//=============================================================================
//              GLOBAL HELPER FUNCTIONS AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------

                 // stringstream helpers - not thread safe!

void getStringFromStream(bsl::ostringstream &o, bsl::string  *out)
    // Set the specified 'out' string to the characters inserted into the
    // specified 'o' output stream.
{
    bslma::TestAllocator osa("osstream");
    bslma::DefaultAllocatorGuard g(&osa);
    *out = o.str();
}

void getStringFromStream(bsl::wostringstream &o, bsl::wstring *out)
    // Set the specified 'out' wide-string to the (wide) characters inserted
    // into the specified 'o' wide output stream.
{
    bslma::TestAllocator osa("osstream");
    bslma::DefaultAllocatorGuard g(&osa);
    *out = o.str();
}

 // String compare for decimal floating point numbers needs 'e'/'E' conversion

bsl::string& decLower(bsl::string& s)
    // Convert all 'E' characters to 'e' characters, in the specified 's'
    // string, and return a reference providing modifiable access.
{
    for (size_t i = 0; i < s.length(); ++i) if ('E' == s[i]) s[i] = 'e';
    return s;
}

bsl::wstring& decLower(bsl::wstring& s)
    // Convert all 'E' wide-characters to 'e' wide-characters, in the specified
    // 's' wide-string, and return a reference providing modifiable access.
{
    for (size_t i = 0; i < s.length(); ++i) if (L'E' == s[i]) s[i] = L'e';
    return s;
}

//-----------------------------------------------------------------------------

template <class EXPECT, class RECEIVED>
void checkType(const RECEIVED&)
    // Assert that the run-time type information for the specified 'EXPECT'
    // type matches that of the implicitly specified 'RECEIVED' type.
{
    ASSERT(typeid(EXPECT) == typeid(RECEIVED));
}

                          // Stream buffer helpers

template <int Size>
struct BufferBuf : bsl::streambuf {
    BufferBuf() { reset(); }
    void reset() { this->setp(this->d_buf, this->d_buf + Size); }
        // Overrides, and implements 'streambuf::reset'.
    const char *str() { *this->pptr() =0; return this->pbase(); }
        // Overrides, and implements 'streambuf::str'.
    char d_buf[Size + 1];
};

struct PtrInputBuf : bsl::streambuf {
    explicit PtrInputBuf(const char *s)
        // Construct a 'PtrInputBuf', from the specified 's' string.
    {
        char *x = const_cast<char *>(s);
        this->setg(x, x, x + strlen(x));
    }
};

struct NulBuf : bsl::streambuf {
    char d_dummy[64];
    virtual int overflow(int c)
        // Overrides, and implements 'streambuf::overflow', passing the
        // specified 'c'.
    {
        setp( d_dummy, d_dummy + sizeof(d_dummy));
        return traits_type::not_eof(c);
    }
};

//-----------------------------------------------------------------------------

BSLMF_ASSERT(sizeof(float) == sizeof(int));
int mantissaBits(float f)
    // Return, as a binary integer, the significand bits from the binary
    // floating point value specified by 'f'.  Note that sign is ignored.
{
    union {
        float    as_float;
        unsigned as_int;
    } x;
    x.as_float = f;
    return x.as_int & 0x7fffff;
}

BSLMF_ASSERT(sizeof(double) == sizeof(long long));
long long mantissaBits(double d)
    // Return, as a binary integer, the significand bits from the double
    // precision binary floating point value specified by 'd'.  Note that sign
    // is ignored.
{
    union {
        double                 as_double;
        unsigned long long int as_int;
    } x;
    x.as_double = d;
    return x.as_int & 0xfffffffffffffull;
}

struct Mantissa128 {
#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
    long long hi;
#endif
    long long lo;
#ifndef BSLS_PLATFORM_IS_BIG_ENDIAN
    long long hi;
#endif
};

                            // Strict comparators

template <class DECIMAL_TYPE>
bool strictEqual(DECIMAL_TYPE lhs, DECIMAL_TYPE rhs)
    // Return true if the bit pattern in the specified 'lhs' decimal type
    // matches that in the specified 'rhs' decimal type.
{
    const void *blhs = &lhs;
    const void *brhs = &rhs;
    return bsl::memcmp(blhs, brhs, sizeof(DECIMAL_TYPE)) == 0;
}

void bufferToStream(bsl::ostream           &out,
                    unsigned char          *buffer,
                    bsls::Types::size_type  size)
{
    out << hex;
    for (bsls::Types::size_type b = 0; b < size; ++b) {
        out << setfill('0') << setw(2)
            << static_cast<int>(buffer[b]);
        if (b != size - 1) {
            out<< " ";
        }
    }
    out << dec;
}

unsigned char * decimal64ToBinaryIntegralNetwork(unsigned char *buffer,
                                                 Decimal64 decimal)
{
    bsls::Types::Uint64 encoded;
    Util::decimal64ToBID(reinterpret_cast<unsigned char *>(&encoded), decimal);

    encoded = BSLS_BYTEORDER_HTONLL(encoded);

    bsl::memcpy(buffer, reinterpret_cast<unsigned char*>(&encoded), 8);
    return buffer + 8;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    int                test = argc > 1 ? atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4;

    bslma::TestAllocator defaultAllocator("default");
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global");
    bslma::Default::setGlobalAllocator(&globalAllocator);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;
    typedef BDEC::DecimalConvertUtil Util;

    cout.precision(35);

    switch (test) { case 0:
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        if (veryVerbose) bsl::cout << "\nSending decimals as octets "
                             "using network format" << bsl::endl;
        // Suppose you have two communicating entities (programs) that talk to
        // each other using a binary (as opposed to text) protocol.  In such
        // protocol it is important to establish a so-called network format,
        // and convert to and from that format in the protocol layer.  The
        // sender (suppose that it is an IBM server that has just finished an
        // expensive calculation involving millions of numbers and needs to
        // send the result to its client) will need to convert the data to
        // network format before sending:
        //..
        { // "server"
            unsigned char   msgbuffer[256];
            BDEC::Decimal64 number(BDLDFP_DECIMAL_DD(1.234567890123456e-42));
            unsigned char   expected[] = {
                0x25, 0x55, 0x34, 0xb9, 0xc1, 0xe2, 0x8e, 0x56 };

            unsigned char *next = msgbuffer;
            next = bdldfp::DecimalConvertUtil::decimalToNetwork(next, number);

            ASSERT(bsl::memcmp(msgbuffer, expected, sizeof(number)) == 0);
        }
        //..
        // The receiver/client shall then restore the number from network
        // format:
        //..
        { // "client"
            const unsigned char   msgbuffer[] ={
                              0x25, 0x55, 0x34, 0xb9, 0xc1, 0xe2, 0x8e, 0x56 };
            BDEC::Decimal64 number;
            BDEC::Decimal64 expected(BDLDFP_DECIMAL_DD(1.234567890123456e-42));

            const unsigned char *next = msgbuffer;
            next = bdldfp::DecimalConvertUtil::decimalFromNetwork(&number,
                                                                  next);

            ASSERT(next == msgbuffer + sizeof(BDEC::Decimal64));
            ASSERT(number == expected);
        }
        //..

        if (veryVerbose) bsl::cout << "\nStoring/sending decimals in binary"
                             " floating-point" << bsl::endl;
        // Suppose you have two communicating entities (programs) that talk to
        // each other using a legacy protocol that employs binary
        // floating-point formats to send/receive numbers.  So your application
        // layer will have to store the decimal into a binary FP variable,
        // ensure that it can be restored (in other words that it has "fit"
        // into the binary type) when sending, and restore the decimal number
        // (from the binary type) when receiving:
        //..
        {
            const BDEC::Decimal64 number(
                BDLDFP_DECIMAL_DD(1.23456789012345e-42));

            double dbl = Util::decimalToDouble(number);

            if (Util::decimal64FromDouble(dbl) != number) {
                // Do what is appropriate for the application
            }
            //..
            // The receiver would then restore the number using the appropriate
            // 'decimal64FromDouble' function:
            //..
            BDEC::Decimal64 restored = Util::decimal64FromDouble(dbl);

            ASSERT(number == restored);
        }
        //..
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // VARIABLE-WIDTH ENCODE AND DECODE
        //
        // Concerns:
        //: 1 'decimal64ToVariableWidthEncoding' correctly encodes values in
        //:   the supported formats.
        //:
        //: 2 Values are always encoded in the smallest supported
        //:   format. E.g. a values that fits in both 2 byte and 3 byte
        //:   encodings will be encoded using the 2 byte encoding.
        //:
        //: 3 'decimal64ToVariableWidthEncoding' does not overwrite any memory
        //:   outside of the range that it is supposed to.
        //:
        //: 4 'decimal64FromMultiWidthEncoding' can decode any value encoded
        //:    using 'decimal64ToVariableWidthEncoding'.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of decimal
        //:   values, and their expected encoded values when using the
        //:   variable-width encoding format.  Ensure that the set of values
        //:   include bondary values in all supported widths of the
        //:   encoder. Use 'decimal64ToVariableWidthEncoding' to encode each
        //:   decimal value in the set. Verify that the encoded values matches
        //:   the expected values.  Additional, verify the encoded values can
        //:   be decoded back to the original decimal values using
        //:   'decimal64FromVaribleWidthEncoding'.  (C-1..3)
        //
        // Testing:
        //   unsigned char *decimal64FromVariableWidthEncoding(*decimal, *buffer);
        //   unsigned char *decimal64ToVariableWidthEncoding(*buffer, value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VARIABLE-WIDTH ENCODE AND DECODE" << endl
                          << "================================" << endl;

        Decimal64 (*MDF)(long long, int) = &DecimalUtil::makeDecimal64;

        static const struct {
            int        d_line;
            Decimal64  d_decodedValue;
            const char d_encodedValue[128];
        } DATA[] = {
            { L_,  MDF(                  0,     0 ),  "40 00" },
            { L_,  MDF(                  1,    -2 ),  "00 01" },
            { L_,  MDF(                  1,    -1 ),  "20 01" },
            { L_,  MDF(                  1,     0 ),  "40 01" },
            { L_,  MDF(                  1,     1 ),  "60 01" },
            { L_,  MDF(      (1 << 13) - 1,     1 ),  "7f ff" },

            { L_,  MDF(                  0,     2 ),  "b0 00 00" },
            { L_,  MDF(                  0,    -3 ),  "88 00 00" },
            { L_,  MDF(                 -1,     3 ),  "f3 00 00 01" },
            { L_,  MDF(          (1 << 13),     3 ),  "b8 20 00" },

            { L_,  MDF(          (1 << 13),     0 ),  "a0 20 00" },
            { L_,  MDF(          (1 << 13),    -4 ),  "80 20 00" },
            { L_,  MDF(      (1 << 13) + 1,     3 ),  "b8 20 01" },
            { L_,  MDF(      (1 << 19) - 1,     3 ),  "bf ff ff" },

            { L_,  MDF(          (1 << 13),     4 ),  "d4 00 20 00" },
            { L_,  MDF(          (1 << 13),    -5 ),  "cb 00 20 00" },
            { L_,  MDF(          (1 << 19),     3 ),  "d3 08 00 00" },
            { L_,  MDF(         -(1 << 19),     3 ),  "f3 08 00 00" },
            { L_,  MDF(      (1 << 24) - 1,    15 ),  "df ff ff ff" },
            { L_,  MDF(      (1 << 24) - 1,   -16 ),  "c0 ff ff ff" },
            { L_,  MDF(   -((1 << 24) - 1),    14 ),  "fe ff ff ff" },
            { L_,  MDF(   -((1 << 24) - 1),   -16 ),  "e0 ff ff ff" },

            // Encoded value == "" indicates that the full 9-bytes encoding
            // format should be used.

            { L_,  MDF(   -((1 << 24) - 1),    15 ),  "" },
            { L_,  MDF(      (1 << 24) - 1,    16 ),  "" },
            { L_,  MDF(            1 << 24,     0 ),  "" },
            { L_,  MDF(         -(1 << 24),     0 ),  "" },

            { L_,        bsl::numeric_limits<Decimal64>::min(), "" },
            { L_,        bsl::numeric_limits<Decimal64>::max(), "" },
            { L_,   bsl::numeric_limits<Decimal64>::infinity(), "" },
            { L_,  bsl::numeric_limits<Decimal64>::quiet_NaN(), "" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i != NUM_DATA; ++i) {
            const int        LINE          = DATA[i].d_line;
            const Decimal64  DECODED_VALUE = DATA[i].d_decodedValue;
            const char      *ENCODED_VALUE = DATA[i].d_encodedValue;

            unsigned char encodedBuffer[12];

            bsls::Types::size_type encodedSize = 0;

            if (ENCODED_VALUE[0]) {
                istringstream encodedBufferString(ENCODED_VALUE);
                int temp;
                while (encodedBufferString >> hex >> temp) {
                    encodedBuffer[encodedSize] =
                                              static_cast<unsigned char>(temp);
                    ++encodedSize;
                }
            }
            else {
                encodedBuffer[0] = 0xFF;
                unsigned char *nextAdr = decimal64ToBinaryIntegralNetwork(
                                             encodedBuffer + 1, DECODED_VALUE);
                encodedSize = nextAdr - encodedBuffer;
            }

            if (veryVerbose) {
                P_(LINE) P_(DECODED_VALUE) P_(encodedSize);
                cout << "ENCODED_VALUE: ";
                bufferToStream(cout, encodedBuffer, encodedSize);
                cout << endl;
            }


            unsigned char actualEncodedBufferOrig[16] = {
                                                      0xDE, 0xAD, 0xBE, 0xEF,
                                                      0xDE, 0xAD, 0xBE, 0xEF,
                                                      0xDE, 0xAD, 0xBE, 0xEF,
                                                      0xDE, 0xAD, 0xBE, 0xEF };
            unsigned char actualEncodedBuffer[16] = {
                                                      0xDE, 0xAD, 0xBE, 0xEF,
                                                      0xDE, 0xAD, 0xBE, 0xEF,
                                                      0xDE, 0xAD, 0xBE, 0xEF,
                                                      0xDE, 0xAD, 0xBE, 0xEF };
            const bsls::Types::size_type actualEncodedBufferOffset = 4;
            bsls::Types::size_type actualEncodedSize;

            actualEncodedSize = Util::decimal64ToVariableWidthEncoding(
                actualEncodedBuffer + actualEncodedBufferOffset,
                DECODED_VALUE)
                - (actualEncodedBuffer + actualEncodedBufferOffset);

            if (veryVerbose) {
                P_(actualEncodedSize);
                cout << "actualEncodeValue: ";
                bufferToStream(cout,
                               actualEncodedBuffer + actualEncodedBufferOffset,
                               actualEncodedSize);
                cout << endl;
            }

            ASSERTV(LINE, encodedSize, actualEncodedSize,
                    encodedSize == actualEncodedSize);
            ASSERTV(LINE, 0 == bsl::memcmp(
                               encodedBuffer,
                               actualEncodedBuffer + actualEncodedBufferOffset,
                               encodedSize));
            ASSERTV(LINE, 0 == bsl::memcmp(actualEncodedBuffer,
                                           actualEncodedBufferOrig,
                                           actualEncodedBufferOffset));
            ASSERTV(LINE, 0 == bsl::memcmp(
             actualEncodedBuffer + actualEncodedBufferOffset + encodedSize,
             actualEncodedBufferOrig + actualEncodedBufferOffset + encodedSize,
             sizeof(actualEncodedBuffer) - encodedSize
                                         - actualEncodedBufferOffset));

            Decimal64 actualDecodedValue = MDF(123,123);  // initialize to
                                                          // unused value

            const unsigned char *actualRet =
                Util::decimal64FromVariableWidthEncoding(
                              &actualDecodedValue,
                              const_cast<const unsigned char*>(encodedBuffer));

            bsls::Types::size_type actualSize = actualRet - encodedBuffer;

            if (veryVerbose) {
                P_(actualDecodedValue); P(actualSize);
            }

            if (DecimalUtil::isNan(DECODED_VALUE)) {
                ASSERTV(LINE, DECODED_VALUE, actualDecodedValue,
                        DecimalUtil::isNan(actualDecodedValue));
            }
            else {
                ASSERTV(LINE, DECODED_VALUE, actualDecodedValue,
                        DECODED_VALUE == actualDecodedValue);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // MULTI-WIDTH DECODE
        //
        // Concerns:
        //: 1 'decimal64FromMultiWidthEncoding' and
        //:   'decimal64FromMultiWidthEncodingRaw' correctly decode values in
        //:   the supported formats.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of decimal
        //:   values, and their encoded values. Ensure that the set contains
        //:   boundary values in all supported widths of the decoder. Use
        //:   'decimal64FromMultiWidthEncoding' and
        //:   'decimal64FromMultiWidthEncodingRaw' to decode the encoded
        //:   values. Verify that the decoded decimal values matches the
        //:   original values. (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-2)
        //
        // Testing:
        //   Decimal64 decimal64FromMultiWidthEncoding(*buffer, size);
        //   Decimal64 decimal64FromMultiWidthEncodingRaw(*buffer, size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MULTI-WIDTH DECODE" << endl
                          << "==================" << endl;

        Decimal64 (*MDF)(long long, int) = &DecimalUtil::makeDecimal64;

        static const struct {
            int        d_line;
            Decimal64  d_decodedValue;
            const char d_encodedValue[128];
        } DATA[] = {
            { L_,  MDF(                  0,    -2 ),  "00" },
            { L_,  MDF(                  0,    -1 ),  "80" },
            { L_,  MDF(                 50,    -2 ),  "32" },
            { L_,  MDF(                 50,    -1 ),  "b2" },
            { L_,  MDF(       (1 << 7) - 1,    -2 ),  "7f" },
            { L_,  MDF(       (1 << 7) - 1,    -1 ),  "ff" },

            { L_,  MDF(                  0,    -1 ),  "80 00" },
            { L_,  MDF(                  1,    -3 ),  "00 01" },
            { L_,  MDF(                  1,    -2 ),  "40 01" },
            { L_,  MDF(                  1,    -1 ),  "80 01" },
            { L_,  MDF(                  1,     0 ),  "c0 01" },
            { L_,  MDF(      (1 << 14) - 1,     0 ),  "ff ff" },

            { L_,  MDF(                  0,     1 ),  "e0 00 00" },
            { L_,  MDF(                  0,    -4 ),  "40 00 00" },
            { L_,  MDF(                 -1,     1 ),  "c4 00 00 01" },
            { L_,  MDF(          (1 << 14),     1 ),  "e0 40 00" },

            { L_,  MDF(          (1 << 14),    -2 ),  "80 40 00" },
            { L_,  MDF(          (1 << 14),    -6 ),  "00 40 00" },
            { L_,  MDF(          (1 << 14),     1 ),  "e0 40 00" },
            { L_,  MDF(      (1 << 21) - 1,     1 ),  "ff ff ff" },

            { L_,  MDF(          (1 << 14),     2 ),  "48 00 40 00" },
            { L_,  MDF(          (1 << 14),    -7 ),  "24 00 40 00" },
            { L_,  MDF(          (1 << 21),     2 ),  "48 20 00 00" },

            { L_,  MDF(          (1 << 21),     0 ),  "40 20 00 00" },
            { L_,  MDF(          (1 << 21),   -16 ),  "00 20 00 00" },
            { L_,  MDF(          (1 << 21),    15 ),  "7c 20 00 00" },
            { L_,  MDF(         -(1 << 21),     0 ),  "c0 20 00 00" },
            { L_,  MDF(         -(1 << 21),   -16 ),  "80 20 00 00" },
            { L_,  MDF(         -(1 << 21),    15 ),  "fc 20 00 00" },
            { L_,  MDF(  ((1ll << 26) - 1),    15 ),  "7f ff ff ff" },
            { L_,  MDF( -((1ll << 26) - 1),    15 ),  "ff ff ff ff" },

            { L_,  MDF(        (1ll << 26),     0 ),  "40 04 00 00 00" },
            { L_,  MDF(        (1ll << 26),   -16 ),  "00 04 00 00 00" },
            { L_,  MDF(        (1ll << 26),    15 ),  "7c 04 00 00 00" },
            { L_,  MDF(       -(1ll << 26),     0 ),  "c0 04 00 00 00" },
            { L_,  MDF(       -(1ll << 26),   -16 ),  "80 04 00 00 00" },
            { L_,  MDF(       -(1ll << 26),    15 ),  "fc 04 00 00 00" },
            { L_,  MDF(  ((1ll << 34) - 1),    15 ),  "7f ff ff ff ff" },
            { L_,  MDF( -((1ll << 34) - 1),    15 ),  "ff ff ff ff ff" },

            // Encoded value == "" indicates that the full 8-bytes DPD encoding
            // format should be used.

            { L_,  MDF(        (1ll << 34),    15 ),  "" },
            { L_,  MDF(       -(1ll << 34),    15 ),  "" },
            { L_,  MDF(                  1,    16 ),  "" },
            { L_,  MDF(                  1,   -17 ),  "" },
            { L_,  MDF(                  1,  -398 ),  "" },

            { L_,        bsl::numeric_limits<Decimal64>::min(), "" },
            { L_,        bsl::numeric_limits<Decimal64>::max(), "" },
            { L_,   bsl::numeric_limits<Decimal64>::infinity(), "" },
            { L_,  bsl::numeric_limits<Decimal64>::quiet_NaN(), "" },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i != NUM_DATA; ++i) {
            const int        LINE          = DATA[i].d_line;
            const Decimal64  DECODED_VALUE = DATA[i].d_decodedValue;
            const char      *ENCODED_VALUE = DATA[i].d_encodedValue;

            unsigned char encodedBuffer[12];
            bsls::Types::size_type encodedSize = 0;

            const bool useFullEncodingFlag = !(ENCODED_VALUE[0]);
            if (!useFullEncodingFlag) {
                istringstream encodedBufferString(ENCODED_VALUE);
                int temp;
                while (encodedBufferString >> hex >> temp) {
                    encodedBuffer[encodedSize] =
                                              static_cast<unsigned char>(temp);
                    ++encodedSize;
                }
            }
            else {
                unsigned char *nextAdr = decimal64ToBinaryIntegralNetwork(
                                                 encodedBuffer, DECODED_VALUE);
                encodedSize = nextAdr - encodedBuffer;
            }

            if (veryVerbose) {
                P_(LINE) P_(DECODED_VALUE)
                cout << "ENCODED_VALUE: ";
                bufferToStream(cout, encodedBuffer, encodedSize);
                cout << endl;
            }

            // Test 'decimal64FromMultiWidthEncoding'.
            {
                Decimal64 actualDecodedValue =
                    Util::decimal64FromMultiWidthEncoding(
                               const_cast<const unsigned char*>(encodedBuffer),
                               encodedSize);
                if (veryVerbose) {
                    P(actualDecodedValue);
                }

                if (DecimalUtil::isNan(DECODED_VALUE)) {
                    ASSERTV(LINE, DECODED_VALUE, actualDecodedValue,
                            DecimalUtil::isNan(actualDecodedValue));
                }
                else {
                    ASSERTV(LINE, DECODED_VALUE, actualDecodedValue,
                            DECODED_VALUE == actualDecodedValue);
                }
            }

            // Test 'decimal64FromMultiWidthEncodingRaw'.
            if (!useFullEncodingFlag) {
                Decimal64 actualDecodedValue =
                    Util::decimal64FromMultiWidthEncodingRaw(
                               const_cast<const unsigned char*>(encodedBuffer),
                               encodedSize);
                if (veryVerbose) {
                    P(actualDecodedValue);
                }

                if (DecimalUtil::isNan(DECODED_VALUE)) {
                    ASSERTV(LINE, DECODED_VALUE, actualDecodedValue,
                            DecimalUtil::isNan(actualDecodedValue));
                }
                else {
                    ASSERTV(LINE, DECODED_VALUE, actualDecodedValue,
                            DECODED_VALUE == actualDecodedValue);
                }
            }
        }

        // Negative Testing
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            unsigned char encodedBuffer[9] = { 0x77, 0xfc, 0xff, 0x3f,
                                               0xcf, 0xf3, 0xfc, 0xff,
                                               0xff };

            ASSERT_SAFE_FAIL(
                      Util::decimal64FromMultiWidthEncoding(encodedBuffer, 0));
            ASSERT_SAFE_PASS(
                      Util::decimal64FromMultiWidthEncoding(encodedBuffer, 1));
            ASSERT_SAFE_PASS(
                      Util::decimal64FromMultiWidthEncoding(encodedBuffer, 2));
            ASSERT_SAFE_PASS(
                      Util::decimal64FromMultiWidthEncoding(encodedBuffer, 3));
            ASSERT_SAFE_PASS(
                      Util::decimal64FromMultiWidthEncoding(encodedBuffer, 4));
            ASSERT_SAFE_PASS(
                      Util::decimal64FromMultiWidthEncoding(encodedBuffer, 5));
            ASSERT_SAFE_FAIL(
                      Util::decimal64FromMultiWidthEncoding(encodedBuffer, 6));
            ASSERT_SAFE_FAIL(
                      Util::decimal64FromMultiWidthEncoding(encodedBuffer, 7));
            ASSERT_SAFE_PASS(
                      Util::decimal64FromMultiWidthEncoding(encodedBuffer, 8));
            ASSERT_SAFE_FAIL(
                      Util::decimal64FromMultiWidthEncoding(encodedBuffer, 9));

            ASSERT_SAFE_FAIL(
                   Util::decimal64FromMultiWidthEncodingRaw(encodedBuffer, 0));
            ASSERT_SAFE_PASS(
                   Util::decimal64FromMultiWidthEncodingRaw(encodedBuffer, 1));
            ASSERT_SAFE_PASS(
                   Util::decimal64FromMultiWidthEncodingRaw(encodedBuffer, 2));
            ASSERT_SAFE_PASS(
                   Util::decimal64FromMultiWidthEncodingRaw(encodedBuffer, 3));
            ASSERT_SAFE_PASS(
                   Util::decimal64FromMultiWidthEncodingRaw(encodedBuffer, 4));
            ASSERT_SAFE_PASS(
                   Util::decimal64FromMultiWidthEncodingRaw(encodedBuffer, 5));
            ASSERT_SAFE_FAIL(
                   Util::decimal64FromMultiWidthEncodingRaw(encodedBuffer, 6));
            ASSERT_SAFE_FAIL(
                   Util::decimal64FromMultiWidthEncodingRaw(encodedBuffer, 7));
            ASSERT_SAFE_FAIL(
                   Util::decimal64FromMultiWidthEncodingRaw(encodedBuffer, 8));
            ASSERT_SAFE_FAIL(
                   Util::decimal64FromMultiWidthEncodingRaw(encodedBuffer, 9));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // MULTI-WIDTH ENCODE
        //
        // Concerns:
        //: 1 'decimal64ToMultiWidthEncoding' and
        //:   'decimal64ToMultiWidthEncodingRaw' correctly encode values in the
        //:   supported formats.
        //:
        //: 2 Values are always encoded in the smallest supported
        //:   format. E.g. a values that fits in both 2 byte and 3 byte
        //:   encodings will be encoded using the 2 byte encoding.
        //:
        //: 3 'decimal64ToMultiWidthEncoding' and
        //:   'decimal64ToMultiWidthEncodingRaw' do not overwrite any memory
        //:   outside of the range that it is supposed to.
        //:
        //: 4 Any value encoded using 'decimal64ToMultiWidthEncoding' can be
        //:   decoded using the function 'decimal64FromMultiWidthEncoding'.
        //:
        //: 5 Any value encoded using 'decimal64ToMultiWidthEncodingRaw' can be
        //:   decoded using the function 'decimal64FromMultiWidthEncodingRaw'.
        //:
        //: 6 'decimal64ToMultiWidthEncodingRaw' returns 0 if the value to
        //:   encode requires the full IEEE format.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of decimal
        //:   values, and their expected encoded values when using the
        //:   multi-width encoding format.  Ensure that the set of values
        //:   include bondary values in all supported widths of the
        //:   encoder. Use 'decimal64ToMultiWidthEncoding' and
        //:   'decimal64ToMultiWidthEncodingRaw' to encode each decimal value
        //:   in the set. Verify that the encoded values matches the expected
        //:   values.  Additional, verify the encoded values can be decoded
        //:   back to the original decimal values using
        //:   'decimal64FromMultiWidthEncoding' and
        //:   'decimal64FromMultiWidthEncodingRaw'.  (C-1..6)
        //
        // Testing:
        //   size_type decimal64ToMultiWidthEncoding(*buffer, decimal);
        //   size_type decimal64ToMultiWidthEncodingRaw(*buffer, decimal);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MULTI-WIDTH ENCODE" << endl
                          << "==================" << endl;

        Decimal64 (*MDF)(long long, int) = &DecimalUtil::makeDecimal64;

        static const struct {
            int        d_line;
            Decimal64  d_decodedValue;
            const char d_encodedValue[128];
        } DATA[] = {
            { L_,  MDF(                  0,    -1 ),  "80 00" },
            { L_,  MDF(                  1,    -3 ),  "00 01" },
            { L_,  MDF(                  1,    -2 ),  "40 01" },
            { L_,  MDF(                  1,    -1 ),  "80 01" },
            { L_,  MDF(                  1,     0 ),  "c0 01" },
            { L_,  MDF(      (1 << 14) - 1,     0 ),  "ff ff" },

            { L_,  MDF(                  0,     1 ),  "e0 00 00" },
            { L_,  MDF(                  0,    -4 ),  "40 00 00" },
            { L_,  MDF(                 -1,     1 ),  "c4 00 00 01" },
            { L_,  MDF(          (1 << 14),     1 ),  "e0 40 00" },

            { L_,  MDF(          (1 << 14),    -2 ),  "80 40 00" },
            { L_,  MDF(          (1 << 14),    -6 ),  "00 40 00" },
            { L_,  MDF(          (1 << 14),     1 ),  "e0 40 00" },
            { L_,  MDF(      (1 << 21) - 1,     1 ),  "ff ff ff" },

            { L_,  MDF(          (1 << 14),     2 ),  "48 00 40 00" },
            { L_,  MDF(          (1 << 14),    -7 ),  "24 00 40 00" },
            { L_,  MDF(          (1 << 21),     2 ),  "48 20 00 00" },

            { L_,  MDF(          (1 << 21),     0 ),  "40 20 00 00" },
            { L_,  MDF(          (1 << 21),   -16 ),  "00 20 00 00" },
            { L_,  MDF(          (1 << 21),    15 ),  "7c 20 00 00" },
            { L_,  MDF(         -(1 << 21),     0 ),  "c0 20 00 00" },
            { L_,  MDF(         -(1 << 21),   -16 ),  "80 20 00 00" },
            { L_,  MDF(         -(1 << 21),    15 ),  "fc 20 00 00" },
            { L_,  MDF(  ((1ll << 26) - 1),    15 ),  "7f ff ff ff" },
            { L_,  MDF( -((1ll << 26) - 1),    15 ),  "ff ff ff ff" },

            { L_,  MDF(        (1ll << 26),     0 ),  "40 04 00 00 00" },
            { L_,  MDF(        (1ll << 26),   -16 ),  "00 04 00 00 00" },
            { L_,  MDF(        (1ll << 26),    15 ),  "7c 04 00 00 00" },
            { L_,  MDF(       -(1ll << 26),     0 ),  "c0 04 00 00 00" },
            { L_,  MDF(       -(1ll << 26),   -16 ),  "80 04 00 00 00" },
            { L_,  MDF(       -(1ll << 26),    15 ),  "fc 04 00 00 00" },
            { L_,  MDF(  ((1ll << 34) - 1),    15 ),  "7f ff ff ff ff" },
            { L_,  MDF( -((1ll << 34) - 1),    15 ),  "ff ff ff ff ff" },

            // Encoded value == "" indicates that the full 8-bytes DPD encoding
            // format should be used.

            { L_,  MDF(        (1ll << 34),    15 ),  "" },
            { L_,  MDF(       -(1ll << 34),    15 ),  "" },
            { L_,  MDF(                  1,    16 ),  "" },
            { L_,  MDF(                  1,   -17 ),  "" },
            { L_,  MDF(                  1,  -398 ),  "" },

            { L_,        bsl::numeric_limits<Decimal64>::min(), "" },
            { L_,        bsl::numeric_limits<Decimal64>::max(), "" },
            { L_,   bsl::numeric_limits<Decimal64>::infinity(), "" },
            { L_,  bsl::numeric_limits<Decimal64>::quiet_NaN(), "" },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i != NUM_DATA; ++i) {
            const int        LINE          = DATA[i].d_line;
            const Decimal64  DECODED_VALUE = DATA[i].d_decodedValue;
            const char      *ENCODED_VALUE = DATA[i].d_encodedValue;

            unsigned char encodedBuffer[12];
            bsls::Types::size_type encodedSize = 0;

            const bool useFullEncodingFlag = !(ENCODED_VALUE[0]);

            if (!useFullEncodingFlag) {
                istringstream encodedBufferString(ENCODED_VALUE);
                int temp;
                while (encodedBufferString >> hex >> temp) {
                    encodedBuffer[encodedSize] =
                                              static_cast<unsigned char>(temp);
                    ++encodedSize;
                }
            }
            else {
                unsigned char *nextAdr = decimal64ToBinaryIntegralNetwork(
                                                 encodedBuffer, DECODED_VALUE);
                encodedSize = nextAdr - encodedBuffer;
            }

            if (veryVerbose) {
                P_(LINE) P_(DECODED_VALUE)
                cout << "ENCODED_VALUE: ";
                bufferToStream(cout, encodedBuffer, encodedSize);
                cout << endl;
            }

            // Test 'decimal64ToMultiWidthEncoding'.

            {
                unsigned char actualEncodedBufferOrig[16] = {
                    0xDE, 0xAD, 0xBE, 0xEF,
                    0xDE, 0xAD, 0xBE, 0xEF,
                    0xDE, 0xAD, 0xBE, 0xEF,
                    0xDE, 0xAD, 0xBE, 0xEF };
                unsigned char actualEncodedBuffer[16] = {
                    0xDE, 0xAD, 0xBE, 0xEF,
                    0xDE, 0xAD, 0xBE, 0xEF,
                    0xDE, 0xAD, 0xBE, 0xEF,
                    0xDE, 0xAD, 0xBE, 0xEF };
                const bsls::Types::size_type actualEncodedBufferOffset = 4;
                bsls::Types::size_type actualEncodedSize;

                actualEncodedSize = Util::decimal64ToMultiWidthEncoding(
                    actualEncodedBuffer + actualEncodedBufferOffset,
                    DECODED_VALUE);

                if (veryVerbose) {
                    P_(actualEncodedSize);
                    cout << "actualEncodeValue: ";
                    bufferToStream(
                               cout,
                               actualEncodedBuffer + actualEncodedBufferOffset,
                               actualEncodedSize);
                    cout << endl;
                }

                ASSERTV(LINE, encodedSize, actualEncodedSize,
                        encodedSize == actualEncodedSize);
                ASSERTV(LINE, 0 == bsl::memcmp(
                               encodedBuffer,
                               actualEncodedBuffer + actualEncodedBufferOffset,
                               encodedSize));
                ASSERTV(LINE, 0 == bsl::memcmp(actualEncodedBuffer,
                                               actualEncodedBufferOrig,
                                               actualEncodedBufferOffset));
                ASSERTV(LINE, 0 == bsl::memcmp(
                                      actualEncodedBuffer +
                                      actualEncodedBufferOffset + encodedSize,
                                      actualEncodedBufferOrig +
                                      actualEncodedBufferOffset + encodedSize,
                                      sizeof(actualEncodedBuffer) - encodedSize
                                      - actualEncodedBufferOffset));

                Decimal64 actualDecodedValue =
                    Util::decimal64FromMultiWidthEncoding(encodedBuffer,
                                                          encodedSize);
                if (veryVerbose) {
                    P(actualDecodedValue);
                }

                if (DecimalUtil::isNan(DECODED_VALUE)) {
                    ASSERTV(LINE, DECODED_VALUE, actualDecodedValue,
                            DecimalUtil::isNan(actualDecodedValue));
                }
                else {
                    ASSERTV(LINE, DECODED_VALUE, actualDecodedValue,
                            DECODED_VALUE == actualDecodedValue);
                }
            }

            // Test 'decimal64ToMultiWidthEncodingRaw'.

            if (useFullEncodingFlag) {
                unsigned char buffer[16];
                bsls::Types::size_type ret =
                    Util::decimal64ToMultiWidthEncodingRaw(buffer,
                                                           DECODED_VALUE);
                ASSERTV(LINE, ret, ret == 0);
            }
            else {
                unsigned char actualEncodedBufferOrig[16] = {
                    0xDE, 0xAD, 0xBE, 0xEF,
                    0xDE, 0xAD, 0xBE, 0xEF,
                    0xDE, 0xAD, 0xBE, 0xEF,
                    0xDE, 0xAD, 0xBE, 0xEF };
                unsigned char actualEncodedBuffer[16] = {
                    0xDE, 0xAD, 0xBE, 0xEF,
                    0xDE, 0xAD, 0xBE, 0xEF,
                    0xDE, 0xAD, 0xBE, 0xEF,
                    0xDE, 0xAD, 0xBE, 0xEF };
                const bsls::Types::size_type actualEncodedBufferOffset = 4;
                bsls::Types::size_type actualEncodedSize;

                actualEncodedSize = Util::decimal64ToMultiWidthEncodingRaw(
                               actualEncodedBuffer + actualEncodedBufferOffset,
                               DECODED_VALUE);

                if (veryVerbose) {
                    P_(actualEncodedSize);
                    cout << "actualEncodeValue: ";
                    bufferToStream(
                               cout,
                               actualEncodedBuffer + actualEncodedBufferOffset,
                               actualEncodedSize);
                    cout << endl;
                }

                ASSERTV(LINE, encodedSize, actualEncodedSize,
                        encodedSize == actualEncodedSize);
                ASSERTV(LINE, 0 == bsl::memcmp(
                            encodedBuffer,
                            actualEncodedBuffer + actualEncodedBufferOffset,
                            encodedSize));
                ASSERTV(LINE, 0 == bsl::memcmp(actualEncodedBuffer,
                                               actualEncodedBufferOrig,
                                               actualEncodedBufferOffset));
                ASSERTV(LINE, 0 == bsl::memcmp(
                                      actualEncodedBuffer +
                                      actualEncodedBufferOffset + encodedSize,
                                      actualEncodedBufferOrig +
                                      actualEncodedBufferOffset + encodedSize,
                                      sizeof(actualEncodedBuffer) - encodedSize
                                      - actualEncodedBufferOffset));

                Decimal64 actualDecodedValue =
                        Util::decimal64FromMultiWidthEncodingRaw(encodedBuffer,
                                                                 encodedSize);
                if (veryVerbose) {
                    P(actualDecodedValue);
                }

                if (DecimalUtil::isNan(DECODED_VALUE)) {
                    ASSERTV(LINE, DECODED_VALUE, actualDecodedValue,
                            DecimalUtil::isNan(actualDecodedValue));
                }
                else {
                    ASSERTV(LINE, DECODED_VALUE, actualDecodedValue,
                            DECODED_VALUE == actualDecodedValue);
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 Forwarding to the right routines
        //
        // Plan:
        //: 1 Try all operations see if basics work
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "BREATHING TEST" << bsl::endl
                               << "==============" << bsl::endl;

        if (veryVerbose) bsl::cout << "C++ Decimal FP to Binary FP"
                                   << bsl::endl;

        for (int i = 0; i < DEC2BIN_DATA_COUNT; ++i) {
            if (veryVerbose) bsl::cout << "Decimal test case: " << i
                                       << bsl::endl;
            const DecBinTestCase& tc = DEC2BIN_DATA[i];

            BDEC::Decimal32  d32 (tc.d32());
            BDEC::Decimal64  d64 (tc.d64());
            BDEC::Decimal128 d128(tc.d128());
#define D2B_ASSERT(nn, fn, mn)                                                \
    LOOP3_ASSERT(tc.d_line, d##nn, Util::decimal##nn##To##fn(d##nn),          \
                            Util::decimal##nn##To##fn(d##nn) == tc.d_##mn);   \
    LOOP3_ASSERT(tc.d_line, d##nn, Util::decimalTo##fn(d##nn),                \
                            Util::decimalTo##fn(d##nn) == tc.d_##mn)
            if (tc.doD32()) {
                D2B_ASSERT(32, Double, d);
                D2B_ASSERT(32, Float, f);

                D2B_ASSERT(64, Double, d);
                D2B_ASSERT(64, Float, f);

                D2B_ASSERT(128, Double, d);
                D2B_ASSERT(128, Float, f);
            }
#undef D2B_ASSERT
        }

        if (veryVerbose) bsl::cout << "DPD, and Network format conversions"
                                   << bsl::endl;

        { // 32
            unsigned char n_d32[] = { 0x26, 0x54, 0xD2, 0xE7 };
            BDEC::Decimal32 h_d32(1234567);
            unsigned char buffer[sizeof(BDEC::Decimal32)];
            BDEC::Decimal32 d32;

            Util::decimalToNetwork(buffer, h_d32);
            ASSERT(0 == bsl::memcmp(n_d32, buffer, sizeof(n_d32)));

            Util::decimalFromNetwork(&d32, buffer);
            LOOP2_ASSERT(d32, h_d32, d32 == h_d32);

            unsigned int rawData = 0x2654D2E7;

            Util::decimalToDPD(buffer, h_d32);
            ASSERT(0 == bsl::memcmp(buffer, &rawData, sizeof(rawData)));

            Util::decimal32ToDPD(buffer, h_d32);
            ASSERT(0 == bsl::memcmp(buffer, &rawData, sizeof(rawData)));

            ASSERT(Util::decimal32FromDPD(buffer) == h_d32);

            Util::decimal32FromDPD(&d32, buffer);
            ASSERT(d32 == h_d32);

            Util::decimalFromDPD(&d32, buffer);
            ASSERT(d32 == h_d32);
        }

        { // 64
            unsigned char n_d64[] = { 0x26, 0x39, 0x34, 0xB9,
                                      0xC1, 0xE2, 0x8E, 0x56 };
            BDEC::Decimal64 h_d64(1234567890123456ull);
            unsigned char buffer[sizeof(BDEC::Decimal64)];
            BDEC::Decimal64 d64;

            Util::decimalToNetwork(buffer, h_d64);
            ASSERT(0 == bsl::memcmp(n_d64, buffer, sizeof(n_d64)));

            Util::decimalFromNetwork(&d64, buffer);
            LOOP2_ASSERT(d64, h_d64, d64 == h_d64);

            unsigned long long rawData = 0x263934B9C1E28E56ULL;

            Util::decimalToDPD(buffer, h_d64);
            ASSERT(0 == bsl::memcmp(&rawData, buffer, sizeof(rawData)));

            Util::decimal64ToDPD(buffer, h_d64);
            ASSERT(0 == bsl::memcmp(&rawData, buffer, sizeof(rawData)));

            ASSERT(Util::decimal64FromDPD(buffer) == h_d64);

            Util::decimal64FromDPD(&d64, buffer);
            ASSERT(d64 == h_d64);

            Util::decimalFromDPD(&d64, buffer);
            ASSERT(d64 == h_d64);
        }

        { // 128
            unsigned char n_d128[] = { 0x26, 0x08, 0x13, 0x4B,
                                       0x9C, 0x1E, 0x28, 0xE5,
                                       0x6F, 0x3C, 0x12, 0x71,
                                       0x77, 0x82, 0x35, 0x34,};
            BDEC::Decimal128 h_d128(
                       BDLDFP_DECIMAL_DL(1234567890123456789012345678901234.));
            unsigned char buffer[sizeof(BDEC::Decimal128)];
            BDEC::Decimal128 d128;

            Util::decimalToNetwork(buffer, h_d128);
            ASSERT(0 == bsl::memcmp(n_d128, buffer, sizeof(n_d128)));

            Util::decimalFromNetwork(&d128, buffer);
            LOOP2_ASSERT(d128, h_d128, d128 == h_d128);

            bdldfp::Uint128 rawData(0x2608134B9C1E28E5ULL,
                                    0x6F3C127177823534ULL);

            Util::decimalToDPD(buffer, h_d128);
            ASSERT(0 == bsl::memcmp(&rawData, buffer, sizeof(rawData)));

            Util::decimal128ToDPD(buffer, h_d128);
            ASSERT(0 == bsl::memcmp(&rawData, buffer, sizeof(rawData)));

            ASSERT(Util::decimal128FromDPD(buffer) == h_d128);

            Util::decimal128FromDPD(&d128, buffer);
            ASSERT(d128 == h_d128);

            Util::decimalFromDPD(&d128, buffer);
            ASSERT(d128 == h_d128);
        }

        if (veryVerbose) bsl::cout << "Decimal-binary-decimal trip"
                                   << bsl::endl;

        // No guarantees if these aren't binary:

        if (std::numeric_limits<double>::radix == 2 &&
            std::numeric_limits< float>::radix == 2)
        {
            if (veryVeryVerbose) bsl::cout << "Decimal32" << bsl::endl;
            {
                BDEC::Decimal32 original(0.0);

                double      bin_d;
                float       bin_f;

                BDEC::Decimal32  restored;

                // Testing 0.0

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal32FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing -0.0

                original = -original;

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal32FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 1.0

                original = BDLDFP_DECIMAL_DF(1.);

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal32FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.1

                original = BDLDFP_DECIMAL_DF(0.1);

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal32FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.2

                original = BDLDFP_DECIMAL_DF(0.2);

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal32FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.3

                original = BDLDFP_DECIMAL_DF(0.3);

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal32FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.123456

                original = BDLDFP_DECIMAL_DF(0.123456);

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal32FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.1234567 -- float cannot do that

                original = BDLDFP_DECIMAL_DF(0.1234567);

                bin_d   = Util::decimalToDouble(original);

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                // Testing Dec32-max

                original = std::numeric_limits<BDEC::Decimal32>::max();

                bin_d   = Util::decimalToDouble(original);

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                // Testing Dec32-min -- float cannot do that

                original = std::numeric_limits<BDEC::Decimal32>::min();

                bin_d   = Util::decimalToDouble(original);

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                // Testing Dec32-denorm_min -- float cannot do that

                original = std::numeric_limits<BDEC::Decimal32>::denorm_min();

                bin_d   = Util::decimalToDouble(original);

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                // Testing NaN

                original = std::numeric_limits<BDEC::Decimal32>::quiet_NaN();

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);


                restored = Util::decimal32FromDouble(bin_d);
                LOOP2_ASSERT(restored, bin_d, restored != restored);

                restored = Util::decimal32FromFloat(bin_f);
                LOOP2_ASSERT(restored, bin_f, restored != restored);

                // Testing +INF

                original = std::numeric_limits<BDEC::Decimal32>::infinity();

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal32FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing -INF

                original = -original;

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal32FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));
            }

            if (veryVeryVerbose) bsl::cout << "Decimal64" << bsl::endl;
            {
                BDEC::Decimal64 original(0.0);

                double      bin_d;
                float       bin_f;

                BDEC::Decimal64  restored;

                // Testing 0.0

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal64FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing -0.0

                original = -original;

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal64FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 1.0

                original = BDLDFP_DECIMAL_DD(1.);

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal64FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.1

                original = BDLDFP_DECIMAL_DD(0.1);

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal64FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.2

                original = BDLDFP_DECIMAL_DD(0.2);

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal64FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.3

                original = BDLDFP_DECIMAL_DD(0.3);

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal64FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.123456 - the last that shall fit a float

                original = BDLDFP_DECIMAL_DD(0.123456);

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal64FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.123456789012345 - the last that shall fit a double

                original = BDLDFP_DECIMAL_DD(0.123456789012345);

                bin_d   = Util::decimalToDouble(original);

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                // Testing NaN

                original = std::numeric_limits<BDEC::Decimal64>::quiet_NaN();
                original = std::numeric_limits<BDEC::Decimal64>::quiet_NaN();
                LOOP2_ASSERT(original,
                    std::numeric_limits<BDEC::Decimal64>::quiet_NaN(),
                    strictEqual(original, std::numeric_limits<BDEC::Decimal64>::quiet_NaN()));
                BDEC::Decimal64 qnan(std::numeric_limits<BDEC::Decimal64>::quiet_NaN());
                BDEC::Decimal64 zero(0.0);
                LOOP2_ASSERT(original, qnan, strictEqual(original, qnan));
                LOOP2_ASSERT(zero, qnan, !strictEqual(zero, qnan));
                LOOP2_ASSERT(zero, qnan, qnan != zero);
                LOOP2_ASSERT(zero, qnan, !(qnan == zero));

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromDouble(bin_d);
                LOOP2_ASSERT(restored, bin_d, restored != restored);

                restored = Util::decimal64FromFloat(bin_f);
                LOOP2_ASSERT(restored, bin_f, restored != restored);

                // Testing +INF

                original = std::numeric_limits<BDEC::Decimal64>::infinity();

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal64FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing -INF

                original = -original;

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal64FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));
            }

            if (veryVeryVerbose) bsl::cout << "Decimal128" << bsl::endl;
            {
                BDEC::Decimal128 original(0.0);

                double      bin_d;
                float       bin_f;

                BDEC::Decimal128  restored;

                // Testing 0.0

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal128FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal128FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing -0.0

                original = -original;

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);


                restored = Util::decimal128FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal128FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 1.0

                original = BDLDFP_DECIMAL_DL(1.);

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal128FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal128FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.1

                original = BDLDFP_DECIMAL_DL(0.1);

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal128FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal128FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.2

                original = BDLDFP_DECIMAL_DL(0.2);

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal128FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal128FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.3

                original = BDLDFP_DECIMAL_DL(0.3);

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal128FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal128FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.123456 - the last that shall fit a float

                original = BDLDFP_DECIMAL_DL(0.123456);

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal128FromDouble(bin_d);
                ASSERT(strictEqual(original, restored));

                restored = Util::decimal128FromFloat(bin_f);
                ASSERT(strictEqual(original, restored));

                // Testing 0.123456789012345 - the last that shall fit a double

                original = BDLDFP_DECIMAL_DL(0.123456789012345);

                bin_d   = Util::decimalToDouble(original);

                restored = Util::decimal128FromDouble(bin_d);
                ASSERT(strictEqual(original, restored));

                // Testing NaN

                original = std::numeric_limits<BDEC::Decimal128>::quiet_NaN();

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal128FromDouble(bin_d);
                ASSERT(restored != restored);

                restored = Util::decimal128FromFloat(bin_f);
                ASSERT(restored != restored);

                // Testing +INF

                original = std::numeric_limits<BDEC::Decimal128>::infinity();

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);


                restored = Util::decimal128FromDouble(bin_d);
                ASSERT(strictEqual(original, restored));

                restored = Util::decimal128FromFloat(bin_f);
                ASSERT(strictEqual(original, restored));

                // Testing -INF

                original = -original;

                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal128FromDouble(bin_d);
                ASSERT(strictEqual(original, restored));

                restored = Util::decimal128FromFloat(bin_f);
                ASSERT(strictEqual(original, restored));
            }
        }
        else {
            if (veryVerbose) bsl::cout << "Skipped, no binary FP" << bsl::endl;
        }
      } break;
      default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: No memory came from the global or default allocator.

    LOOP2_ASSERT(test, globalAllocator.numBlocksTotal(),
                 0 == globalAllocator.numBlocksTotal());
    LOOP2_ASSERT(test, defaultAllocator.numBlocksTotal(),
                 0 == defaultAllocator.numBlocksTotal());

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
