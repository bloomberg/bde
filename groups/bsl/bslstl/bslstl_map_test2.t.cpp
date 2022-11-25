// bslstl_map_test2.t.cpp                                             -*-C++-*-
#include <bslstl_map_test2.h>

#include <bslstl_forwarditerator.h>
#include <bslstl_iterator.h>
#include <bslstl_map.h>
#include <bslstl_pair.h>
#include <bslstl_randomaccessiterator.h>
#include <bslstl_string.h>

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
#include <bslmf_integralconstant.h>
#include <bslmf_movableref.h>
#include <bslmf_removeconst.h>

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
//                      TEST CONFIGURATION MACROS
// ----------------------------------------------------------------------------

#if defined(BDE_BUILD_TARGET_EXC)
// The following enum is set to '1' when exceptions are enabled and to '0'
// otherwise.  It is here to avoid having preprocessor macros throughout.
enum { PLAT_EXC = 1 };
#else
enum { PLAT_EXC = 0 };
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

                            // =============================
                            // struct ThrowingMoveComparator
                            // =============================

template <class TYPE>
struct ThrowingMoveComparator : public std::less<TYPE> {
    // Comparator with throwing move operations.

    // CREATORS
    ThrowingMoveComparator()
        // Create a 'ThrowingMoveComparator' object.
    {
    }

    ThrowingMoveComparator(const ThrowingMoveComparator &other)
        // Create a 'ThrowingMoveComparator' object having the same value as
        // that of the specified 'other'.
    {
        (void)other;
    }

    ThrowingMoveComparator(bslmf::MovableRef<ThrowingMoveComparator> other)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Create a 'ThrowingMoveComparator' object having the same value as
        // that of the specified 'other'.
    {
        (void)other;
    }

    // MANIPULATORS
    ThrowingMoveComparator &operator=(const ThrowingMoveComparator &other)
        // Assign to this object the value of the specified 'other'.
    {
        (void)other;
        return *this;
    }

    ThrowingMoveComparator &operator=(
        bslmf::MovableRef<ThrowingMoveComparator> other)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Assign to this object the value of the specified 'other'.
    {
        (void)other;
        return *this;
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

                   // ===================================
                   // template struct TestMovableTypeUtil
                   // ===================================

template <class ITER, class VALUE_TYPE>
struct TestMovableTypeUtil {
    static ITER findFirstNotMovedInto(ITER, ITER end)
    {
        return end;
    }
};

template <class ITER>
struct TestMovableTypeUtil<ITER, bsltf::MovableAllocTestType> {
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

    typedef bslma::ConstructionUtil               ConstrUtil;
    typedef bslmf::MovableRefUtil                 MoveUtil;
    typedef bsltf::MoveState                      MoveState;
    typedef bsltf::TemplateTestFacility           TstFacility;
    typedef TestMovableTypeUtil<CIter, ValueType> TstMoveUtil;

    typedef bsl::allocator_traits<ALLOC>          AllocatorTraits;

    enum AllocCategory { e_BSLMA, e_ADAPTOR, e_STATEFUL };

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

  public:
    // TEST CASES
    static void testCase28_dispatch();
        // Test move-assignment operator.

    static void testCase28_noexcept();
        // Test move assignment operator noexcept.
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
void TestDriver<KEY, VALUE, COMP, ALLOC>::testCase28_noexcept()
    // Verify that noexcept specification of the move assignment operator is
    // correct.
{
    Obj a;
    Obj b;

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
    const bool isNoexcept =
                        bsl::allocator_traits<ALLOC>::is_always_equal::value &&
                        std::is_nothrow_move_assignable<COMP>::value;
    ASSERT(isNoexcept ==
           BSLS_KEYWORD_NOEXCEPT_OPERATOR(a = MoveUtil::move(b)));
#endif
}

                    // ==============================
                    // template struct MetaTestDriver
                    // ==============================

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
};

                    // ------------------------------
                    // template struct MetaTestDriver
                    // ------------------------------


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
        // --------------------------------------------------------------------
        // TESTING MOVE-ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase28,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        // Because the 'KEY' type in the pair is 'const', the move c'tor for
        // 'bsl::map' calls the copy c'tor of 'KEY', and move-assign calls that
        // move c'tor, we can't move-assign a container with a move-only 'KEY'.

        MetaTestDriver<bsltf::MovableAllocTestType,
                   bsltf::MoveOnlyAllocTestType>::testCase28();

        MetaTestDriver<int,
                   bsltf::MoveOnlyAllocTestType>::testCase28();

        MetaTestDriver<int,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::testCase28();

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
        // Test noexcept
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false> Alloc;
            typedef TestComparator<int> Comp;

            ASSERT(!bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT( std::is_nothrow_move_assignable<Comp>::value);
            TestDriver<int, int, Comp, Alloc>::testCase28_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true> Alloc;
            typedef TestComparator<int> Comp;

            ASSERT( bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT( std::is_nothrow_move_assignable<Comp>::value);
            TestDriver<int, int, Comp, Alloc>::testCase28_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false> Alloc;
            typedef ThrowingMoveComparator<int> Comp;

            ASSERT(!bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT(!std::is_nothrow_move_assignable<Comp>::value);
            TestDriver<int, int, Comp, Alloc>::testCase28_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true> Alloc;
            typedef ThrowingMoveComparator<int> Comp;

            ASSERT( bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT(!std::is_nothrow_move_assignable<Comp>::value);
            TestDriver<int, int, Comp, Alloc>::testCase28_noexcept();
        }
#endif
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
