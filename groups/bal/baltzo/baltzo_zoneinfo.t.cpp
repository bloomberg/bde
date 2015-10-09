// baltzo_zoneinfo.t.cpp                                              -*-C++-*-
#include <baltzo_zoneinfo.h>

#include <baltzo_localtimedescriptor.h>

#include <bdlt_epochutil.h>

#include <bdlt_datetimetz.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

#undef DS

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This component under test provides two classes that represent time zones.
// The 'baltzo::ZoneinfoTransition' is an unconstrained in-core
// (value-semantic) class.  The 'class' supports a subset of the value-semantic
// functionality: basic accessors, equality operators, and 'operator<'.
//
// This particular attribute class does not provide a public default/value
// constructor capable of creating an object in any state relevant for testing.
// The only way to create a transition object is by calling 'addTransition'
// with the attribute values required for the transition and accessing the
// transition via the 'firstTransition' or 'beginTransitions' methods.  We will
// implement a 'gg' function for creating a zone info having any value.  We
// will then use this generator function to create a single transition and
// test its various properties.
//
// We will then follow a subset of our standard 10-case approach to testing
// value-semantic types.
//
// The 'baltzo::Zoneinfo' is a value semantic type containing, an 'identifier'
// string, a sequence of transitions (specified by
// 'baltzo::ZoneinfoTransition'), and the 'baltzo::LocalTimeDescriptor'
// corresponding to the contained transitions.  The 'class' supports a subset
// of the value-semantic functionality: default constructor, copy constructor,
// assignment operator, swap, and basic accessors.
//
// This particular attribute class provides a default constructor and
// 'addTransition' manipulator capable of creating an object in any state
// relevant for testing.  We will implement a 'gg' function for creating a zone
// info having any value.  We will then use the accessors to verify the state.
//-----------------------------------------------------------------------------
//
//                       // -------------------------
//                       // baltzo::ZoneinfoTransition
//                       // -------------------------
//
// ACCESSORS
// [ 4] const baltzo::LocalTimeDescriptor& descriptor() const;
// [ 4] bdlt::EpochUtil::TimeT64 utcTime() const;
//
// [ 6] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 5] bool operator==(const baltzo::ZoneinfoTransition& lhs, rhs);
// [ 5] bool operator!=(const baltzo::ZoneinfoTransition& lhs, rhs);
// [ 7] bool operator<(const baltzo::ZoneinfoTransition& lhs, rhs);
// [ 6] bsl::ostream& operator<<(ostream& s, const ZoneinfoTransition& d);
// ----------------------------------------------------------------------------
//
//                       // ---------------
//                       // baltzo::Zoneinfo
//                       // ---------------
//
// CREATORS
// [ 2] explicit baltzo::Zoneinfo(bslma::Allocator *basicAllocator = 0);
// [11] baltzo::Zoneinfo(const baltzo::Zoneinfo&  original, *bA);
//
// MANIPULATORS
// [13] baltzo::Zoneinfo& operator=(const baltzo::Zoneinfo& rhs);
// [ 2] void addTransition(TimeT64 time, const baltzo::LTD& d);
// [ 9] void setIdentifier(const bslstl::StringRef& identifier);
// [12] void swap(baltzo::Zoneinfo& other);

// ACCESSORS
// [ 4] bslma::Allocator *allocator() const;
// [14] TransitionConstIterator findTransitionForUtcTime(utcTime) const;
// [ 4] const Transition& firstTransition() const;
// [ 9] const bsl::string& identifier() const;
// [ 4] bsl::size_t numTransitions() const;
// [ 4] TransitionConstIterator beginTransitions() const;
// [ 4] TransitionConstIterator endTransitions() const;
// [10] bsl::ostream& print(ostream& stream, level, spl) const;
//
// FREE OPERATORS
// [ 8] bool operator==(const baltzo::Zoneinfo& lhs, rhs);
// [ 8] bool operator!=(const baltzo::Zoneinfo& lhs, rhs);
// [10] bsl::ostream& operator<<(ostream& stream, const baltzo::Zoneinfo&);
//
// FREE FUNCTIONS
// [12] void swap(baltzo::Zoneinfo& first, baltzo::Zoneinfo& second);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST: 'baltzo::Zoneinfo', 'baltzo::ZoneinfoTransition'
// [15] USAGE EXAMPLE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                   STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }

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
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl;
                                              // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;
                                              // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"
#define T_ cout << "\t" << flush;             // Print tab w/o newline
#define L_ __LINE__                           // current Line number

#ifdef BSLS_PLATFORM_CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                 "123456789012345678901234567890123"
#endif

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef baltzo::Zoneinfo              Obj;
typedef baltzo::ZoneinfoTransition    Transition;
typedef Obj::TransitionConstIterator TransitionConstIter;

typedef baltzo::LocalTimeDescriptor   Descriptor;


typedef bdlt::EpochUtil::TimeT64         TimeT64;

typedef map<TimeT64, Descriptor>         my_TransitionMap;
typedef my_TransitionMap::const_iterator my_TransitionMapConstIter;
typedef pair<TimeT64, Descriptor>        my_Transition;



// Attribute values for the local time descriptor

// 'D' values: These are the default constructed values.

const int  D1   = 0;        // 'utcOffsetInSeconds'
const bool D2   = false;    // 'dstInEffectFlag'
const char D3[] = "";       // 'description'

// 'A' values: Should cause memory allocation if possible.

const int  A1   = -24 * 60 * 60 + 1;
const bool A2   = true;
const char A3[] = "a_" SUFFICIENTLY_LONG_STRING;

// 'B' values: Should NOT cause allocation (use alternate string type).

const int    B1 =  24 * 60 * 60 - 1;
const bool   B2 = false;
const string B3 = "EST";

static const TimeT64 TIMES[] = {
    0,
    1000,
    2000
};

static const Descriptor DESCRIPTORS[] = {
    Descriptor(D1, D2, D3),
    Descriptor(A1, A2, A3),
    Descriptor(B1, B2, B3)
};

// ============================================================================
//              GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

bsl::string descriptorPrintOutput(const Descriptor& D, int L, int SPL)
{
    ostringstream os;
    D.print(os, L, SPL);
    return os.str();
}

bsl::string descriptorOperatorOutput(const Descriptor& D)
{
    ostringstream os;
    os << D;
    return os.str();
}

bsl::string transitionPrintOutput(const Transition& T, int L, int SPL)
{
    ostringstream os;
    T.print(os, L, SPL);
    return os.str();
}

bsl::string transitionOperatorOutput(const Transition& T)
{
    ostringstream os;
    os << T;
    return os.str();
}


//=============================================================================
//            GENERATOR FUNCTIONS 'g', 'gg' and 'ggg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the zone info object according to a custom language.
// The given 'spec' contains the data that will be parsed and called on the
// primary manipulator ('addTransition') to bring the object into any state
// suitable for testing.
//
// LANGUAGE SPECIFICATION
// ----------------------
//
// <SPEC>       ::= <EMPTY> | <LIST>
//
// <EMPTY>      ::=
//
// <LIST>       ::= <ELEMENT> | <ELEMENT><LIST>
//
// <ELEMENT>    ::= <TRANSITION>
//
// <TRANSITION> ::= <TIME><DESCRIPTOR>
//
// <TIME>       ::= 'n' | 'x' | 'y'
//
// <DESCRIPTOR> ::= 'D' | 'A' | 'B'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object empty.
// TBD: Complete
// ----------------------------------------------------------------------------

void loadTime(TimeT64 *time, char spec)
{
    LOOP_ASSERT(spec, strchr("nxy", spec));

    const int index = 'n' == spec ? 0
                    : 'x' == spec ? 1
                    :               2;

    *time = TIMES[index];
}

void loadDescriptor(Descriptor *d, char spec)
{
    LOOP_ASSERT(spec, strchr("DAB", spec));

    const int index = 'D' == spec ? 0
                    : 'A' == spec ? 1
                    :               2;

    *d = DESCRIPTORS[index];
}

int ggg(Obj *object, const char *spec)
    // Configure the specified 'object' according to the specified 'spec' using
    // the primary manipulator function.  Return the index of the first
    // invalid character, and a negative value otherwise.  Note that this
    // function is used to implement 'gg' as well as allow for verification of
    // syntax error detection.
{
    enum { SUCCESS = -1 };

    enum { MAX_SPEC_LEN = 200 };

    ASSERT(bsl::strlen(spec) < MAX_SPEC_LEN);

    char cleanSpec[MAX_SPEC_LEN];  // spec after removing all white spaces

    int i = 0;
    for (int j = 0; spec[j]; ++j) {
        if (!isspace(spec[j])) {
            cleanSpec[i++] = spec[j];
        }
    }
    cleanSpec[i] = '\0';

    const char *p = cleanSpec;

    while (*p) {
        TimeT64 t;
        loadTime(&t, *p);
        ++p;
        bslma::TestAllocator ta;
        Descriptor d(&ta);
        loadDescriptor(&d, *p);
        ++p;
        object->addTransition(t, d);
    }

    return SUCCESS;
}

Obj& gg(Obj *object, const char *spec)
    // Return, by reference, the specified 'object' with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

Obj g(const char *spec)
    // Return, by value, a new object corresponding to the specified 'spec'.
{
    Obj object;
    return gg(&object, spec);
}

//=============================================================================
//                                USAGE
//-----------------------------------------------------------------------------
// First we declare the function 'utcToLocalTime' and its contract:
//..
    static bdlt::DatetimeTz utcToLocalTime(const bdlt::Datetime&   utcTime,
                                           const baltzo::Zoneinfo& timeZone)
    {
        // Return the 'bdlt::DatetimeTz' value representing the date, time and
        // offset from UTC (rounded to the minute) value of the local time,
        // corresponding to the specified 'utcTime' in the specified
        // 'timeZone'.  The behavior is undefined if the 'utcTime' precedes the
        // time of the first transition contained in 'timeZone' and
        // '0 < timeZone.numTransitions()'.
//..
// Then, we check the precondition of 'utcToLocalTime', by checking that
// 'timeZone' contains at least one transitions and comparing 'utcTime' to the
// first transition time in 'timeZone':
//..
        BSLS_ASSERT(0 < timeZone.numTransitions());
        BSLS_ASSERT(timeZone.firstTransition().utcTime() <=
                                   bdlt::EpochUtil::convertToTimeT64(utcTime));
//..
// Next, we obtain the appropriate 'baltzo::ZoneinfoTransition' object,
// invoking the method 'findTransitionForUtcTime' on 'timeZone':
//..
        baltzo::Zoneinfo::TransitionConstIterator it =
                                    timeZone.findTransitionForUtcTime(utcTime);
//..
// Then, we access the descriptor associated with the transition to which 'it'
// refers, and calculate the offset from UTC rounded to the minute:
// ..
        const baltzo::ZoneinfoTransition& transition = *it;
        const int offsetInMinutes =
                             transition.descriptor().utcOffsetInSeconds() / 60;
//..
// Now, we apply the obtained 'offsetInMinutes' to the originally specified
// 'utcTime' obtaining the corresponding local time in the specified
// 'timeZone':
//..
        bdlt::Datetime temp(utcTime);
        temp.addMinutes(offsetInMinutes);
//..
// Finally, return the local time value together with its offset from UTC:
//..
        return bdlt::DatetimeTz(temp, offsetInMinutes);
    }
//..

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 17: {
        // --------------------------------------------------------------------
        // 'baltzo::Zoneinfo' 'convertFromTimeT64'
        //
        // Concerns:
        //: 1 That 'convertFromTimeT64' produces the same results as the
        //:   version in 'EpochUtil'.
        //:
        //
        // Plan:
        //: 1 Call 'convertFromTimeT64' and compare the results against
        //:   'bdlt::EpochUtil::convertFromTimeT64'
        //
        // Testing:
        //   int convertFromTimeT64(bdlt::Datetime *, TimeT64);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'baltzo::Zoneinfo' 'convertFromTimeT64"
                          << endl
                          << "==================================="
                          << endl;


        static const struct {
            int              d_line;           // source line number
            bdlt::EpochUtil::TimeT64 d_timet;
        } DATA [] = {
            // LINE
            { L_, -62135596801LL },
            { L_, -62135596800LL },
            { L_, -62135769601LL },
            { L_, -62135769600LL },
            { L_, -60000000000LL },
            { L_,            0LL },
            { L_,       123140LL },
            { L_, 253402300798LL },
            { L_, 253402300799LL },
            { L_, 253402300800LL },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE = DATA[ti].d_line;
            TimeT64 input  = DATA[ti].d_timet;

            bdlt::Datetime expected, result;
            int            expectedRc, resultRc;

            expectedRc = bdlt::EpochUtil::convertFromTimeT64(&expected, input);
            resultRc   = Obj::convertFromTimeT64(&result, input);

            LOOP3_ASSERT(LINE, expectedRc, resultRc, expectedRc == resultRc);
            if (0 == expectedRc) {
                LOOP3_ASSERT(LINE, expected, result, expected == result);
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // 'baltzo::Zoneinfo' 'convertToTimeT64'
        //
        // Concerns:
        //: 1 That 'convertToTimeT64' produces the same results as the version
        //:   in 'EpochUtil'.
        //:
        //
        // Plan:
        //: 1 Call 'convertToTimeT64' and compare the results against
        //:   'bdlt::EpochUtil::convertToTimeT64'
        //
        // Testing:
        //   bdlt::EpochUtil::TimeT64 convertToTimeT64(const bdlt::Datetime& );
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'baltzo::Zoneinfo' 'convertToTimeT64"
                          << endl
                          << "==================================="
                          << endl;


        static const struct {
            int  d_line;           // source line number
            int  d_Y;
            int  d_M;
            int  d_D;
            int  d_h;
            int  d_m;
            int  d_s;
            int  d_ms;
        } DATA [] = {
          // LINE
            { L_, 0001, 01, 01, 00, 00, 00, 000 },
            { L_, 0001, 01, 01, 00, 00, 00, 001 },
            { L_, 0001, 01, 01, 00, 00, 00, 999 },
            { L_, 0001, 01, 01, 01, 01, 01, 000 },
            { L_, 1000, 01, 01, 01, 01, 01, 000 },
            { L_, 1969, 12, 31, 23, 59, 59, 000 },
            { L_, 1969, 12, 31, 23, 59, 59, 999 },
            { L_, 1970, 01, 01, 00, 00, 00, 000 },
            { L_, 1970, 01, 01, 00, 00, 01, 000 },
            { L_, 3000, 01, 01, 00, 00, 01, 000 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE   = DATA[ti].d_line;
            const bdlt::Datetime input(DATA[ti].d_Y,
                                      DATA[ti].d_M,
                                      DATA[ti].d_D,
                                      DATA[ti].d_h,
                                      DATA[ti].d_m,
                                      DATA[ti].d_s,
                                      DATA[ti].d_ms);

            TimeT64 EXPECTED = bdlt::EpochUtil::convertToTimeT64(input);
            TimeT64 result   = Obj::convertToTimeT64(input);

            LOOP3_ASSERT(LINE, EXPECTED, result, EXPECTED == result);
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

///Example 1: Populate a 'baltzo::Zoneinfo'
///- - - - - - - - - - - - - - - - - - - -
// Suppose we want to represent the time-zone information for New York, in 2010
// using a 'baltzo::Zoneinfo' object.  In order to do so,  we need to provide
// the UTC date-times (transitions) after which the time zone changes its
// offset from UTC, or daylight-saving Time starts or ends.
//
// First, we define two times "Mar 14, 2010 6:00 AM" and "Nov 07, 2010 7:00 AM"
// representing respectively the UTC time at which New York transitions to
// Eastern Daylight-saving Time (EDT) and Eastern Standard Time (EST) in 2010:
//..
    bdlt::Datetime edtDatetime(2010, 03, 14, 2, 0);
    bdlt::Datetime estDatetime(2010, 11, 07, 2, 0);
//..
// Then, we create two local-time descriptors that hold the offset from UTC and
// DST status for EST and EDT in New York in 2010, in terms of their
// 'offsetFromUtcInSeconds', 'dstInEffectFlag' and 'description' attributes:
//..
    const baltzo::LocalTimeDescriptor est(-5 * 60 * 60, false, "EST");
    const baltzo::LocalTimeDescriptor edt(-4 * 60 * 60, true,  "EDT");
//..
// Note that these descriptors will be associated with the created transition
// times, to reflect how local time in New York changes its offset from UTC and
// DST status after specific times.
//
// Next, we create an empty 'baltzo::Zoneinfo' object that will be populated
// with the information necessary to describe the time zone for New York:
//..
    baltzo::Zoneinfo newYorkTimeZone;
//..
// Then, before being able to associate the transition times with their
// corresponding descriptors, we need translate the transition times to
// 'bdlt::EpochUtil::TimeT64':
//..
    bdlt::EpochUtil::TimeT64 edtTransitionTime =
                                bdlt::EpochUtil::convertToTimeT64(edtDatetime);
    bdlt::EpochUtil::TimeT64 estTransitionTime =
                                bdlt::EpochUtil::convertToTimeT64(estDatetime);
//..
// Now, we associate the created descriptors with the transitions we indicated
// previously and add them to 'newYorkTimeZone' using the 'addTransition'
// method:
//..
    newYorkTimeZone.addTransition(edtTransitionTime, edt);
    newYorkTimeZone.addTransition(estTransitionTime, est);
//..
// Note that this insertion operation maintains the transitions in order of
// transition time, and therefore inserting transitions out-of-order, while not
// illegal, is very inefficient.
//
// Finally we verify that the 'newYorkTimeZone' contains the transitions we
// indicated:
//..
    {
        baltzo::Zoneinfo::TransitionConstIterator it =
                                            newYorkTimeZone.beginTransitions();

        ASSERT(it->utcTime()    == edtTransitionTime);
        ASSERT(it->descriptor() == edt);
        ++it;
        ASSERT(it->utcTime()    == estTransitionTime);
        ASSERT(it->descriptor() == est);
    }
//..
// Notice that the iterator refers to a 'baltzo::ZoneinfoTransition' object.
//
///Example 2: Converting UTC to Local Time
///- - - - - - - - - - - - - - - - - - - -
// Suppose that we want to program a function, 'utcToLocalTime' to perform the
// conversion from a UTC time value to the corresponding local time value, in a
// given time zone, and return the computed local time:
//
// Suppose, now, we want to convert UTC time to the corresponding local time in
// New York.  We can do so using the previously defined function
// 'utcToLocalTime' and reusing the 'baltzo::Zoneinfo' object,
// 'newYorkTimeZone' of Example 1.
//
// First, we define 'bdlt::Datetime' object representing the UTC time "Apr 10,
// 2010 12:00":
//..
    bdlt::Datetime utcDatetime(2010, 04, 10, 12, 0, 0);
//..
// Then, we invoke 'utcToLocalTime' passing 'newYorkTimeZone' as a time zone
// and save the result:
//..
    bdlt::DatetimeTz nyDatetime = utcToLocalTime(utcDatetime, newYorkTimeZone);
//..
// Finally, we compute the New York local time corresponding to 'utcDatetime',
// verify that "April 10, 2010 8:00" is the computed time:
//..
    const bdlt::Datetime expectedTime(2010, 4, 10, 8, 0, 0);
    ASSERT(-4 * 60 == nyDatetime.offset());
    ASSERT(expectedTime == nyDatetime.localDatetime());
//..
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // 'baltzo::Zoneinfo' 'findTransitionForUtcTime'
        //   Ensure that the correct transition is returned for specified time.
        //
        // Concerns:
        //: 1 The function correctly returns the transition corresponding to
        //:   the supplied utc time.
        //:
        //: 2 There is no allocation from any allocator.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 2 Using the table-driven technique:
        //:
        //:   1 Specify a set of widely varying object values (one per row).
        //:
        //:   2 For each object value also specify a 'utcTime' and expected
        //:     'index' for that 'utcTime'.
        //:
        //: 3 For each row 'R1' in the table of P-3:  (C-1..3)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the generator function to create a modifiable 'Obj', 'mX',
        //:     using 'oa' and having the value described by 'R1'; also create
        //:     a 'bdlt::Datetime' corresponding to the 'utcTime' in that row.
        //:
        //:   3 Verify that the returned transition is as expected.  (C-1)
        //:
        //:   4 Verify that no memory is allocated from any allocator.  (C-2)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to call this function when the
        //:   object contains no transitions or the 'utcTime' is less than the
        //:   'utcTime' of the first transition (using the 'BSLS_ASSERTTEST_*'
        //:   macros).  (C-3)
        //
        // Testing:
        //   TransitionConstIterator findTransitionForUtcTime(utcTime) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'baltzo::Zoneinfo' 'findTransitionForUtcTime"
                          << endl
                          << "==========================================="
                          << endl;

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&da);

        static const struct {
            int         d_line;           // source line number
            const char *d_spec;
            TimeT64     d_utcTime;
            int         d_expIndex;
        } DATA [] = {
          // LINE   SPEC        UTC    INDEX
          // ----   ----        ---    -----

            { L_,   "nD",         1,     0      },
            { L_,   "xA",      1000,     0      },
            { L_,   "yB",      2001,     0      },

            { L_,   "nDxA",       0,     0      },
            { L_,   "nDxA",       1,     0      },
            { L_,   "nDxA",    1000,     1      },
            { L_,   "nDxA",    1001,     1      },

            { L_,   "nDxAyB",     0,     0      },
            { L_,   "nDxAyB",     1,     0      },
            { L_,   "nDxAyB",  1000,     1      },
            { L_,   "nDxAyB",  1001,     1      },
            { L_,   "nDxAyB",  2000,     2      },
            { L_,   "nDxAyB",  2001,     2      },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int            LINE   = DATA[ti].d_line;
            const char *const    SPEC   = DATA[ti].d_spec;
            const TimeT64        UTC    = DATA[ti].d_utcTime;
            const int            INDEX  = DATA[ti].d_expIndex;
            const bdlt::Datetime DT     =
                                      bdlt::EpochUtil::convertFromTimeT64(UTC);

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            bslma::TestAllocatorMonitor oam(&oa);

            Obj::TransitionConstIterator iter = X.findTransitionForUtcTime(DT);

            LOOP2_ASSERT(LINE, INDEX,
                        INDEX == (iter - X.beginTransitions()));
            LOOP_ASSERT(LINE, oam.isInUseSame());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\tWith no transitions" << endl;
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const bdlt::Datetime DT =
                                        bdlt::EpochUtil::convertFromTimeT64(1);

                Obj mX(&oa);  const Obj& X = gg(&mX, "nD");
                Obj mY(&oa);  const Obj& Y = mY;

                ASSERT_SAFE_PASS(X.findTransitionForUtcTime(DT));
                ASSERT_SAFE_FAIL(Y.findTransitionForUtcTime(DT));
            }

            if (veryVerbose) cout << "\tWith no transitions before input"
                                  << endl;
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const bdlt::Datetime DT =
                                        bdlt::EpochUtil::convertFromTimeT64(1);

                Obj mX(&oa);  const Obj& X = gg(&mX, "nD");
                Obj mY(&oa);  const Obj& Y = gg(&mY, "xA");

                ASSERT_SAFE_PASS(X.findTransitionForUtcTime(DT));
                ASSERT_SAFE_FAIL(Y.findTransitionForUtcTime(DT));
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // COPY ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any target object
        //:   to that of any source object.
        //;
        //: 2 The address of the target object's allocator is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 The value of the source object is not modified.
        //:
        //: 7 The allocator of the source object is not modified.
        //:
        //: 8 Any memory allocation is exception neutral.
        //:
        //: 9 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a function pointer
        //:   having the appropriate structure for the copy assignment operator
        //:   defined in this component.  (C-4)
        //:
        //: 2 Construct a 'bslma::TestAllocator' object and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of valid object values (one per row) in terms of
        //:     their individual attributes, including (a) first, the default
        //:     value, (b) boundary values corresponding to every range of
        //:     values that each individual attribute can independently attain,
        //:     and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..3, 5..9)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V' and using a "scratch"
        //:     allocator.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):
        //:
        //:     1 Construct a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Use the value constructor to create a modifiable 'Obj',
        //:       'mX', using 'oa' and having the value 'W'.
        //:
        //:     3 Use the equality comparison operators to verify that the two
        //:       objects, 'mX' and 'Z', are initially equal if and only if the
        //:       table indices from P-4 and P-4.2 are the same.
        //:
        //:     4 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:
        //:     5 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     6 Use the equality comparison operators to verify that:
        //:       (C-1, 6)
        //:
        //:       1 The target object, 'mX', now has the same value as 'Z'.
        //:
        //:       2 'Z' still has the same value as 'ZZ'.
        //:
        //:     7 Use the 'allocator' accessor of both 'mX' and 'Z' to verify
        //:       that both object allocators are unchanged.  (C-2, 7)
        //:
        //:     8 Use appropriate test allocators to verify that:  (C-3, 8)
        //:
        //:       1 No additional memory is allocated by the source object.
        //:
        //:       2 All object memory is released when the object is destroyed.
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3:  (C-1..3, 5..10)
        //:
        //:   1 Construct a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor to create a modifiable 'Obj' 'mX'
        //:     using 'oa' and a 'const' 'Obj' 'ZZ' (using a distinct "scratch"
        //:     allocator).
        //:
        //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   4 Use the equality comparison operators to verify that the target
        //:     object, 'mX', has the same value as 'ZZ'.
        //:
        //:   5 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:     (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:     (C-9)
        //:
        //:   6 Verify that the address of the return value is the same as that
        //:     of 'mX'.  (C-5)
        //:
        //:   7 Use the equality comparison operators to verify that the
        //:     target object, 'mX', still has the same value as 'ZZ'.  (C-10)
        //:
        //:   8 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.  (C-2)
        //:
        //:   9 Use appropriate test allocators to verify that:  (C-3, 8)
        //:
        //:     1 Any memory that is allocated is from the object allocator.
        //:
        //:     2 No additional (e.g., temporary) object memory is allocated
        //:       when assigning an object value that did NOT initially require
        //:       allocated memory.
        //:
        //:     3 All object memory is released when the object is destroyed.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   operator=(const baltzo::Zoneinfo& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            // Quash unused variable warning.

            (void)operatorAssignment;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&da);

        if (verbose) cout <<
        "\nCreate a table of distinct object values and expected memory usage."
                                                                       << endl;

        static const struct {
            int         d_line;           // source line number
            const char *d_spec;
        } DATA[] = {

          // LINE  SPEC
          // ----  ----

            { L_,   ""        },

            { L_,   "nD"      },
            { L_,   "xA"      },
            { L_,   "yB"      },

            { L_,   "nDxA"    },
            { L_,   "nAxD"    },

            { L_,   "nAxDyA"  },
            { L_,   "yAxDnD"  },

            // duplicates

            { L_,   "nDnA"    },
            { L_,   "xDxD"    },

            { L_,   "xDxAxB"  },
            { L_,   "yBnAyA"  },

            { L_,   "xDnAyBxAnByD"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char *const SPEC1   = DATA[ti].d_spec;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZ(&scratch);   const Obj&  Z = gg(&mZ,  SPEC1);
            Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC1);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P_(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1, Z, Obj(&scratch), Z == Obj(&scratch));
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const SPEC2   = DATA[tj].d_spec;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(&oa);  const Obj& X = gg(&mX, SPEC2);

                    if (veryVerbose) { T_ P_(LINE2) P_(X) }

                    LOOP4_ASSERT(LINE1, LINE2, X, Z,
                                 (X == Z) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    Obj *mR = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        mR = &(mX = Z);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    // Verify the address of the return value.

                    LOOP4_ASSERT(LINE1, LINE2, mR, &mX, mR == &mX);

                    LOOP4_ASSERT(LINE1, LINE2,  X, Z,  X == Z);
                    LOOP4_ASSERT(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    LOOP4_ASSERT(LINE1, LINE2, &oa, X.allocator(),
                                 &oa == X.allocator());
                    LOOP4_ASSERT(LINE1, LINE2, &scratch, Z.allocator(),
                                 &scratch == Z.allocator());

                    LOOP2_ASSERT(LINE1, LINE2, sam.isInUseSame());

                    LOOP2_ASSERT(LINE1, LINE2, !da.numBlocksTotal());


                }

                // Verify all memory is released on object destruction.

                LOOP3_ASSERT(LINE1, LINE2, da.numBlocksInUse(),
                             0 == da.numBlocksInUse());
                LOOP3_ASSERT(LINE1, LINE2, oa.numBlocksInUse(),
                             0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&oa);   const Obj&  X = gg(&mX, SPEC1);
                Obj mZZ(&oa);  const Obj& ZZ = gg(&mX, SPEC1);

                const Obj& Z = mX;

                LOOP3_ASSERT(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                Obj *mR = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    mR = &(mX = Z);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                // Verify the address of the return value.

                LOOP3_ASSERT(LINE1, mR, &mX, mR == &mX);

                LOOP3_ASSERT(LINE1, mR, &X, mR == &X);

                LOOP3_ASSERT(LINE1, Z, ZZ, ZZ == Z);

                LOOP3_ASSERT(LINE1, &oa, Z.allocator(), &oa == Z.allocator());

                if (0 == ti) {  // Empty, no allocation.
                    LOOP_ASSERT(LINE1, oam.isInUseSame());
                }

                LOOP_ASSERT(LINE1, sam.isInUseSame());

                LOOP_ASSERT(LINE1, !da.numBlocksTotal());
            }

            // Verify all memory is released on object destruction.

            LOOP2_ASSERT(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
            LOOP2_ASSERT(LINE1, da.numBlocksInUse(), 0 == da.numBlocksInUse());
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that, when member and free 'swap' are implemented, we can
        //   exchange the values of any two objects that use the same
        //   allocator.
        //
        // Concerns:
        //: 1 Both functions exchange the values of the (two) supplied objects.
        //:
        //: 2 The common object allocator address in both objects is unchanged.
        //:
        //: 3 Neither function allocates memory from any allocator.
        //:
        //: 4 Both functions have standard signatures.
        //:
        //: 5 Using either function to swap an object with itself does not
        //:   affect the value of the object (alias-safety).
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the addresses of the 'swap' member and free functions defined
        //:   in this component to initialize, respectively, pointers to member
        //:   and free functions having the appropriate structures.  (C-4)
        //:
        //: 2 Construct a 'bslma::TestAllocator' object and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of widely varying object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-1..3, 5)
        //:
        //:   1 Construct a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor to create a modifiable 'Obj', 'mW',
        //:     using 'oa' and having the value described by 'R1'; also use the
        //:     copy constructor to create a 'const' 'Obj' 'XX' (using a
        //:     "scratch" allocator) from 'mW'.
        //:
        //:   3 Use the member and free 'swap' functions to swap the value of
        //:     'mW' with itself, and then verify:  (C-2, 3, 5)
        //:
        //:     1 The value is unchanged.
        //:
        //:     2 The object allocator address is unchanged.
        //:
        //:     3 There was no additional object memory allocation.
        //:
        //:   4 For each row 'R2' in the table of P-3:
        //:
        //:     1 Use the copy constructor to create a modifiable 'Obj', 'mX',
        //:       (using 'oa') from 'XX' (P-4.2).
        //:
        //:     2 Use the value constructor to create a modifiable 'Obj', 'mY',
        //:       using 'oa' and having the value described by 'R2'; also use
        //:       the copy constructor to create a 'const' 'Obj' 'YY' (using a
        //:       "scratch" allocator) from 'Y'.
        //:
        //:     3 Use the member 'swap' function to swap the values of 'mX'
        //:       and 'mY', and then verify:  (C-1..3)
        //:
        //:       1 The values have been exchanged.
        //:
        //:       2 The object allocator addresses are unchanged.
        //:
        //:       3 There was no additional object memory allocation.
        //:
        //:     4 Use the free 'swap' function to again swap the values of 'mX'
        //:       and 'mY', and then verify:  (C-1..3)
        //:
        //:       1 The values have been exchanged.
        //:
        //:       2 The object allocator addresses are unchanged.
        //:
        //:       3 There was no additional object memory allocation.
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to swap objects that do not
        //:   refer to the same allocator, but not when the allocators are the
        //:   same (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   void swap(baltzo::Zoneinfo& other);
        //   void swap(baltzo::Zoneinfo& a, b);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTIONS" << endl
                          << "==============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each function to a variable." << endl;
        {
            typedef void (Obj::*funcPtr)(Obj&);
            typedef void (*freeFuncPtr)(Obj&, Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     memberSwap = &Obj::swap;
            freeFuncPtr freeSwap   = swap;

            // Quash unused variable warnings.

            (void)memberSwap; (void)freeSwap;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&da);

        if (verbose) cout << "\nCreate a table of distinct object values."
                                                                       << endl;

        static const struct {
            int         d_line;           // source line number
            const char *d_spec;
        } DATA[] = {

          // LINE  SPEC
          // ----  ----

            { L_,   ""        },

            { L_,   "nD"      },
            { L_,   "xA"      },
            { L_,   "yB"      },

            { L_,   "nDxA"    },
            { L_,   "nAxD"    },

            { L_,   "nAxDyA"  },
            { L_,   "yAxDnD"  },

            // duplicates

            { L_,   "nDnA"    },
            { L_,   "xDxD"    },

            { L_,   "xDxAxB"  },
            { L_,   "yBnAyA"  },

            { L_,   "xDnAyBxAnByD"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char *const SPEC1   = DATA[ti].d_spec;

            bslma::TestAllocator oa("object",       veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mW(&oa);  const Obj& W = gg(&mW, SPEC1);
            const Obj XX(W, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(W) P_(XX) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1, W, Obj(&scratch), W == Obj(&scratch));
                firstFlag = false;
            }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mW.swap(mW);

                LOOP3_ASSERT(LINE1, W, XX, W == XX);
                LOOP_ASSERT(LINE1, &oa == W.allocator());
                LOOP_ASSERT(LINE1, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mW, mW);

                LOOP3_ASSERT(LINE1, W, XX, W == XX);
                LOOP_ASSERT(LINE1, &oa == W.allocator());
                LOOP_ASSERT(LINE1, oam.isTotalSame());
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const SPEC2   = DATA[tj].d_spec;

                Obj mX(XX, &oa);  const Obj& X = mX;

                Obj mY(&oa);  const Obj& Y = gg(&mY, SPEC2);
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P_(YY) }

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    mX.swap(mY);

                    LOOP4_ASSERT(LINE1, LINE2, X, YY, X == YY);
                    LOOP4_ASSERT(LINE1, LINE2, Y, XX, Y == XX);
                    LOOP2_ASSERT(LINE1, LINE2, &oa == X.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, &oa == Y.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, oam.isTotalSame());
                }

                // free function 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    swap(mX, mY);

                    LOOP4_ASSERT(LINE1, LINE2, X, XX, X == XX);
                    LOOP4_ASSERT(LINE1, LINE2, Y, YY, Y == YY);
                    LOOP2_ASSERT(LINE1, LINE2, &oa == X.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, &oa == Y.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, oam.isTotalSame());
                }
            }
        }

        // Verify no memory is allocated from the default allocator.

        LOOP_ASSERT(da.numBlocksTotal(), !da.numBlocksTotal());

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'swap' member function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);  Obj mB(&oa1);  Obj mC(&oa1);
                Obj mZ(&oa2);

                ASSERT_SAFE_PASS(mA.swap(mB));
                ASSERT_SAFE_FAIL(mC.swap(mZ));
            }

            if (veryVerbose) cout << "\t'swap' free function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);  Obj mB(&oa1);  Obj mC(&oa1);
                Obj mZ(&oa2);

                ASSERT_SAFE_PASS(swap(mA, mB));
                ASSERT_SAFE_FAIL(swap(mC, mZ));
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor (with or without a supplied allocator)
        //:   creates an object having the same value as the supplied original
        //:   object.
        //:
        //: 2 If an allocator is *not* supplied to the copy constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object (i.e., the
        //:   allocator of the original object is never copied).
        //:
        //: 3 If an allocator is supplied to the copy constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the copy constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator, which is
        //:   returned by the 'allocator' accessor method.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The original object is passed as a reference providing
        //:   non-modifiable access to the object.
        //:
        //:10 The value of the original object is unchanged.
        //:
        //:11 The allocator address of the original object is unchanged.
        //:
        //:12 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of widely varying object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..12)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V' and using a "scratch"
        //:     allocator.
        //:
        //:   2 Execute an inner loop creating three distinct objects in turn,
        //:     each using the copy constructor on 'Z' from P-2.1, but
        //:     configured differently: (a) without passing an allocator,
        //:     (b) passing a null allocator address explicitly, and (c)
        //:     passing the address of a test allocator distinct from the
        //:     default.
        //:
        //:   3 For each of these three iterations (P-2.2):  (C-1..12)
        //:
        //:     1 Construct three 'bslma::TestAllocator' objects and install
        //:       one as the current default allocator (note that a ubiquitous
        //:       test allocator is already installed as the global allocator).
        //:
        //:     2 Use the copy constructor to dynamically create an object 'X',
        //:       supplying it the 'const' object 'Z' (see P-2.1), configured
        //:       appropriately (see P-2.2) using a distinct test allocator
        //:       for the object's footprint.  (C-9)
        //:
        //:     3 Use the equality comparison operators to verify that:
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         'Z'.  (C-1, 5)
        //:
        //:       2 'Z' still has the same value as 'ZZ'.  (C-10)
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also apply the object's
        //:       'allocator' accessor, as well as that of 'Z'.  (C-6, 11)
        //:
        //:     5 Use appropriate test allocators to verify that:  (C-2..4,
        //:       7, 8, 12)
        //:
        //:       1 Memory is always allocated, and comes from the object
        //:         allocator only (irrespective of the specific number of
        //:         allocations or the total amount of memory allocated).
        //:         (C-2, 4)
        //:
        //:       2 If an allocator was supplied at construction (P-2.1c), the
        //:         current default allocator doesn't allocate any memory.
        //:         (C-3)
        //:
        //:       3 No temporary memory is allocated from the object allocator.
        //:         (C-7)
        //:
        //:       4 All object memory is released when the object is destroyed.
        //:         (C-8)
        //:
        //: 3 Create an object as an automatic variable in the presence of
        //:   injected exceptions (using the
        //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros) and verify that
        //:   no memory is leaked.  (C-12)
        //
        // Testing:
        //   baltzo::Zoneinfo(const baltzo::Zoneinfo& o, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout << "\nCreate a table of distinct object values."
                                                                       << endl;

        static const struct {
            int         d_line;           // source line number
            const char *d_spec;
        } DATA[] = {

          // LINE  SPEC
          // ----  ----

            { L_,   ""        },

            { L_,   "nD"      },
            { L_,   "xA"      },
            { L_,   "yB"      },

            { L_,   "nDxA"    },
            { L_,   "nAxD"    },

            { L_,   "nAxDyA"  },
            { L_,   "yAxDnD"  },

            // duplicates

            { L_,   "nDnA"    },
            { L_,   "xDxD"    },

            { L_,   "xDxAxB"  },
            { L_,   "yBnAyA"  },

            { L_,   "xDnAyBxAnByD"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout <<
          "\nConstruct objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char *const SPEC   = DATA[ti].d_spec;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                Obj mZ(&scratch);   const Obj& Z  = gg(&mZ, SPEC);
                Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::Default::setDefaultAllocatorRaw(&da);

                    Obj                 *objPtr;
                    bslma::TestAllocator *objAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        objPtr = new (fa) Obj(Z);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj(Z, 0);
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(Z, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        LOOP_ASSERT(CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    LOOP2_ASSERT(LINE, CONFIG,
                                 sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                    // Ensure the first row of the table contains the
                    // default-constructed value.

                    static bool firstFlag = true;
                    if (firstFlag) {
                        LOOP4_ASSERT(LINE, CONFIG, Obj(), *objPtr,
                                     Obj() == *objPtr)
                        firstFlag = false;
                    }

                    // Verify the value of the object.

                    LOOP4_ASSERT(LINE, CONFIG,  X, Z,  X == Z);

                    // Verify that the value of 'Z' has not changed.

                    LOOP4_ASSERT(LINE, CONFIG, ZZ, Z, ZZ == Z);

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    LOOP2_ASSERT(LINE, CONFIG,
                                 &oa == X.identifier().allocator());

                    // Also apply the object's 'allocator' accessor, as well as
                    // that of 'Z'.

                    LOOP4_ASSERT(LINE, CONFIG, &oa, X.allocator(),
                                 &oa == X.allocator());

                    LOOP4_ASSERT(LINE, CONFIG, &scratch, Z.allocator(),
                                 &scratch == Z.allocator());

                    // Verify no allocation from the non-object allocator.

                    LOOP3_ASSERT(LINE, CONFIG, noa.numBlocksTotal(),
                                 0 == noa.numBlocksTotal());

                    // Verify memory is always allocated (because of bsl::set)
                    // except when default constructed (SPEC == "").

                    if (0 == ti) {
                        LOOP3_ASSERT(LINE, CONFIG, oa.numBlocksInUse(),
                                     0 == oa.numBlocksInUse());
                    }
                    else {
                        LOOP3_ASSERT(LINE, CONFIG, oa.numBlocksInUse(),
                                     0 != oa.numBlocksInUse());
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    LOOP3_ASSERT(LINE, CONFIG, fa.numBlocksInUse(),
                                 0 == fa.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, da.numBlocksInUse(),
                                 0 == da.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, sa.numBlocksInUse(),
                                 0 == sa.numBlocksInUse());
                }  // end foreach configuration

            }  // end foreach row

            // Double check that at least some object memory got allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char *const SPEC   = DATA[ti].d_spec;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                Obj mZ(&scratch);  const Obj& Z = gg(&mZ, SPEC);

                if (veryVerbose) { T_ P_(LINE) P(Z) }

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                bslma::Default::setDefaultAllocatorRaw(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                   if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                   Obj obj(Z, &oa);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP2_ASSERT(LINE, da.numBlocksInUse(), !da.numBlocksInUse());
                LOOP2_ASSERT(LINE, oa.numBlocksInUse(), !oa.numBlocksInUse());
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // 'baltzo::Zoneinfo' 'setIdentifier' & 'identifier'
        //   Ensure that the 'identifier' attribute can be set and accessed
        //   correctly.
        //
        // Concerns:
        //: 1 The 'identifier' attribute can be set to represent any value.
        //:
        //: 2 The 'identifier' accessor returns the attribute value.
        //:
        //: 3 The string argument can be of type 'const char *' or
        //:   'const string&'
        //:
        //: 4 Any memory allocation is from the object allocator.
        //:
        //: 5 There is no temporary allocation from any allocator.
        //:
        //: 6 Each accessor method is declared 'const'.
        //:
        //: 7 No accessor allocates any memory.
        //:
        //: 8 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven approach:
        //:
        //:   1 Specify a set of widely varying 'identifier' values (one per
        //:     row), including (a) first, the default value, and (b) values
        //:     that should require allocation from each individual attribute
        //:     that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 2 For each row (representing a distinct attribute value, 'V') in
        //:   the table described in P-1:  (C-1..8)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Create two representations of the attribute value, one as a
        //:       'const char *' and the other as a 'string'.  (C-3)
        //:
        //:     3 In individual blocks default constructor an object by
        //:       specifying an object allocator.  Within each block, set the
        //:       attribute value to 'V' by passing a 'const char *' and a
        //:       'const string&' respectively.
        //:
        //:     4 Use the attribute accessors to verify the value.  (C-2)
        //:
        //:     5 Verify that no temporary memory is allocated from the object
        //:       allocator.  (C-5)
        //:
        //:     6 Monitor the memory allocated from both the default and object
        //:       allocators before and after calling the accessor; verify that
        //:       there is no change in total memory allocation.  (C-7)
        //:
        //:     7 Verify that the object manipulator and accessor work in the
        //:       presence of injected exceptions (using the
        //:       'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  (C-8)
        //
        // Testing:
        //   void setIdentifier(const bslstl::StringRef& identifier);
        //   const bsl::string& identifier() const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\n'baltzo::Zoneinfo' 'setIdentifier' & 'identifier'"
                 << "\n=================================================\n";

        if (verbose) cout <<
           "\nInstall an allocator as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&da);

        static const struct {
            int         d_line;           // source line number
            char        d_mem;            // expected allocation: 'Y', 'N', '?'
            const char *d_identifier;
        } DATA[] = {

        //LINE  MEM    IDENTIFIER
        //----  ---    ----------

        // default (must be first)
        { L_,   'N',   ""                                 },

        { L_,   '?',   "a"                                },
        { L_,   '?',   "AB"                               },
        { L_,   '?',   "1234567890"                       },

        { L_,   'Y',   SUFFICIENTLY_LONG_STRING           },
        { L_,   'Y',   "a_" SUFFICIENTLY_LONG_STRING      },
        { L_,   'Y',   "abc_" SUFFICIENTLY_LONG_STRING    },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE      = DATA[ti].d_line;
            const char        MEM       = DATA[ti].d_mem;
            const char *const ID_CSTR   = DATA[ti].d_identifier;
            string            ID_STRING(DATA[ti].d_identifier, &scratch);

            if (veryVerbose) { T_ P_(MEM) P(ID_STRING) }

            LOOP2_ASSERT(LINE, MEM, MEM && strchr("YN?", MEM));

            // using string literal
            {
                bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

                Obj mX(&oa);  const Obj& X = mX;

                bslma::TestAllocatorMonitor oam(&oa);

                mX.setIdentifier(ID_CSTR);
                LOOP3_ASSERT(LINE, ID_STRING, X.identifier(),
                            ID_STRING == X.identifier());

                if ('?' != MEM) {
                    LOOP2_ASSERT(LINE, MEM,
                                 ('N' == MEM) == oam.isInUseSame());
                }
            }

            // using bsl::string
            {
                bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

                Obj mX(&oa);  const Obj& X = mX;

                bslma::TestAllocatorMonitor oam(&oa);

                mX.setIdentifier(ID_STRING);
                LOOP3_ASSERT(LINE, ID_STRING, X.identifier(),
                            ID_STRING == X.identifier());

                if ('?' != MEM) {
                    LOOP2_ASSERT(LINE, MEM,
                                 ('N' == MEM) == oam.isInUseSame());
                }
            }

            // with injected exceptions
            {
                bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

                Obj mX(&oa);  const Obj& X = mX;

                bslma::TestAllocatorMonitor oam(&oa);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    mX.setIdentifier(ID_STRING);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP3_ASSERT(LINE, ID_STRING, X.identifier(),
                            ID_STRING == X.identifier());

                if ('?' != MEM) {
                    LOOP2_ASSERT(LINE, MEM,
                                 ('N' == MEM) == oam.isInUseSame());
                }

                anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();
            }
        }

        // Double check that at least some object memory got allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        LOOP_ASSERT(scratch.numBlocksInUse(), 0 == scratch.numBlocksInUse());

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // 'baltzo::Zoneinfo' EQUALITY OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compare
        //:   equal.
        //:
        //: 2 All salient attributes participate in the comparison.
        //:
        //: 3 'true  == (X == X)'  (i.e., aliasing)
        //:
        //: 4 'false == (X != X)'  (i.e., aliasing)
        //:
        //: 5 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 6 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if '!(X == Y)'
        //:
        //: 8 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 9 Non-modifiable objects can be compared (i.e., objects providing
        //:   only non-modifiable access).
        //:
        //:10 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //:
        //:11 The return type of the equality comparison operators is 'bool'.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate structure for
        //:   the two homogeneous, free equality comparison operators defined
        //:   in this component.  (C-8..9, 11)
        //:
        //: 2 Construct a 'bslma::TestAllocator' object and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their individual salient
        //:   attributes such that (a) for each salient attribute, there exists
        //:   a pair of rows that differ (slightly) in only the column
        //:   corresponding to that attribute, and (b) all attribute values
        //:   that can allocate memory on construction do so.
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-1..8)
        //:   1 Construct a single object and use it to verify the reflexive
        //:     (anti-reflexive) property of equality (inequality) in the
        //:     presence of aliasing.  (C-4..5)
        //:
        //:   2 For each row 'R2' in the table of P-3:  (C-1..3, 6..8)
        //:     1 Record, in 'EXP', whether or not distinct objects created,
        //:       from 'R1' and 'R2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 For each of two configurations, 'a' and 'b':  (C-1..3, 6..8)
        //:       1 Create two allocators 'pa' and 'qa'.
        //:
        //:       2 Create an object 'X' having the value 'R1' and using 'pa'.
        //:
        //:       3 Create an object 'Y' having the value 'R2' and using 'pa'
        //:         (in configuration 'a') or 'qa' (in configuration 'b').
        //:
        //:       4 Verify the commutativity property and expected return value
        //:         for both '==' and '!=', while monitoring both 'pa' and 'qa'
        //:         to ensure that no object memory is ever allocated by either
        //:         operator.  (C-1..3, 6..8)
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-11)
        //
        // Testing:
        //   bool operator==(const baltzo::Zoneinfo& lhs, rhs);
        //   bool operator!=(const baltzo::Zoneinfo& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'baltzo::Zoneinfo' EQUALITY OPERATORS" << endl
                          << "====================================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            using namespace baltzo;
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signature and return type is standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            // Quash unused-variable warnings.

            (void)operatorEq; (void)operatorNe;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&da);

        static const struct {
            int         d_lineNum;
            const char *d_spec_p;
        } DATA[] = {
            //line  spec
            //----  ----
            { L_,   ""        },

            { L_,   "nD"      },
            { L_,   "xA"      },
            { L_,   "yB"      },

            { L_,   "nDxA"    },
            { L_,   "nAxD"    },

            { L_,   "nAxDyA"  },
            { L_,   "yAxDnD"  },

            // duplicates

            { L_,   "nDnA"    },
            { L_,   "xDxD"    },

            { L_,   "xDxAxB"  },
            { L_,   "yBnAyA"  },

            { L_,   "xDnAyBxAnByD"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE1 = DATA[ti].d_lineNum;
            const char *const SPEC1 = DATA[ti].d_spec_p;

            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);

                Obj mX(&oa1);  const Obj& X = gg(&mX, SPEC1);  // original spec

                if (veryVerbose) { T_ P_(LINE1) P(X) }

                // Ensure an object compares correctly with itself (alias
                // test).

                LOOP2_ASSERT(LINE1, X,   X == X);
                LOOP2_ASSERT(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA ; ++tj) {
                const int         LINE2 = DATA[tj].d_lineNum;
                const char *const SPEC2 = DATA[tj].d_spec_p;

                const bool EXP = ti == tj;  // expected for equality comparison

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create two distinct test allocators 'pa' and 'qa'.

                    bslma::TestAllocator pa("p", veryVeryVeryVerbose);
                    bslma::TestAllocator qa("q", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = pa;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? pa : qa;

                    Obj mX(&xa);  const Obj& X = gg(&mX, SPEC1);
                    Obj mY(&ya);  const Obj& Y = gg(&mY, SPEC2);

                    if (veryVerbose) { T_ P_(LINE2) P_(EXP) P_(X) P_(Y) }

                    // Verify value, commutativity, and no memory allocation.

                    bslma::TestAllocatorMonitor pam(&pa), qam(&qa);

                    LOOP4_ASSERT(LINE1, LINE2, X, Y, EXP == (X == Y));
                    LOOP4_ASSERT(LINE1, LINE2, X, Y, EXP == (Y == X));

                    LOOP4_ASSERT(LINE1, LINE2, X, Y, !EXP == (X != Y));
                    LOOP4_ASSERT(LINE1, LINE2, X, Y, !EXP == (Y != X));

                    LOOP2_ASSERT(LINE1, LINE2, pam.isTotalSame());
                    LOOP2_ASSERT(LINE1, LINE2, qam.isTotalSame());

                    // Double check that some object memory got allocated,
                    // except in empty configurations.

                    if (0 != ti) {
                        LOOP4_ASSERT(LINE1, LINE2, CONFIG,xa.numBlocksInUse(),
                                    1 <= xa.numBlocksInUse());
                    }
                    if (0 != tj) {
                        LOOP4_ASSERT(LINE1, LINE2, CONFIG,ya.numBlocksInUse(),
                                    1 <= ya.numBlocksInUse());
                    }

                    // Note that memory should be independently allocated for
                    // each attribute capable of allocating memory.
                }
            }

            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // 'baltzo::Zoneinfo' PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)'
        //:   but with each "attributeName = " elided.
        //:
        //: 4 The 'print' method signature and return value is standard.
        //:
        //: 5 The output 'operator<<' signature and return value is standard.
        //:
        //: 6 The 'print' method returns the address of supplied stream.
        //:
        //: 7 The output 'operator<<' returns the address of supplied stream.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' method and 'operator<<' defined
        //:   in this component to initialize, respectively, pointers to member
        //:   and free functions having the appropriate structure.  (C-4, 5)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 6..7)
        //:
        //:   1 Define twelve carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the
        //:     formatting parameters ('level' and 'spacesPerLevel'), along
        //:     with the expected output:
        //:     1. { A } x { 0 }     x { 0, 1, -1 }  -->  3 expected outputs
        //:     2. { A } x { 3, -3 } x { 0, 2, -2 }  -->  6 expected outputs
        //:     3. { B } x { 2 }     x { 3 }         -->  1 expected output
        //:     4. { A B } x { -9 }  x { -9 }        -->  2 expected output
        //:
        //:   2 For each row in the table defined in P2-1:  (C-1..3, 6..7)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', unless the parameters are
        //:       (-9, -9) in which case 'operator<<' will be invoked instead.
        //:       (C-3, 7)
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Very the address of what is returned is that of the supplied
        //:       stream.  (C-6)
        //:
        //:     4 Compare the contents captured in P2-2 with what is expected.
        //:       (C-1..2)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, sPL = 4) const;
        //   operator<<(ostream& s, const baltzo::Zoneinfo& d);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PRINT AND OUTPUT OPERATOR" << endl
                                  << "=========================" << endl;

        if (verbose) cout << "\nAssign the address of print "
                             "and output operator to a variable." << endl;
        {
            using namespace baltzo;
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signature and return type is standard.

            funcPtr     print      = &Obj::print;
            operatorPtr operatorOp = operator<<;

            // Quash unused-variable warnings.

            (void)print; (void)operatorOp;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        const char *ID = "ABC";
        const char *SPEC = "nAxDyB";

        Obj mZ;  const Obj& Z = gg(&mZ, SPEC);

        Obj::TransitionConstIterator iter = Z.beginTransitions();
        const Transition& TA = *iter;
        ++iter;
        const Transition& TD = *iter;
        ++iter;
        const Transition& TB = *iter;

        const struct {
            int           d_line;           // source line number
            int           d_level;
            int           d_spacesPerLevel;

            const char   *d_spec_p;
            const string  d_expected;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1 }  -->  3 expected outputs
        // ------------------------------------------------------------------

        //LINE L SPL  SPEC     EXP
        //---- - ---  ----     ---

        { L_,  0,  0, "xDyB",  "["                                  NL
                               "identifier = \"ABC\""               NL
                               "transitions = ["                    NL
                               + transitionPrintOutput(TD, 1, 0)
                               + transitionPrintOutput(TB, 1, 0) +
                               "]"                                  NL
                               "]"                                  NL
                                                                             },

        { L_,  0,  1, "xDyB",  "["                                  NL
                               " identifier = \"ABC\""              NL
                               " transitions = ["                   NL
                               + transitionPrintOutput(TD, 1, 1)
                               + transitionPrintOutput(TB, 1, 1) +
                               " ]"                                 NL
                               "]"                                  NL
                                                                             },

        { L_,  0, -1, "xDyB",  "["                                  SP
                               "identifier = \"ABC\""               SP
                               "transitions = ["
                               + transitionPrintOutput(TD, 1, -1)
                               + transitionPrintOutput(TB, 1, -1) + SP
                               "]"                                  SP
                               "]"
                                                                             },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2 }  -->  6 expected outputs
        // ------------------------------------------------------------------

        //LINE L SPL  SPEC   EXP
        //---- - ---  ----   ---

        { L_,  3,  0, "xDyB",  "["                                  NL
                               "identifier = \"ABC\""               NL
                               "transitions = ["                    NL
                               + transitionPrintOutput(TD, 4, 0)
                               + transitionPrintOutput(TB, 4, 0) +
                               "]"                                  NL
                               "]"                                  NL
                                                                             },

        { L_,  3,  2, "xDyB",  "      ["                                   NL
                               "        identifier = \"ABC\""              NL
                               "        transitions = ["                   NL
                               + transitionPrintOutput(TD, 4, 2)
                               + transitionPrintOutput(TB, 4, 2) +
                               "        ]"                                 NL
                               "      ]"                                   NL
                                                                             },

        { L_,  3, -2, "xDyB",  "      ["                                   SP
                               "identifier = \"ABC\""                      SP
                               "transitions = ["                           SP
                               + transitionPrintOutput(TD, -4, -2) +       SP
                               + transitionPrintOutput(TB, -4, -2) +       SP
                               "]"                                         SP
                               "]"
                                                                             },

        { L_, -3,  0, "xDyB",  "["                                  NL
                               "identifier = \"ABC\""               NL
                               "transitions = ["                    NL
                               + transitionPrintOutput(TD, 4, 0)
                               + transitionPrintOutput(TB, 4, 0) +
                               "]"                                  NL
                               "]"                                  NL
                                                                             },

        { L_, -3,  2, "xDyB",  "["                                         NL
                               "        identifier = \"ABC\""              NL
                               "        transitions = ["                   NL
                               + transitionPrintOutput(TD, 4, 2)
                               + transitionPrintOutput(TB, 4, 2) +
                               "        ]"                                 NL
                               "      ]"                                   NL
                                                                             },

        { L_, -3, -2, "xDyB",  "["                                         SP
                               "identifier = \"ABC\""                      SP
                               "transitions = ["                           SP
                               + transitionPrintOutput(TD, -4, -2) +       SP
                               + transitionPrintOutput(TB, -4, -2) +       SP
                               "]"                                         SP
                               "]"
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }         -->  1 expected output
        // -----------------------------------------------------------------

        //LINE L SPL  SPEC   EXP
        //---- - ---  ----   ---

        { L_,  2,  3, "nAxD",  "      ["                                   NL
                               "         identifier = \"ABC\""             NL
                               "         transitions = ["                  NL
                               + transitionPrintOutput(TA, 3, 3)
                               + transitionPrintOutput(TD, 3, 3) +
                               "         ]"                                NL
                               "      ]"                                   NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -9 }   x { -9 }      -->  2 expected outputs
        // -----------------------------------------------------------------

        //LINE L SPL  SPEC   EXP
        //---- - ---  ----   ---

        { L_, -9, -9, "xDyB",  "["                                  SP
                               "\"ABC\""                            SP
                               "["                                  SP
                               + transitionOperatorOutput(TD) +     SP
                               + transitionOperatorOutput(TB) +     SP
                               "]"                                  SP
                               "]"
                                                                             },

        { L_, -9, -9, "nAxD",  "["                                  SP
                               "\"ABC\""                            SP
                               "["                                  SP
                               + transitionOperatorOutput(TA) +     SP
                               + transitionOperatorOutput(TD) +     SP
                               "]"                                  SP
                               "]"
                                                                             },

#undef NL
#undef SP

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_line;
                const int         L    = DATA[ti].d_level;
                const int         SPL  = DATA[ti].d_spacesPerLevel;
                const char *const SPEC = DATA[ti].d_spec_p;
                const string      EXP  = DATA[ti].d_expected;

                bslma::TestAllocator oa("supplied", veryVeryVeryVerbose);

                Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);
                mX.setIdentifier(ID);

                ostringstream os;

                if (-9 == L && -9 == SPL) {

                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {

                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &os == &X.print(os, L, SPL));

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                // Verify output is formatted as expected.

                if (veryVeryVerbose) { P(os.str()) }

                LOOP3_ASSERT(LINE, EXP, os.str(), EXP == os.str());
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // 'baltzo::ZoneinfoTransition' 'operator<'
        //   Ensure that the less than operator compares the values of two
        //   objects correctly.
        //
        // Concerns:
        //: 1 An object, 'X', compares less than an object 'Y', if and only
        //:   if 'X.utcTime() < Y.utcTime()'.
        //:
        //: 2 'false == (X < X)'.
        //:
        //: 3 'X < Y' != 'Y < X' unless 'X.utcTime() == Y.utcTime()'.
        //:
        //: 4 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 5 The values of the passed in objects are unmodified.
        //:
        //: 6 The signature and return type are standard.
        //:
        //: 7 There is no allocation from any allocator.
        //
        // Plan:
        //: 1 Use the address of the 'operator<' to initialize a pointer to a
        //:   function having the appropriate structure.  (C-4, 6)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Select candidate attribute values (including the boundaries)
        //:     such that there are transitions that have similar and different
        //:     values for 'utcTime'.
        //:
        //: 4 For each row 'R1' in the table of P-3 having value 'V':  (C-1..7)
        //:
        //:   1 Create two objects, 'X' and 'C' having the value 'V'.
        //:
        //:   2 Verify using 'X' that an object is not less than itself.  (C-2)
        //:
        //:   3 Confirm using the equality operators that the value of 'X' is
        //:     unchanged (and is equal to 'C').  (C-5)
        //:
        //:   4 Verify that no memory is allocated from any allocator.  (C-7)
        //:
        //:   5 For each row 'R2' in the table of P-3 having value 'W':
        //:     (C-1..7)
        //:
        //:     1 Create two objects, 'Y' and 'D' having the value 'W'.
        //:
        //:     2 Record, in 'EXP', whether or not distinct objects 'X' and
        //:       'Y' are expected to compare less than the other.
        //:
        //:     3 Verify that 'EXP == X < Y' and '!EXP == Y < X'.  (C-1, 3)
        //:
        //:     4 Verify that if 'X.utcTime() == Y.utcTime()' then both
        //:       'X < Y' and 'Y < X' return 'false'.  (C-1..3)
        //:
        //:     5 Confirm using the equality operators that the value of 'X'
        //:       and 'Y is unchanged (and is equal to 'C' and 'D'
        //:       respectively).  (C-5)
        //:
        //:     6 Verify that no memory is allocated from any allocator.  (C-7)
        //
        // Testing:
        //   bool operator<(const baltzo::ZoneinfoTransition& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'baltzo::ZoneinfoTransition' 'operator<'" << endl
                          << "=======================================" << endl;

        if (verbose) cout <<
                "\nAssign the address of the operator to a variable." << endl;
        {
            using namespace baltzo;
            typedef bool (*operatorPtr)(const Transition&, const Transition&);

            // Verify that the signature and return type is standard.

            operatorPtr opLess = operator<;

            // Quash unused variable warning.

            (void)opLess;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator da("default",  veryVeryVeryVerbose);

        bslma::Default::setDefaultAllocatorRaw(&da);

        if (verbose) cout <<
                         "\nCreate a table of distinct object values." << endl;

        static const struct {
            int         d_lineNum;
            const char *d_spec_p;
        } DATA[] = {
            //line  spec1
            //----  -----
            { L_,   "nD",     },
            { L_,   "nA",     },
            { L_,   "nB",     },
            { L_,   "xA"      },
            { L_,   "yA"      },
            { L_,   "yB"      },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting 'operator<'" << endl;
        {
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE1 = DATA[ti].d_lineNum;
                const char *const SPEC1 = DATA[ti].d_spec_p;

                // Ensure an object 'operator<'s itself correctly (alias test).
                {
                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                    Obj mA(&oa);  const Obj& A = gg(&mA, SPEC1);

                    Transition mX = A.firstTransition();
                    const Transition& X = mX;

                    const Transition C = X;

                    if (veryVerbose) { T_ P_(LINE1) P_(X) P(C) }

                    bslma::TestAllocatorMonitor dam(&da);
                    bslma::TestAllocatorMonitor oam(&oa);

                    LOOP_ASSERT(LINE1, false == (X < X));

                    LOOP3_ASSERT(LINE1, X, C, C == X);

                    LOOP_ASSERT(LINE1, dam.isTotalSame());
                    LOOP_ASSERT(LINE1, oam.isTotalSame());
                }

                for (int tj = 0; tj < NUM_DATA ; ++tj) {
                    const int         LINE2 = DATA[tj].d_lineNum;
                    const char *const SPEC2 = DATA[tj].d_spec_p;

                    bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                    bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                    Obj mA(&oa1);  const Obj& A = gg(&mA, SPEC1);
                    Obj mB(&oa1);  const Obj& B = gg(&mB, SPEC1);

                    Transition mX = A.firstTransition();
                    const Transition& X = mX;

                    const Transition C = B.firstTransition();

                    Obj mM(&oa2);  const Obj& M = gg(&mM, SPEC2);
                    Obj mN(&oa2);  const Obj& N = gg(&mN, SPEC2);

                    Transition mY = M.firstTransition();
                    const Transition& Y = mY;

                    const Transition D = N.firstTransition();

                    if (veryVerbose) { T_ P_(LINE2) P_(X) P(Y) }

                    bslma::TestAllocatorMonitor oam1(&oa1), oam2(&oa2);

                    if (X.utcTime() == Y.utcTime()) {
                        LOOP4_ASSERT(LINE1, LINE2, X, Y, false == (X < Y));
                        LOOP4_ASSERT(LINE1, LINE2, X, Y, false == (Y < X));
                    }
                    else {
                        const bool EXP = X.utcTime() < Y.utcTime();
                        LOOP5_ASSERT(LINE1, LINE2, X, Y, EXP,  EXP == (X < Y));
                        LOOP5_ASSERT(LINE1, LINE2, X, Y, EXP, !EXP == (Y < X));
                    }

                    LOOP4_ASSERT(LINE1, LINE2, X, C, X == C);
                    LOOP4_ASSERT(LINE1, LINE2, Y, D, Y == D);

                    LOOP2_ASSERT(LINE1, LINE2, oam1.isInUseSame());
                    LOOP2_ASSERT(LINE1, LINE2, oam2.isInUseSame());
                }

                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // 'baltzo::ZoneinfoTransition' EQUALITY OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compare
        //:   equal.
        //:
        //: 2 All salient attributes participate in the comparison.
        //:
        //: 3 'true  == (X == X)'  (i.e., aliasing)
        //:
        //: 4 'false == (X != X)'  (i.e., aliasing)
        //:
        //: 5 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 6 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if '!(X == Y)'
        //:
        //: 8 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 9 Non-modifiable objects can be compared (i.e., objects providing
        //:   only non-modifiable access).
        //:
        //:10 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //:
        //:11 The return type of the equality comparison operators is 'bool'.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate structure for
        //:   the two homogeneous, free equality comparison operators defined
        //:   in this component.  (C-8..9, 11)
        //:
        //: 2 Construct a 'bslma::TestAllocator' object and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their individual salient
        //:   attributes such that (a) for each salient attribute, there exists
        //:   a pair of rows that differ (slightly) in only the column
        //:   corresponding to that attribute, and (b) all attribute values
        //:   that can allocate memory on construction do so.
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-1..8)
        //:   1 Construct a single object and use it to verify the reflexive
        //:     (anti-reflexive) property of equality (inequality) in the
        //:     presence of aliasing.  (C-4..5)
        //:
        //:   2 For each row 'R2' in the table of P-3:  (C-1..3, 6..8)
        //:     1 Record, in 'EXP', whether or not distinct objects created,
        //:       from 'R1' and 'R2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 Create an object 'X' having the value 'R1'.
        //:
        //:     3 Create an object 'Y' having the value 'R2'.
        //:
        //:     4 Verify the commutativity property and expected return value
        //:       for both '==' and '!=', while monitoring that no object
        //:       memory is ever allocated by either operator.  (C-1..3, 6..8)
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-11)
        //
        // Testing:
        //   bool operator==(const baltzo::ZoneinfoTransition& lhs, rhs);
        //   bool operator!=(const baltzo::ZoneinfoTransition& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                         << "'baltzo::ZoneinfoTransition' EQUALITY OPS" << endl
                         << "========================================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            using namespace baltzo;
            typedef bool (*operatorPtr)(const Transition&, const Transition&);

            // Verify that the signature and return type is standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            // Quash unused variable warning.

            (void)operatorEq; (void)operatorNe;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&da);

        static const struct {
            int         d_lineNum;
            const char *d_spec_p;
        } DATA[] = {
            //line  spec
            //----  ----
            { L_,   "nD"      },
            { L_,   "nA"      },
            { L_,   "xA"      },
            { L_,   "xB"      },
            { L_,   "yB"      },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE1 = DATA[ti].d_lineNum;
            const char *const SPEC1 = DATA[ti].d_spec_p;

            bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);

            Obj mA(&oa1);  const Obj& A = gg(&mA, SPEC1);

            const Transition& X = A.firstTransition();

            if (veryVerbose) { T_ P_(LINE1) P_(X) }

            // Ensure an object compares correctly with itself (alias test).

            LOOP2_ASSERT(LINE1, X,   X == X);
            LOOP2_ASSERT(LINE1, X, !(X != X));

            for (int tj = 0; tj < NUM_DATA ; ++tj) {
                const int         LINE2 = DATA[tj].d_lineNum;
                const char *const SPEC2 = DATA[tj].d_spec_p;

                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mB(&oa2);  const Obj& B = gg(&mB, SPEC2);

                const Transition& Z = B.firstTransition();

                const bool EXP = ti == tj;  // expected for equality comparison

                if (veryVerbose) { T_ P_(LINE2) P_(EXP) P(Z) }

                // Verify value, commutativity, and no memory allocation.

                bslma::TestAllocatorMonitor oam1(&oa1), oam2(&oa2);

                LOOP4_ASSERT(LINE1, LINE2, X, Z, EXP == (X == Z));
                LOOP4_ASSERT(LINE1, LINE2, X, Z, EXP == (Z == X));

                LOOP4_ASSERT(LINE1, LINE2, X, Z, !EXP == (X != Z));
                LOOP4_ASSERT(LINE1, LINE2, X, Z, !EXP == (Z != X));

                LOOP2_ASSERT(LINE1, LINE2, oam1.isTotalSame());
                LOOP2_ASSERT(LINE1, LINE2, oam2.isTotalSame());
            }

            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // 'baltzo::ZoneinfoTransition' PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)'
        //:   but with each "attributeName = " elided.
        //:
        //: 4 The 'print' method signature and return value is standard.
        //:
        //: 5 The output 'operator<<' signature and return value is standard.
        //:
        //: 6 The 'print' method returns the address of supplied stream.
        //:
        //: 7 The output 'operator<<' returns the address of supplied stream.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' method and 'operator<<' defined
        //:   in this component to initialize, respectively, pointers to member
        //:   and free functions having the appropriate structure.  (C-4, 5)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 6..7)
        //:
        //:   1 Define twelve carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the
        //:     formatting parameters ('level' and 'spacesPerLevel'), along
        //:     with the expected output:
        //:     1. { A } x { 0 }     x { 0, 1, -1 }  -->  3 expected outputs
        //:     2. { A } x { 3, -3 } x { 0, 2, -2 }  -->  6 expected outputs
        //:     3. { B } x { 2 }     x { 3 }         -->  1 expected output
        //:     4. { A B } x { -9 }  x { -9 }        -->  2 expected output
        //:
        //:   2 For each row in the table defined in P2-1:  (C-1..3, 6..7)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', unless the parameters are
        //:       (-9, -9) in which case 'operator<<' will be invoked instead.
        //:       (C-3, 7)
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Very the address of what is returned is that of the supplied
        //:       stream.  (C-6)
        //:
        //:     4 Compare the contents captured in P2-2 with what is expected.
        //:       (C-1..2)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, sPL = 4) const;
        //   operator<<(ostream& s, const baltzo::ZoneinfoTransition& d);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PRINT AND OUTPUT OPERATOR" << endl
                                  << "=========================" << endl;

        if (verbose) cout << "\nAssign the address of print "
                             "and output operator to a variable." << endl;
        {
            using namespace baltzo;
            typedef ostream& (Transition::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Transition&);

            // Verify that the signature and return type is standard.

            funcPtr     print      = &Transition::print;
            operatorPtr operatorOp = operator<<;

            // Quash unused variable warning.

            (void)print; (void)operatorOp;

        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        const Descriptor& DD = DESCRIPTORS[0];
        const Descriptor& DB = DESCRIPTORS[2];

        const struct {
            int           d_line;           // source line number
            int           d_level;
            int           d_spacesPerLevel;

            const char   *d_spec_p;
            const string  d_expected;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1 }  -->  3 expected outputs
        // ------------------------------------------------------------------

        //LINE L SPL  SPEC   EXP
        //---- - ---  ----   ---

        { L_,  0,  0, "yB",  "["                                  NL
                             "time = 01JAN1970_00:33:20.000"      NL
                             "descriptor = "
                             + descriptorPrintOutput(DB, -1, 0) +
                             "]"                                  NL
                                                                             },

        { L_,  0,  1, "yB",  "["                                  NL
                             " time = 01JAN1970_00:33:20.000"     NL
                             " descriptor = "
                             + descriptorPrintOutput(DB, -1, 1) +
                             "]"                                  NL
                                                                             },

        { L_,  0, -1, "yB",  "["                                   SP
                             "time = 01JAN1970_00:33:20.000"       SP
                             "descriptor = "
                             + descriptorPrintOutput(DB, -1, -1) + SP
                             "]"
                                                                             },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2 }  -->  6 expected outputs
        // ------------------------------------------------------------------

        //LINE L SPL  SPEC   EXP
        //---- - ---  ----   ---

        { L_,  3,  0, "yB",  "["                                      NL
                             "time = 01JAN1970_00:33:20.000"          NL
                             "descriptor = "
                             + descriptorPrintOutput(DB, -4, 0) +
                             "]"                                      NL
                                                                             },

        { L_,  3,  2, "yB",  "      ["                                NL
                             "        time = 01JAN1970_00:33:20.000"  NL
                             "        descriptor = "
                             + descriptorPrintOutput(DB, -4, 2) +
                             "      ]"                                NL
                                                                             },

        { L_,  3, -2, "yB",  "      ["                                SP
                             "time = 01JAN1970_00:33:20.000"          SP
                             "descriptor = "
                             + descriptorPrintOutput(DB, -4, -2) +    SP
                             "]"
                                                                             },


        { L_, -3,  0, "yB",  "["                                      NL
                             "time = 01JAN1970_00:33:20.000"          NL
                             "descriptor = "
                             + descriptorPrintOutput(DB, -4, 0) +
                             "]"                                      NL
                                                                             },


        { L_, -3,  2, "yB",  "["                                      NL
                             "        time = 01JAN1970_00:33:20.000"  NL
                             "        descriptor = "
                             + descriptorPrintOutput(DB, -4, 2) +
                             "      ]"                                NL
                                                                             },

        { L_, -3, -2, "yB",  "["                                      SP
                             "time = 01JAN1970_00:33:20.000"          SP
                             "descriptor = "
                             + descriptorPrintOutput(DB, -4, -2) +     SP
                             "]"
                                                                             },
        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }         -->  1 expected output
        // -----------------------------------------------------------------

        //LINE L SPL  SPEC   EXP
        //---- - ---  ----   ---

        { L_,  2,  3, "xB",  "      ["                                 NL
                             "         time = 01JAN1970_00:16:40.000"  NL
                             "         descriptor = "
                             + descriptorPrintOutput(DB, -3, 3) +
                             "      ]"                                 NL
                                                                             },


        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -9 }   x { -9 }      -->  2 expected outputs
        // -----------------------------------------------------------------

        //LINE L SPL  SPEC   EXP
        //---- - ---  ----   ---

        {
            L_,
            -9,
            -9,
            "yB",
            "[ 01JAN1970_00:33:20.000 " + descriptorOperatorOutput(DB) + " ]"
        },

        {
            L_,
            -9,
            -9,
            "nD",
            "[ 01JAN1970_00:00:00.000 " + descriptorOperatorOutput(DD) + " ]"
        },

#undef NL
#undef SP

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_line;
                const int         L    = DATA[ti].d_level;
                const int         SPL  = DATA[ti].d_spacesPerLevel;
                const char *const SPEC = DATA[ti].d_spec_p;
                const string      EXP  = DATA[ti].d_expected;

                bslma::TestAllocator oa("supplied", veryVeryVeryVerbose);

                Obj mA(&oa);  const Obj& A = gg(&mA, SPEC);

                const Transition& X = A.firstTransition();

                if (veryVerbose) { T_ P_(LINE) P_(X) }

                ostringstream os;

                if (-9 == L && -9 == SPL) {

                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {

                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &os == &X.print(os, L, SPL));

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                // Verify output is formatted as expected.

                if (veryVeryVerbose) { P(os.str()) }

                LOOP3_ASSERT(LINE, EXP, os.str(), EXP == os.str());
            }

        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'baltzo::ZoneinfoTransition' ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute
        //:   of the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 No accessor allocates any memory.
        //:
        //: 4 Attributes that can allocate memory are returned by reference
        //:   providing only non-modifiable access.
        //
        // Plan:
        //
        //: 1 Create two 'bslma::TestAllocator' objects, and install one as
        //:   the current default allocator (note that a ubiquitous test
        //:   allocator is already installed as the global allocator).
        //:
        //: 2 Use the default constructor to create an object (having default
        //:   attribute values) holding the other test allocator created in
        //:   P-1.
        //:
        //: 3 Verify that each basic accessor, invoked on a reference providing
        //:   non-modifiable access to the object created in P2, returns the
        //:   expected value.  (C-2)
        //:
        //: 4 For each salient attribute (contributing to value):  (C-1, 3..4)
        //:   1 Use the corresponding primary manipulator to set the attribute
        //:     to a unique value, making sure to allocate memory if possible.
        //:
        //:   2 Use the corresponding basic accessor to verify the new
        //:     expected value.  (C-1)
        //:
        //:   3 Monitor the memory allocated from both the default and object
        //:     allocators before and after calling the accessor; verify that
        //:     there is no change in total memory allocation.  (C-3..4)
        //
        // Testing:
        //   baltzo::ZoneinfoTransition
        //
        //   const baltzo::LocalTimeDescriptor& descriptor() const;
        //   bdlt::EpochUtil::TimeT64 utcTime() const;
        //
        //   baltzo::Zoneinfo
        //
        //   bslma::Allocator *allocator() const;
        //   const Transition& firstTransition() const;
        //   bsl::size_t numTransitions() const;
        //   TransitionConstIterator beginTransitions() const;
        //   TransitionConstIterator endTransitions() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const TimeT64    D1 = 0;              // 'time'
        const Descriptor D2;                  // 'descriptor'

        // 'A' values: Should cause memory allocation if possible.

        const int   OFF    = -24 * 60 * 60 + 1;
        const bool  DST    = true;
        const char  DESC[] = "a_" SUFFICIENTLY_LONG_STRING;

        const TimeT64    A1 = 100;            // 'time'
        const Descriptor A2(OFF, DST, DESC);  // 'descriptor'

        bslma::TestAllocator da("default",  veryVeryVeryVerbose);
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        bslma::Default::setDefaultAllocatorRaw(&da);

        Obj mX(&oa);  const Obj& X = mX;   // original spec
        LOOP_ASSERT(X.numTransitions(), 0 == X.numTransitions());

        {
            mX.addTransition(A1, A2);
            LOOP_ASSERT(X.numTransitions(), 1 == X.numTransitions());

            bslma::TestAllocatorMonitor oam(&oa), dam(&da);

            const Transition& FRONT = X.firstTransition();
            LOOP2_ASSERT(A1, FRONT.utcTime(),    A1 == FRONT.utcTime());
            LOOP2_ASSERT(A2, FRONT.descriptor(), A2 == FRONT.descriptor());

            TransitionConstIter begin = X.beginTransitions();
            TransitionConstIter end   = X.endTransitions();

            ASSERT(1 == (end - begin));

            LOOP2_ASSERT(A1, begin->utcTime(),    A1 == begin->utcTime());
            LOOP2_ASSERT(A2, begin->descriptor(), A2 == begin->descriptor());

            ASSERT(oam.isInUseSame());  ASSERT(dam.isInUseSame());
        }

        {
            mX.addTransition(D1, D2);
            LOOP_ASSERT(X.numTransitions(), 2 == X.numTransitions());

            bslma::TestAllocatorMonitor oam(&oa), dam(&da);

            const Transition& FRONT = X.firstTransition();
            LOOP2_ASSERT(D1, FRONT.utcTime(),    D1 == FRONT.utcTime());
            LOOP2_ASSERT(D2, FRONT.descriptor(), D2 == FRONT.descriptor());

            TransitionConstIter begin = X.beginTransitions();
            TransitionConstIter end   = X.endTransitions();

            ASSERT(2 == (end - begin));

            LOOP2_ASSERT(D1, begin->utcTime(),    D1 == begin->utcTime());
            LOOP2_ASSERT(D2, begin->descriptor(), D2 == begin->descriptor());

            ++begin;
            LOOP2_ASSERT(A1, begin->utcTime(),    A1 == begin->utcTime());
            LOOP2_ASSERT(A2, begin->descriptor(), A2 == begin->descriptor());

            ASSERT(oam.isInUseSame());  ASSERT(dam.isInUseSame());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout <<
                 "\t'ACCESSOR 'firstTransition' with no transitions" << endl;
            {
                Obj temp;  const Obj& TEMP = temp;
                LOOP_ASSERT(TEMP.numTransitions(), 0 == TEMP.numTransitions());

                ASSERT_SAFE_PASS(X.firstTransition());
                ASSERT_SAFE_FAIL(TEMP.firstTransition());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTION 'gg'
        //   Ensure that the generator function is able to create an object in
        //   any state.
        //
        // Concerns:
        //   The 'gg' generator is implemented as a finite state machine (FSM).
        //   All symbols must be recognized successfully, and all possible
        //   state transitions must be verified.
        //
        // Plan:
        //   Evaluate a series of test strings of increasing complexity to
        //   validate every major state and transition in the FSM underlying
        //   gg.  Add extra tests to validate less likely transitions until
        //   they are all covered.
        //
        // Testing:
        //   Obj& gg(Obj *object, const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Generator Function 'gg'" << endl
                          << "=======================" << endl;

        if (verbose) cout << "\nTesting generator" << endl;
        {
            static const struct {
                int         d_lineNum;
                const char *d_spec_p;
            } DATA[] = {
                //line  spec
                //----  ----
                { L_,   ""        },

                { L_,   "nD"      },
                { L_,   "xA"      },
                { L_,   "yB"      },

                { L_,   "nDxA"    },
                { L_,   "nAxD"    },

                { L_,   "nAxDyA"  },
                { L_,   "yAxDnD"  },

                // duplicates

                { L_,   "nDnA"    },
                { L_,   "xDxD"    },

                { L_,   "xDxAxB"  },
                { L_,   "xDnAxB"  },
                { L_,   "xDnAxD"  },

                { L_,   "xDnAyBxAnByD"  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int         NUM_TRANSITIONS = ((int) strlen(SPEC)) / 2;

                my_TransitionMap expTransitions;
                const my_TransitionMap& EXP = expTransitions;

                for (int i = 0; i < NUM_TRANSITIONS; ++i) {
                    const char TS = SPEC[i * 2];
                    const char DS = SPEC[i * 2 + 1];

                    const int  TI = 'n' == TS ? 0
                                  : 'x' == TS ? 1
                                  :             2;

                    const int  DI = 'D' == DS ? 0
                                  : 'A' == DS ? 1
                                  :             2;

                    const TimeT64     T = TIMES[TI];
                    const Descriptor& D = DESCRIPTORS[DI];

                    expTransitions[T] = D;
                }

                Obj mX;  const Obj& X = gg(&mX, SPEC);   // original spec

                LOOP_ASSERT(LINE, EXP.size() == X.numTransitions());

                my_TransitionMapConstIter expIter = EXP.begin();
                for (TransitionConstIter objIter = X.beginTransitions();
                     objIter != X.endTransitions();
                     ++objIter, ++expIter) {
                    const Transition&        T = *objIter;
                    const my_Transition& EXP_T = *expIter;

                    LOOP3_ASSERT(LINE, EXP_T.first, T.utcTime(),
                                 EXP_T.first == T.utcTime());
                    LOOP3_ASSERT(LINE, EXP_T.second, T.descriptor(),
                                 EXP_T.second == T.descriptor());
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATOR, & DTOR
        //   Ensure that we can default construct an object and use the primary
        //   manipulator to put it into any state relevant for thorough
        //   testing.
        //
        // Concerns:
        //: 1 An object created with the default constructor (with or without
        //:   a supplied allocator) has the contractually specified default
        //:   value.
        //:
        //: 2 If an allocator is *not* supplied to the default constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 3 If an allocator is supplied to the default constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the default constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator, which is
        //:   returned by the 'allocator' accessor method.
        //:
        //: 7 There is no temporary allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 Each attribute is modifiable independently.
        //:
        //:10 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented preconditions.
        //:
        //:11 Any argument can be 'const'.
        //:
        //:12 Any memory allocation is exception neutral.
        //:
        //:13 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create three sets of attribute values for the object: ('D')
        //:   default-constructed values, ('A') values that allocate memory if
        //:   possible, and ('B') other values that do not cause additional
        //:   memory allocation, but use an alternate C++ type if appropriate
        //:   (e.g., 'string' instead of 'char *').  Note that both the 'A' and
        //:   'B' attribute values should be chosen to be boundary values where
        //:   possible.
        //:
        //: 2 Using a loop-based approach, default-construct three distinct
        //:   objects in turn, but configured differently: (a) without passing
        //:   an allocator, (b) passing a null allocator address explicitly,
        //:   and (c) passing the address of a test allocator distinct from the
        //:   default.  For each of these three iterations:  (C-1..12)
        //:
        //:   1 Construct three 'bslma::TestAllocator' objects and install
        //:     one as the current default allocator (note that a ubiquitous
        //:     test allocator is already installed as the global allocator).
        //:
        //:   2 Dynamically default construct an object, configured
        //:     appropriately (see P-2) using a distinct test allocator for
        //:     the object's footprint.
        //:
        //:   3 Use the 'allocator' accessor of each underlying attribute
        //:     capable of allocating memory to ensure that its object
        //:     allocator is properly installed; also apply the (as yet
        //:     unproven) 'allocator' accessor of the object under test.  (C-6)
        //:
        //:   4 Use the individual (as yet unproven) salient attribute
        //:     accessors to verify the default-constructed value.  (C-1)
        //:
        //:   5 For each attribute 'i', in turn, create a local block.  Then
        //:     inside the block, using brute force, set that attribute's
        //:     values, passing a 'const' argument of each value, in turn:
        //:     first to 'Ai', then to 'Bi', and finally back to 'Di' (see
        //:     P-1).  If attribute 'i' can allocate memory, verify that it
        //:     does so on the first value transition ('Di' -> 'Ai') and that
        //:     the corresponding primary manipulator is exception neutral.  In
        //:     all other cases, verify that no memory allocation occurs.
        //:     After each transition, use the (as yet unproven) basic
        //:     accessors to verify that only the intended attribute value
        //:     changed.  (C-2..5, 9..12)
        //:
        //:   7 Verify that no temporary memory is allocated from the object
        //:     allocator.  (C-7)
        //:
        //:   8 Verify that all object memory is released when the object is
        //:     destroyed.  (C-8)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-13)
        //
        // Testing:
        //   baltzo::Zoneinfo(bslma::Allocator *bA = 0);
        //   void addTransition(TimeT64 time, const baltzo::LTD& d);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DEFAULT CTOR & PRIMARY MANIPULATORS" << endl
                          << "===================================" << endl;

        const string ID = "";       // default identifier

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;
        {
            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                bslma::Default::setDefaultAllocatorRaw(&da);

                Obj                 *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj();
                      objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(0);
                      objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(&sa);
                      objAllocatorPtr = &sa;
                  } break;
                  default: {
                      LOOP_ASSERT(CONFIG, !"Bad allocator Config.");
                  } break;
                }

                Obj&                  mX = *objPtr;  const Obj& X = mX;
                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                // -------------------------------------------------------
                // Verify any attribute allocators are installed properly.
                // -------------------------------------------------------

                LOOP_ASSERT(CONFIG, &oa == X.identifier().allocator());

                // -------------------------------------
                // Verify the object's attribute values.
                // -------------------------------------

                LOOP2_ASSERT(CONFIG, X.identifier(),
                             ID == X.identifier());
                LOOP2_ASSERT(CONFIG, X.numTransitions(),
                             0 == X.numTransitions());
                LOOP_ASSERT(CONFIG,
                            X.beginTransitions() == X.endTransitions());

                // Also apply the object's 'allocator' accessor.

                LOOP3_ASSERT(CONFIG, &oa, X.allocator(), &oa == X.allocator());

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                LOOP_ASSERT(fa.numBlocksInUse(),  0 == fa.numBlocksInUse());

                // QOA: The default-constructed 'set' data member does not
                // allocate.

                LOOP2_ASSERT(CONFIG,  oa.numBlocksTotal(),
                             0 ==  oa.numBlocksTotal());

                // Verify no allocation from the non-object allocators.

                LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                             0 == noa.numBlocksTotal());
            }
        }

        if (verbose) cout << "\nTesting 'addTransition' (with exceptions)"
                          << endl;
        {
            static const struct {
                int         d_line;      // source line number

                TimeT64     d_utcSecondsFromEpoch;

                int         d_utcOffsetInSeconds;
                bool        d_dstInEffectFlag;
                const char *d_description_p;
            } DATA[] = {

            //LINE  TIME    OFFSET    FLAG   DESC
            //----  ----    ------    ----   ----

            // default (must be first)
            { L_,   0,        0,  false,  ""                  },

            { L_,   1,        1,   true,  "EST"               },

            { L_,  -1,        0,  false,  "EDT"               },

            { L_,   2,       10,   true,  "New_York/America"  },

            { L_,  -2,      -10,   true,  "abc"               },

            // same utcTime, different descriptor
            { L_,   1,        2,  false,  "EDT"               },

            // same utcTime, same descriptor
            { L_,  -1,        0,  false,  "EDT"               },

            // different utcTime, same descriptor
            { L_,   3,        2,  false,  "EDT"               },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            my_TransitionMap expTransitions(&scratch);
            const my_TransitionMap& EXP = expTransitions;

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
            bslma::Default::setDefaultAllocatorRaw(&da);
            {
                Obj mX(&oa);  const Obj& X = mX;
                LOOP_ASSERT(X.numTransitions(), 0 == X.numTransitions());

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int         LINE   = DATA[ti].d_line;
                    const TimeT64     TIME   = DATA[ti].d_utcSecondsFromEpoch;
                    const int         OFFSET = DATA[ti].d_utcOffsetInSeconds;
                    const bool        FLAG   = DATA[ti].d_dstInEffectFlag;
                    const char *const DESC   = DATA[ti].d_description_p;

                    const Descriptor  LTD(OFFSET, FLAG, DESC, &scratch);

                    if (veryVerbose) { T_ P_(LINE) P_(TIME) P(LTD) }

                    expTransitions[TIME] = LTD;

                    const bsl::size_t NT = EXP.size();

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        mX.addTransition(TIME, LTD);

                        LOOP3_ASSERT(LINE, NT, X.numTransitions(),
                                     NT == X.numTransitions());

                        my_TransitionMapConstIter expIter = EXP.begin();
                        TransitionConstIter objIter = X.beginTransitions();
                        for (; objIter != X.endTransitions();
                             ++objIter, ++expIter) {

                            const Transition&        T = *objIter;
                            const my_Transition& EXP_T = *expIter;

                            LOOP3_ASSERT(LINE, EXP_T.first, T.utcTime(),
                                         EXP_T.first == T.utcTime());
                            LOOP3_ASSERT(LINE, EXP_T.second, T.descriptor(),
                                         EXP_T.second == T.descriptor());

                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
            LOOP_ASSERT(oa.numBlocksTotal(), 0 == oa.numBlocksInUse());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST: 'baltzo::Zoneinfo' & 'baltzo::ZoneinfoTransition'
        //
        // Concerns:
        //   We want to exercise basic value-semantic functionality.  In
        //   particular we want to demonstrate a base-line level of correct
        //   operation of the following methods and operators:
        //      - default and copy constructors (and also the destructor)
        //      - the assignment operator (including aliasing)
        //      - equality operators: 'operator==()' and 'operator!=()'
        //      - the (test-driver supplied) output operator: 'operator<<()'
        //      - primary manipulators: 'addTransition'
        //      - basic accessors: 'beginTransition', 'endTransition'
        //   In addition we would like to exercise objects with potentially
        //   different internal organizations representing the same value.
        //
        // Plan:
        //   Create four test objects by using the default, initializing, and
        //   copy constructors.  Exercise the basic value-semantic methods and
        //   the equality operators using these test objects.  Invoke the
        //   primary manipulator [3, 6], copy constructor [2, 5], and
        //   assignment operator without [7, 8] and with [9] aliasing.  Use the
        //   basic accessors to verify the expected results.  Display object
        //   values frequently in verbose mode.  Note that 'VA', 'VB', and 'VC'
        //   denote unique, but otherwise arbitrary, object values, while 'U'
        //   denotes the valid, but "unknown", default object value.
        //
        // 1.  Create an object x1 (init. to VA).    { x1:VA }
        // 2.  Create an object x2 (copy from x1).   { x1:VA x2:VA }
        // 3.  Set x1 to VB.                         { x1:VB x2:VA }
        // 4.  Create an object x3 (default ctor).   { x1:VB x2:VA x3:U }
        // 5.  Create an object x4 (copy from x3).   { x1:VB x2:VA x3:U  x4:U }
        // 6.  Set x3 to VC.                         { x1:VB x2:VA x3:VC x4:U }
        // 7.  Assign x2 = x1.                       { x1:VB x2:VB x3:VC x4:U }
        // 8.  Assign x2 = x3.                       { x1:VB x2:VC x3:VC x4:U }
        // 9.  Assign x1 = x1 (aliasing).            { x1:VB x2:VB x3:VC x4:U }
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                        << "BREATHING TEST: baltzo::ZoneinfoTransition" << endl
                        << "=========================================" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Values for testing

        const bdlt::Datetime FIRST_DATETIME(1, 1, 1, 0, 0, 0);
        const TimeT64 FIRST_DATETIME64 =
                      bdlt::EpochUtil::convertToTimeT64(FIRST_DATETIME);

        // VA

        const char *TID_A = "America/New_York";
                                        // time zone id for VA

        const int OA_A = -4 * 60 * 60;  // offset for LTA_A
        const int OB_A =  1 * 60 * 60;  // offset for LTA_A
        const int OC_A =  0;            // offset for LTA_A

        const bool IDA_A = true ;       // isDst for LTA_A
        const bool IDB_A = false;       // isDst for LTB_A
        const bool IDC_A = false;       // isDst for LTC_A

        const char *DA_A = "EDT";       // description for LTA_A
        const char *DB_A = "GMT+1";     // description for LTB_A
        const char *DC_A = "UTC";       // description for LTC_A

        const baltzo::LocalTimeDescriptor lta_a(OA_A, IDA_A, DA_A);
        const baltzo::LocalTimeDescriptor ltb_a(OB_A, IDB_A, DB_A);
        const baltzo::LocalTimeDescriptor ltc_a(OC_A, IDC_A, DC_A);

        const baltzo::LocalTimeDescriptor& LTA_A =  lta_a;
        const baltzo::LocalTimeDescriptor& LTB_A =  ltb_a;
        const baltzo::LocalTimeDescriptor& LTC_A =  ltc_a;

        const bdlt::Datetime TA_A(1970,  1,  1,  0,  0,  0);
        const bdlt::Datetime TB_A(2000,  3, 15,  3, 10, 15);
        const bdlt::Datetime TC_A(2038,  1, 19,  3, 14,  7);

        const TimeT64 TTA_A =
                                bdlt::EpochUtil::convertToTimeT64(TA_A);
        const TimeT64 TTB_A =
                                bdlt::EpochUtil::convertToTimeT64(TB_A);
        const TimeT64 TTC_A =
                                bdlt::EpochUtil::convertToTimeT64(TC_A);

        // VB
        const char *TID_B("Europe/Berlin");
                                        // time zone id for VB

        const int OA_B =  2 * 60 * 60;  // offset for LTA_B
        const int OB_B =  1 * 60 * 60;  // offset for LTB_B

        const bool IDA_B = true ;       // isDst for LTA_B
        const bool IDB_B = false;       // isDst for LTB_B

        const char *DA_B = "CEST";      // description for LTA_B
        const char *DB_B = "CET";       // description for LTB_B

        baltzo::LocalTimeDescriptor lta_b(OA_B, IDA_B, DA_B);
        baltzo::LocalTimeDescriptor ltb_b(OB_B, IDB_B, DB_B);

        const baltzo::LocalTimeDescriptor& LTA_B =  lta_b;
        const baltzo::LocalTimeDescriptor& LTB_B =  ltb_b;

        const bdlt::Datetime TA_B(2010,  3, 14,  2,  0,  0);
        const bdlt::Datetime TB_B(2010, 11,  9,  2,  0,  0);

        const TimeT64 TTA_B = bdlt::EpochUtil::convertToTimeT64(TA_B);
        const TimeT64 TTB_B = bdlt::EpochUtil::convertToTimeT64(TB_B);

        // VC
        const char *TID_C("Pacific/Auckland");
                                          // time zone id for VC

        const int   OA_C = 10 * 60 * 60;  // offset for LTA_C

        const bool  IDA_C = true ;        // isDst for LTA_C

        const char *DA_C = "PET";         // description for LTA_C

        baltzo::LocalTimeDescriptor lta_c(OA_C, IDA_C, DA_C);

        const baltzo::LocalTimeDescriptor& LTA_C = lta_c;

        const bdlt::Datetime TA_C(2010,  3, 14,  2,  0,  0);

        const TimeT64 TTA_C = bdlt::EpochUtil::convertToTimeT64(TA_C);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 1. Create an object x1 (init. to VA)."
                              << "\t\t{ x1:VA }"
                              << endl;

        Obj mX1; const Obj& X1 = mX1;

        mX1.setIdentifier(TID_A);

        mX1.addTransition(FIRST_DATETIME64, LTA_A);

        mX1.addTransition(TTA_A, LTA_A);
        mX1.addTransition(TTB_A, LTB_A);
        mX1.addTransition(TTC_A, LTC_A);

        if (veryVerbose) { cout << '\t';  P(X1); }
        if (veryVerbose) cout << "\ta. Check initial state of x1." << endl;

        ASSERT(TID_A  == X1.identifier());

        {
            Obj::TransitionConstIterator itT;
            itT = X1.beginTransitions();
            ASSERT(FIRST_DATETIME64 == itT->utcTime());
            ASSERT(LTA_A          == itT->descriptor());
            ++itT;
            ASSERT(TTA_A == itT->utcTime());
            ASSERT(LTA_A == itT->descriptor());
            ++itT;
            ASSERT(TTB_A == itT->utcTime());
            ASSERT(LTB_A == itT->descriptor());
            ++itT;
            ASSERT(TTC_A == itT->utcTime());
            ASSERT(LTC_A == itT->descriptor());
            ++itT;  ASSERT(X1.endTransitions() == itT);

            ASSERT(4 == X1.numTransitions());
        }

        if (veryVerbose) cout << "\tb. Try equality operators: x1 <op> x1."
                              << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 2. Create an object x2 (copy from x1)."
                              << "\t\t{ x1:VA x2:VA }"
                              << endl;

        Obj mX2(X1);  const Obj& X2 = mX2;
        if (veryVerbose) { cout << '\t';  P(X2); }

        if (veryVerbose) cout << "\ta. Check the initial state of x2." << endl;

        ASSERT(TID_A  == mX2.identifier());

        {
            baltzo::Zoneinfo::TransitionConstIterator itT;
            itT = X2.beginTransitions();
            ASSERT(FIRST_DATETIME64 == itT->utcTime());
            ASSERT(LTA_A          == itT->descriptor());
            ++itT;
            ASSERT(TTA_A == itT->utcTime());
            ASSERT(LTA_A == itT->descriptor());
            ++itT;
            ASSERT(TTB_A == itT->utcTime());
            ASSERT(LTB_A == itT->descriptor());
            ++itT;
            ASSERT(TTC_A == itT->utcTime());
            ASSERT(LTC_A == itT->descriptor());
            ++itT;  ASSERT(X2.endTransitions() == itT);

            ASSERT(4 == X2.numTransitions());
        }

        if (veryVerbose) cout << "\tb. Try equality operators: x2 <op> x1, x2."
                              << endl;

        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1 = Obj();
        mX1.setIdentifier(TID_B);
        ASSERT(0 == X1.numTransitions());

        mX1.addTransition(FIRST_DATETIME64, LTA_B);
        ASSERT(1 == X1.numTransitions());

        mX1.addTransition(TTA_B, LTA_B);
        mX1.addTransition(TTB_B, LTB_B);
        ASSERT(3 == X1.numTransitions());

        if (veryVerbose) { T_ P(X1) }

        if (verbose) cout << "\ta. Check new state of x1." << endl;

        ASSERT(TID_B == X1.identifier());

        {
            baltzo::Zoneinfo::TransitionConstIterator itT;
            itT = X1.beginTransitions();
            ASSERT(FIRST_DATETIME64 == itT->utcTime());
            ASSERT(LTA_B          == itT->descriptor());
            ++itT;
            ASSERT(TTA_B == itT->utcTime());
            ASSERT(LTA_B == itT->descriptor());
            ++itT;
            ASSERT(TTB_B == itT->utcTime());
            ASSERT(LTB_B == itT->descriptor());
            ++itT;  ASSERT(X1.endTransitions() == itT);

            ASSERT(3 == X1.numTransitions());
        }

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (veryVerbose) { T_ P(X3) }

        if (verbose) cout << "\ta. Check initial state of x3." << endl;

        {
            ASSERT("" == mX3.identifier());
            ASSERT(0  == mX3.numTransitions());

            baltzo::Zoneinfo::TransitionConstIterator itT;
            itT = X3.beginTransitions();
            ASSERT(X3.endTransitions() == itT);
        }

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (veryVerbose) { T_ P(X4) }

        if (verbose) cout << "\ta. Check initial state of x4." << endl;

        {
            ASSERT("" == X4.identifier());
            ASSERT(0  == X4.numTransitions());

            baltzo::Zoneinfo::TransitionConstIterator itT;
            itT = X4.beginTransitions();
            ASSERT(X4.endTransitions() == itT);
        }

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;

        mX3.setIdentifier(TID_C);
        ASSERT(0 == X3.numTransitions());

        mX3.addTransition(FIRST_DATETIME64, LTA_C);
        ASSERT(1 == X3.numTransitions());

        mX3.addTransition(TTA_C, LTA_C);
        ASSERT(2 == X3.numTransitions());

        if (veryVerbose) { T_ P(X3) }

        if (verbose) cout << "\ta. Check new state of x3." << endl;

        {
            baltzo::Zoneinfo::TransitionConstIterator itT;
            itT = X3.beginTransitions();
            ASSERT(FIRST_DATETIME64 == itT->utcTime());
            ASSERT(           LTA_C == itT->descriptor());
            ++itT;
            ASSERT(TTA_C == itT->utcTime());
            ASSERT(LTA_C == itT->descriptor());
            ++itT;  ASSERT(X3.endTransitions() == itT);

            ASSERT(2 == X3.numTransitions());
        }

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Assign x2 = x1."
                             "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }" << endl;
        mX2 = X1;
        if (veryVerbose) { T_ P(X2) }

        if (verbose) cout << "\ta. Check new state of x2." << endl;

        ASSERT(TID_B == X2.identifier());
        ASSERT(3     == X2.numTransitions());

        {
            baltzo::Zoneinfo::TransitionConstIterator itT;
            itT = X2.beginTransitions();
            ASSERT(FIRST_DATETIME64 == itT->utcTime());
            ASSERT(LTA_B == itT->descriptor());
            ++itT;
            ASSERT(TTA_B == itT->utcTime());
            ASSERT(LTA_B == itT->descriptor());
            ++itT;
            ASSERT(TTB_B == itT->utcTime());
            ASSERT(LTB_B == itT->descriptor());
            ++itT;  ASSERT(X2.endTransitions() == itT);

            ASSERT(3 == X2.numTransitions());
        }

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x3."
                             "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;
        mX2 = X3;
        if (veryVerbose) { T_ P(X2) }

        if (verbose) cout << "\ta. Check new state of x2." << endl;

        {
            baltzo::Zoneinfo::TransitionConstIterator itT;
            itT = X2.beginTransitions();
            ASSERT(FIRST_DATETIME64 == itT->utcTime());
            ASSERT(LTA_C == itT->descriptor());
            ++itT;
            ASSERT(TTA_C == itT->utcTime());
            ASSERT(LTA_C == itT->descriptor());
            ++itT;  ASSERT(X2.endTransitions() == itT);
            ASSERT(2 == X2.numTransitions());
        }

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 9. Assign x1 = x1 (aliasing)."
                             "\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;
        if (veryVerbose) { T_ P(X1) }

        mX1 = X1;
        if (veryVerbose) { T_ P(X1) }

        if (verbose) cout << "\ta. Check new state of x1." << endl;

        {
            baltzo::Zoneinfo::TransitionConstIterator itT;
            itT = X1.beginTransitions();
            ASSERT(FIRST_DATETIME64 == itT->utcTime());
            ASSERT(LTA_B == itT->descriptor());
            ++itT;
            ASSERT(TTA_B == itT->utcTime());
            ASSERT(LTA_B == itT->descriptor());
            ++itT;
            ASSERT(TTB_B == itT->utcTime());
            ASSERT(LTB_B == itT->descriptor());
            ++itT;  ASSERT(X1.endTransitions() == itT);
            ASSERT(3 == X1.numTransitions());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

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
