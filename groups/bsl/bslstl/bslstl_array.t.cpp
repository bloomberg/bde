// bslstl_array.t.cpp                                                 -*-C++-*-
#include <bslstl_array.h>

#include <bsla_maybeunused.h>

#include <bslh_hash.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorproctor.h>
#include <bslma_sequentialallocator.h>
#include <bslma_testallocator.h>

#include <bslmf_movableref.h>

#include <bsltf_movabletesttype.h>
#include <bsltf_nondefaultconstructibletesttype.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_nameof.h>
#include <bsls_util.h>

#include <algorithm>
#include <cstddef>
#include <ctype.h>
#include <limits.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utility>

using namespace BloombergLP;
using namespace bslstl;

// BDE_VERIFY pragma: -TP06  // 'Testing' section should contain this item

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// Primary Manipulators:
//: o 'operator[]'
//
// Basic Accessors:
//: o 'operator[]'
//: o 'size()'
//
// We will follow our standard 10-case approach to testing value-semantic
// types, with the default constructor and primary manipulators tested fully in
// case 2 and additional operators, accessors, and manipulators tested above
// case 10.  Aggregate initialization is also tested in test case 15 by
// initializing with a variety of sizes and verifying all values are as
// expected.
//
// Global Concerns:
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o Modifying array elements produces expected results.
//: o No memory is every allocated.
//
// Global Assumptions:
//: o ACCESSOR methods are 'const' thread-safe.
//: o The 'VALUE_TYPE' of the array is assignable, default constructable, and
//:   supports operator==.
// ----------------------------------------------------------------------------
// TRAITS:
// [24] bslalg::HasStlIterators
//
// CREATORS
// [ 2] array<TYPE, SIZE>();
// [ 7] array<TYPE,SIZE>(const array<TYPE,SIZE>& original);
// [ 2] ~array();
// [15] array<TYPE, SIZE>{{v1, v2, v3}};
//
// MANIPULATORS
// [14] iterator begin();
// [14] iterator end();
// [14] reverse_iterator rbegin();
// [14] reverse_iterator rend();
// [13] void fill(const TYPE& value)
// [ 9] array<TYPE,SIZE>& operator=(const array<TYPE,SIZE>& rhs);
// [ 8] void swap(array&);
// [ 2] reference operator[](size_type position);
// [18] reference at(size_type position);
// [19] reference back();
// [19] reference front();
// [20] const_pointer data() const;
//
// ACCESSORS
// [14] const_iterator begin() const;
// [14] const_iterator end() const;
// [14] const_reverse_iterator rbegin() const;
// [14] const_reverse_iterator rend() const;
// [14] const_iterator cbegin() const;
// [14] const_iterator cend() const;
// [14] const_reverse_iterator crbegin() const;
// [14] const_reverse_iterator crend() const;
// [ 4] reference operator[](size_type position) const;
// [ 4] size_type size() const;
// [18] reference at(size_type position) const;
// [17] size_type max_size() const;
// [17] bool empty() const;
// [19] const_reference back() const;
// [19] const_reference front() const;
// [20] const_pointer data() const;
//
// FREE OPERATORS
// [ 6] bool operator==(const array<TYPE,SIZE>&, const array<TYPE,SIZE>&);
// [ 6] bool operator!=(const array<TYPE,SIZE>&, const array<TYPE,SIZE>&);
// [16] bool operator<(const array<TYPE,SIZE>&, const array<TYPE,SIZE>&);
// [16] bool operator>(const array<TYPE,SIZE>&, const array<TYPE,SIZE>&);
// [16] bool operator<=(const array<TYPE,SIZE>&, const array<TYPE,SIZE>&);
// [16] bool operator>=(const array<TYPE,SIZE>&, const array<TYPE,SIZE>&);
// [16] auto operator<=>(const array<TYPE,SIZE>&, const array<TYPE,SIZE>&);
// [ 8] void swap(array<TYPE,SIZE>&, array<TYPE,SIZE>&);
// [21] std::tuple_element<bsl::array<TYPE, SIZE> >
// [21] std::tuple_size<bsl::array<TYPE, SIZE> >
// [21] TYPE& get(bsl::array<TYPE, SIZE>& a)
// [21] const TYPE& get(const bsl::array<TYPE, SIZE>& a)
// [21] const TYPE&& get(const bsl::array<TYPE, SIZE>&& a)
// [21] TYPE&& get(bsl::array<TYPE, SIZE>&& a)
// [22] void hashAppend(HASH_ALGORITHM&, const bsl::array<TYPE, SIZE>&);
// [26] array<TYPE, SIZE> to_array(TYPE (&src)[SIZE]);
// [26] array<TYPE, SIZE> to_array(TYPE (&&src)[SIZE]);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [27] USAGE EXAMPLE
// [25] CLASS TEMPLATE DEDUCTION GUIDES
// [23] CONCERN: 'constexpr' FUNCTIONS ARE USABLE IN CONSTANT EVALUATION

// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] int ggg(array<TYPE,SIZE> *object, const char *spec, bool verboseFlag);
// [ 3] array<TYPE,SIZE>& gg(array<TYPE,SIZE> *object, const char *spec);

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

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -SP01:  //  Misspelled word 'printf'

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU
// BDE_VERIFY pragma: pop

//=============================================================================
//                       GLOBAL OBJECTS SHARED BY TEST CASES
//-----------------------------------------------------------------------------

// STATIC DATA
static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                 GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FD01   // Function contracts are descriptive text
// BDE_VERIFY pragma: -AC02   // Implicit copy ctor is not allocator-aware

// suppress comparison of 'unsigned' expression is always false warnings
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

namespace bsl {

// array-specific print function.
template <class TYPE, size_t SIZE>
inline
void debugprint(const array<TYPE, SIZE>& v)
{
    if (v.empty()) {
        printf("<empty>");
    }
    else {
        putchar('{');
        const size_t sz = v.size();
        for (size_t ii = 0; ii < sz; ++ii) {
            const char c = static_cast<char>(
                            bsltf::TemplateTestFacility::getIdentifier(v[ii]));
            putchar(c ? c : '@');
        }
        putchar('}');
    }
    fflush(stdout);
}

}  // close namespace bsl

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

//=============================================================================
//                      TEST APPARATUS: GENERATOR FUNCTIONS
//-----------------------------------------------------------------------------

// The generating functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters [A .. E] correspond to arbitrary (but unique) char values to be
// created in the 'bsl::array<TYPE, SIZE>' object.
//..
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>       ::= <EMPTY>   | <LIST>
//
// <EMPTY>      ::=
//
// <LIST>       ::= <ELEMENT>    | <ELEMENT><LIST>
//
// <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'Y'
//                                      // unique but otherwise arbitrary
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object unchanged.
// "A"          Set the value corresponding to A at index 0.
// "AA"         Set two values both corresponding to A at indices 0 and 1.
// "ABC"        Set three values corresponding to A, B, and C at indices 0, 1,
//              and 2 respectively.
//-----------------------------------------------------------------------------
//..

// suppress comparison of 'unsigned' expression is always false warnings
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

template <class TYPE, size_t SIZE>
void resetMovedInto(bsl::array<TYPE, SIZE> *object)
{
    typedef bsltf::TemplateTestFacility TstFacility;

    for (size_t i = 0; i < SIZE; ++i) {
        TstFacility::setMovedIntoState(object->data() + i,
                                       bsltf::MoveState::e_NOT_MOVED);
    }
}

template <class TYPE, size_t SIZE>
int ggg(bsl::array<TYPE, SIZE> *object,
        const char             *spec,
        bool                    verboseFlag = true)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator function 'operator[]'.  Optionally
    // specify 'false' for 'verboseFlag' to suppress 'spec' syntax error
    // messages.  Return the index of the first invalid character, and a
    // negative value otherwise.  Note that this function is used to implement
    // 'gg' as well as allow for verification of syntax error detection.  Any
    // elements in the array beyond the 'spec' string length will be assigned
    // the value generated by 'TestFacility::create<TYPE>(0)'.
{
    enum { SUCCESS = -1 };
    typedef bsltf::TemplateTestFacility TestFacility;

    size_t LENGTH = SIZE < strlen(spec) ? SIZE : strlen(spec);
    size_t i;
    for (i = 0; i < LENGTH; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Y') {
            (*object)[i] = TestFacility::create<TYPE>(spec[i]);
        }
        else {
            if (verboseFlag) {
                printf("Error, bad character (%c) "
                       "in spec (%s) at position %zd.\n", spec[i], spec, i);
            }

            // Discontinue processing this spec.

            return static_cast<int>(i);                               // RETURN
        }
    }
    for (; i < SIZE; ++i) {
        (*object)[i] = TestFacility::create<TYPE>(0);
    }
    resetMovedInto(object);
    return SUCCESS;
}

template <class TYPE, size_t SIZE>
bsl::array<TYPE, SIZE>& gg(bsl::array<TYPE, SIZE> *object,
                           const char             *spec)
    // Return, by reference, the specified 'object' with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

// suppress comparison of 'unsigned' expression is always false warnings
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

//=============================================================================
//                  HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                            // ====================
                            // class CountedDefault
                            // ====================

template<class TYPE>
class CountedDefault
    //Wrapper class to track number of times constructor and destructor called.
{
  private:
    // CLASS DATA

    static int s_numConstructed;  // track number of objects constructed with
                                  // default constructor
    // DATA
    TYPE d_val;                   // value

    // NOT IMPLEMENTED
    CountedDefault(const CountedDefault& other); // = delete;

  public:
    // CLASS METHODS
    static int numConstructed();

    // CREATORS
    CountedDefault();
    ~CountedDefault();
};
                            // --------------------
                            // class CountedDefault
                            // --------------------
// CLASS DATA
template<class TYPE>
int CountedDefault<TYPE>::s_numConstructed = 0;

// CLASS METHODS
template <class TYPE>
int CountedDefault<TYPE>::numConstructed()
{
    return s_numConstructed;
}

// CREATORS
template<class TYPE>
CountedDefault<TYPE>::CountedDefault()
{
    s_numConstructed += 1;
}

template <class TYPE>
CountedDefault<TYPE>::~CountedDefault()
{
    s_numConstructed -= 1;
}

                            // ======================
                            // class LessThanTestType
                            // ======================

class LessThanTestType
    // Class that supports only 'operator<'.
 {
  private:
    // DATA
    int d_val;

  public:
    // CREATORS
    LessThanTestType();
    explicit LessThanTestType(int v);

    // ACCESSORS
    bool operator<(const LessThanTestType& other) const;

};
                            // ----------------------
                            // class LessThanTestType
                            // ----------------------

LessThanTestType::LessThanTestType()
: d_val(0)
{}

LessThanTestType::LessThanTestType(int v)
: d_val(v)
{}

bool LessThanTestType::operator<(const LessThanTestType& other) const
{
    return d_val < other.d_val;
}

                             // =================
                             // class CountedSwap
                             // =================

class CountedSwap {
    // Wrapper class to track number of times swap is called.

  private:
    // CLASS DATA
    static int s_numSwaps;

    // DATA
    int d_value;

  public:
    // CLASS METHODS
    static int numSwaps();

    static void resetNumSwaps();

    // CREATORS
    CountedSwap();
    explicit CountedSwap(int v);

    // FRIENDS
    friend void swap(CountedSwap& a, CountedSwap& b);
};

                             // -----------------
                             // class CountedSwap
                             // -----------------

// CLASS DATA
int CountedSwap::s_numSwaps = 0;

// CLASS METHODS
int CountedSwap::numSwaps()
{
    return s_numSwaps;
}

void CountedSwap::resetNumSwaps()
{
    s_numSwaps = 0;
}

// CREATORS
CountedSwap::CountedSwap()
{
}

CountedSwap::CountedSwap(int v)
: d_value(v)
{
}

// FREE FUNCTIONS
void swap(CountedSwap& a, CountedSwap& b)
{
    ++CountedSwap::s_numSwaps;

    std::swap(a.d_value, b.d_value);
}

                            // ====================
                            // struct AggregateTest
                            // ====================

template<class TYPE, size_t SIZE>
struct AggregateTest
    // This 'struct' provides a namespace for utility functions that generate
    // arrays using aggregate initialization for 'testCase13()'.
{
    static void testAggregate(const char* spec)
    {
        (void) spec;  // suppress compiler warning
        ASSERTV(SIZE, !"Test is not implemented for this size!");
    }
};

template<class TYPE>
struct AggregateTest<TYPE, 5>
    // This 'struct' provides a namespace for utility functions that generate
    // arrays using aggregate initialization for 'testCase13()'.
{
    typedef bsltf::TemplateTestFacility TestFacility;
    typedef bsl::array<TYPE, 5>         Obj;

    static void testAggregate(const char* spec)
    {
        Obj        mW;
        const Obj& W             = gg(&mW, spec);
        const TYPE DEFAULT_VALUE = TestFacility::create<TYPE>(0);

        Obj        mX5 = {TestFacility::create<TYPE>(spec[0]),
                          TestFacility::create<TYPE>(spec[1]),
                          TestFacility::create<TYPE>(spec[2]),
                          TestFacility::create<TYPE>(spec[3]),
                          TestFacility::create<TYPE>(spec[4])};
        const Obj& X5  = mX5;

        ASSERTV(X5.size(), 5    == X5.size());
        ASSERTV(           W[0] == X5[0]);
        ASSERTV(           W[1] == X5[1]);
        ASSERTV(           W[2] == X5[2]);
        ASSERTV(           W[3] == X5[3]);
        ASSERTV(           W[4] == X5[4]);

        Obj        mX4 = {TestFacility::create<TYPE>(spec[0]),
                          TestFacility::create<TYPE>(spec[1]),
                          TestFacility::create<TYPE>(spec[2]),
                          TestFacility::create<TYPE>(spec[3])};
        const Obj& X4  = mX4;

        ASSERTV(X4.size(), 5             == X4.size());
        ASSERTV(           W[0]          == X4[0]);
        ASSERTV(           W[1]          == X4[1]);
        ASSERTV(           W[2]          == X4[2]);
        ASSERTV(           W[3]          == X4[3]);
        ASSERTV(           DEFAULT_VALUE == X4[4]);


        Obj        mX3 = {TestFacility::create<TYPE>(spec[0]),
                          TestFacility::create<TYPE>(spec[1]),
                          TestFacility::create<TYPE>(spec[2])};
        const Obj& X3  = mX3;

        ASSERTV(X3.size(), 5             == X3.size());
        ASSERTV(           W[0]          == X3[0]);
        ASSERTV(           W[1]          == X3[1]);
        ASSERTV(           W[2]          == X3[2]);
        ASSERTV(           DEFAULT_VALUE == X3[3]);
        ASSERTV(           DEFAULT_VALUE == X3[4]);


        Obj        mX2 = {TestFacility::create<TYPE>(spec[0]),
                          TestFacility::create<TYPE>(spec[1])};
        const Obj& X2  = mX2;

        ASSERTV(X2.size(), 5             == X2.size());
        ASSERTV(           W[0]          == X2[0]);
        ASSERTV(           W[1]          == X2[1]);
        ASSERTV(           DEFAULT_VALUE == X2[2]);
        ASSERTV(           DEFAULT_VALUE == X2[3]);
        ASSERTV(           DEFAULT_VALUE == X2[4]);


        Obj        mX1 = {TestFacility::create<TYPE>(spec[0])};
        const Obj& X1  = mX1;

        ASSERTV(X1.size(), 5             == X1.size());
        ASSERTV(           W[0]          == X1[0]);
        ASSERTV(           DEFAULT_VALUE == X1[1]);
        ASSERTV(           DEFAULT_VALUE == X1[2]);
        ASSERTV(           DEFAULT_VALUE == X1[3]);
        ASSERTV(           DEFAULT_VALUE == X1[4]);


        Obj        mX0 = {};
        const Obj& X0  = mX0;

        ASSERTV(X0.size(), 5             == X0.size());
        ASSERTV(           DEFAULT_VALUE == X0[0]);
        ASSERTV(           DEFAULT_VALUE == X0[1]);
        ASSERTV(           DEFAULT_VALUE == X0[2]);
        ASSERTV(           DEFAULT_VALUE == X0[3]);
        ASSERTV(           DEFAULT_VALUE == X0[4]);
    }
};

template<class TYPE>
struct AggregateTest<TYPE, 4>
    // This 'struct' provides a namespace for utility functions that generate
    // arrays using aggregate initialization for 'testCase13()'.
{
    typedef bsltf::TemplateTestFacility TestFacility;
    typedef bsl::array<TYPE, 4>         Obj;

    static void testAggregate(const char* spec)
    {
        Obj        mW;
        const Obj& W             = gg(&mW, spec);
        const TYPE DEFAULT_VALUE = TestFacility::create<TYPE>(0);

        Obj        mX4 = {TestFacility::create<TYPE>(spec[0]),
                          TestFacility::create<TYPE>(spec[1]),
                          TestFacility::create<TYPE>(spec[2]),
                          TestFacility::create<TYPE>(spec[3])};
        const Obj& X4  = mX4;

        ASSERTV(X4.size(), 4    == X4.size());
        ASSERTV(           W[0] == X4[0]);
        ASSERTV(           W[1] == X4[1]);
        ASSERTV(           W[2] == X4[2]);
        ASSERTV(           W[3] == X4[3]);

        Obj        mX3 = {TestFacility::create<TYPE>(spec[0]),
                          TestFacility::create<TYPE>(spec[1]),
                          TestFacility::create<TYPE>(spec[2])};
        const Obj& X3  = mX3;

        ASSERTV(X3.size(), 4             == X3.size());
        ASSERTV(           W[0]          == X3[0]);
        ASSERTV(           W[1]          == X3[1]);
        ASSERTV(           W[2]          == X3[2]);
        ASSERTV(           DEFAULT_VALUE == X3[3]);

        Obj        mX2 = {TestFacility::create<TYPE>(spec[0]),
                          TestFacility::create<TYPE>(spec[1])};
        const Obj& X2  = mX2;

        ASSERTV(X2.size(), 4             == X2.size());
        ASSERTV(           W[0]          == X2[0]);
        ASSERTV(           W[1]          == X2[1]);
        ASSERTV(           DEFAULT_VALUE == X2[2]);
        ASSERTV(           DEFAULT_VALUE == X2[3]);

        Obj        mX1 = {TestFacility::create<TYPE>(spec[0])};
        const Obj& X1  = mX1;

        ASSERTV(X1.size(), 4             == X1.size());
        ASSERTV(           W[0]          == X1[0]);
        ASSERTV(           DEFAULT_VALUE == X1[1]);
        ASSERTV(           DEFAULT_VALUE == X1[2]);
        ASSERTV(           DEFAULT_VALUE == X1[3]);

        Obj        mX0 = {};
        const Obj& X0  = mX0;

        ASSERTV(X0.size(), 4             == X0.size());
        ASSERTV(           DEFAULT_VALUE == X0[0]);
        ASSERTV(           DEFAULT_VALUE == X0[1]);
        ASSERTV(           DEFAULT_VALUE == X0[2]);
        ASSERTV(           DEFAULT_VALUE == X0[3]);
    }
};

template<class TYPE>
struct AggregateTest<TYPE, 3>
    // This 'struct' provides a namespace for utility functions that generate
    // arrays using aggregate initialization for 'testCase13()'.
{
    typedef bsltf::TemplateTestFacility TestFacility;
    typedef bsl::array<TYPE, 3>         Obj;

    static void testAggregate(const char* spec)
    {
        Obj        mW;
        const Obj& W             = gg(&mW, spec);
        const TYPE DEFAULT_VALUE = TestFacility::create<TYPE>(0);

        Obj        mX3 = {TestFacility::create<TYPE>(spec[0]),
                          TestFacility::create<TYPE>(spec[1]),
                          TestFacility::create<TYPE>(spec[2])};
        const Obj& X3  = mX3;

        ASSERTV(X3.size(), 3             == X3.size());
        ASSERTV(           W[0]          == X3[0]);
        ASSERTV(           W[1]          == X3[1]);
        ASSERTV(           W[2]          == X3[2]);

        Obj        mX2 = {TestFacility::create<TYPE>(spec[0]),
                          TestFacility::create<TYPE>(spec[1])};
        const Obj& X2  = mX2;

        ASSERTV(X2.size(), 3             == X2.size());
        ASSERTV(           W[0]          == X2[0]);
        ASSERTV(           W[1]          == X2[1]);
        ASSERTV(           DEFAULT_VALUE == X2[2]);

        Obj        mX1 = {TestFacility::create<TYPE>(spec[0])};
        const Obj& X1  = mX1;

        ASSERTV(X1.size(), 3             == X1.size());
        ASSERTV(           W[0]          == X1[0]);
        ASSERTV(           DEFAULT_VALUE == X1[1]);
        ASSERTV(           DEFAULT_VALUE == X1[2]);

        Obj        mX0 = {};
        const Obj& X0  = mX0;

        ASSERTV(X0.size(), 3             == X0.size());
        ASSERTV(           DEFAULT_VALUE == X0[0]);
        ASSERTV(           DEFAULT_VALUE == X0[1]);
        ASSERTV(           DEFAULT_VALUE == X0[2]);
    }
};

template<class TYPE>
struct AggregateTest<TYPE, 2>
    // This 'struct' provides a namespace for utility functions that generate
    // arrays using aggregate initialization for 'testCase13()'.
{
    typedef bsltf::TemplateTestFacility TestFacility;
    typedef bsl::array<TYPE, 2>         Obj;

    static void testAggregate(const char* spec)
    {
        Obj        mW;
        const Obj& W             = gg(&mW, spec);
        const TYPE DEFAULT_VALUE = TestFacility::create<TYPE>(0);

        Obj        mX2 = {TestFacility::create<TYPE>(spec[0]),
                          TestFacility::create<TYPE>(spec[1])};
        const Obj& X2  = mX2;

        ASSERTV(X2.size(), 2             == X2.size());
        ASSERTV(           W[0]          == X2[0]);
        ASSERTV(           W[1]          == X2[1]);

        Obj        mX1 = {TestFacility::create<TYPE>(spec[0])};
        const Obj& X1  = mX1;

        ASSERTV(X1.size(), 2             == X1.size());
        ASSERTV(           W[0]          == X1[0]);
        ASSERTV(           DEFAULT_VALUE == X1[1]);

        Obj        mX0 = {};
        const Obj& X0  = mX0;

        ASSERTV(X0.size(), 2             == X0.size());
        ASSERTV(           DEFAULT_VALUE == X0[0]);
        ASSERTV(           DEFAULT_VALUE == X0[1]);
    }
};

template<class TYPE>
struct AggregateTest<TYPE, 1>
    // This 'struct' provides a namespace for utility functions that generate
    // arrays using aggregate initialization for 'testCase13()'.
{
    typedef bsltf::TemplateTestFacility TestFacility;
    typedef bsl::array<TYPE, 1>         Obj;

    static void testAggregate(const char* spec)
    {
        Obj        mW;
        const Obj& W             = gg(&mW, spec);
        const TYPE DEFAULT_VALUE = TestFacility::create<TYPE>(0);

        Obj        mX1 = {TestFacility::create<TYPE>(spec[0])};
        const Obj& X1  = mX1;

        ASSERTV(X1.size(), 1             == X1.size());
        ASSERTV(           W[0]          == X1[0]);

        Obj        mX0 = {};
        const Obj& X0  = mX0;

        ASSERTV(X0.size(), 1             == X0.size());
        ASSERTV(           DEFAULT_VALUE == X0[0]);
    }
};

template<class TYPE>
struct AggregateTest<TYPE, 0>
    // This 'struct' provides a namespace for utility functions that generate
    // arrays using aggregate initialization for 'testCase13()'.
{
    typedef bsltf::TemplateTestFacility TestFacility;
    typedef bsl::array<TYPE, 0>         Obj;

    static void testAggregate(const char* spec)
    {
        (void) spec;  // suppress compiler warning

        Obj        mX0 = {};
        const Obj& X0  = mX0;

        ASSERTV(X0.size(), 0 == X0.size());
    }
};

template <class TYPE>
bool isPtrConstant(const TYPE *)
    // This specialization of 'isPtrConstant' is called for const pointer types
    // and always returns 'true'.
{
    return true;
}

template <class TYPE>
bool isPtrConstant(TYPE *)
    // This specialization of 'isPtrConstant' is called for pointer types and
    // always returns 'false'.
{
    return false;
}

template <class TYPE>
bool isRefConstant(const TYPE&)
    // This specialization of 'isRefConstant' is called for const reference
    // types and always returns 'true'.
{
    return true;
}

template <class TYPE>
bool isRefConstant(TYPE&)
    // This specialization of 'isRefConstant' is called for reference types and
    // always returns 'false'.
{
    return false;
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TYPE>
bool isRefConstant(const TYPE&&)
    // This specialization of 'isRefConstant' is called for const rvalue
    // reference types and always returns 'true'.
{
    return true;
}

template <class TYPE>
bool isRefConstant(TYPE&&)
    // This specialization of 'isRefConstant' is called for rvalue reference
    // types and always returns 'false'.
{
    return false;
}
#endif

template<class TYPE, size_t SIZE>
struct TupleApiTest
    // This 'struct' provides a namespace for utility function that test
    // tuple-API for bsl::arrays.
{
    typedef bsltf::TemplateTestFacility TestFacility;

    static void testTupleApi()
    {
        ASSERTV(SIZE, !"Test is not implemented for this size!");
    }
};

template<class TYPE>
struct TupleApiTest<TYPE, 5>
    // This specialization of 'TupleApiTest' provides a namespace for utility
    // function that test tuple-API for bsl::arrays of size 5.
{
    static void testTupleApi()
    {


        typedef                bsl::array<         TYPE, 5>     TA ;

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
        typedef                bsl::array<const    TYPE, 5>     CTA;
        typedef                bsl::array<volatile TYPE, 5>     VTA;

        typedef const          bsl::array<         TYPE, 5>   C_TA ;
        typedef const          bsl::array<const    TYPE, 5>   C_CTA;
        typedef const          bsl::array<volatile TYPE, 5>   C_VTA;

        typedef       volatile bsl::array<         TYPE, 5>   V_TA ;
        typedef       volatile bsl::array<const    TYPE, 5>   V_CTA;
        typedef       volatile bsl::array<volatile TYPE, 5>   V_VTA;

        typedef const volatile bsl::array<         TYPE, 5>  CV_TA ;
        typedef const volatile bsl::array<const    TYPE, 5>  CV_CTA;
        typedef const volatile bsl::array<volatile TYPE, 5>  CV_VTA;

        // Testing 'tuple_size'.
        {
            typedef std::tuple_size< TA     > TS_TA    ;
            typedef std::tuple_size< CTA    > TS_CTA   ;
            typedef std::tuple_size< VTA    > TS_VTA   ;
            typedef std::tuple_size< C_TA   > TS_C_TA  ;
            typedef std::tuple_size< C_CTA  > TS_C_CTA ;
            typedef std::tuple_size< C_VTA  > TS_C_VTA ;
            typedef std::tuple_size< V_TA   > TS_V_TA  ;
            typedef std::tuple_size< V_CTA  > TS_V_CTA ;
            typedef std::tuple_size< V_VTA  > TS_V_VTA ;
            typedef std::tuple_size< CV_TA  > TS_CV_TA ;
            typedef std::tuple_size< CV_CTA > TS_CV_CTA;
            typedef std::tuple_size< CV_VTA > TS_CV_VTA;

            ASSERT((5u == TS_TA::value    ));
            ASSERT((5u == TS_CTA::value   ));
            ASSERT((5u == TS_VTA::value   ));
            ASSERT((5u == TS_C_TA::value  ));
            ASSERT((5u == TS_C_CTA::value ));
            ASSERT((5u == TS_C_VTA::value ));
            ASSERT((5u == TS_V_TA::value  ));
            ASSERT((5u == TS_V_CTA::value ));
            ASSERT((5u == TS_V_VTA::value ));
            ASSERT((5u == TS_CV_TA::value ));
            ASSERT((5u == TS_CV_CTA::value));
            ASSERT((5u == TS_CV_VTA::value));
        }

        // Testing 'tuple_element'.
        {
            typedef std::tuple_element< 0 ,     TA > TE0_TA    ;
            typedef std::tuple_element< 0 ,    CTA > TE0_CTA   ;
            typedef std::tuple_element< 0 ,    VTA > TE0_VTA   ;
            typedef std::tuple_element< 1u,     TA > TE1_TA    ;
            typedef std::tuple_element< 1u,    CTA > TE1_CTA   ;
            typedef std::tuple_element< 1u,    VTA > TE1_VTA   ;
            typedef std::tuple_element< 2u,     TA > TE2_TA    ;
            typedef std::tuple_element< 2u,    CTA > TE2_CTA   ;
            typedef std::tuple_element< 2u,    VTA > TE2_VTA   ;
            typedef std::tuple_element< 3u,     TA > TE3_TA    ;
            typedef std::tuple_element< 3u,    CTA > TE3_CTA   ;
            typedef std::tuple_element< 3u,    VTA > TE3_VTA   ;
            typedef std::tuple_element< 4u,     TA > TE4_TA    ;
            typedef std::tuple_element< 4u,    CTA > TE4_CTA   ;
            typedef std::tuple_element< 4u,    VTA > TE4_VTA   ;

            typedef std::tuple_element< 0 ,  C_TA  > TE0_C_TA  ;
            typedef std::tuple_element< 0 ,  C_CTA > TE0_C_CTA ;
            typedef std::tuple_element< 0 ,  C_VTA > TE0_C_VTA ;
            typedef std::tuple_element< 1u,  C_TA  > TE1_C_TA  ;
            typedef std::tuple_element< 1u,  C_CTA > TE1_C_CTA ;
            typedef std::tuple_element< 1u,  C_VTA > TE1_C_VTA ;
            typedef std::tuple_element< 2u,  C_TA  > TE2_C_TA  ;
            typedef std::tuple_element< 2u,  C_CTA > TE2_C_CTA ;
            typedef std::tuple_element< 2u,  C_VTA > TE2_C_VTA ;
            typedef std::tuple_element< 3u,  C_TA  > TE3_C_TA  ;
            typedef std::tuple_element< 3u,  C_CTA > TE3_C_CTA ;
            typedef std::tuple_element< 3u,  C_VTA > TE3_C_VTA ;
            typedef std::tuple_element< 4u,  C_TA  > TE4_C_TA  ;
            typedef std::tuple_element< 4u,  C_CTA > TE4_C_CTA ;
            typedef std::tuple_element< 4u,  C_VTA > TE4_C_VTA ;

            typedef std::tuple_element< 0 ,  V_TA  > TE0_V_TA  ;
            typedef std::tuple_element< 0 ,  V_CTA > TE0_V_CTA ;
            typedef std::tuple_element< 0 ,  V_VTA > TE0_V_VTA ;
            typedef std::tuple_element< 1u,  V_TA  > TE1_V_TA  ;
            typedef std::tuple_element< 1u,  V_CTA > TE1_V_CTA ;
            typedef std::tuple_element< 1u,  V_VTA > TE1_V_VTA ;
            typedef std::tuple_element< 2u,  V_TA  > TE2_V_TA  ;
            typedef std::tuple_element< 2u,  V_CTA > TE2_V_CTA ;
            typedef std::tuple_element< 2u,  V_VTA > TE2_V_VTA ;
            typedef std::tuple_element< 3u,  V_TA  > TE3_V_TA  ;
            typedef std::tuple_element< 3u,  V_CTA > TE3_V_CTA ;
            typedef std::tuple_element< 3u,  V_VTA > TE3_V_VTA ;
            typedef std::tuple_element< 4u,  V_TA  > TE4_V_TA  ;
            typedef std::tuple_element< 4u,  V_CTA > TE4_V_CTA ;
            typedef std::tuple_element< 4u,  V_VTA > TE4_V_VTA ;

            typedef std::tuple_element< 0 , CV_TA  > TE0_CV_TA ;
            typedef std::tuple_element< 0 , CV_CTA > TE0_CV_CTA;
            typedef std::tuple_element< 0 , CV_VTA > TE0_CV_VTA;
            typedef std::tuple_element< 1u, CV_TA  > TE1_CV_TA ;
            typedef std::tuple_element< 1u, CV_CTA > TE1_CV_CTA;
            typedef std::tuple_element< 1u, CV_VTA > TE1_CV_VTA;
            typedef std::tuple_element< 2u, CV_TA  > TE2_CV_TA ;
            typedef std::tuple_element< 2u, CV_CTA > TE2_CV_CTA;
            typedef std::tuple_element< 2u, CV_VTA > TE2_CV_VTA;
            typedef std::tuple_element< 3u, CV_TA  > TE3_CV_TA ;
            typedef std::tuple_element< 3u, CV_CTA > TE3_CV_CTA;
            typedef std::tuple_element< 3u, CV_VTA > TE3_CV_VTA;
            typedef std::tuple_element< 4u, CV_TA  > TE4_CV_TA ;
            typedef std::tuple_element< 4u, CV_CTA > TE4_CV_CTA;
            typedef std::tuple_element< 4u, CV_VTA > TE4_CV_VTA;

            // Aliases for returned types.

            typedef                TYPE    T;
            typedef const          TYPE  C_T;
            typedef       volatile TYPE  V_T;
            typedef const volatile TYPE CV_T;

            ASSERT((bsl::is_same<   T,  typename TE0_TA::type     >::value));
            ASSERT((bsl::is_same< C_T,  typename TE0_CTA::type    >::value));
            ASSERT((bsl::is_same< V_T,  typename TE0_VTA::type    >::value));
            ASSERT((bsl::is_same<   T,  typename TE1_TA::type     >::value));
            ASSERT((bsl::is_same< C_T,  typename TE1_CTA::type    >::value));
            ASSERT((bsl::is_same< V_T,  typename TE1_VTA::type    >::value));
            ASSERT((bsl::is_same<   T,  typename TE2_TA::type     >::value));
            ASSERT((bsl::is_same< C_T,  typename TE2_CTA::type    >::value));
            ASSERT((bsl::is_same< V_T,  typename TE2_VTA::type    >::value));
            ASSERT((bsl::is_same<   T,  typename TE3_TA::type     >::value));
            ASSERT((bsl::is_same< C_T,  typename TE3_CTA::type    >::value));
            ASSERT((bsl::is_same< V_T,  typename TE3_VTA::type    >::value));
            ASSERT((bsl::is_same<   T,  typename TE4_TA::type     >::value));
            ASSERT((bsl::is_same< C_T,  typename TE4_CTA::type    >::value));
            ASSERT((bsl::is_same< V_T,  typename TE4_VTA::type    >::value));

            ASSERT((bsl::is_same<  C_T, typename TE0_C_TA::type   >::value));
            ASSERT((bsl::is_same<  C_T, typename TE0_C_CTA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_C_VTA::type  >::value));
            ASSERT((bsl::is_same<  C_T, typename TE1_C_TA::type   >::value));
            ASSERT((bsl::is_same<  C_T, typename TE1_C_CTA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_C_VTA::type  >::value));
            ASSERT((bsl::is_same<  C_T, typename TE2_C_TA::type   >::value));
            ASSERT((bsl::is_same<  C_T, typename TE2_C_CTA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE2_C_VTA::type  >::value));
            ASSERT((bsl::is_same<  C_T, typename TE3_C_TA::type   >::value));
            ASSERT((bsl::is_same<  C_T, typename TE3_C_CTA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE3_C_VTA::type  >::value));
            ASSERT((bsl::is_same<  C_T, typename TE4_C_TA::type   >::value));
            ASSERT((bsl::is_same<  C_T, typename TE4_C_CTA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE4_C_VTA::type  >::value));

            ASSERT((bsl::is_same<  V_T, typename TE0_V_TA::type   >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_V_CTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE0_V_VTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE1_V_TA::type   >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_V_CTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE1_V_VTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE2_V_TA::type   >::value));
            ASSERT((bsl::is_same< CV_T, typename TE2_V_CTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE2_V_VTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE3_V_TA::type   >::value));
            ASSERT((bsl::is_same< CV_T, typename TE3_V_CTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE3_V_VTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE4_V_TA::type   >::value));
            ASSERT((bsl::is_same< CV_T, typename TE4_V_CTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE4_V_VTA::type  >::value));

            ASSERT((bsl::is_same< CV_T, typename TE0_CV_TA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_CV_CTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_CV_VTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_CV_TA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_CV_CTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_CV_VTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE2_CV_TA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE2_CV_CTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE2_CV_VTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE3_CV_TA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE3_CV_CTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE3_CV_VTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE4_CV_TA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE4_CV_CTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE4_CV_VTA::type >::value));
        }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE

        // Testing 'TYPE& get(array&)' and 'const TYPE& get(const array&)'.
        {
            TA        mX;
            const TA& X = mX;

            ASSERT(true          ==         isRefConstant(bsl::get<0>( X)));
            ASSERT(true          ==         isRefConstant(bsl::get<1>( X)));
            ASSERT(true          ==         isRefConstant(bsl::get<2>( X)));
            ASSERT(true          ==         isRefConstant(bsl::get<3>( X)));
            ASSERT(true          ==         isRefConstant(bsl::get<4>( X)));

            ASSERT( X.data() + 0 == bsls::Util::addressOf(bsl::get<0>( X)));
            ASSERT( X.data() + 1 == bsls::Util::addressOf(bsl::get<1>( X)));
            ASSERT( X.data() + 2 == bsls::Util::addressOf(bsl::get<2>( X)));
            ASSERT( X.data() + 3 == bsls::Util::addressOf(bsl::get<3>( X)));
            ASSERT( X.data() + 4 == bsls::Util::addressOf(bsl::get<4>( X)));

            ASSERT(false         ==         isRefConstant(bsl::get<0>(mX)));
            ASSERT(false         ==         isRefConstant(bsl::get<1>(mX)));
            ASSERT(false         ==         isRefConstant(bsl::get<2>(mX)));
            ASSERT(false         ==         isRefConstant(bsl::get<3>(mX)));
            ASSERT(false         ==         isRefConstant(bsl::get<4>(mX)));

            ASSERT(mX.data() + 0 == bsls::Util::addressOf(bsl::get<0>(mX)));
            ASSERT(mX.data() + 1 == bsls::Util::addressOf(bsl::get<1>(mX)));
            ASSERT(mX.data() + 2 == bsls::Util::addressOf(bsl::get<2>(mX)));
            ASSERT(mX.data() + 3 == bsls::Util::addressOf(bsl::get<3>(mX)));
            ASSERT(mX.data() + 4 == bsls::Util::addressOf(bsl::get<4>(mX)));
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // Testing 'TYPE&& get(array&&)' and 'const TYPE&& get(const array&&)'.
        {
            typedef bslmf::MovableRefUtil MoveUtil;

            const char* SPEC = "ABCDEFGHIJKLMNOPQRSTUV";

            TA mW; const  TA& W  = gg(&mW,  SPEC);

            TA mX0; const TA& X0 = gg(&mX0, SPEC);
            TA mX1; const TA& X1 = gg(&mX1, SPEC);
            TA mX2; const TA& X2 = gg(&mX2, SPEC);
            TA mX3; const TA& X3 = gg(&mX3, SPEC);
            TA mX4; const TA& X4 = gg(&mX4, SPEC);

            ASSERT(true  == isRefConstant(bsl::get<0>(MoveUtil::move( X0))));
            ASSERT(true  == isRefConstant(bsl::get<1>(MoveUtil::move( X1))));
            ASSERT(true  == isRefConstant(bsl::get<2>(MoveUtil::move( X2))));
            ASSERT(true  == isRefConstant(bsl::get<3>(MoveUtil::move( X3))));
            ASSERT(true  == isRefConstant(bsl::get<4>(MoveUtil::move( X4))));

            ASSERT(W[0]  ==               bsl::get<0>(MoveUtil::move( X0)));
            ASSERT(W[1]  ==               bsl::get<1>(MoveUtil::move( X1)));
            ASSERT(W[2]  ==               bsl::get<2>(MoveUtil::move( X2)));
            ASSERT(W[3]  ==               bsl::get<3>(MoveUtil::move( X3)));
            ASSERT(W[4]  ==               bsl::get<4>(MoveUtil::move( X4)));

            ASSERT(false == isRefConstant(bsl::get<0>(MoveUtil::move(mX0))));
            ASSERT(false == isRefConstant(bsl::get<1>(MoveUtil::move(mX1))));
            ASSERT(false == isRefConstant(bsl::get<2>(MoveUtil::move(mX2))));
            ASSERT(false == isRefConstant(bsl::get<3>(MoveUtil::move(mX3))));
            ASSERT(false == isRefConstant(bsl::get<4>(MoveUtil::move(mX4))));

            ASSERT(W[0] ==                bsl::get<0>(MoveUtil::move(mX0)));
            ASSERT(W[1] ==                bsl::get<1>(MoveUtil::move(mX1)));
            ASSERT(W[2] ==                bsl::get<2>(MoveUtil::move(mX2)));
            ASSERT(W[3] ==                bsl::get<3>(MoveUtil::move(mX3)));
            ASSERT(W[4] ==                bsl::get<4>(MoveUtil::move(mX4)));
        }
#endif
    }
};

template<class TYPE>
struct TupleApiTest<TYPE, 4>
    // This specialization of 'TupleApiTest' provides a namespace for utility
    // function that test tuple-API for bsl::arrays of size 4.
{
    static void testTupleApi()
    {
        typedef                bsl::array<         TYPE, 4>     TA ;

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
        typedef                bsl::array<const    TYPE, 4>     CTA;
        typedef                bsl::array<volatile TYPE, 4>     VTA;

        typedef const          bsl::array<         TYPE, 4>   C_TA ;
        typedef const          bsl::array<const    TYPE, 4>   C_CTA;
        typedef const          bsl::array<volatile TYPE, 4>   C_VTA;

        typedef       volatile bsl::array<         TYPE, 4>   V_TA ;
        typedef       volatile bsl::array<const    TYPE, 4>   V_CTA;
        typedef       volatile bsl::array<volatile TYPE, 4>   V_VTA;

        typedef const volatile bsl::array<         TYPE, 4>  CV_TA ;
        typedef const volatile bsl::array<const    TYPE, 4>  CV_CTA;
        typedef const volatile bsl::array<volatile TYPE, 4>  CV_VTA;

        // Testing 'tuple_size'.
        {
            typedef std::tuple_size< TA     > TS_TA    ;
            typedef std::tuple_size< CTA    > TS_CTA   ;
            typedef std::tuple_size< VTA    > TS_VTA   ;
            typedef std::tuple_size< C_TA   > TS_C_TA  ;
            typedef std::tuple_size< C_CTA  > TS_C_CTA ;
            typedef std::tuple_size< C_VTA  > TS_C_VTA ;
            typedef std::tuple_size< V_TA   > TS_V_TA  ;
            typedef std::tuple_size< V_CTA  > TS_V_CTA ;
            typedef std::tuple_size< V_VTA  > TS_V_VTA ;
            typedef std::tuple_size< CV_TA  > TS_CV_TA ;
            typedef std::tuple_size< CV_CTA > TS_CV_CTA;
            typedef std::tuple_size< CV_VTA > TS_CV_VTA;

            ASSERT((4u == TS_TA::value    ));
            ASSERT((4u == TS_CTA::value   ));
            ASSERT((4u == TS_VTA::value   ));
            ASSERT((4u == TS_C_TA::value  ));
            ASSERT((4u == TS_C_CTA::value ));
            ASSERT((4u == TS_C_VTA::value ));
            ASSERT((4u == TS_V_TA::value  ));
            ASSERT((4u == TS_V_CTA::value ));
            ASSERT((4u == TS_V_VTA::value ));
            ASSERT((4u == TS_CV_TA::value ));
            ASSERT((4u == TS_CV_CTA::value));
            ASSERT((4u == TS_CV_VTA::value));
        }

        // Testing 'tuple_element'.
        {
            typedef std::tuple_element< 0 ,     TA > TE0_TA    ;
            typedef std::tuple_element< 0 ,    CTA > TE0_CTA   ;
            typedef std::tuple_element< 0 ,    VTA > TE0_VTA   ;
            typedef std::tuple_element< 1u,     TA > TE1_TA    ;
            typedef std::tuple_element< 1u,    CTA > TE1_CTA   ;
            typedef std::tuple_element< 1u,    VTA > TE1_VTA   ;
            typedef std::tuple_element< 2u,     TA > TE2_TA    ;
            typedef std::tuple_element< 2u,    CTA > TE2_CTA   ;
            typedef std::tuple_element< 2u,    VTA > TE2_VTA   ;
            typedef std::tuple_element< 3u,     TA > TE3_TA    ;
            typedef std::tuple_element< 3u,    CTA > TE3_CTA   ;
            typedef std::tuple_element< 3u,    VTA > TE3_VTA   ;

            typedef std::tuple_element< 0 ,  C_TA  > TE0_C_TA  ;
            typedef std::tuple_element< 0 ,  C_CTA > TE0_C_CTA ;
            typedef std::tuple_element< 0 ,  C_VTA > TE0_C_VTA ;
            typedef std::tuple_element< 1u,  C_TA  > TE1_C_TA  ;
            typedef std::tuple_element< 1u,  C_CTA > TE1_C_CTA ;
            typedef std::tuple_element< 1u,  C_VTA > TE1_C_VTA ;
            typedef std::tuple_element< 2u,  C_TA  > TE2_C_TA  ;
            typedef std::tuple_element< 2u,  C_CTA > TE2_C_CTA ;
            typedef std::tuple_element< 2u,  C_VTA > TE2_C_VTA ;
            typedef std::tuple_element< 3u,  C_TA  > TE3_C_TA  ;
            typedef std::tuple_element< 3u,  C_CTA > TE3_C_CTA ;
            typedef std::tuple_element< 3u,  C_VTA > TE3_C_VTA ;

            typedef std::tuple_element< 0 ,  V_TA  > TE0_V_TA  ;
            typedef std::tuple_element< 0 ,  V_CTA > TE0_V_CTA ;
            typedef std::tuple_element< 0 ,  V_VTA > TE0_V_VTA ;
            typedef std::tuple_element< 1u,  V_TA  > TE1_V_TA  ;
            typedef std::tuple_element< 1u,  V_CTA > TE1_V_CTA ;
            typedef std::tuple_element< 1u,  V_VTA > TE1_V_VTA ;
            typedef std::tuple_element< 2u,  V_TA  > TE2_V_TA  ;
            typedef std::tuple_element< 2u,  V_CTA > TE2_V_CTA ;
            typedef std::tuple_element< 2u,  V_VTA > TE2_V_VTA ;
            typedef std::tuple_element< 3u,  V_TA  > TE3_V_TA  ;
            typedef std::tuple_element< 3u,  V_CTA > TE3_V_CTA ;
            typedef std::tuple_element< 3u,  V_VTA > TE3_V_VTA ;

            typedef std::tuple_element< 0 , CV_TA  > TE0_CV_TA ;
            typedef std::tuple_element< 0 , CV_CTA > TE0_CV_CTA;
            typedef std::tuple_element< 0 , CV_VTA > TE0_CV_VTA;
            typedef std::tuple_element< 1u, CV_TA  > TE1_CV_TA ;
            typedef std::tuple_element< 1u, CV_CTA > TE1_CV_CTA;
            typedef std::tuple_element< 1u, CV_VTA > TE1_CV_VTA;
            typedef std::tuple_element< 2u, CV_TA  > TE2_CV_TA ;
            typedef std::tuple_element< 2u, CV_CTA > TE2_CV_CTA;
            typedef std::tuple_element< 2u, CV_VTA > TE2_CV_VTA;
            typedef std::tuple_element< 3u, CV_TA  > TE3_CV_TA ;
            typedef std::tuple_element< 3u, CV_CTA > TE3_CV_CTA;
            typedef std::tuple_element< 3u, CV_VTA > TE3_CV_VTA;

            // Aliases for returned types.

            typedef                TYPE    T;
            typedef const          TYPE  C_T;
            typedef       volatile TYPE  V_T;
            typedef const volatile TYPE CV_T;

            ASSERT((bsl::is_same<   T,  typename TE0_TA::type     >::value));
            ASSERT((bsl::is_same< C_T,  typename TE0_CTA::type    >::value));
            ASSERT((bsl::is_same< V_T,  typename TE0_VTA::type    >::value));
            ASSERT((bsl::is_same<   T,  typename TE1_TA::type     >::value));
            ASSERT((bsl::is_same< C_T,  typename TE1_CTA::type    >::value));
            ASSERT((bsl::is_same< V_T,  typename TE1_VTA::type    >::value));
            ASSERT((bsl::is_same<   T,  typename TE2_TA::type     >::value));
            ASSERT((bsl::is_same< C_T,  typename TE2_CTA::type    >::value));
            ASSERT((bsl::is_same< V_T,  typename TE2_VTA::type    >::value));
            ASSERT((bsl::is_same<   T,  typename TE3_TA::type     >::value));
            ASSERT((bsl::is_same< C_T,  typename TE3_CTA::type    >::value));
            ASSERT((bsl::is_same< V_T,  typename TE3_VTA::type    >::value));

            ASSERT((bsl::is_same<  C_T, typename TE0_C_TA::type   >::value));
            ASSERT((bsl::is_same<  C_T, typename TE0_C_CTA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_C_VTA::type  >::value));
            ASSERT((bsl::is_same<  C_T, typename TE1_C_TA::type   >::value));
            ASSERT((bsl::is_same<  C_T, typename TE1_C_CTA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_C_VTA::type  >::value));
            ASSERT((bsl::is_same<  C_T, typename TE2_C_TA::type   >::value));
            ASSERT((bsl::is_same<  C_T, typename TE2_C_CTA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE2_C_VTA::type  >::value));
            ASSERT((bsl::is_same<  C_T, typename TE3_C_TA::type   >::value));
            ASSERT((bsl::is_same<  C_T, typename TE3_C_CTA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE3_C_VTA::type  >::value));

            ASSERT((bsl::is_same<  V_T, typename TE0_V_TA::type   >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_V_CTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE0_V_VTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE1_V_TA::type   >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_V_CTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE1_V_VTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE2_V_TA::type   >::value));
            ASSERT((bsl::is_same< CV_T, typename TE2_V_CTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE2_V_VTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE3_V_TA::type   >::value));
            ASSERT((bsl::is_same< CV_T, typename TE3_V_CTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE3_V_VTA::type  >::value));

            ASSERT((bsl::is_same< CV_T, typename TE0_CV_TA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_CV_CTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_CV_VTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_CV_TA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_CV_CTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_CV_VTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE2_CV_TA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE2_CV_CTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE2_CV_VTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE3_CV_TA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE3_CV_CTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE3_CV_VTA::type >::value));
        }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE

        // Testing 'TYPE& get(array&)' and 'const TYPE& get(const array&)'.
        {
            TA        mX;
            const TA& X = mX;

            ASSERT(mX.data() + 0 == bsls::Util::addressOf(bsl::get<0>(mX)));
            ASSERT(mX.data() + 1 == bsls::Util::addressOf(bsl::get<1>(mX)));
            ASSERT(mX.data() + 2 == bsls::Util::addressOf(bsl::get<2>(mX)));
            ASSERT(mX.data() + 3 == bsls::Util::addressOf(bsl::get<3>(mX)));

            ASSERT( X.data() + 0 == bsls::Util::addressOf(bsl::get<0>( X)));
            ASSERT( X.data() + 1 == bsls::Util::addressOf(bsl::get<1>( X)));
            ASSERT( X.data() + 2 == bsls::Util::addressOf(bsl::get<2>( X)));
            ASSERT( X.data() + 3 == bsls::Util::addressOf(bsl::get<3>( X)));

            ASSERT(false         ==         isRefConstant(bsl::get<0>(mX)));
            ASSERT(false         ==         isRefConstant(bsl::get<1>(mX)));
            ASSERT(false         ==         isRefConstant(bsl::get<2>(mX)));
            ASSERT(false         ==         isRefConstant(bsl::get<3>(mX)));

            ASSERT(true          ==         isRefConstant(bsl::get<0>( X)));
            ASSERT(true          ==         isRefConstant(bsl::get<1>( X)));
            ASSERT(true          ==         isRefConstant(bsl::get<2>( X)));
            ASSERT(true          ==         isRefConstant(bsl::get<3>( X)));
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // Testing 'TYPE&& get(array&&)' and 'const TYPE&& get(const array&&)'.
        {
            typedef bslmf::MovableRefUtil MoveUtil;

            const char* SPEC = "ABCDEFGHIJKLMNOPQRSTUV";

            TA mW; const  TA& W  = gg(&mW,  SPEC);

            TA mX0; const TA& X0 = gg(&mX0, SPEC);
            TA mX1; const TA& X1 = gg(&mX1, SPEC);
            TA mX2; const TA& X2 = gg(&mX2, SPEC);
            TA mX3; const TA& X3 = gg(&mX3, SPEC);

            ASSERT(true  == isRefConstant(bsl::get<0>(MoveUtil::move( X0))));
            ASSERT(true  == isRefConstant(bsl::get<1>(MoveUtil::move( X1))));
            ASSERT(true  == isRefConstant(bsl::get<2>(MoveUtil::move( X2))));
            ASSERT(true  == isRefConstant(bsl::get<3>(MoveUtil::move( X3))));

            ASSERT(W[0]  ==               bsl::get<0>(MoveUtil::move( X0)));
            ASSERT(W[1]  ==               bsl::get<1>(MoveUtil::move( X1)));
            ASSERT(W[2]  ==               bsl::get<2>(MoveUtil::move( X2)));
            ASSERT(W[3]  ==               bsl::get<3>(MoveUtil::move( X3)));

            ASSERT(false == isRefConstant(bsl::get<0>(MoveUtil::move(mX0))));
            ASSERT(false == isRefConstant(bsl::get<1>(MoveUtil::move(mX1))));
            ASSERT(false == isRefConstant(bsl::get<2>(MoveUtil::move(mX2))));
            ASSERT(false == isRefConstant(bsl::get<3>(MoveUtil::move(mX3))));

            ASSERT(W[0] ==                bsl::get<0>(MoveUtil::move(mX0)));
            ASSERT(W[1] ==                bsl::get<1>(MoveUtil::move(mX1)));
            ASSERT(W[2] ==                bsl::get<2>(MoveUtil::move(mX2)));
            ASSERT(W[3] ==                bsl::get<3>(MoveUtil::move(mX3)));
        }
#endif
    }
};

template<class TYPE>
struct TupleApiTest<TYPE, 3>
    // This specialization of 'TupleApiTest' provides a namespace for utility
    // function that test tuple-API for bsl::arrays of size 3.
{
    static void testTupleApi()
    {
        typedef                bsl::array<         TYPE, 3>     TA ;

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
        typedef                bsl::array<const    TYPE, 3>     CTA;
        typedef                bsl::array<volatile TYPE, 3>     VTA;

        typedef const          bsl::array<         TYPE, 3>   C_TA ;
        typedef const          bsl::array<const    TYPE, 3>   C_CTA;
        typedef const          bsl::array<volatile TYPE, 3>   C_VTA;

        typedef       volatile bsl::array<         TYPE, 3>   V_TA ;
        typedef       volatile bsl::array<const    TYPE, 3>   V_CTA;
        typedef       volatile bsl::array<volatile TYPE, 3>   V_VTA;

        typedef const volatile bsl::array<         TYPE, 3>  CV_TA ;
        typedef const volatile bsl::array<const    TYPE, 3>  CV_CTA;
        typedef const volatile bsl::array<volatile TYPE, 3>  CV_VTA;

        // Testing 'tuple_size'.
        {
            typedef std::tuple_size< TA     > TS_TA    ;
            typedef std::tuple_size< CTA    > TS_CTA   ;
            typedef std::tuple_size< VTA    > TS_VTA   ;
            typedef std::tuple_size< C_TA   > TS_C_TA  ;
            typedef std::tuple_size< C_CTA  > TS_C_CTA ;
            typedef std::tuple_size< C_VTA  > TS_C_VTA ;
            typedef std::tuple_size< V_TA   > TS_V_TA  ;
            typedef std::tuple_size< V_CTA  > TS_V_CTA ;
            typedef std::tuple_size< V_VTA  > TS_V_VTA ;
            typedef std::tuple_size< CV_TA  > TS_CV_TA ;
            typedef std::tuple_size< CV_CTA > TS_CV_CTA;
            typedef std::tuple_size< CV_VTA > TS_CV_VTA;

            ASSERT((3u == TS_TA::value    ));
            ASSERT((3u == TS_CTA::value   ));
            ASSERT((3u == TS_VTA::value   ));
            ASSERT((3u == TS_C_TA::value  ));
            ASSERT((3u == TS_C_CTA::value ));
            ASSERT((3u == TS_C_VTA::value ));
            ASSERT((3u == TS_V_TA::value  ));
            ASSERT((3u == TS_V_CTA::value ));
            ASSERT((3u == TS_V_VTA::value ));
            ASSERT((3u == TS_CV_TA::value ));
            ASSERT((3u == TS_CV_CTA::value));
            ASSERT((3u == TS_CV_VTA::value));
        }
        // Testing 'tuple_element'.
        {
            typedef std::tuple_element< 0 ,     TA > TE0_TA    ;
            typedef std::tuple_element< 0 ,    CTA > TE0_CTA   ;
            typedef std::tuple_element< 0 ,    VTA > TE0_VTA   ;
            typedef std::tuple_element< 1u,     TA > TE1_TA    ;
            typedef std::tuple_element< 1u,    CTA > TE1_CTA   ;
            typedef std::tuple_element< 1u,    VTA > TE1_VTA   ;
            typedef std::tuple_element< 2u,     TA > TE2_TA    ;
            typedef std::tuple_element< 2u,    CTA > TE2_CTA   ;
            typedef std::tuple_element< 2u,    VTA > TE2_VTA   ;

            typedef std::tuple_element< 0 ,  C_TA  > TE0_C_TA  ;
            typedef std::tuple_element< 0 ,  C_CTA > TE0_C_CTA ;
            typedef std::tuple_element< 0 ,  C_VTA > TE0_C_VTA ;
            typedef std::tuple_element< 1u,  C_TA  > TE1_C_TA  ;
            typedef std::tuple_element< 1u,  C_CTA > TE1_C_CTA ;
            typedef std::tuple_element< 1u,  C_VTA > TE1_C_VTA ;
            typedef std::tuple_element< 2u,  C_TA  > TE2_C_TA  ;
            typedef std::tuple_element< 2u,  C_CTA > TE2_C_CTA ;
            typedef std::tuple_element< 2u,  C_VTA > TE2_C_VTA ;

            typedef std::tuple_element< 0 ,  V_TA  > TE0_V_TA  ;
            typedef std::tuple_element< 0 ,  V_CTA > TE0_V_CTA ;
            typedef std::tuple_element< 0 ,  V_VTA > TE0_V_VTA ;
            typedef std::tuple_element< 1u,  V_TA  > TE1_V_TA  ;
            typedef std::tuple_element< 1u,  V_CTA > TE1_V_CTA ;
            typedef std::tuple_element< 1u,  V_VTA > TE1_V_VTA ;
            typedef std::tuple_element< 2u,  V_TA  > TE2_V_TA  ;
            typedef std::tuple_element< 2u,  V_CTA > TE2_V_CTA ;
            typedef std::tuple_element< 2u,  V_VTA > TE2_V_VTA ;

            typedef std::tuple_element< 0 , CV_TA  > TE0_CV_TA ;
            typedef std::tuple_element< 0 , CV_CTA > TE0_CV_CTA;
            typedef std::tuple_element< 0 , CV_VTA > TE0_CV_VTA;
            typedef std::tuple_element< 1u, CV_TA  > TE1_CV_TA ;
            typedef std::tuple_element< 1u, CV_CTA > TE1_CV_CTA;
            typedef std::tuple_element< 1u, CV_VTA > TE1_CV_VTA;
            typedef std::tuple_element< 2u, CV_TA  > TE2_CV_TA ;
            typedef std::tuple_element< 2u, CV_CTA > TE2_CV_CTA;
            typedef std::tuple_element< 2u, CV_VTA > TE2_CV_VTA;

            // Aliases for returned types.

            typedef                TYPE    T;
            typedef const          TYPE  C_T;
            typedef       volatile TYPE  V_T;
            typedef const volatile TYPE CV_T;

            ASSERT((bsl::is_same<   T,  typename TE0_TA::type     >::value));
            ASSERT((bsl::is_same< C_T,  typename TE0_CTA::type    >::value));
            ASSERT((bsl::is_same< V_T,  typename TE0_VTA::type    >::value));
            ASSERT((bsl::is_same<   T,  typename TE1_TA::type     >::value));
            ASSERT((bsl::is_same< C_T,  typename TE1_CTA::type    >::value));
            ASSERT((bsl::is_same< V_T,  typename TE1_VTA::type    >::value));
            ASSERT((bsl::is_same<   T,  typename TE2_TA::type     >::value));
            ASSERT((bsl::is_same< C_T,  typename TE2_CTA::type    >::value));
            ASSERT((bsl::is_same< V_T,  typename TE2_VTA::type    >::value));

            ASSERT((bsl::is_same<  C_T, typename TE0_C_TA::type   >::value));
            ASSERT((bsl::is_same<  C_T, typename TE0_C_CTA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_C_VTA::type  >::value));
            ASSERT((bsl::is_same<  C_T, typename TE1_C_TA::type   >::value));
            ASSERT((bsl::is_same<  C_T, typename TE1_C_CTA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_C_VTA::type  >::value));
            ASSERT((bsl::is_same<  C_T, typename TE2_C_TA::type   >::value));
            ASSERT((bsl::is_same<  C_T, typename TE2_C_CTA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE2_C_VTA::type  >::value));

            ASSERT((bsl::is_same<  V_T, typename TE0_V_TA::type   >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_V_CTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE0_V_VTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE1_V_TA::type   >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_V_CTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE1_V_VTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE2_V_TA::type   >::value));
            ASSERT((bsl::is_same< CV_T, typename TE2_V_CTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE2_V_VTA::type  >::value));

            ASSERT((bsl::is_same< CV_T, typename TE0_CV_TA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_CV_CTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_CV_VTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_CV_TA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_CV_CTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_CV_VTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE2_CV_TA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE2_CV_CTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE2_CV_VTA::type >::value));
        }
#endif // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE

        // Testing 'TYPE& get(array&)' and 'const TYPE& get(const array&)'.
        {
            TA        mX;
            const TA& X = mX;

            ASSERT(mX.data() + 0 == bsls::Util::addressOf(bsl::get<0>(mX)));
            ASSERT(mX.data() + 1 == bsls::Util::addressOf(bsl::get<1>(mX)));
            ASSERT(mX.data() + 2 == bsls::Util::addressOf(bsl::get<2>(mX)));

            ASSERT( X.data() + 0 == bsls::Util::addressOf(bsl::get<0>( X)));
            ASSERT( X.data() + 1 == bsls::Util::addressOf(bsl::get<1>( X)));
            ASSERT( X.data() + 2 == bsls::Util::addressOf(bsl::get<2>( X)));

            ASSERT(false         ==         isRefConstant(bsl::get<0>(mX)));
            ASSERT(false         ==         isRefConstant(bsl::get<1>(mX)));
            ASSERT(false         ==         isRefConstant(bsl::get<2>(mX)));

            ASSERT(true          ==         isRefConstant(bsl::get<0>( X)));
            ASSERT(true          ==         isRefConstant(bsl::get<1>( X)));
            ASSERT(true          ==         isRefConstant(bsl::get<2>( X)));
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // Testing 'TYPE&& get(array&&)' and 'const TYPE&& get(const array&&)'.
        {
            typedef bslmf::MovableRefUtil MoveUtil;

            const char* SPEC = "ABCDEFGHIJKLMNOPQRSTUV";

            TA mW; const  TA& W  = gg(&mW,  SPEC);

            TA mX0; const TA& X0 = gg(&mX0, SPEC);
            TA mX1; const TA& X1 = gg(&mX1, SPEC);
            TA mX2; const TA& X2 = gg(&mX2, SPEC);

            ASSERT(true  == isRefConstant(bsl::get<0>(MoveUtil::move( X0))));
            ASSERT(true  == isRefConstant(bsl::get<1>(MoveUtil::move( X1))));
            ASSERT(true  == isRefConstant(bsl::get<2>(MoveUtil::move( X2))));

            ASSERT(W[0]  ==               bsl::get<0>(MoveUtil::move( X0)));
            ASSERT(W[1]  ==               bsl::get<1>(MoveUtil::move( X1)));
            ASSERT(W[2]  ==               bsl::get<2>(MoveUtil::move( X2)));

            ASSERT(false == isRefConstant(bsl::get<0>(MoveUtil::move(mX0))));
            ASSERT(false == isRefConstant(bsl::get<1>(MoveUtil::move(mX1))));
            ASSERT(false == isRefConstant(bsl::get<2>(MoveUtil::move(mX2))));

            ASSERT(W[0] ==                bsl::get<0>(MoveUtil::move(mX0)));
            ASSERT(W[1] ==                bsl::get<1>(MoveUtil::move(mX1)));
            ASSERT(W[2] ==                bsl::get<2>(MoveUtil::move(mX2)));
        }
#endif
    }
};

template<class TYPE>
struct TupleApiTest<TYPE, 2>
    // This specialization of 'TupleApiTest' provides a namespace for utility
    // function that test tuple-API for bsl::arrays of size 2.
{
    static void testTupleApi()
    {
        typedef                bsl::array<         TYPE, 2>     TA ;

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
        typedef                bsl::array<const    TYPE, 2>     CTA;
        typedef                bsl::array<volatile TYPE, 2>     VTA;

        typedef const          bsl::array<         TYPE, 2>   C_TA ;
        typedef const          bsl::array<const    TYPE, 2>   C_CTA;
        typedef const          bsl::array<volatile TYPE, 2>   C_VTA;

        typedef       volatile bsl::array<         TYPE, 2>   V_TA ;
        typedef       volatile bsl::array<const    TYPE, 2>   V_CTA;
        typedef       volatile bsl::array<volatile TYPE, 2>   V_VTA;

        typedef const volatile bsl::array<         TYPE, 2>  CV_TA ;
        typedef const volatile bsl::array<const    TYPE, 2>  CV_CTA;
        typedef const volatile bsl::array<volatile TYPE, 2>  CV_VTA;

        // Testing 'tuple_size'.
        {
            typedef std::tuple_size< TA     > TS_TA    ;
            typedef std::tuple_size< CTA    > TS_CTA   ;
            typedef std::tuple_size< VTA    > TS_VTA   ;
            typedef std::tuple_size< C_TA   > TS_C_TA  ;
            typedef std::tuple_size< C_CTA  > TS_C_CTA ;
            typedef std::tuple_size< C_VTA  > TS_C_VTA ;
            typedef std::tuple_size< V_TA   > TS_V_TA  ;
            typedef std::tuple_size< V_CTA  > TS_V_CTA ;
            typedef std::tuple_size< V_VTA  > TS_V_VTA ;
            typedef std::tuple_size< CV_TA  > TS_CV_TA ;
            typedef std::tuple_size< CV_CTA > TS_CV_CTA;
            typedef std::tuple_size< CV_VTA > TS_CV_VTA;

            ASSERT((2u == TS_TA::value    ));
            ASSERT((2u == TS_CTA::value   ));
            ASSERT((2u == TS_VTA::value   ));
            ASSERT((2u == TS_C_TA::value  ));
            ASSERT((2u == TS_C_CTA::value ));
            ASSERT((2u == TS_C_VTA::value ));
            ASSERT((2u == TS_V_TA::value  ));
            ASSERT((2u == TS_V_CTA::value ));
            ASSERT((2u == TS_V_VTA::value ));
            ASSERT((2u == TS_CV_TA::value ));
            ASSERT((2u == TS_CV_CTA::value));
            ASSERT((2u == TS_CV_VTA::value));
        }

        // Testing 'tuple_element'.
        {
            typedef std::tuple_element< 0 ,     TA > TE0_TA    ;
            typedef std::tuple_element< 0 ,    CTA > TE0_CTA   ;
            typedef std::tuple_element< 0 ,    VTA > TE0_VTA   ;
            typedef std::tuple_element< 1u,     TA > TE1_TA    ;
            typedef std::tuple_element< 1u,    CTA > TE1_CTA   ;
            typedef std::tuple_element< 1u,    VTA > TE1_VTA   ;

            typedef std::tuple_element< 0 ,  C_TA  > TE0_C_TA  ;
            typedef std::tuple_element< 0 ,  C_CTA > TE0_C_CTA ;
            typedef std::tuple_element< 0 ,  C_VTA > TE0_C_VTA ;
            typedef std::tuple_element< 1u,  C_TA  > TE1_C_TA  ;
            typedef std::tuple_element< 1u,  C_CTA > TE1_C_CTA ;
            typedef std::tuple_element< 1u,  C_VTA > TE1_C_VTA ;

            typedef std::tuple_element< 0 ,  V_TA  > TE0_V_TA  ;
            typedef std::tuple_element< 0 ,  V_CTA > TE0_V_CTA ;
            typedef std::tuple_element< 0 ,  V_VTA > TE0_V_VTA ;
            typedef std::tuple_element< 1u,  V_TA  > TE1_V_TA  ;
            typedef std::tuple_element< 1u,  V_CTA > TE1_V_CTA ;
            typedef std::tuple_element< 1u,  V_VTA > TE1_V_VTA ;

            typedef std::tuple_element< 0 , CV_TA  > TE0_CV_TA ;
            typedef std::tuple_element< 0 , CV_CTA > TE0_CV_CTA;
            typedef std::tuple_element< 0 , CV_VTA > TE0_CV_VTA;
            typedef std::tuple_element< 1u, CV_TA  > TE1_CV_TA ;
            typedef std::tuple_element< 1u, CV_CTA > TE1_CV_CTA;
            typedef std::tuple_element< 1u, CV_VTA > TE1_CV_VTA;

            // Aliases for returned types.

            typedef                TYPE    T;
            typedef const          TYPE  C_T;
            typedef       volatile TYPE  V_T;
            typedef const volatile TYPE CV_T;

            ASSERT((bsl::is_same<   T,  typename TE0_TA::type     >::value));
            ASSERT((bsl::is_same< C_T,  typename TE0_CTA::type    >::value));
            ASSERT((bsl::is_same< V_T,  typename TE0_VTA::type    >::value));
            ASSERT((bsl::is_same<   T,  typename TE1_TA::type     >::value));
            ASSERT((bsl::is_same< C_T,  typename TE1_CTA::type    >::value));
            ASSERT((bsl::is_same< V_T,  typename TE1_VTA::type    >::value));

            ASSERT((bsl::is_same<  C_T, typename TE0_C_TA::type   >::value));
            ASSERT((bsl::is_same<  C_T, typename TE0_C_CTA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_C_VTA::type  >::value));
            ASSERT((bsl::is_same<  C_T, typename TE1_C_TA::type   >::value));
            ASSERT((bsl::is_same<  C_T, typename TE1_C_CTA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_C_VTA::type  >::value));

            ASSERT((bsl::is_same<  V_T, typename TE0_V_TA::type   >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_V_CTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE0_V_VTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE1_V_TA::type   >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_V_CTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE1_V_VTA::type  >::value));

            ASSERT((bsl::is_same< CV_T, typename TE0_CV_TA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_CV_CTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_CV_VTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_CV_TA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_CV_CTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE1_CV_VTA::type >::value));
        }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE

        // Testing 'TYPE& get(array&)' and 'const TYPE& get(const array&)'.
        {
            TA        mX;
            const TA& X = mX;

            ASSERT(mX.data() + 0 == bsls::Util::addressOf(bsl::get<0>(mX)));
            ASSERT(mX.data() + 1 == bsls::Util::addressOf(bsl::get<1>(mX)));

            ASSERT( X.data() + 0 == bsls::Util::addressOf(bsl::get<0>( X)));
            ASSERT( X.data() + 1 == bsls::Util::addressOf(bsl::get<1>( X)));

            ASSERT(false         ==         isRefConstant(bsl::get<0>(mX)));
            ASSERT(false         ==         isRefConstant(bsl::get<1>(mX)));

            ASSERT(true          ==         isRefConstant(bsl::get<0>( X)));
            ASSERT(true          ==         isRefConstant(bsl::get<1>( X)));
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // Testing 'TYPE&& get(array&&)' and 'const TYPE&& get(const array&&)'.
        {
            typedef bslmf::MovableRefUtil MoveUtil;

            const char* SPEC = "ABCDEFGHIJKLMNOPQRSTUV";

            TA mW; const  TA& W  = gg(&mW,  SPEC);

            TA mX0; const TA& X0 = gg(&mX0, SPEC);
            TA mX1; const TA& X1 = gg(&mX1, SPEC);

            ASSERT(true  == isRefConstant(bsl::get<0>(MoveUtil::move( X0))));
            ASSERT(true  == isRefConstant(bsl::get<1>(MoveUtil::move( X1))));

            ASSERT(W[0]  ==               bsl::get<0>(MoveUtil::move( X0)));
            ASSERT(W[1]  ==               bsl::get<1>(MoveUtil::move( X1)));

            ASSERT(false == isRefConstant(bsl::get<0>(MoveUtil::move(mX0))));
            ASSERT(false == isRefConstant(bsl::get<1>(MoveUtil::move(mX1))));

            ASSERT(W[0] ==                bsl::get<0>(MoveUtil::move(mX0)));
            ASSERT(W[1] ==                bsl::get<1>(MoveUtil::move(mX1)));
        }
#endif
    }
};

template<class TYPE>
struct TupleApiTest<TYPE, 1>
    // This specialization of 'TupleApiTest' provides a namespace for utility
    // function that test tuple-API for bsl::arrays of size 1.
{
    static void testTupleApi()
    {
        typedef                bsl::array<         TYPE, 1>     TA ;

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
        typedef                bsl::array<const    TYPE, 1>     CTA;
        typedef                bsl::array<volatile TYPE, 1>     VTA;

        typedef const          bsl::array<         TYPE, 1>   C_TA ;
        typedef const          bsl::array<const    TYPE, 1>   C_CTA;
        typedef const          bsl::array<volatile TYPE, 1>   C_VTA;

        typedef       volatile bsl::array<         TYPE, 1>   V_TA ;
        typedef       volatile bsl::array<const    TYPE, 1>   V_CTA;
        typedef       volatile bsl::array<volatile TYPE, 1>   V_VTA;

        typedef const volatile bsl::array<         TYPE, 1>  CV_TA ;
        typedef const volatile bsl::array<const    TYPE, 1>  CV_CTA;
        typedef const volatile bsl::array<volatile TYPE, 1>  CV_VTA;
        // Testing 'tuple_size'.
        {
            typedef std::tuple_size< TA     > TS_TA    ;
            typedef std::tuple_size< CTA    > TS_CTA   ;
            typedef std::tuple_size< VTA    > TS_VTA   ;
            typedef std::tuple_size< C_TA   > TS_C_TA  ;
            typedef std::tuple_size< C_CTA  > TS_C_CTA ;
            typedef std::tuple_size< C_VTA  > TS_C_VTA ;
            typedef std::tuple_size< V_TA   > TS_V_TA  ;
            typedef std::tuple_size< V_CTA  > TS_V_CTA ;
            typedef std::tuple_size< V_VTA  > TS_V_VTA ;
            typedef std::tuple_size< CV_TA  > TS_CV_TA ;
            typedef std::tuple_size< CV_CTA > TS_CV_CTA;
            typedef std::tuple_size< CV_VTA > TS_CV_VTA;

            ASSERT((1u == TS_TA::value    ));
            ASSERT((1u == TS_CTA::value   ));
            ASSERT((1u == TS_VTA::value   ));
            ASSERT((1u == TS_C_TA::value  ));
            ASSERT((1u == TS_C_CTA::value ));
            ASSERT((1u == TS_C_VTA::value ));
            ASSERT((1u == TS_V_TA::value  ));
            ASSERT((1u == TS_V_CTA::value ));
            ASSERT((1u == TS_V_VTA::value ));
            ASSERT((1u == TS_CV_TA::value ));
            ASSERT((1u == TS_CV_CTA::value));
            ASSERT((1u == TS_CV_VTA::value));
        }

        // Testing 'tuple_element'.
        {
            typedef std::tuple_element< 0 ,     TA > TE0_TA    ;
            typedef std::tuple_element< 0 ,    CTA > TE0_CTA   ;
            typedef std::tuple_element< 0 ,    VTA > TE0_VTA   ;

            typedef std::tuple_element< 0 ,  C_TA  > TE0_C_TA  ;
            typedef std::tuple_element< 0 ,  C_CTA > TE0_C_CTA ;
            typedef std::tuple_element< 0 ,  C_VTA > TE0_C_VTA ;

            typedef std::tuple_element< 0 ,  V_TA  > TE0_V_TA  ;
            typedef std::tuple_element< 0 ,  V_CTA > TE0_V_CTA ;
            typedef std::tuple_element< 0 ,  V_VTA > TE0_V_VTA ;

            typedef std::tuple_element< 0 , CV_TA  > TE0_CV_TA ;
            typedef std::tuple_element< 0 , CV_CTA > TE0_CV_CTA;
            typedef std::tuple_element< 0 , CV_VTA > TE0_CV_VTA;

            // Aliases for returned types.

            typedef                TYPE    T;
            typedef const          TYPE  C_T;
            typedef       volatile TYPE  V_T;
            typedef const volatile TYPE CV_T;

            ASSERT((bsl::is_same<   T,  typename TE0_TA::type     >::value));
            ASSERT((bsl::is_same< C_T,  typename TE0_CTA::type    >::value));
            ASSERT((bsl::is_same< V_T,  typename TE0_VTA::type    >::value));

            ASSERT((bsl::is_same<  C_T, typename TE0_C_TA::type   >::value));
            ASSERT((bsl::is_same<  C_T, typename TE0_C_CTA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_C_VTA::type  >::value));

            ASSERT((bsl::is_same<  V_T, typename TE0_V_TA::type   >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_V_CTA::type  >::value));
            ASSERT((bsl::is_same<  V_T, typename TE0_V_VTA::type  >::value));

            ASSERT((bsl::is_same< CV_T, typename TE0_CV_TA::type  >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_CV_CTA::type >::value));
            ASSERT((bsl::is_same< CV_T, typename TE0_CV_VTA::type >::value));
        }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE

        // Testing 'TYPE& get(array&)' and 'const TYPE& get(const array&)'.
        {
            TA        mX;
            const TA& X = mX;

            ASSERT(mX.data() + 0 == bsls::Util::addressOf(bsl::get<0>(mX)));

            ASSERT( X.data() + 0 == bsls::Util::addressOf(bsl::get<0>( X)));

            ASSERT(false         ==         isRefConstant(bsl::get<0>(mX)));

            ASSERT(true          ==         isRefConstant(bsl::get<0>( X)));
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // Testing 'TYPE&& get(array&&)' and 'const TYPE&& get(const array&&)'.
        {
            typedef bslmf::MovableRefUtil MoveUtil;

            const char* SPEC = "ABCDEFGHIJKLMNOPQRSTUV";

            TA mW; const  TA& W  = gg(&mW,  SPEC);

            TA mX0; const TA& X0 = gg(&mX0, SPEC);

            ASSERT(true  == isRefConstant(bsl::get<0>(MoveUtil::move( X0))));

            ASSERT(W[0]  ==               bsl::get<0>(MoveUtil::move( X0)));

            ASSERT(false == isRefConstant(bsl::get<0>(MoveUtil::move(mX0))));

            ASSERT(W[0] ==                bsl::get<0>(MoveUtil::move(mX0)));
        }
#endif
    }
};

template<class TYPE>
struct TupleApiTest<TYPE, 0>
    // This specialization of 'TupleApiTest' provides a namespace for utility
    // function that test tuple-API for bsl::arrays of size 0`.
{
    static void testTupleApi()
    {
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
        typedef                bsl::array<         TYPE, 0>     TA ;
        typedef                bsl::array<const    TYPE, 0>     CTA;
        typedef                bsl::array<volatile TYPE, 0>     VTA;

        typedef const          bsl::array<         TYPE, 0>   C_TA ;
        typedef const          bsl::array<const    TYPE, 0>   C_CTA;
        typedef const          bsl::array<volatile TYPE, 0>   C_VTA;

        typedef       volatile bsl::array<         TYPE, 0>   V_TA ;
        typedef       volatile bsl::array<const    TYPE, 0>   V_CTA;
        typedef       volatile bsl::array<volatile TYPE, 0>   V_VTA;

        typedef const volatile bsl::array<         TYPE, 0>  CV_TA ;
        typedef const volatile bsl::array<const    TYPE, 0>  CV_CTA;
        typedef const volatile bsl::array<volatile TYPE, 0>  CV_VTA;
        // Testing 'tuple_size'.
        {
            typedef std::tuple_size< TA     > TS_TA    ;
            typedef std::tuple_size< CTA    > TS_CTA   ;
            typedef std::tuple_size< VTA    > TS_VTA   ;
            typedef std::tuple_size< C_TA   > TS_C_TA  ;
            typedef std::tuple_size< C_CTA  > TS_C_CTA ;
            typedef std::tuple_size< C_VTA  > TS_C_VTA ;
            typedef std::tuple_size< V_TA   > TS_V_TA  ;
            typedef std::tuple_size< V_CTA  > TS_V_CTA ;
            typedef std::tuple_size< V_VTA  > TS_V_VTA ;
            typedef std::tuple_size< CV_TA  > TS_CV_TA ;
            typedef std::tuple_size< CV_CTA > TS_CV_CTA;
            typedef std::tuple_size< CV_VTA > TS_CV_VTA;

            ASSERT((0 == TS_TA::value    ));
            ASSERT((0 == TS_CTA::value   ));
            ASSERT((0 == TS_VTA::value   ));
            ASSERT((0 == TS_C_TA::value  ));
            ASSERT((0 == TS_C_CTA::value ));
            ASSERT((0 == TS_C_VTA::value ));
            ASSERT((0 == TS_V_TA::value  ));
            ASSERT((0 == TS_V_CTA::value ));
            ASSERT((0 == TS_V_VTA::value ));
            ASSERT((0 == TS_CV_TA::value ));
            ASSERT((0 == TS_CV_CTA::value));
            ASSERT((0 == TS_CV_VTA::value));
        }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
    }
};

template <class TYPE, size_t SIZE>
bool testEqualityComparison(const bsl::array<TYPE, SIZE>& array1,
                            const bsl::array<TYPE, SIZE>& array2,
                            bool                          specsAreEqual)
    // Compare the specified 'array1' and 'array2' and return 'true' if they
    // are equal (populated based on the same specification (the specified
    // 'specsAreEqual' is true)) and 'false' otherwise.
{
    bool result = true;
    if (specsAreEqual) {
        result &=  (array1 == array2);
        result &=  (array2 == array1);
        result &= !(array1 != array2);
        result &= !(array2 != array1);
    }
    else {
        result &= !(array1 == array2);
        result &= !(array2 == array1);
        result &=  (array1 != array2);
        result &=  (array2 != array1);
    }
    return result;
}

struct CopyState {
  public:
    // TYPES
    enum Enum {
        // Enumeration of copy state.

        e_NOT_COPIED,  // The type was not involved in a copy operation.

        e_COPIED,      // The type was involved in a copy operation.

        e_UNKNOWN      // The type does not expose move-state information.
    };
};

                             // ======================
                             // class CopyOnlyTestType
                             // ======================

class CopyOnlyTestType {
    // This class, that does not support move constructors, provides an
    // unconstrained (value-semantic) attribute type that records when copy
    // semantics have been invoked with the object instance as the source
    // parameter.  Each instance also records if it was default-constructed.

    // DATA
    int                     d_value;       // object's value
    mutable CopyState::Enum d_copiedFrom;  // copied-from state
    mutable CopyState::Enum d_copiedInto;  // copied-into state
    bool                    d_defaultConstructed;
                                           // 'true' if default constructed

  public:
    // CREATORS
    CopyOnlyTestType()
        // Create an object having the null value.
    : d_value(0)
    , d_copiedFrom(CopyState::e_NOT_COPIED)
    , d_copiedInto(CopyState::e_NOT_COPIED)
    , d_defaultConstructed(true)
    {}

    explicit CopyOnlyTestType(int value)
        // Create an object that has the specified 'value'.
    : d_value(value)
    , d_copiedFrom(CopyState::e_NOT_COPIED)
    , d_copiedInto(CopyState::e_NOT_COPIED)
    , d_defaultConstructed(false)
    {}

    CopyOnlyTestType(const CopyOnlyTestType& original)
        // Create an object having the value of the specified 'original'
        // object.
    : d_value(original.d_value)
    , d_copiedFrom(CopyState::e_NOT_COPIED)
    , d_copiedInto(CopyState::e_COPIED)
    , d_defaultConstructed(false)
    {
        original.d_copiedFrom = CopyState::e_COPIED;
    }

    // MANIPULATORS
    CopyOnlyTestType& operator=(CopyOnlyTestType rhs)
    {
        d_value          = rhs.d_value;
        d_copiedFrom     = CopyState::e_NOT_COPIED;
        d_copiedInto     = CopyState::e_COPIED;
        rhs.d_copiedFrom = CopyState::e_COPIED;

        return *this;
    }

    void resetCopyState()
    {
        d_copiedFrom = CopyState::e_NOT_COPIED;
        d_copiedInto = CopyState::e_NOT_COPIED;
    }

    void setCopiedFrom(CopyState::Enum value)
    {
        d_copiedFrom = value;
    }

    void setCopiedInto(CopyState::Enum value)
    {
        d_copiedInto = value;
    }

    // ACCESSORS
    int value() const
        // Return the value of this object.
    {
        return d_value;
    }

    CopyState::Enum copiedInto() const
        // Return the copy state of this object as target of a copy operation.
    {
        return d_copiedInto;
    }

    CopyState::Enum copiedFrom() const
        // Return the copy state of this object as source of a copy operation.
    {
        return d_copiedFrom;
    }

    bool defaultConstructed() const
        // Return 'true' if the default constructor was used to create this
        // object.
    {
        return d_defaultConstructed;
    }

};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14)
constexpr int testConstexprForNonConstBracketOperator()
    // Use the bracket operator of 'bsl::array' that returns a non-'const'
    // reference to each element and (incidentally) return 0.  The compilation
    // of this 'constexpr' function demonstrates that the operator is itself
    // 'constexpr'-qualified as intended.
{
    typedef bsl::array<int, 7> Obj;
    Obj mX = { 3, 1, 4, 1, 5, 9, 3 };

    mX[0] = 0;
    mX[1] = 0;
    mX[2] = 0;
    mX[3] = 0;
    mX[4] = 0;
    mX[5] = 0;
    mX[6] = 0;

    return 0;
}

constexpr int testConstexprForNonConstAt()
    // Use the 'at' manipulator of 'bsl::array' that returns a non-'const'
    // reference to each element and (incidentally) return 0.  The compilation
    // of this 'constexpr' function demonstrates that the manipulator is itself
    // 'constexpr'-qualified as intended.
{
    typedef bsl::array<int, 7> Obj;
    Obj mX = { 3, 1, 4, 1, 5, 9, 3 };

    mX.at(0) = 0;
    mX.at(1) = 0;
    mX.at(2) = 0;
    mX.at(3) = 0;
    mX.at(4) = 0;
    mX.at(5) = 0;
    mX.at(6) = 0;

    return 0;
}

constexpr int testConstexprForNonConstFront()
    // Use the 'front' manipulator of 'bsl::array' that returns a non-'const'
    // reference to the first element and (incidentally) return 0.  The
    // compilation of this 'constexpr' function demonstrates that the
    // manipulator is itself 'constexpr'-qualified as intended.
{
    typedef bsl::array<int, 7> Obj;
    Obj mX = { 3, 1, 4, 1, 5, 9, 3 };

    mX.front() = 0;

    return 0;
}

constexpr int testConstexprForNonConstBack()
    // Use the 'back' manipulator of 'bsl::array' that returns a non-'const'
    // reference to the last element and (incidentally) return 0.  The
    // compilation of this 'constexpr' function demonstrates that the
    // manipulator is itself 'constexpr'-qualified as intended.
{
    typedef bsl::array<int, 7> Obj;
    Obj mX = { 3, 1, 4, 1, 5, 9, 3 };

    mX.back() = 0;

    return 0;
}

constexpr int testConstexprForNonConstData()
    // Use the 'data' manipulator of 'bsl::array' that returns a non-'const'
    // pointer to the first element and (incidentally) return 0.  The
    // compilation of this 'constexpr' function demonstrates that the
    // manipulator is itself 'constexpr'-qualified as intended.
{
    typedef bsl::array<int, 7> Obj;
    Obj mX = { 3, 1, 4, 1, 5, 9, 3 };

    mX.data()[0] = 0;
    mX.data()[1] = 0;
    mX.data()[2] = 0;
    mX.data()[3] = 0;
    mX.data()[4] = 0;
    mX.data()[5] = 0;
    mX.data()[6] = 0;

    return 0;
}

constexpr int testConstexprForNonConstBeginAndEnd()
    // Use the 'begin' and 'end' manipulators of 'bsl::array' that return a
    // non-'const' iteator to the first element and one past the last element,
    // respectively, and (incidentally) return 0.  The compilation of this
    // 'constexpr' function demonstrates that the manipulators are themselves
    // 'constexpr'-qualified as intended.
{
    typedef bsl::array<int, 7> Obj;
    Obj mX = { 3, 1, 4, 1, 5, 9, 3 };

    for (Obj::iterator iter = mX.begin(); iter != mX.end(); ++iter) {
        *iter = 0;
    }

    return 0;
}
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17)
constexpr int testConstexprForNonConstRbegin()
    // Use the 'rbegin' manipulator of 'bsl::array' that returns a non-'const'
    // reverse-iterator to the last element and (incidentally) return 0.  The
    // compilation of this 'constexpr' function demonstrates that the
    // manipulator is itself 'constexpr'-qualified as intended.
{
    typedef bsl::array<int, 7> Obj;
    Obj mX = { 3, 1, 4, 1, 5, 9, 3 };

    *(mX.rbegin()) = 0;

    return 0;
}

constexpr int testConstexprForNonConstRend()
{
    // Use the 'rend' manipulator of 'bsl::array' that returns a non-'const'
    // reverse-iterator to one before the first element and (incidentally)
    // return 0.  The compilation of this 'constexpr' function demonstrates
    // that the manipulator is itself 'constexpr'-qualified as intended.
    typedef bsl::array<int, 7> Obj;
    Obj mX = { 3, 1, 4, 1, 5, 9, 3 };

    *(mX.rend() - 1) = 0;

    return 0;
}

#endif

// ============================================================================
//                          TEST DRIVER TEMPLATE
// ----------------------------------------------------------------------------

template<class TYPE, size_t SIZE>
struct TestDriver {

    typedef bsl::array<TYPE, SIZE> Obj;

    typedef typename Obj::iterator               Iter;
    typedef typename Obj::const_iterator         CIter;
    typedef typename Obj::reverse_iterator       RIter;
    typedef typename Obj::const_reverse_iterator CRIter;
    typedef typename Obj::value_type             ValueType;

    typedef bslmf::MovableRefUtil                MoveUtil;

    typedef bsltf::TestValuesArray<TYPE>         TestValues;
    typedef bsltf::TemplateTestFacility          TestFacility;

    static void testCase26();
        // Test 'to_array'.

    static void testCase24();
        // Test type traits.

    static void testCase22();
        // Test 'hashAppend'.

    static void testCase21();
        // Test tuple interface.

    static void testCase20();
        // Test 'data' member.

    static void testCase19();
        // Test 'front' and 'back' members.

    static void testCase18();
        // Test 'at' member.

    static void testCase17();
        // Test 'empty' and 'max_size' members.

    static void testCase16();
        // Test comparison operators.

    static void testCase15();
        // Test aggregate initialization.

    static void testCase14();
        // Test iterators.

    static void testCase13();
        // Test 'fill' member.

    static void testCase12();
        // Test move assignment operator.

    static void testCase11();
        // Test move constructor.

    static void testCase10();
        // Test streaming functionality.  This test case tests nothing.

    static void testCase9();
        // Test assignment operator ('operator=').

    static void testCase8();
        // Test 'swap' member.

    static void testCase7();
        // Test copy constructor.

    static void testCase6();
        // Test equality operator ('operator==').

    static void testCase5();
        // Test output (<<) operator.  This test case tests nothing.

    static void testCase4();
        // Test basic accessors ('size' and 'operator[]').

    static void testCase3();
        // Test generator functions 'ggg' and 'gg'.

    static void testCase2();
        // Test primary manipulators.

    static void testCase1();
        // Breathing Test.  Exercises basic functionality.
};

template<class TYPE>
struct TestDriverWrapper{

    static void testCase26();
        // Test 'to_array'.

    static void testCase24();
        // Test type traits.

    static void testCase22();
        // Test 'hashAppend'.

    static void testCase21();
        // Test tuple interface.

    static void testCase20();
        // Test 'data' member.

    static void testCase19();
        // Test 'front' and 'back' members.

    static void testCase18();
        // Test 'at' member.

    static void testCase17();
        // Test 'empty' and 'max_size' members.

    static void testCase16();
        // Test comparison operators.

    static void testCase15();
        // Test aggregate initialization.

    static void testCase14();
        // Test iterators.

    static void testCase13();
        // Test 'fill' member.

    static void testCase12();
        // Test move assignment operator.

    static void testCase11();
        // Test move constructor.

    static void testCase10();
        // Test streaming functionality.  This test case tests nothing.

    static void testCase9();
        // Test assignment operator ('operator=').

    static void testCase8();
        // Test 'swap' member.

    static void testCase7();
        // Test copy constructor.

    static void testCase6();
        // Test equality operator ('operator==').

    static void testCase5();
        // Test output (<<) operator.  This test case tests nothing.

    static void testCase4();
        // Test basic accessors ('size' and 'operator[]').

    static void testCase3();
        // Test generator functions 'ggg' and 'gg'.

    static void testCase2();
        // Test primary manipulators.

    static void testCase1();
        // Breathing Test.  Exercises basic functionality.
};

                                // ----------
                                // Test Cases
                                // ----------

// suppress comparison of 'unsigned' expression is always false warnings
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase26()
{
    using bsls::NameOf;

    TestDriver<TYPE,  1>::testCase26();
    TestDriver<TYPE,  2>::testCase26();
    TestDriver<TYPE,  3>::testCase26();
    TestDriver<TYPE,  4>::testCase26();
    TestDriver<TYPE,  5>::testCase26();
    TestDriver<TYPE, 16>::testCase26();
    TestDriver<TYPE, 32>::testCase26();
    TestDriver<TYPE, 64>::testCase26();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase26()
{
    // ------------------------------------------------------------------------
    // TESTING 'to_array'
    //   Ensure that the 'to_array' free functions work correctly.
    //
    // Concerns:
    //: 1 Built-in arrays of copyable types result in all of the elements
    //:   being copied over.
    //:
    //: 2 Built-in arrays of move-only types result in all of the elements
    //:   being moved over (if supported).
    //:
    //: 3 Attempts to call 'to_array' on a multi-dimensional built-in array
    //:   fail.
    //
    // Plan:
    //: 1 Call 'to_array' on an lvalue of a built-in array of copyable values
    //:   and make sure the values were copied.
    //:
    //: 2 Call 'to_array' on an rvalue of a built-in array of move-only values
    //:   and make sure the values were moved.
    //:
    //: 3 Under a #define, call 'to_array' on a multi-dimensional built-in
    //:   array, expecting a compilation failure.
    //
    // Testing:
    //      array<TYPE, SIZE> to_array(TYPE (&src)[SIZE]);
    //      array<TYPE, SIZE> to_array(TYPE (&&src)[SIZE]);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTESTING 'to_array' of size %d"
                        "\n============================\n", int(SIZE));

    enum { k_ARR_SIZE = SIZE };

    std::size_t builtin_size_t_arr[k_ARR_SIZE];

    for (std::size_t i = 0; i < k_ARR_SIZE; ++i) {
        builtin_size_t_arr[i] = i;
    }

    if (veryVerbose) printf("...testing copy of standard type\n");
    {
        bsl::array<std::size_t, k_ARR_SIZE> bsl_arr =
                                             bsl::to_array(builtin_size_t_arr);

        ASSERT(true ==
               std::equal(bsl_arr.begin(), bsl_arr.end(), builtin_size_t_arr));
    }

    if (veryVerbose) printf("...testing copy of class type\n");
    {
        bsltf::MovableTestType builtin_arr[k_ARR_SIZE];

        for (std::size_t i = 0; i < k_ARR_SIZE; ++i) {
            ASSERTV(
                  i,
                  bsltf::MoveState::e_NOT_MOVED == builtin_arr[i].movedFrom());
        }

        bsl::array<bsltf::MovableTestType, k_ARR_SIZE> bsl_arr =
                                                    bsl::to_array(builtin_arr);

        for (std::size_t i = 0; i < k_ARR_SIZE; ++i) {
            ASSERTV(
                  i,
                  bsltf::MoveState::e_NOT_MOVED == builtin_arr[i].movedFrom());
            // Note that the result may be 'movedInto' if the compiler chooses
            // to move the resulting array.
        }
    }

    if (veryVerbose)
        printf("...testing copy of class type with default ctor call check\n");
    {
        CopyOnlyTestType builtin_arr[k_ARR_SIZE];

        for (std::size_t i = 0; i < k_ARR_SIZE; ++i) {
            ASSERTV(i, CopyState::e_NOT_COPIED == builtin_arr[i].copiedFrom());
            ASSERTV(i, builtin_arr[i].defaultConstructed());
        }

        bsl::array<CopyOnlyTestType, k_ARR_SIZE> bsl_arr =
                                                    bsl::to_array(builtin_arr);

        for (std::size_t i = 0; i < k_ARR_SIZE; ++i) {
            ASSERTV(i, CopyState::e_COPIED == builtin_arr[i].copiedFrom());
            ASSERTV(i, CopyState::e_COPIED == bsl_arr[i].copiedInto());
#if (defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES) &&             \
     defined(BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES)) ||            \
   defined(BSLS_PLATFORM_CMP_SUN)
            // The Sun compiler copy-constructs the result of 'to_array' into
            // 'bsl_arr' rather than using RVO like other C++03 compilers.
            ASSERTV(i, !bsl_arr[i].defaultConstructed());
#else
            ASSERTV(i, bsl_arr[i].defaultConstructed());
#endif
        }
    }

#if defined(BSLSTL_ARRAY_CHECK_MULTIDIM_TO_ARRAY_FAILURE)
    // Negative compilation check.  This block fails to compile, validating
    // the restriction that 'TYPE' shall not itself be a built-in array.

    std::size_t builtin_size_t_multi_arr[k_ARR_SIZE][k_ARR_SIZE];

    bsl::array<std::size_t[k_ARR_SIZE], k_ARR_SIZE> multi_bsl_arr = 
        bsl::to_array(builtin_size_t_multi_arr);
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    if (veryVerbose) printf("...testing move of standard type\n");
    {
        bsl::array<std::size_t, k_ARR_SIZE> bsl_size_t_arr =
                                  bsl::to_array(std::move(builtin_size_t_arr));

        for (std::size_t i = 0; i < k_ARR_SIZE; ++i) {
            ASSERTV(i, bsl_size_t_arr[i], i == bsl_size_t_arr[i]);
        }
    }

    if (veryVerbose) printf("...testing move of class type\n");
    {
        bsltf::MovableTestType builtin_arr[k_ARR_SIZE];

        for (std::size_t i = 0; i < k_ARR_SIZE; ++i) {
            ASSERTV(
                  i,
                  bsltf::MoveState::e_NOT_MOVED == builtin_arr[i].movedFrom());
        }

        bsl::array<bsltf::MovableTestType, k_ARR_SIZE> bsl_arr =
                                         bsl::to_array(std::move(builtin_arr));

        for (std::size_t i = 0; i < k_ARR_SIZE; ++i) {
            ASSERTV(i,
                    bsltf::MoveState::e_MOVED == builtin_arr[i].movedFrom());
            ASSERTV(i, bsltf::MoveState::e_MOVED == bsl_arr[i].movedInto());
        }
    }
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#if (defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES) &&             \
     defined(BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES))
    if (veryVerbose)
        printf("...testing copy of NonDefaultConstructibleTestType\n");
    {
        // Note that this test does not depend on 'SIZE' due to the need
        // for an initializer list.
        bsltf::NonDefaultConstructibleTestType builtin_arr[4] = {
            bsltf::NonDefaultConstructibleTestType(1),
            bsltf::NonDefaultConstructibleTestType(2),
            bsltf::NonDefaultConstructibleTestType(3),
            bsltf::NonDefaultConstructibleTestType(4),
        };

        bsl::array<bsltf::NonDefaultConstructibleTestType, 4>
            bsl_arr = bsl::to_array(builtin_arr);
    }
#endif
}


template<class TYPE>
void TestDriverWrapper<TYPE>::testCase24()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase24();
    TestDriver<TYPE, 1>::testCase24();
    TestDriver<TYPE, 2>::testCase24();
    TestDriver<TYPE, 3>::testCase24();
    TestDriver<TYPE, 4>::testCase24();
    TestDriver<TYPE, 5>::testCase24();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase24()
{
    // ------------------------------------------------------------------------
    // TESTING TYPE TRAITS
    //
    // Concerns:
    //: 1 That the array has the 'bslalg::HasStlIterators' trait.
    //
    // Plan:
    //: 1 Test each of the above traits and compare their values to the
    //:   expected values.
    //
    // Testing:
    //   bslalg::HasStlIterators
    // ------------------------------------------------------------------------

    if (verbose) printf("TESTING TYPE TRAITS\n"
                        "===================\n");

    BSLMF_ASSERT(bslalg::HasStlIterators<Obj>::value);
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase22()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase22();
    TestDriver<TYPE, 1>::testCase22();
    TestDriver<TYPE, 2>::testCase22();
    TestDriver<TYPE, 3>::testCase22();
    TestDriver<TYPE, 4>::testCase22();
    TestDriver<TYPE, 5>::testCase22();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase22()
{
    // ------------------------------------------------------------------------
    // TESTING 'hashAppend'
    //
    // Concerns:
    //: 1 Objects constructed with the same values hash as equal.
    //:
    //: 2 Unequal objects hash as unequal (not required, but we can hope).
    //
    // Plan:
    //: 1 Specify a set S of unique object values having various minor or
    //:   subtle differences.
    //:
    //: 2 Verify the correctness of hash values matching using all elements of
    //:   the cross product S X S.  (C-1..2)
    //
    // Testing:
    //   void hashAppend(HASH_ALGORITHM&, const bsl::array<TYPE, SIZE>&);
    // ------------------------------------------------------------------------

    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    typedef ::BloombergLP::bslh::Hash<> Hasher;
    typedef Hasher::result_type         HashType;
    Hasher                              hasher;

    const int NUM_SPECS = 6;

    static const struct {
        int         d_line;               // source line number
        const char *d_spec_p[NUM_SPECS];  // specification strings
    } DATA[] = {
        //line   spec 1   spec 2   spec 3   spec 4   spec 5   spec 6
        //----   -------  -------  -------  -------  -------  -------
        { L_,  { "",      "",      "",      "",      "",      ""     } },  // 0
        { L_,  { "A",     "B",     "C",     "D",     "E",     "F"    } },  // 1
        { L_,  { "AA",    "AB",    "BA",    "BB",    "AC",    "CA"   } },  // 2
        { L_,  { "AAA",   "AAB",   "ABA",   "BAA",   "BBB",   "AAC"  } },  // 3
        { L_,  { "AAAA",  "AAAB",  "AABA",  "ABAA",  "BAAA",  "BBBB" } },  // 4
        { L_,  { "AAAAA", "AAAAB", "AAABA", "AABAA", "ABAAA", "BAAAA"} },  // 5
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    ASSERTV("This size is not supported", SIZE, NUM_DATA > SIZE);

    for (size_t i = 0; i < NUM_SPECS; ++i) {
        const char *const SPEC1 = DATA[SIZE].d_spec_p[i];

        Obj mX1; const Obj& X1 = gg(&mX1, SPEC1);

        for (size_t j = 0; j < NUM_SPECS; ++j) {
            const char* const SPEC2 = DATA[SIZE].d_spec_p[j];

            if (veryVerbose)
                printf("\t\t\tComparing %s and %s.\n", SPEC1, SPEC2);

            Obj mX2; const Obj& X2 = gg(&mX2, SPEC2);

            HashType hX1 = hasher(X1);
            HashType hX2 = hasher(X2);
            if (0 != SIZE) {
                ASSERTV(SIZE, i, j, (i == j) == (hX1 == hX2));
            }
            else {
                ASSERTV(SIZE, i, j, hX1 == hX2);
            }
        }
    }
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase21()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase21();
    TestDriver<TYPE, 1>::testCase21();
    TestDriver<TYPE, 2>::testCase21();
    TestDriver<TYPE, 3>::testCase21();
    TestDriver<TYPE, 4>::testCase21();
    TestDriver<TYPE, 5>::testCase21();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase21()
{
    // ------------------------------------------------------------------------
    // TESTING TUPLE INTERFACE
    //
    // Concerns:
    //: 1 The 'get' function returns array's element with requested index.
    //:
    // Plan:
    //: 1 Create an array from spec string.
    //:
    //: 2 Test that 'get' free function returns correct value on all overloads.
    //:
    //: 3 Test that 'get' free function returns correct type for rvalue
    //:   references.  (C-1)
    //:
    // Testing:
    //   std::tuple_element<bsl::array<TYPE, SIZE> >
    //   std::tuple_size<bsl::array<TYPE, SIZE> >
    //   TYPE& get(bsl::array<TYPE, SIZE>& a)
    //   const TYPE& get(const bsl::array<TYPE, SIZE>& a)
    //   const TYPE&& get(const bsl::array<TYPE, SIZE>&& a)
    //   TYPE&& get(bsl::array<TYPE, SIZE>&& a)
    // ------------------------------------------------------------------------

    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    TupleApiTest<TYPE, SIZE>::testTupleApi();
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase20()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase20();
    TestDriver<TYPE, 1>::testCase20();
    TestDriver<TYPE, 2>::testCase20();
    TestDriver<TYPE, 3>::testCase20();
    TestDriver<TYPE, 4>::testCase20();
    TestDriver<TYPE, 5>::testCase20();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase20()
{
    // ------------------------------------------------------------------------
    // TESTING 'data'
    //
    // Concerns:
    //: 1 'data' member returns a pointer to the raw array in the object.
    //:
    //: 2 'data' has the correct signature when used on a const object.
    //:
    // Plan:
    //: 1 Using special function verify that 'data()' returns constant pointer
    //:   for the constant objects and non-constant pointer otherwise.  (C-2)
    //:
    //: 2 Test 'data' member returns a pointer to the array member variable.
    //:   (C-1)
    //
    // Testing:
    //  pointer data();
    //  const_pointer data() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    Obj        mX;
    const Obj& X = mX;

    ASSERTV(SIZE, false == isPtrConstant(mX.data()));
    ASSERTV(SIZE, true  == isPtrConstant( X.data()));

#if !defined(BSLSTL_ARRAY_IS_ALIASED)
    ASSERTV(mX.d_data == mX.data());
    ASSERTV( X.d_data ==  X.data());
    ASSERTV(mX.d_data == bsl::data(mX));
    ASSERTV( X.d_data == bsl::data( X));
#endif
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase19()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 1>::testCase19();
    TestDriver<TYPE, 2>::testCase19();
    TestDriver<TYPE, 3>::testCase19();
    TestDriver<TYPE, 4>::testCase19();
    TestDriver<TYPE, 5>::testCase19();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase19()
{
    // ------------------------------------------------------------------------
    // TESTING 'front' AND 'back'
    //
    // Concerns:
    //: 1 'front' member accesses first elememt of object correctly.
    //:
    //: 2 'back' member accesses last elememt of object correctly.
    //:
    //: 3 Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Generate an array from a spec string
    //:
    //: 2 Verify that the address of the referenced element returned from
    //:   'front' and 'back' are the addresses of the first and last elements.
    //:   (C-1..2)
    //:
    //: 3 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values, but not triggered for
    //:   adjacent valid ones.  (C-3)
    //
    // Testing:
    //  reference front();
    //  const_reference front() const;
    //  reference back();
    //  const_reference back() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    if (0 != SIZE) {
        if (veryVerbose) printf("\t\t\tTesting basic behavior.\n");

        Obj        mX;
        const Obj& X = mX;

        const TYPE  *EXP_FRONT_ADDRESS = mX.data();
        const TYPE  *EXP_BACK_ADDRESS  = mX.data() + SIZE -1;

        ASSERTV(SIZE, false == isRefConstant(mX.front()));
        ASSERTV(SIZE, true  == isRefConstant( X.front()));
        ASSERTV(SIZE, false == isRefConstant(mX.back() ));
        ASSERTV(SIZE, true  == isRefConstant( X.back() ));

        TYPE&        fr                = mX.front();
        const TYPE&  fcr               = X.front();
        TYPE&        br                = mX.back();
        const TYPE&  bcr               = X.back();

        ASSERTV(SIZE, EXP_FRONT_ADDRESS == bsls::Util::addressOf(fr ));
        ASSERTV(SIZE, EXP_FRONT_ADDRESS == bsls::Util::addressOf(fcr));

        ASSERTV(SIZE, EXP_BACK_ADDRESS  == bsls::Util::addressOf(br ));
        ASSERTV(SIZE, EXP_BACK_ADDRESS  == bsls::Util::addressOf(bcr));
    }

    if (veryVerbose) printf("\t\t\tNegative Testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        Obj        mX;
        const Obj& X = mX;

        if (0 == SIZE) {
#if !defined(BSLSTL_ARRAY_IS_ALIASED)
            ASSERT_FAIL((void)mX.front());
            ASSERT_FAIL((void) X.front());
            ASSERT_FAIL((void)mX.back());
            ASSERT_FAIL((void) X.back());
#endif
        }
        else {
            ASSERT_PASS((void)mX.front());
            ASSERT_PASS((void) X.front());
            ASSERT_PASS((void)mX.back());
            ASSERT_PASS((void) X.back());
        }
    }
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase18()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase18();
    TestDriver<TYPE, 1>::testCase18();
    TestDriver<TYPE, 2>::testCase18();
    TestDriver<TYPE, 3>::testCase18();
    TestDriver<TYPE, 4>::testCase18();
    TestDriver<TYPE, 5>::testCase18();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase18()
{
    // ------------------------------------------------------------------------
    // TESTING 'at'
    //
    // Concerns:
    //: 1 The 'at' method accesses each element both as a reference offering
    //:   modifiable access and as a const-reference.
    //:
    //: 2 Elements can be set using 'at' for any type that supports the
    //:   assignment operator.
    //
    // Plan:
    //: 1 Construct an array from the spec string and verify all values are
    //:   accessed as expected.
    //:
    //: 2 Verify that the address of the referenced element returned from 'at'
    //:   is the same as that return by adding the specified index to the
    //:   'd_data' pointer.  (C-1..2)
    //:
    //: 3 Verify that 'out_of_range' exception thrown when 'pos >= size()'.
    //:   (C-3)
    //
    // Testing:
    //  reference at();
    //  const_reference at();
    // ------------------------------------------------------------------------

    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    const char* SPEC = "ABCDEFGHIJKLMNOPQRSTUV";

    if (veryVerbose)
        printf("\t\t\tTesting 'at' accesses state of object correctly\n");
    {
        Obj mX; const Obj& X = gg(&mX, SPEC);

        for (size_t i = 0; i < SIZE; ++i) {
            const TYPE  *EXP_ADDRESS = mX.data() + i;
            TYPE&        r  = mX.at(i);
            const TYPE&  cr = X.at(i);

            ASSERTV(SIZE, i, mX.data()[i] == r                        );
            ASSERTV(SIZE, i,  X.data()[i] == cr                       );
            ASSERTV(SIZE, i, EXP_ADDRESS  == bsls::Util::addressOf( r));
            ASSERTV(SIZE, i, EXP_ADDRESS  == bsls::Util::addressOf(cr));
        }
    }

#if defined(BDE_BUILD_TARGET_NO_EXC)
    if (veryVerbose) {
        printf("\t\t\tNo testing for 'out_of_range' as exceptions are"
               " disabled.\n");
    }
#else
    if (veryVerbose)
        printf("\t\t\tTesting for out_of_range exceptions thrown by 'at()' "
               "when pos >= size().\n");
    {
        Obj        mX;
        const Obj& X = gg(&mX, SPEC);

        const int NUM_TRIALS = 2;

        // Check exception behavior for non-'const' version of 'at()'.
        // Checking the behavior for 'pos == size()' and 'pos > size()'.

        int exceptions = 0;
        int trials;
        for (trials = 0; trials < NUM_TRIALS; ++trials)
        {
            try {
                (void) mX.at(SIZE + trials);
                ASSERTV(SIZE, trials, mX.size(), false);
            }
            catch (const std::out_of_range&) {
                ++exceptions;
                if (veryVerbose) {
                    printf("\t\t\tIn out_of_range exception.\n");
                    T_ T_ P_(SIZE); P(trials);
                }
            }
        }
        ASSERTV(exceptions, trials, exceptions == trials);

        exceptions = 0;

        // Check exception behavior for const version of at()
        for (trials = 0; trials < NUM_TRIALS; ++trials)
        {
            try {
                (void) X.at(SIZE + trials);
                ASSERTV(SIZE, trials, X.size(), false);
            } catch (const std::out_of_range&) {
                ++exceptions;
                if (veryVerbose) {
                    printf("\t\tIn out_of_range exception." );
                    T_ T_ P_(SIZE); P(trials);
                }
            }
        }
        ASSERTV(exceptions, trials, exceptions == trials);
    }
#endif
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase17()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase17();
    TestDriver<TYPE, 1>::testCase17();
    TestDriver<TYPE, 2>::testCase17();
    TestDriver<TYPE, 3>::testCase17();
    TestDriver<TYPE, 4>::testCase17();
    TestDriver<TYPE, 5>::testCase17();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase17()
{
    // ------------------------------------------------------------------------
    // TESTING CAPACITY
    //
    // Concerns:
    //: 1 'empty' returns 'true' if and only if the array has a size of 0.
    //:
    //: 2 'max_size' returns a value equal to the size of the array.
    //:
    //
    // Plan:
    //: 1 Create two arrays using default constructor and populate one of them.
    //:
    //: 2 Test that if 'empty()' returns 'true' if the arrays have size 0 and
    //:   'false' otherwise.  (C-1)
    //:
    //: 3 Test that 'max_size()' returns a value equal to 'SIZE' for both
    //:   arrays.  (C-2)
    //
    // Testing:
    //  bool empty() const;
    //  size_type max_size() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    const char* SPEC = "ABCDEFGHIJKLMNOPQRSTUV";

    Obj mX1; const Obj& X1 = mX1;
    Obj mX2; const Obj& X2 = gg(&mX2, SPEC);

    ASSERT((0 == SIZE) == X1.empty());
    ASSERT((0 == SIZE) == bsl::empty(X1));
    ASSERT(SIZE        == X1.max_size());
    ASSERT((0 == SIZE) == X2.empty());
    ASSERT((0 == SIZE) == bsl::empty(X2));
    ASSERT(SIZE        == X2.max_size());
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase16()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase16();
    TestDriver<TYPE, 1>::testCase16();
    TestDriver<TYPE, 2>::testCase16();
    TestDriver<TYPE, 3>::testCase16();
    TestDriver<TYPE, 4>::testCase16();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase16()
{
    // ------------------------------------------------------------------------
    // TESTING COMPARISON OPERATORS
    //
    // Concerns:
    //: 1 '<', '>', '<=', '>=' operators lexicographically compare arrays of
    //:   types supporting the '<' operator.
    //:
    //: 2 Comparison operators work on 0 length arrays.
    //:
    //: 3 Only operator '<' is used to perform comparisons.
    //:
    //: 4 Operator '<=>' is defined when available.
    //
    // Plan:
    //: 1 Create a variety of spec strings for each length.
    //:
    //: 2 Test that every operator gives expected results for each combination
    //:   of arrays generated by the spec strings of the length being tested.
    //:
    //: 3 Perform step P-2 for arrays of objects of special type, that supports
    //:   only 'operator<'.  (C-1..3)
    //
    // Testing:
    //  bool operator<(const array& lhs, const array& rhs);
    //  bool operator>(const array& lhs, const array& rhs);
    //  bool operator<=(const array& lhs, const array& rhs);
    //  bool operator>=(const array& lhs, const array& rhs);
    //  auto operator<=>(const array& lhs, const array& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    const int NUM_SPECS = 6;

    static const struct {
        int         d_line;               // source line number
        const char *d_spec_p[NUM_SPECS];  // specification strings
    } DATA[] = {
        //line   spec 1   spec 2   spec 3   spec 4   spec 5   spec 6
        //----   -------  -------  -------  -------  -------  -------
        { L_,  { "",      "",      "",      "",      "",      ""     } },  // 0
        { L_,  { "A",     "B",     "C",     "D",     "E",     "F"    } },  // 1
        { L_,  { "AA",    "AB",    "BA",    "BB",    "AC",    "CA"   } },  // 2
        { L_,  { "AAA",   "AAB",   "ABA",   "BAA",   "BBB",   "AAC"  } },  // 3
        { L_,  { "AAAA",  "AAAB",  "AABA",  "ABAA",  "BAAA",  "BBBB" } },  // 4
        { L_,  { "AAAAA", "AAAAB", "AAABA", "AABAA", "ABAAA", "BAAAA"} },  // 5
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    ASSERTV("This size is not supported", SIZE, NUM_DATA > SIZE);

    for (size_t i = 0; i < NUM_SPECS; ++i) {
        const char *const SPEC1 = DATA[SIZE].d_spec_p[i];

        Obj mX1; const Obj& X1 = gg(&mX1, SPEC1);

        for (size_t j = 0; j < NUM_SPECS; ++j) {
            const char* const SPEC2 = DATA[SIZE].d_spec_p[j];

            if (veryVerbose)
                printf("\t\t\tComparing %s and %s.\n", SPEC1, SPEC2);

            Obj mX2; const Obj& X2 = gg(&mX2, SPEC2);

            if (0 > strcmp(SPEC1, SPEC2)) {
                ASSERTV(SPEC1, SPEC2, X1 < X2);
#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE
                ASSERTV(SPEC1, SPEC2, X1 <=> X2 < 0);
#endif
            }
            else if (0 == strcmp(SPEC1, SPEC2)) {
                ASSERTV(SPEC1, SPEC2, X1 <= X2);
                ASSERTV(SPEC1, SPEC2, X1 >= X2);
#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE
                ASSERTV(SPEC1, SPEC2, X1 <=> X2 <= 0);
                ASSERTV(SPEC1, SPEC2, X1 <=> X2 >= 0);
#endif
            }
            else {
                ASSERTV(SPEC1, SPEC2, X1 > X2);
#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE
                ASSERTV(SPEC1, SPEC2, X1 <=> X2 > 0);
#endif
            }
        }
    }
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase15()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase15();
    TestDriver<TYPE, 1>::testCase15();
    TestDriver<TYPE, 2>::testCase15();
    TestDriver<TYPE, 3>::testCase15();
    TestDriver<TYPE, 4>::testCase15();
    TestDriver<TYPE, 5>::testCase15();

}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING AGGREGATE INITIALIZATION
    //
    // Concerns:
    //: 1 Aggregate initialization correctly sets all values.
    //:
    //: 2 If not all values are provided check remaining elements are value
    //:   initialized.
    //:
    //
    // Plan:
    //: 1 Use aggregate initialization to create arrays of a variety of sizes.
    //:
    //: 2 Ensure elements all have expected values.  (C-1)
    //:
    //: 3 Ensure any elements not provided in braces were default value
    //:   initialized.  (C-2)
    //
    // Testing:
    //  array<V> a = {vt1, vt2, vt3}
    // ------------------------------------------------------------------------

    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    const char* SPEC = "ABCDEFGHIJKLMNOPQRSTUV";

    AggregateTest<TYPE, SIZE>::testAggregate(SPEC);
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase14()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase14();
    TestDriver<TYPE, 1>::testCase14();
    TestDriver<TYPE, 2>::testCase14();
    TestDriver<TYPE, 3>::testCase14();
    TestDriver<TYPE, 4>::testCase14();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING ITERATORS
    //
    // Concerns:
    //: 1 'iterator' is a pointer to 'TYPE'.
    //:
    //: 2 'const_iterator' is a pointer to 'const TYPE'.
    //:
    //: 3 The ranges '[begin(), end())' and '[cbegin(), cend())' traverse the
    //:   elements of the array in index order.
    //:
    //: 4 'reverse_iterator' and 'const_reverse_iterator' are implemented by
    //:   the (fully-tested) 'bslstl::ReverseIterator' over a pointer to 'TYPE'
    //:   or 'const TYPE'.
    //:
    //: 5 The ranges '[rbegin(), rend())' and '[crbegin(), crend())' traverse
    //:   the elements of the array in reverse index order.
    //
    // Plan:
    //: 1 Use 'bsl::is_same' trait to verify iterator types.  (C-1,2,4)
    //:
    //: 2 Construct an array from the 'SPEC'.
    //:
    //: 3 Access each element using iterators, and verify that iterators point
    //:   to the expected addresses.
    //:
    //: 4 Repeat step P-2 with reverse iterators.
    //:
    //: 5 Access each element of a const array using const iterators and const
    //:   reverse iterators to ensure the addresses are as expected.  (C-3,5)
    //
    // Testing:
    //  iterator begin();
    //  iterator end();
    //  reverse_iterator rbegin();
    //  reverse_iterator rend();
    //  const_iterator begin() const;
    //  const_iterator end() const;
    //  const_reverse_iterator rbegin() const;
    //  const_reverse_iterator rend() const;
    //  const_iterator cbegin() const;
    //  const_iterator cend() const;
    //  const_reverse_iterator crbegin() const;
    //  const_reverse_iterator crend() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    if (veryVerbose) printf("\t\t\tTesting iterators are correct types.\n");

#if !defined(BSLSTL_ARRAY_IS_ALIASED)
    ASSERT( (bsl::is_same<Iter,        TYPE *>::value));
    ASSERT( (bsl::is_same<CIter, const TYPE *>::value));
#endif
#ifndef BSLS_PLATFORM_CMP_SUN
    ASSERT( (bsl::is_same<RIter, bsl::reverse_iterator<Iter> >::value));
    ASSERT( (bsl::is_same<CRIter,
                          bsl::reverse_iterator<CIter> >::value));
#endif

    const char *SPEC = "ABCDEFGHIJKLMNOPQRSTUV";

    Obj        mX;
    const Obj& X     = gg(&mX, SPEC);
    int        count = 0;

    if (veryVerbose) printf("\t\t\tTesting forward functions.\n");

    Iter    it = mX.begin();
    CIter  cit =  X.begin();
    CIter ccit =  X.cbegin();

    for (;
         it != mX.end() && cit != X.end() && ccit != X.cend();
         ++it, ++cit, ++ccit) {
        ASSERTV(SIZE, X.data() + count == bsls::Util::addressOf(  *it));
        ASSERTV(SIZE, X.data() + count == bsls::Util::addressOf( *cit));
        ASSERTV(SIZE, X.data() + count == bsls::Util::addressOf(*ccit));
        ASSERTV(SIZE, SPEC[count] == TestFacility::getIdentifier(  *it));
        ASSERTV(SIZE, SPEC[count] == TestFacility::getIdentifier( *cit));
        ASSERTV(SIZE, SPEC[count] == TestFacility::getIdentifier(*ccit));

        ++count;
    }

    ASSERTV(SIZE, count, SIZE == count);
    ASSERTV(SIZE,   it == mX.end());
    ASSERTV(SIZE,  cit ==  X.end());
    ASSERTV(SIZE, ccit ==  X.cend());

    if (veryVerbose) printf("\t\t\tTesting reverse functions.\n");

    count = static_cast<int>(SIZE);

    RIter    rit = mX.rbegin();
    CRIter  crit =  X.rbegin();
    CRIter ccrit =  X.crbegin();

    for (;
         rit != mX.rend() && crit != X.rend() && ccrit != X.crend();
         ++rit, ++crit, ++ccrit) {
        ASSERTV(SIZE, X.data() + count - 1 == bsls::Util::addressOf(  *rit));
        ASSERTV(SIZE, X.data() + count - 1 == bsls::Util::addressOf( *crit));
        ASSERTV(SIZE, X.data() + count - 1 == bsls::Util::addressOf(*ccrit));
        ASSERTV(SIZE, SPEC[count - 1] == TestFacility::getIdentifier(  *rit));
        ASSERTV(SIZE, SPEC[count - 1] == TestFacility::getIdentifier( *crit));
        ASSERTV(SIZE, SPEC[count - 1] == TestFacility::getIdentifier(*ccrit));

        --count;
    }

    ASSERTV(SIZE, count,     0 == count);
    ASSERTV(SIZE,          rit == mX.rend());
    ASSERTV(SIZE,         crit ==  X.rend());
    ASSERTV(SIZE,        ccrit ==  X.crend());

}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase13()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase13();
    TestDriver<TYPE, 1>::testCase13();
    TestDriver<TYPE, 2>::testCase13();
    TestDriver<TYPE, 3>::testCase13();
    TestDriver<TYPE, 4>::testCase13();
    TestDriver<TYPE, 5>::testCase13();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase13()
{
    // ------------------------------------------------------------------------
    // TESTING 'fill'
    //
    // Concerns:
    //: 1 Fill function sets all elements in the array to the supplied value.
    //:
    //: 2 Fill works on default constructed arrays.
    //:
    //: 3 Fill works on arrays that already contain non-default values.
    //:
    //: 4 Fill can be called without effect on arrays of length 0.
    //
    // Plan:
    //: 1 Default construct an array X1.
    //:
    //: 2 Construct an array X2 from the 'SPEC'.
    //:
    //: 3 Use 'fill()' to modify value of each element of arrays with some
    //:   value 'v'.
    //:
    //: 4 Verify values of elements of arrays.  (C-1..4)
    //
    // Testing:
    //   void fill(const TYPE& value)
    // ------------------------------------------------------------------------

    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    const char *SPEC = "ABCDEFGHIJKLMNOPQRSTUV";

    const TestValues VALUES(SPEC);
    const size_t     VALUES_NUM = VALUES.size();

    for(size_t i = 0; i < VALUES_NUM; ++i) {

        // Array default constructed to test fill on.

        Obj mX1; const Obj& X1 = mX1;

        // Array generated from a spec to test fill on.

        Obj mX2; const Obj& X2 = gg(&mX2, SPEC);

        for (size_t j = 0; j < SIZE; ++j) {
            ASSERTV(SIZE, j, X2[j] ==  VALUES[j]);
        }

        mX1.fill(VALUES[i]);
        mX2.fill(VALUES[i]);


        for(size_t j = 0; j < SIZE; ++j) {
            ASSERTV(X1[j] == VALUES[i]);
            ASSERTV(X2[j] == VALUES[i]);
        }
    }
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase12()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase12();
    TestDriver<TYPE, 1>::testCase12();
    TestDriver<TYPE, 2>::testCase12();
    TestDriver<TYPE, 3>::testCase12();
    TestDriver<TYPE, 4>::testCase12();
    TestDriver<TYPE, 5>::testCase12();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase12()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE ASSIGNMENT
    //
    // Concerns:
    //: 1 The move assignment operator calls copy assignment operator for each
    //:   element of type without move assignment operator.
    //:
    //: 2 The move assignment operator calls move assignment operator for each
    //:   element of type having move assignment operator.
    //:
    //
    // Plan:
    //: 1 Construct two arrays (source and target) using default constructor
    //:   and populate them with different values.  Call the move-assignment
    //:   operator and verify the values of the target object.
    //:
    //: 2 Call the move-assignment operator for two arrays with elements of a
    //:   special type allowing to track down movement of each element and
    //:   verify that all elements of the source object are moved.  (C-2)
    //:
    //: 3 Call the move-assignment operator for two arrays with elements of a
    //:   special type without move assignment operator allowing to track down
    //:   copying of each element and verify that all elements of the source
    //:   object are copied.  (C-1)
    //
    // Testing:
    //   array& operator=(array&& other);
    // ------------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    const char *SPEC1 = "ABCDEFGHIJKLMNOPQRSTUV";
    const char *SPEC2 = "VUTSRQPONMLKJIHGFEDCBA";

    if(veryVerbose) printf("\t\t\tTesting basic behavior.\n");
    {
        // Create control objects.

        Obj mW1; const Obj& W1 = gg(&mW1, SPEC1);
        Obj mW2; const Obj& W2 = gg(&mW2, SPEC2);

        // Create source object.
        Obj mX; const Obj& X = gg(&mX, SPEC1);

        // Create target object.

        Obj mY; const Obj& Y = gg(&mY, SPEC2);

        ASSERTV(SIZE, W1 == X);
        ASSERTV(SIZE, W2 == Y);
        if (0 != SIZE) {
            ASSERTV(SIZE, W1 != Y);
        }

        // Move assignment.

        mY = MoveUtil::move(mX);

        // Verification.
        if (0 != SIZE) {
            ASSERTV(SIZE, W2 != Y);
        }
        ASSERTV(SIZE, W1 == Y);

    }

#if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VERSION > 1800
    // According to https://msdn.microsoft.com/en-us/library/hh567368.aspx
    // Visual Studio 2015 has the first MS compiler, that supports compiler
    // generated move assignment operator:
    //
    //    "Rvalue references v3.0" adds new rules to automatically generate
    //    move constructors and move assignment operators under certain
    //    conditions. This is implemented in Visual Studio 2015.
    //
    // So this test fails with Visual Studio 2013, because compiler generated
    // copy assignment operator is used.

    if (veryVerbose)
        printf("\t\t\tTesting that move assignment operator is called for"
               " elements.\n");
    {
        bsl::array<bsltf::MovableTestType, SIZE>        mX;
        const bsl::array<bsltf::MovableTestType, SIZE>&  X = gg(&mX, SPEC1);

        bsl::array<bsltf::MovableTestType, SIZE>        mY;
        const bsl::array<bsltf::MovableTestType, SIZE>&  Y = gg(&mY, SPEC2);

        for(size_t i = 0; i < SIZE; ++i) {
            ASSERTV(SIZE, bsltf::MoveState::e_NOT_MOVED  == X[i].movedFrom());
            ASSERTV(SIZE, bsltf::MoveState::e_NOT_MOVED  == X[i].movedInto());
            ASSERTV(SIZE, bsltf::MoveState::e_NOT_MOVED  == Y[i].movedFrom());
            ASSERTV(SIZE, bsltf::MoveState::e_NOT_MOVED  == Y[i].movedInto());
        }

        mY = MoveUtil::move(mX);

        for(size_t i = 0; i < SIZE; ++i) {
            ASSERTV(SIZE, bsltf::MoveState::e_MOVED      == X[i].movedFrom());
            ASSERTV(SIZE, bsltf::MoveState::e_NOT_MOVED  == X[i].movedInto());
            ASSERTV(SIZE, bsltf::MoveState::e_NOT_MOVED  == Y[i].movedFrom());
            ASSERTV(SIZE, bsltf::MoveState::e_MOVED      == Y[i].movedInto());
        }
    }
#endif

    if(veryVerbose)
        printf( "\t\t\tTesting that copy assignment operator is called for"
                " elements.\n");
    {
        bsl::array<CopyOnlyTestType, SIZE>        mX;
        const bsl::array<CopyOnlyTestType, SIZE>&  X = gg(&mX, SPEC1);

        bsl::array<CopyOnlyTestType, SIZE>        mY;
        const bsl::array<CopyOnlyTestType, SIZE>&  Y = gg(&mY, SPEC2);

        for(size_t i = 0; i < SIZE; ++i) {
            mX[i].resetCopyState();
            mY[i].resetCopyState();

            ASSERTV(SIZE, CopyState::e_NOT_COPIED  == X[i].copiedFrom());
            ASSERTV(SIZE, CopyState::e_NOT_COPIED  == X[i].copiedInto());
            ASSERTV(SIZE, CopyState::e_NOT_COPIED  == Y[i].copiedFrom());
            ASSERTV(SIZE, CopyState::e_NOT_COPIED  == Y[i].copiedInto());
        }

        mY = MoveUtil::move(mX);

        for(size_t i = 0; i < SIZE; ++i) {
            ASSERTV(SIZE, CopyState::e_COPIED      == X[i].copiedFrom());
            ASSERTV(SIZE, CopyState::e_NOT_COPIED  == X[i].copiedInto());
            ASSERTV(SIZE, CopyState::e_NOT_COPIED  == Y[i].copiedFrom());
            ASSERTV(SIZE, CopyState::e_COPIED      == Y[i].copiedInto());
        }
    }
#endif
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase11()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase11();
    TestDriver<TYPE, 1>::testCase11();
    TestDriver<TYPE, 2>::testCase11();
    TestDriver<TYPE, 3>::testCase11();
    TestDriver<TYPE, 4>::testCase11();
    TestDriver<TYPE, 5>::testCase11();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase11()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE CONSTRUCTOR
    //
    // Concerns:
    //: 1 The move constructor calls copy constructor for each element of type
    //:   without move constructor.
    //:
    //: 2 The move constructor calls move constructor for each element of type
    //:   having move constructor.
    //:
    //
    // Plan:
    //: 1 Move construct an array of a type with a default generated move
    //:   constructor.  Ensure the new array has the expected values.
    //:
    //: 2 Move construct an array of a special type allowing to track down
    //:   movement of each element and verify that all elements are moved.
    //:   (C-2)
    //:
    //: 3 Move construct an array of a special type without move constructor
    //:   allowing to track down copying of each element and verify that all
    //:   elements are copied.  (C-1)
    //
    // Testing:
    //   array(array&& original);
    // ------------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    const char *SPEC = "ABCDEFGHIJKLMNOPQRSTUV";

    if(veryVerbose) printf("\t\t\tTesting basic behavior.\n");
    {
        // Create control objects.

        Obj mW; const Obj& W = gg(&mW, SPEC);

        // Create source object.
        Obj mX; const Obj& X = gg(&mX, SPEC);

        ASSERTV(SIZE, W == X);

        // Move construction.

        Obj mY = MoveUtil::move(mX); const Obj& Y  = mY;

        // Verification.

        ASSERTV(SIZE, W == Y);
    }

#if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VERSION > 1800
    // According to https://msdn.microsoft.com/en-us/library/hh567368.aspx
    // Visual Studio 2015 has the first MS compiler, that supports compiler
    // generated move constructor:
    //
    //    "Rvalue references v3.0" adds new rules to automatically generate
    //    move constructors and move assignment operators under certain
    //    conditions. This is implemented in Visual Studio 2015.
    //
    // So this test fails with Visual Studio 2013, because compiler generated
    // copy constructor is used.

    if(veryVerbose)
        printf(
              "\t\t\tTesting that move constructor is called for elements.\n");
    {
        bsl::array<bsltf::MovableTestType, SIZE>        mX;
        const bsl::array<bsltf::MovableTestType, SIZE>&  X = gg(&mX, SPEC);

        for(size_t i = 0; i < SIZE; ++i) {
            ASSERTV(bsltf::MoveState::e_NOT_MOVED  == X[i].movedFrom());
            ASSERTV(bsltf::MoveState::e_NOT_MOVED  == X[i].movedInto());
        }

        bsl::array<bsltf::MovableTestType, SIZE>        mY =
                                                            MoveUtil::move(mX);
        const bsl::array<bsltf::MovableTestType, SIZE>&  Y = mY;

        for(size_t i = 0; i < SIZE; ++i) {
            ASSERTV(bsltf::MoveState::e_MOVED      == X[i].movedFrom());
            ASSERTV(bsltf::MoveState::e_NOT_MOVED  == X[i].movedInto());
            ASSERTV(bsltf::MoveState::e_NOT_MOVED  == Y[i].movedFrom());
            ASSERTV(bsltf::MoveState::e_MOVED      == Y[i].movedInto());
        }
    }
#endif

    if(veryVerbose)
        printf(
              "\t\t\tTesting that copy constructor is called for elements.\n");
    {
        bsl::array<CopyOnlyTestType, SIZE>        mX;
        const bsl::array<CopyOnlyTestType, SIZE>&  X = gg(&mX, SPEC);

        for(size_t i = 0; i < SIZE; ++i) {
            mX[i].resetCopyState();
            ASSERTV(CopyState::e_NOT_COPIED  == X[i].copiedFrom());
            ASSERTV(CopyState::e_NOT_COPIED  == X[i].copiedInto());
        }

        bsl::array<CopyOnlyTestType, SIZE>        mY = MoveUtil::move(mX);
        const bsl::array<CopyOnlyTestType, SIZE>&  Y = mY;

        for(size_t i = 0; i < SIZE; ++i) {
            ASSERTV(CopyState::e_COPIED      == X[i].copiedFrom());
            ASSERTV(CopyState::e_NOT_COPIED  == X[i].copiedInto());
            ASSERTV(CopyState::e_NOT_COPIED  == Y[i].copiedFrom());
            ASSERTV(CopyState::e_COPIED      == Y[i].copiedInto());
        }
    }
#endif
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase9()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase9();
    TestDriver<TYPE, 1>::testCase9();
    TestDriver<TYPE, 2>::testCase9();
    TestDriver<TYPE, 3>::testCase9();
    TestDriver<TYPE, 4>::testCase9();
    TestDriver<TYPE, 5>::testCase9();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase9()
{
    // ------------------------------------------------------------------------
    // TESTING COPY ASSIGNMENT
    //
    // Concerns:
    //: 1 The value of any array can be assigned to any other array of the same
    //:   type and size as long as the element type is assignable.
    //:
    //: 2 The source object must not be affected by the operation.
    //:
    //: 3 The source object going out of scope has no effect on the value of
    //:   the target object after the assignment and vice versa.
    //:
    //: 4 Modifications to the source object have no effect on the value of the
    //:   target object after the assignment and vice versa.
    //:
    //: 5 Aliasing ('x = x'): The assignment operator must always work -- even
    //:   when the target object and the source object are the same object.
    //
    // Plan:
    //: 1 Construct two pairs (object - source) of arrays having different
    //:   values and assign them in pairs.
    //:
    //: 2 Verify that returned values point to target objects, and the values
    //:   of target object are become equal to the source objects.  (C-1)
    //:
    //: 3 In the first pair modify and then destroy the target object and
    //:   verify that the source object remains unchanged.  (C-2..3)
    //:
    //: 4 In the second pair modify and then let the source object go out of
    //:   scope and verify that the target object remains unchanged.  (C-4)
    //:
    //: 5 Create an array and assign it to itself.  Verify that its value
    //:   remains unchanged.  (C-5)
    //
    // Testing:
    //   array& operator=(const array& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    const char *SPEC1 = "ABCDEFGHIJKLMNOPQRSTUV";
    const char *SPEC2 = "VUTSRQPONMLKJIHGFEDCBA";

    // Create control objects.

    Obj mW1; const Obj& W1 = gg(&mW1, SPEC1);
    Obj mW2; const Obj& W2 = gg(&mW2, SPEC2);

    // Create source objects.

    bsls::ObjectBuffer<Obj>       xBuf1;
    new (xBuf1.buffer()) Obj();
    bslma::DestructorProctor<Obj> proctorX1(xBuf1.address());
    Obj&                          mX1 = xBuf1.object();
    const Obj&                    X1 = gg(&mX1, SPEC1);

    Obj mX2; const Obj& X2 = gg(&mX2, SPEC2);

    ASSERTV(SIZE, W1 == X1);
    ASSERTV(SIZE, W2 == X2);

    {
        Obj mY1; const Obj& Y1 = gg(&mY1, SPEC2);
        Obj mY2; const Obj& Y2 = gg(&mY2, SPEC1);

        ASSERTV(SIZE, W2 == Y1);
        ASSERTV(SIZE, W1 == Y2);

        // Copy-assignment.

        Obj *mR1 = &(mY1 = X1);
        Obj *mR2 = &(mY2 = X2);

        // Verify the returned value.

        ASSERTV(SIZE, &Y1 == mR1);
        ASSERTV(SIZE, &Y2 == mR2);

        // Verify that values of target objects are the same as that of the
        // source objects.

        ASSERTV(SIZE, X1 == Y1);
        ASSERTV(SIZE, X2 == Y2);

        // Verify that values of source objects remain unchanged.

        ASSERTV(SIZE, W1 == X1);
        ASSERTV(SIZE, W2 == X2);

        if (0 != SIZE) {
            // Verify that subsequent changes in the source object have no
            // effect on the copy-constructed object.

            mX1[0] = TestFacility::create<TYPE>(SPEC2[0]);

            ASSERTV(SIZE, W1 != X1);
            ASSERTV(SIZE, W1 == Y1);

            // Verify that subsequent changes in the copy-constructed object
            // have no effect on the source object.

            mY2[0] = TestFacility::create<TYPE>(SPEC1[0]);

            ASSERTV(SIZE, W2 != Y2);
            ASSERTV(SIZE, W2 == X2);
        }

        // Verify that destruction of the source object have no effect on the
        // copy-constructed object.

        proctorX1.release();
        xBuf1.address()->~Obj();
        ASSERTV(SIZE, W1 == Y1);
    }

    // Verify that destruction of the copy-constructed object have no effect on
    // the source object.

    ASSERTV(SIZE, W2 == X2);

    // Self-assignment.
    {
        Obj mW; const Obj& W = gg(&mW, SPEC1);
        Obj mX; const Obj& X = gg(&mX, SPEC1);

        ASSERTV(SIZE, W == X);

        // Copy-assignment.

        Obj *mR = &(mX = X);

        // Verify the returned value.

        ASSERTV(SIZE, &X == mR);

        // Verify that value of object remain unchanged.

        ASSERTV(SIZE, W == X);
    }
}

template <size_t SIZE>
void testCase8_elementADL()
{
    // ------------------------------------------------------------------------
    // TESTING SWAP ELEMENT ADL
    //
    // Concerns:
    //: 1 The element's 'swap' is discovered through ADL when swapping arrays.
    //
    // Plan:
    //: 1 Construct arrays 'X1' and 'X2' of 'CountedSwap' elements.
    //:
    //: 3 Use the free 'swap' function to swap X1 and X2, then confirm that
    //:   number of ADL-discovered swaps recorded by 'CountedSwap' equals to
    //:   array size.
    //:
    //: 4 Use the member 'swap' function to swap X1 and X2, then confirm that
    //:   number of ADL-discovered swaps recorded by 'CountedSwap' equals to
    //:   array size.
    //
    // Testing:
    //   void swap(array&);
    //   void swap(array<TYPE,SIZE>& lhs, array<TYPE,SIZE>& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\tFor array of length " ZU "\n", SIZE);

    typedef bsl::array<CountedSwap, SIZE> Obj;

    const char *SPEC1 = "ABCDEFGHIJKLMNOPQRSTUV";
    const char *SPEC2 = "VUTSRQPONMLKJIHGFEDCBA";

    Obj mX1; gg(&mX1, SPEC1);
    Obj mX2; gg(&mX2, SPEC2);

    if (veryVerbose)
        printf("\t\t\tTesting element ADL of free 'swap' function\n");

    CountedSwap::resetNumSwaps();
    swap(mX1, mX2);
    ASSERTV(SIZE, SIZE == CountedSwap::numSwaps());

    if (veryVerbose)
        printf("\t\t\tTesting element ADL of member 'swap' function\n");

    CountedSwap::resetNumSwaps();
    mX1.swap(mX2);
    ASSERTV(SIZE, SIZE == CountedSwap::numSwaps());
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase8()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase8();
    TestDriver<TYPE, 1>::testCase8();
    TestDriver<TYPE, 2>::testCase8();
    TestDriver<TYPE, 3>::testCase8();
    TestDriver<TYPE, 4>::testCase8();
    TestDriver<TYPE, 5>::testCase8();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase8()
{
    // ------------------------------------------------------------------------
    // TESTING SWAP
    //
    // Note that self swap is undefined behavior according to a strict
    // interpretation of the ISO standard.
    //
    // Concerns:
    //: 1 Both functions exchange the values of the two supplied objects.
    //:
    //: 2 The free 'swap' function is discoverable through ADL.
    //:
    //
    // Plan:
    //: 1 Construct arrays 'W1' (control) and 'X1' based on the same spec
    //:   string.
    //:
    //: 2 Construct arrays 'W2' (control) and 'X2' constructed from the same
    //:   spec string different from the P-1 spec.
    //:
    //: 3 Use the free 'swap' function to swap X1 and X2, then confirm that
    //:   'W1 == X2' and 'W2 == X1' respectively.
    //:
    //: 4 Use the member 'swap' function to swap X1 and X2, then confirm that
    //:   'W1 == X1' and 'W2 == X2' respectively.
    //
    // Testing:
    //   void swap(array&);
    //   void swap(array<TYPE,SIZE>& lhs, array<TYPE,SIZE>& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\t\tof length " ZU "\n", SIZE);


    const char *SPEC1 = "ABCDEFGHIJKLMNOPQRSTUV";
    const char *SPEC2 = "VUTSRQPONMLKJIHGFEDCBA";

    // Create control objects.

    Obj mW1; const Obj& W1 = gg(&mW1, SPEC1);
    Obj mW2; const Obj& W2 = gg(&mW2, SPEC2);

    Obj mX1; const Obj& X1 = gg(&mX1, SPEC1);
    Obj mX2; const Obj& X2 = gg(&mX2, SPEC2);

    ASSERTV(SIZE, W1 == X1);
    ASSERTV(SIZE, W2 == X2);

    if (veryVerbose) printf("\t\t\tTesting free 'swap' function\n");

    swap(mX1, mX2);
    ASSERTV(SIZE, W1 == X2);
    ASSERTV(SIZE, W2 == X1);

    if (veryVerbose) printf("\t\t\tTesting member 'swap' function\n");

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
    const bool isSwapNoexcept = bsl::is_nothrow_swappable<TYPE>::value;
    if (0 != SIZE || isSwapNoexcept) {
        ASSERT(isSwapNoexcept == noexcept(mX1.swap(mX2)));
    }
    else  {
        // Some standard library implementations may mark the swapping of a
        // zero length array as unconditionally noexcept, so we simply ignore
        // this case
    }
#endif

    mX1.swap(mX2);
    ASSERTV(SIZE, W1 == X1);
    ASSERTV(SIZE, W2 == X2);
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase7()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase7();
    TestDriver<TYPE, 1>::testCase7();
    TestDriver<TYPE, 2>::testCase7();
    TestDriver<TYPE, 3>::testCase7();
    TestDriver<TYPE, 4>::testCase7();
    TestDriver<TYPE, 5>::testCase7();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR
    //
    // Concerns:
    //: 1 The new object's value is the same as that of the original object
    //:   (relying on the equality operator).
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 3 The value of the original object is left unaffected.
    //:
    //: 4 Changes to or the destruction of the original object do not affect
    //:   the copy-constructed object
    //:
    //: 5 Changes to or the destruction of the copy-constructed object do not
    //:   affect the original object
    //
    // Plan:
    //: 1 Construct two pairs of array(W1, X1 and W2, X2) based on the 'SPEC1'
    //:   and 'SPEC2' respectively.
    //:
    //: 2 Copy-construct arrays Y1 and Y2 from the arrays X1 and X2
    //:   respectively.
    //:
    //: 3 Verify that the array X1 is equal to Y1 and X2 is equal to Y2.
    //:   (C-1..2)
    //:
    //: 4 Verify that the array W1 is equal to X1 and W2 is equal to X2.  (C-3)
    //:
    //: 5 Modify the source array X1 and verify that W1 and Y1 are still equal.
    //:
    //: 6 Modify the copy-constructed array Y2 and check that W2 and X2 are
    //:   still equal.
    //:
    //: 7 Destroy X1 and verify that W1 and Y1 are still equal.  (C-4)
    //:
    //: 8 Allow Y2 to go out of scope and verify that W2 and X2 are still
    //:   equal.  (C-5)
    //
    // Testing:
    //   array<TYPE,SIZE>(const array<TYPE,SIZE>& original);
    // ------------------------------------------------------------------------

    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    const char *SPEC1 = "ABCDEFGHIJKLMNOPQRSTUV";
    const char *SPEC2 = "VUTSRQPONMLKJIHGFEDCBA";

    // Create control objects.

    Obj mW1; const Obj& W1 = gg(&mW1, SPEC1);
    Obj mW2; const Obj& W2 = gg(&mW2, SPEC2);

    // Create source objects.

    bsls::ObjectBuffer<Obj>       xBuf1;
    new (xBuf1.buffer()) Obj();
    bslma::DestructorProctor<Obj> proctorX1(xBuf1.address());
    Obj&                          mX1 = xBuf1.object();
    const Obj&                    X1 = gg(&mX1, SPEC1);

    Obj mX2; const Obj& X2 = gg(&mX2, SPEC2);

    ASSERTV(SIZE, W1 == X1);
    ASSERTV(SIZE, W2 == X2);

    {
        Obj mY1 = Obj(X1); const Obj& Y1 = mY1;
        Obj mY2 = Obj(X2); const Obj& Y2 = mY2;

        // Verify that values of new objects are the same as that of the
        // original objects.

        ASSERTV(SIZE, X1 == Y1);
        ASSERTV(SIZE, X2 == Y2);

        // Verify that values of original objects remain unchanged.

        ASSERTV(SIZE, W1 == X1);
        ASSERTV(SIZE, W2 == X2);

        if (0 != SIZE) {
            // Verify that subsequent changes in the source object have no
            // effect on the copy-constructed object.

            mX1[0] = TestFacility::create<TYPE>(SPEC2[0]);

            ASSERTV(SIZE, W1 != X1);
            ASSERTV(SIZE, W1 == Y1);

            // Verify that subsequent changes in the copy-constructed object
            // have no effect on the source object.

            mY2[0] = TestFacility::create<TYPE>(SPEC1[0]);

            ASSERTV(SIZE, W2 != Y2);
            ASSERTV(SIZE, W2 == X2);
        }

        // Verify that destruction of the source object have no effect on the
        // copy-constructed object.

        proctorX1.release();
        xBuf1.address()->~Obj();
        ASSERTV(SIZE, W1 == Y1);
    }

    // Verify that destruction of the copy-constructed object have no effect on
    // the source object.

    ASSERTV(SIZE, W2 == X2);
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase6()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase6();
    TestDriver<TYPE, 1>::testCase6();
    TestDriver<TYPE, 2>::testCase6();
    TestDriver<TYPE, 3>::testCase6();
    TestDriver<TYPE, 4>::testCase6();
    TestDriver<TYPE, 5>::testCase6();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase6()
{
    // ------------------------------------------------------------------------
    // TESTING COMPARISON OPERATORS
    //
    // Concerns:
    //: 1 Arrays constructed with the same values are returned as equal.
    //:
    //: 2 Unequal arrays are always returned as unequal
    //
    // Plan:
    //: 1 Default construct two instances of the array having certain (template
    //:   parameter) SIZE.
    //:
    //: 2 Populate the arrays based on two SPECs using the 'gg' function.
    //:
    //: 3 Verify that 'operator==' returns 'true' and 'operator!=' returns
    //:   'false' when comparing equal arrays (based on the same spec) or the
    //:   same arrays.  (C-1)
    //:
    //: 4 Verify that 'operator==' returns false and 'operator!=' returns
    //:   'true' when comparing unequal arrays.  (C-2)
    //
    // Testing:
    //   bool operator==(const array& rhs, const array& lhs);
    // ------------------------------------------------------------------------

    using bsls::NameOf;

    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    const int NUM_SPECS = 6;

    static const struct {
        int         d_line;               // source line number
        const char *d_spec_p[NUM_SPECS];  // specification strings
    } DATA[] = {
        //line   spec 1   spec 2   spec 3   spec 4   spec 5   spec 6
        //----   -------  -------  -------  -------  -------  -------
        { L_,  { "",      "",      "",      "",      "",      ""     } },  // 0
        { L_,  { "A",     "B",     "C",     "D",     "E",     "F"    } },  // 1
        { L_,  { "AA",    "AB",    "BA",    "BB",    "AC",    "CA"   } },  // 2
        { L_,  { "AAA",   "AAB",   "ABA",   "BAA",   "BBB",   "AAC"  } },  // 3
        { L_,  { "AAAA",  "AAAB",  "AABA",  "ABAA",  "BAAA",  "BBBB" } },  // 4
        { L_,  { "AAAAA", "AAAAB", "AAABA", "AABAA", "ABAAA", "BAAAA"} },  // 5
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    ASSERTV("This size is not supported", SIZE, NUM_DATA > SIZE);

    for (size_t i = 0; i < NUM_SPECS; ++i) {
        const char *const SPEC1 = DATA[SIZE].d_spec_p[i];

        Obj mX1; const Obj& X1 = gg(&mX1, SPEC1);

        for (size_t j = 0; j < NUM_SPECS; ++j) {
            const char* const SPEC2 = DATA[SIZE].d_spec_p[j];

            if (veryVerbose)
                printf("\t\t\tComparing %s and %s.\n", SPEC1, SPEC2);

            Obj mX2; const Obj& X2 = gg(&mX2, SPEC2);

            if (0 == strcmp(SPEC1, SPEC2)) {
                ASSERTV(NameOf<TYPE>().name(), SPEC1, SPEC2,
                        testEqualityComparison(X1, X2, true));
            }
            else  {
                ASSERTV(NameOf<TYPE>().name(), SPEC1, SPEC2,
                        testEqualityComparison(X1, X2, false));
            }
        }
    }
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase4()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase4();
    TestDriver<TYPE, 1>::testCase4();
    TestDriver<TYPE, 2>::testCase4();
    TestDriver<TYPE, 3>::testCase4();
    TestDriver<TYPE, 4>::testCase4();
    TestDriver<TYPE, 5>::testCase4();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase4()
{
    // ------------------------------------------------------------------------
    // TESTING BASIC ACCESSORS
    //
    // Concerns:
    //: 1 Each accessor returns the value of the correct property of the
    //:   object.
    //:
    //: 2 Each accessor method is declared 'const'.
    //:
    //: 3 Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Use 'gg' function to populate an object based on the spec string and
    //:   obtain constant reference to this object.
    //:
    //: 2 Using 'size()' verify the object contains the expected number of
    //:   elements.
    //:
    //: 3 Iterate through all elements using 'operator[]' and verify the values
    //:   are as expected.  (C-1..2)
    //:
    //: 4 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values, but not triggered for
    //:   adjacent valid ones.  (C-3)
    //
    // Testing:
    //   reference operator[](size_type position) const;
    //   size_type size();
    // ------------------------------------------------------------------------

    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    const char*      SPEC = "ABCDEFGHIJKLMNOPQRSTUV";
    const TestValues EXP(SPEC);

    if (veryVerbose) printf("\t\t\tTesting 'operator[]'.\n");

    {
        Obj        mX;
        const Obj& X = gg(&mX, SPEC);

        for (size_t i = 0; i < SIZE; ++i) {
            TYPE&       valueRef      = mX[i];
            const TYPE& constValueRef =  X[i];

            ASSERTV(i, mX.data() + i == bsls::Util::addressOf(     valueRef));
            ASSERTV(i,  X.data() + i == bsls::Util::addressOf(constValueRef));
            ASSERTV(i, EXP[i]        ==                            valueRef );
            ASSERTV(i, EXP[i]        ==                       constValueRef );
        }
    }

    if (veryVerbose) printf ("\t\t\tTesting 'size()'.\n");
    {
        Obj        mX;
        const Obj& X = mX;

        ASSERT(SIZE == X.size());
    }

    if (veryVerbose) printf("\t\t\tNegative Testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        Obj        mX;
        const Obj& X = gg(&mX, SPEC);

#if !defined(BSLSTL_ARRAY_DISABLE_CONSTEXPR_CONTRACTS) &&                     \
    !defined(BSLSTL_ARRAY_IS_ALIASED)
        ASSERT_FAIL((void) X[SIZE    ]);
#endif
        if (0 != SIZE) {
            ASSERT_PASS((void) X[SIZE - 1]);
        }
    }
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase3()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase3();
    TestDriver<TYPE, 1>::testCase3();
    TestDriver<TYPE, 2>::testCase3();
    TestDriver<TYPE, 3>::testCase3();
    TestDriver<TYPE, 4>::testCase3();
    TestDriver<TYPE, 5>::testCase3();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase3()
{
    // ------------------------------------------------------------------------
    // TESTING GENERATOR FUNCTIONS
    //   Ensure that the 'gg' and 'ggg' function work as expected and that
    //   valid generator syntax produced the expected results.
    //
    // Concerns:
    //: 1 Valid generator syntax produces expected results.
    //:
    //: 2 Invalid syntax is detected and reported.
    //
    // Plan:
    //: 1 For the sequence of valid 'spec' values with length equal to the
    //:   length of the array type to be tested, adjust a default constructed
    //:   array using the 'gg' function.
    //:
    //: 2 Verify that the element at any index is equal to the element at the
    //:   same index in the corresponding test values array 'EXP' which was
    //:   constructed by the same 'spec' string.  (C-1)
    //:
    //: 3 For the sequence of invalid 'spec' values with length equal to the
    //:   length of the array type to be tested, adjust an array using the 'gg'
    //:   function.
    //:
    //: 4 Verify that invalid symbol is determined correctly and array
    //:   adjustment is finished at that symbol.  (C-2)
    //
    // Testing:
    //   int ggg(array<TYPE,SIZE> *object, const char *spec, bool verboseFlag);
    //   array<TYPE,SIZE>& gg(array<TYPE,SIZE> *object, const char *spec);
    // ------------------------------------------------------------------------

    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    if (veryVerbose) printf("\t\t\tTesting generator on valid specs.\n");
    {

        const char*      SPEC = "ABCDEFGHIJKLMNOPQRSTUV";
        const TestValues EXP(SPEC);

        Obj        mX;
        const Obj& X = gg(&mX, SPEC);

        ASSERTV(bsls::Util::addressOf(mX) == bsls::Util::addressOf(X));
        for (size_t i = 0; i < SIZE; ++i) {
            ASSERTV(i, EXP[i] == X.data()[i]);
        }
    }

    if (veryVerbose) printf("\t\t\tTesting generator on invalid specs.\n");
    {
        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_index;    // offending character index
        } DATA[] = {
            //line  spec     index
            //----  -------- -----
            // length = 0
            { L_,   "",      -1,   },  // control

            // length = 1
            { L_,   " ",      0,   },
            { L_,   ".",      0,   },
            { L_,   "E",     -1,   },  // control
            { L_,   "Z",      0,   },

            // length = 2
            { L_,   "AE",    -1,   },  // control
            { L_,   "aE",     0,   },
            { L_,   "Ae",     1,   },
            { L_,   ".~",     0,   },
            { L_,   "~!",     0,   },
            { L_,   "  ",     0,   },

            // length = 3
            { L_,   "ABC",   -1,   },  // control
            { L_,   " BC",    0,   },
            { L_,   "A C",    1,   },
            { L_,   "AB ",    2,   },
            { L_,   "?#:",    0,   },
            { L_,   "   ",    0,   },

            // length = 4
            { L_,   "ABCD",  -1,   },  // control
            { L_,   "aBCD",   0,   },
            { L_,   "ABcD",   2,   },
            { L_,   "ABCd",   3,   },
            { L_,   "AbCd",   1,   },

            // length = 5
            { L_,   "ABCDE",  -1,   },  // control
            { L_,   "aBCDE",   0,   },
            { L_,   "ABcDE",   2,   },
            { L_,   "ABCdE",   3,   },
            { L_,   "ABCDe",   4,   }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        const char       *ORIG_SPEC = "VUTSRQPONMLKJIHGFEDCBA";
        const TestValues  ORIG_EXP(ORIG_SPEC);

        for (size_t ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const int         INDEX  = DATA[ti].d_index;
            const int         LENGTH = static_cast<int>(strlen(SPEC));
            const TestValues  EXP(SPEC);

            if (LENGTH == SIZE) {
                Obj        mX;
                const Obj& X = gg(&mX, ORIG_SPEC);

                for (size_t tj = 0; tj < SIZE; ++tj) {
                    ASSERTV(tj, ORIG_EXP[tj] == X.data()[tj]);
                }

                if (veryVerbose) printf("\t\t\tSpec = \"%s\"\n", SPEC);

                int result = ggg(&mX, SPEC, veryVerbose);
                ASSERTV(LINE, INDEX, result, SPEC, INDEX == result);

                if (-1 == INDEX) {
                    for (size_t tj = 0; tj < SIZE; ++tj) {
                        ASSERTV(tj, EXP[tj] == X.data()[tj]);
                    }
                }
                else {
                    for (int tj = 0; tj < INDEX; ++tj) {
                        ASSERTV(tj, EXP[tj] == X.data()[tj]);
                    }
                    for (int tj = INDEX; tj < static_cast<int>(SIZE); ++tj) {
                        ASSERTV(tj, 0 <= tj);  // quiet 32-bit C++03 warning
                        ASSERTV(tj, ORIG_EXP[tj] == X.data()[tj]);
                    }
                }
            }
        }
    }
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase2()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase2();
    TestDriver<TYPE, 1>::testCase2();
    TestDriver<TYPE, 2>::testCase2();
    TestDriver<TYPE, 3>::testCase2();
    TestDriver<TYPE, 4>::testCase2();
    TestDriver<TYPE, 5>::testCase2();
}

template<class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase2()
{
    // --------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS
    //
    // Concerns:
    //: 1 An object created with the default constructor defalt constructs a
    //:   number of elements equal to the size of the array with the exception
    //:   of 0 length arrays that will construct 1 element.
    //:
    //: 2 Destructor calls the destructor of every element.
    //:
    //: 3 Elements can be set using 'operator[]' for any type that supports the
    //:   assignment operator.
    //:
    //: 4 Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 For each array of different length:
    //:
    //:   1 Use default constructor to create an array of elements of special
    //:     type, counting the number of constructor and destructor
    //:     invocations.
    //:
    //:   2 Verify the size of the constructed array, using 'size()', that
    //:     hasn't been tested yet.
    //:
    //:   3 Verify that the correct number of elements was constructed.  (C-1)
    //:
    //:   4 Verify all elements are deleted when the array is destroyed.  (C-2)
    //:
    //:   5 Create an array and modify it using 'operator[]'.
    //:
    //:   6 Verify that the value of each element is correct by inspecting
    //:     d_data.  (C-3)
    //:
    //:   7 Verify that, in appropriate build modes, defensive checks are
    //:     triggered for invalid attribute values, but not triggered for
    //:     adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-4)
    //
    // Testing:
    //   array();
    //   ~array();
    //   reference operator[](size_type position);
    // --------------------------------------------------------------------

    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    typedef CountedDefault<TYPE>         VALUE_TYPE;
    typedef bsl::array<VALUE_TYPE, SIZE> OBJ;

    if (veryVerbose) printf("\t\t\tTesting defasult constructor.\n");
    {
        ASSERT(VALUE_TYPE::numConstructed() == 0);

        OBJ        mX1;
        const OBJ& X1 = mX1;

        ASSERT(SIZE == X1.size());
#if !defined(BSLSTL_ARRAY_IS_ALIASED)
        ASSERTV(VALUE_TYPE::numConstructed(),
                0 == SIZE ? 1    == VALUE_TYPE::numConstructed()
                          : SIZE == VALUE_TYPE::numConstructed());
#else
        ASSERTV(VALUE_TYPE::numConstructed(),
                0 == SIZE ? 1    >= VALUE_TYPE::numConstructed()
                          : SIZE == VALUE_TYPE::numConstructed());
#endif

        Obj        mX2;
        const Obj& X2 = mX2;

        ASSERT(SIZE == X2.size());

    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) printf("\t\t\tTesting destructor.\n");

    ASSERTV(VALUE_TYPE::numConstructed(), 0 == VALUE_TYPE::numConstructed());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (0 != SIZE) {
        if (veryVerbose) printf("\t\t\tTesting operator[].\n");
        {
            const char       *SPEC1 = "ABCDEFGHIJKLMNOPQRSTUV";
            const char       *SPEC2 = "VUTSRQPONMLKJIHGFEDCBA";
            const TestValues  EXP1(SPEC1);
            const TestValues  EXP2(SPEC2);

            Obj        mX;
            const Obj& X = mX;

            for (size_t i = 0; i < SIZE; ++i) {

                mX[i] = TestFacility::create<TYPE>(SPEC1[i]);
            }

            for (size_t i = 0; i < SIZE; ++i) {
                ASSERTV(SIZE, i, EXP1[i] == X.data()[i]);
            }

            if (veryVerbose) printf(
                 "\t\t\t\tTesting modifying container values from %s to %s.\n",
                 SPEC1,
                 SPEC2);

            for (size_t i = 0; i < SIZE; ++i) {
                for (size_t j = i; j < SIZE; ++j) {
                    ASSERTV(SIZE, i, j, EXP1[j] == X.data()[j]);
                }
                mX[i] = TestFacility::create<TYPE>(SPEC2[i]);
                for (size_t j = 0; j < i; ++j) {
                    ASSERTV(SIZE, i, j, EXP2[j] == X.data()[j]);
                }

                ASSERTV(SIZE, i, EXP2[i] == X.data()[i]);

                for (size_t j = i + 1; j < SIZE; ++j) {
                    ASSERTV(SIZE, i, j, EXP1[j] == X.data()[j]);
                }
            }
        }
    }


    if (veryVerbose) printf("\t\t\tNegative Testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        Obj        mX;

#if !defined(BSLSTL_ARRAY_IS_ALIASED)
        ASSERT_FAIL((void)mX[SIZE    ]);
#endif
        if (0 != SIZE) {
            ASSERT_PASS((void)mX[SIZE - 1]);
        }
    }
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase1()
{
    using bsls::NameOf;

    if (verbose) printf("\tFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase1();
    TestDriver<TYPE, 1>::testCase1();
    TestDriver<TYPE, 2>::testCase1();
    TestDriver<TYPE, 3>::testCase1();
    TestDriver<TYPE, 4>::testCase1();
}

template<class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase1()
{
    // --------------------------------------------------------------------
    // BREATHING TEST
    //  This case exercises (buit does not fully test) basic functionality.
    //
    // Concerns:
    //: 1 The class is sufficiently functional to enable comprehensive testing
    //:   in subsequent test cases.
    //
    // Plan:
    //: 1 Create an object 'x' (default ctor).           { x:D           }
    //: 2 Test mutation methods on 'x'.                  { x:D           }
    //: 3 Create an object 'y' (copy from 'x').          { x:D y:V       }
    //: 4 Test comparison methods on 'x' and 'y'.        { x:D y:V       }
    //
    // Testing:
    //   BREATHING TEST
    // --------------------------------------------------------------------

    if (verbose) printf("\t\tof length " ZU "\n", SIZE);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) printf("\t\t1. Create an object 'x' (default ctor)."
                            "\t\t{ x:D           }\n");

    Obj mX; const Obj& X = mX;

    ASSERT(SIZE == mX.size());
    ASSERT(SIZE == mX.max_size());
    ASSERT(SIZE == X.size());
    ASSERT(SIZE == X.max_size());

    if (0 == SIZE)
        return;                                                       // RETURN

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) printf("\t\t2. Test mutation methods on 'x' "
                            "\t\t{ x:D y:V        }\n");

    for (size_t i = 0; i < SIZE; ++i) {
        mX[i] = static_cast<ValueType>(i);
    }

    ASSERT(mX[SIZE-1] == SIZE-1);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) printf("\t\t3. Create an object 'y' (copy from 'x') "
                            "\t\t{ x:D y:V        }\n");

    Obj mY(mX); const Obj& Y = mY;

    for (size_t i = 0; i < SIZE; ++i) {
        ASSERT(Y[i] == X[i]);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) printf("\t\t4. Test comparison methods on 'x' and 'y'"
                            "\t\t{ x:D y:V        }\n");

    ASSERT(Y == X);
    ASSERT(Y <= X);
    ASSERT(Y >= X);
    ASSERT(!(Y < X));
    ASSERT(!(Y > X));
}

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

// BDE_VERIFY pragma: pop

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

// BDE_VERIFY pragma: push   // Usage examples relax rules for doc clarity
// BDE_VERIFY pragma: -FD01  // Function declaration requires contract
namespace UsageExample {
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Returning an array from a function
///- - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to define a function that will return an array of 'float's.
// If a raw array were used, the size would need to be tracked separately
// because raw arrays decay to pointers when passed as function arguments, or
// returned by-value.  'bsl::array' does not decay, and so provides a simple
// solution to this problem.
//..
typedef bsl::array<float, 3> Point;

Point createPoint(float f1, float f2, float f3)
{
    bsl::array<float, 3> ret = {f1, f2, f3};
    return ret;
}
//..
// Create a bsl::array object containing three values set to the specified
// 'f1', 'f2', 'f3'.
//..
void usageExample()
{
    Point p1 = createPoint(1.0, 1.0, 1.0);
    Point p2 = createPoint(2.0, 2.0, 2.0);
    Point p3 = createPoint(3.0, 3.0, 3.0);

    bsl::array<Point, 3> points = {p1, p2, p3};

    for(size_t i = 0; i < points.size(); ++i) {
        for(size_t j = 0; j < points[i].size(); ++j) {
            points[i][j] *= 2.0f;
        }
    }
}
//..
// Use the createPoint function to generate three arrays of floats.  The arrays
// are returned by copy and the 'size()' member function is used to access the
// size of the arrays that could not be done with a raw array.

}  // close namespace UsageExample

// BDE_VERIFY pragma: pop  // End of usage example-example relaxed rules

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
struct TestDeductionGuides {
    // This struct provides a namespace for functions testing deduction guides.
    // The tests are compile-time only; it is not necessary that these routines
    // be called at run-time.  Note that the following constructors do not have
    // associated deduction guides because the template parameters for
    // 'bsl::array' cannot be deduced from the constructor parameters.
    //..
    // array()
    //..

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

    static void SimpleConstructors ()
        // Test that constructing a 'bsl::array' from various combinations of
        // arguments deduces the correct type.
        //..
        // array(/*braces*/);
        // array(const array<T>&);
        // array(      array<T>&&);
        //..
    {
        bsl::array a1a{12, 23, 34, 45, 56};
        bsl::array a1b{12L, 23L, 34L, 45L};
        ASSERT_SAME_TYPE(decltype(a1a), bsl::array<int, 5>);
        ASSERT_SAME_TYPE(decltype(a1b), bsl::array<long, 4>);

        bsl::array<long long, 6> a2{1LL, 2LL, 3LL, 4LL, 5LL, 6LL};
        bsl::array               a2a(a2);
        ASSERT_SAME_TYPE(decltype(a2a), bsl::array<long long, 6>);

        bsl::array<double, 3> a3{1.0, 2.0, 3.0};
        bsl::array            a3a(std::move(a3));
        ASSERT_SAME_TYPE(decltype(a3a), bsl::array<double, 3>);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Compile-fail tests
// #define BSLSTL_ARRAY_COMPILE_FAIL_MIXED_TYPES_LIST
#if defined BSLSTL_ARRAY_COMPILE_FAIL_MIXED_TYPES_LIST
        bsl::array arr {1.2, 3, 4};
        // This should fail to compile (not all values in the list are the same
        // type).
#endif
    }

#undef ASSERT_SAME_TYPE
};
#endif  // BSLS_COMPILERFEATURES_SUPPORT_CTAD

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    (void)     veryVeryVerbose;  // suppress compiler warning
    (void) veryVeryVeryVerbose;  // suppress compiler warning

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    bslma::TestAllocator         ga("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&ga);

    printf("TEST " __FILE__ " CASE %d\n", test);

// BDE_VERIFY pragma: push   // Usage examples relax rules for doc clarity
// BDE_VERIFY pragma: -TP05  // Test doc is in delegated functions
// BDE_VERIFY pragma: -TP12  // Comment should contain a 'Testing:' section
// BDE_VERIFY pragma: -TP17  // No test-banners in a switch statement
// BDE_VERIFY pragma: -TP30  // Comment should contain a 'Concerns:' section
// BDE_VERIFY pragma: -TP33  // Comment should contain a 'Plan:' section

    switch (test) { case 0:
      case 27: {
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

        UsageExample::usageExample();
      } break;
      case 26: {
        //---------------------------------------------------------------------
        // TESTING 'to_array'
        //   Ensure that the 'to_array' free functions work correctly.
        //
        // Concerns:
        //: 1 Built-in arrays of copyable types result in all of the elements
        //:   being copied over.
        //:
        //: 2 Built-in arrays of move-only types result in all of the elements
        //:   being moved over (if supported).
        //
        // Plan:
        //: 1 Call 'to_array' on a built-in array of copyable values.
        //:
        //: 2 Call 'to_array' on a built-in array of move-only values.
        //
        // Testing:
        //      array<TYPE, SIZE> to_array(TYPE (&src)[SIZE]);
        //      array<TYPE, SIZE> to_array(TYPE (&&src)[SIZE]);
        //---------------------------------------------------------------------

        TestDriverWrapper<int>::testCase26();
      } break;
      case 25: {
        //---------------------------------------------------------------------
        // TESTING CLASS TEMPLATE DEDUCTION GUIDES (AT COMPILE TIME)
        //   Ensure that the deduction guides are properly specified to deduce
        //   the template arguments from the arguments supplied to the
        //   constructors.
        //
        // Concerns:
        //: 1 Argument lists of mixed type deduce the type of the array from
        //:   the type of the first element.
        //
        // Plan:
        //: 1 Create a pair by invoking the constructor without supplying the
        //:   template arguments explicitly.
        //:
        //: 2 Verify that the deduced type is correct.
        //
        // Testing:
        //   CLASS TEMPLATE DEDUCTION GUIDES
        //---------------------------------------------------------------------
        if (verbose)
            printf(
              "\nTESTING CLASS TEMPLATE DEDUCTION GUIDES (AT COMPILE TIME)"
              "\n=========================================================\n");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        // This is a compile-time only test case.
        TestDeductionGuides test;
        (void) test; // This variable only exists for ease of IDE navigation.
#endif
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TYPE TRAITS"
                            "\n===================\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase24,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING 'constexpr' EVALUATION
        //  This component provides many 'constexpr' functions whose runtime
        //  behavior has already been validated by earlier calls, but need
        //  additional tests to ensure that the 'constexpr' behavior is
        //  respected for arrays of literal types.  Testing all 'constexpr'
        //  methods for 'array<int, 7>' should suffice.  For testing purposes,
        //  we will assume any compiler that supports 'constexpr' has long
        //  since supported 'static_assert'.
        //
        // Concerns:
        //: 1 All methods defined with the 'BSLS_KEYWORD_CONSTEXPR' macro can
        //:   be used in constant evaluation on a C++11 compiler (or later).
        //:   o empty
        //:   o max_size
        //:   o size
        //:   o get  (x2)
        //:
        //: 2 All methods defined with the 'BSLS_KEYWORD_CONSTEXPR_CPP14' macro
        //:   can be used in constant evaluation on a C++14 compiler (or
        //:   later).
        //:   o operator[] (const)
        //:   o at
        //:   o front
        //:   o back
        //:   o get  (x2)
        //:
        //: 3 All methods defined with the 'BSLS_KEYWORD_CONSTEXPR_CPP17' macro
        //:   can be used in constant evaluation on a C++17 compiler (or
        //:   later).
        //:   o operator[] (non-const)
        //
        // Plan:
        //: 1 Call each of the listed 'constexpr' functions from a
        //:   'static_assert' expression, guarded by a macro check that C++11
        //:   'constexpr' is available.
        //:   o empty
        //:   o max_size
        //:   o size
        //:   o get
        //:   (C-1)
        //:
        //: 2 Call each of the listed 'constexpr' functions from a
        //:   'static_assert' expression, guarded by a macro check that C++14
        //:   'constexpr' is available.
        //:   o operator[] (const)
        //:   o at
        //:   o front
        //:   o back
        //:   (C-2)
        //:
        //: 3 Call each of the listed 'constexpr' functions from a
        //:   'static_assert' expression, guarded by a macro check that C++17
        //:   'constexpr' is available.
        //:   o operator[] (non-const)
        //
        // Testing:
        //   CONCERN: 'constexpr' FUNCTIONS ARE USABLE IN CONSTANT EVALUATION
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'constexpr' EVALUATION"
                            "\n==============================\n");

        typedef bsl::array<int, 7> Obj;

        BSLA_MAYBE_UNUSED static BSLS_KEYWORD_CONSTEXPR Obj X =
                                                      { 1, 1, 2, 3, 5, 8, 13 };

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
        static_assert(7 == X.size(),     "Bad value for 'size'");
        static_assert(7 == X.max_size(), "Bad value for 'max_size'");

        static_assert(!X.empty(),        "Array should not be empty");

        static_assert( 1 == bsl::get<0>(X), "Bad value for 'get' at index 0");
        static_assert( 1 == bsl::get<1>(X), "Bad value for 'get' at index 1");
        static_assert( 2 == bsl::get<2>(X), "Bad value for 'get' at index 2");
        static_assert( 3 == bsl::get<3>(X), "Bad value for 'get' at index 3");
        static_assert( 5 == bsl::get<4>(X), "Bad value for 'get' at index 4");
        static_assert( 8 == bsl::get<5>(X), "Bad value for 'get' at index 5");
        static_assert(13 == bsl::get<6>(X), "Bad value for 'get' at index 6");
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14)
        static_assert( 1 == X[0], "Bad value at index 0");
        static_assert( 1 == X[1], "Bad value at index 1");
        static_assert( 2 == X[2], "Bad value at index 2");
        static_assert( 3 == X[3], "Bad value at index 3");
        static_assert( 5 == X[4], "Bad value at index 4");
        static_assert( 8 == X[5], "Bad value at index 5");
        static_assert(13 == X[6], "Bad value at index 6");

        static_assert( 1 == *(X.begin()),  "Bad value at '*(begin())'");
        static_assert(13 == *(X.end()-1),  "Bad value at '*(end()-1)'");
        static_assert( 1 == *(X.cbegin()), "Bad value at '*(cbegin())'");
        static_assert(13 == *(X.cend()-1), "Bad value at '*(cend()-1)'");

        static_assert( 1 == X.data()[0], "Bad value via 'data' at index 0");
        static_assert( 1 == X.data()[1], "Bad value via 'data' at index 1");
        static_assert( 2 == X.data()[2], "Bad value via 'data' at index 2");
        static_assert( 3 == X.data()[3], "Bad value via 'data' at index 3");
        static_assert( 5 == X.data()[4], "Bad value via 'data' at index 4");
        static_assert( 8 == X.data()[5], "Bad value via 'data' at index 5");
        static_assert(13 == X.data()[6], "Bad value via 'data' at index 6");

# if !defined(BSLS_PLATFORM_CMP_GNU) || BSLS_PLATFORM_CMP_VERSION >= 60000
        static_assert( 1 == X.at(0), "Bad value 'at' index 0");
        static_assert( 1 == X.at(1), "Bad value 'at' index 1");
        static_assert( 2 == X.at(2), "Bad value 'at' index 2");
        static_assert( 3 == X.at(3), "Bad value 'at' index 3");
        static_assert( 5 == X.at(4), "Bad value 'at' index 4");
        static_assert( 8 == X.at(5), "Bad value 'at' index 5");
        static_assert(13 == X.at(6), "Bad value 'at' index 6");
# endif

        static_assert( 1 == X.front(), "Bad value at 'front' of array");
        static_assert(13 == X.back(),  "Bad value at 'back' of array");
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14)
        static_assert( 0 == testConstexprForNonConstBracketOperator(),
                       "Non-const operator[] test failed");
        static_assert( 0 == testConstexprForNonConstAt(),
                       "Non-const at() test failed");
        static_assert( 0 == testConstexprForNonConstFront(),
                       "Non-const front() test failed");
        static_assert( 0 == testConstexprForNonConstBack(),
                       "Non-const back() test failed");
        static_assert( 0 == testConstexprForNonConstData(),
                       "Non-const data() test failed");
        static_assert( 0 == testConstexprForNonConstBeginAndEnd(),
                       "Non-const begin() test failed");
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17)
        static_assert( 0 == testConstexprForNonConstRbegin(),
                       "Non-const rbegin() test failed");
        static_assert( 0 == testConstexprForNonConstRend(),
                       "Non-const rend() test failed");

        static_assert(13 == *(X.rbegin()), "Bad value at *(rbegin())");
        static_assert( 1 == *(X.rend()-1), "Bad value at *(rend()-1)");
        static_assert(13 == *(X.crbegin()), "Bad value at *(crbegin())");
        static_assert( 1 == *(X.crend()-1), "Bad value at *(crend()-1)");
#endif
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING 'hashAppend'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'hashAppend'"
                            "\n====================\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper
                     , testCase22
                     , signed char
                     , size_t
                     , bsltf::TemplateTestFacility::ObjectPtr
                     , bsltf::TemplateTestFacility::FunctionPtr
                     , const char *);
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING TUPLE INTERFACE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TUPLE INTERFACE"
                            "\n=======================\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase21,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'data'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'data'"
                            "\n==============\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase20,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'front' AND 'back'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'front' AND 'back'"
                            "\n==========================\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase19,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'at'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'at'"
                            "\n============\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase18,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING CAPACITY
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CAPACITY"
                            "\n================\n");

        // Test 'empty' and 'max_size' members.
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase17,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING COMPARISON OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COMPARISON OPERATORS"
                            "\n============================\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase16,
                      signed char,
                      const char *,
                      bsltf::TemplateTestFacility::ObjectPtr,
                      bsltf::TemplateTestFacility::FunctionPtr,
                      LessThanTestType);
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING AGGREGATE INITIALIZATION
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING AGGREGATE INITIALIZATION"
                            "\n================================\n");

        // Test aggregate initialization.
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase15,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ITERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ITERATORS"
                            "\n=================\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'fill'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'fill'"
                            "\n==============\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING MOVE ASSIGNMENT
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING MOVE ASSIGNMENT"
                            "\n=======================\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
#else
        if (verbose)
            printf("Move semantic is not supported on this compiler\n");
#endif
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING MOVE CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING MOVE CONSTRUCTOR"
                            "\n========================\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
#else
        if (verbose)
            printf("Move semantic is not supported on this compiler\n");
#endif
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING STREAMING FUNCTIONALITY"
                            "\n===============================\n");

        if (verbose)
            printf("There is no streaming for this component.\n");

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING COPY ASSIGNMENT
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COPY ASSIGNMENT"
                            "\n=======================\n");

        // Test assignment operator ('operator=').
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'swap'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'swap'"
                            "\n==============\n");

        // Test 'swap' member.
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        if (verbose)
            printf("Testing element 'swap' ADL\n");

        testCase8_elementADL<0>();
        testCase8_elementADL<1>();
        testCase8_elementADL<2>();
        testCase8_elementADL<3>();
        testCase8_elementADL<4>();
        testCase8_elementADL<5>();
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COPY CONSTRUCTOR"
                            "\n========================\n");

        // Test copy constructor.
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING COMPARISON OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COMPARISON OPERATORS"
                            "\n============================\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                TestDriverWrapper,
                                testCase6,
                                BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING OUTPUT (<<) OPERATOR"
                            "\n============================\n");

        if (verbose)
            printf("There is no output operator for this component.\n");

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BASIC ACCESSORS"
                            "\n=======================\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING GENERATOR FUNCTIONS"
                            "\n===========================\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf(
                   "\nTESTING PRIMARY MANIPULATORS"
                   "\n============================\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase1,
                      unsigned int);
      } break;
      default: {
        printf("WARNING: CASE %d NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        printf("Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}
// BDE_VERIFY pragma: pop

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
