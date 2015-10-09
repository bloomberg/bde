// bslalg_rangecompare.t.cpp                                          -*-C++-*-

#include <bslalg_rangecompare.h>

#include <bslmf_isbitwiseequalitycomparable.h>          // for testing only
#include <bslma_usesbslmaallocator.h>                   // for testing only
#include <bslmf_nestedtraitdeclaration.h>               // for testing only
#include <bsls_bsltestutil.h>                           // for testing only

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_bsltestutil.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include <new>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides equality and less-than comparisons on ranges given
// by start and end iterators and, optionally, length, all of parameterizable
// types.  Where length is not provided, the component either calculates it (in
// cases where the iterator is convertible to a 'const VALUE_TYPE *') or falls
// back on an inferior implementation requiring two tests per loop iteration,
// instead of one (in cases where the iterator is not convertible to a
// 'const VALUE_TYPE *').
//
// The implementation uses traits to efficiently forward to 'std::memcmp' or
// 'std::wmemcmp'.  All fundamental and pointer types have the bit-wise
// equality-comparable trait.  The concerns range from correctness of
// implementation to correct selection of traits.  There is no memory
// allocation and therefore there are no exception-related concerns in this
// component.  We address this with two custom test types, one that has the
// bit-wise equality-comparable trait and does not define 'operator==' (to
// ensure that it will not be compiled), and another that has an 'operator=='
// but no trait.  Finally, in order to trigger instantiation of all possible
// overloads, we wrap the range pointers into an iterator type that is *not*
// convertible to the pointer.
//-----------------------------------------------------------------------------
// [ 3] bool equal(start1, end1, length1, start2, end2, length2);
// [ 4] bool lexicographical(start1, end1, length1, start2, end2, length2);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TEST APPARATUS
// [-1] PERFORMANCE TEST
// [ 5] USAGE EXAMPLE

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
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.

inline
void dbg_print(char c)
{
    printf("%c", c);
    fflush(stdout);
}

inline
void dbg_print(unsigned char c)
{
    printf("%c", c);
    fflush(stdout);
}

inline
void dbg_print(signed char c)
{
    printf("%c", c);
    fflush(stdout);
}

inline
void dbg_print(short val)
{
    printf("%d", (int)val);
    fflush(stdout);
}

inline
void dbg_print(unsigned short val)
{
    printf("%d", (int)val);
    fflush(stdout);
}

inline
void dbg_print(int val)
{
    printf("%d", val);
    fflush(stdout);
}

inline
void dbg_print(unsigned val)
{
    printf("%u", val);
    fflush(stdout);
}

#ifdef _MSC_VER

inline
void dbg_print(bsls::Types::Int64 val)
{
    printf("%I64d", val);
    fflush(stdout);
}

inline
void dbg_print(bsls::Types::Uint64 val)
{
    printf("%I64u", val);
    fflush(stdout);
}

#else

inline
void dbg_print(bsls::Types::Int64 val)
{
    printf("%lld", val);
    fflush(stdout);
}

inline
void dbg_print(bsls::Types::Uint64 val)
{
    printf("%llu", val);
    fflush(stdout);
}

#endif

inline
void dbg_print(float val)
{
    printf("'%f'", (double)val);
    fflush(stdout);
}

inline
void dbg_print(double val)
{
    printf("'%f'", val);
    fflush(stdout);
}

inline
void dbg_print(const char *s)
{
    printf("\"%s\"", s);
    fflush(stdout);
}

inline
void dbg_print(const void  *p)
{
    printf("\"%p\"", p);
    fflush(stdout);
}

//=============================================================================
//                     TEST APPARATUS FOR USAGE EXAMPLE
//-----------------------------------------------------------------------------

// Define one container type, 'MyContainer', and two value-semantic types,
// 'MyString' and 'MyPoint', suitable for implementing the usage example.
// These classes use the standard BDE allocator model, however the
// bslalg_scalarprimitives package is not available to bslalg_rangecompare, so
// we also implement an elided struct 'ScalarPrimitives' to provide equivalent
// functionality.

                        // =======================
                        // struct ScalarPrimitives
                        // =======================

struct ScalarPrimitives {
    // This 'struct' provides a namespace for an elided suite of utility
    // functions that operate on elements of (template parameter) type
    // 'TARGET_TYPE'.  The functions provided allow us to call the copy
    // constructor or destructor of 'TARGET_TYPE', correctly taking into
    // account whether or not 'TARGET_TYPE' uses a 'bslma::Allocator'.

  private:
    // PRIVATE CLASS METHODS
    template <class TARGET_TYPE>
    static void doCopyConstruct(TARGET_TYPE         *address,
                                const TARGET_TYPE&   original,
                                bslma::Allocator    *allocator,
                                bsl::false_type);
        // Build an object of the (template parameter) type 'TARGET_TYPE',
        // which does not use a 'bslma::Allocator', from the specified
        // 'original' object of the same 'TARGET_TYPE' in the uninitialized
        // memory at the specified 'address', as if by using the copy
        // constructor of 'TARGET_TYPE'.

    template <class TARGET_TYPE>
    static void doCopyConstruct(TARGET_TYPE         *address,
                                const TARGET_TYPE&   original,
                                bslma::Allocator    *allocator,
                                bsl::true_type);
        // Build an object of the (template parameter) type 'TARGET_TYPE',
        // which uses a 'bslma::Allocator', from the specified 'original'
        // object of the same 'TARGET_TYPE' in the uninitialized memory at the
        // specified 'address', as if by using the copy constructor of
        // 'TARGET_TYPE'.

  public:
    // CLASS METHODS
    template <class TARGET_TYPE>
    static void copyConstruct(TARGET_TYPE        *address,
                              const TARGET_TYPE&  original,
                              bslma::Allocator   *allocator);
        // Build an object of (template parameter) type 'TARGET_TYPE' from the
        // specified 'original' object of the same 'TARGET_TYPE' in the
        // uninitialized memory at the specified 'address', as if by using the
        // copy constructor of 'TARGET_TYPE'.  If the constructor throws, the
        // 'address' is left in an uninitialized state.

    template <class TARGET_TYPE>
    static void destroy(TARGET_TYPE *object);
        // Destroy the specified 'object' of (template parameter) type
        // 'TARGET_TYPE', as if by calling the 'TARGET_TYPE' destructor, but do
        // not deallocate the memory occupied by 'object'.  Note that the
        // destructor may deallocate other memory owned by 'object'.
};

template <class TARGET_TYPE>
void ScalarPrimitives::doCopyConstruct(TARGET_TYPE         *address,
                                       const TARGET_TYPE&   original,
                                       bslma::Allocator    *allocator,
                                       bsl::false_type)
{
    (void) allocator;
    new (address) TARGET_TYPE(original);
}

template <class TARGET_TYPE>
void ScalarPrimitives::doCopyConstruct(TARGET_TYPE         *address,
                                       const TARGET_TYPE&   original,
                                       bslma::Allocator    *allocator,
                                       bsl::true_type)
{
    new (address) TARGET_TYPE(original, allocator);
}

template <class TARGET_TYPE>
void ScalarPrimitives::copyConstruct(TARGET_TYPE               *address,
                                            const TARGET_TYPE&  original,
                                            bslma::Allocator   *allocator)
{
    BSLS_ASSERT_SAFE(address);

    typedef typename bslma::UsesBslmaAllocator<TARGET_TYPE>::type Trait;
    doCopyConstruct(address, original, allocator, Trait());
}

template <>
void ScalarPrimitives::copyConstruct<int>(int              *address,
                                          const int&        original,
                                          bslma::Allocator *allocator)
{
    (void) allocator;
    *address = original;
}

template <class TARGET_TYPE>
void ScalarPrimitives::destroy(TARGET_TYPE *object)
{
    object->~TARGET_TYPE();
}

template <>
void ScalarPrimitives::destroy<int>(int *object)
{
    (void) object;
}

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Defining Equality Comparison Operators on a Container
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we will use the 'bslalg::RangeCompare::equal' class method
// to implement the equality comparison operators for an iterable container
// type residing in the 'bslstl' package, and highlight the circumstances under
// which the optimization provided by the class method may be applied.
//
// Suppose that we have a new iterable container type that will be included in
// the 'bslstl' package, and we wish to define comparison operators for the
// container.  If the container has an iterator that provides access to the
// container's elements in a consistent order, and the elements themselves are
// equality-comparable, we can implement the container's equality comparison
// operators by pair-wise comparing each of the elements over the entire range
// of elements in both containers.  In such cases the container can use the
// 'bslalg::RangeCompare::equal' class method to equal-compare the container's
// elements, taking advantage of the optimizations the class method provides
// for bit-wise equality-comparable objects.
//
// First, we create an elided definition of a container class, 'MyContainer',
// which provides read-only iterators of the type 'MyContainer::ConstIterator':
//..
                        // =================
                        // class MyContainer
                        // =================

template <class VALUE_TYPE>
class MyContainer {
    // This class implements a container, semantically similar to
    // 'std::vector', holding objects of the (template parameter) type
    // 'VALUE_TYPE'.

  private:
    // DATA
    std::size_t       d_size;         // number of elements currently stored
                                      // in the container

    std::size_t       d_capacity;     // capacity of the container

    VALUE_TYPE       *d_start_p;      // allocated memory for the container

    bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)

  private:
    // NOT IMPLEMENTED
    MyContainer(const MyContainer&);
    MyContainer& operator=(const MyContainer&);

  public:
    // PUBLIC TYPES
    typedef const VALUE_TYPE *ConstIterator;
        // This 'typedef' provides an alias for the type of iterator
        // providing non-modifiable access to the elements in the
        // container.

    // CREATORS
    explicit MyContainer(bslma::Allocator *basicAllocator = 0);
        // Create an empty 'MyContainer' object having no capacity.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    explicit MyContainer(std::size_t capacity,
                         bslma::Allocator *basicAllocator = 0);
        // Create an empty 'MyContainer' object having a capacity equal to
        // the specified 'capacity'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~MyContainer();
        // Destroy this object.

    // MANIPULATORS
    void reserve(std::size_t newCapacity);
        // Change the capacity of this vector to the specified
        // 'newCapacity'.  Note that the capacity of a vector is the
        // maximum number of elements it can accommodate without
        // reallocation.  The actual storage allocated may be higher.

    void push_back(const VALUE_TYPE& value);
        // Append the specified 'value' at the past-the-end position in
        // this container, increasing the container's capacity if needed.

    // ...

    // ACCESSORS
    ConstIterator begin() const;
        // Return an iterator providing non-modifiable access to the first
        // element in this container.

    ConstIterator end() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in this container.

    std::size_t size() const;
        // Return the number of elements in this container.

    // ...
};

// CREATORS
template <class VALUE_TYPE>
MyContainer<VALUE_TYPE>::MyContainer(bslma::Allocator *basicAllocator)
: d_size(0)
, d_capacity(0)
, d_start_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(d_allocator_p);
}

template <class VALUE_TYPE>
MyContainer<VALUE_TYPE>::MyContainer(std::size_t capacity,
                                     bslma::Allocator *basicAllocator)
: d_size(0)
, d_capacity(0)
, d_start_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
        BSLS_ASSERT(d_allocator_p);

        d_start_p =
          (VALUE_TYPE *) d_allocator_p->allocate(capacity * sizeof *d_start_p);
        BSLS_ASSERT(d_start_p);
        d_capacity = capacity;
}

template <class VALUE_TYPE>
MyContainer<VALUE_TYPE>::~MyContainer()
{
    if (d_start_p) {
        while (d_size > 0) {
            ScalarPrimitives::destroy<VALUE_TYPE>(&d_start_p[d_size - 1]);
            --d_size;
        }
        BSLS_ASSERT(0 == d_size);
        d_allocator_p->deallocate(d_start_p);
    }
}

// MANIPULATORS
template <class VALUE_TYPE>
void MyContainer<VALUE_TYPE>::reserve(std::size_t newCapacity)
{
    BSLS_ASSERT(newCapacity >= d_size);

    if (newCapacity == d_capacity) {
        // No change in size, so nothing to do.
           return;                                                    // RETURN
    }

    // Build replacement container.
    MyContainer replacement(newCapacity, d_allocator_p);
    BSLS_ASSERT(d_start_p || 0 == d_size);
    BSLS_ASSERT(0 == replacement.d_size);
    while (replacement.d_size < d_size) {
        replacement.push_back(d_start_p[replacement.d_size]);
    }

    // Swap our state with replacement.
    // d_size and d_allocator_p are already the same,
    // so they can be skipped.
    {
        std::size_t temp = d_capacity;
        d_capacity = replacement.d_capacity;
        replacement.d_capacity = temp;
    }
    {
        VALUE_TYPE *temp = d_start_p;
        d_start_p = replacement.d_start_p;
        replacement.d_start_p = temp;
    }

    // Old elements will be destroyed when replacement goes out of scope.
}

template <class VALUE_TYPE>
void MyContainer<VALUE_TYPE>::push_back(const VALUE_TYPE& value)
{
    if (0 == d_capacity) {
        // Container has no storage.
        BSLS_ASSERT(0 == d_size);
        reserve(1);
    } else if (d_capacity == d_size) {
        // Container is at capacity.
        reserve(d_capacity * 2);
    }

    BSLS_ASSERT(d_size < d_capacity);

    ScalarPrimitives::copyConstruct<VALUE_TYPE>(&d_start_p[d_size],
                                                 value,
                                                 d_allocator_p);
    ++d_size;
}

// ACCESSORS
template <class VALUE_TYPE>
typename MyContainer<VALUE_TYPE>::ConstIterator
MyContainer<VALUE_TYPE>::begin() const
{
    return d_start_p;
}

template <class VALUE_TYPE>
typename MyContainer<VALUE_TYPE>::ConstIterator
MyContainer<VALUE_TYPE>::end() const
{
    return d_start_p + d_size;
}

template <class VALUE_TYPE>
std::size_t MyContainer<VALUE_TYPE>::size() const
{
    return d_size;
}
//..
// Notice that 'ConstIterator' is defined as a pointer type, which is one of
// the criteria required to enable the optimizations provided by the
// 'bslalg::RangeCompare::equal' class method.
//
// Then, we declare the equality comparison operators for 'MyContainer':
//..
template <class VALUE_TYPE>
bool operator==(const MyContainer<VALUE_TYPE>& lhs,
                const MyContainer<VALUE_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MyContainer' objects have the
    // same value if they have the same length, and each element in 'lhs'
    // has the same value as the corresponding element in 'rhs'.

template <class VALUE_TYPE>
bool operator!=(const MyContainer<VALUE_TYPE>& lhs,
                const MyContainer<VALUE_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
    // the same value, and 'false' otherwise.  Two 'MyContainer' objects do
    // not have the same value if they do not have the same length, or if
    // any element in 'lhs' does not have the same value as the
    // corresponding element in 'rhs'.
//..
// Next, we implement the equality comparison operators using
// 'bslalg::RangeCompare::equal':
//..
template <class VALUE_TYPE>
inline
bool operator==(const MyContainer<VALUE_TYPE>& lhs,
                const MyContainer<VALUE_TYPE>& rhs)
{
    return BloombergLP::bslalg::RangeCompare::equal(lhs.begin(),
                                                    lhs.end(),
                                                    lhs.size(),
                                                    rhs.begin(),
                                                    rhs.end(),
                                                    rhs.size());
}

template <class VALUE_TYPE>
inline
bool operator!=(const MyContainer<VALUE_TYPE>& lhs,
                const MyContainer<VALUE_TYPE>& rhs)
{
    return !BloombergLP::bslalg::RangeCompare::equal(lhs.begin(),
                                                     lhs.end(),
                                                     lhs.size(),
                                                     rhs.begin(),
                                                     rhs.end(),
                                                     rhs.size());
}
//..
// Then, we create the elided definition of a value-semantic class, 'MyString',
// together with its definition of 'operator==':
//..
                              // ==============
                              // class MyString
                              // ==============

class MyString {
    // This class provides a simple, elided string class that conforms to
    // the 'bslma::Allocator' model.

  private:
    // DATA
    char             *d_start_p;      // storage for the string
    std::size_t       d_length;       // length of the string
    bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)

    // PRIVATE MANIPULATORS
    void set(const char *sourceStr, std::size_t length);
        // Assign the value of the specified 'sourceStr', of length
        // 'length', to this 'MyString' object.

    // FRIENDS
    friend bool operator==(const MyString&, const MyString&);
    friend bool operator!=(const MyString&, const MyString&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MyString,
                                   BloombergLP::bslma::UsesBslmaAllocator);

    // CREATORS
    explicit MyString(const char       *string,
                      bslma::Allocator *basicAllocator = 0);
        // Create a 'MyString' object initialized to the value of the
        // specified 'string'.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    MyString(const MyString&   original,
             bslma::Allocator *basicAllocator = 0);
        // Create a 'MyString' object initialized to the value of the
        // specified 'original' 'MyString'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    ~MyString();
        // Destroy this object.

    // MANIPULATORS
    MyString& operator=(const MyString& rhs);
        // Assign to this object the value of the specified 'rhs' object,
        // and return a reference providing modifiable access to this
        // object.

    // ACCESSORS
    const char *c_str() const;
        // Return a null-terminated byte string representing the value of
        // this 'MyString'.

    std::size_t length() const;
        // Return the number of characters in this 'MyString'.
};

bool operator==(const MyString& lhs, const MyString& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MyString' objects have the same
    // value if the strings they represent are lexicographically equal.

bool operator!=(const MyString& lhs, const MyString& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
    // the same value, and 'false' otherwise.  Two 'MyString' objects do
    // not have the same value if the strings they represent are not
    // lexicographically equal.

MyString::MyString(const char *sourceStr, bslma::Allocator *basicAllocator)
: d_start_p(0)
, d_length(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    set(sourceStr, strlen(sourceStr));
}

MyString::MyString(const MyString& rhs, bslma::Allocator *basicAllocator)
: d_start_p(0)
, d_length(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    set(rhs.d_start_p, rhs.d_length);
}

MyString::~MyString()
{
    d_allocator_p->deallocate(d_start_p);
}

MyString& MyString::operator=(const MyString& rhs)
{
    if (this != &rhs) {
        d_allocator_p->deallocate(d_start_p);
        set(rhs.d_start_p, rhs.d_length);
    }
    return *this;
}

void MyString::set(const char *sourceStr, std::size_t length)
{
    d_length = length;
    d_start_p =
           (char *) d_allocator_p->allocate((length + 1) * sizeof *d_start_p);
    memcpy(d_start_p, sourceStr, length);
    d_start_p[length] = '\0';
}

const char *MyString::c_str() const
{
    return d_start_p;
}

std::size_t MyString::length() const
{
    return d_length;
}

bool operator==(const MyString& lhs, const MyString& rhs)
{
    return lhs.d_length == rhs.d_length
        && 0 == std::strncmp(lhs.d_start_p, rhs.d_start_p, lhs.d_length);
}

bool operator!=(const MyString& lhs, const MyString& rhs)
{
    return ! (lhs == rhs);
}
//..
// Notice that 'MyString' is not bit-wise comparable because the address values
// of the 'd_start_p' pointer data members in two 'MyString' objects will be
// different, even if the string values of the two objects are the same.
//
// Next, we create two 'MyContainer<MyString>' objects, and compare them using
// 'operator==':
//..
void usageTestMyString()
{
    MyContainer<MyString> c1;
    MyContainer<MyString> c2;

    c1.push_back(MyString("hello"));
    c1.push_back(MyString("goodbye"));

    c2.push_back(MyString("hello"));
    c2.push_back(MyString("goodbye"));

    ASSERT(c1 == c2);
}
//..
// Here, the call to the 'bslalg::RangeCompare::equal' class method in
// 'operator==' will perform an unoptimized pair-wise comparison of the
// elements in 'c1' and 'c2'.
//
// Then, we create the elided definition of another value-semantic class,
// 'MyPoint', together with its definition of 'operator==':
//..
                              // =============
                              // class MyPoint
                              // =============

class MyPoint {
    // This class provides a simple, elided point type that is bit-wise
    // comparable with other objects of the same type.

  private:
    // DATA
    int d_x;  // the x-coordinate of the point
    int d_y;  // the y-coordinate of the point

    // FRIENDS
    friend bool operator==(const MyPoint&, const MyPoint&);
    friend bool operator!=(const MyPoint&, const MyPoint&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MyPoint,
                          BloombergLP::bslmf::IsBitwiseEqualityComparable);

    // CREATORS
    MyPoint(int x, int y);
        // Create a 'MyPoint' object whose x- and y-coordinates have the
        // specified 'x' and 'y' values, respectively.

    MyPoint(const MyPoint& original);
        // Create a 'MyPoint' object initialized to the same value as the
        // specified 'original'.

    // ...

    // MANIPULATORS
    MyPoint& operator=(const MyPoint& rhs);
        // Assign to this object the value of the specified 'rhs' object,
        // and return a reference providing modifiable access to this
        // object.
};

bool operator==(const MyPoint& lhs, const MyPoint& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MyPoint' objects have the same value
    // if the x-coordinate and y-coordinate of one object have the same values
    // as the corresponding coordinates of the other object.

bool operator!=(const MyPoint& lhs, const MyPoint& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'MyPoint' objects do not have
    // the same value if either the x-coordinate or y-coordinate of one object
    // does not have the same value as the cooresponding coordinate of the
    // other object.

MyPoint::MyPoint(int x, int y)
: d_x(x)
, d_y(y)
{
}

MyPoint::MyPoint(const MyPoint& original)
: d_x(original.d_x)
, d_y(original.d_y)
{
}

MyPoint& MyPoint::operator=(const MyPoint& rhs)
{
    d_x = rhs.d_x;
    d_y = rhs.d_y;

    return *this;
}

bool operator==(const MyPoint& lhs, const MyPoint& rhs)
{
    return lhs.d_x == rhs.d_x && lhs.d_y == rhs.d_y;
}

bool operator!=(const MyPoint& lhs, const MyPoint& rhs)
{
    return !(lhs == rhs);
}
//..
// Notice that the value of a 'MyPoint' object derives from the values of all
// of its data members, and that no padding is required for alignment.
// Furthermore, 'MyPoint' has no virtual methods.  Therefore, 'MyPoint' objects
// are bit-wise comparable, and we can correctly declare the
// 'bslmf::IsBitwiseEqualityComparable' trait for the class, as shown
// above under the public 'TRAITS' section.
//
// Now, we create two 'MyContainer<MyPoint>' objects and compare them using
// 'operator==':
//..
void usageTestMyPoint()
{
    MyContainer<MyPoint> c3;
    MyContainer<MyPoint> c4;

    c3.push_back(MyPoint(1, 2));
    c3.push_back(MyPoint(3, 4));

    c4.push_back(MyPoint(1, 2));
    c4.push_back(MyPoint(3, 4));

    ASSERT(c3 == c4);
}
//..
// Here, the call to 'bslalg::RangeCompare::equal' in 'operator==' may take
// advantage of the fact that 'MyPoint' is bit-wise comparable and perform the
// comparison by directly bit-wise comparing the entire range of elements
// contained in the 'MyContainer<MyPoint>' objects.  This comparison can
// provide a significant performance boost over the comparison between two
// 'MyContainer<MyPoint>' objects in which the nested
// 'bslmf::IsBitwiseEqualityComparable' trait is not associated with the
// 'MyPoint' class.
//
// Finally, note that we can instantiate 'MyContainer' with 'int' or any other
// primitive type as the 'VALUE_TYPE' and still benefit from the optimized
// comparison operators, because primitive (i.e.: fundamental, enumerated, and
// pointer) types are inherently bit-wise comparable:
//..
void usageTestInt()
{
    MyContainer<int> c5;
    MyContainer<int> c6;

    c5.push_back(1);
    c5.push_back(2);
    c5.push_back(3);

    c6.push_back(1);
    c6.push_back(2);
    c6.push_back(3);

    ASSERT(c5 == c6);
}

//=============================================================================
//                     TEST APPARATUS FOR CASES 1 - 3
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL TYPES/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsls::Types::Uint64    Uint64;
typedef bsls::Types::Int64     Int64;

typedef bslalg::RangeCompare   Obj;

                        // ===========================
                        // class NonBitwiseWithOpEqual
                        // ===========================

class NonBitwiseWithOpEqual {
    // Class that doesn't have the bit-wise equality-comparable traits, but has
    // an 'operator=='.  In order to catch bit-wise comparison, we stuff
    // padding with an extra byte filled with a random value.

    // CLASS DATA
    static char d_seed;

    // DATA
    char d_char;
    char d_random;

  public:
    // CREATORS
    NonBitwiseWithOpEqual(char value = '_');

    // ACCESSORS
    char datum() const;
};

// CLASS DATA
char NonBitwiseWithOpEqual::d_seed = 92;

// CREATORS
NonBitwiseWithOpEqual::NonBitwiseWithOpEqual(char value)
: d_char(value)
, d_random(++d_seed)
{
}

// ACCESSORS
char NonBitwiseWithOpEqual::datum() const
{
    return d_char;
}

// FREE OPERATORS
bool operator==(const NonBitwiseWithOpEqual& lhs,
                const NonBitwiseWithOpEqual& rhs)
{
    return lhs.datum() == rhs.datum();
}

bool operator!=(const NonBitwiseWithOpEqual& lhs,
                const NonBitwiseWithOpEqual& rhs)
{
    return lhs.datum() != rhs.datum();
}

bool operator< (const NonBitwiseWithOpEqual& lhs,
                const NonBitwiseWithOpEqual& rhs)
{
    return lhs.datum() < rhs.datum();
}

                           // ======================
                           // class BitWiseNoOpEqual
                           // ======================

class BitWiseNoOpEqual {
    // Class that has the bit-wise equality-comparable traits, and no
    // 'operator=='.

    // DATA
    char d_char;

  public:
    // CREATORS
    BitWiseNoOpEqual(char value = '_');

    // ACCESSORS
    char datum() const;
};

// TRAITS
namespace BloombergLP {
namespace bslmf {
template <> struct IsBitwiseEqualityComparable<BitWiseNoOpEqual>
    : bsl::true_type {};
}  // close namespace bslmf
}  // close enterprise namespace

// CREATORS
BitWiseNoOpEqual::BitWiseNoOpEqual(char value)
: d_char(value)
{
}

// ACCESSORS
char BitWiseNoOpEqual::datum() const
{
    return d_char;
}

// FREE OPERATORS
bool operator<(const BitWiseNoOpEqual& lhs, const BitWiseNoOpEqual& rhs)
{
    return lhs.datum() < rhs.datum();
}

                 // =========================================
                 // class CharEquivalentNonBitwiseWithOpEqual
                 // =========================================

class CharEquivalentNonBitwiseWithOpEqual : public NonBitwiseWithOpEqual {
    // Class that doesn't have the bit-wise equality-comparable traits, but has
    // an 'operator==' *and* is convertible to char.

  public:
    // CREATORS
    CharEquivalentNonBitwiseWithOpEqual(char value = '_');

    // ACCESSORS
    operator char() const;
};

// CREATORS
CharEquivalentNonBitwiseWithOpEqual::CharEquivalentNonBitwiseWithOpEqual(
                                                                   char value)
: NonBitwiseWithOpEqual(value)
{
}

// ACCESSORS
CharEquivalentNonBitwiseWithOpEqual::operator char() const
{
    return datum();
}

// FREE OPERATORS
bool operator==(const CharEquivalentNonBitwiseWithOpEqual& lhs,
                const CharEquivalentNonBitwiseWithOpEqual& rhs)
{
    return lhs.datum() == rhs.datum();
}

bool operator!=(const CharEquivalentNonBitwiseWithOpEqual& lhs,
                const CharEquivalentNonBitwiseWithOpEqual& rhs)
{
    return lhs.datum() != rhs.datum();
}

bool operator< (const CharEquivalentNonBitwiseWithOpEqual& lhs,
                const CharEquivalentNonBitwiseWithOpEqual& rhs)
{
    return lhs.datum() < rhs.datum();
}

                             // =================
                             // class my_Iterator
                             // =================

template <class TYPE>
class my_Iterator {
    // This class provides a bare-bones wrapper around a pointer to (template
    // parameter) type 'TYPE' that is enough to apply the 'equal' and
    // 'lexicographical' algorithms for generic iterators.

    // DATA
    const TYPE *d_value_p;

  public:
    // PUBLIC TYPES
    typedef TYPE            value_type;
    typedef std::ptrdiff_t  difference_type;
    typedef const TYPE&     reference;
    typedef const TYPE     *pointer;

    // CREATORS
    explicit
    my_Iterator(const TYPE *pointer);
    // Create an iterator storing the specified 'pointer'.

    // MANIPULATORS
    my_Iterator& operator++();
        // Increment this iterator, and return a modifiable reference to it.

    // ACCESSORS
    const TYPE& operator*() const;
        // Return a reference to the non-modifiable element pointed to by this
        // iterator.

    const TYPE *valuePtr() const;
        // Return the address to the non-modifiable element pointed to by this
        // iterator.
};

// CREATORS
template <class TYPE>
my_Iterator<TYPE>::my_Iterator(const TYPE *iter)
: d_value_p(iter)
{}

// MANIPULATORS
template <class TYPE>
my_Iterator<TYPE>&
my_Iterator<TYPE>::operator++()
{
    ++ d_value_p;
    return *this;
}

// ACCESSORS
template <class TYPE>
const TYPE& my_Iterator<TYPE>::operator*() const
{
    return *d_value_p;
}

template <class TYPE>
const TYPE *my_Iterator<TYPE>::valuePtr() const
{
    return d_value_p;
}

// FREE OPERATORS
template <class TYPE>
bool operator==(const my_Iterator<TYPE>& lhs, const my_Iterator<TYPE>& rhs)
{
    return lhs.valuePtr() == rhs.valuePtr();
}

template <class TYPE>
bool operator!=(const my_Iterator<TYPE>& lhs, const my_Iterator<TYPE>& rhs)
{
    return !(lhs == rhs);
}

//=============================================================================
//              GENERATOR FUNCTIONS 'gg' AND 'ggg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure an array according to a custom language.  Letters
// [a .. z, A .. Z] correspond to the corresponding char values used to
// initialize elements of an array of 'T' objects.

enum { GGG_SUCCESS = -1 };

template <class TYPE>
int ggg(TYPE *array, const char *spec, int verboseFlag = 1)
    // Configure the specified 'array' of objects of (template parameter) type
    // 'TYPE' (assumed to be uninitialized) according to the specified 'spec'.
    // Optionally specify a zero 'verboseFlag' to suppress 'spec' syntax error
    // messages.  Return the index of the first invalid character, and a
    // negative value otherwise.  Note that this function is used to implement
    // 'gg' as well as allow for verification of syntax error detection.
{
    for (int i = 0; spec[i]; ++i, ++array) {
        if (!isalpha(spec[i])) {
            if (verboseFlag) {
                printf("Error, bad character ('%c') in spec \"%s\""
                       " at position %d.\n", spec[i], spec, i);
            }
            return i;                                                 // RETURN
                // Discontinue processing this spec.
        }
        *array = spec[i];
    }
    return GGG_SUCCESS;
}

template <class TYPE>
TYPE& gg(TYPE *array, const char *spec)
    // Return a reference to the modifiable first element of the specified
    // 'array' after the value of 'array' has been adjusted according to the
    // specified 'spec'.
{
    ASSERT(ggg(array, spec) < 0);
    return *array;
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 4
//-----------------------------------------------------------------------------

static const struct {
    int          d_lineNum;
    const char  *d_string_p;
} DATA_CASE4[] = {
    // All combinations of up to three letters, then all combinations of
    // identical four or five letters with up to one letter difference.
    // All letters belong to [abc].
    //
    // line   string
    // ----   ------
    {  L_,    "",               },
    {  L_,    "a",              },
    {  L_,    "aa",             },
    {  L_,    "aaa",            },
    {  L_,    "aaaa",           },
    {  L_,    "aaaaa",          },
    {  L_,    "aaaab",          },
    {  L_,    "aaaac",          },
    {  L_,    "aaab",           },
    {  L_,    "aaaba",          },
    {  L_,    "aaaca",          },
    {  L_,    "aab",            },
    {  L_,    "aaba",           },
    {  L_,    "aabaa",          },
    {  L_,    "aac",            },
    {  L_,    "aacaa",          },
    {  L_,    "ab",             },
    {  L_,    "aba",            },
    {  L_,    "abaa",           },
    {  L_,    "abaaa",          },
    {  L_,    "abb",            },
    {  L_,    "abbb",           },
    {  L_,    "abbbb",          },
    {  L_,    "abc",            },
    {  L_,    "ac",             },
    {  L_,    "aca",            },
    {  L_,    "acaaa",          },
    {  L_,    "acb",            },
    {  L_,    "acc",            },
    {  L_,    "acccc",          },
    {  L_,    "b",              },
    {  L_,    "ba",             },
    {  L_,    "baa",            },
    {  L_,    "baaa",           },
    {  L_,    "baaaa",          },
    {  L_,    "bab",            },
    {  L_,    "babb",           },
    {  L_,    "babbb",          },
    {  L_,    "bac",            },
    {  L_,    "bb",             },
    {  L_,    "bba",            },
    {  L_,    "bbab",           },
    {  L_,    "bbabb",          },
    {  L_,    "bbb",            },
    {  L_,    "bbba",           },
    {  L_,    "bbbab",          },
    {  L_,    "bbbb",           },
    {  L_,    "bbbba",          },
    {  L_,    "bbbbb",          },
    {  L_,    "bbbbc",          },
    {  L_,    "bbbc",           },
    {  L_,    "bbbcb",          },
    {  L_,    "bbc",            },
    {  L_,    "bbcb",           },
    {  L_,    "bbcbb",          },
    {  L_,    "bc",             },
    {  L_,    "bca",            },
    {  L_,    "bcb",            },
    {  L_,    "bcbb",           },
    {  L_,    "bcbbb",          },
    {  L_,    "bcc",            },
    {  L_,    "bcccc",          },
    {  L_,    "c",              },
    {  L_,    "ca",             },
    {  L_,    "caa",            },
    {  L_,    "caaaa",          },
    {  L_,    "cab",            },
    {  L_,    "cac",            },
    {  L_,    "caccc",          },
    {  L_,    "cb",             },
    {  L_,    "cba",            },
    {  L_,    "cbb",            },
    {  L_,    "cbbb",           },
    {  L_,    "cbbbb",          },
    {  L_,    "cbc",            },
    {  L_,    "cbccc",          },
    {  L_,    "cc",             },
    {  L_,    "cca",            },
    {  L_,    "ccacc",          },
    {  L_,    "ccb",            },
    {  L_,    "ccbcc",          },
    {  L_,    "ccc",            },
    {  L_,    "cccac",          },
    {  L_,    "cccbc",          },
    {  L_,    "cccc",           },
    {  L_,    "cccca",          },
    {  L_,    "ccccb",          },
    {  L_,    "ccccc",          }
};
const int NUM_DATA_CASE4 = sizeof DATA_CASE4 / sizeof *DATA_CASE4;

template <class TYPE>
void testLexicographical(bool verboseFlag, bslma::TestAllocator& testAllocator)
    // Compare every pair of strings of (template parameter) type 'TYPE' from
    // the specifications in the 'DATA_CASE4' array, and verify that they are
    // equal if and only if their specifications are equal.  Note that the
    // range will be passed using 'const TYPE *'.
{
    TYPE *strings[NUM_DATA_CASE4];
    const TYPE **STRINGS = const_cast<const TYPE **>(strings);

    const Int64 NUM_BYTES = testAllocator.numBytesInUse();

    for (int i = 0; i < NUM_DATA_CASE4; ++i) {
        const char   *STRING = DATA_CASE4[i].d_string_p;
        const size_t  LEN    = std::strlen(STRING);
        strings[i] = (TYPE *) testAllocator.allocate(LEN * sizeof(TYPE));
        gg(strings[i], STRING);
    }

    for (int i = 0; i < NUM_DATA_CASE4; ++i) {
        const int     LINE1   = DATA_CASE4[i].d_lineNum;
        const char   *STRING1 = DATA_CASE4[i].d_string_p;
        const size_t  LEN1    = std::strlen(STRING1);

        for (int j = 0; j < NUM_DATA_CASE4; ++j) {
            const int     LINE2   = DATA_CASE4[j].d_lineNum;
            const char   *STRING2 = DATA_CASE4[j].d_string_p;
            const size_t  LEN2    = std::strlen(STRING2);

            const TYPE *LHS_BEGIN = STRINGS[i];
            const TYPE *LHS_END   = STRINGS[i] + LEN1;
            const size_t LHS_LEN   = LEN1;

            const TYPE *RHS_BEGIN = STRINGS[j];
            const TYPE *RHS_END   = STRINGS[j] + LEN2;
            const size_t RHS_LEN   = LEN2;

            const int EXP = (i == j) ? 0 : ((i < j) ? -1 : 1);

            if (verboseFlag) {
                int result = Obj::lexicographical(LHS_BEGIN, LHS_END, LHS_LEN,
                                                  RHS_BEGIN, RHS_END, RHS_LEN);
                printf("LINE1 = %d, STRING1 = \"%s\", LEN1 = " ZU ", "
                       "LINE2 = %d, STRING2 = \"%s\", LEN2 = " ZU "\n",
                       LINE1, STRING1, LEN1, LINE2, STRING2, LEN2);
                printf("EXP = %d, result = %d\n", EXP, result);
            }

            ASSERT((EXP == Obj::lexicographical(LHS_BEGIN, LHS_END,
                                                RHS_BEGIN, RHS_END)));
            ASSERT((EXP == Obj::lexicographical(LHS_BEGIN, LHS_END, LHS_LEN,
                                                RHS_BEGIN, RHS_END, RHS_LEN)));
        }
    }

    for (int i = 0; i < NUM_DATA_CASE4; ++i) {
        testAllocator.deallocate(strings[i]);
    }

    ASSERT(NUM_BYTES == testAllocator.numBytesInUse());
}

CharEquivalentNonBitwiseWithOpEqual charGenerator(int i, int j)
{
    return CharEquivalentNonBitwiseWithOpEqual(DATA_CASE4[i].d_string_p[j]);
}

int intGenerator(int i, int j)
{
    int result = (2 - j) << ((i / 2) % (8 * sizeof(int)));
    result += j << (i / 3);
    return result;
}

Uint64 uint64Generator(int i, int j)
{
    Uint64 result = (2 - j) << ((i / 2) % 64);
    result += j << (i / 3);
    return result;
}

double doubleGenerator(int i, int j)
{
    double result = 0.1 * (i / 2) * (2 - j);
    result += 0.1 * (i / 3) * j;
    return result;
}

void *ptrGenerator(int i, int j)
{
    char *result = (char *)0xDEADF00D;
    result += (i / 3) * j;
    return (void *)result;
}

template <class TYPE>
void testLexicographicalBuiltin(bool                    verboseFlag,
                                TYPE                  (*generator)(int, int),
                                bslma::TestAllocator&   testAllocator)
    // Compare every pair of strings of (template parameter) type 'TYPE' from
    // the specifications in the 'DATA_CASE4' array, and verify that they are
    // equal if and only if their specifications are equal.  Note that the
    // range will be passed using 'const TYPE *'.
{
    TYPE *strings[NUM_DATA_CASE4];
    const TYPE **STRINGS = const_cast<const TYPE **>(strings);

    const Int64 NUM_BYTES = testAllocator.numBytesInUse();

    for (int i = 0; i < NUM_DATA_CASE4; ++i) {
        const char   *STRING = DATA_CASE4[i].d_string_p;
        const size_t  LEN    = std::strlen(STRING);
        strings[i] = (TYPE *) testAllocator.allocate(LEN * sizeof(TYPE));
        for (int j = 0; j < static_cast<int>(LEN); ++j) {
            strings[i][j] = generator(i, j);
        }
    }

    for (int i = 0; i < NUM_DATA_CASE4; ++i) {
        const int     LINE1   = DATA_CASE4[i].d_lineNum;
        const char   *STRING1 = DATA_CASE4[i].d_string_p;
        const size_t  LEN1    = std::strlen(STRING1);

        for (int j = 0; j < NUM_DATA_CASE4; ++j) {
            const int     LINE2   = DATA_CASE4[j].d_lineNum;
            const char   *STRING2 = DATA_CASE4[j].d_string_p;
            const size_t  LEN2    = std::strlen(STRING2);

            const TYPE   *LHS_BEGIN = STRINGS[i];
            const TYPE   *LHS_END   = STRINGS[i] + LEN1;
            const size_t  LHS_LEN   = LEN1;

            const TYPE   *RHS_BEGIN = STRINGS[j];
            const TYPE   *RHS_END   = STRINGS[j] + LEN2;
            const size_t  RHS_LEN   = LEN2;

            int    exp = 0;
            size_t k   = 0;
            for (k = 0; !exp && k < LHS_LEN && k < RHS_LEN; ++k) {
                if (LHS_BEGIN[k] < RHS_BEGIN[k]) {
                    exp = -1;
                } else if (LHS_BEGIN[k] > RHS_BEGIN[k]) {
                    exp = 1;
                }
            }
            if (!exp) {
               if (k == LHS_LEN) {
                    if (k < RHS_LEN) {
                        exp = -1;  // LHS shorter than RHS
                    }
                } else {
                    exp = 1;       // RHS shorter than LHS
                }
            }
            const int& EXP = exp;

            if (verboseFlag) {
                int result = Obj::lexicographical(LHS_BEGIN, LHS_END, LHS_LEN,
                                                  RHS_BEGIN, RHS_END, RHS_LEN);
                printf("LINE1 = %d, LEN1 = " ZU ", "
                       "LINE2 = %d, LEN2 = " ZU "\n",
                       LINE1, LEN1, LINE2, LEN2);
                printf("LHS = [ ");
                for (k = 0; k < LHS_LEN; ++k) {
                    printf("%s", k ? ", " : "");
                    dbg_print(LHS_BEGIN[k]);
                }
                printf(" ]\nRHS = [ ");
                for (k = 0; k < RHS_LEN; ++k) {
                    printf("%s", k ? ", " : "");
                    dbg_print(RHS_BEGIN[k]);
                }
                printf(" ]\nEXP = %d, result = %d\n", EXP, result);
            }

            LOOP3_ASSERT(i, j, EXP,
                         (EXP == Obj::lexicographical(LHS_BEGIN, LHS_END,
                                                      RHS_BEGIN, RHS_END)));
            LOOP3_ASSERT(i, j, EXP,
                         (EXP == Obj::lexicographical(
                                                LHS_BEGIN, LHS_END, LHS_LEN,
                                                RHS_BEGIN, RHS_END, RHS_LEN)));
        }
    }

    for (int i = 0; i < NUM_DATA_CASE4; ++i) {
        testAllocator.deallocate(strings[i]);
    }

    ASSERT(NUM_BYTES == testAllocator.numBytesInUse());
}

template <class TYPE>
void testLexicographicalNonBitwise(bool                  verboseFlag,
                                   bslma::TestAllocator& testAllocator)
    // Compare every pair of ranges of (template parameter) type 'TYPE' from
    // the specifications in the 'DATA_CASE4' array, and verify that they are
    // equal if and only if their specifications are equal.  Note that the
    // range will be passed using 'my_Iterator<TYPE>', which prevents any kind
    // of bit-wise optimization.
{
    TYPE *strings[NUM_DATA_CASE4];
    const TYPE **STRINGS = const_cast<const TYPE **>(strings);

    const Int64 NUM_BYTES = testAllocator.numBytesInUse();

    for (int i = 0; i < NUM_DATA_CASE4; ++i) {
        const char   *STRING = DATA_CASE4[i].d_string_p;
        const size_t  LEN    = std::strlen(STRING);
        strings[i] = (TYPE *) testAllocator.allocate(LEN * sizeof(TYPE));
        gg(strings[i], STRING);
    }

    for (int i = 0; i < NUM_DATA_CASE4; ++i) {
        const int     LINE1   = DATA_CASE4[i].d_lineNum;
        const char   *STRING1 = DATA_CASE4[i].d_string_p;
        const size_t  LEN1    = std::strlen(STRING1);

        for (int j = 0; j < NUM_DATA_CASE4; ++j) {
            const int     LINE2   = DATA_CASE4[j].d_lineNum;
            const char   *STRING2 = DATA_CASE4[j].d_string_p;
            const size_t  LEN2    = std::strlen(STRING2);

            const TYPE   *LHS_BEGIN = STRINGS[i];
            const TYPE   *LHS_END   = STRINGS[i] + LEN1;
            const size_t  LHS_LEN   = LEN1;

            const TYPE   *RHS_BEGIN = STRINGS[j];
            const TYPE   *RHS_END   = STRINGS[j] + LEN2;
            const size_t  RHS_LEN   = LEN2;

            const int EXP = (i == j) ? 0 : ((i < j) ? -1 : 1);

            if (verboseFlag) {
                int result = Obj::lexicographical(LHS_BEGIN, LHS_END, LHS_LEN,
                                                  RHS_BEGIN, RHS_END, RHS_LEN);
                printf("LINE1 = %d, STRING1 = \"%s\", LEN1 = " ZU ", "
                       "LINE2 = %d, STRING2 = \"%s\", LEN2 = " ZU "\n",
                       LINE1, STRING1, LEN1, LINE2, STRING2, LEN2);
                printf("EXP = %d, result = %d\n", EXP, result);
            }

            typedef my_Iterator<TYPE> Iter;
            ASSERT((EXP == Obj::lexicographical(
                                             Iter(LHS_BEGIN), Iter(LHS_END),
                                             Iter(RHS_BEGIN), Iter(RHS_END))));
            ASSERT((EXP == Obj::lexicographical(
                                    Iter(LHS_BEGIN), Iter(LHS_END), LHS_LEN,
                                    Iter(RHS_BEGIN), Iter(RHS_END), RHS_LEN)));
        }
    }

    for (int i = 0; i < NUM_DATA_CASE4; ++i) {
        testAllocator.deallocate(strings[i]);
    }

    ASSERT(NUM_BYTES == testAllocator.numBytesInUse());
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 3
//-----------------------------------------------------------------------------

static const struct {
    int          d_lineNum;
    const char  *d_string_p;
} DATA_CASE3[] = {
    // line   string
    // ----   ------
    {  L_,    ""              },
    {  L_,    "a"             },
    {  L_,    "b"             },
    {  L_,    "ab"            },
    {  L_,    "ba"            },
    {  L_,    "abc"           },
    {  L_,    "bac"           },
    {  L_,    "cab"           },
    {  L_,    "abcd"          },
    {  L_,    "abca"          },
    {  L_,    "dcba"          },
    {  L_,    "abcde"         },
    {  L_,    "edcba"         }
};
const int NUM_DATA_CASE3 = sizeof DATA_CASE3 / sizeof *DATA_CASE3;

void testGenericEqual(bool verboseFlag)
    // Compare every pair of strings in the 'DATA_CASE3' array, and verify that
    // they are equal according to the generic 'equal' implementation (using
    // four arguments) if and only if they are equal.
{
    for (int i = 0; i < NUM_DATA_CASE3; ++i) {
        const int     LINE1   = DATA_CASE3[i].d_lineNum;
        const char   *STRING1 = DATA_CASE3[i].d_string_p;
        const size_t  LEN1    = std::strlen(STRING1);

        for (int j = 0; j < NUM_DATA_CASE3; ++j) {
            const int     LINE2   = DATA_CASE3[j].d_lineNum;
            const char   *STRING2 = DATA_CASE3[j].d_string_p;
            const size_t  LEN2    = std::strlen(STRING2);

            const bool EXP = (i == j);

            if (verboseFlag) {
                bool result = Obj::equal(STRING1, STRING1 + LEN1,
                                         STRING2, STRING2 + LEN2);
                printf("LINE1 = %d, STRING1 = \"%s\", LEN1 = " ZU ", "
                       "LINE2 = %d, STRING2 = \"%s\", LEN2 = " ZU "\n",
                       LINE1, STRING1, LEN1, LINE2, STRING2, LEN2);
                printf("EXP = %d, result = %d\n", EXP, result);
            }
            ASSERT(EXP == Obj::equal(STRING1, STRING1 + LEN1,
                                     STRING2, STRING2 + LEN2));

            typedef my_Iterator<char> Iter;
            ASSERT(EXP == Obj::equal(Iter(STRING1), Iter(STRING1 + LEN1),
                                     Iter(STRING2), Iter(STRING2 + LEN2)));
        }
    }
}

template <class TYPE>
void testEqual(bool verboseFlag, bslma::TestAllocator& testAllocator)
    // Compare every pair of strings of (template parameter) type 'TYPE' from
    // the specifications in the 'DATA_CASE3' array, and verify that they are
    // equal if and only if their specifications are equal.  Note that the
    // range will be passed using 'const TYPE *'.
{
    TYPE *strings[NUM_DATA_CASE3];
    const TYPE **STRINGS = const_cast<const TYPE **>(strings);

    const Int64 NUM_BYTES = testAllocator.numBytesInUse();

    for (int i = 0; i < NUM_DATA_CASE3; ++i) {
        const char   *STRING = DATA_CASE3[i].d_string_p;
        const size_t  LEN    = std::strlen(STRING);
        strings[i] = (TYPE *) testAllocator.allocate(LEN * sizeof(TYPE));
        gg(strings[i], STRING);
    }

    for (int i = 0; i < NUM_DATA_CASE3; ++i) {
        const int     LINE1   = DATA_CASE3[i].d_lineNum;
        const char   *STRING1 = DATA_CASE3[i].d_string_p;
        const size_t  LEN1    = std::strlen(STRING1);

        for (int j = 0; j < NUM_DATA_CASE3; ++j) {
            const int     LINE2   = DATA_CASE3[j].d_lineNum;
            const char   *STRING2 = DATA_CASE3[j].d_string_p;
            const size_t  LEN2    = std::strlen(STRING2);

            const TYPE   *LHS_BEGIN = STRINGS[i];
            const TYPE   *LHS_END   = STRINGS[i] + LEN1;
            const size_t  LHS_LEN   = LEN1;

            const TYPE   *RHS_BEGIN = STRINGS[j];
            const TYPE   *RHS_END   = STRINGS[j] + LEN2;
            const size_t  RHS_LEN   = LEN2;

            const bool EXP = (i == j);

            if (verboseFlag) {
                bool result = Obj::equal(LHS_BEGIN, LHS_END, LHS_LEN,
                                         RHS_BEGIN, RHS_END, RHS_LEN);
                printf("LINE1 = %d, STRING1 = \"%s\", LEN1 = " ZU ", "
                       "LINE2 = %d, STRING2 = \"%s\", LEN2 = " ZU "\n",
                       LINE1, STRING1, LEN1, LINE2, STRING2, LEN2);
                printf("EXP = %d, result = %d\n", EXP, result);
            }

            if (LHS_LEN == RHS_LEN) {
                ASSERT((EXP == Obj::equal(LHS_BEGIN, LHS_END, RHS_BEGIN)));
            }
            ASSERT((EXP == Obj::equal(LHS_BEGIN, LHS_END, LHS_LEN,
                                      RHS_BEGIN, RHS_END, RHS_LEN)));
        }
    }

    for (int i = 0; i < NUM_DATA_CASE3; ++i) {
        testAllocator.deallocate(strings[i]);
    }

    ASSERT(NUM_BYTES == testAllocator.numBytesInUse());
}

template <class TYPE>
void testEqualNonBitwise(bool verboseFlag, bslma::TestAllocator& testAllocator)
    // Compare every pair of strings of (template parameter) type 'TYPE' from
    // the specifications in the 'DATA_CASE3' array, and verify that they are
    // equal if and only if their specifications are equal.  Note that the
    // range will be passed using 'my_Iterator<TYPE>', which prevents any kind
    // of bit-wise optimization.
{
    TYPE *strings[NUM_DATA_CASE3];
    const TYPE **STRINGS = const_cast<const TYPE **>(strings);

    const Int64 NUM_BYTES = testAllocator.numBytesInUse();

    for (int i = 0; i < NUM_DATA_CASE3; ++i) {
        const char   *STRING = DATA_CASE3[i].d_string_p;
        const size_t  LEN    = std::strlen(STRING);
        strings[i] = (TYPE *) testAllocator.allocate(LEN * sizeof(TYPE));
        gg(strings[i], STRING);
    }

    for (int i = 0; i < NUM_DATA_CASE3; ++i) {
        const int     LINE1   = DATA_CASE3[i].d_lineNum;
        const char   *STRING1 = DATA_CASE3[i].d_string_p;
        const size_t  LEN1    = std::strlen(STRING1);

        for (int j = 0; j < NUM_DATA_CASE3; ++j) {
            const int     LINE2   = DATA_CASE3[j].d_lineNum;
            const char   *STRING2 = DATA_CASE3[j].d_string_p;
            const size_t  LEN2    = std::strlen(STRING2);

            const TYPE   *LHS_BEGIN = STRINGS[i];
            const TYPE   *LHS_END   = STRINGS[i] + LEN1;
            const size_t  LHS_LEN   = LEN1;

            const TYPE   *RHS_BEGIN = STRINGS[j];
            const TYPE   *RHS_END   = STRINGS[j] + LEN2;
            const size_t  RHS_LEN   = LEN2;

            const bool EXP = (i == j);

            if (verboseFlag) {
                bool result = Obj::equal(LHS_BEGIN, LHS_END, LHS_LEN,
                                         RHS_BEGIN, RHS_END, RHS_LEN);
                printf("LINE1 = %d, STRING1 = \"%s\", LEN1 = " ZU ", "
                       "LINE2 = %d, STRING2 = \"%s\", LEN2 = " ZU "\n",
                       LINE1, STRING1, LEN1, LINE2, STRING2, LEN2);
                printf("EXP = %d, result = %d\n", EXP, result);
            }

            typedef my_Iterator<TYPE> Iter;

            if (LHS_LEN == RHS_LEN) {
                ASSERT((EXP == Obj::equal(Iter(LHS_BEGIN),
                                          Iter(LHS_END),
                                          Iter(RHS_BEGIN))));
            }
            ASSERT((EXP == Obj::equal(
                                    Iter(LHS_BEGIN), Iter(LHS_END), LHS_LEN,
                                    Iter(RHS_BEGIN), Iter(RHS_END), RHS_LEN)));
        }
    }

    for (int i = 0; i < NUM_DATA_CASE3; ++i) {
        testAllocator.deallocate(strings[i]);
    }

    ASSERT(NUM_BYTES == testAllocator.numBytesInUse());
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 2
//-----------------------------------------------------------------------------

template <class TEST_TYPE>
void testGGG(bool verbose, bool veryVerbose)
{
    if (verbose) printf("\nTesting generator on invalid specs.\n");
    {
        const int MAX_LENGTH = 10;
        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_index;    // offending character index
        } DATA[] = {
            //line  spec            index
            //----  -------------   -----
            { L_,   "",             GGG_SUCCESS, }, // control

            { L_,   "a",            GGG_SUCCESS, }, // control
            { L_,   " ",            0,           },
            { L_,   ".",            0,           },
            { L_,   "Z",            GGG_SUCCESS, }, // control

            { L_,   "AZ",           GGG_SUCCESS, }, // control
            { L_,   "+E",           0,           },
            { L_,   "A=",           1,           },
            { L_,   ".a",           0,           },
            { L_,   "a!",           1,           },
            { L_,   "  ",           0,           },

            { L_,   "ABC",          GGG_SUCCESS, }, // control
            { L_,   " BC",          0,           },
            { L_,   "A C",          1,           },
            { L_,   "AB ",          2,           },
            { L_,   "?#:",          0,           },
            { L_,   "   ",          0,           },

            { L_,   "ABCDE",        GGG_SUCCESS, }, // control
            { L_,   "!BCDE",        0,           },
            { L_,   "AB!DE",        2,           },
            { L_,   "ABCD!",        4,           },
            { L_,   "A!C+E",        1,           }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int          LINE   = DATA[ti].d_lineNum;
            const char *const SPEC    = DATA[ti].d_spec_p;
            const int         INDEX   = DATA[ti].d_index;
            const int         LENGTH  = (int)strlen(SPEC);

            TEST_TYPE array[MAX_LENGTH];

            if ((int)LENGTH != oldLen) {
                if (verbose) printf("\tof length %d:\n", LENGTH);
                LOOP_ASSERT(LINE, LENGTH <= MAX_LENGTH);
                    // Sufficient space is available.
                LOOP_ASSERT(LINE, oldLen <= (int)LENGTH);  // non-decreasing
                oldLen = LENGTH;
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int result = ggg(array, SPEC, veryVerbose);

            LOOP_ASSERT(LINE, INDEX == result);
        }
    }
}

template <class TEST_TYPE>
void testGG(bool verbose, bool veryVerbose)
{
    if (verbose) printf("\nTesting generator on valid specs.\n");
    {
        const int MAX_LENGTH = 10;
        static const struct {
            int         d_lineNum;               // source line number
            const char *d_spec_p;                // specification string
        } DATA[] = {
            //line    spec
            //----    --------------
            {  L_,    ""              },
            {  L_,    "a"             },
            {  L_,    "b"             },
            {  L_,    "ab"            },
            {  L_,    "ba"            },
            {  L_,    "abc"           },
            {  L_,    "bac"           },
            {  L_,    "cab"           },
            {  L_,    "abcd"          },
            {  L_,    "abca"          },
            {  L_,    "dcba"          },
            {  L_,    "abcde"         },
            {  L_,    "edcba"         }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const int         LENGTH = (int)strlen(SPEC);

            TEST_TYPE array[MAX_LENGTH];
            const TEST_TYPE& X = gg(array, SPEC);
            (void) X; // Supress variable unused warnings
                // first element

            if (LENGTH != oldLen) {
                if (verbose) printf("\tof length %d:\n", LENGTH);
                LOOP_ASSERT(LINE, oldLen <= LENGTH);  // non-decreasing
                LOOP_ASSERT(LINE, LENGTH < MAX_LENGTH);
                    // Extra space is available for uninitialized elements.
                oldLen = LENGTH;
            }

            if (veryVerbose) {
                printf("\t\tSpec = \"%s\"\n", SPEC);
            }

            for (int idx = 0; idx < LENGTH; ++idx) {
                LOOP2_ASSERT(LINE, idx,
                             array[idx].datum() == SPEC[idx]);
            }
            LOOP2_ASSERT(LINE, LENGTH,
                         array[LENGTH].datum() == '_' );
        }
    }
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE -1
//-----------------------------------------------------------------------------

struct TestPairType {
    int first, second;
};

namespace BloombergLP {
namespace bslmf {
template <> struct IsBitwiseEqualityComparable<TestPairType>
    : bsl::true_type {};
}  // close namespace bslmf
}  // close enterprise namespace

bool operator==(const TestPairType& lhs, const TestPairType& rhs)
{
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

bool operator!=(const TestPairType& lhs, const TestPairType& rhs)
{
    return !(lhs == rhs);
}

bool operator<(const TestPairType& lhs, const TestPairType& rhs)
{
    return (lhs.first < rhs.first) ||
           (lhs.first == rhs.first && lhs.second < rhs.second);
}

bool operator>(const TestPairType& lhs, const TestPairType& rhs)
{
    return rhs < lhs;
}

template <class TYPE>
void generateNonNullValue(TYPE *value, int j)
{
    *value = static_cast<TYPE>(j * j);
}

void generateNonNullValue(void **value, int j)
{
    *value = reinterpret_cast<void*>((j + 1) * j);
}

void generateNonNullValue(TestPairType *value, int j)
{
    value->first = (j + 1) * j; value->second = j *(j - 1);
}

template <class TYPE>
void timeEqualAlgorithm(const char *typeName,
                        int         rawBufferSize,
                        char       *rawBuffer1,
                        char       *rawBuffer2,
                        int         numIter)
{
    printf("\n\tcompare with '%s'\n", typeName);
    {
        const size_t bufferSize = rawBufferSize / sizeof(TYPE);
        TYPE *buffer1 = (TYPE*)rawBuffer1;
        TYPE *buffer2 = (TYPE*)rawBuffer2;

        for (size_t j = 0; j < bufferSize; ++j) {
            buffer1[j] = buffer2[j] = TYPE();
        }

        bsls::Stopwatch timer;
        timer.start();
        for (int i = 0; i < numIter; ++i) {
            size_t j = 0;
            for (; j < bufferSize; ++j) {
                if (buffer1[j] != buffer2[j]) {
                    break;
                }
            }
            ASSERT(bufferSize == j);
        }
        timer.stop();
        printf("equal<%s>(0) - single loop : %f\n", typeName,
               timer.elapsedTime());

        timer.reset();
        timer.start();
        for (int i = 0; i < numIter; ++i) {
            ASSERT((Obj::equal(buffer1, buffer1 + bufferSize, bufferSize,
                               buffer2, buffer2 + bufferSize, bufferSize)));
        }
        timer.stop();
        printf("equal<%s>(0) - bslalg      : %f\n", typeName,
               timer.elapsedTime());

        for (size_t j = 0; j < bufferSize; ++j) {
            generateNonNullValue(buffer1 + j, static_cast<int>(j));
            generateNonNullValue(buffer2 + j, static_cast<int>(j));
        }

        timer.reset();
        timer.start();
        for (int i = 0; i < numIter; ++i) {
            size_t j = 0;
            for (; j < bufferSize; ++j) {
                if (buffer1[j] != buffer2[j]) {
                    break;
                }
            }
            ASSERT(bufferSize == j);
        }
        timer.stop();
        printf("equal<%s>(!0) - single loop : %f\n", typeName,
               timer.elapsedTime());

        timer.reset();
        timer.start();
        for (int i = 0; i < numIter; ++i) {
            ASSERT((Obj::equal(buffer1, buffer1 + bufferSize, bufferSize,
                               buffer2, buffer2 + bufferSize, bufferSize)));
        }
        timer.stop();
        printf("equal<%s>(!0) - bslalg      : %f\n", typeName,
               timer.elapsedTime());
    }
}

template <class TYPE>
void timeLexicographicalAlgorithm(const char *typeName,
                                  int         rawBufferSize,
                                  char       *rawBuffer1,
                                  char       *rawBuffer2,
                                  int         numIter)
{
    printf("\n\tcompare with '%s'\n", typeName);
    {
        const size_t bufferSize = rawBufferSize / sizeof(TYPE);
        TYPE *buffer1 = (TYPE*)rawBuffer1;
        TYPE *buffer2 = (TYPE*)rawBuffer2;

        for (size_t j = 0; j < bufferSize; ++j) {
            buffer1[j] = buffer2[j] = TYPE();
        }

        bsls::Stopwatch timer;
        timer.start();
        for (int i = 0; i < numIter; ++i) {
            size_t j = 0;
            for (; j < bufferSize; ++j) {
                if (buffer1[j] < buffer2[j]) {
                    break;
                }
                if (buffer1[j] > buffer2[j]) {
                    break;
                }
            }
            ASSERT(bufferSize == j);
        }
        timer.stop();
        printf("lexicographical<%s>(0) - single loop : %f\n", typeName,
               timer.elapsedTime());

        timer.reset();
        timer.start();
        for (int i = 0; i < numIter; ++i) {
            ASSERT((0 == Obj::lexicographical(
                                  buffer1, buffer1 + bufferSize, bufferSize,
                                  buffer2, buffer2 + bufferSize, bufferSize)));
        }
        timer.stop();
        printf("lexicographical<%s>(0) - bslalg      : %f\n", typeName,
               timer.elapsedTime());

        for (size_t j = 0; j < bufferSize; ++j) {
            generateNonNullValue(buffer1 + j, static_cast<int>(j));
            generateNonNullValue(buffer2 + j, static_cast<int>(j));
        }

        timer.reset();
        timer.start();
        for (int i = 0; i < numIter; ++i) {
            size_t j = 0;
            for (; j < bufferSize; ++j) {
                if (buffer1[j] < buffer2[j]) {
                    break;
                }
                if (buffer1[j] > buffer2[j]) {
                    break;
                }
            }
            ASSERT(bufferSize == j);
        }
        timer.stop();
        printf("lexicographical<%s>(!0) - single loop : %f\n", typeName,
               timer.elapsedTime());

        timer.reset();
        timer.start();
        for (int i = 0; i < numIter; ++i) {
            ASSERT((0 == Obj::lexicographical(
                                  buffer1, buffer1 + bufferSize, bufferSize,
                                  buffer2, buffer2 + bufferSize, bufferSize)));
        }
        timer.stop();
        printf("lexicographical<%s>(!0) - bslalg      : %f\n", typeName,
               timer.elapsedTime());
    }
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

    (void)veryVeryVerbose;      // suppress warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&da);

    // Confirm no static initialization locked the default allocator
    ASSERT(&da == bslma::Default::defaultAllocator());

    bslma::TestAllocator testAllocator(veryVeryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        // Compare non-bit-wise comparable elements.
        usageTestMyString();

        // Compare bit-wise comparable elements.
        usageTestMyPoint();

        // Compare (bit-wise comparable) primitive types.
        usageTestInt();
     } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'lexicographical'
        //
        // Concerns:
        //   o That all implementations of 'lexicographical' are correct.
        //   o That the correct implementation is selected for the
        //     correct trait and given iterator type.
        //   o That no instantiation leads to ambiguity.
        //
        // Plan:
        //   Using the table method, and a generator that can generate ranges
        //   of objects of various types with values based on an initialization
        //   string, compare a pair of of ranges based on every combination of
        //   two initialization strings and compare the output of
        //   'bslalg::RangeCompare::lexicographical' with the expected output
        //   based on the initialization strings used to create the range.  Do
        //   this for the types: char, unsigned char, int, Unit64, double, void
        //   *, NonBitwiseWithOpEqual, BitWiseNoOpEqual and
        //   CharEquivalentNonBitwiseWithOpEqual.
        //
        // Testing:
        //  bool lexicographical(start1, end1, start2, end2);
        //  bool lexicographical(start1, end1, length1, start2, end2, length2);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'lexicographical'"
                            "\n=========================\n");

        if (veryVerbose) printf("\t... with 'char'\n");
        {
            if (veryVerbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographical<char>(veryVerbose, testAllocator);

            if (veryVerbose) printf("\t\tUsing forward iterator.\n");
            testLexicographicalNonBitwise<char>(veryVerbose, testAllocator);
        }

        if (veryVerbose) printf("\t... with 'signed char'\n");
        {
            if (veryVerbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographical<char>(veryVerbose, testAllocator);

            if (veryVerbose) printf("\t\tUsing forward iterator.\n");
            testLexicographicalNonBitwise<char>(veryVerbose, testAllocator);
        }

        if (veryVerbose) printf("\t... with 'unsigned char'\n");
        {
            if (veryVerbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographical<unsigned char>(veryVerbose, testAllocator);

            if (veryVerbose) printf("\t\tUsing forward iterator.\n");
            testLexicographicalNonBitwise<unsigned char>(veryVerbose,
                                                         testAllocator);
        }

        if (veryVerbose) printf("\t... with 'int'\n");
        {
            if (veryVerbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographicalBuiltin<int>(veryVerbose,
                                            &intGenerator,
                                            testAllocator);
        }

        if (veryVerbose) printf("\t... with 'long long'\n");
        {
            if (veryVerbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographicalBuiltin<Uint64>(veryVerbose,
                                               &uint64Generator,
                                               testAllocator);
        }

        if (veryVerbose) printf("\t... with 'double'\n");
        {
            if (veryVerbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographicalBuiltin<double>(veryVerbose,
                                               &doubleGenerator,
                                               testAllocator);
        }

        if (veryVerbose) printf("\t... with 'void *'\n");
        {
            if (veryVerbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographicalBuiltin<void *>(veryVerbose,
                                               &ptrGenerator,
                                               testAllocator);
        }

        if (veryVerbose)
            printf("\t... with non-BitwiseEqualityComparableType.\n");
        {
            if (veryVerbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographical<NonBitwiseWithOpEqual>(veryVerbose,
                                                       testAllocator);

            if (veryVerbose) printf("\t\tUsing forward iterator.\n");
            testLexicographicalNonBitwise<NonBitwiseWithOpEqual>(
                                                                veryVerbose,
                                                                testAllocator);
        }

        if (veryVerbose) printf("\t... with BitwiseEqualityComparableType.\n");
        {
            if (veryVerbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographical<BitWiseNoOpEqual>(veryVerbose, testAllocator);

            // We cannot use forward iterators since it would require
            // 'operator==' for 'BitWiseNoOpEqual'.
        }

        if (veryVerbose) printf("\t... with ConvertibleToBool.\n");
        {
            if (veryVerbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographicalBuiltin<CharEquivalentNonBitwiseWithOpEqual>(
                                                                veryVerbose,
                                                               &charGenerator,
                                                                testAllocator);
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'equal'
        //
        // Concerns:
        //   o That all implementations of 'equal' are correct.
        //   o That the correct implementation is selected for the
        //     correct trait and given iterator type.
        //
        // Plan:
        //   Given an array of string specs, all different, we test every pair
        //   of strings for equality, the expected result being that the
        //   strings differ if their specs differ.  We then apply this test for
        //   all four types, 'char', 'unsigned' char', bit-wise
        //   equality-comparable UDT, and non-bit-wise equality-comparable UDT.
        //   We make sure that the correct implementation is selected for the
        //   trait by not providing an 'operator==' when 'memcmp' should be
        //   used, and by stuffing some random bytes into the footprint of the
        //   class when memcmp should not be used.
        //
        // Testing:
        //   bool equal(start1, end1, start2);
        //   bool equal(start1, end1, start2, end2);
        //   bool equal(start1, end1, length1, start2, end2, length2);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'equal'"
                            "\n===============\n");

        if (veryVerbose) printf("\t... generic 'equal' (four arguments).\n");

        if (veryVerbose) printf("\t\tUsing pointer type for iterator.\n");
        testGenericEqual(veryVerbose);

        if (veryVerbose) printf("\t... with 'char'.\n");
        {
            if (veryVerbose) printf("\t\tUsing pointer type for iterator.\n");
            testEqual<char>(veryVerbose, testAllocator);

            if (veryVerbose) printf("\t\tUsing forward iterator.\n");
            testEqualNonBitwise<char>(veryVerbose, testAllocator);
        }

        if (veryVerbose) printf("\t... with 'unsigned char'.\n");
        {
            if (veryVerbose) printf("\t\tUsing pointer type for iterator.\n");
            testEqual<unsigned char>(veryVerbose, testAllocator);

            if (veryVerbose) printf("\t\tUsing forward iterator.\n");
            testEqualNonBitwise<unsigned char>(veryVerbose, testAllocator);
        }

        if (veryVerbose)
            printf("\t... with non-BitwiseEqualityComparableType.\n");
        {
            if (veryVerbose) printf("\t\tUsing pointer type for iterator.\n");
            testEqual<NonBitwiseWithOpEqual>(veryVerbose, testAllocator);

            if (veryVerbose)
                printf("\t\tMake sure std::memcmp is not used.\n");
            {
                NonBitwiseWithOpEqual buffer1[5];
                NonBitwiseWithOpEqual *string1 = buffer1;
                const NonBitwiseWithOpEqual *STRING1 = string1;
                gg(string1, "abcde");  // Use some seeds.

                NonBitwiseWithOpEqual buffer2[5];
                NonBitwiseWithOpEqual *string2 = buffer2;
                const NonBitwiseWithOpEqual *STRING2 = string2;
                gg(string2, "abcde");  // Use same value but different seeds.

                ASSERT(Obj::equal(STRING1, STRING1 + 5, 5,
                                  STRING2, STRING2 + 5, 5));
            }

            if (veryVerbose) printf("\t\tUsing forward iterator.\n");
            testEqualNonBitwise<NonBitwiseWithOpEqual>(veryVerbose,
                                                       testAllocator);
        }

        if (veryVerbose) printf("\t... with BitwiseEqualityComparableType.\n");
        {
            if (veryVerbose) printf("\t\tUsing pointer type for iterator.\n");
            testEqual<BitWiseNoOpEqual>(veryVerbose, testAllocator);

            // We cannot use forward iterators since it would require
            // 'operator==' for 'BitWiseNoOpEqual'.
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE GENERATOR FUNCTIONS gg AND ggg:
        //
        // Concerns:
        //: o That valid generator syntax produces expected results
        //: o That invalid syntax is detected and reported.
        //
        // Plan:
        //   For each of an enumerated sequence of 'spec' values, ordered by
        //   increasing 'spec' length, use the primitive generator function
        //   'gg' to set the state of a newly created object.  Verify that 'gg'
        //   returns a valid reference to the modified argument object and,
        //   using basic accessors, that the value of the object is as
        //   expected.
        //   Note that we are testing the parser only; the primary manipulators
        //   are already assumed to work.
        //
        //   For each of an enumerated sequence of 'spec' values, ordered by
        //   increasing 'spec' length, use the primitive generator function
        //   'ggg' to set the state of a newly created object.  Verify that
        //   'ggg' returns the expected value corresponding to the location of
        //   the first invalid value of the 'spec'.
        //
        // Testing:
        //   int ggg(TYPE *array, const char *spec, int verboseFlag = 1);
        //   TYPE& gg(TYPE *array, const char *spec);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING PRIMITIVE GENERATOR FUNCTIONS"
                            "\n=====================================");
        if (veryVerbose) printf("\n\t...with 'NonBitwiseWithOpEqual'...");
        testGGG<NonBitwiseWithOpEqual>(verbose, veryVerbose);
        testGG<NonBitwiseWithOpEqual>(verbose, veryVerbose);

        if (veryVerbose) printf("\n\t...with 'BitWiseNoOpEqual'...");
        testGGG<BitWiseNoOpEqual>(verbose, veryVerbose);
        testGG<BitWiseNoOpEqual>(verbose, veryVerbose);

        if (veryVerbose)
            printf("\n\t...with 'CharEquivalentNonBitwiseWithOpEqual'...");
        testGGG<CharEquivalentNonBitwiseWithOpEqual>(verbose, veryVerbose);
        testGG<CharEquivalentNonBitwiseWithOpEqual>(verbose, veryVerbose);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   That the basic functionality of 'equal' and 'lexicographical' are
        //   correct.
        //
        // Plan:
        //   Exercise basic usage of this component.
        //
        // Testing:
        //   This test exercises basic usage but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============");

        if (veryVerbose) printf("\n\tTesting 'equal'...");
        {
            const char   *X = "This is a string";
            const size_t  X_LEN = strlen(X);

            const char *Y = "This is a string also";
            const size_t Y_LEN = strlen(Y);

            const char *Z = "This is also a string";
            const size_t Z_LEN = strlen(Z);

            ASSERT(( Obj::equal(X, X + X_LEN, X_LEN,
                                X, X + X_LEN, X_LEN)));

            ASSERT((!Obj::equal(X, X + X_LEN, X_LEN,
                                Y, Y + Y_LEN, Y_LEN)));

            ASSERT((!Obj::equal(X, X + X_LEN, X_LEN,
                                Z, Z + Z_LEN, Z_LEN)));

            ASSERT((!Obj::equal(Y, Y + Y_LEN, Y_LEN,
                                X, X + X_LEN, X_LEN)));

            ASSERT(( Obj::equal(Y, Y + Y_LEN, Y_LEN,
                                Y, Y + Y_LEN, Y_LEN)));

            ASSERT((!Obj::equal(Y, Y + Y_LEN, Y_LEN,
                                Z, Z + Z_LEN, Z_LEN)));

            ASSERT((!Obj::equal(Z, Z + Z_LEN, Z_LEN,
                                X, X + X_LEN, X_LEN)));

            ASSERT((!Obj::equal(Z, Z + Z_LEN, Z_LEN,
                                Y, Y + Y_LEN, Y_LEN)));

            ASSERT(( Obj::equal(Z, Z + Z_LEN, Z_LEN,
                                Z, Z + Z_LEN, Z_LEN)));
        }

        if (veryVerbose) printf("\n\tTesting 'lexicographical'...");
        {
            const char   *X = "This is a string";
            const size_t  X_LEN = strlen(X);

            const char *Y = "This is a string also";
            const size_t Y_LEN = strlen(Y);

            const char *Z = "This is also a string";
            const size_t Z_LEN = strlen(Z);

            ASSERT((0  == Obj::lexicographical(X, X + X_LEN, X_LEN,
                                               X, X + X_LEN, X_LEN)));

            ASSERT((-1 == Obj::lexicographical(X, X + X_LEN, X_LEN,
                                               Y, Y + Y_LEN, Y_LEN)));

            ASSERT((-1 == Obj::lexicographical(X, X + X_LEN, X_LEN,
                                               Z, Z + Z_LEN, Z_LEN)));

            ASSERT((1  == Obj::lexicographical(Y, Y + Y_LEN, Y_LEN,
                                               X, X + X_LEN, X_LEN)));

            ASSERT((0  == Obj::lexicographical(Y, Y + Y_LEN, Y_LEN,
                                               Y, Y + Y_LEN, Y_LEN)));

            ASSERT((-1 == Obj::lexicographical(Y, Y + Y_LEN, Y_LEN,
                                               Z, Z + Z_LEN, Z_LEN)));

            ASSERT((1  == Obj::lexicographical(Z, Z + Z_LEN, Z_LEN,
                                               X, X + X_LEN, X_LEN)));

            ASSERT((1  == Obj::lexicographical(Z, Z + Z_LEN, Z_LEN,
                                               Y, Y + Y_LEN, Y_LEN)));

            ASSERT((0  == Obj::lexicographical(Z, Z + Z_LEN, Z_LEN,
                                               Z, Z + Z_LEN, Z_LEN)));
        }

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //
        // Concerns:
        //   The implementation of the component takes pains to use 'memcmp'
        //   whenever possible.  This is based on an assumption that 'memcmp'
        //   will be faster than comparing each of the elements in a range in a
        //   single loop.  This assumption should be tested for builtin and for
        //   user types to confirm that the use of 'memcmp' is worthwhile.
        //
        // Plan:
        //   Time both implementations (single loop and 'memcmp') for various
        //   fundamental types, taking care to separate the case where 'memcmp'
        //   can be used.  Display the resulting times.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) printf("\nPERFORMANCE TEST"
                           "\n===============\n");

        enum {
            BUFFER_SIZE = 1 << 24,
            NUM_ITER    = 4
        };

        bslma::TestAllocator  testAllocator(veryVeryVeryVerbose);
        const int rawBufferSize = (argc > 2) ? atoi(argv[2]) : BUFFER_SIZE;
        const int numIter = (argc > 3) ? atoi(argv[3]) : NUM_ITER;
        char *rawBuffer1 = (char *)testAllocator.allocate(rawBufferSize);
        char *rawBuffer2 = (char *)testAllocator.allocate(rawBufferSize);
                                                               // max alignment

        printf("\nUsage: %s [bufferSize] [numIter]"
               "\nParameters:"
               "\n\t\tbufferSize\tin bytes (default: 16777216)"
               "\n\t\tnumIter\t\tto be repeated (default: 4 times)\n",
               __FILE__);

        timeEqualAlgorithm<char>("char",
                                 rawBufferSize,
                                 rawBuffer1,
                                 rawBuffer2,
                                 numIter);

        timeEqualAlgorithm<unsigned char>("unsigned char",
                                          rawBufferSize,
                                          rawBuffer1,
                                          rawBuffer2,
                                          numIter);

        timeEqualAlgorithm<wchar_t>("wchar_t",
                                    rawBufferSize,
                                    rawBuffer1,
                                    rawBuffer2,
                                    numIter);

        timeEqualAlgorithm<int>("int",
                                rawBufferSize,
                                rawBuffer1,
                                rawBuffer2,
                                numIter);

        timeEqualAlgorithm<double>("double",
                                   rawBufferSize,
                                   rawBuffer1,
                                   rawBuffer2,
                                   numIter);

        timeEqualAlgorithm<void *>("void *",
                                   rawBufferSize,
                                   rawBuffer1,
                                   rawBuffer2,
                                   numIter);

        timeEqualAlgorithm<TestPairType>("TestPairType",
                                         rawBufferSize,
                                         rawBuffer1,
                                         rawBuffer2,
                                         numIter);

        timeLexicographicalAlgorithm<char>("char",
                                           rawBufferSize,
                                           rawBuffer1,
                                           rawBuffer2,
                                           numIter);

        timeLexicographicalAlgorithm<unsigned char>("unsigned char",
                                                    rawBufferSize,
                                                    rawBuffer1,
                                                    rawBuffer2,
                                                    numIter);

        timeLexicographicalAlgorithm<wchar_t>("wchar_t",
                                              rawBufferSize,
                                              rawBuffer1,
                                              rawBuffer2,
                                              numIter);

        timeLexicographicalAlgorithm<int>("int",
                                          rawBufferSize,
                                          rawBuffer1,
                                          rawBuffer2,
                                          numIter);

        timeLexicographicalAlgorithm<double>("double",
                                             rawBufferSize,
                                             rawBuffer1,
                                             rawBuffer2,
                                             numIter);

        timeLexicographicalAlgorithm<void *>("void *",
                                             rawBufferSize,
                                             rawBuffer1,
                                             rawBuffer2,
                                             numIter);

        timeLexicographicalAlgorithm<TestPairType>("TestPairType",
                                                   rawBufferSize,
                                                   rawBuffer1,
                                                   rawBuffer2,
                                                   numIter);

        testAllocator.deallocate(rawBuffer1);
        testAllocator.deallocate(rawBuffer2);

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
