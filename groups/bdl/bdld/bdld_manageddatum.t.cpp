// bdld_manageddatum.t.cpp                                            -*-C++-*-
#include <bdld_manageddatum.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimeinterval.h>
#include <bdlt_time.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bslim_testutil.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bslstl;
using namespace bdld;

// ============================================================================
//                                TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test is an full *value-semantic* class that represents
// general purpose values and their datums.  Our testing concerns are limited
// to creating 'ManagedDatum' objects with different types of values,
// retrieving the type and actual value within these objects, copying these
// objects, assigning to these objects, streaming these objects and finally
// destroying them.  At the end of these operations, the same amount of memory
// should be deallocated as was allocated while performing these operations.
//
// Note that 'ManagedDatum' behavior is independent of held 'Datum' object's
// type or value, so we don't need to test component methods with all possible
// 'Datum' types, but at least with one of them.
// ----------------------------------------------------------------------------
// CREATORS
// [ 5] ManagedDatum(bslma::Allocator *basicAllocator);
// [ 2] ManagedDatum(const Datum&, bslma::Allocator *);
// [ 5] ManagedDatum(const ManagedDatum&, bslma::Allocator *);
// [ 2] ~ManagedDatum();
//
// MANIPULATORS
// [ 9] ManagedDatum& operator=(const ManagedDatum&);
// [ 8] void adopt(const Datum&);
// [ 8] void clone(const Datum& value);
// [ 8] Datum release();
// [ 8] void makeNull();
// [ 7] void swap(ManagedDatum& other);
//
// ACCESSORS
// [ 4] const Datum& datum() const;
// [ 4] bslma::Allocator *allocator() const;
// [ 4] const Datum *operator->() const;
// [ 4] const Datum& operator*() const;
// [11] bsl::ostream& print(bsl::ostream&, int, int) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const ManagedDatum&, const ManagedDatum&);
// [ 6] bool operator!=(const ManagedDatum&, const ManagedDatum&);
// [10] bsl::ostream& operator<<(bsl::ostream&, const ManagedDatum&);
//
// FREE FUNCTIONS
// [13] void swap(ManagedDatum& a, ManagedDatum& b);
//
// TRAITS
// [12] bslmf::IsBitwiseEqualityComparable
// [12] bslma::UsesBslmaAllocator
// [12] bsl::is_trivially_copyable
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] TEST APPARATUS
// [14] CONCERN: bsl::vector<ManagedDatum> (DRQS 90054827)
// [15] USAGE EXAMPLE

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
typedef ManagedDatum Obj;
typedef bsls::Types::Int64 Int64;

const double k_DBL_MIN2    = numeric_limits<double>::min();
const double k_DBL_MAX2    = numeric_limits<double>::max();
const double k_DBL_INF     = numeric_limits<double>::infinity();
const double k_DBL_QNAN2   = numeric_limits<double>::quiet_NaN();
const double k_DBL_SNAN    = numeric_limits<double>::signaling_NaN();
const double k_DBL_NEGINF  = -1 * k_DBL_INF;
const double k_DBL_ZERO    = 0.0;
const double k_DBL_NEGZERO = 1 / k_DBL_NEGINF;

const int    UDT_TYPE      = 2;
// ============================================================================
//                              TEST APPARATUS
// ----------------------------------------------------------------------------
Datum createArray(bslma::Allocator *allocator)
    // Create a fixed size array of 'Datum' objects with different value types
    // using the specified 'allocator'.
{
    DatumMutableArrayRef         array;
    const bsls::Types::size_type arraySize = 6;
    Datum::createUninitializedArray(&array,
                                    arraySize,
                                    allocator);
    array.data()[0] = Datum::createInteger(0);
    array.data()[1] = Datum::createDouble(-3.1416);
    array.data()[2] = Datum::copyString("A very long string", allocator);
    array.data()[3] = Datum::copyString("Abc", allocator);
    array.data()[4] = Datum::createDate(bdlt::Date(2010, 1, 5));
    array.data()[5] = Datum::createDatetime(
                            bdlt::Datetime(2010, 1, 5, 16, 45, 32, 12),
                            allocator);
    *(array.length()) = arraySize;
    return Datum::adoptArray(array);
}

Datum createEmptyArray()
    // Create an empty array of 'Datum' objects.
{
    DatumMutableArrayRef array;
    return Datum::adoptArray(array);
}

Datum createEmptyMap()
    // Create an empty map of 'Datum' objects.
{
    DatumMutableMapRef map;
    return Datum::adoptMap(map);
}

Datum createMap(bslma::Allocator *allocator)
    // Create a fixed size map of 'Datum' objects with different value types
    // keyed by string values using the specified 'allocator'.
{
    DatumMutableMapRef           map;
    const bsls::Types::size_type mapSize = 6;
    Datum::createUninitializedMap(&map,
                                  mapSize,
                                  allocator);
    map.data()[0] = DatumMapEntry(
                         StringRef("first", static_cast<int>(strlen("first"))),
                         Datum::createInteger(0));
    map.data()[1] = DatumMapEntry(
                       StringRef("second", static_cast<int>(strlen("second"))),
                       Datum::createDouble(-3.1416));
    map.data()[2] = DatumMapEntry(
                         StringRef("third", static_cast<int>(strlen("third"))),
                         Datum::copyString("A very long string", allocator));
    map.data()[3] = DatumMapEntry(
                       StringRef("fourth", static_cast<int>(strlen("fourth"))),
                       Datum::copyString("Abc", allocator));
    map.data()[4] = DatumMapEntry(
                         StringRef("fifth", static_cast<int>(strlen("fifth"))),
                         Datum::createDate(bdlt::Date(2010, 1, 5)));
    map.data()[5] = DatumMapEntry(
                         StringRef("sixth", static_cast<int>(strlen("sixth"))),
                         Datum::createDatetime(
                                    bdlt::Datetime(2010, 1, 5, 16, 45, 32, 12),
                                    allocator));
    *(map.size()) = mapSize;
    return Datum::adoptMap(map);
}

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------
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
      case 15: {
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
///Example 1: Basic Use of 'bdld::MangedDatum'
///- - - - - - - - - - - - - - - - - - - - - -
// The example demonstrates the basic construction and manipulation of a
// 'ManagedDatum' object.
//
// First, we create a 'ManagedDatum' object that holds a double value and
// verify that it has the same double value inside it:
//..
    bslma::TestAllocator ta("test", veryVeryVerbose);

    const ManagedDatum realObj(Datum::createDouble(-3.4375), &ta);

    ASSERT(realObj->isDouble());
    ASSERT(-3.4375 == realObj->theDouble());
//..
// Next, we create a 'ManagedDatum' object that holds a string value and verify
// that it has the same string value inside it:
//..
    const char         *str = "This is a string";
    const ManagedDatum  strObj(Datum::copyString(str, &ta), &ta);

    ASSERT(strObj->isString());
    ASSERT(str == strObj->theString());
//..
// Then, assign this 'ManagedDatum' object to another object and verify both
// objects have the same value:
//..
    ManagedDatum strObj1(&ta);
    strObj1 = strObj;
    ASSERT(strObj == strObj1);
//..
// Next, copy-construct this 'ManagedDatum' object and verify that the copy has
// the same value as the original:
//..
    const ManagedDatum strObj2(strObj, &ta);
    ASSERT(strObj == strObj2);
//..
// Then, we create a 'ManagedDatum' object that holds an opaque pointer to a
// 'bdlt::Date' object and verify that it has the same user-defined value
// inside it:
//..
    bdlt::Date   udt;
    ManagedDatum udtObj(Datum::createUdt(&udt, UDT_TYPE), &ta);

    ASSERT(udtObj->isUdt());
    ASSERT(&udt == udtObj->theUdt().data());
    ASSERT(UDT_TYPE == udtObj->theUdt().type());
//..
// Next, we assign a boolean value to this 'ManagedDatum' object and verify
// that it has the new value:
//..
    udtObj.adopt(Datum::createBoolean(true));
    ASSERT(udtObj->isBoolean());
    ASSERT(true == udtObj->theBoolean());
//..
// Then, we create a 'ManagedDatum' object having an array and verify that it
// has the same array value.  Note that in practice we would use
// {'bdld_datumarraybuilder'}, but do not do so here to for dependency reasons.
//..
    const Datum datumArray[2] = {
        Datum::createInteger(12),
        Datum::copyString("A long string", &ta)
    };

    DatumMutableArrayRef arr;
    Datum::createUninitializedArray(&arr, 2 , &ta);
    for (int i = 0; i < 2; ++i) {
        arr.data()[i] = datumArray[i];
    }
    *(arr.length()) = 2;
    const ManagedDatum arrayObj(Datum::adoptArray(arr), &ta);

    ASSERT(arrayObj->isArray());
    ASSERT(DatumArrayRef(datumArray, 2) == arrayObj->theArray());
//..
// Next, we create a 'ManagedDatum' object having a map and verify that it has
// the same map value.  Note that in practice we would use
// {'bdld_datummapbuilder'}, but do not do so here to for dependency reasons.
//..
    const DatumMapEntry datumMap[2] = {
        DatumMapEntry(StringRef("first", static_cast<int>(strlen("first"))),
                      Datum::createInteger(12)),
        DatumMapEntry(StringRef("second", static_cast<int>(strlen("second"))),
                      Datum::copyString("A very long string", &ta))
    };

    DatumMutableMapRef mp;
    Datum::createUninitializedMap(&mp, 2 , &ta);
    for (int i = 0; i < 2; ++i) {
        mp.data()[i] = datumMap[i];
    }
    *(mp.size()) = 2;
    const ManagedDatum mapObj(Datum::adoptMap(mp), &ta);

    ASSERT(mapObj->isMap());
    ASSERT(DatumMapRef(datumMap, 2, false, false) == mapObj->theMap());
//..
// Then, we create a 'Datum' object and assign its ownership to a
// 'ManagedDatum' object and verify that the ownership was transferred:
//..
    const Datum  rcObj = Datum::copyString("This is a string", &ta);
    ManagedDatum obj(Datum::createInteger(1), &ta);
    obj.adopt(rcObj);
    ASSERT(obj.datum() == rcObj);
//..
// Next, we release the 'Datum' object inside the 'ManagedDatum' object and
// verify that it was released:
//..
    const Datum internalObj = obj.release();
    ASSERT(obj->isNull());
    ASSERT(internalObj == rcObj);
//..
// Finally, we destroy the released 'Datum' object:
//..
    Datum::destroy(internalObj, obj.allocator());
//..
      } break;
      case 14: {
        //---------------------------------------------------------------------
        // CONCERN: bsl::vector<ManagedDatum> (DRQS 90054827)
        //   A previous test case, 12, verifies the traits defined by
        //   'ManagedDatum' are set as the component author expected.  This
        //   (regression) test-case extends case 12 to verify that the
        //   intended value of the traits chosen by the component author
        //   results in reasonable expected behavior when a 'ManagedDatum' is
        //   loaded into a container that uses those traits.
        //
        // Concerns:
        //: 1 That a vector of 'bdld::ManagedDatum' objects correctly
        //:   releases the resources of those objects when the container
        //:   is destroyed.
        //:
        //: 2 That a vector of 'bdld::ManagedDatum' objects correctly
        //:   manages resources of those objects when the underlying capacity
        //:   is reallocated.
        //
        // Plan:
        //: 1 Create a vector of 'ManagedDatum' objects using a test-allocator
        //:   and insert a series of 'ManagedDatum' objects requiring memory
        //:   allocation.  Observe that the 'ManagedDatum' objects use
        //:   the test allocator (and not the default allocator), and
        //:   that no memory is leaked upon the vector's destructor. (C-1,2)
        //
        // Testing:
        //   CONCERN: bsl::vector<ManagedDatum> (DRQS 90054827)
        //---------------------------------------------------------------------
        if (verbose)
            cout << "CONCERN: bsl::vector<ManagedDatum> (DRQS 90054827)\n"
                 << "==================================================\n";

        if (verbose)
            cout << "\nCreate a vector of managed datum objects" << endl;

        {
            const char *DATA = "0123456789012345678901234567890123456789"
                               "0123456789012345678901234567890123456789";

            bslma::TestAllocator        testAllocator;
            bsl::vector<Obj>            x(&testAllocator);
            bslma::TestAllocatorMonitor dam(&defaultAllocator);
            for (int i = 0; i < 100; ++i) {
                bdld::Datum datum;
                if (veryVerbose) {
                    P(i);
                }

                {
                    bslma::TestAllocatorMonitor tam(&testAllocator);
                    datum = bdld::Datum::copyString(bslstl::StringRef(DATA),
                                                    &testAllocator);
                    ASSERT(true  == tam.isInUseUp());
                    ASSERT(false == dam.isInUseUp());
                }
                {
                    bslma::TestAllocatorMonitor tam(&testAllocator);

                    Obj newElement(datum, &testAllocator);

                    ASSERT(false == tam.isInUseUp());
                    ASSERT(false == dam.isInUseUp());

                    x.push_back(newElement);

                    ASSERT(false == dam.isInUseUp());
                }
                ASSERT(false == dam.isTotalUp());
                ASSERT(false == dam.isInUseUp());
                ASSERT(false == dam.isMaxUp());
            }
            ASSERT(false == dam.isTotalUp());
            ASSERT(false == dam.isInUseUp());
            ASSERT(false == dam.isMaxUp());
        }
      } break;
      case 13: {
        //---------------------------------------------------------------------
        // TESTING FREE FUNCTION 'swap'
        //
        // Concerns:
        //: 1 The 'swap' function swaps object with the same and different
        //:   allocators.
        //
        // Plan:
        //: 1 Create 'Datum' objects with a set of allocators. Ensure that free
        //:   'swap' function correctly swaps those objects.  (C-1)
        //
        // Testing:
        //   void swap(ManagedDatum& a, ManagedDatum& b);
        //---------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING FREE FUNCTION 'swap'" << endl
                          << "============================" << endl;
        const char* str1 = "First long test string";
        const char* str2 = "Second long test string";

        if (verbose)
            cout << "\nSwapping objects with the same allocator." << endl;
        {
            bslma::TestAllocator ta("test", veryVeryVerbose);

            {
                Obj        mMD1(Datum::copyString(str1, &ta), &ta);
                const Obj& MD1 = mMD1;
                Obj        mMD2(Datum::copyString(str2, &ta), &ta);
                const Obj& MD2 = mMD2;

                ASSERT(true == MD1->isString());
                ASSERT(str1 == MD1->theString());
                ASSERT(true == MD2->isString());
                ASSERT(str2 == MD2->theString());

                swap(mMD1, mMD2);

                ASSERT(true == MD1->isString());
                ASSERT(str2 == MD1->theString());
                ASSERT(true == MD2->isString());
                ASSERT(str1 == MD2->theString());
            }
            ASSERT(0 == ta.numBytesInUse());
        }

        if (verbose)
            cout << "\nSwapping objects with different allocators." << endl;
        {
            bslma::TestAllocator ta1("test1", veryVeryVerbose);
            bslma::TestAllocator ta2("test2", veryVeryVerbose);

            {
                Obj        mMD1(Datum::copyString(str1, &ta1), &ta1);
                const Obj& MD1 = mMD1;
                Obj        mMD2(Datum::copyString(str2, &ta2), &ta2);
                const Obj& MD2 = mMD2;

                ASSERT(true == MD1->isString());
                ASSERT(str1 == MD1->theString());
                ASSERT(true == MD2->isString());
                ASSERT(str2 == MD2->theString());

                swap(mMD1, mMD2);

                ASSERT(true == MD1->isString());
                ASSERT(str2 == MD1->theString());
                ASSERT(true == MD2->isString());
                ASSERT(str1 == MD2->theString());
            }
            ASSERT(0 == ta1.numBytesInUse());
            ASSERT(0 == ta2.numBytesInUse());
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING TRAITS
        //
        // Concerns:
        //: 1 bslmf::IsBitwiseMoveable is true for ManagedDatum.
        //:
        //: 2 bslma::UsesBslmaAllocator is true for ManagedDatum.
        //:
        //: 3 bsl::is_trivially_copyable is true for ManagedDatum.
        //
        // Plan:
        //: 1 Assert each trait.  (C-1..3)
        //
        // Testing:
        //   bslmf::IsBitwiseEqualityComparable
        //   bslma::UsesBslmaAllocator
        //   bsl::is_trivially_copyable
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING TRAITS" << endl
                          << "==============" << endl;

        ASSERT(true  == bslmf::IsBitwiseMoveable<Obj>::value);
        ASSERT(true  == bslma::UsesBslmaAllocator<Obj>::value);
        ASSERT(false == bsl::is_trivially_copyable<Obj>::value);
      } break;
      case 11: {
        //---------------------------------------------------------------------
        // TESTING 'print'
        //
        // Concerns:
        //: 1 The 'print' method should print held 'Datum' object to the
        //:   specified 'stream' in the appropriate format.
        //
        // Plan:
        //: 1 Use two different 'ostringstreams' to 'print' held value and
        //:   object. Compare the contents of different streams.  (C-1)
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream&, int, int) const;
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'print'" << endl
                                  << "===============" << endl;

        bslma::TestAllocator ta("test", veryVeryVerbose);

        ostringstream datumStream;
        ostringstream managedDatumStream;

        Datum      intDatum = Datum::createInteger(1);
        Obj        mMD(intDatum, &ta);
        const Obj& MD = mMD;

        const int  LVL = 0;  // indentation level
        const int  SPL = 0;  // spaces per level

        intDatum.print(datumStream, LVL, SPL);
        MD.print(managedDatumStream, LVL, SPL);

        ASSERT(datumStream.str() == managedDatumStream.str());
        ASSERT(0                 == ta.numBytesInUse());
      } break;
      case 10: {
        //---------------------------------------------------------------------
        // TESTING STREAMING OPERATOR
        //
        // Concerns:
        //: 1 The output streaming operator prints held 'Datum' object to the
        //:   specified 'stream'.
        //
        // Plan:
        //: 1 Verify that the output streaming operator produces the same
        //:   result as the output streaming operator of the held 'Datum'
        //:   object.  (C-1)
        //
        // Testing:
        //   bsl::ostream& operator<<(bsl::ostream&, const ManagedDatum&);
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING STREAMING OPERATOR" << endl
                                  << "==========================" << endl;

        bslma::TestAllocator ta("test", veryVeryVerbose);

        const Datum        D = Datum::copyString("A very long string", &ta);
        const ManagedDatum MD(D, &ta);

        {
            // Temporary resetting default allocator because we construct
            // temporary strings in this test
            bslma::TestAllocator         sa("stream", veryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&sa);

            ostringstream datumStream;
            ostringstream managedDatumStream;

            datumStream << D;
            managedDatumStream << MD;

            ASSERT(datumStream.str() == managedDatumStream.str());
        }
      } break;
      case 9: {
        //---------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //
        // Concerns:
        //: 1 The assignment operator destroys the existing 'Datum'
        //:   object held onto by this 'Datum' and release any dynamically
        //:   allocated memory held inside the 'Datum' object.
        //:
        //: 2 A new 'Datum' object is created out of the passed in object.
        //:   Dynamic memory is only allocated when needed.
        //:
        //: 3 The currently stored allocator is used to allocate any memory.
        //:
        //: 4 The object that is assigned, should not be modified.
        //
        // Plan:
        //: 1 Create 'ManagedDatum' object holding 'Datum' object with integer
        //:   value.  Create another 'ManagedDatum' object holding 'Datum'
        //:   object with sufficiently long string value, so as to trigger a
        //:   dynamic memory allocation.  Assign second one to first one.
        //:   Verify, that 'ManagedDatum' objects are equal.  Verify that
        //:   first object's allocator has been used to allocate memory for
        //:   string value.  Verify that second object hasn't been changed.
        //:   (C-1..4)
        //:
        //: 2 Create 'ManagedDatum' object holding 'Datum' object with
        //:   sufficiently long string value, so as to trigger a dynamic memory
        //:   allocation.  Create another 'ManagedDatum' object holding 'Datum'
        //:   object with integer value.  Assign second one to first one.
        //:   Verify, that 'ManagedDatum' objects are equal.  Verify, that
        //:   memory for holding string value has been released.  Verify that
        //:   second object hasn't been changed.  (C-1..4)
        //
        // Testing:
        //   ManagedDatum& operator=(const ManagedDatum&);
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING ASSIGNMENT OPERATOR" << endl
                                  << "===========================" << endl;

        bslma::TestAllocator ta("test", veryVeryVerbose);
        bslma::TestAllocator aa("assignment", veryVeryVeryVerbose);

        if (verbose) cout << "\nTesting memory allocation." << endl;
        {
            ASSERT(0 == ta.numBytesInUse());
            ASSERT(0 == aa.numBytesInUse());

            Obj mMDLeft(Datum::createInteger(1), &ta);
            Obj mMDRight(Datum::copyString("A very long string", &aa), &aa);

            ASSERT(0 == ta.numBytesInUse());
            ASSERT(0 != aa.numBytesInUse());

            const Int64 NUM_BYTES_IN_USE = aa.numBytesInUse();

            mMDLeft = mMDRight;

            ASSERT(mMDLeft          == mMDRight);
            ASSERT(true             == mMDRight->isString());
            ASSERT(0                != ta.numBytesInUse());
            ASSERT(NUM_BYTES_IN_USE == aa.numBytesInUse());
        }

        if (verbose) cout << "\nTesting memory deallocation." << endl;
        {
            ASSERT(0 == ta.numBytesInUse());
            ASSERT(0 == aa.numBytesInUse());

            Obj mMDLeft(Datum::copyString("A very long string", &ta), &ta);
            Obj mMDRight(Datum::createInteger(1), &aa);

            ASSERT(0 != ta.numBytesInUse());
            ASSERT(0 == aa.numBytesInUse());

            mMDLeft = mMDRight;

            ASSERT(mMDLeft == mMDRight);
            ASSERT(true == mMDRight->isInteger());
            ASSERT(0 == ta.numBytesInUse());
            ASSERT(0 == aa.numBytesInUse());
        }

        ASSERT(0 == ta.status());
        ASSERT(0 == aa.status());
      } break;
      case 8: {
        //---------------------------------------------------------------------
        // TESTING MANIPULATORS
        //
        // Concerns:
        //: 1 The manipulators should destroy the existing 'Datum'
        //:   object held onto by this 'Datum' object and release any
        //:   dynamically allocated memory held inside the 'Datum'
        //:   object.
        //:
        //: 2 A new 'Datum' object should be created out of the
        //:   value passed to the manipulator.
        //:
        //: 3 Dynamic memory is allocated only when needed.
        //:
        //: 4 The currently stored allocator is used to allocate and release
        //:   any memory.
        //:
        //: 5 Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create 'Datum' object with a sufficiently long string value,
        //:   so as to trigger a dynamic memory allocation. Call manipulator.
        //:   Confirm that previously held 'Datum' object is destroyed and its
        //:   memory is released.  (C-1)
        //:
        //: 2 Use the basic accessors to verify that the type and value inside
        //:   the 'Datum' objects are as expected.  (C-2)
        //:
        //: 3 Verify that memory allocations are as expected.  (C-3..4)
        //:
        //: 4 Finally destroy these objects and verify that no memory has
        //:   leaked.  (C-4)
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones.  (C-5)
        //
        // Testing:
        //   void makeNull();
        //   void adopt(const Datum&);
        //   void clone(const Datum& value);
        //   Datum release();
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING MANIPULATORS" << endl
                                  << "====================" << endl;

        bslma::TestAllocator ta("test", veryVeryVerbose);

        if (verbose) cout << "\nTesting 'makeNull'." << endl;
        {
            // Create 'ManagedDatum' with a string value.

            Obj        mMD(Datum::copyString("A long string", &ta), &ta);
            const Obj& MD = mMD;
            // Call 'makeNull' and verify that the object is destroyed.

            mMD.makeNull();
            ASSERT(true == MD->isNull());
        }

        // Verify no memory was leaked.

        ASSERT(0 == ta.status());

        if (verbose) cout << "\nTesting 'adopt' method." << endl;
        {
            // Create ManagedDatum with a string value.

            Obj        mMD(Datum::copyString("A very long string", &ta), &ta);
            const Obj& MD = mMD;

            // Adopt a Datum object and verify that the previous object is
            // destroyed and the new object is adopted and not copied.

            const Datum D = Datum::copyString("Another very long string", &ta);
            Int64       numDeallocs = ta.numDeallocations();
            const Int64 numAllocs = ta.numAllocations();

            mMD.adopt(D);

            ASSERT(D           == MD.datum());
            ASSERT(numDeallocs <= ta.numDeallocations());
            ASSERT(numAllocs   == ta.numAllocations());
        }

        // Verify no memory was leaked.

        ASSERT(0 == ta.status());

        if (verbose) cout << "\nTesting 'clone' method." << endl;
        {
            // Create ManagedDatum with a string value.

            Obj        mMD(Datum::copyString("A very long string", &ta), &ta);
            const Obj& MD = mMD;

            // Clone a Datum object and verify that the previous object is
            // destroyed and the new object is copied.

            const Datum D = Datum::copyString("Another very long string", &ta);
            Int64       numDeallocs = ta.numDeallocations();
            Int64       numAllocs = ta.numAllocations();

            mMD.clone(D);

            ASSERT(D           == MD.datum());
            ASSERT(numDeallocs <= ta.numDeallocations());
            ASSERT(numAllocs   <  ta.numAllocations());

            Datum::destroy(D, &ta);
        }

        // Verify no memory was leaked.

        ASSERT(0 == ta.status());

        if (verbose) cout << "\nTesting 'release' method." << endl;
        {
            // Create ManagedDatum with a string value.

            Obj         mMD(Datum::copyString("A very long string", &ta), &ta);
            const Obj&  MD = mMD;
            const Datum DInternal = MD.datum();

            // Call release on the object and verify that the previous value is
            // returned and it is not destroyed.  Also verify that the object
            // does not have any value.

            const Int64 NUM_DEALLOCATIONS = ta.numDeallocations();

            const Datum DResult = mMD.release();

            ASSERT(true              == MD->isNull());
            ASSERT(DInternal         == DResult);
            ASSERT(NUM_DEALLOCATIONS == ta.numDeallocations());

            // Destroy the released 'Datum' object.

            Datum::destroy(DResult, &ta);
        }

        // Verify no memory was leaked.

        ASSERT(0 == ta.status());

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            // Adopt an object already stored inside.

            Obj        mMD(Datum::copyString("A very long string", &ta), &ta);
            const Obj& MD = mMD;

            ASSERT_SAFE_PASS(mMD.adopt(MD.datum()));
            ASSERT_SAFE_PASS(mMD.adopt(
                                  Datum::copyString("Another very long string",
                                  &ta)));
        }
        // Verify no memory was leaked.

        ASSERT(0 == ta.status());
      } break;
      case 7: {
        //---------------------------------------------------------------------
        // TESTING 'swap'
        //
        // Concerns:
        //: 1 The 'swap' method should swap the corresponding (underlying)
        //:   'Datum' objects.
        //:
        //: 2 The 'swap' method does't allocate any additional memory.
        //
        // Plan:
        //: 1 Create 'Datum' object with a sufficiently long string value, so
        //:   as to trigger a dynamic memory allocation. Create 'Datum' objects
        //:   with each of the different types of values and swap them with the
        //:   first object and verify that the objects were indeed swapped and
        //:   there were no memory allocations or deallocations (in the
        //:   process). Swap these objects back and verify that they in their
        //:   original state. Display steps of operation frequently in verbose
        //:   mode.  (C-1..2)
        //
        // Testing:
        //   void swap(ManagedDatum& other);
        //---------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'swap'" << endl
                          << "==============" << endl;

        bslma::TestAllocator ta("test", veryVeryVerbose);
        bdlt::Date           udt;

        const struct {
            Datum  d_obj;      // object for exchange
            string d_message;  // log message
        } DATA[] = {
            {
                Datum::createNull(),
                "null"
            },
            {
                Datum::createInteger(1),
                "integer"
            },
            {
                Datum::createBoolean(true),
                "boolean"
            },
            {
                Datum::createError(5, "some error", &ta),
                "Error"
            },
            {
                Datum::createDate(bdlt::Date(2010, 1, 5)),
                "Date"
            },
            {
                Datum::createTime(bdlt::Time(16, 45, 32, 12)),
                "Time"
            },
            {
                Datum::copyString("Abc", &ta),
                "short string"
            },
            {
                Datum::copyString("A very long string", &ta),
                "long string"
            },
            {
                Datum::createDatetime(
                              bdlt::Datetime(2010, 1, 5, 16, 45, 32, 12), &ta),
                "DateTime"
            },
            {
                Datum::createDatetimeInterval(
                              bdlt::DatetimeInterval(34, 16, 45, 32, 12), &ta),
                "DatetimeInterval"
            },
            {
                Datum::createInteger64(9223372036854775807LL, &ta),
                "Int64"
            },
            {
                Datum::createUdt(&udt, UDT_TYPE),
                "user defined"
            },
            {
                Datum::createDouble(-3.14),
                "double"
            },
            {
                createArray(&ta),
                "array"
            },
            {
                createEmptyArray(),
                "array"
            },
            {
                createMap(&ta),
                "map"
            },
            {
                createEmptyMap(),
                "map"
            }
        };

        const bsl::size_t NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        for (bsl::size_t i = 0; i < NUM_DATA; ++i) {

            if (veryVerbose) {
                T_ P_(i) P(DATA[i].d_message)
            }
            const char* str = "Another very long string";
            // Swap two ManagedDatum objects and verify that there were no
            // memory allocations or deallocations.

            Obj        mMD1(Datum::copyString(str, &ta), &ta);
            const Obj& MD1 = mMD1;
            Obj        mMD2(DATA[i].d_obj, &ta);
            const Obj& MD2 = mMD2;

            const Int64 numCurrAllocs = ta.numAllocations();
            const Int64 numCurrDeallocs = ta.numDeallocations();

            mMD1.swap(mMD2);

            ASSERTV(i, numCurrAllocs == ta.numAllocations());
            ASSERTV(i, numCurrDeallocs == ta.numDeallocations());

            // Verify that both objects have expected values.

            ASSERTV(i, MD2->isString() && str == MD2->theString());
            ASSERTV(i, MD1.datum() == DATA[i].d_obj);

           // Swap them back and verify the values.

            mMD2.swap(mMD1);

            ASSERTV(i, ta.numAllocations() == numCurrAllocs);
            ASSERTV(i, ta.numDeallocations() == numCurrDeallocs);
            ASSERTV(i, MD1->isString() && str == MD1->theString());
            ASSERTV(i, DATA[i].d_obj == MD2.datum());
        }

        ASSERT(0 == ta.status());

      } break;
      case 6: {
        //---------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //
        // Concerns:
        //: 1 Two objects compare equal if and only if their corresponding
        //:   contained 'Datum' objects compare equal.
        //:
        //: 2 Comparison is symmetric.
        //:
        //: 3 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 4 The equality-comparison operators' signatures and return types
        //:   are standard.
        //
        // Plan:
        //: 1 Create 'Datum' objects of different types. Also create 'Datum'
        //:   objects of the same type but with subtle variations in values.
        //:   Verify the correctness of 'operator==' and 'operator!='.
        //:   (C-1..3)
        //:
        //: 2 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.  (C-4)
        //
        // Testing:
        //   bool operator==(const ManagedDatum&, const ManagedDatum&);
        //   bool operator!=(const ManagedDatum&, const ManagedDatum&);
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING EQUALITY OPERATORS" << endl
                                  << "==========================" << endl;

        bslma::TestAllocator ta("test", veryVeryVerbose);

        if (verbose) cout << "\nComparison testing." << endl;
        {
            // Creating data for comparison.

            bdlt::Date udt;
            bdlt::Date udt1;

            static struct {
                int         d_line;
                bdld::Datum d_datum;
            } DATA[] = {
            { L_, Datum::createNull() },
            { L_, Datum::createInteger(0) },
            { L_, Datum::createInteger(1) },
            { L_, Datum::createBoolean(true) },
            { L_, Datum::createBoolean(false) },
            { L_, Datum::createError(0) },
            { L_, Datum::createError(1, "some error", &ta) },
            { L_, Datum::createDate(bdlt::Date(2010, 1, 5)) },
            { L_, Datum::createDate(bdlt::Date(1, 1, 1)) },
            { L_, Datum::createTime(bdlt::Time(16, 45, 32, 12)) },
            { L_, Datum::createTime(bdlt::Time(1, 1, 1, 1)) },
            { L_, Datum::createDatetime(
                bdlt::Datetime(2010, 1, 5, 16, 45, 32, 12), &ta) },
            { L_, Datum::createDatetime(
                   bdlt::Datetime(1, 1, 1, 1, 1, 1, 1), &ta) },
            { L_, Datum::createDatetimeInterval(
                        bdlt::DatetimeInterval(34, 16, 45, 32, 12), &ta) },
            { L_, Datum::createDatetimeInterval(
                            bdlt::DatetimeInterval(1, 1, 1, 1, 1), &ta) },
            { L_, Datum::createInteger64(9223372036854775807LL, &ta) },
            { L_, Datum::createInteger64(1229782938247303441LL, &ta) },
            { L_, Datum::copyString("A long string", &ta) },
            { L_, Datum::copyString("A very long string", &ta) },
            { L_, Datum::copyString("abc", &ta) },
            { L_, Datum::copyString("Abc", &ta) },
            { L_, Datum::createDouble(1.0) },
            { L_, Datum::createDouble(1.2345) },
            { L_, Datum::createDouble(-1.2346) },
            { L_, Datum::createDouble(k_DBL_MIN2) },
            { L_, Datum::createDouble(k_DBL_MAX2) },
            { L_, Datum::createDouble(k_DBL_INF) },
            { L_, Datum::createDouble(k_DBL_NEGINF) },

            // This test will take care of 0.0 as -0.0 == 0.0.

            { L_, Datum::createDouble(k_DBL_NEGZERO) },
            { L_, Datum::createUdt(&udt, UDT_TYPE) },
            { L_, Datum::createUdt(&udt, 1) },
            { L_, Datum::createUdt(&udt1, UDT_TYPE) },

            // Treat NaNs specially as value == value is false for NaN.

            { L_, Datum::createDouble(k_DBL_QNAN2) },
            { L_, Datum::createDouble(k_DBL_SNAN) },
            { L_, createArray(&ta) },
            { L_, createEmptyArray() },
            { L_, createMap(&ta) },
            { L_, createEmptyMap() }
            };
            const bsl::size_t NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            // Comparing ManagedDatum objects.

            for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                for (bsl::size_t j = 0; j < NUM_DATA; ++j) {
                    const int LINE_I = DATA[i].d_line;
                    const int LINE_J = DATA[j].d_line;
                    Obj x(DATA[i].d_datum, &ta); const Obj& X = x;
                    Obj y(DATA[j].d_datum, &ta); const Obj& Y = y;
                    if (i != j ||
                        (X->isDouble() && X->theDouble() != X->theDouble())) {
                        // Treat NaNs specially.
                        ASSERTV(LINE_I, LINE_J, X, Y, !(X == Y));
                        ASSERTV(LINE_I, LINE_J, X, Y, X != Y);
                    }
                    else {
                        ASSERTV(LINE_I, LINE_J, X, Y, (X == Y));
                        ASSERTV(LINE_I, LINE_J, X, Y, !(X != Y));
                    }
                    x.release();
                    y.release();
                }
            }

            for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                // Relase the memory allocated in 'DATA'.

                Datum::destroy(DATA[i].d_datum, &ta);
            }
        }

        if (verbose) cout << "\nTesting operators format." << endl;
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = bdld::operator==;
            operatorPtr operatorNe = bdld::operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OTHER CONSTRUCTORS
        //
        // Concerns:
        //: 1 The constructors bind object with passed allocator so it will be
        //:   used to allocate all necessary dynamic memory.
        //:
        //: 2 The Constructors use default values if expected parameters aren't
        //:   passed.
        //:
        //: 3 The constructors create a shallow copy of the passed 'Datum'
        //:   value without affecting it.
        //:
        //: 4 The constructors create a deep copy of the passed 'ManagedDatum'
        //:   value.
        //
        // Plan:
        //: 1 Create several 'ManagedDatum' objects using different
        //:   combinations of default and user-defined parameter values.
        //:
        //: 2 Verify that correct values are set up in created objects.
        //:   (C-1..3)
        //:
        //: 3 Create 'ManagedDatum object'.  Create it's copy, using copy
        //:   constructor.  Let the copy go out the scope.  Verify that origin
        //:   object isn't affected.  (C-4)
        //
        // Testing:
        //   ManagedDatum(bslma::Allocator *basicAllocator);
        //   ManagedDatum(const ManagedDatum&, bslma::Allocator *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING OTHER CONSTRUCTORS" << endl
                          << "==========================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator ta("test", veryVeryVerbose);

        if (verbose) cout << "\nTesting constructor with no arguments."
                          << endl;
        {
            bslma::DefaultAllocatorGuard guard(&da);

            Obj        mMD;
            const Obj& MD = mMD;

            // Check if object has a nil value.

            ASSERT(true == MD.datum().isNull());

            // Check if the right allocator has been tied with an object.

            ASSERT(&da  == MD.allocator());

            // Verify that no memory was allocated.

            ASSERT(0 == da.status());
        }

        if (verbose) cout << "\nTesting constructor with allocator only."
                          << endl;
        {
            bslma::DefaultAllocatorGuard guard(&da);

            Obj        mMD(&ta);
            const Obj& MD = mMD;

            // Check if object has a nil value.

            ASSERT(true == MD.datum().isNull());

            // Check if the right allocator has been tied with an object.

            ASSERT(&ta  == MD.allocator());

            // Verify that no memory was allocated.

            ASSERT(0 == ta.status());
            ASSERT(0 == da.status());
        }

        if (verbose) cout <<
                           "\nTesting copy constructor with default allocator."
                          << endl;
        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            {
                bslma::DefaultAllocatorGuard guard(&da);

                const Datum D = Datum::copyString("A very long string", &da);
                Obj         mMD(D);
                const Obj&  MD = mMD;

                ASSERT(D   == MD.datum());
                ASSERT(&da == MD.allocator());

                const Int64 NUM_BYTES_IN_USE = da.numBytesInUse();

                bslma::TestAllocator ada("another default",
                                         veryVeryVeryVerbose);
                {
                    // Another default allocator and 'ManagedDatum' object.

                    bslma::DefaultAllocatorGuard guard(&ada);

                    Obj         mMDCopy(MD);
                    const Obj&  MDCopy = mMDCopy;

                    ASSERT(true                 == D.isString());
                    ASSERT("A very long string" == D.theString());
                    ASSERT(MD.datum()           == MDCopy.datum());
                    ASSERT(&ada                 == MDCopy.allocator());
                    ASSERT(NUM_BYTES_IN_USE     == da.numBytesInUse());

                    // Let the object go out the scope to call destructor.
                }

                // Verify that copy destruction doesn't affect origin object.

                ASSERT(D   == MD.datum());
                ASSERT(&da == MD.allocator());
            }
            ASSERT(0 == da.numBytesInUse());
        }

        if (verbose) cout <<
                      "\nTesting copy constructor with user-defined allocator."
                          << endl;
        {
            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard guard(&da);
            {
                const Datum D = Datum::copyString("A very long string", &da);
                Obj         mMD(D);
                const Obj&  MD = mMD;

                ASSERT(D   == MD.datum());
                ASSERT(&da == MD.allocator());

                const Int64 NUM_BYTES_IN_USE = da.numBytesInUse();

                {
                   // Creating copy of 'ManagedDatum' object with user-defined
                   // allocator.

                    ASSERT(0   == ta.numBytesInUse());

                    Obj         mMDCopy(MD, &ta);
                    const Obj&  MDCopy = mMDCopy;

                    ASSERT(true                 == D.isString());
                    ASSERT("A very long string" == D.theString());
                    ASSERT(MD.datum()           == MDCopy.datum());
                    ASSERT(&ta                  == MDCopy.allocator());
                    ASSERT(0                    != ta.numBytesInUse());
                    ASSERT(NUM_BYTES_IN_USE     == da.numBytesInUse());

                    // Let the object go out the scope to call destructor.
                }

                // Verify that copy destruction doesn't affect origin object.

                ASSERT(D   == MD.datum());
                ASSERT(&da == MD.allocator());
            }
            ASSERT(0 == ta.numBytesInUse());
            ASSERT(0 == da.numBytesInUse());
        }
      } break;
      case 4: {
        //---------------------------------------------------------------------
        // BASIC ACCESSORS
        //
        // Concerns:
        //: 1 'datum' method returns a constant reference to the 'Datum' object
        //:    held inside this object.
        //:
        //: 2 'allocator' method returns a pointer providing modifiable access
        //:   to the allocator associated with this 'ManagedDatum'.
        //:
        //: 3 Dereference operator returns a pointer providing non-modifiable
        //:   access to the 'Datum' object held inside this object.
        //:
        //: 4 Indirection operator returns a reference providing non-modifiable
        //:   access to the 'Datum' object held inside this object.
        //
        // Plan:
        //: 1 Create 'ManagedDatum' object and verify that held Datum object
        //    and associated allocator are accessible through tested methods.
        //:   (C-1..4)
        //
        // Testing:
        //   const Datum& datum() const;
        //   bslma::Allocator *allocator() const;
        //   const Datum *operator->() const;
        //   const Datum& operator*() const;
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "BASIC ACCESSORS" << endl
                                  << "===============" << endl;

        bslma::TestAllocator ta("test", veryVeryVerbose);

        if (verbose) cout << "\nTesting 'datum'" << endl;
        {
            Datum intDatum = Datum::createInteger(1);
            Obj        mMD(intDatum, &ta);
            const Obj& MD = mMD;

            ASSERT(intDatum == MD.datum());
        }

        if (verbose) cout << "\nTesting 'allocator'" << endl;
        {
            Obj        mMD(Datum::createInteger(1), &ta);
            const Obj& MD = mMD;

            ASSERT(&ta == MD.allocator());
        }

        if (verbose) cout << "\nTesting 'operator->'" << endl;
        {
            Obj        mMD(Datum::createInteger(1), &ta);
            const Obj& MD = mMD;

            ASSERT(Datum::e_INTEGER == MD->type());
            ASSERT(true             == MD->isInteger());
            ASSERT(1                == MD->theInteger());
        }

        if (verbose) cout << "\nTesting 'operator*'" << endl;
        {
            Obj        mMD(Datum::createInteger(1), &ta);
            const Obj& MD = mMD;

            // copy constructor

            Datum firstCopy(*MD);

            ASSERT(Datum::e_INTEGER == firstCopy.type());
            ASSERT(true             == firstCopy.isInteger());
            ASSERT(1                == firstCopy.theInteger());

            // assignment

            Datum secondCopy;
            secondCopy = *MD;

            ASSERT(Datum::e_INTEGER == secondCopy.type());
            ASSERT(true             == secondCopy.isInteger());
            ASSERT(1                == secondCopy.theInteger());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING TEST APPARATUS
        //
        // Concerns:
        //: 1 All auxiliary functions create expected objects.
        //
        // Plan:
        //: 1 Call all auxiliary and verify their results.  (C-1)
        //
        // Testing:
        //   TEST APPARATUS
        // --------------------------------------------------------------------
        if (verbose) cout << endl << "TESTING TEST APPARATUS" << endl
                                  << "======================" << endl;

        bslma::TestAllocator ta("test", veryVeryVerbose);

        if (verbose) cout << "\nTesting 'createArray'." << endl;
        {
            Datum array = createArray(&ta);
            ASSERT(true == array.isArray());

            DatumArrayRef arrayRef = array.theArray();
            ASSERT(6 == arrayRef.length());

            ASSERT(true == arrayRef[0].isInteger());
            ASSERT(true == arrayRef[1].isDouble());
            ASSERT(true == arrayRef[2].isString());
            ASSERT(true == arrayRef[3].isString());
            ASSERT(true == arrayRef[4].isDate());
            ASSERT(true == arrayRef[5].isDatetime());

            Datum::destroy(array, &ta);
        }

        if (verbose) cout << "\nTesting 'createEmptyArray'." << endl;
        {
            Datum array = createEmptyArray();
            ASSERT(true == array.isArray());

            DatumArrayRef arrayRef = array.theArray();
            ASSERT(0 == arrayRef.length());

            Datum::destroy(array, &ta);
        }

        if (verbose) cout << "\nTesting 'createMap'." << endl;
        {
            Datum map = createMap(&ta);
            ASSERT(true  == map.isMap());

            DatumMapRef mapRef = map.theMap();
            ASSERT(6     == mapRef.size());
            ASSERT(false == mapRef.isSorted());

            ASSERT(true  == mapRef[0].value().isInteger());
            ASSERT(true  == mapRef[1].value().isDouble());
            ASSERT(true  == mapRef[2].value().isString());
            ASSERT(true  == mapRef[3].value().isString());
            ASSERT(true  == mapRef[4].value().isDate());
            ASSERT(true  == mapRef[5].value().isDatetime());

            Datum::destroy(map, &ta);
        }

        if (verbose) cout << "\nTesting 'createEmptyMap'." << endl;
        {
            Datum map = createEmptyMap();

            ASSERT(true  == map.isMap());

            DatumMapRef mapRef = map.theMap();

            ASSERT(0     == mapRef.size());
            ASSERT(false == mapRef.isSorted());

            Datum::destroy(map, &ta);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //
        // Concerns:
        //: 1 The constructor creates a shallow copy of the passed 'Datum'
        //:   value.
        //:
        //: 2 The constructor binds object with passed allocator so it will be
        //:   used to allocate all necessary dynamic memory.
        //:
        //: 3 The destructor triggers a deallocation of any dynamically
        //:    allocated memory.
        //:
        //: 4 Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a 'ManagedDatum' object and verify that 'Datum' object,
        //:   held inside, is equal to the passed one.  (C-1)
        //:
        //: 2 Verify, that object stores reference to the passed allocator.
        //:   (C-2)
        //:
        //: 3 Create a 'ManagedDatum' object.  Let it go out the scope.  Verify
        //:   that all allocated memory (including memory allocated by the
        //:   'Datum' object) is released.  (C-3)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   ManagedDatum(const Datum&, bslma::Allocator *);
        //   ~ManagedDatum();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PRIMARY MANIPULATORS" << endl
                                  << "====================" << endl;

        if (verbose) cout << "\nTesting constructor with default allocator."
                          << endl;
        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            {
                bslma::DefaultAllocatorGuard guard(&da);

                const Datum D = Datum::copyString("A very long string", &da);
                Obj         mMD(D);
                const Obj&  MD = mMD;

                ASSERT(D   == MD.datum());
                ASSERT(&da == MD.allocator());

                bslma::TestAllocator ada("another default",
                                         veryVeryVeryVerbose);
                {
                    // Another default allocator and 'ManagedDatum' object.

                    bslma::DefaultAllocatorGuard guard(&ada);

                    const Datum DA = Datum::copyString("A very long string",
                                                       &ada);
                    Obj         mMDA(DA);
                    const Obj&  MDA = mMDA;

                    ASSERT(true                 == DA.isString());
                    ASSERT("A very long string" == DA.theString());
                    ASSERT(DA                   == MDA.datum());
                    ASSERT(&ada                 == MDA.allocator());
                }

                // Other than another 'ManagedDatum' object.

                const Datum DOta = Datum::copyString("A very long string",
                                                     &da);
                Obj         mMDOta(DOta);
                const Obj&  MDOta = mMDOta;

                ASSERT(0    != da.numBytesInUse());
                ASSERT(0    == ada.numBytesInUse());
                ASSERT(DOta == MDOta.datum());
                ASSERT(&da  == MDOta.allocator());
            }
            ASSERT(0 == da.numBytesInUse());
        }


        if (verbose) cout << "\nTesting constructor." << endl;

        bslma::TestAllocator ta("test", veryVeryVerbose);
        {
            // Create ManagedDatum with a Datum value.

            const Datum D = Datum::copyString("A very long string", &ta);
            const Int64 numCurrBytesInUse = ta.numBytesInUse();

            Obj         mMD(D, &ta);
            const Obj&  MD = mMD;

            // Check if object has the same Datum value.

            ASSERT(D == MD.datum());

            // Check if the right allocator has been tied with an object.

            ASSERT(&ta == MD.allocator());

            //Verify that no memory was allocated.

            ASSERT(numCurrBytesInUse == ta.numBytesInUse());
        }

        if (verbose) cout << "\nTesting destructor." << endl;
        {
            ASSERT(0 == ta.numBytesInUse());

            // Create ManagedDatum with a Datum value.

            {
                const Datum D = Datum::copyString("A very long string", &ta);

                ASSERT(0  != ta.numBytesInUse());

                Obj         mMD(D, &ta);
                const Obj&  MD = mMD;

                // Check if the right allocator has been tied with an object.

                ASSERT(&ta == MD.allocator());

                // Let the object go out the scope to call destructor.
            }
            //Verify that all memory was deallocated.

            ASSERT(0  == ta.numBytesInUse());
        }
        ASSERT(0    == ta.numBytesInUse());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 Exercise a broad cross-section of full value-semantic
        //:   functionality before beginning testing in earnest. Probe the
        //:   functionality systematically and incrementally to discover
        //:   basic errors in isolation.
        //
        // Plan:
        //: 1 Create 'Datum' objects with integer, string and double
        //:   values. Verify that they have the correct type of value and the
        //:   correct value stored in them. Compare all the 'Datum'
        //:   objects. Assign a string value to the 'Datum' object holding
        //:   integer value. Now assign an integer value to the 'Datum'
        //:   object holding a string value and verify that no memory was
        //:   leaked. Create a 'Datum' object with a date+time value. Copy
        //:   construct another object from it. Verify both objects are equal.
        //:   Destroy all the objects and verify that no memory was leaked.
        //:   (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        bslma::TestAllocator ta("test", veryVeryVerbose);
        {
            // Create few ManagedDatum objects.

            Obj        mMD1(Datum::createInteger(5), &ta);
            const Obj& MD1 = mMD1;
            Obj        mMD2(Datum::copyString("This is a string", &ta), &ta);
            const Obj& MD2 = mMD2;
            Obj        mMD3(Datum::createDouble(5.0), &ta);
            const Obj& MD3 = mMD3;
            // Verify that the objects are of correct type and have the correct
            // value.

            ASSERT(MD1->isInteger());
            ASSERT(5 == MD1->theInteger());
            ASSERT(MD2->isString());
            ASSERT("This is a string" == MD2->theString());
            ASSERT(MD3->isDouble());
            ASSERT(5.0 == MD3->theDouble());

            // Compare these objects.

            ASSERT(MD1 != MD3);
            ASSERT(MD1 != MD2);

            // Assign different values to existing objects and verify no memory
            // is leaked.

            mMD1.adopt(Datum::copyString("This is a string", &ta));
            ASSERT(MD1 == MD2);
            const Int64 numCurrBytesInUse = ta.numBytesInUse();
            mMD2.adopt(Datum::createInteger(5));
            ASSERT(ta.numBytesInUse() < numCurrBytesInUse);

            //Copy construct an object. Verify it is equal to the existing
            //object.

            Obj        mMD4(Datum::createDatetime(
                        bdlt::Datetime(2010, 1, 5, 16, 45, 32, 12), &ta), &ta);
            const Obj& MD4 = mMD4;
            Obj        mMD5(MD4, &ta);
            const Obj& MD5 = mMD5;

            ASSERT(MD4 == MD5);
        }
        // Verify that no memory was leaked.
        ASSERT(0 == ta.status());

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
