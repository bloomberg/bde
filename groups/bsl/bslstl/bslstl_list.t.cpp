// bslstl_list.t.cpp                                                  -*-C++-*-

#include <bslstl_list.h>

#include <bslstl_iterator.h>

#include <bslalg_autoarraydestructor.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_stdallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isconvertible.h>
#include <bslmf_isfundamental.h>
#include <bslmf_functionpointertraits.h>
#include <bslmf_integralconstant.h>
#include <bslmf_ismemberfunctionpointer.h>
#include <bslmf_ispointer.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_alignmentutil.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>
#include <bsls_util.h>

#include <bsltf_allocargumenttype.h>
#include <bsltf_argumenttype.h>
#include <bsltf_nonassignabletesttype.h>
#include <bsltf_stdallocatoradaptor.h>
#include <bsltf_stdstatefulallocator.h>
#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <stdexcept>  // 'length_error', 'out_of_range'
#include <algorithm>  // 'next_permutation'
#include <functional> // 'less'

#include <cctype>
#include <cstdlib>
#include <cstring>

#include <stddef.h>
#include <stdio.h>

using namespace BloombergLP;
using namespace bsl;
using bsls::NameOf;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The object under testing is a container whose interface and contract is
// dictated by the C++ standard.  The general concerns are compliance,
// exception safety (including the strong guarantee for insert and emplace),
// and proper dispatching (for member function templates such as assign and
// insert).  In addition, it is a value-semantic type whose salient attributes
// are size and value of each element in sequence.  This container is
// implemented in the form of a class template, and thus its proper
// instantiation for several types is a concern.  Regarding the allocator
// template argument, we use mostly a 'bsl::allocator' together with a
// 'bslma_TestAllocator' mechanism, but we also verify the C++ standard.
//
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'push_back' and 'clear' methods to be used by the generator functions 'g'
// and 'gg'.  Note that some manipulators must support aliasing, and those that
// perform memory allocation must be tested for exception neutrality via the
// 'bslma_testallocator' component.  After the mandatory sequence of cases
// (1--10) for value-semantic types (cases 5 and 10 are not implemented, as
// there is not output or streaming below bslstl), we test each individual
// constructor, manipulator, and accessor in subsequent cases.
//
// Abbreviations:
// --------------
// Throughout this test driver, we use
//     T            VALUE (template argument, no default)
//     A            ALLOCATOR (template argument, default: bsl::allocator<T>)
//     list<T,A>    bsl::list<VALUE_TYPE,ALLOCATOR>
//     list         list<T,A>
//     Args...      shorthand for a family of templates <A1>, <A1,A2>, etc.
//
// The of tests below is grouped as per the definition of list in the C++
// standard (construct, iterators, capacity...) rather than the canonical
// grouping of members per BDE convention (CREATORS, MANIPULATORS, ACCESSORS).
// ----------------------------------------------------------------------------
// class list<T,A> (list)
// ============================================================================
// TYPES AND TRAITS:
// [21] type reference
// [21] type const_reference
// [21] type size_type
// [21] type difference_type
// [21] type value_type
// [21] type allocator_type
// [21] type pointer
// [21] type const_pointer
// [16] type iterator
// [16] type reverse_iterator
// [16] type const_iterator
// [16] type const_reverse_iterator
// [11] Allocator traits and propagation
// [22] bslalg::HasStlIterators
// [22] bslma::UsesBslmaAllocator
// [22] bslmf::IsBitwiseMoveable
// [22] !bsl::is_trivially_copyable
//
// CREATORS
// [ 2] list(const A& a = A());
// [12] list(size_type n);
// [12] list(size_type n, const T& value = T(), const A& a = A());
// [12] template <class iter> list(iter f, iter l, const A& a = A());
// [ 7] list(const list& orig);
// [ 7] list(const list& orig, const A&);
// [ 7] list(list&& orig);
// [30] list(list&& orig, const A& = A());
// [ 2] ~list();
// [32] 'list' supports incomplete types
// [33] list(std::initializer_list, const A& = ALLOCATOR());
//
// MANIPULATORS
// [ 2] void primaryManipulator(list*, int);
// [ 2] void primaryCopier(list*, const T&);
// [ 9] list& operator=(const list& rhs);
// [31] list& operator=(list&& rhs);
// [33] list& operator=(std::initializer_list);
// [13] template <class Iter> assign(Iter first, Iter last);
// [13] void assign(size_type numElements, const T& val);
// [33] void assign(std::initializer_list);
// [16] iterator begin();
// [16] iterator end();
// [16] reverse_iterator rbegin();
// [16] reverse_iterator rend();
// [14] void resize(size_type n);
// [14] void resize(size_type n, const T& val);
// [15] reference front();
// [15] reference back();
// [17] void push_front(const T& value);
// [18] void pop_front();
// [17] void push_back(const T& value);
// [18] void pop_back();
// [17] iterator insert(const_iterator pos, const T& value);
// [17] iterator insert(const_iterator pos, size_type n, const T& value);
// [17] template <class Iter> void insert(CIter pos, Iter f, Iter l);
// [17] iterator emplace(const_iterator pos, Args&&... args);
// [17] void emplace_back(Args&&... args);
// [17] void emplace_front(Args&&... args);
// [29] iterator insert(const_iterator pos, T&& value);
// [29] void push_back(T&& value);
// [29] void push_front(T&& value);
// [18] iterator erase(const_iterator pos);
// [18] iterator erase(const_iterator first, const_iterator last);
// [19] void swap(Obj& rhs);
// [ 2] void clear();
// [24] void splice(iterator pos, list& other);
// [24] void splice(iterator pos, list&& other);
// [24] void splice(iterator pos, list& other, iterator i);
// [24] void splice(iterator pos, list&& other, iterator i);
// [24] void splice(iter pos, list& other, iter first, iter last);
// [24] void splice(iter pos, list&& other, iter first, iter last);
// [25] void remove(const T& val);
// [25] template <class PRED> void remove_if(PRED p);
// [26] void unique();
// [26] template <class BINPRED> void unique(BINPRED p);
// [27] void merge(list& other);
// [27] void merge(list&& other);
// [27] template <class COMP> void merge(list& other, COMP c);
// [27] template <class COMP> void merge(list&& other, COMP c);
// [28] void sort();
// [28] template <class COMP> void sort(COMP c);
// [23] void reverse();
//
// ACCESSORS
// [11] A get_allocator() const;
// [16] const_iterator begin() const;
// [16] const_iterator end() const;
// [16] const_reverse_iterator rbegin() const;
// [16] const_reverse_iterator rend() const;
// [ 4] bool empty() const;
// [ 4] size_type size() const;
// [14] size_type max_size() const;
// [15] const_reference front() const;
// [15] const_reference back() const;
//
// FREE OPERATORS
// [ 6] bool operator==(const list&, const list&);
// [ 6] bool operator!=(const list&, const list&);
// [20] bool operator<(const list&, const list&);
// [20] bool operator>(const list&, const list&);
// [20] bool operator<=(const list&, const list&);
// [20] bool operator>=(const list&, const list&);
// [19] void bsl::swap(Obj& lhs, Obj& rhs);
//
// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] int ggg(list<T,A> *object, const char *spec, int vF = 1);
// [ 3] list<T,A>& gg(list<T,A> *object, const char *spec);
// [ 8] Obj g(const char *spec);
// [ 4] iterator succ(iterator);
// [ 4] const_iterator succ(iterator) const;
// [ 4] T& nthElem(list& x, int n);
// [ 4] const T& nthElem(list& x, int n) const;
// [ 4] bool is_mutable(T& value);
// [ 4] bool is_mutable(const T& value);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [35] USAGE EXAMPLE
// [29] CONCERN: All emplace methods handle rvalues.
// [34] CONCERN: Methods qualified 'noexcept' in standard are so implemented.
//-----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BSL ASSERT TEST FUNCTION
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
//              STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
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
//                      PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU
#define TD BSLS_BSLTESTUTIL_FORMAT_TD

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
enum { k_NO_EXCEPT = false };
#else
enum { k_NO_EXCEPT = true };
#endif

// TYPES
class TestType;
class TestTypeNoAlloc;
class TestTypeOtherAlloc;
template <class TYPE>
class OtherAllocator;

typedef TestType                      T;    // uses 'bslma' allocators
typedef TestTypeNoAlloc               TNA;  // does not use 'bslma' allocators

#if 0
// TBD: commenting this out until bsltf can be enhanced to have types that
// use alternate allocators.
typedef TestTypeOtherAlloc            TOA;  // Uses non-'bslma' allocators

typedef OtherAllocator<TestType>           OAT;   // Non-'bslma' allocator
typedef OtherAllocator<TestTypeOtherAlloc> OATOA; // Non-'bslma' allocator
#endif

typedef bsltf::TemplateTestFacility   TstFacility;

typedef bsls::Types::Int64            Int64;
typedef bsls::Types::Uint64           Uint64;
typedef bsls::Types::IntPtr           IntPtr;

// Define DEFAULT DATA used in multiple test cases.
struct DefaultDataRow {
    int         d_line;       // source line number
    const char *d_spec_p;     // specification string, for input to 'gg'
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line spec
    //---- --------------
    { L_,  ""             },
    { L_,  "A"            },
    { L_,  "B"            },
    { L_,  "AA"           },
    { L_,  "CD"           },
    { L_,  "ABC"          },
    { L_,  "ADE"          },
    { L_,  "ABCD"         },
    { L_,  "BEFG"         },
    { L_,  "ACEG"         },
    { L_,  "ABCDE"        },
    { L_,  "BCDEF"        },
    { L_,  "HHBDF"        },
    { L_,  "ABCDEF"       },
    { L_,  "ABCDEFG"      },
    { L_,  "ABCDEFGH"     },
    { L_,  "ABCDEFGBBB"   },
    { L_,  "ABCDABCDABCD" },
};
enum { DEFAULT_NUM_DATA   = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA,
       DEFAULT_MAX_LENGTH = 12 };

// CONSTANTS
const int UNINITIALIZED_VALUE = '_';
const int DEFAULT_VALUE       = 'Z';
const int VA = 'A';
const int VB = 'B';
const int VC = 'C';
const int VD = 'D';
const int VE = 'E';
const int VF = 'F';
const int VG = 'G';
const int VH = 'H';
    // All test types have int value type.

const int LARGE_SIZE_VALUE = 10;
    // Declare a large value for insertions into the list.

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace bsl {

// List-specific print function.
template <class TYPE, class ALLOC>
void debugprint(const bsl::list<TYPE,ALLOC>& v)
    // Output the specified 'v' to stdout.
{
    if (v.empty()) {
        printf("<empty>");
    }
    else {
        typedef typename list<TYPE,ALLOC>::const_iterator CIter;
        printf("\"");
        for (CIter it = v.begin(); it != v.end(); ++it) {
            printf("%c", static_cast<char>(TstFacility::getIdentifier(*it)));
        }
        printf("\"");
    }
    fflush(stdout);
}

}  // close namespace bsl

namespace incorrect {

template <class TYPE>
void swap(TYPE&, TYPE&)
    // Fail.  In a successful test, this 'swap' should never be called.  It is
    // set up to be called (and fail) in the case where ADL fails to choose the
    // right 'swap' in 'invokeAdlSwap' below.
{
    ASSERT(0 && "incorrect swap called");
}

}  // close namespace incorrect

template <class TYPE>
void invokeAdlSwap(TYPE *a, TYPE *b)
    // Exchange the values of the specified '*a' and '*b' objects using the
    // 'swap' method found by ADL (Argument Dependent Lookup).
{
    using incorrect::swap;

    // A correct ADL will key off the types of '*a' and '*b', which will be of
    // our 'bsl' container type, to find the right 'bsl::swap' and not
    // 'incorrect::swap'.

    swap(*a, *b);
}

template <class TYPE>
void invokePatternSwap(TYPE *a, TYPE *b)
    // Exchange the values of the specified '*a' and '*b' objects using the
    // 'swap' method found by the recommended pattern for calling 'swap'.
{
    // Invoke 'swap' using the recommended pattern for 'bsl' clients.

    using bsl::swap;

    swap(*a, *b);
}

template <class TYPE>
inline
int value_of(const TYPE& x)
    // Return the int value corresponding to the specified 'x'.  Specialized
    // for each test type.
{
    return TstFacility::getIdentifier(x);
}

template <>
inline
int value_of<TstFacility::MethodPtr>(const TstFacility::MethodPtr& x)
{
    return TstFacility::MethodPtr() == x ? -1 : TstFacility::getIdentifier(x);
}

// Return the 'n'th iterator after 'it'.
template <class ITER>
inline
ITER succ(ITER it, Int64 n = 1)
    // Return an iterator pointing to the element the optionally specified 'n'
    // items after the specified 'it'.
{
    for (int i = 0; i < n; ++i) {
        ++it;
    }
    return it;
}

template <class CONTAINER>
inline
typename CONTAINER::value_type& nthElem(CONTAINER& x, Int64 n)
    // For a specified 'n', return the 'n'th element of the specified container
    // 'x', counting from 0.
{
    return *succ(x.begin(), n);
}

template <class CONTAINER>
inline
const typename CONTAINER::value_type& nthElem(const CONTAINER& x, Int64 n)
    // For a specified 'n', return the 'n'th element of the specified container
    // 'x', counting from 0.
{
    return *succ(x.begin(), n);
}

template <class TYPE>
inline
bool is_mutable(TYPE&)
    // Return 'true'.  Preferred match if argument is a modifiable lvalue.
{
    return true;
}

template <class TYPE>
inline
bool is_mutable(const TYPE&)
    // Return 'false'.  Preferred match if argument is an rvalue or const
    // lvalue.
{
    return false;
}

// Compare two integral types where possibly one of them is signed type and the
// other isn't.

#define CMPINT(a, op, b) (((a) >= 0) == ((b) >= 0) &&                         \
                                static_cast<Int64>(a) op static_cast<Int64>(b))

template <class ITER>
unsigned posDistance(ITER lhs, ITER rhs)
    // Return the distance between the specified 'lhs' and the specified 'rhs'.
    // The distance returned is unsigned, and it is an error if 'lhs' > 'rhs'
    // or if 'rhs' is not reachable from 'lhs' via 'operator++'.
{
    Int64 ret = bsl::distance(lhs, rhs);
    ASSERT(ret >= 0);
    return static_cast<unsigned>(ret);
}

//=============================================================================
//                      GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

// STATIC DATA
static int verbose;
static int veryVerbose;
static int veryVeryVerbose;
static int veryVeryVeryVerbose;

static bslma::TestAllocator *globalAllocator_p;
static bslma::TestAllocator *defaultAllocator_p;

static int numDefaultCtorCalls = 0;
static int numIntCtorCalls     = 0;
static int numCopyCtorCalls    = 0;
static int numAssignmentCalls  = 0;
static int numDestructorCalls  = 0;

                                // ---------------
                                // class Recursive
                                // ---------------

class Recursive {
    // The purpose of this class is to ensure that 'sizeof(list)' is
    // independent of 'sizeof(value_type)'.

    // PRIVATE DATA
    bsl::list<Recursive> d_list;

    typedef bsl::list<Recursive> ListType;

  public:
    // MANIPULATORS
    Recursive& back() { return d_list.back(); }
        // Return the back of the contained 'list'.
    Recursive& front() { return d_list.front(); }
        // Return the front of the contained 'list'.

    void pop_back()  { return d_list.pop_back();  }
        // Pop the back element off of the contained 'list'
    void pop_front() { return d_list.pop_front(); }
        // Pop the front element off of the contained 'list'

    void push_back(const Recursive& elem)  { return d_list.push_back(elem);  }
        // Push the specified 'elem' onto the back of the contained 'list'.
    void push_front(const Recursive& elem) { return d_list.push_front(elem); }
        // Push the specified 'elem' onto the front of the contained 'list'.

    // ACCESSORS

                            // *** element access ***

    const Recursive& back()  const { return d_list.back();  }
        // Return the back of the contained 'list'.
    const Recursive& front() const { return d_list.front(); }
        // Return the front of the contained 'list'.

                                 // *** misc ***

    bool empty() const { return d_list.empty(); }
        // Return 'true' is '0 == size()' and 'false' otherwise.
    ListType::size_type size() const { return d_list.size(); }
        // Return the number of 'Recursive' items contained in this container.
};

                               // ==============
                               // class TestType
                               // ==============

class TestType {
    // This test type contains a 'char' in some allocated storage.  It counts
    // the number of default and copy constructions, assignments, and
    // destructions.  It has no traits other than using a 'bslma' allocator.

    int              *d_data_p;
    bslma::Allocator *d_allocator_p;

    void init(const TestType& x)
        // This function is the innards of a c'tor copying from the specified
        // 'x'.
    {
        ++numIntCtorCalls;
        d_data_p  = static_cast<int *>(d_allocator_p->allocate(sizeof(int)));
        *d_data_p = *x.d_data_p;
    }

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TestType, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit
    TestType(bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
        // Create an object using the optionally specified 'ba'.
    {
        ++numDefaultCtorCalls;
        d_data_p  = static_cast<int *>(d_allocator_p->allocate(sizeof(int)));
        *d_data_p = DEFAULT_VALUE;
    }

    explicit
    TestType(int c, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
        // Create an object using the optionally specified 'ba' whose value is
        // initialized to that of the specified 'c'.
    {
        ++numIntCtorCalls;
        d_data_p  = static_cast<int *>(d_allocator_p->allocate(sizeof(int)));
        *d_data_p = c;
    }

    TestType(const TestType& original, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
        // Create a copy of the specified 'original' using the optionally
        // specified 'ba'.
    {
        ++numCopyCtorCalls;
        ASSERT(&original != this);
        d_data_p  = static_cast<int *>(d_allocator_p->allocate(sizeof(int)));
        *d_data_p = *original.d_data_p;
    }

    TestType(int a1, const TestType& x, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
        // Create a copy of the specified 'x' using the optionally specified
        // 'ba', checking that the specified 'a1' has its expected value.
    {
        ASSERT(1 == a1);
        init(x);
    }

    TestType(int a1, int a2, const TestType& x, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
        // Create a copy of the specified 'x' using the optionally specified
        // 'ba', checking that the specified 'a1' and the specified 'a2' have
        // their expected values.
    {
        ASSERT(1 == a1);
        ASSERT(2 == a2);
        init(x);
    }

    TestType(int               a1,
             int               a2,
             int               a3,
             const TestType&   x,
             bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
        // Create a copy of the specified 'x' using the optionally specified
        // 'ba', checking that the specified 'a1', the specified 'a2', and the
        // specified 'a3' have their expected values.
    {
        ASSERT(1 == a1);
        ASSERT(2 == a2);
        ASSERT(3 == a3);
        init(x);
    }

    TestType(int               a1,
             int               a2,
             int               a3,
             int               a4,
             const TestType&   x,
             bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
        // Create a copy of the specified 'x' using the optionally specified
        // 'ba', checking that the specified 'a1', the specified 'a2', the
        // specified 'a3', and the specified 'a4' have their expected values.
    {
        ASSERT(1 == a1);
        ASSERT(2 == a2);
        ASSERT(3 == a3);
        ASSERT(4 == a4);
        init(x);
    }

    ~TestType()
        // Destroy this object.
    {
        ++numDestructorCalls;
        ASSERT(d_data_p != 0);
        *d_data_p = UNINITIALIZED_VALUE;
        d_allocator_p->deallocate(d_data_p);
        d_data_p = 0;
        d_allocator_p = 0;
    }

    // MANIPULATORS
    TestType& operator=(const TestType& rhs)
        // Copy the contents of the specified 'rhs' into this object.
    {
        ++numAssignmentCalls;
        if (&rhs != this) {
            int *newData = static_cast<int *>(d_allocator_p->allocate(
                                                                 sizeof(int)));
            *d_data_p = UNINITIALIZED_VALUE;
            d_allocator_p->deallocate(d_data_p);
            d_data_p  = newData;
            *d_data_p = *rhs.d_data_p;
        }
        return *this;
    }

    void setDatum(int c)
        // Set the value of this object to the specified 'c'.
    {
        *d_data_p = c;
    }

    // ACCESSORS
    int value() const
        // Return the value of this object.
    {
        return *d_data_p;
    }

    operator int() const
        // Return the value of this object.
    {
        return value();
    }

    bslma::Allocator *allocator() const
        // Return the allocator used by this object.
    {
        return d_allocator_p;
    }

    void print() const
        // Print the value of this object.
    {
        if (d_data_p) {
            ASSERT(isalpha(*d_data_p));

            printf("%c (int: %d)\n", *d_data_p, *d_data_p);
        } else {
            printf("VOID\n");
        }
    }
};

// FREE OPERATORS
bool operator==(const TestType& lhs, const TestType& rhs)
    // Return 'true' if the specified 'lhs' and the specified 'rhs' have the
    // same value and 'false' otherwise.
{
    ASSERT(isalpha(lhs.value()));
    ASSERT(isalpha(rhs.value()));

    return lhs.value() == rhs.value();
}

bool operator<(const TestType& lhs, const TestType& rhs)
    // Return 'true' if the specified 'lhs' is less than the specified 'rhs'
    // and 'false' otherwise.
{
    ASSERT(isalpha(lhs.value()));
    ASSERT(isalpha(rhs.value()));

    return lhs.value() < rhs.value();
}

// TestType-specific value_of function.
template <>
inline
int value_of<TestType>(const TestType& x)
{
    return x.value();
}

// Specializations of std::less and std::equal_to should never be called.
// Certain algorithms have variants that call either a predicate function or
// operator<.  A non-compliant implementation may implement the latter variant
// by calling the former variant using std::less because most of the time, they
// are identical.  Unfortunately, the standard does not require that std::less
// do the same thing as operator<.  The same problem occurs with std::equal_to
// and operator==.  These specializations suppress the default definitions of
// std::less and std::equal_t and intercept stray calls by non-compliant
// implementations.

namespace std {

template <>
struct less<TestType> : std::binary_function<TestType, TestType, bool> {
    bool operator()(const TestType& a, const TestType& b) const
        // Return 'true' if the specified 'a' is less than the specified 'b'
        // and 'false' otherwise.
    {
        ASSERT(!"less<TestType> should not be called");
        return a < b;
    }
};

template <>
struct equal_to<TestType> : std::binary_function<TestType,
                                                 TestType,
                                                 bool> {
    bool operator()(const TestType& a, const TestType& b) const
        // Return 'true' if the specified 'a' is equal to the specified 'b' and
        // 'false' otherwise.
    {
        ASSERT(!"equal_to<TestType> should not be called");
        return a == b;
    }
};

}  // close namespace std

                       // =====================
                       // class TestTypeNoAlloc
                       // =====================

class TestTypeNoAlloc {
    // This test type has footprint and interface identical to 'TestType'.  It
    // also counts the number of default and copy constructions, assignments,
    // and destructions.

    // DATA
    union {
        // PUBLIC DATA
        int  d_int;
        int  d_fill[sizeof(TestType)];
    } d_u;

  public:
    // CREATORS
    TestTypeNoAlloc()
        // Create a default-valued object.
    {
        d_u.d_int = DEFAULT_VALUE;
        ++numDefaultCtorCalls;
    }

    explicit
    TestTypeNoAlloc(int c)
        // Create an object with the specified value 'c'.
    {
        d_u.d_int = c;
        ++numIntCtorCalls;
    }

    TestTypeNoAlloc(const TestTypeNoAlloc&  original)
        // Create an object, copying the value of the specified 'original'.
    {
        d_u.d_int = original.d_u.d_int;
        ++numCopyCtorCalls;
    }

    ~TestTypeNoAlloc()
        // Destroy this object.
    {
        ++numDestructorCalls;
        d_u.d_int = '_';
    }

    // MANIPULATORS
    TestTypeNoAlloc& operator=(const TestTypeNoAlloc& rhs)
        // Assign the value of the specified 'rhs' to this object.
    {
        ++numAssignmentCalls;
        d_u.d_int = rhs.d_u.d_int;
        return *this;
    }

    void setDatum(int c)
        // Set the value of this object to the specified 'c'.
    {
        d_u.d_int = c;
    }

    // ACCESSORS
    int value() const
        // Return the value of this object.
    {
        return d_u.d_int;
    }

    operator int() const
        // Return the value of this object.
    {
        return value();
    }

    void print() const
        // Print this object to 'stdout'.
    {
        ASSERT(isalpha(d_u.d_int));
        printf("%c (int: %d)\n", d_u.d_int, d_u.d_int);
    }
};

// FREE OPERATORS
bool operator==(const TestTypeNoAlloc& lhs, const TestTypeNoAlloc& rhs)
{
    ASSERT(isalpha(lhs.value()));
    ASSERT(isalpha(rhs.value()));

    return lhs.value() == rhs.value();
}

// TestTypeNoAlloc-specific value_of function.
template <>
inline
int value_of<TestTypeNoAlloc>(const TestTypeNoAlloc& x)
{
    return x.value();
}

bslma::TestAllocator OtherAllocatorDefaultImp;

#if 0
                               // ====================
                               // class OtherAllocator
                               // ====================

template <class T>
class OtherAllocator {
    // This 'class' is An STL allocator type other than 'bsl::allocator'.  Like
    // 'bsl::allocator', it is constructed with a 'bslma::Allocator' pointer,
    // but it is not implicitly convertible from 'bslma::Allocator *'.  An
    // additional constraint is that this class is not trivially copyable, and
    // so containers using this allocator should not be bitwise movable.

    bslma::Allocator* d_implementation;

  public:
    // TYPES
    typedef T          value_type;
    typedef T         *pointer;
    typedef const T   *const_pointer;
    typedef T&         reference;
    typedef const T&   const_reference;
    typedef size_t     size_type;
    typedef ptrdiff_t  difference_type;

    template <class U>
    struct rebind
    {
        typedef OtherAllocator<U> other;
    };

    // CREATORS
    OtherAllocator() : d_implementation(&OtherAllocatorDefaultImp) {}

    explicit OtherAllocator(bslma::Allocator* a) : d_implementation(a) {}

    OtherAllocator(const OtherAllocator& original)
    : d_implementation(original.implementation())
    {}

    template <class U>
    OtherAllocator(const OtherAllocator<U>& original)
    : d_implementation(original.implementation())
        // non-trivial copy
    {}

    // MANIPULATORS
    T* allocate(size_t n, const void* = 0)
    {
        return static_cast<T*>(d_implementation->allocate(sizeof(T)*n));
    }
    void deallocate(T* p, size_t) { d_implementation->deallocate(p); }
    void construct(pointer p, const T& v) { ::new(p) T(v); }
    void destroy(pointer p) { p->~T(); }

    // ACCESSORS
    size_t max_size() const { return (static_cast<size_t>(-1)) / sizeof(T); }
    bslma::Allocator* implementation() const { return d_implementation; }
};

template <class T, class U>
inline
bool operator==(const OtherAllocator<T>& lhs, const OtherAllocator<U>& rhs)
{
    return lhs.implementation() == rhs.implementation();
}

template <class T, class U>
inline
bool operator!=(const OtherAllocator<T>& lhs, const OtherAllocator<U>& rhs)
{
    return lhs.implementation() != rhs.implementation();
}

                               // ========================
                               // class TestTypeOtherAlloc
                               // ========================

class TestTypeOtherAlloc {
    // This test type contains a 'char' in some allocated storage.  It counts
    // the number of default and copy constructions, assignments, and
    // destructions.  It has no traits other than using a 'bslma' allocator.

    char                 *d_data_p;
    OtherAllocator<char>  d_allocator;

  public:
    // TYPES
    typedef OtherAllocator<char> allocator_type;

    // CREATORS
    explicit
    TestTypeOtherAlloc(allocator_type a = allocator_type())
    : d_data_p(0)
    , d_allocator(a)
    {
        ++numDefaultCtorCalls;
        d_data_p  = d_allocator.allocate(1);
        *d_data_p = DEFAULT_VALUE;
    }

    explicit
    TestTypeOtherAlloc(char c, allocator_type a = allocator_type())
    : d_data_p(0)
    , d_allocator(a)
    {
        ++numIntCtorCalls;
        d_data_p  = d_allocator.allocate(1);
        *d_data_p = c;
    }

    TestTypeOtherAlloc(const TestTypeOtherAlloc& original,
                       allocator_type            a = allocator_type())
    : d_data_p(0)
    , d_allocator(a)
    {
        ++numCopyCtorCalls;
        if (&original != this) {
            d_data_p  = d_allocator.allocate(1);
            *d_data_p = *original.d_data_p;
        }
    }

    ~TestTypeOtherAlloc()
    {
        ++numDestructorCalls;
        *d_data_p = UNINITIALIZED_VALUE;
        d_allocator.deallocate(d_data_p, 1);
        d_data_p = 0;
    }

    // MANIPULATORS
    TestTypeOtherAlloc& operator=(const TestTypeOtherAlloc& rhs)
    {
        ++numAssignmentCalls;
        if (&rhs != this) {
            char *newData  = d_allocator.allocate(1);
            *d_data_p = UNINITIALIZED_VALUE;
            d_allocator.deallocate(d_data_p, 1);
            d_data_p  = newData;
            *d_data_p = *rhs.d_data_p;
        }
        return *this;
    }

    void setDatum(char c)
    {
        *d_data_p = c;
    }

    // ACCESSORS
    char value() const
    {
        return *d_data_p;
    }

    operator int() const
    {
        return static_cast<int>(value());
    }

    allocator_type get_allocator() const
    {
        return d_allocator;
    }

    bslma::Allocator* allocator() const
    {
        return d_allocator.implementation();
    }

    void print() const
    {
        if (d_data_p) {
            ASSERT(isalpha(*d_data_p));
            printf("%c (int: %d)\n", *d_data_p, (int)*d_data_p);
        } else {
            printf("VOID\n");
        }
    }
};

// FREE OPERATORS
bool operator==(const TestTypeOtherAlloc& lhs, const TestTypeOtherAlloc& rhs)
{
    ASSERT(isalpha(lhs.value()));
    ASSERT(isalpha(rhs.value()));

    return lhs.value() == rhs.value();
}

bool operator<(const TestTypeOtherAlloc& lhs, const TestTypeOtherAlloc& rhs)
{
    ASSERT(isalpha(lhs.value()));
    ASSERT(isalpha(rhs.value()));

    return lhs.value() < rhs.value();
}

// TestTypeOtherAlloc-specific print function.

// TestTypeOtherAlloc-specific value_of function.
template <>
inline
int value_of<TestTypeOtherAlloc>(const TestTypeOtherAlloc& x)
{
    return x.value();
}
#endif

                            // ===================
                            // function 'assignTo'
                            // ===================

template <class TYPE>
void assignTo(TYPE *element, int value)
    // For all types to be contained in 'list's in this test driver, assign the
    // specified 'value' to the specified '*element'.
{
    *element = TYPE(static_cast<IntPtr>(value));    // 'reinterpret_cast'
                                                    // doesn't work here, the
                                                    // the function-call cast
                                                    // is necessary.
}

template <>
void assignTo<const char *>(const char **element,
                            int          value)
{
    const char *x;                       // d'tor is trivial
    TstFacility::emplace(&x, value, 0);
    *element = x;
}

template <>
void assignTo<TstFacility::MethodPtr>(TstFacility::MethodPtr *element,
                                      int                     value)
{
    TstFacility::MethodPtr x;            // d'tor is trivial
    TstFacility::emplace(&x, value, 0);
    *element = x;
}

template <>
void assignTo<bsltf::MoveOnlyAllocTestType>(
                                         bsltf::MoveOnlyAllocTestType *element,
                                         int                           value)
{
    element->setData(value);
}

template <>
void assignTo<TestType>(TestType *element,
                        int       value)
{
    element->setDatum(value);
}

template <>
void assignTo<TestTypeNoAlloc>(TestTypeNoAlloc *element,
                               int              value)
{
    element->setDatum(value);
}

                              // =============
                              // class RandSeq
                              // =============

template <class TYPE>
class RandSeq {
    // Random-access sequence.  This class is a simple wrapper around an array
    // offering a random-access iterator access via the 'begin' and 'end'
    // accessors.  The iterator is specifically a *random-access* iterator and
    // its value type is the parameterized 'TYPE'.

    // DATA
    TYPE*  d_values_p;
    size_t d_len;

  public:
    // TYPES
    typedef const TYPE *const_iterator;
        // Random-access iterator.

    // CREATORS
    RandSeq() : d_values_p(0), d_len(0) {}
        // Default-construct an empty object.

    explicit
    RandSeq(const char* spec);
        // Create an object described by the specified 'spec'.

    ~RandSeq();
        // Destroy this object.

    // ACCESSORS
    const TYPE& operator[](size_t index) const;
        // Given a specified 'index', return the 'index'th element in this
        // object.

    const_iterator begin() const;
        // Return an iterator referring to the first element in this object.

    const_iterator end() const;
        // Return an iterator referring to the last element in this object.
};

// CREATORS
template <class TYPE>
RandSeq<TYPE>::RandSeq(const char* spec)
    : d_values_p(0), d_len(std::strlen(spec))
{
    if (d_len)
    {
        d_values_p = static_cast<TYPE *>(::operator new(d_len * sizeof(TYPE)));
        for (size_t i = 0; i < d_len; ++i) {
            TstFacility::emplace(bsls::Util::addressOf(d_values_p[i]),
                                 spec[i],
                                 bslma::Default::defaultAllocator());
        }
    }
}

template <class TYPE>
RandSeq<TYPE>::~RandSeq()
{
    for (size_t i = 0; i < d_len; ++i) {
        d_values_p[i].~TYPE();
    }

    ::operator delete(d_values_p);
}

// ACCESSORS
template <class TYPE>
const TYPE& RandSeq<TYPE>::operator[](size_t index) const
{
    return d_values_p[index];
}

template <class TYPE>
typename RandSeq<TYPE>::const_iterator RandSeq<TYPE>::begin() const
{
    return const_iterator(d_values_p);
}

template <class TYPE>
typename RandSeq<TYPE>::const_iterator RandSeq<TYPE>::end() const
{
    return const_iterator(d_values_p + d_len);
}

                               // ==============
                               // class InputSeq
                               // ==============

template <class TYPE>
class InputSeq;
template <class TYPE>
class InputSeqConstIterator;

template <class TYPE>
bool operator==(InputSeqConstIterator<TYPE> lhs,
                InputSeqConstIterator<TYPE> rhs);

template <class TYPE>
class InputSeqConstIterator {
    // Const iterator type for the InputSeq container type.  This iterator is
    // specifically an *input* iterator.  Iteration is not intended to be
    // restarted from a copy of an iterator.  If one iterator is a copy of
    // another, then incrementing either one will invalidate the other one.
    // Use of an invalidated iterator is checked for.

    typedef typename RandSeq<TYPE>::const_iterator BaseIterType;
    typedef bsl::iterator_traits<BaseIterType>     BaseIterTraits;

    // Input iterators are not value-semantic types.  In particular, if two
    // iterators compare equal and both are incremented, they need not continue
    // to compare equal.  We allow only one "active" iterator on a given
    // sequence.  We keep track of the active iterator by keeping a master
    // iterator in the container.  The 'd_master' member of valid iterator will
    // point to the master and its 'd_imp' member will have the same value as
    // the master.  If a valid iterator is copied by copy construction or
    // assignment, then both the original and copy are valid.  However, as soon
    // as one is incremented, the 'd_imp' of other one will no longer match the
    // master, making it invalid.  As a special case, an iterator with a null
    // master is valid but not incrementable.  These special iterators are used
    // for the 'end' iterator and for the return of the post-increment
    // operator.

    BaseIterType *d_master_p;
    BaseIterType  d_imp;

    InputSeqConstIterator(BaseIterType *m, BaseIterType i)
    : d_master_p(m), d_imp(i)
        // Construct an iterator using the specified 'm' as the address of the
        // master iterator and the specified 'i' as the initial iterator
        // position.  If 'm' is null, then the resulting iterator is valid, but
        // may not be incremented.  If 'm' is non-null and '*m' does not equal
        // 'i', then the resulting iterator is invalid -- it may not be
        // dereferenced or incremented, but it may be assigned a new value.
    {}

    friend class InputSeq<TYPE>;
    template <class OTHER_TYPE>
    friend bool operator==(InputSeqConstIterator<OTHER_TYPE>,
                           InputSeqConstIterator<OTHER_TYPE>);

  public:
    typedef std::input_iterator_tag                  iterator_category;
    typedef typename BaseIterTraits::value_type      value_type;
    typedef typename BaseIterTraits::difference_type difference_type;
    typedef typename BaseIterTraits::pointer         pointer;
    typedef typename BaseIterTraits::reference       reference;

    // Use compiler-generated copy constructor, assignment, and destructor:
    // InputSeqConstIterator(const InputSeqConstIterator&);
    // InputSeqConstIterator& operator=(const InputSeqConstIterator&);
    // ~InputSeqConstIterator();

    // MANIPULATORS
    InputSeqConstIterator& operator++()
        // Pre-increment this object.
    {
        ASSERT(d_master_p && d_imp == *d_master_p);  // test if incrementable
        if (!(d_master_p && d_imp == *d_master_p)) {
            // Continue test despite error by creating self-mastered iterator.
            // This assignment also prevents cascade errors.
            d_master_p = &d_imp;
        }
        d_imp = ++*d_master_p;
        return *this;
    }

#if 0
    const InputSeqConstIterator operator++(int)
    {
        InputSeqConstIterator ret(0,d_imp);

        ++*this;
        return ret;
    }
#endif

    // ACCESSORS
    reference operator*() const
        // Dereference this iterator.
    {
        ASSERT( ! d_master_p || d_imp == *d_master_p); // test if valid
        return *d_imp;
    }

    pointer operator->() const
        // Dereference this iterator.
    {
        return bsls::Util::addressOf(this->operator*());
    }
};

template <class TYPE>
inline
bool operator==(InputSeqConstIterator<TYPE> lhs,
                InputSeqConstIterator<TYPE> rhs)
{
    return (lhs.d_imp == rhs.d_imp &&
                    (lhs.d_master_p == rhs.d_master_p || 0 == lhs.d_master_p ||
                                                         0 == rhs.d_master_p));
}

template <class TYPE>
inline
bool operator!=(InputSeqConstIterator<TYPE> lhs,
                InputSeqConstIterator<TYPE> rhs)
{
    return !(lhs == rhs);
}

template <class TYPE>
class InputSeq {
    // This array class is a simple wrapper on a 'char' array offering an input
    // iterator access via the 'begin' and 'end' accessors.  The iterator is
    // specifically an *input* iterator and its value type is the parameterized
    // 'TYPE'.

    // DATA
    RandSeq<TYPE>                                  d_value;
    mutable typename RandSeq<TYPE>::const_iterator d_masterIter;

  public:
    // TYPES
    typedef InputSeqConstIterator<TYPE> const_iterator;

    // CREATORS
    InputSeq()
    {
        // Default-construct an 'InputSeq'.
    }


    explicit
    InputSeq(const char* value);
        // Create an 'InputSeq' sequence, initializing its contents according
        // to the specified 'value' specification.

    // ACCESSORS
    const_iterator begin() const;
        // Return an iterator to the beginning of this container.

    const_iterator end() const;
        // Return an iterator after the last element of this container.

    const TYPE& operator[](int index) const
        // Return the randomly-accessed element of this container at the
        // specified 'index'.
    {
        return d_value[index];
    }
};

// CREATORS
template <class TYPE>
InputSeq<TYPE>::InputSeq(const char* value)
: d_value(value)
{
}

// ACCESSORS
template <class TYPE>
inline
InputSeqConstIterator<TYPE> InputSeq<TYPE>::begin() const
{
    d_masterIter = d_value.begin();
    return InputSeqConstIterator<TYPE>(&d_masterIter, d_masterIter);
}

template <class TYPE>
inline
InputSeqConstIterator<TYPE> InputSeq<TYPE>::end() const
{
    return InputSeqConstIterator<TYPE>(0, d_value.end());
}

                              // ====================
                              // class LimitAllocator
                              // ====================

template <class ALLOC>
class LimitAllocator : public ALLOC {
  public:
    // TYPES
    typedef typename ALLOC::value_type        value_type;
    typedef typename ALLOC::pointer           pointer;
    typedef typename ALLOC::const_pointer     const_pointer;
    typedef typename ALLOC::reference         reference;
    typedef typename ALLOC::const_reference   const_reference;
    typedef typename ALLOC::size_type         size_type;
    typedef typename ALLOC::difference_type   difference_type;

    template <class OTHER_TYPE>
    struct rebind {
        // It is better not to inherit the rebind template, or else
        // rebind<X>::other would be ALLOC::rebind<OTHER_TYPE>::other instead
        // of LimitAlloc<X>.

        typedef LimitAllocator<typename ALLOC::template
                                             rebind<OTHER_TYPE>::other > other;
    };

  private:
    // PRIVATE TYPES
    typedef ALLOC AllocBase;

    // DATA
    size_type d_limit;

  public:
    // CREATORS
    LimitAllocator()
    : d_limit(-1)
        // Default-construct a 'LimitAllocator'
    {}

    // Parameterize to make this a better match than the next constructor.

    template <class BSLMA_ALLOC>
    explicit
    LimitAllocator(BSLMA_ALLOC *mechanism)
    : AllocBase(mechanism), d_limit(-1)
        // Create a 'LimitAllocator' based on the specified 'mechanism'.
    {}

    template <class U_ALLOC>
    LimitAllocator(const U_ALLOC& other)                            // IMPLICIT
    : AllocBase(other), d_limit(other.max_size())
        // Implicitly create a 'LimitAllocator' from a specified non-pointer
        // allocator 'other'.
    {}

    ~LimitAllocator()
        // Destroy this 'LimitAllocator'.
    { }

    // MANIPULATORS
    void setMaxSize(size_type maxSize)
        // Set the limit of this object to the specified 'maxSize'.
    {
        d_limit = maxSize;
    }

    // ACCESSORS
    size_type max_size() const
        // Return the limit associated with this object.
    {
        return d_limit;
    }
};

                            // ==========================
                            // class StatefulStlAllocator
                            // ==========================

template <class VALUE>
class StatefulStlAllocator : public bsltf::StdTestAllocator<VALUE>
    // This class implements a standard compliant allocator that has an
    // attribute, 'id'.
{
    // DATA
    int d_id;  // identifier

  private:
    // TYPES
    typedef bsltf::StdTestAllocator<VALUE> StlAlloc;
        // Alias for the base class.

  public:
    template <class OTHER_TYPE>
    struct rebind {
        // This nested 'struct' template, parameterized by some 'OTHER_TYPE',
        // provides a namespace for an 'other' type alias, which is an
        // allocator type following the same template as this one but that
        // allocates elements of 'OTHER_TYPE'.  Note that this allocator type
        // is convertible to and from 'other' for any 'OTHER_TYPE' including
        // 'void'.

        typedef StatefulStlAllocator<OTHER_TYPE> other;
    };

    // CREATORS
    StatefulStlAllocator()
        // Create a 'StatefulStlAllocator' object.
    : StlAlloc()
    {
    }

    //! StatefulStlAllocator(const StatefulStlAllocator& original) = default;
        // Create a 'StatefulStlAllocator' object having the same id as the
        // specified 'original'.

    template <class OTHER_TYPE>
    StatefulStlAllocator(const StatefulStlAllocator<OTHER_TYPE>& original)
        // Create a 'StatefulStlAllocator' object having the same id as the
        // specified 'original' with a different template type.
    : StlAlloc(original)
    , d_id(original.id())
    {
    }

    // MANIPULATORS
    void setId(int value)
        // Set the 'id' attribute of this object to the specified 'value'.
    {
        d_id = value;
    }

    // ACCESSORS
    int id() const
        // Return the value of the 'id' attribute of this object.
    {
        return d_id;
    }
};

class SortedSpecGen
{
    // Generate every possible specification up to 4 elements long such that no
    // element has a value less than the previous element.  Using 5 possible
    // values for each element, this results in 126 possible combinations.

  public:
    enum { MAX_SPEC_LEN = 4 };

  private:
    // DATA
    int          d_len;
    char         d_spec[MAX_SPEC_LEN + 1];
    mutable char d_reverse_spec[MAX_SPEC_LEN + 1];

    enum { MAX_ELEMENT = 'E' };

  public:
    // CREATOR
    SortedSpecGen()
    : d_len(0)
        // Create an empty object.
    {
        d_spec[0] = '\0';
    }

    // MANIPULATOR
    SortedSpecGen& operator++()
        // Advance to the next specification.
    {
        // Find the last element with value < MAX_ELEMENT.  Note that with
        // 'MAX_ELEMENT' set to 'F', we are using only 6 of the possible 8
        // values for each element yielding a total of 462 combinations.  For
        // more combinations (and a slower test), extend 'MAX_ELEMENT' to 'G'
        // or (max) 'H'.

        char max_elem_str[2] = { MAX_ELEMENT, 0 };
        int  i = static_cast<int>(std::strcspn(d_spec, max_elem_str)) - 1;

        // If nothing was found, then 'd_spec' is all MAX_ELEMENTs.  Increment
        // length and start over with all 'A's.

        if (i < 0) {
            ++d_len;
            if (MAX_SPEC_LEN < d_len) return *this;                   // RETURN
            std::memset(d_spec, 'A', d_len);
            d_spec[d_len] = '\0';
            return *this;                                             // RETURN
        }

        // d_spec[i] < MAX_ELEMENT.  Increment the element at 'i' and fill the
        // remainder of the spec with the same value.

        char x = static_cast<char>(d_spec[i] + 1);
        std::memset(d_spec + i, x, d_len - i);
        return *this;
    }

    operator bool() const
        // Return true if this object holds a valid spec.
    {
        if (d_len > MAX_SPEC_LEN) {
            return false;                                             // RETURN
        }

        for (int ii = 1; ii < d_len; ++ii) {
            if (d_spec[ii - 1] > d_spec[ii]) {
                return false;                                         // RETURN
            }
        }

        return true;
    }

    // ACCESSORS
    int len() const
        // Return the length of this object.
    {
        return d_len;
    }

    const char* reverse_spec() const
        // Derive 'd_reverse_spec' from 'd_spec' and return a ptr to it.
    {
        for (int i = 0; i < d_len; ++i) {
            d_reverse_spec[d_len - i - 1] = d_spec[i];
        }
        d_reverse_spec[d_len] = '\0';
        return d_reverse_spec;
    }

    const char* spec() const
        // Return a pointer to the spec.
    {
        return d_spec;
    }
};  // End class SortedSpecGen

namespace BloombergLP {
namespace bslmf {

template <class ALLOCATOR>
struct IsBitwiseMoveable<LimitAllocator<ALLOCATOR> >
    : IsBitwiseMoveable<ALLOCATOR>
{};

}  // close namespace bslmf

namespace bslma {

template <class ALLOCATOR>
struct UsesBslmaAllocator<LimitAllocator<ALLOCATOR> >
    : bsl::is_convertible<Allocator*, ALLOCATOR>
{};

}  // close namespace bslma
}  // close enterprise namespace

#if 0
                              // ====================
                              // class PointerWrapper
                              // ====================

template <class T>
class PointerWrapper;

template <class T>
class PointerWrapper<const T>
{
    // Wrapper around a raw pointer to const T.  Convertible both ways.

    // DATA
    T* d_imp;

  public:
    PointerWrapper() { }
    explicit
    PointerWrapper(const T* p) : d_imp(const_cast<T*>(p)) {}

    operator const T* ()  const { return d_imp; }
    const T* operator->() const { return d_imp; }
    const T& operator*()  const { return *d_imp; }
};

template <class T>
class PointerWrapper : public PointerWrapper<const T>
{
    // Wrapper around a raw pointer to mutable T.  Convertible both ways.

  public:
    PointerWrapper() { }
    explicit
    PointerWrapper(T* p) { this->d_imp = p; }

    operator T* ()  const { return this->d_imp; }
    T* operator->() const { return this->d_imp; }
    T& operator*()  const { return *this->d_imp; }
};

                              // ====================
                              // class SmallAllocator
                              // ====================

template <class T>
class SmallAllocator : public bsl::allocator<T> {
    // Allocator type with small size and difference types and non-raw pointer
    // types.  Used to test that these types are used in the interface to the
    // container.

    // PRIVATE TYPES
    typedef bsl::allocator<T> AllocBase;

  public:
    // TYPES
    typedef typename AllocBase::value_type        value_type;
    // typedef typename AllocBase::pointer           pointer;
    // typedef typename AllocBase::const_pointer     const_pointer;

    typedef PointerWrapper<T>                     pointer;
    typedef PointerWrapper<const T>               const_pointer;
    typedef typename AllocBase::reference         reference;
    typedef typename AllocBase::const_reference   const_reference;
    typedef unsigned short                        size_type;
    typedef short                                 difference_type;

    template <class OTHER_TYPE>
    struct rebind {
        typedef SmallAllocator<OTHER_TYPE> other;
    };

    // CREATORS
    SmallAllocator() { }

    explicit SmallAllocator(bslma::Allocator *mechanism)
        : AllocBase(mechanism) { }

    template <class U>
    SmallAllocator(const SmallAllocator<U>& original) : AllocBase(original) {}

    ~SmallAllocator() { }
};
#endif

                              // ================
                              // class IntWrapper
                              // ================

class IntWrapper
{
    // Simple wrapper object implicitly convertible from 'int'.

    int d_val;

  public:
    IntWrapper(int v = 0)                                           // IMPLICIT
    : d_val(v)
        // Create an 'IntWrapper' object initialized to the specified 'v'.
    {}

    int value() const
        // Return the value of this 'IntWrapper' object.
    {
        return d_val;
    }
};

inline
bool operator==(IntWrapper lhs, IntWrapper rhs)
{
    return lhs.value() == rhs.value();
}

inline
bool operator!=(IntWrapper lhs, IntWrapper rhs)
{
    return ! (lhs == rhs);
}

enum TestEnum { TWO = 2, NINETYNINE = 99 };

//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Example 1: Filter "Twinkle Star"
/// - - - - - - - - - - - - - - - -
// Suppose an observatory needs to analyze the results of a sky survey.  The
// raw data is a text file of star observations where each star is represented
// by a tuple of three numbers: (x, y, b), where x and y represent the angular
// coordinates of the star in the sky and b represents its brightness on a
// scale of 0 to 100.  A star having brightness 75 or higher is of particular
// interest, which is named "twinkle star".
//
// Our first example will read such a data file as described above, filter out
// the dim stars (brightness less than 75), and count the "twinkle star"s left
// in the list.  Our test data set has been selected such that there are 10
// stars in the set, of which 4 are sufficiently bright as to pass our filter.
//
// First, we define the class 'Star' that encapsulates a single tuple, and
// provides accessors functions 'x', 'y', and 'brightness', file I/O functions
// 'read' and 'write', and free operators '==', '!=', and '<':
//..
    class Star
        // This class represents a star as seen through a digital telescope.
    {
        // DATA
        double d_x, d_y;     // coordinates

        int    d_brightness; // brightness on a scale of 0 to 100

      public:
        // CREATORS
        Star()
            // Create a 'Star' object located at coordinates '(0, 0)' having
            // '0' brightness.
        : d_x(0), d_y(0), d_brightness(0)
        {
        }

        Star(double x, double y, int b)
            // Create a 'Star' object located at the specified coordinates
            // '(x, y)' having the specified 'b' brightness.
        : d_x(x), d_y(y), d_brightness(b)
        {
        }

        // Compiler-generated copy construction, assignment, and destructor
        //! Star(const Star&) = default;
        //! Star& operator=(const Star&) = default;
        //! ~Star() = default;

        // MANIPULATORS
        bool read(FILE *input);
            // Read x, y, and brightness from the specified 'input' file.
            // Return 'true' if the read succeeded and 'false' otherwise.

        void write(FILE *output) const;
            // Write x, y, and brightness to the specified 'output' file
            // followed by a newline.

        // ACCESSORS
        double x() const
            // Return the x coordinate of this 'Star' object.
        {
            return d_x;
        }

        double y() const
            // Return the y coordinate of this 'Star' object.
        {
            return d_y;
        }

        int brightness() const
            // Return the brightness of this 'Star' object.
        {
            return d_brightness;
        }
    };

    bool Star::read(FILE *input)
    {
        int ret = fscanf(input, "%lf %lf %d", &d_x, &d_y, &d_brightness);
        return 3 == ret;
    }

    void Star::write(FILE *output) const
    {
        fprintf(output, "%f %f %d\n", d_x, d_y, d_brightness);
    }

    bool operator==(const Star& lhs, const Star& rhs)
    {
        return lhs.x() == rhs.x()
            && lhs.y() == rhs.y()
            && lhs.brightness() == rhs.brightness();
    }

    bool operator!=(const Star& lhs, const Star& rhs)
    {
        return !(lhs == rhs);
    }

    bool operator<(const Star& lhs, const Star& rhs)
    {
        if      (lhs.x() < rhs.x()) {
            return true;                                              // RETURN
        }
        else if (rhs.x() < lhs.x()) {
            return false;                                             // RETURN
        }
        else if (lhs.y() < rhs.y()) {
            return true;                                              // RETURN
        }
        else if (rhs.y() < lhs.y()) {
            return false;                                             // RETURN
        }
        else {
            return lhs.brightness() < rhs.brightness();               // RETURN
        }
    }

    // FREE FUNCTIONS
    bool operator==(const Star& lhs, const Star& rhs);
        // Return 'true' if the specified 'lhs' has a value equal to that of
        // the specified 'rhs'.
    bool operator!=(const Star& lhs, const Star& rhs);
        // Return 'true' if the specified 'lhs' has a value that is not equal
        // to that of the specified 'rhs'.
    bool operator< (const Star& lhs, const Star& rhs);
        // Return 'true' if the specified 'lhs' has a value less than that of
        // the specified 'rhs'.

//..
// Then, we define a 'readData' method that reads a file of data points and
// appends each onto a list.  The stars are stored in the data file in
// ascending sorted order by x and y coordinates.
//..
    void readData(list<Star> *starList, FILE *input)
        // Read stars from the specified 'input' and populate the specified
        // list '*starList' with them.
    {
        Star s;
        while (s.read(input)) {
            starList->push_back(s);
        }
    }
//..
// Now, we define the 'filter' method, which is responsible for removing stars
// with a brightness of less than 75 from the data set.  It does this by
// iterating over the list and erasing any element that does not pass the
// filter.  The list object features a fast 'erase' member function.  The
// return value of 'erase' is an iterator to the element immediately following
// the erased element:
//..
    void filter(list<Star> *starList)
        // Remove stars with brightness less than 75 from the specified
        // '*starList'.
    {
        static const int threshold = 75;

        list<Star>::iterator i = starList->begin();
        while (i != starList->end()) {
            if (i->brightness() < threshold) {
                i = starList->erase(i);  // Erase and advance to next element.
            }
            else {
                ++i;  // Advance to next element without erasing
            }
        }
    }
//..
// Finally, we use the methods defined in above steps to put together our
// program to find twinkle stars:
//..
int usageExample1(int verbose)
    // Demonstrate the usage of this component, with the level of output
    // controlled by the specified 'verbose'.
{
    FILE *input = fopen("star_data1.txt", "r");  // Open input file.
    ASSERT(input);

    list<Star> starList;                         // Define a list of stars.
    ASSERT(starList.empty());                    // A list should be empty
                                                 // after default construction.

    readData(&starList, input);                  // Read input to the list.
    ASSERT(10 == starList.size());               // Verify correct reading.
    fclose(input);                               // Close input file.

    filter(&starList);                           // Pick twinkle stars.
    ASSERT(4 == starList.size());                // Verify correct filter.

    // Print out twinkle stars.
    if (verbose) {
        for (list<Star>::const_iterator i = starList.begin();
                i != starList.end(); ++i) {
            i->write(stdout);
        }
    }
    return 0;
}
//..

///Example 2: Combine Two Star Surveys
///- - - - - - - - - - - - - - - - - -
// Now we want to combine the results from two star surveys into a single list,
// using the same 'Star' class defined in the first usage example.
//
// First, we begin by reading both lists and filtering them.  (Our test data is
// selected so that the second data file contains 8 starts of which 3 are
// sufficiently bright as to pass our filter:
//..
int usageExample2(int verbose)
    // Demonstrate the usage of this component, with the level of output
    // controlled by the specified 'verbose'.
{
    FILE *input = fopen("star_data1.txt", "r");  // Open first input file.
    ASSERT(input);

    list<Star> starList1;                    // Define first star list.
    ASSERT(starList1.empty());

    readData(&starList1, input);             // Read input into list.
    ASSERT(10 == starList1.size());
    fclose(input);                           // Close first input file.

    input = fopen("star_data2.txt", "r");    // Open second input file.
    ASSERT(input);

    list<Star> starList2;                    // Define second list.
    ASSERT(starList2.empty());

    readData(&starList2, input);             // Read input into list.
    ASSERT(8 == starList2.size());
    fclose(input);                           // Close input file.

    filter(&starList1);                      // Pick twinkle stars from the
                                             // first star list.
    ASSERT(4 == starList1.size());

    filter(&starList2);                      // Pick twinkle stars from the
                                             // second star list.
    ASSERT(3 == starList2.size());

// Then, we combine the two lists, 'starList1' and 'starList2'.  One way to do
// this is to simply insert the second list at the end of the first:
//..
    list<Star> tmp1(starList1);  // Make a copy of the first list
    list<Star> tmp2(starList2);  // Make a copy of the second list
    tmp1.insert(tmp1.end(), tmp2.begin(), tmp2.end());
    ASSERT(7 == tmp1.size());    // Verify combined size.
    ASSERT(3 == tmp2.size());    // 'tmp2' should be unchanged.
//..
// Next, let's have a closer look of the above code and see if we can improve
// the combination performance.  The above 'insert' method appends a copy of
// each element in 'tmp2' onto the end of 'tmp1'.  This copy is unnecessary
// because we have no need for 'tmp2' after the lists have been combined.  A
// faster and less-memory-intensive technique is to use the 'splice' function,
// which *moves* rather than *copies* elements from one list to another:
//..
    tmp1 = starList1;
    tmp2 = starList2;
    tmp1.splice(tmp1.begin(), tmp2);
    ASSERT(7 == tmp1.size());    // Verify combined size.
    ASSERT(0 == tmp2.size());    // 'tmp2' should be emptied by the splice.
//..
// Notice that, while the original lists were sorted in ascending order
// (because the data files were originally sorted), the combined list is no
// longer sorted.  To fix it, we sort 'tmp1' using the 'sort' member function:
//..
    tmp1.sort();
//..
// Then, we suggest a third, and also the best approach to combine two lists,
// which is to take advantage of the fact that the lists were originally
// sorted, using the 'merge' function:
//..
    starList1.merge(starList2);     // Merge 'starList2' into 'starList1'.
    ASSERT(7 == starList1.size());  // Verify combined size.
    ASSERT(0 == starList2.size());  // starList2 should be emptied by the
                                    // merge.
//..
// Now, since the two star surveys may overlap, we want to eliminate
// duplicates.  We accomplish this by using the 'unique' member function:
//..
    starList1.unique();             // Eliminate duplicates in 'starList1'.
    ASSERT(6 == starList1.size());  // Verify size after elimination.
//..
// Finally, we print the result:
//..
    if (verbose) {
        for (list<Star>::const_iterator i = starList1.begin();
             i != starList1.end(); ++i) {
            i->write(stdout);
        }
    }
    return 0;
}
//..

//=============================================================================
//                          TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
struct TestDriver {
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters '[A .. E]' correspond to arbitrary (but unique) char
    // values to be appended to the 'bsl::list<T>' object.  A tilde ('~')
    // indicates that the logical (but not necessarily physical) state of the
    // object is to be set to its initial, empty state (via the 'clear'
    // method).
    //
    // LANGUAGE SPECIFICATION:
    // -----------------------
    //
    // <SPEC>       ::= <EMPTY>   | <LIST>
    //
    // <EMPTY>      ::=
    //
    // <LIST>       ::= <ITEM>    | <ITEM><LIST>
    //
    // <ITEM>       ::= <ELEMENT> | <CLEAR>
    //
    // <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'H'
    //                                      // unique but otherwise arbitrary
    // <CLEAR>      ::= '~'
    //
    // Spec String  Description
    // -----------  -----------------------------------------------------------
    // ""           Has no effect; leaves the object empty.
    // "A"          Append the value corresponding to A.
    // "AA"         Append two values both corresponding to A.
    // "ABC"        Append three values corresponding to A, B and C.
    // "ABC~"       Append three values corresponding to A, B and C and then
    //              remove all the elements (set array length to 0).  Note that
    //              this spec yields an object that is logically equivalent
    //              (but not necessarily identical internally) to one
    //              yielded by ("").
    // "ABC~DE"     Append three values corresponding to A, B, and C; empty
    //              the object; and append values corresponding to D and E.
    // ------------------------------------------------------------------------

    // TYPES
    typedef list<TYPE,ALLOC>                      Obj;

    // Shorthands
    typedef typename Obj::iterator                iterator;
    typedef typename Obj::const_iterator          const_iterator;
    typedef typename Obj::reverse_iterator        reverse_iterator;
    typedef typename Obj::const_reverse_iterator  const_reverse_iterator;

    typedef bslmf::MovableRefUtil                 MoveUtil;
    typedef bsltf::MoveState                      MoveState;
    typedef bsltf::TestValuesArray<TYPE>          TestValues;

    typedef typename
        bsl::is_convertible<bslma::Allocator*,ALLOC>::type ObjHasBslmaAlloc;
        // true_type if ALLOC is a 'bslma::Allocator' type

    typedef typename bslma::UsesBslmaAllocator<TYPE>::type TypeUsesBslmaAlloc;
        // true_type if TYPE uses 'bslma::Allocator'.

    typedef typename bsl::uses_allocator<TYPE, ALLOC>::type TypeUsesStdAlloc;
        // true_type if TYPE uses standard allocator.

    enum { SCOPED_ALLOC = (ObjHasBslmaAlloc::value &&
                                                     TypeUsesBslmaAlloc::value)
                          || TypeUsesStdAlloc::value};
        // true if both the container shares its allocator with its contained
        // elements.

    struct VPred {
        // Unary predicate matching elements of a specified value.  Note that
        // this object does not store a copy of the value to be matched, it
        // just stores reference to an object that must outlive the use of this
        // predicate.

        const TYPE& d_match;
        explicit VPred(const TYPE& v)
        : d_match(v)
            // Create a value predicate object use to match against the
            // specified 'v'.
        {}

        bool operator()(TYPE& x) const
            // Return 'true' if the specified 'x' matches the object referred
            // to by this predicate, and 'false' otherwise.
        {
            return x == d_match;
        }
    };

    // Binary predicate returning true if the arguments values, expressed as
    // integers, have the same low bit value.  Thus, 'A' and 'C' compare equal;
    // 'B' and 'D' compare equal.  If an allocator is supplied to the predicate
    // constructor, then each call will cause an allocate/deallocate sequence,
    // which might throw an exception and is thus useful for exception testing.

    struct LowBitEQ {
        bslma::Allocator* d_alloc_p;

        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(LowBitEQ, bslma::UsesBslmaAllocator);

        explicit LowBitEQ(bslma::Allocator* a = 0)
        : d_alloc_p(a)
            // Create a 'LowBitEq' object using the optionally specified
            // allocator 'a'.
        {}

        LowBitEQ(const LowBitEQ& original, bslma::Allocator *a = 0)
        : d_alloc_p(a)
            // Create a copy of the specified 'original' using the optionally
            // specified allocator 'a'.
        {
            (void) original;
        }

        bool operator()(const TYPE& a, const TYPE& b) const
            // Do an allocation, then return 'true' if the lowest-order bits of
            // the values of the specified 'a' and the specified 'b' match and
            // 'false' otherwise.
        {
            if (d_alloc_p) {
                void* p = d_alloc_p->allocate(1);
                d_alloc_p->deallocate(p);
            }
            int a1 = value_of(a);
            int b1 = value_of(b);
            return 0 == ((a1 ^ b1) & 1);
        }
    };

    // Binary predicate returning true if b < a

    class GreaterThan : private std::binary_function<TYPE, TYPE, bool> {
        int *d_count_p;              // Pointer to count of times invoked
        int *d_invocationLimit_p;    // Number of invocations before throwing

      public:
        explicit
        GreaterThan(int *count = 0) : d_count_p(count)
            // Create a 'GreaterThan' functor object that will optionally keep
            // track of the number of times it is invoked in the optionally
            // specified '*count'.
        {
            d_invocationLimit_p = 0;
        }

        void setInvocationLimit(int *limit)
            // Change the variable controlling the invocation limit to the one
            // referred by the specified 'limit'.
        {
            d_invocationLimit_p = limit;
        }

        bool operator()(const TYPE& a, const TYPE& b)
            // Return 'true' if the value of the specified 'a' is greater than
            // that of the specified 'b' and 'false' otherwise.  Keep a count
            // of invocations if a count is being kept, and throw when the
            // invocation limit reaches 0.
        {
            if (d_count_p) ++*d_count_p;
#ifdef BDE_BUILD_TARGET_EXC
            if (d_invocationLimit_p) {
                if (0 == *d_invocationLimit_p) throw --*d_invocationLimit_p;
                else if (0 < *d_invocationLimit_p) --*d_invocationLimit_p;
            }
#endif // BDE_BUILD_TARGET_EXC
            return value_of(b) < value_of(a);
        }
    };

                            // ======================
                            // class ExceptionProctor
                            // ======================

    class ExceptionProctor {
        // This scoped proctor helps to verify the full guarantee of rollback
        // in exception-throwing code.

        // DATA
        int  d_lineNum;
        Obj  d_initialState;
        Obj *d_object_p;

      public:
        // CREATORS
        ExceptionProctor(Obj *object, int line);
            // Copy the initial state of the specified '*object' (without using
            // 'Obj's or 'TYPE's copy c'tors), and record the specified line
            // 'line'.

        ~ExceptionProctor();
            // Assert that the value of the object specified at construction is
            // matches the value it had when this object was constructed.

        // MANIPULATORS
        void release();
            // Release this object so that it will no longer track the value of
            // the monitored object at destruction.
    };

    struct HasOperatorLessThan : bsl::integral_constant<bool,
                                            bsl::is_fundamental<TYPE>::value
                                         || bsl::is_enum<TYPE>::value
                                         || bsl::is_pointer<TYPE>::value
                                         || bsl::is_same<TYPE, T>::value
                                         || bsl::is_same<TYPE, TNA>::value> {};

    struct IsMoveAware : bsl::integral_constant<bool,
                   bsl::is_same<TYPE, bsltf::MovableTestType>::value
                || bsl::is_same<TYPE, bsltf::MovableAllocTestType>::value
                || bsl::is_same<TYPE, bsltf::MoveOnlyAllocTestType>::value> {};

#if defined(BSLS_PLATFORM_OS_AIX) || defined(BSLS_PLATFORM_OS_WINDOWS)
    // Aix has a compiler bug where method pointers do not default construct
    // to 0.  Windows has the same problem.

    enum { k_IS_DEFAULT_CONSTRUCTIBLE =
          !bsl::is_same<TYPE, TstFacility::MethodPtr>::value };
#else
    enum { k_IS_DEFAULT_CONSTRUCTIBLE = true };
#endif

    // TEST APPARATUS

    static bool checkIntegrity(const Obj& object, Int64 length);
        // Check the integrity of the specified 'object' by verifying that
        // iterating over the list both forwards and backwards yields the
        // specified 'length' positions and that 'object.size()' equals
        // 'length'.  This simple test should catch most instances of data
        // structure corruption in a doubly-linked-list implementation of list.

    static Int64 deltaBlocks(Int64 n);
        // Return the change in the number of blocks used by a list whose
        // length has changed by a specified 'n' elements.  Note: 'n' may be
        // negative.

    static Int64 expectedBlocks(Int64 n);
        // Return the number of blocks expected to be used by a list of
        // specified length 'n'.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified '*object' with its value
        // adjusted according to the specified 'spec'.

    static int ggg(Obj *object, const char *spec, int verboseFlag = 1);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'push_back' and
        // white-box manipulator 'clear'.  Optionally specify a zero
        // 'verboseFlag' to suppress 'spec' syntax error messages.  Return the
        // index of the first invalid character, and a negative value
        // otherwise.  Note that this function is used to implement 'gg' as
        // well as allow for verification of syntax error detection.

    static Int64 numMovedFrom(const Obj& x);
        // Return the number of elements in the specified 'x' that have been
        // moved from.

    static Int64 numMovedInto(const Obj& x);
        // Return the number of elements in the specified 'x' that have been
        // moved into.

    static void primaryCopier(Obj *container, const TYPE& element);
        // Append an element to the specified '*container' whose value matches
        // than of the specified 'element', without using 'TYPE's copy c'tor.
        // Note that this assumes 'value_of(element)' is defined.

    static void primaryManipulator(Obj *container, int identifier);
        // Append to the specified 'container' the value object indicated by
        // the specified 'identifier', ensuring that the overload of the
        // primary manipulator taking a modifiable rvalue is invoked (rather
        // than the one taking an lvalue).  Return the result of invoking the
        // primary manipulator.

    // TEST CASES
    static void test01_breathingTest();
        // Breathing test.  This test *exercises* basic functionality but
        // *test* nothing.

    static void test02_primaryManipulators();
        // Test primary manipulators ('push_back' and 'clear').

    static void test03_generatorGG();
        // Test generator functions 'ggg' and 'gg'.

    static void test04_basicAccessors();
        // Test basic accessors ('size', 'begin' and 'end').

    static void test06_equalityOp();
        // Test equality operator ('operator==').

    template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
              bool OTHER_FLAGS>
    static void test07_select_on_container_copy_construction_dispatch();
    static void test07_select_on_container_copy_construction();
        // Test 'select_on_container_copy_construction'.

    static void test07_copyCtor();
        // Test copy constructor.

    template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void test09_propagate_on_container_copy_assignment_dispatch();
    static void test09_propagate_on_container_copy_assignment();
        // Test 'propagate_on_container_copy_assignment'.

    static void test09_copyAssignmentOp();
        // Test assignment operator ('operator=').

    static void test11_allocators();
    template <class LHS_TYPE, class RHS_TYPE>
    static void test11_allocators(LHS_TYPE, RHS_TYPE);
    static void test11_allocators(bsl::true_type, bsl::true_type);
        // The first two-argument overload is a no-op.  The second two-argument
        // overload is called only when both 'Obj' and 'TYPE' use
        // 'bslma::Allocator'.  The first overload, with no arguments,
        // dispatches to the other two overloads.
};

template <class TYPE>
class StdBslmaTestDriver : public TestDriver<TYPE,
                            bsltf::StdAllocatorAdaptor<bsl::allocator<TYPE> > >
{
};

                        // ----------------------------------
                        // class TestDriver::ExceptionProctor
                        // ----------------------------------

// CREATORS
template <class TYPE, class ALLOC>
TestDriver<TYPE,ALLOC>::ExceptionProctor::ExceptionProctor(Obj *object,
                                                           int  line)
: d_lineNum(line)
, d_initialState()
, d_object_p(object)
{
    typedef typename Obj::const_iterator CIt;

    const CIt end = object->end();

    for (CIt it = object->begin(); end != it; ++it) {
        primaryCopier(&d_initialState, *it);
    }
}

template <class TYPE, class ALLOC>
TestDriver<TYPE,ALLOC>::ExceptionProctor::~ExceptionProctor()
{
    if (d_object_p) {
        const int LINE = d_lineNum;
        ASSERTV(LINE, d_initialState == *d_object_p);
    }
}

// MANIPULATORS
template <class TYPE, class ALLOC>
inline
void TestDriver<TYPE,ALLOC>::ExceptionProctor::release()
{
    d_object_p = 0;
}

                               // ----------------
                               // class TestDriver
                               // ----------------

template <class TYPE, class ALLOC>
bool TestDriver<TYPE,ALLOC>::checkIntegrity(const Obj& object, Int64 length)
{
    const const_iterator start  = object.begin();
    const const_iterator finish = object.end();
    const_iterator       it;
    int                  count = 0;
    static const int     MAX_SAVE_ITERS = 20;
    const_iterator       save_iters[MAX_SAVE_ITERS];
    static int           DEFAULT_IVALUE;
    static bool          firstTime = true;

    if (firstTime) {
        firstTime = false;

        // Note that aix gives warnings for 'const TYPE& X = TYPE()' for any
        // type that has a private copy c'tor.  This will work for the set of
        // 'TYPE's we actually test.

        TYPE DEFAULT_VALUE;
        if (bsl::is_trivially_default_constructible<TYPE>::value) {
            new (bsls::Util::addressOf(DEFAULT_VALUE)) TYPE();
        }
        DEFAULT_IVALUE = k_IS_DEFAULT_CONSTRUCTIBLE ? value_of(DEFAULT_VALUE)
                                                    : -1;
    }

    bool ret;

    // Iterate over the list.  Terminate the loop at the shorter of 'it ==
    // finish' or 'count == length'.  These should be the same, but data
    // structure corruption such as circular links or skipped nodes could make
    // them different.

    for (it = start; it != finish && count < length; ++it, ++count) {
        // Dereference the iterator and verify that the value is within the
        // expected range.

        if (k_IS_DEFAULT_CONSTRUCTIBLE) {
            const int v = value_of(*it);
            ret = (v >= VA && v <= VH) || DEFAULT_IVALUE == v;
            ASSERTV(NameOf<TYPE>(), v, VA, VH, DEFAULT_VALUE, true == ret);
            if (!ret) {
                return false;                                         // RETURN
            }
        }
        if (count < MAX_SAVE_ITERS) {
            save_iters[count] = it;
        }
    }

    // Verify that 'count' reached 'length' at the same time that 'it' reached
    // 'finish'

    ret = (it == finish) && (count == length);
    ASSERTV(ret, true == ret);
    if (!ret) {
        return false;                                                 // RETURN
    }

    // Iterate over the list in reverse.  Verify that we see the same iterator
    // values on this traversal as we did in the forward direction.

    while (it != start && count > 0) {
        --it;
        --count;
        ret = (count >= MAX_SAVE_ITERS) || (it == save_iters[count]);
        ASSERTV(ret, true == ret);
        if (!ret) {
            return false;                                             // RETURN
        }
    }

    ret = (it == start) && (0 == count);
    ASSERTV(ret, true == ret);
    if (!ret) {
        return false;                                                 // RETURN
    }

    // If got here, then the only integrity test left is to verify that size()
    // returns the actual length of the list.

    ret = (length == static_cast<Int64>(object.size()));
    ASSERTV(ret, true == ret);
    return ret;
}

template <class TYPE, class ALLOC>
inline
Int64 TestDriver<TYPE,ALLOC>::deltaBlocks(Int64 n)
{
    // One block per element plus one additional block per element if the
    // element uses the list's allocator ('SCOPED_ALLOC' == 1).
    return n + n * SCOPED_ALLOC;
}

template <class TYPE, class ALLOC>
inline
Int64 TestDriver<TYPE,ALLOC>::expectedBlocks(Int64 n)
{
    // One block for the sentinel node + block allocations.
    return 1 + deltaBlocks(n);
}

template <class TYPE, class ALLOC>
list<TYPE,ALLOC>& TestDriver<TYPE,ALLOC>::gg(Obj        *object,
                                             const char *spec)
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

template <class TYPE, class ALLOC>
int TestDriver<TYPE,ALLOC>::ggg(Obj        *object,
                                const char *spec,
                                int         verboseFlag)
{
    enum { SUCCESS = -1 };

    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'H') {
            primaryManipulator(object, spec[i]);
        }
        else if ('~' == spec[i]) {
            object->clear();
        }
        else {
            if (verboseFlag) {
                printf("Error, bad character ('%c') "
                       "in spec \"%s\" at position %d.\n", spec[i], spec, i);
            }
            return i;  // Discontinue processing this spec.           // RETURN
        }
   }
   return SUCCESS;
}

template <class TYPE, class ALLOC>
Int64 TestDriver<TYPE,ALLOC>::numMovedFrom(const Obj& x)
{
    Int64                ret = 0;
    const const_iterator itEnd = x.end();

    for  (const_iterator it = x.begin(); itEnd != it; ++it) {
        ret += MoveState::e_MOVED == bsltf::getMovedFrom(*it);
    }

    return ret;
}

template <class TYPE, class ALLOC>
Int64 TestDriver<TYPE,ALLOC>::numMovedInto(const Obj& x)
{
    Int64                ret = 0;
    const const_iterator itEnd = x.end();

    for  (const_iterator it = x.begin(); itEnd != it; ++it) {
        ret += MoveState::e_MOVED == bsltf::getMovedInto(*it);
    }

    return ret;
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::primaryCopier(Obj         *container,
                                           const TYPE&  element)
{
    primaryManipulator(container, value_of(element));
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE, ALLOC>::primaryManipulator(Obj *container,
                                                 int  identifier)
{
    bslma::TestAllocator scratch;
    ALLOC                xscratch(&scratch);

    bsls::ObjectBuffer<TYPE> buffer;
    TstFacility::emplace(buffer.address(), identifier, xscratch);
    bslma::DestructorGuard<TYPE> guard(buffer.address());

    container->push_back(MoveUtil::move(buffer.object()));
}

                                 // ----------
                                 // TEST CASES
                                 // ----------

template <class TYPE, class ALLOC>
template <class LHS_TYPE, class RHS_TYPE>
void TestDriver<TYPE,ALLOC>::test11_allocators(LHS_TYPE, RHS_TYPE)
{
    BSLMF_ASSERT(!ObjHasBslmaAlloc::value || !TypeUsesBslmaAlloc::value);

    if (verbose) printf("testAllocators: not testing\n");

    // no-op
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::test11_allocators(bsl::true_type, bsl::true_type)
{
    // ------------------------------------------------------------------------
    // TEST ALLOCATOR-RELATED CONCERNS
    //
    // Concerns:
    //: o For ALLOC that is a bslma_Allocator
    //:   1 The list class has the 'bslma::UsesBslmaAllocator' trait.
    //:   2 The allocator is not copied when the list is copy-constructed.
    //:   3 The allocator is set with the extended copy-constructor.
    //:   4 The allocator is passed through to elements if the elements also
    //:     use bslma_Allocator.
    //:   5 Creating an empty list allocates exactly one block.
    //
    // Plan:
    //: 1 Default construct a list and observe that a single allocation takes
    //:   place (for the sentinel).
    //:
    //: 2 If the type contained in the list allocates, push a value onto the
    //:   back of the list and observe that 2 more allocations take place.
    //:
    //: 3 Call the 'allocator' accessor on an element in the list and observe
    //:   that the allocator matches that passed to the list.
    //
    // Testing:
    //   Allocator traits and propagation
    // ------------------------------------------------------------------------

    if (verbose) printf("testAllocators: testing\n");

    // Compile-time assert that this is the correct specialization.
    BSLMF_ASSERT(ObjHasBslmaAlloc::value);
    BSLMF_ASSERT(TypeUsesBslmaAlloc::value);

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    if (verbose)
        printf("Testing 'bslma::UsesBslmaAllocator'.\n");

    if (verbose)
        printf("Testing that empty list allocates one block.\n");
    {
        const Int64 BB = oa.numBlocksTotal();
        Obj         mX(&oa);

        ASSERTV(BB + 1 == oa.numBlocksTotal());
        ASSERTV(1 == oa.numBlocksInUse());
    }

    if (verbose)
        printf("Testing allocator propagation for copy construction.\n");
    {
        // Normal copy constructor
        Obj mX(&oa); const Obj& X = mX;
        Obj mY(X);   const Obj& Y = mY;

        // Allocator not copied
        ASSERTV(&oa == X.get_allocator());
        ASSERTV(&oa != Y.get_allocator());

        // Extended copy constructor
        bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

        Obj mZ(X,&oa2); const Obj& Z = mZ;

        ASSERTV(&oa2 == Z.get_allocator());
    }

    if (verbose)
        printf("Testing passing allocator through to elements.\n");

    TestValues VALUES("A");

    const Int64 DD = OtherAllocatorDefaultImp.numBlocksInUse();
    if (bslma::UsesBslmaAllocator<TYPE>::value)
    {
        {
            Obj mX(1, VALUES[0], &oa); const Obj& X = mX;

            ASSERTV(&oa == X.back().allocator());
            ASSERTV(3 == oa.numBlocksInUse());
        }
        {
            Obj mX(&oa);  const Obj& X = mX;
            mX.push_back(VALUES[0]);
            ASSERTV(&oa == X.back().allocator());
            ASSERTV(3 == oa.numBlocksInUse());
        }
    }
    else
    {
        {
            Obj mX(1, VALUES[0], &oa); const Obj& X = mX;

            bslma::TestAllocator *const elemAlloc =
                     dynamic_cast<bslma::TestAllocator*>(X.back().allocator());
            ASSERTV(&oa != elemAlloc);
            ASSERTV(2 == oa.numBlocksInUse());
            ASSERTV(&OtherAllocatorDefaultImp == elemAlloc);
            ASSERTV(DD + 1 == elemAlloc->numBlocksInUse());
        }
        {
            Obj mX(&oa);  const Obj& X = mX;
            mX.push_back(VALUES[0]);
            bslma::TestAllocator *const elemAlloc =
                dynamic_cast<bslma::TestAllocator*>(X.back().allocator());
            ASSERTV(&oa != elemAlloc);
            ASSERTV(2 == oa.numBlocksInUse());
            ASSERTV(&OtherAllocatorDefaultImp == elemAlloc);
            ASSERTV(DD + 1 == elemAlloc->numBlocksInUse());
        }
    }

    ASSERTV(0 == oa.numBytesInUse());
    ASSERTV(DD == OtherAllocatorDefaultImp.numBytesInUse());
}

#if 0
// TBD: this is meant to test objects using allocators other than bslma
// ones.  Perhaps this code can be resurrected once we have updated bsltf
// appropriately, though probably not.

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::test11_allocators(bsl::false_type)
{
    // ------------------------------------------------------------------------
    // TEST ALLOCATOR-RELATED CONCERNS FOR NON-BSLMA ALLOCATORS
    //
    // This template specialization is for containers that use non-bslma
    // Allocators.
    //
    // Concerns:
    //   1. The list class does not have the
    //      'bslma::UsesBslmaAllocator' trait.
    //   2. The allocator is not passed through to elements
    //   3. The allocator is set with the extended copy-constructor.
    //   4. The allocator is copied when the list is copy-constructed.
    //   5. Creating an empty list allocates exactly one block.
    //   6. 'get_allocator' returns the allocator used to construct the
    //      list object.
    //
    // Testing:
    //   allocator_type get_allocator() const;
    //   Allocator traits
    //   Allocator propagation
    //   Allocator traits and propagation
    // ------------------------------------------------------------------------

    // Compile-time assert that this is the correct specialization.
    BSLMF_ASSERT( !ObjHasBslmaAlloc::value );

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    OtherAllocator<TYPE> objAllocator(&testAllocator);

    TestValues VALUES("ABCDEFGH");

    if (verbose)
        printf("Testing 'bslma::UsesBslmaAllocator'.\n");

    ASSERTV(! bslma::UsesBslmaAllocator<Obj>::value);

    if (verbose)
        printf("Testing that empty list allocates one block.\n");
    {
        const Int64 BB = testAllocator.numBlocksTotal();
        Obj         mX(objAllocator);
        ASSERTV(BB + 1 == testAllocator.numBlocksTotal());
        ASSERTV(1 == testAllocator.numBlocksInUse());
    }

    if (verbose)
        printf("Testing that allocator propagation for "
               "copy construction.\n");
    {
        // Normal copy constructor
        Obj mX(objAllocator);    const Obj& X = mX;
        Obj mY(X);               const Obj& Y = mY;

        // Allocator copied
        ASSERTV(objAllocator == X.get_allocator());
        ASSERTV(objAllocator == Y.get_allocator());

        // Extended copy constructor
        bslma::TestAllocator a2;
        OtherAllocator<char> objAlloc2(&a2);

        Obj mZ(X,objAlloc2);     const Obj& Z = mZ;

        // Allocator set to objAlloc2 (not copied)
        ASSERTV(objAllocator   != Z.get_allocator());
        ASSERTV(objAlloc2      == Z.get_allocator());
    }

    if (verbose)
       printf("Testing that allocator is not passed through to elements.\n");

    const Int64 DD = OtherAllocatorDefaultImp.numBlocksInUse();
    if (bslma::UsesBslmaAllocator<TYPE>::value)
    {
        // Elements in container should use default allocator while the
        // container itself uses 'testAllocator'.  Set the default allocator
        // here so that we can track its use.
        bslma::TestAllocator         defAlloc(veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&defAlloc);

        {
            Obj mX(1, VALUES[0], objAllocator);  const Obj& X = mX;

            bslma::TestAllocator *const elemAlloc =
                dynamic_cast<bslma::TestAllocator*>(X.back().allocator());
            ASSERTV(objAllocator == X.get_allocator());
            ASSERTV(&testAllocator != elemAlloc);
            ASSERTV(2 == testAllocator.numBlocksInUse());
            ASSERTV(&defAlloc == elemAlloc);
            ASSERTV(1 == elemAlloc->numBlocksInUse());
        }
        {
            Obj mX(objAllocator);  const Obj& X = mX;
            mX.push_back(VALUES[0]);
            bslma::TestAllocator *const elemAlloc =
                dynamic_cast<bslma::TestAllocator*>(X.back().allocator());
            ASSERTV(objAllocator == X.get_allocator());
            ASSERTV(&testAllocator != elemAlloc);
            ASSERTV(2 == testAllocator.numBlocksInUse());
            ASSERTV(&defAlloc == elemAlloc);
            ASSERTV(1 == elemAlloc->numBlocksInUse());
        }

        ASSERTV(0 == defAlloc.numBlocksInUse());
    }
    else
    {
        // Elements in container should use 'OtherAllocator::d_defaultImp'
        // while the container itself uses 'testAllocator'.

        {
            Obj mX(1, VALUES[0], objAllocator);  const Obj& X = mX;

            bslma::TestAllocator *const elemAlloc =
                dynamic_cast<bslma::TestAllocator*>(X.back().allocator());
            ASSERTV(objAllocator == X.get_allocator());
            ASSERTV(&testAllocator != elemAlloc);
            ASSERTV(2 == testAllocator.numBlocksInUse());
            ASSERTV(&OtherAllocatorDefaultImp == elemAlloc);
            ASSERTV(DD + 1 == elemAlloc->numBlocksInUse());
        }
        {
            Obj mX(objAllocator);  const Obj& X = mX;
            mX.push_back(VALUES[0]);
            bslma::TestAllocator *const elemAlloc =
                dynamic_cast<bslma::TestAllocator*>(X.back().allocator());
            ASSERTV(objAllocator == X.get_allocator());
            ASSERTV(&testAllocator != elemAlloc);
            ASSERTV(2 == testAllocator.numBlocksInUse());
            ASSERTV(&OtherAllocatorDefaultImp == elemAlloc);
            ASSERTV(DD + 1 == elemAlloc->numBlocksInUse());
        }
    }

    ASSERTV(0 == testAllocator.numBytesInUse());
    ASSERTV(DD == OtherAllocatorDefaultImp.numBytesInUse());
}
#endif

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::test11_allocators()
{
    if (verbose) {
        printf("testAllocators: Obj has bslma: %d, TYPE uses bslma: %d\n",
                           ObjHasBslmaAlloc::value, TypeUsesBslmaAlloc::value);
    }

    // Dispatch to the appropriate function
    test11_allocators(ObjHasBslmaAlloc(), TypeUsesBslmaAlloc());
}

template <class TYPE, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver<TYPE, ALLOC>::
                       test09_propagate_on_container_copy_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   TYPE,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS> StdAlloc;

    typedef bsl::list<TYPE, StdAlloc>           Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };
    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    // Create control and source objects.
    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char *const ISPEC   = SPECS[ti];

        TestValues IVALUES(ISPEC);

        bslma::TestAllocator oas("source", veryVeryVeryVerbose);
        bslma::TestAllocator oat("target", veryVeryVeryVerbose);

        StdAlloc mas(&oas);
        StdAlloc mat(&oat);

        StdAlloc scratch(&da);

        const Obj W(IVALUES.begin(), IVALUES.end(), scratch);  // control

        // Create target object.
        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            {
                IVALUES.resetIterators();

                Obj        mY(IVALUES.begin(), IVALUES.end(), mas);
                const Obj& Y = mY;

                if (veryVerbose) { T_ P_(ISPEC) P_(Y) P(W) }

                Obj        mX(JVALUES.begin(), JVALUES.end(), mat);
                const Obj& X = mX;

                bslma::TestAllocatorMonitor oasm(&oas);
                bslma::TestAllocatorMonitor oatm(&oat);

                Obj *mR = &(mX = Y);

                ASSERTV(ISPEC, JSPEC,  W,   X,  W == X);
                ASSERTV(ISPEC, JSPEC,  W,   Y,  W == Y);
                ASSERTV(ISPEC, JSPEC, mR, &mX, mR == &mX);

                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                       !PROPAGATE == (mat == X.get_allocator()));
                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                        PROPAGATE == (mas == X.get_allocator()));

                ASSERTV(ISPEC, JSPEC, mas == Y.get_allocator());

                if (PROPAGATE) {
                    ASSERTV(ISPEC, JSPEC, 0 == oat.numBlocksInUse());
                }
                else {
                    ASSERTV(ISPEC, JSPEC, oasm.isInUseSame());
                }
            }
            ASSERTV(ISPEC, 0 == oas.numBlocksInUse());
            ASSERTV(ISPEC, 0 == oat.numBlocksInUse());
        }
    }
    ASSERTV(0 == da.numBlocksInUse());
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE, ALLOC>::test09_propagate_on_container_copy_assignment()
{
    // ------------------------------------------------------------------------
    // COPY-ASSIGNMENT OPERATOR: ALLOCATOR PROPAGATION
    //
    // Concerns:
    //: 1 If the 'propagate_on_container_copy_assignment' trait is 'false', the
    //:   allocator used by the target object remains unchanged (i.e., the
    //:   source object's allocator is *not* propagated).
    //:
    //: 2 If the 'propagate_on_container_copy_assignment' trait is 'true', the
    //:   allocator used by the target object is updated to be a copy of that
    //:   used by the source object (i.e., the source object's allocator *is*
    //:   propagated).
    //:
    //: 3 The allocator used by the source object remains unchanged whether or
    //;   not it is propagated to the target object.
    //:
    //: 4 If the allocator is propagated from the source object to the target
    //:   object, all memory allocated from the target object's original
    //:   allocator is released.
    //:
    //: 5 The effect of the 'propagate_on_container_copy_assignment' trait is
    //:   independent of the other three allocator propagation traits.
    //
    // Plan:
    //: 1 Specify a set S of object values with varied differences, ordered by
    //:   increasing length, to be used in the following tests.
    //:
    //: 2 Create two 'bsltf::StdStatefulAllocator' objects with their
    //:   'propagate_on_container_copy_assignment' property configured to
    //:   'false'.  In two successive iterations of P-3, first configure the
    //:   three properties not under test to be 'false', then configure them
    //:   all to be 'true'.
    //:
    //: 3 For each value '(x, y)' in the cross product S x S:  (C-1)
    //:
    //:   1 Initialize an object 'X' from 'x' using one of the allocators from
    //:     P-2.
    //:
    //:   2 Initialize two objects from 'y', a control object 'W' using a
    //:     scratch allocator and an object 'Y' using the other allocator from
    //:     P-2.
    //:
    //:   3 Copy-assign 'Y' to 'X' and use 'operator==' to verify that both
    //:     'X' and 'Y' subsequently have the same value as 'W'.
    //:
    //:   4 Use the 'get_allocator' method to verify that the allocator of 'Y'
    //:     is *not* propagated to 'X' and that the allocator used by 'Y'
    //:     remains unchanged.  (C-1)
    //:
    //: 4 Repeat P-2..3 except that this time configure the allocator property
    //:   under test to 'true' and verify that the allocator of 'Y' *is*
    //:   propagated to 'X'.  Also verify that all memory is released to the
    //:   allocator that was in use by 'X' prior to the assignment.  (C-2..5)
    //
    // Testing:
    //   propagate_on_container_copy_assignment
    // ------------------------------------------------------------------------

    if (verbose) printf("\nCOPY-ASSIGNMENT OPERATOR: ALLOCATOR PROPAGATION"
                        "\n===============================================\n");

    if (verbose)
        printf("\n'propagate_on_container_copy_assignment::value == false'\n");

    test09_propagate_on_container_copy_assignment_dispatch<false, false>();
    test09_propagate_on_container_copy_assignment_dispatch<false, true>();

    if (verbose)
        printf("\n'propagate_on_container_copy_assignment::value == true'\n");

    test09_propagate_on_container_copy_assignment_dispatch<true, false>();
    test09_propagate_on_container_copy_assignment_dispatch<true, true>();
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::test09_copyAssignmentOp()
{
    // ------------------------------------------------------------------------
    // TESTING COPY ASSIGNMENT OPERATOR
    //
    // Concerns:
    //: 1 The 'rhs' value must not be affected by the operation.
    //:
    //: 2 'rhs' going out of scope has no effect on the value of 'lhs' after
    //:   the assignment.
    //:
    //: 3 After the assignment, no element of the 'lhs' has the same address an
    //:   element of 'rhs' (except in the case of self-assignment).
    //:
    //: 4 Aliasing (x = x): The assignment operator must always work -- even
    //:   when the lhs and rhs are identically the same object.
    //:
    //: 5 The assignment operator must be neutral with respect to memory
    //:   allocation exceptions.
    //:
    //: 6 The 'lhs' object must allocate all of its internal memory from its
    //:   own allocator, even of 'rhs' uses a different allocator.
    //:
    //: 7 TBD: C++0x move assignment moves the value but not the allocator.
    //:   Memory is allocated only if the 'lhs' and 'rhs' allocators are
    //:   different.
    //
    // Plan:
    //: 1 Specify a set S of unique object values with substantial and varied
    //:   differences, ordered by increasing length.  For each value in S,
    //:   construct an object x along with a sequence of similarly constructed
    //:   duplicates x1, x2, ..., xN.  The elements within each object in S are
    //:   unique so that re-ordering elements cannot preserve equality.
    //:   Attempt to affect every aspect of white-box state by altering each xi
    //:   in a unique way.  Let the union of all such objects be the set T.
    //:
    //: 2 To address concerns 2, 3, and 5, construct tests u = v for all
    //:   '(u, v)' in T X T.  Using canonical controls 'UU' and 'VV', assert
    //:   before the assignment that 'UU == u', 'VV == v', and 'v == u' if and
    //:   only if 'VV == UU'.  After the assignment, assert that 'VV == u',
    //:   'VV == v', and, for grins, that 'v == u'.  Verify that each element
    //:   in v has a different address from the corresponding element in v.
    //:   Let v go out of scope and confirm that 'VV == u'.  All of these tests
    //:   are performed within the 'bslma' exception testing apparatus.  Since
    //:   the execution time is lengthy with exceptions, not every permutation
    //:   is performed when exceptions are tested.  Every permutation is also
    //:   tested separately without exceptions.
    //:
    //: 3 As a separate exercise, we address 4 and 5 by constructing tests
    //:   y = y for all y in T.  Using a canonical control X, we will verify
    //:   that X == y before and after the assignment, again within the bslma
    //:   exception testing apparatus.
    //:
    //: 4 To address concern 5, all these tests are performed on user defined
    //:   types:
    //:   o With allocator, copyable
    //:   o With allocator, movable
    //:   o With allocator, not movable
    //
    // Testing:
    //   list& operator=(const list& rhs);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
    bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);
    ALLOC                xoa1(&oa1);
    ALLOC                xoa2(&oa2);

    int numThrows;

    // Set 'assignOfTypeAllocates' to indicate whether
    // 'TYPE& operator=(const TYPE&)' allocates.

    bool assignOfTypeAllocates;
    {

        Obj mX(xoa1);
        ASSERT(0 > ggg(&mX, "AB"));

        typename Obj::iterator itFront = mX.begin(), itBack = itFront;
        ++itBack;
        ASSERT(mX.end() != itBack && itFront != itBack);
        const Int64 nbt = oa1.numBlocksTotal();
        *itFront = *itBack;
        assignOfTypeAllocates = oa1.numBlocksTotal() > nbt;
    }

    if (verbose)
        printf("Testing 'operator=' with TYPE: %s, assignAllocates: %d\n",
               NameOf<TYPE>().name(), assignOfTypeAllocates);

    // ------------------------------------------------------------------------

    if (veryVerbose) printf("Assign cross product of values "
                            "with varied representations.\n"
                            "Without Exceptions\n");
    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE",
            "DEA",       // Try equal-size assignment of different values
            "DEAB",
            "BACEDEDC",
        };

        enum { NUM_SPECS = sizeof SPECS / sizeof SPECS[0] };

        {
            for (int ui = 0; ui < NUM_SPECS; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const size_t      uLen   = std::strlen(U_SPEC);

                if (veryVerbose) {
                    printf("\tFor lhs objects of length " ZU ":\t", uLen);
                    P(U_SPEC);
                }

                Obj mUU;  const Obj& UU = gg(&mUU, U_SPEC);  // control
                ASSERTV(ui, uLen == UU.size());              // same lengths

                for (int vi = 0; vi < NUM_SPECS; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const size_t      vLen   = std::strlen(V_SPEC);

                    if (veryVeryVerbose) {
                        printf("\t\tFor rhs objects of length " ZU ":\t",
                               vLen);
                        P(V_SPEC);
                    }

                    Obj mVV;  const Obj& VV = gg(&mVV, V_SPEC); // control

                    const bool EXP = (ui == vi); // flag indicating same values

                    {
                        Obj mU(xoa2); const Obj& U = gg(&mU, U_SPEC);
                        Obj mV(xoa1); const Obj& V = gg(&mV, V_SPEC);

                        if (veryVeryVerbose) {
                            printf("\t| "); P_(U); P(V);
                        }

                        ASSERTV(U_SPEC, V_SPEC, UU == U);
                        ASSERTV(U_SPEC, V_SPEC, VV == V);
                        ASSERTV(U_SPEC, V_SPEC, EXP == (V==U));

                        const int    NUM_CTOR_BEFORE = numCopyCtorCalls;
                        const int    NUM_DTOR_BEFORE = numDestructorCalls;
                        const size_t U_LEN_BEFORE = U.size();

                        const Int64 BB1 = oa1.numBlocksTotal();
                        const Int64 B1  = oa1.numBlocksInUse();
                        const Int64 BB2 = oa2.numBlocksTotal();
                        const Int64 B2  = oa2.numBlocksInUse();

                        mU = V; // test assignment here

                        const Int64 AA1 = oa1.numBlocksTotal();
                        const Int64 A1  = oa1.numBlocksInUse();
                        const Int64 AA2 = oa2.numBlocksTotal();
                        const Int64 A2  = oa2.numBlocksInUse();

                        // The assignment may construct as many as V.size()
                        // objects and may destroy as many as U.size() objects,
                        // but could (through the use of element-by-element
                        // assignment) construct and destroy fewer elements.

                        const int NUM_CTOR =
                                          numCopyCtorCalls - NUM_CTOR_BEFORE;
                        const int NUM_DTOR =
                                          numDestructorCalls - NUM_DTOR_BEFORE;
                        ASSERT(NUM_CTOR <= (int)V.size());
                        ASSERT(NUM_DTOR <= (int)U_LEN_BEFORE);

                        ASSERTV(U_SPEC, V_SPEC, checkIntegrity(U, vLen));
                        ASSERTV(U_SPEC, V_SPEC, VV  == U);
                        ASSERTV(U_SPEC, V_SPEC, VV  == V);
                        ASSERTV(U_SPEC, V_SPEC, V   == U);
                        ASSERTV(U_SPEC, V_SPEC, BB1 == AA1);
                        ASSERTV(U_SPEC, V_SPEC, B1  == A1 );
                        ASSERTV(U_SPEC, V_SPEC,
                                               BB2 + deltaBlocks(vLen) >= AA2);

                        ptrdiff_t difference =
                                           static_cast<ptrdiff_t>(vLen - uLen);

                        ASSERTV(U_SPEC, V_SPEC,
                                           B2 + deltaBlocks(difference) == A2);

                        for (const_iterator iu = U.begin(), iv = V.begin();
                             iu != U.end(); ++iu, ++iv) {
                            // Verify that U and V have no elements in common
                            ASSERTV(U_SPEC, V_SPEC, *iu,
                                                  bsls::Util::addressOf(*iv) !=
                                                   bsls::Util::addressOf(*iu));
                        }
                    }

                    // Try move assign with different allocators, should be
                    // the same.

                    {
                        Obj mU(xoa2); const Obj& U = gg(&mU, U_SPEC);
                        Obj mV(xoa1); const Obj& V = gg(&mV, V_SPEC);

                        if (veryVeryVerbose) {
                            printf("\t| "); P_(U); P(V);
                        }

                        ASSERTV(U_SPEC, V_SPEC, UU == U);
                        ASSERTV(U_SPEC, V_SPEC, VV == V);
                        ASSERTV(U_SPEC, V_SPEC, EXP == (V==U));

                        const int    NUM_CTOR_BEFORE = numCopyCtorCalls;
                        const int    NUM_DTOR_BEFORE = numDestructorCalls;
                        const size_t U_LEN_BEFORE    = U.size();

                        const Int64 BB1 = oa1.numBlocksTotal();
                        const Int64 B1  = oa1.numBlocksInUse();
                        const Int64 BB2 = oa2.numBlocksTotal();
                        const Int64 B2  = oa2.numBlocksInUse();

                        ASSERTV(U_SPEC, V_SPEC, VV == V);
                        ASSERTV(U_SPEC, V_SPEC, UU == U);

                        mU = MoveUtil::move(mV); // test assignment here

                        const Int64 AA1 = oa1.numBlocksTotal();
                        const Int64 A1  = oa1.numBlocksInUse();
                        const Int64 AA2 = oa2.numBlocksTotal();
                        const Int64 A2  = oa2.numBlocksInUse();

                        // The assignment may construct as many as V.size()
                        // objects and may destroy as many as U.size() objects,
                        // but could (through the use of element-by-element
                        // assignment) construct and destroy fewer elements.

                        const int NUM_CTOR =
                            numCopyCtorCalls - NUM_CTOR_BEFORE;
                        const int NUM_DTOR =
                            numDestructorCalls - NUM_DTOR_BEFORE;

                        ASSERT(NUM_CTOR <= (int)V.size());
                        ASSERT(NUM_DTOR <= (int)U_LEN_BEFORE);

                        ASSERTV(U_SPEC, V_SPEC, checkIntegrity(U, vLen));
                        ASSERTV(U_SPEC, V_SPEC, VV == U);
                        ASSERTV(U_SPEC, V_SPEC, BB1 == AA1);
                        ASSERTV(U_SPEC, V_SPEC, B1, A1, B1 >= A1 );
                        ASSERTV(U_SPEC, V_SPEC,
                                               BB2 + deltaBlocks(vLen) >= AA2);

                        ptrdiff_t difference =
                                           static_cast<ptrdiff_t>(vLen - uLen);

                        ASSERTV(U_SPEC, V_SPEC,
                                           B2 + deltaBlocks(difference) == A2);

                        for (const_iterator iu = U.begin(), iv = V.begin();
                             iu != U.end(); ++iu, ++iv) {
                            // Verify that U and V have no elements in common
                            ASSERTV(U_SPEC, V_SPEC, *iu,
                                                  bsls::Util::addressOf(*iv) !=
                                                   bsls::Util::addressOf(*iu));
                        }
                    }

                    // Try move assign with the same allocator, should be just
                    // a swap.

                    {
                        Int64 BBB2 = oa2.numBlocksTotal();

                        Obj mU(xoa2); const Obj& U = gg(&mU, U_SPEC);

                        ASSERT(oa2.numBlocksTotal() > BBB2);
                        BBB2 = oa2.numBlocksTotal();

                        Obj mV(xoa2); const Obj& V = gg(&mV, V_SPEC);

                        ASSERT(oa2.numBlocksTotal() > BBB2);

                        if (veryVeryVerbose) {
                            printf("\t| "); P_(U); P(V);
                        }

                        ASSERTV(U_SPEC, V_SPEC, UU == U);
                        ASSERTV(U_SPEC, V_SPEC, VV == V);
                        ASSERTV(U_SPEC, V_SPEC, EXP == (V==U));

                        const int NUM_CTOR_BEFORE = numCopyCtorCalls;
                        const int NUM_DTOR_BEFORE = numDestructorCalls;

                        typename Obj::iterator itU = mU.begin();
                        typename Obj::iterator itV = mV.begin();

                        const Int64 BB2 = oa2.numBlocksTotal();
                        const Int64 B2  = oa2.numBlocksInUse();

                        mU = MoveUtil::move(mV); // test assignment here

                        const Int64 AA2 = oa2.numBlocksTotal();
                        const Int64 A2  = oa2.numBlocksInUse();

                        ASSERT(NUM_CTOR_BEFORE == numCopyCtorCalls);
                        ASSERT(NUM_DTOR_BEFORE == numDestructorCalls);

                        ASSERT(mU.begin() == itV);
                        ASSERT(mV.begin() == itU);

                        ASSERTV(U_SPEC, V_SPEC, checkIntegrity(U, vLen));
                        ASSERTV(U_SPEC, V_SPEC, checkIntegrity(V, uLen));
                        ASSERTV(U_SPEC, V_SPEC, VV  == U);
                        ASSERTV(U_SPEC, V_SPEC, UU  == V);
                        ASSERTV(U_SPEC, V_SPEC, BB2 == AA2);
                        ASSERTV(U_SPEC, V_SPEC, B2  == A2 );

                        for (const_iterator iu = U.begin(), iv = VV.begin();
                             iu != U.end(); ++iu, ++iv) {
                            // Verify that U and V have no elements in common
                            ASSERTV(U_SPEC, V_SPEC, *iu,
                                    bsls::Util::addressOf(*iv) !=
                                                   bsls::Util::addressOf(*iu));
                        }

                        for (const_iterator iu = UU.begin(), iv = V.begin();
                             iu != UU.end(); ++iu, ++iv) {
                            // Verify that U and V have no elements in common
                            ASSERTV(U_SPEC, V_SPEC, *iu,
                                    bsls::Util::addressOf(*iv) !=
                                                   bsls::Util::addressOf(*iu));
                        }
                    }
                } // end for (vi)
            } // end for (ui)
        }
    }

#ifdef BDE_BUILD_TARGET_EXC
    if (veryVerbose)
        printf("Assign cross product of values With Exceptions\n");

    {
        static const char *SPECS[] = { // len: 0-2, 4, 4, 9,
            "",
            "A",
            "BC",
            "DEAB",
            "CBEA",
            "BACEDEDCB"
        };

        enum { NUM_SPECS = sizeof SPECS / sizeof SPECS[0] };

        for (int ui = 0; ui < NUM_SPECS; ++ui) {
            const char *const U_SPEC = SPECS[ui];
            const size_t      uLen   = std::strlen(U_SPEC);

            if (veryVerbose) {
                printf("\tFor lhs objects of length " ZU ":\t", uLen);
                P(U_SPEC);
            }

            Obj mUU; const Obj& UU = gg(&mUU, U_SPEC);  // control

            ASSERTV(ui, uLen == UU.size());             // same lengths

            // int vOldLen = -1;
            for (int vi = 0; vi < NUM_SPECS; ++vi) {
                const char *const V_SPEC = SPECS[vi];
                const size_t      vLen   = std::strlen(V_SPEC);

                if (veryVeryVerbose) {
                    printf("\t\tFor rhs objects of length " ZU ":\t", vLen);
                    P(V_SPEC);
                }

                // control
                Obj mVV; const Obj& VV = gg(&mVV, V_SPEC);

                // Exception-test macros must use 'oa2':
                numThrows = -1;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa2) {
                    ++numThrows;

                    // We want to use the allocation limit only for the
                    // assignment operation, not for producing the initial
                    // objects.  Thus, we save the limit in AL and turn off the
                    // limit until we're ready to test assignment.

                    const Int64 AL = oa2.allocationLimit();
                    oa2.setAllocationLimit(-1);

                    Obj mU(xoa2); const Obj& U = gg(&mU, U_SPEC);
                    Obj mV(xoa1); const Obj& V = gg(&mV, V_SPEC);

                    if (veryVeryVerbose) {
                        printf("\t| "); P_(U); P(V);
                    }

                    const Int64 BB2 = oa2.numBlocksTotal();
                    const Int64 B2  = oa2.numBlocksInUse();

                    oa2.setAllocationLimit(AL);
                    mU = V; // test assignment here

                    const Int64 AA2 = oa2.numBlocksTotal();
                    const Int64 A2  = oa2.numBlocksInUse();

                    ASSERTV(U_SPEC, V_SPEC, checkIntegrity(U, vLen));
                    ASSERTV(U_SPEC, V_SPEC, VV == U);
                    ASSERTV(U_SPEC, V_SPEC, VV == V);
                    ASSERTV(U_SPEC, V_SPEC, V  == U);
                    ASSERTV(U_SPEC, V_SPEC, BB2 + deltaBlocks(vLen) >= AA2);

                    ptrdiff_t difference = static_cast<ptrdiff_t>(vLen-uLen);

                    ASSERTV(U_SPEC, V_SPEC,B2 + deltaBlocks(difference) == A2);

                    ASSERTV(numThrows, U_SPEC, V_SPEC, assignOfTypeAllocates,
                            NameOf<TYPE>(), k_NO_EXCEPT ||
                        (numThrows > 0) ==
                            (VV.size() > UU.size() ||
                                      (!VV.empty() && assignOfTypeAllocates)));

                    // 'mV' (and therefore 'V') now out of scope
                    ASSERTV(U_SPEC, V_SPEC, VV == U);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                // now test move assign where operands use different
                // allocators, should turn out just like normal copy-assign

                // Exception-test macros must use 'oa2':
                numThrows = -1;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa2) {
                    ++numThrows;

                    // We want to use the allocation limit only for the
                    // assignment operation, not for producing the initial
                    // objects.  Thus, we save the limit in AL and turn off the
                    // limit until we're ready to test assignment.

                    const Int64 AL = oa2.allocationLimit();
                    oa2.setAllocationLimit(-1);

                    Obj mU(xoa2); const Obj& U = gg(&mU, U_SPEC);
                    Obj mV(xoa1); const Obj& V = gg(&mV, V_SPEC);

                    if (veryVeryVerbose) {
                        printf("\t| "); P_(U); P(V);
                    }

                    const Int64 BB2 = oa2.numBlocksTotal();
                    const Int64 B2  = oa2.numBlocksInUse();

                    ASSERTV(U_SPEC, V_SPEC, UU == U);
                    ASSERTV(U_SPEC, V_SPEC, VV == V);

                    oa2.setAllocationLimit(AL);
                    mU = MoveUtil::move(mV);       // test move assignment here

                    ASSERTV(k_NO_EXCEPT || (0 < numThrows) ==
                                (UU.size() < VV.size()
                             || (!VV.empty() && SCOPED_ALLOC)));

                    const Int64 AA2 = oa2.numBlocksTotal();
                    const Int64 A2  = oa2.numBlocksInUse();

                    ASSERTV(U_SPEC, V_SPEC, checkIntegrity(U, vLen));
                    ASSERTV(U_SPEC, V_SPEC, VV == U);
                    ASSERTV(U_SPEC, V_SPEC, BB2 + deltaBlocks(vLen) >= AA2);

                    ptrdiff_t difference = static_cast<ptrdiff_t>(vLen-uLen);

                    ASSERTV(U_SPEC, V_SPEC,
                                           B2 + deltaBlocks(difference) == A2);

                    // 'mV' (and therefore 'V') now out of scope
                    ASSERTV(U_SPEC, V_SPEC, VV == U);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                // now test move assign where operands use the same allocator.
                // Should swap without allocating.

                // Exception-test macros must use 'testAllocator':
                numThrows = -1;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa2) {
                    ++numThrows;

                    // We want to use the allocation limit only for the
                    // assignment operation, not for producing the initial
                    // objects.  Thus, we save the limit in AL and turn off the
                    // limit until we're ready to test assignment.

                    const Int64 AL = oa2.allocationLimit();
                    oa2.setAllocationLimit(-1);

                    Obj mU(xoa2); const Obj& U = gg(&mU, U_SPEC);
                    Obj mV(xoa2); const Obj& V = gg(&mV, V_SPEC);

                    if (veryVeryVerbose) {
                        printf("\t| "); P_(U); P(V);
                    }

                    const Int64 BB2 = oa2.numBlocksTotal();
                    const Int64 B2  = oa2.numBlocksInUse();

                    oa2.setAllocationLimit(AL);

                    mU = MoveUtil::move(mV);     // test move assignment here,
                                                 // which should be just a swap

                    ASSERT(0 == numThrows);

                    const Int64 AA2 = oa2.numBlocksTotal();
                    const Int64 A2  = oa2.numBlocksInUse();

                    ASSERTV(U_SPEC, V_SPEC, checkIntegrity(U, vLen));
                    ASSERTV(U_SPEC, V_SPEC, VV  == U);
                    ASSERTV(U_SPEC, V_SPEC, UU  == V);
                    ASSERTV(U_SPEC, V_SPEC, BB2 == AA2);
                    ASSERTV(U_SPEC, V_SPEC, B2  == A2);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            } // end for (vi)
        } // end for (ui)
    } // end exception test
#endif

    // Exception testing of aliasing assigns is inappropriate, they should
    // never allocate and hence never throw.

    if (veryVerbose) printf("Testing self assignment (Aliasing).");
    {
        static const char *SPECS[] = {
            "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
            "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
            "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
        };

        enum { NUM_SPECS = sizeof SPECS / sizeof SPECS[0] };

        int oldLen = -1;
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC = SPECS[ti];
            const int         curLen = static_cast<int>(std::strlen(SPEC));

            if (veryVerbose) {
                printf("\tFor an object of length %d:\t", curLen);
                P(SPEC);
            }

            ASSERTV(SPEC, oldLen < curLen);        // strictly increasing

            oldLen = curLen;

            Obj mX; const Obj& X = gg(&mX, SPEC);  // control

            ASSERTV(ti, curLen == (int)X.size());  // same lengths

            Obj mY(xoa2); const Obj& Y = gg(&mY, SPEC);

            if (veryVeryVerbose) { T_; T_; P(Y); }

            ASSERTV(SPEC, Y == Y);
            ASSERTV(SPEC, X == Y);

            Int64 B2, B3, A2, A3;
            B2 = oa2.numBlocksInUse();
            B3 = oa2.numBlocksTotal();

            // test aliasing assignment here, shouldn't throw or allocate

            mY = Y;

            A2 = oa2.numBlocksInUse();
            A3 = oa2.numBlocksTotal();

            ASSERTV(SPEC, Y  == Y);
            ASSERTV(SPEC, X  == Y);
            ASSERTV(SPEC, B2 == A2);
            ASSERTV(SPEC, B3 == A3);

            B2 = oa2.numBlocksInUse();
            B3 = oa2.numBlocksTotal();

            // test aliasing move assign here, shouldn't throw or allocate

            mY = MoveUtil::move(mY);

            A2 = oa2.numBlocksInUse();
            A3 = oa2.numBlocksTotal();

            ASSERTV(SPEC, Y  == Y);
            ASSERTV(SPEC, X  == Y);
            ASSERTV(SPEC, B2 == A2);
            ASSERTV(SPEC, B3 == A3);
        } // end for (ti)
    } // end self-assignment test
}

template <class TYPE, class ALLOC>
template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
          bool OTHER_FLAGS>
void TestDriver<TYPE, ALLOC>::
                        test07_select_on_container_copy_construction_dispatch()
{
    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value ||
                           bsl::uses_allocator<TYPE, ALLOC>::value;

    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                    TYPE,
                                    SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
                                    OTHER_FLAGS,
                                    OTHER_FLAGS,
                                    OTHER_FLAGS> StdAlloc;
    typedef bsl::list<TYPE, StdAlloc>            Obj;

    const bool PROPAGATE = SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG;

    static const char *SPECS[] = { "", "A", "BC", "CDE" };

    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char *const SPEC   = SPECS[ti];
        const size_t      LENGTH = strlen(SPEC);

        TestValues VALUES(SPEC);

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        StdAlloc ma(&oa);

        {
            const Obj W(VALUES.begin(), VALUES.end(), ma);  // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            VALUES.resetIterators();

            Obj mX(VALUES.begin(), VALUES.end(), ma);  const Obj& X = mX;

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            bslma::TestAllocatorMonitor dam(&da);
            bslma::TestAllocatorMonitor oam(&oa);

            const Obj Y(X);

            ASSERTV(SPEC, W == Y);
            ASSERTV(SPEC, W == X);
            ASSERTV(SPEC, PROPAGATE, PROPAGATE == (ma == Y.get_allocator()));
            ASSERTV(SPEC, PROPAGATE,               ma == X.get_allocator());

            if (PROPAGATE) {
                ASSERTV(SPEC, 0 != TYPE_ALLOC || dam.isInUseSame());
                ASSERTV(SPEC, 0 ==     LENGTH || oam.isInUseUp());
            }
            else {
                ASSERTV(SPEC, 0 ==     LENGTH || dam.isInUseUp());
                ASSERTV(SPEC, oam.isTotalSame());
            }
        }
        ASSERTV(SPEC, 0 == da.numBlocksInUse());
        ASSERTV(SPEC, 0 == oa.numBlocksInUse());
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE, ALLOC>::test07_select_on_container_copy_construction()
{
    // ------------------------------------------------------------------------
    // COPY CONSTRUCTOR: ALLOCATOR PROPAGATION
    //
    // Concerns:
    //: 1 The allocator of a source object using a standard allocator is
    //:   propagated to the newly constructed object according to the
    //:   'select_on_container_copy_construction' method of the allocator.
    //:
    //: 2 In the absence of a 'select_on_container_copy_construction' method,
    //:   the allocator of a source object using a standard allocator is always
    //:   propagated to the newly constructed object (C++03 semantics).
    //:
    //: 3 The effect of the 'select_on_container_copy_construction' trait is
    //:   independent of the other three allocator propagation traits.
    //
    // Plan:
    //: 1 Specify a set S of object values with varied differences, ordered by
    //:   increasing length, to be used in the following tests.
    //:
    //: 2 Create a 'bsltf::StdStatefulAllocator' with its
    //:   'select_on_container_copy_construction' property configured to
    //:   'false'.  In two successive iterations of P-3..5, first configure the
    //:   three properties not under test to be 'false', then configure them
    //:   all to be 'true'.
    //:
    //: 3 For each value in S, initialize objects 'W' (a control) and 'X' using
    //:   the allocator from P-2.
    //:
    //: 4 Copy construct 'Y' from 'X' and use 'operator==' to verify that both
    //:   'X' and 'Y' subsequently have the same value as 'W'.
    //:
    //: 5 Use the 'get_allocator' method to verify that the allocator of 'X'
    //:   is *not* propagated to 'Y'.
    //:
    //: 6 Repeat P-2..5 except that this time configure the allocator property
    //:   under test to 'true' and verify that the allocator of 'X' *is*
    //:   propagated to 'Y'.  (C-1)
    //:
    //: 7 Repeat P-2..5 except that this time use a 'StatefulStlAllocator',
    //:   which does not define a 'select_on_container_copy_construction'
    //:   method, and verify that the allocator of 'X' is *always* propagated
    //:   to 'Y'.  (C-2..3)
    //
    // Testing:
    //   select_on_container_copy_construction
    // ------------------------------------------------------------------------

    if (verbose) printf("\n'select_on_container_copy_construction' "
                        "propagates *default* allocator.\n");

    test07_select_on_container_copy_construction_dispatch<false, false>();
    test07_select_on_container_copy_construction_dispatch<false, true>();

    if (verbose) printf("\n'select_on_container_copy_construction' "
                        "propagates allocator of source object.\n");

    test07_select_on_container_copy_construction_dispatch<true, false>();
    test07_select_on_container_copy_construction_dispatch<true, true>();

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) printf("\nVerify C++03 semantics (allocator has no "
                        "'select_on_container_copy_construction' method).\n");

    typedef StatefulStlAllocator<TYPE> Allocator;
    typedef bsl::list<TYPE, Allocator> Obj;

    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE"
        };
        enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            TestValues VALUES(SPEC);

            const int ALLOC_ID = ti + 73;

            Allocator a;  a.setId(ALLOC_ID);

            const Obj W(VALUES.begin(), VALUES.end(), a);  // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            VALUES.resetIterators();

            Obj mX(VALUES.begin(), VALUES.end(), a);  const Obj& X = mX;

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            const Obj Y(X);

            ASSERTV(SPEC, W == Y);
            ASSERTV(SPEC, W == X);
            ASSERTV(SPEC, ALLOC_ID == Y.get_allocator().id());
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::test07_copyCtor()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTORS:
    //
    // Concerns:
    //: 1 The new object's value is the same as that of the original object
    //:   (relying on the equality operator) and created with the correct
    //:   allocator.
    //:
    //: 2 The value of the original object is left unaffected.
    //:
    //: 3 Subsequent changes in or destruction of the source object have no
    //:   effect on the copy-constructed object.
    //:
    //: 4 Subsequent changes ('push_back's) on the created object have no
    //:   effect on the original.
    //:
    //: 5 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 6 The function is exception neutral w.r.t. memory allocation.
    //:
    //: 7 An object copied from an rvalue with no allocator, or with a matching
    //:   allocator specified, will leave the copied object in a valid,
    //:   default-constructed state.
    //:   1 No allocator specified to c'tor.
    //:   2 Allocator specified to c'tor.
    //:
    //: 8 An object copied from an rvalue with a non-matching allocator
    //:   supplied will not modify the rvalue.
    //
    // Plan:
    //: 1 Specify a set S of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used in the
    //:   following tests.
    //:
    //: 2 For concerns 1 - 3, for each value in S, initialize objects w and x,
    //:   copy construct y from x and use 'operator==' to verify that both x
    //:   and y subsequently have the same value as w.  Let x go out of scope
    //:   and again verify that w == y.
    //:
    //: 3 For concern 4, for each value in S initialize objects w and x, and
    //:   copy construct y from x.  Change the state of y, by using the
    //:   *primary* *manipulator* 'push_back'.  Using the 'operator!=' verify
    //:   that y differs from x and w.
    //:
    //: 4 To address concern 5, we will perform tests performed for concern 1:
    //:   o While passing a testAllocator as a parameter to the new object and
    //:     ascertaining that the new object gets its memory from the provided
    //:     testAllocator.  Also perform test for concerns 2 and 4.
    //:   o Where the object is constructed with an object allocator, and
    //:     neither of global and default allocator is used to supply memory.
    //:
    //: 5 To address concern 6, perform tests for concern 1 performed in the
    //:   presence of exceptions during memory allocations using a
    //:   'bslma_TestAllocator' and varying its *allocation* *limit*.
    //
    // Testing:
    //   list(const list& orig);
    //   list(const list& orig, const A&);
    //   list(list&& orig);
    //   list(list&& orig, const A&);
    // ------------------------------------------------------------------------

    if (verbose) printf("\tTesting type: %s (ALLOC = %d).\n",
                        NameOf<TYPE>().name(), SCOPED_ALLOC);

    bslma::TestAllocator oaa("objectA", veryVeryVeryVerbose);
    const ALLOC          xoa(&oaa);

    bslma::TestAllocator oab("objectB", veryVeryVeryVerbose);
    const ALLOC          xob(&oab);

    ASSERT(xoa!= xob);

    TestValues VALUES("ABCDEFGH");
    const int  NUM_VALUES = 8;

    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE",
            "DEAB",
            "EABCD",
            "ABCDEAB",
            "ABCDEABC",
            "ABCDEABCD",
            "ABCDEABCDEABCDE",
            "ABCDEABCDEABCDEA",
            "ABCDEABCDEABCDEAB"
        };
        enum { NUM_SPECS = sizeof SPECS / sizeof SPECS[0] };

        int oldLen = -1;
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = std::strlen(SPEC);

            if (verbose) {
                printf("For an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            ASSERTV(SPEC, oldLen < (int)LENGTH); // strictly increasing

            oldLen = static_cast<int>(LENGTH);

            // Create control object 'W' and 'D'
            Obj mW;  const Obj& W = gg(&mW, SPEC);

            const Obj D;                         // default constructed

            ASSERTV(ti, LENGTH == W.size());     // same lengths

            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

            ASSERT(X == W);

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            {   // Testing concern 1..4

                if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                Obj *pX = new Obj(xoa);
                gg(pX, SPEC);
                Obj mY0(*pX); const Obj& Y0 = mY0;

                ASSERTV(SPEC, checkIntegrity(Y0, LENGTH));
                ASSERTV(SPEC, W == Y0);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, Y0.get_allocator() ==
                                           bslma::Default::defaultAllocator());

                delete pX;
                ASSERTV(SPEC, W == Y0);

                mY0.push_back(VALUES[LENGTH % NUM_VALUES]);
                if (veryVerbose) {
                    printf("\t\t\t\tAfter Append to new obj : ");
                    P(Y0);
                }
                ASSERTV(SPEC, checkIntegrity(Y0, LENGTH + 1));
                ASSERTV(SPEC, W != Y0);
                ASSERTV(SPEC, Y0.get_allocator() ==
                                           bslma::Default::defaultAllocator());
            }

            {   // Testing concern 4.

                if (veryVerbose) printf("\tInsert into created obj, "
                                        "without test allocator:\n");

                Obj Y1(X);

                Y1.push_back(VALUES[Y1.size() % NUM_VALUES]);

                ASSERTV(SPEC, checkIntegrity(Y1, LENGTH + 1));
                ASSERTV(SPEC, Y1.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y1);
                ASSERTV(SPEC, X != Y1);
                ASSERTV(SPEC, W == X);
            }

            {   // Testing concern 5 with test allocator.

                if (veryVerbose)
                    printf("\tInsert into created obj, "
                           "with test allocator:\n");

                const Int64 BB = oaa.numBlocksTotal();
                const Int64  B = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                Obj Y11(X, xoa);

                const Int64 AA = oaa.numBlocksTotal();
                const Int64  A = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    printf("\t\t\t\tBefore Append: "); P(Y11);
                }

                ASSERTV(SPEC, BB + expectedBlocks(LENGTH) == AA);
                ASSERTV(SPEC,  B + expectedBlocks(LENGTH) ==  A);

                const Int64 CC = oaa.numBlocksTotal();
                const Int64  C = oaa.numBlocksInUse();

                Y11.push_back(VALUES[LENGTH % NUM_VALUES]);

                const Int64 DD = oaa.numBlocksTotal();
                const Int64  D = oaa.numBlocksInUse();

                // Allocations should increase by one node block for the list.
                // If TYPE uses an allocator, allocations should increase by
                // one more block.
                ASSERTV(SPEC, CC + deltaBlocks(1) == DD);
                ASSERTV(SPEC, C  + deltaBlocks(1) == D );

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Append : ");
                    P(Y11);
                }

                ASSERTV(SPEC, Y11.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y11);
                ASSERTV(SPEC, X != Y11);
                ASSERTV(SPEC, Y11.get_allocator() == X.get_allocator());
                ASSERTV(SPEC, X == W);
            }

#ifdef BDE_BUILD_TARGET_EXC
            {   // Exception checking.

                const Int64 BB = oaa.numBlocksTotal();
                const Int64  B = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                size_t allocations = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oaa) {
                    allocations += bslmaExceptionCounter;

                    const Obj Y2(X, xoa);

                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                        printf("\t\t\t\tObj : "); P(Y2);
                    }

                    ASSERTV(SPEC, checkIntegrity(Y2, LENGTH));
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, Y2.get_allocator() == X.get_allocator());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const Int64 AA = oaa.numBlocksTotal();
                const Int64  A = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                }

                ASSERTV(SPEC, BB + (int) allocations == AA);
                ASSERTV(SPEC,  B + 0 ==  A);
            }
#endif // BDE_BUILD_TARGET_EXC

            {   // Testing concern 7.
                if (veryVerbose) { printf("\tRvalue copy, no alloc :"); }

                Obj       RV(X, xoa);
                const Obj Y7(MoveUtil::move(RV));

                if (veryVerbose) {
                    printf("\tObj : "); P(Y7);
                }

                ASSERTV(SPEC, checkIntegrity(Y7, LENGTH));
                ASSERTV(SPEC, checkIntegrity(RV, 0));
                ASSERTV(SPEC, W == Y7);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, D == RV);
                ASSERTV(SPEC, Y7.get_allocator() == xoa); // propagates
                ASSERTV(SPEC, RV.get_allocator() == xoa); // persists

                // Concern 8 - after original is modified

                RV.push_back(VALUES[LENGTH % NUM_VALUES]);

                ASSERTV(SPEC, checkIntegrity(Y7, LENGTH));
                ASSERTV(SPEC, checkIntegrity(RV, 1));
                ASSERTV(SPEC, W == Y7);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, D != RV);
                ASSERTV(SPEC, Y7.get_allocator() == xoa); // propagates
                ASSERTV(SPEC, RV.get_allocator() == xoa); // persists
            }

            {   // Testing concern 7.
                if (veryVerbose) { printf("\tRvalue copy, match alloc :");}

                Obj       RV(X, xoa);
                const Obj Y7(MoveUtil::move(RV), xoa);

                if (veryVerbose) { printf("\tObj : "); P(Y7); }

                ASSERTV(SPEC, checkIntegrity(Y7, LENGTH));
                ASSERTV(SPEC, checkIntegrity(RV, 0));
                ASSERTV(SPEC, W == Y7);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, D == RV);
                ASSERTV(SPEC, Y7.get_allocator() == xoa); // propagates
                ASSERTV(SPEC, RV.get_allocator() == xoa); // persists

                // Concern 8 - after original is modified

                RV.push_back(VALUES[LENGTH % NUM_VALUES]);

                ASSERTV(SPEC, checkIntegrity(Y7, LENGTH));
                ASSERTV(SPEC, checkIntegrity(RV, 1));
                ASSERTV(SPEC, W == Y7);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, D != RV);
                ASSERTV(SPEC, Y7.get_allocator() == xoa); // propagates
                ASSERTV(SPEC, RV.get_allocator() == xoa); // persists
            }

            {   // Testing concern 7.
                if (veryVerbose) { printf("\tRvalue cp, non-match alloc:");}

                Obj       RV(X, xoa);
                const Obj Y7(MoveUtil::move(RV), xob);

                if (veryVerbose) { printf("\tObj : "); P(Y7); }

                ASSERTV(SPEC, checkIntegrity(Y7, LENGTH));
                ASSERTV(SPEC, checkIntegrity(RV, LENGTH));
                ASSERTV(SPEC, W == Y7);
                ASSERTV(SPEC, Y7.get_allocator() == xob);   // no propagate
                ASSERTV(SPEC, RV.get_allocator() == xoa);   // persists

                // Concern 8 - after original is modified

                RV.clear();
                RV.push_back(VALUES[LENGTH % NUM_VALUES]);

                ASSERTV(SPEC, checkIntegrity(Y7, LENGTH));
                ASSERTV(SPEC, checkIntegrity(RV, 1));
                ASSERTV(SPEC, W == Y7);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, D != RV);
                ASSERTV(SPEC, Y7.get_allocator() == xob);
                ASSERTV(SPEC, RV.get_allocator() == xoa); // allocator persists
            }

#ifdef BDE_BUILD_TARGET_EXC
            {   // Exception checking, concern 7.1

                const Int64 BB = oaa.numBlocksTotal();
                const Int64 B  = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                size_t allocations = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oaa) {
                    allocations += bslmaExceptionCounter;

                    Obj       RV(X, xoa);
                    const Obj Y2(MoveUtil::move(RV));

                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                        printf("\t\t\t\tObj : "); P(Y2);
                    }

                    ASSERTV(SPEC, checkIntegrity(Y2, LENGTH));
                    ASSERTV(SPEC, checkIntegrity(RV, 0));
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, D == RV);
                    ASSERTV(SPEC, xoa == Y2.get_allocator());
                    ASSERTV(SPEC, xoa == RV.get_allocator()); // propagates

                    RV.push_back(VALUES[LENGTH % NUM_VALUES]);

                    ASSERTV(SPEC, checkIntegrity(Y2, LENGTH));
                    ASSERTV(SPEC, checkIntegrity(RV, 1));
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, D != RV);
                    ASSERTV(SPEC, xoa == Y2.get_allocator());
                    ASSERTV(SPEC, xoa == RV.get_allocator());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const Int64 AA = oaa.numBlocksTotal();
                const Int64 A  = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                }

                ASSERTV(SPEC, BB + (int) allocations == AA);
                ASSERTV(SPEC,  B + 0 ==  A);
            }

            {   // Exception checking, concern 7.1

                const Int64 BB = oaa.numBlocksTotal();
                const Int64 B  = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                size_t allocations = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oaa) {
                    allocations += bslmaExceptionCounter;

                    Obj       RV(X, xoa);
                    const Obj Y2(MoveUtil::move(RV), xoa);

                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                        printf("\t\t\t\tObj : "); P(Y2);
                    }

                    ASSERTV(SPEC, checkIntegrity(Y2, LENGTH));
                    ASSERTV(SPEC, checkIntegrity(RV, 0));
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, D == RV);
                    ASSERTV(SPEC, xoa == Y2.get_allocator());
                    ASSERTV(SPEC, xoa == RV.get_allocator());

                    RV.push_back(VALUES[LENGTH % NUM_VALUES]);

                    ASSERTV(SPEC, checkIntegrity(Y2, LENGTH));
                    ASSERTV(SPEC, checkIntegrity(RV, 1));
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, D != RV);
                    ASSERTV(SPEC, xoa == Y2.get_allocator());
                    ASSERTV(SPEC, xoa == RV.get_allocator());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const Int64 AA = oaa.numBlocksTotal();
                const Int64 A  = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                }

                ASSERTV(SPEC, BB + (int) allocations == AA);
                ASSERTV(SPEC,  B + 0 ==  A);
            }

            {   // Exception checking, concern 7.2

                const Int64 BB = oaa.numBlocksTotal();
                const Int64 B  = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                size_t allocations = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oaa) {
                    allocations += bslmaExceptionCounter;

                    Obj RV(X, xoa);

                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, W == RV);

                    const Obj Y2(MoveUtil::move(RV), xob);
                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                        printf("\t\t\t\tObj : "); P(Y2);
                    }
                    ASSERTV(SPEC, checkIntegrity(Y2, LENGTH));
                    ASSERTV(SPEC, checkIntegrity(RV, LENGTH));
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, xob == Y2.get_allocator());
                    ASSERTV(SPEC, xoa == RV.get_allocator());  // not prop.

                    RV.clear();
                    RV.push_back(VALUES[LENGTH % NUM_VALUES]);

                    ASSERTV(SPEC, checkIntegrity(Y2, LENGTH));
                    ASSERTV(SPEC, checkIntegrity(RV, 1));
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, D != RV);
                    ASSERTV(SPEC, xob == Y2.get_allocator());
                    ASSERTV(SPEC, xoa == RV.get_allocator());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const Int64 AA = oaa.numBlocksTotal();
                const Int64 A  = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                }

                ASSERTV(SPEC, BB + (int) allocations == AA);
                ASSERTV(SPEC,  B + 0 ==  A);
            }
#endif // BDE_BUILD_TARGET_EXC
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::test06_equalityOp()
{
    // ------------------------------------------------------------------------
    // TESTING EQUALITY OPERATORS
    //
    // Concerns:
    //: 1 Two objects, 'X' and 'Y', compare equal if and only if they contain
    //:   the same values.
    //:
    //: 2 No non-salient attributes (i.e., 'allocator') participate.
    //:
    //: 3 'true  == (X == X)' (i.e., identity)
    //:
    //: 4 'false == (X != X)' (i.e., identity)
    //:
    //: 5 'X == Y' if and only if 'Y == X' (i.e., commutativity)
    //:
    //: 6 'X != Y' if and only if 'Y != X' (i.e., commutativity)
    //:
    //: 7 'X != Y' if and only if '!(X == Y)'
    //:
    //: 8 Comparison is symmetric with respect to user-defined conversion
    //:   (i.e., both comparison operators are free functions).
    //:
    //: 9 Non-modifiable objects can be compared (i.e., objects or references
    //:   providing only non-modifiable access).
    //:
    //:10 No memory allocation occurs as a result of comparison (e.g., the
    //:   arguments are not passed by value).
    //:
    //:11 The equality operator's signature and return type are standard.
    //:
    //:12 The inequality operator's signature and return type are standard.
    //
    // Plan:
    //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
    //:   initialize function pointers having the appropriate signatures and
    //:   return types for the two homogeneous, free equality- comparison
    //:   operators defined in this component.  (C-8..9, 11..12)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 3 Using the table-driven technique, specify a set of distinct
    //:   specifications for the 'gg' function.
    //:
    //: 4 For each row 'R1' in the table of P-3: (C-1..7)
    //:
    //:   1 Create a single object, using a comparator that can be disabled and
    //:     a "scratch" allocator, and use it to verify the reflexive
    //:     (anti-reflexive) property of equality (inequality) in the presence
    //:     of aliasing.  (C-3..4)
    //:
    //:   2 For each row 'R2' in the table of P-3: (C-1..2, 5..7)
    //:
    //:     1 Record, in 'EXP', whether or not distinct objects created from
    //:       'R1' and 'R2', respectively, are expected to have the same value.
    //:
    //:     2 For each of two configurations, 'a' and 'b': (C-1..2, 5..7)
    //:
    //:       1 Create two (object) allocators, 'oax' and 'oay'.
    //:
    //:       2 Create an object 'X', using 'oax', having the value 'R1'.
    //:
    //:       3 Create an object 'Y', using 'oax' in configuration 'a' and
    //:         'oay' in configuration 'b', having the value 'R2'.
    //:
    //:       4 Disable the comparator so that it will cause an error if it's
    //:         used.
    //:
    //:       5 Verify the commutativity property and expected return value for
    //:         both '==' and '!=', while monitoring both 'oax' and 'oay' to
    //:         ensure that no object memory is ever allocated by either
    //:         operator.  (C-1..2, 5..7, 10)
    //:
    //: 5 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-10)
    //
    // Testing:
    //   bool operator==(const list& lhs, const list& rhs);
    //   bool operator!=(const list& lhs, const list& rhs);
    // ------------------------------------------------------------------------

    if (verbose)
              printf("\nAssign the address of each operator to a variable.\n");
    {
        typedef bool (*operatorPtr)(const Obj&, const Obj&);

        // Verify that the signatures and return types are standard.

        operatorPtr operatorEq = operator==;
        operatorPtr operatorNe = operator!=;

        (void) operatorEq;  // quash potential compiler warnings
        (void) operatorNe;
    }

    static const char *SPECS[] = {
        "",
        "A",      "B",
        "AA",     "AB",     "BB",     "BA",
        "AAA",    "BAA",    "ABA",    "AAB",
        "AAAA",   "BAAA",   "ABAA",   "AABA",   "AAAB",
        "AAAAA",  "BAAAA",  "ABAAA",  "AABAA",  "AAABA",  "AAAAB",
        "AAAAAA", "BAAAAA", "AABAAA", "AAABAA", "AAAAAB",
        "AAAAAAA",          "BAAAAAA",          "AAAAABA",
        "AAAAAAAA",         "ABAAAAAA",         "AAAAABAA",
        "AAAAAAAAA",        "AABAAAAAA",        "AAAAABAAA",
        "AAAAAAAAAA",       "AAABAAAAAA",       "AAAAABAAAA",
        "AAAAAAAAAAA",      "AAAABAAAAAA",      "AAAAABAAAAA",
        "AAAAAAAAAAAA",     "AAAABAAAAAAA",     "AAAAABAAAAAA",
        "AAAAAAAAAAAAA",    "AAAABAAAAAAAA",    "AAAAABAAAAAAA",
        "AAAAAAAAAAAAAA",   "AAAABAAAAAAAAA",   "AAAAABAAAAAAAA",
        "AAAAAAAAAAAAAAA",  "AAAABAAAAAAAAAA",  "AAAAABAAAAAAAAA"
    };

    enum { NUM_SPECS = sizeof SPECS / sizeof SPECS[0] };

    if (verbose) printf("\nCompare every value with every value.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC1   = SPECS[ti];
            const size_t      LENGTH1 = strlen(SPEC1);

            if (veryVerbose) { T_ P_(LENGTH1) P(SPEC1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                ALLOC                xscratch(&scratch);

                Obj mX(xscratch);  const Obj& X = gg(&mX, SPEC1);

                ASSERTV(SPEC1, X,   X == X);
                ASSERTV(SPEC1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_SPECS; ++tj) {
                const char *const SPEC2   = SPECS[tj];
                const size_t      LENGTH2 = strlen(SPEC2);
                const bool        EXP     = ti == tj;  // expected result

                if (veryVerbose) { T_ T_ P_(LENGTH2) P(SPEC2) }

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create two distinct test allocators, 'oax' and 'oay'.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    ALLOC xxa(&xa);
                    ALLOC xya(&ya);

                    Obj mX(xxa);  const Obj& X = gg(&mX, SPEC1);
                    Obj mY(xya);  const Obj& Y = gg(&mY, SPEC2);

                    ASSERTV(SPEC1, SPEC2, CONFIG, LENGTH1 == X.size());
                    ASSERTV(SPEC1, SPEC2, CONFIG, LENGTH2 == Y.size());

                    if (veryVerbose) { T_ T_ P_(X) P(Y); }

                    // Verify value, commutativity, and no memory allocation.

                    bslma::TestAllocatorMonitor oaxm(&xa);
                    bslma::TestAllocatorMonitor oaym(&ya);

                    ASSERTV(SPEC1, SPEC2, CONFIG,  EXP == (X == Y));
                    ASSERTV(SPEC1, SPEC2, CONFIG,  EXP == (Y == X));

                    ASSERTV(SPEC1, SPEC2, CONFIG, !EXP == (X != Y));
                    ASSERTV(SPEC1, SPEC2, CONFIG, !EXP == (Y != X));

                    ASSERTV(SPEC1, SPEC2, CONFIG, oaxm.isTotalSame());
                    ASSERTV(SPEC1, SPEC2, CONFIG, oaym.isTotalSame());
                }
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::test04_basicAccessors()
{
    // ------------------------------------------------------------------------
    // TESTING BASIC ACCESSORS:
    //
    // Concerns:
    //: 1 size() returns the list size.
    //:
    //: 2 begin() == end() if and only if the list is empty
    //:
    //: 3 Iterating from begin() to end() will visit every value in a list and
    //:   only the values in that list.
    //:
    //: 4 Iteration works for both const containers (using const_iterator) and
    //:   non-const containers (using iterator).
    //:
    //: 5 empty() returns true if size() return 0.
    //:
    //: 6 The test function 'succ' increments an iterator by n.
    //:
    //: 7 The test function 'nthElem' returns the nth element of a list.
    //:
    //: 8 The test function 'is_mutable' returns true if its argument is a
    //:   mutable lvalue.
    //
    // Plan:
    //: 1 Specify a set S of representative object values ordered by increasing
    //:   length.  For each value w in S, initialize a newly constructed object
    //:   x with w using 'gg' and verify that each basic accessor returns the
    //:   expected result.
    //
    // NOTE: This is not a thorough test of iterators.  This test is only
    // sufficient for using iterators to access the contents of a list in
    // order.
    //
    // Testing:
    //   int size() const;
    //   bool empty() const;
    //   iterator begin();
    //   iterator end();
    //   const_iterator begin() const;
    //   const_iterator end() const;
    //
    //   iterator succ(iterator);
    //   const_iterator succ(iterator) const;
    //   T& nthElem(list& x, int n);
    //   const T& nthElem(list& x, int n) const;
    //   bool is_mutable(T& val);
    //   bool is_mutable(const T& val);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    ALLOC                xoa(&oa);

    const int MAX_LENGTH = 32;

    static const struct {
        int         d_lineNum;                   // source line number
        const char *d_spec_p;                    // specification string
        int         d_length;                    // expected length
        int         d_elements[MAX_LENGTH + 1];  // expected element values
    } DATA[] = {
        //line  spec            length  elements
        //----  --------------  ------  ------------------------
        { L_,   "",                  0, { }                     },
        { L_,   "A",                 1, { VA }                  },
        { L_,   "B",                 1, { VB }                  },
        { L_,   "AB",                2, { VA, VB }              },
        { L_,   "BC",                2, { VB, VC }              },
        { L_,   "BCA",               3, { VB, VC, VA }          },
        { L_,   "CAB",               3, { VC, VA, VB }          },
        { L_,   "CDAB",              4, { VC, VD, VA, VB }      },
        { L_,   "DABC",              4, { VD, VA, VB, VC }      },
        { L_,   "ABCDE",             5, { VA, VB, VC, VD, VE }  },
        { L_,   "EDCBA",             5, { VE, VD, VC, VB, VA }  },
        { L_,   "ABCDEA",            6, { VA, VB, VC, VD, VE,
                                          VA }                  },
        { L_,   "ABCDEAB",           7, { VA, VB, VC, VD, VE,
                                          VA, VB }              },
        { L_,   "BACDEABC",          8, { VB, VA, VC, VD, VE,
                                          VA, VB, VC }          },
        { L_,   "CBADEABCD",         9, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD }      },
        { L_,   "CBADEABCDAB",      11, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB }                  },
        { L_,   "CBADEABCDABC",     12, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC }              },
        { L_,   "CBADEABCDABCDE",   14, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE }      },
        { L_,   "CBADEABCDABCDEA",  15, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE, VA }  },
        { L_,   "CBADEABCDABCDEAB", 16, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE, VA,
                                          VB }                  },
        { L_,   "CBADEABCDABCDEABCBADEABCDABCDEA", 31,
                                        { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE, VA,
                                          VB, VC, VB, VA, VD,
                                          VE, VA, VB, VC, VD,
                                          VA, VB, VC, VD, VE,
                                          VA }                  },
        { L_,   "CBADEABCDABCDEABCBADEABCDABCDEAB", 32,
                                        { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE, VA,
                                          VB, VC, VB, VA, VD,
                                          VE, VA, VB, VC, VD,
                                          VA, VB, VC, VD, VE,
                                          VA, VB }              }
    };

    enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

    if (verbose) printf("Testing is_mutable.\n");
    {
        TYPE        mV;
        TYPE&       mVref = mV;
        const TYPE& cmVref = mV;

        ASSERT(  is_mutable(mV));
        ASSERT(  is_mutable(mVref));
        ASSERT(! is_mutable(cmVref));
    }

    if (verbose) printf("Testing const and non-const versions of "
                        "begin() and end().\n");
    {
        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_lineNum;
            const char *SPEC   = DATA[ti].d_spec_p;
            const int   LENGTH = DATA[ti].d_length;
            const int  *EXP    = DATA[ti].d_elements;

            ASSERT(LENGTH <= MAX_LENGTH);

            Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

            ASSERTV(ti, LENGTH == static_cast<int>(X.size()));
            ASSERTV(ti, (LENGTH == 0) == X.empty());

            if (veryVerbose) {
                printf( "\ton objects of length %d:\n", LENGTH);
            }

            // non-decreasing
            ASSERTV(LINE, oldLen <= LENGTH);
            oldLen = LENGTH;

            if (veryVerbose) printf("\t\tSpec = \"%s\"", SPEC);

            if (veryVerbose) {
                T_; T_; T_; P(X);
            }

            int            i;
            iterator       imX;
            const_iterator iX;
            for (i = 0, imX = mX.begin(), iX = X.begin(); i < LENGTH;
                                                        ++i, ++imX, ++iX) {
                TYPE v;
                assignTo(bsls::Util::addressOf(v), EXP[i]);

                ASSERTV(LINE, i, iX  != X.end());
                ASSERTV(LINE, i, imX != mX.end());
                ASSERTV(LINE, i, imX == iX);
                ASSERTV(LINE, i, v == *iX);
                ASSERTV(LINE, i, v == *imX);
                ASSERTV(LINE, i, succ(X.begin(),i) == iX);
                ASSERTV(LINE, i, succ(mX.begin(),i) == imX);
                ASSERTV(LINE, i, bsls::Util::addressOf(nthElem(X,i)) ==
                                               bsls::Util::addressOf(*iX));
                ASSERTV(LINE, i, bsls::Util::addressOf(nthElem(mX,i)) ==
                                              bsls::Util::addressOf(*imX));
            }
            ASSERTV(LINE, i, iX  == X.end());
            ASSERTV(LINE, i, imX == mX.end());

            // Sanity check that the test driver doesn't have unused data:
            for (; i < MAX_LENGTH; ++i) {
                ASSERTV(LINE, i, 0 == EXP[i]);
            }
        }
    }

    if (verbose) printf("Testing non-const iterators "
                        "modify state of object correctly.\n");
    {
        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const size_t  LENGTH = DATA[ti].d_length;
            const int    *EXP    = DATA[ti].d_elements;

            Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

            ASSERTV(ti, LENGTH == X.size()); // same lengths
            ASSERTV(ti, (LENGTH == 0) == X.empty());

            if (veryVerbose) {
                printf("\tOn objects of length " ZU ":\n", LENGTH);
            }

            // non-decreasing

            ASSERTV(LINE, oldLen <= (int)LENGTH);
            oldLen = static_cast<int>(LENGTH);

            if (veryVerbose) printf( "\t\tSpec = \"%s\"", SPEC);

            if (veryVerbose) {
                T_; T_; T_; P(X);
            }

            Obj mY(xoa);  const Obj& Y = mY;

            for (size_t j = 0; j < LENGTH; j++) {
                primaryManipulator(&mY, 0);
            }

            // Change state of Y so its same as X

            size_t j = 0;
            for (iterator imY = mY.begin(); imY != mY.end(); ++imY, ++j) {
                assignTo(bsls::Util::addressOf(*imY), EXP[j]);
            }
            ASSERTV(ti, LENGTH == j);

            if (veryVerbose) {
                printf("\t\tNew object1: "); P(Y);
            }

            const_iterator iX;
            const_iterator iY;
            for (j = 0, iX = X.begin(), iY = Y.begin(); iX != X.end();
                                                         ++j, ++iX, ++iY) {
                ASSERTV(ti, j, *iY == *iX);
            }
            ASSERTV(ti, iY == Y.end());

            // Just for kicks, use the (untested) operator==
            ASSERTV(ti, Y == X);
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::test03_generatorGG()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMITIVE GENERATOR FUNCTIONS 'gg' AND 'ggg':
    //
    // Concerns:
    //: 1 Having demonstrated that our primary manipulators work as expected
    //:   under normal conditions, we want to verify that:
    //:   o Valid generator syntax produces expected results.
    //:   o Invalid syntax is detected and reported.
    //
    // Plan:
    //: 1 For each of an enumerated sequence of 'spec' values, ordered by
    //:   increasing 'spec' length, use the primitive generator function 'gg'
    //:   to set the state of a newly created object.  Verify that 'gg' returns
    //:   a valid reference to the modified argument object and, using basic
    //:   accessors, that the value of the object is as expected.  Repeat the
    //:   test for a longer 'spec' generated by prepending a string ending in a
    //:   '~' character (denoting 'clear').  Note that we are testing the
    //:   parser only; the primary manipulators are already assumed to work.
    //:
    //: 2 For each of an enumerated sequence of 'spec' values, ordered by
    //:   increasing 'spec' length, use the primitive generator function 'ggg'
    //:   to set the state of a newly created object.  Verify that 'ggg'
    //:   returns the expected value corresponding to the location of the first
    //:   invalid value of the 'spec'.  Repeat the test for a longer 'spec'
    //:   generated by prepending a string ending in a '~' character (denoting
    //:   'clear').
    //
    // Testing:
    //   list<T,A>& gg(list<T,A> *object, const char *spec);
    //   int ggg(list<T,A> *object, const char *spec, int vF = 1);
    // ------------------------------------------------------------------------

    if (verbose) printf("\tTesting type: %s (ALLOC = %d).\n",
                        NameOf<TYPE>().name(), SCOPED_ALLOC);

    bslma::TestAllocator oa("object", veryVeryVerbose);
    const ALLOC          xoa(&oa);

    {
        enum { MAX_LENGTH = 10 };
        static const struct {
            int         d_lineNum;               // source line number
            const char *d_spec_p;                // specification string
            int         d_length;                // expected length
            int         d_elements[MAX_LENGTH];  // expected element values
        } DATA[] = {
            //line  spec            length  elements
            //----  --------------  ------  ----------------------
            { L_,   "",             0,      { 0 }                   },

            { L_,   "A",            1,      { VA }                  },
            { L_,   "B",            1,      { VB }                  },
            { L_,   "~",            0,      { 0 }                   },

            { L_,   "CD",           2,      { VC, VD }              },
            { L_,   "E~",           0,      { 0 }                   },
            { L_,   "~E",           1,      { VE }                  },
            { L_,   "~~",           0,      { 0 }                   },

            { L_,   "ABC",          3,      { VA, VB, VC }          },
            { L_,   "~BC",          2,      { VB, VC }              },
            { L_,   "A~C",          1,      { VC }                  },
            { L_,   "AB~",          0,      { 0 }                   },
            { L_,   "~~C",          1,      { VC }                  },
            { L_,   "~B~",          0,      { 0 }                   },
            { L_,   "A~~",          0,      { 0 }                   },
            { L_,   "~~~",          0,      { 0 }                   },

            { L_,   "ABCD",         4,      { VA, VB, VC, VD }      },
            { L_,   "~BCD",         3,      { VB, VC, VD }          },
            { L_,   "A~CD",         2,      { VC, VD }              },
            { L_,   "AB~D",         1,      { VD }                  },
            { L_,   "ABC~",         0,      { 0 }                   },

            { L_,   "ABCDE",        5,      { VA, VB, VC, VD, VE }  },
            { L_,   "~BCDE",        4,      { VB, VC, VD, VE }      },
            { L_,   "AB~DE",        2,      { VD, VE }              },
            { L_,   "ABCD~",        0,      { 0 }                   },
            { L_,   "A~C~E",        1,      { VE }                  },
            { L_,   "~B~D~",        0,      { 0 }                   },

            { L_,   "~CBA~~ABCDE",  5,      { VA, VB, VC, VD, VE }  },

            { L_,   "ABCDE~CDEC~E", 1,      { VE }                  }
        };
        enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const size_t  LENGTH = DATA[ti].d_length;
            const int    *EXP    = DATA[ti].d_elements;
            const int     curLen = static_cast<int>(std::strlen(SPEC));

            Obj mW(xoa);  const Obj& W = mW;

            ASSERT(0 > ggg(&mW, SPEC));

            Obj mU(xoa);  const Obj& U = gg(&mU, SPEC);  // original spec

            ASSERT(&U == &mU);

            static const char *const MORE_SPEC = "~ABCDEABCDEABCDEABCDE~";

            char buf[100];
            std::strcpy(buf, MORE_SPEC);
            std::strcat(buf, SPEC);

            Obj mV(xoa);  const Obj& V = gg(&mV, buf);   // extended spec

            if (curLen != oldLen) {
                if (verbose) printf("\tof length %d:\n", curLen);
                ASSERTV(LINE, oldLen <= curLen);         // non-decreasing
                oldLen = curLen;
            }

            if (veryVerbose) {
                printf("\t\tSpec = \"%s\"\n", SPEC);
                printf("\t\tBigSpec = \"%s\"\n", buf);
                T_; T_; T_; P(U);
                T_; T_; T_; P(V);
            }

            ASSERTV(LINE, LENGTH == U.size());
            ASSERTV(LINE, LENGTH == V.size());

            const_iterator iu = U.begin();
            const_iterator iv = V.begin();
            const_iterator iw = W.begin();
            for (size_t i = 0; i < LENGTH; ++i, ++iu, ++iv, ++iw) {
                TYPE element;
                assignTo(bsls::Util::addressOf(element), EXP[i]);

                ASSERTV(LINE, i, element == *iu);
                ASSERTV(LINE, i, element == *iv);
                ASSERTV(LINE, i, element == *iw);
            }
        }
    }

    if (verbose) printf("Testing generator on invalid specs.\n");
    {
        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_index;    // offending character index
        } DATA[] = {
            //line  spec     index
            //----  -------  -----
            { L_,   "",      -1,  }, // control

            { L_,   "~",     -1,  }, // control
            { L_,   " ",      0,  },
            { L_,   ".",      0,  },
            { L_,   "E",     -1,  }, // control
            { L_,   "I",      0,  },
            { L_,   "Z",      0,  },

            { L_,   "AE",    -1,  }, // control
            { L_,   "aE",     0,  },
            { L_,   "Ae",     1,  },
            { L_,   ".~",     0,  },
            { L_,   "~!",     1,  },
            { L_,   "  ",     0,  },

            { L_,   "ABC",   -1,  }, // control
            { L_,   " BC",    0,  },
            { L_,   "A C",    1,  },
            { L_,   "AB ",    2,  },
            { L_,   "?#:",    0,  },
            { L_,   "   ",    0,  },

            { L_,   "ABCDE", -1,  }, // control
            { L_,   "aBCDE",  0,  },
            { L_,   "ABcDE",  2,  },
            { L_,   "ABCDe",  4,  },
            { L_,   "AbCdE",  1,  }
        };
        enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const int     INDEX  = DATA[ti].d_index;
            const size_t  LENGTH = std::strlen(SPEC);

            Obj mX(xoa);

            if (static_cast<int>(LENGTH) != oldLen) {
                if (verbose) printf("\tof length " ZU ":\n", LENGTH);
                ASSERTV(LINE, oldLen <= (int)LENGTH);  // non-decreasing
                oldLen = static_cast<int>(LENGTH);
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int result = ggg(&mX, SPEC, veryVerbose);

            ASSERTV(LINE, INDEX == result);
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::test02_primaryManipulators()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
    //
    // Concerns:
    //: 1 The default constructor
    //:   o creates the correct initial value.
    //:   o allocates exactly one block.
    //:
    //: 2 The destructor properly deallocates all allocated memory to its
    //:   corresponding allocator from any attainable state.
    //:
    //: 3 'primaryManipulator' and 'primaryCopier'
    //:   o produces the expected value.
    //:   o maintains valid internal state.
    //:   o preserves the strong exception guarantee and is exception-neutral
    //:     with respect to memory allocation.
    //:   o does not change the address of any other list elements
    //:   o has the internal memory management system hooked up properly so
    //:     that *all* internally allocated memory draws from the same
    //:     user-supplied allocator whenever one is specified.
    //:
    //: 4 'clear'
    //:   o produces the expected value (empty).
    //:   o properly destroys each contained element value.
    //:   o maintains valid internal state.
    //:   o does not allocate memory.
    //:   o deallocates all element memory
    //:
    //: 5 The size-based parameters of the class reflect the platform.
    //
    // Plan:
    //: 1 To address concerns 1a - 1c, create an object using the default
    //:   constructor:
    //:   o With and without passing in an allocator.
    //:   o In the presence of exceptions during memory allocations using a
    //:     'bslma_TestAllocator' and varying its *allocation* *limit*.
    //:   o Where the object is constructed with an object allocator and
    //:     neither of global and default allocator is used to supply memory.
    //:
    //: 2 To address concerns 3a - 3e, construct a series of independent
    //:   objects, ordered by increasing length.  In each test, allow the
    //:   object to leave scope without further modification, so that the
    //:   destructor asserts internal object invariants appropriately.  After
    //:   the final insert operation in each test, use the (untested) basic
    //:   accessors to cross-check the value of the object and the
    //:   'bslma_TestAllocator' to confirm whether memory allocation has
    //:   occurred.
    //:
    //: 3 To address concerns 4a-4e, construct a similar test, replacing
    //:   'primaryCopier' with 'clear'; this time, however, use the test
    //:   allocator to record *numBlocksInUse* rather than *numBlocksTotal*.
    //:
    //: 4 To address concerns 2, 3d, 4d, create a small "area" test that
    //:   exercises the construction and destruction of objects of various
    //:   lengths in the presence of memory allocation exceptions.  Two
    //:   separate tests will be performed:
    //..
    //     Let S be the sequence of integers { 0 .. N - 1 }.
    //     (1) for each i in S, use the default constructor and
    //         'primaryCopier/primaryManipulator' to create an instance
    //         of length i, confirm its value (using basic accessors), and
    //         let it leave scope.
    //     (2) for each (i, j) in S X S, use
    //         'primaryCopier/primaryManipulator' to create an instance of
    //         length i, use 'clear' to clear its value and confirm (with
    //         'length'), use insert to set the instance to a value of
    //         length j, verify the value, and allow the instance to leave
    //         scope.
    //..
    //: 5 The first test acts as a "control" in that 'clear' is not called; if
    //:   only the second test produces an error, we know that 'clear' is to
    //:   blame.  We will rely on 'bslma_TestAllocator' and purify to address
    //:   concern 2, and on the object invariant assertions in the destructor
    //:   to address concerns 3d and 4d.
    //:
    //: 6 To address concern 5, the values will be explicitly compared to the
    //:   expected values.  This will be done first so as to ensure all other
    //:   tests are reliable and may depend upon the class's constants.
    //
    // Testing:
    //   list<T,A>(const A& a = A());
    //   ~list<T,A>(); void
    //   primaryManipulator(list<T,A>*, int); void
    //   primaryCopier(list<T,A>*,const T&); void clear();
    // ------------------------------------------------------------------------

    if (verbose) printf("\tTesting type: %s (ALLOC = %d).\n",
                        NameOf<TYPE>().name(), SCOPED_ALLOC);

    TestValues VALUES("ABCDEFGH");
    const int  NUM_VALUES = 8;

    const char   *spec = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const size_t  numSpecValues = std::strlen(spec);


    if (verbose) printf("\tTesting default ctor (thoroughly).\n");

    if (verbose) printf("\t\tWithout passing in an allocator.\n");
    {

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        const Obj X;

        const Int64 AA = da.numBlocksTotal();
        const Int64 A  = da.numBlocksInUse();

        if (veryVerbose) { T_; T_; P(X); }

        ASSERTV(X.size(), 0 == X.size());
        ASSERTV(&da == X.get_allocator().mechanism());
        ASSERTV(AA, expectedBlocks(0) == AA);
        ASSERTV(A,  expectedBlocks(0) == A);
    }

    if (verbose) printf("\t\tPassing in an allocator.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        const ALLOC          xoa(&oa);

        const Obj mX(xoa);  const Obj& X = mX;

        const Int64 AA = oa.numBlocksTotal();
        const Int64 A  = oa.numBlocksInUse();

        if (veryVerbose) { T_; T_; P(X); }

        ASSERTV(X.size(), 0 == X.size());
        ASSERTV(xoa == X.get_allocator());
        ASSERTV(AA, expectedBlocks(0) == AA);
        ASSERTV(A,  expectedBlocks(0) == A);
    }

    // ------------------------------------------------------------------------
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    const ALLOC          xoa(&oa);

    if (verbose) printf(
          "\tTesting 'primaryManipulator' (bootstrap) without allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX;  const Obj& X = mX;

            const TYPE *elemAddrs[NUM_TRIALS];

            ASSERT(mX.get_allocator().mechanism() == defaultAllocator_p);

            for (size_t i = 0; i < li; ++i) {
                primaryManipulator(&mX, spec[i % numSpecValues]);
                elemAddrs[i] = bsls::Util::addressOf(X.back());
            }

            ASSERTV(li, li == X.size());

            if (veryVerbose){
                printf("\t\t\tBEFORE: "); P(X);
            }

            primaryManipulator(&mX, spec[li % numSpecValues]);
            elemAddrs[li] = bsls::Util::addressOf(X.back());

            if (veryVerbose){
                printf("\t\t\tAFTER: "); P(X);
            }

            ASSERTV(li, li + 1 == X.size());

            const_iterator it = X.begin();
            for (unsigned i = 0; i < li; ++it, ++i) {
                ASSERTV(li, i, spec[i] == value_of(*it));
                ASSERTV(li, i, elemAddrs[i] == bsls::Util::addressOf(*it));
            }

            ASSERTV(li, spec[li] == value_of(*it));
            ASSERTV(li, elemAddrs[li] == bsls::Util::addressOf(*it));
        }
    }

    // ------------------------------------------------------------------------

    if (verbose)
        printf("\tTesting 'primaryCopier' (bootstrap) without allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX;  const Obj& X = mX;

            const TYPE *elemAddrs[NUM_TRIALS];

            ASSERT(mX.get_allocator().mechanism() == defaultAllocator_p);

            for (size_t i = 0; i < li; ++i) {
                primaryCopier(&mX, VALUES[i % NUM_VALUES]);
                elemAddrs[i] = bsls::Util::addressOf(X.back());
            }

            ASSERTV(li, li == X.size());

            if (veryVerbose){
                printf("\t\t\tBEFORE: "); P(X);
            }

            primaryCopier(&mX, VALUES[li % NUM_VALUES]);
            elemAddrs[li] = bsls::Util::addressOf(X.back());

            if (veryVerbose){
                printf("\t\t\tAFTER: "); P(X);
            }

            ASSERTV(li, li + 1 == X.size());

            const_iterator it = X.begin();
            for (unsigned i = 0; i < li; ++it, ++i) {
                ASSERTV(li, i, VALUES[i % NUM_VALUES] == *it);
                ASSERTV(li, i, elemAddrs[i] == bsls::Util::addressOf(*it));
            }

            ASSERTV(li, VALUES[li % NUM_VALUES] == *it);
            ASSERTV(li, elemAddrs[li] == bsls::Util::addressOf(*it));
        }
    }

    // ------------------------------------------------------------------------

    if (verbose) printf(
             "\tTesting 'primaryManipulator' (bootstrap) with allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj         mX(xoa);  const Obj& X = mX;
            const TYPE *elemAddrs[NUM_TRIALS];

            for (size_t i = 0; i < li; ++i) {
                primaryManipulator(&mX, spec[i % numSpecValues]);
                elemAddrs[i] = bsls::Util::addressOf(X.back());
            }

            ASSERTV(li, li == X.size());

            const Int64 BB = oa.numBlocksTotal();
            const Int64 B  = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B); P(X);
            }

            primaryManipulator(&mX, spec[li % numSpecValues]);
            elemAddrs[li] = bsls::Util::addressOf(X.back());

            const Int64 AA = oa.numBlocksTotal();
            const Int64 A  = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t AFTER: ");
                P_(AA); P_(A); P(X);
            }

            ASSERTV(li, NameOf<TYPE>(), BB, deltaBlocks(1),
                    AA, BB + deltaBlocks(1) == AA);

            ASSERTV(li, B + deltaBlocks(1) == A);

            ASSERTV(li, li + 1 == X.size());

            const_iterator it = X.begin();
            for (unsigned i = 0; i < li; ++it, ++i) {
                ASSERTV(li, i, spec[i % numSpecValues] == value_of(*it));
                ASSERTV(li, i, elemAddrs[i] == bsls::Util::addressOf(*it));
            }

            ASSERTV(li, spec[li % numSpecValues] == value_of(*it));
            ASSERTV(li, elemAddrs[li] == bsls::Util::addressOf(*it));
        }
    }

    // ------------------------------------------------------------------------

    if (verbose) printf(
                  "\tTesting 'primaryCopier' (bootstrap) with allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj         mX(xoa);  const Obj& X = mX;
            const TYPE *elemAddrs[NUM_TRIALS];

            for (size_t i = 0; i < li; ++i) {
                primaryCopier(&mX, VALUES[i % NUM_VALUES]);
                elemAddrs[i] = bsls::Util::addressOf(X.back());
            }

            ASSERTV(li, li == X.size());

            const Int64 BB = oa.numBlocksTotal();
            const Int64 B  = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B); P(X);
            }

            primaryCopier(&mX, VALUES[li % NUM_VALUES]);
            elemAddrs[li] = bsls::Util::addressOf(X.back());

            const Int64 AA = oa.numBlocksTotal();
            const Int64 A  = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t AFTER: ");
                P_(AA); P_(A); P(X);
            }

            ASSERTV(li, NameOf<TYPE>(), BB, deltaBlocks(1), AA,
                    BB + deltaBlocks(1) == AA);

            ASSERTV(li, B + deltaBlocks(1) == A);

            ASSERTV(li, li + 1 == X.size());

            const_iterator it = X.begin();
            for (unsigned i = 0; i < li; ++it, ++i) {
                ASSERTV(li, i, VALUES[i % NUM_VALUES] == *it);
                ASSERTV(li, i, elemAddrs[i] == bsls::Util::addressOf(*it));
            }

            ASSERTV(li, VALUES[li % NUM_VALUES] == *it);
            ASSERTV(li, elemAddrs[li] == bsls::Util::addressOf(*it));
        }
    }

    // ------------------------------------------------------------------------

    if (verbose) printf("\tTesting 'clear' without allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX;  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                primaryCopier(&mX, VALUES[i % NUM_VALUES]);
            }

            if (veryVerbose){
                printf("\t\t\tBEFORE "); P(X);
            }

            ASSERTV(li, li == X.size());

            mX.clear();

            if (veryVerbose){
                printf("\t\t\tAFTER "); P(X);
            }

            ASSERTV(li, 0 == X.size());

            for (size_t i = 0; i < li; ++i) {
                primaryCopier(&mX, VALUES[i % NUM_VALUES]);
            }

            if (veryVerbose){
                printf("\t\t\tAFTER SECOND INSERT "); P(X);
            }

            ASSERTV(li, li == X.size());
            const_iterator it = X.begin();
            for (unsigned i = 0; i < li; ++it, ++i) {
                ASSERTV(li, i, VALUES[i % NUM_VALUES] == *it);
            }
        }
    }

    // ------------------------------------------------------------------------

    if (verbose) printf("\tTesting 'clear' with allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX(xoa);  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                primaryCopier(&mX, VALUES[i % NUM_VALUES]);
            }

            ASSERTV(li, li == X.size());

            const Int64 BB = oa.numBlocksTotal();
            const Int64 B  = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B); P(X);
            }

            mX.clear();

            const Int64 AA = oa.numBlocksTotal();
            const Int64 A  = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tAFTER: ");
                P_(AA); P_(A); P(X);
            }

            for (size_t i = 0; i < li; ++i) {
                primaryCopier(&mX, VALUES[i % NUM_VALUES]);
            }

            ASSERTV(li, li == X.size());

            const Int64 CC = oa.numBlocksTotal();
            const Int64 C  = oa.numBlocksInUse();

            if (veryVerbose){
                printf("\t\t\tAFTER SECOND INSERT: ");
                P_(CC); P_(C); P(X);
            }

            ASSERTV(li, li == X.size());

            ASSERTV(li, BB == AA);
            ASSERTV(li, B - deltaBlocks(li) == A);

            ASSERTV(li, BB + deltaBlocks(li) == CC);
            ASSERTV(li, B == C);
        }
    }

    ASSERTV(oa.numBlocksInUse(), 0 == oa.numBlocksInUse());

    // ------------------------------------------------------------------------

    if (verbose) printf("\tTesting the destructor and exception neutrality "
                        "with allocator.\n");

    if (verbose) printf("\t\tWith 'primaryCopier' only\n");
    {
        // For each lengths li up to some modest limit:
        //    1) create an instance
        //    2) insert { V0, V1, V2, V3, V4, V0, ... }  up to length li
        //    3) verify initial length and contents
        //    4) allow the instance to leave scope
        //    5) make sure that the destructor cleans up

        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;
        for (size_t li = 0; li < NUM_TRIALS; ++li) { // i is the length
            if (verbose)
                printf("\t\t\tOn an object of length " ZU ".\n", li);

          int         throws = -1;
          const Int64 beforeInUse = oa.numBlocksInUse();
          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
            ++throws;

            if (oa.numBlocksInUse() != beforeInUse) {
                int jj = 0;
                ++jj;    // someplace to break point
            }

            Obj mX(xoa);  const Obj& X = mX;                        // 1.

            const TYPE *elemAddrs[NUM_TRIALS];

            for (size_t i = 0; i < li; ++i) {                       // 2.
                ExceptionProctor proctor(&mX, L_);
                primaryCopier(&mX, VALUES[i % NUM_VALUES]);
                elemAddrs[i] = bsls::Util::addressOf(X.back());
                proctor.release();
            }

            ASSERTV(li, li == X.size());                            // 3.
            const_iterator it = X.begin();

            for (unsigned i = 0; i < li; ++it, ++i) {
                ASSERTV(li, i, VALUES[i % NUM_VALUES] == *it);
                ASSERTV(li, i, elemAddrs[i] == bsls::Util::addressOf(*it));
            }
          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END                  // 4.
          ASSERTV(li, oa.numBlocksInUse() - beforeInUse,
                  beforeInUse == oa.numBlocksInUse());              // 5.
        }
    }

    if (verbose) printf("\t\tWith 'primaryCopier' and 'clear'\n");
    {
        // For each pair of lengths (i, j) up to some modest limit:
        //    1) create an instance
        //    2) insert V0 values up to a length of i
        //    3) verify initial length and contents
        //    4) clear contents from instance
        //    5) verify length is 0
        //    6) insert { V0, V1, V2, V3, V4, V0, ... }  up to length j
        //    7) verify new length and contents
        //    8) allow the instance to leave scope
        //    9) make sure that the destructor cleans up

        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;
        for (size_t i = 0; i < NUM_TRIALS; ++i) { // i is first length
            if (verbose)
                printf("\t\t\tOn an object of initial length " ZU ".\n", i);

            for (size_t j = 0; j < NUM_TRIALS; ++j) { // j is second length
                if (veryVerbose)
                    printf("\t\t\t\tAnd with final length " ZU ".\n", j);

                size_t k; // loop index

                Obj mX(xoa);  const Obj& X = mX;                    // 1.

                const TYPE *elemAddrs[NUM_TRIALS];

                for (k = 0; k < i; ++k) {                           // 2.
                    ExceptionProctor proctor(&mX, L_);
                    primaryCopier(&mX, VALUES[0]);
                    elemAddrs[k] = bsls::Util::addressOf(X.back());
                    proctor.release();
                }

                ASSERTV(i, j, i == X.size());                       // 3.
                const_iterator it = X.begin();
                for (k = 0; k < i; ++k, ++it) {
                    ASSERTV(i, j, k, VALUES[0] == *it);
                    ASSERTV(i, j, k,
                                   elemAddrs[k] == bsls::Util::addressOf(*it));
                }

                // verify clear doesn't throw

                int numThrows = -1;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                  ++numThrows;
                  mX.clear();                                       // 4.
                  ASSERTV(i, j, 0 == X.size());                     // 5.
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END            // 8.
                ASSERT(0 == numThrows);
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::test01_breathingTest()
{
    // ------------------------------------------------------------------------
    // BREATHING TEST:
    //
    // Concern:
    //: 1 We want to exercise basic value-semantic functionality.  In
    //:   particular we want to demonstrate a base-line level of correct
    //:   operation of the following methods and operators:
    //:   o default and copy constructors (and also the destructor)
    //:   o the assignment operator (including aliasing)
    //:   o equality operators: 'operator==' and 'operator!='
    //:   o primary manipulators: 'push_back' and 'clear' methods
    //:   o basic accessors: 'size' and 'operator[]'
    //
    // Plan:
    //: 1 Create four objects using both the default and copy constructors.
    //:
    //: 2 Exercise these objects using
    //:   o primary manipulators
    //:   o basic accessors
    //:   o equality operators
    //:   o the assignment operator
    //:
    //: 3 Try aliasing with assignment for a non-empty instance [11] and allow
    //:   the result to leave scope, enabling the destructor to assert internal
    //:   object invariants.
    //:
    //: 4 Display object values frequently in verbose mode:
    //..
    // 1) Create an object x1 (default ctor).         { x1: }
    // 2) Create a second object x2 (copy from x1).   { x1: x2: }
    // 3) Prepend an element value A to x1).          { x1:A x2: }
    // 4) Append the same element value A to x2).     { x1:A x2:A }
    // 5) Prepend/Append two values B & C to x2).     { x1:A x2:BAC }
    // 6) Remove all elements from x1.                { x1: x2:BAC }
    // 7) Create a third object x3 (default ctor).    { x1: x2:BAC x3: }
    // 8) Create a forth object x4 (copy of x2).      { x1: x2:BAC x3: x4:BAC }
    // 9) Assign x2 = x1 (non-empty becomes empty).   { x1: x2: x3: x4:BAC }
    // 10) Assign x3 = x4 (empty becomes non-empty).  { x1: x2: x3:BAC x4:BAC }
    // 11) Assign x4 = x4 (aliasing).                 { x1: x2: x3:BAC x4:BAC }
    //..
    //
    // Testing:
    //   This "test" *exercises* basic functionality.
    // ------------------------------------------------------------------------

    TestValues   VALUES("ABCDEFGH");
    const TYPE&  A = VALUES[0];
    const TYPE&  B = VALUES[1];
    const TYPE&  C = VALUES[2];

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf(" 1) Create an object x1 (default ctor)."
                        "\t\t\t{ x1: }\n");

    Obj mX1(&oa);  const Obj& X1 = mX1;

    if (verbose) { T_;  P(X1); }

    if (verbose) printf("\ta) Check initial state of x1.\n");

    ASSERT(0 == X1.size());

    if (verbose) printf("\tb) Try equality operators: x1 <op> x1.\n");

    ASSERT(  X1 == X1);
    ASSERT(!(X1 != X1));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf(" 2) Create a second object x2 (copy from x1)."
                         "\t\t{ x1: x2: }\n");

    Obj mX2(X1, &oa);  const Obj& X2 = mX2;

    if (verbose) { T_;  P(X2); }

    if (verbose) printf("\ta) Check the initial state of x2.\n");

    ASSERT(0 == X2.size());

    if (verbose) printf("\tb) Try equality operators: x2 <op> x1, x2.\n");

    ASSERT(  X2 == X1 );
    ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );
    ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf(" 3) Prepend an element value A to x1)."
                        "\t\t\t{ x1:A x2: }\n");
    mX1.push_front(A);
    if (verbose) { T_;  P(X1); }

    if (verbose) printf("\ta) Check new state of x1.\n");

    ASSERT(1 == X1.size());
    ASSERT(A == X1.front());
    ASSERT(A == X1.back());

    if (verbose) printf("\tb) Try equality operators: x1 <op> x1, x2.\n");

    ASSERT(  X1 == X1 );
    ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));
    ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf(" 4) Append the same element value A to x2)."
                         "\t\t{ x1:A x2:A }\n");
    mX2.push_back(A);

    if (verbose) { T_;  P(X2); }

    if (verbose) printf("\ta) Check new state of x2.\n");

    ASSERT(1 == X2.size());
    ASSERT(A == X2.front());
    ASSERT(A == X2.back());

    if (verbose) printf("\tb) Try equality operators: x2 <op> x1, x2.\n");

    ASSERT(  X2 == X1 );
    ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );
    ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf(" 5) Prepend another element value B and append "
                        "another element c to x2).\t\t{ x1:A x2:BAC }\n");
    mX2.push_front(B);
    mX2.push_back(C);

    if (verbose) { T_;  P(X2); }

    if (verbose) printf("\ta) Check new state of x2.\n");

    ASSERT(3 == X2.size());
    ASSERT(B == X2.front());
    ASSERT(A == nthElem(X2, 1));
    ASSERT(C == X2.back());

    if (verbose) printf("\tb) Try equality operators: x2 <op> x1, x2.\n");

    ASSERT(!(X2 == X1));
    ASSERT(  X2 != X1 );
    ASSERT(  X2 == X2 );
    ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf(" 6) Remove all elements from x1."
                         "\t\t\t{ x1: x2:BAC }\n");
    mX1.clear();

    if (verbose) { T_;  P(X1); }

    if (verbose) printf("\ta) Check new state of x1.\n");

    ASSERT(0 == X1.size());

    if (verbose) printf("\tb) Try equality operators: x1 <op> x1, x2.\n");

    ASSERT(  X1 == X1 );
    ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));
    ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf(" 7) Create a third object x3 (default ctor)."
                         "\t\t{ x1: x2:BAC x3: }\n");

    Obj mX3(&oa);  const Obj& X3 = mX3;

    if (verbose) { T_;  P(X3); }

    if (verbose) printf("\ta) Check new state of x3.\n");
    ASSERT(0 == X3.size());

    if (verbose) printf("\tb) Try equality operators: x3 <op> x1, x2, x3.\n");
    ASSERT(  X3 == X1 );
    ASSERT(!(X3 != X1));
    ASSERT(!(X3 == X2));
    ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );
    ASSERT(!(X3 != X3));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf(" 8) Create a forth object x4 (copy of x2)."
                         "\t\t{ x1: x2:BAC x3: x4:BAC }\n");

    Obj mX4(X2, &oa);  const Obj& X4 = mX4;

    if (verbose) { T_;  P(X4); }

    if (verbose) printf("\ta) Check new state of x4.\n");

    ASSERT(3 == X4.size());
    ASSERT(B == nthElem(X4, 0));
    ASSERT(A == nthElem(X4, 1));
    ASSERT(C == nthElem(X4, 2));

    if (verbose) printf("\tb) Try equality operators: x4 <op> x1 .. x4.\n");
    ASSERT(!(X4 == X1));
    ASSERT(  X4 != X1 );
    ASSERT(  X4 == X2 );
    ASSERT(!(X4 != X2));
    ASSERT(!(X4 == X3));
    ASSERT(  X4 != X3 );
    ASSERT(  X4 == X4 );
    ASSERT(!(X4 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf(" 9) Assign x2 = x1 (non-empty becomes empty)."
                         "\t\t{ x1: x2: x3: x4:BAC }\n");

    mX2 = X1;

    if (verbose) { T_;  P(X2); }

    if (verbose) printf("\ta) Check new state of x2.\n");

    ASSERT(0 == X2.size());

    if (verbose) printf("\tb) Try equality operators: x2 <op> x1 .. x4.\n");
    ASSERT(  X2 == X1 );
    ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );
    ASSERT(!(X2 != X2));
    ASSERT(  X2 == X3 );
    ASSERT(!(X2 != X3));
    ASSERT(!(X2 == X4));
    ASSERT(  X2 != X4 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("10) Assign x3 = x4 (empty becomes non-empty)."
                         "\t\t{ x1: x2: x3:BAC x4:BAC }\n");

    mX3 = X4;

    if (verbose) { T_;  P(X3); }

    if (verbose) printf("\ta) Check new state of x3.\n");

    ASSERT(3 == X3.size());
    ASSERT(B == nthElem(X3, 0));
    ASSERT(A == nthElem(X3, 1));
    ASSERT(C == nthElem(X3, 2));

    if (verbose) printf("\tb) Try equality operators: x3 <op> x1 .. x4.\n");
    ASSERT(!(X3 == X1));
    ASSERT(  X3 != X1 );
    ASSERT(!(X3 == X2));
    ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );
    ASSERT(!(X3 != X3));
    ASSERT(  X3 == X4 );
    ASSERT(!(X3 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("11) Assign x4 = x4 (aliasing)."
                         "\t\t\t\t{ x1: x2: x3:BAC x4:BAC }\n");

    mX4 = X4;

    if (verbose) { T_;  P(X4); }

    if (verbose) printf("\ta) Check new state of x4.\n");

    ASSERT(3 == X4.size());
    ASSERT(B == nthElem(X4, 0));
    ASSERT(A == nthElem(X4, 1));
    ASSERT(C == nthElem(X4, 2));

    if (verbose) printf("\tb) Try equality operators: x4 <op> x1 .. x4.\n");
    ASSERT(!(X4 == X1));
    ASSERT(  X4 != X1 );
    ASSERT(!(X4 == X2));
    ASSERT(  X4 != X2 );
    ASSERT(  X4 == X3 );
    ASSERT(!(X4 != X3));
    ASSERT(  X4 == X4 );
    ASSERT(!(X4 != X4));

    (void) value_of(A);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? std::atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    // As part of our overall allocator testing strategy, we will create two
    // test allocators.

    // Default Test Allocator.
    bslma::TestAllocator         defaultAllocator("Default Allocator",
                                                  veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);
    defaultAllocator_p = &defaultAllocator;

    // Global Test Allocator.
    bslma::TestAllocator  globalAllocator("Global Allocator",
                                         veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);
    globalAllocator_p = &globalAllocator;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    // check DEFAULT_DATA and associated variables

    {
        bool maxed = false;
        for (int ti = 0; ti < DEFAULT_NUM_DATA; ++ti) {
            const char *pcI  = DEFAULT_DATA[ti].d_spec_p;
            size_t      iLen = std::strlen(pcI);

            ASSERT(iLen <= DEFAULT_MAX_LENGTH);
            if (DEFAULT_MAX_LENGTH == iLen) {
                maxed = true;
            }

            for (int tj = 0; tj < DEFAULT_NUM_DATA; ++tj) {
                const char *pcJ = DEFAULT_DATA[tj].d_spec_p;

                ASSERT((ti == tj) == !std::strcmp(pcI, pcJ));
            }
        }
        ASSERT(maxed);
    }

    switch (test) { case 0:  // Zero is always the leading case.
      case 35: {
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

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

        // 10 data points with 4 stars at or above 75 brightness

        static const char STAR_DATA1[] =
            "-1.21 +0.11 76\n"
            "-1.05 +0.70 39\n"
            "-0.89 +1.03 93\n"
            "-0.61 +0.35 71\n"
            "-0.48 +1.19 61\n"
            "-0.01 +0.85 43\n"
            "+0.37 -0.01 90\n"
            "+0.37 +0.90 78\n"
            "+0.70 +0.48 54\n"
            "+0.91 +1.35 38\n";

        // 8 data points with 3 stars at or above 75 brightness
        // 1 point overlaps STAR_DATA1

        static const char STAR_DATA2[] =
            "-1.40 -0.48 74\n"
            "-0.95 -1.00 40\n"
            "-0.91 -0.21 51\n"
            "-0.51 -0.23 88\n"
            "-0.16 -0.55 30\n"
            "+0.37 -0.01 90\n"
            "+0.48 -0.57 66\n"
            "+0.93 -0.35 75\n";

        // Create test data files
        FILE* output = fopen("star_data1.txt", "w");
        ASSERT(output);
        fprintf(output, "%s", STAR_DATA1);
        fclose(output);

        output = fopen("star_data2.txt", "w");
        ASSERT(output);
        fprintf(output, "%s", STAR_DATA2);
        fclose(output);

        if (verbose) printf("usageExample1\n");
        usageExample1(veryVerbose);

        if (verbose) printf("usageExample2\n");
        usageExample2(veryVerbose);

        // Erase output files.

        remove("star_data1.txt");
        remove("star_data2.txt");
      } break;
      case 34: // falls through
      case 33: // falls through
      case 32: // falls through
      case 31: // falls through
      case 30: // falls through
      case 29: // falls through
      case 28: // falls through
      case 27: // falls through
      case 26: // falls through
      case 25: // falls through
      case 24: // falls through
      case 23: // falls through
      case 22: // falls through
      case 21: // falls through
      case 20: // falls through
      case 19: // falls through
      case 18: // falls through
      case 17: // falls through
      case 16: // falls through
      case 15: // falls through
      case 14: // falls through
      case 13: // falls through
      case 12: {
        if (verbose) printf(
                   "\nTEST CASE %d IS DELEGATED TO 'bslstl_list_test.t.cpp'"
                   "\n=====================================================\n",
                   test);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TEST ALLOCATOR-RELATED CONCERNS
        //
        // Concerns:
        //: 1 For ALLOC that is a bslma_Allocator
        //:
        //:   1 The list class has the 'bslma::UsesBslmaAllocator' trait.
        //:
        //:   2 The allocator is not copied when the list is copy-constructed.
        //:
        //:   3 The allocator is set with the extended copy-constructor.
        //:
        //:   4 The allocator is passed through to elements if the elements
        //:     also use bslma_Allocator.
        //:
        //:   5 Creating an empty list allocates exactly one block.
        //
        // Plan:
        //: 1 Default construct a list and observe that a single allocation
        //:   takes place (for the sentinel).
        //:
        //: 2 If the type contained in the list allocates, push a value onto
        //:   the back of the list and observe that 2 more allocations take
        //:   place.
        //:
        //: 3 Call the 'allocator' accessor on an element in the list and
        //:   observe that the allocator matches that passed to the list.
        //
        // Testing:
        //   Allocator traits and propagation
        //   A get_allocator() const;
        // --------------------------------------------------------------------

        if (verbose) printf("TEST ALLOCATOR-RELATED CONCERNS\n"
                            "===============================\n");

        RUN_EACH_TYPE(TestDriver,
                      test11_allocators,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      T,
                      TNA);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY
        //
        // Concerns:
        //: 1 No test.
        //
        // Plan:
        //: 1 No test.
        //
        // Testing:
        //   No test.
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING STREAMING FUNCTIONALITY\n"
                            "===============================\n");

        if (verbose) printf("There is no streaming for this component.\n");
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING COPY ASSIGNMENT OPERATOR
        //
        // Concerns:
        //: 1 The 'rhs' value must not be affected by the operation.
        //:
        //: 2 'rhs' going out of scope has no effect on the value of 'lhs'
        //:   after the assignment.
        //:
        //: 3 After the assignment, no element of the 'lhs' has the same
        //:   address an element of 'rhs' (except in the case of
        //:   self-assignment).
        //:
        //: 4 Aliasing (x = x): The assignment operator must always work --
        //:   even when the lhs and rhs are identically the same object.
        //:
        //: 5 The assignment operator must be neutral with respect to memory
        //:   allocation exceptions.
        //:
        //: 6 The 'lhs' object must allocate all of its internal memory from
        //:   its own allocator, even of 'rhs' uses a different allocator.
        //:
        //: 7 TBD: C++0x move assignment moves the value but not the allocator.
        //:   Memory is allocated only if the 'lhs' and 'rhs' allocators are
        //:   different.
        //
        // Plan:
        //: 1 Specify a set S of unique object values with substantial and
        //:   varied differences, ordered by increasing length.  For each value
        //:   in S, construct an object x along with a sequence of similarly
        //:   constructed duplicates x1, x2, ..., xN.  The elements within each
        //:   object in S are unique so that re-ordering elements cannot
        //:   preserve equality.  Attempt to affect every aspect of white-box
        //:   state by altering each xi in a unique way.  Let the union of all
        //:   such objects be the set T.
        //:
        //: 2 To address concerns 2, 3, and 5, construct tests u = v for all
        //:   '(u, v)' in T X T.  Using canonical controls 'UU' and 'VV',
        //:   assert before the assignment that 'UU == u', 'VV == v', and
        //:   'v == u' if and only if 'VV == UU'.  After the assignment, assert
        //:   that 'VV == u', 'VV == v', and, for grins, that 'v == u'.  Verify
        //:   that each element in v has a different address from the
        //:   corresponding element in v.  Let v go out of scope and confirm
        //:   that 'VV == u'.  All of these tests are performed within the
        //:   'bslma' exception testing apparatus.  Since the execution time is
        //:   lengthy with exceptions, not every permutation is performed when
        //:   exceptions are tested.  Every permutation is also tested
        //:   separately without exceptions.
        //:
        //: 3 As a separate exercise, we address 4 and 5 by constructing tests
        //:   y = y for all y in T.  Using a canonical control X, we will
        //:   verify that X == y before and after the assignment, again within
        //:   the bslma exception testing apparatus.
        //:
        //: 4 To address concern 5, all these tests are performed on user
        //:   defined types:
        //:
        //:   o With allocator, copyable
        //:
        //:   o With allocator, movable
        //:
        //:   o With allocator, not movable
        //
        // Testing:
        //   list& operator=(const list& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING COPY ASSIGNMENT OPERATOR\n"
                            "================================\n");

        RUN_EACH_TYPE(TestDriver,
                      test09_copyAssignmentOp,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      T,
                      TNA);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      test09_copyAssignmentOp,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        // 'propagate_on_container_copy_assignment' testing

        RUN_EACH_TYPE(TestDriver,
                      test09_propagate_on_container_copy_assignment,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'g'
        //
        // Concerns:
        //: 1 No test.
        //
        // Plan:
        //: 1 No test.
        //
        // Testing:
        //   Obj g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING GENERATOR FUNCTION 'g'\n"
                            "==============================\n");

        if (verbose) printf(
                "The 'g' generator function has been removed as 'bsltf'\n"
                "contains a test type that lacks a copy c'tor.  Also, it is\n"
                "not an essential function.\n");
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTORS
        //
        // Concerns:
        //: 1 The new object's value is the same as that of the original
        //:   object (relying on the equality operator) and created with
        //:   the correct allocator.
        //:
        //: 2 The value of the original object is left unaffected.
        //:
        //: 3 Subsequent changes in or destruction of the source object have
        //:   no effect on the copy-constructed object.
        //:
        //: 4 Subsequent changes ('push_back's) on the created object have no
        //:   effect on the original.
        //:
        //: 5 The object has its internal memory management system hooked up
        //:   properly so that *all* internally allocated memory draws
        //:   from a user-supplied allocator whenever one is specified.
        //:
        //: 6 The function is exception neutral w.r.t. memory allocation.
        //:
        //: 7 An object copied from an rvalue with no allocator, or with a
        //:   matching allocator specified, will leave the copied object
        //:   in a valid, default-constructed state.
        //:   1 No allocator specified to c'tor.
        //:   2 Allocator specified to c'tor.
        //:
        //: 8 An object copied from an rvalue with a non-matching allocator
        //:   supplied will not modify the rvalue.
        //
        // Plan:
        //: 1 Specify a set S of object values with substantial and varied
        //:   differences, ordered by increasing length, to be used in the
        //:   following tests.
        //:
        //: 2 For concerns 1 - 3, for each value in S, initialize objects w and
        //:   x, copy construct y from x and use 'operator==' to verify that
        //:   both x and y subsequently have the same value as w.  Let x go out
        //:   of scope and again verify that w == y.
        //:
        //: 3 For concern 4, for each value in S initialize objects w and x,
        //:   and copy construct y from x.  Change the state of y, by using the
        //:   *primary* *manipulator* 'push_back'.  Using the 'operator!='
        //:   verify that y differs from x and w.
        //:
        //: 4 To address concern 5, we will perform tests performed for concern
        //:   1:
        //:
        //:   o While passing a testAllocator as a parameter to the new object
        //:     and ascertaining that the new object gets its memory from the
        //:     provided testAllocator.  Also perform test for concerns 2 and
        //:     4.
        //:
        //:   o Where the object is constructed with an object allocator, and
        //:     neither of global and default allocator is used to supply
        //:     memory.
        //:
        //: 4 To address concern 6, perform tests for concern 1 performed in
        //:   the presence of exceptions during memory allocations using a
        //:   'bslma_TestAllocator' and varying its *allocation* *limit*.
        //
        // Testing:
        //   list(const list& orig);
        //   list(const list& orig, const A&);
        //   list(list&& orig);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING COPY CONSTRUCTORS\n"
                            "=========================\n");

        RUN_EACH_TYPE(TestDriver,
                      test07_copyCtor,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      T,
                      TNA);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      test07_copyCtor,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        // 'select_on_container_copy_construction' testing

        if (verbose) printf("\nCOPY CONSTRUCTOR: ALLOCATOR PROPAGATION"
                            "\n=======================================\n");

        RUN_EACH_TYPE(TestDriver,
                      test07_select_on_container_copy_construction,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      test07_select_on_container_copy_construction,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //
        // Concerns:
        //: 1 Objects constructed with the same values compare equal.
        //:
        //: 2 Objects constructed such that they have same (logical) value but
        //:   different internal representation (due to the lack or presence of
        //:   an allocator) always compare as equal.  3 Unequal objects are
        //:   always compare as unequal.
        //
        // Plan:
        //: 1 Specify a set A of unique allocators including no allocator.
        //:   Specify a set S of unique object values having various minor or
        //:   subtle differences, ordered by non-decreasing length.  Verify the
        //:   correctness of 'operator==' and 'operator!=' (returning either
        //:   true or false) using all elements '(u, ua, v, va)' of the cross
        //:   product S X A X S X A.
        //
        // Note: Since 'operators==' is implemented in terms of basic
        // accessors, it is sufficient to verify only that a difference in
        // value of any one basic accessor for any two given objects implies
        // inequality.  However, to test that no other internal state
        // information is being considered, we want also to verify that
        // 'operator==' reports true when applied to any two objects whose
        // internal representations may be different yet still represent the
        // same (logical) value.  This is achieved by the 'test06_equalityOp'
        // class method of the test driver template, instantiated for the basic
        // test type.  See that function for a list of concerns and a test
        // plan.
        //
        // Testing:
        //   bool operator==(const list&, const list&);
        //   bool operator!=(const list&, const list&);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING EQUALITY OPERATORS\n"
                            "==========================\n");

        RUN_EACH_TYPE(TestDriver,
                      test06_equalityOp,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      T,
                      TNA,
                      bsltf::MoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      test06_equalityOp,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //
        // Concerns:
        //: 1 No test.
        //
        // Plan:
        //: 1 No test.
        //
        // Testing:
        //   No test.
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING OUTPUT (<<) OPERATOR:\n"
                            "=============================\n");

        if (verbose) {
            printf("There is no 'operator<<' for this component.\n");
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //
        // Concerns:
        //: 1 size() returns the list size.
        //:
        //: 2 begin() == end() if and only if the list is empty
        //:
        //: 3 Iterating from begin() to end() will visit every value in a list
        //:   and only the values in that list.
        //:
        //: 4 Iteration works for both const containers (using const_iterator)
        //:   and non-const containers (using iterator).
        //:
        //: 5 empty() returns true if size() return 0
        //:
        //: 6 The test function 'succ' increments an iterator by n.
        //:
        //: 7 The test function 'nthElem' returns the nth element of a list.
        //:
        //: 8 The test function 'is_mutable' returns true if its argument is a
        //:   mutable lvalue.
        //
        // Plan:
        //: 1 Specify a set S of representative object values ordered by
        //:   increasing length.  For each value w in S, initialize a newly
        //:   constructed object x with w using 'gg' and verify that each basic
        //:   accessor returns the expected result.
        //
        // NOTE: This is not a thorough test of iterators.  This test is only
        // sufficient for using iterators to access the contents of a list in
        // order.
        //
        // Testing:
        //   size_type size() const;
        //   bool empty() const;
        //
        //   iterator succ(iterator);
        //   const_iterator succ(iterator) const;
        //   T& nthElem(list& x, int n);
        //   const T& nthElem(list& x, int n) const;
        //   bool is_mutable(T& value);
        //   bool is_mutable(const T& value);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING BASIC ACCESSORS\n"
                            "=======================\n");

        RUN_EACH_TYPE(TestDriver,
                      test04_basicAccessors,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      T,
                      TNA,
                      bsltf::MoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      test04_basicAccessors,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE GENERATOR FUNCTIONS 'gg' AND 'ggg'
        //
        // Concerns:
        //: 1 Having demonstrated that our primary manipulators work as
        //:   expected under normal conditions, we want to verify that:
        //:   o Valid generator syntax produces expected results.
        //:   o Invalid syntax is detected and reported.
        //
        // Plan:
        //: 1 For each of an enumerated sequence of 'spec' values, ordered by
        //:   increasing 'spec' length, use the primitive generator function
        //:   'gg' to set the state of a newly created object.  Verify that
        //:   'gg' returns a valid reference to the modified argument object
        //:   and, using basic accessors, that the value of the object is as
        //:   expected.  Repeat the test for a longer 'spec' generated by
        //:   prepending a string ending in a '~' character (denoting 'clear').
        //:   Note that we are testing the parser only; the primary
        //:   manipulators are already assumed to work.
        //:
        //: 2 For each of an enumerated sequence of 'spec' values, ordered by
        //:   increasing 'spec' length, use the primitive generator function
        //:   'ggg' to set the state of a newly created object.  Verify that
        //:   'ggg' returns the expected value corresponding to the location of
        //:   the first invalid value of the 'spec'.  Repeat the test for a
        //:   longer 'spec' generated by prepending a string ending in a '~'
        //:   character (denoting 'clear').
        //
        // Testing:
        //   list<T,A>& gg(list<T,A> *object, const char *spec);
        //   int ggg(list<T,A> *object, const char *spec, int vF = 1);
        // --------------------------------------------------------------------

        if (verbose) printf(
                    "TESTING PRIMITIVE GENERATOR FUNCTIONS 'gg' AND 'ggg'\n"
                    "====================================================\n");

        RUN_EACH_TYPE(TestDriver,
                      test03_generatorGG,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      T,
                      TNA,
                      bsltf::MoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      test03_generatorGG,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //
        // Concerns:
        //: 1 The default constructor
        //:   o creates the correct initial value.
        //:   o allocates exactly one block.
        //:
        //: 2 The destructor properly deallocates all allocated memory to its
        //:   corresponding allocator from any attainable state.
        //:
        //: 3 'primaryManipulator' and 'primaryCopier'
        //:   o produces the expected value.
        //:   o maintains valid internal state.
        //:   o preserves the strong exception guarantee and is
        //:     exception-neutral with respect to memory allocation.
        //:   o does not change the address of any other list elements
        //:   o has the internal memory management system hooked up properly so
        //:     that *all* internally allocated memory draws from the same
        //:     user-supplied allocator whenever one is specified.
        //:
        //: 4 'clear'
        //:   o produces the expected value (empty).
        //:   o properly destroys each contained element value.
        //:   o maintains valid internal state.
        //:   o does not allocate memory.
        //:   o deallocates all element memory
        //:
        //: 5 The size-based parameters of the class reflect the platform.
        //
        // Plan:
        //: 1 To address concerns 1a - 1c, create an object using the default
        //:   constructor:
        //:
        //:   o With and without passing in an allocator.
        //:
        //:   o In the presence of exceptions during memory allocations using a
        //:     'bslma_TestAllocator' and varying its *allocation* *limit*.
        //:
        //:   o Where the object is constructed with an object allocator and
        //:     neither of global and default allocator is used to supply
        //:     memory.
        //:
        //: 2 To address concerns 3a - 3e, construct a series of independent
        //:   objects, ordered by increasing length.  In each test, allow the
        //:   object to leave scope without further modification, so that the
        //:   destructor asserts internal object invariants appropriately.
        //:   After the final insert operation in each test, use the (untested)
        //:   basic accessors to cross-check the value of the object and the
        //:   'bslma_TestAllocator' to confirm whether memory allocation has
        //:   occurred.
        //:
        //: 3 To address concerns 4a-4e, construct a similar test, replacing
        //:   'primaryCopier' with 'clear'; this time, however, use the test
        //:   allocator to record *numBlocksInUse* rather than
        //:   *numBlocksTotal*.
        //:
        //: 4 To address concerns 2, 3d, 4d, create a small "area" test that
        //:   exercises the construction and destruction of objects of various
        //:   lengths in the presence of memory allocation exceptions.  Two
        //:   separate tests will be performed:
        //..
        //     Let S be the sequence of integers { 0 .. N - 1 }.
        //     (1) for each i in S, use the default constructor and
        //         'primaryCopier/primaryManipulator' to create an instance
        //         of length i, confirm its value (using basic accessors), and
        //         let it leave scope.
        //     (2) for each (i, j) in S X S, use
        //         'primaryCopier/primaryManipulator' to create an instance of
        //         length i, use 'clear' to clear its value and confirm (with
        //         'length'), use insert to set the instance to a value of
        //         length j, verify the value, and allow the instance to leave
        //         scope.
        //..
        //: 5 The first test acts as a "control" in that 'clear' is not called;
        //:   if only the second test produces an error, we know that 'clear'
        //:   is to blame.  We will rely on 'bslma_TestAllocator' and purify to
        //:   address concern 2, and on the object invariant assertions in the
        //:   destructor to address concerns 3d and 4d.
        //:
        //: 6 To address concern 5, the values will be explicitly compared to
        //:   the expected values.  This will be done first so as to ensure all
        //:   other tests are reliable and may depend upon the class's
        //:   constants.
        //
        // Testing:
        //   list(const A& a = A());
        //   ~list();
        //   void primaryManipulator(list*, int);
        //   void primaryCopier(list*, const T&);
        //   void clear();
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING PRIMARY MANIPULATORS (BOOTSTRAP)\n"
                            "========================================\n");

        RUN_EACH_TYPE(TestDriver,
                      test02_primaryManipulators,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      T,
                      TNA,
                      bsltf::MoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      test02_primaryManipulators,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concern:
        //: 1 We want to exercise basic value-semantic functionality.  In
        //:   particular we want to demonstrate a base-line level of correct
        //:   operation of the following methods and operators:
        //:
        //:   o default and copy constructors (and also the destructor)
        //:
        //:   o the assignment operator (including aliasing)
        //:
        //:   o equality operators: 'operator==' and 'operator!='
        //:
        //:   o primary manipulators: 'push_back' and 'clear' methods
        //:
        //:   o basic accessors: 'size' and 'operator[]'
        //
        // Plan:
        //: 1 Create four objects using both the default and copy constructors.
        //:
        //: 2 Exercise these objects using
        //:
        //:   o primary manipulators
        //:
        //:   o basic accessors
        //:
        //:   o equality operators
        //:
        //:   o the assignment operator
        //:
        //: 3 Try aliasing with assignment for a non-empty instance [11] and
        //:   allow the result to leave scope, enabling the destructor to
        //:   assert internal object invariants.
        //:
        //: 4 Display object values frequently in verbose mode.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("BREATHING TEST\n"
                            "==============\n");

        if (verbose) printf("Standard value-semantic test.\n");

        RUN_EACH_TYPE(TestDriver,
                      test01_breathingTest,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      test01_breathingTest,
                      T,
                      TNA);

        // Disabled: we disabled this very infrequent usage for list (it will
        // be flagged by 'BSLMF_ASSERT'), which is illegal as of C++0x anyway:
        //..
        // bslma::TestAllocator oa(veryVeryVerbose);
        // bsl::allocator<int>  xoa(&oa);
        //
        // list<int, bsl::allocator<void*> > zz1, zz2(xoa);
        //..

        if (verbose) printf("Additional tests: misc.\n");

        list<char>                 myLst(5, 'a');
        list<char>::const_iterator citer;

        ASSERT(5 == myLst.size());

        for (citer = myLst.begin(); citer != myLst.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myLst);

        myLst.insert(myLst.begin(), 'z');

        ASSERT(6 == myLst.size());
        ASSERT('z' == myLst.front());

        for (citer = ++myLst.begin(); citer != myLst.end(); ++citer) {
            ASSERT('a' == *citer);
        }

        if (verbose) P(myLst);

        myLst.erase(succ(myLst.begin(),2), succ(myLst.begin(),4));

        ASSERT(4 == myLst.size());
        ASSERT('z' == myLst.front());

        for (citer = succ(myLst.begin()); citer != myLst.end(); ++citer) {
            ASSERT('a' == *citer);
        }

        if (verbose) P(myLst);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
        citer = myLst.insert(succ(myLst.begin(), 2), { 'd', 'e', 'f'});
        ASSERT('d' == *citer);
        ASSERT(succ(myLst.begin(), 2) == citer);
        ASSERT(7 == myLst.size());

        ASSERT(list<char>({'z', 'a', 'd', 'e', 'f', 'a', 'a'}) == myLst);
#endif

        list<list<char> > vv;
        vv.push_front(myLst);
        if (verbose) P(myLst);

        if (verbose) printf("Additional tests: traits.\n");

        ASSERT(  bslmf::IsBitwiseMoveable<list<char> >::value);
        ASSERT(  bslmf::IsBitwiseMoveable<list<T> >::value);
        ASSERT(  bslmf::IsBitwiseMoveable<list<list<int> > >::value);

        ASSERT(! bsl::is_trivially_copyable<list<char> >::value);
        ASSERT(! bsl::is_trivially_copyable<list<T> >::value);
        ASSERT(! bsl::is_trivially_copyable<list<list<int> > >::value);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        if (verbose) printf("\nAdditional tests: initializer lists.\n");
        {
            ASSERT((0 == []() -> bsl::list<int> { return {}; }().size()));
            ASSERT((1 == []() -> bsl::list<int> { return {1}; }().size()));
            ASSERT((3 == []() -> bsl::list<int> {
                return {3, 1, 3};
            }().size()));
        }
#endif

      } break;
      case -1: {
        if (verbose) printf("COMPILE-ONLY TEST:\n"
                            "==================\n");

        // This test is used to verify various list constructors do not require
        // copy-assignable value type as its (template parameter) type 'VALUE'.

        bsltf::NonAssignableTestType       value(1);
        list<bsltf::NonAssignableTestType> firstList(20, value);
        list<bsltf::NonAssignableTestType> secondList(firstList);
        list<bsltf::NonAssignableTestType> thirdList(firstList.begin(),
                                                     firstList.end());
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
