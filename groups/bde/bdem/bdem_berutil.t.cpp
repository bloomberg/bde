// bdem_berutil.t.cpp                  -*-C++-*-
#include <bdem_berutil.h>

#include <bdem_berconstants.h>

#include <bslalg_typetraits.h>

#include <bdeat_attributeinfo.h>
#include <bdeat_selectioninfo.h>
#include <bdeat_typetraits.h>
#include <bdeat_valuetypefunctions.h>

#include <bsls_platformutil.h>

#include <bsls_objectbuffer.h>
#include <bsls_platformutil.h>
#include <bsls_stopwatch.h>

#include <bdeimp_prolepticdateutil.h>

#include <bdesb_memoutstreambuf.h>            // for testing only
#include <bdesb_fixedmemoutstreambuf.h>       // for testing only
#include <bdesb_fixedmeminstreambuf.h>        // for testing only

#include <bdet_date.h>
#include <bdet_datetime.h>
#include <bdet_time.h>

#include <bdet_datetz.h>
#include <bdet_datetimetz.h>
#include <bdet_timetz.h>

#include <bdeu_print.h>
#include <bdeu_printmethods.h>

#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_climits.h>
#include <bsl_cstdlib.h>

#include <bsl_bitset.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
                          << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\n";           \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J    \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\t" << #M << ": " << M << "\n";         \
               aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J     \
                         << "\t" << #K << ": " << K << "\t" << #L << ": "  \
                         << L << "\t" << #M << ": " << M << "\t" << #N     \
                         << ": " << N << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl; // Print ID and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;  // Quote ID literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << flush; // P(X) w/o '\n'
#define L_ __LINE__                                // current Line number
#define T_ bsl::cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };
enum { SUCCESS = 0, FAILURE = -1 };

typedef bsls_PlatformUtil::Int64  Int64;
typedef bsls_PlatformUtil::Uint64 Uint64;

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

typedef bdem_BerUtil Util;
typedef bslstl_StringRef StringRef;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

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
        return c - '0';
    }
    c = toupper(c);
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
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
           return -1;
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

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
//  #include <bdem_berconstants.h>
//  #include <bdem_berutil.h>
//  #include <bdesb_fixedmeminstreambuf.h>
//  #include <bdesb_memoutstreambuf.h>
//
//  using namespace BloombergLP;
//
void usageExample()
{
    bdesb_MemOutStreamBuf osb;

    bdem_BerConstants::TagClass tagClass =
                                    bdem_BerConstants::BDEM_CONTEXT_SPECIFIC;
    bdem_BerConstants::TagType  tagType  = bdem_BerConstants::BDEM_PRIMITIVE;
    int                         tagNumber= 31;

    int retCode = bdem_BerUtil::putIdentifierOctets(&osb,
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
    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());

    bdem_BerConstants::TagClass tagClassIn;
    bdem_BerConstants::TagType  tagTypeIn;
    int                         tagNumberIn;
    int                         numBytesConsumed = 0;

    retCode = bdem_BerUtil::getIdentifierOctets(&isb,
                                                &tagClassIn,
                                                &tagTypeIn,
                                                &tagNumberIn,
                                                &numBytesConsumed);
    ASSERT(0         == retCode);
    ASSERT(2         == numBytesConsumed);
    ASSERT(tagClass  == tagClassIn);
    ASSERT(tagType   == tagTypeIn);
    ASSERT(tagNumber == tagNumberIn);
}
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 20: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Usage Example"
                               << "\n=====================" << bsl::endl;

        usageExample();

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING BRUTE FORCE 'putValue'/'getValue' for date/time components
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

        bdem_BerEncoderOptions options;
        options.setEncodeDateAndTimeTypesAsBinary(true);

        if (verbose) bsl::cout << "\nTesting Date Brute force." << bsl::endl;
        {
            typedef bdeimp_ProlepticDateUtil ProlepticDateUtil;
            typedef bdeimp_DateUtil          DateUtil;

            const int YEARS[] = { 1, 4, 96, 100, 400, 500, 800, 1000, 1600,
                                  1700, 1751, 1752, 1753, 1930, 2000, 2010,
                                  2012, 2019, 2020, 2021, 6478, 6479, 6480,
                                  9998, 9999 };
            const int NUM_YEARS = sizeof YEARS / sizeof *YEARS;

            const int MONTHS[] = { 1, 2, 5, 8, 9, 12 };
            const int NUM_MONTHS = sizeof MONTHS / sizeof *MONTHS;

            const int DAYS[] = { 1, 2, 5, 10, 15, 20, 28, 29, 30, 31 };
            const int NUM_DAYS = sizeof DAYS / sizeof *DAYS;

            for (int i = 0; i <= NUM_YEARS; ++i) {
            for (int j = 0; j <= NUM_MONTHS; ++j) {
            for (int k = 0; k <= NUM_DAYS; ++k) {

                const int YEAR  = YEARS[i];
                const int MONTH = MONTHS[j];
                const int DAY   = DAYS[k];

                if (DateUtil::isValidCalendarDate(YEAR, MONTH, DAY)
                 && ProlepticDateUtil::isValidCalendarDate(YEAR, MONTH, DAY)) {

                    if (veryVerbose) { P_(YEAR) P_(MONTH) P(DAY) }
                    {
                        const bdet_Date VALUE(YEAR, MONTH, DAY);
                        bdet_Date value;

                        bdesb_MemOutStreamBuf osb;
                        ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                        const int LENGTH = osb.length();

                        if (veryVerbose) {
                            cout << "Output Buffer:";
                            printBuffer(osb.data(), osb.length());
                        }
                        int numBytesConsumed = 0;

                        bdesb_FixedMemInStreamBuf isb(osb.data(),
                                                      osb.length());
                        ASSERT(SUCCESS == Util::getValue(&isb,
                                                         &value,
                                                         &numBytesConsumed));
                        ASSERT(0      == isb.length());
                        ASSERT(LENGTH == numBytesConsumed);
                        LOOP2_ASSERT(VALUE, value, VALUE == value);
                    }

                    {
                        const int OFF1 = 0, OFF2 = -1439, OFF3 = 1439;
                        const bdet_DateTz VALUE1(bdet_Date(YEAR, MONTH, DAY),
                                                 OFF1);
                        const bdet_DateTz VALUE2(bdet_Date(YEAR, MONTH, DAY),
                                                 OFF2);
                        const bdet_DateTz VALUE3(bdet_Date(YEAR, MONTH, DAY),
                                                 OFF3);
                        bdet_DateTz value1, value2, value3;

                        bdesb_MemOutStreamBuf osb1, osb2, osb3;

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

                        bdesb_FixedMemInStreamBuf isb1(osb1.data(),
                                                       osb1.length());
                        bdesb_FixedMemInStreamBuf isb2(osb2.data(),
                                                       osb2.length());
                        bdesb_FixedMemInStreamBuf isb3(osb3.data(),
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
            typedef bdeimp_ProlepticDateUtil ProlepticDateUtil;
            typedef bdeimp_DateUtil          DateUtil;

            for (int hour = 0; hour <= 23; ++hour) {
                for (int min = 0; min < 60; ++min) {
                    for (int sec = 0; sec < 60; ++sec) {
                        if (veryVerbose) { P_(hour) P_(min) P(sec) }
                        {
                            const int MS = 0;
                            const bdet_Time VALUE(hour, min, sec, MS);
                            bdet_Time value;

                            bdesb_MemOutStreamBuf osb;
                            ASSERT(0 == Util::putValue(&osb,
                                                       VALUE,
                                                       &options));
                            const int LENGTH = osb.length();

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb.data(), osb.length());
                            }
                            int numBytesConsumed = 0;

                            bdesb_FixedMemInStreamBuf isb(osb.data(),
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
                            const bdet_Time VALUE(hour, min, sec, MS);
                            bdet_Time value;

                            bdesb_MemOutStreamBuf osb;
                            ASSERT(0 == Util::putValue(&osb,
                                                       VALUE,
                                                       &options));
                            const int LENGTH = osb.length();

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb.data(), osb.length());
                            }
                            int numBytesConsumed = 0;

                            bdesb_FixedMemInStreamBuf isb(osb.data(),
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
                            const int MS1 = 0, MS2 = 500, MS3 = 999;
                            const int OFF1 = 0, OFF2 = -1439, OFF3 = 1439;
                            const bdet_TimeTz VALUE1(bdet_Time(hour,
                                                               min,
                                                               sec,
                                                               MS1),
                                                     OFF1);
                            const bdet_TimeTz VALUE2(bdet_Time(hour,
                                                               min,
                                                               sec,
                                                               MS2),
                                                     OFF2);
                            const bdet_TimeTz VALUE3(bdet_Time(hour,
                                                               min,
                                                               sec,
                                                               MS3),
                                                     OFF3);
                            bdet_TimeTz value1, value2, value3;

                            bdesb_MemOutStreamBuf osb1, osb2, osb3;

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

                            bdesb_FixedMemInStreamBuf isb1(osb1.data(),
                                                           osb1.length());
                            bdesb_FixedMemInStreamBuf isb2(osb2.data(),
                                                           osb2.length());
                            bdesb_FixedMemInStreamBuf isb3(osb3.data(),
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
            typedef bdeimp_ProlepticDateUtil ProlepticDateUtil;
            typedef bdeimp_DateUtil          DateUtil;

            const int YEARS[] = { 1, 4, 96, 100, 400, 500, 800, 1000, 1600,
                                  1700, 1751, 1752, 1753, 1930, 2000, 2010,
                                  2012, 2019, 2020, 2021, 6478, 6479, 6480,
                                  9998, 9999 };
            const int NUM_YEARS = sizeof YEARS / sizeof *YEARS;

            const int MONTHS[] = { 1, 2, 5, 8, 9, 12 };
            const int NUM_MONTHS = sizeof MONTHS / sizeof *MONTHS;

            const int DAYS[] = { 1, 2, 5, 10, 15, 20, 28, 29, 30, 31 };
            const int NUM_DAYS = sizeof DAYS / sizeof *DAYS;

            for (int di = 0; di <= NUM_YEARS; ++di) {
            for (int dj = 0; dj <= NUM_MONTHS; ++dj) {
            for (int dk = 0; dk <= NUM_DAYS; ++dk) {

                const int YEAR  = YEARS[di];
                const int MONTH = MONTHS[dj];
                const int DAY   = DAYS[dk];

                if (DateUtil::isValidCalendarDate(YEAR, MONTH, DAY)
                 && ProlepticDateUtil::isValidCalendarDate(YEAR, MONTH, DAY)) {

                    const int HOURS[] = { 0, 12, 23 };
                    const int NUM_HOURS = sizeof HOURS / sizeof *HOURS;

                    const int MINS[] = { 0, 30, 59 };
                    const int NUM_MINS = sizeof MINS / sizeof *MINS;

                    const int SECS[] = { 0, 30, 59 };
                    const int NUM_SECS = sizeof SECS / sizeof *SECS;

                    for (int ti = 0; ti < NUM_HOURS; ++ti) {
                    for (int tj = 0; tj < NUM_MINS; ++tj) {
                    for (int tk = 0; tk < NUM_SECS; ++tk) {

                        const int HOUR = HOURS[ti];
                        const int MIN  = MINS[tj];
                        const int SEC  = SECS[tk];

                        if (veryVerbose) { P_(YEAR) P_(MONTH) P(DAY) }
                        if (veryVerbose) { P_(HOUR) P_(MIN) P(SEC) }
                        {
                            const int MS = 0;
                            const bdet_Date DATE(YEAR, MONTH, DAY);
                            const bdet_Time TIME(HOUR, MIN, SEC, MS);
                            const bdet_Datetime VALUE(DATE, TIME);
                            bdet_Datetime value;

                            bdesb_MemOutStreamBuf osb;
                            ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                            const int LENGTH = osb.length();

                            if (veryVerbose) {
                                cout << "Output Buffer:";
                                printBuffer(osb.data(), osb.length());
                            }
                            int numBytesConsumed = 0;

                            bdesb_FixedMemInStreamBuf isb(osb.data(),
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
                            const int MS1 = 0, MS2 = 500, MS3 = 999;
                            const int OFF1 = 0, OFF2 = -1439, OFF3 = 1439;
                            const bdet_Date DATE1(YEAR, MONTH, DAY);
                            const bdet_Time TIME1(HOUR, MIN, SEC, MS1);

                            const bdet_Date DATE2(YEAR, MONTH, DAY);
                            const bdet_Time TIME2(HOUR, MIN, SEC, MS2);

                            const bdet_Date DATE3(YEAR, MONTH, DAY);
                            const bdet_Time TIME3(HOUR, MIN, SEC, MS3);

                            const bdet_Datetime DT1(DATE1, TIME1);
                            const bdet_Datetime DT2(DATE2, TIME2);
                            const bdet_Datetime DT3(DATE3, TIME3);

                            const bdet_DatetimeTz VALUE1(DT1, OFF1);
                            const bdet_DatetimeTz VALUE2(DT2, OFF2);
                            const bdet_DatetimeTz VALUE3(DT3, OFF3);

                            bdet_DatetimeTz value1, value2, value3;

                            bdesb_MemOutStreamBuf osb1, osb2, osb3;

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

                            bdesb_FixedMemInStreamBuf isb1(osb1.data(),
                                                           osb1.length());
                            bdesb_FixedMemInStreamBuf isb2(osb2.data(),
                                                           osb2.length());
                            bdesb_FixedMemInStreamBuf isb3(osb3.data(),
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

        bdem_BerEncoderOptions options;
        options.setEncodeDateAndTimeTypesAsBinary(true);

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

    {      L_,      1,     1,    1,     1,     1,    1,    1,      0     },
    {      L_,      1,     1,    1,     1,     1,    1,    1,    500     },

    {      L_,      1,     1,    1,     1,    23,   59,   59,      0     },
    {      L_,      1,     1,    1,     1,    23,   59,   59,    999     },

    {      L_,      1,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,    2,     0,     0,    0,    0,   1439     },

    {      L_,      1,     1,    2,     1,     1,    1,    1,      0     },
    {      L_,      1,     1,    2,     1,     1,    1,    1,    500     },

    {      L_,      1,     1,    2,     1,    23,   59,   59,      0     },
    {      L_,      1,     1,    2,     1,    23,   59,   59,    999     },

    {      L_,      1,     1,   10,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,   10,     1,     1,    1,    1,     99     },

    {      L_,      1,     1,   30,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,   31,     0,     0,    0,    0,   1439     },

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

    {      L_,      4,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,      4,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,      4,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,      4,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,      8,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,      8,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,      8,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,      8,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,      8,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,      8,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,    100,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    100,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,    100,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    100,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,    400,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    400,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,    400,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,    400,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,    400,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    400,     3,    1,    23,    59,   59,    0,   1439     },

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

    {      L_,   2020,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   2020,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,   2020,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,   2020,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,   2020,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   2020,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   2021,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,   2022,     1,    2,     0,     0,    0,    0,      0     },

    {      L_,   9999,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   9999,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,   9999,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   9999,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   9999,    12,   30,     0,     0,    0,    0,      0     },
    {      L_,   9999,    12,   30,    23,    59,   59,    0,   1439     },

    {      L_,   9999,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   9999,    12,   31,    23,    59,   59,    0,   1439     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) bsl::cout << "\nTesting 'bdet_Date'." << bsl::endl;
        {
            typedef bdet_Date Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;

                const Type VALUE(Y, M, D); Type value;

                bdesb_MemOutStreamBuf osb;
                ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                const int LENGTH = osb.length();

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }
                int numBytesConsumed = 0;

                bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS == Util::getValue(&isb,
                                                 &value,
                                                 &numBytesConsumed));
                ASSERT(0      == isb.length());
                ASSERT(LENGTH == numBytesConsumed);
                LOOP2_ASSERT(VALUE, value, VALUE == value);
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdet_DateTz'." << bsl::endl;
        {
            typedef bdet_DateTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdet_Date(Y, M, D), OFF); Type value;

                bdesb_MemOutStreamBuf osb;
                ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                const int LENGTH = osb.length();

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }
                int numBytesConsumed = 0;

                bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS == Util::getValue(&isb,
                                                 &value,
                                                 &numBytesConsumed));
                ASSERT(0       == isb.length());
                ASSERT(LENGTH  == numBytesConsumed);
                LOOP2_ASSERT(VALUE, value, VALUE == value);
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdet_Time'." << bsl::endl;
        {
            typedef bdet_Time Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;

                const Type VALUE(H, MM, S, MS); Type value;

                bdesb_MemOutStreamBuf osb;
                ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                const int LENGTH = osb.length();

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }
                int numBytesConsumed = 0;

                bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS == Util::getValue(&isb,
                                                 &value,
                                                 &numBytesConsumed));
                ASSERT(0       == isb.length());
                ASSERT(LENGTH  == numBytesConsumed);
                LOOP2_ASSERT(VALUE, value, VALUE == value);
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdet_TimeTz'." << bsl::endl;
        {
            typedef bdet_TimeTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdet_Time(H, MM, S, MS), OFF); Type value;

                bdesb_MemOutStreamBuf osb;
                ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                const int LENGTH = osb.length();

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }
                int numBytesConsumed = 0;

                bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS == Util::getValue(&isb,
                                                 &value,
                                                 &numBytesConsumed));
                ASSERT(0       == isb.length());
                ASSERT(LENGTH  == numBytesConsumed);
                LOOP2_ASSERT(VALUE, value, VALUE == value);
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdet_Datetime'." << bsl::endl;
        {
            typedef bdet_Datetime Type;

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

                bdesb_MemOutStreamBuf osb;
                ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                const int LENGTH = osb.length();

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }
                int numBytesConsumed = 0;

                bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS == Util::getValue(&isb,
                                                 &value,
                                                 &numBytesConsumed));
                ASSERT(0       == isb.length());
                ASSERT(LENGTH  == numBytesConsumed);
                LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdet_DatetimeTz'." << bsl::endl;
        {
            typedef bdet_DatetimeTz Type;

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

                const Type VALUE(bdet_Datetime(Y, M, D, H, MM, S, MS), OFF);
                Type value;

                bdesb_MemOutStreamBuf osb;
                ASSERT(0 == Util::putValue(&osb, VALUE, &options));
                const int LENGTH = osb.length();

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }
                int numBytesConsumed = 0;

                bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS == Util::getValue(&isb,
                                                 &value,
                                                 &numBytesConsumed));
                ASSERT(0       == isb.length());
                ASSERT(LENGTH  == numBytesConsumed);
                LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'putIdentifierOctets' & 'getIdentifierOctets'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'put/getIdentifierOctets'"
                               << "\n================================="
                               << bsl::endl;

        {
            enum { SUCCESS = 0, FAILURE = -1 };

            enum {
                UNIVERSAL        = bdem_BerConstants::BDEM_UNIVERSAL,
                APPLICATION      = bdem_BerConstants::BDEM_APPLICATION,
                CONTEXT_SPECIFIC = bdem_BerConstants::BDEM_CONTEXT_SPECIFIC,
                PRIVATE          = bdem_BerConstants::BDEM_PRIVATE,
                PRIMITIVE        = bdem_BerConstants::BDEM_PRIMITIVE,
                CONSTRUCTED      = bdem_BerConstants::BDEM_CONSTRUCTED
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
                const bdem_BerConstants::TagClass
                          CLASS = (bdem_BerConstants::TagClass)DATA[i].d_class;
                const bdem_BerConstants::TagType
                            TYPE  = (bdem_BerConstants::TagType)DATA[i].d_type;
                const int   NUMBER  = DATA[i].d_number;
                const char* EXP     = DATA[i].d_exp;
                const int   EXP_LEN = numOctets(EXP);

                if (veryVerbose) {
                    P_(LINE) P_(CLASS) P_(TYPE) P_(NUMBER) P(EXP)
                }

                bdesb_MemOutStreamBuf osb;
                LOOP_ASSERT(LINE, SUCCESS  == Util::putIdentifierOctets(
                                                                      &osb,
                                                                      CLASS,
                                                                      TYPE,
                                                                      NUMBER));
                LOOP_ASSERT(LINE, EXP_LEN == osb.length());
                LOOP_ASSERT(LINE, 0       == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                bdem_BerConstants::TagClass theClass;
                bdem_BerConstants::TagType  theType;
                int                         theNumber        = -1;
                int                         numBytesConsumed = 0;
                bdesb_FixedMemInStreamBuf   isb(osb.data(), osb.length());

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
            bdesb_MemOutStreamBuf osb;
            ASSERT(SUCCESS == Util::putIndefiniteLengthOctet(&osb));
            ASSERT(1       == osb.length());
            ASSERT(bdem_BerUtil_Imp::INDEFINITE_LENGTH_OCTET
                           == (unsigned char)osb.data()[0]);
        }

        if (verbose) bsl::cout << "\nTesting 'putEndOfContentOctets'."
                               << bsl::endl;
        {
            bdesb_MemOutStreamBuf osb;
            ASSERT(SUCCESS == Util::putEndOfContentOctets(&osb));
            ASSERT(2       == osb.length());
            ASSERT(0       == osb.data()[0]);
            ASSERT(0       == osb.data()[1]);
        }

        if (verbose) bsl::cout << "\nTesting 'getEndOfContentOctets'."
                               << bsl::endl;
        {
            char buffer[] = { 0 };
            bdesb_FixedMemInStreamBuf isb(buffer, 0);
            int                       numBytesConsumed = 0;

            ASSERT(FAILURE == Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 0 };
            bdesb_FixedMemInStreamBuf isb(buffer, 1);
            int                       numBytesConsumed = 0;

            ASSERT(FAILURE == Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 0, 0 };
            bdesb_FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS == Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
            ASSERT(2       == numBytesConsumed);
        }
        {
            char buffer[] = { -1, 0 };
            bdesb_FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS != Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 0, -1 };
            bdesb_FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS != Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { -1, -1 };
            bdesb_FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS != Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 1, 0 };
            bdesb_FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS != Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 0, 1 };
            bdesb_FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS != Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 1, 1 };
            bdesb_FixedMemInStreamBuf isb(buffer, 2);
            int                       numBytesConsumed = 0;

            ASSERT(SUCCESS != Util::getEndOfContentOctets(&isb,
                                                          &numBytesConsumed));
        }
        {
            char buffer[] = { 0, 0, 0 };
            bdesb_FixedMemInStreamBuf isb(buffer, 3);
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

        bdem_BerEncoderOptions options;

        if (verbose) bsl::cout << "\nTesting 'bdet_Date'." << bsl::endl;
        {
            typedef bdet_Date Type;

            const Type  VALUE(2005, 02, 23);
            const char *EXP    = "0A 32 30 30 35 2D 30 32 2D 32 33";
            const int   LENGTH = numOctets(EXP);

            bdesb_MemOutStreamBuf osb;
            ASSERT(0      == Util::putValue(&osb, VALUE, &options));
            ASSERT(LENGTH == osb.length());
            ASSERT(0      == compareBuffers(osb.data(), EXP));

            if (veryVerbose) {
                P(EXP)
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            Type value;
            int  numBytesConsumed = 0;

            bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

        if (verbose) bsl::cout << "\nTesting 'bdet_DateTz'." << bsl::endl;
        {
            typedef bdet_DateTz Type;

            const Type  VALUE(bdet_Date(2005, 02, 23), 45);
            const char *EXP    = "10 32 30 30 35 2D 30 32 2D 32 33 2B 30 30 3A"
                                 " 34 35";
            const int   LENGTH = numOctets(EXP);

            bdesb_MemOutStreamBuf osb;
            ASSERT(0      == Util::putValue(&osb, VALUE, &options));
            ASSERT(LENGTH == osb.length());
            ASSERT(0      == compareBuffers(osb.data(), EXP));

            if (veryVerbose) {
                P(EXP)
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            Type value;
            int  numBytesConsumed = 0;

            bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

        if (verbose) bsl::cout << "\nTesting 'bdet_Datetime'." << bsl::endl;
        {
            typedef bdet_Datetime Type;

            const Type  VALUE(2005, 02, 23, 3, 47, 21);
            const char *EXP    = "17 32 30 30 35 2D 30 32 2D 32 33 54 30 33 3A"
                                 " 34 37 3A 32 31 2E 30 30 30";
            const int   LENGTH = numOctets(EXP);

            bdesb_MemOutStreamBuf osb;
            ASSERT(0      == Util::putValue(&osb, VALUE, &options));
            ASSERT(LENGTH == osb.length());
            ASSERT(0      == compareBuffers(osb.data(), EXP));

            if (veryVerbose) {
                P(EXP)
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            Type value;
            int  numBytesConsumed = 0;

            bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

        if (verbose) bsl::cout << "\nTesting 'bdet_DatetimeTz'." << bsl::endl;
        {
            typedef bdet_DatetimeTz Type;

            const Type  VALUE(bdet_Datetime(2005, 02, 23, 3, 47, 21), 45);
            const char *EXP    = "1D 32 30 30 35 2D 30 32 2D 32 33 54 30 33 3A"
                                 " 34 37 3A 32 31 2E 30 30 30 2B 30 30 3A 34"
                                 " 35";
            const int   LENGTH = numOctets(EXP);

            bdesb_MemOutStreamBuf osb;
            ASSERT(0      == Util::putValue(&osb, VALUE, &options));
            ASSERT(LENGTH == osb.length());
            ASSERT(0      == compareBuffers(osb.data(), EXP));

            if (veryVerbose) {
                P(EXP)
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            Type value;
            int  numBytesConsumed = 0;

            bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

        if (verbose) bsl::cout << "\nTesting 'bdet_Time'." << bsl::endl;
        {
            typedef bdet_Time Type;

            const Type  VALUE(21, 26, 43);
            const char *EXP    = "0C 32 31 3A 32 36 3A 34 33 2E 30 30 30";
            const int   LENGTH = numOctets(EXP);

            bdesb_MemOutStreamBuf osb;
            ASSERT(0      == Util::putValue(&osb, VALUE, &options));
            ASSERT(LENGTH == osb.length());
            ASSERT(0      == compareBuffers(osb.data(), EXP));

            if (veryVerbose) {
                P(EXP)
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            Type value;
            int  numBytesConsumed = 0;

            bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

        if (verbose) bsl::cout << "\nTesting 'bdet_TimeTz'." << bsl::endl;
        {
            typedef bdet_TimeTz Type;

            const Type  VALUE(bdet_Time(21, 26, 43), 45);
            const char *EXP    = "12 32 31 3A 32 36 3A 34 33 2E 30 30 30 2B 30"
                                 " 30 3A 34 35";
            const int   LENGTH = numOctets(EXP);

            bdesb_MemOutStreamBuf osb;
            ASSERT(0      == Util::putValue(&osb, VALUE, &options));
            ASSERT(LENGTH == osb.length());
            ASSERT(0      == compareBuffers(osb.data(), EXP));

            if (veryVerbose) {
                P(EXP)
                cout << "Output Buffer:";
                printBuffer(osb.data(), osb.length());
            }

            Type value;
            int  numBytesConsumed = 0;

            bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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
        // TESTING 'putValue' & 'getValue' for 'bslstl_StringRef'
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

                bdesb_MemOutStreamBuf osb;
                LOOP_ASSERT(LINE, 0   == Util::putValue(&osb, VAL));
                LOOP_ASSERT(LINE, LEN == osb.length());
                LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    P(EXP)
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                int numBytesConsumed = 0;
                bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                bdesb_MemOutStreamBuf osb;
                LOOP_ASSERT(LINE, 0   == Util::putValue(&osb, VAL));
                LOOP_ASSERT(LINE, LEN == osb.length());
                LOOP_ASSERT(LINE, 0   == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    P(EXP)
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                int numBytesConsumed = 0;
                bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                bdesb_MemOutStreamBuf osb;
                LOOP_ASSERT(LINE, SUCCESS  == Util::putLength(&osb, LEN));
                LOOP_ASSERT(LINE, EXP_LEN  == osb.length());
                LOOP_ASSERT(LINE, 0        == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    P(EXP)
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }

                int numBytesConsumed = 0;
                bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

            typedef bdem_BerUtil_Imp Imp;

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
                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS  ==
                                               Imp::putDoubleValue(&osb, VAL));
                    LOOP_ASSERT(LINE, LEN      == osb.length());
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
                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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
                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS  == Util::putValue(&osb, F));

                    if (veryVerbose) {
                        P(VAL)
                        cout << "output buffer:";
                        printBuffer(osb.data(), osb.length());
                    }

                    int   numBytesConsumed = 0;
                    float f                = 0.0f;
                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS  == Util::getValue(
                                                           &isb, &f,
                                                           &numBytesConsumed));
                    LOOP3_ASSERT(LINE, F, f, F == f);
                    LOOP_ASSERT(LINE, osb.length() == numBytesConsumed);
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

                bdesb_MemOutStreamBuf osb;

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

                    bdesb_MemOutStreamBuf osb;

                    assembleDouble(&outVal, SIGN, EXPONENT, MANTISSA);

                    LOOP_ASSERT(LINE, RESULT == Imp::putDoubleValue(&osb,
                                                                    outVal));
                    if (SUCCESS == RESULT) {
                        LOOP_ASSERT(LINE, LEN == osb.length());
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

                        bdesb_FixedMemInStreamBuf isb(osb.data(),
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

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, LL));
                    LOOP_ASSERT(LINE, LEN     == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, VI));
                    LOOP_ASSERT(LINE, LEN     == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, L));
                    LOOP_ASSERT(LINE, LEN     == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, S));
                    LOOP_ASSERT(LINE, LEN     == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, C));
                    LOOP_ASSERT(LINE, LEN     == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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
                {  L_, 0xFFFFFF8000000000LL,  "05 80 00 00 00 00"          },
                {  L_, 0xFFFFFF8000000001LL,  "05 80 00 00 00 01"          },
                {  L_, 0xFFFFFF8123456789LL,  "05 81 23 45 67 89"          },
                {  L_, 0xFFFFFF7FFFFFFFFFLL,  "06 FF 7F FF FF FF FF"       },
                {  L_,     0x7FFFFFFFFFFFLL,  "06 7F FF FF FF FF FF"       },
                {  L_,     0x800000000000LL,  "07 00 80 00 00 00 00 00"    },
                {  L_, 0xFFFF800000000000LL,  "06 80 00 00 00 00 00"       },
                {  L_, 0xFFFF7FFFFFFFFFFFLL,  "07 FF 7F FF FF FF FF FF"    },
                {  L_,   0x7FFFFFFFFFFFFFLL,  "07 7F FF FF FF FF FF FF"    },
                {  L_,   0x80000000000000LL,  "08 00 80 00 00 00 00 00 00" },
                {  L_, 0xFF80000000000000LL,  "07 80 00 00 00 00 00 00"    },
                {  L_, 0xFF7FFFFFFFFFFFFFLL,  "08 FF 7F FF FF FF FF FF FF" },
                {  L_, 0x7FFFFFFFFFFFFFFFLL,  "08 7F FF FF FF FF FF FF FF" },
                {  L_, 0x8000000000000001LL,  "08 80 00 00 00 00 00 00 01" },
                {  L_, 0x8000000000000000LL,  "08 80 00 00 00 00 00 00 00" },
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

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, LL));
                    LOOP_ASSERT(LINE, LEN     == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, VI));
                    LOOP_ASSERT(LINE, LEN     == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, L));
                    LOOP_ASSERT(LINE, LEN == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, S));
                    LOOP_ASSERT(LINE, LEN == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, C));
                    LOOP_ASSERT(LINE, LEN     == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

            typedef bdem_BerUtil_Imp Imp;

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

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                    Imp::putIntegerGivenLength(&osb, LL, LEN));
                    LOOP_ASSERT(LINE, LEN == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        printBuffer(osb.data(), osb.length());
                    }

                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                    Imp::putIntegerGivenLength(&osb, VI, LEN));
                    LOOP_ASSERT(LINE, LEN == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                     Imp::putIntegerGivenLength(&osb, L, LEN));
                    LOOP_ASSERT(LINE, LEN == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                     Imp::putIntegerGivenLength(&osb, S, LEN));
                    LOOP_ASSERT(LINE, LEN == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                    Imp::putIntegerGivenLength(&osb, C, LEN));
                    LOOP_ASSERT(LINE, LEN == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

            typedef bdem_BerUtil_Imp Imp;

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
                {  L_, 0xFFFFFF8000000000LL,  5,  "80 00 00 00 00"          },
                {  L_, 0xFFFFFF7FFFFFFFFFLL,  6,  "FF 7F FF FF FF FF"       },
                {  L_,     0x7FFFFFFFFFFFLL,  6,  "7F FF FF FF FF FF"       },
                {  L_,     0x800000000000LL,  7,  "00 80 00 00 00 00 00"    },
                {  L_, 0xFFFF800000000000LL,  6,  "80 00 00 00 00 00"       },
                {  L_, 0xFFFF7FFFFFFFFFFFLL,  7,  "FF 7F FF FF FF FF FF"    },
                {  L_,   0x7FFFFFFFFFFFFFLL,  7,  "7F FF FF FF FF FF FF"    },
                {  L_,   0x80000000000000LL,  8,  "00 80 00 00 00 00 00 00" },
                {  L_, 0xFF80000000000000LL,  7,  "80 00 00 00 00 00 00"    },
                {  L_, 0xFF7FFFFFFFFFFFFFLL,  8,  "FF 7F FF FF FF FF FF FF" },
                {  L_, 0x7FFFFFFFFFFFFFFFLL,  8,  "7F FF FF FF FF FF FF FF" },
                {  L_, 0x8000000000000001LL,  8,  "80 00 00 00 00 00 00 01" },
                {  L_, 0x8000000000000000LL,  8,  "80 00 00 00 00 00 00 00" },
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

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                    Imp::putIntegerGivenLength(&osb, LL, LEN));
                    LOOP_ASSERT(LINE, LEN == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                    Imp::putIntegerGivenLength(&osb, VI, LEN));
                    LOOP_ASSERT(LINE, LEN == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                     Imp::putIntegerGivenLength(&osb, L, LEN));
                    LOOP_ASSERT(LINE, LEN == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                    const int      LINE = DATA[i].d_line;
                    const short    S    = (short) DATA[i].d_value;
                    const int      LEN  = DATA[i].d_length;
                    const char    *EXP  = DATA[i].d_exp;
                          short    s;

                    if (veryVerbose) { P_(i) P_(S) P_(LEN) P(EXP) }

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                     Imp::putIntegerGivenLength(&osb, S, LEN));
                    LOOP_ASSERT(LINE, LEN == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                    const int      LINE = DATA[i].d_line;
                    const char     C    = (char) DATA[i].d_value;
                    const int      LEN  = DATA[i].d_length;
                    const char    *EXP  = DATA[i].d_exp;
                          char     c;

                    if (veryVerbose) { P_(i) P_(C) P_(LEN) P(EXP) }

                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS ==
                                    Imp::putIntegerGivenLength(&osb, C, LEN));
                    LOOP_ASSERT(LINE, LEN == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                      printBuffer(osb.data(), osb.length());
                    }

                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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

                LOOP3_ASSERT(LINE, VAL, RES,
                               RES == bdem_BerUtil_Imp::numBytesToStream(VAL));
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
                // line                        value       res
                // ----                        -----       ---
                {  L_,                            0,         1 },
                {  L_,                            1,         1 },
                {  L_,                           -1,         1 },
                {  L_,                          127,         1 },
                {  L_,                          128,         2 },
                {  L_,                         -128,         1 },
                {  L_,                         -129,         2 },
                {  L_,                        32767,         2 },
                {  L_,                        32768,         3 },
                {  L_,                       -32768,         2 },
                {  L_,                       -32769,         3 },
                {  L_,                      8388607,         3 },
                {  L_,                      8388608,         4 },
                {  L_,                     -8388608,         3 },
                {  L_,                     -8388609,         4 },
                {  L_,                   2147483646,         4 },
                {  L_,                      INT_MAX,         4 },
                {  L_,                  -2147483647,         4 },
                {  L_,                      INT_MIN,         4 },
                {  L_,               0x7FFFFFFFFFLL,         5 },
                {  L_,               0x8000000000LL,         6 },
                {  L_,         0xFFFFFF8000000000LL,         5 },
                {  L_,         0xFFFFFF7FFFFFFFFFLL,         6 },
                {  L_,             0x7FFFFFFFFFFFLL,         6 },
                {  L_,             0x800000000000LL,         7 },
                {  L_,         0xFFFF800000000000LL,         6 },
                {  L_,         0xFFFF7FFFFFFFFFFFLL,         7 },
                {  L_,           0x7FFFFFFFFFFFFFLL,         7 },
                {  L_,           0x80000000000000LL,         8 },
                {  L_,         0xFF80000000000000LL,         7 },
                {  L_,         0xFF7FFFFFFFFFFFFFLL,         8 },
                {  L_,         0x7FFFFFFFFFFFFFFFLL,         8 },
                {  L_,         0x8000000000000001LL,         8 },
                {  L_,         0x8000000000000000LL,         8 },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int       LINE = DATA[i].d_line;
                const long long LL   = DATA[i].d_value;
                const int       RES  = DATA[i].d_result;

                if (veryVerbose) { P_(i) P(LL) }

                LOOP3_ASSERT(LINE, LL, RES,
                                RES == bdem_BerUtil_Imp::numBytesToStream(LL));

                if (LL <= SHRT_MAX && LL >= SHRT_MIN) {
                    const short S = (short) LL;
                    const int   I = (int)   LL;
                    const long  L = (long)  LL;

                    LOOP3_ASSERT(LINE, S, RES,
                                 RES == bdem_BerUtil_Imp::numBytesToStream(S));
                    LOOP3_ASSERT(LINE, I, RES,
                                 RES == bdem_BerUtil_Imp::numBytesToStream(I));
                    LOOP3_ASSERT(LINE, L, RES,
                                 RES == bdem_BerUtil_Imp::numBytesToStream(L));
                    continue;
                }

                if (LL <= INT_MAX && LL >= INT_MIN) {
                    const int  I = (int)  LL;
                    const long L = (long) LL;

                    LOOP3_ASSERT(LINE, I, RES,
                                 RES == bdem_BerUtil_Imp::numBytesToStream(I));
                    LOOP3_ASSERT(LINE, L, RES,
                                 RES == bdem_BerUtil_Imp::numBytesToStream(L));
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
                    bdesb_MemOutStreamBuf osb;

                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, UC));
                    LOOP_ASSERT(LINE, LEN     == osb.length());
                    LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &uc,
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
                const int            LINE = DATA[i].d_line;
                const char           C    = DATA[i].d_value;
                const signed char    SC   = (signed char) DATA[i].d_value;
                const char          *EXP  = DATA[i].d_exp;
                const int            LEN  = numOctets(EXP);
                      char           c;
                      signed char    sc;

                if (veryVerbose) { cout << "\nTesting char values" << endl; }
                {
                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, C));
                    LOOP_ASSERT(LINE, LEN     == osb.length());
                    LOOP_ASSERT(LINE, 0       ==
                                              compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &c,
                                                           &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0 == isb.length());
                    LOOP_ASSERT(LINE, c == C);
                    LOOP_ASSERT(LINE, LEN == numBytesConsumed);
                }

                if (veryVerbose) { cout << "\nTesting signed char values"
                                        << endl; }
                {
                    bdesb_MemOutStreamBuf osb;
                    LOOP_ASSERT(LINE, SUCCESS == Util::putValue(&osb, SC));
                    LOOP_ASSERT(LINE, LEN     == osb.length());
                    LOOP_ASSERT(LINE, 0       ==
                                              compareBuffers(osb.data(), EXP));

                    if (veryVerbose) {
                        P(EXP)
                        cout << "ACT: ";
                        printBuffer(osb.data(), osb.length());
                    }

                    int numBytesConsumed = 0;
                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
                    LOOP_ASSERT(LINE, SUCCESS == Util::getValue(&isb, &sc,
                                                           &numBytesConsumed));
                    LOOP_ASSERT(LINE, 0  == isb.length());
                    LOOP_ASSERT(LINE, sc == SC);
                    LOOP_ASSERT(LINE, LEN  == numBytesConsumed);
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
                bdesb_MemOutStreamBuf osb;
                ASSERT(SUCCESS  == Util::putValue(&osb, XA1));
                ASSERT(2        == osb.length());
                ASSERT(0        == compareBuffers(osb.data(), EA));

                if (veryVerbose) {
                    printBuffer(osb.data(), osb.length());
                }

                int numBytesConsumed = 0;
                bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(SUCCESS == Util::getValue(&isb, &XA2,
                                                 &numBytesConsumed));
                ASSERT(0       == isb.length());
                ASSERT(XA1     == XA2);
                ASSERT(2       == numBytesConsumed);
            }

            {
                bdesb_MemOutStreamBuf osb;
                ASSERT(SUCCESS  == Util::putValue(&osb, XB1));
                ASSERT(2        == osb.length());
                ASSERT(0        == compareBuffers(osb.data(), EB));

                if (veryVerbose) {
                    printBuffer(osb.data(), osb.length());
                }

                int numBytesConsumed = 0;
                bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
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
        //   a 'bdesb_FixedMemInput' or 'bdesb_FixedMemOutput' can bypass
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

        bsls_Stopwatch timer;

        char buffer[64];
        bdesb_FixedMemOutStreamBuf osb(buffer, 64);
        bdesb_FixedMemInStreamBuf isb(buffer, 64);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
