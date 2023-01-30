// ball_recordstringformatter.t.cpp                                   -*-C++-*-
#include <ball_recordstringformatter.h>

#include <ball_attribute.h>
#include <ball_record.h>
#include <ball_recordattributes.h>
#include <ball_severity.h>
#include <ball_userfields.h>

#include <bdlt_currenttime.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_iso8601util.h>
#include <bdlt_localtimeoffset.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_stdtestallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmt_threadutil.h>

#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_iostream.h>
#include <bsl_iomanip.h>
#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_sstream.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>                  // for 'strcmp'

#ifdef BSLS_PLATFORM_OS_UNIX
#include <unistd.h>                      // for 'getpid'
#endif

using namespace BloombergLP;

using bsl::cerr;
using bsl::cout;
using bsl::dec;
using bsl::endl;
using bsl::flush;
using bsl::hex;
using bsl::nouppercase;
using bsl::ostringstream;
using bsl::strcmp;
using bsl::string;
using bsl::uppercase;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The component under test is implemented using one 'bsl::string' object and
// one 'bdlt::DatetimeInterval' object.  The value-semantic correctness of this
// component therefore largely depends on that of those two contained class.
// We simply follow the standard 10-case test suite.  In addition, since the
// implemented class is a function object, the 'operator()' method that
// provides string-based formatting support is extensively tested.
//
// CREATORS
// [ 2] ball::RecordStringFormatter(*ba = 0);
// [10] ball::RecordStringFormatter(alloc);
// [10] ball::RecordStringFormatter(*ba);
// [10] ball::RecordStringFormatter(const char *, alloc);
// [10] ball::RecordStringFormatter(bdlt::DtI, alloc);
// [10] ball::RecordStringFormatter(bool, alloc);
// [10] ball::RecordStringFormatter(bool, *ba);
// [10] ball::RecordStringFormatter(const char *, bdlt::DtI, alloc);
// [13] ball::RecordStringFormatter(bool, alloc);
// [13] ball::RecordStringFormatter(const char *, bool, alloc);
// [ 7] ball::RecordStringFormatter(const ball::RSF&, alloc);
// [ 2] ~ball::RecordStringFormatter();
// MANIPULATORS
// [ 9] const ball::RSF& operator=(const ball::RSF& other);
// [13] void disablePublishInLocalTime();
// [13] void enablePublishInLocalTime();
// [ 2] void setFormat(const char *format);
// [ 2] void setTimestampOffset(const bdlt::DatetimeInterval& offset);
// ACCESSORS
// [ 2] const char *format() const;
// [13] bool isPublishInLocalTimeEnabled() const;
// [ 2] const bdlt::DatetimeInterval& timestampOffset() const;
// [11] void operator()(bsl::ostream&, const ball::Record&) const;
// FREE OPERATORS
// [ 6] bool operator==(const ball::RSF& lhs, const ball::RSF& rhs);
// [ 6] bool operator!=(const ball::RSF& lhs, const ball::RSF& rhs);
// [ 5] bsl::ostream& operator<<(bsl::ostream&, const ball::RSF&);
// ----------------------------------------------------------------------------
// [ 1] breathing test
// [12] USAGE example

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

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

//=============================================================================
//              GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------
static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

typedef ball::RecordStringFormatter Obj;
typedef ball::Record                Rec;
typedef bsls::Types::IntPtr         IntPtr;
typedef bsls::Types::Int64          Int64;
typedef bsls::Types::Uint64         Uint64;

// Values for testing.
const char *F0 = "\n%d %p:%t %s %f:%l %c %m %u\n";
const bdlt::DatetimeInterval T0(0);

const char *FA = "some format";
const bdlt::DatetimeInterval TA(0, 1);

const char *FB = "another format";
const bdlt::DatetimeInterval TB(0, -1);

const char *FC = "yet another format";
const bdlt::DatetimeInterval TC(1, 0);

const char *MSG_1BYTE   = "0";
const char *MSG_20BYTE  = "01234567890123456789";
const char *MSG_200BYTE =
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789";
const char *MSG_450BYTE =
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789";
const char *MSG_550BYTE =
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789"
"01234567890123456789012345678901234567890123456789";


bool compareText(const bsl::string_view& lhs, const bsl::string_view& rhs)
{
    for (unsigned int i = 0; i < lhs.length() && i < rhs.length(); ++i) {
        if (lhs[i] != rhs[i]) {
            cout << "Strings differ at index (" << i << ") "
                 << "lhs[i] = " << lhs[i] << "(" << (int)lhs[i] << ") "
                 << "rhs[i] = " << rhs[i] << "(" << (int)rhs[i] << ")"
                 << endl;
            return false;                                             // RETURN
        }
    }

    if (lhs.length() < rhs.length()) {
        bsl::size_t i = lhs.length();
        cout << "Strings differ at index (" << i << ") "
                 << "lhs[i] = END-OF-STRING "
                 << "rhs[i] = " << rhs[i] << "(" << (int)rhs[i] << ")"
                 << endl;
        return false;                                                 // RETURN

    }
    if (lhs.length() > rhs.length()) {
        bsl::size_t i = rhs.length();
        cout << "Strings differ at index (" << i << ") "
                 << "lhs[i] = " << lhs[i] << "(" << (int)lhs[i] << ") "
                 << "rhs[i] = END-OF-STRING"
                 << endl;
        return false;                                                 // RETURN
    }
    return true;

}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

}  // close unnamed namespace

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = (argc > 1) ? bsl::atoi(argv[1]) : 1;

    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);
    bslma::TestAllocatorMonitor gam(&globalAllocator);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 15: {
        // --------------------------------------------------------------------
        // TESTING: Overload resolution for 'RecordStringFormatter' changed due
        //   to 'allocator_type' (See {DRQS 165125904}).  The compiler prefers
        //   the constructor overload that takes 'bool' instead of the overload
        //   that takes 'const allocator_type&' when creating the
        //   'RecordStringFormatter' object providing a pointer to the
        //   'bslma::Allocator' object as an argument.  To resolve overload
        //   correctly we add a constructor overload that takes
        //   'bslma::Allocator*'.
        //
        // Concerns:
        //: 1 From the list of constructor overloads:
        //:   o RecordStringFormatter(const alloc& = alloc());
        //:   o RecordStringFormatter(bool, const alloc& = alloc());
        //:   o RecordStringFormatter(bslma::Allocator*);
        //:   the compiler chooses the third overload when creating the
        //:   'RecordStringFormatter' object supplying it with a pointer to the
        //:   'bslma::Allocator' object.
        //:
        //: 2 From the list of constructor overloads:
        //:   o RecordStringFormatter(const char*, alloc);
        //:   o RecordStringFormatter(const char*, bool, alloc);
        //:   o RecordStringFormatter(const char*, *ba);
        //:   the compiler chooses the third overload when creating the
        //:   'RecordStringFormatter' object supplying it with c-string and a
        //:   pointer to the 'bslma::Allocator' object.
        //
        // Plan:
        //: 1 Create objects using the different constructors and verify that
        //:   objects have expected allocator.  (C-1..2)
        //
        // Testing:
        //:   RecordStringFormatter(const alloc& = alloc());
        //:   RecordStringFormatter(bool, const alloc& = alloc());
        //:   RecordStringFormatter(bslma::Allocator*);
        //:   RecordStringFormatter(const char*, alloc);
        //:   RecordStringFormatter(const char*, bool, alloc);
        //:   RecordStringFormatter(const char*, *ba);
        // --------------------------------------------------------------------
        {   // C-1

            bslma::TestAllocator         fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator         sa("supplied",  veryVeryVeryVerbose);
            bslma::TestAllocator         da("default",   veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard guard(&da);

            for (int ti = 0; ti < 7; ++ti) {

                bslma::Allocator *objAllocatorPtr = 0;
                Obj              *objPtr = 0;

                switch (ti) {
                  // ------ RecordStringFormatter(const alloc& = alloc()) -----
                  case 0: {
                    objPtr = new (fa) Obj();
                    objAllocatorPtr = &da;
                  } break;
                  case 1: {
                    objPtr = new (fa) Obj(Obj::allocator_type(&sa));
                    objAllocatorPtr = &sa;
                  } break;
                  // --------- RecordStringFormatter(bslma::Allocator*) -------
                  case 2: {
                    objPtr = new (fa) Obj(&sa);
                    objAllocatorPtr = &sa;
                  } break;
                  case 3: {
                    objPtr = new (fa) Obj(static_cast<bslma::Allocator*>(0));
                    objAllocatorPtr = &da;
                    ASSERTV(objPtr->timestampOffset(),
                            objPtr->timestampOffset().totalMilliseconds() == 0)
                  } break;
                  // --- RecordStringFormatter(bool, const alloc& = alloc()) --
                  case 4: {
                    objPtr = new (fa) Obj(false);
                    objAllocatorPtr = &da;
                    ASSERTV(objPtr->timestampOffset(),
                            objPtr->timestampOffset().totalMilliseconds() != 0)
                  } break;
                  case 5: {
                    objPtr = new (fa) Obj(true, &sa);
                    objAllocatorPtr = &sa;
                  } break;
                  case 6: {
                    objPtr = new (fa) Obj(true, Obj::allocator_type(&sa));
                    objAllocatorPtr = &sa;
                  } break;
                }

                ASSERTV(ti, objAllocatorPtr ==
                            objPtr->get_allocator().mechanism());

                fa.deleteObject(objPtr);
            }
        }

        {   // C-2
            bslma::TestAllocator          fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator          sa("supplied",  veryVeryVeryVerbose);
            bslma::TestAllocator          da("default",   veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard  guard(&da);
            const char                   *format = "format";
            for (int ti = 0; ti < 7; ++ti) {

                bslma::Allocator *objAllocatorPtr = 0;
                Obj              *objPtr = 0;

                switch (ti) {
                  // -------- RecordStringFormatter(const char*, alloc) -------
                  case 0: {
                    objPtr = new (fa) Obj(format);
                    objAllocatorPtr = &da;
                  } break;
                  case 1: {
                    objPtr = new (fa) Obj(format, Obj::allocator_type(&sa));
                    objAllocatorPtr = &sa;
                  } break;
                  // - RecordStringFormatter(const char*, bslma::Allocator *) -
                  case 2: {
                    objPtr = new (fa) Obj(format, &sa);
                    objAllocatorPtr = &sa;
                  } break;
                  case 3: {
                      objPtr = new (fa) Obj(format,
                                            static_cast<bslma::Allocator*>(0));
                    objAllocatorPtr = &da;
                    ASSERTV(objPtr->timestampOffset(),
                            objPtr->timestampOffset().totalMilliseconds() == 0)
                  } break;
                  // ----- RecordStringFormatter(const char*, bool, alloc) ----
                  case 4: {
                    objPtr = new (fa) Obj(format, false);
                    objAllocatorPtr = &da;
                    ASSERTV(objPtr->timestampOffset(),
                            objPtr->timestampOffset().totalMilliseconds() != 0)
                  } break;
                  case 5: {
                    objPtr = new (fa) Obj(format, true, &sa);
                    objAllocatorPtr = &sa;
                  } break;
                  case 6: {
                    objPtr = new (fa) Obj(format,
                                          true,
                                          Obj::allocator_type(&sa));
                    objAllocatorPtr = &sa;
                  } break;
                }

                ASSERTV(ti, objAllocatorPtr ==
                            objPtr->get_allocator().mechanism());

                fa.deleteObject(objPtr);
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING: Records Show Calculated Local-Time Offset
        //   Per DRQS 13681097, records observe DST time transitions when
        //   'publishInLocalTime' attribute is 'true'.
        //
        // Concerns:
        //: 1 The CTORs that take a 'publishInLocalTime' parameter create
        //:   objects with the specified attribute.
        //:
        //: 2 The CTORs that do not take a 'publishInLocalTime' parameter
        //:   create objects having a 'publishInLocalTime' attribute that is
        //:   'false'.
        //:
        //: 3 The manipulators of the 'publishInLocalTime' attribute can set
        //:   the unset that attribute, and the accessor for that attribute
        //:   always returns the expected value.
        //:
        //: 4 The manipulators of the 'publishInLocalTime' attribute are
        //:   idempotent.
        //:
        //: 5 Objects having the 'publishInLocalTime' attribute actually
        //:   resolve the '%i' format specifcation in local time, irrespective
        //:   of how that attribute was set (at construction or by
        //:   manipulator).
        //:
        //: 6 Objects having the 'publishInLocalTime' attribute 'false' resolve
        //:   the '%i' format specification with their specified offset.
        //
        // Plan:
        //: 1 Create objects using the different
        //:   constructors and verify that the 'publishInLocalTime' attribute
        //:   has the expected value.  (C-1..2)
        //:
        //: 2 Default create an object and use the manipulator to
        //:   change the attribute, and then reset the attribute to the
        //:   original state.  Each use of a manipulator is done twice
        //:   to confirm idempotence. (C-3..4)
        //:
        //: 3 Create an object with a distinguished local time offset and
        //:   verify that the '%i' format specification is resolved to
        //:   the specified offset or the actual local time offset according
        //:   to the state of the 'publishInLocalTime' attribute.  (C-5..6)
        //
        // Testing:
        //   ball::RecordStringFormatter(bool, alloc);
        //   ball::RecordStringFormatter(const char *, bool, alloc);
        //   void disablePublishInLocalTime();
        //   void enablePublishInLocalTime();
        //   bool isPublishInLocalTimeEnabled() const;
        // --------------------------------------------------------------------

        if (verbose) cout
                << endl
                << "TESTING: Records Show Calculated Local-Time Offset" <<endl
                << "==================================================" <<endl;

        bslma::TestAllocator testAllocator("objectAllocator",
                                           veryVeryVeryVerbose);
        Obj::allocator_type  oa(&testAllocator);

        if (verbose) cout << "\nTest Constructors" << endl;
        {
            Obj mX0(true, oa);
            if (veryVerbose) {
                P(mX0.timestampOffset().totalMilliseconds());
            }
            ASSERT( mX0.isPublishInLocalTimeEnabled());

            Obj mX1(false, oa);
            if (veryVerbose) {
                P(mX1.timestampOffset().totalMilliseconds());
            }
                                    ASSERT(!mX1.isPublishInLocalTimeEnabled());
            Obj mX2("%i", true,  oa);
                                    ASSERT( mX2.isPublishInLocalTimeEnabled());
            Obj mX3("%i", false, oa);
                                    ASSERT(!mX3.isPublishInLocalTimeEnabled());

            Obj mX4(oa);            ASSERT(!mX4.isPublishInLocalTimeEnabled());
            Obj mX5("%i", oa);      ASSERT(!mX5.isPublishInLocalTimeEnabled());
            Obj mX6("%i", bdlt::DatetimeInterval(10), oa);
                                    ASSERT(!mX6.isPublishInLocalTimeEnabled());
        }

        if (verbose) cout << "\nTest Manipulators and Accessor" << endl;
        {
            Obj mX(oa);             ASSERT(!mX.isPublishInLocalTimeEnabled());

            mX.enablePublishInLocalTime();
                                     ASSERT( mX.isPublishInLocalTimeEnabled());
            mX.enablePublishInLocalTime();
                                     ASSERT( mX.isPublishInLocalTimeEnabled());
            mX.disablePublishInLocalTime();
                                     ASSERT(!mX.isPublishInLocalTimeEnabled());
            mX.disablePublishInLocalTime();
                                     ASSERT(!mX.isPublishInLocalTimeEnabled());
        }

        if (verbose) cout << "\nTest Fixed and Calculated Offsets" << endl;
        {
            Obj mX("%i", bdlt::DatetimeInterval(0, 3, 47), oa);
                                     ASSERT(!mX.isPublishInLocalTimeEnabled());

            bdlt::Datetime         dtUtc(2014, 2, 19);

            ball::RecordAttributes fixedFields(dtUtc,
                                               0,
                                               0,
                                               "",
                                               0,
                                               "",
                                               ball::Severity::e_OFF,
                                               "",
                                               oa.mechanism());
            ball::Record           mRecord(fixedFields,
                                           ball::UserFields(oa.mechanism()),
                                           oa.mechanism());
            const ball::Record&    record = mRecord;

            bdlt::DatetimeInterval offset(0, 3, 47);
            bdlt::Datetime dtWithOffset(dtUtc);
            dtWithOffset += offset;

            if (veryVerbose) { P_(dtUtc) P(dtWithOffset); }

            bslma::TestAllocator sa("streamAllocator", veryVeryVeryVerbose);
            ostringstream ossExpected(&sa);
            ossExpected
                 << bsl::setw(4) << bsl::setfill('0') << dtWithOffset.year()
                 << '-'
                 << bsl::setw(2) << bsl::setfill('0') << dtWithOffset.month()
                 << '-'
                 << bsl::setw(2) << bsl::setfill('0') << dtWithOffset.day()
                 << 'T'
                 << bsl::setw(2) << bsl::setfill('0') << dtWithOffset.hour()
                 << ':'
                 << bsl::setw(2) << bsl::setfill('0') << dtWithOffset.minute()
                 << ':'
                 << bsl::setw(2) << bsl::setfill('0') << dtWithOffset.second()
                 << (offset < bdlt::DatetimeInterval(0) ? '-' : '+')
                 << bsl::setw(2) << bsl::setfill('0')
                                                    << bsl::abs(offset.hours())
                 << ':'
                 << bsl::setw(2) << bsl::setfill('0')
                                                 << bsl::abs(offset.minutes());
            ostringstream ossActual(&sa);
            mX(ossActual, record);

            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);

                if (veryVerbose) {
                    T_ P_(ossExpected.str()) P(ossActual.str());
                }
                ASSERT((ossExpected.str() == ossActual.str()));
            }
            ossExpected.str(""); ossActual.str("");

            bsls::Types::Int64 localTimeOffsetInSeconds =
                  bdlt::LocalTimeOffset::localTimeOffset(dtUtc).totalSeconds();
            ASSERT(10 * 24 * 60 * 60 != localTimeOffsetInSeconds);

            if (veryVerbose) { P(localTimeOffsetInSeconds); }

            bdlt::Datetime dtWithLTO(dtUtc);
            dtWithLTO.addSeconds(localTimeOffsetInSeconds);
            offset.setTotalSeconds(localTimeOffsetInSeconds);

            if (veryVerbose) { P_(dtUtc) P(dtWithLTO); }

            ossExpected
                    << bsl::setw(4) << bsl::setfill('0') << dtWithLTO.year()
                    << '-'
                    << bsl::setw(2) << bsl::setfill('0') << dtWithLTO.month()
                    << '-'
                    << bsl::setw(2) << bsl::setfill('0') << dtWithLTO.day()
                    << 'T'
                    << bsl::setw(2) << bsl::setfill('0') << dtWithLTO.hour()
                    << ':'
                    << bsl::setw(2) << bsl::setfill('0') << dtWithLTO.minute()
                    << ':'
                    << bsl::setw(2) << bsl::setfill('0') << dtWithLTO.second();

            if (offset == bdlt::DatetimeInterval(0)) {
                ossExpected << 'Z';
            }
            else {
                ossExpected << (offset < bdlt::DatetimeInterval(0) ? '-' : '+')
                            << bsl::setw(2) << bsl::setfill('0')
                                            << bsl::abs(offset.hours())
                            << ':'
                            << bsl::setw(2) << bsl::setfill('0')
                                            << bsl::abs(offset.minutes());
            }

            mX.enablePublishInLocalTime();
            ASSERT( mX.isPublishInLocalTimeEnabled());

            mX(ossActual, record);

            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) {
                    T_ P_(ossExpected.str()) P(ossActual.str());
                }
                ASSERT((ossExpected.str() == ossActual.str()));
            }
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  Suppress
        //   all 'cout' statements in non-verbose mode, and add streaming to
        //   a buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        bslma::TestAllocator da("usageExampleAllocator",
                                veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        if (verbose) cout << endl<< "Testing Usage Example" << endl
                                 << "=====================" << endl;

///Usage
///-----
// The following snippets of code illustrate how to use an instance of
// 'ball::RecordStringFormatter' to format log records.
//
// First we instantiate a record formatter with an explicit format
// specification (but we accept the default timestamp offset since it will not
// be used in this example):
//..
    ball::RecordStringFormatter formatter("\n%t: %m\n");
//..
// The chosen format specification indicates that, when a record is formatted
// using 'formatter', the thread Id attribute of the record will be output
// followed by the message attribute of the record.
//
// Next we create a default 'ball::Record' and set the thread Id and message
// attributes of the record to dummy values:
//..
    ball::Record record;
//
    record.fixedFields().setThreadID(6);
    record.fixedFields().setMessage("Hello, World!");
//..
// The following "invocation" of the 'formatter' function object formats
// 'record' to 'bsl::cout' according to the format specification supplied at
// construction:
//..
//  formatter(bsl::cout, record);
//..
// As a result of this call, the following is printed to 'stdout':
//..
//  6: Hello, World!
//..
        ostringstream oss;
        formatter(oss, record);
        if (veryVerbose) cout << oss.str();

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING ATTRIBUTE FORMATTING
        //
        // Concerns:
        //: 1 The '%A' specifier in the format string outputs all the
        //:   attributes of the record, separated by space.
        //:
        //: 2 The '%a[key]' specifier (where 'key' is the name of the
        //:   attribute belonged to the record) outputs the attribute in the
        //:   form 'key=value', while '%av[key]' outputs the attribute's value
        //:   only.
        //:
        //: 3 The '%a' specifier output only those attributes not already
        //:   rendered by a qualified '%a'/'%av' instance.
        //:
        //: 4 The '%a' and '%a[key]'/'%av[key]' do not duplicate attribute
        //:   rendering.
        //:
        //: 5 All specifiers output attributes in the order in which they were
        //:   added.
        //:
        //: 6 If an attribute with 'key' specified in the qualified '%a[key]'
        //:   specifier is not found in the record then it is marked as "N/A"
        //:   in the output string.
        //
        // Test plan:
        //: 1 Create a log record and add to the record a set of arbitrary
        //:   attributes.
        //:
        //: 2 Using the table-driven technique:
        //:
        //:   2.1 Set a combination of '%a', '%a[key]', '%av[key] and '%A'
        //:       specifiers as a format string to the object under the test.
        //:
        //:   2.2 Invoke 'operator(bsl::ostream&, const ball::Record&)' and
        //:       make sure that the object under the test writes to the
        //:       stream the expected output.
        //
        // Testing:
        //   void operator()(bsl::ostream&, const ball::Record&) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Attribute Formatting"
                          << "\n============================" << endl;

        {
            bslma::TestAllocator testAllocator("objectAllocator",
                                               veryVeryVeryVerbose);
            Obj::allocator_type  oa(&testAllocator);

            ball::Record        mRecord(oa.mechanism());
            const ball::Record& record = mRecord;

            // Add a couple of log attributes
            mRecord.addAttribute(ball::Attribute("name",   "Name", oa));
            mRecord.addAttribute(ball::Attribute("number", 1234, oa));

            static const struct {
                int         d_line;
                const char *d_spec;
                const char *d_expected;
            } DATA[] = {
    //------------------------------------------------------------------------
    // line | spec                      | expected
    //------------------------------------------------------------------------
    { L_,    "%a",                       "name=\"Name\" number=1234"         },
    { L_,    "%A",                       "name=\"Name\" number=1234"         },
    { L_,    "%a %A",                    "name=\"Name\" number=1234 "
                                         "name=\"Name\" number=1234"         },
    { L_,    "%a %A %a[name]",           "number=1234 name=\"Name\" "
                                         "number=1234 name=\"Name\""         },
    { L_,    "%a[name]",                 "name=\"Name\""                     },
    { L_,    "%a[number]",               "number=1234"                       },
    { L_,    "%a[name] %a[number]",      "name=\"Name\" number=1234"         },
    { L_,    "%a[number] %a[name]",      "number=1234 name=\"Name\""         },
    { L_,    "%a[bad]",                  ""                                  },
    { L_,    "%A %a[name]",       "name=\"Name\" number=1234 name=\"Name\""  },
    { L_,    "%a[name] %A",       "name=\"Name\" name=\"Name\" number=1234"  },
    { L_,    "%a %a[bad]",               "name=\"Name\" number=1234 "        },
    { L_,    "%A %a[bad]",               "name=\"Name\" number=1234 "        },
    { L_,    "%a[name] %a",              "name=\"Name\" number=1234"         },
    { L_,    "%A %a[name]",       "name=\"Name\" number=1234 name=\"Name\""  },
    { L_,    "%a %a[name] %a[number]",   " name=\"Name\" number=1234"        },
    { L_,    "%a[name] %a %a[number]",   "name=\"Name\"  number=1234"        },
    { L_,    "%a[name] %a[number] %a",   "name=\"Name\" number=1234 "        },
    { L_,    "%a[name] %a[bad] %a",      "name=\"Name\"  number=1234"        },

    { L_,    "%avXX",                    "name=\"Name\" number=1234vXX"      },
    { L_,    "%av[name]",                "\"Name\""                          },
    { L_,    "%av[number]",              "1234"                              },
    { L_,    "%av[name] %av[number]",    "\"Name\" 1234"                     },
    { L_,    "%av[number] %av[name]",    "1234 \"Name\""                     },
    { L_,    "%av[bad]",                 ""                                  },
    { L_,    "%A %av[name]",             "name=\"Name\" number=1234 \"Name\""},
    { L_,    "%av[name] %A",             "\"Name\" name=\"Name\" number=1234"},
    { L_,    "%a %av[bad]",              "name=\"Name\" number=1234 "        },
    { L_,    "%A %av[bad]",              "name=\"Name\" number=1234 "        },
    { L_,    "%av[name] %a",             "\"Name\" number=1234"              },
    { L_,    "%A %a[name]",       "name=\"Name\" number=1234 name=\"Name\""  },
    { L_,    "%a %av[name] %av[number]", " \"Name\" 1234"                    },
    { L_,    "%av[name] %a %av[number]", "\"Name\"  1234"                    },
    { L_,    "%av[name] %av[number] %a", "\"Name\" 1234 "                    },
    { L_,    "%av[name] %av[bad] %a",    "\"Name\"  number=1234"             },

    { L_,    "%a[name] %av[name]",       "name=\"Name\" \"Name\""            },
    { L_,    "%av[name] %a[number]",     "\"Name\" number=1234"              },
    { L_,    "NAME=%av[name]",           "NAME=\"Name\""                     },
    { L_,    "NAME=%av[bad]",            "NAME="                             },
            };

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            bslma::TestAllocator sa("streamAllocator", veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE     = DATA[i].d_line;
                const char *SPEC     = DATA[i].d_spec;
                const char *EXPECTED = DATA[i].d_expected;

                Obj mX(oa); const Obj& X = mX;

                mX.setFormat(SPEC);

                ostringstream oss(&sa);
                X(oss, record);
                {
                    bslma::TestAllocator         da;
                    bslma::DefaultAllocatorGuard guard(&da);
                    if (veryVerbose) {
                        T_ P_(LINE); P_(SPEC); P_(oss.str()); P(EXPECTED);
                    }
                    ASSERT(oss.str() == EXPECTED);
                }
            }
        }

        {
            bslma::TestAllocator testAllocator("objectAllocator",
                                               veryVeryVeryVerbose);
            Obj::allocator_type  oa(&testAllocator);
            ball::Record         mRC(oa.mechanism());
            const ball::Record&  RC = mRC;
            int                 *value_p = (int*)42;


            // Add a couple of log attributes
            mRC.addAttribute(ball::Attribute("string",   "string", oa));
            mRC.addAttribute(ball::Attribute("int",      -42,      oa));
            mRC.addAttribute(ball::Attribute("long",     -42L,     oa));
            mRC.addAttribute(ball::Attribute("llong",    -4242LL,  oa));
            mRC.addAttribute(ball::Attribute("uint",      42U,     oa));
            mRC.addAttribute(ball::Attribute("ulong",     42UL,    oa));
            mRC.addAttribute(ball::Attribute("ullong",    4242ULL, oa));
            mRC.addAttribute(ball::Attribute("void_ptr",  value_p, oa));

            static const struct {
                int         d_line;
                const char *d_spec;
                const char *d_expected;
            } DATA[] = {
                //-------------------------------------------
                // line | spec            | expected
                //-------------------------------------------
                { L_,    "%a[string]",      "string=\"string\"" },
                { L_,    "%a[int]",         "int=-42"           },
                { L_,    "%a[long]",        "long=-42"          },
                { L_,    "%a[llong]",       "llong=-4242"       },
                { L_,    "%a[uint]",        "uint=42"           },
                { L_,    "%a[ulong]",       "ulong=42"          },
                { L_,    "%a[ullong]",      "ullong=4242"       },
                { L_,    "%a[void_ptr]",    "void_ptr=0x2a"     },
            };

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            bslma::TestAllocator sa("streamAllocator", veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE     = DATA[i].d_line;
                const char *SPEC     = DATA[i].d_spec;
                const char *EXPECTED = DATA[i].d_expected;

                Obj mX(oa); const Obj& X = mX;

                mX.setFormat(SPEC);

                ostringstream oss(&sa);
                X(oss, RC);
                {
                    bslma::TestAllocator         da;
                    bslma::DefaultAllocatorGuard guard(&da);
                    if (veryVerbose) {
                        T_ P_(LINE); P_(SPEC); P_(oss.str()); P(EXPECTED);
                    }
                    ASSERTV(LINE, SPEC, oss.str(), EXPECTED,
                            oss.str() == EXPECTED);
                }
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING OPERATOR()
        //   Operator () should print out the given 'record' in the format
        //   defined by 'd_format'.
        //
        // Test plan:
        //   Test every pattern to see if the output is the same as expected.
        //
        // Testing:
        //   void operator()(bsl::ostream&, const ball::Record&) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Initialization Constructors"
                          << "\n===================================" << endl;

        bslma::TestAllocator testAllocator("objectAllocator",
                                           veryVeryVeryVerbose);
        Obj::allocator_type  oa(&testAllocator);

        static const struct {
            int         d_lineNum;   // source line number
            int         d_hours;     // offset hours
            int         d_minutes;   // offset minutes
            const char *d_output_p;  // expected output
        } DATA[] = {
            //LINE  HOURS  MINUTES  EXPECTED
            //----  -----  -------  --------
            { L_,    -23,     -59,   "-2359" },
            { L_,    -15,     -26,   "-1526" },
            { L_,     -7,      -8,   "-0708" },
            { L_,      0,      -9,   "-0009" },
            { L_,      0,       0,   "+0000" },
            { L_,      9,       9,   "+0909" },
            { L_,     11,      40,   "+1140" },
            { L_,     23,      59,   "+2359" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int   lineNum   = 542;
        const char *filename  = "subdir/process.cpp";
        const bsls::Types::Uint64
                               threadID  = bslmt::ThreadUtil::selfIdAsUint64();
#ifdef BSLS_PLATFORM_OS_UNIX
        const pid_t processID = getpid();
#else
        const int   processID = 0;
#endif
        const char *MSG       = "Hello world!";
        ball::RecordAttributes fixedFields(bdlt::Datetime(),
                                           processID,
                                           threadID,
                                           filename,
                                           lineNum,
                                           "FOO.BAR.BAZ",
                                           ball::Severity::e_WARN,
                                           MSG,
                                           oa.mechanism());

        fixedFields.setTimestamp(bdlt::CurrentTime::utc());

        ball::UserFields userFields(oa.mechanism());
        userFields.appendString("string");
        userFields.appendDouble(3.14159265);
        userFields.appendInt64(1000000);

        ball::Record mRecord(fixedFields, userFields, oa.mechanism());

        // Add a couple of log attributes
        mRecord.addAttribute(ball::Attribute("name", "Name", oa));
        mRecord.addAttribute(ball::Attribute("number", 1234, oa));

        const ball::Record& record = mRecord;

        bdlt::Datetime timestamp = record.fixedFields().timestamp();
        Obj mX(oa);
        const Obj& X = mX;
        ASSERT(bdlt::DatetimeInterval(0) == X.timestampOffset());

        bslma::TestAllocator sa("streamAllocator", veryVeryVeryVerbose);
        bsl::ostringstream oss1(&sa), oss2(&sa);

        if (verbose) cout << "\n  Testing default format." << endl;
        {
            oss1.str("");
            X(oss1, record);
            if (veryVerbose) {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                P(oss1.str());
            }
        }

        if (verbose) cout << "\n  Testing \"%%\"." << endl;
        {
            oss1.str("");
            mX.setFormat("%%");
            X(oss1, record);
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P(oss1.str()); }
                ASSERT(oss1.str() == "%");
            }
        }

        if (verbose) cout << "\n  Testing \"%d\"." << endl;
        {
            oss1.str("");
            mX.setFormat("%d");
            X(oss1, record);

            const bdlt::Datetime& timestamp = record.fixedFields().timestamp();
            const int SIZE = 32;
            char buffer[SIZE];
            timestamp.printToBuffer(buffer, SIZE, 3);

            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                bsl::string                  EXP(buffer);
                if (veryVerbose) { P_(oss1.str());  P(EXP) }
                ASSERT(oss1.str() == EXP);
            }
        }

        if (verbose) cout << "\n  Testing \"%D\"." << endl;
        {
            oss1.str("");
            mX.setFormat("%D");
            X(oss1, record);

            const bdlt::Datetime& timestamp = record.fixedFields().timestamp();
            const int SIZE = 32;
            char buffer[SIZE];
            timestamp.printToBuffer(buffer, SIZE, 6);

            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                bsl::string                  EXP(buffer);
                if (veryVerbose) { P_(oss1.str());  P(EXP) }
                ASSERT(oss1.str() == EXP);
            }
        }

        if (verbose) cout << "\n  Testing \"%dtz\"." << endl;
        {
            mX.setFormat("%dtz");

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE       = DATA[i].d_lineNum;
                const int   HOURS      = DATA[i].d_hours;
                const int   MINUTES    = DATA[i].d_minutes;
                const char *EXP_OFFSET = DATA[i].d_output_p;

                if (veryVerbose) {
                    T_ P(LINE) P_(HOURS) P_(MINUTES) P(EXP_OFFSET);
                }

                oss1.str("");
                bdlt::DatetimeInterval offset(0, HOURS, MINUTES);
                mX.setTimestampOffset(offset);
                X(oss1, record);

                const bdlt::Datetime& timestamp =
                                     record.fixedFields().timestamp() + offset;
                const int             SIZE = 64;
                char                  buffer[SIZE];
                timestamp.printToBuffer(buffer, SIZE, 3);

                {
                    bslma::TestAllocator         da;
                    bslma::DefaultAllocatorGuard guard(&da);
                    bsl::string                  EXP(buffer);
                    EXP += EXP_OFFSET;
                    if (veryVerbose) { P_(oss1.str());  P(EXP) }
                    ASSERT(oss1.str() == EXP);
                }

                mX.setTimestampOffset(bdlt::DatetimeInterval());
            }
        }

#ifndef BSLS_ASSERT_SAFE_IS_ACTIVE
        // Although an offset greater than 24 hours is undefined behavior, such
        // invalid 'DatetimeTz' objects still can be created under certain
        // circumstances.  We want to enable clients to detect these errors as
        // quickly as possible (DRQS 12693813).

        if (verbose) cout << "\n  Testing special scenario of \"%dtz\"."
                          << endl;
        {
            oss1.str("");
            mX.setFormat("%dtz");

            bdlt::DatetimeInterval offset(0, -100, 0);
            mX.setTimestampOffset(offset);
            X(oss1, record);

            const bdlt::Datetime& timestamp =
                                     record.fixedFields().timestamp() + offset;
            const int             SIZE = 64;
            char                  buffer[SIZE];
            timestamp.printToBuffer(buffer, SIZE, 3);

            {
                bslma::TestAllocator          da;
                bslma::DefaultAllocatorGuard  guard(&da);
                const char                   *EXP_OFFSET = "-XX00";
                bsl::string                   EXP(buffer);
                EXP += EXP_OFFSET;
                if (veryVerbose) { P_(oss1.str());  P(EXP) }
                ASSERTV(EXP, oss1.str(), EXP == oss1.str());
            }

            mX.setTimestampOffset(bdlt::DatetimeInterval());
        }
#endif

        if (verbose) cout << "\n  Testing \"%Dtz\"." << endl;
        {
            mX.setFormat("%Dtz");

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE       = DATA[i].d_lineNum;
                const int   HOURS      = DATA[i].d_hours;
                const int   MINUTES    = DATA[i].d_minutes;
                const char *EXP_OFFSET = DATA[i].d_output_p;

                if (veryVerbose) {
                    T_ P(LINE) P_(HOURS) P_(MINUTES) P(EXP_OFFSET);
                }

                oss1.str("");
                bdlt::DatetimeInterval offset(0, HOURS, MINUTES);
                mX.setTimestampOffset(offset);
                X(oss1, record);

                const bdlt::Datetime& timestamp =
                                     record.fixedFields().timestamp() + offset;
                const int             SIZE = 64;
                char                  buffer[SIZE];
                timestamp.printToBuffer(buffer, SIZE, 6);

                {
                    bslma::TestAllocator         da;
                    bslma::DefaultAllocatorGuard guard(&da);
                    bsl::string                  EXP(buffer);
                    EXP += EXP_OFFSET;
                    if (veryVerbose) { P_(oss1.str());  P(EXP) }
                    ASSERT(oss1.str() == EXP);
                }

                mX.setTimestampOffset(bdlt::DatetimeInterval());
            }
        }

#ifndef BSLS_ASSERT_SAFE_IS_ACTIVE
        // Although an offset greater than 24 hours is undefined behavior, such
        // invalid 'DatetimeTz' objects still can be created under certain
        // circumstances.  We want to enable clients to detect these errors as
        // quickly as possible (DRQS 12693813).

        if (verbose) cout << "\n  Testing special scenario of \"%Dtz\"."
                          << endl;
        {
            oss1.str("");
            mX.setFormat("%Dtz");

            bdlt::DatetimeInterval offset(0, 100, 0);
            mX.setTimestampOffset(offset);
            X(oss1, record);

            const bdlt::Datetime& timestamp =
                                     record.fixedFields().timestamp() + offset;
            const int             SIZE = 64;
            char                  buffer[SIZE];
            timestamp.printToBuffer(buffer, SIZE, 6);

            {
                bslma::TestAllocator          da;
                bslma::DefaultAllocatorGuard  guard(&da);
                const char                   *EXP_OFFSET = "+XX00";
                bsl::string                   EXP(buffer);
                EXP += EXP_OFFSET;
                if (veryVerbose) { P_(oss1.str());  P(EXP) }
                ASSERTV(EXP, oss1.str(), EXP == oss1.str());
            }

            mX.setTimestampOffset(bdlt::DatetimeInterval());
        }
#endif

        if (verbose) cout << "\n  Testing \"%i\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%i");
            X(oss1, record);
            oss2 << bsl::setw(4) << bsl::setfill('0') << timestamp.year()
                << '-'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.month()
                << '-'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.day()
                << 'T'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.hour()
                << ':'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.minute()
                << ':'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.second()
                << 'Z';

            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
                ASSERT(oss1.str() == oss2.str());

                // Is the resulting string parseable?
                bdlt::Datetime dt;
                int len = static_cast<int>(bsl::strlen(oss1.str().c_str()));
                int rc = bdlt::Iso8601Util::parse(&dt,
                                                  oss1.str().c_str(),
                                                  len);
                bdlt::Datetime adjustedTimestamp(timestamp);
                adjustedTimestamp.setMillisecond(0); //"%i" => no msecs printed
                adjustedTimestamp.setMicrosecond(0); //"%i" => no usecs printed

                if (veryVerbose) { P_(rc) P_(adjustedTimestamp) P(dt) }
                ASSERT(0                 == rc);
                ASSERT(adjustedTimestamp == dt);
            }
        }

        if (verbose) cout << "\n  Testing \"%I\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%I");
            X(oss1, record);
            oss2 << bsl::setw(4) << bsl::setfill('0') << timestamp.year()
                << '-'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.month()
                << '-'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.day()
                << 'T'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.hour()
                << ':'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.minute()
                << ':'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.second()
                << '.'
                << bsl::setw(3) << bsl::setfill('0') << timestamp.millisecond()
                << 'Z';
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
                ASSERT(oss1.str() == oss2.str());

                // Is the resulting string parseable?
                bdlt::Datetime dt;
                int len = static_cast<int>(bsl::strlen(oss1.str().c_str()));
                int rc = bdlt::Iso8601Util::parse(&dt,
                                                  oss1.str().c_str(),
                                                  len);

                bdlt::Datetime adjustedTimestamp(timestamp);
                adjustedTimestamp.setMicrosecond(0); //"%I" => no usecs printed

                if (veryVerbose) { P_(rc) P_(adjustedTimestamp) P(dt) }
                ASSERT(0                 == rc);
                ASSERT(adjustedTimestamp == dt);
            }
        }

        if (verbose) cout << "\n  Testing \"%O\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%O");
            X(oss1, record);
            oss2 << bsl::setw(4) << bsl::setfill('0') << timestamp.year()
                << '-'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.month()
                << '-'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.day()
                << 'T'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.hour()
                << ':'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.minute()
                << ':'
                << bsl::setw(2) << bsl::setfill('0') << timestamp.second()
                << '.'
                << bsl::setw(3) << bsl::setfill('0') << timestamp.millisecond()
                << bsl::setw(3) << bsl::setfill('0') << timestamp.microsecond()
                << 'Z';
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
                ASSERT(oss1.str() == oss2.str());

                // Is the resulting string parseable?
                bdlt::Datetime dt;
                int len = static_cast<int>(bsl::strlen(oss1.str().c_str()));
                int rc = bdlt::Iso8601Util::parse(&dt,
                                                  oss1.str().c_str(),
                                                  len);

                if (veryVerbose) { P_(rc) P_(timestamp) P(dt) }
                ASSERT(0         == rc);
                ASSERT(timestamp == dt);
            }
        }

        if (verbose) cout << "\n  Testing \"%i\" with local timestamp."
                          << endl;
        {
            mX.setFormat("%i");

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE       = DATA[i].d_lineNum;
                const int   HOURS      = DATA[i].d_hours;
                const int   MINUTES    = DATA[i].d_minutes;

                if (veryVerbose) {
                    T_ P(LINE) P_(HOURS) P(MINUTES);
                }

                oss1.str("");
                oss2.str("");
                bdlt::DatetimeInterval offset(0, HOURS, MINUTES);
                mX.setTimestampOffset(offset);
                bdlt::Datetime localTime = timestamp + offset;
                X(oss1, record);

                oss2 << bsl::setw(4) << bsl::setfill('0') << localTime.year()
                    << '-'
                    << bsl::setw(2) << bsl::setfill('0') << localTime.month()
                    << '-'
                    << bsl::setw(2) << bsl::setfill('0') << localTime.day()
                    << 'T'
                    << bsl::setw(2) << bsl::setfill('0') << localTime.hour()
                    << ':'
                    << bsl::setw(2) << bsl::setfill('0') << localTime.minute()
                    << ':'
                    << bsl::setw(2) << bsl::setfill('0') << localTime.second();

                if (HOURS || MINUTES) {
                    oss2 << (offset < bdlt::DatetimeInterval(0) ? '-' : '+')
                         << bsl::setw(2) << bsl::setfill('0')
                                                    << bsl::abs(offset.hours())
                         << ':'
                         << bsl::setw(2) << bsl::setfill('0')
                                                 << bsl::abs(offset.minutes());
                }
                else {
                    oss2 << 'Z';
                }

                {
                    bslma::TestAllocator         da;
                    bslma::DefaultAllocatorGuard guard(&da);
                    if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
                    ASSERT(oss1.str() == oss2.str());

                    // Is the resulting string parseable?
                    bdlt::DatetimeTz dt;

                    int len =
                             static_cast<int>(bsl::strlen(oss1.str().c_str()));
                    int rc  = bdlt::Iso8601Util::parse(&dt,
                                                       oss1.str().c_str(),
                                                       len);
                    bdlt::Datetime truncatedLocalTime = localTime;
                    truncatedLocalTime.setMillisecond(0);
                        // "%i" => no msecs printed
                    truncatedLocalTime.setMicrosecond(0);
                        // "%i" => no usecs printed

                    bdlt::DatetimeTz adjustedTimestamp(
                                      truncatedLocalTime,
                                      static_cast<int>(offset.totalMinutes()));

                    if (veryVerbose) { P_(rc) P_(adjustedTimestamp) P(dt) }
                    ASSERT(0                 == rc);
                    ASSERT(adjustedTimestamp == dt);
                }

                mX.setTimestampOffset(bdlt::DatetimeInterval());
            }
        }

        if (verbose) cout << "\n  Testing \"%p\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%p");
            X(oss1, record);
            oss2 << processID;
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
                ASSERT(oss1.str() == oss2.str());
            }
        }

        if (verbose) cout << "\n  Testing \"%t\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%t");
            X(oss1, record);
            oss2 << threadID;
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
                ASSERT(oss1.str() == oss2.str());
            }
        }

        if (verbose) cout << "\n  Testing \"%T\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%T");
            X(oss1, record);
            oss2 << uppercase << hex << threadID << nouppercase << dec;
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
                ASSERT(oss1.str() == oss2.str());
            }
        }

        if (verbose) cout << "\n  Testing \"%s\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%s");
            X(oss1, record);
            oss2 << ball::Severity::toAscii(
                       (ball::Severity::Level)record.fixedFields().severity());
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
                ASSERT(oss1.str() == oss2.str());
            }
        }

        if (verbose) cout << "\n  Testing \"%f\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%f");
            X(oss1, record);
            oss2 << filename;
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
                ASSERT(oss1.str() == oss2.str());
            }
        }

        if (verbose) cout << "\n  Testing \"%F\"." << endl;
        {
#ifdef BSLS_PLATFORM_OS_WINDOWS
#define SLASH "\\"
#else
#define SLASH "/"
#endif
            static const struct {
                int         d_lineNum;     // line number of test vector
                const char *d_recordFile;  // '__FILE__' in record
                const char *d_outputFile;  // part of '__FILE__' output
            } DATA[] = {
                // line   record filename             output filename
                // ----   ---------------             ---------------
                {  L_,    "foo.c",                    "foo.c"            },
                {  L_,    SLASH "foo.c",              "foo.c"            },
                {  L_,    "foo" SLASH "bar.c",        "bar.c"            },
                {  L_,    SLASH "foo" SLASH "bar.c",  "bar.c"            },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            mX.setFormat("%F");

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE       = DATA[ti].d_lineNum;
                const char *RECORDFILE = DATA[ti].d_recordFile;
                const char *OUTPUTFILE = DATA[ti].d_outputFile;

                Rec mR(oa.mechanism());  const Rec &R = mR;
                mR.fixedFields().setFileName(RECORDFILE);

                ostringstream oss(&sa);
                X(oss, R);

                {
                    bslma::TestAllocator         da;
                    bslma::DefaultAllocatorGuard guard(&da);
                    if (veryVerbose) { P_(RECORDFILE);  P(OUTPUTFILE) }
                    ASSERTV(LINE, 0 == bsl::strcmp(OUTPUTFILE,
                                                   oss.str().c_str()));
                }
            }
        }

        if (verbose) cout << "\n  Testing \"%l\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%l");
            X(oss1, record);
            oss2 << lineNum;
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
                ASSERT(oss1.str() == oss2.str());
            }
        }

        if (verbose) cout << "\n  Testing \"%c\"." << endl;
        {
            oss1.str("");
            mX.setFormat("%c");
            X(oss1, record);
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P(oss1.str()); }
                ASSERT(oss1.str() == "FOO.BAR.BAZ");
            }
        }

        if (verbose) cout << "\n  Testing \"%m\"." << endl;
        {
            oss1.str("");
            mX.setFormat("%m");
            X(oss1, record);
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P(oss1.str()); }
                ASSERT(oss1.str() == MSG);
            }
        }

        {
            // Testing regression reported in {DRQS 165063339}.
            // It is not possible to set a message to a string with embedded 0
            // with record's modifiers, so we need to use stream/streambuffer.
            oss1.str("");
            mX.setFormat("%m");

            mRecord.fixedFields().clearMessage();

            bsl::ostream inStream(&mRecord.fixedFields().messageStreamBuf());
            inStream << "header " << '\0' << " footer";

            X(oss1, record);
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);

                bsl::ostringstream outStream(&da);
                outStream << "header " << '\0' << " footer";

                if (veryVerbose) { P_(oss1.str()); P_(outStream.str());}
                ASSERTV(oss1.str(), outStream.str(),
                        oss1.str() == outStream.str());
            }

            // Reset the message field to the original string.
            mRecord.fixedFields().setMessage(MSG);
        }

        if (verbose) cout << "\n  Testing \"%x\" and \"%X\"." << endl;
        {
            static const struct {
                int         d_lineNum;
                const char *d_spec;
                const char *d_expectedResult_x;
                const char *d_expectedResult_X;
            } DATA[] = {

                //line   spec             expectedResult_x     expectedResult_X
                //----   ----             ----------------     ----------------

                // Some printable characters.
                { L_,    "a",             "a",                  "61"         },
                { L_,    "A",             "A",                  "41"         },
                { L_,    "aB",            "aB",                 "6142"       },
                { L_,    "aBc",           "aBc",                "614263"     },

                // Hex conversion
                { L_,    "\x01",          "\\x01",              "01"         },
                { L_,    "\x02",          "\\x02",              "02"         },
                { L_,    "\n",            "\\x0A",              "0A"         },
                { L_,    "\\\r",          "\\\\x0D",            "5C0D"       },
                { L_,    "\x7F",          "\\x7F",              "7F"         },
                { L_,    "\x80",          "\\x80",              "80"         },
                { L_,    "\x81",          "\\x81",              "81"         },
                { L_,    "\xFE\\",        "\\xFE\\",            "FE5C"       },
                { L_,    "\\\xFF",        "\\\\xFF",            "5CFF"       },

                // Interleaved printable, non-printable and slash.
                { L_,    "a\rb\nc\\",     "a\\x0Db\\x0Ac\\",  "610D620A635C" },
                { L_,    "\\aA\rbB\ncC",  "\\aA\\x0DbB\\x0AcC",
                                                        "5C61410D62420A6343" },
                { L_,    " \r\n",         " \\x0D\\x0A",      "200D0A"       },
                { L_,    "\r\nx",         "\\x0D\\x0Ax",      "0D0A78"       },
                { L_,    "\r\nz\\y",      "\\x0D\\x0Az\\y",   "0D0A7A5C79"   },
                { L_,    "a\r\nz\\y",     "a\\x0D\\x0Az\\y",  "610D0A7A5C79" },
                { L_,    "000000000000000000000000000000000000000000000000",
                         "000000000000000000000000000000000000000000000000",
                         "303030303030303030303030303030303030303030303030"
                         "303030303030303030303030303030303030303030303030",
                }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE               = DATA[i].d_lineNum;
                const char *SPEC               = DATA[i].d_spec;
                const char *EXPECTED_RESULT_x  = DATA[i].d_expectedResult_x;
                const char *EXPECTED_RESULT_X  = DATA[i].d_expectedResult_X;

                mRecord.fixedFields().setMessage(SPEC);

                oss1.str("");
                mX.setFormat("%x");
                X(oss1, record);
                {
                    bslma::TestAllocator         da;
                    bslma::DefaultAllocatorGuard guard(&da);
                    if (veryVerbose) {
                        P_(LINE); P_(SPEC); P_(oss1.str());
                        P(EXPECTED_RESULT_x);
                    }
                    ASSERT(oss1.str() == EXPECTED_RESULT_x);
                }

                oss2.str("");
                mX.setFormat("%X");
                X(oss2, record);
                {
                    bslma::TestAllocator         da;
                    bslma::DefaultAllocatorGuard guard(&da);
                    if (veryVerbose) {
                        P_(LINE); P_(SPEC); P_(oss2.str());
                        P(EXPECTED_RESULT_X);
                    }
                    ASSERT(oss2.str() == EXPECTED_RESULT_X);
                }
            }

            // Reset the message field to the original string.
            mRecord.fixedFields().setMessage(MSG);
        }

        if (verbose) cout << "\n  Testing \"%u\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%u");
            X(oss1, record);
            oss2 << "string " << 3.14159265l << " " << 1000000;
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
                ASSERTV(oss1.str(), oss2.str(),
                        compareText(oss1.str(),oss2.str()));
            }
        }

        if (verbose) cout << "\n  Testing \"%a\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%a");
            X(oss1, record);
            oss2 << "name=\"Name\" number=1234";
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
                ASSERTV(oss1.str(), oss2.str(),
                        compareText(oss1.str(),oss2.str()));
            }
        }

        if (verbose) cout << "\n  Testing \"%av\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%av");
            X(oss1, record);
            oss2 << "name=\"Name\" number=1234v";
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
                ASSERTV(oss1.str(), oss2.str(),
                        compareText(oss1.str(),oss2.str()));
            }
        }

        if (verbose) cout << "\n  Testing \"%a[key]\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%a[name]");
            X(oss1, record);
            oss2 << "name=\"Name\"";
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
                ASSERTV(oss1.str(), oss2.str(),
                        compareText(oss1.str(),oss2.str()));
            }
        }

        if (verbose) cout << "\n  Testing \"%A\"." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%A %a[name]");
            X(oss1, record);
            oss2 << "name=\"Name\" number=1234 name=\"Name\"";
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
                ASSERTV(oss1.str(), oss2.str(),
                        compareText(oss1.str(),oss2.str()));
            }
        }

        if (verbose) cout << "\n  Testing \"\\n\"." << endl;
        {
            oss1.str("");
            mX.setFormat("\\n");  X(oss1, record);
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P(oss1.str()); }
                ASSERT(oss1.str() == "\n");
            }
        }

        if (verbose) cout << "\n  Testing \"\\t\"." << endl;
        {
            oss1.str("");
            mX.setFormat("\\t");
            X(oss1, record);
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P(oss1.str()); }
                ASSERT(oss1.str() == "\t");
            }
        }

        if (verbose) cout << "\n  Testing \"\\\\\"." << endl;
        {
            oss1.str("");
            mX.setFormat("\\\\");
            X(oss1, record);
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P(oss1.str()); }
                ASSERT(oss1.str() == "\\");
            }
        }

        if (verbose) cout << "\n  Testing an arbitrary string." << endl;
        {
            oss1.str("");
            mX.setFormat("log this");
            X(oss1, record);
            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                if (veryVerbose) { P(oss1.str()); }
                ASSERT(oss1.str() == "log this");
            }
        }

        if (verbose) cout << "\n Testing a composite string." << endl;
        {
            oss1.str("");
            oss2.str("");
            mX.setFormat("%dtz %l %f log this");
            X(oss1, record);

            const bdlt::Datetime& timestamp = record.fixedFields().timestamp();
            const int             SIZE = 64;
            char                  buffer[SIZE];
            timestamp.printToBuffer(buffer, SIZE, 3);

            {
                bslma::TestAllocator         da;
                bslma::DefaultAllocatorGuard guard(&da);
                bsl::string                  expectedTimestamp(buffer);
                expectedTimestamp += "+0000";
                oss2 << expectedTimestamp << " "
                     << lineNum << " "
                     << filename << " "
                     << "log this";
                if (veryVerbose) { P_(oss1.str());  P(oss2.str()) }
                ASSERTV(oss2.str(), oss1.str(), oss2.str() == oss1.str());
            }
        }

        if (verbose) cout << "\n Testing allocation behavior." << endl;
        {
            const char *TEST_MESSAGES[] = {
                MSG_1BYTE,
                MSG_20BYTE,
                MSG_200BYTE,
                MSG_450BYTE,
                MSG_550BYTE,
            };
            const int NUM_TEST_MESSAGES = sizeof(TEST_MESSAGES) /
                                          sizeof(*TEST_MESSAGES);

            for (int i = 0; i < NUM_TEST_MESSAGES; ++i) {
                const char   *MSG     = TEST_MESSAGES[i];
                const IntPtr  MSG_LEN = bsl::strlen(MSG);

                bslma::TestAllocator objectAllocator("oa",
                                                     veryVeryVeryVerbose);
                Obj::allocator_type  oa(&objectAllocator);
                Obj x("%m", oa); const Obj& X = x;

                ball::RecordAttributes fixedFields(bdlt::Datetime(),
                                                   processID,
                                                   threadID,
                                                   filename,
                                                   lineNum,
                                                   "FOO.BAR.BAZ",
                                                   ball::Severity::e_WARN,
                                                   MSG,
                                                   oa.mechanism());
                fixedFields.setTimestamp(bdlt::CurrentTime::utc());

                ball::UserFields userFields(oa.mechanism());
                ball::Record     record(fixedFields,
                                        userFields,
                                        oa.mechanism());

                bsl::ostringstream           stream(&sa);
                bslma::TestAllocator         da("da",
                                                veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard guard(&da);
                bslma::TestAllocatorMonitor  oam(&objectAllocator), dam(&da);

                X(stream, record);

                const bool expectIncrease = MSG_LEN > 500;

                ASSERT(oam.isInUseSame());
                ASSERT(oam.isMaxSame());
                ASSERT(dam.isInUseSame());
                ASSERT(expectIncrease == dam.isMaxUp());

                if (veryVeryVerbose) {
                    P_(oam.isInUseSame());
                    P_(oam.isMaxSame());
                    P_(dam.isInUseSame());
                    P(dam.isMaxUp());
                    P(stream.str());
                }
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZATION CONSTRUCTORS:
        //   The three constructors must initialize the value correctly.
        //
        // Testing
        //   ball::RecordStringFormatter(const char *, alloc);
        //   ball::RecordStringFormatter(bdlt::DtI, alloc);
        //   ball::RecordStringFormatter(const char *, bdlt::DtI, alloc);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Initialization Constructors"
                          << "\n===================================" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_format;
        } FVALUES[] = {
            // line   format
            // ----   ------
            {  L_,    "some format"                             },
            {  L_,    "other format"                            },
            {  L_,    "yet another format"                      },
            {  L_,    "\n%d %p:%t %s %f:%l %c %m %u\n"          },
            {  L_,    "\n%d %p:%t %s %f:%l %c %x %u\n"          },
            {  L_,    "\n%d %p:%t %s %f:%l %c %X %u\n"          },
            {  L_,    ""                                        },
        };

        static const struct {
            int d_lineNum;
            int d_days;
            int d_hours;
            int d_mins;
            int d_secs;
            int d_msecs;
        } TVALUES[] = {
            ///line  days hours mins secs msecs
            ///----  ---- ----- ---- ---- -----
            { L_,     1,   1,   1,   1,    1 },
            { L_,     1,   1,   1,   1,    2 },
            { L_,     1,   1,   1,   2,    1 },
            { L_,     1,   1,   2,   1,    1 },
            { L_,     1,   2,   1,   1,    1 },
            { L_,     2,   1,   1,   1,    1 },
            { L_, -9999, -23, -59, -59, -999 },
            { L_, -9999, -23, -59, -59, -998 },
            { L_, -9999, -23, -59, -58, -999 },
            { L_, -9999, -23, -58, -59, -999 },
            { L_, -9999, -22, -59, -59, -999 },
            { L_, -9998, -23, -59, -59, -999 },
        };

        const int NUM_FVALUES = sizeof FVALUES / sizeof *FVALUES;
        const int NUM_TVALUES = sizeof TVALUES / sizeof *TVALUES;

        bslma::TestAllocator testAllocator("objectAllocator",
                                           veryVeryVeryVerbose);
        Obj::allocator_type  oa(&testAllocator);

        for (int i = 0; i < NUM_FVALUES; ++i) {
            Obj x(oa);  const Obj& X = x;
            x.setFormat(FVALUES[i].d_format);

            Obj y(FVALUES[i].d_format, oa); const Obj& Y = y;
            ASSERTV(FVALUES[i].d_lineNum, Y == X);
        }

        for (int i = 0; i < NUM_TVALUES; ++i) {
            Obj x(oa);  const Obj& X = x;
            x.setTimestampOffset(
                  bdlt::DatetimeInterval(TVALUES[i].d_days,
                                         TVALUES[i].d_hours,
                                         TVALUES[i].d_mins,
                                         TVALUES[i].d_secs,
                                         TVALUES[i].d_msecs));

            Obj y(bdlt::DatetimeInterval(TVALUES[i].d_days,
                                         TVALUES[i].d_hours,
                                         TVALUES[i].d_mins,
                                         TVALUES[i].d_secs,
                                         TVALUES[i].d_msecs),
                  oa);
            const Obj& Y = y;
            ASSERTV(TVALUES[i].d_lineNum, Y == X);
        }

        for (int i = 0; i < NUM_FVALUES; ++i) {
            for (int j = 0; j < NUM_TVALUES; ++j) {

                Obj x(oa);  const Obj& X = x;
                x.setFormat(FVALUES[i].d_format);
                x.setTimestampOffset(
                      bdlt::DatetimeInterval(TVALUES[j].d_days,
                                             TVALUES[j].d_hours,
                                             TVALUES[j].d_mins,
                                             TVALUES[j].d_secs,
                                             TVALUES[j].d_msecs));

                Obj y(FVALUES[i].d_format,
                      bdlt::DatetimeInterval(TVALUES[j].d_days,
                                             TVALUES[j].d_hours,
                                             TVALUES[j].d_mins,
                                             TVALUES[j].d_secs,
                                             TVALUES[j].d_msecs),
                      oa);
                const Obj& Y = y;
                ASSERTV(FVALUES[i].d_lineNum, TVALUES[j].d_lineNum, X == Y);
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S x S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   const ball::RSF& operator=(const ball::RSF& other);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Assignment Operator"
                          << "\n===========================" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_format;
        } FVALUES[] = {
            // line   format
            // ----   ------
            {  L_,    "some format"                             },
            {  L_,    "other format"                            },
            {  L_,    "yet another format"                      },
            {  L_,    "\n%d %p:%t %s %f:%l %c %m %u\n"          },
            {  L_,    ""                                        },
        };

        static const struct {
            int d_lineNum;
            int d_days;
            int d_hours;
            int d_mins;
            int d_secs;
            int d_msecs;
        } TVALUES[] = {
            ///line  days hours mins secs msecs
            ///----  ---- ----- ---- ---- -----
            { L_,     1,   1,   1,   1,    1 },
            { L_,     1,   1,   1,   1,    2 },
            { L_,     1,   1,   1,   2,    1 },
            { L_,     1,   1,   2,   1,    1 },
            { L_,     1,   2,   1,   1,    1 },
            { L_,     2,   1,   1,   1,    1 },
            { L_, -9999, -23, -59, -59, -999 },
            { L_, -9999, -23, -59, -59, -998 },
            { L_, -9999, -23, -59, -58, -999 },
            { L_, -9999, -23, -58, -59, -999 },
            { L_, -9999, -22, -59, -59, -999 },
            { L_, -9998, -23, -59, -59, -999 },
        };

        const int NUM_FVALUES = sizeof FVALUES / sizeof *FVALUES;
        const int NUM_TVALUES = sizeof TVALUES / sizeof *TVALUES;

        bslma::TestAllocator testAllocator("objectAllocator",
                                           veryVeryVeryVerbose);
        Obj::allocator_type  oa(&testAllocator);

        for (int i1 = 0; i1 < NUM_FVALUES; ++i1) {
            for (int i2 = 0; i2 < NUM_TVALUES; ++i2) {
                Obj v(oa);  const Obj& V = v;
                v.setFormat(FVALUES[i1].d_format);
                v.setTimestampOffset(
                      bdlt::DatetimeInterval(TVALUES[i2].d_days,
                                            TVALUES[i2].d_hours,
                                            TVALUES[i2].d_mins,
                                            TVALUES[i2].d_secs,
                                            TVALUES[i2].d_msecs));

                for (int j1 = 0; j1 < NUM_FVALUES; ++j1) {
                    for (int j2 = 0; j2 < NUM_TVALUES; ++j2) {
                        Obj u(oa);  const Obj& U = u;
                        u.setFormat(FVALUES[j1].d_format);
                        u.setTimestampOffset(
                               bdlt::DatetimeInterval(TVALUES[j2].d_days,
                                                     TVALUES[j2].d_hours,
                                                     TVALUES[j2].d_mins,
                                                     TVALUES[j2].d_secs,
                                                     TVALUES[j2].d_msecs));

                        Obj w(V, oa);  const Obj &W = w;          // control
                        u = V;
                        ASSERTV(FVALUES[i1].d_lineNum,
                                TVALUES[i2].d_lineNum,
                                FVALUES[j1].d_lineNum,
                                TVALUES[j2].d_lineNum, W == U);
                        ASSERTV(FVALUES[i1].d_lineNum,
                                TVALUES[i2].d_lineNum,
                                FVALUES[j1].d_lineNum,
                                TVALUES[j2].d_lineNum, W == V);
                    }
                }
            }
        }

        // Testing assignment u = u (Aliasing).
        for (int i1 = 0; i1 < NUM_FVALUES; ++i1) {
            for (int i2 = 0; i2 < NUM_TVALUES; ++i2) {
                Obj u(oa);  const Obj& U = u;
                u.setFormat(FVALUES[i1].d_format);
                u.setTimestampOffset(
                      bdlt::DatetimeInterval(TVALUES[i2].d_days,
                                            TVALUES[i2].d_hours,
                                            TVALUES[i2].d_mins,
                                            TVALUES[i2].d_secs,
                                            TVALUES[i2].d_msecs));
                Obj w(U, oa);  const Obj &W = w;
                u = u;
                ASSERTV(FVALUES[i1].d_lineNum,
                        TVALUES[i2].d_lineNum, W == U);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'g':
        //   Void for 'ball::RecordStringFormatter'.
        // --------------------------------------------------------------------
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using tested
        //   methods.  Then copy construct an object y from x, and use the
        //   equality operator to assert that both x and y have the same value
        //   as w.
        //
        // Testing:
        //   ball::RecordStringFormatter(const ball::RSF&, alloc);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_format;
            int         d_days;
            int         d_hrs;
            int         d_mins;
            int         d_secs;
            int         d_msecs;
        } VALUES[] = {
            //line fmt              days hrs mins secs msecs
            //---- ---------------  ---- --- ---- ---- -----
            { L_, "%%",              0,   0,   0,   0,   0   },
            { L_, "%d",             -1,  -1,  -1,  -1,   -1  },
            { L_, "%i",              0,   0,   0,   0,  999  },
            { L_, "%p",              0,   0,   0,   0, -999  },
            { L_, "%t",              0,   0,  59,   0,   0   },
            { L_, "%s",              0,   0,   0, -59,   0   },
            { L_, "%f",              0,   0,  59,   0,   0   },
            { L_, "%l",              0,   0, -59,   0,   0   },
            { L_, "%c",              0,  23,   0,   0,   0   },
            { L_, "%m",              0, -23,   0,   0,   0   },
            { L_, "%u",              5,   0,   0,   0,   0   },
            { L_, "",               -5,   0,   0,   0,   0   },
            { L_, "",                5,  23,  22,  21,  209  },
            { L_, "",               -5, -23, -59, -59, -999  },
            { L_, "attributes: %a", -5, -23, -59, -59, -999  },
        };

        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        bslma::TestAllocator testAllocator("objectAllocator",
                                           veryVeryVeryVerbose);
        Obj::allocator_type  oa(&testAllocator);

        for (int i = 0; i < NUM_VALUES; ++i) {
            const bdlt::DatetimeInterval interval(VALUES[i].d_days,
                                                 VALUES[i].d_hrs,
                                                 VALUES[i].d_mins,
                                                 VALUES[i].d_secs,
                                                 VALUES[i].d_msecs);

            Obj w(oa);  const Obj& W = w;           // control
            w.setFormat(VALUES[i].d_format);
            w.setTimestampOffset(interval);

            Obj x(oa);  const Obj& X = x;
            x.setFormat(VALUES[i].d_format);
            x.setTimestampOffset(interval);

            Obj y(X, oa);  const Obj& Y = y;
            ASSERTV(VALUES[i].d_lineNum, X == W);
            ASSERTV(VALUES[i].d_lineNum, Y == W);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //   Any subtle variation in value must be detected by the equality
        //   operators.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product
        //    S X S.
        //
        // Testing:
        //   bool operator==(const ball::RSF& lhs, const ball::RSF& rhs);
        //   bool operator!=(const ball::RSF& lhs, const ball::RSF& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Equality Operators"
                          << "\n==========================" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_format;
        } FVALUES[] = {
            // line   format
            // ----   ------
            {  L_,    "some format"                             },
            {  L_,    "other format"                            },
            {  L_,    "yet another format"                      },
            {  L_,    "\n%d %p:%t %s %f:%l %c %m %u\n"          },
            {  L_,    ""                                        },
        };

        static const struct {
            int d_lineNum;
            int d_days;
            int d_hours;
            int d_mins;
            int d_secs;
            int d_msecs;
        } TVALUES[] = {
            ///line  days hours mins secs msecs
            ///----  ---- ----- ---- ---- -----
            { L_,     1,   1,   1,   1,    1 },
            { L_,     1,   1,   1,   1,    2 },
            { L_,     1,   1,   1,   2,    1 },
            { L_,     1,   1,   2,   1,    1 },
            { L_,     1,   2,   1,   1,    1 },
            { L_,     2,   1,   1,   1,    1 },
            { L_, -9999, -23, -59, -59, -999 },
            { L_, -9999, -23, -59, -59, -998 },
            { L_, -9999, -23, -59, -58, -999 },
            { L_, -9999, -23, -58, -59, -999 },
            { L_, -9999, -22, -59, -59, -999 },
            { L_, -9998, -23, -59, -59, -999 },
        };

        const int NUM_FVALUES = sizeof FVALUES / sizeof *FVALUES;
        const int NUM_TVALUES = sizeof TVALUES / sizeof *TVALUES;

        bslma::TestAllocator testAllocator("objectAllocator",
                                           veryVeryVeryVerbose);
        Obj::allocator_type  oa(&testAllocator);

        for (int i1 = 0; i1 < NUM_FVALUES; ++i1) {
            for (int i2 = 0; i2 < NUM_TVALUES; ++i2) {
                Obj mA(oa);  const Obj& A = mA;
                mA.setFormat(FVALUES[i1].d_format);
                mA.setTimestampOffset(
                       bdlt::DatetimeInterval(TVALUES[i2].d_days,
                                             TVALUES[i2].d_hours,
                                             TVALUES[i2].d_mins,
                                             TVALUES[i2].d_secs,
                                             TVALUES[i2].d_msecs));

                for (int j1 = 0; j1 < NUM_FVALUES; ++j1) {
                    for (int j2 = 0; j2 < NUM_TVALUES; ++j2) {
                        Obj mB(oa);  const Obj& B = mB;
                        mB.setFormat(FVALUES[j1].d_format);
                        mB.setTimestampOffset(
                               bdlt::DatetimeInterval(TVALUES[j2].d_days,
                                                     TVALUES[j2].d_hours,
                                                     TVALUES[j2].d_mins,
                                                     TVALUES[j2].d_secs,
                                                     TVALUES[j2].d_msecs));

                        bool isSame = (i1 == j1) && (i2 == j2);
                        ASSERTV(FVALUES[i1].d_lineNum,
                                TVALUES[i2].d_lineNum,
                                FVALUES[j1].d_lineNum,
                                TVALUES[j2].d_lineNum,
                                isSame == (A == B));
                        ASSERTV(FVALUES[i1].d_lineNum,
                                TVALUES[i2].d_lineNum,
                                FVALUES[j1].d_lineNum,
                                TVALUES[j2].d_lineNum,
                                !isSame == (A != B));
                        ASSERTV(FVALUES[i1].d_lineNum,
                                TVALUES[i2].d_lineNum,
                                FVALUES[j1].d_lineNum,
                                TVALUES[j2].d_lineNum,
                                isSame == (B == A));
                        ASSERTV(FVALUES[i1].d_lineNum,
                                TVALUES[i2].d_lineNum,
                                FVALUES[j1].d_lineNum,
                                TVALUES[j2].d_lineNum,
                                !isSame == (B != A));
                    }
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //   The output operator is trivially implemented using the
        //   fully-tested 'bsl::string' and 'bdlt::DatetimeInterval' output
        //   operators; a very few test vectors can thoroughly test this
        //   functionality.
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   bsl::ostream& operator<<(bsl::ostream&, const ball::RSF&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Output (<<) Operator" << endl
                          << "============================" << endl;

        static const struct {
            int         d_lineNum;   // source line number
            const char *d_format;    // format string
            bool        d_localTime; // format in local time
            const char *d_output;    // expected output format
        } DATA[] = {
            //line fmt localTime expected output
            //---- --- --------- ---------------
            { L_, "%%", true,    "'%%' local-time" },
            { L_, "%",  true,    "'%' local-time"  },
            { L_, "%t", true,    "'%t' local-time" },
            { L_, "",   false,   "'' UTC"          },
            { L_, "\\", false,   "'\\' UTC"        },
        };


        bslma::TestAllocator testAllocator("objectAllocator",
                                           veryVeryVeryVerbose);
        Obj::allocator_type  oa(&testAllocator);

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            Obj mX(DATA[i].d_format, DATA[i].d_localTime, oa);
            const Obj& X = mX;

            ostringstream os;
            os << X;
            ASSERTV(os.str(),
                    DATA[i].d_output,
                    DATA[i].d_lineNum,
                    os.str() == DATA[i].d_output);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS:
        //
        //   This test case is redundant as it will be identical to case 2
        //   (primary manipulators)
        // --------------------------------------------------------------------
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'gg':
        //
        //   Void for 'ball::RecordStringFormatter'.
        // --------------------------------------------------------------------
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //   Setter functions should correctly pass the specified value to the
        //   object.
        //
        // Plan:
        //   First, verify the default constructor by testing the observable
        //   value of the resulting object.
        //
        //
        //   Next, for a sequence of independent test values, use the default
        //   constructor to create an object and use the primary manipulators
        //   to set its value.  Verify that value using the basic accessors.
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //   ball::RecordStringFormatter(*ba = 0);
        //   void setFormat(const char *format);
        //   void setTimestampOffset(const bdlt::DatetimeInterval& offset);
        //   ~ball::RecordStringFormatter();
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Primary Manipulator"
                          << "\n===========================" << endl;

        bslma::TestAllocator testAllocator("objectAllocator",
                                           veryVeryVeryVerbose);
        Obj::allocator_type  oa(&testAllocator);

        if (verbose) cout << "\n  Create a default object." << endl;
        {
            Obj mX(oa);  const Obj& X = mX;
            ASSERT( 0 == strcmp(F0, X.format()));
            ASSERT(T0 == X.timestampOffset());
        }

        if (verbose) cout << "\n  Testing setFormat and setTimestampOffset."
                          << endl;
        {
            static const struct {
                int         d_lineNum;
                const char *d_format;
                int         d_days;
                int         d_hrs;
                int         d_mins;
                int         d_secs;
                int         d_msecs;
            } VALUES[] = {
                //line format       days  hrs mins secs msecs
                //---- ------------ ----  --- ---- ---- -----
                { L_, "%%",            0,   0,   0,   0,   0   },
                { L_, "%d",           -1,  -1,  -1,  -1,   -1  },
                { L_, "%D",           -1,  -1,  -1,  -1,   -1  },
                { L_, "%dtz",          1,   1,   1,   1,    1  },
                { L_, "%Dtz",          2,   2,   2,   2,    2  },
                { L_, "%i",            0,   0,   0,   0,  999  },
                { L_, "%I",            0,   0,   0,   0,  998  },
                { L_, "%O",            0,   0,   0,   0,  997  },
                { L_, "%p",            0,   0,   0,   0, -999  },
                { L_, "%t",            0,   0,  59,   0,   0   },
                { L_, "%s",            0,   0,   0, -59,   0   },
                { L_, "%f",            0,   0,  59,   0,   0   },
                { L_, "%l",            0,   0, -59,   0,   0   },
                { L_, "%c",            0,  23,   0,   0,   0   },
                { L_, "%m",            0, -23,   0,   0,   0   },
                { L_, "%x",            0, -23,   0,   0,   0   },
                { L_, "%u",            5,   0,   0,   0,   0   },
                { L_, "%d%f",          0,   1,   2,   3,   4   },
                { L_, "%Dtz %l text",  0,  -1,  -2,  -3,  -4   },
                { L_, "",             -5,   0,   0,   0,   0   },
                { L_, "",              5,  23,  22,  21,  209  },
                { L_, "",             -5, -23, -59, -59, -999  },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bdlt::DatetimeInterval interval(VALUES[i].d_days,
                                                      VALUES[i].d_hrs,
                                                      VALUES[i].d_mins,
                                                      VALUES[i].d_secs,
                                                      VALUES[i].d_msecs);

                Obj mX(oa);  const Obj& X = mX;
                mX.setFormat(VALUES[i].d_format);
                mX.setTimestampOffset(interval);
                ASSERTV(VALUES[i].d_lineNum,
                        0 == strcmp(VALUES[i].d_format, X.format()));
                ASSERTV(VALUES[i].d_lineNum, interval == X.timestampOffset());
            }

            // reverse the order of two set* functions
            for (int i = 0; i < NUM_VALUES; ++i) {
                const bdlt::DatetimeInterval interval(VALUES[i].d_days,
                                                      VALUES[i].d_hrs,
                                                      VALUES[i].d_mins,
                                                      VALUES[i].d_secs,
                                                      VALUES[i].d_msecs);

                Obj mX(oa);  const Obj& X = mX;
                mX.setTimestampOffset(interval);
                mX.setFormat(VALUES[i].d_format);
                ASSERTV(VALUES[i].d_lineNum,
                        0 == strcmp(VALUES[i].d_format, X.format()));
                ASSERTV(VALUES[i].d_lineNum, interval == X.timestampOffset());
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //   Create four test objects by using the default, initializing, and
        //   copy constructors.  Exercise the basic value-semantic methods and
        //   the equality operators using these test objects.  Invoke the
        //   primary manipulator [3, 6], copy constructor [2, 8], and
        //   assignment operator without [9, 10] and with [11] aliasing.  Use
        //   the direct accessors to verify the expected results.  Display
        //   object values frequently in verbose mode.  Note that 'VA', 'VB',
        //   and 'VC' denote unique, but otherwise arbitrary, object values,
        //   while '0' denotes the default object value.
        //
        // 1.  Create a default object x1.          { x1:0 }
        // 2.  Create an object x2 (copy from x1).  { x1:0  x2:0 }
        // 3.  Set x1 to VA.                        { x1:VA x2:0 }
        // 4.  Set x2 to VA.                        { x1:VA x2:VA }
        // 5.  Set x2 to VB.                        { x1:VA x2:VB }
        // 6.  Set x1 to 0.                         { x1:0  x2:VB }
        // 7.  Create an object x3 (with value VC). { x1:0  x2:VB x3:VC }
        // 8.  Create an object x4 (copy from x1).  { x1:0  x2:VB x3:VC x4:0 }
        // 9.  Assign x2 = x1.                      { x1:0  x2:0  x3:VC x4:0 }
        // 10. Assign x2 = x3.                      { x1:0  x2:VC x3:VC x4:0 }
        // 11. Assign x1 = x1 (aliasing).           { x1:0  x2:VC x3:VC x4:0 }
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        if (verbose) cout << "\n 1. Create a default object x1." << endl;

        bslma::TestAllocator testAllocator("objectAllocator",
                                           veryVeryVeryVerbose);
        Obj::allocator_type  oa(&testAllocator);

        Obj mX1(oa);  const Obj& X1 = mX1;
        ASSERT( 0 == strcmp(F0, X1.format()));
        ASSERT(T0 == X1.timestampOffset());
        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                          << endl;
        Obj mX2(X1, oa);  const Obj& X2 = mX2;
        ASSERT( 0 == strcmp(F0, X2.format()));
        ASSERT(T0 == X2.timestampOffset());
        ASSERT( 1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT( 1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        if (verbose) cout << "\n 3. Set x1 to VA." << endl;
        mX1.setFormat(FA);
        mX1.setTimestampOffset(TA);
        ASSERT( 0 == strcmp(FA, X1.format()));
        ASSERT(TA == X1.timestampOffset());
        ASSERT( 1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT( 0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 4. Set x2 to VA." << endl;
        mX2.setFormat(FA);
        mX2.setTimestampOffset(TA);
        ASSERT( 0 == strcmp(FA, X2.format()));
        ASSERT(TA == X2.timestampOffset());
        ASSERT( 1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT( 1 == (X1 == X2));        ASSERT(0 == (X1 != X2));

        if (verbose) cout << "\n 5. Set x2 to VB." << endl;
        mX2.setFormat(FB);
        mX2.setTimestampOffset(TB);
        ASSERT( 0 == strcmp(FB, X2.format()));
        ASSERT(TB == X2.timestampOffset());
        ASSERT( 1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT( 0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 6. Set x1 to 0." << endl;
        mX1.setFormat(F0);
        mX1.setTimestampOffset(T0);
        ASSERT( 0 == strcmp(F0, X1.format()));
        ASSERT(T0 == X1.timestampOffset());
        ASSERT( 1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT( 0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 7. Create an object x3 (with value VC)."
                           << endl;
        Obj mX3(FC, TC, oa);  const Obj& X3 = mX3;
        ASSERT( 0 == strcmp(FC, X3.format()));
        ASSERT(TC == X3.timestampOffset());
        ASSERT( 1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT( 0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT( 0 == (X3 == X2));        ASSERT(1 == (X3 != X2));

        if (verbose) cout << "\n 8. Create an object x4 (copy from x1)."
                           << endl;
        Obj mX4(X1, oa);  const Obj& X4 = mX4;
        ASSERT( 0 == strcmp(F0, X4.format()));
        ASSERT(T0 == X4.timestampOffset());
        ASSERT( 1 == (X4 == X1));        ASSERT(0 == (X4 != X1));
        ASSERT( 0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT( 0 == (X4 == X3));        ASSERT(1 == (X4 != X3));
        ASSERT( 1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        if (verbose) cout << "\n 9. Assign x2 = x1." << endl;
        mX2 = X1;
        ASSERT( 0 == strcmp(F0, X2.format()));
        ASSERT(T0 == X2.timestampOffset());
        ASSERT( 1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT( 1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT( 0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT( 1 == (X2 == X4));        ASSERT(0 == (X2 != X4));

        if (verbose) cout << "\n 10. Assign x2 = x3." << endl;
        mX2 = X3;
        ASSERT( 0 == strcmp(FC, X2.format()));
        ASSERT(TC == X2.timestampOffset());
        ASSERT( 0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT( 1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT( 1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT( 0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        if (verbose) cout << "\n 11. Assign x1 = x1 (aliasing)." << endl;
        mX1 = X1;
        ASSERT( 0 == strcmp(F0, X1.format()));
        ASSERT(T0 == X1.timestampOffset());
        ASSERT( 1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT( 0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT( 0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT( 1 == (X1 == X4));        ASSERT(0 == (X1 != X4));
      } break;
      default:
        {
            cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
            testStatus = -1;
        }
    }

    // CONCERN: In no case does memory come from the default allocator.

    ASSERT(dam.isTotalSame());

    // CONCERN: In no case does memory come from the global allocator.

    ASSERT(gam.isTotalSame());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
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
