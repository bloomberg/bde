// bdlt_datetimeutil.t.cpp                                            -*-C++-*-
#include <bdlt_datetimeutil.h>

#include <bslim_testutil.h>
#include <bslma_usesbslmaallocator.h>      // usage example
#include <bslmf_nestedtraitdeclaration.h>  // usage example

#include <bsl_algorithm.h> // 'min'
#include <bsl_climits.h>
#include <bsl_cstdio.h>    // 'sprintf'
#include <bsl_cstdlib.h>   // 'atoi'
#include <bsl_cstring.h>   // 'memset'
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#include <bsl_map.h>       // usage example
#include <bsl_string.h>    //   "     "
#include <bsl_utility.h>   //   "     "

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This test driver emphasizes a black-box approach, which is necessarily quite
// voluminous relative to what would be required given white-box knowledge.
// We use the standard table-based test case implementation techniques coupled
// with category partitioning to exercise these utility functions.  We also
// use loop-based, statistical methods to ensure invertibility where
// appropriate.
//-----------------------------------------------------------------------------
// [ 3] tm convertToTm(const Datetime& datetime);
// [ 3] int convertFromTm(Datetime *result, const tm& timeStruct);
//-----------------------------------------------------------------------------
// [ 1] BOOTSTRAP: 'convertToTm'
// [ 2] BOOTSTRAP: 'convertFromTm'
// [ 4] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

//=============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlt::DatetimeUtil  Util;

typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                SEMI-STANDARD HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void loopMeter(unsigned index, unsigned length, unsigned size = 50)
    // Create a visual display for a computation of the specified 'length' and
    // emit updates to 'cerr' as appropriate for each specified 'index'.
    // Optionally specify the 'size' of the display.  The behavior is undefined
    // unless 0 <= index, 0 <= length, 0 < size, and index <= length.  Note
    // that it is expected that indices will be presented in order from 0 to
    // 'length', inclusive, without intervening output to 'stderr'; however,
    // intervening output to 'stdout' may be redirected productively.
{
    ASSERT(0 < size);
    ASSERT(index <= length);

    if (0 == index) {           // We are at the beginning of the loop.
        cerr << "     |";
        for (unsigned i = 1; i < size; ++i) {
            cerr << (i % 5 ? '-' : '+');
        }
        cerr << "|\nBEGIN." << flush;
    }
    else {                      // We are in the middle of the loop.
        int t1 = int((double(index - 1) * size)/length + 0.5);
        int t2 = int((double(index)     * size)/length + 0.5);
        int dt = t2 - t1;       // accumulated ticks (but no accumulated error)

        for (int i = 0; i < dt; ++i) {
           cerr << '.';
        }
        cerr << flush;
    }

    if (index == length) {      // We are at the end of the loop.
        cerr << "END" << endl;
    }
}

template <class INTEGER_TYPE = int>
struct ConfigParser {
    // Provide a namespace for functions reading configuration values from a
    // string.

    static void parse(vector<INTEGER_TYPE> *result, const string& config);
        // Load into the specified '*result' the comma- and/or
        // whitespace-separated integer values, if any, of (template parameter)
        // type 'INTEGER_TYPE' found at the beginning of the specified 'config'
        // string.  Any part of 'config' that cannot be parsed as such a list
        // of integer values, and any following contents are ignored.
};

template <class INTEGER_TYPE>
void ConfigParser<INTEGER_TYPE>::parse(vector<INTEGER_TYPE> *result,
                                       const string&         config)
{
    // Turn the comma-separated values into a string with one value per line.

    string input(config, result->get_allocator());
    replace(input.begin(), input.end(), ',', ' ');

    // Put the values into a stream.

    stringstream stream(result->get_allocator());
    stream << input;

    INTEGER_TYPE value;
    while (stream >> value) {
        result->push_back(value);
    }
}

//=============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                                 // =========
                                 // struct tm
                                 // =========

ostream& dump(ostream& stream, const tm& time)
    // Output to the specified 'stream' a multi-line textual representation of
    // the specified 'time'.  Return 'stream'.
{
    return stream <<
        "tm = {\t\t\t// " << asctime(&time) <<
          "  tm_sec   = " << time.tm_sec   << '\t' << "[0..61]"
        "\n  tm_min   = " << time.tm_min   << '\t' << "[0..59]"
        "\n  tm_hour  = " << time.tm_hour  << '\t' << "[0..23]"
        "\n  tm_mday  = " << time.tm_mday  << '\t' << "[1..31]"
        "\n  tm_mon   = " << time.tm_mon   << '\t' << "[0..11]"
        "\n  tm_year  = " << time.tm_year  << '\t' << "years since 1900"
        "\n  tm_wday  = " << time.tm_wday  << '\t' << "[0..6] since Sunday"
        "\n  tm_yday  = " << time.tm_yday  << '\t' << "[0..365] since 1/1"
        "\n  tm_isdst = " << time.tm_isdst << '\t' << "[-1..1] ?/no/yes"
        "\n};" << endl;
}

ostream& operator<<(ostream& stream, const tm& time)
    // Output to the specified 'stream' a single-line textual representation of
    // the specified 'time'.  Returns 'stream'.
{
    char buffer[100];
    sprintf(buffer,
        "%3s-%04d/%02d/%02d-%02d/%02d/%02d-(%3d,%s)",
        (time.tm_wday != time.tm_wday % 7 + 7 % 7 ? "???" :
        bdlt::DayOfWeek::toAscii(bdlt::DayOfWeek::Enum(time.tm_wday + 1))),
        1900 + time.tm_year, 1 + time.tm_mon, time.tm_mday,
        time.tm_hour, time.tm_min, time.tm_sec,
        1 + time.tm_yday,
        (time.tm_isdst < 0 ? "?" : time.tm_isdst > 0 ? "Y" : "N"));

    return stream << buffer << flush;
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Converting Between 'bsl::tm' and 'bdlt::Datetime'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// When interfacing with legacy systems, we may encounter calls that represent
// date/time information using the standard 'bsl::tm'. In such cases, we have
// to be able to convert that information to/from a 'bdlt::Datetime' object in
// order to interface with the rest of our systems.
//
// Suppose we have a legacy system that tracks last-access times in terms of
// 'bsl::tm'. We can use the 'convertToTm' and 'convertFromTm' routines from
// this component to convert that information.
//
// First, we define a class, 'MyAccessTracker', that the legacy system uses to
// manage last-access times (eliding the implementation for brevity):
//..
    class MyAccessTracker {
        // This class provides a facility for tracking last access times
        // associated with usernames.

        // LOCAL TYPE
        typedef bsl::map<bsl::string, bsl::tm>  TStringTmMap;

        // DATA
        TStringTmMap                            m_accesses; // map names to
                                                            // accesses

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(MyAccessTracker,
                                       bslma::UsesBslmaAllocator);

        // CREATORS
        explicit MyAccessTracker(bslma::Allocator *basicAllocator = 0);
            // Create an object which will track the last access time ...

        // MANIPULATORS
        void updateLastAccess(const bsl::string&  username,
                              const bsl::tm&      accessTime);
            // Update the last access time for the specified 'username' with
            // the specified 'accessTime'.

        // ACCESSORS
        int getLastAccess(bsl::tm *result, const bsl::string& username) const;
            // Load into the specified 'result' the last access time associated
            // with the specified 'username', if any.  Return 0 on success, and
            // non-0 (with no effect on 'result') if there's no access time
            // associated with 'username'.
    };
//..
// Next, we define a utility to allow us to use 'bdlt::Datetime' with our
// legacy access tracker:
//..
    class MyAccessTrackerUtil {
      public:
        static int getLastAccess(bdlt::Datetime         *result,
                                 const MyAccessTracker&  tracker,
                                 const bsl::string&      username);
            // Load into the specified 'result' the last access time associated
            // with the specified 'username' in the specified 'tracker', if
            // any.  Returns 0 on success, and non-0 (with no effect on
            // 'result') if there's no access time associated with 'username'
            // or the associated access time cannot be converted to
            // 'bdlt::Datetime'.

        static void updateLastAccess(MyAccessTracker       *tracker,
                                     const bsl::string&     username,
                                     const bdlt::Datetime&  accessTime);
            // Update the instance pointed to by the specified 'tracker' by
            // adding the specified 'username' with its associated specified
            // 'accessTime'.
    };
//..
// Then, we implement 'getLastAccess':
//..
                            // -------------------------
                            // class MyAccessTrackerUtil
                            // -------------------------

    int MyAccessTrackerUtil::getLastAccess(bdlt::Datetime         *result,
                                           const MyAccessTracker&  tracker,
                                           const bsl::string&      username)
    {
        BSLS_ASSERT(result);

        bsl::tm legacyAccessTime;

        int rc = tracker.getLastAccess(&legacyAccessTime, username);

        if (rc) {
            return rc;                                                // RETURN
        }

        return bdlt::DatetimeUtil::convertFromTm(result, legacyAccessTime);
    }
//..
// Next, we implement 'updateLastAccess':
//..
    void MyAccessTrackerUtil::updateLastAccess(
                                             MyAccessTracker       *tracker,
                                             const bsl::string&     username,
                                             const bdlt::Datetime&  accessTime)
    {
        BSLS_ASSERT(tracker);

        bsl::tm legacyAccessTime;

        legacyAccessTime = bdlt::DatetimeUtil::convertToTm(accessTime);

        tracker->updateLastAccess(username, legacyAccessTime);
    }
//..
// Finally, we create an access tracker then interact with it using
// 'bdlt::Datetime' times.
//..
    void exerciseTracker()
        // Exercise 'MyAccessTracker' for pedagogical purposes.
    {
        MyAccessTracker accessTracker; // Datetime each user last accessed a
                                       // resource.

        bsl::string    richtofenName = "Baron von Richtofen";
        bdlt::Datetime richtofenDate(1918, 4, 21, 11, 0, 0);
        MyAccessTrackerUtil::updateLastAccess(&accessTracker,
                                               richtofenName,
                                               richtofenDate);

        // ... some time later ....

        bdlt::Datetime lastAccessTime;
        int rc = MyAccessTrackerUtil::getLastAccess(&lastAccessTime,
                                                     accessTracker,
                                                     richtofenName);
        ASSERT(0 == rc);
        ASSERT(lastAccessTime == richtofenDate);

        // Do something with the retrieved date...
    }
//..

// The 'MyAccessTracker' implementation for usage example, elided from
// component header.

                        // ---------------------
                        // class MyAccessTracker
                        // ---------------------

// CREATORS
MyAccessTracker::MyAccessTracker(bslma::Allocator *basicAllocator)
  : m_accesses(bslma::Default::allocator(basicAllocator))
{
}

// MANIPULATORS
void MyAccessTracker::updateLastAccess(const bsl::string&  username,
                                       const bsl::tm&      accessTime)
{
    bsl::pair<TStringTmMap::iterator, bool> result
            = m_accesses.insert(bsl::make_pair(username, accessTime));

    if (!result.second) {
        result.first->second = accessTime;
    }
}

// ACCESSORS
int MyAccessTracker::getLastAccess(bsl::tm            *result,
                                   const bsl::string&  username) const
{
    ASSERT(0 != result);

    TStringTmMap::const_iterator find_result = m_accesses.find(username);

    if (find_result == m_accesses.end()) {
        return 1;                                                     // RETURN
    }

    *result = find_result->second;

    return 0;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
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

        if (verbose) {
            cout << "\nUSAGE EXAMPLE"
                 << "\n=============" << endl;
        }

        exerciseTracker();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONVERT DATETIME TO TM THEN BACK TO DATETIME
        //
        // Concerns:
        //: 1 Round-trip conversion from Datetime to 'struct tm' then back
        //:   is idempotent.
        //
        // Plan:
        //: 1 Use a loop-based approach to verify that pseudo-randomly
        //:   selected, non-repeating datetime values that can be converted to
        //:   and then from struct 'tm' objects result in exactly the same
        //:   object.  Note that the intermediate 'tm' instance is initialized
        //:   to a different "garbage" value on each iteration.
        //:
        //:   Note that the number of iterations and the seed for the
        //:   pseudo-random selection can optionally be controlled by
        //:   specifying those values, separated by commas, as the first
        //:   argument on the command line after the test case number, e.g.
        //:   'bdlt_datetimeutil.t 3 1000,7' would run this test case 3 with
        //:   1000 iterations and a seed of 7.  If the iterations are
        //:   specified, the seed is optional.
        //
        // Testing:
        //   tm convertToTm(const Datetime& datetime);
        //   int convertFromTm(Datetime *result, const tm& timeStruct);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONVERT DATETIME TO TM THEN BACK TO DATETIME"
                          << endl
                          << "============================================"
                          << endl;

        if (verbose) cout << "\nDatetime => struct tm => Datetime." << endl;

        vector<Uint64> parameters;

        if (argc > 2) {
            ConfigParser<Uint64>::parse(&parameters, argv[2]);
        }

        int numTrials = 1000000;

        if (parameters.size() > 0) {
            numTrials = static_cast<int>(parameters[0]);
        }

        const Uint64 SIZE = Uint64(9999) * 12 * 31 * 24 * 60 * 60 + 1;

        const Uint64 Z = 1234567;

        const Uint64 RELATIVE_PRIME = Z * Z ;

        // http://www.wolframalpha.com/ lists the factors of 'SIZE' as:
        //..
        //              1
        //           2689
        //      119515009
        //   321375859201
        //..
        // (computed with Factor[9999 * 12 *31 * 24 * 60 * 60 + 1]).
        //
        // http://www.wolframalpha.com/input/?i=Factor[1234567*1234567] lists
        // the factors of 'RELATIVE_PRIME' as:
        //..
        //             1
        //           127
        //          9721
        //         16129
        //       1234567
        //      94497841
        //     156790009
        //   12001225807
        // 1524155677489
        //..
        //
        // Therefore, 'SIZE' and 'RELATIVE_PRIME' are relatively prime (they
        // have no common factors other than 1), and our pseudo-random walk
        // through the search space will not repeat until the 'SIZE'-eth
        // iteration.

        double percentCovered = 100 * double(numTrials) / SIZE;

        Uint64 startingValue = 0;

        if (parameters.size() > 1) {
            startingValue = parameters[1];
        }

        const Uint64 STARTING_VALUE = startingValue;

        Uint64 pseudoRandomValue = startingValue;

        if (verbose) {
            T_
                P_(numTrials)
                P_(SIZE)
                P_(pseudoRandomValue)
                P(percentCovered)
        }

        int numInvalid = 0;

        for (Uint64 i = 0; static_cast<double>(i) < numTrials; ++i) {
            if (veryVerbose) { loopMeter(i, numTrials); }

            // Ensure that there is no premature repetition; ok first time.

            LOOP_ASSERT(i, (STARTING_VALUE != pseudoRandomValue) != !i);

            const Uint64 currentValue = pseudoRandomValue;
            pseudoRandomValue += RELATIVE_PRIME;
            pseudoRandomValue %= SIZE;

            Uint64 s  = currentValue;
            Uint64 m  = s  / 60;
            Uint64 h  = m  / 60;
            Uint64 d  = h  / 24;
            Uint64 mo = d  / 31;
            Uint64 y  = mo / 12;

            s  %= 60;
            m  %= 60;
            h  %= 24;
            d  %= 31;
            mo %= 12;
            y  %= 9999;

            ++d;
            ++mo;
            ++y;

            bool isValid = bdlt::Datetime::isValid(static_cast<int>(y),
                                                   static_cast<int>(mo),
                                                   static_cast<int>(d),
                                                   static_cast<int>(h),
                                                   static_cast<int>(m),
                                                   static_cast<int>(s),
                                                   0);
            if (veryVeryVerbose) {
                P_(isValid) P_(y) P_(mo) P_(d) P_(h) P_(m) P(s)
            }
            numInvalid += !isValid;

            if (!isValid) { continue; }                         // CONTINUE

            const bdlt::Datetime INITIAL_VALUE(static_cast<int>(y),
                                               static_cast<int>(mo),
                                               static_cast<int>(d),
                                               static_cast<int>(h),
                                               static_cast<int>(m),
                                               static_cast<int>(s));

            tm tmp;
            memset(&tmp, i, sizeof tmp);    // junk
  //v-----------^
    tmp = Util::convertToTm(INITIAL_VALUE);

    bdlt::Datetime result(1, 2, 3, 4, 5, 6, 7);

    LOOP_ASSERT(i, 0 == Util::convertFromTm(&result, tmp));

    LOOP_ASSERT(i, INITIAL_VALUE == result);
  //^-------v
        }
        if (verbose) { loopMeter(numTrials, numTrials); }

        double percentInvalid = 100 * double(numInvalid) / numTrials;

        if (verbose) { T_ P_(numInvalid) P(percentInvalid)}

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BOOTSTRAP: 'convertFromTm'
        //
        // Concerns:
        //: 1 All relevant fields are converted properly.
        //: 2 Irrelevant fields are ignored.
        //: 3 The millisecond field is always set to 0.
        //: 4 Illegal dates are detected.
        //: 5 Out-of-range dates are detected.
        //: 6 Leap seconds at the end of the year are treated as failures.
        //: 7 Bad status implies no change to result.
        //: 8 Time = 24:00:00 converts to 24:00:00.000, but only for 1/1/1.
        //
        // Plan:
        //: 1 Approach:
        //:     o Table-Based Implementation
        //:     o Category Partitioning Data Selection
        //:     o Orthogonal Perturbation:
        //:         o Irrelevant Input Fields
        //:         o Unaltered Initial Values
        //:
        //:   Construct a table in which each relevant input field, status, and
        //:   each output field that can differ from its corresponding input
        //:   are represented in separate columns.  A failure status implies
        //:   that the result is not changed, which is verified within the body
        //:   of the loop on two separate initial values.  Irrelevant fields
        //:   are modified orthogonally within the loop.  Note that
        //:   'millisecond' is necessarily 0 and is tested directly within the
        //:   main loop.
        //:
        //:   Note that the 24:00:00 special case is tested outside of the
        //:   table-based implementation.
        //
        // Testing:
        //   BOOTSTRAP: 'convertFromTm'
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BOOTSTRAP: 'convertFromTm'" << endl
                          << "==========================" << endl;

        if (verbose) cout << "\nstruct tm => Datetime." << endl;

        enum { FAIL = 1 }; // in which case output data is ignored

        static const struct {
            int d_lineNum;      // source line number

            int d_tm_year;      // input
            int d_tm_mon;       // input
            int d_tm_mday;      // input
            int d_tm_hour;      // input
            int d_tm_min;       // input
            int d_tm_sec;       // input

            int d_status;       // value returned by function

            int d_year;         // output (different from input)
            int d_month;        // output (different from input)
            int d_hour;         // output (different from input)

        } DATA[] = {                       // <-- expected ->
                 // <--------- input ------>     <- result ->
            //lin   year mon day hou min sec  s  year mon hou
            //---   ---- --- --- --- --- ---  -  ---- --- ---

                // *** All relevant fields are converted properly ***

            //lin   year mon day hou min sec  s  year mon hou
            //---   ---- --- --- --- --- ---  -  ---- --- ---
            { L_,    100,  5, 15, 12, 45, 56, 0, 2000,  6, 12 }, // base

            { L_,    101,  5, 15, 12, 45, 56, 0, 2001,  6, 12 }, // year
            { L_,    100,  6, 15, 12, 45, 56, 0, 2000,  7, 12 }, // mon
            { L_,    100,  5, 16, 12, 45, 56, 0, 2000,  6, 12 }, // day
            { L_,    100,  5, 15, 13, 45, 56, 0, 2000,  6, 13 }, // hour
            { L_,    100,  5, 15, 12, 46, 56, 0, 2000,  6, 12 }, // min
            { L_,    100,  5, 15, 12, 45, 57, 0, 2000,  6, 12 }, // sec

                // *** Time = 24:00:00 is valid for all dates ***

            //lin   year mon day hou min sec  s  year mon hou
            //---   ---- --- --- --- --- ---  -  ---- --- ---
            { L_,    100,  0,  1, 24,  0,  0, 0, 2000,  1,  0 },

                // *** Out-of-range dates are detected ***

            //lin   year mon day hou min sec  s  year mon hou
            //---   ---- --- --- --- --- ---  -  ---- --- ---
            { L_,  -1900, 11, 31, 24,  0,  0, FAIL, 0,  0,  0 }, // year
            { L_,  -1900, 11, 31, 23, 59, 59, FAIL, 0,  0,  0 },
            { L_,  -1900,  0,  1,  0,  0,  0, FAIL, 0,  0,  0 },

            { L_,  -1899,  0,  1,  0,  0,  0, 0,    1,  1,  0 }, // year
            { L_,  -1899,  0,  1, 24,  0,  0, 0,    1,  1, 24 },
            { L_,  -1899,  0,  1, 23, 59, 59, 0,    1,  1, 23 },

            { L_,   8099,  0,  1,  0,  0,  0, 0, 9999,  1,  0 }, // year
            { L_,   8099, 11, 31, 23, 59, 59, 0, 9999, 12, 23 },
            { L_,   8099, 11, 31, 24,  0,  0, 0, 9999, 12,  0 },

            { L_,   8100, 11, 31, 24,  0,  0, FAIL, 0,  0,  0 }, // year
            { L_,   8100, 11, 31, 23, 59, 59, FAIL, 0,  0,  0 },
            { L_,   8100,  0,  1,  0,  0,  0, FAIL, 0,  0,  0 },

                // *** Illegal dates are detected ***

            //lin   year mon day hou min sec  s  year mon hou
            //---   ---- --- --- --- --- ---  -  ---- --- ---
            { L_,    100, -1, 15,  0,  0,  0, FAIL, 0,  0,  0 }, // month
            { L_,    100,  0, 15,  0,  0,  0, 0, 2000,  1,  0 },
            { L_,    100, 11, 15,  0,  0,  0, 0, 2000, 12,  0 },
            { L_,    100, 12, 15,  0,  0,  0, FAIL, 0,  0,  0 },

            { L_,    100,  2,  0,  0,  0,  0, FAIL, 0,  0,  0 }, // day in
            { L_,    100,  2,  1,  0,  0,  0, 0, 2000,  3,  0 }, // March
            { L_,    100,  2, 31,  0,  0,  0, 0, 2000,  3,  0 },
            { L_,    100,  2, 32,  0,  0,  0, FAIL, 0,  0,  0 },

            { L_,    100,  3, 30,  0,  0,  0, 0, 2000,  4,  0 }, // day in
            { L_,    100,  3, 31,  0,  0,  0, FAIL, 0,  0,  0 }, // April

            { L_,    103,  1, 28,  0,  0,  0, 0, 2003,  2,  0 }, // Feb.
            { L_,    103,  1, 29,  0,  0,  0, FAIL, 0,  0,  0 }, // no-leap

            { L_,    104,  1, 29,  0,  0,  0, 0, 2004,  2,  0 }, // Feb.
            { L_,    100,  1, 30,  0,  0,  0, FAIL, 0,  0,  0 }, // leap-yr

            { L_,      0,  1, 28,  0,  0,  0, 0, 1900,  2,  0 }, // Feb.
            { L_,      0,  1, 29,  0,  0,  0, FAIL, 0,  0,  0 }, // century

            { L_,    500,  1, 29,  0,  0,  0, 0, 2400,  2,  0 }, // Feb.
            { L_,    500,  1, 30,  0,  0,  0, FAIL, 0,  0,  0 }, // 400-yr

            { L_,    100,  6, 15, -1, 12, 34, FAIL, 0,  0,  0 }, // hour
            { L_,    100,  6, 15,  0, 12, 34, 0, 2000,  7,  0 },
            { L_,    100,  6, 15, 23, 12, 34, 0, 2000,  7, 23 },
            { L_,    100,  6, 15, 24,  0,  0, 0, 2000,  7,  0 },
            { L_,    100,  6, 15, 24,  0, 34, FAIL, 0,  0,  0 },
            { L_,    100,  6, 15, 25,  0,  0, FAIL, 0,  0,  0 },

            { L_,    100,  6, 15, 12, -1, 34, FAIL, 0,  0,  0 }, // min
            { L_,    100,  6, 15, 12,  0, 34, 0, 2000,  7, 12 },
            { L_,    100,  6, 15, 12, 59, 34, 0, 2000,  7, 12 },
            { L_,    100,  6, 15, 12, 60, 34, FAIL, 0,  0,  0 },

            { L_,    100,  6, 15, 12, 34, -1, FAIL, 0,  0,  0 }, // sec
            { L_,    100,  6, 15, 12, 34,  0, 0, 2000,  7, 12 },
            { L_,    100,  6, 15, 12, 34, 59, 0, 2000,  7, 12 },
            { L_,    100,  6, 15, 12, 34, 60, 0, 2000,  7, 12 },

                // *** Leap seconds at the end of the year fail ***

            //lin   year mon day hou min sec  s  year mon hou
            //---   ---- --- --- --- --- ---  -  ---- --- ---
            { L_,    100, 11, 31, 23, 59, 59, 0, 2000, 12, 23 }, // sec
            { L_,    100, 11, 31, 23, 59, 60, 0, 2001,  1,  0 },
            { L_,    100, 11, 31, 23, 59, 61, 0, 2001,  1,  0 },
            { L_,    100, 11, 31, 23, 59, 62, 0, 2001,  1,  0 },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        // PERTURBATION 1: Various values for the ignored fields
        //                 of a tm struct

        static const struct {
            int d_lineNum;      // source line number

            int d_tm_wday;      // ignored
            int d_tm_yday;      // ignored
            int d_tm_isdst;     // ignored

        } IGNORED_FIELDS[] = {

                 // <-------- input --------->
            //lin   tm_wday  tm_yday  tm_isdst
            //---   -------  -------  --------
            { L_,         0,       0,        0 }, // base-line

            { L_,   INT_MIN,       0,        0 }, // tm_wday candidates
            { L_,        -1,       0,        0 },
            { L_,         0,       0,        0 },
            { L_,         1,       0,        0 },
            { L_,         2,       0,        0 },
            { L_,         3,       0,        0 },
            { L_,         4,       0,        0 },
            { L_,         5,       0,        0 },
            { L_,         6,       0,        0 },
            { L_,         7,       0,        0 },
            { L_,   INT_MAX,       0,        0 },

            { L_,         0, INT_MIN,        0 }, // tm_yday candidates
            { L_,         0,      -1,        0 },
            { L_,         0,       0,        0 },
            { L_,         0,       1,        0 },
            { L_,         0,     364,        0 },
            { L_,         0,     365,        0 },
            { L_,         0,     366,        0 },
            { L_,         0, INT_MAX,        0 },

            { L_,         0,       0,  INT_MIN }, // tm_isdst candidates
            { L_,         0,       0,       -2 },
            { L_,         0,       0,       -1 },
            { L_,         0,       0,        0 },
            { L_,         0,       0,        1 },
            { L_,         0,       0,        2 },
            { L_,         0,       0,  INT_MAX },
        };

        int tmp = sizeof IGNORED_FIELDS / sizeof *IGNORED_FIELDS;
        const int NUM_IGNORED_FIELDS = tmp;

        // PERTURBATION 2: Arbitrary initial datetime values in order to
        //                 verify "No Change" to 'result' on FAIL.

        const bdlt::Date D_1(1234, 11, 13);
        const bdlt::Date D_2(5678, 12, 14);

        const bdlt::Time T_1(1, 2, 3, 4);
        const bdlt::Time T_2(5, 6, 7, 8);

        const bdlt::Datetime INITIAL_VALUE_1(D_1, T_1);
        const bdlt::Datetime INITIAL_VALUE_2(D_2, T_2);

        const bdlt::Datetime *const INITIAL_VALUES[] = {
            &INITIAL_VALUE_1, &INITIAL_VALUE_2
        };
        tmp = sizeof INITIAL_VALUES / sizeof *INITIAL_VALUES;
        const int NUM_INITIAL_VALUES = tmp;

        // MAIN TEST-TABLE LOOP

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE = DATA[ti].d_lineNum;

            const int TM_YEAR = DATA[ti].d_tm_year;
            const int  TM_MON = DATA[ti].d_tm_mon;
            const int TM_MDAY = DATA[ti].d_tm_mday;
            const int TM_HOUR = DATA[ti].d_tm_hour;
            const int  TM_MIN = DATA[ti].d_tm_min;
            const int  TM_SEC = DATA[ti].d_tm_sec;

            const int  STATUS = DATA[ti].d_status;

            const int    YEAR = DATA[ti].d_year;
            const int   MONTH = DATA[ti].d_month;
            const int     DAY = TM_MDAY;            // same
            const int    HOUR = DATA[ti].d_hour;
            const int  MINUTE = TM_MIN;             // same
            const int  SECOND = TM_SEC;             // same

            // *** The millisecond field is always set to 0 ***

            const int    MSEC = 0;                  // zero

            // PERTURBATION 1: some data fields are irrelevant.

            for (int pi = 0; pi < NUM_IGNORED_FIELDS; ++pi) {
                const int        L2 = IGNORED_FIELDS[pi].d_lineNum;

                // *** Irrelevant fields are ignored ***

                const int  TM_WDAY = IGNORED_FIELDS[pi].d_tm_wday;
                const int  TM_YDAY = IGNORED_FIELDS[pi].d_tm_wday;
                const int TM_ISDST = IGNORED_FIELDS[pi].d_tm_isdst;

                if (veryVerbose) {
                    cout << "\n------------------------------------------"
                            "-----------------------------------" << endl;
                    P_(LINE) P_(L2) P_(TM_YEAR) P_(TM_MON) P(TM_MDAY)
                    T_ T_ P_(TM_HOUR) P_(TM_MIN) P(TM_SEC)
                    T_ T_ P_(TM_WDAY) P_(TM_YDAY) P(TM_ISDST)
                    T_ T_ P_(STATUS) P_(YEAR) P_(MONTH) P(DAY)
                    T_ T_ P_(HOUR) P_(MINUTE) P_(SECOND) P(MSEC)
                }

                tm in;
                in.tm_sec   = TM_SEC;
                in.tm_min   = TM_MIN;
                in.tm_hour  = TM_HOUR;
                in.tm_mday  = TM_MDAY;
                in.tm_mon   = TM_MON;
                in.tm_year  = TM_YEAR;
                in.tm_wday  = TM_WDAY;
                in.tm_yday  = TM_YDAY;
                in.tm_isdst = TM_ISDST;

                const tm INPUT(in);
                if (veryVeryVerbose) { P(INPUT); }

                // PERTURBATION 2: have multiple initial result values.

                for (int vi = 0; vi < NUM_INITIAL_VALUES; ++vi) {
  //v---------------^
    if (veryVeryVerbose) { P_(vi) P(*INITIAL_VALUES[vi]); }

    bdlt::Datetime result = *INITIAL_VALUES[vi];

    if (veryVeryVerbose) { cout << "Before:          "; P(result) }

    int status = Util::convertFromTm(&result, INPUT);

    if (veryVeryVerbose) { cout << " After:          "; P_(result) P(status) }

    LOOP4_ASSERT(LINE, L2, vi, status, !STATUS == !status); // black-box

    if (0 != STATUS) {
        LOOP3_ASSERT(LINE, L2, vi, 0 == YEAR);              // double check
        LOOP3_ASSERT(LINE, L2, vi, 0 == MONTH);             // double check
        LOOP3_ASSERT(LINE, L2, vi, 0 == HOUR);              // double check

        // *** Bad status implies no change to result ***

        LOOP4_ASSERT(LINE, L2, vi, result, *INITIAL_VALUES[vi] == result);
        continue;
    }

    bdlt::Datetime expectedResult(
                               TM_YEAR + 1900, TM_MON + 1,  TM_MDAY,
                               TM_HOUR,        TM_MIN,      min(TM_SEC, 59));

    if (TM_SEC > 59) {
        expectedResult.addSeconds(1);
    }

    if (veryVerbose) { cout << "Expected Result: "; P(expectedResult) }

    LOOP4_ASSERT(LINE, L2, vi, result.year(),
                           expectedResult.year() == result.year());
    LOOP4_ASSERT(LINE, L2, vi, result.month(),
                           expectedResult.month() == result.month());
    LOOP4_ASSERT(LINE, L2, vi, result.day(),
                           expectedResult.day() == result.day());
    LOOP4_ASSERT(LINE, L2, vi, result.hour(),
                           expectedResult.hour() == result.hour());
    LOOP4_ASSERT(LINE, L2, vi, result.minute(),
                           expectedResult.minute() == result.minute());
    LOOP4_ASSERT(LINE, L2, vi, result.second(),
                           expectedResult.second() == result.second());
    LOOP4_ASSERT(LINE, L2, vi, result.millisecond(),
                           expectedResult.millisecond()==result.millisecond());
  //^---------------v
                    } // end for vi
                } // end for pi
            } // end for ti

        {
            bdlt::Datetime result;
            tm             in;

            bsl::memset(&in, sizeof(tm), 0);

            // For date 1/1/1, time 24:00:00 is valid.
            in.tm_sec    = 0;
            in.tm_min    = 0;
            in.tm_hour   = 24;
            in.tm_mday   = 1;
            in.tm_mon    = 0;
            in.tm_year   = -1899;

            int status = Util::convertFromTm(&result, in);

            ASSERT(0 == status);

            if (veryVeryVerbose) {
                cout << in
                     << " after convertFromTm is "
                     << result
                     << ", status is "
                     << status
                     << endl;
            }

            // For every date besides 1/1/1, time 24:00:00 is also valid.
            in.tm_year   = -1898;

            status = Util::convertFromTm(&result, in);

            ASSERT(0 == status);

            if (veryVeryVerbose) {
                cout << in
                     << " after convertFromTm is "
                     << result
                     << ", status is "
                     << status
                     << endl;
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BOOTSTRAP: 'convertToTm'
        //
        // Concerns:
        //: 1 All fields are converted properly.
        //: 2 The millisecond field is ignored.
        //: 3 tm_isdst is always set to -1.
        //: 4 Time = 24:00:00.000 converts to 00:00:00, not to 24:00:00.
        //: 5 The operation never fails.
        //
        // Plan:
        //: 1 Test convertToTm
        //:   o Table-Based Implementation
        //:   o Category Partitioning Data Selection
        //:
        //:   Construct a table in which each input field, and each output
        //:   field that can differ from its corresponding input are
        //:   represented in separate columns.  Note that 'tm_isdst' is
        //:   necessarily -1 and is tested directly within the loop.
        //
        // Testing:
        //   BOOTSTRAP: 'convertToTm'
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BOOTSTRAP: 'convertToTm'" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nDatetime => struct tm." << endl;

        static const struct {
            int d_lineNum;      // source line number

            int d_year;         // input
            int d_month;        // input
            int d_day;          // input
            int d_hour;         // input
            int d_minute;       // input
            int d_second;       // input
            int d_millisecond;  // input

            int d_tm_year;      // output (different from input)
            int d_tm_mon;       // output (different from input)
            int d_tm_hour;      // output (different from input)
            int d_tm_wday;      // output (calculated)
            int d_tm_yday;      // output (calculated)
        } DATA[] = {                            // <- expected output ->
               // <--------- input ----------->    <----- result ------>
            //lin year mon day hou min sec msec    year mon hou wda yday
            //--- ---- --- --- --- --- --- ----    ---- --- --- --- ----

                // *** Day, Day-Of-Week, Day-Of-Year, Month, Year ***

            //lin year mon day hou min sec msec    year mon hou wda yday
            //--- ---- --- --- --- --- --- ----    ---- --- --- --- ----
            { L_, 1898, 12, 31,  0,  0,  0,   0,     -2, 11,  0,  6, 364 },
            { L_, 1899,  1,  1,  0,  0,  0,   0,     -1,  0,  0,  0,   0 },
            { L_, 1899,  1,  2,  0,  0,  0,   0,     -1,  0,  0,  1,   1 },
            { L_, 1899,  1,  3,  0,  0,  0,   0,     -1,  0,  0,  2,   2 },

            { L_, 1899,  1, 31,  0,  0,  0,   0,     -1,  0,  0,  2,  30 },
            { L_, 1899,  2,  1,  0,  0,  0,   0,     -1,  1,  0,  3,  31 },
            { L_, 1899,  2, 28,  0,  0,  0,   0,     -1,  1,  0,  2,  58 },
            { L_, 1899,  3,  1,  0,  0,  0,   0,     -1,  2,  0,  3,  59 },

            { L_, 1899, 11, 30,  0,  0,  0,   0,     -1, 10,  0,  4, 333 },
            { L_, 1899, 12,  1,  0,  0,  0,   0,     -1, 11,  0,  5, 334 },
            { L_, 1899, 12, 31,  0,  0,  0,   0,     -1, 11,  0,  0, 364 },
            { L_, 1900,  1,  1,  0,  0,  0,   0,      0,  0,  0,  1,   0 },

            { L_, 1900,  1,  2,  0,  0,  0,   0,      0,  0,  0,  2,   1 },
            { L_, 1900,  1,  3,  0,  0,  0,   0,      0,  0,  0,  3,   2 },

            { L_, 1900,  1, 31,  0,  0,  0,   0,      0,  0,  0,  3,  30 },
            { L_, 1900,  2,  1,  0,  0,  0,   0,      0,  1,  0,  4,  31 },
            { L_, 1900,  2, 28,  0,  0,  0,   0,      0,  1,  0,  3,  58 },
            { L_, 1900,  3,  1,  0,  0,  0,   0,      0,  2,  0,  4,  59 },

            { L_, 1900, 11, 30,  0,  0,  0,   0,      0, 10,  0,  5, 333 },
            { L_, 1900, 12,  1,  0,  0,  0,   0,      0, 11,  0,  6, 334 },
            { L_, 1900, 12, 31,  0,  0,  0,   0,      0, 11,  0,  1, 364 },
            { L_, 1901,  1,  1,  0,  0,  0,   0,      1,  0,  0,  2,   0 },

            { L_, 1901,  1,  2,  0,  0,  0,   0,      1,  0,  0,  3,   1 },
            { L_, 1901,  1,  3,  0,  0,  0,   0,      1,  0,  0,  4,   2 },

            { L_, 1901,  1, 31,  0,  0,  0,   0,      1,  0,  0,  4,  30 },
            { L_, 1901,  2,  1,  0,  0,  0,   0,      1,  1,  0,  5,  31 },
            { L_, 1901,  2, 28,  0,  0,  0,   0,      1,  1,  0,  4,  58 },
            { L_, 1901,  3,  1,  0,  0,  0,   0,      1,  2,  0,  5,  59 },

            { L_, 1901, 11, 30,  0,  0,  0,   0,      1, 10,  0,  6, 333 },
            { L_, 1901, 12,  1,  0,  0,  0,   0,      1, 11,  0,  0, 334 },
            { L_, 1901, 12, 31,  0,  0,  0,   0,      1, 11,  0,  2, 364 },
            { L_, 1902,  1,  1,  0,  0,  0,   0,      2,  0,  0,  3,   0 },

            { L_, 1999, 12, 31,  0,  0,  0,   0,     99, 11,  0,  5, 364 },
            { L_, 2000,  1,  1,  0,  0,  0,   0,    100,  0,  0,  6,   0 },
            { L_, 2000,  1,  2,  0,  0,  0,   0,    100,  0,  0,  0,   1 },
            { L_, 2000,  1,  3,  0,  0,  0,   0,    100,  0,  0,  1,   2 },

            { L_, 2000,  1, 31,  0,  0,  0,   0,    100,  0,  0,  1,  30 },
            { L_, 2000,  2,  1,  0,  0,  0,   0,    100,  1,  0,  2,  31 },
            { L_, 2000,  2, 29,  0,  0,  0,   0,    100,  1,  0,  2,  59 },
            { L_, 2000,  3,  1,  0,  0,  0,   0,    100,  2,  0,  3,  60 },

            { L_, 2000, 11, 30,  0,  0,  0,   0,    100, 10,  0,  4, 334 },
            { L_, 2000, 12,  1,  0,  0,  0,   0,    100, 11,  0,  5, 335 },
            { L_, 2000, 12, 31,  0,  0,  0,   0,    100, 11,  0,  0, 365 },
            { L_, 2001,  1,  1,  0,  0,  0,   0,    101,  0,  0,  1,   0 },

                   // *** Hour, Minute, Second, Milliseconds ***

            //lin year mon day hou min sec msec    year mon hou wda yday
            //--- ---- --- --- --- --- --- ----    ---- --- --- --- ----
            { L_, 2024,  2, 29,  0,  0,  0,   0,    124,  1,  0,  4,  59 },
            { L_, 2024,  2, 29,  1,  0,  0,   0,    124,  1,  1,  4,  59 },
            { L_, 2024,  2, 29, 23,  0,  0,   0,    124,  1, 23,  4,  59 },

            { L_, 2024,  2, 29,  0,  1,  0,   0,    124,  1,  0,  4,  59 },
            { L_, 2024,  2, 29,  0, 59,  0,   0,    124,  1,  0,  4,  59 },

            { L_, 2024,  2, 29,  0,  0,  1,   0,    124,  1,  0,  4,  59 },
            { L_, 2024,  2, 29,  0,  0, 59,   0,    124,  1,  0,  4,  59 },

            { L_, 2024,  2, 29,  0,  0,  1,   0,    124,  1,  0,  4,  59 },
            { L_, 2024,  2, 29,  0,  0, 59,   0,    124,  1,  0,  4,  59 },

            { L_, 2024,  2, 29,  0,  0,  0,   1,    124,  1,  0,  4,  59 },
            { L_, 2024,  2, 29,  0,  0,  0,  60,    124,  1,  0,  4,  59 },
            { L_, 2024,  2, 29,  0,  0,  0, 999,    124,  1,  0,  4,  59 },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        // MAIN TEST-TABLE LOOP

        for (int ti = 0; ti < NUM_DATA; ++ti) {

            const int    LINE = DATA[ti].d_lineNum;
            const int    YEAR = DATA[ti].d_year;
            const int   MONTH = DATA[ti].d_month;
            const int     DAY = DATA[ti].d_day;
            const int    HOUR = DATA[ti].d_hour;
            const int  MINUTE = DATA[ti].d_minute;
            const int  SECOND = DATA[ti].d_second;
            const int    MSEC = DATA[ti].d_millisecond;

            const int TM_YEAR = DATA[ti].d_tm_year;
            const int  TM_MON = DATA[ti].d_tm_mon;
            const int TM_HOUR = DATA[ti].d_tm_hour;
            const int TM_WDAY = DATA[ti].d_tm_wday;
            const int TM_YDAY = DATA[ti].d_tm_yday;

  //v-------^
    if (veryVerbose) {
        cout << "\n--------------------------------------------------" << endl;
        P_(LINE) P_(YEAR) P_(MONTH) P(DAY)
        T_ P_(HOUR) P_(MINUTE) P_(SECOND) P(MSEC)
        T_ P_(TM_YEAR) P_(TM_MON) P_(TM_WDAY) P(TM_YDAY)
    }
    const bdlt::Date DATE(YEAR, MONTH, DAY);
    const bdlt::Time TIME(HOUR, MINUTE, SECOND, MSEC);

    const bdlt::Datetime INPUT(DATE, TIME);
    if (veryVeryVerbose) { P(INPUT); }

    tm result;
    memset(&result, char(ti), sizeof result);   // junk

    if (veryVeryVerbose) { cout << "Before: "; P(result.tm_isdst); }
    result = Util::convertToTm(INPUT);
    if (veryVeryVerbose) { cout << "After: "; P(result.tm_isdst); }
    if (veryVerbose) { dump(cout, result); }

    LOOP_ASSERT(LINE, MONTH - 1 == TM_MON);     // double check
    LOOP_ASSERT(LINE, HOUR % 24 == TM_HOUR);    // double check
    LOOP_ASSERT(LINE, YEAR - 1900 == TM_YEAR);  // double check

    LOOP3_ASSERT(LINE, SECOND, result.tm_sec, SECOND == result.tm_sec);
    LOOP3_ASSERT(LINE, MINUTE, result.tm_min, MINUTE == result.tm_min);
    LOOP3_ASSERT(LINE, TM_HOUR, result.tm_hour, TM_HOUR == result.tm_hour);
    LOOP3_ASSERT(LINE, DAY, result.tm_mday, DAY == result.tm_mday);
    LOOP3_ASSERT(LINE, TM_MON, result.tm_mon, TM_MON == result.tm_mon);
    LOOP3_ASSERT(LINE, TM_YEAR, result.tm_year, TM_YEAR == result.tm_year);
    LOOP3_ASSERT(LINE, TM_WDAY, result.tm_wday, TM_WDAY == result.tm_wday);
    LOOP3_ASSERT(LINE, TM_YDAY, result.tm_yday, TM_YDAY == result.tm_yday);

    LOOP2_ASSERT(LINE, result.tm_isdst, -1 == result.tm_isdst);
  //^-------v
        } // end for ti
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
