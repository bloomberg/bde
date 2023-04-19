// bslstl_map_test1.t.cpp                                             -*-C++-*-
#include <bslstl_map_test1.h>

#include <bslstl_forwarditerator.h>
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
// [40] USAGE EXAMPLE
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
            return bsltf::TemplateTestFacility::getIdentifier(lhs)
                 < bsltf::TemplateTestFacility::getIdentifier(rhs);   // RETURN
        }
        else {
            return bsltf::TemplateTestFacility::getIdentifier(lhs)
                 > bsltf::TemplateTestFacility::getIdentifier(rhs);   // RETURN
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

    template <class BDE_OTHER_TYPE>
    struct rebind
    {
        typedef DummyAllocator<BDE_OTHER_TYPE> other;
    };

    // CREATORS
    DummyAllocator()
    {
    }

    // DummyAllocator(const DummyAllocator& original) = default;

    template <class BDE_OTHER_TYPE>
    DummyAllocator(const DummyAllocator<BDE_OTHER_TYPE>& original)
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

// FREE OPERATORS
template <class TYPE>
bool lessThanFunction(const TYPE& lhs, const TYPE& rhs)
    // Return 'true' if the integer representation of the specified 'lhs' is
    // less than integer representation of the specified 'rhs'.
{
    return bsltf::TemplateTestFacility::getIdentifier(lhs)
         < bsltf::TemplateTestFacility::getIdentifier(rhs);
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
                    bsl::is_same<VALUE, bsltf::MoveOnlyAllocTestType>::value ||
                    bsl::is_same<VALUE,
                               bsltf::WellBehavedMoveOnlyAllocTestType>::value;

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
        // Test 'find', 'contains', 'count', 'upper_bound', 'lower_bound',
        // 'equal_range'.

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
    const int NUM_SPECS = static_cast<int>(sizeof SPECS / sizeof *SPECS);

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

                bslma::DefaultAllocatorGuard dag(&da);

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

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
    (void) [](const Obj& lhs, const Obj& rhs) -> bool {
        return lhs != rhs;
    };
#else
    // template <class Key, class T, class Compare, class Allocator>
    // bool operator!=(const map<Key, T, Compare, Allocator>& x,
    //                 const map<Key, T, Compare, Allocator>& y);

    bool (*operatorNe)(const Obj&, const Obj&) = operator!=;
    (void)operatorNe;
#endif

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE
    (void) [](const Obj& lhs, const Obj& rhs) -> bool {
        return lhs < rhs;
    };
    (void) [](const Obj& lhs, const Obj& rhs) -> bool {
        return lhs > rhs;
    };
    (void) [](const Obj& lhs, const Obj& rhs) -> bool {
        return lhs <= rhs;
    };
    (void) [](const Obj& lhs, const Obj& rhs) -> bool {
        return lhs >= rhs;
    };
    (void) [](const Obj& lhs, const Obj& rhs) {
        return lhs <=> rhs;
    };
#else
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
#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

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

    if (verbose) printf("Testing elemental access for '%s'.\n",
                        NameOf<VALUE>().name());
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
            if (veryVeryVerbose)
                printf("\tTest correct exception is thrown.\n");
            {
                bool exceptionCaught = false;
                try {
                    X.at(ZK);
                }
                catch (const std::out_of_range&) {
                    exceptionCaught = true;
                }
                ASSERTV(LINE, true == exceptionCaught);
            }
            {
                bool exceptionCaught = false;
                try {
                    mX.at(ZK);
                }
                catch (const std::out_of_range&) {
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
                printf("\tTest 'operator[]' on out-of-range key.\n");
            {
                const size_t SIZE = X.size();

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionProctor<Obj> proctor(&X, L_, &scratch);

                    mX[ZK];

                    proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, D, X.find(ZK)->second, D == X.find(ZK)->second);
                ASSERTV(LINE, SIZE, X.size(), SIZE + 1 == X.size());
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
    const int NUM_ID = static_cast<int>(sizeof ID / sizeof *ID);

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
    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

    if (verbose) printf("\tTesting 'empty'.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE = DATA[ti].d_lineNum;
            const char *SPEC = DATA[ti].d_spec_p;

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            ASSERTV(LINE, SPEC, (0 == ti) == X.empty());
            ASSERTV(LINE, SPEC, (0 == ti) == bsl::empty(X));

            mX.clear();

            ASSERTV(LINE, SPEC, true == X.empty());
            ASSERTV(LINE, SPEC, true == bsl::empty(X));
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
    //:
    //: 4 'operator<=>' is consistent with '<', '>', '<=', '>='.
    //
    // Plan:
    //: 1 For a variety of objects of different sizes and different values,
    //:   test that the comparison returns as expected.  (C-1..4)
    //
    // Testing:
    //   bool operator< (const map& lhs, const map& rhs);
    //   bool operator> (const map& lhs, const map& rhs);
    //   bool operator>=(const map& lhs, const map& rhs);
    //   bool operator<=(const map& lhs, const map& rhs);
    //   auto operator<=>(const map& lhs, const map& rhs);
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

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE
                ASSERTV(LINE1, LINE2,  isLess   == (X <=> Y < 0));
                ASSERTV(LINE1, LINE2, !isLessEq == (X <=> Y > 0));
                ASSERTV(LINE1, LINE2,  isLessEq == (X <=> Y <= 0));
                ASSERTV(LINE1, LINE2, !isLess   == (X <=> Y >= 0));
#endif

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
    //: 2 Repeat P-1 with 'erase(iterator position)'.
    //:
    //: 3 Repeat P-1 with 'erase(const key_type& key)' (C-2).
    //:
    //: 4 For range erase, call erase on all possible range of for each length,
    //:   'l' and verify result is as expected.
    //:
    //: 5 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid values, but not triggered for adjacent valid
    //:   ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
    //
    // Testing:
    //   iterator erase(const_iterator position);
    //   iterator erase(iterator position);
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

                Obj        mXC(&oa);              // const_iterator interface
                const Obj&  XC = gg(&mXC, SPEC);
                Obj        mX(&oa);               // iterator interface
                const Obj&  X  = gg(&mX,  SPEC);

                CIter POSC    = XC.find(VALUES[tj].first);
                Iter  POS     = mX.find(VALUES[tj].first);

                CIter AFTERC  = LENGTH - 1 == tj
                                ? XC.end()
                                : XC.find(VALUES[tj + 1].first);
                CIter BEFOREC = 0 == tj
                                ? XC.end()
                                : XC.find(VALUES[tj - 1].first);
                CIter AFTER   = LENGTH - 1 == tj
                                ? X.end()
                                : X.find(VALUES[tj + 1].first);
                CIter BEFORE  = 0 == tj
                                ? X.end()
                                : X.find(VALUES[tj - 1].first);

                ASSERTV(LINE, tj, POSC    != XC.end());
                ASSERTV(LINE, tj, POS     != X.end());
                ASSERTV(LINE, tj, AFTERC  != POSC);
                ASSERTV(LINE, tj, BEFOREC != POSC);
                ASSERTV(LINE, tj, AFTER   != POS);
                ASSERTV(LINE, tj, BEFORE  != POS);

                if (veryVerbose) { P(*POS); }

                bslma::TestAllocatorMonitor oam(&oa);

                const Iter RC = mXC.erase(POSC);
                const Iter R  = mX.erase(POS);

                if (veryVeryVerbose) { T_ T_ P_(XC) P(X); }

                // Check return value

                ASSERTV(LINE, tj, AFTERC == RC);
                ASSERTV(LINE, tj, AFTER  == R );

                // Check the element does not exist

                ASSERTV(LINE, tj, XC.end() == XC.find(VALUES[tj].first));
                ASSERTV(LINE, tj, X.end()  == X.find(VALUES[tj].first));
                if (0 == tj) {
                    ASSERTV(LINE, tj, XC.begin() == AFTERC);
                    ASSERTV(LINE, tj, X.begin()  == AFTER);
                }
                else {
                    ++BEFOREC;
                    ++BEFORE;
                    ASSERTV(LINE, tj, BEFOREC == AFTERC);
                    ASSERTV(LINE, tj, BEFORE  == AFTER);
                }

                ASSERTV(LINE, tj, oam.isTotalSame());
                if (TYPE_ALLOC) {
                    ASSERTV(LINE, tj, oam.isInUseDown());
                }
                else {
                    ASSERTV(LINE, tj, oam.isInUseSame());
                }
                ASSERTV(LINE, tj, XC.size(), LENGTH - 1 == XC.size());
                ASSERTV(LINE, tj, X.size(),  LENGTH - 1 == X.size());
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
        bsls::AssertTestHandlerGuard hG;

        if (veryVerbose) printf("'erase'\n");
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mXC;
            Obj mX;

            pair<CIter, bool> RESULTC = primaryManipulator(&mXC,
                                                           'A',
                                                           &scratch);
            pair<Iter, bool>  RESULT  = primaryManipulator(&mX, 'B', &scratch);
            ASSERTV(RESULTC.second);
            ASSERTV(RESULT.second);

            CIter iterC = mXC.end();
            Iter  iter  = mX.end();

            ASSERT_SAFE_FAIL(mXC.erase(iterC));
            ASSERT_SAFE_FAIL(mX.erase(iter));
            ASSERT_SAFE_PASS(mXC.erase(RESULTC.first));
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
    //: 8 QoI: Range insertion allocates a single block for nodes when the
    //:   number of elements can be determined and no free nodes are available.
    //:   (The contained elements may require additional allocations.)
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
    //: 5 Invoke the 'testRangeInsertOptimization' function that creates a
    //:   container for a non-allocating type that uses a test allocator to
    //:   supply memory.  'insert' elements using forward, random access, and
    //:   input (only) iterators.  'clear' the elements and re-insert them.
    //:   Compare the state of the allocator to expected memory needs at each
    //:   step of the scenario.  (C-8).  TBD TODO - that function is called in
    //:   test case 18, not from here.
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
    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

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
    //:   'contains' returns 'true', and 'upper_bound' returns an iterator
    //:   referring to the element after the searched element.
    //:
    //: 2 If the key being searched does not exist in the container, 'find'
    //:   returns the 'end' iterator, 'contains' returns 'false', and
    //:   'lower_bound' and 'upper_bound' :   return an iterator referring to
    //:   the smallest element greater than the searched element.
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
    //   bool contains(const key_type& key);
    //   bool contains(const LOOKUP_KEY& key);
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

                    bool cShouldBeFound = CITER[idx] != X.end();
                    ASSERTV(
                           ti,
                           tj,
                           cShouldBeFound,
                           cShouldBeFound == X.contains(VALUES[tj].first));

                    bool shouldBeFound  = ITER[idx] != X.end();
                    ASSERTV(cShouldBeFound,
                            shouldBeFound,
                            cShouldBeFound == shouldBeFound);

                    ASSERTV(
                           ti,
                           tj,
                           shouldBeFound,
                           shouldBeFound == mX.contains(VALUES[tj].first));

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
    //:
    //:14 QoI: Range insertion allocates a single block for nodes when the
    //:   number of elements can be determined and no free nodes are available.
    //:   (The contained elements may require additional allocations.)
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
    //:
    //: 4 Invoke the 'testRangeCtorOptimization' function that creates a
    //:   containers for a non-allocating type using random access, forward,
    //:   and input (only) iterators.  Each container is given a test allocator
    //:   to supply memory.  The test allocator is state compared to the
    //:   expected state in each step of the scenario.  (C-14)
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
    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

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

// ============================================================================
//                      RANGE CTOR/INSERT OPTIMIZATION
// ----------------------------------------------------------------------------

typedef bsl::pair<int, int> DataType;
typedef bsl::map <int, int> ContainerType;
static DataType  ARRAY[] = { DataType( 0,  0)
                           , DataType( 1,  1)
                           , DataType( 2,  2)
                           , DataType( 3,  3)
                           , DataType( 4,  4)
                           , DataType( 5,  5)
                           , DataType( 6,  6)
                           , DataType( 7,  7)
                           , DataType( 8,  8)
                           , DataType( 9,  9)
                           , DataType(10, 10)
                           , DataType(11, 11)
                           , DataType(12, 12)
                           , DataType(13, 13)
                           , DataType(14, 14)
                           , DataType(15, 15)
                           , DataType(16, 16)
                           , DataType(17, 17)
                           , DataType(18, 18)
                           , DataType(19, 19)
                           , DataType(20, 20)
                           , DataType(21, 21)
                           , DataType(22, 22)
                           , DataType(23, 23)
                           , DataType(24, 24)
                           , DataType(25, 25)
                           , DataType(26, 26)
                           , DataType(27, 27)
                           , DataType(28, 28)
                           , DataType(29, 29)
                           , DataType(30, 30)
                           , DataType(31, 31)
                           , DataType(32, 32)
                           , DataType(33, 33)
                           , DataType(34, 34)
                           , DataType(35, 35)
                           , DataType(36, 36)
                           , DataType(37, 37)
                           , DataType(38, 38)
                           , DataType(39, 39)
                           };
const std::size_t NUM_ELEMENTS = sizeof ARRAY / sizeof *ARRAY;

typedef bslstl::ForwardIterator<       DataType, DataType *> FwdItr;
typedef bslstl::RandomAccessIterator<  DataType, DataType *> RndItr;
typedef bsltf::TestValuesArrayIterator<DataType>             InpItr;

static void testRangeCtorOptimization()
{
   if (verbose) printf("\nTest Range CTOR Optimization\n");

    FwdItr beginFwd(ARRAY), endFwd(ARRAY + NUM_ELEMENTS);
    RndItr beginRnd(ARRAY), endRnd(ARRAY + NUM_ELEMENTS);

    bool         isValid[NUM_ELEMENTS + 1];
    bool isReferenceable[NUM_ELEMENTS + 1];

    memset(isValid,         true, (NUM_ELEMENTS + 1) * sizeof(bool));
    memset(isReferenceable, true, (NUM_ELEMENTS + 1) * sizeof(bool));
    isReferenceable[NUM_ELEMENTS] = false;  // 'end' is never dereferenceable

    InpItr beginInp(ARRAY,
                    ARRAY + NUM_ELEMENTS,
                    isReferenceable,
                    isValid);
    InpItr   endInp(ARRAY           + NUM_ELEMENTS,
                    ARRAY           + NUM_ELEMENTS,
                    isReferenceable + NUM_ELEMENTS,
                    isValid         + NUM_ELEMENTS);

    bslma::TestAllocator        sa("scratch", veryVeryVeryVerbose);
    bslma::TestAllocatorMonitor sam(&sa);

    if (verbose) {
        printf("\nAfore: Object-Allocator\n");
        sa.print();
    }

    ContainerType mXF(beginFwd, endFwd, &sa); const ContainerType& XF = mXF;
    ASSERT(NUM_ELEMENTS == XF.size());
    ASSERT(1            == sam.numBlocksTotalChange());

    sam.reset();
    ContainerType mXR(beginRnd, endRnd, &sa); const ContainerType& XR = mXR;
    ASSERT(NUM_ELEMENTS == XR.size());
    ASSERT(1            == sam.numBlocksTotalChange());

    sam.reset();
    ContainerType mXI(beginInp, endInp, &sa); const ContainerType& XI = mXI;
    ASSERT(NUM_ELEMENTS == XI.size());
    ASSERT(1            <  sam.numBlocksTotalChange());

    if (verbose) {
        P_(XF.size());
        P_(XR.size());
         P(XI.size());
        printf("\nAfter: Object-Allocator\n");
        sa.print();
    }

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::TestAllocatorMonitor  dam(&da);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) {
        printf("\nAfore: Default-Allocator\n");
        da.print();
    }

    ContainerType mYF(beginFwd, endFwd); const ContainerType& YF = mYF;
    ASSERT(NUM_ELEMENTS == YF.size());
    ASSERT(1            == dam.numBlocksTotalChange());

    dam.reset();
    ContainerType mYR(beginRnd, endRnd); const ContainerType& YR = mYR;
    ASSERT(NUM_ELEMENTS == YR.size());
    ASSERT(1            == dam.numBlocksTotalChange());

    // Our input (only) iterators were consumed in the construction of 'mXI'.
    // Reset them before reuse.

    memset(isValid,         true, (NUM_ELEMENTS + 1) * sizeof(bool));
    memset(isReferenceable, true, (NUM_ELEMENTS + 1) * sizeof(bool));
    isReferenceable[NUM_ELEMENTS] = false;  // 'end' is never dereferenceable

    dam.reset();
    ContainerType mYI(beginInp, endInp); const ContainerType& YI = mYI;
    ASSERT(NUM_ELEMENTS == YI.size());
    ASSERT(1            <  dam.numBlocksTotalChange());

    if (verbose) {
         P(YF.size());
        P_(YR.size());
        printf("\nAfter: Default-Allocator\n");
        da.print();
    }
}

static void testRangeInsertOptimization()
{
   if (verbose) printf("\nTest Range Insert Optimization\n");

    FwdItr beginFwd(ARRAY);
    FwdItr   midFwd(ARRAY + NUM_ELEMENTS/2);
    FwdItr   endFwd(ARRAY + NUM_ELEMENTS);

    RndItr beginRnd(ARRAY);
    RndItr   midRnd(ARRAY + NUM_ELEMENTS/2);
    RndItr   endRnd(ARRAY + NUM_ELEMENTS);
    bool         isValid[NUM_ELEMENTS + 1];
    bool isReferenceable[NUM_ELEMENTS + 1];

    memset(isValid,         true, (NUM_ELEMENTS + 1) * sizeof(bool));
    memset(isReferenceable, true, (NUM_ELEMENTS + 1) * sizeof(bool));
    isReferenceable[NUM_ELEMENTS] = false;  // 'end' is never dereferenceable

    InpItr beginInp(ARRAY,
                    ARRAY + NUM_ELEMENTS,
                    isReferenceable,
                    isValid);
    InpItr   endInp(ARRAY           + NUM_ELEMENTS,
                    ARRAY           + NUM_ELEMENTS,
                    isReferenceable + NUM_ELEMENTS,
                    isValid         + NUM_ELEMENTS);

    bslma::TestAllocator        sa("scratch", veryVeryVeryVerbose);
    bslma::TestAllocatorMonitor sam(&sa);

    if (verbose) {
        printf("\nAfore: Object-Allocator\n");
        sa.print();
    }

    ContainerType mX(&sa); const ContainerType& X = mX;
    ASSERT(0            == X.size());
    ASSERT(0            == sam.numBlocksTotalChange());

    mX.insert(beginFwd, endFwd);        // Insert entire range.
    ASSERT(NUM_ELEMENTS == X.size());   // Added elements.
    ASSERT(1            == sam.numBlocksTotalChange());
                                        // Had to allocate nodes.
                                        // No free nodes left.

    sam.reset();
    mX.insert(beginFwd, endFwd);       // Re-insert entire range.
    ASSERT(NUM_ELEMENTS == X.size());  // No-change since already in map.
    ASSERT(1            == sam.numBlocksTotalChange());
                                       // No free nodes so allocated more;
                                       // however, did not use them.

    sam.reset();
    mX.insert(beginFwd, endFwd);       // Re-re-insert entire range.
    ASSERT(NUM_ELEMENTS == X.size());  // No-change since already in map.
    ASSERT(0            == sam.numBlocksTotalChange());
                                       // Have free nodes so no new allocation.
                                       // The free nodes remain unused.

    sam.reset();
    mX.insert(beginFwd, endFwd);       // Re-re-re-insert entire range.
    ASSERT(NUM_ELEMENTS == X.size());  // No-change since already in map.
    ASSERT(0            == sam.numBlocksTotalChange());
                                       // Have free nodes so no new allocation.
                                       // The free nodes remain unused.
    // ...

    sam.reset();
    ContainerType mY(&sa); const ContainerType& Y = mY;
    ASSERT(0              == Y.size());
    ASSERT(0              == sam.numBlocksInUseChange());

    mY.insert(beginFwd, midFwd);        // Insert first half of 'DATA'.
    ASSERT(NUM_ELEMENTS/2 == Y.size());
    ASSERT(1              == sam.numBlocksInUseChange());

    sam.reset();
    mY.clear();                         // Clear
    ASSERT(0              == Y.size());
    ASSERT(0              == sam.numBlocksInUseChange());

    mY.insert(beginFwd, midFwd);       // Re-insert previous elements
    ASSERT(NUM_ELEMENTS/2 == Y.size());
    ASSERT(0              == sam.numBlocksInUseChange());
                                      // Reused nodes.  No new allocation.
                                      // No free nodes left.

    mY.insert(midRnd, endRnd);        // Insert additional elements
    ASSERT(NUM_ELEMENTS   == Y.size());
    ASSERT(1              == sam.numBlocksInUseChange());
                                      // Allocated more nodes.

    sam.reset();
    ContainerType mZ(&sa); const ContainerType& Z = mZ;
    ASSERT(0              == Z.size());
    ASSERT(0              == sam.numBlocksInUseChange());

    mZ.insert(beginInp, endInp);
    ASSERT(NUM_ELEMENTS  == Z.size());
    ASSERT(1             <  sam.numBlocksInUseChange());

    if (verbose) {
        P(X.size());
        printf("\nAfter: Object-Allocator\n");
        sa.print();
    }
}

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
      case 41: {
        if (verbose) printf(
                  "\nUSAGE EXAMPLE TEST IS HANDLED BY PRIMARY TEST DRIVER'"
                  "\n=====================================================\n");
      } break;
      case 40: // falls through
      case 39: // falls through
      case 38: // falls through
      case 37: // falls through
      case 36: // falls through
      case 35: // falls through
      case 34: // falls through
      case 33: // falls through
      case 32: // falls through
      case 31: // falls through
      case 30: // falls through
      case 29: {
        if (verbose) printf(
                   "\nTEST CASE %d IS DELEGATED TO 'bslstl_map_test3.t.cpp'"
                   "\n=====================================================\n",
                   test);
      } break;
      case 28: {
        if (verbose) printf(
                   "\nTEST CASE %d IS DELEGATED TO 'bslstl_map_test2.t.cpp'"
                   "\n=====================================================\n",
                   test);
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTOR
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase27,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        TestDriver<bsltf::MovableAllocTestType,
                   bsltf::MoveOnlyAllocTestType>::testCase27();

        TestDriver<int,
                   bsltf::MoveOnlyAllocTestType>::testCase27();

        TestDriver<int,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::testCase27();
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
                      bsltf::NonOptionalAllocTestType);

        TestDriver<TestKeyType, TestValueType>::testCase24();
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase23,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

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
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        TestDriver<TestKeyType, TestValueType>::testCase18();
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING RANGE 'insert'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase17,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        TestDriver<TestKeyType, TestValueType>::testCase17();

        testRangeInsertOptimization();
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING INSERTION-WITH-HINT OF SINGLE VALUE
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase16,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        TestDriver<TestKeyType, TestValueType>::testCase16();
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING INSERTION OF SINGLE VALUE
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase15,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        TestDriver<TestKeyType, TestValueType>::testCase15();
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ITERATORS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        TestDriver<TestKeyType, TestValueType>::testCase14();
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING SEARCH FUNCTIONS ('find', 'count', 'contains', etc.)
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

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
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        TestDriver<TestKeyType, TestValueType>::testCase12();

// TBD We should test inserting move-only values from a range given by
// move-enabled iterators.
//
// We should also test that constructing from a range using a different
// allocator (e.g., a range of 'string' values mapped by the key) does not
// incur making a temporary with the default allocator for each iteration.
// This might be a separate test case (12a?) for a single data type to test
// that concern.

        testRangeCtorOptimization();
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
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

        TestDriver<TestKeyType, TestValueType>::testCase9();

        // 'propagate_on_container_copy_assignment' testing

        RUN_EACH_TYPE(TestDriver,
                      testCase9_propagate_on_container_copy_assignment,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

        TestDriver<TestKeyType, TestValueType>::
                            testCase9_propagate_on_container_copy_assignment();
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
