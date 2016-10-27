// bsltf_testvaluesarray.t.cpp                                        -*-C++-*-
#include <bsltf_testvaluesarray.h>

#include <bsltf_stdstatefulallocator.h>
#include <bsltf_templatetestfacility.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_nameof.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;
using namespace BloombergLP::bsltf;
using bsls::NameOf;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The object under test is a container that contains a constant sequence of
// test values.  We need to ensure that it can be constructed and all elements
// can be accessed.  This container is implemented in the form of a class
// template, and thus its proper instantiation for several types is a concern.
// Also we need to test an iterator class 'TestValuesArrayIterator' providing
// access to elements in a 'TestValuesArray' object.
//
// Primary Manipulators:
//: o TestValuesArray(const char *spec);
//: o TestValuesArray(const char *spec, ALLOCATOR basicAllocator);
//
// Basic Accessors:
//: o const VALUE *data() const;
//: o size_t size() const;
//
//-----------------------------------------------------------------------------
//                      // -----------------------------
//                      // class TestValuesArrayIterator
//                      // -----------------------------
//
// CREATORS
// [ 8] TestValuesArrayIterator(const VALUE *, const VALUE *, bool *, bool *);
// [11] TestValuesArrayIterator(const TestValuesArrayIterator& original);
//
// MANIPULATORS
// [12] TestValuesArrayIterator& operator=(const TestValuesArrayIterator&);
// [ 8] TestValuesArrayIterator& operator++();
// [13] TestValuesArray_PostIncrementPtr<VALUE> operator++(int);
//
// ACCESSORS
// [ 9] const VALUE& operator *() const;
// [14] const VALUE *operator->() const;
//
// FREE OPERATORS
// [10] bool operator==(lhs, rhs);
// [10] bool operator!=(lhs, rhs);
//
//                      // ---------------------
//                      // class TestValuesArray
//                      // ---------------------
//
// CREATORS
// [ 5] explicit TestValuesArray();
// [ 5] explicit TestValuesArray(ALLOCATOR basicAllocator);
// [ 3] explicit TestValuesArray(const char *spec);
// [ 3] explicit TestValuesArray(const char *spec, ALLOCATOR basicAllocator);
// [ 3] ~TestValuesArray();
//
// MANIPULATORS
// [15] iterator begin();
// [15] iterator index(size_t position);
// [15] iterator end();
// [15] void resetIterators();
//
// ACCESSORS
// [ 4] const VALUE *data() const;
// [ 6] const VALUE& operator[](size_t index) const;
// [ 4] size_t size() const;
//
//                      // --------------------------------------
//                      // class TestValuesArray_DefaultConverter
//                      // --------------------------------------
//
// CLASS METHODS
// [ 2] createInplace(VALUE *objPtr, char value, ALLOCATOR allocator);
//
//                      // --------------------------------------
//                      // class TestValuesArray_PostIncrementPtr
//                      // --------------------------------------
//
// CREATORS
// [ 7] explicit TestValuesArray_PostIncrementPtr(const VALUE* ptr);
//
// ACCESSORS
// [ 7] const VALUE& operator*() const;
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [16] USAGE EXAMPLE

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

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

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
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static const struct {
    int         d_line;     // source line number
    const char *d_spec;     // specification string, for input
} DATA[] = {
    //line spec
    //---- ----
    { L_,  ""                  },
    { L_,  "A"                 },
    { L_,  "B"                 },
    { L_,  "AA"                },
    { L_,  "AB"                },
    { L_,  "BA"                },
    { L_,  "AC"                },
    { L_,  "CD"                },
    { L_,  "ABC"               },
    { L_,  "ACB"               },
    { L_,  "BAC"               },
    { L_,  "BCA"               },
    { L_,  "CAB"               },
    { L_,  "CBA"               },
    { L_,  "BAD"               },
    { L_,  "ABCDEFGHIJKLMNOP"  },
    { L_,  "PONMLKJIGHFEDCBA"  },
    { L_,  "ABCDEFGHIJKLMNOPQ" },
    { L_,  "DHBIMACOPELGFKNJQ" },
};

const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

//=============================================================================
//                       GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

                            // ===================
                            // class TestConverter
                            // ===================
template <class VALUE, class ALLOCATOR>
struct TestConverter
    // This 'struct' provides a namespace that contains two class method
    // templates, 'createInplace' and 'getIdentifier', that respectively
    // provide a consistent interface to (1) create a specified object of the
    // (template parameter) type 'VALUE' from a char identifier and (2) get
    // the identifier value of a specified object.
{
    // CLASS METHODS
    static void createInplace(VALUE *objPtr, char value, ALLOCATOR allocator);
        // Create an object of the (template parameter) type 'VALUE' at the
        // specified 'objPtr' address whose state is unique for the specified
        // 'value'.  Use the specified 'allocator' to supply memory.  The
        // behavior is undefined unless '0 <= value' and 'VALUE' is contained
        // in the macro 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL'.

    static int getIdentifier(const VALUE& obj);
        // Return the integer identifier that uniquely identifies the specified
        // 'obj'.
};

                            // -------------------
                            // class TestConverter
                            // -------------------

template <class VALUE, class ALLOCATOR>
inline
void TestConverter<VALUE, ALLOCATOR>::createInplace(VALUE     *objPtr,
                                                    char       value,
                                                    ALLOCATOR  allocator)
{
    bsltf::TemplateTestFacility::emplace(objPtr, 127 - value, allocator);
}

template <class VALUE, class ALLOCATOR>
inline
int TestConverter<VALUE, ALLOCATOR>::getIdentifier(const VALUE& obj)
{
    return 127 - TemplateTestFacility::getIdentifier<VALUE>(obj);
}

//=============================================================================
//                            TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

                            // ================
                            // class TestDriver
                            // ================

template <class VALUE,
          class ALLOCATOR = bsl::allocator<VALUE>,
          class CONVERTER = TestConverter<VALUE, ALLOCATOR> >
class TestDriver
    // Test driver class for 'TestValuesArray'.
{
  private:
    // TYPES
    typedef bsltf::TestValuesArray<VALUE, ALLOCATOR, CONVERTER> Obj;
        // The type under testing.

    typedef TestValuesArrayIterator<VALUE> Iterator;
        // The iterator for the type under testing.

    enum { k_IS_BSL_ALLOCATOR = bsl::is_same<ALLOCATOR,
                                             bsl::allocator<VALUE> >::value,
           k_VALUE_USES_BSLMA = bslma::UsesBslmaAllocator<VALUE>::value };

  public:
    // TEST CASES
    static void testCase15();
        // TESTING MANIPULATORS.

    static void testCase14();
        // ITERATOR STRUCTURE DEREFERENCE OPERATOR.

    static void testCase13();
        // ITERATOR POST-INCREMENT OPERATOR.

    static void testCase12();
        // ITERATOR COPY-ASSIGNMENT OPERATOR.

    static void testCase11();
        // ITERATOR COPY CONSTRUCTOR.

    static void testCase10();
        // ITERATOR EQUALITY-COMPARISON OPERATORS.

    static void testCase9();
        // ITERATOR BASIC ACCESSORS.

    static void testCase8();
        // ITERATOR PRIMARY MANIPULATORS.

    static void testCase7();
        // TESTING POST-INCREMENT POINTER.

    static void testCase6();
        // TESTING SUBSCRIPT OPERATOR.

    static void testCase5();
        // CONSTRUCTORS.

    static void testCase4();
        // BASIC ACCESSORS.

    static void testCase3();
        // PRIMARY MANIPULATORS.

    static void testCase2();
        // TESTING DEFAULT CONVERTER.

    static void testCase1();
        // BREATHING TEST.
};

                              // ----------
                              // TEST CASES
                              // ----------
template <class VALUE, class ALLOCATOR, class CONVERTER>
void TestDriver<VALUE, ALLOCATOR, CONVERTER>::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING MANIPULATORS
    //
    // Concerns:
    //: 1 The 'begin()' returns an iterator, referring to the first value in
    //:   the array.
    //:
    //: 2 The 'end()' returns an  iterator, referring to the address, following
    //:   the last value in the array.
    //:
    //: 3 The 'index()' returns an iterator, referring to the value with index,
    //:   passed as a parameter.
    //:
    //: 4 The 'resetIterators()' makes all values accessible through iterators
    //:   again.
    //:
    //: 5 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:
    //:
    //:   1 Create an object, passing a value specification.
    //:
    //:   2 Use 'begin()' to obtain an iterator, referring to the first value
    //:     in the object and verify this iterator.  (C-1)
    //:
    //:   3 Iterate through the whole TestValuesArray and verify iterator
    //:     values.  (C-2)
    //:
    //: 3 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:
    //:
    //:   1 Create an object, passing a value specification.
    //:
    //:   2 Use 'index()' to obtain iterators, referring to each element with
    //:     index from 0 to number of values in consecutive order and verify
    //:     value of this iterator.  (C-3)
    //:
    //: 4 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:
    //:
    //:   1 Create an object, passing a value specification.
    //:
    //:   2 Use 'operator++' and 'operator *' to dereference all values.
    //:     Verify, that 'resetIterators' restores all flags.  (C-4)
    //:
    //: 5 Verify defensive checks are triggered for invalid values.  (C-5)
    //
    // Testing:
    //  iterator begin();
    //  iterator end();
    //  iterator index(size_t position);
    //  void resetIterators();
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    if (verbose) printf("\tTesting 'begin' and 'end'.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE = DATA[ti].d_line;
            const char   *SPEC = DATA[ti].d_spec;
            const size_t  SIZE = strlen(SPEC);

            if (veryVeryVerbose) { T_ T_ P_(LINE) P(SPEC); }

            Obj        mX(SPEC);
            const Obj& X = mX;

            Iterator it = mX.begin();
            for (size_t i = 0; i < SIZE; ++i) {
                ASSERTV(SPEC, i,
                        CONVERTER::getIdentifier(X[i]) ==
                                                CONVERTER::getIdentifier(*it));
                ASSERTV(SPEC, i, it != mX.end());
                ++it;
            }
            ASSERTV(SPEC, it == mX.end());
        }
    }

    if (verbose) printf("\tTesting 'index'.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE = DATA[ti].d_line;
            const char   *SPEC = DATA[ti].d_spec;
            const size_t  SIZE = strlen(SPEC);

            if (veryVeryVerbose) { T_ T_ P_(LINE) P(SPEC); }

            Obj        mX(SPEC);
            const Obj& X = mX;

            for (size_t i = 0; i < SIZE; ++i) {
                Iterator it = mX.index(i);
                ASSERTV(SPEC, i,
                        CONVERTER::getIdentifier(X[i]) ==
                                                CONVERTER::getIdentifier(*it));
            }

            Iterator it = mX.index(SIZE);
            ASSERTV(SPEC, mX.end() == it);
        }
    }

    if (verbose) printf("\tTesting 'resetIterators'.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE = DATA[ti].d_line;
            const char   *SPEC = DATA[ti].d_spec;
            const size_t  SIZE = strlen(SPEC);

            if (veryVeryVerbose) { T_ T_ P_(LINE) P(SPEC); }

            Obj        mX(SPEC);
            const Obj& X = mX;

            Iterator shuttle = mX.begin();
            for (size_t i = 0; i < SIZE; ++i) {
                // Dereference the iterator 'shuttle', so any attempts to
                // dereference it again will lead to undefined behavior (and
                // trigger defensive checks in the appropriate build modes).

                *shuttle;

                // Reset all dereferenceable flags.
                mX.resetIterators();

                // Verify, that iterator can be dereferenced again.

                *shuttle;

                ++shuttle;
                mX.resetIterators();
            }

            // Now all values in array are dereferenceable. Traverse each of
            // them to change their properties.

            shuttle = mX.begin();
            for (size_t i = 0; i < SIZE; ++i) {
                ++shuttle;
            }

            // Verify that 'resetIterators' reset all flags simultaneously.

            mX.resetIterators();

            shuttle = mX.begin();
            for (size_t i = 0; i < SIZE; ++i) {
                ASSERTV(SPEC, i,
                        CONVERTER::getIdentifier(X[i]) ==
                                           CONVERTER::getIdentifier(*shuttle));
                ++shuttle;
            }
        }  // end foreach row
    }

    if (verbose) printf("\tNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        const char   *SPEC = "A";
        const size_t  SIZE = strlen(SPEC);

        Obj mX(SPEC);

        ASSERT_SAFE_FAIL(mX.index(SIZE + 1));
        ASSERT_SAFE_PASS(mX.index(SIZE    ));
    }
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
void TestDriver<VALUE, ALLOCATOR, CONVERTER>::testCase14()
{
    // ------------------------------------------------------------------------
    // ITERATOR DEREFERENCE OPERATOR
    //
    // Concerns:
    //: 1 The 'operator->' returns the address of the element referred by this
    //:   object.
    //:
    //: 2 The 'operator->' modifies 'dereferenceable' flag of the element
    //:   referred by this object.
    //:
    //: 3 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid TestValuesArray object values.
    //:
    //: 2 For each row (representing a distinct TestValuesArray object value,
    //:   'V') in the table described in P-1:
    //:
    //:   1 Create a constant TestValuesArray object, passing a value
    //:     specification.
    //:
    //:   2 Create two boolean arrays, having the same size as the
    //:     TestValuesArray object, to emulate TestValuesArray's
    //:     'dereferenceable' and 'validIterator' arrays.
    //:
    //:   3 Create an iterator, referring to the first element in the
    //:     TestValuesArray and two arrays, specified in P-2.2.
    //:
    //:   4 Iterate through the whole TestValuesArray and verify that
    //:     'operator->' returns correct values.  (C-1)
    //:
    //:   5 Having direct access to boolean arrays, specified in P-2.2, verify
    //:     that 'operator->' modifies value's flags correctly.  (C-2)
    //:
    //: 3 Verify defensive checks are triggered for invalid values.  (C-3)
    //
    // Testing:
    //  const VALUE *TestValuesArrayIterator::operator->() const;
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    if (verbose) printf("\tTesting behavior.\n");
    {
        // We need to exclude empty specification, since 'VALUE' array is not
        // created in this case.

        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int     LINE = DATA[ti].d_line;
            const char   *SPEC = DATA[ti].d_spec;
            const size_t  SIZE = strlen(SPEC);

            if (veryVeryVerbose) { T_ T_ P_(LINE) P(SPEC); }

            const Obj  VALUES_ARRAY(SPEC);
            bool      *isDerefArray = new bool[SIZE + 1];
            bool      *isValidArray = new bool[SIZE + 1];

            for (size_t i = 0; i < SIZE; ++i) {
                isDerefArray[i] = true;
                isValidArray[i] = true;
            }

            isDerefArray[SIZE] = false;
            isValidArray[SIZE] = false;

            // Test object creation.

            Iterator        mX(VALUES_ARRAY.data(),
                               VALUES_ARRAY.data() + SIZE,
                               isDerefArray,
                               isValidArray);
            const Iterator& X = mX;

            for (size_t i = 0; i < SIZE; ++i) {
                ASSERTV(LINE, SPEC[i], true == isDerefArray[i]);
                ASSERTV(LINE, SPEC[i], true == isValidArray[i]);

                ASSERTV(LINE, i, VALUES_ARRAY.data() + i == X.operator->());

                ASSERTV(LINE, SPEC[i], false == isDerefArray[i]);
                ASSERTV(LINE, SPEC[i], true  == isValidArray[i]);

                ++mX;
            }

            // Reclaim dynamically allocated arrays.

            delete [] isDerefArray;
            delete [] isValidArray;
        }
    }

    if (verbose) printf("\tNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        const char *SPEC = "A";
        bool        tBV = true;   // true  boolean value
        bool        fBV = false;  // false boolean value
        const Obj   V(SPEC);

        // We need to deceive constructor to create 'INVALID' and
        // 'DEREFERENCED' objects.

        fBV = true;

        const Iterator INVALID     (V.data(), V.data(), &tBV, &fBV);
        const Iterator DEREFERENCED(V.data(), V.data(), &fBV, &tBV);
        const Iterator VALID       (V.data(), V.data(), &tBV, &tBV);

        // Make 'INVALID' and 'DEREFERENCED' iterators invalid for indirection
        // operator.

        fBV = false;

        ASSERT_OPT_FAIL(INVALID.operator->());
        ASSERT_OPT_FAIL(DEREFERENCED.operator->());
        ASSERT_OPT_PASS(VALID.operator->());
    }
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
void TestDriver<VALUE, ALLOCATOR, CONVERTER>::testCase13()
{
    // ------------------------------------------------------------------------
    // ITERATOR POST-INCREMENT OPERATOR
    //
    // Concerns:
    //: 1 The post-increment operator changes the value of the object to
    //:   refer to the next element in the arrays, supplied at the
    //:   construction.
    //:
    //: 2 The returned pointer refers to the value, that iterator has referred
    //:   to prior to the operator call.
    //:
    //: 3 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid TestValuesArray object values.
    //:
    //: 2 For each row (representing a distinct TestValuesArray object value,
    //:   'V') in the table described in P-1:
    //:
    //:   1 Create a constant TestValuesArray object, passing a value
    //:     specification.
    //:
    //:   2 Create two boolean arrays, having the same size as the
    //:     TestValuesArray object, to emulate TestValuesArray's
    //:     'dereferenceable' and 'validIterator' arrays.
    //:
    //:   3 Create an iterator, referring to the first value in the
    //:     TestValuesArray and two arrays, specified in P-2.2.
    //:
    //:   4 Use 'operator++(int)' to iterate through the whole TestValuesArray
    //:     and verify that it returns correct values. (C-1..2)
    //:
    //: 3 Verify defensive checks are triggered for invalid values.  (C-3)
    //
    // Testing:
    //  TestValuesArray_PostIncrementPtr<VALUE> operator++(int);
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    if (verbose) printf("\tTesting behavior.\n");
    {
        // We need to exclude empty specification, since 'VALUE' array is not
        // created in this case.

        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int     LINE = DATA[ti].d_line;
            const char   *SPEC = DATA[ti].d_spec;
            const size_t  SIZE = strlen(SPEC);

            if (veryVeryVerbose) { T_ T_ P_(LINE) P(SPEC); }

            const Obj  VALUES_ARRAY(SPEC);
            bool      *isDerefArray = new bool[SIZE + 1];
            bool      *isValidArray = new bool[SIZE + 1];

            for (size_t i = 0; i < SIZE; ++i) {
                isDerefArray[i] = true;
                isValidArray[i] = true;
            }

            isDerefArray[SIZE] = false;
            isValidArray[SIZE] = false;

            Iterator mX(VALUES_ARRAY.data(),
                        VALUES_ARRAY.data() + SIZE,
                        isDerefArray,
                        isValidArray);

            for (size_t i = 0; i < SIZE; ++i) {
                ASSERTV(LINE, SPEC[i], true == isDerefArray[i]);
                ASSERTV(LINE, SPEC[i], true == isValidArray[i]);

                ASSERTV(LINE, SPEC[i],
                        SPEC[i] == CONVERTER::getIdentifier(*(mX++)));

                ASSERTV(LINE, SPEC[i], false == isDerefArray[i]);
                ASSERTV(LINE, SPEC[i], false == isValidArray[i]);
            }

            // Reclaim dynamically allocated arrays.

            delete [] isDerefArray;
            delete [] isValidArray;
        }
    }

    if (verbose) printf("\tNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        const char *SPEC = "A";
        bool        tBV = true;   // true  boolean value
        bool        fBV = false;  // false boolean value
        const Obj   V(SPEC);

        // We need to deceive constructor to create 'invalid' object.

        fBV = true;

        Iterator invalid(V.data(), V.data() + 1, &tBV, &fBV);
        Iterator end    (V.data(), V.data(),     &tBV, &tBV);
        Iterator valid  (V.data(), V.data() + 1, &tBV, &tBV);

        // Make 'invalid' iterator invalid.

        fBV = false;

        ASSERT_OPT_FAIL(invalid++);
        ASSERT_OPT_FAIL(end++);
        ASSERT_OPT_PASS(valid++);
    }
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
void TestDriver<VALUE, ALLOCATOR, CONVERTER>::testCase12()
{
    // ------------------------------------------------------------------------
    // ITERATOR COPY-ASSIGNMENT OPERATOR
    //
    // Concerns:
    //: 1 The assignment operator can change the value of any modifiable target
    //:   object to that of any source object.
    //:
    //: 2 The target object gets the same value as the source object.
    //:
    //: 3 The signature and return type are standard.
    //:
    //: 4 The reference returned is to the target object (i.e., '*this').
    //:
    //: 5 The value of the source object is not modified.
    //:
    //: 6 Assigning an object to itself behaves as expected (alias-safety).
    //:
    //: 7 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   copy-assignment operator defined in this component.  (C-3)
    //:
    //: 2 Create two distinct TestValuesArray objects, passing value
    //:   specifications.
    //:
    //: 3 Create two boolean arrays, having the same size as the
    //:   TestValuesArray object, to emulate TestValuesArray's
    //:   'dereferenceable' and 'validIterator' arrays.
    //:
    //: 4 Create source iterator, 'mX', and target iterator, 'mY', referring to
    //:   the first values of the different TestValuesArrays and four arrays,
    //:   specified in P-3.2 (each iterator referring to two distinct arrays).
    //:
    //: 5 Assign 'mY' from 'mX'.  (C-1)
    //:
    //: 6 Verify that the address of the return value is the same as
    //:   that of 'mY'.  (C-4)
    //:
    //: 7 Use the equality-comparison operator and strict access to the arrays,
    //:   specified in P-3, to verify that the target object, 'mY', now has the
    //:   same value as that of 'X'.  (C-2)
    //:
    //: 8 Verify that 'X' still has the same value'.  (C-5)
    //
    //: 9 Create a constant TestValuesArray object.
    //:
    //:10 Create two boolean arrays, having the same size as the
    //:   TestValuesArray object, to emulate TestValuesArray's
    //:   'dereferenceable' and 'validIterator' arrays.
    //:
    //:11 Create an iterator, 'mX', referring to the first value in
    //:   the TestValuesArray, specified in P-9 and two arrays, specified in
    //:   P-10.
    //:
    //:12 Assign 'mX' from 'X'.  (C-1)
    //:
    //:13 Verify that 'X' still has the same value.  (C-6)
    //:
    //:14 Verify defensive checks are triggered for invalid values.  (C-7)
    //
    // Testing:
    //  TestValuesArrayIterator& operator=(const TestValuesArrayIterator&);
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    if (verbose)
        printf("\tTesting signature and return type.\n");
    {
        typedef Iterator& (Iterator::*operatorPtr)(const Iterator&);

        // Verify that the signature and return type are standard.

        operatorPtr operatorAssignment = &Iterator::operator=;

        (void) operatorAssignment;  // quash potential compiler warning
    }

    if (verbose) printf("\tTesting behavior.\n");
    {
        const char   *SPEC = "A";
        const size_t  SIZE = strlen(SPEC);
        const Obj     VALUES_ARRAY(SPEC);
        bool          isDerefArray = true;
        bool          isValidArray = true;

        const char   *TARGET_SPEC = "B";
        const size_t  TARGET_SIZE = strlen(TARGET_SPEC);
        const Obj     TARGET_VALUES_ARRAY (TARGET_SPEC);
        bool          targetIsDerefArray = true;
        bool          targetIsValidArray = true;

        // Source object creation.

        Iterator        mX(VALUES_ARRAY.data(),
                           VALUES_ARRAY.data() + SIZE,
                           &isDerefArray,
                           &isValidArray);
        const Iterator& X = mX;

        // Target object creation.

        Iterator        mY(TARGET_VALUES_ARRAY.data(),
                           TARGET_VALUES_ARRAY.data() + TARGET_SIZE,
                           &targetIsDerefArray,
                           &targetIsValidArray);
        const Iterator& Y = mY;

        // Copy-assignment.

        Iterator *mR = &(mY = X);

        // Verify, that address of target iterator is returned.

        ASSERTV(mR, &mY, mR == &mY);

        // Verify, that the target iterator points to the same value as the
        // origin one.

        ASSERT(X == Y);

        // Verify, that the origin iterator remains unchanged.

        ASSERT(true == isDerefArray);
        ASSERT(true == isValidArray);
        ASSERT(SPEC[0] == CONVERTER::getIdentifier(*X));

        // The inderection operator changes 'dereferenceable' status of
        // 'VALUE' object, iterator pointing to, to 'false', so we need to
        // restore it, to check state of the iterator by it's incrementing.

        isDerefArray = true;

        ASSERTV(true == isDerefArray);
        ASSERTV(true == isValidArray);

        ++mY;

        ASSERTV(false == isDerefArray);
        ASSERTV(false == isValidArray);

        // Verify, that the initial target iterator's data remain unchanged.

        ASSERTV(true == targetIsDerefArray);
        ASSERTV(true == targetIsValidArray);
    }

    if (verbose) printf("\tSelf-assignment.\n");
    {
        const char   *SPEC = "A";
        const size_t  SIZE = strlen(SPEC);
        const Obj     VALUES_ARRAY(SPEC);
        bool          isDerefArray = true;
        bool          isValidArray = true;

        // Object creation.

        Iterator        mX(VALUES_ARRAY.data(),
                           VALUES_ARRAY.data() + SIZE,
                           &isDerefArray,
                           &isValidArray);
        const Iterator& X = mX;

        // Copy-assignment.

        mX = X;

        // Verify, that the data remain unchanged.

        ASSERTV(true == isDerefArray);
        ASSERTV(true == isValidArray);
        ASSERTV(SPEC[0] == CONVERTER::getIdentifier(*X));

        // The inderection operator changes 'dereferenceable' status of
        // 'VALUE' object, iterator pointing to, to 'false', so we need to
        // restore it, to check state of the iterator by it's incrementing.

        isDerefArray = true;

        ASSERTV(true == isDerefArray);
        ASSERTV(true == isValidArray);

        ++mX;

        ASSERTV(false == isDerefArray);
        ASSERTV(false == isValidArray);
    }

    if (verbose) printf("\tNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        const char *SPEC = "A";
        bool        tBV = true;   // true  boolean value
        bool        fBV = false;  // false boolean value
        const Obj   V(SPEC);

        // We need to deceive constructor to create 'INVALID' object.

        fBV = true;

        const Iterator VALID(V.data(), V.data(), &tBV, &tBV);
        const Iterator INVALID(V.data(), V.data(), &tBV, &fBV);

        // Make 'INVALID' iterator invalid.

        fBV = false;

        Iterator mX(V.data(), V.data(), &tBV, &tBV);

        ASSERT_OPT_FAIL(mX = INVALID);
        ASSERT_OPT_PASS(mX = VALID);
    }
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
void TestDriver<VALUE, ALLOCATOR, CONVERTER>::testCase11()
{
    // ------------------------------------------------------------------------
    // ITERATOR COPY CONSTRUCTOR
    //
    // Concerns:
    //: 1 The new object's value is the same as that of the original object.
    //:
    //: 2 The value of the original object is left unaffected.
    //:
    //: 3 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Create a constant TestValuesArray object, passing a value
    //:   specification.
    //:
    //: 2 Create two boolean arrays, having the same size as the
    //:   TestValuesArray object, to emulate TestValuesArray's
    //:   'dereferenceable' and 'validIterator' arrays for two different
    //:   iterators.
    //:
    //: 3 Create a const source iterator, 'X', referring to the first value in
    //:   the TestValuesArray and two arrays, specified in P-2.2.
    //:
    //: 4 Use the copy constructor to create an iterator 'Y', supplying it the
    //:   'const' object 'X'.
    //:
    //: 5 Verify that the newly constructed object 'Y', has the same
    //:   value as that of 'X'.  (C-1)
    //:
    //: 6 Verify that 'X' is left unaffected.  (C-2)
    //:
    //: 7 Verify defensive checks are triggered for invalid values.  (C-3)
    //
    // Testing:
    //  TestValuesArrayIterator(const TestValuesArrayIterator& original);
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    if (verbose) printf("\tTesting behavior.\n");
    {
        const char   *SPEC = "A";
        const size_t  SIZE = strlen(SPEC);
        const Obj     VALUES_ARRAY(SPEC);
        bool          isDerefArray = true;
        bool          isValidArray = true;

        // Source object creation.

        Iterator        mX(VALUES_ARRAY.data(),
                           VALUES_ARRAY.data() + SIZE,
                           &isDerefArray,
                           &isValidArray);
        const Iterator& X = mX;

        // Target object creation.

        Iterator        mY(X);
        const Iterator& Y = mY;

        // Verify, that newly created iterator points to the same value as the
        // origin one.

        ASSERTV(X == Y);

        // Verify, that the origin iterator remains unchanged.

        ASSERTV(true == isDerefArray);
        ASSERTV(true == isValidArray);
        ASSERTV(SPEC[0] == CONVERTER::getIdentifier(*X));

        // The inderection operator changes 'dereferenceable' status of
        // 'VALUE' object, iterator pointing to, to 'false', so we need to
        // restore it, to check state of the newly created iterator.

        isDerefArray = true;

        ASSERTV(true == isDerefArray);
        ASSERTV(true == isValidArray);

        ++mY;

        ASSERTV(false == isDerefArray);
        ASSERTV(false == isValidArray);
    }

    if (verbose) printf("\tNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        const char *SPEC = "A";
        bool        tBV = true;   // true  boolean value
        bool        fBV = false;  // false boolean value
        const Obj   V(SPEC);

        // We need to deceive constructor to create 'INVALID' object.

        fBV = true;

        const Iterator VALID(V.data(), V.data(), &tBV, &tBV);
        const Iterator INVALID(V.data(), V.data(), &tBV, &fBV);

        // Make 'INVALID' iterator invalid.

        fBV = false;

        ASSERT_OPT_FAIL((Iterator(INVALID)));
        ASSERT_OPT_PASS((Iterator(VALID)));
    }
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
void TestDriver<VALUE, ALLOCATOR, CONVERTER>::testCase10()
{
    // --------------------------------------------------------------------
    // ITERATOR EQUALITY-COMPARISON OPERATORS
    //   Ensure that '==' and '!=' are the operational definition of value.
    //
    // Concerns:
    //: 1 Two objects, 'X' and 'Y', compare equal if and only if they point
    //:   to the same object.
    //:
    //: 2 'true  == (X == X)'  (i.e., identity)
    //:
    //: 3 'false == (X != X)'  (i.e., identity)
    //:
    //: 4 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
    //:
    //: 5 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
    //:
    //: 6 'X != Y' if and only if '!(X == Y)'
    //:
    //: 7 Comparison is symmetric with respect to user-defined conversion
    //:   (i.e., both comparison operators are free functions).
    //:
    //: 8 Non-modifiable objects can be compared (i.e., objects or
    //:   references providing only non-modifiable access).
    //:
    //; 9 The equality operator's signature and return type are standard.
    //:
    //:10 The inequality operator's signature and return type are standard.
    //:
    //:11 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
    //:   initialize function pointers having the appropriate signatures
    //:   and return types for the two homogeneous, free equality-
    //:   comparison operators defined in this component.
    //:   (C-7..10)
    //:
    //: 2 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid TestValuesArray object values.
    //:
    //: 3 For each row (representing a distinct TestValuesArray object value,
    //:   'V') in the table described in P-2:
    //:
    //:   1 Create a constant TestValuesArray object, passing a value
    //:     specification.
    //:
    //:   2 Create four boolean arrays, having the same size as the
    //:     TestValuesArray object, to emulate TestValuesArray's
    //:     'dereferenceable' and 'validIterator' arrays for two different
    //:     iterators.
    //:
    //:   3 Create two iterators, referring to the first value in the
    //:     TestValuesArray and four arrays, specified in P-3.2 (each iterator
    //:     referring to two individual arrays).
    //:
    //:   4 Increment both iterators in turn and verify the commutativity
    //:     property and the expected return value for both '==' and '!='.
    //:     (C-1..8)
    //:
    //: 4 Verify defensive checks are triggered for invalid values.  (C-11)
    //
    // Testing:
    //   bool operator==(lhs, rhs);
    //   bool operator!=(lhs, rhs);
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    if (verbose)
        printf("\nTesting signatures and return types.\n");
    {
        typedef bool (*operatorPtr)(const Iterator&, const Iterator&);

        // Verify that the signatures and return types are standard.

        operatorPtr operatorEq = operator==;
        operatorPtr operatorNe = operator!=;

        (void) operatorEq;  // quash potential compiler warnings
        (void) operatorNe;
    }

    if (verbose) printf("\tTesting behavior.\n");
    {
        // We need to exclude empty specification, since 'VALUE' array is not
        // created in this case.

        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int     LINE = DATA[ti].d_line;
            const char   *SPEC = DATA[ti].d_spec;
            const size_t  SIZE = strlen(SPEC);

            if (veryVeryVerbose) { T_ T_ P_(LINE) P(SPEC); }

            const Obj  VALUES_ARRAY(SPEC);
            bool      *isDerefArray1 = new bool[SIZE];
            bool      *isDerefArray2 = new bool[SIZE];
            bool      *isValidArray1 = new bool[SIZE];
            bool      *isValidArray2 = new bool[SIZE];

            for (size_t i = 0; i < SIZE; ++i) {
                isDerefArray1[i] = true;
                isDerefArray2[i] = true;
                isValidArray1[i] = true;
                isValidArray2[i] = true;
            }

            // Test objects creation.

            Iterator        mX1(VALUES_ARRAY.data(),
                                VALUES_ARRAY.data() + SIZE,
                                isDerefArray1,
                                isValidArray1);
            const Iterator& X1 = mX1;
            Iterator        mX2(VALUES_ARRAY.data(),
                                VALUES_ARRAY.data() + SIZE,
                                isDerefArray2,
                                isValidArray2);
            const Iterator& X2 = mX2;

            for (size_t i = 0; i < SIZE - 1; ++i) {
                ASSERTV(LINE, i, X1 == X1);

                ASSERTV(LINE, i, X1 == X2);
                ASSERTV(LINE, i, X2 == X1);
                ASSERTV(LINE, i, !(X1 != X1));
                ASSERTV(LINE, i, !(X1 != X2));
                ASSERTV(LINE, i, !(X2 != X1));

                // Verify, that comparison does not affect values.

                ASSERTV(LINE, i, true == isDerefArray1[i]);
                ASSERTV(LINE, i, true == isValidArray1[i]);
                ASSERTV(LINE, i, true == isDerefArray2[i]);
                ASSERTV(LINE, i, true == isValidArray2[i]);
                ASSERTV(LINE, i,
                        SPEC[i] == CONVERTER::getIdentifier(VALUES_ARRAY[i]));

                // Increment one iterator to change it's value.

                ++mX1;

                // Verify values.

                ASSERTV(LINE, i, X1 != X2);
                ASSERTV(LINE, i, X2 != X1);
                ASSERTV(LINE, i, !(X1 == X2));
                ASSERTV(LINE, i, !(X2 == X1));

                // Increment another iterator to restore equality.

                ++mX2;
            }

            delete [] isDerefArray1;
            delete [] isDerefArray2;
            delete [] isValidArray1;
            delete [] isValidArray2;
        } // foreach raw
    }

    if (verbose) printf("\tNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        const char *SPEC = "A";
        bool        tBV = true;   // true  boolean value
        bool        fBV = false;  // false boolean value
        const Obj   V(SPEC);

        // We need to deceive constructor to create 'INVALID' object.

        fBV = true;

        const Iterator VALID(V.data(), V.data(), &tBV, &tBV);
        const Iterator INVALID(V.data(), V.data(), &tBV, &fBV);

        // Make 'INVALID' iterator invalid.

        fBV = false;

        ASSERT_OPT_FAIL(VALID == INVALID);
        ASSERT_OPT_FAIL(INVALID == VALID);

        ASSERT_OPT_PASS(VALID == VALID);

        ASSERT_OPT_FAIL(VALID != INVALID);
        ASSERT_OPT_FAIL(INVALID != VALID);

        ASSERT_OPT_PASS(VALID != VALID);
    }
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
void TestDriver<VALUE, ALLOCATOR, CONVERTER>::testCase9()
{
    // ------------------------------------------------------------------------
    // ITERATOR BASIC ACCESSORS
    //
    // Concerns:
    //: 1 Dereferencing an iterator refers to the expected element.
    //:
    //: 2 Dereferencing an iterator modifies 'dereferenceable' flag of the
    //:   expected element.
    //:
    //: 3 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid TestValuesArray object values.
    //:
    //: 2 For each row (representing a distinct TestValuesArray object value,
    //:   'V') in the table described in P-1:
    //:
    //:   1 Create a constant TestValuesArray object, passing a value
    //:     specification.
    //:
    //:   2 Create two boolean arrays, having the same size as the
    //:     TestValuesArray object, to emulate TestValuesArray's
    //:     'dereferenceable' and 'validIterator' arrays.
    //:
    //:   3 Create an iterator, referring to the first value in the
    //:     TestValuesArray and two arrays, specified in P-2.2.
    //:
    //:   4 Iterate through the whole TestValuesArray and verify that
    //:     'operator *' returns correct values.  (C-1)
    //:
    //:   5 Having direct access to boolean arrays, specified in P-2.2, verify
    //:     that 'operator *' modifies value's flags correctly.  (C-2)
    //:
    //: 3 Verify defensive checks are triggered for invalid values.  (C-3)
    //
    // Testing:
    //  const VALUE& operator *() const;
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    if (verbose) printf("\tTesting behavior.\n");
    {
        // We need to exclude empty specification, since 'VALUE' array is not
        // created in this case.

        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int     LINE = DATA[ti].d_line;
            const char   *SPEC = DATA[ti].d_spec;
            const size_t  SIZE = strlen(SPEC);

            if (veryVeryVerbose) { T_ T_ P_(LINE) P(SPEC); }

            const Obj  VALUES_ARRAY(SPEC);
            bool      *isDerefArray = new bool[SIZE];
            bool      *isValidArray = new bool[SIZE];

            for (size_t i = 0; i < SIZE; ++i) {
                isDerefArray[i] = true;
                isValidArray[i] = true;
            }

            // Test object creation.

            Iterator        mX(VALUES_ARRAY.data(),
                               VALUES_ARRAY.data() + SIZE,
                               isDerefArray,
                               isValidArray);
            const Iterator& X = mX;

            for (size_t i = 0; i < SIZE; ++i) {
                ASSERTV(LINE, SPEC[i], true == isDerefArray[i]);

                ASSERTV(LINE, SPEC[i],
                        SPEC[i] == CONVERTER::getIdentifier(*X));

                ASSERTV(LINE, SPEC[i], false == isDerefArray[i]);

                ++mX;
            }

            // Reclaim dynamically allocated arrays.

            delete [] isDerefArray;
            delete [] isValidArray;
        }
    }

    if (verbose) printf("\tNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        const char *SPEC = "A";
        bool        tBV = true;   // true  boolean value
        bool        fBV = false;  // false boolean value
        const Obj   V(SPEC);

        // We need to deceive constructor to create 'INVALID' and
        // 'DEREFERENCED' objects.

        fBV = true;

        const Iterator INVALID     (V.data(), V.data(), &tBV, &fBV);
        const Iterator DEREFERENCED(V.data(), V.data(), &fBV, &tBV);
        const Iterator VALID       (V.data(), V.data(), &tBV, &tBV);

        // Make 'INVALID' and 'DEREFERENCED' iterators invalid for indirection
        // operator.

        fBV = false;

        ASSERT_OPT_FAIL(*INVALID);
        ASSERT_OPT_FAIL(*DEREFERENCED);

        ASSERT_OPT_PASS(*VALID);
    }
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
void TestDriver<VALUE, ALLOCATOR, CONVERTER>::testCase8()
{
    // ------------------------------------------------------------------------
    // ITERATOR PRIMARY MANIPULATORS
    //
    // Concerns:
    //: 1 An object created with the value constructor have the
    //:   contractually specified value.
    //:
    //: 2 An object initialized using a value constructor to a valid
    //:   'TestValuesArray' can be incremented using the 'operator++'.
    //:
    //: 3 An object can be used to traverse all values in the array in
    //:   order using the value constructor and the 'operator++'.
    //:
    //: 4 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid TestValuesArray object values.
    //:
    //: 2 For each row (representing a distinct TestValuesArray object value,
    //:   'V') in the table described in P-1:
    //:
    //:   1 Create a constant TestValuesArray object, passing a value
    //:     specification.
    //:
    //:   2 Create two boolean arrays, having the same size as the
    //:     TestValuesArray object, to emulate TestValuesArray's
    //:     'dereferenceable' and 'validIterator' arrays.
    //:
    //:   3 Create an iterator, referring to the first value in the
    //:     TestValuesArray and two arrays, specified in P-2.2.
    //:
    //:   4 Use 'operator++'  to iterate through the all TestValuesArray values
    //:      and (untested) 'operator *' to verify that object's value pointers
    //:      are installed properly and 'operator++' behavior is correct.
    //:      (C-3)
    //:
    //:   5 Having direct access to boolean arrays, specified in P-2.2, verify
    //:     that 'dereferenceable' and 'isValid' object's pointers are
    //:     installed properly and 'operator++' behavior is correct.  (C-1..2)
    //:
    //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
    //
    // Testing:
    //  TestValuesArrayIterator(const VALUE *, const VALUE *, bool *, bool *);
    //  TestValuesArrayIterator& operator++();
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    if (verbose) printf("\tTesting behavior.\n");
    {
        // We need to exclude empty specification, since 'VALUE' array is not
        // created in this case.

        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int     LINE = DATA[ti].d_line;
            const char   *SPEC = DATA[ti].d_spec;
            const size_t  SIZE = strlen(SPEC);

            if (veryVeryVerbose) { T_ T_ P_(LINE) P(SPEC); }

            const Obj  VALUES_ARRAY(SPEC);
            bool      *isDerefArray = new bool[SIZE];
            bool      *isValidArray = new bool[SIZE];

            for (size_t i = 0; i < SIZE; ++i) {
                isDerefArray[i] = true;
                isValidArray[i] = true;
            }

            // Test object creation.

            Iterator        mX(VALUES_ARRAY.data(),
                               VALUES_ARRAY.data() + SIZE,
                               isDerefArray,
                               isValidArray);
            const Iterator& X = mX;

            // Verify any attribute values are installed properly.

            for (size_t i = 0; i < SIZE; ++i) {

                ASSERTV(LINE, SPEC[i],
                        SPEC[i] == CONVERTER::getIdentifier(*X));

                // The indirection operator changes 'dereferenceable' status of
                // 'VALUE' object, iterator pointing to, to 'false', so we need
                // to restore it, to check behavior of the increment operator.

                ASSERTV(LINE, SPEC[i], false == isDerefArray[i]);
                isDerefArray[i] = true;

                ASSERTV(LINE, SPEC[i], true == isDerefArray[i]);
                ASSERTV(LINE, SPEC[i], true == isValidArray[i]);

                ++mX;

                ASSERTV(LINE, SPEC[i], false == isDerefArray[i]);
                ASSERTV(LINE, SPEC[i], false == isValidArray[i]);
            }

            // Reclaim dynamically allocated arrays.

            delete [] isDerefArray;
            delete [] isValidArray;
        }
    }

    if (verbose) printf("\tNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        const char *SPEC = "A";
        bool        tBV = true;   // true  boolean value
        bool        fBV = false;  // false boolean value
        const Obj   V(SPEC);

        // Constructor testing.
        {
            ASSERT_SAFE_FAIL(Iterator(0,        V.data(), &tBV, &tBV));
            ASSERT_SAFE_FAIL(Iterator(V.data(), 0,        &tBV, &tBV));
            ASSERT_SAFE_FAIL(Iterator(V.data(), V.data(), 0,    &tBV));
            ASSERT_SAFE_FAIL(Iterator(V.data(), V.data(), &tBV, 0));
            ASSERT_SAFE_FAIL(Iterator(V.data(), V.data(), &tBV, &fBV));

            ASSERT_SAFE_PASS(Iterator(V.data(), V.data(), &tBV, &tBV));
        }

        // Increment operator testing.
        {
            // We need to deceive constructor to create 'invalid' object.

            fBV = true;

            Iterator invalid(V.data(), V.data() + 1, &tBV, &fBV);
            Iterator end    (V.data(), V.data(),     &tBV, &tBV);
            Iterator valid  (V.data(), V.data() + 1, &tBV, &tBV);

            // Make 'invalid' iterator invalid.

            fBV = false;

            ASSERT_OPT_FAIL(++invalid);
            ASSERT_OPT_FAIL(++end);

            ASSERT_OPT_PASS(++valid);
        }
    }
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
void TestDriver<VALUE, ALLOCATOR, CONVERTER>::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING POST-INCREMENT POINTER
    //
    // Concerns:
    //: 1 The TestValuesArray_PostIncrementPtr object is properly initialized
    //:   upon construction.
    //:
    //: 2 The 'operator*' returns the reference providing non-modifiable access
    //:   to the value of the entity to which this object refers.
    //:
    //: 3 QoI: asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid TestValuesArray object values.
    //:
    //: 2 For each row (representing a distinct TestValuesArray object value,
    //:   'V') in the table described in P-1:
    //:
    //:   1 Create a constant TestValuesArray object, passing a value
    //:     specification.
    //:
    //:   2 Use address of each value, held in the array, to create a
    //:     TestValuesArray_PostIncrementPtr object.
    //:
    //:   3 Using the 'operator*', verify that TestValuesArray_PostIncrementPtr
    //:     object is properly initialized.  (C-1..2)
    //:
    //: 3 Verify defensive checks are triggered for invalid values.  (C-3)
    //
    // Testing:
    //  explicit TestValuesArray_PostIncrementPtr(const VALUE* ptr);
    //  const VALUE& operator*() const;
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    typedef TestValuesArray_PostIncrementPtr<VALUE> PostIncrementPtr;

    if (verbose) printf("\tTesting behavior.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE = DATA[ti].d_line;
            const char   *SPEC = DATA[ti].d_spec;
            const size_t  SIZE = strlen(SPEC);

            if (veryVeryVerbose) { T_ T_ P_(LINE) P(SPEC); }

            // Construct object.

            const Obj VALUES(SPEC);

            for (size_t i = 0; i < SIZE; ++i) {
                PostIncrementPtr        mX(VALUES.data() + i);
                const PostIncrementPtr& X = mX;

                ASSERTV(LINE, SPEC[i],
                        SPEC[i] == CONVERTER::getIdentifier(*X));
            }
        }  // end foreach row
    }

    if (verbose) printf("\tNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        const char *SPEC = "A";
        const Obj   VALUES(SPEC);

        (void) VALUES;

        ASSERT_SAFE_FAIL(PostIncrementPtr(0));
        ASSERT_SAFE_PASS(PostIncrementPtr(VALUES.data()));
    }
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
void TestDriver<VALUE, ALLOCATOR, CONVERTER>::testCase6()
{
    // ------------------------------------------------------------------------
    //  TESTING SUBSCRIPT OPERATOR
    //
    // Concerns:
    //: 1 The 'operator[]' returns correct reference.
    //:
    //: 2 QoI: asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:
    //:
    //:   1 Create an object, passing a value specification.
    //:
    //:   2 Obtain elements with index from 0 to size - 1 in consecutive
    //:     order and verify their values.  (C-1)
    //:
    //: 3 Verify defensive checks are triggered for invalid values.  (C-2)
    //
    // Testing:
    //  const VALUE& operator[](size_t index) const;
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    if (verbose) printf("\tTesting behavior.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE = DATA[ti].d_line;
            const char   *SPEC = DATA[ti].d_spec;
            const size_t  SIZE = strlen(SPEC);

            if (veryVeryVerbose) { T_ T_ P_(LINE) P(SPEC); }

            // Construct object.

            Obj        mX(SPEC);
            const Obj& X = mX;

            for (size_t i = 0; i < SIZE; ++i) {
                ASSERTV(LINE, SPEC[i],
                        SPEC[i] == CONVERTER::getIdentifier(X[i]));
            }

        }  // end foreach row
    }

    if (verbose) printf("\tNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        const char   *SPEC = "A";
        const size_t  SIZE = strlen(SPEC);

        const Obj NotEmpty(SPEC);
        const Obj Empty("");

        ASSERT_SAFE_FAIL(Empty[0]);
        ASSERT_SAFE_FAIL(NotEmpty[SIZE]);
        ASSERT_SAFE_PASS(NotEmpty[SIZE-1]);
        ASSERT_SAFE_PASS(NotEmpty[0]);
    }
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
void TestDriver<VALUE, ALLOCATOR, CONVERTER>::testCase5()
{
    // ------------------------------------------------------------------------
    // TESTING OTHER CONSTRUCTORS
    //
    // Concerns:
    //: 1 An object created with the default constructor (with or without a
    //:   supplied allocator) has the contractually specified default value.
    //:
    //: 2 If an allocator is NOT supplied to the default constructor, the
    //:   default allocator in effect at the time of construction becomes the
    //:   object allocator for the resulting object.
    //:
    //: 3 If an allocator IS supplied to the default constructor, that
    //:   allocator becomes the object allocator for the resulting object.
    //:
    //: 4 Any memory allocation is from the object allocator.
    //:
    //: 5 Every object releases any allocated memory at destruction.
    //
    // Plan:
    //: 1 Execute a loop creating two distinct objects, in turn, each object
    //:   configured differently identified by 'CONFIG':
    //:
    //:   'a': passing a value specification, but without passing an allocator;
    //:
    //:   'b': passing a value specification, and the test allocator distinct
    //:        from the default allocator.
    //:
    //: 2 For each of the two iterations in P-1:
    //:
    //:   1 Create three 'bslma_TestAllocator' objects, and install one as the
    //:     current default allocator (note that a ubiquitous test allocator is
    //:     already installed as the global allocator).
    //:
    //:   2 Choose the constructor depending on 'CONFIG' to dynamically create
    //:     an object, with its object allocator configured appropriately (see
    //:     P-1); use a distinct test allocator for the object's footprint.
    //:
    //:   3 Use the appropriate test allocator to verify that:
    //:
    //:     1 If an allocator was not supplied at construction (P-1a), the
    //:       non-object allocator doesn't allocate any memory.  (C-2,4)
    //:
    //:     2 If an allocator was supplied at construction (P-1b), all memory
    //:       is allocated from it.  (C-3..4)
    //:
    //:   4 Verify that all of the attributes of each object have their
    //:     expected values.  (C-1)
    //:
    //:   5 Destroy target object and verify, that all memory has been
    //:     released.  (C-5)
    //
    // Testing:
    //  explicit TestValuesArray();
    //  explicit TestValuesArray(ALLOCATOR basicAllocator);
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s, %s\n", NameOf<VALUE>().name(),
                                    k_VALUE_USES_BSLMA ? "bslma" : "stateful");

    const char   DEFAULT_SPEC[] =
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const size_t DEFAULT_SIZE = strlen(DEFAULT_SPEC);

    {
        for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the constructor

            if (veryVeryVerbose) { T_ P(CONFIG) }

            bslma::TestAllocator doa("default",   veryVeryVeryVerbose);
            bslma::TestAllocator foa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator soa("supplied",  veryVeryVeryVerbose);

            ALLOCATOR da(&doa);
            ALLOCATOR sa(&soa);

            // Install default allocator.

            bslma::DefaultAllocatorGuard dag(&doa);

            Obj *objPtr;

            // Construct object.

            switch (CONFIG) {
              case 'a': {
                objPtr = new (foa) Obj;
              } break;
              case 'b': {
                objPtr = new (foa) Obj(sa);
              } break;
              default: {
                ASSERTV(CONFIG, !"Bad constructor config.");
                return;                                               // RETURN
              } break;
            }

            // Verify no allocation from the default allocator, unless 'VALUE'
            // uses 'bslma::Allocator' and the 'ALLOCATOR' template argument is
            // not 'bsl::allocator'.

            ASSERTV(NameOf<Obj>(), CONFIG, doa.numBlocksTotal(),
                    (!k_IS_BSL_ALLOCATOR && k_VALUE_USES_BSLMA) ||
                                                    0 == doa.numBlocksTotal());

            // Verify sizes of allocated memory blocks.

            ASSERTV(CONFIG, foa.numBytesInUse(),
                    sizeof(Obj) == foa.numBytesInUse());

            if (CONFIG == 'a') {
                // MallocFreeAllocator is used.  It has no accessors, so we
                // can't confirm memory allocation.

                ASSERTV(CONFIG, soa.numBytesInUse(),
                        0 == soa.numBytesInUse());
            } else {
                // We can't calculate exact size of allocated memory, as
                // 'VALUE' objects can allocate some extra memory in their
                // constructors.

                ASSERTV(CONFIG, soa.numBytesInUse(),
                        0 < soa.numBytesInUse());
            }

            // Verify the expected attributes values.

            Obj&       mX = *objPtr;
            const Obj& X = mX;

            ASSERTV(CONFIG, DEFAULT_SIZE == X.size());
            ASSERTV(CONFIG, 0            != X.data());

            for (size_t i = 0; i < DEFAULT_SIZE; ++i) {
                ASSERTV(
                     CONFIG, DEFAULT_SPEC[i],
                     DEFAULT_SPEC[i] == CONVERTER::getIdentifier(X.data()[i]));
            }

            // Reclaim dynamically allocated object.

            foa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(foa.numBlocksInUse(), 0 == foa.numBlocksInUse());
            ASSERTV(soa.numBlocksInUse(), 0 == soa.numBlocksInUse());
        }  // end foreach configuration
    }
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
void TestDriver<VALUE, ALLOCATOR, CONVERTER>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    //
    // Concerns:
    //: 1 Each accessor returns the value of the correct property of the
    //:   object.
    //:
    //: 2 Each accessor method is declared 'const'.
    //:
    //: 3 No accessor allocates any memory.
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:
    //:
    //:   1 Create an object, passing a value specification.
    //:
    //:   2 Use 'size' to verify the object contains the expected number of
    //:     elements.  (C-1..2)
    //:
    //:   3 Use 'data' to verify the values are as expected. (C-1..2)
    //:
    //:   4 Monitor the memory allocated from object allocator before and after
    //:     calling the accessor and verify that there is no change in total
    //:     memory allocation.  (C-3)
    //
    // Testing:
    //  const VALUE *data() const;
    //  size_t size() const;
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE = DATA[ti].d_line;
            const char   *SPEC = DATA[ti].d_spec;
            const size_t  SIZE = strlen(SPEC);

            if (veryVeryVerbose) { T_ T_ P_(LINE) P(SPEC); }

            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            // Construct object.

            Obj        mX(SPEC, &ta);
            const Obj& X = mX;

            bslma::TestAllocatorMonitor tam(&ta);

            // Verify basic accessors

            ASSERTV(LINE, SIZE == X.size());

            for (size_t i = 0; i < SIZE; ++i) {
                ASSERTV(LINE, SPEC[i],
                        SPEC[i] == CONVERTER::getIdentifier(X.data()[i]));
            }

            ASSERTV(LINE, tam.isTotalSame());

        }  // end foreach row
    }
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
void TestDriver<VALUE, ALLOCATOR, CONVERTER>::testCase3()
{
    // ------------------------------------------------------------------------
    // PRIMARY MANIPULATORS
    //
    // Concerns:
    //: 1 An object created with the constructor (with or without a supplied
    //:   allocator) has the value, configured according to the specified
    //:   'spec'.
    //:
    //: 2 If an allocator is NOT supplied to the constructor, the
    //:   default allocator in effect at the time of construction becomes the
    //:   object allocator for the resulting object.
    //:
    //: 3 If an allocator IS supplied to the constructor, that allocator
    //:   becomes the object allocator for the resulting object.
    //:
    //: 4 Any memory allocation is from the object allocator.
    //:
    //: 5 Every object releases any allocated memory at destruction.
    //:
    //: 6 QoI: asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:
    //:
    //:   1 Execute an inner loop creating two distinct objects, in turn, each
    //:     object having the same value, 'V', but configured differently
    //:     identified by 'CONFIG':
    //:
    //:     'a': passing a value specification, but without passing an
    //:          allocator;
    //:
    //:     'b': passing a value specification, and the test allocator distinct
    //:          from the default allocator.
    //:
    //:   2 For each of the two iterations in P-2.1:
    //:
    //:     1 Create three 'bslma_TestAllocator' objects, and install one as
    //:       the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     2 Choose the value constructor depending on 'CONFIG' to dynamically
    //:       create an object using a value specification, with its object
    //:       allocator configured appropriately (see P-2.2.1); use a distinct
    //:       test allocator for the object's footprint.
    //:
    //:     3 Use the appropriate test allocator to verify that:
    //:
    //:       1 If an allocator was not supplied at construction (P-2.1a), the
    //:         non-object allocator doesn't allocate any memory.  (C-2,4)
    //:
    //:       2 If an allocator was supplied at construction (P-2.1b), all
    //:         memory is allocated from it.  (C-3..4)
    //:
    //:     4 Use the (untested) basic accessors to verify that all of the
    //:       attributes of each object have their expected values.  (C-1)
    //:
    //:     5 Destroy target object and verify, that all memory has been
    //:       released.  (C-5)
    //:
    //: 3 Verify defensive checks are triggered for invalid values.  (C-6)
    //
    // Testing:
    //  explicit TestValuesArray(const char *spec);
    //  explicit TestValuesArray(const char *spec, ALLOCATOR basicAllocator);
    //  ~TestValuesArray();
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s, %s\n", NameOf<VALUE>().name(),
                                    k_VALUE_USES_BSLMA ? "bslma" : "stateful");

    if (verbose) printf("\tTesting behavior.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE = DATA[ti].d_line;
            const char   *SPEC = DATA[ti].d_spec;
            const size_t  SIZE = strlen(SPEC);

            if (veryVeryVerbose) { T_ T_ P_(LINE) P(SPEC); }
            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the constructor

                if (veryVeryVerbose) { T_ T_ P(CONFIG) }

                bslma::TestAllocator doa("default",   veryVeryVeryVerbose);
                bslma::TestAllocator foa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator soa("supplied",  veryVeryVeryVerbose);

                ALLOCATOR da(&doa);
                ALLOCATOR sa(&soa);

                // Install default allocator.

                bslma::DefaultAllocatorGuard dag(&doa);

                Obj *objPtr;

                // Construct object.

                switch (CONFIG) {
                  case 'a': {
                    objPtr = new (foa) Obj(SPEC);
                  } break;
                  case 'b': {
                    objPtr = new (foa) Obj(SPEC, sa);
                  } break;
                  default: {
                    ASSERTV(LINE, CONFIG, !"Bad constructor config.");
                    return;                                           // RETURN
                  } break;
                }

                // Verify no allocation from the default allocator.

                ASSERTV(NameOf<Obj>(), SPEC, CONFIG, doa.numBlocksTotal(),
                        (!k_IS_BSL_ALLOCATOR && k_VALUE_USES_BSLMA) ||
                                                        !doa.numBlocksTotal());

                // Verify sizes of allocated memory blocks.

                ASSERTV(LINE, CONFIG, foa.numBytesInUse(),
                        sizeof(Obj) == foa.numBytesInUse());

                if (CONFIG == 'a') {
                    // MallocFreeAllocator is used.  It has no accessors, so we
                    // can't confirm memory allocation.

                    ASSERTV(LINE, CONFIG, soa.numBytesInUse(),
                            0 == soa.numBytesInUse());
                } else {
                    // We can't calculate exact size of allocated memory, as
                    // 'VALUE' objects can allocate some extra memory in their
                    // constructors.

                    ASSERTV(LINE, CONFIG, soa.numBytesInUse(),
                            0 < soa.numBytesInUse());
                }

                // Verify the expected attributes values.

                Obj&       mX = *objPtr;
                const Obj& X = mX;

                ASSERTV(LINE, CONFIG, SIZE == X.size());
                ASSERTV(LINE, CONFIG, 0    != X.data());

                for (size_t i = 0; i < SIZE; ++i) {
                    ASSERTV(LINE, CONFIG, SPEC[i],
                            SPEC[i] == CONVERTER::getIdentifier(X.data()[i]));
                }

                // Testing destructor.

                foa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(LINE, foa.numBlocksInUse(), 0 == foa.numBlocksInUse());
                ASSERTV(LINE, soa.numBlocksInUse(), 0 == soa.numBlocksInUse());
            }  // end foreach configuration
        }  // end foreach row
    }

    if (verbose) printf("\tNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        const char           *SPEC = "A";
        bslma::TestAllocator  toa("test", veryVeryVeryVerbose);

        ALLOCATOR ta(&toa);

        ASSERT_SAFE_FAIL(Obj(static_cast<const char *>(0)));
        ASSERT_SAFE_FAIL(Obj(static_cast<const char *>(0), ta));
        ASSERT_SAFE_PASS(Obj(SPEC));
        ASSERT_SAFE_PASS(Obj(SPEC, ta));
    }
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
void TestDriver<VALUE, ALLOCATOR, CONVERTER>::testCase2()
{
    // ------------------------------------------------------------------------
    // TESTING DEFAULT CONVERTER
    //
    // Concerns:
    //: 1 For all test types, invoking the
    //:   'TestValuesArray_DefaultConverter::createInplace' class method
    //:   passing in a char value return a new object of that type.
    //
    //: 2 For all test types, the
    //:   'TestValuesArray_DefaultConverter::createInplace' method supports
    //:   char values from 0 to 127.
    //
    // Plan:
    //: 1 Create a buffer, sufficient for storing an object of the (template
    //    parameter) type and do the following: For each char value from 0 to
    //    127, create an object using
    //    'TestValuesArray_DefaultConverter::createInplace' passing in the char
    //    value.  Verify that the integer value returned from
    //    'TemplateTestFacility::getIdentifier' compare equal to the value
    //    passed to 'createInplace'.  (C-1..2)
    //
    // Testing:
    //   createInplace(VALUE *objPtr, char value, ALLOCATOR allocator);
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    typedef TestValuesArray_DefaultConverter<VALUE, ALLOCATOR> Converter;

    bslma::TestAllocator         defaultAllocator("defaultAllocator",
                                                  veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultGuard(&defaultAllocator);
    bslma::TestAllocator         objectAllocator("objectAllocator",
                                                 veryVeryVeryVerbose);

    bsls::ObjectBuffer<VALUE>  buffer;
    VALUE&                     mX = buffer.object();
    const VALUE&               X = mX;
    VALUE                     *address = buffer.address();

    for (size_t ti = 0; ti <= 127; ++ti) {
        Converter::createInplace(address,
                                 static_cast<char>(ti),
                                 &objectAllocator);
        int value = TemplateTestFacility::getIdentifier<VALUE>(X);

        if (veryVeryVerbose) {
            T_ T_ P_(ti) P(value) }

        ASSERTV(ti, value, ti == value);

        bslma::DestructionUtil::destroy(address);
    }

    ASSERTV(defaultAllocator.numBlocksTotal(),
            0 == defaultAllocator.numBlocksTotal());
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
void TestDriver<VALUE, ALLOCATOR, CONVERTER>::testCase1()
{
    // ------------------------------------------------------------------------
    // BREATHING TEST
    //   This case exercises (but does not fully test) basic functionality.
    //
    // Concerns:
    //: 1 The class is sufficiently functional to enable comprehensive
    //:   testing in subsequent test cases.
    //
    // Plan:
    //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
    //
    // Testing:
    //   BREATHING TEST
    // ------------------------------------------------------------------------

    bsltf::TestValuesArray<VALUE> X;
    bsltf::TestValuesArray<VALUE> Y("ABC");

    ASSERTV(X.size(), 52 == X.size());

    typename bsltf::TestValuesArray<VALUE>::iterator IT = Y.begin();
    ASSERTV(IT != Y.end());
    ASSERTV('A' == *IT);
    ++IT;
    ASSERTV(IT != Y.end());
    ASSERTV('B' == *IT);
    ++IT;
    ASSERTV(IT != Y.end());
    ASSERTV('C' == *IT);
    ++IT;
    ASSERTV(IT == Y.end());
}

// Create a version of 'TestDriver' that uses the 'StdStatefulAllocator'
// instead of 'bsl::allocator' in order to run some of our tests and make sure
// that the 'TestValuesArray' is compatible with the standard stateful
// allocator.

template <class VALUE>
struct StatefulTestDriver : public TestDriver<VALUE,
                                              StdStatefulAllocator<VALUE,
                                                                   0,
                                                                   0,
                                                                   0,
                                                                   0> > {};

// ============================================================================
//                              USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Example 1: Testing a Simple Template Function
///- - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we have a function that we would like to test.  This function
// take in a range defined by two input iterators and returns the largest value
// in that range.
//
// First, we define the function we would like to test:
//..
    template <class VALUE, class INPUT_ITERATOR>
    VALUE myMaxValue(INPUT_ITERATOR first, INPUT_ITERATOR last)
        // Return the largest value referred to by the iterators in the range
        // beginning at the specified 'first' and up to, but not including, the
        // specified 'last'.  The behavior is undefined unless [first, last)
        // specifies a valid range and 'first != last'.
    {
        ASSERT(first != last);

        VALUE largestValue(*first);
        ++first;
        for(;first != last; ++first) {
            // Store in temporary variable to avoid dereferencing twice.
            const VALUE& temp = *first;
            if (largestValue < temp) {
                largestValue = temp;
            }
        }
        return largestValue;
    }
//..
// Next, we implement a test function 'runTest' that allows the function to be
// tested with different types:
//..
    template <class VALUE>
    void runTest()
        // Test driver.
    {
//..
//  Then, we define a set of test values and expected results:
//..
        struct {
            const char *d_spec;
            const char  d_result;
        } DATA[] = {
            { "A",     'A' },
            { "ABC",   'C' },
            { "ADCB",  'D' },
            { "EDCBA", 'E' }
        };
        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;
//..
//  Now, for each set of test values, verify that the function return the
//  expected result.
//..
        for (size_t i = 0; i < NUM_DATA; ++i) {
            const char *const SPEC = DATA[i].d_spec;
            const VALUE       EXP  =
                  bsltf::TemplateTestFacility::create<VALUE>(DATA[i].d_result);

            bsltf::TestValuesArray<VALUE> values(SPEC);
            ASSERT(EXP == myMaxValue<VALUE>(values.begin(), values.end()));
        }
    }
//..

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 16: {
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
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

// Finally, we invoke the test function to verify our function is implemented
// correctly.  The test function to run without triggering the 'assert'
// statement:
//..
    runTest<char>();
//..
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING MANIPULATORS"
                            "\n====================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase15,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // ITERATOR DEREFERENCE OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nITERATOR DEREFERENCE OPERATOR"
                            "\n=============================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // ITERATOR POST-INCREMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nITERATOR POST-INCREMENT OPERATOR"
                            "\n================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // ITERATOR COPY-ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nITERATOR COPY-ASSIGNMENT OPERATOR"
                            "\n=================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // ITERATOR COPY CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nITERATOR COPY CONSTRUCTOR"
                            "\n=========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // ITERATOR EQUALITY-COMPARISON OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nITERATOR EQUALITY-COMPARISON OPERATORS"
                            "\n======================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase10,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // ITERATOR BASIC ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nITERATOR BASIC ACCESSORS"
                            "\n========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // ITERATOR PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nITERATOR PRIMARY MANIPULATORS"
                            "\n=============================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING POST-INCREMENT POINTER
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING POST-INCREMENT POINTER"
                            "\n==============================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING SUBSCRIPT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING SUBSCRIPT OPERATOR"
                            "\n==========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OTHER CONSTRUCTORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING OTHER CONSTRUCTORS"
                            "\n==========================\n");

        RUN_EACH_TYPE(StatefulTestDriver,
                      testCase5,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase5,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC ACCESSORS"
                            "\n===============\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nPRIMARY MANIPULATORS"
                            "\n====================\n");

        RUN_EACH_TYPE(StatefulTestDriver,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING DEFAULT CONVERTER
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING DEFAULT CONVERTER"
                            "\n=========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef bsl::allocator<char> BreathingTestAllocator;
        typedef TestValuesArray_DefaultConverter<char, BreathingTestAllocator>
                                                        BreathingTestConverter;

        TestDriver<char,
                   BreathingTestAllocator,
                   BreathingTestConverter>::testCase1();

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
