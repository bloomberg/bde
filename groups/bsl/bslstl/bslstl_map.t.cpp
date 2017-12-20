// bslstl_map.t.cpp                                                   -*-C++-*-
#include <bslstl_map.h>

#include <bslalg_rangecompare.h>
#include <bslalg_scalarprimitives.h>

#include <bslma_allocator.h>
#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructionutil.h>
#include <bslma_destructorguard.h>
#include <bslma_destructorproctor.h>
#include <bslma_mallocfreeallocator.h>
#include <bslma_stdallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_haspointersemantics.h>
#include <bslmf_integralconstant.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>
#include <bslmf_removeconst.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsls_util.h>

#include <bslstl_vector.h>            // for testing only

#include <bsltf_allocargumenttype.h>
#include <bsltf_allocemplacabletesttype.h>
#include <bsltf_argumenttype.h>
#include <bsltf_emplacabletesttype.h>
#include <bsltf_movablealloctesttype.h>
#include <bsltf_movabletesttype.h>
#include <bsltf_moveonlyalloctesttype.h>
#include <bsltf_movestate.h>
#include <bsltf_nondefaultconstructibletesttype.h>
#include <bsltf_nonoptionalalloctesttype.h>
#include <bsltf_nontypicaloverloadstesttype.h>
#include <bsltf_stdallocatoradaptor.h>
#include <bsltf_stdstatefulallocator.h>
#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <utility>

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#include <initializer_list>
#endif

#include <ctype.h>   // 'isalpha', 'tolower', 'toupper'
#include <limits.h>  // 'INT_MIN', 'INT_MAX'
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND) \
 && (defined(BSLS_PLATFORM_CMP_IBM)   \
  || defined(BSLS_PLATFORM_CMP_CLANG) \
  || defined(BSLS_PLATFORM_CMP_MSVC)  \
  ||(defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION == 0x5130) \
     )
# define BSL_DO_NOT_TEST_MOVE_FORWARDING 1
// Some compilers produce ambiguities when trying to construct our test types
// for 'emplace'-type functionality with the C++03 move-emulation.  This is a
// compiler bug triggering in lower level components, so we simply disable
// those aspects of testing, and rely on the extensive test coverage on other
// platforms.
#endif

#if !defined(BSLS_COMPILER_FEATURES_SUPPORT_RVALUE_REFERENCES) \
 &&  defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION == 0x5130
#   define BSL_COMPILER_THINKS_MOVE_AMBIGUOUS_WITH_COPY 1
#endif

#if defined(BSL_COMPILER_THINKS_MOVE_AMBIGUOUS_WITH_COPY)
# define BAD_MOVE_GUARD(IDENTIFIER) int
#else
# define BAD_MOVE_GUARD(IDENTIFIER) IDENTIFIER
#endif

#if defined(BDE_BUILD_TARGET_EXC)
// The following enum is set to '1' when exceptions are enabled and to '0'
// otherwise.  It is here to avoid having preprocessor macros throughout.
enum { PLAT_EXC = 1 };
#else
enum { PLAT_EXC = 0 };
#endif


// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The object under test is a container whose interface and contract is
// dictated by the C++ standard.  The general concerns are compliance,
// exception safety, and proper dispatching (for member function templates such
// as insert).  This container is implemented in the form of a class template,
// and thus its proper instantiation for several types is a concern.  Regarding
// the allocator template argument, we use mostly a 'bsl::allocator' together
// with a 'bslma::TestAllocator' mechanism, but we also verify the C++
// standard.
//
// Primary Manipulators:
//: o 'insert(value_type&&)'  (via helper function 'primaryManipulator')
//: o 'clear'
//
// Basic Accessors:
//: o 'cbegin'
//: o 'cend'
//: o 'size'
//: o 'get_allocator'
//
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'insert(value_type&&)'[*] and 'clear' methods, where the former is used by
// the generator function 'ggg'.  Note that some manipulators must support
// aliasing, and those that perform memory allocation must be tested for
// exception neutrality via the 'bslma_testallocator' component.  After the
// mandatory sequence of cases (1-10) for value-semantic types (cases 5 and 10
// are not implemented as there is no output or BDEX streaming below 'bslstl'),
// we test each individual constructor, manipulator, and accessor in subsequent
// cases.
//
// [*] 'insert(value_type&&)' was chosen as our primary manipulator rather than
// 'emplace' with a single parameter since: 1) 'insert' is more primitive than
// 'emplace' as the latter requires that a key object be constructed before
// searching the tree, and 2) move-only objects cannot be emplaced.
// ----------------------------------------------------------------------------
// 23.4.6.2, construct/copy/destroy:
// [ 2] map(const C& comparator, const A& allocator);
// [ 2] map(const A& allocator);
// [ 7] map(const map& original);
// [27] map(map&& original);
// [ 7] map(const map& original, const A& allocator);
// [27] map(map&&, const A& allocator);
// [12] map(ITER first, ITER last, const C& comparator, const A& allocator);
// [12] map(ITER first, ITER last, const A& allocator);
// [33] map(initializer_list<value_type>, const C& comp, const A& allocator);
// [33] map(initializer_list<value_type>, const A& allocator);
// [ 2] ~map();
// [ 9] map& operator=(const map& rhs);
// [28] map& operator=(map&& rhs);
// [33] map& operator=(initializer_list<value_type>);
// [ 4] allocator_type get_allocator() const;
//
// iterators:
// [14] iterator begin();
// [14] iterator end();
// [14] reverse_iterator rbegin();
// [14] reverse_iterator rend();
// [14] const_iterator begin() const;
// [14] const_iterator end() const;
// [14] const_reverse_iterator rbegin() const;
// [14] const_reverse_iterator rend() const;
// [ 4] const_iterator cbegin() const;
// [ 4] const_iterator cend() const;
// [14] const_reverse_iterator crbegin() const;
// [14] const_reverse_iterator crend() const;
//
// capacity:
// [20] bool empty() const;
// [ 4] size_type size() const;
// [20] size_type max_size() const;
//
// element access:
// [24] VALUE& operator[](const key_type& key);
// [34] VALUE& operator[](key_type&& key);
// [24] VALUE& at(const key_type& key);
// [24] const VALUE& at(const key_type& key) const;
//
// modifiers:
// [15] pair<iterator, bool> insert(const value_type& value);
// [29] pair<iterator, bool> insert(value_type&& value);
// [29] pair<iterator, bool> insert(ALT_VALUE_TYPE&& value);
// [16] iterator insert(const_iterator position, const value_type& value);
// [30] iterator insert(const_iterator position, value_type&& value);
// [30] iterator insert(const_iterator position, ALT_VALUE_TYPE&& value);
// [17] void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
// [33] void insert(initializer_list<value_type>);
//
// [31] iterator emplace(Args&&... args);
// [32] iterator emplace_hint(const_iterator position, Args&&... args);
//
// [18] iterator erase(const_iterator position);
// [18] size_type erase(const key_type& key);
// [18] iterator erase(const_iterator first, const_iterator last);
// [ 8] void swap(map& other);
// [ 2] void clear();
//
// comparators:
// [21] key_compare key_comp() const;
// [21] value_compare value_comp() const;
//
// map operations:
// [13] iterator find(const key_type& key);
// [13] iterator lower_bound(const key_type& key);
// [13] iterator upper_bound(const key_type& key);
// [13] pair<iterator, iterator> equal_range(const key_type& key);
//
// [13] const_iterator find(const key_type& key) const;
// [13] size_type count(const key_type& key) const;
// [13] const_iterator lower_bound(const key_type& key) const;
// [13] const_iterator upper_bound(const key_type& key) const;
// [13] pair<const_iter, const_iter> equal_range(const key_type&) const;
//
// [ 6] bool operator==(const map& lhs, const map& rhs);
// [ 6] bool operator!=(const map& lhs, const map& rhs);
// [19] bool operator< (const map& lhs, const map& rhs);
// [19] bool operator> (const map& lhs, const map& rhs);
// [19] bool operator>=(const map& lhs, const map& rhs);
// [19] bool operator<=(const map& lhs, const map& rhs);
//
//// specialized algorithms:
// [ 8] void swap(map& a, map& b);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [38] USAGE EXAMPLE
//
// TEST APPARATUS
// [ 3] int ggg(map *object, const char *spec, bool verbose = true);
// [ 3] map& gg(map *object, const char *spec);
// [ 5] 'debugprint' functions (TBD not yet tested)
//
// [22] CONCERN: 'map' is compatible with standard allocators.
// [23] CONCERN: 'map' has the necessary type traits.
// [26] CONCERN: The type provides the full interface defined by the standard.
// [35] CONCERN: 'map' supports incomplete types.
// [TBD] CONCERN: 'map' object size is commensurate with that of 'C' and 'A'.
// [36] CONCERN: Methods qualifed 'noexcept' in standard are so implemented.
// [37] CONCERN: 'bslmf::MovableRef<T>' does not escape (in C++03 mode).

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

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

// ============================================================================
//                             SWAP TEST HELPERS
// ----------------------------------------------------------------------------

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

// The following 'using' directives must come *after* the definition of
// 'invokeAdlSwap' and 'invokePatternSwap' (above).

using namespace BloombergLP;
using bsl::pair;
using bsl::map;
using bsls::NameOf;

// ============================================================================
//                          GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsls::Types::Int64 Int64;

// Define DEFAULT DATA used in multiple test cases.

struct DefaultDataRow {
    int         d_line;       // source line number
    int         d_index;      // lexical order
    const char *d_spec_p;     // specification string, for input to 'gg'
    const char *d_results_p;  // expected element values
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line idx  spec                 results
    //---- ---  ------------------   -------------------
    { L_,    0, "",                  ""                   },

    { L_,    1, "A",                 "A"                  },
    { L_,    1, "AA",                "A"                  },
    { L_,    1, "Aa",                "A"                  },
    { L_,    1, "AAA",               "A"                  },

    { L_,    2, "AB",                "AB"                 },
    { L_,    2, "BA",                "AB"                 },

    { L_,    3, "ABC",               "ABC"                },
    { L_,    3, "ACB",               "ABC"                },
    { L_,    3, "BAC",               "ABC"                },
    { L_,    3, "BCA",               "ABC"                },
    { L_,    3, "CAB",               "ABC"                },
    { L_,    3, "CBA",               "ABC"                },
    { L_,    3, "ABCA",              "ABC"                },
    { L_,    3, "ABCB",              "ABC"                },
    { L_,    3, "ABCC",              "ABC"                },
    { L_,    3, "ABAC",              "ABC"                },
    { L_,    3, "ABCABC",            "ABC"                },
    { L_,    3, "AABBCC",            "ABC"                },

    { L_,    4, "ABCD",              "ABCD"               },
    { L_,    4, "ACBD",              "ABCD"               },
    { L_,    4, "BDCA",              "ABCD"               },
    { L_,    4, "DCBA",              "ABCD"               },

    { L_,    5, "ABCDE",             "ABCDE"              },
    { L_,    5, "ACBDE",             "ABCDE"              },
    { L_,    5, "CEBDA",             "ABCDE"              },
    { L_,    5, "EDCBA",             "ABCDE"              },

    { L_,    6, "FEDCBA",            "ABCDEF"             },

    { L_,    7, "ABCDEFG",           "ABCDEFG"            },

    { L_,    8, "ABCDEFGH",          "ABCDEFGH"           },

    { L_,    9, "ABCDEFGHI",         "ABCDEFGHI"          },

    { L_,   10, "ABCDEFGHIJKLMNOP",  "ABCDEFGHIJKLMNOP"   },
    { L_,   10, "PONMLKJIGHFEDCBA",  "ABCDEFGHIJKLMNOP"   },

    { L_,   11, "ABCDEFGHIJKLMNOPQ", "ABCDEFGHIJKLMNOPQ"  },
    { L_,   11, "DHBIMACOPELGFKNJQ", "ABCDEFGHIJKLMNOPQ"  },

    { L_,   12, "BAD",               "ABD"                },

    { L_,   13, "BEAD",              "ABDE"               },

    { L_,   14, "AC",                "AC"                 },
    { L_,   14, "ACc",               "AC"                 },

    { L_,   15, "Ac",                "Ac"                 },
    { L_,   15, "AcC",               "Ac"                 },

    { L_,   16, "a",                 "a"                  },
    { L_,   16, "aA",                "a"                  },

    { L_,   17, "ac",                "ac"                 },
    { L_,   17, "ca",                "ac"                 },

    { L_,   18, "B",                 "B"                  },

    { L_,   19, "BCDE",              "BCDE"               },

    { L_,   20, "FEDCB",             "BCDEF"              },

    { L_,   21, "CD",                "CD"                 }
};
enum { DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA };

// Define values used to initialize positional arguments for
// 'bsltf::EmplacableTestType' and 'bsltf::AllocEmplacableTestType'
// constructors.  Note, that you cannot change those values as they are used by
// 'TemplateTestFacility::getIdentifier' to map the constructed emplacable
// objects to their integer identifiers.
static const int K01 = 1;
static const int K02 = 20;
static const int K03 = 23;
static const int V01 = 44;
static const int V02 = 68;
static const int V03 = 912;

// TBD There is a fundamental flaw when testing operations involving two maps,
// such as operator== and operator<, that the 'DEFAULT_DATA' table does not
// produce maps that have the same keys, but different values.  It is possible
// that we are not comparing 'value' (as opposed to 'key') in the tests and we
// would never know.  This is a pretty serious omission.  In fact, it extends
// to 'ggg', 'primaryManipulator', 'createInplace', etc.

typedef bsltf::NonDefaultConstructibleTestType TestKeyType;
typedef bsltf::NonTypicalOverloadsTestType     TestValueType;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental type-specific print functions.

namespace bsl {

template <class FIRST, class SECOND>
inline
void debugprint(const pair<FIRST, SECOND>& p)
{
    bsls::BslTestUtil::callDebugprint(p.first);
    bsls::BslTestUtil::callDebugprint(p.second);
}

// map-specific print function.
template <class KEY, class VALUE, class COMP, class ALLOC>
void debugprint(const bsl::map<KEY, VALUE, COMP, ALLOC>& s)
{
    if (s.empty()) {
        printf("<empty>");
    }
    else {
        typedef typename bsl::map<KEY, VALUE, COMP, ALLOC>::const_iterator
                                                                         CIter;
        putchar('"');
        for (CIter it = s.begin(); it != s.end(); ++it) {
            putchar(static_cast<char>(
                       bsltf::TemplateTestFacility::getIdentifier(it->first)));
        }
        putchar('"');
    }
    fflush(stdout);
}

}  // close namespace bsl

bool expectToAllocate(size_t n)
    // Return 'true' if the container is expected to allocate memory on the
    // specified 'n'th element, and 'false' otherwise.
{
    if (n > 32) {
        return 0 == n % 32;                                           // RETURN
    }
    return 0 == ((n - 1) & n);  // Allocate when 'n' is a power of 2.
}

template <class CONTAINER, class VALUES>
int verifyContainer(const CONTAINER& container,
                    const VALUES&    expectedValues,
                    size_t           expectedSize)
    // Verify the specified 'container' has the specified 'expectedSize' and
    // contains the same values as the array in the specified 'expectedValues'.
    // Return 0 if 'container' has the expected values, and a non-zero value
    // otherwise.
{
    ASSERTV(expectedSize, container.size(), expectedSize == container.size());

    if (container.size() != expectedSize) {
        return -1;                                                    // RETURN
    }

    typename CONTAINER::const_iterator it = container.cbegin();
    for (size_t i = 0; i < expectedSize; ++i) {
        ASSERTV(it != container.cend());
        ASSERTV(i, expectedValues[i], *it, expectedValues[i] == *it);

        if (bsltf::TemplateTestFacility::getIdentifier(expectedValues[i].first)
            != bsltf::TemplateTestFacility::getIdentifier(it->first)) {
            return static_cast<int>(i + 1);                           // RETURN
        }
        ++it;
    }
    return 0;
}

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

                            // ======================
                            // class ExceptionProctor
                            // ======================

template <class OBJECT>
struct ExceptionProctor {
    // This class provides a mechanism to verify the strong exception guarantee
    // in exception-throwing code.  On construction, this class stores a copy
    // of an object of the (template parameter) type 'OBJECT' and the address
    // of that object.  On destruction, if 'release' was not invoked, it will
    // verify the value of the object is the same as the value of the copy
    // created on construction.  This class requires that the copy constructor
    // and 'operator ==' be tested before use.

    // DATA
    int           d_line;      // line number at construction
    OBJECT        d_control;   // copy of the object being proctored
    const OBJECT *d_object_p;  // address of the original object

  private:
    // NOT IMPLEMENTED
    ExceptionProctor(const ExceptionProctor&);
    ExceptionProctor& operator=(const ExceptionProctor&);

  public:
    // CREATORS
    ExceptionProctor(const OBJECT     *object,
                     int               line,
                     bslma::Allocator *basicAllocator = 0)
    : d_line(line)
    , d_control(*object, basicAllocator)
    , d_object_p(object)
        // Create an exception proctor for the specified 'object' at the
        // specified 'line' number.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.
    {
    }

    ExceptionProctor(const OBJECT              *object,
                     int                        line,
                     bslmf::MovableRef<OBJECT>  control)
    : d_line(line)
    , d_control(bslmf::MovableRefUtil::move(control))
    , d_object_p(object)
        // Create an exception proctor for the specified 'object' at the
        // specified 'line' number using the specified 'control' object.
    {
    }

    ~ExceptionProctor()
        // Destroy this exception proctor.  If the proctor was not released,
        // verify that the state of the object supplied at construction has not
        // changed.
    {
        if (d_object_p) {
            const int LINE = d_line;
            ASSERTV(LINE, d_control, *d_object_p, d_control == *d_object_p);
        }
    }

    // MANIPULATORS
    void release()
        // Release this proctor from verifying the state of the object
        // supplied at construction.
    {
        d_object_p = 0;
    }
};

namespace {

bslma::TestAllocator *scratchSingleton()
{
    static bslma::TestAllocator scratch("scratch singleton",
                                        veryVeryVeryVerbose);

    return &scratch;
}

bool g_enableLessThanFunctorFlag = true;

                       // ====================
                       // class TestComparator
                       // ====================

template <class TYPE>
class TestComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the template parameter 'TYPE'.
    // The function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.  The function-call operator also
    // increments a global counter used to keep track the method call count.
    // Object of this class can be identified by an id passed on construction.

    // DATA
    int         d_id;           // identifier for the functor
    bool        d_compareLess;  // indicate whether this object use '<' or '>'
    mutable int d_count;        // number of times 'operator()' is called

  public:
    // CLASS METHOD
    static void disableFunctor()
        // Disable all objects of 'TestComparator' such that an 'ASSERT' will
        // be triggered if 'operator()' is invoked.
    {
        g_enableLessThanFunctorFlag = false;
    }

    static void enableFunctor()
        // Enable all objects of 'TestComparator' such that 'operator()' may be
        // invoked.
    {
        g_enableLessThanFunctorFlag = true;
    }

    // CREATORS
    //! TestComparator(const TestComparator& original) = default;
        // Create a copy of the specified 'original'.

    explicit TestComparator(int id = 0, bool compareLess = true)
        // Create a 'TestComparator'.  Optionally, specify 'id' that can be
        // used to identify the object.
    : d_id(id)
    , d_compareLess(compareLess)
    , d_count(0)
    {
    }

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.
    {
        if (!g_enableLessThanFunctorFlag) {
            ASSERTV(!"'TestComparator' was invoked when it was disabled");
        }

        ++d_count;

        if (d_compareLess) {
            return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
            < bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);  // RETURN
        }
        else {
            return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
            > bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);  // RETURN
        }
    }

    bool operator== (const TestComparator& rhs) const
    {
        return (id() == rhs.id() && d_compareLess == rhs.d_compareLess);
    }

    int id() const
        // Return the 'id' of this object.
    {
        return d_id;
    }

    size_t count() const
        // Return the number of times 'operator()' is called.
    {
        return d_count;
    }
};

                       // ============================
                       // class TestComparatorNonConst
                       // ============================

template <class TYPE>
class TestComparatorNonConst {
    // This test class provides a mechanism that defines a non-'const'
    // function-call operator that compares two objects of the template
    // parameter 'TYPE'.  The function-call operator is implemented with
    // integer comparison using integers converted from objects of 'TYPE' by
    // the class method 'TemplateTestFacility::getIdentifier'.  The
    // function-call operator also increments a counter used to keep track of
    // the method call count.  Objects of this class can be identified by an id
    // passed on construction.

    // DATA
    int         d_id;           // identifier for the functor
    bool        d_compareLess;  // indicate whether this object use '<' or '>'
    mutable int d_count;        // number of times 'operator()' was called

  public:
    // CREATORS
    explicit TestComparatorNonConst(int id = 0, bool compareLess = true)
    : d_id(id)
    , d_compareLess(compareLess)
    , d_count(0)
    {
    }

    // MANIPULATORS
    bool operator()(const TYPE& lhs, const TYPE& rhs)
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs', and 'false' otherwise.  Note that C++17 will likely require
        // that the 'operator()' method of comparators be 'const'-qualified.
    {
        ++d_count;

        if (d_compareLess) {
            return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
            < bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);  // RETURN
        }
        else {
            return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
            > bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);  // RETURN
        }
    }

    // ACCESSORS
    bool operator==(const TestComparatorNonConst& rhs) const
    {
        return id() == rhs.id();
    }

    int id() const
        // Return the 'id' of this object.
    {
        return d_id;
    }

    size_t count() const
        // Return the number of times 'operator()' has been called.
    {
        return d_count;
    }
};

                       // =====================
                       // class TemplateWrapper
                       // =====================

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
class TemplateWrapper {
    // This class wraps the container, but does nothing otherwise.  A compiler
    // bug on AIX (xlC) prevents the compiler from finding the definitions of
    // the default arguments for the default constructor.  This class was
    // created to test this scenario.

    // DATA
    bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR> d_member;

  public:
    // CREATORS
    TemplateWrapper()
    : d_member()
    {
    }

    //! TemplateWrapper(const TemplateWrapper&) = default;

    template <class INPUT_ITERATOR>
    TemplateWrapper(INPUT_ITERATOR begin, INPUT_ITERATOR end)
    : d_member(begin, end)
    {
    }
};

                       // =====================
                       // class DummyComparator
                       // =====================

class DummyComparator {
    // A dummy comparator class.  This must be defined after 'TemplateWrapper'
    // to reproduce the AIX bug.

  public:
    bool operator() (int, int)
    {
        return true;
    }
};

                       // ====================
                       // class DummyAllocator
                       // ====================

template <class TYPE>
class DummyAllocator {
    // A dummy allocator class.  This must be defined after 'TemplateWrapper'
    // to reproduce the AIX bug.  Every method is a no-op.

  public:
    // PUBLIC TYPES
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef TYPE           *pointer;
    typedef const TYPE     *const_pointer;
    typedef TYPE&           reference;
    typedef const TYPE&     const_reference;
    typedef TYPE            value_type;

    template <class OTHER_TYPE>
    struct rebind
    {
        typedef DummyAllocator<OTHER_TYPE> other;
    };

    // CREATORS
    DummyAllocator()
    {
    }

    // DummyAllocator(const DummyAllocator& original) = default;

    template <class OTHER_TYPE>
    DummyAllocator(const DummyAllocator<OTHER_TYPE>& original)
    {
        (void) original;
    }

    //! ~DummyAllocator() = default;
        // Destroy this object.

    // MANIPULATORS
    //! DummyAllocator& operator=(const DummyAllocator& rhs) = default;

    pointer allocate(size_type    numElements,
                     const void * hint = 0)
    {
        (void) numElements;    (void) hint;

        return 0;
    }

    void deallocate(pointer address, size_type numElements = 1)
    {
        (void) address;    (void) numElements;
    }

    template <class ELEMENT_TYPE>
    void construct(ELEMENT_TYPE *      address) { (void) address; }
    template <class ELEMENT_TYPE>
    void construct(ELEMENT_TYPE *      address,
                   const ELEMENT_TYPE& value)
    {
        (void) address;    (void) value;
    }

    template <class ELEMENT_TYPE>
    void destroy(ELEMENT_TYPE *address) { (void) address; }

    // ACCESSORS
    pointer address(reference object) const { (void) object; return 0; }

    const_pointer address(const_reference object) const
    {
        (void) object;
        return 0;
    }

    size_type max_size() const { return 0; }
};

                          // =========================
                          // class AmbiguousTestType03
                          // =========================

class AmbiguousTestType03 {
    // This class is convertible from any type that has a 'data' method
    // returning a 'int' value.  It is used to facilitate testing that the
    // implementation of 'bslstl::map' does not pass a 'bslmf::MovableRef<T>'
    // object to a class whose interface does not support it (in C++03 mode).
    // For more details, see the test plan for case 37.

    // DATA
    int d_data;  // value not meaningful

  public:
    // CREATORS
    AmbiguousTestType03()
        // Create a 'AmbiguousTestType03' object having the default value.
    : d_data(0)
    {
    }

    AmbiguousTestType03(const AmbiguousTestType03& original)
        // Create a 'AmbiguousTestType03' object having the same value as the
        // specified 'original' object.
    : d_data(original.d_data)
    {
    }

    template <class TYPE>
    AmbiguousTestType03(const TYPE& other)  // IMPLICIT
        // Create a 'AmbiguousTestType03' object having the same value as the
        // specified 'other' object of (template parameter) 'TYPE'.
    : d_data(other.data())
    {
    }

    // MANIPULATORS
    AmbiguousTestType03& operator=(const AmbiguousTestType03& rhs)
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object.
    {
        d_data = rhs.d_data;
        return *this;
    }

    template <class TYPE>
    AmbiguousTestType03& operator=(const TYPE& rhs)
        // Assign to this object the value of the specified 'rhs' object of
        // (template parameter) 'TYPE', and return a non-'const' reference to
        // this object.
    {
        d_data = rhs.data();
        return *this;
    }

    // ACCESSORS
    int data() const
        // Return the (meaningless) value held by this object.
    {
        return d_data;
    }
};

                       // ========================
                       // class IntToPairConverter
                       // ========================

template <class KEY, class VALUE, class ALLOC>
struct IntToPairConverter {
    // Convert an 'int' identifier to a 'bsl::pair' of the template parameter
    // 'KEY' and 'VALUE' types.

    // CLASS METHODS
    static void
    createInplace(pair<KEY, VALUE> *address, int id, ALLOC allocator)
        // Create a new 'pair<KEY, VALUE>' object at the specified 'address',
        // passing values derived from the specified 'id' to the 'KEY' and
        // 'VALUE' constructors and using the specified 'allocator' to supply
        // memory.  The behavior is undefined unless '0 < id < 128'.
    {
        BSLS_ASSERT(address);
        BSLS_ASSERT( 0 < id);
        BSLS_ASSERT(id < 128);

        typedef typename bsl::remove_const<KEY>::type VarKey;

        // Support generation of pairs '(K, V1)', '(K, V2)' where
        // 'V1 != V2'.  E.g., 'A' and 'a' map to the same 'KEY' but
        // distinct 'VALUE's.

        int key, value;

        if (islower(id)) {
            key   = toupper(id);
            value = key + 1;
        }
        else {
            key   = id;
            value = key - 'A' + '0';
        }

        // Tests have been written that exactly calculate the number of
        // expected allocations and we don't want to rewrite those tests.  This
        // code was originally written only supporting the 'bsl::allocator'
        // allocator type, but we want to expand it to support other allocator
        // types.  The tests were assuming the allocator used here was a
        // scratch allocator, so allocations in this routine weren't counted
        // by the test code that counts allocations.  Then when the objects are
        // copied or moved into the container, the container allocator is
        // passed to the copy or move c'tors so that the right allocator is
        // used in that case.

        // Then we wanted to expand the range of this function to be able to
        // handle other types for 'ALLOC', including std stateful allocators.
        // The problem then is that for that type of the allocator the move and
        // copy c'tors aren't passed an allocator, so in the case of movable
        // allocating types, the allocator we use here will be the allocator
        // the object has within the container.  So, in the case of movable
        // allocating types, we use the 'allocator' passed in as an arg,
        // otherwise we use the scratch singleton.

        bslma::TestAllocator *pss = scratchSingleton();
        const bool useSingleton =
                     !bsl::is_same<VALUE, bsltf::MovableAllocTestType>::value
                  && !bsl::is_same<VALUE, bsltf::MoveOnlyAllocTestType>::value;

        // Note that 'allocator' and 'pss' are of different types, and
        // sometimes this function is called with 'ALLOC' being a type that has
        // no c'tor that takes an 'bslma::Allocator *' arg, so we can't use a
        // ternary on 'useSingleton' to choose which allocator to pass to the
        // 'emplace' methods.

        bsls::ObjectBuffer<VarKey> tempKey;
        if (useSingleton) {
            bsltf::TemplateTestFacility::emplace(tempKey.address(), key, pss);
        }
        else {
            bsltf::TemplateTestFacility::emplace(
                                            tempKey.address(), key, allocator);
        }
        bslma::DestructorGuard<VarKey> keyGuard(tempKey.address());

        bsls::ObjectBuffer<VALUE>  tempValue;
        if (useSingleton) {
            bsltf::TemplateTestFacility::emplace(
                                              tempValue.address(), value, pss);
        }
        else {
            bsltf::TemplateTestFacility::emplace(
                                        tempValue.address(), value, allocator);
        }
        bslma::DestructorGuard<VALUE>  valueGuard(tempValue.address());

        bsl::allocator_traits<ALLOC>::construct(
                          allocator,
                          address,
                          bslmf::MovableRefUtil::move(tempKey.object()),
                          bslmf::MovableRefUtil::move(tempValue.object()));
    }
};

// TBD Comparator-related concerns that are not noted elsewhere or tested:
//
// 1) Testing a comparator that uses a sort order other than the default.
//  'GreaterThanFunctor' is defined (below) to support this, but is not used.
//
// 2) Comparator functions that throw--especially w.r.t. exception neutrality.
//
// 3) Confirm that the allocator for the map does NOT propagate to the
//  comparator; e.g., a comparator with a 'bsl::string' ID that defines the
//  'UsesBslmaAllocator' trait will always use the default allocator and never
//  the object allocator [which is now the standard requirement].
//
// Additional comparator-related review comments:
//
// Function-pointers as comparators, comparators with 'operator()' templates
// (deducing arguments), comparators that copy their arguments (a likely
// throwing-comparator), comparators with
// conversion-to-function-pointer/reference operators, evil comparators that
// disable address-of, copy-assignment, and the comma operator (for good
// measure).  Note that a non-copy-assignable comparator is not swappable by
// default.  (We can also create a comparator that is not assignable, but IS
// ADL swappable, to poke into really dark corners.)
//
// There is NO testing of comparators other than the default, in particular
// there is a serious omission of testing stateful comparators, which would be
// observable changing through the assignment operators and swap.  For a
// full-scale test, I suggest we need a stateful comparator whose state affects
// the sort order.  Two possible examples: both use an 'int' for ID, so we can
// validate state.  The ID should affect sorting; one way would be to use
// operator< or operator> depending on whether the ID is odd or even (e.g.,
// see 'TestComparatorNonConst' (above); an alternative would be to have a
// struct as key, and the ID says which element of the struct should be used
// for sorting.  The latter would be more helpful for testing the comparison
// operators highlighted above.

template <class TYPE>
class GreaterThanFunctor {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the template parameter 'TYPE'.
    // The function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.

  public:
    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const
        // Return 'true' if the integer representation of the specified 'lhs'
        // is greater than the integer representation of the specified 'rhs',
        // and 'false' otherwise.
    {
        return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
             > bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);
    }
};

// FREE OPERATORS
template <class TYPE>
bool lessThanFunction(const TYPE& lhs, const TYPE& rhs)
    // Return 'true' if the integer representation of the specified 'lhs' is
    // less than integer representation of the specified 'rhs'.
{
    return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
         < bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);
}

}  // close unnamed namespace

// ============================================================================
//                      GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

template <class ITER, class VALUE_TYPE>
class TestMovableTypeUtil {
  public:
    static ITER findFirstNotMovedInto(ITER, ITER end)
    {
        return end;
    }
};

template <class ITER>
class TestMovableTypeUtil<ITER, bsltf::MovableAllocTestType> {
  public:
    static ITER findFirstNotMovedInto(ITER begin, ITER end)
    {
        for (; begin != end; ++begin) {
            if (!begin->movedInto()) {
                break;
            }
        }
        return begin;
    }
};

class TestAllocatorUtil {
  public:
    template <class TYPE>
    static void test(int, const TYPE&, const bslma::Allocator&)
    {
    }

    static void test(int                                   line,
                     const bsltf::AllocEmplacableTestType& value,
                     const bslma::Allocator&               allocator)
    {
        ASSERTV(line, &allocator == value.arg01().allocator());
        ASSERTV(line, &allocator == value.arg02().allocator());
        ASSERTV(line, &allocator == value.arg03().allocator());
        ASSERTV(line, &allocator == value.arg04().allocator());
        ASSERTV(line, &allocator == value.arg05().allocator());
        ASSERTV(line, &allocator == value.arg06().allocator());
        ASSERTV(line, &allocator == value.arg07().allocator());
        ASSERTV(line, &allocator == value.arg08().allocator());
        ASSERTV(line, &allocator == value.arg09().allocator());
        ASSERTV(line, &allocator == value.arg10().allocator());
    }
};

namespace {

                       // =========================
                       // struct TestIncompleteType
                       // =========================

struct IncompleteType;
struct TestIncompleteType {
    // This 'struct' provides a simple compile-time test to verify that
    // incomplete types can be used in container definitions.  Currently,
    // definitions of 'bsl::map' can contain incomplete types on all supported
    // platforms.
    //
    // The text below captures the original (now obsolete) rationale for
    // creating this test:
    //..
    //  struct Recursive {
    //      bsl::map<int, Recursive> d_data;
    //  };
    //..
    // This 'struct' provides a simple compile-time test that exposes a bug in
    // the Sun compiler when parsing member-function templates that make use of
    // 'enable_if' to trigger SFINAE effects.  While the 'enable_if' template
    // should not be instantiated until parsing client code calling that
    // function, by which time any incomplete types must have become complete,
    // the Sun CC compiler is parsing the whole 'enable_if' metafunction as
    // soon as it sees it, while instantiating any use of the 'map'.  This
    // causes a request to instantiate 'is_convertible' with incomplete types,
    // which is undefined behavior.  A recent update to the 'is_convertible'
    // trait added a static assertion precisely to catch such misuse.
    //
    // To provide a simple example that will fail to compile (thanks to the
    // static assertion above) unless the problem is worked around, we create a
    // recursive data structure using a map, as the struct 'Recursive' is an
    // incomplete type within its own definition.  Note that there are no test
    // cases exercising 'Recursive', it is sufficient just to define the class.
    //
    // We decided to note the above, but allow the use of the 'is_convertible'
    // meta-function on Sun since it is so important to the new features added
    // as part of the C++11 project.  Now the check is done on every platform
    // *except* for Sun, where we know that a problem exists.

    // PUBLIC TYPES
    typedef bsl::map<int, IncompleteType>::iterator            Iter1;
    typedef bsl::map<IncompleteType, int>::iterator            Iter2;
    typedef bsl::map<IncompleteType, IncompleteType>::iterator Iter3;

    // PUBLIC DATA
    bsl::map<int, IncompleteType>            d_data1;
    bsl::map<IncompleteType, int>            d_data2;
    bsl::map<IncompleteType, IncompleteType> d_data3;
};

struct IncompleteType {
    int d_data;
};

}  // close unnamed namespace

// ============================================================================
//                          TEST DRIVER TEMPLATE
// ----------------------------------------------------------------------------

template <class KEY,
          class VALUE = KEY,
          class COMP  = TestComparator<KEY>,
          class ALLOC = bsl::allocator<bsl::pair<const KEY, VALUE> > >
class TestDriver {
    // This class template provides a namespace for testing the 'map'
    // container.  The template parameter types 'KEY'/'VALUE', 'COMP', and
    // 'ALLOC' specify the value type, comparator type, and allocator type,
    // respectively.  Each "testCase*" method tests a specific aspect of
    // 'map<KEY, VALUE, COMP, ALLOC>'.  Every test case should be invoked with
    // various type arguments to fully test the container.  Note that the
    // (template parameter) 'VALUE' type must be defaulted (to 'KEY') for the
    // benefit of 'RUN_EACH_TYPE'-style testing.

  private:
    // TYPES
    typedef bsl::map<KEY, VALUE, COMP, ALLOC>     Obj;
        // Type under test.

    typedef TestComparatorNonConst<KEY>           NonConstComp;
        // Comparator functor with a non-'const' function call operator.

    // Shorthands

    typedef typename Obj::iterator                Iter;
    typedef typename Obj::const_iterator          CIter;
    typedef typename Obj::reverse_iterator        RIter;
    typedef typename Obj::const_reverse_iterator  CRIter;
    typedef typename Obj::size_type               SizeType;
    typedef typename Obj::value_type              ValueType;

    typedef bsltf::TestValuesArray<typename Obj::value_type, ALLOC,
                      IntToPairConverter<const KEY, VALUE, ALLOC> > TestValues;

    typedef bslma::ConstructionUtil               ConstrUtil;
    typedef bslmf::MovableRefUtil                 MoveUtil;
    typedef bsltf::MoveState                      MoveState;
    typedef bsltf::TemplateTestFacility           TstFacility;
    typedef TestMovableTypeUtil<CIter, ValueType> TstMoveUtil;

    typedef bsl::allocator_traits<ALLOC>          AllocatorTraits;

    enum AllocCategory { e_BSLMA, e_ADAPTOR, e_STATEFUL };

#if defined(BSLS_PLATFORM_OS_AIX) || defined(BSLS_PLATFORM_OS_WINDOWS)
    // Aix has a compiler bug where method pointers do not default construct to
    // 0.  Windows has the same problem.

    enum { k_IS_VALUE_DEFAULT_CONSTRUCTIBLE =
                !bsl::is_same<VALUE,
                              bsltf::TemplateTestFacility::MethodPtr>::value };
#else
    enum { k_IS_VALUE_DEFAULT_CONSTRUCTIBLE = true };
#endif

  public:
    typedef bsltf::StdTestAllocator<ValueType> StlAlloc;

  private:
    // TEST APPARATUS
    //-------------------------------------------------------------------------
    // The generating functions interpret a given 'spec' in order from left to
    // right to configure a given object according to a custom language.  ASCII
    // letters [A..Za..z] correspond to arbitrary (but unique) 'pair' values to
    // be appended to the 'map<KEY, VALUE, COMP, ALLOC>' object.
    //
    // LANGUAGE SPECIFICATION
    // ----------------------
    //
    // <SPEC>       ::= <EMPTY>   | <LIST>
    //
    // <EMPTY>      ::=
    //
    // <LIST>       ::= <ELEMENT> | <ELEMENT> <LIST>
    //
    // <ELEMENT>    ::= 'A' | 'B' | ... | 'Z' | 'a' | 'b' | ... | 'z'
    //                                      // unique but otherwise arbitrary
    // Spec String  Description
    // -----------  -----------------------------------------------------------
    // ""           Has no effect; leaves the object in its original state.
    // "A"          Insert the value corresponding to A.
    // "AA"         Insert two values, both corresponding to A.
    // "ABC"        Insert three values corresponding to A, B, and C.
    //-------------------------------------------------------------------------

    // CLASS DATA
    static
    const AllocCategory s_allocCategory =
                        bsl::is_same<ALLOC, bsl::allocator<ValueType> >::value
                        ? e_BSLMA
                        : bsl::is_same<ALLOC,
                                       bsltf::StdAllocatorAdaptor<
                                           bsl::allocator<ValueType> > >::value
                        ? e_ADAPTOR
                        : e_STATEFUL;

    static
    const bool s_valueIsMoveEnabled =
                     bsl::is_same<VALUE, bsltf::MovableTestType>::value ||
                     bsl::is_same<VALUE, bsltf::MovableAllocTestType>::value ||
                     bsl::is_same<VALUE, bsltf::MoveOnlyAllocTestType>::value;

    // CLASS METHODS
    static
    const char *allocCategoryAsStr()
    {
        return e_BSLMA == s_allocCategory ? "bslma"
                                          : e_ADAPTOR == s_allocCategory
                                          ? "adaptor"
                                          : e_STATEFUL == s_allocCategory
                                          ? "stateful"
                                          : "<INVALID>";
    }

    static int ggg(Obj *object, const char *spec, bool verbose = true);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'insert'.  Optionally
        // specify a 'false' 'verbose' to suppress 'spec' syntax error
        // messages.  Return the index of the first invalid character, and a
        // negative value otherwise.  Note that this function is used to
        // implement 'gg' as well as allow for verification of syntax error
        // detection.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified 'object' with its value adjusted
        // according to the specified 'spec'.

    static void storeFirstNElemAddr(const typename Obj::value_type *pointers[],
                                    const Obj&                      object,
                                    size_t                          n);
        // Load into the specified 'pointers' array, in order, the addresses
        // that provide non-modifiable access to the specified initial 'n'
        // elements in the ordered sequence of 'value_type' values held by the
        // specified 'object'.  The behavior is undefined unless the length of
        // 'pointers' is at least 'n'.

    static int checkFirstNElemAddr(const typename Obj::value_type *pointers[],
                                   const Obj&                      object,
                                   size_t                          n);
        // Return the number of items in the specified 'pointers' array whose
        // values, in order, are not the same as the addresses that provide
        // non-modifiable access to the specified initial 'n' elements in the
        // ordered sequence of 'value_type' values held by the specified
        // 'object'.  The behavior is undefined unless the length of 'pointers'
        // is at least 'n'.

    static pair<Iter, bool> primaryManipulator(Obj   *container,
                                               int    identifier,
                                               ALLOC  allocator);
        // Insert into the specified 'container' the 'pair' object indicated by
        // the specified 'identifier', ensuring that the overload of the
        // primary manipulator taking a modifiable rvalue is invoked (rather
        // than the one taking an lvalue).  Return the result of invoking the
        // primary manipulator.

    template <class T>
    static bslmf::MovableRef<T> testArg(T& t, bsl::true_type)
    {
        return MoveUtil::move(t);
    }

    template <class T>
    static const T&             testArg(T& t, bsl::false_type)
    {
        return t;
    }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
    template <int NUM_KEY_ARGS,
              int NK1,
              int NK2,
              int NK3,
              int NUM_VALUE_ARGS,
              int NV1,
              int NV2,
              int NV3>
    static void testCase31a_RunTest(Obj *target, bool inserted);
        // Call 'emplace' on the specified 'target' container and verify that a
        // value was newly inserted if and only if the specified 'inserted'
        // flag is 'true'.  Forward 'NUM_KEY_ARGS' arguments (for
        // 'value_type.first', the 'KEY') and 'NUM_VALUE_ARGS' arguments (for
        // 'value_type.second', the 'VALUE') to the 'emplace' method and
        // ensure: 1) that argument values are properly passed to the piecewise
        // constructor of 'value_type', 2) that the allocator is correctly
        // configured for each argument in the newly inserted element in
        // 'target', and 3) that the arguments are forwarded using copy or move
        // semantics based on the integer template parameters '[NK1 .. NK3]'
        // and '[NV1 .. NV3]'.  Interpret the values of 'NKi' and 'NVi' as
        // follows:
        //..
        //  NKi == 0 => forward the i'th 'KEY' argument using copy semantics
        //  NVi == 0 => forward the i'th 'VALUE' argument using copy semantics
        //
        //  NKi == 1 => forward the i'th 'KEY' argument using move semantics
        //  NVi == 1 => forward the i'th 'VALUE' argument using move semantics
        //
        //  NKi == 2 => don't forward a value for the i'th 'KEY' argument
        //  NVi == 2 => don't forward a value for the i'th 'VALUE' argument
        //..
        // The behavior is undefined unless '0 <= NUM_KEY_ARGS <= 3',
        // '0 <= NUM_VALUE_ARGS <= 3', all '[NK1 .. NK3]' and '[NV1 .. NV3]'
        // parameters are in the range '[0 .. 2]', '2 == NK2' if '2 == NK1',
        // '2 == NK3' if '2 == NK2', '2 == NV2' if '2 == NV1', and '2 == NV3'
        // if '2 == NV2'.

    template <int NUM_KEY_ARGS,
              int NK1,
              int NK2,
              int NK3,
              int NUM_VALUE_ARGS,
              int NV1,
              int NV2,
              int NV3>
    static Iter testCase32a_RunTest(Obj *target, CIter hint, bool inserted);
        // Call 'emplace_hint' on the specified 'target' container with the
        // specified 'hint' and verify that a value was newly inserted if and
        // only if the specified 'inserted' flag is 'true'.  Forward
        // 'NUM_KEY_ARGS' arguments (for 'value_type.first', the 'KEY') and
        // 'NUM_VALUE_ARGS' arguments (for 'value_type.second', the 'VALUE') to
        // the 'emplace_hint' method and ensure: 1) that argument values are
        // properly passed to the piecewise constructor of 'value_type', 2)
        // that the allocator is correctly configured for each argument in the
        // newly inserted element in 'target', and 3) that the arguments are
        // forwarded using copy or move semantics based on the integer template
        // parameters '[NK1 .. NK3]' and '[NV1 .. NV3]'.  Interpret the values
        // of 'NKi' and 'NVi' as follows:
        //..
        //  NKi == 0 => forward the i'th 'KEY' argument using copy semantics
        //  NVi == 0 => forward the i'th 'VALUE' argument using copy semantics
        //
        //  NKi == 1 => forward the i'th 'KEY' argument using move semantics
        //  NVi == 1 => forward the i'th 'VALUE' argument using move semantics
        //
        //  NKi == 2 => don't forward a value for the i'th 'KEY' argument
        //  NVi == 2 => don't forward a value for the i'th 'VALUE' argument
        //..
        // The behavior is undefined unless '0 <= NUM_KEY_ARGS <= 3',
        // '0 <= NUM_VALUE_ARGS <= 3', all '[NK1 .. NK3]' and '[NV1 .. NV3]'
        // parameters are in the range '[0 .. 2]', '2 == NK2' if '2 == NK1',
        // '2 == NK3' if '2 == NK2', '2 == NV2' if '2 == NV1', and '2 == NV3'
        // if '2 == NV2'.
#endif

  public:
    // TEST CASES

    static void testCase37();
        // Test 'bslmf::MovableRef<T>' does not escape (in C++03 mode).

    static void testCase36();
        // Test 'noexcept' specifications

    static void testCase34();
        // Test element access with movable key.  'VALUE' must be default
        // constructible.

    static void testCase33();
        // Test initializer list functions.

    static void testCase32a();
        // Test forwarding of arguments in 'emplace_hint' method (adjunct to
        // 'testCase32').

    static void testCase32();
        // Test 'emplace_hint' method (except for forwarding of arguments).

    static void testCase31a();
        // Test forwarding of arguments in 'emplace' method (adjunct to
        // 'testCase31').

    static void testCase31();
        // Test 'emplace' method (except for forwarding of arguments).

    static void testCase30();
        // Test insert with hint of movable value.

    static void testCase29();
        // Test insert of movable value.

    static void testCase28_dispatch();
        // Test move-assignment operator.

    static void testCase27();
        // Test move constructor.

    static void testCase26();
        // Test standard interface coverage.

    static void testCase25();
        // Test constructors of a template wrapper class.

    static void testCase24();
        // Test element access.  'VALUE' must be default constructible.

    static void testCase23();
        // Test type traits.

    static void testCase22();
        // Test STL allocator.

    static void testCase21();
        // Test comparators.

    static void testCase20();
        // Test 'max_size' and 'empty'.

    static void testCase19();
        // Test free comparison operators.  'operator<' must be defined for the
        // (template parameter) 'KEY' and 'VALUE' types.

    static void testCase18();
        // Test 'erase'.

    static void testCase17();
        // Test range 'insert'.

    static void testCase16();
        // Test 'insert' with hint of single value.

    static void testCase15();
        // Test 'insert' of single value.

    static void testCase14();
        // Test iterators.

    static void testCase13();
        // Test 'find', 'count', 'upper_bound', 'lower_bound', 'equal_range'.

    static void testCase12();
        // Test range constructors.

    // static void testCase11();
        // Does nothing -- formerly tested the 'g' function, which is no longer
        // implemented.

    // static void testCase10();
        // Reserved for BDEX.

    template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase9_propagate_on_container_copy_assignment_dispatch();
    static void testCase9_propagate_on_container_copy_assignment();
        // Test 'propagate_on_container_copy_assignment'.

    static void testCase9();
        // Test copy-assignment operator.

    static void testCase8_dispatch();
        // Test 'swap' member and free functions.

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
        // Test basic accessors ('size', 'cbegin', 'cend', 'get_allocator').

    static void testCase3();
        // Test generator functions 'ggg', and 'gg'.

    static void testCase2();
        // Test primary manipulators ('insert' and 'clear').

    static void testCase1(const COMP&  comparator,
                          KEY         *testKeys,
                          VALUE       *testValues,
                          size_t       numValues);
        // Breathing test.  This test *exercises* basic functionality but
        // *test* nothing.
};

template <class KEY, class VALUE = KEY>
class StdAllocTestDriver :
    public TestDriver<KEY,
                      VALUE,
                      TestComparator<KEY>,
                      bsltf::StdTestAllocator<bsl::pair<const KEY, VALUE> > >
{
};

                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class KEY, class VALUE, class COMP, class ALLOC>
int TestDriver<KEY, VALUE, COMP, ALLOC>::ggg(Obj        *object,
                                             const char *spec,
                                             bool        verbose)
{
    enum { SUCCESS = -1 };

    bslma::TestAllocator *pss = scratchSingleton();
    const Int64 B = pss->numBlocksInUse();
    ALLOC ss(pss);

    for (int i = 0; spec[i]; ++i) {
        if (isalpha(spec[i])) {
            primaryManipulator(object, spec[i], ss);
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

    ASSERTV(NameOf<VALUE>(), spec, pss->numBlocksInUse(), B,
                                                   pss->numBlocksInUse() == B);

    return SUCCESS;
}

template <class KEY, class VALUE, class COMP, class ALLOC>
bsl::map<KEY, VALUE, COMP, ALLOC>& TestDriver<KEY, VALUE, COMP, ALLOC>::gg(
                                                            Obj        *object,
                                                            const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::storeFirstNElemAddr(
                                    const typename Obj::value_type *pointers[],
                                    const Obj&                      object,
                                    size_t                          n)
{
    size_t i = 0;

    for (CIter b = object.cbegin(); b != object.cend() && i < n; ++b) {
        pointers[i++] = bsls::Util::addressOf(*b);
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
int TestDriver<KEY, VALUE, COMP, ALLOC>::checkFirstNElemAddr(
                                    const typename Obj::value_type *pointers[],
                                    const Obj&                      object,
                                    size_t                          n)
{
    int    count = 0;
    size_t i     = 0;

    for (CIter b = object.cbegin(); b != object.end() && i < n; ++b) {
        if (bsls::Util::addressOf(*b) != pointers[i++]) {
            ++count;
        }
    }

    return count;
}

template <class KEY, class VALUE, class COMP, class ALLOC>
pair<typename TestDriver<KEY, VALUE, COMP, ALLOC>::Iter, bool>
TestDriver<KEY, VALUE, COMP, ALLOC>::primaryManipulator(Obj   *container,
                                                        int    identifier,
                                                        ALLOC  allocator)
{
    typedef pair<KEY, VALUE> TValueType;

    // If the 'VALUE' type is a move-enabled allocating type, use the
    // container allocator, in which case the memory the object allocates will
    // be moved into the object inserted into the container.  Otherwise, the
    // 'move' will wind up doing a 'copy', in which case we will have done
    // extra allocations using the container's allocator, which would throw
    // off some test cases which are carefully counting allocations done by
    // that allocator in the ggg function.

    ALLOC allocToUse =
                     (bsl::is_same<VALUE, bsltf::MovableAllocTestType>::value
                   || bsl::is_same<VALUE, bsltf::MoveOnlyAllocTestType>::value)
                   ? container->get_allocator()
                   : allocator;

    bsls::ObjectBuffer<TValueType> buffer;
    IntToPairConverter<KEY, VALUE, ALLOC>::createInplace(buffer.address(),
                                                         identifier,
                                                         allocToUse);
    bslma::DestructorGuard<TValueType> guard(buffer.address());

    return container->insert(MoveUtil::move(buffer.object()));
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
template <class KEY, class VALUE, class COMP, class ALLOC>
template <int NUM_KEY_ARGS,
          int NK1,
          int NK2,
          int NK3,
          int NUM_VALUE_ARGS,
          int NV1,
          int NV2,
          int NV3>
void
TestDriver<KEY, VALUE, COMP, ALLOC>::testCase31a_RunTest(Obj  *target,
                                                         bool  inserted)
{
    BSLMF_ASSERT((0 <= NUM_KEY_ARGS   && NUM_KEY_ARGS   <= 3));
    BSLMF_ASSERT((0 <= NUM_VALUE_ARGS && NUM_VALUE_ARGS <= 3));

    // In C++17, these become the simpler-to-name 'bool_constant'.

    static const bsl::integral_constant<bool, NK1 == 1> MOVE_K1 = {};
    static const bsl::integral_constant<bool, NK2 == 1> MOVE_K2 = {};
    static const bsl::integral_constant<bool, NK3 == 1> MOVE_K3 = {};
    static const bsl::integral_constant<bool, NV1 == 1> MOVE_V1 = {};
    static const bsl::integral_constant<bool, NV2 == 1> MOVE_V2 = {};
    static const bsl::integral_constant<bool, NV3 == 1> MOVE_V3 = {};

    bslma::TestAllocator *testAlloc = dynamic_cast<bslma::TestAllocator *>(
                                          target->get_allocator().mechanism());
    if (!testAlloc) {
        ASSERT(!"Allocator in test case 31 is not a test allocator!");
        return;                                                       // RETURN
    }

    bslma::TestAllocator& oa = *testAlloc;
    Obj& mX = *target;  const Obj& X = mX;

    bslma::TestAllocator aa("args", veryVeryVeryVerbose);
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    pair<Iter, bool> result;

    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

        // Construct all arguments inside the exception test loop as the
        // exception thrown after moving only a portion of arguments leave the
        // moved arguments in a valid, but unspecified state.
        bsls::ObjectBuffer<typename KEY::ArgType01> BUFK1;
        ConstrUtil::construct(BUFK1.address(), &aa, K01);
        typename KEY::ArgType01& AK1 = BUFK1.object();
        bslma::DestructorGuard<typename KEY::ArgType01>   GK1(&AK1);

        bsls::ObjectBuffer<typename KEY::ArgType02> BUFK2;
        ConstrUtil::construct(BUFK2.address(), &aa, K02);
        typename KEY::ArgType02& AK2 = BUFK2.object();
        bslma::DestructorGuard<typename KEY::ArgType02>   GK2(&AK2);

        bsls::ObjectBuffer<typename KEY::ArgType03> BUFK3;
        ConstrUtil::construct(BUFK3.address(), &aa, K03);
        typename KEY::ArgType03& AK3 = BUFK3.object();
        bslma::DestructorGuard<typename KEY::ArgType03>   GK3(&AK3);

        bsls::ObjectBuffer<typename VALUE::ArgType01> BUFV1;
        ConstrUtil::construct(BUFV1.address(), &aa, V01);
        typename VALUE::ArgType01& AV1 = BUFV1.object();
        bslma::DestructorGuard<typename VALUE::ArgType01> GV1(&AV1);

        bsls::ObjectBuffer<typename VALUE::ArgType02> BUFV2;
        ConstrUtil::construct(BUFV2.address(), &aa, V02);
        typename VALUE::ArgType02& AV2 = BUFV2.object();
        bslma::DestructorGuard<typename VALUE::ArgType02> GV2(&AV2);

        bsls::ObjectBuffer<typename VALUE::ArgType03> BUFV3;
        ConstrUtil::construct(BUFV3.address(), &aa, V03);
        typename VALUE::ArgType03& AV3 = BUFV3.object();
        bslma::DestructorGuard<typename VALUE::ArgType03> GV3(&AV3);

        ExceptionProctor<Obj> proctor(&X, L_, &scratch);

        switch (NUM_KEY_ARGS) {
          case 0: {
            switch (NUM_VALUE_ARGS) {
              case 0: {
                result = mX.emplace(native_std::piecewise_construct,
                                    native_std::forward_as_tuple(),
                                    native_std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2),
                                                       testArg(AV3, MOVE_V3)));
              } break;
              default: {
                // Invalid number of arguments ('BSLMF_ASSERT'ed on entry).
              } return;                                               // RETURN
            }
          } break;
          case 1: {
            switch (NUM_VALUE_ARGS) {
              case 0: {
                result = mX.emplace(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1)),
                          native_std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1)),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1)),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1)),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2),
                                                       testArg(AV3, MOVE_V3)));
              } break;
              default: {
                // Invalid number of arguments ('BSLMF_ASSERT'ed on entry).
              } return;                                               // RETURN
            }
          } break;
          case 2: {
            switch (NUM_VALUE_ARGS) {
              case 0: {
                result = mX.emplace(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2)),
                          native_std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2)),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2)),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2)),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2),
                                                       testArg(AV3, MOVE_V3)));
              } break;
              default: {
                // Invalid number of arguments ('BSLMF_ASSERT'ed on entry).
              } return;                                               // RETURN
            }
          } break;
          case 3: {
            switch (NUM_VALUE_ARGS) {
              case 0: {
                result = mX.emplace(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2),
                                                       testArg(AK3, MOVE_K3)),
                          native_std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2),
                                                       testArg(AK3, MOVE_K3)),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2),
                                                       testArg(AK3, MOVE_K3)),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2),
                                                       testArg(AK3, MOVE_K3)),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2),
                                                       testArg(AV3, MOVE_V3)));
              } break;
              default: {
                // Invalid number of arguments ('BSLMF_ASSERT'ed on entry).
              } return;                                               // RETURN
            }
          } break;
          default: {
            // Invalid number of arguments ('BSLMF_ASSERT'ed on entry).
          } return;                                                   // RETURN
        }

        proctor.release();

        ASSERTV(inserted, inserted == result.second);

        ASSERTV(MOVE_K1, AK1.movedFrom(),
               MOVE_K1 == (MoveState::e_MOVED == AK1.movedFrom()) || 2 == NK1);
        ASSERTV(MOVE_K2, AK2.movedFrom(),
               MOVE_K2 == (MoveState::e_MOVED == AK2.movedFrom()) || 2 == NK2);
        ASSERTV(MOVE_K3, AK3.movedFrom(),
               MOVE_K3 == (MoveState::e_MOVED == AK3.movedFrom()) || 2 == NK3);

        ASSERTV(MOVE_V1, AV1.movedFrom(),
               MOVE_V1 == (MoveState::e_MOVED == AV1.movedFrom()) || 2 == NV1);
        ASSERTV(MOVE_V2, AV2.movedFrom(),
               MOVE_V2 == (MoveState::e_MOVED == AV2.movedFrom()) || 2 == NV2);
        ASSERTV(MOVE_V3, AV3.movedFrom(),
               MOVE_V3 == (MoveState::e_MOVED == AV3.movedFrom()) || 2 == NV3);

        const KEY& K = result.first->first;
        const VALUE& V = result.first->second;

        ASSERTV(K01, K.arg01(), K01 == K.arg01() || 2 == NK1);
        ASSERTV(K02, K.arg02(), K02 == K.arg02() || 2 == NK2);
        ASSERTV(K03, K.arg03(), K03 == K.arg03() || 2 == NK3);

        if (inserted) {
            ASSERTV(V01, V.arg01(), V01 == V.arg01() || 2 == NV1);
            ASSERTV(V02, V.arg02(), V02 == V.arg02() || 2 == NV2);
            ASSERTV(V03, V.arg03(), V03 == V.arg03() || 2 == NV3);
        }

        TestAllocatorUtil::test(L_, K, oa);
        TestAllocatorUtil::test(L_, V, oa);
    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
}

template <class KEY, class VALUE, class COMP, class ALLOC>
template <int NUM_KEY_ARGS,
          int NK1,
          int NK2,
          int NK3,
          int NUM_VALUE_ARGS,
          int NV1,
          int NV2,
          int NV3>
typename TestDriver<KEY, VALUE, COMP, ALLOC>::Iter
TestDriver<KEY, VALUE, COMP, ALLOC>::testCase32a_RunTest(Obj   *target,
                                                         CIter  hint,
                                                         bool   inserted)
{
    BSLMF_ASSERT((0 <= NUM_KEY_ARGS   && NUM_KEY_ARGS   <= 3));
    BSLMF_ASSERT((0 <= NUM_VALUE_ARGS && NUM_VALUE_ARGS <= 3));

    // In C++17, these become the simpler-to-name 'bool_constant'.

    static const bsl::integral_constant<bool, NK1 == 1> MOVE_K1 = {};
    static const bsl::integral_constant<bool, NK2 == 1> MOVE_K2 = {};
    static const bsl::integral_constant<bool, NK3 == 1> MOVE_K3 = {};
    static const bsl::integral_constant<bool, NV1 == 1> MOVE_V1 = {};
    static const bsl::integral_constant<bool, NV2 == 1> MOVE_V2 = {};
    static const bsl::integral_constant<bool, NV3 == 1> MOVE_V3 = {};

    bslma::TestAllocator *testAlloc = dynamic_cast<bslma::TestAllocator *>(
                                          target->get_allocator().mechanism());
    if (!testAlloc) {
        ASSERT(!"Allocator in test case 32 is not a test allocator!");
        return target->end();                                         // RETURN
    }

    bslma::TestAllocator& oa = *testAlloc;
    Obj& mX = *target;  const Obj& X = mX;

    bslma::TestAllocator aa("args", veryVeryVeryVerbose);
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    Iter result;

    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

        // Construct all arguments inside the exception test loop as the
        // exception thrown after moving only a portion of arguments leave the
        // moved arguments in a valid, but unspecified state.
        bsls::ObjectBuffer<typename KEY::ArgType01> BUFK1;
        ConstrUtil::construct(BUFK1.address(), &aa, K01);
        typename KEY::ArgType01& AK1 = BUFK1.object();
        bslma::DestructorGuard<typename KEY::ArgType01>   GK1(&AK1);

        bsls::ObjectBuffer<typename KEY::ArgType02> BUFK2;
        ConstrUtil::construct(BUFK2.address(), &aa, K02);
        typename KEY::ArgType02& AK2 = BUFK2.object();
        bslma::DestructorGuard<typename KEY::ArgType02>   GK2(&AK2);

        bsls::ObjectBuffer<typename KEY::ArgType03> BUFK3;
        ConstrUtil::construct(BUFK3.address(), &aa, K03);
        typename KEY::ArgType03& AK3 = BUFK3.object();
        bslma::DestructorGuard<typename KEY::ArgType03>   GK3(&AK3);

        bsls::ObjectBuffer<typename VALUE::ArgType01> BUFV1;
        ConstrUtil::construct(BUFV1.address(), &aa, V01);
        typename VALUE::ArgType01& AV1 = BUFV1.object();
        bslma::DestructorGuard<typename VALUE::ArgType01> GV1(&AV1);

        bsls::ObjectBuffer<typename VALUE::ArgType02> BUFV2;
        ConstrUtil::construct(BUFV2.address(), &aa, V02);
        typename VALUE::ArgType02& AV2 = BUFV2.object();
        bslma::DestructorGuard<typename VALUE::ArgType02> GV2(&AV2);

        bsls::ObjectBuffer<typename VALUE::ArgType03> BUFV3;
        ConstrUtil::construct(BUFV3.address(), &aa, V03);
        typename VALUE::ArgType03& AV3 = BUFV3.object();
        bslma::DestructorGuard<typename VALUE::ArgType03> GV3(&AV3);

        ExceptionProctor<Obj> proctor(&X, L_, &scratch);

        switch (NUM_KEY_ARGS) {
          case 0: {
            switch (NUM_VALUE_ARGS) {
              case 0: {
                result = mX.emplace_hint(hint,
                                         native_std::piecewise_construct,
                                         native_std::forward_as_tuple(),
                                         native_std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace_hint(
                          hint,
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace_hint(
                          hint,
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace_hint(
                          hint,
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2),
                                                       testArg(AV3, MOVE_V3)));
              } break;
              default: {
                // Invalid number of arguments ('BSLMF_ASSERT'ed on entry).
              } return mX.end();                                      // RETURN
            }
          } break;
          case 1: {
            switch (NUM_VALUE_ARGS) {
              case 0: {
                result = mX.emplace_hint(
                          hint,
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1)),
                          native_std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace_hint(
                          hint,
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1)),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace_hint(
                          hint,
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1)),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace_hint(
                          hint,
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1)),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2),
                                                       testArg(AV3, MOVE_V3)));
              } break;
              default: {
                // Invalid number of arguments ('BSLMF_ASSERT'ed on entry).
              } return mX.end();                                      // RETURN
            }
          } break;
          case 2: {
            switch (NUM_VALUE_ARGS) {
              case 0: {
                result = mX.emplace_hint(
                          hint,
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2)),
                          native_std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace_hint(
                          hint,
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2)),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace_hint(
                          hint,
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2)),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace_hint(
                          hint,
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2)),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2),
                                                       testArg(AV3, MOVE_V3)));
              } break;
              default: {
                // Invalid number of arguments ('BSLMF_ASSERT'ed on entry).
              } return mX.end();                                      // RETURN
            }
          } break;
          case 3: {
            switch (NUM_VALUE_ARGS) {
              case 0: {
                result = mX.emplace_hint(
                          hint,
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2),
                                                       testArg(AK3, MOVE_K3)),
                          native_std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace_hint(
                          hint,
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2),
                                                       testArg(AK3, MOVE_K3)),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace_hint(
                          hint,
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2),
                                                       testArg(AK3, MOVE_K3)),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace_hint(
                          hint,
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2),
                                                       testArg(AK3, MOVE_K3)),
                          native_std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2),
                                                       testArg(AV3, MOVE_V3)));
              } break;
              default: {
                // Invalid number of arguments ('BSLMF_ASSERT'ed on entry).
              } return mX.end();                                      // RETURN
            }
          } break;
          default: {
            // Invalid number of arguments ('BSLMF_ASSERT'ed on entry).
          } return mX.end();                                          // RETURN
        }

        proctor.release();

        ASSERTV(inserted, inserted == (&(*result) != &(*hint)));

        ASSERTV(MOVE_K1, AK1.movedFrom(),
               MOVE_K1 == (MoveState::e_MOVED == AK1.movedFrom()) || 2 == NK1);
        ASSERTV(MOVE_K2, AK2.movedFrom(),
               MOVE_K2 == (MoveState::e_MOVED == AK2.movedFrom()) || 2 == NK2);
        ASSERTV(MOVE_K3, AK3.movedFrom(),
               MOVE_K3 == (MoveState::e_MOVED == AK3.movedFrom()) || 2 == NK3);

        ASSERTV(MOVE_V1, AV1.movedFrom(),
               MOVE_V1 == (MoveState::e_MOVED == AV1.movedFrom()) || 2 == NV1);
        ASSERTV(MOVE_V2, AV2.movedFrom(),
               MOVE_V2 == (MoveState::e_MOVED == AV2.movedFrom()) || 2 == NV2);
        ASSERTV(MOVE_V3, AV3.movedFrom(),
               MOVE_V3 == (MoveState::e_MOVED == AV3.movedFrom()) || 2 == NV3);

        const KEY&   K = result->first;
        const VALUE& V = result->second;

        ASSERTV(K01, K.arg01(), K01 == K.arg01() || 2 == NK1);
        ASSERTV(K02, K.arg02(), K02 == K.arg02() || 2 == NK2);
        ASSERTV(K03, K.arg03(), K03 == K.arg03() || 2 == NK3);

        if (inserted) {
            ASSERTV(V01, V.arg01(), V01 == V.arg01() || 2 == NV1);
            ASSERTV(V02, V.arg02(), V02 == V.arg02() || 2 == NV2);
            ASSERTV(V03, V.arg03(), V03 == V.arg03() || 2 == NV3);
        }

        TestAllocatorUtil::test(L_, K, oa);
        TestAllocatorUtil::test(L_, V, oa);

    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
    return result;
}
#endif

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase37()
{
    // ------------------------------------------------------------------------
    // 'bslmf::MovableRef<T>' DOES NOT ESCAPE
    //
    // Concerns:
    //: 1 That the implementation of 'bslstl::map' does not pass a
    //:   'bslmf::MovableRef<T>' object to a class whose interface does not
    //:   support it (in C++03 mode).
    //
    // Plan:
    //: 1 In the absence of a trait, the 'bslstl::map' implementation cannot
    //:   determine whether a 'T' type supports 'bslmf::MovableRef<T>' in its
    //:   interface (important to C++03 mode only).  Consequently, the
    //:   implementation cannot pass 'bslmf::MovableRef<T>' to any 'T' method
    //:   or else a compilation error may result (for example, a converting
    //:   constructor passed a 'bslmf::MovableRef<T>' may fail to compile).
    //:   Using brute force and a specially tailored test type,
    //:   'AmbiguousTestType03', we verify that such an argument is not passed
    //:   to user-defined constructors.  Note that this is a compile-only test;
    //:   runtime values are not checked.  (C-1)
    //
    // Testing:
    //   CONCERN: 'bslmf::MovableRef<T>' does not escape (in C++03 mode).
    // ------------------------------------------------------------------------

    if (verbose) printf("'bslmf::MovableRef<T>' DOES NOT ESCAPE\n"
                        "======================================\n");

    if (verbose) printf("\tTesting 'operator[]'\n");
    {
        Obj mX;
        VALUE z = mX[15];
        (void)z;
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase36()
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
    //: 2 Since the 'noexcept' specification does not vary with the 'TYPE'
    //:   of the container, we need test for just one general type and any
    //:   'TYPE' specializations.
    //
    // Testing:
    //   CONCERN: Methods qualifed 'noexcept' in standard are so implemented.
    // ------------------------------------------------------------------------

    if (verbose) {
        P(bsls::NameOf<Obj>())
        P(bsls::NameOf<KEY>())
        P(bsls::NameOf<VALUE>())
        P(bsls::NameOf<COMP>())
        P(bsls::NameOf<ALLOC>())
    }

    // N4594: page 863: 23.4.4 Class template 'map' [map]

    // page 864
    //..
    //  // 23.4.4.2, construct/copy/destroy:
    //  map& operator=(map&& x)
    //      noexcept(allocator_traits<Allocator>::is_always_equal::value &&
    //               is_nothrow_move_assignable<Compare>::value);
    //  allocator_type get_allocator() const noexcept;
    //..

    {
        Obj mX;    const Obj& X = mX;
        Obj mY;

        (void) X;    (void) mY;

        ASSERT(BSLS_CPP11_PROVISIONALLY_FALSE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(mX =
                                             bslmf::MovableRefUtil::move(mY)));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR( X.get_allocator()));
    }

    // page 864-865
    //..
    //  // iterators:
    //  iterator begin() noexcept;
    //  const_iterator begin() const noexcept;
    //  iterator end() noexcept;
    //  const_iterator end() const noexcept;
    //  reverse_iterator rbegin() noexcept;
    //  const_reverse_iterator rbegin() const noexcept;
    //  reverse_iterator rend() noexcept;
    //  const_reverse_iterator rend() const noexcept;
    //  const_iterator cbegin() const noexcept;
    //  const_iterator cend() const noexcept;
    //  const_reverse_iterator crbegin() const noexcept;
    //  const_reverse_iterator crend() const noexcept;
    //..

    {
        Obj mX;    const Obj& X = mX;    (void) X;

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(mX.begin()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR( X.begin()));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(mX.end()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR( X.end()));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(mX.rbegin()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR( X.rbegin()));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(mX.rend()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR( X.rend()));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR( X.cbegin()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR( X.cend()));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR( X.crbegin()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR( X.crend()));
    }

    // page 865
    //..
    //  // capacity:
    //  bool empty() const noexcept;
    //  size_type size() const noexcept;
    //  size_type max_size() const noexcept;
    //..

    {
        Obj mX;    const Obj& X = mX;    (void) X;

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(X.empty()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(X.size()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(X.max_size()));
    }

    // page 866
    //..
    //  // 23.4.4.4, modifiers:
    //  void swap(map&)
    //      noexcept(allocator_traits<Allocator>::is_always_equal::value &&
    //               is_nothrow_swappable_v<Compare>);
    //  void clear() noexcept;
    //..

    {
        Obj x;    (void) x;
        Obj y;    (void) y;

        ASSERT(BSLS_CPP11_PROVISIONALLY_FALSE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(x.swap(y)));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(x.clear()));
    }

    // page 866
    //..
    //  // 23.4.4.5, specialized algorithms:
    //  template <class Key, class T, class Compare, class Allocator>
    //  void swap(map<Key, T, Compare, Allocator>& x,
    //  map<Key, T, Compare, Allocator>& y)
    //      noexcept(noexcept(x.swap(y)));
    //..

    {
        Obj x;    (void) x;
        Obj y;    (void) y;

        ASSERT(BSLS_CPP11_PROVISIONALLY_FALSE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(x.swap(y)));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(x.clear()));
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase34()
{
    // ------------------------------------------------------------------------
    // TESTING ELEMENTAL ACCESS WITH MOVABLE KEY
    //
    // Concerns:
    //: 1 'operator[]' returns the value associated with the key.
    //:
    //: 2 'operator[]' can be used to set the value.
    //:
    //: 3 'operator[]' creates a default constructed value if the key does not
    //:   exist in the object.
    //:
    //: 4 New keys are move-inserted into the container.
    //:
    //: 5 Any memory allocations come from the object's allocator.
    //:
    //: 6 'operator[]' is exception neutral.
    //
    // Plan:
    //: 1 Using the table-driven technique, for each set of key-value pairs:
    //:
    //:   1 For each key-value pair in the object:
    //:
    //:     1 Verify 'operator[]' returns the expected 'VALUE'.
    //:
    //:     2 Verify no memory is allocated.
    //:
    //:     3 Set the value to something different with 'operator[]' and verify
    //:       the value is changed.  Set the value back to its original value.
    //:
    //:     4 Verify memory use did not increase.
    //:
    //:   2 Verify no memory is allocated from the default allocator.
    //:
    //:   3 Invoke 'operator[]' using an out-of-range key under the presence of
    //:     exceptions.
    //:
    //:   4 Verify that a default 'VALUE' is created.
    //:
    //:   5 Verify memory usage is as expected.
    //
    // Testing:
    //   VALUE& operator[](key_type&& key);
    // ------------------------------------------------------------------------

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const TestValues VALUES("yz");      // values not in any spec

    const KEY&   ZK = VALUES[0].first;
    const VALUE& ZV = VALUES[1].second;

    if (verbose) printf("Testing elemental access with movable key.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const size_t      LENGTH = strlen(DATA[ti].d_results_p);
            const TestValues  VALUES(DATA[ti].d_results_p);

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
                bslma::TestAllocator za("different", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

                bslma::TestAllocator *valAllocator = 0;
                switch (cfg) {
                  case 'a': {
                    // inserted key has same allocator
                    valAllocator = &oa;
                  } break;
                  case 'b': {
                    // inserted key has different allocator
                    valAllocator = &za;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }
                bslma::TestAllocator& sa = *valAllocator;

                if (veryVeryVerbose) { T_ P_(LINE) P_(SPEC) P(LENGTH); }

                MoveState::Enum mStateKey;

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    const KEY&   K = VALUES[tj].first;
                    const VALUE& V = VALUES[tj].second;

                    bsls::ObjectBuffer<KEY> buffer;
                    KEY *keyptr = buffer.address();
                    TstFacility::emplace(keyptr,
                                         TstFacility::getIdentifier(K),
                                         &sa);
                    bslma::DestructorGuard<KEY> guard(keyptr);

                    bslma::TestAllocatorMonitor oam(&oa);

                    ASSERTV(LINE, V, V == mX[MoveUtil::move(*keyptr)]);

                    mStateKey = TstFacility::getMovedFromState(*keyptr);

                    ASSERTV(LINE, tj, mStateKey,
                            MoveState::e_UNKNOWN   == mStateKey
                         || MoveState::e_NOT_MOVED == mStateKey);

                    ASSERTV(LINE, tj, oam.isTotalSame());
                    ASSERTV(LINE, tj, oam.isInUseSame());

                    // Verify 'operator[]' can be used to set the value.

                    MoveState::Enum mStateValue;

                    {
                        bsls::ObjectBuffer<VALUE> valueBuffer;
                        VALUE *valueptr = valueBuffer.address();
                        TstFacility::emplace(valueptr,
                                             TstFacility::getIdentifier(ZV),
                                             &sa);
                        bslma::DestructorGuard<VALUE> guard(valueptr);

                        mX[MoveUtil::move(*keyptr)] =
                                                     MoveUtil::move(*valueptr);

                        mStateKey   = TstFacility::getMovedFromState(*keyptr);
                        mStateValue =
                                     TstFacility::getMovedFromState(*valueptr);
                    }

                    ASSERTV(LINE, tj, ZV == X.find(K)->second);
                    ASSERTV(LINE, tj, mStateKey,
                            MoveState::e_UNKNOWN   == mStateKey
                         || MoveState::e_NOT_MOVED == mStateKey);
                    ASSERTV(LINE, tj, mStateValue,
                            MoveState::e_UNKNOWN   == mStateValue
                         || MoveState::e_MOVED     == mStateValue);

                    {
                        bsls::ObjectBuffer<VALUE> valueBuffer;
                        VALUE *valueptr = valueBuffer.address();
                        TstFacility::emplace(valueptr,
                                             TstFacility::getIdentifier(V),
                                             &sa);
                        bslma::DestructorGuard<VALUE> guard(valueptr);

                        mX[MoveUtil::move(*keyptr)] =
                                                     MoveUtil::move(*valueptr);

                        mStateKey   = TstFacility::getMovedFromState(*keyptr);
                        mStateValue =
                                     TstFacility::getMovedFromState(*valueptr);
                    }

                    ASSERTV(LINE, tj, V == X.find(K)->second);
                    ASSERTV(LINE, tj, mStateKey,
                            MoveState::e_UNKNOWN   == mStateKey
                         || MoveState::e_NOT_MOVED == mStateKey);
                    ASSERTV(LINE, tj, mStateValue,
                            MoveState::e_UNKNOWN   == mStateValue
                         || MoveState::e_MOVED     == mStateValue);

                    ASSERTV(LINE, tj, oam.isInUseSame());
                }

                ASSERTV(LINE, 0 == da.numBlocksTotal());

#if defined(BDE_BUILD_TARGET_EXC)
                if (veryVeryVerbose)
                    printf("Test 'operator[]' on out-of-range "
                           "key with injected exceptions.\n");

                // Constructing default 'VALUE' to compare with.  Note, that we
                // construct default value this way to support some types that
                // do not meet C++ requirement of 'default-insertable'.
                bslma::TestAllocator value("value", veryVeryVeryVerbose);
                ALLOC                xvalue(&value);

                bsls::ObjectBuffer<VALUE> d;
                bsl::allocator_traits<ALLOC>::construct(xvalue, d.address());
                bslma::DestructorGuard<VALUE> defaultValueGuard(d.address());

                const VALUE& D = d.object();

                {
                    const size_t SIZE = X.size();

                    bslma::TestAllocator scratch("scratch",
                                                 veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        Obj mW(&scratch);        gg(&mW, SPEC);
                        ExceptionProctor<Obj> proctor(&X, L_,
                                                      MoveUtil::move(mW));

                        bsls::ObjectBuffer<KEY> buffer;
                        KEY *keyptr = buffer.address();
                        TstFacility::emplace(keyptr,
                                             TstFacility::getIdentifier(ZK),
                                             &sa);
                        bslma::DestructorGuard<KEY> keyGuard(keyptr);

                        mX[MoveUtil::move(*keyptr)];
                        mStateKey = TstFacility::getMovedFromState(*keyptr);

                        proctor.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE, SIZE, SIZE + 1 == X.size());
                    if (k_IS_VALUE_DEFAULT_CONSTRUCTIBLE) {
                        ASSERTV(LINE, NameOf<VALUE>(), D, X.find(ZK)->second,
                                D == X.find(ZK)->second);
                    }

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
                    ASSERTV(LINE, mStateKey,
                            MoveState::e_UNKNOWN == mStateKey
                         || MoveState::e_MOVED   == mStateKey);
#else
                    ASSERTV(LINE, mStateKey,
                            MoveState::e_UNKNOWN   == mStateKey
                         || MoveState::e_NOT_MOVED == mStateKey);
#endif  // BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES
                }
#endif
                ASSERTV(LINE, 0 == da.numBlocksInUse());
            }
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase33()
{
    // ------------------------------------------------------------------------
    // TESTING FUNCTIONS TAKING INITIALIZER LISTS
    //
    // Concerns:
    //: 1 The four functions that take an initializer list (two constructors,
    //:   an assignment operator, and the 'insert' function) simply forward to
    //:   another already tested function.
    //
    // TBD Missing concerns that the correct comparator is used.  We should be
    // testing with a stateful comparator (testing two states) and the default
    // comparator.  A (stateful) comparator that simply holds an ID would be
    // good enough.  (also test cases 2 & 12).
    //
    // Plan:
    //: 1 TBD Note that we are interested here only in ensuring that the
    //:   forwarding is working -- not retesting already verified
    //:   functionality.
    //
    // Testing:
    //   map(initializer_list<value_type>, const C& comp, const A& allocator);
    //   map(initializer_list<value_type>, const A& allocator);
    //   map& operator=(initializer_list<value_type>);
    //   void insert(initializer_list<value_type>);
    // -----------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)

    const TestValues V;

    if (verbose) printf("\nTesting constructors with initializer lists.\n");
    {
        const struct {
            int                               d_line;      // source line #
            std::initializer_list<ValueType>  d_list;      // source list
            const char                       *d_result_p;  // expected result
        } DATA[] = {
                //line          list             result
                //----          ----             ------
                { L_,   {                  },        ""   },
                { L_,   { V[0]             },       "A"   },
                { L_,   { V[0], V[0]       },       "A"   },
                { L_,   { V[1], V[0]       },      "AB"   },
                { L_,   { V[0], V[1], V[2] },     "ABC"   },
                { L_,   { V[0], V[1], V[0] },      "AB"   },
        };
        const int NUM_SPECS =
                            static_cast<const int>(sizeof DATA / sizeof *DATA);

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);
        bslma::TestAllocatorMonitor  dam(&da);

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            Obj mY(&scratch);  const Obj& Y = gg(&mY, DATA[ti].d_result_p);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj mX(DATA[ti].d_list, COMP(), &oa);  const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&oa == X.get_allocator());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(&scratch == Y.get_allocator());
        }
        ASSERT(dam.isTotalSame());
    }

    {
        const struct {
            int                               d_line;      // source line #
            std::initializer_list<ValueType>  d_list;      // source list
            const char                       *d_result_p;  // expected result
        } DATA[] = {
                //line          list             result
                //----          ----             ------
                { L_,   {                  },        ""   },
                { L_,   { V[0]             },       "A"   },
                { L_,   { V[0], V[0]       },       "A"   },
                { L_,   { V[1], V[0]       },      "AB"   },
                { L_,   { V[0], V[1], V[2] },     "ABC"   },
                { L_,   { V[0], V[1], V[0] },      "AB"   },
        };
        const int NUM_SPECS =
                            static_cast<const int>(sizeof DATA / sizeof *DATA);

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            ASSERT(0 == da.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            Obj mY(&scratch);  const Obj& Y = gg(&mY, DATA[ti].d_result_p);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(da) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj mX(DATA[ti].d_list, COMP());  const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&da == X.get_allocator());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(&scratch == Y.get_allocator());
        }
        ASSERT(0 == da.numBytesInUse());
    }

    {
        const struct {
            int                               d_line;      // source line #
            std::initializer_list<ValueType>  d_list;      // source list
            const char                       *d_result_p;  // expected result
        } DATA[] = {
                //line          list             result
                //----          ----             ------
                { L_,   {                  },        ""   },
                { L_,   { V[0]             },       "A"   },
                { L_,   { V[0], V[0]       },       "A"   },
                { L_,   { V[1], V[0]       },      "AB"   },
                { L_,   { V[0], V[1], V[2] },     "ABC"   },
                { L_,   { V[0], V[1], V[0] },      "AB"   },
        };
        const int NUM_SPECS =
                            static_cast<const int>(sizeof DATA / sizeof *DATA);

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);
        bslma::TestAllocatorMonitor  dam(&da);

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            Obj mY(&scratch);  const Obj& Y = gg(&mY, DATA[ti].d_result_p);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj mX(DATA[ti].d_list, &oa);  const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&oa == X.get_allocator());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(&scratch == Y.get_allocator());
        }
        ASSERT(dam.isTotalSame());
    }

    {
        const struct {
            int                               d_line;      // source line #
            std::initializer_list<ValueType>  d_list;      // source list
            const char                       *d_result_p;  // expected result
        } DATA[] = {
                //line          list             result
                //----          ----             ------
                { L_,   {                  },        ""   },
                { L_,   { V[0]             },       "A"   },
                { L_,   { V[0], V[0]       },       "A"   },
                { L_,   { V[1], V[0]       },      "AB"   },
                { L_,   { V[0], V[1], V[2] },     "ABC"   },
                { L_,   { V[0], V[1], V[0] },      "AB"   },
        };
        const int NUM_SPECS =
                            static_cast<const int>(sizeof DATA / sizeof *DATA);

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);
        bslma::TestAllocatorMonitor  dam(&da);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            Obj mY(&scratch);  const Obj& Y = gg(&mY, DATA[ti].d_result_p);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(da) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj mX = DATA[ti].d_list;  const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&da == X.get_allocator());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(&scratch == Y.get_allocator());
        }
        ASSERT(dam.isInUseSame());
    }

    if (verbose) printf("\nTesting 'operator=' with initializer lists.\n");
    {
        const struct {
            int                               d_line;      // source line #
            const char                       *d_spec_p;    // target string
            std::initializer_list<ValueType>  d_list;      // source list
            const char                       *d_result_p;  // expected result
        } DATA[] = {
                //line  lhs            list                result
                //----  ------         ----                ------
                { L_,   "",       {                  },        ""   },
                { L_,   "",       { V[0]             },       "A"   },
                { L_,   "A",      {                  },        ""   },
                { L_,   "A",      { V[1]             },       "B"   },
                { L_,   "A",      { V[0], V[1]       },      "AB"   },
                { L_,   "A",      { V[1], V[2]       },      "BC"   },
                { L_,   "AB",     {                  },        ""   },
                { L_,   "AB",     { V[0], V[1], V[2] },     "ABC"   },
                { L_,   "AB",     { V[2], V[3], V[4] },     "CDE"   },
        };
        const int NUM_SPECS =
                            static_cast<const int>(sizeof DATA / sizeof *DATA);

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);
        bslma::TestAllocatorMonitor  dam(&da);

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            Obj mY(&scratch);  const Obj& Y = gg(&mY, DATA[ti].d_result_p);

            Obj mX(&oa);  const Obj& X = gg(&mX, DATA[ti].d_spec_p);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj *mR = &(mX = DATA[ti].d_list);
                ASSERTV(mR, &mX, mR == &mX);
                ASSERTV(Y,    X,    Y == X);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        ASSERT(dam.isTotalSame());
    }

    if (verbose) printf("\nTesting 'insert' with initializer lists.\n");
    {
        const struct {
            int                               d_line;      // source line #
            const char                       *d_spec_p;    // target string
            std::initializer_list<ValueType>  d_list;      // source list
            const char                       *d_result_p;  // expected result
        } DATA[] = {
                //line  source         list                result
                //----  ------         ----                ------
                { L_,   "",       {                  },        ""   },
                { L_,   "",       { V[0]             },       "A"   },
                { L_,   "A",      {                  },       "A"   },
                { L_,   "A",      { V[0]             },       "A"   },
                { L_,   "A",      { V[1]             },      "AB"   },
                { L_,   "AB",     { V[0], V[1]       },      "AB"   },
                { L_,   "AB",     { V[1], V[2]       },     "ABC"   },
                { L_,   "AB",     { V[0], V[1], V[2] },     "ABC"   },
                { L_,   "AB",     { V[2], V[3], V[4] },   "ABCDE"   },
        };
        const int NUM_SPECS =
                            static_cast<const int>(sizeof DATA / sizeof *DATA);

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);
        bslma::TestAllocatorMonitor  dam(&da);

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {

            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            Obj mY(&scratch);  const Obj& Y = gg(&mY, DATA[ti].d_result_p);

            Obj mX(&oa);  const Obj& X = gg(&mX, DATA[ti].d_spec_p);

            mX.insert(DATA[ti].d_list);

            ASSERTV(X, Y, X == Y);
        }
        ASSERT(dam.isTotalSame());
    }
#endif
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase32a()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH EMPLACE WITH HINT
    //
    // Concerns:
    //: 1 'emplace_hint' correctly forwards arguments, including the object
    //:   allocator, to the piecewise constructor of the ('pair') value type,
    //:   up to 3 arguments each for the 'KEY' and 'VALUE' parts of the pair.
    //:   Note that only the forwarding of arguments is tested in this
    //:   function; all other functionality is tested in 'testCase32'.  Also
    //:   note that at most 3 arguments are used for each part of the pair as
    //:   we assume that 'bslstl_pair' tests piecewise construction with up to
    //:   10 arguments for each part (10 being the maximum number of arguments
    //:   supported for C++03 compatibility).
    //:
    //: 2 'emplace_hint' is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 This test makes material use of template method 'testCase32a_RunTest'
    //:   where the first and fifth integer template parameters indicate the
    //:   number of arguments to use for the 'KEY' and 'VALUE' parts,
    //:   respectively, the other 6 integer template parameters (3 each for
    //:   'KEY' and 'VALUE') indicating '0' for copy, '1' for move, and '2' for
    //:   not-applicable (i.e., beyond the number of arguments being tested),
    //:   and taking as arguments an address providing modifiable
    //:   access to a container and a hint.
    //:
    //:   1 Create 3 argument values for the 'KEY' part and 3 argument values
    //:     for the 'VALUE' part, each with their own argument-specific
    //:     allocator.
    //:
    //:   2 Based on the first and fifth template parameters indicating the
    //:     number of arguments to pass in for the 'KEY' and 'VALUE' parts,
    //:     respectively, call 'emplace_hint' with the argument values
    //:     corresponding to the other six template parameters, performing an
    //:     explicit move of each argument, or copy, as indicated by the
    //:     template parameter corresponding to the argument, all in the
    //:     presence of injected exceptions.
    //:
    //:   3 Verify that the argument values were passed correctly.
    //:
    //:   4 Verify that any memory that is allocated is from the object
    //:     allocator, indicating that the allocator was forwarded correctly.
    //:
    //:   5 Verify that the move-state for each argument is as expected.
    //:
    //:   6 If the object did not contain the emplaced value, verify it now
    //:     exists.
    //:
    //:   7 Verify the return value is as expected.
    //:
    //:   8 Return the iterator returned by the call to 'emplace_hint'.
    //:
    //: 2 Create a container with its own object-specific allocator.
    //:
    //: 3 Call 'testCase32a_RunTest' in various configurations:
    //:   1 For 1..3 arguments each for the 'KEY' and 'VALUE' parts, call with
    //:     the move flag set to '1' and then with the move flag set to '0'.
    //:
    //:   2 For 1..3 arguments each for the 'KEY' and 'VALUE' parts, call with
    //:     move flags set to '0', '1', and each move flag set independently.
    //
    // Testing:
    //   iterator emplace_hint(const_iterator hint, Args&&... args);
    // ------------------------------------------------------------------------

// TBD There is an innate complexity to this test that masks some concerns that
// I think are mostly tested, but could do with being walked through with more
// local comments in the case itself.
//
// (1) I /think/ we start out by demonstrating that we can emplace multiple
// items and grow the map - although I don't see duplicate keys being tested,
// and certainly no 'ABAC' equivalent test.
//
// (2) That is followed by a more exhaustive test of forwarding each argument,
// where each test scenario emplaces a single element into an empty map, and
// then confirms that a duplicate key with a different value will not emplace.
//
// My gut is that there is some overlap set of concerns addressed by (1) and
// (2) that are not being tested.
//
// At no point do we test that multiple emplaced values result in the correct
// sequence.
//
// I don't see us testing that emplace after an erase or clear does not
// allocate a new node (as it re-uses one from the pool).
//
// (This comment also applies to 'testCase31a'.)

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)

    Iter hint;

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
    if (verbose) printf("\nTesting emplace 1..3 args, move=1"
                        "\n---------------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,0,2,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,1,2,2,0,2,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<2,1,1,2,0,2,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<3,1,1,1,0,2,2,2>(&mX, hint, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,1,1,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,1,2,2,1,1,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<2,1,1,2,1,1,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<3,1,1,1,1,1,2,2>(&mX, hint, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,2,1,1,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,1,2,2,2,1,1,2>(&mX, hint, true);
            hint = testCase32a_RunTest<2,1,1,2,2,1,1,2>(&mX, hint, true);
            hint = testCase32a_RunTest<3,1,1,1,2,1,1,2>(&mX, hint, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,3,1,1,1>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,1,2,2,3,1,1,1>(&mX, hint, true);
            hint = testCase32a_RunTest<2,1,1,2,3,1,1,1>(&mX, hint, true);
            hint = testCase32a_RunTest<3,1,1,1,3,1,1,1>(&mX, hint, true);
        }
    }

    if (verbose) printf("\nTesting emplace 1..3 args, move=0"
                        "\n---------------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,0,2,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,0,2,2,0,2,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<2,0,0,2,0,2,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<3,0,0,0,0,2,2,2>(&mX, hint, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,1,0,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,0,2,2,1,0,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<2,0,0,2,1,0,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<3,0,0,0,1,0,2,2>(&mX, hint, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,2,0,0,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,0,2,2,2,0,0,2>(&mX, hint, true);
            hint = testCase32a_RunTest<2,0,0,2,2,0,0,2>(&mX, hint, true);
            hint = testCase32a_RunTest<3,0,0,0,2,0,0,2>(&mX, hint, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,3,0,0,0>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,0,2,2,3,0,0,0>(&mX, hint, true);
            hint = testCase32a_RunTest<2,0,0,2,3,0,0,0>(&mX, hint, true);
            hint = testCase32a_RunTest<3,0,0,0,3,0,0,0>(&mX, hint, true);
        }
    }

    if (verbose) printf("\nTesting emplace with 0 args"
                        "\n---------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);  const Obj& X = mX;

        hint = testCase32a_RunTest<0,2,2,2,0,2,2,2>(&mX, X.end(), true);
    }

    if (verbose) printf("\nTesting emplace with 1 arg"
                        "\n--------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,1,0,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<0,2,2,2,1,1,2,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<1,0,2,2,0,2,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,1,2,2,0,2,2,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<1,0,2,2,1,0,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,1,2,2,1,0,2,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<1,0,2,2,1,1,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,1,2,2,1,1,2,2>(&mX, hint, false);
        }
    }

    if (verbose) printf("\nTesting emplace with 2 args"
                        "\n---------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,2,0,0,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<0,2,2,2,2,0,1,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,2,1,0,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<0,2,2,2,2,1,1,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,0,0,2,0,2,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,0,1,2,0,2,2,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,1,0,2,0,2,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,1,1,2,0,2,2,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,0,0,2,2,0,0,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,0,1,2,2,0,0,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,1,0,2,2,0,0,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,1,1,2,2,0,0,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,0,0,2,2,0,1,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,0,1,2,2,0,1,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,1,0,2,2,0,1,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,1,1,2,2,0,1,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,0,0,2,2,1,0,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,0,1,2,2,1,0,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,1,0,2,2,1,0,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,1,1,2,2,1,0,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,0,0,2,2,1,1,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,0,1,2,2,1,1,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,1,0,2,2,1,1,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,1,1,2,2,1,1,2>(&mX, hint, false);
        }
    }

    if (verbose) printf("\nTesting emplace with 3 args"
                        "\n---------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<3,1,0,0,3,1,0,0>(&mX, X.end(), true);
            hint = testCase32a_RunTest<3,0,1,1,3,0,1,1>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<3,0,1,0,3,0,1,0>(&mX, X.end(), true);
            hint = testCase32a_RunTest<3,1,0,1,3,1,0,1>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<3,0,0,1,3,0,0,1>(&mX, X.end(), true);
            hint = testCase32a_RunTest<3,1,1,0,3,0,1,1>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<3,0,1,1,3,0,1,1>(&mX, X.end(), true);
            hint = testCase32a_RunTest<3,1,0,0,3,1,0,0>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<3,1,0,1,3,1,0,1>(&mX, X.end(), true);
            hint = testCase32a_RunTest<3,0,1,0,3,0,1,0>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<3,1,1,0,3,1,1,0>(&mX, X.end(), true);
            hint = testCase32a_RunTest<3,0,0,1,3,1,0,0>(&mX, hint, false);
        }
    }
#else
    if (verbose) printf("\nTesting emplace 0..3 args, move=0"
                        "\n---------------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,0,2,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,0,2,2,0,2,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<2,0,0,2,0,2,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<3,0,0,0,0,2,2,2>(&mX, hint, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,1,0,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,0,2,2,1,0,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<2,0,0,2,1,0,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<3,0,0,0,1,0,2,2>(&mX, hint, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,2,0,0,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,0,2,2,2,0,0,2>(&mX, hint, true);
            hint = testCase32a_RunTest<2,0,0,2,2,0,0,2>(&mX, hint, true);
            hint = testCase32a_RunTest<3,0,0,0,2,0,0,2>(&mX, hint, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,3,0,0,0>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,0,2,2,3,0,0,0>(&mX, hint, true);
            hint = testCase32a_RunTest<2,0,0,2,3,0,0,0>(&mX, hint, true);
            hint = testCase32a_RunTest<3,0,0,0,3,0,0,0>(&mX, hint, true);
        }
    }
#endif
#endif
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase32()
{
    // ------------------------------------------------------------------------
    // TESTING EMPLACE WITH HINT
    //
    // Concerns:
    //: 1 'emplace_hint' returns an iterator referring to the newly inserted
    //:   element if it did not already exists, and to the existing element if
    //:   it did.
    //:
    //: 2 A new element is added to the container if the element did not
    //:   already exist, and the order of the container remains correct.
    //:
    //: 3 Inserting with the correct hint places the new element right before
    //:   the hint.
    //:
    //: 4 Inserting with the correct hint requires no more than 2 comparisons.
    //:
    //: 5 Incorrect hint will be ignored and 'insert' will proceed as if the
    //:   hint is not supplied.
    //:
    //: 6 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 7 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 We will use 'value' as the single argument to the 'emplace_hint'
    //:   function and will test proper forwarding of constructor arguments in
    //:   test 'testCase32a'.
    //:
    //: 2 For insertion we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 For each set of values, set hint to be 'lower_bound', 'begin',
    //:     'begin' + 1, 'end' - 1, 'end'
    //:
    //:     1 For each value in the set, 'insert' the value with hint.
    //:
    //:       1 Compute the number of allocations and verify it is as
    //:         expected.
    //:
    //:       2 Verify the return value and the resulting data in the container
    //:         is as expected.
    //:
    //:       3 Verify the new element is inserted right before the hint if
    //:         the hint is valid.  (C-3)
    //:
    //:       4 Verify the number of comparisons is no more than 2 if the hint
    //:         is valid.  (C-4)
    //:
    //:       5 Verify all allocations are from the object's allocator.  (C-6)
    //:
    //: 3 Repeat P-2 under the presence of injected exceptions.  (C-7)
    //
    // Testing:
    //   iterator emplace_hint(const_iterator position, Args&&... args);
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value +
                           bslma::UsesBslmaAllocator<VALUE>::value;

    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);

    static const struct {
        int         d_line;      // source line number
        const char *d_spec_p;    // specification string
        const char *d_unique_p;  // expected element values
        const char *d_allocs_p;  // expected pool resizes
    } DATA[] = {
        //line  spec           isUnique       poolAlloc
        //----  ----           --------       ---------

        { L_,   "A",           "Y",           "+"           },
        { L_,   "AAA",         "YNN",         "++-"         },
        { L_,   "ABCDEFGH",    "YYYYYYYY",    "++-+---+"    },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY", "++-+-------" },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN",  "++---+----"  }
    };
    const int NUM_DATA = static_cast<const int>(sizeof DATA / sizeof *DATA);

    const int MAX_LENGTH = 10;

    if (verbose) printf("\nTesting 'emplace' with hint.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const char *const UNIQUE = DATA[ti].d_unique_p;
            const char *const ALLOCS = DATA[ti].d_allocs_p;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                Obj mX(&oa);  const Obj &X = mX;

                for (int tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                    if (IS_UNIQ) {
                        EXPECTED[SIZE] = SPEC[tj];
                        std::sort(EXPECTED, EXPECTED + SIZE + 1);
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { P(EXPECTED); }
                    }

                    CIter hint;

                    switch (CONFIG) {
                      case 'a': {
                        hint = X.lower_bound(VALUES[tj].first);
                      } break;
                      case 'b': {
                        hint = X.begin();
                      } break;
                      case 'c': {
                        hint = X.begin();
                        if (hint != X.end()) {
                            ++hint;
                        }
                      } break;
                      case 'd': {
                        hint = X.end();
                        if (hint != X.begin()) {
                            --hint;
                        }
                      } break;
                      case 'e': {
                        hint = X.end();
                      } break;
                      default: {
                        ASSERTV(!"Unexpected configuration");
                      } return;                                       // RETURN
                    }

                    size_t EXP_COMP = X.key_comp().count();
                    if ('a' == CONFIG) {
                        if (!IS_UNIQ) {
                            EXP_COMP += 2;
                        }
                        else {
                            if (hint != X.begin()) {
                                ++EXP_COMP;
                            }
                            if (hint != X.end()) {
                                ++EXP_COMP;
                            }
                        }
                    }

                    const bsls::Types::Int64 BB = oa.numBlocksTotal();
                    const bsls::Types::Int64 B  = oa.numBlocksInUse();

                    Iter RESULT = mX.emplace_hint(hint, VALUES[tj]);

                    const bsls::Types::Int64 AA = oa.numBlocksTotal();
                    const bsls::Types::Int64 A  = oa.numBlocksInUse();

                    ASSERTV(LINE, CONFIG, tj, SIZE,
                            VALUES[tj] == *RESULT);

                    if ('a' == CONFIG) {
                        ASSERTV(LINE, tj, EXP_COMP, X.key_comp().count(),
                                EXP_COMP == X.key_comp().count());

                        if (IS_UNIQ) {
                            ASSERTV(LINE, tj, hint == ++RESULT);
                        }
                        else {
                            ASSERTV(LINE, tj, hint == RESULT);
                        }
                    }

                    if (IS_UNIQ) {
                        if ('+' == ALLOCS[tj]) {
                            ASSERTV(LINE, tj, AA, BB,
                                    BB + 1 + TYPE_ALLOC == AA);
                            ASSERTV(LINE, tj, A, B,
                                    B + 1 + TYPE_ALLOC == A);
                        }
                        else {
                            ASSERTV(LINE, tj, AA, BB,
                                    BB + 0 + TYPE_ALLOC == AA);
                            ASSERTV(LINE, tj, A, B, B + 0 + TYPE_ALLOC == A);
                        }
                        ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                        TestValues exp(EXPECTED);

                        ASSERTV(LINE, tj,
                                0 == verifyContainer(X, exp, SIZE + 1));
                    }
                    else {
                        if ('+' == ALLOCS[tj]) {
                            ASSERTV(LINE, tj, AA, BB,
                                    BB + 1 + TYPE_ALLOC == AA);
                            ASSERTV(LINE, tj,  A,  B,
                                     B + 1              ==  A);
                        }
                        else {
                            ASSERTV(LINE, tj, AA, BB,
                                    BB + 0 + TYPE_ALLOC == AA);
                            ASSERTV(LINE, tj,  A,  B,
                                     B + 0              ==  A);
                        }
                        ASSERTV(LINE, tj, SIZE == X.size());

                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE));
                    }
                }
            }
        }
    }
    if (verbose) printf("\nTesting 'emplace_hint' with exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const char *const UNIQUE = DATA[ti].d_unique_p;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                Obj mX(&oa);  const Obj &X = mX;

                for (int tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                    if (IS_UNIQ) {
                        EXPECTED[SIZE] = SPEC[tj];
                        std::sort(EXPECTED, EXPECTED + SIZE + 1);
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { P(EXPECTED); }
                    }

                    CIter hint;

                    switch (CONFIG) {
                      case 'a': {
                        hint = X.lower_bound(VALUES[tj].first);
                      } break;
                      case 'b': {
                        hint = X.begin();
                      } break;
                      case 'c': {
                        hint = X.begin();
                        if (hint != X.end()) {
                            ++hint;
                        }
                      } break;
                      case 'd': {
                        hint = X.end();
                        if (hint != X.begin()) {
                            --hint;
                        }
                      } break;
                      case 'e': {
                        hint = X.end();
                      } break;
                      default: {
                        ASSERTV(!"Unexpected configuration");
                      } return;                                       // RETURN
                    }

                    bslma::TestAllocator scratch("scratch",
                                                 veryVeryVeryVerbose);
                    Iter RESULT;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ExceptionProctor<Obj> proctor(&X, L_, &scratch);

                        RESULT = mX.emplace_hint(hint, VALUES[tj]);

                        proctor.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE, CONFIG, tj, SIZE, VALUES[tj] == *RESULT);

                    if (IS_UNIQ) {
                        ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, tj,
                                0 == verifyContainer(X, exp, SIZE + 1));
                    }
                    else {
                        ASSERTV(LINE, tj, SIZE == X.size());

                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE));
                    }
                }
            }
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase31a()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH EMPLACE
    //
    // Concerns:
    //: 1 'emplace' correctly forwards arguments, including the object
    //:   allocator, to the piecewise constructor of the ('pair') value type,
    //:   up to 3 arguments each for the 'KEY' and 'VALUE' parts of the pair.
    //:   Note that only the forwarding of arguments is tested in this
    //:   function; all other functionality is tested in 'testCase31'.  Also
    //:   note that at most 3 arguments are used for each part of the pair as
    //:   we assume that 'bslstl_pair' tests piecewise construction with up to
    //:   10 arguments for each part (10 being the maximum number of arguments
    //:   supported for C++03 compatibility).
    //:
    //: 2 'emplace' is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 This test makes material use of template method 'testCase31a_RunTest'
    //:   where the first and fifth integer template parameters indicate the
    //:   number of arguments to use for the 'KEY' and 'VALUE' parts,
    //:   respectively, the other 6 integer template parameters (3 each for
    //:   'KEY' and 'VALUE') indicating '0' for copy, '1' for move, and '2' for
    //:   not-applicable (i.e., beyond the number of arguments being tested),
    //:   and taking as the only argument an address providing modifiable
    //:   access to a container.
    //:
    //:   1 Create 3 argument values for the 'KEY' part and 3 argument values
    //:     for the 'VALUE' part, each with their own argument-specific
    //:     allocator.
    //:
    //:   2 Based on the first and fifth template parameters indicating the
    //:     number of arguments to pass in for the 'KEY' and 'VALUE' parts,
    //:     respectively, call 'emplace' with the argument values corresponding
    //:     to the other six template parameters, performing an explicit move
    //:     of each argument, or copy, as indicated by the template parameter
    //:     corresponding to the argument, all in the presence of injected
    //:     exceptions.
    //:
    //:   3 Verify that the argument values were passed correctly.
    //:
    //:   4 Verify that any memory that is allocated is from the object
    //:     allocator, indicating that the allocator was forwarded correctly.
    //:
    //:   5 Verify that the move-state for each argument is as expected.
    //:
    //:   6 If the object did not contain the emplaced value, verify it now
    //:     exists.
    //:
    //:   7 Verify the return value is as expected.
    //:
    //: 2 Create a container with its own object-specific allocator.
    //:
    //: 3 Call 'testCase31a_RunTest' in various configurations:
    //:   1 For 1..3 arguments each for the 'KEY' and 'VALUE' parts, call with
    //:     the move flag set to '1' and then with the move flag set to '0'.
    //:
    //:   2 For 1..3 arguments each for the 'KEY' and 'VALUE' parts, call with
    //:     move flags set to '0', '1', and each move flag set independently.
    //
    // Testing:
    //   iterator emplace(Args&&... args);
    // ------------------------------------------------------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
    if (verbose) printf("\nTesting emplace 1..3 args, move=1"
                        "\n---------------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<1,1,2,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<2,1,1,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<3,1,1,1,0,2,2,2>(&mX, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,1,1,2,2>(&mX, true);
            testCase31a_RunTest<1,1,2,2,1,1,2,2>(&mX, true);
            testCase31a_RunTest<2,1,1,2,1,1,2,2>(&mX, true);
            testCase31a_RunTest<3,1,1,1,1,1,2,2>(&mX, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,2,1,1,2>(&mX, true);
            testCase31a_RunTest<1,1,2,2,2,1,1,2>(&mX, true);
            testCase31a_RunTest<2,1,1,2,2,1,1,2>(&mX, true);
            testCase31a_RunTest<3,1,1,1,2,1,1,2>(&mX, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,3,1,1,1>(&mX, true);
            testCase31a_RunTest<1,1,2,2,3,1,1,1>(&mX, true);
            testCase31a_RunTest<2,1,1,2,3,1,1,1>(&mX, true);
            testCase31a_RunTest<3,1,1,1,3,1,1,1>(&mX, true);
        }
    }

    if (verbose) printf("\nTesting emplace 1..3 args, move=0"
                        "\n---------------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<1,0,2,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<2,0,0,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<3,0,0,0,0,2,2,2>(&mX, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,1,0,2,2>(&mX, true);
            testCase31a_RunTest<1,0,2,2,1,0,2,2>(&mX, true);
            testCase31a_RunTest<2,0,0,2,1,0,2,2>(&mX, true);
            testCase31a_RunTest<3,0,0,0,1,0,2,2>(&mX, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,2,0,0,2>(&mX, true);
            testCase31a_RunTest<1,0,2,2,2,0,0,2>(&mX, true);
            testCase31a_RunTest<2,0,0,2,2,0,0,2>(&mX, true);
            testCase31a_RunTest<3,0,0,0,2,0,0,2>(&mX, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,3,0,0,0>(&mX, true);
            testCase31a_RunTest<1,0,2,2,3,0,0,0>(&mX, true);
            testCase31a_RunTest<2,0,0,2,3,0,0,0>(&mX, true);
            testCase31a_RunTest<3,0,0,0,3,0,0,0>(&mX, true);
        }
    }

    if (verbose) printf("\nTesting emplace with 0 args"
                        "\n---------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);

        testCase31a_RunTest<0,2,2,2,0,2,2,2>(&mX, true);
    }

    if (verbose) printf("\nTesting emplace with 1 arg"
                        "\n--------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,1,0,2,2>(&mX, true);
            testCase31a_RunTest<0,2,2,2,1,1,2,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<1,0,2,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<1,1,2,2,0,2,2,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<1,0,2,2,1,0,2,2>(&mX, true);
            testCase31a_RunTest<1,1,2,2,1,0,2,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<1,0,2,2,1,1,2,2>(&mX, true);
            testCase31a_RunTest<1,1,2,2,1,1,2,2>(&mX, false);
        }
    }

    if (verbose) printf("\nTesting emplace with 2 args"
                        "\n---------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,2,0,0,2>(&mX, true);
            testCase31a_RunTest<0,2,2,2,2,0,1,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,2,1,0,2>(&mX, true);
            testCase31a_RunTest<0,2,2,2,2,1,1,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,0,0,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<2,0,1,2,0,2,2,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,1,0,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<2,1,1,2,0,2,2,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,0,0,2,2,0,0,2>(&mX, true);
            testCase31a_RunTest<2,0,1,2,2,0,0,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,1,0,2,2,0,0,2>(&mX, true);
            testCase31a_RunTest<2,1,1,2,2,0,0,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,0,0,2,2,0,1,2>(&mX, true);
            testCase31a_RunTest<2,0,1,2,2,0,1,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,1,0,2,2,0,1,2>(&mX, true);
            testCase31a_RunTest<2,1,1,2,2,0,1,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,0,0,2,2,1,0,2>(&mX, true);
            testCase31a_RunTest<2,0,1,2,2,1,0,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,1,0,2,2,1,0,2>(&mX, true);
            testCase31a_RunTest<2,1,1,2,2,1,0,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,0,0,2,2,1,1,2>(&mX, true);
            testCase31a_RunTest<2,0,1,2,2,1,1,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,1,0,2,2,1,1,2>(&mX, true);
            testCase31a_RunTest<2,1,1,2,2,1,1,2>(&mX, false);
        }
    }

    if (verbose) printf("\nTesting emplace with 3 args"
                        "\n---------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<3,1,0,0,3,1,0,0>(&mX, true);
            testCase31a_RunTest<3,0,1,1,3,0,1,1>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<3,0,1,0,3,0,1,0>(&mX, true);
            testCase31a_RunTest<3,1,0,1,3,1,0,1>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<3,0,0,1,3,0,0,1>(&mX, true);
            testCase31a_RunTest<3,1,1,0,3,0,1,1>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<3,0,1,1,3,0,1,1>(&mX, true);
            testCase31a_RunTest<3,1,0,0,3,1,0,0>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<3,1,0,1,3,1,0,1>(&mX, true);
            testCase31a_RunTest<3,0,1,0,3,0,1,0>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<3,1,1,0,3,1,1,0>(&mX, true);
            testCase31a_RunTest<3,0,0,1,3,1,0,0>(&mX, false);
        }
    }
#else
    if (verbose) printf("\nTesting emplace 0..3 args, move=0"
                        "\n---------------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<1,0,2,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<2,0,0,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<3,0,0,0,0,2,2,2>(&mX, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,1,0,2,2>(&mX, true);
            testCase31a_RunTest<1,0,2,2,1,0,2,2>(&mX, true);
            testCase31a_RunTest<2,0,0,2,1,0,2,2>(&mX, true);
            testCase31a_RunTest<3,0,0,0,1,0,2,2>(&mX, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,2,0,0,2>(&mX, true);
            testCase31a_RunTest<1,0,2,2,2,0,0,2>(&mX, true);
            testCase31a_RunTest<2,0,0,2,2,0,0,2>(&mX, true);
            testCase31a_RunTest<3,0,0,0,2,0,0,2>(&mX, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,3,0,0,0>(&mX, true);
            testCase31a_RunTest<1,0,2,2,3,0,0,0>(&mX, true);
            testCase31a_RunTest<2,0,0,2,3,0,0,0>(&mX, true);
            testCase31a_RunTest<3,0,0,0,3,0,0,0>(&mX, true);
        }
    }
#endif
#endif
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase31()
{
    // ------------------------------------------------------------------------
    // TESTING EMPLACE
    //
    // Concerns:
    //: 1 'emplace' returns a pair containing an iterator and a 'bool'
    //:
    //: 2 The iterator returned refers to the newly inserted element if it did
    //:   not already exists, and to the existing element if it did.
    //:
    //: 3 The 'bool' returned is 'true' if a new element is inserted, and
    //:   'false' otherwise.
    //:
    //: 4 A new element is added to the container if the element did not
    //:   already exist, and the order of the container remains correct.
    //:
    //: 5 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 6 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 We will use 'value' as the single argument to the 'emplace' function
    //:   and will test proper forwarding of constructor arguments in test
    //:   'testCase31a'.
    //:
    //: 2 For emplace, we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 Compute the number of allocations and verify it is as expected.
    //:
    //:   2 If the object did not contain 'value', verify it now exist.
    //:     Otherwise, verify the return value is as expected.  (C-1..4)
    //:
    //:   3 Verify all allocations are from the object's allocator.  (C-5)
    //:
    //: 3 Repeat P-2 under the presence of injected exceptions.  (C-6)
    //
    // Testing:
    //   pair<iterator, bool> emplace(Args&&... arguments);
    // -----------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value +
                           bslma::UsesBslmaAllocator<VALUE>::value;

    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);

    static const struct {
        int         d_line;      // source line number
        const char *d_spec_p;    // specification string
        const char *d_unique_p;  // expected element values
        const char *d_allocs_p;  // expected pool resizes
    } DATA[] = {
        //line  spec           isUnique       poolAlloc
        //----  ----           --------       ---------

        { L_,   "A",           "Y",           "+"           },
        { L_,   "AAA",         "YNN",         "++-"         },
        { L_,   "ABCDEFGH",    "YYYYYYYY",    "++-+---+"    },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY", "++-+-------" },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN",  "++---+----"  }
    };
    const int NUM_DATA = static_cast<const int>(sizeof DATA / sizeof *DATA);

    const int MAX_LENGTH = 10;

    if (verbose) printf("\nTesting 'emplace' without exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const char *const UNIQUE = DATA[ti].d_unique_p;
            const char *const ALLOCS = DATA[ti].d_allocs_p;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj &X = mX;

            for (int tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                const size_t SIZE    = X.size();

                if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }
                }

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                pair<Iter, bool> RESULT = mX.emplace(VALUES[tj]);

                ASSERTV(LINE, tj, SIZE, IS_UNIQ    == RESULT.second);
                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *(RESULT.first));

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                if (IS_UNIQ) {
                    if ('+' == ALLOCS[tj]) {
                        ASSERTV(LINE, tj, AA, BB, BB + 1 + TYPE_ALLOC == AA);
                        ASSERTV(LINE, tj, A, B, B + 1 + TYPE_ALLOC == A);
                    }
                    else {
                        ASSERTV(LINE, tj, AA, BB, BB + 0 + TYPE_ALLOC == AA);
                        ASSERTV(LINE, tj, A, B, B + 0 + TYPE_ALLOC == A);
                    }
                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE + 1));
                }
                else {
                    if ('+' == ALLOCS[tj]) {
                        ASSERTV(LINE, tj, AA, BB, BB + 1 + TYPE_ALLOC == AA);
                        ASSERTV(LINE, tj,  A,  B,  B + 1              ==  A);
                    }
                    else {
                        ASSERTV(LINE, tj, AA, BB, BB + 0 + TYPE_ALLOC == AA);
                        ASSERTV(LINE, tj,  A,  B,  B + 0              ==  A);
                    }
                    ASSERTV(LINE, tj, SIZE == X.size());

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE));
                }
            }
        }
    }
    if (verbose) printf("\nTesting 'emplace' with injected exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const char *const UNIQUE = DATA[ti].d_unique_p;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj &X = mX;

            for (int tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                const size_t SIZE    = X.size();

                if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionProctor<Obj> proctor(&X, L_, &scratch);

                    pair<Iter, bool> RESULT = mX.emplace(VALUES[tj]);

                    proctor.release();

                    ASSERTV(LINE, tj, SIZE, IS_UNIQ == RESULT.second);
                    ASSERTV(LINE, tj, SIZE, VALUES[tj] == *(RESULT.first));
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                if (IS_UNIQ) {
                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE + 1));
                }
                else {
                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE));
                }
            }
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase30()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION-WITH-HINT OF MOVABLE VALUES
    //
    // TBD Need to test with types that are convertible to 'value_type' (i.e.,
    // the 'ALT_VALUE_TYPE&&' template method).
    //
    // Concerns:
    //: 1 'insert' returns an iterator referring to the newly inserted element
    //:   if it did not already exists, and to the existing element if it did.
    //:
    //: 2 A new element is added to the container if the element did not
    //:   already exist, and the order of the container remains correct.
    //:
    //: 3 Inserting with the correct hint places the new element right before
    //:   the hint.
    //:
    //: 4 Inserting with the correct hint requires no more than 2 comparisons.
    //:
    //: 5 Incorrect hint will be ignored and 'insert' will proceed as if the
    //:   hint is not supplied.
    //:
    //: 6 The new element is move-inserted into the container.
    //:
    //: 7 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 8 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 For insertion we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 For each set of values, set hint to be 'lower_bound', 'begin',
    //:     'begin' + 1, 'end' - 1, 'end'
    //:
    //:     1 For each value in the set, 'insert' the value with hint.
    //:
    //:       1 Compute the number of allocations and verify it is as
    //:         expected.
    //:
    //:       2 Verify the return value and the resulting data in the container
    //:         is as expected.
    //:
    //:       3 Verify the new element is inserted right before the hint if
    //:         the hint is valid.  (C-3)
    //:
    //:       4 Verify the number of comparisons is no more than 2 if the hint
    //:         is valid.  (C-4)
    //:
    //:       5 Ensure that the move constructor was called on the value type
    //:         (if that type has a mechanism to detect such) where the value
    //:         has the same allocator as that of the container and a different
    //:         allocator than that of the container.  (C-5)
    //:
    //:       5 Verify all allocations are from the object's allocator.  (C-7)
    //:
    //: 2 Repeat P-1 under the presence of injected exceptions.  (C-8)
    //
    // Testing:
    //   iterator insert(const_iterator position, value_type&& value);
    //   iterator insert(const_iterator position, ALT_VALUE_TYPE&& value);
    // -----------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value +
                           bslma::UsesBslmaAllocator<VALUE>::value;

    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);

    static const struct {
        int         d_line;      // source line number
        const char *d_spec_p;    // specification string
        const char *d_unique_p;  // expected element values
    } DATA[] = {
        //line  spec           isUnique
        //----  --------       --------

        { L_,   "A",           "Y"           },
        { L_,   "AAA",         "YNN"         },
        { L_,   "ABCDEFGH",    "YYYYYYYY"    },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY" },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"  }
    };
    const int NUM_DATA = static_cast<const int>(sizeof DATA / sizeof *DATA);

    const int MAX_LENGTH = 10;

    if (verbose) printf("\nTesting 'insert' with hint.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const char *const UNIQUE = DATA[ti].d_unique_p;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;

                for (char cfg2 = 'a'; cfg2 <= 'b'; ++cfg2) {
                    const char CONFIG2 = cfg2;

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    Obj mX(&oa);  const Obj &X = mX;

                    bslma::TestAllocator za("different", veryVeryVeryVerbose);
                    bslma::TestAllocator *valAllocator = 0;

                    switch (cfg2) {
                      case 'a': {
                        // inserted value has same allocator
                        valAllocator = &oa;
                      } break;
                      case 'b': {
                        // inserted value has different allocator
                        valAllocator = &za;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } return;                                       // RETURN
                    }
                    bslma::TestAllocator& sa = *valAllocator;

                    bool lastIsUnique = true;
                    for (int tj = 0; tj < LENGTH; ++tj) {
                        const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                        const size_t SIZE    = X.size();

                        if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                        if (IS_UNIQ) {
                            EXPECTED[SIZE] = SPEC[tj];
                            std::sort(EXPECTED, EXPECTED + SIZE + 1);
                            EXPECTED[SIZE + 1] = '\0';

                            if (veryVeryVerbose) { P(EXPECTED); }
                        }

                        CIter hint;

                        switch (CONFIG) {
                          case 'a': {
                            hint = X.lower_bound(VALUES[tj].first);
                          } break;
                          case 'b': {
                            hint = X.begin();
                          } break;
                          case 'c': {
                            hint = X.begin();
                            if (hint != X.end()) {
                                ++hint;
                            }
                          } break;
                          case 'd': {
                            hint = X.end();
                            if (hint != X.begin()) {
                                --hint;
                            }
                          } break;
                          case 'e': {
                            hint = X.end();
                          } break;
                          default: {
                            ASSERTV(!"Unexpected configuration");
                          } return;                                   // RETURN
                        }

                        size_t EXP_COMP = X.key_comp().count();
                        if ('a' == CONFIG) {
                            if (!IS_UNIQ) {
                                EXP_COMP += 2;
                            }
                            else {
                                if (hint != X.begin()) {
                                    ++EXP_COMP;
                                }
                                if (hint != X.end()) {
                                    ++EXP_COMP;
                                }
                            }
                        }

                        const bsls::Types::Int64 BB = oa.numBlocksTotal();
                        const bsls::Types::Int64 B  = oa.numBlocksInUse();

                        typedef pair<KEY, VALUE> TValueType;

                        bsls::ObjectBuffer<TValueType> buffer;
                        TValueType *valptr = buffer.address();

                        IntToPairConverter<KEY, VALUE, ALLOC>::createInplace(
                                  valptr,
                                  TstFacility::getIdentifier(VALUES[tj].first),
                                  &sa);

                        Iter RESULT = mX.insert(hint, MoveUtil::move(*valptr));
                        MoveState::Enum mStateFirst =
                                TstFacility::getMovedFromState(valptr->first);
                        MoveState::Enum mStateSecond =
                                TstFacility::getMovedFromState(valptr->second);

                        bslma::DestructionUtil::destroy(valptr);

                        const bsls::Types::Int64 AA = oa.numBlocksTotal();
                        const bsls::Types::Int64 A  = oa.numBlocksInUse();

                        ASSERTV(LINE, CONFIG, tj, SIZE,
                                VALUES[tj] == *RESULT);

                        if ('a' == CONFIG) {
                            ASSERTV(LINE, tj, EXP_COMP, X.key_comp().count(),
                                    EXP_COMP == X.key_comp().count());

                            if (IS_UNIQ) {
                                ASSERTV(LINE, tj, hint == ++RESULT);
                            }
                            else {
                                ASSERTV(LINE, tj, hint == RESULT);
                            }
                        }

                        if (IS_UNIQ) {
                            bsls::Types::Int64 expAA = BB + TYPE_ALLOC;
                                                                    // new item
                            bsls::Types::Int64 expA  = B  + TYPE_ALLOC;
                                                                    // new item

                            ASSERTV(mStateFirst,
                                    MoveState::e_UNKNOWN == mStateFirst
                                 || MoveState::e_MOVED   == mStateFirst);
                            ASSERTV(mStateSecond,
                                    MoveState::e_UNKNOWN == mStateSecond
                                 || MoveState::e_MOVED   == mStateSecond);

                            if (expectToAllocate(SIZE + 1) && lastIsUnique) {
                                expA  = expA  + 1;     // resize on pool
                                expAA = expAA + 1;     // resize on pool
                            }

                            if (&sa == &oa) {
                                if (MoveState::e_MOVED != mStateFirst) {
                                    expAA +=
                                         bslma::UsesBslmaAllocator<KEY>::value;
                                                       // temporary (not moved)
                                }
                                if (MoveState::e_MOVED != mStateSecond) {
                                    expAA +=
                                       bslma::UsesBslmaAllocator<VALUE>::value;
                                                       // temporary (not moved)
                                }
                            }

                            ASSERTV(NameOf<ValueType>(), LINE, CONFIG2, AA,
                                                           expAA, AA == expAA);
                            ASSERTV(LINE, CONFIG2, tj, A,  expA,  A  == expA);
                            ASSERTV(LINE, CONFIG2, tj, SIZE,
                                                       SIZE + 1 == X.size());

                            TestValues exp(EXPECTED);
                            ASSERTV(LINE, tj,
                                    0 == verifyContainer(X, exp, SIZE + 1));
                        }
                        else {
                            bsls::Types::Int64 expAA = BB + TYPE_ALLOC;
                            bsls::Types::Int64 expA  = B;

                            ASSERTV(mStateFirst,
                                    MoveState::e_UNKNOWN == mStateFirst
                                 || MoveState::e_MOVED   == mStateFirst);
                            ASSERTV(mStateSecond,
                                    MoveState::e_UNKNOWN == mStateSecond
                                 || MoveState::e_MOVED   == mStateSecond);

                            if (expectToAllocate(SIZE + 1) && lastIsUnique) {
                                expA  = expA  + 1;          // resize on pool
                                expAA = expAA + 1;          // resize on pool
                            }

                            if (&sa == &oa) {
                                if (MoveState::e_MOVED != mStateFirst) {
                                    expAA +=
                                         bslma::UsesBslmaAllocator<KEY>::value;
                                                       // temporary (not moved)
                                }
                                if (MoveState::e_MOVED != mStateSecond) {
                                    expAA +=
                                       bslma::UsesBslmaAllocator<VALUE>::value;
                                                       // temporary (not moved)
                                }
                            }

                            ASSERTV(NameOf<ValueType>(), LINE, CONFIG2, AA,
                                                           expAA, expAA == AA);
                            ASSERTV(LINE, CONFIG2, tj, A,  expA,  expA  == A);
                            ASSERTV(LINE, CONFIG2, tj, SIZE == X.size());

                            TestValues exp(EXPECTED);
                            ASSERTV(LINE, CONFIG2, tj,
                                    0 == verifyContainer(X, exp, SIZE));
                        }

                        lastIsUnique = IS_UNIQ;
                    }
                }
            }
        }
    }
    if (verbose) printf("\nTesting 'insert' with exception.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const char *const UNIQUE = DATA[ti].d_unique_p;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;

                for (char cfg2 = 'a'; cfg2 <= 'b'; ++cfg2) {

                    bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
                    bslma::TestAllocator za("different", veryVeryVeryVerbose);

                    Obj mX(&oa);  const Obj &X = mX;

                    bslma::TestAllocator *valAllocator = 0;

                    switch (cfg2) {
                      case 'a': {
                        // inserted value has same allocator
                        valAllocator = &oa;
                      } break;
                      case 'b': {
                        // inserted value has different allocator
                        valAllocator = &za;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } return;                                       // RETURN
                    }
                    bslma::TestAllocator& sa = *valAllocator;

                    for (int tj = 0; tj < LENGTH; ++tj) {
                        const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                        const size_t SIZE    = X.size();

                        if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                        if (IS_UNIQ) {
                            EXPECTED[SIZE] = SPEC[tj];
                            std::sort(EXPECTED, EXPECTED + SIZE + 1);
                            EXPECTED[SIZE + 1] = '\0';

                            if (veryVeryVerbose) { P(EXPECTED); }
                        }

                        CIter hint;

                        switch (CONFIG) {
                          case 'a': {
                            hint = X.lower_bound(VALUES[tj].first);
                          } break;
                          case 'b': {
                            hint = X.begin();
                          } break;
                          case 'c': {
                            hint = X.begin();
                            if (hint != X.end()) {
                                ++hint;
                            }
                          } break;
                          case 'd': {
                            hint = X.end();
                            if (hint != X.begin()) {
                                --hint;
                            }
                          } break;
                          case 'e': {
                            hint = X.end();
                          } break;
                          default: {
                            ASSERTV(!"Unexpected configuration");
                          } return;                                   // RETURN
                        }

                        bslma::TestAllocator scratch("scratch",
                                                     veryVeryVeryVerbose);

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                            Obj mZ(&scratch);   const Obj& Z = mZ;
                            for (int tk = 0; tk < tj; ++tk) {
                                primaryManipulator(
                                  &mZ,
                                  TstFacility::getIdentifier(VALUES[tk].first),
                                  &scratch);
                            }
                            ASSERTV(Z, X, Z == X);

                            ExceptionProctor<Obj> proctor(&X, L_,
                                                          MoveUtil::move(mZ));

                            typedef pair<KEY, VALUE> TValueType;

                            bsls::ObjectBuffer<TValueType> buffer;
                            TValueType *valptr = buffer.address();

                            IntToPairConverter<KEY, VALUE, ALLOC>::
                                createInplace(
                                  valptr,
                                  TstFacility::getIdentifier(VALUES[tj].first),
                                  &sa);

                            bslma::DestructorGuard<TValueType> valueGuard(
                                                                       valptr);
                            Iter RESULT =
                                      mX.insert(hint, MoveUtil::move(*valptr));

                            proctor.release();

                            ASSERTV(LINE, CONFIG, tj, SIZE,
                                    VALUES[tj] == *RESULT);

                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                        if (IS_UNIQ) {
                            ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                            TestValues exp(EXPECTED);
                            ASSERTV(LINE, tj,
                                    0 == verifyContainer(X, exp, SIZE + 1));
                        }
                        else {
                            ASSERTV(LINE, tj, SIZE == X.size());

                            TestValues exp(EXPECTED);
                            ASSERTV(LINE, tj,
                                    0 == verifyContainer(X, exp, SIZE));
                        }
                    }
                }
            }
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase29()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION OF MOVABLE VALUES
    //
    // TBD Need to test with types that are convertible to 'value_type' (i.e.,
    // the 'ALT_VALUE_TYPE&&' template method).
    //
    // Concerns:
    //: 1 'insert' returns a pair containing an iterator and a 'bool'
    //:
    //: 2 The iterator returned refers to the newly inserted element if it did
    //:   not already exists, and to the existing element if it did.
    //:
    //: 3 The 'bool' returned is 'true' if a new element is inserted, and
    //:   'false' otherwise.
    //:
    //: 4 A new element is added to the container if the element did not
    //:   already exist, and the order of the container remains correct.
    //:
    //: 5 The new element is move-inserted into the container.
    //:
    //: 6 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 7 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 For insertion we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 Compute the number of allocations and verify it is as expected.
    //:
    //:   2 If the object did not contain 'value', verify it now exist.
    //:     Otherwise, verify the return value is as expected.  (C-1..4)
    //:
    //:   3 Ensure that the move constructor was called on the value type (if
    //:     that type has a mechanism to detect such) where the value has the
    //:     same allocator as that of the container and a different allocator
    //:     than that of the container.  (C-5)
    //:
    //:   4 Verify all allocations are from the object's allocator.  (C-6)
    //:
    //: 2 Repeat P-1 under the presence of injected exceptions.  (C-7)
    //
    // Testing:
    //   pair<iterator, bool> insert(value_type&& value);
    //   pair<iterator, bool> insert(ALT_VALUE_TYPE&& value);
    // -----------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value +
                           bslma::UsesBslmaAllocator<VALUE>::value;

    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);

    static const struct {
        int         d_line;      // source line number
        const char *d_spec_p;    // specification string
        const char *d_unique_p;  // expected element values
    } DATA[] = {
        //line  spec           isUnique
        //----  ----           --------
        { L_,   "A",           "Y"           },
        { L_,   "AB",          "YY"          },
        { L_,   "ABC",         "YYY"         },
        { L_,   "AAA",         "YNN"         },
        { L_,   "ABCDEFGH",    "YYYYYYYY"    },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY" },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"  }
    };
    const int NUM_DATA = static_cast<const int>(sizeof DATA / sizeof *DATA);

    const int MAX_LENGTH = 10;

    if (verbose) printf("\nTesting 'insert' without exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const char *const UNIQUE = DATA[ti].d_unique_p;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
                bslma::TestAllocator za("different", veryVeryVeryVerbose);

                Obj mX(&oa);  const Obj &X = mX;

                bslma::TestAllocator *valAllocator = 0;

                switch (cfg) {
                  case 'a': {
                    // inserted value has same allocator
                    valAllocator = &oa;
                  } break;
                  case 'b': {
                    // inserted value has different allocator
                    valAllocator = &za;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }
                bslma::TestAllocator& sa = *valAllocator;

                bool lastIsUnique = true;
                for (int tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                    if (IS_UNIQ) {
                        EXPECTED[SIZE] = SPEC[tj];
                        std::sort(EXPECTED, EXPECTED + SIZE + 1);
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { P(EXPECTED); }
                    }

                    const bsls::Types::Int64 BB = oa.numBlocksTotal();
                    const bsls::Types::Int64 B  = oa.numBlocksInUse();

                    typedef pair<KEY, VALUE> TValueType;

                    bsls::ObjectBuffer<TValueType> buffer;
                    TValueType *valptr = buffer.address();

                    IntToPairConverter<KEY, VALUE, ALLOC>::createInplace(
                                  valptr,
                                  TstFacility::getIdentifier(VALUES[tj].first),
                                  &sa);

                    pair<Iter, bool> RESULT =
                                            mX.insert(MoveUtil::move(*valptr));
                    MoveState::Enum mStateFirst =
                                TstFacility::getMovedFromState(valptr->first);
                    MoveState::Enum mStateSecond =
                                TstFacility::getMovedFromState(valptr->second);

                    bslma::DestructionUtil::destroy(valptr);

                    ASSERTV(LINE, CONFIG, tj, SIZE,
                            IS_UNIQ    == RESULT.second);
                    ASSERTV(LINE, CONFIG, tj, SIZE,
                            VALUES[tj] == *(RESULT.first));

                    const bsls::Types::Int64 AA = oa.numBlocksTotal();
                    const bsls::Types::Int64 A  = oa.numBlocksInUse();

                    if (IS_UNIQ) {
                        bsls::Types::Int64 expAA = BB + TYPE_ALLOC; // new item
                        bsls::Types::Int64 expA  = B  + TYPE_ALLOC; // new item

                        ASSERTV(mStateFirst,
                                MoveState::e_UNKNOWN == mStateFirst
                             || MoveState::e_MOVED   == mStateFirst);
                        ASSERTV(mStateSecond,
                                MoveState::e_UNKNOWN == mStateSecond
                             || MoveState::e_MOVED   == mStateSecond);

                        if (expectToAllocate(SIZE + 1) && lastIsUnique) {
                            expA  = expA  + 1;              // resize on pool
                            expAA = expAA + 1;              // resize on pool
                        }

                        if (&sa == &oa) {
                            if (MoveState::e_MOVED != mStateFirst) {
                                expAA += bslma::UsesBslmaAllocator<KEY>::value;
                                                       // temporary (not moved)
                            }
                            if (MoveState::e_MOVED != mStateSecond) {
                                expAA +=
                                       bslma::UsesBslmaAllocator<VALUE>::value;
                                                       // temporary (not moved)
                            }
                        }

                        ASSERTV(NameOf<ValueType>(), LINE, CONFIG, AA,
                                                           expAA, expAA == AA);
                        ASSERTV(LINE, CONFIG, tj, A,  expA,  expA  == A);
                        ASSERTV(LINE, CONFIG, tj, SIZE, SIZE + 1 == X.size());

                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, CONFIG, tj, 0 == verifyContainer(
                                                                    X,
                                                                    exp,
                                                                    SIZE + 1));
                    }
                    else {
                        bsls::Types::Int64 expAA = BB + TYPE_ALLOC;
                        bsls::Types::Int64 expA  = B;

                        ASSERTV(mStateFirst,
                                MoveState::e_UNKNOWN == mStateFirst
                             || MoveState::e_MOVED   == mStateFirst);
                        ASSERTV(mStateSecond,
                                MoveState::e_UNKNOWN == mStateSecond
                             || MoveState::e_MOVED   == mStateSecond);

                        if (expectToAllocate(SIZE + 1) && lastIsUnique) {
                            expA  = expA  + 1;              // resize on pool
                            expAA = expAA + 1;              // resize on pool
                        }

                        if (&sa == &oa) {
                            if (MoveState::e_MOVED != mStateFirst) {
                                expAA += bslma::UsesBslmaAllocator<KEY>::value;
                                                       // temporary (not moved)
                            }
                            if (MoveState::e_MOVED != mStateSecond) {
                                expAA +=
                                       bslma::UsesBslmaAllocator<VALUE>::value;
                                                       // temporary (not moved)
                            }
                        }

                        ASSERTV(NameOf<ValueType>(), LINE, CONFIG, AA,
                                                           expAA, expAA == AA);
                        ASSERTV(LINE, CONFIG, tj, A,  expA,  expA  == A);
                        ASSERTV(LINE, CONFIG, tj, SIZE == X.size());

                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, CONFIG, tj, 0 == verifyContainer(X,
                                                                       exp,
                                                                       SIZE));
                    }

                    lastIsUnique = IS_UNIQ;
                }
            }
        }
    }

    if (verbose) printf("\nTesting 'insert' with injected exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const char *const UNIQUE = DATA[ti].d_unique_p;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
                bslma::TestAllocator za("different", veryVeryVeryVerbose);

                Obj mX(&oa);  const Obj &X = mX;

                bslma::TestAllocator *valAllocator = 0;

                switch (cfg) {
                  case 'a': {
                    // inserted value has same allocator
                    valAllocator = &oa;
                  } break;
                  case 'b': {
                    // inserted value has different allocator
                    valAllocator = &za;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }
                bslma::TestAllocator& sa = *valAllocator;

                for (int tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                    if (IS_UNIQ) {
                        EXPECTED[SIZE] = SPEC[tj];
                        std::sort(EXPECTED, EXPECTED + SIZE + 1);
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { P(EXPECTED); }
                    }

                    bslma::TestAllocator scratch("scratch",
                                                 veryVeryVeryVerbose);
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        Obj mZ(&scratch);   const Obj& Z = mZ;
                        for (int tk = 0; tk < tj; ++tk) {
                            primaryManipulator(
                                  &mZ,
                                  TstFacility::getIdentifier(VALUES[tk].first),
                                  &scratch);
                        }
                        ASSERTV(Z, X, Z == X);

                        ExceptionProctor<Obj> proctor(&X, L_,
                                                      MoveUtil::move(mZ));

                        typedef pair<KEY, VALUE> TValueType;

                        bsls::ObjectBuffer<TValueType> buffer;
                        TValueType *valptr = buffer.address();

                        IntToPairConverter<KEY, VALUE, ALLOC>::createInplace(
                                  valptr,
                                  TstFacility::getIdentifier(VALUES[tj].first),
                                  &sa);

                        bslma::DestructorGuard<TValueType> valueGuard(valptr);

                        pair<Iter, bool> RESULT =
                                            mX.insert(MoveUtil::move(*valptr));

                        proctor.release();

                        ASSERTV(LINE, tj, SIZE, IS_UNIQ == RESULT.second);
                        ASSERTV(LINE, tj, SIZE, VALUES[tj] == *(RESULT.first));
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    if (IS_UNIQ) {
                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, tj,
                                0 == verifyContainer(X, exp, SIZE + 1));
                    }
                    else {
                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, tj,
                                0 == verifyContainer(X, exp, SIZE));
                    }
                }
            }
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase28_dispatch()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE-ASSIGNMENT OPERATOR
    //
    // Concerns:
    //  TBD: the test does not yet cover the case where allocator propagation
    //       is enabled for move assignment (hard-coded to 'false') -- i.e.,
    //       parts of C-5..6 are currently not addressed.
    //
    //: 1 The signature and return type are standard.
    //:
    //: 2 The reference returned is to the target object (i.e., '*this').
    //:
    //: 3 The move assignment operator can change the value of a modifiable
    //:   target object to that of any source object.
    //:
    //: 4 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 5 If allocator propagation is not enabled for move-assignment, the
    //:   allocator address held by the target object is unchanged; otherwise,
    //:   the allocator address held by the target object is changed to that of
    //:   the source.
    //:
    //: 6 If allocator propagation is enabled for move-assignment, any memory
    //:   allocation from the original target allocator is released after the
    //:   operation has completed.
    //:
    //: 7 All elements in the target object are either move-assigned to or
    //:   destroyed.
    //:
    //: 8 The source object is left in a valid state but unspecified state; the
    //:   allocator address held by the original object is unchanged.
    //:
    //: 9 Subsequent changes to or destruction of the original object have no
    //:   effect on the move-constructed object and vice-versa.
    //:
    //:10 Assigning a source object having the default-constructed value
    //:   allocates no memory; assigning a value to a target object in the
    //:   default state does not allocate or deallocate any memory.
    //:
    //:11 Every object releases any allocated memory at destruction.
    //:
    //:12 Any memory allocation is exception neutral.
    //:
    //:13 Assigning an object to itself behaves as expected (alias-safety).
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   copy-assignment operator defined in this component.             (C-1)
    //:
    //: 2 Iterate over a set of object values with substantial and varied
    //:   differences, ordered by increasing length, and create for each a
    //:   control object representing the source of the assignment, with its
    //:   own scratch allocator.
    //:
    //: 3 Iterate again over the same set of object values and create a
    //:   object representing the target of the assignment, with its own unique
    //:   object allocator.
    //:
    //: 4 In a loop consisting of two iterations, create a source object (a
    //:   copy of the control object in P-1) with 1) a different allocator than
    //:   that of target and 2) the same allocator as that of the target.
    //:
    //: 5 Call the move-assignment operator in the presence of exceptions
    //:   during memory allocations (using a 'bslma::Allocator' and varying
    //:   its allocation limit) and verify the following:                (C-12)
    //:
    //:   1 The address of the return value is the same as that of the target
    //:     object.                                                       (C-2)
    //:
    //:   2 The object being assigned to has the same value as that of the
    //:     source object before assignment (i.e., the control object).   (C-3)
    //:
    //:   3 CONTAINER SPECIFIC NOTE: Ensure that the comparator was assigned.
    //:
    //:   4 If the source and target objects use the same allocator, ensure
    //:     that there is no net increase in memory use from the common
    //:     allocator.  Also consider the following cases:
    //:
    //:     1 If the source object is empty, confirm that there are no bytes
    //:       currently in use from the common allocator.                (C-10)
    //:
    //:     2 If the target object is empty, confirm that there was no memory
    //:       change in memory usage.                                    (C-10)
    //:
    //:   5 If the source and target objects use different allocators, ensure
    //:     that each element in the source object is move-inserted into the
    //:     target object.                                                (C-7)
    //:
    //:   6 Ensure that the source, target, and control object continue to have
    //:     the correct allocator and that all memory allocations come from the
    //:     appropriate allocator.                                        (C-4)
    //:
    //:   7 Manipulate the source object (after assignment) to ensure that it
    //:     is in a valid state, destroy it, and then manipulate the target
    //:     object to ensure that it is in a valid state.                 (C-8)
    //:
    //:   8 Verify all memory is released when the source and target objects
    //:     are destroyed.                                               (C-11)
    //:
    //: 6 Use a test allocator installed as the default allocator to verify
    //:   that no memory is ever allocated from the default allocator.
    //
    // Testing:
    //   map& operator=(map&& rhs);
    // ------------------------------------------------------------------------

    // Since this function is called with a variety of template arguments, it
    // is necessary to infer some things about our template arguments in order
    // to print a meaningful banner.

    const bool isPropagate =
                AllocatorTraits::propagate_on_container_move_assignment::value;
    const bool otherTraitsSet =
                AllocatorTraits::propagate_on_container_copy_assignment::value;

    // We can print the banner now:

    if (verbose) printf("%sTESTING MOVE-ASSIGN '%s' OTHER:%c PROP:%c"
                                                                " ALLOC: %s\n",
                        veryVerbose ? "\n" : "",
                        NameOf<VALUE>().name(), otherTraitsSet ? 'T' : 'F',
                        isPropagate ? 'T' : 'F',
                        allocCategoryAsStr());

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value +
                           bslma::UsesBslmaAllocator<VALUE>::value;

    const int              NUM_DATA        = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         doa("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&doa);

    {
        using namespace bsl;

        Obj& (Obj::*operatorMAg) (bslmf::MovableRef<Obj>) = &Obj::operator=;
        (void)operatorMAg;  // quash potential compiler warning
    }

    bslma::TestAllocator soa("scratch",   veryVeryVeryVerbose);
    bslma::TestAllocator ooa("object",    veryVeryVeryVerbose);
    bslma::TestAllocator zoa("different", veryVeryVeryVerbose);

    ALLOC da(&doa);
    ALLOC sa(&soa);
    ALLOC oa(&ooa);
    ALLOC za(&zoa);

    bslma::TestAllocator foa("footprint", veryVeryVeryVerbose);

    // Check remaining properties of allocator to make sure they all match
    // 'otherTraitsSet'.

    BSLMF_ASSERT(otherTraitsSet ==
                          AllocatorTraits::propagate_on_container_swap::value);
    ASSERT((otherTraitsSet ? sa : da) ==
                   AllocatorTraits::select_on_container_copy_construction(sa));

    // Create first object.
    if (veryVerbose)
        printf("\nTesting move assignment.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const char *const SPEC1   = DATA[ti].d_results_p;
            const size_t      LENGTH1 = strlen(SPEC1);

            if (ti && DATA[ti].d_index == DATA[ti-1].d_index) {
                continue;    // redundant, skip
            }

            const bsls::Types::Int64 BEFORE = soa.numBytesInUse();

            Obj  mZZ(sa);  const Obj&  ZZ = gg(&mZZ, SPEC1);

            const bsls::Types::Int64 BIU = soa.numBytesInUse() - BEFORE;

            if (veryVerbose) { T_ P_(SPEC1) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.
            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(SPEC1, Obj(), ZZ, Obj() == ZZ);
                firstFlag = false;
            }

            // Create second object.
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const char *const SPEC2   = DATA[tj].d_results_p;

                if (tj && DATA[tj].d_index == DATA[tj-1].d_index) {
                    continue;    // redundant, skip
                }

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                    const char CONFIG = cfg;  // how we specify the allocator

                    Obj *objPtr = new (foa) Obj(oa);
                    Obj& mX = *objPtr;  const Obj& X  = gg(&mX, SPEC2);

                    bslma::TestAllocator& roa = 'a' == CONFIG ? zoa : ooa;
                    ALLOC&                ra  = 'a' == CONFIG ? za  : oa;

                    Obj *srcPtr = new (foa) Obj(ra);
                    Obj& mZ = *srcPtr;  const Obj& Z = gg(&mZ, SPEC1);

                    if (veryVerbose) { T_ P_(SPEC2) P(Z) }
                    if (veryVerbose) { T_ P_(SPEC2) P(X) }

                    ASSERTV(SPEC1, SPEC2, Z, X,
                            (Z == X) == (ti == tj));

                    bool empty = 0 == ZZ.size();

                    const typename Obj::value_type *pointers[2];
                    storeFirstNElemAddr(pointers, Z,
                                        sizeof pointers / sizeof *pointers);

                    bslma::TestAllocatorMonitor oam(&ooa), zam(&zoa);

                    Obj *mR = &(mX = MoveUtil::move(mZ));
                    ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                    // Verify the value of the object.
                    ASSERTV(SPEC1, SPEC2,  X,  ZZ,  X ==  ZZ);
                    ASSERTV(SPEC1, SPEC2, isPropagate, Z, Z.empty());

                    if (0 == LENGTH1) {
                        // assigned an empty map
                        ASSERTV(SPEC1, SPEC2, ooa.numBytesInUse(),
                                0 == ooa.numBytesInUse());
                    }

                    // CONTAINER SPECIFIC NOTE: For 'map', the original object
                    // is left in the default state even when the source and
                    // target objects use different allocators because
                    // move-insertion changes the value of the source elements
                    // and violates the uniqueness requirements for keys
                    // contained in the 'map'.

                    if (&ra == &oa) {
                        // same allocator
                        ASSERTV(SPEC1, SPEC2, oam.isTotalSame());
                        if (!*SPEC2) {
                            // assigning to an empty map
                            ASSERTV(SPEC1, SPEC2, oam.isInUseSame());
                            ASSERTV(SPEC1, SPEC2, BIU, ooa.numBytesInUse(),
                                    BIU == ooa.numBytesInUse());
                        }

                        // 2. unchanged address of contained element(s)
                        ASSERT(0 == checkFirstNElemAddr(pointers,
                                                        X,
                                                        sizeof pointers /
                                                            sizeof *pointers));

                        // 3. original object left empty
                        ASSERTV(SPEC1, SPEC2, &ra == &oa, Z, 0 == Z.size());

                        // 4. nothing from the other allocator
                        ASSERTV(SPEC1, SPEC2, zam.isTotalSame());
                    }
                    else {
                        // 1. each element in original move-inserted
                        ASSERTV(SPEC1,
                                SPEC2,
                                X.end() == TstMoveUtil::findFirstNotMovedInto(
                                               X.begin(), X.end()));

                        // 2. CONTAINER SPECIFIC NOTE: original object left
                        // empty
                        ASSERTV(SPEC1, SPEC2, &ra == &oa, Z, 0 == Z.size());

                        // 3. additional memory checks
                        ASSERTV(SPEC1, SPEC2, zam.isTotalSame());
                        if (isPropagate) {
                            ASSERTV(SPEC1, SPEC2, oam.isTotalSame(),
                                                            oam.isTotalSame());
                        }
                        else {
                            ASSERTV(SPEC1, SPEC2, oam.isTotalSame(),
                                                   empty == oam.isTotalSame());
                        }
                    }

                    ALLOC& dstAlloc = (isPropagate ? ra : oa);
                    bslma::TestAllocator& dstOA = (isPropagate ? roa : ooa);

                    // Verify that 'X', 'Z', and 'ZZ' have correct allocator.
                    ASSERTV(SPEC1, SPEC2, sa == ZZ.get_allocator());
                    ASSERTV(SPEC1, SPEC2, isPropagate, &ra == &oa,
                                                dstAlloc == X.get_allocator());
                    ASSERTV(SPEC1, SPEC2, isPropagate, &ra == &oa,
                                                      ra == Z.get_allocator());

                    // Manipulate source object 'Z' to ensure it is in a valid
                    // state and is independent of 'X'.
                    pair<Iter, bool> RESULT;
                    RESULT = primaryManipulator(&mZ, 'Z', ra);
                    ASSERTV(true == RESULT.second);
                    ASSERTV(SPEC1, SPEC2, isPropagate, Z, 1 == Z.size());
                    ASSERTV(SPEC1, SPEC2, Z, ZZ, Z != ZZ);
                    ASSERTV(SPEC1, SPEC2, X, ZZ, X == ZZ);

                    foa.deleteObject(srcPtr);

                    ASSERTV(SPEC1, roa.numBlocksInUse(), &ra == &oa,
                                                                   isPropagate,
                                     (*SPEC1 && (isPropagate || &ra == &oa)) ==
                                                   (0 < roa.numBlocksInUse()));

                    // Verify subsequent manipulation of target object 'X'.
                    RESULT = primaryManipulator(&mX, 'Z', dstAlloc);
                    ASSERTV(true == RESULT.second);
                    ASSERTV(SPEC1, SPEC2, LENGTH1 + 1, X.size(),
                                                      LENGTH1 + 1 == X.size());
                    ASSERTV(SPEC1, SPEC2, X, ZZ, X != ZZ);

                    foa.deleteObject(objPtr);

                    ASSERTV(SPEC1, SPEC2, ooa.numBlocksInUse(),
                            0 == dstOA.numBlocksInUse());
                    ASSERTV(SPEC1, SPEC2, roa.numBlocksInUse(),
                            0 == roa.numBlocksInUse());
                }

                // self-assignment

                {
                    Obj mX(oa);   const Obj& X = gg(&mX,  SPEC1);

                    ASSERTV(SPEC1, ZZ, X, ZZ == X);

                    bslma::TestAllocatorMonitor oam(&ooa);

                    int numPasses = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ooa) {
                        ++numPasses;

                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = MoveUtil::move(mX));
                        ASSERTV(SPEC1, mR, &X, mR == &X);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERT(1 == numPasses);
                    ASSERTV(SPEC1, ZZ,  X, ZZ == X);
                    ASSERTV(SPEC1, oa == X.get_allocator());

                    ASSERTV(SPEC1, oam.isTotalSame());

                    ASSERTV(e_STATEFUL == s_allocCategory ||
                                                    0 == doa.numBlocksTotal());
                }
            }

            // Verify all object memory is released on destruction.

            ASSERTV(SPEC1, foa.numBlocksInUse(), 0 == foa.numBlocksInUse());
            ASSERTV(SPEC1, ooa.numBlocksInUse(), 0 == ooa.numBlocksInUse());
            ASSERTV(SPEC1, zoa.numBlocksInUse(), 0 == zoa.numBlocksInUse());
        }
    }

    if (veryVerbose)
        printf("\nTesting move assignment with injected exceptions.\n");
    {
        // Create first object.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const char *const SPEC1   = DATA[ti].d_results_p;
            const size_t      LENGTH1 = strlen(SPEC1);

            if (ti && DATA[ti].d_index == DATA[ti-1].d_index) {
                continue;    // redundant, skip
            }

            if (LENGTH1 > 4) {
                continue;    // skip to save time (each loop is O(N**2))
            }

            Obj mZZ(sa);  const Obj& ZZ = gg(&mZZ, SPEC1);

            if (veryVerbose) { T_ P_(SPEC1) P(ZZ) }

            // Create second object.
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const char *const SPEC2   = DATA[tj].d_results_p;

                if (tj && DATA[tj].d_index == DATA[tj-1].d_index) {
                    continue;    // redundant, skip
                }

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                    const char CONFIG = cfg;  // how we specify the allocator
                    ALLOC& ra  = 'a' == CONFIG ? za  : oa;

                    int numPasses = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ooa) {
                        ++numPasses;

                        Int64 al = ooa.allocationLimit();
                        ooa.setAllocationLimit(-1);

                        Obj mX(oa);  const Obj& X = gg(&mX, SPEC2);
                        Obj mZ(ra);  const Obj& Z = gg(&mZ, SPEC1);

                        if (1 == numPasses && veryVeryVerbose) {
                            T_ P_(SPEC1) P_(SPEC2) P_(Z) P(X);
                        }

                        ASSERTV(SPEC1, SPEC2, Z, X, (Z == X) == (ti == tj));

                        ooa.setAllocationLimit(al);

                        Obj mE(sa);    // uses 'sa', not 'oa', so construction
                                       // & initialization won't throw
                        if (&ra != &oa
                         && ooa.allocationLimit() >= 0
                         && ooa.allocationLimit() <= TYPE_ALLOC) {
                            // We will throw on the reserveNodes so that source
                            // object will be unchanged on exception.
                            gg(&mE, SPEC1);
                        }
                        ExceptionProctor<Obj> proctor(&Z, L_,
                                                      MoveUtil::move(mE));

                        Obj *mR = &(mX = MoveUtil::move(mZ));
                        ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                        proctor.release();
                        ooa.setAllocationLimit(-1);

                        // Verify the value of the object.
                        ASSERTV(SPEC1, SPEC2,   X,  ZZ,   ZZ == X);
                        ASSERTV(SPEC1, SPEC2, isPropagate, Z, Z.empty());

                        // Manipulate source object 'Z' to ensure it is in a
                        // valid state and is independent of 'X' or 'ZZ'.
                        pair<Iter, bool> RESULT;
                        RESULT = primaryManipulator(&mZ, 'Z', ra);
                        ASSERTV(true == RESULT.second);
                        ASSERTV(SPEC1, SPEC2, isPropagate, Z, 1 == Z.size());
                        ASSERTV(SPEC1, SPEC2, Z, ZZ, Z != ZZ);
                        ASSERTV(SPEC1, SPEC2, X, ZZ, X == ZZ);

                        // Verify subsequent manipulation of target object 'X'.

                        RESULT = primaryManipulator(&mX, 'Z', oa);
                        ASSERTV(true == RESULT.second);
                        ASSERTV(SPEC1, SPEC2, LENGTH1 + 1, X.size(),
                                LENGTH1 + 1 == X.size());
                        ASSERTV(SPEC1, SPEC2, X, ZZ, X != ZZ);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(SPEC1, SPEC2, numPasses, &ra != &oa,
                          (PLAT_EXC && &ra != &oa && !isPropagate && *SPEC1) ==
                                                              (numPasses > 1));
                }
            }
        }
    }

    ASSERTV(e_STATEFUL == s_allocCategory || 0 == doa.numBlocksTotal());
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase27()
{
    // ------------------------------------------------------------------------
    // MOVE CONSTRUCTOR
    //
    // Concerns:
    //: 1 The newly created object has the same value (using the equality
    //:   operator) as that of the original object before the call.
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 3 The allocator is propagated to the newly created object if (and only
    //:   if) no allocator is specified in the call to the move constructor.
    //:
    //: 4 A constant-time move, with no additional memory allocations or
    //:   deallocations, is performed when no allocator or the same allocator
    //:   as that of the original object is passed to the move constructor.
    //:
    //: 5 A linear operation, where each element is move-inserted into the
    //:   newly created object, is performed when a 0 or an allocator that is
    //:   different than that of the original object is explicitly passed to
    //:   the move constructor.
    //:
    //: 6 The original object is always left in a valid state; the allocator
    //:   address held by the original object is unchanged.
    //:
    //: 7 Subsequent changes to, or destruction of, the original object have no
    //:   effect on the move-constructed object and vice-versa.
    //:
    //: 8 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 9 Every object releases any allocated memory at destruction.
    //
    //:10 Any memory allocation is exception neutral.
    //
    // Plan:
    //: 1 Specify a set 'S' of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used sequentially in
    //:   the following tests; for each entry, create a control object.   (C-2)
    //:
    //: 2 Call the move constructor to create the container in all relevant use
    //:   cases involving the allocator 1) no allocator passed in, 2) a 0 is
    //    explicitly passed in as the allocator argument, 3) the same allocator
    //:   as that of the original object is explicitly passed in, and 4) a
    //:   different allocator than that of the original object is passed in.
    //:
    //: 3 For each of the object values (P-1) and for each configuration (P-2),
    //:   verify the following:
    //:
    //:   1 Verify the newly created object has the same value as that of the
    //:     original object before the call to the move constructor (control
    //:     value).                                                       (C-1)
    //:
    //:   2 CONTAINER SPECIFIC NOTE: Ensure that the comparator was copied.
    //:
    //:   3 Where a constant-time move is expected, ensure that no memory was
    //:     allocated, that element addresses did not change, and that the
    //:     original object is left in the default state.         (C-3..5, C-7)
    //:
    //:   4 Where a linear-time move is expected, ensure that the move
    //:     constructor was called for each element.                   (C-6..7)
    //:
    //:   5 CONTAINER SPECIFIC:
    //:     Where a linear-time move is expected, the value of the original
    //:     object is also left in the default state because move insertion
    //:     changes the value of the original pair object and violates the
    //:     class invariant enforcing uniqueness of keys.                 (C-7)
    //:
    //:   6 Ensure that the new original, and control object continue to have
    //:     the correct allocator and that all memory allocations come from the
    //:     appropriate allocator.                                   (C-3, C-9)
    //:
    //:   7 Manipulate the original object (after the move construction) to
    //:     ensure it is in a valid state, destroy it, and then manipulate the
    //:     newly created object to ensure that it is in a valid state.   (C-8)
    //:
    //:   8 Verify all memory is released when the object is destroyed.  (C-11)
    //;
    //: 4 Perform tests in the presence of exceptions during memory allocations
    //:   using a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //:                                                                  (C-10)
    //
    // Testing:
    //   map(map&& original);
    //   map(map&& original, const A& allocator);
    // ------------------------------------------------------------------------

    const TestValues VALUES;

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value +
                           bslma::UsesBslmaAllocator<VALUE>::value;

    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
        "DEAB",
        "EABCD",
        "ABCDEFG",
        "HFGEDCBA",
        "CFHEBIDGA",
        "BENCKHGMALJDFOI",
        "IDMLNEFHOPKGBCJA",
        "OIQGDNPMLKBACHFEJ"
    };
    const int NUM_SPECS = static_cast<const int>(sizeof SPECS / sizeof *SPECS);

    if (verbose)
        printf("\nTesting both versions of move constructor.\n");
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\nFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            // Create control object, 'ZZ', with the scratch allocator.

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC);

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(ti, Obj(), ZZ, Obj() == ZZ);
                firstFlag = false;
            }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator za("different", veryVeryVeryVerbose);

                bslma::Default::setDefaultAllocatorRaw(&da);

                // Create source object 'Z'.
                Obj *srcPtr = new (fa) Obj(&sa);
                Obj& mZ = *srcPtr;  const Obj& Z = gg(&mZ, SPEC);

                const typename Obj::value_type *pointers[2];
                storeFirstNElemAddr(pointers, Z,
                                    sizeof pointers / sizeof *pointers);

                bslma::TestAllocatorMonitor oam(&da), sam(&sa);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;
                bslma::TestAllocator *othAllocatorPtr;

                bool empty = 0 == ZZ.size();

                switch (CONFIG) {
                  case 'a': {
                    oam.reset(&sa);
                    objPtr = new (fa) Obj(MoveUtil::move(mZ));
                    objAllocatorPtr = &sa;
                    othAllocatorPtr = &da;
                  } break;
                  case 'b': {
                    oam.reset(&da);
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), 0);
                    objAllocatorPtr = &da;
                    othAllocatorPtr = &za;
                  } break;
                  case 'c': {
                    oam.reset(&sa);
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), &sa);
                    objAllocatorPtr = &sa;
                    othAllocatorPtr = &da;
                  } break;
                  case 'd': {
                    oam.reset(&za);
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), &za);
                    objAllocatorPtr = &za;
                    othAllocatorPtr = &da;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = *othAllocatorPtr;

                Obj& mX = *objPtr;      const Obj& X = mX;

                // Verify the value of the object.
                ASSERTV(SPEC, CONFIG, X == ZZ);

                // Verify that the comparator was copied.
                ASSERTV(SPEC, CONFIG,
                        X.key_comp().count() == Z.key_comp().count());

                // CONTAINER SPECIFIC NOTE: For 'map', the original object is
                // left in the default state even when the source and target
                // objects use different allocators because move-insertion
                // changes the value of the source element and violates the
                // uniqueness requirements for keys contained in the 'map'.

                if (&sa == &oa) {
                    // 1. no memory allocation
                    ASSERTV(SPEC, CONFIG, &sa == &oa, oam.isTotalSame());
                    // 2. unchanged address of contained element(s)
                    ASSERT(0 == checkFirstNElemAddr(
                                          pointers, X,
                                          sizeof pointers / sizeof *pointers));
                    // 3. original object left empty
                    ASSERTV(SPEC, CONFIG, &sa == &oa, Z, 0 == Z.size());
                }
                else {
                    // 1. each element in original move-inserted
                    ASSERTV(SPEC, X.end() ==
                     TstMoveUtil::findFirstNotMovedInto(X.cbegin(), X.cend()));

                    // 2. original object left empty
                    ASSERTV(SPEC, CONFIG, &sa == &oa, Z, 0 == Z.size());

                    // 3. additional memory checks
                    ASSERTV(SPEC, CONFIG, &sa == &oa,
                            oam.isTotalUp() || empty);
                }

                // Verify that 'X', 'Z', and 'ZZ' have the correct allocator.

                ASSERTV(SPEC, CONFIG, &scratch == ZZ.get_allocator());
                ASSERTV(SPEC, CONFIG,      &sa ==  Z.get_allocator());
                ASSERTV(SPEC, CONFIG,      &oa ==  X.get_allocator());

                // Verify no allocation from the non-object allocator and that
                // object allocator is hooked up.

                ASSERTV(SPEC, CONFIG, 0 == noa.numBlocksTotal());
                ASSERTV(SPEC, CONFIG, 0 < oa.numBlocksTotal() || empty);

                // Manipulate source object 'Z' to ensure it is in a valid
                // state and is independent of 'X'.

                pair<Iter, bool> RESULT = primaryManipulator(&mZ, 'Z', &sa);
                ASSERTV(true == RESULT.second);
                ASSERTV(SPEC, CONFIG, Z, 1 == Z.size());
                ASSERTV(SPEC, CONFIG, Z, ZZ, Z != ZZ);
                ASSERTV(SPEC, CONFIG, X, ZZ, X == ZZ);

                fa.deleteObject(srcPtr);

                ASSERTV(SPEC, CONFIG, X, ZZ, X == ZZ);

                // Verify subsequent manipulation of new object 'X'.

                RESULT = primaryManipulator(&mX, 'Z', &oa);
                ASSERTV(true == RESULT.second);
                ASSERTV(SPEC, LENGTH + 1 == X.size());
                ASSERTV(SPEC, X != ZZ);

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(SPEC, 0 == fa.numBlocksInUse());
                ASSERTV(SPEC, 0 == da.numBlocksInUse());
                ASSERTV(SPEC, 0 == sa.numBlocksInUse());
                ASSERTV(SPEC, 0 == za.numBlocksInUse());
            }
        }
    }

    if (verbose)
        printf("\nTesting move constructor with injected exceptions.\n");
#if defined(BDE_BUILD_TARGET_EXC)
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC);

            if (verbose) {
                printf("\nFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
            bslma::TestAllocator za("different", veryVeryVeryVerbose);

            const bsls::Types::Int64 BB = oa.numBlocksTotal();
            const bsls::Types::Int64  B = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
            }

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                Obj mE(&scratch);
                if (oa.allocationLimit() >= 0
                 && oa.allocationLimit() <= TYPE_ALLOC) {
                    // We will throw on the reserveNodes so that source object
                    // will be unchanged on exception.
                    gg(&mE, SPEC);
                }
                // The else here is that the source object will be made empty
                // on exception.
                Obj mZ(&za);  const Obj& Z = gg(&mZ, SPEC);
                ExceptionProctor<Obj> proctor(&Z, L_, MoveUtil::move(mE));

                Obj mX(MoveUtil::move(mZ), &oa);  const Obj& X = mX;

                proctor.release();

                if (veryVerbose) {
                    printf("\t\t\tException Case  :\n");
                    printf("\t\t\t\tObj : "); P(X);
                }

                ASSERTV(SPEC, X, ZZ, ZZ == X);
                ASSERTV(SPEC, 0 == Z.size());
                ASSERTV(SPEC, Z.get_allocator() != X.get_allocator());

                // Manipulate source object 'Z' to ensure it is in a valid
                // state and is independent of 'X'.

                pair<Iter, bool> RESULT = primaryManipulator(&mZ, 'Z', &za);

                ASSERTV(SPEC, Z, true == RESULT.second);
                ASSERTV(SPEC, Z, 1    == Z.size());
                ASSERTV(SPEC, Z, ZZ, Z != ZZ);
                ASSERTV(SPEC, X, ZZ, X == ZZ);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            const bsls::Types::Int64 AA = oa.numBlocksTotal();
            const bsls::Types::Int64  A = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
            }

            if (0 == LENGTH) {
                ASSERTV(SPEC, BB + 0 == AA);
                ASSERTV(SPEC,  B + 0 ==  A);
            }
            else {
                const int TL = static_cast<int>(TYPE_ALLOC * LENGTH);
                const int TYPE_ALLOCS = (TL + 1) * (TL + 2) / 2;
                ASSERTV(SPEC, BB, AA, TYPE_ALLOCS, BB + TYPE_ALLOCS == AA);
                ASSERTV(SPEC, B + 0 == A);
            }
        }
    }
#endif  // BDE_BUILD_TARGET_EXC
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase26()
{
    // ------------------------------------------------------------------------
    // TESTING STANDARD INTERFACE COVERAGE
    //
    // Concerns:
    //: 1 The type provides the full interface defined by the section
    //:   '[map.overview]' in the C++11 standard, with exceptions for methods
    //:   that require C+11 compiler support.
    //
    // Plan:
    //: 1 Invoke each constructor defined by the standard.  (C-1)
    //:
    //: 2 For each method and free function defined in the standard, use the
    //:   respective address of the method to initialize function pointers
    //:   having the appropriate signatures and return type for that method
    //:   according to the standard.  (C-1)
    //
    // Testing:
    //   CONCERN: The type provides the full interface defined by the standard.
    // ------------------------------------------------------------------------

    // 23.4.4.2, construct/copy/destroy

    // explicit map(const Compare& comp = Compare(), const Allocator& =
    // Allocator());

    bsl::map<KEY, VALUE, COMP, StlAlloc> A((COMP()), StlAlloc());

    // template <class InputIterator> map(InputIterator first, InputIterator
    // last, const Compare& comp = Compare(), const Allocator& = Allocator());

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    TestValues values("ABC", &scratch);
    bsl::map<KEY, VALUE, COMP, StlAlloc>
                           B(values.begin(), values.end(), COMP(), StlAlloc());

    {
        TestValues values("DEF", &scratch);
        bsl::map<KEY, VALUE, COMP, StlAlloc>
                           B(values.begin(), values.end(), StlAlloc());
    }

    // map(const map<Key, T, Compare, Allocator>& x);
    bsl::map<KEY, VALUE, COMP, StlAlloc> C(B);

    // C++11 only:
    // map(map<Key, T, Compare, Allocator>&& x);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    bsl::map<KEY, VALUE, COMP, StlAlloc>
                     D(MoveUtil::move(bsl::map<KEY, VALUE, COMP, StlAlloc>()));
#else
    bsl::map<KEY, VALUE, COMP, StlAlloc> dummyD;
    bsl::map<KEY, VALUE, COMP, StlAlloc> D(MoveUtil::move(dummyD));
#endif

    // explicit map(const Allocator&);
    bsl::map<KEY, VALUE, COMP, StlAlloc> E((StlAlloc()));

    // map(const map&, const Allocator&);
    bsl::map<KEY, VALUE, COMP, StlAlloc> F(B, StlAlloc());

    // C++11 only:
    // map(map&&, const Allocator&);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    bsl::map<KEY, VALUE, COMP, StlAlloc>
                     G(MoveUtil::move(bsl::map<KEY, VALUE, COMP, StlAlloc>()),
                     StlAlloc());
#else
    bsl::map<KEY, VALUE, COMP, StlAlloc> dummyG;
    bsl::map<KEY, VALUE, COMP, StlAlloc> G(MoveUtil::move(dummyG), StlAlloc());
#endif

    // C++11 only:
    // map(initializer_list<value_type>, const Compare& = Compare(),
    //                                   const Allocator& = Allocator());
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VERSION != 1800
    // MSVC cl 18.00 fails to compile for KEY/VALUE int/int or char/char.
    bsl::map<KEY, VALUE, COMP, StlAlloc> H({});
#endif
#endif

    // ~map();  (destructor always exist)

    // map<Key, T, Compare, Allocator>& operator=(
    //                               const map<Key, T, Compare, Allocator>& x);
    Obj& (Obj::*operatorAg)(const Obj&) = &Obj::operator=;
    (void)operatorAg;  // quash potential compiler warning

    {
        using namespace bsl;
        // map<Key, T, Compare, Allocator>& operator=(
        //                                map<Key, T, Compare, Allocator>&& x);
        Obj& (Obj::*operatorMAg)(bslmf::MovableRef<Obj>) = &Obj::operator=;
        (void)operatorMAg;  // quash potential compiler warning

        // C++11 only:
        // map& operator=(initializer_list<value_type>);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
        Obj& (Obj::*operatorILAg)(
            std::initializer_list<typename Obj::value_type>) = &Obj::operator=;
        (void)operatorILAg;  // quash potential compiler warning
#endif
    }

    // allocator_type get_allocator() const noexcept;
    typename Obj::allocator_type (Obj::*methodGetAllocator)() const =
                                                           &Obj::get_allocator;
    (void)methodGetAllocator;

    // iterators

    // iterator begin() noexcept;
    typename Obj::iterator (Obj::*methodBegin)() = &Obj::begin;
    (void)methodBegin;

    // const_iterator begin() const noexcept;
    typename Obj::const_iterator (Obj::*methodBeginConst)() const =
                                                                   &Obj::begin;
    (void)methodBeginConst;

    // iterator end() noexcept;
    typename Obj::iterator (Obj::*methodEnd)() = &Obj::end;
    (void)methodEnd;

    // const_iterator end() const noexcept;
    typename Obj::const_iterator (Obj::*methodEndConst)() const = &Obj::end;
    (void)methodEndConst;

    // reverse_iterator rbegin() noexcept;
    typename Obj::reverse_iterator (Obj::*methodRbegin)() = &Obj::rbegin;
    (void)methodRbegin;

    // const_reverse_iterator rbegin() const noexcept;
    typename Obj::const_reverse_iterator (Obj::*methodRbeginConst)() const =
                                                                  &Obj::rbegin;
    (void)methodRbeginConst;

    // reverse_iterator rend() noexcept;
    typename Obj::reverse_iterator (Obj::*methodRend)() = &Obj::rend;
    (void)methodRend;

    // const_reverse_iterator rend() const noexcept;
    typename Obj::const_reverse_iterator (Obj::*methodRendConst)() const =
                                                                    &Obj::rend;
    (void)methodRendConst;

    // const_iterator cbegin() const noexcept;
    typename Obj::const_iterator (Obj::*methodCbegin)() const = &Obj::cbegin;
    (void)methodCbegin;

    // const_iterator cend() const noexcept;
    typename Obj::const_iterator (Obj::*methodCend)() const = &Obj::cend;
    (void)methodCend;

    // const_reverse_iterator crbegin() const noexcept;
    typename Obj::const_reverse_iterator (Obj::*methodCrbegin)() const =
                                                                 &Obj::crbegin;
    (void)methodCrbegin;

    // const_reverse_iterator crend() const noexcept;
    typename Obj::const_reverse_iterator (Obj::*methodCrend)() const =
                                                                   &Obj::crend;
    (void)methodCrend;

    // capacity

    // bool empty() const noexcept;
    bool (Obj::*methodEmpty)() const = &Obj::empty;
    (void)methodEmpty;

    // size_type size() const noexcept;
    typename Obj::size_type (Obj::*methodSize)() const = &Obj::size;
    (void)methodSize;

    // size_type max_size() const noexcept;
    typename Obj::size_type (Obj::*methodMaxSize)() const = &Obj::max_size;
    (void)methodMaxSize;

    // 23.4.4.3, element access

    // T& operator[](const key_type& x);
    VALUE& (Obj::*operatorIdx)(const typename Obj::key_type&) =
                                                              &Obj::operator[];
    (void)operatorIdx;

    // C++11 only:
    // T& operator[](key_type&& x);
    VALUE& (Obj::*operatorMIdx)(bslmf::MovableRef<typename Obj::key_type>) =
                                                              &Obj::operator[];
    (void)operatorMIdx;

    // T& at(const key_type& x);
    VALUE& (Obj::*methodAt)(const typename Obj::key_type&) = &Obj::at;
    (void)methodAt;

    // const T& at(const key_type& x) const;
    const VALUE& (Obj::*methodAtConst)(const typename Obj::key_type&) const =
                                                                      &Obj::at;
    (void)methodAtConst;

    // 23.4.4.4, modifiers

    TestValues empValues("AB", &scratch);

    // C++11 only:
    // template <class... Args> pair<iterator, bool> emplace(Args&&... args);
    E.emplace(empValues[0]);

    // C++11 only:
    // template <class... Args>
    // iterator emplace_hint(const_iterator position, Args&&... args);
    E.emplace_hint(E.end(), empValues[1]);

    // pair<iterator, bool> insert(const value_type& x);

    pair<typename Obj::iterator, bool> (Obj::*methodInsert)(
                               const typename Obj::value_type&) = &Obj::insert;
    (void)methodInsert;

    // C++11 only:
    // template <class P> pair<iterator, bool> insert(P&& x);
#if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VERSION != 1800
    // MSVC cl 18.00 fails to compile for KEY/VALUE int/int or char/char.
    pair<typename Obj::iterator, bool> (Obj::*methodInsert2)(
                 BSLS_COMPILERFEATURES_FORWARD_REF(typename Obj::value_type)) =
                                                                  &Obj::insert;
    (void)methodInsert2;
#endif

    // iterator insert(const_iterator position, const value_type& x);
    typename Obj::iterator (Obj::*methodInsert3)(
        typename Obj::const_iterator, const typename Obj::value_type&) =
                                                                  &Obj::insert;
    (void)methodInsert3;

    // C++11 only:
    // template <class P> iterator insert(const_iterator position, P&&);
#if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VERSION != 1800
    // MSVC cl 18.00 fails to compile for KEY/VALUE int/int or char/char.
    typename Obj::iterator (Obj::*methodInsert4)(
                 typename Obj::const_iterator,
                 BSLS_COMPILERFEATURES_FORWARD_REF(typename Obj::value_type)) =
                                                                  &Obj::insert;
    (void)methodInsert4;
#endif

    // template <class InputIterator>
    // void insert(InputIterator first, InputIterator last);
    void (Obj::*methodInsert5)(
                typename Obj::iterator, typename Obj::iterator) = &Obj::insert;
    (void)methodInsert5;

    // C++11 only:
    // void insert(initializer_list<value_type>);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    void (Obj::*methodInsert6)(std::initializer_list<typename Obj::value_type>)
                                                                = &Obj::insert;
    (void)methodInsert6;
#endif

    // iterator erase(const_iterator position);
    typename Obj::iterator (Obj::*methodErase)(typename Obj::const_iterator) =
                                                                   &Obj::erase;
    (void)methodErase;

    // size_type erase(const key_type& x);
    typename Obj::size_type (Obj::*methodErase2)(
        const typename Obj::key_type&) = &Obj::erase;
    (void)methodErase2;

    // iterator erase(const_iterator first, const_iterator last);
    typename Obj::iterator (Obj::*methodErase3)(
        typename Obj::const_iterator, typename Obj::const_iterator) =
                                                                   &Obj::erase;
    (void)methodErase3;

    // void swap(map<Key, T, Compare, Allocator>&);
    void (Obj::*methodSwap)(Obj&) = &Obj::swap;
    (void)methodSwap;

    // void clear() noexcept;
    void (Obj::*methodClear)() = &Obj::clear;
    (void)methodClear;

    // observers

    // key_compare key_comp() const;
    typename Obj::key_compare (Obj::*methodKeyComp)() const = &Obj::key_comp;
    (void)methodKeyComp;

    // value_compare value_comp() const;
    typename Obj::value_compare (Obj::*methodValueComp)() const =
                                                           &Obj::value_comp;
    (void)methodValueComp;

    // 23.4.4.5, map operations

    // iterator find(const key_type& x);
    typename Obj::iterator (Obj::*methodFind)(const typename Obj::key_type&) =
                                                                    &Obj::find;
    (void)methodFind;

    // const_iterator find(const key_type& x) const;
    typename Obj::const_iterator (Obj::*methodFindConst)(
                             const typename Obj::key_type&) const = &Obj::find;
    (void)methodFindConst;

    // size_type count(const key_type& x) const;
    typename Obj::size_type (Obj::*methodCount)(
                            const typename Obj::key_type&) const = &Obj::count;
    (void)methodCount;

    // iterator lower_bound(const key_type& x);
    typename Obj::iterator (Obj::*methodLowerBound)(
                            const typename Obj::key_type&) = &Obj::lower_bound;
    (void)methodLowerBound;

    // const_iterator lower_bound(const key_type& x) const;
    typename Obj::const_iterator (Obj::*methodLowerBoundConst)(
                      const typename Obj::key_type&) const = &Obj::lower_bound;
    (void)methodLowerBoundConst;

    // iterator upper_bound(const key_type& x);
    typename Obj::iterator (Obj::*methodUpperBound)(
                            const typename Obj::key_type&) = &Obj::upper_bound;
    (void)methodUpperBound;

    // const_iterator upper_bound(const key_type& x) const;
    typename Obj::const_iterator (Obj::*methodUpperBoundConst)(
                      const typename Obj::key_type&) const = &Obj::upper_bound;
    (void)methodUpperBoundConst;

    // pair<iterator, iterator> equal_range(const key_type& x);
    pair<typename Obj::iterator, typename Obj::iterator> (
        Obj::*methodEqualRange)(const typename Obj::key_type&) =
                                                             &Obj::equal_range;
    (void)methodEqualRange;

    // pair<const_iterator, const_iterator> equal_range(const key_type& x)
    // const;
    pair<typename Obj::const_iterator, typename Obj::const_iterator> (
        Obj::*methodEqualRangeConst)(const typename Obj::key_type&) const =
                                                             &Obj::equal_range;
    (void)methodEqualRangeConst;

    using namespace bsl;

    // template <class Key, class T, class Compare, class Allocator>
    // bool operator==(const map<Key, T, Compare, Allocator>& x,
    //                 const map<Key, T, Compare, Allocator>& y);

    bool (*operatorEq)(const Obj&, const Obj&) = operator==;
    (void)operatorEq;

    // template <class Key, class T, class Compare, class Allocator>
    // bool operator!=(const map<Key, T, Compare, Allocator>& x,
    //                 const map<Key, T, Compare, Allocator>& y);

    bool (*operatorNe)(const Obj&, const Obj&) = operator!=;
    (void)operatorNe;

    // template <class Key, class T, class Compare, class Allocator>
    // bool operator< (const map<Key, T, Compare, Allocator>& x,
    //                 const map<Key, T, Compare, Allocator>& y);

    bool (*operatorLt)(const Obj&, const Obj&) = operator<;
    (void)operatorLt;

    // template <class Key, class T, class Compare, class Allocator>
    // bool operator> (const map<Key, T, Compare, Allocator>& x,
    //                 const map<Key, T, Compare, Allocator>& y);

    bool (*operatorGt)(const Obj&, const Obj&) = operator>;
    (void)operatorGt;

    // template <class Key, class T, class Compare, class Allocator>
    // bool operator>=(const map<Key, T, Compare, Allocator>& x,
    //                 const map<Key, T, Compare, Allocator>& y);

    bool (*operatorGe)(const Obj&, const Obj&) = operator>=;
    (void)operatorGe;

    // template <class Key, class T, class Compare, class Allocator>
    // bool operator<=(const map<Key, T, Compare, Allocator>& x,
    //                 const map<Key, T, Compare, Allocator>& y);

    bool (*operatorLe)(const Obj&, const Obj&) = operator<=;
    (void)operatorLe;

    // specialized algorithms

    // template <class Key, class T, class Compare, class Allocator>
    // void swap(map<Key, T, Compare, Allocator>& x,
    //           map<Key, T, Compare, Allocator>& y);

    void (*functionSwap)(Obj&, Obj&) = &swap;
    (void)functionSwap;
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase25()
{
    // ------------------------------------------------------------------------
    // TESTING CONSTRUCTORS OF A TEMPLATE WRAPPER CLASS
    //
    // Concerns:
    //: 1 The constructors of a parameterized wrapper around the container
    //:   compile.
    //
    // Plan:
    //: 1 Invoke each constructor of a template class that wraps the container.
    //:   (C-1)
    //
    // Testing:
    //   CONCERN: The constructors of a template wrapper class compile.
    // ------------------------------------------------------------------------

    // The following may fail to compile on AIX.

    TemplateWrapper<KEY,
                    VALUE,
                    DummyComparator,
                    DummyAllocator<pair<const KEY, VALUE> > > obj1;
    (void)obj1;

    // The following compiles because the copy constructor doesn't have any
    // default arguments.

    TemplateWrapper<KEY,
                    VALUE,
                    DummyComparator,
                    DummyAllocator<pair<const KEY, VALUE> > > obj2(obj1);
    (void)obj2;

    // The following also compiles, most likely because the constructor is
    // parameterized.

    typename Obj::value_type array[1];
    TemplateWrapper<KEY,
                    VALUE,
                    DummyComparator,
                    DummyAllocator<pair<const KEY, VALUE> > > obj3(array,
                                                                   array);
    (void)obj3;
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase24()
{
    // ------------------------------------------------------------------------
    // TESTING ELEMENTAL ACCESS
    //
    // Concerns:
    //: 1 All elemental access methods return the value associated with the
    //:   key.
    //:
    //: 2 'operator[]' and 'at' can be used to set the value.
    //:
    //: 3 'at' throws 'std::out_of_range' exception if the key does not exist.
    //:
    //: 4 'operator[]' creates a default constructed value if the key does not
    //:   exist in the object.
    //:
    //: 5 Any memory allocations come from the object's allocator.
    //:
    //: 6 'operator[]' is exception neutral.
    //
    // Plan:
    //: 1 Using the table-driven technique, for each set of key-value pairs:
    //:
    //:   1 For each key-value pair in the object:
    //:
    //:     1 Verify 'operator[]' and 'at' returns the expected 'VALUE'.
    //:
    //:     2 Verify no memory is allocated.
    //:
    //:     3 Set the value to something different with 'operator[]' and verify
    //:       the value is changed.  Set the value back to its original value.
    //:
    //:     4 Repeat P-1.1.3 with the 'at' method.
    //:
    //:     5 Verify memory use did not increase.
    //:
    //:   2 Verify no memory is allocated from the default allocator.
    //:
    //:   3 Invoke the 'const' version of 'at' method with a key that does not
    //:     exist in the container.
    //:
    //:   4 Verify 'std::out_of_range' is thrown.
    //:
    //:   5 Repeat P-1.3..4 with the non-'const' version of the 'at' method.
    //:
    //:   6 Invoke 'operator[]' using the out-of-range key in the presence of
    //:     exceptions.
    //:
    //:   7 Verify that a default 'VALUE' is created.
    //:
    //:   8 Verify memory usage is as expected.
    //
    // Testing:
    //   VALUE& operator[](const key_type& key);
    //   VALUE& at(const key_type& key);
    //   const VALUE& at(const key_type& key) const;
    // ------------------------------------------------------------------------

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const KEY   ZK = TestValues("y")[0].first;   // A value not in any spec.
    const VALUE ZV = TestValues("z")[0].second;  // A value not in any spec.

    if (verbose) printf("Testing elemental access.");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const size_t      LENGTH = strlen(DATA[ti].d_results_p);
            const TestValues  VALUES(DATA[ti].d_results_p);

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            if (veryVeryVerbose) { T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const KEY&   K = VALUES[tj].first;
                const VALUE& V = VALUES[tj].second;

                bslma::TestAllocatorMonitor oam(&oa);

                ASSERTV(LINE, V, mX[K],    V == mX[K]);
                ASSERTV(LINE, V, mX.at(K), V == mX.at(K));
                ASSERTV(LINE, V, X.at(K),  V == X.at(K));

                ASSERTV(LINE, tj, oam.isTotalSame());
                ASSERTV(LINE, tj, oam.isInUseSame());

                // Verify 'operator[]' and 'at' can be used to set the value.

                mX[K] = ZV;
                ASSERTV(LINE, ZV == X.find(K)->second);

                mX[K] = V;
                ASSERTV(LINE,  V == X.find(K)->second);

                mX.at(K) = ZV;
                ASSERTV(LINE, ZV == X.find(K)->second);

                mX.at(K) = V;
                ASSERTV(LINE,  V == X.find(K)->second);

                ASSERTV(LINE, tj, oam.isInUseSame());
            }

            ASSERTV(LINE, 0 == da.numBlocksTotal());

#if defined(BDE_BUILD_TARGET_EXC)
            if (veryVeryVerbose) printf("Test correct exception is thrown.\n");
            {
                bool exceptionCaught = false;
                try {
                    X.at(ZK);
                }
                catch (std::out_of_range) {
                    exceptionCaught = true;
                }
                ASSERTV(LINE, true == exceptionCaught);
            }
            {
                bool exceptionCaught = false;
                try {
                    mX.at(ZK);
                }
                catch (std::out_of_range) {
                    exceptionCaught = true;
                }
                ASSERTV(LINE, true == exceptionCaught);
            }

            // Constructing default 'VALUE' to compare with.  Note, that we
            // construct default value this way to support some types that do
            // not meet C++ requirement of 'default-insertable'.
            bslma::TestAllocator value("value", veryVeryVeryVerbose);
            ALLOC                xvalue(&value);

            bsls::ObjectBuffer<VALUE> d;
            bsl::allocator_traits<ALLOC>::construct(xvalue, d.address());
            bslma::DestructorGuard<VALUE> defaultValueGuard(d.address());

            const VALUE& D = d.object();

            if (veryVeryVerbose)
                printf("Test 'operator[]' on out-of-range key.\n");
            {
                const size_t SIZE = X.size();

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionProctor<Obj> proctor(&X, L_, &scratch);

                    mX[ZK];

                    proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, D == X.find(ZK)->second);
                ASSERTV(LINE, SIZE, SIZE + 1 == X.size());
            }
#endif

            ASSERTV(LINE, 0 == da.numBlocksInUse());
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase23()
{
    // ------------------------------------------------------------------------
    // TESTING TYPE TRAITS
    //
    // Concerns:
    //: 1 The object has the necessary type traits.
    //
    // Plan:
    //: 1 Use 'BSLMF_ASSERT' to verify all the type traits exists.  (C-1)
    //
    // Testing:
    //   CONCERN: 'map' has the necessary type traits.
    // ------------------------------------------------------------------------

    // Verify map defines the expected traits.

    BSLMF_ASSERT((1 == bslalg::HasStlIterators<Obj>::value));
    BSLMF_ASSERT((1 == bslma::UsesBslmaAllocator<Obj>::value));

    // Verify the bslma-allocator trait is not defined for non
    // bslma-allocators.

    BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<bsl::map<KEY, VALUE,
                                          std::less<KEY>, StlAlloc> >::value));

    // Verify map does not define other common traits.

    BSLMF_ASSERT((0 == bsl::is_trivially_copyable<Obj>::value));

    BSLMF_ASSERT((0 == bslmf::IsBitwiseEqualityComparable<Obj>::value));

    BSLMF_ASSERT((0 == bslmf::IsBitwiseMoveable<Obj>::value));

    BSLMF_ASSERT((0 == bslmf::HasPointerSemantics<Obj>::value));

    BSLMF_ASSERT((0 == bsl::is_trivially_default_constructible<Obj>::value));
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase22()
{
    // ------------------------------------------------------------------------
    // TESTING STL ALLOCATOR
    //
    // Concerns:
    //: 1 A standard compliant allocator can be used instead of
    //:   'bsl::allocator'.
    //:
    //: 2 Methods that uses the allocator (e.g., variations of constructor,
    //:   'insert' and 'swap') can successfully populate the object.
    //:
    //: 3 'KEY' types that allocate memory uses the default allocator instead
    //:   of the object allocator.
    //:
    //: 4 Every object releases any allocated memory at destruction.
    //
    // Plan:
    //: 1 Using a loop base approach, create a list of specs and their
    //:   expected value.  For each spec:
    //:
    //:   1 Create an object using a standard allocator through multiple ways,
    //:     including: range-based constructor, copy constructor, range-based
    //:     insert, multiple inserts, and swap.
    //:
    //:   2 Verify the value of each objects is as expected.
    //:
    //:   3 For types that allocate memory, verify memory for the elements
    //:     comes from the default allocator.
    //
    // Testing:
    //   CONCERN: 'map' is compatible with standard allocators.
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value +
                           bslma::UsesBslmaAllocator<VALUE>::value;

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    typedef bsl::map<KEY, VALUE, COMP, StlAlloc> Obj;

    // bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    StlAlloc scratch;

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec_p;
        const ptrdiff_t   LENGTH = strlen(DATA[ti].d_results_p);
        const TestValues  EXP(DATA[ti].d_results_p, scratch);

        ASSERT(0 <= LENGTH);

        TestValues CONT(SPEC, scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        {
            Obj mX(BEGIN, END);  const Obj& X = mX;

            ASSERTV(StlAlloc() == X.get_allocator());

            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(LINE, da.numBlocksInUse(),
                    TYPE_ALLOC * LENGTH == da.numBlocksInUse());

            Obj mY(X);  const Obj& Y = mY;

            ASSERTV(StlAlloc() == Y.get_allocator());

            ASSERTV(LINE, 0 == verifyContainer(Y, EXP, LENGTH));
            ASSERTV(LINE, da.numBlocksInUse(),
                    2 * TYPE_ALLOC * LENGTH == da.numBlocksInUse());

            Obj mZ;  const Obj& Z = mZ;

            ASSERTV(StlAlloc() == Z.get_allocator());

            mZ.swap(mX);

            ASSERTV(LINE, 0 == verifyContainer(Z, EXP, LENGTH));
            ASSERTV(LINE, da.numBlocksInUse(),
                    2 * TYPE_ALLOC * LENGTH == da.numBlocksInUse());
        }

        CONT.resetIterators();

        {
            Obj mX;  const Obj& X = mX;
            mX.insert(BEGIN, END);

            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(LINE, da.numBlocksInUse(),
                    TYPE_ALLOC * LENGTH == da.numBlocksInUse());
        }

        CONT.resetIterators();

        {
            Obj mX;  const Obj& X = mX;
            for (size_t tj = 0; tj < CONT.size(); ++tj) {
                pair<Iter, bool> RESULT = mX.insert(CONT[tj]);

                if (RESULT.second) {
                    ASSERTV(LINE, tj, LENGTH, CONT[tj] == *(RESULT.first));
                }
            }
            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(LINE, da.numBlocksInUse(),
                    TYPE_ALLOC * LENGTH == da.numBlocksInUse());
        }

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

    // IBM empty class swap bug test

    {
        typedef bsl::map<int, int, std::less<int>, StlAlloc> TestObj;
        TestObj mX;
        mX.insert(typename TestObj::value_type(1, 1));
        TestObj mY;
        mY = mX;
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase21()
{
    // ------------------------------------------------------------------------
    // TESTING COMPARATOR
    //
    // Concerns:
    //: 1 Both a functor and a function type can be used.
    //:
    //: 2 If a comparator is not supplied, it defaults to 'std::less'.
    //:
    //: 3 The comparator is set correctly.
    //:
    //: 4 'key_comp' returns the comparator that was supplied at construction;
    //:   'value_comp' returns a nested 'value_compare' object holding a copy
    //:   of the originally supplied comparator.
    //:
    //: 5 The supplied comparator is used in all key-comparison operations
    //:   instead of 'operator<' *except* for the free comparison operators.
    //:
    //: 6 The comparator is properly propagated on copy construction, move
    //:   construction, copy assignment, move assignment, and swap.
    //:
    //: 7 A functor with a non-'const' function call operator can be used --
    //:   any non-'const' operation on a map that utilizes the comparator can
    //:   be invoked from a reference providing non-modifiable access to the
    //:   map.
    //
    // Plan:
    //: 1 Create a default object and verify that the comparator is
    //:   'std::less'.  (C-2)
    //:
    //: 2 Create an object with a function-type comparator.
    //:
    //: 3 Create an object with a functor-type comparator with each of the
    //:   constructors.  (C-1, 3..4)
    //:
    //: 4 Using the table-driven technique, specify a set of (unique) valid
    //:   object values.
    //:
    //: 5 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-4:
    //:
    //:   1 Create a comparator object using the greater-than operator for
    //:     comparisons.
    //:
    //:   2 Create an object having the value 'V' using the range constructor
    //:     passing in the comparator created in P-5.1.  Verify the object's
    //:     value.  (P-5)
    //:
    //:   3 Copy construct an object from the object created in P-5.2, and
    //:     verify that the comparator compares equal to the one created in
    //:     P-5.1.  (P-6)
    //:
    //:   4 Default construct an object and assign to the object from the one
    //:     created in P-5.2.  Verify that the comparator of the new object
    //:     compares equal to the one created in P-5.1.  (P-6)
    //:
    //:   5 Default construct an object and swap this object with the one
    //:     created in P-5.2.  Verify that the comparator of the new object
    //:     compares equal to the one created in P-5.1, and the original object
    //:     has a default-constructed comparator.  (P-6)
    //:
    //: 6 Repeat P-5 except with a comparator having a non-'const' function
    //:   call operator.  (P-7)
    //
    // Testing:
    //   key_compare key_comp() const;
    //   value_compare value_comp() const;
    // ------------------------------------------------------------------------

// TBD Also test function pointer for each constructor; 0 for null pointer most
// likely to cause problems, especially with stateful comparators (as well as
// 'bslma::Allocator *).
//
// 'value_comp' accessor not nested.
//
// Comparator propagation not tested with move construction, move assignment.

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    {
        bsl::map<int, int> X;

        std::less<int>                    keyComparator   = X.key_comp();
        bsl::map<int, int>::value_compare valueComparator = X.value_comp();

        (void)keyComparator;    // quash potential compiler warning
        (void)valueComparator;  // quash potential compiler warning
    }

    {
        typedef bool (*ComparatorFunction)(const KEY&, const KEY& rhs);
        bsl::map<KEY, VALUE, ComparatorFunction> X(&lessThanFunction<KEY>);
        ASSERTV((ComparatorFunction)&lessThanFunction<KEY> == X.key_comp());
    }

    static const int ID[] = { 0, 1, 2 };
    const int NUM_ID = static_cast<const int>(sizeof ID / sizeof *ID);

    for (int ti = 0; ti < NUM_ID; ++ti) {
        const COMP C(ti);
        {
            const Obj X(C);
            ASSERTV(ti, C.id() == X.key_comp().id());
            ASSERTV(ti,      0 == X.key_comp().count());
        }
        {
            const Obj X(C, &scratch);
            ASSERTV(ti, C.id() == X.key_comp().id());
            ASSERTV(ti,      0 == X.key_comp().count());
        }
        {
            const Obj X(C, (bslma::Allocator *)0);
            ASSERTV(ti, C.id() == X.key_comp().id());
            ASSERTV(ti,      0 == X.key_comp().count());
        }
        {
            const Obj X((typename Obj::value_type *)0,
                        (typename Obj::value_type *)0,
                        C,
                        &scratch);
            ASSERTV(ti, C.id() == X.key_comp().id());
            ASSERTV(ti,      0 == X.key_comp().count());
        }
        {
            const Obj X((typename Obj::value_type *)0,
                        (typename Obj::value_type *)0,
                        C,
                        (bslma::Allocator *)0);
            ASSERTV(ti, C.id() == X.key_comp().id());
            ASSERTV(ti,      0 == X.key_comp().count());
        }
    }

    static const struct {
        int         d_line;       // source line number
        const char *d_spec_p;     // spec
        const char *d_results_p;
    } DATA[] = {
        { L_,  "",           ""          },
        { L_,  "A",          "A"         },
        { L_,  "ABC",        "CBA"       },
        { L_,  "ACBD",       "DCBA"      },
        { L_,  "BCDAE",      "EDCBA"     },
        { L_,  "GFEDCBA",    "GFEDCBA"   },
        { L_,  "ABCDEFGH",   "HGFEDCBA"  },
        { L_,  "BCDEFGHIA",  "IHGFEDCBA" }
    };
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec_p;
        const size_t      LENGTH = strlen(DATA[ti].d_results_p);
        const TestValues  EXP(DATA[ti].d_results_p, &scratch);

        TestValues CONT(SPEC, &scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        const COMP C(1, false);  // create comparator that uses greater than

        {
            Obj mW(BEGIN, END, C);  const Obj& W = mW;

            ASSERTV(LINE, 0 == verifyContainer(W, EXP, LENGTH));
            ASSERTV(LINE, C == W.key_comp());

            Obj mX(W);  const Obj& X = mX;

            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(LINE, C == X.key_comp());

            Obj mY;  const Obj& Y = mY;
            mY = mW;

            ASSERTV(LINE, 0 == verifyContainer(Y, EXP, LENGTH));
            ASSERTV(LINE, C == Y.key_comp());

            Obj mZ;  const Obj& Z = mZ;
            mZ.swap(mW);

            ASSERTV(LINE, 0      == verifyContainer(Z, EXP, LENGTH));
            ASSERTV(LINE, C      == Z.key_comp());
            ASSERTV(LINE, COMP() == W.key_comp());
        }

        CONT.resetIterators();

        {
            Obj mX(C);  const Obj& X = mX;
            mX.insert(BEGIN, END);

            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
        }

        CONT.resetIterators();

        {
            Obj mX(C);  const Obj& X = mX;
            for (size_t tj = 0; tj < CONT.size(); ++tj) {
                pair<Iter, bool> RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH, CONT[tj] == *(RESULT.first));
            }
            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
        }

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec_p;
        const size_t      LENGTH = strlen(DATA[ti].d_results_p);
        const TestValues  EXP(DATA[ti].d_results_p, &scratch);

        TestValues CONT(SPEC, &scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        const NonConstComp C(1, false);  // create non-'const' comparator that
                                         // uses the greater-than operator
        typedef bsl::map<KEY, VALUE, NonConstComp> ObjNCC;

        {
            ObjNCC mW(BEGIN, END, C);  const ObjNCC& W = mW;

            ASSERTV(LINE, 0 == verifyContainer(W, EXP, LENGTH));
            ASSERTV(LINE, C == W.key_comp());

            ObjNCC mX(W);  const ObjNCC& X = mX;

            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(LINE, C == X.key_comp());

            ObjNCC mY;  const ObjNCC& Y = mY;
            mY = W;

            ASSERTV(LINE, 0 == verifyContainer(Y, EXP, LENGTH));
            ASSERTV(LINE, C == Y.key_comp());

            ObjNCC mZ;  const ObjNCC& Z = mZ;
            mZ.swap(mW);

            ASSERTV(LINE, 0              == verifyContainer(Z, EXP, LENGTH));
            ASSERTV(LINE, C              == Z.key_comp());
            ASSERTV(LINE, NonConstComp() == W.key_comp());
        }

        CONT.resetIterators();

        {
            ObjNCC mX(C);  const ObjNCC& X = mX;
            mX.insert(BEGIN, END);

            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
        }

        CONT.resetIterators();

        {
            ObjNCC mX(C);  const ObjNCC& X = mX;
            for (size_t tj = 0; tj < CONT.size(); ++tj) {
                pair<Iter, bool> RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH, CONT[tj] == *(RESULT.first));
            }
            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
        }

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase20()
{
    // ------------------------------------------------------------------------
    // TESTING 'max_size' and 'empty'
    //
    // Concerns:
    //: 1 'max_size' returns the 'max_size' of the supplied allocator.
    //:
    //: 2 'empty' returns 'true' only when the object is empty.
    //
    // Plan:
    //: 1 Run each function and verify the result.  (C-1..2)
    //
    // Testing:
    //   bool empty() const;
    //   size_type max_size() const;
    // ------------------------------------------------------------------------

    bslma::TestAllocator  oa(veryVeryVeryVerbose);

    if (verbose) printf("\tTesting 'max_size'.\n");
    {
        // This is the maximum value.  Any larger value would be cause for
        // potential bugs.

        Obj X;
        ALLOC a;
        ASSERTV(~(size_t)0 / sizeof(KEY) >= X.max_size());
        ASSERTV(a.max_size(), X.max_size(), a.max_size() == X.max_size());
    }

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;   // initial
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
    const int NUM_DATA = static_cast<const int>(sizeof DATA / sizeof *DATA);

    if (verbose) printf("\tTesting 'empty'.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE = DATA[ti].d_lineNum;
            const char *SPEC = DATA[ti].d_spec_p;

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            ASSERTV(LINE, SPEC, (0 == ti) == X.empty());

            mX.clear();

            ASSERTV(LINE, SPEC, true == X.empty());
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase19()
{
    // ------------------------------------------------------------------------
    // TESTING FREE COMPARISON OPERATORS
    //
    // Concerns:
    //: 1 'operator<' returns the lexicographic comparison on two containers.
    //:
    //: 2 Comparison operator uses 'operator<' on 'key_type' instead of the
    //:   supplied comparator.
    //:
    //: 3 'operator>', 'operator<=', and 'operator>=' are correctly tied to
    //:   'operator<'.  I.e., for two maps, 'a' and 'b':
    //:
    //:   1 '(a > b) == (b < a)'
    //:
    //:   2 '(a <= b) == !(b < a)'
    //:
    //:   3 '(a >= b) == !(a < b)'
    //
    // Plan:
    //: 1 For a variety of objects of different sizes and different values,
    //:   test that the comparison returns as expected.  (C-1..3)
    //
    // Testing:
    //   bool operator< (const map& lhs, const map& rhs);
    //   bool operator> (const map& lhs, const map& rhs);
    //   bool operator>=(const map& lhs, const map& rhs);
    //   bool operator<=(const map& lhs, const map& rhs);
    // ------------------------------------------------------------------------

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\nCompare each pair of similar and different"
                        " values (u, ua, v, va) in S X A X S X A"
                        " without perturbation.\n");
    {
        // Create first object.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec_p;
            const size_t      LENGTH1 = strlen(DATA[ti].d_results_p);

           if (veryVerbose) { T_ P_(LINE1) P_(INDEX1) P_(LENGTH1) P(SPEC1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&scratch);  const Obj& X = gg(&mX, SPEC1);

                ASSERTV(LINE1, X,   X == X);
                ASSERTV(LINE1, X, !(X != X));
            }

            // Create second object.
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec_p;
                const size_t      LENGTH2 = strlen(DATA[tj].d_results_p);

                if (veryVerbose) {
                              T_ T_ P_(LINE2) P_(INDEX2) P_(LENGTH2) P(SPEC2) }

                // Create two distinct test allocators, 'oax' and 'oay'.

                bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                // Map allocators above to objects 'X' and 'Y' below.

                Obj mX(&oax);  const Obj& X = gg(&mX, SPEC1);
                Obj mY(&oay);  const Obj& Y = gg(&mY, SPEC2);

                if (veryVerbose) { T_ T_ P_(X) P(Y); }

                // Verify value and no memory allocation.

                bslma::TestAllocatorMonitor oaxm(&oax);
                bslma::TestAllocatorMonitor oaym(&oay);

                const bool isLess   = INDEX1 <  INDEX2;
                const bool isLessEq = INDEX1 <= INDEX2;

                TestComparator<KEY>::disableFunctor();

                ASSERTV(LINE1, LINE2,  isLess   == (X < Y));
                ASSERTV(LINE1, LINE2, !isLessEq == (X > Y));
                ASSERTV(LINE1, LINE2,  isLessEq == (X <= Y));
                ASSERTV(LINE1, LINE2, !isLess   == (X >= Y));

                TestComparator<KEY>::enableFunctor();

                ASSERTV(LINE1, LINE2, oaxm.isTotalSame());
                ASSERTV(LINE1, LINE2, oaym.isTotalSame());
            }
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase18()
{
    // ------------------------------------------------------------------------
    // TESTING ERASE
    //
    // Concerns:
    //: 1 'erase' with iterators returns the iterator right after the erased
    //:   value(s).
    //:
    //: 2 'erase' with 'key' returns 1 if 'key' exist, and 0 with no other
    //:   effect otherwise.
    //:
    //: 3 Erased values are removed.
    //:
    //: 4 Erasing do not throw.
    //:
    //: 5 No memory is allocated.
    //:
    //: 6 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 For each distinct length, 'l':
    //:
    //:   1 For each value, v, that would be in the object with that length:
    //:
    //:     1 Create an object with length, 'l'.
    //:
    //:     2 Find 'v' to get its iterator
    //:
    //:     2 Erase 'v' with 'erase(const_iterator position)'.  (C-4)
    //:
    //:     2 Verify return value.  (C-1)
    //:
    //:     3 Verify value is erased with 'find'.  (C-3)
    //:
    //:     4 Verify no memory is allocated.  (C-5)
    //:
    //: 2 Repeat P-1 with 'erase(const key_type& key)' (C-2).
    //:
    //: 3 For range erase, call erase on all possible range of for each length,
    //:   'l' and verify result is as expected.
    //:
    //: 4 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid values, but not triggered for adjacent valid
    //:   ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
    //
    // Testing:
    //   iterator erase(const_iterator position);
    //   size_type erase(const key_type& key);
    //   iterator erase(const_iterator first, const_iterator last);
    // -----------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value +
                           bslma::UsesBslmaAllocator<VALUE>::value;

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\nTesting 'erase(pos)' on non-empty map.\n");
    {
        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const size_t      LENGTH = strlen(DATA[ti].d_results_p);
            const TestValues  VALUES(DATA[ti].d_results_p);

            if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

                CIter POS = X.find(VALUES[tj].first);
                CIter AFTER  = LENGTH - 1 == tj
                               ? X.end()
                               : X.find(VALUES[tj + 1].first);
                CIter BEFORE = 0 == tj
                               ? X.end()
                               : X.find(VALUES[tj - 1].first);

                ASSERTV(LINE, tj, POS != X.end());
                ASSERTV(LINE, tj, AFTER  != POS);
                ASSERTV(LINE, tj, BEFORE != POS);

                if (veryVerbose) { P(*POS); }

                bslma::TestAllocatorMonitor oam(&oa);

                const Iter R = mX.erase(POS);

                if (veryVeryVerbose) { T_ T_ P(X); }

                ASSERTV(LINE, tj, AFTER == R); // Check return value

                // Check the element does not exist

                ASSERTV(LINE, tj, X.end() == X.find(VALUES[tj].first));
                if (0 == tj) {
                    ASSERTV(LINE, tj, X.begin() == AFTER);
                }
                else {
                    ++BEFORE;
                    ASSERTV(LINE, tj, BEFORE == AFTER);
                }

                ASSERTV(LINE, tj, oam.isTotalSame());
                if (TYPE_ALLOC) {
                    ASSERTV(LINE, tj, oam.isInUseDown());
                }
                else {
                    ASSERTV(LINE, tj, oam.isInUseSame());
                }
                ASSERTV(LINE, tj, X.size(), LENGTH - 1 == X.size());
            }
        }
    }

    if (verbose) printf("\nTesting 'erase(key)' on non-empty map.\n");
    {
        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const size_t      LENGTH = strlen(DATA[ti].d_results_p);
            const TestValues  VALUES(DATA[ti].d_results_p);

            if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

                CIter AFTER  = LENGTH - 1 == tj
                               ? X.end()
                               : X.find(VALUES[tj + 1].first);
                CIter BEFORE = 0 == tj
                               ? X.end()
                               : X.find(VALUES[tj - 1].first);

                if (veryVerbose) { P(tj); }

                bslma::TestAllocatorMonitor oam(&oa);

                ASSERTV(LINE, tj, 1 == mX.erase(VALUES[tj].first));

                if (veryVeryVerbose) {
                    T_ T_ P(X);
                }

                // Check the element does not exist

                ASSERTV(LINE, tj, X.end() == X.find(VALUES[tj].first));
                if (0 == tj) {
                    ASSERTV(LINE, tj, X.begin() == AFTER);
                }
                else {
                    ++BEFORE;
                    ASSERTV(LINE, tj, BEFORE == AFTER);
                }

                ASSERTV(LINE, tj, oam.isTotalSame());
                if (TYPE_ALLOC) {
                    ASSERTV(LINE, tj, oam.isInUseDown());
                }
                else {
                    ASSERTV(LINE, tj, oam.isInUseSame());
                }
                ASSERTV(LINE, tj, X.size(), LENGTH - 1 == X.size());

                // Erase a non-existing element.
                ASSERTV(LINE, tj, 0 == mX.erase(VALUES[tj].first));
                ASSERTV(LINE, tj, X.size(), LENGTH - 1 == X.size());
            }
        }
    }

    if (verbose) printf("\nTesting 'erase(first, last)'.\n");
    {
        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const size_t      LENGTH = strlen(DATA[ti].d_results_p);
            const TestValues  VALUES(DATA[ti].d_results_p);

            if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0;  tj <= LENGTH; ++tj) {
                for (size_t tk = tj; tk <= LENGTH; ++tk) {
                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                    Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

                    CIter FIRST = LENGTH == tj
                                  ? X.end()
                                  : X.find(VALUES[tj].first);
                    CIter LAST  = LENGTH == tk
                                  ? X.end()
                                  : X.find(VALUES[tk].first);
                    const size_t NUM_ELEMENTS = tk - tj;

                    const CIter AFTER  = LAST;
                    const CIter BEFORE = 0 == tj
                                         ? X.end()
                                         : X.find(VALUES[tj - 1].first);

                    if (veryVerbose) {
                        if (FIRST != X.end()) {
                            P(*FIRST)
                        }
                        if (LAST != X.end()) {
                            P(*LAST);
                        }
                    }

                    bslma::TestAllocatorMonitor oam(&oa);

                    const Iter R = mX.erase(FIRST, LAST);
                    ASSERTV(LINE, tj, AFTER == R);  // Check return value.

                    if (veryVeryVerbose) {
                        T_ T_ P(X);
                    }

                    // Check the element does not exist.

                    if (0 == tj) {
                        ASSERTV(LINE, tj, tk, X.begin() == AFTER);
                    }
                    else {
                        CIter next = BEFORE;
                        ++next;
                        ASSERTV(LINE, tj, tk, AFTER == next);
                    }

                    ASSERTV(LINE, tj, oam.isTotalSame());
                    if (TYPE_ALLOC && 1 <= NUM_ELEMENTS) {
                        ASSERTV(LINE, tj, tk, oam.isInUseDown());
                    }
                    else {
                        ASSERTV(LINE, tj, tk, oam.isInUseSame());
                    }
                    ASSERTV(LINE, tj, tk, LENGTH,
                            LENGTH == X.size() + NUM_ELEMENTS);
                }
            }
        }
    }

    if (verbose) printf("\nNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        if (veryVerbose) printf("'erase'\n");
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mX;  const Obj& X = mX;

            pair<Iter, bool> RESULT = primaryManipulator(&mX, 'A', &scratch);
            ASSERTV(RESULT.second);

            (void)X;  // 'X' is not used in some build modes

            ASSERT_SAFE_FAIL(mX.erase(X.end()));
            ASSERT_SAFE_PASS(mX.erase(RESULT.first));
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase17()
{
    // ------------------------------------------------------------------------
    // TESTING RANGE 'insert'
    //
    // Concerns:
    //: 1 All values within the range [first, last) are inserted.
    //:
    //: 2 Each iterator is dereferenced only once.
    //:
    //: 3 Repeated values are ignored.
    //:
    //: 4 Any memory allocation is from the object allocator.
    //:
    //: 5 There is no temporary memory allocation from any allocator.
    //:
    //: 6 Inserting no elements allocates no memory.
    //:
    //: 7 Any memory allocation is exception neutral.
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a map of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value map, 'V') in the
    //:   table described in P-1:
    //:
    //:   1 Use the range constructor to create a object with part of the
    //:     elements in 'V'.
    //:
    //:   2 Insert the rest of 'V' under the presence of exceptions.  (C-7)
    //:
    //:   3 Verify the object's value.  (C-1..3)
    //:
    //:   4 If the range is empty, verify no memory is allocated  (C-6)
    //:
    //:   5 Verify no temporary memory is allocated.  (C-5)
    //:
    //:   6 Verify no memory is allocated from the default allocator (C-4)
    //
    // Testing:
    //   void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
    // ------------------------------------------------------------------------

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec_p;
        const size_t      LENGTH = strlen(DATA[ti].d_results_p);
        const TestValues  EXP(DATA[ti].d_results_p);

        TestValues CONT(SPEC);
        for (size_t tj = 0; tj <= CONT.size(); ++tj) {

            CONT.resetIterators();
            typename TestValues::iterator BEGIN = CONT.begin();
            typename TestValues::iterator MID   = CONT.index(tj);
            typename TestValues::iterator END   = CONT.end();

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(BEGIN, MID, COMP(), &oa);  const Obj& X = mX;

            bslma::TestAllocatorMonitor oam(&oa);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                CONT.resetIterators();

                mX.insert(MID, END);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            if (CONT.size() == tj) {
                ASSERTV(LINE, oam.isTotalSame());
            }
            ASSERTV(LINE, tj, 0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(LINE, tj, da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase16()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION-WITH-HINT OF SINGLE VALUE
    //
    // Concerns:
    //: 1 'insert' returns an iterator referring to the newly inserted element
    //:   if it did not already exists, and to the existing element if it did.
    //:
    //: 2 A new element is added to the container if the element did not
    //:   already exist, and the order of the container remains correct.
    //:
    //: 3 Inserting with the correct hint places the new element right before
    //:   the hint.
    //:
    //: 4 Inserting with the correct hint requires no more than 2 comparisons.
    //:
    //: 5 Incorrect hint will be ignored and 'insert' will proceed as if the
    //:   hint is not supplied.
    //:
    //: 6 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 7 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 For insertion we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 For each set of values, set hint to be 'lower_bound', 'begin',
    //:     'begin' + 1, 'end' - 1, 'end'
    //:
    //:     1 For each value in the set, 'insert' the value with hint.
    //:
    //:       1 Compute the number of allocations and verify it is as
    //:         expected.
    //:
    //:       2 Verify the return value and the resulting data in the container
    //:         is as expected.
    //:
    //:       3 Verify the new element is inserted right before the hint if
    //:         the hint is valid.  (C-3)
    //:
    //:       4 Verify the number of comparisons is no more than 2 if the hint
    //:         is valid.  (C-4)
    //:
    //:       5 Verify all allocations are from the object's allocator.  (C-6)
    //:
    //: 2 Repeat P-1 under the presence of injected exceptions.  (C-7)
    //
    // Testing:
    //   iterator insert(const_iterator position, const value_type& value);
    // -----------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value +
                           bslma::UsesBslmaAllocator<VALUE>::value;

    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);

    static const struct {
        int         d_line;      // source line number
        const char *d_spec_p;    // specification string
        const char *d_unique_p;  // expected element values
    } DATA[] = {
        //line  spec           isUnique
        //----  --------       --------

        { L_,   "A",           "Y"           },
        { L_,   "AAA",         "YNN"         },
        { L_,   "ABCDEFGH",    "YYYYYYYY"    },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY" },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"  }
    };
    const int NUM_DATA = static_cast<const int>(sizeof DATA / sizeof *DATA);

    const int MAX_LENGTH = 10;

    if (verbose) printf("\nTesting 'insert' with hint.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const char *const UNIQUE = DATA[ti].d_unique_p;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                Obj mX(&oa);  const Obj &X = mX;

                for (int tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                    if (IS_UNIQ) {
                        EXPECTED[SIZE] = SPEC[tj];
                        std::sort(EXPECTED, EXPECTED + SIZE + 1);
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { P(EXPECTED); }
                    }

                    CIter hint;

                    switch (CONFIG) {
                      case 'a': {
                        hint = X.lower_bound(VALUES[tj].first);
                      } break;
                      case 'b': {
                        hint = X.begin();
                      } break;
                      case 'c': {
                        hint = X.begin();
                        if (hint != X.end()) {
                            ++hint;
                        }
                      } break;
                      case 'd': {
                        hint = X.end();
                        if (hint != X.begin()) {
                            --hint;
                        }
                      } break;
                      case 'e': {
                        hint = X.end();
                      } break;
                      default: {
                        ASSERTV(!"Unexpected configuration");
                      } return;                                       // RETURN
                    }

                    size_t EXP_COMP = X.key_comp().count();
                    if ('a' == CONFIG) {
                        if (!IS_UNIQ) {
                            EXP_COMP += 2;
                        }
                        else {
                            if (hint != X.begin()) {
                                ++EXP_COMP;
                            }
                            if (hint != X.end()) {
                                ++EXP_COMP;
                            }
                        }
                    }

                    const bsls::Types::Int64 BB = oa.numBlocksTotal();
                    const bsls::Types::Int64 B  = oa.numBlocksInUse();

                    Iter RESULT = mX.insert(hint, VALUES[tj]);

                    const bsls::Types::Int64 AA = oa.numBlocksTotal();
                    const bsls::Types::Int64 A  = oa.numBlocksInUse();

                    ASSERTV(LINE, CONFIG, tj, SIZE,
                            VALUES[tj] == *RESULT);

                    if ('a' == CONFIG) {
                        ASSERTV(LINE, tj, EXP_COMP, X.key_comp().count(),
                                EXP_COMP == X.key_comp().count());

                        if (IS_UNIQ) {
                            ASSERTV(LINE, tj, hint == ++RESULT);
                        }
                        else {
                            ASSERTV(LINE, tj, hint == RESULT);
                        }
                    }

                    if (IS_UNIQ) {
                        if (expectToAllocate(SIZE + 1)) {
                            ASSERTV(LINE, tj, A, B,
                                    B + 1 + TYPE_ALLOC == A);
                        }
                        else {
                            ASSERTV(LINE, tj, A, B, B + 0 + TYPE_ALLOC == A);
                        }
                        ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, tj,
                                0 == verifyContainer(X, exp, SIZE + 1));
                    }
                    else {
                        ASSERTV(LINE, tj, AA, BB, BB == AA);
                        ASSERTV(LINE, tj, A,  B,  B == A);
                        ASSERTV(LINE, tj, SIZE == X.size());

                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE));
                    }
                }
            }
        }
    }

    if (verbose) printf("\nTesting 'insert' with exception.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const char *const UNIQUE = DATA[ti].d_unique_p;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                Obj mX(&oa);  const Obj &X = mX;

                for (int tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                    if (IS_UNIQ) {
                        EXPECTED[SIZE] = SPEC[tj];
                        std::sort(EXPECTED, EXPECTED + SIZE + 1);
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { P(EXPECTED); }
                    }

                    CIter hint;

                    switch (CONFIG) {
                      case 'a': {
                        hint = X.lower_bound(VALUES[tj].first);
                      } break;
                      case 'b': {
                        hint = X.begin();
                      } break;
                      case 'c': {
                        hint = X.begin();
                        if (hint != X.end()) {
                            ++hint;
                        }
                      } break;
                      case 'd': {
                        hint = X.end();
                        if (hint != X.begin()) {
                            --hint;
                        }
                      } break;
                      case 'e': {
                        hint = X.end();
                      } break;
                      default: {
                        ASSERTV(!"Unexpected configuration");
                      } return;                                       // RETURN
                    }

                    bslma::TestAllocator scratch("scratch",
                                                 veryVeryVeryVerbose);

                    const bsls::Types::Int64 BB = oa.numBlocksTotal();
                    const bsls::Types::Int64 B  = oa.numBlocksInUse();

                    Iter RESULT;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ExceptionProctor<Obj> proctor(&X, L_, &scratch);

                        RESULT = mX.insert(hint, VALUES[tj]);

                        proctor.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    const bsls::Types::Int64 AA = oa.numBlocksTotal();
                    const bsls::Types::Int64 A  = oa.numBlocksInUse();

                    ASSERTV(LINE, CONFIG, tj, SIZE, VALUES[tj] == *RESULT);

                    if (IS_UNIQ) {
                        if (expectToAllocate(SIZE + 1)) {
                            ASSERTV(LINE, tj, AA, BB,
                                    BB + 1 + TYPE_ALLOC == AA);
                            ASSERTV(LINE, tj, A, B, B + 1 + TYPE_ALLOC == A);
                        }
                        else {
                            ASSERTV(LINE, tj, A, B, B + 0 + TYPE_ALLOC == A);
                        }
                        ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, tj,
                                0 == verifyContainer(X, exp, SIZE + 1));
                    }
                    else {
                        ASSERTV(LINE, tj, AA, BB, BB == AA);
                        ASSERTV(LINE, tj, A,  B,  B  == A);
                        ASSERTV(LINE, tj, SIZE == X.size());

                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE));
                    }
                }
            }
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION OF SINGLE VALUE
    //
    // Concerns:
    //: 1 'insert' returns a pair containing an iterator and a 'bool'
    //:
    //: 2 The iterator returned refers to the newly inserted element if it did
    //:   not already exists, and to the existing element if it did.
    //:
    //: 3 The 'bool' returned is 'true' if a new element is inserted, and
    //:   'false' otherwise.
    //:
    //: 4 A new element is added to the container if the element did not
    //:   already exist, and the order of the container remains correct.
    //:
    //: 5 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 6 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 For insertion we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 Compute the number of allocations and verify it is as expected.
    //:
    //:   2 If the object did not contain 'value', verify it now exist.
    //:     Otherwise, verify the return value is as expected.  (C-1..4)
    //:
    //:   3 Verify all allocations are from the object's allocator.  (C-5)
    //:
    //: 2 Repeat P-1 under the presence of injected exceptions.  (C-6)
    //
    // Testing:
    //   pair<iterator, bool> insert(const value_type& value);
    // -----------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value +
                           bslma::UsesBslmaAllocator<VALUE>::value;

    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);

    static const struct {
        int         d_line;      // source line number
        const char *d_spec_p;    // specification string
        const char *d_unique_p;  // expected element values
    } DATA[] = {
        //line  spec           isUnique
        //----  ----           --------

        { L_,   "A",           "Y"           },
        { L_,   "AAA",         "YNN"         },
        { L_,   "ABCDEFGH",    "YYYYYYYY"    },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY" },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"  }
    };
    const int NUM_DATA = static_cast<const int>(sizeof DATA / sizeof *DATA);

    const int MAX_LENGTH = 10;

    if (verbose) printf("\nTesting 'insert' without exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const char *const UNIQUE = DATA[ti].d_unique_p;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj &X = mX;

            for (int tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                const size_t SIZE    = X.size();

                if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }
                }

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                pair<Iter, bool> RESULT = mX.insert(VALUES[tj]);

                ASSERTV(LINE, tj, SIZE, IS_UNIQ    == RESULT.second);
                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *(RESULT.first));

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                if (IS_UNIQ) {
                    if (expectToAllocate(SIZE + 1)) {
                        ASSERTV(LINE, tj, AA, BB, BB + 1 + TYPE_ALLOC == AA);
                        ASSERTV(LINE, tj, A, B, B + 1 + TYPE_ALLOC == A);
                    }
                    else {
                        ASSERTV(LINE, tj, AA, BB, BB + 0 + TYPE_ALLOC == AA);
                        ASSERTV(LINE, tj, A, B, B + 0 + TYPE_ALLOC == A);
                    }
                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE + 1));
                }
                else {
                    ASSERTV(LINE, tj, AA, BB, BB == AA);
                    ASSERTV(LINE, tj, A,  B,  B == A);
                    ASSERTV(LINE, tj, SIZE == X.size());

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE));
                }
            }
        }
    }

    if (verbose) printf("\nTesting 'insert' with injected exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const char *const UNIQUE = DATA[ti].d_unique_p;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj &X = mX;

            for (int tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                const size_t SIZE    = X.size();

                if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionProctor<Obj> proctor(&X, L_, &scratch);

                    pair<Iter, bool> RESULT = mX.insert(VALUES[tj]);

                    proctor.release();

                    ASSERTV(LINE, tj, SIZE, IS_UNIQ    == RESULT.second);
                    ASSERTV(LINE, tj, SIZE, VALUES[tj] == *(RESULT.first));
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                if (IS_UNIQ) {
                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE + 1));
                }
                else {
                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE));
                }
            }
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING ITERATORS
    //
    // Concerns:
    //: 1 'begin' and 'end' return non-mutable iterators.
    //:
    //: 2 The range '[begin(), end())' contains all values inserted into the
    //:   container in ascending order.
    //:
    //: 3 The range '[rbegin(), rend())' contains all values inserted into the
    //:   container in descending order.
    //:
    //: 4 'iterator' is a pointer to 'const KEY'.
    //:
    //: 5 'const_iterator' is a pointer to 'const KEY'.
    //:
    //: 6 'reverse_iterator' and 'const_reverse_iterator' are implemented by
    //:   the (fully-tested) 'bslstl_ReverseIterator' over a pointer to
    //:   'const TYPE'.
    //
    // Plan:
    //: 1 For each value given by variety of specifications of different
    //:   lengths:
    //:
    //:   1 Create an object this value, and access each element in sequence
    //:     and in reverse sequence, both as a modifiable reference (setting it
    //:     to a default value, then back to its original value, and as a
    //:     non-modifiable reference.  (C-1..3)
    //:
    //: 2 Use 'bsl::is_same' to assert the identity of iterator types.
    //:   (C-4..6)
    //
    // Testing:
    //   iterator begin();
    //   iterator end();
    //   reverse_iterator rbegin();
    //   reverse_iterator rend();
    //   const_iterator begin() const;
    //   const_iterator end() const;
    //   const_reverse_iterator rbegin() const;
    //   const_reverse_iterator rend() const;
    //   const_reverse_iterator crbegin() const;
    //   const_reverse_iterator crend() const;
    // ------------------------------------------------------------------------

    const TestValues VALUES;

    bslma::TestAllocator oa(veryVeryVeryVerbose);

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;   // initial
    } DATA[] = {
        { L_,  ""          },
        { L_,  "A"         },
        { L_,  "ABC"       },
        { L_,  "ACBD"      },
        { L_,  "BCDAE"     },
        { L_,  "GFEDCBA"   },
        { L_,  "ABCDEFGH"  },
        { L_,  "BCDEFGHIA" }
    };
    const int NUM_DATA = static_cast<const int>(sizeof DATA / sizeof *DATA);

    if (verbose) printf("Testing 'iterator', 'begin', and 'end',"
                        " and 'const' variants.\n");
    {
        ASSERTV(1 == (bsl::is_same<typename Iter::pointer,
                                   pair<const KEY, VALUE>*>::value));
        ASSERTV(1 == (bsl::is_same<typename Iter::reference,
                                   pair<const KEY, VALUE>&>::value));
        ASSERTV(1 == (bsl::is_same<typename CIter::pointer,
                                   const pair<const KEY, VALUE>*>::value));
        ASSERTV(1 == (bsl::is_same<typename CIter::reference,
                                   const pair<const KEY, VALUE>&>::value));

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const size_t  LENGTH = strlen(SPEC);

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            if (verbose) { P_(LINE); P(SPEC); }

            size_t i = 0;
            if (0 == ti) {
                ASSERTV(mX.size(), 0 == mX.size());
                ASSERTV(mX.begin() == mX.end());
            }
            for (Iter iter = mX.begin(); iter != mX.end(); ++iter, ++i) {
                ASSERTV(LINE, i, VALUES[i] == *iter);
            }
            ASSERTV(LINE, LENGTH == i);

            i = 0;
            for (CIter iter = X.begin(); iter != X.end(); ++iter, ++i) {
                ASSERTV(LINE, i, VALUES[i] == *iter);
            }
            ASSERTV(LINE, LENGTH == i);
        }
    }

    if (verbose) printf("Testing 'reverse_iterator', 'rbegin', and 'rend',"
                        " and 'const' variants.\n");
    {
        ASSERTV(1 == (bsl::is_same<RIter,
                                   bsl::reverse_iterator<Iter> >::value));
        ASSERTV(1 == (bsl::is_same<CRIter,
                                   bsl::reverse_iterator<CIter> >::value));

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const size_t  LENGTH = strlen(SPEC);

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            if (verbose) { P_(LINE); P(SPEC); }

            int i = static_cast<int>(LENGTH) - 1;
            for (RIter riter = mX.rbegin(); riter != mX.rend(); ++riter, --i) {
                ASSERTV(LINE, VALUES[i] == *riter);
            }
            ASSERTV(LINE, -1 == i);

            i = static_cast<int>(LENGTH) - 1;
            for (CRIter riter = X.rbegin(); riter != X.rend(); ++riter, --i) {
                ASSERTV(LINE, VALUES[i] == *riter);
            }
            ASSERTV(LINE, -1 == i);

            i = static_cast<int>(LENGTH) - 1;
            for (CRIter riter = mX.crbegin(); riter != mX.crend();
                                                                ++riter, --i) {
                ASSERTV(LINE, VALUES[i] == *riter);
            }
            ASSERTV(LINE, -1 == i);
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase13()
{
    // ------------------------------------------------------------------------
    // SEARCH FUNCTIONS
    //
    // Concerns:
    //: 1 If the key being searched exists in the container, 'find' and
    //:   'lower_bound' return an iterator referring to the existing element,
    //:   and 'upper_bound' returns an iterator referring to the element after
    //:   the searched element.
    //:
    //: 2 If the key being searched does not exist in the container, 'find'
    //:   returns the 'end' iterator, and 'lower_bound' and 'upper_bound'
    //:   return an iterator referring to the smallest element greater than the
    //:   searched element.
    //:
    //: 3 'equal_range(key)' returns
    //:   'std::make_pair(lower_bound(key), upper_bound(key))'.
    //:
    //: 4 'count' returns the number of elements having equivalent keys as
    //:   defined by the comparator (i.e., 0 or 1).
    //:
    //: 5 Both the 'const' and non-'const' versions of the methods return the
    //:   same value.
    //:
    //: 6 The methods work with movable and move-only types.
    //:
    //: 7 No memory is allocated.
    //
    // Plan:
    //: 1 Use a loop-based approach for different lengths:
    //:
    //:   1 Create an object for each length using values where every
    //:     consecutive pair of values has at least 1 value that is between
    //:     those two values.
    //:
    //:   2 Exercise all search functions on all values in the container and
    //:     values between each consecutive pair of values in the container,
    //:     ensuring that movable and move-only types are tested.
    //:
    //:   3 Verify the expected result is returned.  (C-1..6)
    //:
    //:   4 Verify no memory is allocated from any allocator.  (C-7)
    //
    // Testing:
    //   iterator find(const key_type& key);
    //   const_iterator find(const key_type& key) const;
    //   size_type count(const key_type& key) const;
    //   iterator lower_bound(const key_type& key);
    //   const_iterator lower_bound(const key_type& key) const;
    //   iterator upper_bound(const key_type& key);
    //   const_iterator upper_bound(const key_type& key) const;
    //   pair<iterator, iterator> equal_range(const key_type& key);
    //   pair<const_iter, const_iter> equal_range(const key_type&) const;
    // ------------------------------------------------------------------------

    const TestValues VALUES;    // contains 52 distinct increasing values

    const int MAX_LENGTH = 13;  // effectively use only 25 elements in 'VALUES'

    if (verbose) printf("\nTesting various search methods.\n");
    {
        for (int ti = 0; ti < MAX_LENGTH; ++ti) {
            const int LENGTH = ti;

            CIter CITER[MAX_LENGTH];
            Iter  ITER[MAX_LENGTH];

            bslma::TestAllocator da("default",      veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",       veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&oa);  const Obj& X = mX;

            for (int i = 0; i < LENGTH; ++i) {
                const int idx = 2 * i + 1;

                pair<Iter, bool> RESULT = primaryManipulator(
                                 &mX,
                                 TstFacility::getIdentifier(VALUES[idx].first),
                                 &scratch);

                ASSERTV(ti, i, true        == RESULT.second);
                ASSERTV(ti, i, VALUES[idx] == *RESULT.first);

                CITER[i] = RESULT.first;
                ITER[i]  = RESULT.first;
            }
            CITER[LENGTH] = X.cend();
            ITER[LENGTH]  = mX.end();
            ASSERTV(ti, LENGTH == (int)X.size());

            bslma::TestAllocatorMonitor oam(&oa);

            for (int tj = 0; tj <= 2 * LENGTH; ++tj) {
                if (1 == tj % 2) {
                    const int idx = tj / 2;
                    ASSERTV(ti, tj, CITER[idx] == X.find(VALUES[tj].first));
                    ASSERTV(ti, tj, ITER[idx]  == mX.find(VALUES[tj].first));
                    ASSERTV(ti, tj,
                            CITER[idx] == X.lower_bound(VALUES[tj].first));
                    ASSERTV(ti, tj,
                            ITER[idx] == mX.lower_bound(VALUES[tj].first));
                    ASSERTV(ti, tj,
                            CITER[idx + 1] == X.upper_bound(VALUES[tj].first));
                    ASSERTV(ti, tj,
                            ITER[idx + 1] == mX.upper_bound(VALUES[tj].first));

                    pair<CIter, CIter> R1 = X.equal_range(VALUES[tj].first);
                    ASSERTV(ti, tj, CITER[idx]     == R1.first);
                    ASSERTV(ti, tj, CITER[idx + 1] == R1.second);

                    pair<Iter, Iter> R2 = mX.equal_range(VALUES[tj].first);
                    ASSERTV(ti, tj, ITER[idx]     == R2.first);
                    ASSERTV(ti, tj, ITER[idx + 1] == R2.second);

                    ASSERTV(ti, tj, 1 == X.count(VALUES[tj].first));
                }
                else {
                    const int idx = tj / 2;
                    ASSERTV(ti, tj, X.cend() == X.find(VALUES[tj].first));
                    ASSERTV(ti, tj, mX.end() == mX.find(VALUES[tj].first));
                    ASSERTV(ti, tj,
                            CITER[idx] == X.lower_bound(VALUES[tj].first));
                    ASSERTV(ti, tj,
                            ITER[idx]  == mX.lower_bound(VALUES[tj].first));
                    ASSERTV(ti, tj,
                            CITER[idx] == X.upper_bound(VALUES[tj].first));
                    ASSERTV(ti, tj,
                            ITER[idx]  == mX.upper_bound(VALUES[tj].first));

                    pair<CIter, CIter> R1 = X.equal_range(VALUES[tj].first);
                    ASSERTV(ti, tj, CITER[idx] == R1.first);
                    ASSERTV(ti, tj, CITER[idx] == R1.second);

                    pair<Iter, Iter> R2 = mX.equal_range(VALUES[tj].first);
                    ASSERTV(ti, tj, ITER[idx] == R2.first);
                    ASSERTV(ti, tj, ITER[idx] == R2.second);

                    ASSERTV(ti, tj, 0 == X.count(VALUES[tj].first));
                }
            }
            ASSERTV(ti, oam.isTotalSame());
            ASSERTV(ti, da.numBlocksInUse(), 0 == da.numBlocksInUse());
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase12()
{
    // ------------------------------------------------------------------------
    // RANGE (TEMPLATE) CONSTRUCTORS
    //
    // Concerns:
    //: 1 All values within the range '[first, last)' are inserted.
    //:
    //: 2 Each iterator is dereferenced only once.
    //:
    //: 3 Repeated values are ignored.
    //:
    //: 4 If an allocator is NOT supplied to the value constructor, the
    //:   default allocator in effect at the time of construction becomes
    //:   the object allocator for the resulting object.
    //:
    //: 5 If an allocator IS supplied to the value constructor, that
    //:   allocator becomes the object allocator for the resulting object.
    //:
    //: 6 Supplying a null allocator address has the same effect as not
    //:   supplying an allocator.
    //:
    //: 7 Supplying an allocator to the value constructor has no effect
    //:   on subsequent object values.
    //:
    //: 8 Constructing from an ordered list requires linear time.
    //:
    //: 9 Any memory allocation is from the object allocator.
    //:
    //:10 There is no temporary memory allocation from any allocator.
    //:
    //:11 Every object releases any allocated memory at destruction.
    //:
    //:12 QoI: Creating an object having the default-constructed value
    //:   allocates no memory.
    //:
    //:13 Any memory allocation is exception neutral.
    //
    // TBD Missing concerns that the correct comparator is used.  We should be
    // testing with a stateful comparator (testing two states) and the default
    // comparator.  A (stateful) comparator that simply holds an ID would be
    // good enough.  (also test cases 2 & 33).
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:
    //:
    //:   1 Execute an inner loop creating three distinct objects, in turn,
    //:     each object having the same value, 'V', but configured differently:
    //:     (a) without passing an allocator, (b) passing a null allocator
    //:     address explicitly, and (c) passing the address of a test allocator
    //:     distinct from the default allocator.
    //:
    //:   2 For each of the three iterations in P-2.1:
    //:
    //:     1 Insert the test data to a specialized container that have returns
    //:       standard conforming input iterators.
    //:
    //:     2 Create three 'bslma::TestAllocator' objects, and install one as
    //:       the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     3 Use the value constructor to dynamically create an object using
    //:       'begin' and 'end' from the container in P-2.1, with its object
    //:       allocator configured appropriately (see P-2.2), supplying all the
    //:       arguments as 'const'; use a distinct test allocator for the
    //:       object's footprint.
    //:
    //:     4 Verify that all of the attributes of each object have their
    //:       expected values.
    //:
    //:     5 Use the 'allocator' accessor of each underlying attribute capable
    //:       of allocating memory to ensure that its object allocator is
    //:       properly installed; also invoke the (as yet unproven) 'allocator'
    //:       accessor of the object under test.
    //:
    //:     6 Use the appropriate test allocators to verify that:
    //:
    //:       1 An object that IS expected to allocate memory does so from the
    //:         object allocator only (irrespective of the specific number of
    //:         allocations or the total amount of memory allocated).
    //:
    //:       2 An object that is expected NOT to allocate memory does not
    //:         allocate memory.
    //:
    //:       3 If an allocator was supplied at construction (P-2.1c), the
    //:         default allocator doesn't allocate any memory.
    //:
    //:       4 If the input range is ordered, verify the number of comparisons
    //:         is equal to 'LENGTH - 1', where 'LENGTH' is the number of
    //:         elements in the input range.
    //:
    //:       5 No temporary memory is allocated from the object allocator.
    //:
    //:       6 All object memory is released when the object is destroyed.
    //:
    //: 3 Repeat the steps in P-2 for the supplied allocator configuration
    //:   (P-2.1c) on the data of P-1, but this time create the object as an
    //:   automatic variable in the presence of injected exceptions (using the
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros); represent any string
    //:   arguments in terms of 'string' using a "scratch" allocator.
    //
    // Testing:
    //   map(ITER first, ITER last, const C& comparator, const A& allocator);
    //   map(ITER first, ITER last, const A& allocator);
    // ------------------------------------------------------------------------

    static const struct {
        int         d_line;         // source line number
        const char *d_spec_p;       // specification string
        const char *d_results_p;    // expected element values
        bool        d_orderedFlag;  // is the spec in ascending order
    } DATA[] = {
        //line  spec          elements  ordered
        //----  --------      --------  -------
        { L_,   "",           "",          true },
        { L_,   "A",          "A",         true },
        { L_,   "AB",         "AB",        true },
        { L_,   "ABC",        "ABC",       true },
        { L_,   "ABCD",       "ABCD",      true },
        { L_,   "ABCDE",      "ABCDE",     true },
        { L_,   "AABCCDDDE",  "ABCDE",     true },
        { L_,   "DABEC",      "ABCDE",    false },
        { L_,   "EDCBACBA",   "ABCDE",    false },
        { L_,   "ABCDEABCD",  "ABCDE",    false }
    };
    const int NUM_DATA = static_cast<const int>(sizeof DATA / sizeof *DATA);

    if (verbose) printf("\nTesting without injected exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE    = DATA[ti].d_line;
            const char       *SPEC    = DATA[ti].d_spec_p;
            const size_t      LENGTH  = strlen(DATA[ti].d_results_p);
            const bool        ORDERED = DATA[ti].d_orderedFlag;
            const TestValues  EXP(DATA[ti].d_results_p);

            if (verbose) { P_(LINE) P_(SPEC) P(LENGTH); }

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                if (veryVerbose) { T_ T_ P(CONFIG) }

                TestValues CONT(SPEC);
                typename TestValues::iterator BEGIN = CONT.begin();
                typename TestValues::iterator END   = CONT.end();

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                    objPtr = new (fa) Obj(BEGIN, END);
                    objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                    objPtr = new (fa) Obj(BEGIN, END, COMP(), 0);
                    objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                    objPtr = new (fa) Obj(BEGIN, END, COMP(), &sa);
                    objAllocatorPtr = &sa;
                  } break;
                  case 'd': {
                    objPtr = new (fa) Obj(BEGIN, END, 0);
                    objAllocatorPtr = &da;
                  } break;
                  case 'e': {
                    objPtr = new (fa) Obj(BEGIN, END, &sa);
                    objAllocatorPtr = &sa;
                  } break;
                  default: {
                    ASSERTV(LINE, CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }
                ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                Obj& mX = *objPtr;  const Obj& X = mX;

                if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = 'c' == CONFIG || 'e' == CONFIG
                                            ? da
                                            : sa;

                // Use untested functionality to help ensure the first row of
                // the table contains the default-constructed value.

                static bool firstFlag = true;
                if (firstFlag) {
                    ASSERTV(LINE, CONFIG, Obj(), *objPtr, Obj() == *objPtr);
                    firstFlag = false;
                }

                // -------------------------------------------------------
                // Verify any attribute allocators are installed properly.
                // -------------------------------------------------------

                ASSERTV(LINE, CONFIG, &oa == X.get_allocator());

                if (ORDERED && LENGTH > 0) {
                    const size_t SPECLEN = strlen(DATA[ti].d_spec_p);
                    ASSERTV(LINE, CONFIG, (SPECLEN - 1) * 2,
                            X.key_comp().count(),
                            (SPECLEN - 1) * 2 == X.key_comp().count());
                }
                // Verify no allocation from the non-object allocator.

                ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                             0 == noa.numBlocksTotal());

                // Verify no temporary memory is allocated from the object
                // allocator.

                ASSERTV(LINE, CONFIG, oa.numBlocksTotal(), oa.numBlocksInUse(),
                        oa.numBlocksTotal() == oa.numBlocksInUse());

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa.numBlocksInUse(),
                        0 == sa.numBlocksInUse());

            }  // end for each configuration
        }  // end for each row
    }

    if (verbose) printf("\nTesting with injected exceptions.\n");
    {
        // Note that any string arguments are now of type 'string', which
        // require their own "scratch" allocator.

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char       *SPEC   = DATA[ti].d_spec_p;
            const size_t      LENGTH = strlen(DATA[ti].d_results_p);
            const TestValues  EXP(DATA[ti].d_results_p);

            TestValues CONT(SPEC);
            typename TestValues::iterator BEGIN = CONT.begin();
            typename TestValues::iterator END   = CONT.end();

            if (verbose) { P_(LINE) P(SPEC) P_(LENGTH); }

            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                CONT.resetIterators();

                Obj mX(BEGIN, END, COMP(), &sa);

                ASSERTV(LINE, mX, 0 == verifyContainer(mX, EXP, LENGTH));
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
            ASSERTV(LINE, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver<KEY, VALUE, COMP, ALLOC>::
                    testCase9_propagate_on_container_copy_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   KEY,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS>  StdAlloc;

    typedef bsl::map<KEY, VALUE, COMP, StdAlloc> Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };
    const int NUM_SPECS = static_cast<const int>(sizeof SPECS / sizeof *SPECS);

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

        const Obj W(IVALUES.begin(), IVALUES.end(), COMP(), scratch);
                                                                     // control

        // Create target object.
        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            {
                IVALUES.resetIterators();

                Obj mY(IVALUES.begin(), IVALUES.end(), COMP(), mas);
                const Obj& Y = mY;

                if (veryVerbose) { T_ P_(ISPEC) P_(Y) P(W) }

                Obj mX(JVALUES.begin(), JVALUES.end(), COMP(), mat);
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

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::
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

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase9()
{
    // ------------------------------------------------------------------------
    // COPY-ASSIGNMENT OPERATOR
    //   Ensure that we can assign the value of any object of the class to any
    //   other object of the class, such that the two objects subsequently have
    //   the same value.
    //
    // Concerns:
    //: 1 The assignment operator can change the value of any modifiable target
    //:   object to that of any source object.
    //:
    //: 2 If allocator propagation is not enabled for copy assignment, the
    //:   allocator address held by the target object is unchanged.
    //:
    //: 3 If allocator propagation is not enabled for copy assignment, any
    //:   memory allocation is from the target object's allocator.
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
    //: 9 Any memory allocation is exception neutral.
    //:
    //:10 Assigning an object to itself behaves as expected (alias-safety).
    //:
    //:11 Every object releases any allocated memory at destruction.
    //:
    //:12 If allocator propagation is enabled for copy assignment,
    //:   any memory allocation is from the source object's
    //:   allocator.
    //:
    //:13 If allocator propagation is enabled for copy assignment, the
    //:   allocator address held by the target object is changed to that of the
    //:   source.
    //:
    //:14 If allocator propagation is enabled for copy assignment, any memory
    //:   allocation is from the original target allocator will be released
    //:   after copy assignment.
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   copy-assignment operator defined in this component.  (C-4)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 3 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 4 For each row 'R1' (representing a distinct object value, 'V') in the
    //:   table described in P-3: (C-1..2, 5..8, 11)
    //:
    //:   1 Use the value constructor and a "scratch" allocator to create two
    //:     'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
    //:
    //:   2 Execute an inner loop that iterates over each row 'R2'
    //:     (representing a distinct object value, 'W') in the table described
    //:     in P-3:
    //:
    //:   3 For each of the iterations (P-4.2): (C-1..2, 5..8, 11)
    //:
    //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:     2 Use the value constructor and 'oa' to create a modifiable 'Obj',
    //:       'mX', having the value 'W'.
    //:
    //:     3 Assign 'mX' from 'Z' in the presence of injected exceptions
    //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
    //:
    //:     4 Verify that the address of the return value is the same as that
    //:       of 'mX'.  (C-5)
    //:
    //:     5 Use the equality-comparison operator to verify that: (C-1, 6)
    //:
    //:       1 The target object, 'mX', now has the same value as that of 'Z'.
    //:         (C-1)
    //:
    //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
    //:
    //:     6 Use the 'allocator' accessor of both 'mX' and 'Z' to verify that
    //:       the respective allocator addresses held by the target and source
    //:       objects are unchanged.  (C-2, 7)
    //:
    //:     7 Use the appropriate test allocators to verify that: (C-8, 11)
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
    //:         (C-11)
    //:
    //: 5 Repeat steps similar to those described in P-4 except that, this
    //:   time, there is no inner loop (as in P-4.2); instead, the source
    //:   object, 'Z', is a reference to the target object, 'mX', and both 'mX'
    //:   and 'ZZ' are initialized to have the value 'V'.  For each row
    //:   (representing a distinct object value, 'V') in the table described in
    //:   P-3: (C-9)
    //:
    //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
    //:     'mX'; also use the value constructor and a distinct "scratch"
    //:     allocator to create a 'const' 'Obj' 'ZZ'.
    //:
    //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
    //:
    //:   4 Assign 'mX' from 'Z' in the presence of injected exceptions (using
    //:     the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  (C-9)
    //:
    //:   5 Verify that the address of the return value is the same as that of
    //:     'mX'.
    //:
    //:   6 Use the equality-comparison operator to verify that the target
    //:     object, 'mX', still has the same value as that of 'ZZ'.
    //:
    //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still the
    //:     object allocator.
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
    //   map& operator=(const map& rhs);
    // ------------------------------------------------------------------------

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf("\nCompare each pair of similar and different"
                        " values (u, ua, v, va) in S X A X S X A"
                        " without perturbation.\n");
    {
        // Create first object.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1  = DATA[ti].d_line;
            const int         INDEX1 = DATA[ti].d_index;
            const char *const SPEC1  = DATA[ti].d_spec_p;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZ(&scratch);   const Obj& Z  = gg(&mZ,  SPEC1);
            Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC1);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            // Create second object.
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2  = DATA[tj].d_line;
                const int         INDEX2 = DATA[tj].d_index;
                const char *const SPEC2  = DATA[tj].d_spec_p;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(&oa);  const Obj& X = gg(&mX, SPEC2);

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    ASSERTV(LINE1, LINE2, Z, X,
                            (Z == X) == (INDEX1 == INDEX2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                    ASSERTV(LINE1, LINE2, &scratch == Z.get_allocator());

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

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

                Obj mX(&oa);        const Obj& X  = gg(&mX,  SPEC1);
                Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC1);

                const Obj& Z = mX;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = Z);
                    ASSERTV(LINE1, ZZ,   Z, ZZ == Z);
                    ASSERTV(LINE1, mR,  &X, mR == &X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE1, &oa == Z.get_allocator());

                ASSERTV(LINE1, sam.isTotalSame());
                ASSERTV(LINE1, oam.isTotalSame());

                ASSERTV(LINE1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase8_dispatch()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS
    //   Ensure that, when member and free 'swap' are implemented, we can
    //   exchange the values of any two objects that use the same
    //   allocator.
    //
    // Concerns:
    //: 1 Both functions exchange the values of the (two) supplied objects.
    //:
    //: 2 Both functions have standard signatures and return types.
    //:
    //: 3 Using either function to swap an object with itself does not
    //:   affect the value of the object (alias-safety).
    //:
    //: 4 If the two objects being swapped uses the same allocator, neither
    //:   function allocates memory from any allocator and the allocator
    //:   address held by both objects is unchanged.
    //:
    //: 5 If the two objects being swapped uses different allocators and
    //:   'AllocatorTraits::propagate_on_container_swap' is an alias to
    //:   'false_type', then both function may allocate memory and the
    //:   allocator address held by both object is unchanged.
    //:
    //: 6 If the two objects being swapped uses different allocators and
    //:   'AllocatorTraits::propagate_on_container_swap' is an alias to
    //:   'true_type', then no memory will be allocated and the allocators will
    //:   also be swapped.
    //:
    //: 7 The free 'swap' function is discoverable through ADL (Argument
    //:   Dependent Lookup).
    //
    // Plan:
    //: 1 Use the addresses of the 'swap' member and free functions defined
    //:   in this component to initialize, respectively, member-function
    //:   and free-function pointers having the appropriate signatures and
    //:   return types.  (C-2)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the
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
    //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
    //:     the expectation of memory allocation for all typical
    //:     implementations of individual attribute types: ('Y') "Yes",
    //:     ('N') "No", or ('?') "implementation-dependent".
    //:
    //: 4 For each row 'R1' in the table of P-3:  (C-1, 3..7)
    //:
    //:   1 Create two 'bslma::TestAllocator' objects, 'oa' and 'ob'.
    //:
    //:   2 Use the value constructor and 'oa' to create a modifiable
    //:     'Obj', 'mW', having the value described by 'R1'; also use the
    //:     copy constructor and a "scratch" allocator to create a 'const'
    //:     'Obj' 'XX' from 'mW'.
    //:
    //:   3 Use the member and free 'swap' functions to swap the value of
    //:     'mW' with itself; verify, after each swap, that:  (C-3..4)
    //:
    //:     1 The value is unchanged.  (C-3)
    //:
    //:     2 The allocator address held by the object is unchanged.  (C-4)
    //:
    //:     3 There was no additional object memory allocation.  (C-4)
    //:
    //:   4 For each row 'R2' in the table of P-3:  (C-1, 4)
    //:
    //:     1 Use the copy constructor and 'oa' to create a modifiable
    //:       'Obj', 'mX', from 'XX'.  (P-4.2).
    //:
    //:     2 Use the value constructor and 'oa' to create a modifiable
    //:       'Obj', 'mY', and having the value described by 'R2'; also use
    //:       the copy constructor to create, using a "scratch" allocator,
    //:       a 'const' 'Obj', 'YY', from 'Y'.
    //:
    //:     3 Use, in turn, the member and free 'swap' functions to swap
    //:       the values of 'mX' and 'mY'; verify, after each swap, that:
    //:       (C-1, 4)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 The common object allocator address held by 'mX' and 'mY'
    //:         is unchanged in both objects.  (C-4)
    //:
    //:       3 There was no additional object memory allocation.  (C-4)
    //:
    //:     4 Create a new object allocator, 'oaz'.
    //:
    //:     5 Use the value constructor and 'oaz' to a create a modifiable
    //:       'Obj' 'mZ', having the value described by 'R2'; also use the copy
    //:       constructor to create, using a "scratch" allocator, a const
    //:       'Obj', 'ZZ', from 'Z'.
    //:
    //:     6 Use the member and free 'swap' functions to swap the values of
    //:       'mX' and 'mZ' respectively (when
    //:       AllocatorTraits::propagate_on_container_swap is an alias to
    //:       false_type) verify, after each swap, that:  (C-1, 5)
    //:
    //:       1 The values have been exchanged. (C-1)
    //:
    //:       2 The common object allocator address held by 'mX' and 'mZ' is
    //:         unchanged in both objects.  (C-5)
    //:
    //:       3 Temporary memory were allocated from 'oa' if 'mZ' is not empty,
    //:         and temporary memory were allocated from 'oaz' if 'mX' is not
    //:         empty.  (C-5)
    //:
    //:     7 Create a new object allocator, 'oap'.
    //:
    //:     8 Use the value constructor and 'oap' to create a modifiable 'Obj'
    //:       'mP', having the value described by 'R2'; also use the copy
    //:       constructor to create, using a "scratch" allocator, a const
    //:       'Obj', 'PP', from 'P.
    //:
    //:     9 Manually change 'AllocatorTraits::propagate_on_container_swap' to
    //:       be an alias to 'true_type' (Instead of this manual step, use an
    //:       allocator that enables propagate_on_container_swap when
    //:       AllocatorTraits supports it) and use the member and free 'swap'
    //:       functions to swap the values 'mX' and 'mZ' respectively; verify,
    //:       after each swap, that: (C-1, 6)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 The allocators addresses have been exchanged.  (C-6)
    //:
    //:       3 There was no additional object memory allocation.  (C-6)
    //:
    //: 5 Verify that the free 'swap' function is discoverable through ADL:
    //:   (C-7)
    //:
    //:   1 Create a set of attribute values, 'A', distinct from the values
    //:     corresponding to the default-constructed object, choosing
    //:     values that allocate memory if possible.
    //:
    //:   2 Create a 'bslma::TestAllocator' object, 'oa'.
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
    //:     1 The values have been exchanged.  (C-1)
    //:
    //:     2 There was no additional object memory allocation.  (C-4)
    //
    // Testing:
    //   void swap(map& other);
    //   void swap(map& a, map& b);
    // ------------------------------------------------------------------------

    // Since this function is called with a variety of template arguments, it
    // is necessary to infer some things about our template arguments in order
    // to print a meaningful banner.

    const bool isPropagate =
                           AllocatorTraits::propagate_on_container_swap::value;
    const bool otherTraitsSet =
                AllocatorTraits::propagate_on_container_copy_assignment::value;

    // We can print the banner now:

    if (verbose) printf("%sTESTING SWAP '%s' OTHER:%c PROP:%c ALLOC: %s\n",
                        veryVerbose ? "\n" : "",
                        NameOf<VALUE>().name(), otherTraitsSet ? 'T' : 'F',
                        isPropagate ? 'T' : 'F',
                        allocCategoryAsStr());

    if (veryVerbose) printf(
                     "\nAssign the address of each function to a variable.\n");
    {
        typedef void (Obj::*funcPtr)(Obj&);
        typedef void (*freeFuncPtr)(Obj&, Obj&);

        // Verify that the signatures and return types are standard.

        funcPtr     memberSwap = &Obj::swap;
        freeFuncPtr freeSwap   = bsl::swap;

        (void)memberSwap;  // quash potential compiler warnings
        (void)freeSwap;
    }

    if (veryVerbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

    bslma::TestAllocator         doa("default",  veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&doa);
    ALLOC                        da(&doa);

    bslma::TestAllocator         ooa("object",   veryVeryVeryVerbose);
    bslma::TestAllocator         soa("scratch",  veryVeryVeryVerbose);
    bslma::TestAllocator         zoa("z_object", veryVeryVeryVerbose);

    ALLOC                        oa(&ooa);
    ALLOC                        sa(&soa);
    ALLOC                        za(&zoa);

    // Check remaining properties of allocator to make sure they all match
    // 'otherTraitsSet'.

    BSLMF_ASSERT(otherTraitsSet ==
               AllocatorTraits::propagate_on_container_move_assignment::value);
    ASSERT((otherTraitsSet ? sa : da) ==
                   AllocatorTraits::select_on_container_copy_construction(sa));

    if (veryVerbose) printf(
       "\nUse a table of distinct object values and expected memory usage.\n");

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const char *const SPEC1   = DATA[ti].d_results_p;

        if (ti && DATA[ti-1].d_index == DATA[ti].d_index) {
            // redundant, skip

            continue;
        }

        ASSERT(0 == doa.numBlocksInUse());
        ASSERT(0 == ooa.numBlocksInUse());
        ASSERT(0 == soa.numBlocksInUse());
        ASSERT(0 == zoa.numBlocksInUse());

        Obj mW(oa);     const Obj& W  = gg(&mW,  SPEC1);
        Obj mXX(sa);    const Obj& XX = gg(&mXX, SPEC1);

        if (veryVerbose) { T_ P_(SPEC1) P_(W) P(XX) }

        // Ensure the first row of the table contains the default-constructed
        // value.

        static bool firstFlag = true;
        if (firstFlag) {
            ASSERTV(SPEC1, Obj(), W, Obj() == W);
            firstFlag = false;
        }

        for (int member = 0; member < 2; ++member) {
            bslma::TestAllocatorMonitor oam(&ooa);

            if (member) {
                mW.swap(mW);
            }
            else {
                swap(mW, mW);
            }

            ASSERTV(SPEC1, XX, W, XX == W);
            ASSERTV(SPEC1, oa == W.get_allocator());
            ASSERTV(SPEC1, oam.isTotalSame());
        }

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const char *const SPEC2   = DATA[tj].d_results_p;

            if (tj && DATA[tj-1].d_index == DATA[tj].d_index) {
                // redundant, skip

                continue;
            }

            Obj mYY(sa);    const Obj& YY = gg(&mYY, SPEC2);

            for (int member = 0; member < 2; ++member) {
                Obj mX(oa);          const Obj& X  = gg(&mX,  SPEC1);
                Obj mY(oa);          const Obj& Y  = gg(&mY,  SPEC2);

                if (veryVerbose) { T_ P_(member) P_(X) P_(Y) P(YY) }

                bslma::TestAllocatorMonitor oam(&ooa);

                if (member) {
                    mX.swap(mY);
                }
                else {
                    swap(mX, mY);
                }

                ASSERTV(SPEC1, SPEC2, YY, X, YY == X);
                ASSERTV(SPEC1, SPEC2, XX, Y, XX == Y);
                ASSERTV(SPEC1, SPEC2, oa == X.get_allocator());
                ASSERTV(SPEC1, SPEC2, oa == Y.get_allocator());
                ASSERTV(SPEC1, SPEC2, oam.isTotalSame());

                Obj mZ(za);         const Obj& Z  = gg(&mZ,  SPEC1);
                ASSERT(XX == Z);

                if (veryVerbose) { T_ P_(SPEC2) P_(X) P_(Y) P(YY) }

                bslma::TestAllocatorMonitor zoam(&zoa);

                if (member) {
                    mX.swap(mZ);
                }
                else {
                    swap(mX, mZ);
                }

                ASSERTV(SPEC1, SPEC2, XX, X, XX == X);
                ASSERTV(SPEC1, SPEC2, YY, Z, YY == Z);
                ASSERTV(SPEC1, SPEC2, isPropagate, (isPropagate ? za : oa) ==
                                                            X.get_allocator());
                ASSERTV(SPEC1, SPEC2, isPropagate, (isPropagate ? oa : za) ==
                                                            Z.get_allocator());

                if (isPropagate || 0 == X.size()) {
                    ASSERTV(SPEC1, SPEC2, oam.isTotalSame());
                }
                else {
                    ASSERTV(SPEC1, SPEC2, oam.isTotalUp());
                }

                if (isPropagate || 0 == Z.size()) {
                    ASSERTV(SPEC1, SPEC2, zoam.isTotalSame());
                }
                else {
                    ASSERTV(SPEC1, SPEC2, zoam.isTotalUp());
                }
            }

            ASSERT(0 == zoa.numBlocksInUse());
        }
    }

    if (veryVerbose) printf(
            "Invoke free 'swap' function in a context where ADL is used.\n");
    {
        // 'A' values: Should cause memory allocation if possible.

        Obj mX(oa);          const Obj& X  = gg(&mX,  "DE");
        Obj mXX(sa);         const Obj& XX = gg(&mXX, "DE");

        Obj mY(oa);          const Obj& Y  = gg(&mY,  "ABC");
        Obj mYY(sa);         const Obj& YY = gg(&mYY, "ABC");

        if (veryVerbose) printf(
            "Invoke free 'swap' function in a context where ADL is used.\n");

        if (veryVerbose) { T_ P_(X) P(Y) }

        bslma::TestAllocatorMonitor oam(&ooa);

        invokeAdlSwap(&mX, &mY);

        ASSERTV(YY, X, YY == X);
        ASSERTV(XX, Y, XX == Y);
        ASSERT(oam.isTotalSame());

        if (veryVerbose) { T_ P_(X) P(Y) }

        if (veryVerbose) printf(
               "Invoke free 'swap' function via that standard BDE pattern.\n");

        invokePatternSwap(&mX, &mY);

        ASSERTV(YY, X, XX == X);
        ASSERTV(XX, Y, YY == Y);
        ASSERT(oam.isTotalSame());

        if (veryVerbose) { T_ P_(X) P(Y) }
    }

    ASSERTV(e_STATEFUL == s_allocCategory || 0 == doa.numBlocksTotal());
}

template <class KEY, class VALUE, class COMP, class ALLOC>
template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
          bool OTHER_FLAGS>
void TestDriver<KEY, VALUE, COMP, ALLOC>::
                     testCase7_select_on_container_copy_construction_dispatch()
{
    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value +
                           bslma::UsesBslmaAllocator<VALUE>::value;

    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                    KEY,
                                    SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
                                    OTHER_FLAGS,
                                    OTHER_FLAGS,
                                    OTHER_FLAGS> StdAlloc;

    typedef bsl::map<KEY, VALUE, COMP, StdAlloc> Obj;

    const bool PROPAGATE = SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };
    const int NUM_SPECS = static_cast<const int>(sizeof SPECS / sizeof *SPECS);

    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char *const SPEC   = SPECS[ti];
        const size_t      LENGTH = strlen(SPEC);

        TestValues VALUES(SPEC);

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        StdAlloc ma(&oa);

        {
            const Obj W(VALUES.begin(), VALUES.end(), COMP(), ma);  // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            VALUES.resetIterators();

            Obj mX(VALUES.begin(), VALUES.end(), COMP(), ma);
            const Obj& X = mX;

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

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::
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

    testCase7_select_on_container_copy_construction_dispatch<false, false>();
    testCase7_select_on_container_copy_construction_dispatch<false, true>();

    if (verbose) printf("\n'select_on_container_copy_construction' "
                        "propagates allocator of source object.\n");

    testCase7_select_on_container_copy_construction_dispatch<true, false>();
    testCase7_select_on_container_copy_construction_dispatch<true, true>();

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) printf("\nVerify C++03 semantics (allocator has no "
                        "'select_on_container_copy_construction' method).\n");

    typedef StatefulStlAllocator<KEY>             Allocator;
    typedef bsl::map<KEY, VALUE, COMP, Allocator> Obj;

    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE",
        };
        const int NUM_SPECS =
                          static_cast<const int>(sizeof SPECS / sizeof *SPECS);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);
            TestValues VALUES(SPEC);

            const int ALLOC_ID = ti + 73;

            Allocator a;  a.setId(ALLOC_ID);

            const Obj W(VALUES.begin(), VALUES.end(), COMP(), a);  // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            VALUES.resetIterators();

            Obj mX(VALUES.begin(), VALUES.end(), COMP(), a); const Obj& X = mX;

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            const Obj Y(X);

            ASSERTV(SPEC,        W == Y);
            ASSERTV(SPEC,        W == X);
            ASSERTV(SPEC, ALLOC_ID == Y.get_allocator().id());
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase7()
{
    // ------------------------------------------------------------------------
    // COPY CONSTRUCTOR
    //
    // Concerns:
    //: 1 The new object's value is the same as that of the original object
    //:   (relying on the equality operator) and created with the correct
    //:   capacity.
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 3 The value of the original object is left unaffected.
    //:
    //: 4 Subsequent changes in or destruction of the source object have no
    //:   effect on the copy-constructed object.
    //:
    //: 5 Subsequent changes ('insert's) on the created object have no
    //:   effect on the original and change the capacity of the new object
    //:   correctly.
    //:
    //: 6 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 7 The function is exception neutral w.r.t. memory allocation.
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
    //:   'push_back'.  Using the 'operator!=' verify that y differs from x and
    //:   w, and verify that the capacity of y changes correctly.  (C-5)
    //:
    //: 4 Perform tests performed as P-2:  (C-6)
    //:   1 While passing a testAllocator as a parameter to the new object and
    //:     ascertaining that the new object gets its memory from the provided
    //:     testAllocator.
    //:   2 Verify neither of global and default allocator is used to supply
    //:     memory.  (C-6)
    //:
    //: 5 Perform tests as P-2 in the presence of exceptions during memory
    //:   allocations using a 'bslma::TestAllocator' and varying its
    //:   *allocation* *limit*.  (C-7)
    //
    // Testing:
    //   map(const map& original);
    //   map(const map& original, const A& allocator);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa(veryVeryVeryVerbose);

    const TestValues VALUES;

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value +
                           bslma::UsesBslmaAllocator<VALUE>::value;

    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);
    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE",
            "DEAB",
            "EABCD",
            "ABCDEFG",
            "HFGEDCBA",
            "CFHEBIDGA",
            "BENCKHGMALJDFOI",
            "IDMLNEFHOPKGBCJA",
            "OIQGDNPMLKBACHFEJ"
        };
        const int NUM_SPECS =
                          static_cast<const int>(sizeof SPECS / sizeof *SPECS);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\nFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            // Create control object w.
            Obj mW;  const Obj& W = gg(&mW, SPEC);

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
                ASSERTV(SPEC, Y0.get_allocator() ==
                                           bslma::Default::defaultAllocator());

                delete pX;
                ASSERTV(SPEC, W == Y0);
            }
            {   // Testing concern 5.

                if (veryVerbose) printf("\t\t\tInsert into created obj, "
                                        "without test allocator:\n");

                Obj Y1(X);

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Insert: "); P(Y1);
                }

                pair<Iter, bool> RESULT = Y1.insert(VALUES['Z' - 'A']);

                ASSERTV(true == RESULT.second);

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
                    printf("\t\t\tInsert into created obj, "
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
                    ASSERTV(SPEC, BB + 0 == AA);
                    ASSERTV(SPEC,  B + 0 ==  A);
                }
                else {
                    const int TYPE_ALLOCS = TYPE_ALLOC
                                            * static_cast<int>(X.size());
                    ASSERTV(SPEC, BB + 1 + TYPE_ALLOCS == AA);
                    ASSERTV(SPEC,  B + 1 + TYPE_ALLOCS ==  A);
                }

                const bsls::Types::Int64 CC = oa.numBlocksTotal();
                const bsls::Types::Int64  C = oa.numBlocksInUse();

                pair<Iter, bool> RESULT = Y11.insert(VALUES['Z' - 'A']);
                ASSERTV(true == RESULT.second);

                const bsls::Types::Int64 DD = oa.numBlocksTotal();
                const bsls::Types::Int64  D = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Append : ");
                    P(Y11);
                }

                ASSERTV(SPEC, CC + 1 + TYPE_ALLOC == DD);
                ASSERTV(SPEC, C  + 1 + TYPE_ALLOC ==  D);

                ASSERTV(SPEC, Y11.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y11);
                ASSERTV(SPEC, X != Y11);
                ASSERTV(SPEC, Y11.get_allocator() == X.get_allocator());
            }
            {   // Exception checking.

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64  B = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    const Obj Y2(X, &oa);
                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                        printf("\t\t\t\tObj : "); P(Y2);
                    }
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, Y2.get_allocator() == X.get_allocator());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64  A = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                }

                if (0 == LENGTH) {
                    ASSERTV(SPEC, BB + 0 == AA);
                    ASSERTV(SPEC,  B + 0 ==  A);
                }
                else {
                    ASSERTV(SPEC, B + 0 == A);
                }
            }
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase6()
{
    // ---------------------------------------------------------------------
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
    //:10 'operator==' is defined in terms of 'operator==(KEY)' instead of the
    //:   supplied comparator function.
    //:
    //:11 No memory allocation occurs as a result of comparison (e.g., the
    //:   arguments are not passed by value).
    //:
    //:12 The equality operator's signature and return type are standard.
    //:
    //:13 The inequality operator's signature and return type are standard.
    //
    // Plan:
    //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
    //:   initialize function pointers having the appropriate signatures and
    //:   return types for the two homogeneous, free equality- comparison
    //:   operators defined in this component.  (C-8..9, 12..13)
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
    //:     a"scratch" allocator, and use it to verify the reflexive
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
    //:       4 Disable the comparator so that it will cause an error if it is
    //:         used.
    //:
    //:       5 Verify the commutativity property and expected return value for
    //:         both '==' and '!=', while monitoring both 'oax' and 'oay' to
    //:         ensure that no object memory is ever allocated by either
    //:         operator.  (C-1..2, 5..7, 10)
    //:
    //: 5 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-11)
    //
    // Testing:
    //   bool operator==(const map& lhs, const map& rhs);
    //   bool operator!=(const map& lhs, const map& rhs);
    // ------------------------------------------------------------------------

// TBD lacks interesting test data, such as:
//  o equivalent keys (per 'COMP') that do not compare equal (per '==')
//  o equal values with unequal keys

    if (verbose) printf("\nEQUALITY-COMPARISON OPERATORS"
                        "\n=============================\n");

    if (verbose)
              printf("\nAssign the address of each operator to a variable.\n");
    {
        using namespace bsl;

        typedef bool (*operatorPtr)(const Obj&, const Obj&);

        // Verify that the signatures and return types are standard.

        operatorPtr operatorEq = operator==;
        operatorPtr operatorNe = operator!=;

        (void)operatorEq;  // quash potential compiler warnings
        (void)operatorNe;
    }

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\nCompare every value with every value.\n");
    {
        // Create first object.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec_p;
            const size_t      LENGTH1 = strlen(DATA[ti].d_results_p);

           if (veryVerbose) { T_ P_(LINE1) P_(INDEX1) P_(LENGTH1) P(SPEC1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&scratch);  const Obj& X = gg(&mX, SPEC1);

                ASSERTV(LINE1, X,   X == X);
                ASSERTV(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec_p;
                const size_t      LENGTH2 = strlen(DATA[tj].d_results_p);

                if (veryVerbose) {
                              T_ T_ P_(LINE2) P_(INDEX2) P_(LENGTH2) P(SPEC2) }

                const bool EXP = INDEX1 == INDEX2;  // expected result

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create two distinct test allocators, 'oax' and 'oay'.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    Obj mX(&xa);  const Obj& X = gg(&mX, SPEC1);
                    Obj mY(&ya);  const Obj& Y = gg(&mY, SPEC2);

                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH1 == X.size());
                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH2 == Y.size());

                    if (veryVerbose) { T_ T_ P_(X) P(Y); }

                    // Verify value, commutativity, and no memory allocation.

                    bslma::TestAllocatorMonitor oaxm(&xa);
                    bslma::TestAllocatorMonitor oaym(&ya);

                    TestComparator<KEY>::disableFunctor();

                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (Y != X));

                    TestComparator<KEY>::enableFunctor();

                    ASSERTV(LINE1, LINE2, CONFIG, oaxm.isTotalSame());
                    ASSERTV(LINE1, LINE2, CONFIG, oaym.isTotalSame());
                }
            }
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    //   Ensure each basic accessor ('cbegin', 'cend', 'size', 'get_allocator')
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
    //: 4 The range '[cbegin(), cend())' contains inserted elements the sorted
    //:   order.
    //
    // Plan:
    //: 1 For each set of 'SPEC' of different length:
    //:
    //:   1 Default construct the object with various configuration:
    //:
    //:     1 Use the 'gg' function to populate the object based on the SPEC.
    //:
    //:     2 Verify the correct allocator is installed with the
    //:       'get_allocator' method.
    //:
    //:     3 Verify the object contains the expected number of elements.
    //:
    //:     4 Use 'cbegin' and 'cend' to iterate through all elements and
    //:       verify the values are as expected.  (C-1..2, 4)
    //:
    //:     5 Monitor the memory allocated from both the default and object
    //:       allocators before and after calling the accessor; verify that
    //:       there is no change in total memory allocation.  (C-3)
    //
    // Testing:
    //   allocator_type get_allocator() const;
    //   const_iterator cbegin() const;
    //   const_iterator cend() const;
    //   size_type size() const;
    // ------------------------------------------------------------------------

    static const struct {
        int         d_line;       // source line number
        const char *d_spec_p;     // specification string
        const char *d_results_p;  // expected results
    } DATA[] = {
        //line  spec      result
        //----  --------  ------
        { L_,   "",       ""      },
        { L_,   "A",      "A"     },
        { L_,   "AB",     "AB"    },
        { L_,   "ABC",    "ABC"   },
        { L_,   "ABCD",   "ABCD"  },
        { L_,   "ABCDE",  "ABCDE" }
    };
    const int NUM_DATA = static_cast<const int>(sizeof DATA / sizeof *DATA);

    if (verbose) { printf(
                "\nCreate objects with various allocator configurations.\n"); }
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const size_t      LENGTH = strlen(DATA[ti].d_results_p);
            const TestValues  EXP(DATA[ti].d_results_p);

            if (verbose) { P_(LINE) P_(LENGTH) P(SPEC); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator da("default",    veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint",  veryVeryVeryVerbose);
                bslma::TestAllocator sa1("supplied1", veryVeryVeryVerbose);
                bslma::TestAllocator sa2("supplied2", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                    objPtr = new (fa) Obj();
                    objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                    objPtr = new (fa) Obj(0);
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
                  } return;                                           // RETURN
                }

                Obj& mX = *objPtr;  const Obj& X = gg(&mX, SPEC);
                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = ('c' == CONFIG || 'd' == CONFIG)
                                            ? da
                                            : sa1;

                // --------------------------------------------------------

                // Verify basic accessor

                bslma::TestAllocatorMonitor oam(&oa);

                ASSERTV(LINE, SPEC, CONFIG, &oa == X.get_allocator());
                ASSERTV(LINE, SPEC, CONFIG, LENGTH == X.size());

                size_t i = 0;
                for (CIter iter = X.cbegin(); iter != X.cend(); ++iter, ++i) {
                    ASSERTV(LINE, SPEC, CONFIG, EXP[i] == *iter);
                }

                ASSERTV(LINE, SPEC, CONFIG, LENGTH == i);

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

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase3()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMITIVE GENERATOR FUNCTIONS 'gg' AND 'ggg'
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
    //   int ggg(map *object, const char *spec, bool verbose = true);
    //   map& gg(map *object, const char *spec);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa(veryVeryVeryVerbose);

    if (verbose) printf("\nTesting generator on valid specs.\n");
    {
        static const struct {
            int         d_line;       // source line number
            const char *d_spec_p;     // specification string
            const char *d_results_p;  // expected element values
        } DATA[] = {
            //line  spec      results
            //----  --------  -------
            { L_,   "",       ""      },
            { L_,   "A",      "A"     },
            { L_,   "B",      "B"     },
            { L_,   "Z",      "Z"     },
            { L_,   "a",      "a"     },
            { L_,   "z",      "z"     },
            { L_,   "AB",     "AB"    },
            { L_,   "CD",     "CD"    },
            { L_,   "ABC",    "ABC"   },
            { L_,   "ABCD",   "ABCD"  },
            { L_,   "AbCdE",  "AbCdE" },

        };
        const int NUM_DATA =
                            static_cast<const int>(sizeof DATA / sizeof *DATA);

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const size_t      LENGTH = strlen(DATA[ti].d_results_p);
            const TestValues  EXP(DATA[ti].d_results_p);
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
                T_ T_ T_ P(X);
                T_ T_ T_ P(Y);
            }

            ASSERTV(LINE, LENGTH == X.size());
            ASSERTV(LINE, LENGTH == Y.size());
            ASSERTV(0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(0 == verifyContainer(Y, EXP, LENGTH));
        }
    }

    if (verbose) printf("\nTesting generator on invalid specs.\n");
    {
        static const struct {
            int         d_line;     // source line number
            const char *d_spec_p;   // specification string
            int         d_index;    // offending character index
        } DATA[] = {
            //line  spec      index
            //----  --------  -----
            { L_,   "",       -1,     }, // control

            { L_,   "A",      -1,     }, // control
            { L_,   " ",       0,     },
            { L_,   ".",       0,     },

            { L_,   "AE",     -1,     }, // control
            { L_,   ".~",      0,     },
            { L_,   "~!",      0,     },
            { L_,   "  ",      0,     },

            { L_,   "ABC",    -1,     }, // control
            { L_,   " BC",     0,     },
            { L_,   "A C",     1,     },
            { L_,   "AB ",     2,     },
            { L_,   "?#:",     0,     },
            { L_,   "   ",     0,     },

            { L_,   "ABCDE",  -1,     }, // control
            { L_,   "2BCDE",   0,     },
            { L_,   "AB@DE",   2,     },
            { L_,   "ABCD$",   4,     },
            { L_,   "A*C=E",   1,     }
        };
        const int NUM_DATA =
                            static_cast<const int>(sizeof DATA / sizeof *DATA);

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const int         INDEX  = DATA[ti].d_index;
            const size_t      LENGTH = strlen(SPEC);

            Obj mX(&oa);

            if ((int)LENGTH != oldLen) {
                if (verbose) printf("\tof length " ZU ":\n", LENGTH);
                ASSERTV(LINE, oldLen <= (int)LENGTH);  // non-decreasing
                oldLen = static_cast<int>(LENGTH);
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int RESULT = ggg(&mX, SPEC, veryVerbose);

            ASSERTV(LINE, INDEX == RESULT);
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase2()
{
    // ------------------------------------------------------------------------
    // DEFAULT CTOR, PRIMARY MANIPULATORS (BOOTSTRAP), & DTOR
    //   The basic concern is that the default constructor, the destructor,
    //   and, under normal conditions (i.e., no aliasing), the primary
    //   manipulators:
    //     - insert(value_type&&) (via helper function 'primaryManipulator')
    //     - clear
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
    //: 7 There is no temporary allocation from any allocator.
    //:
    //: 8 Every object releases any allocated memory at destruction.
    //:
    //: 9 QoI: The default constructor allocates no memory.
    //:
    //:10 'insert' adds an additional element to the object if the element
    //:   being inserted does not already exist.
    //:
    //:11 'insert' returns a pair with an iterator of the element that was just
    //:   inserted or the element that already exist in the object, and a
    //:   boolean indicating whether element being inserted already exist in
    //:   the object.
    //:
    //:12 'clear' properly destroys each contained element value.
    //:
    //:13 'clear' does not allocate memory.
    //:
    //:14 Any argument can be 'const'.
    //:
    //:15 Any memory allocation is exception neutral.
    //
    // TBD Missing concerns that the correct comparator is used.  We should be
    // testing with a stateful comparator (testing two states) and the default
    // comparator.  A (stateful) comparator that simply holds an ID would be
    // good enough.  (also test cases 12 & 33).
    //
    // Plan:
    //: 1 For each value of increasing length, 'L':
    //:
    //:   2 Using a loop-based approach, default-construct three distinct
    //:     objects, in turn, but configured differently: (a) without passing
    //:     an allocator, (b) passing a null allocator address explicitly,
    //:     and (c) passing the address of a test allocator distinct from the
    //:     default.  For each of these three iterations:  (C-1..14)
    //:
    //:     1 Create three 'bslma::TestAllocator' objects, and install one as
    //:       the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     2 Use the default constructor to dynamically create an object
    //:       'X', with its object allocator configured appropriately (see
    //:       P-2); use a distinct test allocator for the object's footprint.
    //:
    //:     3 Use the (as yet unproven) 'get_allocator' to ensure that its
    //:       object allocator is properly installed.  (C-2..4)
    //:
    //:     4 Use the appropriate test allocators to verify that no memory is
    //:       allocated by the default constructor.  (C-9)
    //:
    //:     5 Use the individual (as yet unproven) salient attribute accessors
    //:       to verify the default-constructed value.  (C-1)
    //:
    //:     6 Insert 'L - 1' elements in order of increasing value into the
    //:       container.
    //:
    //:     7 Insert the 'L'th value in the presence of exceptions and use the
    //:       (as yet unproven) basic accessors to verify the container has the
    //:       expected values.  Verify the number of allocation is as expected.
    //:       (C-5..6, 13..14)
    //:
    //:     8 Verify that no temporary memory is allocated from the object
    //:       allocator.  (C-7)
    //:
    //:     9 Invoke 'clear' and verify that the container is empty.  Verify
    //:       that no memory is allocated.  (C-11..12)
    //:
    //:    10 Verify that all object memory is released when the object is
    //:       destroyed.  (C-8)
    //
    // Testing:
    //   map(const C& comparator, const A& allocator);
    //   map(const A& allocator);
    //   ~map();
    //   BOOTSTRAP: pair<iterator, bool> insert(value_type&& value);
    //   void clear();
    // ------------------------------------------------------------------------

    if (verbose) printf(
                 "\nDEFAULT CTOR, PRIMARY MANIPULATORS (BOOTSTRAP), & DTOR"
                 "\n======================================================\n");

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value +
                           bslma::UsesBslmaAllocator<VALUE>::value;

    if (verbose) { P(TYPE_ALLOC); }

    const TestValues VALUES;  // contains 52 distinct increasing values

    const size_t MAX_LENGTH = 9;

    for (size_t ti = 0; ti < MAX_LENGTH; ++ti) {
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

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) Obj();
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objPtr = new (fa) Obj(0);
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objPtr = new (fa) Obj(&sa);
                objAllocatorPtr = &sa;
              } break;
              default: {
                ASSERTV(CONFIG, !"Bad allocator config.");
              } return;                                               // RETURN
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify any attribute allocators are installed properly.

            ASSERTV(LENGTH, CONFIG, &oa == X.get_allocator());

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(LENGTH, CONFIG, oa.numBlocksTotal(),
                    0 ==  oa.numBlocksTotal());
            ASSERTV(LENGTH, CONFIG, noa.numBlocksTotal(),
                    0 == noa.numBlocksTotal());

            ASSERTV(LENGTH, CONFIG, 0          == X.size());
            ASSERTV(LENGTH, CONFIG, X.cbegin() == X.cend());

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\n\tTesting 'insert' (bootstrap).\n"); }

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            if (0 < LENGTH) {
                if (verbose) {
                    printf("\t\tOn an object of initial length " ZU ".\n",
                           LENGTH);
                }

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    int id = TstFacility::getIdentifier(VALUES[tj].first);

                    pair<Iter, bool> RESULT =
                                         primaryManipulator(&mX, id, &scratch);

                    ASSERTV(LENGTH, tj, CONFIG, true       == RESULT.second);
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == *(RESULT.first));
                    ASSERTV(LENGTH, tj, tj + 1 == X.size());
                }
                ASSERTV(LENGTH, CONFIG, LENGTH == X.size());

                if (veryVerbose) {
                    printf("\t\t\tBEFORE: ");
                    P(X);
                }

                // Verify behavior when the key already exists in the object.

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    int id = TstFacility::getIdentifier(VALUES[tj].first);
                    ASSERTV(isupper(id));

                    // Attempt to insert the same key paired with a different
                    // mapped value than was initially inserted into the map.

                    pair<Iter, bool> RESULT = primaryManipulator(&mX,
                                                                 tolower(id),
                                                                 &scratch);

                    ASSERTV(LENGTH, tj, CONFIG, false      == RESULT.second);
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == *(RESULT.first));
                }
                ASSERTV(LENGTH, CONFIG, LENGTH == X.size());
            }

            // ----------------------------------------------------------------

            if (veryVerbose) printf("\n\tTesting 'clear'.\n");
            {
                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                mX.clear();

                ASSERTV(LENGTH, CONFIG,          0 == X.size());
                ASSERTV(LENGTH, CONFIG, X.cbegin() == X.cend());

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LENGTH, CONFIG, BB == AA);
                ASSERTV(LENGTH, CONFIG, B, A,
                        B - (int)LENGTH * TYPE_ALLOC == A);

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    int id = TstFacility::getIdentifier(VALUES[tj].first);

                    pair<Iter, bool> RESULT =
                                         primaryManipulator(&mX, id, &scratch);

                    ASSERTV(LENGTH, tj, CONFIG, true       == RESULT.second);
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == *(RESULT.first));
                }

                ASSERTV(LENGTH, CONFIG, LENGTH == X.size());
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

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase1(const COMP&  comparator,
                                                    KEY         *testKeys,
                                                    VALUE       *testValues,
                                                    size_t       numValues)
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
    //: 1 Execute each methods to verify functionality for simple case.
    //
    // Testing:
    //   BREATHING TEST
    // ------------------------------------------------------------------------

    typedef bsl::map<KEY, VALUE, COMP>  Obj;
    typedef typename Obj::iterator               iterator;
    typedef typename Obj::const_iterator         const_iterator;
    typedef typename Obj::reverse_iterator       reverse_iterator;
    typedef typename Obj::const_reverse_iterator const_reverse_iterator;

    typedef typename Obj::value_type             Value;
    typedef pair<iterator, bool>                 InsertResult;

    bslma::TestAllocator defaultAllocator("defaultAllocator",
                                          veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultGuard(&defaultAllocator);

    bslma::TestAllocator objectAllocator("objectAllocator",
                                         veryVeryVeryVerbose);

    // Sanity check.

    ASSERTV(0 < numValues);
    ASSERTV(8 > numValues);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Default construct an empty map.\n");
    }
    {
        Obj x(&objectAllocator);  const Obj& X = x;
        ASSERTV(0    == X.size());
        ASSERTV(true == X.empty());
        ASSERTV(0    <  X.max_size());
        ASSERTV(0    == defaultAllocator.numBytesInUse());
        ASSERTV(0    == objectAllocator.numBytesInUse());
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Test use of allocators.\n");
    }
    {
        bslma::TestAllocator objectAllocator1("objectAllocator1",
                                              veryVeryVeryVerbose);
        bslma::TestAllocator objectAllocator2("objectAllocator2",
                                              veryVeryVeryVerbose);

        Obj o1(comparator, &objectAllocator1);  const Obj& O1 = o1;
        ASSERTV(&objectAllocator1 == O1.get_allocator().mechanism());

        for (size_t i = 0; i < numValues; ++i) {
            o1.insert(Value(testKeys[i], testValues[i]));
        }
        ASSERTV(numValues == O1.size());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 == objectAllocator2.numBytesInUse());

        bslma::TestAllocatorMonitor monitor1(&objectAllocator1);
        Obj o2(O1, &objectAllocator2);  const Obj& O2 = o2;
        ASSERTV(&objectAllocator2 == O2.get_allocator().mechanism());

        ASSERTV(numValues == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(monitor1.isInUseSame());
        ASSERTV(monitor1.isTotalSame());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 <  objectAllocator2.numBytesInUse());

        Obj o3(comparator, &objectAllocator1);  const Obj& O3 = o3;
        ASSERTV(&objectAllocator1 == O3.get_allocator().mechanism());

        ASSERTV(numValues == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(0         == O3.size());
        ASSERTV(monitor1.isInUseSame());
        ASSERTV(monitor1.isTotalSame());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 <  objectAllocator2.numBytesInUse());

        o1.swap(o3);
        ASSERTV(0         == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(numValues == O3.size());
        ASSERTV(monitor1.isInUseSame());
        ASSERTV(monitor1.isTotalSame());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 <  objectAllocator2.numBytesInUse());

        o3.swap(o2);
        ASSERTV(0         == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(numValues == O3.size());
        ASSERTV(!monitor1.isInUseUp());  // Memory usage may go down depending
                                         // on implementation
        ASSERTV(monitor1.isTotalUp());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 <  objectAllocator2.numBytesInUse());

        ASSERTV(&objectAllocator1 == O1.get_allocator().mechanism());
        ASSERTV(&objectAllocator2 == O2.get_allocator().mechanism());
        ASSERTV(&objectAllocator1 == O3.get_allocator().mechanism());
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Test primary manipulators/accessors on every permutation.\n");
    }

    native_std::sort(testKeys, testKeys + numValues, comparator);
    do {
        // For each possible permutation of values, insert values, iterate over
        // the resulting container, find values, and then erase values.

        Obj x(comparator, &objectAllocator);  const Obj& X = x;
        for (size_t i = 0; i < numValues; ++i) {
            Obj y(X, &objectAllocator);  const Obj& Y = y;
            Obj z(X, &objectAllocator);  const Obj& Z = z;
            ASSERTV(X == Y);
            ASSERTV(!(X != Y));

            ASSERTV(i, X.end() == X.find(testKeys[i]));

            // Test 'insert'.
            Value value(testKeys[i], testValues[i]);
            InsertResult result = x.insert(value);
            ASSERTV(X.end()       != result.first);
            ASSERTV(true          == result.second);
            ASSERTV(testKeys[i]   == result.first->first);
            ASSERTV(testValues[i] == result.first->second);


            // Test size, empty.
            ASSERTV(i + 1 == X.size());
            ASSERTV(false == X.empty());

            // Test insert duplicate key.
            ASSERTV(result.first == x.insert(value).first);
            ASSERTV(false        == x.insert(value).second);
            ASSERTV(i + 1        == X.size());

            // Test find, operator[], at.
            ASSERTV(result.first  == X.find(testKeys[i]));
            ASSERTV(testValues[i] == x[testKeys[i]]);
            ASSERTV(testValues[i] == x.at(testKeys[i]));
            ASSERTV(testValues[i] == X.at(testKeys[i]));

            // Test operator[]
            ASSERTV(!(X == Z));
            ASSERTV(  X != Z);
            ASSERTV(VALUE() == z[testKeys[i]]);
            z[testKeys[i]] = testValues[i];
            ASSERTV(testValues[i] == z[testKeys[i]]);
            ASSERTV( (X == Z));
            ASSERTV(!(X != Z));


            ASSERTV(X != Y);
            ASSERTV(!(X == Y));

            y = x;
            ASSERTV(X == Y);
            ASSERTV(!(X != Y));
        }

        ASSERTV(0 != objectAllocator.numBytesInUse());
        ASSERTV(0 == defaultAllocator.numBytesInUse());
        // Verify sorted order of elements.

        {
            const_iterator last = X.begin();
            const_iterator it   = ++(X.begin());
            while (it != X.end()) {
                ASSERTV(comparator(last->first, it->first));
                ASSERTV(comparator((*last).first, (*it).first));

                last = it;
                ++it;
            }
        }

        // Test iterators.
        {
            const_iterator cbi  = X.begin();
            const_iterator ccbi = X.cbegin();
            iterator       bi   = x.begin();

            const_iterator last = X.begin();
            while (cbi != X.end()) {
                ASSERTV(cbi == ccbi);
                ASSERTV(cbi == bi);

                if (cbi != X.begin()) {
                    ASSERTV(comparator(last->first, cbi->first));
                }
                last = cbi;
                ++bi; ++ccbi; ++cbi;
            }

            ASSERTV(cbi  == X.end());
            ASSERTV(ccbi == X.end());
            ASSERTV(bi   == X.end());
            --bi; --ccbi; --cbi;

            reverse_iterator       ri   = x.rbegin();
            const_reverse_iterator rci  = X.rbegin();
            const_reverse_iterator rcci = X.crbegin();

            while  (rci != X.rend()) {
                ASSERTV(cbi == ccbi);
                ASSERTV(cbi == bi);
                ASSERTV(rci == rcci);
                ASSERTV(ri->first == rcci->first);

                if (rci !=  X.rbegin()) {
                    ASSERTV(comparator(cbi->first, last->first));
                    ASSERTV(comparator(rci->first, last->first));
                }

                last = cbi;
                if (cbi != X.begin()) {
                    --bi; --ccbi; --cbi;
                }
                ++ri; ++rcci; ++rci;
            }
            ASSERTV(cbi  == X.begin());
            ASSERTV(ccbi == X.begin());
            ASSERTV(bi   == X.begin());

            ASSERTV(rci  == X.rend());
            ASSERTV(rcci == X.rend());
            ASSERTV(ri   == x.rend());
        }

        // Use erase(iterator) on all the elements.
        for (size_t i = 0; i < numValues; ++i) {
            const_iterator it     = x.find(testKeys[i]);
            const_iterator nextIt = it;
            ++nextIt;

            ASSERTV(X.end()       != it);
            ASSERTV(testKeys[i]   == it->first);
            ASSERTV(testValues[i] == it->second);

            const_iterator resIt     = x.erase(it);
            ASSERTV(resIt             == nextIt);
            ASSERTV(numValues - i - 1 == X.size());
            if (resIt != X.end()) {
                ASSERTV(comparator(testKeys[i], resIt->first));
            }
        }
    } while (native_std::next_permutation(testKeys,
                                          testKeys + numValues,
                                          comparator));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    native_std::sort(testKeys, testKeys + numValues, comparator);
    if (veryVerbose) {
        printf("Test 'lower_bound' and 'upper_bound'.\n");
    }
    {
        Obj x(comparator, &objectAllocator);  const Obj& X = x;

        // Insert every other value into the map.
        for (size_t i = 0; i < numValues; ++i) {
            if (i % 2) {
                Value value(testKeys[i], testValues[i]);
                x.insert(value);
            }
        }

        for (size_t i = 0; i < numValues; ++i) {
            iterator       li = x.lower_bound(testKeys[i]);
            const_iterator LI = X.lower_bound(testKeys[i]);
            iterator       ui = x.upper_bound(testKeys[i]);
            const_iterator UI = X.upper_bound(testKeys[i]);

            ASSERTV(li == LI);
            ASSERTV(ui == UI);

            // If test value 'i' was inserted in the map then 'lower_bound'
            // will return an iterator to that value; otherwise, 'lower_bound'
            // will return an iterator to the subsequent inserted value if one
            // exists, and the end iterator otherwise.
            const_iterator EXP_LOWER = i % 2
                                       ? X.find(testKeys[i])
                                       : i + 1 < numValues
                                             ? X.find(testKeys[i+1])
                                             : X.end();

            // If test value 'i' was inserted in the map, then 'upper_bound'
            // should return an iterator to the subsequent value as
            // 'lower_bound', and the same iterator otherwise.
            const_iterator EXP_UPPER = EXP_LOWER;
            if (i % 2) {
                ++EXP_UPPER;
            }

            ASSERTV(EXP_LOWER == li);
            ASSERTV(EXP_LOWER == LI);
            ASSERTV(EXP_UPPER == ui);
            ASSERTV(EXP_UPPER == UI);
        }
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    native_std::random_shuffle(testKeys,  testKeys + numValues);
    if (veryVerbose) {
        printf("Test 'erase(const key_type&)'.\n");
    }
    {
        Obj x(comparator, &objectAllocator);  const Obj& X = x;
        for (size_t i = 0; i < numValues; ++i) {
            Value value(testKeys[i], testValues[i]);
            InsertResult result = x.insert(value);
        }

        for (size_t i = 0; i < numValues; ++i) {
            ASSERTV(1 == x.erase(testKeys[i]));
            ASSERTV(0 == x.erase(testKeys[i]));
            ASSERTV(numValues - i - 1 == X.size());
        }
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Test 'erase(const_iterator, const_iterator )'.\n");
    }
    {
        for (size_t i = 0; i < numValues; ++i) {
            for (size_t j = 0; j < numValues; ++j) {
                Obj x(comparator, &objectAllocator);  const Obj& X = x;
                for (size_t k = 0; k < numValues; ++k) {
                    Value value(testKeys[k], testValues[k]);
                    InsertResult result = x.insert(value);
                }

                const_iterator a = X.find(testKeys[i]);
                const_iterator b = X.find(testKeys[j]);

                if (!comparator(testKeys[i], testKeys[j])) {
                    native_std::swap(a, b);
                }
                KEY min = a->first;
                KEY max = b->first;
                ASSERTV(!comparator(max, min)); // min <= max

                size_t numElements = bsl::distance(a, b);
                iterator endPoint = x.erase(a, b);

                ASSERTV(numValues - numElements == X.size());
                ASSERTV(endPoint                == b);

                for (size_t k = 0; k < numValues; ++k) {
                    if (comparator(testKeys[k], min) ||
                        !comparator(testKeys[k], max)) {
                        ASSERTV(testKeys[k] == X.find(testKeys[k])->first);
                    }
                    else {
                        ASSERTV(X.end() == X.find(testKeys[k]));
                    }
                }
            }
        }

        // Test 'erase(const_iterator, const_iterator )' for end of range.
        for (size_t i = 0; i < numValues; ++i) {
            Obj x(comparator, &objectAllocator);  const Obj& X = x;
            for (size_t k = 0; k < numValues - 1; ++k) {
                // Insert 1 fewer than the total number of keys.

                Value value(testKeys[k], testValues[k]);
                InsertResult result = x.insert(value);
            }

            const_iterator a = X.find(testKeys[i]);
            const_iterator b = X.end();
            size_t numElements = bsl::distance(a, b);
            iterator endPoint = x.erase(a, b);

            ASSERTV(numValues - numElements - 1 == X.size());
            ASSERTV(endPoint                    == b);
        }
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Test insert & map for iterator ranges.\n");
    }
    {

        typedef pair<KEY, VALUE> NonConstValue;
        NonConstValue *myValues = new NonConstValue[numValues];
        for (size_t i = 0; i < numValues; ++i) {
            myValues[i].first  = testKeys[i];
            myValues[i].second = testValues[i];
        }

        for (size_t i = 0; i < numValues; ++i) {
            for (size_t length = 0; length <= numValues - i; ++length) {
                Obj x(comparator, &objectAllocator);  const Obj& X = x;
                for (size_t k = 0; k < length; ++k) {
                    size_t index = i + k;
                    InsertResult result = x.insert(myValues[index]);
                }
                Obj y(comparator, &objectAllocator);  const Obj& Y = y;
                y.insert(myValues + i, myValues + (i + length));

                Obj z(myValues + i,
                      myValues + (i + length),
                      comparator,
                      &objectAllocator);
                const Obj& Z = z;
                ASSERTV(X == Y);
                ASSERTV(X == Z);
            }
        }
        delete [] myValues;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Test 'equal_range'.\n");
    }
    {
        Obj x(comparator, &objectAllocator);  const Obj& X = x;
        for (size_t i = 0; i < numValues; ++i) {
            Value value(testKeys[i], testValues[i]);
            InsertResult result = x.insert(value);
        }

        for (size_t i = 0; i < numValues; ++i) {
            pair<iterator, iterator> result = x.equal_range(testKeys[i]);
            pair<const_iterator, const_iterator> cresult =
                                                  X.equal_range(testKeys[i]);

            ASSERTV(cresult.first  == result.first);
            ASSERTV(cresult.second == result.second);

            ASSERTV(result.first->first == testKeys[i]);
            ASSERTV(X.end() == result.second ||
                   result.second->first != testKeys[i]);
        }
        for (size_t i = 0; i < numValues; ++i) {
            x.erase(testKeys[i]);
            pair<iterator, iterator> result = x.equal_range(testKeys[i]);
            pair<const_iterator, const_iterator> cresult =
                                                  x.equal_range(testKeys[i]);

            iterator       li = x.lower_bound(testKeys[i]);
            const_iterator LI = X.lower_bound(testKeys[i]);
            iterator       ui = x.upper_bound(testKeys[i]);
            const_iterator UI = X.upper_bound(testKeys[i]);

            ASSERTV(result.first   == li);
            ASSERTV(result.second  == ui);
            ASSERTV(cresult.first  == LI);
            ASSERTV(cresult.second == UI);
        }
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Test 'operator<', 'operator>', 'operator<=', 'operator>='.\n");
    }
    {
        // Iterate over possible selections of elements to add to two
        // containers, 'X' and 'Y' then compare the results of the comparison
        // operators to an "oracle" result from
        // 'bslalg::RangeCompare::lexicographical' over the same range.

        for (size_t i = 0; i < numValues; ++i) {
            for (size_t j = 0; j < numValues; ++j) {
                for (size_t length = 0; length < numValues; ++length) {
                    Obj x(comparator, &objectAllocator);  const Obj& X = x;
                    Obj y(comparator, &objectAllocator);  const Obj& Y = y;
                    for (size_t k = 0; k < j; ++k) {
                        size_t xIndex = (i + length) % numValues;
                        size_t yIndex = (j + length) % numValues;

                        Value xValue(testKeys[xIndex], testValues[xIndex]);
                        x.insert(xValue);
                        Value yValue(testKeys[yIndex], testValues[yIndex]);
                        y.insert(yValue);
                    }

                    int comp = bslalg::RangeCompare::lexicographical(X.begin(),
                                                                    X.end(),
                                                                    Y.begin(),
                                                                    Y.end());
                    ASSERTV((comp < 0)  == (X < Y));
                    ASSERTV((comp > 0)  == (X > Y));
                    ASSERTV((comp <= 0) == (X <= Y));
                    ASSERTV((comp >= 0) == (X >= Y));
                }
            }
        }
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    native_std::sort(testKeys, testKeys + numValues, comparator);
    if (veryVerbose) {
        printf("Test 'key_comp' and 'value_comp'.\n");
    }
    {
        Obj x(comparator, &objectAllocator);  const Obj& X = x;
        typename Obj::key_compare   keyComp   = X.key_comp();
        typename Obj::value_compare valueComp = X.value_comp();
        for (size_t i = 0; i < numValues - 1; ++i) {
            ASSERTV(keyComp(testKeys[i], testKeys[i+1]));
            ASSERTV(valueComp(Value(testKeys[i],   testValues[i]),
                              Value(testKeys[i+1], testValues[i+1])));
        }
    }
}

template <class KEY,
          class VALUE = KEY,
          class COMP  = TestComparator<KEY> >
struct MetaTestDriver {
    // This 'struct' is to be call by the 'RUN_EACH_TYPE' macro, and the
    // functions within it dispatch to functions in 'TestDriver' instantiated
    // with different types of allocator.

    typedef bsl::pair<const KEY, VALUE>     Pair;
    typedef bsl::allocator<Pair>            BAP;
    typedef bsltf::StdAllocatorAdaptor<BAP> SAA;

    static void testCase28();
        // Test move-sassign.

    static void testCase8();
        // Test member and free 'swap'.
};

template <class KEY, class VALUE, class COMP>
void MetaTestDriver<KEY, VALUE, COMP>::testCase28()
{
    // The low-order bit of the identifier specifies whether the fourth boolean
    // argument of the stateful allocator, which indicates propagate on
    // move-assign, is set.

    typedef bsltf::StdStatefulAllocator<Pair, false, false, false, false> S00;
    typedef bsltf::StdStatefulAllocator<Pair, false, false, false,  true> S01;
    typedef bsltf::StdStatefulAllocator<Pair,  true,  true,  true, false> S10;
    typedef bsltf::StdStatefulAllocator<Pair,  true,  true,  true,  true> S11;

    if (verbose) printf("\n");

    TestDriver<KEY, VALUE, COMP, BAP>::testCase28_dispatch();

    TestDriver<KEY, VALUE, COMP, SAA>::testCase28_dispatch();

    TestDriver<KEY, VALUE, COMP, S00>::testCase28_dispatch();
    TestDriver<KEY, VALUE, COMP, S01>::testCase28_dispatch();
    TestDriver<KEY, VALUE, COMP, S10>::testCase28_dispatch();
    TestDriver<KEY, VALUE, COMP, S11>::testCase28_dispatch();
}

template <class KEY, class VALUE, class COMP>
void MetaTestDriver<KEY, VALUE, COMP>::testCase8()
{
    // The low-order bit of the identifier specifies whether the third boolean
    // argument of the stateful allocator, which indicates propagate on
    // container swap, is set.

    typedef bsltf::StdStatefulAllocator<Pair, false, false, false, false> S00;
    typedef bsltf::StdStatefulAllocator<Pair, false, false,  true, false> S01;
    typedef bsltf::StdStatefulAllocator<Pair,  true,  true, false,  true> S10;
    typedef bsltf::StdStatefulAllocator<Pair,  true,  true,  true,  true> S11;

    if (verbose) printf("\n");

    TestDriver<KEY, VALUE, COMP, BAP>::testCase8_dispatch();

    TestDriver<KEY, VALUE, COMP, SAA>::testCase8_dispatch();

    TestDriver<KEY, VALUE, COMP, S00>::testCase8_dispatch();
    TestDriver<KEY, VALUE, COMP, S01>::testCase8_dispatch();
    TestDriver<KEY, VALUE, COMP, S10>::testCase8_dispatch();
    TestDriver<KEY, VALUE, COMP, S11>::testCase8_dispatch();
}


//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace UsageExample {

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Creating a Trade Matching System
///- - - - - - - - - - - - - - - - - - - - - -
// In this example, we will utilize 'bsl::map' to define and implement a class,
// 'TradeMatcher', that provides a simple trade matching system for a single
// stock.  The manipulators of 'TradeMatcher' will allow clients to place buy
// orders and sell orders, and the accessors of 'TradeMatcher' will allow
// clients to retrieve active orders and past executions.
//
// First, we define the public interface for 'TradeMatcher':
//..
class TradeMatcher {
    // This class provides a mechanism that characterizes a simple trade
    // matching system for one stock.  An object of this class allows clients
    // to place orders and view the active orders.
//..
// Here, we create two type aliases, 'SellOrdersMap' and 'BuyOrdersMap', for
// two 'bsl::map' instantiations that maps the price of an order (type
// 'double') to the quantity of the order (type 'int').  'SellOrdersMap' uses
// the default 'bsl::less' comparator to store the sequence of sell orders in
// ascending price order.  'BuyOrdersMap' uses the 'bsl::greater' comparator to
// store the sequence of buy orders in descending price order.  Also note that
// we use the default 'ALLOCATOR' template parameter for both aliases as we
// intend to provide memory with 'bslma' style allocators:
//..
    // PRIVATE TYPES
    typedef bsl::map<double, int> SellOrdersMap;
        // This 'typedef' is an alias for a mapping between the price and
        // quantity of an order in ascending price order.

    typedef bsl::map<double, int, std::greater<double> > BuyOrdersMap;
        // This 'typedef' is an alias for a mapping between the price and
        // quantity of an order in descending price order.

    typedef bsl::vector<bsl::pair<double, int> > ExecutionVector;
        // This 'typedef' is an alias for a 'vector' of executions, each of
        // which comprises the execution price and quantity.

    // DATA
    SellOrdersMap   d_sellOrders;  // current sell orders
    BuyOrdersMap    d_buyOrders;   // current buy orders

  private:
    // NOT IMPLEMENTED
    TradeMatcher& operator=(const TradeMatcher&);
    TradeMatcher(const TradeMatcher&);

  public:
    // PUBLIC TYPES
    typedef SellOrdersMap::const_iterator SellOrdersConstIterator;
        // This 'typedef' provides an alias for the type of an iterator
        // providing non-modifiable access to sell orders in a 'TradeMatcher'.

    typedef BuyOrdersMap::const_iterator BuyOrdersConstIterator;
        // This 'typedef' provides an alias for the type of an iterator
        // providing non-modifiable access to buy orders in a 'TradeMatcher'.

    // CREATORS
    explicit
    TradeMatcher(bslma::Allocator *basicAllocator = 0);
        // Create an empty 'TradeMatcher' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    //! ~TradeMatcher() = default;
        // Destroy this object.

    // MANIPULATORS
    void placeBuyOrder(double price, int numShares);
        // Place an order to buy the specified 'numShares' at the specified
        // 'price'.  The placed buy order will (possibly partially) execute
        // when active sale orders exist in the system at or below 'price'.
        // The behavior is undefined unless '0 < price' and '0 < numShares'.

    void placeSellOrder(double price, int numShares);
        // Place an order to sell the specified 'numShares' at the specified
        // 'price'.  The placed sell order will (possibly partially) execute
        // when active buy orders exist in the system at or above 'price'.  The
        // behavior is undefined unless '0 < price' and '0 < numShares'.

    // ACCESSORS
    SellOrdersConstIterator beginSellOrders() const;
        // Return an iterator providing non-modifiable access to the active
        // sell order at the lowest price in the ordered sequence (from low
        // price to high price) of sell orders maintained by this object.

    SellOrdersConstIterator endSellOrders() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end sell order in the ordered sequence (from low price to
        // high price) of sell orders maintained by this object.

    BuyOrdersConstIterator beginBuyOrders() const;
        // Return an iterator providing non-modifiable access to the active buy
        // order at the highest price in the ordered sequence (from high price
        // to low price) of buy orders maintained by this object.

    BuyOrdersConstIterator endBuyOrders() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end buy order in the ordered sequence (from high price to
        // low price) of buy orders maintained by this object.
};

//..
// Now, we define the implementations methods of the 'TradeMatcher' class:
//..
// CREATORS
TradeMatcher::TradeMatcher(bslma::Allocator *basicAllocator)
: d_sellOrders(basicAllocator)
, d_buyOrders(basicAllocator)
{
}
//..
// Notice that, on construction, we pass the contained 'bsl::map' objects the
// 'bsl::Allocator' supplied at construction'.
//..
// MANIPULATORS
void TradeMatcher::placeBuyOrder(double price, int numShares)
{
    BSLS_ASSERT(0 < price);
    BSLS_ASSERT(0 < numShares);

    // Buy shares from sellers from the one with the lowest price up to but not
    // including the first seller with a price greater than the specified
    // 'price'.

    SellOrdersMap::iterator itr = d_sellOrders.begin();

    while (numShares && itr != d_sellOrders.upper_bound(price)) {
        if (itr->second > numShares) {
            itr->second -= numShares;
            numShares = 0;
            break;
        }

        itr = d_sellOrders.erase(itr);
        numShares -= itr->second;
    }

    if (numShares > 0) {
        d_buyOrders[price] += numShares;
    }
}

void TradeMatcher::placeSellOrder(double price, int numShares)
{
    BSLS_ASSERT(0 < price);
    BSLS_ASSERT(0 < numShares);

    // Sell shares to buyers from the one with the highest price up to but not
    // including the first buyer with a price smaller than the specified
    // 'price'.

    BuyOrdersMap::iterator itr = d_buyOrders.begin();

    while (numShares && itr != d_buyOrders.upper_bound(price)) {
        if (itr->second > numShares) {
            itr->second -= numShares;
            numShares = 0;
            break;
        }

        itr = d_buyOrders.erase(itr);
        numShares -= itr->second;
    }

    if (numShares > 0) {
        d_sellOrders[price] += numShares;
    }
}

// ACCESSORS
TradeMatcher::SellOrdersConstIterator TradeMatcher::beginSellOrders() const
{
    return d_sellOrders.begin();
}

TradeMatcher::SellOrdersConstIterator TradeMatcher::endSellOrders() const
{
    return d_sellOrders.end();
}

TradeMatcher::BuyOrdersConstIterator TradeMatcher::beginBuyOrders() const
{
    return d_buyOrders.begin();
}

TradeMatcher::BuyOrdersConstIterator TradeMatcher::endBuyOrders() const
{
    return d_buyOrders.end();
}
//..

}  // close namespace UsageExample

// ============================================================================
//                              MAIN PROGRAM
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

    switch (test) { case 0:
      case 38: {
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
        {
            using namespace UsageExample;
            bslma::TestAllocator defaultAllocator("defaultAllocator",
                                                  veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard defaultGuard(&defaultAllocator);

            bslma::TestAllocator objectAllocator("objectAllocator",
                                                 veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch",
                                         veryVeryVeryVerbose);

            TradeMatcher matcher(&objectAllocator);

            matcher.placeBuyOrder(15, 5);
            matcher.placeBuyOrder(20, 1);

            matcher.placeSellOrder(18, 2);

            matcher.placeBuyOrder(10, 20);
            matcher.placeSellOrder(16, 10);
            matcher.placeBuyOrder(17, 9);
            matcher.placeBuyOrder(16, 2);

            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 <  objectAllocator.numBytesInUse());
        }
      } break;
      case 37: {
        // --------------------------------------------------------------------
        // 'bslmf::MovableRef<T>' does not escape (in C+++03 mode)
        // --------------------------------------------------------------------
        TestDriver<int, AmbiguousTestType03>::testCase37();
      } break;
      case 36: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "'noexcept' SPECIFICATION" "\n"
                                 "========================" "\n");

        TestDriver<int>::testCase36();

      } break;
      case 35: {
        // --------------------------------------------------------------------
        // TESTING SUPPORT FOR INCOMPLETE TYPES
        //
        // Concerns:
        //: 1 The type can be declared with incomplete types.
        //
        // Plan:
        //: 1 Instantiate a test object that uses incomplete types in the class
        //:   declaration.  (C-1)
        //
        // Testing:
        //   CONCERN: 'map' supports incomplete types.
        // --------------------------------------------------------------------
        TestIncompleteType x;
        (void) x;
      } break;
      case 34: {
        // --------------------------------------------------------------------
        // TESTING ELEMENTAL ACCESS WITH MOVABLE KEY
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase34,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        TestDriver<BAD_MOVE_GUARD(bsltf::MovableAllocTestType),
                   BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType)>::testCase34();
#endif

        TestDriver<TestKeyType, TestValueType>::testCase34();
      } break;
      case 33: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS TAKING INITIALIZER LISTS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase33,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));
      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING EMPLACE WITH HINT
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase32,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonDefaultConstructibleTestType,
                      bsltf::NonOptionalAllocTestType,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

        RUN_EACH_TYPE(TestDriver,
                      testCase32a,
                      bsltf::EmplacableTestType,
                      bsltf::AllocEmplacableTestType);
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // TESTING EMPLACE
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase31,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonDefaultConstructibleTestType,
                      bsltf::NonOptionalAllocTestType,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

        RUN_EACH_TYPE(TestDriver,
                      testCase31a,
                      bsltf::EmplacableTestType,
                      bsltf::AllocEmplacableTestType);
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING INSERTION-WITH-HINT OF MOVABLE VALUES
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase30,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType),
                      BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType));
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING INSERTION OF MOVABLE VALUES
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase29,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType),
                      BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType));
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING MOVE-ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase28,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

        // Because the 'KEY' type in the pair is 'const', the move c'tor for
        // 'bsl::map' calls the copy c'tor of 'KEY', and move-assign calls that
        // move c'tor, we can't move-assign a container with a move-only 'KEY'.

        MetaTestDriver<BAD_MOVE_GUARD(bsltf::MovableAllocTestType),
                   BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType)>::testCase28();

        MetaTestDriver<int,
                   BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType)>::testCase28();
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTOR
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase27,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

        TestDriver<BAD_MOVE_GUARD(bsltf::MovableAllocTestType),
                   BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType)>::testCase27();

        TestDriver<int,
                   BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType)>::testCase27();
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING STANDARD INTERFACE COVERAGE
        // --------------------------------------------------------------------
        // Test only 'int' and 'char' parameter types, because map's
        // 'operator<' and related operators only support (template parameter)
        // types that define 'operator<'.

        RUN_EACH_TYPE(TestDriver, testCase26, int, char);
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS OF A TEMPLATE WRAPPER CLASS
        // --------------------------------------------------------------------
        // KEY/VALUE doesn't affect the test, so run the test only for 'int'.

        TestDriver<int, int>::testCase25();
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING ELEMENTAL ACCESS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase24,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

        TestDriver<TestKeyType, TestValueType>::testCase24();
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase23,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

        TestDriver<TestKeyType, TestValueType>::testCase23();
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING STL ALLOCATOR
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(StdAllocTestDriver,
                      testCase22,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        StdAllocTestDriver<TestKeyType, TestValueType>::testCase22();
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING COMPARATOR
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase21,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        TestDriver<TestKeyType, TestValueType>::testCase21();
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'max_size' and 'empty'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase20,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        TestDriver<TestKeyType, TestValueType>::testCase20();
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING FREE COMPARISON OPERATORS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase19,
                      signed char,
                      size_t,
                      bsltf::TemplateTestFacility::ObjectPtr);

        // Note relational operators between components compare pair.second
        // as well, and do not use 'COMP', but rather, naked
        // 'operator<(const pair&, const pair&)', which severely limits the
        // variety of types we can use here.

// TBD Testing only 'map<int, char>' misses all of the interesting concerns.
// (See the TBD regarding 'DEFAULT_DATA' above.)
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'erase'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase18,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType),
                      BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType));

        TestDriver<TestKeyType, TestValueType>::testCase18();
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING RANGE 'insert'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase17,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

        TestDriver<TestKeyType, TestValueType>::testCase17();
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING INSERTION-WITH-HINT OF SINGLE VALUE
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase16,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

        TestDriver<TestKeyType, TestValueType>::testCase16();
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING INSERTION OF SINGLE VALUE
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase15,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

        TestDriver<TestKeyType, TestValueType>::testCase15();
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ITERATORS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

        TestDriver<TestKeyType, TestValueType>::testCase14();
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING SEARCH FUNCTIONS ('find', 'count', etc.)
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType),
                      BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType));

        TestDriver<TestKeyType, TestValueType>::testCase13();
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // RANGE (TEMPLATE) CONSTRUCTORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Value Constructor"
                            "\n=========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

        TestDriver<TestKeyType, TestValueType>::testCase12();

// TBD We should test inserting move-only values from a range given by
// move-enabled iterators.
//
// We should also test that constructing from a range using a different
// allocator (e.g., a range of 'string' values mapped by the key) does not
// incur making a temporary with the default allocator for each iteration.
// This might be a separate test case (12a?) for a single data type to test
// that concern.

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTION 'g'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'g'"
                            "\n===========\n");

        if (verbose) printf("\nThis test has been disabled.\n");
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
        // COPY-ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Copy-Assignment Operator"
                            "\n================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

        TestDriver<TestKeyType, TestValueType>::testCase9();

        // 'propagate_on_container_copy_assignment' testing

        RUN_EACH_TYPE(TestDriver,
                      testCase9_propagate_on_container_copy_assignment,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

        TestDriver<TestKeyType, TestValueType>::
                            testCase9_propagate_on_container_copy_assignment();
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // MANIPULATOR AND FREE FUNCTION 'swap'
        // --------------------------------------------------------------------

        if (verbose) printf("\nMANIPULATOR AND FREE FUNCTION 'swap'"
                            "\n====================================\n");

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

        // Because the 'KEY' type in the pair is 'const', the move c'tor for
        // 'bsl::map' calls the copy c'tor of 'KEY'.  So we can't swap a
        // container with a move-only 'KEY'.

        MetaTestDriver<int,
                    BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType)>::testCase8();
        MetaTestDriver<BAD_MOVE_GUARD(bsltf::MovableAllocTestType),
                    BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType)>::testCase8();
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTOR"
                            "\n================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

        TestDriver<TestKeyType, TestValueType>::testCase7();

        // 'select_on_container_copy_construction' testing

        if (verbose) printf("\nCOPY CONSTRUCTOR: ALLOCATOR PROPAGATION"
                            "\n=======================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase7_select_on_container_copy_construction,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

        TestDriver<TestKeyType, TestValueType>::
                             testCase7_select_on_container_copy_construction();
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Equality Operators"
                            "\n==========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType),
                      BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType));

        TestDriver<TestKeyType, TestValueType>::testCase6();
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
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

        RUN_EACH_TYPE(TestDriver,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType),
                      BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType));

        TestDriver<TestKeyType, TestValueType>::testCase4();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTIONS 'gg' and 'ggg'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'gg'"
                            "\n============\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType),
                      BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType));

        TestDriver<TestKeyType, TestValueType>::testCase3();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Primary Manipulators"
                            "\n============================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType),
                      BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType));

        TestDriver<TestKeyType, TestValueType>::testCase2();
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
        //: 1 Run each method with arbitrary inputs and verify the behavior is
        //:   as expected.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");
        {
            int INT_VALUES[]   = { INT_MIN, -2, -1, 0, 1, 2, INT_MAX };
            enum { NUM_INT_VALUES = sizeof(INT_VALUES) / sizeof(*INT_VALUES) };

            typedef bool (*Comparator)(int, int);
            TestDriver<int, int, Comparator>::testCase1(&intLessThan,
                                                        INT_VALUES,
                                                        INT_VALUES,
                                                        NUM_INT_VALUES);

            TestDriver<int, int, std::less<int> >::testCase1(std::less<int>(),
                                                             INT_VALUES,
                                                             INT_VALUES,
                                                             NUM_INT_VALUES);
        }

        {
            // verify integrity of 'DEFAULT_DATA'

            const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            for (size_t ti = 0; ti < NUM_DATA; ++ti) {
                for (size_t tj = 0; tj < NUM_DATA; ++tj) {
                    ASSERT((DATA[ti].d_index == DATA[tj].d_index) ==
                          !strcmp(DATA[ti].d_results_p, DATA[tj].d_results_p));
                }
            }
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        if (verbose) printf("\nAdditional tests: initializer lists.\n");
        {
            ASSERT((0 == []() -> bsl::map<char, int> { return {}; }().size()));
            ASSERT((1 == []() -> bsl::map<char, int> {
                return {{'a', 1}};
            }().size()));
            ASSERT((2 == []() -> bsl::map<char, int> {
                return {{'a', 1}, {'b', 2}, {'a', 3}};
            }().size()));
        }
#endif
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
