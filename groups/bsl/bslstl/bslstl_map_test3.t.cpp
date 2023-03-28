// bslstl_map_test3.t.cpp                                             -*-C++-*-
#include <bslstl_map_test3.h>

#include <bslstl_iterator.h>
#include <bslstl_map.h>
#include <bslstl_pair.h>
#include <bslstl_randomaccessiterator.h>
#include <bslstl_string.h>

#include <bslalg_rangecompare.h>
#include <bslalg_scalarprimitives.h>

#include <bslma_allocator.h>
#include <bslma_allocatortraits.h>
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
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsls_util.h>

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
#include <bsltf_wellbehavedmoveonlyalloctesttype.h>

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

using namespace BloombergLP;
using bsl::pair;
using bsl::map;
using bsls::NameOf;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
// NOTICE: To reduce the compilation time (as well as enable the tests to build
// with certain compilers), this test driver has been broken into 4 parts:
//:
//: * 'bslstl_map.t.cpp' (cases 1-8, usage example, future test items (TBD))
//: * 'bslstl_map_test1.cpp' (cases 9-27)
//: * 'bslstl_map_test2.cpp' (case 28)
//: * 'bslstl_map_test3.cpp' (cases 29 and higher).
//
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
// [43] pair<iterator, bool> try_emplace(const key&, Args&&...);
// [43] iterator try_emplace(const_iterator, const key&, Args&&...);
// [43] pair<iterator, bool> try_emplace(key&&, Args&&...);
// [43] iterator try_emplace(const_iterator, key&&, Args&&...);
// [43] pair<iterator, bool> insert_or_assign(const key&, OTHER&&);
// [43] iterator insert_or_assign(const_iterator, const key&, OTHER&&);
// [43] pair<iterator, bool> insert_or_assign(key&&, OTHER&&);
// [43] iterator insert_or_assign(const_iterator, key&&, OTHER&&);
//
// [31] iterator emplace(Args&&... args);
// [32] iterator emplace_hint(const_iterator position, Args&&... args);
//
// [18] iterator erase(const_iterator position);
// [18] iterator erase(iterator position);
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
// [44] USAGE EXAMPLE
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
// [  ] CONCERN: 'map' object size is commensurate with that of 'C' and 'A'.
// [36] CONCERN: Methods qualifed 'noexcept' in standard are so implemented.
// [37] CONCERN: 'bslmf::MovableRef<T>' does not escape (in C++03 mode).
// [38] CONCERN: 'erase' overload is deduced correctly.
// [39] CONCERN: 'find'        properly handles transparent comparators.
// [39] CONCERN: 'count'       properly handles transparent comparators.
// [39] CONCERN: 'lower_bound' properly handles transparent comparators.
// [39] CONCERN: 'upper_bound' properly handles transparent comparators.
// [39] CONCERN: 'equal_range' properly handles transparent comparators.
// [40] CONCERN: 'count'       properly handles multi-value comparators.
// [40] CONCERN: 'find'        properly handles multi-value comparators.
// [40] CONCERN: 'lower_bound' properly handles multi-value comparators.
// [40] CONCERN: 'upper_bound' properly handles multi-value comparators.
// [40] CONCERN: 'equal_range' properly handles multi-value comparators.
// [41] CLASS TEMPLATE DEDUCTION GUIDES

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
//                      TEST CONFIGURATION MACROS
// ----------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND) \
 && (defined(BSLS_PLATFORM_CMP_IBM)   \
  || defined(BSLS_PLATFORM_CMP_CLANG) \
  || defined(BSLS_PLATFORM_CMP_MSVC)  \
  ||(defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION >= 0x5130) \
     )
# define BSL_DO_NOT_TEST_MOVE_FORWARDING 1
// Some compilers produce ambiguities when trying to construct our test types
// for 'emplace'-type functionality with the C++03 move-emulation.  This is a
// compiler bug triggering in lower level components, so we simply disable
// those aspects of testing, and rely on the extensive test coverage on other
// platforms.
#endif

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

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
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
#endif

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
            return bsltf::TemplateTestFacility::getIdentifier(lhs)
                 < bsltf::TemplateTestFacility::getIdentifier(rhs);   // RETURN
        }
        else {
            return bsltf::TemplateTestFacility::getIdentifier(lhs)
                 > bsltf::TemplateTestFacility::getIdentifier(rhs);   // RETURN
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
                    // struct TransparentComparator
                    // ============================

struct TransparentComparator
    // This class can be used as a comparator for containers.  It has a nested
    // type 'is_transparent', so it is classified as transparent by the
    // 'bslmf::IsTransparentPredicate' metafunction and can be used for
    // heterogeneous comparison.
 {
    typedef void is_transparent;

    template <class LHS, class RHS>
    bool operator()(const LHS& lhs, const RHS& rhs) const
        // Return 'true' if the specified 'lhs' is less than the specified
        // 'rhs' and 'false' otherwise.
    {
        return lhs < rhs;
    }
};

                    // =============================
                    // class TransparentlyComparable
                    // =============================

class TransparentlyComparable {
    // DATA
    int d_conversionCount;  // number of times 'operator int' has been called
    int d_value;            // the value

  private:
    // NOT IMPLEMENTED
    TransparentlyComparable(const TransparentlyComparable&);  // = delete

  public:
    // CREATORS
    explicit TransparentlyComparable(int value)
        // Create an object having the specified 'value'.

    : d_conversionCount(0)
    , d_value(value)
    {
    }

    // MANIPULATORS
    operator int()
        // Return the current value of this object.
    {
        ++d_conversionCount;
        return d_value;
    }

    // ACCESSORS
    int conversionCount() const
        // Return the number of times 'operator int' has been called.
    {
        return d_conversionCount;
    }

    int value() const
        // Return the current value of this object.
    {
        return d_value;
    }

    friend bool operator<(const TransparentlyComparable& lhs, int rhs)
        // Return 'true' if the value of the specified 'lhs' is less than the
        // specified 'rhs', and 'false' otherwise.
    {
        return lhs.d_value < rhs;
    }

    friend bool operator<(int lhs, const TransparentlyComparable& rhs)
        // Return 'true' if the specified 'lhs' is less than the value of the
        // specified 'rhs', and 'false' otherwise.
    {
        return lhs < rhs.d_value;
    }
};

template <class Container>
void testTransparentComparator(Container& container,
                               bool       isTransparent,
                               int        initKeyValue)
    // Search for a value equal to the specified 'initKeyValue' in the
    // specified 'container', and count the number of conversions expected
    // based on the specified 'isTransparent'.  Note that 'Container' may
    // resolve to a 'const'-qualified type, we are using the "reference" here
    // as a sort of universal reference.  Conceptually, the object remains
    // constant, but we want to test 'const'-qualified and
    // non-'const'-qualified overloads.
{
    typedef typename Container::const_iterator Iterator;
    typedef typename Container::size_type      Count;

    int expectedConversionCount = 0;

    TransparentlyComparable existingKey(initKeyValue);
    TransparentlyComparable nonExistingKey(initKeyValue ? -initKeyValue
                                                        : -100);

    ASSERT(existingKey.conversionCount() == expectedConversionCount);

    // Testing 'find'.

    const Iterator EXISTING_F = container.find(existingKey);
    if (!isTransparent) {
        ++expectedConversionCount;
    }

    ASSERT(container.end()               != EXISTING_F);
    ASSERT(existingKey.value()           == EXISTING_F->first);
    ASSERT(existingKey.conversionCount() == expectedConversionCount);

    const Iterator NON_EXISTING_F = container.find(nonExistingKey);
    ASSERT(container.end()                  == NON_EXISTING_F);
    ASSERT(nonExistingKey.conversionCount() == expectedConversionCount);

    // Testing 'contains'.

    const bool EXISTING_CONTAINS = container.contains(existingKey);
    if (!isTransparent) {
        ++expectedConversionCount;
    }

    ASSERT(true == EXISTING_CONTAINS);
    ASSERT(existingKey.conversionCount() == expectedConversionCount);

    const bool NON_EXISTING_CONTAINS = container.contains(nonExistingKey);
    ASSERT(false == NON_EXISTING_CONTAINS);
    ASSERT(nonExistingKey.conversionCount() == expectedConversionCount);

    // Testing 'count'.

    const Count EXISTING_C = container.count(existingKey);
    if (!isTransparent) {
        ++expectedConversionCount;
    }

    ASSERT(1                       == EXISTING_C);
    ASSERT(expectedConversionCount == existingKey.conversionCount());

    const Count NON_EXISTING_C = container.count(nonExistingKey);
    ASSERT(0                       == NON_EXISTING_C);
    ASSERT(expectedConversionCount == nonExistingKey.conversionCount());

    // Testing 'lower_bound'.

    const Iterator EXISTING_LB = container.lower_bound(existingKey);
    if (!isTransparent) {
        ++expectedConversionCount;
    }

    ASSERT(EXISTING_F              == EXISTING_LB);
    ASSERT(expectedConversionCount == existingKey.conversionCount());

    const Iterator NON_EXISTING_LB = container.lower_bound(nonExistingKey);

    ASSERT(container.begin()       == NON_EXISTING_LB);
    ASSERT(expectedConversionCount == nonExistingKey.conversionCount());

    // Testing 'upper_bound'.

    TransparentlyComparable upperBoundValue(initKeyValue + 1);
    const Iterator          EXPECTED_UB = container.find(upperBoundValue);
    const Iterator          EXISTING_UB = container.upper_bound(existingKey);
    if (!isTransparent) {
        ++expectedConversionCount;
    }

    ASSERT(EXPECTED_UB             == EXISTING_UB);
    ASSERT(expectedConversionCount == existingKey.conversionCount());

    const Iterator NON_EXISTING_UB = container.upper_bound(nonExistingKey);

    ASSERT(container.begin()       == NON_EXISTING_UB);
    ASSERT(expectedConversionCount == nonExistingKey.conversionCount());

    // Testing 'equal_range'.

    const bsl::pair<Iterator, Iterator> EXISTING_ER =
                                            container.equal_range(existingKey);
    if (!isTransparent) {
        ++expectedConversionCount;
    }

    ASSERT(EXISTING_LB             == EXISTING_ER.first);
    ASSERT(EXPECTED_UB             == EXISTING_ER.second);
    ASSERT(expectedConversionCount == existingKey.conversionCount());

    const bsl::pair<Iterator, Iterator> NON_EXISTING_ER =
                                         container.equal_range(nonExistingKey);

    ASSERT(NON_EXISTING_LB         == NON_EXISTING_ER.first);
    ASSERT(NON_EXISTING_UB         == NON_EXISTING_ER.second);
    ASSERT(expectedConversionCount == nonExistingKey.conversionCount());
}

              // ====================================================
              // struct TransparentComparatorWithMultiValueEqualRange
              // ====================================================

struct TransparentComparatorWithMultiValueEqualRange {
    // This class can be used as a comparator for containers, having
    // 'bsl::string' type as a 'KEY'.  It is classified as transparent by the
    // 'bslmf::IsTransparentPredicate' metafunction and can be used for
    // heterogeneous comparison.  But note that additional operators accepting
    // 'char' objects behave differently than the main one accepting
    // 'bsl::string' objects only.  That leads to puzzling return values of
    // such container methods as 'count' and 'equal_range' accepting 'char'
    // objects.  'count' can return a value greater than one and 'equal_range'
    // can return a range containing more than one element, while container is
    // still has only unique values.

    typedef void is_transparent;

    bool operator()(const bsl::string& lhs, const bsl::string& rhs) const
        // Return 'true' if the specified 'lhs' is less than the specified
        // 'rhs', and 'false' otherwise.
    {
        return lhs < rhs;
    }

    bool operator()(const bsl::string& s, char c) const
        // Return 'true' if the first symbol of the specified 's' is less than
        // the specified 'c', and 'false' otherwise.
    {
        return s.length() == 0 || s[0] < c;
    }

    bool operator()(char c, const bsl::string& s) const
        // Return 'true' if the specified 'c' is less than the first symbol of
        // the specified 's', and 'false' otherwise.
    {
        return s.length() != 0 && c < s[0];
    }
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
    AmbiguousTestType03(const TYPE& other)                          // IMPLICIT
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
                   && !bsl::is_same<VALUE, bsltf::MoveOnlyAllocTestType>::value
                   && !bsl::is_same<VALUE,
                               bsltf::WellBehavedMoveOnlyAllocTestType>::value;


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

}  // close unnamed namespace

// ============================================================================
//                      GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

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

                       // =============================
                       // struct EraseAmbiguityTestType
                       // =============================

struct EraseAmbiguityTestType
    // This test type has a template constructor that can accept iterator.
{
    // CREATORS
    template <class T>
    explicit
    EraseAmbiguityTestType(T&)
        // Construct an object.
    {}
};

bool operator<(const EraseAmbiguityTestType& lhs,
               const EraseAmbiguityTestType& rhs)
    // This operator is no-op and written only to satisfy requirements for
    // 'key_type' class.
{
    (void) lhs;    (void) rhs;

    return false;
}

void runErasure(bsl::map<EraseAmbiguityTestType, int>& container,
                EraseAmbiguityTestType                 element)
    // Look for the specified 'element' in the specified 'container' and delete
    // it if found.  Code is written in such a way as to reveal the ambiguity
    // of the 'erase' method call.
{
    bsl::map<EraseAmbiguityTestType, int>::iterator it =
                                                       container.find(element);
    if (it != container.end()) {
        container.erase(it);
    }
}

}  // close unnamed namespace

                       // ================
                       // struct EqKeyPred
                       // ================

template <class KEY, class VALUE>
struct EqKeyPred {
    // A predicate for testing 'erase_if'; it takes a value at construction
    // and uses it for comparisons later.

    KEY d_key;
    EqKeyPred(KEY val) : d_key(val) {}

    bool operator() (const bsl::pair<const KEY, VALUE> &p) const
        // return 'true' if the second member of the specified pair 'p' is
        // equal to the stored value, and 'false' otherwise.
    {
        return d_key == p.first;
    }
 };

                         // ================
                         // class IntValue
                         // ================

struct IntValue {
    // A struct that holds an integer value, but has multiple constructors
    // that add the different arguments.

    IntValue ()                    : d_value(0) {}
        // Construct an IntValue, setting d_value to zero.

    IntValue (int a)               : d_value(a) {}
        // Construct an IntValue from the specified 'a'

    IntValue (int a, int b)        : d_value(a + b) {}
        // Construct an IntValue from the specified 'a' and 'b'.

    IntValue (int a, int b, int c) : d_value(a + b + c) {}
        // Construct an IntValue from the specified 'a', 'b', and 'c'.

    int d_value;
};

                         // ================
                         // class MoveHolder
                         // ================

template <class TYPE>
struct MoveHolder {
    // A simple class that holds a value, but records if the value has ever
    // been moved from.

    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;
        // This typedef is a convenient alias for the utility associated with
        // movable references.

    MoveHolder()
    : d_value()
    , d_moved(false)
        // Default initialize the held value.
    {}

    explicit MoveHolder(const TYPE &value)
    : d_value(value)
    , d_moved(false)
        // Construct from the specified 'value'
    {}

    MoveHolder(const MoveHolder& rhs)
    : d_value(rhs.d_value)
    , d_moved(false)
        // Copy-construct from the specified 'rhs'
    {}

    MoveHolder(BloombergLP::bslmf::MovableRef<MoveHolder> rhs)
    : d_value(MoveUtil::move(MoveUtil::access(rhs).d_value))
    , d_moved(false)
        // Move-construct from the specified 'rhs'. Set the 'rhs' moved-from
        // flag to 'true'.
    {
        MoveHolder& lvalue = rhs;
        lvalue.d_moved = true;
    }

    MoveHolder& operator=(const MoveHolder& rhs)
        // Copy-assign from the specified 'rhs'.
    {
        d_value = rhs.d_value;
        d_moved = rhs.d_moved;
        return *this;
    }

    MoveHolder& operator=(BloombergLP::bslmf::MovableRef<MoveHolder> rhs)
        // Move-assign from the specified 'rhs'. Set the 'rhs' moved-from flag
        // to 'true'.
    {
        MoveHolder& lvalue = rhs;

        d_value = MoveUtil::move(MoveUtil::access(rhs).d_value);
        d_moved = lvalue.d_moved;
        lvalue.d_moved = true;
        return *this;
    }

    bool hasBeenMoved() const
        // Return 'true' if this object has been moved from, and 'false'
        // otherwise.
    {
        return d_moved;
    }

    TYPE d_value;
    bool d_moved;
};

template <class TYPE>
bool operator==(const MoveHolder<TYPE>& lhs, const MoveHolder<TYPE>& rhs)
    // Return 'true' if both the held value and the moved-from flag of the
    // specified 'lhs' are equal to the corresponding fields in the specified
    // 'rhs', and 'false' otherwise.
{
    return lhs.hasBeenMoved() == rhs.hasBeenMoved()
                                                 && lhs.d_value == rhs.d_value;
}

template <class TYPE>
bool operator<(const MoveHolder<TYPE>& lhs, const MoveHolder<TYPE>& rhs)
    // If the specified 'lhs' has been moved from and the specified 'rhs' has
    // not, return 'true'.  If 'rhs' has been moved from and 'lhs' has not,
    // return 'false'.  Otherwise, return 'lhs.d_value < rhs.d_value'.
{
    return lhs.hasBeenMoved() == rhs.hasBeenMoved()
        ? lhs.d_value < rhs.d_value
        : lhs.hasBeenMoved();
}


template <class TYPE>
bool operator==(const MoveHolder<TYPE>& lhs, const TYPE& rhs)
    // Return 'true' if the held value in the specified 'lhs' is equal to the
    // specified 'rhs', and 'false' otherwise.  If 'lhs' has been moved from,
    // then return 'false'.
{
    return lhs.hasBeenMoved() ? false : lhs.d_value == rhs;
}

template <class TYPE>
bool operator==(const TYPE& lhs, const MoveHolder<TYPE>& rhs)
    // Return 'true' if the held value in the specified 'rhs' is equal to the
    // specified 'lhs', and 'false' otherwise.  If 'rhs' has been moved from,
    // then return 'false'.
{
    return rhs.hasBeenMoved() ? false : lhs == rhs.d_value;
}

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

    typedef bsl::allocator_traits<ALLOC>          AllocatorTraits;

    enum AllocCategory { e_BSLMA, e_ADAPTOR, e_STATEFUL };

    enum { k_IS_KEY_WELL_BEHAVED = bsl::is_same<KEY,
                               bsltf::WellBehavedMoveOnlyAllocTestType>::value,
           k_IS_VALUE_WELL_BEHAVED = bsl::is_same<VALUE,
                               bsltf::WellBehavedMoveOnlyAllocTestType>::value,
           k_IS_KEY_MOVE_AWARE =
                    bsl::is_same<KEY, bsltf::MovableTestType>::value ||
                    bsl::is_same<KEY, bsltf::MovableAllocTestType>::value ||
                    bsl::is_same<KEY, bsltf::MoveOnlyAllocTestType>::value ||
                                                         k_IS_KEY_WELL_BEHAVED,
           k_IS_VALUE_MOVE_AWARE =
                    bsl::is_same<VALUE, bsltf::MovableTestType>::value ||
                    bsl::is_same<VALUE, bsltf::MovableAllocTestType>::value ||
                    bsl::is_same<VALUE, bsltf::MoveOnlyAllocTestType>::value ||
                                                     k_IS_VALUE_WELL_BEHAVED };

#if defined(BSLS_PLATFORM_OS_AIX) || defined(BSLS_PLATFORM_OS_WINDOWS)
    // Aix has a compiler bug where method pointers do not default construct to
    // 0.  Windows has the same problem.

    enum { k_IS_VALUE_DEFAULT_CONSTRUCTIBLE =
                !bsl::is_same<VALUE,
                              bsltf::TemplateTestFacility::MethodPtr>::value };
#else
    enum { k_IS_VALUE_DEFAULT_CONSTRUCTIBLE = true };
#endif

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

    // CLASS METHODS
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

    static void testCase43();
        // Test 'try_emplace' and 'insert_or_assign'

    static void testCase42();
        // Test free function 'bsl::erase_if'.

    static void testCase38();
        // Test absence of 'erase' method ambiguity.

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
                   || bsl::is_same<VALUE, bsltf::MoveOnlyAllocTestType>::value
                   || bsl::is_same<VALUE,
                               bsltf::WellBehavedMoveOnlyAllocTestType>::value)
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
                result = mX.emplace(std::piecewise_construct,
                                    std::forward_as_tuple(),
                                    std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace(
                                 std::piecewise_construct,
                                 std::forward_as_tuple(),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace(
                                 std::piecewise_construct,
                                 std::forward_as_tuple(),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace(
                                 std::piecewise_construct,
                                 std::forward_as_tuple(),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1),
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
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1)),
                                 std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace(
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1)),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace(
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1)),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace(
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1)),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1),
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
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2)),
                                 std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace(
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2)),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace(
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2)),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace(
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2)),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1),
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
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2),
                                                       testArg(AK3, MOVE_K3)),
                                 std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace(
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2),
                                                       testArg(AK3, MOVE_K3)),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace(
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2),
                                                       testArg(AK3, MOVE_K3)),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace(
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2),
                                                       testArg(AK3, MOVE_K3)),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1),
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
                                         std::piecewise_construct,
                                         std::forward_as_tuple(),
                                         std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace_hint(
                                 hint,
                                 std::piecewise_construct,
                                 std::forward_as_tuple(),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace_hint(
                                 hint,
                                 std::piecewise_construct,
                                 std::forward_as_tuple(),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace_hint(
                                 hint,
                                 std::piecewise_construct,
                                 std::forward_as_tuple(),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1),
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
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1)),
                                 std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace_hint(
                                 hint,
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1)),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace_hint(
                                 hint,
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1)),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace_hint(
                                 hint,
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1)),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1),
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
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2)),
                                 std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace_hint(
                                 hint,
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2)),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace_hint(
                                 hint,
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2)),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace_hint(
                                 hint,
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2)),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1),
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
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2),
                                                       testArg(AK3, MOVE_K3)),
                                 std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace_hint(
                                 hint,
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2),
                                                       testArg(AK3, MOVE_K3)),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace_hint(
                                 hint,
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2),
                                                       testArg(AK3, MOVE_K3)),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1),
                                                       testArg(AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace_hint(
                                 hint,
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(AK1, MOVE_K1),
                                                       testArg(AK2, MOVE_K2),
                                                       testArg(AK3, MOVE_K3)),
                                 std::forward_as_tuple(testArg(AV1, MOVE_V1),
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
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase43()
{
    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

    static const struct {
        int         d_line;      // source line number
        const char *d_key_p;     // list of keys string
        const char *d_value_p;   // list of values string
        const char *d_results_p; // expected element values
    } DATA[] = {
        //line  key            value          results
        //----  ---            -----          -------

        { L_,   "A",           "0",           "Y"           },
        { L_,   "AAA",         "012",         "YNN"         },
        { L_,   "ABCDEFGH",    "01234567",    "YYYYYYYY"    },
        { L_,   "ABCDEABCDEF", "0123456789A", "YYYYYNNNNNY" },
        { L_,   "EEDDCCBBAA",  "0123456789",  "YNYNYNYNYN"  }
    };
    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

    typedef bsl::map<MoveHolder<KEY>, VALUE> MH_map;

    // Sanity checks
    for (size_t i = 0; i < NUM_DATA; ++i) {
        ASSERT(strlen(DATA[i].d_key_p) == strlen(DATA[i].d_value_p));
        ASSERT(strlen(DATA[i].d_key_p) == strlen(DATA[i].d_results_p));
    }

    // try_emplace (const KEY &)
    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        typedef bsl::pair<typename Obj::iterator, bool> PAIR;
        int             LINE = DATA[i].d_line;
        const char     *keys = DATA[i].d_key_p;
        const size_t    keysLen = strlen(keys);
        const char     *values = DATA[i].d_value_p;
        const char     *results = DATA[i].d_results_p;

        Obj    mX;
        size_t map_size = 0;

        for (size_t j = 0; j < keysLen; ++j)
        {
            ASSERTV(LINE, j, map_size == mX.size());
            PAIR res = mX.try_emplace(keys[j], values[j]);
            ASSERTV(LINE, j, ('Y' == results[j]) == res.second);
            ASSERTV(LINE, j, KEY(keys[j]) == res.first->first);
            if (res.second) {
                ASSERTV(LINE, j, VALUE(values[j]) == res.first->second);
                ASSERTV(LINE, j, ++map_size == mX.size());
            }
        }
    }

    // try_emplace (KEY&&)
    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        typedef bsl::pair<typename MH_map::iterator, bool> PAIR;
        int             LINE = DATA[i].d_line;
        const char     *keys = DATA[i].d_key_p;
        const size_t    keysLen = strlen(keys);
        const char     *values = DATA[i].d_value_p;
        const char     *results = DATA[i].d_results_p;

        MH_map mX;
        size_t map_size = 0;

        for (size_t j = 0; j < keysLen; ++j)
        {
            MoveHolder<KEY> k(static_cast<KEY>(keys[j]));

            ASSERTV(LINE, j, map_size == mX.size());
            PAIR res = mX.try_emplace(MoveUtil::move(k), values[j]);
            ASSERTV(LINE, j, ('Y' == results[j]) == res.second);
            ASSERTV(LINE, j, ('Y' == results[j]) == k.hasBeenMoved());
            ASSERTV(LINE, j, KEY(keys[j]) == res.first->first);
            if (res.second) {
                ASSERTV(LINE, j, VALUE(values[j]) == res.first->second);
                ASSERTV(LINE, j, ++map_size == mX.size());
            }
        }
    }

    // try_emplace (hint, const KEY &)
    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        int             LINE = DATA[i].d_line;
        const char     *keys = DATA[i].d_key_p;
        const size_t    keysLen = strlen(keys);
        const char     *values = DATA[i].d_value_p;
        const char     *results = DATA[i].d_results_p;

        Obj    mX;
        size_t map_size = 0;

        for (size_t j = 0; j < keysLen; ++j)
        {
            ASSERTV(LINE, j, map_size == mX.size());
            typename Obj::iterator it =
                                  mX.try_emplace(mX.end(), keys[j], values[j]);
            ASSERTV(LINE, j, KEY(keys[j]) == it->first);
            if ('Y' == results[j]) {
                ASSERTV(LINE, j, VALUE(values[j]) == it->second);
                ASSERTV(LINE, j, ++map_size == mX.size());
            }
        }
    }

    // try_emplace (hint, KEY&&)
    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        int             LINE = DATA[i].d_line;
        const char     *keys = DATA[i].d_key_p;
        const size_t    keysLen = strlen(keys);
        const char     *values = DATA[i].d_value_p;
        const char     *results = DATA[i].d_results_p;

        MH_map mX;
        size_t map_size = 0;

        for (size_t j = 0; j < keysLen; ++j)
        {
            MoveHolder<KEY> k(static_cast<KEY>(keys[j]));

            ASSERTV(LINE, j, map_size == mX.size());
            typename MH_map::iterator it =
                        mX.try_emplace(mX.end(), MoveUtil::move(k), values[j]);
            ASSERTV(LINE, j, KEY(keys[j]) == it->first);
            ASSERTV(LINE, j, ('Y' == results[j]) == k.hasBeenMoved());
            if ('Y' == results[j]) {
                ASSERTV(LINE, j, VALUE(values[j]) == it->second);
                ASSERTV(LINE, j, ++map_size == mX.size());
            }
        }
    }

    // insert_or_assign (const KEY &)
    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        typedef bsl::pair<typename Obj::iterator, bool> PAIR;
        int             LINE = DATA[i].d_line;
        const char     *keys = DATA[i].d_key_p;
        const size_t    keysLen = strlen(keys);
        const char     *values = DATA[i].d_value_p;
        const char     *results = DATA[i].d_results_p;

        Obj    mX;
        size_t map_size = 0;

        for (size_t j = 0; j < keysLen; ++j)
        {
            ASSERTV(LINE, j, map_size == mX.size());
            PAIR res = mX.insert_or_assign(keys[j], values[j]);
            ASSERTV(LINE, j, ('Y' == results[j]) == res.second);
            ASSERTV(LINE, j, KEY(keys[j])     == res.first->first);
            ASSERTV(LINE, j, VALUE(values[j]) == res.first->second);
            if (res.second) {
                ASSERTV(LINE, j, ++map_size == mX.size());
            }
        }
    }

    // insert_or_assign (KEY&&)
    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        typedef bsl::pair<typename MH_map::iterator, bool> PAIR;
        int             LINE = DATA[i].d_line;
        const char     *keys = DATA[i].d_key_p;
        const size_t    keysLen = strlen(keys);
        const char     *values = DATA[i].d_value_p;
        const char     *results = DATA[i].d_results_p;

        MH_map mX;
        size_t map_size = 0;

        for (size_t j = 0; j < keysLen; ++j)
        {
            MoveHolder<KEY> k(static_cast<KEY>(keys[j]));

            ASSERTV(LINE, j, map_size == mX.size());
            PAIR res = mX.insert_or_assign(MoveUtil::move(k), values[j]);
            ASSERTV(LINE, j, ('Y' == results[j]) == res.second);
            ASSERTV(LINE, j, ('Y' == results[j]) == k.hasBeenMoved());
            ASSERTV(LINE, j, KEY(keys[j])     == res.first->first);
            ASSERTV(LINE, j, VALUE(values[j]) == res.first->second);
            if (res.second) {
                ASSERTV(LINE, j, ++map_size == mX.size());
            }
        }
    }

    // insert_or_assign (hint, const KEY &)
    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        int             LINE = DATA[i].d_line;
        const char     *keys = DATA[i].d_key_p;
        const size_t    keysLen = strlen(keys);
        const char     *values = DATA[i].d_value_p;
        const char     *results = DATA[i].d_results_p;

        Obj    mX;
        size_t map_size = 0;

        for (size_t j = 0; j < keysLen; ++j)
        {
            ASSERTV(LINE, j, map_size == mX.size());
            typename Obj::iterator it =
                            mX.insert_or_assign(mX.end(), keys[j], values[j]);
            ASSERTV(LINE, j, KEY(keys[j])     == it->first);
            ASSERTV(LINE, j, VALUE(values[j]) == it->second);
            if ('Y' == results[j]) {
                ASSERTV(LINE, j, ++map_size == mX.size());
            }
        }
    }

    // insert_or_assign (hint, KEY&&)
    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        int             LINE = DATA[i].d_line;
        const char     *keys = DATA[i].d_key_p;
        const size_t    keysLen = strlen(keys);
        const char     *values = DATA[i].d_value_p;
        const char     *results = DATA[i].d_results_p;

        MH_map mX;
        size_t map_size = 0;

        for (size_t j = 0; j < keysLen; ++j)
        {
            MoveHolder<KEY> k(static_cast<KEY>(keys[j]));

            ASSERTV(LINE, j, map_size == mX.size());
            typename MH_map::iterator it =
                   mX.insert_or_assign(mX.end(), MoveUtil::move(k), values[j]);
            ASSERTV(LINE, j, KEY(keys[j])     == it->first);
            ASSERTV(LINE, j, VALUE(values[j]) == it->second);
            ASSERTV(LINE, j, ('Y' == results[j]) == k.hasBeenMoved());
            if ('Y' == results[j]) {
                ASSERTV(LINE, j, ++map_size == mX.size());
            }
        }
    }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase42()
{
    // --------------------------------------------------------------------
    // TESTING FREE FUNCTION 'BSL::ERASE_IF'
    //
    // Concerns:
    //: 1 The free function exists, and is callable with a map.
    //
    // Plan:
    //: 1 Fill a map with known values, then attempt to erase some of
    //:   the values using 'bsl::erase_if'.  Verify that the resultant map
    //:   is the right size, contains the correct values, and that the
    //:   value returned from the functions is correct.
    //
    // Testing:
    //   size_t erase_if(map&, PREDICATE);
    // --------------------------------------------------------------------

    static const struct {
        int         d_line;       // source line number
        const char *d_initial_p;  // initial values
        char        d_element;    // value to remove
        const char *d_results_p;  // expected result value
    } DATA[] = {
        //line  initial              element  results
        //----  -------------------  -------  -------------------
        { L_,   "",                  'A',     ""                  },
        { L_,   "A",                 'A',     ""                  },
        { L_,   "A",                 'B',     "A"                 },
        { L_,   "B",                 'A',     "B"                 },
        { L_,   "AB",                'A',     "B"                 },
        { L_,   "BA",                'A',     "B"                 },
        { L_,   "BC",                'D',     "BC"                },
        { L_,   "ABC",               'C',     "AB"                },
        { L_,   "CBADEGHIJKL",       'B',     "CADEGHIJKL"        }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        typedef bsl::pair<const KEY, VALUE> PAIR;
        int             LINE = DATA[i].d_line;
        const char     *initial = DATA[i].d_initial_p;
        size_t          initialLen = strlen(initial);
        const char     *results = DATA[i].d_results_p;
        size_t          resultsLen = strlen(results);

        Obj mX;
        Obj mRes;

        for (size_t j = 0; j < initialLen; ++j)
        {
            mX.insert(PAIR(initial[j], 0));
        }

        for (size_t j = 0; j < resultsLen; ++j)
        {
            mRes.insert(PAIR(results[j], 0));
        }

        EqKeyPred<KEY, VALUE> pred(DATA[i].d_element);
        size_t                ret   = bsl::erase_if(mX, pred);

        // Is the modified container correct?
        ASSERTV(LINE, mX == mRes);

        // Is the return value correct?
        ASSERTV(LINE, ret == initialLen - resultsLen);
        }
}

template <class KEY, class VALUE, class COMP, class ALLOC>
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase38()
{
    // ------------------------------------------------------------------------
    // TESTING ABSENCE OF ERASE AMBIGUITY
    //  'std::map::erase' takes an iterator in C++03, but a const_iterator in
    //  C++0x.  This breaks code where the map's 'key_type' has a constructor
    //  which accepts an iterator (for example a template constructor), as the
    //  compiler cannot choose between 'erase(const key_type&)' and
    //  'erase(const_iterator)'. As BDE library duplicates 'std' interfaces, it
    //  has the same problem. The solution is to restore the iterator overload
    //  in addition to the const_iterator overload.
    //  See https://cplusplus.github.io/LWG/lwg-defects.html#2059
    //
    // Concerns:
    //: 1 Adding the iterator overload takes away the ambiguity of 'erase'
    //:   method.
    //
    // Plan:
    //: 1 Using brute force and a specially tailored test type,
    //:   'EraseAmbiguityTestType', we verify that appropriate overload is
    //:   deduced successfully.  Note that this is a compile-only test; runtime
    //:   values are not checked.  (C-1)
    //
    // Testing:
    //   CONCERN: 'erase' overload is deduced correctly.
    // ------------------------------------------------------------------------

    VALUE                       value(5);
    KEY                         key(value);
    bsl::map<KEY, VALUE>        mX;
    const bsl::map<KEY, VALUE>& X = mX;

    mX.insert(bsl::pair<KEY, VALUE>(key, value));
    ASSERTV(X.size(), 1 == X.size());
    runErasure(mX, key);
    ASSERTV(X.size(), 0 == X.size());
}

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

        ASSERT(false
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX =
                                             bslmf::MovableRefUtil::move(mY)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.get_allocator()));
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

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.begin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.begin()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.end()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.end()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.rbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.rbegin()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.rend()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.rend()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.cbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.cend()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.crbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.crend()));
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

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.empty()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.size()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.max_size()));
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

        ASSERT(false
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.swap(y)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.clear()));
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

        ASSERT(false
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.swap(y)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.clear()));
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

                    const MoveState::Enum expKeyMove = k_IS_KEY_MOVE_AWARE
                                                       ? MoveState::e_NOT_MOVED
                                                       : MoveState::e_UNKNOWN;
                    const MoveState::Enum expValueMove =
                                       k_IS_VALUE_MOVE_AWARE
                                       ? (k_IS_VALUE_WELL_BEHAVED && &oa != &sa
                                                       ? MoveState::e_NOT_MOVED
                                                       : MoveState::e_MOVED)
                                                    : MoveState::e_UNKNOWN;

                    ASSERTV(mStateKey, expKeyMove, NameOf<KEY>(),
                                                      expKeyMove == mStateKey);
                    ASSERTV(mStateValue, expValueMove, NameOf<VALUE>(),
                                                  expValueMove == mStateValue);

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
                    ASSERTV(mStateKey, expKeyMove, NameOf<KEY>(),
                                                      expKeyMove == mStateKey);
                    ASSERTV(mStateValue, expValueMove, NameOf<VALUE>(),
                                                  expValueMove == mStateValue);

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
        const int NUM_SPECS = static_cast<int>(sizeof DATA / sizeof *DATA);

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
        const int NUM_SPECS = static_cast<int>(sizeof DATA / sizeof *DATA);

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
        const int NUM_SPECS = static_cast<int>(sizeof DATA / sizeof *DATA);

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
        const int NUM_SPECS = static_cast<int>(sizeof DATA / sizeof *DATA);

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
        const int NUM_SPECS = static_cast<int>(sizeof DATA / sizeof *DATA);

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
        const int NUM_SPECS = static_cast<int>(sizeof DATA / sizeof *DATA);

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
    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

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
    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

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
    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

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

                        const MoveState::Enum expKeyMove =
                                         k_IS_KEY_MOVE_AWARE
                                         ? (k_IS_KEY_WELL_BEHAVED && &oa != &sa
                                                      ? MoveState::e_NOT_MOVED
                                                      : MoveState::e_MOVED)
                                                   : MoveState::e_UNKNOWN;
                        const MoveState::Enum expValueMove =
                                       k_IS_VALUE_MOVE_AWARE
                                       ? (k_IS_VALUE_WELL_BEHAVED && &oa != &sa
                                                       ? MoveState::e_NOT_MOVED
                                                       : MoveState::e_MOVED)
                                                    : MoveState::e_UNKNOWN;

                        ASSERTV(mStateFirst, expKeyMove, NameOf<KEY>(),
                                           IS_UNIQ, expKeyMove == mStateFirst);
                        ASSERTV(mStateSecond, expValueMove, NameOf<VALUE>(),
                                        IS_UNIQ, expValueMove == mStateSecond);

                        if (IS_UNIQ) {
                            bsls::Types::Int64 expAA = BB + TYPE_ALLOC;
                                                                    // new item
                            bsls::Types::Int64 expA  = B  + TYPE_ALLOC;
                                                                    // new item

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
    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

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

                    const MoveState::Enum expKeyMove =
                                         k_IS_KEY_MOVE_AWARE
                                         ? (k_IS_KEY_WELL_BEHAVED && &oa != &sa
                                                      ? MoveState::e_NOT_MOVED
                                                      : MoveState::e_MOVED)
                                                   : MoveState::e_UNKNOWN;
                    const MoveState::Enum expValueMove =
                                       k_IS_VALUE_MOVE_AWARE
                                       ? (k_IS_VALUE_WELL_BEHAVED && &oa != &sa
                                                       ? MoveState::e_NOT_MOVED
                                                       : MoveState::e_MOVED)
                                                    : MoveState::e_UNKNOWN;

                    ASSERTV(mStateFirst, expKeyMove, NameOf<KEY>(), IS_UNIQ,
                                                    expKeyMove == mStateFirst);
                    ASSERTV(mStateSecond, expValueMove, NameOf<VALUE>(),
                                        IS_UNIQ, expValueMove == mStateSecond);

                    if (IS_UNIQ) {
                        bsls::Types::Int64 expAA = BB + TYPE_ALLOC; // new item
                        bsls::Types::Int64 expA  = B  + TYPE_ALLOC; // new item

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

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
struct TestDeductionGuides {
    // This struct provides a namespace for functions testing deduction guides.
    // The tests are compile-time only; it is not necessary that these routines
    // be called at run-time.  Note that the following constructors do not have
    // associated deduction guides because the template parameters for
    // 'bsl::map' cannot be deduced from the constructor parameters.
    //..
    // map()
    // explicit map(COMPARATOR, ALLOCATOR = ALLOCATOR());
    // map(ALLOCATOR)
    //..

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

    static void SimpleConstructors ()
        // Test that constructing a 'bsl::map' from various combinations of
        // arguments deduces the correct type.
        //..
        // map(const map&  m)            -> decltype(m)
        // map(const map&  m, ALLOCATOR) -> decltype(m)
        // map(      map&& m)            -> decltype(m)
        // map(      map&& m, ALLOCATOR) -> decltype(m)
        //
        // map(Iter, Iter, COMPARATOR = COMPARATOR(), ALLOCATOR = ALLOCATOR())
        // map(Iter, Iter, ALLOCATOR)
        //
        // map(initializer_list, COMPARATOR = COMPARATOR(),
        //                                            ALLOCATOR = ALLOCATOR())
        // map(initializer_list, ALLOCATOR)
        //..
    {
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;

        typedef int T1;
        bsl::map<T1, T1> m1;
        bsl::map         m1a(m1);
        ASSERT_SAME_TYPE(decltype(m1a), bsl::map<T1, T1>);

        typedef float T2;
        typedef bsl::allocator<bsl::pair<const T2, T2>> BA2;
        bsl::map<T2, T2> m2;
        bsl::map         m2a(m2, BA2{});
        bsl::map         m2b(m2, a1);
        bsl::map         m2c(m2, a2);
        bsl::map         m2d(m2, bsl::allocator<int>{});
        ASSERT_SAME_TYPE(decltype(m2a), bsl::map<T2, T2>);
        ASSERT_SAME_TYPE(decltype(m2b), bsl::map<T2, T2>);
        ASSERT_SAME_TYPE(decltype(m2c), bsl::map<T2, T2>);
        ASSERT_SAME_TYPE(decltype(m2d), bsl::map<T2, T2>);

        typedef short T3;
        bsl::map<T3, T3> m3;
        bsl::map         m3a(std::move(m3));
        ASSERT_SAME_TYPE(decltype(m3a), bsl::map<T3, T3>);

        typedef long double T4;
        typedef bsl::allocator<bsl::pair<const T4, T4>> BA4;
        bsl::map<T4, T4> m4;
        bsl::map         m4a(std::move(m4), BA4{});
        bsl::map         m4b(std::move(m4), a1);
        bsl::map         m4c(std::move(m4), a2);
        bsl::map         m4d(std::move(m4), bsl::allocator<int>{});
        ASSERT_SAME_TYPE(decltype(m4a), bsl::map<T4, T4>);
        ASSERT_SAME_TYPE(decltype(m4b), bsl::map<T4, T4>);
        ASSERT_SAME_TYPE(decltype(m4c), bsl::map<T4, T4>);
        ASSERT_SAME_TYPE(decltype(m4d), bsl::map<T4, T4>);

        typedef long T5;
        typedef std::greater<T5> CompT5;
        typedef bsl::allocator<bsl::pair<const T5, T5>> BA5;
        typedef std::allocator<bsl::pair<const T5, T5>> SA5;
        bsl::pair<T5, T5>          *p5b = nullptr;
        bsl::pair<T5, T5>          *p5e = nullptr;
        bsl::map<T5, T5>::iterator  i5b;
        bsl::map<T5, T5>::iterator  i5e;

        bsl::map m5a(p5b, p5e);
        bsl::map m5b(i5b, i5e);
        bsl::map m5c(p5b, p5e, CompT5{});
        bsl::map m5d(i5b, i5e, CompT5{});
        bsl::map m5e(p5b, p5e, CompT5{}, BA5{});
        bsl::map m5f(p5b, p5e, CompT5{}, a1);
        bsl::map m5g(p5b, p5e, CompT5{}, a2);
        bsl::map m5h(p5b, p5e, CompT5{}, SA5{});
        bsl::map m5i(i5b, i5e, CompT5{}, BA5{});
        bsl::map m5j(i5b, i5e, CompT5{}, a1);
        bsl::map m5k(i5b, i5e, CompT5{}, a2);
        bsl::map m5l(i5b, i5e, CompT5{}, SA5{});

        ASSERT_SAME_TYPE(decltype(m5a), bsl::map<T5, T5>);
        ASSERT_SAME_TYPE(decltype(m5b), bsl::map<T5, T5>);
        ASSERT_SAME_TYPE(decltype(m5c), bsl::map<T5, T5, CompT5>);
        ASSERT_SAME_TYPE(decltype(m5d), bsl::map<T5, T5, CompT5>);
        ASSERT_SAME_TYPE(decltype(m5e), bsl::map<T5, T5, CompT5, BA5>);
        ASSERT_SAME_TYPE(decltype(m5f), bsl::map<T5, T5, CompT5, BA5>);
        ASSERT_SAME_TYPE(decltype(m5g), bsl::map<T5, T5, CompT5, BA5>);
        ASSERT_SAME_TYPE(decltype(m5h), bsl::map<T5, T5, CompT5, SA5>);
        ASSERT_SAME_TYPE(decltype(m5i), bsl::map<T5, T5, CompT5, BA5>);
        ASSERT_SAME_TYPE(decltype(m5j), bsl::map<T5, T5, CompT5, BA5>);
        ASSERT_SAME_TYPE(decltype(m5k), bsl::map<T5, T5, CompT5, BA5>);
        ASSERT_SAME_TYPE(decltype(m5l), bsl::map<T5, T5, CompT5, SA5>);


        typedef short T6;
        typedef bsl::allocator<bsl::pair<const T6, T6>> BA6;
        typedef std::allocator<bsl::pair<const T6, T6>> SA6;

        bsl::pair<T6, T6>          *p6b = nullptr;
        bsl::pair<T6, T6>          *p6e = nullptr;
        bsl::map<T6, T6>::iterator  i6b;
        bsl::map<T6, T6>::iterator  i6e;

        bsl::map m6a(p6b, p6e, BA6{});
        bsl::map m6b(p6b, p6e, a1);
        bsl::map m6c(p6b, p6e, a2);
        bsl::map m6d(p6b, p6e, SA6{});

        ASSERT_SAME_TYPE(decltype(m6a), bsl::map<T6, T6, std::less<T6>, BA6>);
        ASSERT_SAME_TYPE(decltype(m6b), bsl::map<T6, T6, std::less<T6>, BA6>);
        ASSERT_SAME_TYPE(decltype(m6c), bsl::map<T6, T6, std::less<T6>, BA6>);
        ASSERT_SAME_TYPE(decltype(m6d), bsl::map<T6, T6, std::less<T6>, SA6>);


        typedef long T7;
        typedef std::greater<T7> CompT7;
        typedef bsl::allocator       <bsl::pair<const T7, T7>> BA7;
        typedef std::allocator       <bsl::pair<const T7, T7>> SA7;
        typedef std::initializer_list<bsl::pair<const T7, T7>> IL7;

        IL7      il7({{1L, 1L}, {2L, 3L}});
        bsl::map m7a(il7);
        bsl::map m7b(il7, CompT7{});
        bsl::map m7c(il7, CompT7{}, BA7{});
        bsl::map m7d(il7, CompT7{}, a1);
        bsl::map m7e(il7, CompT7{}, a2);
        bsl::map m7f(il7, CompT7{}, SA7{});

        ASSERT_SAME_TYPE(decltype(m7a), bsl::map<T7, T7>);
        ASSERT_SAME_TYPE(decltype(m7b), bsl::map<T7, T7, CompT7>);
        ASSERT_SAME_TYPE(decltype(m7c), bsl::map<T7, T7, CompT7, BA7>);
        ASSERT_SAME_TYPE(decltype(m7d), bsl::map<T7, T7, CompT7, BA7>);
        ASSERT_SAME_TYPE(decltype(m7e), bsl::map<T7, T7, CompT7, BA7>);
        ASSERT_SAME_TYPE(decltype(m7f), bsl::map<T7, T7, CompT7, SA7>);

        typedef long long T8;
        typedef bsl::allocator       <bsl::pair<const T8, T8>> BA8;
        typedef std::allocator       <bsl::pair<const T8, T8>> SA8;
        typedef std::initializer_list<bsl::pair<const T8, T8>> IL8;

        IL8      il8({{3LL, 3LL}, {2LL, 1LL}});
        bsl::map m8a(il8, BA8{});
        bsl::map m8b(il8, a1);
        bsl::map m8c(il8, a2);
        bsl::map m8d(il8, SA8{});

        ASSERT_SAME_TYPE(decltype(m8a), bsl::map<T8, T8, std::less<T8>, BA8>);
        ASSERT_SAME_TYPE(decltype(m8b), bsl::map<T8, T8, std::less<T8>, BA8>);
        ASSERT_SAME_TYPE(decltype(m8c), bsl::map<T8, T8, std::less<T8>, BA8>);
        ASSERT_SAME_TYPE(decltype(m8d), bsl::map<T8, T8, std::less<T8>, SA8>);
    }

#undef ASSERT_SAME_TYPE
};
#endif  // BSLS_COMPILERFEATURES_SUPPORT_CTAD

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

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
      case 44: {
        if (verbose) printf(
                  "\nUSAGE EXAMPLE TEST IS HANDLED BY PRIMARY TEST DRIVER'"
                  "\n=====================================================\n");
      } break;
      case 43: {
        // --------------------------------------------------------------------
        // TESTING 'TRY_EMPLACE' AND 'INSERT_OR_ASSIGN'
        //
        // Concerns:
        //: 1 'try_emplace' only adds an entry to the map if the key does not
        //:   already exist, otherwise it does nothing.  Specifically, it does
        //:   not consume the arguments if the key already exists.
        //:
        //: 1 'insert_or_assign' only adds an entry to the map if the key does
        //:   not already exist, and otherwise updates an existing entry.
        //
        // Plan:
        //: 1 Construct a map and call 'try_emplace' with keys that both do
        //:   and do not exist.  Examine the map and the parameters to confirm
        //:   correct behavior. (C-1)
        //:
        //: 1 Construct a map and call 'insert_or_assign' with keys that both
        //:   do and do not exist.  Examine the map to confirm correct
        //:   behavior. (C-2)
        //
        // Testing:
        //   pair<iterator, bool> try_emplace(const key&, Args&&...);
        //   iterator try_emplace(const_iterator, const key&, Args&&...);
        //   pair<iterator, bool> try_emplace(key&&, Args&&...);
        //   iterator try_emplace(const_iterator, key&&, Args&&...);
        //   pair<iterator, bool> insert_or_assign(const key&, OTHER&&);
        //   iterator insert_or_assign(const_iterator, const key&, OTHER&&);
        //   pair<iterator, bool> insert_or_assign(key&&, OTHER&&);
        //   iterator insert_or_assign(const_iterator, key&&, OTHER&&);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'TRY_EMPLACE' AND 'INSERT_OR_ASSIGN'\n"
                     "============================================\n");
        TestDriver<char, size_t>::testCase43();
        TestDriver<int,  size_t>::testCase43();
        TestDriver<long, size_t>::testCase43();

        // test 'try_emplace' with different numbers of arguments
        {
            typedef bsl::map<int, IntValue> Map;
            typedef bsl::pair<Map::iterator, bool> PAIR;

            Map  m;
            PAIR p;

            p = m.try_emplace(1);
            ASSERT(p.second);
            ASSERT(1 == p.first->first);
            ASSERT(0 == p.first->second.d_value);

            p = m.try_emplace(2, 3);
            ASSERT(p.second);
            ASSERT(2 == p.first->first);
            ASSERT(3 == p.first->second.d_value);

            p = m.try_emplace(4, 5, 6);
            ASSERT(p.second);
            ASSERT(4 == p.first->first);
            ASSERT(11 == p.first->second.d_value);

            p = m.try_emplace(7, 8, 9, 10);
            ASSERT(p.second);
            ASSERT(7 == p.first->first);
            ASSERT(27 == p.first->second.d_value);
        }

      } break;
      case 42: {
        // --------------------------------------------------------------------
        // TESTING ERASE_IF
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FREE FUNCTION 'BSL::ERASE_IF'"
                            "\n=====================================\n");

        TestDriver<char, size_t>::testCase42();
        TestDriver<int,  size_t>::testCase42();
        TestDriver<long, size_t>::testCase42();
      } break;
      case 41: {
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
        //: 1 Create a map by invoking the constructor without supplying the
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
      case 40: {
        // --------------------------------------------------------------------
        // TESTING COMPARATORS WITH MULTI-VALUE EQUAL RANGES
        //   Although in most cases additional operators accepting reference
        //   to the 'LOOKUP_KEY' value are in compliance with the main
        //   comparator's operator, accepting two references to the 'KEY'
        //   value, user can provide a transparent comparator, whose operators
        //   change the equality ranges.  We need to check that our container
        //   handles such comparators correctly.
        //
        // Concerns:
        //: 1 The following 'map' methods: 'find, 'count', 'lower_bound',
        //:   'upper_bound' and 'equal_range' correctly handle additional
        //:   operators (those that accept parameters other than map 'KEY') of
        //:   supplied comparator, even if their behavior differs from the
        //:   behavior of the main one.
        //:
        //: 2 'count' can return a value greater than one.
        //:
        //: 3 'equal_range' can return range, containing more than one element
        //
        // Plan:
        //: 1 Construct a map with a transparent comparator, that has
        //:   additional operators, allowing multiple matches for the same
        //:   parameter value.
        //:
        //: 2 Fill the map with unique values and call the lookup functions
        //:   with a parameters that can be both unambiguously and ambiguously
        //:   matched with the map elements.  Verify the results.  (C-1..3)
        //
        // Testing:
        //   CONCERN: 'count'       properly handles multi-value comparators.
        //   CONCERN: 'find'        properly handles multi-value comparators.
        //   CONCERN: 'lower_bound' properly handles multi-value comparators.
        //   CONCERN: 'upper_bound' properly handles multi-value comparators.
        //   CONCERN: 'equal_range' properly handles multi-value comparators.
        // --------------------------------------------------------------------

        if (verbose) printf(
                "\n" "TESTING COMPARATORS WITH MULTI-VALUE EQUAL RANGES" "\n"
                     "=================================================" "\n");

        typedef bsl::map<bsl::string,
                         int,
                         TransparentComparatorWithMultiValueEqualRange>
                                                           TransparentMap;
        typedef TransparentMap::value_type                 TransparentMapValue;
        typedef TransparentMap::size_type                  size_type;
        typedef TransparentMap::iterator                   Iterator;
        typedef TransparentMap::const_iterator             ConstIterator;
        typedef bsl::pair<Iterator, Iterator>              ERResultType;
        typedef bsl::pair<ConstIterator, ConstIterator>    ConstERResultType;

        static const struct {
            int         d_lineNum;
            const char *d_keyValue_p;
        } DATA[] = {
        //    LINE  VALUE
        //    ----  ------
            { L_,   ""    },
            { L_,   "b"   },
            { L_,   "d"   },
            { L_,   "dd"  },
            { L_,   "f"   },
            { L_,   "ff"  },
            { L_,   "fff" },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bsl::map<char, int>   symbolEntryNumMap;

        TransparentMap        mX;
        const TransparentMap& X = mX;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                 LINE  = DATA[i].d_lineNum;
            const bsl::string         VALUE = DATA[i].d_keyValue_p;
            TransparentMapValue       MAP_VALUE(VALUE, i);

            mX.insert(MAP_VALUE);
            symbolEntryNumMap[VALUE[0]] += 1;

            for (char c = 'a'; c < 'h'; ++c) {
                // Testing 'find'.
                {
                    ConstIterator constResult = X.find(c);
                    Iterator      result      = mX.find(c);
                    if (0 == symbolEntryNumMap[c]) {
                        ASSERTV(LINE, c, X.end()  == constResult);
                        ASSERTV(LINE, c, mX.end() == result     );
                    }
                    else {
                        ASSERTV(LINE, c, X.end()      != constResult);
                        ASSERTV(LINE, c, mX.end()     != result     );
                        ASSERTV(LINE, c, *constResult == *result    );

                        ASSERTV(LINE, c, (*constResult).first[0],
                                c == (*constResult).first[0]);
                    }
                }

                // Testing 'lower_bound'.
                {
                    ConstIterator EXPECTED_CONST_LB =  X.begin();
                    Iterator      EXPECTED_LB       = mX.begin();

                    while (EXPECTED_CONST_LB != X.end()) {
                        if (c <= (*EXPECTED_CONST_LB).first[0]) {
                            break;
                        }
                        ++EXPECTED_CONST_LB;
                        ++EXPECTED_LB;
                    }

                    ConstIterator constResult = X.lower_bound(c);
                    Iterator      result      = mX.lower_bound(c);

                    ASSERTV(LINE, c, EXPECTED_CONST_LB == constResult);
                    ASSERTV(LINE, c, EXPECTED_LB       == result     );
                }

                // Testing 'upper_bound'.
                {
                    ConstIterator EXPECTED_CONST_UB =  X.begin();
                    Iterator      EXPECTED_UB       = mX.begin();

                    while (EXPECTED_CONST_UB != X.end()) {
                        if (c < (*EXPECTED_CONST_UB).first[0]) {
                            break;
                        }
                        ++EXPECTED_CONST_UB;
                        ++EXPECTED_UB;
                    }

                    ConstIterator constResult = X.upper_bound(c);
                    Iterator      result      = mX.upper_bound(c);

                    ASSERTV(LINE, c, EXPECTED_CONST_UB == constResult);
                    ASSERTV(LINE, c, EXPECTED_UB       == result     );
                }

                // Testing 'count'.
                {
                    ConstIterator   EXPECTED_CONST_LB = X.lower_bound(c);
                    const size_type EXPECTED_COUNT =
                     (X.end() == EXPECTED_CONST_LB) ? 0 : symbolEntryNumMap[c];

                    size_type       count       = X.count(c);

                    ASSERTV(c, EXPECTED_COUNT, count ,EXPECTED_COUNT == count);
                }

                // Testing 'equal_range'.
                {
                    ConstIterator   EXPECTED_CONST_LB = X.lower_bound(c);
                    ConstIterator   EXPECTED_CONST_UB = X.upper_bound(c);
                    Iterator        EXPECTED_LB = mX.lower_bound(c);
                    Iterator        EXPECTED_UB = mX.upper_bound(c);
                    const size_type EXPECTED_COUNT = X.count(c);

                    ERResultType      result      = mX.equal_range(c);
                    ConstERResultType constResult = X.equal_range(c);

                    ConstIterator  resultConstLB = constResult.first;
                    ConstIterator  resultConstUB = constResult.second;
                    Iterator       resultLB      = result.first;
                    Iterator       resultUB      = result.second;

                    ASSERTV(LINE, c, EXPECTED_CONST_LB == resultConstLB);
                    ASSERTV(LINE, c, EXPECTED_CONST_UB == resultConstUB);
                    ASSERTV(LINE, c, EXPECTED_LB       == resultLB     );
                    ASSERTV(LINE, c, EXPECTED_UB       == resultUB     );

                    for (size_type counter = 0;
                         counter < EXPECTED_COUNT;
                         ++counter) {
                         ++resultConstLB;
                         ++resultLB;
                    }

                    ASSERTV(LINE, c, EXPECTED_COUNT,
                            resultConstUB == resultConstLB);
                    ASSERTV(LINE, c, EXPECTED_COUNT,
                            resultUB      == resultLB     );
                }
            }
        }
      } break;
      case 39: {
        // --------------------------------------------------------------------
        // TESTING TRANSPARENT COMPARATOR
        //
        // Concerns:
        //: 1 'map' does not have a transparent set of lookup functions if the
        //:   comparator is not transparent.
        //:
        //: 2 'map' has a transparent set of lookup functions if the comparator
        //:   is transparent.
        //
        // Plan:
        //: 1 Construct a non-transparent map and call the lookup functions
        //:   with a type that is convertible to the 'value_type'.  There
        //:   should be exactly one conversion per call to a lookup function.
        //:   (C-1)
        //:
        //: 2 Construct a transparent map and call the lookup functions with a
        //:   type that is convertible to the 'value_type'.  There should be no
        //:   conversions.  (C-2)
        //
        // Testing:
        //   CONCERN: 'find'        properly handles transparent comparators.
        //   CONCERN: 'count'       properly handles transparent comparators.
        //   CONCERN: 'lower_bound' properly handles transparent comparators.
        //   CONCERN: 'upper_bound' properly handles transparent comparators.
        //   CONCERN: 'equal_range' properly handles transparent comparators.
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "TESTING TRANSPARENT COMPARATOR" "\n"
                                 "==============================" "\n");

        typedef bsl::map<int, int>                        NonTransparentMap;
        typedef NonTransparentMap::value_type             Value;
        typedef bsl::map<int, int, TransparentComparator> TransparentMap;

        const int DATA[] = { 0, 1, 2, 3, 4 };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        NonTransparentMap        mXNT;
        const NonTransparentMap& XNT = mXNT;

        for (int i = 0; i < NUM_DATA; ++i) {
            if (veryVeryVeryVerbose) {
                printf("Constructing test data\n");
            }
            const Value VALUE(DATA[i], DATA[i]);
            mXNT.insert(VALUE);
        }

        TransparentMap        mXT(mXNT.begin(), mXNT.end());
        const TransparentMap& XT = mXT;

        ASSERT(NUM_DATA == XNT.size());
        ASSERT(NUM_DATA == XT.size() );

        for (int i = 0; i < NUM_DATA; ++i) {
            const int VALUE = DATA[i];
            if (veryVerbose) {
                printf("Testing transparent comparators with a value of %d\n",
                       VALUE);
            }

            if (veryVerbose) {
                printf("\tTesting const non-transparent map.\n");
            }
            testTransparentComparator( XNT, false, VALUE);

            if (veryVerbose) {
                printf("\tTesting mutable non-transparent map.\n");
            }
            testTransparentComparator(mXNT, false, VALUE);

            if (veryVerbose) {
                printf("\tTesting const transparent map.\n");
            }
            testTransparentComparator( XT,  true,  VALUE);

            if (veryVerbose) {
                printf("\tTesting mutable transparent map.\n");
            }
            testTransparentComparator(mXT,  true,  VALUE);
        }
      } break;
      case 38: {
        // --------------------------------------------------------------------
        // 'erase' overload is deduced correctly
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING ABSENCE OF ERASE AMBIGUITY\n"
                            "==================================\n");

        TestDriver<EraseAmbiguityTestType, int>::testCase38();
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
                      bsltf::NonOptionalAllocTestType);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        TestDriver<bsltf::MovableAllocTestType,
                   bsltf::MoveOnlyAllocTestType>::testCase34();
        TestDriver<bsltf::MovableAllocTestType,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::
                                                                  testCase34();
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
                      bsltf::NonOptionalAllocTestType);
      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING EMPLACE WITH HINT
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase32,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonDefaultConstructibleTestType,
                      bsltf::NonOptionalAllocTestType);

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
                      bsltf::NonOptionalAllocTestType);

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
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING INSERTION OF MOVABLE VALUES
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase29,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
      } break;
      case 28: {
        if (verbose) printf(
                   "\nTEST CASE %d IS DELEGATED TO 'bslstl_map_test2.t.cpp'"
                   "\n=====================================================\n",
                   test);
      } break;
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
      case 12: // falls through
      case 11: // falls through
      case 10: // falls through
      case  9: {
        if (verbose) printf(
                   "\nTEST CASE %d IS DELEGATED TO 'bslstl_map_test1.t.cpp'"
                   "\n=====================================================\n",
                   test);
      } break;
      case  8: // falls through
      case  7: // falls through
      case  6: // falls through
      case  5: // falls through
      case  4: // falls through
      case  3: // falls through
      case  2: // falls through
      case  1: {
        if (verbose)
            printf("\nTEST CASE %d IS HANDLED BY PRIMARY TEST DRIVER"
                   "\n==============================================\n",
                   test);
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
