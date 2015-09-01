// bdldfp_decimalconvertutil.t.cpp                                    -*-C++-*-

#include <bdldfp_decimalconvertutil.h>

#include <bslim_testutil.h>

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
// [ 5] Decimal64 decimal64FromDouble(double);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE
// [-1] CONVERSION TEST
// [-2] ROUND TRIP CONVERSION TEST
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
}  // close namespace UsageExample

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
      case 6: {
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
      case 5: {
        // --------------------------------------------------------------------
        // DTOA CONVERSION TEST
        //
        // Concerns:
        //: 1 Conversion from double to Decimal64 gives valid results.
        //
        // Plan:
        //: 1 Using a table of random numbers converted to string using David
        //:   M. Gay's shortest-representation 'dtoa' algorithm, where that
        //:   representation has no more than 15 significant digits, convert
        //:   those doubles to Decimal64 and verify that the expected result
        //:   is produced.
        //
        // Testing:
        //   unsigned char *decimal64FromDouble(double);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nDTOA CONVERSION TEST"
                             "\n====================\n";

        static const struct {
            int         d_line;         // line number
            double      d_binary;       // binary floating point value
            const char *d_significand;  // shortest decimal significand
            int         d_exponent;     // power of ten multiplier
        } DATA[] = {
            { L_, 1.0013008351397400105892583e-037, "100130083513974",  -36 },
            { L_, 1.0026086550369900394909922e+047, "100260865503699",   48 },
            { L_, 1.0049042631880600797400313e+280, "100490426318806",  281 },
            { L_, 1.0057549493145399316764999e-279, "100575494931454", -278 },
            { L_, 1.0073542921390200006177046e+174, "100735429213902",  175 },
            { L_, 1.0080240142565499926828482e+094, "100802401425655",   95 },
            { L_, 1.0095880711492899978888203e-195, "100958807114929", -194 },
            { L_, 1.0114420634257499518646410e+277, "101144206342575",  278 },
            { L_, 1.0118628505848700135927114e+204, "101186285058487",  205 },
            { L_, 1.0125379999767700540911455e-097, "101253799997677",  -96 },
            { L_, 1.0139767342957900235010906e+232, "101397673429579",  233 },
            { L_, 1.0145755525670300444121268e-190, "101457555256703", -189 },
            { L_, 1.0153606990726199905164834e-106, "101536069907262", -105 },
            { L_, 1.0155348706246100153067787e+067, "101553487062461",   68 },
            { L_, 1.0165376881550599851473585e-020, "101653768815506",  -19 },
            { L_, 1.0168220286192399926188488e+246, "101682202861924",  247 },
            { L_, 1.0172701383559199627029088e-007, "101727013835592",   -6 },
            { L_, 1.0186889588345099238162868e+091, "101868895883451",   92 },
            { L_, 1.0199237950667800368784131e-225, "101992379506678", -224 },
            { L_, 1.0201990418358300409999592e-216, "102019904183583", -215 },
            { L_, 1.0221105680542400403082918e+057, "102211056805424",   58 },
            { L_, 1.0222239888039599755019286e-150, "102222398880396", -149 },
            { L_, 1.0242114216755999713099572e-052,  "10242114216756",  -51 },
            { L_, 1.0263847514109591822395668e-309, "102638475141096", -308 },
            { L_, 1.0285272342245899611976712e+091, "102852723422459",   92 },
            { L_, 1.0313355883343599892692978e+265, "103133558833436",  266 },
            { L_, 1.0314431153079500167364853e-137, "103144311530795", -136 },
            { L_, 1.0318608983432899627858881e+210, "103186089834329",  211 },
            { L_, 1.0326745537766499557435498e+250, "103267455377665",  251 },
            { L_, 1.0331853006253999985115630e+186,  "10331853006254",  187 },
            { L_, 1.0361514232490700334433577e-240, "103615142324907", -239 },
            { L_, 1.0363526560649798946588345e-059, "103635265606498",  -58 },
            { L_, 1.0364205126199999841549660e+192,    "103642051262",  193 },
            { L_, 1.0378181231639297554314604e-308, "103781812316393", -307 },
            { L_, 1.0406020413346900515068370e+155, "104060204133469",  156 },
            { L_, 1.0407876513608999426201191e-020,  "10407876513609",  -19 },
            { L_, 1.0418392057694000349753905e+162,  "10418392057694",  163 },
            { L_, 1.0419067779422200553730968e+192, "104190677794222",  193 },
            { L_, 1.0430429559083200840943072e+038, "104304295590832",   39 },
            { L_, 1.0454567612228900436884347e+127, "104545676122289",  128 },
            { L_, 1.0466939686421500370511642e+191, "104669396864215",  192 },
            { L_, 1.0467328835487599582893589e-244, "104673288354876", -243 },
            { L_, 1.0487849425396400411370739e-144, "104878494253964", -143 },
            { L_, 1.0517991557603400534983359e+106, "105179915576034",  107 },
            { L_, 1.0519077747326000462635420e+004,  "10519077747326",    5 },
            { L_, 1.0525800987026800080343747e+037, "105258009870268",   38 },
            { L_, 1.0527870829931500066517017e+233, "105278708299315",  234 },
            { L_, 1.0541715136282600558915149e+286, "105417151362826",  287 },
            { L_, 1.0546514631449299871078272e+163, "105465146314493",  164 },
            { L_, 1.0559363262438099970728432e-141, "105593632624381", -140 },
            { L_, 1.0561825682471699498476577e-058, "105618256824717",  -57 },
            { L_, 1.0573055291231199373471988e-029, "105730552912312",  -28 },
            { L_, 1.0630110416746299232999473e-173, "106301104167463", -172 },
            { L_, 1.0659686799474200473568560e-137, "106596867994742", -136 },
            { L_, 1.0665975537587899809590344e-113, "106659755375879", -112 },
            { L_, 1.0670305123710100178863941e+156, "106703051237101",  157 },
            { L_, 1.0677505099478799613628995e-224, "106775050994788", -223 },
            { L_, 1.0698126177743299420285794e-126, "106981261777433", -125 },
            { L_, 1.0698146935516299060232625e-167, "106981469355163", -166 },
            { L_, 1.0698157849661699236738089e+211, "106981578496617",  212 },
            { L_, 1.0705097237329599458453626e+223, "107050972373296",  224 },
            { L_, 1.0761388851121400544654969e+052, "107613888511214",   53 },
            { L_, 1.0768384733606500961080321e+078, "107683847336065",   79 },
            { L_, 1.0790597462727600077725315e-234, "107905974627276", -233 },
            { L_, 1.0822626915341699538752931e+068, "108226269153417",   69 },
            { L_, 1.0824661875931700890324688e-226, "108246618759317", -225 },
            { L_, 1.0829304102888599667561082e+307, "108293041028886",  308 },
            { L_, 1.0832952179602499949793794e-118, "108329521796025", -117 },
            { L_, 1.0880557804912599299239007e+156, "108805578049126",  157 },
            { L_, 1.0902777787905399980456989e-152, "109027777879054", -151 },
            { L_, 1.0914956007524399264563200e+023, "109149560075244",   24 },
            { L_, 1.0923201562285000538375552e+254,  "10923201562285",  255 },
            { L_, 1.0936676781274799349353139e+229, "109366767812748",  230 },
            { L_, 1.0937007727426599412089487e-288, "109370077274266", -287 },
            { L_, 1.0937446428254699303607588e-025, "109374464282547",  -24 },
            { L_, 1.0943372424030499315623466e-036, "109433724240305",  -35 },
            { L_, 1.0951949757487900033934444e+049, "109519497574879",   50 },
            { L_, 1.0956330209338000419438891e+112,  "10956330209338",  113 },
            { L_, 1.0966912053652399809528527e+165, "109669120536524",  166 },
            { L_, 1.0994627272877799820284304e+154, "109946272728778",  155 },
            { L_, 1.1012403336558700000308789e-151, "110124033365587", -150 },
            { L_, 1.1039540669320299549248570e+278, "110395406693203",  279 },
            { L_, 1.1054649008822200235497541e-273, "110546490088222", -272 },
            { L_, 1.1058373548864300529980143e-144, "110583735488643", -143 },
            { L_, 1.1069379997028399628332697e-294, "110693799970284", -293 },
            { L_, 1.1080796145148800609487421e+027, "110807961451488",   28 },
            { L_, 1.1105860877846200375009487e-023, "111058608778462",  -22 },
            { L_, 1.1111357254919099538550128e-069, "111113572549191",  -68 },
            { L_, 1.1130883083698499825107138e+130, "111308830836985",  131 },
            { L_, 1.1139070691362698970674171e-221, "111390706913627", -220 },
            { L_, 1.1143943113794100377959407e-086, "111439431137941",  -85 },
            { L_, 1.1176382286521299979344553e-035, "111763822865213",  -34 },
            { L_, 1.1182655383033800650124055e-029, "111826553830338",  -28 },
            { L_, 1.1209175000034300088580096e-119, "112091750000343", -118 },
            { L_, 1.1223409732312799746564448e-044, "112234097323128",  -43 },
            { L_, 1.1238905145897800863669395e-018, "112389051458978",  -17 },
            { L_, 1.1245020220497498958867336e-121, "112450202204975", -120 },
            { L_, 1.1248075703456099715293976e-139, "112480757034561", -138 },
            { L_, 1.1250909478899799880541336e-123, "112509094788998", -122 },
            { L_, 1.1266028243966000281368083e+306,  "11266028243966",  307 },
            { L_, 1.1272289388274599348473178e+043, "112722893882746",   44 },
            { L_, 1.1272568606186499592294898e+038, "112725686061865",   39 },
            { L_, 1.1299656846487700282034846e+189, "112996568464877",  190 },
            { L_, 1.1305481077221999753224218e-079,  "11305481077222",  -78 },
            { L_, 1.1305534908357999249934967e+103,  "11305534908358",  104 },
            { L_, 1.1305984259232000493316996e-180,  "11305984259232", -179 },
            { L_, 1.1353330724710999366794406e+262,  "11353330724711",  263 },
            { L_, 1.1411177317922599776569240e+219, "114111773179226",  220 },
            { L_, 1.1433295099136000450870914e+102,  "11433295099136",  103 },
            { L_, 1.1443823791696399343506353e+235, "114438237916964",  236 },
            { L_, 1.1474929638312199647047234e-137, "114749296383122", -136 },
            { L_, 1.1519056047395701085854766e+071, "115190560473957",   72 },
            { L_, 1.1522983256201399635767987e+260, "115229832562014",  261 },
            { L_, 1.1561120149767400097819030e+307, "115611201497674",  308 },
            { L_, 1.1576414846567199851715349e-206, "115764148465672", -205 },
            { L_, 1.1592392007995500666568807e-211, "115923920079955", -210 },
            { L_, 1.1594186481565399305124656e-103, "115941864815654", -102 },
            { L_, 1.1604930049425200748387090e+255, "116049300494252",  256 },
            { L_, 1.1619655485170899295849217e-041, "116196554851709",  -40 },
            { L_, 1.1629737688912100529018380e+146, "116297376889121",  147 },
            { L_, 1.1646263833048499433045747e+166, "116462638330485",  167 },
            { L_, 1.1647490361166800249851163e-255, "116474903611668", -254 },
            { L_, 1.1654943640030300204146773e+115, "116549436400303",  116 },
            { L_, 1.1662690909169800849004691e-053, "116626909091698",  -52 },
            { L_, 1.1671916753361100751216189e+100, "116719167533611",  101 },
            { L_, 1.1672239150618400197722276e-173, "116722391506184", -172 },
            { L_, 1.1683595705415199667630804e+088, "116835957054152",   89 },
            { L_, 1.1701633376884900379050665e+080, "117016333768849",   81 },
            { L_, 1.1704493593923100019829624e+089, "117044935939231",   90 },
            { L_, 1.1714875973872800265500950e+172, "117148759738728",  173 },
            { L_, 1.1723615673816999694737496e-039,  "11723615673817",  -38 },
            { L_, 1.1729976093352900058601508e+144, "117299760933529",  145 },
            { L_, 1.1730434144255199789537697e-241, "117304341442552", -240 },
            { L_, 1.1735585977737000291068900e+253,  "11735585977737",  254 },
            { L_, 1.1740029117843199347978731e+292, "117400291178432",  293 },
            { L_, 1.1750033666251199319958687e+006, "117500336662512",    7 },
            { L_, 1.1762908192221800436497288e+114, "117629081922218",  115 },
            { L_, 1.1765105273385500694804470e-062, "117651052733855",  -61 },
            { L_, 1.1817371874449500282693699e+126, "118173718744495",  127 },
            { L_, 1.1825962013015099707606369e-101, "118259620130151", -100 },
            { L_, 1.1826100333759399991818683e-238, "118261003337594", -237 },
            { L_, 1.1834301717834499745810566e-044, "118343017178345",  -43 },
            { L_, 1.1836096243641000302931592e-306,  "11836096243641", -305 },
            { L_, 1.1888310648178700148995455e-269, "118883106481787", -268 },
            { L_, 1.1909101487219098879801908e+251, "119091014872191",  252 },
            { L_, 1.1909192843495099493111470e-240, "119091928434951", -239 },
            { L_, 1.1918484360026299408166597e-149, "119184843600263", -148 },
            { L_, 1.1923246569067199797564149e-211, "119232465690672", -210 },
            { L_, 1.1938342248968399774520180e+067, "119383422489684",   68 },
            { L_, 1.1980699487037700255839418e-198, "119806994870377", -197 },
            { L_, 1.1997546194894799881842476e+073, "119975461948948",   74 },
            { L_, 1.2008652131863299831470042e+160, "120086521318633",  161 },
            { L_, 1.2013098983619299459446376e-227, "120130989836193", -226 },
            { L_, 1.2025643805248400558587570e-095, "120256438052484",  -94 },
            { L_, 1.2030738509937799474158737e+000, "120307385099378",    1 },
            { L_, 1.2034753885125999308592348e-135,  "12034753885126", -134 },
            { L_, 1.2048938545359899439928943e-122, "120489385453599", -121 },
            { L_, 1.2061801781051400765319467e-300, "120618017810514", -299 },
            { L_, 1.2081147487507000641269791e-176,  "12081147487507", -175 },
            { L_, 1.2091493619836599397412684e-054, "120914936198366",  -53 },
            { L_, 1.2097366300183899567531491e+028, "120973663001839",   29 },
            { L_, 1.2104073300992699436515998e-209, "121040733009927", -208 },
            { L_, 1.2116540305988200216168356e-083, "121165403059882",  -82 },
            { L_, 1.2118045242652199971924255e+111, "121180452426522",  112 },
            { L_, 1.2119596174460500855509161e+115, "121195961744605",  116 },
            { L_, 1.2135748313969099302934651e+228, "121357483139691",  229 },
            { L_, 1.2180161647224799677948567e-120, "121801616472248", -119 },
            { L_, 1.2196035392452599561829833e+182, "121960353924526",  183 },
            { L_, 1.2198183227500600868380143e+221, "121981832275006",  222 },
            { L_, 1.2203036989512799452753532e-295, "122030369895128", -294 },
            { L_, 1.2210481151423699546803319e-023, "122104811514237",  -22 },
            { L_, 1.2233644646313799607777741e+265, "122336446463138",  266 },
            { L_, 1.2256418833145600091341805e+112, "122564188331456",  113 },
            { L_, 1.2283558047979900505796168e-131, "122835580479799", -130 },
            { L_, 1.2284358815113000337066855e-046,  "12284358815113",  -45 },
            { L_, 1.2295470414159298861650293e+304, "122954704141593",  305 },
            { L_, 1.2300268145728299093656629e-296, "123002681457283", -295 },
            { L_, 1.2322976440325700555104068e+064, "123229764403257",   65 },
            { L_, 1.2330462057190499029973486e-205, "123304620571905", -204 },
            { L_, 1.2351188034147100358141509e+113, "123511880341471",  114 },
            { L_, 1.2355997844823400143043722e+061, "123559978448234",   62 },
            { L_, 1.2365356507307799315423663e-098, "123653565073078",  -97 },
            { L_, 1.2373082571533699424695289e-171, "123730825715337", -170 },
            { L_, 1.2380298735061100383015382e-062, "123802987350611",  -61 },
            { L_, 1.2398485899036699762743366e-292, "123984858990367", -291 },
            { L_, 1.2405001921987000099459536e+265,  "12405001921987",  266 },
            { L_, 1.2411892476838300880246142e-286, "124118924768383", -285 },
            { L_, 1.2415556638630700148133675e-148, "124155566386307", -147 },
            { L_, 1.2417284232424699521797696e+100, "124172842324247",  101 },
            { L_, 1.2424242025234100000000000e+014, "124242420252341",   15 },
            { L_, 1.2425234536215199039715714e+149, "124252345362152",  150 },
            { L_, 1.2439415217366400461363619e+047, "124394152173664",   48 },
            { L_, 1.2441205329312099995547807e-113, "124412053293121", -112 },
            { L_, 1.2452102537108099486204431e+268, "124521025371081",  269 },
            { L_, 1.2491644873181599466063100e-121, "124916448731816", -120 },
            { L_, 1.2512915494811800322942317e-243, "125129154948118", -242 },
            { L_, 1.2518181826104600254387724e+283, "125181818261046",  284 },
            { L_, 1.2534952614518300101877190e+208, "125349526145183",  209 },
            { L_, 1.2536922056909200226623545e-136, "125369220569092", -135 },
            { L_, 1.2552816615881699425229309e-169, "125528166158817", -168 },
            { L_, 1.2569002856458300057064069e-140, "125690028564583", -139 },
            { L_, 1.2570687374845800692199321e-203, "125706873748458", -202 },
            { L_, 1.2628661920457000223655001e-139,  "12628661920457", -138 },
            { L_, 1.2650793251581600816350554e+181, "126507932515816",  182 },
            { L_, 1.2685918518815700176434376e-299, "126859185188157", -298 },
            { L_, 1.2697658927458900339668935e-082, "126976589274589",  -81 },
            { L_, 1.2728096571016400521685302e-017, "127280965710164",  -16 },
            { L_, 1.2740941476169399942878166e-191, "127409414761694", -190 },
            { L_, 1.2771791761570198879478713e-181, "127717917615702", -180 },
            { L_, 1.2809425451870800239947137e+033, "128094254518708",   34 },
            { L_, 1.2831618016745800613315440e+191, "128316180167458",  192 },
            { L_, 1.2840115661452999250684727e-282,  "12840115661453", -281 },
            { L_, 1.2850156011485699636409663e+001, "128501560114857",    2 },
            { L_, 1.2859427698691999422654332e-122,  "12859427698692", -121 },
            { L_, 1.2881241051634400567251041e+067, "128812410516344",   68 },
            { L_, 1.2887203440678300941021042e-295, "128872034406783", -294 },
            { L_, 1.2901701169407500836929384e-030, "129017011694075",  -29 },
            { L_, 1.2912274876111398755295106e-277, "129122748761114", -276 },
            { L_, 1.2937572680931399645216837e-167, "129375726809314", -166 },
            { L_, 1.2947927596069699153304100e+007, "129479275960697",    8 },
            { L_, 1.2997631256079800470309854e+253, "129976312560798",  254 },
            { L_, 1.3019852209396500436275260e+053, "130198522093965",   54 },
            { L_, 1.3029928042342499088105922e-056, "130299280423425",  -55 },
            { L_, 1.3053705077766598986941019e+105, "130537050777666",  106 },
            { L_, 1.3065623245122400299282319e-255, "130656232451224", -254 },
            { L_, 1.3086280648349100613529703e-046, "130862806483491",  -45 },
            { L_, 1.3110316003528700577715896e+201, "131103160035287",  202 },
            { L_, 1.3128839266304200987095808e+220, "131288392663042",  221 },
            { L_, 1.3146491673228000602261229e+249,  "13146491673228",  250 },
            { L_, 1.3147855046174799185053335e-031, "131478550461748",  -30 },
            { L_, 1.3155310966550899272580783e-106, "131553109665509", -105 },
            { L_, 1.3179352150490200347224982e+052, "131793521504902",   53 },
            { L_, 1.3182600229523901028164210e-035, "131826002295239",  -34 },
            { L_, 1.3187048831377201004449705e+227, "131870488313772",  228 },
            { L_, 1.3194935708509400931761383e-223, "131949357085094", -222 },
            { L_, 1.3199471594323899674644440e-248, "131994715943239", -247 },
            { L_, 1.3262000111227399995421607e+205, "132620001112274",  206 },
            { L_, 1.3300624617482300246796596e-148, "133006246174823", -147 },
            { L_, 1.3317963000897000706872527e-206,  "13317963000897", -205 },
            { L_, 1.3339170331232800426589637e-026, "133391703312328",  -25 },
            { L_, 1.3357485373894299348838839e-074, "133574853738943",  -73 },
            { L_, 1.3388033166767800627201337e-090, "133880331667678",  -89 },
            { L_, 1.3395366939110599977468956e-251, "133953669391106", -250 },
            { L_, 1.3397998113432199257240180e-056, "133979981134322",  -55 },
            { L_, 1.3424946090742800252109967e-298, "134249460907428", -297 },
            { L_, 1.3427641261079400000000000e+014, "134276412610794",   15 },
            { L_, 1.3446775244476699935066457e+042, "134467752444767",   43 },
            { L_, 1.3475419170896399346233104e+241, "134754191708964",  242 },
            { L_, 1.3478941155072299196482699e+080, "134789411550723",   81 },
            { L_, 1.3505961186567899868860192e-137, "135059611865679", -136 },
            { L_, 1.3556689511133099274998884e+184, "135566895111331",  185 },
            { L_, 1.3575260684157400609602984e-098, "135752606841574",  -97 },
            { L_, 1.3588934497389299524975597e-035, "135889344973893",  -34 },
            { L_, 1.3605285470928500776702248e+243, "136052854709285",  244 },
            { L_, 1.3606819564999300703722514e-234, "136068195649993", -233 },
            { L_, 1.3616995834496000026010197e+173,  "13616995834496",  174 },
            { L_, 1.3632928428586001083494581e-038,  "13632928428586",  -37 },
            { L_, 1.3681551204031199874836385e+212, "136815512040312",  213 },
            { L_, 1.3708754750253001057959453e-048,  "13708754750253",  -47 },
            { L_, 1.3722972760132999165975724e-205,  "13722972760133", -204 },
            { L_, 1.3724670821708100529592686e+117, "137246708217081",  118 },
            { L_, 1.3743186396943700968764044e+229, "137431863969437",  230 },
            { L_, 1.3746955881566200516349315e-121, "137469558815662", -120 },
            { L_, 1.3751445633070499324129258e-248, "137514456330705", -247 },
            { L_, 1.3758102352738600089427107e+175, "137581023527386",  176 },
            { L_, 1.3763638160764199555722186e-111, "137636381607642", -110 },
            { L_, 1.3832653148275600790174511e+107, "138326531482756",  108 },
            { L_, 1.3833271206920000327344804e-249,   "1383327120692", -248 },
            { L_, 1.3840305133832500317054441e-176, "138403051338325", -175 },
            { L_, 1.3841503295512601146706943e-237, "138415032955126", -236 },
            { L_, 1.3850281154606499716960731e-263, "138502811546065", -262 },
            { L_, 1.3855764910838401303993811e-231, "138557649108384", -230 },
            { L_, 1.3859052272354600117873179e-282, "138590522723546", -281 },
            { L_, 1.3859759338911900588183239e-069, "138597593389119",  -68 },
            { L_, 1.3861616515923899441986578e+003, "138616165159239",    4 },
            { L_, 1.3868352633574400752527158e-027, "138683526335744",  -26 },
            { L_, 1.3885731023578400545099795e+072, "138857310235784",   73 },
            { L_, 1.3896057304133700459492053e-112, "138960573041337", -111 },
            { L_, 1.3905360301997399356195780e+289, "139053603019974",  290 },
            { L_, 1.3923030167864300220996837e-173, "139230301678643", -172 },
            { L_, 1.3933481279121399988280982e+165, "139334812791214",  166 },
            { L_, 1.3959501926242899396114634e-078, "139595019262429",  -77 },
            { L_, 1.3978163336362099951516967e+032, "139781633363621",   33 },
            { L_, 1.4005688061553300662477459e+097, "140056880615533",   98 },
            { L_, 1.4048779031432099273501990e+001, "140487790314321",    2 },
            { L_, 1.4052162709077799880003891e-196, "140521627090778", -195 },
            { L_, 1.4055697344600000617065497e-303,    "140556973446", -302 },
            { L_, 1.4082850268704200444919201e+155, "140828502687042",  156 },
            { L_, 1.4084740982509199411373152e-056, "140847409825092",  -55 },
            { L_, 1.4087228557800900565974615e+236, "140872285578009",  237 },
            { L_, 1.4098730878483898805821815e-162, "140987308784839", -161 },
            { L_, 1.4099601372939799403584370e+221, "140996013729398",  222 },
            { L_, 1.4103634755357699546131478e+221, "141036347553577",  222 },
            { L_, 1.4109509016475200573495612e-093, "141095090164752",  -92 },
            { L_, 1.4137381588069799037537398e-236, "141373815880698", -235 },
            { L_, 1.4141157798277099450075935e+162, "141411577982771",  163 },
            { L_, 1.4148763416779600523484186e-191, "141487634167796", -190 },
            { L_, 1.4151162704100500501422394e-239, "141511627041005", -238 },
            { L_, 1.4225642351837900028257082e+148, "142256423518379",  149 },
            { L_, 1.4227843551032099520814974e-030, "142278435510321",  -29 },
            { L_, 1.4233914931492299794927531e+242, "142339149314923",  243 },
            { L_, 1.4236556586343999164936682e-255,  "14236556586344", -254 },
            { L_, 1.4237090872110499665793608e-167, "142370908721105", -166 },
            { L_, 1.4252524232786100360430110e-208, "142525242327861", -207 },
            { L_, 1.4265666937795499566883441e+112, "142656669377955",  113 },
            { L_, 1.4270603485935600787770984e+193, "142706034859356",  194 },
            { L_, 1.4276885458499699227840245e+135, "142768854584997",  136 },
            { L_, 1.4298093716743900608285055e-145, "142980937167439", -144 },
            { L_, 1.4302550649761501249822487e-023, "143025506497615",  -22 },
            { L_, 1.4315689338832699942723483e-149, "143156893388327", -148 },
            { L_, 1.4335721030544599771118050e-027, "143357210305446",  -26 },
            { L_, 1.4357311987128099325333629e+030, "143573119871281",   31 },
            { L_, 1.4365411037677600825999579e-280, "143654110376776", -279 },
            { L_, 1.4371975664713600342702777e+166, "143719756647136",  167 },
            { L_, 1.4390208757887100652570732e-250, "143902087578871", -249 },
            { L_, 1.4398541630619198762220821e+095, "143985416306192",   96 },
            { L_, 1.4402477540658898647052411e+002, "144024775406589",    3 },
            { L_, 1.4404071525596900982253397e-245, "144040715255969", -244 },
            { L_, 1.4408083003376199226703315e-037, "144080830033762",  -36 },
            { L_, 1.4414068866091600065284319e-220, "144140688660916", -219 },
            { L_, 1.4423383331150701179476749e-287, "144233833311507", -286 },
            { L_, 1.4428550663425001035878081e+151,  "14428550663425",  152 },
            { L_, 1.4448515019590799489263881e+078, "144485150195908",   79 },
            { L_, 1.4469666032674399590370354e-019, "144696660326744",  -18 },
            { L_, 1.4481349865976699239726584e+093, "144813498659767",   94 },
            { L_, 1.4485662749452600000000000e+015, "144856627494526",   16 },
            { L_, 1.4494507975203101322171185e+030, "144945079752031",   31 },
            { L_, 1.4502746591683199310276361e+221, "145027465916832",  222 },
            { L_, 1.4513515566048000971478703e+180,  "14513515566048",  181 },
            { L_, 1.4515990571605499458832031e-065, "145159905716055",  -64 },
            { L_, 1.4546140202525399067342812e+037, "145461402025254",   38 },
            { L_, 1.4580894488668100089163358e+060, "145808944886681",   61 },
            { L_, 1.4584285249540800950361820e-258, "145842852495408", -257 },
            { L_, 1.4585962981273200440275303e-061, "145859629812732",  -60 },
            { L_, 1.4590219844158901129941529e-215, "145902198441589", -214 },
            { L_, 1.4594259290462300431787676e+061, "145942592904623",   62 },
            { L_, 1.4606359430264400522740651e-115, "146063594302644", -114 },
            { L_, 1.4646478942676500532854622e+090, "146464789426765",   91 },
            { L_, 1.4662825622431700927143798e-192, "146628256224317", -191 },
            { L_, 1.4669238795173099522009947e-135, "146692387951731", -134 },
            { L_, 1.4673642646381899794505443e+003, "146736426463819",    4 },
            { L_, 1.4684104194525798400000000e+017, "146841041945258",   18 },
            { L_, 1.4701338190181600835248554e+054, "147013381901816",   55 },
            { L_, 1.4705790366659100819574143e-125, "147057903666591", -124 },
            { L_, 1.4711381842424799264674753e-231, "147113818424248", -230 },
            { L_, 1.4723401647401599884033203e+010, "147234016474016",   11 },
            { L_, 1.4744660093481499408556236e+245, "147446600934815",  246 },
            { L_, 1.4773656462998600247260583e+001, "147736564629986",    2 },
            { L_, 1.4800115128510599541983394e-187, "148001151285106", -186 },
            { L_, 1.4812210039261999656565056e+090,  "14812210039262",   91 },
            { L_, 1.4813780530319399877995148e-235, "148137805303194", -234 },
            { L_, 1.4829201690073699855649396e-233, "148292016900737", -232 },
            { L_, 1.4842925593047599337819096e+237, "148429255930476",  238 },
            { L_, 1.4843201057162599489598248e-153, "148432010571626", -152 },
            { L_, 1.4865602089038300813165753e-134, "148656020890383", -133 },
            { L_, 1.4887468738750299537697677e+082, "148874687387503",   83 },
            { L_, 1.4893773090514100478756679e-032, "148937730905141",  -31 },
            { L_, 1.4899122673813099768755062e+156, "148991226738131",  157 },
            { L_, 1.4899549997721200061660803e-167, "148995499977212", -166 },
            { L_, 1.4926063178331999137843416e-174,  "14926063178332", -173 },
            { L_, 1.4929174623324199568736588e-222, "149291746233242", -221 },
            { L_, 1.4929696391821499541751276e+068, "149296963918215",   69 },
            { L_, 1.4949801691414800149177389e+228, "149498016914148",  229 },
            { L_, 1.4975354339488598715561683e+117, "149753543394886",  118 },
            { L_, 1.4989709091214800273712681e+134, "149897090912148",  135 },
            { L_, 1.4991405829807600017506374e+192, "149914058298076",  193 },
            { L_, 1.4994311576040500564551916e-069, "149943115760405",  -68 },
            { L_, 1.5003231459073100781004438e+218, "150032314590731",  219 },
            { L_, 1.5013803065234800024869104e+228, "150138030652348",  229 },
            { L_, 1.5026674822596599449438424e-255, "150266748225966", -254 },
            { L_, 1.5031889628018401623927119e+284, "150318896280184",  285 },
            { L_, 1.5035904473074499324212676e+159, "150359044730745",  160 },
            { L_, 1.5047780013448900665803939e-264, "150477800134489", -263 },
            { L_, 1.5053438457879900841547681e-259, "150534384578799", -258 },
            { L_, 1.5092097305454399422456730e-208, "150920973054544", -207 },
            { L_, 1.5095383554351999379471809e-009,  "15095383554352",   -8 },
            { L_, 1.5153542670627300945944722e-178, "151535426706273", -177 },
            { L_, 1.5163598830191799943610891e+172, "151635988301918",  173 },
            { L_, 1.5196183420057200401893161e-102, "151961834200572", -101 },
            { L_, 1.5199319717198898914451636e+298, "151993197171989",  299 },
            { L_, 1.5208896795093099003704408e-182, "152088967950931", -181 },
            { L_, 1.5213303212101999764194255e-206,  "15213303212102", -205 },
            { L_, 1.5259223767862700975557557e+152, "152592237678627",  153 },
            { L_, 1.5271420341817000192497295e+174,  "15271420341817",  175 },
            { L_, 1.5277022023646101162688120e+151, "152770220236461",  152 },
            { L_, 1.5281515334369699134139001e+108, "152815153343697",  109 },
            { L_, 1.5288427785764199814440902e-248, "152884277857642", -247 },
            { L_, 1.5295213035656599478859371e+247, "152952130356566",  248 },
            { L_, 1.5312354854041000264193887e+133,  "15312354854041",  134 },
            { L_, 1.5317680157055700703851924e-135, "153176801570557", -134 },
            { L_, 1.5343236482071100631658141e-084, "153432364820711",  -83 },
            { L_, 1.5352369100417200759798967e+226, "153523691004172",  227 },
            { L_, 1.5374936467563399719991992e-201, "153749364675634", -200 },
            { L_, 1.5397507574653699542984002e-192, "153975075746537", -191 },
            { L_, 1.5429669062532398789469703e-290, "154296690625324", -289 },
            { L_, 1.5431938812877399189761714e+030, "154319388128774",   31 },
            { L_, 1.5444838787321401107035749e-290, "154448387873214", -289 },
            { L_, 1.5449228057713699614307405e+036, "154492280577137",   37 },
            { L_, 1.5461290978212199236487260e-182, "154612909782122", -181 },
            { L_, 1.5481165783126899973768810e-095, "154811657831269",  -94 },
            { L_, 1.5496146048774700381730400e+029, "154961460487747",   30 },
            { L_, 1.5496169864063599288856841e+112, "154961698640636",  113 },
            { L_, 1.5506788046360600815333558e+131, "155067880463606",  132 },
            { L_, 1.5550035018171800529655809e-221, "155500350181718", -220 },
            { L_, 1.5577573728774399332337010e+227, "155775737287744",  228 },
            { L_, 1.5593707280528000471029044e+215,  "15593707280528",  216 },
            { L_, 1.5630570913225700024895559e+001, "156305709132257",    2 },
            { L_, 1.5633317626431300704206831e+224, "156333176264313",  225 },
            { L_, 1.5667817326090800404596844e-163, "156678173260908", -162 },
            { L_, 1.5676472641842699099074728e+046, "156764726418427",   47 },
            { L_, 1.5712084814152200622115691e+246, "157120848141522",  247 },
            { L_, 1.5715925276395899300411188e+264, "157159252763959",  265 },
            { L_, 1.5735562122743600462198301e-026, "157355621227436",  -25 },
            { L_, 1.5770082549604798992892692e-059, "157700825496048",  -58 },
            { L_, 1.5774090517924899320184684e+193, "157740905179249",  194 },
            { L_, 1.5799808626950600362098732e-063, "157998086269506",  -62 },
            { L_, 1.5815887324690099200000000e+017, "158158873246901",   18 },
            { L_, 1.5835982046854898929595947e+009, "158359820468549",   10 },
            { L_, 1.5846531193285801442239101e+278, "158465311932858",  279 },
            { L_, 1.5866856705033500843680690e+222, "158668567050335",  223 },
            { L_, 1.5890532182789599247497465e-054, "158905321827896",  -53 },
            { L_, 1.5903329839729399364546130e-242, "159033298397294", -241 },
            { L_, 1.5920723013538000657178973e+296,  "15920723013538",  297 },
            { L_, 1.5928882794115600952794734e+246, "159288827941156",  247 },
            { L_, 1.5939362206084699457174643e-053, "159393622060847",  -52 },
            { L_, 1.5947055924688004253007524e-309,  "15947055924688", -308 },
            { L_, 1.5959886681240399703837980e+121, "159598866812404",  122 },
            { L_, 1.5983001384834198468066113e+113, "159830013848342",  114 },
            { L_, 1.5992741250479398792782480e-287, "159927412504794", -286 },
            { L_, 1.6055506979502299530859562e-179, "160555069795023", -178 },
            { L_, 1.6055525665416000942318582e+102,  "16055525665416",  103 },
            { L_, 1.6106877198524900822836326e+300, "161068771985249",  301 },
            { L_, 1.6116355785584400647199162e+198, "161163557855844",  199 },
            { L_, 1.6118713545872501135210234e+139, "161187135458725",  140 },
            { L_, 1.6120784713815699366693731e+205, "161207847138157",  206 },
            { L_, 1.6123228662296999195669659e+252,  "16123228662297",  253 },
            { L_, 1.6125552318342200781257735e-178, "161255523183422", -177 },
            { L_, 1.6128428962014100560281600e+023, "161284289620141",   24 },
            { L_, 1.6133957648422901401575508e-045, "161339576484229",  -44 },
            { L_, 1.6146253848246200465883946e+168, "161462538482462",  169 },
            { L_, 1.6178066290831900777473134e-131, "161780662908319", -130 },
            { L_, 1.6217566092428899643756952e-177, "162175660924289", -176 },
            { L_, 1.6218136032996499850526834e+061, "162181360329965",   62 },
            { L_, 1.6246944494707300791167793e-207, "162469444947073", -206 },
            { L_, 1.6249498177996200017855012e+238, "162494981779962",  239 },
            { L_, 1.6257390566811798850231895e+098, "162573905668118",   99 },
            { L_, 1.6264442836835798621177673e+008, "162644428368358",    9 },
            { L_, 1.6269699755761099383467212e-092, "162696997557611",  -91 },
            { L_, 1.6297323296737100418724332e-282, "162973232967371", -281 },
            { L_, 1.6349112287747800877780636e+300, "163491122877478",  301 },
            { L_, 1.6374308650487099180233773e-212, "163743086504871", -211 },
            { L_, 1.6384166054985601223057661e-126, "163841660549856", -125 },
            { L_, 1.6391100960905700687958841e+112, "163911009609057",  113 },
            { L_, 1.6396083862697500576021782e-075, "163960838626975",  -74 },
            { L_, 1.6408619717972099854206097e-159, "164086197179721", -158 },
            { L_, 1.6420381765464600373091573e+289, "164203817654646",  290 },
            { L_, 1.6434712685561200158453769e+131, "164347126855612",  132 },
            { L_, 1.6443435192019799092438835e-216, "164434351920198", -215 },
            { L_, 1.6482884101595800227912530e+270, "164828841015958",  271 },
            { L_, 1.6486172274471000806750492e+195,  "16486172274471",  196 },
            { L_, 1.6499451680209499020577057e-217, "164994516802095", -216 },
            { L_, 1.6507889569916599671560846e+081, "165078895699166",   82 },
            { L_, 1.6525105275579100627134679e-074, "165251052755791",  -73 },
            { L_, 1.6547798638160999989466707e-216,  "16547798638161", -215 },
            { L_, 1.6548496759866500120600626e-271, "165484967598665", -270 },
            { L_, 1.6553195318459900954466666e-183, "165531953184599", -182 },
            { L_, 1.6561013641517900487386509e-283, "165610136415179", -282 },
            { L_, 1.6577716546522300898845733e+128, "165777165465223",  129 },
            { L_, 1.6579860709069498660300908e-101, "165798607090695", -100 },
            { L_, 1.6582438836050498842093564e-116, "165824388360505", -115 },
            { L_, 1.6586109999448100473213648e-173, "165861099994481", -172 },
            { L_, 1.6600771229974300672000000e+019, "166007712299743",   20 },
            { L_, 1.6606501262264999281903856e-200,  "16606501262265", -199 },
            { L_, 1.6613710017929498894168360e+204, "166137100179295",  205 },
            { L_, 1.6630045687832599853359040e+041, "166300456878326",   42 },
            { L_, 1.6634350517729300124156189e+252, "166343505177293",  253 },
            { L_, 1.6656479089057799961620205e+159, "166564790890578",  160 },
            { L_, 1.6671721724322600558543551e+270, "166717217243226",  271 },
            { L_, 1.6681592556497899149288835e-037, "166815925564979",  -36 },
            { L_, 1.6682109064227900034893537e+003, "166821090642279",    4 },
            { L_, 1.6688093663979799048927985e-165, "166880936639798", -164 },
            { L_, 1.6692089888636600033175148e-047, "166920898886366",  -46 },
            { L_, 1.6719371490931900818097021e+163, "167193714909319",  164 },
            { L_, 1.6731484422257699441942574e+175, "167314844222577",  176 },
            { L_, 1.6753769947139299158772264e-225, "167537699471393", -224 },
            { L_, 1.6764204845478901023477682e+199, "167642048454789",  200 },
            { L_, 1.6796539065179598635126673e+094, "167965390651796",   95 },
            { L_, 1.6831638201486699278363930e-038, "168316382014867",  -37 },
            { L_, 1.6833047096735900848110408e-089, "168330470967359",  -88 },
            { L_, 1.6841442113234400651619911e-167, "168414421132344", -166 },
            { L_, 1.6849171570553901164356523e+071, "168491715705539",   72 },
            { L_, 1.6851440019343401446592898e+110, "168514400193434",  111 },
            { L_, 1.6864744749465400320000000e+018, "168647447494654",   19 },
            { L_, 1.6865415573615998833181883e-097,  "16865415573616",  -96 },
            { L_, 1.6870257012869299571497909e-295, "168702570128693", -294 },
            { L_, 1.6887427759473801457873873e-198, "168874277594738", -197 },
            { L_, 1.6899432760312799372527037e+227, "168994327603128",  228 },
            { L_, 1.6902344504594301073726709e-290, "169023445045943", -289 },
            { L_, 1.6920873807244299884692184e-116, "169208738072443", -115 },
            { L_, 1.6935165105127901186012711e+095, "169351651051279",   96 },
            { L_, 1.6958594795892601311399471e-007, "169585947958926",   -6 },
            { L_, 1.6960797579951200517150940e-118, "169607975799512", -117 },
            { L_, 1.6968475652367801062364408e+121, "169684756523678",  122 },
            { L_, 1.7008796069744999685958468e+192,  "17008796069745",  193 },
            { L_, 1.7062430532801099247108447e-031, "170624305328011",  -30 },
            { L_, 1.7066663696304101760390862e+094, "170666636963041",   95 },
            { L_, 1.7102033276946600618105659e+293, "171020332769466",  294 },
            { L_, 1.7135564541123301160527621e-302, "171355645411233", -301 },
            { L_, 1.7144034147777401084246672e+071, "171440341477774",   72 },
            { L_, 1.7159774508058900961311398e-262, "171597745080589", -261 },
            { L_, 1.7164162686910700771767670e-186, "171641626869107", -185 },
            { L_, 1.7193916796126199088024403e+045, "171939167961262",   46 },
            { L_, 1.7216140937559399872081039e-114, "172161409375594", -113 },
            { L_, 1.7225621927871600417333749e-017, "172256219278716",  -16 },
            { L_, 1.7253224632182598561860885e-098, "172532246321826",  -97 },
            { L_, 1.7295518914018101019043333e+261, "172955189140181",  262 },
            { L_, 1.7296399222463898469835443e+191, "172963992224639",  192 },
            { L_, 1.7313825391495101127517847e+086, "173138253914951",   87 },
            { L_, 1.7340060941143500926345311e-059, "173400609411435",  -58 },
            { L_, 1.7360569091077500348625100e-135, "173605690910775", -134 },
            { L_, 1.7392002179133400568122466e+177, "173920021791334",  178 },
            { L_, 1.7407452964028699841160522e-059, "174074529640287",  -58 },
            { L_, 1.7425837201762600397023492e-271, "174258372017626", -270 },
            { L_, 1.7427426004254100806804058e+244, "174274260042541",  245 },
            { L_, 1.7429282123717299187938238e-272, "174292821237173", -271 },
            { L_, 1.7435665666202800268432787e-206, "174356656662028", -205 },
            { L_, 1.7447219502824899119105405e-097, "174472195028249",  -96 },
            { L_, 1.7448147913777598960196021e-042, "174481479137776",  -41 },
            { L_, 1.7473037009713399807745032e-035, "174730370097134",  -34 },
            { L_, 1.7505795750534499652632068e-207, "175057957505345", -206 },
            { L_, 1.7505980451697199264103369e+097, "175059804516972",   98 },
            { L_, 1.7521978673075999283392045e+174,  "17521978673076",  175 },
            { L_, 1.7527292733654998598176541e-086,  "17527292733655",  -85 },
            { L_, 1.7555614742582598634641013e+148, "175556147425826",  149 },
            { L_, 1.7556134781009300677459968e+025, "175561347810093",   26 },
            { L_, 1.7564308392674300089264584e+224, "175643083926743",  225 },
            { L_, 1.7605723555953599975281368e+277, "176057235559536",  278 },
            { L_, 1.7634589570594199991146581e-034, "176345895705942",  -33 },
            { L_, 1.7644330627320599261446389e-103, "176443306273206", -102 },
            { L_, 1.7646051859753900609384406e-043, "176460518597539",  -42 },
            { L_, 1.7654809934716999932081031e+140,  "17654809934717",  141 },
            { L_, 1.7680212046636899957294659e-093, "176802120466369",  -92 },
            { L_, 1.7680711196833899094182054e-084, "176807111968339",  -83 },
            { L_, 1.7715633691677400816384878e+127, "177156336916774",  128 },
            { L_, 1.7822975813429400908927484e+225, "178229758134294",  226 },
            { L_, 1.7833324230546700661301229e-004, "178333242305467",   -3 },
            { L_, 1.7833462135884900028548479e-175, "178334621358849", -174 },
            { L_, 1.7836063833009400203193586e-212, "178360638330094", -211 },
            { L_, 1.7860314939119401611362036e-201, "178603149391194", -200 },
            { L_, 1.7874417499426199518252700e-194, "178744174994262", -193 },
            { L_, 1.7875056203820299697130751e-125, "178750562038203", -124 },
            { L_, 1.7876952746293399739586391e+087, "178769527462934",   88 },
            { L_, 1.7888583387999401664569897e-015, "178885833879994",  -14 },
            { L_, 1.7896022416731901316252454e-148, "178960224167319", -147 },
            { L_, 1.7911395862826600436537350e+267, "179113958628266",  268 },
            { L_, 1.7913959299276299556403719e+154, "179139592992763",  155 },
            { L_, 1.7923243451722499132993327e+143, "179232434517225",  144 },
            { L_, 1.7949712678043399379030950e-224, "179497126780434", -223 },
            { L_, 1.7979473746497601319521680e+119, "179794737464976",  120 },
            { L_, 1.7983504413013999823078848e+000,  "17983504413014",    1 },
            { L_, 1.8009221056854599585162628e-218, "180092210568546", -217 },
            { L_, 1.8010529347597700206312759e-169, "180105293475977", -168 },
            { L_, 1.8019446676773299893483765e-178, "180194466767733", -177 },
            { L_, 1.8032584453448301076150528e+001, "180325844534483",    2 },
            { L_, 1.8042426573445400697775462e+133, "180424265734454",  134 },
            { L_, 1.8042799819311299927755075e+037, "180427998193113",   38 },
            { L_, 1.8085961802341099669794017e+121, "180859618023411",  122 },
            { L_, 1.8087864707028999474688633e-139,  "18087864707029", -138 },
            { L_, 1.8096331041992601150827081e+195, "180963310419926",  196 },
            { L_, 1.8098378974921201830100544e-257, "180983789749212", -256 },
            { L_, 1.8106066714991499673600000e+021, "181060667149915",   22 },
            { L_, 1.8107119526670999969869432e+177,  "18107119526671",  178 },
            { L_, 1.8117594603464798960619086e-056, "181175946034648",  -55 },
            { L_, 1.8120134998319300287316642e-129, "181201349983193", -128 },
            { L_, 1.8129067638862999977892485e-214,  "18129067638863", -213 },
            { L_, 1.8131976950050500993260516e+220, "181319769500505",  221 },
            { L_, 1.8153306994506901016129552e-028, "181533069945069",  -27 },
            { L_, 1.8158746932567900843183298e-036, "181587469325679",  -35 },
            { L_, 1.8170023332023000795819473e-103,  "18170023332023", -102 },
            { L_, 1.8178709625564900107888657e-106, "181787096255649", -105 },
            { L_, 1.8187413983669399745688555e+270, "181874139836694",  271 },
            { L_, 1.8197159413960400659599521e+179, "181971594139604",  180 },
            { L_, 1.8208725233265601258134086e+284, "182087252332656",  285 },
            { L_, 1.8225120202562600081583468e-269, "182251202025626", -268 },
            { L_, 1.8243389521420998657737288e+055,  "18243389521421",   56 },
            { L_, 1.8255186506872699418186917e-308, "182551865068727", -307 },
            { L_, 1.8255464603631600341419369e-113, "182554646036316", -112 },
            { L_, 1.8272561506708900428261172e-029, "182725615067089",  -28 },
            { L_, 1.8273555085946399900940748e-047, "182735550859464",  -46 },
            { L_, 1.8280069309520499132304394e+002, "182800693095205",    3 },
            { L_, 1.8302843970863299030147245e-256, "183028439708633", -255 },
            { L_, 1.8309816564421200916462691e+105, "183098165644212",  106 },
            { L_, 1.8313571775141900592029607e-041, "183135717751419",  -40 },
            { L_, 1.8334025242692401015963907e-078, "183340252426924",  -77 },
            { L_, 1.8336550417969398975783355e+297, "183365504179694",  298 },
            { L_, 1.8347313346160698788641837e+085, "183473133461607",   86 },
            { L_, 1.8411255673066701211278339e+073, "184112556730667",   74 },
            { L_, 1.8420807746085199834537432e-108, "184208077460852", -107 },
            { L_, 1.8459034677351901167534016e+216, "184590346773519",  217 },
            { L_, 1.8472948696281998779353172e+291,  "18472948696282",  292 },
            { L_, 1.8513952521077898746220246e+200, "185139525210779",  201 },
            { L_, 1.8516541670805998930189231e-140,  "18516541670806", -139 },
            { L_, 1.8518686158125898542360794e-222, "185186861581259", -221 },
            { L_, 1.8550102672401398816806576e-043, "185501026724014",  -42 },
            { L_, 1.8556249926558799461380183e+102, "185562499265588",  103 },
            { L_, 1.8591963975442398410689817e-297, "185919639754424", -296 },
            { L_, 1.8605356050124399149137683e-180, "186053560501244", -179 },
            { L_, 1.8617790769712701565097920e+125, "186177907697127",  126 },
            { L_, 1.8628789288918400858220740e-161, "186287892889184", -160 },
            { L_, 1.8636865433134699030226747e+282, "186368654331347",  283 },
            { L_, 1.8637350408910399915615135e-171, "186373504089104", -170 },
            { L_, 1.8641135639878899014646627e+176, "186411356398789",  177 },
            { L_, 1.8672287177244799758238397e+039, "186722871772448",   40 },
            { L_, 1.8700364017078999187266087e-181,  "18700364017079", -180 },
            { L_, 1.8706371637586400119529413e-237, "187063716375864", -236 },
            { L_, 1.8725448133659300483474637e-087, "187254481336593",  -86 },
            { L_, 1.8729595465004300809277156e-222, "187295954650043", -221 },
            { L_, 1.8733041269715400114124722e-295, "187330412697154", -294 },
            { L_, 1.8744584369090801224908800e+023, "187445843690908",   24 },
            { L_, 1.8746383922270800565425084e-223, "187463839222708", -222 },
            { L_, 1.8748608908459399556547457e-033, "187486089084594",  -32 },
            { L_, 1.8752867409219501321281926e-120, "187528674092195", -119 },
            { L_, 1.8779483312776700671108223e-185, "187794833127767", -184 },
            { L_, 1.8787843687391701443063741e-217, "187878436873917", -216 },
            { L_, 1.8798161815881300195685943e+128, "187981618158813",  129 },
            { L_, 1.8802283269951499562689261e+084, "188022832699515",   85 },
            { L_, 1.8806570194060600334622128e-155, "188065701940606", -154 },
            { L_, 1.8839790812759899187126246e-224, "188397908127599", -223 },
            { L_, 1.8847285606557199630603246e+198, "188472856065572",  199 },
            { L_, 1.8848214140868101110367669e-198, "188482141408681", -197 },
            { L_, 1.8870265180054200504398079e-267, "188702651800542", -266 },
            { L_, 1.8879377513956797813291832e-308, "188793775139568", -307 },
            { L_, 1.8892820199074400275107687e-014, "188928201990744",  -13 },
            { L_, 1.8898907714585401786777822e-105, "188989077145854", -104 },
            { L_, 1.8908127463301598531064572e-188, "189081274633016", -187 },
            { L_, 1.8915798722830501328624419e+051, "189157987228305",   52 },
            { L_, 1.8922594028412399109173262e+272, "189225940284124",  273 },
            { L_, 1.8939396949526900471519121e+133, "189393969495269",  134 },
            { L_, 1.8945518541025101359025736e-042, "189455185410251",  -41 },
            { L_, 1.8949824418917899315658570e-274, "189498244189179", -273 },
            { L_, 1.8981377843637000000000000e+016,  "18981377843637",   17 },
            { L_, 1.8991086240714101230824640e+046, "189910862407141",   47 },
            { L_, 1.9035422161446801163571007e+075, "190354221614468",   76 },
            { L_, 1.9096404176538600000000000e+016, "190964041765386",   17 },
            { L_, 1.9119213772520800494070990e+190, "191192137725208",  191 },
            { L_, 1.9131666132042600335967743e+138, "191316661320426",  139 },
            { L_, 1.9138193433750000054765253e+118,   "1913819343375",  119 },
            { L_, 1.9139310908206998885558264e+206,  "19139310908207",  207 },
            { L_, 1.9142166440006900908237317e+285, "191421664400069",  286 },
            { L_, 1.9145750368321298362373865e+169, "191457503683213",  170 },
            { L_, 1.9158536305001701307026096e-101, "191585363050017", -100 },
            { L_, 1.9174095796943301408495217e+262, "191740957969433",  263 },
            { L_, 1.9185172315771301883259394e-040, "191851723157713",  -39 },
            { L_, 1.9195415216656000797900800e+025,  "19195415216656",   26 },
            { L_, 1.9212285883089500077541378e-180, "192122858830895", -179 },
            { L_, 1.9224672714700199082792515e+214, "192246727147002",  215 },
            { L_, 1.9240201575723699459955098e+028, "192402015757237",   29 },
            { L_, 1.9254724195459999066685379e+190,   "1925472419546",  191 },
            { L_, 1.9258547568255500652629747e+280, "192585475682555",  281 },
            { L_, 1.9268983522418499842865434e+074, "192689835224185",   75 },
            { L_, 1.9319160460115699543107867e-105, "193191604601157", -104 },
            { L_, 1.9370388272874100202720461e+026, "193703882728741",   27 },
            { L_, 1.9376185709645700913978109e-128, "193761857096457", -127 },
            { L_, 1.9389529415777098491332272e-148, "193895294157771", -147 },
            { L_, 1.9397220405320500165851516e-284, "193972204053205", -283 },
            { L_, 1.9422457682359799984265454e-075, "194224576823598",  -74 },
            { L_, 1.9423260603915600660937193e-197, "194232606039156", -196 },
            { L_, 1.9436459421026198585581509e+064, "194364594210262",   65 },
            { L_, 1.9447860015489699939403543e-191, "194478600154897", -190 },
            { L_, 1.9461510494769299092144120e-178, "194615104947693", -177 },
            { L_, 1.9487384142995300203543593e+153, "194873841429953",  154 },
            { L_, 1.9519797939843898566826348e+281, "195197979398439",  282 },
            { L_, 1.9580516418052899671481312e-221, "195805164180529", -220 },
            { L_, 1.9581620251701700736339734e-260, "195816202517017", -259 },
            { L_, 1.9584435934533101103155803e-226, "195844359345331", -225 },
            { L_, 1.9591956338020400872376127e+276, "195919563380204",  277 },
            { L_, 1.9593186987459699535519085e+067, "195931869874597",   68 },
            { L_, 1.9604139032161201307463799e+167, "196041390321612",  168 },
            { L_, 1.9613401260972199075275524e+073, "196134012609722",   74 },
            { L_, 1.9614261693640199939423993e+134, "196142616936402",  135 },
            { L_, 1.9616318209537901253501834e+032, "196163182095379",   33 },
            { L_, 1.9621497004882300000000000e+016, "196214970048823",   17 },
            { L_, 1.9633365461468399685502343e-072, "196333654614684",  -71 },
            { L_, 1.9633372106351398519939336e+045, "196333721063514",   46 },
            { L_, 1.9659485629372599559431852e+074, "196594856293726",   75 },
            { L_, 1.9665080901093698625387401e+116, "196650809010937",  117 },
            { L_, 1.9670696061985299954355328e+112, "196706960619853",  113 },
            { L_, 1.9695200492507799329138036e-234, "196952004925078", -233 },
            { L_, 1.9703669039219098830972499e+053, "197036690392191",   54 },
            { L_, 1.9704026857507698890480995e-224, "197040268575077", -223 },
            { L_, 1.9711092196543699840864807e+151, "197110921965437",  152 },
            { L_, 1.9712226028487199218750000e+013, "197122260284872",   14 },
            { L_, 1.9721163327876999069481123e+159,  "19721163327877",  160 },
            { L_, 1.9740441749831301323594663e-182, "197404417498313", -181 },
            { L_, 1.9753861015874198161295811e+094, "197538610158742",   95 },
            { L_, 1.9755975469121099225542010e-099, "197559754691211",  -98 },
            { L_, 1.9765069872896401511331882e+103, "197650698728964",  104 },
            { L_, 1.9772891072200100540211423e+197, "197728910722001",  198 },
            { L_, 1.9780214054006001169151458e+252,  "19780214054006",  253 },
            { L_, 1.9781109596424499480032220e+293, "197811095964245",  294 },
            { L_, 1.9790107887829599180358898e-007, "197901078878296",   -6 },
            { L_, 1.9796204696470600443164040e+253, "197962046964706",  254 },
            { L_, 1.9806395438486499029645775e+078, "198063954384865",   79 },
            { L_, 1.9815967442029599346432107e-083, "198159674420296",  -82 },
            { L_, 1.9829920065083699097527122e-259, "198299200650837", -258 },
            { L_, 1.9833483322862799075935699e-181, "198334833228628", -180 },
            { L_, 1.9841312505166199371419090e+142, "198413125051662",  143 },
            { L_, 1.9851869209069099365642993e-154, "198518692090691", -153 },
            { L_, 1.9858299591974599009863053e-200, "198582995919746", -199 },
            { L_, 1.9883448180051100399069858e-069, "198834481800511",  -68 },
            { L_, 1.9897172601661300148533945e+164, "198971726016613",  165 },
            { L_, 1.9905322605367298126220703e+010, "199053226053673",   11 },
            { L_, 1.9913998196077399578469235e-119, "199139981960774", -118 },
            { L_, 1.9922366231930101302037646e-291, "199223662319301", -290 },
            { L_, 1.9938830321447899823029817e+189, "199388303214479",  190 },
            { L_, 1.9943905866256800059941090e+182, "199439058662568",  183 },
            { L_, 1.9970155444143798585109248e+300, "199701554441438",  301 },
            { L_, 1.9988014655535399614287527e-221, "199880146555354", -220 },
            { L_, 1.9994982438043500432112227e+216, "199949824380435",  217 },
            { L_, 2.0006225587668699955072229e-159, "200062255876687", -158 },
            { L_, 2.0030203038080501385209490e-231, "200302030380805", -230 },
            { L_, 2.0040387321693600689723272e-137, "200403873216936", -136 },
            { L_, 2.0048172587983099929192038e+026, "200481725879831",   27 },
            { L_, 2.0058641123088601042443545e-050, "200586411230886",  -49 },
            { L_, 2.0059179868972599227693155e-193, "200591798689726", -192 },
            { L_, 2.0086080953787600994110107e+008, "200860809537876",    9 },
            { L_, 2.0091183904937501006274998e-175, "200911839049375", -174 },
            { L_, 2.0097450322071899759447618e+086, "200974503220719",   87 },
            { L_, 2.0143899046018001626391472e-039,  "20143899046018",  -38 },
            { L_, 2.0161797301012599924766511e+033, "201617973010126",   34 },
            { L_, 2.0163861497924700998548167e-267, "201638614979247", -266 },
            { L_, 2.0165462408030800777496018e-059, "201654624080308",  -58 },
            { L_, 2.0187544052609800403081946e-245, "201875440526098", -244 },
            { L_, 2.0201066657651300536443540e+189, "202010666576513",  190 },
            { L_, 2.0215917566888099846555829e-277, "202159175668881", -276 },
            { L_, 2.0221903057044800456643564e-079, "202219030570448",  -78 },
            { L_, 2.0225411345496899383588057e+267, "202254113454969",  268 },
            { L_, 2.0226325498759899717635622e-063, "202263254987599",  -62 },
            { L_, 2.0243012515756401640486667e+069, "202430125157564",   70 },
            { L_, 2.0253438460164900077690503e+267, "202534384601649",  268 },
            { L_, 2.0255384076440000427868123e+159,   "2025538407644",  160 },
            { L_, 2.0265300808108600882368319e+159, "202653008081086",  160 },
            { L_, 2.0323101253036600313713333e-187, "203231012530366", -186 },
            { L_, 2.0360939297106500150941390e-255, "203609392971065", -254 },
            { L_, 2.0400137731766200198758400e+023, "204001377317662",   24 },
            { L_, 2.0415358360750400244420596e-048, "204153583607504",  -47 },
            { L_, 2.0433423951422999064654851e+109,  "20433423951423",  110 },
            { L_, 2.0457460375178699246119284e+197, "204574603751787",  198 },
            { L_, 2.0462889759702898829230515e+162, "204628897597029",  163 },
            { L_, 2.0469427492577899661898572e+250, "204694274925779",  251 },
            { L_, 2.0487147309757101065309057e-022, "204871473097571",  -21 },
            { L_, 2.0510460125663001114705226e+070,  "20510460125663",   71 },
            { L_, 2.0518961021690801558285788e-247, "205189610216908", -246 },
            { L_, 2.0539471465663200097527411e+167, "205394714656632",  168 },
            { L_, 2.0552467051835099009870950e-281, "205524670518351", -280 },
            { L_, 2.0560396901747098704992596e-191, "205603969017471", -190 },
            { L_, 2.0562908140565498570391349e-231, "205629081405655", -230 },
            { L_, 2.0563916653488098144531250e+010, "205639166534881",   11 },
            { L_, 2.0587576454087400617031376e-216, "205875764540874", -215 },
            { L_, 2.0592112779490100990801768e+127, "205921127794901",  128 },
            { L_, 2.0602845022196798699312743e-076, "206028450221968",  -75 },
            { L_, 2.0615626422638699173392637e+167, "206156264226387",  168 },
            { L_, 2.0626310369835299220090657e-225, "206263103698353", -224 },
            { L_, 2.0637206148101800096991859e-289, "206372061481018", -288 },
            { L_, 2.0643784203275598528348006e-080, "206437842032756",  -79 },
            { L_, 2.0650400814595999973294695e+129,  "20650400814596",  130 },
            { L_, 2.0675046880686599988981001e-057, "206750468806866",  -56 },
            { L_, 2.0702020137750398153872353e+196, "207020201377504",  197 },
            { L_, 2.0746593527917400695251597e-221, "207465935279174", -220 },
            { L_, 2.0827024034356000482107160e+083,  "20827024034356",   84 },
            { L_, 2.0861010975800398498678387e-161, "208610109758004", -160 },
            { L_, 2.0880193955230598396760051e+231, "208801939552306",  232 },
            { L_, 2.0881980377489700337867801e-161, "208819803774897", -160 },
            { L_, 2.0933356937742000933130853e-260,  "20933356937742", -259 },
            { L_, 2.0943166221022699855373749e-302, "209431662210227", -301 },
            { L_, 2.0946081293686701836405315e+187, "209460812936867",  188 },
            { L_, 2.0949314656085798500210829e-053, "209493146560858",  -52 },
            { L_, 2.0981361326444801604493535e-289, "209813613264448", -288 },
            { L_, 2.0984646758955201160750325e-281, "209846467589552", -280 },
            { L_, 2.0992862278443098001611359e+112, "209928622784431",  113 },
            { L_, 2.0995828972108498966520074e-002, "209958289721085",   -1 },
            { L_, 2.1066493352786500867544487e-238, "210664933527865", -237 },
            { L_, 2.1072834076921901018546033e-272, "210728340769219", -271 },
            { L_, 2.1075412260504700708116580e+040, "210754122605047",   41 },
            { L_, 2.1084055481615799473514101e+107, "210840554816158",  108 },
            { L_, 2.1117627816643200228915927e+162, "211176278166432",  163 },
            { L_, 2.1119086583706702139733349e+152, "211190865837067",  153 },
            { L_, 2.1123874833787899976065029e+249, "211238748337879",  250 },
            { L_, 2.1124908751971901024870607e+114, "211249087519719",  115 },
            { L_, 2.1128465001869999253767042e+070,   "2112846500187",   71 },
            { L_, 2.1137690263432498760996861e-176, "211376902634325", -175 },
            { L_, 2.1139302734036099408687210e+172, "211393027340361",  173 },
            { L_, 2.1166184290292400121606865e-137, "211661842902924", -136 },
            { L_, 2.1180390245266999325142656e-132,  "21180390245267", -131 },
            { L_, 2.1189242816482099606864877e+110, "211892428164821",  111 },
            { L_, 2.1189476590785500351324428e-011, "211894765907855",  -10 },
            { L_, 2.1192311928327500840289072e-236, "211923119283275", -235 },
            { L_, 2.1199597319834501861468615e+261, "211995973198345",  262 },
            { L_, 2.1208077175690001361920880e-083,   "2120807717569",  -82 },
            { L_, 2.1229738931144799257633770e-194, "212297389311448", -193 },
            { L_, 2.1251020033498099829230943e-075, "212510200334981",  -74 },
            { L_, 2.1254583648810599825907770e+251, "212545836488106",  252 },
            { L_, 2.1257626324716699311908654e-268, "212576263247167", -267 },
            { L_, 2.1291859928549499779616046e-024, "212918599285495",  -23 },
            { L_, 2.1393148005589300065592168e-027, "213931480055893",  -26 },
            { L_, 2.1400550616854798908589593e+033, "214005506168548",   34 },
            { L_, 2.1410964539903099665605064e+183, "214109645399031",  184 },
            { L_, 2.1417875561011000687491977e+163,  "21417875561011",  164 },
            { L_, 2.1428331332081599274095117e+245, "214283313320816",  246 },
            { L_, 2.1438056644326000409824460e-175,  "21438056644326", -174 },
            { L_, 2.1439864859067601087344440e-249, "214398648590676", -248 },
            { L_, 2.1448157195280400572861171e-194, "214481571952804", -193 },
            { L_, 2.1452352201103701317623742e+125, "214523522011037",  126 },
            { L_, 2.1452829664290500473465130e-120, "214528296642905", -119 },
            { L_, 2.1457944684881798949676081e+168, "214579446848818",  169 },
            { L_, 2.1466648503351999974781313e+133,  "21466648503352",  134 },
            { L_, 2.1478728265872699308297638e+043, "214787282658727",   44 },
            { L_, 2.1488488527742199928668179e+129, "214884885277422",  130 },
            { L_, 2.1498527124392498708608857e+255, "214985271243925",  256 },
            { L_, 2.1509886463583698624013496e+204, "215098864635837",  205 },
            { L_, 2.1521466828561302040011316e+149, "215214668285613",  150 },
            { L_, 2.1553717609386198600281420e+084, "215537176093862",   85 },
            { L_, 2.1558378166611201164403308e-095, "215583781666112",  -94 },
            { L_, 2.1590422946310298985506256e-135, "215904229463103", -134 },
            { L_, 2.1595932015084698378199725e-300, "215959320150847", -299 },
            { L_, 2.1597654269490801056450619e-237, "215976542694908", -236 },
            { L_, 2.1601945169124199221584921e+209, "216019451691242",  210 },
            { L_, 2.1628745353885701285322158e+213, "216287453538857",  214 },
            { L_, 2.1680301395539301200052259e-210, "216803013955393", -209 },
            { L_, 2.1694150392291598789099472e-036, "216941503922916",  -35 },
            { L_, 2.1715711813978300713934799e-120, "217157118139783", -119 },
            { L_, 2.1735052260359501459458441e+123, "217350522603595",  124 },
            { L_, 2.1747633615074400943744339e+066, "217476336150744",   67 },
            { L_, 2.1825138204648601356481149e-167, "218251382046486", -166 },
            { L_, 2.1866231229156199987379361e-086, "218662312291562",  -85 },
            { L_, 2.1874605520542499540890230e+094, "218746055205425",   95 },
            { L_, 2.1885361824238598869105874e-069, "218853618242386",  -68 },
            { L_, 2.1902734441648501493203157e+247, "219027344416485",  248 },
            { L_, 2.1905213847653400998496066e-189, "219052138476534", -188 },
            { L_, 2.1953985008236801113254341e-257, "219539850082368", -256 },
            { L_, 2.1968263453344398702544771e-145, "219682634533444", -144 },
            { L_, 2.1971049444597800764748854e-197, "219710494445978", -196 },
            { L_, 2.1973044349212500044339339e+206, "219730443492125",  207 },
            { L_, 2.1979867266958299540224834e-057, "219798672669583",  -56 },
            { L_, 2.1988790300268699503922687e+181, "219887903002687",  182 },
            { L_, 2.1999273399302901762256898e+140, "219992733993029",  141 },
            { L_, 2.2011633577514898967767137e-280, "220116335775149", -279 },
            { L_, 2.2113065950981700326534566e+288, "221130659509817",  289 },
            { L_, 2.2145001522025699905866816e-305, "221450015220257", -304 },
            { L_, 2.2157382616935300355178177e+045, "221573826169353",   46 },
            { L_, 2.2198241015821400737714758e+179, "221982410158214",  180 },
            { L_, 2.2250444899003801124315640e-206, "222504448990038", -205 },
            { L_, 2.2257854463260901711074674e-096, "222578544632609",  -95 },
            { L_, 2.2268655016192499193165482e+306, "222686550161925",  307 },
            { L_, 2.2275689647858198811854461e-160, "222756896478582", -159 },
            { L_, 2.2281530959904698766987405e-213, "222815309599047", -212 },
            { L_, 2.2285802085461100000127682e+277, "222858020854611",  278 },
            { L_, 2.2289770597398798933915719e+085, "222897705973988",   86 },
            { L_, 2.2294855527120999611282875e+220,  "22294855527121",  221 },
            { L_, 2.2306702324270500310204253e-174, "223067023242705", -173 },
            { L_, 2.2320050778383001100432116e+217,  "22320050778383",  218 },
            { L_, 2.2328170182283900602823144e+046, "223281701822839",   47 },
            { L_, 2.2339074989467399745371418e-254, "223390749894674", -253 },
            { L_, 2.2366474792927702068117234e-155, "223664747929277", -154 },
            { L_, 2.2371969016611600658025099e+246, "223719690166116",  247 },
            { L_, 2.2406920250047001405581349e+094,  "22406920250047",   95 },
            { L_, 2.2414417631072599794819346e-278, "224144176310726", -277 },
            { L_, 2.2433246853518699897768635e+054, "224332468535187",   55 },
            { L_, 2.2446211497892598525716186e+149, "224462114978926",  150 },
            { L_, 2.2453770105754400089673802e-027, "224537701057544",  -26 },
            { L_, 2.2457608583726900412366848e+052, "224576085837269",   53 },
            { L_, 2.2479053833124900716215624e-302, "224790538331249", -301 },
            { L_, 2.2504413878205400035928969e-038, "225044138782054",  -37 },
            { L_, 2.2520727677311198942936156e+188, "225207276773112",  189 },
            { L_, 2.2525421996338699346710312e+187, "225254219963387",  188 },
            { L_, 2.2531321587080801092619067e-125, "225313215870808", -124 },
            { L_, 2.2549409147722000520499705e-111,  "22549409147722", -110 },
            { L_, 2.2569370395469198449229363e-179, "225693703954692", -178 },
            { L_, 2.2575850346841898763667052e+300, "225758503468419",  301 },
            { L_, 2.2592413346328501673569449e-012, "225924133463285",  -11 },
            { L_, 2.2609195722175599035823932e+032, "226091957221756",   33 },
            { L_, 2.2634535722701201937641990e+149, "226345357227012",  150 },
            { L_, 2.2635855727518999439611587e+177,  "22635855727519",  178 },
            { L_, 2.2642134958231197959142037e+099, "226421349582312",  100 },
            { L_, 2.2642429793198400497856339e-257, "226424297931984", -256 },
            { L_, 2.2673657593362899477593989e-195, "226736575933629", -194 },
            { L_, 2.2687583161209999205968430e+148,   "2268758316121",  149 },
            { L_, 2.2687977390568700631627382e-183, "226879773905687", -182 },
            { L_, 2.2699231397345399661292682e-149, "226992313973454", -148 },
            { L_, 2.2741839444580100185790148e+109, "227418394445801",  110 },
            { L_, 2.2746555703045399348984722e+193, "227465557030454",  194 },
            { L_, 2.2748088136546899699838816e+224, "227480881365469",  225 },
            { L_, 2.2775324193156601294841419e+116, "227753241931566",  117 },
            { L_, 2.2781781652003199212823944e+157, "227817816520032",  158 },
            { L_, 2.2789523727404300507820740e-122, "227895237274043", -121 },
            { L_, 2.2799166043534100466436966e+082, "227991660435341",   83 },
            { L_, 2.2810647933863900522111531e+187, "228106479338639",  188 },
            { L_, 2.2826388552032400398974855e-219, "228263885520324", -218 },
            { L_, 2.2844008142949999310076237e+006,   "2284400814295",    7 },
            { L_, 2.2867336523187600072973785e+100, "228673365231876",  101 },
            { L_, 2.2885532034314501617773437e-252, "228855320343145", -251 },
            { L_, 2.2896960507318599693578627e-134, "228969605073186", -133 },
            { L_, 2.2910637856009997845120998e+220,   "2291063785601",  221 },
            { L_, 2.2914360291926801122574009e-039, "229143602919268",  -38 },
            { L_, 2.2926449740840201218465834e+092, "229264497408402",   93 },
            { L_, 2.2927130749193699996822762e+175, "229271307491937",  176 },
            { L_, 2.2931843368044198488620868e-195, "229318433680442", -194 },
            { L_, 2.2960327690412098348907843e-297, "229603276904121", -296 },
            { L_, 2.2961120179805599907955505e+250, "229611201798056",  251 },
            { L_, 2.2971211745183298536409749e-229, "229712117451833", -228 },
            { L_, 2.2973552114963100485124698e+271, "229735521149631",  272 },
            { L_, 2.2973997096221400424838066e+006, "229739970962214",    7 },
            { L_, 2.2976274179126398765285198e+058, "229762741791264",   59 },
            { L_, 2.2978737729339399998905171e-077, "229787377293394",  -76 },
            { L_, 2.3024477005983297919328684e-193, "230244770059833", -192 },
            { L_, 2.3030484850957401357648835e+216, "230304848509574",  217 },
            { L_, 2.3070191317533999485081323e-048,  "23070191317534",  -47 },
            { L_, 2.3072856108540598908604371e+296, "230728561085406",  297 },
            { L_, 2.3102887535956999677693084e-171,  "23102887535957", -170 },
            { L_, 2.3139362003176899747954465e-069, "231393620031769",  -68 },
            { L_, 2.3139810040966700263032358e-083, "231398100409667",  -82 },
            { L_, 2.3142300302422899265679395e+083, "231423003024229",   84 },
            { L_, 2.3144942826068898994997173e-299, "231449428260689", -298 },
            { L_, 2.3145211250760398103309637e+071, "231452112507604",   72 },
            { L_, 2.3148881840704298197099281e+289, "231488818407043",  290 },
            { L_, 2.3181068060723300320020788e+173, "231810680607233",  174 },
            { L_, 2.3194122397300799887790662e-150, "231941223973008", -149 },
            { L_, 2.3194968249613200603844076e-097, "231949682496132",  -96 },
            { L_, 2.3198596678295100762830283e+281, "231985966782951",  282 },
            { L_, 2.3203192148368202040784546e+171, "232031921483682",  172 },
            { L_, 2.3207972427213500558180683e-226, "232079724272135", -225 },
            { L_, 2.3238953481939099425315639e-263, "232389534819391", -262 },
            { L_, 2.3240818051661300604124978e-198, "232408180516613", -197 },
            { L_, 2.3247013848664298526292199e+153, "232470138486643",  154 },
            { L_, 2.3271604977906200710391505e-154, "232716049779062", -153 },
            { L_, 2.3275778490658601562500000e+013, "232757784906586",   14 },
            { L_, 2.3289456690396100359618560e+024, "232894566903961",   25 },
            { L_, 2.3290341026126501077229093e-021, "232903410261265",  -20 },
            { L_, 2.3292883592011799630959834e+163, "232928835920118",  164 },
            { L_, 2.3305847964316699725406405e+103, "233058479643167",  104 },
            { L_, 2.3323801922447498535874486e+284, "233238019224475",  285 },
            { L_, 2.3360519095148598503443679e+039, "233605190951486",   40 },
            { L_, 2.3361931071275501303897342e-033, "233619310712755",  -32 },
            { L_, 2.3364676497034199852334220e-073, "233646764970342",  -72 },
            { L_, 2.3365267386020499765031216e-270, "233652673860205", -269 },
            { L_, 2.3366478776514201696557524e-291, "233664787765142", -290 },
            { L_, 2.3390832799984800641336455e+136, "233908327999848",  137 },
            { L_, 2.3413336429827698792749992e+247, "234133364298277",  248 },
            { L_, 2.3418049642475298557921069e-017, "234180496424753",  -16 },
            { L_, 2.3467483338114699677660761e+218, "234674833381147",  219 },
            { L_, 2.3476794709416099981730621e+131, "234767947094161",  132 },
            { L_, 2.3503900586465899910114222e+107, "235039005864659",  108 },
            { L_, 2.3514942670995299433308313e-208, "235149426709953", -207 },
            { L_, 2.3521513545732500075653424e-236, "235215135457325", -235 },
            { L_, 2.3522645593817301021473109e+158, "235226455938173",  159 },
            { L_, 2.3560466811214500765331866e-234, "235604668112145", -233 },
            { L_, 2.3578657078115899857460509e-254, "235786570781159", -253 },
            { L_, 2.3603438223037299640257825e-107, "236034382230373", -106 },
            { L_, 2.3623051623675500063519414e-079, "236230516236755",  -78 },
            { L_, 2.3634665041478699246583951e-227, "236346650414787", -226 },
            { L_, 2.3641145997393399445567822e-285, "236411459973934", -284 },
            { L_, 2.3676191492989000078547381e-077,  "23676191492989",  -76 },
            { L_, 2.3681109802502900940800000e+021, "236811098025029",   22 },
            { L_, 2.3684044667457400620927642e+179, "236840446674574",  180 },
            { L_, 2.3701862230182700893444867e+218, "237018622301827",  219 },
            { L_, 2.3721717930009901085627372e-066, "237217179300099",  -65 },
            { L_, 2.3735830777513899317738267e+230, "237358307775139",  231 },
            { L_, 2.3763145263623698529635549e+071, "237631452636237",   72 },
            { L_, 2.3797323681095600484868417e-115, "237973236810956", -114 },
            { L_, 2.3827177725903999020658954e+272,  "23827177725904",  273 },
            { L_, 2.3829017822068799689807521e-055, "238290178220688",  -54 },
            { L_, 2.3834498931276300288000000e+020, "238344989312763",   21 },
            { L_, 2.3883541742701199334149898e+281, "238835417427012",  282 },
            { L_, 2.3906100322259901662030236e+074, "239061003222599",   75 },
            { L_, 2.3923060729798099875740783e+109, "239230607297981",  110 },
            { L_, 2.3948834073484400003246558e+197, "239488340734844",  198 },
            { L_, 2.3952312202836800450436106e+091, "239523122028368",   92 },
            { L_, 2.3962407022349299657188132e+230, "239624070223493",  231 },
            { L_, 2.3972542485905598732216241e-206, "239725424859056", -205 },
            { L_, 2.3974646685880101809939977e-193, "239746466858801", -192 },
            { L_, 2.3977988629705097657051078e-041, "239779886297051",  -40 },
            { L_, 2.3979762036541402095754570e+161, "239797620365414",  162 },
            { L_, 2.3992181583678801077626013e+126, "239921815836788",  127 },
            { L_, 2.4000048780698700341368463e+169, "240000487806987",  170 },
            { L_, 2.4008443690200701603051213e+261, "240084436902007",  262 },
            { L_, 2.4011041151563598632974835e-095, "240110411515636",  -94 },
            { L_, 2.4020752075547200991828992e+103, "240207520755472",  104 },
            { L_, 2.4032198032550200257711891e+298, "240321980325502",  299 },
            { L_, 2.4036441582678000260951717e-257,  "24036441582678", -256 },
            { L_, 2.4063719997524501433031238e-275, "240637199975245", -274 },
            { L_, 2.4115220985537400092004597e-228, "241152209855374", -227 },
            { L_, 2.4168146224603801712197743e-080, "241681462246038",  -79 },
            { L_, 2.4168942540041600549344975e-084, "241689425400416",  -83 },
            { L_, 2.4169112667386599688329113e-177, "241691126673866", -176 },
            { L_, 2.4187274435677502118524517e-146, "241872744356775", -145 },
            { L_, 2.4188207913551201490268930e-244, "241882079135512", -243 },
            { L_, 2.4194427356996999585149747e+027,  "24194427356997",   28 },
            { L_, 2.4205320753467499162070453e+005, "242053207534675",    6 },
            { L_, 2.4230072892272398433052707e-257, "242300728922724", -256 },
            { L_, 2.4342806233599801032357348e-179, "243428062335998", -178 },
            { L_, 2.4346512710100501557220087e+034, "243465127101005",   35 },
            { L_, 2.4364812702623299158686918e-218, "243648127026233", -217 },
            { L_, 2.4370524189874101369302165e-113, "243705241898741", -112 },
            { L_, 2.4385028948127900029837969e-179, "243850289481279", -178 },
            { L_, 2.4421663091224499824794041e-021, "244216630912245",  -20 },
            { L_, 2.4421995137767498575747240e+069, "244219951377675",   70 },
            { L_, 2.4438126221328400136069423e-081, "244381262213284",  -80 },
            { L_, 2.4469006863906198842537673e-220, "244690068639062", -219 },
            { L_, 2.4493886052659798175861523e-276, "244938860526598", -275 },
            { L_, 2.4525775625144400478084231e-287, "245257756251444", -286 },
            { L_, 2.4532355372092901174883592e+101, "245323553720929",  102 },
            { L_, 2.4535047324987900545133219e-195, "245350473249879", -194 },
            { L_, 2.4535968935752498645542796e-082, "245359689357525",  -81 },
            { L_, 2.4614672108648299018116006e+098, "246146721086483",   99 },
            { L_, 2.4620349638572500373061759e+232, "246203496385725",  233 },
            { L_, 2.4651459205735898511026698e+219, "246514592057359",  220 },
            { L_, 2.4660694386104899622695236e+148, "246606943861049",  149 },
            { L_, 2.4678210063119599986803535e-244, "246782100631196", -243 },
            { L_, 2.4690120728030901192362443e+250, "246901207280309",  251 },
            { L_, 2.4703930119748097812067234e+292, "247039301197481",  293 },
            { L_, 2.4724204285978801310666010e+210, "247242042859788",  211 },
            { L_, 2.4763940514925600913132447e-116, "247639405149256", -115 },
            { L_, 2.4775965823510301985094044e-183, "247759658235103", -182 },
            { L_, 2.4828201092595800327031119e-063, "248282010925958",  -62 },
            { L_, 2.4828263116037901311774720e+293, "248282631160379",  294 },
            { L_, 2.4835641499196298574302811e+135, "248356414991963",  136 },
            { L_, 2.4861095833287900942789029e+159, "248610958332879",  160 },
            { L_, 2.4872468338883599645968379e-296, "248724683388836", -295 },
            { L_, 2.4879309562564400060025129e+260, "248793095625644",  261 },
            { L_, 2.4897168459843098452506461e+228, "248971684598431",  229 },
            { L_, 2.4901256277428499576350937e+253, "249012562774285",  254 },
            { L_, 2.4905918977529100949450428e+098, "249059189775291",   99 },
            { L_, 2.4932580423976800988058980e+248, "249325804239768",  249 },
            { L_, 2.4936643171948001433695429e+244,  "24936643171948",  245 },
            { L_, 2.4939844309634499468043448e+179, "249398443096345",  180 },
            { L_, 2.4948953479855098517538866e-235, "249489534798551", -234 },
            { L_, 2.4958007870001700456816280e-215, "249580078700017", -214 },
            { L_, 2.4961569164145101427654994e+035, "249615691641451",   36 },
            { L_, 2.4973776922576599450864587e-015, "249737769225766",  -14 },
            { L_, 2.4976983539979901431286435e-284, "249769835399799", -283 },
            { L_, 2.4994070679343497460411809e-237, "249940706793435", -236 },
            { L_, 2.5013947384562800507451800e+224, "250139473845628",  225 },
            { L_, 2.5046047554087899698060937e+003, "250460475540879",    4 },
            { L_, 2.5066262587028297461125281e-004, "250662625870283",   -3 },
            { L_, 2.5067835677518799346609278e+271, "250678356775188",  272 },
            { L_, 2.5070880858752799937945917e-211, "250708808587528", -210 },
            { L_, 2.5089084432068701244995560e+037, "250890844320687",   38 },
            { L_, 2.5109880043674101116254199e+238, "251098800436741",  239 },
            { L_, 2.5116435379724401678671138e-127, "251164353797244", -126 },
            { L_, 2.5116455429131599388237359e-208, "251164554291316", -207 },
            { L_, 2.5125090264550599241928331e-026, "251250902645506",  -25 },
            { L_, 2.5127747205951499297374231e-278, "251277472059515", -277 },
            { L_, 2.5147165874318801582315245e+125, "251471658743188",  126 },
            { L_, 2.5152734663229200151590703e+001, "251527346632292",    2 },
            { L_, 2.5170589712580400355301345e-025, "251705897125804",  -24 },
            { L_, 2.5179003855033698222936112e+251, "251790038550337",  252 },
            { L_, 2.5229263070336301115853946e-253, "252292630703363", -252 },
            { L_, 2.5289135412711199335805145e-238, "252891354127112", -237 },
            { L_, 2.5339391018294099822952019e+189, "253393910182941",  190 },
            { L_, 2.5345880735978398012169749e-302, "253458807359784", -301 },
            { L_, 2.5354013965743601538976809e+274, "253540139657436",  275 },
            { L_, 2.5372769131068700711976960e+026, "253727691310687",   27 },
            { L_, 2.5409464838805699166227548e-010, "254094648388057",   -9 },
            { L_, 2.5414827187384999815678721e-192,  "25414827187385", -191 },
            { L_, 2.5416252524884999029563621e-080,  "25416252524885",  -79 },
            { L_, 2.5424293049929000333847336e+154,  "25424293049929",  155 },
            { L_, 2.5425389098326701204896224e+273, "254253890983267",  274 },
            { L_, 2.5439056019633098189152705e-276, "254390560196331", -275 },
            { L_, 2.5450417222407298498124714e+090, "254504172224073",   91 },
            { L_, 2.5451954372092700907066915e-123, "254519543720927", -122 },
            { L_, 2.5481842721899398412061989e+141, "254818427218994",  142 },
            { L_, 2.5484970398347299119734256e-059, "254849703983473",  -58 },
            { L_, 2.5495656663584899392690105e-099, "254956566635849",  -98 },
            { L_, 2.5497691588109100460761974e-067, "254976915881091",  -66 },
            { L_, 2.5499131555941701217820710e+292, "254991315559417",  293 },
            { L_, 2.5511673163395699858665466e+008, "255116731633957",    9 },
            { L_, 2.5515137888291800805774652e+067, "255151378882918",   68 },
            { L_, 2.5529484169330199809653333e+278, "255294841693302",  279 },
            { L_, 2.5532367425905400032829110e+213, "255323674259054",  214 },
            { L_, 2.5569194153364598471370358e-301, "255691941533646", -300 },
            { L_, 2.5572607521922801590467952e+242, "255726075219228",  243 },
            { L_, 2.5574351647057701381895895e+236, "255743516470577",  237 },
            { L_, 2.5579689103365201821655126e-152, "255796891033652", -151 },
            { L_, 2.5599547752356201328212083e-141, "255995477523562", -140 },
            { L_, 2.5617627269603801676692788e+112, "256176272696038",  113 },
            { L_, 2.5618200665295499947304550e+026, "256182006652955",   27 },
            { L_, 2.5627338685192598218472176e-009, "256273386851926",   -8 },
            { L_, 2.5633069859419298540669429e+127, "256330698594193",  128 },
            { L_, 2.5640818858940798699393655e-040, "256408188589408",  -39 },
            { L_, 2.5678274109030798302999575e-211, "256782741090308", -210 },
            { L_, 2.5689971849085900855248623e+185, "256899718490859",  186 },
            { L_, 2.5697343551245898792931995e-199, "256973435512459", -198 },
            { L_, 2.5706652881563098421446128e-114, "257066528815631", -113 },
            { L_, 2.5717639119555397714058683e-280, "257176391195554", -279 },
            { L_, 2.5728029840303999646117741e+119,  "25728029840304",  120 },
            { L_, 2.5747692786100198872370213e+245, "257476927861002",  246 },
            { L_, 2.5752232191962898839025534e-123, "257522321919629", -122 },
            { L_, 2.5766534782894300804228566e+224, "257665347828943",  225 },
            { L_, 2.5768280092478799969679670e-002, "257682800924788",   -1 },
            { L_, 2.5770622191623098355055130e-167, "257706221916231", -166 },
            { L_, 2.5772074849924499891855843e-018, "257720748499245",  -17 },
            { L_, 2.5796570088024500980654488e-060, "257965700880245",  -59 },
            { L_, 2.5826208985715797593680580e-165, "258262089857158", -164 },
            { L_, 2.5841321593422897753443320e+304, "258413215934229",  305 },
            { L_, 2.5845283941797899621354650e+204, "258452839417979",  205 },
            { L_, 2.5867188922200099096275195e+203, "258671889222001",  204 },
            { L_, 2.5902320171837100192568514e+041, "259023201718371",   42 },
            { L_, 2.5916256116062200570396497e-294, "259162561160622", -293 },
            { L_, 2.5921293325649099739865736e-126, "259212933256491", -125 },
            { L_, 2.5934316165808299039117166e-123, "259343161658083", -122 },
            { L_, 2.5943545221665801655378765e+190, "259435452216658",  191 },
            { L_, 2.5952960503942500333900910e-098, "259529605039425",  -97 },
            { L_, 2.5993986405024298421112731e-139, "259939864050243", -138 },
            { L_, 2.6016556457739001847669915e-112,  "26016556457739", -111 },
            { L_, 2.6041673867431398535251682e-136, "260416738674314", -135 },
            { L_, 2.6047475835042098250868414e+046, "260474758350421",   47 },
            { L_, 2.6054184839147599594433003e-168, "260541848391476", -167 },
            { L_, 2.6061948869525200695430958e+185, "260619488695252",  186 },
            { L_, 2.6080986144389100646759106e-280, "260809861443891", -279 },
            { L_, 2.6088697960647000312744705e-285,  "26088697960647", -284 },
            { L_, 2.6088976731550798759035278e-187, "260889767315508", -186 },
            { L_, 2.6110917993718400662135999e+246, "261109179937184",  247 },
            { L_, 2.6118162806343599508904618e-209, "261181628063436", -208 },
            { L_, 2.6122397107158200340882261e-296, "261223971071582", -295 },
            { L_, 2.6143877948987601729099318e-119, "261438779489876", -118 },
            { L_, 2.6195547675156102847791771e+151, "261955476751561",  152 },
            { L_, 2.6207617092971902556891464e+086, "262076170929719",   87 },
            { L_, 2.6213819560410201538337355e+227, "262138195604102",  228 },
            { L_, 2.6230807309507100116225875e-232, "262308073095071", -231 },
            { L_, 2.6237043503555499000916527e-086, "262370435035555",  -85 },
            { L_, 2.6240876838676999071533035e-200,  "26240876838677", -199 },
            { L_, 2.6249776387283197822270689e+260, "262497763872832",  261 },
            { L_, 2.6280115661960200689487603e-269, "262801156619602", -268 },
            { L_, 2.6281798455643901820703217e+186, "262817984556439",  187 },
            { L_, 2.6288951130064901865100714e-022, "262889511300649",  -21 },
            { L_, 2.6299040621826697869029927e-172, "262990406218267", -171 },
            { L_, 2.6318423016760098740062242e+185, "263184230167601",  186 },
            { L_, 2.6359280333755699155264335e+039, "263592803337557",   40 },
            { L_, 2.6366098688256200109955284e+159, "263660986882562",  160 },
            { L_, 2.6374291459590899473366544e+062, "263742914595909",   63 },
            { L_, 2.6410010195607999077826826e-279,  "26410010195608", -278 },
            { L_, 2.6412439032564498812057934e+096, "264124390325645",   97 },
            { L_, 2.6428272978099000324989763e-256,  "26428272978099", -255 },
            { L_, 2.6463854462051301012659282e+301, "264638544620513",  302 },
            { L_, 2.6476239708391201532560020e-014, "264762397083912",  -13 },
            { L_, 2.6496340299508299564504500e-290, "264963402995083", -289 },
            { L_, 2.6506604429543901365962919e+233, "265066044295439",  234 },
            { L_, 2.6510876374913598510008658e-006, "265108763749136",   -5 },
            { L_, 2.6511332011805000232558948e+070,  "26511332011805",   71 },
            { L_, 2.6529303180691201484423236e-053, "265293031806912",  -52 },
            { L_, 2.6529648956302100250489792e-008, "265296489563021",   -7 },
            { L_, 2.6545224719548998095910707e+174,  "26545224719549",  175 },
            { L_, 2.6587927723591598234605784e-212, "265879277235916", -211 },
            { L_, 2.6592855420457900318796460e-160, "265928554204579", -159 },
            { L_, 2.6609567170713998262563624e+218,  "26609567170714",  219 },
            { L_, 2.6613474843738998829100351e+139,  "26613474843739",  140 },
            { L_, 2.6614583478042802092260610e+049, "266145834780428",   50 },
            { L_, 2.6649492735305798400000000e+017, "266494927353058",   18 },
            { L_, 2.6696434337688001209543407e-308,  "26696434337688", -307 },
            { L_, 2.6709699885299598780610838e-270, "267096998852996", -269 },
            { L_, 2.6712104774021301366230005e-107, "267121047740213", -106 },
            { L_, 2.6723954175524301068355802e-173, "267239541755243", -172 },
            { L_, 2.6730013787011198911514540e+199, "267300137870112",  200 },
            { L_, 2.6744625952149199469252634e-091, "267446259521492",  -90 },
            { L_, 2.6748045320933599648597646e+302, "267480453209336",  303 },
            { L_, 2.6750477184341001445250909e+222,  "26750477184341",  223 },
            { L_, 2.6754404737472400289334819e+220, "267544047374724",  221 },
            { L_, 2.6786600234829599781439806e+157, "267866002348296",  158 },
            { L_, 2.6812859384899398304874206e+243, "268128593848994",  244 },
            { L_, 2.6814041327063699502404605e-035, "268140413270637",  -34 },
            { L_, 2.6825304102933498871293626e-255, "268253041029335", -254 },
            { L_, 2.6869876083520999753109899e-122,  "26869876083521", -121 },
            { L_, 2.6874843456285601929150351e-171, "268748434562856", -170 },
            { L_, 2.6876389487750300906953541e-217, "268763894877503", -216 },
            { L_, 2.6877300869580800981189674e+201, "268773008695808",  202 },
            { L_, 2.6887312626532600665939637e-055, "268873126265326",  -54 },
            { L_, 2.6895395131608399230430649e-081, "268953951316084",  -80 },
            { L_, 2.6906334582562697836843596e+143, "269063345825627",  144 },
            { L_, 2.6925027840829799313678659e-289, "269250278408298", -288 },
            { L_, 2.6932071055321701267557273e-183, "269320710553217", -182 },
            { L_, 2.6960298226644301002932251e+032, "269602982266443",   33 },
            { L_, 2.6964517982386001130091594e+104,  "26964517982386",  105 },
            { L_, 2.6970583023145301681333309e+034, "269705830231453",   35 },
            { L_, 2.6982613053760801374895283e+162, "269826130537608",  163 },
            { L_, 2.6984483078367300161608466e-096, "269844830783673",  -95 },
            { L_, 2.6994491718146300523335520e-190, "269944917181463", -189 },
            { L_, 2.7030450400835797441574334e-169, "270304504008358", -168 },
            { L_, 2.7033967070685999884683114e+233,  "27033967070686",  234 },
            { L_, 2.7034482323389800609206006e+164, "270344823233898",  165 },
            { L_, 2.7046696534820500712414695e+084, "270466965348205",   85 },
            { L_, 2.7072423447293101292361881e+264, "270724234472931",  265 },
            { L_, 2.7077702548090699954915914e-032, "270777025480907",  -31 },
            { L_, 2.7081980918809199218750000e+012, "270819809188092",   13 },
            { L_, 2.7130905882506100469301453e-066, "271309058825061",  -65 },
            { L_, 2.7138432796162999213163032e+226,  "27138432796163",  227 },
            { L_, 2.7145704276866301081759064e+145, "271457042768663",  146 },
            { L_, 2.7150065064091899566561371e+306, "271500650640919",  307 },
            { L_, 2.7152799139447200788734548e+306, "271527991394472",  307 },
            { L_, 2.7166492606047900662109483e-059, "271664926060479",  -58 },
            { L_, 2.7214103533527500117623843e-041, "272141035335275",  -40 },
            { L_, 2.7245074680138698988589969e-044, "272450746801387",  -43 },
            { L_, 2.7257609580025300580882261e-275, "272576095800253", -274 },
            { L_, 2.7265853738855598828191248e-214, "272658537388556", -213 },
            { L_, 2.7317941922495702778166184e-138, "273179419224957", -137 },
            { L_, 2.7361110280418598557132394e-290, "273611102804186", -289 },
            { L_, 2.7376221194026000806329137e-194,  "27376221194026", -193 },
            { L_, 2.7382431223590000303584442e+194,   "2738243122359",  195 },
            { L_, 2.7388656627796298747458669e+245, "273886566277963",  246 },
            { L_, 2.7407905978282599695858867e-238, "274079059782826", -237 },
            { L_, 2.7413440062000800761012191e+263, "274134400620008",  264 },
            { L_, 2.7436873936169401119899833e-058, "274368739361694",  -57 },
            { L_, 2.7441474610247401527244717e-105, "274414746102474", -104 },
            { L_, 2.7444510634182400657443981e-181, "274445106341824", -180 },
            { L_, 2.7457866435271400132980809e-263, "274578664352714", -262 },
            { L_, 2.7459539406633598148632623e+086, "274595394066336",   87 },
            { L_, 2.7465613849424501325882302e+270, "274656138494245",  271 },
            { L_, 2.7466757103334799694493995e+001, "274667571033348",    2 },
            { L_, 2.7478236838294400188249841e-177, "274782368382944", -176 },
            { L_, 2.7497448691138900290892555e+216, "274974486911389",  217 },
            { L_, 2.7507658901506997448248184e+139,  "27507658901507",  140 },
            { L_, 2.7511742303442799761567548e+144, "275117423034428",  145 },
            { L_, 2.7607957922675199847529527e-006, "276079579226752",   -5 },
            { L_, 2.7610168594520098676560759e-275, "276101685945201", -274 },
            { L_, 2.7614505095311201476705284e-236, "276145050953112", -235 },
            { L_, 2.7615194251430998448636980e-022,  "27615194251431",  -21 },
            { L_, 2.7616553094685701281976287e-199, "276165530946857", -198 },
            { L_, 2.7616557429702400940889771e-269, "276165574297024", -268 },
            { L_, 2.7618874204674898995786964e+258, "276188742046749",  259 },
            { L_, 2.7639297557934901331634161e+161, "276392975579349",  162 },
            { L_, 2.7654281968613801662768540e+115, "276542819686138",  116 },
            { L_, 2.7673307087933399088797159e+003, "276733070879334",    4 },
            { L_, 2.7673968189306700025869375e-007, "276739681893067",   -6 },
            { L_, 2.7674336874568398870253065e+129, "276743368745684",  130 },
            { L_, 2.7685244800035698713399245e-132, "276852448000357", -131 },
            { L_, 2.7690436643729600961839435e+119, "276904366437296",  120 },
            { L_, 2.7695629521242498304456387e-029, "276956295212425",  -28 },
            { L_, 2.7702792042234298598113899e+123, "277027920422343",  124 },
            { L_, 2.7724876403956701548898362e-173, "277248764039567", -172 },
            { L_, 2.7727591712271001017579869e+234,  "27727591712271",  235 },
            { L_, 2.7763356507810798753883201e-217, "277633565078108", -216 },
            { L_, 2.7764002249590300641728388e-146, "277640022495903", -145 },
            { L_, 2.7765126494110699050243242e-146, "277651264941107", -145 },
            { L_, 2.7772879387897201069268580e+036, "277728793878972",   37 },
            { L_, 2.7776471830416201476035961e-067, "277764718304162",  -66 },
            { L_, 2.7793166335355498542703313e+136, "277931663353555",  137 },
            { L_, 2.7795225150734697710710972e-256, "277952251507347", -255 },
            { L_, 2.7881049883332000449408532e-052,  "27881049883332",  -51 },
            { L_, 2.7889431483059200784147616e+082, "278894314830592",   83 },
            { L_, 2.7899093000467299801940949e+153, "278990930004673",  154 },
            { L_, 2.7903842230644798152179180e-298, "279038422306448", -297 },
            { L_, 2.7918611496959601118976381e+081, "279186114969596",   82 },
            { L_, 2.7956057723369799106194478e+001, "279560577233698",    2 },
            { L_, 2.7988791179575601764071729e+269, "279887911795756",  270 },
            { L_, 2.8005293465095199656127174e+102, "280052934650952",  103 },
            { L_, 2.8011069102233600249764730e-059, "280110691022336",  -58 },
            { L_, 2.8012088308558898985429855e+186, "280120883085589",  187 },
            { L_, 2.8068038045732399652392089e-100, "280680380457324",  -99 },
            { L_, 2.8085016738602399530667190e-141, "280850167386024", -140 },
            { L_, 2.8088261712901001465409576e+074,  "28088261712901",   75 },
            { L_, 2.8089457039357000698448497e-004,  "28089457039357",   -3 },
            { L_, 2.8105245993735299725385989e+077, "281052459937353",   78 },
            { L_, 2.8111402013226900712628632e-235, "281114020132269", -234 },
            { L_, 2.8112395597053699548985190e-089, "281123955970537",  -88 },
            { L_, 2.8121481215156598487583216e-248, "281214812151566", -247 },
            { L_, 2.8129804959776897129769224e+098, "281298049597769",   99 },
            { L_, 2.8140536960209901215406391e-113, "281405369602099", -112 },
            { L_, 2.8144547035690700166008059e-143, "281445470356907", -142 },
            { L_, 2.8146450206655200948402634e+267, "281464502066552",  268 },
            { L_, 2.8148285657781300347589454e+105, "281482856577813",  106 },
            { L_, 2.8187532907383799735711040e-155, "281875329073838", -154 },
            { L_, 2.8199327218359798803963893e+219, "281993272183598",  220 },
            { L_, 2.8248844433692300153175737e+254, "282488444336923",  255 },
            { L_, 2.8263849529783399844455537e+275, "282638495297834",  276 },
            { L_, 2.8298269330028200525845101e-241, "282982693300282", -240 },
            { L_, 2.8322834442643399757618242e-036, "283228344426434",  -35 },
            { L_, 2.8336058078960901496871141e+279, "283360580789609",  280 },
            { L_, 2.8342810889126201877338769e-081, "283428108891262",  -80 },
            { L_, 2.8358363264982601191844328e-159, "283583632649826", -158 },
            { L_, 2.8371613188737697537571537e+254, "283716131887377",  255 },
            { L_, 2.8372070059532498342368416e+154, "283720700595325",  155 },
            { L_, 2.8380487631507702172983460e-209, "283804876315077", -208 },
            { L_, 2.8383343937117798763334432e+107, "283833439371178",  108 },
            { L_, 2.8395632037985301519827081e+262, "283956320379853",  263 },
            { L_, 2.8407225339242898667866502e-155, "284072253392429", -154 },
            { L_, 2.8469315467429498572640685e-194, "284693154674295", -193 },
            { L_, 2.8490884540792399773909900e+157, "284908845407924",  158 },
            { L_, 2.8515067909749200285620604e+225, "285150679097492",  226 },
            { L_, 2.8524285605143299675477790e-274, "285242856051433", -273 },
            { L_, 2.8525756605689699759823788e-265, "285257566056897", -264 },
            { L_, 2.8526478241908101553190617e+163, "285264782419081",  164 },
            { L_, 2.8553727482864097899221875e+167, "285537274828641",  168 },
            { L_, 2.8578894276188298897121733e-076, "285788942761883",  -75 },
            { L_, 2.8638805226321299639884974e+112, "286388052263213",  113 },
            { L_, 2.8647266001917101342809107e-150, "286472660019171", -149 },
            { L_, 2.8667373008924498365651965e+133, "286673730089245",  134 },
            { L_, 2.8674051756221901556898529e-272, "286740517562219", -271 },
            { L_, 2.8688347036051199678841073e-002, "286883470360512",   -1 },
            { L_, 2.8691853828877099852310112e+100, "286918538288771",  101 },
            { L_, 2.8696858529019499754029132e-261, "286968585290195", -260 },
            { L_, 2.8703864132960199642122262e+224, "287038641329602",  225 },
            { L_, 2.8802928411355199199558338e+195, "288029284113552",  196 },
            { L_, 2.8807458926559200335759670e+004, "288074589265592",    5 },
            { L_, 2.8808295324317101900673540e-023, "288082953243171",  -22 },
            { L_, 2.8817899457357198959511064e-033, "288178994573572",  -32 },
            { L_, 2.8821305684178302860225026e-110, "288213056841783", -109 },
            { L_, 2.8860207650977399497427367e+260, "288602076509774",  261 },
            { L_, 2.8885337005963698877803796e-044, "288853370059637",  -43 },
            { L_, 2.8889705466353997830907177e-278,  "28889705466354", -277 },
            { L_, 2.8890338279658001013765865e-023,  "28890338279658",  -22 },
            { L_, 2.8891717571108699956798131e+185, "288917175711087",  186 },
            { L_, 2.8902663919649700142676832e-273, "289026639196497", -272 },
            { L_, 2.8922075315655798319876724e-238, "289220753156558", -237 },
            { L_, 2.8925513964700701904371030e+295, "289255139647007",  296 },
            { L_, 2.8930927661388402924951173e-194, "289309276613884", -193 },
            { L_, 2.8964337637399099316496024e+280, "289643376373991",  281 },
            { L_, 2.8987233581634101581516345e-233, "289872335816341", -232 },
            { L_, 2.8991911989519201082332937e-239, "289919119895192", -238 },
            { L_, 2.9024678122806301201048885e+089, "290246781228063",   90 },
            { L_, 2.9027983346795700892917103e-178, "290279833467957", -177 },
            { L_, 2.9073916101895798079497158e-046, "290739161018958",  -45 },
            { L_, 2.9074763768226401461575517e-005, "290747637682264",   -4 },
            { L_, 2.9076038980841998235296327e-181,  "29076038980842", -180 },
            { L_, 2.9080234315256499827185744e-245, "290802343152565", -244 },
            { L_, 2.9120924558578798835210893e-146, "291209245585788", -145 },
            { L_, 2.9134338161128798388357609e+270, "291343381611288",  271 },
            { L_, 2.9136822753501298688000000e+019, "291368227535013",   20 },
            { L_, 2.9144585614134100200073827e-141, "291445856141341", -140 },
            { L_, 2.9158996647135397676526272e+183, "291589966471354",  184 },
            { L_, 2.9192160864521901807465857e-274, "291921608645219", -273 },
            { L_, 2.9197636436228298450107539e+078, "291976364362283",   79 },
            { L_, 2.9205654011776799026092461e+096, "292056540117768",   97 },
            { L_, 2.9210576940704700548465754e+103, "292105769407047",  104 },
            { L_, 2.9233906917852498945440508e+140, "292339069178525",  141 },
            { L_, 2.9249216935815700799288102e-064, "292492169358157",  -63 },
            { L_, 2.9256583618982298837139155e-244, "292565836189823", -243 },
            { L_, 2.9288320485725600820757738e-206, "292883204857256", -205 },
            { L_, 2.9329928113459001512882612e-217,  "29329928113459", -216 },
            { L_, 2.9349680708918798477868992e-112, "293496807089188", -111 },
            { L_, 2.9357536585948002443826561e+220,  "29357536585948",  221 },
            { L_, 2.9358881040271101713758051e+230, "293588810402711",  231 },
            { L_, 2.9361407159099201231084113e+286, "293614071590992",  287 },
            { L_, 2.9407286920131698497598941e+259, "294072869201317",  260 },
            { L_, 2.9430968241022701444567447e-297, "294309682410227", -296 },
            { L_, 2.9437563263750497623438086e-035, "294375632637505",  -34 },
            { L_, 2.9437579937478598039961600e+024, "294375799374786",   25 },
            { L_, 2.9445575003413498580974039e+283, "294455750034135",  284 },
            { L_, 2.9463148052423798689098488e-273, "294631480524238", -272 },
            { L_, 2.9473520900834999924899904e-002,  "29473520900835",   -1 },
            { L_, 2.9516676035350602089630683e-054, "295166760353506",  -53 },
            { L_, 2.9519178872992902320350232e+046, "295191788729929",   47 },
            { L_, 2.9533496466740200000000000e+016, "295334964667402",   17 },
            { L_, 2.9544346159286700764712403e+034, "295443461592867",   35 },
            { L_, 2.9545910840064297485767752e-153, "295459108400643", -152 },
            { L_, 2.9549166880888799232000000e+019, "295491668808888",   20 },
            { L_, 2.9558991169272498452371846e-254, "295589911692725", -253 },
            { L_, 2.9559727071411697888071135e-028, "295597270714117",  -27 },
            { L_, 2.9560326260988100351074375e+299, "295603262609881",  300 },
            { L_, 2.9565619111981102518947778e-271, "295656191119811", -270 },
            { L_, 2.9585819336348700843704320e+025, "295858193363487",   26 },
            { L_, 2.9601884693543299998990547e+050, "296018846935433",   51 },
            { L_, 2.9611316407009199317585441e-181, "296113164070092", -180 },
            { L_, 2.9632037466914901253011464e+137, "296320374669149",  138 },
            { L_, 2.9633637005555200878496227e+247, "296336370055552",  248 },
            { L_, 2.9655098750169299978805906e+209, "296550987501693",  210 },
            { L_, 2.9663387710635200170718014e-080, "296633877106352",  -79 },
            { L_, 2.9665192584492600237318984e+048, "296651925844926",   49 },
            { L_, 2.9677092937287301987193484e+167, "296770929372873",  168 },
            { L_, 2.9684791638226799227674599e+143, "296847916382268",  144 },
            { L_, 2.9732900231086697920562855e+177, "297329002310867",  178 },
            { L_, 2.9739633554546597365457912e+092, "297396335545466",   93 },
            { L_, 2.9750918365401700901255259e+293, "297509183654017",  294 },
            { L_, 2.9751291066773902447390695e+166, "297512910667739",  167 },
            { L_, 2.9781799672098198403044265e-282, "297817996720982", -281 },
            { L_, 2.9845137711517998971099419e-010,  "29845137711518",   -9 },
            { L_, 2.9856060736643098102308156e+195, "298560607366431",  196 },
            { L_, 2.9874276387385101499530824e+179, "298742763873851",  180 },
            { L_, 2.9880033389040198326110840e+009, "298800333890402",   10 },
            { L_, 2.9882925004139197785522173e-057, "298829250041392",  -56 },
            { L_, 2.9888909234029600661241836e+053, "298889092340296",   54 },
            { L_, 2.9897523827774698818636743e+091, "298975238277747",   92 },
            { L_, 2.9901127108110301799611103e+196, "299011271081103",  197 },
            { L_, 2.9923679664778302245644784e+247, "299236796647783",  248 },
            { L_, 2.9925768711648899034403478e+195, "299257687116489",  196 },
            { L_, 2.9927808634098198277010954e-009, "299278086340982",   -8 },
            { L_, 2.9940547024210199249176982e-046, "299405470242102",  -45 },
            { L_, 2.9968982068588801882452717e-091, "299689820685888",  -90 },
            { L_, 2.9979419023068198522067633e-127, "299794190230682", -126 },
            { L_, 2.9979679507013799233114803e+246, "299796795070138",  247 },
            { L_, 2.9991293194204097982625456e-203, "299912931942041", -202 },
            { L_, 2.9993690628146001030397037e-089,  "29993690628146",  -88 },
            { L_, 3.0032613797901901108848206e+272, "300326137979019",  273 },
            { L_, 3.0045142911704899120341187e+075, "300451429117049",   76 },
            { L_, 3.0115173486980701100268646e-099, "301151734869807",  -98 },
            { L_, 3.0128394397231501996490993e-177, "301283943972315", -176 },
            { L_, 3.0142581619160400846647723e-011, "301425816191604",  -10 },
            { L_, 3.0160341996584301042473112e-052, "301603419965843",  -51 },
            { L_, 3.0181905919983499904678310e-069, "301819059199835",  -68 },
            { L_, 3.0196471430843399675440635e+128, "301964714308434",  129 },
            { L_, 3.0204972725014901813607781e-043, "302049727250149",  -42 },
            { L_, 3.0216652590659199955558534e+115, "302166525906592",  116 },
            { L_, 3.0241900468451499205682541e+150, "302419004684515",  151 },
            { L_, 3.0246663822377698668045271e-069, "302466638223777",  -68 },
            { L_, 3.0280410477804300596455663e+084, "302804104778043",   85 },
            { L_, 3.0284289573335998177496235e-162,  "30284289573336", -161 },
            { L_, 3.0329163961274397305988621e+263, "303291639612744",  264 },
            { L_, 3.0357876564650298442490077e+303, "303578765646503",  304 },
            { L_, 3.0368284447339202301148108e-287, "303682844473392", -286 },
            { L_, 3.0382669452887501323231590e-016, "303826694528875",  -15 },
            { L_, 3.0407070057976098479544660e+271, "304070700579761",  272 },
            { L_, 3.0434177402776099161307640e+166, "304341774027761",  167 },
            { L_, 3.0442903872705002051820655e-267,  "30442903872705", -266 },
            { L_, 3.0456674792460897600452740e-147, "304566747924609", -146 },
            { L_, 3.0457939418004402843353791e+244, "304579394180044",  245 },
            { L_, 3.0477304418518199248692629e-223, "304773044185182", -222 },
            { L_, 3.0490447811600001253275512e-280,    "304904478116", -279 },
            { L_, 3.0495391725748499124468933e-253, "304953917257485", -252 },
            { L_, 3.0554457216671098588297378e-186, "305544572166711", -185 },
            { L_, 3.0588130156123802556344063e+247, "305881301561238",  248 },
            { L_, 3.0608270245917199125963170e-101, "306082702459172", -100 },
            { L_, 3.0622236683783200641700293e+098, "306222366837832",   99 },
            { L_, 3.0624660823077899803184515e+112, "306246608230779",  113 },
            { L_, 3.0636737814398200391441225e+053, "306367378143982",   54 },
            { L_, 3.0649846396727401649697472e+159, "306498463967274",  160 },
            { L_, 3.0668172921395300859418408e-206, "306681729213953", -205 },
            { L_, 3.0677083519967800800148011e+075, "306770835199678",   76 },
            { L_, 3.0677579217680897897039852e-154, "306775792176809", -153 },
            { L_, 3.0698143883481498495401921e+158, "306981438834815",  159 },
            { L_, 3.0698810726074800486452387e+050, "306988107260748",   51 },
            { L_, 3.0734079303786099608154642e-215, "307340793037861", -214 },
            { L_, 3.0737297790748001764106853e-135,  "30737297790748", -134 },
            { L_, 3.0751479775318998667049455e-040,  "30751479775319",  -39 },
            { L_, 3.0772315945267598888006079e+297, "307723159452676",  298 },
            { L_, 3.0807914693325101732816080e-079, "308079146933251",  -78 },
            { L_, 3.0816044660203097084297657e-073, "308160446602031",  -72 },
            { L_, 3.0816608926158798782948314e-160, "308166089261588", -159 },
            { L_, 3.0838130406964101692443318e-271, "308381304069641", -270 },
            { L_, 3.0850943576555698664666946e-048, "308509435765557",  -47 },
            { L_, 3.0862847987385199539119336e+116, "308628479873852",  117 },
            { L_, 3.0863117653312099932487050e+140, "308631176533121",  141 },
            { L_, 3.0903653250263001021756426e-057,  "30903653250263",  -56 },
            { L_, 3.0906575984047498361739846e+300, "309065759840475",  301 },
            { L_, 3.0907965512179701986829651e+065, "309079655121797",   66 },
            { L_, 3.0927420407411100685185980e+034, "309274204074111",   35 },
            { L_, 3.0940234551294102676834591e+114, "309402345512941",  115 },
            { L_, 3.0940663482048200547524856e+203, "309406634820482",  204 },
            { L_, 3.0941495720058200463262070e-055, "309414957200582",  -54 },
            { L_, 3.0958368347469597635938165e-300, "309583683474696", -299 },
            { L_, 3.0971925088659400206337272e+214, "309719250886594",  215 },
            { L_, 3.0975890235437697782465400e-156, "309758902354377", -155 },
            { L_, 3.1017078016347798911318738e-034, "310170780163478",  -33 },
            { L_, 3.1024391905413102001282472e+227, "310243919054131",  228 },
            { L_, 3.1027972164480999245995870e-294,  "31027972164481", -293 },
            { L_, 3.1030633069555300933855626e+048, "310306330695553",   49 },
            { L_, 3.1050690819025501219437147e+116, "310506908190255",  117 },
            { L_, 3.1089491142331701425809792e-145, "310894911423317", -144 },
            { L_, 3.1094322164654601490599076e-108, "310943221646546", -107 },
            { L_, 3.1101250371314100267489882e-175, "311012503713141", -174 },
            { L_, 3.1129537651945100615565721e+047, "311295376519451",   48 },
            { L_, 3.1150011916844901962608138e+034, "311500119168449",   35 },
            { L_, 3.1161191250310201621587371e-191, "311611912503102", -190 },
            { L_, 3.1165502537365098559929036e-013, "311655025373651",  -12 },
            { L_, 3.1179163868202201779860250e-051, "311791638682022",  -50 },
            { L_, 3.1228927433970000131813527e-047,   "3122892743397",  -46 },
            { L_, 3.1230752082110003120904872e+278,   "3123075208211",  279 },
            { L_, 3.1245944580447802356246357e+294, "312459445804478",  295 },
            { L_, 3.1278437627259800877539637e+084, "312784376272598",   85 },
            { L_, 3.1278449164591299505332475e+137, "312784491645913",  138 },
            { L_, 3.1287550329363800241206037e-252, "312875503293638", -251 },
            { L_, 3.1297253250876101758688193e+105, "312972532508761",  106 },
            { L_, 3.1321807640729801314172819e-228, "313218076407298", -227 },
            { L_, 3.1367778709596097552225865e+120, "313677787095961",  121 },
            { L_, 3.1415039495434303077780054e-216, "314150394954343", -215 },
            { L_, 3.1418172791372600537743282e-067, "314181727913726",  -66 },
            { L_, 3.1423435047509701709723002e-055, "314234350475097",  -54 },
            { L_, 3.1427175944831597802693594e-237, "314271759448316", -236 },
            { L_, 3.1429793261248099878830891e-158, "314297932612481", -157 },
            { L_, 3.1437559781603301584309409e+291, "314375597816033",  292 },
            { L_, 3.1445672837742799750065062e-126, "314456728377428", -125 },
            { L_, 3.1451046991969198272276235e+233, "314510469919692",  234 },
            { L_, 3.1460465651605800739187097e-071, "314604656516058",  -70 },
            { L_, 3.1460663818059600990648287e+035, "314606638180596",   36 },
            { L_, 3.1465288088245699741473298e+114, "314652880882457",  115 },
            { L_, 3.1467673205122197196185834e+306, "314676732051222",  307 },
            { L_, 3.1491894958878798970155460e+110, "314918949588788",  111 },
            { L_, 3.1513337674977600093171743e+273, "315133376749776",  274 },
            { L_, 3.1519676264560399631809836e+063, "315196762645604",   64 },
            { L_, 3.1537027738646100662367919e-119, "315370277386461", -118 },
            { L_, 3.1547224955841201443892783e+127, "315472249558412",  128 },
            { L_, 3.1553516241750299220455854e-255, "315535162417503", -254 },
            { L_, 3.1562604223000500638998036e-273, "315626042230005", -272 },
            { L_, 3.1596961735802702749722765e+241, "315969617358027",  242 },
            { L_, 3.1603359373301998123718047e-133,  "31603359373302", -132 },
            { L_, 3.1614463514532099243615374e+221, "316144635145321",  222 },
            { L_, 3.1639582316270000427111424e+261,   "3163958231627",  262 },
            { L_, 3.1656504112609197916697692e-035, "316565041126092",  -34 },
            { L_, 3.1659798808130202888009696e-278, "316597988081302", -277 },
            { L_, 3.1664978142084402226462585e+251, "316649781420844",  252 },
            { L_, 3.1721818857796298453792190e+236, "317218188577963",  237 },
            { L_, 3.1728679885793500208854281e-027, "317286798857935",  -26 },
            { L_, 3.1802048906469599018958635e-144, "318020489064696", -143 },
            { L_, 3.1821574651077198458041766e-052, "318215746510772",  -51 },
            { L_, 3.1832891957486901439181794e+162, "318328919574869",  163 },
            { L_, 3.1840412224494598126032513e-283, "318404122244946", -282 },
            { L_, 3.1847641032380897686004066e-054, "318476410323809",  -53 },
            { L_, 3.1878943110572001355366400e+023,  "31878943110572",   24 },
            { L_, 3.1892116192351199375068491e+063, "318921161923512",   64 },
            { L_, 3.1923023878675898504039024e-204, "319230238786759", -203 },
            { L_, 3.1923432407569998502731323e+008,   "3192343240757",    9 },
            { L_, 3.1925484782341602231321619e+207, "319254847823416",  208 },
            { L_, 3.1967523209397298343635354e+031, "319675232093973",   32 },
            { L_, 3.1999114646309000862287966e-140,  "31999114646309", -139 },
            { L_, 3.2011589418695800234752904e+207, "320115894186958",  208 },
            { L_, 3.2017794955762901847761778e+145, "320177949557629",  146 },
            { L_, 3.2018655987259098305648872e+214, "320186559872591",  215 },
            { L_, 3.2025184219491900039357214e+217, "320251842194919",  218 },
            { L_, 3.2036079734281098639478164e-022, "320360797342811",  -21 },
            { L_, 3.2046216876118899302305355e-204, "320462168761189", -203 },
            { L_, 3.2070308483886198650116148e-219, "320703084838862", -218 },
            { L_, 3.2076959526695200638997693e+276, "320769595266952",  277 },
            { L_, 3.2085650065187202446794000e-169, "320856500651872", -168 },
            { L_, 3.2087600465898399727970004e-253, "320876004658984", -252 },
            { L_, 3.2119243475565299977806228e-105, "321192434755653", -104 },
            { L_, 3.2119965404697301814715886e-236, "321199654046973", -235 },
            { L_, 3.2129556136625498653253327e+215, "321295561366255",  216 },
            { L_, 3.2144514564308798736179157e-031, "321445145643088",  -30 },
            { L_, 3.2148928609710900661053190e+121, "321489286097109",  122 },
            { L_, 3.2190774642773500633258634e-101, "321907746427735", -100 },
            { L_, 3.2193745503068401050437086e-161, "321937455030684", -160 },
            { L_, 3.2222058756728598330741403e+286, "322220587567286",  287 },
            { L_, 3.2255208323497698487131387e-211, "322552083234977", -210 },
            { L_, 3.2263485560558502918925160e+060, "322634855605585",   61 },
            { L_, 3.2318929748695298319309173e+115, "323189297486953",  116 },
            { L_, 3.2323119241086601002770400e+091, "323231192410866",   92 },
            { L_, 3.2331936119741000089526243e-257,  "32331936119741", -256 },
            { L_, 3.2388428771321798793909451e-106, "323884287713218", -105 },
            { L_, 3.2397769883364601030446896e-300, "323977698833646", -299 },
            { L_, 3.2417565903700697256460490e+197, "324175659037007",  198 },
            { L_, 3.2422084866487401754872407e-095, "324220848664874",  -94 },
            { L_, 3.2423630733363399164687344e+164, "324236307333634",  165 },
            { L_, 3.2429926448865099969970757e-014, "324299264488651",  -13 },
            { L_, 3.2481237358174900338800470e-095, "324812373581749",  -94 },
            { L_, 3.2483121344294798150437867e+082, "324831213442948",   83 },
            { L_, 3.2494936078585400720692497e+134, "324949360785854",  135 },
            { L_, 3.2495305171596897599912587e-250, "324953051715969", -249 },
            { L_, 3.2507266487886301253222440e-140, "325072664878863", -139 },
            { L_, 3.2534086757336196750049308e-176, "325340867573362", -175 },
            { L_, 3.2552605047215098664892362e+095, "325526050472151",   96 },
            { L_, 3.2554756714361899707358901e-142, "325547567143619", -141 },
            { L_, 3.2559772404715499573837280e+240, "325597724047155",  241 },
            { L_, 3.2570658695788199886831037e+239, "325706586957882",  240 },
            { L_, 3.2579388897916302248518815e-008, "325793888979163",   -7 },
            { L_, 3.2608965449032200000000000e+014, "326089654490322",   15 },
            { L_, 3.2632370581541799344701148e+106, "326323705815418",  107 },
            { L_, 3.2641780532434596929111286e-129, "326417805324346", -128 },
            { L_, 3.2647249648472098622652103e-292, "326472496484721", -291 },
            { L_, 3.2688564546820102687819975e+266, "326885645468201",  267 },
            { L_, 3.2693003682659701780721662e+002, "326930036826597",    3 },
            { L_, 3.2702600943305098539062282e+232, "327026009433051",  233 },
            { L_, 3.2711886069423296834561400e-207, "327118860694233", -206 },
            { L_, 3.2719920333293398045733780e+117, "327199203332934",  118 },
            { L_, 3.2754614231366297219988451e-086, "327546142313663",  -85 },
            { L_, 3.2756139819006900582021302e-002, "327561398190069",   -1 },
            { L_, 3.2787809480639998712709293e+201,   "3278780948064",  202 },
            { L_, 3.2809030037646399181698588e-137, "328090300376464", -136 },
            { L_, 3.2812295851890098622498423e+158, "328122958518901",  159 },
            { L_, 3.2826332898912100869388781e-226, "328263328989121", -225 },
            { L_, 3.2826825996364800405337812e-090, "328268259963648",  -89 },
            { L_, 3.2844724019278799594096032e+268, "328447240192788",  269 },
            { L_, 3.2846972966217796629887517e+032, "328469729662178",   33 },
            { L_, 3.2855462318033898693581369e-015, "328554623180339",  -14 },
            { L_, 3.2866663456149098633065297e+123, "328666634561491",  124 },
            { L_, 3.2886999513720602316419779e+132, "328869995137206",  133 },
            { L_, 3.2890590223377498392354874e+150, "328905902233775",  151 },
            { L_, 3.2916900895658402078866972e-020, "329169008956584",  -19 },
            { L_, 3.2927113155472499807544938e-303, "329271131554725", -302 },
            { L_, 3.2930370618829697974272000e+022, "329303706188297",   23 },
            { L_, 3.2932056580023100072619971e-200, "329320565800231", -199 },
            { L_, 3.2933588864449601468563579e+044, "329335888644496",   45 },
            { L_, 3.2935327246392696968379078e+138, "329353272463927",  139 },
            { L_, 3.2956041975982996590490633e+119,  "32956041975983",  120 },
            { L_, 3.2960937592709801110250264e-265, "329609375927098", -264 },
            { L_, 3.3022155291123798087251124e+204, "330221552911238",  205 },
            { L_, 3.3046218142409101318459108e+048, "330462181424091",   49 },
            { L_, 3.3048342755520099949694538e-216, "330483427555201", -215 },
            { L_, 3.3055716183753102431751813e-113, "330557161837531", -112 },
            { L_, 3.3066734441612301273663763e-186, "330667344416123", -185 },
            { L_, 3.3081392618238001368524692e+000,  "33081392618238",    1 },
            { L_, 3.3091605575187098643818065e+305, "330916055751871",  306 },
            { L_, 3.3092439228154102463251335e-215, "330924392281541", -214 },
            { L_, 3.3095579763392797918625301e+264, "330955797633928",  265 },
            { L_, 3.3135782173342400216519761e+246, "331357821733424",  247 },
            { L_, 3.3137495236843002463922376e+279,  "33137495236843",  280 },
            { L_, 3.3142092058131303322632277e-002, "331420920581313",   -1 },
            { L_, 3.3152810121346001139516628e+237,  "33152810121346",  238 },
            { L_, 3.3169101026919501379873586e+064, "331691010269195",   65 },
            { L_, 3.3186574212828000617851642e-263,  "33186574212828", -262 },
            { L_, 3.3200059851403003058522690e+048,  "33200059851403",   49 },
            { L_, 3.3202267696646802226687504e+272, "332022676966468",  273 },
            { L_, 3.3222602654008297609979642e+085, "332226026540083",   86 },
            { L_, 3.3226685357846997160053632e-058,  "33226685357847",  -57 },
            { L_, 3.3242604732166699949981710e-283, "332426047321667", -282 },
            { L_, 3.3242637521822801364428330e-054, "332426375218228",  -53 },
            { L_, 3.3243171496068898899297577e+119, "332431714960689",  120 },
            { L_, 3.3262195382744498255257158e+147, "332621953827445",  148 },
            { L_, 3.3295365039171302625913078e+027, "332953650391713",   28 },
            { L_, 3.3301349388328002174100223e-154,  "33301349388328", -153 },
            { L_, 3.3313652616036699972878513e+265, "333136526160367",  266 },
            { L_, 3.3316330334714700810705302e-227, "333163303347147", -226 },
            { L_, 3.3318108722767997710907520e-213,  "33318108722768", -212 },
            { L_, 3.3326951580970601008989571e+042, "333269515809706",   43 },
            { L_, 3.3331655544029200195771459e-034, "333316555440292",  -33 },
            { L_, 3.3339794857963697376879771e-098, "333397948579637",  -97 },
            { L_, 3.3354904520619301324204960e-058, "333549045206193",  -57 },
            { L_, 3.3356586092184902580542852e-235, "333565860921849", -234 },
            { L_, 3.3357712912961498441577487e-065, "333577129129615",  -64 },
            { L_, 3.3362574964479798164054682e-211, "333625749644798", -210 },
            { L_, 3.3379570220927599726058899e+213, "333795702209276",  214 },
            { L_, 3.3391309967440501061005386e-096, "333913099674405",  -95 },
            { L_, 3.3424325954687298778079528e-220, "334243259546873", -219 },
            { L_, 3.3424845002383096856828747e+113, "334248450023831",  114 },
            { L_, 3.3425354458029997613277562e-004,   "3342535445803",   -3 },
            { L_, 3.3442770730170297902200052e+260, "334427707301703",  261 },
            { L_, 3.3447371987551797641216000e+023, "334473719875518",   24 },
            { L_, 3.3461653138123700123377346e+127, "334616531381237",  128 },
            { L_, 3.3467605431010701055920865e+181, "334676054310107",  182 },
            { L_, 3.3481977033440498658633226e-054, "334819770334405",  -53 },
            { L_, 3.3523956804543701681997245e+188, "335239568045437",  189 },
            { L_, 3.3532490944098398146607764e+153, "335324909440984",  154 },
            { L_, 3.3557909672554001876806416e-202,  "33557909672554", -201 },
            { L_, 3.3562063789471300413379130e+248, "335620637894713",  249 },
            { L_, 3.3563714052453299200000000e+017, "335637140524533",   18 },
            { L_, 3.3564602750383400373958883e+215, "335646027503834",  216 },
            { L_, 3.3574715398075298070684285e+176, "335747153980753",  177 },
            { L_, 3.3628234403588100808886899e-210, "336282344035881", -209 },
            { L_, 3.3632085942583399794718469e+053, "336320859425834",   54 },
            { L_, 3.3703096755266600965481631e+280, "337030967552666",  281 },
            { L_, 3.3704567606846297832884401e+037, "337045676068463",   38 },
            { L_, 3.3712426574917498201251918e-195, "337124265749175", -194 },
            { L_, 3.3715297041716000545119339e-261,  "33715297041716", -260 },
            { L_, 3.3752316163423499825471901e+244, "337523161634235",  245 },
            { L_, 3.3761154018497398059543272e+175, "337611540184974",  176 },
            { L_, 3.3772699433362299836645309e+162, "337726994333623",  163 },
            { L_, 3.3786407358572498500363610e+160, "337864073585725",  161 },
            { L_, 3.3790132236870102133651794e+195, "337901322368701",  196 },
            { L_, 3.3794331904606598240594297e+028, "337943319046066",   29 },
            { L_, 3.3806131561623599603161277e+103, "338061315616236",  104 },
            { L_, 3.3818859359816598269116706e-019, "338188593598166",  -18 },
            { L_, 3.3823469687800899835283787e+136, "338234696878009",  137 },
            { L_, 3.3832323136763196891827302e-275, "338323231367632", -274 },
            { L_, 3.3834184032465201177639760e+035, "338341840324652",   36 },
            { L_, 3.3856425819401900358550715e+129, "338564258194019",  130 },
            { L_, 3.3863169967561399793856704e-013, "338631699675614",  -12 },
            { L_, 3.3889139892657500115953495e+296, "338891398926575",  297 },
            { L_, 3.3897165779774098327211482e+251, "338971657797741",  252 },
            { L_, 3.3931097779213401880543462e-277, "339310977792134", -276 },
            { L_, 3.3951518766539198033787096e-201, "339515187665392", -200 },
            { L_, 3.3983766003990400075652323e-245, "339837660039904", -244 },
            { L_, 3.4003029170754301094762661e-047, "340030291707543",  -46 },
            { L_, 3.4005966712106499276214773e-079, "340059667121065",  -78 },
            { L_, 3.4017725118039699456000000e+019, "340177251180397",   20 },
            { L_, 3.4020559218649401199330828e+263, "340205592186494",  264 },
            { L_, 3.4038126335480602408656248e+263, "340381263354806",  264 },
            { L_, 3.4080516537047400300615174e+293, "340805165370474",  294 },
            { L_, 3.4099374069795901403774263e+067, "340993740697959",   68 },
            { L_, 3.4105732689168299141229568e+274, "341057326891683",  275 },
            { L_, 3.4110335275553398236765025e-156, "341103352755534", -155 },
            { L_, 3.4111689652759700177815257e-261, "341116896527597", -260 },
            { L_, 3.4125928289734299564033190e-105, "341259282897343", -104 },
            { L_, 3.4172203404749202675731648e+222, "341722034047492",  223 },
            { L_, 3.4175757430414600060242292e+269, "341757574304146",  270 },
            { L_, 3.4176596768437399172830080e-165, "341765967684374", -164 },
            { L_, 3.4211059333550898162509531e-220, "342110593335509", -219 },
            { L_, 3.4251791043721102214264154e+043, "342517910437211",   44 },
            { L_, 3.4263070429713903109049523e-064, "342630704297139",  -63 },
            { L_, 3.4267600011573303478697423e+234, "342676000115733",  235 },
            { L_, 3.4272684218828400906213182e-141, "342726842188284", -140 },
            { L_, 3.4279181510062097535687902e-083, "342791815100621",  -82 },
            { L_, 3.4303116990368301804782635e+204, "343031169903683",  205 },
            { L_, 3.4305803548568900775838761e+130, "343058035485689",  131 },
            { L_, 3.4374683316785097995605127e+178, "343746833167851",  179 },
            { L_, 3.4379605852999198135387878e-266, "343796058529992", -265 },
            { L_, 3.4386123794502101204654615e+227, "343861237945021",  228 },
            { L_, 3.4404982106110702405939933e+293, "344049821061107",  294 },
            { L_, 3.4413832032390299772844175e-218, "344138320323903", -217 },
            { L_, 3.4450448233101097620238872e-243, "344504482331011", -242 },
            { L_, 3.4452887071679402603187598e+222, "344528870716794",  223 },
            { L_, 3.4472126360636697207029715e+262, "344721263606367",  263 },
            { L_, 3.4476634997634997494110373e-240,  "34476634997635", -239 },
            { L_, 3.4496805997698201930813313e-141, "344968059976982", -140 },
            { L_, 3.4522788309028498109246621e-154, "345227883090285", -153 },
            { L_, 3.4524005085287398761693094e+157, "345240050852874",  158 },
            { L_, 3.4543412770357399592298376e+038, "345434127703574",   39 },
            { L_, 3.4552652820698102808987297e+184, "345526528206981",  185 },
            { L_, 3.4555386207492401246965951e-043, "345553862074924",  -42 },
            { L_, 3.4565746621012097695302806e-024, "345657466210121",  -23 },
            { L_, 3.4568072474300902303292883e+063, "345680724743009",   64 },
            { L_, 3.4580584534751198498807887e+280, "345805845347512",  281 },
            { L_, 3.4586830117887402121498370e-295, "345868301178874", -294 },
            { L_, 3.4589713212055098265409470e+006, "345897132120551",    7 },
            { L_, 3.4602400885252202410433918e+257, "346024008852522",  258 },
            { L_, 3.4611179384666901364720438e+102, "346111793846669",  103 },
            { L_, 3.4634852958003099506383966e+229, "346348529580031",  230 },
            { L_, 3.4654041428817900942631163e-186, "346540414288179", -185 },
            { L_, 3.4668530276513701385590897e-137, "346685302765137", -136 },
            { L_, 3.4691837350999003349928425e+169,  "34691837350999",  170 },
            { L_, 3.4698409486890601562500000e+013, "346984094868906",   14 },
            { L_, 3.4715062938822101847036437e+249, "347150629388221",  250 },
            { L_, 3.4729840661243699572637195e-085, "347298406612437",  -84 },
            { L_, 3.4742643895591498613637626e+302, "347426438955915",  303 },
            { L_, 3.4755973412550102964189468e-151, "347559734125501", -150 },
            { L_, 3.4794594146794700669788453e-081, "347945941467947",  -80 },
            { L_, 3.4795011580242797930330718e+028, "347950115802428",   29 },
            { L_, 3.4797673589385101077060885e-196, "347976735893851", -195 },
            { L_, 3.4808826335295198360065421e+267, "348088263352952",  268 },
            { L_, 3.4816616191805301428926841e+089, "348166161918053",   90 },
            { L_, 3.4849952320852498220380764e-230, "348499523208525", -229 },
            { L_, 3.4857186260720499149101670e+002, "348571862607205",    3 },
            { L_, 3.4880013263186399479174240e+289, "348800132631864",  290 },
            { L_, 3.4880809038164299880264840e-034, "348808090381643",  -33 },
            { L_, 3.4890362991310100232392808e+202, "348903629913101",  203 },
            { L_, 3.4907766238916399324364748e-006, "349077662389164",   -5 },
            { L_, 3.4912440729944098861089459e-181, "349124407299441", -180 },
            { L_, 3.4948642330085100924691202e+281, "349486423300851",  282 },
            { L_, 3.4972364128879598244352483e-244, "349723641288796", -243 },
            { L_, 3.5030142731868997996882428e+028,  "35030142731869",   29 },
            { L_, 3.5038884960507000906597703e+043,  "35038884960507",   44 },
            { L_, 3.5049792246958199908342221e+052, "350497922469582",   53 },
            { L_, 3.5066244315213000961892513e-186,  "35066244315213", -185 },
            { L_, 3.5078468251241198056298913e+199, "350784682512412",  200 },
            { L_, 3.5082880300391401155584144e-032, "350828803003914",  -31 },
            { L_, 3.5086636772789098905600000e+020, "350866367727891",   21 },
            { L_, 3.5112370317936799562283009e+106, "351123703179368",  107 },
            { L_, 3.5139916993740700948680929e-215, "351399169937407", -214 },
            { L_, 3.5141037809145299520737862e-165, "351410378091453", -164 },
            { L_, 3.5153339061799101869970653e-117, "351533390617991", -116 },
            { L_, 3.5159194540831096778892769e+200, "351591945408311",  201 },
            { L_, 3.5182072437508899845172019e+216, "351820724375089",  217 },
            { L_, 3.5192823660087601177037215e-177, "351928236600876", -176 },
            { L_, 3.5202925030932197986382550e+068, "352029250309322",   69 },
            { L_, 3.5229423122093703125000000e+013, "352294231220937",   14 },
            { L_, 3.5264037782218600726139020e-135, "352640377822186", -134 },
            { L_, 3.5267332131663800252907901e+075, "352673321316638",   76 },
            { L_, 3.5312621242332003049688707e+187,  "35312621242332",  188 },
            { L_, 3.5333835686123802568681404e-179, "353338356861238", -178 },
            { L_, 3.5379559354544497781247784e+116, "353795593545445",  117 },
            { L_, 3.5393262070735998822227198e-189,  "35393262070736", -188 },
            { L_, 3.5396895461615902103466162e+089, "353968954616159",   90 },
            { L_, 3.5399784026920001291463924e-087,   "3539978402692",  -86 },
            { L_, 3.5409245312486999395971544e-145,  "35409245312487", -144 },
            { L_, 3.5444778302370101701914746e-212, "354447783023701", -211 },
            { L_, 3.5465540919226698841315355e+282, "354655409192267",  283 },
            { L_, 3.5466924001446500524282626e-167, "354669240014465", -166 },
            { L_, 3.5467908658837799862340034e+001, "354679086588378",    2 },
            { L_, 3.5468225742795698811791710e-093, "354682257427957",  -92 },
            { L_, 3.5473974915744500470044011e-237, "354739749157445", -236 },
            { L_, 3.5503273491464298954752000e+025, "355032734914643",   26 },
            { L_, 3.5511050349510699386984307e+058, "355110503495107",   59 },
            { L_, 3.5539149324639597095996781e+138, "355391493246396",  139 },
            { L_, 3.5560713290002499785769981e-273, "355607132900025", -272 },
            { L_, 3.5572812903483601663595917e-125, "355728129034836", -124 },
            { L_, 3.5594775967029497996458524e+174, "355947759670295",  175 },
            { L_, 3.5596217022077400282643883e+264, "355962170220774",  265 },
            { L_, 3.5616399434861999177096862e-059,  "35616399434862",  -58 },
            { L_, 3.5649221467937801612328454e-011, "356492214679378",  -10 },
            { L_, 3.5663469561124803271278421e+293, "356634695611248",  294 },
            { L_, 3.5666328699354998358221618e-040,  "35666328699355",  -39 },
            { L_, 3.5667082999864399475376370e+103, "356670829998644",  104 },
            { L_, 3.5675239253809799029383687e-096, "356752392538098",  -95 },
            { L_, 3.5712027536131099950520486e-065, "357120275361311",  -64 },
            { L_, 3.5712943255408297213264381e+191, "357129432554083",  192 },
            { L_, 3.5719194029229399934408321e+252, "357191940292294",  253 },
            { L_, 3.5726088201330898483215223e-118, "357260882013309", -117 },
            { L_, 3.5738044383934198513705360e-004, "357380443839342",   -3 },
            { L_, 3.5742229631366699941391340e-275, "357422296313667", -274 },
            { L_, 3.5743476545272798900542923e-122, "357434765452728", -121 },
            { L_, 3.5765718260197299016812457e-035, "357657182601973",  -34 },
            { L_, 3.5795936825929301178647155e-025, "357959368259293",  -24 },
            { L_, 3.5837445225017799817689844e+151, "358374452250178",  152 },
            { L_, 3.5886263139178299722712718e-177, "358862631391783", -176 },
            { L_, 3.5904305194677201534044055e-246, "359043051946772", -245 },
            { L_, 3.5907048879972197069914850e-157, "359070488799722", -156 },
            { L_, 3.5928165850093098123357464e+093, "359281658500931",   94 },
            { L_, 3.5935531769026996998768335e+138,  "35935531769027",  139 },
            { L_, 3.5938136300485300983020512e+245, "359381363004853",  246 },
            { L_, 3.5952927404363798761474570e-282, "359529274043638", -281 },
            { L_, 3.5970214887773898559127092e-210, "359702148877739", -209 },
            { L_, 3.5971915190276700012109485e-097, "359719151902767",  -96 },
            { L_, 3.5986204079546201062948118e-036, "359862040795462",  -35 },
            { L_, 3.5998492722409597676090721e+154, "359984927224096",  155 },
            { L_, 3.6001559207680703141819102e-213, "360015592076807", -212 },
            { L_, 3.6002093091553901346477770e-267, "360020930915539", -266 },
            { L_, 3.6003515002668701619049836e+182, "360035150026687",  183 },
            { L_, 3.6033762024734698837486184e+130, "360337620247347",  131 },
            { L_, 3.6042893738569001957687395e+255,  "36042893738569",  256 },
            { L_, 3.6052999790023898815922547e-283, "360529997900239", -282 },
            { L_, 3.6079772884021697906852172e+269, "360797728840217",  270 },
            { L_, 3.6101176137115599610278524e+035, "361011761371156",   36 },
            { L_, 3.6121761574198402440597380e-278, "361217615741984", -277 },
            { L_, 3.6142508774971303032369149e-111, "361425087749713", -110 },
            { L_, 3.6144164984188598562046976e+224, "361441649841886",  225 },
            { L_, 3.6152095641676898559912031e+215, "361520956416769",  216 },
            { L_, 3.6154637129595701236098915e-035, "361546371295957",  -34 },
            { L_, 3.6157954869187602443027857e+266, "361579548691876",  267 },
            { L_, 3.6164196409423701119193287e-098, "361641964094237",  -97 },
            { L_, 3.6177903280838397132334981e-092, "361779032808384",  -91 },
            { L_, 3.6181527689597400348460996e-012, "361815276895974",  -11 },
            { L_, 3.6182581314037701062804003e-270, "361825813140377", -269 },
            { L_, 3.6206077726696497169805562e-281, "362060777266965", -280 },
            { L_, 3.6258566951466798990683272e-126, "362585669514668", -125 },
            { L_, 3.6268948396300401105189748e+129, "362689483963004",  130 },
            { L_, 3.6302521031526697914095095e+073, "363025210315267",   74 },
            { L_, 3.6312112007963397819303637e+172, "363121120079634",  173 },
            { L_, 3.6313368151400299443407587e+058, "363133681514003",   59 },
            { L_, 3.6327859177436997273356162e+277,  "36327859177437",  278 },
            { L_, 3.6334215820810798035436985e+092, "363342158208108",   93 },
            { L_, 3.6341989927740303161396040e-282, "363419899277403", -281 },
            { L_, 3.6343997221776499770247489e+219, "363439972217765",  220 },
            { L_, 3.6374033492785702850271726e-048, "363740334927857",  -47 },
            { L_, 3.6407916363289202008648456e+046, "364079163632892",   47 },
            { L_, 3.6445949620134999520322764e-221,  "36445949620135", -220 },
            { L_, 3.6487329440642201356274952e-228, "364873294406422", -227 },
            { L_, 3.6487797191044696457484895e+128, "364877971910447",  129 },
            { L_, 3.6516149260540701731616420e-067, "365161492605407",  -66 },
            { L_, 3.6549310559632103491715677e-105, "365493105596321", -104 },
            { L_, 3.6560669794669301096691069e-162, "365606697946693", -161 },
            { L_, 3.6561589737552601981722127e-040, "365615897375526",  -39 },
            { L_, 3.6577391905339000406284970e+092,  "36577391905339",   93 },
            { L_, 3.6584391549359897600000000e+017, "365843915493599",   18 },
            { L_, 3.6621826303985503198934982e+221, "366218263039855",  222 },
            { L_, 3.6628392709346801375529586e+173, "366283927093468",  174 },
            { L_, 3.6648437251356202866082584e+175, "366484372513562",  176 },
            { L_, 3.6661051597895799053959734e+291, "366610515978958",  292 },
            { L_, 3.6673569979043199363793472e+152, "366735699790432",  153 },
            { L_, 3.6693309263984900696848035e+195, "366933092639849",  196 },
            { L_, 3.6694743298262697496269938e+296, "366947432982627",  297 },
            { L_, 3.6721566556540402612933650e-188, "367215665565404", -187 },
            { L_, 3.6748257685892897653571611e-079, "367482576858929",  -78 },
            { L_, 3.6764415786293198106917417e-214, "367644157862932", -213 },
            { L_, 3.6766739336775401963647961e-136, "367667393367754", -135 },
            { L_, 3.6775293630310601344530165e+233, "367752936303106",  234 },
            { L_, 3.6778040896192097775884654e+102, "367780408961921",  103 },
            { L_, 3.6800245179634999706118417e+117,  "36800245179635",  118 },
            { L_, 3.6802152966343102702255322e-278, "368021529663431", -277 },
            { L_, 3.6804552362848400799265136e-302, "368045523628484", -301 },
            { L_, 3.6821754329665698310641947e-127, "368217543296657", -126 },
            { L_, 3.6833096649091097207498848e+154, "368330966490911",  155 },
            { L_, 3.6848810341701602076971730e-250, "368488103417016", -249 },
            { L_, 3.6880099186715101547392014e+118, "368800991867151",  119 },
            { L_, 3.6907913986954899937988016e+084, "369079139869549",   85 },
            { L_, 3.6910984744952501908891014e-043, "369109847449525",  -42 },
            { L_, 3.6927763673884103047957854e+141, "369277636738841",  142 },
            { L_, 3.6979840691625602915028537e+107, "369798406916256",  108 },
            { L_, 3.7006864374161297911888868e-184, "370068643741613", -183 },
            { L_, 3.7028142068792298143053736e-301, "370281420687923", -300 },
            { L_, 3.7037451327319097454928990e+157, "370374513273191",  158 },
            { L_, 3.7060732052326999207830779e+227,  "37060732052327",  228 },
            { L_, 3.7074606408105503066617890e-095, "370746064081055",  -94 },
            { L_, 3.7105831867382901293572040e+147, "371058318673829",  148 },
            { L_, 3.7129358676425799207232025e-145, "371293586764258", -144 },
            { L_, 3.7153676512964497369129079e-029, "371536765129645",  -28 },
            { L_, 3.7172285645173200583840387e-137, "371722856451732", -136 },
            { L_, 3.7176922788092099381630572e+064, "371769227880921",   65 },
            { L_, 3.7187174642281000892001089e+300,  "37187174642281",  301 },
            { L_, 3.7188778904751799439810850e-113, "371887789047518", -112 },
            { L_, 3.7191868758917901999105148e-303, "371918687589179", -302 },
            { L_, 3.7193841114487602733870964e+126, "371938411144876",  127 },
            { L_, 3.7202618977570598550801142e-227, "372026189775706", -226 },
            { L_, 3.7239228932696800629710812e-067, "372392289326968",  -66 },
            { L_, 3.7255130199741297791544914e-035, "372551301997413",  -34 },
            { L_, 3.7255502998862403207917957e-173, "372555029988624", -172 },
            { L_, 3.7292102671168698696059048e+046, "372921026711687",   47 },
            { L_, 3.7299340830804099693607171e+164, "372993408308041",  165 },
            { L_, 3.7324390591157399136755732e+115, "373243905911574",  116 },
            { L_, 3.7330125058111201145822958e+152, "373301250581112",  153 },
            { L_, 3.7337054157777900810555561e+244, "373370541577779",  245 },
            { L_, 3.7356152157695601703782800e-299, "373561521576956", -298 },
            { L_, 3.7361052007247596990286342e-117, "373610520072476", -116 },
            { L_, 3.7372101734297899459651030e-292, "373721017342979", -291 },
            { L_, 3.7379658756025300672768805e-074, "373796587560253",  -73 },
            { L_, 3.7391577142751202944054409e-191, "373915771427512", -190 },
            { L_, 3.7441012171269398799329224e+307, "374410121712694",  308 },
            { L_, 3.7447773013734301851713434e-286, "374477730137343", -285 },
            { L_, 3.7454040747635999583584084e-230,  "37454040747636", -229 },
            { L_, 3.7456970982028798014103049e-086, "374569709820288",  -85 },
            { L_, 3.7461200229931601462565729e-300, "374612002299316", -299 },
            { L_, 3.7465119115026099237956290e+232, "374651191150261",  233 },
            { L_, 3.7474158408936899106186048e-276, "374741584089369", -275 },
            { L_, 3.7483734454561602640885231e-061, "374837344545616",  -60 },
            { L_, 3.7491443029882498083571888e-289, "374914430298825", -288 },
            { L_, 3.7516711631393099643746017e+171, "375167116313931",  172 },
            { L_, 3.7527852096010201479177167e-273, "375278520960102", -272 },
            { L_, 3.7545376946507902915745581e-073, "375453769465079",  -72 },
            { L_, 3.7587182468424695418288325e-309, "375871824684247", -308 },
            { L_, 3.7606466745366700027395582e-172, "376064667453667", -171 },
            { L_, 3.7640145978701100575116762e-116, "376401459787011", -115 },
            { L_, 3.7648746685004101167487780e+221, "376487466850041",  222 },
            { L_, 3.7649306027463696681928088e+110, "376493060274637",  111 },
            { L_, 3.7662593118826801580833393e-032, "376625931188268",  -31 },
            { L_, 3.7685253378414299230438786e-100, "376852533784143",  -99 },
            { L_, 3.7702925413702800060742664e+303, "377029254137028",  304 },
            { L_, 3.7725726109372700685825776e-225, "377257261093727", -224 },
            { L_, 3.7728002203476901598353755e-131, "377280022034769", -130 },
            { L_, 3.7792803219006698357068895e-044, "377928032190067",  -43 },
            { L_, 3.7804614048218796875986346e-145, "378046140482188", -144 },
            { L_, 3.7810416738341300852206554e-240, "378104167383413", -239 },
            { L_, 3.7821064742399802005653649e+130, "378210647423998",  131 },
            { L_, 3.7833781143006299351438001e-249, "378337811430063", -248 },
            { L_, 3.7851608067044900641517604e-249, "378516080670449", -248 },
            { L_, 3.7852967793500398479756748e-243, "378529677935004", -242 },
            { L_, 3.7871852031797002185758760e+039,  "37871852031797",   40 },
            { L_, 3.7882331591655302168882983e+132, "378823315916553",  133 },
            { L_, 3.7902873375661700048050871e+256, "379028733756617",  257 },
            { L_, 3.7980780000580199335496709e-283, "379807800005802", -282 },
            { L_, 3.7995934609407501120866766e-164, "379959346094075", -163 },
            { L_, 3.8041585071815901397907484e+281, "380415850718159",  282 },
            { L_, 3.8041711081974903147734787e-173, "380417110819749", -172 },
            { L_, 3.8042476077966598501256458e-063, "380424760779666",  -62 },
            { L_, 3.8094065658899499229004364e+259, "380940656588995",  260 },
            { L_, 3.8104061169058799223836162e-237, "381040611690588", -236 },
            { L_, 3.8130881680663201976144399e-017, "381308816806632",  -16 },
            { L_, 3.8147349821464800276782560e-150, "381473498214648", -149 },
            { L_, 3.8160867426470002399619690e+268,   "3816086742647",  269 },
            { L_, 3.8164216212238598794151992e+112, "381642162122386",  113 },
            { L_, 3.8182434885250398037459832e+276, "381824348852504",  277 },
            { L_, 3.8186273410621296391842312e-065, "381862734106213",  -64 },
            { L_, 3.8188801302376698348205616e-260, "381888013023767", -259 },
            { L_, 3.8207944485132499269150393e-081, "382079444851325",  -80 },
            { L_, 3.8267673942670900916319489e+157, "382676739426709",  158 },
            { L_, 3.8310781720828301649152744e-153, "383107817208283", -152 },
            { L_, 3.8342886632129102430375961e+104, "383428866321291",  105 },
            { L_, 3.8424346023913397887905975e-106, "384243460239134", -105 },
            { L_, 3.8435009650410501638070026e+158, "384350096504105",  159 },
            { L_, 3.8442718064262598354565283e-274, "384427180642626", -273 },
            { L_, 3.8446247326111898336943487e+066, "384462473261119",   67 },
            { L_, 3.8452791906457401994140323e+207, "384527919064574",  208 },
            { L_, 3.8459177933022299077008846e-263, "384591779330223", -262 },
            { L_, 3.8474253994604703808263929e-217, "384742539946047", -216 },
            { L_, 3.8485975705894299849627199e+207, "384859757058943",  208 },
            { L_, 3.8494449425489500000000000e+015, "384944494254895",   16 },
            { L_, 3.8498020297642503218337285e+265, "384980202976425",  266 },
            { L_, 3.8509740363836503748462953e-220, "385097403638365", -219 },
            { L_, 3.8521963495907299525465551e+048, "385219634959073",   49 },
            { L_, 3.8552612830266402681511566e+081, "385526128302664",   82 },
            { L_, 3.8556102143077601409235940e-134, "385561021430776", -133 },
            { L_, 3.8588065822617801477961422e+245, "385880658226178",  246 },
            { L_, 3.8623621942586801103923093e+080, "386236219425868",   81 },
            { L_, 3.8638439360802102845340248e+126, "386384393608021",  127 },
            { L_, 3.8638491701908601777493459e-149, "386384917019086", -148 },
            { L_, 3.8657659506690798119800876e+300, "386576595066908",  301 },
            { L_, 3.8659251736984897781477793e-210, "386592517369849", -209 },
            { L_, 3.8678268019206698523145960e-138, "386782680192067", -137 },
            { L_, 3.8688399088211398348753316e-027, "386883990882114",  -26 },
            { L_, 3.8689081822607901473214643e+160, "386890818226079",  161 },
            { L_, 3.8692949119114900743929077e+128, "386929491191149",  129 },
            { L_, 3.8697515345799696274826684e-105, "386975153457997", -104 },
            { L_, 3.8701746439521999652137649e+214,  "38701746439522",  215 },
            { L_, 3.8703400209849696825072756e+290, "387034002098497",  291 },
            { L_, 3.8707062493743400190334877e+176, "387070624937434",  177 },
            { L_, 3.8723137014868301963401712e-044, "387231370148683",  -43 },
            { L_, 3.8758200315431697277771207e-067, "387582003154317",  -66 },
            { L_, 3.8768842719880399243197032e-218, "387688427198804", -217 },
            { L_, 3.8769065654017300513158807e+090, "387690656540173",   91 },
            { L_, 3.8777664069300798350225953e+065, "387776640693008",   66 },
            { L_, 3.8811880682234100483145188e+044, "388118806822341",   45 },
            { L_, 3.8881513511484600613939678e+180, "388815135114846",  181 },
            { L_, 3.8925768700170797559094807e-075, "389257687001708",  -74 },
            { L_, 3.8950139563167298719150968e-027, "389501395631673",  -26 },
            { L_, 3.8951750687751497696672631e-010, "389517506877515",   -9 },
            { L_, 3.8963308385090698493513628e+001, "389633083850907",    2 },
            { L_, 3.8989898066449001829004375e-129,  "38989898066449", -128 },
            { L_, 3.8991435621875400177508440e+134, "389914356218754",  135 },
            { L_, 3.9004184328788199613427385e-291, "390041843287882", -290 },
            { L_, 3.9065453316165498739643171e+153, "390654533161655",  154 },
            { L_, 3.9070390516485000215637490e+151,  "39070390516485",  152 },
            { L_, 3.9107175906277602223757849e-286, "391071759062776", -285 },
            { L_, 3.9146420615507498996007291e-005, "391464206155075",   -4 },
            { L_, 3.9147175780455997353767964e-250,  "39147175780456", -249 },
            { L_, 3.9150708285095797864013691e-108, "391507082850958", -107 },
            { L_, 3.9151277184375601508181367e-101, "391512771843756", -100 },
            { L_, 3.9187283385283799676912277e+145, "391872833852838",  146 },
            { L_, 3.9199100565049000231910307e-243,  "39199100565049", -242 },
            { L_, 3.9201404119432300800000000e+017, "392014041194323",   18 },
            { L_, 3.9226491021351897557474492e+196, "392264910213519",  197 },
            { L_, 3.9244151048655802412829674e+046, "392441510486558",   47 },
            { L_, 3.9244964378026896388607953e-043, "392449643780269",  -42 },
            { L_, 3.9248235386456199307595242e-080, "392482353864562",  -79 },
            { L_, 3.9258016376085801240916106e-084, "392580163760858",  -83 },
            { L_, 3.9266354939042499043428528e+109, "392663549390425",  110 },
            { L_, 3.9277345179200401796224230e+060, "392773451792004",   61 },
            { L_, 3.9334561658759101702380916e-278, "393345616587591", -277 },
            { L_, 3.9414238541192700219394867e-104, "394142385411927", -103 },
            { L_, 3.9424901731574500811157039e+259, "394249017315745",  260 },
            { L_, 3.9435042287768199067820713e-052, "394350422877682",  -51 },
            { L_, 3.9436498194161599714023571e-039, "394364981941616",  -38 },
            { L_, 3.9449968903122301309592399e+200, "394499689031223",  201 },
            { L_, 3.9450224717517197213220994e+200, "394502247175172",  201 },
            { L_, 3.9498939934364701547510235e-177, "394989399343647", -176 },
            { L_, 3.9505783226471799030996878e+195, "395057832264718",  196 },
            { L_, 3.9525180161836798476658118e-140, "395251801618368", -139 },
            { L_, 3.9527752300273701773841613e-271, "395277523002737", -270 },
            { L_, 3.9532754138643301885607723e-163, "395327541386433", -162 },
            { L_, 3.9543125368675098080970946e-053, "395431253686751",  -52 },
            { L_, 3.9564206615983700189241256e-231, "395642066159837", -230 },
            { L_, 3.9582772615957597604973392e+226, "395827726159576",  227 },
            { L_, 3.9598866975676698201219862e+247, "395988669756767",  248 },
            { L_, 3.9607065640080999123682543e+104,  "39607065640081",  105 },
            { L_, 3.9610180370143696853953259e+290, "396101803701437",  291 },
            { L_, 3.9615706280693998041230230e+092,  "39615706280694",   93 },
            { L_, 3.9623048179122301303141574e+254, "396230481791223",  255 },
            { L_, 3.9650266571352900441889292e+175, "396502665713529",  176 },
            { L_, 3.9653699790990296307192381e-139, "396536997909903", -138 },
            { L_, 3.9654324300679201400881477e+304, "396543243006792",  305 },
            { L_, 3.9656314174918497651153315e+120, "396563141749185",  121 },
            { L_, 3.9662023798802696970429100e+085, "396620237988027",   86 },
            { L_, 3.9668686612649898849062360e+074, "396686866126499",   75 },
            { L_, 3.9681658244641102150969574e+241, "396816582446411",  242 },
            { L_, 3.9689955587780402324241929e-169, "396899555877804", -168 },
            { L_, 3.9741212984495298715154416e+160, "397412129844953",  161 },
            { L_, 3.9752132147459698082072914e-277, "397521321474597", -276 },
            { L_, 3.9779225689558499197527372e-289, "397792256895585", -288 },
            { L_, 3.9821645506007297811753825e+162, "398216455060073",  163 },
            { L_, 3.9824162201193098039454237e+074, "398241622011931",   75 },
            { L_, 3.9834695680739397960012814e-022, "398346956807394",  -21 },
            { L_, 3.9841703372540301502240612e-058, "398417033725403",  -57 },
            { L_, 3.9849412314322201487207508e-271, "398494123143222", -270 },
            { L_, 3.9851536297723100931945132e+264, "398515362977231",  265 },
            { L_, 3.9851939690113401600000000e+017, "398519396901134",   18 },
            { L_, 3.9856112264693299365157116e-207, "398561122646933", -206 },
            { L_, 3.9864358362323699610030623e-163, "398643583623237", -162 },
            { L_, 3.9904432883285298493510766e-156, "399044328832853", -155 },
            { L_, 3.9923524471950903267696961e+088, "399235244719509",   89 },
            { L_, 3.9930624090919096679025883e+081, "399306240909191",   82 },
            { L_, 3.9958362819857199523484655e+195, "399583628198572",  196 },
            { L_, 3.9959938288746101443463723e+146, "399599382887461",  147 },
            { L_, 3.9985026020460801734737920e+024, "399850260204608",   25 },
            { L_, 3.9996804404077501967549324e+006, "399968044040775",    7 },
            { L_, 4.0010749818515097001404939e-095, "400107498185151",  -94 },
            { L_, 4.0036436986353102672325505e+072, "400364369863531",   73 },
            { L_, 4.0048466364181500689961420e+210, "400484663641815",  211 },
            { L_, 4.0060991181719898517526568e+277, "400609911817199",  278 },
            { L_, 4.0077173552661497779815270e+296, "400771735526615",  297 },
            { L_, 4.0079174888961901847657089e-037, "400791748889619",  -36 },
            { L_, 4.0080687591359800795831023e+035, "400806875913598",   36 },
            { L_, 4.0089182743948201525114396e-172, "400891827439482", -171 },
            { L_, 4.0095086605398799313194482e+115, "400950866053988",  116 },
            { L_, 4.0097000831161799068798717e-171, "400970008311618", -170 },
            { L_, 4.0099183163335900210798898e+034, "400991831633359",   35 },
            { L_, 4.0136310308977801242857194e+089, "401363103089778",   90 },
            { L_, 4.0151085905502798486706429e+106, "401510859055028",  107 },
            { L_, 4.0164641079723397499003052e+197, "401646410797234",  198 },
            { L_, 4.0179532987970503433908936e+305, "401795329879705",  306 },
            { L_, 4.0214581448655901188142569e-007, "402145814486559",   -6 },
            { L_, 4.0234720740167601176694772e+140, "402347207401676",  141 },
            { L_, 4.0269302501483896832000000e+019, "402693025014839",   20 },
            { L_, 4.0277357572092199794335987e+086, "402773575720922",   87 },
            { L_, 4.0281987506069600664173437e-246, "402819875060696", -245 },
            { L_, 4.0289282878893897788128253e+161, "402892828788939",  162 },
            { L_, 4.0291492439049802131056731e-277, "402914924390498", -276 },
            { L_, 4.0294854179021499582912472e-252, "402948541790215", -251 },
            { L_, 4.0299087197702097186801673e-067, "402990871977021",  -66 },
            { L_, 4.0299830754990100525461091e-171, "402998307549901", -170 },
            { L_, 4.0301355437529301108973865e+113, "403013554375293",  114 },
            { L_, 4.0303220720663499025178243e+271, "403032207206635",  272 },
            { L_, 4.0336900736310798172825568e-048, "403369007363108",  -47 },
            { L_, 4.0362551153231801925471159e+278, "403625511532318",  279 },
            { L_, 4.0395278069080799236309812e-304, "403952780690808", -303 },
            { L_, 4.0402276960257399779696503e+257, "404022769602574",  258 },
            { L_, 4.0406723027516801870951750e-076, "404067230275168",  -75 },
            { L_, 4.0427875670545898971658216e+306, "404278756705459",  307 },
            { L_, 4.0430938390558700117694932e+285, "404309383905587",  286 },
            { L_, 4.0442779001367300676729604e-260, "404427790013673", -259 },
            { L_, 4.0448219857603699292704949e+068, "404482198576037",   69 },
            { L_, 4.0458212084941498505414469e-230, "404582120849415", -229 },
            { L_, 4.0458951198212502002953270e+303, "404589511982125",  304 },
            { L_, 4.0486466465553201511994228e-258, "404864664655532", -257 },
            { L_, 4.0508385417539198308928142e+051, "405083854175392",   52 },
            { L_, 4.0517388237759098863554893e-050, "405173882377591",  -49 },
            { L_, 4.0523381852873999143495949e+047,  "40523381852874",   48 },
            { L_, 4.0526915536254200521924718e+280, "405269155362542",  281 },
            { L_, 4.0548433205283801562634422e-153, "405484332052838", -152 },
            { L_, 4.0567067743355103749259664e-264, "405670677433551", -263 },
            { L_, 4.0581884110691199739389921e-293, "405818841106912", -292 },
            { L_, 4.0594710874215401111052629e-072, "405947108742154",  -71 },
            { L_, 4.0622927283981801971154257e-294, "406229272839818", -293 },
            { L_, 4.0626996296624099443072022e+215, "406269962966241",  216 },
            { L_, 4.0639167392689799201456697e-228, "406391673926898", -227 },
            { L_, 4.0687227709046899529741995e-155, "406872277090469", -154 },
            { L_, 4.0698059084923499920368211e-191, "406980590849235", -190 },
            { L_, 4.0712858603838100751592312e-064, "407128586038381",  -63 },
            { L_, 4.0712924454471597017728531e+275, "407129244544716",  276 },
            { L_, 4.0713183341647599761086786e-120, "407131833416476", -119 },
            { L_, 4.0766784614785001397455456e+199,  "40766784614785",  200 },
            { L_, 4.0796446079136199282385579e+080, "407964460791362",   81 },
            { L_, 4.0800646830751896810330257e+140, "408006468307519",  141 },
            { L_, 4.0804444112510600613838829e+053, "408044441125106",   54 },
            { L_, 4.0812328270834103688530868e-124, "408123282708341", -123 },
            { L_, 4.0846462463966002018400815e+226,  "40846462463966",  227 },
            { L_, 4.0865727161609697142239550e-015, "408657271616097",  -14 },
            { L_, 4.0880491132353398269599419e+192, "408804911323534",  193 },
            { L_, 4.0913130060802400188714713e+272, "409131300608024",  273 },
            { L_, 4.0929117087325598603020844e+148, "409291170873256",  149 },
            { L_, 4.0933064503917101941798965e+150, "409330645039171",  151 },
            { L_, 4.0937531606754398023572013e+155, "409375316067544",  156 },
            { L_, 4.0970301904162202606530181e+122, "409703019041622",  123 },
            { L_, 4.0977133267077300426171469e-126, "409771332670773", -125 },
            { L_, 4.1003577325433300522142941e+068, "410035773254333",   69 },
            { L_, 4.1024800060111900537488159e-051, "410248000601119",  -50 },
            { L_, 4.1045125029672000177155657e-094,  "41045125029672",  -93 },
            { L_, 4.1049347514399502379789545e+071, "410493475143995",   72 },
            { L_, 4.1059346953603401479331080e-153, "410593469536034", -152 },
            { L_, 4.1066576380623301228660472e-132, "410665763806233", -131 },
            { L_, 4.1121059656992598671464042e+127, "411210596569926",  128 },
            { L_, 4.1143575755276998405462885e+187,  "41143575755277",  188 },
            { L_, 4.1146279050460196436366822e+062, "411462790504602",   63 },
            { L_, 4.1151625980410700127805280e-026, "411516259804107",  -25 },
            { L_, 4.1156533308980699277255233e-088, "411565333089807",  -87 },
            { L_, 4.1192513105126402167293550e+131, "411925131051264",  132 },
            { L_, 4.1259187911972997948663398e+026,  "41259187911973",   27 },
            { L_, 4.1267198387629698608726377e+188, "412671983876297",  189 },
            { L_, 4.1280823344599801828543494e+204, "412808233445998",  205 },
            { L_, 4.1302997037828901852583208e-134, "413029970378289", -133 },
            { L_, 4.1305586714546697995536594e+295, "413055867145467",  296 },
            { L_, 4.1319572116946198977252132e+182, "413195721169462",  183 },
            { L_, 4.1323322357955800251957467e-033, "413233223579558",  -32 },
            { L_, 4.1326038182055995810215367e+208,  "41326038182056",  209 },
            { L_, 4.1346009080231997693069886e+154,  "41346009080232",  155 },
            { L_, 4.1348057263235602595636507e+224, "413480572632356",  225 },
            { L_, 4.1350055291661196615790154e+050, "413500552916612",   51 },
            { L_, 4.1383912900996402659670122e-157, "413839129009964", -156 },
            { L_, 4.1405949463158899607607666e+135, "414059494631589",  136 },
            { L_, 4.1417178519032997267274747e+287,  "41417178519033",  288 },
            { L_, 4.1425299295750400547753920e+128, "414252992957504",  129 },
            { L_, 4.1428092438787799095507678e-282, "414280924387878", -281 },
            { L_, 4.1441286483102597433571974e-192, "414412864831026", -191 },
            { L_, 4.1445395063442803302048443e-083, "414453950634428",  -82 },
            { L_, 4.1469290524201602025552237e-045, "414692905242016",  -44 },
            { L_, 4.1476876380030200813878021e-011, "414768763800302",  -10 },
            { L_, 4.1483908202517500196603928e+234, "414839082025175",  235 },
            { L_, 4.1494709831673102629062069e-248, "414947098316731", -247 },
            { L_, 4.1512303796576796517941507e-130, "415123037965768", -129 },
            { L_, 4.1512840579290002183740555e-268,   "4151284057929", -267 },
            { L_, 4.1525717123436298471001302e-159, "415257171234363", -158 },
            { L_, 4.1528306166575399294802359e+048, "415283061665754",   49 },
            { L_, 4.1538295548090102670776720e+215, "415382955480901",  216 },
            { L_, 4.1575939351880297064781189e+007, "415759393518803",    8 },
            { L_, 4.1579329901161698230044937e-128, "415793299011617", -127 },
            { L_, 4.1580167828028798098256463e+183, "415801678280288",  184 },
            { L_, 4.1582853425819498432032613e+102, "415828534258195",  103 },
            { L_, 4.1589769604624700732529163e+006, "415897696046247",    7 },
            { L_, 4.1593771068991404137698372e-028, "415937710689914",  -27 },
            { L_, 4.1600218372549896642840161e+243, "416002183725499",  244 },
            { L_, 4.1668124032343800605082827e-246, "416681240323438", -245 },
            { L_, 4.1668925315171799011082450e+280, "416689253151718",  281 },
            { L_, 4.1693984263914303924109672e-102, "416939842639143", -101 },
            { L_, 4.1708646380130400186819513e+179, "417086463801304",  180 },
            { L_, 4.1714747066959500481084437e-230, "417147470669595", -229 },
            { L_, 4.1736078158893297459864852e-014, "417360781588933",  -13 },
            { L_, 4.1742264470365002611332025e-025,  "41742264470365",  -24 },
            { L_, 4.1770490819324999967376069e+250,  "41770490819325",  251 },
            { L_, 4.1778052530162898195799109e-070, "417780525301629",  -69 },
            { L_, 4.1792421165026000400698810e-023,  "41792421165026",  -22 },
            { L_, 4.1810681193257000960000000e+019,  "41810681193257",   20 },
            { L_, 4.1817297387657998656889497e-010,  "41817297387658",   -9 },
            { L_, 4.1824960583526499433254297e-047, "418249605835265",  -46 },
            { L_, 4.1825507021366400718688965e+009, "418255070213664",   10 },
            { L_, 4.1826675128723203125000000e+013, "418266751287232",   14 },
            { L_, 4.1836428710452899864809675e+234, "418364287104529",  235 },
            { L_, 4.1840610737861396362571643e+115, "418406107378614",  116 },
            { L_, 4.1844387323893997128405268e+060,  "41844387323894",   61 },
            { L_, 4.1856161626547002031911608e-277,  "41856161626547", -276 },
            { L_, 4.1870467471556799020326230e-157, "418704674715568", -156 },
            { L_, 4.1871447195943501527928603e+211, "418714471959435",  212 },
            { L_, 4.1900659732897198985298779e+146, "419006597328972",  147 },
            { L_, 4.1913446779670201810192767e-014, "419134467796702",  -13 },
            { L_, 4.1930857570361700000000000e+014, "419308575703617",   15 },
            { L_, 4.1937357182393400891443631e-229, "419373571823934", -228 },
            { L_, 4.1940605011174798995071042e+037, "419406050111748",   38 },
            { L_, 4.1940659155928799867076392e+201, "419406591559288",  202 },
            { L_, 4.1960656725251397165557120e+107, "419606567252514",  108 },
            { L_, 4.1978123732058099601481533e-016, "419781237320581",  -15 },
            { L_, 4.1984366973331798144615615e-069, "419843669733318",  -68 },
            { L_, 4.1985906701390298923571899e-010, "419859067013903",   -9 },
            { L_, 4.1988289121618503704667091e+006, "419882891216185",    7 },
            { L_, 4.1988848144362301575061575e-065, "419888481443623",  -64 },
            { L_, 4.1998136280997397887085666e+033, "419981362809974",   34 },
            { L_, 4.2016369352092299281048319e-210, "420163693520923", -209 },
            { L_, 4.2081230957248699031901272e-230, "420812309572487", -229 },
            { L_, 4.2081899903722901488784815e-300, "420818999037229", -299 },
            { L_, 4.2082888881055798738203843e-109, "420828888810558", -108 },
            { L_, 4.2088263777916500810274492e+195, "420882637779165",  196 },
            { L_, 4.2098189434117602426883596e-145, "420981894341176", -144 },
            { L_, 4.2128926221109000073693157e-206,  "42128926221109", -205 },
            { L_, 4.2150245580468995768097532e+264,  "42150245580469",  265 },
            { L_, 4.2169770685180400854218798e-274, "421697706851804", -273 },
            { L_, 4.2176257717846000066185983e+201,  "42176257717846",  202 },
            { L_, 4.2179163464202997830799918e+293,  "42179163464203",  294 },
            { L_, 4.2193567462005901846272060e-219, "421935674620059", -218 },
            { L_, 4.2212583095172303104140160e+191, "422125830951723",  192 },
            { L_, 4.2217064174012197426082144e-290, "422170641740122", -289 },
            { L_, 4.2262184900035800900946818e+153, "422621849000358",  154 },
            { L_, 4.2272211104846499860286713e+007, "422722111048465",    8 },
            { L_, 4.2290366575179401542894187e+288, "422903665751794",  289 },
            { L_, 4.2291132739164204071617588e-034, "422911327391642",  -33 },
            { L_, 4.2305572703371499671616549e+275, "423055727033715",  276 },
            { L_, 4.2358783075792602424040025e+142, "423587830757926",  143 },
            { L_, 4.2386872086604802472199646e+126, "423868720866048",  127 },
            { L_, 4.2412699722515701936112125e-238, "424126997225157", -237 },
            { L_, 4.2429064432310095871923291e+180, "424290644323101",  181 },
            { L_, 4.2433270248258903570717940e-059, "424332702482589",  -58 },
            { L_, 4.2446715476119597211641623e-081, "424467154761196",  -80 },
            { L_, 4.2457875655915596500225417e-220, "424578756559156", -219 },
            { L_, 4.2479432590673999867211928e+078,  "42479432590674",   79 },
            { L_, 4.2490483556339900724037886e+183, "424904835563399",  184 },
            { L_, 4.2493682010768901952066282e+118, "424936820107689",  119 },
            { L_, 4.2495047647224097963386339e-234, "424950476472241", -233 },
            { L_, 4.2511066155349298681861299e+168, "425110661553493",  169 },
            { L_, 4.2513292597696099141638616e+274, "425132925976961",  275 },
            { L_, 4.2519160096096197844644926e-061, "425191600960962",  -60 },
            { L_, 4.2554268853321503341271387e+056, "425542688533215",   57 },
            { L_, 4.2614326781570601622499945e-167, "426143267815706", -166 },
            { L_, 4.2636991354164100342658783e+105, "426369913541641",  106 },
            { L_, 4.2677282557244498456935787e-075, "426772825572445",  -74 },
            { L_, 4.2679613589389098316025470e-285, "426796135893891", -284 },
            { L_, 4.2699867529345501103926348e+077, "426998675293455",   78 },
            { L_, 4.2704926919991101015856735e+070, "427049269199911",   71 },
            { L_, 4.2724894887064597879847179e-037, "427248948870646",  -36 },
            { L_, 4.2733570489409600658469952e-022, "427335704894096",  -21 },
            { L_, 4.2741608045004001051203075e-146,  "42741608045004", -145 },
            { L_, 4.2766065698167702064761437e+126, "427660656981677",  127 },
            { L_, 4.2804427282273398326373821e+171, "428044272822734",  172 },
            { L_, 4.2831522820619297435690078e+153, "428315228206193",  154 },
            { L_, 4.2844107941246200092862580e-089, "428441079412462",  -88 },
            { L_, 4.2846813663544401206296347e-005, "428468136635444",   -4 },
            { L_, 4.2851050874862600192000000e+020, "428510508748626",   21 },
            { L_, 4.2851182509411202412914198e+226, "428511825094112",  227 },
            { L_, 4.2859806591010102837248000e+022, "428598065910101",   23 },
            { L_, 4.2865848623862801179351777e+250, "428658486238628",  251 },
            { L_, 4.2885282065401400376189926e+069, "428852820654014",   70 },
            { L_, 4.2887661893921601538987317e+035, "428876618939216",   36 },
            { L_, 4.2897404276365795628815659e+087, "428974042763658",   88 },
            { L_, 4.2899583098423302148341671e+039, "428995830984233",   40 },
            { L_, 4.2901441006585801587491154e+223, "429014410065858",  224 },
            { L_, 4.2939093410305397354878827e+295, "429390934103054",  296 },
            { L_, 4.2965246324440601579441628e+130, "429652463244406",  131 },
            { L_, 4.2975033286393500321090216e+119, "429750332863935",  120 },
            { L_, 4.2991738329068002627207540e-048,  "42991738329068",  -47 },
            { L_, 4.3000843976157302284939929e-043, "430008439761573",  -42 },
            { L_, 4.3007724136377401913688347e+191, "430077241363774",  192 },
            { L_, 4.3009676499314899732818465e+306, "430096764993149",  307 },
            { L_, 4.3015191929961896771619442e-167, "430151919299619", -166 },
            { L_, 4.3015628596684602815678270e-256, "430156285966846", -255 },
            { L_, 4.3019361729303997503964026e+118,  "43019361729304",  119 },
            { L_, 4.3026554668974599163882781e-150, "430265546689746", -149 },
            { L_, 4.3041762501452982100437574e-309,  "43041762501453", -308 },
            { L_, 4.3053591990894096585495456e-059, "430535919908941",  -58 },
            { L_, 4.3076855631237301418477507e+236, "430768556312373",  237 },
            { L_, 4.3079356579645498165655496e+278, "430793565796455",  279 },
            { L_, 4.3104417920066997522317538e-018,  "43104417920067",  -17 },
            { L_, 4.3132247750150698394948563e-015, "431322477501507",  -14 },
            { L_, 4.3163527967065401599294579e-210, "431635279670654", -209 },
            { L_, 4.3169312354010599410177075e-053, "431693123540106",  -52 },
            { L_, 4.3171676949422701165934003e+155, "431716769494227",  156 },
            { L_, 4.3176729259667202719535000e-214, "431767292596672", -213 },
            { L_, 4.3180813354479699573781753e+307, "431808133544797",  308 },
            { L_, 4.3191693806511300105850529e+251, "431916938065113",  252 },
            { L_, 4.3198762583125097088574890e-233, "431987625831251", -232 },
            { L_, 4.3199905395098797637330919e-006, "431999053950988",   -5 },
            { L_, 4.3224748789105498002948543e-295, "432247487891055", -294 },
            { L_, 4.3234209644622900000000000e+014, "432342096446229",   15 },
            { L_, 4.3289751386709895919862280e+050, "432897513867099",   51 },
            { L_, 4.3290025240058098561127603e+222, "432900252400581",  223 },
            { L_, 4.3298595917179100000000000e+014, "432985959171791",   15 },
            { L_, 4.3314579848200096773442709e+227, "433145798482001",  228 },
            { L_, 4.3318743063633502197308037e+189, "433187430636335",  190 },
            { L_, 4.3319137327375498248644860e+054, "433191373273755",   55 },
            { L_, 4.3328850883392200028072170e-294, "433288508833922", -293 },
            { L_, 4.3390781912090896353144406e+137, "433907819120909",  138 },
            { L_, 4.3396537466238803176592701e+163, "433965374662388",  164 },
            { L_, 4.3411147476270800780840863e+176, "434111474762708",  177 },
            { L_, 4.3412866096727998159702838e+055,  "43412866096728",   56 },
            { L_, 4.3441100124363700493887822e+132, "434411001243637",  133 },
            { L_, 4.3445774226474001929979318e-181,  "43445774226474", -180 },
            { L_, 4.3446286815373297417109189e+119, "434462868153733",  120 },
            { L_, 4.3452995504461803401282510e+085, "434529955044618",   86 },
            { L_, 4.3487751231157803523105297e-167, "434877512311578", -166 },
            { L_, 4.3509212887031297849369289e+284, "435092128870313",  285 },
            { L_, 4.3521002170602201178530763e-277, "435210021706022", -276 },
            { L_, 4.3544969405353999659576112e+290,  "43544969405354",  291 },
            { L_, 4.3550925145837802393072140e-136, "435509251458378", -135 },
            { L_, 4.3569084165321902515919034e-204, "435690841653219", -203 },
            { L_, 4.3584229445641702508731567e-028, "435842294456417",  -27 },
            { L_, 4.3591075184623702811634732e+256, "435910751846237",  257 },
            { L_, 4.3601198282955202494464000e+022, "436011982829552",   23 },
            { L_, 4.3613705638347901389183626e+186, "436137056383479",  187 },
            { L_, 4.3614455565711696430470568e+178, "436144555657117",  179 },
            { L_, 4.3622116865453498425787800e-044, "436221168654535",  -43 },
            { L_, 4.3628106014962301808451664e-043, "436281060149623",  -42 },
            { L_, 4.3632404807401299928903129e-184, "436324048074013", -183 },
            { L_, 4.3662832446728500694776165e-025, "436628324467285",  -24 },
            { L_, 4.3677594757120196928931721e-255, "436775947571202", -254 },
            { L_, 4.3687153091499700834075506e-033, "436871530914997",  -32 },
            { L_, 4.3705129694709300894666245e+229, "437051296947093",  230 },
            { L_, 4.3709714541250201458429899e-159, "437097145412502", -158 },
            { L_, 4.3710792647934802703936504e+188, "437107926479348",  189 },
            { L_, 4.3714878555130196911582103e+115, "437148785551302",  116 },
            { L_, 4.3742726383430403641300796e+115, "437427263834304",  116 },
            { L_, 4.3771786944672996881385781e-158,  "43771786944673", -157 },
            { L_, 4.3774202779542298702138076e+102, "437742027795423",  103 },
            { L_, 4.3891549431798401258235587e-304, "438915494317984", -303 },
            { L_, 4.3893445573382403628710955e+215, "438934455733824",  216 },
            { L_, 4.3917814160758798187703149e+278, "439178141607588",  279 },
            { L_, 4.3929101682906700938161773e-147, "439291016829067", -146 },
            { L_, 4.3939137159233300848016039e+172, "439391371592333",  173 },
            { L_, 4.3945191165655498503811906e-290, "439451911656555", -289 },
            { L_, 4.3956608331680399879507732e-290, "439566083316804", -289 },
            { L_, 4.3959683758407601556625522e+186, "439596837584076",  187 },
            { L_, 4.3981162126030800144710575e+117, "439811621260308",  118 },
            { L_, 4.3984299894619699801125351e-021, "439842998946197",  -20 },
            { L_, 4.3990627609205598124121783e+126, "439906276092056",  127 },
            { L_, 4.3992467334203602458678485e+099, "439924673342036",  100 },
            { L_, 4.4031859384262003300506717e+113,  "44031859384262",  114 },
            { L_, 4.4040064309225102698872832e+025, "440400643092251",   26 },
            { L_, 4.4056563024910700744024776e+140, "440565630249107",  141 },
            { L_, 4.4094115132507601430880547e-211, "440941151325076", -210 },
            { L_, 4.4100971396957098182339845e-164, "441009713969571", -163 },
            { L_, 4.4139213120985000179154306e+203,  "44139213120985",  204 },
            { L_, 4.4140859153728998501629763e+077,  "44140859153729",   78 },
            { L_, 4.4149592236247101032234376e+244, "441495922362471",  245 },
            { L_, 4.4162018851171103134150909e-105, "441620188511711", -104 },
            { L_, 4.4187247039765103728659843e+155, "441872470397651",  156 },
            { L_, 4.4188039956041900377812744e-271, "441880399560419", -270 },
            { L_, 4.4192805467093600705093493e+117, "441928054670936",  118 },
            { L_, 4.4238744220375302366017094e+273, "442387442203753",  274 },
            { L_, 4.4250419535408100698093408e-201, "442504195354081", -200 },
            { L_, 4.4285765448632499545356412e+130, "442857654486325",  131 },
            { L_, 4.4339060721982499476538812e-172, "443390607219825", -171 },
            { L_, 4.4348789228862301635570157e-226, "443487892288623", -225 },
            { L_, 4.4351328633164702428832739e+083, "443513286331647",   84 },
            { L_, 4.4386265020260903619934363e-108, "443862650202609", -107 },
            { L_, 4.4393387529610697768438246e+124, "443933875296107",  125 },
            { L_, 4.4407534464671898091969369e-128, "444075344646719", -127 },
            { L_, 4.4430161771140003074711797e+069,   "4443016177114",   70 },
            { L_, 4.4464340595041499252073653e+197, "444643405950415",  198 },
            { L_, 4.4479183349634799818921235e+218, "444791833496348",  219 },
            { L_, 4.4485380605871601169733265e-023, "444853806058716",  -22 },
            { L_, 4.4498074562322800520009244e+275, "444980745623228",  276 },
            { L_, 4.4512470525974598271961413e+274, "445124705259746",  275 },
            { L_, 4.4521767411110100108530494e-237, "445217674111101", -236 },
            { L_, 4.4544604755863098949429924e-075, "445446047558631",  -74 },
            { L_, 4.4558204643864698307116498e+183, "445582046438647",  184 },
            { L_, 4.4581378336588101827740069e+170, "445813783365881",  171 },
            { L_, 4.4619458066297996376127650e+273,  "44619458066298",  274 },
            { L_, 4.4635180182285998555815718e-181,  "44635180182286", -180 },
            { L_, 4.4649427315384999391508308e-160,  "44649427315385", -159 },
            { L_, 4.4652563939996998262184457e+256,  "44652563939997",  257 },
            { L_, 4.4673689019084402316411967e+092, "446736890190844",   93 },
            { L_, 4.4703141173787399963299912e-207, "447031411737874", -206 },
            { L_, 4.4727723723433799661850789e+165, "447277237234338",  166 },
            { L_, 4.4731865936132999592938584e-250,  "44731865936133", -249 },
            { L_, 4.4747327539969799785796649e+161, "447473275399698",  162 },
            { L_, 4.4754131389884002476232474e-020,  "44754131389884",  -19 },
            { L_, 4.4785766670438900213995512e+208, "447857666704389",  209 },
            { L_, 4.4820558375701699852818473e-075, "448205583757017",  -74 },
            { L_, 4.4835705836858795652119883e-252, "448357058368588", -251 },
            { L_, 4.4838737841910503432416687e-022, "448387378419105",  -21 },
            { L_, 4.4842745377990697618080711e-125, "448427453779907", -124 },
            { L_, 4.4847127527293700620373282e+277, "448471275272937",  278 },
            { L_, 4.4850211598783899604380023e+109, "448502115987839",  110 },
            { L_, 4.4865933636397897831205256e-091, "448659336363979",  -90 },
            { L_, 4.4871514225356998162072260e-282,  "44871514225357", -281 },
            { L_, 4.4874904127265099901238308e-040, "448749041272651",  -39 },
            { L_, 4.4890551770020702404322731e-209, "448905517700207", -208 },
            { L_, 4.4894236797147197323336967e-199, "448942367971472", -198 },
            { L_, 4.4906412544132799002268943e+174, "449064125441328",  175 },
            { L_, 4.4910685028606099231075349e-232, "449106850286061", -231 },
            { L_, 4.4918320963267098262372022e+226, "449183209632671",  227 },
            { L_, 4.4939902561264201053560269e+078, "449399025612642",   79 },
            { L_, 4.4950626465409298435571660e+281, "449506264654093",  282 },
            { L_, 4.4955880718891900899777236e-206, "449558807188919", -205 },
            { L_, 4.4966242541931997400088102e-126,  "44966242541932", -125 },
            { L_, 4.4995584930816203067345762e+106, "449955849308162",  107 },
            { L_, 4.4995797786423797471775461e+123, "449957977864238",  124 },
            { L_, 4.5003907651698601637910199e+295, "450039076516986",  296 },
            { L_, 4.5127167699797301458615288e-006, "451271676997973",   -5 },
            { L_, 4.5132599750766195734845973e+239, "451325997507662",  240 },
            { L_, 4.5154189932957801667357657e+216, "451541899329578",  217 },
            { L_, 4.5173245750650597386254004e+115, "451732457506506",  116 },
            { L_, 4.5173360905597902170419973e+188, "451733609055979",  189 },
            { L_, 4.5182409908336197635917451e+106, "451824099083362",  107 },
            { L_, 4.5182669880481497910008798e-036, "451826698804815",  -35 },
            { L_, 4.5182973635791499692888179e-297, "451829736357915", -296 },
            { L_, 4.5187081805571403481207602e-186, "451870818055714", -185 },
            { L_, 4.5193180904039999227646191e-169,   "4519318090404", -168 },
            { L_, 4.5208188059561501792497093e-021, "452081880595615",  -20 },
            { L_, 4.5246596723808698877649179e+086, "452465967238087",   87 },
            { L_, 4.5256975454859701481875983e-231, "452569754548597", -230 },
            { L_, 4.5264856855395098301705903e-004, "452648568553951",   -3 },
            { L_, 4.5277960894179499067081984e+183, "452779608941795",  184 },
            { L_, 4.5292155725009000889075688e-171,  "45292155725009", -170 },
            { L_, 4.5296980506517401059911163e+255, "452969805065174",  256 },
            { L_, 4.5301346021418199626742887e+099, "453013460214182",  100 },
            { L_, 4.5313208014646703286680542e+256, "453132080146467",  257 },
            { L_, 4.5326148962001498148310930e+004, "453261489620015",    5 },
            { L_, 4.5342084891197997671015839e-244,  "45342084891198", -243 },
            { L_, 4.5400079727031003644531577e-049,  "45400079727031",  -48 },
            { L_, 4.5407031645995202885996974e-303, "454070316459952", -302 },
            { L_, 4.5414450739109296134621971e-162, "454144507391093", -161 },
            { L_, 4.5453580299440796868504536e-239, "454535802994408", -238 },
            { L_, 4.5486003988448000875369687e-187,  "45486003988448", -186 },
            { L_, 4.5489178743466603602871615e-211, "454891787434666", -210 },
            { L_, 4.5490340172205800942796095e-015, "454903401722058",  -14 },
            { L_, 4.5506189331749800312013204e+151, "455061893317498",  152 },
            { L_, 4.5507726540728103715409491e-168, "455077265407281", -167 },
            { L_, 4.5510676865275699200000000e+017, "455106768652757",   18 },
            { L_, 4.5519938875599999104020548e+231,    "455199388756",  232 },
            { L_, 4.5546206494317097389763938e+104, "455462064943171",  105 },
            { L_, 4.5589728642128799916710078e-121, "455897286421288", -120 },
            { L_, 4.5603721171002701135049929e+098, "456037211710027",   99 },
            { L_, 4.5621849187832201042264683e+207, "456218491878322",  208 },
            { L_, 4.5653894706071797168898889e+290, "456538947060718",  291 },
            { L_, 4.5656880810399301945360813e-122, "456568808103993", -121 },
            { L_, 4.5660500530212398545157143e-091, "456605005302124",  -90 },
            { L_, 4.5692896617516498029314779e+302, "456928966175165",  303 },
            { L_, 4.5717654427423298945239326e-281, "457176544274233", -280 },
            { L_, 4.5722922526852199653998421e+055, "457229225268522",   56 },
            { L_, 4.5723741224839695964563162e-249, "457237412248397", -248 },
            { L_, 4.5725725076592498779296875e+011, "457257250765925",   12 },
            { L_, 4.5745157781166197771284876e-159, "457451577811662", -158 },
            { L_, 4.5745709342031801718097196e-051, "457457093420318",  -50 },
            { L_, 4.5748706496030803520146355e-305, "457487064960308", -304 },
            { L_, 4.5756267158777397969167985e-007, "457562671587774",   -6 },
            { L_, 4.5780582708456301937052914e+209, "457805827084563",  210 },
            { L_, 4.5795231219566798077198403e-122, "457952312195668", -121 },
            { L_, 4.5799101252499599014006840e-191, "457991012524996", -190 },
            { L_, 4.5818725074040895968502408e-013, "458187250740409",  -12 },
            { L_, 4.5837366094588002533449152e-253,  "45837366094588", -252 },
            { L_, 4.5847904592010300791387077e+200, "458479045920103",  201 },
            { L_, 4.5854333180553997097695512e+244,  "45854333180554",  245 },
            { L_, 4.5865070647945003803169815e-273,  "45865070647945", -272 },
            { L_, 4.5889701326468999606305863e-052,  "45889701326469",  -51 },
            { L_, 4.5935530871810498287260515e-272, "459355308718105", -271 },
            { L_, 4.5961974611223799877633292e-048, "459619746112238",  -47 },
            { L_, 4.5974075129715399267338658e-085, "459740751297154",  -84 },
            { L_, 4.6005272221874900520890678e-301, "460052722218749", -300 },
            { L_, 4.6020005855793501281514324e-002, "460200058557935",   -1 },
            { L_, 4.6041185379713697338624621e-261, "460411853797137", -260 },
            { L_, 4.6047891933356902010225837e-257, "460478919333569", -256 },
            { L_, 4.6049441934210100489388548e-300, "460494419342101", -299 },
            { L_, 4.6066697985796901386707276e+086, "460666979857969",   87 },
            { L_, 4.6122607110693799411703383e-098, "461226071106938",  -97 },
            { L_, 4.6124538290732699250049875e+282, "461245382907327",  283 },
            { L_, 4.6128290427970302848234243e+266, "461282904279703",  267 },
            { L_, 4.6135700374283801799605151e-084, "461357003742838",  -83 },
            { L_, 4.6148531472438699051915685e-181, "461485314724387", -180 },
            { L_, 4.6180102339340300873166682e-261, "461801023393403", -260 },
            { L_, 4.6185069113413697852280209e-017, "461850691134137",  -16 },
            { L_, 4.6192592938622899475641401e-274, "461925929386229", -273 },
            { L_, 4.6202711141973801026257154e-142, "462027111419738", -141 },
            { L_, 4.6218681217548402828447352e+294, "462186812175484",  295 },
            { L_, 4.6229818425893396946335761e+054, "462298184258934",   55 },
            { L_, 4.6257160911260395706960969e-121, "462571609112604", -120 },
            { L_, 4.6270994526788296412854333e+245, "462709945267883",  246 },
            { L_, 4.6276964306674699882185172e-194, "462769643066747", -193 },
            { L_, 4.6287309780277601121488390e-078, "462873097802776",  -77 },
            { L_, 4.6297838951568697853423492e-304, "462978389515687", -303 },
            { L_, 4.6324750444764099121093750e+011, "463247504447641",   12 },
            { L_, 4.6331931619411097119426554e-164, "463319316194111", -163 },
            { L_, 4.6336402387888795554199420e+304, "463364023878888",  305 },
            { L_, 4.6360488007458599939103977e-079, "463604880074586",  -78 },
            { L_, 4.6365714206938902741244338e-075, "463657142069389",  -74 },
            { L_, 4.6408469967001603053913437e+216, "464084699670016",  217 },
            { L_, 4.6412290406135597473487028e-305, "464122904061356", -304 },
            { L_, 4.6434200038379200820943153e-042, "464342000383792",  -41 },
            { L_, 4.6454144469579597558984383e-108, "464541444695796", -107 },
            { L_, 4.6461499965005800631134449e-244, "464614999650058", -243 },
            { L_, 4.6462036082378999201330632e+274,  "46462036082379",  275 },
            { L_, 4.6462884150608703021292787e+069, "464628841506087",   70 },
            { L_, 4.6470878099064198796305522e-199, "464708780990642", -198 },
            { L_, 4.6470921295305502203332833e+094, "464709212953055",   95 },
            { L_, 4.6512455970638397262373219e-099, "465124559706384",  -98 },
            { L_, 4.6538241718932997683078614e-033,  "46538241718933",  -32 },
            { L_, 4.6539779025668298194457450e-012, "465397790256683",  -11 },
            { L_, 4.6568069438165197030477140e-191, "465680694381652", -190 },
            { L_, 4.6573320860888800660137830e-240, "465733208608888", -239 },
            { L_, 4.6573731268653498126058506e-071, "465737312686535",  -70 },
            { L_, 4.6584049874351702372439392e+159, "465840498743517",  160 },
            { L_, 4.6597320736817897413521585e+210, "465973207368179",  211 },
            { L_, 4.6613841171738297697289988e-113, "466138411717383", -112 },
            { L_, 4.6616503966328502416875604e+269, "466165039663285",  270 },
            { L_, 4.6621088785123701727532804e-236, "466210887851237", -235 },
            { L_, 4.6642115263325701024254294e+085, "466421152633257",   86 },
            { L_, 4.6676537774939401521421543e+173, "466765377749394",  174 },
            { L_, 4.6680870737279998779296875e+010,   "4668087073728",   11 },
            { L_, 4.6681687027198897573926113e-012, "466816870271989",  -11 },
            { L_, 4.6696005633736799661499682e-291, "466960056337368", -290 },
            { L_, 4.6733304785585199133894363e+058, "467333047855852",   59 },
            { L_, 4.6735828156677603304743909e+081, "467358281566776",   82 },
            { L_, 4.6751333558558699946781402e-015, "467513335585587",  -14 },
            { L_, 4.6768700832609997688778602e-244,   "4676870083261", -243 },
            { L_, 4.6777615252321997981749407e-027,  "46777615252322",  -26 },
            { L_, 4.6782290228541903937802939e-307, "467822902285419", -306 },
            { L_, 4.6803741699480102843318361e+061, "468037416994801",   62 },
            { L_, 4.6804377714148297481298556e-032, "468043777141483",  -31 },
            { L_, 4.6811407076344398779222444e+285, "468114070763444",  286 },
            { L_, 4.6821043109150503425619963e+190, "468210431091505",  191 },
            { L_, 4.6834319979583401561772393e-142, "468343199795834", -141 },
            { L_, 4.6847753941351801486694830e-053, "468477539413518",  -52 },
            { L_, 4.6863689171408001412889247e-290,  "46863689171408", -289 },
            { L_, 4.6874118895472103247612143e+289, "468741188954721",  290 },
            { L_, 4.6891250035347399999890896e-064, "468912500353474",  -63 },
            { L_, 4.6911653472149598379061210e-162, "469116534721496", -161 },
            { L_, 4.6916927637470697657197417e+097, "469169276374707",   98 },
            { L_, 4.6950259899318896214794401e+077, "469502598993189",   78 },
            { L_, 4.6963159055854397750576674e+067, "469631590558544",   68 },
            { L_, 4.7014739599909902462925777e-249, "470147395999099", -248 },
            { L_, 4.7015446302217601930103025e-262, "470154463022176", -261 },
            { L_, 4.7031391567158098123432937e-293, "470313915671581", -292 },
            { L_, 4.7033864761508498066738815e-004, "470338647615085",   -3 },
            { L_, 4.7034243474512203131708301e+043, "470342434745122",   44 },
            { L_, 4.7048713155479204025209141e-270, "470487131554792", -269 },
            { L_, 4.7121138973680502903962511e-244, "471211389736805", -243 },
            { L_, 4.7121704451423401806769229e+108, "471217044514234",  109 },
            { L_, 4.7135229479971603116727685e-123, "471352294799716", -122 },
            { L_, 4.7143915917537599963735885e+160, "471439159175376",  161 },
            { L_, 4.7169000977428199110708039e-109, "471690009774282", -108 },
            { L_, 4.7173690367280197388510348e-298, "471736903672802", -297 },
            { L_, 4.7225999588092302821080416e+113, "472259995880923",  114 },
            { L_, 4.7258594202755699513061070e-161, "472585942027557", -160 },
            { L_, 4.7316384488913304813811809e+102, "473163844889133",  103 },
            { L_, 4.7323718554929397466999892e-026, "473237185549294",  -25 },
            { L_, 4.7324235793419201782798791e-195, "473242357934192", -194 },
            { L_, 4.7325013956897598981162737e+031, "473250139568976",   32 },
            { L_, 4.7327410206417797964471007e-222, "473274102064178", -221 },
            { L_, 4.7335733314873899110925844e-064, "473357333148739",  -63 },
            { L_, 4.7348320232441195861088846e-174, "473483202324412", -173 },
            { L_, 4.7377574735842101071708160e+024, "473775747358421",   25 },
            { L_, 4.7387825318039698800388854e-072, "473878253180397",  -71 },
            { L_, 4.7394184510182103195665772e+112, "473941845101821",  113 },
            { L_, 4.7395309188109201934157568e+114, "473953091881092",  115 },
            { L_, 4.7418784581311499579406600e+104, "474187845813115",  105 },
            { L_, 4.7446965176434999711978760e+288,  "47446965176435",  289 },
            { L_, 4.7451619190933502573915852e+298, "474516191909335",  299 },
            { L_, 4.7454087675641698303036939e+207, "474540876756417",  208 },
            { L_, 4.7502777772626702697930452e-143, "475027777726267", -142 },
            { L_, 4.7504794302902799608886711e+284, "475047943029028",  285 },
            { L_, 4.7527785484253896692934016e+141, "475277854842539",  142 },
            { L_, 4.7535151203161699024804846e-085, "475351512031617",  -84 },
            { L_, 4.7542889253124496121287351e+218, "475428892531245",  219 },
            { L_, 4.7547381636216002047018277e-287,  "47547381636216", -286 },
            { L_, 4.7547579173949400553705559e+122, "475475791739494",  123 },
            { L_, 4.7605915460540002723683548e-195,   "4760591546054", -194 },
            { L_, 4.7624270864001300000000000e+014, "476242708640013",   15 },
            { L_, 4.7682635292384598077038457e-167, "476826352923846", -166 },
            { L_, 4.7728093399080299642244198e+062, "477280933990803",   63 },
            { L_, 4.7735240024039000000000000e+013,  "47735240024039",   14 },
            { L_, 4.7749887934088003039832475e-195,  "47749887934088", -194 },
            { L_, 4.7770437176876703446331936e+087, "477704371768767",   88 },
            { L_, 4.7776593873243995650465382e-193,  "47776593873244", -192 },
            { L_, 4.7800707492300898154009324e-278, "478007074923009", -277 },
            { L_, 4.7808562166988603566963884e+158, "478085621669886",  159 },
            { L_, 4.7822681061706401293640180e+186, "478226810617064",  187 },
            { L_, 4.7824115304916004009771111e+254,  "47824115304916",  255 },
            { L_, 4.7848813148517703677048006e+221, "478488131485177",  222 },
            { L_, 4.7852377911535096746923822e-022, "478523779115351",  -21 },
            { L_, 4.7856812251286198920495825e-271, "478568122512862", -270 },
            { L_, 4.7866121831153101384961793e-014, "478661218311531",  -13 },
            { L_, 4.7926771936229501527399950e-301, "479267719362295", -300 },
            { L_, 4.7958337970652402896334167e-270, "479583379706524", -269 },
            { L_, 4.7960831048585696670336826e-301, "479608310485857", -300 },
            { L_, 4.7978842013331901810324932e+172, "479788420133319",  173 },
            { L_, 4.7988812383288201202765030e+148, "479888123832882",  149 },
            { L_, 4.7990571776620204424453991e+223, "479905717766202",  224 },
            { L_, 4.8004862025306698775675270e-177, "480048620253067", -176 },
            { L_, 4.8007660384659499603539564e-110, "480076603846595", -109 },
            { L_, 4.8018170979656098594694502e+238, "480181709796561",  239 },
            { L_, 4.8042331195398898281905887e+122, "480423311953989",  123 },
            { L_, 4.8068750865139401254704884e+143, "480687508651394",  144 },
            { L_, 4.8081916568693900102860800e+023, "480819165686939",   24 },
            { L_, 4.8081982552604398745058819e-199, "480819825526044", -198 },
            { L_, 4.8089842508045401607174556e+112, "480898425080454",  113 },
            { L_, 4.8115576145540396974268869e+100, "481155761455404",  101 },
            { L_, 4.8137244020667899567712139e-107, "481372440206679", -106 },
            { L_, 4.8168550938771903243226247e+087, "481685509387719",   88 },
            { L_, 4.8170160483068296746160064e-184, "481701604830683", -183 },
            { L_, 4.8170341613263597501218036e-131, "481703416132636", -130 },
            { L_, 4.8176501273255700868773572e-206, "481765012732557", -205 },
            { L_, 4.8177217151450301907518490e+074, "481772171514503",   75 },
            { L_, 4.8179226708838103343347535e-263, "481792267088381", -262 },
            { L_, 4.8190973708187995780915892e+195,  "48190973708188",  196 },
            { L_, 4.8205551509876299374342500e+188, "482055515098763",  189 },
            { L_, 4.8225494310355400042961561e+154, "482254943103554",  155 },
            { L_, 4.8265606260204203468578619e+246, "482656062602042",  247 },
            { L_, 4.8273419149610199014008678e+192, "482734191496102",  193 },
            { L_, 4.8274208529156598054822089e-257, "482742085291566", -256 },
            { L_, 4.8306366152161100786332850e-171, "483063661521611", -170 },
            { L_, 4.8327555227478097559479420e-082, "483275552274781",  -81 },
            { L_, 4.8334984670945800138473260e-196, "483349846709458", -195 },
            { L_, 4.8348329256495901190220008e-185, "483483292564959", -184 },
            { L_, 4.8369138100032401664288027e+291, "483691381000324",  292 },
            { L_, 4.8377289409916298582939827e-104, "483772894099163", -103 },
            { L_, 4.8407236167194100987586178e-032, "484072361671941",  -31 },
            { L_, 4.8415577861082801591513954e+271, "484155778610828",  272 },
            { L_, 4.8436343240514599007187029e-154, "484363432405146", -153 },
            { L_, 4.8478150830289602226558585e-229, "484781508302896", -228 },
            { L_, 4.8481767580758696780256659e+164, "484817675807587",  165 },
            { L_, 4.8513247473545297884025486e-053, "485132474735453",  -52 },
            { L_, 4.8529206721688796168116351e-096, "485292067216888",  -95 },
            { L_, 4.8533783164895098310680323e+244, "485337831648951",  245 },
            { L_, 4.8551116389070304000000000e+016, "485511163890703",   17 },
            { L_, 4.8573402482327300418617439e+129, "485734024823273",  130 },
            { L_, 4.8580568102769500334689330e+062, "485805681027695",   63 },
            { L_, 4.8593495681952101253504422e+160, "485934956819521",  161 },
            { L_, 4.8609832817714199937495072e+166, "486098328177142",  167 },
            { L_, 4.8653239280818601789906861e+211, "486532392808186",  212 },
            { L_, 4.8661284122880301549811507e+289, "486612841228803",  290 },
            { L_, 4.8675771794363502100154339e-035, "486757717943635",  -34 },
            { L_, 4.8675967453873099636935012e-208, "486759674538731", -207 },
            { L_, 4.8676034657803301766984517e+039, "486760346578033",   40 },
            { L_, 4.8697798951484096734665951e-149, "486977989514841", -148 },
            { L_, 4.8745699561248297553628230e-228, "487456995612483", -227 },
            { L_, 4.8755671244719103185616405e+000, "487556712447191",    1 },
            { L_, 4.8771631224691199446542822e-269, "487716312246912", -268 },
            { L_, 4.8780648839595696385608468e+062, "487806488395957",   63 },
            { L_, 4.8782332131937597543351044e+205, "487823321319376",  206 },
            { L_, 4.8799140811916996640109343e-052,  "48799140811917",  -51 },
            { L_, 4.8808324031321503335506604e-195, "488083240313215", -194 },
            { L_, 4.8809979413374702486455992e+104, "488099794133747",  105 },
            { L_, 4.8838530949721900617040767e-173, "488385309497219", -172 },
            { L_, 4.8883469516781900024571583e+194, "488834695167819",  195 },
            { L_, 4.8912141833281799563337700e+002, "489121418332818",    3 },
            { L_, 4.8940963294508799438258248e-143, "489409632945088", -142 },
            { L_, 4.8942774315983798321484813e+036, "489427743159838",   37 },
            { L_, 4.8981452645873301015358167e-229, "489814526458733", -228 },
            { L_, 4.9008770327233802527189255e+005, "490087703272338",    6 },
            { L_, 4.9016427912798095839014998e-037, "490164279127981",  -36 },
            { L_, 4.9048882340438600973687703e-056, "490488823404386",  -55 },
            { L_, 4.9053411612337001881988061e-007,  "49053411612337",   -6 },
            { L_, 4.9073871175630996746247207e+102,  "49073871175631",  103 },
            { L_, 4.9101476537190801821159672e+066, "491014765371908",   67 },
            { L_, 4.9141152615913999707542251e-196,  "49141152615914", -195 },
            { L_, 4.9168381855396697386267610e+150, "491683818553967",  151 },
            { L_, 4.9172850189343001579630225e-090,  "49172850189343",  -89 },
            { L_, 4.9189374484157200000000000e+015, "491893744841572",   16 },
            { L_, 4.9196902129783203398780540e+136, "491969021297832",  137 },
            { L_, 4.9203430330707301708004582e-067, "492034303307073",  -66 },
            { L_, 4.9204749698056202057208276e-104, "492047496980562", -103 },
            { L_, 4.9233323912256002492843369e-273,  "49233323912256", -272 },
            { L_, 4.9246143612370600000000000e+015, "492461436123706",   16 },
            { L_, 4.9249754799138899724094054e+106, "492497547991389",  107 },
            { L_, 4.9253710990113397266576198e+250, "492537109901134",  251 },
            { L_, 4.9258658177676102404577839e+096, "492586581776761",   97 },
            { L_, 4.9269544952387798289898117e-208, "492695449523878", -207 },
            { L_, 4.9277237141877698013558984e+005, "492772371418777",    6 },
            { L_, 4.9303668712537601437641308e+203, "493036687125376",  204 },
            { L_, 4.9308799598964303430572426e+134, "493087995989643",  135 },
            { L_, 4.9326530442506801445042720e-079, "493265304425068",  -78 },
            { L_, 4.9341471686674995845901457e+065,  "49341471686675",   66 },
            { L_, 4.9341948795672601188920286e-033, "493419487956726",  -32 },
            { L_, 4.9349466260760001550676849e-237,   "4934946626076", -236 },
            { L_, 4.9367398414954603524753334e+254, "493673984149546",  255 },
            { L_, 4.9377754791035599450533435e+204, "493777547910356",  205 },
            { L_, 4.9379621393051002599877871e-076,  "49379621393051",  -75 },
            { L_, 4.9406501977264097652649612e+184, "494065019772641",  185 },
            { L_, 4.9416525669953998700210382e+081,  "49416525669954",   82 },
            { L_, 4.9473512085506899444081291e+253, "494735120855069",  254 },
            { L_, 4.9475048410116501515772746e+207, "494750484101165",  208 },
            { L_, 4.9480260205075800841051139e-248, "494802602050758", -247 },
            { L_, 4.9482798864433601728747061e-205, "494827988644336", -204 },
            { L_, 4.9498678868711600341800372e+104, "494986788687116",  105 },
            { L_, 4.9503967121473696790812205e+192, "495039671214737",  193 },
            { L_, 4.9520648835693702427901917e-070, "495206488356937",  -69 },
            { L_, 4.9537037962475898288112299e+033, "495370379624759",   34 },
            { L_, 4.9555557873547200965254214e-009, "495555578735472",   -8 },
            { L_, 4.9555927533231095841240452e-174, "495559275332311", -173 },
            { L_, 4.9564936761573296814467133e+166, "495649367615733",  167 },
            { L_, 4.9603493009915699009240755e+230, "496034930099157",  231 },
            { L_, 4.9609583394400299115270869e-253, "496095833944003", -252 },
            { L_, 4.9637405591783496453855853e+267, "496374055917835",  268 },
            { L_, 4.9650537984246900929765979e-028, "496505379842469",  -27 },
            { L_, 4.9650732221420398115928988e+058, "496507322214204",   59 },
            { L_, 4.9661282998897601314998264e-191, "496612829988976", -190 },
            { L_, 4.9690689028836802429434094e-003, "496906890288368",   -2 },
            { L_, 4.9697579430952900210122889e-239, "496975794309529", -238 },
            { L_, 4.9700727304553298994211456e-157, "497007273045533", -156 },
            { L_, 4.9710019569044802754459374e-041, "497100195690448",  -40 },
            { L_, 4.9734058340311601187820428e+273, "497340583403116",  274 },
            { L_, 4.9735652005057398737905136e-166, "497356520050574", -165 },
            { L_, 4.9758971105663298095001069e+046, "497589711056633",   47 },
            { L_, 4.9773244932369602397784379e-023, "497732449323696",  -22 },
            { L_, 4.9777366784771002829074860e+007,  "49777366784771",    8 },
            { L_, 4.9785298879200499358202527e+092, "497852988792005",   93 },
            { L_, 4.9790427366030797717212559e+302, "497904273660308",  303 },
            { L_, 4.9791875789529303855938513e-025, "497918757895293",  -24 },
            { L_, 4.9797568217591501466616882e-113, "497975682175915", -112 },
            { L_, 4.9827962077681898604341410e+213, "498279620776819",  214 },
            { L_, 4.9854859841806598592266336e-285, "498548598418066", -284 },
            { L_, 4.9864317278824397547753315e-191, "498643172788244", -190 },
            { L_, 4.9905302583785700768976281e+144, "499053025837857",  145 },
            { L_, 4.9950054209788299838964467e+301, "499500542097883",  302 },
            { L_, 4.9967185895202799467074358e+268, "499671858952028",  269 },
            { L_, 4.9975938377417102331852183e-266, "499759383774171", -265 },
            { L_, 4.9976903291979702696563389e+029, "499769032919797",   30 },
            { L_, 5.0003107144417497577445409e-255, "500031071444175", -254 },
            { L_, 5.0007188199611098857380877e-132, "500071881996111", -131 },
            { L_, 5.0009311863594200989588711e-254, "500093118635942", -253 },
            { L_, 5.0019105955937204210925984e-001, "500191059559372",    0 },
            { L_, 5.0034884046144098626720632e+305, "500348840461441",  306 },
            { L_, 5.0039861245171199364252506e-192, "500398612451712", -191 },
            { L_, 5.0060309448965603844266729e-268, "500603094489656", -267 },
            { L_, 5.0113607094366402202819118e+127, "501136070943664",  128 },
            { L_, 5.0174961184012099730137462e+176, "501749611840121",  177 },
            { L_, 5.0178018489812699817748202e+224, "501780184898127",  225 },
            { L_, 5.0180141451900700648584384e+279, "501801414519007",  280 },
            { L_, 5.0201433414803998296674465e-158,  "50201433414804", -157 },
            { L_, 5.0207379920469201239488118e-084, "502073799204692",  -83 },
            { L_, 5.0232269371811600772453767e+290, "502322693718116",  291 },
            { L_, 5.0234685036345603390708829e+090, "502346850363456",   91 },
            { L_, 5.0249890497816897396875157e-198, "502498904978169", -197 },
            { L_, 5.0272699021773296426643245e+080, "502726990217733",   81 },
            { L_, 5.0280713033816201595333736e-195, "502807130338162", -194 },
            { L_, 5.0290965338986602665483091e+156, "502909653389866",  157 },
            { L_, 5.0292533104773498556206606e+111, "502925331047735",  112 },
            { L_, 5.0298188146516100667834292e+255, "502981881465161",  256 },
            { L_, 5.0299039284261397533455003e-210, "502990392842614", -209 },
            { L_, 5.0368182938353105020930744e-097, "503681829383531",  -96 },
            { L_, 5.0369220751091599459694147e-247, "503692207510916", -246 },
            { L_, 5.0369702600516501403087861e+066, "503697026005165",   67 },
            { L_, 5.0375571214519496586856156e+267, "503755712145195",  268 },
            { L_, 5.0390072897782095780800905e+245, "503900728977821",  246 },
            { L_, 5.0396446006432098811828064e+183, "503964460064321",  184 },
            { L_, 5.0453485542738602243779790e+152, "504534855427386",  153 },
            { L_, 5.0459766834777898847512987e-115, "504597668347779", -114 },
            { L_, 5.0487027209168502276721235e-164, "504870272091685", -163 },
            { L_, 5.0509159829541096228535831e-068, "505091598295411",  -67 },
            { L_, 5.0516859612558804001618304e-168, "505168596125588", -167 },
            { L_, 5.0519220489325896350838287e-007, "505192204893259",   -6 },
            { L_, 5.0530581423696799849078864e+035, "505305814236968",   36 },
            { L_, 5.0533962721950899360187651e+307, "505339627219509",  308 },
            { L_, 5.0533984598598497550357449e+048, "505339845985985",   49 },
            { L_, 5.0534439223276698047206803e-057, "505344392232767",  -56 },
            { L_, 5.0538753029951696000000000e+016, "505387530299517",   17 },
            { L_, 5.0561434015955302196153083e+222, "505614340159553",  223 },
            { L_, 5.0578893444585596050667451e-217, "505788934445856", -216 },
            { L_, 5.0592894249211900170787730e-065, "505928942492119",  -64 },
            { L_, 5.0603933969383398327811045e+300, "506039339693834",  301 },
            { L_, 5.0605589701467800444010011e-081, "506055897014678",  -80 },
            { L_, 5.0608458020681003498524321e-258,  "50608458020681", -257 },
            { L_, 5.0614977439284898648849042e-050, "506149774392849",  -49 },
            { L_, 5.0618781770710497242608129e-054, "506187817707105",  -53 },
            { L_, 5.0636336418858800449753996e-081, "506363364188588",  -80 },
            { L_, 5.0643194959706896366483851e-227, "506431949597069", -226 },
            { L_, 5.0644818093381502467638542e+222, "506448180933815",  223 },
            { L_, 5.0685551801906099309561480e+191, "506855518019061",  192 },
            { L_, 5.0692011825459599527713364e+307, "506920118254596",  308 },
            { L_, 5.0699658426951001214807016e-203,  "50699658426951", -202 },
            { L_, 5.0704766629998196601018013e+189, "507047666299982",  190 },
            { L_, 5.0716667573949699018671013e-101, "507166675739497", -100 },
            { L_, 5.0725716554266496843680103e+222, "507257165542665",  223 },
            { L_, 5.0749174367761304656224771e+301, "507491743677613",  302 },
            { L_, 5.0760805040495897479948810e+111, "507608050404959",  112 },
            { L_, 5.0762075918662299439896573e+123, "507620759186623",  124 },
            { L_, 5.0784614723349900206723997e-302, "507846147233499", -301 },
            { L_, 5.0784762913968402873047733e-288, "507847629139684", -287 },
            { L_, 5.0804231002091699218750000e+011, "508042310020917",   12 },
            { L_, 5.0804304176883899010931235e+093, "508043041768839",   94 },
            { L_, 5.0843460201057400932764138e-062, "508434602010574",  -61 },
            { L_, 5.0866392390559200398589976e-143, "508663923905592", -142 },
            { L_, 5.0891619703776198251255901e+303, "508916197037762",  304 },
            { L_, 5.0903391218624002762132879e-141,  "50903391218624", -140 },
            { L_, 5.0905917267835902184940032e-169, "509059172678359", -168 },
            { L_, 5.0914423660317596276395599e-307, "509144236603176", -306 },
            { L_, 5.1006718643610702730665658e-275, "510067186436107", -274 },
            { L_, 5.1012327190668698668025979e+070, "510123271906687",   71 },
            { L_, 5.1025177281092599155771219e-209, "510251772810926", -208 },
            { L_, 5.1038452537335401254009621e-009, "510384525373354",   -8 },
            { L_, 5.1090769949001601667630449e+250, "510907699490016",  251 },
            { L_, 5.1094838146838200099773027e-281, "510948381468382", -280 },
            { L_, 5.1115469693376604811162992e-218, "511154696933766", -217 },
            { L_, 5.1132852022749702389760000e+022, "511328520227497",   23 },
            { L_, 5.1157018355847299920917047e+187, "511570183558473",  188 },
            { L_, 5.1161689715741297599672388e+161, "511616897157413",  162 },
            { L_, 5.1171228167294496599883327e-077, "511712281672945",  -76 },
            { L_, 5.1184347750765096848405827e-008, "511843477507651",   -7 },
            { L_, 5.1184555550593201810373594e-040, "511845555505932",  -39 },
            { L_, 5.1188996639719798362661221e+288, "511889966397198",  289 },
            { L_, 5.1223422298702801159715673e-232, "512234222987028", -231 },
            { L_, 5.1227684442197701752325382e-198, "512276844421977", -197 },
            { L_, 5.1240373534283001386139906e-110,  "51240373534283", -109 },
            { L_, 5.1255168876845003256174517e+203,  "51255168876845",  204 },
            { L_, 5.1255587704456000181545368e-198,  "51255587704456", -197 },
            { L_, 5.1256955129607703137776982e+125, "512569551296077",  126 },
            { L_, 5.1265123632049402989679576e+082, "512651236320494",   83 },
            { L_, 5.1296251568920999784931894e-166,  "51296251568921", -165 },
            { L_, 5.1343870695240101315339372e-023, "513438706952401",  -22 },
            { L_, 5.1380666498933998558752368e-291,  "51380666498934", -290 },
            { L_, 5.1395427864566798294444770e+080, "513954278645668",   81 },
            { L_, 5.1397710187648000624515532e-123,  "51397710187648", -122 },
            { L_, 5.1443544675127299522647974e+255, "514435446751273",  256 },
            { L_, 5.1474378699560200719671051e+116, "514743786995602",  117 },
            { L_, 5.1484088388224601666391716e+270, "514840883882246",  271 },
            { L_, 5.1511213172776400412031428e-053, "515112131727764",  -52 },
            { L_, 5.1526532208470601575601360e+280, "515265322084706",  281 },
            { L_, 5.1531627636803397159748424e-255, "515316276368034", -254 },
            { L_, 5.1547941073272503471565402e-293, "515479410732725", -292 },
            { L_, 5.1569049135533796875000000e+013, "515690491355338",   14 },
            { L_, 5.1578142937692799885444032e+304, "515781429376928",  305 },
            { L_, 5.1583930629964597828863327e+048, "515839306299646",   49 },
            { L_, 5.1594984089681402630488761e+180, "515949840896814",  181 },
            { L_, 5.1595914771251899754843637e+082, "515959147712519",   83 },
            { L_, 5.1598492876979001403131012e+071,  "51598492876979",   72 },
            { L_, 5.1617043495897002118838269e-079,  "51617043495897",  -78 },
            { L_, 5.1635513946854803032254283e+074, "516355139468548",   75 },
            { L_, 5.1650757443612600957026614e-082, "516507574436126",  -81 },
            { L_, 5.1703453021757099133282503e+151, "517034530217571",  152 },
            { L_, 5.1705298011242501337530166e+105, "517052980112425",  106 },
            { L_, 5.1733363335581696953189196e+289, "517333633355817",  290 },
            { L_, 5.1741119612172098364701550e-225, "517411196121721", -224 },
            { L_, 5.1746118043471099409714967e-088, "517461180434711",  -87 },
            { L_, 5.1761013631406297491908635e+234, "517610136314063",  235 },
            { L_, 5.1817506222353399339268105e-287, "518175062223534", -286 },
            { L_, 5.1834873610280197213175248e-101, "518348736102802", -100 },
            { L_, 5.1836047088116497525402494e+223, "518360470881165",  224 },
            { L_, 5.1846654227599496238685181e+187, "518466542275995",  188 },
            { L_, 5.1905616319723996307082668e-224,  "51905616319724", -223 },
            { L_, 5.1960365279615603431881466e-111, "519603652796156", -110 },
            { L_, 5.1963002449759197366664398e-182, "519630024497592", -181 },
            { L_, 5.1970981546109800276244118e-271, "519709815461098", -270 },
            { L_, 5.1979890839907598570718432e+096, "519798908399076",   97 },
            { L_, 5.1981007242856897052708015e+078, "519810072428569",   79 },
            { L_, 5.2011106199583801328070416e+215, "520111061995838",  216 },
            { L_, 5.2013884668422196144942143e-199, "520138846684222", -198 },
            { L_, 5.2017228806424902896979965e-098, "520172288064249",  -97 },
            { L_, 5.2019009859778303020241225e+250, "520190098597783",  251 },
            { L_, 5.2037713404848696879656353e+138, "520377134048487",  139 },
            { L_, 5.2055053761354697250997856e-074, "520550537613547",  -73 },
            { L_, 5.2074236307876701044819017e-174, "520742363078767", -173 },
            { L_, 5.2078867505337202305165463e+248, "520788675053372",  249 },
            { L_, 5.2079010685137499648516225e+293, "520790106851375",  294 },
            { L_, 5.2086379279385503289267859e+153, "520863792793855",  154 },
            { L_, 5.2103302911047399713175432e+249, "521033029110474",  250 },
            { L_, 5.2104970272944902891661340e-149, "521049702729449", -148 },
            { L_, 5.2112206415051699964194068e-093, "521122064150517",  -92 },
            { L_, 5.2144768906968100208721000e-108, "521447689069681", -107 },
            { L_, 5.2146127038080599217428930e+068, "521461270380806",   69 },
            { L_, 5.2146147413485697517134713e-288, "521461474134857", -287 },
            { L_, 5.2146899637245796987249615e-156, "521468996372458", -155 },
            { L_, 5.2190712448978302258099302e+214, "521907124489783",  215 },
            { L_, 5.2212953093770003235015962e-209,   "5221295309377", -208 },
            { L_, 5.2236632608791900282951634e-064, "522366326087919",  -63 },
            { L_, 5.2240678391556902123923720e+166, "522406783915569",  167 },
            { L_, 5.2247137470968902827351291e-095, "522471374709689",  -94 },
            { L_, 5.2283831181045704378522068e-150, "522838311810457", -149 },
            { L_, 5.2288654085950395889594186e-072, "522886540859504",  -71 },
            { L_, 5.2291569524881099627919825e-094, "522915695248811",  -93 },
            { L_, 5.2315992100899297806364082e-211, "523159921008993", -210 },
            { L_, 5.2346688818917202381103342e-213, "523466888189172", -212 },
            { L_, 5.2351987176941898943634838e+038, "523519871769419",   39 },
            { L_, 5.2356794120317598572000861e+005, "523567941203176",    6 },
            { L_, 5.2361450106212401755820838e-255, "523614501062124", -254 },
            { L_, 5.2373859673183596690285481e-223, "523738596731836", -222 },
            { L_, 5.2406955627696799181161558e+175, "524069556276968",  176 },
            { L_, 5.2422070598886802373617938e+250, "524220705988868",  251 },
            { L_, 5.2432808499375496353639980e+198, "524328084993755",  199 },
            { L_, 5.2433528276699599275104808e-101, "524335282766996", -100 },
            { L_, 5.2480156089332402727475547e-037, "524801560893324",  -36 },
            { L_, 5.2480422611135202659888080e-111, "524804226111352", -110 },
            { L_, 5.2510074140366297891710991e+032, "525100741403663",   33 },
            { L_, 5.2517646810834395949549920e-119, "525176468108344", -118 },
            { L_, 5.2539869523914397765802589e-149, "525398695239144", -148 },
            { L_, 5.2549782226649196007694359e-069, "525497822266492",  -68 },
            { L_, 5.2565445976515095160032907e-187, "525654459765151", -186 },
            { L_, 5.2573854258082196038076084e-243, "525738542580822", -242 },
            { L_, 5.2594469077603003641010340e-175,  "52594469077603", -174 },
            { L_, 5.2611327187770802964937913e+252, "526113271877708",  253 },
            { L_, 5.2661253265955798825314481e+162, "526612532659558",  163 },
            { L_, 5.2697431531229001961644169e+307,  "52697431531229",  308 },
            { L_, 5.2711618955927799657024238e+065, "527116189559278",   66 },
            { L_, 5.2735012647239000450178690e+047,  "52735012647239",   48 },
            { L_, 5.2751161269036903724456462e+192, "527511612690369",  193 },
            { L_, 5.2752224753461804431434499e-143, "527522247534618", -142 },
            { L_, 5.2786719248227797720474370e-185, "527867192482278", -184 },
            { L_, 5.2793691590334897385938352e-186, "527936915903349", -185 },
            { L_, 5.2802790553941204747011080e-181, "528027905539412", -180 },
            { L_, 5.2811220539071798166280165e+078, "528112205390718",   79 },
            { L_, 5.2821225331880101971530847e-193, "528212253318801", -192 },
            { L_, 5.2835350904934495549264627e+220, "528353509049345",  221 },
            { L_, 5.2836090000376096863200174e-007, "528360900003761",   -6 },
            { L_, 5.2860137319071001091528918e+264,  "52860137319071",  265 },
            { L_, 5.2861112897061997140981018e-242,  "52861112897062", -241 },
            { L_, 5.2872658350039697449827698e-216, "528726583500397", -215 },
            { L_, 5.2890908966028298615570215e+255, "528909089660283",  256 },
            { L_, 5.2904422069402400972635166e+112, "529044220694024",  113 },
            { L_, 5.2904857099678603597439975e-167, "529048570996786", -166 },
            { L_, 5.2905741467425597440000000e+018, "529057414674256",   19 },
            { L_, 5.2957068088871797102495459e-018, "529570680888718",  -17 },
            { L_, 5.2959886835408705451335810e-116, "529598868354087", -115 },
            { L_, 5.2983701802204897048406119e-210, "529837018022049", -209 },
            { L_, 5.3010719299921797146667267e-292, "530107192999218", -291 },
            { L_, 5.3014591724550900444508120e+160, "530145917245509",  161 },
            { L_, 5.3014765397446501973982141e+298, "530147653974465",  299 },
            { L_, 5.3018246187373198637919989e+166, "530182461873732",  167 },
            { L_, 5.3033725456025896684856830e-058, "530337254560259",  -57 },
            { L_, 5.3036760268213404732269527e+027, "530367602682134",   28 },
            { L_, 5.3048324593061803403205394e-075, "530483245930618",  -74 },
            { L_, 5.3106146657780098202383690e+039, "531061466577801",   40 },
            { L_, 5.3113966752568502206871380e+295, "531139667525685",  296 },
            { L_, 5.3115046799858900802613766e-018, "531150467998589",  -17 },
            { L_, 5.3131018048707199834309977e-201, "531310180487072", -200 },
            { L_, 5.3138923376994601073960958e-145, "531389233769946", -144 },
            { L_, 5.3147223147283600283288100e-023, "531472231472836",  -22 },
            { L_, 5.3175022129350604775766997e+260, "531750221293506",  261 },
            { L_, 5.3185775089369598524171947e+228, "531857750893696",  229 },
            { L_, 5.3202446249398798031437145e-261, "532024462493988", -260 },
            { L_, 5.3208274394072401502632132e+088, "532082743940724",   89 },
            { L_, 5.3213006860987397310488905e-251, "532130068609874", -250 },
            { L_, 5.3213706651082299475582903e-016, "532137066510823",  -15 },
            { L_, 5.3216759256839597963543592e-104, "532167592568396", -103 },
            { L_, 5.3242742759618501421518943e-009, "532427427596185",   -8 },
            { L_, 5.3256585900413899935429101e-152, "532565859004139", -151 },
            { L_, 5.3274900127096598983666258e-164, "532749001270966", -163 },
            { L_, 5.3302825483526102786087167e+162, "533028254835261",  163 },
            { L_, 5.3324611980230798374330635e-124, "533246119802308", -123 },
            { L_, 5.3329049391257295186542653e-271, "533290493912573", -270 },
            { L_, 5.3363750140107299557712395e+002, "533637501401073",    3 },
            { L_, 5.3381762477134500196492472e+217, "533817624771345",  218 },
            { L_, 5.3389354959526001295686933e-053,  "53389354959526",  -52 },
            { L_, 5.3431129044274196587153913e-110, "534311290442742", -109 },
            { L_, 5.3456077189997194888649490e+108, "534560771899972",  109 },
            { L_, 5.3462177125222699422268233e-178, "534621771252227", -177 },
            { L_, 5.3476784350830695340196234e-265, "534767843508307", -264 },
            { L_, 5.3479888688388902236099665e-136, "534798886883889", -135 },
            { L_, 5.3485139384247003657791478e-189,  "53485139384247", -188 },
            { L_, 5.3520675118276697274694423e+149, "535206751182767",  150 },
            { L_, 5.3537876641041198858094950e+037, "535378766410412",   38 },
            { L_, 5.3539099793654698155485389e+233, "535390997936547",  234 },
            { L_, 5.3541595243500003738369864e+307,    "535415952435",  308 },
            { L_, 5.3542387971818402705549169e-107, "535423879718184", -106 },
            { L_, 5.3562282808904999660740367e-279,  "53562282808905", -278 },
            { L_, 5.3576260785618697474935115e+143, "535762607856187",  144 },
            { L_, 5.3588383234461500636705227e-020, "535883832344615",  -19 },
            { L_, 5.3628103849823499211933452e+149, "536281038498235",  150 },
            { L_, 5.3641522570989200175972655e+003, "536415225709892",    4 },
            { L_, 5.3669265734231301920208064e+187, "536692657342313",  188 },
            { L_, 5.3728870061917297275235843e-264, "537288700619173", -263 },
            { L_, 5.3746786112080501422712176e-113, "537467861120805", -112 },
            { L_, 5.3750290369502496971956305e+037, "537502903695025",   38 },
            { L_, 5.3757188475212899447797099e+002, "537571884752129",    3 },
            { L_, 5.3787773833196900545388044e+151, "537877738331969",  152 },
            { L_, 5.3788958616352902400000000e+017, "537889586163529",   18 },
            { L_, 5.3800299229147896197816314e-296, "538002992291479", -295 },
            { L_, 5.3802394498073594289924464e-141, "538023944980736", -140 },
            { L_, 5.3805954501869894614332991e+120, "538059545018699",  121 },
            { L_, 5.3814382625397303681432930e+193, "538143826253973",  194 },
            { L_, 5.3821450292199602940017078e+068, "538214502921996",   69 },
            { L_, 5.3844322306989501518764554e+062, "538443223069895",   63 },
            { L_, 5.3858475203000397064377678e-209, "538584752030004", -208 },
            { L_, 5.3907395526151302460773246e+262, "539073955261513",  263 },
            { L_, 5.3916297350930101562500000e+013, "539162973509301",   14 },
            { L_, 5.3929987837954004703079695e-255,  "53929987837954", -254 },
            { L_, 5.3940398162798101196449579e+120, "539403981627981",  121 },
            { L_, 5.3952559446487903385451599e+164, "539525594464879",  165 },
            { L_, 5.3954285571192298210289785e-195, "539542855711923", -194 },
            { L_, 5.3964408570114499574014476e+170, "539644085701145",  171 },
            { L_, 5.3976633697650001621632565e+240,   "5397663369765",  241 },
            { L_, 5.4026178215008098530295802e+032, "540261782150081",   33 },
            { L_, 5.4074724166484095815058880e+223, "540747241664841",  224 },
            { L_, 5.4076394544683796336752919e-127, "540763945446838", -126 },
            { L_, 5.4101557584361503116705186e-049, "541015575843615",  -48 },
            { L_, 5.4104314293233999153609394e+116,  "54104314293234",  117 },
            { L_, 5.4109358127493497719466259e+161, "541093581274935",  162 },
            { L_, 5.4116286526607703397903477e+263, "541162865266077",  264 },
            { L_, 5.4161476638637401705107131e+191, "541614766386374",  192 },
            { L_, 5.4189199474177299200000000e+017, "541891994741773",   18 },
            { L_, 5.4210172999834704860505828e-206, "542101729998347", -205 },
            { L_, 5.4210891235533904268684877e+121, "542108912355339",  122 },
            { L_, 5.4213354383801197350058308e+179, "542133543838012",  180 },
            { L_, 5.4242447534716201934005010e-037, "542424475347162",  -36 },
            { L_, 5.4256784195881402197934193e-256, "542567841958814", -255 },
            { L_, 5.4265602420387602731972584e+186, "542656024203876",  187 },
            { L_, 5.4276164871506902652007340e-073, "542761648715069",  -72 },
            { L_, 5.4291053004662694646890226e+213, "542910530046627",  214 },
            { L_, 5.4297726089680599843241826e-259, "542977260896806", -258 },
            { L_, 5.4315429040662302973716094e-011, "543154290406623",  -10 },
            { L_, 5.4331522756566501313009022e-280, "543315227565665", -279 },
            { L_, 5.4388804465760201702530982e-228, "543888044657602", -227 },
            { L_, 5.4389090515801398101206933e-054, "543890905158014",  -53 },
            { L_, 5.4406894046566600191606262e+044, "544068940465666",   45 },
            { L_, 5.4410287571694797629805706e-083, "544102875716948",  -82 },
            { L_, 5.4431086929098100404131009e-009, "544310869290981",   -8 },
            { L_, 5.4437682746146103771635598e+067, "544376827461461",   68 },
            { L_, 5.4449576403333695735912438e-299, "544495764033337", -298 },
            { L_, 5.4452546920138299955418149e-301, "544525469201383", -300 },
            { L_, 5.4459025674349595683393710e+152, "544590256743496",  153 },
            { L_, 5.4470116623639701360638608e-038, "544701166236397",  -37 },
            { L_, 5.4474656761550497539296688e+287, "544746567615505",  288 },
            { L_, 5.4480410924309599204378840e+190, "544804109243096",  191 },
            { L_, 5.4486224515686197145093094e-033, "544862245156862",  -32 },
            { L_, 5.4487313161176801546166041e-251, "544873131611768", -250 },
            { L_, 5.4534026356390997314453125e+011,  "54534026356391",   12 },
            { L_, 5.4542584660121002613604961e+125,  "54542584660121",  126 },
            { L_, 5.4545139486417000389947045e-128,  "54545139486417", -127 },
            { L_, 5.4552900320945399698737684e-030, "545529003209454",  -29 },
            { L_, 5.4580861108078402414629021e-151, "545808611080784", -150 },
            { L_, 5.4582615520277697072704776e-307, "545826155202777", -306 },
            { L_, 5.4630540303348800407470977e-281, "546305403033488", -280 },
            { L_, 5.4645650251879399178699028e-293, "546456502518794", -292 },
            { L_, 5.4647378591723801857630306e-112, "546473785917238", -111 },
            { L_, 5.4732820965380004590167072e-116,   "5473282096538", -115 },
            { L_, 5.4743823739166098653639282e+178, "547438237391661",  179 },
            { L_, 5.4786631680476398010365682e+243, "547866316804764",  244 },
            { L_, 5.4796481201362400000000000e+016, "547964812013624",   17 },
            { L_, 5.4799111593857602672461211e+295, "547991115938576",  296 },
            { L_, 5.4838231861788696285613372e+044, "548382318617887",   45 },
            { L_, 5.4841089612461196471046413e+088, "548410896124612",   89 },
            { L_, 5.4846781186788695823129002e+092, "548467811867887",   93 },
            { L_, 5.4850057986507599728864933e+127, "548500579865076",  128 },
            { L_, 5.4864160014348999023437500e+010,  "54864160014349",   11 },
            { L_, 5.4886173990810303680423236e-029, "548861739908103",  -28 },
            { L_, 5.4907315685353299791156881e-180, "549073156853533", -179 },
            { L_, 5.4914599210945299784584446e-154, "549145992109453", -153 },
            { L_, 5.4961069089400505168280476e-302, "549610690894005", -301 },
            { L_, 5.4962357903233402594732787e-276, "549623579032334", -275 },
            { L_, 5.4970609444032402112212054e-077, "549706094440324",  -76 },
            { L_, 5.4992555057776099764735781e+192, "549925550577761",  193 },
            { L_, 5.4993372343199901685181423e-223, "549933723431999", -222 },
            { L_, 5.4993796106981597062478774e+132, "549937961069816",  133 },
            { L_, 5.5016296193837104084467559e-262, "550162961938371", -261 },
            { L_, 5.5027158614061401344330857e+243, "550271586140614",  244 },
            { L_, 5.5034564091463201861541673e-131, "550345640914632", -130 },
            { L_, 5.5040587438695101380088659e-006, "550405874386951",   -5 },
            { L_, 5.5044525035709098498220201e-131, "550445250357091", -130 },
            { L_, 5.5085604760117902835712000e+022, "550856047601179",   23 },
            { L_, 5.5140776158185198225948473e-300, "551407761581852", -299 },
            { L_, 5.5150025937176802759815741e+225, "551500259371768",  226 },
            { L_, 5.5150146386467798331536711e-020, "551501463864678",  -19 },
            { L_, 5.5193067641438601884425998e-239, "551930676414386", -238 },
            { L_, 5.5196424534066000861174591e-065,  "55196424534066",  -64 },
            { L_, 5.5220875364165304156445686e+074, "552208753641653",   75 },
            { L_, 5.5249858367170601373923080e+226, "552498583671706",  227 },
            { L_, 5.5259464067792700249490404e+119, "552594640677927",  120 },
            { L_, 5.5272498961577501723863733e-128, "552724989615775", -127 },
            { L_, 5.5313013845687402244212826e-090, "553130138456874",  -89 },
            { L_, 5.5330705956433899214069035e+090, "553307059564339",   91 },
            { L_, 5.5342172173000601316114945e+086, "553421721730006",   87 },
            { L_, 5.5343545551063405823781435e+067, "553435455510634",   68 },
            { L_, 5.5345259484958498444877777e-123, "553452594849585", -122 },
            { L_, 5.5348553277064096252200436e+205, "553485532770641",  206 },
            { L_, 5.5351279268388896384860397e-286, "553512792683889", -285 },
            { L_, 5.5355407469878401270328801e+227, "553554074698784",  228 },
            { L_, 5.5359805811491599287151312e+050, "553598058114916",   51 },
            { L_, 5.5368749214752797724846258e-283, "553687492147528", -282 },
            { L_, 5.5374350632546503019844682e+294, "553743506325465",  295 },
            { L_, 5.5384055693182003122521493e-019,  "55384055693182",  -18 },
            { L_, 5.5389911793128700859758304e-065, "553899117931287",  -64 },
            { L_, 5.5397957550261604420053118e+204, "553979575502616",  205 },
            { L_, 5.5428509297343401609011035e-223, "554285092973434", -222 },
            { L_, 5.5429085928584100246598043e-006, "554290859285841",   -5 },
            { L_, 5.5430678807008301923960442e+303, "554306788070083",  304 },
            { L_, 5.5431428113810799388664514e+128, "554314281138108",  129 },
            { L_, 5.5437794847285800749679938e-266, "554377948472858", -265 },
            { L_, 5.5439058094053802920382212e-030, "554390580940538",  -29 },
            { L_, 5.5440577922471703295752243e-281, "554405779224717", -280 },
            { L_, 5.5445574437377300384202533e-125, "554455744373773", -124 },
            { L_, 5.5460248345670499462052583e-005, "554602483456705",   -4 },
            { L_, 5.5463351571376504277484095e+213, "554633515713765",  214 },
            { L_, 5.5499136170388597033368318e+227, "554991361703886",  228 },
            { L_, 5.5516137024609298706054688e+010, "555161370246093",   11 },
            { L_, 5.5532946470839396682639411e-004, "555329464708394",   -3 },
            { L_, 5.5539715820762804400260405e-218, "555397158207628", -217 },
            { L_, 5.5542508108570098516530277e-174, "555425081085701", -173 },
            { L_, 5.5589052055548198555246274e+203, "555890520555482",  204 },
            { L_, 5.5589965958484898911220941e+227, "555899659584849",  228 },
            { L_, 5.5623520683852102374804529e-245, "556235206838521", -244 },
            { L_, 5.5626844507407397192472349e+131, "556268445074074",  132 },
            { L_, 5.5635365707392198940038702e-042, "556353657073922",  -41 },
            { L_, 5.5648377229122698048659322e-152, "556483772291227", -151 },
            { L_, 5.5657095587277199228992686e+003, "556570955872772",    4 },
            { L_, 5.5658986234698296987004332e-257, "556589862346983", -256 },
            { L_, 5.5672015761652599223301752e-018, "556720157616526",  -17 },
            { L_, 5.5673493118202697909073141e+092, "556734931182027",   93 },
            { L_, 5.5686710482436299953339539e-118, "556867104824363", -117 },
            { L_, 5.5709297360803502120189329e+052, "557092973608035",   53 },
            { L_, 5.5725662504549702260955685e+212, "557256625045497",  213 },
            { L_, 5.5731922357032695979593695e-006, "557319223570327",   -5 },
            { L_, 5.5735369522950299458561329e-068, "557353695229503",  -67 },
            { L_, 5.5757598180658200310314043e-224, "557575981806582", -223 },
            { L_, 5.5864956230293397596880745e-305, "558649562302934", -304 },
            { L_, 5.5866918009871002286672592e+006,  "55866918009871",    7 },
            { L_, 5.5886210993885005195936846e-225,  "55886210993885", -224 },
            { L_, 5.5908598109859097262189919e-253, "559085981098591", -252 },
            { L_, 5.5919248659042002598972602e+255,  "55919248659042",  256 },
            { L_, 5.5936647937842797851562500e+011, "559366479378428",   12 },
            { L_, 5.5957795571604400371919148e-063, "559577955716044",  -62 },
            { L_, 5.5972028983091796379930654e-063, "559720289830918",  -62 },
            { L_, 5.5988837593634297594691574e-268, "559888375936343", -267 },
            { L_, 5.5999989796463801734553973e+120, "559999897964638",  121 },
            { L_, 5.6021376904972100667047551e-265, "560213769049721", -264 },
            { L_, 5.6049332077421700904185409e+176, "560493320774217",  177 },
            { L_, 5.6058611142995103225916369e-276, "560586111429951", -275 },
            { L_, 5.6065806439871704782254903e+127, "560658064398717",  128 },
            { L_, 5.6077105467889698450518874e+221, "560771054678897",  222 },
            { L_, 5.6079724484310395457215993e+055, "560797244843104",   56 },
            { L_, 5.6081263025107701720921055e-272, "560812630251077", -271 },
            { L_, 5.6099767079191099233325975e+285, "560997670791911",  286 },
            { L_, 5.6122497939811199949364671e-086, "561224979398112",  -85 },
            { L_, 5.6159275598400899828431346e+224, "561592755984009",  225 },
            { L_, 5.6206310686456298398006357e+188, "562063106864563",  189 },
            { L_, 5.6221496787684501568421870e+169, "562214967876845",  170 },
            { L_, 5.6223425628233100726213220e-100, "562234256282331",  -99 },
            { L_, 5.6227853342162200880369736e-229, "562278533421622", -228 },
            { L_, 5.6249698781739401775029143e+115, "562496987817394",  116 },
            { L_, 5.6258859077469798623251055e+134, "562588590774698",  135 },
            { L_, 5.6336447415753200000000000e+016, "563364474157532",   17 },
            { L_, 5.6403460889345697864728854e+180, "564034608893457",  181 },
            { L_, 5.6444863536621999847975406e+093,  "56444863536622",   94 },
            { L_, 5.6449162901160996935062339e-197,  "56449162901161", -196 },
            { L_, 5.6476093492980899823163590e+184, "564760934929809",  185 },
            { L_, 5.6487313881740698505430575e+187, "564873138817407",  188 },
            { L_, 5.6499827359195401257699960e-114, "564998273591954", -113 },
            { L_, 5.6509490444450996067643207e-132,  "56509490444451", -131 },
            { L_, 5.6514920449791696395776219e-080, "565149204497917",  -79 },
            { L_, 5.6547139829023503102623064e-142, "565471398290235", -141 },
            { L_, 5.6553971435667903262569685e+121, "565539714356679",  122 },
            { L_, 5.6575437367371196746826172e+009, "565754373673712",   10 },
            { L_, 5.6585829543652902037319590e-061, "565858295436529",  -60 },
            { L_, 5.6586916725563696283743894e+196, "565869167255637",  197 },
            { L_, 5.6594077469206696520115800e+181, "565940774692067",  182 },
            { L_, 5.6615284915223402188853262e-213, "566152849152234", -212 },
            { L_, 5.6622678560823200410281598e-180, "566226785608232", -179 },
            { L_, 5.6635587670179402363864561e-180, "566355876701794", -179 },
            { L_, 5.6640466247095999221076629e+189,  "56640466247096",  190 },
            { L_, 5.6652779597732304218861935e-091, "566527795977323",  -90 },
            { L_, 5.6657098475741799077821809e+159, "566570984757418",  160 },
            { L_, 5.6657230440176299523849076e-294, "566572304401763", -293 },
            { L_, 5.6676641841428399987757053e-171, "566766418414284", -170 },
            { L_, 5.6686768429866596654778583e+042, "566867684298666",   43 },
            { L_, 5.6688917073913903836057259e+294, "566889170739139",  295 },
            { L_, 5.6714350987402197499167730e-121, "567143509874022", -120 },
            { L_, 5.6715407503569403047704926e-276, "567154075035694", -275 },
            { L_, 5.6742023288998505805159914e+306, "567420232889985",  307 },
            { L_, 5.6799879243285997626541712e+055,  "56799879243286",   56 },
            { L_, 5.6805500673505803726982961e+192, "568055006735058",  193 },
            { L_, 5.6838752057225801946796391e+282, "568387520572258",  283 },
            { L_, 5.6855634480706396864863194e-059, "568556344807064",  -58 },
            { L_, 5.6902745815026595940735508e+295, "569027458150266",  296 },
            { L_, 5.6914693140784302358464633e-112, "569146931407843", -111 },
            { L_, 5.6920101858856704096114921e-287, "569201018588567", -286 },
            { L_, 5.6963171481705996566341095e-306,  "56963171481706", -305 },
            { L_, 5.6978089786626302526936374e+101, "569780897866263",  102 },
            { L_, 5.6987302384261796857142546e-128, "569873023842618", -127 },
            { L_, 5.7010850033794605776309087e+073, "570108500337946",   74 },
            { L_, 5.7019284969436002461117786e+162,  "57019284969436",  163 },
            { L_, 5.7022198141444297507405281e+006, "570221981414443",    7 },
            { L_, 5.7047030549590003283393745e-072,   "5704703054959",  -71 },
            { L_, 5.7048259551335203241378928e+094, "570482595513352",   95 },
            { L_, 5.7066406161036399727474877e+165, "570664061610364",  166 },
            { L_, 5.7109769098567595348202811e-019, "571097690985676",  -18 },
            { L_, 5.7116580144842800441494829e+127, "571165801448428",  128 },
            { L_, 5.7159384278540594719249491e-110, "571593842785406", -109 },
            { L_, 5.7159785217301500936641093e-139, "571597852173015", -138 },
            { L_, 5.7165959039898200767839947e-275, "571659590398982", -274 },
            { L_, 5.7167646335863698270074819e+194, "571676463358637",  195 },
            { L_, 5.7223234817856903076171875e+010, "572232348178569",   11 },
            { L_, 5.7241989648312798622428348e+032, "572419896483128",   33 },
            { L_, 5.7247975503699201944429724e-121, "572479755036992", -120 },
            { L_, 5.7260848024943297038405865e+050, "572608480249433",   51 },
            { L_, 5.7322046012304297065277625e-058, "573220460123043",  -57 },
            { L_, 5.7324743731950296632597248e-140, "573247437319503", -139 },
            { L_, 5.7342845085099501813760000e+021, "573428450850995",   22 },
            { L_, 5.7344376020638003200000000e+017,  "57344376020638",   18 },
            { L_, 5.7354871465373298049801973e-254, "573548714653733", -253 },
            { L_, 5.7362247354592003570517824e+126,  "57362247354592",  127 },
            { L_, 5.7374756476823698063127137e-298, "573747564768237", -297 },
            { L_, 5.7399991143584604201147073e-131, "573999911435846", -130 },
            { L_, 5.7415008913814401963951299e-149, "574150089138144", -148 },
            { L_, 5.7461586014686798912221022e-003, "574615860146868",   -2 },
            { L_, 5.7464150259596997126046334e-204,  "57464150259597", -203 },
            { L_, 5.7489043953813303482013930e+220, "574890439538133",  221 },
            { L_, 5.7511877414469901104162066e-279, "575118774144699", -278 },
            { L_, 5.7530176105376299089920000e+021, "575301761053763",   22 },
            { L_, 5.7539933688316496269269423e-091, "575399336883165",  -90 },
            { L_, 5.7540620094724999248072757e+255,  "57540620094725",  256 },
            { L_, 5.7564783890324198058978143e-105, "575647838903242", -104 },
            { L_, 5.7572173801327294065332250e-306, "575721738013273", -305 },
            { L_, 5.7587097058505101383570167e+298, "575870970585051",  299 },
            { L_, 5.7641601056376595389657394e+254, "576416010563766",  255 },
            { L_, 5.7664301906600798462215784e-077, "576643019066008",  -76 },
            { L_, 5.7664645403120399600192395e-150, "576646454031204", -149 },
            { L_, 5.7687196887286399141231124e-217, "576871968872864", -216 },
            { L_, 5.7699282462664300511732764e+198, "576992824626643",  199 },
            { L_, 5.7726468327263100829679483e-186, "577264683272631", -185 },
            { L_, 5.7730217320621700881077983e+234, "577302173206217",  235 },
            { L_, 5.7748703866702804565429688e+009, "577487038667028",   10 },
            { L_, 5.7754293452189501959395819e-180, "577542934521895", -179 },
            { L_, 5.7758609621235901892924724e-264, "577586096212359", -263 },
            { L_, 5.7779633210281704330886611e-147, "577796332102817", -146 },
            { L_, 5.7793581118339195329390930e+307, "577935811183392",  308 },
            { L_, 5.7796447553376497232600992e-110, "577964475533765", -109 },
            { L_, 5.7797303136014396489383055e+297, "577973031360144",  298 },
            { L_, 5.7817359590884000868014192e-152,  "57817359590884", -151 },
            { L_, 5.7848235217676204912635134e+195, "578482352176762",  196 },
            { L_, 5.7888213226025400210265233e+048, "578882132260254",   49 },
            { L_, 5.7907308166302700058736264e+000, "579073081663027",    1 },
            { L_, 5.7912604115809595338671853e-079, "579126041158096",  -78 },
            { L_, 5.7915829286559594924483568e+105, "579158292865596",  106 },
            { L_, 5.7921617262618598137983054e+180, "579216172626186",  181 },
            { L_, 5.7938889223908402159672292e-198, "579388892239084", -197 },
            { L_, 5.7952732085665702418055168e+026, "579527320856657",   27 },
            { L_, 5.7953170377253596448619000e-003, "579531703772536",   -2 },
            { L_, 5.7992303823093703465984350e+159, "579923038230937",  160 },
            { L_, 5.8005098870666794626198340e+076, "580050988706668",   77 },
            { L_, 5.8031730052606701699121819e+166, "580317300526067",  167 },
            { L_, 5.8036796129785501260027021e-069, "580367961297855",  -68 },
            { L_, 5.8041011306119601360365118e-023, "580410113061196",  -22 },
            { L_, 5.8048601462966101132072888e+114, "580486014629661",  115 },
            { L_, 5.8084141273335600650602570e+116, "580841412733356",  117 },
            { L_, 5.8114345723699600614624980e-204, "581143457236996", -203 },
            { L_, 5.8132729347505597155484833e-090, "581327293475056",  -89 },
            { L_, 5.8140860292736394853905605e+070, "581408602927364",   71 },
            { L_, 5.8151115133849299488213678e-122, "581511151338493", -121 },
            { L_, 5.8161728031367599181450655e+030, "581617280313676",   31 },
            { L_, 5.8169596574105700199652732e-078, "581695965741057",  -77 },
            { L_, 5.8179222088942094888468480e+024, "581792220889421",   25 },
            { L_, 5.8188985399109199671599939e-285, "581889853991092", -284 },
            { L_, 5.8196163695957102905903923e-160, "581961636959571", -159 },
            { L_, 5.8218133599727096610806310e+045, "582181335997271",   46 },
            { L_, 5.8233262003707995329935608e+033,  "58233262003708",   34 },
            { L_, 5.8239798396990404697604584e-029, "582397983969904",  -28 },
            { L_, 5.8258599953710303746419735e-175, "582585999537103", -174 },
            { L_, 5.8265784833159301755288557e-061, "582657848331593",  -60 },
            { L_, 5.8267787048901701110264487e+098, "582677870489017",   99 },
            { L_, 5.8272548066044198423584639e-048, "582725480660442",  -47 },
            { L_, 5.8277093551910996628855120e+105,  "58277093551911",  106 },
            { L_, 5.8283257897093201294973750e+194, "582832578970932",  195 },
            { L_, 5.8351798559270601545700673e-113, "583517985592706", -112 },
            { L_, 5.8358611436873095123364482e+188, "583586114368731",  189 },
            { L_, 5.8386667196399399858552190e+168, "583866671963994",  169 },
            { L_, 5.8406411828912404630525850e-199, "584064118289124", -198 },
            { L_, 5.8408066348741499849692835e+261, "584080663487415",  262 },
            { L_, 5.8441675503032397841512129e+157, "584416755030324",  158 },
            { L_, 5.8448601443364303153916799e-202, "584486014433643", -201 },
            { L_, 5.8450226655391197796700425e+262, "584502266553912",  263 },
            { L_, 5.8459258558136597419052216e+095, "584592585581366",   96 },
            { L_, 5.8463733024316997204451600e-154,  "58463733024317", -153 },
            { L_, 5.8474906933412000428537649e+133,  "58474906933412",  134 },
            { L_, 5.8475888316095703634147426e+033, "584758883160957",   34 },
            { L_, 5.8479154876348202290670607e-150, "584791548763482", -149 },
            { L_, 5.8498435808108398679775517e+243, "584984358081084",  244 },
            { L_, 5.8505397739052996926499987e-248,  "58505397739053", -247 },
            { L_, 5.8556949932301998414795204e-117,  "58556949932302", -116 },
            { L_, 5.8568175943943400422456154e+282, "585681759439434",  283 },
            { L_, 5.8569480534663496426601400e-078, "585694805346635",  -77 },
            { L_, 5.8572806483532103238098365e+059, "585728064835321",   60 },
            { L_, 5.8600335697188198180578905e-125, "586003356971882", -124 },
            { L_, 5.8608842895257298654270985e+082, "586088428952573",   83 },
            { L_, 5.8624215854262104837055303e-035, "586242158542621",  -34 },
            { L_, 5.8650851666144703250978315e+169, "586508516661447",  170 },
            { L_, 5.8653658940785302178455261e+111, "586536589407853",  112 },
            { L_, 5.8665799352520802616292457e-057, "586657993525208",  -56 },
            { L_, 5.8689960513377203384267294e-158, "586899605133772", -157 },
            { L_, 5.8692317972351598859942711e-227, "586923179723516", -226 },
            { L_, 5.8695813075844700851169377e-021, "586958130758447",  -20 },
            { L_, 5.8698124552951402098575871e-010, "586981245529514",   -9 },
            { L_, 5.8706595426318298963279059e-080, "587065954263183",  -79 },
            { L_, 5.8736749326314297435057491e-267, "587367493263143", -266 },
            { L_, 5.8738962120569497059328000e+022, "587389621205695",   23 },
            { L_, 5.8748590617141801417935400e-210, "587485906171418", -209 },
            { L_, 5.8800669213926901616504895e+116, "588006692139269",  117 },
            { L_, 5.8812096668161302118671346e+085, "588120966681613",   86 },
            { L_, 5.8812957184831200126904708e-250, "588129571848312", -249 },
            { L_, 5.8828448413256696593639364e+080, "588284484132567",   81 },
            { L_, 5.8840163833495402854639502e-064, "588401638334954",  -63 },
            { L_, 5.8842326969623405116593877e-138, "588423269696234", -137 },
            { L_, 5.8850142455733797847629927e+237, "588501424557338",  238 },
            { L_, 5.8853490352304798492694824e-062, "588534903523048",  -61 },
            { L_, 5.8871080174751901367589055e+210, "588710801747519",  211 },
            { L_, 5.8877565130575400766175274e-052, "588775651305754",  -51 },
            { L_, 5.8899708601228399148285582e+045, "588997086012284",   46 },
            { L_, 5.8906170695523595372007289e-135, "589061706955236", -134 },
            { L_, 5.8925914580641700421291213e+177, "589259145806417",  178 },
            { L_, 5.8939156084040403602197649e+301, "589391560840404",  302 },
            { L_, 5.8963925328000001571087510e-061,     "58963925328",  -60 },
            { L_, 5.8977744058588698354700656e-170, "589777440585887", -169 },
            { L_, 5.8994261109196296910453795e-089, "589942611091963",  -88 },
            { L_, 5.9000370828184997754515652e-144,  "59000370828185", -143 },
            { L_, 5.9004665993761702644071706e-037, "590046659937617",  -36 },
            { L_, 5.9016208030610798365126783e+076, "590162080306108",   77 },
            { L_, 5.9022406967384697520733572e+033, "590224069673847",   34 },
            { L_, 5.9022755252710598166790180e+249, "590227552527106",  250 },
            { L_, 5.9038986815444200072372609e-122, "590389868154442", -121 },
            { L_, 5.9069497790851699937997332e+201, "590694977908517",  202 },
            { L_, 5.9074388131274202305228781e+218, "590743881312742",  219 },
            { L_, 5.9098903638572797732553247e-125, "590989036385728", -124 },
            { L_, 5.9100406093107898544922694e+048, "591004060931079",   49 },
            { L_, 5.9107520992772301120259306e+059, "591075209927723",   60 },
            { L_, 5.9128035998854897077435722e-246, "591280359988549", -245 },
            { L_, 5.9136712781411601682710199e+111, "591367127814116",  112 },
            { L_, 5.9157970509408300391407133e-035, "591579705094083",  -34 },
            { L_, 5.9186370992050097388257280e+024, "591863709920501",   25 },
            { L_, 5.9194648681729598884171087e+116, "591946486817296",  117 },
            { L_, 5.9204163071142999292897600e+057,  "59204163071143",   58 },
            { L_, 5.9206036332873003459158373e+285,  "59206036332873",  286 },
            { L_, 5.9211740037861296147458145e+280, "592117400378613",  281 },
            { L_, 5.9223675800741204051779821e+112, "592236758007412",  113 },
            { L_, 5.9241729188810900874538996e-133, "592417291888109", -132 },
            { L_, 5.9262791245031504304722502e+306, "592627912450315",  307 },
            { L_, 5.9264030248411100284031955e+208, "592640302484111",  209 },
            { L_, 5.9268881631665202246006334e-083, "592688816316652",  -82 },
            { L_, 5.9283160143022695365624719e+254, "592831601430227",  255 },
            { L_, 5.9284193403953299745540842e-249, "592841934039533", -248 },
            { L_, 5.9289080624493496753770849e-060, "592890806244935",  -59 },
            { L_, 5.9290050100122401177047112e-021, "592900501001224",  -20 },
            { L_, 5.9293385123991199290967416e+220, "592933851239912",  221 },
            { L_, 5.9301728285135496742332267e+261, "593017282851355",  262 },
            { L_, 5.9303101252810202717570770e+231, "593031012528102",  232 },
            { L_, 5.9318849204943501699322912e-091, "593188492049435",  -90 },
            { L_, 5.9323538947719203518047657e-098, "593235389477192",  -97 },
            { L_, 5.9351318014049599836958016e+297, "593513180140496",  298 },
            { L_, 5.9355464550253898175901715e-189, "593554645502539", -188 },
            { L_, 5.9360512524845903709335121e+291, "593605125248459",  292 },
            { L_, 5.9361219661483106121312434e-014, "593612196614831",  -13 },
            { L_, 5.9368969973708899610437485e-271, "593689699737089", -270 },
            { L_, 5.9380304580806599901399377e-305, "593803045808066", -304 },
            { L_, 5.9380561181157902045921919e+241, "593805611811579",  242 },
            { L_, 5.9381337346115801637882686e-035, "593813373461158",  -34 },
            { L_, 5.9416810024058796599517658e+163, "594168100240588",  164 },
            { L_, 5.9421407099822905516874018e+163, "594214070998229",  164 },
            { L_, 5.9440341895435498258030242e-151, "594403418954355", -150 },
            { L_, 5.9486438526550301456828912e-077, "594864385265503",  -76 },
            { L_, 5.9503886433630097720001580e-145, "595038864336301", -144 },
            { L_, 5.9504039183639797677081764e+197, "595040391836398",  198 },
            { L_, 5.9525202329465604981683487e-066, "595252023294656",  -65 },
            { L_, 5.9525277806703394856830852e-275, "595252778067034", -274 },
            { L_, 5.9563880707859696366534586e-073, "595638807078597",  -72 },
            { L_, 5.9591602943610802856657071e+259, "595916029436108",  260 },
            { L_, 5.9597923262638701568594689e-121, "595979232626387", -120 },
            { L_, 5.9609210779858399066693731e+087, "596092107798584",   88 },
            { L_, 5.9626653600169296727282828e-222, "596266536001693", -221 },
            { L_, 5.9648482705098397090146593e-125, "596484827050984", -124 },
            { L_, 5.9655875091642597002779357e+126, "596558750916426",  127 },
            { L_, 5.9660055409118301530148618e-134, "596600554091183", -133 },
            { L_, 5.9671353870079499084856433e-156, "596713538700795", -155 },
            { L_, 5.9679708860510698994198641e-023, "596797088605107",  -22 },
            { L_, 5.9695280018401996886748624e-060,  "59695280018402",  -59 },
            { L_, 5.9708011720890598398502054e-264, "597080117208906", -263 },
            { L_, 5.9713351905071301720923918e-024, "597133519050713",  -23 },
            { L_, 5.9718489888753402654277469e+230, "597184898887534",  231 },
            { L_, 5.9719879709739898327924674e-179, "597198797097399", -178 },
            { L_, 5.9723636209009797013649772e+269, "597236362090098",  270 },
            { L_, 5.9728304713630098504737129e-228, "597283047136301", -227 },
            { L_, 5.9744466735859000841804090e+245,  "59744466735859",  246 },
            { L_, 5.9779691843555797220670763e-028, "597796918435558",  -27 },
            { L_, 5.9780984880966197252753132e+100, "597809848809662",  101 },
            { L_, 5.9803084287151694233175163e+073, "598030842871517",   74 },
            { L_, 5.9803615861946002224817855e+051,  "59803615861946",   52 },
            { L_, 5.9858907160995096229322028e-087, "598589071609951",  -86 },
            { L_, 5.9913263273731303417641746e+281, "599132632737313",  282 },
            { L_, 5.9939405144855302063358832e-007, "599394051448553",   -6 },
            { L_, 5.9953071324643603225911974e+226, "599530713246436",  227 },
            { L_, 5.9957331022029994984130528e+223,   "5995733102203",  224 },
            { L_, 5.9974705095926798982688410e+039, "599747050959268",   40 },
            { L_, 6.0009760795199204461035110e-073, "600097607951992",  -72 },
            { L_, 6.0010767188632498308238325e+265, "600107671886325",  266 },
            { L_, 6.0029489209612198465038091e+004, "600294892096122",    5 },
            { L_, 6.0030145861713303051770724e+036, "600301458617133",   37 },
            { L_, 6.0033086742145897526045288e-065, "600330867421459",  -64 },
            { L_, 6.0080367049244500000000000e+015, "600803670492445",   16 },
            { L_, 6.0086310858104499943425531e+297, "600863108581045",  298 },
            { L_, 6.0096453952824796603110069e-294, "600964539528248", -293 },
            { L_, 6.0099233840703198102414663e-237, "600992338407032", -236 },
            { L_, 6.0105300214486801416171534e-180, "601053002144868", -179 },
            { L_, 6.0105893855661699926834354e+109, "601058938556617",  110 },
            { L_, 6.0106605899736099197516000e+274, "601066058997361",  275 },
            { L_, 6.0117605079516202752051479e+050, "601176050795162",   51 },
            { L_, 6.0125182635174503772819917e-251, "601251826351745", -250 },
            { L_, 6.0137480740236696956927038e-025, "601374807402367",  -24 },
            { L_, 6.0143222638132600113749504e+006, "601432226381326",    7 },
            { L_, 6.0161046285212498958831726e-266, "601610462852125", -265 },
            { L_, 6.0195526053550100462684219e-168, "601955260535501", -167 },
            { L_, 6.0213651179182800866210308e-206, "602136511791828", -205 },
            { L_, 6.0226342284287500131673130e-106, "602263422842875", -105 },
            { L_, 6.0238231046878396971588642e+093, "602382310468784",   94 },
            { L_, 6.0239856372380595249772665e+256, "602398563723806",  257 },
            { L_, 6.0260410551505003640926384e-288,  "60260410551505", -287 },
            { L_, 6.0262442205318895780410367e-247, "602624422053189", -246 },
            { L_, 6.0266572021219697617971298e-219, "602665720212197", -218 },
            { L_, 6.0267760481827396651593012e-008, "602677604818274",   -7 },
            { L_, 6.0270130418779802028336974e+081, "602701304187798",   82 },
            { L_, 6.0291385484554702987823807e-213, "602913854845547", -212 },
            { L_, 6.0334551106992799069568445e+297, "603345511069928",  298 },
            { L_, 6.0337594398714601179014328e-304, "603375943987146", -303 },
            { L_, 6.0341000712697399199879888e-128, "603410007126974", -127 },
            { L_, 6.0364323803583702040641574e+100, "603643238035837",  101 },
            { L_, 6.0389515690301998885304198e+152,  "60389515690302",  153 },
            { L_, 6.0392324946237405092090775e+241, "603923249462374",  242 },
            { L_, 6.0392552525119401025012311e+243, "603925525251194",  244 },
            { L_, 6.0396827983957096333637679e-078, "603968279839571",  -77 },
            { L_, 6.0397714125930597964716574e-059, "603977141259306",  -58 },
            { L_, 6.0413558876921895194768682e-165, "604135588769219", -164 },
            { L_, 6.0414316430259498891838767e+252, "604143164302595",  253 },
            { L_, 6.0451712881336697640571083e-060, "604517128813367",  -59 },
            { L_, 6.0455277626473799029291385e-096, "604552776264738",  -95 },
            { L_, 6.0455928326321502742199240e-142, "604559283263215", -141 },
            { L_, 6.0475321663205800569770180e-302, "604753216632058", -301 },
            { L_, 6.0489641416131599885229570e-172, "604896414161316", -171 },
            { L_, 6.0522310231075702989222052e+027, "605223102310757",   28 },
            { L_, 6.0525630653301401921277649e-158, "605256306533014", -157 },
            { L_, 6.0532939628843103839415044e-068, "605329396288431",  -67 },
            { L_, 6.0549275300776997884005202e+184,  "60549275300777",  185 },
            { L_, 6.0593927152751001289651354e+037,  "60593927152751",   38 },
            { L_, 6.0596633486361295257782463e+051, "605966334863613",   52 },
            { L_, 6.0600330508487198588154917e-101, "606003305084872", -100 },
            { L_, 6.0615697546642588534196954e-309, "606156975466426", -308 },
            { L_, 6.0625502416032498163992690e+143, "606255024160325",  144 },
            { L_, 6.0629932600127603006867335e-182, "606299326001276", -181 },
            { L_, 6.0635464028143301427200000e+020, "606354640281433",   21 },
            { L_, 6.0645580239725799547574140e+175, "606455802397258",  176 },
            { L_, 6.0663548736544801832396990e-308, "606635487365448", -307 },
            { L_, 6.0681362681693999063756223e+212,  "60681362681694",  213 },
            { L_, 6.0690258742914198986359483e+087, "606902587429142",   88 },
            { L_, 6.0698981771123199723660946e+005, "606989817711232",    6 },
            { L_, 6.0710253211081801001800776e-220, "607102532110818", -219 },
            { L_, 6.0721037350305598482529142e-163, "607210373503056", -162 },
            { L_, 6.0732577199185800469236672e+196, "607325771991858",  197 },
            { L_, 6.0735558518917897302959585e-116, "607355585189179", -115 },
            { L_, 6.0746746520129595379055943e+155, "607467465201296",  156 },
            { L_, 6.0756637559015302893176141e+123, "607566375590153",  124 },
            { L_, 6.0775094526261602823069552e+002, "607750945262616",    3 },
            { L_, 6.0792383527849098712181330e+000, "607923835278491",    1 },
            { L_, 6.0807636048313898114448289e+260, "608076360483139",  261 },
            { L_, 6.0819709594706395643661864e+139, "608197095947064",  140 },
            { L_, 6.0853215306224598512515038e+124, "608532153062246",  125 },
            { L_, 6.0853585150481599276717614e-142, "608535851504816", -141 },
            { L_, 6.0874761741539901078391596e+185, "608747617415399",  186 },
            { L_, 6.0888515011957304008215861e+252, "608885150119573",  253 },
            { L_, 6.0888704728594396907336091e-084, "608887047285944",  -83 },
            { L_, 6.0903629495419897565966494e-001, "609036294954199",    0 },
            { L_, 6.0930161505381902267106924e-091, "609301615053819",  -90 },
            { L_, 6.0969400813105398064870438e-120, "609694008131054", -119 },
            { L_, 6.0974522231171901553357174e+272, "609745222311719",  273 },
            { L_, 6.0991563375027804818085411e-036, "609915633750278",  -35 },
            { L_, 6.0993073093781197190116593e-241, "609930730937812", -240 },
            { L_, 6.1000112629649697767361042e-108, "610001126296497", -107 },
            { L_, 6.1026914147578500879396984e-118, "610269141475785", -117 },
            { L_, 6.1030090884331101000063381e+277, "610300908843311",  278 },
            { L_, 6.1037695131422499091313333e-032, "610376951314225",  -31 },
            { L_, 6.1039029448905296666122935e+305, "610390294489053",  306 },
            { L_, 6.1044291934726097615572194e-060, "610442919347261",  -59 },
            { L_, 6.1071202689751395944529577e-174, "610712026897514", -173 },
            { L_, 6.1093007110054400935497384e+079, "610930071100544",   80 },
            { L_, 6.1128549862884095447231433e-153, "611285498628841", -152 },
            { L_, 6.1145617572085896417169930e+192, "611456175720859",  193 },
            { L_, 6.1218210091280898728664447e-006, "612182100912809",   -5 },
            { L_, 6.1236151731299399359204125e-064, "612361517312994",  -63 },
            { L_, 6.1300257151042695051240216e+129, "613002571510427",  130 },
            { L_, 6.1309264080621696969959418e+108, "613092640806217",  109 },
            { L_, 6.1312176665549303854247790e-224, "613121766655493", -223 },
            { L_, 6.1328082618387797980417447e-158, "613280826183878", -157 },
            { L_, 6.1398517687238203507746982e-183, "613985176872382", -182 },
            { L_, 6.1405646015062495718327737e-087, "614056460150625",  -86 },
            { L_, 6.1409654362071396239988904e-049, "614096543620714",  -48 },
            { L_, 6.1413287178777398767486552e-219, "614132871787774", -218 },
            { L_, 6.1435076992945297456345114e-203, "614350769929453", -202 },
            { L_, 6.1441696519997900155512525e+094, "614416965199979",   95 },
            { L_, 6.1444722807310401645932177e-096, "614447228073104",  -95 },
            { L_, 6.1453995619290397692262086e-169, "614539956192904", -168 },
            { L_, 6.1469657759267504733759742e+105, "614696577592675",  106 },
            { L_, 6.1494191575604797243933718e+232, "614941915756048",  233 },
            { L_, 6.1508962151510697805410886e+111, "615089621515107",  112 },
            { L_, 6.1518041520582400284470701e-024, "615180415205824",  -23 },
            { L_, 6.1533466492926503805164031e-026, "615334664929265",  -25 },
            { L_, 6.1535704430495302314069464e+140, "615357044304953",  141 },
            { L_, 6.1543489301860803089120058e-287, "615434893018608", -286 },
            { L_, 6.1557352908214402444993032e-015, "615573529082144",  -14 },
            { L_, 6.1562918025203600591195163e-035, "615629180252036",  -34 },
            { L_, 6.1618412119266194864672233e+120, "616184121192662",  121 },
            { L_, 6.1658557985846100412546010e+178, "616585579858461",  179 },
            { L_, 6.1677825334050995308770693e-116,  "61677825334051", -115 },
            { L_, 6.1696133547597097593807490e-073, "616961335475971",  -72 },
            { L_, 6.1740283782248896000000000e+016, "617402837822489",   17 },
            { L_, 6.1750183452135496780367498e-020, "617501834521355",  -19 },
            { L_, 6.1758364419514999550567311e+085,  "61758364419515",   86 },
            { L_, 6.1764027544419101562500000e+012, "617640275444191",   13 },
            { L_, 6.1793222425915197099531337e+139, "617932224259152",  140 },
            { L_, 6.1807849716995103649548855e-231, "618078497169951", -230 },
            { L_, 6.1809967729457704344042668e-001, "618099677294577",    0 },
            { L_, 6.1816243621900599241559444e-249, "618162436219006", -248 },
            { L_, 6.1834063743180603999065069e-231, "618340637431806", -230 },
            { L_, 6.1853653935313203276366845e-053, "618536539353132",  -52 },
            { L_, 6.1858927641164899136990242e-125, "618589276411649", -124 },
            { L_, 6.1870887801540104747458367e-094, "618708878015401",  -93 },
            { L_, 6.1875293810768400951777339e+097, "618752938107684",   98 },
            { L_, 6.1884187663629598744718311e+294, "618841876636296",  295 },
            { L_, 6.1886037210188899646735136e+195, "618860372101889",  196 },
            { L_, 6.1889112032443602759230178e+216, "618891120324436",  217 },
            { L_, 6.1940903332607397509290039e+241, "619409033326074",  242 },
            { L_, 6.1949375659059200787378178e-156, "619493756590592", -155 },
            { L_, 6.1989099493090995762146236e-302,  "61989099493091", -301 },
            { L_, 6.1995782680135199314950793e+066, "619957826801352",   67 },
            { L_, 6.2005610738857399380297437e-065, "620056107388574",  -64 },
            { L_, 6.2030200526852502128025146e-005, "620302005268525",   -4 },
            { L_, 6.2047519037461004956402549e-256,  "62047519037461", -255 },
            { L_, 6.2055754043203802518201318e-063, "620557540432038",  -62 },
            { L_, 6.2056908082941005139058770e+110,  "62056908082941",  111 },
            { L_, 6.2062061314569101461378360e+120, "620620613145691",  121 },
            { L_, 6.2080347393405002889055569e-288,  "62080347393405", -287 },
            { L_, 6.2101478978352904830208129e+099, "621014789783529",  100 },
            { L_, 6.2107390351538905482903798e+259, "621073903515389",  260 },
            { L_, 6.2121587231444995247582960e-181,  "62121587231445", -180 },
            { L_, 6.2141521649567595183599414e+244, "621415216495676",  245 },
            { L_, 6.2209379727052302523084792e-255, "622093797270523", -254 },
            { L_, 6.2236054916495504465971734e+096, "622360549164955",   97 },
            { L_, 6.2245392617352696772628905e+091, "622453926173527",   92 },
            { L_, 6.2262970062272798314967368e-137, "622629700622728", -136 },
            { L_, 6.2271200820602996845760162e-166,  "62271200820603", -165 },
            { L_, 6.2271649765286301949764433e-208, "622716497652863", -207 },
            { L_, 6.2277794164830101780947219e+175, "622777941648301",  176 },
            { L_, 6.2285337665502699522760558e-151, "622853376655027", -150 },
            { L_, 6.2290782179889700467267049e-256, "622907821798897", -255 },
            { L_, 6.2294481352461202161953572e+295, "622944813524612",  296 },
            { L_, 6.2297048759835497847434993e-291, "622970487598355", -290 },
            { L_, 6.2363300613546501782735731e-001, "623633006135465",    0 },
            { L_, 6.2369937743487404148865340e-265, "623699377434874", -264 },
            { L_, 6.2395025648039298684758323e+264, "623950256480393",  265 },
            { L_, 6.2412762289508396832769864e+064, "624127622895084",   65 },
            { L_, 6.2414970373017996214719988e+065,  "62414970373018",   66 },
            { L_, 6.2418807032545604097561068e+027, "624188070325456",   28 },
            { L_, 6.2420140661255602613555146e-135, "624201406612556", -134 },
            { L_, 6.2435767557345994966536776e-029,  "62435767557346",  -28 },
            { L_, 6.2436177727383999444492550e+268,  "62436177727384",  269 },
            { L_, 6.2437730480404900989799156e-085, "624377304804049",  -84 },
            { L_, 6.2477562833118400728248871e-046, "624775628331184",  -45 },
            { L_, 6.2506161747628200009353838e+106, "625061617476282",  107 },
            { L_, 6.2526254856199400195678587e+260, "625262548561994",  261 },
            { L_, 6.2529168826781795809045695e+233, "625291688267818",  234 },
            { L_, 6.2532133402500703545945080e-155, "625321334025007", -154 },
            { L_, 6.2546238792816403586104729e+151, "625462387928164",  152 },
            { L_, 6.2564136709426702601749664e-232, "625641367094267", -231 },
            { L_, 6.2577995422731798177428220e+276, "625779954227318",  277 },
            { L_, 6.2592991143576702019017886e+050, "625929911435767",   51 },
            { L_, 6.2598731855519697441594282e+263, "625987318555197",  264 },
            { L_, 6.2613154254195705331792942e-144, "626131542541957", -143 },
            { L_, 6.2651085541815296793787164e+179, "626510855418153",  180 },
            { L_, 6.2686287869204601931209566e+116, "626862878692046",  117 },
            { L_, 6.2740504682615399705239830e+109, "627405046826154",  110 },
            { L_, 6.2743981634636702802989226e-259, "627439816346367", -258 },
            { L_, 6.2751086407339095482247826e+213, "627510864073391",  214 },
            { L_, 6.2798959823055298454070491e+085, "627989598230553",   86 },
            { L_, 6.2804967874969204815551600e-116, "628049678749692", -115 },
            { L_, 6.2808638556003296187436235e-144, "628086385560033", -143 },
            { L_, 6.2819927893738202219573191e+073, "628199278937382",   74 },
            { L_, 6.2836651360791501353848767e+069, "628366513607915",   70 },
            { L_, 6.2837978485050003392623444e+126,   "6283797848505",  127 },
            { L_, 6.2851635048880900865982434e+214, "628516350488809",  215 },
            { L_, 6.2860389360520302992405981e-257, "628603893605203", -256 },
            { L_, 6.2861629400771899778227202e-304, "628616294007719", -303 },
            { L_, 6.2876622636411295213464824e+213, "628766226364113",  214 },
            { L_, 6.2883530462260696811831296e+025, "628835304622607",   26 },
            { L_, 6.2890626948047804441185288e+045, "628906269480478",   46 },
            { L_, 6.2923714668348703735648499e+117, "629237146683487",  118 },
            { L_, 6.2953439151962803381679757e+097, "629534391519628",   98 },
            { L_, 6.3002941846686203343324342e-085, "630029418466862",  -84 },
            { L_, 6.3027285577946302346152147e-052, "630272855779463",  -51 },
            { L_, 6.3050632759524202789955217e-139, "630506327595242", -138 },
            { L_, 6.3062699874705498869483673e+227, "630626998747055",  228 },
            { L_, 6.3063974705602700000000000e+014, "630639747056027",   15 },
            { L_, 6.3064329918375500023798636e+156, "630643299183755",  157 },
            { L_, 6.3066461743866502018169702e-143, "630664617438665", -142 },
            { L_, 6.3105447015329596902530884e-150, "631054470153296", -149 },
            { L_, 6.3131960520683002321054299e-163,  "63131960520683", -162 },
            { L_, 6.3134882968937998128335562e+213,  "63134882968938",  214 },
            { L_, 6.3135974289406203716325044e-271, "631359742894062", -270 },
            { L_, 6.3146166951004900798338770e-004, "631461669510049",   -3 },
            { L_, 6.3157299430501200290832277e-118, "631572994305012", -117 },
            { L_, 6.3162208891271402126545645e-092, "631622088912714",  -91 },
            { L_, 6.3165403029406403086490439e+193, "631654030294064",  194 },
            { L_, 6.3167373000966896380448536e+131, "631673730009669",  132 },
            { L_, 6.3170910415176898236592646e-248, "631709104151769", -247 },
            { L_, 6.3187524582923798828920901e-256, "631875245829238", -255 },
            { L_, 6.3188988224740601669636130e-113, "631889882247406", -112 },
            { L_, 6.3209091323065195312675922e-287, "632090913230652", -286 },
            { L_, 6.3230221050166200452456295e-049, "632302210501662",  -48 },
            { L_, 6.3232744157189604186949644e-038, "632327441571896",  -37 },
            { L_, 6.3242841825604302368550428e-226, "632428418256043", -225 },
            { L_, 6.3318875657352800923966409e+201, "633188756573528",  202 },
            { L_, 6.3325331733893095925908084e+093, "633253317338931",   94 },
            { L_, 6.3354036107384402511855264e-180, "633540361073844", -179 },
            { L_, 6.3363616602353298750882368e+254, "633636166023533",  255 },
            { L_, 6.3391982328901003522596454e-105,  "63391982328901", -104 },
            { L_, 6.3402204215087003074560000e+022,  "63402204215087",   23 },
            { L_, 6.3407051168965704284767237e-044, "634070511689657",  -43 },
            { L_, 6.3408214293754805837702019e-045, "634082142937548",  -44 },
            { L_, 6.3408759496374996003133061e+126,  "63408759496375",  127 },
            { L_, 6.3426532029903801099962601e-004, "634265320299038",   -3 },
            { L_, 6.3433821675948999202706142e+074,  "63433821675949",   75 },
            { L_, 6.3448212173502099237353120e-204, "634482121735021", -203 },
            { L_, 6.3449915666991103199942398e+145, "634499156669911",  146 },
            { L_, 6.3475130130431800999229300e+186, "634751301304318",  187 },
            { L_, 6.3491826586874195595906159e-073, "634918265868742",  -72 },
            { L_, 6.3500018377421096363229184e+025, "635000183774211",   26 },
            { L_, 6.3502212668254999751354452e+035,  "63502212668255",   36 },
            { L_, 6.3525722205322200886264983e-059, "635257222053222",  -58 },
            { L_, 6.3554947915814894793944634e+185, "635549479158149",  186 },
            { L_, 6.3638179941631904057430802e+002, "636381799416319",    3 },
            { L_, 6.3643199144338401022365978e-199, "636431991443384", -198 },
            { L_, 6.3660456539871500600813177e-290, "636604565398715", -289 },
            { L_, 6.3667721812363704557534095e-262, "636677218123637", -261 },
            { L_, 6.3693341432239101675346542e-029, "636933414322391",  -28 },
            { L_, 6.3707217622166901344060719e-005, "637072176221669",   -4 },
            { L_, 6.3709237766916602631118666e+155, "637092377669166",  156 },
            { L_, 6.3718806126423403453539834e+037, "637188061264234",   38 },
            { L_, 6.3738622746233397564813811e-086, "637386227462334",  -85 },
            { L_, 6.3761127240569702193668831e-023, "637611272405697",  -22 },
            { L_, 6.3764467477921395059966157e+201, "637644674779214",  202 },
            { L_, 6.3770166473565198837184440e-130, "637701664735652", -129 },
            { L_, 6.3775565974039893665313073e+045, "637755659740399",   46 },
            { L_, 6.3778342325742504777752180e+104, "637783423257425",  105 },
            { L_, 6.3805978828712800142399182e+233, "638059788287128",  234 },
            { L_, 6.3817270999962497038102275e+119, "638172709999625",  120 },
            { L_, 6.3821578116325895051808908e+057, "638215781163259",   58 },
            { L_, 6.3839376341268204245031637e+261, "638393763412682",  262 },
            { L_, 6.3840810121830094383386126e+256, "638408101218301",  257 },
            { L_, 6.3848771459651903919234927e-277, "638487714596519", -276 },
            { L_, 6.3855592346219203921160503e+110, "638555923462192",  111 },
            { L_, 6.3904622480919599696057210e-299, "639046224809196", -298 },
            { L_, 6.3909599041466698329082701e-039, "639095990414667",  -38 },
            { L_, 6.3911927975135696026526508e-065, "639119279751357",  -64 },
            { L_, 6.3932359089533199032418001e-288, "639323590895332", -287 },
            { L_, 6.3947582837801300243202841e-257, "639475828378013", -256 },
            { L_, 6.3950739937008903634380742e-202, "639507399370089", -201 },
            { L_, 6.3974810337750397518136181e-117, "639748103377504", -116 },
            { L_, 6.4001393292284205926989671e+001, "640013932922842",    2 },
            { L_, 6.4014783707866505422156090e-278, "640147837078665", -277 },
            { L_, 6.4015838007195003612338325e+053,  "64015838007195",   54 },
            { L_, 6.4018246946707903436949410e+157, "640182469467079",  158 },
            { L_, 6.4027326644410302629699125e-230, "640273266444103", -229 },
            { L_, 6.4029772462074403345982748e-073, "640297724620744",  -72 },
            { L_, 6.4041481259744300126699520e+024, "640414812597443",   25 },
            { L_, 6.4043450956876299731475822e+044, "640434509568763",   45 },
            { L_, 6.4059339990742598656194708e-248, "640593399907426", -247 },
            { L_, 6.4074741340093602511920811e-123, "640747413400936", -122 },
            { L_, 6.4083435975526097452561094e+206, "640834359755261",  207 },
            { L_, 6.4084712664165902724599327e-070, "640847126641659",  -69 },
            { L_, 6.4110598086816597628902055e+277, "641105980868166",  278 },
            { L_, 6.4112638736723700886879778e-048, "641126387367237",  -47 },
            { L_, 6.4120050545586398640706671e-082, "641200505455864",  -81 },
            { L_, 6.4120685495932098929161329e-186, "641206854959321", -185 },
            { L_, 6.4126738106377797073427214e+067, "641267381063778",   68 },
            { L_, 6.4143714533623600947712824e+270, "641437145336236",  271 },
            { L_, 6.4191604165356595602900553e+129, "641916041653566",  130 },
            { L_, 6.4250248085129102559776639e-054, "642502480851291",  -53 },
            { L_, 6.4251069575059101158292613e+229, "642510695750591",  230 },
            { L_, 6.4256194260526002461802564e+123,  "64256194260526",  124 },
            { L_, 6.4262857099055700977569763e+120, "642628570990557",  121 },
            { L_, 6.4302002594010399476602214e-050, "643020025940104",  -49 },
            { L_, 6.4364239677584197507299436e+179, "643642396775842",  180 },
            { L_, 6.4379665370453902133445889e-056, "643796653704539",  -55 },
            { L_, 6.4389620966508199266342898e-201, "643896209665082", -200 },
            { L_, 6.4405907702337697533738868e+230, "644059077023377",  231 },
            { L_, 6.4427349274117402969089943e+154, "644273492741174",  155 },
            { L_, 6.4452283260413000645363288e-164,  "64452283260413", -163 },
            { L_, 6.4464655814453896487412222e-080, "644646558144539",  -79 },
            { L_, 6.4479695578964802188417353e-218, "644796955789648", -217 },
            { L_, 6.4507917866750396964483027e-271, "645079178667504", -270 },
            { L_, 6.4508543434820199678876651e+157, "645085434348202",  158 },
            { L_, 6.4519813072730204293837748e+211, "645198130727302",  212 },
            { L_, 6.4528449773250900666217204e+168, "645284497732509",  169 },
            { L_, 6.4544286708014202416250242e-069, "645442867080142",  -68 },
            { L_, 6.4563474657615498225282007e-226, "645634746576155", -225 },
            { L_, 6.4566415097833496162575226e-171, "645664150978335", -170 },
            { L_, 6.4569908206478396856976365e-110, "645699082064784", -109 },
            { L_, 6.4574160499895796592614386e+053, "645741604998958",   54 },
            { L_, 6.4577404714466600960000000e+018, "645774047144666",   19 },
            { L_, 6.4586712056524096576829167e+292, "645867120565241",  293 },
            { L_, 6.4589616661860200282265891e+268, "645896166618602",  269 },
            { L_, 6.4594529009693700631189664e+086, "645945290096937",   87 },
            { L_, 6.4602158128703099391467594e-240, "646021581287031", -239 },
            { L_, 6.4615406352541003887500621e-093,  "64615406352541",  -92 },
            { L_, 6.4619463182871596407520854e-124, "646194631828716", -123 },
            { L_, 6.4621641542277300435845185e-126, "646216415422773", -125 },
            { L_, 6.4627928393532497022301168e-141, "646279283935325", -140 },
            { L_, 6.4634665146862798831820769e+041, "646346651468628",   42 },
            { L_, 6.4647550435527601258264288e+193, "646475504355276",  194 },
            { L_, 6.4716543430071402181933828e-209, "647165434300714", -208 },
            { L_, 6.4731483203155401940335276e+047, "647314832031554",   48 },
            { L_, 6.4739533111105396648233541e-233, "647395331111054", -232 },
            { L_, 6.4744725109471399802834073e-261, "647447251094714", -260 },
            { L_, 6.4756561826956404038189743e-185, "647565618269564", -184 },
            { L_, 6.4771260554755801646916806e-054, "647712605547558",  -53 },
            { L_, 6.4812931423257800398968097e-174, "648129314232578", -173 },
            { L_, 6.4814759251149995584817454e-219,   "6481475925115", -218 },
            { L_, 6.4825750000819704434564597e-087, "648257500008197",  -86 },
            { L_, 6.4840412457825098407006788e-018, "648404124578251",  -17 },
            { L_, 6.4859283625329703794072622e+071, "648592836253297",   72 },
            { L_, 6.4859881122141703189137043e+084, "648598811221417",   85 },
            { L_, 6.4862184542286895631947611e+116, "648621845422869",  117 },
            { L_, 6.4879726027109503371695016e+273, "648797260271095",  274 },
            { L_, 6.4889543638950401635659644e-215, "648895436389504", -214 },
            { L_, 6.4903553576695304370761166e+182, "649035535766953",  183 },
            { L_, 6.4917218215756505685466155e+287, "649172182157565",  288 },
            { L_, 6.4937916820623796204266369e-109, "649379168206238", -108 },
            { L_, 6.4963875411640796427515310e-055, "649638754116408",  -54 },
            { L_, 6.4967892182519102087401941e-018, "649678921825191",  -17 },
            { L_, 6.4975623308520802536958007e-204, "649756233085208", -203 },
            { L_, 6.4986714809914499670848004e+065, "649867148099145",   66 },
            { L_, 6.5009272179165901421195625e+272, "650092721791659",  273 },
            { L_, 6.5012002398341296836694664e+243, "650120023983413",  244 },
            { L_, 6.5041632219060902735204097e-270, "650416322190609", -269 },
            { L_, 6.5053506277184098643381027e+208, "650535062771841",  209 },
            { L_, 6.5071378416843699276553508e+093, "650713784168437",   94 },
            { L_, 6.5084806247701197112372892e-025, "650848062477012",  -24 },
            { L_, 6.5091909600224194696380117e+073, "650919096002242",   74 },
            { L_, 6.5092268846271103234045098e-209, "650922688462711", -208 },
            { L_, 6.5112395097026505427591649e-135, "651123950970265", -134 },
            { L_, 6.5114365354900204092380380e-122, "651143653549002", -121 },
            { L_, 6.5162162387476096032330414e+126, "651621623874761",  127 },
            { L_, 6.5200042348414897715798326e-301, "652000423484149", -300 },
            { L_, 6.5223217075799401618346459e-021, "652232170757994",  -20 },
            { L_, 6.5252859952217000763812706e+222,  "65252859952217",  223 },
            { L_, 6.5261264242293400192190584e-184, "652612642422934", -183 },
            { L_, 6.5279330143551605271527005e-293, "652793301435516", -292 },
            { L_, 6.5284657234940098176905942e+201, "652846572349401",  202 },
            { L_, 6.5332977544762294011136039e-263, "653329775447623", -262 },
            { L_, 6.5334325439550799835621413e-071, "653343254395508",  -70 },
            { L_, 6.5341566372665601082206502e-234, "653415663726656", -233 },
            { L_, 6.5344400927791197518171185e+267, "653444009277912",  268 },
            { L_, 6.5355942704153903777336358e+187, "653559427041539",  188 },
            { L_, 6.5373238844640404599468026e+173, "653732388446404",  174 },
            { L_, 6.5385401243482298227546365e+159, "653854012434823",  160 },
            { L_, 6.5403531247002894195119825e+039, "654035312470029",   40 },
            { L_, 6.5414998202606002809354468e-186,  "65414998202606", -185 },
            { L_, 6.5435691523376597619559257e-008, "654356915233766",   -7 },
            { L_, 6.5458429411071004894280464e-176,  "65458429411071", -175 },
            { L_, 6.5477287257485403621078436e+159, "654772872574854",  160 },
            { L_, 6.5486319520107199435749926e+221, "654863195201072",  222 },
            { L_, 6.5531293351093203737011921e-103, "655312933510932", -102 },
            { L_, 6.5539057023605402952349264e+031, "655390570236054",   32 },
            { L_, 6.5547403136806100783645718e-045, "655474031368061",  -44 },
            { L_, 6.5569499521232197549424872e-050, "655694995212322",  -49 },
            { L_, 6.5583740889073501688198008e-190, "655837408890735", -189 },
            { L_, 6.5588804661712996271031639e+077,  "65588804661713",   78 },
            { L_, 6.5597571070438201016138662e+243, "655975710704382",  244 },
            { L_, 6.5598487401479396888243588e-131, "655984874014794", -130 },
            { L_, 6.5608846065415402259742720e+025, "656088460654154",   26 },
            { L_, 6.5655847649773805254736935e+197, "656558476497738",  198 },
            { L_, 6.5667939529487902694540049e+298, "656679395294879",  299 },
            { L_, 6.5686933828769803880658865e+075, "656869338287698",   76 },
            { L_, 6.5688054893838406037537891e+039, "656880548938384",   40 },
            { L_, 6.5701273121733198965801415e+190, "657012731217332",  191 },
            { L_, 6.5711420932131397973735160e+032, "657114209321314",   33 },
            { L_, 6.5741213806978900390993908e+121, "657412138069789",  122 },
            { L_, 6.5752487854394003703364106e-308,  "65752487854394", -307 },
            { L_, 6.5754198192096297212868187e-179, "657541981920963", -178 },
            { L_, 6.5768292946729901568117267e-034, "657682929467299",  -33 },
            { L_, 6.5769584954769103685527886e-029, "657695849547691",  -28 },
            { L_, 6.5781055288668000139296055e+005,  "65781055288668",    6 },
            { L_, 6.5798983098053903264019489e+250, "657989830980539",  251 },
            { L_, 6.5812356538509998787498645e-042,   "6581235653851",  -41 },
            { L_, 6.5826801162005500537675637e-225, "658268011620055", -224 },
            { L_, 6.5830683706422896356741511e+078, "658306837064229",   79 },
            { L_, 6.5833226917107100167265291e-218, "658332269171071", -217 },
            { L_, 6.5856466610862599232278344e+297, "658564666108626",  298 },
            { L_, 6.5873586949503706295569006e+259, "658735869495037",  260 },
            { L_, 6.5933746645804397831771789e-038, "659337466458044",  -37 },
            { L_, 6.5935456384703997325037134e+246,  "65935456384704",  247 },
            { L_, 6.5946472337464895703078176e+118, "659464723374649",  119 },
            { L_, 6.5947108832796403784630378e-210, "659471088327964", -209 },
            { L_, 6.5948150949752303652349486e-027, "659481509497523",  -26 },
            { L_, 6.5958528461109803616519981e+037, "659585284611098",   38 },
            { L_, 6.5995004944130201831231070e+095, "659950049441302",   96 },
            { L_, 6.6010313778759501393471665e+031, "660103137787595",   32 },
            { L_, 6.6071549607888604288552514e-003, "660715496078886",   -2 },
            { L_, 6.6074468478040395557703095e+101, "660744684780404",  102 },
            { L_, 6.6075395702421897443945540e-183, "660753957024219", -182 },
            { L_, 6.6079512097461299556445390e+220, "660795120974613",  221 },
            { L_, 6.6083760649854696385957124e-051, "660837606498547",  -50 },
            { L_, 6.6089710815203999632107277e+217,  "66089710815204",  218 },
            { L_, 6.6110479277187405035599576e-253, "661104792771874", -252 },
            { L_, 6.6110524712913905132273398e-201, "661105247129139", -200 },
            { L_, 6.6115460613155698196372612e-275, "661154606131557", -274 },
            { L_, 6.6127840478826399817080528e+001, "661278404788264",    2 },
            { L_, 6.6159177822161303455869636e+235, "661591778221613",  236 },
            { L_, 6.6164503999888000228862958e+121,  "66164503999888",  122 },
            { L_, 6.6204022774536698252233510e+212, "662040227745367",  213 },
            { L_, 6.6208772834105400579090321e-040, "662087728341054",  -39 },
            { L_, 6.6213683446501904860289396e+105, "662136834465019",  106 },
            { L_, 6.6215663719902901961272536e+224, "662156637199029",  225 },
            { L_, 6.6224502820051099194336037e+131, "662245028200511",  132 },
            { L_, 6.6242350597152496401070935e+220, "662423505971525",  221 },
            { L_, 6.6248131350114096429264271e-245, "662481313501141", -244 },
            { L_, 6.6251169870736994153449496e-241,  "66251169870737", -240 },
            { L_, 6.6255204938526896495062424e+144, "662552049385269",  145 },
            { L_, 6.6259579556677500141386229e+265, "662595795566775",  266 },
            { L_, 6.6279743849928596324388258e-152, "662797438499286", -151 },
            { L_, 6.6300770615118805639811445e+306, "663007706151188",  307 },
            { L_, 6.6305749586229097569727249e+254, "663057495862291",  255 },
            { L_, 6.6328981522916002477793456e-102,  "66328981522916", -101 },
            { L_, 6.6355777443599802545583198e+247, "663557774435998",  248 },
            { L_, 6.6367132595491998285150213e+196,  "66367132595492",  197 },
            { L_, 6.6382893014675401222335692e+232, "663828930146754",  233 },
            { L_, 6.6390788589897898581396860e-192, "663907885898979", -191 },
            { L_, 6.6393959131340904741787326e+279, "663939591313409",  280 },
            { L_, 6.6401365042579500004649162e+006, "664013650425795",    7 },
            { L_, 6.6408673298221495538589671e+117, "664086732982215",  118 },
            { L_, 6.6411314769693101510896593e+160, "664113147696931",  161 },
            { L_, 6.6430429337039303915542663e-121, "664304293370393", -120 },
            { L_, 6.6443717557239195883941599e+180, "664437175572392",  181 },
            { L_, 6.6454296587047300162741927e+193, "664542965870473",  194 },
            { L_, 6.6468917226187896886744911e+192, "664689172261879",  193 },
            { L_, 6.6499934595929803331331556e-067, "664999345959298",  -66 },
            { L_, 6.6511129229468897682874626e+130, "665111292294689",  131 },
            { L_, 6.6545146816508795871850663e-230, "665451468165088", -229 },
            { L_, 6.6556760741012298941448902e+274, "665567607410123",  275 },
            { L_, 6.6566736713805496687128085e-135, "665667367138055", -134 },
            { L_, 6.6569982846465703157197844e+207, "665699828464657",  208 },
            { L_, 6.6593801303739601663504593e-167, "665938013037396", -166 },
            { L_, 6.6599189385287499137322469e-261, "665991893852875", -260 },
            { L_, 6.6602174309628600313094773e+109, "666021743096286",  110 },
            { L_, 6.6618856278924500737986751e-240, "666188562789245", -239 },
            { L_, 6.6659185056521199843938795e-221, "666591850565212", -220 },
            { L_, 6.6697736982305697855386255e-197, "666977369823057", -196 },
            { L_, 6.6720475327372000142789419e-254,  "66720475327372", -253 },
            { L_, 6.6724877128816596476654719e+229, "667248771288166",  230 },
            { L_, 6.6731922237696000889813760e+285,  "66731922237696",  286 },
            { L_, 6.6753448495631095838120928e-001, "667534484956311",    0 },
            { L_, 6.6782309562572705763203676e-200, "667823095625727", -199 },
            { L_, 6.6798255450418493814127056e+051, "667982554504185",   52 },
            { L_, 6.6803755529764601140161390e-263, "668037555297646", -262 },
            { L_, 6.6830206894686102524619924e-192, "668302068946861", -191 },
            { L_, 6.6874271967875600648392164e+143, "668742719678756",  144 },
            { L_, 6.6891593541631494807910401e+073, "668915935416315",   74 },
            { L_, 6.6891780805780201268788224e+081, "668917808057802",   82 },
            { L_, 6.6894708532644699240459332e+108, "668947085326447",  109 },
            { L_, 6.6916533269350396516448299e-261, "669165332693504", -260 },
            { L_, 6.6926284757689896882937308e-005, "669262847576899",   -4 },
            { L_, 6.6932380772002304000000000e+017, "669323807720023",   18 },
            { L_, 6.6996408817041105024090042e-296, "669964088170411", -295 },
            { L_, 6.7018218201122200378444713e+283, "670182182011222",  284 },
            { L_, 6.7029589063317799300301941e-293, "670295890633178", -292 },
            { L_, 6.7029648583949199265309910e-085, "670296485839492",  -84 },
            { L_, 6.7044850823762199295446800e-234, "670448508237622", -233 },
            { L_, 6.7047247672970401735555674e+238, "670472476729704",  239 },
            { L_, 6.7109019104522805367432806e+026, "671090191045228",   27 },
            { L_, 6.7113845912850900465550415e+200, "671138459128509",  201 },
            { L_, 6.7116490588529901930789808e-256, "671164905885299", -255 },
            { L_, 6.7153367072664396581249087e+243, "671533670726644",  244 },
            { L_, 6.7169155459780600009009882e+306, "671691554597806",  307 },
            { L_, 6.7196678628056001524260593e+149,  "67196678628056",  150 },
            { L_, 6.7216430974803999809400140e-055,  "67216430974804",  -54 },
            { L_, 6.7223290381262298362088541e-304, "672232903812623", -303 },
            { L_, 6.7267966796303695186941700e+071, "672679667963037",   72 },
            { L_, 6.7273307921152995320685755e+254,  "67273307921153",  255 },
            { L_, 6.7281775243221303864969968e-126, "672817752432213", -125 },
            { L_, 6.7295276324007798151147285e+117, "672952763240078",  118 },
            { L_, 6.7299182082534697302040919e+111, "672991820825347",  112 },
            { L_, 6.7301830621327802635907549e+252, "673018306213278",  253 },
            { L_, 6.7304796276141303223994328e+001, "673047962761413",    2 },
            { L_, 6.7330483708171799963611056e-179, "673304837081718", -178 },
            { L_, 6.7378043399620203468292466e-158, "673780433996202", -157 },
            { L_, 6.7391222035777206710932198e+085, "673912220357772",   86 },
            { L_, 6.7392230669887303931313906e+213, "673922306698873",  214 },
            { L_, 6.7393939487738201561706151e+207, "673939394877382",  208 },
            { L_, 6.7400908214505498602518230e+211, "674009082145055",  212 },
            { L_, 6.7401719545536304501390917e-116, "674017195455363", -115 },
            { L_, 6.7403394483569496072415793e+057, "674033944835695",   58 },
            { L_, 6.7426526912364696291077074e+255, "674265269123647",  256 },
            { L_, 6.7432610379082797829016719e-046, "674326103790828",  -45 },
            { L_, 6.7466748597600296223281004e+075, "674667485976003",   76 },
            { L_, 6.7468170486716200881035954e+247, "674681704867162",  248 },
            { L_, 6.7471128143178998629514394e-212,  "67471128143179", -211 },
            { L_, 6.7496011340367199833463034e+188, "674960113403672",  189 },
            { L_, 6.7498131344623596267736739e+115, "674981313446236",  116 },
            { L_, 6.7499137983480504706204001e+091, "674991379834805",   92 },
            { L_, 6.7514010820717804305497892e-172, "675140108207178", -171 },
            { L_, 6.7547316576752599914364952e+040, "675473165767526",   41 },
            { L_, 6.7556057106682402139040268e-241, "675560571066824", -240 },
            { L_, 6.7562674736296002719012973e+228,  "67562674736296",  229 },
            { L_, 6.7573723514928497889457268e+185, "675737235149285",  186 },
            { L_, 6.7575211289392597637047910e+039, "675752112893926",   40 },
            { L_, 6.7644983070308400150902449e-156, "676449830703084", -155 },
            { L_, 6.7650495921504296307261440e+024, "676504959215043",   25 },
            { L_, 6.7653794185181600798378781e+231, "676537941851816",  232 },
            { L_, 6.7657590657710902548170961e+063, "676575906577109",   64 },
            { L_, 6.7683548351082698409107022e-126, "676835483510827", -125 },
            { L_, 6.7725954382497203350067139e+008, "677259543824972",    9 },
            { L_, 6.7736265989672795672029864e-115, "677362659896728", -114 },
            { L_, 6.7748050929366701193735450e+295, "677480509293667",  296 },
            { L_, 6.7763744234630099292358785e+246, "677637442346301",  247 },
            { L_, 6.7800907486445997046028177e+138,  "67800907486446",  139 },
            { L_, 6.7812811036000599349743060e-060, "678128110360006",  -59 },
            { L_, 6.7818972434646100592770102e-073, "678189724346461",  -72 },
            { L_, 6.7850637076830804389852213e-305, "678506370768308", -304 },
            { L_, 6.7883447490559800901452242e+168, "678834474905598",  169 },
            { L_, 6.7922272150277601047832858e-011, "679222721502776",  -10 },
            { L_, 6.7932555822653698792570102e-062, "679325558226537",  -61 },
            { L_, 6.7937328656773406787337068e-226, "679373286567734", -225 },
            { L_, 6.7941826406949997086713317e-084,   "6794182640695",  -83 },
            { L_, 6.7963044355312096757494509e-175, "679630443553121", -174 },
            { L_, 6.7989611266792801035116592e-141, "679896112667928", -140 },
            { L_, 6.7997503428042200823711028e+000, "679975034280422",    1 },
            { L_, 6.7998322553735904375142387e-126, "679983225537359", -125 },
            { L_, 6.8003327185388095379753021e-294, "680033271853881", -293 },
            { L_, 6.8005696202038994599325393e+151,  "68005696202039",  152 },
            { L_, 6.8014876878821999950680499e-031,  "68014876878822",  -30 },
            { L_, 6.8020210786868799507116360e-027, "680202107868688",  -26 },
            { L_, 6.8038107019067096152460161e-113, "680381070190671", -112 },
            { L_, 6.8055447996671601955165472e+050, "680554479966716",   51 },
            { L_, 6.8059888806149197882733467e-263, "680598888061492", -262 },
            { L_, 6.8066790056485196497614400e-150, "680667900564852", -149 },
            { L_, 6.8101860138047602195169280e+024, "681018601380476",   25 },
            { L_, 6.8108869276810599828082688e+088, "681088692768106",   89 },
            { L_, 6.8110241277470303615841096e+267, "681102412774703",  268 },
            { L_, 6.8115209674696600084706336e+144, "681152096746966",  145 },
            { L_, 6.8134867333890599812803683e-302, "681348673338906", -301 },
            { L_, 6.8174628553579902699509832e-154, "681746285535799", -153 },
            { L_, 6.8182142707550199647471903e+170, "681821427075502",  171 },
            { L_, 6.8189032280459203327396633e+098, "681890322804592",   99 },
            { L_, 6.8197791333637904049880470e+199, "681977913336379",  200 },
            { L_, 6.8201595666409900863208159e-054, "682015956664099",  -53 },
            { L_, 6.8214784748480402855656663e+052, "682147847484804",   53 },
            { L_, 6.8240134825453096310286811e-168, "682401348254531", -167 },
            { L_, 6.8243777876346000556325783e-030,  "68243777876346",  -29 },
            { L_, 6.8258973748634602430213438e+063, "682589737486346",   64 },
            { L_, 6.8273465655191393325836681e+200, "682734656551914",  201 },
            { L_, 6.8273943914306197386634712e+243, "682739439143062",  244 },
            { L_, 6.8287399857114495198141032e+172, "682873998571145",  173 },
            { L_, 6.8289955719578604409048848e-254, "682899557195786", -253 },
            { L_, 6.8300097109723403984360610e-023, "683000971097234",  -22 },
            { L_, 6.8309521795417801300692593e-269, "683095217954178", -268 },
            { L_, 6.8341374256093797752631099e-267, "683413742560938", -266 },
            { L_, 6.8346923269353005680102804e-284,  "68346923269353", -283 },
            { L_, 6.8373637079989601544310730e-212, "683736370799896", -211 },
            { L_, 6.8377492927336700289068829e+259, "683774929273367",  260 },
            { L_, 6.8380153297139398437500000e+013, "683801532971394",   14 },
            { L_, 6.8390440258845398128981458e+286, "683904402588454",  287 },
            { L_, 6.8400614757666501374694108e+158, "684006147576665",  159 },
            { L_, 6.8409369960509196754614831e-105, "684093699605092", -104 },
            { L_, 6.8422302892959303515504683e+234, "684223028929593",  235 },
            { L_, 6.8425391271962901577838850e-155, "684253912719629", -154 },
            { L_, 6.8464443767627000692728561e+157,  "68464443767627",  158 },
            { L_, 6.8469679486015999261038781e+304,  "68469679486016",  305 },
            { L_, 6.8472226370678097655040065e+206, "684722263706781",  207 },
            { L_, 6.8486872550982396754055831e-031, "684868725509824",  -30 },
            { L_, 6.8497771505564301615836564e-242, "684977715055643", -241 },
            { L_, 6.8543155212694100628825055e-154, "685431552126941", -153 },
            { L_, 6.8543452977899696075372924e+254, "685434529778997",  255 },
            { L_, 6.8543794638222898412384181e+129, "685437946382229",  130 },
            { L_, 6.8551784206422797089189067e+172, "685517842064228",  173 },
            { L_, 6.8562678898051403665189881e-085, "685626788980514",  -84 },
            { L_, 6.8565524211971499937963768e+120, "685655242119715",  121 },
            { L_, 6.8577222083362195897439922e-148, "685772220833622", -147 },
            { L_, 6.8583485702373099497802740e-051, "685834857023731",  -50 },
            { L_, 6.8611115473561103850042016e+088, "686111154735611",   89 },
            { L_, 6.8650675618572999109998597e+078,  "68650675618573",   79 },
            { L_, 6.8663328648470501426322068e-231, "686633286484705", -230 },
            { L_, 6.8679122382882707014363781e-257, "686791223828827", -256 },
            { L_, 6.8696520302777900810408710e-138, "686965203027779", -137 },
            { L_, 6.8705618666647304763430409e+282, "687056186666473",  283 },
            { L_, 6.8713903190799599100803488e+220, "687139031907996",  221 },
            { L_, 6.8736777268150103346418758e+029, "687367772681501",   30 },
            { L_, 6.8741587947738197782052509e-230, "687415879477382", -229 },
            { L_, 6.8751550039524497316933138e-011, "687515500395245",  -10 },
            { L_, 6.8768714579010998008628417e+098,  "68768714579011",   99 },
            { L_, 6.8781082605837506035300971e-306, "687810826058375", -305 },
            { L_, 6.8797069423161202576011419e+005, "687970694231612",    6 },
            { L_, 6.8811568613713598813813543e+145, "688115686137136",  146 },
            { L_, 6.8824709364964906629964264e-058, "688247093649649",  -57 },
            { L_, 6.8863507120630099629896929e+062, "688635071206301",   63 },
            { L_, 6.8883047303577201610129284e-148, "688830473035772", -147 },
            { L_, 6.8885498328076494570082327e-282, "688854983280765", -281 },
            { L_, 6.8888457615035895852817564e+030, "688884576150359",   31 },
            { L_, 6.8890124587708300462268579e+149, "688901245877083",  150 },
            { L_, 6.8894417628624905044113619e+135, "688944176286249",  136 },
            { L_, 6.8907341307344999055403056e+247,  "68907341307345",  248 },
            { L_, 6.8909697029524299686940914e+208, "689096970295243",  209 },
            { L_, 6.8943978060678103595945655e+300, "689439780606781",  301 },
            { L_, 6.8966649446113192644733427e+038, "689666494461132",   39 },
            { L_, 6.8975485824359403776638293e+142, "689754858243594",  143 },
            { L_, 6.8985929505535402673304957e+172, "689859295055354",  173 },
            { L_, 6.9014745973753405047867663e-122, "690147459737534", -121 },
            { L_, 6.9018360577592296733653531e-055, "690183605775923",  -54 },
            { L_, 6.9020446437709998284294325e+242,   "6902044643771",  243 },
            { L_, 6.9028879105247900868287260e+037, "690288791052479",   38 },
            { L_, 6.9038210975973594300541165e+265, "690382109759736",  266 },
            { L_, 6.9039126679057002779285420e+067,  "69039126679057",   68 },
            { L_, 6.9091343754333201953992754e-174, "690913437543332", -173 },
            { L_, 6.9133434181612305627420678e-108, "691334341816123", -107 },
            { L_, 6.9146990710253905359665467e+238, "691469907102539",  239 },
            { L_, 6.9156293487797996374943313e-024,  "69156293487798",  -23 },
            { L_, 6.9175300570995800010866310e+160, "691753005709958",  161 },
            { L_, 6.9188801010833394741369334e+101, "691888010108334",  102 },
            { L_, 6.9196114381734998193407987e-276,  "69196114381735", -275 },
            { L_, 6.9205604600394097296908078e-177, "692056046003941", -176 },
            { L_, 6.9248478868645902543860818e-087, "692484788686459",  -86 },
            { L_, 6.9261112969327097809281527e-138, "692611129693271", -137 },
            { L_, 6.9262620271941699979701853e+143, "692626202719417",  144 },
            { L_, 6.9267042621561203677521703e-270, "692670426215612", -269 },
            { L_, 6.9307932275995301118451705e-176, "693079322759953", -175 },
            { L_, 6.9336640856241004569381734e-271,  "69336640856241", -270 },
            { L_, 6.9363436973887294644332177e+287, "693634369738873",  288 },
            { L_, 6.9376013600785100330992852e+236, "693760136007851",  237 },
            { L_, 6.9386715211744300667196654e-284, "693867152117443", -283 },
            { L_, 6.9389784062428996988333044e+060,  "69389784062429",   61 },
            { L_, 6.9392528032319596902063567e+052, "693925280323196",   53 },
            { L_, 6.9402265935263797491474478e-084, "694022659352638",  -83 },
            { L_, 6.9412241255531800375404386e+285, "694122412555318",  286 },
            { L_, 6.9456873269290602433266174e+301, "694568732692906",  302 },
            { L_, 6.9485177080182101295905820e+201, "694851770801821",  202 },
            { L_, 6.9488231602966494566343325e+055, "694882316029665",   56 },
            { L_, 6.9494565641922397339519741e+181, "694945656419224",  182 },
            { L_, 6.9531760813834895822564654e-277, "695317608138349", -276 },
            { L_, 6.9541910086238099785505546e-205, "695419100862381", -204 },
            { L_, 6.9565010019884100047524165e+103, "695650100198841",  104 },
            { L_, 6.9574298555334700527156115e-140, "695742985553347", -139 },
            { L_, 6.9591283488462097707171840e+025, "695912834884621",   26 },
            { L_, 6.9602700145349396401257823e+211, "696027001453494",  212 },
            { L_, 6.9625726602103304248060570e+142, "696257266021033",  143 },
            { L_, 6.9628652053882302130495883e+052, "696286520538823",   53 },
            { L_, 6.9637728148098197246680940e+272, "696377281480982",  273 },
            { L_, 6.9666532826125501882666245e-184, "696665328261255", -183 },
            { L_, 6.9708937510109403106105615e-014, "697089375101094",  -13 },
            { L_, 6.9721376516639201916890873e+178, "697213765166392",  179 },
            { L_, 6.9746983040488096636596205e+126, "697469830404881",  127 },
            { L_, 6.9755535000863693278068061e+265, "697555350008637",  266 },
            { L_, 6.9767023713887298750434184e-297, "697670237138873", -296 },
            { L_, 6.9768573352337596782651320e-280, "697685733523376", -279 },
            { L_, 6.9801196556655095258508368e+245, "698011965566551",  246 },
            { L_, 6.9809073099001298270979200e-185, "698090730990013", -184 },
            { L_, 6.9821890413805899791398117e+237, "698218904138059",  238 },
            { L_, 6.9826114375201999294058263e+227,  "69826114375202",  228 },
            { L_, 6.9827123848681598161647344e+000, "698271238486816",    1 },
            { L_, 6.9840861087773301415803632e+233, "698408610877733",  234 },
            { L_, 6.9841938748551599369426172e+230, "698419387485516",  231 },
            { L_, 6.9863348271863401348386638e+068, "698633482718634",   69 },
            { L_, 6.9867218147074302529672320e+096, "698672181470743",   97 },
            { L_, 6.9872456205957402535994215e-054, "698724562059574",  -53 },
            { L_, 6.9881865967116603729742736e+039, "698818659671166",   40 },
            { L_, 6.9884633051349401466991096e-192, "698846330513494", -191 },
            { L_, 6.9886723579672600972999848e+036, "698867235796726",   37 },
            { L_, 6.9896817828368604401824127e+065, "698968178283686",   66 },
            { L_, 6.9902206620775595444537788e+164, "699022066207756",  165 },
            { L_, 6.9915796533959498254808203e-168, "699157965339595", -167 },
            { L_, 6.9931012333180198605654090e-043, "699310123331802",  -42 },
            { L_, 6.9931837077285202599136048e-137, "699318370772852", -136 },
            { L_, 6.9935709292283602220270861e+173, "699357092922836",  174 },
            { L_, 6.9943041243305705095720494e-274, "699430412433057", -273 },
            { L_, 6.9945866356212198290510897e-207, "699458663562122", -206 },
            { L_, 6.9954913155237500373521661e-283, "699549131552375", -282 },
            { L_, 6.9973802300336798653006554e+006, "699738023003368",    7 },
            { L_, 6.9978757701403403833439469e-176, "699787577014034", -175 },
            { L_, 6.9981734071505505729185304e-225, "699817340715055", -224 },
            { L_, 7.0000264943796801761982608e-123, "700002649437968", -122 },
            { L_, 7.0001760729598397445541954e+191, "700017607295984",  192 },
            { L_, 7.0009083587494503298950591e-262, "700090835874945", -261 },
            { L_, 7.0023357791827400665547622e-137, "700233577918274", -136 },
            { L_, 7.0025894612554000879097704e+168,  "70025894612554",  169 },
            { L_, 7.0037484941771803914466599e-044, "700374849417718",  -43 },
            { L_, 7.0038101396807604626361085e-053, "700381013968076",  -52 },
            { L_, 7.0046007467575102665613165e-148, "700460074675751", -147 },
            { L_, 7.0053153009106602754836213e-126, "700531530091066", -125 },
            { L_, 7.0063224784017095565795898e+009, "700632247840171",   10 },
            { L_, 7.0077658384341297513299142e-062, "700776583843413",  -61 },
            { L_, 7.0099747343330702278240250e+200, "700997473433307",  201 },
            { L_, 7.0134907061882802902908820e+271, "701349070618828",  272 },
            { L_, 7.0149676658922697136609365e+096, "701496766589227",   97 },
            { L_, 7.0157815328058497755671570e-142, "701578153280585", -141 },
            { L_, 7.0166642042646198448787918e+214, "701666420426462",  215 },
            { L_, 7.0179741447121594990283654e-203, "701797414471216", -202 },
            { L_, 7.0181802763514499252997523e+271, "701818027635145",  272 },
            { L_, 7.0184956456878606819724033e-046, "701849564568786",  -45 },
            { L_, 7.0191398966252401189141146e+123, "701913989662524",  124 },
            { L_, 7.0195473177772396091390479e-290, "701954731777724", -289 },
            { L_, 7.0196103536899801301767691e-127, "701961035368998", -126 },
            { L_, 7.0201463635448700303887926e-252, "702014636354487", -251 },
            { L_, 7.0209084007574103253905688e-165, "702090840075741", -164 },
            { L_, 7.0232048672996197792765818e-039, "702320486729962",  -38 },
            { L_, 7.0233595037289397951509516e-166, "702335950372894", -165 },
            { L_, 7.0234585033851196182039727e+185, "702345850338512",  186 },
            { L_, 7.0235512008194898613207771e+282, "702355120081949",  283 },
            { L_, 7.0239400121943894256091916e-223, "702394001219439", -222 },
            { L_, 7.0240011300687498437789980e+099, "702400113006875",  100 },
            { L_, 7.0257518164683402831662411e-200, "702575181646834", -199 },
            { L_, 7.0258976070962004077111745e-011,  "70258976070962",  -10 },
            { L_, 7.0264673792385897700766278e+170, "702646737923859",  171 },
            { L_, 7.0281339040363103250719778e-107, "702813390403631", -106 },
            { L_, 7.0289199127160404295497056e+177, "702891991271604",  178 },
            { L_, 7.0297081646933799779230275e+285, "702970816469338",  286 },
            { L_, 7.0303935431433102529506411e+069, "703039354314331",   70 },
            { L_, 7.0303968551633996400498593e-066,  "70303968551634",  -65 },
            { L_, 7.0329265786606393118324402e+305, "703292657866064",  306 },
            { L_, 7.0349187461616296722418992e-035, "703491874616163",  -34 },
            { L_, 7.0373807556390802579171070e+120, "703738075563908",  121 },
            { L_, 7.0374857609673499822758433e-182, "703748576096735", -181 },
            { L_, 7.0380627795970698141823206e-246, "703806277959707", -245 },
            { L_, 7.0396337003410803115337907e-112, "703963370034108", -111 },
            { L_, 7.0417288090210197736171725e+285, "704172880902102",  286 },
            { L_, 7.0422728670209393963461530e-272, "704227286702094", -271 },
            { L_, 7.0429163708985201457133006e-183, "704291637089852", -182 },
            { L_, 7.0454013010394304066606298e-227, "704540130103943", -226 },
            { L_, 7.0455143568949301006575491e+199, "704551435689493",  200 },
            { L_, 7.0488026512743398661020972e+043, "704880265127434",   44 },
            { L_, 7.0501325206897297555101860e+075, "705013252068973",   76 },
            { L_, 7.0521073172168501262448978e+054, "705210731721685",   55 },
            { L_, 7.0523919527351701482625438e-048, "705239195273517",  -47 },
            { L_, 7.0548471637376297991893696e-276, "705484716373763", -275 },
            { L_, 7.0564936043583196679302065e+268, "705649360435832",  269 },
            { L_, 7.0573751389081596779119959e-095, "705737513890816",  -94 },
            { L_, 7.0593093419457095656803866e-212, "705930934194571", -211 },
            { L_, 7.0597557033209095175388758e+058, "705975570332091",   59 },
            { L_, 7.0598585847811206191641280e+160, "705985858478112",  161 },
            { L_, 7.0618124281283703570994224e+066, "706181242812837",   67 },
            { L_, 7.0622342349092704000000000e+016, "706223423490927",   17 },
            { L_, 7.0629613127315399998369905e-069, "706296131273154",  -68 },
            { L_, 7.0657236349381997462556306e+048,  "70657236349382",   49 },
            { L_, 7.0659190635693702332560527e+260, "706591906356937",  261 },
            { L_, 7.0690063385367396305148942e+181, "706900633853674",  182 },
            { L_, 7.0692601087164301515229123e+257, "706926010871643",  258 },
            { L_, 7.0694219084942698976027100e-033, "706942190849427",  -32 },
            { L_, 7.0723147031962596688420485e-075, "707231470319626",  -74 },
            { L_, 7.0734041946853295388731662e+256, "707340419468533",  257 },
            { L_, 7.0742508475199704924894999e+111, "707425084751997",  112 },
            { L_, 7.0757502121617300411499164e-112, "707575021216173", -111 },
            { L_, 7.0794779207140601449261112e+199, "707947792071406",  200 },
            { L_, 7.0806994405384699575898674e+257, "708069944053847",  258 },
            { L_, 7.0812878179411896720517993e+272, "708128781794119",  273 },
            { L_, 7.0815176331286398866002817e-156, "708151763312864", -155 },
            { L_, 7.0818675654526297143735675e-043, "708186756545263",  -42 },
            { L_, 7.0872273547550001434000796e+089,   "7087227354755",   90 },
            { L_, 7.0886548411382098691576578e-259, "708865484113821", -258 },
            { L_, 7.0894672464842900611676875e-163, "708946724648429", -162 },
            { L_, 7.0924042060272497328345413e-031, "709240420602725",  -30 },
            { L_, 7.0926247423942902501964711e-286, "709262474239429", -285 },
            { L_, 7.0931393064403400120498739e-108, "709313930644034", -107 },
            { L_, 7.0933773528682498755588873e+092, "709337735286825",   93 },
            { L_, 7.0940344993212896505434717e+170, "709403449932129",  171 },
            { L_, 7.0941319710761596543070016e-178, "709413197107616", -177 },
            { L_, 7.0943058149386102477615818e+115, "709430581493861",  116 },
            { L_, 7.0963949885763200980970301e+147, "709639498857632",  148 },
            { L_, 7.0987538323600602275208296e+196, "709875383236006",  197 },
            { L_, 7.0995287242993101335189874e+148, "709952872429931",  149 },
            { L_, 7.1010121804662402800356338e-090, "710101218046624",  -89 },
            { L_, 7.1014792694488700428982171e+109, "710147926944887",  110 },
            { L_, 7.1028484954462498023508626e+249, "710284849544625",  250 },
            { L_, 7.1041815971745298662480417e+064, "710418159717453",   65 },
            { L_, 7.1050165885160294171141362e-051, "710501658851603",  -50 },
            { L_, 7.1082480233473303371872161e-186, "710824802334733", -185 },
            { L_, 7.1153501362154504509742435e-249, "711535013621545", -248 },
            { L_, 7.1162310138903403601864594e-046, "711623101389034",  -45 },
            { L_, 7.1163340813614606092565808e+129, "711633408136146",  130 },
            { L_, 7.1182121367774405076136483e-035, "711821213677744",  -34 },
            { L_, 7.1195923741613602747504635e-277, "711959237416136", -276 },
            { L_, 7.1204313010083999951260132e-031,  "71204313010084",  -30 },
            { L_, 7.1250235895505302837171649e+137, "712502358955053",  138 },
            { L_, 7.1256293469904500433455202e-071, "712562934699045",  -70 },
            { L_, 7.1266410248481100772056139e+164, "712664102484811",  165 },
            { L_, 7.1278941791419400601754342e-243, "712789417914194", -242 },
            { L_, 7.1279924466196605364704979e+263, "712799244661966",  264 },
            { L_, 7.1306441110357500807978265e-020, "713064411103575",  -19 },
            { L_, 7.1312219612940703095103783e-181, "713122196129407", -180 },
            { L_, 7.1328852993393198562344084e+252, "713288529933932",  253 },
            { L_, 7.1338884369656401648182498e-220, "713388843696564", -219 },
            { L_, 7.1341273907945096295219200e+023, "713412739079451",   24 },
            { L_, 7.1361171309366399383796785e-100, "713611713093664",  -99 },
            { L_, 7.1367542467191895049177256e-147, "713675424671919", -146 },
            { L_, 7.1368822119564603582432861e+104, "713688221195646",  105 },
            { L_, 7.1378001652415003628089863e-163,  "71378001652415", -162 },
            { L_, 7.1385403214811900070667424e-304, "713854032148119", -303 },
            { L_, 7.1389937519526606366439089e+085, "713899375195266",   86 },
            { L_, 7.1423780082120799627458459e+155, "714237800821208",  156 },
            { L_, 7.1424985929251102361481269e-043, "714249859292511",  -42 },
            { L_, 7.1432097531826706197303191e+262, "714320975318267",  263 },
            { L_, 7.1451292352975302492896222e+188, "714512923529753",  189 },
            { L_, 7.1455490031073993716433797e-241,  "71455490031074", -240 },
            { L_, 7.1477113310277305733665057e-300, "714771133102773", -299 },
            { L_, 7.1482482206003092964434577e-052, "714824822060031",  -51 },
            { L_, 7.1488796091771299840000000e+018, "714887960917713",   19 },
            { L_, 7.1537714209516200919040000e+022, "715377142095162",   23 },
            { L_, 7.1546028678209194299572973e-182, "715460286782092", -181 },
            { L_, 7.1547738470881804505105432e-025, "715477384708818",  -24 },
            { L_, 7.1552869728042404958245254e-250, "715528697280424", -249 },
            { L_, 7.1572386046171397131267484e-176, "715723860461714", -175 },
            { L_, 7.1610112289753801928657089e-187, "716101122897538", -186 },
            { L_, 7.1629099228903005179806376e-138,  "71629099228903", -137 },
            { L_, 7.1629433778880004712527607e-005,   "7162943377888",   -4 },
            { L_, 7.1693092392051695031110385e+265, "716930923920517",  266 },
            { L_, 7.1698031257074102752839761e-185, "716980312570741", -184 },
            { L_, 7.1716115036202499733329705e-139, "717161150362025", -138 },
            { L_, 7.1750217765235296214197214e+188, "717502177652353",  189 },
            { L_, 7.1779110750504104444208242e+184, "717791107505041",  185 },
            { L_, 7.1801832236212598501087484e+190, "718018322362126",  191 },
            { L_, 7.1817209373229604726409811e+086, "718172093732296",   87 },
            { L_, 7.1818439866935195172275599e-035, "718184398669352",  -34 },
            { L_, 7.1820335905070803840150589e+029, "718203359050708",   30 },
            { L_, 7.1821560572808598216582578e-272, "718215605728086", -271 },
            { L_, 7.1825730314324397658243323e+191, "718257303143244",  192 },
            { L_, 7.1828958913403400905117278e+204, "718289589134034",  205 },
            { L_, 7.1836557379621305956510766e-102, "718365573796213", -101 },
            { L_, 7.1845448811963902298525852e-225, "718454488119639", -224 },
            { L_, 7.1863423439355698117946588e-087, "718634234393557",  -86 },
            { L_, 7.1870332074183398419760126e-260, "718703320741834", -259 },
            { L_, 7.1873668507619293559711565e-260, "718736685076193", -259 },
            { L_, 7.1876071156253893601299234e+060, "718760711562539",   61 },
            { L_, 7.1905446466879503284814156e-262, "719054464668795", -261 },
            { L_, 7.1906620055003993663173335e-272,  "71906620055004", -271 },
            { L_, 7.1956136862988500058328272e-026, "719561368629885",  -25 },
            { L_, 7.1985319497076298844717712e+000, "719853194970763",    1 },
            { L_, 7.2051416477437697070943496e-173, "720514164774377", -172 },
            { L_, 7.2089130429455796717580338e-062, "720891304294558",  -61 },
            { L_, 7.2090535636438805417672305e-136, "720905356364388", -135 },
            { L_, 7.2093513524462101480560685e-214, "720935135244621", -213 },
            { L_, 7.2115471084045697941632810e-156, "721154710840457", -155 },
            { L_, 7.2115832938408197545198450e+270, "721158329384082",  271 },
            { L_, 7.2119129012131696894098201e-274, "721191290121317", -273 },
            { L_, 7.2145944702900296551804780e+245, "721459447029003",  246 },
            { L_, 7.2160203512225303025180162e+071, "721602035122253",   72 },
            { L_, 7.2164376263488805273600000e+020, "721643762634888",   21 },
            { L_, 7.2183043238995301490934934e+042, "721830432389953",   43 },
            { L_, 7.2199624882197595422720000e+021, "721996248821976",   22 },
            { L_, 7.2226820741537902581475633e-013, "722268207415379",  -12 },
            { L_, 7.2241865242927203641908350e+193, "722418652429272",  194 },
            { L_, 7.2248040604533999994742911e+038,  "72248040604534",   39 },
            { L_, 7.2254245356373999925404292e+183,  "72254245356374",  184 },
            { L_, 7.2256198164804799121420916e-247, "722561981648048", -246 },
            { L_, 7.2268734374382202428496659e-289, "722687343743822", -288 },
            { L_, 7.2269861979649203805297008e+033, "722698619796492",   34 },
            { L_, 7.2274515917074198218884807e+235, "722745159170742",  236 },
            { L_, 7.2348174429511901347716328e-016, "723481744295119",  -15 },
            { L_, 7.2373704018573705592274582e+241, "723737040185737",  242 },
            { L_, 7.2397552924352101693541813e-124, "723975529243521", -123 },
            { L_, 7.2428227066633902608434971e+076, "724282270666339",   77 },
            { L_, 7.2440871182955097743098028e+154, "724408711829551",  155 },
            { L_, 7.2442894971374901665625505e-292, "724428949713749", -291 },
            { L_, 7.2443303970256198233783006e-101, "724433039702562", -100 },
            { L_, 7.2456909477605905315502661e-071, "724569094776059",  -70 },
            { L_, 7.2466082298445802953518588e-227, "724660822984458", -226 },
            { L_, 7.2474269878488896026029324e+264, "724742698784889",  265 },
            { L_, 7.2482371600919499099720779e-017, "724823716009195",  -16 },
            { L_, 7.2487781977920299413805349e-264, "724877819779203", -263 },
            { L_, 7.2491389298550801672258654e+255, "724913892985508",  256 },
            { L_, 7.2494474647111494813276097e+178, "724944746471115",  179 },
            { L_, 7.2496758152470407556553352e-192, "724967581524704", -191 },
            { L_, 7.2497890986171806887001287e-226, "724978909861718", -225 },
            { L_, 7.2527213741411800350685927e+258, "725272137414118",  259 },
            { L_, 7.2551042672954896321997224e+119, "725510426729549",  120 },
            { L_, 7.2568766281329105777786974e+160, "725687662813291",  161 },
            { L_, 7.2579879472807700119935359e+108, "725798794728077",  109 },
            { L_, 7.2583321421654700568084480e+024, "725833214216547",   25 },
            { L_, 7.2598986928541100669652105e+286, "725989869285411",  287 },
            { L_, 7.2623340420926005811966256e-086,  "72623340420926",  -85 },
            { L_, 7.2640193296019098372174261e-171, "726401932960191", -170 },
            { L_, 7.2645052247654103872746518e-167, "726450522476541", -166 },
            { L_, 7.2657025899176696934839642e+127, "726570258991767",  128 },
            { L_, 7.2657144329823503910241105e-149, "726571443298235", -148 },
            { L_, 7.2694252148179097905778721e+085, "726942521481791",   86 },
            { L_, 7.2747330840056296513047024e-287, "727473308400563", -286 },
            { L_, 7.2764179654176593498983084e+287, "727641796541766",  288 },
            { L_, 7.2801029830382999528837150e+039,  "72801029830383",   40 },
            { L_, 7.2803927310196395522849590e+278, "728039273101964",  279 },
            { L_, 7.2822131684152404173668503e+142, "728221316841524",  143 },
            { L_, 7.2828864089658702614928724e+223, "728288640896587",  224 },
            { L_, 7.2831149215818099095454209e-056, "728311492158181",  -55 },
            { L_, 7.2858783499593196886685555e-191, "728587834995932", -190 },
            { L_, 7.2879286573893098831176758e+009, "728792865738931",   10 },
            { L_, 7.2879694381496695366288648e+297, "728796943814967",  298 },
            { L_, 7.2879741205158299850598488e+208, "728797412051583",  209 },
            { L_, 7.2892728675219200474818559e-127, "728927286752192", -126 },
            { L_, 7.2959129631733199901361084e+201, "729591296317332",  202 },
            { L_, 7.2973960301384201343056373e-227, "729739603013842", -226 },
            { L_, 7.2995894446957201725713293e-238, "729958944469572", -237 },
            { L_, 7.2996118428663503340353848e+170, "729961184286635",  171 },
            { L_, 7.2998686947417503688484430e+005, "729986869474175",    6 },
            { L_, 7.3016205071024202935575752e+298, "730162050710242",  299 },
            { L_, 7.3030994080677799066738746e+099, "730309940806778",  100 },
            { L_, 7.3035895453682196398775058e-177, "730358954536822", -176 },
            { L_, 7.3072925503289192914962769e+007, "730729255032892",    8 },
            { L_, 7.3084270348363605996164242e-226, "730842703483636", -225 },
            { L_, 7.3084455388089997565311895e+266,   "7308445538809",  267 },
            { L_, 7.3098613603245104980468750e+011, "730986136032451",   12 },
            { L_, 7.3146363416141094847944066e+041, "731463634161411",   42 },
            { L_, 7.3159406650603205219357845e+157, "731594066506032",  158 },
            { L_, 7.3165900229704004209195057e+247,  "73165900229704",  248 },
            { L_, 7.3172262942073299654822117e-264, "731722629420733", -263 },
            { L_, 7.3189546753279894887362819e-173, "731895467532799", -172 },
            { L_, 7.3201857060748801709234956e+119, "732018570607488",  120 },
            { L_, 7.3206949849674600763404467e-069, "732069498496746",  -68 },
            { L_, 7.3214487186246599891627324e+307, "732144871862466",  308 },
            { L_, 7.3222016843323396789723662e+274, "732220168433234",  275 },
            { L_, 7.3237625024090300569655382e+185, "732376250240903",  186 },
            { L_, 7.3266710044902895653438983e-014, "732667100449029",  -13 },
            { L_, 7.3321307906099398956036389e-174, "733213079060994", -173 },
            { L_, 7.3321574409526896468965094e+170, "733215744095269",  171 },
            { L_, 7.3328409863447199394066953e+131, "733284098634472",  132 },
            { L_, 7.3351122048754895607238255e-240, "733511220487549", -239 },
            { L_, 7.3364439920536000470712320e+024,  "73364439920536",   25 },
            { L_, 7.3383690360055995512090876e+177,  "73383690360056",  178 },
            { L_, 7.3400581463543197535035800e-177, "734005814635432", -176 },
            { L_, 7.3408725728197994806372539e+259,  "73408725728198",  260 },
            { L_, 7.3438573684708101861368426e-243, "734385736847081", -242 },
            { L_, 7.3445091732676801573980500e+145, "734450917326768",  146 },
            { L_, 7.3457140020310105274727740e+157, "734571400203101",  158 },
            { L_, 7.3468150122828204488874430e+162, "734681501228282",  163 },
            { L_, 7.3478920234399599748345447e-224, "734789202343996", -223 },
            { L_, 7.3499873515520202280459087e-165, "734998735155202", -164 },
            { L_, 7.3503692546585400547846999e+227, "735036925465854",  228 },
            { L_, 7.3518248588295602421679721e+200, "735182485882956",  201 },
            { L_, 7.3537761574278994766328219e+129,  "73537761574279",  130 },
            { L_, 7.3550670351438300070686279e-152, "735506703514383", -151 },
            { L_, 7.3560213772860398553775770e+217, "735602137728604",  218 },
            { L_, 7.3564054286905896188069932e+081, "735640542869059",   82 },
            { L_, 7.3573111421138399197159907e+121, "735731114211384",  122 },
            { L_, 7.3573619316445402863690468e+236, "735736193164454",  237 },
            { L_, 7.3578170436843096296652800e+024, "735781704368431",   25 },
            { L_, 7.3633744297600202262401581e+006, "736337442976002",    7 },
            { L_, 7.3638465707517692937159880e-024, "736384657075177",  -23 },
            { L_, 7.3641483635495395412605595e+044, "736414836354954",   45 },
            { L_, 7.3656252538277597732676692e-114, "736562525382776", -113 },
            { L_, 7.3676608598722600376829268e-025, "736766085987226",  -24 },
            { L_, 7.3685767740588001969418464e-135,  "73685767740588", -134 },
            { L_, 7.3691404357854402573491497e-188, "736914043578544", -187 },
            { L_, 7.3705961661188002755379200e+023,  "73705961661188",   24 },
            { L_, 7.3719097372942902572871680e+172, "737190973729429",  173 },
            { L_, 7.3722724824994903701405779e-170, "737227248249949", -169 },
            { L_, 7.3723996521081495061600691e+110, "737239965210815",  111 },
            { L_, 7.3725655858982700314672640e+229, "737256558589827",  230 },
            { L_, 7.3762332201253395292572564e-288, "737623322012534", -287 },
            { L_, 7.3795834030556100540735812e-191, "737958340305561", -190 },
            { L_, 7.3800106578319901007202872e-106, "738001065783199", -105 },
            { L_, 7.3862611572275497382059715e+064, "738626115722755",   65 },
            { L_, 7.3866884753273196673753956e-059, "738668847532732",  -58 },
            { L_, 7.3868114279823204173283026e+242, "738681142798232",  243 },
            { L_, 7.3873258560257998329025284e-261,  "73873258560258", -260 },
            { L_, 7.3889073028409598987202597e-174, "738890730284096", -173 },
            { L_, 7.3889912603761196399897102e-155, "738899126037612", -154 },
            { L_, 7.3891734371517505548919044e+104, "738917343715175",  105 },
            { L_, 7.3907618527558006040311224e+163,  "73907618527558",  164 },
            { L_, 7.3916366566974494662045339e-197, "739163665669745", -196 },
            { L_, 7.3940794731688494337156013e-179, "739407947316885", -178 },
            { L_, 7.3948176826247795761704687e-093, "739481768262478",  -92 },
            { L_, 7.3954381424632895369047336e-107, "739543814246329", -106 },
            { L_, 7.3969814265895901766737312e-228, "739698142658959", -227 },
            { L_, 7.3973468445944596475621902e-005, "739734684459446",   -4 },
            { L_, 7.3985003603344793905213630e+029, "739850036033448",   30 },
            { L_, 7.4005521299947501056654684e-230, "740055212999475", -229 },
            { L_, 7.4066523937502197846453187e-292, "740665239375022", -291 },
            { L_, 7.4066917242786400423203326e+162, "740669172427864",  163 },
            { L_, 7.4070108143650496828309052e+209, "740701081436505",  210 },
            { L_, 7.4090600958487899060792630e+258, "740906009584879",  259 },
            { L_, 7.4090940930900504507986767e+256, "740909409309005",  257 },
            { L_, 7.4121364442709497774518717e+139, "741213644427095",  140 },
            { L_, 7.4133759263781797189646555e-196, "741337592637818", -195 },
            { L_, 7.4141401850630895210956915e+147, "741414018506309",  148 },
            { L_, 7.4146868000049901444796203e-041, "741468680000499",  -40 },
            { L_, 7.4162547312690095905103440e+039, "741625473126901",   40 },
            { L_, 7.4173240718152397350829727e+107, "741732407181524",  108 },
            { L_, 7.4195644227527200604082387e-103, "741956442275272", -102 },
            { L_, 7.4210348445561404290980253e-181, "742103484455614", -180 },
            { L_, 7.4212964271713003969869527e-090,  "74212964271713",  -89 },
            { L_, 7.4221295308942902220346353e+170, "742212953089429",  171 },
            { L_, 7.4226813084974502188787607e+231, "742268130849745",  232 },
            { L_, 7.4240864522210496145735651e+293, "742408645222105",  294 },
            { L_, 7.4309088220967502926867876e-149, "743090882209675", -148 },
            { L_, 7.4309601072026396092008824e-015, "743096010720264",  -14 },
            { L_, 7.4316838789783101444757605e+125, "743168387897831",  126 },
            { L_, 7.4331748846355299369329766e-103, "743317488463553", -102 },
            { L_, 7.4351296197043201521170118e-091, "743512961970432",  -90 },
            { L_, 7.4359060889534801097459126e+164, "743590608895348",  165 },
            { L_, 7.4384947074845903776314450e+217, "743849470748459",  218 },
            { L_, 7.4403311614071402112364069e-282, "744033116140714", -281 },
            { L_, 7.4428072803097103622637650e-185, "744280728030971", -184 },
            { L_, 7.4434107495815197908133081e+151, "744341074958152",  152 },
            { L_, 7.4453226168967497873021935e+107, "744532261689675",  108 },
            { L_, 7.4487435258963002847430837e-202,  "74487435258963", -201 },
            { L_, 7.4527933784398903493568390e-291, "745279337843989", -290 },
            { L_, 7.4533318338915204867833607e+085, "745333183389152",   86 },
            { L_, 7.4560600989278601680903196e+093, "745606009892786",   94 },
            { L_, 7.4561398421063695754303542e+294, "745613984210637",  295 },
            { L_, 7.4576394771320002311618122e+250,   "7457639477132",  251 },
            { L_, 7.4576933159701703499898314e+062, "745769331597017",   63 },
            { L_, 7.4598981471032704413681671e+177, "745989814710327",  178 },
            { L_, 7.4629809433282495028781999e+079, "746298094332825",   80 },
            { L_, 7.4655561383347295890445301e-169, "746555613833473", -168 },
            { L_, 7.4656240711615202045600376e-179, "746562407116152", -178 },
            { L_, 7.4673541652488596002805855e+070, "746735416524886",   71 },
            { L_, 7.4674520463776901012281592e-121, "746745204637769", -120 },
            { L_, 7.4717016517106499937488586e-054, "747170165171065",  -53 },
            { L_, 7.4765090824109806238866653e+246, "747650908241098",  247 },
            { L_, 7.4778583065200904123664932e+114, "747785830652009",  115 },
            { L_, 7.4785671968583599004274446e+042, "747856719685836",   43 },
            { L_, 7.4795088672236293554306030e+007, "747950886722363",    8 },
            { L_, 7.4812191384133398101019313e-042, "748121913841334",  -41 },
            { L_, 7.4819856258753502611190253e+070, "748198562587535",   71 },
            { L_, 7.4822449050560397764297518e+124, "748224490505604",  125 },
            { L_, 7.4834143853937602747440741e+306, "748341438539376",  307 },
            { L_, 7.4834628553198202549798097e+266, "748346285531982",  267 },
            { L_, 7.4854194419806594347827200e+023, "748541944198066",   24 },
            { L_, 7.4860964378276392817497253e+007, "748609643782764",    8 },
            { L_, 7.4873626945155594356614373e-170, "748736269451556", -169 },
            { L_, 7.4881827532884402611417958e+044, "748818275328844",   45 },
            { L_, 7.4889793755468503932367439e-301, "748897937554685", -300 },
            { L_, 7.4906656535275698991631488e-279, "749066565352757", -278 },
            { L_, 7.4925207323836602032120227e+071, "749252073238366",   72 },
            { L_, 7.4937064097692005111315096e-306,  "74937064097692", -305 },
            { L_, 7.4946618549644600708110743e+237, "749466185496446",  238 },
            { L_, 7.4949006589638003801972465e+238,  "74949006589638",  239 },
            { L_, 7.4977054690553606272470055e-061, "749770546905536",  -60 },
            { L_, 7.4988705928524701681560166e+208, "749887059285247",  209 },
            { L_, 7.4990554429741793155450955e-201, "749905544297418", -200 },
            { L_, 7.4993423119004701485575817e-110, "749934231190047", -109 },
            { L_, 7.5000379924108995529807564e-064,  "75000379924109",  -63 },
            { L_, 7.5015719610037201434954004e+295, "750157196100372",  296 },
            { L_, 7.5017200477224495653546745e+065, "750172004772245",   66 },
            { L_, 7.5035808107587507078262527e-189, "750358081075875", -188 },
            { L_, 7.5047378941891703088939739e-098, "750473789418917",  -97 },
            { L_, 7.5048320627941603848296824e+054, "750483206279416",   55 },
            { L_, 7.5074732630302597470141626e+237, "750747326303026",  238 },
            { L_, 7.5095672459305800139740155e-139, "750956724593058", -138 },
            { L_, 7.5107989544381202384349071e+055, "751079895443812",   56 },
            { L_, 7.5109255298569595666629589e-186, "751092552985696", -185 },
            { L_, 7.5109445680170598462514767e-236, "751094456801706", -235 },
            { L_, 7.5113448591390203571438270e-290, "751134485913902", -289 },
            { L_, 7.5126987527705004483455085e+254,  "75126987527705",  255 },
            { L_, 7.5131108880376999775420199e-212,  "75131108880377", -211 },
            { L_, 7.5150644872703600011759781e-059, "751506448727036",  -58 },
            { L_, 7.5167497851330799013250484e+229, "751674978513308",  230 },
            { L_, 7.5177285371967193011840985e-142, "751772853719672", -141 },
            { L_, 7.5181230693447895132256802e-263, "751812306934479", -262 },
            { L_, 7.5185983216670904159545898e+009, "751859832166709",   10 },
            { L_, 7.5240637508618704077025743e-071, "752406375086187",  -70 },
            { L_, 7.5289362778899405384436250e+004, "752893627788994",    5 },
            { L_, 7.5311726364873901137524776e+168, "753117263648739",  169 },
            { L_, 7.5328729774166205467764666e-009, "753287297741662",   -8 },
            { L_, 7.5335742470812597592674816e+273, "753357424708126",  274 },
            { L_, 7.5336942222891894505832322e+291, "753369422228919",  292 },
            { L_, 7.5338108416083694472451843e-148, "753381084160837", -147 },
            { L_, 7.5356353251659406139510505e+032, "753563532516594",   33 },
            { L_, 7.5361517622165503205070794e-016, "753615176221655",  -15 },
            { L_, 7.5366534194049000273641605e+235,  "75366534194049",  236 },
            { L_, 7.5377510644023804737695243e-119, "753775106440238", -118 },
            { L_, 7.5417005616267996908901814e+195,  "75417005616268",  196 },
            { L_, 7.5418154350165400009124478e+242, "754181543501654",  243 },
            { L_, 7.5419236454362703704579612e+163, "754192364543627",  164 },
            { L_, 7.5426476067502502468072983e-226, "754264760675025", -225 },
            { L_, 7.5436765391882994011156397e-073,  "75436765391883",  -72 },
            { L_, 7.5460786281635703694346591e-132, "754607862816357", -131 },
            { L_, 7.5461993562421301269531250e+011, "754619935624213",   12 },
            { L_, 7.5491777752151502914617058e+073, "754917777521515",   74 },
            { L_, 7.5513019087244994527503384e-216,  "75513019087245", -215 },
            { L_, 7.5520663044801100838924706e-138, "755206630448011", -137 },
            { L_, 7.5522379763690598456010849e+093, "755223797636906",   94 },
            { L_, 7.5556127900873693092578728e-189, "755561279008737", -188 },
            { L_, 7.5568361912933698294684886e-103, "755683619129337", -102 },
            { L_, 7.5575934176461998601130802e-118,  "75575934176462", -117 },
            { L_, 7.5581319649127493231815473e+212, "755813196491275",  213 },
            { L_, 7.5629984958172305687604891e-272, "756299849581723", -271 },
            { L_, 7.5638760574823892377407451e-071, "756387605748239",  -70 },
            { L_, 7.5643662549395896200673819e-195, "756436625493959", -194 },
            { L_, 7.5677433753149303836382621e-151, "756774337531493", -150 },
            { L_, 7.5689494594540495462587100e-305, "756894945945405", -304 },
            { L_, 7.5716517696266696767275619e+211, "757165176962667",  212 },
            { L_, 7.5719632563130001753175658e-093,   "7571963256313",  -92 },
            { L_, 7.5747590893952093424258149e-161, "757475908939521", -160 },
            { L_, 7.5759745408873201217602092e-181, "757597454088732", -180 },
            { L_, 7.5784871056243804818138760e+234, "757848710562438",  235 },
            { L_, 7.5786723898170396216998866e+254, "757867238981704",  255 },
            { L_, 7.5812105713233796634968172e-305, "758121057132338", -304 },
            { L_, 7.5822458702135304322980965e-261, "758224587021353", -260 },
            { L_, 7.5833107847637494882721276e-005, "758331078476375",   -4 },
            { L_, 7.5839169676939099534629961e+044, "758391696769391",   45 },
            { L_, 7.5861917373409299354617286e-043, "758619173734093",  -42 },
            { L_, 7.5894547996014199087652194e+115, "758945479960142",  116 },
            { L_, 7.5922537449416805285223030e+185, "759225374494168",  186 },
            { L_, 7.5926125298136296906597345e+274, "759261252981363",  275 },
            { L_, 7.5930121202263798192309663e-100, "759301212022638",  -99 },
            { L_, 7.5946097583047998810287849e+031,  "75946097583048",   32 },
            { L_, 7.5970499050472202056548445e-017, "759704990504722",  -16 },
            { L_, 7.5977019745919097003560218e-045, "759770197459191",  -44 },
            { L_, 7.5980327058139196346252663e+191, "759803270581392",  192 },
            { L_, 7.5990629058543897975092556e-249, "759906290585439", -248 },
            { L_, 7.6036767344059301101441133e-051, "760367673440593",  -50 },
            { L_, 7.6043711760447396586747745e+238, "760437117604474",  239 },
            { L_, 7.6044797501653201574285932e-163, "760447975016532", -162 },
            { L_, 7.6055296407844502626360850e-002, "760552964078445",   -1 },
            { L_, 7.6057781567588698748155348e-108, "760577815675887", -107 },
            { L_, 7.6059532338368402935946167e-040, "760595323383684",  -39 },
            { L_, 7.6090755419117999860858128e-203,  "76090755419118", -202 },
            { L_, 7.6113405453713200500018212e-153, "761134054537132", -152 },
            { L_, 7.6127617613328699964998564e+269, "761276176133287",  270 },
            { L_, 7.6157420587531099436184929e+069, "761574205875311",   70 },
            { L_, 7.6175440963712193384617237e+165, "761754409637122",  166 },
            { L_, 7.6193603168821695558305007e-129, "761936031688217", -128 },
            { L_, 7.6193725483823896625661219e+138, "761937254838239",  139 },
            { L_, 7.6196876348983897772527812e-123, "761968763489839", -122 },
            { L_, 7.6218995437208201586065399e+087, "762189954372082",   88 },
            { L_, 7.6239402020904001586129567e-028,  "76239402020904",  -27 },
            { L_, 7.6280209212418499838181797e+167, "762802092124185",  168 },
            { L_, 7.6306316189247698194176249e-151, "763063161892477", -150 },
            { L_, 7.6308584682795501901495880e+169, "763085846827955",  170 },
            { L_, 7.6320407244738998989410521e+036,  "76320407244739",   37 },
            { L_, 7.6331598826981402964131840e+026, "763315988269814",   27 },
            { L_, 7.6365488992511101274740516e+207, "763654889925111",  208 },
            { L_, 7.6368112796269897364785884e+240, "763681127962699",  241 },
            { L_, 7.6404022676698306029184047e-073, "764040226766983",  -72 },
            { L_, 7.6427045115340598252945885e+256, "764270451153406",  257 },
            { L_, 7.6439537800517402511996782e+108, "764395378005174",  109 },
            { L_, 7.6449238722223106157596724e+247, "764492387222231",  248 },
            { L_, 7.6453644955648097066776783e+199, "764536449556481",  200 },
            { L_, 7.6460480398014297644546618e+082, "764604803980143",   83 },
            { L_, 7.6466433286339197958646467e+078, "764664332863392",   79 },
            { L_, 7.6482041375504499529914160e-199, "764820413755045", -198 },
            { L_, 7.6486070080560893101277268e+091, "764860700805609",   92 },
            { L_, 7.6490607689327294904238058e+302, "764906076893273",  303 },
            { L_, 7.6513212762251402713486671e+141, "765132127622514",  142 },
            { L_, 7.6515653922074702087806725e+037, "765156539220747",   38 },
            { L_, 7.6517574310005007271353771e-164,  "76517574310005", -163 },
            { L_, 7.6547661168842905260916736e+026, "765476611688429",   27 },
            { L_, 7.6558720679669095738120120e+254, "765587206796691",  255 },
            { L_, 7.6562164846932795666337462e-118, "765621648469328", -117 },
            { L_, 7.6569678826569001211661087e+295,  "76569678826569",  296 },
            { L_, 7.6578775271825305600000000e+017, "765787752718253",   18 },
            { L_, 7.6585942336480204492350165e-288, "765859423364802", -287 },
            { L_, 7.6590269841095394872959396e-144, "765902698410954", -143 },
            { L_, 7.6612472273082595548913378e+234, "766124722730826",  235 },
            { L_, 7.6650825133232405603229004e+296, "766508251332324",  297 },
            { L_, 7.6674255688903500274603721e-214, "766742556889035", -213 },
            { L_, 7.6677226473116305848072747e+101, "766772264731163",  102 },
            { L_, 7.6715025152232600198825740e+195, "767150251522326",  196 },
            { L_, 7.6733408655868095498541429e-020, "767334086558681",  -19 },
            { L_, 7.6749424451213101761355373e-009, "767494244512131",   -8 },
            { L_, 7.6787147241500993587869639e-055,  "76787147241501",  -54 },
            { L_, 7.6819835570891403165500453e+065, "768198355708914",   66 },
            { L_, 7.6820291205884594945959090e+079, "768202912058846",   80 },
            { L_, 7.6825881102077401837652966e+274, "768258811020774",  275 },
            { L_, 7.6828454205838196336340475e+227, "768284542058382",  228 },
            { L_, 7.6842128060985904743662533e-235, "768421280609859", -234 },
            { L_, 7.6843094529267499617064410e+234, "768430945292675",  235 },
            { L_, 7.6844600518577196590718846e-029, "768446005185772",  -28 },
            { L_, 7.6864152204954800235443720e+195, "768641522049548",  196 },
            { L_, 7.6894718816011098807754726e-301, "768947188160111", -300 },
            { L_, 7.6897744668836704894323778e+144, "768977446688367",  145 },
            { L_, 7.6937150104832006036293035e-030,  "76937150104832",  -29 },
            { L_, 7.6943998183040692902069463e-093, "769439981830407",  -92 },
            { L_, 7.6952064948434100070471743e-258, "769520649484341", -257 },
            { L_, 7.6979808582398994786238012e+231,  "76979808582399",  232 },
            { L_, 7.6987092102882303600267993e-167, "769870921028823", -166 },
            { L_, 7.6987252286507301362231339e+251, "769872522865073",  252 },
            { L_, 7.6999220138990298193846232e-141, "769992201389903", -140 },
            { L_, 7.7001013068656103313489681e+292, "770010130686561",  293 },
            { L_, 7.7010372893251998852543650e+091,  "77010372893252",   92 },
            { L_, 7.7017723671962498915759565e+171, "770177236719625",  172 },
            { L_, 7.7035729459024296014291742e-081, "770357294590243",  -80 },
            { L_, 7.7063640511322304694641121e+238, "770636405113223",  239 },
            { L_, 7.7125389711906105997473598e-132, "771253897119061", -131 },
            { L_, 7.7131512134230603971034969e+106, "771315121342306",  107 },
            { L_, 7.7174096391870294481579737e+137, "771740963918703",  138 },
            { L_, 7.7174308612673802968599392e-243, "771743086126738", -242 },
            { L_, 7.7191390089666297037319995e-015, "771913900896663",  -14 },
            { L_, 7.7197682461703101533169071e+137, "771976824617031",  138 },
            { L_, 7.7231013955825304928123120e+082, "772310139558253",   83 },
            { L_, 7.7264498192399595718445051e+142, "772644981923996",  143 },
            { L_, 7.7272166002897003304089913e-028,  "77272166002897",  -27 },
            { L_, 7.7303878890605696606147964e+077, "773038788906057",   78 },
            { L_, 7.7308373500965899819489237e+296, "773083735009659",  297 },
            { L_, 7.7328204969300702301021429e+113, "773282049693007",  114 },
            { L_, 7.7333726946006704416207808e-226, "773337269460067", -225 },
            { L_, 7.7347112992342599074324334e+104, "773471129923426",  105 },
            { L_, 7.7373975884933406159949714e-139, "773739758849334", -138 },
            { L_, 7.7374589482673704624176025e+007, "773745894826737",    8 },
            { L_, 7.7381834625787402655803186e-037, "773818346257874",  -36 },
            { L_, 7.7392377968445598496475295e+120, "773923779684456",  121 },
            { L_, 7.7406987313369402810482923e+270, "774069873133694",  271 },
            { L_, 7.7424144048929506811916765e-307, "774241440489295", -306 },
            { L_, 7.7435975292653992057159919e-186,  "77435975292654", -185 },
            { L_, 7.7442124476248299926524741e-097, "774421244762483",  -96 },
            { L_, 7.7442907452966195687014274e+228, "774429074529662",  229 },
            { L_, 7.7451177116174302264603346e+130, "774511771161743",  131 },
            { L_, 7.7456721833812001570160478e-158,  "77456721833812", -157 },
            { L_, 7.7460005221712608000000000e+016, "774600052217126",   17 },
            { L_, 7.7489326730997302677726813e-014, "774893267309973",  -13 },
            { L_, 7.7503442938117194141965649e-005, "775034429381172",   -4 },
            { L_, 7.7508670728507096925416830e-280, "775086707285071", -279 },
            { L_, 7.7511760961249596256468728e+080, "775117609612496",   81 },
            { L_, 7.7517945968417602611855166e-003, "775179459684176",   -2 },
            { L_, 7.7520462436700500291756554e-214, "775204624367005", -213 },
            { L_, 7.7528445649711299401499288e-109, "775284456497113", -108 },
            { L_, 7.7544876620293303020602539e+181, "775448766202933",  182 },
            { L_, 7.7573346184282998792249249e-216,  "77573346184283", -215 },
            { L_, 7.7576263478325905498297881e-180, "775762634783259", -179 },
            { L_, 7.7578424978782700902494918e+257, "775784249787827",  258 },
            { L_, 7.7620233088427804523491548e+086, "776202330884278",   87 },
            { L_, 7.7683884576681304299899500e-122, "776838845766813", -121 },
            { L_, 7.7722409690453795042346486e+156, "777224096904538",  157 },
            { L_, 7.7728737397201601775766779e+141, "777287373972016",  142 },
            { L_, 7.7735150139923800140059110e+093, "777351501399238",   94 },
            { L_, 7.7746908242988595802538005e+133, "777469082429886",  134 },
            { L_, 7.7758638182497205234426795e+286, "777586381824972",  287 },
            { L_, 7.7769036862691204326970045e+054, "777690368626912",   55 },
            { L_, 7.7774297431013498211497763e-212, "777742974310135", -211 },
            { L_, 7.7792407096035800359460405e-186, "777924070960358", -185 },
            { L_, 7.7796221205114000684140962e-011,  "77796221205114",  -10 },
            { L_, 7.7796549638463406488536752e-096, "777965496384634",  -95 },
            { L_, 7.7804462267464905589212268e-195, "778044622674649", -194 },
            { L_, 7.7814798227825104762865016e-041, "778147982278251",  -40 },
            { L_, 7.7822960357755694017782018e-275, "778229603577557", -274 },
            { L_, 7.7834695457772298570578063e-212, "778346954577723", -211 },
            { L_, 7.7845338023854703802559628e-290, "778453380238547", -289 },
            { L_, 7.7885733660356001666738453e+124,  "77885733660356",  125 },
            { L_, 7.7918939110533004776430676e+133,  "77918939110533",  134 },
            { L_, 7.7940065392925901568983765e-262, "779400653929259", -261 },
            { L_, 7.7941997022996796976092007e+182, "779419970229968",  183 },
            { L_, 7.7946473364258696808335417e-155, "779464733642587", -154 },
            { L_, 7.7953309848504194853808778e+063, "779533098485042",   64 },
            { L_, 7.7964612531596502220180691e+220, "779646125315965",  221 },
            { L_, 7.7983946210204996920647800e-113,  "77983946210205", -112 },
            { L_, 7.8024602420039201963481734e+244, "780246024200392",  245 },
            { L_, 7.8029396903825203621025008e+179, "780293969038252",  180 },
            { L_, 7.8052570540538901047826973e+028, "780525705405389",   29 },
            { L_, 7.8053071115815105093491139e-299, "780530711158151", -298 },
            { L_, 7.8054848434837501359278962e-086, "780548484348375",  -85 },
            { L_, 7.8081252814204996211868999e+240,  "78081252814205",  241 },
            { L_, 7.8086471276853997025568788e+000,  "78086471276854",    1 },
            { L_, 7.8087732130987800969235378e-025, "780877321309878",  -24 },
            { L_, 7.8092571868405896244362679e-196, "780925718684059", -195 },
            { L_, 7.8096618576310697999162463e-007, "780966185763107",   -6 },
            { L_, 7.8099028227584899199087902e+096, "780990282275849",   97 },
            { L_, 7.8134939685313101157091663e+084, "781349396853131",   85 },
            { L_, 7.8142949585120799019932747e+006, "781429495851208",    7 },
            { L_, 7.8153828281295396725338755e+244, "781538282812954",  245 },
            { L_, 7.8191414911056603586395614e-078, "781914149110566",  -77 },
            { L_, 7.8202579441535401697792927e-122, "782025794415354", -121 },
            { L_, 7.8203167149039598032983598e-209, "782031671490396", -208 },
            { L_, 7.8226084071281802484420408e-072, "782260840712818",  -71 },
            { L_, 7.8254751366852700000000000e+014, "782547513668527",   15 },
            { L_, 7.8280014451459395631242674e-156, "782800144514594", -155 },
            { L_, 7.8286091374436496211621200e+092, "782860913744365",   93 },
            { L_, 7.8290271668444201525548084e-257, "782902716684442", -256 },
            { L_, 7.8308090155715997155771863e-209,  "78308090155716", -208 },
            { L_, 7.8317052962701102832978645e-205, "783170529627011", -204 },
            { L_, 7.8345203530693400000000000e+014, "783452035306934",   15 },
            { L_, 7.8345720900916196670566351e-079, "783457209009162",  -78 },
            { L_, 7.8345909379871795773652423e+080, "783459093798718",   81 },
            { L_, 7.8345956969671795730361389e-233, "783459569696718", -232 },
            { L_, 7.8357318713548999710037934e-230,  "78357318713549", -229 },
            { L_, 7.8363447128262296246248297e+003, "783634471282623",    4 },
            { L_, 7.8363805603377598413475419e-143, "783638056033776", -142 },
            { L_, 7.8374813330476894269192356e-216, "783748133304769", -215 },
            { L_, 7.8422559420573800084137021e+301, "784225594205738",  302 },
            { L_, 7.8426178322185803599988847e+127, "784261783221858",  128 },
            { L_, 7.8448202143398093610926480e-002, "784482021433981",   -1 },
            { L_, 7.8462933258886596171144162e-161, "784629332588866", -160 },
            { L_, 7.8466940460756903770548570e-197, "784669404607569", -196 },
            { L_, 7.8484473527288401646722015e-054, "784844735272884",  -53 },
            { L_, 7.8492983367538501196262900e+051, "784929833675385",   52 },
            { L_, 7.8498118985605796529668194e+108, "784981189856058",  109 },
            { L_, 7.8502007052786903777406190e+076, "785020070527869",   77 },
            { L_, 7.8549470768400805565215394e+092, "785494707684008",   93 },
            { L_, 7.8550058852057898913678589e-011, "785500588520579",  -10 },
            { L_, 7.8565199913539497668610298e-189, "785651999135395", -188 },
            { L_, 7.8567852910368603835234912e-042, "785678529103686",  -41 },
            { L_, 7.8578519175353400894578705e+090, "785785191753534",   91 },
            { L_, 7.8597070871612703179889788e+167, "785970708716127",  168 },
            { L_, 7.8614933279641196062183402e-188, "786149332796412", -187 },
            { L_, 7.8623097909064202208592685e-125, "786230979090642", -124 },
            { L_, 7.8645200779462495747428094e+232, "786452007794625",  233 },
            { L_, 7.8648638583613102368072363e+176, "786486385836131",  177 },
            { L_, 7.8664887115755705786569791e-222, "786648871157557", -221 },
            { L_, 7.8666866811586897323540454e-031, "786668668115869",  -30 },
            { L_, 7.8681201805063394753743020e-272, "786812018050634", -271 },
            { L_, 7.8686165332267495768062838e-259, "786861653322675", -258 },
            { L_, 7.8702958091762804731849071e-021, "787029580917628",  -20 },
            { L_, 7.8708025612759902879688883e+145, "787080256127599",  146 },
            { L_, 7.8722434939254404204801039e+186, "787224349392544",  187 },
            { L_, 7.8733091377302995977904843e-003,  "78733091377303",   -2 },
            { L_, 7.8753127679070294718081079e+076, "787531276790703",   77 },
            { L_, 7.8753466139616498840667819e-292, "787534661396165", -291 },
            { L_, 7.8756556431352397824000000e+020, "787565564313524",   21 },
            { L_, 7.8762363633175003051757812e+010,  "78762363633175",   11 },
            { L_, 7.8764820678942503392328317e-166, "787648206789425", -165 },
            { L_, 7.8766408317266803263026316e-111, "787664083172668", -110 },
            { L_, 7.8774421157234098170283667e+170, "787744211572341",  171 },
            { L_, 7.8776074961074794665299115e-020, "787760749610748",  -19 },
            { L_, 7.8783494754223404559894559e-092, "787834947542234",  -91 },
            { L_, 7.8791081816088195767356345e+087, "787910818160882",   88 },
            { L_, 7.8795560800650894493297191e+072, "787955608006509",   73 },
            { L_, 7.8810285627839597862186013e-074, "788102856278396",  -73 },
            { L_, 7.8821333092779796875000000e+013, "788213330927798",   14 },
            { L_, 7.8824291904199600219726562e+007, "788242919041996",    8 },
            { L_, 7.8828220180155203549092847e+062, "788282201801552",   63 },
            { L_, 7.8829380298424501052888032e-160, "788293802984245", -159 },
            { L_, 7.8835743995103796636780686e+180, "788357439951038",  181 },
            { L_, 7.8852888241342598174633006e-103, "788528882413426", -102 },
            { L_, 7.8860049292165095798581481e+043, "788600492921651",   44 },
            { L_, 7.8860194712151698821152211e-280, "788601947121517", -279 },
            { L_, 7.8860954966615204590103219e-275, "788609549666152", -274 },
            { L_, 7.8875820494676799648279968e+192, "788758204946768",  193 },
            { L_, 7.8881257238120898959801958e+033, "788812572381209",   34 },
            { L_, 7.8923893985663599475082671e-032, "789238939856636",  -31 },
            { L_, 7.8944235697914401042428112e+121, "789442356979144",  122 },
            { L_, 7.8948776940820907503928702e-015, "789487769408209",  -14 },
            { L_, 7.8990564065912693466037458e+261, "789905640659127",  262 },
            { L_, 7.9008614361011299355583218e+096, "790086143610113",   97 },
            { L_, 7.9016904254821895392524311e+002, "790169042548219",    3 },
            { L_, 7.9018472677157297952563255e-148, "790184726771573", -147 },
            { L_, 7.9030767197735093004667206e-291, "790307671977351", -290 },
            { L_, 7.9034047861489903426155072e+172, "790340478614899",  173 },
            { L_, 7.9034199600475798982355714e+245, "790341996004758",  246 },
            { L_, 7.9039944307654899601689783e-035, "790399443076549",  -34 },
            { L_, 7.9058605441191106273479017e-298, "790586054411911", -297 },
            { L_, 7.9069540206359698054332315e+269, "790695402063597",  270 },
            { L_, 7.9074105989905798363063795e+120, "790741059899058",  121 },
            { L_, 7.9094637010939606379531035e+262, "790946370109396",  263 },
            { L_, 7.9108391191787904652839577e-111, "791083911917879", -110 },
            { L_, 7.9120690447966002022760011e+307,  "79120690447966",  308 },
            { L_, 7.9163963109087197491385055e-308, "791639631090872", -307 },
            { L_, 7.9174431187953696391735378e-220, "791744311879537", -219 },
            { L_, 7.9193585176864703125000000e+013, "791935851768647",   14 },
            { L_, 7.9200080067980894731372542e+162, "792000800679809",  163 },
            { L_, 7.9208824754979991940408671e-236,   "7920882475498", -235 },
            { L_, 7.9230506233561898170025220e-253, "792305062335619", -252 },
            { L_, 7.9251559697018696726841019e-098, "792515596970187",  -97 },
            { L_, 7.9257654216282792448496624e+249, "792576542162828",  250 },
            { L_, 7.9285482871060501283877357e-066, "792854828710605",  -65 },
            { L_, 7.9286552328997504768230997e-154, "792865523289975", -153 },
            { L_, 7.9298251441891997470476943e+064,  "79298251441892",   65 },
            { L_, 7.9343022593538996632705792e+303,  "79343022593539",  304 },
            { L_, 7.9358571547003794552891656e-102, "793585715470038", -101 },
            { L_, 7.9370372551833101582019481e-002, "793703725518331",   -1 },
            { L_, 7.9404248000316204059488300e-211, "794042480003162", -210 },
            { L_, 7.9468182674772704018529536e+096, "794681826747727",   97 },
            { L_, 7.9471221184457897215442371e+086, "794712211844579",   87 },
            { L_, 7.9498996699750604767153525e-167, "794989966997506", -166 },
            { L_, 7.9503375810305695862546233e-302, "795033758103057", -301 },
            { L_, 7.9540994881158594375012511e-135, "795409948811586", -134 },
            { L_, 7.9550895089476002341209448e+292,  "79550895089476",  293 },
            { L_, 7.9551910559667800305344691e+034, "795519105596678",   35 },
            { L_, 7.9557335072199998552549008e-166,    "795573350722", -165 },
            { L_, 7.9590859971836600825983618e-280, "795908599718366", -279 },
            { L_, 7.9594805115264198222130730e+118, "795948051152642",  119 },
            { L_, 7.9600105168012200316514537e+226, "796001051680122",  227 },
            { L_, 7.9615192686582207792101567e+171, "796151926865822",  172 },
            { L_, 7.9623436692510399863032189e-273, "796234366925104", -272 },
            { L_, 7.9648884764719703365195841e-246, "796488847647197", -245 },
            { L_, 7.9704305184627300052656253e+067, "797043051846273",   68 },
            { L_, 7.9706449616523098079207778e-199, "797064496165231", -198 },
            { L_, 7.9709309772664600346167949e-099, "797093097726646",  -98 },
            { L_, 7.9719783320543600093935482e-043, "797197833205436",  -42 },
            { L_, 7.9724903873696103373321755e-259, "797249038736961", -258 },
            { L_, 7.9736187688600000000000000e+017,    "797361876886",   18 },
            { L_, 7.9756576307872400145771500e-121, "797565763078724", -120 },
            { L_, 7.9758642078127091947358039e-118, "797586420781271", -117 },
            { L_, 7.9759802623150394800338515e-296, "797598026231504", -295 },
            { L_, 7.9774069617140902022699894e-042, "797740696171409",  -41 },
            { L_, 7.9798690858904006668636219e+230,  "79798690858904",  231 },
            { L_, 7.9811874484712201944002364e-175, "798118744847122", -174 },
            { L_, 7.9823289865186597464158826e-113, "798232898651866", -112 },
            { L_, 7.9829130815170305163113137e+168, "798291308151703",  169 },
            { L_, 7.9829947566318695563017302e-258, "798299475663187", -257 },
            { L_, 7.9852711418761402867492373e-258, "798527114187614", -257 },
            { L_, 7.9893906781173903587955118e+060, "798939067811739",   61 },
            { L_, 7.9895655684520897921174765e+002, "798956556845209",    3 },
            { L_, 7.9902653881638002395629883e+009,  "79902653881638",   10 },
            { L_, 7.9913813924736302371183912e+256, "799138139247363",  257 },
            { L_, 7.9920189416190197602078647e-032, "799201894161902",  -31 },
            { L_, 7.9960698975716099375665927e+073, "799606989757161",   74 },
            { L_, 7.9975260348353297385443466e+295, "799752603483533",  296 },
            { L_, 8.0028363952705803964921521e-163, "800283639527058", -162 },
            { L_, 8.0053559188538203125000000e+012, "800535591885382",   13 },
            { L_, 8.0059329900062698792876174e+141, "800593299000627",  142 },
            { L_, 8.0060445831603806425606743e+194, "800604458316038",  195 },
            { L_, 8.0078066120388898870102872e-197, "800780661203889", -196 },
            { L_, 8.0098233966100698957645828e+048, "800982339661007",   49 },
            { L_, 8.0141117761417198601913604e-014, "801411177614172",  -13 },
            { L_, 8.0154025713790795491111892e-022, "801540257137908",  -21 },
            { L_, 8.0167248614338297039538457e-024, "801672486143383",  -23 },
            { L_, 8.0187916731185898306306329e+287, "801879167311859",  288 },
            { L_, 8.0194281729183901587349298e+032, "801942817291839",   33 },
            { L_, 8.0206683080630495956826867e-053, "802066830806305",  -52 },
            { L_, 8.0208220606480197009448013e-034, "802082206064802",  -33 },
            { L_, 8.0212057009633997639799212e+248,  "80212057009634",  249 },
            { L_, 8.0212556864852900442072127e+049, "802125568648529",   50 },
            { L_, 8.0240143876645196081759879e-309, "802401438766452", -308 },
            { L_, 8.0243234352408204397102648e+282, "802432343524082",  283 },
            { L_, 8.0251840945445002201359855e+259,  "80251840945445",  260 },
            { L_, 8.0293978977206400284788719e-207, "802939789772064", -206 },
            { L_, 8.0305231741932394668820607e-027, "803052317419324",  -26 },
            { L_, 8.0313274552542601387073099e+068, "803132745525426",   69 },
            { L_, 8.0337126432713795080959321e+195, "803371264327138",  196 },
            { L_, 8.0362285695396199893592644e-013, "803622856953962",  -12 },
            { L_, 8.0366707074680096458324465e-117, "803667070746801", -116 },
            { L_, 8.0372362445961098565959453e+139, "803723624459611",  140 },
            { L_, 8.0375937475524501759301575e-055, "803759374755245",  -54 },
            { L_, 8.0416757775915197188646031e+279, "804167577759152",  280 },
            { L_, 8.0422715109524900484123865e-061, "804227151095249",  -60 },
            { L_, 8.0423355999165199338878214e+273, "804233559991652",  274 },
            { L_, 8.0424299711675701791484277e-138, "804242997116757", -137 },
            { L_, 8.0430375655548004150390625e+010,  "80430375655548",   11 },
            { L_, 8.0449267899296204996453160e-287, "804492678992962", -286 },
            { L_, 8.0483528604618296440457895e+164, "804835286046183",  165 },
            { L_, 8.0492359304293697939129715e+042, "804923593042937",   43 },
            { L_, 8.0494314065341296314269959e+128, "804943140653413",  129 },
            { L_, 8.0501911771483899654720393e-046, "805019117714839",  -45 },
            { L_, 8.0521273109010105339368326e+260, "805212731090101",  261 },
            { L_, 8.0523509431947699636091061e+150, "805235094319477",  151 },
            { L_, 8.0557942772449003042201469e+196,  "80557942772449",  197 },
            { L_, 8.0613161381012602206461592e-166, "806131613810126", -165 },
            { L_, 8.0641123068998204544186592e+006, "806411230689982",    7 },
            { L_, 8.0659137186893101625678151e-258, "806591371868931", -257 },
            { L_, 8.0665351730172997594910303e-269,  "80665351730173", -268 },
            { L_, 8.0671475658368104617472169e+198, "806714756583681",  199 },
            { L_, 8.0717129383634597224494536e+031, "807171293836346",   32 },
            { L_, 8.0766171434243400863076980e+048, "807661714342434",   49 },
            { L_, 8.0774156589522801707775900e+275, "807741565895228",  276 },
            { L_, 8.0820503589316400283430581e-072, "808205035893164",  -71 },
            { L_, 8.0831105139184705026404550e-110, "808311051391847", -109 },
            { L_, 8.0835455134003303179345678e+158, "808354551340033",  159 },
            { L_, 8.0842411317922300807671033e+303, "808424113179223",  304 },
            { L_, 8.0850657172102396973846646e-237, "808506571721024", -236 },
            { L_, 8.0884105527338902506262215e-216, "808841055273389", -215 },
            { L_, 8.0924730026483007614189795e+218,  "80924730026483",  219 },
            { L_, 8.0930511581769194975717909e+087, "809305115817692",   88 },
            { L_, 8.0941018589583499306327650e-017, "809410185895835",  -16 },
            { L_, 8.0941714000903600627503259e-222, "809417140009036", -221 },
            { L_, 8.0943928562574898202919427e-296, "809439285625749", -295 },
            { L_, 8.0953850995300705587031828e+207, "809538509953007",  208 },
            { L_, 8.0959895895627895085909998e-163, "809598958956279", -162 },
            { L_, 8.0974717085123698274586748e-073, "809747170851237",  -72 },
            { L_, 8.0980403176931292334860377e+171, "809804031769313",  172 },
            { L_, 8.0985508951201998756139951e+186,  "80985508951202",  187 },
            { L_, 8.0991512498622798242166452e+191, "809915124986228",  192 },
            { L_, 8.1014280541310293024007554e-233, "810142805413103", -232 },
            { L_, 8.1014499641384297439348652e-125, "810144996413843", -124 },
            { L_, 8.1025583706876596772532216e-016, "810255837068766",  -15 },
            { L_, 8.1031904884937295651196785e-307, "810319048849373", -306 },
            { L_, 8.1052043790078097003786454e+117, "810520437900781",  118 },
            { L_, 8.1052492938765602021491402e-162, "810524929387656", -161 },
            { L_, 8.1055914704888502517515661e-130, "810559147048885", -129 },
            { L_, 8.1057634486789097324668091e+046, "810576344867891",   47 },
            { L_, 8.1058954283935094494566810e+147, "810589542839351",  148 },
            { L_, 8.1067055951075096395411773e-063, "810670559510751",  -62 },
            { L_, 8.1086259384785903251498636e-224, "810862593847859", -223 },
            { L_, 8.1161316123651600578222857e-263, "811613161236516", -262 },
            { L_, 8.1173466860353695136244548e-215, "811734668603537", -214 },
            { L_, 8.1191313186820598172421093e-102, "811913131868206", -101 },
            { L_, 8.1193801456526002672392517e-005,  "81193801456526",   -4 },
            { L_, 8.1205022656023800444116331e-271, "812050226560238", -270 },
            { L_, 8.1233583853506900113752497e+052, "812335838535069",   53 },
            { L_, 8.1255012003072900343975236e+291, "812550120030729",  292 },
            { L_, 8.1271741769891398819047294e+169, "812717417698914",  170 },
            { L_, 8.1275180894681398058310151e-001, "812751808946814",    0 },
            { L_, 8.1291246604676802349397645e+213, "812912466046768",  214 },
            { L_, 8.1291256674990201012879308e-157, "812912566749902", -156 },
            { L_, 8.1295797020467894681600000e+020, "812957970204679",   21 },
            { L_, 8.1298433786333899605718133e-299, "812984337863339", -298 },
            { L_, 8.1346180757336099380500508e+190, "813461807573361",  191 },
            { L_, 8.1381732207252904412566298e-172, "813817322072529", -171 },
            { L_, 8.1396606992456797679529135e-182, "813966069924568", -181 },
            { L_, 8.1418094311335904729572230e+061, "814180943113359",   62 },
            { L_, 8.1425263054805506384154033e-077, "814252630548055",  -76 },
            { L_, 8.1431642651554901867585589e+225, "814316426515549",  226 },
            { L_, 8.1450862809323001773643732e-164,  "81450862809323", -163 },
            { L_, 8.1478526159665901637616610e+166, "814785261596659",  167 },
            { L_, 8.1490237884212292515826994e+277, "814902378842123",  278 },
            { L_, 8.1496861464100996242984566e+233,  "81496861464101",  234 },
            { L_, 8.1541660485605003583216164e+218,  "81541660485605",  219 },
            { L_, 8.1552734510701798419819756e-164, "815527345107018", -163 },
            { L_, 8.1553873913175301178260439e-077, "815538739131753",  -76 },
            { L_, 8.1556973324384697420617611e+212, "815569733243847",  213 },
            { L_, 8.1604263674346299070458321e+264, "816042636743463",  265 },
            { L_, 8.1613386457629999753019983e-279,   "8161338645763", -278 },
            { L_, 8.1622610294964805205522747e+095, "816226102949648",   96 },
            { L_, 8.1624890762262995093847235e+109,  "81624890762263",  110 },
            { L_, 8.1640409777930995616777104e-053,  "81640409777931",  -52 },
            { L_, 8.1655621127715596027899140e-306, "816556211277156", -305 },
            { L_, 8.1661143894562096211302924e-056, "816611438945621",  -55 },
            { L_, 8.1663819803570806548346378e-027, "816638198035708",  -26 },
            { L_, 8.1676320549240192000000000e+016, "816763205492402",   17 },
            { L_, 8.1701071383935700225915764e-175, "817010713839357", -174 },
            { L_, 8.1708006515787999933855218e+074,  "81708006515788",   75 },
            { L_, 8.1722679180572701626663376e-018, "817226791805727",  -17 },
            { L_, 8.1757275936514704428971710e+102, "817572759365147",  103 },
            { L_, 8.1757941375228301927747315e+095, "817579413752283",   96 },
            { L_, 8.1759704407169201763145554e+104, "817597044071692",  105 },
            { L_, 8.1808243403350805788717826e+143, "818082434033508",  144 },
            { L_, 8.1820295395994397812605658e+108, "818202953959944",  109 },
            { L_, 8.1825207559171595885805236e-168, "818252075591716", -167 },
            { L_, 8.1830705953180304938340317e+033, "818307059531803",   34 },
            { L_, 8.1835345628852803145470448e-223, "818353456288528", -222 },
            { L_, 8.1837526496507707019372259e-012, "818375264965077",  -11 },
            { L_, 8.1837724369149193500597231e-120, "818377243691492", -119 },
            { L_, 8.1877178538568701494260055e+288, "818771785385687",  289 },
            { L_, 8.1899448348429495568147776e-066, "818994483484295",  -65 },
            { L_, 8.1908879918754694053040131e+107, "819088799187547",  108 },
            { L_, 8.1923328903884102952787567e-143, "819233289038841", -142 },
            { L_, 8.1925332759114203578069898e-247, "819253327591142", -246 },
            { L_, 8.1955258015576094749142507e-169, "819552580155761", -168 },
            { L_, 8.1958096523259103827196981e-003, "819580965232591",   -2 },
            { L_, 8.1979360960451605844812834e-020, "819793609604516",  -19 },
            { L_, 8.1980794624769303609263226e+128, "819807946247693",  129 },
            { L_, 8.2006499671498002816907284e+201,  "82006499671498",  202 },
            { L_, 8.2022466200217893827517722e+135, "820224662002179",  136 },
            { L_, 8.2034555791452304010288458e-078, "820345557914523",  -77 },
            { L_, 8.2057461088089394450927812e-136, "820574610880894", -135 },
            { L_, 8.2069717734904698665172652e-262, "820697177349047", -261 },
            { L_, 8.2076488776524403826669949e-019, "820764887765244",  -18 },
            { L_, 8.2082601341334900402951766e-169, "820826013413349", -168 },
            { L_, 8.2084182839343806612570831e-052, "820841828393438",  -51 },
            { L_, 8.2133743377226897663945238e-001, "821337433772269",    0 },
            { L_, 8.2181886569413599789351913e+128, "821818865694136",  129 },
            { L_, 8.2212151451046201745749105e-293, "822121514510462", -292 },
            { L_, 8.2245530401358995586633682e+006,  "82245530401359",    7 },
            { L_, 8.2245878486966900023645129e-168, "822458784869669", -167 },
            { L_, 8.2247404988970296066650051e+168, "822474049889703",  169 },
            { L_, 8.2286567075864392867950571e-102, "822865670758644", -101 },
            { L_, 8.2295399967495198216974773e-224, "822953999674952", -223 },
            { L_, 8.2309374162543503779319181e+125, "823093741625435",  126 },
            { L_, 8.2347838764532193988808220e+042, "823478387645322",   43 },
            { L_, 8.2352162551656804403408678e-019, "823521625516568",  -18 },
            { L_, 8.2396260235488501209788527e-112, "823962602354885", -111 },
            { L_, 8.2401258085210698332797208e-294, "824012580852107", -293 },
            { L_, 8.2409410292892303391542344e-125, "824094102928923", -124 },
            { L_, 8.2428139269484900881919656e-122, "824281392694849", -121 },
            { L_, 8.2432050113449002683213919e-025,  "82432050113449",  -24 },
            { L_, 8.2459558786956696899741076e-113, "824595587869567", -112 },
            { L_, 8.2484054463115001338770897e+058,  "82484054463115",   59 },
            { L_, 8.2494065666861699733877206e+132, "824940656668617",  133 },
            { L_, 8.2505093844109697038931816e+109, "825050938441097",  110 },
            { L_, 8.2521920117373196469861795e+189, "825219201173732",  190 },
            { L_, 8.2525839134069400920916212e-274, "825258391340694", -273 },
            { L_, 8.2526425481831002165479818e+266,  "82526425481831",  267 },
            { L_, 8.2527187200488605361612374e-188, "825271872004886", -187 },
            { L_, 8.2531193526553200000000000e+015, "825311935265532",   16 },
            { L_, 8.2553912829478098258622111e+242, "825539128294781",  243 },
            { L_, 8.2582914724739199514478753e+287, "825829147247392",  288 },
            { L_, 8.2583986213210198890054713e+189, "825839862132102",  190 },
            { L_, 8.2637803176704499482976432e+055, "826378031767045",   56 },
            { L_, 8.2655268557352696610143457e+051, "826552685573527",   52 },
            { L_, 8.2659127154882293574427543e-241, "826591271548823", -240 },
            { L_, 8.2674632979341505257670753e+078, "826746329793415",   79 },
            { L_, 8.2677593221613403316513433e+270, "826775932216134",  271 },
            { L_, 8.2694396429791802815739284e-240, "826943964297918", -239 },
            { L_, 8.2712773958751697253904097e-201, "827127739587517", -200 },
            { L_, 8.2724864442616296249267888e+176, "827248644426163",  177 },
            { L_, 8.2725410127092398442131651e+259, "827254101270924",  260 },
            { L_, 8.2761503075045100801222949e-126, "827615030750451", -125 },
            { L_, 8.2763244706157299656667909e-102, "827632447061573", -101 },
            { L_, 8.2769303177525794295520067e+076, "827693031775258",   77 },
            { L_, 8.2779393804879501777593253e-122, "827793938048795", -121 },
            { L_, 8.2781728906835201896070055e-143, "827817289068352", -142 },
            { L_, 8.2797694687410003203435286e-203,   "8279769468741", -202 },
            { L_, 8.2804927539376003083175384e-240,  "82804927539376", -239 },
            { L_, 8.2832319394868697904553002e-016, "828323193948687",  -15 },
            { L_, 8.2834261446289098254718837e+256, "828342614462891",  257 },
            { L_, 8.2873222474448903469182814e+214, "828732224744489",  215 },
            { L_, 8.2877558973580594642944000e+022, "828775589735806",   23 },
            { L_, 8.2877981914054495906829607e-120, "828779819140545", -119 },
            { L_, 8.2924077492632396307975835e-035, "829240774926324",  -34 },
            { L_, 8.2963219252060503659681880e+179, "829632192520605",  180 },
            { L_, 8.2967715384644601164909739e-108, "829677153846446", -107 },
            { L_, 8.2980915772914706716420737e-229, "829809157729147", -228 },
            { L_, 8.2982175557813599704202682e+077, "829821755578136",   78 },
            { L_, 8.3016163757104402516685235e-063, "830161637571044",  -62 },
            { L_, 8.3020108582917997435289600e+023,  "83020108582918",   24 },
            { L_, 8.3038957517680300913770909e+047, "830389575176803",   48 },
            { L_, 8.3042627482719001903930443e-245,  "83042627482719", -244 },
            { L_, 8.3042985629033405185988296e-029, "830429856290334",  -28 },
            { L_, 8.3051918746616204650601057e+072, "830519187466162",   73 },
            { L_, 8.3062220889075801433775946e-213, "830622208890758", -212 },
            { L_, 8.3071858074718995219048857e+284,  "83071858074719",  285 },
            { L_, 8.3081786583529298985773133e+237, "830817865835293",  238 },
            { L_, 8.3127151885122200994394222e-215, "831271518851222", -214 },
            { L_, 8.3192678474913206969438421e+255, "831926784749132",  256 },
            { L_, 8.3193028017009096056803737e-169, "831930280170091", -168 },
            { L_, 8.3210721520534399695138544e+133, "832107215205344",  134 },
            { L_, 8.3255481913198203063606446e-200, "832554819131982", -199 },
            { L_, 8.3259047206849201455240851e+225, "832590472068492",  226 },
            { L_, 8.3264556009988792724937303e-289, "832645560099888", -288 },
            { L_, 8.3264885972247895662562127e-281, "832648859722479", -280 },
            { L_, 8.3272634150222607980164558e+199, "832726341502226",  200 },
            { L_, 8.3272982955165401473053364e+242, "832729829551654",  243 },
            { L_, 8.3306381428188699586711279e-243, "833063814281887", -242 },
            { L_, 8.3320026287551004266642785e+099,  "83320026287551",  100 },
            { L_, 8.3321369159241802762636048e+270, "833213691592418",  271 },
            { L_, 8.3327325300735804405691806e-187, "833273253007358", -186 },
            { L_, 8.3353784729823606531656969e-177, "833537847298236", -176 },
            { L_, 8.3356841263163099244758878e+282, "833568412631631",  283 },
            { L_, 8.3397669535684197465103829e-265, "833976695356842", -264 },
            { L_, 8.3441738192145695231998698e+163, "834417381921457",  164 },
            { L_, 8.3455032006391995502252369e-085,  "83455032006392",  -84 },
            { L_, 8.3463575969021091870712423e-084, "834635759690211",  -83 },
            { L_, 8.3468158601445305347442627e+008, "834681586014453",    9 },
            { L_, 8.3476909663898098053978833e+244, "834769096638981",  245 },
            { L_, 8.3477494086675999884212065e+068,  "83477494086676",   69 },
            { L_, 8.3478154449123704961099233e-082, "834781544491237",  -81 },
            { L_, 8.3498550253514099902447947e-017, "834985502535141",  -16 },
            { L_, 8.3511542891287897678427433e-283, "835115428912879", -282 },
            { L_, 8.3557281618251798117497331e-037, "835572816182518",  -36 },
            { L_, 8.3567417472200994525069252e-260,  "83567417472201", -259 },
            { L_, 8.3580439335202295678152260e-225, "835804393352023", -224 },
            { L_, 8.3585928426182099903324341e-004, "835859284261821",   -3 },
            { L_, 8.3587002412594398532078076e+295, "835870024125944",  296 },
            { L_, 8.3619860984581304549728464e-284, "836198609845813", -283 },
            { L_, 8.3623266830513502797500693e-259, "836232668305135", -258 },
            { L_, 8.3637880659058393113912633e+208, "836378806590584",  209 },
            { L_, 8.3664097294634099317480845e-224, "836640972946341", -223 },
            { L_, 8.3687891349340500016808541e-275, "836878913493405", -274 },
            { L_, 8.3695834779067897369456728e-069, "836958347790679",  -68 },
            { L_, 8.3699173147884097065182741e+111, "836991731478841",  112 },
            { L_, 8.3723116102084899995353960e-225, "837231161020849", -224 },
            { L_, 8.3725618212724300954249774e+194, "837256182127243",  195 },
            { L_, 8.3726796571320996639963143e-066,  "83726796571321",  -65 },
            { L_, 8.3740809313402398210422095e-078, "837408093134024",  -77 },
            { L_, 8.3741747834718495771195095e+304, "837417478347185",  305 },
            { L_, 8.3748078602680197810700779e-113, "837480786026802", -112 },
            { L_, 8.3752092508190703765571471e-102, "837520925081907", -101 },
            { L_, 8.3776420581961696896768728e+292, "837764205819617",  293 },
            { L_, 8.3812061384227195486431604e-066, "838120613842272",  -65 },
            { L_, 8.3831138824142496122128217e+266, "838311388241425",  267 },
            { L_, 8.3836185595794397237235269e-123, "838361855957944", -122 },
            { L_, 8.3861841283593992965129022e+109,  "83861841283594",  110 },
            { L_, 8.3909117850674895258511386e+228, "839091178506749",  229 },
            { L_, 8.3925558352711303899214898e-112, "839255583527113", -111 },
            { L_, 8.3970396132495299351252042e+304, "839703961324953",  305 },
            { L_, 8.3971692168773004615432064e-059,  "83971692168773",  -58 },
            { L_, 8.4009409245442204173814331e+193, "840094092454422",  194 },
            { L_, 8.4050754942881795465274282e+127, "840507549428818",  128 },
            { L_, 8.4061675188158198895364005e+144, "840616751881582",  145 },
            { L_, 8.4074327325563898522203729e+180, "840743273255639",  181 },
            { L_, 8.4075950430651207188706511e+162, "840759504306512",  163 },
            { L_, 8.4102406748771396139637147e-110, "841024067487714", -109 },
            { L_, 8.4128189316261796844569183e+064, "841281893162618",   65 },
            { L_, 8.4146118974677691637609200e-003, "841461189746777",   -2 },
            { L_, 8.4148331390721796088183678e+249, "841483313907218",  250 },
            { L_, 8.4157096333395595979547626e-279, "841570963333956", -278 },
            { L_, 8.4173266364455405336545818e+277, "841732663644554",  278 },
            { L_, 8.4178431150681305068244753e+142, "841784311506813",  143 },
            { L_, 8.4213077675101707937509514e-093, "842130776751017",  -92 },
            { L_, 8.4241849536756707580695339e-230, "842418495367567", -229 },
            { L_, 8.4264921169486498832702637e+008, "842649211694865",    9 },
            { L_, 8.4272623158538606523225819e-223, "842726231585386", -222 },
            { L_, 8.4275912242723000968258385e-153,  "84275912242723", -152 },
            { L_, 8.4281411859742304141536395e+029, "842814118597423",   30 },
            { L_, 8.4299415579831097819630866e-178, "842994155798311", -177 },
            { L_, 8.4315344192212706518518061e+143, "843153441922127",  144 },
            { L_, 8.4340649289657005947784634e-188,  "84340649289657", -187 },
            { L_, 8.4360421371273405183852652e-164, "843604213712734", -163 },
            { L_, 8.4388479425283695840895676e-239, "843884794252837", -238 },
            { L_, 8.4412155548954002908904315e+175,  "84412155548954",  176 },
            { L_, 8.4427936210242995336955540e+154,  "84427936210243",  155 },
            { L_, 8.4438499498279596406722095e-205, "844384994982796", -204 },
            { L_, 8.4448909410979701965858536e-255, "844489094109797", -254 },
            { L_, 8.4458667518064898116527905e+040, "844586675180649",   41 },
            { L_, 8.4539320919475902627108532e-080, "845393209194759",  -79 },
            { L_, 8.4549442390727501314059874e-227, "845494423907275", -226 },
            { L_, 8.4558180246379399249395854e+100, "845581802463794",  101 },
            { L_, 8.4565326145712704914142810e-080, "845653261457127",  -79 },
            { L_, 8.4580550842006202837118682e+136, "845805508420062",  137 },
            { L_, 8.4592482320832199790385394e+081, "845924823208322",   82 },
            { L_, 8.4595945612161197245793584e-246, "845959456121612", -245 },
            { L_, 8.4606449108092397593130184e+227, "846064491080924",  228 },
            { L_, 8.4642934034635996604987910e+182,  "84642934034636",  183 },
            { L_, 8.4654570090259398929521226e-226, "846545700902594", -225 },
            { L_, 8.4663777035455994416828609e-211,  "84663777035456", -210 },
            { L_, 8.4690752318743099716340873e+201, "846907523187431",  202 },
            { L_, 8.4696315409114306164523351e-195, "846963154091143", -194 },
            { L_, 8.4718255774664196172098449e+057, "847182557746642",   58 },
            { L_, 8.4721628485958695562964479e+257, "847216284859587",  258 },
            { L_, 8.4722475780243902422399043e-186, "847224757802439", -185 },
            { L_, 8.4725720518289298320223247e+224, "847257205182893",  225 },
            { L_, 8.4729863697927798914186511e+207, "847298636979278",  208 },
            { L_, 8.4736871038850900808039946e+173, "847368710388509",  174 },
            { L_, 8.4742266088941402504362178e-187, "847422660889414", -186 },
            { L_, 8.4771005235580804562774480e-137, "847710052355808", -136 },
            { L_, 8.4796684864922096000175531e+158, "847966848649221",  159 },
            { L_, 8.4807941945150904863917382e-188, "848079419451509", -187 },
            { L_, 8.4810752459200496604287707e+105, "848107524592005",  106 },
            { L_, 8.4816158318228000644675648e-052,  "84816158318228",  -51 },
            { L_, 8.4858269583782504348990046e-211, "848582695837825", -210 },
            { L_, 8.4859496313831897576964978e+088, "848594963138319",   89 },
            { L_, 8.4891304096975400135973349e+060, "848913040969754",   61 },
            { L_, 8.4897938948854400536919754e-198, "848979389488544", -197 },
            { L_, 8.4906269333460203948617302e-054, "849062693334602",  -53 },
            { L_, 8.4907739457503306145223799e+038, "849077394575033",   39 },
            { L_, 8.4908745289954702561186177e+198, "849087452899547",  199 },
            { L_, 8.4910178020868701475340335e-274, "849101780208687", -273 },
            { L_, 8.4935514314418396708982805e+306, "849355143144184",  307 },
            { L_, 8.4942945592910804250222448e+234, "849429455929108",  235 },
            { L_, 8.4962827238760706323332828e-161, "849628272387607", -160 },
            { L_, 8.4977511222446203094007832e+194, "849775112224462",  195 },
            { L_, 8.4996196519712394380826806e-166, "849961965197124", -165 },
            { L_, 8.4996417218306204694341537e-146, "849964172183062", -145 },
            { L_, 8.5009325890232200097880722e+139, "850093258902322",  140 },
            { L_, 8.5013478817095494456005154e-030, "850134788170955",  -29 },
            { L_, 8.5018732811528807742445411e-270, "850187328115288", -269 },
            { L_, 8.5046274019638299410753318e-050, "850462740196383",  -49 },
            { L_, 8.5048557215164101171718937e-186, "850485572151641", -185 },
            { L_, 8.5071381343436502049204632e+281, "850713813434365",  282 },
            { L_, 8.5079176366213693451744051e+026, "850791763662137",   27 },
            { L_, 8.5084891420066196435042111e+142, "850848914200662",  143 },
            { L_, 8.5092656700656303125543065e-190, "850926567006563", -189 },
            { L_, 8.5095769332969804755149867e-005, "850957693329698",   -4 },
            { L_, 8.5119823799848504483061793e+071, "851198237998485",   72 },
            { L_, 8.5130274234053500116470934e+210, "851302742340535",  211 },
            { L_, 8.5131785081750805919566052e-160, "851317850817508", -159 },
            { L_, 8.5160945390004898851657242e+106, "851609453900049",  107 },
            { L_, 8.5163001547318500898835524e+141, "851630015473185",  142 },
            { L_, 8.5182757408880705354024039e-143, "851827574088807", -142 },
            { L_, 8.5210091360292004996335694e-176,  "85210091360292", -175 },
            { L_, 8.5215509550041803591193771e-146, "852155095500418", -145 },
            { L_, 8.5224260800969103970101826e-303, "852242608009691", -302 },
            { L_, 8.5252623523475198191587578e-206, "852526235234752", -205 },
            { L_, 8.5262335589335605139899309e+228, "852623355893356",  229 },
            { L_, 8.5334338187803308100416798e+124, "853343381878033",  125 },
            { L_, 8.5335178518294799109855024e+086, "853351785182948",   87 },
            { L_, 8.5395475119209804256232621e+072, "853954751192098",   73 },
            { L_, 8.5410429687339805561075894e-201, "854104296873398", -200 },
            { L_, 8.5455853196131196990891424e-120, "854558531961312", -119 },
            { L_, 8.5482338653580604246174687e+095, "854823386535806",   96 },
            { L_, 8.5503080812776404678369662e+277, "855030808127764",  278 },
            { L_, 8.5504927176266703125000000e+013, "855049271762667",   14 },
            { L_, 8.5505942326946793484607183e-126, "855059423269468", -125 },
            { L_, 8.5512227163997304679424832e-279, "855122271639973", -278 },
            { L_, 8.5515657357141502858634320e-288, "855156573571415", -287 },
            { L_, 8.5536177330664403594079116e+137, "855361773306644",  138 },
            { L_, 8.5544587932275903064112661e-070, "855445879322759",  -69 },
            { L_, 8.5602974709203500189020403e+083, "856029747092035",   84 },
            { L_, 8.5632818061062306762296986e+261, "856328180610623",  262 },
            { L_, 8.5652332482776006970649656e+072,  "85652332482776",   73 },
            { L_, 8.5672985986009703687957782e-218, "856729859860097", -217 },
            { L_, 8.5724886738264496500524881e+074, "857248867382645",   75 },
            { L_, 8.5731306671861199592272109e+177, "857313066718612",  178 },
            { L_, 8.5736422023121903166112355e-172, "857364220231219", -171 },
            { L_, 8.5748447813170305386136488e+237, "857484478131703",  238 },
            { L_, 8.5779247162784299410146915e-215, "857792471627843", -214 },
            { L_, 8.5799355703729502616482245e-178, "857993557037295", -177 },
            { L_, 8.5820613619786794028257229e+038, "858206136197868",   39 },
            { L_, 8.5829055139570807006452522e-199, "858290551395708", -198 },
            { L_, 8.5834694074450601419812301e+002, "858346940744506",    3 },
            { L_, 8.5838119333216701476750056e-260, "858381193332167", -259 },
            { L_, 8.5855140771240898768524567e-203, "858551407712409", -202 },
            { L_, 8.5856948193432793201076107e-242, "858569481934328", -241 },
            { L_, 8.5863054568168594755185850e-169, "858630545681686", -168 },
            { L_, 8.5875473380975991800987086e+264,  "85875473380976",  265 },
            { L_, 8.5909668545338702249790357e-169, "859096685453387", -168 },
            { L_, 8.5932787368832696229219437e+006, "859327873688327",    7 },
            { L_, 8.5939133503126798699863929e-162, "859391335031268", -161 },
            { L_, 8.5946963679929700487612352e-031, "859469636799297",  -30 },
            { L_, 8.5953764553427405321067456e-284, "859537645534274", -283 },
            { L_, 8.5975776627388804839243776e+025, "859757766273888",   26 },
            { L_, 8.5982273027653692606244703e+137, "859822730276537",  138 },
            { L_, 8.5986049045759800671731908e-198, "859860490457598", -197 },
            { L_, 8.5992795968059805923803279e-254, "859927959680598", -253 },
            { L_, 8.6081405094084803354617907e+083, "860814050940848",   84 },
            { L_, 8.6103402647693200703377689e+165, "861034026476932",  166 },
            { L_, 8.6110995522082993920267942e+087,  "86110995522083",   88 },
            { L_, 8.6131644885148898429747761e-300, "861316448851489", -299 },
            { L_, 8.6154479082864903875213335e+057, "861544790828649",   58 },
            { L_, 8.6159815961891503831484857e-218, "861598159618915", -217 },
            { L_, 8.6170054251245504044221376e+048, "861700542512455",   49 },
            { L_, 8.6180197703581295318079109e+090, "861801977035813",   91 },
            { L_, 8.6182553087583003936071249e-015,  "86182553087583",  -14 },
            { L_, 8.6191998263505700552556322e+197, "861919982635057",  198 },
            { L_, 8.6209256075667399105109686e+111, "862092560756674",  112 },
            { L_, 8.6212566592701597446209895e+225, "862125665927016",  226 },
            { L_, 8.6262001915202799654666240e+024, "862620019152028",   25 },
            { L_, 8.6301676124905106158871347e-099, "863016761249051",  -98 },
            { L_, 8.6308771257626202443877749e-069, "863087712576262",  -68 },
            { L_, 8.6309584053157996139308326e-037,  "86309584053158",  -36 },
            { L_, 8.6319385059648303194152004e-254, "863193850596483", -253 },
            { L_, 8.6323529502492698993188266e-041, "863235295024927",  -40 },
            { L_, 8.6355475396146194989922719e-230, "863554753961462", -229 },
            { L_, 8.6366494149050003826757625e+180,   "8636649414905",  181 },
            { L_, 8.6368680735965301370460046e-195, "863686807359653", -194 },
            { L_, 8.6378028561066199967011454e+238, "863780285610662",  239 },
            { L_, 8.6383578008465099207224824e-003, "863835780084651",   -2 },
            { L_, 8.6385542435515196159921525e-065, "863855424355152",  -64 },
            { L_, 8.6396519601307403553496986e-226, "863965196013074", -225 },
            { L_, 8.6405340947678398844160009e-057, "864053409476784",  -56 },
            { L_, 8.6408866974583604246818728e-082, "864088669745836",  -81 },
            { L_, 8.6414719027579693507739212e-008, "864147190275797",   -7 },
            { L_, 8.6437469477218305565248627e-115, "864374694772183", -114 },
            { L_, 8.6438777517643590958595382e+124, "864387775176436",  125 },
            { L_, 8.6441514690594797417055058e+129, "864415146905948",  130 },
            { L_, 8.6483272214813403248652823e+220, "864832722148134",  221 },
            { L_, 8.6487254650215793727463402e-067, "864872546502158",  -66 },
            { L_, 8.6517825055754292551698543e+128, "865178250557543",  129 },
            { L_, 8.6521434759256596728133918e+140, "865214347592566",  141 },
            { L_, 8.6521485237742596265690271e+158, "865214852377426",  159 },
            { L_, 8.6521575093329792998874124e+087, "865215750933298",   88 },
            { L_, 8.6525470260516399691212239e-244, "865254702605164", -243 },
            { L_, 8.6534072384456695164963871e+071, "865340723844567",   72 },
            { L_, 8.6542905652868297019631532e-183, "865429056528683", -182 },
            { L_, 8.6549836208961808113529736e-034, "865498362089618",  -33 },
            { L_, 8.6597286003567895579198340e+232, "865972860035679",  233 },
            { L_, 8.6600971370541900130583057e+102, "866009713705419",  103 },
            { L_, 8.6607580267515403061711881e+294, "866075802675154",  295 },
            { L_, 8.6615817613099701528166400e+023, "866158176130997",   24 },
            { L_, 8.6621647968791696949965864e+057, "866216479687917",   58 },
            { L_, 8.6644367174890905422412755e+243, "866443671748909",  244 },
            { L_, 8.6646495853831295869638353e+040, "866464958538313",   41 },
            { L_, 8.6671129669288400065846212e-167, "866711296692884", -166 },
            { L_, 8.6679936659878006333481674e+048,  "86679936659878",   49 },
            { L_, 8.6682381535643799442710561e+037, "866823815356438",   38 },
            { L_, 8.6710757981938296919041883e-054, "867107579819383",  -53 },
            { L_, 8.6725803597606003755585946e-041,  "86725803597606",  -40 },
            { L_, 8.6728363177159497388371539e+204, "867283631771595",  205 },
            { L_, 8.6734296280521105583688366e-046, "867342962805211",  -45 },
            { L_, 8.6743306650430596946413200e-204, "867433066504306", -203 },
            { L_, 8.6766107839968097264875168e-154, "867661078399681", -153 },
            { L_, 8.6788912306991497163744071e-031, "867889123069915",  -30 },
            { L_, 8.6812676552320296154048918e+205, "868126765523203",  206 },
            { L_, 8.6817365624666195733809080e+257, "868173656246662",  258 },
            { L_, 8.6830545205226901046874867e-172, "868305452052269", -171 },
            { L_, 8.6853509703734104320024703e-151, "868535097037341", -150 },
            { L_, 8.6867142867428104011727122e+187, "868671428674281",  188 },
            { L_, 8.6874464106048300530380079e+282, "868744641060483",  283 },
            { L_, 8.6884046134358891870768977e-155, "868840461343589", -154 },
            { L_, 8.6898753433735301326039626e-072, "868987534337353",  -71 },
            { L_, 8.6912206163768500273548135e-050, "869122061637685",  -49 },
            { L_, 8.6963330688189501592807865e-023, "869633306881895",  -22 },
            { L_, 8.6975050187576197059977248e+198, "869750501875762",  199 },
            { L_, 8.7022264456630599009718683e-081, "870222644566306",  -80 },
            { L_, 8.7101467157838496129386843e+250, "871014671578385",  251 },
            { L_, 8.7107528510175199293960682e-051, "871075285101752",  -50 },
            { L_, 8.7118562070767792496900494e+103, "871185620707678",  104 },
            { L_, 8.7120256925863606839637273e+230, "871202569258636",  231 },
            { L_, 8.7129084649554194771531567e+292, "871290846495542",  293 },
            { L_, 8.7136331953460697615945431e+207, "871363319534607",  208 },
            { L_, 8.7152488762690499361125159e-151, "871524887626905", -150 },
            { L_, 8.7155049929876095603484162e-215, "871550499298761", -214 },
            { L_, 8.7202565927682097742509452e+107, "872025659276821",  108 },
            { L_, 8.7217958549707807357040585e-264, "872179585497078", -263 },
            { L_, 8.7218996481136096263889212e-235, "872189964811361", -234 },
            { L_, 8.7221048504009997701518018e+164,   "8722104850401",  165 },
            { L_, 8.7230347737154303973586447e-104, "872303477371543", -103 },
            { L_, 8.7231960947269208074449405e-112, "872319609472692", -111 },
            { L_, 8.7233145734730597594487434e-218, "872331457347306", -217 },
            { L_, 8.7249819830739404460484054e+090, "872498198307394",   91 },
            { L_, 8.7252028755713201017744626e-214, "872520287557132", -213 },
            { L_, 8.7280937314256501287865292e-091, "872809373142565",  -90 },
            { L_, 8.7332822211944297683837511e+222, "873328222119443",  223 },
            { L_, 8.7333313567929495715840000e+024, "873333135679295",   25 },
            { L_, 8.7333480751871595308621537e-250, "873334807518716", -249 },
            { L_, 8.7365386073012401720970210e-185, "873653860730124", -184 },
            { L_, 8.7375760679738698893415131e+158, "873757606797387",  159 },
            { L_, 8.7392814660789095681511135e+138, "873928146607891",  139 },
            { L_, 8.7409026868383796104385802e-147, "874090268683838", -146 },
            { L_, 8.7420391458281205412871226e+126, "874203914582812",  127 },
            { L_, 8.7431952344744694473059151e+259, "874319523447447",  260 },
            { L_, 8.7457097865219099363078111e-084, "874570978652191",  -83 },
            { L_, 8.7462457845545696742420651e+166, "874624578455457",  167 },
            { L_, 8.7485877503562203755450498e+265, "874858775035622",  266 },
            { L_, 8.7493067717039201977019651e-284, "874930677170392", -283 },
            { L_, 8.7506281396459398314149973e+188, "875062813964594",  189 },
            { L_, 8.7517996770966200971349859e+051, "875179967709662",   52 },
            { L_, 8.7528036887672892267331253e-034, "875280368876729",  -33 },
            { L_, 8.7544038130870403429166670e-278, "875440381308704", -277 },
            { L_, 8.7546474282812802131691100e-055, "875464742828128",  -54 },
            { L_, 8.7551387880897698593816881e-272, "875513878808977", -271 },
            { L_, 8.7582911706864002851421583e-190,  "87582911706864", -189 },
            { L_, 8.7584792193842401836758845e-038, "875847921938424",  -37 },
            { L_, 8.7586465794541599846589654e+071, "875864657945416",   72 },
            { L_, 8.7594383813119407083901367e+218, "875943838131194",  219 },
            { L_, 8.7603661255274095997011732e+174, "876036612552741",  175 },
            { L_, 8.7604963034344104702833254e+094, "876049630343441",   95 },
            { L_, 8.7607321168454205669504924e+157, "876073211684542",  158 },
            { L_, 8.7687695323132800194738869e+205, "876876953231328",  206 },
            { L_, 8.7701054795172591656281652e-289, "877010547951726", -288 },
            { L_, 8.7706859162181996230423660e-230,  "87706859162182", -229 },
            { L_, 8.7728121248066201171875000e+012, "877281212480662",   13 },
            { L_, 8.7734060543049402955618332e+049, "877340605430494",   50 },
            { L_, 8.7734392176761404616916700e-027, "877343921767614",  -26 },
            { L_, 8.7752526516516898915508556e-008, "877525265165169",   -7 },
            { L_, 8.7770828314879493224872812e-155, "877708283148795", -154 },
            { L_, 8.7782110635637793605675274e+184, "877821106356378",  185 },
            { L_, 8.7796297369632700917759794e-159, "877962973696327", -158 },
            { L_, 8.7823930819826308310047949e+112, "878239308198263",  113 },
            { L_, 8.7836513168298296764043305e-299, "878365131682983", -298 },
            { L_, 8.7864668791223199356756538e+089, "878646687912232",   90 },
            { L_, 8.7877001764058793690174530e+090, "878770017640588",   91 },
            { L_, 8.7905828827254204954782282e-150, "879058288272542", -149 },
            { L_, 8.7912202289978102577547436e-272, "879122022899781", -271 },
            { L_, 8.7953860694117300123931748e-120, "879538606941173", -119 },
            { L_, 8.7960878806445104956740521e+116, "879608788064451",  117 },
            { L_, 8.7963439627359301950504209e+288, "879634396273593",  289 },
            { L_, 8.7978450097147003351772922e+182,  "87978450097147",  183 },
            { L_, 8.7978725734015501999062302e+058, "879787257340155",   59 },
            { L_, 8.8008305839826298703545150e-244, "880083058398263", -243 },
            { L_, 8.8016332989760806075756547e+093, "880163329897608",   94 },
            { L_, 8.8025886436623497168030827e-108, "880258864366235", -107 },
            { L_, 8.8041096617460294465261352e+211, "880410966174603",  212 },
            { L_, 8.8053422636794895608058804e+207, "880534226367949",  208 },
            { L_, 8.8061697541690299127217171e-009, "880616975416903",   -8 },
            { L_, 8.8073726227362004960002065e-272,  "88073726227362", -271 },
            { L_, 8.8082015328720101389828239e-149, "880820153287201", -148 },
            { L_, 8.8110672138589902181757423e+054, "881106721385899",   55 },
            { L_, 8.8124969102147303057611972e+099, "881249691021473",  100 },
            { L_, 8.8130938241436692972032527e+243, "881309382414367",  244 },
            { L_, 8.8134546042213702519343887e-186, "881345460422137", -185 },
            { L_, 8.8144068443454107782979603e+137, "881440684434541",  138 },
            { L_, 8.8170900587641907908393003e-078, "881709005876419",  -77 },
            { L_, 8.8172628997267498027297529e-160, "881726289972675", -159 },
            { L_, 8.8191687551375601288529581e-010, "881916875513756",   -9 },
            { L_, 8.8199210960684107470003629e+156, "881992109606841",  157 },
            { L_, 8.8210325877498298159239088e+042, "882103258774983",   43 },
            { L_, 8.8213654057158198962995384e-294, "882136540571582", -293 },
            { L_, 8.8232792321247303676243898e+210, "882327923212473",  211 },
            { L_, 8.8265997461714396872169928e-043, "882659974617144",  -42 },
            { L_, 8.8283404625692696040189354e-077, "882834046256927",  -76 },
            { L_, 8.8293124362675499931978742e+229, "882931243626755",  230 },
            { L_, 8.8323264448272993681336526e+287,  "88323264448273",  288 },
            { L_, 8.8329279791532900000000000e+015, "883292797915329",   16 },
            { L_, 8.8349208711859398724904060e-133, "883492087118594", -132 },
            { L_, 8.8364878595238995063622312e+198,  "88364878595239",  199 },
            { L_, 8.8368644081995996417572041e-098,  "88368644081996",  -97 },
            { L_, 8.8377388281365301362919508e+244, "883773882813653",  245 },
            { L_, 8.8383574829865296268210241e+226, "883835748298653",  227 },
            { L_, 8.8399587101071900454793100e-222, "883995871010719", -221 },
            { L_, 8.8409748097887195171970448e+265, "884097480978872",  266 },
            { L_, 8.8416922641818398477154189e+223, "884169226418184",  224 },
            { L_, 8.8466124958530897097264287e+080, "884661249585309",   81 },
            { L_, 8.8494563165929900000000000e+014, "884945631659299",   15 },
            { L_, 8.8531939517090605522489611e-132, "885319395170906", -131 },
            { L_, 8.8541886774711203378304860e+034, "885418867747112",   35 },
            { L_, 8.8548481494680501808775337e+291, "885484814946805",  292 },
            { L_, 8.8551307958339997118821077e+215,   "8855130795834",  216 },
            { L_, 8.8578698429404001956024236e-069,  "88578698429404",  -68 },
            { L_, 8.8589117330712398769339825e+095, "885891173307124",   96 },
            { L_, 8.8615208684752106258552289e+273, "886152086847521",  274 },
            { L_, 8.8634066577313797392628756e-186, "886340665773138", -185 },
            { L_, 8.8665436342024399914655624e+267, "886654363420244",  268 },
            { L_, 8.8670709651456901471377951e+081, "886707096514569",   82 },
            { L_, 8.8673132793033106266011371e-071, "886731327930331",  -70 },
            { L_, 8.8679206248966407527129865e+270, "886792062489664",  271 },
            { L_, 8.8706053085303701063577990e+140, "887060530853037",  141 },
            { L_, 8.8710561828936203177608664e+262, "887105618289362",  263 },
            { L_, 8.8716506335752696958455727e+244, "887165063357527",  245 },
            { L_, 8.8717938705497593357899583e+041, "887179387054976",   42 },
            { L_, 8.8724765074172100691560281e+259, "887247650741721",  260 },
            { L_, 8.8729354196280401632589705e-161, "887293541962804", -160 },
            { L_, 8.8752413633386192053264984e-295, "887524136333862", -294 },
            { L_, 8.8767209080275696923018656e+211, "887672090802757",  212 },
            { L_, 8.8773405474023503637232105e-015, "887734054740235",  -14 },
            { L_, 8.8773418086001405407176257e+056, "887734180860014",   57 },
            { L_, 8.8778239095630391808103157e-233, "887782390956304", -232 },
            { L_, 8.8786263241841094864488031e-227, "887862632418411", -226 },
            { L_, 8.8798614254974502094632657e-120, "887986142549745", -119 },
            { L_, 8.8825690782414804105971721e+050, "888256907824148",   51 },
            { L_, 8.8843729100774104710271911e+239, "888437291007741",  240 },
            { L_, 8.8854982744408796594369060e-045, "888549827444088",  -44 },
            { L_, 8.8859906981409595302205519e-091, "888599069814096",  -90 },
            { L_, 8.8860266091685099594254647e-237, "888602660916851", -236 },
            { L_, 8.8864961109014394490098889e+182, "888649611090144",  183 },
            { L_, 8.8870186038096695211575623e+265, "888701860380967",  266 },
            { L_, 8.8870285680480406581473036e+206, "888702856804804",  207 },
            { L_, 8.8917531952033596351037237e+210, "889175319520336",  211 },
            { L_, 8.8925222572346000444450677e+040,  "88925222572346",   41 },
            { L_, 8.8927913939105693136198529e+128, "889279139391057",  129 },
            { L_, 8.8952565359162194272306834e+147, "889525653591622",  148 },
            { L_, 8.8964125506570094200650890e+243, "889641255065701",  244 },
            { L_, 8.8977683484450403246143936e+063, "889776834844504",   64 },
            { L_, 8.9026658387256498187147673e+229, "890266583872565",  230 },
            { L_, 8.9035464719747703836978492e+169, "890354647197477",  170 },
            { L_, 8.9041263740319495371886576e+279, "890412637403195",  280 },
            { L_, 8.9045413108098995827118870e-073,  "89045413108099",  -72 },
            { L_, 8.9072222228667494922787087e+033, "890722222286675",   34 },
            { L_, 8.9107505187047999352216721e+006,  "89107505187048",    7 },
            { L_, 8.9117051500404097147631374e-198, "891170515004041", -197 },
            { L_, 8.9120909338225104744202194e+090, "891209093382251",   91 },
            { L_, 8.9127713215974505390770479e-231, "891277132159745", -230 },
            { L_, 8.9137052232166600121512155e-045, "891370522321666",  -44 },
            { L_, 8.9141186975449995553833141e-096,   "8914118697545",  -95 },
            { L_, 8.9144830577138493033446871e+124, "891448305771385",  125 },
            { L_, 8.9151921657436396812967352e+223, "891519216574364",  224 },
            { L_, 8.9158512381100703573404808e-108, "891585123811007", -107 },
            { L_, 8.9158797349622704749509501e+065, "891587973496227",   66 },
            { L_, 8.9176492575688996660419029e-265,  "89176492575689", -264 },
            { L_, 8.9178904745141603932063409e+063, "891789047451416",   64 },
            { L_, 8.9198023061958404614340916e+229, "891980230619584",  230 },
            { L_, 8.9216223723271002070487254e-071,  "89216223723271",  -70 },
            { L_, 8.9216867615367805684683786e-132, "892168676153678", -131 },
            { L_, 8.9220831633616396207816824e+129, "892208316336164",  130 },
            { L_, 8.9227306759021500000000000e+015, "892273067590215",   16 },
            { L_, 8.9232552101253796633634673e-251, "892325521012538", -250 },
            { L_, 8.9257199264921797769696379e+027, "892571992649218",   28 },
            { L_, 8.9280431312492804089839616e+025, "892804313124928",   26 },
            { L_, 8.9288969862645601151610042e-005, "892889698626456",   -4 },
            { L_, 8.9329379579301894967859732e-106, "893293795793019", -105 },
            { L_, 8.9329775932833595768520104e+239, "893297759328336",  240 },
            { L_, 8.9334901069731191824985217e+000, "893349010697312",    1 },
            { L_, 8.9341297841543897949593147e-164, "893412978415439", -163 },
            { L_, 8.9347652293517694691548166e+088, "893476522935177",   89 },
            { L_, 8.9363959736170698023672730e-122, "893639597361707", -121 },
            { L_, 8.9424181113380595448604950e-229, "894241811133806", -228 },
            { L_, 8.9490570748339904736186570e-193, "894905707483399", -192 },
            { L_, 8.9511367966882393082971660e-121, "895113679668824", -120 },
            { L_, 8.9523375507279804056564294e+087, "895233755072798",   88 },
            { L_, 8.9525045035938599088315841e+105, "895250450359386",  106 },
            { L_, 8.9528756675093805427594997e+182, "895287566750938",  183 },
            { L_, 8.9537331636220497120149051e+231, "895373316362205",  232 },
            { L_, 8.9609398108256404195743320e-157, "896093981082564", -156 },
            { L_, 8.9627412123786702488582840e+187, "896274121237867",  188 },
            { L_, 8.9648965443008993539874351e+100,  "89648965443009",  101 },
            { L_, 8.9675002997759103888143720e+229, "896750029977591",  230 },
            { L_, 8.9697743133911099463605906e-228, "896977431339111", -227 },
            { L_, 8.9700200104573296747933165e-278, "897002001045733", -277 },
            { L_, 8.9718825646324200293291959e+076, "897188256463242",   77 },
            { L_, 8.9731183849263904868556244e-030, "897311838492639",  -29 },
            { L_, 8.9734036572113093917013436e-188, "897340365721131", -187 },
            { L_, 8.9737570459363502960457474e+206, "897375704593635",  207 },
            { L_, 8.9751240601219503882056385e-280, "897512406012195", -279 },
            { L_, 8.9771490287916794227378840e+278, "897714902879168",  279 },
            { L_, 8.9783960388710704194367989e-035, "897839603887107",  -34 },
            { L_, 8.9783979586723400425701461e+232, "897839795867234",  233 },
            { L_, 8.9790148716131699335634391e+093, "897901487161317",   94 },
            { L_, 8.9796897399706005318939709e-226,  "89796897399706", -225 },
            { L_, 8.9826016905942497977679168e+133, "898260169059425",  134 },
            { L_, 8.9827851375022804691425690e+026, "898278513750228",   27 },
            { L_, 8.9835628233504902862045087e-062, "898356282335049",  -61 },
            { L_, 8.9854105492703395524464797e-098, "898541054927034",  -97 },
            { L_, 8.9862615003476201135643807e-190, "898626150034762", -189 },
            { L_, 8.9905091372276395138857655e-243, "899050913722764", -242 },
            { L_, 8.9918433962685103270265386e+184, "899184339626851",  185 },
            { L_, 8.9920900632003200487864399e+119, "899209006320032",  120 },
            { L_, 8.9937889045098894736124154e+199, "899378890450989",  200 },
            { L_, 8.9941461179807902328381655e+247, "899414611798079",  248 },
            { L_, 8.9964151866621400449024000e+023, "899641518666214",   24 },
            { L_, 8.9969827012489796985919951e+073, "899698270124898",   74 },
            { L_, 8.9980222896848005194522031e-172,  "89980222896848", -171 },
            { L_, 8.9983218992276300916306653e+122, "899832189922763",  123 },
            { L_, 8.9989322049924793047533206e-233, "899893220499248", -232 },
            { L_, 9.0011963733973596663878170e-100, "900119637339736",  -99 },
            { L_, 9.0039693913421103524557135e+115, "900396939134211",  116 },
            { L_, 9.0042891538943494877601996e-306, "900428915389435", -305 },
            { L_, 9.0093207692623191323054354e+031, "900932076926232",   32 },
            { L_, 9.0106854711572196815753472e+264, "901068547115722",  265 },
            { L_, 9.0109363711474706130285867e-248, "901093637114747", -247 },
            { L_, 9.0120906098097103813350560e+107, "901209060980971",  108 },
            { L_, 9.0126788988610902044874857e+083, "901267889886109",   84 },
            { L_, 9.0133671191259307200858392e+068, "901336711912593",   69 },
            { L_, 9.0177911689979705928340616e+267, "901779116899797",  268 },
            { L_, 9.0207982092943297568475333e+218, "902079820929433",  219 },
            { L_, 9.0210701764702896466616487e-015, "902107017647029",  -14 },
            { L_, 9.0228111836403402940727684e+285, "902281118364034",  286 },
            { L_, 9.0231353893276594672656616e+259, "902313538932766",  260 },
            { L_, 9.0265095020702101544006595e-259, "902650950207021", -258 },
            { L_, 9.0274567524764303078565855e-244, "902745675247643", -243 },
            { L_, 9.0274700090415600859122802e+212, "902747000904156",  213 },
            { L_, 9.0295031087001193996479435e+070, "902950310870012",   71 },
            { L_, 9.0326774184766207504743678e+056, "903267741847662",   57 },
            { L_, 9.0336996789562799517944980e-298, "903369967895628", -297 },
            { L_, 9.0347834812621899376573317e-130, "903478348126219", -129 },
            { L_, 9.0349287788917707183038181e+097, "903492877889177",   98 },
            { L_, 9.0352177779470002030672164e-119,   "9035217777947", -118 },
            { L_, 9.0371583751688601489439715e-208, "903715837516886", -207 },
            { L_, 9.0376366967529603404219567e-298, "903763669675296", -297 },
            { L_, 9.0385208956796400356463461e+044, "903852089567964",   45 },
            { L_, 9.0393667723168398888487965e-025, "903936677231684",  -24 },
            { L_, 9.0408078471945104587752618e+052, "904080784719451",   53 },
            { L_, 9.0448869034675498930746657e+135, "904488690346755",  136 },
            { L_, 9.0453846234000999342104812e-119,  "90453846234001", -118 },
            { L_, 9.0454889374503600811988254e+077, "904548893745036",   78 },
            { L_, 9.0462164891383397847910255e-296, "904621648913834", -295 },
            { L_, 9.0466937752832197522494746e-119, "904669377528322", -118 },
            { L_, 9.0472312997206004677176243e-014,  "90472312997206",  -13 },
            { L_, 9.0481879734030792311227938e+267, "904818797340308",  268 },
            { L_, 9.0489473879787200863538115e-009, "904894738797872",   -8 },
            { L_, 9.0513676163774494605973306e-068, "905136761637745",  -67 },
            { L_, 9.0541718456494204696704845e+115, "905417184564942",  116 },
            { L_, 9.0561088269981900154019097e+149, "905610882699819",  150 },
            { L_, 9.0576947113914593146600137e+224, "905769471139146",  225 },
            { L_, 9.0617392963792103486382201e-226, "906173929637921", -225 },
            { L_, 9.0656938453960101153784216e-243, "906569384539601", -242 },
            { L_, 9.0685303407888707293092140e-171, "906853034078887", -170 },
            { L_, 9.0692156835538601849266531e-219, "906921568355386", -218 },
            { L_, 9.0699573113672499673006989e-197, "906995731136725", -196 },
            { L_, 9.0723209522370900785831418e-038, "907232095223709",  -37 },
            { L_, 9.0748725346658800874982370e-105, "907487253466588", -104 },
            { L_, 9.0749061353383307749204829e-133, "907490613533833", -132 },
            { L_, 9.0761488374850099820440229e-305, "907614883748501", -304 },
            { L_, 9.0788435962129796369875119e+074, "907884359621298",   75 },
            { L_, 9.0825973949047900160000000e+018, "908259739490479",   19 },
            { L_, 9.0829983928013004272851449e-254,  "90829983928013", -253 },
            { L_, 9.0837286348672991189014108e+270,  "90837286348673",  271 },
            { L_, 9.0851214918170306149186266e-130, "908512149181703", -129 },
            { L_, 9.0869559734157003487593465e-228,  "90869559734157", -227 },
            { L_, 9.0869994352914903250797844e+260, "908699943529149",  261 },
            { L_, 9.0909192440658396387685756e+130, "909091924406584",  131 },
            { L_, 9.0913383700222596047338309e-112, "909133837002226", -111 },
            { L_, 9.0920280264175195191500503e-155, "909202802641752", -154 },
            { L_, 9.0933230265321796801897693e-137, "909332302653218", -136 },
            { L_, 9.0933326096302299796570304e+111, "909333260963023",  112 },
            { L_, 9.0948159538659801017946861e+180, "909481595386598",  181 },
            { L_, 9.0972321178087395081345139e+053, "909723211780874",   54 },
            { L_, 9.1003679201027699198927919e-164, "910036792010277", -163 },
            { L_, 9.1020949206054496131964508e+239, "910209492060545",  240 },
            { L_, 9.1049979342291900293651048e+134, "910499793422919",  135 },
            { L_, 9.1055128245665100162659213e-020, "910551282456651",  -19 },
            { L_, 9.1060759153911194904076803e-198, "910607591539112", -197 },
            { L_, 9.1080063754059299160957846e-194, "910800637540593", -193 },
            { L_, 9.1090495408537299484352040e+177, "910904954085373",  178 },
            { L_, 9.1092454843953100881920000e+021, "910924548439531",   22 },
            { L_, 9.1118490344914304224377056e-178, "911184903449143", -177 },
            { L_, 9.1130082803185995690803200e+023,  "91130082803186",   24 },
            { L_, 9.1144995203026704667239030e-248, "911449952030267", -247 },
            { L_, 9.1177110688930203070893074e-071, "911771106889302",  -70 },
            { L_, 9.1205682624679704471713712e-078, "912056826246797",  -77 },
            { L_, 9.1215031373365006140373212e-005,  "91215031373365",   -4 },
            { L_, 9.1224073759530906904050568e-118, "912240737595309", -117 },
            { L_, 9.1239108708062497982321336e+200, "912391087080625",  201 },
            { L_, 9.1267502315236693794848631e-277, "912675023152367", -276 },
            { L_, 9.1271619239378793498028872e-232, "912716192393788", -231 },
            { L_, 9.1273548483777095162788556e-178, "912735484837771", -177 },
            { L_, 9.1276561030135097503662109e+009, "912765610301351",   10 },
            { L_, 9.1276836073300604544065313e-175, "912768360733006", -174 },
            { L_, 9.1288141535835102223496023e-057, "912881415358351",  -56 },
            { L_, 9.1293730474270299925144150e-186, "912937304742703", -185 },
            { L_, 9.1323955735280505381152379e-200, "913239557352805", -199 },
            { L_, 9.1333648290956595914159436e+143, "913336482909566",  144 },
            { L_, 9.1350998272459497532009373e-011, "913509982724595",  -10 },
            { L_, 9.1353512058444600060616080e+190, "913535120584446",  191 },
            { L_, 9.1417117054645995792642768e-289,  "91417117054646", -288 },
            { L_, 9.1445520750748804051564598e+070, "914455207507488",   71 },
            { L_, 9.1457729467131906250000000e+013, "914577294671319",   14 },
            { L_, 9.1464839402608001716716198e-100,  "91464839402608",  -99 },
            { L_, 9.1485121641934991469087762e-084,  "91485121641935",  -83 },
            { L_, 9.1486247096609096361098646e+306, "914862470966091",  307 },
            { L_, 9.1514781143635405101562564e-135, "915147811436354", -134 },
            { L_, 9.1515367563605193683485838e-084, "915153675636052",  -83 },
            { L_, 9.1534674606702696054756637e+160, "915346746067027",  161 },
            { L_, 9.1565897088941003648927922e-029,  "91565897088941",  -28 },
            { L_, 9.1571822577669696003588532e+207, "915718225776697",  208 },
            { L_, 9.1583509456323393700481857e-288, "915835094563234", -287 },
            { L_, 9.1620553060136201284066331e+230, "916205530601362",  231 },
            { L_, 9.1660268265671306075139777e-118, "916602682656713", -117 },
            { L_, 9.1665228838065495522964404e-207, "916652288380655", -206 },
            { L_, 9.1681984494370402890063500e-069, "916819844943704",  -68 },
            { L_, 9.1684132145514892223656796e-143, "916841321455149", -142 },
            { L_, 9.1704410169677808422768483e-267, "917044101696778", -266 },
            { L_, 9.1731540690049397276937032e+213, "917315406900494",  214 },
            { L_, 9.1803060460944195728035203e+257, "918030604609442",  258 },
            { L_, 9.1815692167257201099670893e+217, "918156921672572",  218 },
            { L_, 9.1831682621348904877926891e-077, "918316826213489",  -76 },
            { L_, 9.1834960928360103140595341e+118, "918349609283601",  119 },
            { L_, 9.1862316902218095990012106e+104, "918623169022181",  105 },
            { L_, 9.1864170488262998528075357e-150,  "91864170488263", -149 },
            { L_, 9.1871756570434906664426254e+178, "918717565704349",  179 },
            { L_, 9.1876864119458197927874105e+148, "918768641194582",  149 },
            { L_, 9.1925294914707394813922943e-058, "919252949147074",  -57 },
            { L_, 9.1931515470484996392847810e-174,  "91931515470485", -173 },
            { L_, 9.1956686053140301117221478e-245, "919566860531403", -244 },
            { L_, 9.1956792008591701329064424e+089, "919567920085917",   90 },
            { L_, 9.1960465963298599300497278e-254, "919604659632986", -253 },
            { L_, 9.1977642391563495250289580e+065, "919776423915635",   66 },
            { L_, 9.1988928870783096443189655e+123, "919889288707831",  124 },
            { L_, 9.2002797718807496893026413e-191, "920027977188075", -190 },
            { L_, 9.2004971273002695611616603e-118, "920049712730027", -117 },
            { L_, 9.2012733048217898150507145e+246, "920127330482179",  247 },
            { L_, 9.2047153267543001048457832e-141,  "92047153267543", -140 },
            { L_, 9.2056850370883995868576148e+291,  "92056850370884",  292 },
            { L_, 9.2060548327690603791202442e-263, "920605483276906", -262 },
            { L_, 9.2092334258243800058061021e-244, "920923342582438", -243 },
            { L_, 9.2113841833582800746118510e+229, "921138418335828",  230 },
            { L_, 9.2115785860170801312714885e+122, "921157858601708",  123 },
            { L_, 9.2137980879560605476661777e-045, "921379808795606",  -44 },
            { L_, 9.2144678567739598996785764e-227, "921446785677396", -226 },
            { L_, 9.2161717762131095859195357e-196, "921617177621311", -195 },
            { L_, 9.2170004435396503255227039e-060, "921700044353965",  -59 },
            { L_, 9.2171856471680598914005849e-126, "921718564716806", -125 },
            { L_, 9.2214136217264601685725512e+250, "922141362172646",  251 },
            { L_, 9.2221201644993699909838652e-053, "922212016449937",  -52 },
            { L_, 9.2255722987995896015357078e+139, "922557229879959",  140 },
            { L_, 9.2285032145146498892038145e+305, "922850321451465",  306 },
            { L_, 9.2297392403821198509343264e+222, "922973924038212",  223 },
            { L_, 9.2298250537397501665513153e-254, "922982505373975", -253 },
            { L_, 9.2298480308341396024513968e+295, "922984803083414",  296 },
            { L_, 9.2302630280611295116398706e+281, "923026302806113",  282 },
            { L_, 9.2323515282979001580400699e-264,  "92323515282979", -263 },
            { L_, 9.2347728046179895169514405e-196, "923477280461799", -195 },
            { L_, 9.2358267309291503269609304e+050, "923582673092915",   51 },
            { L_, 9.2380094379754698068323276e+225, "923800943797547",  226 },
            { L_, 9.2393938372107300705415404e-098, "923939383721073",  -97 },
            { L_, 9.2408774518532199813242285e+170, "924087745185322",  171 },
            { L_, 9.2412580141239598479081545e-122, "924125801412396", -121 },
            { L_, 9.2421964878346697912376669e+221, "924219648783467",  222 },
            { L_, 9.2431457439573005748662059e+221,  "92431457439573",  222 },
            { L_, 9.2450939931387798273027698e-190, "924509399313878", -189 },
            { L_, 9.2452278742725502828451926e-078, "924522787427255",  -77 },
            { L_, 9.2460852972428706588466519e-280, "924608529724287", -279 },
            { L_, 9.2473365040947294301271545e+045, "924733650409473",   46 },
            { L_, 9.2487648302627007314265578e+289,  "92487648302627",  290 },
            { L_, 9.2492777367517292157845427e-220, "924927773675173", -219 },
            { L_, 9.2493310667026501823766708e+275, "924933106670265",  276 },
            { L_, 9.2501190834848394830750055e+193, "925011908348484",  194 },
            { L_, 9.2510669439994195669933558e-244, "925106694399942", -243 },
            { L_, 9.2512590765641992946170376e+199,  "92512590765642",  200 },
            { L_, 9.2545720829893305925484639e+038, "925457208298933",   39 },
            { L_, 9.2556223053329006538953138e+125,  "92556223053329",  126 },
            { L_, 9.2572193638758599347946815e-287, "925721936387586", -286 },
            { L_, 9.2576242428022204844193133e-157, "925762424280222", -156 },
            { L_, 9.2599341895872804957731764e+249, "925993418958728",  250 },
            { L_, 9.2637289408868494688196134e+276, "926372894088685",  277 },
            { L_, 9.2644289210035906773083057e+234, "926442892100359",  235 },
            { L_, 9.2666347121553200949470833e+146, "926663471215532",  147 },
            { L_, 9.2682995267892796819384216e-212, "926829952678928", -211 },
            { L_, 9.2694307598898104281311679e+093, "926943075988981",   94 },
            { L_, 9.2708292265744098618964383e-297, "927082922657441", -296 },
            { L_, 9.2717699483426399075687111e+305, "927176994834264",  306 },
            { L_, 9.2736778593938192644108298e-003, "927367785939382",   -2 },
            { L_, 9.2744035956461501152553913e+111, "927440359564615",  112 },
            { L_, 9.2766710440794800402627338e-182, "927667104407948", -181 },
            { L_, 9.2796926061251799448092000e+101, "927969260612518",  102 },
            { L_, 9.2845162003724398667555168e-047, "928451620037244",  -46 },
            { L_, 9.2859108177549003072515953e+097,  "92859108177549",   98 },
            { L_, 9.2864393886011295279772980e+188, "928643938860113",  189 },
            { L_, 9.2885140476500308649016157e-149, "928851404765003", -148 },
            { L_, 9.2889889225433499488325932e-052, "928898892254335",  -51 },
            { L_, 9.2893367565846800859165705e-247, "928933675658468", -246 },
            { L_, 9.2900924938858292318818206e-046, "929009249388583",  -45 },
            { L_, 9.2911700062243599605699812e-126, "929117000622436", -125 },
            { L_, 9.2923648957182703761721130e+081, "929236489571827",   82 },
            { L_, 9.2930561018195990508037470e-047,  "92930561018196",  -46 },
            { L_, 9.2976406894161999471544177e+284,  "92976406894162",  285 },
            { L_, 9.2977151863900997508238788e-001,  "92977151863901",    0 },
            { L_, 9.2988839463125006861873424e-015,  "92988839463125",  -14 },
            { L_, 9.2996004586290098990011786e-084, "929960045862901",  -83 },
            { L_, 9.3015063255556494807861434e-181, "930150632555565", -180 },
            { L_, 9.3047646342547898705976441e+040, "930476463425479",   41 },
            { L_, 9.3050261252337594470967564e-162, "930502612523376", -161 },
            { L_, 9.3059971400012701614554763e-271, "930599714000127", -270 },
            { L_, 9.3064612587456103507487921e-073, "930646125874561",  -72 },
            { L_, 9.3077318188513602133153740e-309, "930773181885136", -308 },
            { L_, 9.3084126998723702087725866e-144, "930841269987237", -143 },
            { L_, 9.3085721678915006625567626e-187,  "93085721678915", -186 },
            { L_, 9.3101672039169300256462932e+299, "931016720391693",  300 },
            { L_, 9.3130869900147599871157231e-210, "931308699001476", -209 },
            { L_, 9.3133078652088202895360238e+146, "931330786520882",  147 },
            { L_, 9.3149022535765694224773873e-304, "931490225357657", -303 },
            { L_, 9.3175175192637697420064477e+078, "931751751926377",   79 },
            { L_, 9.3211104123548394811074359e+151, "932111041235484",  152 },
            { L_, 9.3212174640581593063084335e+100, "932121746405816",  101 },
            { L_, 9.3235363860121100707235569e-047, "932353638601211",  -46 },
            { L_, 9.3262333414081000149178080e-301,  "93262333414081", -300 },
            { L_, 9.3309694606050700495982897e-077, "933096946060507",  -76 },
            { L_, 9.3374461888837506353108835e-267, "933744618888375", -266 },
            { L_, 9.3377919414106798883318086e-216, "933779194141068", -215 },
            { L_, 9.3381795170423196868984738e+246, "933817951704232",  247 },
            { L_, 9.3389139622881803825130302e-076, "933891396228818",  -75 },
            { L_, 9.3394648162173495596646610e-157, "933946481621735", -156 },
            { L_, 9.3396343166819695132269934e-198, "933963431668197", -197 },
            { L_, 9.3400863729075895730796511e+297, "934008637290759",  298 },
            { L_, 9.3401588205021602577164160e-098, "934015882050216",  -97 },
            { L_, 9.3408004831841805255299354e+082, "934080048318418",   83 },
            { L_, 9.3433063421602304970414129e-195, "934330634216023", -194 },
            { L_, 9.3433356843559902077371551e-173, "934333568435599", -172 },
            { L_, 9.3434040382152905301893497e-001, "934340403821529",    0 },
            { L_, 9.3500345860858694166117459e+081, "935003458608587",   82 },
            { L_, 9.3502884019950899233093138e-239, "935028840199509", -238 },
            { L_, 9.3524036310457402449963473e+144, "935240363104574",  145 },
            { L_, 9.3526970524064591766661300e-025, "935269705240646",  -24 },
            { L_, 9.3532524287269300927727400e-003, "935325242872693",   -2 },
            { L_, 9.3544874316860599328723091e-026, "935448743168606",  -25 },
            { L_, 9.3555370292703491927830712e+270, "935553702927035",  271 },
            { L_, 9.3568512128799407827344204e-255, "935685121287994", -254 },
            { L_, 9.3570301994398300000000000e+014, "935703019943983",   15 },
            { L_, 9.3575534592099893371508536e-149, "935755345920999", -148 },
            { L_, 9.3604347713031102585173066e-185, "936043477130311", -184 },
            { L_, 9.3636094644318096583051759e-231, "936360946443181", -230 },
            { L_, 9.3658179172445197053478765e-125, "936581791724452", -124 },
            { L_, 9.3685666339159297456465488e-178, "936856663391593", -177 },
            { L_, 9.3689623984688593657811398e+197, "936896239846886",  198 },
            { L_, 9.3713044041499503467827710e-033, "937130440414995",  -32 },
            { L_, 9.3736098134618202588624403e+095, "937360981346182",   96 },
            { L_, 9.3758951336077796790257954e+127, "937589513360778",  128 },
            { L_, 9.3760408381244502540829908e-082, "937604083812445",  -81 },
            { L_, 9.3761425431818101398204340e+118, "937614254318181",  119 },
            { L_, 9.3761487979848893784031022e-043, "937614879798489",  -42 },
            { L_, 9.3762924119969506423037866e+200, "937629241199695",  201 },
            { L_, 9.3780156136612400234058548e+084, "937801561366124",   85 },
            { L_, 9.3795745303432405847123315e-077, "937957453034324",  -76 },
            { L_, 9.3841608348040102633517551e+117, "938416083480401",  118 },
            { L_, 9.3886777787449993631617671e-015,   "9388677778745",  -14 },
            { L_, 9.3887151609446599451839230e+091, "938871516094466",   92 },
            { L_, 9.3927873838155499361072874e+298, "939278738381555",  299 },
            { L_, 9.3977154767035703547604624e+171, "939771547670357",  172 },
            { L_, 9.3991415315746002275595714e-103,  "93991415315746", -102 },
            { L_, 9.3998579239270902483659022e-049, "939985792392709",  -48 },
            { L_, 9.4016647753788304819355090e+227, "940166477537883",  228 },
            { L_, 9.4038280826852300237390451e+081, "940382808268523",   82 },
            { L_, 9.4049070203413799087819758e-263, "940490702034138", -262 },
            { L_, 9.4072306015606105082825703e+111, "940723060156061",  112 },
            { L_, 9.4124116871288498932759205e-137, "941241168712885", -136 },
            { L_, 9.4126236340399904139386973e-223, "941262363403999", -222 },
            { L_, 9.4132376627214700397100431e-173, "941323766272147", -172 },
            { L_, 9.4160202489887408870893416e+105, "941602024898874",  106 },
            { L_, 9.4162121367388199757379964e-258, "941621213673882", -257 },
            { L_, 9.4174676372351097214977794e-240, "941746763723511", -239 },
            { L_, 9.4176115331564595177553328e+069, "941761153315646",   70 },
            { L_, 9.4179786632506302945625019e+263, "941797866325063",  264 },
            { L_, 9.4183127660955505487933209e+291, "941831276609555",  292 },
            { L_, 9.4228967640960708509515900e-255, "942289676409607", -254 },
            { L_, 9.4237522583266603544769440e+165, "942375225832666",  166 },
            { L_, 9.4239857729955803811980428e+274, "942398577299558",  275 },
            { L_, 9.4250022375476803637444273e+178, "942500223754768",  179 },
            { L_, 9.4258294973204405823416793e+107, "942582949732044",  108 },
            { L_, 9.4260154712986601155145689e-073, "942601547129866",  -72 },
            { L_, 9.4282690820030897979724184e-212, "942826908200309", -211 },
            { L_, 9.4293380304879098943447747e-275, "942933803048791", -274 },
            { L_, 9.4293489612095596172445232e-130, "942934896120956", -129 },
            { L_, 9.4328319812910408074755651e-158, "943283198129104", -157 },
            { L_, 9.4331487981794005751778628e-022,  "94331487981794",  -21 },
            { L_, 9.4348271273422006996868588e+074,  "94348271273422",   75 },
            { L_, 9.4355131877681703076073914e-301, "943551318776817", -300 },
            { L_, 9.4362699253137093932526440e-055, "943626992531371",  -54 },
            { L_, 9.4372584264545097806539162e-164, "943725842645451", -163 },
            { L_, 9.4380915817585191886854441e-180, "943809158175852", -179 },
            { L_, 9.4381306700772305900971895e-074, "943813067007723",  -73 },
            { L_, 9.4403915016211506273963894e-084, "944039150162115",  -83 },
            { L_, 9.4408474957330702509260366e-061, "944084749573307",  -60 },
            { L_, 9.4416068428701901085734028e-108, "944160684287019", -107 },
            { L_, 9.4416362751098198425600000e+021, "944163627510982",   22 },
            { L_, 9.4440867820104893800664794e-213, "944408678201049", -212 },
            { L_, 9.4475530349588998046646042e-018,  "94475530349589",  -17 },
            { L_, 9.4485215343551797672885464e+076, "944852153435518",   77 },
            { L_, 9.4497075676433305612807892e-168, "944970756764333", -167 },
            { L_, 9.4526448186775605033658649e-040, "945264481867756",  -39 },
            { L_, 9.4534324168254099825926365e-134, "945343241682541", -133 },
            { L_, 9.4535120134928997472558286e-186,  "94535120134929", -185 },
            { L_, 9.4536341390177608019241716e+031, "945363413901776",   32 },
            { L_, 9.4558778818057000468916956e+269,  "94558778818057",  270 },
            { L_, 9.4572432201552498080844143e+037, "945724322015525",   38 },
            { L_, 9.4586447854096101077412686e-135, "945864478540961", -134 },
            { L_, 9.4595207773228702881897998e-013, "945952077732287",  -12 },
            { L_, 9.4643307826287107506201371e+043, "946433078262871",   44 },
            { L_, 9.4645638006697700309419305e-085, "946456380066977",  -84 },
            { L_, 9.4657493099200996136440044e+107,  "94657493099201",  108 },
            { L_, 9.4673728237631798278534925e-015, "946737282376318",  -14 },
            { L_, 9.4687501592220194611993275e+201, "946875015922202",  202 },
            { L_, 9.4689636139982692689625181e+224, "946896361399827",  225 },
            { L_, 9.4717949553059806026313050e-005, "947179495530598",   -4 },
            { L_, 9.4730261641954603827200000e+020, "947302616419546",   21 },
            { L_, 9.4763143528527597631752188e-064, "947631435285276",  -63 },
            { L_, 9.4768845574511195621937006e-050, "947688455745112",  -49 },
            { L_, 9.4800411941582992304240925e+072,  "94800411941583",   73 },
            { L_, 9.4842401703546098277209365e+174, "948424017035461",  175 },
            { L_, 9.4879746448493299862155423e+088, "948797464484933",   89 },
            { L_, 9.4912780185988303069549880e+286, "949127801859883",  287 },
            { L_, 9.4915963704484596519250772e+001, "949159637044846",    2 },
            { L_, 9.4928613964884093178084062e-307, "949286139648841", -306 },
            { L_, 9.4948326592463603108226919e-104, "949483265924636", -103 },
            { L_, 9.4951408743877402943953766e-296, "949514087438774", -295 },
            { L_, 9.4951947486406393812821883e+183, "949519474864064",  184 },
            { L_, 9.4973237826388704789061507e-223, "949732378263887", -222 },
            { L_, 9.4984868485267601039145295e+072, "949848684852676",   73 },
            { L_, 9.4993276251154197563507060e-121, "949932762511542", -120 },
            { L_, 9.5004692251635302212588891e+255, "950046922516353",  256 },
            { L_, 9.5008159203743403835773034e+155, "950081592037434",  156 },
            { L_, 9.5016546608629205393270462e-145, "950165466086292", -144 },
            { L_, 9.5021262568728398266654964e+233, "950212625687284",  234 },
            { L_, 9.5044445361290406328330299e+301, "950444453612904",  302 },
            { L_, 9.5059316812380305608385096e+284, "950593168123803",  285 },
            { L_, 9.5074961617481399832703832e-041, "950749616174814",  -40 },
            { L_, 9.5085954782521704866022748e-154, "950859547825217", -153 },
            { L_, 9.5107458507828499111688045e-057, "951074585078285",  -56 },
            { L_, 9.5115330219341999605079300e-054,  "95115330219342",  -53 },
            { L_, 9.5143143941529395407114329e-084, "951431439415294",  -83 },
            { L_, 9.5143463385631194999178417e-180, "951434633856312", -179 },
            { L_, 9.5152813731210397402732151e+147, "951528137312104",  148 },
            { L_, 9.5153547394398595603593474e+225, "951535473943986",  226 },
            { L_, 9.5159665062611795875886809e-261, "951596650626118", -260 },
            { L_, 9.5229296140594601259487920e-127, "952292961405946", -126 },
            { L_, 9.5293086291309300000000000e+014, "952930862913093",   15 },
            { L_, 9.5301230972925193642001429e-272, "953012309729252", -271 },
            { L_, 9.5328003673174193832489873e+212, "953280036731742",  213 },
            { L_, 9.5391917950732202409577175e+198, "953919179507322",  199 },
            { L_, 9.5392988446186395027903751e-041, "953929884461864",  -40 },
            { L_, 9.5402379360505499641596774e+256, "954023793605055",  257 },
            { L_, 9.5422989308068103499518643e+226, "954229893080681",  227 },
            { L_, 9.5446964643976999881371633e-228,  "95446964643977", -227 },
            { L_, 9.5512104628514305921739783e-101, "955121046285143", -100 },
            { L_, 9.5526877748591708555467940e-224, "955268777485917", -223 },
            { L_, 9.5527190194889703245932181e+178, "955271901948897",  179 },
            { L_, 9.5567491977339904491779423e+206, "955674919773399",  207 },
            { L_, 9.5582593176860105662611898e+255, "955825931768601",  256 },
            { L_, 9.5589371543548095774206046e+154, "955893715435481",  155 },
            { L_, 9.5596120927196897503199855e-025, "955961209271969",  -24 },
            { L_, 9.5596213642514705928799823e-166, "955962136425147", -165 },
            { L_, 9.5607405337322403752690005e+210, "956074053373224",  211 },
            { L_, 9.5641698641376505154309287e-101, "956416986413765", -100 },
            { L_, 9.5642508894214206128469524e-266, "956425088942142", -265 },
            { L_, 9.5650702580573696422010233e-005, "956507025805737",   -4 },
            { L_, 9.5706611778977603944677706e-255, "957066117789776", -254 },
            { L_, 9.5718685537682304511137597e-030, "957186855376823",  -29 },
            { L_, 9.5739037981335604592837857e-083, "957390379813356",  -82 },
            { L_, 9.5763083564513998346184649e-118,  "95763083564514", -117 },
            { L_, 9.5763462147407500651512628e+066, "957634621474075",   67 },
            { L_, 9.5763494356336101119176771e-138, "957634943563361", -137 },
            { L_, 9.5791209333677204796238172e-079, "957912093336772",  -78 },
            { L_, 9.5802850626974203453985778e-067, "958028506269742",  -66 },
            { L_, 9.5817444016876700615472021e+255, "958174440168767",  256 },
            { L_, 9.5818993448346502990563725e-301, "958189934483465", -300 },
            { L_, 9.5827643708409204976107852e-118, "958276437084092", -117 },
            { L_, 9.5874145640265103118691964e+104, "958741456402651",  105 },
            { L_, 9.5920477442104501206492127e-018, "959204774421045",  -17 },
            { L_, 9.5991444267874002095262026e+282,  "95991444267874",  283 },
            { L_, 9.5995184162459302172853455e-132, "959951841624593", -131 },
            { L_, 9.6003369882244002450967238e-202,  "96003369882244", -201 },
            { L_, 9.6005622721247792447208680e+115, "960056227212478",  116 },
            { L_, 9.6018821576509201428653237e-006, "960188215765092",   -5 },
            { L_, 9.6028073750828304399518002e+139, "960280737508283",  140 },
            { L_, 9.6028279733244997020721744e+054,  "96028279733245",   55 },
            { L_, 9.6071142541287103082536285e+232, "960711425412871",  233 },
            { L_, 9.6076640441174405074466086e-023, "960766404411744",  -22 },
            { L_, 9.6086784224582092553968144e-105, "960867842245821", -104 },
            { L_, 9.6105048590114794637866900e+043, "961050485901148",   44 },
            { L_, 9.6124785656721793192142246e-040, "961247856567218",  -39 },
            { L_, 9.6142893104831998257400258e+127,  "96142893104832",  128 },
            { L_, 9.6149390089226700991584889e+290, "961493900892267",  291 },
            { L_, 9.6153711045056793268929241e-089, "961537110450568",  -88 },
            { L_, 9.6168479687268304495058896e+181, "961684796872683",  182 },
            { L_, 9.6231683147348098164923652e-055, "962316831473481",  -54 },
            { L_, 9.6242897078185904930270780e+254, "962428970781859",  255 },
            { L_, 9.6245767911090100871297772e+066, "962457679110901",   67 },
            { L_, 9.6255743395756702071575367e+063, "962557433957567",   64 },
            { L_, 9.6279427060625898543216037e+128, "962794270606259",  129 },
            { L_, 9.6294088404336800696947881e-053, "962940884043368",  -52 },
            { L_, 9.6322032555637404755471944e+169, "963220325556374",  170 },
            { L_, 9.6328118921529493951614613e-165, "963281189215295", -164 },
            { L_, 9.6334072538596392836119888e+276, "963340725385964",  277 },
            { L_, 9.6386312886989799072267240e+165, "963863128869898",  166 },
            { L_, 9.6388208003844493749966478e+200, "963882080038445",  201 },
            { L_, 9.6393347301015595716093979e+259, "963933473010156",  260 },
            { L_, 9.6394083444500606664888251e-198, "963940834445006", -197 },
            { L_, 9.6397039150740108489990234e+009, "963970391507401",   10 },
            { L_, 9.6411465006035506298071010e-306, "964114650060355", -305 },
            { L_, 9.6415557553040697279036033e-173, "964155575530407", -172 },
            { L_, 9.6431277642232101212630756e+139, "964312776422321",  140 },
            { L_, 9.6436907656890600780930481e+274, "964369076568906",  275 },
            { L_, 9.6444826064265500428797594e+110, "964448260642655",  111 },
            { L_, 9.6459126770755903110966855e+248, "964591267707559",  249 },
            { L_, 9.6463443045482091581569382e+081, "964634430454821",   82 },
            { L_, 9.6470901814928996282308705e+173,  "96470901814929",  174 },
            { L_, 9.6482392674196201024936429e-039, "964823926741962",  -38 },
            { L_, 9.6483529619681701292806025e-084, "964835296196817",  -83 },
            { L_, 9.6493152843722298466166615e-080, "964931528437223",  -79 },
            { L_, 9.6503630097886304877311898e-207, "965036300978863", -206 },
            { L_, 9.6504948575288601893945256e-206, "965049485752886", -205 },
            { L_, 9.6543214996538199926178405e-154, "965432149965382", -153 },
            { L_, 9.6543598701734903913087940e-241, "965435987017349", -240 },
            { L_, 9.6555481546631702126666404e-262, "965554815466317", -261 },
            { L_, 9.6578806069777102455618397e-026, "965788060697771",  -25 },
            { L_, 9.6595450374203001729803481e+169,  "96595450374203",  170 },
            { L_, 9.6611096117779298407072262e-086, "966110961177793",  -85 },
            { L_, 9.6624772875717804127376388e+082, "966247728757178",   83 },
            { L_, 9.6627871583048700015932962e+249, "966278715830487",  250 },
            { L_, 9.6637028063666302315150643e-215, "966370280636663", -214 },
            { L_, 9.6656768591518695694348281e+207, "966567685915187",  208 },
            { L_, 9.6694421112236095965877702e+201, "966944211122361",  202 },
            { L_, 9.6705564743807395456587283e-226, "967055647438074", -225 },
            { L_, 9.6706413890586305709522808e-278, "967064138905863", -277 },
            { L_, 9.6707640828476505129560194e-186, "967076408284765", -185 },
            { L_, 9.6715005399588499637762160e+176, "967150053995885",  177 },
            { L_, 9.6724458668790300371384829e+062, "967244586687903",   63 },
            { L_, 9.6730871331012804954501503e-201, "967308713310128", -200 },
            { L_, 9.6749223930040392681246036e+218, "967492239300404",  219 },
            { L_, 9.6764217448588595659591256e+166, "967642174485886",  167 },
            { L_, 9.6767873419472403635657639e-134, "967678734194724", -133 },
            { L_, 9.6819634080078709007098462e+242, "968196340800787",  243 },
            { L_, 9.6849387986179799756214630e+307, "968493879861798",  308 },
            { L_, 9.6851271797910589822245192e-156, "968512717979106", -155 },
            { L_, 9.6852312296775101601257332e+245, "968523122967751",  246 },
            { L_, 9.6856298069598102939812269e-264, "968562980695981", -263 },
            { L_, 9.6860234372579995888115193e-121,   "9686023437258", -120 },
            { L_, 9.6919658836047395621115113e-036, "969196588360474",  -35 },
            { L_, 9.6933737591491697288073971e-167, "969337375914917", -166 },
            { L_, 9.6944293313406305969692767e+004, "969442933134063",    5 },
            { L_, 9.6948151720214504467671924e-059, "969481517202145",  -58 },
            { L_, 9.6956091450267091562014299e-140, "969560914502671", -139 },
            { L_, 9.6999149074308900637720956e-095, "969991490743089",  -94 },
            { L_, 9.7002797676507294045377974e+302, "970027976765073",  303 },
            { L_, 9.7007031288846195301450726e-004, "970070312888462",   -3 },
            { L_, 9.7024040644049291820445001e+143, "970240406440493",  144 },
            { L_, 9.7035111694763602452186398e+128, "970351116947636",  129 },
            { L_, 9.7060390034809898515682039e+141, "970603900348099",  142 },
            { L_, 9.7093717210762209756746066e+279, "970937172107622",  280 },
            { L_, 9.7094359678319200077615778e-027, "970943596783192",  -26 },
            { L_, 9.7125498289034497116046835e-176, "971254982890345", -175 },
            { L_, 9.7182994497740695249087159e-069, "971829944977407",  -68 },
            { L_, 9.7202814158618410270224806e-156, "972028141586184", -155 },
            { L_, 9.7202907895320697747771001e-191, "972029078953207", -190 },
            { L_, 9.7221627816573800896714670e+215, "972216278165738",  216 },
            { L_, 9.7246858126641202785860229e+292, "972468581266412",  293 },
            { L_, 9.7252213068802005613249972e+146,  "97252213068802",  147 },
            { L_, 9.7256952772164698914208474e-015, "972569527721647",  -14 },
            { L_, 9.7275309155341502618502882e-101, "972753091553415", -100 },
            { L_, 9.7306555172354495374685562e-087, "973065551723545",  -86 },
            { L_, 9.7321494491293408013146307e-090, "973214944912934",  -89 },
            { L_, 9.7331167290112801450655508e-129, "973311672901128", -128 },
            { L_, 9.7347155452900197519667385e-079, "973471554529002",  -78 },
            { L_, 9.7366849721221400953974757e+156, "973668497212214",  157 },
            { L_, 9.7388109410830698283665564e+201, "973881094108307",  202 },
            { L_, 9.7393151639784805079903175e-281, "973931516397848", -280 },
            { L_, 9.7398637949251400212889758e+234, "973986379492514",  235 },
            { L_, 9.7401719070775996116942430e+079,  "97401719070776",   80 },
            { L_, 9.7425186727062792653994606e-146, "974251867270628", -145 },
            { L_, 9.7448183022263495178405940e-256, "974481830222635", -255 },
            { L_, 9.7484903245075800495227926e-173, "974849032450758", -172 },
            { L_, 9.7485011804763494583341770e-178, "974850118047635", -177 },
            { L_, 9.7485167752983204413652388e-178, "974851677529832", -177 },
            { L_, 9.7500290905729100131416318e-237, "975002909057291", -236 },
            { L_, 9.7511817764729194868052021e+071, "975118177647292",   72 },
            { L_, 9.7515580580515698808303135e+246, "975155805805157",  247 },
            { L_, 9.7554905115865107385603959e+221, "975549051158651",  222 },
            { L_, 9.7556277649078607820453410e+108, "975562776490786",  109 },
            { L_, 9.7577963042994603225070048e+155, "975779630429946",  156 },
            { L_, 9.7584873673719303974043638e-290, "975848736737193", -289 },
            { L_, 9.7622112290415895022275248e-049, "976221122904159",  -48 },
            { L_, 9.7644793506755902255213094e+147, "976447935067559",  148 },
            { L_, 9.7665182718466605834917694e-120, "976651827184666", -119 },
            { L_, 9.7674593542949804767806328e-259, "976745935429498", -258 },
            { L_, 9.7677292129904603880678145e+133, "976772921299046",  134 },
            { L_, 9.7684879271662305607546290e+274, "976848792716623",  275 },
            { L_, 9.7696071329425302115174770e+125, "976960713294253",  126 },
            { L_, 9.7718299270913900711641499e+031, "977182992709139",   32 },
            { L_, 9.7749442378411794085699817e+070, "977494423784118",   71 },
            { L_, 9.7759292671146700036554698e+229, "977592926711467",  230 },
            { L_, 9.7766256325541593947208039e+251, "977662563255416",  252 },
            { L_, 9.7775356001455405645471533e-291, "977753560014554", -290 },
            { L_, 9.7789400090529006207258107e-289,  "97789400090529", -288 },
            { L_, 9.7801784365887700329008477e+199, "978017843658877",  200 },
            { L_, 9.7830296819991106235877686e+234, "978302968199911",  235 },
            { L_, 9.7844533227392400358152254e+194, "978445332273924",  195 },
            { L_, 9.7856478029750400048191796e-118, "978564780297504", -117 },
            { L_, 9.7876366020034202718270393e+155, "978763660200342",  156 },
            { L_, 9.7880763284706206064860343e-163, "978807632847062", -162 },
            { L_, 9.7882288572808704157973942e+287, "978822885728087",  288 },
            { L_, 9.7883720612268002508800000e+020,  "97883720612268",   21 },
            { L_, 9.7887319274454097267314148e+275, "978873192744541",  276 },
            { L_, 9.7908125244739199863683400e+215, "979081252447392",  216 },
            { L_, 9.7909743272116504589442181e+166, "979097432721165",  167 },
            { L_, 9.7911479522460803017257218e+193, "979114795224608",  194 },
            { L_, 9.7923813773463104701474886e-129, "979238137734631", -128 },
            { L_, 9.7934984352585299166234270e-181, "979349843525853", -180 },
            { L_, 9.7940709649234110320442371e+198, "979407096492341",  199 },
            { L_, 9.7947204820890002874454165e+203,   "9794720482089",  204 },
            { L_, 9.7948270050565495361585685e-228, "979482700505655", -227 },
            { L_, 9.7992602826937491045100614e+003, "979926028269375",    4 },
            { L_, 9.8004433730975793150678779e+097, "980044337309758",   98 },
            { L_, 9.8004777046704501286829046e+135, "980047770467045",  136 },
            { L_, 9.8013631469201794609268232e-019, "980136314692018",  -18 },
            { L_, 9.8027083474658793695861516e+284, "980270834746588",  285 },
            { L_, 9.8054847830511706141359302e+113, "980548478305117",  114 },
            { L_, 9.8061527971781908376464529e-280, "980615279717819", -279 },
            { L_, 9.8070088029675498659081259e-141, "980700880296755", -140 },
            { L_, 9.8100105940518892262399061e-308, "981001059405189", -307 },
            { L_, 9.8110954435799906250000000e+013, "981109544357999",   14 },
            { L_, 9.8127995667263191061021047e+186, "981279956672632",  187 },
            { L_, 9.8128374110270690137469298e-277, "981283741102707", -276 },
            { L_, 9.8177667941628799458819682e+232, "981776679416288",  233 },
            { L_, 9.8178443725190305478029601e+259, "981784437251903",  260 },
            { L_, 9.8180630225397393616644069e-066, "981806302253974",  -65 },
            { L_, 9.8194090005660595071978853e-047, "981940900056606",  -46 },
            { L_, 9.8201869245757396765895826e+048, "982018692457574",   49 },
            { L_, 9.8215182158536489707401631e-240, "982151821585365", -239 },
            { L_, 9.8216249394462402744021345e+037, "982162493944624",   38 },
            { L_, 9.8256850176274792670125828e+168, "982568501762748",  169 },
            { L_, 9.8276639524504193572890426e+254, "982766395245042",  255 },
            { L_, 9.8280677701027705446662651e+105, "982806777010277",  106 },
            { L_, 9.8318519149314707157461767e+062, "983185191493147",   63 },
            { L_, 9.8325592021173508186361492e-044, "983255920211735",  -43 },
            { L_, 9.8340083164011693477586898e-035, "983400831640117",  -34 },
            { L_, 9.8362970686414399436585556e-187, "983629706864144", -186 },
            { L_, 9.8425142732558795610935216e-147, "984251427325588", -146 },
            { L_, 9.8429220873829605797764599e+105, "984292208738296",  106 },
            { L_, 9.8438531479669594755816094e+073, "984385314796696",   74 },
            { L_, 9.8490292806053500406358608e-237, "984902928060535", -236 },
            { L_, 9.8518126466807183399070893e-309, "985181264668072", -308 },
            { L_, 9.8523407239894003486939988e+286,  "98523407239894",  287 },
            { L_, 9.8526205428227699778535591e-266, "985262054282277", -265 },
            { L_, 9.8532893288567005051846456e+108,  "98532893288567",  109 },
            { L_, 9.8562657024227195591065600e+023, "985626570242272",   24 },
            { L_, 9.8573395004970694210605413e-244, "985733950049707", -243 },
            { L_, 9.8609458138355903810039345e+264, "986094581383559",  265 },
            { L_, 9.8623706498810407632033085e+078, "986237064988104",   79 },
            { L_, 9.8642882070508709903840995e-038, "986428820705087",  -37 },
            { L_, 9.8664749070460904446208080e+095, "986647490704609",   96 },
            { L_, 9.8695357901453593943697466e+168, "986953579014536",  169 },
            { L_, 9.8725135425690003504180433e-239,   "9872513542569", -238 },
            { L_, 9.8728849644905695295668598e+064, "987288496449057",   65 },
            { L_, 9.8755750721624703125000000e+013, "987557507216247",   14 },
            { L_, 9.8763521506873203709301030e+035, "987635215068732",   36 },
            { L_, 9.8776633146262407011875690e+226, "987766331462624",  227 },
            { L_, 9.8776895043899895356062885e+268, "987768950438999",  269 },
            { L_, 9.8819527833147409116839716e+192, "988195278331474",  193 },
            { L_, 9.8820180398256998341745247e-303,  "98820180398257", -302 },
            { L_, 9.8825604839805793603503240e-190, "988256048398058", -189 },
            { L_, 9.8828464258750297929038464e-071, "988284642587503",  -70 },
            { L_, 9.8846327795843094983082230e+194, "988463277958431",  195 },
            { L_, 9.8852433953013799484560454e+029, "988524339530138",   30 },
            { L_, 9.8862854682357004238848000e+022,  "98862854682357",   23 },
            { L_, 9.8883351834763393155776581e+128, "988833518347634",  129 },
            { L_, 9.8886295626493192181821764e+186, "988862956264932",  187 },
            { L_, 9.8892665430949301015040541e-241, "988926654309493", -240 },
            { L_, 9.8894548143825806394893966e-283, "988945481438258", -282 },
            { L_, 9.8901317991979896005028975e-091, "989013179919799",  -90 },
            { L_, 9.8916706693669303489538872e+057, "989167066936693",   58 },
            { L_, 9.8918001475903701465584427e-129, "989180014759037", -128 },
            { L_, 9.8954215338853490463508056e-249, "989542153388535", -248 },
            { L_, 9.8957403697988602289002222e-208, "989574036979886", -207 },
            { L_, 9.8965434058732410067349711e+139, "989654340587324",  140 },
            { L_, 9.8980370228524993148225869e-052,  "98980370228525",  -51 },
            { L_, 9.8990458002647403579976902e+302, "989904580026474",  303 },
            { L_, 9.8998994104886802223789732e-210, "989989941048868", -209 },
            { L_, 9.9006869204185293319702950e-167, "990068692041853", -166 },
            { L_, 9.9022642665087304600998027e-111, "990226426650873", -110 },
            { L_, 9.9033290287792797005823559e+098, "990332902877928",   99 },
            { L_, 9.9068358341826603202631821e+060, "990683583418266",   61 },
            { L_, 9.9079538946501500545537084e+087, "990795389465015",   88 },
            { L_, 9.9106311777267598357476531e+129, "991063117772676",  130 },
            { L_, 9.9106539111053698134855379e-106, "991065391110537", -105 },
            { L_, 9.9130295068502201254241336e+048, "991302950685022",   49 },
            { L_, 9.9131480002974290544540262e-283, "991314800029743", -282 },
            { L_, 9.9141072095205796317307102e+131, "991410720952058",  132 },
            { L_, 9.9147667919018498044808227e-250, "991476679190185", -249 },
            { L_, 9.9150265818931294282846799e+070, "991502658189313",   71 },
            { L_, 9.9170459826729500011030061e+104, "991704598267295",  105 },
            { L_, 9.9178225102235695221909054e-254, "991782251022357", -253 },
            { L_, 9.9207721896353300477796033e+127, "992077218963533",  128 },
            { L_, 9.9211135492026708157689620e-208, "992111354920267", -207 },
            { L_, 9.9211622025568400651219048e-279, "992116220255684", -278 },
            { L_, 9.9219166642554200058958622e+181, "992191666425542",  182 },
            { L_, 9.9230285214260303254721787e-082, "992302852142603",  -81 },
            { L_, 9.9232000526321199441564995e-037, "992320005263212",  -36 },
            { L_, 9.9286398884942105817748780e-127, "992863988849421", -126 },
            { L_, 9.9290980601257799919254065e+290, "992909806012578",  291 },
            { L_, 9.9326703340010597625539532e-181, "993267033400106", -180 },
            { L_, 9.9336838686580898521193242e+266, "993368386865809",  267 },
            { L_, 9.9347871327388396005605819e+086, "993478713273884",   87 },
            { L_, 9.9392916802035892924088974e+071, "993929168020359",   72 },
            { L_, 9.9404763424121904298244409e-140, "994047634241219", -139 },
            { L_, 9.9431075396191490615348527e+071, "994310753961915",   72 },
            { L_, 9.9445307398151195536002918e-177, "994453073981512", -176 },
            { L_, 9.9465438379308999393973232e+289,  "99465438379309",  290 },
            { L_, 9.9488318219923499710673236e-088, "994883182199235",  -87 },
            { L_, 9.9490073366881594078530388e-267, "994900733668816", -266 },
            { L_, 9.9494033008551705705283236e-261, "994940330085517", -260 },
            { L_, 9.9495725032919404171445792e-238, "994957250329194", -237 },
            { L_, 9.9504002842264804775872385e-225, "995040028422648", -224 },
            { L_, 9.9527245277442507976040387e-252, "995272452774425", -251 },
            { L_, 9.9535952516936296811437259e-256, "995359525169363", -255 },
            { L_, 9.9536475848972103065509952e-079, "995364758489721",  -78 },
            { L_, 9.9539119468807099266707135e-284, "995391194688071", -283 },
            { L_, 9.9569011809203105173010912e-105, "995690118092031", -104 },
            { L_, 9.9591963915687300450969092e+110, "995919639156873",  111 },
            { L_, 9.9600401203065004236395188e+197,  "99600401203065",  198 },
            { L_, 9.9631524964015203825076823e-165, "996315249640152", -164 },
            { L_, 9.9668788171586101657058208e+083, "996687881715861",   84 },
            { L_, 9.9699944977932102290988002e-016, "996999449779321",  -15 },
            { L_, 9.9704900917834194199084992e+039, "997049009178342",   40 },
            { L_, 9.9710797935152098627613597e+172, "997107979351521",  173 },
            { L_, 9.9763325473069515485925727e-309, "997633254730695", -308 },
            { L_, 9.9773803623584999935045214e+220,  "99773803623585",  221 },
            { L_, 9.9785381162406797173482200e+216, "997853811624068",  217 },
            { L_, 9.9787941336830206616135728e-182, "997879413368302", -181 },
            { L_, 9.9798879447118597454391420e+004, "997988794471186",    5 },
            { L_, 9.9803649979573903805160603e+113, "998036499795739",  114 },
            { L_, 9.9856450030248003363845400e-211,  "99856450030248", -210 },
            { L_, 9.9856986163082203448118560e-138, "998569861630822", -137 },
            { L_, 9.9868764536277300230435925e+206, "998687645362773",  207 },
            { L_, 9.9896285765458297455647459e+120, "998962857654583",  121 },
            { L_, 9.9935798648223598526634320e-238, "999357986482236", -237 },
            { L_, 9.9960611654740699840035259e+127, "999606116547407",  128 },
            { L_, 9.9964855704873605088228284e-275, "999648557048736", -274 },
            { L_, 9.9985002049778294410553577e+098, "999850020497783",   99 },
            { L_, 9.9990689927606503236037228e-194, "999906899276065", -193 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
#ifdef BSLS_PLATFORM_OS_WINDOWS
#   define strtoll _strtoi64
#endif
        for (int i = 0; i < NUM_DATA; ++i) {
            int         LINE        = DATA[i].d_line;
            double      BINARY      = DATA[i].d_binary;
            const char *SIGNIFICAND = DATA[i].d_significand;
            int         EXPONENT    = DATA[i].d_exponent;
            long long   sig         = strtoll(SIGNIFICAND, 0, 10);
            int         exp         = EXPONENT - strlen(SIGNIFICAND);
            Decimal64   fromBinary  = Util::decimal64FromDouble(BINARY);
            Decimal64   expected    = DecimalUtil::makeDecimal64(sig, exp);

            if (veryVeryVerbose) {
                P_(LINE) P_(BINARY) P_(sig) P_(exp) P_(expected) P(fromBinary)
            }

            ASSERTV(LINE, BINARY, sig, exp, fromBinary, expected,
                    strictEqual(fromBinary, expected));
        }
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
        //   decimal64FromVariableWidthEncoding(*decimal, *buffer);
        //   decimal64ToVariableWidthEncoding(*buffer, value);
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
                LOOP2_ASSERT(
                       original,
                       std::numeric_limits<BDEC::Decimal64>::quiet_NaN(),
                       strictEqual(
                           original,
                           std::numeric_limits<BDEC::Decimal64>::quiet_NaN()));
                BDEC::Decimal64 qnan(
                            std::numeric_limits<BDEC::Decimal64>::quiet_NaN());
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
      case -1: {
        // --------------------------------------------------------------------
        // DECIMAL ORIGIN CONVERSION TEST
        //
        // Concerns:
        //: 1 Short decimal strings round-trip correctly.
        //
        // Plan:
        //: 1 For each digit string of the form -/+NNNNNNNNeE, -9 <= E <= 0,
        //:   use strtod to convert it to 'double', convert the result to
        //:   'Decimal64', then convert that back to 'double', and verify that
        //:   the two 'double' values are identical.
        //:
        //: 2 For each digit string of the form -/+NNNNNNeE, -9 <= E <= 4,
        //:   use sscanf to convert it to 'float', convert the result to
        //:   'Decimal64', then convert that back to 'float', and verify that
        //:   the two 'float' values are identical.  (The exponent range is
        //:   somewhat arbitrary; it overlaps the optimized range on both
        //:   ends.)
        //:
        //: 3 Additionally, directly parse the digit strings to decimal values
        //:   and verify that these values too match the values converted from
        //:   binary.
        //
        // Testing:
        //   CONVERSION TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nDECIMAL ORIGIN CONVERSION TEST"
                               << "\n==============================\n";

        if (verbose) {
            bsl::cout << "float\n";
        }
        {
            for (int e = -9; e <= 4; ++e) {
                for (int n = -999999; n <= 999999; ++n) {
                    char buf[30];
                    sprintf(buf, "%de%d", n, e);
                    float b;
                    sscanf(buf, "%f", &b);
                    Decimal64 d = Util::decimal64FromFloat(b);
                    Decimal64 p = PARSEDEC64(buf);
                    float t = Util::decimalToFloat(d);
                    ASSERTV(e, n, d, p, b, t, b == t);
                    ASSERTV(e, n, d, p, b, t, p == d);
                    if (verbose) {
                        if (rand() % 500 == 0 && rand() % 500 == 0) {
                            P_(e) P_(n) P_(d) P(b)
                        }
                    }
                }
            }
        }

        if (verbose) {
            bsl::cout << "double\n";
        }
        {
            for (int e = -9; e <= 0; ++e) {
                for (int n = -99999999; n <= 99999999; ++n) {
                    char buf[30];
                    sprintf(buf, "%de%d", n, e);
                    double b = strtod(buf, 0);
                    Decimal64 d = Util::decimal64FromDouble(b);
                    Decimal64 p = PARSEDEC64(buf);
                    double t = Util::decimalToDouble(d);
                    ASSERTV(e, n, d, p, b, t, b == t);
                    ASSERTV(e, n, d, p, b, t, p == d);
                    if (verbose) {
                        if (rand() % 5000 == 0 && rand() % 5000 == 0) {
                            P_(e) P_(n) P_(d) P(b)
                        }
                    }
                }
            }
        }
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // ROUND TRIP CONVERSION TEST
        //
        // Concerns:
        //: 1 Decimal string round-trips correctly.
        //
        // Plan:
        //: 1 For each command-line argument, use strtod to convert it to
        //:   'double', convert the result to 'Decimal64', then convert that
        //:   back to 'double', and verify that the two 'double' values are
        //:   identical.
        //
        // Testing:
        //   ROUND TRIP CONVERSION TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nROUND TRIP CONVERSION TEST"
                               << "\n==========================\n";

        for (int i = 2; i < argc; ++i) {
            const char *s = argv[i];
            {
                float b;
                sscanf(s, "%f", &b);
                Decimal64 d = Util::decimal64FromFloat(b);
                float t = Util::decimalToFloat(d);
                ASSERTV(s, d, b, t, b == t);
                P_(s) P_(d) P_(b) Q("float")
            }
            {
                double b = strtod(s, 0);
                Decimal64 d = Util::decimal64FromDouble(b);
                double t = Util::decimalToDouble(d);
                ASSERTV(s, d, b, t, b == t);
                P_(s) P_(d) P_(b) Q("double")
            }
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
