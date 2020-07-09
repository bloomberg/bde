// bdlt_timetable.t.cpp                                               -*-C++-*-
#include <bdlt_timetable.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_dayofweek.h>
#include <bdlt_time.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_issame.h>

#include <bsls_asserttest.h>
#include <bsls_review.h>

#include <bsl_cctype.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iosfwd.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_unordered_map.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// A 'bdlt::Timetable' is a value-semantic type that represents a timetable of
// state transitions.  Two timetables have the same value if they have the same
// valid range (or are both empty), the same number of transitions, and each
// corresponding pair of transitions has the same value.
//
// We have chosen the primary manipulators for 'bdlt::Timetable' to be
// 'addTransition(const Datetime& datetime, int code)',
// 'setInitialTransitionCode', 'setValidRange', and 'reset'.
//
// We have chosen the basic accessors for 'bdlt::Timetable' to be 'allocator',
// 'firstDate', 'initialTransitionCode', 'lastDate', 'length', and
// 'transitionCodeInEffect(const Datetime& datetime)'.
// ----------------------------------------------------------------------------
// 'TimetableTransition'
// [16] const Datetime& TT:datetime() const;
// [16] int TT:transitionCode() const;
// [ 5] ostream& TT::print(stream, level, spacesPerLevel) const;
//
// ITERATOR
// [16] Timetable::const_iterator();
// [16] Timetable::const_iterator(const TCI& original);
// [16] ~Timetable::const_iterator();
// [16] TCI& operator=(const TCI& rhs);
// [16] TCI& TCI::operator++();
// [16] TCI& TCI::operator--();
// [16] const TimetableTransition& TCI::operator*() const;
// [16] const TimetableTransition *TCI::operator->() const;
// [16] TCI operator++(TCI& iterator, int);
// [16] TCI operator--(TCI& iterator, int);
// [16] bool operator==(const TCI& lhs, const TCI& rhs);
// [16] bool operator!=(const TCI& lhs, const TCI& rhs);
//
// CREATORS
// [ 2] Timetable(bslma::Allocator *basicAllocator = 0);
// [12] Timetable(firstDate, lastDate, initTransCode = UNSET, ba = 0);
// [ 7] Timetable(const Timetable& original, ba = 0);
// [ 2] ~Timetable();
//
// MANIPULATORS
// [ 9] Timetable& operator=(const Timetable& rhs);
// [13] void addTransition(date, time, code);
// [ 2] void addTransition(const Datetime& datetime, int code);
// [13] void addTransitions(dow, time, code, firstDate, lastDate);
// [14] void removeAllTransitions();
// [16] void removeTransition(const Date& date, const Time& time);
// [16] void removeTransition(const Datetime& datetime);
// [15] void removeTransitions(const Date& date);
// [16] void removeTransitions(dayOfWeek, time, firstDate, lastDate);
// [ 2] void reset();
// [ 2] void setInitialTransitionCode(int code);
// [ 2] void setValidRange(const Date& firstDate, const Date& lastDate);
// [ 8] void swap(Timetable& other);
//
// ACCESSORS
// [17] Timetable::const_iterator begin() const;
// [17] Timetable::const_iterator end() const;
// [ 4] const Date& firstDate() const;
// [ 4] int initialTransitionCode() const;
// [11] bool isInRange(const Date& date) const;
// [ 4] const Date& lastDate() const;
// [ 4] int length() const;
// [11] int transitionCodeInEffect(const Date&, const Time&) const;
// [ 4] int transitionCodeInEffect(const Datetime& datetime) const;
// [ 4] bslma::Allocator *allocator() const;
// [ 5] ostream& print(ostream& stream, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 6] bool operator==(lhs, rhs);
// [ 6] bool operator!=(lhs, rhs);
// [ 5] ostream& operator<<(ostream&, const Timetable&);
//
// FREE FUNCTIONS
// [18] void hashAppend(HASHALG&, const Timetable&);
// [ 8] void swap(Timetable& a, Timetable& b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [19] USAGE EXAMPLE
// [10] TESTING BDEX STREAMING
// [ 3] Obj& gg(Obj *object, const char *spec);
// [ 3] int ggg(Obj *object, const char *spec, bool vF);
// ============================================================================

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

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlt::Timetable                 Obj;
typedef bdlt::Timetable::const_iterator Iterator;

static const int UNSET = bdlt::Timetable::k_UNSET_TRANSITION_CODE;

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

static const char *DEFAULT_SPECS[] = {
    "",
    "!20180101_20180131",
    "!20180102_20180131",
    "!20180101_20180131 0@20180102_0123",
    "!20180101_20180131 1@20180102_0123",
    "!20180101_20180131 0@20180102_0123 1@20180102_0125",
    "!20180101_20180131 0@20180102_0123 2@20180102_0125",
    "!20180101_20180131 0@20180102_0123 1@20180102_0125 2@20180103_0629",
    0 // Null string required as last element.
};

// 'DEFAULT_SPECS_NUM_TRANSITIONS' provides the number of transitions in the
// corresponding 'DEFAULT_SPECS', necessary for iterator testing.

static const int DEFAULT_SPECS_NUM_TRANSITION[] = {
    0,
    0,
    0,
    1,
    1,
    2,
    2,
    3
};

// ============================================================================
//               GENERATOR FUNCTIONS 'gg' AND 'ggg' FOR TESTING
// ----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.
//
///LANGUAGE SPECIFICATION
//-----------------------
// <SPEC>            ::= <EMPTY> | <LIST>
//
// <EMPTY>           ::=
//
// <LIST>            ::= <ITEM> | <ITEM><LIST>
//
// <ITEM>            ::= <INIT_CODE> | <RANGE><TRANSITION_LIST> | <REMOVE_ALL>
//
// <INIT_CODE>       ::= I<CODE>
//                           // Represents a code to be used with
//                           //'setInitialTransitionCode'.
//
// <RANGE>           ::= !<DATE>_<DATE>
//                           // Represents a 'firstDate' and 'lastDate' to be
//                           // used with 'setValidRange'.
//
// <REMOVE_ALL>      ::= R   // Represents an invocation of 'reset'.
//
// <TRANSITION_LIST> ::= <EMPTY> | <TRANSITION> | <TRANSITION><TRANSITION_LIST>
//
// <TRANSITION>      ::= <CODE>@<DATETIME>
//                           // Represents an invocation of 'addTransition'
//                           // with the provided datetime and transition code.
//
// <CODE>            ::= non-negative integer value
//
// <DATE>            ::= yyyymmdd
//
// <DATETIME>        ::= yyyymmdd_hhmm

enum { e_SUCCESS = -1, e_FAILURE = 1 };

void skipOptionalWhitespace(const char **endPos, const char *input)
    // Load into the specified 'endPos' the first position that is '!isspace',
    // or the end of the string if no such position is found, starting from the
    // specified 'input'.
{
    ASSERT(endPos);
    ASSERT(input);

    while (*input && isspace(*input)) {
        ++input;
    }

    *endPos = input;
}

int parseUint(const char **endPos, int *result, const char *input)
    // Load into the specified 'result' the value of the unsigned integer
    // starting at the specified 'input' position.  Load into the specified
    // 'endPos' the first position that is after the parsed value.  Return 0 if
    // the parse succeeds, and a non-zero value otherwise.
{
    ASSERT(endPos);
    ASSERT(result);
    ASSERT(input);

    if (!isdigit(*input)) {
        *endPos = input;
        return e_FAILURE;                                             // RETURN
    }

    int tmp = 0;
    while (isdigit(*input)) {
        tmp *= 10;
        tmp += *input - '0';
        ++input;
    }
    *result = tmp;

    *endPos = input;
    return e_SUCCESS;
}

int parseDate(const char **endPos, bdlt::Date *result, const char *input)
    // Load into the specified 'result' the value of the 'bdlt::Date' starting
    // at the specified 'input' position.  Load into the specified 'endPos' the
    // first position that is after the parsed value.  Return 0 if the parse
    // succeeds, and a non-zero value otherwise.
{
    ASSERT(endPos);
    ASSERT(result);
    ASSERT(input);

    int value;

    if (e_FAILURE == parseUint(endPos, &value, input)) {
        return e_FAILURE;                                             // RETURN
    }

    int y = value / 10000;
    int m = (value / 100) % 100;
    int d = value % 100;
    if (!bdlt::Date::isValidYearMonthDay(y, m, d)) {
        *endPos = input;
        return e_FAILURE;                                             // RETURN
    }

    result->setYearMonthDay(y, m, d);

    return e_SUCCESS;
}

int parseRange(const char **endPos,
               bdlt::Date  *firstDate,
               bdlt::Date  *lastDate,
               const char  *input)
    // Load into the specified 'firstDate' and 'lastDate' the values of the
    // 'bdlt::Date' starting at the specified 'input' position.  Load into the
    // specified 'endPos' the first position that is after the parsed value.
    // Return 0 if the parse succeeds, and a non-zero value otherwise.
{
    ASSERT(endPos);
    ASSERT(firstDate);
    ASSERT(lastDate);
    ASSERT(input);

    if ('!' != *input) {
        *endPos = input;
        return e_FAILURE;                                             // RETURN
    }
    ++input;

    if (e_FAILURE == parseDate(endPos, firstDate, input)) {
        return e_FAILURE;                                             // RETURN
    }
    input = *endPos;

    if ('_' != *input) {
        *endPos = input;
        return e_FAILURE;                                             // RETURN
    }
    ++input;

    if (e_FAILURE == parseDate(endPos, lastDate, input)) {
        return e_FAILURE;                                             // RETURN
    }

    return e_SUCCESS;
}

int parseTransition(const char     **endPos,
                    int             *code,
                    bdlt::Datetime  *datetime,
                    const char      *input)
    // Load into the specified 'code' and 'datetime' the value of the
    // transition code and 'bdlt::Datetime' starting at the specified 'input'
    // position.  Load into the specified 'endPos' the first position that is
    // after the parsed value.  Return 0 if the parse succeeds, and a non-zero
    // value otherwise.
{
    ASSERT(endPos);
    ASSERT(code);
    ASSERT(datetime);
    ASSERT(input);

    if (e_FAILURE == parseUint(endPos, code, input)) {
        return e_FAILURE;                                             // RETURN
    }
    input = *endPos;

    if ('@' != *input) {
        *endPos = input;
        return e_FAILURE;                                             // RETURN
    }
    ++input;

    bdlt::Date date;

    if (e_FAILURE == parseDate(endPos, &date, input)) {
        return e_FAILURE;                                             // RETURN
    }
    input = *endPos;

    if ('_' != *input) {
        *endPos = input;
        return e_FAILURE;                                             // RETURN
    }
    ++input;

    int value;

    if (e_FAILURE == parseUint(endPos, &value, input)) {
        return e_FAILURE;                                             // RETURN
    }

    int h = value / 100;
    int m = value % 100;
    if (h < 0 || h > 23 || m < 0 || m > 59) {
        *endPos = input;
        return e_FAILURE;                                             // RETURN
    }

    datetime->setDatetime(date, h, m);

    return e_SUCCESS;
}

int ggg(Obj *object, const char *spec, bool verboseFlag = true)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulators
    // 'addTransition(const Datetime& datetime, int code)',
    // 'setInitialTransitionCode', 'setValidRange', and 'reset'.  Optionally
    // specify a 'false' 'verboseFlag' to suppress 'spec' syntax error
    // messages.  Return the index of the first invalid character, and a
    // negative value otherwise.  Note that this function is used to implement
    // 'gg' as well as allowing for verification of syntax error detection.
{
    ASSERT(object);
    ASSERT(spec);

    const char *input  = spec;
    const char *endPos = input;

    while (*input) {
        skipOptionalWhitespace(&endPos, input);
        input = endPos;

        if ('!' == *input) {
            bdlt::Date firstDate;
            bdlt::Date lastDate;

            if (e_SUCCESS == parseRange(&endPos,
                                        &firstDate,
                                        &lastDate,
                                        input)) {
                if (lastDate < firstDate) {
                    break;
                }

                object->setValidRange(firstDate, lastDate);

                input = endPos;

                bool failed = false;

                if ('\0' != *input && ' ' != *input) {
                    failed = true;
                }

                while (' ' == *input && !failed) {
                    skipOptionalWhitespace(&endPos, input);
                    input = endPos;

                    if (isdigit(*input)) {
                        int            code;
                        bdlt::Datetime datetime;

                        if (e_SUCCESS == parseTransition(&endPos,
                                                         &code,
                                                         &datetime,
                                                         input)) {
                            if (   object->firstDate() <= datetime.date()
                                && object->lastDate()  >= datetime.date()) {
                                object->addTransition(datetime, code);

                                input = endPos;

                                if ('\0' != *input && ' ' != *input) {
                                    failed = true;
                                }
                            }
                            else {
                                failed = true;
                            }
                        }
                        else {
                            failed = true;
                        }
                    }
                }

                if (failed) {
                    break;
                }
            }
            else {
                break;
            }
        }
        else if ('I' == *input) {
            if (!isdigit(*(input + 1))) {
                break;
            }
            ++input;
            endPos = input;

            int value;

            if (e_FAILURE == parseUint(&endPos, &value, input)) {
                break;
            }
            input = endPos;

            object->setInitialTransitionCode(value);

            if ('\0' != *input && ' ' != *input) {
                break;
            }
        }
        else if ('R' == *input) {
            object->reset();
            ++input;
            endPos = input;

            if ('\0' != *input && ' ' != *input) {
                break;
            }
        }
        else {
            if (verboseFlag) {
                cout << "Error: unrecognized character." << endl;
            }
            break;
        }

        input = endPos; // So far so good; update input and continue.
    } // end while not done

    if (*input) {
        int idx = static_cast<int>(endPos - spec);
        if (verboseFlag) {
            cout << "An error occurred near character ('" << spec[idx]
                 << "') in spec \"" << spec << "\" at position " << idx
                 << '.' << endl;
        }

        // Discontinue processing this spec.

        return idx;                                                   // RETURN
    }

    // All input was consumed.

    return -1;
}

Obj& gg(Obj *object, const char *spec)
    // Return, by reference, the specified 'object' with its value adjusted
    // according to the specified 'spec' according to the custom language
    // described above.
{
    ASSERT(object);
    ASSERT(spec);
    ASSERT(ggg(object, spec) < 0);

    return *object;
}

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator         defaultAllocator("default",
                                                  veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 19: {
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
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: 'Exchange Schedule'
/// - - - - - - - - - - - - - - -
// Suppose we want to track the open and close times for an exchange.  Most
// Mondays (and Tuesdays, Wednesdays, etc.) will have the same schedule,
// although some may differ.  We can use 'bdlt::Timetable' to efficiently store
// this data.
//
// First, we create an instance of 'bdlt::Timetable' with the desired valid
// range:
//..
    bdlt::Timetable timetable(bdlt::Date(2018, 1, 1),
                              bdlt::Date(2018, 12, 31));
//..
// Then, we define the codes for start-of-trading and end-of-trading and
// populate the typical transitions into the timetable:
//..
    const int k_TRADING    = 0;
    const int k_NO_TRADING = 1;

    timetable.setInitialTransitionCode(k_NO_TRADING);

    for (int i = 0; i < 5; ++ i) {
        timetable.addTransitions(static_cast<bdlt::DayOfWeek::Enum>(
                                                   bdlt::DayOfWeek::e_MON + i),
                                 bdlt::Time(8, 30),
                                 k_TRADING,
                                 timetable.firstDate(),
                                 timetable.lastDate());

        timetable.addTransitions(static_cast<bdlt::DayOfWeek::Enum>(
                                                   bdlt::DayOfWeek::e_MON + i),
                                 bdlt::Time(16, 30),
                                 k_NO_TRADING,
                                 timetable.firstDate(),
                                 timetable.lastDate());
    }
//..
// Next, we add a holiday on January 19, 2018:
//..
    timetable.removeTransitions(bdlt::Date(2018, 1, 19));
//..
// Then, we add a half-day on November 23, 2018:
//..
    timetable.addTransition(bdlt::Datetime(2018, 11, 23, 12, 30),
                            k_NO_TRADING);

    timetable.removeTransition(bdlt::Datetime(2018, 11, 23, 16, 30));
//..
// Finally, we verify the transition code in effect at a few datetimes.
//..
    ASSERT(k_NO_TRADING == timetable.transitionCodeInEffect(
                                        bdlt::Datetime(2018,  1, 15,  8,  0)));

    ASSERT(k_TRADING    == timetable.transitionCodeInEffect(
                                        bdlt::Datetime(2018,  1, 15,  8, 30)));

    ASSERT(k_TRADING    == timetable.transitionCodeInEffect(
                                        bdlt::Datetime(2018,  1, 15, 16,  0)));

    ASSERT(k_NO_TRADING == timetable.transitionCodeInEffect(
                                        bdlt::Datetime(2018,  1, 15, 16, 30)));

    ASSERT(k_NO_TRADING == timetable.transitionCodeInEffect(
                                        bdlt::Datetime(2018, 11, 23,  8,  0)));

    ASSERT(k_TRADING    == timetable.transitionCodeInEffect(
                                        bdlt::Datetime(2018, 11, 23,  8, 30)));

    ASSERT(k_TRADING    == timetable.transitionCodeInEffect(
                                        bdlt::Datetime(2018, 11, 23, 12,  0)));

    ASSERT(k_NO_TRADING == timetable.transitionCodeInEffect(
                                        bdlt::Datetime(2018, 11, 23, 12, 30)));
//..
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING: hashAppend
        //
        // Concerns:
        //: 1 Hope that different inputs hash differently
        //: 2 Verify that equal inputs hash identically
        //: 3 Works for 'const' and non-'const' values
        //
        // Plan:
        //: 1 Use a table specifying a set of distinct objects, verify that
        //:   hashes of equivalent objects match and hashes on unequal objects
        //:   do not.
        //
        // Testing:
        //    void hashAppend(HASHALG& hashAlg, const Timetable&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING 'hashAppend'"
                          << "\n====================\n";

        typedef ::BloombergLP::bslh::Hash<>   Hasher;
        typedef Hasher::result_type           HashType;
        Hasher                                hasher;
        static const char                   **SPECS = DEFAULT_SPECS;

        if (verbose) {
            cout << "\nCompare hashes of every value with every value.\n";
        }

        for (int ti = 0; SPECS[ti]; ++ti) {
            for (int tj = 0; SPECS[tj]; ++tj) {
                Obj mX;  const Obj& X = gg(&mX, SPECS[ti]);
                Obj mY;  const Obj  Y = gg(&mY, SPECS[tj]);

                HashType hX = hasher(X);
                HashType hY = hasher(Y);

                if (veryVerbose) { T_ P_(ti) P_(tj) P_(hX) P(hY) }

                LOOP4_ASSERT(ti, tj, hX, hY,  (ti == tj) == (hX == hY));
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // CONST ITERATOR BASIC METHODS
        //   Ensure we can create a 'const' iterator, inspect the value at
        //   the iterator, and destroy the iterator.
        //
        // Concerns:
        //: 1 The equality, assignment, and copy construct methods work as
        //:   expected.
        //:
        //: 2 The other basic methods work correctly.
        //:
        //: 3 The destructor functions properly.
        //:
        //: 4 The iterator returned by 'begin()' is valid and constant
        //:   regardless of the length of the array, and is equal to the
        //:   iterator returned by 'end()' if the array is empty.
        //:
        //: 5 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct arrays and verify the results of the equality,
        //:   assignment, and copy construct methods.  (C-1)
        //:
        //: 2 Use the generator function to produce objects of arbitrary state.
        //:   Verify the basic methods against the expected values.  (C-2)
        //:
        //: 3 Allow the iterators to go out-of-scope; there is nothing to
        //:   explicitly verify.  (C-3)
        //:
        //: 4 Create an empty array and verify 'begin() == end()'.  Use the
        //:   generator function to produce objects of arbitrary state and
        //:   assign the empty array to this array.  Verify the iterator
        //:   returned by 'begin()' on this array has the same value as the
        //:   one obtained while the array was empty.  (C-4)
        //:
        //: 5 Verify defensive checks are triggered for invalid values.  (C-5)
        //
        // Testing:
        //   Timetable::const_iterator begin() const;
        //   Timetable::const_iterator end() const;
        //   Timetable::const_iterator();
        //   Timetable::const_iterator(const TCI& original);
        //   ~Timetable::const_iterator();
        //   TCI& operator=(const TCI& rhs);
        //   TCI& TCI::operator++();
        //   TCI& TCI::operator--();
        //   const TimetableTransition& TCI::operator*() const;
        //   const TimetableTransition *TCI::operator->() const;
        //   TCI operator++(TCI& iterator, int);
        //   TCI operator--(TCI& iterator, int);
        //   bool operator==(const TCI& lhs, const TCI& rhs);
        //   bool operator!=(const TCI& lhs, const TCI& rhs);
        //   const Datetime& TT:datetime() const;
        //   int TT:code() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONST ITERATOR BASIC METHODS" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting equality operators."
                          << endl;
        {
            Obj mX;  const Obj& X = gg(&mX,
                                       "!20171225_20171231 0@20171226_1217");
            Obj mY;  const Obj& Y = gg(&mY,
                                       "!20171225_20171231 0@20171226_1217");

            Iterator iter1 = X.begin();
            Iterator iter2 = X.end();
            Iterator iter3 = Y.begin();
            Iterator iter4 = Y.end();

            ASSERT(iter1 == iter1);
            ASSERT(iter1 != iter2);
            ASSERT(iter1 != iter3);
            ASSERT(iter1 != iter4);

            ASSERT(iter2 != iter1);
            ASSERT(iter2 == iter2);
            ASSERT(iter2 != iter3);
            ASSERT(iter2 != iter4);

            ASSERT(iter3 != iter1);
            ASSERT(iter3 != iter2);
            ASSERT(iter3 == iter3);
            ASSERT(iter3 != iter4);

            ASSERT(iter4 != iter1);
            ASSERT(iter4 != iter2);
            ASSERT(iter4 != iter3);
            ASSERT(iter4 == iter4);

            mX.addTransition(bdlt::Datetime(2017, 12, 27, 1), 0);
            iter2 = mX.end();
            ASSERT(iter1 != iter2);
            ++iter1;
            ASSERT(iter1 != iter2);
            --iter2;
            ASSERT(iter1 == iter2);
        }

        if (verbose) cout << "\nTesting assignment and copy construct."
                          << endl;
        {
            Obj mX;  const Obj& X = gg(&mX,
                                       "!20171225_20171231 0@20171226_1217");

            Iterator iter1 = X.begin();
            Iterator iter2 = X.end();
            ASSERT(iter1 != iter2);

            {
                Iterator iter;

                ASSERT(iter != iter1);
                ASSERT(iter != iter2);

                iter = iter1;

                ASSERT(iter == iter1);
                ASSERT(iter != iter2);

                iter = iter2;

                ASSERT(iter != iter1);
                ASSERT(iter == iter2);
            }
            {
                Iterator iter(iter1);

                ASSERT(iter == iter1);
                ASSERT(iter != iter2);
            }
            {
                Iterator iter(iter2);

                ASSERT(iter != iter1);
                ASSERT(iter == iter2);
            }
        }

        if (verbose) cout << "\nTesting basic methods."
                          << endl;
        {
            for (int ti = 0; DEFAULT_SPECS[ti]; ++ti) {
                const char *const SPEC = DEFAULT_SPECS[ti];
                const int         LEN  = DEFAULT_SPECS_NUM_TRANSITION[ti];

                Obj mX;  const Obj& X = gg(&mX, SPEC);

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                // Verify assignment return value.

                Iterator iter;
                ASSERT(&iter == &(iter = X.begin()));

                int previousTransitionCode = X.initialTransitionCode();

                for (int i = 0; i < LEN; ++i) {
                    // Verify 'operator*' and 'operator->'.

                    bdlt::Datetime BEFORE = iter->datetime();
                    BEFORE.addTime(0, 0, 0, 0, -1);

                    LOOP_ASSERT(i, *iter == *iter.operator->());
                    LOOP_ASSERT(i, iter->code() == X.transitionCodeInEffect(
                                                            iter->datetime()));
                    LOOP_ASSERT(i, previousTransitionCode
                                          == X.transitionCodeInEffect(BEFORE));

                    previousTransitionCode = X.transitionCodeInEffect(
                                                             iter->datetime());

                    // Verify 'operator++' and 'operator--'.

                    Iterator iterOrig(iter);

                    Iterator iter2(++iter);
                    ASSERT(iter2 == iter);

                    Iterator iter3(--iter2);
                    ASSERT(iter2 == iterOrig);
                    ASSERT(iter3 == iterOrig);

                    Iterator iter4(iterOrig);
                    ASSERT(iterOrig == iter4++);
                    ASSERT(iter     == iter4);

                    Iterator iter5(iter4);
                    ASSERT(iter4    == iter5--);
                    ASSERT(iterOrig == iter5);
                }

                ASSERT(iter == X.end());

                ASSERT(defaultAllocator.numAllocations() == allocations);

                // Verify 'begin()'.

                Obj mY;  const Obj Y = mY;

                Iterator initialBegin = Y.begin();

                iter = Y.begin();
                ASSERT(iter == Y.end());
                ASSERT(iter == initialBegin);

                mY = X;
                iter = Y.begin();
                ASSERT(iter == initialBegin);
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX;  const Obj& X = gg(&mX,
                                       "!20171225_20171231 0@20171226_1217");

            Iterator iter;
            ASSERT_SAFE_FAIL(*iter);
            ASSERT_SAFE_FAIL(iter.operator->());

            iter = X.begin();
            ASSERT_PASS(++iter);
            ASSERT_PASS(--iter);
            ASSERT_FAIL(--iter);

            iter = X.end();
            ASSERT_PASS(--iter);
            ASSERT_PASS(++iter);
            ASSERT_FAIL(++iter);

            iter = X.begin();
            ASSERT_PASS(iter++);
            ASSERT_PASS(iter--);
            ASSERT_FAIL(iter--);

            iter = X.end();
            ASSERT_PASS(iter--);
            ASSERT_PASS(iter++);
            ASSERT_FAIL(iter++);

            iter = X.begin();
            ASSERT_SAFE_PASS(*iter);
            ASSERT_SAFE_PASS(iter.operator->());
            mX.reset();
            ASSERT_SAFE_FAIL(*iter);
            ASSERT_SAFE_FAIL(iter.operator->());
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'removeTransition' METHODS
        //   The manipulators operate as expected.
        //
        // Concerns:
        //: 1 Each non-primary manipulator operates as expected.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Directly test that each non-primary manipulator, invoked on a set
        //:   of objects created with the generator function, returns the
        //:   expected value.  (C-1)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   void removeTransition(const Date& date, const Time& time);
        //   void removeTransition(const Datetime& datetime);
        //   void removeTransitions(dayOfWeek, time, firstDate, lastDate);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'removeTransition' METHODS"
                 << endl
                 << "=================================="
                 << endl;
        }

        if (verbose) cout << "\nTesting 'removeTransition(datetime)'." << endl;
        {
            const char *specX = "!20171225_20171231";
            const char *specY = "!20171225_20171231";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.removeTransition(bdlt::Datetime(2017, 12, 26, 12, 17));

            ASSERT(X == Y);
        }
        {
            const char *specX = "!20171225_20171231 0@20171226_1217";
            const char *specY = "!20171225_20171231";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.removeTransition(bdlt::Datetime(2017, 12, 26, 12, 17));

            ASSERT(X == Y);
        }
        {
            const char *specX = "!20171225_20171231 0@20171226_1217"
                                " 1@20171226_1456 2@20171227_0123";
            const char *specY = "!20171225_20171231 1@20171226_1456"
                                " 2@20171227_0123";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.removeTransition(bdlt::Datetime(2017, 12, 26, 12, 17));

            ASSERT(X == Y);
        }
        {
            const char *specX = "!20171225_20171231 0@20171226_1217"
                                " 1@20171226_1456 2@20171227_0123";
            const char *specY = "!20171225_20171231 0@20171226_1217"
                                " 2@20171227_0123";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.removeTransition(bdlt::Datetime(2017, 12, 26, 14, 56));

            ASSERT(X == Y);
        }
        {
            const char *specX = "!20171225_20171231 0@20171226_1217"
                                " 1@20171226_1456 2@20171227_0123";
            const char *specY = "!20171225_20171231 0@20171226_1217"
                                " 1@20171226_1456";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.removeTransition(bdlt::Datetime(2017, 12, 27, 1, 23));

            ASSERT(X == Y);
        }
        {
            const char *specX = "!20171225_20171231 0@20171226_1217"
                                " 1@20171226_1456 2@20171227_0123";
            const char *specY = "!20171225_20171231 0@20171226_1217"
                                " 1@20171226_1456 2@20171227_0123";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.removeTransition(bdlt::Datetime(2017, 12, 26, 1, 23));

            ASSERT(X == Y);
        }

        if (verbose) {
            cout << "\nTesting 'removeTransition(date, time)'." << endl;
        }
        {
            const char *specX = "!20171225_20171231 0@20171226_1217"
                                " 1@20171226_1456 2@20171227_0123";
            const char *specY = "!20171225_20171231 1@20171226_1456"
                                " 2@20171227_0123";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.removeTransition(bdlt::Date(2017, 12, 26), bdlt::Time(12, 17));

            ASSERT(X == Y);
        }
        {
            const char *specX = "!20171225_20171231 0@20171226_1217"
                                " 1@20171226_1456 2@20171227_0123";
            const char *specY = "!20171225_20171231 0@20171226_1217"
                                " 1@20171226_1456";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.removeTransition(bdlt::Date(2017, 12, 27), bdlt::Time(1, 23));

            ASSERT(X == Y);
        }

        if (verbose) {
            cout << "\nTesting 'removeTransitions(dow, t, f, l)'." << endl;
        }
        {
            const char *spec = "!20180101_20180131 0@20180105_1217"
                               " 1@20180115_1456 2@20180117_0123";

            Obj mX;  const Obj& X = gg(&mX, spec);

            mX.addTransitions(bdlt::DayOfWeek::e_MONDAY,
                              bdlt::Time(12, 34),
                              3,
                              bdlt::Date(2018, 1,  1),
                              bdlt::Date(2018, 1, 31));

            Obj mY(X);  const Obj& Y = mY;

            mX.removeTransitions(bdlt::DayOfWeek::e_MONDAY,
                                 bdlt::Time(12, 34),
                                 bdlt::Date(2018, 1,  1),
                                 bdlt::Date(2018, 1, 31));

            mY.removeTransition(bdlt::Date(2018, 1,  1),
                                bdlt::Time(12, 34));
            mY.removeTransition(bdlt::Date(2018, 1,  8),
                                bdlt::Time(12, 34));
            mY.removeTransition(bdlt::Date(2018, 1, 15),
                                bdlt::Time(12, 34));
            mY.removeTransition(bdlt::Date(2018, 1, 22),
                                bdlt::Time(12, 34));
            mY.removeTransition(bdlt::Date(2018, 1, 29),
                                bdlt::Time(12, 34));

            ASSERT(X == Y);
        }
        {
            const char *spec = "!20180101_20180131 0@20180105_1217"
                               " 1@20180115_1456 2@20180117_0123";

            Obj mX;  const Obj& X = gg(&mX, spec);

            mX.addTransitions(bdlt::DayOfWeek::e_WEDNESDAY,
                              bdlt::Time(12, 34),
                              3,
                              bdlt::Date(2018, 1,  1),
                              bdlt::Date(2018, 1, 31));

            Obj mY(X);  const Obj& Y = mY;

            mX.removeTransitions(bdlt::DayOfWeek::e_WEDNESDAY,
                                 bdlt::Time(12, 34),
                                 bdlt::Date(2018, 1, 16),
                                 bdlt::Date(2018, 1, 31));

            mY.removeTransition(bdlt::Date(2018, 1, 17),
                                bdlt::Time(12, 34));
            mY.removeTransition(bdlt::Date(2018, 1, 24),
                                bdlt::Time(12, 34));
            mY.removeTransition(bdlt::Date(2018, 1, 31),
                                bdlt::Time(12, 34));

            ASSERT(X == Y);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX;  ggg(&mX, "!20180101_20180131");

            ASSERT_PASS(mX.removeTransition(bdlt::Datetime(2018, 1, 1,  1)));
            ASSERT_FAIL(mX.removeTransition(bdlt::Datetime(2018, 1, 1, 24)));
            ASSERT_FAIL(mX.removeTransition(bdlt::Datetime(2019, 1, 1,  1)));

            ASSERT_PASS(mX.removeTransition(bdlt::Date(2018, 1, 1),
                                            bdlt::Time(1)));
            ASSERT_FAIL(mX.removeTransition(bdlt::Date(2018, 1, 1),
                                            bdlt::Time(24)));
            ASSERT_FAIL(mX.removeTransition(bdlt::Date(2017, 1, 1),
                                            bdlt::Time(1)));

            ASSERT_PASS(mX.removeTransitions(bdlt::DayOfWeek::e_MONDAY,
                                             bdlt::Time(1),
                                             bdlt::Date(2018, 1, 1),
                                             bdlt::Date(2018, 1, 2)));
            ASSERT_FAIL(mX.removeTransitions(bdlt::DayOfWeek::e_MONDAY,
                                             bdlt::Time(24),
                                             bdlt::Date(2018, 1, 1),
                                             bdlt::Date(2018, 1, 2)));
            ASSERT_FAIL(mX.removeTransitions(bdlt::DayOfWeek::e_MONDAY,
                                             bdlt::Time(1),
                                             bdlt::Date(2018, 1, 2),
                                             bdlt::Date(2018, 1, 1)));
            ASSERT_FAIL(mX.removeTransitions(bdlt::DayOfWeek::e_MONDAY,
                                             bdlt::Time(1),
                                             bdlt::Date(2017, 1, 1),
                                             bdlt::Date(2018, 1, 2)));
            ASSERT_FAIL(mX.removeTransitions(bdlt::DayOfWeek::e_MONDAY,
                                             bdlt::Time(1),
                                             bdlt::Date(2018, 1, 1),
                                             bdlt::Date(2019, 1, 2)));
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'removeTransitions(const Date& date)'
        //   The manipulator operates as expected.
        //
        // Concerns:
        //: 1 The non-primary manipulator operates as expected.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Directly test that the non-primary manipulator, invoked on a set
        //:   of objects created with the generator function, returns the
        //:   expected value.  (C-1)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   void removeTransitions(const Date& date);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'removeTransitions(const Date& date)'"
                 << endl
                 << "================================================"
                 << endl;
        }

        if (verbose) {
            cout << "\nTesting 'removeTransitions(const Date& date)'."
                 << endl;
        }
        {
            const char *specX = "!20171225_20171231";
            const char *specY = "!20171225_20171231";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.removeTransitions(bdlt::Date(2017, 12, 26));

            ASSERT(X == Y);
        }
        {
            const char *specX = "!20171225_20171231 0@20171226_1217";
            const char *specY = "!20171225_20171231";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.removeTransitions(bdlt::Date(2017, 12, 26));

            ASSERT(X == Y);
        }
        {
            const char *specX = "!20171225_20171231 0@20171226_1217"
                                " 1@20171226_1456 2@20171227_0123";
            const char *specY = "!20171225_20171231 2@20171227_0123";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.removeTransitions(bdlt::Date(2017, 12, 26));

            ASSERT(X == Y);
        }
        {
            const char *specX = "!20171225_20171231 0@20171226_1217"
                                " 1@20171226_1456 2@20171227_0123";
            const char *specY = "!20171225_20171231 0@20171226_1217"
                                " 1@20171226_1456";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.removeTransitions(bdlt::Date(2017, 12, 27));

            ASSERT(X == Y);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX;  ggg(&mX, "!20180101_20180131");

            ASSERT_PASS(mX.removeTransitions(bdlt::Date(2018, 1, 1)));
            ASSERT_FAIL(mX.removeTransitions(bdlt::Date(2019, 1, 1)));
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'removeAllTransitions()'
        //   The manipulator operates as expected.
        //
        // Concerns:
        //: 1 The non-primary manipulator operates as expected.
        //
        // Plan:
        //: 1 Directly test that the non-primary manipulator, invoked on a set
        //:   of objects created with the generator function, returns the
        //:   expected value.  (C-1)
        //
        // Testing:
        //   void removeAllTransitions();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'removeAllTransitions()'"
                 << endl
                 << "================================"
                 << endl;
        }

        if (verbose) {
            cout << "\nTesting 'removeAllTransitions()'."
                 << endl;
        }
        {
            const char *specX = "!20171225_20171231";
            const char *specY = "!20171225_20171231";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.removeAllTransitions();

            ASSERT(X == Y);
        }
        {
            const char *specX = "!20171225_20171231 0@20171226_1217";
            const char *specY = "!20171225_20171231";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.removeAllTransitions();

            ASSERT(X == Y);
        }
        {
            const char *specX = "!20171225_20171231 0@20171226_1217"
                                " 1@20171226_1456 2@20171227_0123";
            const char *specY = "!20171225_20171231";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.removeAllTransitions();

            ASSERT(X == Y);
        }
        {
            const char *specX = "!20171225_20171231 0@20171226_1217"
                                " 1@20171226_1456 2@20171227_0123";
            const char *specY = "!20171225_20171231";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.removeAllTransitions();

            ASSERT(X == Y);
        }
        {
            const char *specX = "I0 !20171225_20171231 0@20171226_1217"
                                " 1@20171226_1456 2@20171227_0123";
            const char *specY = "I0 !20171225_20171231";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.removeAllTransitions();

            ASSERT(X == Y);
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'addTransition(d, t, c)' & 'addTransitions(dow, t, c, f, l)'
        //   The manipulators operate as expected.
        //
        // Concerns:
        //: 1 Each non-primary manipulator operates as expected.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Directly test that each non-primary manipulator, invoked on a set
        //:   of objects created with the generator function, returns the
        //:   expected value.  (C-1)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   void addTransition(date, time, code);
        //   void addTransitions(dow, time, code, firstDate, lastDate);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'addTransition(d, t, c)' & "
                 << "'addTransitions(dow, t, c, f, l)'"
                 << endl
                 << "==================================="
                 << "================================"
                 << endl;
        }

        if (verbose) cout << "\nTesting 'addTransition(d, t, c)'." << endl;
        {
            const char *specX = "!20171225_20171231";
            const char *specY = "!20171225_20171231 0@20171226_1217";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.addTransition(bdlt::Date(2017, 12, 26), bdlt::Time(12, 17), 0);

            ASSERT(X == Y);
        }
        {
            const char *specX = "!20171225_20171231";
            const char *specY = "!20171225_20171231 1@20171227_1115";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.addTransition(bdlt::Date(2017, 12, 27), bdlt::Time(11, 15), 1);

            ASSERT(X == Y);
        }

        if (verbose) {
            cout << "\nTesting 'addTransition(dow, t, c, f, l)'." << endl;
        }
        {
            const char *specX = "!20180101_20180131";
            const char *specY = "!20180101_20180131 0@20180101_1115"
                                " 0@20180108_1115 0@20180115_1115"
                                " 0@20180122_1115 0@20180129_1115";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.addTransitions(bdlt::DayOfWeek::e_MONDAY,
                              bdlt::Time(11, 15),
                              0,
                              bdlt::Date(2018, 1,  1),
                              bdlt::Date(2018, 1, 31));

            ASSERT(X == Y);
        }
        {
            const char *specX = "!20180101_20180131";
            const char *specY = "!20180101_20180131"
                                " 0@20180108_1115 0@20180115_1115"
                                " 0@20180122_1115 0@20180129_1115";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.addTransitions(bdlt::DayOfWeek::e_MONDAY,
                              bdlt::Time(11, 15),
                              0,
                              bdlt::Date(2018, 1,  2),
                              bdlt::Date(2018, 1, 29));

            ASSERT(X == Y);
        }
        {
            const char *specX = "!20180101_20180131";
            const char *specY = "!20180101_20180131";

            Obj mX;  const Obj& X = gg(&mX, specX);
            Obj mY;  const Obj& Y = gg(&mY, specY);

            mX.addTransitions(bdlt::DayOfWeek::e_MONDAY,
                              bdlt::Time(11, 15),
                              0,
                              bdlt::Date(2018, 1, 2),
                              bdlt::Date(2018, 1, 5));

            ASSERT(X == Y);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX;  ggg(&mX, "!20180101_20180131");

            ASSERT_PASS(mX.addTransition(bdlt::Date(2018, 01, 01),
                                         bdlt::Time(1),
                                         0));
            ASSERT_PASS(mX.addTransition(bdlt::Date(2018, 01, 01),
                                         bdlt::Time(1),
                                         Obj::k_UNSET_TRANSITION_CODE));
            ASSERT_FAIL(mX.addTransition(bdlt::Date(2017, 01, 01),
                                         bdlt::Time(1),
                                         0));
            ASSERT_FAIL(mX.addTransition(bdlt::Date(2018, 01, 01),
                                         bdlt::Time(24),
                                         0));
            ASSERT_FAIL(mX.addTransition(bdlt::Date(2018, 01, 01),
                                         bdlt::Time(1),
                                         -2));

            ASSERT_PASS(mX.addTransitions(bdlt::DayOfWeek::e_MONDAY,
                                          bdlt::Time(1),
                                          0,
                                          bdlt::Date(2018, 01, 01),
                                          bdlt::Date(2018, 01, 03)));
            ASSERT_PASS(mX.addTransitions(bdlt::DayOfWeek::e_MONDAY,
                                          bdlt::Time(1),
                                          Obj::k_UNSET_TRANSITION_CODE,
                                          bdlt::Date(2018, 01, 01),
                                          bdlt::Date(2018, 01, 03)));
            ASSERT_FAIL(mX.addTransitions(bdlt::DayOfWeek::e_MONDAY,
                                          bdlt::Time(24),
                                          0,
                                          bdlt::Date(2018, 01, 01),
                                          bdlt::Date(2018, 01, 03)));
            ASSERT_FAIL(mX.addTransitions(bdlt::DayOfWeek::e_MONDAY,
                                          bdlt::Time(1),
                                          0,
                                          bdlt::Date(2018, 01, 03),
                                          bdlt::Date(2018, 01, 01)));
            ASSERT_FAIL(mX.addTransitions(bdlt::DayOfWeek::e_MONDAY,
                                          bdlt::Time(1),
                                          0,
                                          bdlt::Date(2017, 01, 01),
                                          bdlt::Date(2018, 01, 03)));
            ASSERT_FAIL(mX.addTransitions(bdlt::DayOfWeek::e_MONDAY,
                                          bdlt::Time(1),
                                          0,
                                          bdlt::Date(2018, 01, 01),
                                          bdlt::Date(2019, 01, 03)));
            ASSERT_FAIL(mX.addTransitions(bdlt::DayOfWeek::e_MONDAY,
                                          bdlt::Time(1),
                                          -2,
                                          bdlt::Date(2018, 01, 01),
                                          bdlt::Date(2018, 01, 03)));
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // RANGE CONSTRUCTOR
        //   The constructor operates as expected.
        //
        // Concerns:
        //: 1 The range constructor creates the correct initial value and has
        //:   the internal memory management system hooked up properly so that
        //:   *all* internally allocated memory draws from the same
        //:   user-supplied allocator whenever one is specified.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a set of objects using the range constructor and verify
        //:   the resultant object's value with basic accessors.
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   Timetable(firstDate, lastDate, initTransCode = UNSET, ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "RANGE CONSTRUCTOR" << endl
                          << "=================" << endl;

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            Obj mX(bdlt::Date(2018, 1, 1), bdlt::Date(2018, 1, 5), 0);

            const Obj& X = mX;

            ASSERT(&defaultAllocator      == X.allocator());
            ASSERT(bdlt::Date(2018, 1, 1) == X.firstDate());
            ASSERT(bdlt::Date(2018, 1, 5) == X.lastDate());
            ASSERT(                     0 == X.initialTransitionCode());

            ASSERT(allocations < defaultAllocator.numAllocations());
        }
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            Obj mX(bdlt::Date(2018, 1, 1),
                   bdlt::Date(2018, 1, 5),
                   0,
                   reinterpret_cast<bslma::TestAllocator *>(0));

            const Obj& X = mX;

            ASSERT(&defaultAllocator      == X.allocator());
            ASSERT(bdlt::Date(2018, 1, 1) == X.firstDate());
            ASSERT(bdlt::Date(2018, 1, 5) == X.lastDate());
            ASSERT(                     0 == X.initialTransitionCode());

            ASSERT(allocations < defaultAllocator.numAllocations());
        }
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(bdlt::Date(2018, 1, 1),
                   bdlt::Date(2018, 1, 5),
                   0,
                   &sa);

            const Obj& X = mX;

            ASSERT(&sa                    == X.allocator());
            ASSERT(bdlt::Date(2018, 1, 1) == X.firstDate());
            ASSERT(bdlt::Date(2018, 1, 5) == X.lastDate());
            ASSERT(                     0 == X.initialTransitionCode());

            ASSERT(allocations == defaultAllocator.numAllocations());
            ASSERT(0 < sa.numAllocations());
        }

        if (verbose) cout << "\nTesting resultant object's value."
                          << endl;
        {
            Obj mX(bdlt::Date(2018, 2, 1), bdlt::Date(2018, 2, 5));

            const Obj& X = mX;

            ASSERT(&defaultAllocator            == X.allocator());
            ASSERT(bdlt::Date(2018, 2, 1)       == X.firstDate());
            ASSERT(bdlt::Date(2018, 2, 5)       == X.lastDate());
            ASSERT(Obj::k_UNSET_TRANSITION_CODE == X.initialTransitionCode());
        }
        {
            Obj mX(bdlt::Date(2018, 2, 1), bdlt::Date(2018, 2, 5), 1);

            const Obj& X = mX;

            ASSERT(&defaultAllocator      == X.allocator());
            ASSERT(bdlt::Date(2018, 2, 1) == X.firstDate());
            ASSERT(bdlt::Date(2018, 2, 5) == X.lastDate());
            ASSERT(                     1 == X.initialTransitionCode());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            ASSERT_PASS(Obj(bdlt::Date(2012, 1, 5), bdlt::Date(2012, 1, 5)));
            ASSERT_FAIL(Obj(bdlt::Date(2012, 1, 5), bdlt::Date(2012, 1, 4)));
        }
      } break;
      case 11: {
        // -------------------------------------------------------------------
        // NON-BASIC ACCESSORS
        //   Ensure each non-basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each of the non-basic accessors returns the expected value.
        //:
        //: 2 Each non-basic accessor method is declared 'const'.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Directly test that each non-basic accessor, invoked on a set of
        //:   'const' objects created with the generator function, returns the
        //:   expected value as determined by an oracle implementation using
        //:   the basic accessors.  (C-1..2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   bool isInRange(const Date& date) const;
        //   int transitionCodeInEffect(const Date&, const Time&) const;
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "NON-BASIC ACCESSORS" << endl
                          << "===================" << endl;

        if (verbose) cout << "\nTest 'isInRange'." << endl;
        {
            {
                const char *spec = "";

                Obj mX;  const Obj& X = gg(&mX, spec);

                ASSERT(false == X.isInRange(bdlt::Date(2018, 1, 3)));

                mX.setInitialTransitionCode(0);

                ASSERT(false == X.isInRange(bdlt::Date(2018, 1, 3)));
            }

            {
                const char *spec = "!20171225_20171231";

                Obj mX;  const Obj& X = gg(&mX, spec);

                const bdlt::Date VA(2017, 12, 24);
                const bdlt::Date VB(2017, 12, 25);
                const bdlt::Date VC(2017, 12, 26);
                const bdlt::Date VD(2017, 12, 30);
                const bdlt::Date VE(2017, 12, 31);
                const bdlt::Date VF(2018,  1,  1);

                ASSERT(   (VA >= X.firstDate() && VA <= X.lastDate())
                       == X.isInRange(VA));

                ASSERT(   (VB >= X.firstDate() && VB <= X.lastDate())
                       == X.isInRange(VB));

                ASSERT(   (VC >= X.firstDate() && VC <= X.lastDate())
                       == X.isInRange(VC));

                ASSERT(   (VD >= X.firstDate() && VD <= X.lastDate())
                       == X.isInRange(VD));

                ASSERT(   (VE >= X.firstDate() && VE <= X.lastDate())
                       == X.isInRange(VE));

                ASSERT(   (VF >= X.firstDate() && VF <= X.lastDate())
                       == X.isInRange(VF));
            }
        }

        if (verbose) {
            cout << "\nTest 'transitionCodeInEffect(date, time)'." << endl;
        }
        {
            const char *spec = "!20171225_20171231 1@20171226_1217";

            Obj mX;  const Obj& X = gg(&mX, spec);

            const bdlt::Datetime VA(2017, 12, 25, 12, 16);
            const bdlt::Datetime VB(2017, 12, 25, 12, 17);
            const bdlt::Datetime VC(2017, 12, 25, 12, 18);

            ASSERT(   X.transitionCodeInEffect(VA.date(), VA.time())
                   == X.transitionCodeInEffect(VA));

            ASSERT(   X.transitionCodeInEffect(VB.date(), VB.time())
                   == X.transitionCodeInEffect(VB));

            ASSERT(   X.transitionCodeInEffect(VC.date(), VC.time())
                   == X.transitionCodeInEffect(VC));
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX;  const Obj& X = mX;

            ASSERT_SAFE_FAIL(X.transitionCodeInEffect(bdlt::Date(2018,
                                                                 1,
                                                                 2),
                                                      bdlt::Time(1)));

            mX.setValidRange(bdlt::Date(2018, 1, 1), bdlt::Date(2018, 1, 5));

            ASSERT_SAFE_PASS(X.firstDate());
            ASSERT_SAFE_PASS(X.lastDate());

            ASSERT_SAFE_FAIL(X.transitionCodeInEffect(bdlt::Date(2018,
                                                                 1,
                                                                 2),
                                                      bdlt::Time(24)));

            ASSERT_SAFE_PASS(X.transitionCodeInEffect(bdlt::Date(2018,
                                                                 1,
                                                                 2),
                                                      bdlt::Time(1)));
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING BDEX STREAMING
        //
        // The functionality was omitted.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING BDEX STREAMING" << endl
                          << "======================" << endl;
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any target object
        //:   to that of any source object.
        //:
        //: 2 The address of the target object's allocator is unchanged.
        //:
        //: 3 Any non-temporary memory allocation is from the target object's
        //:   allocator.
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
        //:   having the appropriate structure for the copy-assignment operator
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
        //:   the table described in P-3: (C-1..3, 5..9)
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
        //:     2 Use the value constructor to create a modifiable 'Obj', 'mX',
        //:       using 'oa' and having the value 'W'.
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
        //:     6 Use the equality comparison operators to verify that: (C-1,
        //:       6)
        //:
        //:       1 The target object, 'mX', now has the same value as 'Z'.
        //:
        //:       2 'Z' still has the same value as 'ZZ'.
        //:
        //:     7 Use the 'allocator' accessor of both 'mX' and 'Z' to verify
        //:       that both object allocators are unchanged.  (C-2, 7)
        //:
        //:     8 Use appropriate test allocators to verify that: (C-3, 8)
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
        //:   described in P-3: (C-1..3, 5..10)
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
        //:   7 Use the equality comparison operators to verify that the target
        //:     object, 'mX', still has the same value as 'ZZ'.  (C-10)
        //:
        //:   8 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.  (C-2)
        //:
        //:   9 Use appropriate test allocators to verify that: (C-3, 8)
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
        //:   allocated for non-temporary values from the default allocator.
        //:   (C-3)
        //
        // Testing:
        //   Timetable& operator=(const Timetable& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY-ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            // Quash unused variable warning.

            Obj a;
            Obj b;
            (a.*operatorAssignment)(b);
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
        "\nCreate a table of distinct object values and expected memory usage."
                                                                       << endl;

        static const char **SPECS = DEFAULT_SPECS;

        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC1 = SPECS[ti];

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZ(&scratch);   const Obj& Z  = gg(&mZ,  SPEC1);

            Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC1);

            if (veryVerbose) { T_ P_(ti) P_(Z) P_(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(ti, Z, Obj(&scratch), Z == Obj(&scratch));
                firstFlag = false;
            }

            // Test assignment from 'bdlt::Timetable'.

            for (int tj = 0; SPECS[tj]; ++tj) {
                const char *const SPEC2 = SPECS[tj];

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(&oa);  const Obj& X = gg(&mX, SPEC2);

                    if (veryVerbose) { T_ P_(tj) P_(X) }

                    LOOP4_ASSERT(ti, tj, X, Z,
                                 (X == Z) == (ti == tj));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    Obj *mR = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        mR = &(mX = Z);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    // Verify the address of the return value.

                    LOOP4_ASSERT(ti, tj, mR, &mX, mR == &mX);

                    LOOP4_ASSERT(ti, tj,  X, Z,  X == Z);
                    LOOP4_ASSERT(ti, tj, ZZ, Z, ZZ == Z);

                    LOOP4_ASSERT(ti, tj, &oa, X.allocator(),
                                 &oa == X.allocator());
                    LOOP4_ASSERT(ti, tj, &scratch, Z.allocator(),
                                 &scratch == Z.allocator());

                    LOOP2_ASSERT(ti, tj, sam.isInUseSame());

                    LOOP2_ASSERT(ti, tj, 0 == da.numBytesInUse());
                }

                // Verify all memory is released on object destruction.

                LOOP3_ASSERT(ti, tj, da.numBytesInUse(),
                             0 == da.numBytesInUse());
                LOOP3_ASSERT(ti, tj, oa.numBytesInUse(),
                             0 == oa.numBytesInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&oa);   const Obj& X = gg(&mX, SPEC1);

                Obj mZZ(&oa);  const Obj& ZZ = gg(&mZZ, SPEC1);

                const Obj& Z = mX;

                LOOP3_ASSERT(ti, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                Obj *mR = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    mR = &(mX = Z);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                // Verify the address of the return value.

                LOOP3_ASSERT(ti, mR, &mX, mR == &mX);

                LOOP3_ASSERT(ti, mR, &X, mR == &X);

                LOOP3_ASSERT(ti, Z, ZZ, ZZ == Z);

                LOOP3_ASSERT(ti, &oa, Z.allocator(), &oa == Z.allocator());

                if (0 == ti) {  // Empty, no allocation.
                    LOOP_ASSERT(ti, oam.isInUseSame());
                }

                LOOP_ASSERT(ti, sam.isInUseSame());

                LOOP_ASSERT(ti, 0 == da.numBytesInUse());
            }

            // Verify all memory is released on object destruction.

            LOOP2_ASSERT(ti, oa.numBytesInUse(), 0 == oa.numBytesInUse());
            LOOP2_ASSERT(ti, da.numBytesInUse(), 0 == da.numBytesInUse());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTION
        //   Ensure that, when member and free 'swap' are implemented, we can
        //   exchange the values of any two objects that use the same
        //   allocator.
        //
        // Concerns:
        //: 1 Both functions exchange the values of the (two) supplied objects.
        //:
        //: 2 The common object allocator address in both objects is unchanged.
        //:
        //: 3 The member function does not allocate memory from any allocator.
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
        //: 4 For each row 'R1' in the table of P-3: (C-1..3, 5)
        //:
        //:   1 Construct a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor to create a modifiable 'Obj', 'mW',
        //:     using 'oa' and having the value described by 'R1'; also use the
        //:     copy constructor to create a 'const' 'Obj' 'XX' (using a
        //:     "scratch" allocator) from 'mW'.
        //:
        //:   3 Use the member and free 'swap' functions to swap the value of
        //:     'mW' with itself, and then verify: (C-2, 3, 5)
        //:
        //:     1 The value is unchanged.
        //:
        //:     2 The object allocator address is unchanged.
        //:
        //:     3 For the member function, there was no additional object
        //:       memory allocation.
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
        //:     3 Use the member 'swap' function to swap the values of 'mX' and
        //:       'mY', and then verify: (C-1..3)
        //:
        //:       1 The values have been exchanged.
        //:
        //:       2 The object allocator addresses are unchanged.
        //:
        //:       3 There was no additional object memory allocation.
        //:
        //:     4 Use the free 'swap' function to again swap the values of 'mX'
        //:       and 'mY', and then verify: (C-1..3)
        //:
        //:       1 The values have been exchanged.
        //:
        //:       2 The object allocator addresses are unchanged.
        //:
        //:       3 The method works correctly when the objects have different
        //:         allocators.
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator when using the member
        //:   'swap'.  (C-3)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to swap objects that do not
        //:   refer to the same allocator, but not when the allocators are the
        //:   same (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   void swap(Timetable& other);
        //   void swap(Timetable& a, Timetable& b);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTION" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each function to a variable." << endl;
        {
            using namespace bdlc;

            typedef void (Obj::*funcPtr)(Obj&);

            typedef void (*freeFuncPtr)(Obj&, Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     memberSwap = &Obj::swap;
            freeFuncPtr freeSwap   = swap;

            // Quash unused variable warnings.

            Obj a;
            Obj b;
            (a.*memberSwap)(b);
            freeSwap(a, b);
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nCreate a table of distinct object values."
                                                                       << endl;

        const char **SPECS = DEFAULT_SPECS;

        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC1 = SPECS[ti];

            bslma::TestAllocator oa("object",       veryVeryVeryVerbose);
            bslma::TestAllocator oa2("object2",     veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj        mW(&oa);
            const Obj& W = gg(&mW, SPEC1);
            const Obj  XX(W, &scratch);

            if (veryVerbose) { T_ P_(ti) P_(W) P_(XX) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(ti, W, Obj(&scratch), W == Obj(&scratch));
                firstFlag = false;
            }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mW.swap(mW);

                LOOP3_ASSERT(ti, W, XX, W == XX);

                LOOP_ASSERT(ti, &oa == W.allocator());
                LOOP_ASSERT(ti, oam.isTotalSame());
            }

            // free function 'swap'
            {
                // While not required, when the allocator is the same no
                // allocation should occur.

                bslma::TestAllocatorMonitor oam(&oa);

                swap(mW, mW);

                LOOP3_ASSERT(ti, W, XX, W == XX);

                LOOP_ASSERT(ti, &oa == W.allocator());
                LOOP_ASSERT(ti, oam.isTotalSame());
            }

            for (int tj = 0; SPECS[tj]; ++tj) {
                const char *const SPEC2 = SPECS[tj];

                Obj        mX(XX, &oa);
                const Obj& X = mX;

                Obj        mY(&oa);
                const Obj& Y = gg(&mY, SPEC2);
                const Obj  YY(Y, &scratch);

                Obj        mZ(&oa2);
                const Obj& Z = gg(&mZ, SPEC2);
                const Obj  ZZ(Z, &scratch);

                if (veryVerbose) { T_ P_(tj) P_(X) P_(Y) P_(YY) }

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    mX.swap(mY);

                    LOOP4_ASSERT(ti, tj, X, YY, X == YY);
                    LOOP4_ASSERT(ti, tj, Y, XX, Y == XX);
                    LOOP2_ASSERT(ti, tj, &oa == X.allocator());
                    LOOP2_ASSERT(ti, tj, &oa == Y.allocator());
                    LOOP2_ASSERT(ti, tj, oam.isTotalSame());
                }

                // free function 'swap', same allocator
                {
                    // While not required, when the allocator is the same no
                    // allocation should occur.

                    bslma::TestAllocatorMonitor oam(&oa);

                    swap(mX, mY);

                    LOOP4_ASSERT(ti, tj, X, XX, X == XX);
                    LOOP4_ASSERT(ti, tj, Y, YY, Y == YY);
                    LOOP2_ASSERT(ti, tj, &oa == X.allocator());
                    LOOP2_ASSERT(ti, tj, &oa == Y.allocator());
                    LOOP2_ASSERT(ti, tj, oam.isTotalSame());
                }

                // free function 'swap', different allocator
                {
                    swap(mX, mZ);

                    LOOP4_ASSERT(ti, tj, X, XX, X == ZZ);
                    LOOP4_ASSERT(ti, tj, Z, ZZ, Z == XX);
                    LOOP2_ASSERT(ti, tj, &oa  == X.allocator());
                    LOOP2_ASSERT(ti, tj, &oa2 == Z.allocator());
                }
            }
        }

        // Verify no memory is allocated from the default allocator for the
        // methods under test.

        LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) cout << "\t'swap' member function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);
                Obj mB(&oa1);
                Obj mC(&oa1);
                Obj mZ(&oa2);

                ASSERT_SAFE_PASS(mA.swap(mB));
                ASSERT_SAFE_FAIL(mC.swap(mZ));
            }
        }
      } break;
      case 7: {
        // -------------------------------------------------------------------
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
        //: 5 Supplying an allocator to the copy constructor has no effect on
        //:   subsequent object values.
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
        //:     default value, (b) boundary values corresponding to every
        //:     range of values that each individual attribute can
        //:     independently attain, and (c) values that should require
        //:     allocation from each individual attribute that can
        //:     independently allocate memory.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1..12)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V' and using a "scratch"
        //:     allocator.
        //:
        //:   2 Execute an inner loop creating three distinct objects in turn,
        //:     each using the copy constructor on 'Z' from P-2.1, but
        //:     configured differently: (a) without passing an allocator, (b)
        //:     passing a null allocator address explicitly, and (c) passing
        //:     the address of a test allocator distinct from the default.
        //:
        //:   3 For each of these three iterations (P-2.2): (C-1..12)
        //:
        //:     1 Construct three 'bslma::TestAllocator' objects and install
        //:       one as the current default allocator (note that a ubiquitous
        //:       test allocator is already installed as the global
        //:       allocator).
        //:
        //:     2 Use the copy constructor to dynamically create an object
        //:       'X', supplying it the 'const' object 'Z' (see P-2.1),
        //:       configured appropriately (see P-2.2) using a distinct test
        //:       allocator for the object's footprint.  (C-9)
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
        //:     5 Use appropriate test allocators to verify that: (C-2..4, 7,
        //:       8, 12)
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
        //:       3 No temporary memory is allocated from the object
        //:         allocator.  (C-7)
        //:
        //:       4 All object memory is released when the object is
        //:         destroyed. (C-8)
        //:
        //: 3 Create an object as an automatic variable in the presence of
        //:   injected exceptions (using the
        //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros) and verify that
        //:   no memory is leaked.  (C-12)
        //
        // Testing:
        //   Timetable(const Timetable& original, ba = 0);
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        {
            static const char **SPECS = DEFAULT_SPECS;

            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.

            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mZ(&scratch);   const Obj& Z  = gg(&mZ,  SPEC);
                Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr;
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
                    LOOP2_ASSERT(ti, CONFIG,
                                 sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                    // Ensure the first row of the table contains the
                    // default-constructed value.

                    static bool firstFlag = true;
                    if (firstFlag) {
                        LOOP4_ASSERT(ti, CONFIG, Obj(), *objPtr,
                                     Obj() == *objPtr)
                        firstFlag = false;
                    }

                    // Verify the value of the object.

                    LOOP4_ASSERT(ti, CONFIG,  X, Z,  X == Z);

                    // Verify that the value of 'Z' has not changed.

                    LOOP4_ASSERT(ti, CONFIG, ZZ, Z, ZZ == Z);

                    // Also apply the object's 'allocator' accessor, as well as
                    // that of 'Z'.

                    LOOP4_ASSERT(ti, CONFIG, &oa, X.allocator(),
                                 &oa == X.allocator());

                    LOOP4_ASSERT(ti, CONFIG, &scratch, Z.allocator(),
                                 &scratch == Z.allocator());

                    // Verify no allocation from the non-object allocator.

                    LOOP3_ASSERT(ti, CONFIG, noa.numBlocksTotal(),
                                 0 == noa.numBlocksTotal());

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    LOOP3_ASSERT(ti, CONFIG, fa.numBlocksInUse(),
                                 0 == fa.numBlocksInUse());
                    LOOP3_ASSERT(ti, CONFIG, da.numBlocksInUse(),
                                 0 == da.numBlocksInUse());
                    LOOP3_ASSERT(ti, CONFIG, sa.numBlocksInUse(),
                                 0 == sa.numBlocksInUse());
                }  // end foreach configuration

            }  // end foreach row

            // Double check that at least some object memory got allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.

            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mZ(&scratch);  const Obj& Z = gg(&mZ, SPEC);

                if (veryVerbose) { T_ P_(ti) P(Z) }

                bslma::TestAllocator da("default", veryVeryVeryVerbose);
                bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(Z, &oa);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP2_ASSERT(ti, da.numBlocksInUse(), !da.numBlocksInUse());
                LOOP2_ASSERT(ti, oa.numBlocksInUse(), !oa.numBlocksInUse());
            }
        }
      } break;
      case 6: {
        // -------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of
        //   value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if they have
        //:   the same initial transition code, valid range (or are both
        //:   empty), transitions, and each corresponding pair of transitions
        //:   has the same transition code.
        //:
        //: 2 'true  == (X == X)' (i.e., identity).
        //:
        //: 3 'false == (X != X)' (i.e., identity).
        //:
        //: 4 'X == Y' if and only if 'Y == X' (i.e., commutativity).
        //:
        //: 5 'X != Y' if and only if 'Y != X' (i.e., commutativity).
        //:
        //: 6 'X != Y' if and only if '!(X == Y)'.
        //:
        //: 7 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 8 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 9 The equality-comparison operators' signatures and return types
        //:   are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.  (C-7..9)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row).
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1..6)
        //:
        //:   1 Use the generator function to create a 'const' object, 'W',
        //:     having the value from 'R1'.
        //:
        //:   2 Using 'W', verify the reflexive (anti-reflexive) property of
        //:     equality (inequality) in the presence of aliasing.  (C-2..3)
        //:
        //:   3 For each row 'R2' in the table of P-2:  (C-1, 4..6)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects set to
        //:       values from 'R1' and 'R2', respectively, are expected to
        //:       have the same value.
        //:
        //:     2 Use the generator function to create a 'const' object, 'X',
        //:       having the value from 'R1', and a second 'const' object,
        //:       'Y', having the value from 'R2'.
        //:
        //:     3 Using 'X' and 'Y', verify the commutativity property and
        //:       expected return value for both '==' and '!='.  (C-1, 4..6)
        //
        // Testing:
        //   bool operator==(lhs, rhs);
        //   bool operator!=(lhs, rhs);
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = bdlt::operator==;
            operatorPtr operatorNe = bdlt::operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        static const char **SPECS = DEFAULT_SPECS;

        if (verbose) cout << "\nCompare every value with every value."
                          << endl;

        for (int ti = 0; SPECS[ti]; ++ti) {
            // Ensure an object compares correctly with itself (alias test).
            {
                Obj mW;  const Obj W = gg(&mW, SPECS[ti]);

                LOOP2_ASSERT(ti, W,   W == W);
                LOOP2_ASSERT(ti, W, !(W != W));

                // Ensure the first row of the table contains the
                // default-constructed value.

                static bool firstFlag = true;
                if (firstFlag) {
                    LOOP3_ASSERT(ti, Obj(), W, Obj() == W)
                    firstFlag = false;
                }
            }

            for (int tj = 0; SPECS[tj]; ++tj) {
                const bool EXP = ti == tj;

                Obj mX;  const Obj& X = gg(&mX, SPECS[ti]);

                Obj mY;  const Obj  Y = gg(&mY, SPECS[tj]);

                if (veryVerbose) { T_ T_ T_ P_(EXP) P_(X) P(Y) }

                // Verify value and commutativity.

                LOOP4_ASSERT(ti, tj, X, Y,  EXP == (X == Y));
                LOOP4_ASSERT(ti, tj, Y, X,  EXP == (Y == X));

                LOOP4_ASSERT(ti, tj, X, Y, !EXP == (X != Y));
                LOOP4_ASSERT(ti, tj, Y, X, !EXP == (Y != X));
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR (<<)
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)'.
        //:
        //: 4 The 'print' method's signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values (0 and 4, respectively).
        //:
        //: 7 The output 'operator<<'s signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the destination 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5..6, 8)
        //:
        //:   1 Define fourteen carefully selected combinations of (two) object
        //:     values ('A' and 'B') and various values for the two formatting
        //:     parameters, along with the expected output.
        //:
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2, -8 } --> 6 expected o/ps
        //:     3 { B   } x {  2     } x {  3            } --> 1 expected o/p
        //:     4 { A B } x { -8     } x { -8            } --> 2 expected o/ps
        //:     5 { A B } x { -9     } x { -9            } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5..6, 8)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, '(-9, -9)', then
        //:       invoke the 'operator<<' instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5, 8)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3, 6)
        //
        // Testing:
        //   ostream& print(ostream& stream, int level = 0, int sPL = 4) const;
        //   ostream& operator<<(ostream&, const Timetable&);
        //   ostream& TT::print(stream, level, spacesPerLevel) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR (<<)" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOut = bdlt::operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOut;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        Obj mA;  const Obj& A = gg(&mA, "!20171225_20171231");
        Obj mB;  const Obj& B = gg(&mB, "!20180101_20180131 0@20180102_0123");

        static const struct {
            int         d_line;            // source line number
            int         d_level;
            int         d_spacesPerLevel;
            const Obj  *d_value_p;
            const char *d_expected_p;
        } DATA[] = {

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 } x { 0, 1, -1, -8 } --> 4 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL    VALUE                        EXP
        //---- - ---  ---------  ------------------------------------------

        { L_,  0,  0,  &A,       "[\n"
                                 "firstDate = 25DEC2017\n"
                                 "lastDate = 31DEC2017\n"
                                 "initialTransitionCode = -1\n"
                                 "[\n"
                                 "]\n"
                                 "]\n"                                       },

        { L_,  0,  1,  &A,       "[\n"
                                 " firstDate = 25DEC2017\n"
                                 " lastDate = 31DEC2017\n"
                                 " initialTransitionCode = -1\n"
                                 " [\n"
                                 " ]\n"
                                 "]\n"                                       },

        { L_,  0, -1,  &A,       "[ "
                                 "firstDate = 25DEC2017 "
                                 "lastDate = 31DEC2017 "
                                 "initialTransitionCode = -1 "
                                 "[ "
                                 "] "
                                 "]"                                         },

        { L_,  0, -8,  &A,       "[\n"
                                 "    firstDate = 25DEC2017\n"
                                 "    lastDate = 31DEC2017\n"
                                 "    initialTransitionCode = -1\n"
                                 "    [\n"
                                 "    ]\n"
                                 "]\n"                                       },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } --> 6 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL    VALUE                        EXP
        //---- - ---  ---------  ------------------------------------------

        { L_,  3,  0,  &A,       "[\n"
                                 "firstDate = 25DEC2017\n"
                                 "lastDate = 31DEC2017\n"
                                 "initialTransitionCode = -1\n"
                                 "[\n"
                                 "]\n"
                                 "]\n"                                       },

        { L_,  3,  2,  &A,       "      [\n"
                                 "        firstDate = 25DEC2017\n"
                                 "        lastDate = 31DEC2017\n"
                                 "        initialTransitionCode = -1\n"
                                 "        [\n"
                                 "        ]\n"
                                 "      ]\n"                                 },

        { L_,  3, -2,  &A,       "      [ "
                                 "firstDate = 25DEC2017 "
                                 "lastDate = 31DEC2017 "
                                 "initialTransitionCode = -1 "
                                 "[ "
                                 "] "
                                 "]"                                         },

        { L_,  3,  -8, &A,       "            [\n"
                                 "                firstDate = 25DEC2017\n"
                                 "                lastDate = 31DEC2017\n"
                                 "                initialTransitionCode = -1\n"
                                 "                [\n"
                                 "                ]\n"
                                 "            ]\n"                           },

        { L_, -3,  0,  &A,       "[\n"
                                 "firstDate = 25DEC2017\n"
                                 "lastDate = 31DEC2017\n"
                                 "initialTransitionCode = -1\n"
                                 "[\n"
                                 "]\n"
                                 "]\n"                                       },

        { L_, -3,  2,  &A,       "[\n"
                                 "        firstDate = 25DEC2017\n"
                                 "        lastDate = 31DEC2017\n"
                                 "        initialTransitionCode = -1\n"
                                 "        [\n"
                                 "        ]\n"
                                 "      ]\n"                                 },

        { L_, -3, -2,  &A,       "[ "
                                 "firstDate = 25DEC2017 "
                                 "lastDate = 31DEC2017 "
                                 "initialTransitionCode = -1 "
                                 "[ "
                                 "] "
                                 "]"                                         },

        { L_, -3,  -8, &A,       "[\n"
                                 "                firstDate = 25DEC2017\n"
                                 "                lastDate = 31DEC2017\n"
                                 "                initialTransitionCode = -1\n"
                                 "                [\n"
                                 "                ]\n"
                                 "            ]\n"                           },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 } x { 3 } --> 1 expected o/p
        // -----------------------------------------------------------------

        //LINE L SPL    VALUE                        EXP
        //---- - ---  ---------  ------------------------------------------

        { L_,  2,  3,  &B,       "      [\n"
                                 "         firstDate = 01JAN2018\n"
                                 "         lastDate = 31JAN2018\n"
                                 "         initialTransitionCode = -1\n"
                                 "         [\n"
                                 "            0@02JAN2018_01:23:00.000000\n"
                                 "         ]\n"
                                 "      ]\n"                                 },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 } x { -8 } --> 2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL    VALUE                        EXP
        //---- - ---  ---------  ------------------------------------------

        { L_, -8, -8,  &A,       "[\n"
                                 "    firstDate = 25DEC2017\n"
                                 "    lastDate = 31DEC2017\n"
                                 "    initialTransitionCode = -1\n"
                                 "    [\n"
                                 "    ]\n"
                                 "]\n"                                       },

        { L_, -8, -8,  &B,       "[\n"
                                 "    firstDate = 01JAN2018\n"
                                 "    lastDate = 31JAN2018\n"
                                 "    initialTransitionCode = -1\n"
                                 "    [\n"
                                 "        0@02JAN2018_01:23:00.000000\n"
                                 "    ]\n"
                                 "]\n"                                       },

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 } x { -9 } --> 2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL    VALUE                        EXP
        //---- - ---  ---------  ------------------------------------------

        { L_, -9, -9,  &A,       "[ "
                                 "firstDate = 25DEC2017 "
                                 "lastDate = 31DEC2017 "
                                 "initialTransitionCode = -1 "
                                 "[ "
                                 "] "
                                 "]"                                         },

        { L_, -9, -9,  &B,       "[ "
                                 "firstDate = 01JAN2018 "
                                 "lastDate = 31JAN2018 "
                                 "initialTransitionCode = -1 "
                                 "[ "
                                 "0@02JAN2018_01:23:00.000000 "
                                 "] "
                                 "]"                                         },

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE  = DATA[ti].d_line;
                const int          L     = DATA[ti].d_level;
                const int          SPL   = DATA[ti].d_spacesPerLevel;
                const Obj         *VALUE = DATA[ti].d_value_p;
                const char *const  EXP   = DATA[ti].d_expected_p;

                if (veryVerbose) {
                    T_ P_(LINE) P_(L) P(SPL);
                }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                const Obj& X = *VALUE;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                ostringstream os(&oa);

                // Verify supplied stream is returned by reference.

                if (-9 == L && -9 == SPL) {
                    LOOP_ASSERT(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    LOOP_ASSERT(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        LOOP_ASSERT(LINE, &os == &X.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        LOOP_ASSERT(LINE, &os == &X.print(os, L));
                    }
                    else {
                        LOOP_ASSERT(LINE, &os == &X.print(os));
                    }

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                {
                    bslma::TestAllocator da("default", veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    // Verify output is formatted as expected.

                    bsl::string osStr = os.str();

                    if (veryVeryVerbose) { P(osStr) }

                    LOOP3_ASSERT(LINE, EXP, osStr, EXP == osStr);
                }
            }
        }

        // Verify behavior when the stream is initially 'bad()'.

        {
            Obj mX;  const Obj& X = gg(&mX, "!20180101_20180131");

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            ostringstream os(&oa);

            os.setstate(bsl::ios::badbit);

            ASSERT(&os == &X.print(os));

            ASSERTV(os.str(), os.str() == "");
        }
      } break;
      case 4: {
        // -------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each of the basic accessors returns the expected value.
        //:
        //: 2 Each basic accessor method is declared 'const'.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 To test 'allocator', create object with various allocators and
        //:   ensure the returned value matches the supplied allocator.
        //:
        //: 2 Directly test that each basic accessor, invoked on a set of
        //:   'const' objects created with the generator function, returns the
        //:   expected value.  (C-1..2)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   const Date& firstDate() const;
        //   int initialTransitionCode() const;
        //   const Date& lastDate() const;
        //   int length() const;
        //   int transitionCodeInEffect(const Datetime& datetime) const;
        //   bslma::Allocator *allocator() const;
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nTesting 'allocator'." << endl;
        {
            Obj mX;  const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());
        }
        {
            Obj        mX(reinterpret_cast<bslma::TestAllocator *>(0));
            const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());
        }
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            ASSERT(&sa == X.allocator());
        }

        if (verbose) cout << "\nTesting residual basic accessors." << endl;

        {
            const char *spec = "";

            Obj mX;  const Obj& X = gg(&mX, spec);

            ASSERT(0 == X.length());

            ASSERT(Obj::k_UNSET_TRANSITION_CODE == X.initialTransitionCode());

            mX.setInitialTransitionCode(0);

            ASSERT(0 == X.initialTransitionCode());
        }

        {
            const char *spec = "!20171225_20171231";

            Obj mX;  const Obj& X = gg(&mX, spec);

            ASSERT(7 == X.length());

            ASSERT(bdlt::Date(2017, 12, 25) == X.firstDate());
            ASSERT(bdlt::Date(2017, 12, 31) == X.lastDate());

            ASSERT(Obj::k_UNSET_TRANSITION_CODE ==
                      X.transitionCodeInEffect(bdlt::Datetime(X.firstDate())));
            ASSERT(Obj::k_UNSET_TRANSITION_CODE ==
                      X.transitionCodeInEffect(bdlt::Datetime(X.lastDate())));

            mX.setInitialTransitionCode(0);

            ASSERT(0 == X.initialTransitionCode());

            ASSERT(0 ==
                      X.transitionCodeInEffect(bdlt::Datetime(X.firstDate())));
            ASSERT(0 ==
                      X.transitionCodeInEffect(bdlt::Datetime(X.lastDate())));
        }

        {
            const char *spec = "!20171225_20171231 !20180101_20180131";

            Obj mX;  const Obj& X = gg(&mX, spec);

            ASSERT(31 == X.length());

            ASSERT(bdlt::Date(2018, 1,  1) == X.firstDate());
            ASSERT(bdlt::Date(2018, 1, 31) == X.lastDate());

            ASSERT(Obj::k_UNSET_TRANSITION_CODE ==
                      X.transitionCodeInEffect(bdlt::Datetime(X.firstDate())));
            ASSERT(Obj::k_UNSET_TRANSITION_CODE ==
                      X.transitionCodeInEffect(bdlt::Datetime(X.lastDate())));

            mX.setInitialTransitionCode(0);

            ASSERT(0 == X.initialTransitionCode());

            ASSERT(0 ==
                      X.transitionCodeInEffect(bdlt::Datetime(X.firstDate())));
            ASSERT(0 ==
                      X.transitionCodeInEffect(bdlt::Datetime(X.lastDate())));
        }

        {
            const char *spec = "!20171225_20171231 1@20171226_1217";

            Obj mX;  const Obj& X = gg(&mX, spec);

            ASSERT(7 == X.length());

            ASSERT(bdlt::Date(2017, 12, 25) == X.firstDate());
            ASSERT(bdlt::Date(2017, 12, 31) == X.lastDate());

            ASSERT(Obj::k_UNSET_TRANSITION_CODE ==
                      X.transitionCodeInEffect(bdlt::Datetime(X.firstDate())));
            ASSERT(Obj::k_UNSET_TRANSITION_CODE ==
                      X.transitionCodeInEffect(bdlt::Datetime(2017,
                                                              12,
                                                              26,
                                                              12,
                                                              16)));
            ASSERT(1 ==
                      X.transitionCodeInEffect(bdlt::Datetime(2017,
                                                              12,
                                                              26,
                                                              12,
                                                              17)));
            ASSERT(1 ==
                      X.transitionCodeInEffect(bdlt::Datetime(2017,
                                                              12,
                                                              26,
                                                              12,
                                                              18)));
            ASSERT(1 ==
                      X.transitionCodeInEffect(bdlt::Datetime(X.lastDate())));

            mX.setInitialTransitionCode(0);

            ASSERT(0 == X.initialTransitionCode());

            ASSERT(0 ==
                      X.transitionCodeInEffect(bdlt::Datetime(X.firstDate())));
            ASSERT(0 ==
                      X.transitionCodeInEffect(bdlt::Datetime(2017,
                                                              12,
                                                              26,
                                                              12,
                                                              16)));
            ASSERT(1 ==
                      X.transitionCodeInEffect(bdlt::Datetime(2017,
                                                              12,
                                                              26,
                                                              12,
                                                              17)));
            ASSERT(1 ==
                      X.transitionCodeInEffect(bdlt::Datetime(2017,
                                                              12,
                                                              26,
                                                              12,
                                                              18)));
            ASSERT(1 ==
                      X.transitionCodeInEffect(bdlt::Datetime(X.lastDate())));
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX;  const Obj& X = mX;

            ASSERT_SAFE_FAIL(X.firstDate());
            ASSERT_SAFE_FAIL(X.lastDate());

            ASSERT_SAFE_FAIL(X.transitionCodeInEffect(bdlt::Datetime(2018,
                                                                     1,
                                                                     2,
                                                                     1)));

            mX.setValidRange(bdlt::Date(2018, 1, 1), bdlt::Date(2018, 1, 5));

            ASSERT_SAFE_PASS(X.firstDate());
            ASSERT_SAFE_PASS(X.lastDate());

            ASSERT_SAFE_FAIL(X.transitionCodeInEffect(bdlt::Datetime(2018,
                                                                     1,
                                                                     2,
                                                                     24)));

            ASSERT_SAFE_PASS(X.transitionCodeInEffect(bdlt::Datetime(2018,
                                                                     1,
                                                                     2,
                                                                     1)));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMITIVE GENERATOR FUNCTIONS
        //   Ensure that the primitive generator functions are able to create
        //   an object in any state.
        //
        // Concerns:
        //: 1 Invalid syntax is detected and reported.
        //:
        //: 2 Valid syntax produces the expected results.
        //
        // Plan:
        //: 1 Evaluate the 'ggg' function with a series of test strings of
        //:   increasing complexity and verify its return value.  (C-1)
        //:
        //: 2 Evaluate a series of test strings of increasing complexity to
        //:   set the state of a newly created object and verify the returned
        //:   object using basic accessors.  (C-2)
        //
        // Testing:
        //   Obj& gg(Obj *object, const char *spec);
        //   int ggg(Obj *object, const char *spec, bool vF);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMITIVE GENERATOR FUNCTIONS" << endl
                          << "=============================" << endl;

        static struct {
            int               d_lineNumber;  // line number
            const char       *d_input_p;     // input command
            int               d_retCode;     // return code of ggg()
        } DATA[] = {
            //LN  Input                                                   RC
            //--  ------------------------------------------------------  --
            { L_, ""                                                    , -1 },

            { L_, "R"                                                   , -1 },
            { L_, "R "                                                  , -1 },
            { L_, " R"                                                  , -1 },
            { L_, "a"                                                   ,  0 },
            { L_, "I"                                                   ,  0 },
            { L_, "I "                                                  ,  0 },
            { L_, "I0"                                                  , -1 },
            { L_, "I0 "                                                 , -1 },
            { L_, "!"                                                   ,  1 },
            { L_, "!20171225_20171231"                                  , -1 },
            { L_, "!20171225_20171231 "                                 , -1 },
            { L_, " !20171225_20171231"                                 , -1 },
            { L_, "!2017122_20171231"                                   ,  1 },
            { L_, "!20171225-20171231"                                  ,  9 },
            { L_, "!20171225_2017123"                                   , 10 },
            { L_, "!20171225_20171232"                                  , 10 },

            { L_, "RR"                                                  ,  1 },
            { L_, "Ra"                                                  ,  1 },
            { L_, "R R"                                                 , -1 },
            { L_, "R a"                                                 ,  2 },
            { L_, "!20171225_20171231!20180101_20180131"                , 18 },
            { L_, "!20171225_20171231 !20180101_20180131"               , -1 },
            { L_, "!20171225_20171231 0@20171226_1217"                  , -1 },
            { L_, "!20171225_20171231 0-20171226_1217"                  , 20 },
            { L_, "!20171225_20171231 0@20171226-1217"                  , 29 },

            { L_, "!20171225_20171231 0@20171226_1217 1@20171227_2359"  , -1 },
            { L_, "!20171225_20171231 0@20171226_1217 1@20171227_2400"  , 46 },

            { L_, "!20171225_20171231 0@20171226_1217 1@20171227_2359 R", -1 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            if (veryVerbose) { T_; P(ti); }
            const int         LINE  =  DATA[ti].d_lineNumber;
            const char       *INPUT =  DATA[ti].d_input_p;
            const int         RC    =  DATA[ti].d_retCode;

            if (veryVeryVerbose) {
                T_; T_; P(INPUT);
                T_; T_; P(RC);
            }

            Obj mX;  const Obj& X = mX;

            int retCode = ggg(&mX, INPUT, false);
            if (veryVerbose) { T_ P(ti) P(INPUT) P(X) }

            LOOP3_ASSERT(LINE, RC,  retCode,    RC == retCode);
        }

        {
            const char *spec = "";

            Obj mX;  const Obj& X = mX;

            int retCode = ggg(&mX, spec, false);
            ASSERT(-1 == retCode);

            ASSERT(0 == X.length());
        }

        {
            const char *spec = "R";

            Obj mX;  const Obj& X = mX;

            int retCode = ggg(&mX, spec, false);
            ASSERT(-1 == retCode);

            ASSERT(0 == X.length());
        }

        {
            const char *spec = "!20171225_20171231";

            Obj mX;  const Obj& X = mX;

            int retCode = ggg(&mX, spec, false);
            ASSERT(-1 == retCode);

            ASSERT(7 == X.length());

            ASSERT(bdlt::Date(2017, 12, 25) == X.firstDate());
            ASSERT(bdlt::Date(2017, 12, 31) == X.lastDate());

            ASSERT(Obj::k_UNSET_TRANSITION_CODE ==
                      X.transitionCodeInEffect(bdlt::Datetime(X.firstDate())));
            ASSERT(Obj::k_UNSET_TRANSITION_CODE ==
                      X.transitionCodeInEffect(bdlt::Datetime(X.lastDate())));
        }

        {
            const char *spec = "!20171225_20171231 !20180101_20180131";

            Obj mX;  const Obj& X = mX;

            int retCode = ggg(&mX, spec, false);
            ASSERT(-1 == retCode);

            ASSERT(31 == X.length());

            ASSERT(bdlt::Date(2018, 1,  1) == X.firstDate());
            ASSERT(bdlt::Date(2018, 1, 31) == X.lastDate());

            ASSERT(Obj::k_UNSET_TRANSITION_CODE ==
                      X.transitionCodeInEffect(bdlt::Datetime(X.firstDate())));
            ASSERT(Obj::k_UNSET_TRANSITION_CODE ==
                      X.transitionCodeInEffect(bdlt::Datetime(X.lastDate())));
        }

        {
            const char *spec = "!20171225_20171231 0@20171226_1217";

            Obj mX;  const Obj& X = mX;

            int retCode = ggg(&mX, spec, false);
            ASSERT(-1 == retCode);

            ASSERT(7 == X.length());

            ASSERT(bdlt::Date(2017, 12, 25) == X.firstDate());
            ASSERT(bdlt::Date(2017, 12, 31) == X.lastDate());

            ASSERT(Obj::k_UNSET_TRANSITION_CODE ==
                      X.transitionCodeInEffect(bdlt::Datetime(X.firstDate())));
            ASSERT(0 ==
                      X.transitionCodeInEffect(bdlt::Datetime(X.lastDate())));
        }

        // Test 'gg'.

        {
            const char *spec = "";

            Obj  mX;
            Obj&  Y = gg(&mX, spec);

            ASSERT(&Y == &mX);

            ASSERT(0 == Y.length());
        }
        {
            const char *spec = "!20171225_20171231 0@20171226_1217";

            Obj  mX;
            Obj&  Y = gg(&mX, spec);

            ASSERT(&Y == &mX);

            ASSERT(7 == Y.length());

            ASSERT(bdlt::Date(2017, 12, 25) == Y.firstDate());
            ASSERT(bdlt::Date(2017, 12, 31) == Y.lastDate());

            ASSERT(Obj::k_UNSET_TRANSITION_CODE ==
                      Y.transitionCodeInEffect(bdlt::Datetime(Y.firstDate())));
            ASSERT(0 ==
                      Y.transitionCodeInEffect(bdlt::Datetime(Y.lastDate())));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS TEST
        //   The basic concern is that the default constructor, the destructor,
        //   and the primary manipulators:
        //      - addTransition(const Datetime& datetime, int code)
        //      - reset
        //      - setValidRange
        //   operate as expected.
        //
        // Concerns:
        //: 1 Default constructor
        //:     a. creates an object with the expected value
        //:     b. properly wires the optionally-specified allocator
        //:
        //: 2 That 'addTransition(datetime, code)'
        //:     a. properly handles duplicates
        //:     b. is exception-neutral
        //:
        //: 3 That 'reset'
        //:     a. produces the expected value (empty)
        //:     b. leaves the object in a consistent state
        //:
        //: 4 That 'setInitialTransitionCode'
        //:     a. produces the expected value
        //:
        //: 5 That 'setValidRange'
        //:     a. produces the expected value
        //:     b. is exception-neutral
        //:
        //: 6 Memory is not leaked by any method and the destructor properly
        //:   deallocates the residual allocated memory.
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create an object using the default constructor with and without
        //:   passing in an allocator, verify the allocator is stored using the
        //:   (untested) 'allocator' accessor, and verifying all allocations
        //:   are done from the allocator in future tests.  (C-1)
        //:
        //: 2 Create objects using the 'bslma::TestAllocator', use the primary
        //:   manipulator method with various values, and the (untested)
        //:   accessors to verify the value of the object.  Also vary the test
        //:   allocator's allocation limit to verify behavior in the presence
        //:   of exceptions.  (C-2..5)
        //:
        //: 3 Use a supplied 'bslma::TestAllocator' that goes out-of-scope
        //:   at the conclusion of each test to ensure all memory is returned
        //:   to the allocator.  (C-6)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-7)
        //
        // Testing:
        //   Timetable(bslma::Allocator *basicAllocator = 0);
        //   ~Timetable();
        //   void addTransition(const Datetime& datetime, int code);
        //   void reset();
        //   void setInitialTransitionCode(int code);
        //   void setValidRange(const Date& firstDate, const Date& lastDate);
        // --------------------------------------------------------------------

        bslma::TestAllocator testAllocator(veryVeryVerbose);

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS TEST" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            Obj mX;  const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());

            mX.setValidRange(bdlt::Date(2018, 1, 1), bdlt::Date(2018, 1, 1));
            ASSERT(allocations < defaultAllocator.numAllocations());
        }
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            Obj        mX(reinterpret_cast<bslma::TestAllocator *>(0));
            const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());

            mX.setValidRange(bdlt::Date(2018, 1, 1), bdlt::Date(2018, 1, 1));
            ASSERT(allocations < defaultAllocator.numAllocations());
        }
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            ASSERT(&sa == X.allocator());

            mX.setValidRange(bdlt::Date(2018, 1, 1), bdlt::Date(2018, 1, 1));
            ASSERT(allocations == defaultAllocator.numAllocations());
            ASSERT(0 < sa.numAllocations());
        }

        if (verbose) {
            cout << "\nTesting 'setValidRange' and 'setInitialTransitionCode'."
                 << endl;
        }
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                Obj mX(&sa);  const Obj& X = mX;

                ASSERT(0 == X.length());

                // Set initial range.

                mX.setValidRange(bdlt::Date(2018, 1, 1),
                                 bdlt::Date(2018, 1, 1));
                ASSERT(1 == X.length());
                ASSERT(bdlt::Date(2018, 1, 1) == X.firstDate());
                ASSERT(bdlt::Date(2018, 1, 1) == X.lastDate());

                ASSERT(UNSET == X.initialTransitionCode());

                ASSERT(UNSET == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.firstDate())));
                ASSERT(UNSET == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.lastDate())));

                mX.setInitialTransitionCode(0);

                ASSERT(0 == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.firstDate())));
                ASSERT(0 == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.lastDate())));

                // Extend to the back of the timetable.

                mX.setValidRange(bdlt::Date(2018, 1, 1),
                                 bdlt::Date(2018, 1, 2));
                ASSERT(2 == X.length());
                ASSERT(bdlt::Date(2018, 1, 1) == X.firstDate());
                ASSERT(bdlt::Date(2018, 1, 2) == X.lastDate());

                ASSERT(0 == X.initialTransitionCode());

                ASSERT(0 == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.firstDate())));
                ASSERT(0 == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.lastDate())));

                // Change the range to a non-overlapping range.

                mX.setValidRange(bdlt::Date(2018, 1, 3),
                                 bdlt::Date(2018, 1, 7));
                ASSERT(5 == X.length());
                ASSERT(bdlt::Date(2018, 1, 3) == X.firstDate());
                ASSERT(bdlt::Date(2018, 1, 7) == X.lastDate());

                ASSERT(0 == X.initialTransitionCode());

                ASSERT(0 == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.firstDate())));
                ASSERT(0 == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.lastDate())));

                mX.addTransition(bdlt::Datetime(2018, 1, 3, 12), 1);

                ASSERT(0 == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.firstDate())));
                ASSERT(1 == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.lastDate())));

                // Remove from the beginning of the range.

                mX.setValidRange(bdlt::Date(2018, 1, 5),
                                 bdlt::Date(2018, 1, 7));
                ASSERT(3 == X.length());
                ASSERT(bdlt::Date(2018, 1, 5) == X.firstDate());
                ASSERT(bdlt::Date(2018, 1, 7) == X.lastDate());

                ASSERT(0 == X.initialTransitionCode());

                ASSERT(0 == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.firstDate())));
                ASSERT(0 == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.lastDate())));

                mX.addTransition(bdlt::Datetime(2018, 1, 6, 12), 1);

                ASSERT(0 == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.firstDate())));
                ASSERT(1 == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.lastDate())));

                // Remove from the end of the range.

                mX.setValidRange(bdlt::Date(2018, 1, 5),
                                 bdlt::Date(2018, 1, 6));
                ASSERT(2 == X.length());
                ASSERT(bdlt::Date(2018, 1, 5) == X.firstDate());
                ASSERT(bdlt::Date(2018, 1, 6) == X.lastDate());

                ASSERT(0 == X.initialTransitionCode());

                ASSERT(0 == X.transitionCodeInEffect(
                                              bdlt::Datetime(X.firstDate())));
                ASSERT(0 == X.transitionCodeInEffect(
                                              bdlt::Datetime(X.lastDate())));
                ASSERT(1 == X.transitionCodeInEffect(
                                              bdlt::Datetime(X.lastDate(),
                                                             bdlt::Time(14))));

                // Extend to the front of the range.

                mX.setValidRange(bdlt::Date(2018, 1, 4),
                                 bdlt::Date(2018, 1, 6));
                ASSERT(3 == X.length());
                ASSERT(bdlt::Date(2018, 1, 4) == X.firstDate());
                ASSERT(bdlt::Date(2018, 1, 6) == X.lastDate());

                ASSERT(0 == X.initialTransitionCode());

                ASSERT(0 == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.firstDate())));
                ASSERT(0 == X.transitionCodeInEffect(
                                              bdlt::Datetime(X.lastDate())));
                ASSERT(1 == X.transitionCodeInEffect(
                                              bdlt::Datetime(X.lastDate(),
                                                             bdlt::Time(14))));

                // Extend to the back of the range.

                mX.setValidRange(bdlt::Date(2018, 1, 4),
                                 bdlt::Date(2018, 1, 7));
                ASSERT(4 == X.length());
                ASSERT(bdlt::Date(2018, 1, 4) == X.firstDate());
                ASSERT(bdlt::Date(2018, 1, 7) == X.lastDate());

                ASSERT(0 == X.initialTransitionCode());

                ASSERT(0 == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.firstDate())));
                ASSERT(1 == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.lastDate())));

                // Extend both sides of the range.

                mX.setValidRange(bdlt::Date(2018, 1, 3),
                                 bdlt::Date(2018, 1, 8));
                ASSERT(6 == X.length());
                ASSERT(bdlt::Date(2018, 1, 3) == X.firstDate());
                ASSERT(bdlt::Date(2018, 1, 8) == X.lastDate());

                ASSERT(0 == X.initialTransitionCode());

                ASSERT(0 == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.firstDate())));
                ASSERT(1 == X.transitionCodeInEffect(
                                               bdlt::Datetime(X.lastDate())));
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

        if (verbose) {
            cout << "\nTesting 'addTransition(datetime, code)'."
                 << endl;
        }
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                const bdlt::Date FIRST_DATE(2018, 1, 3);
                const bdlt::Date  LAST_DATE(2018, 1, 7);

                const bdlt::Datetime FIRST(FIRST_DATE);
                const bdlt::Datetime   MID(FIRST_DATE + 2, bdlt::Time(12));
                const bdlt::Datetime MID_A(MID.date(),
                                           bdlt::Time(11, 59, 59, 999, 999));
                const bdlt::Datetime MID_B(MID.date(),
                                           bdlt::Time(12,  0,  0,   0,   1));
                const bdlt::Datetime  LAST(LAST_DATE,
                                           bdlt::Time(23, 59, 59, 999, 999));

                Obj mX(&sa);  const Obj& X = mX;

                ASSERT(0 == X.length());

                mX.setValidRange(FIRST_DATE, LAST_DATE);

                ASSERT(         5 == X.length());
                ASSERT(FIRST_DATE == X.firstDate());
                ASSERT( LAST_DATE == X.lastDate());

                ASSERT(UNSET == X.transitionCodeInEffect(FIRST));
                ASSERT(UNSET == X.transitionCodeInEffect(MID_A));
                ASSERT(UNSET == X.transitionCodeInEffect(MID));
                ASSERT(UNSET == X.transitionCodeInEffect(MID_B));
                ASSERT(UNSET == X.transitionCodeInEffect(LAST));

                mX.addTransition(MID, 0);

                ASSERT(UNSET == X.transitionCodeInEffect(FIRST));
                ASSERT(UNSET == X.transitionCodeInEffect(MID_A));
                ASSERT(    0 == X.transitionCodeInEffect(MID));
                ASSERT(    0 == X.transitionCodeInEffect(MID_B));
                ASSERT(    0 == X.transitionCodeInEffect(LAST));

                mX.addTransition(FIRST, 1);

                ASSERT(    1 == X.transitionCodeInEffect(FIRST));
                ASSERT(    1 == X.transitionCodeInEffect(MID_A));
                ASSERT(    0 == X.transitionCodeInEffect(MID));
                ASSERT(    0 == X.transitionCodeInEffect(MID_B));
                ASSERT(    0 == X.transitionCodeInEffect(LAST));

                mX.addTransition(LAST, 2);

                ASSERT(    1 == X.transitionCodeInEffect(FIRST));
                ASSERT(    1 == X.transitionCodeInEffect(MID_A));
                ASSERT(    0 == X.transitionCodeInEffect(MID));
                ASSERT(    0 == X.transitionCodeInEffect(MID_B));
                ASSERT(    2 == X.transitionCodeInEffect(LAST));

                mX.addTransition(MID, 3);

                ASSERT(    1 == X.transitionCodeInEffect(FIRST));
                ASSERT(    1 == X.transitionCodeInEffect(MID_A));
                ASSERT(    3 == X.transitionCodeInEffect(MID));
                ASSERT(    3 == X.transitionCodeInEffect(MID_B));
                ASSERT(    2 == X.transitionCodeInEffect(LAST));
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

        if (verbose) cout << "\nTesting 'reset'." << endl;
        {
            const bdlt::Date FIRST_DATE(2018, 1, 3);
            const bdlt::Date  LAST_DATE(2018, 1, 7);

            const bdlt::Datetime FIRST(FIRST_DATE);
            const bdlt::Datetime   MID(FIRST_DATE + 2, bdlt::Time(12));
            const bdlt::Datetime MID_A(MID.date(),
                                       bdlt::Time(11, 59, 59, 999, 999));
            const bdlt::Datetime MID_B(MID.date(),
                                       bdlt::Time(12,  0,  0,   0,   1));
            const bdlt::Datetime  LAST(LAST_DATE,
                                       bdlt::Time(23, 59, 59, 999, 999));

            Obj mX;  const Obj& X = mX;

            ASSERT(0 == X.length());

            mX.setValidRange(FIRST_DATE, LAST_DATE);

            ASSERT(         5 == X.length());
            ASSERT(FIRST_DATE == X.firstDate());
            ASSERT( LAST_DATE == X.lastDate());

            ASSERT(UNSET == X.transitionCodeInEffect(FIRST));
            ASSERT(UNSET == X.transitionCodeInEffect(MID_A));
            ASSERT(UNSET == X.transitionCodeInEffect(MID));
            ASSERT(UNSET == X.transitionCodeInEffect(MID_B));
            ASSERT(UNSET == X.transitionCodeInEffect(LAST));

            mX.addTransition(MID, 0);

            ASSERT(UNSET == X.transitionCodeInEffect(FIRST));
            ASSERT(UNSET == X.transitionCodeInEffect(MID_A));
            ASSERT(    0 == X.transitionCodeInEffect(MID));
            ASSERT(    0 == X.transitionCodeInEffect(MID_B));
            ASSERT(    0 == X.transitionCodeInEffect(LAST));

            mX.addTransition(FIRST, 1);

            ASSERT(    1 == X.transitionCodeInEffect(FIRST));
            ASSERT(    1 == X.transitionCodeInEffect(MID_A));
            ASSERT(    0 == X.transitionCodeInEffect(MID));
            ASSERT(    0 == X.transitionCodeInEffect(MID_B));
            ASSERT(    0 == X.transitionCodeInEffect(LAST));

            mX.addTransition(LAST, 2);

            ASSERT(    1 == X.transitionCodeInEffect(FIRST));
            ASSERT(    1 == X.transitionCodeInEffect(MID_A));
            ASSERT(    0 == X.transitionCodeInEffect(MID));
            ASSERT(    0 == X.transitionCodeInEffect(MID_B));
            ASSERT(    2 == X.transitionCodeInEffect(LAST));

            mX.addTransition(MID, 3);

            ASSERT(    1 == X.transitionCodeInEffect(FIRST));
            ASSERT(    1 == X.transitionCodeInEffect(MID_A));
            ASSERT(    3 == X.transitionCodeInEffect(MID));
            ASSERT(    3 == X.transitionCodeInEffect(MID_B));
            ASSERT(    2 == X.transitionCodeInEffect(LAST));

            mX.reset();

            ASSERT(0 == X.length());

            mX.setValidRange(FIRST_DATE, LAST_DATE);

            ASSERT(         5 == X.length());
            ASSERT(FIRST_DATE == X.firstDate());
            ASSERT( LAST_DATE == X.lastDate());

            ASSERT(UNSET == X.transitionCodeInEffect(FIRST));
            ASSERT(UNSET == X.transitionCodeInEffect(MID_A));
            ASSERT(UNSET == X.transitionCodeInEffect(MID));
            ASSERT(UNSET == X.transitionCodeInEffect(MID_B));
            ASSERT(UNSET == X.transitionCodeInEffect(LAST));

            mX.addTransition(MID, 0);

            ASSERT(UNSET == X.transitionCodeInEffect(FIRST));
            ASSERT(UNSET == X.transitionCodeInEffect(MID_A));
            ASSERT(    0 == X.transitionCodeInEffect(MID));
            ASSERT(    0 == X.transitionCodeInEffect(MID_B));
            ASSERT(    0 == X.transitionCodeInEffect(LAST));

            mX.addTransition(FIRST, 1);

            ASSERT(    1 == X.transitionCodeInEffect(FIRST));
            ASSERT(    1 == X.transitionCodeInEffect(MID_A));
            ASSERT(    0 == X.transitionCodeInEffect(MID));
            ASSERT(    0 == X.transitionCodeInEffect(MID_B));
            ASSERT(    0 == X.transitionCodeInEffect(LAST));

            mX.addTransition(LAST, 2);

            ASSERT(    1 == X.transitionCodeInEffect(FIRST));
            ASSERT(    1 == X.transitionCodeInEffect(MID_A));
            ASSERT(    0 == X.transitionCodeInEffect(MID));
            ASSERT(    0 == X.transitionCodeInEffect(MID_B));
            ASSERT(    2 == X.transitionCodeInEffect(LAST));

            mX.addTransition(MID, 3);

            ASSERT(    1 == X.transitionCodeInEffect(FIRST));
            ASSERT(    1 == X.transitionCodeInEffect(MID_A));
            ASSERT(    3 == X.transitionCodeInEffect(MID));
            ASSERT(    3 == X.transitionCodeInEffect(MID_B));
            ASSERT(    2 == X.transitionCodeInEffect(LAST));
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const bdlt::Date FIRST_DATE(2018, 1, 3);
            const bdlt::Date  LAST_DATE(2018, 1, 7);

            {
                Obj mX;

                ASSERT_PASS(mX.setInitialTransitionCode(0));
                ASSERT_PASS(mX.setInitialTransitionCode(
                                                Obj::k_UNSET_TRANSITION_CODE));
                ASSERT_FAIL(mX.setInitialTransitionCode(-2));
            }

            {
                Obj mX;

                ASSERT_PASS(mX.setValidRange(FIRST_DATE, LAST_DATE));
                ASSERT_FAIL(mX.setValidRange(LAST_DATE,  FIRST_DATE));
            }

            {
                Obj mX;

                mX.setValidRange(FIRST_DATE, LAST_DATE);

                ASSERT_PASS(mX.addTransition(bdlt::Datetime(FIRST_DATE),  0));
                ASSERT_PASS(mX.addTransition(bdlt::Datetime(FIRST_DATE),  1));
                ASSERT_PASS(mX.addTransition(bdlt::Datetime(FIRST_DATE),
                                             Obj::k_UNSET_TRANSITION_CODE));
                ASSERT_FAIL(mX.addTransition(bdlt::Datetime(FIRST_DATE), -2));

                ASSERT_FAIL(mX.addTransition(bdlt::Datetime(FIRST_DATE + 1,
                                                            bdlt::Time()),
                                             1));

                ASSERT_FAIL(mX.addTransition(bdlt::Datetime(FIRST_DATE - 1),
                                             1));

                ASSERT_FAIL(mX.addTransition(bdlt::Datetime(LAST_DATE   + 1),
                                             1));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create objects using various constructors.
        //:
        //: 2 Exercise these objects using primary manipulators, basic
        //:   accessors, equality operators, and the assignment operator.
        //:
        //: 3 Verify expected values throughout.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const bdlt::Date VA(2017, 1,  1);
        const bdlt::Date VB(2017, 1, 15);
        const bdlt::Date VC(2017, 1, 31);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << endl << "default constructor" << endl;
        {
            Obj mX;  const Obj& X = mX;

            if (verbose) {  cout << '\t'; P(X);  }

            ASSERT(0 == X.length());

            ASSERT(true  == (X == X));
            ASSERT(false == (X != X));
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << endl << "range constructor" << endl;
        {
            Obj mX(VA, VB);  const Obj& X = mX;

            if (verbose) {  cout << '\t'; P(X);  }

            ASSERT(15 == X.length());
            ASSERT(VA == X.firstDate());
            ASSERT(VB == X.lastDate());

            ASSERT(true  == X.isInRange(VA));
            ASSERT(true  == X.isInRange(VB));
            ASSERT(false == X.isInRange(VC));

            ASSERT(Obj::k_UNSET_TRANSITION_CODE ==
                                 X.transitionCodeInEffect(VA, bdlt::Time(12)));

            ASSERT(true  == (X == X));
            ASSERT(false == (X != X));

            {
                Obj mY;  const Obj& Y = mY;

                ASSERT(false == (X == Y));
                ASSERT(true  == (X != Y));

                ASSERT(false == (Y == X));
                ASSERT(true  == (Y != X));
            }
            {
                Obj mY(VA, VB);  const Obj& Y = mY;

                ASSERT(true  == (X == Y));
                ASSERT(false == (X != Y));

                ASSERT(true  == (Y == X));
                ASSERT(false == (Y != X));
            }
            {
                Obj mY(VA, VC);  const Obj& Y = mY;

                ASSERT(false == (X == Y));
                ASSERT(true  == (X != Y));

                ASSERT(false == (Y == X));
                ASSERT(true  == (Y != X));
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << endl << "copy constructor" << endl;
        {
            Obj mX(VA, VB);  const Obj& X = mX;

            Obj mY(X);  const Obj& Y = mY;

            ASSERT(true  == (X == Y));
            ASSERT(false == (X != Y));

            ASSERT(true  == (Y == X));
            ASSERT(false == (Y != X));
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << endl << "'addTransition'" << endl;
        {
            Obj mX(VA, VB);  const Obj& X = mX;

            mX.addTransition(VA, bdlt::Time(11), 1);

            if (verbose) {  cout << '\t'; P(X);  }

            ASSERT(15 == X.length());
            ASSERT(VA == X.firstDate());
            ASSERT(VB == X.lastDate());

            ASSERT(true  == X.isInRange(VA));
            ASSERT(true  == X.isInRange(VB));
            ASSERT(false == X.isInRange(VC));

            ASSERT(Obj::k_UNSET_TRANSITION_CODE ==
                                 X.transitionCodeInEffect(VA, bdlt::Time(10)));

            ASSERT(1 == X.transitionCodeInEffect(VA, bdlt::Time(11)));
            ASSERT(1 == X.transitionCodeInEffect(VA, bdlt::Time(12)));

            ASSERT(1 == X.transitionCodeInEffect(VB, bdlt::Time(10)));
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
