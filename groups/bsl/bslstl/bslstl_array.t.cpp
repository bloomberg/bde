// bslstl_array.t.cpp                                                 -*-C++-*-
#include <bslstl_array.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_nameof.h>
#include <bsls_outputredirector.h>
#include <bsls_util.h>

#include <bsltf_movabletesttype.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <stdexcept>
#include <algorithm>
#include <cstddef>
#include <utility>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

using namespace BloombergLP;
using namespace bslstl;

// BDE_VERIFY pragma: -FD01
// BDE_VERIFY pragma: -AT02
// BDE_VERIFY pragma: -AC02

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
//
// Primary Manipulators:
//: o 'operator[]'
//
// Basic Accessors:
//: o 'operator[]'
//: o 'size()'
//
// We will follow our standard 10-case approach to testing value-semantic
// types, with the default constructor and primary manipulators tested fully
// in case 2 and additional operators, accessors, and manipulators tested above
// case 10. Aggregate initialization is also tested in test case 15 by
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
//
// CREATORS
// [ 2] array<T, S>();
// [ 7] array<T,S>(const array<T,S>& original);
// [ 2] ~array();
// [15] array<T, S>{{v1, v2, v3}};
//
// MANIPULATORS
// [14] iterator begin();
// [14] iterator end();
// [14] reverse_iterator rbegin();
// [14] reverse_iterator rend();
// [13] void fill(const T& value)
// [ 9] array<T,S>& operator=(const array<T,S>& rhs);
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
// [ 6] bool operator==(const array<T,S>&, const array<T,S>&);
// [ 6] bool operator!=(const array<T,S>&, const array<T,S>&);
// [16] bool operator<(const array<T,S>&, const array<T,S>&);
// [16] bool operator>(const array<T,S>&, const array<T,S>&);
// [16] bool operator<=(const array<T,S>&, const array<T,S>&);
// [16] bool operator>=(const array<T,S>&, const array<T,S>&);
// [ 8] void swap(array<T,S>&, array<T,S>&);
// [21] T& get(array<T, N>& p)
// [21] const T& get(const array<T, N>& p)
// [21] const T&& get(const array<T, N>&& p)
// [21] T&& get(array<T, N>&& p)
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [22] USAGE EXAMPLE

// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] int ggg(array<T,S> *object, const char *spec, int vF = 1);
// [ 3] array<T,S>& gg(array<T,S> *object, const char *spec);

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

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

//=============================================================================
//                       GLOBAL OBJECTS SHARED BY TEST CASES
//-----------------------------------------------------------------------------

// STATIC DATA
static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//track number of objects constructed with default constructor
static int s_numConstructed = 0;

//track if comparison operators use any operators other than <
static bool s_operators = false;

//=============================================================================
//                  HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

                            // ====================
                            // class CountedDefault
                            // ====================

template<class TYPE>
class CountedDefault {
    //Wrapper class to track number of times constructor and destructor called.
  public:
    CountedDefault();
    ~CountedDefault();

  private:
    TYPE d_val;
    CountedDefault(const CountedDefault& other); // = delete;
};

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

                            // ==============
                            // class LessThan
                            // ==============

class LessThan {
    //class that supports only 'operator<'.
  public:

    bool operator<(const LessThan& other) const;
    bool operator>(const LessThan& other) const;
    bool operator<=(const LessThan& other) const;
    bool operator>=(const LessThan& other) const;
    LessThan();
    explicit LessThan(int v);

  private:
    int d_val;
};

LessThan::LessThan(): d_val(0){}
LessThan::LessThan(int v): d_val(v){}
bool LessThan::operator<(const LessThan& other) const{
    return d_val < other.d_val;
}
bool LessThan::operator>(const LessThan& other) const{
    s_operators = true;
    return d_val > other.d_val;
}
bool LessThan::operator<=(const LessThan& other) const{
    s_operators = true;
    return d_val <= other.d_val;
}
bool LessThan::operator>=(const LessThan& other) const{
    s_operators = true;
    return d_val >= other.d_val;
}

//=============================================================================
//                 GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

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

//=============================================================================
//                      TEST APPARATUS: GENERATOR FUNCTIONS
//-----------------------------------------------------------------------------

// The generating functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters [A .. E] correspond to arbitrary (but unique) char values to be
// created in the 'bsl::array<T, S>' object.
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
            return i;  // Discontinue processing this spec.           // RETURN
        }
    }
    for (; i < SIZE; ++i){
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


//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace UsageExample {
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Returning an array from a function
///- - - - - - - - - - - - - - - - -
// Suppose we want to define a function that will return an array of floats.
// If a raw array was used, the size would need to be tracked seperately
// because raw arrays decay to pointers.  With bsl::array the result can be
// returned by value.
//..
typedef bsl::array<float, 3> Point;

Point createPoint(float f1, float f2, float f3)
{
    bsl::array<float, 3> ret = {f1, f2, f3};
    return ret;
}
// Create a bsl::array object containing three values set to the specified
// 'f1', 'f2', 'f3'.

void usageExample(){
    Point p1 = createPoint(1.0, 1.0, 1.0);
    Point p2 = createPoint(2.0, 2.0, 2.0);
    Point p3 = createPoint(3.0, 3.0, 3.0);

    bsl::array<Point, 3> points = {p1, p2, p3};

    for(size_t i = 0; i < points.size(); ++i){
        for(size_t j = 0; j < points[i].size(); ++j){
            points[i][j] *= 2.0f;
        }
    }
}
// Use the createPoint function to generate 3 arrays of floats.  The arrays
// are returned by copy and the 'size()' member function is used to access
// the size of the arrays that could not be done with a raw array.

}  // close namespace UsageExample

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

// Generate arrays using aggregate initialization for testCase13.
template<class TYPE>
struct AggregateTest {
    typedef bsltf::TemplateTestFacility TestFacility;

    static bsl::array<TYPE, 0> getAggregate(const char* spec,
            bsl::integral_constant<size_t, 0>);
    static bsl::array<TYPE, 1> getAggregate(const char* spec,
            bsl::integral_constant<size_t, 1>);
    static bsl::array<TYPE, 2> getAggregate(const char* spec,
            bsl::integral_constant<size_t, 2>);
    static bsl::array<TYPE, 3> getAggregate(const char* spec,
            bsl::integral_constant<size_t, 3>);
    static bsl::array<TYPE, 4> getAggregate(const char* spec,
            bsl::integral_constant<size_t, 4>);
};

template<class TYPE, size_t SIZE>
struct TestDriver {

    typedef bsl::array<TYPE, SIZE> Obj;

    typedef typename Obj::iterator               iterator;
    typedef typename Obj::const_iterator         const_iterator;
    typedef typename Obj::reverse_iterator       reverse_iterator;
    typedef typename Obj::const_reverse_iterator const_reverse_iterator;
    typedef typename Obj::value_type             ValueType;

    typedef bsltf::TestValuesArray<TYPE>         TestValues;
    typedef bsltf::TemplateTestFacility          TestFacility;

    static void testCase21();
        // Test tuple interface

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
        // Test debug print.

    static void testCase4();
        // Test basic accessors ('size' and 'operator[]').

    static void testCase3();
        // Test generator functions 'ggg' and 'gg'.

    static void testCase2();
        // Test primary manipulators, ctor, dtor.

    static void testCase1();
        // Breathing Test. Exercises basic functionality.
};

template<class TYPE>
struct TestDriverWrapper{

    static void testCase21();
        // Test tuple interface

    static void testCase20();
        // Test 'data' member.

    static void testCase19();
        // Test 'front' and 'back'  members.

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
        // Test debug print.

    static void testCase4();
        // Test basic accessors ('size' and 'operator[]').

    static void testCase3();
        // Test generator functions 'ggg' and 'gg'.

    static void testCase2();
        // Test primary manipulators, ctor, dtor.

    static void testCase1();
        // Breathing Test. Exercises basic functionality.
};

                                // ----------
                                // Test Cases
                                // ----------

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase21()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase21();
    TestDriver<TYPE, 1>::testCase21();
    TestDriver<TYPE, 2>::testCase21();
    TestDriver<TYPE, 3>::testCase21();
    TestDriver<TYPE, 4>::testCase21();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase21()
{
    // ------------------------------------------------------------------------
    // TESTING TUPLE INTERFACE
    //
    // Concerns:
    //: 1 'get' free functions return correct value.
    //:
    //: 2 'get' free functions return correct types.
    //:
    // Plan:
    //: 1 Create an array from spec string.
    //:
    //: 2 Test that 'get' free function returns correct value on all overloads.
    //:
    //: 3 Test that 'get' free function returns correct type for rvalue
    //:   references.
    //:
    // Testing:
    // T& get(array<T, N>& p)
    // const T& get(const array<T, N>& p)
    // const T&& get(const array<T, N>&& p)
    // T&& get(array<T, N>&& p)
    // ------------------------------------------------------------------------

    if (verbose) printf("\tof length " ZU "\n", SIZE);

    if (0 == SIZE) {
        if (verbose) printf("\t\tNo valid index to test");
        return;
    }

    static const struct {
        int         d_line;     // source line number
        const char *d_spec_p;     // specification string
    } DATA[] = {
    //------^
    //line spec
    //---- --------------------------------------------------------------------
    { L_,  "",                                                               },
    { L_,  "A",                                                              },
    { L_,  "BC",                                                             },
    { L_,  "CAB",                                                            },
    { L_,  "DABC",                                                           },
    { L_,  "EDCBA",                                                          }
    //------v
    };

    const char* const   SPEC   = DATA[SIZE].d_spec_p;

    Obj mW; const Obj& W = gg(&mW, SPEC);

    if (verbose) printf("\nTesting 'get' free function.\n");

    ASSERTV(bsls::Util::addressOf(bsl::get<SIZE-1>(mW))
         == bsls::Util::addressOf(mW[SIZE-1]));
    ASSERTV(bsls::Util::addressOf(bsl::get<SIZE-1>(W))
         == bsls::Util::addressOf(W[SIZE-1]));

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    if (verbose) printf("\nTesting 'get' free function with rvalues.\n");
    {
        TYPE&& mX       = bsl::get<SIZE-1>(std::move(mW));
        const TYPE&& X  = bsl::get<SIZE-1>(std::move(W));
        ASSERTV(mX == TestFacility::create<TYPE>(SPEC[SIZE-1]));
        ASSERTV(X  == TestFacility::create<TYPE>(SPEC[SIZE-1]));
    }
#endif
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase20()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase20();
    TestDriver<TYPE, 1>::testCase20();
    TestDriver<TYPE, 2>::testCase20();
    TestDriver<TYPE, 3>::testCase20();
    TestDriver<TYPE, 4>::testCase20();
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
    //: 1 Default construct array
    //:
    //: 2 Test 'data' member returns a pointer to the array member variable
    //:
    //: 3 Test 'data' member has the correct signature
    //:
    // Testing:
    //  pointer data();
    //  const_pointer data() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\tof length " ZU "\n", SIZE);

    Obj mW;
    const Obj& W = mW;

    ASSERTV(mW.data() == mW.d_data);
    ASSERTV(W.data() == W.d_data);
    ASSERT((bsl::is_same<typename Obj::const_pointer, const TYPE*>::value));
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase19()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 1>::testCase19();
    TestDriver<TYPE, 2>::testCase19();
    TestDriver<TYPE, 3>::testCase19();
    TestDriver<TYPE, 4>::testCase19();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase19()
{
    // ------------------------------------------------------------------------
    // TESTING 'front' AND 'back'
    //
    // Concerns:
    //: 1 'front' member modifies last elememt of object correctly
    //:
    //: 2 'front' member accesses last elememt of object correctly
    //:
    //: 3 'back' member modifies last elememt of object correctly
    //:
    //: 4 'back' member accesses last elememt of object correctly
    //:
    // Plan:
    //: 1 Generate an array from a spec string
    //:
    //: 2 Test 'front' and 'back' access correctly
    //:
    //: 3 Test 'front' and 'back' modify correctly
    //:
    // Testing:
    //  reference front();
    //  const_reference front() const;
    //  reference back();
    //  const_reference back() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\tof length " ZU "\n", SIZE);

    static const struct {
        int         d_line;     // source line number
        const char *d_spec_p;     // specification string
    } DATA[] = {
    //------^
    //line spec
    //---- --------------------------------------------------------------------
    { L_,  "",                                                               },
    { L_,  "A",                                                              },
    { L_,  "BC",                                                             },
    { L_,  "CAB",                                                            },
    { L_,  "DABC",                                                           },
    { L_,  "EDCBA",                                                          }
    //------v
    };

    const char* const SPEC = DATA[SIZE].d_spec_p;
    const char        V    = 'V';

    if (verbose) printf("\nGenerating array from spec string %s.\n", SPEC);

    Obj mW; const Obj& W = gg(&mW, SPEC);

    if (verbose) printf("\nTest 'front' and 'back' access correctly.\n");

    ASSERTV(&(W.front()) == &(W[0]));
    ASSERTV(&(W.back()) == &(W[SIZE-1]));

    if (verbose) printf("\nTest 'front' and 'back' modify correctly.\n");

    mW.front() = TestFacility::create<TYPE>(V);
    ASSERTV(W.front() == TestFacility::create<TYPE>(V));

    mW.back() = TestFacility::create<TYPE>(V);
    ASSERTV(W.back() == TestFacility::create<TYPE>(V));
}



template<class TYPE>
void TestDriverWrapper<TYPE>::testCase18()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase18();
    TestDriver<TYPE, 1>::testCase18();
    TestDriver<TYPE, 2>::testCase18();
    TestDriver<TYPE, 3>::testCase18();
    TestDriver<TYPE, 4>::testCase18();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase18()
{
    // ------------------------------------------------------------------------
    // TESTING 'at'
    //
    // Concerns:
    //: 1 'at' member accesses state of object correctly
    //:
    //: 2 'at' member modifies state of object correctly
    //:
    //: 3 Elements can be set using 'at' for any type that supports
    //:   the assignment operator.
    //
    // Plan:
    //: 1 Construct an array from the spec string and verify all values are
    //:   accessed as expected.
    //:
    //: 2 Modify each element one at a time and verify the rest of the array
    //:   was not changed and that the element was modified as expected.
    //:
    //: 3 Verify that 'out_of_range' exception thrown when 'pos >= size()'
    //:
    //
    // Testing:
    //  reference at();
    //  const_reference at();
    // ------------------------------------------------------------------------

    if (verbose) printf("\tof length " ZU "\n", SIZE);

    static const struct {
        int         d_line;     // source line number
        const char *d_spec_p;     // specification string
    } DATA[] = {
    //------^
    //line spec
    //---- --------------------------------------------------------------------
    { L_,  "",                                                               },
    { L_,  "A",                                                              },
    { L_,  "BC",                                                             },
    { L_,  "CAB",                                                            },
    { L_,  "DABC",                                                           },
    { L_,  "EDCBA",                                                          }
    //------v
    };

    const int           LINE   = DATA[SIZE].d_line;
    const char* const   SPEC   = DATA[SIZE].d_spec_p;
    const size_t        LENGTH = strlen(DATA[SIZE].d_spec_p);
    const TestValues    EXP(DATA[SIZE].d_spec_p);

    const char V = 'V';

    if (verbose) printf("Testing 'at' accesses state of object correctly\n");
    {
        Obj        mW;
        const Obj& W = gg(&mW, SPEC);

        if (veryVerbose) printf("\t\tTesting on container values %s.\n", SPEC);

        for (size_t i = 0; i < LENGTH; ++i) {
            TYPE&       xi = mW.at(i);
            const TYPE& Xi = W.at(i);
            ASSERTV(LINE, i, xi, EXP[i] == mW.at(i));
            ASSERTV(LINE, i, Xi, EXP[i] == W.at(i));
        }
    }

    if (verbose) printf("Testing modifying container values.\n");
    {
        Obj        mW;
        const Obj& W  = gg(&mW, SPEC);
        Obj        mX = W;
        const Obj& X  = mX;

        for (size_t i = 0; i < LENGTH; ++i) {
            mW.at(i) = TestFacility::create<TYPE>(V);
            for (size_t j = 0; j <= i; ++j) {
                ASSERTV(LINE, j, TestFacility::create<TYPE>(V) == mW.at(j));
                ASSERTV(LINE, j, TestFacility::create<TYPE>(V) == W.at(j));
            }
            for (size_t j = i+1; j < LENGTH; ++j) {
                ASSERTV(LINE, j, mX[j] == mW.at(j));
                ASSERTV(LINE, j, X[j] == W.at(j));
            }
        }
    }

#if defined(BDE_BUILD_TARGET_NO_EXC)
    if (verbose) {
        printf("No testing for 'out_of_range' as exceptions are disbaled.\n");
    }
#else
    if (verbose) printf("Testing for out_of_range exceptions thrown by at() "
                        "when pos >= size().\n");
    {
        Obj        mW;
        const Obj& W = gg(&mW, SPEC);

        const int NUM_TRIALS = 2;
        // Check exception behavior for non-const version of 'at()'.  Checking
        // the behavior for 'pos == size()' and 'pos > size()'.

        int exceptions = 0;
        int trials;
        for (trials = 0; trials < NUM_TRIALS; ++trials)
        {
            try {
                mW.at(LENGTH + trials);
                ASSERTV(LENGTH, trials, mW.size(), false);
            }
            catch (const std::out_of_range&) {
                ++exceptions;
                if (veryVerbose) {
                    printf("\t\tIn out_of_range exception.\n");
                    T_ T_ P_(LINE); P(trials);
                }
            }
        }
        ASSERTV(exceptions, trials, exceptions == trials);

        exceptions = 0;
        // Check exception behavior for const version of at()
        for (trials = 0; trials < NUM_TRIALS; ++trials)
        {
            try {
                W.at(LENGTH + trials);
                ASSERTV(LENGTH, trials, W.size(), false);
            } catch (const std::out_of_range&) {
                ++exceptions;
                if (veryVerbose) {
                    printf("\t\tIn out_of_range exception." );
                    T_ T_ P_(LINE); P(trials);
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

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase17();
    TestDriver<TYPE, 1>::testCase17();
    TestDriver<TYPE, 2>::testCase17();
    TestDriver<TYPE, 3>::testCase17();
    TestDriver<TYPE, 4>::testCase17();
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
    //: 1 Create an 'array', 'w'.
    //:
    //: 2 Test that 'max_size()' returns a value equal to 'SIZE'.
    //:
    //: 3 Test that if 'empty()' returns 'true' if the array has size 0 and
    //:   'false' otherwise.
    //:
    //
    // Testing:
    //  bool empty() const;
    //  size_type max_size() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\tof length " ZU "\n", SIZE);

    Obj mW; const Obj& W = mW;

    ASSERT(W.empty() == (0 == SIZE));
    ASSERT(W.max_size() == SIZE);
}


template<class TYPE>
void TestDriverWrapper<TYPE>::testCase16()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

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
    //: 1 <, >, <=, >= operators lexicographically compare arrays of types
    //:   supporting the < operator.
    //:
    //: 2 Comparison operators work on 0 length arrays.
    //:
    //: 3 Only operator < is used to perform comparisons.
    //
    // Plan:
    //: 1 Create a variety of spec strings for each length.
    //:
    //: 2 Test that every operator gives expected results for each combination
    //:   of arrays generated by the spec strings of the length being tested.
    //
    // Testing:
    //  bool operator<(const array& lhs, const array& rhs);
    //  bool operator>(const array& lhs, const array& rhs);
    //  bool operator<=(const array& lhs, const array& rhs);
    //  bool operator>=(const array& lhs, const array& rhs);

    // ------------------------------------------------------------------------

    if (verbose) printf("\tof length " ZU "\n", SIZE);

    static const struct {
        int         d_line;     // source line number
        const char *d_spec_p;  // specification string
    } DATA[] = {
    //------^
    //line spec1
    //---- --------------------------------------------------------------------
    { L_,  "",                                                               },
    { L_,  "A",                                                              },
    { L_,  "AAB",                                                            },
    { L_,  "AB",                                                             },
    { L_,  "ABB",                                                            },
    { L_,  "ABCC",                                                           },
    { L_,  "B",                                                              },
    { L_,  "BAAAA",                                                          },
    { L_,  "BBAAA",                                                          },
    { L_,  "D"                                                               }
    //------v
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\nValidating all four relational operators.\n");
    {
        for (size_t i = 0; i < NUM_DATA; ++i) {
            if (strlen(DATA[i].d_spec_p) > SIZE) {
                continue;
            }

            Obj mW; const Obj& W = gg(&mW, DATA[i].d_spec_p);

            ASSERTV(W <= W);
            ASSERTV(W >= W);

            for (size_t j = 0; j != i; ++j) {
                if(strlen(DATA[j].d_spec_p) > SIZE) {
                    continue;
                }

                if (veryVerbose) printf("\tComparing %s and %s.\n",
                        DATA[j].d_spec_p, DATA[i].d_spec_p);

                Obj mX; const Obj& X = gg(&mX, DATA[j].d_spec_p);

                ASSERTV(DATA[j].d_spec_p, DATA[i].d_spec_p, X, W, X < W);
                ASSERTV(DATA[j].d_spec_p, DATA[i].d_spec_p, X, W, X <= W);
                ASSERTV(DATA[i].d_spec_p, DATA[j].d_spec_p, W, X, W > X);
                ASSERTV(DATA[i].d_spec_p, DATA[j].d_spec_p, W, X, W >= X);
            }

            for (size_t j = i+1; j < NUM_DATA; ++j) {
                if(strlen(DATA[j].d_spec_p) > SIZE) {
                    continue;
                }

                if (veryVerbose) printf("\tComparing %s and %s.\n",
                        DATA[j].d_spec_p, DATA[i].d_spec_p);

                Obj mX; const Obj& X = gg(&mX, DATA[j].d_spec_p);

                ASSERTV(DATA[i].d_spec_p, DATA[j].d_spec_p, W, X, W < X);
                ASSERTV(DATA[i].d_spec_p, DATA[j].d_spec_p, W, X, W <= X);
                ASSERTV(DATA[j].d_spec_p, DATA[i].d_spec_p, X, W, X > W);
                ASSERTV(DATA[j].d_spec_p, DATA[i].d_spec_p, X, W, X >= W);
            }
        }
    }

    if (verbose) printf("\nConfirming comparisons use only 'operator<'.\n");
    {
        bsl::array<LessThan, 3>        mX;
        const bsl::array<LessThan, 3>& X = mX;
        mX[0] = LessThan(1);
        mX[1] = LessThan(2);
        mX[2] = LessThan(3);

        (void) (X < X);
        (void) (X <= X);
        (void) (X > X);
        (void) (X >= X);

        ASSERTV(s_operators == false);
    }
}


template<class TYPE>
void TestDriverWrapper<TYPE>::testCase15()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase15();
    TestDriver<TYPE, 1>::testCase15();
    TestDriver<TYPE, 2>::testCase15();
    TestDriver<TYPE, 3>::testCase15();
    TestDriver<TYPE, 4>::testCase15();
}


template <class TYPE>
bsl::array<TYPE, 0>
AggregateTest<TYPE>::getAggregate(const char* spec,
        bsl::integral_constant<size_t, 0>)
{
    ASSERT(strlen(spec) == 0);
    bsl::array<TYPE, 0> ret = {{}};
    return ret;
}

template <class TYPE>
bsl::array<TYPE, 1>
AggregateTest<TYPE>::getAggregate(const char* spec,
        bsl::integral_constant<size_t, 1>)
{
    bsl::array<TYPE, 1> ret = {{TestFacility::create<TYPE>(spec[0])}};
    return ret;
}

template <class TYPE>
bsl::array<TYPE, 2>
AggregateTest<TYPE>::getAggregate(const char* spec,
        bsl::integral_constant<size_t, 2>)
{
    bsl::array<TYPE, 2> ret = {{TestFacility::create<TYPE>(spec[0]),
             TestFacility::create<TYPE>(spec[1])}};
        return ret;
}

template <class TYPE>
bsl::array<TYPE, 3>
AggregateTest<TYPE>::getAggregate(const char* spec,
        bsl::integral_constant<size_t, 3>)
{
    bsl::array<TYPE, 3> ret = {{TestFacility::create<TYPE>(spec[0]),
             TestFacility::create<TYPE>(spec[1]),
             TestFacility::create<TYPE>(spec[2])}};
        return ret;
}

template <class TYPE>
bsl::array<TYPE, 4>
AggregateTest<TYPE>::getAggregate(const char* spec,
        bsl::integral_constant<size_t, 4>)
{
    bsl::array<TYPE, 4> ret = {{TestFacility::create<TYPE>(spec[0]),
             TestFacility::create<TYPE>(spec[1]),
             TestFacility::create<TYPE>(spec[2]),
             TestFacility::create<TYPE>(spec[3])}};
        return ret;
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
    //: 2 Ensure elements all have expected values.
    //:
    //: 3 Ensure any elements not provided in braces were value initialized.
    //:
    //
    // Testing:
    //  array<V> a = {{vt1, vt2, vt3}}
    //  array<V> a = {vt1, vt2, vt3}

    // ------------------------------------------------------------------------

    if (verbose) printf("\tof length " ZU "\n", SIZE);

    static const struct {
        int         d_line;     // source line number
        const char *d_spec_p;     // specification string
        const char *d_results_p;  // expected element values
    } DATA[] = {
    //------^
    //line spec                                result
    //---- ----------------------------------- --------------------------------
    { L_,  "",                                 ""                            },
    { L_,  "A",                                "A"                           },
    { L_,  "BC",                               "BC"                          },
    { L_,  "CAB",                              "CAB"                         },
    { L_,  "DABC",                             "DABC"                        }
    //------v
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    const char* const   SPEC   = DATA[SIZE].d_spec_p;

    if (verbose) printf("\ntesting initialization of all elemenets.\n");
    {
        Obj        mW;
        const Obj& W = mW;

        Obj        mX;
        const Obj& X = mX;

        mW = AggregateTest<TYPE>::getAggregate(SPEC,
                bsl::integral_constant<size_t, SIZE>());
        mX = gg(&mX, SPEC);

        ASSERTV(W==X);
    }

    if (verbose) printf("\ntest that extra elements are value-initialized.\n");

    if(SIZE == 4)
    {
        bsl::array<TYPE, 4> mW = {{TestFacility::create<TYPE>(SPEC[0]),
             TestFacility::create<TYPE>(SPEC[1]),
             TestFacility::create<TYPE>(SPEC[2])}};

        const bsl::array<TYPE, 4>& W = mW;

        ASSERTV(W[SIZE-1] == TestFacility::create<TYPE>(0));
    }
}



template<class TYPE>
void TestDriverWrapper<TYPE>::testCase14()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

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
    //: 3 'begin' and 'end' return mutable iterators for a
    //:   reference to a modifiable array, and non-mutable iterators
    //:   otherwise.
    //:
    //: 4 The range '[begin(), end())' traverses the elements of the array in
    //:   index order.
    //:
    //: 5 'reverse_iterator' and 'const_reverse_iterator' are
    //:   implemented by the (fully-tested) 'bslstl::ReverseIterator' over a
    //:   pointer to 'TYPE' or 'const TYPE'.
    //:
    //: 6 The range '[rbegin(), rend())' traverses the elements of the array in
    //:   reverse index order.
    //
    // Plan:
    //: 1 Construct an array w from the 'SPEC'.
    //:
    //: 2 Access and modify each element using iterators, then change each
    //:   element back.
    //:
    //: 3 Repeat step 2 with reverse iterators.
    //:
    //: 4 Access each element of a const array using const iterators and const
    //:   reverse iterators to ensure the values are as expected.
    //:
    //: 5 Use 'bsl::is_same' to check that the iterators are correct types.
    //:
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

    if (verbose) printf("\tof length " ZU "\n", SIZE);

    if (verbose) printf("\nTesting iterators are correct types.\n");
    ASSERT( (bsl::is_same<iterator, TYPE *>::value));
    ASSERT( (bsl::is_same<const_iterator, const TYPE *>::value));
#ifdef BSLS_PLATFORM_CMP_SUN
#else
    ASSERT( (bsl::is_same<reverse_iterator,
                               bsl::reverse_iterator<TYPE *> >::value));
    ASSERT( (bsl::is_same<const_reverse_iterator,
                            bsl::reverse_iterator<const TYPE *> >::value));
#endif

    static const struct {
        int         d_line;     // source line number
        const char *d_spec_p;     // specification string
        const char *d_results_p;  // expected element values
    } DATA[] = {
    //------^
    //line spec                                result
    //---- ----------------------------------- --------------------------------
    { L_,  "",                                 ""                            },
    { L_,  "A",                                "A"                           },
    { L_,  "BC",                               "BC"                          },
    { L_,  "CAB",                              "CAB"                         },
    { L_,  "DABC",                             "DABC"                        },
    { L_,  "EDCBA",                            "EDCBA"                       }
    //------v
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    const int           LINE   = DATA[SIZE].d_line;
    const char* const   SPEC   = DATA[SIZE].d_spec_p;

    if (verbose) printf("\nTesting iterator functions.\n");

    const char V = 'V';

    Obj        mW;
    const Obj& W = mW;

    mW = gg(&mW, SPEC);
    int count = 0;

    if (verbose) printf("\nTesting iterator goes through entire array.\n");
    for (typename Obj::iterator it = mW.begin(); it != mW.end(); ++it){
        ASSERTV(LINE, bsls::Util::addressOf(*it) == W.d_data + count);
        ++count;
        ASSERTV(LINE, TestFacility::getIdentifier(*it)==SPEC[it - mW.begin()]);

        if (verbose) printf("\tTesting iterator changes elements correctly\n");

        *it = TestFacility::create<TYPE>(V);
        ASSERTV(LINE, W[it - mW.begin()] == TestFacility::create<TYPE>(V));
    }
    ASSERTV(count, SIZE, count == SIZE);

    if (verbose) printf("\nTesting reverse iterators.\n");

    //reset array
    mW = gg(&mW, SPEC);

    count = 0;
    for (typename Obj::reverse_iterator it = mW.rbegin();it != mW.rend();++it){
        ASSERTV(TestFacility::getIdentifier(*it)==SPEC[SIZE-1-count]);

        if (verbose) printf("\tTest reverse iter changes elems correctly.\n");

        *it = TestFacility::create<TYPE>(V);
        ASSERTV(LINE,
                bsls::Util::addressOf(*it) == W.d_data + SIZE - 1 - count);
        ASSERTV(LINE, W[SIZE - 1 - count] == TestFacility::create<TYPE>(V));

        ++count;
    }
    ASSERTV(count, SIZE, count == SIZE);

    //reset array
    mW = gg(&mW, SPEC);
    count = 0;

    if (verbose) printf("\nTesting iterators on const arrays.\n");

    for (typename Obj::const_iterator it = W.begin(); it != W.end(); ++it){
        ASSERTV(LINE, &(*it) == W.d_data + count);
        ++count;
        ASSERTV(LINE, TestFacility::getIdentifier(*it)==SPEC[it - W.begin()]);
    }
    ASSERTV(count, SIZE, count == SIZE);
    count = 0;
    for (typename Obj::const_reverse_iterator it = W.rbegin();
            it != W.rend();it++){
        ASSERTV(LINE, &(*it) == W.d_data + SIZE - 1 - count);
        ASSERTV(TestFacility::getIdentifier(*it)==SPEC[SIZE-1-count]);
        count++;
    }
    ASSERTV(count, SIZE, count == SIZE);
    count = 0;

    if (verbose) printf("\nTesting cend(), cbegin(), crend(), crbegin().\n");

    for (typename Obj::const_iterator it = mW.cbegin(); it != mW.cend(); it++){
        ASSERTV(LINE, &(*it) == W.d_data + count);
        count++;
        ASSERTV(LINE, TestFacility::getIdentifier(*it)==SPEC[it-mW.cbegin()]);
    }
    ASSERTV(count, SIZE, count == SIZE);
    count = 0;
    for (typename Obj::const_reverse_iterator it = mW.crbegin();
            it != mW.crend();it++){
        ASSERTV(LINE, &(*it) == W.d_data + SIZE - 1 - count);
        ASSERTV(TestFacility::getIdentifier(*it)==SPEC[SIZE-1-count]);
        count++;
    }
    ASSERTV(count, SIZE, count == SIZE);

}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase13()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase13();
    TestDriver<TYPE, 1>::testCase13();
    TestDriver<TYPE, 2>::testCase13();
    TestDriver<TYPE, 3>::testCase13();
    TestDriver<TYPE, 4>::testCase13();
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
    //: 3 Fill works on arrays that already contain values.
    //:
    //: 4 Fill can be called without effect on arrays of length 0
    //
    // Plan:
    //: 1 Default construct an array w.
    //:
    //: 2 Construct an array x from the 'SPEC'.
    //:
    //: 3 Construct an array y such that all elements are the value v.
    //:
    //: 4 Use fill to fill w and x with v. Test w == x == y.
    //:
    //: 5 Modify each element of w and test that the other elements dont change
    //
    // Testing:
    //   void fill(const T& value)
    // ------------------------------------------------------------------------

    if (verbose) printf("\tof length " ZU "\n", SIZE);

    static const struct {
        int         d_line;     // source line number
        const char *d_spec_p;     // specification string
        const char *d_results_p;  // expected element values
    } DATA[] = {
    //------^
    //line spec                                result
    //---- ----------------------------------- --------------------------------
    { L_,  "",                                 ""                            },
    { L_,  "A",                                "A"                           },
    { L_,  "BC",                               "BC"                          },
    { L_,  "CAB",                              "CAB"                         },
    { L_,  "DABC",                             "DABC"                        },
    { L_,  "EDCBA",                            "EDCBA"                       }
    //------v
    };

    static const char TEST_VALS[] = "ABCDEFGHIJKLMNOPQRSTUV";

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    const char* const   SPEC   = DATA[SIZE].d_spec_p;

    for(size_t i = 0; i < strlen(TEST_VALS); ++i){

        // Array default constructed to test fill on.
        Obj mW; const Obj& W = mW;

        // Array generated from a spec to test fill on.
        Obj mX; const Obj& X = gg(&mX, SPEC);

        //Values to fill arrays with
        const char V = TEST_VALS[i];

        mW.fill(TestFacility::create<TYPE>(V));
        mX.fill(TestFacility::create<TYPE>(V));

        if (verbose) printf("\nTesting W and X were properly filled\n");

        for(size_t j = 0; j < SIZE; ++j){
            ASSERTV(W[j] == TestFacility::create<TYPE>(V));
            ASSERTV(W[j] == X[j]);
            ASSERTV(X[j] == TestFacility::create<TYPE>(V));
        }

        //Value to change single elements to for testing.
        const char N = 'W';

        if (verbose) printf("\tTesting changes to W have no side effects\n");
        for (size_t i = 0; i != SIZE; ++i) {
            mW[i] = TestFacility::create<TYPE>(N);
            for (size_t j = 0; j != SIZE; ++j) {
                if(j != i) ASSERT(TestFacility::getIdentifier(W[j]) == V);
            }

            //check changes to W didnt change X.
            for(size_t j = 0; j != SIZE; ++j){
                ASSERTV(X[j] == TestFacility::create<TYPE>(V));
            }
            mW[i] = TestFacility::create<TYPE>(V);
        }
    }
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase12()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase12();
    TestDriver<TYPE, 1>::testCase12();
    TestDriver<TYPE, 2>::testCase12();
    TestDriver<TYPE, 3>::testCase12();
    TestDriver<TYPE, 4>::testCase12();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase12()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE ASSIGNMENT
    //
    // Concerns:
    //: 1 A class with a compiler generated move assignment operator will be
    //:   copied.
    //:
    //: 2 A class with an explicit move assignment operator will have the move
    //:   assignment operator called.
    //:
    //
    // Plan:
    //: 1 Move construct an array of a type with a default generated move
    //:   constructor. Ensure the new array has the expected values.
    //:
    //: 2 Move construct an array of a type with an explicit move assignment
    //:   operator. Ensure the new array has the expected values and that the
    //:   move assignment operator was called.
    //:
    // Testing:
    //
    // ------------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    if (verbose) printf("\tof length " ZU "\n", SIZE);

    static const struct {
        int         d_line;     // source line number
        const char *d_spec_p;     // specification string
    } DATA[] = {
    //------^
    //line spec
    //---- --------------------------------------------------------------------
    { L_,  ""                                                                },
    { L_,  "A"                                                               },
    { L_,  "BC"                                                              },
    { L_,  "CAB"                                                             },
    { L_,  "DABC"                                                            },
    { L_,  "EDCBA"                                                           }
    //------v
    };

    const char* const SPEC = DATA[SIZE].d_spec_p;

    Obj mW;
    gg(&mW, SPEC);

    Obj mX; const Obj& X = mX;

    if(verbose) printf("\nTesting that lhs is properly set.\n");

    mX = std::move(mW);
    for(size_t i = 0; i < SIZE; ++i){
        ASSERTV(X[i] == TestFacility::create<TYPE>(SPEC[i]));
    }

    bsl::array<bsltf::MovableTestType, SIZE> mY;
    const bsl::array<bsltf::MovableTestType, SIZE>& Y = gg(&mY, SPEC);

    bsl::array<bsltf::MovableTestType, SIZE> mZ;
    const bsl::array<bsltf::MovableTestType, SIZE>& Z = mZ;

    mZ = std::move(mY);

    if(verbose) printf("\nTesting that elements are in proper move states.\n");

    for(size_t i = 0; i < SIZE; ++i){
        ASSERTV(bsltf::MoveState::e_MOVED      == Y[i].movedFrom());
        ASSERTV(bsltf::MoveState::e_NOT_MOVED  == Y[i].movedInto());
        ASSERTV(bsltf::MoveState::e_NOT_MOVED  == Z[i].movedFrom());
        ASSERTV(bsltf::MoveState::e_MOVED      == Z[i].movedInto());
    }
#endif
}


template<class TYPE>
void TestDriverWrapper<TYPE>::testCase11()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase11();
    TestDriver<TYPE, 1>::testCase11();
    TestDriver<TYPE, 2>::testCase11();
    TestDriver<TYPE, 3>::testCase11();
    TestDriver<TYPE, 4>::testCase11();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase11()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE CONSTRUCTOR
    //
    // Concerns:
    //: 1 A class with a compiler generated move constructor will be copy
    //:   constructed.
    //:
    //: 2 A class with an explicit move constructor will have the move
    //:   constructor called.
    //:
    //
    // Plan:
    //: 1 Move construct an array of a type with a default generated move
    //:   constructor. Ensure the new array has the expected values.
    //:
    //: 2 Move construct an array of a type with an explicit move constructor.
    //:   Ensure the new array has the expected values and that the move
    //:   constructor was called.
    //:
    //
    // Testing:
    //
    // ------------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    if (verbose) printf("\tof length " ZU "\n", SIZE);

    static const struct {
        int         d_line;     // source line number
        const char *d_spec_p;     // specification string
    } DATA[] = {
    //------^
    //line spec
    //---- --------------------------------------------------------------------
    { L_,  ""                                                                },
    { L_,  "A"                                                               },
    { L_,  "BC"                                                              },
    { L_,  "CAB"                                                             },
    { L_,  "DABC"                                                            },
    { L_,  "EDCBA"                                                           }
    //------v
    };

    const char* const   SPEC   = DATA[SIZE].d_spec_p;

    Obj mW;
    gg(&mW, SPEC);

    Obj mX = std::move(mW);  // Construction being tested

    const Obj& X  = mX;

    if(verbose) printf("\nTesting that lhs is properly set.\n");

    for(size_t i = 0; i < SIZE; ++i){
        ASSERTV(X[i], X[i] == TestFacility::create<TYPE>(SPEC[i]));
    }

    if(verbose) printf("\nTesting that elements are in proper move states.\n");

    bsl::array<bsltf::MovableTestType, SIZE> mY;
    const bsl::array<bsltf::MovableTestType, SIZE>& Y = gg(&mY, SPEC);

    bsl::array<bsltf::MovableTestType, SIZE> mZ = std::move(mY);
    const bsl::array<bsltf::MovableTestType, SIZE>& Z = mZ;

    for(size_t i = 0; i < SIZE; ++i){
        ASSERTV(bsltf::MoveState::e_MOVED      == Y[i].movedFrom());
        ASSERTV(bsltf::MoveState::e_NOT_MOVED  == Y[i].movedInto());
        ASSERTV(bsltf::MoveState::e_NOT_MOVED  == Z[i].movedFrom());
        ASSERTV(bsltf::MoveState::e_MOVED      == Z[i].movedInto());
    }
#endif
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase9()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase9();
    TestDriver<TYPE, 1>::testCase9();
    TestDriver<TYPE, 2>::testCase9();
    TestDriver<TYPE, 3>::testCase9();
    TestDriver<TYPE, 4>::testCase9();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase9()
{
    // ------------------------------------------------------------------------
    // TESTING COPY ASSIGNMENT
    //
    // Concerns:
    //: 1 The value of any array can be assigned to any other array of the same
    //:   type as long as the element type is assignable.
    //:
    //: 2 The 'rhs' value must not be affected by the operation.
    //:
    //: 3 'rhs' going out of scope has no effect on the value of 'lhs' after
    //:   the assignment.
    //:
    //: 4 Modifications to 'rhs' have no effect on the value of 'lhs' after
    //:   the assignment.
    //:
    //: 5 Aliasing ('x = x'): The assignment operator must always work -- even
    //:   when the 'lhs' and 'rhs' are the same object.
    //:
    //
    // Plan:
    //: 1 Construct array w from the 'SPEC' and arrays y and z from a
    //:   different 'SPEC' string.
    //:
    //: 2 Set w = y. Check y == z and w == z.
    //:
    //: 3 Modify y and check that w is still equal to z.
    //:
    //: 4 Set w = w and check that w is still equal to z.
    //:
    //
    // Testing:
    //   array& operator=(const array& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\tof length " ZU "\n", SIZE);

    static const struct {
        int         d_line;     // source line number
        const char *d_spec_p;     // specification string
        const char *d_results_p;  // expected element values
    } DATA[] = {
    //------^
    //line spec                                result
    //---- ----------------------------------- --------------------------------
    { L_,  "",                                 ""                            },
    { L_,  "A",                                "A"                           },
    { L_,  "BC",                               "BC"                          },
    { L_,  "CAB",                              "CAB"                         },
    { L_,  "DABC",                             "DABC"                        },
    { L_,  "EDCBA",                            "EDCBA"                       }
    //------v
    };

    //Simple spec list to generate arrays to test assignment operator with
    static const char *DATA2[] = { "", "A", "AA", "AAA", "AAAA", "AAAAA"};

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    const int           LINE   = DATA[SIZE].d_line;
    const char* const   SPEC   = DATA[SIZE].d_spec_p;
    const char* const   SPEC2  = DATA2[SIZE];

    if (verbose) printf("\nConstructing arrays from different specs\n");

    Obj mW; const Obj& W = mW;
    Obj mZ; const Obj& Z = mZ;
    {
        Obj mY; const Obj& Y = mY;
        mW  = gg(&mW, SPEC);
        mY  = gg(&mY, SPEC2);
        mZ  = gg(&mZ, SPEC2);

        if (verbose) printf("\tTesting operator= does not modify 'rhs'\n");
        mW = mY;
        ASSERTV(LINE, Y == Z);
        ASSERTV(LINE, W == Z);

        if (verbose) printf("\tTesting modifications to 'rhs' does not\n"
                            "\taffect 'lhs' after assignment\n");
        mY = gg(&mY, SPEC);
        ASSERTV(LINE, W == Z);

        mW = mW;
        ASSERTV(LINE, W == Z);
    }
    //mY goes out of scope
    if (verbose) printf("\tTesting 'rhs' going out of scope does not\n"
                        "\taffect 'lhs' after assignment\n");
    ASSERTV(LINE, W == Z);
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase8()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase8();
    TestDriver<TYPE, 1>::testCase8();
    TestDriver<TYPE, 2>::testCase8();
    TestDriver<TYPE, 3>::testCase8();
    TestDriver<TYPE, 4>::testCase8();
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
    //: 1 Construct arrays 'w' and 'x' based on the same spec string.
    //:
    //: 2 Construct arrays 'y' and 'z' constructed from the same spec
    //:   string which is different from 'w' and 'x' above.
    //:
    //: 3 Use the free 'swap' function to swap w and y, then confirm that
    //:   'w == z' and 'y == x' respectively.
    //:
    //: 4 Use the member 'swap' function to swap w and y, then confirm that
    //:   'w == x' and 'y == z' respectively.
    //
    // Testing:
    //   void swap(array&);
    //   void swap(array<T,A>& lhs, array<T,A>& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\tof length " ZU "\n", SIZE);

    static const struct {
        int         d_line;     // source line number
        const char *d_spec_p;     // specification string
        const char *d_results_p;  // expected element values
    } DATA[] = {
    //------^
    //line spec                                result
    //---- ----------------------------------- --------------------------------
    { L_,  "",                                 ""                            },
    { L_,  "A",                                "A"                           },
    { L_,  "BC",                               "BC"                          },
    { L_,  "CAB",                              "CAB"                         },
    { L_,  "DABC",                             "DABC"                        },
    { L_,  "EDCBA",                            "EDCBA"                       }
    //------v
    };

    //Simple spec list to generate arrays to swap with
    static const char *DATA2[] = { "", "A", "AA", "AAA", "AAAA", "AAAAA"};

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    const int           LINE   = DATA[SIZE].d_line;
    const char* const   SPEC   = DATA[SIZE].d_spec_p;
    const char* const   SPEC2  = DATA2[SIZE];

    Obj mW; const Obj& W = mW;
    Obj mX; const Obj& X = mX;
    Obj mY; const Obj& Y = mY;
    Obj mZ; const Obj& Z = mZ;

    if (verbose) printf("\nConstructing arrays from the different specs");

    mW = gg(&mW, SPEC);
    mX = gg(&mX, SPEC);
    mY = gg(&mY, SPEC2);
    mZ = gg(&mZ, SPEC2);

    ASSERTV(LINE, W == X);
    ASSERTV(LINE, Y == Z);

    if (verbose) printf("\nTesting free 'swap' function\n");

    swap(mW, mY);
    ASSERTV(LINE, W == Z);
    ASSERTV(LINE, Y == X);

    if (verbose) printf("\nTesting member 'swap' function\n");

    mW.swap(mY);
    ASSERTV(LINE, W == X);
    ASSERTV(LINE, Y == Z);
}



template<class TYPE>
void TestDriverWrapper<TYPE>::testCase7()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase7();
    TestDriver<TYPE, 1>::testCase7();
    TestDriver<TYPE, 2>::testCase7();
    TestDriver<TYPE, 3>::testCase7();
    TestDriver<TYPE, 4>::testCase7();
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
    //: 1 Construct two arrays w and x based on the 'SPEC'.
    //:
    //: 2 Copy-construct an array y from the array w constructed in step 1.
    //:
    //: 3 Check the arrays w, x, and y are all equal.
    //:
    //: 4 Modify the copy-constructed array y and check that w and x are
    //:   still equal. Also check that y and w are no longer equal.
    //:
    //: 5 Copy-construct another array z from x. Then modify x and check
    //:   that x and z are no longer equal.
    //:
    //
    // Testing:
    //   array<T,S>(const array<T,S>& original);
    // ------------------------------------------------------------------------

    if (verbose) printf("\tof length " ZU "\n", SIZE);

    static const struct {
        int         d_line;     // source line number
        const char *d_spec_p;     // specification string
        const char *d_results_p;  // expected element values
    } DATA[] = {
    //------^
    //line spec                                result
    //---- ----------------------------------- --------------------------------
    { L_,  "",                                 ""                            },
    { L_,  "A",                                "A"                           },
    { L_,  "BC",                               "BC"                          },
    { L_,  "CAB",                              "CAB"                         },
    { L_,  "DABC",                             "DABC"                        },
    { L_,  "EDCBA",                            "EDCBA"                       }
    //------v
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    const int           LINE   = DATA[SIZE].d_line;
    const char* const   SPEC   = DATA[SIZE].d_spec_p;
    const size_t        LENGTH = strlen(DATA[SIZE].d_results_p);

    Obj mW; const Obj& W = gg(&mW, SPEC);
    Obj mX; const Obj& X = gg(&mX, SPEC);

    Obj mY = Obj(W); const Obj& Y  = mY;

    if (verbose) printf("\tTesting copy compares as equal to original.\n");

    ASSERTV(LINE, mY == mW);
    ASSERTV(LINE, Y == W);
    ASSERTV(LINE, mY == mX);
    ASSERTV(LINE, Y == X);

    if (verbose) printf(
                   "\tTesting modifications to copy do not affect original\n");

    for (size_t i = 0; i < LENGTH; ++i){
        const char s = SPEC[i];
        mY[i] = TestFacility::create<TYPE>(s+1);

        ASSERTV(LINE, !(Y == X));
        ASSERTV(LINE, !(Y == W));
        ASSERTV(LINE, !(mY == mX));
        ASSERTV(LINE, !(mY == mW));
    }

    Obj mZ = Obj(X); const Obj& Z  = mZ;

    if (verbose) printf(
                   "\tTesting modifications to original do not affect copy\n");

    for (size_t i = 0; i < LENGTH; ++i){
        const char s = SPEC[i];
        mX[i] = TestFacility::create<TYPE>(s+1);

        ASSERTV(Z, X, LINE, !(Z == X));
        ASSERTV(Z, X, LINE, !(mZ == mX));
    }
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase6()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase6();
    TestDriver<TYPE, 1>::testCase6();
    TestDriver<TYPE, 2>::testCase6();
    TestDriver<TYPE, 3>::testCase6();
    TestDriver<TYPE, 4>::testCase6();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase6()
{
    // ------------------------------------------------------------------------
    // TESTING 'OPERATOR=='
    //   Ensure operator== properly compares equality.
    //
    // Concerns:
    //: 1 Arrays constructed with the same values are returned as equal.
    //:
    //: 2 Unequal arrays are always returned as unequal
    //
    // Plan:
    //: 1 For each set of 'SPEC' of different length:
    //:
    //:   1 Default construct two instances of the array.
    //:
    //:   2 Populate the arrays based on the same SPEC Using the 'gg' function.
    //:
    //:   3 Verify that operator== returns true when comparing the arrays.
    //:
    //:   4 Modify an element in one of the arrays.
    //:
    //:   5 Verify that operator== returns false when comparing the arrays
    // Testing:
    //   bool operator==(const array& rhs, const array& lhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\tof length " ZU "\n", SIZE);

    static const struct {
        int         d_line;     // source line number
        const char *d_spec_p;     // specification string
        const char *d_results_p;  // expected element values
    } DATA[] = {
    //------^
    //line spec                                result
    //---- ----------------------------------- --------------------------------
    { L_,  "",                                 ""                            },
    { L_,  "A",                                "A"                           },
    { L_,  "BC",                               "BC"                          },
    { L_,  "CAB",                              "CAB"                         },
    { L_,  "DABC",                             "DABC"                        },
    { L_,  "EDCBA",                            "EDCBA"                       }
    //------v
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    const int           LINE   = DATA[SIZE].d_line;
    const char* const   SPEC   = DATA[SIZE].d_spec_p;
    const size_t        LENGTH = strlen(DATA[SIZE].d_results_p);

    if (verbose) printf(
                    "Testing arrays with the same values compare as equal.\n");
    {
        Obj mY; const Obj& Y = mY;
        Obj mZ; const Obj& Z = mZ;

        if (veryVerbose) printf(
                        "\tVerify default constructed array equals itself.\n");
        ASSERTV(LINE, mY == mY);
        ASSERTV(LINE, Y == Y);
        ASSERTV(LINE, mZ == mZ);
        ASSERTV(LINE, Z == Z);

        if (veryVerbose) printf(
                "\tVerify arrays generated with the same 'SPEC' are equal.\n");
        Obj mW; const Obj& W = gg(&mW, SPEC);
        Obj mX; const Obj& X = gg(&mX, SPEC);

        ASSERTV(LINE, mW == mW);
        ASSERTV(LINE,  W == mW);
        ASSERTV(LINE,  W ==  X);
        ASSERTV(LINE, mW ==  X);
    }

    if (verbose) printf("Testing arrays do not compare equal after modifying "
                        "elements in one of the arrays.\n");
    {
        typedef bsltf::TemplateTestFacility TestFacility;

        Obj mW; const Obj& W = gg(&mW, SPEC);

        for (size_t i = 0; i < LENGTH; ++i) {
            Obj mX; const Obj& X = gg(&mX, SPEC);

            const char s = SPEC[i];
            mW[i]        = TestFacility::create<TYPE>(s + 1);

            ASSERTV(W, X, LINE, !(W == X));
            ASSERTV(X, W, LINE, !(X == W));
        }
    }
}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase5()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase5();
    TestDriver<TYPE, 1>::testCase5();
    TestDriver<TYPE, 2>::testCase5();
    TestDriver<TYPE, 3>::testCase5();
    TestDriver<TYPE, 4>::testCase5();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase5()
{
    // ------------------------------------------------------------------------
    // TESTING PRINT CONCERNS
    //
    // Concerns:
    //: 1 The standard test values that support 'debugprint' can be printed
    //:   using the standard test-driver 'P' and 'P_' macros.
    //:
    //: 2 The printed values are correct when inspected by hand, running this
    //:   test case in 'verbose' mode.
    //
    // Plan:
    //: 1 Create a constant array object, passing a value
    //:   specification.
    //:
    //: 2 Loop over all the values, and in verbose mode print the values, and
    //:   the corresponding 'SPEC' character, using the 'P_' and 'P' macros.
    //:
    //: 3 Run in verbose mode to inspect the printed value, and confirm all the
    //:   printed stings have the expected values.
    //
    // Testing:
    //  CONCERN: all values in the array are printable with 'P()' macro
    // ------------------------------------------------------------------------

    using bsls::NameOf;

    if (verbose) printf("\tof length " ZU "\n", SIZE);

    const char   *SPEC = "ABCDEFGHIJKLMNOPQRSTUVWXY";
    Obj           mW;
    const Obj     VALUES     = gg(&mW, SPEC);
    const size_t  NUM_VALUES = VALUES.size();

    if (verbose) {
        printf("\tPrinting test values for manual validation\n");
        printf("\t\tSPEC: %s\n", SPEC);
        // Rely on manual inspection, no automatic validation in verbose mode.

        { T_ P(VALUES) }
        for (size_t i = 0; i != NUM_VALUES; ++i) {
            { T_ T_ P_(i) P_(SPEC[i]) P(VALUES[i]) }
        }
        puts("\n");
    }
    else {
        // Intercept stdout and read back formatted results to validate for the
        // test result.
#if 0

        bsls::OutputRedirector redirector(
                                      bsls::OutputRedirector::e_STDOUT_STREAM,
                                      true,
                                      true);

#if 1
        redirector.disable();
#else
        // Write just the test values, and parse them back as integers, with
        // adjustments for known corner cases.

        for (size_t i = 0; i != NUM_VALUES; ++i) {
            redirector.enable();
            redirector.clear();
            P_(VALUES[i]);

            if (!redirector.load()) {
                redirector.disable();
                ASSERT(!"Could not load redirected output into buffer.");
                break;
            }
            redirector.disable();

            if (!redirector.isOutputReady()) {
                ASSERT(!"Redirected output buffer is empty.");
                break;
            }
            if (redirector.outputSize() < 12 ) {
                ASSERT(!"Redirected output buffer is shorted than expected.");
                break;
            }

            const char *printedText = redirector.getOutput();
            ASSERT(0 == strncmp("VALUES[i] = ", printedText, 12));

            printedText += 12;
            if ('{' == *printedText) {
                ++printedText;
            }

            if (bsl::is_member_pointer<TYPE>::value) {
                // There is no 'debugprint' overload for member-pointers.
                // Instead, member-pointers match the overload for 'bool' with
                // a built-in conversion, so we expect the output to be simply
                // "true" for a non-null value, and "false" for a null member
                // pointer.  We can spot the latter case as it has the testing
                // framework gives null the identifier '0'.

                if (0 == bsltf::TemplateTestFacility::getIdentifier(VALUES[i]))
                {
                    ASSERTV(i, printedText,
                            0 == strncmp("false", printedText, 5));
                }
                else {
                    ASSERTV(i, printedText,
                            0 == strncmp("true", printedText, 4));
                }
            }
            else {
                // For types other than member-pointer, the output format is an
                // integer, potentially TYPE, potentially in hexadecimal
                // format, that can be parsed simply with 'strtol'.  Note that
                // we should check that some output was consumed by the call to
                // 'strtol', otherwise we might be fooled if '0' is a valid
                // result (as happens for 'const char *').

                char *result = 0;

                // Note that pointer values are usually printed as hex, but IBM
                // fails to precede the hex-string with '0x', forcing use of
                // the explicit hexadecimal radix.  Note that 'const char *'
                // values are actually string representations, holding the
                // decimal string representation of the corresponding ID.

                const int radix = bsl::is_pointer<TYPE>::value &&
                                  !bsl::is_same<const char *, TYPE>::value
                                  ? 16  : 0;
                const long parsedValue = strtol(printedText, &result, radix);
                if (result == printedText) {
                    if (bsl::is_pointer<TYPE>::value) {
                        ASSERTV(
                     NameOf<TYPE>().name(),
                     result,
                     bsltf::TemplateTestFacility::getIdentifier(VALUES[i]),
                     bsltf::TemplateTestFacility::getIdentifier(VALUES[i]) == 0
                     || !"Parsing redirected output did not consume any text");
                    }
                    else {
                        ASSERTV(NameOf<TYPE>().name(), result,
                        !"Parsing redirected output did not consume any text");
                    }
                }
                else {
                    ASSERTV(NameOf<TYPE>().name(),
                            'A' + i,
                            parsedValue,
                            printedText,
                            long('A' + i) == parsedValue);
                }
            }
        }
#endif
#endif
    }
}


template<class TYPE>
void TestDriverWrapper<TYPE>::testCase4()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase4();
    TestDriver<TYPE, 1>::testCase4();
    TestDriver<TYPE, 2>::testCase4();
    TestDriver<TYPE, 3>::testCase4();
    TestDriver<TYPE, 4>::testCase4();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase4()
{
    // ------------------------------------------------------------------------
    // TESTING BASIC ACCESSORS
    //   Ensure each basic accessor:
    //     - operator[]
    //     - size
    //   properly interprets object state.
    //
    // Concerns:
    //: 1 Each accessor returns the value of the correct property of the
    //:   object.
    //:
    //: 2 Each accessor method is declared 'const'.
    //
    // Plan:
    //: 1 Use 'gg' function to populate the object based on the spec string.
    //:
    //: 2 Verify the object contains the expected number of elements.
    //:
    //: 3 Iterate through all elements and verify the values are as expected.
    //:
    //
    // Testing:
    //   reference operator[](size_type position) const;
    //   reference at(size_type position) const;
    //   size_type size();
    // ------------------------------------------------------------------------

    if (verbose) printf("\tof length " ZU "\n", SIZE);

    static const struct {
        int         d_line;       // source line number
        const char *d_spec_p;     // specification string
        const char *d_results_p;  // expected element values
    } DATA[] = {
    //------^
    //line spec                                result
    //---- ----------------------------------- --------------------------------
    { L_,  "",                                 ""                            },
    { L_,  "A",                                "A"                           },
    { L_,  "BC",                               "BC"                          },
    { L_,  "CAB",                              "CAB"                         },
    { L_,  "DABC",                             "DABC"                        },
    { L_,  "EDCBA",                            "EDCBA"                       }
    //------v
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    const int           LINE   = DATA[SIZE].d_line;
    const char* const   SPEC   = DATA[SIZE].d_spec_p;
    const size_t        LENGTH = strlen(DATA[SIZE].d_results_p);
    const TestValues    EXP(DATA[SIZE].d_results_p);

    if (verbose) printf("\nTesting operator[] and function at() access state"
                        "of object correctly\n");
    {
        Obj        mW;
        const Obj& W = gg(&mW, SPEC);

        if (veryVerbose) printf("\t\tTesting on container values %s.\n", SPEC);

        for (size_t i = 0; i < LENGTH; ++i) {
            ASSERTV(LINE, i, EXP[i] == mW[i]);
            ASSERTV(LINE, i, EXP[i] == W[i]);
        }
        if (verbose) printf ("Testing size() returns correct object length\n");

        ASSERT(mW.size() == SIZE);
        ASSERT(W.size() == SIZE);
    }

}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase3()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase3();
    TestDriver<TYPE, 1>::testCase3();
    TestDriver<TYPE, 2>::testCase3();
    TestDriver<TYPE, 3>::testCase3();
    TestDriver<TYPE, 4>::testCase3();
}

template <class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase3()
{
    // ------------------------------------------------------------------------
    // TESTING GENERATOR FUNCTIONS
    //   Ensure that the gg function works as expected and that valid generator
    //   syntax produced the expected results.
    //
    // Concerns:
    //: 1 'gg' function returns an 'array' that accurately represents an array
    //:   created by the test facility using the 'spec' of the correct size.
    //
    // Plan:
    //: 1 For the sequence of 'spec' values with length equal to the length of
    //:   the array type to be tested, create an array using the gg function
    //:   and the 'spec' string.
    //:
    //: 2 For each index in the array:
    //:
    //:     1: Check that the element at that index is equal to the element at
    //:        that index in the corrosponding test values array 'EXP' which
    //:        was constructed by the same 'spec' string.
    //
    // Testing:
    //   array<T,A>& gg(array<T,A> *object, const char *spec);
    // ------------------------------------------------------------------------

    if (verbose) printf("\tof length " ZU "\n", SIZE);

    if (verbose) printf("\nTesting generator on valid specs.\n");
    {
        static const struct {
            int         d_line;     // source line number
            const char *d_spec_p;     // specification string
            const char *d_results_p;  // expected element values
        } DATA[] = {
        //------^
        //line spec                                result
        //---- ----------------------------------- ----------------------------
        { L_,  "",                                 ""                        },
        { L_,  "A",                                "A"                       },
        { L_,  "BC",                               "BC"                      },
        { L_,  "CAB",                              "CAB"                     },
        { L_,  "DABC",                             "DABC"                    },
        { L_,  "EDCBA",                            "EDCBA"                   }
        //------v
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        const int           LINE   = DATA[SIZE].d_line;
        const char* const   SPEC   = DATA[SIZE].d_spec_p;
        const size_t        LENGTH = strlen(DATA[SIZE].d_results_p);
        const TestValues    EXP(DATA[SIZE].d_results_p);

        Obj        mW;
        const Obj& W = gg(&mW, SPEC);

        for (size_t i = 0; i < LENGTH; ++i) {
            ASSERTV(LINE, i, EXP[i] == mW.d_data[i]);
            ASSERTV(LINE, i, EXP[i] == W.d_data[i]);
        }
    }
    if (verbose) printf("\nTesting generator on invalid specs.\n");
    {
        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p_p;   // specification string
            int         d_index;    // offending character index
        } DATA[] = {
            //line  spec      index
            //----  --------  -----
            { L_,   "",       -1,   },  // control

            { L_,   " ",       0,   },
            { L_,   ".",       0,   },
            { L_,   "E",      -1,   },  // control
            { L_,   "Z",       0,   },

            { L_,   "AE",     -1,   },  // control
            { L_,   "aE",      0,   },
            { L_,   "Ae",      1,   },
            { L_,   ".~",      0,   },
            { L_,   "~!",      0,   },
            { L_,   "  ",      0,   },

            { L_,   "ABC",    -1,   },  // control
            { L_,   " BC",     0,   },
            { L_,   "A C",     1,   },
            { L_,   "AB ",     2,   },
            { L_,   "?#:",     0,   },
            { L_,   "   ",     0,   },

            { L_,   "ABCDE",  -1,   },  // control
            { L_,   "aBCDE",   0,   },
            { L_,   "ABcDE",   2,   },
            { L_,   "ABCDe",   4,   },
            { L_,   "AbCdE",   1,   }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int          LINE  = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p_p;
            const int         INDEX  = DATA[ti].d_index;
            const int         LENGTH = static_cast<int>(strlen(SPEC));


            if (LENGTH == SIZE){
                Obj mX;

                if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

                int result = ggg(&mX, SPEC, veryVerbose);
                ASSERTV(LINE, INDEX, result, SPEC, INDEX == result);
            }
        }
    }
}


template<class TYPE>
void TestDriverWrapper<TYPE>::testCase2()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

    TestDriver<TYPE, 0>::testCase2();
    TestDriver<TYPE, 1>::testCase2();
    TestDriver<TYPE, 2>::testCase2();
    TestDriver<TYPE, 3>::testCase2();
    TestDriver<TYPE, 4>::testCase2();
}

template<class TYPE, size_t SIZE>
void TestDriver<TYPE, SIZE>::testCase2()
{
    // --------------------------------------------------------------------
    // TESTING DEFAULT CTOR, PRIMARY MANIPULATORS, AND DTOR
    //   Ensure that we can use the default constructor to create an
    //   object (having the default-constructed value), use the primary
    //   manipulators to put that object into any state relevant for
    //   thorough testing, and use the destructor to destroy it safely.
    //
    // Concerns:
    //: 1 An object created with the default constructor defalt constructs
    //:   a number of elements equal to the size of the array with the
    //:   exception of 0 length arrays that will construct 1 element.
    //:
    //: 2 Destructor calls the destructor of every element.
    //:
    //: 3 Elements can be set using 'operator[]' for any type that supports
    //:   the assignment operator.
    //:
    // Plan:
    //: 1 For each array of different length:
    //:
    //:   1 Construct an array of that length.
    //:
    //:   2 Verify that the correct number of elements was constructed.
    //:
    //:   3 Verify all elements are deleted when the array is destroyed.
    //:
    //:   4 Construct an array based on the 'SPEC' configuration for the
    //:     current length.
    //:
    //:   5 Verify that the value of each element is correct by inspecting
    //:     d_data.
    //
    // Testing:
    //   array();
    //   ~array();
    //   reference operator[](size_type position);
    // --------------------------------------------------------------------

    if (verbose) printf("\tof length " ZU "\n", SIZE);

    static const struct {
        int         d_line;     // source line number
        const char *d_spec_p;     // specification string
        const char *d_spec2_p;    // second specification string
        const char *d_results_p;  // expected element values
    } DATA[] = {
    //------^
    //line spec            spec2               result
    //---- --------------- ------------------- --------------------------------
    { L_,  "",             "",                 ""                            },
    { L_,  "A",            "B",                "B"                           },
    { L_,  "BC",           "CD",               "CD"                          },
    { L_,  "CAB",          "ABC",              "ABC"                         },
    { L_,  "DABC",         "CABD",             "CABD"                        },
    { L_,  "EDCBA",        "DECBA",            "DECBA"                       }
    //------v
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) printf("\tCreate an object 'c' using CountedDefault to count "
                        "how many elements are constructed (default ctor).\n");
    {
    ASSERT(s_numConstructed == 0);
    bsl::array<CountedDefault<TYPE>, SIZE> c;

    ASSERT(SIZE == c.size());
    ASSERTV(s_numConstructed, SIZE == s_numConstructed || SIZE == 0);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) printf("\tDestructor destroys each element.\n");

    ASSERTV(s_numConstructed, 0 == s_numConstructed);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) printf("\tTesting operator[] (Primary manipulator).\n");
    {
        const int           LINE   = DATA[SIZE].d_line;
        const char* const   SPEC   = DATA[SIZE].d_spec_p;
        const char* const   SPEC2   = DATA[SIZE].d_spec2_p;
        const size_t        LENGTH = strlen(DATA[SIZE].d_results_p);
        const TestValues    EXP(DATA[SIZE].d_spec_p);
        const TestValues    EXP2(DATA[SIZE].d_results_p);

        Obj        mW;
        const Obj& W = mW;

        Obj        mX;
        const Obj& X = mX;

        if (veryVerbose) printf("\t\tTesting on container values %s.\n", SPEC);

        for (size_t i = 0; i < LENGTH; ++i) {
            mW[i] = TestFacility::create<TYPE>(SPEC[i]);
            mX[i] = TestFacility::create<TYPE>(SPEC[i]);
        }

        for (size_t i = 0; i < LENGTH; ++i) {
            ASSERTV(LINE, i, EXP[i] == mW.d_data[i]);
            ASSERTV(LINE, i, EXP[i] == W.d_data[i]);
        }

        if (veryVerbose) printf(
                     "\t\tTesting modifying container values from %s to %s.\n",
                     SPEC,
                     SPEC2);

        for (size_t i = 0; i < LENGTH; ++i) {
            mW[i] = TestFacility::create<TYPE>(SPEC2[i]);
            for (size_t j = 0; j <= i; ++j) {
                ASSERTV(LINE, j, EXP2[j] == mW.d_data[j]);
                ASSERTV(LINE, j, EXP2[j] == W.d_data[j]);
            }
            for (size_t j = i+1; j < LENGTH; ++j) {
                ASSERTV(LINE, j, mX.d_data[j] == mW.d_data[j]);
                ASSERTV(LINE, j, X.d_data[j] == W.d_data[j]);
            }
        }
    }

}

template<class TYPE>
void TestDriverWrapper<TYPE>::testCase1()
{
    using bsls::NameOf;

    if (verbose) printf("\nFor array of type: '%s'\n", NameOf<TYPE>().name());

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
    //: 1 The class is sufficiently functional to enable comprehensive
    //:   testing in subsequent test cases.
    //
    // Plan:
    //: 1 Create an object 'w' (default ctor).           { w:D           }
    //: 2 Test mutation methods on 'w'.                  { w:D           }
    //: 3 Create an object 'x' (copy from 'w').          { w:D x:V       }
    //: 4 Test comparison methods on 'w' and 'x'.        { w:D x:V       }
    //
    // Testing:
    //   BREATHING TEST
    // --------------------------------------------------------------------

    if (verbose) printf("\tof length " ZU "\n", SIZE);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) printf("\n 1. Create an object 'w' (default ctor)."
                        "\t\t{ w:D           }\n");

    Obj mW; const Obj& W = mW;

    ASSERT(SIZE == mW.size());
    ASSERT(SIZE == mW.max_size());
    ASSERT(SIZE == W.size());
    ASSERT(SIZE == W.max_size());

    if (0 == SIZE)
        return;                                                       // RETURN

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) printf("\n 2. Test mutation methods on 'w' "
                        "\t\t{ w:D x:V        }\n");

    for (size_t i = 0; i < SIZE; ++i) {
        mW[i] = i;
    }

    ASSERT(mW[SIZE-1] == SIZE-1);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) printf("\n 3. Create an object 'x' (copy from 'w') "
                        "\t\t{ w:D x:V        }\n");

    Obj mX(mW); const Obj& X = mX;

    for (size_t i = 0; i < SIZE; ++i) {
        ASSERT(X[i] == W[i]);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) printf("\n 4. Test comparison methods on 'w' and 'x'"
                        "\t\t{ w:D x:V        }\n");

    ASSERT(X == W);
    ASSERT(X <= W);
    ASSERT(X >= W);
    ASSERT(!(X < W));
    ASSERT(!(X > W));
}

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    printf("TEST " __FILE__ " CASE %d\n", test);

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -TP05
// BDE_VERIFY pragma: -TP17
// BDE_VERIFY pragma: -TP30
    switch (test){ case 0:
      case 22: {
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
      case 21: {
        // --------------------------------------------------------------------
        // TESTING TUPLE INTERFACE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TUPLE INTERFACE"
                            "\n=======================\n");

        // Test tuple interface.
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

        // Test 'data' member.
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

        // Test 'front' and 'back' members.
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

        // Test 'at' member.
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

        // Test comparison operators.
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase16,
                      signed char,
                      const char *,
                      bsltf::TemplateTestFacility::ObjectPtr,
                      bsltf::TemplateTestFacility::FunctionPtr);
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

        // Test iterators.
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

        // Test 'fill' member.
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

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        if (verbose) printf("move semantics not supported on this compiler\n");
#else
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
#endif
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING MOVE CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING MOVE CONSTRUCTOR"
                            "\n========================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        if (verbose) printf("move semantics not supported on this compiler\n");
#else
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
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
        // TESTING SWAP
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING SWAP"
                            "\n============\n");

        // Test 'swap' member.
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COPY CONSTRUCTOR"
                            "\n====================\n");

        // Test copy constructor.
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'OPERATOR=='
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'OPERATOR=='"
                            "\n====================\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING PRINT CONCERNS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING PRINT CONCERNS"
                            "\n======================\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverWrapper,
                      testCase5,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
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
        // TESTING DEFAULT CTOR, PRIMARY MANIPULATORS, AND DTOR
        // --------------------------------------------------------------------

        if (verbose) printf(
                   "\nTESTING DEFAULT CTOR, PRIMARY MANIPULATORS, AND DTOR"
                   "\n====================================================\n");

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
