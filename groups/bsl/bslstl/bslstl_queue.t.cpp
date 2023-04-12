// bslstl_queue.t.cpp                                                 -*-C++-*-
#include <bslstl_queue.h>

#include <bslstl_deque.h>
#include <bslstl_list.h>
#include <bslstl_vector.h>

#include <bsla_maybeunused.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_mallocfreeallocator.h>
#include <bslma_stdallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_testallocatorexception.h>

#include <bslmf_haspointersemantics.h>
#include <bslmf_movableref.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_nameof.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsls_util.h>

#include <bsltf_moveonlyalloctesttype.h>
#include <bsltf_stdstatefulallocator.h>
#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <algorithm>

#include <stdlib.h>      // atoi
#include <utility>       // move

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Mon Nov 22 15:36:14 2021
// Command line: sim_cpp11_features.pl bslstl_queue.t.cpp
# define COMPILING_BSLSTL_QUEUE_T_CPP
# include <bslstl_queue_cpp03.t.cpp>
# undef COMPILING_BSLSTL_QUEUE_T_CPP
#else

// ============================================================================
//                          ADL SWAP TEST HELPER
// ----------------------------------------------------------------------------

template <class TYPE>
void invokeAdlSwap(TYPE& a, TYPE& b)
    // Exchange the values of the specified 'a' and 'b' objects using the
    // 'swap' method found by ADL (Argument Dependent Lookup).  The behavior is
    // undefined unless 'a' and 'b' were created with the same allocator.
{
    // BSLS_ASSERT_OPT(a.get_allocator() == b.get_allocator());

    using namespace bsl;
    swap(a, b);
}

// The following 'using' directives must come *after* the definition of
// 'invokeAdlSwap' (above).

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test has two value-semantic container adaptors whose
// states are represented by their underlying containers.
//
// The primary manipulators for 'bsl::queue' are the default constructor,
// 'push', and 'pop'.  The basic accessors are 'size' , 'front', and 'back'.
//
// The primary manipulators for 'bsl::queue' are the default constructor,
// 'push', and 'pop'.  The basic accessors are 'size' and 'top'.
// ----------------------------------------------------------------------------
// CLASS 'bsl::queue'
//
// CREATORS
// [ 2] explicit queue();
// [12] explicit queue(const CONTAINER& container);
// [ 7] queue(const queue& original);
// [ 2] explicit queue(const ALLOCATOR& allocator);
// [12] queue(const CONTAINER& container, const ALLOCATOR& allocator);
// [ 7] queue(const queue& original, const ALLOCATOR& allocator);
// [18] queue(MovableRef container);
// [18] queue(MovableRef original);
// [18] queue(MovableRef container, const ALLOCATOR& allocator);
// [18] queue(MovableRef original,  const ALLOCATOR& allocator);
//
// MANIPULATORS
// [ 9] queue& operator=(const queue& rhs);
// [19] queue& operator=(MovableRef rhs);
// [ 2] void push(const value_type& value);
// [19] void push(Args&&.. args)
// [ 2] void pop();
// [ 8] void swap(queue& other);
// [13] reference front();
// [13] reference back();
// [19] emplace(Args&&.. args)
//
// ACCESSORS
// [14] bool empty() const;
// [ 4] size_type size() const;
// [ 4] const_reference front() const;
// [ 4] const_reference back() const;
//
// FREE FUNCTIONS
// [ 6] bool operator==(const queue& lhs, const queue& rhs);
// [ 6] bool operator!=(const queue& lhs, const queue& rhs);
// [15] bool operator< (const queue& lhs, const queue& rhs);
// [15] bool operator> (const queue& lhs, const queue& rhs);
// [15] bool operator>=(const queue& lhs, const queue& rhs);
// [15] bool operator<=(const queue& lhs, const queue& rhs);
// [15] auto operator<=>(const queue& lhs, const queue& rhs);
//
// specialized algorithms:
// [ 8] void swap(queue& lhs,queue& rhs);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] TRAITS
// [16] TESTING NON ALLOCATOR SUPPORTING TYPE
// [17] USAGE EXAMPLE
//
// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] ggg(queue<V,C> *object, const char *spec, int verbose = 1);
// [ 3] gg(queue<V,C> *object, const char *spec);
//
// [ 5] TESTING OUTPUT: Not Applicable
// [10] STREAMING: Not Applicable
// [**] CONCERN: The object is compatible with STL allocator.
// [20] CONCERN: Methods qualified 'noexcept' in standard are so implemented.
// [21] CLASS TEMPLATE DEDUCTION GUIDES

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

// Define default data for testing 'bsl::queue'.

struct DefaultDataRow {
    int         d_line;     // source line number
    int         d_index;    // lexical order
    const char *d_spec;     // specification string, for input to 'gg' function
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line index  spec
    //---- -----  ----
    { L_,      0,  "",                 },
    { L_,      1,  "A",                },
    { L_,     21,  "B",                },
    { L_,      2,  "AA",               },
    { L_,      4,  "AB",               },
    { L_,     22,  "BA",               },
    { L_,     17,  "AC",               },
    { L_,     31,  "CD",               },
    { L_,      5,  "ABC",              },
    { L_,     18,  "ACB",              },
    { L_,     23,  "BAC",              },
    { L_,     25,  "BCA",              },
    { L_,     29,  "CAB",              },
    { L_,     30,  "CBA",              },
    { L_,     24,  "BAD",              },
    { L_,      6,  "ABCA",             },
    { L_,      8,  "ABCB",             },
    { L_,      9,  "ABCC",             },
    { L_,     10,  "ABCD",             },
    { L_,     19,  "ACBD",             },
    { L_,     27,  "BDCA",             },
    { L_,     33,  "DCBA",             },
    { L_,     28,  "BEAD",             },
    { L_,     26,  "BCDE",             },
    { L_,     11,  "ABCDE",            },
    { L_,     20,  "ACBDE",            },
    { L_,     32,  "CEBDA",            },
    { L_,     35,  "EDCBA",            },
    { L_,     36,  "FEDCB",            },
    { L_,     37,  "FEDCBA",           },
    { L_,      7,  "ABCABC",           },
    { L_,      3,  "AABBCC",           },
    { L_,     12,  "ABCDEFG",          },
    { L_,     13,  "ABCDEFGH",         },
    { L_,     14,  "ABCDEFGHI",        },
    { L_,     15,  "ABCDEFGHIJKLMNOP", },
    { L_,     38,  "PONMLKJIGHFEDCBA", },
    { L_,     16,  "ABCDEFGHIJKLMNOPQ",},
    { L_,     34,  "DHBIMACOPELGFKNJQ",},
};

static
const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

typedef bsltf::AllocTestType  TestValueType;
typedef bslmf::MovableRefUtil MoveUtil;

int SPECIAL_INT_VALUES[]       = { INT_MIN, -2, -1, 0, 1, 2, INT_MAX };
int NUM_SPECIAL_INT_VALUES     =
                      sizeof(SPECIAL_INT_VALUES) / sizeof(*SPECIAL_INT_VALUES);

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------


#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#define PRVALUE(x) x
#else
template <class T>
bslmf::MovableRef<T> cpp03_prvalue(const T& x)
    // Return a movable reference to the specified 'x'.  The behavior is
    // undefined unless 'x' can be safely 'const_cast' to a modifiable
    // reference.
{
    return bslmf::MovableRefUtil::move(const_cast<T&>(x));
}
#define PRVALUE(x) cpp03_prvalue(x)
#endif


#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE
//#define TEST_TYPES_REGULAR BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR

#ifndef BSLS_PLATFORM_OS_WINDOWS
# define TEST_TYPES_REGULAR                                                   \
        signed char,                                                          \
        size_t,                                                               \
        bsltf::TemplateTestFacility::ObjectPtr,                               \
        bsltf::TemplateTestFacility::FunctionPtr,                             \
        bsltf::TemplateTestFacility::MethodPtr,                               \
        bsltf::EnumeratedTestType::Enum,                                      \
        bsltf::SimpleTestType,                                                \
        bsltf::AllocTestType,                                                 \
        bsltf::BitwiseMoveableTestType,                                       \
        bsltf::AllocBitwiseMoveableTestType,                                  \
        bsltf::NonTypicalOverloadsTestType
#else
# define TEST_TYPES_REGULAR                                                   \
        signed char,                                                          \
        size_t,                                                               \
        bsltf::TemplateTestFacility::ObjectPtr,                               \
        bsltf::TemplateTestFacility::MethodPtr,                               \
        bsltf::EnumeratedTestType::Enum,                                      \
        bsltf::SimpleTestType,                                                \
        bsltf::AllocTestType,                                                 \
        bsltf::BitwiseMoveableTestType,                                       \
        bsltf::AllocBitwiseMoveableTestType,                                  \
        bsltf::NonTypicalOverloadsTestType
#endif


// 'queue' specific print function
namespace bsl {

template <class VALUE, class CONTAINER>
void debugprint(const bsl::queue<VALUE, CONTAINER>& q)
{
    if (q.empty()) {
        printf("<empty>");
    }
    else {
        using namespace bsls;

        printf("size: %d, front: ", (int) q.size());
        bsls::BslTestUtil::callDebugprint(static_cast<char>(
                       bsltf::TemplateTestFacility::getIdentifier(q.front())));
        printf(", back: ");
        bsls::BslTestUtil::callDebugprint(static_cast<char>(
                        bsltf::TemplateTestFacility::getIdentifier(q.back())));
    }
    fflush(stdout);
}

}  // close namespace bsl

                          // =======================
                          // class NonAllocContainer
                          // =======================

template <class VALUE>
class NonAllocContainer {
  private:
    // DATA
    bsl::deque<VALUE> d_deque;

  public:
    // PUBLIC TYPES
    typedef VALUE        value_type;
    typedef VALUE&       reference;
    typedef const VALUE& const_reference;
    typedef std::size_t  size_type;

    // CREATORS
    NonAllocContainer() : d_deque(&bslma::MallocFreeAllocator::singleton()) {}
    NonAllocContainer(const NonAllocContainer& rhs) : d_deque(rhs.d_deque) {}

    ~NonAllocContainer() {}

    // MANIPULATORS
    NonAllocContainer& operator=(const NonAllocContainer& other)
    {
        d_deque = other.d_deque;
        return *this;
    }

    reference front() { return d_deque.front(); }

    reference back()  { return d_deque.back(); }

    void pop_front()  { d_deque.pop_front(); }

    void push_back(const value_type& value) { d_deque.push_back(value); }

    bsl::deque<value_type>& contents() { return d_deque; }

    // ACCESSORS
    bool operator==(const NonAllocContainer& rhs) const
    {
        return d_deque == rhs.d_deque;
    }

    bool operator!=(const NonAllocContainer& rhs) const
    {
        return !operator==(rhs);
    }

    bool operator<(const NonAllocContainer& rhs) const
    {
        return d_deque < rhs.d_deque;
    }

    bool operator>=(const NonAllocContainer& rhs) const
    {
        return !operator<(rhs);
    }

    bool operator>(const NonAllocContainer& rhs) const
    {
        return d_deque > rhs.d_deque;
    }

    bool operator<=(const NonAllocContainer& rhs) const
    {
        return !operator>(rhs);
    }

    const_reference front() const { return d_deque.front(); }

    const_reference back()  const { return d_deque.back(); }

    size_type size()        const { return d_deque.size(); }

    bool empty()            const { return d_deque.empty(); }
};

namespace std {
    template <class VALUE>
    void swap(NonAllocContainer<VALUE>& lhs, NonAllocContainer<VALUE>& rhs)
    {
        lhs.contents().swap(rhs.contents());
    }
}  // close namespace std

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

                            // ====================
                            // class ExceptionGuard
                            // ====================

template <class OBJECT>
struct ExceptionGuard {
    // This class provide a mechanism to verify the strong exception guarantee
    // in exception-throwing code.  On construction, this class stores a copy
    // of an object of the parameterized type 'OBJECT' and the address of that
    // object.  On destruction, if 'release' was not invoked, it will verify
    // the value of the object is the same as the value of the copy created
    // on construction.  This class requires the copy constructor and
    // 'operator ==' to be tested before use.

    // DATA
    int           d_line;      // the line number at construction
    OBJECT        d_copy;      // copy of the object being tested
    const OBJECT *d_object_p;  // address of the original object

  public:
    // CREATORS
    ExceptionGuard(const OBJECT    *object,
                   int              line,
                   bslma::Allocator *basicAllocator = 0)
    : d_line(line)
    , d_copy(*object, basicAllocator)
    , d_object_p(object)
        // Create the exception guard for the specified 'object' at the
        // specified 'line' number.  Optionally, specify 'basicAllocator' used
        // to supply memory.
    {}

    ~ExceptionGuard()
        // Destroy the exception guard.  If the guard was not released, verify
        // that the state of the object supplied at construction has not
        // change.
    {
        if (d_object_p) {
            const int LINE = d_line;
            ASSERTV(LINE, d_copy == *d_object_p);
        }
    }

    // MANIPULATORS
    void release()
        // Release the guard from verifying the state of the object.
    {
        d_object_p = 0;
    }
};

// ----------------------------------------------------------------------------
//  HELPERS: "Called Method" Classes: 'NonMovableVector' and 'MovableVector'
// ----------------------------------------------------------------------------

enum CalledMethod
    // Enumerations used to indicate if appropriate special container's method
    // has been invoked.

{
    e_NONE                    = 0

  , e_CTOR_DFT_SANS_ALLOC     = 1 <<  0
  , e_CTOR_DFT_AVEC_ALLOC     = 1 <<  1

  , e_CTOR_CPY_SANS_ALLOC     = 1 <<  3
  , e_CTOR_CPY_AVEC_ALLOC     = 1 <<  4

  , e_CTOR_MOV_SANS_ALLOC     = 1 <<  5
  , e_CTOR_MOV_AVEC_ALLOC     = 1 <<  6

  , e_ASSIGN_CREF             = 1 <<  7
  , e_ASSIGN_MOVE             = 1 <<  8

  , e_PUSH_BACK_CREF          = 1 <<  9
  , e_PUSH_BACK_MOVE          = 1 << 10

  , e_EMPLACE_0               = 1 << 11
  , e_EMPLACE_1               = 1 << 12
  , e_EMPLACE_2               = 1 << 13
  , e_EMPLACE_3               = 1 << 14
  , e_EMPLACE_4               = 1 << 15
  , e_EMPLACE_5               = 1 << 16
  , e_EMPLACE_6               = 1 << 17
  , e_EMPLACE_7               = 1 << 18
  , e_EMPLACE_8               = 1 << 19
  , e_EMPLACE_9               = 1 << 20
  , e_EMPLACE_A               = 1 << 21
};

void debugprint(enum CalledMethod calledMethod)
{
    const char *ascii;
#define CASE(X) case(e_ ## X): ascii =  #X; break;

    switch (calledMethod) {
      CASE(NONE)

      CASE(CTOR_DFT_SANS_ALLOC)
      CASE(CTOR_DFT_AVEC_ALLOC)

      CASE(CTOR_CPY_SANS_ALLOC)
      CASE(CTOR_CPY_AVEC_ALLOC)

      CASE(CTOR_MOV_SANS_ALLOC)
      CASE(CTOR_MOV_AVEC_ALLOC)

      CASE(ASSIGN_CREF)
      CASE(ASSIGN_MOVE)

      CASE(PUSH_BACK_CREF)
      CASE(PUSH_BACK_MOVE)

      CASE(EMPLACE_0)
      CASE(EMPLACE_1)
      CASE(EMPLACE_2)
      CASE(EMPLACE_3)
      CASE(EMPLACE_4)
      CASE(EMPLACE_5)
      CASE(EMPLACE_6)
      CASE(EMPLACE_7)
      CASE(EMPLACE_8)
      CASE(EMPLACE_9)
      CASE(EMPLACE_A)

      default: ascii = "(* UNKNOWN *)";
    }

#undef CASE

    printf("%s", ascii);
}

inline CalledMethod operator|=(CalledMethod& lhs, CalledMethod rhs)
    // Bitwise OR the values of the specified 'lhs' and 'rhs' flags, and return
    // the resulting value.
{
    lhs = static_cast<CalledMethod>(
                                static_cast<int>(lhs) | static_cast<int>(rhs));
    return lhs;
}

CalledMethod g_calledMethodFlag;  // global variable, that stores information
                                  // about called methods for special
                                  // containers 'NonMovableVector' and
                                  // 'MovableVector'.

void setupCalledMethodCheck()
    // Reset 'g_calledMethodFlag' global variable's value.
{
    g_calledMethodFlag = e_NONE;
}

enum CalledMethod getCalledMethod()
{
    return g_calledMethodFlag;
}

                           // ======================
                           // class NonMovableVector
                           // ======================

template <class VALUE, class ALLOCATOR>
class NonMovableVector;

template<class VALUE, class ALLOCATOR>
bool operator==(const NonMovableVector<VALUE, ALLOCATOR>& lhs,
                const NonMovableVector<VALUE, ALLOCATOR>& rhs);

template <class VALUE, class ALLOCATOR = bsl::allocator<VALUE> >
class NonMovableVector {
    // This class is a value-semantic class template, acting as a transparent
    // proxy for the underlying 'bsl::vector' container, that holds elements of
    // the (template parameter) 'VALUE', and recording in the global variable
    // 'g_calledMethodFlag' methods being invoked.  The information recorded is
    // used to verify that 'stack' invokes expected container methods.

    // DATA
    bsl::vector<VALUE> d_vector;  // container for it's behavior simulation

    // FRIENDS
    friend bool operator==<VALUE, ALLOCATOR>(const NonMovableVector& lhs,
                                             const NonMovableVector& rhs);

  public:
    // CLASS METHODS
    static int              GGG(NonMovableVector *object,
                                const char       *spec,
                                int               verbose = 1);
    static NonMovableVector  GG(NonMovableVector *object,
                                const char       *spec);

    // PUBLIC TYPES
    typedef ALLOCATOR     allocator_type;
    typedef VALUE         value_type;
    typedef VALUE&        reference;
    typedef const VALUE&  const_reference;
    typedef std::size_t   size_type;
    typedef VALUE        *iterator;
    typedef const VALUE  *const_iterator;

    // CREATORS
    NonMovableVector()
    : d_vector()
        // Create an empty vector.  Method invocation is recorded.
    {
        g_calledMethodFlag |= e_CTOR_DFT_SANS_ALLOC;
    }

    NonMovableVector(const ALLOCATOR& basicAllocator)
    : d_vector(basicAllocator)
        // Create an empty vector, using the specified 'basicAllocator' to
        // supply memory.  Method invocation is recorded.
    {
        g_calledMethodFlag |= e_CTOR_DFT_AVEC_ALLOC;
    }

    NonMovableVector(const NonMovableVector& original)
        // Create a vector that has the same value as the specified 'original'
        // vector.  Method invocation is recorded.
    : d_vector(original.d_vector)
    {
        g_calledMethodFlag |= e_CTOR_CPY_SANS_ALLOC;
    }

    NonMovableVector(const NonMovableVector& original,
                     const ALLOCATOR& basicAllocator)
        // Create a vector that has the same value as the specified 'original'
        // vector, using the specified 'basicAllocator' to supply memory.
        // Method invocation is recorded.
    : d_vector(original.d_vector, basicAllocator)
    {
        g_calledMethodFlag |= e_CTOR_CPY_AVEC_ALLOC;
    }

    // MANIPULATORS
    NonMovableVector& operator=(const NonMovableVector& rhs)
        // Assign to this vector the value of the specified 'other' vector and
        // return a reference to this modifiable vector.  Method invocation is
        // recorded.
    {
        d_vector = rhs.d_vector;
        g_calledMethodFlag |= e_ASSIGN_CREF;
        return *this;
    }

    void pop_back()
        // Erase the last element from this vector.
    {
        d_vector.pop_back();
    }

    void push_back(const value_type& value)
        // Append a copy of the specified 'value' at the end of this vector.
        // Method invocation is recorded.
    {
        g_calledMethodFlag |= e_PUSH_BACK_CREF;
        d_vector.push_back(value);
    }

    template <class INPUT_ITER>
    iterator insert(const_iterator position,
                    INPUT_ITER     first,
                    INPUT_ITER     last)
            // Insert at the specified 'position' in this vector the values in
            // the range starting at the specified 'first' and ending
            // immediately before the specified 'last' iterators of the
            // (template parameter) type 'INPUT_ITER', and return an iterator
            // to the first newly inserted element.
    {
        return d_vector.insert(position, first, last);
    }

    iterator begin()
        // Return an iterator pointing the first element in this modifiable
        // vector (or the past-the-end iterator if this vector is empty).
    {
        return d_vector.begin();
    }

    iterator end()
        // Return the past-the-end iterator for this modifiable vector.
    {
        return d_vector.end();
    }

    reference front()
        // Return a reference to the modifiable element at the first position
        // in this vector.  The behavior is undefined if this vector is empty.
    {
        return d_vector.front();
    }

    reference back()
        // Return a reference to the modifiable element at the last position in
        // this vector.  The behavior is undefined if this vector is empty.
    {
        return d_vector.back();
    }

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... Args> void emplace_back(
                                         BSLA_MAYBE_UNUSED Args&&... arguments)
        // Append to the end of this vector a newly created 'value_type'
        // object, constructed with integer literal as a parameter,
        // irrespective of the specified 'arguments'.  Note that this method is
        // written only for testing purposes, it DOESN'T simulate standard
        // vector behavior and requires that the (template parameter) type
        // 'VALUE_TYPE' has constructor, accepting integer value as a
        // parameter.  Method invocation is recorded.
    {
         int argumentsNumber = sizeof...(Args);
         g_calledMethodFlag |= static_cast<CalledMethod>(
                     static_cast<int>(e_EMPLACE_0) << argumentsNumber);
         d_vector.push_back(PRVALUE(value_type(1)));
    }
#endif

    // ACCESSORS
    const_iterator begin() const
        // Return an iterator pointing the first element in this non-modifiable
        // vector (or the past-the-end iterator if this vector is empty).
    {
        return d_vector.begin();
    }

    const_iterator end() const
        // Return the past-the-end iterator for this non-modifiable vector.
    {
        return d_vector.end();
    }

    const_reference front() const
        // Return a reference to the non-modifiable element at the first
        // position in this vector.  The behavior is undefined if this vector
        // is empty.
    {
        return d_vector.front();
    }

    const_reference back() const
        // Return a reference to the non-modifiable element at the last
        // position in this vector.  The behavior is undefined if this vector
        // is empty.
    {
        return d_vector.back();
    }

    size_type size() const
        // Return the number of elements in this vector.
    {
        return d_vector.size();
    }

    bool empty() const
        // Return 'true' if this vector has size 0, and 'false' otherwise.
    {
        return d_vector.empty();
    }
};

                           // ----------------------
                           // class NonMovableVector
                           // ----------------------

// CLASS METHODS
template <class VALUE, class CONTAINER>
class TestDriver;

template <class VALUE, class ALLOCATOR>
int NonMovableVector<VALUE, ALLOCATOR>::
GGG(NonMovableVector *object,
    const char       *spec,
    int               verbose)
{
    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());

    typename TestDriver<value_type,
                        NonMovableVector>::TestValues VALUES;

    enum { SUCCESS = -1 };

    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Z') {
            object->push_back(VALUES[spec[i] - 'A']);
        }
        else {
            if (verbose) {
                printf("Error, bad character ('%c') "
                       "in spec \"%s\" at position %d.\n", spec[i], spec, i);
            }

            // Discontinue processing this spec.

            return i;                                                 // RETURN
        }
   }
   return SUCCESS;
}

template <class VALUE, class ALLOCATOR>
NonMovableVector<VALUE, ALLOCATOR>
NonMovableVector<VALUE, ALLOCATOR>::
GG(NonMovableVector *object, const char *spec)
{
    ASSERTV(GGG(object, spec) < 0);
    return *object;
}

// FREE OPERATORS
template<class VALUE, class ALLOCATOR>
bool operator==(const NonMovableVector<VALUE, ALLOCATOR>& lhs,
                const NonMovableVector<VALUE, ALLOCATOR>& rhs)
{
    return lhs.d_vector == rhs.d_vector;
}

                            // ===================
                            // class MovableVector
                            // ===================

template <class VALUE, class ALLOCATOR>
class MovableVector;

template<class VALUE, class ALLOCATOR>
bool operator==(const MovableVector<VALUE, ALLOCATOR>& lhs,
                const MovableVector<VALUE, ALLOCATOR>& rhs);

template <class VALUE, class ALLOCATOR = bsl::allocator<VALUE> >
class MovableVector {
    // TBD
    //
    // This class is a value-semantic class template, acting as a transparent
    // proxy for the underlying 'bsl::vector' container, that holds elements of
    // the (template parameter) 'VALUE', and recording in the global variable
    // 'g_calledMethodFlag' methods being invoked.  The information recorded is
    // used to verify that 'queue' invokes expected container methods.

  private:
    // DATA
    bsl::vector<VALUE> d_vector;  // provides required behavior

    // FRIENDS
    friend bool operator==<VALUE, ALLOCATOR>(
                                   const MovableVector<VALUE, ALLOCATOR>& lhs,
                                   const MovableVector<VALUE, ALLOCATOR>& rhs);
  public:
    // CLASS METHODS
    static int            GGG(MovableVector *object,
                              const char    *spec,
                              int            verbose = 1);
    static MovableVector  GG(MovableVector *object,
                             const char    *spec);

    // PUBLIC TYPES
    typedef ALLOCATOR    allocator_type;
    typedef VALUE        value_type;
    typedef VALUE&       reference;
    typedef const VALUE& const_reference;
    typedef std::size_t  size_type;
    typedef VALUE*       iterator;
    typedef const VALUE* const_iterator;

    // CREATORS
    MovableVector()
    : d_vector()
        // Create an empty vector.  Method invocation is recorded.
    {
        g_calledMethodFlag |= e_CTOR_DFT_SANS_ALLOC;
    }

    MovableVector(const ALLOCATOR& basicAllocator)
    : d_vector( basicAllocator)
        // Create an empty vector, using the specified 'basicAllocator' to
        // supply memory.  Method invocation is recorded.
    {
        g_calledMethodFlag |= e_CTOR_DFT_AVEC_ALLOC;
    }

    MovableVector(const MovableVector& original)
        // Create a vector that has the same value as the specified 'original'
        // vector.  Method invocation is recorded.
    : d_vector(original.d_vector)
    {
        g_calledMethodFlag |= e_CTOR_CPY_SANS_ALLOC;
    }

    MovableVector(bslmf::MovableRef<MovableVector> original)
        // Create a vector that has the same value as the specified 'original'
        // vector.  Method invocation is recorded.
    : d_vector(MoveUtil::move(MoveUtil::access(original).d_vector))
    {
        g_calledMethodFlag |= e_CTOR_MOV_SANS_ALLOC;
    }

    MovableVector(const MovableVector& original,
                  const ALLOCATOR&     basicAllocator)
        // Create a vector that has the same value as the specified 'original'
        // vector, using the specified 'basicAllocator' to supply memory.
        // Method invocation is recorded.
    : d_vector(original.d_vector, basicAllocator)
    {
        g_calledMethodFlag |= e_CTOR_CPY_AVEC_ALLOC;
    }

    MovableVector(bslmf::MovableRef<MovableVector> original,
                  const ALLOCATOR&                 basicAllocator)
        // Create a vector that has the same value as the specified 'original'
        // vector, using the specified 'basicAllocator' to supply memory.
        // Method invocation is recorded.
    : d_vector(MoveUtil::move(MoveUtil::access(original).d_vector),
               basicAllocator)
    {
        g_calledMethodFlag |= e_CTOR_MOV_AVEC_ALLOC;
    }

    // MANIPULATORS
    MovableVector& operator=(const MovableVector& rhs)
        // Assign to this vector the value of the specified 'other' vector and
        // return a reference to this modifiable vector.  Method invocation is
        // recorded.
    {
        g_calledMethodFlag |= e_ASSIGN_CREF;
        d_vector = rhs.d_vector;
        return *this;
    }

    MovableVector& operator=(bslmf::MovableRef<MovableVector> rhs)
        // Assign to this vector the value of the specified 'other' vector and
        // return a reference to this modifiable vector.  Method invocation is
        // recorded.
    {
        g_calledMethodFlag |= e_ASSIGN_MOVE;
        d_vector = MoveUtil::move(MoveUtil::access(rhs).d_vector);
        return *this;
    }

    void pop_back()
        // Erase the last element from this vector.
    {
        d_vector.pop_back();
    }

    void push_back(const value_type& value)
        // Append a copy of the specified 'value' at the end of this vector.
        // Method invocation is recorded.
    {
        g_calledMethodFlag |= e_PUSH_BACK_CREF;
        d_vector.push_back(value);
    }

    void push_back(bslmf::MovableRef<value_type> value)
        // Append a copy of the specified 'value' at the end of this vector.
        // Method invocation is recorded.
    {
        g_calledMethodFlag |= e_PUSH_BACK_MOVE;
        d_vector.push_back(MoveUtil::move(value));
    }

    template <class INPUT_ITER>
    iterator insert(const_iterator position,
                    INPUT_ITER     first,
                    INPUT_ITER     last)
            // Insert at the specified 'position' in this vector the values in
            // the range starting at the specified 'first' and ending
            // immediately before the specified 'last' iterators of the
            // (template parameter) type 'INPUT_ITER', and return an iterator
            // to the first newly inserted element.
    {
        return d_vector.insert(position, first, last);
    }

    iterator begin()
        // Return an iterator pointing the first element in this modifiable
        // vector (or the past-the-end iterator if this vector is empty).
    {
        return d_vector.begin();
    }

    iterator end()
        // Return the past-the-end iterator for this modifiable vector.
    {
        return d_vector.end();
    }

    reference front()
        // Return a reference to the modifiable element at the first position
        // in this vector.  The behavior is undefined if this vector is empty.
    {
        return d_vector.front();
    }

    reference back()
        // Return a reference to the modifiable element at the last position in
        // this vector.  The behavior is undefined if this vector is empty.
    {
        return d_vector.back();
    }

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... Args> void emplace_back(
                                         BSLA_MAYBE_UNUSED Args&&... arguments)
        // Append to the end of this vector a newly created 'value_type'
        // object, constructed with integer literal as a parameter,
        // irrespective of the specified 'arguments'.  Note that this method is
        // written only for testing purposes, it DOESN'T simulate standard
        // vector behavior and requires that the (template parameter) type
        // 'VALUE_TYPE' has constructor, accepting integer value as a
        // parameter.  Method invocation is recorded.
    {
         int argumentsNumber = sizeof...(Args);
         g_calledMethodFlag |= static_cast<CalledMethod>(
                     static_cast<int>(e_EMPLACE_0) << argumentsNumber);
         d_vector.push_back(PRVALUE(value_type(1)));
    }
#endif

    // ACCESSORS
    const_iterator begin() const
        // Return an iterator pointing the first element in this non-modifiable
        // vector (or the past-the-end iterator if this vector is empty).
    {
        return d_vector.begin();
    }

    const_iterator end() const
        // Return the past-the-end iterator for this non-modifiable vector.
    {
        return d_vector.end();
    }

    const_reference front() const
        // Return a reference to the non-modifiable element at the first
        // position in this vector.  The behavior is undefined if this vector
        // is empty.
    {
        return d_vector.front();
    }

    const_reference back() const
        // Return a reference to the non-modifiable element at the last
        // position in this vector.  The behavior is undefined if this vector
        // is empty.
    {
        return d_vector.back();
    }

    size_type size() const
        // Return the number of elements in this vector.
    {
        return d_vector.size();
    }

    bool empty() const
        // Return 'true' if this vector has size 0, and 'false' otherwise.
    {
        return d_vector.empty();
    }
};

                            // -------------------
                            // class MovableVector
                            // -------------------

// CLASS METHODS
template <class VALUE, class CONTAINER>
class TestDriver;

template <class VALUE, class ALLOCATOR>
int MovableVector<VALUE, ALLOCATOR>::
GGG(MovableVector *object,
    const char    *spec,
    int            verbose)
{
    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());

    typename TestDriver<value_type,
                        MovableVector>::TestValues VALUES;

    enum { SUCCESS = -1 };

    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Z') {
            object->push_back(VALUES[spec[i] - 'A']);
        }
        else {
            if (verbose) {
                printf("Error, bad character ('%c') "
                       "in spec \"%s\" at position %d.\n", spec[i], spec, i);
            }

            // Discontinue processing this spec.

            return i;                                                 // RETURN
        }
   }
   return SUCCESS;
}

template <class VALUE, class ALLOCATOR>
MovableVector<VALUE, ALLOCATOR>
MovableVector<VALUE, ALLOCATOR>::
GG(MovableVector *object, const char *spec)
{
    ASSERTV(GGG(object, spec) < 0);
    return *object;
}

// FREE OPERATORS
template<class VALUE, class ALLOCATOR>
bool operator==(const MovableVector<VALUE, ALLOCATOR>& lhs,
                const MovableVector<VALUE, ALLOCATOR>& rhs)
{
    return lhs.d_vector == rhs.d_vector;
}


template <class T>
struct SpecialContainerTrait
    // A class should declare this trait if it registers it's methods
    // invocation in 'g_calledMethodFlag' global variable.
{
    static const bool is_special_container = false;
};

template <class T>
struct SpecialContainerTrait<NonMovableVector<T> >
{
    static const bool is_special_container = true;
};

template <class T>
struct SpecialContainerTrait<MovableVector<T> >
{
    static const bool is_special_container = true;
};

template <class CONTAINER>
bool isCalledMethodCheckPassed(CalledMethod flag)
    // Return 'true' if global variable 'g_calledMethodFlag' has the same value
    // as the specified 'flag', and 'false' otherwise.  Note that this check is
    // performed only for special containers, defined above.  Function always
    // returns 'true' for all other classes.
{
    if (SpecialContainerTrait<CONTAINER>::is_special_container) {
        return flag == g_calledMethodFlag;
    }
    return true;
}

                            // ======================
                            // class NothrowSwapDeque
                            // ======================

template <class VALUE>
class NothrowSwapDeque : public bsl::deque<VALUE, bsl::allocator<VALUE> > {
    // 'deque' with non-throwing 'swap'

    // TYPES
    typedef bsl::deque<VALUE, bsl::allocator<VALUE> > base;
        // Base class alias.
  public:
    // MANIPULATORS
    void swap(NothrowSwapDeque& other) BSLS_KEYWORD_NOEXCEPT
        // Exchange the value of this object with that of the specified 'other'
        // object.
    {
        base::swap(other);
    }

    // FREE FUNCTIONS
    friend void swap(NothrowSwapDeque& a,
                     NothrowSwapDeque& b) BSLS_KEYWORD_NOEXCEPT
        // Exchange the values of the specified 'a' and 'b' objects.
    {
        a.swap(b);
    }
};

//=============================================================================
//                       TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

                              // ----------------
                              // class TestDriver
                              // ----------------

template <class VALUE, class CONTAINER = deque<VALUE> >
class TestDriver {
    // Test driver class for 'queue'

  private:

    // TYPES
    typedef bsl::queue<VALUE, CONTAINER> Obj;
        // type under testing

    typedef typename Obj::value_type      value_type;
    typedef typename Obj::reference       reference;
    typedef typename Obj::const_reference const_reference;
    typedef typename Obj::size_type       size_type;
    typedef typename Obj::container_type  container_type;
        // shorthands

  public:
    typedef bsltf::TestValuesArray<typename Obj::value_type> TestValues;

  private:
    // TEST APPARATUS
    //-------------------------------------------------------------------------
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the 'queue<VALUE, CONTAINER>' object according to
    // a custom language.  Uppercase letters [A..Z] correspond to arbitrary
    // (but unique) 'VALUE' object.
    //..
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
    // <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'Z'
    //                                      // unique but otherwise arbitrary
    // Spec String  Description
    // -----------  -----------------------------------------------------------
    // ""           Has no effect; leaves the object empty.
    // "A"          Push the 'VALUE' object corresponding to A.
    // "AA"         Push two 'VALUE' objects both corresponding to A.
    // "ABC"        Push three 'VALUE' objects corresponding to A, B and 'C'.
    //..
    //-------------------------------------------------------------------------

    static int ggg(Obj *object, const char *spec, int verbose = 1);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'push' and 'pop' .
        // Optionally specify a zero 'verbose' to suppress 'spec' syntax error
        // messages.  Return the index of the first invalid character, and a
        // negative value otherwise.  Note that this function is used to
        // implement 'gg' as well as allow for verification of syntax error
        // detection.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified object with its value adjusted
        // according to the specified 'spec'.

    template <class VALUES>
    static size_t verify_object(Obj&          object,
                                const VALUES& expectedValues,
                                size_t        expectedSize);

    static bool use_same_allocator(Obj&                  object,
                                   int                   TYPE_ALLOC,
                                   bslma::TestAllocator *ta);

    static void populate_container(CONTAINER&        container,
                                   const char*       SPEC,
                                   size_t            length);

  public:
    // TEST CASES
    static void testCase20();
        // Test 'noexcept' specifications

    static void testCase19MoveOnlyType();
        // Test move manipulators on move-only types

    static void testCase18MoveOnlyType();
        // Test move manipulators on move-only types

    template <bool PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase19_propagate_on_container_move_assignment_dispatch();
    static void testCase19_propagate_on_container_move_assignment();
        // Test 'propagate_on_container_move_assignment'.

    static void testCase19(bool isMovableContainer);
        // Test move manipulators

    static void testCase18(bool isMovableContainer);
        // Test move constructors

    static void testCase15();
        // Test comparison free operators.

    static void testCase14();
        // Test accessor 'empty'.

    static void testCase13();
        // Test manipulators 'front' and 'back'.

    static void testCase12();
        // Test user-supplied constructors.

    static void testCase11();
        // Test type traits.

    // static void testCase10();
        // Reserved for BDEX.

    template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase9_propagate_on_container_copy_assignment_dispatch();
    static void testCase9_propagate_on_container_copy_assignment();
        // Test 'propagate_on_container_copy_assignment'.

    static void testCase9();
        // Test assignment operator ('operator=').

    template <bool PROPAGATE_ON_CONTAINER_SWAP_FLAG,
              bool OTHER_FLAGS>
    static void testCase8_propagate_on_container_swap_dispatch();
    static void testCase8_propagate_on_container_swap();
        // Test 'propagate_on_container_swap'.

    static void testCase8();
        // Test 'swap' member.

    template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
              bool OTHER_FLAGS>
    static void testCase7_select_on_container_copy_construction_dispatch();
    static void testCase7_select_on_container_copy_construction();
        // Test 'select_on_container_copy_construction'.

    static void testCase7();
        // Test copy constructor.

    static void testCase6();
        // Test equality operator ('operator==').

    // static void testCase5();
        // Reserved for (<<) operator.

    static void testCase4();
        // Test basic accessors ('size', 'front', and 'back').

    static void testCase3();
        // Test generator functions 'ggg', and 'gg'.

    static void testCase2();
        // Test primary manipulators (default ctor, 'push', and 'pop')

    static void testCase1(const VALUE  *testValues, size_t  numValues);
        // Breathing test.  This test *exercises* basic functionality but
        // *test* nothing.

    static void testCase1_NoAlloc(const VALUE  *testValues, size_t  numValues);
        // Breathing test, except on a non-allocator container.  This test
        // *exercises* basic functionality but *test* nothing.
};

                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class VALUE, class CONTAINER>
template <class VALUES>
size_t TestDriver<VALUE, CONTAINER>::verify_object(
                                                  Obj&          object,
                                                  const VALUES& expectedValues,
                                                  size_t        expectedSize)
    // Verify the specified 'object' has the specified 'expectedSize' and
    // contains the same values as the array in the specified 'expectedValues'.
    // Return 0 if 'object' has the expected values, and a non-zero value
    // otherwise.
{
    ASSERTV(expectedSize, object.size(), expectedSize == object.size());

    if(expectedSize != object.size()) {
        return static_cast<size_t>(-1);                               // RETURN
    }

    for (size_t i = 0; i < expectedSize; ++i) {
        if (!(object.front() == expectedValues[i])) {
            return i + 1;                                             // RETURN
        }
        object.pop();
    }
    return 0;
}

template <class VALUE, class CONTAINER>
bool TestDriver<VALUE, CONTAINER>::use_same_allocator(
                                               Obj&                 object,
                                               int                  TYPE_ALLOC,
                                               bslma::TestAllocator *ta)
{
    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());
    const TestValues VALUES;

    if (0 == TYPE_ALLOC) {  // If 'VALUE' does not use allocator, return true.
        return true;                                                  // RETURN
    }

    const bsls::Types::Int64 BB = ta->numBlocksTotal();
    const bsls::Types::Int64  B = ta->numBlocksInUse();

    object.push(VALUES[0]);

    const bsls::Types::Int64 AA = ta->numBlocksTotal();
    const bsls::Types::Int64  A = ta->numBlocksInUse();

    if (BB + TYPE_ALLOC <= AA && B + TYPE_ALLOC <= A) {
        return true;                                                  // RETURN
    }

    return false;
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::populate_container(
                                                   CONTAINER&        container,
                                                   const char*       SPEC,
                                                   size_t            length)
{
    bslma::DefaultAllocatorGuard guard(&bslma::NewDeleteAllocator::
                                                                  singleton());
    const TestValues VALUES;

    for (size_t i = 0; i < length; ++i) {
        container.push_back(VALUES[SPEC[i] - 'A']);
    }
}


template <class VALUE, class CONTAINER>
int TestDriver<VALUE, CONTAINER>::ggg(Obj        *object,
                                      const char *spec,
                                      int         verbose)
{
    bslma::DefaultAllocatorGuard guard(&bslma::NewDeleteAllocator::
                                                                  singleton());
    const TestValues VALUES;

    enum { SUCCESS = -1 };

    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Z') {
            object->push(VALUES[spec[i] - 'A']);
        }
        else {
            if (verbose) {
                printf("Error, bad character ('%c') "
                       "in spec \"%s\" at position %d.\n", spec[i], spec, i);
            }

            // Discontinue processing this spec.

            return i;                                                 // RETURN
        }
   }
   return SUCCESS;
}

template <class VALUE, class CONTAINER>
queue<VALUE, CONTAINER>&
TestDriver<VALUE, CONTAINER>::gg(Obj *object, const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

                                 // ----------
                                 // TEST CASES
                                 // ----------

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase20()
{
    // ------------------------------------------------------------------------
    // 'noexcept' SPECIFICATION
    //
    // Concerns:
    //: 1 The 'noexcept' specification has been applied to all class interfaces
    //:   required by the standard.
    //
    // Plan:
    //: 1 Apply the uniary 'noexcept' operator to expressions that mimic those
    //:   appearing in the standard and confirm that calculated boolean value
    //:   matches the expected value.
    //:
    //: 2 Since the 'noexcept' specification does not vary with the 'TYPE' of
    //:   the container, we need test for just one general type and any 'TYPE'
    //:   specializations.
    //
    // Testing:
    //   CONCERN: Methods qualified 'noexcept' in standard are so implemented.
    // ------------------------------------------------------------------------

    if (verbose) {
        P(bsls::NameOf<VALUE>())
        P(bsls::NameOf<CONTAINER>())
    }

    // N4594: 23.6.4.1 'queue' definition

    // page 900
    //..
    //     void swap(queue& q) noexcept(is_nothrow_swappable_v<Container>)
    //         { using std::swap; swap(c, q.c); }
    //..

    {
        Obj x;
        Obj q;

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
        ASSERT(bsl::is_nothrow_swappable<CONTAINER>::value ==
               BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.swap(q)));
#endif
    }

    // page 900
    //..
    //     template <class T, class Container>
    //     void swap(queue<T, Container>& x, queue<T, Container>& y)
    //         noexcept(noexcept(x.swap(y)));
    //..

    {
        Obj x;
        Obj y;

        ASSERT(false == BSLS_KEYWORD_NOEXCEPT_OPERATOR(swap(x, y)));
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase19MoveOnlyType()
{
    // ------------------------------------------------------------------------
    // MOVE MANIPULATORS FOR MOVE ONLY TYPES
    //
    // Concerns:
    //: 1 The implementation of the move manipulator methods do not rely on the
    //:   (non-existent) copy construction or copy assignment methods of the
    //:   contained type.
    //
    // Plan:
    //: 1 Instantiate this test method for the instrumented helper container
    //:   class, 'MovableVector', using 'bsltf::MoveOnlyAllocTestType' for the
    //:   contained value type.
    //:
    //: 2 Recast the tests of 'testCase19' so there is no reliance on copy
    //:   construction or copy assignment.
    //
    // Testing:
    //   operator=(MovableRef queue)
    //   emplace(Args&&.. args)
    //   push(MovableRef value)
    // ------------------------------------------------------------------------

    enum { k_MAX_NUM_PARAMS = 10 };

    const int  TYPE_ALLOC            = bslma::UsesBslmaAllocator<VALUE>::value;
    const bool is_special_container  =
                        SpecialContainerTrait<CONTAINER>::is_special_container;
   const bool is_copy_constructible = bsl::is_copy_constructible<VALUE>::value;

    if (verbose) {
            P_(bsls::NameOf<CONTAINER>())
            P_(bsls::NameOf<VALUE>())
            P_(is_special_container)
            P_(is_copy_constructible)
            P (TYPE_ALLOC)
    }

    ASSERT( is_special_container);
    ASSERT(!is_copy_constructible);

    if (verbose) { printf("Movable 'push'"); }
    {
        const CalledMethod expectedPushMethod = e_PUSH_BACK_MOVE;
        const int          count              = 3;

        Obj mX; const Obj& X = mX;  // test object for 'push'

        for (int i = 0; i < count; ++i) {

            if (veryVerbose) { P(i) }

            const VALUE lastValue(i);

            VALUE prValue1(0);
            static VALUE value0(MoveUtil::move(prValue1));

            setupCalledMethodCheck();
            VALUE prValue2(i);
            mX.push(MoveUtil::move(prValue2));
            ASSERT(isCalledMethodCheckPassed<CONTAINER>(expectedPushMethod));

            ASSERT(value0    == X.front());
            ASSERT(lastValue == X.back());
        }
    }

    if (verbose) { printf("Movable 'operator='"); }
    {
        const CalledMethod expectedAssignMethod = e_ASSIGN_MOVE;
        const int          count                = 3;

        for (int i = 0; i < count; ++i) {

            if (veryVerbose) { P(i) }

            Obj mX;
            Obj mY; const Obj& Y = mY;

            for (int j = 0; j < i; ++j) {
                VALUE prValue1(j);
                mX.push(bslmf::MovableRefUtil::move(prValue1));
                VALUE prValue2(j);
                mY.push(bslmf::MovableRefUtil::move(prValue2));
            }

            Obj mZ; const Obj& Z = mZ;

            setupCalledMethodCheck();
            mZ = MoveUtil::move(mX);
            ASSERTV(
                   i,
                   bsls::NameOf<CONTAINER>(),
                   expectedAssignMethod,
                   getCalledMethod(),
                   isCalledMethodCheckPassed<CONTAINER>(expectedAssignMethod));

            ASSERT(Y == Z);
        }
    }

    if (verbose) { printf("'emplace'"); }
    {
        Obj mA; const Obj& A = mA;  // test   object  for 'emplace'
        Obj mB; const Obj& B = mB;  // control object for 'emplace'

        (void) A;  // Compiler warnings suppression.
        (void) B;  // Compiler warnings suppression.

        for (int numArgs = 0; numArgs < k_MAX_NUM_PARAMS; ++numArgs) {

            if (veryVerbose) { P(numArgs) }

            VALUE *addressOfResult = 0;

            CalledMethod expectedEmplacePush =
                    static_cast<CalledMethod>(static_cast<int>(e_EMPLACE_0)
                                                                   << numArgs);
            setupCalledMethodCheck();

            switch (numArgs) {
              case  0: {
                VALUE& result = mA.emplace();
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case  1: {
                VALUE& result = mA.emplace(0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case  2: {
                VALUE& result = mA.emplace(0, 0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case  3: {
                VALUE& result = mA.emplace(0, 0, 0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case  4: {
                VALUE& result = mA.emplace(0, 0, 0, 0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case  5: {
                VALUE& result = mA.emplace(0, 0, 0, 0, 0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case  6: {
                VALUE& result = mA.emplace(0, 0, 0, 0, 0, 0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case  7: {
                VALUE& result = mA.emplace(0, 0, 0, 0, 0, 0, 0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case  8: {
                VALUE& result = mA.emplace(0, 0, 0, 0, 0, 0, 0, 0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case  9: {
                VALUE& result = mA.emplace(0, 0, 0, 0, 0, 0, 0, 0, 0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case 10: {
                VALUE& result = mA.emplace(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              default:
                  ASSERT(!"'value' not in range '[0, k_MAX_NUM_PARAMS]'");
            }
            ASSERTV(
                   numArgs,
                   bsls::NameOf<CONTAINER>(),
                   expectedEmplacePush,
                   getCalledMethod(),
                   isCalledMethodCheckPassed<CONTAINER>(expectedEmplacePush));

            const VALUE *ADDRESS_OF_LAST_VALUE =
                                               bsls::Util::addressOf(A.back());
            ASSERTV(numArgs, bsls::NameOf<CONTAINER>(),
                    ADDRESS_OF_LAST_VALUE == addressOfResult);

            // Track expected value of 'A'.  Note that the 'emplace' methods of
            // '(Non)?MovableVector' append 'VALUE(1)' regardless the number
            // and value of their arguments.

            VALUE prValue(1);
            mB.push(bslmf::MovableRefUtil::move(prValue));

            ASSERTV(A.size(), B.size(), B == A);
        }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase18MoveOnlyType()
{
    // ------------------------------------------------------------------------
    // MOVE CONSTRUCTORS FOR MOVE ONLY TYPES
    //
    // Concerns:
    //: 1 The implementation of the move constructors do not rely on the
    //:   (non-existent) copy construction and copy assignment methods of the
    //:   contained type.
    //
    // Plan:
    //: 1 Instantiate this test method for the instrumented helper container
    //:   class, 'MovableVector', using 'bsltf::MoveOnlyAllocTestType' for the
    //:   contained value type.
    //:
    //: 2 Recast the tests of 'testCase18' so there is no reliance on copy
    //:   construction or copy assignment.
    //
    // Testing:
    //   queue(MovableRef container);
    //   queue(MovableRef original);
    //   queue(MovableRef container, const ALLOCATOR& allocator);
    //   queue(MovableRef original,  const ALLOCATOR& allocator);
    // ------------------------------------------------------------------------

    const int  TYPE_ALLOC            = bslma::UsesBslmaAllocator<VALUE>::value;
    const bool is_special_container  =
                        SpecialContainerTrait<CONTAINER>::is_special_container;
   const bool is_copy_constructible = bsl::is_copy_constructible<VALUE>::value;

    if (verbose) {
            P_(bsls::NameOf<CONTAINER>())
            P_(bsls::NameOf<VALUE>())
            P_(is_special_container)
            P_(is_copy_constructible)
            P (TYPE_ALLOC)
    }

    ASSERT( is_special_container);
    ASSERT(!is_copy_constructible);

    {
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;
        const TestValues VALUES;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE = DATA[ti].d_line;     // source line number
            const char *const SPEC = DATA[ti].d_spec;

            if (veryVerbose) {
                T_ P_(LINE) P(SPEC);
            }

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator sa("source" , veryVeryVeryVerbose);

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;  // how we call the constructor

                if (veryVerbose) {
                    T_ T_ P(CONFIG);
                }

                // Create source object
                Obj       *pX  = new Obj(&sa);
                Obj&       mX  = *pX;

                // Create control object
                Obj        mZ;

                // Create value ('CONTAINER') object
                CONTAINER  mC(&sa);

                // Install default allocator.
                bslma::DefaultAllocatorGuard dag(&da);

                bslma::TestAllocator  ta("target",    veryVeryVeryVerbose);
                bslma::TestAllocator  fa("footprint", veryVeryVeryVerbose);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                setupCalledMethodCheck();

                CalledMethod expectedCtor;

                switch (CONFIG) {
                  case 'a': {
                      objPtr          = new (fa) Obj(MoveUtil::move(mX));
                      objAllocatorPtr = &sa;
                      expectedCtor    = e_CTOR_MOV_SANS_ALLOC;
                  } break;
                  case 'b': {
                      objPtr          = new (fa) Obj(MoveUtil::move(mX),
                                                     (bslma::Allocator *)0);
                      objAllocatorPtr = &da;
                      expectedCtor    = e_CTOR_MOV_AVEC_ALLOC;
                  } break;
                  case 'c': {
                      objPtr          = new (fa) Obj(MoveUtil::move(mX), &ta);
                      objAllocatorPtr = &ta;
                      expectedCtor    = e_CTOR_MOV_AVEC_ALLOC;
                  } break;
                  case 'd': {
                      objPtr          = new (fa) Obj(MoveUtil::move(mC));
                      objAllocatorPtr = &sa;
                      expectedCtor    = e_CTOR_MOV_SANS_ALLOC;
                  } break;
                  case 'e': {
                      objPtr          = new (fa) Obj(MoveUtil::move(mC),
                                                     (bslma::Allocator *)0);
                      objAllocatorPtr = &da;
                      expectedCtor    = e_CTOR_MOV_AVEC_ALLOC;
                  } break;
                  case 'f': {
                      objPtr          = new (fa) Obj(MoveUtil::move(mC), &ta);
                      objAllocatorPtr = &ta;
                      expectedCtor    = e_CTOR_MOV_AVEC_ALLOC;
                  } break;
                  default: {
                      ASSERTV(LINE, SPEC, CONFIG, !"Bad constructor config.");
                      return;                                         // RETURN
                  } break;
                }

                Obj& mY = *objPtr;  const Obj& Y = mY;  // test object

                ASSERTV(
                  bsls::NameOf<CONTAINER>(),
                  LINE,
                  SPEC,
                  expectedCtor,
                  getCalledMethod(),
                  true == isCalledMethodCheckPassed<CONTAINER>(expectedCtor));

                ASSERTV(LINE, SPEC, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                // 'use_same_allocator' does not work with move=only types for
                // now, so just mark that we aren't going to make use of these
                // variables
                (void)objAllocatorPtr;
                (void)Y;

                // Reclaim dynamically allocated source object.

                delete pX;

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(LINE, SPEC, CONFIG, fa.numBlocksInUse(),
                                       0 == fa.numBlocksInUse());
                ASSERTV(LINE, SPEC, CONFIG, ta.numBlocksInUse(),
                                       0 == ta.numBlocksInUse());
            }

            ASSERTV(LINE, SPEC, da.numBlocksInUse(), 0 == da.numBlocksInUse());
            ASSERTV(LINE, SPEC, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        }
    }
}

template <class VALUE, class CONTAINER>
template <bool PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver<VALUE, CONTAINER>::
                   testCase19_propagate_on_container_move_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   VALUE,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG>
                                        StdAlloc;

    typedef bsl::deque<VALUE, StdAlloc> CObj;
    typedef bsl::queue<VALUE, CObj>     Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };
    const int NUM_SPECS = static_cast<int>(sizeof SPECS / sizeof *SPECS);

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
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

        const CObj CI(IVALUES.begin(), IVALUES.end(), scratch);

        const Obj W(CI, scratch);  // control

        // Create target object.
        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            {
                Obj mY(CI, mas);  const Obj& Y = mY;

                if (veryVerbose) { T_ P_(ISPEC) P_(Y) P(W) }

                const CObj CJ(JVALUES.begin(), JVALUES.end(), scratch);

                Obj mX(CJ, mat);  const Obj& X = mX;

                bslma::TestAllocatorMonitor oasm(&oas);
                bslma::TestAllocatorMonitor oatm(&oat);

                Obj *mR = &(mX = MoveUtil::move(mY));

                ASSERTV(ISPEC, JSPEC,  W,   X,  W == X);
                ASSERTV(ISPEC, JSPEC, mR, &mX, mR == &mX);

// TBD no 'get_allocator' in 'queue'
#if 0
                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                       !PROPAGATE == (mat == X.get_allocator()));
                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                        PROPAGATE == (mas == X.get_allocator()));

                ASSERTV(ISPEC, JSPEC, mas == Y.get_allocator());
#endif

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

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::
                            testCase19_propagate_on_container_move_assignment()
{
    // ------------------------------------------------------------------------
    // MOVE-ASSIGNMENT OPERATOR: ALLOCATOR PROPAGATION
    //
    // Concerns:
    //: 1 If the 'propagate_on_container_move_assignment' trait is 'false', the
    //:   allocator used by the target object remains unchanged (i.e., the
    //:   source object's allocator is *not* propagated).
    //:
    //: 2 If the 'propagate_on_container_move_assignment' trait is 'true', the
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
    //: 5 The effect of the 'propagate_on_container_move_assignment' trait is
    //:   independent of the other three allocator propagation traits.
    //
    // Plan:
    //: 1 Specify a set S of object values with varied differences, ordered by
    //:   increasing length, to be used in the following tests.
    //:
    //: 2 Create two 'bsltf::StdStatefulAllocator' objects with their
    //:   'propagate_on_container_move_assignment' property configured to
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
    //:   3 Move-assign 'Y' to 'X' and use 'operator==' to verify that 'X'
    //:     subsequently has the same value as 'W'.
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
    //   propagate_on_container_move_assignment
    // ------------------------------------------------------------------------

    if (verbose) printf("\nMOVE-ASSIGNMENT OPERATOR: ALLOCATOR PROPAGATION"
                        "\n===============================================\n");

    if (verbose)
        printf("\n'propagate_on_container_move_assignment::value == false'\n");

    testCase19_propagate_on_container_move_assignment_dispatch<false, false>();
    testCase19_propagate_on_container_move_assignment_dispatch<false, true>();

    if (verbose)
        printf("\n'propagate_on_container_move_assignment::value == true'\n");

    testCase19_propagate_on_container_move_assignment_dispatch<true, false>();
    testCase19_propagate_on_container_move_assignment_dispatch<true, true>();
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase19(bool isMovableContainer)
{
    // ------------------------------------------------------------------------
    // MOVE MANIPULATORS:
    //
    // Concerns:
    //: 1 Each of the methods under test correctly forwards its arguments to
    //:   the corresponding method of the underlying 'CONTAINER' when that
    //:   container provides those "move" methods, and to the expected
    //:   alternate methods otherwise.
    //:
    //: 2 The reference returned from the assignment operator is to the target
    //:   object (i.e., '*this').
    //
    //:
    //: 3 'emplace_back' returns a reference to the inserted element.
    //
    // Plan:
    //: 1 Instantiate this test method for the two instrumented helper
    //:   container classes: 'NonMovableVector' and 'MovableVector'.
    //:
    //: 2 Use loop-based tests that iterate for a small number of values.  Use
    //:   3 different values for the 'push' and assignment tests.  The
    //:   'emplace' tests a different number of parameters on each test.  Those
    //:   require 10 iterations to address each of the 10 overloads used when
    //:   CPP11 support is not available.
    //:
    //: 3 For each test create a "control" object that has the expected value
    //:   of the object under test.  Create the control object using the
    //:   previously tested (non-movable) 'push' method.
    //:
    //: 4 Invoke the method under test on the object under test.  Confirm that
    //:   the expected enumerated value was set in the global variable.
    //:   Confirm that the test object has the expected value.  Confirm that
    //:   the expected value is returned (if any).
    //
    // Testing:
    //   operator=(MovableRef queue)
    //   emplace(Args&&.. args)
    //   push(MovableRef value)
    // ------------------------------------------------------------------------

    // typedef typename CONTAINER::value_type VALUE;

    enum { k_MAX_NUM_PARAMS = 10 };

    const int TYPE_ALLOC            = bslma::UsesBslmaAllocator<VALUE>::value;

    const bool is_special_container =
                        SpecialContainerTrait<CONTAINER>::is_special_container;

  //const int NUM_DATA                     = DEFAULT_NUM_DATA;
  //const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;
    const TestValues VALUES;

    if (verbose) {
            P_(bsls::NameOf<CONTAINER>())
            P_(bsls::NameOf<VALUE>())
            P_(is_special_container)
            P (TYPE_ALLOC)
    }

    ASSERT(is_special_container);

    if (verbose) { printf("Movable 'push'"); }
    {
        Obj mX; const Obj& X = mX;  // test    object for 'push'
        Obj mY; const Obj& Y = mY;  // control object for 'push'

        CalledMethod expectedPushMethod = isMovableContainer
                                          ? e_PUSH_BACK_MOVE
                                          : e_PUSH_BACK_CREF;

        for (int i = 0; i < 3; ++i) {

            if (veryVerbose) { P(i) }

            VALUE value          = VALUES[i];
            VALUE valueToBeMoved = value;

            setupCalledMethodCheck();
            mX.push(MoveUtil::move(valueToBeMoved));
            ASSERT(isCalledMethodCheckPassed<CONTAINER>(expectedPushMethod));

            setupCalledMethodCheck();
            mY.push(               value);
            ASSERT(isCalledMethodCheckPassed<CONTAINER>(e_PUSH_BACK_CREF));

            ASSERT(Y == X);
        }
    }

    if (verbose) { printf("Movable 'operator='"); }
    {
        CalledMethod expectedAssignMethod = isMovableContainer
                                            ? e_ASSIGN_MOVE
                                            : e_ASSIGN_CREF;
        Obj mX; const Obj& X = mX;  // test object for 'push'

        for (int i = 0; i < 3; ++i) {

            if (veryVerbose) { P(i) }

            VALUE value = VALUES[i];

            Obj mU;     const Obj& U = mU;   // test      object
            Obj mV;     const Obj& V = mV;   // control   object

            mX.push(value);
            Obj mT(X);                       // sacrifice object

            Obj *mR = 0;

            setupCalledMethodCheck();
            mR = &(mU = MoveUtil::move(mT));

            ASSERTV(bsls::Util::addressOf(U) == mR);

            ASSERTV(
                   i,
                   bsls::NameOf<CONTAINER>(),
                   expectedAssignMethod,
                   getCalledMethod(),
                   isCalledMethodCheckPassed<CONTAINER>(expectedAssignMethod));

            ASSERT(U == X);

            setupCalledMethodCheck();
            mV = X;
            ASSERTV(i,
                    bsls::NameOf<CONTAINER>(),
                    expectedAssignMethod,
                    getCalledMethod(),
                    isCalledMethodCheckPassed<CONTAINER>(e_ASSIGN_CREF));
            ASSERT(V == X);

            ASSERT(U == V);
        }
    }

    if (verbose) { printf("'emplace'"); }
    {
        Obj mA; const Obj& A = mA;  // test   object  for 'emplace'
        Obj mB; const Obj& B = mB;  // control object for 'emplace'

        (void) A;  // Compiler warnings suppression.
        (void) B;  // Compiler warnings suppression.

        for (int numArgs = 0; numArgs < k_MAX_NUM_PARAMS; ++numArgs) {

            if (veryVerbose) { P(numArgs) }

            VALUE *addressOfResult = 0;

            CalledMethod expectedEmplacePush =
                    static_cast<CalledMethod>(static_cast<int>(e_EMPLACE_0)
                                                                   << numArgs);
            setupCalledMethodCheck();

            switch (numArgs) {
             case  0: {
                VALUE& result = mA.emplace();
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case  1: {
                VALUE& result = mA.emplace(0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case  2: {
                VALUE& result = mA.emplace(0, 0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case  3: {
                VALUE& result = mA.emplace(0, 0, 0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case  4: {
                VALUE& result = mA.emplace(0, 0, 0, 0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case  5: {
                VALUE& result = mA.emplace(0, 0, 0, 0, 0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case  6: {
                VALUE& result = mA.emplace(0, 0, 0, 0, 0, 0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case  7: {
                VALUE& result = mA.emplace(0, 0, 0, 0, 0, 0, 0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case  8: {
                VALUE& result = mA.emplace(0, 0, 0, 0, 0, 0, 0, 0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case  9: {
                VALUE& result = mA.emplace(0, 0, 0, 0, 0, 0, 0, 0, 0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              case 10: {
                VALUE& result = mA.emplace(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                addressOfResult = bsls::Util::addressOf(result);
              } break;
              default:
                  ASSERT(!"'value' not in range '[0, k_MAX_NUM_PARAMS]'");
            }
            ASSERTV(
                   numArgs,
                   bsls::NameOf<CONTAINER>(),
                   expectedEmplacePush,
                   getCalledMethod(),
                   isCalledMethodCheckPassed<CONTAINER>(expectedEmplacePush));

            const VALUE *ADDRESS_OF_LAST_VALUE =
                                               bsls::Util::addressOf(A.back());
            ASSERTV(numArgs, bsls::NameOf<CONTAINER>(),
                    ADDRESS_OF_LAST_VALUE == addressOfResult);

            // Track expected value of 'A'.  Note that the 'emplace' methods of
            // '(Non)?MovableVector' append 'VALUE(1)' regardless the number
            // and value of their arguments.

            mB.push(VALUE(1));

            ASSERTV(A.size(), B.size(), B == A);
        }
    }
}


template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase18(bool isMovableContainer)
{
    // ------------------------------------------------------------------------
    // MOVE CONSTRUCTORS:
    //   Ensure that we can construct any object of the class, having other
    //   object of the class as the source.  To provide backward compatibility,
    //   copy constructor should be used in the absence of move constructor.
    //   We are going to use two special containers 'NonMovableVector' and
    //   'MovableVector', that register called method, to verify it.
    //
    // Concerns:
    //: 1 Appropriate constructor of underlying container (move or copy) is
    //:   called.
    //:
    //: 2 The new object has the same value as the source object.
    //:
    //: 3 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 4 The source object is left in a valid but unspecified state.
    //:
    //: 5 No additional memory is allocated by the target object.
    //:
    //: 5 If an allocator is NOT supplied to the constructor, the allocator of
    //:   the source object in effect at the time of construction becomes the
    //:   object allocator for the resulting object.
    //:
    //: 6 If an allocator IS supplied to the constructor, that allocator
    //:   becomes the object allocator for the resulting object.
    //:
    //: 7 If a null allocator address IS supplied to the constructor, the
    //:   default allocator in effect at the time of construction becomes the
    //:   object allocator for the resulting object.
    //:
    //: 8 Supplying an allocator to the constructor has no effect on subsequent
    //:   object values.
    //:
    //: 9 Subsequent changes to or destruction of the source object have no
    //:   effect on the move-constructed object and vice-versa.
    //:
    //:10 Every object releases any allocated memory at destruction.
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid source object values.
    //:
    //:   2 Specify a set of (unique) valid value ('CONTAINER') objects.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:
    //:
    //:   1 Execute an inner loop creating three distinct objects, in turn,
    //:     each object having the same value, 'V', but configured differently
    //:     identified by 'CONFIG':
    //:
    //:     'a': passing a source object without passing an allocator;
    //:
    //:     'b': passing a source object and an explicit null allocator;
    //:
    //:     'c': passing a source object and the address of a test allocator
    //:          distinct from the default and source object's allocators.
    //:
    //:     'd': passing a value object without passing an allocator;
    //:
    //:     'e': passing a value object and an explicit null allocator;
    //:
    //:     'f': passing a value object and the address of a test allocator
    //:          distinct from the default and source object's allocators.
    //:
    //:   2 For each of the four iterations in P-2.1:
    //:
    //:     1 Use the value constructor with 'sa' allocator to create dynamic
    //:       source object 'mX' and control object 'mZ', each having the value
    //:       'V'.
    //:
    //:     2 Create a 'bslma_TestAllocator' object, and install it as the
    //:       default allocator (note that a ubiquitous test allocator is
    //:       already installed as the global allocator).
    //:
    //:     3 Choose the move constructor depending on 'CONFIG' to dynamically
    //:       create an object, 'mY', using movable reference of 'mX'.
    //:
    //:     4 Verify that the appropriate constructor of underlying container
    //:       has been called.  Note that this check is skipped for all classes
    //:       except special containers 'NonMovableVector' and 'MovableVector'.
    //:       (C-1)
    //:
    //:     5 Use the appropriate test allocator to verify that no additional
    //:       memory is allocated by the target object.  (C-5)
    //:
    //:     6 Use the helper function 'use_same_allocator' to verify each
    //:       underlying attribute capable of allocating memory to ensure that
    //:       its object allocator is properly installed.  (C-6..9)
    //:
    //:     7 Use the helper function 'use_same_comparator' to verify that the
    //:       target object, 'mY', has the same comparator as that of 'mZ', to
    //:       ensure that the new object's comparator is properly installed.
    //:       (C-2..3)
    //:
    //:     8 Add some values to the source and target object separately.
    //:       Verify that they change independently.  Destroy source object.
    //:       Verify that target object is unaffected.  (C-4, 10)
    //:
    //:     9 Delete the target object and let the control object go out of
    //:       scope to verify, that all memory has been released.  (C-11)
    //
    // Testing:
    //   queue(MovableRef container);
    //   queue(MovableRef original);
    //   queue(MovableRef container, const ALLOCATOR& allocator);
    //   queue(MovableRef original,  const ALLOCATOR& allocator);
    // ------------------------------------------------------------------------

    // typedef typename CONTAINER::value_type VALUE;

    const int TYPE_ALLOC            = bslma::UsesBslmaAllocator<VALUE>::value;

    const bool is_special_container =
                        SpecialContainerTrait<CONTAINER>::is_special_container;

    if (verbose) {
            P_(bsls::NameOf<CONTAINER>())
            P_(bsls::NameOf<VALUE>())
            P_(is_special_container)
            P (TYPE_ALLOC)
    }

    ASSERT(is_special_container);

    {
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;
        const TestValues VALUES;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE = DATA[ti].d_line;     // source line number
            const char *const SPEC = DATA[ti].d_spec;

            if (veryVerbose) {
                T_ P_(LINE) P(SPEC);
            }

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator sa("source" , veryVeryVeryVerbose);

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;  // how we call the constructor

                if (veryVerbose) {
                    T_ T_ P(CONFIG);
                }

                // Create source object
                Obj        *pX = new Obj(&sa);
                Obj&        mX = gg(pX, SPEC);
                const Obj&  X = mX;

                // Create control object
                Obj        mZ; const Obj&       Z = gg(&mZ, SPEC);

                // Create value ('CONTAINER') object
                      CONTAINER  mC(&sa);
                const CONTAINER& C = CONTAINER::GG(&mC, SPEC);
                (void) C;

                // Install default allocator.
                bslma::DefaultAllocatorGuard dag(&da);

                bslma::TestAllocator  ta("target",    veryVeryVeryVerbose);
                bslma::TestAllocator  fa("footprint", veryVeryVeryVerbose);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                setupCalledMethodCheck();

                CalledMethod expectedCtor;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj(MoveUtil::move(mX));
                      objAllocatorPtr = isMovableContainer
                                        ? &sa
                                        : &da;
                      expectedCtor    = isMovableContainer
                                        ? e_CTOR_MOV_SANS_ALLOC
                                        : e_CTOR_CPY_SANS_ALLOC;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(MoveUtil::move(mX),
                                                        (bslma::Allocator *)0);
                      objAllocatorPtr = &da;
                      expectedCtor    = isMovableContainer
                                        ? e_CTOR_MOV_AVEC_ALLOC
                                        : e_CTOR_CPY_AVEC_ALLOC;
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(MoveUtil::move(mX), &ta);
                      objAllocatorPtr = &ta;
                      expectedCtor    = isMovableContainer
                                        ? e_CTOR_MOV_AVEC_ALLOC
                                        : e_CTOR_CPY_AVEC_ALLOC;
                  } break;
                  case 'd': {
                      objPtr = new (fa) Obj(MoveUtil::move(mC));
                      objAllocatorPtr = isMovableContainer
                                        ? &sa
                                        : &da;
                      expectedCtor    = isMovableContainer
                                        ? e_CTOR_MOV_SANS_ALLOC
                                        : e_CTOR_CPY_SANS_ALLOC;
                  } break;
                  case 'e': {
                      objPtr = new (fa) Obj(MoveUtil::move(mC),
                                                        (bslma::Allocator *)0);
                      objAllocatorPtr = &da;
                      expectedCtor    = isMovableContainer
                                        ? e_CTOR_MOV_AVEC_ALLOC
                                        : e_CTOR_CPY_AVEC_ALLOC;
                  } break;
                  case 'f': {
                      objPtr = new (fa) Obj(MoveUtil::move(mC), &ta);
                      objAllocatorPtr = &ta;
                      expectedCtor    = isMovableContainer
                                        ? e_CTOR_MOV_AVEC_ALLOC
                                        : e_CTOR_CPY_AVEC_ALLOC;
                  } break;
                  default: {
                      ASSERTV(LINE, SPEC, CONFIG, !"Bad constructor config.");
                      return;                                         // RETURN
                  } break;
                }

                Obj& mY = *objPtr;  const Obj& Y = mY;  // test object

                ASSERTV(
                  bsls::NameOf<CONTAINER>(),
                  LINE,
                  SPEC,
                  expectedCtor,
                  getCalledMethod(),
                  true == isCalledMethodCheckPassed<CONTAINER>(expectedCtor));

                ASSERTV(LINE, SPEC, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                // Verify correctness of the contents moving.

                ASSERTV(LINE, SPEC, CONFIG, Z == Y);

                // Verify any attribute allocators are installed properly.

                bool objectUsesCorrectAllocatorFlag =
                                           use_same_allocator(mY,
                                                              TYPE_ALLOC,
                                                              objAllocatorPtr);
                ASSERTV(LINE, SPEC, CONFIG,
                        true == objectUsesCorrectAllocatorFlag);

                // Verify independence of the target object from the source
                // one.

                size_t sourceSize = X.size();
                size_t targetSize = Y.size();

                mX.push(VALUES[0]);

                ASSERTV(LINE, SPEC, CONFIG, sourceSize != X.size());
                ASSERTV(LINE, SPEC, CONFIG, targetSize == Y.size());

                sourceSize = X.size();

                mY.push(VALUES[0]);

                ASSERTV(LINE, SPEC, CONFIG, sourceSize == X.size());
                ASSERTV(LINE, SPEC, CONFIG, targetSize != Y.size());

                targetSize       = Y.size();
                const VALUE top  = Y.back();

                // Reclaim dynamically allocated source object.

                delete pX;

                ASSERTV(LINE, SPEC, CONFIG, top        == Y.back());
                ASSERTV(LINE, SPEC, CONFIG, targetSize == Y.size());

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(LINE, SPEC, CONFIG, fa.numBlocksInUse(),
                                       0 == fa.numBlocksInUse());
                ASSERTV(LINE, SPEC, CONFIG, ta.numBlocksInUse(),
                                       0 == ta.numBlocksInUse());
            }

            ASSERTV(LINE, SPEC, da.numBlocksInUse(), 0 == da.numBlocksInUse());
            ASSERTV(LINE, SPEC, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING COMPARISON FREE OPERATORS
    //
    // Concerns:
    //: 1 'operator<' returns the lexicographic comparison on two objects.
    //:
    //: 2 'operator>', 'operator<=', and 'operator>=' are correctly tied to
    //:   'operator<'.  i.e., For two objects, 'a' and 'b':
    //:
    //:   1 '(a > b) == (b < a)'
    //:
    //:   2 '(a <= b) == !(b < a)'
    //:
    //:   3 '(a >= b) == !(a < b)'
    //:
    //: 3 'operator<=>' is consistent with '<', '>', '<=', '>='.
    //
    // Plan:
    //: 1 For a variety of objects of different sizes and different values,
    //:   test that the comparison returns as expected.  (C-1..3)
    //
    // Testing:
    //   bool operator< (const queue<V, C>& lhs, const queue<V, C>& rhs);
    //   bool operator> (const queue<V, C>& lhs, const queue<V, C>& rhs);
    //   bool operator>=(const queue<V, C>& lhs, const queue<V, C>& rhs);
    //   bool operator<=(const queue<V, C>& lhs, const queue<V, C>& rhs);
    //   auto operator<=>(const queue<V, C>& lhs, const queue<V, C>& rhs);
    // ------------------------------------------------------------------------

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\nCompare each pair of similar and different"
                        " values (u, ua, v, va) in S X A X S X A"
                        " without perturbation.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec;
            const size_t      LENGTH1 = strlen(DATA[ti].d_spec);

           if (veryVerbose) { T_ P_(LINE1) P_(INDEX1) P_(LENGTH1) P(SPEC1) }

            // Ensure an object compares correctly with itself (alias test).

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&scratch); const Obj& X = gg(&mX, SPEC1);

                ASSERTV(LINE1, X,   X == X);
                ASSERTV(LINE1, X, !(X != X));
            }

            // Create second object.

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;
                const size_t      LENGTH2 = strlen(DATA[tj].d_spec);

                if (veryVerbose) {
                              T_ T_ P_(LINE2) P_(INDEX2) P_(LENGTH2) P(SPEC2) }

                // Create two distinct test allocators, 'oax' and 'oay'.

                bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                // Map allocators above to objects 'X' and 'Y' below.

                Obj mX(&oax); const Obj& X = gg(&mX, SPEC1);
                Obj mY(&oay); const Obj& Y = gg(&mY, SPEC2);

                if (veryVerbose) { T_ T_ P_(X) P(Y); }

                // Verify value and no memory allocation.

                bslma::TestAllocatorMonitor oaxm(&oax);
                bslma::TestAllocatorMonitor oaym(&oay);

                const bool isLess = INDEX1 < INDEX2;
                const bool isLessEq = INDEX1 <= INDEX2;

                ASSERTV(LINE1, LINE2,  isLess   == (X < Y));
                ASSERTV(LINE1, LINE2, !isLessEq == (X > Y));
                ASSERTV(LINE1, LINE2,  isLessEq == (X <= Y));
                ASSERTV(LINE1, LINE2, !isLess   == (X >= Y));

#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
                if constexpr (bsl::three_way_comparable<CONTAINER>) {
                    ASSERTV(LINE1, LINE2,  isLess   == (X <=> Y <  0));
                    ASSERTV(LINE1, LINE2, !isLessEq == (X <=> Y >  0));
                    ASSERTV(LINE1, LINE2,  isLessEq == (X <=> Y <= 0));
                    ASSERTV(LINE1, LINE2, !isLess   == (X <=> Y >= 0));
                }
#endif

                ASSERTV(LINE1, LINE2, oaxm.isTotalSame());
                ASSERTV(LINE1, LINE2, oaym.isTotalSame());
            }
        }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING 'empty'
    //
    // Concern:
    //: 1 'empty' returns 'true' only when the object is empty.
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:  (C-1)
    //:
    //:   1 Create an object and populate it with 'V'.  Invoke 'empty' to
    //:     verify it returns 'true' if 'V' is not empty.  (C-1)
    //:
    //:   2 Clear the object by popping out 'LENGTH' of elements, where
    //:     'LENGTH' is the length of 'V'.  Invoke 'empty' to verify it returns
    //:     'false'.  (C-1)
    //
    // Testing:
    //  bool empty() const;
    // ------------------------------------------------------------------------

    bslma::TestAllocator  oa(veryVeryVerbose);

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // initial
    } DATA[] = {
        { L_,  ""          },
        { L_,  "A"         },
        { L_,  "ABC"       },
        { L_,  "ABCD"      },
        { L_,  "ABCDE"     },
        { L_,  "ABCDEFG"   },
        { L_,  "ABCDEFGH"  },
        { L_,  "ABCDEFGHI" }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\tTesting 'empty'.\n");
    {
        const TestValues VALUES;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            // Verify 'empty' returns correct values.

            ASSERTV(LINE, SPEC, (0 == ti) == X.empty());
            ASSERTV(LINE, SPEC, (0 == ti) == bsl::empty(X));

            for (size_t tj = 0; tj < strlen(SPEC); ++tj) {
                mX.pop();
            }

            ASSERTV(LINE, SPEC, true == X.empty());
            ASSERTV(LINE, SPEC, true == bsl::empty(X));
        }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase13()
{
    // ------------------------------------------------------------------------
    // TESTING 'front' and 'back'
    //
    // Concern:
    //: 1 'front' and 'back' returns a modifiable reference to the front and
    //:   back element of the queue respectively.
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:  (C-1)
    //:
    //:   1 Invoke 'front' and 'back', use the return object as left hand
    //:     operator and assign a distinct value.  Invoke the constant version
    //:     of 'front' and 'back' to verify the changes.  (C-1)
    //
    // Testing:
    //  reference front();
    //  reference back();
    // ------------------------------------------------------------------------

    bslma::TestAllocator  oa(veryVeryVerbose);

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // initial
    } DATA[] = {
        { L_,  ""          },
        { L_,  "A"         },
        { L_,  "ABC"       },
        { L_,  "ABCD"      },
        { L_,  "ABCDE"     },
        { L_,  "ABCDEFG"   },
        { L_,  "ABCDEFGH"  },
        { L_,  "ABCDEFGHI" }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\tTesting 'empty', 'front' and 'back'.\n");
    {
        const TestValues VALUES;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(DATA[ti].d_spec);

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            if (0 != ti) {

                ASSERTV(LINE, SPEC, X.front() == VALUES[SPEC[0] - 'A']);
                ASSERTV(LINE, SPEC,
                        X.back() == VALUES[SPEC[LENGTH - 1] - 'A']);

                // Test 'front'.

                mX.front() = VALUES['Y' - 'A'];
                ASSERTV(LINE, SPEC, X.front() == VALUES['Y' - 'A']);

                // Test 'back'.

                mX.back() = VALUES['Z' - 'A'];
                ASSERTV(LINE, SPEC, X.back() == VALUES['Z' - 'A']);
            }
        }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase12()
{
    // ------------------------------------------------------------------------
    // VALUE (TEMPLATE) CONSTRUCTORS:
    //
    // Concern:
    //: 1 The object is constructed with all values in the user-supplied
    //:   container in the same order.
    //:
    //: 2 If an allocator is NOT supplied to the value constructor, the default
    //:   allocator in effect at the time of construction becomes the object
    //:   allocator for the resulting object.
    //:
    //: 3 If an allocator IS supplied to the value constructor, that allocator
    //:   becomes the object allocator for the resulting object.
    //:
    //: 4 Supplying a null allocator address has the same effect as not
    //:   supplying an allocator.
    //:
    //: 5 Supplying an allocator to the value constructor has no effect on
    //:   subsequent object values.
    //:
    //: 6 Any memory allocation is from the object allocator.
    //:
    //: 7 There is no temporary memory allocation from any allocator.
    //:
    //: 8 Every object releases any allocated memory at destruction.
    //:
    //: 9 QoI: Creating an object having the default-constructed value
    //:   allocates no memory.
    //:
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:  (C-1..8)
    //:
    //:   1 Execute an inner loop creating three distinct objects, in turn,
    //:     each object having the same value, 'V', but configured differently:
    //:     (a) without passing an allocator, (b) passing a null allocator
    //:     address explicitly, and (c) passing the address of a test allocator
    //:     distinct from the default allocator.
    //:
    //:   2 For each of the three iterations in P-2.1:  (C-1..8)
    //:
    //:     1 Insert the test data to a specialized container
    //:
    //:     2 Create three 'bslma_TestAllocator' objects, and install one as
    //:       the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     3 Use the value constructor to dynamically create an object using
    //:       the container in P-2.2.1, with its object allocator configured
    //:       appropriately (see P-2.2.2), supplying all the arguments as
    //:       'const'; use a distinct test allocator for the object's
    //:       footprint.
    //:
    //:     4 Verify that all of the attributes of each object have their
    //:       expected values.  (C-1, 5)
    //:
    //:     5 Use the appropriate test allocators to verify that:
    //:       (C-2..4, 6..8)
    //:
    //:       1 An object that IS expected to allocate memory does so from the
    //:         object allocator only (irrespective of the specific number of
    //:         allocations or the total amount of memory allocated).  (C-6)
    //:
    //:       2 An object that is expected NOT to allocate memory does not
    //:         allocate memory.
    //:
    //:       3 If an allocator was supplied at construction (P-2.1c), the
    //:         default allocator doesn't allocate any memory.  (C-6)
    //:
    //:       4 No temporary memory is allocated from the object allocator.
    //:         (C-7)
    //:
    //:       5 All object memory is released when the object is destroyed.
    //:         (C-8)
    //:
    //:     6 Use the helper function 'use_same_allocator' to verify each
    //:       underlying attribute capable of allocating memory to ensure
    //:       that its object allocator is properly installed.  (C-2..4)
    //:
    // Testing:
    //   queue(const CONTAINER& container);
    //   queue(const CONTAINER& container, const ALLOCATOR& allocator);
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\nTesting value constructors.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE    = DATA[ti].d_line;
            const char       *SPEC    = DATA[ti].d_spec;
            const TestValues  EXP(DATA[ti].d_spec);
            const size_t      LENGTH  = strlen(DATA[ti].d_spec);

            if (verbose) { P_(LINE) P_(SPEC) P(LENGTH); }

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                if (veryVerbose) { T_ T_ P(CONFIG) }

                // Insert test data to a container.

                CONTAINER container;
                populate_container(container, SPEC, LENGTH);

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                // Install default allocator.

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                 *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj(container);
                      objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(container, (bslma::Allocator*)0);
                      objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(container, &sa);
                      objAllocatorPtr = &sa;
                  } break;
                  default: {
                      ASSERTV(LINE, CONFIG, !"Bad allocator config.");
                      return;                                         // RETURN
                  } break;
                }
                ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                Obj& mX = *objPtr;  const Obj& X = mX;

                if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                // Verify no temporary memory is allocated from the object
                // allocator.

                ASSERTV(LINE, CONFIG, oa.numBlocksTotal(), oa.numBlocksInUse(),
                        oa.numBlocksTotal() == oa.numBlocksInUse());

                // Ensure the first row of the table contains the
                // default-constructed value.

                static bool firstFlag = true;
                if (firstFlag) {
                    ASSERTV(LINE, CONFIG, Obj(), *objPtr, Obj() == *objPtr);
                    firstFlag = false;
                }

                // Verify the expected attributes values.

                ASSERTV(LINE, SPEC, LENGTH,
                        0 == verify_object(mX, EXP, LENGTH));

                // Verify any attribute allocators are installed properly.

                ASSERTV(LINE, CONFIG, use_same_allocator(mX, TYPE_ALLOC, &oa));

                // Verify no allocation from the non-object allocator.

                ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa.numBlocksInUse(),
                        0 == sa.numBlocksInUse());

            }  // end foreach configuration
        }  // end foreach row
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase11()
{
    // ------------------------------------------------------------------------
    // TESTING TYPE TRAITS
    //
    // Concern:
    //: 1 The object has the necessary type traits.
    //
    // Plan:
    //: 1 Use 'BSLMF_ASSERT' to verify all the type traits exist.  (C-1)
    //
    // Testing:
    //   CONCERN: The object has the necessary type traits.
    // ------------------------------------------------------------------------

    // Verify 'queue' defines the expected traits.

    enum { CONTAINER_USES_ALLOC =
                                 bslma::UsesBslmaAllocator<CONTAINER>::value };

    BSLMF_ASSERT(
         ((int)CONTAINER_USES_ALLOC == bslma::UsesBslmaAllocator<Obj>::value));

    // Verify 'queue' does not define other common traits.

    BSLMF_ASSERT((0 == bslalg::HasStlIterators<Obj>::value));

    BSLMF_ASSERT((0 == bsl::is_trivially_copyable<Obj>::value));

    BSLMF_ASSERT((0 == bslmf::IsBitwiseEqualityComparable<Obj>::value));

    BSLMF_ASSERT((0 == bslmf::IsBitwiseMoveable<Obj>::value));

    BSLMF_ASSERT((0 == bslmf::HasPointerSemantics<Obj>::value));

    BSLMF_ASSERT((0 == bsl::is_trivially_default_constructible<Obj>::value));
}

template <class VALUE, class CONTAINER>
template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver<VALUE, CONTAINER>::
                    testCase9_propagate_on_container_copy_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   VALUE,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS> StdAlloc;

    typedef bsl::deque<VALUE, StdAlloc>         CObj;
    typedef bsl::queue<VALUE, CObj>             Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };
    const int NUM_SPECS = static_cast<int>(sizeof SPECS / sizeof *SPECS);

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
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

        const CObj CI(IVALUES.begin(), IVALUES.end(), scratch);

        const Obj W(CI, scratch);  // control

        // Create target object.
        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            {
                Obj mY(CI, mas);  const Obj& Y = mY;

                if (veryVerbose) { T_ P_(ISPEC) P_(Y) P(W) }

                const CObj CJ(JVALUES.begin(), JVALUES.end(), scratch);

                Obj mX(CJ, mat);  const Obj& X = mX;

                bslma::TestAllocatorMonitor oasm(&oas);
                bslma::TestAllocatorMonitor oatm(&oat);

                Obj *mR = &(mX = Y);

                ASSERTV(ISPEC, JSPEC,  W,   X,  W == X);
                ASSERTV(ISPEC, JSPEC,  W,   Y,  W == Y);
                ASSERTV(ISPEC, JSPEC, mR, &mX, mR == &mX);

// TBD no 'get_allocator' in 'queue'
#if 0
                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                       !PROPAGATE == (mat == X.get_allocator()));
                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                        PROPAGATE == (mas == X.get_allocator()));

                ASSERTV(ISPEC, JSPEC, mas == Y.get_allocator());
#endif

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

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::
                             testCase9_propagate_on_container_copy_assignment()
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

    testCase9_propagate_on_container_copy_assignment_dispatch<false, false>();
    testCase9_propagate_on_container_copy_assignment_dispatch<false, true>();

    if (verbose)
        printf("\n'propagate_on_container_copy_assignment::value == true'\n");

    testCase9_propagate_on_container_copy_assignment_dispatch<true, false>();
    testCase9_propagate_on_container_copy_assignment_dispatch<true, true>();
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase9()
{
    // ------------------------------------------------------------------------
    // COPY-ASSIGNMENT OPERATOR:
    //   Ensure that we can assign the value of any object of the class to any
    //   object of the class, such that the two objects subsequently have the
    //   same value.
    //
    // Concerns:
    //: 1 The assignment operator can change the value of any modifiable target
    //:   object to that of any source object.
    //:
    //: 2 The allocator address held by the target object is unchanged.
    //:
    //: 3 Any memory allocation is from the target object's allocator.
    //:
    //: 4 The signature and return type are standard.
    //:
    //: 5 The reference returned is to the target object (i.e., '*this').
    //:
    //: 6 The value of the source object is not modified.
    //:
    //: 7 The allocator address held by the source object is unchanged.
    //:
    //: 8 QoI: Assigning a source object having the default-constructed value
    //:   allocates no memory.
    //:
    //: 9 Assigning an object to itself behaves as expected (alias-safety).
    //:
    //:10 Every object releases any allocated memory at destruction.
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   copy-assignment operator defined in this component.  (C-4)
    //:
    //: 2 Create a 'bslma_TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 3 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 4 For each row 'R1' (representing a distinct object value, 'V') in the
    //:   table described in P-3: (C-1..2, 5..8, 10)
    //:
    //:   1 Execute an inner loop that iterates over each row 'R2'
    //:     (representing a distinct object value, 'W') in the table described
    //:     in P-3. For each of the iterations (P-4.2): (C-1..2, 5..8, 10):
    //:
    //:     1 Use the value constructor and a "scratch" allocator to create two
    //:       'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
    //:
    //:     2 Create a 'bslma_TestAllocator' object, 'oa'.
    //:
    //:     3 Use the value constructor and 'oa' to create a modifiable 'Obj',
    //:       'mX', having the value 'W'.
    //:
    //:     4 Assign 'mX' from 'Z'.
    //:
    //:     5 Verify that the address of the return value is the same as that
    //:       of 'mX'.  (C-5)
    //:
    //:     6 Use the equality-comparison operator to verify that: (C-1, 6)
    //:
    //:       1 The target object, 'mX', now has the same value as that of 'Z'.
    //:         (C-1)
    //:
    //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
    //:
    //:     7 Verify that the respective allocator addresses held by 'mX' and
    //:       'Z' are unchanged.  (C-2, 7)
    //:
    //:     8 Use the appropriate test allocators to verify that: (C-8, 10)
    //:
    //:       1 For an object that (a) is initialized with a value that did NOT
    //:         require memory allocation, and (b) is then assigned a value
    //:         that DID require memory allocation, the target object DOES
    //:         allocate memory from its object allocator only (irrespective of
    //:         the specific number of allocations or the total amount of
    //:         memory allocated); also cross check with what is expected for
    //:         'mX' and 'Z'.
    //:
    //:       2 An object that is assigned a value that did NOT require memory
    //:         allocation, does NOT allocate memory from its object allocator;
    //:         also cross check with what is expected for 'Z'.
    //:
    //:       3 No additional memory is allocated by the source object.  (C-8)
    //:
    //:       4 All object memory is released when the object is destroyed.
    //:         (C-10)
    //:
    //: 5 Repeat steps similar to those described in P-4 except that, this
    //:   time, there is no inner loop (as in P-4.2); instead, the source
    //:   object, 'Z', is a reference to the target object, 'mX', and both 'mX'
    //:   and 'ZZ' are initialized to have the value 'V'.  For each row
    //:   (representing a distinct object value, 'V') in the table described in
    //:   P-3: (C-9)
    //:
    //:   1 Create a 'bslma_TestAllocator' object, 'oa'.
    //:
    //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
    //:     'mX'; also use the value constructor and a distinct "scratch"
    //:     allocator to create a 'const' 'Obj' 'ZZ'.
    //:
    //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
    //:
    //:   4 Assign 'mX' from 'Z'.
    //:
    //:   5 Verify that the address of the return value is the same as that of
    //:     'mX'.
    //:
    //:   6 Use the equality-comparison operator to verify that the target
    //:     object, 'mX', still has the same value as that of 'ZZ'.
    //:
    //:   7 Verify 'mX' is still using the object allocator.
    //:
    //:   8 Use the appropriate test allocators to verify that:
    //:
    //:     1 Any memory that is allocated is from the object allocator.
    //:
    //:     2 No additional (e.g., temporary) object memory is allocated when
    //:       assigning an object value that did NOT initially require
    //:       allocated memory.
    //:
    //:     3 All object memory is released when the object is destroyed.
    //:
    //: 6 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-3)
    //
    // Testing:
    //   queue& operator=(const queue& rhs);
    // ------------------------------------------------------------------------


    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf("\nCompare each pair of similar and different"
                        " values (u, ua, v, va) in S X A X S X A"
                        " without perturbation.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char *const SPEC1   = DATA[ti].d_spec;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            // Create second object
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const SPEC2   = DATA[tj].d_spec;

                Obj mZ(&scratch);  const Obj& Z  = gg(&mZ,  SPEC1);
                Obj mZZ(&scratch); const Obj& ZZ = gg(&mZZ, SPEC1);

                if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(&oa);  const Obj& X  = gg(&mX,  SPEC2);

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    Obj *mR = &(mX = Z);
                    ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                    ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);

                    ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

                    ASSERTV(LINE1, LINE2,
                            use_same_allocator(mX, TYPE_ALLOC, &oa));
                    ASSERTV(LINE1, LINE2,
                            use_same_allocator(mZ, TYPE_ALLOC, &scratch));

                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                             0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj  mX(&oa);        const Obj& X  = gg(&mX,  SPEC1);
                Obj  mZZ(&scratch);  const Obj& ZZ = gg(&mZZ,  SPEC1);
                Obj& mZ = mX;        const Obj& Z  = mZ;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                Obj *mR = &(mX = Z);
                ASSERTV(LINE1, ZZ,   Z, ZZ == Z);
                ASSERTV(LINE1, mR,  &X, mR == &X);

                ASSERTV(LINE1, sam.isTotalSame());
                ASSERTV(LINE1, oam.isTotalSame());

                ASSERTV(LINE1, use_same_allocator(mZ, TYPE_ALLOC, &oa));

                ASSERTV(LINE1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }
    }
}

template <class VALUE, class CONTAINER>
template <bool PROPAGATE_ON_CONTAINER_SWAP_FLAG,
          bool OTHER_FLAGS>
void TestDriver<VALUE, CONTAINER>::
                               testCase8_propagate_on_container_swap_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<VALUE,
                                        OTHER_FLAGS,
                                        OTHER_FLAGS,
                                        PROPAGATE_ON_CONTAINER_SWAP_FLAG,
                                        OTHER_FLAGS> StdAlloc;

    typedef bsl::deque<VALUE, StdAlloc>              CObj;
    typedef bsl::queue<VALUE, CObj>                  Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_SWAP_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };
    const int NUM_SPECS = static_cast<int>(sizeof SPECS / sizeof *SPECS);

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char *const ISPEC   = SPECS[ti];

        TestValues IVALUES(ISPEC);

        bslma::TestAllocator xoa("x-original", veryVeryVeryVerbose);
        bslma::TestAllocator yoa("y-original", veryVeryVeryVerbose);

        StdAlloc xma(&xoa);
        StdAlloc yma(&yoa);

        StdAlloc scratch(&da);

        const CObj CI(IVALUES.begin(), IVALUES.end(), scratch);

        const Obj ZZ(CI, scratch);  // control

        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            const CObj CJ(JVALUES.begin(), JVALUES.end(), scratch);

            const Obj WW(CJ, scratch);  // control

            {
                Obj mX(CI, xma);  const Obj& X = mX;

                if (veryVerbose) { T_ P_(ISPEC) P_(X) P(ZZ) }

                Obj mY(CJ, yma);  const Obj& Y = mY;

                ASSERTV(ISPEC, JSPEC, ZZ, X, ZZ == X);
                ASSERTV(ISPEC, JSPEC, WW, Y, WW == Y);

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor dam(&da);
                    bslma::TestAllocatorMonitor xoam(&xoa);
                    bslma::TestAllocatorMonitor yoam(&yoa);

                    mX.swap(mY);

                    ASSERTV(ISPEC, JSPEC, WW, X, WW == X);
                    ASSERTV(ISPEC, JSPEC, ZZ, Y, ZZ == Y);

                    if (PROPAGATE) {
// TBD no 'get_allocator' in 'queue'
#if 0
                        ASSERTV(ISPEC, JSPEC, yma == X.get_allocator());
                        ASSERTV(ISPEC, JSPEC, xma == Y.get_allocator());
#endif

                        ASSERTV(ISPEC, JSPEC, dam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, xoam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, yoam.isTotalSame());
                    }
// TBD no 'get_allocator' in 'queue'
#if 0
                    else {
                        ASSERTV(ISPEC, JSPEC, xma == X.get_allocator());
                        ASSERTV(ISPEC, JSPEC, yma == Y.get_allocator());
                    }
#endif
                }

                // free function 'swap'
                {
                    bslma::TestAllocatorMonitor dam(&da);
                    bslma::TestAllocatorMonitor xoam(&xoa);
                    bslma::TestAllocatorMonitor yoam(&yoa);

                    swap(mX, mY);

                    ASSERTV(ISPEC, JSPEC, ZZ, X, ZZ == X);
                    ASSERTV(ISPEC, JSPEC, WW, Y, WW == Y);

// TBD no 'get_allocator' in 'queue'
#if 0
                    ASSERTV(ISPEC, JSPEC, xma == X.get_allocator());
                    ASSERTV(ISPEC, JSPEC, yma == Y.get_allocator());
#endif

                    if (PROPAGATE) {
                        ASSERTV(ISPEC, JSPEC, dam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, xoam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, yoam.isTotalSame());
                    }
                }
            }
            ASSERTV(ISPEC, 0 == xoa.numBlocksInUse());
            ASSERTV(ISPEC, 0 == yoa.numBlocksInUse());
        }
    }
    ASSERTV(0 == da.numBlocksInUse());
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase8_propagate_on_container_swap()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS: ALLOCATOR PROPAGATION
    //
    // Concerns:
    //: 1 If the 'propagate_on_container_swap' trait is 'false', the allocators
    //:   used by the source and target objects remain unchanged (i.e., the
    //:   allocators are *not* exchanged).
    //:
    //: 2 If the 'propagate_on_container_swap' trait is 'true', the allocator
    //:   used by the target (source) object is updated to be a copy of that
    //:   used by the source (target) object (i.e., the allocators *are*
    //:   exchanged).
    //:
    //: 3 If the allocators are propagated (i.e., exchanged), there is no
    //:   additional allocation from any allocator.
    //:
    //: 4 The effect of the 'propagate_on_container_swap' trait is independent
    //:   of the other three allocator propagation traits.
    //:
    //: 5 Following the swap operation, neither object holds on to memory
    //:   allocated from the other object's allocator.
    //
    // Plan:
    //: 1 Specify a set S of object values with varied differences, ordered by
    //:   increasing length, to be used in the following tests.
    //:
    //: 2 Create two 'bsltf::StdStatefulAllocator' objects with their
    //:   'propagate_on_container_swap' property configured to 'false'.  In two
    //:   successive iterations of P-3, first configure the three properties
    //:   not under test to be 'false', then configure them all to be 'true'.
    //:
    //: 3 For each value '(x, y)' in the cross product S x S:  (C-1)
    //:
    //:   1 Initialize two objects from 'x', a control object 'ZZ' using a
    //:     scratch allocator and an object 'X' using one of the allocators
    //:     from P-2.
    //:
    //:   2 Initialize two objects from 'y', a control object 'WW' using a
    //:     scratch allocator and an object 'Y' using the other allocator from
    //:     P-2.
    //:
    //:   3 Using both member 'swap' and free function 'swap', swap 'X' with
    //:     'Y' and use 'operator==' to verify that 'X' and 'Y' have the
    //:     expected values.
    //:
    //:   4 Use the 'get_allocator' method to verify that the allocators of 'X'
    //:     and 'Y' are *not* exchanged.  (C-1)
    //:
    //: 4 Repeat P-2..3 except that this time configure the allocator property
    //:   under test to 'true' and verify that the allocators of 'X' and 'Y'
    //:   *are* exchanged.  Also verify that there is no additional allocation
    //:   from any allocator.  (C-2..5)
    //
    // Testing:
    //   propagate_on_container_swap
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nSWAP MEMBER AND FREE FUNCTIONS: ALLOCATOR PROPAGATION"
               "\n=====================================================\n");

    if (verbose) printf("\n'propagate_on_container_swap::value == false'\n");

    testCase8_propagate_on_container_swap_dispatch<false, false>();
    testCase8_propagate_on_container_swap_dispatch<false, true>();

    if (verbose) printf("\n'propagate_on_container_swap::value == true'\n");

    testCase8_propagate_on_container_swap_dispatch<true, false>();
    testCase8_propagate_on_container_swap_dispatch<true, true>();
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase8()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS
    //   Ensure that, when member and free 'swap' are implemented, we can
    //   exchange the values of any two objects that use the same allocator.
    //
    // Concerns:
    //: 1 Both functions exchange the values of the (two) supplied objects.
    //:
    //: 2 The common object allocator address held by both objects is
    //:   unchanged.
    //:
    //: 3 If the two objects being swapped use the same allocators, neither
    //:   function allocates memory from any allocator.
    //:
    //: 4 Both functions have standard signatures and return types.
    //:
    //: 5 Two objects with different allocators may be swapped.  In which case,
    //:   memory may be allocated.
    //:
    //: 6 Using either function to swap an object with itself does not affect
    //:   the value of the object (alias-safety).
    //:
    //: 7 The free 'swap' function is discoverable through ADL (Argument
    //:   Dependent Lookup).
    //:
    //: 8 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Use the addresses of the 'swap' member and free functions defined in
    //:   this component to initialize, respectively, member-function and
    //:   free-function pointers having the appropriate signatures and return
    //:   types.  (C-4)
    //:
    //: 2 Create a 'bslma_TestAllocator' object, and install it as the
    //:   default allocator (note that a ubiquitous test allocator is
    //:   already installed as the global allocator).
    //:
    //: 3 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values (one per row) in
    //:     terms of their individual attributes, including (a) first, the
    //:     default value, (b) boundary values corresponding to every range
    //:     of values that each individual attribute can independently
    //:     attain, and (c) values that should require allocation from each
    //:     individual attribute that can independently allocate memory.
    //:
    //: 4 For each row 'R1' in the table of P-3:  (C-1..2, 6)
    //:
    //:   1 Create a 'bslma_TestAllocator' object, 'oa'.
    //:
    //:   2 Use the value constructor and 'oa' to create a modifiable
    //:     'Obj', 'mW', having the value described by 'R1'; also use the
    //:     copy constructor and a "scratch" allocator to create a 'const'
    //:     'Obj' 'XX' from 'mW'.  Use the member and free 'swap' functions
    //:     to swap the value of 'mW' with itself; verify, after each swap,
    //:     that:  (C-6)
    //:
    //:     1 The value is unchanged.  (C-6)
    //:
    //:     2 There was no additional object memory allocation.
    //:
    //:   3 For each row 'R2' in the table of P-3:  (C-1..2)
    //:
    //:     1 Use the value constructor and 'oa' to create a modifiable
    //:       'Obj', 'mW', having the value described by 'R1'; also use the
    //:       copy constructor and a "scratch" allocator to create a 'const'
    //:       'Obj' 'XX' from 'mW'.
    //:
    //:     2 Use the copy constructor and 'oa' to create a modifiable
    //:       'Obj', 'mX', from 'XX'.
    //:
    //:     3 Use, in turn, the member and free 'swap' functions to swap
    //:       the values of 'mX' and 'mY'; verify, after each swap, that:
    //:       (C-1)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 There was no additional object memory allocation.
    //:
    //:       3 The common object allocator address held by 'mX' and 'mY'
    //:         is unchanged in both objects.  Note that a 'queue' object
    //:         does not provide public access to the allocator held.  We test
    //:         this indirectly by calling 'push' to add a value to the object
    //:         and then check the allocator statistics variation.  (C-2)
    //:
    //: 5 Verify that the free 'swap' function is discoverable through ADL:
    //:   (C-7)
    //:
    //:   1 Create a set of attribute values, 'A', distinct from the values
    //:     corresponding to the default-constructed object, choosing
    //:     values that allocate memory if possible.
    //:
    //:   2 Create a 'bslma_TestAllocator' object, 'oa'.
    //:
    //:   3 Use the default constructor and 'oa' to create a modifiable
    //:     'Obj' 'mX' (having default attribute values); also use the copy
    //:     constructor and a "scratch" allocator to create a 'const' 'Obj'
    //:     'XX' from 'mX'.
    //:
    //:   4 Use the value constructor and 'oa' to create a modifiable 'Obj'
    //:     'mY' having the value described by the 'Ai' attributes; also
    //:     use the copy constructor and a "scratch" allocator to create a
    //:     'const' 'Obj' 'YY' from 'mY'.
    //:
    //:   5 Use the 'invokeAdlSwap' helper function template to swap the
    //:     values of 'mX' and 'mY', using the free 'swap' function defined
    //:     in this component, then verify that:  (C-7)
    //:
    //:     1 The values have been exchanged.
    //:
    //:     2 There was no additional object memory allocation.  (C-7)
    //:
    //: 6 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-3)
    //:
    //: 7 Verify that, in appropriate build modes, defensive checks are
    //:   triggered when an attempt is made to swap objects that do not refer
    //:   to the same allocator, but not when the allocators are the same
    //:   (using the 'BSLS_ASSERTTEST_*' macros).  (C-7)
    //
    // Testing:
    //   void swap(queue& other);
    //   void swap(queue<V, C>& a, queue<V, C>& b);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nSWAP MEMBER AND FREE FUNCTIONS"
                        "\n==============================\n");

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    if (verbose) printf(
                     "\nAssign the address of each function to a variable.\n");
    {
        typedef void (Obj::*funcPtr)(Obj&);
        typedef void (*freeFuncPtr)(Obj&, Obj&);

        // Verify that the signatures and return types are standard.

        funcPtr     memberSwap = &Obj::swap;
        freeFuncPtr freeSwap   = bsl::swap;

        (void) memberSwap;  // quash potential compiler warnings
        (void) freeSwap;
    }

    if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf("\nUse a table of distinct object values.\n");

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE1   = DATA[ti].d_line;
        const char *const SPEC1   = DATA[ti].d_spec;

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);


        // Ensure the first row of the table contains the default-constructed
        // value.

        static bool firstFlag = true;
        if (firstFlag) {
            Obj mW(&oa);  const Obj& W = gg(&mW,  SPEC1);

            if (veryVerbose) { T_ P_(LINE1) P_(W) }

            ASSERTV(LINE1, Obj(), W, Obj() == W);
            firstFlag = false;
        }

        // member 'swap'
        {
            Obj mW(&oa);  const Obj& W = gg(&mW,  SPEC1);
            const Obj XX(W, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

            bslma::TestAllocatorMonitor oam(&oa);

            mW.swap(mW);

            ASSERTV(LINE1, XX, W, XX == W);
            ASSERTV(LINE1, oam.isTotalSame());
        }

        // free function 'swap'
        {
            Obj mW(&oa);  const Obj& W = gg(&mW,  SPEC1);
            const Obj XX(W, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

            bslma::TestAllocatorMonitor oam(&oa);

            swap(mW, mW);

            ASSERTV(LINE1, XX, W, XX == W);
            ASSERTV(LINE1, oam.isTotalSame());
        }

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const int         LINE2   = DATA[tj].d_line;
            const char *const SPEC2   = DATA[tj].d_spec;

            // member 'swap'
            {
                Obj mW(&oa);  const Obj& W = gg(&mW,  SPEC1);
                const Obj XX(W, &scratch);

                if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

                Obj mX(XX, &oa);  const Obj& X = mX;

                Obj mY(&oa);  const Obj& Y = gg(&mY, SPEC2);
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                bslma::TestAllocatorMonitor oam(&oa);

                mX.swap(mY);

                ASSERTV(LINE1, LINE2, YY, X, YY == X);
                ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
                ASSERT(use_same_allocator(mX, TYPE_ALLOC, &oa));
                ASSERT(use_same_allocator(mY, TYPE_ALLOC, &oa));
            }

            // free function 'swap'
            {
                Obj mW(&oa);  const Obj& W = gg(&mW,  SPEC1);
                const Obj XX(W, &scratch);

                if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

                Obj mX(XX, &oa);  const Obj& X = mX;

                Obj mY(&oa);  const Obj& Y = gg(&mY, SPEC2);
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                bslma::TestAllocatorMonitor oam(&oa);

                swap(mX, mY);

                ASSERTV(LINE1, LINE2, YY, X, YY == X);
                ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }
        }
    }

    if (verbose) printf(
            "\nInvoke free 'swap' function in a context where ADL is used.\n");
    {
        // 'A' values: Should cause memory allocation if possible.

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;
        const Obj XX(X, &scratch);

        Obj mY(&oa);  const Obj& Y = gg(&mY, "ABC");
        const Obj YY(Y, &scratch);

        if (veryVerbose) { T_ P_(X) P(Y) }

        bslma::TestAllocatorMonitor oam(&oa);

        invokeAdlSwap(mX, mY);

        ASSERTV(YY, X, YY == X);
        ASSERTV(XX, Y, XX == Y);
        ASSERT(oam.isTotalSame());

        if (veryVerbose) { T_ P_(X) P(Y) }
    }
}

template <class VALUE, class CONTAINER>
template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
          bool OTHER_FLAGS>
void TestDriver<VALUE, CONTAINER>::
                     testCase7_select_on_container_copy_construction_dispatch()
{
    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                    VALUE,
                                    SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
                                    OTHER_FLAGS,
                                    OTHER_FLAGS,
                                    OTHER_FLAGS> StdAlloc;

    typedef bsl::deque<VALUE, StdAlloc>          CObj;
    typedef bsl::queue<VALUE, CObj>              Obj;

    const bool PROPAGATE = SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };
    const int NUM_SPECS = static_cast<int>(sizeof SPECS / sizeof *SPECS);

    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char *const SPEC   = SPECS[ti];
        const size_t      LENGTH = strlen(SPEC);

        TestValues VALUES(SPEC);

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        StdAlloc ma(&oa);
        StdAlloc scratch(&da);

        {
            const CObj C(VALUES.begin(), VALUES.end(), scratch);

            const Obj W(C, ma);  // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            Obj mX(C, ma);  const Obj& X = mX;

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            bslma::TestAllocatorMonitor dam(&da);
            bslma::TestAllocatorMonitor oam(&oa);

            const Obj Y(X);

            ASSERTV(SPEC, W == Y);
            ASSERTV(SPEC, W == X);
// TBD no 'get_allocator' in 'queue'
#if 0
            ASSERTV(SPEC, PROPAGATE, PROPAGATE == (ma == Y.get_allocator()));
            ASSERTV(SPEC, PROPAGATE,               ma == X.get_allocator());
#endif

            if (PROPAGATE) {
                ASSERTV(SPEC, 0 != TYPE_ALLOC || dam.isInUseSame());
                ASSERTV(SPEC, 0 == LENGTH || oam.isInUseUp());
            }
            else {
                ASSERTV(SPEC, 0 == LENGTH || dam.isInUseUp());
                ASSERTV(SPEC, oam.isTotalSame());
            }
        }
        ASSERTV(SPEC, 0 == da.numBlocksInUse());
        ASSERTV(SPEC, 0 == oa.numBlocksInUse());
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::
                              testCase7_select_on_container_copy_construction()
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
    //:   three properties not under test to be 'false', then confgiure them
    //:   all to be 'true'.
    //:
    //: 3 For each value in S, initialize objects 'W' (a control) and 'X' using
    //:   the allocator from P-2.
    //:
    //: 4 Copy construct 'Y' from 'X' and use 'operator==' to verify that both
    //:   'X' and 'Y' subsequently have the same value as 'W'.
    //:
    //: 5 Use the 'get_allocator' method to verify that the allocator of 'X' is
    //:   *not* propagated to 'Y'.
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

    testCase7_select_on_container_copy_construction_dispatch<false, false>();
    testCase7_select_on_container_copy_construction_dispatch<false, true>();

    if (verbose) printf("\n'select_on_container_copy_construction' "
                        "propagates allocator of source object.\n");

    testCase7_select_on_container_copy_construction_dispatch<true, false>();
    testCase7_select_on_container_copy_construction_dispatch<true, true>();

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) printf("\nVerify C++03 semantics (allocator has no "
                        "'select_on_container_copy_construction' method).\n");

    typedef StatefulStlAllocator<VALUE>  Allocator;

    typedef bsl::deque<VALUE, Allocator> CObj;
    typedef bsl::queue<VALUE, CObj>      Obj;

    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE",
        };
        const int NUM_SPECS = static_cast<int>(sizeof SPECS / sizeof *SPECS);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);
            TestValues VALUES(SPEC);

            const int ALLOC_ID = ti + 73;

            Allocator a;  a.setId(ALLOC_ID);

            const CObj C(VALUES.begin(), VALUES.end(), a);

            const Obj W(C, a);  // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            const Obj X(C, a);

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            const Obj Y(X);

            ASSERTV(SPEC,        W == Y);
            ASSERTV(SPEC,        W == X);
// TBD no 'get_allocator' in 'queue'
#if 0
            ASSERTV(SPEC, ALLOC_ID == Y.get_allocator().id());
#endif
        }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR:
    //: 1 The new object's value is the same as that of the original object
    //:   (relying on the equality operator).
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 3 The value of the original object is left unaffected.
    //:
    //: 4 Subsequent changes in or destruction of the source object have no
    //:   effect on the copy-constructed object.
    //:
    //: 5 Subsequent changes ('push's) on the created object have no effect on
    //:   the original.
    //:
    //: 6 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //
    // Plan:
    //: 1 Specify a set S of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used in the
    //:   following tests.
    //:
    //: 2 For each value in S, initialize objects w and x, copy construct y
    //:   from x and use 'operator==' to verify that both x and y subsequently
    //:   have the same value as w.  Let x go out of scope and again verify
    //:   that w == y.  (C-1..4)
    //:
    //: 3 For each value in S initialize objects w and x, and copy construct y
    //:   from x.  Change the state of y, by using the *primary* *manipulator*
    //:   'push'.  Using the 'operator!=' verify that y differs from x and w.
    //:   (C-5)
    //:
    //: 4 Perform tests performed as P-2:  (C-6)
    //:   1 While passing a testAllocator as a parameter to the new object and
    //:     ascertaining that the new object gets its memory from the provided
    //:     testAllocator.
    //:   2 Verify neither of global and default allocator is used to supply
    //:     memory.  (C-6)
    //:
    // Testing:
    //   queue(const queue& original);
    //   queue(const queue& original, const A& allocator);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa(veryVeryVerbose);

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);
    {
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;
        const TestValues VALUES;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\nFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            // Create control object w.
            Obj mW; const Obj& W = gg(&mW, SPEC);

            ASSERTV(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            {   // Testing concern 1..4.

                if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                Obj *pX = new Obj(&oa);
                gg(pX, SPEC);

                const Obj Y0(*pX);

                ASSERTV(SPEC, W == Y0);
                ASSERTV(SPEC, W == X);

                delete pX;
                ASSERTV(SPEC, W == Y0);
            }
            {   // Testing concern 5.

                if (veryVerbose) printf("\t\t\tPush into created obj, "
                                        "without test allocator:\n");

                Obj Y1(X);

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Insert: "); P(Y1);
                }

                Y1.push(VALUES[0]);

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Insert : ");
                    P(Y1);
                }

                ASSERTV(SPEC, Y1.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y1);
                ASSERTV(SPEC, X != Y1);
            }
            {   // Testing concern 5 with test allocator.

                if (veryVerbose)
                    printf("\t\t\tPush into created obj, "
                           "with test allocator:\n");

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64  B = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                Obj Y11(X, &oa);

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64  A = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    printf("\t\t\t\tBefore Append: "); P(Y11);
                }

                if (0 == LENGTH) {
                    ASSERTV(SPEC, BB <= AA);
                    ASSERTV(SPEC,  B <=  A);
                }
                else {
                    const int TYPE_ALLOCS = TYPE_ALLOC
                                            * static_cast<int>(X.size());
                    ASSERTV(SPEC, BB + 1 + TYPE_ALLOCS <= AA);
                    ASSERTV(SPEC,  B + 1 + TYPE_ALLOCS <=  A);
                }

                const bsls::Types::Int64 CC = oa.numBlocksTotal();
                const bsls::Types::Int64  C = oa.numBlocksInUse();

                Y11.push(VALUES[0]);

                const bsls::Types::Int64 DD = oa.numBlocksTotal();
                const bsls::Types::Int64  D = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Append : ");
                    P(Y11);
                }

                ASSERTV(SPEC, CC + TYPE_ALLOC <= DD);
                ASSERTV(SPEC,  C + TYPE_ALLOC <=  D);

                ASSERTV(SPEC, Y11.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y11);
                ASSERTV(SPEC, X != Y11);
            }
        }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase6()
{
    // ---------------------------------------------------------------------
    // TESTING EQUALITY OPERATORS:
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
    //: 2 Create a 'bslma_TestAllocator' object, and install it as the default
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
    //:       4 Verify the commutativity property and expected return value for
    //:         both '==' and '!=', while monitoring both 'oax' and 'oay' to
    //:         ensure that no object memory is ever allocated by either
    //:         operator.  (C-1..2, 5..7, 10)
    //:
    //: 5 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-11)
    //
    // Testing:
    //   bool operator==(const queue<V, C>& lhs, const queue<V, C>& rhs);
    //   bool operator!=(const queue<V, C>& lhs, const queue<V, C>& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nEQUALITY-COMPARISON OPERATORS"
                        "\n=============================\n");

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

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\nCompare every value with every value.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char *const SPEC1   = DATA[ti].d_spec;
            const size_t      LENGTH1 = strlen(DATA[ti].d_spec);

           if (veryVerbose) { T_ P_(LINE1) P_(LENGTH1) P(SPEC1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&scratch); const Obj& X = gg(&mX, SPEC1);

                ASSERTV(LINE1, X,   X == X);
                ASSERTV(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const SPEC2   = DATA[tj].d_spec;
                const size_t      LENGTH2 = strlen(DATA[tj].d_spec);

                if (veryVerbose) { T_ T_ P_(LINE2) P_(LENGTH2) P(SPEC2) }

                const bool EXP = !strcmp(SPEC1, SPEC2);  // expected result

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create two distinct test allocators, 'oax' and 'oay'.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    Obj mX(&xa); const Obj& X = gg(&mX, SPEC1);
                    Obj mY(&ya); const Obj& Y = gg(&mY, SPEC2);

                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH1 == X.size());
                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH2 == Y.size());

                    if (veryVerbose) { T_ T_ P_(X) P(Y); }

                    // Verify value, commutativity, and no memory allocation.

                    bslma::TestAllocatorMonitor oaxm(&xa);
                    bslma::TestAllocatorMonitor oaym(&ya);

                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (Y != X));

                    ASSERTV(LINE1, LINE2, CONFIG, oaxm.isTotalSame());
                    ASSERTV(LINE1, LINE2, CONFIG, oaym.isTotalSame());
                }
            }
        }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    //   Ensure each basic accessor:
    //     - front
    //     - back
    //     - size
    //   properly interprets object state.
    //
    // Concerns:
    //: 1 Each accessor returns the value of the correct property of the
    //:   object.
    //:
    //: 2 Each accessor method is declared 'const'.
    //:
    //: 3 No accessor allocates any memory.
    //:
    // Plan:
    //: 1 For each set of 'SPEC' of different length:
    //:
    //:   1 Default construct the object with various configuration:
    //:
    //:     1 Use the 'gg' function to populate the object based on the SPEC.
    //:
    //:     2 Verify the object contains the expected number of elements.
    //:       (C-1)
    //:
    //:     3 Use 'front' and 'back' to verify the values are as expected.
    //:       (C-1..2)
    //:
    //:     4 Monitor the memory allocated from both the default and object
    //:       allocators before and after calling the accessor; verify that
    //:       there is no change in total memory allocation.  (C-3)
    //
    // Testing:
    //   const_reference front() const;
    //   const_reference back()  const;
    //   size_type       size()  const;
    // ------------------------------------------------------------------------

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) { printf(
                "\nCreate objects with various allocator configurations.\n"); }
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const TestValues  EXP(DATA[ti].d_spec);
            const size_t      LENGTH = strlen(DATA[ti].d_spec);

            if (verbose) { P_(LINE) P_(LENGTH) P(SPEC); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa1("supplied1",  veryVeryVeryVerbose);
                bslma::TestAllocator sa2("supplied2",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                 *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj();
                      objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(static_cast<bslma::Allocator*>(0));
                      objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(&sa1);
                      objAllocatorPtr = &sa1;
                  } break;
                  case 'd': {
                      objPtr = new (fa) Obj(&sa2);
                      objAllocatorPtr = &sa2;
                  } break;
                  default: {
                      ASSERTV(CONFIG, !"Bad allocator config.");
                      return;                                         // RETURN
                  } break;
                }

                Obj& mX = *objPtr;  const Obj& X = gg(&mX, SPEC);
                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = ('c' == CONFIG || 'd' == CONFIG)
                                         ? da
                                         : sa1;

                // --------------------------------------------------------

                // Verify basic accessor

                bslma::TestAllocatorMonitor oam(&oa);

                ASSERTV(LINE, SPEC, CONFIG, LENGTH == X.size());
                if (LENGTH) {
                    ASSERTV(LINE, SPEC, CONFIG,          EXP[0] == X.front());
                    ASSERTV(LINE, SPEC, CONFIG, EXP[LENGTH - 1] == X.back ());
                }
                ASSERT(oam.isTotalSame());

                // --------------------------------------------------------

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify no allocation from the non-object allocator.

                ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());

                // Verify all memory is released on object destruction.

                ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa1.numBlocksInUse(),
                        0 == sa1.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa2.numBlocksInUse(),
                        0 == sa2.numBlocksInUse());
            }
        }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase3()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMITIVE GENERATOR FUNCTIONS gg AND ggg:
    //   Having demonstrated that our primary manipulators work as expected
    //   under normal conditions
    //
    // Concerns:
    //: 1 Valid generator syntax produces expected results
    //:
    //: 2 Invalid syntax is detected and reported.
    //
    // Plan:
    //: 1 For each of an enumerated sequence of 'spec' values, ordered by
    //:   increasing 'spec' length:
    //:
    //:   1 Use the primitive generator function 'gg' to set the state of a
    //:     newly created object.
    //:
    //:   2 Verify that 'gg' returns a valid reference to the modified argument
    //:     object.
    //:
    //:   3 Use the basic accessors to verify that the value of the object is
    //:     as expected.  (C-1)
    //:
    //: 2 For each of an enumerated sequence of 'spec' values, ordered by
    //:   increasing 'spec' length, use the primitive generator function 'ggg'
    //:   to set the state of a newly created object.
    //:
    //:   1 Verify that 'ggg' returns the expected value corresponding to the
    //:     location of the first invalid value of the 'spec'.  (C-2)
    //
    // Testing:
    //   queue<V,C>& gg(queue<V,C> *object, const char* spec);
    //   int ggg(queue<V,C> *object, const char *spec, int verbose = 1);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa(veryVeryVerbose);

    if (verbose) printf("\nTesting generator on valid specs.\n");
    {
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_spec);
            const TestValues  EXP(DATA[ti].d_spec);
            const int         curLen = (int)strlen(SPEC);

            Obj mX(&oa);
            const Obj& X = gg(&mX, SPEC);   // original spec

            Obj mY(&oa);
            const Obj& Y = gg(&mY, SPEC);    // extended spec

            if (curLen != oldLen) {
                if (verbose) printf("\tof length %d:\n", curLen);
                ASSERTV(LINE, oldLen <= curLen);  // non-decreasing
                oldLen = curLen;
            }

            if (veryVerbose) {
                printf("\t\tSpec = \"%s\"\n", SPEC);
            }

            ASSERTV(LINE, LENGTH == X.size());
            ASSERTV(LINE, LENGTH == Y.size());
            ASSERTV(0 == verify_object(mX, EXP, LENGTH));
            ASSERTV(0 == verify_object(mY, EXP, LENGTH));
        }
    }

    if (verbose) printf("\nTesting generator on invalid specs.\n");
    {
        static const struct {
            int         d_line;    // source line number
            const char *d_spec;    // specification string
            int         d_return;  // expected return value from 'gg'
        } DATA[] = {
            //line  spec      expected return
            //----  --------  ---------------
            { L_,   "",       -1,             },

            { L_,   "A",      -1,             },
            { L_,   " ",       0,             },
            { L_,   ".",       0,             },
            { L_,   "E",       -1,            },
            { L_,   "a",       0,             },
            { L_,   "z",       0,             },

            { L_,   "AE",     -1,             },
            { L_,   "aE",      0,             },
            { L_,   "Ae",      1,             },
            { L_,   ".~",      0,             },
            { L_,   "~!",      0,             },
            { L_,   "  ",      0,             },

            { L_,   "ABC",    -1,             },
            { L_,   " BC",     0,             },
            { L_,   "A C",     1,             },
            { L_,   "AB ",     2,             },
            { L_,   "?#:",     0,             },
            { L_,   "   ",     0,             },

            { L_,   "ABCDE",  -1,             },
            { L_,   "aBCDE",   0,             },
            { L_,   "ABcDE",   2,             },
            { L_,   "ABCDe",   4,             },
            { L_,   "AbCdE",   1,             }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         EXPR   = DATA[ti].d_return;
            const int         LENGTH = static_cast<int>(strlen(SPEC));

            Obj mX(&oa);

            if (LENGTH != oldLen) {
                if (verbose) printf("\tof length %d:\n", LENGTH);
                ASSERTV(LINE, oldLen <= LENGTH);  // non-decreasing
                oldLen = LENGTH;
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int RESULT = ggg(&mX, SPEC, veryVerbose);

            ASSERTV(LINE, EXPR == RESULT);
        }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase2()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
    //   The basic concern is that the default constructor, the destructor,
    //   and, under normal conditions (i.e., no aliasing), the primary
    //   manipulators
    //      - push
    //      - pop
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
    //: 4 Supplying a null allocator address has the same effect as not
    //:   supplying an allocator.
    //:
    //: 5 Supplying an allocator to the default constructor has no effect on
    //:   subsequent object values.
    //:
    //: 6 Any memory allocation is from the object allocator.
    //:
    //: 7 Every object releases any allocated memory at destruction.
    //:
    //: 8 QoI: The default constructor allocates no memory.
    //:
    //: 9 'push' adds an additional element to the end of the queue.
    //:
    //:10 'pop' removes the element at front of the queue if there is any.
    //
    // Plan:
    //:   1 For each value of increasing length, 'L':
    //:
    //:   2 Using a loop-based approach, default-construct three distinct
    //:     objects, in turn, but configured differently: (a) without passing
    //:     an allocator, (b) passing a null allocator address explicitly,
    //:     and (c) passing the address of a test allocator distinct from the
    //:     default.  For each of these three iterations:  (C-1..10)
    //:
    //:     1 Create three 'bslma_TestAllocator' objects, and install one as as
    //:       the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     2 Use the default constructor to dynamically create an object
    //:       'X', with its object allocator configured appropriately; use a
    //:       distinct test allocator for the object's footprint.
    //:
    //:     3 Use the appropriate test allocators to verify that no memory is
    //:       allocated by the default constructor.  (C-8)
    //:
    //:     4 Use the individual (as yet unproven) salient attribute accessors
    //:       to verify the default-constructed value.  (C-1)
    //:
    //:     5 Push 'L' elements in order of increasing value into the container
    //:       and use the (as yet unproven) basic accessors to verify the
    //:       container has the expected values.  Verify the number of
    //:       allocation is as expected. (C-5..6)
    //:
    //:     6 Invoke 'pop' and verify that the container has one fewer element
    //:       (C-10).
    //:
    //:     7 Verify that all object memory is released when the object is
    //:       destroyed.  (C-7)
    //
    // Testing:
    //   queue()
    //   queue(const A& allocator);
    //   ~queue();
    //   void push(const value_type& value);
    //   void pop();
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    if (verbose) { P(TYPE_ALLOC); }

    const TestValues VALUES;  // contains 52 distinct increasing values

    const size_t MAX_LENGTH = 9;

    for (size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const size_t LENGTH = ti;

        if (verbose) {
            printf("\nTesting with various allocator configurations.\n");
        }
        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting default constructor.\n");
            }

            Obj                 *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                  objPtr = new (fa) Obj();
                  objAllocatorPtr = &da;
              } break;
              case 'b': {
                  objPtr = new (fa) Obj(static_cast<bslma::Allocator*>(0));
                  objAllocatorPtr = &da;
              } break;
              case 'c': {
                  objPtr = new (fa) Obj(&sa);
                  objAllocatorPtr = &sa;
              } break;
              default: {
                  ASSERTV(CONFIG, !"Bad allocator config.");
                  return;                                             // RETURN
              } break;
            }

            Obj&                  mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify no allocation from the non-object allocator.

            ASSERTV(LENGTH, CONFIG, noa.numBlocksTotal(),
                    0 ==  noa.numBlocksTotal());
            ASSERTV(LENGTH, CONFIG, 0 == X.size());

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\n\tTesting 'push' (bootstrap).\n"); }

            if (veryVeryVerbose) {
                printf("\t\tOn an object of initial length " ZU ".\n", LENGTH);
            }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma::TestAllocatorMonitor tam(&oa);

                mX.push(VALUES[tj]);
                ASSERTV(LENGTH, CONFIG, tj + 1 == X.size());

                ASSERTV(X.front() == VALUES[0]);
                ASSERTV(X.back () == VALUES[tj]);

                if (0 < TYPE_ALLOC) {
                    //ASSERTV(CONFIG, tam.isTotalUp());
                    ASSERTV(CONFIG, tam.isInUseUp());
                }
                else {
                    ASSERTV(CONFIG, tam.isTotalSame());
                    ASSERTV(CONFIG, tam.isInUseSame());
                }
            }

            // ----------------------------------------------------------------

            if (veryVerbose) printf("\n\tTesting 'pop'.\n");

            if (verbose) {
                printf("\t\tOn an object of initial length " ZU ".\n",
                       LENGTH + 1);
            }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                mX.pop();

                ASSERTV(LENGTH, CONFIG, LENGTH - 1 - tj == X.size());
                if (LENGTH - 1 == tj) {
                    ASSERTV(true == X.empty());
                }
                else {
                    ASSERTV(X.front() == VALUES[tj + 1]);
                    ASSERTV(X.back () == VALUES[LENGTH - 1]);
                }

                const bsls::Types::Int64 A  = oa.numBlocksInUse();
                ASSERTV(LENGTH, CONFIG, B, A, B - TYPE_ALLOC == A);
            }

            // ----------------------------------------------------------------

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(LENGTH, CONFIG, da.numBlocksInUse(),
                    0 == da.numBlocksInUse());
            ASSERTV(LENGTH, CONFIG, fa.numBlocksInUse(),
                    0 == fa.numBlocksInUse());
            ASSERTV(LENGTH, CONFIG, sa.numBlocksInUse(),
                    0 == sa.numBlocksInUse());
        }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase1_NoAlloc(const VALUE  *testValues,
                                                     size_t        numValues)
{
    // --------------------------------------------------------------------
    // BREATHING TEST:
    //   This case exercises (but does not fully test) basic functionality.
    //
    // Concerns:
    //: 1 The class is sufficiently functional to enable comprehensive
    //:   testing in subsequent test cases.
    //
    // Plan:
    //   Create four objects using both the default and copy constructors.
    //   Exercise these objects using primary manipulators [1, 5], basic
    //   accessors, equality operators, copy constructors [2, 8], and the
    //   assignment operator [9, 10].  Try aliasing with assignment for a
    //   non-empty object [11].
    //
    //: 1  Create an object x1 (default ctor).       { x1: }
    //:
    //: 2  Create a second object x2 (copy from x1). { x1: x2: }
    //:
    //: 3  Append an element of value A to x1).      { x1:A x2: }
    //:
    //: 4  Append the same value A to x2).           { x1:A x2:A }
    //:
    //: 5  Append another element of value B to x2). { x1:A x2:AB }
    //:
    //: 6  Remove element of value A  from x1.       { x1: x2:AB }
    //:
    //: 7  Create a third object x3 (default ctor).  { x1: x2:AB x3: }
    //:
    //: 8  Create a forth object x4 (copy of x2).    { x1: x2:AB x3: x4:AB }
    //:
    //: 9  Assign x2 = x1 (non-empty becomes empty). { x1: x2: x3: x4:AB }
    //:
    //: 10 Assign x3 = x4 (empty becomes non-empty).{ x1: x2: x3:AB x4:AB }
    //:
    //: 11 Assign x4 = x4 (aliasing).               { x1: x2: x3:AB x4:AB }
    //
    // Testing:
    //   This "test" *exercises* basic functionality.
    // --------------------------------------------------------------------

    ASSERT(testValues);
    ASSERT(1 < numValues);  // Need at least two test elements

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 1) Create an object x1 (default ctor)."
                            "\t\t\t{ x1: }\n");
    Obj mX1;  const Obj& X1 = mX1;

    ASSERT(   0 == X1.size());
    ASSERT(true == X1.empty());

    ASSERT(  X1  == X1 );        ASSERT(!(X1  != X1));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 2) Create a second object x2 (copy from x1)."
                            "\t\t{ x1: x2: }\n");
    Obj mX2(X1);  const Obj& X2 = mX2;
    ASSERT(   0 == X2.size ());
    ASSERT(true == X1.empty());

    ASSERT(X2 == X1 );           ASSERT(!(X2 != X1));
    ASSERT(X2 == X2 );           ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 3) Push an element of value A to x1)."
                                "\t\t\t{ x1:A x2: }\n");

    mX1.push(testValues[0]);

    ASSERT(            1 == X1.size ());
    ASSERT(        false == X1.empty());
    ASSERT(testValues[0] == X1.front());
    ASSERT(testValues[0] == X1.back ());

    ASSERT(  X1 == X1 );         ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));         ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 4) Append the same value A to x2)."
                            "\t\t\t{ x1:A x2:A }\n");

    mX2.push(testValues[0]);

    ASSERT(            1 == X2.size ());
    ASSERT(        false == X2.empty());
    ASSERT(testValues[0] == X2.front());
    ASSERT(testValues[0] == X2.back ());

    ASSERT(  X2 == X1 );         ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );         ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 5) Append another element of value B to x2)."
                            "\t\t{ x1:A x2:AB }\n");

    mX2.push(testValues[1]);

    ASSERT(            2 == X2.size ());
    ASSERT(testValues[0] == X2.front());
    ASSERT(testValues[1] == X2.back ());

    ASSERT(!(X2 == X1));         ASSERT(  X2 != X1 );
    ASSERT(  X2 == X2 );         ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 6) Remove element of value A from x1."
                            "\t\t\t{ x1: x2:AB }\n");
    mX1.pop();

    ASSERT(0 == X1.size());

    ASSERT(  X1 == X1 );         ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));         ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 7) Create a third object x3 (default ctor)."
                            "\t\t{ x1: x2:AB x3: }\n");

    Obj mX3;  const Obj& X3 = mX3;

    ASSERT(   0 == X3.size ());
    ASSERT(true == X3.empty());

    ASSERT(  X3 == X1 );         ASSERT(!(X3 != X1));
    ASSERT(!(X3 == X2));         ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );         ASSERT(!(X3 != X3));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 8) Create a forth object x4 (copy of x2)."
                            "\t\t{ x1: x2:AB x3: x4:AB }\n");

    Obj mX4(X2);  const Obj& X4 = mX4;


    ASSERT(            2 == X4.size ());
    ASSERT(        false == X4.empty());
    ASSERT(testValues[0] == X4.front());
    ASSERT(testValues[1] == X4.back ());

    ASSERT(!(X4 == X1));         ASSERT(  X4 != X1 );
    ASSERT(  X4 == X2 );         ASSERT(!(X4 != X2));
    ASSERT(!(X4 == X3));         ASSERT(  X4 != X3 );
    ASSERT(  X4 == X4 );         ASSERT(!(X4 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 9) Assign x2 = x1 (non-empty becomes empty)."
                            "\t\t{ x1: x2: x3: x4:AB }\n");

    mX2 = X1;

    ASSERT(   0 == X2.size ());
    ASSERT(true == X2.empty());

    ASSERT(  X2 == X1 );         ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );         ASSERT(!(X2 != X2));
    ASSERT(  X2 == X3 );         ASSERT(!(X2 != X3));
    ASSERT(!(X2 == X4));         ASSERT(  X2 != X4 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n10) Assign x3 = x4 (empty becomes non-empty)."
                         "\t\t{ x1: x2: x3:AB x4:AB }\n");

    mX3 = X4;

    ASSERT(            2 == X3.size ());
    ASSERT(        false == X3.empty());
    ASSERT(testValues[0] == X3.front());
    ASSERT(testValues[1] == X3.back ());

    ASSERT(!(X3 == X1));         ASSERT(  X3 != X1 );
    ASSERT(!(X3 == X2));         ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );         ASSERT(!(X3 != X3));
    ASSERT(  X3 == X4 );         ASSERT(!(X3 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n11) Assign x4 = x4 (aliasing)."
                            "\t\t\t\t{ x1: x2: x3:AB x4:AB }\n");

    mX4 = X4;

    ASSERT(            2 == X4.size ());
    ASSERT(        false == X4.empty());
    ASSERT(testValues[0] == X4.front());
    ASSERT(testValues[1] == X4.back ());

    ASSERT(!(X4 == X1));          ASSERT(  X4 != X1 );
    ASSERT(!(X4 == X2));          ASSERT(  X4 != X2 );
    ASSERT(  X4 == X3 );          ASSERT(!(X4 != X3));
    ASSERT(  X4 == X4 );          ASSERT(!(X4 != X4));
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase1(const VALUE  *testValues,
                                             size_t        numValues)
{
    // --------------------------------------------------------------------
    // BREATHING TEST:
    //   This case exercises (but does not fully test) basic functionality.
    //
    // Concerns:
    //: 1 The class is sufficiently functional to enable comprehensive
    //:   testing in subsequent test cases.
    //
    // Plan:
    //   Create four objects using both the default and copy constructors.
    //   Exercise these objects using primary manipulators [1, 5], basic
    //   accessors, equality operators, copy constructors [2, 8], and the
    //   assignment operator [9, 10].  Try aliasing with assignment for a
    //   non-empty object [11].
    //
    //: 1  Create an object x1 (default ctor).       { x1: }
    //:
    //: 2  Create a second object x2 (copy from x1). { x1: x2: }
    //:
    //: 3  Append an element of value A to x1).      { x1:A x2: }
    //:
    //: 4  Append the same value A to x2).           { x1:A x2:A }
    //:
    //: 5  Append another element of value B to x2). { x1:A x2:AB }
    //:
    //: 6  Remove element of value A  from x1.       { x1: x2:AB }
    //:
    //: 7  Create a third object x3 (default ctor).  { x1: x2:AB x3: }
    //:
    //: 8  Create a forth object x4 (copy of x2).    { x1: x2:AB x3: x4:AB }
    //:
    //: 9  Assign x2 = x1 (non-empty becomes empty). { x1: x2: x3: x4:AB }
    //:
    //: 10 Assign x3 = x4 (empty becomes non-empty).{ x1: x2: x3:AB x4:AB }
    //:
    //: 11 Assign x4 = x4 (aliasing).               { x1: x2: x3:AB x4:AB }
    //
    // Testing:
    //   This "test" *exercises* basic functionality.
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    ASSERT(testValues);
    ASSERT(1 < numValues);  // Need at least two test elements

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 1) Create an object x1 (default ctor)."
                            "\t\t\t{ x1: }\n");
    Obj mX1(&testAllocator);  const Obj& X1 = mX1;

    ASSERT(   0 == X1.size());
    ASSERT(true == X1.empty());

    ASSERT(  X1  == X1 );        ASSERT(!(X1  != X1));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 2) Create a second object x2 (copy from x1)."
                            "\t\t{ x1: x2: }\n");
    Obj mX2(X1, &testAllocator);  const Obj& X2 = mX2;
    ASSERT(   0 == X2.size ());
    ASSERT(true == X1.empty());

    ASSERT(X2 == X1 );           ASSERT(!(X2 != X1));
    ASSERT(X2 == X2 );           ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 3) Push an element of value A to x1)."
                                "\t\t\t{ x1:A x2: }\n");

    mX1.push(testValues[0]);

    ASSERT(            1 == X1.size ());
    ASSERT(        false == X1.empty());
    ASSERT(testValues[0] == X1.front());
    ASSERT(testValues[0] == X1.back ());

    ASSERT(  X1 == X1 );         ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));         ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 4) Append the same value A to x2)."
                            "\t\t\t{ x1:A x2:A }\n");

    mX2.push(testValues[0]);

    ASSERT(            1 == X2.size ());
    ASSERT(        false == X2.empty());
    ASSERT(testValues[0] == X2.front());
    ASSERT(testValues[0] == X2.back ());

    ASSERT(  X2 == X1 );         ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );         ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 5) Append another element of value B to x2)."
                            "\t\t{ x1:A x2:AB }\n");

    mX2.push(testValues[1]);

    ASSERT(            2 == X2.size ());
    ASSERT(testValues[0] == X2.front());
    ASSERT(testValues[1] == X2.back ());

    ASSERT(!(X2 == X1));         ASSERT(  X2 != X1 );
    ASSERT(  X2 == X2 );         ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 6) Remove element of value A from x1."
                            "\t\t\t{ x1: x2:AB }\n");
    mX1.pop();

    ASSERT(0 == X1.size());

    ASSERT(  X1 == X1 );         ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));         ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 7) Create a third object x3 (default ctor)."
                            "\t\t{ x1: x2:AB x3: }\n");

    Obj mX3(&testAllocator);  const Obj& X3 = mX3;

    ASSERT(   0 == X3.size ());
    ASSERT(true == X3.empty());

    ASSERT(  X3 == X1 );         ASSERT(!(X3 != X1));
    ASSERT(!(X3 == X2));         ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );         ASSERT(!(X3 != X3));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 8) Create a forth object x4 (copy of x2)."
                            "\t\t{ x1: x2:AB x3: x4:AB }\n");

    Obj mX4(X2, &testAllocator);  const Obj& X4 = mX4;


    ASSERT(            2 == X4.size ());
    ASSERT(        false == X4.empty());
    ASSERT(testValues[0] == X4.front());
    ASSERT(testValues[1] == X4.back ());

    ASSERT(!(X4 == X1));         ASSERT(  X4 != X1 );
    ASSERT(  X4 == X2 );         ASSERT(!(X4 != X2));
    ASSERT(!(X4 == X3));         ASSERT(  X4 != X3 );
    ASSERT(  X4 == X4 );         ASSERT(!(X4 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 9) Assign x2 = x1 (non-empty becomes empty)."
                            "\t\t{ x1: x2: x3: x4:AB }\n");

    mX2 = X1;

    ASSERT(   0 == X2.size ());
    ASSERT(true == X2.empty());

    ASSERT(  X2 == X1 );         ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );         ASSERT(!(X2 != X2));
    ASSERT(  X2 == X3 );         ASSERT(!(X2 != X3));
    ASSERT(!(X2 == X4));         ASSERT(  X2 != X4 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n10) Assign x3 = x4 (empty becomes non-empty)."
                         "\t\t{ x1: x2: x3:AB x4:AB }\n");

    mX3 = X4;

    ASSERT(            2 == X3.size ());
    ASSERT(        false == X3.empty());
    ASSERT(testValues[0] == X3.front());
    ASSERT(testValues[1] == X3.back ());

    ASSERT(!(X3 == X1));         ASSERT(  X3 != X1 );
    ASSERT(!(X3 == X2));         ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );         ASSERT(!(X3 != X3));
    ASSERT(  X3 == X4 );         ASSERT(!(X3 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n11) Assign x4 = x4 (aliasing)."
                            "\t\t\t\t{ x1: x2: x3:AB x4:AB }\n");

    mX4 = X4;

    ASSERT(            2 == X4.size ());
    ASSERT(        false == X4.empty());
    ASSERT(testValues[0] == X4.front());
    ASSERT(testValues[1] == X4.back ());

    ASSERT(!(X4 == X1));          ASSERT(  X4 != X1 );
    ASSERT(!(X4 == X2));          ASSERT(  X4 != X2 );
    ASSERT(  X4 == X3 );          ASSERT(!(X4 != X3));
    ASSERT(  X4 == X4 );          ASSERT(!(X4 != X4));
}


// ============================================================================
//                                  USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace UsageExample {

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Messages Queue
///- - - - - - - - - - - - -
// In this example, we will use the 'bsl::queue' container adapter to implement
// a message processor in a server program that receives and displays messages
// from clients.
//
// Suppose we want to write a server program that has two threads: one thread
// (receiving thread) receives messages from clients, passing them to a message
// processor; the other thread (processing thread) runs the message processor,
// printing the messages to console in the same order as they were received.
// To accomplish this task, we can use 'bsl::queue' in the message processor to
// buffer received, but as yet unprinted messages.  The message processor
// pushes newly received messages onto the queue in the receiving thread, and
// pops them off the queue in the processing thread.
//
// First, we define a 'Message' type:
//..
struct Message {
    int         d_msgId;  // message identifier given by client
    const char *d_msg_p;  // message content (null terminated byte string,
                          // not owned)
};
//..
// Then, we define the class 'MessageProcessor', which provides methods to
// receive and process messages:
//..
class MessageProcessor {
    // This class receives and processes messages from clients.
//..
// Here, we define a private data member of 'bsl::queue<Message>' type, which
// is an instantiation of 'bsl::queue' that uses 'Message' for its 'VALUE'
// (template parameter) type and (by default) 'bsl::deque<Message>' for its
// 'CONTAINER' (template parameter) type:
//..
    // DATA
    bsl::queue<Message> d_msgQueue;  // queue holding received but
                                     // unprocessed messages
    // ...

  public:
    // CREATORS
    explicit MessageProcessor(bslma::Allocator *basicAllocator = 0);
        // Create a message processor object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    // MANIPULATORS
    void receiveMessage(const Message &message);
        // Enqueue the specified 'message' for processing.

    void processMessages(int verbose);
        // Dequeue all messages currently contained by this processor, and
        // print them to the console if the specified 'verbose' flag
        // is not 0.
};
//..
// Next, we implement the 'MessageProcessor' constructor:
//..
MessageProcessor::MessageProcessor(bslma::Allocator *basicAllocator)
: d_msgQueue(basicAllocator)
{
}
//..
// Notice that we pass to the contained 'd_msgQueue' object the
// 'bslma::Allocator' supplied to the 'MessageProcessor' at construction.
//
// Now, we implement the 'receiveMessage' method, which pushes the given
// message onto the queue object:
//..
void MessageProcessor::receiveMessage(const Message &message)
{
    // ... (some synchronization)

    d_msgQueue.push(message);

    // ...
}
//..
// Finally, we implement the 'processMessages' method, which pops all messages
// off the queue object:
//..
void MessageProcessor::processMessages(int verbose)
{
    // ... (some synchronization)

    while (!d_msgQueue.empty()) {
        const Message& message = d_msgQueue.front();
        if (verbose) {
            printf("Msg %d: %s\n", message.d_msgId, message.d_msg_p);
        }
        d_msgQueue.pop();
    }

    // ...
}
//..
// Notice that the sequence of messages popped out will be in exactly the same
// order in which they are pushed, due to the first-in-first-out property of
// the queue.

}  // close namespace UsageExample

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
struct TestDeductionGuides {

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

    // This struct provides a namespace for functions testing deduction guides.
    // The tests are compile-time only; it is not necessary that these routines
    // be called at run-time.  Note that the following constructors do not have
    // associated deduction guides because the template parameters for
    // 'bsl::queue' cannot be deduced from the constructor parameters.
    //..
    // queue()
    // queue(ALLOC)
    //..

    void SimpleConstructors ()
        // Test that constructing a 'bsl::queue' from various combinations of
        // arguments deduces the correct type.
        //..
        // queue(const queue&  q)        -> decltype(q)
        // queue(const queue&  q, ALLOC) -> decltype(q)
        // queue(      queue&& q)        -> decltype(q)
        // queue(      queue&& q, ALLOC) -> decltype(q)
        // queue(const CONTAINER &) -> queue<CONTAINER, CONTAINER::vlue_type)
        // queue(const CONTAINER &,  ALLOC)
        //                            -> queue<CONTAINER, CONTAINER::vlue_type)
        // queue(      CONTAINER &&)  -> queue<CONTAINER, CONTAINER::vlue_type)
        // queue(      CONTAINER &&, ALLOC)
        //                            -> queue<CONTAINER, CONTAINER::vlue_type)
    {
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;

        typedef int T1;
        bsl::queue<T1> q1;
        bsl::queue     q1a(q1);
        ASSERT_SAME_TYPE(decltype(q1a), bsl::queue<T1>);

        typedef float T2;
        bsl::queue<T2> q2;
        bsl::queue     q2a(q2, bsl::allocator<T2>());
        bsl::queue     q2b(q2, a1);
        bsl::queue     q2c(q2, a2);
        bsl::queue     q2d(q2, bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(q2a), bsl::queue<T2>);
        ASSERT_SAME_TYPE(decltype(q2b), bsl::queue<T2>);
        ASSERT_SAME_TYPE(decltype(q2c), bsl::queue<T2>);
        ASSERT_SAME_TYPE(decltype(q2d), bsl::queue<T2>);

        typedef short T3;
        bsl::queue<T3> q3;
        bsl::queue     q3a(std::move(q3));
        ASSERT_SAME_TYPE(decltype(q3a), bsl::queue<T3>);

        typedef long double T4;
        bsl::queue<T4> q4;
        bsl::queue     q4a(std::move(q4), bsl::allocator<T4>{});
        bsl::queue     q4b(std::move(q4), a1);
        bsl::queue     q4c(std::move(q4), a2);
        bsl::queue     q4d(std::move(q4), bsl::allocator<int>{});
        ASSERT_SAME_TYPE(decltype(q4a), bsl::queue<T4>);
        ASSERT_SAME_TYPE(decltype(q4b), bsl::queue<T4>);
        ASSERT_SAME_TYPE(decltype(q4c), bsl::queue<T4>);
        ASSERT_SAME_TYPE(decltype(q4d), bsl::queue<T4>);

        typedef long T5;
        bsl::vector<T5>       v5;
        NonAllocContainer<T5> nc5;
        bsl::queue            q5a(v5);
        bsl::queue            q5b(nc5);
        ASSERT_SAME_TYPE(decltype(q5a), bsl::queue<T5, bsl::vector<T5>>);
        ASSERT_SAME_TYPE(decltype(q5b), bsl::queue<T5, NonAllocContainer<T5>>);

        typedef double T6;
        bsl::vector<T6> v6;
        bsl::queue      q6a(v6, bsl::allocator<T6>());
        bsl::queue      q6b(v6, a1);
        bsl::queue      q6c(v6, a2);
        bsl::queue      q6d(v6, bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(q6a), bsl::queue<T6, bsl::vector<T6>>);
        ASSERT_SAME_TYPE(decltype(q6b), bsl::queue<T6, bsl::vector<T6>>);
        ASSERT_SAME_TYPE(decltype(q6c), bsl::queue<T6, bsl::vector<T6>>);
        ASSERT_SAME_TYPE(decltype(q6d), bsl::queue<T6, bsl::vector<T6>>);

        typedef long long T7;
        bsl::vector<T7>       v7;
        NonAllocContainer<T7> nc7;
        bsl::queue            q7a(std::move(v7));
        bsl::queue            q7b(std::move(nc7));
        ASSERT_SAME_TYPE(decltype(q7a), bsl::queue<T7, bsl::vector<T7>>);
        ASSERT_SAME_TYPE(decltype(q7b), bsl::queue<T7, NonAllocContainer<T7>>);

        typedef double T8;
        bsl::vector<T8> v8;
        bsl::queue      q8a(std::move(v8), bsl::allocator<T8>());
        bsl::queue      q8b(std::move(v8), a1);
        bsl::queue      q8c(std::move(v8), a2);
        bsl::queue      q8d(std::move(v8), bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(q6a), bsl::queue<T8, bsl::vector<T8>>);
        ASSERT_SAME_TYPE(decltype(q8b), bsl::queue<T8, bsl::vector<T8>>);
        ASSERT_SAME_TYPE(decltype(q8c), bsl::queue<T8, bsl::vector<T8>>);
        ASSERT_SAME_TYPE(decltype(q8d), bsl::queue<T8, bsl::vector<T8>>);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Compile-fail tests
// #define BSLSTL_QUEUE_COMPILE_FAIL_ALLOCATOR_IS_NOT_A_CONTAINER
#ifdef  BSLSTL_QUEUE_COMPILE_FAIL_ALLOCATOR_IS_NOT_A_CONTAINER
        bsl::queue q98(bsl::allocator<char>{});
        // This should fail to compile (Allocator is not a container)
#endif

// #define BSLSTL_QUEUE_COMPILE_FAIL_NON_ALLOCATOR_AWARE_CONTAINER
#ifdef  BSLSTL_QUEUE_COMPILE_FAIL_NON_ALLOCATOR_AWARE_CONTAINER
        typedef unsigned short T99;
        NonAllocContainer<T99> nc99;
        bsl::queue             q99a(nc99, bsl::allocator<T99>{});
        bsl::queue             q99b(std::move(nc99), bsl::allocator<T99>{});
        // These should fail to compile (can't supply an allocator to a
        // non-allocator-aware container.)
#endif
    }

#undef ASSERT_SAME_TYPE
};
#endif  // BSLS_COMPILERFEATURES_SUPPORT_CTAD

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

bool intLessThan(int a, int b)
{
    return a < b;
}

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);
    bslma::TestAllocator ta(veryVeryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 21: {
        //---------------------------------------------------------------------
        // TESTING CLASS TEMPLATE DEDUCTION GUIDES (AT COMPILE TIME)
        //   Ensure that the deduction guides are properly specified to deduce
        //   the template arguments from the arguments supplied to the
        //   constructors.
        //
        // Concerns:
        //: 1 Construction from iterators deduces the value type from the value
        //:   type of the iterator.
        //
        //: 2 Construction with a 'bslma::Allocator *' deduces the correct
        //:   specialization of 'bsl::allocator' for the type of the allocator.
        //
        // Plan:
        //: 1 Create a list by invoking the constructor without supplying the
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
      case 20: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "'noexcept' SPECIFICATION" "\n"
                                 "------------------------" "\n");

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
        ASSERT(!bsl::is_nothrow_swappable<bsl::vector<int> >::value);
        TestDriver<int, bsl::vector<int> >::testCase20();

#ifndef BSLMF_ISNOTHROWSWAPPABLE_ALWAYS_FALSE
        ASSERT(bsl::is_nothrow_swappable<NothrowSwapDeque<int> >::value);
        TestDriver<int, NothrowSwapDeque<int> >::testCase20();
#endif
#endif

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // MOVE MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "MOVE MANIPULATORS" "\n"
                                 "-----------------" "\n");

        typedef signed char                                SC;
        typedef size_t                                     SZ;
        typedef bsltf::TemplateTestFacility::ObjectPtr TTF_OP;
     // typedef bsltf::TemplateTestFacility::MethodPtr TTF_MP;
        typedef bsltf::EnumeratedTestType::Enum           ETT;
        typedef bsltf::SimpleTestType                     STT;
        typedef bsltf::AllocTestType                      ATT;
        typedef bsltf::BitwiseMoveableTestType           BMTT;
        typedef bsltf::AllocBitwiseMoveableTestType     ABMTT;
        typedef bsltf::NonTypicalOverloadsTestType      NTOTT;

        TestDriver<   int,    MovableVector<   int> >::testCase19(true );
        TestDriver<   int, NonMovableVector<   int> >::testCase19(false);

        TestDriver<    SC,    MovableVector<    SC> >::testCase19(true );
        TestDriver<    SC, NonMovableVector<    SC> >::testCase19(false);

        TestDriver<    SZ,    MovableVector<    SZ> >::testCase19(true );
        TestDriver<    SZ, NonMovableVector<    SZ> >::testCase19(false);

        TestDriver<TTF_OP,    MovableVector<TTF_OP> >::testCase19(true );
        TestDriver<TTF_OP, NonMovableVector<TTF_OP> >::testCase19(false);

     // TestDriver<TTF_MP,    MovableVector<TTF_MP> >::testCase19(true );
     // TestDriver<TTF_MP, NonMovableVector<TTF_MP> >::testCase19(false);

        TestDriver<   ETT,    MovableVector<   ETT> >::testCase19(true );
        TestDriver<   ETT, NonMovableVector<   ETT> >::testCase19(false);

        TestDriver<   STT,    MovableVector<   STT> >::testCase19(true );
        TestDriver<   STT, NonMovableVector<   STT> >::testCase19(false);

        TestDriver<   ATT,    MovableVector<   ATT> >::testCase19(true );
        TestDriver<   ATT, NonMovableVector<   ATT> >::testCase19(false);

        TestDriver<  BMTT,    MovableVector<  BMTT> >::testCase19(true );
        TestDriver<  BMTT, NonMovableVector<  BMTT> >::testCase19(false);

        TestDriver< ABMTT,    MovableVector< ABMTT> >::testCase19(true );
        TestDriver< ABMTT, NonMovableVector< ABMTT> >::testCase19(false);

        TestDriver< NTOTT,    MovableVector< NTOTT> >::testCase19(true );
        TestDriver< NTOTT, NonMovableVector< NTOTT> >::testCase19(false);

#ifndef BSLS_PLATFORM_OS_WINDOWS
        typedef bsltf::TemplateTestFacility::ObjectPtr TTF_FP;

        TestDriver<TTF_FP,    MovableVector<TTF_FP> >::testCase19(true );
        TestDriver<TTF_FP, NonMovableVector<TTF_FP> >::testCase19(false);
#endif

        if (verbose) printf("\n" "Move Only Type" "\n"
                                 "--------------" "\n");

#ifndef BSLS_PLATFORM_CMP_IBM
        // move-only tests on AIX actually do compile and pass, but they
        // produce a vast number of warnings because of the poor handling of
        // temporaries in xlC.
        typedef bsltf::MoveOnlyAllocTestType            MOATT;
        TestDriver<MOATT,   MovableVector<MOATT> >::testCase19MoveOnlyType();

        typedef bsltf::WellBehavedMoveOnlyAllocTestType  WBMOATT;
        TestDriver<WBMOATT, MovableVector<WBMOATT> >::testCase19MoveOnlyType();
#endif

        // 'propagate_on_container_move_assignment' testing

// TBD enable this
#if 0
        RUN_EACH_TYPE(TestDriver,
                      testCase19_propagate_on_container_move_assignment,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase19_propagate_on_container_move_assignment,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);
#endif

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTORS
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "MOVE CONSTRUCTORS" "\n"
                                 "-----------------" "\n");
        typedef signed char                                SC;
        typedef size_t                                     SZ;
        typedef bsltf::TemplateTestFacility::ObjectPtr TTF_OP;
        typedef bsltf::TemplateTestFacility::MethodPtr TTF_MP;
        typedef bsltf::EnumeratedTestType::Enum           ETT;
        typedef bsltf::SimpleTestType                     STT;
        typedef bsltf::AllocTestType                      ATT;
        typedef bsltf::BitwiseMoveableTestType           BMTT;
        typedef bsltf::AllocBitwiseMoveableTestType     ABMTT;
        typedef bsltf::NonTypicalOverloadsTestType      NTOTT;

        TestDriver<   int,    MovableVector<   int> >::testCase18(true );
        TestDriver<   int, NonMovableVector<   int> >::testCase18(false);

        TestDriver<    SC,    MovableVector<    SC> >::testCase18(true );
        TestDriver<    SC, NonMovableVector<    SC> >::testCase18(false);

        TestDriver<    SZ,    MovableVector<    SZ> >::testCase18(true );
        TestDriver<    SZ, NonMovableVector<    SZ> >::testCase18(false);

        TestDriver<TTF_OP,    MovableVector<TTF_OP> >::testCase18(true );
        TestDriver<TTF_OP, NonMovableVector<TTF_OP> >::testCase18(false);

        TestDriver<TTF_MP,    MovableVector<TTF_MP> >::testCase18(true );
        TestDriver<TTF_MP, NonMovableVector<TTF_MP> >::testCase18(false);

        TestDriver<   ETT,    MovableVector<   ETT> >::testCase18(true );
        TestDriver<   ETT, NonMovableVector<   ETT> >::testCase18(false);

        TestDriver<   STT,    MovableVector<   STT> >::testCase18(true );
        TestDriver<   STT, NonMovableVector<   STT> >::testCase18(false);

        TestDriver<   ATT,    MovableVector<   ATT> >::testCase18(true );
        TestDriver<   ATT, NonMovableVector<   ATT> >::testCase18(false);

        TestDriver<  BMTT,    MovableVector<  BMTT> >::testCase18(true );
        TestDriver<  BMTT, NonMovableVector<  BMTT> >::testCase18(false);

        TestDriver< ABMTT,    MovableVector< ABMTT> >::testCase18(true );
        TestDriver< ABMTT, NonMovableVector< ABMTT> >::testCase18(false);

        TestDriver< NTOTT,    MovableVector< NTOTT> >::testCase18(true );
        TestDriver< NTOTT, NonMovableVector< NTOTT> >::testCase18(false);

#ifndef BSLS_PLATFORM_OS_WINDOWS
        typedef bsltf::TemplateTestFacility::ObjectPtr TTF_FP;

        TestDriver<TTF_FP,    MovableVector<TTF_FP> >::testCase18(true );
        TestDriver<TTF_FP, NonMovableVector<TTF_FP> >::testCase18(false);
#endif

        if (verbose) printf("\n" "Move Only Type" "\n"
                                 "--------------" "\n");

        typedef bsltf::MoveOnlyAllocTestType             MOATT;

        TestDriver<MOATT,   MovableVector<MOATT>   >::testCase18MoveOnlyType();

        typedef bsltf::WellBehavedMoveOnlyAllocTestType  WBMOATT;

        TestDriver<WBMOATT, MovableVector<WBMOATT> >::testCase18MoveOnlyType();
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Usage Example"
                            "\n=====================\n");

        using namespace UsageExample;

        const int TOTAL_MSGS = 20;
        const int PROCESS_TIME[] = {1, 4, 9, 13, 19,};
        const int numProcess = sizeof(PROCESS_TIME) / sizeof(PROCESS_TIME[0]);

        int              k = 0;
        char             buffer[256];
        Message          msg;
        MessageProcessor msgProcessor(&ta);

        for (int i = 0;i < TOTAL_MSGS; ++i) {
            snprintf(buffer, sizeof(buffer), "This is message %d", i);
            msg.d_msgId = i;
            msg.d_msg_p = buffer;
            msgProcessor.receiveMessage(msg);

            if (k < numProcess && i == PROCESS_TIME[k]) {
                msgProcessor.processMessages(veryVerbose);
                ++k;
            }
            msgProcessor.processMessages(veryVerbose);
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING NON ALLOCATOR SUPPORTING TYPE
        // --------------------------------------------------------------------

        typedef queue<int, NonAllocContainer<int> > NonAllocQueue;

        NonAllocQueue mX;    const NonAllocQueue& X = mX;

        ASSERT(X.empty());

        mX.push(3);
        mX.push(4);
        mX.push(5);

        ASSERT(! X.empty());
        ASSERT(3 == X.size());
        ASSERT(3 == X.front());
        ASSERT(5 == X.back());

        NonAllocQueue mY(X);   const NonAllocQueue& Y = mY;

        ASSERT(X == Y);         ASSERT(!(X != Y));
        ASSERT(X <= Y);         ASSERT(!(X >  Y));
        ASSERT(X >= Y);         ASSERT(!(X <  Y));

        mY.pop();
        mY.push(6);

        ASSERT(X != Y);         ASSERT(!(X == Y));
        ASSERT(X <  Y);         ASSERT(!(X >= Y));
        ASSERT(X <= Y);         ASSERT(!(X >  Y));
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING FREE COMPARISON OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Free Comparison Operators"
                            "\n=================================\n");

        RUN_EACH_TYPE(TestDriver, testCase15, char, int);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // OTHER ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Other Accessors"
                            "\n=======================\n");

        RUN_EACH_TYPE(TestDriver, testCase14, TEST_TYPES_REGULAR);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // OTHER MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Other Manipulators"
                            "\n==========================\n");

        RUN_EACH_TYPE(TestDriver, testCase13, TEST_TYPES_REGULAR);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Value Constructor"
                            "\n=========================\n");

        RUN_EACH_TYPE(TestDriver, testCase12, TEST_TYPES_REGULAR);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Type Traits\n"
                            "\n===================\n");

        // Verify the 'UsesBslmaAllocator' trait is not defined for non-'bslma'
        // allocators.

        typedef bsltf::StdTestAllocator<bsltf::AllocTestType> StlAlloc;

        typedef bsltf::AllocTestType ATT;

        typedef deque< ATT, StlAlloc> WeirdAllocDeque;
        typedef vector<ATT, StlAlloc> WeirdAllocVector;

        typedef bsl::queue<ATT, WeirdAllocDeque >        WeirdAllocDequeQueue;
        typedef bsl::queue<ATT, WeirdAllocVector>        WeirdAllocVectorQueue;
        typedef bsl::queue<int, NonAllocContainer<int> > NonAllocQueue;

        if (verbose) printf("NonAllocContainer ---------------------------\n");
        BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<
                                             NonAllocContainer<int> >::value));
        BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<
                                             NonAllocQueue>::value));
        TestDriver<NonAllocContainer<int> >::testCase11();

        if (verbose) printf("deque ---------------------------------------\n");
        BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<
                                                WeirdAllocDeque>::value));
        BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<
                                                WeirdAllocDequeQueue>::value));
        RUN_EACH_TYPE(TestDriver, testCase11, TEST_TYPES_REGULAR);

        if (verbose) printf("vector --------------------------------------\n");
        BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<
                                               WeirdAllocVector>::value));
        BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<
                                               WeirdAllocVectorQueue>::value));
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // STREAMING FUNCTIONALITY
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Streaming Functionality"
                            "\n===============================\n");

        if (verbose) printf("There is no streaming for this component.\n");

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Assignment Operator"
                            "\n===========================\n");

        RUN_EACH_TYPE(TestDriver,  testCase9, TEST_TYPES_REGULAR);

        // 'propagate_on_container_copy_assignment' testing

// TBD enable this
#if 0
        RUN_EACH_TYPE(TestDriver,
                      testCase9_propagate_on_container_copy_assignment,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase9_propagate_on_container_copy_assignment,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);
#endif
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // MANIPULATOR AND FREE FUNCTION 'swap'
        // --------------------------------------------------------------------

        if (verbose) printf("\nMANIPULATOR AND FREE FUNCTION 'swap'"
                            "\n====================================\n");

        RUN_EACH_TYPE(TestDriver,  testCase8, TEST_TYPES_REGULAR);

        // 'propagate_on_container_swap' testing

// TBD enable this
#if 0
        RUN_EACH_TYPE(TestDriver,
                      testCase8_propagate_on_container_swap,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase8_propagate_on_container_swap,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Copy Constructors"
                            "\n=========================\n");

        RUN_EACH_TYPE(TestDriver,  testCase7, TEST_TYPES_REGULAR);

        // 'select_on_container_copy_construction' testing

        if (verbose) printf("\nCOPY CONSTRUCTOR: ALLOCATOR PROPAGATION"
                            "\n=======================================\n");

// TBD enable this
#if 0
        RUN_EACH_TYPE(TestDriver,
                      testCase7_select_on_container_copy_construction,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase7_select_on_container_copy_construction,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Equality Operators"
                            "\n==========================\n");

        RUN_EACH_TYPE(TestDriver, testCase6, TEST_TYPES_REGULAR);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Output (<<) Operator"
                            "\n============================\n");

        if (verbose)
            printf("There is no output operator for this component.\n");

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Basic Accessors"
                            "\n=======================\n");

        RUN_EACH_TYPE(TestDriver,  testCase4, TEST_TYPES_REGULAR);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTIONS 'gg' and 'ggg'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'gg'"
                            "\n============\n");

        RUN_EACH_TYPE(TestDriver,  testCase3, TEST_TYPES_REGULAR);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Primary Manipulators"
                            "\n============================\n");

        RUN_EACH_TYPE(TestDriver,  testCase2, TEST_TYPES_REGULAR);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");
        if (verbose) printf("deque<int>:\n");
        TestDriver<int, deque<int> >::testCase1(SPECIAL_INT_VALUES,
                                                NUM_SPECIAL_INT_VALUES);

        if (verbose) printf("list:\n");
        TestDriver<int, list<int> >::testCase1(SPECIAL_INT_VALUES,
                                               NUM_SPECIAL_INT_VALUES);

        if (verbose) printf("NonAllocContainer<int>:\n");
        TestDriver<int, NonAllocContainer<int> >::testCase1_NoAlloc(
                                                       SPECIAL_INT_VALUES,
                                                       NUM_SPECIAL_INT_VALUES);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.
    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

#endif // End C++11 code

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
