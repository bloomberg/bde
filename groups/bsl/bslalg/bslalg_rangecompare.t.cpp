// bslalg_rangecompare.t.cpp                                          -*-C++-*-

#include <bslalg_rangecompare.h>

#include <bslalg_typetraits.h>                          // for testing only
#include <bslalg_typetraitbitwiseequalitycomparable.h>  // for testing only

#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bsls_types.h>
#include <bsls_stopwatch.h>

#include <new> // placement 'new'
#include <cstdio>
#include <cstdlib>
#include <cctype>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides equality and less-than comparisons on ranges given
// by start and finish iterators and length, all of parameterizable types.
// The length is necessary because we do not, at the 'bslalg' level, have an
// algorithm to compute the length of the range, and doing without the length
// is an inferior implementation (it requires two tests per loop iteration,
// instead of one).  Also, the implementation distinguishes the cases where the
// iterator is convertible to a 'const VALUE_TYPE *' or not.
//
// The implementation uses traits to efficiently forward to 'std::memcmp' or
// 'std::wmemcmp'.  All fundamental and pointer types haves the
// bitwise-equality comparable trait.  The concerns range from correctness of
// implementation to correct selection of traits.  There is no assignment and
// therefore no exception-related concerns in this component.  We address this
// with two custom test types, one that has the bitwise-equality comparable
// trait and does not define 'operator==' (to ensure that it will not be
// compiled), and another that has an 'operator==' but no trait.  Finally, in
// order to trigger instantiation of all possible overloads, we wrap the
// range pointers into an iterator type that is *not* convertible to the
// pointer.
//-----------------------------------------------------------------------------
// [ 2] bool equal(begin1, end1, length1, begin2, end2, length2);
// [ 3] bool lexicographical(begin1, end1, length1, begin2, end2, length2);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [-1] PERFORMANCE TEST
// [ 4] USAGE EXAMPLE

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

namespace {
void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
// NOTE: This implementation of LOOP_ASSERT macros must use printf since
//       cout uses new and must not be called during exception testing.

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                    \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");     // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n")  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", ") // P(X) without '\n'
#define L_ __LINE__                        // current Line number
#define T_ putchar('\t');                  // Print a tab (w/o newline)

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
inline void dbg_print(char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(unsigned char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(signed char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(short val) { printf("%d", (int)val); fflush(stdout); }
inline void dbg_print(unsigned short val) {
    printf("%d", (int)val); fflush(stdout);
}
inline void dbg_print(int val) { printf("%d", val); fflush(stdout); }
inline void dbg_print(unsigned val) { printf("%u", val); fflush(stdout); }

#ifdef _MSC_VER
inline void dbg_print(bsls::Types::Int64 val) {
    printf("%I64d", val); fflush(stdout);
}
inline void dbg_print(bsls::Types::Uint64 val) {
    printf("%I64u", val); fflush(stdout);
}
#else
inline void dbg_print(bsls::Types::Int64 val) {
    printf("%lld", val); fflush(stdout);
}
inline void dbg_print(bsls::Types::Uint64 val) {
    printf("%llu", val); fflush(stdout);
}
#endif
inline void dbg_print(float val) {
    printf("'%f'", (double)val); fflush(stdout);
}
inline void dbg_print(double val) {
    printf("'%f'", val); fflush(stdout);
}
inline void dbg_print(const char* s) {
    printf("\"%s\"", s); fflush(stdout);
}
inline void dbg_print(const void * p) {
    printf("\"%p\"", p); fflush(stdout);
}

//=============================================================================
//                  GLOBAL TYPES/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsls::Types::Uint64    Uint64;

typedef bslalg::RangeCompare   Obj;

                        // =======================
                        // struct ScalarPrimitives
                        // =======================

struct ScalarPrimitives
{
  public:
    template <typename TARGET_TYPE>
    static void copyConstruct(TARGET_TYPE        *address,
                              const TARGET_TYPE&  original,
                              bslma::Allocator   *allocator);
        // Build an object of the parameterized 'TARGET_TYPE' from the
        // specified 'original' object of the same 'TARGET_TYPE' in the
        // uninitialized memory at the specified 'address', as if by using the
        // copy constructor of 'TARGET_TYPE'.  If the constructor throws, the
        // 'address' is left in an uninitialized state.

    template <typename TARGET_TYPE>
    static void destroy(TARGET_TYPE *object);
        // Destroy the specified 'object' of the parameterized 'TARGET_TYPE',
        // as if by calling the 'TARGET_TYPE' destructor, but do not deallocate
        // the memory occupied by 'object'.  Note that the destructor may
        // deallocate other memory owned by 'object'.
};

template <typename TARGET_TYPE>
void ScalarPrimitives::copyConstruct(TARGET_TYPE               *address,
                                            const TARGET_TYPE&  original,
                                            bslma::Allocator   *allocator)
{
    new (address) TARGET_TYPE(original, allocator);
}

template <>
void ScalarPrimitives::copyConstruct<int>(int              *address,
                                          const int&        original,
                                          bslma::Allocator *allocator)
{
    (void) allocator;
    *address = original;
}

template <typename TARGET_TYPE>
void ScalarPrimitives::destroy(TARGET_TYPE *object)
{
    object->~TARGET_TYPE();
}

template <>
void ScalarPrimitives::destroy<int>(int *object)
{
    (void) object;
}



                        // =================
                        // class MyContainer
                        // =================

template <class VALUE_TYPE>
class MyContainer {
    // This class implements a container, semantically similar to
    // std::vector, holding objects of the given 'VALUE_TYPE',
    // which must provide value semantics.

  private:
    // DATA
    std::size_t       d_size;
    std::size_t       d_capacity;
    VALUE_TYPE       *d_start_p;
    bslma::Allocator *d_allocator_p;

    // NOT IMPLEMENTED
    MyContainer(const MyContainer&);
    MyContainer& operator=(const MyContainer&);

  public:
    // PUBLIC TYPES
    typedef VALUE_TYPE const *ConstIterator;
        // This 'typedef' provides an alias for the type of iterator
        // providing non-modifiable access to the elements in the
        // container.

    // CREATORS
    explicit MyContainer(bslma::Allocator *basicAllocator = 0);
        // Initialize this object as an empty container with zero capacity.

    MyContainer(std::size_t capacity, bslma::Allocator *basicAllocator = 0);
        // Initialize this object as an empty container
        // with the given capacity

    ~MyContainer();

    // MANIPULATORS
    void reserve(std::size_t newCapacity);
        // Change the capacity of this vector to the specified 'newCapacity'.
        // Note that the capacity of a vector is the maximum number of elements
        // it can accommodate without reallocation.  The actual storage
        // allocated may be higher.

    void push_back(const VALUE_TYPE& value);
        // Add the specified value at the past-the-end position in this
        // container, increasing the container's capacity if needed.

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
        return;
    }

    // Build replacement container
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

                              // ==============
                              // class MyString
                              // ==============

class MyString {
  private:
    // DATA
    char *d_start_p;
    std::size_t d_length;
    bslma::Allocator *d_allocator_p;

    void set(const char* s, std::size_t length);

    // FRIENDS
    friend bool operator==(const MyString&, const MyString&);
    friend bool operator!=(const MyString&, const MyString&);

  public:
    explicit MyString(const char* s, bslma::Allocator *basicAllocator = 0);
    MyString(const MyString& original, bslma::Allocator *basicAllocator = 0);
    MyString& operator=(const MyString& rhs);
    ~MyString();

    const char* c_str() const;
    std::size_t length() const;
};

MyString::MyString(const char* s, bslma::Allocator *basicAllocator)
    : d_start_p(0)
    , d_length(0)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    set(s, strlen(s));
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

void MyString::set(const char* s, std::size_t length)
{
    d_length = length;
    d_start_p =
           (char *) d_allocator_p->allocate((length + 1) * sizeof *d_start_p);
    memcpy(d_start_p, s, length);
    d_start_p[length] = '\0';
}

const char* MyString::c_str() const
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
        && 0 == strncmp(lhs.d_start_p, rhs.d_start_p, lhs.d_length);
}

bool operator!=(const MyString& lhs, const MyString& rhs)
{
    return ! (lhs == rhs);
}

                              // =============
                              // class MyPoint
                              // =============

class MyPoint {
  private:
    // DATA
    int d_x;
    int d_y;

    // FRIENDS
    friend bool operator==(const MyPoint&, const MyPoint&);
    friend bool operator!=(const MyPoint&, const MyPoint&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(MyPoint,
                     BloombergLP::bslalg::TypeTraitBitwiseEqualityComparable);

    // CREATORS
    MyPoint(int x, int y, bslma::Allocator *basicAllocator = 0);
    MyPoint(const MyPoint& original, bslma::Allocator *basicAllocator = 0);

    // ...

    // MANIPULATORS
    MyPoint& operator=(const MyPoint& rhs);
};

MyPoint::MyPoint(int x, int y, bslma::Allocator *basicAllocator)
    : d_x(x)
    , d_y(y)
{
    (void) basicAllocator;
}

MyPoint::MyPoint(const MyPoint& original, bslma::Allocator *basicAllocator)
    : d_x(other.d_x)
    , d_y(other.d_y)
{
    (void) basicAllocator;
}

MyPoint& MyPoint::operator=(const MyPoint& rhs)
{
    d_x = other.d_x;
    d_y = other.d_y;
}

bool operator==(const MyPoint& lhs, const MyPoint& rhs)
{
    return lhs.d_x == rhs.d_x && lhs.d_y == rhs.d_y;
}

                              // ===============
                              // class my_Class1
                              // ===============

class my_Class1 {
    // Class that doesn't have the bitwise equality comparable traits, but has
    // an 'operator=='.  In order to catch bitwise comparison, we stuff padding
    // with an extra byte filled with a random value.

    // CLASS DATA
    static char d_seed;

    // DATA
    char d_char;
    char d_random;

  public:
    // CREATORS
    my_Class1(char value = '_') : d_char(value), d_random(++d_seed) {}

    // ACCESSORS
    char datum() const { return d_char; }
};

// CLASS DATA
char my_Class1::d_seed = 92;

// FREE OPERATORS
bool operator==(const my_Class1& lhs, const my_Class1& rhs)
{
    return lhs.datum() == rhs.datum();
}

bool operator!=(const my_Class1& lhs, const my_Class1& rhs)
{
    return lhs.datum() != rhs.datum();
}

bool operator< (const my_Class1& lhs, const my_Class1& rhs)
{
    return lhs.datum() < rhs.datum();
}

                              // ===============
                              // class my_Class2
                              // ===============

class my_Class2 {
    // Class that has the bitwise equality comparable traits, and no
    // 'operator=='.

    // DATA
    char d_char;

  public:
    // CREATORS
    my_Class2(char value = '_') : d_char(value) {}

    // ACCESSORS
    char datum() const { return d_char; }
};

// FREE OPERATORS
bool operator<(const my_Class2& lhs, const my_Class2& rhs)
{
    return lhs.datum() < rhs.datum();
}

// TRAITS
namespace BloombergLP {

template <>
struct bslalg_TypeTraits<my_Class2>
: bslalg::TypeTraitBitwiseEqualityComparable { };

}  // close enterprise namespace

                              // ===============
                              // class my_Class3
                              // ===============

class my_Class3 : public my_Class1 {
    // Class that doesn't have the bitwise equality comparable traits, but has
    // an 'operator==' *and* is convertible to char.

  public:
    // CREATORS
    my_Class3(char value = '_') : my_Class1(value) {}

    // ACCESSORS
    operator char() const { return datum(); }
};

// FREE OPERATORS
bool operator==(const my_Class3& lhs, const my_Class3& rhs)
{
    return lhs.datum() == rhs.datum();
}

bool operator!=(const my_Class3& lhs, const my_Class3& rhs)
{
    return lhs.datum() != rhs.datum();
}

bool operator< (const my_Class3& lhs, const my_Class3& rhs)
{
    return lhs.datum() < rhs.datum();
}

                             // =================
                             // class my_Iterator
                             // =================

template <class TYPE>
class my_Iterator {
    // This class provides a bare-bones wrapper around a pointer to the
    // parameterized 'TYPE' that is enough to apply the 'equal' and
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
        // Increment this iterator.

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

template <class TYPE>
int ggg(TYPE *array, const char *spec, int verboseFlag = 1)
    // Configure the specified 'array' of objects of the parameterized 'TYPE'
    // (assumed to be uninitialized) according to the specified 'spec'.
    // Optionally specify a zero 'verboseFlag' to suppress 'spec' syntax error
    // messages.  Return the index of the first invalid character, and a
    // negative value otherwise.  Note that this function is used to implement
    // 'gg' as well as allow for verification of syntax error detection.
{
    enum { SUCCESS = -1 };
    for (int i = 0; spec[i]; ++i, ++array) {
        if (!isalpha(spec[i])) {
            if (verboseFlag) {
                printf("Error, bad character ('%c') in spec \"%s\""
                       " at position %d.\n", spec[i], spec, i);
            }
            return i;  // Discontinue processing this spec.
        }
        *array = spec[i];
    }
    return SUCCESS;
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
//                  GLOBAL HELPER FUNCTIONS FOR USAGE EXAMPLE
//-----------------------------------------------------------------------------

template<class VALUE_TYPE>
inline
bool operator==(const MyContainer<VALUE_TYPE>& lhs,
                const MyContainer<VALUE_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' instances have the same
    // value, and 'false' otherwise.  Two instances have the same value if the
    // have the same length and each element in lhs has the same value as the
    // corresponding element in rhs.

template<class VALUE_TYPE>
inline
bool operator!=(const MyContainer<VALUE_TYPE>& lhs,
                const MyContainer<VALUE_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' instances do not have the
    // same value, and 'false' otherwise.  Two instances differ in value if
    // they have differing lengths or if any element in lhs has differs in
    // value from the corresponding element in rhs.

template<class VALUE_TYPE>
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

template<class VALUE_TYPE>
inline
bool operator!=(const MyContainer<VALUE_TYPE>& lhs,
                const MyContainer<VALUE_TYPE>& rhs)
{
    return ! BloombergLP::bslalg::RangeCompare::equal(lhs.begin(),
                                                    lhs.end(),
                                                    lhs.size(),
                                                    rhs.begin(),
                                                    rhs.end(),
                                                    rhs.size());
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 3
//-----------------------------------------------------------------------------

static const struct {
    int          d_lineNum;
    const char  *d_string_p;
} DATA_3[] = {
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
const int NUM_DATA_3 = sizeof DATA_3 / sizeof *DATA_3;

template <class TYPE>
void testLexicographical(bool verboseFlag, bslma::TestAllocator& testAllocator)
    // Compare every pair of strings of the parameterized 'TYPE' from the
    // specifications in the 'DATA_2' array, and verify that they are equal if
    // and only if their specifications are equal.  Note that the range will be
    // passed using 'const TYPE *'.
{
    TYPE *strings[NUM_DATA_3];
    const TYPE **STRINGS = const_cast<const TYPE **>(strings);

    const int NUM_BYTES = testAllocator.numBytesInUse();

    for (int i = 0; i < NUM_DATA_3; ++i) {
        const char *STRING = DATA_3[i].d_string_p;
        const int   LEN    = std::strlen(STRING);
        strings[i] = (TYPE *) testAllocator.allocate(LEN * sizeof(TYPE));
        gg(strings[i], STRING);
    }

    for (int i = 0; i < NUM_DATA_3; ++i) {
        const int   LINE1   = DATA_3[i].d_lineNum;
        const char *STRING1 = DATA_3[i].d_string_p;
        const int   LEN1    = std::strlen(STRING1);

        for (int j = 0; j < NUM_DATA_3; ++j) {
            const int   LINE2   = DATA_3[j].d_lineNum;
            const char *STRING2 = DATA_3[j].d_string_p;
            const int   LEN2    = std::strlen(STRING2);

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
                printf("LINE1 = %d, STRING1 = \"%s\", LEN1 = %d, "
                       "LINE2 = %d, STRING2 = \"%s\", LEN2 = %d\n",
                       LINE1, STRING1, LEN1, LINE2, STRING2, LEN2);
                printf("EXP = %d, result = %d\n", EXP, result);
            }

            ASSERT((EXP == Obj::lexicographical(LHS_BEGIN, LHS_END,
                                                RHS_BEGIN, RHS_END)));
            ASSERT((EXP == Obj::lexicographical(LHS_BEGIN, LHS_END, LHS_LEN,
                                                RHS_BEGIN, RHS_END, RHS_LEN)));
        }
    }

    for (int i = 0; i < NUM_DATA_3; ++i) {
        testAllocator.deallocate(strings[i]);
    }

    ASSERT(NUM_BYTES == testAllocator.numBytesInUse());
}

my_Class3 charGenerator(int i, int j)
{
    return my_Class3(DATA_3[i].d_string_p[j]);
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
    // Compare every pair of strings of the parameterized 'TYPE' from the
    // specifications in the 'DATA_2' array, and verify that they are equal if
    // and only if their specifications are equal.  Note that the range will be
    // passed using 'const TYPE *'.
{
    TYPE *strings[NUM_DATA_3];
    const TYPE **STRINGS = const_cast<const TYPE **>(strings);

    const int NUM_BYTES = testAllocator.numBytesInUse();

    for (int i = 0; i < NUM_DATA_3; ++i) {
        const char *STRING = DATA_3[i].d_string_p;
        const int   LEN    = std::strlen(STRING);
        strings[i] = (TYPE *) testAllocator.allocate(LEN * sizeof(TYPE));
        for (int j = 0; j < LEN; ++j) {
            strings[i][j] = generator(i, j);
        }
    }

    for (int i = 0; i < NUM_DATA_3; ++i) {
        const int   LINE1   = DATA_3[i].d_lineNum;
        const char *STRING1 = DATA_3[i].d_string_p;
        const int   LEN1    = std::strlen(STRING1);

        for (int j = 0; j < NUM_DATA_3; ++j) {
            const int   LINE2   = DATA_3[j].d_lineNum;
            const char *STRING2 = DATA_3[j].d_string_p;
            const int   LEN2    = std::strlen(STRING2);

            const TYPE *LHS_BEGIN = STRINGS[i];
            const TYPE *LHS_END   = STRINGS[i] + LEN1;
            const int   LHS_LEN   = LEN1;

            const TYPE *RHS_BEGIN = STRINGS[j];
            const TYPE *RHS_END   = STRINGS[j] + LEN2;
            const int   RHS_LEN   = LEN2;

            int exp = 0, k = 0;
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
                printf("LINE1 = %d, LEN1 = %d, "
                       "LINE2 = %d, LEN2 = %d\n",
                       LINE1, LEN1, LINE2, LEN2);
                printf("LHS = [ ");
                for (k = 0; k < LHS_LEN; ++k) {
                    printf("%s", k ? (char*)", " : (char*)"");
                    dbg_print(LHS_BEGIN[k]);
                }
                printf(" ]\nRHS = [ ");
                for (k = 0; k < RHS_LEN; ++k) {
                    printf("%s", k ? (char*)", " : (char*)"");
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

    for (int i = 0; i < NUM_DATA_3; ++i) {
        testAllocator.deallocate(strings[i]);
    }

    ASSERT(NUM_BYTES == testAllocator.numBytesInUse());
}

template <class TYPE>
void testLexicographicalNonBitwise(bool                  verboseFlag,
                                   bslma::TestAllocator& testAllocator)
    // Compare every pair of ranges of the parameterized 'TYPE' from the
    // specifications in the 'DATA_2' array, and verify that they are equal if
    // and only if their specifications are equal.  Note that the range will be
    // passed using 'my_Iterator<TYPE>', which prevents any kind of bitwise
    // optimization.
{
    TYPE *strings[NUM_DATA_3];
    const TYPE **STRINGS = const_cast<const TYPE **>(strings);

    const int NUM_BYTES = testAllocator.numBytesInUse();

    for (int i = 0; i < NUM_DATA_3; ++i) {
        const char *STRING = DATA_3[i].d_string_p;
        const int   LEN    = std::strlen(STRING);
        strings[i] = (TYPE *) testAllocator.allocate(LEN * sizeof(TYPE));
        gg(strings[i], STRING);
    }

    for (int i = 0; i < NUM_DATA_3; ++i) {
        const int   LINE1   = DATA_3[i].d_lineNum;
        const char *STRING1 = DATA_3[i].d_string_p;
        const int   LEN1    = std::strlen(STRING1);

        for (int j = 0; j < NUM_DATA_3; ++j) {
            const int   LINE2   = DATA_3[j].d_lineNum;
            const char *STRING2 = DATA_3[j].d_string_p;
            const int   LEN2    = std::strlen(STRING2);

            const TYPE *LHS_BEGIN = STRINGS[i];
            const TYPE *LHS_END   = STRINGS[i] + LEN1;
            const int   LHS_LEN   = LEN1;

            const TYPE *RHS_BEGIN = STRINGS[j];
            const TYPE *RHS_END   = STRINGS[j] + LEN2;
            const int   RHS_LEN   = LEN2;

            const int EXP = (i == j) ? 0 : ((i < j) ? -1 : 1);

            if (verboseFlag) {
                int result = Obj::lexicographical(LHS_BEGIN, LHS_END, LHS_LEN,
                                                  RHS_BEGIN, RHS_END, RHS_LEN);
                printf("LINE1 = %d, STRING1 = \"%s\", LEN1 = %d, "
                       "LINE2 = %d, STRING2 = \"%s\", LEN2 = %d\n",
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

    for (int i = 0; i < NUM_DATA_3; ++i) {
        testAllocator.deallocate(strings[i]);
    }

    ASSERT(NUM_BYTES == testAllocator.numBytesInUse());
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 2
//-----------------------------------------------------------------------------

static const struct {
    int          d_lineNum;
    const char  *d_string_p;
} DATA_2[] = {
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
const int NUM_DATA_2 = sizeof DATA_2 / sizeof *DATA_2;

void testGenericEqual(bool verboseFlag, bslma::TestAllocator& testAllocator)
    // Compare every pair of strings in the 'DATA_2' array, and verify that
    // they are equal according to the generic 'equal' implementation (using
    // four arguments) if and only if they are equal.
{
    for (int i = 0; i < NUM_DATA_2; ++i) {
        const int   LINE1   = DATA_2[i].d_lineNum;
        const char *STRING1 = DATA_2[i].d_string_p;
        const int   LEN1    = std::strlen(STRING1);

        for (int j = 0; j < NUM_DATA_2; ++j) {
            const int   LINE2   = DATA_2[j].d_lineNum;
            const char *STRING2 = DATA_2[j].d_string_p;
            const int   LEN2    = std::strlen(STRING2);

            const bool EXP = (i == j);

            if (verboseFlag) {
                bool result = Obj::equal(STRING1, STRING1 + LEN1,
                                         STRING2, STRING2 + LEN2);
                printf("LINE1 = %d, STRING1 = \"%s\", LEN1 = %d, "
                       "LINE2 = %d, STRING2 = \"%s\", LEN2 = %d\n",
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
    // Compare every pair of strings of the parameterized 'TYPE' from the
    // specifications in the 'DATA_2' array, and verify that they are equal if
    // and only if their specifications are equal.  Note that the range will be
    // passed using 'const TYPE *'.
{
    TYPE *strings[NUM_DATA_2];
    const TYPE **STRINGS = const_cast<const TYPE **>(strings);

    const int NUM_BYTES = testAllocator.numBytesInUse();

    for (int i = 0; i < NUM_DATA_2; ++i) {
        const char *STRING = DATA_2[i].d_string_p;
        const int   LEN    = std::strlen(STRING);
        strings[i] = (TYPE *) testAllocator.allocate(LEN * sizeof(TYPE));
        gg(strings[i], STRING);
    }

    for (int i = 0; i < NUM_DATA_2; ++i) {
        const int   LINE1   = DATA_2[i].d_lineNum;
        const char *STRING1 = DATA_2[i].d_string_p;
        const int   LEN1    = std::strlen(STRING1);

        for (int j = 0; j < NUM_DATA_2; ++j) {
            const int   LINE2   = DATA_2[j].d_lineNum;
            const char *STRING2 = DATA_2[j].d_string_p;
            const int   LEN2    = std::strlen(STRING2);

            const TYPE *LHS_BEGIN = STRINGS[i];
            const TYPE *LHS_END   = STRINGS[i] + LEN1;
            const int   LHS_LEN   = LEN1;

            const TYPE *RHS_BEGIN = STRINGS[j];
            const TYPE *RHS_END   = STRINGS[j] + LEN2;
            const int   RHS_LEN   = LEN2;

            const bool EXP = (i == j);

            if (verboseFlag) {
                bool result = Obj::equal(LHS_BEGIN, LHS_END, LHS_LEN,
                                         RHS_BEGIN, RHS_END, RHS_LEN);
                printf("LINE1 = %d, STRING1 = \"%s\", LEN1 = %d, "
                       "LINE2 = %d, STRING2 = \"%s\", LEN2 = %d\n",
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

    for (int i = 0; i < NUM_DATA_2; ++i) {
        testAllocator.deallocate(strings[i]);
    }

    ASSERT(NUM_BYTES == testAllocator.numBytesInUse());
}

template <class TYPE>
void testEqualNonBitwise(bool verboseFlag, bslma::TestAllocator& testAllocator)
    // Compare every pair of strings of the parameterized 'TYPE' from the
    // specifications in the 'DATA_2' array, and verify that they are equal if
    // and only if their specifications are equal.  Note that the range will be
    // passed using 'my_Iterator<TYPE>', which prevents any kind of bitwise
    // optimization.
{
    TYPE *strings[NUM_DATA_2];
    const TYPE **STRINGS = const_cast<const TYPE **>(strings);

    const int NUM_BYTES = testAllocator.numBytesInUse();

    for (int i = 0; i < NUM_DATA_2; ++i) {
        const char *STRING = DATA_2[i].d_string_p;
        const int   LEN    = std::strlen(STRING);
        strings[i] = (TYPE *) testAllocator.allocate(LEN * sizeof(TYPE));
        gg(strings[i], STRING);
    }

    for (int i = 0; i < NUM_DATA_2; ++i) {
        const int   LINE1   = DATA_2[i].d_lineNum;
        const char *STRING1 = DATA_2[i].d_string_p;
        const int   LEN1    = std::strlen(STRING1);

        for (int j = 0; j < NUM_DATA_2; ++j) {
            const int   LINE2   = DATA_2[j].d_lineNum;
            const char *STRING2 = DATA_2[j].d_string_p;
            const int   LEN2    = std::strlen(STRING2);

            const TYPE *LHS_BEGIN = STRINGS[i];
            const TYPE *LHS_END   = STRINGS[i] + LEN1;
            const int   LHS_LEN   = LEN1;

            const TYPE *RHS_BEGIN = STRINGS[j];
            const TYPE *RHS_END   = STRINGS[j] + LEN2;
            const int   RHS_LEN   = LEN2;

            const bool EXP = (i == j);

            if (verboseFlag) {
                bool result = Obj::equal(LHS_BEGIN, LHS_END, LHS_LEN,
                                         RHS_BEGIN, RHS_END, RHS_LEN);
                printf("LINE1 = %d, STRING1 = \"%s\", LEN1 = %d, "
                       "LINE2 = %d, STRING2 = \"%s\", LEN2 = %d\n",
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

    for (int i = 0; i < NUM_DATA_2; ++i) {
        testAllocator.deallocate(strings[i]);
    }

    ASSERT(NUM_BYTES == testAllocator.numBytesInUse());
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE -1
//-----------------------------------------------------------------------------

struct TestPairType {
    BSLALG_DECLARE_NESTED_TRAITS(TestPairType,
                                 bslalg::TypeTraitBitwiseEqualityComparable);
    int first, second;
};

bool operator==(const TestPairType& lhs, const TestPairType& rhs) {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

bool operator!=(const TestPairType& lhs, const TestPairType& rhs) {
    return !(lhs == rhs);
}

bool operator<(const TestPairType& lhs, const TestPairType& rhs) {
    return lhs.first < rhs.first ||
           lhs.first == rhs.first && lhs.second < rhs.second;
}

bool operator>(const TestPairType& lhs, const TestPairType& rhs) {
    return rhs < lhs;
}

template <class TYPE>
void generateNonNullValue(TYPE *value, int j) {
    *value = j * j;
}

void generateNonNullValue(void **value, int j) {
    *value = reinterpret_cast<void*>((j + 1) * j);
}

void generateNonNullValue(TestPairType *value, int j) {
    value->first = (j + 1) * j; value->second = j *(j - 1);
}

template <class TYPE>
void timeEqualAlgorithm(const char *typeName,
                        int         rawBufferSize,
                        const char *rawBuffer1,
                        const char *rawBuffer2,
                        int         numIter)
{
    printf("\n\tcompare with '%s'\n", typeName);
    {
        const int bufferSize = rawBufferSize / sizeof(TYPE);
        TYPE *buffer1 = (TYPE*)rawBuffer1;
        TYPE *buffer2 = (TYPE*)rawBuffer2;

        for (int j = 0; j < bufferSize; ++j) {
            buffer1[j] = buffer2[j] = TYPE();
        }

        bsls::Stopwatch timer;
        timer.start();
        for (int i = 0; i < numIter; ++i) {
            int j = 0;
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

        for (int j = 0; j < bufferSize; ++j) {
            generateNonNullValue(buffer1 + j, j);
            generateNonNullValue(buffer2 + j, j);
        }

        timer.reset();
        timer.start();
        for (int i = 0; i < numIter; ++i) {
            int j = 0;
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
                                  const char *rawBuffer1,
                                  const char *rawBuffer2,
                                  int         numIter)
{
    printf("\n\tcompare with '%s'\n", typeName);
    {
        const int bufferSize = rawBufferSize / sizeof(TYPE);
        TYPE *buffer1 = (TYPE*)rawBuffer1;
        TYPE *buffer2 = (TYPE*)rawBuffer2;

        for (int j = 0; j < bufferSize; ++j) {
            buffer1[j] = buffer2[j] = TYPE();
        }

        bsls::Stopwatch timer;
        timer.start();
        for (int i = 0; i < numIter; ++i) {
            int j = 0;
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

        for (int j = 0; j < bufferSize; ++j) {
            generateNonNullValue(buffer1 + j, j);
            generateNonNullValue(buffer2 + j, j);
        }

        timer.reset();
        timer.start();
        for (int i = 0; i < numIter; ++i) {
            int j = 0;
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
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
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

        {
            // Compare non-bitwise-comparable elements
            MyContainer<MyString> c1;
            MyContainer<MyString> c2;

            c1.push_back(MyString("hello"));
            c1.push_back(MyString("goodbye"));

            c2.push_back(MyString("hello"));
            c2.push_back(MyString("goodbye"));

            ASSERT(c1 == c2);
        }

        {
            // Compare bitwise-comparable elements
            MyContainer<MyPoint> c1;
            MyContainer<MyPoint> c2;

            c1.push_back(MyPoint(1, 2));
            c1.push_back(MyPoint(3, 4));

            c2.push_back(MyPoint(1, 2));
            c2.push_back(MyPoint(3, 4));

            ASSERT(c1 == c2);
        }

        {
            // Compare (bitwise-comparable) primitive types
            MyContainer<int> c1;
            MyContainer<int> c2;

            c1.push_back(1);
            c1.push_back(2);
            c1.push_back(3);

            c2.push_back(1);
            c2.push_back(2);
            c2.push_back(3);

            ASSERT(c1 == c2);
        }
     } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'lexicographical'
        //
        // Concerns:
        //   o That all implementations of 'equal' are correct.
        //   o That the correct implementation is selected for the
        //     correct trait and given iterator type.
        //   o That no instantiation leads to ambiguity.
        //
        // Plan:
        //
        // Testing:
        //  bool lexicographical(begin1, end1, length1, begin2, end2, length2);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'lexicographical'"
                            "\n=========================\n");

        if (verbose) printf("\t... with 'char'\n");
        {
            if (verbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographical<char>(veryVerbose, testAllocator);

            if (verbose) printf("\t\tUsing forward iterator.\n");
            testLexicographicalNonBitwise<char>(veryVerbose, testAllocator);
        }

        if (verbose) printf("\t... with 'signed char'\n");
        {
            if (verbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographical<char>(veryVerbose, testAllocator);

            if (verbose) printf("\t\tUsing forward iterator.\n");
            testLexicographicalNonBitwise<char>(veryVerbose, testAllocator);
        }

        if (verbose) printf("\t... with 'unsigned char'\n");
        {
            if (verbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographical<unsigned char>(veryVerbose, testAllocator);

            if (verbose) printf("\t\tUsing forward iterator.\n");
            testLexicographicalNonBitwise<unsigned char>(veryVerbose,
                                                         testAllocator);
        }

        if (verbose) printf("\t... with 'int'\n");
        {
            if (verbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographicalBuiltin<int>(veryVerbose,
                                            &intGenerator,
                                            testAllocator);
        }

        if (verbose) printf("\t... with 'long long'\n");
        {
            if (verbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographicalBuiltin<Uint64>(veryVerbose,
                                               &uint64Generator,
                                               testAllocator);
        }

        if (verbose) printf("\t... with 'double'\n");
        {
            if (verbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographicalBuiltin<double>(veryVerbose,
                                               &doubleGenerator,
                                               testAllocator);
        }

        if (verbose) printf("\t... with 'void *'\n");
        {
            if (verbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographicalBuiltin<void *>(veryVerbose,
                                               &ptrGenerator,
                                               testAllocator);
        }

        if (verbose) printf("\t... with non-BitwiseEqualityComparableType.\n");
        {
            if (verbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographical<my_Class1>(veryVerbose, testAllocator);

            if (verbose) printf("\t\tUsing forward iterator.\n");
            testLexicographicalNonBitwise<my_Class1>(veryVerbose,
                                                     testAllocator);
        }

        if (verbose) printf("\t... with BitwiseEqualityComparableType.\n");
        {
            if (verbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographical<my_Class2>(veryVerbose, testAllocator);

            // We cannot use forward iterators since it would require
            // 'operator==' for 'my_Class2'.
        }

        if (verbose) printf("\t... with ConvertibleToBool.\n");
        {
            if (verbose) printf("\t\tUsing pointer type for iterator.\n");
            testLexicographicalBuiltin<my_Class3>(veryVerbose,
                                                  &charGenerator,
                                                  testAllocator);
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'equal'
        //
        // Concerns:
        //   o That all implementations of 'equal' are correct.
        //   o That the correct implementation is selected for the
        //     correct trait and given iterator type.
        //
        // Plan:  Given an array of string specs, all different, we test every
        //   pair of strings for equality, the expected result being that the
        //   strings differ if their specs differ.  We then apply this test for
        //   all four types, 'char', 'unsigned' char', bitwise equality
        //   comparable UDT, and non-bitwise equality comparable UDT.  We make
        //   sure that the correct implementation is selected for the trait by
        //   not providing an 'operator==' when 'memcmp' should be used, and by
        //   stuffing some random bytes into the footprint of the class when
        //   memcmp should not be used.
        //
        // Testing:
        //   bool equal(begin1, end1, begin2, end2);
        //   bool equal(begin1, end1, length1, begin2, end2, length2);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'equal'"
                            "\n===============\n");

        if (verbose) printf("\t... generic 'equal' (four arguments).\n");

        if (verbose) printf("\t\tUsing pointer type for iterator.\n");
        testGenericEqual(veryVerbose, testAllocator);

        if (verbose) printf("\t... with 'char'.\n");
        {
            if (verbose) printf("\t\tUsing pointer type for iterator.\n");
            testEqual<char>(veryVerbose, testAllocator);

            if (verbose) printf("\t\tUsing forward iterator.\n");
            testEqualNonBitwise<char>(veryVerbose, testAllocator);
        }

        if (verbose) printf("\t... with 'unsigned char'.\n");
        {
            if (verbose) printf("\t\tUsing pointer type for iterator.\n");
            testEqual<unsigned char>(veryVerbose, testAllocator);

            if (verbose) printf("\t\tUsing forward iterator.\n");
            testEqualNonBitwise<unsigned char>(veryVerbose, testAllocator);
        }

        if (verbose) printf("\t... with non-BitwiseEqualityComparableType.\n");
        {
            if (verbose) printf("\t\tUsing pointer type for iterator.\n");
            testEqual<my_Class1>(veryVerbose, testAllocator);

            if (verbose) printf("\t\tMake sure std::memcmp is not used.\n");
            {
                my_Class1 buffer1[5];
                my_Class1 *string1 = buffer1;
                const my_Class1 *STRING1 = string1;
                gg(string1, "abcde");  // use some seeds

                my_Class1 buffer2[5];
                my_Class1 *string2 = buffer2;
                const my_Class1 *STRING2 = string2;
                gg(string2, "abcde");  // use same value but different seeds

                ASSERT(Obj::equal(STRING1, STRING1 + 5, 5,
                                  STRING2, STRING2 + 5, 5));
            }

            if (verbose) printf("\t\tUsing forward iterator.\n");
            testEqualNonBitwise<my_Class1>(veryVerbose, testAllocator);
        }

        if (verbose) printf("\t... with BitwiseEqualityComparableType.\n");
        {
            if (verbose) printf("\t\tUsing pointer type for iterator.\n");
            testEqual<my_Class2>(veryVerbose, testAllocator);

            // We cannot use forward iterators since it would require
            // 'operator==' for 'my_Class2'.
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //   That the basic usage is functional and correct.
        //
        // Plan:  Exercise basic usage of this component.
        //
        // Testing:
        //   This test exercises basic usage but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============");

        if (verbose) printf("\n\tTesting equality...");
        {
            const char *X = "This is a string";
            const int  X_LEN = strlen(X);

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

        if (verbose) printf("\n\tTesting lexicographical...");
        {
            const char *X = "This is a string";
            const int  X_LEN = strlen(X);

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
        //   It isn't clear if a single loop is slower or faster than memcmp
        //   for builtin and for user types.
        //
        // Plan:  Time both implementations (memcmp) for various fundamental
        //   types and taking care to separate the care where 'memcmp' can be
        //   used.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) printf("\nPERFORMANCE TEST"
                           "\n===============\n");

        enum {
            BUFFER_SIZE = 1 << 24,
            NUM_ITER    = 4
        };

        bslma::TestAllocator  testAllocator(veryVeryVerbose);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
