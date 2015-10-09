// bslalg_containerbase.t.cpp                                         -*-C++-*-
#include <bslalg_containerbase.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isbitwiseequalitycomparable.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>
#include <bsls_util.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

#include <new>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

template <class T>
class Allocator {
    // An STL-compatible allocator that forwards allocation calls to an
    // underlying mechanism object of a type derived from 'bslma::Allocator'.
    // Duplicate of 'bslstl_allocator'.

    // DATA
    BloombergLP::bslma::Allocator *d_mechanism;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Allocator, bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(Allocator, bslmf::IsBitwiseMoveable);
    BSLMF_NESTED_TRAIT_DECLARATION(
            Allocator,
            bslmf::IsBitwiseEqualityComparable);
        // Declare nested type traits for this class.

    // PUBLIC TYPES
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef T              *pointer;
    typedef const T        *const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef T               value_type;

    template <class U> struct rebind
    {
        // This nested 'struct' template, parameterized by some type 'U',
        // provides a namespace for an 'other' type alias, which is an
        // allocator type following the same template as this one but that
        // allocates elements of type 'U'.  Note that this allocator type is
        // convertible to and from 'other' for any type 'U' including 'void'.

        typedef Allocator<U> other;
    };

    // CREATORS
    Allocator();
        // Construct a proxy object which will forward allocation calls to the
        // object pointed to by 'bslma::Default::defaultAllocator()'.
        // Postcondition:
        //..
        //  this->mechanism() == bslma::Default::defaultAllocator();
        //..

    Allocator(BloombergLP::bslma::Allocator *mechanism);
        // Convert a 'bslma::Allocator' pointer to a 'allocator' object which
        // forwards allocation calls to the object pointed to by the specified
        // 'mechanism'.  If 'mechanism' is 0, then the currently installed
        // default allocator is used instead.  Postcondition:
        // '0 == mechanism || this->mechanism() == mechanism'.

    Allocator(const Allocator& rhs);
        // Copy construct a proxy object using the same mechanism as rhs.
        // Postcondition: 'this->mechanism() == rhs.mechanism()'.

    template <class U>
    Allocator(const Allocator<U>& rhs);
        // Construct a proxy object sharing the same mechanism object as 'rhs'.
        // The newly constructed allocator will compare equal to rhs, even
        // though they are instantiated on different types.  Postcondition:
        // 'this->mechanism() == rhs.mechanism()'.

    //! ~Allocator();
        // Destroy this object.  Note that this does not delete the object
        // pointed to by 'mechanism()'.  Also note that this method's
        // definition is compiler generated.

    //! Allocator& operator=(const Allocator& rhs);
        // Assign this object the value of the specified 'rhs'.  Postcondition:
        // 'this->mechanism() == rhs->mechanism()'.  Note that this does not
        // delete the object pointed to by the previous value of 'mechanism()'.
        // Also note that this method's definition is compiler generated.

    // MANIPULATORS
    pointer allocate(size_type n, const void *hint = 0);
        // Allocate enough (properly aligned) space for 'n' objects of type 'T'
        // by calling 'allocate' on the mechanism object.  The 'hint' argument
        // is ignored by this allocator type.  The behavior is undefined unless
        // 'n <= max_size()'.

    void deallocate(pointer p, size_type n = 1);
        // Return memory previously allocated with 'allocate' to the underlying
        // mechanism object by calling 'deallocate' on the the mechanism
        // object.  The 'n' argument is ignored by this allocator type.

    void construct(pointer p, const T& val);
        // Copy-construct a 'T' object at the memory address specified by 'p'.
        // Do not directly allocate memory.  Undefined if 'p' is not properly
        // aligned for 'T'.

    void destroy(pointer p);
        // Call the 'T' destructor for the object pointed to by 'p'.  Do not
        // directly deallocate any memory.

    // ACCESSORS
    BloombergLP::bslma::Allocator *mechanism() const;
        // Return a pointer to the mechanism object to which this proxy
        // forwards allocation and deallocation calls.

    pointer address(reference x) const;
        // Return the address of 'x'.

    const_pointer address(const_reference x) const;
        // Return the address of 'x'.

    size_type max_size() const;
        // Return the maximum number of elements of type 'T' that can be
        // allocated using this allocator.  Note that there is no guarantee
        // that attempts at allocating less elements than the value returned by
        // 'max_size' will not throw.
};

// FREE OPERATORS
template <class T1, class T2>
inline
bool operator==(const Allocator<T1>& lhs,
                const Allocator<T2>& rhs);
    // Return true 'lhs' and 'rhs' are proxies for the same 'bslma::Allocator'
    // object.  This is a practical implementation of the STL requirement that
    // two allocators compare equal if and only if memory allocated from one
    // can be deallocated from the other.  Note that the two allocators need
    // not be instantiated on the same type in order to compare equal.

template <class T1, class T2>
inline
bool operator!=(const Allocator<T1>& lhs,
                const Allocator<T2>& rhs);
    // Return '!(lhs == rhs)'.

                             // ---------------
                             // class Allocator
                             // ---------------

// LOW-LEVEL ACCESSORS
template <class T>
inline
BloombergLP::bslma::Allocator *Allocator<T>::mechanism() const
{
    return d_mechanism;
}

// CREATORS
template <class T>
inline
Allocator<T>::Allocator()
: d_mechanism(BloombergLP::bslma::Default::defaultAllocator())
{
}

template <class T>
inline
Allocator<T>::Allocator(BloombergLP::bslma::Allocator *mechanism)
: d_mechanism(BloombergLP::bslma::Default::allocator(mechanism))
{
}

template <class T>
inline
Allocator<T>::Allocator(const Allocator& rhs)
: d_mechanism(rhs.mechanism())
{
}

template <class T>
template <class U>
inline
Allocator<T>::Allocator(const Allocator<U>& rhs)
: d_mechanism(rhs.mechanism())
{
}

// MANIPULATORS
template <class T>
inline
typename Allocator<T>::pointer
Allocator<T>::allocate(typename Allocator::size_type  n,
                       const void                    *hint)
{
    // Both 'bslma::Allocator::size_type' and 'Allocator<T>::size_type' have
    // the same width; however, the former is signed, but the latter is not.
    // Hence the cast in the argument of 'allocate' below.

    (void) hint;  // suppress warning
    return static_cast<pointer>(d_mechanism->allocate(
                     BloombergLP::bslma::Allocator::size_type(n * sizeof(T))));
}

template <class T>
inline
void Allocator<T>::deallocate(typename Allocator::pointer   p,
                              typename Allocator::size_type n)
{
    (void) n;  // suppress warning
    d_mechanism->deallocate(p);
}

template <class T>
inline
void Allocator<T>::construct(typename Allocator::pointer p,
                             const T&                    val)
{
    new(static_cast<void*>(p)) T(val);
}

template <class T>
inline
void Allocator<T>::destroy(typename Allocator::pointer p)
{
    p->~T();
}

// ACCESSORS
template <class T>
inline
typename Allocator<T>::size_type Allocator<T>::max_size() const
{
    // Return the largest value, 'v', such that 'v * sizeof(T)' fits in a
    // 'size_type'.

    // TBD Should these 'const' variables be declared 'static'?
    static const std::size_t MAX_NUM_BYTES    = ~std::size_t(0);
    static const std::size_t MAX_NUM_ELEMENTS =
                                        std::size_t(MAX_NUM_BYTES) / sizeof(T);

    return MAX_NUM_ELEMENTS;
}

template <class T>
inline
typename Allocator<T>::pointer Allocator<T>::address(reference x) const
{
    return BSLS_UTIL_ADDRESSOF(x);
}

template <class T>
inline
typename Allocator<T>::const_pointer Allocator<T>::address(const_reference x)
                                                                          const
{
    return BSLS_UTIL_ADDRESSOF(x);
}

class TestType{
    // DATA
    unsigned char d_data1[32];

    static int s_numCopyConstruct;
    static int s_numDestroy;

  public:
    // CLASS METHODS
    static int numCopyConstruct() { return s_numCopyConstruct; }
    static int numDestroy() { return s_numDestroy; }

    // CREATORS
    TestType() {}
    TestType(const TestType&) { ++s_numCopyConstruct; }
    ~TestType() { ++s_numDestroy; }
};

// FREE OPERATORS
template <class T1, class T2>
inline
bool operator==(const Allocator<T1>& lhs, const Allocator<T2>& rhs)
{
    return lhs.mechanism() == rhs.mechanism();
}

template <class T1, class T2>
inline
bool operator!=(const Allocator<T1>& lhs, const Allocator<T2>& rhs)
{
    return ! (lhs == rhs);
}


int TestType::s_numCopyConstruct = 0;
int TestType::s_numDestroy = 0;

}  // close unnamed namespace

//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a Fixed-Size Array with 'bslalg::ContainerBase'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we would like to implement a fixed-size array that allocate memory
// on the heap at construction.
//
// First, we define the interface of the container, 'MyFixedSizeArray', that
// derives from 'ContainerBase'.  The implementation is elided for brevity:
//..
    template <class VALUE, class ALLOCATOR>
    class MyFixedSizeArray : private bslalg::ContainerBase<ALLOCATOR>
        // This class implements a container that contains a fixed number of
        // elements of the parameterized type 'VALUE' using the parameterized
        // 'ALLOCATOR' to allocate memory.  The number of elements is specified
        // on construction.
    {
//..
// Notice that to use this component, a class should derive from
// 'ContainerBase' in order to take advantage of empty-base optimization.
//..
        // DATA
        VALUE     *d_array;  // head pointer to the array of elements
        const int  d_size;   // (fixed) number of elements in 'd_array'
//
      public:
        // CREATORS
        MyFixedSizeArray(int size, const ALLOCATOR& allocator = ALLOCATOR());
            // Create a 'MyFixedSizeArray' object having the specified 'size'
            // elements, and using the specified 'allocator' to supply memory.
//
        MyFixedSizeArray(const MyFixedSizeArray& original,
                         const ALLOCATOR&        allocator = ALLOCATOR());
            // Create a 'MyFixedSizeArray' object having same number of
            // elements as that of the specified 'original', the same value of
            // each element as that of corresponding element in 'original', and
            // using the specified 'allocator' to supply memory.
//
        ~MyFixedSizeArray();
            // Destroy this object.
//
        // MANIPULATORS
        VALUE& operator[](int i);
            // Return the reference of the specified 'i'th element of this
            // object.  The behavior is undefined unless 'i < size()'.
//
        // ACCESSORS
        int size() const;
            // Return the number of elements contained in this object.
    };
//..

template<class VALUE, class ALLOCATOR>
MyFixedSizeArray<VALUE,ALLOCATOR>::MyFixedSizeArray(int              size,
                                                    const ALLOCATOR& allocator)
: bslalg::ContainerBase<ALLOCATOR>(allocator)
, d_size(size)
{
    d_array = this->allocate(d_size);  // sizeof(VALUE)*d_size bytes

    // Default construct each element of the array:
    for (int i = 0; i < d_size; ++i) {
        this->construct(&d_array[i], VALUE());
    }
}

template<class VALUE, class ALLOCATOR>
MyFixedSizeArray<VALUE,ALLOCATOR>::MyFixedSizeArray(
                                             const MyFixedSizeArray& rhs,
                                             const ALLOCATOR&        allocator)
: bslalg::ContainerBase<ALLOCATOR>(allocator)
, d_size(rhs.d_size)
{
    d_array = this->allocate(d_size);  // sizeof(VALUE) * d_size bytes

    // copy construct each element of the array:
    for (int i = 0; i < d_size; ++i) {
        this->construct(&d_array[i], rhs.d_array[i]);
    }
}

template<class VALUE, class ALLOCATOR>
MyFixedSizeArray<VALUE,ALLOCATOR>::~MyFixedSizeArray()
{
    // Call destructor for each element
    for (int i = 0; i < d_size; ++i) {
        this->destroy(&d_array[i]);
    }

    // Return memory to allocator.
    this->deallocate(d_array, d_size);
}

template<class VALUE, class ALLOCATOR>
inline VALUE& MyFixedSizeArray<VALUE,ALLOCATOR>::operator[](int i)
{
    return d_array[i];
}

template<class VALUE, class ALLOCATOR>
inline int MyFixedSizeArray<VALUE,ALLOCATOR>::size() const
{
    return d_size;
}

template<class VALUE, class ALLOCATOR>
bool operator==(const MyFixedSizeArray<VALUE,ALLOCATOR>& lhs,
                const MyFixedSizeArray<VALUE,ALLOCATOR>& rhs)
{
    if (lhs.size() != rhs.size()) {
        return false;                                                 // RETURN
    }
    for (int i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;                                             // RETURN
        }
    }
    return true;
}



//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
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
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

// Finally, assuming we have a STL compliant allocator named 'Allocator', we
// can create a 'MyFixedSizeArray' object and populate it with data.
//..
    MyFixedSizeArray<int, Allocator<int> > fixedArray(3);
    fixedArray[0] = 1;
    fixedArray[1] = 2;
    fixedArray[2] = 3;
//
    ASSERT(fixedArray[0] == 1);
    ASSERT(fixedArray[1] == 2);
    ASSERT(fixedArray[2] == 3);
//..

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
        //: 1 Invoke all methods and verify their behavior.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator ta;
        Allocator<int> a(&ta);

        typedef bslalg::ContainerBase<Allocator<TestType> > Obj;

        Obj mX(a);

        for (int i = 1; i <= 5; ++i) {
            bslma::TestAllocatorMonitor tam(&ta);

            Obj::pointer ptr = mX.allocate(i);
            ASSERTV(i, i * static_cast<ptrdiff_t>(sizeof(TestType))
                                                        == ta.numBytesInUse());

            int nc = TestType::numCopyConstruct();
            mX.construct(ptr, TestType());
            ASSERTV(TestType::numCopyConstruct(), nc + 1,
                    TestType::numCopyConstruct() == nc + 1);


            int nd = TestType::numDestroy();
            mX.destroy(ptr);
            ASSERTV(TestType::numDestroy(), nd + 1,
                    TestType::numDestroy() == nd + 1);

            mX.deallocate(ptr);
            ASSERTV(0 == ta.numBytesInUse());


            int *intPtr = 0;
            intPtr = mX.allocateN(intPtr, i);
            ASSERTV(i, i * static_cast<ptrdiff_t>(sizeof(int))
                                                        == ta.numBytesInUse());

            mX.deallocateN(intPtr, i);
            ASSERTV(i, 0 == ta.numBytesInUse());


            ASSERTV(i, true == tam.isTotalUp());
        }

        if (verbose) printf("Test 'equalAllocator'\n");
        {
            bslma::TestAllocator oa1, oa2;
            Allocator<int> a1(&oa1);
            Allocator<int> a2(&oa2);

            Obj mX(a1);
            Obj mY(a2);

            ASSERTV(true  == mX.equalAllocator(mX));
            ASSERTV(false == mX.equalAllocator(mY));
        }

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
