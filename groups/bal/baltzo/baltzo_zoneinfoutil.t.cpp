// baltzo_zoneinfoutil.t.cpp                                          -*-C++-*-
#include <baltzo_zoneinfoutil.h>

#include <baltzo_localtimedescriptor.h>
#include <baltzo_zoneinfo.h>

#include <ball_administration.h>
#include <ball_defaultobserver.h>
#include <ball_log.h>
#include <ball_loggermanager.h>
#include <ball_loggermanagerconfiguration.h>
#include <ball_severity.h>

#include <bdlt_iso8601util.h>

#include <bdlt_dateutil.h>
#include <bdlt_dateutil.h>
#include <bdlt_epochutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'baltzo::ZoneinfoUtil' is a utility for performing operations using a
// 'baltzo::Zoneinfo' value.  This test driver tests each implemented utility
// function independently.
//
// Global Concerns:
//: o Pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o All explicit memory allocations are presumed to use the global, default,
//:   or object allocator.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 3] void convertUtcToLocalTime(DatetimeTz *, Transition *, UTC, Zone);
// [ 4] void loadRelevantTransitions(TIt *, TIt *, Valid *, localTime, TZ);
// [ 2] bool isWellFormed(const baltzo::Zoneinfo& timeZone);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
// [ 4] CONCERN: parameters are declared 'const'.
// [ 4] CONCERN: No memory is ever allocated from the global allocator.
// [ 4] CONCERN: Precondition violations are detected.
//-----------------------------------------------------------------------------
//=============================================================================

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
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
//                   STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\t" \
                    << #K << ": " << K <<  "\n"; aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
#define P(X)  cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X)  cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_    cout << "\t" << flush;          // Print a tab (w/o newline)
#define L_ __LINE__                           // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
typedef baltzo::ZoneinfoUtil                      Obj;
typedef baltzo::Zoneinfo                          Tz;
typedef baltzo::Zoneinfo::TransitionConstIterator TzIt;
typedef baltzo::LocalTimeDescriptor               Desc;
typedef baltzo::LocalTimeValidity                 Validity;

// ============================================================================
//                              TEST FUNCTIONS
// ----------------------------------------------------------------------------

bdlt::EpochUtil::TimeT64 toTimeT(const bdlt::Datetime& value)
    // Return the interval in seconds from UNIX epoch time of the specified
    // 'value'.  Note that this method is shorthand for
    // 'bdlt::EpochUtil::convertToTimeT64'.
{
    return bdlt::EpochUtil::convertToTimeT64(value);
}

bdlt::Datetime toDatetime(const char *iso8601TimeString)
    // Return the datetime value indicated by the specified
    // 'iso8601TimeString'.  The behavior is undefined unless
    // 'iso8601TimeString' is a null-terminated C-string containing a time
    // description matching the iso8601 specification (see 'bdlt_iso8601util').
{
    bdlt::Datetime time;
    int rc = bdlt::Iso8601Util::parse(&time,
                                  iso8601TimeString,
                                  bsl::strlen(iso8601TimeString));
    BSLS_ASSERT(0 == rc);
    return time;
}

int toOffsetInMilliseconds(const char *iso8601Value)
    // Return the number of milliseconds in the specified 'iso8601Value',
    // where 'iso8601Value' is a iso860 time, optionally prefixed by white
    // space and a '-' character (for negative offsets) -- i.e.,
    // " *-?HH:MM:SS.mmm".
{
    // Drop initial white space
    for (; *iso8601Value && *iso8601Value==' '; ++iso8601Value) {
    }

    BSLS_ASSERT(*iso8601Value);

    int sign = 1;
    if (*iso8601Value == '-') {
        sign = -1;
        ++iso8601Value;
    }

    int length = strlen(iso8601Value);

    bdlt::Time time;
    int rc = bdlt::Iso8601Util::parse(&time, iso8601Value, length);

    BSLS_ASSERT(0 == rc);
    return sign * (
           (1000 * 60 * 60 * time.hour()) +
           (1000 * 60 * time.minute()) +
           (1000 * time.second()) +
            time.millisecond());
}

struct TransitionDescription {
    // A 'struct' describing a transitions.  Note that this type is meant to
    // be used to create data tables for use with 'addTransitions'.

    int         d_line;
    const char *d_transitionTime;
    int         d_offsetMins;
    const char *d_abbrev;
    bool        d_isDst;
};

void addTransitions(baltzo::Zoneinfo            *result,
                    const TransitionDescription *descriptions,
                    int                          numDescriptions)
    // Insert to the specified 'result' the contiguous sequence of specified
    // 'descriptions', of length 'numDescriptions'.
{
    BSLS_ASSERT(result);
    for (int i = 0; i < numDescriptions; ++i) {
        const char *TRANS = descriptions[i].d_transitionTime;
        baltzo::LocalTimeDescriptor desc(descriptions[i].d_offsetMins * 60,
                                        descriptions[i].d_isDst,
                                        descriptions[i].d_abbrev);
        result->addTransition(toTimeT(toDatetime(TRANS)), desc);
    }
}

// ============================================================================
//                        GLOBAL CLASSES FOR TESTING
// ----------------------------------------------------------------------------

struct LogVerbosityGuard {
    // The Logger verbosity guard disables logging on construction, and
    // re-enables logging, based on the prior default pass-through level, when
    // it goes out of scope and is destroyed.  It is intended to suppress
    // logged output for intentional errors when the test driver is run in
    // non-verbose mode.

    bool d_verbose;             // verbose mode does not disable logging
    int  d_defaultPassthrough;  // default passthrough log level

    LogVerbosityGuard(bool verbose = false)
        // If the optionally specified 'verbose' is 'false' disable logging
        // until this guard is destroyed.
    {
        d_verbose = verbose;
        if (!d_verbose) {
            d_defaultPassthrough =
                  ball::LoggerManager::singleton().defaultPassThresholdLevel();

            ball::Administration::setDefaultThresholdLevels(
                                              ball::Severity::e_OFF,
                                              ball::Severity::e_OFF,
                                              ball::Severity::e_OFF,
                                              ball::Severity::e_OFF);
            ball::Administration::setThresholdLevels(
                                              "*",
                                              ball::Severity::e_OFF,
                                              ball::Severity::e_OFF,
                                              ball::Severity::e_OFF,
                                              ball::Severity::e_OFF);

        }
    }

    ~LogVerbosityGuard()
        // Set the logging verbosity back to its default state.
    {
        if (!d_verbose) {
            ball::Administration::setDefaultThresholdLevels(
                                              ball::Severity::e_OFF,
                                              d_defaultPassthrough,
                                              ball::Severity::e_OFF,
                                              ball::Severity::e_OFF);
            ball::Administration::setThresholdLevels(
                                              "*",
                                              ball::Severity::e_OFF,
                                              d_defaultPassthrough,
                                              ball::Severity::e_OFF,
                                              ball::Severity::e_OFF);
        }
    }
};

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    ball::DefaultObserver            observer(&bsl::cout);
    ball::LoggerManagerConfiguration configuration;
    ball::LoggerManager&             manager =
                  ball::LoggerManager::initSingleton(&observer, configuration);

    bslma::TestAllocator defaultAllocator;  // To be used to make sure the
                                            // allocator is always passed down
                                            // where necessary.

    bslma::TestAllocator  testAllocator;
    bslma::TestAllocator *Z = &testAllocator;  // To be used to allocate
                                               // everything in this code.

    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    const bdlt::EpochUtil::TimeT64 MIN_DATETIME =
                                              toTimeT(bdlt::Datetime(1, 1, 1));
    const bdlt::EpochUtil::TimeT64 MAX_DATETIME =
                        toTimeT(bdlt::Datetime(9999, 12, 31, 23, 59, 59, 999));

    const Validity::Enum U = baltzo::LocalTimeValidity::e_VALID_UNIQUE;
    const Validity::Enum A = baltzo::LocalTimeValidity::e_VALID_AMBIGUOUS;
    const Validity::Enum I = baltzo::LocalTimeValidity::e_INVALID;

    switch (test) { case 0:
      case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'BSLS_ASSERT' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

///Usage
///-----
// The following examples demonstrate how to use a 'ZoneinfoUtil' to perform
// common operations on time values using a Zoneinfo description of a time
// zone.
//
///Prologue: Initializing a 'baltzo::Zoneinfo' object.
///- - - - - - - - - - - - - - - - - - - - - - - - -
// We start by creating a Zoneinfo time zone description for New York, which
// we will use in subsequent examples.  Note that, in practice, clients should
// obtain time zone information from a data source (see
// 'baltzo_zoneinfocache').
//
// First we create a Zoneinfo object for New York, and populate 'newYork' with
// the correct time zone identifier:
//..
    baltzo::Zoneinfo newYork;
    newYork.setIdentifier("America/New_York");
//..
// Next we create two local-time descriptors, one for standard time and one
// for daylight-saving time:
//..
    baltzo::LocalTimeDescriptor est(-18000, false, "EST");
    baltzo::LocalTimeDescriptor edt(-14400, true,  "EDT");
//..
// Then we set the initial descriptor for 'newYork' to Eastern Standard
// Time.  Note that such an initial transition is required for a
// 'baltzo::Zoneinfo' object to be considered Well-Defined (see
// 'isWellFormed'):
//..
    newYork.addTransition(bdlt::EpochUtil::convertToTimeT64(
                                                      bdlt::Datetime(1, 1, 1)),
                          est);
//..
// Next we create a series of transitions between these local-time descriptors
// for the years 2007-2011.  Note that the United States transitions to
// daylight saving time on the second Sunday in March, at 2am local time
// (07:00 UTC), and transitions back to standard time on the first Sunday in
// November at 2am local time (06:00 UTC), resulting in an even number of
// transitions:
//..
    static const bdlt::Datetime TRANSITION_TIMES[] = {
        bdlt::Datetime(2007,  3, 11, 7),
        bdlt::Datetime(2007, 11,  4, 6),
        bdlt::Datetime(2008,  3,  9, 7),
        bdlt::Datetime(2008, 11,  2, 6),
        bdlt::Datetime(2009,  3,  8, 7),
        bdlt::Datetime(2009, 11,  1, 6),
        bdlt::Datetime(2010,  3, 14, 7),
        bdlt::Datetime(2010, 11,  7, 6),
        bdlt::Datetime(2011,  3, 13, 7),
        bdlt::Datetime(2011, 11,  6, 6),
    };
    const int NUM_TRANSITION_TIMES =
                            sizeof TRANSITION_TIMES / sizeof *TRANSITION_TIMES;
    ASSERT(0 == NUM_TRANSITION_TIMES % 2);
//
    for (int i = 0; i < NUM_TRANSITION_TIMES; i += 2) {
        newYork.addTransition(bdlt::EpochUtil::convertToTimeT64(
                                                      TRANSITION_TIMES[i]),
                              edt);
        newYork.addTransition(bdlt::EpochUtil::convertToTimeT64(
                                                     TRANSITION_TIMES[i + 1]),
                              est);
    }
//..
// Finally we verify that the time zone information we've created is
// considered well-defined (as discussed above):
//..
    ASSERT(true == baltzo::ZoneinfoUtil::isWellFormed(newYork));
//..
//
///Example 1: Converting from a UTC time to a local time.
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we demonstrate how to convert a UTC time to the
// corresponding local time using the 'convertUtcToLocalTime' class method.
//
// We start by creating a 'bdlt::Datetime' representing the UTC time
// "Dec 12, 2010 15:00":
//..
    bdlt::Datetime utcTime(2010, 12, 12, 15, 0, 0);
//..
// Now, we call 'convertUtcToLocalTime' and supply as input both 'utcTime' and
// the Zoneinfo description for 'newYork' (which we initialized in the prologue
// above):
//..
    bdlt::DatetimeTz                          localNYTime;
    baltzo::Zoneinfo::TransitionConstIterator iterator;
    baltzo::ZoneinfoUtil::convertUtcToLocalTime(&localNYTime,
                                               &iterator,
                                               utcTime,
                                               newYork);
//..
// Then we verify that 'localNYTime' is "Dec 12, 2010 10:00+5:00", the time in
// New York corresponding to the UTC time "Dec 12, 2010 15:00".
//..
    ASSERT(utcTime                         == localNYTime.utcDatetime());
    ASSERT(bdlt::Datetime(2010, 12, 12, 10) == localNYTime.localDatetime());
    ASSERT(-5 * 60                         == localNYTime.offset());
//..
// Finally, we verify that the returned 'iterator' refers to the local-time
// transition immediately before 'utcTime', and that that transition refers to
// a local-time descriptor characterizing standard-time in New York:
//..
    baltzo::Zoneinfo::TransitionConstIterator transitionIter     = iterator;
    baltzo::Zoneinfo::TransitionConstIterator nextTransitionIter = ++iterator;
//
    const bdlt::EpochUtil::TimeT64 utcTimeT =
                                    bdlt::EpochUtil::convertToTimeT64(utcTime);
    ASSERT(utcTimeT >= transitionIter->utcTime());
    ASSERT(utcTimeT <  nextTransitionIter->utcTime());
//
    ASSERT(false        == transitionIter->descriptor().dstInEffectFlag());
    ASSERT(-5 * 60 * 60 == transitionIter->descriptor().utcOffsetInSeconds());
    ASSERT("EST"        == transitionIter->descriptor().description());
//..
//
///Example 2: Determining the Type of a Local Time
///- - - - - - - - - - - - - - - - - - - - - - - -
// In this next example we use 'loadRelevantTransitions' to determine the
// local-time descriptor (see 'baltzo_localtimedescriptor') that applies to a
// local time value, represented using a 'bdlt::Datetime' object.
//
// We start by defining a 'bdlt::Datetime' object for "Jan 1, 2011 12:00" in
// New York:
//..
    bdlt::Datetime nyLocalTime(2011, 1, 1, 12);
//..
// Then, we call 'loadRelevantTransitions', and supply, as input, both
// 'nyLocalTime' and the Zoneinfo description for 'newYork' (which we
// initialized in the prologue above):
//..
    baltzo::LocalTimeValidity::Enum           validity;
    baltzo::Zoneinfo::TransitionConstIterator firstTransition;
    baltzo::Zoneinfo::TransitionConstIterator secondTransition;
    baltzo::ZoneinfoUtil::loadRelevantTransitions(&firstTransition,
                                                  &secondTransition,
                                                  &validity,
                                                  nyLocalTime,
                                                  newYork);
//..
// "Jan 1, 2011 12:00" in New York, is not near a daylight-saving time
// transition, so it uniquely describes a valid time (in New York) which falls
// during Eastern Standard Time, and whose local time offset from UTC is
// -5:00.  Because "Jan 1, 2011 12:00" is both a valid and unique local time,
// the returned validity will be
// 'baltzo::LocalTimeValidity::e_VALID_UNIQUE' and the two returned transition
// iterators will be equal:
//..
    ASSERT(baltzo::LocalTimeValidity::e_VALID_UNIQUE == validity);
    ASSERT(firstTransition == secondTransition);
//
    ASSERT(false    == firstTransition->descriptor().dstInEffectFlag());
    ASSERT(-5*60*60 == firstTransition->descriptor().utcOffsetInSeconds());
    ASSERT("EST"    == firstTransition->descriptor().description());
//..
// Next, we create a second 'bdlt::Datetime' object to represent
// "Nov 7, 2010 1:30" in New York.  Note that the clock time
// "Nov 7, 2010 1:30" occurred twice in New York, as clocks were set back by an
// hour an instant before the local clock would have reached
// "Nov 7, 2010 02:00 EDT", and it is therefore ambiguous which of those two
// values that local time is meant to refer.
//..
    bdlt::Datetime ambiguousLocalTime(2010, 11, 7, 1, 30);
//..
// Now, we call 'loadRelevantTransitions', this time supplying
// 'ambiguousLocalTime':
//..
    baltzo::ZoneinfoUtil::loadRelevantTransitions(&firstTransition,
                                                 &secondTransition,
                                                 &validity,
                                                 ambiguousLocalTime,
                                                 newYork);
//..
// Finally we observe that the local time was ambiguous and that the returned
// transitions are distinct:
//..
    ASSERT(baltzo::LocalTimeValidity::e_VALID_AMBIGUOUS == validity);
    ASSERT(firstTransition != secondTransition);
//..
// Because 'ambiguousLocalTime' may refer to either the standard or the
// daylight-saving time value "Nov 7, 2010 01:30", the returned validity will
// be 'e_VALID_AMBIGUOUS', and the 'first' and 'second' iterators will differ.
// 'first' will refer to a description of the local time before the transition
// (daylight-saving time) and 'second' will refer to a description of local
// time after the transition (standard-time):
//..
    ASSERT(true      == firstTransition->descriptor().dstInEffectFlag());
    ASSERT(-4*60*60  == firstTransition->descriptor().utcOffsetInSeconds());
    ASSERT("EDT"     == firstTransition->descriptor().description());
//
    ASSERT(false     == secondTransition->descriptor().dstInEffectFlag());
    ASSERT(-5*60*60  == secondTransition->descriptor().utcOffsetInSeconds());
    ASSERT("EST"     == secondTransition->descriptor().description());
//..
// Note that the two transitions returned are adjacent:
//..
    ++firstTransition;
    ASSERT(firstTransition == secondTransition);
//..

    } break;
     case 5: {
        // --------------------------------------------------------------------
        // TESTING: 'loadRelevantTransitions'
        //   Ensure that 'loadRelevantTransitions' returns either one relevant
        //   transition, or two adjacent relevant transitions, and the correct
        //   validity status for the supplied local time.
        //
        // Concerns:
        //  1 'loadRelevantTransitions' returns a single transition for
        //     unambiguous input times, whose UTC offset, when applied to the
        //     input time, results in a time within the interval over which
        //     that transition applies.
        //
        //  2 'loadRelevantTransitions' returns a two transitions for
        //     ambiguous input times, whose UTC offsets, when applied to the
        //     input time, both result in a time within the interval over which
        //     that transition applies.
        //
        //  3 'loadRelevantTransitions' returns a two transitions for
        //     invalid input times, the first UTC offset, when applied to the
        //     latter transition, falls before the supplied time, and the
        //     latter UTC offset, when applied to the latter transitions,
        //     falls after the supplied time.
        //
        //  4 'loadRelevantTransitions' correctly accepts, as input, a time
        //     zone with many transitions, and 'utcTimes' anywhere within the
        //     range of valid transitions.
        //
        //  5 'loadRelevantTransitions' correctly accepts, as input, a time
        //     zone with a single transition.
        //
        //  6 'loadRelevantTransitions' returns the correct validity status
        //     for times near a transition value where the UTC offset is
        //     increasing (creating an interval of invalid times), decreases
        //     (creating an interval of ambiguous times), or remains the
        //     same.  Note that in the special case where the UTC offset
        //     remains the same at a transition, there is no interval of
        //     either ambiguity or invalidity.
        //
        //  5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //  1 Using a table-driven approach: (C-1..4, 6)
        //    1 Create a test time zone with transitions at various UTC times
        //      with various UTC offsets.
        //
        //    2 Create a test table where each row indicates a UTC test value
        //      to pass to 'loadRelevantTransitions' (for the test time zone),
        //      and the expected result values for the validity and transition
        //      iterators.
        //
        //    3 For each element of the test table:
        //      1 Call 'loadRelevantTransitions' with the test UTC value, and
        //        verify that the results match the expected results in the
        //        test table.
        //
        //  2 Using a table-driven approach with an "Oracle": (C-1..3, 6)
        //    1 Create a varied set of time-differences, representing the
        //      difference in UTC offsets between two local time descriptors.
        //
        //    2 Create a varied set of epsilon values, representing
        //      test points in a range around a time that we will test.
        //
        //    3 For each time-difference in the set of 'DIFFERENCES':
        //      1 Create a test time zone, with an initial transition with a
        //        UTC offset of 0, and a second transition whose UTC offset is
        //        the current 'DIFFERENCE' (from 0)
        //
        //      2 Create a 'PRE_TRANSITION' time value, representing the
        //        local-time an instant before the transition, and
        //        'POST_TRANSITION', representing the local-time an instant
        //        after the transition.
        //
        //      3 For each possible epsilon in the set of test epsilons:
        //        Note that the following steps define an "Oracle" for testing
        //        the results of 'loadRelevantTransitions':
        //        1 For both 'PRE_TRANSITION' and 'POST_TRANSITION':
        //          1 Apply the current epsilon (to either 'PRE_TRANSITION' or
        //            'POST_TRANSITION') to create a test local time value.
        //
        //          2 Call 'loadRelevantTransitions' on the test local time
        //            value.
        //
        //          3 If the test local time is outside the range defined by
        //            'PRE_TRANSITION' and 'POST_TRANSITION' then verify the
        //            result is unambiguous, and that the correct transition is
        //            returned.
        //
        //          4 If test local time is in the range defined by
        //            'PRE_TRANSITION' and 'POST_TRANSITION' and
        //            'PRE_TRANSITION > POST_TRANSITION', then verify the
        //            result is VALID_BUT_AMBIGUOUS, and that the correct two
        //            transitions are returned.
        //
        //          5 If test local time is in the range defined by
        //            'PRE_TRANSITION' and 'POST_TRANSITION' and
        //            'PRE_TRANSITION < POST_TRANSITION', then verify the
        //            result is 'INVALID', and that the correct two
        //            transitions are returned.
        //
        //  3 Using a table-driven approach with an Oracle (C-4):
        //    Note that this test was added as a refinement to the previous
        //    test (P-2) that focused on Concern 4, testing a variety of
        //    transition times:
        //    1 Create a time zone with a varied set of transitions.
        //
        //    2 Create a table with a varied series of epsilon value.
        //
        //    3 For each transition in the test time zone:
        //      1 Create a vector of UTC times to test near that transition.
        //
        //      2 For each epsilon in the test table of epsilon values:
        //        1 Add an element to the test vector that is the current
        //          epsilon from: (1) local-time immediately before the
        //          current transition, (2) local-time immediately after the
        //          current transitions, (3) the current transition time.
        //
        //      3 Call 'loadRelevantTransitions' for each element in the
        //        vector of test values.
        //
        //      4 Use the "Oracle" procedure described in the previous test
        //      (P-2) to verify the result.
        //
        //  4 Using a table-driven approach (C-5):
        //    1 Create a table of test UTC times.
        //
        //    2 Create a time zone with a single transition (at 1/1/1_00:00,
        //      as required for well-formed time zones).
        //
        //    3 Call 'loadRelevantTransitions' for each test UTC time, and
        //      verify the correct (unique and valid) result is returned.
        //
        //  5 Verify that, in appropriate build modes, defensive checks are
        //    triggered for argument values (using the 'BSLS_ASSERTTEST_*'
        //    macros).  (C-7)
        //
        // Testing:
        //   void loadRelevantTransitions(TIt *, TIt *, Valid *, localTime, TZ)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: 'loadRelevantTransitions'" << endl
                          << "==================================" << endl;

        enum {
            MS_PER_MIN = 60 * 1000
        };

        {
            if (veryVerbose) {
                cout << "\tTest a time zone with multiple of transitions."
                     << endl;
            }
           const TransitionDescription TZ_DATA[] = {
                { L_, "0001-01-01T00:00:00.000",     0, "A", false},
                { L_, "0002-01-01T12:00:00.000", -1439, "B", false},
                { L_, "0003-01-01T12:00:00.000",  1439, "C", false},
                { L_, "9999-04-01T12:00:00.000",   -60, "D", false},
                { L_, "9999-12-31T23:59:59.000",     0, "E", false}
            };
            const int NUM_TZ_DATA = sizeof TZ_DATA / sizeof *TZ_DATA;

            Tz tz(Z); const Tz& TZ = tz;
            addTransitions(&tz, TZ_DATA, NUM_TZ_DATA);

            struct {
               int             d_line;
               const char     *d_testTime;
               Validity::Enum  d_validity;
               int             d_firstIdx;
               int             d_secondIdx;
            } DATA[] = {
                { L_, "0001-01-01T00:00:00.000", U, 0, 0 },
                // Transition idx 1: 0002-01-01T12:00:00.000, offset: -11:59
                { L_, "0001-12-31T12:00:00.000", U, 0, 0 },
                { L_, "0001-12-31T12:00:59.999", U, 0, 0 },
                { L_, "0001-12-31T12:01:00.000", A, 0, 1 },
                { L_, "0001-12-31T12:01:01.000", A, 0, 1 },
                { L_, "0002-01-01T00:00:00.000", A, 0, 1 },
                { L_, "0002-01-01T11:59:59.999", A, 0, 1 },
                { L_, "0002-01-01T12:00:00.000", U, 1, 1 },
                { L_, "0002-01-01T12:00:01.000", U, 1, 1 },
                // Transition idx 2: "0003-01-01T12:00:00.000", offset: +11:59
                { L_, "0002-12-31T12:00:00.000", U, 1, 1 },
                { L_, "0002-12-31T12:00:59.999", U, 1, 1 },
                { L_, "0002-12-31T12:01:00.000", I, 1, 2 },
                { L_, "0002-12-31T12:01:01.000", I, 1, 2 },
                { L_, "0003-01-01T00:00:00.000", I, 1, 2 },
                { L_, "0003-01-02T11:58:59.999", I, 1, 2 },
                { L_, "0003-01-02T11:59:00.000", U, 2, 2 },
                { L_, "0003-01-02T12:00:00.000", U, 2, 2 },
                // Transition idx 3: "9999-04-01T12:00:00.000", offset: -01:00
                { L_, "9999-04-01T10:59:59.999", U, 2, 2 },
                { L_, "9999-04-01T11:00:00.000", A, 2, 3 },
                { L_, "9999-04-01T11:00:01.000", A, 2, 3 },
                { L_, "9999-04-01T12:00:00.000", A, 2, 3 },
                { L_, "9999-04-02T11:58:59.999", A, 2, 3 },
                { L_, "9999-04-02T11:59:00.000", U, 3, 3 },
                { L_, "9999-04-02T12:00:00.000", U, 3, 3 },
                // Transition idx 4: "9999-12-31T23:59:59.000", offset: 0
                { L_, "9999-12-31T22:59:58.999", U, 3, 3 },
                { L_, "9999-12-31T22:59:59.000", I, 3, 4 },
                { L_, "9999-12-31T23:30:00.000", I, 3, 4 },
                { L_, "9999-12-31T23:59:58.999", I, 3, 4 },
                { L_, "9999-12-31T23:59:59.000", U, 4, 4 },
                { L_, "9999-12-31T23:59:59.999", U, 4, 4 },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE    = DATA[i].d_line;
                const char *EXP_ID1 = TZ_DATA[DATA[i].d_firstIdx ].d_abbrev;
                const char *EXP_ID2 = TZ_DATA[DATA[i].d_secondIdx].d_abbrev;
                const Validity::Enum EXP_VALIDITY = DATA[i].d_validity;
                const bsl::string inputStr(DATA[i].d_testTime);

                bdlt::Datetime    inputTime;
                ASSERT(0 == bdlt::Iso8601Util::parse(&inputTime,
                                                 inputStr.c_str(),
                                                 inputStr.size()));

                TzIt           resultIt1, resultIt2;
                Validity::Enum resultValidity;
                Obj::loadRelevantTransitions(&resultIt1,
                                             &resultIt2,
                                             &resultValidity,
                                             inputTime,
                                             TZ);
                LOOP_ASSERT(LINE,
                            EXP_ID1 == resultIt1->descriptor().description());
                LOOP_ASSERT(LINE,
                            EXP_ID2 == resultIt2->descriptor().description());
                LOOP_ASSERT(LINE, EXP_VALIDITY == resultValidity);
            }
        }
        {
            if (veryVerbose) {
                cout << "\tTest differences between adjacent transitions"
                     << endl;
            }

            // Each value in the set of 'DIFFERENCES' represents the
            // *difference* between two adjacent local time periods.
            const char *DIFFERENCES[] = {
                "-23:59:59",
                "-23:59:58",
                "-23:59:57",
                "-22:59:00",
                "-22:00:00",
                "-12:00:00",
                "-01:00:00",
                "-00:30:00",
                "-00:01:00",
                "-00:00:30",
                "-00:00:01",
                " 00:00:00",
                " 00:00:01",
                "-00:00:30",
                " 00:01:00",
                " 00:30:00",
                " 01:00:00",
                " 12:00:00",
                " 22:00:00",
                " 23:59:00",
                " 23:59:57",
                " 23:59:58",
                " 23:59:59"
            };
            const int NUM_DIFFERENCES = sizeof DIFFERENCES /
                                        sizeof *DIFFERENCES;

            // Each value in the set of 'EPSILONS' represents an offset from a
            // time value that we want to test.
            const char *EPSILONS[] = {
                "-01:00:00.000",
                "-00:01:00.000",
                "-00:00:03.000",
                "-00:00:02.000",
                "-00:00:01.000",
                "-00:00:00.003",
                "-00:00:00.002",
                "-00:00:00.001",
                " 00:00:00.000",
                " 00:00:00.001",
                " 00:00:00.002",
                " 00:00:00.003",
                " 00:00:01.000",
                " 00:00:02.000",
                " 00:00:03.000",
                " 00:01:00.000",
                " 01:00:00.000"
            };
            const int NUM_EPSILONS = sizeof EPSILONS / sizeof *EPSILONS;

            const bdlt::Datetime TRANS_TIME(2000, 1, 1);
            const int           TRANS_TIME_T = toTimeT(TRANS_TIME);

            for (int i = 0; i < NUM_DIFFERENCES; ++i) {
                Tz tz(Z); const Tz& TZ = tz;
                Desc desc1(0, false, "A");
                tz.addTransition(MIN_DATETIME, desc1);

                const int DIFF = toOffsetInMilliseconds(DIFFERENCES[i])/1000;

                Desc desc2(DIFF, false, "B");
                tz.addTransition(TRANS_TIME_T, desc2);

                // Create two local times, the first, 'PRE_TRANSITION',
                // representing local time the instant before the transition,
                // and the second, 'POST_TRANSITIONS', representing local time
                // the instant after the transition.
                //..
                //
                //                                            either ambiguous
                //                                        ,-- or invalid times
                //                                       /
                //                                      /
                //                                     /
                //                            minTime / maxTime
                // Local Time +-----------------O  -  -  @---------
                //                             /     ___/
                //                            /  ___/
                //                           /__/
                // UTC Time   +-------------+----------------------
                //            | Previous    | Current
                //            | Descriptor  | Descriptor
                //            |-------------|----------------------
                //            |             |
                //            Previous      Current
                //            Transition    Transition
                //..

                bdlt::Datetime PRE_TRANSITION(TRANS_TIME);
                bdlt::Datetime POST_TRANSITION(TRANS_TIME);
                POST_TRANSITION.addSeconds(DIFF);

                // Note that if 'PRE_TRANSITION > POST_TRANSITION' (i.e.,
                // 'DIFF < 0') then the range of local times between
                // PRE_TRANSITION and POST_TRANSITIONS are ambiguous,
                // otherwise they are invalid.
                bdlt::Datetime minTime = PRE_TRANSITION < POST_TRANSITION
                                        ? PRE_TRANSITION
                                        : POST_TRANSITION;
                bdlt::Datetime maxTime = PRE_TRANSITION > POST_TRANSITION
                                        ? PRE_TRANSITION
                                        : POST_TRANSITION;

                if (veryVeryVerbose) {
                    P_(DIFF);
                    P_(PRE_TRANSITION);
                    P(POST_TRANSITION);
                }

                for (int j = 0; j < NUM_EPSILONS; ++j) {
                    // Iterate over adjustments to create a range of test
                    // local-time values around both PRE_TRANSITION and
                    // POST_TRANSITION values.
                    const int ADJUST = toOffsetInMilliseconds(EPSILONS[j]);

                    bdlt::Datetime TEST_PRE(PRE_TRANSITION);
                    bdlt::Datetime TEST_POST(POST_TRANSITION);
                    TEST_PRE.addMilliseconds(ADJUST);
                    TEST_POST.addMilliseconds(ADJUST);

                    bdlt::Datetime VALUES[2] = { TEST_PRE, TEST_POST };

                    for (int k = 0; k < 2; ++k) {
                        const bdlt::Datetime& VALUE = VALUES[k];

                        if (veryVeryVerbose) {
                            P(VALUE);
                        }
                        TzIt           resultIt1, resultIt2;
                        Validity::Enum resultValidity;
                        Obj::loadRelevantTransitions(&resultIt1,
                                                     &resultIt2,
                                                     &resultValidity,
                                                     VALUE,
                                                     TZ);
                        if (VALUE < minTime || VALUE >= maxTime) {
                            ASSERT(U         == resultValidity);
                            ASSERT(resultIt1 == resultIt2);
                            const Desc *EXP = VALUE<minTime ? &desc1 : &desc2;

                            bdlt::Datetime utcTime(VALUE);
                            utcTime.addSeconds(
                                -resultIt1->descriptor().utcOffsetInSeconds());
                            ASSERT(resultIt1 ==
                                   TZ.findTransitionForUtcTime(utcTime));
                            ASSERT(*EXP == resultIt1->descriptor());
                        }
                        else if (DIFF < 0) {
                            ASSERT(A         == resultValidity);
                            ASSERT(resultIt1 != resultIt2);
                            TzIt next = resultIt1; ++next;
                            ASSERT(next      == resultIt2);

                            ASSERT(desc1 == resultIt1->descriptor());
                            ASSERT(desc2 == resultIt2->descriptor());

                            // The definition of ambiguity is that there are
                            // two valid UTC times which correspond to UTC
                            // time.
                            bdlt::Datetime utcTime1(VALUE), utcTime2(VALUE);
                            utcTime1.addSeconds(
                                -resultIt1->descriptor().utcOffsetInSeconds());
                            utcTime2.addSeconds(
                                -resultIt2->descriptor().utcOffsetInSeconds());
                            ASSERT(resultIt1 ==
                                   TZ.findTransitionForUtcTime(utcTime1));
                            ASSERT(resultIt2 ==
                                   TZ.findTransitionForUtcTime(utcTime2));
                        }
                        else {
                            BSLS_ASSERT(DIFF > 0);

                            ASSERT(I         == resultValidity);
                            ASSERT(resultIt1 != resultIt2);
                            TzIt next = resultIt1; ++next;
                            ASSERT(next      == resultIt2);

                            ASSERT(desc1 == resultIt1->descriptor());
                            ASSERT(desc2 == resultIt2->descriptor());

                            ASSERT(VALUE >= PRE_TRANSITION);
                            ASSERT(VALUE <  POST_TRANSITION);
                        }
                    }
                }
            }
        }
        {
            // Note that this test is very duplicative with the preceding
            // test, but tests a range a time zone with a range of transition
            // values.

            if (veryVerbose) {
                cout << "\tTest a time zone with multiple of transitions."
                     << endl;
            }

            const TransitionDescription DATA[] = {
                { L_, "0001-01-01T00:00:00.000",     0, "A", false},
                { L_, "0002-01-01T12:00:00.000", -1439, "B", false},
                { L_, "0003-01-01T12:00:00.000",  1439, "C", false},
                { L_, "1000-05-31T12:00:00.000", -1439, "D", false},
                { L_, "1001-03-13T07:00:00.000",  1439, "E", false},
                { L_, "2012-01-01T12:00:00.000",     1, "F", false},
                { L_, "2013-01-01T12:00:00.000",    -1, "G", false},
                { L_, "9999-01-01T12:00:00.000",    60, "H", false},
                { L_, "9999-03-01T12:00:00.000",    60, "I", false},
                { L_, "9999-04-01T12:00:00.000",   -60, "J", false},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            // Each value in the set of 'EPSILONS' represents an offset from a
            // time value that we want to test.
            const char *EPSILONS[] = {
                "-01:00:00.000",
                "-00:01:00.000",
                "-00:00:03.000",
                "-00:00:02.000",
                "-00:00:01.000",
                "-00:00:00.003",
                "-00:00:00.002",
                "-00:00:00.001",
                " 00:00:00.000",
                " 00:00:00.001",
                " 00:00:00.002",
                " 00:00:00.003",
                " 00:00:01.000",
                " 00:00:02.000",
                " 00:00:03.000",
                " 00:01:00.000",
                " 01:00:00.000"
            };
            const int NUM_EPSILONS = sizeof EPSILONS / sizeof *EPSILONS;

            Tz tz(Z); const Tz& TZ = tz;
            addTransitions(&tz, DATA, NUM_DATA);

            for (int i = 1; i < NUM_DATA; ++i) {
                const bdlt::Datetime UTC_TRANSITION(
                                         toDatetime(DATA[i].d_transitionTime));
                const char          *ID1            = DATA[i-1].d_abbrev;
                const char          *ID2            = DATA[i].d_abbrev;

                const int    prevOffset = DATA[i - 1].d_offsetMins;
                const int currentOffset = DATA[i].d_offsetMins;

                bdlt::Datetime PRE_TRANSITION(UTC_TRANSITION);
                PRE_TRANSITION.addMinutes(prevOffset);
                bdlt::Datetime POST_TRANSITION(UTC_TRANSITION);
                POST_TRANSITION.addMinutes(currentOffset);

                bdlt::Datetime minTime = PRE_TRANSITION < POST_TRANSITION
                                        ? PRE_TRANSITION
                                        : POST_TRANSITION;
                bdlt::Datetime maxTime = PRE_TRANSITION > POST_TRANSITION
                                        ? PRE_TRANSITION
                                        : POST_TRANSITION;

                if (veryVeryVerbose) {
                    P(UTC_TRANSITION);
                    P_(PRE_TRANSITION); P(prevOffset);
                    P_(POST_TRANSITION); P(currentOffset);
                    P(minTime);
                    P(maxTime);
                }

                bsl::vector<bdlt::EpochUtil::TimeT64> TEST_OFFSETS(Z);
                for (int j = 0; j < NUM_EPSILONS; ++j) {
                    const int OFFSET = toOffsetInMilliseconds(EPSILONS[j]);
                    TEST_OFFSETS.push_back(OFFSET);
                    TEST_OFFSETS.push_back(prevOffset*MS_PER_MIN + OFFSET);
                    TEST_OFFSETS.push_back(currentOffset*MS_PER_MIN + OFFSET);
                }

                for (int j = 0; j < TEST_OFFSETS.size(); ++j) {
                    bdlt::Datetime VALUE(UTC_TRANSITION);
                    VALUE.addMilliseconds(TEST_OFFSETS[j]);

                    if (veryVeryVerbose) {
                        P(VALUE);
                    }
                    TzIt           resultIt1, resultIt2;
                    Validity::Enum resultValidity;
                    Obj::loadRelevantTransitions(&resultIt1,
                                                 &resultIt2,
                                                 &resultValidity,
                                                 VALUE,
                                                 TZ);
                    if (VALUE < minTime || VALUE >= maxTime) {
                        ASSERT(U         == resultValidity);
                        ASSERT(resultIt1 == resultIt2);
                        const char *EXP = VALUE < minTime ? ID1 : ID2;
                        ASSERT(EXP == resultIt1->descriptor().description());
                    }
                    else if (prevOffset > currentOffset) {
                        ASSERT(A         == resultValidity);
                        ASSERT(resultIt1 != resultIt2);
                        TzIt next = resultIt1; ++next;
                        ASSERT(next      == resultIt2);
                        ASSERT(ID1 == resultIt1->descriptor().description());
                        ASSERT(ID2 == resultIt2->descriptor().description());
                    }
                    else {
                        BSLS_ASSERT(prevOffset < currentOffset);
                        ASSERT(I         == resultValidity);
                        ASSERT(resultIt1 != resultIt2);
                        TzIt next = resultIt1; ++next;
                        ASSERT(next      == resultIt2);

                        ASSERT(ID1 == resultIt1->descriptor().description());
                        ASSERT(ID2 == resultIt2->descriptor().description());

                        ASSERT(VALUE >= PRE_TRANSITION);
                        ASSERT(VALUE <  POST_TRANSITION);
                    }
                }
            }
        }
        {
            if (veryVerbose) {
                cout << "\tTest a time zone with a single transition."
                     << endl;
            }

            Tz tz(Z); const Tz& TZ = tz;
            Desc desc(0, false, "A");
            tz.addTransition(MIN_DATETIME, desc);

            const bdlt::Datetime TEST_TIMES[] = {
                bdlt::Datetime(1, 1, 1),
                bdlt::Datetime(1, 1, 2),
                bdlt::Datetime(2, 1, 1),
                bdlt::Datetime(1000, 5, 31),
                bdlt::Datetime(2011, 3, 13, 7),
                bdlt::Datetime(2012, 1, 1),
                bdlt::Datetime(9999, 1, 1),
                bdlt::Datetime(9999, 3, 1),
                bdlt::Datetime(9999, 12, 31, 23, 59, 59, 999)
            };
            const int NUM_TEST_TIMES = sizeof TEST_TIMES / sizeof *TEST_TIMES;

            for (int i = 0; i < NUM_TEST_TIMES; ++i) {
                const bdlt::Datetime TEST_TIME = TEST_TIMES[i];

                if (veryVeryVerbose) {
                    P(TEST_TIME);
                }
                TzIt           resultIt1, resultIt2;
                Validity::Enum resultValidity;
                Obj::loadRelevantTransitions(&resultIt1,
                                             &resultIt2,
                                             &resultValidity,
                                             TEST_TIME,
                                             TZ);
                ASSERT(U         == resultValidity);
                ASSERT(resultIt1 == resultIt2);
                ASSERT(desc      == resultIt1->descriptor());
            }
        }

        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);
            if (veryVerbose) cout << "\tTest assertions." << endl;

            bdlt::Datetime   time(2010, 1, 1);
            Validity::Enum  resultValidity;
            TzIt            resultIt1, resultIt2;

            Tz tz(Z); const Tz& TZ = tz;
            ASSERT_SAFE_FAIL(Obj::loadRelevantTransitions(&resultIt1,
                                                          &resultIt2,
                                                          &resultValidity,
                                                          time,
                                                          TZ));
            tz.addTransition(MIN_DATETIME, Desc(0, false, ""));

            ASSERT_SAFE_PASS(Obj::loadRelevantTransitions(&resultIt1,
                                                          &resultIt2,
                                                          &resultValidity,
                                                          time,
                                                          TZ));

            ASSERT_FAIL(Obj::loadRelevantTransitions(&resultIt1,
                                                     &resultIt2,
                                                     0,
                                                     time,
                                                     TZ));
            ASSERT_FAIL(Obj::loadRelevantTransitions(&resultIt1,
                                                     0,
                                                     &resultValidity,
                                                     time,
                                                     TZ));
            ASSERT_FAIL(Obj::loadRelevantTransitions(0,
                                                     &resultIt2,
                                                     &resultValidity,
                                                     time,
                                                     TZ));
            ASSERT_FAIL(Obj::loadRelevantTransitions(&resultIt1,
                                                     &resultIt1,
                                                     &resultValidity,
                                                     time,
                                                     TZ));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING: 'convertUtcToLocalTime'
        //   Ensure that 'convertUtcToLocalTime' returns the correct
        //   local-time value for the provided UTC time value.
        //
        //   White-box note: these concerns assume that
        //   'baltzo::Zoneinfo::findTransitionForUtcTime' correctly finds that
        //   transition at or before the provided 'utcTime'.
        //
        // Concerns:
        //: 1 Sanity test: 'convertUtcToLocalTime' returns the correct
        //:   transition for input times immediately before, at, and after, a
        //:   transition.  Note that this is really re-testing
        //:   'baltzo::Zoneinfo::findTransitionForUtcTime'
        //:
        //: 2 That the returned local-time is correctly adjusted from the
        //:   input UTC time by the offset found in the returned transition,
        //:   and that offset also matches that in the returned UTC offset.
        //:
        //: 3 That 'convertUtcToLocalTime' returns both a local time
        //:   (adjusted from the provided UTC time) and a UTC offset that are
        //:   rounded down to the nearest minute (even in instances where the
        //:   Zoneinfo specifies an offset in seconds).
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using a table-driven approach (C-1):
        //:   1 Create a test time zone value with a varied set of transitions
        //:
        //:   2 For each transition in the test time zone:
        //:     1 For a sequence of negative and positive adjustments:
        //:       1 Adjust the current transition by the current adjustment to
        //:         create a test time value
        //:
        //:       2 Generate an expected result, either based on the previous
        //:         transition (if the adjustment is negative) or the current
        //:         transition.
        //:
        //:       3 Call 'convertUtcToLocalTime' and test the result against
        //:         the expected result.
        //:
        //: 2 Using a table-driven approach (C-2):
        //:   1 Create a table of varying UTC offset values.
        //:
        //:   2 For each test UTC offset value:
        //:     1 Create a well-defined test time zone value with a transition
        //:       holding a descriptor with the test UTC offset value.
        //:
        //:     2 Search for the test time transition.
        //:
        //:     2 Verify that the returned result is adjusted correctly by the
        //:       test offset value.
        //:
        //: 3 Using a table-driven approach (C-3):
        //:   1 Create a table of UTC offset values in seconds and the rounded
        //:     value in minutes, emphasizing the boundary cases for rounding
        //:     seconds to minutes.
        //:
        //:   2 For each test UTC offset value:
        //:     1 Create a well-defined test time zone value with a transition
        //:       holding a descriptor with the test UTC offset value.
        //:
        //:     2 Search for the test time transition.
        //:
        //:     2 Verify that the returned result is adjusted correctly by the
        //:       rounded test offset value.
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for argument values (using the 'BSLS_ASSERTTEST_*'
        //:   macros).  (C-4)
        //
        // Testing:
        //   void convertUtcToLocalTime(DatetimeTz *, Transition *, UTC, Zone);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: 'convertUtcToLocalTime'" << endl
                          << "================================" << endl;
        {
            if (veryVerbose) {
                cout << "\tTesting 1ms before, at, and 1ms after transitions"
                     << endl;
            }

            const TransitionDescription DATA[] = {
                { L_, "0001-01-01T00:00:00.000",     0, "A", false},
                { L_, "0002-01-01T12:00:00.000",     1, "B", false},
                { L_, "0003-01-01T12:00:00.000",     2, "C", false},
                { L_, "1000-05-31T12:00:00.000",     3, "D", false},
                { L_, "1001-03-13T07:00:00.000",     4, "E", false},
                { L_, "2012-01-01T12:00:00.000",     5, "F", false},
                { L_, "2013-01-01T12:00:00.000",     6, "G", false},
                { L_, "9999-01-01T12:00:00.000",     7, "H", false},
                { L_, "9999-03-01T12:00:00.000",     8, "I", false},
                { L_, "9999-04-01T12:00:00.000",     9, "J", false},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            Tz tz(Z); const Tz& TZ = tz;
            addTransitions(&tz, DATA, NUM_DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_line;
                for (int offset = -5; offset < 6; ++offset) {
                    bdlt::Datetime value(toDatetime(DATA[i].d_transitionTime));
                    const bdlt::Datetime& VALUE = value;

                    if (value == bdlt::Datetime(1, 1, 1) && offset < 0) {
                        continue;
                    }
                    value.addMilliseconds(offset);

                    const int RESULT_IDX = (offset < 0) ? i - 1 : i;
                    Desc expDesc(DATA[RESULT_IDX].d_offsetMins * 60,
                                 DATA[RESULT_IDX].d_isDst,
                                 DATA[RESULT_IDX].d_abbrev,
                                 Z);
                    bdlt::Datetime expLocal = value;
                    expLocal.addSeconds(expDesc.utcOffsetInSeconds());
                    const int expTzOffset(expDesc.utcOffsetInSeconds()/60);

                    bdlt::DatetimeTz result;
                    TzIt            resultIt;
                    Obj::convertUtcToLocalTime(&result,
                                               &resultIt,
                                               VALUE,
                                               TZ);

                    if (veryVeryVerbose) {
                        P_(LINE); P_(offset); P(result);
                    }
                    ASSERT(resultIt->utcTime() ==
                           toTimeT(toDatetime(
                                       DATA[RESULT_IDX].d_transitionTime)));
                    ASSERT(expDesc     == resultIt->descriptor());
                    ASSERT(VALUE       == result.utcDatetime());
                    ASSERT(expLocal    == result.localDatetime());
                    ASSERT(expTzOffset == result.offset());
                    ASSERT(resultIt    == TZ.findTransitionForUtcTime(VALUE));
                }
            }
        }
        {
            if (veryVerbose) {
                cout << "\tTesting a range of TZ offsets"
                     << endl;
            }
            const int DATA[] = {
                -1339, -1000, -60, -1, 0, 1, 60, 1000, 1339
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int OFFSET = DATA[i];

                const bdlt::Datetime TIME(2000, 1, 1);
                bdlt::EpochUtil::TimeT64 TIME_T = toTimeT(TIME);

                Tz tz(Z); const Tz& TZ = tz;
                tz.addTransition(MIN_DATETIME, Desc(0, false, ""));
                tz.addTransition(TIME_T, Desc(OFFSET * 60, false, ""));

                bdlt::Datetime expLocal = TIME;
                expLocal.addMinutes(OFFSET);
                const int N = testAllocator.numBytesInUse();

                bdlt::DatetimeTz result;
                TzIt            resultIt;
                Obj::convertUtcToLocalTime(&result,
                                           &resultIt,
                                           TIME,
                                           TZ);

                ASSERT(N == testAllocator.numBytesInUse());
                ASSERT(0 == defaultAllocator.numBytesInUse());

                ASSERT(TIME_T   == resultIt->utcTime());
                ASSERT(TIME     == result.utcDatetime());
                ASSERT(expLocal == result.localDatetime());
                ASSERT(OFFSET   == result.offset());
            }
        }
        {
            if (veryVerbose) {
                cout << "\tTesting precision of TZ offsets" << endl;
            }
            struct {
                int d_line;
                int d_offset;
                int d_expResultOffsetMin;
            } DATA[] = {
                { L_,    1,  0 },
                { L_,   59,  0 },
                { L_,   60,  1 },
                { L_,   61,  1 },
                { L_,  119,  1 },
                { L_,  120,  2 },
                { L_,  121,  2 },
                { L_,   -1,  0 },
                { L_,  -59,  0 },
                { L_,  -60, -1 },
                { L_,  -61, -1 },
                { L_, -119, -1 },
                { L_, -120, -2 },
                { L_, -121, -2 }

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int OFFSET         = DATA[i].d_offset;
                const int EXP_RES_OFFSET = DATA[i].d_expResultOffsetMin;

                const bdlt::Datetime TIME(2000, 1, 1);
                bdlt::EpochUtil::TimeT64 TIME_T = toTimeT(TIME);

                Tz tz(Z); const Tz& TZ = tz;
                tz.addTransition(MIN_DATETIME, Desc(0, false, ""));
                tz.addTransition(TIME_T, Desc(OFFSET, false, ""));

                bdlt::Datetime expLocal = TIME;
                expLocal.addMinutes(EXP_RES_OFFSET);

                bdlt::DatetimeTz result;
                TzIt            resultIt;
                Obj::convertUtcToLocalTime(&result,
                                           &resultIt,
                                           TIME,
                                           TZ);

                ASSERT(TIME_T         == resultIt->utcTime());
                ASSERT(TIME           == result.utcDatetime());
                ASSERT(expLocal       == result.localDatetime());
                ASSERT(EXP_RES_OFFSET == result.offset());
            }
        }
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);
            if (veryVerbose) cout << "\tTest assertions." << endl;

            bdlt::Datetime   time(2010, 1, 1);
            bdlt::DatetimeTz result;
            TzIt            resultIt;

            Tz tz(Z); const Tz& TZ = tz;

            ASSERT_SAFE_FAIL(Obj::convertUtcToLocalTime(&result,
                                                        &resultIt,
                                                        time,
                                                        TZ));
            tz.addTransition(MIN_DATETIME, Desc(0, false, ""));
            ASSERT_SAFE_PASS(Obj::convertUtcToLocalTime(&result,
                                                        &resultIt,
                                                        time,
                                                        TZ));

            ASSERT_FAIL(Obj::convertUtcToLocalTime(&result,
                                                   0,
                                                   time,
                                                   TZ));
            ASSERT_FAIL(Obj::convertUtcToLocalTime(0,
                                                   &resultIt,
                                                   time,
                                                   TZ));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING: 'isWellFormed'
        //   Ensure that 'isWellFormed' correctly determines whether a
        //   'baltzo::Zoneinfo' meets the definition of a "well-defined' value.
        //
        // Concerns:
        //: 1 A default constructed Zoneinfo is not well-defined
        //:
        //: 2 Any time zone which does not have an initial transition at
        //:   1/1/1_00:00:00 is not well-defined.
        //:
        //: 3 For two transitions, if the local-time immediately before or
        //:   after the latter transition is at an earlier representation to
        //:   the local-time immediately before or after the former
        //:   transition, then the Zoneinfo is not well-defined.
        //:
        //: 4 That Concern 3 is true for every consecutive pair of transitions
        //:   in the sequence.
        //:
        //: 5 Zoneinfo objects that violate more than one constraint are
        //:   considered not well-formed.
        //:
        //: 6 Zoneinfo objects that violate none of the constraints are
        //:   considered well-formed.
        //
        // Plan:
        //: 1 Create a default constructed Zoneinfo and verify that
        //:   'isWellFormed' returns false.  (C-1)
        //:
        //: 2 Using a table-driven approach (C-2):
        //:   1 Create a set of varied time transition values including one at
        //:     the minimum representable 'bdlt::Datetime' value.
        //:
        //:   2 Iterate over the number of test table elements selecting an
        //:     initial index.  Creating a default (empty) Zoneinfo and mark it
        //:     as invalid:
        //:     1 For each initial index, iterate over the number of test
        //:       table elements (modulo the length of the test table)
        //:       inserting a transition into the table.
        //:
        //:     2 If the newly added transition is at 'bdlt::Datetime(1, 1, 1)'
        //:       mark the Zoneinfo as valid.
        //:
        //:     3 Test whether 'isWellFormed' returns the expected result.
        //:
        //: 3 Using a table-driven approach (C-3)
        //:   1 Create a table describing 3 consecutive transitions in a time
        //:     zone.  In particular, define 3 UTC offsets ('utcOffsetT0',
        //:     'utcOffsetT1', 'utcOffsetT2'), for the series of transitions
        //:     (T0, T1, and T2), a time interval between T1 and T2
        //:     ('interval'), and an indication of whether the resulting
        //:     sequence of  transitions is well-formed.
        //:
        //:   2 For each test element in the test table:
        //:     1 Create a Zoneinfo object holding 3 transitions:
        //:       T0: At    1/1/1_00:00:00           UTC Offset: utcOffsetT0
        //:       T1: At 1000/1/1_00:00:00           UTC Offset: utcOffsetT1
        //:       T2: At 1000/1/1_00:00:00+interval  UTC Offset: utcOffsetT0
        //:
        //:   3 Call 'isWellFormed' and verify it returns the expected result.
        //:
        //: 4 Using a table-driven approach (C-4)
        //:   1 Create a table describing a time zone with transitions
        //:     occurring at various UTC times.  Configuration an initial time
        //:     zone value using those transitions.
        //:
        //:   2 For transitions in the initial time zone value:
        //:     1 Copy the initial time zone value and verify it is
        //:       well-formed.
        //:
        //:     2 Add an additional transition 1 second after the current
        //:       transition, with a UTC offset of -01:00.
        //:
        //:     3 Verify the resulting Zoneinfo is not well-formed.
        //:
        //: 4 Create a Zoneinfo without a transition at the 1/1/1_00:00:00 and
        //:   two transitions within 24 hours of each other.  Verify
        //:   'isWellFormed' returns 'false'.
        //
        // Testing:
        //   int toOffsetInMilliseconds(const char *iso8601Value)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: 'isWellFormed'" << endl
                          << "========================" << endl;

        LogVerbosityGuard logGuard;
        baltzo::LocalTimeDescriptor desc(0, false, "junk", Z);
        {
            if (veryVerbose) {
                cout << "\tTesting a default constructed 'Zoneinfo' "
                     << "(constraint 1)." << endl;
            }
            Tz tz(Z); const Tz& TZ = tz;

            const int N = testAllocator.numBytesInUse();
            ASSERT(false == Obj::isWellFormed(TZ));
            ASSERT(N == testAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        {
            if (veryVerbose) {
                cout << "\tTesting initial transition at 1/1/1_00:00:00 "
                     << "(constraint 2)." << endl;
            }
            const bdlt::Datetime DATA[] = {
                bdlt::Datetime(1, 1, 1),
                bdlt::Datetime(1, 1, 2),
                bdlt::Datetime(2, 1, 1),
                bdlt::Datetime(9999, 12, 31, 23, 59, 59, 999),
                bdlt::Datetime(1, 1, 1),
                bdlt::Datetime(9999,  1,  1, 1),
                bdlt::Datetime(1000,  1,  1, 1),
                bdlt::Datetime(2011,  3, 13, 7),
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int INITIAL_INDEX = i;
                bool      VALID         = false;
                Tz tz(Z); const Tz& TZ = tz;

                // For the i'th iteration of the outer loop, start adding
                // transitions at the i'th element of the 'DATA' array.
                for (int j = 0; j < NUM_DATA; ++j) {
                    const int INDEX   = (INITIAL_INDEX + j) % NUM_DATA;

                    ASSERT(VALID == Obj::isWellFormed(TZ));

                    if (bdlt::Datetime(1, 1, 1) == DATA[INDEX]) {
                        VALID = true;
                    }
                    bdlt::EpochUtil::TimeT64 timeT = toTimeT(DATA[INDEX]);
                    tz.addTransition(timeT, desc);
                    ASSERT(VALID == Obj::isWellFormed(TZ));
                }
            }
        }
        {
            if (veryVerbose) {
                cout << "\tTesting adjacent transitions for overlapping "
                     << "invalid ranges (constraint 3)." << endl;
            }

            // For this test case, we create 3 transitions:
            //   + T0: At    1/1/1_00:00:00                  and 'utcOffsetT0'
            //   + T1: At 1000/1/1_00:00:00'                 and 'utcOffsetT1'
            //   + T2: At 1000/1/1_00:00:00 + 'intervalMins' and 'utcOffsetT2'
            //
            // There are 4 cases that are relevant:
            // 1) Valid transitions -- No overlaps in the ranges of ambiguous
            //    or invalid times.
            //..
            // (1) Valid Transitions
            //
            //                                  ,-- range of ambiguous/invalid
            //                                 /         local times
            //                                /___________________
            //                               /                    \
            //                         @--------O             @--------O
            //
            // Local Time +------------O  -  -  @-------------O  -  -  @------
            //                        /     ___/             /     ___/
            //           UtcOffsetT0 /  ___/   UtcOffsetT1  /  ___/ UtcOffsetT2
            //                      /__/                   /__/
            // UTC Time   +--------+----------------------+-------------------
            //            T0       T1 -- intervalMins --> T2
            //         (1/1/1)   (1000/1/1)
            //..
            // 2) Invalid I -- T1 adjusts local time forward,
            //                 T2 adjusts local time back.
            //..
            // Local Time               T1          T1'
            //                          O- - - - - >@
            //                          |      T2'  |            T2
            //                          |       @< -|- - - - - - O
            //                         /        _\_/          __/
            //                        /     ___/  \        __/
            //                       /  ___/       \    __/
            //                      /__/            \__/
            // UTC Time   +--------+-----------------+-------------------
            //            T0       T1                T2
            //..
            // 2) Invalid II -- T1 adjusts local time backward,
            //                  T2 adjusts local time forward.
            //..
            // Local Time   T1'                      T1
            //              @< - - - - - - - - - - -O
            //               \        T2            |       T2'
            //                \        O- - - - - - | - - ->@
            //                 \        \       ___/    ___/
            //                  \        \  ___/    ___/
            //                   \      __\/    ___/
            //                    \ ___/   \___/
            // UTC Time   +--------+--------+----------------------------
            //            T0       T1       T2
            //..
            // 3) Invalid III -- T1 adjusts local time backward,
            //                   T2 adjusts local time backward
            //..
            // (3) Invalid Transitions Type III
            //
            // Local Time        T1'               T1
            //                   @< - - - - - -O
            //                T2' \        T2 /
            //                 @< -\- - - -O /
            //                  \___\       \
            //                       \\___ / \
            //                        \   /\  \
            //                         \_/  \__\
            // UTC Time   +------------+--------+----------------------------
            //            T0           T1       T2
            //..

            struct {
                int         d_line;
                const char *d_utcOffsetT0;
                const char *d_utcOffsetT1;
                int         d_intervalMins;
                const char *d_utcOffsetT2;
                bool        d_wellFormed;
            } DATA[] = {
                // Invalid Type I - T1 Forward, T2 Backward
        { L_, " 00:00:00.000", " 01:00:00.000",   61, " 00:00:00.000",  true },
        { L_, " 00:00:00.000", " 01:00:00.000",   60, " 00:00:00.000", false },
        { L_, " 00:00:00.000", " 01:00:00.000",   59, " 00:00:00.000", false },
        { L_, " 00:00:00.000", " 01:00:00.000",    1, " 00:00:00.000", false },

                // Invalid Type II - T1 Backward, T2 Forward
        { L_, " 00:00:00.000", "-01:00:00.000",   61, " 00:00:00.000",  true },
        { L_, " 00:00:00.000", "-01:00:00.000",   60, " 00:00:00.000", false },
        { L_, " 00:00:00.000", "-01:00:00.000",   59, " 00:00:00.000", false },
        { L_, " 00:00:00.000", "-01:00:00.000",    1, " 00:00:00.000", false },

                // Invalid Type III - T1 Backward, T2 Backward
        { L_, " 00:00:00.000", "-01:00:00.000",  121, "-02:00:00.000",  true },
        { L_, " 00:00:00.000", "-01:00:00.000",  120, "-02:00:00.000", false },
        { L_, " 00:00:00.000", "-01:00:00.000",  119, "-02:00:00.000", false },
        { L_, " 00:00:00.000", "-01:00:00.000",    1, "-02:00:00.000", false },

                // Valid Type - T1 Forward, T1 Forward
        { L_, " 00:00:00.000", " 01:00:00.000",    1, " 01:00:00.000",  true },
        { L_, " 00:00:00.000", " 01:00:00.000",    1, " 01:00:01.000",  true },
        { L_, " 00:00:00.000", " 01:00:00.000",    1, " 01:01:00.000",  true },
        { L_, " 00:00:00.000", " 01:00:00.000",    1, " 02:00:00.000",  true },

                        // Edge cases around 24 hour UTC offsets
        { L_, "-23:59:59.000", " 23:59:59.000", 2880, "-23:59:59.000",  true },
        { L_, "-23:59:59.000", " 23:59:59.000", 2879, "-23:59:59.000", false },
        { L_, "-23:59:59.000", " 23:59:59.000", 1440, "-23:59:59.000", false },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *OFFSET_T0  = DATA[i].d_utcOffsetT0;
                const char *OFFSET_T1  = DATA[i].d_utcOffsetT1;
                const char *OFFSET_T2  = DATA[i].d_utcOffsetT2;
                const int   INTERVAL_M = DATA[i].d_intervalMins;
                const bool  EXP_RESULT = DATA[i].d_wellFormed;

                const bdlt::EpochUtil::TimeT64 T0 = MIN_DATETIME;
                const bdlt::EpochUtil::TimeT64 T1 =
                                           toTimeT(bdlt::Datetime(1000, 1, 1));
                const bdlt::EpochUtil::TimeT64 T2 = T1 + (INTERVAL_M * 60);

                const int UTC_OFFSET_T0_S =
                                toOffsetInMilliseconds(OFFSET_T0) / 1000;
                const int UTC_OFFSET_T1_S =
                                toOffsetInMilliseconds(OFFSET_T1) / 1000;
                const int UTC_OFFSET_T2_S =
                                toOffsetInMilliseconds(OFFSET_T2) / 1000;

                Tz tz(Z); const Tz& TZ = tz;
                tz.addTransition(T0, Desc(UTC_OFFSET_T0_S, true, "A"));
                tz.addTransition(T1, Desc(UTC_OFFSET_T1_S, true, "B"));
                tz.addTransition(T2, Desc(UTC_OFFSET_T2_S, true, "C"));

                if (veryVeryVerbose) {
                    P(bdlt::EpochUtil::convertFromTimeT64(T0));
                    P(bdlt::EpochUtil::convertFromTimeT64(MIN_DATETIME));
                    P_(T0); P_(T1); P(T2);
                    P_(UTC_OFFSET_T0_S);
                    P_(UTC_OFFSET_T1_S);
                    P(UTC_OFFSET_T2_S);
                    TZ.print(cout, 1, 3);
                }
                LOOP_ASSERT(LINE, EXP_RESULT == Obj::isWellFormed(TZ));
            }
        }
        {
            if (veryVerbose) {
                cout << "\tTesting overlapping invalid ranges at different "
                     << "transition indices (constraint 3)." << endl;
            }
            const TransitionDescription DATA[] = {
                { L_, "0001-01-01T00:00:00.000", 0, "A", false},
                { L_, "0002-01-01T12:00:00.000", 0, "B", false},
                { L_, "0003-01-01T12:00:00.000", 0, "C", false},
                { L_, "1000-05-31T12:00:00.000", 0, "D", false},
                { L_, "1001-03-13T07:00:00.000", 0, "E", false},
                { L_, "9999-12-30T23:59:59.000", 0, "F", false},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            Tz initialTz(Z); const Tz& INITIAL_TZ = initialTz;
            addTransitions(&initialTz, DATA, NUM_DATA);

            for (TzIt it =  INITIAL_TZ.beginTransitions();
                      it != INITIAL_TZ.endTransitions();
                    ++it) {
                Tz tz(INITIAL_TZ, Z); const Tz& TZ = tz;
                ASSERT(Obj::isWellFormed(TZ));
                tz.addTransition(it->utcTime() + 1,
                                 Desc(-10, false, "X"));
                ASSERT(!Obj::isWellFormed(TZ));
            }
        }
        {
            if (veryVerbose) {
                cout << "\tTesting combinations of constraints."
                     << endl;
            }

            Tz tz(Z); const Tz& TZ = tz;
            tz.addTransition(toTimeT(bdlt::Datetime(1000, 1, 1)),
                             Desc(1439 * 60, 0, "A"));
            tz.addTransition(toTimeT(bdlt::Datetime(1000, 1, 1, 1)),
                             Desc(-1439 * 60, 0, "B"));
            ASSERT(false == Obj::isWellFormed(TZ));
            tz.addTransition(toTimeT(bdlt::Datetime(1, 1, 1)),
                             Desc(0, 0, "C"));
            ASSERT(false == Obj::isWellFormed(TZ));
        }
      } break;
        // --------------------------------------------------------------------
        // TESTING TEST APPARATUS
        //   That the various test functions behave as described.
        //
        // Concerns:
        //: 1 'toOffsetInSeconds' properly removes white space and returns the
        //:    correct (potentially negative) value in milliseconds.
        //:
        //: 2 'addTransitions' adds the sequence of specified transitions to a
        //:    time zone.
        //
        // Plan:
        //
        // Testing:
        //   int toOffsetInMilliseconds(const char *iso8601Value)
        //   void addTransitions(baltzo::Zoneinfo *, TransDescription *, int);
        // --------------------------------------------------------------------
      case 2: {

        if (verbose) cout << endl
                          << "TESTING TEST APPARATUS" << endl
                          << "======================" << endl;
        {
            if (veryVerbose) {
                cout << "\tTest 'toOffsetInMilliseconds'." << endl;
            }

            struct {
                int                 d_line;
                const char         *d_value;
                bsls::Types::Int64  d_expectedResult;
            } DATA[] = {
                { L_,     "00:00:00.000",        0 },
                { L_,    " 00:00:00.000",        0 },
                { L_, "    00:00:00.000",        0 },
                { L_, "   -00:00:00.000",        0 },
                { L_, "    00:00:00.125",      125 },
                { L_, "   -00:00:00.125",     -125 },
                { L_, "    00:00:01.000",     1000 },
                { L_, "   -00:00:01.000",    -1000 },
                { L_, "    00:01:01.000",    61000 },
                { L_, "   -00:01:01.000",   -61000 },
                { L_, "    01:02:03.004",  3723004 },
                { L_, "   -01:02:03.004", -3723004 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                 LINE   = DATA[i].d_line;
                const char               *VALUE  = DATA[i].d_value;
                const bsls::Types::Int64  RESULT = DATA[i].d_expectedResult;

                bsls::Types::Int64 result = toOffsetInMilliseconds(VALUE);
                LOOP_ASSERT(LINE, RESULT == result);
            }
        }
        {
            if (veryVerbose) {
                cout << "\tTest 'addTransitions'." << endl;
            }
            const TransitionDescription DATA[] = {
                { L_, "0001-01-01T00:00:00.000",  0, "A", false},
                { L_, "0002-03-04T05:06:07.000", -1, "B", true},
                { L_, "0004-05-06T07:08:09.000",  1, "C", false},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            Tz tz(Z); const Tz& TZ = tz;
            addTransitions(&tz, DATA, NUM_DATA);

            bsls::Types::Int64 T0 = toTimeT(bdlt::Datetime(1, 1, 1));
            bsls::Types::Int64 T1 = toTimeT(bdlt::Datetime(2, 3, 4, 5, 6, 7));
            bsls::Types::Int64 T2 = toTimeT(bdlt::Datetime(4, 5, 6, 7, 8, 9));

            TzIt it = TZ.beginTransitions();
            ASSERT(T0    == it->utcTime());
            ASSERT(0     == it->descriptor().utcOffsetInSeconds());
            ASSERT("A"   == it->descriptor().description());
            ASSERT(false == it->descriptor().dstInEffectFlag());

            ++it;
            ASSERT(T1    == it->utcTime());
            ASSERT(-60   == it->descriptor().utcOffsetInSeconds());
            ASSERT("B"   == it->descriptor().description());
            ASSERT(true  == it->descriptor().dstInEffectFlag());

            ++it;
            ASSERT(T2    == it->utcTime());
            ASSERT(60    == it->descriptor().utcOffsetInSeconds());
            ASSERT("C"   == it->descriptor().description());
            ASSERT(false == it->descriptor().dstInEffectFlag());

            ++it;
            ASSERT(TZ.endTransitions() == it);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Plan:
        //   Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        baltzo::LocalTimeDescriptor est(-18000, false, "EST");
        baltzo::LocalTimeDescriptor edt(-14400, true,  "EDT");
        baltzo::Zoneinfo newYork(Z);
        newYork.setIdentifier("America/New_York");
        newYork.addTransition(toTimeT(bdlt::Datetime(1, 1, 1)), est);
        static const bdlt::Datetime TRANSITION_TIMES[] = {
            bdlt::Datetime(2007,  3, 11, 7),
            bdlt::Datetime(2007, 11,  4, 6),
            bdlt::Datetime(2008,  3,  9, 7),
            bdlt::Datetime(2008, 11,  2, 6),
            bdlt::Datetime(2009,  3,  8, 7),
            bdlt::Datetime(2009, 11,  1, 6),
            bdlt::Datetime(2010,  3, 14, 7),
            bdlt::Datetime(2010, 11,  7, 6),
            bdlt::Datetime(2011,  3, 13, 7),
            bdlt::Datetime(2011, 11,  6, 6),
        };
        const int NUM_TRANSITION_TIMES =
                            sizeof TRANSITION_TIMES / sizeof *TRANSITION_TIMES;
        ASSERT(0 == NUM_TRANSITION_TIMES % 2);
        for (int i = 0; i < NUM_TRANSITION_TIMES; i += 2) {
            newYork.addTransition(toTimeT(TRANSITION_TIMES[i]), edt);
            newYork.addTransition(toTimeT(TRANSITION_TIMES[i + 1]), est);
        }
        if (veryVerbose) {
            newYork.print(bsl::cout, 1, 3);
        }

        if (verbose) cout << "\texercising 'convertUtcToLocalTime'" << endl;

        static const struct {
            int d_lineNum;   // source line number
            int d_year;      // local time year
            int d_month;     // local time month
            int d_day;       // local time day
            int d_hour;      // local time hour
            int d_minute;    // local time minute
            int d_second;    // local time second
            int d_offset;    // expected offset
            int d_dst;       // expected dst
        } DATA_UTC[] = {
            //LINE YEAR  MO. DAY HR. MIN SEC OFFSET        DST
            //---- ----  --- --- --- --- --- ------------  -----
            { L_,  2008,  2, 16, 17, 30, 00, -5 * 60 * 60, false  },
            { L_,  2008,  3,  9,  6, 59, 59, -5 * 60 * 60, false  },
            { L_,  2008,  3,  9,  7, 00, 00, -4 * 60 * 60, true   },
            { L_,  2008,  3,  9,  7, 00, 01, -4 * 60 * 60, true   },
            { L_,  2008, 11,  2,  5, 59, 59, -4 * 60 * 60, true   },
            { L_,  2008, 11,  2,  6, 00, 00, -5 * 60 * 60, false  },
            { L_,  2008, 11,  2,  6, 00, 01, -5 * 60 * 60, false  },
        };

        const int NUM_DATA_UTC = sizeof DATA_UTC / sizeof *DATA_UTC;

        for (int ti = 0; ti < NUM_DATA_UTC; ++ti) {
                const int LINE   = DATA_UTC[ti].d_lineNum;
                const int YEAR   = DATA_UTC[ti].d_year;
                const int MONTH  = DATA_UTC[ti].d_month;
                const int DAY    = DATA_UTC[ti].d_day;
                const int HOUR   = DATA_UTC[ti].d_hour;
                const int MINUTE = DATA_UTC[ti].d_minute;
                const int SECOND = DATA_UTC[ti].d_second;
                const int OFF    = DATA_UTC[ti].d_offset;
                const int DST    = DATA_UTC[ti].d_dst;

                if (veryVerbose) {
                    T_ P_(LINE) P_(YEAR)   P_(MONTH) P_(DAY)
                       P_(HOUR) P_(MINUTE) P(SECOND)
                    T_ P_(OFF) P(DST)
                }

                bdlt::Datetime x(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND);
                const bdlt::Datetime& X = x;

                bdlt::DatetimeTz y;
                const bdlt::DatetimeTz& Y = y;

                baltzo::Zoneinfo::TransitionConstIterator r;
                const baltzo::Zoneinfo::TransitionConstIterator& R = r;

                Obj::convertUtcToLocalTime(&y, &r, X, newYork);

                if (veryVerbose) { T_ P_(X) P(Y) }

                ASSERT(newYork.endTransitions() != R);
                LOOP_ASSERT(LINE,
                            R->descriptor().utcOffsetInSeconds() == OFF);
                LOOP_ASSERT(LINE,
                            R->descriptor().dstInEffectFlag()  == DST);
        }

        if (verbose) cout << "\texercising 'loadRelevantTransitions" << endl;
        static const struct {
            int         d_lineNum;   // source line number
            const char *d_datetime;
            int         d_validity;
        } DATA_LOCAL[] = {
            //LINE  Local Datetime            Validity
            //----  ------------------------  --------
            { L_,  "2008-02-16T17:30:00.000", U },

            { L_,  "2008-03-09T01:59:59.999", U },
            { L_,  "2008-03-09T02:00:00.000", I },
            { L_,  "2008-03-09T02:00:00.001", I },
            { L_,  "2008-03-09T02:59:59.999", I },
            { L_,  "2008-03-09T03:00:00.000", U },
            { L_,  "2008-03-09T03:00:00.001", U },

            { L_,  "2008-11-02T00:59:59.999", U },
            { L_,  "2008-11-02T01:00:00.000", A },
            { L_,  "2008-11-02T01:00:00.001", A },
            { L_,  "2008-11-02T01:59:59.000", A },
            { L_,  "2008-11-02T02:00:00.000", U },
            { L_,  "2008-11-02T02:00:00.001", U }
        };

        const int NUM_DATA_LOCAL = sizeof DATA_LOCAL / sizeof *DATA_LOCAL;

        for (int ti = 0; ti < NUM_DATA_LOCAL; ++ti) {
                const int LINE   = DATA_LOCAL[ti].d_lineNum;
                const int VALID  = DATA_LOCAL[ti].d_validity;
                const bsl::string inputStr(DATA_LOCAL[ti].d_datetime);
                if (veryVerbose) {
                    T_ P_(LINE); P_(inputStr); P(VALID);
                }

                bdlt::Datetime input;
                ASSERT(0 == bdlt::Iso8601Util::parse(&input,
                                                 inputStr.c_str(),
                                                 inputStr.size()));

                baltzo::Zoneinfo::TransitionConstIterator y;
                const baltzo::Zoneinfo::TransitionConstIterator& Y = y;
                baltzo::Zoneinfo::TransitionConstIterator z;
                const baltzo::Zoneinfo::TransitionConstIterator& Z = z;
                baltzo::LocalTimeValidity::Enum validity;

                Obj::loadRelevantTransitions(
                                           &y, &z, &validity, input, newYork);
                if (veryVerbose) {
                          T_
                          P_(Y->utcTime()) P_(Z->utcTime())
                          P_(Y->descriptor()) P(Z->descriptor())
                }

                LOOP3_ASSERT(LINE,
                             validity,
                             VALID,
                             validity == VALID);
        }
        {
            LogVerbosityGuard logGuard;
            if (verbose) cout << "\tTest 'isWellFormed'" << endl;

            // Test that violating each constraint will return 'false'.
            {
                // Test time zone with no transitions
                Tz mX(Z); const Tz& X = mX;
                ASSERT(false == Obj::isWellFormed(X));
            }
            {
                // Test the first transition at a time other than 1/1/1 00:00.
                Tz mX(Z); const Tz& X = mX;
                baltzo::LocalTimeDescriptor desc(0, true, "", Z);

                bdlt::Datetime invalid1(2000, 1, 1);
                bdlt::Datetime invalid2(1, 1, 2);
                bdlt::Datetime valid(1, 1, 1);

                mX.addTransition(toTimeT(invalid1), desc);
                ASSERT(false == Obj::isWellFormed(X));

                mX.addTransition(toTimeT(invalid2), desc);
                ASSERT(false == Obj::isWellFormed(X));

                mX.addTransition(toTimeT(valid), desc);
                ASSERT(true == Obj::isWellFormed(X));
            }
            {
                // Test time zone with multiple local time types one
                // transitions

                enum {
                    SIX_HOURS_IN_S = 6 * 60 * 60
                };

                Tz mX(Z); const Tz& X = mX;
                baltzo::LocalTimeDescriptor desc(0, true, "", Z);
                bdlt::Datetime initial(1, 1, 1);
                bdlt::Datetime t0(2000, 1, 1);
                bdlt::Datetime t1(2000, 1, 2);
                bdlt::Datetime t1Plus1Hour(2000, 1, 2, 1);
                bdlt::Datetime t2(2001, 1, 1);

                mX.addTransition(toTimeT(initial), Desc(0, true, ""));
                ASSERT(true == Obj::isWellFormed(X));

                mX.addTransition(toTimeT(t0), Desc(0, true, ""));
                ASSERT(true == Obj::isWellFormed(X));

                mX.addTransition(toTimeT(t1),
                                 Desc(SIX_HOURS_IN_S, true, ""));
                ASSERT(true == Obj::isWellFormed(X));

                // Add and overlapping range
                mX.addTransition(toTimeT(t1Plus1Hour),
                                 Desc(-SIX_HOURS_IN_S, true, ""));
                ASSERT(false == Obj::isWellFormed(X));

                mX.addTransition(toTimeT(t2), Desc(0, true, ""));
                ASSERT(false == Obj::isWellFormed(X));

            }
        }
        ASSERT(0 == defaultAllocator.numBytesInUse());
       } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
