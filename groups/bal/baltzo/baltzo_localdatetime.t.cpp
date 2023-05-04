// baltzo_localdatetime.t.cpp                                         -*-C++-*-

#include <baltzo_localdatetime.h>

#include <bdlt_datetime.h>

#include <bslim_testutil.h>

#include <bslx_testinstream.h>           // for testing only
#include <bslx_testinstreamexception.h>  // for testing only
#include <bslx_testoutstream.h>          // for testing only

#include <bslalg_swaputil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_usesallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_review.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a single, unconstrained (value-semantic)
// attribute class that characterizes a local time value as a aggregation of a
// 'bdlt::DatetimeTz' value (i.e., 'bdlt::Datetime' value aggregated with an
// offset value (in minutes) from Coordinated Universal Time [UTC]), and an a
// time-zone identifier.  In practice, the time-zone identifier will correspond
// to an identifier from the Zoneinfo (a.k.a., Olson) database; however, the
// class does not enforce the practice.  The Primary Manipulators and Basic
// Accessors are therefore respectively the attribute setters and getters, each
// of which follows our standard unconstrained attribute-type naming
// conventions: 'setAttributeName' and 'attributeName'.
//
// Primary Manipulators:
//: o 'setDatetimeTz'
//: o 'setTimeZoneId'
//
// Basic Accessors:
//: o 'get_allocator' (orthogonal to value)
//: o 'datetimeTz'
//: o 'timeZoneId'
//
// This particular attribute class also provides a value constructor capable of
// creating an object in any state relevant for thorough testing, obviating the
// primitive generator function, 'gg', normally used for this purpose.  We will
// therefore follow our standard 10-case approach to testing value-semantic
// types except that we will test the value constructor in case 3 (in lieu of
// the generator function), with the default constructor and primary
// manipulators tested fully in case 2.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Any allocated memory is always from the object allocator.
//: o An object's value is independent of the allocator used to supply memory.
//: o Injected exceptions are safely propagated during memory allocation.
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
// [10] static int maxSupportedBdexVersion();
// [10] static int maxSupportedBdexVersion(int);
//
// CREATORS
// [ 2] LocalDatetime();
// [ 2] LocalDatetime(const allocator_type& a);
// [ 2] ~LocalDatetime();
// [ 3] LocalDatetime(DatetimeTz& d, const StringRef& t, a = { });
// [ 3] LocalDatetime(DatetimeTz& d, const char *t, a = { });
// [ 7] LocalDatetime(const LocalDatetime& o, a = { });
// [11] LocalDatetime(MovableRef<LocalDatetime> o);
// [11] LocalDatetime(MovableRef<LocalDatetime> o, a);
//
// MANIPULATORS
// [ 9] LocalDatetime& operator=(const LocalDatetime& rhs);
// [12] LocalDatetime& operator=(MovableRef<LocalDatetime> rhs);
// [ 2] setDatetimeTz(const bdlt::DatetimeTz& value);
// [ 2] setTimeZoneId(const StringRef& value);
// [ 2] setTimeZoneId(const char *value);
//
// [ 8] void swap(LocalDatetime& other);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
//
// ACCESSORS
// [ 4] allocator_type get_allocator() const;
// [ 4] bslma::Allocator *allocator() const;
// [ 4] const bdlt::DatetimeTz& datetimeTz() const;
// [ 4] const bsl::string& timeZoneId() const;
//
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const LocalDatetime&, const LocalDatetime&);
// [ 6] bool operator!=(const LocalDatetime&, const LocalDatetime&);
// [ 5] operator<<(ostream& s, const LocalDatetime& d);
//
// FREE FUNCTIONS
// [ 8] void swap(LocalDatetime& a, LocalDatetime& b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [13] USAGE EXAMPLE
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ 3] CONCERN: All creator/manipulator ptr./ref. parameters are 'const'.
// [ 5] CONCERN: All accessor methods are declared 'const'.
// [ 3] CONCERN: String arguments can be either 'char *' or 'string'.
// [ 9] CONCERN: All memory allocation is from the object's allocator.
// [ 9] CONCERN: All memory allocation is exception neutral.
// [ 9] CONCERN: Object value is independent of the object allocator.
// [ 9] CONCERN: There is no temporary allocation from any allocator.
// [ 8] CONCERN: Precondition violations are detected when enabled.

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
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)

// ============================================================================
//                      CONVENIENCE MACROS
// ----------------------------------------------------------------------------

// For use in ASSERTV macro invocations to print allocator.
#define ALLOC_OF(EXPR) (EXPR).get_allocator().mechanism()

// ============================================================================
//                    EXCEPTION TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define EXCEPTION_COUNT bslmaExceptionCounter

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef baltzo::LocalDatetime Obj;
typedef Obj::allocator_type   AllocType;  // Test 'allocator_type' exists.

typedef bslma::TestAllocator TestAllocator;
typedef bslx::TestInStream   In;
typedef bslx::TestOutStream  Out;

#define VERSION_SELECTOR 20140601

// ============================================================================
//                       HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static bool someDiff(const Obj& a, const Obj& b)
    // Return 'true' if the specified 'a' and 'b' differ in some way and
    // 'false' otherwise.
{
    return a.datetimeTz() != b.datetimeTz()
        || a.timeZoneId() != b.timeZoneId();
}

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

// ============================================================================
//                               TEST APPARATUS
// ----------------------------------------------------------------------------

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

    TestAllocator globalAllocator(veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 13: {
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
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Creation and Use of a 'baltzo::LocalDatetime' Object
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we default-construct a 'baltzo::LocalDatetime' object:
//..
    baltzo::LocalDatetime localDatetime;
//..
// Next, we update the time referred to by 'localDatetime' to the New York
// time "December 25, 2009, 11:00" with the time-zone identifier set to
// "America/New_York":
//..
    bdlt::Datetime   datetime(2009, 12, 25, 11, 00, 00);
    bdlt::DatetimeTz datetimeTz(datetime, -5 * 60);  // offset is specified
                                                    // in minutes from UTC
    bsl::string     timeZoneId("America/New_York");
    localDatetime.setDatetimeTz(datetimeTz);
    localDatetime.setTimeZoneId(timeZoneId);

    ASSERT(datetimeTz == localDatetime.datetimeTz());
    ASSERT(timeZoneId == localDatetime.timeZoneId());
//..
// Now, we change the time-zone identifier to another string, for example
// "Europe/Berlin":
//..
    bsl::string anotherTimeZoneId("Europe/Berlin");
    localDatetime.setTimeZoneId(anotherTimeZoneId);

    ASSERT(datetimeTz        == localDatetime.datetimeTz());
    ASSERT(anotherTimeZoneId == localDatetime.timeZoneId());
//..
// Finally, we stream 'localDatetime' to 'bsl::cout':
//..
    bsl::cout << localDatetime << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  [ 25DEC2009_11:00:00.000-0500 "Europe/Berlin" ]
//..

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // MOVE-ASSIGNMENT OPERATOR
        //   Ensure that we can move the value of any object of the class to
        //   any object of the class, such that the target object subsequently
        //   has the source value, and there are no additional allocations if
        //   only one allocator is being used, and the source object is
        //   unchanged if allocators are different.
        //
        // Concerns:
        //: 1 The move assignment operator can change the value of any
        //:   modifiable target object to that of any source object.
        //:
        //: 2 The allocator used by the target object is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 If the allocators are different, the value of the source object
        //:   is not modified.
        //:
        //: 7 If the allocators are the same, no new allocations happen when
        //:   the move assignment happens.
        //:
        //: 8 The allocator used by the source object is unchanged.
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
        //:       verify that the respective allocators used by the target and
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
        //   LocalDatetime& operator=(MovableRef<LocalDatetime> rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MOVE-ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(bslmf::MovableRef<Obj>);

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
        bdlt::DatetimeTz defaultDtz;

        bdlt::Datetime   smallDt; smallDt.addMilliseconds(1);
        bdlt::Datetime    someDt(2011,  5,  3, 15, 32);
        bdlt::Datetime   largeDt(9999, 12, 31, 23, 59, 59, 999);

        bdlt::DatetimeTz smallDtz(smallDt, -(24 * 60 - 1));
        bdlt::DatetimeTz  someDtz( someDt, -( 4 * 60 - 0));
        bdlt::DatetimeTz largeDtz(largeDt,  (24 * 60 - 1));

        const char *defaultTzId = "";
        const char   *smallTzId = "a";
        const char   *largeTzId = LONGEST_STRING;

        const struct {
            int               d_line;  // source line number
            char              d_mem;   // expected allocation: 'Y', 'N', '?'
            bdlt::DatetimeTz *d_datetimeTz_p;
            const char       *d_timeZoneId_p;
        } DATA[] = {

            //LINE  MEM  DTTZ          TZID
            //----  ---  -----------   -----------

            // default (must be first)
            { L_,   'N', &defaultDtz,  defaultTzId },

            // 'datetimeTz'
            { L_,   'N',   &smallDtz,  defaultTzId },
            { L_,   'N',    &someDtz,  defaultTzId },
            { L_,   'N',   &largeDtz,  defaultTzId },

            // 'timeZoneId'
            { L_,   '?', &defaultDtz,    smallTzId },
            { L_,   'Y', &defaultDtz,    largeTzId },

            // other
            { L_,   '?',    &someDtz,    smallTzId },
            { L_,   'Y',    &someDtz,    largeTzId },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int               LINE1   =  DATA[ti].d_line;
            const char              MEMSRC1 =  DATA[ti].d_mem;
            const bdlt::DatetimeTz& DTTZ1   = *DATA[ti].d_datetimeTz_p;
            const char *const       TZID1   =  DATA[ti].d_timeZoneId_p;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj  Z(DTTZ1, TZID1, &scratch);
            const Obj ZZ(DTTZ1, TZID1, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            // move assignment with the same allocator

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int               LINE2 =  DATA[tj].d_line;
                const bdlt::DatetimeTz& DTTZ2 = *DATA[tj].d_datetimeTz_p;
                const char *const       TZID2 =  DATA[tj].d_timeZoneId_p;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    // Test move assignment with same allocator.

                    Obj mF(DTTZ1, TZID1, &oa);  const Obj& F = mF;
                    Obj mX(DTTZ2, TZID2, &oa);  const Obj& X = mX;

                    if (veryVerbose) { T_ P_(LINE2) P_(F) P(X) }

                    ASSERTV(LINE1, LINE2, F, X, (F == X) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor oam(&oa);

                    Obj *mR = &(mX = bslmf::MovableRefUtil::move(mF));
                    ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                    ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);

                    ASSERTV(LINE1, LINE2, oam.isTotalSame());

                    ASSERTV(LINE1, LINE2, &oa, ALLOC_OF(X),
                            &oa == X.get_allocator());
                    ASSERTV(LINE1, LINE2, &oa, ALLOC_OF(F),
                            &oa == F.get_allocator());

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                        0 == oa.numBlocksInUse());
            }

            // move assignment with different allocator

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int               LINE2   =  DATA[tj].d_line;
                const char              MEMDST2 =  DATA[tj].d_mem;
                const bdlt::DatetimeTz& DTTZ2   = *DATA[tj].d_datetimeTz_p;
                const char *const       TZID2   =  DATA[tj].d_timeZoneId_p;

                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                {
                    // Test move assignment with different allocator.

                    Obj mF(DTTZ1, TZID1, &oa1);  const Obj& F = mF;
                    Obj mX(DTTZ2, TZID2, &oa2);  const Obj& X = mX;

                    if (veryVerbose) { T_ P_(LINE2) P_(F) P(X) }

                    ASSERTV(LINE1, LINE2, F, X, (F == X) == (LINE1 == LINE2));

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa2) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = bslmf::MovableRefUtil::move(mF));
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);

                        ASSERTV(LINE1, LINE2,  Z,   F,  Z == F);

                        ASSERTV(LINE1, LINE2, &oa2, ALLOC_OF(X),
                               &oa2 == X.get_allocator());
                        ASSERTV(LINE1, LINE2, &oa1, ALLOC_OF(F),
                               &oa1 == F.get_allocator());

#ifdef BDE_BUILD_TARGET_EXC
                        if ('N' == MEMDST2 && 'Y' == MEMSRC1) {
                            ASSERTV(LINE1, LINE2, 0 < EXCEPTION_COUNT);
                        }
#endif
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    anyObjectMemoryAllocatedFlag |= !!oa1.numBlocksInUse();
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, oa1.numBlocksInUse(),
                        0 == oa1.numBlocksInUse());
                ASSERTV(LINE1, LINE2, oa2.numBlocksInUse(),
                        0 == oa2.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj       mX(DTTZ1, TZID1, &oa);  Obj& Z = mX;
                const Obj ZZ(DTTZ1, TZID1, &scratch);

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa);

                Obj *mR = &(mX = bslmf::MovableRefUtil::move(Z));
                ASSERTV(LINE1, ZZ,   Z, ZZ == Z);
                ASSERTV(LINE1, mR, &mX, mR == &mX);

                ASSERTV(LINE1, &oa, ALLOC_OF(Z), &oa == Z.get_allocator());

                ASSERTV(LINE1, oam.isInUseSame());

                ASSERTV(LINE1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }

        // Double check that some object memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the new object has the original value.
        //   Verify that if the same allocator is used there have been no new
        //   allocations, and if a different allocator is used the source
        //   object has the original value.
        //
        // Concerns:
        //: 1 The move constructor (with or without a supplied allocator)
        //:   creates an object having the same value as the original object
        //:   started with.
        //:
        //: 2 If an allocator is NOT supplied, the allocator of the new object
        //:   is the same as the original object, and no new allocations occur.
        //:
        //: 3 If an allocator is supplied that is the same as the original
        //:   object, then no new allocations occur.
        //:
        //: 4 If an allocator is supplied that is different from the original
        //:   object, then the original object's value remains unchanged.
        //:
        //: 5 Supplying a default-constructed allocator explicitly is the same
        //:   as supplying the default allocator.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The allocator used by the original object is unchanged.
        //:
        //:10 Any memory allocation is exception neutral.
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
        //:   table described in P-1:  (C-1..9)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that creates an object by
        //:     move-constructing from a newly created object with value V,
        //:     but invokes the move constructor differently in each
        //:     iteration: (a) using the standard single-argument move
        //:     constructor, (b) using the extended move constructor with a
        //:     default-constructed allocator argument (to use the default
        //:     allocator), (c) using the extended move constructor with the
        //:     same allocator as the moved-from object, and (d) using the
        //:     extended move constructor with a different allocator than the
        //:     moved-from object.
        //:
        //: 3 For each of these iterations (P-2.2):
        //:
        //:   1 Create four 'bslma::TestAllocator' objects, and install one as
        //:     the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Dynamically allocate another object 'F" using the 's1'
        //:     allocator having the same value V, using a distinct allocator
        //:     for the object's footprint.
        //:
        //:   3 Dynamically allocate an object 'X' using the appropriate move
        //:     constructor to move from 'F', passing as a second argument
        //:     (a) nothing, (b) 'allocator_type()', (c) '&s1', or (d)
        //:     'allocator_type(&s2)'.
        //:
        //:   4 Record the allocator expected to be used by the new object and
        //:     how much memory it used before the move constructor.
        //:
        //:   5 Verify that space for 2 objects is used in the footprint
        //:     allocator
        //:
        //:   6 Verify that the moved-to object has the expected value 'V' by
        //:     comparing to 'Z'.
        //:
        //:   7 If the allocators of 'F' and 'X' are different, verify that the
        //:     value of 'F' is still 'V', and that the amount of memory
        //:     used in the allocator for 'X' is the same as the amount of
        //:     that was used by 'F'.
        //:
        //:   8 If the allocators of 'F' and 'X' are the same, verify that no
        //:     extra memory was used by the move constructor.
        //:
        //:   9 Verify that no memory was used by the move constructor as
        //:     temporary memory, and no unused allocators have had any memory
        //:     used.
        //:
        //:  10 Delete both dynamically allocated objects and verify that all
        //:     temporary allocators have had all memory returned to them.
        //:
        //: 3 Test again, using the data of P-1, but this time just for the
        //:   supplied allocator configuration (P-2.2c), and create the object
        //:   as an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  Do
        //:   this by creating one object with one test allocator ('s1') and
        //:   then using the move constructor with a separate test allocator
        //:   that is injecting exceptions ('s2').
        //:   (C-10)
        //
        // Testing:
        //   LocalDatetime(MovableRef<LocalDatetime> o);
        //   LocalDatetime(MovableRef<LocalDatetime> o, a);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MOVE CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        bdlt::DatetimeTz defaultDtz;

        bdlt::Datetime   smallDt; smallDt.addMilliseconds(1);
        bdlt::Datetime    someDt(2011,  5,  3, 15, 32);
        bdlt::Datetime   largeDt(9999, 12, 31, 23, 59, 59, 999);

        bdlt::DatetimeTz smallDtz(smallDt, -(24 * 60 - 1));
        bdlt::DatetimeTz  someDtz( someDt, -( 4 * 60 - 0));
        bdlt::DatetimeTz largeDtz(largeDt,  (24 * 60 - 1));

        const char *defaultTzId = "";
        const char   *smallTzId = "a";
        const char   *largeTzId = LONGEST_STRING;

        const struct {
            int               d_line;  // source line number
            char              d_mem;   // expected allocation: 'Y', 'N', '?'
            bdlt::DatetimeTz *d_datetimeTz_p;
            const char       *d_timeZoneId_p;
        } DATA[] = {

            //LINE  MEM  DTTZ          TZID
            //----  ---  -----------   -----------

            // default (must be first)
            { L_,   'N', &defaultDtz,  defaultTzId },

            // 'datetimeTz'
            { L_,   'N',   &smallDtz,  defaultTzId },
            { L_,   'N',    &someDtz,  defaultTzId },
            { L_,   'N',   &largeDtz,  defaultTzId },

            // 'timeZoneId'
            { L_,   '?', &defaultDtz,    smallTzId },
            { L_,   'Y', &defaultDtz,    largeTzId },

            // other
            { L_,   '?',    &someDtz,    smallTzId },
            { L_,   'Y',    &someDtz,    largeTzId },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int               LINE =  DATA[ti].d_line;
                const char              MEM  =  DATA[ti].d_mem;
                const bdlt::DatetimeTz& DTTZ = *DATA[ti].d_datetimeTz_p;
                const char *const       TZID =  DATA[ti].d_timeZoneId_p;

                ASSERTV(LINE, MEM, MEM && strchr("YN?", MEM));

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const Obj  Z(DTTZ, TZID, &scratch);
                const Obj ZZ(DTTZ, TZID, &scratch);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator s1("supplied",  veryVeryVeryVerbose);
                    bslma::TestAllocator s2("supplied2", veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj *fromPtr = new (fa) Obj(DTTZ, TZID, &s1);

                    Obj&  mF = *fromPtr;  const Obj& F = mF;
                    Obj  *objPtr = 0;

                    bslma::TestAllocator *objAllocatorPtr = 0;
                    bsls::Types::Int64    s1Alloc = s1.numBytesInUse();
                    bsls::Types::Int64    objAlloc;

                    switch (CONFIG) {
                      case 'a': {
                        // normal move constructor
                        objAllocatorPtr = &s1;
                        objAlloc = objAllocatorPtr->numBytesInUse();
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF));
                      } break;
                      case 'b': {
                        // allocator move constructor, default allocator
                        objAllocatorPtr = &da;
                        objAlloc = objAllocatorPtr->numBytesInUse();
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF),
                                              Obj::allocator_type());
                      } break;
                      case 'c': {
                        // allocator move constructor, same allocator
                        objAllocatorPtr = &s1;
                        objAlloc = objAllocatorPtr->numBytesInUse();
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF),
                                              objAllocatorPtr);
                      } break;
                      case 'd': {
                        // allocator move constructor, different allocator
                        objAllocatorPtr = &s2;
                        Obj::allocator_type alloc(objAllocatorPtr);
                        objAlloc = objAllocatorPtr->numBytesInUse();
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF),
                                              alloc);
                      } break;
                      default: {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(LINE, CONFIG, 2*sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(mF) P(X) }

                    bslma::TestAllocator& oa = *objAllocatorPtr;

                    // Ensure the first row of the table contains the
                    // default-constructed value.

                    static bool firstFlag = true;
                    if (firstFlag) {
                        ASSERTV(LINE, CONFIG, Obj(), *objPtr,
                                Obj() == *objPtr);
                        firstFlag = false;
                    }

                    // Verify the value of the object.

                    ASSERTV(LINE, CONFIG, Z, X, Z == X);

                    if (objAllocatorPtr != F.get_allocator()) {
                        // If the allocators are different, verify that the
                        // value of 'fX' has not changed.

                        ASSERTV(LINE, CONFIG, Z, F, Z == F);

                         // If memory was used, verify that the same amount was
                         // used by the moved-to object.

                        bsls::Types::Int64 moveBytesUsed =
                                   objAllocatorPtr->numBytesInUse() - objAlloc;
                        ASSERTV(LINE, CONFIG,
                                 s1.numBytesInUse(), moveBytesUsed,
                                 s1.numBytesInUse() == moveBytesUsed);
                    }
                    else {
                        // If the allocators are the same, verify that no new
                        // bytes were allocated by moving.

                        ASSERTV(LINE, CONFIG, s1Alloc, s1.numBytesInUse(),
                                s1Alloc == s1.numBytesInUse());
                    }

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    ASSERTV(LINE, CONFIG,
                            &oa == X.timeZoneId().get_allocator());

                    // Also invoke the object's 'get_allocator' accessor, as
                    // well as that of 'Z'.

                    ASSERTV(LINE, CONFIG, &oa, ALLOC_OF(X),
                            &oa == X.get_allocator());

                    ASSERTV(LINE, CONFIG, &scratch, ALLOC_OF(Z),
                            &scratch == Z.get_allocator());

                    // Verify no allocation from the non-object allocators.
                    if (objAllocatorPtr != &da) {
                        ASSERTV(LINE, CONFIG, da.numBlocksTotal(),
                                0 == da.numBlocksTotal());
                    }

                    if (objAllocatorPtr != &s2) {
                        ASSERTV(LINE, CONFIG, s2.numBlocksTotal(),
                                0 == s2.numBlocksTotal());
                    }

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    ASSERTV(LINE, CONFIG, oa.numBlocksTotal(),
                            oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());


                    // Verify expected ('Y'/'N') object-memory allocations.

                    if ('?' != MEM) {
                        ASSERTV(LINE, CONFIG, MEM, oa.numBlocksInUse(),
                                ('N' == MEM) == (0 == oa.numBlocksInUse()));
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated objects under test.

                    fa.deleteObject(fromPtr);
                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                            0 == da.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, s1.numBlocksInUse(),
                            0 == s1.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, s2.numBlocksInUse(),
                            0 == s2.numBlocksInUse());
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
                const int               LINE =  DATA[ti].d_line;
                const char              MEM  =  DATA[ti].d_mem;
                const bdlt::DatetimeTz& DTTZ = *DATA[ti].d_datetimeTz_p;
                const char *const       TZID =  DATA[ti].d_timeZoneId_p;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator s1("supplied1", veryVeryVeryVerbose);
                bslma::TestAllocator s2("supplied2", veryVeryVeryVerbose);

                const Obj Z (DTTZ, TZID, &scratch);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(s2) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj from(DTTZ, TZID, &s1);

                    Obj obj(bslmf::MovableRefUtil::move(from), &s2);
                    ASSERTV(LINE, Z, obj, Z == obj);

#ifdef BDE_BUILD_TARGET_EXC
                    if ('Y' == MEM) {
                        ASSERTV(LINE, 0 < EXCEPTION_COUNT);
                    }
#endif
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, &scratch, ALLOC_OF(Z),
                        &scratch == Z.get_allocator());
                ASSERTV(LINE, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LINE, s1.numBlocksInUse(),
                        0 == s1.numBlocksInUse());
                ASSERTV(LINE, s2.numBlocksInUse(),
                        0 == s2.numBlocksInUse());
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BDEX STREAMING
        //   Ensure that we can serialize the value of any object of the class
        //   via its 'bdexStreamOut' method, and then deserialize that value
        //   back into any object of the class, via its 'bdexStreamIn' method.
        //
        // Concerns:
        //: 1 The signature and return type of 'bdexStreamOut', 'bdexStreamIn',
        //:   and 'maxSupportedBdexVersion' are standard.
        //:
        //: 2 The 'maxSupportedBdexVersion' method returns the expected value.
        //:
        //: 3 For all supported versions, any sequence of valid values can be
        //:   externalized using the 'bdexStreamOut' method.
        //:
        //: 4 For all supported versions, any valid value on the wire can
        //:   unexternalized by 'bdexStreamIn' into an object having that
        //:   value, irrespective of the initial value of the object.
        //:
        //: 5 Both stream methods always return a reference with modifiable
        //:   access to the specified 'stream'.
        //:
        //: 6 For both stream methods, the specified 'stream' is left in an
        //:   invalid state, with no other effect, if the specified 'version'
        //:   is outside the range '[1 .. maxSupportedBdexVersion]'.
        //:
        //: 7 Both stream methods must return with no effect, if the specified
        //:   'stream' is invalid on entry.
        //:
        //: 8 If 'stream' becomes invalid during an invocation of
        //:   'bdexStreamIn', the object is left in a valid state, but possibly
        //:   modified, state.
        //:
        //: 9 If an exception is thrown during a call to the 'bdexStreamIn'
        //:   method, the object is left in an unspecified but valid state.
        //:   In no event is memory leaked.
        //:
        //:10 The wire format of this object is be the concatenation of the
        //:   wire formats of its constituent attributes, in the order of their
        //:   declaration.  (TBD)
        //:
        //:11 The specified 'version' is not part of the wire format generated
        //:   by the 'bdexStreamOut' method, and is not expected by the
        //:   'bdexStreamIn' method.
        //:
        //:12 The 'bdexStreamIn' method leaves the object unchanged, when the
        //:   specified 'stream' is empty.
        //:
        //:13 The 'bdexStreamIn' method leaves the object in a valid but
        //:   unspecified state, when the specified 'stream' contains
        //:   well-formed data that violates object constraints.
        //:
        //:14 The 'bdexStreamIn' method leaves the object in a valid (but
        //:   unspecified) state when the specified 'stream' contains valid but
        //:   incomplete data.
        //:
        //:15 QoI: No memory is allocated by the object by the 'bdexStreamOut'
        //:   method.
        //:
        //:16 QoI: All memory allocated by the 'bdexStreamIn' method is from
        //:   the object allocator.
        //
        // Plan:
        //: 1 Use the addresses of the (templated) 'bdexStreamIn' and
        //:   'bdexStreamOut', instantiated using the 'bslx::TestInStream' and
        //:   'bslx::TestOutStream types, respectively, to initialize
        //:   member-function pointers each having the standard signature and
        //:   return type for the for these member-functions.  Use the
        //:   'maxSupportedBdexVersion' static function to initialize a
        //:   function pointer having the standard signature and return type
        //:   for that function.  (C-1)
        //:
        //: 2 Compare the return value of the 'maxSupportedBdexVersion' to the
        //:   expected value for this implementation.  (C-2)
        //:
        //: 3 In test cases with both valid and invalid input, compare the
        //:   return values of 'bdexStreamIn' and 'bdexStreamOut' methods with
        //:   their specified 'stream'. (C-5)
        //:
        //: 3 Specify a set 'S' of unique object values with substantial and
        //:   varied differences.  For each value in 'S', construct an object
        //:   'x' along with a sequence of similarly constructed duplicates
        //:   'x1,   x2, ..., xN'.  Attempt to affect every aspect of white-box
        //:   state by altering each 'xi' in a unique way.  Let the union of
        //:   all such objects be the set 'T', programmatically represented by
        //:   the 'VALUES' array of objects.
        //:
        //: 4 Using all combinations of '(u, v)' in 'T X T', stream-out the
        //:   value of u into a buffer and stream it back into (an independent
        //:   object of) 'v', and assert that 'u == v'.  (C-3) Compare the
        //:   return value of the each 'bdexStreamIn' and 'bdexStreamOut'
        //:   method call with the supplied 'stream'.  (C-4)
        //:
        //: 5 Throughout this test case, wrap 'bdexStreamIn' calls with the
        //:   standard 'BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN' and
        //:   'BSLX_TESTINSTREAM_EXCEPTION_TEST_END' macros to confirm
        //:   exception neutrality.  (C-7)
        //:
        //: 6 For each 'x' in 'T', attempt to stream into (a temporary copy of)
        //:   'x' from an empty (but valid) and then invalid stream.  Verify
        //:   after each try that the object is unchanged and that the stream
        //:   is invalid.  (C-7, C-12) For each 'x' in 'T' attempt to stream
        //:   'x' into an initially invalid stream.  Check that the stream is
        //:   unchanged.  For each stream operation (both in and out), compare
        //:   the return value of the each 'bdexStreamIn' and 'bdexStreamOut'
        //:   method call with the supplied 'stream'.  (C-5)
        //:
        //: 7 Write three distinct objects to an output stream buffer of total
        //:   length 'N'.  For each partial stream length from 0 to 'N - 1',
        //:   construct a truncated input stream and attempt to read into
        //:   objects initialized with distinct values.  Verify values of
        //:   objects that are either successfully modified or left entirely
        //:   unmodified, and that the stream became invalid immediately after
        //:   the first incomplete read.  Finally ensure that each object
        //:   streamed into is in some valid state by creating a copy and then
        //:   assigning a known value to that copy; allow the original object
        //:   to leave scope without further modification, so that the
        //:   destructor asserts internal object invariants appropriately.
        //:   (C-14)
        //:
        //: 8 TBD: Iteratively write three distinct objects to an output
        //:   stream buffer, instructing the output stream to make the next
        //:   output operation invalid before one of the calls to  the
        //:   object's 'bdexStreamOut' method.  On each iteration, change the
        //:   point at which the invalid operation is introduced.  In each
        //:   case, the reconstruction of the object from the stream must
        //:   return normally up to the point at which the error was introduced
        //:   (known) and, after that point, an invalid stream, with valid
        //:   though unspecified objects.  (C-8)
        //:
        //: 9 For every supported version, check that an object can be streamed
        //:   and reconstructed (streamed) successfully.  For version numbers
        //:   below and above the supported range (i.e., 0 and
        //:   'maxBdexVersion() + 1), confirm that these operations have no
        //:   effect and the initially valid stream is left in an invalid
        //:   state.  (C-6)
        //:
        //:10 For every object 'x' in 'T' create two outstream: one via the
        //:   object's 'bdexStreamOut' method, and the other by explicitly
        //:   outstreaming each constituent attribute (in order of
        //:   declaration).  (C-10) Confirm that the outstream created via
        //:   individual attributes can be used to reconstruct the object, with
        //:   no data left unread. This accounts for all of the information
        //:   conveyed from the original to the duplicate object; thus, no
        //:   version information was sent.  (C-11).
        //:
        //:11 This class imposes no restrictions on the values of its
        //:   constituent attributes.  If an instream contains data that is
        //:   well-formed but in violation of object constraints, the
        //:   'bdexStreamIn' method of that constituent attribute will fail and
        //:   leave this object in some valid, though unspecified, state.
        //:   (C-13)
        //:
        //:12 In each test using the 'bdexStreamIn' or 'bdexStreamOut' method
        //:   install 'bslma::TestAllocator' object as the default allocator.
        //:   After each 'bdexStreamIn' and 'bdexStreamOut' invocation check
        //:   that the default allocator was not used.  Additionally, after
        //:   'bdexStreamOut' invocation, check that the object allocator was
        //:   not used.  (C15, C-16)
        //
        // Testing:
        //   static int maxSupportedBdexVersion();
        //   static int maxSupportedBdexVersion(int);
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BDEX STREAMING" << endl
                          << "==============" << endl;

        if (verbose) cout <<
                   "\nAssign the addresses of the BDEX methods to variables."
                                                                       << endl;
        {
            // Verify that the signatures and return types are standard.

            typedef In&  (Obj::*funcInPtr) (In&,  int);
            typedef Out& (Obj::*funcOutPtr)(Out&, int) const;
            typedef int  (*funcVerPtr)(int);
            typedef int  (*funcDepPtr)();

            funcInPtr  fIn  = &Obj::bdexStreamIn<In>;
            funcOutPtr fOut = &Obj::bdexStreamOut<Out>;
            funcVerPtr fVer =  Obj::maxSupportedBdexVersion;
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            funcDepPtr fDep =  Obj::maxSupportedBdexVersion;
#endif
            // quash potential compiler warnings
            (void)fIn;
            (void)fOut;
            (void)fVer;
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            (void)fDep;
#endif
        }

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'." << endl;
        {
            if (veryVerbose) cout << "\tusing object syntax:" << endl;
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion(VERSION_SELECTOR));
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(1 == X.maxSupportedBdexVersion());
#endif
            if (veryVerbose) cout << "\tusing class method syntax:" << endl;
            ASSERT(1 == Obj::maxSupportedBdexVersion(VERSION_SELECTOR));
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(1 == Obj::maxSupportedBdexVersion());
#endif
        }

        // ------------------------------------
        // Values used in several stream tests.
        // ------------------------------------

        const int MAX_VERSION = Obj::maxSupportedBdexVersion(VERSION_SELECTOR);

        bdlt::DatetimeTz defaultDtz;

        bdlt::Datetime   smallDt; smallDt.addMilliseconds(1);
        bdlt::Datetime    someDt(2011,  5,  3, 15, 32);
        bdlt::Datetime   largeDt(9999, 12, 31, 23, 59, 59, 999);

        bdlt::DatetimeTz smallDtz(smallDt, -(24 * 60 - 1));
        bdlt::DatetimeTz  someDtz( someDt, -( 4 * 60 - 0));
        bdlt::DatetimeTz largeDtz(largeDt,  (24 * 60 - 1));

        const char *defaultTzId = "";
        const char   *smallTzId = "a";
        const char   *largeTzId = LONGEST_STRING;

        const Obj V0(defaultDtz, defaultTzId);
        const Obj V1(  smallDtz, defaultTzId);
        const Obj V2(   someDtz, defaultTzId);
        const Obj V3(  largeDtz, defaultTzId);
        const Obj V4(defaultDtz,   smallTzId);
        const Obj V5(  smallDtz,   smallTzId);
        const Obj V6(   someDtz,   smallTzId);
        const Obj V7(  largeDtz,   smallTzId);
        const Obj V8(defaultDtz,   largeTzId);
        const Obj V9(defaultDtz,   largeTzId);
        const Obj VA(   someDtz,   largeTzId);
        const Obj VB(   someDtz,   largeTzId);

        const Obj VALUES[]   =
                            { V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, VA, VB };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        if (verbose) cout << "\nDirect initial trial of 'streamOut' and"
                             " (valid) 'streamIn' functionality." << endl;
        //TBD: What does this *test*?
        {
            for (int version = 1; version < MAX_VERSION; ++version) {
                if (veryVerbose) { T_ T_ P(version) }

                bslma::TestAllocator testAllocator("tA", veryVeryVeryVerbose);

                const Obj X(bdlt::DatetimeTz(bdlt::Datetime(2011, 5, 3, 15),
                                            -4 * 60),
                            "a_" SUFFICIENTLY_LONG_STRING,
                            &testAllocator);
                if (veryVerbose) { cout << "\t   Value being streamed: ";
                                   P(X); }
                Out out(1);
                ASSERT(&out == &(X.bdexStreamOut(out, version)));

                const char *const OD  = out.data();
                const bsl::size_t LOD = out.length();

                In in(OD, LOD);  ASSERT(in);  ASSERT(!in.isEmpty());

                Obj t(&testAllocator);  ASSERT(X != t);

                if (veryVerbose) { cout << "\tValue being overwritten: ";
                                   P(t); }
                ASSERT(X != t);

                t.bdexStreamIn(in, version);
                ASSERT(in);
                ASSERT(in.isEmpty());

                if (veryVerbose) { cout << "\t  Value after overwrite: ";
                                   P(t); }
                ASSERT(X == t);
            }
        }

        if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
        {
            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard guard(&da);

            for (int version = 1; version < MAX_VERSION; ++version) {
                if (veryVerbose) { T_ T_ P(version) }

                for (int ui = 0; ui < NUM_VALUES; ++ui) {
                    if (veryVerbose) { T_ T_ P(ui) }

                    bslma::TestAllocator oau("oau", veryVeryVeryVerbose);
                    Obj                  mU(VALUES[ui], &oau);
                    const Obj&           U = mU;
                    const Obj            Z(VALUES[ui], &oau);
                    Out                  out(1);

                    bslma::TestAllocatorMonitor oaum(&oau), dam(&da);
                    ASSERTV(ui, &out == &(U.bdexStreamOut(out, version)));
                    ASSERTV(ui, oaum.isTotalSame());
                    ASSERTV(ui, dam.isTotalSame());

                    const char *const OD  = out.data();
                    const bsl::size_t LOD = out.length();

                    In in(OD, LOD);

                    ASSERTV(U, in);
                    ASSERTV(U, !in.isEmpty());

                    // Verify that each new value overwrites every old value
                    // and that the input stream is emptied, but remains valid.

                    for (int vi = 0; vi < NUM_VALUES; ++vi) {
                        if (veryVerbose) { T_ T_ P(vi) }

                        bslma::TestAllocator oav("oav", veryVeryVeryVerbose);

                        Obj mV(VALUES[vi], &oav);  const Obj& V = mV;


                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oav) {
                            BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {

                                in.reset();
                                bslma::TestAllocatorMonitor dam(&da);
                                ASSERTV(vi,
                                        &in == &mV.bdexStreamIn(in, version));
                                ASSERTV(version, ui, vi, dam.isTotalSame());

                            } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                        ASSERTV(version, ui, vi, U == Z);
                        ASSERTV(version, ui, vi, V == Z);
                        ASSERTV(version, ui, vi, in);
                        ASSERTV(version, ui, vi, in.isEmpty());
                    }
                }
            }
        }

        if (verbose) cout << "\tOn empty and invalid input streams." << endl;
        {
            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard guard(&da);

            Out               out(1);
            const char *const OD  = out.data();
            const bsl::size_t LOD = out.length();
            ASSERT(0 == LOD);

            for (int version = 1; version < MAX_VERSION; ++version) {
                if (veryVerbose) { T_ T_ P(version) }

                for (int i = 0; i < NUM_VALUES; ++i) {
                    In in(OD, LOD);
                    ASSERTV(i, in);
                    ASSERTV(i, in.isEmpty());

                    // Ensure that reading from an empty or invalid input
                    // stream leaves the stream invalid and the target object
                    // unchanged.

                    const Obj X(VALUES[i]);  Obj t1(X), t2(X);
                                             ASSERTV(i, X == t1);
                                             ASSERTV(i, X == t2);

                    BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                        in.reset();
                        ASSERTV(i, in);

                        // read from empty
                        bslma::TestAllocatorMonitor dam1(&da);
                        ASSERTV(i, &in == &(t1.bdexStreamIn(in, version)));
                        ASSERTV(i, dam1.isTotalSame());
                        ASSERTV(i, !in);
                        ASSERTV(i, X == t1);

                        // read from (the now) invalid stream
                        bslma::TestAllocatorMonitor dam2(&da);
                        ASSERTV(i, &in == &(t2.bdexStreamIn(in, version)));
                        ASSERTV(i, dam2.isTotalSame());
                        ASSERTV(i, !in);
                        ASSERTV(i, X == t2);

                    } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
                }
            }
        }

        if (verbose) cout << "\tOn invalid out streams." << endl;
        {
            for (int version = 1; version < MAX_VERSION; ++version) {
                if (veryVerbose) { T_ T_ P(version) }

                for (int i = 0; i < NUM_VALUES; ++i) {
                    if (veryVerbose) { T_ T_ P(i) }

                    bslma::TestAllocator         oa("oa", veryVeryVeryVerbose);
                    bslma::TestAllocator         da("default",
                                                    veryVeryVeryVerbose);
                    bslma::DefaultAllocatorGuard guard(&da);

                    Obj mU(VALUES[i], &oa);  const Obj& U = mU;

                    Out out(1);
                    out.invalidate();
                    ASSERTV(i, !out);
                    const void  *data   = out.data();
                    bsl::size_t  length = out.length();

                    bslma::TestAllocatorMonitor oam(&oa), dam(&da);
                    ASSERTV(version, i, &out ==
                                             &(U.bdexStreamOut(out, version)));
                    ASSERTV(version, i, dam.isTotalSame());
                    ASSERTV(version, i, oam.isTotalSame());
                    ASSERTV(version, i, !out);
                    ASSERTV(version, i, data   == out.data());
                    ASSERTV(version, i, length == out.length());
                }
            }
        }
        if (verbose) cout << "\tOn incomplete (but otherwise valid) data."
                          << endl;
        {
            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard guard(&da);

            // Each object has unique attributes w.r.t. objects to be compared
            // (e.g., W1, X1, Y1).  Note that 'smallDtz', 'someDtz', and
            // 'largeDtz' differ in each constituent attribute and that each
            // 'timeZoneId' is unique.  Thus, partially constructed 'tn'
            // objects will match their target value only when 'tn' is
            // completed assembled.

            const int VERSION = 1;

            const Obj W1(smallDtz, "");
            const Obj X1( someDtz, "a");
            const Obj Y1(largeDtz, "bb");

            const Obj W2(largeDtz, "ccc");
            const Obj X2(smallDtz, "dddd");
            const Obj Y2( someDtz, "eeeee");

            const Obj W3( someDtz, "ffffff");
            const Obj X3(largeDtz, "hhhhhhh");
            const Obj Y3(smallDtz, "iiiiiiii");

            Out out(1);

            X1.bdexStreamOut(out, VERSION);
            const bsl::size_t LOD1 = out.length();
            X2.bdexStreamOut(out, VERSION);
            const bsl::size_t LOD2 = out.length();
            X3.bdexStreamOut(out, VERSION);
            const bsl::size_t LOD  = out.length();

            const char *const OD = out.data();

            for (bsl::size_t i = 0; i < LOD; ++i) {
                In in(OD, i);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();
                    ASSERTV(i, in);
                    ASSERTV(i, !i == in.isEmpty());

                    Obj t1(W1), t2(W2), t3(W3);

                    if (i < LOD1) {
                        bslma::TestAllocatorMonitor dam1(&da);
                        ASSERTV(i, &in == &(t1.bdexStreamIn(in, VERSION)));
                        ASSERTV(i, dam1.isTotalSame());
                        ASSERTV(i, !in);
                        ASSERTV(i, someDiff(X1, t1));

                        ASSERTV(i, bdlt::DatetimeTz::isValid(
                                          t1.datetimeTz().dateTz().localDate(),
                                          t1.datetimeTz().offset()));

                        bslma::TestAllocatorMonitor dam2(&da);
                        ASSERTV(i, &in == &(t2.bdexStreamIn(in, VERSION)));
                        ASSERTV(i, dam2.isTotalSame());
                        ASSERTV(i, !in);
                        ASSERTV(i, W2 == t2);

                        bslma::TestAllocatorMonitor dam3(&da);
                        ASSERTV(i, &in == &(t3.bdexStreamIn(in, VERSION)));
                        ASSERTV(i, dam3.isTotalSame());
                        ASSERTV(i, !in);    ASSERTV(i, W3 == t3);
                    }
                    else if (i < LOD2) {
                        bslma::TestAllocatorMonitor dam1(&da);
                        ASSERTV(i, &in == &(t1.bdexStreamIn(in, VERSION)));
                        ASSERTV(i, dam1.isTotalSame());
                        ASSERTV(i, in);
                        ASSERTV(i, X1 == t1);

                        bslma::TestAllocatorMonitor dam2(&da);
                        ASSERTV(i, &in == &(t2.bdexStreamIn(in, VERSION)));
                        ASSERTV(i, dam2.isTotalSame());
                        ASSERTV(i, !in);
                        ASSERTV(i, someDiff(X2, t2));

                        ASSERTV(i, bdlt::DatetimeTz::isValid(
                                          t2.datetimeTz().dateTz().localDate(),
                                          t2.datetimeTz().offset()));

                        bslma::TestAllocatorMonitor dam3(&da);
                        ASSERTV(i, &in == &(t3.bdexStreamIn(in, VERSION)));
                        ASSERTV(i, dam3.isTotalSame());
                        ASSERTV(i, !in);
                        ASSERTV(i, W3 == t3);
                    }
                    else {
                        bslma::TestAllocatorMonitor dam1(&da);
                        ASSERTV(i, &in == &(t1.bdexStreamIn(in, VERSION)));
                        ASSERTV(i, dam1.isTotalSame());
                        ASSERTV(i, in);
                        ASSERTV(i, X1 == t1);

                        bslma::TestAllocatorMonitor dam2(&da);
                        ASSERTV(i, &in == &(t2.bdexStreamIn(in, VERSION)));
                        ASSERTV(i, dam2.isTotalSame());
                        ASSERTV(i, in);
                        ASSERTV(i, X2 == t2);

                        bslma::TestAllocatorMonitor dam3(&da);
                        ASSERTV(i, &in == &(t3.bdexStreamIn(in, VERSION)));
                        ASSERTV(i, dam3.isTotalSame());
                        ASSERTV(i, !in);
                        ASSERTV(i, someDiff(X3, t3));

                        ASSERTV(i, bdlt::DatetimeTz::isValid(
                                          t3.datetimeTz().dateTz().localDate(),
                                          t3.datetimeTz().offset()));
                    }

                    // Check the validity of the target objects, 'tn', with
                    // some (light) usage (assignment).

                                ASSERTV(i, Y1 != t1);
                    t1 = Y1;    ASSERTV(i, Y1 == t1);

                                ASSERTV(i, Y2 != t2);
                    t2 = Y2;    ASSERTV(i, Y2 == t2);

                                ASSERTV(i, Y3 != t3);
                    t3 = Y3;    ASSERTV(i, Y3 == t3);

                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

#if TBD  // Check 'makeNextInvalid' method.
        if (verbose) cout << "\tOn complete data in an invalidated stream."
                          << endl;
        {
            // Each object unique has unique attributes w.r.t. objects to be
            // compared (e.g., W1, X1, Y1).  Note that 'smallDtz', 'someDtz',
            // and 'largeDtz' differ in each constituent attribute and that
            // each 'timeZoneId' is unique.  Thus, partially constructed 'tn'
            // objects will match their target value only 'tn' is completed
            // assembled.

            const int VERSION = 1;

            const Obj W1(smallDtz, "");
            const Obj X1( someDtz, "a");
            const Obj Y1(largeDtz, "bb");

            const Obj W2(largeDtz, "ccc");
            const Obj X2(smallDtz, "dddd");
            const Obj Y2( someDtz, "eeeee");

            const Obj W3( someDtz, "ffffff");
            const Obj X3(largeDtz, "hhhhhhh");
            const Obj Y3(smallDtz, "iiiiiiii");

            Out out(1);

            X1.bdexStreamOut(out, VERSION);
            ASSERT(&out == &(X1.bdexStreamOut(out, VERSION)));
            ASSERT(&out == &(X2.bdexStreamOut(out, VERSION)));
            out.makeNextInvalid();
            ASSERT(&out == &(X3.bdexStreamOut(out, VERSION)));

            In in(out.data(), out.length());

            BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {

              Obj t1(W1), t2(W2), t3(W3);

              ASSERT(&in == &(t1.bdexStreamIn(in, VERSION)));
              ASSERT( out);
              ASSERT(&in == &(t2.bdexStreamIn(in, VERSION)));
              ASSERT( out);
              ASSERT(&in == &(t3.bdexStreamIn(in, VERSION)));
              ASSERT(!out);

              // Check the validity of the target objects, 'tn', with some
              // (light) usage (assignment).

                            ASSERT(Y1 != t1);
                t1 = Y1;    ASSERT(Y1 == t1);

                            ASSERT(Y2 != t2);
                t2 = Y2;    ASSERT(Y2 == t2);

                            ASSERT(Y3 != t3);
                t3 = Y3;    ASSERT(Y3 == t3);

            } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
        }
#endif

        if (verbose) cout << "\tOn invalid versions" << endl;
        {
            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::TestAllocator         oa("object",  veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            const Obj W(&oa);               // default value
            const Obj X(someDtz, "A", &oa); // original (control) value
            const Obj Y(someDtz, "B", &oa); // new (streamed-out) value

            if (veryVerbose) cout << "\t\tGood Version on instream" << endl;
            {
                const char version = 1;

                Out out(1);
                out.putString(Y.timeZoneId());        // 1. Stream out "new"
                                                      //    value
                Y.datetimeTz().bdexStreamOut(out, 1); // 2. Stream out "new"
                                                      //    value
                const char *const OD  = out.data();
                const bsl::size_t LOD = out.length();

                Obj t(X);
                ASSERT(W != t);
                ASSERT(X == t);
                ASSERT(Y != t);

                In in(OD, LOD);
                ASSERT(in);

                bslma::TestAllocatorMonitor dam(&da);
                ASSERT(&in == &(t.bdexStreamIn(in, version)));
                ASSERT(dam.isTotalSame());
                ASSERT(in);
                ASSERT(W != t); ASSERT(X != t);  ASSERT(Y == t);
            }

            if (veryVerbose) cout << "\t\tBad versions on instream." << endl;
            {
                const char version = 0; // too small ('version' must be >= 1)

                Out out(1);
                Y.datetimeTz().bdexStreamOut(out, 1); // 1. Stream out "new"
                                                      //    value
                out.putString(Y.timeZoneId());        // 2. Stream out "new"
                                                      //    value

                const char *const OD  = out.data();
                const bsl::size_t LOD = out.length();

                Obj t(X);  ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
                In in(OD, LOD);  ASSERT(in);
                in.setQuiet(!veryVerbose);
                bslma::TestAllocatorMonitor dam(&da);
                ASSERT(&in == &(t.bdexStreamIn(in, version)));
                ASSERT(dam.isTotalSame());
                ASSERT(!in);
                ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            }

            {
                const char version = 2 ; // too large (current version is 1)

                Out out(1);
                Y.datetimeTz().bdexStreamOut(out, 1); // 1. Stream out "new"
                                                      //    value
                out.putString(Y.timeZoneId());        // 2. Stream out "new"
                                                      //    value

                const char *const OD  = out.data();
                const bsl::size_t LOD = out.length();

                Obj t(X);  ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);

                In in(OD, LOD);  ASSERT(in);
                in.setQuiet(!veryVerbose);
                bslma::TestAllocatorMonitor dam(&da);
                ASSERT(&in == &(t.bdexStreamIn(in, version)));
                ASSERT(dam.isTotalSame());
                ASSERT(!in);
                ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            }

            if (veryVerbose) cout << "\t\tGood Version on outstream" << endl;
            {
                const char version = 1;

                bslma::TestAllocator sa("scratch", veryVeryVeryVerbose);

                Out out(1, &sa);
                ASSERT(out);
                ASSERT(0 == out.length());

                bslma::TestAllocatorMonitor oam(&oa), dam(&da);
                ASSERT(&out == &(Y.bdexStreamOut(out, version)));
                ASSERT(dam.isTotalSame());
                ASSERT(oam.isTotalSame());
                ASSERT(out);
                ASSERT(0 != out.length());
            }

            if (veryVerbose) cout << "\t\tBad versions on instream." << endl;
            {
                const char version = 0; // too small ('version' must be >= 1)

                Out out(1);
                ASSERT(out);
                ASSERT(0 == out.length());

                bslma::TestAllocatorMonitor oam(&oa), dam(&da);
                ASSERT(&out == &(Y.bdexStreamOut(out, version)));
                ASSERT(dam.isTotalSame());
                ASSERT(oam.isTotalSame());
                ASSERT(!out);
                ASSERT(0 == out.length());
            }

            {
                const char version = 2 ; // too large, current max version is 1

                Out out(1);
                ASSERT(out);
                ASSERT(0 == out.length());

                bslma::TestAllocatorMonitor oam(&oa), dam(&da);
                ASSERT(&out == &(Y.bdexStreamOut(out, version)));
                ASSERT(dam.isTotalSame());
                ASSERT(oam.isTotalSame());
                ASSERT(!out);
                ASSERT(0 == out.length());
            }
        }

        if (verbose) cout << "\nWire format direct tests." << endl;
        {
            for (int i = 0; i < NUM_VALUES; ++i) {

                Obj       mX(VALUES[i]);  const Obj& X = mX;
                const int VERSION = 1;

                if (veryVerbose) { T_ P(X) }

                Out outO(1);  X.bdexStreamOut(outO, VERSION);

                Out outA(1);  outA.putString(X.timeZoneId());
                           X.datetimeTz().bdexStreamOut(outA, VERSION);

                ASSERTV(i, outA.length() == outO.length());
                ASSERTV(i, 0             == memcmp(outO.data(),
                                                   outA.data(),
                                                   outA.length()));

                Obj        mY;  const Obj& Y = mY;
                In         in(outA.data(), outA.length());
                ASSERT(&in == &(mY.bdexStreamIn(in, VERSION)));

                ASSERTV(i, in.isEmpty());
                ASSERTV(i, X == Y);
            }
        }
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
        //:       verify that the respective allocators used by the target and
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
        //   LocalDatetime& operator=(const LocalDatetime& rhs);
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
        bdlt::DatetimeTz defaultDtz;

        bdlt::Datetime   smallDt; smallDt.addMilliseconds(1);
        bdlt::Datetime    someDt(2011,  5,  3, 15, 32);
        bdlt::Datetime   largeDt(9999, 12, 31, 23, 59, 59, 999);

        bdlt::DatetimeTz smallDtz(smallDt, -(24 * 60 - 1));
        bdlt::DatetimeTz  someDtz( someDt, -( 4 * 60 - 0));
        bdlt::DatetimeTz largeDtz(largeDt,  (24 * 60 - 1));

        const char *defaultTzId = "";
        const char   *smallTzId = "a";
        const char   *largeTzId = LONGEST_STRING;

        const struct {
            int               d_line;  // source line number
            char              d_mem;   // expected allocation: 'Y', 'N', '?'
            bdlt::DatetimeTz *d_datetimeTz_p;
            const char       *d_timeZoneId_p;
        } DATA[] = {

            //LINE  MEM  DTTZ          TZID
            //----  ---  -----------   -----------

            // default (must be first)
            { L_,   'N', &defaultDtz,  defaultTzId },

            // 'datetimeTz'
            { L_,   'N',   &smallDtz,  defaultTzId },
            { L_,   'N',    &someDtz,  defaultTzId },
            { L_,   'N',   &largeDtz,  defaultTzId },

            // 'timeZoneId'
            { L_,   '?', &defaultDtz,    smallTzId },
            { L_,   'Y', &defaultDtz,    largeTzId },

            // other
            { L_,   '?',    &someDtz,    smallTzId },
            { L_,   'Y',    &someDtz,    largeTzId },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int               LINE1   =  DATA[ti].d_line;
            const char              MEMSRC1 =  DATA[ti].d_mem;
            const bdlt::DatetimeTz& DTTZ1   = *DATA[ti].d_datetimeTz_p;
            const char *const       TZID1   =  DATA[ti].d_timeZoneId_p;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj  Z(DTTZ1, TZID1, &scratch);
            const Obj ZZ(DTTZ1, TZID1, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int               LINE2   =  DATA[tj].d_line;
                const char              MEMDST2 =  DATA[tj].d_mem;
                const bdlt::DatetimeTz& DTTZ2   = *DATA[tj].d_datetimeTz_p;
                const char *const       TZID2   =  DATA[tj].d_timeZoneId_p;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(DTTZ2, TZID2, &oa);  const Obj& X = mX;

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    ASSERTV(LINE1, LINE2, Z, X, (Z == X) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    ASSERTV(LINE1, LINE2, &oa, ALLOC_OF(X),
                            &oa == X.get_allocator());
                    ASSERTV(LINE1, LINE2, &scratch, ALLOC_OF(Z),
                            &scratch == Z.get_allocator());

                    if ('N' == MEMDST2 && 'Y' == MEMSRC1) {
                        ASSERTV(LINE1, LINE2, oam.isInUseUp());
                    }
                    else if ('Y' == MEMDST2) {
                        ASSERTV(LINE1, LINE2, oam.isInUseSame());
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                        0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj        mX(DTTZ1, TZID1, &oa);
                const Obj& Z = mX;
                const Obj  ZZ(DTTZ1, TZID1, &scratch);

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = Z);
                    ASSERTV(LINE1, ZZ,   Z, ZZ == Z);
                    ASSERTV(LINE1, mR, &mX, mR == &mX);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE1, &oa, ALLOC_OF(Z), &oa == Z.get_allocator());

                ASSERTV(LINE1, oam.isInUseSame());

                ASSERTV(LINE1, sam.isInUseSame());

                ASSERTV(LINE1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }

        // Double check that some object memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that the free 'swap' function is implemented and can
        //   exchange the values of any two objects.  Ensure that member
        //   'swap' is implemented and can exchange the values of any two
        //   objects that use the same allocator.
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
        //:       'Obj', 'mY', having the value described by 'R2'; also use the
        //:       copy constructor to create, using a "scratch" allocator, a
        //:       'const' 'Obj', 'YY', from 'Y'.
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
        //   void swap(LocalDatetime& other);
        //   void swap(LocalDatetime& a, LocalDatetime& b);
        //   CONCERN: Precondition violations are detected when enabled.
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

            (void)memberSwap;  // quash potential compiler warnings
            (void)freeSwap;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        bdlt::DatetimeTz defaultDtz;

        bdlt::Datetime   smallDt; smallDt.addMilliseconds(1);
        bdlt::Datetime    someDt(2011,  5,  3, 15, 32);
        bdlt::Datetime   largeDt(9999, 12, 31, 23, 59, 59, 999);

        bdlt::DatetimeTz smallDtz(smallDt, -(24 * 60 - 1));
        bdlt::DatetimeTz  someDtz( someDt, -( 4 * 60 - 0));
        bdlt::DatetimeTz largeDtz(largeDt,  (24 * 60 - 1));

        const char *defaultTzId = "";
        const char   *smallTzId = "a";
        const char   *largeTzId = LONGEST_STRING;

        const struct {
            int               d_line;  // source line number
            char              d_mem;   // expected allocation: 'Y', 'N', '?'
            bdlt::DatetimeTz *d_datetimeTz_p;
            const char       *d_timeZoneId_p;
        } DATA[] = {

            //LINE  MEM  DTTZ          TZID
            //----  ---  -----------   -----------

            // default (must be first)
            { L_,   'N', &defaultDtz,  defaultTzId },

            // 'datetimeTz'
            { L_,   'N',   &smallDtz,  defaultTzId },
            { L_,   'N',    &someDtz,  defaultTzId },
            { L_,   'N',   &largeDtz,  defaultTzId },

            // 'timeZoneId'
            { L_,   '?', &defaultDtz,    smallTzId },
            { L_,   'Y', &defaultDtz,    largeTzId },

            // other
            { L_,   '?',    &someDtz,    smallTzId },
            { L_,   'Y',    &someDtz,    largeTzId },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int               LINE1 =  DATA[ti].d_line;
            const char              MEM1  =  DATA[ti].d_mem;
            const bdlt::DatetimeTz& DTTZ1 = *DATA[ti].d_datetimeTz_p;
            const char *const       TZID1 =  DATA[ti].d_timeZoneId_p;

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj       mW(DTTZ1, TZID1, &oa);  const Obj& W = mW;
            const Obj XX(W, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE1, Obj(), W, Obj() == W);
                firstFlag = false;
            }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mW.swap(mW);

                ASSERTV(LINE1, XX, W, XX == W);
                ASSERTV(LINE1, &oa == W.get_allocator());
                ASSERTV(LINE1, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mW, mW);

                ASSERTV(LINE1, XX, W, XX == W);
                ASSERTV(LINE1, &oa == W.get_allocator());
                ASSERTV(LINE1, oam.isTotalSame());
            }

            // Verify expected ('Y'/'N') object-memory allocations.

            if ('?' != MEM1) {
                ASSERTV(LINE1, MEM1, oa.numBlocksInUse(),
                        ('N' == MEM1) == (0 == oa.numBlocksInUse()));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int               LINE2 =  DATA[tj].d_line;
                const bdlt::DatetimeTz& DTTZ2 = *DATA[tj].d_datetimeTz_p;
                const char *const       TZID2 =  DATA[tj].d_timeZoneId_p;

                Obj       mX(XX, &oa);            const Obj& X = mX;
                Obj       mY(DTTZ2, TZID2, &oa);  const Obj& Y = mY;
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    mX.swap(mY);

                    ASSERTV(LINE1, LINE2, YY, X, YY == X);
                    ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                    ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                    ASSERTV(LINE1, LINE2, &oa == Y.get_allocator());
                    ASSERTV(LINE1, LINE2, oam.isTotalSame());
                }

                // free function 'swap', same allocator
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    swap(mX, mY);

                    ASSERTV(LINE1, LINE2, XX, X, XX == X);
                    ASSERTV(LINE1, LINE2, YY, Y, YY == Y);
                    ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                    ASSERTV(LINE1, LINE2, &oa == Y.get_allocator());
                    ASSERTV(LINE1, LINE2, oam.isTotalSame());
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

            const bdlt::DatetimeTz A1(bdlt::Datetime(2011, 5, 3, 15), -4 * 60);
            const char             A2[] = "a_" SUFFICIENTLY_LONG_STRING;

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj       mX(&oa);  const Obj& X = mX;
            const Obj XX(X, &scratch);

            Obj       mY(A1, A2, &oa);  const Obj& Y = mY;
            const Obj YY(Y, &scratch);

            if (veryVerbose) { T_ P_(X) P(Y) }

            bslma::TestAllocatorMonitor oam(&oa);

            bslalg::SwapUtil::swap(&mX, &mY);

            ASSERTV(YY, X, YY == X);
            ASSERTV(XX, Y, XX == Y);
            ASSERT(oam.isTotalSame());

            if (veryVerbose) { T_ P_(X) P(Y) }
        }

        // Verify no memory is allocated from the default allocator.

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());

        if (verbose) cout <<
                   "\nFree 'swap' function with different allocators." << endl;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int               LINE1 =  DATA[ti].d_line;
            const bdlt::DatetimeTz& DTTZ1 = *DATA[ti].d_datetimeTz_p;
            const char *const       TZID1 =  DATA[ti].d_timeZoneId_p;

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator     oa2("object2", veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj XX(DTTZ1, TZID1, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P(XX) }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int               LINE2 =  DATA[tj].d_line;
                const bdlt::DatetimeTz& DTTZ2 = *DATA[tj].d_datetimeTz_p;
                const char *const       TZID2 =  DATA[tj].d_timeZoneId_p;

                Obj mX(XX, &oa);             const Obj& X = mX;
                Obj mY(DTTZ2, TZID2, &oa2);  const Obj& Y = mY;

                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                // free function 'swap', different allocators
                {
                    swap(mX, mY);

                    ASSERTV(LINE1, LINE2, YY, X, YY == X);
                    ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                    ASSERTV(LINE1, LINE2, &oa  == X.get_allocator());
                    ASSERTV(LINE1, LINE2, &oa2 == Y.get_allocator());
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

                ASSERT_PASS(mA.swap(mB));
                ASSERT_FAIL(mA.swap(mZ));
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
        //: 4 Supplying a default-constructed allocator has the same effect as
        //:   not supplying an allocator.
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
        //:     configured differently: (a) without passing an allocator,
        //:     (b) passing a default-constructed allocator explicitly, and
        //:     (c) passing the address of a test allocator distinct from the
        //:     default.
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
        //:     4 Use the 'get_allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also use the 'get_allocator'
        //:       accessor of 'X' to verify that its object allocator is
        //:       properly installed, and use the 'get_allocator' accessor of
        //:       'Z' to verify that the allocator that it holds is unchanged.
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
        //   LocalDatetime(const LocalDatetime& o, a = { });
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;

        bdlt::DatetimeTz defaultDtz;

        bdlt::Datetime   smallDt; smallDt.addMilliseconds(1);
        bdlt::Datetime    someDt(2011,  5,  3, 15, 32);
        bdlt::Datetime   largeDt(9999, 12, 31, 23, 59, 59, 999);

        bdlt::DatetimeTz smallDtz(smallDt, -(24 * 60 - 1));
        bdlt::DatetimeTz  someDtz( someDt, -( 4 * 60 - 0));
        bdlt::DatetimeTz largeDtz(largeDt,  (24 * 60 - 1));

        const char *defaultTzId = "";
        const char   *smallTzId = "a";
        const char   *largeTzId = LONGEST_STRING;

        const struct {
            int               d_line;  // source line number
            char              d_mem;   // expected allocation: 'Y', 'N', '?'
            bdlt::DatetimeTz *d_datetimeTz_p;
            const char       *d_timeZoneId_p;
        } DATA[] = {

            //LINE  MEM  DTTZ          TZID
            //----  ---  -----------   -----------

            // default (must be first)
            { L_,   'N', &defaultDtz,  defaultTzId },

            // 'datetimeTz'
            { L_,   'N',   &smallDtz,  defaultTzId },
            { L_,   'N',    &someDtz,  defaultTzId },
            { L_,   'N',   &largeDtz,  defaultTzId },

            // 'timeZoneId'
            { L_,   '?', &defaultDtz,    smallTzId },
            { L_,   'Y', &defaultDtz,    largeTzId },

            // other
            { L_,   '?',    &someDtz,    smallTzId },
            { L_,   'Y',    &someDtz,    largeTzId },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int               LINE =  DATA[ti].d_line;
                const char              MEM  =  DATA[ti].d_mem;
                const bdlt::DatetimeTz& DTTZ = *DATA[ti].d_datetimeTz_p;
                const char *const       TZID =  DATA[ti].d_timeZoneId_p;

                ASSERTV(LINE, MEM, MEM && strchr("YN?", MEM));

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const Obj  Z(DTTZ, TZID, &scratch);
                const Obj ZZ(DTTZ, TZID, &scratch);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr = 0;
                    bslma::TestAllocator *objAllocatorPtr = 0;

                    switch (CONFIG) {
                      case 'a': {
                        objPtr = new (fa) Obj(Z);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj(Z, Obj::allocator_type());
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(Z, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                    // Ensure the first row of the table contains the
                    // default-constructed value.

                    static bool firstFlag = true;
                    if (firstFlag) {
                        ASSERTV(LINE, CONFIG, Obj(), *objPtr,
                                Obj() == *objPtr)
                        firstFlag = false;
                    }

                    // Verify the value of the object.

                    ASSERTV(LINE, CONFIG,  Z, X,  Z == X);

                    // Verify that the value of 'Z' has not changed.

                    ASSERTV(LINE, CONFIG, ZZ, Z, ZZ == Z);

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    ASSERTV(LINE, CONFIG,
                            &oa == X.timeZoneId().get_allocator());

                    // Also invoke the object's 'get_allocator' accessor, as
                    // well as that of 'Z'.

                    ASSERTV(LINE, CONFIG, &oa, ALLOC_OF(X),
                            &oa == X.get_allocator());

                    ASSERTV(LINE, CONFIG, &scratch, ALLOC_OF(Z),
                            &scratch == Z.get_allocator());

                    // Verify no allocation from the non-object allocator.
                    ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    ASSERTV(LINE, CONFIG,
                            oa.numBlocksTotal(), oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Verify expected ('Y'/'N') object-memory allocations.

                    if ('?' != MEM) {
                        ASSERTV(LINE, CONFIG, MEM, oa.numBlocksInUse(),
                                   ('N' == MEM) == (0 == oa.numBlocksInUse()));
                    }

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

            }  // end foreach row

            // Double check that some object memory was allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int               LINE =  DATA[ti].d_line;
                const char              MEM  =  DATA[ti].d_mem;
                const bdlt::DatetimeTz& DTTZ = *DATA[ti].d_datetimeTz_p;
                const char *const       TZID =  DATA[ti].d_timeZoneId_p;

                if (veryVerbose) { T_ P_(MEM) P_(DTTZ) P(TZID) }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const Obj  Z(DTTZ, TZID, &scratch);
                const Obj ZZ(DTTZ, TZID, &scratch);

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(Z, &sa);
                    ASSERTV(LINE, Z, obj, Z == obj);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, ZZ, Z, ZZ == Z);

                ASSERTV(LINE, &scratch, ALLOC_OF(Z),
                        &scratch == Z.get_allocator());
                ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
                ASSERTV(LINE, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
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
        //: 3 No non-salient attributes (i.e., 'get_allocator') participate.
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
        //   bool operator==(const LocalDatetime&, const LocalDatetime&);
        //   bool operator!=(const LocalDatetime&, const LocalDatetime&);
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

        if (verbose) cout <<
            "\nDefine appropriate individual attribute values, 'Ai' and 'Bi'."
                                                                       << endl;

        // Attribute Types

        typedef bdlt::DatetimeTz  T1;  // 'datetimeTz'
        typedef const char       *T2;  // 'timeZoneId'

        // Attribute 1 Values: 'datetimeTz'

        const T1 A1(bdlt::Datetime(2011, 5, 3, 15), -4 * 60);
        const T1 B1(bdlt::Datetime(2011, 5, 3, 15), -5 * 60);

        // Attribute 2 Values: 'timeZoneId'

        const T2 A2 =   LONG_STRING;
        const T2 B2 = LONGER_STRING;

        if (verbose) cout <<
            "\nCreate a table of distinct, but similar object values." << endl;

        struct {
            int       d_line;           // source line number
            const T1 *d_datetimeTz_p;
            const T2  d_timeZoneId_p;
        } DATA[] = {

            // The first row of the table below represents an object value
            // consisting of "baseline" attribute values (A1..An).  Each
            // subsequent row differs (slightly) from the first in exactly one
            // attribute value (Bi).

            //LINE  DTTZ          TZID
            //----  -----------   -----------

            { L_,    &A1,          A2},          // baseline

            { L_,    &B1,          A2},
            { L_,    &A1,          B2},
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE1 =  DATA[ti].d_line;
            const T1& DTTZ1 = *DATA[ti].d_datetimeTz_p;
            const T2  TZID1 =  DATA[ti].d_timeZoneId_p;

            if (veryVerbose) { T_ P_(LINE1) P_(DTTZ1) P(TZID1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const Obj X(DTTZ1, TZID1, &scratch);

                ASSERTV(LINE1, X,   X == X);
                ASSERTV(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int LINE2 =  DATA[tj].d_line;
                const T1& DTTZ2 = *DATA[tj].d_datetimeTz_p;
                const T2  TZID2 =  DATA[tj].d_timeZoneId_p;

                if (veryVerbose) {
                               T_ T_ P_(LINE2) P_(DTTZ2) P(TZID2) }

                const bool EXP = ti == tj;  // expected for equality comparison

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create two distinct test allocators, 'oax' and 'oay'.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    const Obj X(DTTZ1, TZID1, &xa);
                    const Obj Y(DTTZ2, TZID2, &ya);

                    if (veryVerbose) {
                        T_ T_ T_ P_(EXP) P_(CONFIG) P_(X) P(Y) }

                    // Verify value, commutativity, and no memory allocation.

                    bslma::TestAllocatorMonitor oaxm(&oax), oaym(&oay);

                    ASSERTV(LINE1, LINE2, CONFIG, X, Y,  EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG, Y, X,  EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, X, Y, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, Y, X, !EXP == (Y != X));

                    ASSERTV(LINE1, LINE2, CONFIG, oaxm.isTotalSame());
                    ASSERTV(LINE1, LINE2, CONFIG, oaym.isTotalSame());

                    // Double check that some object memory was allocated.

                    ASSERTV(LINE1, LINE2, CONFIG, 1 <= xa.numBlocksInUse());
                    ASSERTV(LINE1, LINE2, CONFIG, 1 <= ya.numBlocksInUse());

                    // Note that memory should be independently allocated for
                    // each attribute capable of allocating memory.
                }
            }

            ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
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
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values.
        //:
        //: 7 The output 'operator<<' signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the supplied 'ostream'.
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
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the
        //:     two formatting parameters, along with the expected output
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2, -8 } --> 8 expected o/ps
        //:     3 { B   } x {  2     } x {  3            } --> 1 expected op
        //:     4 { A B } x { -8     } x { -8            } --> 2 expected o/ps
        //:     4 { A B } x { -9     } x { -9            } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5, 7)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, (-9, -9), then
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
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   operator<<(ostream& s, const LocalDatetime& d);
        //   CONCERN: All accessor methods are declared 'const'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            using namespace baltzo;
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        static const Obj A(bdlt::DatetimeTz(bdlt::Datetime(2011, 5, 3, 15),
                                           -4 * 60),
                           "EDT");

        static const Obj B(bdlt::DatetimeTz(bdlt::Datetime(2011, 1, 9, 10),
                                           2 * 60),
                           "IST");

        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;
            const Obj  *d_object_p;
            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // -------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 }  -->  4 expected o/ps
        // -------------------------------------------------------------------

        //LINE L SPL  OBJ   EXPECTED
        //---- - ---  ---   ---------------------------------------------------

        { L_,  0,  0,  &A,  "["                                              NL
                            "datetimeTz = 03MAY2011_15:00:00.000000-0400"    NL
                            "timeZoneId = \"EDT\""                           NL
                            "]"                                              NL
                                                                             },

        { L_,  0,  1,  &A,  "["                                              NL
                            " datetimeTz = 03MAY2011_15:00:00.000000-0400"   NL
                            " timeZoneId = \"EDT\""                          NL
                            "]"                                              NL
                                                                             },

        { L_,  0, -1,  &A,  "["                                              SP
                            "datetimeTz = 03MAY2011_15:00:00.000000-0400"    SP
                            "timeZoneId = \"EDT\""                           SP
                            "]"
                                                                             },

        { L_,  0, -8,  &A, "["                                               NL
                           "    datetimeTz = 03MAY2011_15:00:00.000000-0400" NL
                           "    timeZoneId = \"EDT\""                        NL
                           "]"                                               NL
                                                                             },
        // -------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2 }      -->  8 expected o/ps
        // -------------------------------------------------------------------

        //LINE L SPL  OBJ   EXPECTED
        //---- - ---  ---   ---------------------------------------------------

        { L_,  3,  0,  &A,  "["                                              NL
                            "datetimeTz = 03MAY2011_15:00:00.000000-0400"    NL
                            "timeZoneId = \"EDT\""                           NL
                            "]"                                              NL
                                                                             },

        { L_,  3,  2,  &A,
                       "      ["                                             NL
                       "        datetimeTz = 03MAY2011_15:00:00.000000-0400" NL
                       "        timeZoneId = \"EDT\""                        NL
                       "      ]"                                             NL
                                                                             },

        { L_,  3, -2,  &A,  "      ["                                        SP
                            "datetimeTz = 03MAY2011_15:00:00.000000-0400"    SP
                            "timeZoneId = \"EDT\""                           SP
                            "]"
                                                                             },

        { L_,  3, -8,  &A,
               "            ["                                               NL
               "                datetimeTz = 03MAY2011_15:00:00.000000-0400" NL
               "                timeZoneId = \"EDT\""                        NL
               "            ]"                                               NL
                                                                             },

        { L_, -3,  0,  &A,  "["                                              NL
                            "datetimeTz = 03MAY2011_15:00:00.000000-0400"    NL
                            "timeZoneId = \"EDT\""                           NL
                            "]"                                              NL
                                                                             },

        { L_, -3,  2,  &A,
                       "["                                                   NL
                       "        datetimeTz = 03MAY2011_15:00:00.000000-0400" NL
                       "        timeZoneId = \"EDT\""                        NL
                       "      ]"                                             NL
                                                                             },

        { L_, -3, -2,  &A,  "["                                              SP
                            "datetimeTz = 03MAY2011_15:00:00.000000-0400"    SP
                            "timeZoneId = \"EDT\""                           SP
                            "]"
                                                                             },

        { L_, -3, -8,  &A,
               "["                                                           NL
               "                datetimeTz = 03MAY2011_15:00:00.000000-0400" NL
               "                timeZoneId = \"EDT\""                        NL
               "            ]"                                               NL
                                                                             },

        // ------------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }             -->  1 expected o/p
        // ------------------------------------------------------------------

        //LINE L SPL  OBJ   EXPECTED
        //---- - ---  ---   ---------------------------------------------------

        { L_,  2,  3,  &B,
                      "      ["                                              NL
                      "         datetimeTz = 09JAN2011_10:00:00.000000+0200" NL
                      "         timeZoneId = \"IST\""                        NL
                      "      ]"                                              NL
                                                                             },
        // -------------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }  x { -8 }            -->  2 expected o/ps
        // -------------------------------------------------------------------

        //LINE L SPL  OBJ   EXPECTED
        //---- - ---  ---   ---------------------------------------------------

        { L_, -8, -8,  &A,
                           "["                                               NL
                           "    datetimeTz = 03MAY2011_15:00:00.000000-0400" NL
                           "    timeZoneId = \"EDT\""                        NL
                           "]"                                               NL
                                                                             },

        { L_, -8, -8,  &B,
                           "["                                               NL
                           "    datetimeTz = 09JAN2011_10:00:00.000000+0200" NL
                           "    timeZoneId = \"IST\""                        NL
                           "]"                                               NL
                                                                             },
        // -------------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }  x { -9 }            -->  2 expected o/ps
        // -------------------------------------------------------------------

        //LINE L SPL  OBJ   EXPECTED
        //---- - ---  ---   ---------------------------------------------------

        { L_, -9, -9,  &A,  "["                                              SP
                            "03MAY2011_15:00:00.000000-0400"                 SP
                            "\"EDT\""                                        SP
                            "]"
                                                                             },

        { L_, -9, -9,  &B,  "["                                              SP
                            "09JAN2011_10:00:00.000000+0200"                 SP
                            "\"IST\""                                        SP
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
                const int   LINE =  DATA[ti].d_line;
                const int   L    =  DATA[ti].d_level;
                const int   SPL  =  DATA[ti].d_spacesPerLevel;
                const Obj&  OBJ  = *DATA[ti].d_object_p;
                const char *EXP  =  DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(L) P_(SPL) P(OBJ) }

                if (veryVeryVerbose) { T_ T_ Q(EXP) cout << EXP; }

                ostringstream os;

                // Verify supplied stream is returned by reference.

                if (-9 == L && -9 == SPL) {
                    ASSERTV(LINE, &os == &(os << OBJ));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    ASSERTV(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        ASSERTV(LINE, &os == &OBJ.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        ASSERTV(LINE, &os == &OBJ.print(os, L));
                    }
                    else {
                        ASSERTV(LINE, &os == &OBJ.print(os));
                    }

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                // Verify output is formatted as expected.

                if (veryVeryVerbose) { P(os.str()) }

                ASSERTV(LINE, EXP, os.str(), EXP == os.str());
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
        //:
        //:   2 Use the corresponding basic accessor to verify the new
        //:     expected value.  (C-1)
        //:
        //:   3 Monitor the memory allocated from both the default and object
        //:     allocators before and after calling the accessor; verify that
        //:     there is no change in total memory allocation.  (C-3..4)
        //
        // Testing:
        //   allocator_type get_allocator() const;
        //   bslma::Allocator *allocator() const;
        //   const bdlt::DatetimeTz& datetimeTz() const;
        //   const bsl::string& timeZoneId() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // Attribute Types

        typedef bdlt::DatetimeTz T1;  // 'datetimeTz'
        typedef bsl::string      T2;  // 'timeZoneId'

        // -----------------------------------------------------
        // 'D' values: These are the default-constructed values.
        // -----------------------------------------------------

        const T1 D1;       // default value
        const T2 D2 = "";  // default value

        // -------------------------------------------------------
        // 'A' values: Should cause memory allocation if possible.
        // -------------------------------------------------------

        const T1 A1(bdlt::Datetime(2011, 5, 3, 15), -4 * 60);
        const T2 A2 = "a_" SUFFICIENTLY_LONG_STRING;

        if (verbose) cout <<
           "\nCreate two test allocators; install one as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
        bsl::allocator<char> oaa(&oa);

        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
                 "\nCreate an object, passing in the other allocator." << endl;

        Obj mX(&oa);  const Obj& X = mX;

        if (verbose) cout <<
                "\nVerify all basic accessors report expected values." << endl;
        {
            const T1& datetimeTz = X.datetimeTz();
            ASSERTV(D1, datetimeTz, D1 == datetimeTz);

            const T2 timeZoneId = X.timeZoneId();
            ASSERTV(D2, timeZoneId, D2 == timeZoneId);

            ASSERTV(ALLOC_OF(X),   oaa == X.get_allocator());
            ASSERTV(X.allocator(), &oa == X.allocator());
        }

        if (verbose) cout <<
            "\nApply primary manipulators and verify expected values." << endl;

        if (veryVerbose) { T_ Q(datetimeTz) }
        {
            mX.setDatetimeTz(A1);

            bslma::TestAllocatorMonitor oam(&oa), dam(&da);

            const T1& datetimeTz = X.datetimeTz();
            ASSERTV(A1, datetimeTz, A1 == datetimeTz);

            ASSERT(oam.isInUseSame());  ASSERT(dam.isInUseSame());
        }

        if (veryVerbose) { T_ Q(timeZoneId) }
        {
            mX.setTimeZoneId(A2);

            bslma::TestAllocatorMonitor oam(&oa), dam(&da);

            const T2& timeZoneId = X.timeZoneId();
            ASSERTV(A2, timeZoneId, A2 == timeZoneId);

            ASSERT(oam.isTotalSame());  ASSERT(dam.isTotalSame());
        }

        // Double check that some object memory was allocated.

        ASSERT(1 <= oa.numBlocksTotal());

        // Note that memory should be independently allocated for each
        // attribute capable of allocating memory.

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());

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
        //: 6 Supplying a default-constructed allocator has the same effect as
        //:   not supplying an allocator.
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
        //:     default-constructed allocator explicitly, and (c) passing the
        //:     address of a test allocator distinct from the default
        //:     allocator.
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
        //:     4 Use the 'get_allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also invoke the (as yet
        //:       unproven) 'get_allocator' accessor of the object under test.
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
        //
        // Testing:
        //   LocalDatetime(DatetimeTz& d, const StringRef& t, a = { });
        //   LocalDatetime(DatetimeTz& d, const char *t, a = { });
        //   CONCERN: All creator/manipulator ptr./ref. parameters are 'const'.
        //   CONCERN: String arguments can be either 'char *' or 'string'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CTOR" << endl
                          << "==========" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;

        bdlt::DatetimeTz defaultDtz;

        bdlt::Datetime   smallDt; smallDt.addMilliseconds(1);
        bdlt::Datetime    someDt(2011,  5,  3, 15, 32);
        bdlt::Datetime   largeDt(9999, 12, 31, 23, 59, 59, 999);

        bdlt::DatetimeTz smallDtz(smallDt, -(24 * 60 - 1));
        bdlt::DatetimeTz  someDtz( someDt, -( 4 * 60 - 0));
        bdlt::DatetimeTz largeDtz(largeDt,  (24 * 60 - 1));

        const char *defaultTzId = 0;
        const char   *smallTzId = "a";
        const char   *largeTzId = LONGEST_STRING;

        const struct {
            int               d_line;  // source line number
            char              d_mem;   // expected allocation: 'Y', 'N', '?'
            bdlt::DatetimeTz *d_datetimeTz_p;
            const char       *d_timeZoneId_p;
        } DATA[] = {

            //LINE  MEM  DTTZ          TZID
            //----  ---  -----------   -----------

            // default (must be first)
            { L_,   'N', &defaultDtz,  defaultTzId },

            // 'datetimeTz'
            { L_,   'N',   &smallDtz,  defaultTzId },
            { L_,   'N',    &someDtz,  defaultTzId },
            { L_,   'N',   &largeDtz,  defaultTzId },

            // 'timeZoneId'
            { L_,   '?', &defaultDtz,    smallTzId },
            { L_,   'Y', &defaultDtz,    largeTzId },

            // other
            { L_,   '?',    &someDtz,    smallTzId },
            { L_,   'Y',    &someDtz,    largeTzId },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int               LINE   =  DATA[ti].d_line;
                const char              MEM    =  DATA[ti].d_mem;
                const bdlt::DatetimeTz& DTTZ   = *DATA[ti].d_datetimeTz_p;
                const char *const       TZID   =  DATA[ti].d_timeZoneId_p;
                const bsl::string_view  TZIDSV =  DATA[ti].d_timeZoneId_p
                                               ?  DATA[ti].d_timeZoneId_p
                                               :  "";

                if (veryVerbose) { T_ P_(MEM) P_(DTTZ) P(TZID) }

                ASSERTV(LINE, MEM, MEM && strchr("YN?", MEM));

                for (char cfg = 'a'; cfg <= 'f'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    if (veryVerbose) { T_ T_ P(CONFIG) }

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr = 0;
                    bslma::TestAllocator *objAllocatorPtr = 0;

                    switch (CONFIG) {
                      case 'a': {
                        objPtr = new (fa) Obj(DTTZ, TZID);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj(DTTZ, TZID,
                                              Obj::allocator_type());
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(DTTZ, TZID, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      case 'd': {
                        objPtr = new (fa) Obj(DTTZ, TZIDSV);
                        objAllocatorPtr = &da;
                      } break;
                      case 'e': {
                        objPtr = new (fa) Obj(DTTZ, TZIDSV,
                                              Obj::allocator_type());
                        objAllocatorPtr = &da;
                      } break;
                      case 'f': {
                        objPtr = new (fa) Obj(DTTZ, TZIDSV, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        ASSERTV(LINE, CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = &sa == objAllocatorPtr
                                              ? da : sa;

                    // Use untested functionality to help ensure the first row
                    // of the table contains the default-constructed value.

                    static bool firstFlag = true;
                    if (firstFlag) {
                        ASSERTV(LINE, CONFIG, Obj(), *objPtr,
                                Obj() == *objPtr)
                        firstFlag = false;
                    }

                    // -------------------------------------
                    // Verify the object's attribute values.
                    // -------------------------------------

                    ASSERTV(LINE, CONFIG, DTTZ, X.datetimeTz(),
                            DTTZ == X.datetimeTz());

                    ASSERTV(LINE, CONFIG, TZID, X.timeZoneId(),
                            TZIDSV == X.timeZoneId());

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    ASSERTV(LINE, CONFIG,
                            &oa == X.timeZoneId().get_allocator());

                    // Also invoke the object's 'get_allocator' accessor.

                    ASSERTV(LINE, CONFIG, &oa, ALLOC_OF(X),
                            &oa == X.get_allocator());

                    // Verify no allocation from the non-object allocator.

                    ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    ASSERTV(LINE, CONFIG,
                            oa.numBlocksTotal(), oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Verify expected ('Y'/'N') object-memory allocations.

                    if ('?' != MEM) {
                        ASSERTV(LINE, CONFIG, MEM, oa.numBlocksInUse(),
                                   ('N' == MEM) == (0 == oa.numBlocksInUse()));
                    }

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
                const int               LINE   =  DATA[ti].d_line;
                const char              MEM    =  DATA[ti].d_mem;
                const bdlt::DatetimeTz& DTTZ   = *DATA[ti].d_datetimeTz_p;
                const char *const       TZID   =  DATA[ti].d_timeZoneId_p;
                const bsl::string_view  TZIDSV =  DATA[ti].d_timeZoneId_p
                                               ?  DATA[ti].d_timeZoneId_p
                                               :  "";

                if (veryVerbose) { T_ P_(MEM) P_(DTTZ) P(TZID) }

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(DTTZ, TZID, &sa);
                    ASSERTV(LINE, DTTZ, obj.datetimeTz(),
                            DTTZ == obj.datetimeTz());
                    ASSERTV(LINE, TZID, obj.timeZoneId(),
                            TZIDSV == obj.timeZoneId());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
                ASSERTV(LINE, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
            }
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
        //: 4 Supplying a default-constructed allocator has the same effect as
        //:   not supplying an allocator.
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
        //:   an allocator, (b) passing a default-constructed allocator
        //:   explicitly, and (c) passing the address of a test allocator
        //:   distinct from the default.  For each of these three iterations:
        //:   (C-1..14)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     as the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object
        //:     'X', with its object allocator configured appropriately (see
        //:     P-2); use a distinct test allocator for the object's footprint.
        //:
        //:   3 Use the 'get_allocator' accessor of each underlying attribute
        //:     capable of allocating memory to ensure that its object
        //:     allocator is properly installed; also invoke the (as yet
        //:     unproven) 'get_allocator' accessor of the object under test.
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
        //:
        //:   7 Corroborate that attributes are modifiable independently by
        //:     first setting all of the attributes to their 'A' values.  Then
        //:     incrementally set each attribute to it's corresponding  'B'
        //:     value and verify after each manipulation that only that
        //:     attribute's value changed.  (C-10)
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
        //   LocalDatetime();
        //   LocalDatetime(const allocator_type& a);
        //   ~LocalDatetime();
        //   setDatetimeTz(const bdlt::DatetimeTz& value);
        //   setTimeZoneId(const char *value);
        //   setTimeZoneId(const StringRef& value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const bdlt::DatetimeTz  D1   = bdlt::DatetimeTz();  // default value
        const char             *D2   = 0;                    // default value
        const bsl::string_view  D2SV;

        // 'A' values: Should cause memory allocation if possible.

        const bdlt::DatetimeTz  A1(bdlt::Datetime(2011, 5, 3, 15), -4 * 60);
        const char             *A2 = "a_" SUFFICIENTLY_LONG_STRING;

        // 'B' values: Should NOT cause allocation (use alternate string type).

        const bdlt::Datetime    maxDatetime(9999, 12, 31, 23, 59, 59, 999);
        const bdlt::DatetimeTz  B1 = bdlt::DatetimeTz(maxDatetime, 1440 - 1);
        const bsl::string_view  B2 = "xyz";

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr(0);
            bslma::TestAllocator *objAllocatorPtr(0);

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
                ASSERTV(CONFIG, !"Bad allocator config.");
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // -------------------------------------------------------
            // Verify any attribute allocators are installed properly.
            // -------------------------------------------------------

            ASSERTV(CONFIG, &oa == X.timeZoneId().get_allocator());

            // Also invoke the object's 'get_allocator' accessor.

            ASSERTV(CONFIG, &oa, ALLOC_OF(X), &oa == X.get_allocator());

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(CONFIG,  oa.numBlocksTotal(), 0 ==  oa.numBlocksTotal());
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // -------------------------------------
            // Verify the object's attribute values.
            // -------------------------------------

            ASSERTV(CONFIG, D1, X.datetimeTz(), D1   == X.datetimeTz());
            ASSERTV(CONFIG, D2, X.timeZoneId(), D2SV == X.timeZoneId());

            // -----------------------------------------------------
            // Verify that each attribute is independently settable.
            // -----------------------------------------------------

            // 'datetimeTz'
            {
                bslma::TestAllocatorMonitor tam(&oa);

                mX.setDatetimeTz(A1);
                ASSERTV(CONFIG, A1   == X.datetimeTz());
                ASSERTV(CONFIG, D2SV == X.timeZoneId());

                mX.setDatetimeTz(B1);
                ASSERTV(CONFIG, B1   == X.datetimeTz());
                ASSERTV(CONFIG, D2SV == X.timeZoneId());

                mX.setDatetimeTz(D1);
                ASSERTV(CONFIG, D1   == X.datetimeTz());
                ASSERTV(CONFIG, D2SV == X.timeZoneId());

                ASSERTV(CONFIG, tam.isTotalSame());
            }

            // 'timeZoneId'
            {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    bslma::TestAllocatorMonitor tam(&oa);
                    mX.setTimeZoneId(A2);
                    ASSERTV(CONFIG, tam.isInUseUp());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERTV(CONFIG, D1 == X.datetimeTz());
                ASSERTV(CONFIG, A2 == X.timeZoneId());

                bslma::TestAllocatorMonitor tam(&oa);

                mX.setTimeZoneId(B2);
                ASSERTV(CONFIG, D1 == X.datetimeTz());
                ASSERTV(CONFIG, B2 == X.timeZoneId());

                mX.setTimeZoneId(D2);
                ASSERTV(CONFIG, D1   == X.datetimeTz());
                ASSERTV(CONFIG, D2SV == X.timeZoneId());

                ASSERTV(CONFIG, tam.isTotalSame());
            }

            // Corroborate attribute independence.
            {
                // Set all attributes to their 'A' values.

                mX.setDatetimeTz(A1);
                mX.setTimeZoneId(A2);

                ASSERTV(CONFIG, A1 == X.datetimeTz());
                ASSERTV(CONFIG, A2 == X.timeZoneId());

                // Set all attributes to their 'B' values.

                mX.setDatetimeTz(B1);
                ASSERTV(CONFIG, B1 == X.datetimeTz());
                ASSERTV(CONFIG, A2 == X.timeZoneId());

                mX.setTimeZoneId(B2);
                ASSERTV(CONFIG, B1 == X.datetimeTz());
                ASSERTV(CONFIG, B2 == X.timeZoneId());
            }

            // Verify no temporary memory is allocated from the object
            // allocator.

            ASSERTV(CONFIG, oa.numBlocksMax(), 1 == oa.numBlocksMax());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
            ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
            ASSERTV(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Double check that some object memory was allocated.

            ASSERTV(CONFIG, 1 <= oa.numBlocksTotal());

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
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

        // Attribute Types

        typedef bdlt::DatetimeTz  T1; // 'datetimeTz'
        typedef const char      *T2; // 'timeZoneId'

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Values for testing

        // Attribute 1 Values: 'datetimeTz'

        const T1 D1;  // default value
        const T1 A1(bdlt::Datetime(2011, 5, 3, 15), -4 * 60);

        // Attribute 2 Values: 'timeZoneId'

        const T2 D2 = "";                 // default value
        const T2 A2 = "America/New_York";

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (default ctor)."
                             "\t\t{ w:D             }" << endl;

        Obj mW;  const Obj& W = mW;

        if (veryVerbose) cout << "\ta. Check initial value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.datetimeTz());
        ASSERT(D2 == W.timeZoneId());

        if (veryVerbose) cout <<
                  "\tb. Try equality operators: 'w' <op> 'w'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                             "\t\t{ w:D x:D         }" << endl;

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) cout << "\ta. Check initial value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(D1 == X.datetimeTz());
        ASSERT(D2 == X.timeZoneId());

        if (veryVerbose) cout <<
                   "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 'D')."
                             "\t\t{ w:D x:A         }" << endl;

        mX.setDatetimeTz(A1);
        mX.setTimeZoneId(A2);

        if (veryVerbose) cout << "\ta. Check new value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.datetimeTz());
        ASSERT(A2 == X.timeZoneId());

        if (veryVerbose) cout <<
             "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create an object 'y' (init. to 'A')."
                             "\t\t{ w:D x:A y:A     }" << endl;

        Obj mY(A1, A2);  const Obj& Y = mY;

        if (veryVerbose) cout << "\ta. Check initial value of 'y'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Y) }

        ASSERT(A1 == Y.datetimeTz());
        ASSERT(A2 == Y.timeZoneId());

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

        ASSERT(A1 == Z.datetimeTz());
        ASSERT(A2 == Z.timeZoneId());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set 'z' to 'D' (the default value)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;

        mZ.setDatetimeTz(D1);
        mZ.setTimeZoneId(D2);

        if (veryVerbose) cout << "\ta. Check new value of 'z'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(D1 == Z.datetimeTz());
        ASSERT(D2 == Z.timeZoneId());

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

        ASSERT(A1 == W.datetimeTz());
        ASSERT(A2 == W.timeZoneId());

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

        ASSERT(D1 == W.datetimeTz());
        ASSERT(D2 == W.timeZoneId());

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

        ASSERT(A1 == X.datetimeTz());
        ASSERT(A2 == X.timeZoneId());

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

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
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
