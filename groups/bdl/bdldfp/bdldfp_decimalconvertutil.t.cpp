// bdldfp_decimalconvertutil.t.cpp                                    -*-C++-*-

#include <bdldfp_decimalconvertutil.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_cstdlib.h>
#include <bsl_climits.h>
#include <bsl_limits.h>
#include <bsl_cmath.h>
#include <bsl_cfloat.h>
#include <bsl_algorithm.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bslmf_assert.h>

#include <typeinfo>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;
using bsl::hex;
using bsl::atoi;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// TBD:
// ----------------------------------------------------------------------------
// CREATORS
//
// MANIPULATORS
//
// ACCESSORS
//
// FREE OPERATORS
//
// TRAITS
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE
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
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

#define PX(X) cout << #X " = " << hex << (X) << endl; // Print id and hex value

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace BDEC = BloombergLP::bdldfp;

static bslma::Allocator *ia = bslma::Default::globalAllocator();

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

BSLMF_ASSERT(sizeof(long double) <= sizeof(long long) * 2);
Mantissa128 mantissaBits(long double ld)
    // Return, as a binary integer, the significand of the specifies floating
    // point value.  Note that sign is ignored.  The return value of this
    // function is unspecifed when it is called with a floating point value of
    // +/- infinity, or a subnormal value.  The behavior is undefined if the
    // specified 'ld' is NaN.
{
    union {
        long double as_ldouble;
        Mantissa128 as_int;
    } x;

    if (sizeof(long double) == sizeof(double)) {
        x.as_int.hi = 0;
        x.as_int.lo = mantissaBits(double(ld));
    }
    else {
        x.as_ldouble = ld;
        x.as_int.hi &= 0xffffffffffffull;
    }

    return x.as_int;
}

                            // Strict comparators

template <class DECIMAL_TYPE>
bool strictEqual(DECIMAL_TYPE lhs, DECIMAL_TYPE rhs)
    // Return true if the bit pattern in the specified 'lhs' decimal type
    // matches that in the specified 'rhs' decimal type.
{
    const void *blhs = &lhs;
    const void *brhs = &rhs;
    return memcmp(blhs, brhs, sizeof(DECIMAL_TYPE)) == 0;
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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;


    typedef BDEC::DecimalConvertUtil Util;

    cout.precision(35);

    switch (test) { case 0:
    case 2: {
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

            ASSERT(memcmp(msgbuffer, expected, sizeof(number)) == 0);
        }
        //..
        // The receiver/client shall then restore the number from network
        // format:
        //..
        { // "client"
            unsigned char   msgbuffer[] ={
                              0x25, 0x55, 0x34, 0xb9, 0xc1, 0xe2, 0x8e, 0x56 };
            BDEC::Decimal64 number;
            BDEC::Decimal64 expected(BDLDFP_DECIMAL_DD(1.234567890123456e-42));

            unsigned char *next = msgbuffer;
            next = bdldfp::DecimalConvertUtil::decimalFromNetwork(&number,
                                                                  next);

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
                D2B_ASSERT(32, LongDouble, ld);
                D2B_ASSERT(32, Double, d);
                D2B_ASSERT(32, Float, f);

                D2B_ASSERT(64, LongDouble, ld);
                D2B_ASSERT(64, Double, d);
                D2B_ASSERT(64, Float, f);

                D2B_ASSERT(128, LongDouble, ld);
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
            ASSERT(0 == memcmp(n_d32, buffer, sizeof(n_d32)));

            Util::decimalFromNetwork(&d32, buffer);
            LOOP2_ASSERT(d32, h_d32, d32 == h_d32);

            unsigned int rawData = 0x2654D2E7;

            Util::decimalToDenselyPacked(buffer, h_d32);
            ASSERT(0 == memcmp(buffer, &rawData, sizeof(rawData)));

            Util::decimal32ToDenselyPacked(buffer, h_d32);
            ASSERT(0 == memcmp(buffer, &rawData, sizeof(rawData)));

            ASSERT(Util::decimal32FromDenselyPacked(buffer) == h_d32);

            Util::decimal32FromDenselyPacked(&d32, buffer);
            ASSERT(d32 == h_d32);

            Util::decimalFromDenselyPacked(&d32, buffer);
            ASSERT(d32 == h_d32);
        }

        { // 64
            unsigned char n_d64[] = { 0x26, 0x39, 0x34, 0xB9,
                                      0xC1, 0xE2, 0x8E, 0x56 };
            BDEC::Decimal64 h_d64(1234567890123456ull);
            unsigned char buffer[sizeof(BDEC::Decimal64)];
            BDEC::Decimal64 d64;

            Util::decimalToNetwork(buffer, h_d64);
            ASSERT(0 == memcmp(n_d64, buffer, sizeof(n_d64)));

            Util::decimalFromNetwork(&d64, buffer);
            LOOP2_ASSERT(d64, h_d64, d64 == h_d64);

            unsigned long long rawData = 0x263934B9C1E28E56llu;

            Util::decimalToDenselyPacked(buffer, h_d64);
            ASSERT(0 == memcmp(&rawData, buffer, sizeof(rawData)));

            Util::decimal64ToDenselyPacked(buffer, h_d64);
            ASSERT(0 == memcmp(&rawData, buffer, sizeof(rawData)));

            ASSERT(Util::decimal64FromDenselyPacked(buffer) == h_d64);

            Util::decimal64FromDenselyPacked(&d64, buffer);
            ASSERT(d64 == h_d64);

            Util::decimalFromDenselyPacked(&d64, buffer);
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
            ASSERT(0 == memcmp(n_d128, buffer, sizeof(n_d128)));

            Util::decimalFromNetwork(&d128, buffer);
            LOOP2_ASSERT(d128, h_d128, d128 == h_d128);

            bdldfp::Uint128 rawData(0x2608134B9C1E28E5llu,
                                    0x6F3C127177823534llu);

            Util::decimalToDenselyPacked(buffer, h_d128);
            ASSERT(0 == memcmp(&rawData, buffer, sizeof(rawData)));

            Util::decimal128ToDenselyPacked(buffer, h_d128);
            ASSERT(0 == memcmp(&rawData, buffer, sizeof(rawData)));

            ASSERT(Util::decimal128FromDenselyPacked(buffer) == h_d128);

            Util::decimal128FromDenselyPacked(&d128, buffer);
            ASSERT(d128 == h_d128);

            Util::decimalFromDenselyPacked(&d128, buffer);
            ASSERT(d128 == h_d128);
        }

        if (veryVerbose) bsl::cout << "Decimal-binary-decimal trip"
                                   << bsl::endl;

        // No guarantees if these aren't binary:

        if (std::numeric_limits<long double>::radix == 2 &&
            std::numeric_limits<     double>::radix == 2 &&
            std::numeric_limits<      float>::radix == 2)
        {
            if (veryVeryVerbose) bsl::cout << "Decimal32" << bsl::endl;
            {
                BDEC::Decimal32 original(0.0);

                long double bin_ld;  // Carrier binaries
                double      bin_d;
                float       bin_f;

                BDEC::Decimal32  restored;

                // Testing 0.0

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal32FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing -0.0

                original = -original;

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal32FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 1.0

                original = BDLDFP_DECIMAL_DF(1.);

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal32FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.1

                original = BDLDFP_DECIMAL_DF(0.1);

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal32FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.2

                original = BDLDFP_DECIMAL_DF(0.2);

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal32FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.3

                original = BDLDFP_DECIMAL_DF(0.3);

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal32FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.123456

                original = BDLDFP_DECIMAL_DF(0.123456);

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal32FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.1234567 -- float cannot do that

                original = BDLDFP_DECIMAL_DF(0.1234567);

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);

                restored = Util::decimal32FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                // Testing Dec32-max

                original = std::numeric_limits<BDEC::Decimal32>::max();

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);

                restored = Util::decimal32FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                // Testing Dec32-min -- float cannot do that

                original = std::numeric_limits<BDEC::Decimal32>::min();

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);

                restored = Util::decimal32FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                // Testing Dec32-denorm_min -- float cannot do that

                original = std::numeric_limits<BDEC::Decimal32>::denorm_min();

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);

                restored = Util::decimal32FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                // Testing NaN

                original = std::numeric_limits<BDEC::Decimal32>::quiet_NaN();

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromLongDouble(bin_ld);
                LOOP4_ASSERT(original,
                             Util::decimalToLongDouble(original),
                             restored,
                             bin_ld,
                             restored != restored);

                restored = Util::decimal32FromDouble(bin_d);
                LOOP2_ASSERT(restored, bin_d, restored != restored);

                restored = Util::decimal32FromFloat(bin_f);
                LOOP2_ASSERT(restored, bin_f, restored != restored);

                // Testing +INF

                original = std::numeric_limits<BDEC::Decimal32>::infinity();

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal32FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal32FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing -INF

                original = -original;

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal32FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

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

                long double bin_ld;  // Carrier binaries
                double      bin_d;
                float       bin_f;

                BDEC::Decimal64  restored;

                // Testing 0.0

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal64FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing -0.0

                original = -original;

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal64FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 1.0

                original = BDLDFP_DECIMAL_DD(1.);

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal64FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.1

                original = BDLDFP_DECIMAL_DD(0.1);

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal64FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.2

                original = BDLDFP_DECIMAL_DD(0.2);

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal64FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.3

                original = BDLDFP_DECIMAL_DD(0.3);

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal64FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.123456 - the last that shall fit a float

                original = BDLDFP_DECIMAL_DD(0.123456);

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal64FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.123456789012345 - the last that shall fit a double

                original = BDLDFP_DECIMAL_DD(0.123456789012345);

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);

                restored = Util::decimal64FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                if (sizeof(double) < sizeof(long double)) {
                    // Testing 0.1234567890123456 -- Need 128 bits in binary

                    original = BDLDFP_DECIMAL_DD(0.1234567890123456);

                    bin_ld  = Util::decimalToLongDouble(original);

                    restored = Util::decimal64FromLongDouble(bin_ld);
                    LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                    // Testing Dec64-max  -- won't fit in a double or float

                    original = std::numeric_limits<BDEC::Decimal64>::max();

                    bin_ld  = Util::decimalToLongDouble(original);

                    restored = Util::decimal64FromLongDouble(bin_ld);
                    LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                    // Testing Dec64-min -- double, float cannot do that

                    original = std::numeric_limits<BDEC::Decimal64>::min();

                    bin_ld  = Util::decimalToLongDouble(original);

                    restored = Util::decimal64FromLongDouble(bin_ld);
                    LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                    // Testing Dec64-denorm_min -- float/double cannot do that

                    original =
                            std::numeric_limits<BDEC::Decimal64>::denorm_min();

                    bin_ld  = Util::decimalToLongDouble(original);

                    restored = Util::decimal64FromLongDouble(bin_ld);
                    LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));
                }

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

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromLongDouble(bin_ld);
                LOOP4_ASSERT(reinterpret_cast<const unsigned int &>(restored),
reinterpret_cast<const unsigned int &>(bin_ld), restored, bin_ld, restored != restored);

                restored = Util::decimal64FromDouble(bin_d);
                LOOP2_ASSERT(restored, bin_d, restored != restored);

                restored = Util::decimal64FromFloat(bin_f);
                LOOP2_ASSERT(restored, bin_f, restored != restored);

                // Testing +INF

                original = std::numeric_limits<BDEC::Decimal64>::infinity();

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal64FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal64FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing -INF

                original = -original;

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal64FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

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

                long double bin_ld;  // Carrier binaries
                double      bin_d;
                float       bin_f;

                BDEC::Decimal128  restored;

                // Testing 0.0

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal128FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal128FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal128FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing -0.0

                original = -original;

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal128FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal128FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal128FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 1.0

                original = BDLDFP_DECIMAL_DL(1.);

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal128FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal128FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal128FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.1

                original = BDLDFP_DECIMAL_DL(0.1);

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal128FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal128FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal128FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.2

                original = BDLDFP_DECIMAL_DL(0.2);

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal128FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal128FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal128FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.3

                original = BDLDFP_DECIMAL_DL(0.3);

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal128FromLongDouble(bin_ld);
                LOOP3_ASSERT(original, restored, bin_ld,
                             strictEqual(original, restored));

                restored = Util::decimal128FromDouble(bin_d);
                LOOP3_ASSERT(original, restored, bin_d,
                             strictEqual(original, restored));

                restored = Util::decimal128FromFloat(bin_f);
                LOOP3_ASSERT(original, restored, bin_f,
                             strictEqual(original, restored));

                // Testing 0.123456 - the last that shall fit a float

                original = BDLDFP_DECIMAL_DL(0.123456);

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal128FromLongDouble(bin_ld);
                ASSERT(strictEqual(original, restored));

                restored = Util::decimal128FromDouble(bin_d);
                ASSERT(strictEqual(original, restored));

                restored = Util::decimal128FromFloat(bin_f);
                ASSERT(strictEqual(original, restored));

                // Testing 0.123456789012345 - the last that shall fit a double

                original = BDLDFP_DECIMAL_DL(0.123456789012345);

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);

                restored = Util::decimal128FromLongDouble(bin_ld);
                ASSERT(strictEqual(original, restored));

                restored = Util::decimal128FromDouble(bin_d);
                ASSERT(strictEqual(original, restored));

                if (sizeof(double) < sizeof(long double)) {
                    // Testing 0.1234567890123456 -- Need 128 bits in binary

                    original = BDLDFP_DECIMAL_DL(0.1234567890123456);

                    bin_ld  = Util::decimalToLongDouble(original);

                    restored = Util::decimal128FromLongDouble(bin_ld);
                    ASSERT(strictEqual(original, restored));

                    // Testing Dec128-max  -- won't fit into anything

                    // Testing Dec128-min  -- won't fit into anything

                    // Testing Dec128-denorm_min -- won't fit into anything
                }

                // Testing NaN

                original = std::numeric_limits<BDEC::Decimal128>::quiet_NaN();

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal128FromLongDouble(bin_ld);
                ASSERT(restored != restored);

                restored = Util::decimal128FromDouble(bin_d);
                ASSERT(restored != restored);

                restored = Util::decimal128FromFloat(bin_f);
                ASSERT(restored != restored);

                // Testing +INF

                original = std::numeric_limits<BDEC::Decimal128>::infinity();

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal128FromLongDouble(bin_ld);
                ASSERT(strictEqual(original, restored));

                restored = Util::decimal128FromDouble(bin_d);
                ASSERT(strictEqual(original, restored));

                restored = Util::decimal128FromFloat(bin_f);
                ASSERT(strictEqual(original, restored));

                // Testing -INF

                original = -original;

                bin_ld  = Util::decimalToLongDouble(original);
                bin_d   = Util::decimalToDouble(original);
                bin_f   = Util::decimalToFloat(original);

                restored = Util::decimal128FromLongDouble(bin_ld);
                ASSERT(strictEqual(original, restored));

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
// Copyright (C) 2014 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
