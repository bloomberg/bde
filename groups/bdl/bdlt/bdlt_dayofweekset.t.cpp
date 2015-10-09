// bdlt_dayofweekset.t.cpp                                            -*-C++-*-
#include <bdlt_dayofweekset.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bsls_asserttest.h>

#include <bslx_byteinstream.h>
#include <bslx_byteoutstream.h>
#include <bslx_instreamfunctions.h>
#include <bslx_outstreamfunctions.h>
#include <bslx_testinstream.h>
#include <bslx_testinstreamexception.h>
#include <bslx_testoutstream.h>
#include <bslx_versionfunctions.h>

#include <bsl_new.h>          // placement syntax
#include <bsl_cstring.h>      // 'strlen', 'memset', 'memcpy', 'memcmp'
#include <bsl_c_stdlib.h>     // 'atoi'
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'add' and 'removeAll' methods to be used by the generator functions 'g' and
// 'gg'.  Additional helper functions are provided to facilitate sorting the
// elements in expected test case results.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [10] static int maxSupportedBdexVersion(int versionSelector);
//
// CREATORS
// [ 2] DayOfWeekSet();
// [ 7] DayOfWeekSet(const DayOfWeekSet& original);
// [ 2] ~DayOfWeekSet();
//
// MANIPULATORS
// [13] DayOfWeekSet& operator|=(const DayOfWeekSet& rhs);
// [13] DayOfWeekSet& operator&=(const DayOfWeekSet& rhs);
// [13] DayOfWeekSet& operator^=(const DayOfWeekSet& rhs);
// [13] DayOfWeekSet& operator-=(const DayOfWeekSet& rhs);
// [11] void add(DayOfWeek value);
// [11] int remove(DayOfWeek value);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
// [ 9] DayOfWeekSet& operator=(const DayOfWeekSet& rhs);
// [ 2] void removeAll();
// [ 2] void add(DayOfWeek value);
//
// ACCESSORS
// [12] bool areMembers(const DayOfWeekSet& set) const;
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 5] ostream& print(ostream& stream, int level, int spacesPerLevel);
// [ 4] iterator begin() const;
// [ 4] iterator end() const;
// [ 4] bool isEmpty() const;
// [ 4] bool isMember(Month::Month value) const;
// [ 4] int length() const;
// [ 4] reverse_iterator rbegin() const;
// [ 4] reverse_iterator rend() const;
// [ 4] const DayOfWeek::Day& operator*() const;
//
// FREE OPERATORS
// [13] DayOfWeekSet operator~(const DayOfWeekSet& set);
// [13] operator|(const DayOfWeekSet&, const DayOfWeekSet&);
// [13] operator&(const DayOfWeekSet&, const DayOfWeekSet&);
// [13] operator^(const DayOfWeekSet&, const DayOfWeekSet&);
// [13] operator-(const DayOfWeekSet&, const DayOfWeekSet&);
// [ 6] operator==(const DayOfWeekSet&, const DayOfWeekSet&);
// [ 6] operator!=(const DayOfWeekSet&, const DayOfWeekSet&);
// [ 5] operator<<(ostream&, const DayOfWeekSet&);
// ----------------------------------------------------------------------------
// [ 4] DayOfWeekSet_Iter();
// [ 4] DayOfWeekSet_Iter(int data, int index);
// [ 4] DayOfWeekSet_Iter(const DayOfWeekSet_Iter& original);
// [ 4] ~DayOfWeekSet_Iter();
// [ 4] DayOfWeekSet_Iter& operator=(const DayOfWeekSet_Iter&);
// [ 4] DayOfWeekSet_Iter& operator++();
// [ 4] DayOfWeekSet_Iter& operator--();
// [ 4] const DayOfWeek::Enum& operator*() const;
// [ 4] DayOfWeekSet_Iter& operator=(const DayOfWeekSet_Iter&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] DayOfWeekSet();
// [ 2] ~DayOfWeekSet();
// [ 2] void add(DayOfWeek value);
// [ 2] void removeAll();
// [15] USAGE EXAMPLE
// [ 3] DayOfWeekSet& gg(DayOfWeekSet* obj, const char *spec);
// [ 3] int ggg(DayOfWeekSet *object, const char *spec, int vF = 1);
// [ 3] DayOfWeekSet g(const char *spec);
// [ *] CONCERN: negative testing
// [ *] CONCERN: no use of global allocator
// [ *] CONCERN: no use of default allocator
// [14] NEGATIVE TESTING
// [ 8] Reserved for 'swap' testing.

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
//                        NON-STANDARD OUTPUT MACROS
// ----------------------------------------------------------------------------

#define DOT     (cout << '.' << flush)    // Output a '.' with no newline
#define CR      (cout << endl)            // Output a newline

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

typedef bdlt::DayOfWeekSet                     Obj;
typedef bdlt::DayOfWeek::Enum                  Element;
typedef bdlt::DayOfWeekSet_Iter                Iterator;
typedef bdlt::DayOfWeekSet::reverse_iterator   ReverseIterator;

typedef bslx::TestInStream                     In;
typedef bslx::TestOutStream                    Out;

#define VERSION_SELECTOR 20140601

const Element VALUES[] =
{
    bdlt::DayOfWeek::e_SUN,
    bdlt::DayOfWeek::e_TUE,
    bdlt::DayOfWeek::e_THU,
    bdlt::DayOfWeek::e_MON,
    bdlt::DayOfWeek::e_FRI,
    bdlt::DayOfWeek::e_SAT,
    bdlt::DayOfWeek::e_WED
};

const Element &V0 = VALUES[0], &VA = V0,  // V0, V1, ... are used in
              &V1 = VALUES[1], &VB = V1,  // conjunction with the VALUES array.
              &V2 = VALUES[2], &VC = V2,
              &V3 = VALUES[3], &VD = V3,  // VA, VB, ... are used in
              &V4 = VALUES[4], &VE = V4,  // conjunction with 'g' and 'gg'.
              &V5 = VALUES[5], &VF = V5,
              &V6 = VALUES[6], &VG = V6;

const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

enum {
    SU = 1 << bdlt::DayOfWeek::e_SUN,
    MO = 1 << bdlt::DayOfWeek::e_MON,
    TU = 1 << bdlt::DayOfWeek::e_TUE,
    WE = 1 << bdlt::DayOfWeek::e_WED,
    TH = 1 << bdlt::DayOfWeek::e_THU,
    FR = 1 << bdlt::DayOfWeek::e_FRI,
    SA = 1 << bdlt::DayOfWeek::e_SAT
};

const int BIT_MASK[] = {
    0, // filler
    SU,
    MO,
    TU,
    WE,
    TH,
    FR,
    SA,
};

// ============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static Element nextElement(const Element *array,
                           int            arrayLen,
                           Element        element)
    // Returns the smallest element that is greater than the specified
    // 'element' from the specified 'array' (of the specified 'arrayLen').
    // Return 'element' if there are no elements in 'array' that are greater
    // than 'element'.
{
    Element ret = element;

    for (int i = 0; i < arrayLen; ++i) {
        if (element < array[i]) {
            if (ret == element) {
                if (ret < array[i]) {
                    ret = array[i];
                }
            }
            else if (ret > array[i]) {
                ret = array[i];
            }
        }
    }

    return ret;
}

static int sortElementsAndRemoveDuplicates(Element       *dest,
                                           int            destLen,
                                           const Element *src,
                                           int            srcLen)
    // Load into the specified 'dest' array (of the specified 'destLen') the
    // elements from the specified 'src' array (of the specified 'srcLen'), in
    // ascending order and removing duplicated elements.  Return the number of
    // elements added.  The behavior is undefined unless 'srcLen <= destLen'.
{
    ASSERT(srcLen <= destLen);

    Element last   = Element(0);
    int     dstPos = 0;

    for (Element cur = nextElement(src, srcLen, last);
         cur != last;
         last = cur, cur = nextElement(src, srcLen, last)) {
        dest[dstPos++] = cur;
    }

    return dstPos;
}

// ============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
// ----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters [A .. G] correspond to arbitrary (but unique) DayOfWeek values to be
// added to the 'bdlt::DayOfWeekSet' object.  A tilde ('~') indicates that the
// logical (but not necessarily physical) state of the object is to be set to
// its initial, empty state (via the 'removeAll' method).
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>       ::= <EMPTY>   | <LIST>
//
// <EMPTY>      ::=
//
// <LIST>       ::= <ITEM>    | <ITEM><LIST>
//
// <ITEM>       ::= <ELEMENT> | <REMOVE_ALL>
//
// <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G'
//                                      // unique but otherwise arbitrary
// <REMOVE_ALL> ::= '~'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object empty.
// "A"          Add the value corresponding to A.
// "AA"         Add two values both corresponding to A.
// "ABC"        Add three values corresponding to A, B and C.
// "ABC~"       Add three values corresponding to A, B and C and then
//              remove all the elements (set set length to 0).  Note that
//              this spec yields an object that is logically equivalent
//              (but not necessarily identical internally) to one
//              yielded by ("").
// "ABC~DE"     Add three values corresponding to A, B, and C; empty
//              the object; and add values corresponding to D and E.
//
// ----------------------------------------------------------------------------

int ggg(bdlt::DayOfWeekSet *object, const char *spec, int verboseFlag = 1)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator function 'add' and white-box
    // manipulator 'removeAll'.  Optionally specify a zero 'verboseFlag' to
    // suppress 'spec' syntax error messages.  Return the index of the first
    // invalid character, and a negative value otherwise.  Note that this
    // function is used to implement 'gg' as well as allow for verification of
    // syntax error detection.
{
    object->removeAll();

    enum { SUCCESS = -1 };
    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'G') {
            object->add(VALUES[spec[i] - 'A']);
        }
        else if ('~' == spec[i]) {
            object->removeAll();
        }
        else {
            if (verboseFlag) {
                cout << "Error, bad character ('" << spec[i] << "') in spec \""
                     << spec << "\" at position " << i << '.' << endl;
            }

            // Discontinue processing this spec.

            return i;                                                 // RETURN
        }
    }
    return SUCCESS;
}

bdlt::DayOfWeekSet& gg(bdlt::DayOfWeekSet *object, const char *spec)
    // Return, by reference, the specified '*object' with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

bdlt::DayOfWeekSet g(const char *spec)
    // Return, by value, a new object corresponding to the specified 'spec'.
{
    bdlt::DayOfWeekSet object;
    return gg(&object, spec);
}

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator globalAlloc;
    bslma::TestAllocator dfltAlloc;

    bslma::Default::setGlobalAllocator(&globalAlloc);
    bslma::DefaultAllocatorGuard allocatorGuard(&dfltAlloc);

    switch (test) { case 0:  // Zero is always the leading case.
      case 15: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
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
///Example 1: Manipulation and Traversal of Day of Week Sets
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A 'bdlt::DayOfWeekSet' is useful for recording recurring appointments, or
// special days (e.g., weekend days), in a calendar.  The following snippets of
// code illustrate how to create and use a 'bdlt::DayOfWeek' set.
//
// First, we create a couple of commonly useful sets.  First we define the
// 'bdlt::DayOfWeekSet' 'weekendDays':
//..
    bdlt::DayOfWeekSet weekendDays;
//..
// Then, we notice that this set is initially empty.
//..
    ASSERT(0 == weekendDays.length());
//..
// Next, we add the days that characterize weekends:
//..
    weekendDays.add(bdlt::DayOfWeek::e_SUN);
    ASSERT(1 == weekendDays.length());

    weekendDays.add(bdlt::DayOfWeek::e_SAT);
    ASSERT(2 == weekendDays.length());
//..
// Then, we observe that 'weekendDays' now contains precisely the days we
// expect it to contain:
//..
    ASSERT(true  == weekendDays.isMember(bdlt::DayOfWeek::e_SUN));
    ASSERT(false == weekendDays.isMember(bdlt::DayOfWeek::e_MON));
    ASSERT(false == weekendDays.isMember(bdlt::DayOfWeek::e_TUE));
    ASSERT(false == weekendDays.isMember(bdlt::DayOfWeek::e_WED));
    ASSERT(false == weekendDays.isMember(bdlt::DayOfWeek::e_THU));
    ASSERT(false == weekendDays.isMember(bdlt::DayOfWeek::e_FRI));
    ASSERT(true  == weekendDays.isMember(bdlt::DayOfWeek::e_SAT));
//..
// Next, we create the complementary 'bdlt::DayOfWeekSet' 'weekDays' directly
// from 'weekendDays' via a combination of unary negation and copy
// construction:
//..
    bdlt::DayOfWeekSet weekDays(~weekendDays);

    ASSERT(5 == weekDays.length());

    ASSERT(false == weekDays.isMember(bdlt::DayOfWeek::e_SUN));
    ASSERT(true  == weekDays.isMember(bdlt::DayOfWeek::e_MON));
    ASSERT(true  == weekDays.isMember(bdlt::DayOfWeek::e_TUE));
    ASSERT(true  == weekDays.isMember(bdlt::DayOfWeek::e_WED));
    ASSERT(true  == weekDays.isMember(bdlt::DayOfWeek::e_THU));
    ASSERT(true  == weekDays.isMember(bdlt::DayOfWeek::e_FRI));
    ASSERT(false == weekDays.isMember(bdlt::DayOfWeek::e_SAT));
//..
// Then, to create a set containing all of the days in the week, we do so via
// unary negation of the default constructed value:
//..
    const bdlt::DayOfWeekSet NO_DAYS;
    const bdlt::DayOfWeekSet ALL_DAYS(~NO_DAYS);

    ASSERT(7 == ALL_DAYS.length());
//..
// Next, we observe that neither 'weekDays' nor 'weekendDays' represent the
// same value as 'ALL_DAYS', but their union does:
//..
    ASSERT(ALL_DAYS != weekendDays);
    ASSERT(ALL_DAYS != weekDays);
    ASSERT(ALL_DAYS == (weekDays | weekendDays));
    ASSERT(ALL_DAYS == (weekDays ^ weekendDays));

    ASSERT(weekendDays == ALL_DAYS - weekDays);

    ASSERT(weekDays    == ALL_DAYS - weekendDays);

    ASSERT(weekDays    == ALL_DAYS - weekendDays);
//..
// Then, we observe that similarly, neither 'weekDays' nor 'weekendDays'
// represents the same value as 'NO_DAYS', but their intersection does:
//..
    ASSERT(NO_DAYS != weekendDays);
    ASSERT(NO_DAYS != weekDays);
    ASSERT(NO_DAYS == (weekDays & weekendDays));

    ASSERT(weekendDays == weekendDays - weekDays);

    ASSERT(weekDays    == weekDays - weekendDays);
//..
// Next, we create the corresponding set 'eDays' consisting of the only days of
// the week that have an 'E' in them: 'TUESDAY' and 'WEDNESDAY':
//..
    bdlt::DayOfWeekSet eDays;                 ASSERT(0 == eDays.length());
    eDays.add(bdlt::DayOfWeek::e_TUE);     ASSERT(1 == eDays.length());
    eDays.add(bdlt::DayOfWeek::e_WED);     ASSERT(2 == eDays.length());

    ASSERT(false == eDays.isMember(bdlt::DayOfWeek::e_SUN));
    ASSERT(false == eDays.isMember(bdlt::DayOfWeek::e_MON));
    ASSERT(true  == eDays.isMember(bdlt::DayOfWeek::e_TUE));
    ASSERT(true  == eDays.isMember(bdlt::DayOfWeek::e_WED));
    ASSERT(false == eDays.isMember(bdlt::DayOfWeek::e_THU));
    ASSERT(false == eDays.isMember(bdlt::DayOfWeek::e_FRI));
    ASSERT(false == eDays.isMember(bdlt::DayOfWeek::e_SAT));
//..
// Then, we create a set consisting of days that have an 'n' in them: 'MONDAY',
// 'WEDNESDAY', and 'SUNDAY'.  We create the corresponding set 'nDays' starting
// with the value of 'eDays' by first removing 'TUESDAY', and then adding
// 'SUNDAY' and 'MONDAY':
//..
    bdlt::DayOfWeekSet nDays(eDays);          ASSERT(2 == nDays.length());

    nDays.remove(bdlt::DayOfWeek::e_TUE);  ASSERT(1 == nDays.length());

    nDays.add(bdlt::DayOfWeek::e_SUN);     ASSERT(2 == nDays.length());
    nDays.add(bdlt::DayOfWeek::e_MON);     ASSERT(3 == nDays.length());

    ASSERT(true  == nDays.isMember(bdlt::DayOfWeek::e_SUN));
    ASSERT(true  == nDays.isMember(bdlt::DayOfWeek::e_MON));
    ASSERT(false == nDays.isMember(bdlt::DayOfWeek::e_TUE));
    ASSERT(true  == nDays.isMember(bdlt::DayOfWeek::e_WED));
    ASSERT(false == nDays.isMember(bdlt::DayOfWeek::e_THU));
    ASSERT(false == nDays.isMember(bdlt::DayOfWeek::e_FRI));
    ASSERT(false == nDays.isMember(bdlt::DayOfWeek::e_SAT));
//..
// Next, we observe that all 'eDays' are 'weekDays', but that's not true of
// 'nDays':
//..
    ASSERT(true  == weekDays.areMembers(eDays));
    ASSERT(false == weekDays.areMembers(nDays));
//..
// Now, we observe that iteration order is defined by increasing enumerated
// 'bdlt::DayOfWeek::Day' value '[ SUN .. SAT ]'.  The following use of the
// *forward* (bi-directional) iterator:
//..
    for (bdlt::DayOfWeekSet::iterator it  = ALL_DAYS.begin();
                                      it != ALL_DAYS.end();
                                      ++it) {
        bsl::cout << *it << bsl::endl;
    }
//..
// produces:
//..
//  SUN
//  MON
//  TUE
//  WED
//  THU
//  FRI
//  SAT
//..
// on standard output.
//
// Finally, we observe that, similarly, the following use of the *reverse*
// iterator:
//..
    for (bdlt::DayOfWeekSet::reverse_iterator it  = weekDays.rbegin();
                                              it != weekDays.rend();
                                              ++it) {
        bsl::cout << *it << bsl::endl;
    }
//..
// produces:
//..
//  FRI
//  THU
//  WED
//  TUE
//  MON
//..
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // NEGATIVE TESTING
        //
        // Concerns:
        //: 1 Provoke every instance of the word 'undefined' from the function
        //:   doc of this component, and verify that they are all tested for by
        //:   safe asserts.
        //
        // Plan:
        //   Note that this test was built and run with dbg_exc_mt_safe,
        //   dbg_exc_mt, and opt_exc_mt).
        //: 1 Construct Iter with invalid index.
        //: 2 Construct Iter with bit 0 set.
        //: 3 Construct valid Iters.
        //: 4 Dereference invalid and valid forward iters.
        //: 5 Dereference invalid and valid reverse iters.
        //: 6 Compare fwd/rvrs Iters to the same set.
        //: 7 Compare fwd/rvrs Iters to different sets.
        //
        // Testing:
        //   NEGATIVE TESTING
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "NEGATIVE TESTING" << endl
                          << "================" << endl;

        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        if (verbose) cout << "Construct Iter with invalid index.\n";
        {
            ASSERT_SAFE_FAIL(bdlt::DayOfWeekSet_Iter(0, -1));
            ASSERT_SAFE_FAIL(bdlt::DayOfWeekSet_Iter(0,  9));
        }

        if (verbose) cout << "Construct Iter with bit 0 set.\n";
        {
            ASSERT_SAFE_FAIL(bdlt::DayOfWeekSet_Iter(1,  1));
            ASSERT_SAFE_FAIL(bdlt::DayOfWeekSet_Iter(0xff,  1));
        }

        if (verbose) cout << "Construct valid Iters.\n";
        {
            ASSERT_SAFE_PASS(bdlt::DayOfWeekSet_Iter(0xfe, 0));
            ASSERT_SAFE_PASS(bdlt::DayOfWeekSet_Iter(0xfe, 1));
            ASSERT_SAFE_PASS(bdlt::DayOfWeekSet_Iter(0xfe, 7));
            ASSERT_SAFE_PASS(bdlt::DayOfWeekSet_Iter(0xfe, 8));
        }

        if (verbose) cout << "Dereference invalid and valid forward iters.\n";

        Obj mX;    const Obj& X = mX;
        mX.add(bdlt::DayOfWeek::e_MONDAY);
        Iterator it = X.end();

        ASSERT_SAFE_FAIL(*it);
        --it;
        ASSERT_SAFE_PASS(*it);

        if (verbose) cout << "Dereference invalid and valid reverse iters.\n";

        ReverseIterator rit = X.rend();

        ASSERT_SAFE_FAIL(*rit);
        --rit;
        ASSERT_SAFE_PASS(*rit);

        Iterator itB = it;
        ReverseIterator ritB = rit;

        ++it;
        ++rit;

        if (verbose) cout << "Compare fwd/rvrs Iters to the same set.\n";
        {
            ASSERT_SAFE_PASS((void) (itB  == it));
            ASSERT_SAFE_PASS((void) (itB  != it));
            ASSERT_SAFE_PASS((void) (ritB == rit));
            ASSERT_SAFE_PASS((void) (ritB != rit));
        }

        if (verbose) cout << "Compare fwd/rvrs Iters to different sets.\n";
        {
            mX.add(bdlt::DayOfWeek::e_SATURDAY);

            Iterator itC = X.begin();

            ASSERT_SAFE_FAIL((void) (itC == itB));
            ASSERT_SAFE_FAIL((void) (itC != itB));

            ReverseIterator ritC = X.rend();

            ASSERT_SAFE_FAIL((void) (ritC == ritB));
            ASSERT_SAFE_FAIL((void) (ritC != ritB));
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING SET ARITHMETIC OPERATORS
        //
        // Concerns:
        //: 1 For the infix '|', '&', '^', and '-' operators:
        //:   A The return values are correct.
        //:   B The objects input are not modified.
        //:   C The object returned is not the lhs or rhs operand.
        //:
        //: 2 For the assignment '|=', '&=', '^', and '-=' operators:
        //:   A The end value of the lhs is correct (and the same as the result
        //:     of the corresponding infix operator).
        //:   B The object on the rhs is not modified.
        //:   C The object returned is the object on the lhs.
        //:   D The operation works correctly under aliasing.
        //:
        //: 3 For unary '~' operator:
        //:   A The return value is correct.
        //:   B The parameter is not modified.
        //:   C The object returned is not the object passed.
        //
        // Plan:
        //   We have a table where each of the lines of the table indicates
        //   specs for two 'DayOfWeekSet' values, and then the specs for each
        //   of the the 4 infix operations:
        //:  1 Perform the infix operation, assigning the result to a
        //:    reference.
        //:
        //:  2 Verify that the value of the reference is the expected value
        //:    from the table (C-1-A).
        //:
        //:  3 Verify that the address of the reference is not the address of
        //:    either the lhs or rhs argument (C-1-C).
        //:
        //:  4 Verify that the left or right arguments have not been modified
        //:    by comparing them with copies taken before the operation.
        //:
        //:  5 Perform the assignment equivalent of the infix operation, taking
        //:    a reference to a modifiable of the result.
        //:
        //:  6 Verify that the value of the lhs of the assignment is now equal
        //:    to the expected value from the table (C-2-A).
        //:
        //:  7 Verify that the address of the reference is equal to the address
        //:    of the lhs of the assignment (C-2-3).
        //:
        //:  8 Verify that the value of the rhs argument has not changed
        //:    (C-2-2).
        //:
        //:  9 Create an object that is a copy of the lhs object.
        //:
        //: 10 Apply the assignment to itself.
        //:
        //: 11 Verify the assigned object has the expect value (C-2-D).
        //
        // Then for the unary '~' operator:
        //:  1 Apply 'operator~' and keep a reference to the result.
        //:
        //:  2 Verify the value of the reference is as expected (C-3-A).
        //:
        //:  3 Verify the address of the reference is not the address of the
        //:    argument (C-3-C)
        //:
        //:  4 Verify the value of the argument has not changed (C-3-B).
        //
        // Testing:
        //   DayOfWeekSet& operator|=(const DayOfWeekSet& rhs);
        //   DayOfWeekSet& operator&=(const DayOfWeekSet& rhs);
        //   DayOfWeekSet& operator^=(const DayOfWeekSet& rhs);
        //   DayOfWeekSet& operator-=(const DayOfWeekSet& rhs);
        //   DayOfWeekSet operator~(const DayOfWeekSet& set);
        //   operator|(const DayOfWeekSet&, const DayOfWeekSet&);
        //   operator&(const DayOfWeekSet&, const DayOfWeekSet&);
        //   operator^(const DayOfWeekSet&, const DayOfWeekSet&);
        //   operator-(const DayOfWeekSet&, const DayOfWeekSet&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING SET ARITHMETIC OPERATORS" << endl
                          << "================================" << endl;

        if (verbose) cout <<
            "\nTesting set arithmetic operators." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_specA;    // source string for A
                const char *d_specB;    // source string for B
                const char *d_specU;    // expected spec for A | B
                const char *d_specI;    // expected spec for A & B
                const char *d_specE;    // expected spec for A ^ B
                const char *d_specS;    // expected spec for A - B
                const char *d_specC;    // expected spec for ~A
            } DATA[] = {
            //line   A      B      A|B       A&B    A^B     A-B        ~A
            //---- -----  -----  --------  ------  -----    ---         --
            { L_,  "",    "",    "",       "",     "",       "",    "ABCDEFG"},
            { L_,  "",    "A",   "A",      "",     "A",      "",    "ABCDEFG"},
            { L_,  "",    "B",   "B",      "",     "B",      "",    "ABCDEFG"},
            { L_,  "",    "AB",  "AB",     "",     "AB",     "",    "ABCDEFG"},
            { L_,  "",    "BC",  "BC",     "",     "BC",     "",    "ABCDEFG"},
            { L_,  "",    "CD",  "CD",     "",     "CD",     "",    "ABCDEFG"},
            { L_,  "",    "ABC", "ABC",    "",     "ABC",    "",    "ABCDEFG"},
            { L_,  "",    "BCD", "BCD",    "",     "BCD",    "",    "ABCDEFG"},
            { L_,  "",    "CDE", "CDE",    "",     "CDE",    "",    "ABCDEFG"},
            { L_,  "",    "DEF", "DEF",    "",     "DEF",    "",    "ABCDEFG"},

            { L_,  "A",   "",    "A",      "",     "A",      "A",   "BCDEFG" },
            { L_,  "A",   "A",   "A",      "A",    "",       "",    "BCDEFG" },
            { L_,  "A",   "B",   "AB",     "",     "AB",     "A",   "BCDEFG" },
            { L_,  "A",   "AB",  "AB",     "A",    "B",      "",    "BCDEFG" },
            { L_,  "A",   "BC",  "ABC",    "",     "ABC",    "A",   "BCDEFG" },
            { L_,  "A",   "CD",  "ACD",    "",     "ACD",    "A",   "BCDEFG" },
            { L_,  "A",   "ABC", "ABC",    "A",    "BC",     "",    "BCDEFG" },
            { L_,  "A",   "BCD", "ABCD",   "",     "ABCD",   "A",   "BCDEFG" },
            { L_,  "A",   "CDE", "ACDE",   "",     "ACDE",   "A",   "BCDEFG" },
            { L_,  "A",   "DEF", "ADEF",   "",     "ADEF",   "A",   "BCDEFG" },

            { L_,  "B",   "",    "B",      "",     "B",      "B",   "ACDEFG" },
            { L_,  "B",   "A",   "AB",     "",     "AB",     "B",   "ACDEFG" },
            { L_,  "B",   "B",   "B",      "B",    "",       "",    "ACDEFG" },
            { L_,  "B",   "AB",  "AB",     "B",    "A",      "",    "ACDEFG" },
            { L_,  "B",   "BC",  "BC",     "B",    "C",      "",    "ACDEFG" },
            { L_,  "B",   "CD",  "BCD",    "",     "BCD",    "B",   "ACDEFG" },
            { L_,  "B",   "ABC", "ABC",    "B",    "AC",     "",    "ACDEFG" },
            { L_,  "B",   "BCD", "BCD",    "B",    "CD",     "",    "ACDEFG" },
            { L_,  "B",   "CDE", "BCDE",   "",     "BCDE",   "B",   "ACDEFG" },
            { L_,  "B",   "DEF", "BDEF",   "",     "BDEF",   "B",   "ACDEFG" },

            { L_,  "AB",  "",    "AB",     "",     "AB",     "AB",  "CDEFG"  },
            { L_,  "AB",  "A",   "AB",     "A",    "B",      "B",   "CDEFG"  },
            { L_,  "AB",  "B",   "AB",     "B",    "A",      "A",   "CDEFG"  },
            { L_,  "AB",  "AB",  "AB",     "AB",   "",       "",    "CDEFG"  },
            { L_,  "AB",  "BC",  "ABC",    "B",    "AC",     "A",   "CDEFG"  },
            { L_,  "AB",  "CD",  "ABCD",   "",     "ABCD",   "AB",  "CDEFG"  },
            { L_,  "AB",  "ABC", "ABC",    "AB",   "C",      "",    "CDEFG"  },
            { L_,  "AB",  "BCD", "ABCD",   "B",    "ACD",    "A",   "CDEFG"  },
            { L_,  "AB",  "CDE", "ABCDE",  "",     "ABCDE",  "AB",  "CDEFG"  },
            { L_,  "AB",  "DEF", "ABDEF",  "",     "ABDEF",  "AB",  "CDEFG"  },

            { L_,  "BC",  "",    "BC",     "",     "BC",     "BC",  "ADEFG"  },
            { L_,  "BC",  "A",   "ABC",    "",     "ABC",    "BC",  "ADEFG"  },
            { L_,  "BC",  "B",   "BC",     "B",    "C",      "C",   "ADEFG"  },
            { L_,  "BC",  "AB",  "ABC",    "B",    "AC",     "C",   "ADEFG"  },
            { L_,  "BC",  "BC",  "BC",     "BC",   "",       "",    "ADEFG"  },
            { L_,  "BC",  "CD",  "BCD",    "C",    "BD",     "B",   "ADEFG"  },
            { L_,  "BC",  "ABC", "ABC",    "BC",   "A",      "",    "ADEFG"  },
            { L_,  "BC",  "BCD", "BCD",    "BC",   "D",      "",    "ADEFG"  },
            { L_,  "BC",  "CDE", "BCDE",   "C",    "BDE",    "B",   "ADEFG"  },
            { L_,  "BC",  "DEF", "BCDEF",  "",     "BCDEF",  "BC",  "ADEFG"  },

            { L_,  "CD",  "",    "CD",     "",     "CD",     "CD",  "ABEFG"  },
            { L_,  "CD",  "A",   "ACD",    "",     "ACD",    "CD",  "ABEFG"  },
            { L_,  "CD",  "B",   "BCD",    "",     "BCD",    "CD",  "ABEFG"  },
            { L_,  "CD",  "AB",  "ABCD",   "",     "ABCD",   "CD",  "ABEFG"  },
            { L_,  "CD",  "BC",  "BCD",    "C",    "BD",     "D",   "ABEFG"  },
            { L_,  "CD",  "CD",  "CD",     "CD",   "",       "",    "ABEFG"  },
            { L_,  "CD",  "ABC", "ABCD",   "C",    "ABD",    "D",   "ABEFG"  },
            { L_,  "CD",  "BCD", "BCD",    "CD",   "B",      "",    "ABEFG"  },
            { L_,  "CD",  "CDE", "CDE",    "CD",   "E",      "",    "ABEFG"  },
            { L_,  "CD",  "DEF", "CDEF",   "D",    "CEF",    "C",   "ABEFG"  },

            { L_,  "ABC", "",    "ABC",    "",     "ABC",    "ABC", "DEFG"   },
            { L_,  "ABC", "A",   "ABC",    "A",    "BC",     "BC",  "DEFG"   },
            { L_,  "ABC", "B",   "ABC",    "B",    "AC",     "AC",  "DEFG"   },
            { L_,  "ABC", "AB",  "ABC",    "AB",   "C",      "C",   "DEFG"   },
            { L_,  "ABC", "BC",  "ABC",    "BC",   "A",      "A",   "DEFG"   },
            { L_,  "ABC", "CD",  "ABCD",   "C",    "ABD",    "AB",  "DEFG"   },
            { L_,  "ABC", "ABC", "ABC",    "ABC",  "",       "",    "DEFG"   },
            { L_,  "ABC", "BCD", "ABCD",   "BC",   "AD",     "A",   "DEFG"   },
            { L_,  "ABC", "CDE", "ABCDE",  "C",    "ABDE",   "AB",  "DEFG"   },
            { L_,  "ABC", "DEF", "ABCDEF", "",     "ABCDEF", "ABC", "DEFG"   },

            { L_,  "BCD", "",    "BCD",    "",     "BCD",    "BCD", "AEFG"   },
            { L_,  "BCD", "A",   "ABCD",   "",     "ABCD",   "BCD", "AEFG"   },
            { L_,  "BCD", "B",   "BCD",    "B",    "CD",     "CD",  "AEFG"   },
            { L_,  "BCD", "AB",  "ABCD",   "B",    "ACD",    "CD",  "AEFG"   },
            { L_,  "BCD", "BC",  "BCD",    "BC",   "D",      "D",   "AEFG"   },
            { L_,  "BCD", "CD",  "BCD",    "CD",   "B",      "B",   "AEFG"   },
            { L_,  "BCD", "ABC", "ABCD",   "BC",   "AD",     "D",   "AEFG"   },
            { L_,  "BCD", "BCD", "BCD",    "BCD",  "",       "",    "AEFG"   },
            { L_,  "BCD", "CDE", "BCDE",   "CD",   "BE",     "B",   "AEFG"   },
            { L_,  "BCD", "DEF", "BCDEF",  "D",    "BCEF",   "BC",  "AEFG"   },

            { L_,  "CDE", "",    "CDE",    "",     "CDE",    "CDE", "ABFG"   },
            { L_,  "CDE", "A",   "ACDE",   "",     "ACDE",   "CDE", "ABFG"   },
            { L_,  "CDE", "B",   "BCDE",   "",     "BCDE",   "CDE", "ABFG"   },
            { L_,  "CDE", "AB",  "ABCDE",  "",     "ABCDE",  "CDE", "ABFG"   },
            { L_,  "CDE", "BC",  "BCDE",   "C",    "BDE",    "DE",  "ABFG"   },
            { L_,  "CDE", "CD",  "CDE",    "CD",   "E",      "E",   "ABFG"   },
            { L_,  "CDE", "ABC", "ABCDE",  "C",    "ABDE",   "DE",  "ABFG"   },
            { L_,  "CDE", "BCD", "BCDE",   "CD",   "BE",     "E",   "ABFG"   },
            { L_,  "CDE", "CDE", "CDE",    "CDE",  "",       "",    "ABFG"   },
            { L_,  "CDE", "DEF", "CDEF",   "DE",   "CF",     "C",   "ABFG"   },

            { L_,  "DEF", "",    "DEF",    "",     "DEF",    "DEF", "ABCG"   },
            { L_,  "DEF", "A",   "ADEF",   "",     "ADEF",   "DEF", "ABCG"   },
            { L_,  "DEF", "B",   "BDEF",   "",     "BDEF",   "DEF", "ABCG"   },
            { L_,  "DEF", "AB",  "ABDEF",  "",     "ABDEF",  "DEF", "ABCG"   },
            { L_,  "DEF", "BC",  "BCDEF",  "",     "BCDEF",  "DEF", "ABCG"   },
            { L_,  "DEF", "CD",  "CDEF",   "D",    "CEF",    "EF",  "ABCG"   },
            { L_,  "DEF", "ABC", "ABCDEF", "",     "ABCDEF", "DEF", "ABCG"   },
            { L_,  "DEF", "BCD", "BCDEF",  "D",    "BCEF",   "EF",  "ABCG"   },
            { L_,  "DEF", "CDE", "CDEF",   "DE",   "CF",     "F",   "ABCG"   },
            { L_,  "DEF", "DEF", "DEF",    "DEF",  "",       "",    "ABCG"   }
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *SPEC_A = DATA[ti].d_specA;
                const char *SPEC_B = DATA[ti].d_specB;
                const char *SPEC_U = DATA[ti].d_specU;
                const char *SPEC_I = DATA[ti].d_specI;
                const char *SPEC_E = DATA[ti].d_specE;
                const char *SPEC_S = DATA[ti].d_specS;
                const char *SPEC_C = DATA[ti].d_specC;

                const int curLen = strlen(SPEC_A);
                if (curLen != oldLen) {
                    if (veryVerbose) cout << "\tUsing lhs objects of length "
                                          << curLen << '.' << endl;
                    ASSERTV(ti, oldLen <= curLen); // non-decreasing
                    oldLen = curLen;
                }

                const Obj U = g(SPEC_U);
                const Obj I = g(SPEC_I);
                const Obj E = g(SPEC_E);
                const Obj S = g(SPEC_S);
                const Obj C = g(SPEC_C);

                Obj mX;  const Obj& X = mX;
                gg(&mX, SPEC_A);
                const Obj XX(X);

                Obj mY;  const Obj& Y = mY;
                gg(&mY, SPEC_B);
                const Obj YY(Y);

                // testing 'operator|' and 'operator|='
                {
                    const Obj& Z = X | Y;
                    ASSERTV(LINE, U == Z);
                    ASSERTV(&Z != &X);
                    ASSERTV(&Z != &Y);

                    Obj mR;  const Obj& R = mR;
                    mR = X;

                    Obj& K = mR |= Y;
                    ASSERTV(LINE, U == R);
                    ASSERTV(&K == &R);

                    ASSERTV(LINE, XX == X);
                    ASSERTV(LINE, YY == Y);

                    // Alias test

                    Obj mA(X);  const Obj& A = mA;

                    mA |= mA;
                    ASSERTV(LINE, X == A);
                }

                // testing 'operator&' and 'operator&='
                {
                    const Obj& Z = X & Y;
                    ASSERTV(LINE, I == Z);
                    ASSERTV(&Z != &X);
                    ASSERTV(&Z != &Y);

                    Obj mR;  const Obj& R = mR;
                    mR = X;

                    Obj& K = mR &= Y;
                    ASSERTV(LINE, I == R);
                    ASSERTV(&K == &R);

                    ASSERTV(LINE, XX == X);
                    ASSERTV(LINE, YY == Y);

                    // Alias test

                    Obj mA(X);  const Obj& A = mA;

                    mA &= mA;
                    ASSERTV(LINE, X == A);
                }

                // testing 'operator^' and 'operator^='
                {
                    const Obj& Z = X ^ Y;
                    ASSERTV(LINE, E == Z);
                    ASSERTV(&Z != &X);
                    ASSERTV(&Z != &Y);

                    Obj mR;  const Obj& R = mR;
                    mR = X;

                    Obj& K = mR ^= Y;
                    ASSERTV(LINE, E == R);
                    ASSERTV(&K == &R);

                    ASSERTV(LINE, XX == X);
                    ASSERTV(LINE, YY == Y);

                    // Alias test

                    Obj mA(X);  const Obj& A = mA;

                    mA ^= mA;
                    ASSERTV(LINE, Obj() == A);
                }

                // testing 'operator-' and 'operator-='
                {
                    const Obj& Z = X - Y;
                    ASSERTV(LINE, S == Z);
                    ASSERTV(&Z != &X);
                    ASSERTV(&Z != &Y);

                    Obj mR;  const Obj& R = mR;
                    mR = X;

                    Obj& K = mR -= Y;
                    ASSERTV(LINE, S == R);
                    ASSERTV(&K == &R);

                    ASSERTV(LINE, XX == X);
                    ASSERTV(LINE, YY == Y);

                    // Alias test

                    Obj mA(X);  const Obj& A = mA;

                    mA -= mA;
                    ASSERTV(LINE, Obj() == A);
                }

                // testing 'operator~'
                {
                    const Obj& Z = ~X;
                    ASSERTV(LINE, C  == Z);
                    ASSERTV(LINE, XX == X);
                    ASSERTV(&Z != &X);

                    Obj mA;  const Obj& A = mA;
                    mA = X;

                    mA = ~mA; // alias test
                    ASSERTV(LINE, C == A);
                }
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'areMembers'
        //
        // Concerns:
        //: 1 'X.areMembers(Y)' returns 'true' if all the days in 'Y' are set
        //:   in 'X' and 'false' otherwise.
        //:
        //: 2 Neither 'X' nor 'Y' are modified by 'X.areMembers(Y)'.
        //:
        //: 3 Any valid 'DayOfWeekSet' 'areMembers' itself.
        //
        // Plan:
        //   Iterate through a table of spec pairs 'specA' and 'specB', along
        //   with 'LE", a bool that indicates that the days in 'specA' are a
        //   (possibly improper) subset of the days in 'specB', and a bool 'GE'
        //   that indicates vice-versa.  For each line of the table:
        //: 1 Evaluate 'X.areMembers(Y)' and 'Y.areMembers(X)' and verify that
        //:   they return the boolean values predicted by 'LE' and 'GE' (C-1).
        //:
        //: 2 Prior to doing that, copy-construct copies of 'X' and 'Y', then
        //:   after doing it, compare them to the current values of 'X' and 'Y'
        //:   and verify they haven't changed (C-2).
        //:
        //: 3 Verify that all sets 'areMembers' of themselves. (C-3)
        //
        // Testing:
        //   bool areMembers(const DayOfWeekSet& set) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'areMembers'" << endl
                          << "====================" << endl;

        if (verbose) cout << "\nTesting 'areMembers'." << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_specA;    // source string for A
                const char *d_specB;    // source string for B
                int         d_LE;       // expected rv for B.areMembers(A)
                int         d_GE;       // expected rv for A.areMembers(B)
            } DATA[] = {
                //line   A       B     LE  GE
                //---- ------  ------  --  --
                { L_,  "",     "",      1,  1 },
                { L_,  "",     "A",     1,  0 },
                { L_,  "",     "B",     1,  0 },
                { L_,  "",     "AB",    1,  0 },
                { L_,  "",     "BC",    1,  0 },
                { L_,  "",     "CD",    1,  0 },
                { L_,  "",     "ABC",   1,  0 },
                { L_,  "",     "BCD",   1,  0 },
                { L_,  "",     "CDE",   1,  0 },
                { L_,  "",     "DEF",   1,  0 },
                { L_,  "",     "ABCD",  1,  0 },
                { L_,  "",     "BCDE",  1,  0 },
                { L_,  "",     "CDEF",  1,  0 },
                { L_,  "",     "DEFG",  1,  0 },

                { L_,  "A",    "A",     1,  1 },
                { L_,  "A",    "B",     0,  0 },
                { L_,  "A",    "AB",    1,  0 },
                { L_,  "A",    "BC",    0,  0 },
                { L_,  "A",    "CD",    0,  0 },
                { L_,  "A",    "ABC",   1,  0 },
                { L_,  "A",    "BCD",   0,  0 },
                { L_,  "A",    "CDE",   0,  0 },
                { L_,  "A",    "DEF",   0,  0 },
                { L_,  "A",    "ABCD",  1,  0 },
                { L_,  "A",    "BCDE",  0,  0 },
                { L_,  "A",    "CDEF",  0,  0 },
                { L_,  "A",    "DEFG",  0,  0 },

                { L_,  "B",    "B",     1,  1 },
                { L_,  "B",    "AB",    1,  0 },
                { L_,  "B",    "BC",    1,  0 },
                { L_,  "B",    "CD",    0,  0 },
                { L_,  "B",    "ABC",   1,  0 },
                { L_,  "B",    "BCD",   1,  0 },
                { L_,  "B",    "CDE",   0,  0 },
                { L_,  "B",    "DEF",   0,  0 },
                { L_,  "B",    "ABCD",  1,  0 },
                { L_,  "B",    "BCDE",  1,  0 },
                { L_,  "B",    "CDEF",  0,  0 },
                { L_,  "B",    "DEFG",  0,  0 },

                { L_,  "AB",   "AB",    1,  1 },
                { L_,  "AB",   "BC",    0,  0 },
                { L_,  "AB",   "CD",    0,  0 },
                { L_,  "AB",   "ABC",   1,  0 },
                { L_,  "AB",   "BCD",   0,  0 },
                { L_,  "AB",   "CDE",   0,  0 },
                { L_,  "AB",   "DEF",   0,  0 },
                { L_,  "AB",   "ABCD",  1,  0 },
                { L_,  "AB",   "BCDE",  0,  0 },
                { L_,  "AB",   "CDEF",  0,  0 },
                { L_,  "AB",   "DEFG",  0,  0 },

                { L_,  "BC",   "BC",    1,  1 },
                { L_,  "BC",   "CD",    0,  0 },
                { L_,  "BC",   "ABC",   1,  0 },
                { L_,  "BC",   "BCD",   1,  0 },
                { L_,  "BC",   "CDE",   0,  0 },
                { L_,  "BC",   "DEF",   0,  0 },
                { L_,  "BC",   "ABCD",  1,  0 },
                { L_,  "BC",   "BCDE",  1,  0 },
                { L_,  "BC",   "CDEF",  0,  0 },
                { L_,  "BC",   "DEFG",  0,  0 },

                { L_,  "CD",   "CD",    1,  1 },
                { L_,  "CD",   "ABC",   0,  0 },
                { L_,  "CD",   "BCD",   1,  0 },
                { L_,  "CD",   "CDE",   1,  0 },
                { L_,  "CD",   "DEF",   0,  0 },
                { L_,  "CD",   "ABCD",  1,  0 },
                { L_,  "CD",   "BCDE",  1,  0 },
                { L_,  "CD",   "CDEF",  1,  0 },
                { L_,  "CD",   "DEFG",  0,  0 },

                { L_,  "ABC",  "ABC",   1,  1 },
                { L_,  "ABC",  "BCD",   0,  0 },
                { L_,  "ABC",  "CDE",   0,  0 },
                { L_,  "ABC",  "DEF",   0,  0 },
                { L_,  "ABC",  "ABCD",  1,  0 },
                { L_,  "ABC",  "BCDE",  0,  0 },
                { L_,  "ABC",  "CDEF",  0,  0 },
                { L_,  "ABC",  "DEFG",  0,  0 },

                { L_,  "BCD",  "BCD",   1,  1 },
                { L_,  "BCD",  "CDE",   0,  0 },
                { L_,  "BCD",  "DEF",   0,  0 },
                { L_,  "BCD",  "ABCD",  1,  0 },
                { L_,  "BCD",  "BCDE",  1,  0 },
                { L_,  "BCD",  "CDEF",  0,  0 },
                { L_,  "BCD",  "DEFG",  0,  0 },

                { L_,  "CDE",  "CDE",   1,  1 },
                { L_,  "CDE",  "DEF",   0,  0 },
                { L_,  "CDE",  "ABCD",  0,  0 },
                { L_,  "CDE",  "BCDE",  1,  0 },
                { L_,  "CDE",  "CDEF",  1,  0 },
                { L_,  "CDE",  "DEFG",  0,  0 },

                { L_,  "DEF",  "DEF",   1,  1 },
                { L_,  "DEF",  "ABCD",  0,  0 },
                { L_,  "DEF",  "BCDE",  0,  0 },
                { L_,  "DEF",  "CDEF",  1,  0 },
                { L_,  "DEF",  "DEFG",  1,  0 },

                { L_,  "ABCD", "ABCD",  1,  1 },
                { L_,  "ABCD", "BCDE",  0,  0 },
                { L_,  "ABCD", "CDEF",  0,  0 },
                { L_,  "ABCD", "DEFG",  0,  0 },

                { L_,  "BCDE", "BCDE",  1,  1 },
                { L_,  "BCDE", "CDEF",  0,  0 },
                { L_,  "BCDE", "DEFG",  0,  0 },

                { L_,  "CDEF", "CDEF",  1,  1 },
                { L_,  "CDEF", "DEFG",  0,  0 },

                { L_,  "DEFG", "DEFG",  1,  1 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *SPEC_A = DATA[ti].d_specA;
                const char *SPEC_B = DATA[ti].d_specB;
                const int   LE     = DATA[ti].d_LE;
                const int   GE     = DATA[ti].d_GE;

                const int curLen = strlen(SPEC_A);
                if (curLen != oldLen) {
                    if (veryVerbose) cout << "\tUsing lhs objects of length "
                                          << curLen << '.' << endl;
                    ASSERTV(LINE, oldLen <= curLen); // non-decreasing
                    oldLen = curLen;
                }

                Obj mX;  const Obj& X = mX;
                gg(&mX, SPEC_A);
                const Obj XX(X);

                Obj mY;  const Obj& Y = mY;
                gg(&mY, SPEC_B);
                const Obj YY(Y);

                ASSERTV(LINE, LE == Y.areMembers(X));
                ASSERTV(LINE, GE == X.areMembers(Y));
                ASSERTV(LINE, 1  == X.areMembers(X));
                ASSERTV(LINE, 1  == Y.areMembers(Y));
                ASSERTV(LINE, XX == X);
                ASSERTV(LINE, YY == Y);
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING ADD AND REMOVE
        //
        // Concerns:
        //: 1 An element is always a member after it has been added with 'add'.
        //:
        //: 2 An element is never a member after it has been removed.
        //:
        //: 3 'remove' properly returns 'true' only if the day removed was
        //:   previously an element.
        //:
        //: 4 Redundant 'add's don't change the value of an object.
        //:
        //: 5 Redundant 'remove's don't change the value of an object.
        //
        // Plan:
        //   For a variety of initials values driven by the 'SPECS" array:
        //: 1 iterate through all the possible days of the week
        //:   o Check if the day is set
        //:   o 'remove' the day
        //:   o verify the day is no longer a member, regardless whether it was
        //:     in the first place (C-2)
        //:   o verify that the value returned by 'remove' corresponds
        //:     properly with whether the day was set (C--3)
        //:   o do a redundant 'remove' and observe that it returns 'false'
        //:     and does not change the value of the object (C-5)
        //:   o 'add' the day again.
        //:   o verify that the day is not a member (C-1)
        //:   o do a redundant 'add'.
        //:   o verify the value has not been changed (C-4).
        //:   o reset the object to the specification
        //:   o iterate through the days, 'remove'ing days from the object,
        //:     observing that the value of the object and the return value
        //:     of 'remove' are as they should be (C-2, C-3, C-5);
        //
        // Testing:
        //   void add(DayOfWeek value);
        //   int remove(DayOfWeek value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ADD AND REMOVE" << endl
                          << "======================" << endl;

        if (verbose) cout <<
            "\nTesting x.add(value) and x.remove(value)" << endl;
        {
            static const char *SPECS[] = {
                "", "A", "B", "C", "AB", "BC", "AC", "ABC", "ACD", "ADE",
                "BCD", "CDE", "BDE", "ABCD", "ACDE", "ACEF", "BCDE", "BDEF",
                "DEFG", "ABCDE", "ACDEF", "ACDFG", "BCDEF", "CDEFG",
                "ABCDEF", "ACDEFG", "ABDEFG", "ABCEFG", "ABCDFG", "ABCDEG",
                "ABCDEFG",
            0 }; // Null string required as last element.

            const Obj& FULL_WEEK = g("ABCDEFG");

            int oldLen = -1;
            int k;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC   = SPECS[ti];
                const int         curLen = strlen(SPEC);
                const bool isFullWeek    = !strcmp(SPEC, "ABCDEFG");

                if (veryVerbose) cout << "\t\tFor objects of initial length "
                                      << curLen << '.' << endl;
                ASSERTV(SPEC, oldLen <= curLen); // non-decreasing
                oldLen = curLen;

                if (veryVerbose) { cout << "\t\t\t"; P(SPEC); }

                Obj mX;  const Obj& X = mX;
                gg(&mX, SPEC);
                ASSERTV(ti, curLen == X.length()); // same lengths
                ASSERT(isFullWeek == (FULL_WEEK == X));

                // test adding and removing
                for (k = 0; k < NUM_VALUES; ++k) {
                    int rv;
                    int initialLength = X.length();

                    if (veryVeryVerbose) { P_(VALUES[k]); };

                    int erv = !X.isMember(VALUES[k]);

                    rv = mX.remove(VALUES[k]);
                    ASSERTV(ti, k, (!erv) == rv);
                    ASSERTV(X.length() == initialLength - rv);

                    ASSERT(!X.isMember(VALUES[k]));

                    const Obj without = X;

                    rv = mX.remove(VALUES[k]);        // redundant remove
                    ASSERTV(ti, k, 0 == rv);

                    ASSERT(without == X);

                    ASSERT(!X.isMember(VALUES[k]));
                    mX.add(VALUES[k]);
                    ASSERT(X.isMember(VALUES[k]));

                    const Obj with = X;

                    mX.add(VALUES[k]);                // redundant ADD
                    ASSERT(X.isMember(VALUES[k]));

                    ASSERT(with == X);

                    rv = mX.remove(VALUES[k]);
                    ASSERTV(ti, k, 1 == rv);
                    ASSERT(!X.isMember(VALUES[k]));

                    ASSERT(without == X);

                    mX.add(VALUES[k]);
                    ASSERT(X.isMember(VALUES[k]));

                    ASSERT(with == X);

                    ASSERTV(ti, k, initialLength + erv == X.length());
                    ASSERTV(X.length() >= k + 1);
                }
                if (veryVeryVerbose) cout << endl;

                ASSERT(FULL_WEEK == X);
                ASSERT(7 == X.length());

                gg(&mX, SPEC);

                ASSERT(isFullWeek == (FULL_WEEK == X));

                // test removing everything
                for (k = 0; k < NUM_VALUES; ++k) {
                    if (veryVeryVerbose) { P_(VALUES[k]); };

                    int initialLength = X.length();
                    int erv = X.isMember(VALUES[k]);
                    int rv  = mX.remove(VALUES[k]);
                    ASSERTV(ti, k, erv == rv);
                    ASSERTV(ti, k, initialLength - erv == X.length());
                    ASSERTV(X.length() <= 6 - k);
                }
                if (veryVeryVerbose) cout << endl;

                ASSERTV(X == Obj());
                ASSERTV(0 == X.length());
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING BDEX STREAMING
        //   Verify the BDEX streaming implementation works correctly.
        //   Specific concerns include wire format, handling of stream states
        //   (valid, empty, invalid, incomplete, and corrupted), and exception
        //   neutrality.
        //
        // Concerns:
        //: 1 The class method 'maxSupportedBdexVersion' returns the correct
        //:   version to be used for the specified 'versionSelector'.
        //:
        //: 2 The 'bdexStreamOut' method is callable on a reference providing
        //:   only non-modifiable access.
        //:
        //: 3 For valid streams, externalization and unexternalization are
        //:   inverse operations.
        //:
        //: 4 For invalid streams, externalization leaves the stream invalid
        //:   and unexternalization does not alter the value of the object and
        //:   leaves the stream invalid.
        //:
        //: 5 Unexternalizing of incomplete, invalid, or corrupted data results
        //:   in a valid object of unspecified value and an invalidated stream.
        //:
        //: 6 The wire format of the object is as expected.
        //:
        //: 7 All methods are exception neutral.
        //:
        //: 8 The 'bdexStreamIn' and 'bdexStreamOut' methods return a reference
        //:   to the provided stream in all situations.
        //:
        //: 9 The initial value of the object has no affect on
        //:   unexternalization.
        //
        // Plan:
        //: 1 Test 'maxSupportedBdexVersion' explicitly.  (C-1)
        //:
        //: 2 All calls to the 'bdexStreamOut' accessor will be done from a
        //:   'const' object or reference and all calls to the 'bdexStreamOut'
        //:   free function (provided by 'bslx') will be supplied a 'const'
        //:   object or reference.  (C-2)
        //:
        //: 3 Perform a direct test of the 'bdexStreamOut' and 'bdexStreamIn'
        //:   methods (the rest of the testing will use the free functions
        //:   'bslx::OutStreamFunctions::bdexStreamOut' and
        //:   'bslx::InStreamFunctions::bdexStreamIn').
        //:
        //: 4 Define a set 'S' of test values to be used throughout the test
        //:   case.
        //:
        //: 5 For all '(u, v)' in the cross product 'S X S', stream the value
        //:   of 'u' into (a temporary copy of) 'v', 'T', and assert 'T == u'.
        //:   (C-3, 9)
        //:
        //: 6 For all 'u' in 'S', create a copy of 'u' and attempt to stream
        //:   into it from an invalid stream.  Verify after each attempt that
        //:   the object is unchanged and that the stream is invalid.  (C-4)
        //:
        //: 7 Write 3 distinct objects to an output stream buffer of total
        //:   length 'N'.  For each partial stream length from 0 to 'N - 1',
        //:   construct an input stream and attempt to read into objects
        //:   initialized with distinct values.  Verify values of objects
        //:   that are either successfully modified or left entirely
        //:   unmodified, and that the stream became invalid immediately after
        //:   the first incomplete read.  Finally, ensure that each object
        //:   streamed into is in some valid state.
        //:
        //: 8 Use the underlying stream package to simulate a typical valid
        //:   (control) stream and verify that it can be streamed in
        //:   successfully.  Then for each data field in the stream (beginning
        //:   with the version number), provide one or more similar tests with
        //:   that data field corrupted.  After each test, verify that the
        //:   object is in some valid state after streaming, and that the
        //:   input stream has become invalid.  (C-5)
        //:
        //: 9 Explicitly test the wire format.  (C-6)
        //:
        //:10 In all cases, confirm exception neutrality using the specially
        //:   instrumented 'bslx::TestInStream' and a pair of standard macros,
        //:   'BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN' and
        //:   'BSLX_TESTINSTREAM_EXCEPTION_TEST_END', which configure the
        //:   'bslx::TestInStream' object appropriately in a loop.  (C-7)
        //:
        //:11 In all cases, verify the return value of the tested method.
        //:   (C-8)
        //
        // Testing:
        //   static int maxSupportedBdexVersion(int versionSelector);
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // --------------------------------------------------------------------

        // Allocator to use instead of the default allocator.
        bslma::TestAllocator allocator("bslx", veryVeryVeryVerbose);

        if (verbose) cout << endl
                          << "TESTING BDEX STREAMING" << endl
                          << "======================" << endl;

        if (verbose) {
            cout << "\nTesting 'maxSupportedBdexVersion'." << endl;
        }
        {
            ASSERT(1 == Obj::maxSupportedBdexVersion(0));
            ASSERT(1 == Obj::maxSupportedBdexVersion(VERSION_SELECTOR));

            using bslx::VersionFunctions::maxSupportedBdexVersion;

            ASSERT(1 == maxSupportedBdexVersion(reinterpret_cast<Obj *>(0),
                                                0));
            ASSERT(1 == maxSupportedBdexVersion(reinterpret_cast<Obj *>(0),
                                                VERSION_SELECTOR));
        }

        const int VERSION = Obj::maxSupportedBdexVersion(0);

        if (verbose) {
            cout << "\nDirect initial trial of 'bdexStreamOut' and (valid) "
                 << "'bdexStreamIn' functionality." << endl;
        }
        {
            Obj mX;  const Obj& X = mX;
            mX.add(VC);

            Out       out(VERSION_SELECTOR, &allocator);

            Out& rvOut = X.bdexStreamOut(out, VERSION);
            ASSERT(&out == &rvOut);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            In in(OD, LOD);
            ASSERT(in);
            ASSERT(!in.isEmpty());

            Obj mT;  const Obj& T = mT;
            mT.add(VA);
            ASSERT(X != T);

            In& rvIn = mT.bdexStreamIn(in, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(X == T);
            ASSERT(in);
            ASSERT(in.isEmpty());
        }

        // We will use the stream free functions provided by 'bslx', as opposed
        // to the class member functions, since the 'bslx' implementation gives
        // priority to the free function implementations; we want to test what
        // will be used.  Furthermore, toward making this test case more
        // reusable in other components, from here on we generally use the
        // 'bdexStreamIn' and 'bdexStreamOut' free functions that are defined
        // in the 'bslx' package rather than call the like-named member
        // functions directly.

        if (verbose) {
            cout << "\nThorough test using stream free functions."
                 << endl;
        }
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj mX;  const Obj& X = mX;
                mX.add(VALUES[i]);

                Out out(VERSION_SELECTOR, &allocator);

                using bslx::OutStreamFunctions::bdexStreamOut;
                using bslx::InStreamFunctions::bdexStreamIn;

                Out& rvOut = bdexStreamOut(out, X, VERSION);
                LOOP_ASSERT(i, &out == &rvOut);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value and
                // that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    In in(OD, LOD);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());

                    Obj mT;  const Obj& T = mT;
                    mT.add(VALUES[j]);
                    LOOP2_ASSERT(i, j, (X == T) == (i == j));

                    BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                        in.reset();
                        In& rvIn = bdexStreamIn(in, mT, VERSION);
                        LOOP2_ASSERT(i, j, &in == &rvIn);
                    } BSLX_TESTINSTREAM_EXCEPTION_TEST_END

                    LOOP2_ASSERT(i, j, X == T);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        if (verbose) {
            cout << "\tOn empty streams and non-empty, invalid streams."
                 << endl;
        }

        // Verify correct behavior for empty streams (valid and invalid).

        {
            Out               out(VERSION_SELECTOR, &allocator);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                using bslx::InStreamFunctions::bdexStreamIn;

                Obj mX;  const Obj& X = mX;
                mX.add(VALUES[i]);

                Obj        mT(X);
                const Obj& T = mT;
                LOOP_ASSERT(i, X == T);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();

                    // Stream is valid.
                    In& rvIn1 = bdexStreamIn(in, mT, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn1);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);

                    // Stream is invalid.
                    In& rvIn2 = bdexStreamIn(in, mT, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn2);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        // Verify correct behavior for non-empty, invalid streams.

        {
            Out               out(VERSION_SELECTOR, &allocator);

            using bslx::OutStreamFunctions::bdexStreamOut;
            Out& rvOut = bdexStreamOut(out, Obj(), VERSION);
            ASSERT(&out == &rvOut);

            const char *const OD  = out.data();
            const int         LOD = out.length();
            ASSERT(0 < LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                in.invalidate();
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, !in.isEmpty());

                // Ensure that reading from a non-empty, invalid input stream
                // leaves the stream invalid and the target object unchanged.

                using bslx::InStreamFunctions::bdexStreamIn;

                Obj mX;  const Obj& X = mX;
                mX.add(VALUES[i]);

                Obj        mT(X);
                const Obj& T = mT;
                LOOP_ASSERT(i, X == T);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();
                    in.invalidate();
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, !in.isEmpty());

                    In& rvIn = bdexStreamIn(in, mT, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        if (verbose) {
            cout << "\tOn incomplete (but otherwise valid) data."
                 << endl;
        }
        {
            Obj mW1;  const Obj& W1 = mW1;
            mW1.add(VA);

            Obj mX1;  const Obj& X1 = mX1;
            mW1.add(VB);

            Obj mW2;  const Obj& W2 = mW2;
            mW2.add(VB);

            Obj mX2;  const Obj& X2 = mX2;
            mX2.add(VC);

            Obj mW3;  const Obj& W3 = mW3;
            mW3.add(VC);

            Obj mX3;  const Obj& X3 = mX3;
            mX3.add(VD);

            using bslx::OutStreamFunctions::bdexStreamOut;
            using bslx::InStreamFunctions::bdexStreamIn;

            Out out(VERSION_SELECTOR, &allocator);

            Out& rvOut1 = bdexStreamOut(out, X1, VERSION);
            ASSERT(&out == &rvOut1);
            const int         LOD1 = out.length();

            Out& rvOut2 = bdexStreamOut(out, X2, VERSION);
            ASSERT(&out == &rvOut2);
            const int         LOD2 = out.length();

            Out& rvOut3 = bdexStreamOut(out, X3, VERSION);
            ASSERT(&out == &rvOut3);
            const int         LOD3 = out.length();
            const char *const OD3  = out.data();

            for (int i = 0; i < LOD3; ++i) {
                In in(OD3, i);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();
                    LOOP_ASSERT(i, in);
                    LOOP_ASSERT(i, !i == in.isEmpty());

                    Obj mT1(W1);  const Obj& T1 = mT1;
                    Obj mT2(W2);  const Obj& T2 = mT2;
                    Obj mT3(W3);  const Obj& T3 = mT3;

                    if (i < LOD1) {
                        In& rvIn1 = bdexStreamIn(in, mT1, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn1);
                        LOOP_ASSERT(i, !in);
                        if (0 == i) LOOP_ASSERT(i, W1 == T1);
                        In& rvIn2 = bdexStreamIn(in, mT2, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn2);
                        LOOP_ASSERT(i, !in);
                        LOOP_ASSERT(i, W2 == T2);
                        In& rvIn3 = bdexStreamIn(in, mT3, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn3);
                        LOOP_ASSERT(i, !in);
                        LOOP_ASSERT(i, W3 == T3);
                    }
                    else if (i < LOD2) {
                        In& rvIn1 = bdexStreamIn(in, mT1, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn1);
                        LOOP_ASSERT(i,  in);
                        LOOP_ASSERT(i, X1 == T1);
                        In& rvIn2 = bdexStreamIn(in, mT2, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn2);
                        LOOP_ASSERT(i, !in);
                        if (LOD1 <= i) LOOP_ASSERT(i, W2 == T2);
                        In& rvIn3 = bdexStreamIn(in, mT3, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn3);
                        LOOP_ASSERT(i, !in);
                        LOOP_ASSERT(i, W3 == T3);
                    }
                    else {  // 'LOD2 <= i < LOD3'
                        In& rvIn1 = bdexStreamIn(in, mT1, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn1);
                        LOOP_ASSERT(i,  in);
                        LOOP_ASSERT(i, X1 == T1);
                        In& rvIn2 = bdexStreamIn(in, mT2, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn2);
                        LOOP_ASSERT(i,  in);
                        LOOP_ASSERT(i, X2 == T2);
                        In& rvIn3 = bdexStreamIn(in, mT3, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn3);
                        LOOP_ASSERT(i, !in);
                        if (LOD2 <= i) LOOP_ASSERT(i, W3 == T3);
                    }

                    // Verify the objects are in a valid state.

                    LOOP_ASSERT(i, false == T1.isMember(
                                       static_cast<bdlt::DayOfWeek::Enum>(0)));

                    LOOP_ASSERT(i, false == T2.isMember(
                                       static_cast<bdlt::DayOfWeek::Enum>(0)));

                    LOOP_ASSERT(i, false == T3.isMember(
                                       static_cast<bdlt::DayOfWeek::Enum>(0)));

                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        if (verbose) {
            cout << "\tOn corrupted data." << endl;
        }

        const Obj W;                   // default value

        Obj mX;  const Obj& X = mX;    // original (control)
        mX.add(VA);

        Obj mY;  const Obj& Y = mY;    // new (streamed-out)
        mY.add(VB);

        // Verify the three objects are distinct.
        ASSERT(W != X);
        ASSERT(W != Y);
        ASSERT(X != Y);

        const int SERIAL_Y = 8;       // internal rep. of 'Y'

        if (verbose) {
            cout << "\t\tGood stream (for control)." << endl;
        }
        {
            Out out(VERSION_SELECTOR, &allocator);
            out.putUint8(SERIAL_Y);  // Stream out "new" value.
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(in);
            ASSERT(Y == T);
        }

        if (verbose) {
            cout << "\t\tBad version." << endl;
        }
        {
            const char version = 0; // too small ('version' must be >= 1)

            Out out(VERSION_SELECTOR, &allocator);
            out.putUint8(SERIAL_Y);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, version);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }
        {
            const char version = 2 ; // too large (current version is 1)

            Out out(VERSION_SELECTOR, &allocator);
            out.putUint8(SERIAL_Y);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, version);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
            cout << "\t\tValue must not have least-significant bit set."
                 << endl;
        }
        {
            Out out(VERSION_SELECTOR, &allocator);
            out.putUint8(1);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
             cout << "\nWire format direct tests." << endl;
        }
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_version;  // version to stream with
                int         d_length;   // expect output length
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //LINE  SPEC       VER  LEN  FORMAT
                //----  ---------  ---  ---  ------
                { L_,          "",   1,   1,  "\x00"  },
                { L_,         "A",   1,   1,  "\x02"  },
                { L_,         "D",   1,   1,  "\x04"  },
                { L_,         "B",   1,   1,  "\x08"  },
                { L_,        "AD",   1,   1,  "\x06"  },
                { L_,       "ADB",   1,   1,  "\x0e"  }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const char *const SPEC    = DATA[i].d_spec_p;
                const int         VERSION = DATA[i].d_version;
                const int         LEN     = DATA[i].d_length;
                const char *const FMT     = DATA[i].d_fmt_p;

                // Test using class methods.

                {
                    Obj mX;  const Obj& X = gg(&mX, SPEC);

                    bslx::ByteOutStream  out(VERSION_SELECTOR, &allocator);
                    bslx::ByteOutStream& rvOut = X.bdexStreamOut(out, VERSION);
                    LOOP_ASSERT(LINE, &out == &rvOut);

                    LOOP_ASSERT(LINE, LEN == out.length());
                    LOOP_ASSERT(LINE, 0 == memcmp(out.data(), FMT, LEN));

                    if (verbose && memcmp(out.data(), FMT, LEN)) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (int j = 0; j < out.length(); ++j) {
                            cout << "\\x"
                                 << hex[static_cast<unsigned char>
                                            ((*(out.data() + j) >> 4) & 0x0f)]
                                 << hex[static_cast<unsigned char>
                                                   (*(out.data() + j) & 0x0f)];
                        }
                        cout << endl;
                    }

                    Obj mY;  const Obj& Y = mY;

                    bslx::ByteInStream  in(out.data(), out.length());
                    bslx::ByteInStream& rvIn = mY.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(LINE, &in == &rvIn);
                    LOOP_ASSERT(LINE, X == Y);
                }

                // Test using free functions.

                {
                    Obj mX;  const Obj& X = gg(&mX, SPEC);

                    using bslx::OutStreamFunctions::bdexStreamOut;

                    bslx::ByteOutStream  out(VERSION_SELECTOR, &allocator);
                    bslx::ByteOutStream& rvOut = bdexStreamOut(out,
                                                               X,
                                                               VERSION);
                    LOOP_ASSERT(LINE, &out == &rvOut);

                    LOOP_ASSERT(LINE, LEN == out.length());
                    LOOP_ASSERT(LINE, 0 == memcmp(out.data(), FMT, LEN));

                    if (verbose && memcmp(out.data(), FMT, LEN)) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (int j = 0; j < out.length(); ++j) {
                            cout << "\\x"
                                 << hex[static_cast<unsigned char>
                                            ((*(out.data() + j) >> 4) & 0x0f)]
                                 << hex[static_cast<unsigned char>
                                                   (*(out.data() + j) & 0x0f)];
                        }
                        cout << endl;
                    }

                    Obj mY;  const Obj& Y = mY;

                    using bslx::InStreamFunctions::bdexStreamIn;

                    bslx::ByteInStream  in(out.data(), out.length());
                    bslx::ByteInStream& rvIn = bdexStreamIn(in, mY, VERSION);
                    LOOP_ASSERT(LINE, &in == &rvIn);
                    LOOP_ASSERT(LINE, X == Y);
                }
            }
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //
        // Concerns:
        //   We have the following concerns:
        //: 1 The value represented by any instance can be assigned to any
        //:   other value.
        //:
        //: 2 The 'rhs' value must not be affected by the operation.
        //:
        //: 3 'rhs' being modified or going out of scope has no effect on the
        //:   value of 'lhs' after the assignment.
        //:
        //: 4 The assignment operator returns a reference providing modifiable
        //:   access to the object.
        //:
        //: 5 Aliasing (x = x): The assignment operator must always work.
        //
        // Plan:
        //: 1 Declare an array of 'gg' specs, and iterate two nested loops
        //:   through them.  The specs associated with the outer loop will
        //:   represent the value to be written over, the value of the inner
        //:   loop will represent the value to be assigned to.
        //:
        //: 2 Do the assignment, keeping a reference to the result of the
        //:   assignment.
        //:
        //: 3 Verify the reference points to the modified object (C-4)
        //:
        //: 4 Verify 'lhs' is equivalent to the value that should have been
        //:   assigned (C-1).
        //:
        //: 5 Verify the 'rhs' is unchanged (C-2);
        //:
        //: 6 Modify the 'rhs' variable by running 'gg' on it.
        //:
        //: 7 Verify the 'lhs' was unchanged by modifying 'rhs'.  (C-3)
        //:
        //: 8 Have the 'rhs' go out of scope, and again verify that 'lhs' was
        //:   unchanged by this (C-3).
        //:
        //: 9 Have another loop iterate through the specs, assigning an object
        //
        // Testing:
        //   DayOfWeekSet& operator=(const DayOfWeekSet& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ASSIGNMENT OPERATOR" << endl
                          << "===========================" << endl;

        static const char *SPECS[] = {
            "", "A", "B", "C", "AB", "BC", "AC", "ABC", "ACD", "ADE",
            "BCD", "CDE", "BDE", "ABCD", "ACDE", "ACEF", "BCDE", "BDEF",
            "DEFG", "ABCDE", "ACDEF", "ACDFG", "BCDEF", "CDEFG",
            "ABCDEF", "ACDEFG", "ABDEFG", "ABCEFG", "ABCDFG", "ABCDEG",
            "ABCDEFG",
        0 }; // Null string required as last element.

        if (verbose) cout <<
            "\nAssign cross product of values with varied representations."
                                                                       << endl;
        {
            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int         uLen   = strlen(U_SPEC);

                if (verbose && uLen > uOldLen) {
                    cout << "\tFor lhs objects of length " << uLen << ":\t";
                    P(U_SPEC);
                }

                ASSERTV(U_SPEC, ui, uOldLen <= uLen);  // non-decreasing
                uOldLen = uLen;

                const Obj& UU = g(U_SPEC);               // control
                ASSERTV(ui, uLen == UU.length());   // same lengths

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const int         vLen   = strlen(V_SPEC);

                    if (veryVerbose) {
                        cout << "\t\tFor rhs objects of length " << vLen
                                                                 << ":\t";
                        P(V_SPEC);
                    }

                    const Obj& VV = g(V_SPEC);           // control

                    const int Z = ui == vi; // flag indicating same values

                    Obj mU;
                    const Obj& U = mU; gg(&mU, U_SPEC);

                    {
                        Obj mV;    const Obj& V = mV;
                        gg(&mV, V_SPEC);

                        ASSERTV(U_SPEC, V_SPEC, UU == U);
                        ASSERTV(U_SPEC, V_SPEC, VV == V);
                        ASSERTV(U_SPEC, V_SPEC, Z==(V==U));

                        Obj& R = mU = V; // test assignment here

                        ASSERTV(U_SPEC, V_SPEC, &R == &U);
                        ASSERTV(U_SPEC, V_SPEC, VV == U);
                        ASSERTV(U_SPEC, V_SPEC, VV == V);
                        ASSERTV(U_SPEC, V_SPEC,  V == U);

                        gg(&mV, U_SPEC);

                        ASSERTV(U_SPEC, V_SPEC, Z, (VV == V), Z == (VV == V));
                    }

                    // 'mV' (and therefore 'V') now out of scope
                    ASSERTV(U_SPEC, V_SPEC, VV == U);
                }
            }
        }

        if (verbose) cout << "\nTesting self assignment (Aliasing)." << endl;
        {
            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC   = SPECS[ti];
                const int         curLen = strlen(SPEC);

                if (veryVerbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                ASSERTV(SPEC, oldLen <= curLen);  // non-decreasing
                oldLen = curLen;

                const Obj X = g(SPEC);                  // control
                ASSERTV(ti, curLen == X.length());  // same lengths

                Obj mY; const Obj& Y = mY; gg(&mY, SPEC);

                if (veryVerbose) { cout << "\t\t"; P(Y); }

                ASSERTV(SPEC, Y == Y);
                ASSERTV(SPEC, X == Y);

                mY = Y; // test assignment here

                ASSERTV(SPEC, Y == Y);
                ASSERTV(SPEC, X == Y);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that, when member and free 'swap' are implemented, we can
        //   exchange the values of any two objects.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //  Reserved for 'swap' testing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTIONS" << endl
                          << "==============================" << endl;

        if (verbose) cout << "Not implemented for 'bdlt::DayOfWeekSet'."
                          << endl;

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //
        // Concerns:
        //: 1 The new object's value is the same as that of the original
        //:   object (relying on the previously tested equality operators).
        //:
        //: 2 The value of the original object is left unaffected.
        //:
        //: 3 Subsequent changes in or destruction of the source object have
        //:   no effect on the copy-constructed object.
        //
        // Plan:
        //: 1 Create an object using the 'gg' function from the same spac,
        //:   verify they compare equal.  Create the 2nd of the two in dynamic
        //:   memory.
        //:
        //: 2 Copy construct a third object from the 2nd object.  Verify they
        //:   compare equal.  Verify the first two objects still compare equal.
        //:   (C-1, C-2)
        //:
        //: 3 Destroy the 2nd object and trash its memory.
        //:
        //: 4 Verify the first object and the third are still equal. (C-3)
        //
        // Testing:
        //   DayOfWeekSet(const DayOfWeekSet& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING COPY CONSTRUCTOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
            "\nCopy construct values with varied representations." << endl;
        {
            static const char *SPECS[] = {
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEFG",
            0}; // Null string required as last element.

            bslma::TestAllocator ta;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC   = SPECS[ti];
                const int         curLen = strlen(SPEC);

                if (veryVerbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }

                ASSERTV(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                // Create control object w.
                Obj mW; gg(&mW, SPEC); const Obj& W = mW;
                ASSERTV(ti, curLen == W.length()); // same lengths
                if (veryVerbose) { cout << "\t"; P(W); }

                Obj *mX = static_cast<Obj *>(ta.allocate(sizeof(Obj)));
                new (mX) Obj; const Obj& X = *mX;
                gg(mX, SPEC);

                ASSERTV(ti, X == W);

                Obj mY(X);  const Obj &Y = mY;
                ASSERTV(ti, Y == X);
                ASSERTV(ti, X == W);  ASSERTV(ti, Y == W);

                if (veryVerbose) { T_;  P_(W);  P_(X);  P(Y); }

                mX->~Obj();
                bsl::memset(mX, 0xff, sizeof(Obj));    // Trash the memory

                ASSERTV(ti, Y == W);

                ta.deallocate(mX);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //
        // Concerns:
        //: 1 Verify that a difference in return values of 'isMember(<member>)'
        //:   for any possible 'member' of a set will result in two
        //:   'DayOfWeekSet' object comparing as unequal, and that otherwise
        //:   they will compare as equal.
        //:
        //: 2 Neither 'lhs' nor 'rhs' value may be modified.
        //
        // Plan:
        //: 1 Construct a table of specs to be fed to 'ggg' to generate
        //:   different values.  The specs used shall all have sorted
        //:   characters, thus making it straightforward to verify that they
        //:   all specify different values.
        //:
        //: 2 Form 2 nested loops iterating through the table, and form
        //:   'Obj' values 'X' and 'Y' corresponding to the specs specified
        //:   by each of the 2 loops.
        //:
        //: 3 Apply 'isMember' exhaustively to 'X' and 'Y' and verify that they
        //:   yield the same values only when 'X' and 'Y' were formed from
        //:   the same spec.
        //:
        //: 4 Verify that the results of 'operator==' and 'operator!=' yield
        //:   results consistent with the results from exhaustive application
        //:   of 'isMember'.
        //:
        //: 5 Whenever calling 'operator==' or 'operator!=', passed references
        //:   to const objects, thus confirming (C-2).
        //
        // Testing:
        //   operator==(const DayOfWeekSet&, const DayOfWeekSet&);
        //   operator!=(const DayOfWeekSet&, const DayOfWeekSet&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING EQUALITY OPERATORS" << endl
                          << "==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of similar values (u, v) in S X S." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // string
            } DATA[] = {
                //line spec
                //---- ---------------------
                { L_,  ""                 },
                { L_,  "A"                },
                { L_,  "B"                },
                { L_,  "AB"               },
                { L_,  "BC"               },
                { L_,  "CD"               },
                { L_,  "ABC"              },
                { L_,  "BCD"              },
                { L_,  "CDE"              },
                { L_,  "DEF"              },
                { L_,  "ABCD"             },
                { L_,  "AEFG"             },
                { L_,  "BCDE"             },
                { L_,  "CDEF"             },
                { L_,  "DEFG"             },
                { L_,  "ABCDE"            },
                { L_,  "ABCEFG"           },
                { L_,  "ABCDEFG"          }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int i = 0; i < NUM_DATA; ++i) {
                const int curLen = strlen(DATA[i].d_spec);
                if (curLen != oldLen) {
                    if (veryVerbose) cout << "\tUsing lhs objects of length "
                                          << curLen << '.' << endl;
                    ASSERTV(i, oldLen <= curLen); // non-decreasing
                    oldLen = curLen;
                }

                Obj mX;  const Obj& X = mX;
                ASSERT(-1 == ggg(&mX, DATA[i].d_spec));

                if (veryVerbose) { P_(i); P_(DATA[i].d_spec); P(X); }

                for (int j = 0; j < NUM_DATA; ++j) {
                    Obj mY;  const Obj& Y = mY;
                    ASSERT(-1 == ggg(&mY, DATA[j].d_spec));

                    if (veryVerbose) {
                        cout << "  ";
                        P_(j);
                        P_(DATA[j].d_spec);
                        P(Y);
                    }

                    bool r = true;

                    for (int k = 0; k < NUM_VALUES; ++k) {
                        if (veryVeryVeryVerbose) (void) k;
                        if (X.isMember(VALUES[k]) != Y.isMember(VALUES[k])) {
                            r = false;
                            break;
                        }
                    }

                    ASSERT(r == (i == j));

                    ASSERTV(i, j,  r == (X == Y));
                    ASSERTV(i, j,  1 == (X == X));
                    ASSERTV(i, j,  1 == (Y == Y));
                    ASSERTV(i, j, !r == (X != Y));
                    ASSERTV(i, j,  0 == (X != X));
                    ASSERTV(i, j,  0 == (Y != Y));
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING PRINT METHOD AND 'operator<<'
        //
        // Concerns:
        //: 1 The print method formats the value of the object directly from
        //:   the underlying state information according to supplied arguments.
        //:   Ensure that the method formats properly for:
        //:   o empty and non-empty values.
        //:   o negative, 0, and positive levels.
        //:   o 0 and non-zero spaces per level.
        //: 2 Since 'operator<<' is layered on basic accessors, it is
        //:   sufficient to test only the output *format* (and to ensure that
        //:   no additional characters are written past the terminating null).
        //
        // Plan:
        //: 1 For each of an enumerated set of object, 'level', and
        //:   'spacesPerLevel' values, ordered by increasing object length, use
        //:   'ostringstream' to 'print' that object's value, using the
        //:   tabulated parameters.  Compare the contents of the 'ostrstream'
        //:   with the literal expected output format.
        //: 2 To test 'operator<<', for each of a small representative set of
        //:   object values, ordered by increasing length, use 'ostringstream'
        //:   to write that object's value.  Compare the contents of the
        //:   'ostrstream' with the literal expected output format.
        //
        // Testing:
        //   ostream& print(ostream& stream, int level, int spacesPerLevel);
        //   operator<<(ostream&, const DayOfWeekSet&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PRINT METHOD AND 'operator<<'" << endl
                          << "=====================================" << endl;

        if (verbose) cout << "\nTesting 'print' (ostream)." << endl;
#define NL "\n"
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  spec    indent +/-  spaces/Tab  format        // ADJUST
                //----  ----    ----------  ----------  ------------------
                { L_,   "",        0,         0,        "["           NL
                                                        "]"           NL  },

                { L_,   "",        0,         2,        "["           NL
                                                        "]"           NL  },

                { L_,   "",        1,         1,        " ["          NL
                                                        " ]"          NL  },

                { L_,   "",        1,         2,        "  ["         NL
                                                        "  ]"         NL  },

                { L_,   "",       -1,         2,        "["           NL
                                                        "  ]"         NL  },

                { L_,   "",       -2,         2,        "["           NL
                                                        "    ]"       NL  },

                { L_,   "",        1,        -2,        "  [ ]"           },
                { L_,   "",        2,        -2,        "    [ ]"         },
                { L_,   "",       -1,        -3,        "[ ]"             },
                { L_,   "",       -2,        -3,        "[ ]"             },

                { L_,  "A",        0,         0,        "["           NL
                                                        "SUN"         NL
                                                        "]"           NL  },

                { L_,  "A",        0,         2,        "["           NL
                                                        "  SUN"       NL
                                                        "]"           NL  },

                { L_,  "A",        1,         1,        " ["          NL
                                                        "  SUN"       NL
                                                        " ]"          NL  },

                { L_,  "A",        1,         2,        "  ["         NL
                                                        "    SUN"     NL
                                                        "  ]"         NL  },

                { L_,  "A",       -1,         2,        "["           NL
                                                        "    SUN"     NL
                                                        "  ]"         NL  },

                { L_,  "A",       -2,         2,        "["           NL
                                                        "      SUN"   NL
                                                        "    ]"       NL  },

                { L_,  "A",        1,        -2,        "  [ SUN ]"       },
                { L_,  "A",        2,        -2,        "    [ SUN ]"     },
                { L_,  "A",       -1,        -3,        "[ SUN ]"         },
                { L_,  "A",       -2,        -3,        "[ SUN ]"         },

                { L_, "BC",        0,         0,        "["           NL
                                                        "TUE THU"     NL
                                                        "]"           NL  },

                { L_, "BC",        0,         2,        "["           NL
                                                        "  TUE THU"   NL
                                                        "]"           NL  },

                { L_, "BC",        1,         1,        " ["          NL
                                                        "  TUE THU"   NL
                                                        " ]"          NL  },

                { L_, "BC",        1,         2,        "  ["         NL
                                                        "    TUE THU" NL
                                                        "  ]"         NL  },

                { L_, "BC",       -1,         2,        "["           NL
                                                        "    TUE THU" NL
                                                        "  ]"         NL  },

                { L_, "BC",       -2,         2,        "["             NL
                                                        "      TUE THU" NL
                                                        "    ]"         NL  },

                { L_, "BC",        1,        -2,        "  [ TUE THU ]"   },
                { L_, "BC",        2,        -2,        "    [ TUE THU ]" },
                { L_, "BC",       -1,        -3,        "[ TUE THU ]"     },
                { L_, "BC",       -2,        -3,        "[ TUE THU ]"     },

                { L_,   "ABCDE",   1,         3,        "   ["        NL
                                                        "      SUN"
                                                             " MON"
                                                             " TUE"
                                                             " THU"
                                                             " FRI"   NL
                                                        "   ]"        NL  },
            };
#undef NL

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         IND    = DATA[ti].d_indent;
                const int         SPL    = DATA[ti].d_spaces;
                const char *const FMT    = DATA[ti].d_fmt_p;
                const int         curLen = strlen(SPEC);

                Obj mX;  const Obj& X = gg(&mX, SPEC);
                ASSERTV(LINE, curLen == X.length());  // same lengths

                if (curLen != oldLen) {
                    if (veryVerbose) cout << "\ton objects of length "
                                          << curLen << ':' << endl;
                    ASSERTV(LINE, oldLen < curLen);  // strictly increasing
                    oldLen = curLen;
                }

                if (veryVerbose) { cout << "\t\tSpec = \"" << SPEC << "\", ";
                                                          P_(IND); P(SPL); }
                if (veryVerbose) cout << "EXPECTED FORMAT:" << endl
                                      << FMT                << endl;

                // Override the default allocator since we will use it when we
                // read the string stream.

                bslma::TestAllocator tmpDfltAlloc;
                bslma::DefaultAllocatorGuard allocatorGuard(&tmpDfltAlloc);

                bsl::ostringstream os;
                bsl::ostream& osB = X.print(os, IND, SPL);

                ASSERT(&osB == &os);

                const bsl::string& str = os.str();
                if (veryVerbose) cout << "ACTUAL FORMAT:"   << endl
                                      << str                << endl;

                ASSERTV(LINE, FMT, str, FMT == str);
            }
        }

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                // Note that the output is counterintuitive, because the chars
                // 'A-G' in 'spec' are not in the order of the days of the
                // week.

                //line  spec            output format
                //----  --------------  ---------------------------------

                { L_,   "",             "[ ]"                           },
                { L_,   "A",            "[ SUN ]"                       },
                { L_,   "F",            "[ SAT ]"                       },
                { L_,   "BC",           "[ TUE THU ]"                   },
                { L_,   "AF",           "[ SUN SAT ]"                   },
                { L_,   "ABCDE",        "[ SUN MON TUE THU FRI ]"       },
                { L_,   "EDCBA",        "[ SUN MON TUE THU FRI ]"       },
                { L_,   "ABCDEF",       "[ SUN MON TUE THU FRI SAT ]"   }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const char *const FMT    = DATA[ti].d_fmt_p;
                const int         curLen = strlen(SPEC);

                Obj mX;  const Obj& X = gg(&mX, SPEC);
                ASSERTV(ti, curLen == X.length());  // same lengths

                if (curLen != oldLen) {
                    if (veryVerbose) cout << "\ton objects of length "
                                          << curLen << ':' << endl;
                    ASSERTV(LINE, oldLen < curLen);  // strictly increasing
                    oldLen = curLen;
                }

                if (veryVerbose) cout << "\t\tSpec = \"" << SPEC << "\"\n"
                                      << "EXPECTED FORMAT:" << endl
                                      << FMT                << endl;

                // Override the default allocator since we will use it when we
                // read the string stream.

                bslma::TestAllocator tmpDfltAlloc;
                bslma::DefaultAllocatorGuard allocatorGuard(&tmpDfltAlloc);

                bsl::ostringstream os;  os << X;
                const bsl::string& str = os.str();

                if (veryVerbose) cout << "ACTUAL FORMAT:" << endl
                                      << str              << endl;

                ASSERTV(LINE, FMT, str, FMT == str);
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   This will test the basic accessors and also the
        //   DayOfWeekSet_Iter class.
        //
        // Concerns:
        //: 1 The iterator copy constructor, initializing constructor, and
        //:   assignment operators set the internal state of the iterator
        //:   correctly.
        //:
        //: 2 The set accessors return expected values.
        //:
        //: 3 The increment and decrement operators on the iterators work as
        //:   expected.
        //
        // Plan:
        //: 1 Test iterator constructors and 'operator='.
        //:   For each line in a set of test data, initialize a 'data'
        //:   variable containing the the elements from the line.  Create the
        //:   following 3 iterators:
        //:   o X (default ctor)
        //:   o Y (initial ctor)
        //:   o Z (copy of Y using copy ctor)
        //:
        //: 2 Loop through the sorted array of elements, checking the value
        //:   of '*Y' and '*Z' at each iteration.  At the mid-point, assign
        //:   'Y' to 'X'.
        //:
        //: 3 Loop through the sorted array of elements starting at the
        //:   mid-point, checking the value of '*X' at each iteration.
        //:
        //: 4 Test set accessors.  For each line in a set of test data,
        //:   initialize a set using the 'gg' generator function.  Check that
        //:   the length is as expected and that each element is a member of
        //:   the set.  Test the forward and reverse iterators by creating a
        //:   sorted array of elements and checking that the iterators contain
        //:   the same elements as the sorted array, and in the same order.
        //
        // Testing:
        //   iterator begin() const;
        //   iterator end() const;
        //   bool isEmpty() const;
        //   bool isMember(Month::Month value) const;
        //   int length() const;
        //   reverse_iterator rbegin() const;
        //   reverse_iterator rend() const;
        //
        //   DayOfWeekSet_Iter();
        //   DayOfWeekSet_Iter(int data, int index);
        //   DayOfWeekSet_Iter(const DayOfWeekSet_Iter& original);
        //   ~DayOfWeekSet_Iter();
        //   DayOfWeekSet_Iter& operator=(const DayOfWeekSet_Iter&);
        //   DayOfWeekSet_Iter& operator++();
        //   DayOfWeekSet_Iter& operator--();
        //   const DayOfWeek::Day& operator*() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING BASIC ACCESSORS" << endl
                          << "=======================" << endl;

        const int SZ = 10;
        static const struct {
            int         d_lineNum;          // source line number
            const char *d_spec_p;           // specification string
            int         d_length;           // expected length
            Element     d_elements[SZ];     // expected element values
        } DATA[] = {
            //line  spec            length  elements
            //----  --------------  ------  ------------------------
            { L_,   "",             0,      { VA }                          },
            { L_,   "A",            1,      { VA }                          },
            { L_,   "B",            1,      { VB }                          },
            { L_,   "AA",           1,      { VA }                          },
            { L_,   "AB",           2,      { VA, VB }                      },
            { L_,   "BC",           2,      { VB, VC }                      },
            { L_,   "AABB",         2,      { VA, VB }                      },
            { L_,   "ABAB",         2,      { VA, VB }                      },
            { L_,   "ABBA",         2,      { VA, VB }                      },
            { L_,   "BAAB",         2,      { VA, VB }                      },
            { L_,   "BABA",         2,      { VA, VB }                      },
            { L_,   "BBAA",         2,      { VA, VB }                      },
            { L_,   "ABABAB",       2,      { VA, VB }                      },
            { L_,   "ABC",          3,      { VA, VB, VC }                  },

            { L_,   "AABBCC",       3,      { VA, VB, VC }                  },
            { L_,   "AABCBC",       3,      { VA, VB, VC }                  },
            { L_,   "AABCCB",       3,      { VA, VB, VC }                  },
            { L_,   "AACBBC",       3,      { VA, VB, VC }                  },
            { L_,   "AACBCB",       3,      { VA, VB, VC }                  },
            { L_,   "AACCBB",       3,      { VA, VB, VC }                  },

            { L_,   "ABABCC",       3,      { VA, VB, VC }                  },
            { L_,   "ABACBC",       3,      { VA, VB, VC }                  },
            { L_,   "ABACCB",       3,      { VA, VB, VC }                  },
            { L_,   "ACABBC",       3,      { VA, VB, VC }                  },
            { L_,   "ACABCB",       3,      { VA, VB, VC }                  },
            { L_,   "ACACBB",       3,      { VA, VB, VC }                  },

            { L_,   "ABBACC",       3,      { VA, VB, VC }                  },
            { L_,   "ABCABC",       3,      { VA, VB, VC }                  },
            { L_,   "ABCACB",       3,      { VA, VB, VC }                  },
            { L_,   "ACBABC",       3,      { VA, VB, VC }                  },
            { L_,   "ACBACB",       3,      { VA, VB, VC }                  },
            { L_,   "ACCABB",       3,      { VA, VB, VC }                  },

            { L_,   "ABBCAC",       3,      { VA, VB, VC }                  },
            { L_,   "ABCBAC",       3,      { VA, VB, VC }                  },
            { L_,   "ABCCAB",       3,      { VA, VB, VC }                  },
            { L_,   "ACBBAC",       3,      { VA, VB, VC }                  },
            { L_,   "ACBCAB",       3,      { VA, VB, VC }                  },
            { L_,   "ACCBAB",       3,      { VA, VB, VC }                  },

            { L_,   "ABBCCA",       3,      { VA, VB, VC }                  },
            { L_,   "ABCBCA",       3,      { VA, VB, VC }                  },
            { L_,   "ABCCBA",       3,      { VA, VB, VC }                  },
            { L_,   "ACBBCA",       3,      { VA, VB, VC }                  },
            { L_,   "ACBCBA",       3,      { VA, VB, VC }                  },
            { L_,   "ACCBBA",       3,      { VA, VB, VC }                  },

            { L_,   "BAABCC",       3,      { VA, VB, VC }                  },
            { L_,   "BAACBC",       3,      { VA, VB, VC }                  },
            { L_,   "BAACCB",       3,      { VA, VB, VC }                  },
            { L_,   "CAABBC",       3,      { VA, VB, VC }                  },
            { L_,   "CAABCB",       3,      { VA, VB, VC }                  },
            { L_,   "CAACBB",       3,      { VA, VB, VC }                  },

            { L_,   "BABACC",       3,      { VA, VB, VC }                  },
            { L_,   "BACABC",       3,      { VA, VB, VC }                  },
            { L_,   "BACACB",       3,      { VA, VB, VC }                  },
            { L_,   "CABABC",       3,      { VA, VB, VC }                  },
            { L_,   "CABACB",       3,      { VA, VB, VC }                  },
            { L_,   "CACABB",       3,      { VA, VB, VC }                  },

            { L_,   "BABCAC",       3,      { VA, VB, VC }                  },
            { L_,   "BACBAC",       3,      { VA, VB, VC }                  },
            { L_,   "BACCAB",       3,      { VA, VB, VC }                  },
            { L_,   "CABBAC",       3,      { VA, VB, VC }                  },
            { L_,   "CABCAB",       3,      { VA, VB, VC }                  },
            { L_,   "CACBAB",       3,      { VA, VB, VC }                  },

            { L_,   "BABCCA",       3,      { VA, VB, VC }                  },
            { L_,   "BACBCA",       3,      { VA, VB, VC }                  },
            { L_,   "BACCBA",       3,      { VA, VB, VC }                  },
            { L_,   "CABBCA",       3,      { VA, VB, VC }                  },
            { L_,   "CABCBA",       3,      { VA, VB, VC }                  },
            { L_,   "CACBBA",       3,      { VA, VB, VC }                  },

            { L_,   "BBAACC",       3,      { VA, VB, VC }                  },
            { L_,   "BCAABC",       3,      { VA, VB, VC }                  },
            { L_,   "BCAACB",       3,      { VA, VB, VC }                  },
            { L_,   "CBAABC",       3,      { VA, VB, VC }                  },
            { L_,   "CBAACB",       3,      { VA, VB, VC }                  },
            { L_,   "CCAABB",       3,      { VA, VB, VC }                  },

            { L_,   "BBACAC",       3,      { VA, VB, VC }                  },
            { L_,   "BCABAC",       3,      { VA, VB, VC }                  },
            { L_,   "BCACAB",       3,      { VA, VB, VC }                  },
            { L_,   "CBABAC",       3,      { VA, VB, VC }                  },
            { L_,   "CBACAB",       3,      { VA, VB, VC }                  },
            { L_,   "CCABAB",       3,      { VA, VB, VC }                  },

            { L_,   "BBACCA",       3,      { VA, VB, VC }                  },
            { L_,   "BCABCA",       3,      { VA, VB, VC }                  },
            { L_,   "BCACBA",       3,      { VA, VB, VC }                  },
            { L_,   "CBABCA",       3,      { VA, VB, VC }                  },
            { L_,   "CBACBA",       3,      { VA, VB, VC }                  },
            { L_,   "CCABBA",       3,      { VA, VB, VC }                  },

            { L_,   "BBCAAC",       3,      { VA, VB, VC }                  },
            { L_,   "BCBAAC",       3,      { VA, VB, VC }                  },
            { L_,   "BCCAAB",       3,      { VA, VB, VC }                  },
            { L_,   "CBBAAC",       3,      { VA, VB, VC }                  },
            { L_,   "CBCAAB",       3,      { VA, VB, VC }                  },
            { L_,   "CCBAAB",       3,      { VA, VB, VC }                  },

            { L_,   "BBCACA",       3,      { VA, VB, VC }                  },
            { L_,   "BCBACA",       3,      { VA, VB, VC }                  },
            { L_,   "BCCABA",       3,      { VA, VB, VC }                  },
            { L_,   "CBBACA",       3,      { VA, VB, VC }                  },
            { L_,   "CBCABA",       3,      { VA, VB, VC }                  },
            { L_,   "CCBABA",       3,      { VA, VB, VC }                  },

            { L_,   "BBCCAA",       3,      { VA, VB, VC }                  },
            { L_,   "BCBCAA",       3,      { VA, VB, VC }                  },
            { L_,   "BCCBAA",       3,      { VA, VB, VC }                  },
            { L_,   "CBBCAA",       3,      { VA, VB, VC }                  },
            { L_,   "CBCBAA",       3,      { VA, VB, VC }                  },
            { L_,   "CCBBAA",       3,      { VA, VB, VC }                  },

            { L_,   "ABCABCABC",    3,      { VA, VB, VC }                  },
            { L_,   "BCA",          3,      { VB, VC, VA }                  },
            { L_,   "CAB",          3,      { VC, VA, VB }                  },
            { L_,   "CDAB",         4,      { VC, VD, VA, VB }              },
            { L_,   "DABC",         4,      { VD, VA, VB, VC }              },
            { L_,   "ABCDE",        5,      { VA, VB, VC, VD, VE }          },
            { L_,   "EDCBA",        5,      { VA, VB, VC, VD, VE }          },
            { L_,   "ABCDEAB",      5,      { VA, VB, VC, VD, VE }          },
            { L_,   "BACDEABC",     5,      { VA, VB, VC, VD, VE }          },
            { L_,   "CBADEABCD",    5,      { VA, VB, VC, VD, VE }          },

            { L_,   "ABFCDE",       6,      { VA, VB, VC, VD, VE, VF }      },
            { L_,   "EDCFBA",       6,      { VA, VB, VC, VD, VE, VF }      },
            { L_,   "ABCDFEAB",     6,      { VA, VB, VC, VD, VE, VF }      },
            { L_,   "BACDEFABC",    6,      { VA, VB, VC, VD, VE, VF }      },
            { L_,   "CBADEAFBCD",   6,      { VA, VB, VC, VD, VE, VF }      },

            { L_,   "AGBFCDE",      7,      { VA, VB, VC, VD, VE, VF, VG }  },
            { L_,   "EDGCFBA",      7,      { VA, VB, VC, VD, VE, VF, VG }  },
            { L_,   "ABCDFEGAB",    7,      { VA, VB, VC, VD, VE, VF, VG }  },
            { L_,   "BACDEFGABC",   7,      { VA, VB, VC, VD, VE, VF, VG }  },
            { L_,   "CBAGDEAFBCD",  7,      { VA, VB, VC, VD, VE, VF, VG }  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        int ti;

        if (verbose) cout << "\nTesting iterator constructors and 'operator='."
                          << endl;
        for (ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE         = DATA[ti].d_lineNum;
            const int LENGTH       = DATA[ti].d_length;
            const Element *const e = DATA[ti].d_elements;
            const int MID_POINT    = LENGTH / 2;

            int data = 0;
            int i;

            if (veryVerbose) {
                cout << "\tSet = [ ";
            }

            for (i = 0; i < LENGTH; ++i) {
                const Element element = e[i];

                data |= BIT_MASK[element];

                if (veryVerbose) {
                    cout << element << " ";
                }
            }

            if (veryVerbose) {
                cout << "]" << endl;
            }

            Iterator        mX;
            const Iterator& X = mX;
            Iterator        mY(data, 1);
            const Iterator& Y = mY;
            Iterator        mZ(Y);
            const Iterator& Z = mZ;

            Element       sortedElements[SZ];
            int           length = sortElementsAndRemoveDuplicates(
                                                                sortedElements,
                                                                SZ,
                                                                e,
                                                                LENGTH);
            ASSERTV(LINE, length, LENGTH == length);

            for (i = 0; i < LENGTH; ++i, ++mY, ++mZ) {
                const Element element =  sortedElements[i];
                const Element eY      = *Y;
                const Element eZ      = *Z;

                if (veryVerbose) DOT;

                if (i == MID_POINT) {
                    mX = mY;
                }

                ASSERTV(LINE, i, eY, element == eY);
                ASSERTV(LINE, i, eZ, element == eZ);
            }
            if (veryVerbose) CR;

            for (i = MID_POINT; i < LENGTH; ++i, ++mX) {
                const Element element =  sortedElements[i];
                const Element eX      = *X;

                if (veryVerbose) DOT;

                ASSERTV(LINE, i, eX, element == eX);
            }
            if (veryVerbose) CR;
        }

        if (verbose) cout << "\nTesting 'isEmpty', 'isMember', 'length', "
                          << "'begin', 'end', 'rbegin', 'rend', 'operator++',"
                          << " 'operator--', and 'operator*'."
                          << endl;
        for (ti = 0; ti < NUM_DATA ; ++ti) {
            const int LINE         = DATA[ti].d_lineNum;
            const char *const SPEC = DATA[ti].d_spec_p;
            const int LENGTH       = DATA[ti].d_length;
            const Element *const e = DATA[ti].d_elements;
            const int curLen = LENGTH;

            Obj mX;
            const Obj& X = gg(&mX, SPEC);

            ASSERTV(ti, (0 == curLen) == X.isEmpty());
            ASSERTV(ti, curLen == X.length());

            if (curLen != oldLen) {
                if (veryVerbose) cout << "\ton objects of length "
                                  << curLen << ':' << endl;
                ASSERTV(LINE, oldLen <= curLen);  // non-decreasing
                oldLen = curLen;
            }

            if (veryVerbose) cout << "\t\tSpec = \"" << SPEC << '"' << endl;
            if (veryVeryVerbose) { cout << "\t\t\t"; P(X); }

            ASSERTV(LINE, (0 == LENGTH) == X.isEmpty());
            ASSERTV(LINE, LENGTH == X.length());

            int i;
            for (i = 0; i < NUM_VALUES; ++i) {
                if (veryVeryVerbose) DOT;
                bool ev = false;
                for (int j = 0; j < LENGTH; ++j) {
                    if (veryVeryVeryVerbose) DOT;
                    if (VALUES[i] == e[j]) {
                        ev = true;
                        break;
                    }
                }
                if (veryVeryVeryVerbose) CR;
                ASSERTV(LINE, i, ev == X.isMember(VALUES[i]));
            }
            if (veryVeryVerbose) CR;

            if (veryVerbose) cout << "\t\tTesting forward iterator." << endl;
            {
                Element       sortedElements[SZ];
                Obj::iterator testIterator;
                int           length = sortElementsAndRemoveDuplicates(
                                                                sortedElements,
                                                                SZ,
                                                                e,
                                                                LENGTH);

                ASSERTV(LINE, length, LENGTH == length);

                for (i = 0, testIterator = X.begin();
                     i < LENGTH;
                     ++i, ++testIterator) {
                    ASSERTV(LINE, X.end() != testIterator);

                    if (veryVeryVerbose) {
                        cout << "\t\t\t";
                        P_(sortedElements[i]) P(*testIterator)
                    }

                    ASSERTV(LINE, sortedElements[i],   *testIterator,
                                       sortedElements[i] == *testIterator);
                }

                ASSERTV(LINE, X.end() == testIterator);

                for (--i; i >= 0; --i) {
                    --testIterator;

                    if (veryVeryVerbose) {
                        cout << "\t\t\t";
                        P_(sortedElements[i]) P(*testIterator)
                    }

                    ASSERTV(LINE, sortedElements[i],   *testIterator,
                                       sortedElements[i] == *testIterator);
                }

                ASSERTV(LINE, X.begin() == testIterator);
            }

            if (veryVerbose) cout << "\t\tTesting reverse iterator." << endl;
            {
                Element               sortedElements[SZ];
                Obj::reverse_iterator testIterator;
                int                   length = sortElementsAndRemoveDuplicates(
                                                                sortedElements,
                                                                SZ,
                                                                e,
                                                                LENGTH);

                ASSERTV(LINE, length, LENGTH == length);

                for (i = LENGTH - 1, testIterator = X.rbegin();
                     i >= 0;
                     --i, ++testIterator) {
                    ASSERTV(LINE, X.rend() != testIterator);

                    if (veryVeryVerbose) {
                        cout << "\t\t\t";
                        P_(sortedElements[i]) P(*testIterator)
                    }

                    ASSERTV(LINE, sortedElements[i],   *testIterator,
                                       sortedElements[i] == *testIterator);
                }

                ASSERTV(LINE, X.rend() == testIterator);

                for (++i; i < LENGTH; ++i) {
                    --testIterator;

                    if (veryVeryVerbose) {
                        cout << "\t\t\t";
                        P_(sortedElements[i]) P(*testIterator)
                    }

                    ASSERTV(LINE, sortedElements[i],   *testIterator,
                                       sortedElements[i] == *testIterator);
                }

                ASSERTV(LINE, X.rbegin() == testIterator);
            }
        }

        if (verbose) cout << "\nEnd of Basic Accessors Test." << endl;
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE GENERATOR FUNCTION 'gg'
        //
        // Concerns:
        //: 1 That 'gg' produces expected values for given valid spec strings.
        //:
        //: 2 That 'ggg' properly reports the index in the spec string of
        //:   the first char of invalid input, and -1 if the input is valid.
        //: 3 Objects created with 'g' are equivalent to objects created by
        //:   'gg' for the same spec.
        //: 4 The object created by 'g' is a new object and not a copy of a
        //:   preexisting object.
        //
        // Plan:
        //: 1 For each of an enumerated sequence of 'spec' values, ordered by
        //:   increasing 'spec' length, use the primitive generator function
        //:   'gg' to set the state of a newly created object.  Verify that
        //:   'gg' returns a valid reference to the modified argument object
        //:   and, using basic accessors, that the value of the object is as
        //:   expected.  Repeat the test for a longer 'spec' generated by
        //:   prepending a string ending in a '~' character (denoting
        //:   'removeAll').  Note that we are testing the parser only; the
        //:   primary manipulators are already assumed to work. (C-1)
        //:
        //: 2 Iterate through a similar loop calling 'ggg' instead of 'gg' and,
        //:   for each length of SPEC, first try a valid spec and verify that
        //:   -1 is returned, then try invalid specs and verify that in all
        //:   cases the index of the first invalid char of the spec is
        //:   returned.
        //:
        //: 3 For a variety of specs, populate an object using 'gg', then
        //:   confirm it is equivalent to an object returned by 'g' for the
        //:   same spec. (C-1)
        //:
        //: 4 Create an object and populate it a couple of times with 'gg',
        //:   verify that 'gg' returns the address of that object.  Create
        //:   a couple of objects with 'g' and verify their addresses do not
        //:   match, and do not match that of the object populated with 'gg.
        //:   (C-2)
        //
        // Testing:
        //   DayOfWeekSet& gg(DayOfWeekSet* obj, const char *spec);
        //   int ggg(DayOfWeekSet *object, const char *spec, int vF = 1);
        //   DayOfWeekSet g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING PRIMITIVE GENERATOR FUNCTION 'gg'" << endl
                 << "=========================================" << endl;

        if (verbose) cout << "\nTesting generator on valid specs." << endl;
        {
            const int SZ = 10;
            static const struct {
                int         d_lineNum;          // source line number
                const char *d_spec_p;           // specification string
                int         d_length;           // expected length
                Element     d_elements[SZ];     // expected element values
            } DATA[] = {
                //line  spec            length  elements
                //----  --------------  ------  ------------------------
                { L_,   "A",            1,      { VA }                  },
                { L_,   "B",            1,      { VB }                  },
                { L_,   "F",            1,      { VF }                  },

                { L_,   "CD",           2,      { VC, VD }              },
                { L_,   "AF",           2,      { VA, VF }              },
                { L_,   "~E",           1,      { VE }                  },

                { L_,   "ABC",          3,      { VA, VB, VC }          },
                { L_,   "~BC",          2,      { VB, VC }              },
                { L_,   "A~C",          1,      { VC }                  },
                { L_,   "A~F",          1,      { VF }                  },
                { L_,   "~~C",          1,      { VC }                  },
                { L_,   "~~F",          1,      { VF }                  },

                { L_,   "ABCD",         4,      { VA, VB, VC, VD }      },
                { L_,   "~BCD",         3,      { VB, VC, VD }          },
                { L_,   "A~CD",         2,      { VC, VD }              },
                { L_,   "AB~D",         1,      { VD }                  },

                { L_,   "ABCDE",        5,      { VA, VB, VC, VD, VE }  },
                { L_,   "~BCDE",        4,      { VB, VC, VD, VE }      },
                { L_,   "AB~DE",        2,      { VD, VE }              },
                { L_,   "A~C~E",        1,      { VE }                  },

                { L_,   "~CBA~~ABCDE",  5,      { VA, VB, VC, VD, VE }  },

                { L_,   "ABCDE~CDEC~E", 1,      { VE }                  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int            LINE   = DATA[ti].d_lineNum;
                const char *const    SPEC   = DATA[ti].d_spec_p;
                const int            LENGTH = DATA[ti].d_length;
                const Element *const ELEMS  = DATA[ti].d_elements;
                const int            curLen = strlen(SPEC);

                Obj mX;
                Obj& mmX = gg(&mX, SPEC);   // original spec
                const Obj& X = mmX;

                ASSERT(&mX == &X);

                static const char *const MORE_SPEC = "~ABCDEABCDEABCDEABCDE~";
                char buf[100]; strcpy(buf, MORE_SPEC); strcat(buf, SPEC);

                Obj mY;
                const Obj& Y = gg(&mY, buf);    // extended spec

                if (curLen != oldLen) {
                    if (veryVeryVerbose) cout << "\tof length "
                                              << curLen << ':' << endl;
                    ASSERTV(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) {
                    cout << "\t\t   Spec = \"" << SPEC << '"' << endl;
                    cout << "\t\tBigSpec = \"" << buf << '"' << endl;
                    cout << "\t\t\t"; P(X);
                    cout << "\t\t\t"; P(Y);
                }

                ASSERTV(LINE, (0 == LENGTH) == X.isEmpty());
                ASSERTV(LINE, (0 == LENGTH) == Y.isEmpty());
                ASSERTV(LINE, LENGTH == X.length());
                ASSERTV(LINE, LENGTH == Y.length());
                for (int i = 0; i < LENGTH; ++i) {
                    if (veryVeryVerbose) DOT;
                    ASSERTV(LINE, i, X.isMember(ELEMS[i]));
                    ASSERTV(LINE, i, Y.isMember(ELEMS[i]));
                }
                if (veryVeryVerbose) CR;
            }
        }

        if (verbose) cout << "\nTesting generator on invalid specs." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_index;    // offending character index
            } DATA[] = {
                //line  spec            index
                //----  -------------   -----
                { L_,   "",             -1,     }, // control

                { L_,   "~",            -1,     }, // control
                { L_,   " ",             0,     },
                { L_,   ".",             0,     },
                { L_,   "J",             0,     },

                { L_,   "AE",           -1,     }, // control
                { L_,   "aE",            0,     },
                { L_,   "Ae",            1,     },
                { L_,   ".~",            0,     },
                { L_,   "~!",            1,     },
                { L_,   "  ",            0,     },

                { L_,   "ABC",          -1,     }, // control
                { L_,   " BC",           0,     },
                { L_,   "A C",           1,     },
                { L_,   "AB ",           2,     },
                { L_,   "?#:",           0,     },
                { L_,   "   ",           0,     },

                { L_,   "ABCDE",        -1,     }, // control
                { L_,   "aBCDE",         0,     },
                { L_,   "ABcDE",         2,     },
                { L_,   "ABCDe",         4,     },
                { L_,   "AbCdE",         1,     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         INDEX  = DATA[ti].d_index;
                const int         curLen = strlen(SPEC);

                Obj mX;

                if (curLen != oldLen) {
                    if (veryVerbose) cout << "\tof length "
                                      << curLen << ':' << endl;
                    ASSERTV(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) cout <<
                    "\t\tSpec = \"" << SPEC << '"' << endl;

                int result = ggg(&mX, SPEC, veryVerbose);

                ASSERTV(LINE, INDEX == result);
            }
        }

        if (verbose) cout <<
            "\nCompare values produced by 'g' and 'gg' for various inputs."
                                                                       << endl;
        {
            static const char *SPECS[] = {
                    "", "~", "A", "B", "AB", "DEF", "EFG", "~D~E", "ABCDE",
                     "ABC~DE", 0 }; // Null string required as last element.

            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *spec = SPECS[ti];
                if (veryVerbose) { P_(ti);  P(spec); }
                Obj mX;  gg(&mX, spec);  const Obj& X = mX;
                if (veryVerbose) {
                    cout << "\t g = " << g(spec) << endl;
                    cout << "\tgg = " << X       << endl;
                }

                ASSERTV(ti, X == g(spec));
            }
        }

        if (verbose) cout << "\nConfirm return-by-value." << endl;
        {
            const char *spec = "ABCDE";

            ASSERT(sizeof(Obj) == sizeof g(spec));      // compile-time fact

            Obj x;                      // runtime tests
            Obj& r1 = gg(&x, spec);
            Obj& r2 = gg(&x, spec);
            const Obj& r3 = g(spec);
            const Obj& r4 = g(spec);
            ASSERT(&r2 == &r1);
            ASSERT(&x  == &r1);
            ASSERT(&r4 != &r3);
            ASSERT(&x  != &r3);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS
        //
        // Concerns:
        //: 1 The default constructor creates the correct initial value.
        //:
        //: 2 The destructor doesn't segfault, abort, or exit
        //:
        //: 3 'add'
        //:    A Produces the expected value.
        //:    B Maintains valid internal state.
        //:
        //: 4 'removeAll' produces the expected value (empty).
        //
        // Plan:
        //:  1 Default construct a value and confirm its value with the
        //:    'length' and 'isMember' accessors, then destroy it. (C-1, C-2)
        //:
        //:  2 Repetitively default construct objects and then apply different
        //:    sequences of additions to it, then confirm its state and confirm
        //:    its validity by destroying it. (C-3 A,B)
        //:
        //:  3 Incrementally build an instance using 'add' (and sometimes
        //:    redundant 'add's), then empty it using 'removeAll', then rebuild
        //:    it and destroy it. (C-3, C-4)
        //
        // Testing:
        //   DayOfWeekSet();
        //   ~DayOfWeekSet();
        //   void add(DayOfWeek value);
        //   void removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PRIMARY MANIPULATORS" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting default ctor." << endl;
        {
            const Obj X;
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.length());
            ASSERT(!X.isMember(V0));
            ASSERT(!X.isMember(V1));
            ASSERT(!X.isMember(V2));
            ASSERT(!X.isMember(V3));
            ASSERT(!X.isMember(V4));
            ASSERT(!X.isMember(V5));
            ASSERT(!X.isMember(V6));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'add' (bootstrap)." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            Obj mX;  const Obj& X = mX;

            mX.add(V0);
            ASSERT(1 == X.length());
            ASSERT(X.isMember(V0));
            ASSERT(!X.isMember(V1));
            ASSERT(!X.isMember(V2));
            ASSERT(!X.isMember(V3));
            ASSERT(!X.isMember(V4));
            ASSERT(!X.isMember(V5));
            ASSERT(!X.isMember(V6));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX;  const Obj& X = mX;
            mX.add(V0);

            mX.add(V1);
            ASSERT(2 == X.length());
            ASSERT(X.isMember(V0));
            ASSERT(X.isMember(V1));
            ASSERT(!X.isMember(V2));
            ASSERT(!X.isMember(V3));
            ASSERT(!X.isMember(V4));
            ASSERT(!X.isMember(V5));
            ASSERT(!X.isMember(V6));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            Obj mX;  const Obj& X = mX;
            mX.add(V0); mX.add(V1);

            mX.add(V2);
            ASSERT(3 == X.length());
            ASSERT(X.isMember(V0));
            ASSERT(X.isMember(V1));
            ASSERT(X.isMember(V2));
            ASSERT(!X.isMember(V3));
            ASSERT(!X.isMember(V4));
            ASSERT(!X.isMember(V5));
            ASSERT(!X.isMember(V6));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            Obj mX;  const Obj& X = mX;
            mX.add(V0); mX.add(V1); mX.add(V2);

            mX.add(V3);
            ASSERT(4 == X.length());
            ASSERT(X.isMember(V0));
            ASSERT(X.isMember(V1));
            ASSERT(X.isMember(V2));
            ASSERT(X.isMember(V3));
            ASSERT(!X.isMember(V4));
            ASSERT(!X.isMember(V5));
            ASSERT(!X.isMember(V6));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 4." << endl;
            Obj mX;  const Obj& X = mX;
            mX.add(V0); mX.add(V1); mX.add(V2); mX.add(V3);

            mX.add(V4);
            ASSERT(5 == X.length());
            ASSERT(X.isMember(V0));
            ASSERT(X.isMember(V1));
            ASSERT(X.isMember(V2));
            ASSERT(X.isMember(V3));
            ASSERT(X.isMember(V4));
            ASSERT(!X.isMember(V5));
            ASSERT(!X.isMember(V6));
        }

        {
            if (verbose) cout << "\tOn an object of initial length 5." << endl;
            Obj mX;  const Obj& X = mX;
            mX.add(V0); mX.add(V1); mX.add(V2); mX.add(V3); mX.add(V4);

            mX.add(V5);
            ASSERTV(X.length(), 6 == X.length());

            ASSERT(X.isMember(V0));
            ASSERT(X.isMember(V1));
            ASSERT(X.isMember(V2));
            ASSERT(X.isMember(V3));
            ASSERT(X.isMember(V4));
            ASSERT(X.isMember(V5));
            ASSERT(!X.isMember(V6));
        }

        {
            if (verbose) cout << "\tOn an object of initial length 6." << endl;
            Obj mX;  const Obj& X = mX;
            mX.add(V0); mX.add(V1); mX.add(V2);
            mX.add(V3); mX.add(V4); mX.add(V5);

            mX.add(V6);
            ASSERT(7 == X.length());

            ASSERT(X.isMember(V0));
            ASSERT(X.isMember(V1));
            ASSERT(X.isMember(V2));
            ASSERT(X.isMember(V3));
            ASSERT(X.isMember(V4));
            ASSERT(X.isMember(V5));
            ASSERT(X.isMember(V6));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'removeAll'." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            Obj mX;  const Obj& X = mX;
            ASSERT(0 == X.length());

            mX.removeAll();
            ASSERT(0 == X.length());

            ASSERT(!X.isMember(V0));
            ASSERT(!X.isMember(V1));
            ASSERT(!X.isMember(V2));
            ASSERT(!X.isMember(V3));
            ASSERT(!X.isMember(V4));
            ASSERT(!X.isMember(V5));
            ASSERT(!X.isMember(V6));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX;  const Obj& X = mX;
            mX.add(V0);
            ASSERT(1 == X.length());

            mX.removeAll();
            ASSERT(0 == X.length());

            ASSERT(!X.isMember(V0));
            ASSERT(!X.isMember(V1));
            ASSERT(!X.isMember(V2));
            ASSERT(!X.isMember(V3));
            ASSERT(!X.isMember(V4));
            ASSERT(!X.isMember(V5));
            ASSERT(!X.isMember(V6));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            Obj mX;  const Obj& X = mX;
            mX.add(V0); mX.add(V1);
            ASSERT(2 == X.length());

            mX.removeAll();
            ASSERT(0 == X.length());

            ASSERT(!X.isMember(V0));
            ASSERT(!X.isMember(V1));
            ASSERT(!X.isMember(V2));
            ASSERT(!X.isMember(V3));
            ASSERT(!X.isMember(V4));
            ASSERT(!X.isMember(V5));
            ASSERT(!X.isMember(V6));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            Obj mX;  const Obj& X = mX;
            mX.add(V0); mX.add(V1); mX.add(V2);
            ASSERT(3 == X.length());

            mX.removeAll();
            ASSERT(0 == X.length());

            ASSERT(!X.isMember(V0));
            ASSERT(!X.isMember(V1));
            ASSERT(!X.isMember(V2));
            ASSERT(!X.isMember(V3));
            ASSERT(!X.isMember(V4));
            ASSERT(!X.isMember(V5));
            ASSERT(!X.isMember(V6));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting the destructor." << endl;

        if (verbose) cout << "\tWith 'add' only" << endl;
        {
            // For each lengths i up to some modest limit:
            //    1) create an instance
            //    2) add { V0, V1, V2, V3, V4, V5, ... }  up to length i
            //    3) verify initial length and contents
            //    4) allow the instance to leave scope

            const int NUM_TRIALS = 15;
            for (int i = 0; i < NUM_TRIALS; ++i) { // i is the length
                if (veryVerbose) cout <<
                             "\t\tOn an object of length " << i << '.' << endl;

                {
                    int k; // loop index

                    Obj mX;  const Obj& X = mX;                         // 1.
                    for (k = 0; k < i; ++k) {                           // 2.
                        if (veryVeryVerbose) DOT;
                        mX.add(VALUES[k % NUM_VALUES]);
                    }
                    if (veryVeryVerbose) CR;

                    const int len = i > NUM_VALUES ? NUM_VALUES : i;
                    ASSERTV(i, len == X.length());                      // 3.
                    for (k = 0; k < i; ++k) {
                        if (veryVeryVerbose) DOT;
                        ASSERTV(i, k, X.isMember(VALUES[k % NUM_VALUES]));
                    }
                    if (veryVeryVerbose) CR;
                }                                                       // 4.
            }
        }

        if (verbose) cout << "\tWith 'add' and 'removeAll'" << endl;
        {
            // For each pair of lengths (i, j) up to some modest limit:
            //    1) create an instance
            //    2) add values up to a length of i
            //    3) verify initial length and contents
            //    4) removeAll contents from instance
            //    5) verify length is 0
            //    6) add { V0, V1, V2, V3, V4, V0, ... }  up to length j
            //    7) verify new length and contents
            //    8) allow the instance to leave scope

            const int NUM_TRIALS = 15;
            for (int i = 0; i < NUM_TRIALS; ++i) { // i is first length
                if (veryVerbose) cout <<
                    "\t\tOn an object of initial length " << i << '.' << endl;

                for (int j = 0; j < NUM_TRIALS; ++j) { // j is second length
                    if (veryVeryVerbose) cout <<
                        "\t\t\tAnd with final length " << j << '.' << endl;

                    {
                        int k; // loop index

                        Obj mX;  const Obj& X = mX;                     // 1.
                        for (k = 0; k < i; ++k) {                       // 2.
                            if (veryVeryVeryVerbose) DOT;
                            mX.add(VALUES[k % NUM_VALUES]);
                        }
                        if (veryVeryVeryVerbose) CR;

                        const int len = i > NUM_VALUES ? NUM_VALUES : i;
                        ASSERTV(i, j, len == X.length());               // 3.
                        for (k = 0; k < i; ++k) {
                            if (veryVeryVeryVerbose) DOT;
                            ASSERTV(i, j, k,
                                       X.isMember(VALUES[k % NUM_VALUES]));
                        }
                        if (veryVeryVeryVerbose) CR;

                        mX.removeAll();                                 // 4.
                        ASSERTV(i, j, 0 == X.length());                 // 5.
                        for (k = 0; k < NUM_VALUES; ++k) {
                            if (veryVeryVeryVerbose) DOT;
                            ASSERTV(i, j, k,
                                       !X.isMember(VALUES[k]));
                        }
                        if (veryVeryVeryVerbose) CR;

                        for (k = 0; k < j; ++k) {                       // 6.
                            if (veryVeryVeryVerbose) DOT;
                            mX.add(VALUES[k % NUM_VALUES]);
                        }
                        if (veryVeryVeryVerbose) CR;

                        const int len2 = j > NUM_VALUES ? NUM_VALUES : j;
                        ASSERTV(i, j, len2 == X.length());              // 7.
                        for (k = 0; k < j; ++k) {
                            if (veryVeryVeryVerbose) DOT;
                            ASSERTV(i, j, k,
                                       X.isMember(VALUES[k % NUM_VALUES]));
                        }
                        if (veryVeryVeryVerbose) CR;
                    }                                                   // 8.
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: o Demonstrate operation of:
        //:    1 creators (default, copy, d'tor)
        //:    2 assignment
        //:    3 equality operations
        //:    4 output operator
        //:    5 primary manipulators ('add', 'removeAll')
        //:    6 basic accessor ('length')
        //:    7 When applicable, the above should be tested under aliasing.
        //:    8 Iterator, Reverse iterator
        //:    9 print method
        //:   10 operator<<
        //
        // Plan:
        //:  1 Default construct, compare with itself (C-1, C-3, C-7)
        //:
        //:  2 Copy construct default constructed, compare (C-1, C-3, C-7)
        //:
        //:  3 Add a day to an object (C-5, C-6, C-3)
        //:
        //:  4 Add another day (C-5, C-6, C-3)
        //:
        //:  5 'removeAll' (C-5, C-6, C-3)
        //:
        //:  6 Another default constructed object (C-1, C-3, C-6)
        //:
        //:  7 Another copy constructed object (C-1, C-3, C-6)
        //:
        //:  8 Assign empty (C-2, C-3, C-6)
        //:
        //:  9 Assign non-empty (C-2, C-3, C-6)
        //:
        //: 10 Assign aliasing (C-2, C-3, C-6)
        //:
        //: 11 Iterator, reverse iterator (C-8)
        //:
        //: 12 Print method (C-9)
        //:
        //: 13 operator<< (C-10)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "P-1) Create an object x1 (default ctor)."
                             "\t\t\t{ x1: }" << endl;
        Obj mX1;  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta) Check initial state of x1." << endl;
        ASSERT(0 == X1.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x1 <op> x1." << endl;
        ASSERT((X1 == X1) == 1);        ASSERT((X1 != X1) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "P-2) Create a second object x2 (copy from x1)."
                             "\t\t{ x1: x2: }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check the initial state of x2." << endl;
        ASSERT(0 == X2.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT((X2 == X1) == 1);        ASSERT((X2 != X1) == 0);
        ASSERT((X2 == X2) == 1);        ASSERT((X2 != X2) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "P-3 a) Add an element value A to x1."
                             "\t\t\t{ x1:A x2: }" << endl;
        mX1.add(VA);
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta) Check new state of x1." << endl;
        ASSERT(1 == X1.length());
        ASSERT(X1.isMember(VA));

        if (verbose) cout <<
            "\tb) Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT((X1 == X1) == 1);        ASSERT((X1 != X1) == 0);
        ASSERT((X1 == X2) == 0);        ASSERT((X1 != X2) == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "P-3 b) Add the same element value A to x2."
                             "\t\t\t{ x1:A x2:A }" << endl;
        mX2.add(VA);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check new state of x2." << endl;
        ASSERT(1 == X2.length());
        ASSERT(X2.isMember(VA));

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT((X2 == X1) == 1);        ASSERT((X2 != X1) == 0);
        ASSERT((X2 == X2) == 1);        ASSERT((X2 != X2) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "P 4) Add another element value B to x2."
                             "\t\t\t{ x1:A x2:AB }" << endl;

        mX2.add(VB);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check new state of x2." << endl;
        ASSERT(2 == X2.length());
        ASSERT(X2.isMember(VA));
        ASSERT(X2.isMember(VB));

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT((X2 == X1) == 0);        ASSERT((X2 != X1) == 1);
        ASSERT((X2 == X2) == 1);        ASSERT((X2 != X2) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "P 5) Remove all elements from x1."
                             "\t\t\t{ x1: x2:AB }" << endl;
        mX1.removeAll();
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta) Check new state of x1." << endl;
        ASSERT(0 == X1.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT((X1 == X1) == 1);        ASSERT((X1 != X1) == 0);
        ASSERT((X1 == X2) == 0);        ASSERT((X1 != X2) == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "P 6) Create a third object x3 (default ctor)."
                             "\t\t{ x1: x2:AB x3: }" << endl;

        Obj mX3;  const Obj& X3 = mX3;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout <<
            "\ta) Check new state of x3." << endl;
        ASSERT(0 == X3.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT((X3 == X1) == 1);        ASSERT((X3 != X1) == 0);
        ASSERT((X3 == X2) == 0);        ASSERT((X3 != X2) == 1);
        ASSERT((X3 == X3) == 1);        ASSERT((X3 != X3) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "P 7) Create a forth object x4 (copy of x2)."
                             "\t\t{ x1: x2:AB x3: x4:AB }" << endl;

        Obj mX4(X2);  const Obj& X4 = mX4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout <<
            "\ta) Check new state of x4." << endl;

        ASSERT(2 == X4.length());
        ASSERT(X4.isMember(VA));
        ASSERT(X4.isMember(VB));
        ASSERT(!X4.isMember(VC));

        if (verbose) cout <<
            "\tb) Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT((X4 == X1) == 0);        ASSERT((X4 != X1) == 1);
        ASSERT((X4 == X2) == 1);        ASSERT((X4 != X2) == 0);
        ASSERT((X4 == X3) == 0);        ASSERT((X4 != X3) == 1);
        ASSERT((X4 == X4) == 1);        ASSERT((X4 != X4) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "P 8) Assign x2 = x1 (non-empty becomes empty)."
                             "\t\t{ x1: x2: x3: x4:AB }" << endl;

        mX2 = X1;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check new state of x2." << endl;
        ASSERT(0 == X2.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT((X2 == X1) == 1);        ASSERT((X2 != X1) == 0);
        ASSERT((X2 == X2) == 1);        ASSERT((X2 != X2) == 0);
        ASSERT((X2 == X3) == 1);        ASSERT((X2 != X3) == 0);
        ASSERT((X2 == X4) == 0);        ASSERT((X2 != X4) == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "P 9) Assign x3 = x4 (empty becomes non-empty)."
                             "\t\t{ x1: x2: x3:AB x4:AB }" << endl;

        mX3 = X4;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout <<
            "\ta) Check new state of x3." << endl;
        ASSERT(2 == X3.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT((X3 == X1) == 0);        ASSERT((X3 != X1) == 1);
        ASSERT((X3 == X2) == 0);        ASSERT((X3 != X2) == 1);
        ASSERT((X3 == X3) == 1);        ASSERT((X3 != X3) == 0);
        ASSERT((X3 == X4) == 1);        ASSERT((X3 != X4) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "P 10) Assign x4 = x4 (aliasing)."
                             "\t\t\t\t{ x1: x2: x3:AB x4:AB }" << endl;

        mX4 = X4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout <<
            "\ta) Check new state of x4." << endl;
        ASSERT(2 == X4.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT((X4 == X1) == 0);        ASSERT((X4 != X1) == 1);
        ASSERT((X4 == X2) == 0);        ASSERT((X4 != X2) == 1);
        ASSERT((X4 == X3) == 1);        ASSERT((X4 != X3) == 0);
        ASSERT((X4 == X4) == 1);        ASSERT((X4 != X4) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "P 11 a) Trying the iterator..." << endl;
        {
            if (verbose) {
                cout << "\tX4 = {" << flush;
                for (Obj::iterator it = X4.begin(); it != X4.end(); ++it) {
                    cout << ' ' << *it << flush;
                }
                cout << " }" << endl;
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "P 11 b) Trying the reverse_iterator..." << endl;
        {
            if (verbose) {
                cout << "\tX4 = {" << flush;
                for (Obj::reverse_iterator it = X4.rbegin(); it != X4.rend();
                                                                        ++it) {
                    cout << ' ' << *it << flush;
                }
                cout << " }" << endl;
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "P 12) Trying the 'print' method..." << endl;
        if (verbose) {
            cout << endl;
            X4.print(cout);
            X4.print(cout,  0, 2);
            X4.print(cout,  1, 2);
            X4.print(cout,  2, 2);
            X4.print(cout,  0, -1);  cout << endl;
            X4.print(cout,  0, -2);  cout << endl;
            X4.print(cout,  0, -4);  cout << endl;
            X4.print(cout,  2, -4);  cout << endl;
            X4.print(cout,  4, -4);  cout << endl;
            X4.print(cout, -2, -4);  cout << endl;
            X4.print(cout, -4, -4);  cout << endl;
            X4.print(cout,  0, 2);
            cout << X4 << endl;
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "P 13) Trying 'operator<<'..." << endl;
        if (verbose) {
            cout << endl;
            cout << X4 << endl;
            cout << X4 << X4 << X4 << endl;
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    ASSERT(0 == globalAlloc.numBlocksTotal());
    ASSERT(0 == dfltAlloc.  numBlocksTotal());

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
