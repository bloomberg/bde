// bdld_datummaker.t.cpp                                              -*-C++-*-
#include <bdld_datummaker.h>

#include <bdlma_localsequentialallocator.h>

#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>            // to verify that we do not
#include <bslma_testallocatormonitor.h>     // allocate any memory

#include <bsls_bsltestutil.h>
#include <bsls_asserttest.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// TBD
//
// Primary Manipulators:
//: o TBD
//
// Basic Accessors:
//: o TBD
//
// Global Concerns:
//: o No memory is allocated.
//: o TBD
//
// Global Assumptions:
//: o ACCESSOR methods are 'const' thread-safe.
//: o TBD
// ----------------------------------------------------------------------------
// CLASS METHODS
// [  ] TBD
//
// CREATORS
// [  ] TBD
//
// MANIPULATORS
// [  ] TBD
//
// ACCESSORS
// [  ] TBD
//
// FREE OPERATORS
// [  ] TBD
//
// FREE FUNCTIONS
// [  ] TBD
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
// [ *] CONCERN: DOES NOT ALLOCATE MEMORY
// [  ] TEST APPARATUS: TBD
// [ 5] PRINTING: TBD


// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

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
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

//=============================================================================
//              PLATFORM DEFECT TRACKING MACROS TO SUPPORT TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_SUN)
// Last tested with the 12.3 compiler chain, and applies only to the
// (default) Rogue Wave library
# define BSLSTL_BITSET_NO_REBIND_IN_NATIVE_ALLOCATOR
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdld::DatumMaker Obj;

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Example 1: Testing of a function
/// - - - - - - - - - - - - - - - -
// Suppose we want to test a function, 'numCount', that returns the number of
// numeric elements in a 'bdld::Datum' array.
//
// First we implement the function:
//..
bdld::Datum numCount(const bdld::Datum arrray)
{
    bdld::DatumArrayRef aRef = arrray.theArray();

    int count = 0;

    for (bdld::DatumArrayRef::SizeType i = 0; i < aRef.length(); ++i) {
        if (aRef[i].isInteger() ||
            aRef[i].isInteger64() ||
            aRef[i].isDouble()) {
            ++count;
        }
    }

    return bdld::Datum::createInteger(count);
}
//..


// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3; (void)veryVerbose;
    bool     veryVeryVerbose = argc > 4; (void)veryVeryVerbose;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: DOES NOT ALLOCATE MEMORY

    bslma::TestAllocator ga("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&ga);

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&da);

    bslma::TestAllocator ta("test", veryVeryVeryVerbose);

    bslma::TestAllocatorMonitor gam(&ga), dam(&da);

    switch (test) { case 0:
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //:1 The usage example provided in the component header file compiles,
        //   links, and runs as shown.
        //
        // Plan:
        //:1 Incorporate usage example from header into test driver, remove
        //   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        bdlma::LocalSequentialAllocator<64> sa(&ta);

///Example 1: Testing of a function
/// - - - - - - - - - - - - - - - -
// Suppose we want to test a function, 'numCount', that returns the number of
// numeric elements in a 'bdld::Datum' array.
//
// First we implement the function.  See function implementation of 'numCount'
// in the section 'USAGE EXAMPLE' above!
//
// Then, within the test driver for 'numCount', we define a 'bdld::DatumMaker',
// and use it to initialize an array to test 'numCount':
//..
    bdld::DatumMaker m(&sa);
//..
// Here, we create the array we want to use as an argument to 'numCount':
//..
    bdld::Datum array = m.a(
        m(),
        m(bdld::DatumError(-1)),
        m.a(
            m(true),
            m(false)),
        m(42.0),
        m(false),
        m(0),
        m(true),
        m(bsls::Types::Int64(424242)),
        m.m(
            "firstName", "Bart",
            "lastName",  "Simpson",
            "age",       10
        ),
        m(bdlt::Date(2016, 10, 14)),
        m(bdlt::Time(13, 00, 00, 000)),
        m(bdlt::Datetime(2016, 10, 14, 13, 01, 30, 87)),
        m(bdlt::DatetimeInterval(280, 13, 41, 12, 321)),
        m("foobar")
    );
//..
// Next we call the function with the array-'Datum' as its first argument:
//..
    bdld::Datum retVal = numCount(array);
//..
// Finally we verify the return value:
//..
    ASSERT(retVal.theInteger() == 3);
//..

      } break;

      case 5: {
        //---------------------------------------------------------------------
        // OWNED KEY TESTS:
        //   This case exercises the key-owning map creating members and
        //   verifies that the keys are really copied.
        //
        // Concerns:
        //:
        //: 1 The keys are copied into the 'Datum' map..
        //
        // Plan:
        //:
        //: 1 Create destroyable strings for the keys.
        //:
        //: 2 Create a key-owning map with the 'DatumMaker'.
        //:
        //: 3 Destroy the original keys.
        //:
        //: 4 Verify that the map still contains the properties with the keys.
        //
        // Testing:
        //    bdld::Datum mok(...) const;
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "OWNED KEY TESTS" << endl
                                  << "===============" << endl;

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        Obj m(&ta);

        bsl::string foo("foo", &ta);
        bsl::string bar("bar", &ta);

        bdld::Datum d = m.mok(foo, 1, bar, 42);
        foo = "off";
        bar = "pub";

        ASSERT(d.isMap());

        bdld::DatumMapRef ref = d.theMap();

        const bdld::Datum *value;

        ASSERT((value = ref.find("foo")) &&
               *value == bdld::Datum::createInteger(1));

        ASSERT((value = ref.find("bar")) &&
               *value == bdld::Datum::createInteger(42));

        bdld::Datum::destroy(d, &ta);

      } break;

      case 4: {
        //---------------------------------------------------------------------
        // ALLOCATOR TESTS:
        //   This case exercises all 'Datum' creating members and verifies that
        //   the allocator specified at construction time is used to allocate
        //   memory for the 'Datum's created.
        //
        // Concerns:
        //:
        //: 1 The allocator specified at construction time is used to allocate
        //:  memory for the 'Datum's created.
        //:
        //: 2 Non-allocating members do not allocate.
        //
        // Plan:
        //:
        //: 1 Supply a 'TestAllocator' at creation to the 'DatumMaker'.
        //:
        //: 2 Use 'TestAllocatorMonitor' to determine the number of allocations
        //:   made while creating all types of 'Datum's.
        //:
        //: 3 Use 'bdld::Datum::destroy' to release the memory allocated.
        //
        // Testing:
        //    operator()() const;
        //    operator()(const bslmf::Nil&                value) const;
        //    operator()(int                              value) const;
        //    operator()(double                           value) const;
        //    operator()(bool                             value) const;
        //    operator()(const bdld::Error&               value) const;
        //    operator()(const bdlt::Date&                value) const;
        //    operator()(const bdlt::Time&                value) const;
        //    operator()(const bdlt::Datetime&            value) const;
        //    operator()(const bdlt::DatetimeInterval&    value) const;
        //    operator()(bsls::Types::Int64               value) const;
        //    operator()(const bdld::Udt&                 value) const;
        //    operator()(const bdld::Datum&               value) const;
        //    operator()(const bslstl::StringRef&         value) const;
        //    operator()(const char                      *value) const;
        //    template <class TYPE>
        //    operator()(const bdlb::NullableValue<TYPE>& value) const;
        //    bdld::Datum a(...) const;
        //    operator()(const bdld::Datum               *value,
        //               int                              size)  const;
        //    operator()(const bdld::ConstDatumArrayRef&  value) const;
        //    bdld::Datum m(...) const;
        //    bdld::Datum mok(...) const;
        //    operator()(const bdld::DatumMapRef&     value) const;
        //    operator()(const bdld::DatumMapEntry   *value,
        //               int                          size,
        //               bool                         sorted = false)  const;
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "ALLOCATOR TESTS" << endl
                                  << "===============" << endl;

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        bslma::TestAllocatorMonitor tam(&ta);
        Obj m(&ta);

        bdld::Datum d;

        dam.reset(); // Don't care what has happened before.

        if (veryVerbose) cout << "Non-allocating scalar members" << endl;
        d = m();
        d = m(bslmf::Nil());
        d = m(1);
        d = m(2.0);
        d = m(true);
        d = m(false);
        d = m(bdld::DatumError(2));
        d = m(bdlt::Date(2014, 2, 2));
        d = m(bdlt::Time(1, 1));
        d = m(bdld::DatumUdt(0, 1));
        d = m(bdld::Datum::createInteger(1));
        typedef bdlb::NullableValue<bool> TriBool;
        d = m(TriBool());
        d = m(TriBool(true));
        d = m(TriBool(false));
        ASSERT(tam.isInUseSame());
        ASSERT(dam.isInUseSame());
        tam.reset(); // Avoid false results if there was allocation above.

        if (veryVerbose) cout << "Allocating scalar members" << endl;
        d = m(bdlt::DatetimeInterval(0, 0, 0, 0,
                                  bdlt::DatetimeInterval::k_MILLISECONDS_MAX));
#ifdef BSLS_PLATFORM_CPU_32_BIT
        ASSERT(tam.isInUseUp());
#else
        ASSERT(tam.isInUseSame()); // 64-bit 'Datum' does not allocate
#endif
        bdld::Datum::destroy(d, &ta);
        ASSERT(dam.isInUseSame());
        ASSERT(tam.isInUseSame());

        d = m(bdlt::Datetime(898, 2, 2));
        ASSERT(dam.isInUseSame());
#ifdef BSLS_PLATFORM_CPU_32_BIT
        ASSERT(tam.isInUseUp());
#else
        ASSERT(tam.isInUseSame()); // 64-bit 'Datum' does not allocate
#endif
        bdld::Datum::destroy(d, &ta);
        ASSERT(dam.isInUseSame());
        ASSERT(tam.isInUseSame());

        d =  m(bsls::Types::Int64(
                              bsl::numeric_limits<bsls::Types::Int64>::max()));
        ASSERT(dam.isInUseSame());
#ifdef BSLS_PLATFORM_CPU_32_BIT
        ASSERT(tam.isInUseUp());
#else
        ASSERT(tam.isInUseSame()); // 64-bit 'Datum' does not allocate
#endif
        bdld::Datum::destroy(d, &ta);
        ASSERT(dam.isInUseSame());
        ASSERT(tam.isInUseSame());

        d = m(bslstl::StringRef("Very long string to get allocation forced."));
        ASSERT(dam.isInUseSame());
        ASSERT(tam.isInUseUp());
        bdld::Datum::destroy(d, &ta);
        ASSERT(dam.isInUseSame());
        ASSERT(tam.isInUseSame());

        d = m("Very long string to get allocation forced.");
        ASSERT(dam.isInUseSame());
        ASSERT(tam.isInUseUp());
        bdld::Datum::destroy(d, &ta);
        ASSERT(dam.isInUseSame());
        ASSERT(tam.isInUseSame());


        if (veryVerbose) cout << "Array-creating members" << endl;
        if (veryVeryVerbose) cout << "    Non-allocating" << endl;
        {
            const bdld::Datum arr[] = {
                m(0), m(1), m(2), m(3), m(4), m(5), m(6), m(7), m(8), m(9),
                m(10), m(11), m(12), m(13), m(14), m(15)
            };
            ASSERT(tam.isInUseSame());
            ASSERT(dam.isInUseSame());
            tam.reset(); // Avoid false results if there was allocation above.

            const int sizeOfArr = static_cast<int>(sizeof(arr) / sizeof(*arr));

            const bdld::DatumArrayRef aRef(arr, sizeOfArr);
            d = m(aRef);
            ASSERT(tam.isInUseSame());
            ASSERT(dam.isInUseSame());
            tam.reset(); // Avoid false results if there was allocation above.

            d = m(arr, 5);
            ASSERT(tam.isInUseSame());
            ASSERT(dam.isInUseSame());
            tam.reset(); // Avoid false results if there was allocation above.

            d = m.a();
            ASSERT(tam.isInUseSame());
            ASSERT(dam.isInUseSame());
            tam.reset(); // Avoid false results if there was allocation above.
        }

        if (veryVeryVerbose) cout << "    Allocating" << endl;
        {
            d = m.a(1);
            ASSERT(dam.isInUseSame());
            ASSERT(tam.isInUseUp());
            bdld::Datum::destroy(d, &ta);
            ASSERT(dam.isInUseSame());
            ASSERT(tam.isInUseSame());
        }

        if (veryVerbose) cout << "Map-creating members" << endl;
        if (veryVeryVerbose) cout << "    Non-allocating" << endl;
        {
            const bdld::DatumMapEntry map[] = {
                bdld::DatumMapEntry("foo", m(1)),
                bdld::DatumMapEntry("bar", m(42))
            };
            const int mSize = sizeof(map) / sizeof(*map);
            bdld::DatumMutableMapRef mRef;
            bdld::Datum::createUninitializedMap(&mRef, mSize, &ta);
            tam.reset();  // Ignore this allocation
            for (int i = 0; i < mSize; ++i) {
                mRef.data()[i] = map[i];
            }

            d = m(mRef);
            ASSERT(tam.isInUseSame());
            ASSERT(dam.isInUseSame());
            tam.reset(); // Avoid false results if there was allocation above.

            d.adoptMap(mRef);
            bdld::Datum::destroy(d, &ta);
            tam.reset();  // Ignore this deallocation

            d = m(map, mSize);
            ASSERT(dam.isInUseSame());
            ASSERT(tam.isInUseUp());
            bdld::Datum::destroy(d, &ta);
            ASSERT(dam.isInUseSame());
            ASSERT(tam.isInUseSame());
        }

        if (veryVeryVerbose) cout << "    Allocating" << endl;
        {
            d = m.m("foo", 1, "bar", 42);
            ASSERT(dam.isInUseSame());
            ASSERT(tam.isInUseUp());
            bdld::Datum::destroy(d, &ta);
            ASSERT(dam.isInUseSame());
            ASSERT(tam.isInUseSame());

            d = m.mok("foo", 1, "bar", 42);
            ASSERT(dam.isInUseSame());
            ASSERT(tam.isInUseUp());
            bdld::Datum::destroy(d, &ta);
            ASSERT(dam.isInUseSame());
            ASSERT(tam.isInUseSame());
        }

      } break;

      case 3: {
        //---------------------------------------------------------------------
        // MAP TESTS:
        //   This case exercises the map constructors, both with referenced and
        //   owned keys ('m' and 'mok' functions respectively).
        //
        // Concerns:
        //:
        //: 1 Map constructors create 'bdld::Datum' maps.
        //:
        //: 2 At least 16 entries are supported on compilers not providing
        //:   variadic template support.
        //:
        //: 3 More than 16 entries are supported on compilers with variadic
        //:   template support.
        //:
        //: 4 Entries of the map retain the values they are provided in the
        //:   constructor.
        //
        // Plan:
        //:
        //: 1 Use 'DatumMaker' to create maps.
        //:
        //: 2 Use 'bdld::Datum::create*' functions to create oracle maps.
        //:
        //: 3 Compare the 'DatumMaker' made and the oracle maps for equality.
        //
        // Testing:
        //    bdld::Datum m(...) const;
        //    bdld::Datum mok(...) const;
        //    operator()(const bdld::DatumMapRef&     value) const;
        //    operator()(const bdld::DatumMapEntry   *value,
        //               int                          size,
        //               bool                         sorted = false)  const;
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "MAP TESTS" << endl
                                  << "=========" << endl;

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        bdlma::LocalSequentialAllocator<64> sa(&ta);
        Obj m(&sa);

        const bdld::DatumMapEntry map[] = {
            bdld::DatumMapEntry("a", m(0)),
            bdld::DatumMapEntry("b", m(1)),
            bdld::DatumMapEntry("c", m(2)),
            bdld::DatumMapEntry("d", m(3)),
            bdld::DatumMapEntry("e", m(4)),
            bdld::DatumMapEntry("f", m(5)),
            bdld::DatumMapEntry("g", m(6)),
            bdld::DatumMapEntry("h", m(7)),
            bdld::DatumMapEntry("i", m(8)),
            bdld::DatumMapEntry("j", m(9)),
            bdld::DatumMapEntry("k", m(10)),
            bdld::DatumMapEntry("l", m(11)),
            bdld::DatumMapEntry("m", m(12)),
            bdld::DatumMapEntry("n", m(13)),
            bdld::DatumMapEntry("o", m(14)),
            bdld::DatumMapEntry("p", m(15)),
            bdld::DatumMapEntry("x", m(-1)),
            bdld::DatumMapEntry("y", m(-1)),
            bdld::DatumMapEntry("z", m(-1)),
            bdld::DatumMapEntry("w", m(-1)),
        };
        const int mSize = sizeof(map) / sizeof(*map);
        bdld::DatumMutableMapRef mRef;
        bdld::Datum::createUninitializedMap(&mRef, mSize, &sa);
        for (int i = 0; i < mSize; ++i) {
            mRef.data()[i] = map[i];
        }
        bsls::Types::size_type& size = *mRef.size();

        size = mSize;
        ASSERT(bdld::Datum::adoptMap(mRef) == m(mRef));
        ASSERT(bdld::Datum::adoptMap(mRef) == m(map, mSize));


        size =  0; ASSERT(bdld::Datum::adoptMap(mRef) == m.m());
        size =  1; ASSERT(bdld::Datum::adoptMap(mRef) == m.m("a", 0));
        size =  2; ASSERT(bdld::Datum::adoptMap(mRef) == m.m("a", 0,
                                                             "b", 1));
        size =  3; ASSERT(bdld::Datum::adoptMap(mRef) == m.m("a", 0,
                                                             "b", 1,
                                                             "c", 2));
        size =  4; ASSERT(bdld::Datum::adoptMap(mRef) == m.m("a", 0,
                                                             "b", 1,
                                                             "c", 2,
                                                             "d", 3));
        size =  5; ASSERT(bdld::Datum::adoptMap(mRef) == m.m("a", 0,
                                                             "b", 1,
                                                             "c", 2,
                                                             "d", 3,
                                                             "e", 4));
        size =  6; ASSERT(bdld::Datum::adoptMap(mRef) == m.m("a", 0,
                                                             "b", 1,
                                                             "c", 2,
                                                             "d", 3,
                                                             "e", 4,
                                                             "f", 5));
        size =  7; ASSERT(bdld::Datum::adoptMap(mRef) == m.m("a", 0,
                                                             "b", 1,
                                                             "c", 2,
                                                             "d", 3,
                                                             "e", 4,
                                                             "f", 5,
                                                             "g", 6));
        size =  8; ASSERT(bdld::Datum::adoptMap(mRef) == m.m("a", 0,
                                                             "b", 1,
                                                             "c", 2,
                                                             "d", 3,
                                                             "e", 4,
                                                             "f", 5,
                                                             "g", 6,
                                                             "h", 7));
        size =  9; ASSERT(bdld::Datum::adoptMap(mRef) == m.m("a", 0,
                                                             "b", 1,
                                                             "c", 2,
                                                             "d", 3,
                                                             "e", 4,
                                                             "f", 5,
                                                             "g", 6,
                                                             "h", 7,
                                                             "i", 8));
        size = 10; ASSERT(bdld::Datum::adoptMap(mRef) == m.m("a", 0,
                                                             "b", 1,
                                                             "c", 2,
                                                             "d", 3,
                                                             "e", 4,
                                                             "f", 5,
                                                             "g", 6,
                                                             "h", 7,
                                                             "i", 8,
                                                             "j", 9));
        size = 11; ASSERT(bdld::Datum::adoptMap(mRef) == m.m("a", 0,
                                                             "b", 1,
                                                             "c", 2,
                                                             "d", 3,
                                                             "e", 4,
                                                             "f", 5,
                                                             "g", 6,
                                                             "h", 7,
                                                             "i", 8,
                                                             "j", 9,
                                                             "k", 10));
        size = 12; ASSERT(bdld::Datum::adoptMap(mRef) == m.m("a", 0,
                                                             "b", 1,
                                                             "c", 2,
                                                             "d", 3,
                                                             "e", 4,
                                                             "f", 5,
                                                             "g", 6,
                                                             "h", 7,
                                                             "i", 8,
                                                             "j", 9,
                                                             "k", 10,
                                                             "l", 11));
        size = 13; ASSERT(bdld::Datum::adoptMap(mRef) == m.m("a", 0,
                                                             "b", 1,
                                                             "c", 2,
                                                             "d", 3,
                                                             "e", 4,
                                                             "f", 5,
                                                             "g", 6,
                                                             "h", 7,
                                                             "i", 8,
                                                             "j", 9,
                                                             "k", 10,
                                                             "l", 11,
                                                             "m", 12));
        size = 14; ASSERT(bdld::Datum::adoptMap(mRef) == m.m("a", 0,
                                                             "b", 1,
                                                             "c", 2,
                                                             "d", 3,
                                                             "e", 4,
                                                             "f", 5,
                                                             "g", 6,
                                                             "h", 7,
                                                             "i", 8,
                                                             "j", 9,
                                                             "k", 10,
                                                             "l", 11,
                                                             "m", 12,
                                                             "n", 13));
        size = 15; ASSERT(bdld::Datum::adoptMap(mRef) == m.m("a", 0,
                                                             "b", 1,
                                                             "c", 2,
                                                             "d", 3,
                                                             "e", 4,
                                                             "f", 5,
                                                             "g", 6,
                                                             "h", 7,
                                                             "i", 8,
                                                             "j", 9,
                                                             "k", 10,
                                                             "l", 11,
                                                             "m", 12,
                                                             "n", 13,
                                                             "o", 14));
        size = 16; ASSERT(bdld::Datum::adoptMap(mRef) == m.m("a", 0,
                                                             "b", 1,
                                                             "c", 2,
                                                             "d", 3,
                                                             "e", 4,
                                                             "f", 5,
                                                             "g", 6,
                                                             "h", 7,
                                                             "i", 8,
                                                             "j", 9,
                                                             "k", 10,
                                                             "l", 11,
                                                             "m", 12,
                                                             "n", 13,
                                                             "o", 14,
                                                             "p", 15));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
        size = 20; ASSERT(bdld::Datum::adoptMap(mRef) == m.m("a", 0,
                                                             "b", 1,
                                                             "c", 2,
                                                             "d", 3,
                                                             "e", 4,
                                                             "f", 5,
                                                             "g", 6,
                                                             "h", 7,
                                                             "i", 8,
                                                             "j", 9,
                                                             "k", 10,
                                                             "l", 11,
                                                             "m", 12,
                                                             "n", 13,
                                                             "o", 14,
                                                             "p", 15,
                                                             "x", -1,
                                                             "y", -1,
                                                             "z", -1,
                                                             "w", -1));
#endif

        size =  0; ASSERT(bdld::Datum::adoptMap(mRef) == m.mok());
        size =  1; ASSERT(bdld::Datum::adoptMap(mRef) == m.mok("a", 0));
        size =  2; ASSERT(bdld::Datum::adoptMap(mRef) == m.mok("a", 0,
                                                               "b", 1));
        size =  3; ASSERT(bdld::Datum::adoptMap(mRef) == m.mok("a", 0,
                                                               "b", 1,
                                                               "c", 2));
        size =  4; ASSERT(bdld::Datum::adoptMap(mRef) == m.mok("a", 0,
                                                               "b", 1,
                                                               "c", 2,
                                                               "d", 3));
        size =  5; ASSERT(bdld::Datum::adoptMap(mRef) == m.mok("a", 0,
                                                               "b", 1,
                                                               "c", 2,
                                                               "d", 3,
                                                               "e", 4));
        size =  6; ASSERT(bdld::Datum::adoptMap(mRef) == m.mok("a", 0,
                                                               "b", 1,
                                                               "c", 2,
                                                               "d", 3,
                                                               "e", 4,
                                                               "f", 5));
        size =  7; ASSERT(bdld::Datum::adoptMap(mRef) == m.mok("a", 0,
                                                               "b", 1,
                                                               "c", 2,
                                                               "d", 3,
                                                               "e", 4,
                                                               "f", 5,
                                                               "g", 6));
        size =  8; ASSERT(bdld::Datum::adoptMap(mRef) == m.mok("a", 0,
                                                               "b", 1,
                                                               "c", 2,
                                                               "d", 3,
                                                               "e", 4,
                                                               "f", 5,
                                                               "g", 6,
                                                               "h", 7));
        size =  9; ASSERT(bdld::Datum::adoptMap(mRef) == m.mok("a", 0,
                                                               "b", 1,
                                                               "c", 2,
                                                               "d", 3,
                                                               "e", 4,
                                                               "f", 5,
                                                               "g", 6,
                                                               "h", 7,
                                                               "i", 8));
        size = 10; ASSERT(bdld::Datum::adoptMap(mRef) == m.mok("a", 0,
                                                               "b", 1,
                                                               "c", 2,
                                                               "d", 3,
                                                               "e", 4,
                                                               "f", 5,
                                                               "g", 6,
                                                               "h", 7,
                                                               "i", 8,
                                                               "j", 9));
        size = 11; ASSERT(bdld::Datum::adoptMap(mRef) == m.mok("a", 0,
                                                               "b", 1,
                                                               "c", 2,
                                                               "d", 3,
                                                               "e", 4,
                                                               "f", 5,
                                                               "g", 6,
                                                               "h", 7,
                                                               "i", 8,
                                                               "j", 9,
                                                               "k", 10));
        size = 12; ASSERT(bdld::Datum::adoptMap(mRef) == m.mok("a", 0,
                                                               "b", 1,
                                                               "c", 2,
                                                               "d", 3,
                                                               "e", 4,
                                                               "f", 5,
                                                               "g", 6,
                                                               "h", 7,
                                                               "i", 8,
                                                               "j", 9,
                                                               "k", 10,
                                                               "l", 11));
        size = 13; ASSERT(bdld::Datum::adoptMap(mRef) == m.mok("a", 0,
                                                               "b", 1,
                                                               "c", 2,
                                                               "d", 3,
                                                               "e", 4,
                                                               "f", 5,
                                                               "g", 6,
                                                               "h", 7,
                                                               "i", 8,
                                                               "j", 9,
                                                               "k", 10,
                                                               "l", 11,
                                                               "m", 12));
        size = 14; ASSERT(bdld::Datum::adoptMap(mRef) == m.mok("a", 0,
                                                               "b", 1,
                                                               "c", 2,
                                                               "d", 3,
                                                               "e", 4,
                                                               "f", 5,
                                                               "g", 6,
                                                               "h", 7,
                                                               "i", 8,
                                                               "j", 9,
                                                               "k", 10,
                                                               "l", 11,
                                                               "m", 12,
                                                               "n", 13));
        size = 15; ASSERT(bdld::Datum::adoptMap(mRef) == m.mok("a", 0,
                                                               "b", 1,
                                                               "c", 2,
                                                               "d", 3,
                                                               "e", 4,
                                                               "f", 5,
                                                               "g", 6,
                                                               "h", 7,
                                                               "i", 8,
                                                               "j", 9,
                                                               "k", 10,
                                                               "l", 11,
                                                               "m", 12,
                                                               "n", 13,
                                                               "o", 14));
        size = 16; ASSERT(bdld::Datum::adoptMap(mRef) == m.mok("a", 0,
                                                               "b", 1,
                                                               "c", 2,
                                                               "d", 3,
                                                               "e", 4,
                                                               "f", 5,
                                                               "g", 6,
                                                               "h", 7,
                                                               "i", 8,
                                                               "j", 9,
                                                               "k", 10,
                                                               "l", 11,
                                                               "m", 12,
                                                               "n", 13,
                                                               "o", 14,
                                                               "p", 15));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
        size = 20; ASSERT(bdld::Datum::adoptMap(mRef) == m.mok("a", 0,
                                                               "b", 1,
                                                               "c", 2,
                                                               "d", 3,
                                                               "e", 4,
                                                               "f", 5,
                                                               "g", 6,
                                                               "h", 7,
                                                               "i", 8,
                                                               "j", 9,
                                                               "k", 10,
                                                               "l", 11,
                                                               "m", 12,
                                                               "n", 13,
                                                               "o", 14,
                                                               "p", 15,
                                                               "x", -1,
                                                               "y", -1,
                                                               "z", -1,
                                                               "w", -1));
#endif

      } break;

      case 2: {
        //---------------------------------------------------------------------
        // ARRAY TESTS:
        //   This case exercises the array constructors.
        //
        // Concerns:
        //:
        //: 1 Array constructors create 'bdld::Datum' arrays.
        //:
        //: 2 At least 16 elements are supported on compilers not providing
        //:   variadic template support.
        //:
        //: 3 More than 16 elements are supported on compilers with variadic
        //:   template support.
        //:
        //: 4 Elements of the array retain the order they are provided to the
        //:   constructor.
        //
        // Plan:
        //:
        //: 1 Use 'DatumMaker' to create arrays.
        //:
        //: 2 Use 'bdld::Datum::create*' functions to create oracle arrays.
        //:
        //: 3 Compare the 'DatumMaker' made and the oracle arrays for
        //:   equality.
        //
        // Testing:
        //    bdld::Datum a(...) const;
        //    operator()(const bdld::Datum               *value,
        //               int                              size)  const;
        //    operator()(const bdld::ConstDatumArrayRef&  value) const;
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "ARRAY TESTS" << endl
                                  << "===========" << endl;

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        bdlma::LocalSequentialAllocator<64> sa(&ta);
        Obj m(&sa);

        const bdld::Datum arr[] = {
            m(0), m(1), m(2), m(3), m(4), m(5), m(6), m(7), m(8), m(9), m(10),
            m(11), m(12), m(13), m(14), m(15), m(-1), m(-1), m(-1), m(-1),
            m(-1), m(-1), m(-1), m(-1), m(-1), m(-1), m(-1), m(-1), m(-1)
        };

        const int sizeOfArr = static_cast<int>(sizeof(arr) / sizeof(*arr));

        const bdld::DatumArrayRef aRef(arr, 5);

        ASSERT(bdld::Datum::createArrayReference(aRef, &sa) == m(aRef));
        ASSERT(bdld::Datum::createArrayReference(aRef, &sa) == m(arr, 5));

        ASSERT(bdld::Datum::createArrayReference(0, 0, &sa) == m.a());
        ASSERT(bdld::Datum::createArrayReference(arr, 1, &sa) == m.a(0));
        ASSERT(bdld::Datum::createArrayReference(arr, 2, &sa) ==
               m.a(0, 1));
        ASSERT(bdld::Datum::createArrayReference(arr, 3, &sa) ==
               m.a(0, 1, 2));
        ASSERT(bdld::Datum::createArrayReference(arr, 4, &sa) ==
               m.a(0, 1, 2, 3));
        ASSERT(bdld::Datum::createArrayReference(arr, 5, &sa) ==
               m.a(0, 1, 2, 3, 4));
        ASSERT(bdld::Datum::createArrayReference(arr, 6, &sa) ==
               m.a(0, 1, 2, 3, 4, 5));
        ASSERT(bdld::Datum::createArrayReference(arr, 7, &sa) ==
               m.a(0, 1, 2, 3, 4, 5, 6));
        ASSERT(bdld::Datum::createArrayReference(arr, 8, &sa) ==
               m.a(0, 1, 2, 3, 4, 5, 6, 7));
        ASSERT(bdld::Datum::createArrayReference(arr, 9, &sa) ==
               m.a(0, 1, 2, 3, 4, 5, 6, 7, 8));
        ASSERT(bdld::Datum::createArrayReference(arr, 10, &sa) ==
               m.a(0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
        ASSERT(bdld::Datum::createArrayReference(arr, 11, &sa) ==
               m.a(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
        ASSERT(bdld::Datum::createArrayReference(arr, 12, &sa) ==
               m.a(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11));
        ASSERT(bdld::Datum::createArrayReference(arr, 13, &sa) ==
               m.a(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12));
        ASSERT(bdld::Datum::createArrayReference(arr, 14, &sa) ==
               m.a(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13));
        ASSERT(bdld::Datum::createArrayReference(arr, 15, &sa) ==
               m.a(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14));
        ASSERT(bdld::Datum::createArrayReference(arr, 16, &sa) ==
               m.a(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
        ASSERT(bdld::Datum::createArrayReference(arr, sizeOfArr, &sa) ==
                                      m.a(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                          11, 12, 13, 14, 15, -1, -1, -1, -1,
                                          -1, -1, -1, -1, -1, -1, -1, -1, -1));
#endif
      } break;

      case 1: {
        //---------------------------------------------------------------------
        // SCALAR TESTS:
        //   This case exercises the scalar constructors.
        //
        // Concerns:
        //:
        //: 1 Scalar constructors create the required type of 'bdld::Datum'.
        //
        // Plan:
        //:
        //: 1 Use 'DatumMaker' to create scalars.
        //:
        //: 2 Use 'bdld::Datum::create*' functions to create oracle scalars.
        //:
        //: 3 Compare the 'DatumMaker' made and the oracle scalars for
        //:   equality.
        //
        // Testing:
        //    operator()() const;
        //    operator()(const bslmf::Nil&                value) const;
        //    operator()(int                              value) const;
        //    operator()(double                           value) const;
        //    operator()(bool                             value) const;
        //    operator()(const bdld::Error&               value) const;
        //    operator()(const bdlt::Date&                value) const;
        //    operator()(const bdlt::Time&                value) const;
        //    operator()(const bdlt::Datetime&            value) const;
        //    operator()(const bdlt::DatetimeInterval&    value) const;
        //    operator()(bsls::Types::Int64               value) const;
        //    operator()(const bdld::Udt&                 value) const;
        //    operator()(const bdld::Datum&               value) const;
        //    operator()(const bslstl::StringRef&         value) const;
        //    operator()(const char                      *value) const;
        //    template <class TYPE>
        //    operator()(const bdlb::NullableValue<TYPE>& value) const;
        //
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "SCALAR TESTS" << endl
                                  << "============" << endl;

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        bdlma::LocalSequentialAllocator<64> sa(&ta);
        Obj m(&sa);

        ASSERT(bdld::Datum::createNull() == m());
        ASSERT(bdld::Datum::createNull() == m(bslmf::Nil()));

        ASSERT(bdld::Datum::createInteger(1) == m(1));

        ASSERT(bdld::Datum::createDouble(2) == m(2.0));

        ASSERT(bdld::Datum::createBoolean(true) == m(true));
        ASSERT(bdld::Datum::createBoolean(false) == m(false));

        ASSERT(bdld::Datum::createError(2) == m(bdld::DatumError(2)));

        ASSERT(bdld::Datum::createDate(bdlt::Date(2014,2,2)) ==
                                                      m(bdlt::Date(2014,2,2)));
        ASSERT(bdld::Datum::createTime(bdlt::Time(1,1)) == m(bdlt::Time(1,1)));
        ASSERT(bdld::Datum::createDatetime(bdlt::Datetime(2014,2,2), &sa) ==
                                                  m(bdlt::Datetime(2014,2,2)));
        ASSERT(bdld::Datum::createDecimal64(BDLDFP_DECIMAL_DD(12.75), &sa) ==
                                                  m(BDLDFP_DECIMAL_DD(12.75)));
        ASSERT(bdld::Datum::createDatetimeInterval(bdlt::DatetimeInterval(9),
                                                   &sa) ==
                                                 m(bdlt::DatetimeInterval(9)));
        ASSERT(bdld::Datum::createInteger64(3, &sa) ==
                                                     m(bsls::Types::Int64(3)));
        ASSERT(bdld::Datum::createUdt(0, 1) == m(bdld::DatumUdt(0,1)));

        ASSERT(bdld::Datum::createInteger(1) ==
                                             m(bdld::Datum::createInteger(1)));

        ASSERT(bdld::Datum::createStringRef("foo", &sa) ==
                                                  m(bslstl::StringRef("foo")));
        ASSERT(bdld::Datum::createStringRef("foo", &sa) == m("foo"));

        typedef bdlb::NullableValue<bool> TriBool;
        ASSERT(bdld::Datum::createNull() == m(TriBool()));
        ASSERT(bdld::Datum::createBoolean(true) == m(TriBool(true)));
        ASSERT(bdld::Datum::createBoolean(false) == m(TriBool(false)));


      } break;

      default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
