// baltzo_localtimeperiod.t.cpp                                       -*-C++-*-
#include <baltzo_localtimeperiod.h>

#include <baltzo_localtimedescriptor.h>

#include <bdlt_datetime.h>

#include <bslalg_swaputil.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_usesallocator.h>

#include <bsls_asserttest.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_review.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a single, simply constrained
// (value-semantic) attribute class.  The Primary Manipulators and Basic
// Accessors are therefore, respectively, the attribute setters and getters,
// each of which follows our standard unconstrained attribute-type naming
// conventions: 'setAttributeName' and 'attributeName'.
//
// Primary Manipulators:
//: o 'setDescriptor'
//: o 'setUtcStartAndEndTime'
//
// Basic Accessors:
//: o 'get_allocator' (orthogonal to value)
//: o 'descriptor'
//: o 'utcStartTime'
//: o 'utcEndTime'
//
// This particular attribute class also provides a value constructor capable of
// creating an object in any state relevant for thorough testing, obviating the
// primitive generator function, 'gg', normally used for this purpose.  We will
// therefore follow our standard 10-case approach to testing value-semantic
// types except that we will test the value constructor in case 3 (in lieu of
// the generator function), with the default constructor and primary
// manipulators tested fully in case 2.
//
// Certain standard value-semantic-type test cases are omitted:
//: o [10] -- BSLX streaming is not (yet) implemented for this class.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Any allocated memory is always from the object allocator.
//: o An object's value is independent of the allocator used to supply memory.
//: o Injected exceptions are safely propagated during memory allocation.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o All explicit memory allocations are presumed to use the global, default,
//:   or object allocator.
//: o ACCESSOR methods are 'const' thread-safe.
//: o Individual attribute types are presumed to be *alias-safe*; hence, only
//:   certain methods require the testing of this property:
//:   o copy-assignment
//:   o swap
// ----------------------------------------------------------------------------
// CLASS METHODS
// [11] bool isValidUtcStartAndEndTime(const bdlt::Datetime& start, end);
//
// CREATORS
// [ 2] LocalTimePeriod(Al a = {});
// [ 3] LocalTimePeriod(const LTD& d, const Dt& s, e, Al a = {});
// [ 7] LocalTimePeriod(const Obj& o, Al a = {});
// [12] LocalTimePeriod(const Obj&& o);
// [12] LocalTimePeriod(const Obj&& o, Al a = {});
// [ 2] ~LocalTimePeriod();
//
// MANIPULATORS
// [ 9] operator=(const Obj& rhs);
// [13] operator=(const Obj&& rhs);
// [ 2] void setDescriptor(const Desc& value);
// [ 2] void setUtcStartAndEndTime(const bdlt::Datetime& start, end);
//
// [ 8] void swap(Obj& other);
//
// ACCESSORS
// [ 4] bslma::Allocator *allocator() const;
// [ 4] bsl::allocator<char> get_allocator() const;
// [ 4] const Obj& descriptor() const;
// [ 4] const bdlt::Datetime& utcStartTime() const;
// [ 4] const bdlt::Datetime& utcEndTime() const;
//
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const Obj& lhs, rhs);
// [ 6] bool operator!=(const Obj& lhs, rhs);
// [ 5] operator<<(ostream& s, const Obj& d);
//
// FREE FUNCTIONS
// [ 8] void swap(Obj& a, b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [14] USAGE EXAMPLE
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ 3] CONCERN: All creator/manipulator ref params are 'const' or '&&'.
// [ 4] CONCERN: All accessor methods are declared 'const'.
// [ 3] CONCERN: String arguments can be either 'char *' or 'string'.
// [ 9] CONCERN: All memory allocation is from the object's allocator.
// [ 9] CONCERN: All memory allocation is exception neutral.
// [ 9] CONCERN: Object value is independent of the object allocator.
// [ 9] CONCERN: There is no temporary allocation from any allocator.
// [10] Reserved for 'bslx' streaming.
//
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
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef baltzo::LocalTimePeriod     Obj;
typedef baltzo::LocalTimeDescriptor Desc;
typedef bslmf::MovableRefUtil       MoveUtil;
typedef bsls::Types::Int64          Int64;

typedef Desc                        T1;  // 'descriptor'
typedef bdlt::Datetime              T2;  // 'utcStartTime'
typedef bdlt::Datetime              T3;  // 'utcEndTime'

// ============================================================================
//                                TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(bslmf::IsBitwiseMoveable<Obj>::value);
BSLMF_ASSERT(bslma::UsesBslmaAllocator<Obj>::value);
BSLMF_ASSERT((bsl::uses_allocator<Obj, bsl::allocator<char> >::value));

// ============================================================================
//                     GLOBAL CONSTANTS USED FOR TESTING
// ----------------------------------------------------------------------------

// Define 'bsl::string' value long enough to ensure dynamic memory allocation.

// JSL: Do we want to move this string to the component of bsl::string itself?
// JSL: e.g.,  #define BSLSTL_LONG_STRING ...   TBD!

#ifdef BSLS_PLATFORM_CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                 "123456789012345678901234567890123"
#endif
BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));

const char *const LONG_STRING    = "a_"   SUFFICIENTLY_LONG_STRING;
const char *const LONGER_STRING  = "ab_"  SUFFICIENTLY_LONG_STRING;
const char *const LONGEST_STRING = "abc_" SUFFICIENTLY_LONG_STRING;

const int UTC_MIN = -24 * 60 * 60 + 1;
const int UTC_MAX =  24 * 60 * 60 - 1;

// Define DEFAULT DATA used by test cases 3, 7, 8, and 9.


// ============================================================================
//                               TEST APPARATUS
// ----------------------------------------------------------------------------

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Converting a UTC Time to a Local Time
/// - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we illustrate how to use a local time period to convert a
// UTC time to the corresponding local time in some time zone.
//
// First, we define a function that performs a conversion from UTC time to
// local time:
//..
    int convertUtcToLocalTime(bdlt::Datetime        *result,
                              const bdlt::Datetime&  utcTime,
                              const Obj&             period)
        // Load into the specified 'result' the date-time value corresponding
        // to the specified 'utcTime' in the local time described by the
        // specified 'period'.  Return 0 on success, and a non-zero value if
        // 'utcTime < period.utcStartTime()' or
        // 'utcTime >= period.utcEndTime()'.
    {
        BSLS_ASSERT(result);

        if (utcTime <  period.utcStartTime() ||
            utcTime >= period.utcEndTime()) {
            return 1;                                                 // RETURN
        }

        *result = utcTime;
        result->addSeconds(period.descriptor().utcOffsetInSeconds());
        return 0;
    }
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: This test driver is reusable w/other, similar components.

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator         allocator("zA", veryVeryVeryVerbose);
    bslma::TestAllocator         defaultAllocator("dA", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    switch (test) { case 0:
      case 14: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

// Then, we create a 'baltzo::LocalTimePeriod' object, 'edt2010', that
// describes New York Daylight-Saving Time in 2010:
//..
    enum { NEW_YORK_DST_OFFSET = -4 * 60 * 60 };  // -4 hours in seconds

    Desc edt(NEW_YORK_DST_OFFSET, true, "EDT");

    baltzo::LocalTimePeriod    edt2010(edt,
                                       bdlt::Datetime(2010,  3, 14, 7),
                                       bdlt::Datetime(2010, 11,  7, 6));

    ASSERT(bdlt::Datetime(2010,  3, 14, 7) == edt2010.utcStartTime());
    ASSERT(bdlt::Datetime(2010, 11,  7, 6) == edt2010.utcEndTime());
    ASSERT("EDT" == edt2010.descriptor().description());
    ASSERT(true  == edt2010.descriptor().dstInEffectFlag());
    ASSERT(NEW_YORK_DST_OFFSET == edt2010.descriptor().utcOffsetInSeconds());
//..
// Next, we create a 'bdlt::Datetime', 'utcDatetime', representing the (UTC)
// time "Jul 20, 2010 11:00":
//..
    bdlt::Datetime utcDatetime(2010, 7, 20, 11, 0, 0);
//..
// Now, we use the 'convertUtcToLocalTime' function we defined earlier to
// convert 'utcDatetime' into its local time in Eastern Daylight Time (as
// described by 'edt2010'):
//..
    bdlt::Datetime localDatetime;
    int           status = convertUtcToLocalTime(&localDatetime,
                                                 utcDatetime,
                                                 edt2010);
    if (0 != status) {
        // The conversion failed so return an error code.

        return 1;                                                     // RETURN
    }
//..
// Finally, we verify that the result corresponds to the expected local time in
// New York, "Jul 20, 2010 7:00":
//..
    ASSERT(bdlt::Datetime(2010, 7, 20, 7) == localDatetime);
//..

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // MOVE-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the assigned-to object
        //   subsequently had the value the assign-from object had before the
        //   assignment.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to the original value of any source object.
        //:
        //: 2 The allocator used by the target object is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 If the allocators don't match, the value of the source object is
        //:   not modified.
        //:
        //: 7 The allocator used by the source object is unchanged.
        //:
        //: 8 QoI: Assigning a source object having the default-constructed
        //:   value allocates no memory.
        //:
        //: 9 Any memory allocation is exception neutral.
        //:
        //:10 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //:11 Every object releases any allocated memory at destruction.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-4)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a column, 'MEM', indicating the
        //:     expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes", or
        //:     ('N') "No".
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..2, 5..8, 11)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     a 'const' 'Obj', 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
        //:
        //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Iterate a 'bool', 'MATCH' from 'false' to 'true'.
        //:
        //:     3 Create a reference, 'za', to either 'scratch' or 'oa',
        //:       depending on 'MATCH'.  'za' will be the allocator with which
        //:       the 'rhs' value of the move-assign will be created, and
        //:       depending on 'MATCH' it will either match the allocator of
        //:       the 'lhs' value.
        //:
        //:     4 Use the copy constructor to create a modifiable object 'Z'
        //:       that is equal to 'ZZ', using the allocator 'za'.
        //:
        //:     5 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     6 Move-assign 'mX' from 'Z' in the presence of injected
        //:       exceptions (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:       macros).
        //:
        //:     7 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     8 Use the equality-comparison operator to verify that: (C-1, 6)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'ZZ'.  (C-1)
        //:
        //:       2 if the allocators don't match, 'Z' still has the same value
        //:         as that of 'ZZ'.  (C-6)
        //:
        //:     9 Use the 'allocator' and 'get_allocator' accessors of both
        //:       'mX' and 'Z' to verify that the respective allocator
        //:       addresses held by the target and source objects are
        //:       unchanged.  (C-2, 7)
        //:
        //:    10 Use the appropriate test allocators to verify that:
        //:       (C-8, 11)
        //:
        //:       1 For an object that (a) is initialized with a value that did
        //:         NOT require memory allocation, and (b) is then assigned a
        //:         value that DID require memory allocation, the target object
        //:         DOES allocate memory from its object allocator only
        //:         (irrespective of the specific number of allocations or the
        //:         total amount of memory allocated); also cross check with
        //:         what is expected for 'mX' and 'Z'.
        //:
        //:       2 An object that is assigned a value that did NOT require
        //:         memory allocation, does NOT allocate memory from its object
        //:         allocator; also cross check with what is expected for 'Z'.
        //:
        //:       3 No additional memory is allocated by the source object.
        //:         (C-8)
        //:
        //:       4 All object memory is released when the object is destroyed.
        //:         (C-11)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3:  (C-9)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mX'; also use the value constructor and a distinct "scratch"
        //:     allocator to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   4 Move-assign 'mX' from 'Z' in the presence of injected
        //:     exceptions (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:     macros).  (C-9)
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the
        //:     target object, 'mX', still has the same value as that of 'ZZ'.
        //:
        //:   7 Use the 'allocator' and 'get_allocator' accessors of 'mX' to
        //:     verify that it is still the object allocator.
        //:
        //:   8 Use the appropriate test allocators to verify that:
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
        //   operator=(const Obj&& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nMOVE-ASSIGNMENT OPERATOR\n"
                               "========================\n";

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorMovePtr)(bslmf::MovableRef<Obj>);

            // Verify that the signature and return type are standard.

            operatorMovePtr operatorMoveAssignment = &Obj::operator=;

            (void)operatorMoveAssignment;  // quash potential compiler warning
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        // Attribute 1 Values: 'descriptor'

        const T1 Adefault(&scratch);
        const T1 Asmall(UTC_MIN, false, "",             &scratch);
        const T1 Alarge(UTC_MAX, true,  LONGEST_STRING, &scratch);

        // Attribute 2 Values: 'utcStartTime'

        const T2 Bdefault;
        const T2 Bsmall(   1,  1,  1,  1,  0,  0, 001);
        const T2 Blarge(9999, 12, 31, 23, 59, 59, 999);

        // Attribute 3 Values: 'utcEndTime'

        const T3 Cdefault;
        const T3 Csmall(   1,  1,  1,  1,  0,  0, 001);
        const T3 Clarge(9999, 12, 31, 23, 59, 59, 999);

        const struct {
            int       d_line;  // source line number
            char      d_mem;   // expected allocation: 'Y', 'N', '?'
            const T1 *d_descriptor_p;
            const T2 *d_utcStartTime_p;
            const T3 *d_utcEndTime_p;
        } DATA[] = {
            //LINE  MEM    DESCR      START      END
            //----  ---    -----      -----      ---

            // default (must be first)
            { L_,   'N',   &Adefault, &Bdefault, &Cdefault },

            // 'descriptor'
            { L_,   'N',   &Asmall,   &Bdefault, &Cdefault },
            { L_,   'Y',   &Alarge,   &Bdefault, &Cdefault },

            // 'utcStartTime' and 'utcEndTime'
            { L_,   'N',   &Adefault, &Bsmall,   &Csmall   },
            { L_,   'N',   &Adefault, &Bsmall,   &Clarge   },
            { L_,   'N',   &Adefault, &Blarge,   &Clarge   },

            // other
            { L_,   'Y',   &Alarge,   &Bsmall,   &Csmall   },
            { L_,   'Y',   &Alarge,   &Bsmall,   &Clarge   },
            { L_,   'Y',   &Alarge,   &Blarge,   &Clarge   },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE1   =  DATA[ti].d_line;
            const char MEMSRC1 =  DATA[ti].d_mem;
            const T1&  DESCR1  = *DATA[ti].d_descriptor_p;
            const T2&  START1  = *DATA[ti].d_utcStartTime_p;
            const T3&  END1    = *DATA[ti].d_utcEndTime_p;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj ZZ(DESCR1, START1, END1, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            ASSERTV(LINE1, Obj(), ZZ, 0 != ti || Obj() == ZZ);

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int  LINE2   =  DATA[tj].d_line;
                const char MEMSRC2 =  DATA[tj].d_mem;
                const T1&  DESCR2  = *DATA[tj].d_descriptor_p;
                const T2&  START2  = *DATA[tj].d_utcStartTime_p;
                const T3&  END2    = *DATA[tj].d_utcEndTime_p;

                for (int tk = 0; tk < 2; ++tk) {
                    const bool MATCH = tk;    // do allocators match?

                    bslma::TestAllocator  oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator& za = MATCH ? oa : scratch;

                    Obj Z(ZZ, &za);
                    ASSERT(ZZ == Z);

                    Obj mX(DESCR2, START2, END2, &oa);  const Obj& X = mX;

                    if (veryVerbose) { T_ P_(LINE2); P_(ZZ); P(X); }

                    ASSERTV(LINE1, LINE2, ZZ, X,
                                                (ZZ == X) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch),
                                                                      zam(&za);

                    int numPasses = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ++numPasses;

                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = MoveUtil::move(Z));
                        ASSERTV(LINE1, LINE2, ZZ,   X, ZZ == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE1, LINE2, ZZ, Z, MATCH || ZZ == Z);

                    ASSERTV(LINE1, LINE2, &oa, &oa == X.get_allocator());
                    ASSERTV(LINE1, LINE2, &scratch, &za == Z.get_allocator());

                    if (MATCH) {
                        ASSERT(1 == numPasses);
                        ASSERTV(LINE1, LINE2,
                                          'Y' != MEMSRC2 || oam.isInUseDown());
                    }
                    else {
                        ASSERTV(zam.isTotalSame());
                        ASSERTV(zam.isInUseSame());

                        if ('Y' == MEMSRC1) {
                            if ('N' == MEMSRC2) {
                                ASSERTV(LINE1, LINE2, 1 < numPasses);
                                ASSERTV(LINE1, LINE2, oam.isInUseUp());
                            }
                            else {
                                ASSERTV(LINE1, LINE2, 1 == numPasses);
                                ASSERTV(LINE1, LINE2, oam.isInUseSame());
                            }
                        }
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());
                }
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                      Obj mX(DESCR1, START1, END1, &oa);
                const Obj ZZ(DESCR1, START1, END1, &scratch);

                const Obj& Z = mX;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;

                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = MoveUtil::move(Z));
                    ASSERTV(LINE1, ZZ,   Z, ZZ == Z);
                    ASSERTV(LINE1, ZZ,  mX, ZZ == mX);
                    ASSERTV(LINE1, mR, &mX, mR == &mX);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(1 == numPasses);

                ASSERTV(LINE1, &oa, &oa == Z.get_allocator());

                ASSERTV(LINE1, oam.isInUseSame());

                ASSERTV(LINE1, sam.isInUseSame());

                ASSERTV(LINE1, 0 == da.numBlocksTotal());
            }
        }

        // Double check that some object memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTOR
        //   Ensure that we can create a new object of the class from any
        //   other one, such that the new object has the same value that the
        //   old one did prior to the move.
        //
        // Concerns:
        //: 1 The move constructor (with or without a supplied allocator)
        //:   creates an object having the same value as the original value of
        //:   the supplied original object.
        //:
        //: 2 If an allocator is NOT supplied to the move constructor, the
        //:   allocator of the source object the object allocator for the
        //:   resulting object.
        //:
        //: 3 If an allocator IS supplied to the move constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying no allocator propagates the allocator to the new
        //:   object.
        //:
        //: 5 Supplying an allocator to the move constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 If no allocator is passed, or if an allocator matching that of
        //:   the source object is passed, no memory allocation takes place.
        //:
        //: 8 There is no temporary memory allocation from any allocator.
        //:
        //: 9 Every object releases any allocated memory at destruction.
        //:
        //:10 If an allocator is supplied that does not match the allocator of
        //:   the original object, the value of the original object is
        //:   unchanged.
        //:
        //:11 The allocator used by the original object is unchanged.
        //:
        //:12 QoI: Moving an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:13 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No".
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..12)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     one 'const' 'Obj', 'ZZ', having the value 'V'.
        //:
        //:   2 Execute an inner 'config' loop choosing a different way to call
        //:     the move constructor.
        //:
        //:   3 For each of these 'config' iterations (P-2.2):  (C-1..12)
        //:
        //:     1 Create four 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Create a modifiable object 'Z' with the same value as 'ZZ',
        //:       and using the allocator 'za'.
        //:
        //:     3 Execute an inner loop that creates an object by
        //:       move-constructing from a newly created object with value V,
        //:       but invokes the move constructor differently in each
        //:       iteration: (a) using the standard single-argument move
        //:       constructor, (b) using the extended move constructor with a
        //:       default-constructed allocator argument (to use the default
        //:       allocator), (c) using the extended move constructor with the
        //:       same allocator as the moved-from object, and (d) using the
        //:       extended move constructor with a different allocator than the
        //:       moved-from object.
        //:
        //:     3 Use the equality-comparison operator to verify that:
        //:       (C-1, 5, 10)
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         that of 'ZZ'.  (C-1, 5)
        //:
        //:       2 If the new object does not use the same allocator as the
        //:         original object, the original object is unchanged.
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also use the 'allocator'
        //:       accessor of 'X' to verify that its object allocator is
        //:       properly installed, and use the 'allocator' accessor of 'Z'
        //:       to verify that the allocator that it holds is unchanged.
        //:       (C-6, 11)
        //:
        //:     5 Use the appropriate test allocators to verify that:  (C-2..4,
        //:       7..8, 12)
        //:
        //:       1 An object that IS expected to allocate memory does so
        //:         from the object allocator only (irrespective of the
        //:         specific number of allocations or the total amount of
        //:         memory allocated).  (C-2, 4)
        //:
        //:       2 An object that is expected NOT to allocate memory doesn't.
        //:         (C-12)
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         current default allocator doesn't allocate any memory.
        //:         (C-3)
        //:
        //:       4 No temporary memory is allocated from any allocator.
        //:         (C-7)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-8)
        //:
        //: 3 Test again, using the data of P-1, but this time just for the
        //:   supplied allocator configuration (P-2.2c), and create the object
        //:   as an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:   (C-13)
        //:
        //:   1 Observe that no throws happened on a move where the created
        //:     object uses the same allocator as the original object.
        //
        // Testing:
        //   LocalTimePeriod(const Obj&& o);
        //   LocalTimePeriod(const Obj&& o, Al a = {});
        // --------------------------------------------------------------------

        if (verbose) cout << "\nMOVE CONSTRUCTOR\n"
                               "================\n";

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        // Attribute 1 Values: 'descriptor'

        const T1 Adefault(&scratch);
        const T1 Asmall(UTC_MIN, false, "",             &scratch);
        const T1 Alarge(UTC_MAX, true,  LONGEST_STRING, &scratch);

        // Attribute 2 Values: 'utcStartTime'

        const T2 Bdefault;
        const T2 Bsmall(   1,  1,  1,  1,  0,  0, 001);
        const T2 Blarge(9999, 12, 31, 23, 59, 59, 999);

        // Attribute 3 Values: 'utcEndTime'

        const T3 Cdefault;
        const T3 Csmall(   1,  1,  1,  1,  0,  0, 001);
        const T3 Clarge(9999, 12, 31, 23, 59, 59, 999);
        const struct {
            int       d_line;  // source line number
            char      d_mem;   // expected allocation: 'Y', 'N', '?'
            const T1 *d_descriptor_p;
            const T2 *d_utcStartTime_p;
            const T3 *d_utcEndTime_p;
        } DATA[] = {
            //LINE  MEM    DESCR      START      END
            //----  ---    -----      -----      ---

            // default (must be first)
            { L_,   'N',   &Adefault, &Bdefault, &Cdefault },

            // 'descriptor'
            { L_,   'N',   &Asmall,   &Bdefault, &Cdefault },
            { L_,   'Y',   &Alarge,   &Bdefault, &Cdefault },

            // 'utcStartTime' and 'utcEndTime'
            { L_,   'N',   &Adefault, &Bsmall,   &Csmall   },
            { L_,   'N',   &Adefault, &Bsmall,   &Clarge   },
            { L_,   'N',   &Adefault, &Blarge,   &Clarge   },

            // other
            { L_,   'Y',   &Alarge,   &Bsmall,   &Csmall   },
            { L_,   'Y',   &Alarge,   &Bsmall,   &Clarge   },
            { L_,   'Y',   &Alarge,   &Blarge,   &Clarge   },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE  =  DATA[ti].d_line;
                const char MEM   =  DATA[ti].d_mem;
                const T1&  DESCR = *DATA[ti].d_descriptor_p;
                const T2&  START = *DATA[ti].d_utcStartTime_p;
                const T3&  END   = *DATA[ti].d_utcEndTime_p;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                const Obj ZZ(DESCR, START, END, &scratch);

                if (veryVerbose) { T_ P(ZZ) }

                bool done = false;
                for (char cfg = 'a'; !done; ++cfg) {
                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                    bslma::TestAllocator za("zobject",   veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    bsl::allocator<char> daa(&da);
                    bsl::allocator<char> saa(&sa);
                    bsl::allocator<char> zaa(&za);

                    Obj  Z(DESCR, START, END, zaa);

                    const Int64 zaAlloc = za.numBytesInUse();

                    bslma::TestAllocatorMonitor zam(&za);

                    Obj                  *objPtr;
                    Int64                 objAlloc;
                    bslma::TestAllocator *objAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        objAllocatorPtr = &za;
                        objAlloc        = objAllocatorPtr->numBytesInUse();
                        objPtr = new (fa) Obj(MoveUtil::move(Z));
                      } break;
                      case 'b': {
                        objAllocatorPtr = &da;
                        objAlloc        = objAllocatorPtr->numBytesInUse();
                        objPtr = new (fa) Obj(MoveUtil::move(Z), 0);
                      } break;
                      case 'c': {
                        objAllocatorPtr = &da;
                        objAlloc        = objAllocatorPtr->numBytesInUse();
                        objPtr = new (fa) Obj(MoveUtil::move(Z),
                                              bsl::allocator<char>());
                      } break;
                      case 'd': {
                        objAllocatorPtr = &da;
                        objAlloc        = objAllocatorPtr->numBytesInUse();
                        objPtr = new (fa) Obj(MoveUtil::move(Z), daa);
                      } break;
                      case 'e': {
                        objAllocatorPtr = &sa;
                        objAlloc        = objAllocatorPtr->numBytesInUse();
                        objPtr = new (fa) Obj(MoveUtil::move(Z), &sa);
                      } break;
                      case 'f': {
                        objAllocatorPtr = &sa;
                        objAlloc        = objAllocatorPtr->numBytesInUse();
                        objPtr = new (fa) Obj(MoveUtil::move(Z), saa);
                      } break;
                      case 'g': {
                        objAllocatorPtr = &za;
                        objAlloc        = objAllocatorPtr->numBytesInUse();
                        objPtr = new (fa) Obj(MoveUtil::move(Z), &za);
                      } break;
                      case 'h': {
                        objAllocatorPtr = &za;
                        objAlloc        = objAllocatorPtr->numBytesInUse();
                        objPtr = new (fa) Obj(MoveUtil::move(Z), zaa);
                        done = true;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(LINE, CONFIG,
                                 sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;

                    ASSERT(zam.isTotalSame());
                    ASSERT(zam.isInUseSame());

                    ASSERTV(&oa == &da || 0 == da.numAllocations());

                    // Ensure the first row of the table contains the
                    // default-constructed value.

                    if (0 == ti) {
                        ASSERTV(LINE, CONFIG, Obj(), *objPtr,
                                     Obj() == *objPtr);
                    }

                    // Verify the value of the object.

                    ASSERTV(LINE, CONFIG, ZZ, Z, ZZ == X);
                    ASSERTV(LINE, CONFIG, ZZ, Z, &oa == &za || ZZ == Z);

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    ASSERTV(LINE, CONFIG,
                                        &oa == X.descriptor().get_allocator());

                    // Also invoke the object's 'allocator' accessor, as well
                    // as that of 'Z'.

                    ASSERTV(LINE, CONFIG, &oa, &oa == X.get_allocator());

                    ASSERTV(LINE, CONFIG, &scratch, &za == Z.get_allocator());

                    // Verify no temporary memory was freed from any allocator.

                    bslma::TestAllocator *allocs[] = { &da, &fa, &sa, &za, 0 };
                    for (bslma::TestAllocator **ppAlloc = allocs;
                                                          *ppAlloc; ++ppAlloc){
                        bslma::TestAllocator *alloc = *ppAlloc;

                        ASSERTV(LINE, CONFIG, ppAlloc - allocs,
                           alloc->numBlocksTotal(), alloc->numBlocksInUse(),
                           alloc->numBlocksTotal() == alloc->numBlocksInUse());
                    }

                    // Verify expected object-memory allocations.

                    ASSERTV(LINE, CONFIG, MEM, oa.numBlocksInUse(),
                                   ('N' == MEM) == (0 == oa.numBlocksInUse()));
                    ASSERT(da.numBytesInUse() ==
                                     (&da == &oa && 'Y' == MEM ? zaAlloc : 0));
                    ASSERT(oa.numBytesInUse() - objAlloc ==
                                     (&za != &oa && 'Y' == MEM ? zaAlloc : 0));

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                                 0 == da.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                                 0 == fa.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, sa.numBlocksInUse(),
                                 0 == sa.numBlocksInUse());
                }  // end foreach configuration
                ASSERT(done);
            }  // end foreach row

            // Double check that some object memory was allocated in some row.

            ASSERT(anyObjectMemoryAllocatedFlag);

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE  =  DATA[ti].d_line;
                const char MEM   =  DATA[ti].d_mem;
                const T1&  DESCR = *DATA[ti].d_descriptor_p;
                const T2&  START = *DATA[ti].d_utcStartTime_p;
                const T3&  END   = *DATA[ti].d_utcEndTime_p;

                const Obj ZZ(DESCR, START, END, &scratch);

                bool done = false;
                for (char c = 'a'; !done; ++c) {
                    const char CONFIG = c;

                    bslma::TestAllocator scratch("scratch",
                                                          veryVeryVeryVerbose);
                    bslma::TestAllocator za(     "zobject",
                                                          veryVeryVeryVerbose);
                    Obj Z(ZZ, &za);

                    ASSERT(ZZ == Z);

                    if (veryVerbose) { T_ P_(CONFIG); P_(MEM); P(ZZ); }

                    bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    bslma::TestAllocator& oa = 'a' == CONFIG ? sa : za;

                    int numPasses = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ++numPasses;

                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        ASSERTV(numPasses, ZZ == Z);

                        bsls::ObjectBuffer<Obj> ob;
                        switch (CONFIG) {
                          case 'a': {
                            new (ob.address()) Obj(MoveUtil::move(Z), &sa);
                          } break;
                          case 'b': {
                            new (ob.address()) Obj(MoveUtil::move(Z));
                          } break;
                          case 'c': {
                            new (ob.address()) Obj(MoveUtil::move(Z), &za);
                            done = true;
                          } break;
                          default: {
                            ASSERTV(CONFIG, !"second switch failed");
                          }
                        }

                        Obj& obj = ob.object();
                        ASSERTV(LINE, ZZ, obj, ZZ == obj);
                        ASSERT(&oa == obj.get_allocator());

                        obj.~Obj();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(&oa == &sa || 1 == numPasses);
                    ASSERTV(LINE, ZZ, Z, &oa == &za || ZZ == Z);

                    ASSERTV(LINE, &za, &za == Z.get_allocator());
                }
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'isValidUtcStartAndEndTime'
        //   Ensure that the method correctly identifies the valid range.
        //
        // Concerns:
        //: 1 The method correctly identifies the valid range of values for the
        //:   'utcStartTime' and 'utcEndTime' attributes.
        //:
        //: 2 The (predicate) method under test has return type 'bool'.
        //:
        //: 3 There is no allocation from any allocator.
        //
        // The 'isValidUtcStartAndEndTime' must provide the following
        // symmetric property when 'A' and 'B' are comparable (i.e., neither:
        // equals the default value).
        //..
        //  if (A == B) {
        //      ASSERT(isValidUtcStartAndEndTime(A, B)
        //          == isValidUtcStartAndEndTime(B, A)
        //  } else {
        //      ASSERT(isValidUtcStartAndEndTime(A, B)
        //         == !isValidUtcStartAndEndTime(B, A)
        //  }
        //..
        //
        // Plan:
        //: 1 Use the address of the 'isValidUtcOffsetInSeconds' class method
        //:   to initialize a pointer to a function having the appropriate
        //:   signature and return type.  (C-2)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Using the category partitioning method, for each attribute,
        //:     select candidate attribute values including values at the near
        //:     the boundaries.  Treat the default value ('bdlt::Datetime()')
        //:     as a distinguished since it is not comparable to any other
        //:     value (and our method must return 'false'.  When values are
        //:     comparable, generate a case with the values in reverse order
        //:     and check the symmetry property of this method.
        //:
        //:   2 Additionally provide a column, 'EXPECTED', that specifies
        //:     whether the value in that row is valid for the attribute.
        //:
        //: 4 For each row (representing a distinct attribute value, 'V') in
        //:   the table of P-3, verify that the class method returns the
        //:   expected value.  (C-1)
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   bool isValidUtcStartAndEndTime(const bdlt::Datetime& start, end);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "CLASS METHOD 'isValidUtcStartAndEndTime'" << endl
                 << "========================================" << endl;

        if (verbose) cout <<
           "\nAssign the address of 'isValidUtcStartAndEndTime' to a variable."
                                                                       << endl;
        {
            typedef bool (*freeFuncPtr)(const bdlt::Datetime&,
                                        const bdlt::Datetime&);

            // Verify that the signature and return type are as expected.

            freeFuncPtr funcPtr = &Obj::isValidUtcStartAndEndTime;

            (void)funcPtr;  // quash potential compiler warning
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
            "\nCreate a table of distinct candidate attribute values." << endl;

        const bdlt::Datetime      defaultDt;
        const bdlt::Datetime       earliest(   1,  1,  1,  1,  0,  0, 000);
        const bdlt::Datetime almostEarliest(   1,  1,  1,  1,  0,  0, 001);
        const bdlt::Datetime           some(2010,  4, 15,  8);
        const bdlt::Datetime         latest(9999, 12, 31, 23, 59, 59, 999);
        const bdlt::Datetime   almostLatest(9999, 12, 31, 23, 59, 59, 998);

        static const struct {
            int       d_line;           // source line number
            const T2 *d_utcStartTime_p;
            const T3 *d_utcEndTime_p;
            bool      d_isValidUtcStartAndEndTime;
        } DATA[] = {

            //LINE         START              END   EXP
            //---- -------------  ---------------   -----

            // default (must be first)
            { L_,      &defaultDt,      &defaultDt, true  },

            { L_,      &defaultDt, &almostEarliest, false },
            { L_,      &defaultDt,           &some, false },
            { L_,      &defaultDt,   &almostLatest, false },
            { L_,      &defaultDt,         &latest, false },

            { L_,  &almostEarliest,     &defaultDt, false },
            { L_,            &some,     &defaultDt, false },
            { L_,    &almostLatest,     &defaultDt, false },
            { L_,          &latest,     &defaultDt, false },

            { L_, &almostEarliest, &almostEarliest, true  },
            { L_, &almostEarliest,           &some, true  },
            { L_, &almostEarliest,   &almostLatest, true  },
            { L_, &almostEarliest,         &latest, true  },

            { L_, &almostEarliest, &almostEarliest, true  },
            { L_,           &some, &almostEarliest, false },
            { L_,   &almostLatest, &almostEarliest, false },
            { L_,         &latest, &almostEarliest, false },

            { L_,           &some, &almostEarliest, false },
            { L_,           &some,           &some, true  },
            { L_,           &some,   &almostLatest, true  },
            { L_,           &some,         &latest, true  },

            { L_, &almostEarliest,           &some, true  },
            { L_,           &some,           &some, true  },
            { L_,   &almostLatest,           &some, false },
            { L_,         &latest,           &some, false },

            { L_,   &almostLatest, &almostEarliest, false },
            { L_,   &almostLatest,           &some, false },
            { L_,   &almostLatest,   &almostLatest, true  },
            { L_,   &almostLatest,         &latest, true  },

            { L_, &almostEarliest,   &almostLatest, true  },
            { L_,           &some,   &almostLatest, true  },
            { L_,   &almostLatest,   &almostLatest, true  },
            { L_,         &latest,   &almostLatest, false },

            { L_,         &latest, &almostEarliest, false },
            { L_,         &latest,           &some, false },
            { L_,         &latest,   &almostLatest, false },
            { L_,         &latest,         &latest, true  },

            { L_, &almostEarliest,         &latest, true  },
            { L_,           &some,         &latest, true  },
            { L_,   &almostLatest,         &latest, true  },
            { L_,         &latest,         &latest, true  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE     =  DATA[ti].d_line;
            const T2&  START    = *DATA[ti].d_utcStartTime_p;
            const T3&  END      = *DATA[ti].d_utcEndTime_p;
            const bool EXPECTED =  DATA[ti].d_isValidUtcStartAndEndTime;

            if (veryVerbose) { T_ P_(START) P_(END) P(EXPECTED) }

            LOOP4_ASSERT(LINE, START, END, EXPECTED,
                         EXPECTED ==
                                   Obj::isValidUtcStartAndEndTime(START, END));

            LOOP2_ASSERT(LINE, da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BDEX STREAMING
        //   Ensure that we can serialize the value of any object of the class,
        //   and then deserialize that value back into any object of the class.
        //
        // Concerns:
        //: 1 N/A
        //
        // Plan:
        //: 1 N/A
        //
        // Testing:
        //   Reserved for BDEX streaming.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BDEX STREAMING" << endl
                          << "==============" << endl;

        if (verbose) cout << "Not yet implemented." << endl;

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The allocator used by the target object is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 The value of the source object is not modified.
        //:
        //: 7 The allocator used by the source object is unchanged.
        //:
        //: 8 QoI: Assigning a source object having the default-constructed
        //:   value allocates no memory.
        //:
        //: 9 Any memory allocation is exception neutral.
        //:
        //:10 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //:11 Every object releases any allocated memory at destruction.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-4)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..2, 5..8, 11)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
        //:
        //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     3 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:
        //:     4 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     5 Use the equality-comparison operator to verify that: (C-1, 6)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
        //:
        //:     6 Use the 'get_allocator' accessor of both 'mX' and 'Z' to
        //:       verify that the respective allocators held by the target and
        //:       source objects are unchanged.  (C-2, 7)
        //:
        //:     7 Use the appropriate test allocators to verify that:
        //:       (C-8, 11)
        //:
        //:       1 For an object that (a) is initialized with a value that did
        //:         NOT require memory allocation, and (b) is then assigned a
        //:         value that DID require memory allocation, the target object
        //:         DOES allocate memory from its object allocator only
        //:         (irrespective of the specific number of allocations or the
        //:         total amount of memory allocated); also cross check with
        //:         what is expected for 'mX' and 'Z'.
        //:
        //:       2 An object that is assigned a value that did NOT require
        //:         memory allocation, does NOT allocate memory from its object
        //:         allocator; also cross check with what is expected for 'Z'.
        //:
        //:       3 No additional memory is allocated by the source object.
        //:         (C-8)
        //:
        //:       4 All object memory is released when the object is destroyed.
        //:         (C-11)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3:  (C-9)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mX'; also use the value constructor and a distinct "scratch"
        //:     allocator to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   4 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:     (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:     (C-9)
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the
        //:     target object, 'mX', still has the same value as that of 'ZZ'.
        //:
        //:   7 Use the 'get_allocator' accessor of 'mX' to verify that it is
        //:     still the object allocator.
        //:
        //:   8 Use the appropriate test allocators to verify that:
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
        //   operator=(const Obj& rhs);
        //   CONCERN: All memory allocation is from the object's allocator.
        //   CONCERN: All memory allocation is exception neutral.
        //   CONCERN: Object value is independent of the object allocator.
        //   CONCERN: There is no temporary allocation from any allocator.
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

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        // Attribute 1 Values: 'descriptor'

        const T1 Adefault(&scratch);
        const T1 Asmall(UTC_MIN, false, "",             &scratch);
        const T1 Alarge(UTC_MAX, true,  LONGEST_STRING, &scratch);

        // Attribute 2 Values: 'utcStartTime'

        const T2 Bdefault;
        const T2 Bsmall(   1,  1,  1,  1,  0,  0, 001);
        const T2 Blarge(9999, 12, 31, 23, 59, 59, 999);

        // Attribute 3 Values: 'utcEndTime'

        const T3 Cdefault;
        const T3 Csmall(   1,  1,  1,  1,  0,  0, 001);
        const T3 Clarge(9999, 12, 31, 23, 59, 59, 999);

        const struct {
            int       d_line;  // source line number
            char      d_mem;   // expected allocation: 'Y', 'N', '?'
            const T1 *d_descriptor_p;
            const T2 *d_utcStartTime_p;
            const T3 *d_utcEndTime_p;
        } DATA[] = {
            //LINE  MEM    DESCR      START      END
            //----  ---    -----      -----      ---

            // default (must be first)
            { L_,   'N',   &Adefault, &Bdefault, &Cdefault },

            // 'descriptor'
            { L_,   'N',   &Asmall,   &Bdefault, &Cdefault },
            { L_,   'Y',   &Alarge,   &Bdefault, &Cdefault },

            // 'utcStartTime' and 'utcEndTime'
            { L_,   'N',   &Adefault, &Bsmall,   &Csmall   },
            { L_,   'N',   &Adefault, &Bsmall,   &Clarge   },
            { L_,   'N',   &Adefault, &Blarge,   &Clarge   },

            // other
            { L_,   'Y',   &Alarge,   &Bsmall,   &Csmall   },
            { L_,   'Y',   &Alarge,   &Bsmall,   &Clarge   },
            { L_,   'Y',   &Alarge,   &Blarge,   &Clarge   },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE1   =  DATA[ti].d_line;
            const char MEMSRC1 =  DATA[ti].d_mem;
            const T1&  DESCR1  = *DATA[ti].d_descriptor_p;
            const T2&  START1  = *DATA[ti].d_utcStartTime_p;
            const T3&  END1    = *DATA[ti].d_utcEndTime_p;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj  Z(DESCR1, START1, END1, &scratch);
            const Obj ZZ(DESCR1, START1, END1, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int  LINE2   =  DATA[tj].d_line;
                const char MEMSRC2 =  DATA[tj].d_mem;
                const T1&  DESCR2  = *DATA[tj].d_descriptor_p;
                const T2&  START2  = *DATA[tj].d_utcStartTime_p;
                const T3&  END2    = *DATA[tj].d_utcEndTime_p;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(DESCR2, START2, END2, &oa);  const Obj& X = mX;

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    LOOP4_ASSERT(LINE1, LINE2, Z, X,
                                 (Z == X) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = Z);
                        LOOP4_ASSERT(LINE1, LINE2,  Z,   X,  Z == X);
                        LOOP4_ASSERT(LINE1, LINE2, mR, &mX, mR == &mX);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    LOOP4_ASSERT(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    ASSERTV(LINE1, LINE2, &oa, &oa == X.get_allocator());
                    ASSERTV(LINE1, LINE2, &scratch,
                                                &scratch == Z.get_allocator());

                    if ('N' == MEMSRC2 && 'Y' == MEMSRC1) {
                        LOOP2_ASSERT(LINE1, LINE2, oam.isInUseUp());
                    }
                    else if ('Y' == MEMSRC2) {
                        LOOP2_ASSERT(LINE1, LINE2, oam.isInUseSame());
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    LOOP2_ASSERT(LINE1, LINE2, sam.isInUseSame());

                    LOOP2_ASSERT(LINE1, LINE2, 0 == da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                LOOP3_ASSERT(LINE1, LINE2, oa.numBlocksInUse(),
                             0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                      Obj mX(DESCR1, START1, END1, &oa);
                const Obj ZZ(DESCR1, START1, END1, &scratch);

                const Obj& Z = mX;

                LOOP3_ASSERT(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = Z);
                    LOOP3_ASSERT(LINE1, ZZ,   Z, ZZ == Z);
                    LOOP3_ASSERT(LINE1, mR, &mX, mR == &mX);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE1, &oa, &oa == Z.get_allocator());

                LOOP_ASSERT(LINE1, oam.isInUseSame());

                LOOP_ASSERT(LINE1, sam.isInUseSame());

                LOOP_ASSERT(LINE1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            LOOP2_ASSERT(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }

        // Double check that some object memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that, when member and free 'swap' are implemented, we can
        //   exchange the values of any two objects that use the same
        //   allocator.
        //
        // Concerns:
        //: 1 Both functions exchange the values of the (two) supplied objects.
        //:
        //: 2 The common object allocator used by both objects is unchanged.
        //:
        //: 3 The member function does not allocate memory from any allocator;
        //:   nor does the free function when the two objects being swapped use
        //:   the same allocator.
        //:
        //: 4 The free function can be called with two objects that use
        //:   different allocators.
        //:
        //: 5 Both functions have standard signatures and return types.
        //:
        //: 6 Using either function to swap an object with itself does not
        //:   affect the value of the object (alias-safety).
        //:
        //: 7 The free 'swap' function is discoverable through ADL (Argument
        //:   Dependent Lookup).
        //:
        //: 8 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the addresses of the 'swap' member and free functions defined
        //:   in this component to initialize, respectively, member-function
        //:   and free-function pointers having the appropriate signatures and
        //:   return types.  (C-5)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-1..2, 6)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable
        //:     'Obj', 'mW', having the value described by 'R1'; also use the
        //:     copy constructor and a "scratch" allocator to create a 'const'
        //:     'Obj' 'XX' from 'mW'.
        //:
        //:   3 Use the member and free 'swap' functions to swap the value of
        //:     'mW' with itself; verify, after each swap, that:  (C-6)
        //:
        //:     1 The value is unchanged.  (C-6)
        //:
        //:     2 The allocator used by the object is unchanged.
        //:
        //:     3 There was no additional object memory allocation.
        //:
        //:   4 For each row 'R2' in the table of P-3:  (C-1..2)
        //:
        //:     1 Use the copy constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', from 'XX' (P-4.2).
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mY', and having the value described by 'R2'; also use
        //:       the copy constructor to create, using a "scratch" allocator,
        //:       a 'const' 'Obj', 'YY', from 'Y'.
        //:
        //:     3 Use, in turn, the member and free 'swap' functions to swap
        //:       the values of 'mX' and 'mY'; verify, after each swap, that:
        //:       (C-1..2)
        //:
        //:       1 The values have been exchanged.  (C-1)
        //:
        //:       2 The common object allocator used by 'mX' and 'mY' is
        //:         unchanged in both objects.  (C-2)
        //:
        //:       3 There was no additional object memory allocation.
        //:
        //: 5 Verify that the free 'swap' function is discoverable through ADL:
        //:   (C-7)
        //:
        //:   1 Create a set of attribute values, 'A', distinct from the values
        //:     corresponding to the default-constructed object, choosing
        //:     values that allocate memory if possible.
        //:
        //:   2 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   3 Use the default constructor and 'oa' to create a modifiable
        //:     'Obj' 'mX' (having default attribute values); also use the copy
        //:     constructor and a "scratch" allocator to create a 'const' 'Obj'
        //:     'XX' from 'mX'.
        //:
        //:   4 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mY' having the value described by the 'Ai' attributes; also
        //:     use the copy constructor and a "scratch" allocator to create a
        //:     'const' 'Obj' 'YY' from 'mY'.
        //:
        //:   5 Use the 'bslalg::SwapUtil' helper function template to swap the
        //:     values of 'mX' and 'mY', using the free 'swap' function defined
        //:     in this component, then verify that:  (C-7)
        //:
        //:     1 The values have been exchanged.
        //:
        //:     2 There was no additional object memory allocation.  (C-7)
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory was
        //:   allocated from the default allocator.  (C-3)
        //:
        //: 7 Verify that free 'swap' exchanges the values of any two objects
        //:   that use different allocators.  (C-4)
        //:
        //: 8 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when, using the member 'swap' function, an attempt is
        //:   made to swap objects that do not refer to the same allocator, but
        //:   not when the allocators are the same (using the
        //:   'BSLS_ASSERTTEST_*' macros).  (C-8)
        //
        // Testing:
        //   void swap(Obj& other);
        //   void swap(Obj& a, b);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTIONS" << endl
                          << "==============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each function to a variable." << endl;
        {
            typedef void (Obj::*funcPtr)(Obj&);
            typedef void (*freeFuncPtr)(Obj&, Obj&);

            using namespace baltzo;

            // Verify that the signatures and return types are standard.

            funcPtr     memberSwap = &Obj::swap;
            freeFuncPtr freeSwap   = swap;

            (void)memberSwap;  // quash potential compiler warnings
            (void)freeSwap;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        // Attribute 1 Values: 'descriptor'

        const T1 Adefault(&scratch);
        const T1 Asmall(UTC_MIN, false, "",             &scratch);
        const T1 Alarge(UTC_MAX, true,  LONGEST_STRING, &scratch);

        // Attribute 2 Values: 'utcStartTime'

        const T2 Bdefault;
        const T2 Bsmall(   1,  1,  1,  1,  0,  0, 001);
        const T2 Blarge(9999, 12, 31, 23, 59, 59, 999);

        // Attribute 3 Values: 'utcEndTime'

        const T3 Cdefault;
        const T3 Csmall(   1,  1,  1,  1,  0,  0, 001);
        const T3 Clarge(9999, 12, 31, 23, 59, 59, 999);

        const struct {
            int       d_line;  // source line number
            char      d_mem;   // expected allocation: 'Y', 'N', '?'
            const T1 *d_descriptor_p;
            const T2 *d_utcStartTime_p;
            const T3 *d_utcEndTime_p;
        } DATA[] = {
            //LINE  MEM    DESCR      START      END
            //----  ---    -----      -----      ---

            // default (must be first)
            { L_,   'N',   &Adefault, &Bdefault, &Cdefault },

            // 'descriptor'
            { L_,   'N',   &Asmall,   &Bdefault, &Cdefault },
            { L_,   'Y',   &Alarge,   &Bdefault, &Cdefault },

            // 'utcStartTime' and 'utcEndTime'
            { L_,   'N',   &Adefault, &Bsmall,   &Csmall   },
            { L_,   'N',   &Adefault, &Bsmall,   &Clarge   },
            { L_,   'N',   &Adefault, &Blarge,   &Clarge   },

            // other
            { L_,   'Y',   &Alarge,   &Bsmall,   &Csmall   },
            { L_,   'Y',   &Alarge,   &Bsmall,   &Clarge   },
            { L_,   'Y',   &Alarge,   &Blarge,   &Clarge   },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE1  =  DATA[ti].d_line;
            const char MEM1   =  DATA[ti].d_mem;
            const T1&  DESCR1 = *DATA[ti].d_descriptor_p;
            const T2&  START1 = *DATA[ti].d_utcStartTime_p;
            const T3&  END1   = *DATA[ti].d_utcEndTime_p;

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                  Obj mW(DESCR1, START1, END1, &oa);  const Obj& W = mW;
            const Obj XX(W, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1, Obj(), W, Obj() == W);
                firstFlag = false;
            }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mW.swap(mW);

                LOOP3_ASSERT(LINE1, XX, W, XX == W);
                LOOP_ASSERT(LINE1, &oa == W.get_allocator());
                LOOP_ASSERT(LINE1, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mW, mW);

                LOOP3_ASSERT(LINE1, XX, W, XX == W);
                LOOP_ASSERT(LINE1, &oa == W.get_allocator());
                LOOP_ASSERT(LINE1, oam.isTotalSame());
            }

            // Verify expected ('Y'/'N') object-memory allocations.

            if ('?' != MEM1) {
                LOOP3_ASSERT(LINE1, MEM1, oa.numBlocksInUse(),
                           ('N' == MEM1) == (0 == oa.numBlocksInUse()));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int  LINE2  =  DATA[tj].d_line;
                const T1&  DESCR2 = *DATA[tj].d_descriptor_p;
                const T2&  START2 = *DATA[tj].d_utcStartTime_p;
                const T3&  END2   = *DATA[tj].d_utcEndTime_p;

                      Obj mX(XX, &oa);  const Obj& X = mX;

                      Obj mY(DESCR2, START2, END2, &oa);  const Obj& Y = mY;
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    mX.swap(mY);

                    LOOP4_ASSERT(LINE1, LINE2, YY, X, YY == X);
                    LOOP4_ASSERT(LINE1, LINE2, XX, Y, XX == Y);
                    LOOP2_ASSERT(LINE1, LINE2, &oa == X.get_allocator());
                    LOOP2_ASSERT(LINE1, LINE2, &oa == Y.get_allocator());
                    LOOP2_ASSERT(LINE1, LINE2, oam.isTotalSame());
                }

                // free function 'swap', same allocator
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    swap(mX, mY);

                    LOOP4_ASSERT(LINE1, LINE2, XX, X, XX == X);
                    LOOP4_ASSERT(LINE1, LINE2, YY, Y, YY == Y);
                    LOOP2_ASSERT(LINE1, LINE2, &oa == X.get_allocator());
                    LOOP2_ASSERT(LINE1, LINE2, &oa == Y.get_allocator());
                    LOOP2_ASSERT(LINE1, LINE2, oam.isTotalSame());
                }
            }

            // Record if some object memory was allocated.

            anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();
        }

        // Double check that some object memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);

        if (verbose) cout <<
                "\nInvoke free 'swap' function in a context where ADL is used."
                                                                       << endl;
        {
            // 'A' values: Should cause memory allocation if possible.

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const T1 A1(-4 * 60 * 60, true, LONG_STRING, &scratch);
            const T2 A2 = bdlt::Datetime(2010,  3, 14, 7);
            const T3 A3 = bdlt::Datetime(2010, 11,  7, 6);

                  Obj mX(&oa);  const Obj& X = mX;
            const Obj XX(X, &scratch);

                  Obj mY(A1, A2, A3, &oa);  const Obj& Y = mY;
            const Obj YY(Y, &scratch);

            if (veryVerbose) { T_ P_(X) P(Y) }

            bslma::TestAllocatorMonitor oam(&oa);

            bslalg::SwapUtil::swap(&mX, &mY);

            LOOP2_ASSERT(YY, X, YY == X);
            LOOP2_ASSERT(XX, Y, XX == Y);
            ASSERT(oam.isTotalSame());

            if (veryVerbose) { T_ P_(X) P(Y) }
        }

        // Verify no memory is allocated from the default allocator.

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

        if (verbose) cout <<
                   "\nFree 'swap' function with different allocators." << endl;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE1  =  DATA[ti].d_line;
            const T1& DESCR1 = *DATA[ti].d_descriptor_p;
            const T2& START1 = *DATA[ti].d_utcStartTime_p;
            const T3& END1   = *DATA[ti].d_utcEndTime_p;

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator     oa2("object2", veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj XX(DESCR1, START1, END1, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P(XX) }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int LINE2  =  DATA[tj].d_line;
                const T1& DESCR2 = *DATA[tj].d_descriptor_p;
                const T2& START2 = *DATA[tj].d_utcStartTime_p;
                const T3& END2   = *DATA[tj].d_utcEndTime_p;

                Obj mX(XX, &oa);                     const Obj& X = mX;
                Obj mY(DESCR2, START2, END2, &oa2);  const Obj& Y = mY;

                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                // free function 'swap', different allocators
                {
                    swap(mX, mY);

                    LOOP4_ASSERT(LINE1, LINE2, YY, X, YY == X);
                    LOOP4_ASSERT(LINE1, LINE2, XX, Y, XX == Y);
                    LOOP2_ASSERT(LINE1, LINE2, &oa  == X.get_allocator());
                    LOOP2_ASSERT(LINE1, LINE2, &oa2 == Y.get_allocator());
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) cout << "\t'swap' member function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);  Obj mB(&oa1);
                Obj mZ(&oa2);

                ASSERT_SAFE_PASS(mA.swap(mB));
                ASSERT_SAFE_FAIL(mA.swap(mZ));
            }
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor (with or without a supplied allocator)
        //:   creates an object having the same value as that of the supplied
        //:   original object.
        //:
        //: 2 If an allocator is NOT supplied to the copy constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object (i.e., the
        //:   allocator of the original object is never copied).
        //:
        //: 3 If an allocator IS supplied to the copy constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address or a default-constructed
        //:   allocator has the same effect as not supplying an allocator.
        //:
        //: 5 Supplying an allocator to the copy constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //:10 The value of the original object is unchanged.
        //:
        //:11 The allocator used by the original object is unchanged.
        //:
        //:12 QoI: Copying an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:13 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..12)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop creating three distinct objects in turn,
        //:     each using the copy constructor on 'Z' from P-2.1, but
        //:     configured differently: (a) without passing an allocator, (b)
        //:     passing a null allocator address or a default-constructed
        //:     allocato explicitly, and (c) passing the address of a test
        //:     allocator distinct from the default.
        //:
        //:   3 For each of these three iterations (P-2.2):  (C-1..12)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the copy constructor to dynamically create an object 'X',
        //:       with its object allocator configured appropriately (see
        //:       P-2.2), supplying it the 'const' object 'Z' (see P-2.1); use
        //:       a distinct test allocator for the object's footprint.  (C-9)
        //:
        //:     3 Use the equality-comparison operator to verify that:
        //:       (C-1, 5, 10)
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         that of 'Z'.  (C-1, 5)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-10)
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also use the 'allocator'
        //:       accessor of 'X' to verify that its object allocator is
        //:       properly installed, and use the 'allocator' accessor of 'Z'
        //:       to verify that the allocator that it holds is unchanged.
        //:       (C-6, 11)
        //:
        //:     5 Use the appropriate test allocators to verify that:  (C-2..4,
        //:       7..8, 12)
        //:
        //:       1 An object that IS expected to allocate memory does so
        //:         from the object allocator only (irrespective of the
        //:         specific number of allocations or the total amount of
        //:         memory allocated).  (C-2, 4)
        //:
        //:       2 An object that is expected NOT to allocate memory doesn't.
        //:         (C-12)
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         current default allocator doesn't allocate any memory.
        //:         (C-3)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-7)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-8)
        //:
        //: 3 Test again, using the data of P-1, but this time just for the
        //:   supplied allocator configuration (P-2.2c), and create the object
        //:   as an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:   (C-13)
        //
        // Testing:
        //   LocalTimePeriod(const Obj& o, Al a = {});
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        // Attribute 1 Values: 'descriptor'

        const T1 Adefault(&scratch);
        const T1 Asmall(UTC_MIN, false, "",             &scratch);
        const T1 Alarge(UTC_MAX, true,  LONGEST_STRING, &scratch);

        // Attribute 2 Values: 'utcStartTime'

        const T2 Bdefault;
        const T2 Bsmall(   1,  1,  1,  1,  0,  0, 001);
        const T2 Blarge(9999, 12, 31, 23, 59, 59, 999);

        // Attribute 3 Values: 'utcEndTime'

        const T3 Cdefault;
        const T3 Csmall(   1,  1,  1,  1,  0,  0, 001);
        const T3 Clarge(9999, 12, 31, 23, 59, 59, 999);
        const struct {
            int       d_line;  // source line number
            char      d_mem;   // expected allocation: 'Y', 'N', '?'
            const T1 *d_descriptor_p;
            const T2 *d_utcStartTime_p;
            const T3 *d_utcEndTime_p;
        } DATA[] = {
            //LINE  MEM    DESCR      START      END
            //----  ---    -----      -----      ---

            // default (must be first)
            { L_,   'N',   &Adefault, &Bdefault, &Cdefault },

            // 'descriptor'
            { L_,   'N',   &Asmall,   &Bdefault, &Cdefault },
            { L_,   'Y',   &Alarge,   &Bdefault, &Cdefault },

            // 'utcStartTime' and 'utcEndTime'
            { L_,   'N',   &Adefault, &Bsmall,   &Csmall   },
            { L_,   'N',   &Adefault, &Bsmall,   &Clarge   },
            { L_,   'N',   &Adefault, &Blarge,   &Clarge   },

            // other
            { L_,   'Y',   &Alarge,   &Bsmall,   &Csmall   },
            { L_,   'Y',   &Alarge,   &Bsmall,   &Clarge   },
            { L_,   'Y',   &Alarge,   &Blarge,   &Clarge   },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE  =  DATA[ti].d_line;
                const char MEM   =  DATA[ti].d_mem;
                const T1&  DESCR = *DATA[ti].d_descriptor_p;
                const T2&  START = *DATA[ti].d_utcStartTime_p;
                const T3&  END   = *DATA[ti].d_utcEndTime_p;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                const Obj  Z(DESCR, START, END, &scratch);
                const Obj ZZ(DESCR, START, END, &scratch);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'e'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bsl::allocator<char> saa(&sa);

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
                        objPtr = new (fa) Obj(Z, bsl::allocator<char>());
                        objAllocatorPtr = &da;
                      } break;
                      case 'd': {
                        objPtr = new (fa) Obj(Z, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      case 'e': {
                        objPtr = new (fa) Obj(Z, saa);
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
                    bslma::TestAllocator& noa = &sa == &oa ? da : sa;

                    // Ensure the first row of the table contains the
                    // default-constructed value.

                    static bool firstFlag = true;
                    if (firstFlag) {
                        LOOP4_ASSERT(LINE, CONFIG, Obj(), *objPtr,
                                     Obj() == *objPtr)
                        firstFlag = false;
                    }

                    // Verify the value of the object.

                    LOOP4_ASSERT(LINE, CONFIG,  Z, X,  Z == X);

                    // Verify that the value of 'Z' has not changed.

                    LOOP4_ASSERT(LINE, CONFIG, ZZ, Z, ZZ == Z);

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    LOOP2_ASSERT(LINE, CONFIG,
                                 &oa == X.descriptor().get_allocator());

                    // Also invoke the object's 'allocator' accessor, as well
                    // as that of 'Z'.

                    ASSERTV(LINE, CONFIG, &oa, &oa == X.get_allocator());

                    ASSERTV(LINE, CONFIG, &scratch,
                                                &scratch == Z.get_allocator());

                    // Verify no allocation from the non-object allocator.

                    LOOP3_ASSERT(LINE, CONFIG, noa.numBlocksTotal(),
                                 0 == noa.numBlocksTotal());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    LOOP4_ASSERT(LINE, CONFIG, oa.numBlocksTotal(),
                                                           oa.numBlocksInUse(),
                                 oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Verify expected ('Y'/'N') object-memory allocations.

                    if ('?' != MEM) {
                        LOOP4_ASSERT(LINE, CONFIG, MEM, oa.numBlocksInUse(),
                                   ('N' == MEM) == (0 == oa.numBlocksInUse()));
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    LOOP3_ASSERT(LINE, CONFIG, da.numBlocksInUse(),
                                 0 == da.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, fa.numBlocksInUse(),
                                 0 == fa.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, sa.numBlocksInUse(),
                                 0 == sa.numBlocksInUse());
                }  // end foreach configuration

            }  // end foreach row

            // Double check that some object memory was allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE  =  DATA[ti].d_line;
                const char MEM   =  DATA[ti].d_mem;
                const T1&  DESCR = *DATA[ti].d_descriptor_p;
                const T2&  START = *DATA[ti].d_utcStartTime_p;
                const T3&  END   = *DATA[ti].d_utcEndTime_p;

                if (veryVerbose) { T_ P_(MEM) P_(DESCR) P_(START) P(END) }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                const Obj  Z(DESCR, START, END, &scratch);
                const Obj ZZ(DESCR, START, END, &scratch);

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(Z, &sa);
                    LOOP3_ASSERT(LINE, Z, obj, Z == obj);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP3_ASSERT(LINE, ZZ, Z, ZZ == Z);

                ASSERTV(LINE, &scratch, &scratch == Z.get_allocator());
                LOOP2_ASSERT(LINE, da.numBlocksInUse(),
                             0 == da.numBlocksInUse());
                LOOP2_ASSERT(LINE, sa.numBlocksInUse(),
                             0 == sa.numBlocksInUse());
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compares
        //:   equal.
        //:
        //: 2 All salient attributes participate in the comparison.
        //:
        //: 3 No non-salient attributes (i.e., 'allocator') participate.
        //:
        //: 4 'true  == (X == X)'  (i.e., identity)
        //:
        //: 5 'false == (X != X)'  (i.e., identity)
        //:
        //: 6 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 8 'X != Y' if and only if '!(X == Y)'
        //:
        //: 9 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //:10 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //:11 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //:
        //:12 The equality operator's signature and return type are standard.
        //:
        //:13 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-9..10, 12..13)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
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
        //:
        //:   1 Create a single object, using a "scratch" allocator, and
        //:     use it to verify the reflexive (anti-reflexive) property of
        //:     equality (inequality) in the presence of aliasing.  (C-4..5)
        //:
        //:   2 For each row 'R2' in the table of P-3:  (C-1..3, 6..8)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects created
        //:       from 'R1' and 'R2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 For each of two configurations, 'a' and 'b':  (C-1..3, 6..8)
        //:
        //:       1 Create two (object) allocators, 'oax' and 'oay'.
        //:
        //:       2 Create an object 'X', using 'oax', having the value 'R1'.
        //:
        //:       3 Create an object 'Y', using 'oax' in configuration 'a' and
        //:         'oay' in configuration 'b', having the value 'R2'.
        //:
        //:       4 Verify the commutativity property and expected return value
        //:         for both '==' and '!=', while monitoring both 'oax' and
        //:         'oay' to ensure that no object memory is ever allocated by
        //:         either operator.  (C-1..3, 6..8)
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-11)
        //
        // Testing:
        //   bool operator==(const Obj& lhs, rhs);
        //   bool operator!=(const Obj& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            using namespace baltzo;
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        if (verbose) cout <<
            "\nDefine appropriate individual attribute values, 'Ai' and 'Bi'."
                                                                       << endl;

        // Attribute 1 Values: 'descriptor'

        const T1 A1(-4 * 60 * 60, true,  LONG_STRING,   &scratch);
        const T1 B1( 5 * 60 * 60, false, LONGER_STRING, &scratch);

        // Attribute 2 Values: 'utcStartTime'

        const T2 A2 = bdlt::Datetime(2010,  3, 14, 7);  // baseline value
        const T2 B2 = bdlt::Datetime(2010,  3, 14, 8);

        // Attribute 3 Values: 'utcEndTime'

        const T3 A3 = bdlt::Datetime(2010, 11,  7, 6);  // baseline value
        const T3 B3 = bdlt::Datetime(2010, 11,  7, 7);

        if (verbose) cout <<
            "\nCreate a table of distinct, but similar object values." << endl;

        static const struct {
            int       d_line;           // source line number
            const T1 *d_descriptor_p;
            const T2 *d_utcStartTime_p;
            const T3 *d_utcEndTime_p;
        } DATA[] = {

        // The first row of the table below represents an object value
        // consisting of "baseline" attribute values (A1..An).  Each subsequent
        // row differs (slightly) from the first in exactly one attribute
        // value (Bi).

        //LINE  DESCR  START  END
        //----  -----  -----  ---

        { L_,     &A1,   &A2, &A3 },          // baseline

        { L_,     &B1,   &A2, &A3 },
        { L_,     &A1,   &B2, &A3 },
        { L_,     &A1,   &A2, &B3 },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE1  = DATA[ti].d_line;
            const T1  *DESCR1 = DATA[ti].d_descriptor_p;
            const T2  *START1 = DATA[ti].d_utcStartTime_p;
            const T3  *END1   = DATA[ti].d_utcEndTime_p;

            if (veryVerbose) { T_ P_(LINE1) P_(*DESCR1) P_(*START1) P(*END1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const Obj X(*DESCR1, *START1, *END1, &scratch);

                LOOP2_ASSERT(LINE1, X,   X == X);
                LOOP2_ASSERT(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int  LINE2  = DATA[tj].d_line;
                const T1  *DESCR2 = DATA[tj].d_descriptor_p;
                const T2  *START2 = DATA[tj].d_utcStartTime_p;
                const T3  *END2   = DATA[tj].d_utcEndTime_p;

                if (veryVerbose) {
                               T_ T_ P_(LINE2)
                                     P_(*DESCR2)
                                     P_(*START2)
                                     P(*END2) }

                const bool EXP = ti == tj;  // expected for equality comparison

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create two distinct test allocators, 'oax' and 'oay'.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    const Obj X(*DESCR1, *START1, *END1, &xa);
                    const Obj Y(*DESCR2, *START2, *END2, &ya);

                    if (veryVerbose) {
                        T_ T_ T_ P_(EXP) P_(CONFIG) P_(X) P(Y) }

                    // Verify value, commutativity, and no memory allocation.

                    bslma::TestAllocatorMonitor oaxm(&oax), oaym(&oay);

                    LOOP5_ASSERT(LINE1, LINE2, CONFIG, X, Y,  EXP == (X == Y));
                    LOOP5_ASSERT(LINE1, LINE2, CONFIG, Y, X,  EXP == (Y == X));

                    LOOP5_ASSERT(LINE1, LINE2, CONFIG, X, Y, !EXP == (X != Y));
                    LOOP5_ASSERT(LINE1, LINE2, CONFIG, Y, X, !EXP == (Y != X));

                    LOOP3_ASSERT(LINE1, LINE2, CONFIG, oaxm.isTotalSame());
                    LOOP3_ASSERT(LINE1, LINE2, CONFIG, oaym.isTotalSame());

                    // Double check that some object memory was allocated.

                    LOOP3_ASSERT(LINE1, LINE2, CONFIG,
                                 1 <= xa.numBlocksInUse());
                    LOOP3_ASSERT(LINE1, LINE2, CONFIG,
                                 1 <= ya.numBlocksInUse());

                    // Note that memory should be independently allocated for
                    // each attribute capable of allocating memory.
                }
            }

            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)',
        //:   but with each "attributeName = " elided.
        //:
        //: 4 The 'print' method signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The output 'operator<<' signature and return type are standard.
        //:
        //: 7 The output 'operator<<' returns the supplied 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5, 7)
        //:
        //:   1 Define twelve carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the
        //:     two formatting parameters, along with the expected output
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1 }  -->  3 expected outputs
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2 }  -->  6 expected outputs
        //:     3 { B   } x {  2     } x {  3        }  -->  1 expected output
        //:     4 { A B } x { -9     } x { -9        }  -->  2 expected output
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5, 7)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', unless the parameters are,
        //:       arbitrarily, (-9, -9), in which case 'operator<<' will be
        //:       invoked instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5, 7)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   operator<<(ostream& s, const Obj& d);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            using namespace baltzo;
            typedef bsl::ostream ostream;

            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        const Obj A(Desc(-7, true,  "MDT"),
                    bdlt::Datetime(2010,  4, 15,  8),
                    bdlt::Datetime(2010, 12,  8,  9));

        const Obj B(Desc( 2, false, "IDT"),
                    bdlt::Datetime(2011,  5, 16,  9),
                    bdlt::Datetime(2012,  1,  9, 10));

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;
            const Obj  *d_object_p;
            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1 }  -->  3 expected outputs
        // ------------------------------------------------------------------

        //LINE L SPL  OBJ  EXPECTED
        //---- - ---  ---  ---------------------------------------------------

        { L_,  0,  0,  &A, "["                                               NL
                           "descriptor = ["                                  NL
                           "utcOffsetInSeconds = -7"                         NL
                           "dstInEffectFlag = true"                          NL
                           "description = \"MDT\""                           NL
                           "]"                                               NL
                           "utcStartTime = 15APR2010_08:00:00.000000"        NL
                           "utcEndTime = 08DEC2010_09:00:00.000000"          NL
                           "]"                                               NL
                                                                            },

        { L_,  0,  1,  &A, "["                                               NL
                           " descriptor = ["                                 NL
                           "  utcOffsetInSeconds = -7"                       NL
                           "  dstInEffectFlag = true"                        NL
                           "  description = \"MDT\""                         NL
                           " ]"                                              NL
                           " utcStartTime = 15APR2010_08:00:00.000000"       NL
                           " utcEndTime = 08DEC2010_09:00:00.000000"         NL
                           "]"                                               NL
                                                                            },

        { L_,  0, -1,  &A, "["                                               SP
                           "descriptor = ["                                  SP
                           "utcOffsetInSeconds = -7"                         SP
                           "dstInEffectFlag = true"                          SP
                           "description = \"MDT\""                           SP
                           "]"                                               SP
                           "utcStartTime = 15APR2010_08:00:00.000000"        SP
                           "utcEndTime = 08DEC2010_09:00:00.000000"          SP
                           "]"
                                                                            },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2 }  -->  6 expected outputs
        // ------------------------------------------------------------------

        //LINE L SPL  OBJ  EXPECTED
        //---- - ---  ---  ---------------------------------------------------

        { L_,  3,  0,  &A, "["                                               NL
                           "descriptor = ["                                  NL
                           "utcOffsetInSeconds = -7"                         NL
                           "dstInEffectFlag = true"                          NL
                           "description = \"MDT\""                           NL
                           "]"                                               NL
                           "utcStartTime = 15APR2010_08:00:00.000000"        NL
                           "utcEndTime = 08DEC2010_09:00:00.000000"          NL
                           "]"                                               NL
                                                                            },

        { L_,  3,  2,  &A,
                          "      ["                                          NL
                          "        descriptor = ["                           NL
                          "          utcOffsetInSeconds = -7"                NL
                          "          dstInEffectFlag = true"                 NL
                          "          description = \"MDT\""                  NL
                          "        ]"                                        NL
                          "        utcStartTime = 15APR2010_08:00:00.000000" NL
                          "        utcEndTime = 08DEC2010_09:00:00.000000"   NL
                          "      ]"                                          NL
                                                                            },

        { L_,  3, -2,  &A, "      ["                                         SP
                           "descriptor = ["                                  SP
                           "utcOffsetInSeconds = -7"                         SP
                           "dstInEffectFlag = true"                          SP
                           "description = \"MDT\""                           SP
                           "]"                                               SP
                           "utcStartTime = 15APR2010_08:00:00.000000"        SP
                           "utcEndTime = 08DEC2010_09:00:00.000000"          SP
                           "]"
                                                                            },

        { L_, -3,  0,  &A, "["                                               NL
                           "descriptor = ["                                  NL
                           "utcOffsetInSeconds = -7"                         NL
                           "dstInEffectFlag = true"                          NL
                           "description = \"MDT\""                           NL
                           "]"                                               NL
                           "utcStartTime = 15APR2010_08:00:00.000000"        NL
                           "utcEndTime = 08DEC2010_09:00:00.000000"          NL
                           "]"                                               NL
                                                                            },

        { L_, -3,  2,  &A,
                          "["                                                NL
                          "        descriptor = ["                           NL
                          "          utcOffsetInSeconds = -7"                NL
                          "          dstInEffectFlag = true"                 NL
                          "          description = \"MDT\""                  NL
                          "        ]"                                        NL
                          "        utcStartTime = 15APR2010_08:00:00.000000" NL
                          "        utcEndTime = 08DEC2010_09:00:00.000000"   NL
                          "      ]"                                          NL
                                                                            },

        { L_, -3, -2,  &A, "["                                               SP
                           "descriptor ="                                    SP
                           "["                                               SP
                           "utcOffsetInSeconds = -7"                         SP
                           "dstInEffectFlag = true"                          SP
                           "description = \"MDT\""                           SP
                           "]"                                               SP
                           "utcStartTime = 15APR2010_08:00:00.000000"        SP
                           "utcEndTime = 08DEC2010_09:00:00.000000"          SP
                           "]"
                                                                            },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }         -->  1 expected output
        // -----------------------------------------------------------------

        //LINE L SPL  OBJ  EXPECTED
        //---- - ---  ---  ---------------------------------------------------
        { L_,  2,  3,  &B,
                         "      ["                                           NL
                         "         descriptor = ["                           NL
                         "            utcOffsetInSeconds = 2"                NL
                         "            dstInEffectFlag = false"               NL
                         "            description = \"IDT\""                 NL
                         "         ]"                                        NL
                         "         utcStartTime = 16MAY2011_09:00:00.000000" NL
                         "         utcEndTime = 09JAN2012_10:00:00.000000"   NL
                         "      ]"                                           NL
                                                                            },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -9 }   x { -9 }      -->  2 expected outputs
        // -----------------------------------------------------------------

        //LINE L SPL  OBJ  EXPECTED
        //---- - ---  ---  ---------------------------------------------------

        { L_, -9, -9,  &A, "["                                              SP
                           "descriptor ="                                   SP
                           "["                                              SP
                           "utcOffsetInSeconds = -7"                        SP
                           "dstInEffectFlag = true"                         SP
                           "description = \"MDT\""                          SP
                           "]"                                              SP
                           "utcStartTime = 15APR2010_08:00:00.000000"       SP
                           "utcEndTime = 08DEC2010_09:00:00.000000"         SP
                           "]"
                                                                            },

        { L_, -9, -9,  &B, "["                                              SP
                           "descriptor ="                                   SP
                           "["                                              SP
                           "utcOffsetInSeconds = 2"                         SP
                           "dstInEffectFlag = false"                        SP
                           "description = \"IDT\""                          SP
                           "]"                                              SP
                           "utcStartTime = 16MAY2011_09:00:00.000000"       SP
                           "utcEndTime = 09JAN2012_10:00:00.000000"         SP
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
                const int   LINE = DATA[ti].d_line;
                const int   L    = DATA[ti].d_level;
                const int   SPL  = DATA[ti].d_spacesPerLevel;
                const Obj  *OBJ  = DATA[ti].d_object_p;
                const char *EXP  = DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(L) P_(SPL) P(*OBJ) }

                if (veryVeryVerbose) { T_ T_ Q(EXP) cout << EXP; }

                bsl::ostringstream os;

                if (-9 == L && -9 == SPL) {

                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &os == &(os << *OBJ));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {

                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &os == &(*OBJ).print(os, L, SPL));

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
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        //   In case 3 we demonstrated that all basic accessors work properly
        //   with respect to attributes initialized by the value constructor.
        //   Here we use the default constructor and primary manipulators,
        //   which were fully tested in case 2, to further corroborate that
        //   these accessors are properly interpreting object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute
        //:   of the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 No accessor allocates any memory.
        //:
        //: 4 Accessors for attributes that can allocate memory (i.e., those
        //:   that take an allocator in their constructor) return a reference
        //:   providing only non-modifiable access.
        //
        // Plan:
        //: 1 Create two 'bslma::TestAllocator' objects, and install one as
        //:   the current default allocator (note that a ubiquitous test
        //:   allocator is already installed as the global allocator).
        //:
        //: 2 Use the default constructor, using the other test allocator
        //:   from P-1, to create an object (having default attribute values).
        //:
        //: 3 Verify that each basic accessor, invoked on a reference providing
        //:   non-modifiable access to the object created in P2, returns the
        //:   expected value.  (C-2)
        //:
        //: 4 For each salient attribute (contributing to value):  (C-1, 3..4)
        //:   1 Use the corresponding primary manipulator to set the attribute
        //:     to a unique value, making sure to allocate memory if possible.
        //:     o In this class there is only one manipulator,
        //:       'setUtcStartAndEndTime', to set two attributes,
        //:       'utcStartTime' and 'utcEndTime', only one local block is
        //:       needed.  The two attributes set are chosen to be different;
        //:       otherwise, the tests for setting attributes independently
        //:       would be invalid.
        //:
        //:   2 Use the corresponding basic accessor to verify the new
        //:     expected value.  (C-1)
        //:
        //:   3 Monitor the memory allocated from both the default and object
        //:     allocators before and after calling the accessor; verify that
        //:     there is no change in total memory allocation.  (C-3..4)
        //
        // Testing:
        //   bslma::Allocator *allocator() const;
        //   bsl::allocator<char> get_allocator() const;
        //   const Obj& descriptor() const;
        //   const bdlt::Datetime& utcStartTime() const;
        //   const bdlt::Datetime& utcEndTime() const;
        //   CONCERN: All accessor methods are declared 'const'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // -----------------------------------------------------
        // 'D' values: These are the default-constructed values.
        // -----------------------------------------------------

        const T1 D1 = Desc();              // 'descriptor'
        const T2 D2 = bdlt::Datetime();    // 'utcStartTime'
        const T3 D3 = bdlt::Datetime();    // 'utcEndTime'

        // -------------------------------------------------------
        // 'A' values: Should cause memory allocation if possible.
        // -------------------------------------------------------

        const T1 A1 = Desc(UTC_MIN, true, "a_" SUFFICIENTLY_LONG_STRING);
        const T2 A2 = bdlt::Datetime(2010,  4, 15,  8);
        const T3 A3 = bdlt::Datetime(2010, 12,  8,  9);

        if (verbose) cout <<
           "\nCreate two test allocators; install one as the default." << endl;

        if (verbose) cout <<
                 "\nCreate an object, passing in the other allocator." << endl;


        enum { e_NO_ALLOC, e_BSL_ALLOC, e_BSLMA_ALLOC };

        for (int ti = 0; ti <= e_BSLMA_ALLOC; ++ti) {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

            bsl::allocator<char> daa(&da);
            bsl::allocator<char> oaa(&oa);

            bslma::DefaultAllocatorGuard dag(&da);

            bsls::ObjectBuffer<Obj> footprint;

            switch (ti) {
              case e_NO_ALLOC: {
                new (footprint.address()) Obj();
              } break;
              case e_BSL_ALLOC: {
                new (footprint.address()) Obj(oaa);
              } break;
              case e_BSLMA_ALLOC: {
                new (footprint.address()) Obj(&oa);
              } break;
            }

            Obj& mX = footprint.object();  const Obj& X = mX;

            if (veryVerbose) cout <<
                      "\nVerify all basic accessors report expected values.\n";
            {
                const T1& descriptor = X.descriptor();
                LOOP2_ASSERT(D1, descriptor, D1 == descriptor);

                const T2& utcStartTime = X.utcStartTime();
                LOOP2_ASSERT(D2, utcStartTime, D2 == utcStartTime);

                const T3& utcEndTime = X.utcEndTime();
                LOOP2_ASSERT(D3, utcEndTime, D3 == utcEndTime);

                ASSERT((e_NO_ALLOC == ti ? daa : oaa) == X.get_allocator());
                ASSERT((e_NO_ALLOC == ti ? &da : &oa) == X.get_allocator());
            }

            if (veryVerbose) cout <<
                  "\nApply primary manipulators and verify expected values.\n";

            if (veryVerbose) { T_ Q(descriptor) }
            {
                mX.setDescriptor(A1);

                bslma::TestAllocatorMonitor oam(&oa), dam(&da);

                const T1& descriptor = X.descriptor();
                LOOP2_ASSERT(A1, descriptor, A1 == descriptor);

                ASSERT(oam.isInUseSame());  ASSERT(dam.isInUseSame());
            }

            if (veryVerbose) { T_ Q(utcStartTime and utcEndTime) }
            {
                ASSERT(A2 != A3);
                mX.setUtcStartAndEndTime(A2, A3);

                bslma::TestAllocatorMonitor oam(&oa), dam(&da);

                const T2& utcStartTime = X.utcStartTime();
                LOOP2_ASSERT(A2, utcStartTime, A2 == utcStartTime);

                const T3& utcEndTime = X.utcEndTime();
                LOOP2_ASSERT(A3, utcEndTime, A3 == utcEndTime);

                ASSERT(oam.isInUseSame());  ASSERT(dam.isInUseSame());
            }

            // Double check that some object memory was allocated.

            ASSERT(1 <= (e_NO_ALLOC == ti ? da.numBlocksTotal()
                                          : oa.numBlocksTotal()));

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.

            if (e_NO_ALLOC != ti) {
                    LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
            }

            mX.~Obj();
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE CTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The value constructor (with or without a supplied allocator) can
        //:   create an object having any value that does not violate the
        //:   constructor's documented preconditions.
        //:
        //: 2 Any string arguments can be of type 'char *' or 'string'.
        //:
        //: 3 Any argument can be 'const'.
        //:
        //: 4 If an allocator is NOT supplied to the value constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 5 If an allocator IS supplied to the value constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 6 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 7 Supplying an allocator to the value constructor has no effect
        //:   on subsequent object values.
        //:
        //: 8 Any memory allocation is from the object allocator.
        //:
        //: 9 There is no temporary memory allocation from any allocator.
        //:
        //:10 Every object releases any allocated memory at destruction.
        //:
        //:11 QoI: Creating an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:12 Any memory allocation is exception neutral.
        //:
        //:13 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1, 3..11)
        //:
        //:   1 Execute an inner loop creating three distinct objects, in turn,
        //:     each object having the same value, 'V', but configured
        //:     differently: (a) without passing an allocator, (b) passing a
        //:     null allocator address or a default constructed allocator
        //:     explicitly, and (c) passing the address of a test allocator
        //:     distinct from the default allocator.
        //:
        //:   2 For each of the three iterations in P-2.1:  (C-1, 4..11)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the value constructor to dynamically create an object
        //:       having the value 'V', with its object allocator configured
        //:       appropriately (see P-2.1), supplying all the arguments as
        //:       'const' and representing any string arguments as 'char *';
        //:       use a distinct test allocator for the object's footprint.
        //:
        //:     3 Use the (as yet unproven) salient attribute accessors to
        //:       verify that all of the attributes of each object have their
        //:       expected values.  (C-1, 7)
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also invoke the (as yet
        //:       unproven) 'allocator' accessor of the object under test.
        //:       (C-8)
        //:
        //:     5 Use the appropriate test allocators to verify that:  (C-4..6,
        //:       9..11)
        //:
        //:       1 An object that IS expected to allocate memory does so
        //:         from the object allocator only (irrespective of the
        //:         specific number of allocations or the total amount of
        //:         memory allocated).  (C-4, 6)
        //:
        //:       2 An object that is expected NOT to allocate memory doesn't.
        //:         (C-11)
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         default allocator doesn't allocate any memory.  (C-5)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-9)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-10)
        //:
        //: 3 Repeat the steps in P-2 for the supplied allocator configuration
        //:   (P-2.1c) on the data of P-1, but this time create the object as
        //:   an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros);
        //:   represent any string arguments in terms of 'string' using a
        //:   "scratch" allocator.  (C-2, 12)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-13)
        //
        // Testing:
        //   LocalTimePeriod(const LTD& d, const Dt& s, e, Al a = {});
        //   CONCERN: String arguments can be either 'char *' or 'string'.
        //   CONCERN: All creator/manipulator ref params are 'const' or '&&'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CTOR" << endl
                          << "==========" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const struct {
            int  d_line;  // source line number
            char d_mem;   // expected allocation: 'Y', 'N', '?'
            T1   d_descriptor;
            T2   d_utcStartTime;
            T3   d_utcEndTime;
        } DATA[] = {
            //LINE  MEM    DESCR
            //----  ---    -----
            //             START
            //             -----
            //             END
            //             ---

            // default (must be first)
            { L_,   'N',   Desc(),
                           bdlt::Datetime(),
                           bdlt::Datetime()
            },

            // 'descriptor'
            { L_,   'N',   Desc(UTC_MIN, false, ""),
                           bdlt::Datetime(),
                           bdlt::Datetime()
            },
            { L_,   'Y',   Desc(UTC_MAX, true, LONGEST_STRING),
                           bdlt::Datetime(),
                           bdlt::Datetime()
            },

            // 'utcStartTime' and 'utcEndTime'
            { L_,   'N',   Desc(),
                           bdlt::Datetime(   1,  1,  1,  1,  0,  0, 001),
                           bdlt::Datetime(   1,  1,  1,  1,  0,  0, 001)
            },
            { L_,   'N',   Desc(),
                           bdlt::Datetime(   1,  1,  1,  1,  0,  0, 001),
                           bdlt::Datetime(9999, 12, 31, 23, 59, 59, 999)
            },
            { L_,   'N',   Desc(),
                           bdlt::Datetime(9999, 12, 31, 23, 59, 59, 999),
                           bdlt::Datetime(9999, 12, 31, 23, 59, 59, 999)
            },

            // other
            { L_,   'Y',   Desc(UTC_MAX, true, LONGEST_STRING),
                           bdlt::Datetime(   1,  1,  1,  1,  0,  0, 001),
                           bdlt::Datetime(   1,  1,  1,  1,  0,  0, 001)
            },
            { L_,   'Y',   Desc(UTC_MAX, true, LONGEST_STRING),
                           bdlt::Datetime(   1,  1,  1,  1,  0,  0, 001),
                           bdlt::Datetime(9999, 12, 31, 23, 59, 59, 999)
            },
            { L_,   'Y',   Desc(UTC_MAX, true, LONGEST_STRING),
                           bdlt::Datetime(9999, 12, 31, 23, 59, 59, 999),
                           bdlt::Datetime(9999, 12, 31, 23, 59, 59, 999)
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE  = DATA[ti].d_line;
                const char MEM   = DATA[ti].d_mem;
                const T1&  DESCR = DATA[ti].d_descriptor;
                const T2&  START = DATA[ti].d_utcStartTime;
                const T3&  END   = DATA[ti].d_utcEndTime;

                if (veryVerbose) { T_ P_(MEM) P_(DESCR) P_(START) P(END) }

                LOOP2_ASSERT(LINE, MEM, MEM && strchr("YN?", MEM));

                for (char cfg = 'a'; cfg <= 'd'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    if (veryVerbose) { T_ T_ P(CONFIG) }

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr;
                    bslma::TestAllocator *objAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        objPtr = new (fa) Obj(DESCR, START, END);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj(DESCR, START, END, 0);
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(DESCR,
                                              START,
                                              END,
                                              bsl::allocator<char>());
                        objAllocatorPtr = &da;
                      } break;
                      case 'd': {
                        objPtr = new (fa) Obj(DESCR, START, END, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        LOOP2_ASSERT(LINE, CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    LOOP2_ASSERT(LINE, CONFIG,
                                 sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'd' != CONFIG ? sa : da;

                    // Use untested functionality to help ensure the first row
                    // of the table contains the default-constructed value.

                    static bool firstFlag = true;
                    if (firstFlag) {
                        LOOP4_ASSERT(LINE, CONFIG, Obj(), *objPtr,
                                     Obj() == *objPtr)
                        firstFlag = false;
                    }

                    // -------------------------------------
                    // Verify the object's attribute values.
                    // -------------------------------------

                    LOOP4_ASSERT(LINE, CONFIG, DESCR, X.descriptor(),
                                 DESCR == X.descriptor());

                    LOOP4_ASSERT(LINE, CONFIG, START, X.utcStartTime(),
                                 START == X.utcStartTime());

                    LOOP4_ASSERT(LINE, CONFIG, END, X.utcEndTime(),
                                 END == X.utcEndTime());

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    LOOP2_ASSERT(LINE, CONFIG,
                                 &oa == X.descriptor().get_allocator());

                    // Also invoke the object's 'get_allocator' accessor.

                    ASSERTV(LINE, CONFIG, &oa, &oa == X.get_allocator());

                    // Verify no allocation from the non-object allocator.

                    LOOP3_ASSERT(LINE, CONFIG, noa.numBlocksTotal(),
                                 0 == noa.numBlocksTotal());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    LOOP4_ASSERT(LINE, CONFIG, oa.numBlocksTotal(),
                                                           oa.numBlocksInUse(),
                                 oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Verify expected ('Y'/'N') object-memory allocations.

                    if ('?' != MEM) {
                        LOOP4_ASSERT(LINE, CONFIG, MEM, oa.numBlocksInUse(),
                                   ('N' == MEM) == (0 == oa.numBlocksInUse()));
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    LOOP3_ASSERT(LINE, CONFIG, da.numBlocksInUse(),
                                 0 == da.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, fa.numBlocksInUse(),
                                 0 == fa.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, sa.numBlocksInUse(),
                                 0 == sa.numBlocksInUse());

                }  // end foreach configuration
            }  // end foreach row

            // Double check that some object memory was allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            // Note that any string arguments are now of type 'string', which
            // require their own "scratch" allocator.

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int                         LINE  = DATA[ti].d_line;
                const char                        MEM   = DATA[ti].d_mem;
                const Desc&                       DESCR =
                                                         DATA[ti].d_descriptor;
                const bdlt::Datetime&             START =
                                                       DATA[ti].d_utcStartTime;
                const bdlt::Datetime&             END   =
                                                         DATA[ti].d_utcEndTime;

                if (veryVerbose) { T_ P_(MEM) P_(DESCR) P_(START) P(END) }

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(DESCR, START, END, &sa);
                    LOOP3_ASSERT(LINE, DESCR, obj.descriptor(),
                                 DESCR == obj.descriptor());
                    LOOP3_ASSERT(LINE, START, obj.utcStartTime(),
                                 START == obj.utcStartTime());
                    LOOP3_ASSERT(LINE, END, obj.utcEndTime(),
                                 END == obj.utcEndTime());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP2_ASSERT(LINE, da.numBlocksInUse(),
                             0 == da.numBlocksInUse());
                LOOP2_ASSERT(LINE, sa.numBlocksInUse(),
                             0 == sa.numBlocksInUse());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) cout << "\t'utcStartTime' and 'utcEndTime'"
                                  << endl;

            const Desc            defaultLTD;
            const bdlt::Datetime  defaultDt;
            const bdlt::Datetime  someDt( 2010,  4, 15,  8);
            const bdlt::Datetime  earlyDt(2011,  6, 16,  9);
            const bdlt::Datetime  laterDt(2012,  1, 10, 11);

            ASSERT_SAFE_PASS(Obj(defaultLTD, defaultDt, defaultDt));
            ASSERT_SAFE_FAIL(Obj(defaultLTD, defaultDt,    someDt));
            ASSERT_SAFE_FAIL(Obj(defaultLTD,    someDt, defaultDt));
            ASSERT_SAFE_PASS(Obj(defaultLTD,   earlyDt,   laterDt));
            ASSERT_SAFE_FAIL(Obj(defaultLTD,   laterDt,   earlyDt));
            ASSERT_SAFE_PASS(Obj(defaultLTD,    someDt,    someDt));
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value), use the primary
        //   manipulators to put that object into any state relevant for
        //   thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor (with or without
        //:   a supplied allocator) has the contractually specified default
        //:   value.
        //:
        //: 2 If an allocator is NOT supplied to the default constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 3 If an allocator IS supplied to the default constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the default constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 QoI: The default constructor allocates no memory.
        //:
        //:10 Each attribute is modifiable independently.
        //:
        //:11 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //:
        //:12 Any string arguments can be of type 'char *' or 'string'.
        //:
        //:13 Any argument can be 'const'.
        //:
        //:14 Any memory allocation is exception neutral.
        //:
        //:15 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create three sets of attribute values for the object: ('D')
        //:   values corresponding to the default-constructed object, ('A')
        //:   values that allocate memory if possible, and ('B') other values
        //:   that do not cause additional memory allocation beyond that which
        //:   may be incurred by 'A'.  Both the 'A' and 'B' attribute values
        //:   should be chosen to be boundary values where possible.  If an
        //:   attribute can be supplied via alternate C++ types (e.g., 'string'
        //:   instead of 'char *'), use the alternate type for 'B'.
        //:
        //: 2 Using a loop-based approach, default-construct three distinct
        //:   objects, in turn, but configured differently: (a) without passing
        //:   an allocator, (b) passing a null allocator address or
        //:   default-constructed allocator explicitly, and (c) passing the
        //:   address of a test allocator distinct from the default.  For each
        //:   of these three iterations: (C-1..14)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     as the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object
        //:     'X', with its object allocator configured appropriately (see
        //:     P-2); use a distinct test allocator for the object's footprint.
        //:
        //:   3 Use the 'allocator' accessor of each underlying attribute
        //:     capable of allocating memory to ensure that its object
        //:     allocator is properly installed; also invoke the (as yet
        //:     unproven) 'allocator' accessor of the object under test.
        //:     (C-2..4)
        //:
        //:   4 Use the appropriate test allocators to verify that no memory
        //:     is allocated by the default constructor.  (C-9)
        //:
        //:   5 Use the individual (as yet unproven) salient attribute
        //:     accessors to verify the default-constructed value.  (C-1)
        //:
        //:   6 For each attribute 'i', in turn, create a local block.  Then
        //:     inside the block, using brute force, set that attribute's
        //:     value, passing a 'const' argument representing each of the
        //:     three test values, in turn (see P-1), first to 'Ai', then to
        //:     'Bi', and finally back to 'Di'.  If attribute 'i' can allocate
        //:     memory, verify that it does so on the first value transition
        //:     ('Di' -> 'Ai'), and that the corresponding primary manipulator
        //:     is exception neutral (using the
        //:     'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  In all other
        //:     cases, verify that no memory allocation occurs.  After each
        //:     transition, use the (as yet unproven) basic accessors to verify
        //:     that only the intended attribute value changed.  (C-5..6,
        //:     11..14)
        //:     o In this class there is only one manipulator,
        //:       'setUtcStartAndEndTime', to set two attributes,
        //:       'utcStartTime' and 'utcEndTime', only one local block is
        //:       needed.  The two attributes set are chosen to be different;
        //:       otherwise, the tests for setting attributes independently
        //:       would be invalid.
        //:
        //:   7 Corroborate that attributes are modifiable independently by
        //:     first setting all of the attributes to their 'A' values, then
        //:     setting all of the attributes to their 'B' values.  (C-10)
        //:
        //:   8 Verify that no temporary memory is allocated from the object
        //:     allocator.  (C-7)
        //:
        //:   9 Verify that all object memory is released when the object is
        //:     destroyed.  (C-8)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-15)
        //
        // Testing:
        //   LocalTimePeriod(Al a = {});
        //   ~LocalTimePeriod();
        //   void setDescriptor(const Desc& value);
        //   void setUtcStartAndEndTime(const bdlt::Datetime& start, end);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const T1 D1 = Desc();  // 'descriptor'
        const T2 D2 = bdlt::Datetime();               // 'utcStartTime'
        const T3 D3 = bdlt::Datetime();               // 'utcEndTime'

        // 'A' values: Should cause memory allocation if possible.

        const T1 A1 = Desc(UTC_MIN, true, "a_" SUFFICIENTLY_LONG_STRING);
        const T2 A2 = bdlt::Datetime(2010,  4, 15,  8);
        const T3 A3 = bdlt::Datetime(2010, 12,  8,  9);

        // 'B' values: Should NOT cause allocation (use alternate string type).

        const T1 B1 = Desc(UTC_MAX, false, "ETD");
        const T2 B2 = bdlt::Datetime(2011,  6, 16,  9);
        const T3 B3 = bdlt::Datetime(2012,  1, 10, 11);

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;

        for (char cfg = 'a'; cfg <= 'd'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr;
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
                objPtr = new (fa) Obj(bsl::allocator<char>());
                objAllocatorPtr = &da;
              } break;
              case 'd': {
                objPtr = new (fa) Obj(&sa);
                objAllocatorPtr = &sa;
              } break;
              default: {
                LOOP_ASSERT(CONFIG, !"Bad allocator config.");
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'd' != CONFIG ? sa : da;

            // -------------------------------------------------------
            // Verify any attribute allocators are installed properly.
            // -------------------------------------------------------

            LOOP_ASSERT(CONFIG, &oa == X.descriptor().get_allocator());

            // Also invoke the object's 'gt_allocator' accessor.

            LOOP3_ASSERT(CONFIG, &oa, X.allocator(), &oa == X.get_allocator());

            // Verify no allocation from the object/non-object allocators.

            LOOP2_ASSERT(CONFIG,  oa.numBlocksTotal(),
                         0 ==  oa.numBlocksTotal());
            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());

            // -------------------------------------
            // Verify the object's attribute values.
            // -------------------------------------

            LOOP3_ASSERT(CONFIG, D1, X.descriptor(),
                         D1 == X.descriptor());
            LOOP3_ASSERT(CONFIG, D2, X.utcStartTime(),
                         D2 == X.utcStartTime());
            LOOP3_ASSERT(CONFIG, D3, X.utcEndTime(),
                         D3 == X.utcEndTime());

            // -----------------------------------------------------
            // Verify that each attribute is independently settable.
            // -----------------------------------------------------

            // 'descriptor'
            {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    bslma::TestAllocatorMonitor tam(&oa);
                    mX.setDescriptor(A1);
                    LOOP_ASSERT(CONFIG, tam.isInUseUp());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                bslma::TestAllocatorMonitor tam(&oa);

                LOOP_ASSERT(CONFIG, A1 == X.descriptor());
                LOOP_ASSERT(CONFIG, D2 == X.utcStartTime());
                LOOP_ASSERT(CONFIG, D3 == X.utcEndTime());

                mX.setDescriptor(B1);
                LOOP_ASSERT(CONFIG, B1 == X.descriptor());
                LOOP_ASSERT(CONFIG, D2 == X.utcStartTime());
                LOOP_ASSERT(CONFIG, D3 == X.utcEndTime());

                mX.setDescriptor(D1);
                LOOP_ASSERT(CONFIG, D1 == X.descriptor());
                LOOP_ASSERT(CONFIG, D2 == X.utcStartTime());
                LOOP_ASSERT(CONFIG, D3 == X.utcEndTime());

                LOOP_ASSERT(CONFIG, tam.isTotalSame());
            }

            // 'utcStartTime' and 'utcEndTime'
            {
                bslma::TestAllocatorMonitor tam(&oa);

                ASSERT(A2 != A3);
                mX.setUtcStartAndEndTime(A2, A3);
                LOOP_ASSERT(CONFIG, D1 == X.descriptor());
                LOOP_ASSERT(CONFIG, A2 == X.utcStartTime());
                LOOP_ASSERT(CONFIG, A3 == X.utcEndTime());

                ASSERT(B2 != B3);
                mX.setUtcStartAndEndTime(B2, B3);
                LOOP_ASSERT(CONFIG, D1 == X.descriptor());
                LOOP_ASSERT(CONFIG, B2 == X.utcStartTime());
                LOOP_ASSERT(CONFIG, B3 == X.utcEndTime());

                LOOP_ASSERT(CONFIG, tam.isTotalSame());
            }

            // Corroborate attribute independence.
            {
                bslma::TestAllocatorMonitor tam(&oa);

                // Set all attributes to their 'A' values.

                mX.setDescriptor(A1);
                ASSERT(A2 != A3);
                mX.setUtcStartAndEndTime(A2, A3);

                LOOP_ASSERT(CONFIG, A1 == X.descriptor());
                LOOP_ASSERT(CONFIG, A2 == X.utcStartTime());
                LOOP_ASSERT(CONFIG, A3 == X.utcEndTime());

                // Set all attributes to their 'B' values.

                mX.setDescriptor(B1);
                ASSERT(B2 != B3);
                mX.setUtcStartAndEndTime(B2, B3);

                LOOP_ASSERT(CONFIG, B1 == X.descriptor());
                LOOP_ASSERT(CONFIG, B2 == X.utcStartTime());
                LOOP_ASSERT(CONFIG, B3 == X.utcEndTime());

                LOOP_ASSERT(CONFIG, tam.isTotalSame());
            }

            // Verify no temporary memory is allocated from the object
            // allocator.

            LOOP3_ASSERT(CONFIG, oa.numBlocksTotal(), oa.numBlocksInUse(),
                         oa.numBlocksTotal() == oa.numBlocksInUse());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            LOOP_ASSERT(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
            LOOP_ASSERT(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
            LOOP_ASSERT(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Double check that some object memory was allocated.

            LOOP_ASSERT(CONFIG, 1 <= oa.numBlocksTotal());

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj obj;

            if (veryVerbose) cout << "\tutcOffsetInSeconds" << endl;
            {
                const bdlt::Datetime defaultDatetime;
                const bdlt::Datetime  someDatetime(2010,  4, 15,  8);
                const bdlt::Datetime earlyDatetime(2011,  6, 16,  9);
                const bdlt::Datetime laterDatetime(2012,  1, 10, 11);

                ASSERT_SAFE_PASS(obj.setUtcStartAndEndTime(defaultDatetime,
                                                           defaultDatetime));
                ASSERT_SAFE_FAIL(obj.setUtcStartAndEndTime(defaultDatetime,
                                                              someDatetime));
                ASSERT_SAFE_FAIL(obj.setUtcStartAndEndTime(   someDatetime,
                                                           defaultDatetime));
                ASSERT_SAFE_PASS(obj.setUtcStartAndEndTime(  earlyDatetime,
                                                             laterDatetime));
                ASSERT_SAFE_FAIL(obj.setUtcStartAndEndTime(  laterDatetime,
                                                             earlyDatetime));
                ASSERT_SAFE_PASS(obj.setUtcStartAndEndTime(   someDatetime,
                                                              someDatetime));
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
        //: 1 Create an object 'w' (default ctor).       { w:D             }
        //: 2 Create an object 'x' (copy from 'w').      { w:D x:D         }
        //: 3 Set 'x' to 'A' (value distinct from 'D').  { w:D x:A         }
        //: 4 Create an object 'y' (init. to 'A').       { w:D x:A y:A     }
        //: 5 Create an object 'z' (copy from 'y').      { w:D x:A y:A z:A }
        //: 6 Set 'z' to 'D' (the default value).        { w:D x:A y:A z:D }
        //: 7 Assign 'w' from 'x'.                       { w:A x:A y:A z:D }
        //: 8 Assign 'w' from 'z'.                       { w:D x:A y:A z:D }
        //: 9 Assign 'x' from 'x' (aliasing).            { w:D x:A y:A z:D }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // Attribute 1 Values: 'descriptor'

        const T1 D1 = Desc();     // default value
        const T1 A1 = Desc(-4 * 60 * 60, true, "EDT");

        // Attribute 2 Values: 'utcStartTime'

        const T2 D2 = bdlt::Datetime();                  // default value
        const T2 A2 = bdlt::Datetime(2010,  3, 14, 7);

        // Attribute 3 Values: 'utcEndTime'

        const T3 D3 = bdlt::Datetime();                  // default value
        const T3 A3 = bdlt::Datetime(2010, 11,  7, 6);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (default ctor)."
                             "\t\t{ w:D             }" << endl;

        Obj mW;  const Obj& W = mW;

        if (veryVerbose) cout << "\ta. Check initial value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.descriptor());
        ASSERT(D2 == W.utcStartTime());
        ASSERT(D3 == W.utcEndTime());

        if (veryVerbose) cout <<
                  "\tb. Try equality operators: 'w' <op> 'w'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                             "\t\t{ w:D x:D         }" << endl;

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) cout << "\ta. Check initial value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(D1 == X.descriptor());
        ASSERT(D2 == X.utcStartTime());
        ASSERT(D3 == X.utcEndTime());

        if (veryVerbose) cout <<
                   "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 'D')."
                             "\t\t{ w:D x:A         }" << endl;

        mX.setDescriptor(A1);
        mX.setUtcStartAndEndTime(A2, A3);

        if (veryVerbose) cout << "\ta. Check new value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.descriptor());
        ASSERT(A2 == X.utcStartTime());
        ASSERT(A3 == X.utcEndTime());

        if (veryVerbose) cout <<
             "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create an object 'y' (init. to 'A')."
                             "\t\t{ w:D x:A y:A     }" << endl;

        Obj mY(A1, A2, A3);  const Obj& Y = mY;

        if (veryVerbose) cout << "\ta. Check initial value of 'y'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Y) }

        ASSERT(A1 == Y.descriptor());
        ASSERT(A2 == Y.utcStartTime());
        ASSERT(A3 == Y.utcEndTime());

        if (veryVerbose) cout <<
             "\tb. Try equality operators: 'y' <op> 'w', 'x', 'y'" << endl;

        ASSERT(0 == (Y == W));        ASSERT(1 == (Y != W));
        ASSERT(1 == (Y == X));        ASSERT(0 == (Y != X));
        ASSERT(1 == (Y == Y));        ASSERT(0 == (Y != Y));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 5. Create an object 'z' (copy from 'y')."
                             "\t\t{ w:D x:A y:A z:A }" << endl;

        Obj mZ(Y);  const Obj& Z = mZ;

        if (veryVerbose) cout << "\ta. Check initial value of 'z'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(A1 == Z.descriptor());
        ASSERT(A2 == Z.utcStartTime());
        ASSERT(A3 == Z.utcEndTime());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set 'z' to 'D' (the default value)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;

        mZ.setDescriptor(D1);
        mZ.setUtcStartAndEndTime(D2, D3);

        if (veryVerbose) cout << "\ta. Check new value of 'z'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(D1 == Z.descriptor());
        ASSERT(D2 == Z.utcStartTime());
        ASSERT(D3 == Z.utcEndTime());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (Z == W));        ASSERT(0 == (Z != W));
        ASSERT(0 == (Z == X));        ASSERT(1 == (Z != X));
        ASSERT(0 == (Z == Y));        ASSERT(1 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 7. Assign 'w' from 'x'."
                             "\t\t\t\t{ w:A x:A y:A z:D }" << endl;
        mW = X;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(A1 == W.descriptor());
        ASSERT(A2 == W.utcStartTime());
        ASSERT(A3 == W.utcEndTime());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'w' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(1 == (W == X));        ASSERT(0 == (W != X));
        ASSERT(1 == (W == Y));        ASSERT(0 == (W != Y));
        ASSERT(0 == (W == Z));        ASSERT(1 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 8. Assign 'w' from 'z'."
                             "\t\t\t\t{ w:D x:A y:A z:D }" << endl;
        mW = Z;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.descriptor());
        ASSERT(D2 == W.utcStartTime());
        ASSERT(D3 == W.utcEndTime());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(0 == (W == X));        ASSERT(1 == (W != X));
        ASSERT(0 == (W == Y));        ASSERT(1 == (W != Y));
        ASSERT(1 == (W == Z));        ASSERT(0 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 9. Assign 'x' from 'x' (aliasing)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;
        mX = X;

        if (veryVerbose) cout << "\ta. Check (same) value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.descriptor());
        ASSERT(A2 == X.utcStartTime());
        ASSERT(A3 == X.utcEndTime());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));
        ASSERT(1 == (X == Y));        ASSERT(0 == (X != Y));
        ASSERT(0 == (X == Z));        ASSERT(1 == (X != Z));

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
