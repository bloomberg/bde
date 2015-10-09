// balber_berutil.t.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balber_berutil.h>

#include <balber_berconstants.h>

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>
#include <bdlat_selectioninfo.h>
#include <bdlat_typetraits.h>
#include <bdlat_valuetypefunctions.h>

#include <bsls_objectbuffer.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bslim_testutil.h>

#include <bdlsb_memoutstreambuf.h>
#include <bdlsb_fixedmemoutstreambuf.h>
#include <bdlsb_fixedmeminstreambuf.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_time.h>
#include <bdlt_datetz.h>
#include <bdlt_datetimetz.h>
#include <bdlt_serialdateimputil.h>
#include <bdlt_timetz.h>

#include <bdlb_print.h>
#include <bdlb_printmethods.h>

#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_climits.h>
#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//
//
// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };
enum { SUCCESS = 0, FAILURE = -1 };

typedef bsls::Types::Int64      Int64;
typedef bsls::Types::Uint64     Uint64;

typedef balber::BerUtil         Util;
typedef bslstl::StringRef       StringRef;

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

int numOctets(const char *s)
    // Return the number of octets contained in the specified 's'.  Note that
    // it is assumed that each octet in 's' is specified in hex format.
{
    int length = 0;
    for (; *s; ++s) {
        if (' ' == *s) {
            continue;
        }

        ++length;
    }
    return length / 2;
}

int getIntValue(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';                                               // RETURN
    }
    c = toupper(c);
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;                                          // RETURN
    }
    ASSERT(0);
    return -1;
}

int compareBuffers(const char *stream, const char *buffer)
    // Compare the data written to the  specified 'stream' with the data in the
    // specified 'buffer'.  Return 0 on success, and -1 otherwise.
{
    while (*buffer) {
        if (' ' == *buffer) {
            ++buffer;
            continue;
        }
        char temp = (char) getIntValue(*buffer) << 4;
        ++buffer;
        temp |= (char) getIntValue(*buffer);
        if (*stream != temp) {
           return -1;                                                 // RETURN
        }
        ++stream;
        ++buffer;
    }
    return 0;
}

void printBuffer(const char *buffer, int length)
    // Print the specified 'buffer' of the specified 'length' in hex form
{
    cout << hex;
    int numOutput = 0;
    for (int i = 0; i < length; ++i) {
        if ((unsigned char) buffer[i] < 16) {
            cout << '0';
        }
        cout << (int) (unsigned char) buffer[i];
        numOutput += 2;
        if (0 == numOutput % 8) {
            cout << " ";
        }
    }
    cout << dec << endl;
}

#define DOUBLE_MANTISSA_MASK   0xfffffffffffffLL
#define DOUBLE_SIGN_MASK       ((long long) ((long long) 1                   \
                                               << (sizeof(long long) * 8 - 1)))

void assembleDouble(double *value, int sign, int exponent, long long mantissa)
{
    enum {
        DOUBLE_EXPONENT_SHIFT  = 52,
        DOUBLE_BIAS            = 1023
    };

    unsigned long long *longLongValue
                               = reinterpret_cast<unsigned long long *>(value);

    *longLongValue  = (unsigned long long) exponent << DOUBLE_EXPONENT_SHIFT;
    *longLongValue |= mantissa & DOUBLE_MANTISSA_MASK;

    if (sign) {
        *longLongValue |= DOUBLE_SIGN_MASK;
    }
}

void ignoreAssertHandler(const char *, const char *, int)
{
}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------


// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4; (void) veryVeryVerbose;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 22: {
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

        if (verbose) bsl::cout << "\n" "USAGE EXAMPLE" "\n"
                                       "=============" "\n";
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:  Reading and Writing Identifier Octets
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate the usage of this component.  Due
// to the low-level nature of this component, an extended usage example is not
// necessary.
//
// Suppose we wanted to write the identifier octets for a BER tag having the
// following properties:
//..
//    Tag Class:   Context-specific
//    Tag Type:    Primitive
//    Tag Number:  31
//..
// According to the BER specification, this should generate two octets
// containing the values 0x9F and 0x1F.  The following function demonstrates
// this:
//..
    bdlsb::MemOutStreamBuf osb;

    balber::BerConstants::TagClass tagClass  =
                                      balber::BerConstants::e_CONTEXT_SPECIFIC;
    balber::BerConstants::TagType  tagType   =
                                             balber::BerConstants::e_PRIMITIVE;
    int                            tagNumber = 31;

    int retCode = balber::BerUtil::putIdentifierOctets(&osb,
                                                       tagClass,
                                                       tagType,
                                                       tagNumber);
    ASSERT(0    == retCode);
    ASSERT(2    == osb.length());
    ASSERT(0x9F == (unsigned char)osb.data()[0]);
    ASSERT(0x1F == (unsigned char)osb.data()[1]);
//..
// The next part of the function will read the identifier octets from the
// stream and verify its contents:
//..
    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());

    balber::BerConstants::TagClass tagClassIn;
    balber::BerConstants::TagType  tagTypeIn;
    int                            tagNumberIn;
    int                            numBytesConsumed = 0;

    retCode = balber::BerUtil::getIdentifierOctets(&isb,
                                                   &tagClassIn,
                                                   &tagTypeIn,
                                                   &tagNumberIn,
                                                   &numBytesConsumed);
    ASSERT(0         == retCode);
    ASSERT(2         == numBytesConsumed);
    ASSERT(tagClass  == tagClassIn);
    ASSERT(tagType   == tagTypeIn);
    ASSERT(tagNumber == tagNumberIn);
//..

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' FOR DATE/TIME COMPONENTS USING A VARIANT
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\n"
                         "TESTING 'getValue' FOR DATE/TIME USING VARIANT" "\n"
                         "==============================================" "\n";

        balber::BerEncoderOptions options;
        options.setEncodeDateAndTimeTypesAsBinary(true);
        const balber::BerEncoderOptions DEFOPTS;

        if (verbose) bsl::cout << "\nDefine data" << bsl::endl;

        static const struct {
            int d_lineNum;   // source line number
            int d_year;      // year under test
            int d_month;     // month under test
            int d_day;       // day under test
            int d_hour;      // hour under test
            int d_minutes;   // minutes under test
            int d_seconds;   // seconds under test
            int d_milliSecs; // milli seconds under test
            int d_tzoffset;  // time zone offset
        } DATA[] = {
   //-------^
   //line no.  year   month   day   hour    min   sec    ms  offset
   //-------   -----  -----   ---   ----    ---   ---    --  ------
    {      L_,      1,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,     45     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    1,     1,     1,    1,    1,      0     },
    {      L_,      1,     1,    1,     1,     1,    1,    1,    500     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    1,     1,    23,   59,   59,      0     },
    {      L_,      1,     1,    1,     1,    23,   59,   59,   1439     },
    {      L_,      1,     1,    1,     1,    23,   59,   59,  -1439     },

    {      L_,      1,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,    2,     0,     0,    0,    0,   1439     },
    {      L_,      1,     1,    2,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    2,     1,     1,    1,    1,      0     },
    {      L_,      1,     1,    2,     1,     1,    1,    1,    500     },

    {      L_,      1,     1,    2,     1,    23,   59,   59,      0     },
    {      L_,      1,     1,    2,     1,    23,   59,   59,    500     },
    {      L_,      1,     1,    2,     1,    23,   59,   59,   -500     },

    {      L_,      1,     1,   10,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,   10,     1,     1,    1,    1,     99     },

    {      L_,      1,     1,   30,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,   31,     0,     0,    0,    0,   1439     },
    {      L_,      1,     1,   31,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     2,    1,     0,     0,    0,    0,      0     },
    {      L_,      1,     2,    1,    23,    59,   59,    0,   1439     },

    {      L_,      1,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,      1,    12,   31,    23,    59,   59,    0,   1439     },

    {      L_,      2,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      2,     1,    1,    23,    59,   59,    0,   1439     },

    {      L_,      4,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      4,     1,    1,    23,    59,   59,    0,   1439     },

    {      L_,      4,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,      4,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,      4,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,      4,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,      4,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,      4,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,      4,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,      4,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,      4,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,      8,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,      8,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,      8,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,      8,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,      8,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,      8,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,    100,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    100,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,    100,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,    100,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    100,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,    100,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    400,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,    400,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,    400,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,    400,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    400,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,    400,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,    500,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    500,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,    500,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    500,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,    800,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    800,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,    800,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,    800,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,    800,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    800,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   1000,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   1000,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,   1000,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   1000,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   2000,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,   2000,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   2000,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   2016,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2017,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2018,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2019,    12,   31,     0,     0,    0,    0,      0     },

    {      L_,   2020,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,   2020,     1,    1,     0,     0,    0,    0,   1439     },
    {      L_,   2020,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,   2020,     1,    1,    23,    59,   59,  999,      0     },
    {      L_,   2020,     1,    1,    23,    59,   59,  999,   1439     },
    {      L_,   2020,     1,    1,    23,    59,   59,  999,  -1439     },

    {      L_,   2020,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,   2020,     1,    2,     0,     0,    0,    0,   1439     },
    {      L_,   2020,     1,    2,     0,     0,    0,    0,  -1439     },

    {      L_,   2020,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   2020,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,   2020,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,   2020,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,   2020,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   2020,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,   2021,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,   2022,     1,    2,     0,     0,    0,    0,      0     },

    {      L_,   9999,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   9999,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,   9999,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,   9999,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   9999,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,   9999,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,   9999,    12,   30,     0,     0,    0,    0,      0     },
    {      L_,   9999,    12,   30,    23,    59,   59,    0,   1439     },

    {      L_,   9999,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   9999,    12,   31,    23,    59,   59,    0,   1439     },
   //-------v
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) bsl::cout << "\nTesting variant of 'bdlt::Date'"
                               << " and 'bdlt::DateTz'." << bsl::endl;
        {
            typedef bdlb::Variant2<bdlt::Date, bdlt::DateTz> Variant;

            typedef bdlt::Date Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;

                if (veryVerbose) { T_ P_(LINE) P_(Y) P_(M) P(D) }

                const Type VALUE(Y, M, D);
                Variant    value1, value2;

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value1,
                                                     &numBytesConsumed));
                    ASSERT(0      == isb.length());
                    ASSERT(LENGTH == numBytesConsumed);
                    LOOP_ASSERT(LINE, value1.is<bdlt::Date>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::Date>());
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value2,
                                                     &numBytesConsumed));
                    ASSERT(0      == isb.length());
                    ASSERT(LENGTH == numBytesConsumed);
                    LOOP_ASSERT(LINE, value2.is<bdlt::Date>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::Date>());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::DateTz'." << bsl::endl;
        {
            typedef bdlb::Variant2<bdlt::Date, bdlt::DateTz> Variant;

            typedef bdlt::DateTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int OFF  = DATA[i].d_tzoffset;

                if (veryVerbose) { T_ P_(LINE) P_(Y) P_(M) P_(D) P(OFF) }

                const Type VALUE(bdlt::Date(Y, M, D), OFF);
                Variant    value1, value2;

                if (OFF) {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value1,
                                                     &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP_ASSERT(LINE, value1.is<bdlt::DateTz>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::DateTz>());
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value2,
                                                     &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP_ASSERT(LINE, value2.is<bdlt::DateTz>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::DateTz>());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting variant of 'bdlt::Time'"
                               << " and 'bdlt::TimeTz'." << bsl::endl;
        {
            typedef bdlb::Variant2<bdlt::Time, bdlt::TimeTz> Variant;

            typedef bdlt::Time Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;

                if (veryVerbose) { T_ P_(LINE) P_(H) P_(MM) P_(S) P(MS) }

                const Type VALUE(H, MM, S, MS);
                Variant    value1, value2;

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value1,
                                                     &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP_ASSERT(LINE, value1.is<bdlt::Time>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::Time>());
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value2,
                                                     &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP_ASSERT(LINE, value2.is<bdlt::Time>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::Time>());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::TimeTz'." << bsl::endl;
        {
            typedef bdlb::Variant2<bdlt::Time, bdlt::TimeTz> Variant;

            typedef bdlt::TimeTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;
                const int OFF  = DATA[i].d_tzoffset;

                if (veryVerbose) {
                                 T_ P_(LINE) P_(H) P_(MM) P_(S) P_(MS) P(OFF) }

                const Type VALUE(bdlt::Time(H, MM, S, MS), OFF);
                Variant    value1, value2;

                if (OFF) {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value1,
                                                     &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP_ASSERT(LINE, value1.is<bdlt::TimeTz>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::TimeTz>());
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value2,
                                                     &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP_ASSERT(LINE, value2.is<bdlt::TimeTz>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::TimeTz>());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting variant of 'bdlt::Datetime'"
                               << " and 'bdlt::DatetimeTz'." << bsl::endl;
        {
            typedef bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz> Variant;

            typedef bdlt::Datetime Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;

                if (veryVerbose) {
                       T_ P_(LINE) P_(Y) P_(M) P_(D) P_(H) P_(MM) P_(S) P(MS) }

                const Type VALUE(Y, M, D, H, MM, S, MS);
                Variant    value1, value2;

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = osb.length();

                    if (LENGTH > 6) {
                        // Datetime objects having length greater that 6 bytes
                        // are always encoded with a time zone.

                        continue;
                    }

                    if (veryVerbose) {
                        P(LINE)
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value1,
                                                     &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP_ASSERT(LINE, value1.is<bdlt::Datetime>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::Datetime>());
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value2,
                                                     &numBytesConsumed));
                    ASSERT(0      == isb.length());
                    ASSERT(LENGTH == numBytesConsumed);
                    LOOP_ASSERT(LINE, value2.is<bdlt::Datetime>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::Datetime>());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::DatetimeTz'." << bsl::endl;
        {
            typedef bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz> Variant;

            typedef bdlt::DatetimeTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;
                const int OFF  = DATA[i].d_tzoffset;

                if (veryVerbose) {
                          T_ P_(Y) P_(M) P_(D) P_(H) P_(MM) P_(S) P_(MS) P(OFF)
                }

                const Type VALUE(bdlt::Datetime(Y, M, D, H, MM, S, MS), OFF);
                Variant    value1, value2;

                if (OFF) {

                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value1,
                                                     &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP_ASSERT(LINE, value1.is<bdlt::DatetimeTz>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::DatetimeTz>());
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value2,
                                                     &numBytesConsumed));
                    ASSERT(0      == isb.length());
                    ASSERT(LENGTH == numBytesConsumed);
                    LOOP_ASSERT(LINE, value2.is<bdlt::DatetimeTz>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::DatetimeTz>());
                }
            }
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' FOR DATE/TIME COMPONENTS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\n" "TESTING 'putValue' FOR DATE/TIME" "\n"
                                       "================================" "\n";

        if (verbose) bsl::cout << "\nTesting Date." << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                bool        d_useBinary; // whether to use binary format
                const char *d_exp;       // expected output
            } DATA[] = {
  //------------^
  //line  year   month   day   opt  exp
  //----  -----  -----   ---   ---  ---
  {   L_, 2020,      1,    1,    1, "01 00"                       },
  {   L_, 2020,      1,    1,    0, "0a 323032 302d3031 2d3031"   },

  {   L_, 2020,      1,    2,    1, "01 01"                       },
  {   L_, 2020,      1,    2,    0, "0a 323032 302d3031 2d3032"   },

  {   L_, 2020,      1,   31,    1, "01 1E"                       },
  {   L_, 2020,      1,   31,    0, "0a 323032 302d3031 2d3331"   },

  {   L_, 2020,      2,    1,    1, "01 1F"                       },
  {   L_, 2020,      2,    1,    0, "0a 323032 302d3032 2d3031"   },

  {   L_, 2020,      2,   28,    1, "01 3A"                       },
  {   L_, 2020,      2,   28,    0, "0a 323032 302d3032 2d3238"   },

  {   L_, 2020,      2,   29,    1, "01 3B"                       },
  {   L_, 2020,      2,   29,    0, "0a 323032 302d3032 2d3239"   },

  {   L_, 2020,      3,    1,    1, "01 3C"                       },
  {   L_, 2020,      3,    1,    0, "0a 323032 302d3033 2d3031"   },

  {   L_, 2020,      5,    7,    1, "01 7F"                       },
  {   L_, 2020,      5,    7,    0, "0a 323032 302d3035 2d3037"   },

  {   L_, 2020,      5,    8,    1, "02 0080"                     },
  {   L_, 2020,      5,    8,    0, "0a 323032 302d3035 2d3038"   },

  {   L_, 2020,     12,   31,    1, "02 016D"                     },
  {   L_, 2020,     12,   31,    0, "0a 323032 302d3132 2d3331"   },

  {   L_, 2021,      1,    1,    1, "02 016E"                     },
  {   L_, 2021,      1,    1,    0, "0a 323032 312d3031 2d3031"   },

  {   L_, 2023,     12,   31,    1, "02 05B4"                     },
  {   L_, 2023,     12,   31,    0, "0a 323032 332d3132 2d3331"   },

  {   L_, 2024,      1,    1,    1, "02 05B5"                     },
  {   L_, 2024,      1,    1,    0, "0a 323032 342d3031 2d3031"   },

  {   L_, 2099,     12,   31,    1, "02 7223"                     },
  {   L_, 2099,     12,   31,    0, "0a 323039 392d3132 2d3331"   },

  {   L_, 2100,      1,    1,    1, "02 7224"                     },
  {   L_, 2100,      1,    1,    0, "0a 323130 302d3031 2d3031"   },

  {   L_, 2100,      2,   28,    1, "02 725E"                     },
  {   L_, 2100,      2,   28,    0, "0a 323130 302d3032 2d3238"   },

  {   L_, 2100,      3,    1,    1, "02 725F"                     },
  {   L_, 2100,      3,    1,    0, "0a 323130 302d3033 2d3031"   },

  {   L_, 2109,      9,   18,    1, "02 7FFF"                     },
  {   L_, 2109,      9,   18,    0, "0a 323130 392d3039 2d3138"   },

  {   L_, 2109,      9,   19,    1, "03 008000"                   },
  {   L_, 2109,      9,   19,    0, "0a 323130 392d3039 2d3139"   },

  {   L_, 2119,     12,   31,    1, "03 008EAB"                   },
  {   L_, 2119,     12,   31,    0, "0a 323131 392d3132 2d3331"   },

  {   L_, 2120,      1,    1,    1, "03 008EAC"                   },
  {   L_, 2120,      1,    1,    0, "0a 323132 302d3031 2d3031"   },

  {   L_, 2019,     12,   31,    1, "01 FF"                       },
  {   L_, 2019,     12,   31,    0, "0a 323031 392d3132 2d3331"   },

  {   L_, 2019,      8,   26,    1, "01 80"                       },
  {   L_, 2019,      8,   26,    0, "0a 323031 392d3038 2d3236"   },

  {   L_, 2019,      8,   25,    1, "02 FF7F"                     },
  {   L_, 2019,      8,   25,    0, "0a 323031 392d3038 2d3235"   },

  {   L_, 2016,      2,   28,    1, "02 FA85"                     },
  {   L_, 2016,      2,   28,    0, "0a 323031 362d3032 2d3238"   },

  {   L_, 2016,      2,   29,    1, "02 FA86"                     },
  {   L_, 2016,      2,   29,    0, "0a 323031 362d3032 2d3239"   },

  {   L_, 2016,      3,    1,    1, "02 FA87"                     },
  {   L_, 2016,      3,    1,    0, "0a 323031 362d3033 2d3031"   },

  {   L_, 2012,      2,   28,    1, "02 F4D0"                     },
  {   L_, 2012,      2,   28,    0, "0a 323031 322d3032 2d3238"   },

  {   L_, 2012,      2,   29,    1, "02 F4D1"                     },
  {   L_, 2012,      2,   29,    0, "0a 323031 322d3032 2d3239"   },

  {   L_, 2012,      3,    1,    1, "02 F4D2"                     },
  {   L_, 2012,      3,    1,    0, "0a 323031 322d3033 2d3031"   },

  {   L_, 2000,      2,   28,    1, "02 E3B1"                     },
  {   L_, 2000,      2,   28,    0, "0a 323030 302d3032 2d3238"   },

  {   L_, 2000,      2,   29,    1, "02 E3B2"                     },
  {   L_, 2000,      2,   29,    0, "0a 323030 302d3032 2d3239"   },

  {   L_, 2000,      3,    1,    1, "02 E3B3"                     },
  {   L_, 2000,      3,    1,    0, "0a 323030 302d3033 2d3031"   },

  {   L_, 1930,      4,   15,    1, "02 8000"                     },
  {   L_, 1930,      4,   15,    0, "0a 313933 302d3034 2d3135"   },

  {   L_, 1930,      4,   14,    1, "03 FF7FFF"                   },
  {   L_, 1930,      4,   14,    0, "0a 313933 302d3034 2d3134"   },

  {   L_, 1066,     10,   14,    1, "03 FAB005"                   },
  {   L_, 1066,     10,   14,    0, "0a 313036 362d3130 2d3134"   },

  {   L_, 9999,     12,   31,    1, "03 2C794A"                   },
  {   L_, 9999,     12,   31,    0, "0a 393939 392d3132 2d3331"   },

  {   L_,    1,      1,    1,    1, "03 F4BF70"                   },
  {   L_,    1,      1,    1,    0, "0a 303030 312d3031 2d3031"   },
  //------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE  = DATA[i].d_lineNum;
                const int   YEAR  = DATA[i].d_year;
                const int   MONTH = DATA[i].d_month;
                const int   DAY   = DATA[i].d_day;
                const bool  BIN   = DATA[i].d_useBinary;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                if (veryVerbose) {
                           T_ P_(YEAR) P_(MONTH) P_(DAY) P_(BIN) P_(EXP) P(LEN)
                }

                LOOP_ASSERT(LINE, bdlt::Date::isValidYearMonthDay(YEAR,
                                                                  MONTH,
                                                                  DAY));

                if (veryVerbose) { P_(YEAR) P_(MONTH) P_(DAY) P(EXP) }

                balber::BerEncoderOptions options;
                options.setEncodeDateAndTimeTypesAsBinary(BIN);

                const bdlt::Date VALUE(YEAR, MONTH, DAY);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                LOOP2_ASSERT(LEN, (int)osb.length(), LEN == (int)osb.length());
                LOOP2_ASSERT(osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                bdlt::Date value;
                int numBytesConsumed = 0;

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS == Util::getValue(&isb,
                                                 &value,
                                                 &numBytesConsumed));
                ASSERT(0   == isb.length());
                ASSERT(LEN == numBytesConsumed);
                LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
            }
        }

        if (verbose) bsl::cout << "\nTesting DateTz." << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                int         d_tzoffset;  // time zone offset
                bool        d_useBinary; // whether to use binary format
                const char *d_exp;       // expected output
            } DATA[] = {
  //------------^
  //line  year   month   day   off   opt  exp
  //----  -----  -----   ---   ---   ---  ---
  {   L_, 2020,      1,    1,    0,    1, "01 00"                            },
  {   L_, 2020,      1,    1,    0,    0,
                                 "10 323032 302d3031 2d30312B 30303A30 30"   },

  {   L_, 2020,      1,    1, 1439,    1, "04 059F0000"                      },
  {   L_, 2020,      1,    1, 1439,    0,
                                 "10 323032 302d3031 2d30312B 32333A35 39"   },

  {   L_, 2020,      1,    1,-1439,    1, "04 FA610000"                      },
  {   L_, 2020,      1,    1,-1439,    0,
                                 "10 323032 302d3031 2d30312D 32333A35 39"   },

  {   L_, 2020,      1,    2,    0,    1, "01 01"                            },
  {   L_, 2020,      1,    2,    0,    0,
                                 "10 323032 302d3031 2d30322B 30303A30 30"   },

  {   L_, 2020,      1,    2, 1439,    1, "04 059F0001"                      },
  {   L_, 2020,      1,    2, 1439,    0,
                                 "10 323032 302d3031 2d30322B 32333A35 39"   },

  {   L_, 2020,      1,    2,-1439,    1, "04 FA610001"                      },
  {   L_, 2020,      1,    2,-1439,    0,
                                 "10 323032 302d3031 2d30322D 32333A35 39"   },

  {   L_, 2019,     12,   31,    0,    1, "01 FF"                            },
  {   L_, 2019,     12,   31,    0,    0,
                                 "10 323031 392d3132 2d33312B 30303A30 30"   },

  {   L_, 2019,     12,   31, 1439,    1, "04 059FFFFF"                      },
  {   L_, 2019,     12,   31, 1439,    0,
                                 "10 323031 392d3132 2d33312B 32333A35 39"   },

  {   L_, 2019,     12,   31,-1439,    1, "04 FA61FFFF"                      },
  {   L_, 2019,     12,   31,-1439,    0,
                                 "10 323031 392d3132 2d33312D 32333A35 39"   },

  {   L_,    1,      1,    1,    0,    1, "03 F4BF70"                        },
  {   L_,    1,      1,    1,    0,    0,
                                 "10 303030 312d3031 2d30312B 30303A30 30"   },

  {   L_,    1,      1,    1, 1439,    1, "05 059FF4BF 70"                   },
  {   L_,    1,      1,    1, 1439,    0,
                                 "10 303030 312d3031 2d30312B 32333A35 39"   },

  {   L_,    1,      1,    1,-1439,    1, "05 FA61F4BF 70"                   },
  {   L_,    1,      1,    1,-1439,    0,
                                 "10 303030 312d3031 2d30312D 32333A35 39"   },

  {   L_, 2020,      5,    7,    0,    1, "01 7F"                            },
  {   L_, 2020,      5,    7,    0,    0,
                                 "10 323032 302d3035 2d30372B 30303A30 30"   },

  {   L_, 2020,      5,    7, 1439,    1, "04 059F007F"                      },
  {   L_, 2020,      5,    7, 1439,    0,
                                 "10 323032 302d3035 2d30372B 32333A35 39"   },

  {   L_, 2020,      5,    7,-1439,    1, "04 FA61007F"                      },
  {   L_, 2020,      5,    7,-1439,    0,
                                 "10 323032 302d3035 2d30372D 32333A35 39"   },

  {   L_, 2020,      5,    8,    0,    1, "02 0080"                          },
  {   L_, 2020,      5,    8,    0,    0,
                                 "10 323032 302d3035 2d30382B 30303A30 30"   },

  {   L_, 2020,      5,    8, 1439,    1, "04 059F0080"                      },
  {   L_, 2020,      5,    8, 1439,    0,
                                 "10 323032 302d3035 2d30382B 32333A35 39"   },

  {   L_, 2020,      5,    8,-1439,    1, "04 FA610080"                      },
  {   L_, 2020,      5,    8,-1439,    0,
                                 "10 323032 302d3035 2d30382D 32333A35 39"   },

  {   L_, 2109,      9,   18,    0,    1, "02 7FFF"                          },
  {   L_, 2109,      9,   18,    0,    0,
                                 "10 323130 392d3039 2d31382B 30303A30 30"   },

  {   L_, 2109,      9,   18, 1439,    1, "04 059F7FFF"                      },
  {   L_, 2109,      9,   18, 1439,    0,
                                 "10 323130 392d3039 2d31382B 32333A35 39"   },

  {   L_, 2109,      9,   18,-1439,    1, "04 FA617FFF"                      },
  {   L_, 2109,      9,   18,-1439,    0,
                                 "10 323130 392d3039 2d31382D 32333A35 39"   },

  {   L_, 2109,      9,   19,    0,    1, "03 008000"                        },
  {   L_, 2109,      9,   19,    0,    0,
                                 "10 323130 392d3039 2d31392B 30303A30 30"   },

  {   L_, 2109,      9,   19, 1439,    1, "05 059F008000"                    },
  {   L_, 2109,      9,   19, 1439,    0,
                                 "10 323130 392d3039 2d31392B 32333A35 39"   },

  {   L_, 2109,      9,   19,-1439,    1, "05 FA61008000"                    },
  {   L_, 2109,      9,   19,-1439,    0,
                                 "10 323130 392d3039 2d31392D 32333A35 39"   },

  {   L_, 2019,      8,   26,    0,    1, "01 80"                            },
  {   L_, 2019,      8,   26,    0,    0,
                                 "10 323031 392d3038 2d32362B 30303A30 30"   },

  {   L_, 2019,      8,   26, 1439,    1, "04 059FFF80"                      },
  {   L_, 2019,      8,   26, 1439,    0,
                                 "10 323031 392d3038 2d32362B 32333A35 39"   },

  {   L_, 2019,      8,   26,-1439,    1, "04 FA61FF80"                      },
  {   L_, 2019,      8,   26,-1439,    0,
                                 "10 323031 392d3038 2d32362D 32333A35 39"   },

  {   L_, 2019,      8,   25,    0,    1, "02 FF7F"                          },
  {   L_, 2019,      8,   25,    0,    0,
                                 "10 323031 392d3038 2d32352B 30303A30 30"   },

  {   L_, 2019,      8,   25, 1439,    1, "04 059FFF7F"                      },
  {   L_, 2019,      8,   25, 1439,    0,
                                 "10 323031 392d3038 2d32352B 32333A35 39"   },

  {   L_, 2019,      8,   25,-1439,    1, "04 FA61FF7F"                      },
  {   L_, 2019,      8,   25,-1439,    0,
                                 "10 323031 392d3038 2d32352D 32333A35 39"   },

  {   L_, 1930,      4,   15,    0,    1, "02 8000"                          },
  {   L_, 1930,      4,   15,    0,    0,
                                 "10 313933 302d3034 2d31352B 30303A30 30"   },

  {   L_, 1930,      4,   15, 1439,    1, "04 059F8000"                      },
  {   L_, 1930,      4,   15, 1439,    0,
                                 "10 313933 302d3034 2d31352B 32333A35 39"   },

  {   L_, 1930,      4,   15,-1439,    1, "04 FA618000"                      },
  {   L_, 1930,      4,   15,-1439,    0,
                                 "10 313933 302d3034 2d31352D 32333A35 39"   },

  {   L_, 1930,      4,   14,    0,    1, "03 FF7FFF"                        },
  {   L_, 1930,      4,   14,    0,    0,
                                 "10 313933 302d3034 2d31342B 30303A30 30"   },

  {   L_, 1930,      4,   14, 1439,    1, "05 059FFF7FFF"                    },
  {   L_, 1930,      4,   14, 1439,    0,
                                 "10 313933 302d3034 2d31342B 32333A35 39"   },

  {   L_, 1930,      4,   14,-1439,    1, "05 FA61FF7FFF"                    },
  {   L_, 1930,      4,   14,-1439,    0,
                                 "10 313933 302d3034 2d31342D 32333A35 39"   },

  {   L_, 1066,     10,   14,    0,    1, "03 FAB005"                        },
  {   L_, 1066,     10,   14,    0,    0,
                                 "10 313036 362d3130 2d31342B 30303A30 30"   },

  {   L_, 1066,     10,   14, 1439,    1, "05 059FFAB0 05"                   },
  {   L_, 1066,     10,   14, 1439,    0,
                                 "10 313036 362d3130 2d31342B 32333A35 39"   },

  {   L_, 1066,     10,   14,-1439,    1, "05 FA61FAB0 05"                   },
  {   L_, 1066,     10,   14,-1439,    0,
                                 "10 313036 362d3130 2d31342D 32333A35 39"   },

  {   L_, 9999,     12,   31,    0,    1, "03 2C794A"                        },
  {   L_, 9999,     12,   31,    0,    0,
                                 "10 393939 392d3132 2d33312B 30303A30 30"   },

  {   L_, 9999,     12,   31, 1439,    1, "05 059F2C79 4A"                   },
  {   L_, 9999,     12,   31, 1439,    0,
                                 "10 393939 392d3132 2d33312B 32333A35 39"   },

  {   L_, 9999,     12,   31,-1439,    1, "05 FA612C79 4A"                   },
  {   L_, 9999,     12,   31,-1439,    0,
                                 "10 393939 392d3132 2d33312D 32333A35 39"   },
  //------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_lineNum;
                const int   YEAR  = DATA[i].d_year;
                const int   MONTH = DATA[i].d_month;
                const int   DAY   = DATA[i].d_day;
                const int   OFF   = DATA[i].d_tzoffset;
                const bool  BIN   = DATA[i].d_useBinary;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                ASSERT(bdlt::Date::isValidYearMonthDay(YEAR, MONTH, DAY));

                if (veryVerbose) { P_(YEAR) P_(MONTH) P_(DAY) P_(OFF) P(EXP) }

                balber::BerEncoderOptions options;
                options.setEncodeDateAndTimeTypesAsBinary(BIN);

                const bdlt::DateTz VALUE(bdlt::Date(YEAR, MONTH, DAY), OFF);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                LOOP2_ASSERT(LEN, (int)osb.length(), LEN == (int)osb.length());
                LOOP2_ASSERT(osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                bdlt::DateTz value;
                int numBytesConsumed = 0;

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS == Util::getValue(&isb,
                                                 &value,
                                                 &numBytesConsumed));
                ASSERT(0   == isb.length());
                ASSERT(LEN == numBytesConsumed);
                LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
            }
        }

        if (verbose) bsl::cout << "\nTesting Time." << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_hour;      // hour under test
                int         d_min;       // min under test
                int         d_sec;       // sec under test
                int         d_milliSec;  // milliSec under test
                bool        d_useBinary; // whether to use binary format
                const char *d_exp;       // expected output
            } DATA[] = {
  //------------^
  //line  hour   min  sec  ms   opt  exp
  //----  -----  ---  ---  ---  ---  ---
  {   L_,     0,   0,   0,   0,   1, "01 00"                                 },
  {   L_,     0,   0,   0,   0,   0, "0C 30303A30 303A3030 2E303030"         },

  {   L_,     0,   0,   0, 127,   1, "01 7F"                                 },
  {   L_,     0,   0,   0, 127,   0, "0C 30303A30 303A3030 2E313237"         },

  {   L_,     0,   0,   0, 128,   1, "02 0080"                               },
  {   L_,     0,   0,   0, 128,   0, "0C 30303A30 303A3030 2E313238"         },

  {   L_,     0,   0,   0, 255,   1, "02 00FF"                               },
  {   L_,     0,   0,   0, 255,   0, "0C 30303A30 303A3030 2E323535"         },

  {   L_,     0,   0,   0, 256,   1, "02 0100"                               },
  {   L_,     0,   0,   0, 256,   0, "0C 30303A30 303A3030 2E323536"         },

  {   L_,     0,   0,  32, 767,   1, "02 7FFF"                               },
  {   L_,     0,   0,  32, 767,   0, "0C 30303A30 303A3332 2E373637"         },

  {   L_,     0,   0,  32, 768,   1, "03 008000"                             },
  {   L_,     0,   0,  32, 768,   0, "0C 30303A30 303A3332 2E373638"         },

  {   L_,     2,  19,  48, 607,   1, "03 7FFFFF"                             },
  {   L_,     2,  19,  48, 607,   0, "0C 30323A31 393A3438 2E363037"         },

  {   L_,     2,  19,  48, 608,   1, "04 00800000"                           },
  {   L_,     2,  19,  48, 608,   0, "0C 30323A31 393A3438 2E363038"         },

  {   L_,    12,  33,  45, 999,   1, "04 02B2180F"                           },
  {   L_,    12,  33,  45, 999,   0, "0C 31323A33 333A3435 2E393939"         },

  {   L_,    23,  59,  59, 999,   1, "04 05265BFF"                           },
  {   L_,    23,  59,  59, 999,   0, "0C 32333A35 393A3539 2E393939"         },

  {   L_,    24,   0,   0,   0,   1, "01 00"                                 },
// TBD: Current doesnt work
// {  L_,    24,   0,   0,   0,   0, "0C 30303A30 303A3030 2E303030"         },
  //------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE  = DATA[i].d_lineNum;
                const int   HOUR  = DATA[i].d_hour;
                const int   MIN   = DATA[i].d_min;
                const int   SECS  = DATA[i].d_sec;
                const int   MSEC  = DATA[i].d_milliSec;
                const bool  BIN   = DATA[i].d_useBinary;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                if (veryVerbose) { P_(HOUR) P_(MIN) P_(SECS) P(MSEC) }

                balber::BerEncoderOptions options;
                options.setEncodeDateAndTimeTypesAsBinary(BIN);

                const bdlt::Time VALUE(HOUR, MIN, SECS, MSEC);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                LOOP2_ASSERT(LEN, (int)osb.length(), LEN == (int)osb.length());
                LOOP2_ASSERT(osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                bdlt::Time value;
                int numBytesConsumed = 0;

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS == Util::getValue(&isb,
                                                 &value,
                                                 &numBytesConsumed));
                ASSERT(0   == isb.length());
                ASSERT(LEN == numBytesConsumed);
                if (24 == HOUR) {
                    LOOP2_ASSERT(LINE, value, bdlt::Time(0) == value);
                }
                else {
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting TimeTz." << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_hour;      // hour under test
                int         d_min;       // min under test
                int         d_sec;       // sec under test
                int         d_milliSec;  // milliSec under test
                int         d_offset;    // offset
                bool        d_useBinary; // whether to use binary format
                const char *d_exp;       // expected output
            } DATA[] = {
  //------------^
  //line  hour   min  sec  ms     off  opt  exp
  //----  -----  ---  ---  ---    ---  ---  ---
  {   L_,     0,   0,   0,   0,     0,  1, "01 00"                           },
  {   L_,     0,   0,   0,   0,     0,  0,
                               "12 30303A30 303A3030 2E303030 2B30303A 3030" },

  {   L_,     0,   0,   0,   0,  1439,  1, "05 059F0000 00"                  },
  {   L_,     0,   0,   0,   0,  1439,  0,
                               "12 30303A30 303A3030 2E303030 2B32333A 3539" },

  {   L_,     0,   0,   0,   0, -1439,  1, "05 FA610000 00"                  },
  {   L_,     0,   0,   0,   0, -1439,  0,
                               "12 30303A30 303A3030 2E303030 2D32333A 3539" },

  {   L_,     0,   0,   0, 127,     0,  1, "01 7F"                           },
  {   L_,     0,   0,   0, 127,     0,  0,
                               "12 30303A30 303A3030 2E313237 2B30303A 3030" },

  {   L_,     0,   0,   0, 127,  1439,  1, "05 059F0000 7F"                  },
  {   L_,     0,   0,   0, 127,  1439,  0,
                               "12 30303A30 303A3030 2E313237 2B32333A 3539" },

  {   L_,     0,   0,   0, 127, -1439,  1, "05 FA610000 7F"                  },
  {   L_,     0,   0,   0, 127, -1439,  0,
                               "12 30303A30 303A3030 2E313237 2D32333A 3539" },

  {   L_,     0,   0,   0, 128,     0,  1, "02 0080"                         },
  {   L_,     0,   0,   0, 128,     0,  0,
                               "12 30303A30 303A3030 2E313238 2B30303A 3030" },

  {   L_,     0,   0,   0, 128,  1439,  1, "05 059F0000 80"                  },
  {   L_,     0,   0,   0, 128,  1439,  0,
                               "12 30303A30 303A3030 2E313238 2B32333A 3539" },

  {   L_,     0,   0,   0, 128, -1439,  1, "05 FA610000 80"                  },
  {   L_,     0,   0,   0, 128, -1439,  0,
                               "12 30303A30 303A3030 2E313238 2D32333A 3539" },

  {   L_,     0,   0,  32, 767,     0,  1, "02 7FFF"                         },
  {   L_,     0,   0,  32, 767,     0,  0,
                               "12 30303A30 303A3332 2E373637 2B30303A 3030" },

  {   L_,     0,   0,  32, 767,  1439,  1, "05 059F007F FF"                  },
  {   L_,     0,   0,  32, 767,  1439,  0,
                               "12 30303A30 303A3332 2E373637 2B32333A 3539" },

  {   L_,     0,   0,  32, 767, -1439,  1, "05 FA61007F FF"                  },
  {   L_,     0,   0,  32, 767, -1439,  0,
                               "12 30303A30 303A3332 2E373637 2D32333A 3539" },

  {   L_,     0,   0,  32, 768,     0,  1, "03 008000"                       },
  {   L_,     0,   0,  32, 768,     0,  0,
                               "12 30303A30 303A3332 2E373638 2B30303A 3030" },

  {   L_,     0,   0,  32, 768,  1439,  1, "05 059F0080 00"                  },
  {   L_,     0,   0,  32, 768,  1439,  0,
                               "12 30303A30 303A3332 2E373638 2B32333A 3539" },

  {   L_,     0,   0,  32, 768, -1439,  1, "05 FA610080 00"                  },
  {   L_,     0,   0,  32, 768, -1439,  0,
                               "12 30303A30 303A3332 2E373638 2D32333A 3539" },

  {   L_,     2,  19,  48, 607,     0,  1, "03 7FFFFF"                       },
  {   L_,     2,  19,  48, 607,     0,  0,
                               "12 30323A31 393A3438 2E363037 2B30303A 3030" },

  {   L_,     2,  19,  48, 607,  1439,  1, "05 059F7FFFFF"                   },
  {   L_,     2,  19,  48, 607,  1439,  0,
                               "12 30323A31 393A3438 2E363037 2B32333A 3539" },

  {   L_,     2,  19,  48, 607, -1439,  1, "05 FA617FFFFF"                   },
  {   L_,     2,  19,  48, 607, -1439,  0,
                               "12 30323A31 393A3438 2E363037 2D32333A 3539" },

  {   L_,     2,  19,  48, 608,     0,  1, "04 00800000"                     },
  {   L_,     2,  19,  48, 608,     0,  0,
                               "12 30323A31 393A3438 2E363038 2B30303A 3030" },

  {   L_,     2,  19,  48, 608,  1439,  1, "06 059F0080 0000"                },
  {   L_,     2,  19,  48, 608,  1439,  0,
                               "12 30323A31 393A3438 2E363038 2B32333A 3539" },

  {   L_,     2,  19,  48, 608, -1439,  1, "06 FA610080 0000"                },
  {   L_,     2,  19,  48, 608, -1439,  0,
                               "12 30323A31 393A3438 2E363038 2D32333A 3539" },

  {   L_,    12,  33,  45, 999,     0,  1, "04 02B2180F"                     },
  {   L_,    12,  33,  45, 999,     0,  0,
                               "12 31323A33 333A3435 2E393939 2B30303A 3030" },

  {   L_,    12,  33,  45, 999,  1439,  1, "06 059F02B2 180F"                },
  {   L_,    12,  33,  45, 999,  1439,  0,
                               "12 31323A33 333A3435 2E393939 2B32333A 3539" },

  {   L_,    12,  33,  45, 999, -1439,  1, "06 FA6102B2 180F"                },
  {   L_,    12,  33,  45, 999, -1439,  0,
                               "12 31323A33 333A3435 2E393939 2D32333A 3539" },

  {   L_,    23,  59,  59, 999,     0,  1, "04 05265BFF"                     },
  {   L_,    23,  59,  59, 999,     0,  0,
                               "12 32333A35 393A3539 2E393939 2B30303A 3030" },

  {   L_,    23,  59,  59, 999,  1439,  1, "06 059F0526 5BFF"                },
  {   L_,    23,  59,  59, 999,  1439,  0,
                               "12 32333A35 393A3539 2E393939 2B32333A 3539" },

  {   L_,    23,  59,  59, 999, -1439,  1, "06 FA610526 5BFF"                },
  {   L_,    23,  59,  59, 999, -1439,  0,
                               "12 32333A35 393A3539 2E393939 2D32333A 3539" },

  {   L_,    24,   0,   0,   0,     0,  1, "01 00"                           },
// TBD: Current doesnt work
//{   L_,    24,   0,   0,   0,   0, "0C 30303A30 303A3030 2E303030"         },
  //------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE  = DATA[i].d_lineNum;
                const int   HOUR  = DATA[i].d_hour;
                const int   MIN   = DATA[i].d_min;
                const int   SECS  = DATA[i].d_sec;
                const int   MSEC  = DATA[i].d_milliSec;
                const int   OFF   = DATA[i].d_offset;
                const bool  BIN   = DATA[i].d_useBinary;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                if (veryVerbose) { P_(HOUR) P_(MIN) P_(SECS) P_(MSEC) P(OFF) }

                balber::BerEncoderOptions options;
                options.setEncodeDateAndTimeTypesAsBinary(BIN);

                const bdlt::TimeTz VALUE(bdlt::Time(HOUR, MIN, SECS, MSEC),
                                         OFF);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                LOOP2_ASSERT(LEN, (int)osb.length(), LEN == (int)osb.length());
                LOOP2_ASSERT(osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                bdlt::TimeTz value;
                int numBytesConsumed = 0;

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS == Util::getValue(&isb,
                                                 &value,
                                                 &numBytesConsumed));
                ASSERT(0   == isb.length());
                ASSERT(LEN == numBytesConsumed);
                if (24 == HOUR) {
                    LOOP2_ASSERT(LINE, value,
                                bdlt::TimeTz(bdlt::Time(0), OFF) == value);
                }
                else {
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting Datetime." << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                int         d_hour;      // hour under test
                int         d_min;       // min under test
                int         d_sec;       // sec under test
                int         d_milliSec;  // milliSec under test
                bool        d_useBinary; // whether to use binary format
                const char *d_exp;       // expected output
            } DATA[] = {
  //------------^
  //line  year  mon  day  hour   min  sec    ms    opt  exp
  //----  ----- ---  ---  ----   ---  ---    --    ---  ---
  {   L_, 2020,   1,   1,    0,    0,   0,    0,     1,
                                               "01 00"                       },
  {   L_, 2020,   1,   1,    0,    0,   0,    0,     0,
                    "17 32303230 2d30312d 30315430 303A3030 3A30302E 303030" },

  {   L_, 2020,   1,   1,    0,    0,   0,    1,     1,
                                               "01 01"                       },
  {   L_, 2020,   1,   1,    0,    0,   0,    1,     0,
                    "17 32303230 2d30312d 30315430 303A3030 3A30302E 303031" },

  {   L_, 2020,   1,   1,    0,    0,   0,  127,     1,
                                               "01 7F"                       },
  {   L_, 2020,   1,   1,    0,    0,   0,  127,     0,
                    "17 32303230 2d30312d 30315430 303A3030 3A30302E 313237" },

  {   L_, 2020,   1,   1,    0,    0,   0,  128,     1,
                                               "02 0080"                     },
  {   L_, 2020,   1,   1,    0,    0,   0,  128,     0,
                    "17 32303230 2d30312d 30315430 303A3030 3A30302E 313238" },

  {   L_, 2020,   1,   1,    0,    0,   0,  999,     1,
                                               "02 03E7"                     },
  {   L_, 2020,   1,   1,    0,    0,   0,  999,     0,
                    "17 32303230 2d30312d 30315430 303A3030 3A30302E 393939" },

  {   L_, 2020,   1,   1,    0,    0,   1,    0,     1,
                                               "02 03E8"                     },
  {   L_, 2020,   1,   1,    0,    0,   1,    0,     0,
                    "17 32303230 2d30312d 30315430 303A3030 3A30312E 303030" },

  {   L_, 2020,   1,   1,    0,    0,   1,    1,     1,
                                               "02 03E9"                     },
  {   L_, 2020,   1,   1,    0,    0,   1,    1,     0,
                    "17 32303230 2d30312d 30315430 303A3030 3A30312E 303031" },

  {   L_, 2020,   1,   1,    0,    0,  32,  767,     1,
                                               "02 7FFF"                     },
  {   L_, 2020,   1,   1,    0,    0,  32,  767,     0,
                    "17 32303230 2d30312d 30315430 303A3030 3A33322E 373637" },

  {   L_, 2020,   1,   1,    0,    0,  32,  768,     1,
                                               "03 008000"                   },
  {   L_, 2020,   1,   1,    0,    0,  32,  768,     0,
                    "17 32303230 2d30312d 30315430 303A3030 3A33322E 373638" },

  {   L_, 2020,   1,   1,    0,    0,  59,  999,     1,
                                               "03 00EA5F"                   },
  {   L_, 2020,   1,   1,    0,    0,  59,  999,     0,
                    "17 32303230 2d30312d 30315430 303A3030 3A35392E 393939" },

  {   L_, 2020,   1,   1,    0,    1,   0,    0,     1,
                                               "03 00EA60"                   },
  {   L_, 2020,   1,   1,    0,    1,   0,    0,     0,
                    "17 32303230 2d30312d 30315430 303A3031 3A30302E 303030" },

  {   L_, 2020,   1,   1,    0,    1,   0,    1,     1,
                                               "03 00EA61"                   },
  {   L_, 2020,   1,   1,    0,    1,   0,    1,     0,
                    "17 32303230 2d30312d 30315430 303A3031 3A30302E 303031" },

  {   L_, 2020,   1,   1,    0,   59,  59,  999,     1,
                                               "03 36EE7F"                   },
  {   L_, 2020,   1,   1,    0,   59,  59,  999,     0,
                    "17 32303230 2d30312d 30315430 303A3539 3A35392E 393939" },

  {   L_, 2020,   1,   1,    1,    0,   0,    0,     1,
                                               "03 36EE80"                   },
  {   L_, 2020,   1,   1,    1,    0,   0,    0,     0,
                    "17 32303230 2d30312d 30315430 313A3030 3A30302E 303030" },

  {   L_, 2020,   1,   1,    1,    0,   0,    1,     1,
                                               "03 36EE81"                   },
  {   L_, 2020,   1,   1,    1,    0,   0,    1,     0,
                    "17 32303230 2d30312d 30315430 313A3030 3A30302E 303031" },

  {   L_, 2020,   1,   1,    2,   19,  48,  607,     1,
                                               "03 7FFFFF"                   },
  {   L_, 2020,   1,   1,    2,   19,  48,  607,     0,
                    "17 32303230 2d30312d 30315430 323A3139 3A34382E 363037" },

  {   L_, 2020,   1,   1,    2,   19,  48,  608,     1,
                                               "04 00800000"                 },
  {   L_, 2020,   1,   1,    2,   19,  48,  608,     0,
                    "17 32303230 2d30312d 30315430 323A3139 3A34382E 363038" },

  {   L_, 2020,   1,   1,   23,   59,  59,  999,     1,
                                               "04 05265BFF"                 },
  {   L_, 2020,   1,   1,   23,   59,  59,  999,     0,
                    "17 32303230 2d30312d 30315432 333A3539 3A35392E 393939" },

  {   L_, 2020,   1,   2,    0,    0,   0,    0,     1,
                                               "04 05265C00"                 },
  {   L_, 2020,   1,   2,    0,    0,   0,    0,     0,
                    "17 32303230 2d30312d 30325430 303A3030 3A30302E 303030" },

  {   L_, 2020,   1,   2,    0,    0,   0,    1,     1,
                                               "04 05265C01"                 },
  {   L_, 2020,   1,   2,    0,    0,   0,    1,     0,
                    "17 32303230 2d30312d 30325430 303A3030 3A30302E 303031" },

  {   L_, 2020,   1,  25,   20,   31,  23,  647,     1,
                                               "04 7FFFFFFF"                 },
  {   L_, 2020,   1,  25,   20,   31,  23,  647,     0,
                    "17 32303230 2d30312d 32355432 303A3331 3A32332E 363437" },

  {   L_, 2020,   1,  25,   20,   31,  23,  648,     1,
                                               "05 00800000 00"              },
  {   L_, 2020,   1,  25,   20,   31,  23,  648,     0,
                    "17 32303230 2d30312d 32355432 303A3331 3A32332E 363438" },

  {   L_, 2020,   1,  25,   20,   31,  23,  649,     1,
                                               "05 00800000 01"              },
  {   L_, 2020,   1,  25,   20,   31,  23,  649,     0,
                    "17 32303230 2d30312d 32355432 303A3331 3A32332E 363439" },

  {   L_, 2020,   1,  31,   23,   59,  59,  999,     1,
                                               "05 009FA523 FF"              },
  {   L_, 2020,   1,  31,   23,   59,  59,  999,     0,
                    "17 32303230 2d30312d 33315432 333A3539 3A35392E 393939" },

  {   L_, 2020,   2,   1,    0,    0,   0,    0,     1,
                                               "05 009FA524 00"              },
  {   L_, 2020,   2,   1,    0,    0,   0,    0,     0,
                    "17 32303230 2d30322d 30315430 303A3030 3A30302E 303030" },

  {   L_, 2020,   2,   1,    0,    0,   0,    1,     1,
                                               "05 009FA524 01"              },
  {   L_, 2020,   2,   1,    0,    0,   0,    1,     0,
                    "17 32303230 2d30322d 30315430 303A3030 3A30302E 303031" },

  {   L_, 2020,   2,  28,   23,   59,  59,  999,     1,
                                               "05 012FD733 FF"              },
  {   L_, 2020,   2,  28,   23,   59,  59,  999,     0,
                    "17 32303230 2d30322d 32385432 333A3539 3A35392E 393939" },

  {   L_, 2020,   2,  29,    0,    0,   0,    0,     1,
                                               "05 012FD734 00"              },
  {   L_, 2020,   2,  29,    0,    0,   0,    0,     0,
                    "17 32303230 2d30322d 32395430 303A3030 3A30302E 303030" },

  {   L_, 2020,   2,  29,   23,   59,  59,  999,     1,
                                               "05 0134FD8F FF"              },
  {   L_, 2020,   2,  29,   23,   59,  59,  999,     0,
                    "17 32303230 2d30322d 32395432 333A3539 3A35392E 393939" },

  {   L_, 2020,   3,   1,    0,    0,   0,    0,     1,
                                               "05 0134FD90 00"              },
  {   L_, 2020,   3,   1,    0,    0,   0,    0,     0,
                    "17 32303230 2d30332d 30315430 303A3030 3A30302E 303030" },

  {   L_, 2020,  12,  31,   23,   59,  59,  999,     1,
                                               "05 075CD787 FF"              },
  {   L_, 2020,  12,  31,   23,   59,  59,  999,     0,
                    "17 32303230 2d31322d 33315432 333A3539 3A35392E 393939" },

  {   L_, 2021,   1,   1,    0,    0,   0,    0,     1,
                                               "05 075CD788 00"              },
  {   L_, 2021,   1,   1,    0,    0,   0,    0,     0,
                    "17 32303231 2d30312d 30315430 303A3030 3A30302E 303030" },

  {   L_, 2023,  12,  31,   23,   59,  59,  999,     1,
                                               "05 1D63EB0B FF"              },
  {   L_, 2023,  12,  31,   23,   59,  59,  999,     0,
                    "17 32303233 2d31322d 33315432 333A3539 3A35392E 393939" },

  {   L_, 2024,   1,   1,    0,    0,   0,    0,     1,
                                               "05 1D63EB0C 00"              },
  {   L_, 2024,   1,   1,    0,    0,   0,    0,     0,
                    "17 32303234 2d30312d 30315430 303A3030 3A30302E 303030" },

  {   L_, 2024,   1,   1,    0,    0,   0,    1,     1,
                                               "05 1D63EB0C 01"              },
  {   L_, 2024,   1,   1,    0,    0,   0,    1,     0,
                    "17 32303234 2d30312d 30315430 303A3030 3A30302E 303031" },

  {   L_, 2037,   6,   2,   21,   56,  53,  887,     1,
                                               "05 7FFFFFFF FF"              },
  {   L_, 2037,   6,   2,   21,   56,  53,  887,     0,
                    "17 32303337 2d30362d 30325432 313A3536 3A35332E 383837" },

  {   L_, 2037,   6,   2,   21,   56,  53,  888,     1,
                                               "06 00800000 0000"            },
  {   L_, 2037,   6,   2,   21,   56,  53,  888,     0,
                    "17 32303337 2d30362d 30325432 313A3536 3A35332E 383838" },

  {   L_, 2037,   6,   2,   21,   56,  53,  889,     1,
                                               "06 00800000 0001"            },
  {   L_, 2037,   6,   2,   21,   56,  53,  889,     0,
                    "17 32303337 2d30362d 30325432 313A3536 3A35332E 383839" },

  {   L_, 2099,  12,  31,   23,   59,  59,  999,     1,
                                               "06 024BCE5C EFFF"            },
  {   L_, 2099,  12,  31,   23,   59,  59,  999,     0,
                    "17 32303939 2d31322d 33315432 333A3539 3A35392E 393939" },

  {   L_, 2100,   1,   1,    0,    0,   0,    0,     1,
                                               "06 024BCE5C F000"            },
  {   L_, 2100,   1,   1,    0,    0,   0,    0,     0,
                    "17 32313030 2d30312d 30315430 303A3030 3A30302E 303030" },

  {   L_, 6479,  10,  17,    2,   45,  55,  327,     1,
                                               "06 7FFFFFFF FFFF"            },
  {   L_, 6479,  10,  17,    2,   45,  55,  327,     0,
                    "17 36343739 2d31302d 31375430 323A3435 3A35352E 333237" },

  {   L_, 6479,  10,  17,    2,   45,  55,  328,     1,
                                               "09 00000080 00000000 00"     },
  {   L_, 6479,  10,  17,    2,   45,  55,  328,     0,
                    "17 36343739 2d31302d 31375430 323A3435 3A35352E 333238" },

  {   L_, 6479,  10,  17,    2,   45,  55,  329,     1,
                                               "09 00000080 00000000 01"     },
  {   L_, 6479,  10,  17,    2,   45,  55,  329,     0,
                    "17 36343739 2d31302d 31375430 323A3435 3A35352E 333239" },

  {   L_, 2019,  12,  31,   23,   59,  59,  999,     1,
                                               "01 FF"                       },
  {   L_, 2019,  12,  31,   23,   59,  59,  999,     0,
                    "17 32303139 2d31322d 33315432 333A3539 3A35392E 393939" },

  {   L_, 2019,  12,  31,   23,   59,  59,  872,     1,
                                               "01 80"                       },
  {   L_, 2019,  12,  31,   23,   59,  59,  872,     0,
                    "17 32303139 2d31322d 33315432 333A3539 3A35392E 383732" },

  {   L_, 2019,  12,  31,   23,   59,  59,  871,     1,
                                               "02 FF7F"                     },
  {   L_, 2019,  12,  31,   23,   59,  59,  871,     0,
                    "17 32303139 2d31322d 33315432 333A3539 3A35392E 383731" },

  {   L_, 2019,  12,  31,   23,   59,  59,    1,     1,
                                               "02 FC19"                     },
  {   L_, 2019,  12,  31,   23,   59,  59,    1,     0,
                    "17 32303139 2d31322d 33315432 333A3539 3A35392E 303031" },

  {   L_, 2019,  12,  31,   23,   59,  59,    0,     1,
                                               "02 FC18"                     },
  {   L_, 2019,  12,  31,   23,   59,  59,    0,     0,
                    "17 32303139 2d31322d 33315432 333A3539 3A35392E 303030" },

  {   L_, 2019,  12,  31,   23,   59,  58,  999,     1,
                                               "02 FC17"                     },
  {   L_, 2019,  12,  31,   23,   59,  58,  999,     0,
                    "17 32303139 2d31322d 33315432 333A3539 3A35382E 393939" },

  {   L_, 2019,  12,  31,   23,   59,  27,  232,     1,
                                               "02 8000"                     },
  {   L_, 2019,  12,  31,   23,   59,  27,  232,     0,
                    "17 32303139 2d31322d 33315432 333A3539 3A32372E 323332" },

  {   L_, 2019,  12,  31,   23,   59,  27,  231,     1,
                                               "03 FF7FFF"                   },
  {   L_, 2019,  12,  31,   23,   59,  27,  231,     0,
                    "17 32303139 2d31322d 33315432 333A3539 3A32372E 323331" },

  {   L_, 2019,  12,  31,   23,   59,   0,    1,     1,
                                               "03 FF15A1"                   },
  {   L_, 2019,  12,  31,   23,   59,   0,    1,     0,
                    "17 32303139 2d31322d 33315432 333A3539 3A30302E 303031" },

  {   L_, 2019,  12,  31,   23,   59,   0,    0,     1,
                                               "03 FF15A0"                   },
  {   L_, 2019,  12,  31,   23,   59,   0,    0,     0,
                    "17 32303139 2d31322d 33315432 333A3539 3A30302E 303030" },

  {   L_, 2019,  12,  31,   23,   58,  59,  999,     1,
                                               "03 FF159F"                   },
  {   L_, 2019,  12,  31,   23,   58,  59,  999,     0,
                    "17 32303139 2d31322d 33315432 333A3538 3A35392E 393939" },

  {   L_, 2019,  12,  31,   23,    0,   0,    1,     1,
                                               "03 C91181"                   },
  {   L_, 2019,  12,  31,   23,    0,   0,    1,     0,
                    "17 32303139 2d31322d 33315432 333A3030 3A30302E 303031" },

  {   L_, 2019,  12,  31,   23,    0,   0,    0,     1,
                                               "03 C91180"                   },
  {   L_, 2019,  12,  31,   23,    0,   0,    0,     0,
                    "17 32303139 2d31322d 33315432 333A3030 3A30302E 303030" },

  {   L_, 2019,  12,  31,   22,   59,  59,  999,     1,
                                               "03 C9117F"                   },
  {   L_, 2019,  12,  31,   22,   59,  59,  999,     0,
                    "17 32303139 2d31322d 33315432 323A3539 3A35392E 393939" },

  {   L_, 2019,  12,  31,   21,   40,  11,  392,     1,
                                               "03 800000"                   },
  {   L_, 2019,  12,  31,   21,   40,  11,  392,     0,
                    "17 32303139 2d31322d 33315432 313A3430 3A31312E 333932" },

  {   L_, 2019,  12,  31,   21,   40,  11,  391,     1,
                                               "04 FF7FFFFF"                 },
  {   L_, 2019,  12,  31,   21,   40,  11,  391,     0,
                    "17 32303139 2d31322d 33315432 313A3430 3A31312E 333931" },

  {   L_, 2019,  12,  07,    3,   28,  36,  352,     1,
                                               "04 80000000"                 },
  {   L_, 2019,  12,  07,    3,   28,  36,  352,     0,
                    "17 32303139 2d31322d 30375430 333A3238 3A33362E 333532" },

  {   L_, 2019,  12,  07,    3,   28,  36,  351,     1,
                                               "05 FF7FFFFF FF"              },
  {   L_, 2019,  12,  07,    3,   28,  36,  351,     0,
                    "17 32303139 2d31322d 30375430 333A3238 3A33362E 333531" },

  {   L_, 2002,   7,  31,    2,    3,   6,  112,     1,
                                               "05 80000000 00"              },
  {   L_, 2002,   7,  31,    2,    3,   6,  112,     0,
                    "17 32303032 2d30372d 33315430 323A3033 3A30362E 313132" },

  {   L_, 2002,   7,  31,    2,    3,   6,  111,     1,
                                               "06 FF7FFFFF FFFF"            },
  {   L_, 2002,   7,  31,    2,    3,   6,  111,     0,
                    "17 32303032 2d30372d 33315430 323A3033 3A30362E 313131" },

  {   L_,    1,   1,   1,    0,    0,   0,    0,     1,
                                               "06 C60D8F6C 4000"            },
  {   L_,    1,   1,   1,    0,    0,   0,    0,     0,
                    "17 30303031 2d30312d 30315430 303A3030 3A30302E 303030" },

  {   L_, 9999,  12,  31,   23,   59,  59,  999,     1,
                                               "09 000000E5 0873B8F3 FF"     },
  {   L_, 9999,  12,  31,   23,   59,  59,  999,     0,
                    "17 39393939 2d31322d 33315432 333A3539 3A35392E 393939" },
  //------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE  = DATA[i].d_lineNum;
                const int   YEAR  = DATA[i].d_year;
                const int   MONTH = DATA[i].d_month;
                const int   DAY   = DATA[i].d_day;
                const int   HOUR  = DATA[i].d_hour;
                const int   MIN   = DATA[i].d_min;
                const int   SECS  = DATA[i].d_sec;
                const int   MSEC  = DATA[i].d_milliSec;
                const bool  BIN   = DATA[i].d_useBinary;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                LOOP_ASSERT(LINE, bdlt::Date::isValidYearMonthDay(YEAR,
                                                                  MONTH,
                                                                  DAY));

                if (veryVerbose) { P_(YEAR) P_(MONTH) P_(DAY)
                                   P_(HOUR) P_(MIN) P_(SECS) P(MSEC) P(EXP) }

                balber::BerEncoderOptions options;
                options.setEncodeDateAndTimeTypesAsBinary(BIN);

                const bdlt::Datetime VALUE(YEAR, MONTH, DAY,
                                          HOUR, MIN, SECS, MSEC);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                LOOP2_ASSERT(LEN, (int)osb.length(), LEN == (int)osb.length());
                LOOP2_ASSERT(osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                bdlt::Datetime value;
                int numBytesConsumed = 0;

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS == Util::getValue(&isb,
                                                 &value,
                                                 &numBytesConsumed));
                ASSERT(0   == isb.length());
                ASSERT(LEN == numBytesConsumed);
                LOOP2_ASSERT(VALUE, value, VALUE == value);
            }
        }

        if (verbose) bsl::cout << "\nTesting DatetimeTz." << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                int         d_hour;      // hour under test
                int         d_min;       // min under test
                int         d_sec;       // sec under test
                int         d_milliSec;  // milliSec under test
                int         d_offset;    // timezone offset
                bool        d_useBinary; // whether to use binary format
                const char *d_exp;       // expected output
            } DATA[] = {
  //line  year  mon  day  hour   min  sec    ms    off    opt  exp
  //----  ----- ---  ---  ----   ---  ---    --    ---    ---  ---
  {   L_, 2020,   1,   1,    0,    0,   0,    0,     0,     1,
                                               "01 00"                       },
  {   L_, 2020,   1,   1,    0,    0,   0,    0,     0,     0,
      "1D 32303230 2d30312d 30315430 303A3030 3A30302E 3030302B 30303A30 30" },

  {   L_, 2020,   1,   1,    0,    0,   0,    0,   1439,     1,
                                               "07 059F0000 000000"          },
  {   L_, 2020,   1,   1,    0,    0,   0,    0,   1439,     0,
      "1D 32303230 2d30312d 30315430 303A3030 3A30302E 3030302B 32333A35 39" },

  {   L_, 2020,   1,   1,    0,    0,   0,    0,  -1439,     1,
                                               "07 FA610000 000000"          },
  {   L_, 2020,   1,   1,    0,    0,   0,    0,  -1439,     0,
      "1D 32303230 2d30312d 30315430 303A3030 3A30302E 3030302D 32333A35 39" },

  {   L_, 2020,   1,   1,    0,    0,   0,  127,     0,     1,
                                               "01 7F"                       },
  {   L_, 2020,   1,   1,    0,    0,   0,  127,     0,     0,
      "1D 32303230 2d30312d 30315430 303A3030 3A30302E 3132372B 30303A30 30" },

  {   L_, 2020,   1,   1,    0,    0,   0,  127,   1439,     1,
                                               "07 059F0000 00007F"          },
  {   L_, 2020,   1,   1,    0,    0,   0,  127,   1439,     0,
      "1D 32303230 2d30312d 30315430 303A3030 3A30302E 3132372B 32333A35 39" },

  {   L_, 2020,   1,   1,    0,    0,   0,  127,  -1439,     1,
                                               "07 FA610000 00007F"          },
  {   L_, 2020,   1,   1,    0,    0,   0,  127,  -1439,     0,
      "1D 32303230 2d30312d 30315430 303A3030 3A30302E 3132372D 32333A35 39" },

  {   L_, 2020,   1,   1,    0,    0,   0,  128,     0,     1,
                                               "02 0080"                     },
  {   L_, 2020,   1,   1,    0,    0,   0,  128,     0,     0,
      "1D 32303230 2d30312d 30315430 303A3030 3A30302E 3132382B 30303A30 30" },

  {   L_, 2020,   1,   1,    0,    0,   0,  128,   1439,     1,
                                               "07 059F0000 000080"          },
  {   L_, 2020,   1,   1,    0,    0,   0,  128,   1439,     0,
      "1D 32303230 2d30312d 30315430 303A3030 3A30302E 3132382B 32333A35 39" },

  {   L_, 2020,   1,   1,    0,    0,   0,  128,  -1439,     1,
                                               "07 FA610000 000080"          },
  {   L_, 2020,   1,   1,    0,    0,   0,  128,  -1439,     0,
      "1D 32303230 2d30312d 30315430 303A3030 3A30302E 3132382D 32333A35 39" },

  {   L_, 2020,   1,   1,    0,    0,  32,  767,     0,     1,
                                               "02 7FFF"                     },
  {   L_, 2020,   1,   1,    0,    0,  32,  767,     0,     0,
      "1D 32303230 2d30312d 30315430 303A3030 3A33322E 3736372B 30303A30 30" },

  {   L_, 2020,   1,   1,    0,    0,  32,  767,  1439,     1,
                                               "07 059F0000 007FFF"          },
  {   L_, 2020,   1,   1,    0,    0,  32,  767,  1439,     0,
      "1D 32303230 2d30312d 30315430 303A3030 3A33322E 3736372B 32333A35 39" },

  {   L_, 2020,   1,   1,    0,    0,  32,  767, -1439,     1,
                                               "07 FA610000 007FFF"          },
  {   L_, 2020,   1,   1,    0,    0,  32,  767, -1439,     0,
      "1D 32303230 2d30312d 30315430 303A3030 3A33322E 3736372D 32333A35 39" },

  {   L_, 2020,   1,   1,    0,    0,  32,  768,     0,     1,
                                               "03 008000"                   },
  {   L_, 2020,   1,   1,    0,    0,  32,  768,     0,     0,
      "1D 32303230 2d30312d 30315430 303A3030 3A33322E 3736382B 30303A30 30" },

  {   L_, 2020,   1,   1,    0,    0,  32,  768,  1439,     1,
                                               "07 059F0000 008000"          },
  {   L_, 2020,   1,   1,    0,    0,  32,  768,  1439,     0,
      "1D 32303230 2d30312d 30315430 303A3030 3A33322E 3736382B 32333A35 39" },

  {   L_, 2020,   1,   1,    0,    0,  32,  768, -1439,     1,
                                               "07 FA610000 008000"          },
  {   L_, 2020,   1,   1,    0,    0,  32,  768, -1439,     0,
      "1D 32303230 2d30312d 30315430 303A3030 3A33322E 3736382D 32333A35 39" },

  {   L_, 2020,   1,   1,    2,   19,  48,  607,     0,     1,
                                               "03 7FFFFF"                   },
  {   L_, 2020,   1,   1,    2,   19,  48,  607,     0,     0,
      "1D 32303230 2d30312d 30315430 323A3139 3A34382E 3630372B 30303A30 30" },

  {   L_, 2020,   1,   1,    2,   19,  48,  607,  1439,     1,
                                               "07 059F0000 7FFFFF"          },
  {   L_, 2020,   1,   1,    2,   19,  48,  607,  1439,     0,
      "1D 32303230 2d30312d 30315430 323A3139 3A34382E 3630372B 32333A35 39" },

  {   L_, 2020,   1,   1,    2,   19,  48,  607, -1439,     1,
                                               "07 FA610000 7FFFFF"          },
  {   L_, 2020,   1,   1,    2,   19,  48,  607, -1439,     0,
      "1D 32303230 2d30312d 30315430 323A3139 3A34382E 3630372D 32333A35 39" },

  {   L_, 2020,   1,   1,    2,   19,  48,  608,     0,     1,
                                               "04 00800000"                 },
  {   L_, 2020,   1,   1,    2,   19,  48,  608,     0,     0,
      "1D 32303230 2d30312d 30315430 323A3139 3A34382E 3630382B 30303A30 30" },

  {   L_, 2020,   1,   1,    2,   19,  48,  608,  1439,     1,
                                               "07 059F0000 800000"          },
  {   L_, 2020,   1,   1,    2,   19,  48,  608,  1439,     0,
      "1D 32303230 2d30312d 30315430 323A3139 3A34382E 3630382B 32333A35 39" },

  {   L_, 2020,   1,   1,    2,   19,  48,  608, -1439,     1,
                                               "07 FA610000 800000"          },
  {   L_, 2020,   1,   1,    2,   19,  48,  608, -1439,     0,
      "1D 32303230 2d30312d 30315430 323A3139 3A34382E 3630382D 32333A35 39" },

  {   L_, 2020,   1,  25,   20,   31,  23,  647,     0,     1,
                                               "04 7FFFFFFF"                 },
  {   L_, 2020,   1,  25,   20,   31,  23,  647,     0,     0,
      "1D 32303230 2d30312d 32355432 303A3331 3A32332E 3634372B 30303A30 30" },

  {   L_, 2020,   1,  25,   20,   31,  23,  647,  1439,     1,
                                               "07 059F007F FFFFFF"          },
  {   L_, 2020,   1,  25,   20,   31,  23,  647,  1439,     0,
      "1D 32303230 2d30312d 32355432 303A3331 3A32332E 3634372B 32333A35 39" },

  {   L_, 2020,   1,  25,   20,   31,  23,  647, -1439,     1,
                                               "07 FA61007F FFFFFF"          },
  {   L_, 2020,   1,  25,   20,   31,  23,  647, -1439,     0,
      "1D 32303230 2d30312d 32355432 303A3331 3A32332E 3634372D 32333A35 39" },

  {   L_, 2020,   1,  25,   20,   31,  23,  648,     0,     1,
                                               "05 00800000 00"              },
  {   L_, 2020,   1,  25,   20,   31,  23,  648,     0,     0,
      "1D 32303230 2d30312d 32355432 303A3331 3A32332E 3634382B 30303A30 30" },

  {   L_, 2020,   1,  25,   20,   31,  23,  648,  1439,     1,
                                               "07 059F0080 000000"          },
  {   L_, 2020,   1,  25,   20,   31,  23,  648,  1439,     0,
      "1D 32303230 2d30312d 32355432 303A3331 3A32332E 3634382B 32333A35 39" },

  {   L_, 2020,   1,  25,   20,   31,  23,  648, -1439,     1,
                                               "07 FA610080 000000"          },
  {   L_, 2020,   1,  25,   20,   31,  23,  648, -1439,     0,
      "1D 32303230 2d30312d 32355432 303A3331 3A32332E 3634382D 32333A35 39" },

  {   L_, 2037,   6,   2,   21,   56,  53,  887,     0,     1,
                                               "05 7FFFFFFF FF"              },
  {   L_, 2037,   6,   2,   21,   56,  53,  887,     0,     0,
      "1D 32303337 2d30362d 30325432 313A3536 3A35332E 3838372B 30303A30 30" },

  {   L_, 2037,   6,   2,   21,   56,  53,  887,  1439,     1,
                                               "07 059F7FFF FFFFFF"          },
  {   L_, 2037,   6,   2,   21,   56,  53,  887,  1439,     0,
      "1D 32303337 2d30362d 30325432 313A3536 3A35332E 3838372B 32333A35 39" },

  {   L_, 2037,   6,   2,   21,   56,  53,  887, -1439,     1,
                                               "07 FA617FFF FFFFFF"          },
  {   L_, 2037,   6,   2,   21,   56,  53,  887, -1439,     0,
      "1D 32303337 2d30362d 30325432 313A3536 3A35332E 3838372D 32333A35 39" },

  {   L_, 2037,   6,   2,   21,   56,  53,  888,     0,     1,
                                               "06 00800000 0000"            },
  {   L_, 2037,   6,   2,   21,   56,  53,  888,     0,     0,
      "1D 32303337 2d30362d 30325432 313A3536 3A35332E 3838382B 30303A30 30" },

  {   L_, 2037,   6,   2,   21,   56,  53,  888,  1439,     1,
                                               "08 059F0080 00000000"        },
  {   L_, 2037,   6,   2,   21,   56,  53,  888,  1439,     0,
      "1D 32303337 2d30362d 30325432 313A3536 3A35332E 3838382B 32333A35 39" },

  {   L_, 2037,   6,   2,   21,   56,  53,  888, -1439,     1,
                                               "08 FA610080 00000000"        },
  {   L_, 2037,   6,   2,   21,   56,  53,  888, -1439,     0,
      "1D 32303337 2d30362d 30325432 313A3536 3A35332E 3838382D 32333A35 39" },

  {   L_, 6479,  10,  17,    2,   45,  55,  327,     0,     1,
                                               "06 7FFFFFFF FFFF"            },
  {   L_, 6479,  10,  17,    2,   45,  55,  327,     0,     0,
      "1D 36343739 2d31302d 31375430 323A3435 3A35352E 3332372B 30303A30 30" },

  {   L_, 6479,  10,  17,    2,   45,  55,  327,  1439,     1,
                                               "08 059F7FFF FFFFFFFF"        },
  {   L_, 6479,  10,  17,    2,   45,  55,  327,  1439,     0,
      "1D 36343739 2d31302d 31375430 323A3435 3A35352E 3332372B 32333A35 39" },

  {   L_, 6479,  10,  17,    2,   45,  55,  327, -1439,     1,
                                               "08 FA617FFF FFFFFFFF"        },
  {   L_, 6479,  10,  17,    2,   45,  55,  327, -1439,     0,
      "1D 36343739 2d31302d 31375430 323A3435 3A35352E 3332372D 32333A35 39" },

  {   L_, 6479,  10,  17,    2,   45,  55,  328,     0,     1,
                                               "09 00000080 00000000 00 "    },
  {   L_, 6479,  10,  17,    2,   45,  55,  328,     0,     0,
      "1D 36343739 2d31302d 31375430 323A3435 3A35352E 3332382B 30303A30 30" },

  {   L_, 6479,  10,  17,    2,   45,  55,  328,  1439,     1,
                                               "09 059F0080 00000000 00"     },
  {   L_, 6479,  10,  17,    2,   45,  55,  328,  1439,     0,
      "1D 36343739 2d31302d 31375430 323A3435 3A35352E 3332382B 32333A35 39" },

  {   L_, 6479,  10,  17,    2,   45,  55,  328, -1439,     1,
                                               "09 FA610080 00000000 00"     },
  {   L_, 6479,  10,  17,    2,   45,  55,  328, -1439,     0,
      "1D 36343739 2d31302d 31375430 323A3435 3A35352E 3332382D 32333A35 39" },

  {   L_, 2019,  12,  31,   23,   59,  59,  999,     0,     1,
                                               "01 FF"                       },
  {   L_, 2019,  12,  31,   23,   59,  59,  999,     0,     0,
      "1D 32303139 2d31322d 33315432 333A3539 3A35392E 3939392B 30303A30 30" },

  {   L_, 2019,  12,  31,   23,   59,  59,  999,  1439,     1,
                                               "07 059FFFFF FFFFFF"          },
  {   L_, 2019,  12,  31,   23,   59,  59,  999,  1439,     0,
      "1D 32303139 2d31322d 33315432 333A3539 3A35392E 3939392B 32333A35 39" },

  {   L_, 2019,  12,  31,   23,   59,  59,  999, -1439,     1,
                                               "07 FA61FFFF FFFFFF"          },
  {   L_, 2019,  12,  31,   23,   59,  59,  999, -1439,     0,
      "1D 32303139 2d31322d 33315432 333A3539 3A35392E 3939392D 32333A35 39" },

  {   L_, 2019,  12,  31,   23,   59,  59,  872,     0,     1,
                                               "01 80"                       },
  {   L_, 2019,  12,  31,   23,   59,  59,  872,     0,     0,
      "1D 32303139 2d31322d 33315432 333A3539 3A35392E 3837322B 30303A30 30" },

  {   L_, 2019,  12,  31,   23,   59,  59,  872,  1439,     1,
                                               "07 059FFFFF FFFF80"          },
  {   L_, 2019,  12,  31,   23,   59,  59,  872,  1439,     0,
      "1D 32303139 2d31322d 33315432 333A3539 3A35392E 3837322B 32333A35 39" },

  {   L_, 2019,  12,  31,   23,   59,  59,  872, -1439,     1,
                                               "07 FA61FFFF FFFF80"          },
  {   L_, 2019,  12,  31,   23,   59,  59,  872, -1439,     0,
      "1D 32303139 2d31322d 33315432 333A3539 3A35392E 3837322D 32333A35 39" },

  {   L_, 2019,  12,  31,   23,   59,  59,  871,     0,     1,
                                               "02 FF7F"                     },
  {   L_, 2019,  12,  31,   23,   59,  59,  871,     0,     0,
      "1D 32303139 2d31322d 33315432 333A3539 3A35392E 3837312B 30303A30 30" },

  {   L_, 2019,  12,  31,   23,   59,  59,  871,  1439,     1,
                                               "07 059FFFFF FFFF7F"          },
  {   L_, 2019,  12,  31,   23,   59,  59,  871,  1439,     0,
      "1D 32303139 2d31322d 33315432 333A3539 3A35392E 3837312B 32333A35 39" },

  {   L_, 2019,  12,  31,   23,   59,  59,  871, -1439,     1,
                                               "07 FA61FFFF FFFF7F"          },
  {   L_, 2019,  12,  31,   23,   59,  59,  871, -1439,     0,
      "1D 32303139 2d31322d 33315432 333A3539 3A35392E 3837312D 32333A35 39" },

  {   L_, 2019,  12,  31,   23,   59,  27,  232,     0,     1,
                                               "02 8000"                     },
  {   L_, 2019,  12,  31,   23,   59,  27,  232,     0,     0,
      "1D 32303139 2d31322d 33315432 333A3539 3A32372E 3233322B 30303A30 30" },

  {   L_, 2019,  12,  31,   23,   59,  27,  232,  1439,     1,
                                               "07 059FFFFF FF8000"          },
  {   L_, 2019,  12,  31,   23,   59,  27,  232,  1439,     0,
      "1D 32303139 2d31322d 33315432 333A3539 3A32372E 3233322B 32333A35 39" },

  {   L_, 2019,  12,  31,   23,   59,  27,  232, -1439,     1,
                                               "07 FA61FFFF FF8000"          },
  {   L_, 2019,  12,  31,   23,   59,  27,  232, -1439,     0,
      "1D 32303139 2d31322d 33315432 333A3539 3A32372E 3233322D 32333A35 39" },

  {   L_, 2019,  12,  31,   23,   59,  27,  231,     0,     1,
                                               "03 FF7FFF"                   },
  {   L_, 2019,  12,  31,   23,   59,  27,  231,     0,     0,
      "1D 32303139 2d31322d 33315432 333A3539 3A32372E 3233312B 30303A30 30" },

  {   L_, 2019,  12,  31,   23,   59,  27,  231,  1439,     1,
                                               "07 059FFFFF FF7FFF"          },
  {   L_, 2019,  12,  31,   23,   59,  27,  231,  1439,     0,
      "1D 32303139 2d31322d 33315432 333A3539 3A32372E 3233312B 32333A35 39" },

  {   L_, 2019,  12,  31,   23,   59,  27,  231, -1439,     1,
                                               "07 FA61FFFF FF7FFF"          },
  {   L_, 2019,  12,  31,   23,   59,  27,  231, -1439,     0,
      "1D 32303139 2d31322d 33315432 333A3539 3A32372E 3233312D 32333A35 39" },

  {   L_, 2019,  12,  31,   21,   40,  11,  392,     0,     1,
                                               "03 800000"                   },
  {   L_, 2019,  12,  31,   21,   40,  11,  392,     0,     0,
      "1D 32303139 2d31322d 33315432 313A3430 3A31312E 3339322B 30303A30 30" },

  {   L_, 2019,  12,  31,   21,   40,  11,  392,  1439,     1,
                                               "07 059FFFFF 800000"          },
  {   L_, 2019,  12,  31,   21,   40,  11,  392,  1439,     0,
      "1D 32303139 2d31322d 33315432 313A3430 3A31312E 3339322B 32333A35 39" },

  {   L_, 2019,  12,  31,   21,   40,  11,  392, -1439,     1,
                                               "07 FA61FFFF 800000"          },
  {   L_, 2019,  12,  31,   21,   40,  11,  392, -1439,     0,
      "1D 32303139 2d31322d 33315432 313A3430 3A31312E 3339322D 32333A35 39" },

  {   L_, 2019,  12,  31,   21,   40,  11,  391,     0,     1,
                                               "04 FF7FFFFF"                 },
  {   L_, 2019,  12,  31,   21,   40,  11,  391,     0,     0,
      "1D 32303139 2d31322d 33315432 313A3430 3A31312E 3339312B 30303A30 30" },

  {   L_, 2019,  12,  31,   21,   40,  11,  391,  1439,     1,
                                               "07 059FFFFF 7FFFFF"          },
  {   L_, 2019,  12,  31,   21,   40,  11,  391,  1439,     0,
      "1D 32303139 2d31322d 33315432 313A3430 3A31312E 3339312B 32333A35 39" },

  {   L_, 2019,  12,  31,   21,   40,  11,  391, -1439,     1,
                                               "07 FA61FFFF 7FFFFF"          },
  {   L_, 2019,  12,  31,   21,   40,  11,  391, -1439,     0,
      "1D 32303139 2d31322d 33315432 313A3430 3A31312E 3339312D 32333A35 39" },

  {   L_, 2019,  12,  07,    3,   28,  36,  352,     0,     1,
                                               "04 80000000"                 },
  {   L_, 2019,  12,  07,    3,   28,  36,  352,     0,     0,
      "1D 32303139 2d31322d 30375430 333A3238 3A33362E 3335322B 30303A30 30" },

  {   L_, 2019,  12,  07,    3,   28,  36,  352,  1439,     1,
                                               "07 059FFF80 000000"          },
  {   L_, 2019,  12,  07,    3,   28,  36,  352,  1439,     0,
      "1D 32303139 2d31322d 30375430 333A3238 3A33362E 3335322B 32333A35 39" },

  {   L_, 2019,  12,  07,    3,   28,  36,  352, -1439,     1,
                                               "07 FA61FF80 000000"          },
  {   L_, 2019,  12,  07,    3,   28,  36,  352, -1439,     0,
      "1D 32303139 2d31322d 30375430 333A3238 3A33362E 3335322D 32333A35 39" },

  {   L_, 2019,  12,  07,    3,   28,  36,  351,     0,     1,
                                               "05 FF7FFFFF FF"              },
  {   L_, 2019,  12,  07,    3,   28,  36,  351,     0,     0,
      "1D 32303139 2d31322d 30375430 333A3238 3A33362E 3335312B 30303A30 30" },

  {   L_, 2019,  12,  07,    3,   28,  36,  351,  1439,     1,
                                               "07 059FFF7F FFFFFF"          },
  {   L_, 2019,  12,  07,    3,   28,  36,  351,  1439,     0,
      "1D 32303139 2d31322d 30375430 333A3238 3A33362E 3335312B 32333A35 39" },

  {   L_, 2019,  12,  07,    3,   28,  36,  351, -1439,     1,
                                               "07 FA61FF7F FFFFFF"          },
  {   L_, 2019,  12,  07,    3,   28,  36,  351, -1439,     0,
      "1D 32303139 2d31322d 30375430 333A3238 3A33362E 3335312D 32333A35 39" },

  {   L_, 2002,   7,  31,    2,    3,   6,  112,     0,     1,
                                               "05 80000000 00"              },
  {   L_, 2002,   7,  31,    2,    3,   6,  112,     0,     0,
      "1D 32303032 2d30372d 33315430 323A3033 3A30362E 3131322B 30303A30 30" },

  {   L_, 2002,   7,  31,    2,    3,   6,  112,  1439,     1,
                                               "07 059F8000 000000"          },
  {   L_, 2002,   7,  31,    2,    3,   6,  112,  1439,     0,
      "1D 32303032 2d30372d 33315430 323A3033 3A30362E 3131322B 32333A35 39" },

  {   L_, 2002,   7,  31,    2,    3,   6,  112, -1439,     1,
                                               "07 FA618000 000000"          },
  {   L_, 2002,   7,  31,    2,    3,   6,  112, -1439,     0,
      "1D 32303032 2d30372d 33315430 323A3033 3A30362E 3131322D 32333A35 39" },

  {   L_, 2002,   7,  31,    2,    3,   6,  111,     0,     1,
                                               "06 FF7FFFFF FFFF"            },
  {   L_, 2002,   7,  31,    2,    3,   6,  111,     0,     0,
      "1D 32303032 2d30372d 33315430 323A3033 3A30362E 3131312B 30303A30 30" },

  {   L_, 2002,   7,  31,    2,    3,   6,  111,  1439,     1,
                                               "08 059FFF7F FFFFFFFF"        },
  {   L_, 2002,   7,  31,    2,    3,   6,  111,  1439,     0,
      "1D 32303032 2d30372d 33315430 323A3033 3A30362E 3131312B 32333A35 39" },

  {   L_, 2002,   7,  31,    2,    3,   6,  111, -1439,     1,
                                               "08 FA61FF7F FFFFFFFF"        },
  {   L_, 2002,   7,  31,    2,    3,   6,  111, -1439,     0,
      "1D 32303032 2d30372d 33315430 323A3033 3A30362E 3131312D 32333A35 39" },

  {   L_,    1,   1,   1,    0,    0,   0,    0,     0,     1,
                                               "06 C60D8F6C 4000"            },
  {   L_,    1,   1,   1,    0,    0,   0,    0,     0,     0,
      "1D 30303031 2d30312d 30315430 303A3030 3A30302E 3030302B 30303A30 30" },

  {   L_,    1,   1,   1,    0,    0,   0,    0,  1439,     1,
                                               "08 059FC60D 8F6C4000"        },
  {   L_,    1,   1,   1,    0,    0,   0,    0,  1439,     0,
      "1D 30303031 2d30312d 30315430 303A3030 3A30302E 3030302B 32333A35 39" },

  {   L_,    1,   1,   1,    0,    0,   0,    0, -1439,     1,
                                               "08 FA61C60D 8F6C4000"        },
  {   L_,    1,   1,   1,    0,    0,   0,    0, -1439,     0,
      "1D 30303031 2d30312d 30315430 303A3030 3A30302E 3030302D 32333A35 39" },

  {   L_, 9999,  12,  31,   23,   59,  59,  999,     0,     1,
                                               "09 000000E5 0873B8F3 FF"     },
  {   L_, 9999,  12,  31,   23,   59,  59,  999,     0,     0,
      "1D 39393939 2d31322d 33315432 333A3539 3A35392E 3939392B 30303A30 30" },

  {   L_, 9999,  12,  31,   23,   59,  59,  999,  1439,     1,
                                               "09 059F00E5 0873B8F3 FF"     },
  {   L_, 9999,  12,  31,   23,   59,  59,  999,  1439,     0,
      "1D 39393939 2d31322d 33315432 333A3539 3A35392E 3939392B 32333A35 39" },

  {   L_, 9999,  12,  31,   23,   59,  59,  999, -1439,     1,
                                               "09 FA6100E5 0873B8F3 FF"     },
  {   L_, 9999,  12,  31,   23,   59,  59,  999, -1439,     0,
      "1D 39393939 2d31322d 33315432 333A3539 3A35392E 3939392D 32333A35 39" },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE  = DATA[i].d_lineNum;
                const int   YEAR  = DATA[i].d_year;
                const int   MONTH = DATA[i].d_month;
                const int   DAY   = DATA[i].d_day;
                const int   HOUR  = DATA[i].d_hour;
                const int   MIN   = DATA[i].d_min;
                const int   SECS  = DATA[i].d_sec;
                const int   MSEC  = DATA[i].d_milliSec;
                const int   OFF   = DATA[i].d_offset;
                const bool  BIN   = DATA[i].d_useBinary;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                LOOP_ASSERT(LINE, bdlt::Date::isValidYearMonthDay(YEAR,
                                                                  MONTH,
                                                                  DAY));

                if (veryVerbose) { P_(YEAR) P_(MONTH) P_(DAY) P_(OFF)
                                   P_(HOUR) P_(MIN) P_(SECS) P(MSEC) P(EXP) }

                balber::BerEncoderOptions options;
                options.setEncodeDateAndTimeTypesAsBinary(BIN);

                const bdlt::DatetimeTz VALUE(bdlt::Datetime(YEAR, MONTH, DAY,
                                                          HOUR, MIN, SECS,
                                                          MSEC),
                                            OFF);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                LOOP2_ASSERT(LEN, (int)osb.length(), LEN == (int)osb.length());
                LOOP2_ASSERT(osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                bdlt::DatetimeTz value;
                int numBytesConsumed = 0;

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS == Util::getValue(&isb,
                                                 &value,
                                                 &numBytesConsumed));
                ASSERT(0   == isb.length());
                ASSERT(LEN == numBytesConsumed);
                LOOP2_ASSERT(VALUE, value, VALUE == value);
            }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING BRUTE FORCE 'putValue'/'getValue' FOR DATE/TIME
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\n"
                           "TESTING 'putValue', 'getValue' FOR DATE/TIME" "\n"
                           "============================================" "\n";

        balber::BerEncoderOptions options;
        options.setEncodeDateAndTimeTypesAsBinary(true);
        const balber::BerEncoderOptions DEFOPTS;

        if (verbose) bsl::cout << "\nTesting Date Brute force." << bsl::endl;
        {
            const int YEARS[] = { 1, 4, 96, 100, 400, 500, 800, 1000, 1600,
                                  1700, 1751, 1752, 1753, 1930, 2000, 2010,
                                  2012, 2019, 2020, 2021, 6478, 6479, 6480,
                                  9998, 9999 };
            const int NUM_YEARS = sizeof YEARS / sizeof *YEARS;

            const int MONTHS[] = { 1, 2, 5, 8, 9, 12 };
            const int NUM_MONTHS = sizeof MONTHS / sizeof *MONTHS;

            const int DAYS[] = { 1, 2, 5, 10, 15, 20, 28, 29, 30, 31 };
            const int NUM_DAYS = sizeof DAYS / sizeof *DAYS;

            for (int i = 0; i < NUM_YEARS; ++i) {
            for (int j = 0; j < NUM_MONTHS; ++j) {
            for (int k = 0; k < NUM_DAYS; ++k) {

                const int YEAR  = YEARS[i];
                const int MONTH = MONTHS[j];
                const int DAY   = DAYS[k];

                if (bdlt::Date::isValidYearMonthDay(YEAR, MONTH, DAY)
                 && bdlt::SerialDateImpUtil::isValidYearMonthDay(YEAR,
                                                                 MONTH,
                                                                 DAY)) {

                    if (veryVerbose) { P_(YEAR) P_(MONTH) P(DAY) }

                    const bdlt::Date VALUE(YEAR, MONTH, DAY);
                    bdlt::Date value;

                    const int OFF1 = 0, OFF2 =-1439, OFF3 =1439;
                    const bdlt::DateTz VALUE1(bdlt::Date(YEAR, MONTH, DAY),
                                             OFF1);
                    const bdlt::DateTz VALUE2(bdlt::Date(YEAR, MONTH, DAY),
                                             OFF2);
                    const bdlt::DateTz VALUE3(bdlt::Date(YEAR, MONTH, DAY),
                                             OFF3);
                    bdlt::DateTz value1, value2, value3;

                    {
                        bdlsb::MemOutStreamBuf osb;
                        ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                        const int LENGTH = osb.length();

                        if (veryVerbose) {
                            cout << "Output Buffer:";
                            printBuffer(osb.data(), osb.length());
                        }
                        int numBytesConsumed = 0;

                        bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                      osb.length());
                        ASSERT(SUCCESS == Util::getValue(&isb,
                                                         &value,
                                                         &numBytesConsumed));
                        ASSERT(0      == isb.length());
                        ASSERT(LENGTH == numBytesConsumed);
                        LOOP2_ASSERT(VALUE, value, VALUE == value);
                    }

                    {
                        bdlsb::MemOutStreamBuf osb;
                        ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                        const int LENGTH = osb.length();

                        if (veryVerbose) {
                            cout << "Output Buffer:";
                            printBuffer(osb.data(), osb.length());
                        }
                        int numBytesConsumed = 0;

                        bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                      osb.length());
                        ASSERT(SUCCESS == Util::getValue(&isb,
                                                         &value,
                                                         &numBytesConsumed));
                        ASSERT(0      == isb.length());
                        ASSERT(LENGTH == numBytesConsumed);
                        LOOP2_ASSERT(VALUE, value, VALUE == value);
                    }

                    {
                        bdlsb::MemOutStreamBuf osb1, osb2, osb3;

                        ASSERT(0 == Util::putValue(&osb1, VALUE1, &options));
                        const int LENGTH1 = osb1.length();

                        ASSERT(0 == Util::putValue(&osb2, VALUE2, &options));
                        const int LENGTH2 = osb2.length();

                        ASSERT(0 == Util::putValue(&osb3, VALUE3, &options));
                        const int LENGTH3 = osb3.length();

                        if (veryVerbose) {
                            cout << "Output Buffer:";
                            printBuffer(osb1.data(), osb1.length());
                            printBuffer(osb2.data(), osb2.length());
                            printBuffer(osb3.data(), osb3.length());
                        }
                        int nbc1 = 0, nbc2 = 0, nbc3 = 0;

                        bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                       osb1.length());
                        bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                       osb2.length());
                        bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                       osb3.length());

                        ASSERT(SUCCESS == Util::getValue(&isb1,
                                                         &value1,
                                                         &nbc1));
                        ASSERT(SUCCESS == Util::getValue(&isb2,
                                                         &value2,
                                                         &nbc2));
                        ASSERT(SUCCESS == Util::getValue(&isb3,
                                                         &value3,
                                                         &nbc3));

                        ASSERT(0       == isb1.length());
                        ASSERT(0       == isb2.length());
                        ASSERT(0       == isb3.length());

                        ASSERT(LENGTH1 == nbc1);
                        ASSERT(LENGTH2 == nbc2);
                        ASSERT(LENGTH3 == nbc3);

                        LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                        LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                        LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                    }

                    {
                        bdlsb::MemOutStreamBuf osb1, osb2, osb3;

                        ASSERT(0 == Util::putValue(&osb1, VALUE1, &DEFOPTS));
                        const int LENGTH1 = osb1.length();

                        ASSERT(0 == Util::putValue(&osb2, VALUE2, &DEFOPTS));
                        const int LENGTH2 = osb2.length();

                        ASSERT(0 == Util::putValue(&osb3, VALUE3, &DEFOPTS));
                        const int LENGTH3 = osb3.length();

                        if (veryVerbose) {
                            cout << "Output Buffer:";
                            printBuffer(osb1.data(), osb1.length());
                            printBuffer(osb2.data(), osb2.length());
                            printBuffer(osb3.data(), osb3.length());
                        }
                        int nbc1 = 0, nbc2 = 0, nbc3 = 0;

                        bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                       osb1.length());
                        bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                       osb2.length());
                        bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                       osb3.length());

                        ASSERT(SUCCESS == Util::getValue(&isb1,
                                                         &value1,
                                                         &nbc1));
                        ASSERT(SUCCESS == Util::getValue(&isb2,
                                                         &value2,
                                                         &nbc2));
                        ASSERT(SUCCESS == Util::getValue(&isb3,
                                                         &value3,
                                                         &nbc3));

                        ASSERT(0       == isb1.length());
                        ASSERT(0       == isb2.length());
                        ASSERT(0       == isb3.length());

                        ASSERT(LENGTH1 == nbc1);
                        ASSERT(LENGTH2 == nbc2);
                        ASSERT(LENGTH3 == nbc3);

                        LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                        LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                        LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                    }
                }
            }
            }
            }
        }

        if (verbose) bsl::cout << "\nTesting Time Brute force." << bsl::endl;
        {
            for (int hour = 0; hour <= 23; ++hour) {
                for (int min = 0; min < 60; ++min) {
                    for (int sec = 0; sec < 60; ++sec) {

                        if (veryVerbose) { P_(hour) P_(min) P(sec) }

                        const int MS1 = 0, MS2 = 500, MS3 = 999;
                        const int OFF1 = 0, OFF2 =-1439, OFF3 =1439;
                        const bdlt::TimeTz VALUE1(bdlt::Time(hour,
                                                           min,
                                                           sec,
                                                           MS1),
                                                 OFF1);
                        const bdlt::TimeTz VALUE2(bdlt::Time(hour,
                                                           min,
                                                           sec,
                                                           MS2),
                                                 OFF2);
                        const bdlt::TimeTz VALUE3(bdlt::Time(hour,
                                                           min,
                                                           sec,
                                                           MS3),
                                                 OFF3);
                        bdlt::TimeTz value1, value2, value3;

                        {
                            const int MS = 0;
                            const bdlt::Time VALUE(hour, min, sec, MS);
                            bdlt::Time value;

                            bdlsb::MemOutStreamBuf osb;
                            ASSERT(0 == Util::putValue(&osb,
                                                       VALUE,
                                                       &options));
                            const int LENGTH = osb.length();

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb.data(), osb.length());
                            }
                            int numBytesConsumed = 0;

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                          osb.length());
                            ASSERT(SUCCESS == Util::getValue(
                                                           &isb,
                                                           &value,
                                                           &numBytesConsumed));
                            ASSERT(0      == isb.length());
                            ASSERT(LENGTH == numBytesConsumed);
                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                        }

                        {
                            const int MS = 999;
                            const bdlt::Time VALUE(hour, min, sec, MS);
                            bdlt::Time value;

                            bdlsb::MemOutStreamBuf osb;
                            ASSERT(0 == Util::putValue(&osb,
                                                       VALUE,
                                                       &options));
                            const int LENGTH = osb.length();

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb.data(), osb.length());
                            }
                            int numBytesConsumed = 0;

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                          osb.length());
                            ASSERT(SUCCESS == Util::getValue(
                                                           &isb,
                                                           &value,
                                                           &numBytesConsumed));
                            ASSERT(0      == isb.length());
                            ASSERT(LENGTH == numBytesConsumed);
                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;

                            ASSERT(0 == Util::putValue(&osb1,
                                                       VALUE1,
                                                       &options));
                            const int LENGTH1 = osb1.length();

                            ASSERT(0 == Util::putValue(&osb2,
                                                       VALUE2,
                                                       &options));
                            const int LENGTH2 = osb2.length();

                            ASSERT(0 == Util::putValue(&osb3,
                                                       VALUE3,
                                                       &options));
                            const int LENGTH3 = osb3.length();

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }
                            int nbc1 = 0, nbc2 = 0, nbc3 = 0;

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                           osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                           osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                           osb3.length());

                            ASSERT(SUCCESS == Util::getValue(&isb1,
                                                             &value1,
                                                             &nbc1));
                            ASSERT(SUCCESS == Util::getValue(&isb2,
                                                             &value2,
                                                             &nbc2));
                            ASSERT(SUCCESS == Util::getValue(&isb3,
                                                             &value3,
                                                             &nbc3));

                            ASSERT(0       == isb1.length());
                            ASSERT(0       == isb2.length());
                            ASSERT(0       == isb3.length());

                            ASSERT(LENGTH1 == nbc1);
                            ASSERT(LENGTH2 == nbc2);
                            ASSERT(LENGTH3 == nbc3);

                            LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                            LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                            LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                        }

                        {
                            const int MS = 0;
                            const bdlt::Time VALUE(hour, min, sec, MS);
                            bdlt::Time value;

                            bdlsb::MemOutStreamBuf osb;
                            ASSERT(0 == Util::putValue(&osb,
                                                       VALUE,
                                                       &DEFOPTS));
                            const int LENGTH = osb.length();

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb.data(), osb.length());
                            }
                            int numBytesConsumed = 0;

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                          osb.length());
                            ASSERT(SUCCESS == Util::getValue(
                                                           &isb,
                                                           &value,
                                                           &numBytesConsumed));
                            ASSERT(0      == isb.length());
                            ASSERT(LENGTH == numBytesConsumed);
                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                        }

                        {
                            const int MS = 999;
                            const bdlt::Time VALUE(hour, min, sec, MS);
                            bdlt::Time value;

                            bdlsb::MemOutStreamBuf osb;
                            ASSERT(0 == Util::putValue(&osb,
                                                       VALUE,
                                                       &DEFOPTS));
                            const int LENGTH = osb.length();

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb.data(), osb.length());
                            }
                            int numBytesConsumed = 0;

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                          osb.length());
                            ASSERT(SUCCESS == Util::getValue(
                                                           &isb,
                                                           &value,
                                                           &numBytesConsumed));
                            ASSERT(0      == isb.length());
                            ASSERT(LENGTH == numBytesConsumed);
                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;

                            ASSERT(0 == Util::putValue(&osb1,
                                                       VALUE1,
                                                       &DEFOPTS));
                            const int LENGTH1 = osb1.length();

                            ASSERT(0 == Util::putValue(&osb2,
                                                       VALUE2,
                                                       &DEFOPTS));
                            const int LENGTH2 = osb2.length();

                            ASSERT(0 == Util::putValue(&osb3,
                                                       VALUE3,
                                                       &DEFOPTS));
                            const int LENGTH3 = osb3.length();

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }
                            int nbc1 = 0, nbc2 = 0, nbc3 = 0;

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                           osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                           osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                           osb3.length());

                            ASSERT(SUCCESS == Util::getValue(&isb1,
                                                             &value1,
                                                             &nbc1));
                            ASSERT(SUCCESS == Util::getValue(&isb2,
                                                             &value2,
                                                             &nbc2));
                            ASSERT(SUCCESS == Util::getValue(&isb3,
                                                             &value3,
                                                             &nbc3));

                            ASSERT(0       == isb1.length());
                            ASSERT(0       == isb2.length());
                            ASSERT(0       == isb3.length());

                            ASSERT(LENGTH1 == nbc1);
                            ASSERT(LENGTH2 == nbc2);
                            ASSERT(LENGTH3 == nbc3);

                            LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                            LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                            LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                        }
                    }
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting Datetime Brute force."
                               << bsl::endl;
        {
            const int YEARS[] = { 1, 4, 96, 100, 400, 500, 800, 1000, 1600,
                                  1700, 1751, 1752, 1753, 1930, 2000, 2010,
                                  2012, 2019, 2020, 2021, 6478, 6479, 6480,
                                  9998, 9999 };
            const int NUM_YEARS = sizeof YEARS / sizeof *YEARS;

            const int MONTHS[] = { 1, 2, 5, 8, 9, 12 };
            const int NUM_MONTHS = sizeof MONTHS / sizeof *MONTHS;

            const int DAYS[] = { 1, 2, 5, 10, 15, 20, 28, 29, 30, 31 };
            const int NUM_DAYS = sizeof DAYS / sizeof *DAYS;

            for (int di = 0; di < NUM_YEARS; ++di) {
            for (int dj = 0; dj < NUM_MONTHS; ++dj) {
            for (int dk = 0; dk < NUM_DAYS; ++dk) {

                const int YEAR  = YEARS[di];
                const int MONTH = MONTHS[dj];
                const int DAY   = DAYS[dk];

                if (bdlt::Date::isValidYearMonthDay(YEAR, MONTH, DAY)
                 && bdlt::SerialDateImpUtil::isValidYearMonthDay(YEAR,
                                                                 MONTH,
                                                                 DAY)) {

                    const int HOURS[] = { 0, 12, 23 };
                    const int NUM_HOURS = sizeof HOURS / sizeof *HOURS;

                    const int MINS[] = { 0, 30, 59 };
                    const int NUM_MINS = sizeof MINS / sizeof *MINS;

                    const int SECONDS[] = { 0, 30, 59 };
                    const int NUM_SECS = sizeof SECONDS / sizeof *SECONDS;

                    for (int ti = 0; ti < NUM_HOURS; ++ti) {
                    for (int tj = 0; tj < NUM_MINS; ++tj) {
                    for (int tk = 0; tk < NUM_SECS; ++tk) {

                        const int HOUR = HOURS[ti];
                        const int MIN  = MINS[tj];
                        const int SECS = SECONDS[tk];

                        if (veryVerbose) { P_(YEAR) P_(MONTH) P(DAY) }
                        if (veryVerbose) { P_(HOUR) P_(MIN) P(SECS) }

                        const int MS = 0;
                        const bdlt::Date DATE(YEAR, MONTH, DAY);
                        const bdlt::Time TIME(HOUR, MIN, SECS, MS);
                        const bdlt::Datetime VALUE(DATE, TIME);
                        bdlt::Datetime value;

                        const int MS1 = 0, MS2 = 500, MS3 = 999;
                        const int OFF1 = 0, OFF2 =-1439, OFF3 =1439;
                        const bdlt::Date DATE1(YEAR, MONTH, DAY);
                        const bdlt::Time TIME1(HOUR, MIN, SECS, MS1);

                        const bdlt::Date DATE2(YEAR, MONTH, DAY);
                        const bdlt::Time TIME2(HOUR, MIN, SECS, MS2);

                        const bdlt::Date DATE3(YEAR, MONTH, DAY);
                        const bdlt::Time TIME3(HOUR, MIN, SECS, MS3);

                        const bdlt::Datetime DT1(DATE1, TIME1);
                        const bdlt::Datetime DT2(DATE2, TIME2);
                        const bdlt::Datetime DT3(DATE3, TIME3);

                        const bdlt::DatetimeTz VALUE1(DT1, OFF1);
                        const bdlt::DatetimeTz VALUE2(DT2, OFF2);
                        const bdlt::DatetimeTz VALUE3(DT3, OFF3);

                        bdlt::DatetimeTz value1, value2, value3;

                        {
                            bdlsb::MemOutStreamBuf osb;
                            ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                            const int LENGTH = osb.length();

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb.data(), osb.length());
                            }
                            int numBytesConsumed = 0;

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                          osb.length());
                            ASSERT(SUCCESS == Util::getValue(
                                                           &isb,
                                                           &value,
                                                           &numBytesConsumed));
                            ASSERT(0      == isb.length());
                            ASSERT(LENGTH == numBytesConsumed);
                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;

                            ASSERT(0 == Util::putValue(&osb1,
                                                       VALUE1,
                                                       &options));
                            const int LENGTH1 = osb1.length();

                            ASSERT(0 == Util::putValue(&osb2,
                                                       VALUE2,
                                                       &options));
                            const int LENGTH2 = osb2.length();

                            ASSERT(0 == Util::putValue(&osb3,
                                                       VALUE3,
                                                       &options));
                            const int LENGTH3 = osb3.length();

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }
                            int nbc1 = 0, nbc2 = 0, nbc3 = 0;

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                           osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                           osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                           osb3.length());

                            ASSERT(SUCCESS == Util::getValue(&isb1,
                                                             &value1,
                                                             &nbc1));
                            ASSERT(SUCCESS == Util::getValue(&isb2,
                                                             &value2,
                                                             &nbc2));
                            ASSERT(SUCCESS == Util::getValue(&isb3,
                                                             &value3,
                                                             &nbc3));

                            ASSERT(0       == isb1.length());
                            ASSERT(0       == isb2.length());
                            ASSERT(0       == isb3.length());

                            ASSERT(LENGTH1 == nbc1);
                            ASSERT(LENGTH2 == nbc2);
                            ASSERT(LENGTH3 == nbc3);

                            LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                            LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                            LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                        }

                        {
                            bdlsb::MemOutStreamBuf osb;
                            ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                            const int LENGTH = osb.length();

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb.data(), osb.length());
                            }
                            int numBytesConsumed = 0;

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                          osb.length());
                            ASSERT(SUCCESS == Util::getValue(
                                                           &isb,
                                                           &value,
                                                           &numBytesConsumed));
                            ASSERT(0      == isb.length());
                            ASSERT(LENGTH == numBytesConsumed);
                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;

                            ASSERT(0 == Util::putValue(&osb1,
                                                       VALUE1,
                                                       &DEFOPTS));
                            const int LENGTH1 = osb1.length();

                            ASSERT(0 == Util::putValue(&osb2,
                                                       VALUE2,
                                                       &DEFOPTS));
                            const int LENGTH2 = osb2.length();

                            ASSERT(0 == Util::putValue(&osb3,
                                                       VALUE3,
                                                       &DEFOPTS));
                            const int LENGTH3 = osb3.length();

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }
                            int nbc1 = 0, nbc2 = 0, nbc3 = 0;

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                           osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                           osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                           osb3.length());

                            ASSERT(SUCCESS == Util::getValue(&isb1,
                                                             &value1,
                                                             &nbc1));
                            ASSERT(SUCCESS == Util::getValue(&isb2,
                                                             &value2,
                                                             &nbc2));
                            ASSERT(SUCCESS == Util::getValue(&isb3,
                                                             &value3,
                                                             &nbc3));

                            ASSERT(0       == isb1.length());
                            ASSERT(0       == isb2.length());
                            ASSERT(0       == isb3.length());

                            ASSERT(LENGTH1 == nbc1);
                            ASSERT(LENGTH2 == nbc2);
                            ASSERT(LENGTH3 == nbc3);

                            LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                            LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                            LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                        }
                    }
                    }
                    }
                }
            }
            }
            }
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'putValue', 'getValue' FOR DATE/TIME
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\n"
                           "TESTING 'putValue', 'getValue' FOR DATE/TIME" "\n"
                           "============================================" "\n";

        balber::BerEncoderOptions options;
        options.setEncodeDateAndTimeTypesAsBinary(true);
        const balber::BerEncoderOptions DEFOPTS;

        if (verbose) bsl::cout << "\nDefine data" << bsl::endl;

        static const struct {
            int d_lineNum;   // source line number
            int d_year;      // year under test
            int d_month;     // month under test
            int d_day;       // day under test
            int d_hour;      // hour under test
            int d_minutes;   // minutes under test
            int d_seconds;   // seconds under test
            int d_milliSecs; // milli seconds under test
            int d_tzoffset;  // time zone offset
        } DATA[] = {
   //line no.  year   month   day   hour    min   sec    ms  offset
   //-------   -----  -----   ---   ----    ---   ---    --  ------
    {      L_,      1,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,     45     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    1,     1,     1,    1,    1,      0     },
    {      L_,      1,     1,    1,     1,     1,    1,    1,    500     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    1,     1,    23,   59,   59,      0     },
    {      L_,      1,     1,    1,     1,    23,   59,   59,   1439     },
    {      L_,      1,     1,    1,     1,    23,   59,   59,  -1439     },

    {      L_,      1,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,    2,     0,     0,    0,    0,   1439     },
    {      L_,      1,     1,    2,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    2,     1,     1,    1,    1,      0     },
    {      L_,      1,     1,    2,     1,     1,    1,    1,    500     },

    {      L_,      1,     1,    2,     1,    23,   59,   59,      0     },
    {      L_,      1,     1,    2,     1,    23,   59,   59,    500     },
    {      L_,      1,     1,    2,     1,    23,   59,   59,   -500     },

    {      L_,      1,     1,   10,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,   10,     1,     1,    1,    1,     99     },

    {      L_,      1,     1,   30,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,   31,     0,     0,    0,    0,   1439     },
    {      L_,      1,     1,   31,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     2,    1,     0,     0,    0,    0,      0     },
    {      L_,      1,     2,    1,    23,    59,   59,    0,   1439     },

    {      L_,      1,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,      1,    12,   31,    23,    59,   59,    0,   1439     },

    {      L_,      2,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      2,     1,    1,    23,    59,   59,    0,   1439     },

    {      L_,      4,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      4,     1,    1,    23,    59,   59,    0,   1439     },

    {      L_,      4,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,      4,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,      4,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,      4,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,      4,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,      4,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,      4,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,      4,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,      4,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,      8,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,      8,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,      8,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,      8,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,      8,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,      8,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,    100,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    100,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,    100,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,    100,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    100,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,    100,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    400,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,    400,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,    400,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,    400,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    400,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,    400,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,    500,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    500,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,    500,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    500,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,    800,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    800,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,    800,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,    800,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,    800,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    800,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   1000,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   1000,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,   1000,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   1000,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   2000,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,   2000,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   2000,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   2016,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2017,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2018,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2019,    12,   31,     0,     0,    0,    0,      0     },

    {      L_,   2020,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,   2020,     1,    1,     0,     0,    0,    0,   1439     },
    {      L_,   2020,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,   2020,     1,    1,    23,    59,   59,  999,      0     },
    {      L_,   2020,     1,    1,    23,    59,   59,  999,   1439     },
    {      L_,   2020,     1,    1,    23,    59,   59,  999,  -1439     },

    {      L_,   2020,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,   2020,     1,    2,     0,     0,    0,    0,   1439     },
    {      L_,   2020,     1,    2,     0,     0,    0,    0,  -1439     },

    {      L_,   2020,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   2020,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,   2020,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,   2020,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,   2020,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   2020,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,   2021,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,   2022,     1,    2,     0,     0,    0,    0,      0     },

    {      L_,   9999,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   9999,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,   9999,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,   9999,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   9999,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,   9999,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,   9999,    12,   30,     0,     0,    0,    0,      0     },
    {      L_,   9999,    12,   30,    23,    59,   59,    0,   1439     },

    {      L_,   9999,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   9999,    12,   31,    23,    59,   59,    0,   1439     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) bsl::cout << "\nTesting 'bdlt::Date'." << bsl::endl;
        {
            typedef bdlt::Date Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;

                const Type VALUE(Y, M, D); Type value;

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value,
                                                     &numBytesConsumed));
                    ASSERT(0      == isb.length());
                    ASSERT(LENGTH == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value,
                                                     &numBytesConsumed));
                    ASSERT(0      == isb.length());
                    ASSERT(LENGTH == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }
            }

            // Invalid value returns an error
            {
                bsls::AssertFailureHandlerGuard guard(&ignoreAssertHandler);
                const Type VALUE1(0, 1, 1);
                const Type VALUE2(1, 0, 1);
                const Type VALUE3(1, 1, 0);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 != Util::putValue(&osb, VALUE1));
                ASSERT(0 != Util::putValue(&osb, VALUE1, &options));

                // The date value is converted to a valid date in this case
                // and the error remains undetected.  This is fine since the
                // user is already in undefined behavior and our error is at
                // best a convenience and not contractually required.

//                 ASSERT(0 != Util::putValue(&osb, VALUE2));
//                 ASSERT(0 != Util::putValue(&osb, VALUE2, &options));

                ASSERT(0 != Util::putValue(&osb, VALUE3));
                ASSERT(0 != Util::putValue(&osb, VALUE3, &options));
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::DateTz'." << bsl::endl;
        {
            typedef bdlt::DateTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdlt::Date(Y, M, D), OFF); Type value;

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value,
                                                     &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value,
                                                     &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }
            }

            // Invalid value returns an error
            {
                bsls::AssertFailureHandlerGuard guard(&ignoreAssertHandler);
                const Type VALUE1(bdlt::Date(0, 1, 1), 0);
                const Type VALUE2(bdlt::Date(0, 1, 1), 2000);
                const Type VALUE3(bdlt::Date(1, 0, 1), 0);
                const Type VALUE4(bdlt::Date(1, 0, 1), 2000);
                const Type VALUE5(bdlt::Date(1, 1, 0), 0);
                const Type VALUE6(bdlt::Date(1, 1, 0), 2000);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 != Util::putValue(&osb, VALUE1));
                ASSERT(0 != Util::putValue(&osb, VALUE1, &options));
                ASSERT(0 != Util::putValue(&osb, VALUE2));
                ASSERT(0 != Util::putValue(&osb, VALUE2, &options));

                // The date value is converted to a valid date in this case
                // and the error remains undetected.  This is fine since the
                // user is already in undefined behavior and our error is at
                // best a convenience and not contractually required.

//                 ASSERT(0 != Util::putValue(&osb, VALUE3));
//                 ASSERT(0 != Util::putValue(&osb, VALUE3, &options));

                ASSERT(0 != Util::putValue(&osb, VALUE4));
                ASSERT(0 != Util::putValue(&osb, VALUE4, &options));
                ASSERT(0 != Util::putValue(&osb, VALUE5));
                ASSERT(0 != Util::putValue(&osb, VALUE5, &options));
                ASSERT(0 != Util::putValue(&osb, VALUE6));
                ASSERT(0 != Util::putValue(&osb, VALUE6, &options));
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::Time'." << bsl::endl;
        {
            typedef bdlt::Time Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;

                const Type VALUE(H, MM, S, MS); Type value;

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value,
                                                     &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value,
                                                     &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::TimeTz'." << bsl::endl;
        {
            typedef bdlt::TimeTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdlt::Time(H, MM, S, MS), OFF); Type value;

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value,
                                                     &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value,
                                                     &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::Datetime'." << bsl::endl;
        {
            typedef bdlt::Datetime Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;

                const Type VALUE(Y, M, D, H, MM, S, MS); Type value;

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value,
                                                     &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value,
                                                     &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }
            }

            // Invalid value returns an error
            {
                bsls::AssertFailureHandlerGuard guard(&ignoreAssertHandler);
                const Type VALUE1(bdlt::Datetime(bdlt::Date(0, 1, 1),
                                                bdlt::Time(0, 0, 0, 0)));
                const Type VALUE2(bdlt::Datetime(bdlt::Date(1, 0, 1),
                                                bdlt::Time(0, 0, 0, 0)));
                const Type VALUE3(bdlt::Datetime(bdlt::Date(1, 1, 0),
                                                bdlt::Time(0, 0, 0, 0)));

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 != Util::putValue(&osb, VALUE1));
                ASSERT(0 != Util::putValue(&osb, VALUE1, &options));

                // The date value is converted to a valid date in this case
                // and the error remains undetected.  This is fine since the
                // user is already in undefined behavior and our error is at
                // best a convenience and not contractually required.

//                 ASSERT(0 != Util::putValue(&osb, VALUE2));
//                 ASSERT(0 != Util::putValue(&osb, VALUE2, &options));

                ASSERT(0 != Util::putValue(&osb, VALUE3));
                ASSERT(0 != Util::putValue(&osb, VALUE3, &options));
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::DatetimeTz'." << bsl::endl;
        {
            typedef bdlt::DatetimeTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdlt::Datetime(Y, M, D, H, MM, S, MS), OFF);
                Type value;

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value,
                                                     &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    ASSERT(0 == Util::putValue(&osb, VALUE, &DEFOPTS));
                    const int LENGTH = osb.length();

                    if (veryVerbose) {
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }
                    int numBytesConsumed = 0;

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(SUCCESS == Util::getValue(&isb,
                                                     &value,
                                                     &numBytesConsumed));
                    ASSERT(0       == isb.length());
                    ASSERT(LENGTH  == numBytesConsumed);
                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                }
            }

            // Invalid value returns an error
            {
                bsls::AssertFailureHandlerGuard guard(&ignoreAssertHandler);
                const Type VALUE1(bdlt::Datetime(bdlt::Date(0, 1, 1),
                                                bdlt::Time(0, 0, 0, 0)),
                                                0);
                const Type VALUE2(bdlt::Datetime(bdlt::Date(0, 1, 1),
                                                bdlt::Time(0, 0, 0, 0)),
                                                2000);
                const Type VALUE3(bdlt::Datetime(bdlt::Date(1, 0, 1),
                                                bdlt::Time(0, 0, 0, 0)),
                                                0);
                const Type VALUE4(bdlt::Datetime(bdlt::Date(1, 0, 1),
                                                bdlt::Time(0, 0, 0, 0)),
                                                2000);
                const Type VALUE5(bdlt::Datetime(bdlt::Date(1, 1, 0),
                                                bdlt::Time(0, 0, 0, 0)),
                                                0);
                const Type VALUE6(bdlt::Datetime(bdlt::Date(1, 1, 0),
                                                bdlt::Time(0, 0, 0, 0)),
                                                2000);

                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 != Util::putValue(&osb, VALUE1));
                ASSERT(0 != Util::putValue(&osb, VALUE1, &options));
                ASSERT(0 != Util::putValue(&osb, VALUE2));
                ASSERT(0 != Util::putValue(&osb, VALUE2, &options));

                // The date value is converted to a valid date in this case
                // and the error remains undetected.  This is fine since the
                // user is already in undefined behavior and our error is at
                // best a convenience and not contractually required.

//                 ASSERT(0 != Util::putValue(&osb, VALUE3));
//                 ASSERT(0 != Util::putValue(&osb, VALUE3, &options));
//                 ASSERT(0 != Util::putValue(&osb, VALUE4));
//                 ASSERT(0 != Util::putValue(&osb, VALUE4, &options));

                ASSERT(0 != Util::putValue(&osb, VALUE5));
                ASSERT(0 != Util::putValue(&osb, VALUE5, &options));
                ASSERT(0 != Util::putValue(&osb, VALUE6));
                ASSERT(0 != Util::putValue(&osb, VALUE6, &options));
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'put/getIdentifierOctets'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\n"
                                      "TESTING 'put/getIdentifierOctets'" "\n"
                                      "=================================" "\n";

        {
            enum { SUCCESS = 0, FAILURE = -1 };

            enum {
                UNIVERSAL        = balber::BerConstants::e_UNIVERSAL,
                APPLICATION      = balber::BerConstants::e_APPLICATION,
                CONTEXT_SPECIFIC = balber::BerConstants::e_CONTEXT_SPECIFIC,
                PRIVATE          = balber::BerConstants::e_PRIVATE,
                PRIMITIVE        = balber::BerConstants::e_PRIMITIVE,
                CONSTRUCTED      = balber::BerConstants::e_CONSTRUCTED
            };

            static const struct {
                int         d_line;    // line number
                int         d_class;   // tag class
                int         d_type;    // tag type
                int         d_number;  // tag number
                const char *d_exp;     // expected output onto the stream
            } DATA[] = {
                //line  class        type         number   exp
                //----  -----        ----         ------   ---
                { L_,   UNIVERSAL,   PRIMITIVE,   0,       "00"              },
                { L_,   UNIVERSAL,   PRIMITIVE,   1,       "01"              },
                { L_,   UNIVERSAL,   PRIMITIVE,   29,      "1D"              },
                { L_,   UNIVERSAL,   PRIMITIVE,   30,      "1E"              },
                { L_,   UNIVERSAL,   PRIMITIVE,   31,      "1F 1F"           },
                { L_,   UNIVERSAL,   PRIMITIVE,   127,     "1F 7F"           },
                { L_,   UNIVERSAL,   PRIMITIVE,   128,     "1F 81 00"        },
                { L_,   UNIVERSAL,   PRIMITIVE,   129,     "1F 81 01"        },
                { L_,   UNIVERSAL,   PRIMITIVE,   INT_MAX, "1F 87 FF FF FF 7F"
                                                                             },

                { L_,   UNIVERSAL,   CONSTRUCTED, 0,       "20"              },
                { L_,   UNIVERSAL,   CONSTRUCTED, 1,       "21"              },
                { L_,   UNIVERSAL,   CONSTRUCTED, 29,      "3D"              },
                { L_,   UNIVERSAL,   CONSTRUCTED, 30,      "3E"              },
                { L_,   UNIVERSAL,   CONSTRUCTED, 31,      "3F 1F"           },
                { L_,   UNIVERSAL,   CONSTRUCTED, 127,     "3F 7F"           },
                { L_,   UNIVERSAL,   CONSTRUCTED, 128,     "3F 81 00"        },
                { L_,   UNIVERSAL,   CONSTRUCTED, 129,     "3F 81 01"        },
                { L_,   UNIVERSAL,   CONSTRUCTED, INT_MAX, "3F 87 FF FF FF 7F"
                                                                             },

                { L_,   APPLICATION, PRIMITIVE,   0,       "40"              },
                { L_,   APPLICATION, PRIMITIVE,   1,       "41"              },
                { L_,   APPLICATION, PRIMITIVE,   29,      "5D"              },
                { L_,   APPLICATION, PRIMITIVE,   30,      "5E"              },
                { L_,   APPLICATION, PRIMITIVE,   31,      "5F 1F"           },
                { L_,   APPLICATION, PRIMITIVE,   127,     "5F 7F"           },
                { L_,   APPLICATION, PRIMITIVE,   128,     "5F 81 00"        },
                { L_,   APPLICATION, PRIMITIVE,   129,     "5F 81 01"        },
                { L_,   APPLICATION, PRIMITIVE,   INT_MAX, "5F 87 FF FF FF 7F"
                                                                             },

                { L_,   APPLICATION, CONSTRUCTED, 0,       "60"              },
                { L_,   APPLICATION, CONSTRUCTED, 1,       "61"              },
                { L_,   APPLICATION, CONSTRUCTED, 29,      "7D"              },
                { L_,   APPLICATION, CONSTRUCTED, 30,      "7E"              },
                { L_,   APPLICATION, CONSTRUCTED, 31,      "7F 1F"           },
                { L_,   APPLICATION, CONSTRUCTED, 127,     "7F 7F"           },
                { L_,   APPLICATION, CONSTRUCTED, 128,     "7F 81 00"        },
                { L_,   APPLICATION, CONSTRUCTED, 129,     "7F 81 01"        },
                { L_,   APPLICATION, CONSTRUCTED, INT_MAX, "7F 87 FF FF FF 7F"
                                                                             },

                { L_,   CONTEXT_SPECIFIC,
                                     PRIMITIVE,   0,       "80"              },
                { L_,   CONTEXT_SPECIFIC,
                                     PRIMITIVE,   1,       "81"              },
                { L_,   CONTEXT_SPECIFIC,
                                     PRIMITIVE,   29,      "9D"              },
                { L_,   CONTEXT_SPECIFIC,
                                     PRIMITIVE,   30,      "9E"              },
                { L_,   CONTEXT_SPECIFIC,
                                     PRIMITIVE,   31,      "9F 1F"           },
                { L_,   CONTEXT_SPECIFIC,
                                     PRIMITIVE,   INT_MAX, "9F 87 FF FF FF 7F"
                                                                             },

                { L_,   CONTEXT_SPECIFIC,
                                     CONSTRUCTED, 0,       "A0"              },
                { L_,   CONTEXT_SPECIFIC,
                                     CONSTRUCTED, 1,       "A1"              },
                { L_,   CONTEXT_SPECIFIC,
                                     CONSTRUCTED, 29,      "BD"              },
                { L_,   CONTEXT_SPECIFIC,
                                     CONSTRUCTED, 30,      "BE"              },
                { L_,   CONTEXT_SPECIFIC,
                                     CONSTRUCTED, 31,      "BF 1F"           },
                { L_,   CONTEXT_SPECIFIC,
                                     CONSTRUCTED, INT_MAX, "BF 87 FF FF FF 7F"
                                                                             },

                { L_,   PRIVATE,     PRIMITIVE,   0,       "C0"              },
                { L_,   PRIVATE,     PRIMITIVE,   1,       "C1"              },
                { L_,   PRIVATE,     PRIMITIVE,   29,      "DD"              },
                { L_,   PRIVATE,     PRIMITIVE,   30,      "DE"              },
                { L_,   PRIVATE,     PRIMITIVE,   31,      "DF 1F"           },
                { L_,   PRIVATE,     PRIMITIVE,   INT_MAX, "DF 87 FF FF FF 7F"
                                                                             },

                { L_,   PRIVATE,     CONSTRUCTED, 0,       "E0"              },
                { L_,   PRIVATE,     CONSTRUCTED, 1,       "E1"              },
                { L_,   PRIVATE,     CONSTRUCTED, 29,      "FD"              },
                { L_,   PRIVATE,     CONSTRUCTED, 30,      "FE"              },
                { L_,   PRIVATE,     CONSTRUCTED, 31,      "FF 1F"           },
                { L_,   PRIVATE,     CONSTRUCTED, INT_MAX, "FF 87 FF FF FF 7F"
                                                                             },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE    = DATA[i].d_line;
                const balber::BerConstants::TagClass
                            CLASS   =
                               (balber::BerConstants::TagClass)DATA[i].d_class;
                const balber::BerConstants::TagType
                            TYPE    =
                                 (balber::BerConstants::TagType)DATA[i].d_type;
                const int   NUMBER  = DATA[i].d_number;
                const char* EXP     = DATA[i].d_exp;
                const int   EXP_LEN = numOctets(EXP);

                if (veryVerbose) {
                    P_(LINE) P_(CLASS) P_(TYPE) P_(NUMBER) P(EXP)
                }

                bdlsb::MemOutStreamBuf osb;
                LOOP_ASSERT(LINE, SUCCESS  == Util::putIdentifierOctets(
                                                                      &osb,
                                                                      CLASS,
                                                                      TYPE,
                                                                      NUMBER));
                LOOP_ASSERT(LINE, EXP_LEN == (int)osb.length());
                LOOP_ASSERT(LINE, 0       == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                balber::BerConstants::TagClass theClass;
                balber::BerConstants::TagType  theType;
                int                         theNumber        = -1;
                int                         numBytesConsumed = 0;
                bdlsb::FixedMemInStreamBuf   isb(osb.data(), osb.length());

                LOOP_ASSERT(LINE, SUCCESS  == Util::getIdentifierOctets(
                                                          &isb, &theClass,
                                                          &theType, &theNumber,
                                                          &numBytesConsumed));
                LOOP_ASSERT(LINE, 0        == isb.length());
                LOOP3_ASSERT(LINE, CLASS,  theClass,  CLASS  == theClass);
                LOOP3_ASSERT(LINE, TYPE,   theType,   TYPE   == theType);
                LOOP3_ASSERT(LINE, NUMBER, theNumber, NUMBER == theNumber);
                LOOP3_ASSERT(LINE, EXP_LEN,   numBytesConsumed,
                                   EXP_LEN == numBytesConsumed);
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'putIndefiniteLengthOctet' & 'put/getEndOfContentOctets'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putIndefiniteLengthOctet', "
                                  "'put/getEndOfContentOctets'"
                               << "\n===================================="
                                  "=================="
                               << bsl::endl;

        if (verbose) bsl::cout << "\nTesting 'putIndefiniteLengthOctet'."
                               << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;
            ASSERT(SUCCESS == Util::putIndefiniteLengthOctet(&osb));
            ASSERT(1       == osb.length());
            ASSERT(balber::BerUtil_Imp::e_INDEFINITE_LENGTH_OCTET
                           == (unsigned char)osb.data()[0]);
        }

        if (verbose) bsl::cout << "\nTesting 'putEndOfContentOctets'."
                               << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;
            ASSERT(SUCCESS == Util::putEndOfContentOctets(&osb));
            ASSERT(2       == osb.length());
            ASSERT(0       == osb.data()[0]);
            ASSERT(0       == osb.data()[1]);
        }

        if (verbose) bsl::cout << "\nTesting 'getEndOfContentOctets'."
                               << bsl::endl;
        {
            char buffer[] = { 0 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 0);
            int                       numBytesConsumed = 0;

            ASSERT(FAILURE == Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 0 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 1);
            int                       numBytesConsumed = 0;

            ASSERT(FAILURE == Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 0, 0 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS == Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
            ASSERT(2       == numBytesConsumed);
        }
        {
            char buffer[] = { -1, 0 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS != Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 0, -1 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS != Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { -1, -1 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS != Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 1, 0 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS != Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 0, 1 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS != Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 1, 1 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS != Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 0, 0, 0 };
            bdlsb::FixedMemInStreamBuf isb(buffer, 3);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS == Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
            ASSERT(2       == numBytesConsumed);
        }

        if (verbose) bsl::cout << "End of test." << bsl::endl;
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for date/time components
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout
                            << "\nTESTING 'putValue', 'getValue' for date/time"
                            << "\n============================================"
                            << bsl::endl;

        balber::BerEncoderOptions options;

        if (verbose) bsl::cout << "\nTesting 'bdlt::Date'." << bsl::endl;
        {
            typedef bdlt::Date Type;

            const Type  VALUE(2005, 02, 23);
            const char *EXP    = "0A 32 30 30 35 2D 30 32 2D 32 33";
            const int   LENGTH = numOctets(EXP);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0      == Util::putValue(&osb, VALUE, &options));
            ASSERT(LENGTH == (int)osb.length());
            ASSERT(0      == compareBuffers(osb.data(), EXP));

            if (veryVerbose) {
                P(EXP)
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            Type value;
            int  numBytesConsumed = 0;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(SUCCESS == Util::getValue(&isb, &value, &numBytesConsumed));
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value);
            ASSERT(LENGTH  == numBytesConsumed);

            Type value2;
            numBytesConsumed = 0;
            isb.pubsetbuf(osb.data(), osb.length());
            int len = 0;
            ASSERT(SUCCESS == Util::getLength(&isb, &len, &numBytesConsumed));
            ASSERT(SUCCESS == Util::getValue(&isb, &value2, len));
            numBytesConsumed += len;
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value2);
            ASSERT(LENGTH  == numBytesConsumed);
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::DateTz'." << bsl::endl;
        {
            typedef bdlt::DateTz Type;

            const Type  VALUE(bdlt::Date(2005, 02, 23), 45);
            const char *EXP    = "10 32 30 30 35 2D 30 32 2D 32 33 2B 30 30 3A"
                                 " 34 35";
            const int   LENGTH = numOctets(EXP);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0      == Util::putValue(&osb, VALUE, &options));
            ASSERT(LENGTH == (int)osb.length());
            ASSERT(0      == compareBuffers(osb.data(), EXP));

            if (veryVerbose) {
                P(EXP)
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            Type value;
            int  numBytesConsumed = 0;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(SUCCESS == Util::getValue(&isb, &value, &numBytesConsumed));
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value);
            ASSERT(LENGTH  == numBytesConsumed);

            Type value2;
            numBytesConsumed = 0;
            isb.pubsetbuf(osb.data(), osb.length());
            int len = 0;
            ASSERT(SUCCESS == Util::getLength(&isb, &len, &numBytesConsumed));
            ASSERT(SUCCESS == Util::getValue(&isb, &value2, len));
            numBytesConsumed += len;
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value2);
            ASSERT(LENGTH  == numBytesConsumed);
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::Datetime'." << bsl::endl;
        {
            typedef bdlt::Datetime Type;

            const Type  VALUE(2005, 02, 23, 3, 47, 21);
            const char *EXP    = "17 32 30 30 35 2D 30 32 2D 32 33 54 30 33 3A"
                                 " 34 37 3A 32 31 2E 30 30 30";
            const int   LENGTH = numOctets(EXP);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0      == Util::putValue(&osb, VALUE, &options));
            ASSERT(LENGTH == (int)osb.length());
            ASSERT(0      == compareBuffers(osb.data(), EXP));

            if (veryVerbose) {
                P(EXP)
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            Type value;
            int  numBytesConsumed = 0;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(SUCCESS == Util::getValue(&isb, &value, &numBytesConsumed));
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value);
            ASSERT(LENGTH  == numBytesConsumed);

            Type value2;
            numBytesConsumed = 0;
            isb.pubsetbuf(osb.data(), osb.length());
            int len = 0;
            ASSERT(SUCCESS == Util::getLength(&isb, &len, &numBytesConsumed));
            ASSERT(SUCCESS == Util::getValue(&isb, &value2, len));
            numBytesConsumed += len;
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value2);
            ASSERT(LENGTH  == numBytesConsumed);
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::DatetimeTz'." << bsl::endl;
        {
            typedef bdlt::DatetimeTz Type;

            const Type  VALUE(bdlt::Datetime(2005, 02, 23, 3, 47, 21), 45);
            const char *EXP    = "1D 32 30 30 35 2D 30 32 2D 32 33 54 30 33 3A"
                                 " 34 37 3A 32 31 2E 30 30 30 2B 30 30 3A 34"
                                 " 35";
            const int   LENGTH = numOctets(EXP);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0      == Util::putValue(&osb, VALUE, &options));
            ASSERT(LENGTH == (int)osb.length());
            ASSERT(0      == compareBuffers(osb.data(), EXP));

            if (veryVerbose) {
                P(EXP)
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            Type value;
            int  numBytesConsumed = 0;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(SUCCESS == Util::getValue(&isb, &value, &numBytesConsumed));
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value);
            ASSERT(LENGTH  == numBytesConsumed);

            Type value2;
            numBytesConsumed = 0;
            isb.pubsetbuf(osb.data(), osb.length());
            int len = 0;
            ASSERT(SUCCESS == Util::getLength(&isb, &len, &numBytesConsumed));
            ASSERT(SUCCESS == Util::getValue(&isb, &value2, len));
            numBytesConsumed += len;
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value2);
            ASSERT(LENGTH  == numBytesConsumed);
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::Time'." << bsl::endl;
        {
            typedef bdlt::Time Type;

            const Type  VALUE(21, 26, 43);
            const char *EXP    = "0C 32 31 3A 32 36 3A 34 33 2E 30 30 30";
            const int   LENGTH = numOctets(EXP);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0      == Util::putValue(&osb, VALUE, &options));
            ASSERT(LENGTH == (int)osb.length());
            ASSERT(0      == compareBuffers(osb.data(), EXP));

            if (veryVerbose) {
                P(EXP)
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            Type value;
            int  numBytesConsumed = 0;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(SUCCESS == Util::getValue(&isb, &value, &numBytesConsumed));
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value);
            ASSERT(LENGTH  == numBytesConsumed);

            Type value2;
            numBytesConsumed = 0;
            isb.pubsetbuf(osb.data(), osb.length());
            int len = 0;
            ASSERT(SUCCESS == Util::getLength(&isb, &len, &numBytesConsumed));
            ASSERT(SUCCESS == Util::getValue(&isb, &value2, len));
            numBytesConsumed += len;
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value2);
            ASSERT(LENGTH  == numBytesConsumed);
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::TimeTz'." << bsl::endl;
        {
            typedef bdlt::TimeTz Type;

            const Type  VALUE(bdlt::Time(21, 26, 43), 45);
            const char *EXP    = "12 32 31 3A 32 36 3A 34 33 2E 30 30 30 2B 30"
                                 " 30 3A 34 35";
            const int   LENGTH = numOctets(EXP);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0      == Util::putValue(&osb, VALUE, &options));
            ASSERT(LENGTH == (int)osb.length());
            ASSERT(0      == compareBuffers(osb.data(), EXP));

            if (veryVerbose) {
                P(EXP)
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            Type value;
            int  numBytesConsumed = 0;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(SUCCESS == Util::getValue(&isb, &value, &numBytesConsumed));
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value);
            ASSERT(LENGTH  == numBytesConsumed);

            Type value2;
            numBytesConsumed = 0;
            isb.pubsetbuf(osb.data(), osb.length());
            int len = 0;
            ASSERT(SUCCESS == Util::getLength(&isb, &len, &numBytesConsumed));
            ASSERT(SUCCESS == Util::getValue(&isb, &value2, len));
            numBytesConsumed += len;
            ASSERT(0       == isb.length());
            ASSERT(VALUE   == value2);
            ASSERT(LENGTH  == numBytesConsumed);
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for 'bslstl::StringRef'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putValue', 'getValue' for string"
                               << "\n========================================="
                               << bsl::endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_string;   // string value
                const char *d_exp;      // expected value
            } DATA[] = {
                //line no.  string                exp
                //-------   ------                ---
                { L_,        "",                  "00" },
                { L_,        " ",                 "01 20" },
                { L_,        "-+",                "02 2D 2B" },
                { L_,        "Hello",             "05 48 65 6C 6C 6F" },
                { L_,        "12345",             "05 31 32 33 34 35" },
                { L_,        "&$#",               "03 26 24 23" },
                { L_,        "Have a nice day",   "0F 48 61 76 65 20 61 20 6E"
                                                  "69 63 65 20 64 61 79" },
                { L_,        "QWERTY",            "06 51 57 45 52 54 59" },
                { L_,        "This is an extremely long line that spans "
                             "a few lines",       "35 54 68 69 73 20 69 73 20"
                                                  "61 6E 20 65 78 74 72 65 6D"
                                                  "65 6C 79 20 6C 6F 6E 67 20"
                                                  "6C 69 6E 65 20 74 68 61 74"
                                                  "20 73 70 61 6E 73 20 61 20"
                                                "66 65 77 20 6C 69 6E 65 73" },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA; ++di) {
                const int        LINE = DATA[di].d_lineNum;
                const StringRef  VAL  = DATA[di].d_string;
                const char      *EXP  = DATA[di].d_exp;
                const int        LEN  = numOctets(EXP);

                string  val;

                bdlsb::MemOutStreamBuf osb;
                LOOP_ASSERT(LINE, 0   == Util::putValue(&osb, VAL));
                LOOP_ASSERT(LINE, LEN == (int)osb.length());
                LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    P(EXP)
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                int numBytesConsumed = 0;
                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                LOOP_ASSERT(LINE, SUCCESS  == Util::getValue(
                                                           &isb,
                                                           &val,
                                                           &numBytesConsumed));
                LOOP_ASSERT(LINE, 0 == isb.length());
                LOOP2_ASSERT(VAL, val, VAL == val);
                LOOP3_ASSERT(LINE,
                             LEN,
                             numBytesConsumed,
                             LEN == numBytesConsumed);
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for bsl::string
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putValue', 'getValue' for string"
                               << "\n========================================="
                               << bsl::endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_string;   // string value
                const char *d_exp;      // expected value
            } DATA[] = {
                //line no.  string                exp
                //-------   ------                ---
                { L_,        "",                  "00" },
                { L_,        " ",                 "01 20" },
                { L_,        "-+",                "02 2D 2B" },
                { L_,        "Hello",             "05 48 65 6C 6C 6F" },
                { L_,        "12345",             "05 31 32 33 34 35" },
                { L_,        "&$#",               "03 26 24 23" },
                { L_,        "Have a nice day",   "0F 48 61 76 65 20 61 20 6E"
                                                  "69 63 65 20 64 61 79" },
                { L_,        "QWERTY",            "06 51 57 45 52 54 59" },
                { L_,        "This is an extremely long line that spans "
                             "a few lines",       "35 54 68 69 73 20 69 73 20"
                                                  "61 6E 20 65 78 74 72 65 6D"
                                                  "65 6C 79 20 6C 6F 6E 67 20"
                                                  "6C 69 6E 65 20 74 68 61 74"
                                                  "20 73 70 61 6E 73 20 61 20"
                                                "66 65 77 20 6C 69 6E 65 73" },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA; ++di) {
                const int     LINE = DATA[di].d_lineNum;
                const string  VAL  = DATA[di].d_string;
                const char   *EXP  = DATA[di].d_exp;
                const int     LEN  = numOctets(EXP);
                      string  val;

                bdlsb::MemOutStreamBuf osb;
                LOOP_ASSERT(LINE, 0   == Util::putValue(&osb, VAL));
                LOOP_ASSERT(LINE, LEN == (int)osb.length());
                LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    P(EXP)
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                int numBytesConsumed = 0;
                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                LOOP_ASSERT(LINE, SUCCESS  == Util::getValue(
                                                           &isb, &val,
                                                           &numBytesConsumed));
                LOOP_ASSERT(LINE, 0        == isb.length());
                LOOP2_ASSERT(VAL, val, VAL == val);
                LOOP3_ASSERT(LINE, LEN, numBytesConsumed,
                             LEN == numBytesConsumed);
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'putLength' & 'getLength'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putLength', 'getLength'"
                               << "\n================================"
                               << bsl::endl;

        {
            enum { SUCCESS = 0, FAILURE = -1 };

            static const struct {
                int         d_line;   // line number
                int         d_length; // length value
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                // line        length    exp
                // ----        ------    ---
                { L_,              0,    "00"               },
                { L_,              1,    "01"               },
                { L_,             15,    "0F"               },
                { L_,             16,    "10"               },
                { L_,             32,    "20"               },
                { L_,             64,    "40"               },
                { L_,            127,    "7F"               },
                { L_,            128,    "81 80"            },
                { L_,            255,    "81 FF"            },
                { L_,            256,    "82 01 00"         },
                { L_,          32767,    "82 7F FF"         },
                { L_,          32768,    "82 80 00"         },
                { L_,          65535,    "82 FF FF"         },
                { L_,        8388608,    "83 80 00 00"      },
                { L_,       16777215,    "83 FF FF FF"      },
                { L_,     2147483646,    "84 7F FF FF FE"   },
                { L_,        INT_MAX,    "84 7F FF FF FF"   },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int            LINE = DATA[i].d_line;
                const int            LEN  = DATA[i].d_length;
                const char          *EXP  = DATA[i].d_exp;
                const int            EXP_LEN  = numOctets(EXP);
                      int            len;

                if (veryVerbose) { P_(i) P(LEN) }

                bdlsb::MemOutStreamBuf osb;
                LOOP_ASSERT(LINE, SUCCESS  == Util::putLength(&osb, LEN));
                LOOP_ASSERT(LINE, EXP_LEN  == (int)osb.length());
                LOOP_ASSERT(LINE, 0        == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    P(EXP)
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                int numBytesConsumed = 0;
                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                LOOP_ASSERT(LINE, SUCCESS  == Util::getLength(
                                                           &isb, &len,
                                                           &numBytesConsumed));
                LOOP_ASSERT(LINE, 0        == isb.length());
                LOOP3_ASSERT(LINE, LEN, len, len == LEN);
                LOOP3_ASSERT(LINE, EXP_LEN, numBytesConsumed,
                             EXP_LEN == numBytesConsumed);
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'putDoubleValue' & 'getDoubleValue' for float and double
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putDoubleValue','getDoubleValue'"
                               << "\n========================================="
                               << bsl::endl;

        {
            enum { SUCCESS = 0, FAILURE = -1 };

            typedef balber::BerUtil_Imp Imp;

            static const struct {
                int         d_line;   // line number
                double      d_value;  // unsigned char value stored in an int
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                // line        value    exp
                // ----        -----    ---
                { L_,           1.1,    "09 80 CD 08 CC CC CC CC CC CD"    },
                { L_,        99.234,    "09 80 D2 18 CE F9 DB 22 D0 E5"    },
                { L_,             0,    "00"                               },
                { L_,          1.25,    "03 80 FE 05"                      },
                { L_,           1.1,    "09 80 CD 08 CC CC CC CC CC CD"    },
                { L_,        0.0176,    "09 80 C6 12 05 BC 01 A3 6E 2F"    },
                { L_,    -7.8752345,    "09 C0 CE 1F 80 3D 79 07 52 DB"    },
                { L_,        99.234,
                                        "09 80 D2 18 CE F9 DB 22 D0 E5"    },
                { L_,      -100.987,
                                        "09 C0 D3 0C 9F 95 81 06 24 DD"    },
                { L_,    -77723.875,    "05 C0 FD 09 7C DF"                },
                { L_, 19998989.1234,    "09 80 E4 13 12 90 D1 F9 72 47"    },
                { L_,   79879879249686698E-100,
                                        "0A 81 FE B7 1F 09 39 59 03 93 B3" },
                { L_,   -9999999999999999E25,
                                        "09 C0 55 09 2E FD 1B 8D 0C F3"    },
                { L_,   1.8768E-307,    "0A 81 FB D1 10 DE 9A FC 0F BA 43" },
                { L_,   1.8768E-306,    "0A 81 FB D6 05 45 90 6E C4 EA 35" },
                { L_,   1.8768E-305,    "0A 81 FB D7 1A 5B D2 29 D8 93 09" },

                // Single precision denormalized numbers
                { L_,   1.4E-45,        "0A 81 FF 37 0F FC 34 5F A6 CA B5" },
                { L_,   -1.4E-45,       "0A C1 FF 37 0F FC 34 5F A6 CA B5" },

                // Double precision denormalized numbers

                // Commented test data that are out of range :
                // DBL_MIN  - DBL_MAX
                // This is done because we can not guarantee portability
                // across platforms.

                //{ L_,   1E-314,         "07 81 FB CE 78 A4 22 05"          },
                //{ L_,   -1E-314,        "07 C1 FB CE 78 A4 22 05"          },

                { L_,  0.1e-307,        "0A 81 FB CF 03 98 6B 3C 0C F4 69" },
                //{ L_,  1.32423e-308,    "0A 81 FB CE 09 85 B1 4E 40 CE 07" },

                //{ L_,  1.8768e-310,     "09 81 FB D0 08 A3 20 3F 84 FB" },
                { L_,  783462315e-312,  "0A 81 FB DD 11 31 53 DC 2E 24 55" },
                //{ L_,  9.2332323e-316,  "07 81 FB CF 05 91 CD 47" },

                { L_,  -0.1e-307,       "0A C1 FB CF 03 98 6B 3C 0C F4 69" },
                //{ L_, -1.32423e-308,    "0A C1 FB CE 09 85 B1 4E 40 CE 07" },

                //{ L_, -1.8768e-310,     "09 C1 FB D0 08 A3 20 3F 84 FB" },
                { L_, -783462315e-312,  "0A C1 FB DD 11 31 53 DC 2E 24 55" },
                //{ L_, -9.2332323e-316,  "07 C1 FB CF 05 91 CD 47" },

                //{ L_,  1.11254E-308,    "0A 81 FB CF 04 00 00 B9 3A AB 81" },
                //{ L_, -9.7347E-309,     "0A C1 FB CE 07 00 00 16 8C 57 15" },
                //{ L_,  5.56268E-309,    "0A 81 FB CE 03 FF FF C7 F2 67 77" },

                // FLT_MAX & FLT_MIN
                { L_,   1.175494351E-38,
                                        "0A 81 FF 4E 10 00 00 00 0A 63 9B" },
                { L_,   3.402823466E+38,
                                        "09 80 4C 0F FF FF EF F8 38 1B"    },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int     LINE = DATA[i].d_line;
                const double  VAL  = DATA[i].d_value;
                const float   F    = (float) DATA[i].d_value;
                const char   *EXP  = DATA[i].d_exp;
                const int     LEN  = numOctets(EXP);

                if (veryVerbose) { P_(i) P(VAL) }

                {
                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS  ==
                                               Imp::putDoubleValue(&osb, VAL));
                    LOOP_ASSERT(LINE, LEN      == (int)osb.length());
                    LOOP_ASSERT(LINE, 0        ==
                                              compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "Output Buffer:";
                        printBuffer(osb.data(), osb.length());
                    }

                    int    numBytesConsumed = 0;
                    int    length           = 0;
                    double val              = 0.0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE,
                                SUCCESS == Imp::getLength(&isb,
                                                          &length,
                                                          &numBytesConsumed));
                    LOOP_ASSERT(LINE,
                                SUCCESS == Imp::getDoubleValue(&isb,
                                                               &val,
                                                               length));
                    numBytesConsumed += length;
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP3_ASSERT(LINE, VAL, val, val == VAL);
                    LOOP3_ASSERT(LINE, LEN, numBytesConsumed,
                                 LEN == numBytesConsumed);
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS  == Util::putValue(&osb, F));

                    if (veryVerbose) {
                        P(VAL)
                        cout << "output buffer:";
                        printBuffer(osb.data(), osb.length());
                    }

                    int   numBytesConsumed = 0;
                    float f                = 0.0f;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS  == Util::getValue(
                                                           &isb, &f,
                                                           &numBytesConsumed));
                    LOOP3_ASSERT(LINE, F, f, F == f);
                    LOOP_ASSERT(LINE, (int)osb.length() == numBytesConsumed);
                }
            }

            if (verbose) { cout << "\nTesting for special values" << endl; }
            {
                static const struct {
                    int         d_lineNum;  // source line number
                    int         d_sign;     // sign value (0 - +ve, 1 - -ve)
                    int         d_exponent; // biased (IEEE) exponent value
                    long long   d_mantissa; // mantissa value
                    const char *d_exp;      // encoded result
                    int         d_result;   // SUCCESS (0) / FAILURE (-1)
                } REAL[] = {
                    //line  sign  exp      man          exp          res
                    //----  ----  ---      ---          ------       ---
                    // +ve & -ve infinity
                    { L_,      0, 2047,     0,         "01 40",   SUCCESS },
                    { L_,      1, 2047,     0,         "01 41",   SUCCESS },

                    // +ve & -ve NaN
                    { L_,      0, 2047,     1,         "01 42",   SUCCESS },
                    { L_,      1, 2047,     1,         "01 42",   SUCCESS },

                    // +ve & -ve 0
                    { L_,      0,    0,     0,         "00",      SUCCESS },
                    { L_,      1,    0,     0,         "00",      SUCCESS },

                    // Denormalized numbers
                    { L_,      0,    0,     1,         "04 81 FB CE 01",
                                                                  SUCCESS },
                    { L_,      1,    0,     1,         "04 C1 FB CE 01",
                                                                  SUCCESS },
                    { L_,      0,    0,  1987,         "05 81 FB CE 07 C3",
                                                                  SUCCESS },
                    { L_,      1,    0,  9832,         "05 C1 FB D1 04 CD",
                                                                  SUCCESS },
                };

                bdlsb::MemOutStreamBuf osb;

                const int NUM_DATA = sizeof REAL / sizeof *REAL;

                for (int di = 0; di < NUM_DATA; ++di) {
                    const int        LINE     = REAL[di].d_lineNum;
                    const int        SIGN     = REAL[di].d_sign;
                    const int        EXPONENT = REAL[di].d_exponent;
                    const long long  MANTISSA = REAL[di].d_mantissa;
                    const char      *EXP      = REAL[di].d_exp;
                    const int        LEN      = numOctets(EXP);
                    const int        RESULT   = REAL[di].d_result;
                          double     outVal, inVal;

                    bdlsb::MemOutStreamBuf osb;

                    assembleDouble(&outVal, SIGN, EXPONENT, MANTISSA);

                    LOOP_ASSERT(LINE, RESULT == Imp::putDoubleValue(&osb,
                                                                    outVal));
                    if (SUCCESS == RESULT) {
                        LOOP_ASSERT(LINE, LEN == (int)osb.length());
                        LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(),
                                                                EXP));
                    }

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    if (SUCCESS == RESULT) {
                        int numBytesConsumed = 0;
                        int length           = 0;

                        bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                      osb.length());
                        LOOP_ASSERT(LINE,
                                SUCCESS == Imp::getLength(&isb,
                                                          &length,
                                                          &numBytesConsumed));
                        LOOP_ASSERT(LINE,
                                    SUCCESS == Imp::getDoubleValue(&isb,
                                                                   &inVal,
                                                                   length));
                        numBytesConsumed += length;
                        LOOP_ASSERT(LINE, 0     == isb.length());

                        // to avoid NaN comparisons
                        if (inVal == inVal)
                           LOOP_ASSERT(LINE, inVal == outVal);

                        LOOP_ASSERT(LINE, LEN   == numBytesConsumed);
                    }
                }
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for unsigned integral values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putValue' and 'getValue'"
                               << "\n================================="
                               << bsl::endl;

        {
            static const struct {
                int                 d_line;   // line number
                unsigned long long  d_value;  // value
                const char         *d_exp;    // expected output
            } DATA[] = {
                // line              value      exp
                // ----              -----      ---
               { L_,                    0, "01 00"                          },
               { L_,                    1, "01 01"                          },
               { L_,                    5, "01 05"                          },
               { L_,                   65, "01 41"                          },
               { L_,                   66, "01 42"                          },
               { L_,                   67, "01 43"                          },
               { L_,                  126, "01 7E"                          },
               { L_,                  127, "01 7F"                          },
               { L_,                  128, "02 00 80"                       },
               { L_,                  129, "02 00 81"                       },
               { L_,                  254, "02 00 FE"                       },
               { L_,                  255, "02 00 FF"                       },
               { L_,                  256, "02 01 00"                       },
               { L_,                  257, "02 01 01"                       },
               { L_,                32766, "02 7F FE"                       },
               { L_,                32767, "02 7F FF"                       },
               { L_,                32768, "03 00 80 00"                    },
               { L_,                32769, "03 00 80 01"                    },
               { L_,              8388606, "03 7F FF FE"                    },
               { L_,              8388607, "03 7F FF FF"                    },
               { L_,              8388608, "04 00 80 00 00"                 },
               { L_,              8388609, "04 00 80 00 01"                 },
               { L_,           2147483645, "04 7F FF FF FD"                 },
               { L_,           2147483646, "04 7F FF FF FE"                 },
               { L_,              INT_MAX, "04 7F FF FF FF"                 },
               { L_,(unsigned)INT_MAX + 1, "05 00 80 00 00 00"              },
               { L_,         UINT_MAX - 1, "05 00 FF FF FF FE"              },
               { L_,             UINT_MAX, "05 00 FF FF FF FF"              },
               { L_,      0x7FFFFFFFFEULL, "05 7F FF FF FF FE"              },
               { L_,      0x7FFFFFFFFFULL, "05 7F FF FF FF FF"              },
               { L_,      0x8000000000ULL, "06 00 80 00 00 00 00"           },
               { L_,      0x8000000001ULL, "06 00 80 00 00 00 01"           },
               { L_,    0x7FFFFFFFFFFEULL, "06 7F FF FF FF FF FE"           },
               { L_,    0x7FFFFFFFFFFFULL, "06 7F FF FF FF FF FF"           },
               { L_,    0x800000000000ULL, "07 00 80 00 00 00 00 00"        },
               { L_,    0x800000000001ULL, "07 00 80 00 00 00 00 01"        },
               { L_,  0x7FFFFFFFFFFFFEULL, "07 7F FF FF FF FF FF FE"        },
               { L_,  0x7FFFFFFFFFFFFFULL, "07 7F FF FF FF FF FF FF"        },
               { L_,  0x80000000000000ULL, "08 00 80 00 00 00 00 00 00"     },
               { L_,  0x80000000000001ULL, "08 00 80 00 00 00 00 00 01"     },
               { L_,0x7FFFFFFFFFFFFFFEULL, "08 7F FF FF FF FF FF FF FE"     },
               { L_,0x7FFFFFFFFFFFFFFFULL, "08 7F FF FF FF FF FF FF FF"     },
               { L_,0x8000000000000000ULL, "09 00 80 00 00 00 00 00 00 00"  },
               { L_,0x8000000000000001ULL, "09 00 80 00 00 00 00 00 00 01"  },
               { L_,0xFFFFFFFFFFFFFFFEULL, "09 00 FF FF FF FF FF FF FF FE"  },
               { L_,0xFFFFFFFFFFFFFFFFULL, "09 00 FF FF FF FF FF FF FF FF"  },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            if (veryVerbose) { cout << "Testing for unsigned long long"
                                    << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const int                 LINE = DATA[i].d_line;
                    const unsigned long long  LL   = DATA[i].d_value;
                    const char               *EXP  = DATA[i].d_exp;
                    const int                 LEN  = numOctets(EXP);
                          unsigned long long  ll;

                    if (veryVerbose) { P_(i) P_(LL) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, LL));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &ll,
                                                           &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, ll  == LL);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }

            if (veryVerbose) { cout << "Testing for unsigned int" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const unsigned long long LL   = DATA[i].d_value;
                    if (LL > UINT_MAX) {
                        continue;
                    }

                    const int            LINE = DATA[i].d_line;
                    const unsigned int   VI   = (unsigned int) DATA[i].d_value;
                    const char          *EXP  = DATA[i].d_exp;
                    const int            LEN  = numOctets(EXP);
                          unsigned int   vi;

                    if (veryVerbose) { P_(i) P_(VI) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, VI));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &vi,
                                                           &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, vi  == VI);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }

            if (veryVerbose) { cout << "Testing for unsigned long" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const unsigned long long LL   = DATA[i].d_value;
                    if (LL > ULONG_MAX) {
                        continue;
                    }

                    const int            LINE = DATA[i].d_line;
                    const unsigned long  L    =
                                               (unsigned long) DATA[i].d_value;
                    const char          *EXP  = DATA[i].d_exp;
                    const int            LEN  = numOctets(EXP);
                          unsigned long  l;

                    if (veryVerbose) { P_(i) P_(L) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, L));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &l,
                                                           &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, l   == L);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }

            if (veryVerbose) { cout << "Testing for unsigned short" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const unsigned long long LL = DATA[i].d_value;
                    if (LL > USHRT_MAX) {
                        continue;
                    }

                    const int             LINE = DATA[i].d_line;
                    const unsigned short  S    =
                                              (unsigned short) DATA[i].d_value;
                    const char           *EXP  = DATA[i].d_exp;
                    const int             LEN  = numOctets(EXP);
                          unsigned short  s;

                    if (veryVerbose) { P_(i) P_(S) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, S));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &s,
                                                           &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, s   == S);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }

            if (veryVerbose) { cout << "Testing for unsigned char" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const unsigned long long LL   = DATA[i].d_value;
                    if (LL > UCHAR_MAX) {
                        continue;
                    }

                    const int            LINE = DATA[i].d_line;
                    const unsigned char  C    =
                                               (unsigned char) DATA[i].d_value;
                    const char          *EXP  = DATA[i].d_exp;
                    const int            LEN  = numOctets(EXP);
                          unsigned char  c;

                    if (veryVerbose) { P_(i) P_(C) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, C));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &c,
                                                           &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, c   == C);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for signed integral values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putValue' and 'getValue'"
                               << "\n================================="
                               << bsl::endl;

        {
            static const struct {
                int         d_line;   // line number
                long long   d_value;  // value
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                // line                value  exp
                // ----                -----  ---
                {  L_,                    0,  "01 00"                      },
                {  L_,                    1,  "01 01"                      },
                {  L_,                    5,  "01 05"                      },
                {  L_,                   -1,  "01 FF"                      },
                {  L_,                   -2,  "01 FE"                      },
                {  L_,                   -5,  "01 FB"                      },
                {  L_,                  126,  "01 7E"                      },
                {  L_,                  127,  "01 7F"                      },
                {  L_,                  128,  "02 00 80"                   },
                {  L_,                  129,  "02 00 81"                   },
                {  L_,                 -127,  "01 81"                      },
                {  L_,                 -128,  "01 80"                      },
                {  L_,                 -129,  "02 FF 7F"                   },
                {  L_,                32766,  "02 7F FE"                   },
                {  L_,                32767,  "02 7F FF"                   },
                {  L_,                32768,  "03 00 80 00"                },
                {  L_,                32769,  "03 00 80 01"                },
                {  L_,               -32767,  "02 80 01"                   },
                {  L_,               -32768,  "02 80 00"                   },
                {  L_,               -32769,  "03 FF 7F FF"                },
                {  L_,              8388606,  "03 7F FF FE"                },
                {  L_,              8388607,  "03 7F FF FF"                },
                {  L_,              8388608,  "04 00 80 00 00"             },
                {  L_,              8388609,  "04 00 80 00 01"             },
                {  L_,             -8388607,  "03 80 00 01"                },
                {  L_,             -8388608,  "03 80 00 00"                },
                {  L_,             -8388609,  "04 FF 7F FF FF"             },
                {  L_,             -8388610,  "04 FF 7F FF FE"             },
                {  L_,           2147483646,  "04 7F FF FF FE"             },
                {  L_,              INT_MAX,  "04 7F FF FF FF"             },
                {  L_,          -2147483647,  "04 80 00 00 01"             },
                {  L_,              INT_MIN,  "04 80 00 00 00"             },
                {  L_,       0x7F12345678LL,  "05 7F 12 34 56 78"          },
                {  L_,       0x7FFFFFFFFFLL,  "05 7F FF FF FF FF"          },
                {  L_,       0x8000000000LL,  "06 00 80 00 00 00 00"       },
                {  L_,       0x8000000001LL,  "06 00 80 00 00 00 01"       },
                {  L_, static_cast<long long>(0xFFFFFF8000000000LL),
                                              "05 80 00 00 00 00"          },
                {  L_, static_cast<long long>(0xFFFFFF8000000001LL),
                                              "05 80 00 00 00 01"          },
                {  L_, static_cast<long long>(0xFFFFFF8123456789LL),
                                              "05 81 23 45 67 89"          },
                {  L_, static_cast<long long>(0xFFFFFF7FFFFFFFFFLL),
                                              "06 FF 7F FF FF FF FF"       },
                {  L_,     0x7FFFFFFFFFFFLL,  "06 7F FF FF FF FF FF"       },
                {  L_,     0x800000000000LL,  "07 00 80 00 00 00 00 00"    },
                {  L_, static_cast<long long>(0xFFFF800000000000LL),
                                              "06 80 00 00 00 00 00"       },
                {  L_, static_cast<long long>(0xFFFF7FFFFFFFFFFFLL),
                                              "07 FF 7F FF FF FF FF FF"    },
                {  L_,   0x7FFFFFFFFFFFFFLL,  "07 7F FF FF FF FF FF FF"    },
                {  L_,   0x80000000000000LL,  "08 00 80 00 00 00 00 00 00" },
                {  L_, static_cast<long long>(0xFF80000000000000LL),
                                              "07 80 00 00 00 00 00 00"    },
                {  L_, static_cast<long long>(0xFF7FFFFFFFFFFFFFLL),
                                              "08 FF 7F FF FF FF FF FF FF" },
                {  L_, 0x7FFFFFFFFFFFFFFFLL,  "08 7F FF FF FF FF FF FF FF" },
                {  L_, static_cast<long long>(0x8000000000000001LL),
                                              "08 80 00 00 00 00 00 00 01" },
                {  L_, static_cast<long long>(0x8000000000000000LL),
                                              "08 80 00 00 00 00 00 00 00" },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            if (veryVerbose) { cout << "Testing for long long" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const int            LINE = DATA[i].d_line;
                    const long long      LL   = DATA[i].d_value;
                    const char          *EXP  = DATA[i].d_exp;
                    const int            LEN  = numOctets(EXP);
                          long long      ll;

                    if (veryVerbose) { P_(i) P_(LL) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, LL));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &ll,
                                                           &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0       == isb.length());
                    LOOP_ASSERT(LINE, ll  == LL);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }

            if (veryVerbose) { cout << "Testing for int" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const long long LL   = DATA[i].d_value;
                    if (LL > INT_MAX || LL < INT_MIN) {
                        continue;
                    }

                    const int      LINE = DATA[i].d_line;
                    const int      VI   = (int) DATA[i].d_value;
                    const char    *EXP  = DATA[i].d_exp;
                    const int      LEN  = numOctets(EXP);
                          int      vi;

                    if (veryVerbose) { P_(i) P_(VI) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, VI));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &vi,
                                                           &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, vi  == VI);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }

            if (veryVerbose) { cout << "Testing for long" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const long long LL   = DATA[i].d_value;
                    if (LL > LONG_MAX || LL < LONG_MIN) {
                        continue;
                    }

                    const int      LINE = DATA[i].d_line;
                    const long     L    = (long) DATA[i].d_value;
                    const char    *EXP  = DATA[i].d_exp;
                    const int      LEN  = numOctets(EXP);
                          long     l;

                    if (veryVerbose) { P_(i) P_(L) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, L));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &l,
                                                           &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, l   == L);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }

            if (veryVerbose) { cout << "Testing for short" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const long long LL   = DATA[i].d_value;
                    if (LL > SHRT_MAX || LL < SHRT_MIN) {
                        continue;
                    }

                    const int      LINE = DATA[i].d_line;
                    const short    S    = (short) DATA[i].d_value;
                    const char    *EXP  = DATA[i].d_exp;
                    const int      LEN  = numOctets(EXP);
                          short    s;

                    if (veryVerbose) { P_(i) P_(S) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, S));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &s,
                                                           &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, s   == S);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }

            if (veryVerbose) { cout << "Testing for char" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const long long LL   = DATA[i].d_value;
                    if (LL > SCHAR_MAX || LL < SCHAR_MIN) {
                        continue;
                    }

                    const int      LINE = DATA[i].d_line;
                    const char     C    = (char) DATA[i].d_value;
                    const char    *EXP  = DATA[i].d_exp;
                    const int      LEN  = numOctets(EXP);
                          char     c;

                    if (veryVerbose) { P_(i) P_(C) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, C));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &c,
                                                           &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, c   == C);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING put and get given length functions for unsigned types
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putIntegerGivenLength' "
                               << "and 'getIntegerGivenLength' for unsigned"
                               << "\n================================="
                               << "========================================"
                               << bsl::endl;

        {
            enum { SUCCESS = 0, FAILURE = -1 };

            typedef balber::BerUtil_Imp Imp;

            static const struct {
                int                 d_line;   // line number
                unsigned long long  d_value;  // int value stored in an int
                int                 d_length; // length
                const char         *d_exp;    // expected output
            } DATA[] = {
                // line             value len exp
                // ----             ----- --- ---
               { L_,                    0, 1, "00"                          },
               { L_,                    1, 1, "01"                          },
               { L_,                  127, 1, "7F"                          },
               { L_,                32767, 2, "7F FF"                       },
               { L_,              8388607, 3, "7F FF FF"                    },
               { L_,           2147483646, 4, "7F FF FF FE"                 },
               { L_,              INT_MAX, 4, "7F FF FF FF"                 },
               { L_,      0x7FFFFFFFFFULL, 5, "7F FF FF FF FF"              },
               { L_,      0x8000000000ULL, 6, "00 80 00 00 00 00"           },
               { L_,    0x7FFFFFFFFFFFULL, 6, "7F FF FF FF FF FF"           },
               { L_,    0x800000000000ULL, 7, "00 80 00 00 00 00 00"        },
               { L_,  0x7FFFFFFFFFFFFFULL, 7, "7F FF FF FF FF FF FF"        },
               { L_,  0x80000000000000ULL, 8, "00 80 00 00 00 00 00 00"     },
               { L_,0x7FFFFFFFFFFFFFFFULL, 8, "7F FF FF FF FF FF FF FF"     },
               { L_,0x8000000000000001ULL, 8, "80 00 00 00 00 00 00 01"     },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            if (veryVerbose) { cout << "Testing for unsigned long long"
                                    << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const int                 LINE = DATA[i].d_line;
                    const unsigned long long  LL   = DATA[i].d_value;
                    const int                 LEN  = DATA[i].d_length;
                    const char               *EXP  = DATA[i].d_exp;
                          unsigned long long  ll;

                    if (veryVerbose) { P_(i) P_(LL) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                    Imp::putIntegerGivenLength(&osb, LL, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS ==
                                   Imp::getIntegerValue(&isb, &ll, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, ll == LL);
                }
            }

            if (veryVerbose) { cout << "Testing for unsigned int" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const unsigned long long LL   = DATA[i].d_value;
                    if (LL > UINT_MAX) {
                        continue;
                    }

                    const int           LINE = DATA[i].d_line;
                    const unsigned int  VI   = (unsigned int) DATA[i].d_value;
                    const int           LEN  = DATA[i].d_length;
                    const char         *EXP  = DATA[i].d_exp;
                          unsigned int  vi;

                    if (veryVerbose) { P_(i) P_(VI) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                    Imp::putIntegerGivenLength(&osb, VI, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS ==
                                   Imp::getIntegerValue(&isb, &vi, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, vi == VI);
                }
            }

            if (veryVerbose) { cout << "Testing for unsigned long" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const unsigned long long LL   = DATA[i].d_value;
                    if (LL > ULONG_MAX) {
                        continue;
                    }

                    const int            LINE = DATA[i].d_line;
                    const unsigned long  L    = (unsigned long)DATA[i].d_value;
                    const int            LEN  = DATA[i].d_length;
                    const char          *EXP  = DATA[i].d_exp;
                          unsigned long  l;

                    if (veryVerbose) { P_(i) P_(L) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                     Imp::putIntegerGivenLength(&osb, L, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS ==
                                    Imp::getIntegerValue(&isb, &l, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, l == L);
                }
            }

            if (veryVerbose) { cout << "Testing for unsigned short" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const unsigned long long LL   = DATA[i].d_value;
                    if (LL > USHRT_MAX) {
                        continue;
                    }

                    const int             LINE = DATA[i].d_line;
                    const unsigned short  S    =
                                              (unsigned short) DATA[i].d_value;
                    const int             LEN  = DATA[i].d_length;
                    const char           *EXP  = DATA[i].d_exp;
                          unsigned short  s;

                    if (veryVerbose) { P_(i) P_(S) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                     Imp::putIntegerGivenLength(&osb, S, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS ==
                                   Imp::getIntegerValue(&isb, &s, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, s == S);
                }
            }

            if (veryVerbose) { cout << "Testing for unsigned char" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const unsigned long long LL   = DATA[i].d_value;
                    if (LL > UCHAR_MAX) {
                        continue;
                    }

                    const int            LINE = DATA[i].d_line;
                    const unsigned char  C    = (unsigned char)DATA[i].d_value;
                    const int            LEN  = DATA[i].d_length;
                    const char          *EXP  = DATA[i].d_exp;
                          unsigned char  c;

                    if (veryVerbose) { P_(i) P_(C) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                    Imp::putIntegerGivenLength(&osb, C, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS ==
                                   Imp::getIntegerValue(&isb, &c, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, c == C);
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'putIntegerGivenLength' & 'getIntegerValue'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putIntegerGivenLength' "
                               << "and 'getIntegerValue'"
                               << "\n================================="
                               << "==========================="
                               << bsl::endl;

        {
            enum { SUCCESS = 0, FAILURE = -1 };

            typedef balber::BerUtil_Imp Imp;

            static const struct {
                int         d_line;   // line number
                long long   d_value;  // int value stored in an int
                int         d_length; // length
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                // line                value len  exp
                // ----                ----- ---  ---
                {  L_,                    0,  1,  "00"                      },
                {  L_,                    1,  1,  "01"                      },
                {  L_,                   -2,  1,  "FE"                      },
                {  L_,                  127,  1,  "7F"                      },
                {  L_,                  128,  2,  "00 80"                   },
                {  L_,                 -128,  1,  "80"                      },
                {  L_,                 -129,  2,  "FF 7F"                   },
                {  L_,                32767,  2,  "7F FF"                   },
                {  L_,                32768,  3,  "00 80 00"                },
                {  L_,               -32768,  2,  "80 00"                   },
                {  L_,               -32769,  3,  "FF 7F FF"                },
                {  L_,              8388607,  3,  "7F FF FF"                },
                {  L_,              8388608,  4,  "00 80 00 00"             },
                {  L_,             -8388608,  3,  "80 00 00"                },
                {  L_,             -8388609,  4,  "FF 7F FF FF"             },
                {  L_,           2147483646,  4,  "7F FF FF FE"             },
                {  L_,              INT_MAX,  4,  "7F FF FF FF"             },
                {  L_,          -2147483647,  4,  "80 00 00 01"             },
                {  L_,              INT_MIN,  4,  "80 00 00 00"             },
                {  L_,       0x7FFFFFFFFFLL,  5,  "7F FF FF FF FF"          },
                {  L_,       0x8000000000LL,  6,  "00 80 00 00 00 00"       },
                {  L_, static_cast<long long>(0xFFFFFF8000000000LL),
                                              5,  "80 00 00 00 00"          },
                {  L_, static_cast<long long>(0xFFFFFF7FFFFFFFFFLL),
                                              6,  "FF 7F FF FF FF FF"       },
                {  L_,     0x7FFFFFFFFFFFLL,  6,  "7F FF FF FF FF FF"       },
                {  L_,     0x800000000000LL,  7,  "00 80 00 00 00 00 00"    },
                {  L_, static_cast<long long>(0xFFFF800000000000LL),
                                              6,  "80 00 00 00 00 00"       },
                {  L_, static_cast<long long>(0xFFFF7FFFFFFFFFFFLL),
                                              7,  "FF 7F FF FF FF FF FF"    },
                {  L_,   0x7FFFFFFFFFFFFFLL,  7,  "7F FF FF FF FF FF FF"    },
                {  L_,   0x80000000000000LL,  8,  "00 80 00 00 00 00 00 00" },
                {  L_, static_cast<long long>(0xFF80000000000000LL),
                                              7,  "80 00 00 00 00 00 00"    },
                {  L_, static_cast<long long>(0xFF7FFFFFFFFFFFFFLL),
                                              8,  "FF 7F FF FF FF FF FF FF" },
                {  L_, 0x7FFFFFFFFFFFFFFFLL,  8,  "7F FF FF FF FF FF FF FF" },
                {  L_, static_cast<long long>(0x8000000000000001LL),
                                              8,  "80 00 00 00 00 00 00 01" },
                {  L_, static_cast<long long>(0x8000000000000000LL),
                                              8,  "80 00 00 00 00 00 00 00" },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            if (veryVerbose) { cout << "Testing for long long" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const int            LINE = DATA[i].d_line;
                    const long long      LL   = DATA[i].d_value;
                    const int            LEN  = DATA[i].d_length;
                    const char          *EXP  = DATA[i].d_exp;
                          long long      ll;

                    if (veryVerbose) { P_(i) P_(LL) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                    Imp::putIntegerGivenLength(&osb, LL, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS ==
                                   Imp::getIntegerValue(&isb, &ll, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, ll == LL);
                }
            }

            if (veryVerbose) { cout << "Testing for int" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const long long LL   = DATA[i].d_value;
                    if (LL > INT_MAX || LL < INT_MIN) {
                        continue;
                    }

                    const int      LINE = DATA[i].d_line;
                    const int      VI   = (int) DATA[i].d_value;
                    const int      LEN  = DATA[i].d_length;
                    const char    *EXP  = DATA[i].d_exp;
                          int      vi;

                    if (veryVerbose) { P_(i) P_(VI) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                    Imp::putIntegerGivenLength(&osb, VI, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS ==
                                   Imp::getIntegerValue(&isb, &vi, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, vi == VI);
                }
            }

            if (veryVerbose) { cout << "Testing for long" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const long long LL   = DATA[i].d_value;
                    if (LL > LONG_MAX || LL < LONG_MIN) {
                        continue;
                    }

                    const int      LINE = DATA[i].d_line;
                    const long     L    = (long) DATA[i].d_value;
                    const int      LEN  = DATA[i].d_length;
                    const char    *EXP  = DATA[i].d_exp;
                          long     l;

                    if (veryVerbose) { P_(i) P_(L) P_(LEN) P(EXP) }

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                     Imp::putIntegerGivenLength(&osb, L, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS ==
                                    Imp::getIntegerValue(&isb, &l, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, l == L);
                }
            }

            if (veryVerbose) { cout << "Testing for short" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const long long LL   = DATA[i].d_value;
                    if (LL > SHRT_MAX || LL < SHRT_MIN) {
                        continue;
                    }

                    const int    LINE = DATA[i].d_line;
                    const short  S    = (short) DATA[i].d_value;
                    const int    LEN  = DATA[i].d_length;
                    const char  *EXP  = DATA[i].d_exp;

                    if (veryVerbose) { P_(i) P_(S) P_(LEN) P(EXP) }

                    short s;

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                     Imp::putIntegerGivenLength(&osb, S, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS ==
                                   Imp::getIntegerValue(&isb, &s, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, s == S);
                }
            }

            if (veryVerbose) { cout << "Testing for char" << endl; }
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const long long LL   = DATA[i].d_value;
                    if (LL > SCHAR_MAX || LL < SCHAR_MIN) {
                        continue;
                    }

                    const int   LINE = DATA[i].d_line;
                    const char  C    = (char) DATA[i].d_value;
                    const int   LEN  = DATA[i].d_length;
                    const char *EXP  = DATA[i].d_exp;

                    if (veryVerbose) { P_(i) P_(C) P_(LEN) P(EXP) }

                    char     c;

                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                    Imp::putIntegerGivenLength(&osb, C, LEN));
                    LOOP_ASSERT(LINE, LEN == (int)osb.length());
                    LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS ==
                                   Imp::getIntegerValue(&isb, &c, LEN));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, c == C);
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'numBytesToStream' for unsigned types
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   int numBytesToStream(TYPE value);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'numBytesToStream' for unsigned"
                               << "\n======================================="
                               << bsl::endl;

        {
            static const struct {
                int                d_line;   // line number
                unsigned long long d_value;  // integer value
                int                d_result; // expected result
            } DATA[] = {
                // line                        value       res
                // ----                        -----       ---
                {  L_,                            0,         1 },
                {  L_,                            1,         1 },
                {  L_,                          127,         1 },
                {  L_,                          128,         2 },
                {  L_,                        32767,         2 },
                {  L_,                        32768,         3 },
                {  L_,                      8388607,         3 },
                {  L_,                      8388608,         4 },
                {  L_,                   2147483646,         4 },
                {  L_,                      INT_MAX,         4 },
                {  L_,               0x7FFFFFFFFFLL,         5 },
                {  L_,               0x8000000000LL,         6 },
                {  L_,             0x7FFFFFFFFFFFLL,         6 },
                {  L_,             0x800000000000LL,         7 },
                {  L_,           0x7FFFFFFFFFFFFFLL,         7 },
                {  L_,           0x80000000000000LL,         8 },
                {  L_,         0x7FFFFFFFFFFFFFFFLL,         8 },
                {  L_,         0x8000000000000000LL,         9 },
                {  L_,         0xFFFFFFFFFFFFFFFFLL,         9 },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE = DATA[i].d_line;
                const unsigned long long VAL  = DATA[i].d_value;
                const int                RES  = DATA[i].d_result;

                if (veryVerbose) { P_(i) P(VAL) }

                LOOP3_ASSERT(
                            LINE, VAL, RES,
                            RES == balber::BerUtil_Imp::numBytesToStream(VAL));
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'numBytesToStream'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   int numBytesToStream(TYPE value);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'numBytesToStream'"
                               << "\n=========================="
                               << bsl::endl;

        {
            static const struct {
                int       d_line;   // line number
                long long d_value;  // integer value
                int       d_result; // expected result
            } DATA[] = {
                // line                                        value       res
                // ----                                        -----       ---
                {  L_,                                             0,      1 },
                {  L_,                                             1,      1 },
                {  L_,                                            -1,      1 },
                {  L_,                                           127,      1 },
                {  L_,                                           128,      2 },
                {  L_,                                          -128,      1 },
                {  L_,                                          -129,      2 },
                {  L_,                                         32767,      2 },
                {  L_,                                         32768,      3 },
                {  L_,                                        -32768,      2 },
                {  L_,                                        -32769,      3 },
                {  L_,                                       8388607,      3 },
                {  L_,                                       8388608,      4 },
                {  L_,                                      -8388608,      3 },
                {  L_,                                      -8388609,      4 },
                {  L_,                                    2147483646,      4 },
                {  L_,                                       INT_MAX,      4 },
                {  L_,                                   -2147483647,      4 },
                {  L_,                                       INT_MIN,      4 },

                {  L_,                               0x7FFFFFFFFFLL ,      5 },
                {  L_,                               0x8000000000LL ,      6 },
                {  L_,  static_cast<long long>(0xFFFFFF8000000000LL),      5 },
                {  L_,  static_cast<long long>(0xFFFFFF7FFFFFFFFFLL),      6 },
                {  L_,                             0x7FFFFFFFFFFFLL ,      6 },
                {  L_,                             0x800000000000LL ,      7 },
                {  L_,  static_cast<long long>(0xFFFF800000000000LL),      6 },
                {  L_,  static_cast<long long>(0xFFFF7FFFFFFFFFFFLL),      7 },
                {  L_,                           0x7FFFFFFFFFFFFFLL ,      7 },
                {  L_,                           0x80000000000000LL ,      8 },
                {  L_,  static_cast<long long>(0xFF80000000000000LL),      7 },
                {  L_,  static_cast<long long>(0xFF7FFFFFFFFFFFFFLL),      8 },
                {  L_,                         0x7FFFFFFFFFFFFFFFLL ,      8 },
                {  L_,  static_cast<long long>(0x8000000000000001LL),      8 },
                {  L_,  static_cast<long long>(0x8000000000000000LL),      8 },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int       LINE = DATA[i].d_line;
                const long long LL   = DATA[i].d_value;
                const int       RES  = DATA[i].d_result;

                if (veryVerbose) { P_(i) P(LL) }

                LOOP4_ASSERT(
                      LINE, LL, RES, balber::BerUtil_Imp::numBytesToStream(LL),
                      RES == balber::BerUtil_Imp::numBytesToStream(LL));

                if (LL <= SHRT_MAX && LL >= SHRT_MIN) {
                    const short S = (short) LL;
                    const int   I = (int)   LL;
                    const long  L = (long)  LL;

                    LOOP4_ASSERT(
                        LINE, S, RES, balber::BerUtil_Imp::numBytesToStream(S),
                        RES == balber::BerUtil_Imp::numBytesToStream(S));
                    LOOP4_ASSERT(
                        LINE, I, RES, balber::BerUtil_Imp::numBytesToStream(I),
                        RES == balber::BerUtil_Imp::numBytesToStream(I));
                    LOOP4_ASSERT(
                        LINE, L, RES, balber::BerUtil_Imp::numBytesToStream(L),
                        RES == balber::BerUtil_Imp::numBytesToStream(L));
                    continue;
                }

                if (LL <= INT_MAX && LL >= INT_MIN) {
                    const int  I = (int)  LL;
                    const long L = (long) LL;

                    LOOP3_ASSERT(
                              LINE, I, RES,
                              RES == balber::BerUtil_Imp::numBytesToStream(I));
                    LOOP3_ASSERT(
                              LINE, L, RES,
                              RES == balber::BerUtil_Imp::numBytesToStream(L));
                }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for unsigned char
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putValue' and 'getValue'"
                               << "\n================================="
                               << bsl::endl;

        {
            static const struct {
                int            d_line;   // line number
                unsigned char  d_value;  // unsigned char value
                const char    *d_exp;    // expected output onto the stream
            } DATA[] = {
                // line   value       exp
                // ----   -----       ---
                {  L_,       0,      "01 00"    },
                {  L_,       1,      "01 01"    },
                {  L_,      95,      "01 5F"    },
                {  L_,     127,      "01 7F"    },
                {  L_,     128,      "02 00 80" },
                {  L_,     200,      "02 00 C8" },
                {  L_,     255,      "02 00 FF" },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int            LINE = DATA[i].d_line;
                const unsigned char  UC   = DATA[i].d_value;
                const char          *EXP  = DATA[i].d_exp;
                const int            LEN  = numOctets(EXP);

                unsigned char  uc;

                {
                    bdlsb::MemOutStreamBuf osb;

                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, UC));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(
                                                           &isb,
                                                           &uc,
                                                           &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, uc  == UC);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for signed char values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putValue' and 'getValue'"
                               << "\n================================="
                               << bsl::endl;

        {
            static const struct {
                int         d_line;   // line number
                char        d_value;  // char value
                const char *d_exp;    // expected output onto the stream
            } DATA[] = {
                // line   value       exp
                // ----   -----       ---
                {  L_,       0,      "01 00" },
                {  L_,       1,      "01 01" },
                {  L_,      -2,      "01 FE" },
                {  L_,      95,      "01 5F" },
                {  L_,    -106,      "01 96" },
                {  L_,     127,      "01 7F" },
                {  L_,    -127,      "01 81" },
                {  L_,    -128,      "01 80" },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE = DATA[i].d_line;
                const char         C    = DATA[i].d_value;
                const signed char  SC   = (signed char) DATA[i].d_value;
                const char        *EXP  = DATA[i].d_exp;
                const int          LEN  = numOctets(EXP);

                char          c;
                signed char  sc;

                if (veryVerbose) { cout << "\nTesting char values" << endl; }
                {
                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, C));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0       ==
                                              compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &c,
                                                           &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, c == C);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }

                if (veryVerbose) { cout << "\nTesting signed char values"
                                        << endl; }
                {
                    bdlsb::MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, SC));
                    LOOP_ASSERT(LINE, LEN     == (int)osb.length());
                    LOOP_ASSERT(LINE, 0       ==
                                              compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(
                                                           &isb,
                                                           &sc,
                                                           &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0   == isb.length());
                    LOOP_ASSERT(LINE, sc  == SC);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'putValue' & 'getValue' for bool values
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'putValue' and 'getValue'"
                               << "\n================================="
                               << bsl::endl;

        {
            const bool XA1 = true;  bool XA2; const char *EA = "01 01";
            const bool XB1 = false; bool XB2; const char *EB = "01 00";

            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(SUCCESS  == Util::putValue(&osb, XA1));
                ASSERT(2        == osb.length());
                ASSERT(0        == compareBuffers(osb.data(), EA));

                if (veryVerbose) {
                    printBuffer(osb.data(), osb.length());
                }

                int numBytesConsumed = 0;
                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS == Util::getValue(&isb, &XA2,
                                                 &numBytesConsumed));
                ASSERT(0       == isb.length());
                ASSERT(XA1     == XA2);
                ASSERT(2       == numBytesConsumed);
            }

            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(SUCCESS  == Util::putValue(&osb, XB1));
                ASSERT(2        == osb.length());
                ASSERT(0        == compareBuffers(osb.data(), EB));

                if (veryVerbose) {
                    printBuffer(osb.data(), osb.length());
                }

                int numBytesConsumed = 0;
                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS == Util::getValue(&isb, &XB2,
                                                 &numBytesConsumed));
                ASSERT(0       == isb.length());
                ASSERT(XB1     == XB2);
                ASSERT(2       == numBytesConsumed);
            }
        }
      } break;
      case 1: {
        // BREATHING TEST
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //
        // Concerns:  There are several concerns about performance.  For one
        //   thing, the use of 'bsl::streambuf' can slow things down (if the
        //   length of the buffer is known in advance, a direct access through
        //   a 'bdlsb::FixedMemInput' or 'bdlsb::FixedMemOutput' can bypass
        //   virtual function calls.  Then there is the encoding/decoding
        //   proper.
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        bsl::cout << "\nPERFORMANCE TESTING"
                  << "\n==================="
                  << bsl::endl;

        enum { MAX_ITER = 10000000 };
        int maxIter = MAX_ITER;
        if (argc > 2) {
            maxIter = bsl::atoi(argv[2]);
        }

        bsls::Stopwatch timer;

        char buffer[64];
        bdlsb::FixedMemOutStreamBuf osb(buffer, 64);
        bdlsb::FixedMemInStreamBuf isb(buffer, 64);
        timer.start();
        for (int i = 0; i < maxIter; ++i) {
            osb.pubseekpos(0);           // avoid creating streambuf every time
            isb.pubseekpos(0);           // avoid creating streambuf every time
            const bool B = i & 0x1;
            Util::putValue(&osb, B);

            int   numBytesConsumed = 0;
            bool  b                = true;
            Util::getValue(&isb, &b, &numBytesConsumed);
        }
        timer.stop();
        bsl::cout << maxIter << " iterations for put(bool)/get(&bool): "
                  << timer.elapsedTime() << bsl::endl;

        timer.reset();
        timer.start();
        for (int i = 0; i < maxIter; ++i) {
            osb.pubseekpos(0);           // avoid creating streambuf every time
            isb.pubseekpos(0);           // avoid creating streambuf every time
            const char C = '0' + i % 64;
            Util::putValue(&osb, C);

            int   numBytesConsumed = 0;
            char  c                = ' ';
            Util::getValue(&isb, &c, &numBytesConsumed);
        }
        timer.stop();
        bsl::cout << maxIter << " iterations for put(char)/get(&char): "
                  << timer.elapsedTime() << bsl::endl;

        timer.reset();
        timer.start();
        int I = 1;
        for (int i = 0; i < maxIter / 2; ++i) {
            osb.pubseekpos(0);           // avoid creating streambuf every time
            isb.pubseekpos(0);           // avoid creating streambuf every time

            Util::putValue(&osb, I);
            Util::putValue(&osb, -I);

            int   numBytesConsumed = 0;
            int   x                = 0;
            Util::getValue(&isb, &x, &numBytesConsumed);
            Util::getValue(&isb, &x, &numBytesConsumed);

            if (I > INT_MAX / 7) {
                I = 1;
            }
            else {
                I *= 7;
            }
        }
        timer.stop();
        bsl::cout << maxIter << " iterations for put(int)/get(&int): "
                  << timer.elapsedTime() << bsl::endl;

        timer.reset();
        timer.start();
        long long LL = 1;
        for (int i = 0; i < maxIter / 2; ++i) {
            osb.pubseekpos(0);           // avoid creating streambuf every time
            isb.pubseekpos(0);           // avoid creating streambuf every time

            Util::putValue(&osb, LL);
            Util::putValue(&osb, -LL);

            int   numBytesConsumed = 0;
            long long   x                = 0;
            Util::getValue(&isb, &x, &numBytesConsumed);
            Util::getValue(&isb, &x, &numBytesConsumed);

            if (LL > LLONG_MAX / 7) {
                LL = 1;
            }
            else {
                LL *= 7;
            }
        }
        timer.stop();
        bsl::cout << maxIter
                  << " iterations for put(long long)/get(&long long): "
                  << timer.elapsedTime() << bsl::endl;

        timer.reset();
        const double F = 3.1415927;
        timer.start();
        for (int i = 0; i < maxIter; ++i) {
            osb.pubseekpos(0);           // avoid creating streambuf every time
            isb.pubseekpos(0);           // avoid creating streambuf every time
            Util::putValue(&osb, F);

            int   numBytesConsumed = 0;
            float f                = 0.0f;
            Util::getValue(&isb, &f, &numBytesConsumed);
        }
        timer.stop();
        bsl::cout << maxIter << " iterations for put(double)/get(&double): "
                  << timer.elapsedTime() << bsl::endl;

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
