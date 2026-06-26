// ball_hierarchicalcategorysetting.t.cpp                             -*-C++-*-
#include <ball_hierarchicalcategorysetting.h>

#include <ball_severity.h>                                  // for testing only
#include <ball_thresholdaggregate.h>

#include <bdlsb_memoutstreambuf.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>
#include <bslmf_movableref.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_review.h>
#include <bsls_types.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The component under test implements a single, simply constrained
// (value-semantic) attribute class.  The Primary Manipulators and Basic
// Accessors are therefore, respectively, the attribute setters and getters,
// each of which follows our standard unconstrained attribute-type naming
// conventions: `setLevels` and level accessor methods.
//
// Primary Manipulators:
//  - `setLevels`
//
// Basic Accessors:
//  - `get_allocator` (orthogonal to value)
//  - `categoryPrefix`
//  - `recordLevel`
//  - `passLevel`
//  - `triggerLevel`
//  - `triggerAllLevel`
//
// This particular attribute class also provides a value constructor capable of
// creating an object in any state relevant for thorough testing, obviating the
// primitive generator function, `gg`, normally used for this purpose.  We will
// therefore follow our standard 10-case approach to testing value-semantic
// types except that we will test the value constructor in case 3 (in lieu of
// the generator function), with the default constructor and primary
// manipulators tested fully in case 2.
//
// Certain standard value-semantic-type test cases are omitted:
//  - [10] -- BSLX streaming is not (yet) implemented for this class.
//
// Global Concerns:
//  - The test driver is robust w.r.t. reuse in other, similar components.
//  - ACCESSOR methods are declared `const`.
//  - CREATOR & MANIPULATOR pointer/reference parameters are declared `const`.
//  - No memory is ever allocated from the global allocator.
//  - Any allocated memory is always from the object allocator.
//  - An object's value is independent of the allocator used to supply memory.
//  - Injected exceptions are safely propagated during memory allocation.
//  - Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//  - All explicit memory allocations are presumed to use the global, default,
//    or object allocator.
//  - ACCESSOR methods are `const` thread-safe.
//  - Individual attribute types are presumed to be *alias-safe*; hence, only
//    certain methods require the testing of this property:
//    - copy-assignment
//    - swap
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] HierarchicalCategorySetting(prefix, r, p, t, ta, a = {});
// [ 2] HierarchicalCategorySetting(prefix, levels, a = {});
// [ 7] HierarchicalCategorySetting(const HCS& original, a = {});
// [ 8] HierarchicalCategorySetting(MovableRef<HCS> original);
// [ 8] HierarchicalCategorySetting(MovableRef<HCS> original, a);
// [ 2] ~HierarchicalCategorySetting();
//
// MANIPULATORS
// [ 9] operator=(const HierarchicalCategorySetting& rhs);
// [11] operator=(bslmf::MovableRef<HierarchicalCategorySetting> rhs);
// [ 2] int setLevels(int r, int p, int t, int ta);
//
// ACCESSORS
// [ 4] allocator_type get_allocator() const;
// [ 4] bsl::string_view categoryPrefix() const;
// [ 4] int recordLevel() const;
// [ 4] int passLevel() const;
// [ 4] int triggerLevel() const;
// [ 4] int triggerAllLevel() const;
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const HCS& lhs, const HCS& rhs);
// [ 6] bool operator!=(const HCS& lhs, const HCS& rhs);
// [ 5] operator<<(ostream& s, const HierarchicalCategorySetting& d);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ 3] CONCERN: All creator/manipulator ptr./ref. parameters are `const`.
// [ 4] CONCERN: All accessor methods are declared `const`.
// [ 9] CONCERN: All memory allocation is from the object's allocator.
// [ 9] CONCERN: All memory allocation is exception neutral.
// [ 9] CONCERN: Object value is independent of the object allocator.
// [ 9] CONCERN: There is no temporary allocation from any allocator.
// [10] Reserved for `bslx` streaming.

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::HierarchicalCategorySetting Obj;

// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define `bsl::string` value long enough to ensure dynamic memory allocation.

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

// Define DEFAULT DATA used by test cases 3, 7, 8, and 9.

struct DefaultDataRow {
    int         d_line;           // source line number
    const char *d_categoryPrefix;
    int         d_recordLevel;
    int         d_passLevel;
    int         d_triggerLevel;
    int         d_triggerAllLevel;
};

static
const DefaultDataRow DEFAULT_DATA[] =
{
    //LINE  PREFIX           REC  PASS  TRIG  TALL
    //----  ------           ---  ----  ----  ----

    // default (must be first)
    { L_,   "",                0,   64,    0,    0  },

    // `categoryPrefix`
    { L_,   "a",               0,   64,    0,    0  },
    { L_,   "AB",              0,   64,    0,    0  },
    { L_,   "EQ",              0,   64,    0,    0  },
    { L_,   "EQ.MARKET",       0,   64,    0,    0  },
    { L_,   "1234567890",      0,   64,    0,    0  },
    { L_,   LONG_STRING,       0,   64,    0,    0  },
    { L_,   LONGER_STRING,     0,   64,    0,    0  },
    { L_,   LONGEST_STRING,    0,   64,    0,    0  },

    // threshold levels
    { L_,   "",                0,    0,    0,    0  },
    { L_,   "",                1,    2,    3,    4  },
    { L_,   "",              255,  255,  255,  255  },
    { L_,   "",              192,   96,   64,   32  },
    { L_,   "",              160,   80,   40,   20  },

    // combinations
    { L_,   "EQ",            192,   96,   64,   32  },
    { L_,   "EQ.MARKET",     193,   97,   65,   33  },
    { L_,   LONG_STRING,     255,  255,  255,  255  },
};
enum { DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA };

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int  test                = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose             = argc > 2;
    const bool veryVerbose         = argc > 3;  (void)veryVerbose;
    const bool veryVeryVerbose     = argc > 4;  (void)veryVeryVerbose;
    const bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: This test driver is reusable w/other, similar components.

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);

    switch (test) { case 0:
      case 12: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE"
                             "\n=============\n";

///Example 1: Creating and Accessing Hierarchical Category Settings
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates creating a hierarchical category
// setting and accessing its properties.
//
// First, we create a hierarchical category setting for the category prefix
// "EQ" with specific threshold levels:
// ```
    ball::HierarchicalCategorySetting setting("EQ",
                                              192,  // recordLevel
                                              96,   // passLevel
                                              64,   // triggerLevel
                                              32);  // triggerAllLevel
// ```
// Then, we can query the category prefix and threshold levels:
// ```
    ASSERT("EQ" == setting.categoryPrefix());
    ASSERT(192  == setting.recordLevel());
    ASSERT(96   == setting.passLevel());
    ASSERT(64   == setting.triggerLevel());
    ASSERT(32   == setting.triggerAllLevel());
// ```
// Finally, we can modify the threshold levels:
// ```
    setting.setLevels(200, 100, 80, 40);

    ASSERT(200 == setting.recordLevel());
    ASSERT(100 == setting.passLevel());
    ASSERT(80  == setting.triggerLevel());
    ASSERT(40  == setting.triggerAllLevel());
// ```
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, AND DTOR
        //   Ensure that we can use the default constructor to create an
        //   object, use the primary manipulators to set its value, and use
        //   the destructor to destroy it.
        //
        // Concerns:
        // 1. An object created with the default constructor (with or without
        //    a supplied allocator) has default values for its attributes.
        //
        // 2. If an allocator is NOT supplied to the default constructor, the
        //    default allocator in effect at the time of construction becomes
        //    the object allocator for the resulting object.
        //
        // 3. If an allocator IS supplied to the default constructor, that
        //    allocator becomes the object allocator for the resulting object.
        //
        // 4. Supplying a null allocator address has the same effect as not
        //    supplying an allocator.
        //
        // 5. The 'setLevels' method sets the threshold levels correctly.
        //
        // 6. The 'setLevels' method returns 0 on success and non-zero when
        //    any level is out of range [0..255].
        //
        // 7. Any memory allocation is from the object allocator.
        //
        // 8. There is no temporary memory allocation from any allocator.
        //
        // 9. Every object releases any allocated memory at destruction.
        //
        // Plan:
        // 1. Using a loop-based approach, default-construct three distinct
        //    objects, in turn, but configured differently: (a) without
        //    passing an allocator, (b) passing a null allocator address
        //    explicitly, and (c) passing the address of a test allocator
        //    distinct from the default allocator.  For each of these three
        //    iterations:  (C-1..4, 7..9)
        //
        //    1. Create three 'bslma::TestAllocator' objects, and install one
        //       as the current default allocator (note that a ubiquitous test
        //      allocator is already installed as the global allocator).
        //
        //    2. Use the default constructor to dynamically create an object
        //       'X', with its object allocator configured appropriately (see
        //       P-1); use a distinct test allocator for the object's
        //       footprint.
        //
        //    3. Use the (as yet unproven) 'get_allocator' and 'categoryPrefix'
        //       accessors to verify that the default values are correct.
        //       (C-1)
        //
        //    4. Use the 'setLevels' manipulator to set the object's value.
        //       (C-5..6)
        //
        //    5. Verify that all of the attributes of 'X' have their expected
        //       values.  (C-5..6)
        //
        //    6. Verify that the 'setLevels' method correctly rejects invalid
        //       values (< 0 or > 255).  (C-6)
        //
        //    7. Verify that no temporary memory is allocated from the object
        //       allocator.  (C-8)
        //
        //    8. Verify that all object memory is released when the object is
        //       destroyed.  (C-9)
        //
        // Testing:
        //   HierarchicalCategorySetting(bslma::Allocator *ba = 0);
        //   ~HierarchicalCategorySetting();
        //   int setLevels(int, int, int, int);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nDEFAULT CTOR, PRIMARY MANIPULATORS, AND DTOR"
                    "\n============================================\n";

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            if (veryVerbose)
                  cout << "\tTesting with various allocator configurations.\n";

            Obj                  *objPtr = 0;
            bslma::TestAllocator *objAllocatorPtr = 0;

            switch (CONFIG) {
              case 'a': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj("", 0, 64, 0, 0);
              } break;
              case 'b': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj("", 0, 64, 0, 0, 0);
              } break;
              case 'c': {
                objAllocatorPtr = &sa;
                objPtr = new (fa) Obj("", 0, 64, 0, 0, objAllocatorPtr);
              } break;
              default: {
                ASSERTV(CONFIG, 0 == "Bad allocator config.");
              } break;
            }
            ASSERTV(CONFIG, sizeof(Obj) == fa.numBytesInUse());

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

            // Verify initial values.

            ASSERTV(CONFIG, X.categoryPrefix().empty());
            ASSERTV(CONFIG,   0 == X.recordLevel());
            ASSERTV(CONFIG,  64 == X.passLevel());
            ASSERTV(CONFIG,   0 == X.triggerLevel());
            ASSERTV(CONFIG,   0 == X.triggerAllLevel());

            ASSERTV(CONFIG, &oa == X.get_allocator());

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(CONFIG, oa.numBlocksTotal(),
                    0 ==  oa.numBlocksTotal());
            ASSERTV(CONFIG, noa.numBlocksTotal(),
                    0 == noa.numBlocksTotal());

            // Set valid threshold levels and verify.

            ASSERTV(CONFIG, 0 == mX.setLevels(192, 96, 64, 32));

            ASSERTV(CONFIG, 192 == X.recordLevel());
            ASSERTV(CONFIG,  96 == X.passLevel());
            ASSERTV(CONFIG,  64 == X.triggerLevel());
            ASSERTV(CONFIG,  32 == X.triggerAllLevel());

            // Test with boundary values.

            ASSERTV(CONFIG, 0 == mX.setLevels(0, 0, 0, 0));

            ASSERTV(CONFIG, 0 == X.recordLevel());
            ASSERTV(CONFIG, 0 == X.passLevel());
            ASSERTV(CONFIG, 0 == X.triggerLevel());
            ASSERTV(CONFIG, 0 == X.triggerAllLevel());

            ASSERTV(CONFIG, 0 == mX.setLevels(255, 255, 255, 255));

            ASSERTV(CONFIG, 255 == X.recordLevel());
            ASSERTV(CONFIG, 255 == X.passLevel());
            ASSERTV(CONFIG, 255 == X.triggerLevel());
            ASSERTV(CONFIG, 255 == X.triggerAllLevel());

            // Test invalid values (should fail and leave values unchanged).

            ASSERTV(CONFIG, 0 != mX.setLevels(-1, 100, 100, 100));
            ASSERTV(CONFIG, 255 == X.recordLevel());  // unchanged

            ASSERTV(CONFIG, 0 != mX.setLevels(100, -1, 100, 100));
            ASSERTV(CONFIG, 255 == X.passLevel());  // unchanged

            ASSERTV(CONFIG, 0 != mX.setLevels(100, 100, -1, 100));
            ASSERTV(CONFIG, 255 == X.triggerLevel());  // unchanged

            ASSERTV(CONFIG, 0 != mX.setLevels(100, 100, 100, -1));
            ASSERTV(CONFIG, 255 == X.triggerAllLevel());  // unchanged

            ASSERTV(CONFIG, 0 != mX.setLevels(256, 100, 100, 100));
            ASSERTV(CONFIG, 255 == X.recordLevel());  // unchanged

            ASSERTV(CONFIG, 0 != mX.setLevels(100, 256, 100, 100));
            ASSERTV(CONFIG, 255 == X.passLevel());  // unchanged

            ASSERTV(CONFIG, 0 != mX.setLevels(100, 100, 256, 100));
            ASSERTV(CONFIG, 255 == X.triggerLevel());  // unchanged

            ASSERTV(CONFIG, 0 != mX.setLevels(100, 100, 100, 256));
            ASSERTV(CONFIG, 255 == X.triggerAllLevel());  // unchanged

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(CONFIG, fa.numBlocksInUse(),  0 == fa.numBlocksInUse());
            ASSERTV(CONFIG, oa.numBlocksInUse(),  0 == oa.numBlocksInUse());
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // MOVE-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the target object subsequently
        //   has the source value, and the source object is left in a valid
        //   unspecified state.
        //
        // Concerns:
        // 1. The move assignment operator can change the value of a modifiable
        //    target object to that of any source object.
        //
        // 2. The source object is left in a valid but unspecified state.
        //
        // `3 The allocator used by the target object is unchanged.
        //
        // 4. Any memory allocation is from the target object's allocator.
        //
        // 5. Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        // 1. Use the address of 'operator=' to initialize a member-function
        //    pointer having the appropriate signature and return type for the
        //    move assignment operator.  (C-1)
        //
        // 2. Create a 'bslma::TestAllocator' object, and install it as the
        //    default allocator.
        //
        // 3. Using the table-driven technique, specify a set of distinct
        //    object values (one per row) in terms of their attributes.
        //
        // 4. For each row 'R1' in the table of P-3:  (C-1..3)
        //
        //    1. Use the value constructor to create a modifiable 'Obj mZ',
        //       having the value described by 'R1', using the default
        //       allocator.
        //
        //    2. For each row 'R2' in the table of P-3:  (C-1..3)
        //
        //       1. Use the value constructor to create a modifiable 'Obj mX',
        //          having the value described by 'R2', using the default
        //          allocator.
        //
        //       2. Use the value constructor to create a modifiable 'Obj mY',
        //          having the same value as 'mX', using the default allocator.
        //
        //       3. Move-assign 'mX' from 'bslmf::MovableRefUtil::move(mY)'.
        //
        //       4. Verify that the address of the return value is the same as
        //          that of 'mX'.  (C-1)
        //
        //       5. Verify that 'mX' now has the same value as 'mZ'.  (C-1)
        //
        //       6. Verify that 'mY' is in a valid but unspecified state.
        //         (C-2)
        //
        //       7. Verify that the allocator used by 'mX' is the default
        //          allocator.  (C-3)
        //
        // 5. Test aliasing concerns by move-assigning an object to itself.
        //    (C-5)
        //
        // Testing:
        //   HierarchicalCategorySetting& operator=(MovableRef rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nMOVE-ASSIGNMENT OPERATOR"
                             "\n========================\n";

        if (verbose)
               cout << "\nAssign the address of the operator to a variable.\n";
        {
            typedef Obj& (Obj::*operatorPtr)(bslmf::MovableRef<Obj>);

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
                  "\nCreate a test allocator and install it as the default.\n";

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        if (verbose) cout << "\nUse a table of distinct object values and"
                                                   " expected memory usage.\n";

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE1   = DATA[ti].d_line;
            const char *PREFIX1 = DATA[ti].d_categoryPrefix;
            const int   R1      = DATA[ti].d_recordLevel;
            const int   P1      = DATA[ti].d_passLevel;
            const int   T1      = DATA[ti].d_triggerLevel;
            const int   TA1     = DATA[ti].d_triggerAllLevel;

            if (veryVerbose) {
                T_ P_(LINE1) P_(PREFIX1) P_(R1) P_(P1) P_(T1) P(TA1);
            }

            Obj mZ(PREFIX1, R1, P1, T1, TA1, &oa);  const Obj& Z = mZ;

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   LINE2   = DATA[tj].d_line;
                const char *PREFIX2 = DATA[tj].d_categoryPrefix;
                const int   R2      = DATA[tj].d_recordLevel;
                const int   P2      = DATA[tj].d_passLevel;
                const int   T2      = DATA[tj].d_triggerLevel;
                const int   TA2     = DATA[tj].d_triggerAllLevel;

                if (veryVerbose) {
                    T_ T_ P_(LINE2) P_(PREFIX2) P_(R2) P_(P2) P_(T2) P(TA2);
                }

                Obj mX(PREFIX2, R2, P2, T2, TA2, &oa);  const Obj& X = mX;
                Obj mY(PREFIX2, R2, P2, T2, TA2, &oa);  const Obj& Y = mY;

                ASSERTV(LINE1, LINE2, Y == X);
                ASSERTV(LINE1, LINE2, (ti == tj) == (Z == X));

                Obj *mR = &(mX = bslmf::MovableRefUtil::move(mY));

                ASSERTV(LINE1, LINE2, mR,  &mX, mR == &mX);
                ASSERTV(LINE1, LINE2, (ti == tj) == (Z == X));

                ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
            }
        }

        if (verbose) cout << "\nTesting self-assignment (alias test).\n";
        {
            Obj mX("self", 128, 64, 32, 16, &oa);  const Obj& X = mX;
            Obj mZ("self", 128, 64, 32, 16, &oa);  const Obj& Z = mZ;

            ASSERTV(Z == X);

            Obj *mR = &(mX = bslmf::MovableRefUtil::move(mX));

            ASSERTV(mR, &mX, mR == &mX);
            ASSERTV(Z == X);
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BSLX STREAMING
        //   Not applicable - there is no BSLX streaming for this component.
        // --------------------------------------------------------------------
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        // 1. The assignment operator can change the value of any modifiable
        //    target object to that of any source object.
        //
        // 2. The allocator used by the target object is unchanged.
        //
        // 3. Any memory allocation is from the target object's allocator.
        //
        // 4. The signature and return type are standard.
        //
        // 5. The reference returned is to the target object (i.e., '*this').
        //
        // 6. The value of the source object is not modified.
        //
        // 7. Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        // 1. Use the address of 'operator=' to initialize a member-function
        //    pointer having the appropriate signature and return type for the
        //    copy-assignment operator.  (C-4)
        //
        // 2. Create a 'bslma::TestAllocator' object, and install it as the
        //    default allocator.
        //
        // 3. Using the table-driven technique, specify a set of distinct
        //    object values (one per row) in terms of their attributes.
        //
        // 4. For each row 'R1' in the table of P-3:  (C-1..3, 5..6)
        //
        //    1. Use the value constructor to create a modifiable 'Obj mZ',
        //       having the value described by 'R1', using the default
        //       allocator.
        //
        //    2. For each row 'R2' in the table of P-3:  (C-1..3, 5..6)
        //
        //       1. Use the value constructor to create a modifiable 'Obj mX',
        //          having the value described by 'R2', using the default
        //          allocator.
        //
        //       2. Assign 'mX' from 'mZ'.
        //
        //       3. Verify that the address of the return value is the same as
        //          that of 'mX'.  (C-5)
        //
        //       4. Verify that 'mX' now has the same value as 'mZ'.  (C-1)
        //
        //       5. Verify that 'mZ' is unchanged.  (C-6)
        //
        //       6. Verify that the allocator used by 'mX' is the default
        //          allocator.  (C-2..3)
        //
        // 5. Test aliasing concerns by assigning an object to itself.  (C-7)
        //
        // Testing:
        //   HierarchicalCategorySetting& operator=(const Obj& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nCOPY-ASSIGNMENT OPERATOR"
                             "\n========================\n";

        if (verbose)
               cout << "\nAssign the address of the operator to a variable.\n";
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
                  "\nCreate a test allocator and install it as the default.\n";

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        if (verbose) cout << "\nUse a table of distinct object values and"
                                                   " expected memory usage.\n";

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE1   = DATA[ti].d_line;
            const char *PREFIX1 = DATA[ti].d_categoryPrefix;
            const int   R1      = DATA[ti].d_recordLevel;
            const int   P1      = DATA[ti].d_passLevel;
            const int   T1      = DATA[ti].d_triggerLevel;
            const int   TA1     = DATA[ti].d_triggerAllLevel;

            if (veryVerbose) {
                T_ P_(LINE1) P_(PREFIX1) P_(R1) P_(P1) P_(T1) P(TA1);
            }

            Obj mZ(PREFIX1, R1, P1, T1, TA1, &oa);  const Obj& Z = mZ;

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   LINE2   = DATA[tj].d_line;
                const char *PREFIX2 = DATA[tj].d_categoryPrefix;
                const int   R2      = DATA[tj].d_recordLevel;
                const int   P2      = DATA[tj].d_passLevel;
                const int   T2      = DATA[tj].d_triggerLevel;
                const int   TA2     = DATA[tj].d_triggerAllLevel;

                if (veryVerbose) {
                    T_ T_ P_(LINE2) P_(PREFIX2) P_(R2) P_(P2) P_(T2) P(TA2);
                }

                Obj mX(PREFIX2, R2, P2, T2, TA2, &oa);  const Obj& X = mX;

                ASSERTV(LINE1, LINE2, (ti == tj) == (Z == X));

                const Obj ZZ(Z, &oa);

                Obj *mR = &(mX = Z);

                ASSERTV(LINE1, LINE2, mR,  &mX, mR == &mX);
                ASSERTV(LINE1, LINE2, ZZ, X, ZZ == X);
                ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);

                ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
            }
        }

        if (verbose) cout << "\nTesting self-assignment (alias test).\n";
        {
            Obj mX("self", 128, 64, 32, 16, &oa);  const Obj& X = mX;
            Obj mZ("self", 128, 64, 32, 16, &oa);  const Obj& Z = mZ;

            ASSERTV(Z == X);

            Obj *mR = &(mX = X);

            ASSERTV(mR, &mX, mR == &mX);
            ASSERTV(Z == X);
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTOR
        //   Ensure that we can construct an object from a movable reference
        //   to another object of the same class.
        //
        // Concerns:
        // 1. The move constructor creates an object having the same value as
        //    that of the supplied original object.
        //
        // 2. The original object is left in a valid but unspecified state.
        //
        // 3. If an allocator is NOT supplied to the move constructor, the
        //    allocator of the original object is propagated.
        //
        // 4. If an allocator IS supplied to the move constructor, that
        //    allocator is used.
        //
        // 5. Supplying a null allocator address has the same effect as not
        //    supplying an allocator.
        //
        // 6. Any memory allocation is from the object allocator.
        //
        // 7. There is no temporary memory allocation from any allocator.
        //
        // Plan:
        // 1. Using the table-driven technique, specify a set of distinct
        //    object values (one per row) in terms of their attributes.
        //
        // 2. For each row (representing a distinct object value, 'V') in the
        //    table described in P-1:  (C-1..7)
        //
        //    1. Use the value constructor to create two objects with values
        //       corresponding to 'V', one using the default allocator, and the
        //       other using a test allocator that is distinct from the default
        //       allocator.
        //
        //    2. Move-construct a new object from each of the objects created
        //       in P-2.1 without supplying an allocator, and verify:  (C-1..3)
        //
        //       1. The newly constructed object has the same value as 'V'.
        //
        //       2. The allocator of the newly constructed object is the same
        //          as the allocator of the original object.
        //
        //       3. Move-construct a new object from each of the objects
        //          created in P-2.1, explicitly supplying an allocator, and
        //          verify:  (C-1, 4..7)
        //
        //          1. The newly constructed object has the same value as 'V'.
        //
        //          2. The allocator of the newly constructed object is the
        //             supplied allocator.
        //
        //          3. No temporary memory is allocated.
        //
        // Testing:
        //   HierarchicalCategorySetting(MovableRef original);
        //   HierarchicalCategorySetting(MovableRef o, Allocator *ba);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nMOVE CONSTRUCTOR"
                             "\n================\n";

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nUse a table of distinct object values and"
                                                   " expected memory usage.\n";

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_line;
            const char *PREFIX = DATA[ti].d_categoryPrefix;
            const int   R      = DATA[ti].d_recordLevel;
            const int   P      = DATA[ti].d_passLevel;
            const int   T      = DATA[ti].d_triggerLevel;
            const int   TA     = DATA[ti].d_triggerAllLevel;

            if (veryVerbose) {
                T_ P_(LINE) P_(PREFIX) P_(R) P_(P) P_(T) P(TA);
            }

            bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
            bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

            // Test move constructor without explicit allocator.

            {
                Obj mX(PREFIX, R, P, T, TA, &oa1);  const Obj& X = mX;
                Obj mZ(PREFIX, R, P, T, TA, &oa1);  const Obj& Z = mZ;

                ASSERTV(LINE, Z == X);

                Obj mY(bslmf::MovableRefUtil::move(mX));  const Obj& Y = mY;

                ASSERTV(LINE, Z, Y, Z == Y);
                ASSERTV(LINE, &oa1 == Y.get_allocator());
            }

            // Test move constructor with explicit allocator.

            {
                Obj mX(PREFIX, R, P, T, TA, &oa1);  const Obj& X = mX;
                Obj mZ(PREFIX, R, P, T, TA, &oa1);  const Obj& Z = mZ;

                ASSERTV(LINE, Z == X);

                Obj mY(bslmf::MovableRefUtil::move(mX), &oa2);
                const Obj& Y = mY;

                ASSERTV(LINE, Z, Y, Z == Y);
                ASSERTV(LINE, &oa2 == Y.get_allocator());
            }

            // Test move constructor with null allocator.

            {
                Obj mX(PREFIX, R, P, T, TA, &oa1);  const Obj& X = mX;
                Obj mZ(PREFIX, R, P, T, TA, &oa1);  const Obj& Z = mZ;

                ASSERTV(LINE, Z == X);

                Obj mY(bslmf::MovableRefUtil::move(mX), 0);
                const Obj& Y = mY;

                ASSERTV(LINE, Z, Y, Z == Y);
                ASSERTV(LINE, &da == Y.get_allocator());
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
        // 1. The copy constructor creates an object having the same value as
        //    that of the supplied original object.
        //
        // 2. The original object is passed as a reference to 'const'.
        //
        // 3. The value of the original object is unchanged.
        //
        // 4. If an allocator is NOT supplied to the copy constructor, the
        //    default allocator in effect at the time of construction becomes
        //    the object allocator for the resulting object.
        //
        // 5. If an allocator IS supplied to the copy constructor, that
        //    allocator becomes the object allocator for the resulting object.
        //
        // 6. Supplying a null allocator address has the same effect as not
        //    supplying an allocator.
        //
        // 7. Any memory allocation is from the object allocator.
        //
        // 8. There is no temporary memory allocation from any allocator.
        //
        // Plan:
        // 1. Using the table-driven technique, specify a set of distinct
        //    object values (one per row) in terms of their attributes.
        //
        // 2. For each row (representing a distinct object value, 'V') in the
        //    table described in P-1:  (C-1..8)
        //
        //    1. Use the value constructor to create an object with value
        //      corresponding to 'V', using the default allocator.
        //
        //    2. Use the copy constructor to create three distinct objects, in
        //      turn, each having the value of the object created in P-2.1 but
        //      configured differently: (a) without passing an allocator, (b)
        //      passing a null allocator address explicitly, and (c) passing
        //      the address of a test allocator distinct from the default
        //      allocator.  (C-1..8)
        //
        //    3. For each of the three objects created in P-2.2, verify:
        //       (C-1..8)
        //
        //       1. The newly created object has the same value as the original
        //          object.  (C-1)
        //
        //       2. The original object is unchanged.  (C-3)
        //
        //       3. The allocator of the newly created object is as expected.
        //          (C-4..6)
        //
        //       4. No temporary memory is allocated.  (C-8)
        //
        // Testing:
        //   HierarchicalCategorySetting(const Obj& o, Allocator *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nCOPY CONSTRUCTOR"
                             "\n================\n";

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
            "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_line;
            const char *PREFIX = DATA[ti].d_categoryPrefix;
            const int   R      = DATA[ti].d_recordLevel;
            const int   P      = DATA[ti].d_passLevel;
            const int   T      = DATA[ti].d_triggerLevel;
            const int   TA     = DATA[ti].d_triggerAllLevel;

            if (veryVerbose) {
                T_ P_(LINE) P_(PREFIX) P_(R) P_(P) P_(T) P(TA);
            }

            Obj mX(PREFIX, R, P, T, TA);  const Obj& X = mX;

            // Copy construct without explicit allocator.

            {
                const Obj Y(X);

                ASSERTV(LINE, X, Y, X == Y);
                ASSERTV(LINE, &da == Y.get_allocator());
            }

            // Copy construct with null allocator.

            {
                const Obj Y(X, 0);

                ASSERTV(LINE, X, Y, X == Y);
                ASSERTV(LINE, &da == Y.get_allocator());
            }

            // Copy construct with explicit allocator.

            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const Obj Y(X, &oa);

                ASSERTV(LINE, X, Y, X == Y);
                ASSERTV(LINE, &oa == Y.get_allocator());
            }

            // Verify original object is unchanged.

            ASSERTV(LINE, PREFIX,   X.categoryPrefix(),
                          PREFIX == X.categoryPrefix());
            ASSERTV(LINE, R,  X.recordLevel(),     R  == X.recordLevel());
            ASSERTV(LINE, P,  X.passLevel(),       P  == X.passLevel());
            ASSERTV(LINE, T,  X.triggerLevel(),    T  == X.triggerLevel());
            ASSERTV(LINE, TA, X.triggerAllLevel(), TA == X.triggerAllLevel());
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        //   Ensure that '==' and '!=' are the operational definitions of
        //   value equality.
        //
        // Concerns:
        // 1. Two objects, 'X' and 'Y', compare equal if and only if their
        //    corresponding salient attributes compare equal.
        //
        // 2. 'true  == (X == X)'  (i.e., identity)
        //
        // 3. 'false == (X != X)'  (i.e., identity)
        //
        // 4. 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //
        // 5. 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //
        // 6. 'X != Y' if and only if '!(X == Y)'
        //
        // 7. Comparison is symmetric with respect to user-defined conversion
        //    (i.e., both comparison operators are free functions).
        //
        // 8. Non-modifiable objects can be compared (i.e., 'const' objects
        //    and 'const' references).
        //
        // 9. The equality operators' signatures and return types are standard.
        //
        // Plan:
        // 1. Use the respective addresses of 'operator==' and 'operator!=' to
        //    initialize function pointers having the appropriate signatures
        //    and return types for the two homogeneous, free equality-
        //    comparison operators defined in this component.  (C-7..9)
        //
        // 2. Using the table-driven technique, specify a set of distinct
        //    object values (one per row) in terms of their attributes.
        //
        // 3. For each row 'R1' in the table of P-2:  (C-1..6)
        //
        //   1. Create an object 'X' having the value 'R1'.
        //
        //   2. For each row 'R2' in the table of P-2:  (C-1..6)
        //
        //     1. Create an object 'Y' having the value 'R2'.
        //
        //     2. Verify the commutativity property and the expected return
        //        value for both '==' and '!='.  (C-1..6)
        //
        // Testing:
        //   bool operator==(const Obj& lhs, const Obj& rhs);
        //   bool operator!=(const Obj& lhs, const Obj& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nEQUALITY OPERATORS"
                             "\n==================\n";

        if (verbose) cout <<
                      "\nAssign the address of each operator to a variable.\n";
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            operatorPtr operatorEq = ball::operator==;
            operatorPtr operatorNe = ball::operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        if (verbose) cout <<
                  "\nCreate a test allocator and install it as the default.\n";

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nUse a table of distinct object values and"
                                                   " expected memory usage.\n";

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE1   = DATA[ti].d_line;
            const char *PREFIX1 = DATA[ti].d_categoryPrefix;
            const int   R1      = DATA[ti].d_recordLevel;
            const int   P1      = DATA[ti].d_passLevel;
            const int   T1      = DATA[ti].d_triggerLevel;
            const int   TA1     = DATA[ti].d_triggerAllLevel;

            if (veryVerbose) {
                T_ P_(LINE1) P_(PREFIX1) P_(R1) P_(P1) P_(T1) P(TA1);
            }

            Obj mX(PREFIX1, R1, P1, T1, TA1);  const Obj& X = mX;

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   LINE2   = DATA[tj].d_line;
                const char *PREFIX2 = DATA[tj].d_categoryPrefix;
                const int   R2      = DATA[tj].d_recordLevel;
                const int   P2      = DATA[tj].d_passLevel;
                const int   T2      = DATA[tj].d_triggerLevel;
                const int   TA2     = DATA[tj].d_triggerAllLevel;

                if (veryVerbose) {
                    T_ T_ P_(LINE2) P_(PREFIX2) P_(R2) P_(P2) P_(T2) P(TA2);
                }

                Obj mY(PREFIX2, R2, P2, T2, TA2);  const Obj& Y = mY;

                const bool EXP = ti == tj;

                ASSERTV(LINE1, LINE2, X, Y,  EXP == (X == Y));
                ASSERTV(LINE1, LINE2, X, Y,  EXP == (Y == X));

                ASSERTV(LINE1, LINE2, X, Y, !EXP == (X != Y));
                ASSERTV(LINE1, LINE2, X, Y, !EXP == (Y != X));
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an `ostream` in some standard, human-readable form.
        //
        // Concerns:
        // 1. The `print` method writes the value to the specified `ostream`.
        //
        // 2. The `print` method writes the value in the intended format.
        //
        // 3. The output using `s << obj` is the same as `obj.print(s, 0, -1)`,
        //    but with each "attributeName = " elided.
        //
        // 4. The `print` method signature and return type are standard.
        //
        // 5. The `print` method returns the supplied `ostream`.
        //
        // 6. The optional `level` and `spacesPerLevel` parameters have the
        //   correct default values.
        //
        // 7. The output `operator<<` signature and return type are standard.
        //
        // 8. The output `operator<<` returns the supplied `ostream`.
        //
        // Plan:
        // 1. Use the addresses of the `print` member function and `operator<<`
        //    free function defined in this component to initialize,
        //    respectively, member-function and free-function pointers having
        //    the appropriate signatures and return types.  (C-4, 7)
        //
        // 2. Using the table-driven technique:  (C-1..3, 5..6, 8)
        //
        //   1. Define twelve carefully selected combinations of (two) object
        //      values (`A` and `B`), having distinct values for each
        //      corresponding salient attribute, and various values for the
        //      two formatting parameters, along with the expected output.
        //
        //      ( `value` x  `level`   x `spacesPerLevel` ):
        //       1 { A   } x {  0     } x {  0, 1, -1, -8 } --> 4 expected o/ps
        //       2 { A   } x {  3, -3 } x {  0, 2, -2, -8 } --> 6 expected o/ps
        //       3 { B   } x {  2     } x {  3            } --> 1 expected o/p
        //       4 { A B } x { -9     } x { -9            } --> 2 expected o/ps
        //
        //    2. For each row in the table defined in P-2.1:  (C-1..3, 5..6, 8)
        //
        //     1. Using a `const` `Obj`, supply each object value and pair of
        //        formatting parameters to `print`, omitting the `level` or
        //        `spacesPerLevel` parameter if the value of that argument is
        //        `-8`.  If the parameters are, arbitrarily, (-9, -9), then
        //        invoke the `operator<<` instead.
        //
        //     2. Use a standard `ostringstream` to capture the actual output.
        //
        //     3. Verify the address of what is returned is that of the
        //        supplied stream.  (C-5, 8)
        //
        //     4. Compare the contents captured in P-2.2.2 with what is
        //        expected.  (C-1..3, 6)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   operator<<(ostream& s, const HierarchicalCategorySetting& d);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nPRINT AND OUTPUT OPERATOR"
                             "\n=========================\n";

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocator         sa("scratch", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nAssign the addresses of `print` and"
                                    " the output `operator<<` to variables.\n";
        {
            using namespace ball;
            typedef
                  bsl::ostream& (Obj::*funcPtr)(bsl::ostream&, int, int) const;
            typedef bsl::ostream& (*operatorPtr)(bsl::ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (verbose) cout <<
                   "\nCreate a table of distinct value/format combinations.\n";

        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;

            const char *d_categoryPrefix;
            int         d_recordLevel;
            int         d_passLevel;
            int         d_triggerLevel;
            int         d_triggerAllLevel;

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL  PREFIX R  P  T  TA EXP
        //---- - ---  ------ -  -  -  -- ---

        { L_,  0,  0, "AB", 192, 128, 96, 64,
                                  "["                                      NL
                                  "categoryPrefix = \"AB\""                NL
                                  "recordLevel = 192"                      NL
                                  "passLevel = 128"                        NL
                                  "triggerLevel = 96"                      NL
                                  "triggerAllLevel = 64"                   NL
                                  "]"                                      NL
                                                                           },

        { L_,  0,  1, "AB", 192, 128, 96, 64,
                                  "["                                      NL
                                  " categoryPrefix = \"AB\""               NL
                                  " recordLevel = 192"                     NL
                                  " passLevel = 128"                       NL
                                  " triggerLevel = 96"                     NL
                                  " triggerAllLevel = 64"                  NL
                                  "]"                                      NL
                                                                           },

        { L_,  0, -1, "AB", 192, 128, 96, 64,
                                  "["                                      SP
                                  "categoryPrefix = \"AB\""                SP
                                  "recordLevel = 192"                      SP
                                  "passLevel = 128"                        SP
                                  "triggerLevel = 96"                      SP
                                  "triggerAllLevel = 64"                   SP
                                  "]"
                                                                           },

        { L_,  0, -8, "AB", 192, 128, 96, 64,
                                  "["                                      NL
                                  "    categoryPrefix = \"AB\""            NL
                                  "    recordLevel = 192"                  NL
                                  "    passLevel = 128"                    NL
                                  "    triggerLevel = 96"                  NL
                                  "    triggerAllLevel = 64"               NL
                                  "]"                                      NL
                                                                           },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } -->  6 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL  PREFIX R  P  T  TA EXP
        //---- - ---  ------ -  -  -  -- ---

        { L_,  3,  0, "AB", 192, 128, 96, 64,
                                  "["                                      NL
                                  "categoryPrefix = \"AB\""                NL
                                  "recordLevel = 192"                      NL
                                  "passLevel = 128"                        NL
                                  "triggerLevel = 96"                      NL
                                  "triggerAllLevel = 64"                   NL
                                  "]"                                      NL
                                                                           },

        { L_,  3,  2, "AB", 192, 128, 96, 64,
                                  "      ["                                NL
                                  "        categoryPrefix = \"AB\""        NL
                                  "        recordLevel = 192"              NL
                                  "        passLevel = 128"                NL
                                  "        triggerLevel = 96"              NL
                                  "        triggerAllLevel = 64"           NL
                                  "      ]"                                NL
                                                                           },

        { L_,  3, -2, "AB", 192, 128, 96, 64,
                                  "      ["                                SP
                                  "categoryPrefix = \"AB\""                SP
                                  "recordLevel = 192"                      SP
                                  "passLevel = 128"                        SP
                                  "triggerLevel = 96"                      SP
                                  "triggerAllLevel = 64"                   SP
                                  "]"
                                                                           },

        { L_,  3, -8, "AB", 192, 128, 96, 64,
                                "            ["                            NL
                                "                categoryPrefix = \"AB\""  NL
                                "                recordLevel = 192"        NL
                                "                passLevel = 128"          NL
                                "                triggerLevel = 96"        NL
                                "                triggerAllLevel = 64"     NL
                                "            ]"                            NL
                                                                           },

        { L_, -3,  0, "AB", 192, 128, 96, 64,
                                  "["                                      NL
                                  "categoryPrefix = \"AB\""                NL
                                  "recordLevel = 192"                      NL
                                  "passLevel = 128"                        NL
                                  "triggerLevel = 96"                      NL
                                  "triggerAllLevel = 64"                   NL
                                  "]"                                      NL
                                                                           },

        { L_, -3,  2, "AB", 192, 128, 96, 64,
                                  "["                                      NL
                                  "        categoryPrefix = \"AB\""        NL
                                  "        recordLevel = 192"              NL
                                  "        passLevel = 128"                NL
                                  "        triggerLevel = 96"              NL
                                  "        triggerAllLevel = 64"           NL
                                  "      ]"                                NL
                                                                           },

        { L_, -3, -2, "AB", 192, 128, 96, 64,
                                  "["                                      SP
                                  "categoryPrefix = \"AB\""                SP
                                  "recordLevel = 192"                      SP
                                  "passLevel = 128"                        SP
                                  "triggerLevel = 96"                      SP
                                  "triggerAllLevel = 64"                   SP
                                  "]"
                                                                           },
        { L_, -3, -8, "AB", 192, 128, 96, 64,
                                "["                                        NL
                                "                categoryPrefix = \"AB\""  NL
                                "                recordLevel = 192"        NL
                                "                passLevel = 128"          NL
                                "                triggerLevel = 96"        NL
                                "                triggerAllLevel = 64"     NL
                                "            ]"                            NL
                                                                           },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }            -->  1 expected o/p
        // -----------------------------------------------------------------

        //LINE L SPL  PREFIX R  P  T  TA EXP
        //---- - ---  ------ -  -  -  -- ---

        { L_,  2,  3, "EQ.MARKET", 160, 96, 64, 32,
                                  "      ["                                 NL
                                  "         categoryPrefix = \"EQ.MARKET\"" NL
                                  "         recordLevel = 160"              NL
                                  "         passLevel = 96"                 NL
                                  "         triggerLevel = 64"              NL
                                  "         triggerAllLevel = 32"           NL
                                  "      ]"                                 NL
                                                                           },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -9 }   x { -9 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL  PREFIX R  P  T  TA EXP
        //---- - ---  ------ -  -  -  -- ---

        { L_, -9, -9, "AB", 192, 128, 96, 64,
          "[ categoryPrefix = \"AB\" recordLevel = 192 passLevel = 128 "
          "triggerLevel = 96 triggerAllLevel = 64 ]"               },

        { L_, -9, -9, "EQ.MARKET", 160, 96, 64, 32,
          "[ categoryPrefix = \"EQ.MARKET\" recordLevel = 160 "
          "passLevel = 96 triggerLevel = 64 triggerAllLevel = 32 ]"  },

#undef NL
#undef SP

        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "\nTesting with various print specifications.\n";
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const int         L      = DATA[ti].d_level;
                const int         SPL    = DATA[ti].d_spacesPerLevel;
                const char *const PREFIX = DATA[ti].d_categoryPrefix;
                const int         R      = DATA[ti].d_recordLevel;
                const int         P      = DATA[ti].d_passLevel;
                const int         T      = DATA[ti].d_triggerLevel;
                const int         TA     = DATA[ti].d_triggerAllLevel;
                const char *const EXP    = DATA[ti].d_expected_p;

                if (veryVerbose) {
                    T_ P_(L) P_(SPL) P_(PREFIX) P_(R) P_(P) P_(T) P(TA);
                }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                const Obj X(PREFIX, R, P, T, TA);

                bdlsb::MemOutStreamBuf osb(&sa);
                bsl::ostream           os(&osb);

                // Verify supplied stream is returned by reference.

                if (-9 == L && -9 == SPL) {
                    ASSERTV(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    ASSERTV(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        ASSERTV(LINE, &os == &X.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        ASSERTV(LINE, &os == &X.print(os, L));
                    }
                    else {
                        ASSERTV(LINE, &os == &X.print(os));
                    }

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                // Verify output is formatted as expected.

                const bsl::string_view result(osb.data(), osb.length());

                if (veryVeryVerbose) { P(result) }

                ASSERTV(LINE, EXP, result, EXP == result);
            }
        }

        // Verify no memory came from the default allocator.

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure that each basic accessor properly interprets object state.
        //
        // Concerns:
        // 1. Each accessor returns the value of the corresponding attribute
        //    of the object.
        //
        // 2. Each accessor method is declared 'const'.
        //
        // 3. No accessor allocates memory.
        //
        // 4. The object reports the allocator specified at construction.
        //
        // Plan:
        // 1. Using the table-driven technique, create objects having different
        //    attribute values and verify that each basic accessor returns the
        //    expected value.  (C-1..4)
        //
        // Testing:
        //   const bsl::string& categoryPrefix() const;
        //   int recordLevel() const;
        //   int passLevel() const;
        //   int triggerLevel() const;
        //   int triggerAllLevel() const;
        //   bslma::Allocator *get_allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBASIC ACCESSORS"
                             "\n===============\n";

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nUse a table of distinct object values and "
                                                    "expected memory usage.\n";

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_line;
            const char *PREFIX = DATA[ti].d_categoryPrefix;
            const int   R      = DATA[ti].d_recordLevel;
            const int   P      = DATA[ti].d_passLevel;
            const int   T      = DATA[ti].d_triggerLevel;
            const int   TA     = DATA[ti].d_triggerAllLevel;

            if (veryVerbose) {
                T_ P_(LINE) P_(PREFIX) P_(R) P_(P) P_(T) P(TA);
            }

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(PREFIX, R, P, T, TA, &oa);  const Obj& X = mX;

            ASSERTV(LINE, PREFIX, X.categoryPrefix(),
                    PREFIX == X.categoryPrefix());
            ASSERTV(LINE, R,  X.recordLevel(),     R  == X.recordLevel());
            ASSERTV(LINE, P,  X.passLevel(),       P  == X.passLevel());
            ASSERTV(LINE, T,  X.triggerLevel(),    T  == X.triggerLevel());
            ASSERTV(LINE, TA, X.triggerAllLevel(), TA == X.triggerAllLevel());

            ASSERTV(LINE, &oa == X.get_allocator());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTORS
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        // 1. The value constructor can create an object having any value that
        //    does not violate the documented constraints.
        //
        // 2. If an allocator is NOT supplied to the value constructor, the
        //    default allocator in effect at the time of construction becomes
        //    the object allocator for the resulting object.
        //
        // 3. If an allocator IS supplied to the value constructor, that
        //    allocator becomes the object allocator for the resulting object.
        //
        // 4. Supplying a null allocator address has the same effect as not
        //    supplying an allocator.
        //
        // 5. Any memory allocation is from the object allocator.
        //
        // 6. There is no temporary memory allocation from any allocator.
        //
        // 7. Every object releases any allocated memory at destruction.
        //
        // 8. The constructor taking a ThresholdAggregate behaves identically
        //    to the constructor taking individual threshold levels.
        //
        // Plan:
        // 1. Using the table-driven technique, specify a set of distinct
        //    object values (one per row) in terms of their attributes.
        //
        // 2. For each row (representing a distinct object value, 'V') in the
        //    table described in P-1:  (C-1..7)
        //
        //   1. Execute an inner loop creating three distinct objects, in
        //      turn, each object having the same value, 'V', but configured
        //      differently: (a) without passing an allocator, (b) passing a
        //      null allocator address explicitly, and (c) passing the address
        //      of a test allocator distinct from the default allocator.
        //
        //   2. For each of the three iterations in P-2.1:  (C-1..7)
        //
        //     1. Create three 'bslma::TestAllocator' objects, and install one
        //        as the current default allocator (note that a ubiquitous test
        //        allocator is already installed as the global allocator).
        //
        //     2. Use the value constructor to dynamically create an object
        //        having the value 'V', with its object allocator configured
        //        appropriately (see P-2.1), supplying all the arguments as
        //        'const'; use a distinct test allocator for the object's
        //        footprint.  (C-1..6)
        //
        //     3. Use the (as yet unproven) salient attribute accessors to
        //        verify that all of the attributes of the object have their
        //        expected values.  (C-1, 3..6)
        //
        //     4. Verify that no temporary memory is allocated from the object
        //        allocator.  (C-7)
        //
        //     5. Verify that all object memory is released when the object is
        //        destroyed.  (C-8)
        //
        //   3. Repeat P-2 but use the constructor that takes a
        //      ThresholdAggregate instead of individual threshold levels.
        //      (C-9)
        //
        // Testing:
        //   HierarchicalCategorySetting(prefix, r, p, t, ta, *ba);
        //   HierarchicalCategorySetting(prefix, levels, *ba);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nVALUE CONSTRUCTORS"
                             "\n==================\n";

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nUse a table of distinct object values and"
                                                   " expected memory usage.\n";

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_line;
            const char *PREFIX = DATA[ti].d_categoryPrefix;
            const int   R      = DATA[ti].d_recordLevel;
            const int   P      = DATA[ti].d_passLevel;
            const int   T      = DATA[ti].d_triggerLevel;
            const int   TA     = DATA[ti].d_triggerAllLevel;

            if (veryVerbose) {
                T_ P_(LINE) P_(PREFIX) P_(R) P_(P) P_(T) P(TA);
            }

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                if (veryVerbose) cout <<
                          "\tTesting with various allocator configurations.\n";

                Obj                  *objPtr = 0;
                bslma::TestAllocator *objAllocatorPtr = 0;
                bslma::TestAllocator *noaPtr = 0;

                switch (CONFIG) {
                  case 'a': {
                    objAllocatorPtr = &da;
                    noaPtr = &sa;
                  } break;
                  case 'b': {
                    objAllocatorPtr = &da;
                    noaPtr = &sa;
                  } break;
                  case 'c': {
                    objAllocatorPtr = &sa;
                    noaPtr = &da;
                  } break;
                  default: {
                    ASSERTV(CONFIG, 0 == "Bad allocator config.");
                  } break;
                }

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = *noaPtr;

                // Record non-object allocator state before construction.

                const bsls::Types::Int64 NOA_BLOCKS = noa.numBlocksTotal();

                switch (CONFIG) {
                  case 'a': {
                    objPtr = new (fa) Obj(PREFIX, R, P, T, TA);
                  } break;
                  case 'b': {
                    objPtr = new (fa) Obj(PREFIX, R, P, T, TA, 0);
                  } break;
                  case 'c': {
                    objPtr = new
                                (fa) Obj(PREFIX, R, P, T, TA, objAllocatorPtr);
                  } break;
                  default: {
                    ASSERTV(CONFIG, 0 == "Bad allocator config.");
                  } break;
                }
                ASSERTV(CONFIG, sizeof(Obj) == fa.numBytesInUse());

                Obj& mX = *objPtr;  const Obj& X = mX;

                // Verify the attributes.

                ASSERTV(LINE, CONFIG, PREFIX, X.categoryPrefix(),
                        PREFIX == X.categoryPrefix());
                ASSERTV(LINE, CONFIG, R,  X.recordLevel(),
                        R  == X.recordLevel());
                ASSERTV(LINE, CONFIG, P,  X.passLevel(),
                        P  == X.passLevel());
                ASSERTV(LINE, CONFIG, T,  X.triggerLevel(),
                        T  == X.triggerLevel());
                ASSERTV(LINE, CONFIG, TA, X.triggerAllLevel(),
                        TA == X.triggerAllLevel());

                ASSERTV(LINE, CONFIG, &oa == X.get_allocator());

                // Verify no allocation from the non-object allocator.

                ASSERTV(LINE, CONFIG, noa.numBlocksTotal(), NOA_BLOCKS,
                        NOA_BLOCKS == noa.numBlocksTotal());

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());
                ASSERTV(LINE, CONFIG, oa.numBlocksInUse(),
                        0 == oa.numBlocksInUse());
            }

            // Test constructor with ThresholdAggregate.

            if (veryVerbose)
                         cout << "\tTesting ThresholdAggregate constructor.\n";

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                const char CONFIG = cfg;

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                ball::ThresholdAggregate levels(R, P, T, TA);

                Obj                  *objPtr = 0;
                bslma::TestAllocator *objAllocatorPtr = 0;

                switch (CONFIG) {
                  case 'a': {
                    objAllocatorPtr = &da;
                    objPtr = new (fa) Obj(PREFIX, levels);
                  } break;
                  case 'b': {
                    objAllocatorPtr = &da;
                    objPtr = new (fa) Obj(PREFIX, levels, 0);
                  } break;
                  case 'c': {
                    objAllocatorPtr = &sa;
                    objPtr = new (fa) Obj(PREFIX, levels, objAllocatorPtr);
                  } break;
                  default: {
                    ASSERTV(CONFIG, 0 == "Bad allocator config.");
                  } break;
                }

                Obj&                   mX = *objPtr;  const Obj& X = mX;
                bslma::TestAllocator&  oa = *objAllocatorPtr;

                ASSERTV(LINE, CONFIG, PREFIX, X.categoryPrefix(),
                        PREFIX == X.categoryPrefix());
                ASSERTV(LINE, CONFIG, R,  X.recordLevel(),
                        R  == X.recordLevel());
                ASSERTV(LINE, CONFIG, P,  X.passLevel(),
                        P  == X.passLevel());
                ASSERTV(LINE, CONFIG, T,  X.triggerLevel(),
                        T  == X.triggerLevel());
                ASSERTV(LINE, CONFIG, TA, X.triggerAllLevel(),
                        TA == X.triggerAllLevel());

                ASSERTV(LINE, CONFIG, &oa == X.get_allocator());

                fa.deleteObject(objPtr);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";

        bslma::TestAllocator         oa("object", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&oa);

        {
            Obj mX("example", 1, 2, 3, 4, &oa);  const Obj& X = mX;

            ASSERTV(X.categoryPrefix(),  "example" == X.categoryPrefix());

            ASSERTV(X.recordLevel(),     1 == X.recordLevel());
            ASSERTV(X.passLevel(),       2 == X.passLevel());
            ASSERTV(X.triggerLevel(),    3 == X.triggerLevel());
            ASSERTV(X.triggerAllLevel(), 4 == X.triggerAllLevel());

            ASSERT(X.get_allocator().mechanism() == &oa);
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    // CONCERN: In no case does memory come from the default allocator.

    ASSERTV(defaultAllocator.numBlocksTotal(),
            0 == defaultAllocator.numBlocksTotal());

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
