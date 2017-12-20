// bdld_datummapbuilder.t.cpp                                         -*-C++-*-
#include <bdld_datummapbuilder.h>

#include <bslim_testutil.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_default.h>               // for testing only
#include <bslma_defaultallocatorguard.h> // for testing only

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_vector.h>
#include <bsl_algorithm.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace BloombergLP::bdld;
using namespace bsl;
using namespace bslstl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a utility for building 'Datum' objects holding
// maps of 'Datum' objects.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] DatumMapBuilder(SizeType, bslma::Allocator *);
// [ 2] ~DatumMapBuilder();
//
// MANIPULATORS
// [ 4] void pushBack(const bslstl::StringRef&, const Datum&);
// [ 2] void append(const DatumMapEntry *, int);
// [ 2] Datum commit();
// [ 5] void setSorted(bool);
// [ 6] Datum sortAndCommit();
//
// ACCESSORS
// [ 3] SizeType capacity() const;
// [ 3] SizeType size() const;
//
// TRAITS
// [ 7] bslma::UsesBslmaAllocator
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE

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

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                    GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef DatumMapBuilder Obj;

DatumMapEntry values[] = {
    DatumMapEntry(StringRef("first"),  Datum::createInteger(1)),
    DatumMapEntry(StringRef("second"), Datum::createInteger(2)),
    DatumMapEntry(StringRef("third"),  Datum::createInteger(3)),
    DatumMapEntry(StringRef("fourth"), Datum::createInteger(4)),
    DatumMapEntry(StringRef("fifth"),  Datum::createInteger(5)),
};

const size_t NUM_ELEMENTS = sizeof(values) / sizeof(DatumMapEntry);

//=============================================================================
//               GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
bool compareGreater(const DatumMapEntry& lhs, const DatumMapEntry& rhs)
    // Return 'true' if key in the specified 'lhs' is greater than key in the
    // specified 'rhs' and 'false' otherwise.
{
    return lhs.key() > rhs.key();
}

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);
    bslma::TestAllocatorMonitor gam(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
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
        //:   leading comment characters and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// Suppose we need a map for some personal data.  And the values in that map
// can be different types.   The following code illustrates how to use
// 'bdld::DatumMapBuilder' to create such map easily.
//
// First, we need data to fill our map:
//..
    bslma::TestAllocator ta("test", veryVeryVerbose);

    DatumMapEntry bartData[] = {
        DatumMapEntry(StringRef("firstName"),
                      Datum:: createStringRef("Bart", &ta)),
        DatumMapEntry(StringRef("lastName"),
                      Datum:: createStringRef("Simpson", &ta)),
        DatumMapEntry(StringRef("sex"), Datum::createStringRef("male", &ta)),
        DatumMapEntry(StringRef("age"), Datum::createInteger(10))
    };

    const size_t DATA_SIZE  = sizeof(bartData) / sizeof(DatumMapEntry);
//..
// Next, we create an object of 'DatumMapBuilder' class with initial capacity
// sufficient for storing all our data:
//..
    DatumMapBuilder builder(DATA_SIZE, &ta);
//..
// Then, we load our builder with these data:
//..
    for (size_t i = 0; i < DATA_SIZE; ++i) {
        builder.pushBack(bartData[i].key(), bartData[i].value());
    }
//..
// Next, we adopt the map, held by our builder, by newly created 'Datum'
// object:
//..
    Datum bart = builder.commit();
//..
// Now, we can check that all data have been correctly added to the map at the
// required order:
//..
    ASSERT(true == bart.isMap());
    ASSERT(DATA_SIZE == bart.theMap().size());

    ASSERT("firstName" == bart.theMap()[0].key());
    ASSERT(true        == bart.theMap()[0].value().isString());
    ASSERT("Bart"      == bart.theMap()[0].value().theString());

    ASSERT("lastName"  == bart.theMap()[1].key());
    ASSERT(true        == bart.theMap()[1].value().isString());
    ASSERT("Simpson"   == bart.theMap()[1].value().theString());

    ASSERT("sex"       == bart.theMap()[2].key());
    ASSERT(true        == bart.theMap()[2].value().isString());
    ASSERT("male"      == bart.theMap()[2].value().theString());

    ASSERT("age"       == bart.theMap()[3].key());
    ASSERT(true        == bart.theMap()[3].value().isInteger());
    ASSERT(10          == bart.theMap()[3].value().theInteger());
//..
// Finally, we destroy the 'Datum' object to release all allocated memory
// correctly:
//..
    Datum::destroy(bart, &ta);
    ASSERT(0 == ta.numBytesInUse());
//..
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING TRAITS
        //
        // Concerns:
        //: 1 bslma::UsesBslmaAllocator is true for DatumMapBuilder.
        //
        // Plan:
        //: 1 Assert trait.  (C-1)
        //
        // Testing:
        //   bslma::UsesBslmaAllocator
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING TRAITS" << endl
                          << "==============" << endl;

        ASSERT(bslma::UsesBslmaAllocator<Obj>::value);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'sortAndCommit'
        //
        // Concerns:
        //: 1 'sortAndCommit' doesn't sort empty map.  Adopted map is marked as
        //:   unsorted.
        //:
        //: 2 'sortAndCommit' sorts the elements of non-empty map before
        //:   adopting.  Adopted map is marked as sorted.
        //:
        //: 3 No memory leak occur during map sorting and adoption.
        //
        // Plan:
        //: 1 Create a 'DatumMapBuilder' object using specified allocator.  Get
        //:   a 'Datum' object by calling 'sortAndCommit' and verify it's
        //:   status.  (C-1)
        //:
        //: 2 Create a 'DatumMapBuilder' object using specified allocator.
        //:   Append few elements to the map.  Get a 'Datum' object by calling
        //:   'sortAndCommit' and verify it's status.  Also verify that the
        //:   elements in the map have been sorted.  (C-2)
        //:
        //: 3 After all objects deletion verify that no memory from specified
        //:   allocator is used.  (C-3)
        //
        // Testing:
        //    Datum sortAndCommit();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'sortAndCommit'" << endl
                          << "=======================" << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVerbose);

            if (verbose) cout << "\nTesting 'sortAndCommit' with empty map."
                              << endl;
            {
                Obj        mB(0, &ta);

                Datum        mD = mB.sortAndCommit();
                const Datum& D = mD;

                ASSERT(true  == D.isMap());
                ASSERT(0     == D.theMap().size());
                ASSERT(false == D.theMap().isSorted());
            }

            if (verbose) cout
                    << "\nTesting 'sortAndCommit' with non-empty map." << endl;
            {
                bslma::TestAllocator ta("test", veryVeryVerbose);

                Obj mB(NUM_ELEMENTS, &ta);

                mB.append(values, NUM_ELEMENTS);

                Datum        mD = mB.sortAndCommit();
                const Datum& D = mD;

                ASSERT(true == D.isMap());

                const DatumMapRef ref = D.theMap();

                ASSERT(NUM_ELEMENTS == ref.size());
                ASSERT(true         == ref.isSorted());

                for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
                    if (veryVerbose) {
                        T_ P_(i) P_(values[i].key()) P(ref[i].key())
                    }

                    ASSERTV(i, 0 != ref.find(values[i].key()));
                }

                const DatumMapEntry *MAP_END = ref.data() + ref.size();
                ASSERT(MAP_END == bsl::adjacent_find(ref.data(),
                                                     ref.data() + ref.size(),
                                                     compareGreater));
                Datum::destroy(mD, &ta);
            }

            ASSERT(0 == ta.numBytesInUse());
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'setSorted'
        //
        // Concerns:
        //: 1 'setSorted' sets the sort flag of empty map to 'false' anyway.
        //:
        //: 2 'setSorted' sets given value to the sort flag of non-empty map.
        //
        // Plan:
        //: 1 Create a 'DatumMapBuilder' object with empty capacity.  Set the
        //:   sort flag to 'true'.  Get a 'Datum' object by calling 'commit'
        //:   and verify that map is marked as unsorted.  (C-1)
        //:
        //: 2 Create a 'DatumMapBuilder' object.  Append few elements to the
        //:   map in a sorted order.  Set the sort flag.  Get a 'Datum' object
        //:   by calling 'commit' and verify that map is marked as sorted.
        //:   Also verify that the elements in the map are still in sorted
        //:   order.  (C-2)
        //:
        //: 3 Create another 'DatumMapBuilder' object.  Append few elements to
        //:   map.  Set the sort flag and then reset it.  Get a 'Datum' object
        //:   by calling 'commit' and verify that map is marked as unsorted.
        //:   Also verify that the elements in the map are still in the order
        //:   specified on the map creation.  (C-3)
        //
        // Testing:
        //    void setSorted(bool);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'setSorted'" << endl
                          << "===================" << endl;

        if (verbose) cout << "\nTesting 'setSorted' with empty map." << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVerbose);


            Obj mB(0, &ta);
            mB.setSorted(true);

            Datum        mD = mB.commit();
            const Datum& D = mD;

            ASSERT(true  == D.isMap());
            ASSERT(false == D.theMap().isSorted());
            ASSERT(0     == D.theMap().size());

            Datum::destroy(mD, &ta);
            ASSERT(0 == ta.numBytesInUse());
        }

        if (verbose) cout << "\nTesting 'setSorted' with sorted map." << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVerbose);

            bslstl::StringRef keys[] = {
                "first",
                "second",
                "third",
                "fourth",
                "fifth"
            };

            const size_t NUM_KEYS = sizeof(keys) / sizeof(bslstl::StringRef);

            bsl::sort(keys, keys + NUM_KEYS);

            Obj mB(NUM_KEYS, &ta);
            for (size_t i = 0; i < NUM_KEYS; ++i) {
                if (veryVerbose) {
                    T_ P_(keys[i])  P(static_cast<int>(i + 1))
                }

                mB.pushBack(keys[i],
                            Datum::createInteger(static_cast<int>(i + 1)));
            }
            mB.setSorted(true);

            Datum        mD = mB.commit();
            const Datum& D = mD;

            ASSERT(true == D.isMap());

            const DatumMapRef ref = D.theMap();

            ASSERT(true     == ref.isSorted());
            ASSERT(NUM_KEYS == ref.size());

            for (size_t i = 0; i < NUM_KEYS; ++i) {
                ASSERTV(i, 0 != ref.find(keys[i]));
            }
            const DatumMapEntry *MAP_END = ref.data() + ref.size();
            ASSERT(MAP_END == bsl::adjacent_find(ref.data(),
                                                 ref.data() + ref.size(),
                                                 compareGreater));

            Datum::destroy(mD, &ta);
            ASSERT(0 == ta.numBytesInUse());
        }

        if (verbose) cout << "\nTesting 'setSorted' with unsorted map."
                          << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVerbose);

            Obj mB(NUM_ELEMENTS, &ta);
            mB.setSorted(true);
            mB.append(values, NUM_ELEMENTS);
            mB.setSorted(false);

            Datum        mD = mB.commit();
            const Datum& D = mD;

            ASSERT(true == D.isMap());

            const DatumMapRef ref = D.theMap();

            ASSERT(false        == ref.isSorted());
            ASSERT(NUM_ELEMENTS == ref.size());

            for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
                ASSERTV(i, values[i] == ref[i]);
            }

            Datum::destroy(mD, &ta);
            ASSERT(0 == ta.numBytesInUse());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'pushBack'
        //
        // Concerns:
        //: 1 The 'pushBack' method appends 'Datum' value with the specified
        //:   key to the end of the map.
        //:
        //: 2 The 'pushBack' method grows the map, only if it is filled to the
        //:   capacity.
        //:
        //: 3 The 'pushBack' method uses allocator, passed at the construction,
        //:   to grow the map.
        //
        // Plan:
        //: 1 Create a 'DatumMapBuilder' object.  Push back few elements to the
        //:   map and verify that the capacity has increased only if there is
        //:   no place for new item.  Verify that memory is allocated by the
        //:   allocator passed on the builder construction.  (C-2..3)
        //:
        //: 2 Get a 'Datum' object by committing the map and verify it contains
        //:   item in the same order as they has been passed to the pushBack
        //:   method.  (C-1)
        //
        // Testing:
        //    void pushBack(const bslstl::StringRef&, const Datum&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'pushBack'" << endl
                          << "==================" << endl;

        {
            bslma::TestAllocator ta("test", veryVeryVerbose);

            Obj        mB(0, &ta);
            const Obj& B = mB;

            // Checking memory allocation.

            ASSERT(0 == B.capacity());
            bsls::Types::Int64 numBytesInUse = ta.numBytesInUse();

            mB.pushBack(values[0].key(), values[0].value());
            ASSERT(1             == B.capacity());
            ASSERT(numBytesInUse <  ta.numBytesInUse());

            numBytesInUse = ta.numBytesInUse();

            mB.pushBack(values[1].key(), values[1].value());
            ASSERT(2             == B.capacity());
            ASSERT(numBytesInUse <  ta.numBytesInUse());

            numBytesInUse = ta.numBytesInUse();

            mB.pushBack(values[2].key(), values[2].value());
            ASSERT(4             == B.capacity());
            ASSERT(numBytesInUse <  ta.numBytesInUse());

            numBytesInUse = ta.numBytesInUse();

            mB.pushBack(values[3].key(), values[3].value());
            ASSERT(4             == B.capacity());
            ASSERT(numBytesInUse == ta.numBytesInUse());

            mB.pushBack(values[4].key(), values[4].value());
            ASSERT(8             == B.capacity());
            ASSERT(numBytesInUse <  ta.numBytesInUse());

            Datum        mD = mB.commit();
            const Datum& D = mD;

            // Checking correctness of elements arrangement.

            ASSERT(true == D.isMap());

            const DatumMapRef ref = D.theMap();

            ASSERT(NUM_ELEMENTS == ref.size());
            ASSERT(false        == ref.isSorted());

            for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
                if (veryVerbose) {
                    T_ P_(i) P_(values[i].key()) P(ref[i].key())
                }

                ASSERTV(i, values[i].key() == ref[i].key());
            }

            Datum::destroy(mD, &ta);

            ASSERT(0 == ta.numBytesInUse());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //
        // Concerns:
        //: 1 The 'capacity' method returns the capacity of the datum map.
        //:
        //: 2 The 'size' method returns the current size of the datum map.
        //
        // Plan:
        //: 1 Create a 'DatumMapBuilder' object. Append few elements to the
        //:   map and verify that the 'capacity' and 'size' methods return
        //:   expected values.  (C-1,2)
        //
        // Testing:
        //    SizeType capacity() const;
        //    SizeType size() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nTesting 'capacity' and 'size'." << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVerbose);

            Obj        mB(0, &ta);
            const Obj& B = mB;

            ASSERT(0 == B.capacity());
            ASSERT(0 == B.size());

            mB.append(values, 1);

            ASSERT(1 == B.capacity());
            ASSERT(1 == B.size());

            mB.append(values + 1, 1);

            ASSERT(2 == B.capacity());
            ASSERT(2 == B.size());

            mB.append(values + 2, 3);

            ASSERT(8 == B.capacity());
            ASSERT(5 == B.size());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //
        // Concerns:
        //: 1 The constructor correctly sets required capacity and binds given
        //:   allocator with created object.
        //:
        //: 2 The 'append' method appends 'Datum' value with the specified
        //:   key to the end of the map.
        //:
        //: 3 The 'append' method grows the map, only if there is no place to
        //:   store passed objects.
        //:
        //: 4 The 'append' method uses allocator, passed at the construction,
        //:   to grow the map.
        //:
        //: 5 The 'commit' method creates a 'Datum' object holding a map of
        //:   'Datum' objects.
        //:
        //: 6 The destructor releases all memory, allocated during map or it's
        //:   items creation, if map hasn't been committed.
        //:
        //: 7 The destructor doesn't affect commited map.
        //:
        //: 8 Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a several 'DatumMapBuilder' objects with user-defined or
        //:   default capacity and allocator.  Verify that enough memory to
        //:   store required number of elements has been allocated by the
        //:   specified allocator.  (C-1)
        //:
        //: 2 Create a 'DatumMapBuilder' object.  Append few elements to the
        //:   map and verify that the capacity has been increased only if there
        //:   is no place for new items.  Verify that memory is allocated by
        //:   the allocator passed on the builder construction.  (C-3..4)
        //:
        //: 3 Create a 'DatumMapBuilder' object and get a 'Datum' object by
        //:   committing the map.  Verify that acquired object contains empty
        //:   map.  (C-5)
        //:
        //: 4 Create a 'DatumMapBuilder' object.  Append few elements to the
        //:   map and get a 'Datum' object by committing it.  Verify that
        //:   received object contains items in the same order as they has been
        //:   passed to the 'append' method.  (C-2,5)
        //:
        //: 5 Create a 'DatumMapBuilder' object.  Append few elements to the
        //:   map and let the object leave the scope.  Verify no memory,
        //:   allocated by allocator, given on object creation, is used.  (C-6)
        //:
        //: 6 Create a 'DatumMapBuilder' object.  Append few elements to the
        //:   map and get a 'Datum' object by committing it.  Let the builder
        //:   leave the scope.  Verify that 'Datum' object hasn't been changed.
        //:   (C-7)
        //:
        //: 7 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-8)
        //
        // Testing:
        //    DatumMapBuilder(SizeType, bslma::Allocator *);
        //    ~DatumMapBuilder();
        //    void append(const DatumMapEntry *, int);
        //    Datum commit();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;

        if (verbose) cout << "\nTesting constructor." << endl;
        {
            const size_t CAPACITY            = 10;
            const size_t NUM_ALLOCATED_BYTES =
                                        (CAPACITY + 1) * sizeof(DatumMapEntry);


            if (verbose) cout << "\tTesting constructor with parameters."
                              << endl;
            {
                bslma::TestAllocator ta("test", veryVeryVerbose);
                ASSERT(0 == ta.numBytesInUse());

                Obj        mB(CAPACITY, &ta);
                const Obj& B = mB;

                ASSERT(CAPACITY            == B.capacity());
                ASSERT(NUM_ALLOCATED_BYTES == ta.numBytesInUse());
            }


            if (verbose) cout <<
                     "\tTesting constructor with default allocator." << endl;
            {
                bslma::TestAllocator         ta("test", veryVeryVerbose);
                bslma::DefaultAllocatorGuard guard(&ta);

                Obj        mB(CAPACITY);
                const Obj& B = mB;

                ASSERT(CAPACITY            == B.capacity());
                ASSERT(NUM_ALLOCATED_BYTES == ta.numBytesInUse());
            }

            if (verbose) cout <<
                      "\tTesting constructor with default capacity." << endl;
            {
                bslma::TestAllocator         ta("test", veryVeryVerbose);
                bslma::DefaultAllocatorGuard guard(&ta);

                Obj        mB;
                const Obj& B = mB;

                ASSERT(0 == B.capacity());
                ASSERT(0 == ta.numBytesInUse());
            }
        }

        if (verbose) cout << "\nTesting 'append'." << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVerbose);

            {
                Obj        mB(1, &ta);
                const Obj& B = mB;

                ASSERT(1 == B.capacity());

                const bsls::Types::Int64 INIT_NUM_ALLOCATIONS =
                                                           ta.numAllocations();
                const bsls::Types::Int64 INIT_NUM_DEALLOCATIONS =
                                                         ta.numDeallocations();

                mB.append(values, 1);

                ASSERT(1                      == B.capacity());
                ASSERT(INIT_NUM_ALLOCATIONS   == ta.numAllocations());
                ASSERT(INIT_NUM_DEALLOCATIONS == ta.numDeallocations());

                mB.append(values + 1, 1);

                ASSERT(2                          == B.capacity());
                ASSERT(INIT_NUM_ALLOCATIONS + 1   == ta.numAllocations());
                ASSERT(INIT_NUM_DEALLOCATIONS + 1 == ta.numDeallocations());

                mB.append(values + 2, 3);

                ASSERT(8                          == B.capacity());
                ASSERT(INIT_NUM_ALLOCATIONS + 2   == ta.numAllocations());
                ASSERT(INIT_NUM_DEALLOCATIONS + 2 == ta.numDeallocations());
            }
            ASSERT(0 == ta.numBytesInUse());
        }

        if (verbose) cout << "\nTesting 'commit'." << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVerbose);

            if (verbose) cout << "\tCommiting empty map." << endl;
            {
                Obj        mB(0, &ta);
                const Obj& B = mB;
                ASSERT(0 == B.capacity());

                Datum        mD = mB.commit();
                const Datum& D = mD;

                ASSERT(true  == D.isMap());
                ASSERT(0     == D.theMap().size());
                ASSERT(false == D.theMap().isSorted());

                Datum::destroy(mD, &ta);
            }

            if (verbose) cout << "\tCommiting non-empty map." << endl;
            {
                Obj        mB(NUM_ELEMENTS, &ta);
                const Obj& B = mB;

                mB.append(values, NUM_ELEMENTS);

                ASSERT(NUM_ELEMENTS == B.capacity());

                Datum        mD = mB.commit();
                const Datum& D = mD;

                ASSERT(true == D.isMap());

                const DatumMapRef ref = D.theMap();

                ASSERT(NUM_ELEMENTS == ref.size());
                ASSERT(false        == ref.isSorted());

                for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
                    if (veryVerbose) {
                        T_ P_(i) P_(values[i].key()) P(ref[i].key())
                    }

                    ASSERTV(i, values[i].key() == ref[i].key());
                }

                Datum::destroy(mD, &ta);
            }
            ASSERT(0 == ta.numBytesInUse());
        }

        if (verbose) cout << "\nTesting destructor." << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVerbose);

            if (verbose) cout << "\tTesting empty object." << endl;
            {
                {
                    Obj firstMapBuilder(0, &ta);

                    // Let object go out the scope to call destructor.
                }
                ASSERT(0 == ta.numBytesInUse());
            }

            if (verbose) cout << "\tTesting object with uncommitted map."
                              << endl;
            {
                {
                    // Map entries creation.

                    Obj firstMapBuilder(3, &ta);
                    firstMapBuilder.append(values, 3);
                    Datum firstMap = firstMapBuilder.commit();

                    Obj secondMapBuilder(4, &ta);
                    secondMapBuilder.append(values, 4);
                    Datum secondMap = secondMapBuilder.commit();

                    DatumMapEntry entries[] = {
                        DatumMapEntry(StringRef("first"),  firstMap),
                        DatumMapEntry(StringRef("second"), secondMap)
                    };

                    ASSERT(0 != ta.numBytesInUse());

                    // Test object creation.

                    Obj mB (2, &ta);
                    mB.append(entries, 2);

                    // Let object go out the scope to call destructor.
                }

                // Verify that all datum objects have been destroyed and
                // allocated memory has been released.

                ASSERT(0 == ta.numBytesInUse());
            }

            if (verbose) cout << "\tTesting object with committed map."
                              << endl;
            {
                Datum        mD;
                const Datum& D = mD;

                {
                    Obj mB(NUM_ELEMENTS, &ta);
                    mB.append(values, NUM_ELEMENTS);

                    mD = mB.commit();

                    ASSERT(true         == D.isMap());
                    const DatumMapRef ref = D.theMap();

                    ASSERT(NUM_ELEMENTS == ref.size());
                    ASSERT(false        == ref.isSorted());

                    for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
                        if (veryVerbose) {
                            T_ P_(i) P_(values[i].key()) P(ref[i].key())
                        }

                        ASSERTV(i, values[i].key() == ref[i].key());
                    }

                    // Let object go out the scope to call destructor.
                }

                // Verify that created datum object hasn't been affected.

                ASSERT(true == D.isMap());

                const DatumMapRef ref = D.theMap();

                ASSERT(NUM_ELEMENTS == ref.size());
                ASSERT(false        == ref.isSorted());

                for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
                    if (veryVerbose) {
                        T_ P_(i) P_(values[i].key()) P(ref[i].key())
                    }

                    ASSERTV(i, values[i].key() == ref[i].key());
                }

                ASSERT(0 != ta.numBytesInUse());

                Datum::destroy(mD, &ta);
                ASSERT(0 == ta.numBytesInUse());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVerbose);

            bsls::AssertFailureHandlerGuard hG(
                                           bsls::AssertTest::failTestDriver);
            if (verbose) cout << "\tTesting 'append'." << endl;
            {
                Obj           mB(0, &ta);
                DatumMapEntry entry(StringRef("first"),
                                    Datum::createInteger(1));

                ASSERT_SAFE_FAIL(mB.append(0,      0));
                ASSERT_SAFE_FAIL(mB.append(&entry, 0));
                ASSERT_SAFE_FAIL(mB.append(0,      1));
                ASSERT_SAFE_PASS(mB.append(&entry, 1));
            }

            if (verbose) cout << "\tTesting 'commit'." << endl;
            {
                DatumMapEntry first (StringRef("first"),
                                     Datum::createInteger(1));
                DatumMapEntry second(StringRef("second"),
                                     Datum::createInteger(2));

                Obj realSorted(2, &ta);
                realSorted.append(&first, 1);
                realSorted.append(&second, 1);

                Obj fakeSorted(2, &ta);
                fakeSorted.append(&second, 1);
                fakeSorted.append(&first, 1);

                Obj unsorted(2, &ta);
                unsorted.append(&second, 1);
                unsorted.append(&first, 1);

                // Obtained map has 'sorted' flag set to 'false' by default.
                // To test assertion we need to change its value.  'setSorted'
                // hasn't been tested yet, but there is no other method to
                // setup 'sorted' flag of map.  And to test 'setSorted' we have
                // to use 'commit' method to obtain map with its accessor.  So
                // 'setSorted' is bootstrapped as simpler method.

                realSorted.setSorted(true);
                fakeSorted.setSorted(true);
                unsorted.setSorted(false);   // unnecessary

                Datum realSortedMap;
                Datum fakeSortedMap;
                Datum   unsortedMap;

                ASSERT_SAFE_PASS(realSortedMap = realSorted.commit());
                ASSERT_SAFE_FAIL(fakeSortedMap = fakeSorted.commit());
                ASSERT_SAFE_PASS(unsortedMap   = unsorted.commit());

                Datum::destroy(realSortedMap, &ta);
                Datum::destroy(fakeSortedMap, &ta);
                Datum::destroy(unsortedMap  , &ta);
            }
            ASSERT(0 == ta.numBytesInUse());
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
        //: 1 Developer test sandbox.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        {
            bslma::TestAllocator ta(veryVeryVerbose);
            {
                // Basic test.

                DatumMapBuilder builder(10, &ta);
                StringRef       first("first");
                builder.pushBack(first, Datum::createInteger(1));
                Datum map = builder.commit();
                Datum::destroy(map, &ta);
            }

            {
                // Testing empty StringRef.

                DatumMapBuilder builder(10, &ta);
                StringRef       first("first");
                builder.pushBack((StringRef()), Datum::createInteger(1));
                builder.pushBack(first, Datum::createInteger(2));
                builder.pushBack((StringRef()), Datum::createInteger(3));
                Datum        map = builder.commit();
                ASSERT(3  == map.theMap().size());
                const Datum *entry = map.theMap().find("first");
                ASSERT(2  == entry->theInteger());
                entry = map.theMap().find((StringRef()));
                ASSERT(1  == entry->theInteger());
                ASSERT(3  == map.theMap()[2].value().theInteger());
                Datum::destroy(map, &ta);
            }
            ASSERT(0 == ta.numBytesInUse());
        }
      } break;
      default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the default allocator.

    ASSERT(dam.isTotalSame());

    // CONCERN: In no case does memory come from the global allocator.

    ASSERT(gam.isTotalSame());

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
