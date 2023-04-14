// bslstl_list.0.t.cpp                                                -*-C++-*-
#include <bslstl_list.h>

// This test driver contains common test machinery for all 'bslstl_list' test
// drivers.  If '#include'd after defining the 'BSLSTL_LIST_0T_AS_INCLUDE'
// macro it will just provide those common pieces (and no 'main' function).

#include <bsla_fallthrough.h>
#include <bsla_unused.h>

#include <bslstl_iterator.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_stdallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isconvertible.h>
#include <bslmf_isfundamental.h>
#include <bslmf_integralconstant.h>
#include <bslmf_ispointer.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
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
#include <bsltf_wellbehavedmoveonlyalloctesttype.h>

#include <stdexcept>  // 'length_error', 'out_of_range'
#include <algorithm>  // 'next_permutation'
#include <functional> // 'less'
#include <new>        // ::operator new

#include <cctype>
#include <cstdlib>
#include <cstring>

#include <stddef.h>
#include <stdio.h>

using namespace BloombergLP;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
// NOTICE: To reduce the compilation time, this test driver has been broken
// into 3 parts, 'bslstl_list.0.t.cpp' (common code), 'bslstl_list.1.t.cpp'
// (cases 1-11, plus the usage example), and 'bslstl_list.2.t.cpp' (cases 12
// and higher).
//
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
// constructor, manipulator, and accessor in subsequent cases.  Note that due
// to the test driver split up test case 12 is also the  case number for the
// usage example test in test driver part 1.
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
// ~~ bslstl_list.1.t.cpp ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TYPES AND TRAITS:
// [11] Allocator traits and propagation
//
// CREATORS
// [ 2] list(const A& a = A());
// [ 7] list(const list& orig);
// [ 7] list(const list& orig, const A&);
// [ 7] list(list&& orig);
// [ 2] ~list();
//
// MANIPULATORS
// [ 2] void primaryManipulator(list*, int);
// [ 2] void primaryCopier(list*, const T&);
// [ 9] list& operator=(const list& rhs);
// [ 2] void clear();
//
// ACCESSORS
// [11] A get_allocator() const;
// [ 4] bool empty() const;
// [ 4] size_type size() const;
//
// FREE OPERATORS
// [ 6] bool operator==(const list&, const list&);
// [ 6] bool operator!=(const list&, const list&);
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
// [12] USAGE EXAMPLE
//
// ~~ bslstl_list.2.t.cpp ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
// [22] bslalg::HasStlIterators
// [22] bslma::UsesBslmaAllocator
// [22] bslmf::IsBitwiseMoveable
// [22] !bsl::is_trivially_copyable
//
// CREATORS
// [12] list(size_type n);
// [12] list(size_type n, const T& value = T(), const A& a = A());
// [12] template <class iter> list(iter f, iter l, const A& a = A());
// [30] list(list&& orig, const A& = A());
// [33] list(std::initializer_list, const A& = ALLOCATOR());
//
// MANIPULATORS
// [31] list& operator=(list&& rhs);
// [33] list& operator=(std::initializer_list);
// [13] template <class Iter> void assign(Iter first, Iter last);
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
// [17] template <class Iter> iterator insert(CIter pos, Iter f, Iter l);
// [17] iterator emplace(const_iterator pos, Args&&... args);
// [17] reference emplace_back(Args&&... args);
// [17] reference emplace_front(Args&&... args);
// [29] iterator insert(const_iterator pos, T&& value);
// [29] void push_back(T&& value);
// [29] void push_front(T&& value);
// [18] iterator erase(const_iterator pos);
// [18] iterator erase(const_iterator first, const_iterator last);
// [19] void swap(Obj& rhs);
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
// [16] const_iterator begin() const;
// [16] const_iterator end() const;
// [16] const_reverse_iterator rbegin() const;
// [16] const_reverse_iterator rend() const;
// [14] size_type max_size() const;
// [15] const_reference front() const;
// [15] const_reference back() const;
//
// FREE OPERATORS
// [20] bool operator<(const list&, const list&);
// [20] bool operator>(const list&, const list&);
// [20] bool operator<=(const list&, const list&);
// [20] bool operator>=(const list&, const list&);
// [20] auto operator<=>(const list&, const list&);
// [19] void bsl::swap(Obj& lhs, Obj& rhs);
// [36] size_t erase(list<T,A>&, const U&);
// [36] size_t erase_if(list<T,A>&, PREDICATE);
//-----------------------------------------------------------------------------
// [29] CONCERN: All emplace methods handle rvalues.
// [32] INCOMPLETE VALUE TYPES ARE SUPPORTED
// [34] CONCERN: Methods qualified 'noexcept' in standard are so implemented.
// [35] CLASS TEMPLATE DEDUCTION GUIDES
// [37] INCOMPLETE TYPE SUPPORT
//-----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

#ifndef BSLSTL_LIST_0T_AS_INCLUDE
void aSsErT(bool condition, const char *message, int line) BSLA_UNUSED;
#endif

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

typedef TestType                      TTA;  // uses 'bslma' allocators
typedef TestTypeNoAlloc               TNA;  // does not use 'bslma' allocators

#if 0
// TBD: commenting this out until bsltf can be enhanced to have types that
// use alternate allocators.
typedef TestTypeOtherAlloc                 TOA;  // Uses non-'bslma' allocators

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
struct less<TestType> {
     // public type names
    typedef TestType  first_argument_type;
    typedef TestType  second_argument_type;
    typedef bool      result_type;

    bool operator()(const TestType& a, const TestType& b) const
        // Return 'true' if the specified 'a' is less than the specified 'b'
        // and 'false' otherwise.
    {
        ASSERT(!"less<TestType> should not be called");
        return a < b;
    }
};

template <>
struct equal_to<TestType> {
    // public type names
    typedef TestType  first_argument_type;
    typedef TestType  second_argument_type;
    typedef bool      result_type;

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
#ifdef BSLS_PLATFORM_CMP_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#endif
    *element = TYPE(static_cast<IntPtr>(value));    // 'reinterpret_cast'
                                                    // doesn't work here, the
                                                    // function-call cast is
                                                    // necessary.
#ifdef BSLS_PLATFORM_CMP_CLANG
#pragma clang diagnostic pop
#endif
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

template <>
void assignTo<bsltf::WellBehavedMoveOnlyAllocTestType>(
                              bsltf::WellBehavedMoveOnlyAllocTestType *element,
                              int                                      value)
{
    element->setData(value);
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
    // sequence.  We keep track of the active iterator by keeping a main
    // iterator in the container.  The 'd_main' member of valid iterator will
    // point to the main iterator and its 'd_imp' member will have the same
    // value as the main iterator.  If a valid iterator is copied by copy
    // construction or assignment, then both the original and copy are valid.
    // However, as soon as one is incremented, the 'd_imp' of other one will no
    // longer match the main iterator, making it invalid.  As a special case,
    // an iterator with a null main iterator is valid but not incrementable.
    // These special iterators are used for the 'end' iterator and for the
    // return of the post-increment operator.

    BaseIterType *d_main_p;
    BaseIterType  d_imp;

    InputSeqConstIterator(BaseIterType *m, BaseIterType i)
    : d_main_p(m), d_imp(i)
        // Construct an iterator using the specified 'm' as the address of the
        // main iterator and the specified 'i' as the initial iterator
        // position.  If 'm' is null, then the resulting iterator is valid, but
        // may not be incremented.  If 'm' is non-null and '*m' does not equal
        // 'i', then the resulting iterator is invalid -- it may not be
        // dereferenced or incremented, but it may be assigned a new value.
    {}

    friend class InputSeq<TYPE>;
    template <class BDE_OTHER_TYPE>
    friend bool operator==(InputSeqConstIterator<BDE_OTHER_TYPE>,
                           InputSeqConstIterator<BDE_OTHER_TYPE>);

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
        ASSERT(d_main_p && d_imp == *d_main_p);  // test if incrementable
        if (!(d_main_p && d_imp == *d_main_p)) {
            // Continue test despite error by making iterator its own main.
            // This assignment also prevents cascade errors.
            d_main_p = &d_imp;
        }
        d_imp = ++*d_main_p;
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
        ASSERT( ! d_main_p || d_imp == *d_main_p); // test if valid
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
            (lhs.d_main_p == rhs.d_main_p || !lhs.d_main_p || !rhs.d_main_p));
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
    mutable typename RandSeq<TYPE>::const_iterator d_mainIter;

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
    d_mainIter = d_value.begin();
    return InputSeqConstIterator<TYPE>(&d_mainIter, d_mainIter);
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
  private:
    // PRIVATE TYPES
    typedef ALLOC                        AllocBase;
    typedef bsl::allocator_traits<ALLOC> TraitsBase;

  public:
    // TYPES
    typedef typename TraitsBase::size_type         size_type;

    template <class BDE_OTHER_TYPE> struct rebind {
        // It is better not to inherit the 'rebind' template, or else
        // 'rebind<X>::other' would be 'ALLOC::rebind<BDE_OTHER_TYPE>::other'
        // instead of 'LimitAlloc<ALLOC::rebind<BDE_OTHER_TYPE>::otherX>'.

        typedef typename TraitsBase::template rebind_traits<BDE_OTHER_TYPE>
                                                              RebindTraitsBase;

        typedef LimitAllocator<typename RebindTraitsBase::allocator_type>
                                                              other;
    };

  private:
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
    template <class BDE_OTHER_TYPE>
    struct rebind {
        // This nested 'struct' template, parameterized by some
        // 'BDE_OTHER_TYPE', provides a namespace for an 'other' type alias,
        // which is an allocator type following the same template as this one
        // but that allocates elements of 'BDE_OTHER_TYPE'.  Note that this
        // allocator type is convertible to and from 'other' for any
        // 'BDE_OTHER_TYPE' including 'void'.

        typedef StatefulStlAllocator<BDE_OTHER_TYPE> other;
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

    template <class BDE_OTHER_TYPE>
    StatefulStlAllocator(const StatefulStlAllocator<BDE_OTHER_TYPE>& original)
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

    template <class BDE_OTHER_TYPE>
    struct rebind {
        typedef SmallAllocator<BDE_OTHER_TYPE> other;
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
//                          TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
struct TestSupport {
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
    typedef bsl::list<TYPE,ALLOC>                 Obj;

    // Shorthands
    typedef typename Obj::iterator                iterator;
    typedef typename Obj::const_iterator          const_iterator;
    typedef typename Obj::reverse_iterator        reverse_iterator;
    typedef typename Obj::const_reverse_iterator  const_reverse_iterator;

    typedef bslmf::MovableRefUtil                 MoveUtil;
    typedef bsltf::MoveState                      MoveState;
    typedef bsltf::TestValuesArray<TYPE>          TestValues;
    typedef bsl::allocator_traits<ALLOC>          AllocTraits;

    typedef typename
        bsl::is_convertible<bslma::Allocator*,ALLOC>::type ObjHasBslmaAlloc;
        // true_type if ALLOC is a 'bslma::Allocator' type

    typedef typename bslma::UsesBslmaAllocator<TYPE>::type TypeUsesBslmaAlloc;
        // true_type if TYPE uses 'bslma::Allocator'.

    typedef typename bsl::uses_allocator<TYPE, ALLOC>::type TypeUsesStdAlloc;
        // true_type if TYPE uses standard allocator.

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

    class GreaterThan {
      private:
        // private type names, odd design?!
        typedef TYPE  first_argument_type;
        typedef TYPE  second_argument_type;
        typedef bool  result_type;

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
                                         || bsl::is_same<TYPE, TTA>::value
                                         || bsl::is_same<TYPE, TNA>::value> {};

    struct IsMoveAware : bsl::integral_constant<bool,
                   bsl::is_same<TYPE, bsltf::MovableTestType>::value
                || bsl::is_same<TYPE, bsltf::MovableAllocTestType>::value
                || bsl::is_same<TYPE, bsltf::MoveOnlyAllocTestType>::value
                || bsl::is_same<TYPE,
                           bsltf::WellBehavedMoveOnlyAllocTestType>::value> {};

    // CONSTANTS
    static const bool k_SCOPED_ALLOC =
        (ObjHasBslmaAlloc::value && TypeUsesBslmaAlloc::value)
                                                    || TypeUsesStdAlloc::value;
        // true if both the container shares its allocator with its contained
        // elements.

    static const bool k_IS_WELL_BEHAVED =
            bsl::is_same<TYPE, bsltf::WellBehavedMoveOnlyAllocTestType>::value;

                             static const bool k_IS_DEFAULT_CONSTRUCTIBLE =
#if defined(BSLS_PLATFORM_OS_AIX) || defined(BSLS_PLATFORM_OS_WINDOWS)
    // Aix has a compiler bug where method pointers do not default construct
    // to 0.  Windows has the same problem.
                            !bsl::is_same<TYPE, TstFacility::MethodPtr>::value;
#else
                            true;
#endif

    // CLASS METHODS
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
};

// Unfortunately these names have to be made available "by hand" due to two
// phase name lookup not reaching into dependent bases.
#define BSLSTL_LIST_0T_PULL_TESTSUPPORT_NAMES                                 \
    typedef TestSupport<TYPE, ALLOC> Base;                                    \
                                                                              \
    typedef typename Base::Obj                    Obj;                        \
                                                                              \
    typedef typename Base::iterator               iterator;                   \
    typedef typename Base::const_iterator         const_iterator;             \
    typedef typename Base::reverse_iterator       reverse_iterator;           \
    typedef typename Base::const_reverse_iterator const_reverse_iterator;     \
                                                                              \
    typedef typename Base::MoveUtil               MoveUtil;                   \
    typedef typename Base::MoveState              MoveState;                  \
    typedef typename Base::TestValues             TestValues;                 \
    typedef typename Base::AllocTraits            AllocTraits;                \
                                                                              \
    typedef typename Base::ObjHasBslmaAlloc       ObjHasBslmaAlloc;           \
    typedef typename Base::TypeUsesBslmaAlloc     TypeUsesBslmaAlloc;         \
    typedef typename Base::TypeUsesStdAlloc       TypeUsesStdAlloc;           \
                                                                              \
    typedef typename Base::VPred                  VPred;                      \
    typedef typename Base::LowBitEQ               LowBitEQ;                   \
    typedef typename Base::GreaterThan            GreaterThan;                \
    typedef typename Base::ExceptionProctor       ExceptionProctor;           \
                                                                              \
    typedef typename Base::HasOperatorLessThan    HasOperatorLessThan;        \
    typedef typename Base::IsMoveAware            IsMoveAware;                \
                                                                              \
    using Base::k_SCOPED_ALLOC;                                               \
    using Base::k_IS_WELL_BEHAVED;                                            \
    using Base::k_IS_DEFAULT_CONSTRUCTIBLE;                                   \
                                                                              \
    using Base::checkIntegrity;                                               \
    using Base::deltaBlocks;                                                  \
    using Base::expectedBlocks;                                               \
    using Base::ggg;                                                          \
    using Base::gg;                                                           \
    using Base::numMovedFrom;                                                 \
    using Base::numMovedInto;                                                 \
    using Base::primaryCopier;                                                \
    using Base::primaryManipulator

                    // =================================
                    // template class StatelessAllocator
                    // =================================

template <class TYPE>
struct StatelessAllocator {
    // Stateless std allocator with 'is_always_equal == true_type'

    // TYPES
    typedef TYPE      value_type;
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;

    typedef value_type       *pointer;
    typedef const value_type *const_pointer;

    template <class OTHER_TYPE> struct rebind {
        typedef StatelessAllocator<OTHER_TYPE> other;
    };

    typedef bsl::true_type is_always_equal;

    // CREATORS
    StatelessAllocator()
        // Create a 'StatelessAllocator' object.
    {
    }
    template <class OTHER_TYPE>
    StatelessAllocator(const StatelessAllocator<OTHER_TYPE>&)
        // Create a 'StatelessAllocator' object.
    {
    }

    // MANIPULATORS
    pointer allocate(size_type count)
        // Return a pointer to an uninitialized memory that is enough to store
        // an array of the specified 'count' objects.
    {
        return static_cast<pointer>(::operator new(count *
                                                   sizeof(value_type)));
    }
    void deallocate(pointer address, size_type)
        // Return the memory at the specified 'address' to this allocator.
    {
        ::operator delete(static_cast<void *>(address));
    }

    // FREE OPERATORS
    friend bool operator==(StatelessAllocator, StatelessAllocator)
    {
        return true;
    }
    friend bool operator!=(StatelessAllocator, StatelessAllocator)
    {
        return false;
    }
};

                 // =======================================
                 // template class StdBslmaTestDriverHelper
                 // =======================================

template <template <class, class> class DRIVER, class TYPE>
class StdBslmaTestDriverHelper : public DRIVER<
                            TYPE,
                            bsltf::StdAllocatorAdaptor<bsl::allocator<TYPE> > >
{
};

                    // -----------------------------------
                    // class TestSupport::ExceptionProctor
                    // -----------------------------------

// CREATORS
template <class TYPE, class ALLOC>
TestSupport<TYPE,ALLOC>::ExceptionProctor::ExceptionProctor(Obj *object,
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
TestSupport<TYPE,ALLOC>::ExceptionProctor::~ExceptionProctor()
{
    if (d_object_p) {
        const int LINE = d_lineNum;
        ASSERTV(LINE, d_initialState == *d_object_p);
    }
}

// MANIPULATORS
template <class TYPE, class ALLOC>
inline
void TestSupport<TYPE,ALLOC>::ExceptionProctor::release()
{
    d_object_p = 0;
}

                            // -----------------
                            // class TestSupport
                            // -----------------

template <class TYPE, class ALLOC>
bool TestSupport<TYPE,ALLOC>::checkIntegrity(const Obj& object, Int64 length)
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
#if defined(BSLS_PLATFORM_CMP_MSVC)
// False warning about memory leak (constructor *is* trivial, placement new)
#pragma warning( push )
#pragma warning( disable : 4291 )
#endif
            new (bsls::Util::addressOf(DEFAULT_VALUE)) TYPE();
#if defined(BSLS_PLATFORM_CMP_MSVC)
// Turn back on locally disabled warnings
#pragma warning( pop )
#endif
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
            ASSERTV(bsls::NameOf<TYPE>(), v, VA, VH, DEFAULT_VALUE,
                    true == ret);
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
Int64 TestSupport<TYPE,ALLOC>::deltaBlocks(Int64 n)
{
    // One block per element plus one additional block per element if the
    // element uses the list's allocator ('k_SCOPED_ALLOC' == 1).
    return n + n * k_SCOPED_ALLOC;
}

template <class TYPE, class ALLOC>
inline
Int64 TestSupport<TYPE,ALLOC>::expectedBlocks(Int64 n)
{
    // One block for the sentinel node + block allocations.
    return 1 + deltaBlocks(n);
}

template <class TYPE, class ALLOC>
bsl::list<TYPE,ALLOC>& TestSupport<TYPE,ALLOC>::gg(Obj        *object,
                                                   const char *spec)
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

template <class TYPE, class ALLOC>
int TestSupport<TYPE,ALLOC>::ggg(Obj        *object,
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
Int64 TestSupport<TYPE,ALLOC>::numMovedFrom(const Obj& x)
{
    Int64                ret = 0;
    const const_iterator itEnd = x.end();

    for  (const_iterator it = x.begin(); itEnd != it; ++it) {
        ret += MoveState::e_MOVED == bsltf::getMovedFrom(*it);
    }

    return ret;
}

template <class TYPE, class ALLOC>
Int64 TestSupport<TYPE,ALLOC>::numMovedInto(const Obj& x)
{
    Int64                ret = 0;
    const const_iterator itEnd = x.end();

    for  (const_iterator it = x.begin(); itEnd != it; ++it) {
        ret += MoveState::e_MOVED == bsltf::getMovedInto(*it);
    }

    return ret;
}

template <class TYPE, class ALLOC>
void TestSupport<TYPE,ALLOC>::primaryCopier(Obj         *container,
                                            const TYPE&  element)
{
    primaryManipulator(container, value_of(element));
}

template <class TYPE, class ALLOC>
void TestSupport<TYPE, ALLOC>::primaryManipulator(Obj *container,
                                                  int  identifier)
{
    bslma::TestAllocator scratch;
    ALLOC                xscratch(&scratch);

    bsls::ObjectBuffer<TYPE> buffer;
    TstFacility::emplace(buffer.address(), identifier, xscratch);
    bslma::DestructorGuard<TYPE> guard(buffer.address());

    container->push_back(MoveUtil::move(buffer.object()));
}

#ifndef BSLSTL_LIST_0T_AS_INCLUDE

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main()
{
    (void)verbose;
    (void)veryVerbose;
    (void)veryVeryVerbose;
    (void)veryVeryVeryVerbose;

    (void)globalAllocator_p;
    (void)defaultAllocator_p;

    (void)LARGE_SIZE_VALUE;
    (void)DEFAULT_DATA;
    (void)VB;  (void)VC;  (void)VD;  (void)VE;  (void)VF;  (void)VG;

    return -1;
}

#endif  // not defined 'BSLSTL_LIST_0T_AS_INCLUDE'

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
