// bslstl_unorderedmultimap.t.cpp                                     -*-C++-*-
#include <bslstl_unorderedmultimap.h>

#include <bslstl_iterator.h>  // for testing only
#include <bslstl_string.h>    // for testing only

#include <bslalg_rangecompare.h>
#include <bslalg_swaputil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_mallocfreeallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>
#include <bslmf_haspointersemantics.h>
#include <bslmf_integralconstant.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_platform.h>
#include <bsls_util.h>

#include <bsltf_nonoptionalalloctesttype.h>
#include <bsltf_stdallocatoradaptor.h>
#include <bsltf_stdstatefulallocator.h>
#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;
using bsls::NameOf;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// NOTICE: To reduce the compilation time, this test driver has been broken
// into 2 parts, 'bslstl_unorderedmultimap.t.cpp' (cases 1-11, plus the usage
// example), and 'bslstl_unorderedmultimap_test.cpp' (cases 12 and higher).
//
//                             Overview
//                             --------
// The component under test defines a container whose interface and contract
// are dictated by the C++ standard.  The general concerns are compliance,
// exception safety, and proper dispatching (for member function templates such
// as assign and insert).  This container is implemented in the form of a class
// template, and thus its proper instantiation for several types is a concern.
// Regarding the allocator template argument, we use mostly a 'bsl::allocator'
// together with a 'bslma::TestAllocator' mechanism, but we also verify the C++
// standard.
//
// Primary Manipulators:
//: o 'insert'  (via 'primaryManipulator' helper function)
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
// 'insert' and 'clear' methods to be used by the generator functions 'gg' and
// 'ggg'.  Note that some manipulators must support aliasing, and those that
// perform memory allocation must be tested for exception neutrality via the
// 'bslma_testallocator' component.  After the mandatory sequence of cases
// (1--10) for value-semantic types (cases 5 and 10 are not implemented, as
// there is not output or streaming below bslstl), we test each individual
// constructor, manipulator, and accessor in subsequent cases.
//
// An initial breathing test iterates over all operations with a single
// template instantiation and tests obvious boundary conditions and iterator
// stability guarantees.
//
// TBD: MULTIMAP TEST SHOULD TEST DUPLICATE KEYS (by equivalence functor) WITH
// DIFFERENT VALUES (by operator==).
//
// TBD: MEMORY CONSUMPTION TESTS ARE GENERALLY DISABLED, OTHER THAN LEAKS AT
// THE END OF TEST SEQUENCES
//
// TBD: NEED A PROPER TEST TO EVALUATE unordered_multimap INVARIANTS:
//    equivalent keys form contiguous ranges
//    once inserted, relative order is preserved through all operations
//    contained values correspond to inserted values, counting duplicates
// ----------------------------------------------------------------------------
// [unord.map] construct/copy/destroy:
// [ 2] unordered_multimap(const A& allocator);
// [ 2] unordered_multimap(size_t, allocator);
// [ 2] unordered_multimap(size_t, hash, allocator);
// [ 2] unordered_multimap(size_t, hash, equal, allocator);
// [12] unordered_multimap(ITER, ITER, allocator);
// [12] unordered_multimap(ITER, ITER, size_t, allocator);
// [12] unordered_multimap(ITER, ITER, size_t, hash, allocator);
// [12] unordered_multimap(ITER, ITER, size_t, hash, equal, allocator);
// [ 7] unordered_multimap(const unordered_multimap& original);
// [ 7] unordered_multimap(const unordered_multimap& original, const A& alloc);
// [26] unordered_multimap(unordered_multimap&& original);
// [26] unordered_multimap(unordered_multimap&& original, const A& allocator);
// [32] unordered_multimap(initializer_list, const A& allocator);
// [32] unordered_multimap(initializer_list, size_t, const A& allocator);
// [32] unordered_multimap(initializer_list, size_t, hasher, const A& alloc);
// [32] unordered_multimap(initializer_list, size_t, hasher, equal, const A&);
// [ 2] ~unordered_multimap();
// [11] unordered_multimap& operator=(const unordered_multimap& rhs);
// [27] unordered_multimap& operator=(unordered_multimap&& rhs);
// [32] unordered_multimap& operator=(initializer_list<value_type>);
//
// iterators:
// [14] iterator begin();
// [14] const_iterator begin() const;
// [14] iterator end();
// [14] const_iterator end() const;
// [ 4] const_iterator cbegin() const;
// [ 4] const_iterator cend() const;
//
// capacity:
// [20] bool empty() const;
// [ 4] size_type size() const;
// [20] size_type max_size() const;
//
// modifiers:
// [ 2] void clear();
// [30] iterator emplace(Args&&... arguments);
// [31] iterator emplace_hint(const_iterator hint, Args&&... args);
// [18] iterator erase(const_iterator position);
// [18] iterator erase(iterator position);
// [18] size_type erase(const key_type& key);
// [18] iterator erase(const_iterator first, const_iterator last);
// [15] iterator insert(const value_type& value);
// [28] iterator insert(value_type&& value);
// [ 2] iterator insert(const_iterator hint, const value_type& value);
// [29] iterator insert(const_iterator hint, const value_type&& value);
// [32] iterator insert(initializer_list<value_type>);
// [17] void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
// [ 8] void swap(unordered_multimap& other);
//
// observers:
// [21] hasher hash_function() const;
// [21] key_equal key_eq() const;
// [ 4] allocator_type get_allocator() const;
//
// unordered_multimap operations:
// [13] bool contains(const key_type& key);
// [13] bool contains(const LOOKUP_KEY& key);
// [13] size_type count(const KEY& key) const;
// [13] iterator find(const key_type& key);
// [13] const_iterator find(const key_type& key) const;
// [13] size_type count(const key_type& key) const;
// [13] bsl::pair<iterator, iterator> equal_range(const key_type& key);
// [13] bsl::pair<const_iter, const_iter> equal_range(const key_type&) const;
//
// bucket interface:
// [ 4] size_type bucket_count() const;
// [ 4] size_type max_bucket_count() const;
// [ 4] size_type bucket_size(size_type n) const;
// [ 4] size_type bucket(const key_type& k) const;
//
// bucket iterators:
// [14] local_iterator begin(size_type n);
// [14] const_local_iterator begin(size_type n) const;
// [14] local_iterator end(size_type n);
// [14] const_local_iterator end(size_type n) const;
// [14] const_local_iterator cbegin(size_type n) const;
// [14] const_local_iterator cend(size_type n) const;
//
// hash policy:
// [34] float load_factor() const;
// [34] float max_load_factor() const;
// [34] void max_load_factor(float z);
// [34] void rehash(size_type n);
// [34] void reserve(size_type n);
//
// specialized algorithms:
// [ 6] bool operator==(unordered_multimap& lhs, unordered_multimap& rhs);
// [ 6] bool operator!=(unordered_multimap& lhs, unordered_multimap& rhs);
// [ 8] void swap(unordered_multimap& a, unordered_multimap& b);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] default construction (only)
// [39] CLASS TEMPLATE DEDUCTION GUIDES
// [40] USAGE EXAMPLE
//
// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] int ggg(unordered_multimap *object, const char *s, int verbose);
// [ 3] unordered_multimap& gg(unordered_multimap *object, const char *s);
//
// [22] CONCERN: 'unordered_multimap' is compatible with standard allocators.
// [23] CONCERN: 'unordered_multimap' has the necessary type traits.
// [  ] CONCERN: The type provides the full interface defined by the standard.
// [36] CONCERN: The values are spread into different buckets.
// [35] CONCERN: 'unordered_multimap' supports incomplete types
// [36] CONCERN: Methods qualifed 'noexcept' in standard are so implemented.
// [37] CONCERN: 'erase' overload is deduced correctly.
// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
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
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
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

#define EXCEPTION_TEST_BEGIN(CONTAINER)                                       \
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(                             \
          (* (bslma::TestAllocator *) (CONTAINER).get_allocator().mechanism()))

#define EXCEPTION_TEST_END  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

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
//                      TEST CONFIGURATION MACROS
// ----------------------------------------------------------------------------

enum {
#if defined(BDE_BUILD_TARGET_EXC)
    PLAT_EXC = 1
#else
    PLAT_EXC = 0
#endif
};

// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                             BSL TEST SUPPORT
//-----------------------------------------------------------------------------

namespace bsl {

template <class FIRST, class SECOND>
inline
void debugprint(const bsl::pair<FIRST, SECOND>& p)
{
    bsls::BslTestUtil::callDebugprint(p.first);
    bsls::BslTestUtil::callDebugprint(p.second);
}

// unordered_multimap-specific print function.

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
void debugprint(const unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>&
                                                                             s)
{
    if (s.empty()) {
        printf("<empty>");
    }
    else {
        typedef unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR> Obj;
        typedef typename Obj::const_iterator CIter;

        putchar('"');
        const CIter end = s.cend();
        for (CIter ci = s.cbegin(); end != ci; ++ci) {
            putchar(static_cast<char>(
                       bsltf::TemplateTestFacility::getIdentifier(ci->first)));
        }
        putchar('"');
    }
    fflush(stdout);
}

}  // close namespace bsl

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslma::ConstructionUtil                ConsUtil;
typedef bslmf::MovableRefUtil                  MoveUtil;
typedef bsltf::TemplateTestFacility            TstFacility;
typedef bsls::Types::UintPtr                   UintPtr;
typedef bsls::Types::Int64                     Int64;

typedef bsltf::NonDefaultConstructibleTestType TestKeyType;
typedef bsltf::NonTypicalOverloadsTestType     TestValueType;

// Define DEFAULT DATA used in multiple test cases.
struct DefaultDataRow {
    int         d_line;     // source line number
    int         d_index;    // lexical order
    const char *d_spec;     // specification string, for input to 'gg' function
    const char *d_results;  // expected element values
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line idx  spec                 results
    //---- ---  --------             -------
    { L_,    0, "",                  "" },
    { L_,    1, "A",                 "A" },
    { L_,    2, "B",                 "B" },
    { L_,    3, "AA",                "AA" },
    { L_,    4, "AB",                "AB" },
    { L_,    4, "BA",                "AB" },
    { L_,    5, "AC",                "AC" },
    { L_,    6, "CD",                "CD" },
    { L_,    7, "ABC",               "ABC" },
    { L_,    7, "ACB",               "ABC" },
    { L_,    7, "BAC",               "ABC" },
    { L_,    7, "BCA",               "ABC" },
    { L_,    7, "CAB",               "ABC" },
    { L_,    7, "CBA",               "ABC" },
    { L_,   25, "AAB",               "AAB" },
    { L_,   25, "ABA",               "AAB" },
    { L_,   25, "BAA",               "AAB" },
    { L_,   26, "ABB",               "ABB" },
    { L_,   26, "BBA",               "ABB" },
    { L_,   26, "BAB",               "ABB" },
    { L_,    8, "BAD",               "ABD" },
    { L_,    9, "ABCA",              "AABC" },
    { L_,   10, "ABCB",              "ABBC" },
    { L_,   11, "ABCC",              "ABCC" },
    { L_,   13, "ABCD",              "ABCD" },
    { L_,   13, "ACBD",              "ABCD" },
    { L_,   13, "BDCA",              "ABCD" },
    { L_,   13, "DCBA",              "ABCD" },
    { L_,   14, "BEAD",              "ABDE" },
    { L_,   15, "BCDE",              "BCDE" },
    { L_,   16, "ABCDE",             "ABCDE" },
    { L_,   16, "ACBDE",             "ABCDE" },
    { L_,   16, "CEBDA",             "ABCDE" },
    { L_,   16, "EDCBA",             "ABCDE" },
    { L_,   17, "FEDCB",             "BCDEF" },
    { L_,   12, "ABCABC",            "AABBCC" },
    { L_,   12, "AABBCC",            "AABBCC" },
    { L_,   12, "CABBAC",            "AABBCC" },
    { L_,   18, "FEDCBA",            "ABCDEF" },
    { L_,   18, "DAFCBE",            "ABCDEF" },
    { L_,   19, "ABCDEFG",           "ABCDEFG" },
    { L_,   20, "ABCDEFGH",          "ABCDEFGH" },
    { L_,   21, "ABCDEFGHI",         "ABCDEFGHI" },
    { L_,   22, "ABCDEFGHIJKLMNOP",  "ABCDEFGHIJKLMNOP" },
    { L_,   22, "PONMLKJIGHFEDCBA",  "ABCDEFGHIJKLMNOP" },
    { L_,   24, "ABCDEFGHIJKLMNOPQ", "ABCDEFGHIJKLMNOPQ" },
    { L_,   24, "DHBIMACOPELGFKNJQ", "ABCDEFGHIJKLMNOPQ" }
};
static const int DEFAULT_NUM_DATA = static_cast<int>(
                                   sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA);

//=============================================================================
//                              TEST SUPPORT
//-----------------------------------------------------------------------------

namespace {

template <class TYPE>
const TYPE& my_max(const TYPE& x, const TYPE& y)
{
    return x > y ? x : y;
}

template <class TYPE>
TYPE my_abs(const TYPE& x)
{
    return x < 0 ? -x : x;
}

template <class TYPE>
bool nearlyEqual(const TYPE& x, const TYPE& y)
{
    TYPE tolerance = my_max(my_abs(x), my_abs(y)) * 0.0001;
    return my_abs(x - y) <= tolerance;
}

template <class KEY, class VALUE, class ALLOC>
struct IntToPairConverter {
    // Convert an 'int' value to a 'bsl::pair' of the (template parameter)
    // 'KEY' and 'VALUE' types.

    // CLASS METHODS
    static void createInplace(bsl::pair<KEY, VALUE> *address,
                              int                    value,
                              ALLOC                  allocator)
    {
        BSLS_ASSERT(address);
        BSLS_ASSERT(0 < value && value < 128);

        // If creating the 'key' and 'value' temporary objects requires an
        // allocator, it should not be the default allocator as that will
        // confuse the arithmetic of our test machinery.  Therefore, we will
        // use the global MallocFree allocator, as being the simplest, least
        // obtrusive allocator that is also unlikely to be employed by an end
        // user.

        bslma::Allocator *privateAllocator =
                                      &bslma::MallocFreeAllocator::singleton();

        bsls::ObjectBuffer<typename bsl::remove_const<KEY>::type > tempKey;
        TstFacility::emplace(tempKey.address(),
                             value,
                             privateAllocator);

        bsls::ObjectBuffer<VALUE> tempValue;
        TstFacility::emplace(tempValue.address(),
                             value - 'A' + '0',
                             privateAllocator);

        bsl::allocator_traits<ALLOC>::construct(
                                           allocator,
                                           address,
                                           MoveUtil::move(tempKey.object()),
                                           MoveUtil::move(tempValue.object()));
    }
};

                                //-----------
                                // verifySpec
                                //-----------

template <class CONTAINER>
bool verifySpec(const CONTAINER& obj, const char *spec, bool keysOnly = false)
    // Return 'true' if the specified 'object' exactly matches the specified
    // 'spec'.  If the spec is invalid (contains characters outside the range
    // "['A' .. 'Z']") 'false' will be returned.  If the specified 'keysOnly'
    // is 'true', only key values in the map are verified, otherwise both key
    // values and mapped values are verified.
{
    typedef typename CONTAINER::key_type        Key;
    typedef typename CONTAINER::mapped_type     Value;
    typedef typename CONTAINER::const_iterator  CIter;
    typedef typename CONTAINER::value_type      ValueType;
    typedef bsl::allocator<ValueType>           Alloc;

    bslma::Allocator *pDa = &bslma::NewDeleteAllocator::singleton();
    bslma::DefaultAllocatorGuard guard(pDa);

    bsltf::TestValuesArray<
                          ValueType,
                          Alloc,
                          IntToPairConverter<const Key, Value, Alloc> > VALUES;
    const char *ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // If the spec contains any chars outside "['A' .. 'Z']" it's an invalid
    // spec.
    for (const char *pc = spec; *pc; ++pc) {
        if (*pc < 'A' || *pc > 'Z') {
            return false;                                             // RETURN
        }
    }

    for (const char *pc = ALPHABET; *pc; ++pc) {
        const char C = *pc;

        size_t count = 0;
        for (const char *pcB = spec; *pcB; ++pcB) {
            count += (C == *pcB);
        }

        // We can't rely on the 'count' member function yet, so we have to do
        // it by hand.

        // Entries with equivalent values must be contiguos.
        CIter it = obj.begin();
        while (obj.end() != it &&
               (keysOnly ?  VALUES[C - 'A'].first  != it->first
                         : (VALUES[C - 'A'].first  != it->first &&
                            VALUES[C - 'A'].second != it->second))) {
            ++it;
        }

        while (obj.end() != it &&
               (keysOnly ?  VALUES[C - 'A'].first  == it->first
                         : (VALUES[C - 'A'].first  == it->first &&
                            VALUES[C - 'A'].second == it->second))) {
            ++it;
            --count;
        }

        if (0 != count) {
            return false;                                             // RETURN
        }
    }

    return true;
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

                            // ======================
                            // class ExceptionProctor
                            // ======================

template <class OBJECT, class ALLOCATOR>
struct ExceptionProctor{
    // This class provide a mechanism to verify the strong exception guarantee
    // in exception-throwing code.  On construction, this class stores a copy
    // of an object of the (template parameter) type 'OBJECT' and the address
    // of that object.  On destruction, if 'release' was not invoked, it will
    // verify the value of the object is the same as the value of the copy
    // created on construction.  This class requires the copy constructor and
    // 'operator ==' to be tested before use.

    // DATA
    int           d_line;      // the line number at construction
    OBJECT        d_control;   // copy of the object being tested
    const OBJECT *d_object_p;  // address of the original object

  public:
    // CREATORS
    ExceptionProctor(const OBJECT    *object,
                     int              line,
                     const ALLOCATOR& basicAllocator)
    : d_line(line)
    , d_control(*object, basicAllocator)
    , d_object_p(object)
        // Create an exception proctor for the specified 'object' at the
        // specified 'line' number that uses the specified 'basicAllocator' to
        // supply memory.  Note that a 'bslma::Allocator *' can be supplied for
        // 'basicAllocator' if the type 'ALLOCATOR' is 'bsl::allocator'.
    {
    }

    ExceptionProctor(const OBJECT             *object,
                     int                       line,
                     bslmf::MovableRef<OBJECT> control)
    : d_line(line)
    , d_control(MoveUtil::move(control))
    , d_object_p(object)
        // Create an exception proctor for the specified 'object' at the
        // specified 'line' number using the specified 'control' object.
    {
    }

    ~ExceptionProctor()
        // Destroy this object.  If the proctor was not released, verify that
        // the state of the object supplied at construction has not change.
    {
        if (d_object_p) {
            const int LINE = d_line;
            ASSERTV(LINE, d_control, *d_object_p, d_control == *d_object_p);
        }
    }

    // MANIPULATORS
    void release()
        // Release this proctor from verifying the state of the object.
    {
        d_object_p = 0;
    }
};

                            // ====================
                            // class CompareProctor
                            // ====================

template <class OBJECT>
struct CompareProctor {
    // Take pointers to two non-owned objects of the same type, and ensure that
    // they compare equal upon destruction.

    const OBJECT *d_a_p;
    const OBJECT *d_b_p;
    const int     d_line;

    // CREATOR
    CompareProctor(const OBJECT& a, const OBJECT& b, int line)
    : d_a_p(&a)
    , d_b_p(&b)
    , d_line(line)
    {}

    ~CompareProctor()
    {
        ASSERTV(d_line, !d_a_p == !d_b_p);

        if (d_a_p) {
            ASSERTV(d_line, *d_a_p, *d_b_p, *d_a_p == *d_b_p);
        }
    }

    // MANIPULATORS
    void release()
    {
        d_a_p = d_b_p = 0;
    }
};

                       // ====================
                       // class TestComparator
                       // ====================

bool g_enableAllFunctorsFlag = true;

template <class TYPE>
class TestEqualityComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the (template parameter) 'TYPE'.
    // The function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.  The function-call operator also
    // increments a counter used to keep track the method call count.  Object
    // of this class can be identified by an id passed on construction.

    // DATA
    int         d_id;           // identifier for the functor
    mutable int d_count;        // number of times 'operator()' is called

  public:
    // CLASS METHOD
    static void disableFunctor()
        // Disable all objects of 'TestComparator' such that an 'ASSERT' will
        // be triggered if 'operator()' is invoked
    {
        g_enableAllFunctorsFlag = false;
    }

    static void enableFunctor()
        // Enable all objects of 'TestComparator' such that 'operator()' may
        // be invoked
    {
        g_enableAllFunctorsFlag = true;
    }

    // CREATORS
    //! TestEqualityComparator(const TestEqualityComparator& original) =
    //                                                                 default;
        // Create a copy of the specified 'original'.

    explicit TestEqualityComparator(int id = 0)
        // Create a 'TestComparator'.  Optionally, specify 'id' that can be
        // used to identify the object.
    : d_id(id)
    , d_count(0)
    {
    }

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs', and 'false' otherwise.
    {
        if (!g_enableAllFunctorsFlag) {
            ASSERTV(!"'TestComparator' was invoked when it was disabled");
        }

        ++d_count;

        return TstFacility::getIdentifier(lhs)
            == TstFacility::getIdentifier(rhs);
    }

    bool operator== (const TestEqualityComparator& rhs) const
    {
        return (id() == rhs.id());// && d_compareLess == rhs.d_compareLess);
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


template <class TYPE>
class TestNonConstEqualityComparator {
    // This test class provides a functor for equality comparison of objects
    // where the 'operator()' is not declared const.

  public:
    // CREATORS
    TestNonConstEqualityComparator() {}
        // Create a 'TestComparator'.  Optionally, specify 'id' that can be
        // used to identify the object.

    //! TestNonConstEqualityComparator(const TestEqualityComparator& original)
    //                                                               = default;
        // Create a copy of the specified 'original'.

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs)
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs', and 'false' otherwise.
    {
        return TstFacility::getIdentifier(lhs)
            == TstFacility::getIdentifier(rhs);
    }

    bool operator==(const TestNonConstEqualityComparator&) const
    {
        return true;
    }
};

template <class TYPE>
class TestHashFunctor {
    // This test class provides a mechanism that defines a function-call
    // operator that computes the hash of objects of the (template parameter)
    // 'TYPE'.  The function-call operator returns the hash value equal to the
    // integer converted from the object of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.  The function-call operator also
    // increments a counter used to keep track the method call count.  Object
    // of this class can be identified by an id passed on construction.

    // DATA
    int         d_id;           // identifier for the functor
    mutable int d_count;        // number of times 'operator()' is called
    bool        d_shortCircuit; // always returns 0 if true

  public:
    // CREATORS
    //! TestHashFunctor(const TestHashFunctor& original) = default;
        // Create a copy of the specified 'original'.

    explicit TestHashFunctor(int id = 0, bool shortCircuit = false)
        // Create a 'TestHashFunctor'.  Optionally, specify 'id' that can be
        // used to identify the object.  Also optionally specify
        // 'shortCircuit', which, if 'true' indicates that this functor will
        // always return 0.
    : d_id(id)
    , d_count(0)
    , d_shortCircuit(shortCircuit)
    {
    }

    // ACCESSORS
    size_t operator() (const TYPE& obj) const
        // Increment a counter that records the number of times this method is
        // called.   Return the hash value equal to the integer converted from
        // the object of 'TYPE' by the class method
        // 'TemplateTestFacility::getIdentifier' if 'd_shortCircuit == false',
        // and '0' otherwise.
    {
        ++d_count;

        return d_shortCircuit ? 0 : TstFacility::getIdentifier(obj);
    }

    bool operator==(const TestHashFunctor& rhs) const
    {
        return (id() == rhs.id());
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

template <class TYPE>
class TestNonConstHashFunctor {
    // This class provides a hash functor whose 'operator()()' hasn't been
    // declared 'const'.

  public:
    // CREATORS
    TestNonConstHashFunctor() {}
        // Create a copy of the specified 'original'.
    //! TestNonConstHashFunctor(const TestNonConstHashFunctor& original)
    //                                                               = default;
        // Create a copy of the specified 'original'.

    // ACCESSORS
    size_t operator() (const TYPE& obj)
        // Return the hash value equal to the integer converted from the object
        // of 'TYPE' by the class method 'TemplateTestFacility::getIdentifier'.
    {
        return TstFacility::getIdentifier(obj);
    }

    bool operator==(const TestNonConstHashFunctor&)
    {
        return true;
    }
};

                            // =======================
                            // struct ThrowingSwapHash
                            // =======================

template <class TYPE>
struct ThrowingSwapHash : public bsl::hash<TYPE> {
    // Hash functor with throwing 'swap'.

    // MANIPULATORS
    void swap(
            ThrowingSwapHash& other) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Exchange the value of this object with that of the specified 'other'
        // object.
    {
        (void)other;
    }

    // FREE FUNCTIONS
    friend void swap(
                ThrowingSwapHash& a,
                ThrowingSwapHash& b) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Exchange the values of the specified 'a' and 'b' objects.
    {
        (void)a;
        (void)b;
    }
};
                            // ========================
                            // struct ThrowingSwapEqual
                            // ========================

template <class TYPE>
struct ThrowingSwapEqual : public bsl::equal_to<TYPE> {
    // Equal functor with throwing 'swap'.

    // MANIPULATORS
    void swap(
           ThrowingSwapEqual& other) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Exchange the value of this object with that of the specified 'other'
        // object.
    {
        (void)other;
    }

    // FREE FUNCTIONS
    friend void swap(
               ThrowingSwapEqual& a,
               ThrowingSwapEqual& b) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Exchange the values of the specified 'a' and 'b' objects.
    {
        (void)a;
        (void)b;
    }
};

}  // close unnamed namespace

template <class ITER, class KEY, class VALUE>
class TestMovableTypeUtil
{
  public:
    static ITER findFirstNotMovedInto(ITER, ITER end)
    {
        return end;
    }
};

template <class ITER>
class TestMovableTypeUtil<ITER,
                          bsltf::MovableAllocTestType,
                          bsltf::MovableAllocTestType>
{
  public:
    static ITER findFirstNotMovedInto(ITER begin, ITER end)
    {
        for (; begin != end; ++begin) {
            if (!begin->second.movedInto())
            {
                break;
            }
        }
        return begin;
    }
};

class TestAllocatorUtil
{
  public:
    template <class TYPE>
    static void test(const TYPE&, const bslma::Allocator&)
    {
    }

    static void test(const bsltf::AllocEmplacableTestType& value,
                     const bslma::Allocator&               oa)
    {
        ASSERTV(&oa == value.arg01().allocator());
        ASSERTV(&oa == value.arg02().allocator());
        ASSERTV(&oa == value.arg03().allocator());
        ASSERTV(&oa == value.arg04().allocator());
        ASSERTV(&oa == value.arg05().allocator());
        ASSERTV(&oa == value.arg06().allocator());
        ASSERTV(&oa == value.arg07().allocator());
        ASSERTV(&oa == value.arg08().allocator());
        ASSERTV(&oa == value.arg09().allocator());
        ASSERTV(&oa == value.arg10().allocator());
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
    // definitions of 'bsl::unordered_multimap' can contain incomplete types on
    // all supported platforms.
    //
    // See 'TestIncompleteType' in bslstl_map.t.cpp for the rationale behind
    // this test type.

    // PUBLIC TYPES
    typedef bsl::unordered_multimap<int, IncompleteType>::iterator       Iter1;
    typedef bsl::unordered_multimap<IncompleteType, int>::iterator       Iter2;
    typedef bsl::unordered_multimap<IncompleteType, IncompleteType>::iterator
                                                                         Iter3;

    // PUBLIC DATA
    bsl::unordered_multimap<int, IncompleteType>            d_data1;
    bsl::unordered_multimap<IncompleteType, int>            d_data2;
    bsl::unordered_multimap<IncompleteType, IncompleteType> d_data3;

#if defined(BDE_BUILD_TARGET_SAFE_2)
    ~TestIncompleteType();
        // If building in 'SAFE_2' mode, the invariants-check in the destructor
        // for 'bslstl::HashTable' requires the key type to be complete, and
        // hashable.  Deferring the definition of this class's destructor until
        // after the incomplete key type has been fully defined, with hashing
        // support provided, resolves these issues.
#endif
};

struct IncompleteType {
    int d_data;
};

#if defined(BDE_BUILD_TARGET_SAFE_2)
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM &hashAlg, const IncompleteType &object)
    // Apply the specified 'hashAlg' to the specified 'object'
{
    using bslh::hashAppend;
    hashAppend(hashAlg, object.d_data);
}

inline
TestIncompleteType::~TestIncompleteType()
{
}
#endif

}  // close unnamed namespace

//=============================================================================
//                              TestDriver
//-----------------------------------------------------------------------------

                            // ================
                            // class TestDriver
                            // ================

template <class KEY,
          class VALUE = KEY,
          class HASH  = TestHashFunctor<KEY>,
          class EQUAL = TestEqualityComparator<KEY>,
          class ALLOC = bsl::allocator<bsl::pair<const KEY, VALUE> > >
class TestDriver {
    // This parameterized class provides a namespace for testing the
    // 'unordered_multimap' container.  The (template parameter) 'KEY',
    // 'VALUE', 'HASH', 'EQUAL', and 'ALLOC' types specify the key type, mapped
    // type, hash functor, key-equivalence comparator, and allocator type,
    // respectively.  Each "testCase*" method tests a specific aspect of
    // 'unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOC>'.  Every test case
    // should be invoked with various types to fully test the container.

  private:
    // TYPES
    typedef bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOC>  Obj;

    typedef typename Obj::iterator                           Iter;
    typedef typename Obj::const_iterator                     CIter;
    typedef typename Obj::local_iterator                     LIter;
    typedef typename Obj::const_local_iterator               CLIter;
    typedef typename Obj::size_type                          SizeType;
    typedef typename Obj::value_type                         ValueType;

    typedef bsltf::TestValuesArray<
                                 ValueType,
                                 ALLOC,
                                 IntToPairConverter<const KEY, VALUE, ALLOC> >
                                                                    TestValues;
    // Shorthands
    typedef bslmf::MovableRefUtil                            MoveUtil;
    typedef bsltf::MoveState                                 MoveState;
    typedef TestMovableTypeUtil<CIter, KEY, VALUE>           TstMoveUtil;

    typedef typename bsl::remove_const<KEY>::type            NoConstKey;
    typedef bsl::pair<NoConstKey, VALUE>                     TValueType;

    BSLMF_ASSERT((!bslmf::IsSame< Iter,  CIter>::value));
    BSLMF_ASSERT((!bslmf::IsSame<LIter, CLIter>::value));

    typedef TestNonConstEqualityComparator<KEY>              NonConstEqual;
        // Comparator functor with a non-'const' function call operator.

    static const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value +
                                  bslma::UsesBslmaAllocator<VALUE>::value;

    typedef bsl::integral_constant<bool,
        bsl::is_same<VALUE, bsltf::MoveOnlyAllocTestType>::value ||
        bsl::is_same<VALUE,
              bsltf::WellBehavedMoveOnlyAllocTestType>::value> IsValueMoveOnly;

  public:
    typedef bsltf::StdTestAllocator<ValueType> StlAlloc;

  private:
    // TEST APPARATUS
    //-------------------------------------------------------------------------
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters [A..Z] correspond to arbitrary (but unique) char
    // values to be appended to the
    // 'unordered_multimap<KEY, VALUE, COMP, ALLOC>' object.
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
    // <ITEM>       ::= <ELEMENT>
    //
    // <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'Z'
    //                                      // unique but otherwise arbitrary
    // Spec String  Description
    // -----------  -----------------------------------------------------------
    // ""           Has no effect; leaves the object unchanged.
    // "A"          Insert the value corresponding to A.
    // "AA"         Insert two values both corresponding to A.
    // "ABC"        Insert three values corresponding to A, B and C.
    //-------------------------------------------------------------------------

    static int ggg(Obj *object, const char *spec, int verbose = 1);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'insert' and white-box
        // manipulator 'clear'.  Optionally specify a zero 'verbose' to
        // suppress 'spec' syntax error messages.  Return the index of the
        // first invalid character, and a negative value otherwise.  Note that
        // this function is used to implement 'gg' as well as allow for
        // verification of syntax error detection.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified object with its value adjusted
        // according to the specified 'spec'.

    static bool matchFirstValues(const Obj&        object,
                                 const TestValues& values,
                                 const size_t      count,
                                 bool              keysOnly = false);
        // Return true if exactly the first specified 'count' values of the
        // specified array 'values' match all of the objects in the specified
        // 'object', and 'false' otherwise.

    static void storeFirstNElemAddr(typename Obj::const_pointer *pointers,
                                    const Obj&                   object,
                                    size_t                       n)
    {
        size_t i = 0;

        for (CIter b = object.cbegin(); b != object.cend() && i < n; ++b)
        {
            pointers[i++] = bsls::Util::addressOf(*b);
        }
    }

    static
    int checkFirstNElemAddr(typename Obj::const_pointer *pointers,
                            const Obj&                   object,
                            size_t                       n)
    {
        int    count = 0;
        size_t i     = 0;

        for (CIter b = object.cbegin(); b != object.end() && i < n; ++b)
        {
            if (pointers[i++] != bsls::Util::addressOf(*b)) {
                ++count;
            }
        }
        return count;
    }

    static Iter primaryManipulator(Obj   *container,
                                   int    identifier,
                                   ALLOC  allocator)
        // Insert into the specified 'container' the value object indicated by
        // the specified 'identifier', ensuring that the overload of the
        // primary manipulator taking a modifiable rvalue is invoked (rather
        // than the one taking an lvalue).  Return the result of invoking the
        // primary manipulator.
    {
        typedef bsl::pair<KEY, VALUE> TValueType;

        bsls::ObjectBuffer<TValueType> buffer;
        IntToPairConverter<KEY, VALUE, ALLOC>::createInplace(buffer.address(),
                                                             identifier,
                                                             allocator);
        bslma::DestructorGuard<TValueType> guard(buffer.address());

        return container->insert(MoveUtil::move(buffer.object()));
    }

    static int getIndexForIter(const Obj& obj, Iter it);
        // Find the index corresponding to the specified iterator 'it', which
        // must be a valid iterator referring to the specified 'obj'.  Note
        // that 'obj.end() == it' is allowed.

    static CIter getIterForIndex(const Obj& obj, size_t idx);
    static Iter  getIterForIndex(      Obj& obj, size_t idx);
        // Return the iterator relating to the specified 'obj' with specified
        // index 'idx'.  The behavior is undefined unless 'idx < obj.size()'.

    template <class T>
    static bslmf::MovableRef<T> testArg(T& t, bsl::true_type )
    {
        return MoveUtil::move(t);
    }

    template <class T>
    static const T&             testArg(T& t, bsl::false_type)
    {
        return t;
    }

  public:
    // TEST CASES
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

    static void testCase8_noexcept();
        // Test 'swap' noexcept.

    static void testCase8();
        // Test 'swap' member.

    template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
              bool OTHER_FLAGS>
    static void testCase7_select_on_container_copy_construction_dispatch();
    static void testCase7_select_on_container_copy_construction();
        // Test 'select_on_container_copy_construction'.

    static void testCase7();
        // Test copy constructor.

    static void testCase6_perturbValue(Obj             *pmX,
                                       Obj             *pmY,
                                       bsl::false_type  isMoveOnly);
    static void testCase6_perturbValue(Obj             *pmX,
                                       Obj             *pmY,
                                       bsl::true_type   isMoveOnly);
        // If the specified 'isMoveOnly' is 'false_type', meaning that 'VALUE'
        // has a public copy c'tor or public copy-assignment operator, modify
        // the 'mapped' elements, but not the 'key' elements of the specified
        // container '*pmX', and compare it to the specified container '*pmY'
        // and verify they compare unequal.  Then restore '*pmX' to its initial
        // state and verify the containers compare equal.  If 'isMoveOnly'
        // argument is a 'true_type', meaning 'VALUE' is move-only, use the
        // 'setData' accessor to perturb the 'mapped' elements.
        //
    static void testCase6();
        // Test equality operator ('operator==').

#if 0
    static void testCase5();
        // Reserved for (<<) operator. N/A
#endif

    static void testCase4();
        // Test basic accessors.

    static void testCase3();
        // Test generator functions 'ggg' and 'gg'.

    static void testCase2();
        // Test primary manipulators.
};

template <class KEY, class VALUE = KEY>
class StdAllocTestDriver : public TestDriver<KEY,
                                             VALUE,
                                             TestHashFunctor<KEY>,
                                             TestEqualityComparator<KEY>,
                      bsltf::StdTestAllocator<bsl::pair<const KEY, VALUE> > > {
};

template <class KEY, class VALUE = KEY>
class StdBslmaTestDriver : public TestDriver<KEY,
                                             VALUE,
                                             TestHashFunctor<KEY>,
                                             TestEqualityComparator<KEY>,
    bsltf::StdAllocatorAdaptor<bsl::allocator<bsl::pair<const KEY, VALUE> > > >
{
};

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
int TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::ggg(Obj        *object,
                                                    const char *spec,
                                                    int         verbose)
{
    enum { SUCCESS = -1 };

    bslma::TestAllocator scratch;
    bsl::allocator<ValueType> sscratch(&scratch);

    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Z') {
            primaryManipulator(object, spec[i], sscratch);
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

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOC>&
TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::gg(Obj        *object,
                                               const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
bool TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::matchFirstValues(
                                                    const Obj&        object,
                                                    const TestValues& values,
                                                    const size_t      count,
                                                    bool              keysOnly)
{
    CIter end = object.cend();
    size_t sizeObj = 0;
    for (CIter it = object.cbegin(), itB = it; end != it; it = itB) {
        size_t numInObj = 0;
        do {
            ++numInObj;
            ++itB;
        } while (end != itB && (keysOnly ? it->first == itB->first
                                         : *it       == *itB));

        size_t numInValues = 0;
        for (size_t j = 0; j < count; ++j) {
            if (keysOnly ? values[j].first == it->first
                         : values[j]       == *it) {
                ++numInValues;
            }
        }

        if (numInObj != numInValues) {
            return false;                                             // RETURN
        }

        sizeObj += numInObj;
    }

    return true;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
int TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::getIndexForIter(const Obj& obj,
                                                                Iter       it)
{
    int ret = 0;
    for (Iter itb = obj.begin(); it != itb; ++itb) {
        ++ret;
    }

    return ret;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
typename
        bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOC>::const_iterator
TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::getIterForIndex(const Obj& obj,
                                                            size_t     idx)
{
    if (idx > obj.size()) {
        ASSERTV(idx <= obj.size());

        return obj.end();
    }

    CIter  ret = obj.begin();
    size_t i   = 0;
    for (; i < idx && obj.end() != ret; ++i) {
        ++ret;
    }

    ASSERTV(idx == i);

    return ret;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
typename bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOC>::iterator
TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::getIterForIndex(Obj&   obj,
                                                            size_t idx)
{
    if (idx > obj.size()) {
        ASSERTV(idx <= obj.size());

        return obj.end();
    }

    Iter   ret = obj.begin();
    size_t i   = 0;
    for (; i < idx && obj.end() != ret; ++i) {
        ++ret;
    }

    ASSERTV(idx == i);

    return ret;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::
                    testCase9_propagate_on_container_copy_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   KEY,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS>                       StdAlloc;

    typedef bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, StdAlloc> Obj;

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

        const Obj W(IVALUES.begin(), IVALUES.end(),
                    1, HASH(), EQUAL(), scratch);    // control

        // Create target object.
        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            {
                IVALUES.resetIterators();

                Obj mY(IVALUES.begin(), IVALUES.end(),
                       1, HASH(), EQUAL(), mas);
                const Obj& Y = mY;

                if (veryVerbose) { T_ P_(ISPEC) P_(Y) P(W) }

                Obj mX(JVALUES.begin(), JVALUES.end(),
                       1, HASH(), EQUAL(), mat);
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

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::
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

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase9()
{
    // ------------------------------------------------------------------------
    // COPY-ASSIGNMENT OPERATOR
    //   Ensure that we can assign the value of any object of the class to any
    //   object of the class, such that the two objects subsequently have the
    //   same value.
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
    //   unordered_multimap& operator=(const unordered_multimap& rhs);
    // ------------------------------------------------------------------------

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
            const int         INDEX1  = DATA[ti].d_index;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZ(&scratch);  const Obj& Z  = gg(&mZ,  SPEC1);
            Obj mZZ(&scratch); const Obj& ZZ = gg(&mZZ, SPEC1);

            mZ.max_load_factor(2.0);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            // Create second object
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2  = DATA[tj].d_line;
                const char *const SPEC2  = DATA[tj].d_spec;
                const int         INDEX2 = DATA[tj].d_index;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX( &oa);  const Obj& X  = gg(&mX,   SPEC2);
                    Obj mXX(&oa);  const Obj& XX = gg(&mXX,  SPEC2);

                    mX.max_load_factor(3.0);

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    ASSERTV(LINE1, LINE2, Z, X,
                            (Z == X) == (INDEX1 == INDEX2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        ASSERT(XX == X);

                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(2.0 == X.max_load_factor());

                    ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                    ASSERTV(LINE1, LINE2, &scratch == Z.get_allocator());

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

#if !defined(BDE_BUILD_TARGET_SAFE_2)
                    // When mX and Z use different allocators, the assignment
                    // is done through the copy-swap idiom.  This means the
                    // copy is destroyed within the assignment, and in SAFE2
                    // mode the destructor calls 'isWellFormed', which
                    // allocates from the default allocator.

                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());
#endif
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                             0 == oa.numBlocksInUse());
            }
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_SWAP_FLAG,
          bool OTHER_FLAGS>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::
                               testCase8_propagate_on_container_swap_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<KEY,
                                        OTHER_FLAGS,
                                        OTHER_FLAGS,
                                        PROPAGATE_ON_CONTAINER_SWAP_FLAG,
                                        OTHER_FLAGS>                  StdAlloc;

    typedef bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, StdAlloc> Obj;

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

        // if 'false == PROPAGATE', the allocators must compare equal

        StdAlloc xma(&xoa);
        StdAlloc yma(PROPAGATE ? &yoa : &xoa);

        StdAlloc scratch(&da);

        const Obj ZZ(IVALUES.begin(), IVALUES.end(),
                     1, HASH(), EQUAL(), scratch);        // control

        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            const Obj WW(JVALUES.begin(), JVALUES.end(),
                         1, HASH(), EQUAL(), scratch);    // control

            {
                IVALUES.resetIterators();

                Obj mX(IVALUES.begin(), IVALUES.end(),
                       1, HASH(), EQUAL(), xma);
                const Obj& X = mX;

                if (veryVerbose) { T_ P_(ISPEC) P_(X) P(ZZ) }

                JVALUES.resetIterators();

                Obj mY(JVALUES.begin(), JVALUES.end(),
                       1, HASH(), EQUAL(), yma);
                const Obj& Y = mY;

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
                        ASSERTV(ISPEC, JSPEC, yma == X.get_allocator());
                        ASSERTV(ISPEC, JSPEC, xma == Y.get_allocator());

                        ASSERTV(ISPEC, JSPEC, dam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, xoam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, yoam.isTotalSame());
                    }
                    else {
                        ASSERTV(ISPEC, JSPEC, xma == X.get_allocator());
                        ASSERTV(ISPEC, JSPEC, yma == Y.get_allocator());
                    }
                }

                // free function 'swap'
                {
                    bslma::TestAllocatorMonitor dam(&da);
                    bslma::TestAllocatorMonitor xoam(&xoa);
                    bslma::TestAllocatorMonitor yoam(&yoa);

                    swap(mX, mY);

                    ASSERTV(ISPEC, JSPEC, ZZ, X, ZZ == X);
                    ASSERTV(ISPEC, JSPEC, WW, Y, WW == Y);

                    ASSERTV(ISPEC, JSPEC, xma == X.get_allocator());
                    ASSERTV(ISPEC, JSPEC, yma == Y.get_allocator());

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

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::
                                        testCase8_propagate_on_container_swap()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS: ALLOCATOR PROPAGATION
    //
    // Concerns:
    //: 1 If the 'propagate_on_container_swap' trait is 'false', the
    //:   allocators used by the source and target objects remain unchanged
    //:   (i.e., the allocators are *not* exchanged).
    //:
    //: 2 If the 'propagate_on_container_swap' trait is 'true', the
    //:   allocator used by the target (source) object is updated to be a copy
    //:   of that used by the source (target) object (i.e., the allocators
    //:   *are* exchanged).
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

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase8_noexcept()
    // Verify that noexcept specification of the member 'swap' function is
    // correct.
{
    Obj a;
    Obj b;

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
    const bool isNoexcept =
                        bsl::allocator_traits<ALLOC>::is_always_equal::value &&
                        bsl::is_nothrow_swappable<HASH>::value &&
                        bsl::is_nothrow_swappable<EQUAL>::value;
    ASSERT(isNoexcept == BSLS_KEYWORD_NOEXCEPT_OPERATOR(a.swap(b)));
#endif
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase8()
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
    //: 7 Both functions provides the strong exception guarantee w.t.r. to
    //:   memory allocation .
    //:
    //: 8 The free 'swap' function is discoverable through ADL (Argument
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
    //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
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
    //:       'Obj', 'mX', from 'XX' (P-4.2).
    //:
    //:     2 Use the value constructor and 'oa' to create a modifiable
    //:       'Obj', 'mY', and having the value described by 'R2'; also use
    //:       the copy constructor to create, using a "scratch" allocator,
    //:       a 'const' 'Obj', 'YY', from 'Y'.
    //:
    //:     3 Use, in turn, the member and free 'swap' functions to swap
    //:       the values of 'mX' and 'mY'; verify, after each swap, that:
    //:       (C-1..2)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 The common object allocator address held by 'mX' and 'mY'
    //:         is unchanged in both objects.  (C-4)
    //:
    //:       3 There was no additional object memory allocation.  (C-4)
    //:
    //:     5 Use the value constructor and 'oaz' to a create a modifiable
    //:       'Obj' 'mZ', having the value described by 'R2'; also use the copy
    //:       constructor to create, using a "scratch" allocator, a const
    //:       'Obj', 'ZZ', from 'Z'.
    //:
    //:     6 Use the member and free 'swap' functions to swap the values of
    //:       'mX' and 'mZ' respectively (when
    //:       AllocatorTraits::propagate_on_container_swap is an alias to
    //:       false_type) under the presence of exception; verify, after each
    //:       swap, that:  (C-1, 5, 7)
    //:
    //:       1 If exception occurred during the swap, both values are
    //:         unchanged.  (C-7)
    //
    //:       2 If no exception occurred, the values have been exchanged.
    //:         (C-1)
    //:
    //:       3 The common object allocator address held by 'mX' and 'mZ' is
    //:         unchanged in both objects.  (C-5)
    //:
    //:       4 Temporary memory were allocated from 'oa' if 'mZ' is not empty,
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
    //:    7 Create a new object allocator, 'oap'.
    //:
    //:    8 Use the value constructor and 'oap' to create a modifiable 'Obj'
    //:      'mP', having the value described by 'R2'; also use the copy
    //:      constructor to create, using a "scratch" allocator, a const 'Obj',
    //:      'PP', from 'P.
    //:
    //:    9 Manually change 'AllocatorTraits::propagate_on_container_swap' to
    //:      be an alias to 'true_type' (Instead of this manual step, use an
    //:      allocator that enables propagate_on_container_swap when
    //:      AllocatorTraits supports it) and use the member and free 'swap'
    //:      functions to swap the values 'mX' and 'mZ' respectively; verify,
    //:      after each swap, that:  (C-1, 6)
    //:
    //:      1 The values have been exchanged.  (C-1)
    //:
    //:      2 The allocators addresses have been exchanged.  (C-6)
    //:
    //:      3 There was no additional object memory allocation.  (C-6)
    //:
    //: 5 Verify that the free 'swap' function is discoverable through ADL:
    //:   (C-8)
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
    //:     in this component, then verify that:  (C-8)
    //:
    //:     1 The values have been exchanged.  (C-1)
    //:
    //:     2 There was no additional object memory allocation.  (C-4)
    //
    // Testing:
    //   void swap(unordered_multimap& other);
    //   void swap(unordered_multimap& a, unordered_multimap& b);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nSWAP MEMBER AND FREE FUNCTIONS"
                        "\n==============================\n");

    if (verbose) printf("\nTesting signatures.\n");
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

    if (verbose) printf(
       "\nUse a table of distinct object values and expected memory usage.\n");

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE1   = DATA[ti].d_line;
        const char *const SPEC1   = DATA[ti].d_spec;

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj mW(&oa);  const Obj& W = gg(&mW,  SPEC1);
        const Obj XX(W, &scratch);

        if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

        // Ensure the first row of the table contains the
        // default-constructed value.

        ASSERTV(LINE1, Obj(), W, 0 != ti || Obj() == W);

        // member 'swap'
        {
            bslma::TestAllocatorMonitor oam(&oa);

            mW.swap(mW);

            ASSERTV(LINE1, XX, W, XX == W);
            ASSERTV(LINE1, &oa == W.get_allocator());
            ASSERTV(LINE1, oam.isTotalSame());
        }

        // free function 'swap'
        {
            bslma::TestAllocatorMonitor oam(&oa);

            swap(mW, mW);

            ASSERTV(LINE1, XX, W, XX == W);
            ASSERTV(LINE1, &oa == W.get_allocator());
            ASSERTV(LINE1, oam.isTotalSame());
        }

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const int         LINE2   = DATA[tj].d_line;
            const char *const SPEC2   = DATA[tj].d_spec;

            Obj mX(XX, &oa);  const Obj& X = mX;

            Obj mY(&oa);  const Obj& Y = gg(&mY, SPEC2);
            const Obj YY(Y, &scratch);

            mX.max_load_factor(2.0f);
            mY.max_load_factor(3.0f);

            ASSERT(2.0f == X.max_load_factor());
            ASSERT(3.0f == Y.max_load_factor());

            if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mX.swap(mY);

                ASSERT(3.0f == X.max_load_factor());
                ASSERT(2.0f == Y.max_load_factor());

                ASSERTV(LINE1, LINE2, YY, X, YY == X);
                ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                ASSERTV(LINE1, LINE2, &oa == Y.get_allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mX, mY);

                ASSERTV(2.0f == X.max_load_factor());
                ASSERTV(3.0f == Y.max_load_factor());

                ASSERTV(LINE1, LINE2, XX, X, XX == X);
                ASSERTV(LINE1, LINE2, YY, Y, YY == Y);
                ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                ASSERTV(LINE1, LINE2, &oa == Y.get_allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }

            // Unlike ordered containers, unordered do not support swapping
            // with unequal bslma allocators.
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

        mX.max_load_factor(2.0);
        mY.max_load_factor(3.0);

        if (veryVerbose) { T_ P_(X) P(Y) }

        bslma::TestAllocatorMonitor oam(&oa);

        // We know that the types of 'mX' and 'mY' do not overload the unary
        // address-of 'operator&'.

        bslalg::SwapUtil::swap(&mX, &mY);

        ASSERTV(YY, X, YY == X);
        ASSERTV(XX, Y, XX == Y);
        ASSERTV(oam.isTotalSame());

        ASSERTV(3.0 == X.max_load_factor());
        ASSERTV(2.0 == Y.max_load_factor());

        if (veryVerbose) { T_ P_(X) P(Y) }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
          bool OTHER_FLAGS>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::
                     testCase7_select_on_container_copy_construction_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                    KEY,
                                    SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
                                    OTHER_FLAGS,
                                    OTHER_FLAGS,
                                    OTHER_FLAGS>                      StdAlloc;

    typedef bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, StdAlloc> Obj;

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

        {
            const Obj W(VALUES.begin(), VALUES.end(), 1, HASH(), EQUAL(), ma);
                                                                     // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            VALUES.resetIterators();

            Obj mX(VALUES.begin(), VALUES.end(), 1, HASH(), EQUAL(), ma);
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

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::
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

    typedef StatefulStlAllocator<KEY>                                Allocator;
    typedef bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, Allocator> Obj;

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

            const Obj W(VALUES.begin(), VALUES.end(), 1, HASH(), EQUAL(), a);
                                                                     // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            VALUES.resetIterators();

            Obj mX(VALUES.begin(), VALUES.end(), 1, HASH(), EQUAL(), a);
            const Obj& X = mX;

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            const Obj Y(X);

            ASSERTV(SPEC,        W == Y);
            ASSERTV(SPEC,        W == X);
            ASSERTV(SPEC, ALLOC_ID == Y.get_allocator().id());
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR
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
    //   unordered_multimap(const unordered_multimap& orig);
    //   unordered_multimap(const unordered_multimap& orig, const A& alloc);
    // ------------------------------------------------------------------------

    typedef bsl::allocator_traits<ALLOC> Traits;

    bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
    bslma::TestAllocator da("default", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    ASSERT(&da == Traits::select_on_container_copy_construction(&oa));

    const TestValues VALUES;

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\nFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            // Create control object 'W'.
            Obj mW; const Obj& W = gg(&mW, SPEC);

            ASSERTV(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            {   // Testing concern 1..4.

                if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                const Obj Y0(X);

                ASSERTV(SPEC, X == Y0);
                ASSERTV(SPEC, W == Y0);
                ASSERTV(SPEC, Y0.get_allocator() == &da);
            }
            {   // Testing concern 5.

                if (veryVerbose) printf("\t\t\tInsert into created obj, "
                                        "without test allocator:\n");

                Obj Y1(X);

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Insert: "); P(Y1);
                }

                Y1.insert(VALUES['Z' - 'A']);

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Insert : ");
                    P(Y1);
                }

                ASSERTV(SPEC, Y1.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y1);
                ASSERTV(SPEC, X != Y1);
            }
            {   // Testing concern 5 with test allocator.
                if (veryVerbose) {
                    printf("\t\t\tInsert into created obj, "
                           "with test allocator:\n");
                }

                const bsls::Types::Int64 A = oa.numBlocksTotal();

                Obj Y11(X, &oa);

                ASSERT(0 == LENGTH || oa.numBlocksTotal() > A);

                // Due of pooling of memory allocation, we can't predict
                // whether this insert will allocate or not.

                Y11.insert(VALUES['Z' - 'A']);

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Append : ");
                    P(Y11);
                }

                ASSERTV(SPEC, Y11.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y11);
                ASSERTV(SPEC, X != Y11);
                ASSERTV(SPEC, Y11.get_allocator() == X.get_allocator());
            }
            {   // Exception checking.

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
            }
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase6_perturbValue(
                                 Obj *pmX, Obj *pmY, bsl::true_type isMoveOnly)
{
    // 'VALUE' is move-only, therefore no copy-assignment operator or copy
    // c'tor.  We do, however, have the 'setData' manipulator.

    ASSERT(isMoveOnly);
    BSLMF_ASSERT((bsl::is_same<VALUE, bsltf::MoveOnlyAllocTestType>::value ||
                  bsl::is_same<VALUE,
                             bsltf::WellBehavedMoveOnlyAllocTestType>::value));

    Obj& mX = *pmX; const Obj& X = mX;
    Obj& mY = *pmY; const Obj& Y = mY;

    unsigned count = 0;
    const Iter end = mX.end();
    for (Iter it = mX.begin(); end != it; ++it, ++count) {
        int id = it->second.data();
        it->second.setData(id + 1);
        ASSERTV(!(X == Y));
        ASSERTV(!(Y == X));
        ASSERTV(  X != Y);
        ASSERTV(  Y != X);
        it->second.setData(id);
        ASSERTV(  X == Y);
        ASSERTV(  Y == X);
        ASSERTV(!(X != Y));
        ASSERTV(!(Y != X));
    }

    ASSERT(X.size() == count);
}
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase6_perturbValue(
                                Obj *pmX, Obj *pmY, bsl::false_type isMoveOnly)
{
    ASSERT(!isMoveOnly);

    Obj& mX = *pmX; const Obj& X = mX;
    Obj& mY = *pmY; const Obj& Y = mY;

    unsigned count = 0;
    const Iter end = mX.end();
    for (Iter it = mX.begin(); end != it; ++it, ++count) {
        VALUE v = it->second;
        int id = TstFacility::getIdentifier(it->second);
        ++id;
        it->second = TstFacility::create<VALUE>(id);
        ASSERTV(!(X == Y));
        ASSERTV(!(Y == X));
        ASSERTV(  X != Y);
        ASSERTV(  Y != X);
        it->second = v;
        ASSERTV(  X == Y);
        ASSERTV(  Y == X);
        ASSERTV(!(X != Y));
        ASSERTV(!(Y != X));
    }

    ASSERT(X.size() == count);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase6()
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
    //:       4 Disable the comparator so that it will cause an error if it's
    //:         used.
    //:
    //:       5 Verify the commutativity property and expected return value for
    //:         both '==' and '!=', while monitoring both 'oax' and 'oay' to
    //:         ensure that no object memory is ever allocated by either
    //:         operator.  (C-1..2, 5..7, 10)
    //:
    //:       6 Verify that modifying the 'second' fields of the pair affects
    //:         the result -- when X == Y, go through the elements of X,
    //:         perterbing the 'second' field of every element, and verify
    //:         that even this small change causing an inequality result.
    //:
    //: 5 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-11)
    //
    // Testing:
    //   bool operator==(const unordered_multimap& lhs,
    //                   const unordered_multimap& rhs);
    //   bool operator!=(const unordered_multimap& lhs,
    //                   const unordered_multimap& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("EQUALITY-COMPARISON OPERATORS\n"
                            "=============================\n");

    if (veryVerbose)
              printf("\nTesting signatures.\n");
    {
        typedef bool (*OP)(const Obj&, const Obj&);

        OP op = bsl::operator==;
#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
        (void) [](const Obj& lhs, const Obj& rhs) -> bool {
            return lhs != rhs;
        };
#else
        op    = bsl::operator!=;
#endif
        (void) op;
    }

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const TestValues VALUES;

    if (verbose) printf("\nCompare every value with every value.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char *const SPEC1   = DATA[ti].d_spec;
            const int         INDEX1  = DATA[ti].d_index;
            const size_t      LENGTH1 = strlen(SPEC1);

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
                const int         INDEX2  = DATA[tj].d_index;
                const size_t      LENGTH2 = strlen(SPEC2);

                if (veryVerbose) { T_ T_ P_(LINE2) P_(LENGTH2) P(SPEC2) }

                const bool EXP = (INDEX1 == INDEX2);  // expected result

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

                    bsls::Types::Int64 numX = xa.numBlocksTotal();
                    bsls::Types::Int64 numY = ya.numBlocksTotal();

                    // EQUAL::disableFunctor();
                            // TBD -- fails this test EQUAL is used to
                            // determine the equality groups, then
                            // bsl::permutation is used to determine if they're
                            // isomorphic, and bsl::permutation uses
                            // 'operator=='.  It will take a lot of work to
                            // verify that this is the case, putting it off for
                            // later.

                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (Y != X));

                    ASSERTV(xa.numBlocksTotal() == numX);
                    ASSERTV(ya.numBlocksTotal() == numY);

                    if (EXP) {
                        testCase6_perturbValue(
                                 &mX,
                                 &mY,
                                 IsValueMoveOnly());
                    }

                    const size_t NUM_BUCKETS = Y.bucket_count();
                    mY.reserve((Y.size() + 1) * 5);
                    ASSERTV(NUM_BUCKETS < Y.bucket_count());
                    ASSERTV(!EXP || X.bucket_count() != Y.bucket_count());

                    numX = xa.numBlocksTotal();
                    numY = ya.numBlocksTotal();

                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (Y != X));

                    ASSERTV(xa.numBlocksTotal() == numX);
                    ASSERTV(ya.numBlocksTotal() == numY);

                    if (EXP) {
                        testCase6_perturbValue(
                                 &mX,
                                 &mY,
                                 IsValueMoveOnly());
                    }
                }
            }
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    //   Ensure each basic accessor:
    //     - cbegin
    //     - cend
    //     - size
    //     - get_allocator
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
    //: 4 The range '[cbegin(), cend())' contains inserted elements, but not
    //:   necessarily in any give order.
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
    //   iterator begin();
    //   iterator end();
    //   const_iterator begin() const;
    //   const_iterator end() const;
    //   const_iterator cbegin() const;
    //   const_iterator cend() const;
    //   size_type size() const;
    //   allocator_type get_allocator() const;
    // ------------------------------------------------------------------------
    if (verbose) printf("\nBASIC ACCESSORS"
                        "\n===============\n");

    BSLMF_ASSERT((! bslmf::IsSame<Iter, CIter>::value));

    if (verbose) printf("Testing Signatures\n");
    {
        typedef bool (Obj::*MP)() const;
        MP mp = &Obj::empty;
        (void) mp;
    }

    {
        typedef size_t (Obj::*MP)() const;
        MP mp = &Obj::size;
        (void) mp;
        mp    = &Obj::max_size;
        (void) mp;
    }

    {
        typedef Iter (Obj::*MP)();
        MP mp = &Obj::begin;
        (void) mp;
        mp    = &Obj::end;
        (void) mp;
    }

    {
        typedef CIter (Obj::*MP)() const;
        MP mp = &Obj::begin;
        (void) mp;
        mp    = &Obj::end;
        (void) mp;
        mp    = &Obj::cbegin;
        (void) mp;
        mp    = &Obj::cend;
        (void) mp;
    }

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const HASH  defaultHasher     = HASH();
    const EQUAL defaultComparator = EQUAL();

    if (verbose) { printf(
            "\nCreate objects with various allocator configurations.\n"); }

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE     = DATA[ti].d_line;
        const char *const SPEC     = DATA[ti].d_spec;
        const char *const EXP_SPEC = DATA[ti].d_results;
        const size_t      LENGTH   = strlen(EXP_SPEC);

        const TestValues  EXP(EXP_SPEC);

        if (veryVerbose) { P_(LINE) P_(LENGTH) P(SPEC); }

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
                objPtr = new (fa) Obj(0,
                                      defaultHasher,
                                      defaultComparator);
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objPtr = new (fa) Obj(0,
                                      defaultHasher,
                                      defaultComparator,
                                      (bslma::Allocator *)0);
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objPtr = new (fa) Obj(0,
                                      defaultHasher,
                                      defaultComparator,
                                      &sa1);
                objAllocatorPtr = &sa1;
              } break;
              case 'd': {
                objPtr = new (fa) Obj(0,
                                      defaultHasher,
                                      defaultComparator,
                                      &sa2);
                objAllocatorPtr = &sa2;
              } break;
              default: {
                ASSERTV(CONFIG, !"Bad allocator config.");
              } return;                                               // RETURN
            }

            Obj& mX = *objPtr; const Obj& X = gg(&mX, SPEC);
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = ('c' == CONFIG || 'd' == CONFIG)
                ? da
                : sa1;

            // Verify basic accessors

            bslma::TestAllocatorMonitor oam(&oa);

            ASSERTV(LINE, SPEC, CONFIG, &oa == X.get_allocator());
            ASSERTV(LINE, SPEC, CONFIG, LENGTH == X.size());
            ASSERTV((0 == LENGTH) == X.empty());

            ASSERTV(verifySpec(X, EXP_SPEC));

            const Iter begin = mX.begin();
            CIter cBegin = X.begin();
            ASSERTV(begin == cBegin);
            cBegin = X.cbegin();
            ASSERTV(begin == cBegin);
            const Iter end = mX.end();
            CIter cend = X.end();
            ASSERTV(end == cend);
            cend = X.end();
            ASSERTV(end == cend);

            ASSERTV(oam.isTotalSame());

            ASSERTV(!LENGTH == (begin == end));

            {
                size_t count = 0;
                for (Iter it = begin; end != it; ++it) {
                    ++count;
                }
                ASSERTV(LENGTH == count);
            }

            ASSERTV(LENGTH != 0 || 1 == X.bucket_count());

            // --------------------------------------------------------
            // Verify no allocation from the non-object allocator (before
            // deletion -- in SAFE2 mode, the d'tor calls 'isWellFormed',
            // which uses the default allocator.

            ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                    0 == noa.numBlocksTotal());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

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

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase3()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMITIVE GENERATOR FUNCTIONS gg AND ggg
    //   Demonstrated that our primary manipulators work as expected under
    //   normal conditions.
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
    //   multimap& gg(multimap *object, const char *spec);
    //   int ggg(multimap *object, const char *spec, int verbose = 1);
    //   bool verifySpec(const unordered_multimap& object, const char* spec);
    // ------------------------------------------------------------------------

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    HASH  h( 7);
    EQUAL eq(9);

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         INDEX1  = DATA[ti].d_index;
        const char *const SPEC1   = DATA[ti].d_spec;
        const char *const RESULT1 = DATA[ti].d_results;
        const size_t      LENGTH1 = strlen(RESULT1);

        bslma::TestAllocator da("default",  veryVeryVeryVerbose);
        bslma::TestAllocator sc("scratch",  veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        ASSERTV(0 == sa.numBlocksInUse());

        {
            Obj mX(&sa); const Obj& X = mX;

            ASSERTV(-1 == ggg(&mX, SPEC1));

            ASSERTV(0         ==   da.numBlocksInUse());
            ASSERTV(!!LENGTH1 == !!sa.numBlocksInUse());

            ASSERTV(LENGTH1   == X.size());

            ASSERTV(verifySpec(X, RESULT1));

            // Matches all specs in the same group and does not match any specs
            // from other groups.
            for (size_t tj = 0; tj < NUM_DATA; ++tj) {
                const char *const SPEC2   = DATA[tj].d_spec;
                const int         INDEX2  = DATA[tj].d_index;
                const bool        MATCH   = (INDEX1 == INDEX2);

                ASSERTV(MATCH, SPEC1, SPEC2, X.size(),
                        MATCH == verifySpec(X, SPEC2));
            }

            mX.clear();

            ASSERTV(0 == X.size());

            const Obj& XX = gg(&mX, SPEC1);

            ASSERTV(&XX == &X);

            ASSERTV(0        ==   da.numBlocksInUse());
            ASSERTV(!!LENGTH1 == !!sa.numBlocksInUse());

            ASSERTV(LENGTH1  == X.size());

            ASSERTV(verifySpec(X, SPEC1));
        }
    }

    struct {
        int d_line;
        const char *d_spec;
        const char *d_goodSpec;
    } BAD_SPECS[] = {
        { L_, " ",    ""    },
        { L_, "+",    ""    },
        { L_, "ABCa", "ABC" },
        { L_, "ABC+", "ABC" },
        { L_, "A+BC", "ABC" },
        { L_, "A,+C", "AC"  },
        { L_, "A1BC", "ABC" },
        { L_, "1234", ""    }
    };
    enum { NUM_BAD_SPECS = sizeof BAD_SPECS / sizeof *BAD_SPECS };

    bslma::TestAllocator da("default",  veryVeryVeryVerbose);
    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    for (size_t ti = 0; ti < NUM_BAD_SPECS; ++ti) {
        const char *SPEC      = BAD_SPECS[ti].d_spec;
        const char *GOOD_SPEC = BAD_SPECS[ti].d_goodSpec;

        {
            Obj mX(&sa); const Obj& X = mX;

            ASSERTV(-1 != ggg(&mX, SPEC, 0));
            ASSERTV(!verifySpec(X, SPEC));
        }

        {
            Obj mX(&sa); const Obj& X = mX;
            gg(&mX, GOOD_SPEC);

            ASSERTV( verifySpec(X, GOOD_SPEC));
            ASSERTV(!verifySpec(X, SPEC));
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase2()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
    //   The basic concern is that the default constructor, the destructor,
    //   and, under normal conditions (i.e., no aliasing), the primary
    //   manipulators
    //      - insert (via helper function 'primaryManipulator')
    //      - clear
    //   can be used to create an object and put that object into a state
    //   relevant for testing.
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
    // Plan:
    //: 1 For each value of increasing length, 'L':
    //:
    //:   2 Using a loop-based approach, default-construct three distinct
    //:     objects, in turn, but configured differently: (a) without passing
    //:     an allocator, (b) passing a null allocator address explicitly, and
    //:     (c) passing the address of a test allocator distinct from the
    //:     default.  For each of these three iterations: (C-1..14)
    //:
    //:     1 Create three 'bslma::TestAllocator' objects, and install one as
    //:       as the current default allocator (note that a ubiquitous test
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
    //:     7 Insert the 'L'th value in the presense of exception and use the
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
    //   default construction (only)
    //   unordered_multimap(const allocator_type&);  // bslma::Allocator* only
    //   ~unordered_multimap();
    //   iterator insert(value_type&& value);
    //   void clear();
    // ------------------------------------------------------------------------

    const TestValues VALUES;  // contains 52 distinct increasing values

    const size_t MAX_LENGTH = 9;

    const HASH  defaultHasher     = HASH(7);
    const EQUAL defaultComparator = EQUAL(9);

    for (size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const size_t LENGTH = ti;

        if (verbose) {
            printf("\nTesting with various allocator configurations.\n");
        }

        bool done = false;
        for (char cfg = 'a'; cfg <= 't'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting default constructor.\n");
            }

            Obj *objPtr = 0;

            bslma::TestAllocator&  oa  = strchr("cgikprt", CONFIG) ? sa : da;
            bslma::TestAllocator&  noa = &sa == &oa ? da : sa;

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj;
                  } break;
                  case 'b': {
                      // Ambiguous -- does '0' mean 'initialNumBuckets' or
                      // 'allocator'? -- Result is same either way.
                      // Note: resolves to 'initialNumBuckets' as it does not
                      // require any conversion.
                      objPtr = new (fa) Obj(0);
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(&sa);
                  } break;
                  case 'd': {
                      objPtr = new (fa) Obj(0,
                                            defaultHasher);
                  } break;
                  case 'e': {
                      objPtr = new (fa) Obj(0,
                                            defaultHasher,
                                            defaultComparator);
                  } break;
                  case 'f': {
                      objPtr = new (fa) Obj(0,
                                            defaultHasher,
                                            defaultComparator,
                                            (bslma::Allocator *)0);
                  } break;
                  case 'g': {
                      objPtr = new (fa) Obj(0,
                                            defaultHasher,
                                            defaultComparator,
                                            &sa);
                  } break;
                  case 'h': {
                      objPtr = new (fa) Obj(0,
                                            (bslma::Allocator *)0);
                  } break;
                  case 'i': {
                      objPtr = new (fa) Obj(0,
                                            &sa);
                  } break;
                  case 'j': {
                      objPtr = new (fa) Obj(0,
                                            defaultHasher,
                                            (bslma::Allocator *)0);
                  } break;
                  case 'k': {
                      objPtr = new (fa) Obj(0,
                                            defaultHasher,
                                            &sa);
                  } break;
                  case 'l': {
                      objPtr = new (fa) Obj(100);
                  } break;
                  case 'm': {
                      objPtr = new (fa) Obj(100,
                                            defaultHasher);
                  } break;
                  case 'n': {
                      objPtr = new (fa) Obj(100,
                                            defaultHasher,
                                            defaultComparator);
                  } break;
                  case 'o': {
                      objPtr = new (fa) Obj(100,
                                            defaultHasher,
                                            defaultComparator,
                                            (bslma::Allocator *)0);
                  } break;
                  case 'p': {
                      objPtr = new (fa) Obj(100,
                                            defaultHasher,
                                            defaultComparator,
                                            &sa);
                  } break;
                  case 'q': {
                      objPtr = new (fa) Obj(100,
                                            (bslma::Allocator *)0);
                  } break;
                  case 'r': {
                      objPtr = new (fa) Obj(100,
                                            &sa);
                  } break;
                  case 's': {
                      objPtr = new (fa) Obj(100,
                                            defaultHasher,
                                            (bslma::Allocator *)0);
                  } break;
                  case 't': {
                      objPtr = new (fa) Obj(100,
                                            defaultHasher,
                                            &sa);
                      done = true;
                  } break;
                  default: {
                      ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV((!PLAT_EXC || CONFIG <= 'k') == (1 == numPasses));

            Obj& mX = *objPtr; const Obj& X = mX;

            // Verify any attribute allocators are installed properly.

            ASSERTV(LENGTH, CONFIG, &oa == X.get_allocator());

            if (CONFIG <= 'k') {
                // Verify no allocation from the object/non-object allocators.
                // NOTE THAT THIS QoI TEST IS STILL AN OPEN DESIGN ISSUE

                ASSERTV(LENGTH, CONFIG, oa.numBlocksTotal(),
                        0 ==  oa.numBlocksTotal());
                ASSERTV(LENGTH, CONFIG, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());
                ASSERTV(LENGTH, CONFIG, X.bucket_count(),
                        1 == X.bucket_count());
            }
            else {
                ASSERTV(LENGTH, CONFIG, X.bucket_count(),
                        100 <= X.bucket_count());
            }

            ASSERTV(LENGTH, CONFIG, 0 == X.size());
            ASSERTV(LENGTH, CONFIG, X.cbegin() == X.cend());

            ASSERTV((strchr("abchilqr",       CONFIG) ? HASH()
                                              : HASH(7)) == X.hash_function());
            ASSERTV((strchr("abcdhijklmqrst", CONFIG) ? EQUAL()
                                              : EQUAL(9)) == X.key_eq());

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\n\tTesting 'insert' (bootstrap).\n"); }

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            if (0 < LENGTH) {
                if (verbose) printf(
                       "\t\tOn an object of initial length " ZU ".\n", LENGTH);

                for (size_t tj = 0; tj < LENGTH - 1; ++tj) {
                    if (veryVeryVeryVerbose) {
                       printf("\t\t\t\t Inserting: ");
                       P(VALUES[tj]);
                    }
                    int id = TstFacility::getIdentifier(VALUES[tj].first);
                    Iter RESULT = primaryManipulator(&mX, id, &scratch);
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == *RESULT);
                }

                ASSERTV(LENGTH, CONFIG, LENGTH - 1 == X.size());
                if (veryVerbose) {
                    printf("\t\t\tBEFORE: ");
                    P(X);
                }

                if (veryVerbose) {
                    printf("\t\t Testing allocator exceptions\n");
                }
                //bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                if (&oa == &da) {
                    int id =
                          TstFacility::getIdentifier(VALUES[LENGTH - 1].first);
                    primaryManipulator(&mX, id, &scratch);
                }
                else {
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        Obj mZ(&scratch); const Obj& Z = mZ;

                        for (size_t tj = 0; tj < LENGTH - 1; ++tj) {
                            int id =
                                  TstFacility::getIdentifier(VALUES[tj].first);
                            primaryManipulator(&mZ, id, &scratch);
                        }
                        ASSERTV(Z, X, Z == X);

                        ExceptionProctor<Obj, ALLOC> guard(&X, L_,
                                                           MoveUtil::move(mZ));

                        if (veryVeryVeryVerbose) {
                            printf("\t\t\t\t Inserting: ");
                            P(VALUES[LENGTH - 1]);
                        }

                        int id =
                           TstFacility::getIdentifier(VALUES[LENGTH -1].first);
                        Iter RESULT = primaryManipulator(&mX, id, &scratch);

                        ASSERTV(LENGTH, CONFIG, VALUES[LENGTH - 1] == *RESULT);

                        guard.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                ASSERTV(LENGTH, CONFIG, LENGTH, X.size(), LENGTH == X.size());

                {
                    bool *foundValues = new bool[X.size()];
                    for (size_t j = 0; j != X.size(); ++j) {
                        foundValues[j] = false;
                    }

                    size_t i = 0;
                    for (CIter it = X.cbegin(); it != X.cend(); ++it, ++i) {
                        size_t j = 0;
                        do {
                            if (VALUES[j] == *it) {
                                ASSERTV(LENGTH, CONFIG, VALUES[j],
                                        !foundValues[j]);
                                foundValues[j] = true;
                            }
                        }
                        while (++j != X.size());
                    }
                    size_t missing = 0;
                    for (size_t j = 0; j != X.size(); ++j) {
                        if (!foundValues[j]) { ++missing; }
                    }
                    ASSERTV(LENGTH, CONFIG, missing, 0 == missing);

                    delete[] foundValues;

                    ASSERTV(LENGTH, CONFIG, X.size() == i);
                }

                // Verify behavior when element already exist in the object

                if (veryVerbose) {
                    printf("\t\t Verifying already inserted values\n");
                }
                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    if (veryVeryVeryVerbose) {
                       printf("\t\t\t\t Inserting: ");
                       P(VALUES[tj]);
                    }
                    int id = TstFacility::getIdentifier(VALUES[tj].first);
                    Iter RESULT = primaryManipulator(&mX, id, &scratch);
                    ASSERTV(LENGTH, tj, CONFIG, *RESULT, VALUES[tj] ==
                                                                      *RESULT);
                }
            }

            // ----------------------------------------------------------------

            if (veryVerbose) printf("\n\tTesting 'clear'.\n");
            {
                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                if (veryVeryVeryVerbose) printf("mX.clear();\n");
                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;

                    mX.clear();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                // No allocations done by 'clear'
                ASSERTV(1 == numPasses);

                ASSERTV(LENGTH, CONFIG, 0 == X.size());
                ASSERTV(LENGTH, CONFIG, X.cbegin() == X.cend());

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LENGTH, CONFIG, BB == AA);
                // 2x time due to inserting full set of duplicated values
                ASSERTV(LENGTH, CONFIG, B, A,
                        0 == B - LENGTH * TYPE_ALLOC * 2 - A);

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    int id = TstFacility::getIdentifier(VALUES[tj].first);
                    Iter RESULT = primaryManipulator(&mX, id, &scratch);
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == *RESULT);
                }

                ASSERTV(LENGTH, CONFIG, LENGTH == X.size());
            }

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
        ASSERTV(done);
    }

    {
        const size_t initialNumBuckets[] = { 0, 1, 2, 3, 5, 7, 11, 13, 17, 19,
                                    23, 29, 31, 37, 41, 43, 47, 51, 100, 200 };
        const int NUM_INITIAL_NUM_BUCKETS = static_cast<int>
                       (sizeof initialNumBuckets / sizeof *initialNumBuckets);

        for (int ti = 0; ti < NUM_INITIAL_NUM_BUCKETS; ++ti) {
            {
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                {
                    Obj mX(initialNumBuckets[ti]);  const Obj& X = mX;
                    ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);
                }
                {
                    Obj mX(initialNumBuckets[ti],
                           defaultHasher);
                    const Obj& X = mX;
                    ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);
                }
                {
                    Obj mX(initialNumBuckets[ti],
                           defaultHasher,
                           defaultComparator);
                    const Obj& X = mX;
                    ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);
                }
                {
                    Obj mX(initialNumBuckets[ti],
                           defaultHasher,
                           defaultComparator,
                           (bslma::Allocator *) 0);
                    const Obj& X = mX;
                    ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);
                }

                ASSERTV(ti, da.numBlocksTotal(),
                                       (ti > 0) || (0 == da.numBlocksTotal()));
            }
            {
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                Obj mX(initialNumBuckets[ti],
                       defaultHasher,
                       defaultComparator,
                       &sa);
                const Obj& X = mX;
                ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);

                ASSERTV(0 == da.numBlocksTotal());
                ASSERTV(ti, sa.numBlocksTotal(),
                                       (ti > 0) || (0 == sa.numBlocksTotal()));
            }
        }
    }
}

//=============================================================================
//                                  USAGE
//-----------------------------------------------------------------------------

void usage()
{
#ifndef DONT_DO_USAGE
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Creating a Concordance
///- - - - - - - - - - - - - - - - -
// Unordered multimaps are useful in situations when there is no meaningful
// way to compare key values, when the order of the keys is irrelevant to the
// problem domain, or (even if there is a meaningful ordering) the benefit of
// ordering the results is outweighed by the higher performance provided by
// unordered multimaps (compared to ordered multimaps).
//
// One uses a multimap (ordered or unordered) when there may be more than one
// mapped value associated with a key value.  In this example we will use
// 'bslstl_unorderedmultimap' to create a concordance (an index of where each
// unique word appears in the set of documents).
//
// Our source of documents is a set of statically initialized arrrays:
//..
    static char document0[] =
    " IN CONGRESS, July 4, 1776.\n"
    "\n"
    " The unanimous Declaration of the thirteen united States of America,\n"
    "\n"
    " When in the Course of human events, it becomes necessary for one\n"
    " people to dissolve the political bands which have connected them with\n"
    " another, and to assume among the powers of the earth, the separate\n"
    " and equal station to which the Laws of Nature and of Nature's God\n"
    " entitle them, a decent respect to the opinions of mankind requires\n"
    " that they should declare the causes which impel them to the\n"
    " separation.  We hold these truths to be self-evident, that all men\n"
    " are created equal, that they are endowed by their Creator with\n"
    " certain unalienable Rights, that among these are Life, Liberty and\n"
    " the pursuit of Happiness.--That to secure these rights, Governments\n"
    " are instituted among Men, deriving their just powers from the consent\n"
    " of the governed, --That whenever any Form of Government becomes\n"
    " destructive of these ends, it is the Right of the People to alter or\n"
    " to abolish it, and to institute new Government, laying its foundation\n"
    " on such principles and organizing its powers in such form, as to them\n"
    " shall seem most likely to effect their Safety and Happiness.\n"
    " Prudence, indeed, will dictate that Governments long established\n"
    " should not be changed for light and transient causes; and accordingly\n"
    " all experience hath shewn, that mankind are more disposed to suffer,\n"
    " while evils are sufferable, than to right themselves by abolishing\n"
    " the forms to which they are accustomed.  But when a long train of\n"
    " abuses and usurpations, pursuing invariably the same Object evinces a\n"
    " design to reduce them under absolute Despotism, it is their right, it\n"
    " is their duty, to throw off such Government, and to provide new\n"
    " Guards for their future security.--Such has been the patient\n"
    " sufferance of these Colonies; and such is now the necessity which\n"
    " constrains them to alter their former Systems of Government.  The\n"
    " history of the present King of Great Britain is a history of repeated\n"
    " injuries and usurpations, all having in direct object the\n"
    " establishment of an absolute Tyranny over these States.  To prove\n"
    " this, let Facts be submitted to a candid world.\n"
    "\n"
    ": o He has refused his Assent to Laws, the most wholesome and\n"
    ":   necessary for the public good.\n"
    ":\n"
    ": o He has forbidden his Governors to pass Laws of immediate and\n"
    ":   pressing importance, unless suspended in their operation till his\n"
    ":   Assent should be obtained; and when so suspended, he has utterly\n"
    ":   neglected to attend to them.\n"
    ":\n"
    ": o He has refused to pass other Laws for the accommodation of large\n"
    ":   districts of people, unless those people would relinquish the\n"
    ":   right of Representation in the Legislature, a right inestimable to\n"
    ":   them and formidable to tyrants only.\n"
    ":\n"
    ": o He has called together legislative bodies at places unusual,\n"
    ":   uncomfortable, and distant from the depository of their public\n"
    ":   Records, for the sole purpose of fatiguing them into compliance\n"
    ":   with his measures.\n"
    ":\n"
    ": o He has dissolved Representative Houses repeatedly, for opposing\n"
    ":   with manly firmness his invasions on the rights of the people.\n"
    ":\n"
    ": o He has refused for a long time, after such dissolutions, to cause\n"
    ":   others to be elected; whereby the Legislative powers, incapable of\n"
    ":   Annihilation, have returned to the People at large for their\n"
    ":   exercise; the State remaining in the mean time exposed to all the\n"
    ":   dangers of invasion from without, and convulsions within.\n"
    ":\n"
    ": o He has endeavoured to prevent the population of these States; for\n"
    ":   that purpose obstructing the Laws for Naturalization of\n"
    ":   Foreigners; refusing to pass others to encourage their migrations\n"
    ":   hither, and raising the conditions of new Appropriations of Lands.\n"
    ":\n"
    ": o He has obstructed the Administration of Justice, by refusing his\n"
    ":   Assent to Laws for establishing Judiciary powers.\n"
    ":\n"
    ": o He has made Judges dependent on his Will alone, for the tenure of\n"
    ":   their offices, and the amount and payment of their salaries.\n"
    ":\n"
    ": o He has erected a multitude of New Offices, and sent hither swarms\n"
    ":   of Officers to harrass our people, and eat out their substance.\n"
    ":\n"
    ": o He has kept among us, in times of peace, Standing Armies without\n"
    ":   the Consent of our legislatures.\n"
    ":\n"
    ": o He has affected to render the Military independent of and superior\n"
    ":   to the Civil power.\n"
    ":\n"
    ": o He has combined with others to subject us to a jurisdiction\n"
    ":   foreign to our constitution, and unacknowledged by our laws;\n"
    ":   giving his Assent to their Acts of pretended Legislation:\n"
    ":\n"
    ": o For Quartering large bodies of armed troops among us: For\n"
    ":   protecting them, by a mock Trial, from punishment for any Murders\n"
    ":   which they should commit on the Inhabitants of these States:\n"
    ":\n"
    ": o For cutting off our Trade with all parts of the world:\n"
    ":\n"
    ": o For imposing Taxes on us without our Consent: For depriving us in\n"
    ":   many cases, of the benefits of Trial by Jury:\n"
    ":\n"
    ": o For transporting us beyond Seas to be tried for pretended offences\n"
    ":\n"
    ": o For abolishing the free System of English Laws in a neighbouring\n"
    ":   Province, establishing therein an Arbitrary government, and\n"
    ":   enlarging its Boundaries so as to render it at once an example and\n"
    ":   fit instrument for introducing the same absolute rule into these\n"
    ":   Colonies:\n"
    ":\n"
    ": o For taking away our Charters, abolishing our most valuable Laws,\n"
    ":   and altering fundamentally the Forms of our Governments:\n"
    ":\n"
    ": o For suspending our own Legislatures, and declaring themselves\n"
    ":   invested with power to legislate for us in all cases whatsoever.\n"
    ":\n"
    ": o He has abdicated Government here, by declaring us out of his\n"
    ":   Protection and waging War against us.\n"
    ":\n"
    ": o He has plundered our seas, ravaged our Coasts, burnt our towns,\n"
    ":   and destroyed the lives of our people.  He is at this time\n"
    ":   transporting large Armies of foreign Mercenaries to compleat the\n"
    ":   works of death, desolation and tyranny, already begun with\n"
    ":   circumstances of Cruelty & perfidy scarcely paralleled in the most\n"
    ":   barbarous ages, and totally unworthy the Head of a civilized\n"
    ":   nation.\n"
    ":\n"
    ": o He has constrained our fellow Citizens taken Captive on the high\n"
    ":   Seas to bear Arms against their Country, to become the\n"
    ":   executioners of their friends and Brethren, or to fall themselves\n"
    ":   by their Hands.\n"
    ":\n"
    ": o He has excited domestic insurrections amongst us, and has\n"
    ":   endeavoured to bring on the inhabitants of our frontiers, the\n"
    ":   merciless Indian Savages, whose known rule of warfare, is an\n"
    ":   undistinguished destruction of all ages, sexes and conditions.\n"
    "\n"
    " In every stage of these Oppressions We have Petitioned for Redress in\n"
    " the most humble terms: Our repeated Petitions have been answered only\n"
    " by repeated injury.  A Prince whose character is thus marked by every\n"
    " act which may define a Tyrant, is unfit to be the ruler of a free\n"
    " people.\n"
    "\n"
    " Nor have We been wanting in attentions to our Brittish brethren.  We\n"
    " have warned them from time to time of attempts by their legislature\n"
    " to extend an unwarrantable jurisdiction over us.  We have reminded\n"
    " them of the circumstances of our emigration and settlement here.  We\n"
    " have appealed to their native justice and magnanimity, and we have\n"
    " conjured them by the ties of our common kindred to disavow these\n"
    " usurpations, which, would inevitably interrupt our connections and\n"
    " correspondence.  They too have been deaf to the voice of justice and\n"
    " of consanguinity.  We must, therefore, acquiesce in the necessity,\n"
    " which denounces our Separation, and hold them, as we hold the rest of\n"
    " mankind, Enemies in War, in Peace Friends.\n"
    "\n"
    " We, therefore, the Representatives of the united States of America,\n"
    " in General Congress, Assembled, appealing to the Supreme Judge of the\n"
    " world for the rectitude of our intentions, do, in the Name, and by\n"
    " Authority of the good People of these Colonies, solemnly publish and\n"
    " declare, That these United Colonies are, and of Right ought to be\n"
    " Free and Independent States; that they are Absolved from all\n"
    " Allegiance to the British Crown, and that all political connection\n"
    " between them and the State of Great Britain, is and ought to be\n"
    " totally dissolved; and that as Free and Independent States, they have\n"
    " full Power to levy War, conclude Peace, contract Alliances, establish\n"
    " Commerce, and to do all other Acts and Things which Independent\n"
    " States may of right do.  And for the support of this Declaration,\n"
    " with a firm reliance on the protection of divine Providence, we\n"
    " mutually pledge to each other our Lives, our Fortunes and our sacred\n"
    " Honor.\n";

    static char document1[] =
    "/The Universal Declaration of Human Rights\n"
    "/-----------------------------------------\n"
    "/Preamble\n"
    "/ - - - -\n"
    " Whereas recognition of the inherent dignity and of the equal and\n"
    " inalienable rights of all members of the human family is the\n"
    " foundation of freedom, justice and peace in the world,\n"
    "\n"
    " Whereas disregard and contempt for human rights have resulted in\n"
    " barbarous acts which have outraged the conscience of mankind, and the\n"
    " advent of a world in which human beings shall enjoy freedom of speech\n"
    " and belief and freedom from fear and want has been proclaimed as the\n"
    " highest aspiration of the common people,\n"
    "\n"
    " Whereas it is essential, if man is not to be compelled to have\n"
    " recourse, as a last resort, to rebellion against tyranny and\n"
    " oppression, that human rights should be protected by the rule of law,\n"
    "\n"
    " Whereas it is essential to promote the development of friendly\n"
    " relations between nations,\n"
    "\n"
    " Whereas the peoples of the United Nations have in the Charter\n"
    " reaffirmed their faith in fundamental human rights, in the dignity\n"
    " and worth of the human person and in the equal rights of men and\n"
    " women and have determined to promote social progress and better\n"
    " standards of life in larger freedom,\n"
    "\n"
    " Whereas Member States have pledged themselves to achieve, in\n"
    " co-operation with the United Nations, the promotion of universal\n"
    " respect for and observance of human rights and fundamental freedoms,\n"
    "\n"
    " Whereas a common understanding of these rights and freedoms is of the\n"
    " greatest importance for the full realization of this pledge, Now,\n"
    " Therefore THE GENERAL ASSEMBLY proclaims THIS UNIVERSAL DECLARATION\n"
    " OF HUMAN RIGHTS as a common standard of achievement for all peoples\n"
    " and all nations, to the end that every individual and every organ of\n"
    " society, keeping this Declaration constantly in mind, shall strive by\n"
    " teaching and education to promote respect for these rights and\n"
    " freedoms and by progressive measures, national and international, to\n"
    " secure their universal and effective recognition and observance, both\n"
    " among the peoples of Member States themselves and among the peoples\n"
    " of territories under their jurisdiction.\n"
    "\n"
    "/Article 1\n"
    "/- - - - -\n"
    " All human beings are born free and equal in dignity and rights.  They\n"
    " are endowed with reason and conscience and should act towards one\n"
    " another in a spirit of brotherhood.\n"
    "\n"
    "/Article 2\n"
    "/- - - - -\n"
    " Everyone is entitled to all the rights and freedoms set forth in this\n"
    " Declaration, without distinction of any kind, such as race, colour,\n"
    " sex, language, religion, political or other opinion, national or\n"
    " social origin, property, birth or other status.  Furthermore, no\n"
    " distinction shall be made on the basis of the political,\n"
    " jurisdictional or international status of the country or territory to\n"
    " which a person belongs, whether it be independent, trust,\n"
    " non-self-governing or under any other limitation of sovereignty.\n"
    "\n"
    "/Article 3\n"
    "/- - - - -\n"
    " Everyone has the right to life, liberty and security of person.\n"
    "\n"
    "/Article 4\n"
    "/- - - - -\n"
    " No one shall be held in slavery or servitude; slavery and the slave\n"
    " trade shall be prohibited in all their forms.\n"
    "\n"
    "/Article 5\n"
    "/- - - - -\n"
    " No one shall be subjected to torture or to cruel, inhuman or\n"
    " degrading treatment or punishment.\n"
    "\n"
    "/Article 6\n"
    "/- - - - -\n"
    " Everyone has the right to recognition everywhere as a person before\n"
    " the law.\n"
    "\n"
    "/Article 7\n"
    "/- - - - -\n"
    " All are equal before the law and are entitled without any\n"
    " discrimination to equal protection of the law.  All are entitled to\n"
    " equal protection against any discrimination in violation of this\n"
    " Declaration and against any incitement to such discrimination.\n"
    "\n"
    "/Article 8\n"
    "/- - - - -\n"
    " Everyone has the right to an effective remedy by the competent\n"
    " national tribunals for acts violating the fundamental rights granted\n"
    " him by the constitution or by law.\n"
    "\n"
    "/Article 9\n"
    "/- - - - -\n"
    " No one shall be subjected to arbitrary arrest, detention or exile.\n"
    "\n"
    "/Article 10\n"
    "/ - - - - -\n"
    " Everyone is entitled in full equality to a fair and public hearing by\n"
    " an independent and impartial tribunal, in the determination of his\n"
    " rights and obligations and of any criminal charge against him.\n"
    "\n"
    "/Article 11\n"
    "/ - - - - -\n"
    ": 1 Everyone charged with a penal offence has the right to be presumed\n"
    ":   innocent until proved guilty according to law in a public trial at\n"
    ":   which he has had all the guarantees necessary for his defence.\n"
    ":\n"
    ": 2 No one shall be held guilty of any penal offence on account of any\n"
    ":   act or omission which did not constitute a penal offence, under\n"
    ":   national or international law, at the time when it was committed.\n"
    ":   Nor shall a heavier penalty be imposed than the one that was\n"
    ":   applicable at the time the penal offence was committed.\n"
    "\n"
    "/Article 12\n"
    "/ - - - - -\n"
    " No one shall be subjected to arbitrary interference with his privacy,\n"
    " family, home or correspondence, nor to attacks upon his honour and\n"
    " reputation.  Everyone has the right to the protection of the law\n"
    " against such interference or attacks.\n"
    "\n"
    "/Article 13\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to freedom of movement and residence within\n"
    ":   the borders of each state.\n"
    ":\n"
    ": 2 Everyone has the right to leave any country, including his own,\n"
    ":   and to return to his country.\n"
    "\n"
    "/Article 14\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to seek and to enjoy in other countries\n"
    ":   asylum from persecution.\n"
    ":\n"
    ": 2 This right may not be invoked in the case of prosecutions\n"
    ":   genuinely arising from non-political crimes or from acts contrary\n"
    ":   to the purposes and principles of the United Nations.\n"
    "\n"
    "/Article 15\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to a nationality.\n"
    ":\n"
    ": 2 No one shall be arbitrarily deprived of his nationality nor denied\n"
    ":   the right to change his nationality.\n"
    "\n"
    "/Article 16\n"
    "/ - - - - -\n"
    ": 1 Men and women of full age, without any limitation due to race,\n"
    ":   nationality or religion, have the right to marry and to found a\n"
    ":   family.  They are entitled to equal rights as to marriage, during\n"
    ":   marriage and at its dissolution.\n"
    ":\n"
    ": 2 Marriage shall be entered into only with the free and full consent\n"
    ":   of the intending spouses.\n"
    ":\n"
    ": 3 The family is the natural and fundamental group unit of society\n"
    ":   and is entitled to protection by society and the State.\n"
    "\n"
    "/Article 17\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to own property alone as well as in\n"
    ":   association with others.\n"
    ":\n"
    ": 2 No one shall be arbitrarily deprived of his property.\n"
    "\n"
    "/Article 18\n"
    "/ - - - - -\n"
    " Everyone has the right to freedom of thought, conscience and\n"
    " religion; this right includes freedom to change his religion or\n"
    " belief, and freedom, either alone or in community with others and in\n"
    " public or private, to manifest his religion or belief in teaching,\n"
    " practice, worship and observance.\n"
    "\n"
    "/Article 19\n"
    "/ - - - - -\n"
    " Everyone has the right to freedom of opinion and expression; this\n"
    " right includes freedom to hold opinions without interference and to\n"
    " seek, receive and impart information and ideas through any media and\n"
    " regardless of frontiers.\n"
    "\n"
    "/Article 20\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to freedom of peaceful assembly and\n"
    ":   association.\n"
    ":\n"
    ": 2 No one may be compelled to belong to an association.\n"
    "\n"
    "/Article 21\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to take part in the government of his\n"
    ":   country, directly or through freely chosen representatives.\n"
    ":\n"
    ": 2 Everyone has the right of equal access to public service in his\n"
    ":   country.\n"
    ":\n"
    ": 3 The will of the people shall be the basis of the authority of\n"
    ":   government; this will shall be expressed in periodic and genuine\n"
    ":   elections which shall be by universal and equal suffrage and shall\n"
    ":   be held by secret vote or by equivalent free voting procedures.\n"
    "\n"
    "/Article 22\n"
    "/ - - - - -\n"
    " Everyone, as a member of society, has the right to social security\n"
    " and is entitled to realization, through national effort and\n"
    " international co-operation and in accordance with the organization\n"
    " and resources of each State, of the economic, social and cultural\n"
    " rights indispensable for his dignity and the free development of his\n"
    " personality.\n"
    "\n"
    "/Article 23\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to work, to free choice of employment, to\n"
    ":   just and favourable conditions of work and to protection against\n"
    ":   unemployment.\n"
    ":\n"
    ": 2 Everyone, without any discrimination, has the right to equal pay\n"
    ":   for equal work.\n"
    ":\n"
    ": 3 Everyone who works has the right to just and favourable\n"
    ":   remuneration ensuring for himself and his family an existence\n"
    ":   worthy of human dignity, and supplemented, if necessary, by other\n"
    ":   means of social protection.\n"
    ":\n"
    ": 4 Everyone has the right to form and to join trade unions for the\n"
    ":   protection of his interests.\n"
    "\n"
    "/Article 24\n"
    "/ - - - - -\n"
    " Everyone has the right to rest and leisure, including reasonable\n"
    " limitation of working hours and periodic holidays with pay.\n"
    "\n"
    "/Article 25\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to a standard of living adequate for the\n"
    ":   health and well-being of himself and of his family, including\n"
    ":   food, clothing, housing and medical care and necessary social\n"
    ":   services, and the right to security in the event of unemployment,\n"
    ":   sickness, disability, widowhood, old age or other lack of\n"
    ":   livelihood in circumstances beyond his control.\n"
    ":\n"
    ": 2 Motherhood and childhood are entitled to special care and\n"
    ":   assistance.  All children, whether born in or out of wedlock,\n"
    ":   shall enjoy the same social protection.\n"
    "\n"
    "/Article 26\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to education.  Education shall be free, at\n"
    ":   least in the elementary and fundamental stages.  Elementary\n"
    ":   education shall be compulsory.  Technical and professional\n"
    ":   education shall be made generally available and higher education\n"
    ":   shall be equally accessible to all on the basis of merit.\n"
    ":\n"
    ": 2 Education shall be directed to the full development of the human\n"
    ":   personality and to the strengthening of respect for human rights\n"
    ":   and fundamental freedoms.  It shall promote understanding,\n"
    ":   tolerance and friendship among all nations, racial or religious\n"
    ":   groups, and shall further the activities of the United Nations for\n"
    ":   the maintenance of peace.\n"
    ":\n"
    ": 3 Parents have a prior right to choose the kind of education that\n"
    ":   shall be given to their children.\n"
    "\n"
    "/Article 27\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right freely to participate in the cultural life\n"
    ":   of the community, to enjoy the arts and to share in scientific\n"
    ":   advancement and its benefits.\n"
    ":\n"
    ": 2 Everyone has the right to the protection of the moral and material\n"
    ":   interests resulting from any scientific, literary or artistic\n"
    ":   production of which he is the author.\n"
    "\n"
    "/Article 28\n"
    "/ - - - - -\n"
    " Everyone is entitled to a social and international order in which the\n"
    " rights and freedoms set forth in this Declaration can be fully\n"
    " realized.\n"
    "\n"
    "/Article 29\n"
    "/ - - - - -\n"
    ": 1 Everyone has duties to the community in which alone the free and\n"
    ":   full development of his personality is possible.\n"
    ":\n"
    ": 2 In the exercise of his rights and freedoms, everyone shall be\n"
    ":   subject only to such limitations as are determined by law solely\n"
    ":   for the purpose of securing due recognition and respect for the\n"
    ":   rights and freedoms of others and of meeting the just requirements\n"
    ":   of morality, public order and the general welfare in a democratic\n"
    ":   society.\n"
    ":\n"
    ": 3 These rights and freedoms may in no case be exercised contrary to\n"
    ":   the purposes and principles of the United Nations.\n"
    "\n"
    "/Article 30\n"
    "/ - - - - -\n"
    " Nothing in this Declaration may be interpreted as implying for any\n"
    " State, group or person any right to engage in any activity or to\n"
    " perform any act aimed at the destruction of any of the rights and\n"
    " freedoms set forth herein.\n";

    static char document2[] =
    "/CHARTER OF FUNDAMENTAL RIGHTS OF THE EUROPEAN UNION\n"
    "/---------------------------------------------------\n"
    " PREAMBLE\n"
    "\n"
    " The peoples of Europe, in creating an ever closer union among them,\n"
    " are resolved to share a peaceful future based on common values.\n"
    "\n"
    " Conscious of its spiritual and moral heritage, the Union is founded\n"
    " on the indivisible, universal values of human dignity, freedom,\n"
    " equality and solidarity; it is based on the principles of democracy\n"
    " and the rule of law.  It places the individual at the heart of its\n"
    " activities, by establishing the citizenship of the Union and by\n"
    " creating an area of freedom, security and justice.\n"
    "\n"
    " The Union contributes to the preservation and to the development of\n"
    " these common values while respecting the diversity of the cultures\n"
    " and traditions of the peoples of Europe as well as the national\n"
    " identities of the Member States and the organisation of their public\n"
    " authorities at national, regional and local levels; it seeks to\n"
    " promote balanced and sustainable development and ensures free\n"
    " movement of persons, goods, services and capital, and the freedom of\n"
    " establishment.\n"
    "\n"
    " To this end, it is necessary to strengthen the protection of\n"
    " fundamental rights in the light of changes in society, social\n"
    " progress and scientific and technological developments by making\n"
    " those rights more visible in a Charter.\n"
    "\n"
    " This Charter reaffirms, with due regard for the powers and tasks of\n"
    " the Community and the Union and the principle of subsidiarity, the\n"
    " rights as they result, in particular, from the constitutional\n"
    " traditions and international obligations common to the Member States,\n"
    " the Treaty on European Union, the Community Treaties, the European\n"
    " Convention for the Protection of Human Rights and Fundamental\n"
    " Freedoms, the Social Charters adopted by the Community and by the\n"
    " Council of Europe and the case-law of the Court of Justice of the\n"
    " European Communities and of the European Court of Human Rights.\n"
    "\n"
    " Enjoyment of these rights entails responsibilities and duties with\n"
    " regard to other persons, to the human community and to future\n"
    " generations.\n"
    "\n"
    " The Union therefore recognises the rights, freedoms and principles\n"
    " set out hereafter.\n"
    "\n"
    "/CHAPTER I\n"
    "/- - - - -\n"
    " DIGNITY\n"
    "\n"
    "/Article 1\n"
    "/  -  -  -\n"
    " Human dignity\n"
    "\n"
    " Human dignity is inviolable.  It must be respected and protected.\n"
    "\n"
    "/Article 2\n"
    "/  -  -  -\n"
    " Right to life\n"
    "\n"
    ": 1 Everyone has the right to life.\n"
    ": 2 No one shall be condemned to the death penalty, or executed.\n"
    "\n"
    "/Article 3\n"
    "/  -  -  -\n"
    " Right to the integrity of the person\n"
    "\n"
    ": 1 Everyone has the right to respect for his or her physical and\n"
    ":   mental integrity.\n"
    ":\n"
    ": 2 In the fields of medicine and biology, the following must be\n"
    ":   respected in particular:\n"
    ":\n"
    ":   o the free and informed consent of the person concerned, according\n"
    ":     to the procedures laid down by law,\n"
    ":\n"
    ":   o the prohibition of eugenic practices, in particular those aiming\n"
    ":     at the selection of persons,\n"
    ":\n"
    ":   o the prohibition on making the human body and its parts as such a\n"
    ":     source of financial gain,\n"
    ":\n"
    ":   o the prohibition of the reproductive cloning of human beings.\n"
    "\n"
    "/Article 4\n"
    "/  -  -  -\n"
    " Prohibition of torture and inhuman or degrading treatment or\n"
    " punishment\n"
    "\n"
    " No one shall be subjected to torture or to inhuman or degrading\n"
    " treatment or punishment.\n"
    "\n"
    "/Article 5\n"
    "/  -  -  -\n"
    " Prohibition of slavery and forced labour\n"
    "\n"
    ": 1 No one shall be held in slavery or servitude.\n"
    ": 2 No one shall be required to perform forced or compulsory labour.\n"
    ": 3 Trafficking in human beings is prohibited.\n"
    "\n"
    "/CHAPTER II\n"
    "/ - - - - -\n"
    " FREEDOMS\n"
    "\n"
    "/Article 6\n"
    "/  -  -  -\n"
    " Right to liberty and security\n"
    "\n"
    " Everyone has the right to liberty and security of person.\n"
    "\n"
    "/Article 7\n"
    "/  -  -  -\n"
    " Respect for private and family life\n"
    "\n"
    " Everyone has the right to respect for his or her private and family\n"
    " life, home and communications.\n"
    "\n"
    "/Article 8\n"
    "/  -  -  -\n"
    " Protection of personal data\n"
    "\n"
    ": 1 Everyone has the right to the protection of personal data\n"
    ":   concerning him or her.\n"
    ":\n"
    ": 2 Such data must be processed fairly for specified purposes and on\n"
    ":   the basis of the consent of the person concerned or some other\n"
    ":   legitimate basis laid down by law.  Everyone has the right of\n"
    ":   access to data which has been collected concerning him or her, and\n"
    ":   the right to have it rectified.\n"
    ":\n"
    ": 3 Compliance with these rules shall be subject to control by an\n"
    ":   independent authority.\n"
    "\n"
    "/Article 9\n"
    "/  -  -  -\n"
    " Right to marry and right to found a family\n"
    "\n"
    " The right to marry and the right to found a family shall be\n"
    " guaranteed in accordance with the national laws governing the\n"
    " exercise of these rights.\n"
    "\n"
    "/Article 10\n"
    "/-  -  -  -\n"
    " Freedom of thought, conscience and religion\n"
    "\n"
    ": 1 Everyone has the right to freedom of thought, conscience and\n"
    ":   religion.  This right includes freedom to change religion or\n"
    ":   belief and freedom, either alone or in community with others and\n"
    ":   in public or in private, to manifest religion or belief, in\n"
    ":   worship, teaching, practice and observance.\n"
    ":\n"
    ": 2 The right to conscientious objection is recognised, in accordance\n"
    ":   with the national laws governing the exercise of this right.\n"
    "\n"
    "/Article 11\n"
    "/-  -  -  -\n"
    " Freedom of expression and information\n"
    "\n"
    ": 1 Everyone has the right to freedom of expression.  This right shall\n"
    ":   include freedom to hold opinions and to receive and impart\n"
    ":   information and ideas without interference by public authority and\n"
    ":   regardless of frontiers.\n"
    ":\n"
    ": 2 The freedom and pluralism of the media shall be respected.\n"
    "\n"
    "/Article 12\n"
    "/-  -  -  -\n"
    " Freedom of assembly and of association\n"
    "\n"
    ": 1 Everyone has the right to freedom of peaceful assembly and to\n"
    ":   freedom of association at all levels, in particular in political,\n"
    ":   trade union and civic matters, which implies the right of everyone\n"
    ":   to form and to join trade unions for the protection of his or her\n"
    ":   interests.\n"
    ":\n"
    ": 2 Political parties at Union level contribute to expressing the\n"
    ":   political will of the citizens of the Union.\n"
    "\n"
    "/Article 13\n"
    "/-  -  -  -\n"
    " Freedom of the arts and sciences\n"
    "\n"
    " The arts and scientific research shall be free of constraint.\n"
    " Academic freedom shall be respected.\n"
    "\n"
    "/Article 14\n"
    "/-  -  -  -\n"
    " Right to education\n"
    "\n"
    ": 1 Everyone has the right to education and to have access to\n"
    ":   vocational and continuing training.\n"
    ":\n"
    ": 2 This right includes the possibility to receive free compulsory\n"
    ":   education.\n"
    ":\n"
    ": 3 The freedom to found educational establishments with due respect\n"
    ":   for democratic principles and the right of parents to ensure the\n"
    ":   education and teaching of their children in conformity with their\n"
    ":   religious, philosophical and pedagogical convictions shall be\n"
    ":   respected, in accordance with the national laws governing the\n"
    ":   exercise of such freedom and right.\n"
    "\n"
    "/Article 15\n"
    "/-  -  -  -\n"
    " Freedom to choose an occupation and right to engage in work\n"
    "\n"
    ": 1 Everyone has the right to engage in work and to pursue a freely\n"
    ":   chosen or accepted occupation.\n"
    ":\n"
    ": 2 Every citizen of the Union has the freedom to seek employment, to\n"
    ":   work, to exercise the right of establishment and to provide\n"
    ":   services in any Member State.\n"
    ":\n"
    ": 3 Nationals of third countries who are authorised to work in the\n"
    ":   territories of the Member States are entitled to working\n"
    ":   conditions equivalent to those of citizens of the Union.\n"
    "\n"
    "/Article 16\n"
    "/-  -  -  -\n"
    " Freedom to conduct a business\n"
    "\n"
    " The freedom to conduct a business in accordance with Community law\n"
    " and national laws and practices is recognised.\n"
    "\n"
    "/Article 17\n"
    "/-  -  -  -\n"
    " Right to property\n"
    "\n"
    ": 1 Everyone has the right to own, use, dispose of and bequeath his or\n"
    ":   her lawfully acquired possessions.  No one may be deprived of his\n"
    ":   or her possessions, except in the public interest and in the cases\n"
    ":   and under the conditions provided for by law, subject to fair\n"
    ":   compensation being paid in good time for their loss.  The use of\n"
    ":   property may be regulated by law in so far as is necessary for the\n"
    ":   general interest.\n"
    ":\n"
    ": 2 Intellectual property shall be protected.\n"
    "\n"
    "/Article 18\n"
    "/-  -  -  -\n"
    " Right to asylum\n"
    "\n"
    " The right to asylum shall be guaranteed with due respect for the\n"
    " rules of the Geneva Convention of 28 July 1951 and the Protocol of 31\n"
    " January 1967 relating to the status of refugees and in accordance\n"
    " with the Treaty establishing the European Community.\n"
    "\n"
    "/Article 19\n"
    "/-  -  -  -\n"
    " Protection in the event of removal, expulsion or extradition\n"
    "\n"
    ": 1 Collective expulsions are prohibited.\n"
    ":\n"
    ": 2 No one may be removed, expelled or extradited to a State where\n"
    ":   there is a serious risk that he or she would be subjected to the\n"
    ":   death penalty, torture or other inhuman or degrading treatment or\n"
    ":   punishment.\n"
    "\n"
    "/CHAPTER III\n"
    "/- - - - - -\n"
    " EQUALITY\n"
    "\n"
    "/Article 20\n"
    "/-  -  -  -\n"
    " Equality before the law\n"
    "\n"
    " Everyone is equal before the law.\n"
    "\n"
    "/Article 21\n"
    "/-  -  -  -\n"
    " Non-discrimination\n"
    "\n"
    ": 1 Any discrimination based on any ground such as sex, race, colour,\n"
    ":   ethnic or social origin, genetic features, language, religion or\n"
    ":   belief, political or any other opinion, membership of a national\n"
    ":   minority, property, birth, disability, age or sexual orientation\n"
    ":   shall be prohibited.\n"
    ":\n"
    ": 2 Within the scope of application of the Treaty establishing the\n"
    ":   European Community and of the Treaty on European Union, and\n"
    ":   without prejudice to the special provisions of those Treaties, any\n"
    ":   discrimination on grounds of nationality shall be prohibited.\n"
    "\n"
    "/Article 22\n"
    "/-  -  -  -\n"
    " Cultural, religious and linguistic diversity\n"
    "\n"
    " The Union shall respect cultural, religious and linguistic diversity.\n"
    "\n"
    "/Article 23\n"
    "/-  -  -  -\n"
    " Equality between men and women\n"
    "\n"
    " Equality between men and women must be ensured in all areas,\n"
    " including employment, work and pay.  The principle of equality shall\n"
    " not prevent the maintenance or adoption of measures providing for\n"
    " specific advantages in favour of the under-represented sex.\n"
    "\n"
    "/Article 24\n"
    "/-  -  -  -\n"
    " The rights of the child\n"
    "\n"
    ": 1 Children shall have the right to such protection and care as is\n"
    ":   necessary for their well-being.  They may express their views\n"
    ":   freely.  Such views shall be taken into consideration on matters\n"
    ":   which concern them in accordance with their age and maturity.\n"
    ":\n"
    ": 2 In all actions relating to children, whether taken by public\n"
    ":   authorities or private institutions, the child's best interests\n"
    ":   must be a primary consideration.\n"
    ":\n"
    ": 3 Every child shall have the right to maintain on a regular basis a\n"
    ":   personal relationship and direct contact with both his or her\n"
    ":   parents, unless that is contrary to his or her interests.\n"
    "\n"
    "/Article 25\n"
    "/-  -  -  -\n"
    " The rights of the elderly\n"
    "\n"
    " The Union recognises and respects the rights of the elderly to lead a\n"
    " life of dignity and independence and to participate in social and\n"
    " cultural life.\n"
    "\n"
    "/Article 26\n"
    "/-  -  -  -\n"
    " Integration of persons with disabilities\n"
    "\n"
    " The Union recognises and respects the right of persons with\n"
    " disabilities to benefit from measures designed to ensure their\n"
    " independence, social and occupational integration and participation\n"
    " in the life of the community.\n"
    "\n"
    "/CHAPTER IV\n"
    "/ - - - - -\n"
    " SOLIDARITY\n"
    "\n"
    "/Article 27\n"
    "/-  -  -  -\n"
    " Workers' right to information and consultation within the undertaking\n"
    "\n"
    " Workers or their representatives must, at the appropriate levels, be\n"
    " guaranteed information and consultation in good time in the cases and\n"
    " under the conditions provided for by Community law and national laws\n"
    " and practices.\n"
    "\n"
    "/Article 28\n"
    "/-  -  -  -\n"
    " Right of collective bargaining and action\n"
    "\n"
    " Workers and employers, or their respective organisations, have, in\n"
    " accordance with Community law and national laws and practices, the\n"
    " right to negotiate and conclude collective agreements at the\n"
    " appropriate levels and, in cases of conflicts of interest, to take\n"
    " collective action to defend their interests, including strike action.\n"
    "\n"
    "/Article 29\n"
    "/-  -  -  -\n"
    " Right of access to placement services\n"
    "\n"
    " Everyone has the right of access to a free placement service.\n"
    "\n"
    "/Article 30\n"
    "/-  -  -  -\n"
    " Protection in the event of unjustified dismissal\n"
    "\n"
    " Every worker has the right to protection against unjustified\n"
    " dismissal, in accordance with Community law and national laws and\n"
    " practices.\n"
    "\n"
    "/Article 31\n"
    "/-  -  -  -\n"
    " Fair and just working conditions\n"
    "\n"
    ": 1 Every worker has the right to working conditions which respect his\n"
    ":   or her health, safety and dignity.\n"
    ":\n"
    ": 2 Every worker has the right to limitation of maximum working hours,\n"
    ":   to daily and weekly rest periods and to an annual period of paid\n"
    ":   leave.\n"
    "\n"
    "/Article 32\n"
    "/-  -  -  -\n"
    " Prohibition of child labour and protection of young people at work\n"
    "\n"
    " The employment of children is prohibited.  The minimum age of\n"
    " admission to employment may not be lower than the minimum\n"
    " school-leaving age, without prejudice to such rules as may be more\n"
    " favourable to young people and except for limited derogations.  Young\n"
    " people admitted to work must have working conditions appropriate to\n"
    " their age and be protected against economic exploitation and any work\n"
    " likely to harm their safety, health or physical, mental, moral or\n"
    " social development or to interfere with their education.\n"
    "\n"
    "/Article 33\n"
    "/-  -  -  -\n"
    " Family and professional life\n"
    "\n"
    ": 1 The family shall enjoy legal, economic and social protection.\n"
    ":\n"
    ": 2 To reconcile family and professional life, everyone shall have the\n"
    ":   right to protection from dismissal for a reason connected with\n"
    ":   maternity and the right to paid maternity leave and to parental\n"
    ":   leave following the birth or adoption of a child.\n"
    "\n"
    "/Article 34\n"
    "/-  -  -  -\n"
    " Social security and social assistance\n"
    "\n"
    ": 1 The Union recognises and respects the entitlement to social\n"
    ":   security benefits and social services providing protection in\n"
    ":   cases such as maternity, illness, industrial accidents, dependency\n"
    ":   or old age, and in the case of loss of employment, in accordance\n"
    ":   with the rules laid down by Community law and national laws and\n"
    ":   practices.\n"
    ":\n"
    ": 2 Everyone residing and moving legally within the European Union is\n"
    ":   entitled to social security benefits and social advantages in\n"
    ":   accordance with Community law and national laws and practices.\n"
    ":\n"
    ": 3 In order to combat social exclusion and poverty, the Union\n"
    ":   recognises and respects the right to social and housing assistance\n"
    ":   so as to ensure a decent existence for all those who lack\n"
    ":   sufficient resources, in accordance with the rules laid down by\n"
    ":   Community law and national laws and practices.\n"
    "\n"
    "/Article 35\n"
    "/-  -  -  -\n"
    " Health care\n"
    "\n"
    " Everyone has the right of access to preventive health care and the\n"
    " right to benefit from medical treatment under the conditions\n"
    " established by national laws and practices.  A high level of human\n"
    " health protection shall be ensured in the definition and\n"
    " implementation of all Union policies and activities.\n"
    "\n"
    "/Article 36\n"
    "/-  -  -  -\n"
    " Access to services of general economic interest\n"
    "\n"
    " The Union recognises and respects access to services of general\n"
    " economic interest as provided for in national laws and practices, in\n"
    " accordance with the Treaty establishing the European Community, in\n"
    " order to promote the social and territorial cohesion of the Union.\n"
    "\n"
    "/Article 37\n"
    "/-  -  -  -\n"
    " Environmental protection\n"
    "\n"
    " A high level of environmental protection and the improvement of the\n"
    " quality of the environment must be integrated into the policies of\n"
    " the Union and ensured in accordance with the principle of sustainable\n"
    " development.\n"
    "\n"
    "/Article 38\n"
    "/-  -  -  -\n"
    " Consumer protection\n"
    "\n"
    " Union policies shall ensure a high level of consumer protection.\n"
    "\n"
    "/CHAPTER V\n"
    "/- - - - -\n"
    " CITIZENS' RIGHTS\n"
    "\n"
    "/Article 39\n"
    "/-  -  -  -\n"
    " Right to vote and to stand as a candidate at elections to the\n"
    " European Parliament\n"
    "\n"
    ": 1 Every citizen of the Union has the right to vote and to stand as a\n"
    ":   candidate at elections to the European Parliament in the Member\n"
    ":   State in which he or she resides, under the same conditions as\n"
    ":   nationals of that State.\n"
    ":\n"
    ": 2 Members of the European Parliament shall be elected by direct\n"
    ":   universal suffrage in a free and secret ballot.\n"
    "\n"
    "/Article 40\n"
    "/-  -  -  -\n"
    " Right to vote and to stand as a candidate at municipal elections\n"
    "\n"
    " Every citizen of the Union has the right to vote and to stand as a\n"
    " candidate at municipal elections in the Member State in which he or\n"
    " she resides under the same conditions as nationals of that State.\n"
    "\n"
    "/Article 41\n"
    "/-  -  -  -\n"
    " Right to good administration\n"
    "\n"
    ": 1 Every person has the right to have his or her affairs handled\n"
    ":   impartially, fairly and within a reasonable time by the\n"
    ":   institutions and bodies of the Union.\n"
    ":\n"
    ": 2 This right includes:\n"
    ":\n"
    ":   o the right of every person to be heard, before any individual\n"
    ":     measure which would affect him or her adversely is taken;\n"
    ":\n"
    ":   o the right of every person to have access to his or her file,\n"
    ":     while respecting the legitimate interests of confidentiality and\n"
    ":     of professional and business secrecy;\n"
    ":\n"
    ":   o the obligation of the administration to give reasons for its\n"
    ":     decisions.\n"
    ":\n"
    ": 3 Every person has the right to have the Community make good any\n"
    ":   damage caused by its institutions or by its servants in the\n"
    ":   performance of their duties, in accordance with the general\n"
    ":   principles common to the laws of the Member States.\n"
    ":\n"
    ": 4 Every person may write to the institutions of the Union in one of\n"
    ":   the languages of the Treaties and must have an answer in the same\n"
    ":   language.\n"
    "\n"
    "/Article 42\n"
    "/-  -  -  -\n"
    " Right of access to documents\n"
    "\n"
    " Any citizen of the Union, and any natural or legal person residing or\n"
    " having its registered office in a Member State, has a right of access\n"
    " to European Parliament, Council and Commission documents.\n"
    "\n"
    "/Article 43\n"
    "/-  -  -  -\n"
    " Ombudsman\n"
    "\n"
    " Any citizen of the Union and any natural or legal person residing or\n"
    " having its registered office in a Member State has the right to refer\n"
    " to the Ombudsman of the Union cases of maladministration in the\n"
    " activities of the Community institutions or bodies, with the\n"
    " exception of the Court of Justice and the Court of First Instance\n"
    " acting in their judicial role.\n"
    "\n"
    "/Article 44\n"
    "/-  -  -  -\n"
    " Right to petition\n"
    "\n"
    " Any citizen of the Union and any natural or legal person residing or\n"
    " having its registered office in a Member State has the right to\n"
    " petition the European Parliament.\n"
    "\n"
    "/Article 45\n"
    "/-  -  -  -\n"
    " Freedom of movement and of residence\n"
    "\n"
    ": 1 Every citizen of the Union has the right to move and reside freely\n"
    ":   within the territory of the Member States.\n"
    ":\n"
    ": 2 Freedom of movement and residence may be granted, in accordance\n"
    ":   with the Treaty establishing the European Community, to nationals\n"
    ":   of third countries legally resident in the territory of a Member\n"
    ":   State.\n"
    "\n"
    "/Article 46\n"
    "/-  -  -  -\n"
    " Diplomatic and consular protection\n"
    "\n"
    " Every citizen of the Union shall, in the territory of a third country\n"
    " in which the Member State of which he or she is a national is not\n"
    " represented, be entitled to protection by the diplomatic or consular\n"
    " authorities of any Member State, on the same conditions as the\n"
    " nationals of that Member State.\n"
    "\n"
    "/CHAPTER VI\n"
    "/ - - - - -\n"
    " JUSTICE\n"
    "\n"
    "/Article 47\n"
    "/-  -  -  -\n"
    " Right to an effective remedy and to a fair trial\n"
    "\n"
    " Everyone whose rights and freedoms guaranteed by the law of the Union\n"
    " are violated has the right to an effective remedy before a tribunal\n"
    " in compliance with the conditions laid down in this Article.\n"
    " Everyone is entitled to a fair and public hearing within a reasonable\n"
    " time by an independent and impartial tribunal previously established\n"
    " by law.  Everyone shall have the possibility of being advised,\n"
    " defended and represented.  Legal aid shall be made available to those\n"
    " who lack sufficient resources in so far as such aid is necessary to\n"
    " ensure effective access to justice.\n"
    "\n"
    "/Article 48\n"
    "/-  -  -  -\n"
    " Presumption of innocence and right of defence\n"
    "\n"
    ": 1 Everyone who has been charged shall be presumed innocent until\n"
    ":   proved guilty according to law.\n"
    ":\n"
    ": 2 Respect for the rights of the defence of anyone who has been\n"
    ":   charged shall be guaranteed.\n"
    "\n"
    "/Article 49\n"
    "/-  -  -  -\n"
    " Principles of legality and proportionality of criminal offences and\n"
    " penalties\n"
    "\n"
    ": 1 No one shall be held guilty of any criminal offence on account of\n"
    ":   any act or omission which did not constitute a criminal offence\n"
    ":   under national law or international law at the time when it was\n"
    ":   committed.  Nor shall a heavier penalty be imposed than that which\n"
    ":   was applicable at the time the criminal offence was committed.\n"
    ":   If, subsequent to the commission of a criminal offence, the law\n"
    ":   provides for a lighter penalty, that penalty shall be applicable.\n"
    ":\n"
    ": 2 This Article shall not prejudice the trial and punishment of any\n"
    ":   person for any act or omission which, at the time when it was\n"
    ":   committed, was criminal according to the general principles\n"
    ":   recognised by the community of nations.\n"
    ":\n"
    ": 3 The severity of penalties must not be disproportionate to the\n"
    ":   criminal offence.\n"
    "\n"
    "/Article 50\n"
    "/-  -  -  -\n"
    " Right not to be tried or punished twice in criminal proceedings for\n"
    " the same criminal offence\n"
    "\n"
    " No one shall be liable to be tried or punished again in criminal\n"
    " proceedings for an offence for which he or she has already been\n"
    " finally acquitted or convicted within the Union in accordance with\n"
    " the law.\n"
    "\n"
    "/CHAPTER VII\n"
    "/- - - - - -\n"
    " GENERAL PROVISIONS\n"
    "\n"
    "/Article 51\n"
    "/-  -  -  -\n"
    " Scope\n"
    "\n"
    ": 1 The provisions of this Charter are addressed to the institutions\n"
    ":   and bodies of the Union with due regard for the principle of\n"
    ":   subsidiarity and to the Member States only when they are\n"
    ":   implementing Union law.  They shall therefore respect the rights,\n"
    ":   observe the principles and promote the application thereof in\n"
    ":   accordance with their respective powers.\n"
    ":\n"
    ": 2 This Charter does not establish any new power or task for the\n"
    ":   Community or the Union, or modify powers and tasks defined by the\n"
    ":   Treaties.\n"
    "\n"
    "/Article 52\n"
    "/-  -  -  -\n"
    " Scope of guaranteed rights\n"
    "\n"
    ": 1 Any limitation on the exercise of the rights and freedoms\n"
    ":   recognised by this Charter must be provided for by law and respect\n"
    ":   the essence of those rights and freedoms.  Subject to the\n"
    ":   principle of proportionality, limitations may be made only if they\n"
    ":   are necessary and genuinely meet objectives of general interest\n"
    ":   recognised by the Union or the need to protect the rights and\n"
    ":   freedoms of others.\n"
    ":\n"
    ": 2 Rights recognised by this Charter which are based on the Community\n"
    ":   Treaties or the Treaty on European Union shall be exercised under\n"
    ":   the conditions and within the limits defined by those Treaties.\n"
    ":\n"
    ": 3 In so far as this Charter contains rights which correspond to\n"
    ":   rights guaranteed by the Convention for the Protection of Human\n"
    ":   Rights and Fundamental Freedoms, the meaning and scope of those\n"
    ":   rights shall be the same as those laid down by the said\n"
    ":   Convention.  This provision shall not prevent Union law providing\n"
    ":   more extensive protection.\n"
    "\n"
    "/Article 53\n"
    "/-  -  -  -\n"
    " Level of protection\n"
    "\n"
    " Nothing in this Charter shall be interpreted as restricting or\n"
    " adversely affecting human rights and fundamental freedoms as\n"
    " recognised, in their respective fields of application, by Union law\n"
    " and international law and by international agreements to which the\n"
    " Union, the Community or all the Member States are party, including\n"
    " the European Convention for the Protection of Human Rights and\n"
    " Fundamental Freedoms, and by the Member States' constitutions.\n"
    "\n"
    "/Article 54\n"
    "/-  -  -  -\n"
    " Prohibition of abuse of rights\n"
    "\n"
    " Nothing in this Charter shall be interpreted as implying any right to\n"
    " engage in any activity or to perform any act aimed at the destruction\n"
    " of any of the rights and freedoms recognised in this Charter or at\n"
    " their limitation to a greater extent than is provided for herein.\n";

    static char * const documents[]  = { document0,
                                         document1,
                                         document2
                                       };
    const int           numDocuments = sizeof documents / sizeof *documents;
//..
// First, we define several aliases to make our code more comprehensible:
//..
    typedef bsl::pair<int, int>                  WordLocation;
        // Document code number ('first') and word offset ('second') in that
        // document specify a word location.  The first word in the document
        // is at word offset 0.

    typedef bsl::unordered_multimap<bsl::string, WordLocation>
                                                 Concordance;
    typedef Concordance::const_iterator          ConcordanceConstItr;
//..
// Next, we create an (empty) unordered multimap to hold our word tallies:
//..
    Concordance concordance;
//..
// Then, we define the set of characters that define word boundaries:
//..
    const char *delimiters = " \n\t,:;.()[]?!/";
//..
// Next, we extract the words from our documents.  Note that 'strtok' modifies
// the document arrays (which were not made 'const').
//
// As each word is located, we create a map value -- a pair of the word
// converted to a 'bsl::string' and a 'WordLocation' object (itself a pair of
// document code and (word) offset of that word in the document) -- and insert
// the map value into the unordered multimap.  Note that (unlike maps and
// unordered maps) there is no status to check; the insertion succeeds even if
// the key is already present in the unordered multimap.
//..
    for (int idx = 0; idx < numDocuments; ++idx) {
        int wordOffset = 0;
        for (char *cur = strtok(documents[idx], delimiters);
                   cur;
                   cur = strtok(0,              delimiters)) {
            WordLocation            location(idx, wordOffset++);
            Concordance::value_type value(bsl::string(cur), location);
            concordance.insert(value);
        }
    }
//..
// Then, we can print a complete concordance by iterating through the unordered
// multimap:
//..
    for (ConcordanceConstItr itr  = concordance.begin(),
                             end  = concordance.end();
                             end != itr; ++itr) {
if (verbose) {
        printf("\"%s\", %2d, %4d\n",
               itr->first.c_str(),
               itr->second.first,
               itr->second.second);
}
    }
//..
// Standard output shows:
//..
//  "extent",  2, 3837
//  "greater",  2, 3836
//  "abuse",  2, 3791
//  "constitutions",  2, 3782
//  "affecting",  2, 3727
//  ...
//  "he",  1, 1746
//  "he",  1,  714
//  "he",  0,  401
//  "include",  2,  847
//..
// Next, if there are some particular words of interest, we seek them out using
// the 'equal_range' method of the 'concordance' object:
//..
    const bsl::string wordsOfInterest[] = { "human",
                                            "rights",
                                            "unalienable",
                                            "inalienable"
                                          };

    const size_t numWordsOfInterest = sizeof  wordsOfInterest
                                      / sizeof *wordsOfInterest;

    for (size_t idx = 0; idx < numWordsOfInterest; ++idx) {
       bsl::pair<ConcordanceConstItr,
                 ConcordanceConstItr> found = concordance.equal_range(
                                                         wordsOfInterest[idx]);
       for (ConcordanceConstItr itr  = found.first,
                                end  = found.second;
                                end != itr; ++itr) {
if (verbose) {
           printf("\"%s\", %2d, %4d\n",
                  itr->first.c_str(),
                  itr->second.first,
                  itr->second.second);
}
       }
if (verbose) {
       printf("\n");
}
    }
//..
// Finally, we see on standard output:
//..
//  "human",  2, 3492
//  "human",  2, 2192
//  "human",  2,  534
//  ...
//  "human",  1,   65
//  "human",  1,   43
//  "human",  1,   25
//  "human",  0,   20
//
//  "rights",  2, 3583
//  "rights",  2, 3553
//  "rights",  2, 3493
//  ...
//  "rights",  1,   44
//  "rights",  1,   19
//  "rights",  0,  496
//  "rights",  0,  126
//
//  "unalienable",  0,  109
//
//  "inalienable",  1,   18
//
//..
// {'bslstl_unorderedmap'|Example 3} shows how to use the concordance to create
// an inverse concordance, and how to use the inverse concordance to find the
// context (surrouding words) of a word of interest.
#endif // !defined(DONT_DO_USAGE)
}

//=============================================================================
//                              BREATHING TEST
//-----------------------------------------------------------------------------

namespace BREATHING_TEST {

template<class CONTAINER>
const typename CONTAINER::key_type
keyForValue(const typename CONTAINER::value_type v)
{
    return v.first;  // for associative containers
}

template <class CONTAINER>
void testConstEmptyContainer(const CONTAINER& x)
{
    typedef CONTAINER TestType;
    typedef typename TestType::size_type SizeType;

    ASSERT(x.empty());
    ASSERT(0 == x.size());
    ASSERT(0.f == x.load_factor());

    ASSERT(x.begin() == x.end());
    ASSERT(x.cbegin() == x.cend());

    ASSERT(0 == x.count(5));
    ASSERT(x.end() == x.find(5));

    SizeType nBuckets = x.bucket_count();
    for (SizeType i = 0; i != nBuckets; ++i) {
        ASSERT(0 == x.bucket_size(i));
        ASSERT(x.begin(i) == x.end(i));
        ASSERT(x.cbegin(i) == x.cend(i));
    }

    const bsl::pair<typename TestType::const_iterator,
                    typename TestType::const_iterator>
                                                  emptyRange(x.end(), x.end());
    ASSERT(x.equal_range(42) == emptyRange);

    ASSERT(0 == x.count(37));
    ASSERT(x.end() == x.find(26));

    ASSERT(x == x);
    ASSERT(!(x != x));
}

template <class CONTAINER>
void testEmptyContainer(CONTAINER& x)
{
    typedef CONTAINER TestType;
    typedef typename TestType::size_type SizeType;

    ASSERT(x.empty());
    ASSERT(0 == x.size());
    ASSERT(0.f == x.load_factor());

    ASSERT(x.begin() == x.end());
    ASSERT(x.cbegin() == x.cend());
    // Check iterator/const_iterator comparisons compile
    ASSERT(x.begin() == x.cend());
    ASSERT(x.cbegin() == x.end());

    ASSERT(0 == x.count(5));
    ASSERT(x.end() == x.find(5));

    SizeType nBuckets = x.bucket_count();
    for (SizeType i = 0; i != nBuckets; ++i) {
        ASSERT(0 == x.bucket_size(i));
        ASSERT(x.begin(i) == x.end(i));
        ASSERT(x.cbegin(i) == x.cend(i));
    }

    const bsl::pair<typename TestType::iterator, typename TestType::iterator>
                                                  emptyRange(x.end(), x.end());
    ASSERT(x.equal_range(42) == emptyRange);

    ASSERT(0 == x.count(37));
    ASSERT(x.end() == x.find(26));

    typename TestType::iterator it = x.erase(x.begin(), x.end());
                                                          // should not assert

    ASSERT(x.end() == it);

    ASSERT(0 == x.erase(93));

    ASSERT(x == x);
    ASSERT(!(x != x));
}

template <class CONTAINER>
void testContainerHasData(const CONTAINER&                      x,
                          typename CONTAINER::size_type         nCopies,
                          const typename CONTAINER::value_type *data,
                          typename CONTAINER::size_type         size)
{
    typedef typename CONTAINER::size_type      SizeType;
    typedef typename CONTAINER::const_iterator TestIterator;

    LOOP2_ASSERT(x.size(),   nCopies * size,
                 x.size() == nCopies * size);

    for (SizeType i = 0; i != size; ++i) {
        TestIterator it = x.find(keyForValue<CONTAINER>(data[i]));
        ASSERT(x.end() != it);
        ASSERT(*it == data[i]);
        LOOP2_ASSERT(keyForValue<CONTAINER>(data[i]),
                     x.count(keyForValue<CONTAINER>(data[i])),
                     nCopies == x.count(keyForValue<CONTAINER>(data[i])));

        bsl::pair<TestIterator, TestIterator> range =
                                x.equal_range(keyForValue<CONTAINER>(data[i]));
        ASSERT(range.first == it);
        for(SizeType iterations = nCopies; --iterations; ++it) {
            ASSERT(*it == data[i]);
        }
        // Need one extra increment to reach past-the-range iterator.
        if (++it != x.end()) {
            ASSERT(*it != data[i]);
        }
        ASSERT(range.second == it);
    }
}

template <class CONTAINER>
void fillContainerWithData(CONTAINER&                            x,
                           const typename CONTAINER::value_type *data,
                           size_t                                size)
{
    typedef CONTAINER TestType;

    size_t initialSize = x.size();
    x.insert(data, data + size);
    ASSERT(x.size() == initialSize + size);

    for (size_t i = 0; i != size; ++i) {
        typename TestType::iterator it =
                                       x.find(keyForValue<CONTAINER>(data[i]));
        ASSERT(x.end() != it);
        ASSERT(data[i] == *it);
    }
}

template <class CONTAINER>
void validateIteration(CONTAINER &c)
{
    typedef typename CONTAINER::iterator       iterator;
    typedef typename CONTAINER::const_iterator const_iterator;
    typedef typename CONTAINER::size_type      SizeType;

    const SizeType size = c.size();

    SizeType counter = 0;
    for (iterator it = c.begin(); it != c.end(); ++it, ++counter) {}
    ASSERT(size == counter);

    counter = 0;
    for (const_iterator it = c.cbegin(); it != c.cend(); ++it, ++counter) {}
    ASSERT(size == counter);

    const CONTAINER& cc = c;

    counter = 0;
    for (const_iterator it = cc.begin(); it != cc.end(); ++it, ++counter) {}
    ASSERT(size == counter);

    counter = 0;
    for (const_iterator it = cc.cbegin(); it != cc.cend(); ++it, ++counter) {}
    ASSERT(size == counter);
}

template <class CONTAINER>
void testBuckets(CONTAINER& mX)
{
    // Basic test of buckets:
    // number of buckets is returned by bucket_count
    // Number of buckets should reflect load_factor and max_load_factor
    // Each bucket hold a number of elements specified by bucket_size
    // bucket can be iterated from bucket_begin to bucket_end
    //    each element should match the bucket number via bucket(key)
    //    should have as many elements as reported by bucket_count
    // adding elements from all buckets should exactly equal 'size'
    // large buckets imply many hash collisions, which is undesirable
    //    large buckets may be consequence of multicontainers
    typedef typename       CONTAINER::local_iterator       local_iterator;
    typedef typename CONTAINER::const_local_iterator const_local_iterator;


    const CONTAINER &x = mX;

    size_t bucketCount = x.bucket_count();
    size_t collisions = 0;
    size_t itemCount  = 0;

    for (size_t i = 0; i != bucketCount; ++i ) {
        const size_t count = x.bucket_size(i);
        if (0 == count) {
            ASSERT(x.begin(i) == x.end(i));
            ASSERT(mX.begin(i) == mX.end(i));
            ASSERT(mX.cbegin(i) == mX.cend(i));
            // compile test iterator compatibility here, not needed later
            ASSERT(mX.cbegin(i) == mX.end(i));
            ASSERT(mX.begin(i) == mX.cend(i));
            continue;
        }

        itemCount += count;
        collisions += count-1;
        unsigned int bucketItems = 0;
        for (const_local_iterator iter = x.begin(i); iter != x.end(i); ++iter)
        {
            ASSERT(i == x.bucket(keyForValue<CONTAINER>(*iter)));
            ++bucketItems;
        }
        ASSERT(count == bucketItems);

        bucketItems = 0;
        for (local_iterator iter = mX.begin(i); iter != mX.end(i); ++iter) {
            ASSERT(i == x.bucket(keyForValue<CONTAINER>(*iter)));
            ++bucketItems;
        }
        ASSERT(count == bucketItems);

        bucketItems = 0;
        for (const_local_iterator iter = mX.cbegin(i);
             iter != mX.cend(i);
             ++iter)
        {
            ASSERT(i == x.bucket(keyForValue<CONTAINER>(*iter)));
            ++bucketItems;
        }
        ASSERT(count == bucketItems);

    }
    ASSERT(itemCount == x.size());
}


template <class CONTAINER>
void testErase(CONTAINER& mX)
{
    typedef typename CONTAINER::      iterator       iterator;
    typedef typename CONTAINER::const_iterator const_iterator;

    const CONTAINER& x = mX;
    size_t size = x.size();

    // test empty sub-ranges
    ASSERT(x.begin() == mX.erase(x.cbegin(), x.cbegin()));
    ASSERT(x.size() == size);

    ASSERT(x.begin() == mX.erase(x.begin(), x.begin()));
    ASSERT(x.size() == size);

    ASSERT(x.end() == mX.erase(x.cend(), x.cend()));
    ASSERT(x.size() == size);

    ASSERT(x.end() == mX.erase(x.end(), x.end()));
    ASSERT(x.size() == size);

    // eliminate all elements with a matching key
    const_iterator cIter = x.begin();
    bsl::advance(cIter, x.size()/10);
    typename CONTAINER::key_type key = keyForValue<CONTAINER>(*cIter);
    size_t duplicates = x.count(key);
    ASSERT(0 != duplicates);  // already tested, just a sanity check
    bsl::pair<const_iterator, const_iterator> valRange = x.equal_range(key);
    ASSERT(valRange.second != x.end());  // or else container is *tiny*
    ASSERT(mX.erase(valRange.first, valRange.second) == valRange.second);
    ASSERT(x.size() + duplicates == size);
    ASSERT(x.end() == x.find(key));
    validateIteration(mX);

    // cIter is invalidated, so reset and start next sub-test
    // range-erase all matching values for a given key value
    size = x.size();
    cIter = x.begin();
    bsl::advance(cIter, x.size()/5);
    key = keyForValue<CONTAINER>(*cIter);
    duplicates = x.count(key);
    ASSERT(0 != duplicates);  // already tested, just a sanity check
    valRange = x.equal_range(keyForValue<CONTAINER>(*cIter));
    ASSERT(valRange.second != x.end());  // or else container is *tiny*
                            // Will handle subranges terminating in 'end' later
    cIter = valRange.first;
    while (cIter != valRange.second) {
        iterator next = mX.erase(cIter);  // compile check for return type
        cIter = next;
    }
    ASSERT(x.size() + duplicates == size);
    ASSERT(x.end() == x.find(key));
    validateIteration(mX);

    // cIter is invalidated, so reset and start next sub-test
    // range-erase all matching values for a given key value
    size = x.size();
    cIter = x.begin();
    bsl::advance(cIter, x.size()/3);
    key = keyForValue<CONTAINER>(*cIter);
    duplicates = x.count(key);
    ASSERT(0 != duplicates);  // already tested, just a sanity check
    ASSERT(mX.erase(key) == duplicates);
    ASSERT(x.end() == x.find(key));
    ASSERT(x.size() + duplicates == size);
    validateIteration(mX);

    // erase elements straddling a bucket
    // cIter is invalidated, so reset and start next sub-test
    // range-erase all matching values for a given key value
    size = x.size();
    cIter = x.begin();
    key = keyForValue<CONTAINER>(*cIter);
    const_iterator next = cIter;
    while (key == keyForValue<CONTAINER>(*++next)) {
        cIter = next;
    }
    key = keyForValue<CONTAINER>(*next);
    while (key == keyForValue<CONTAINER>(*++next)) {}
    // cIter/next now point to elements either side of a key-range
    // confirm they are not in the same bucket:
    ASSERT(x.bucket(key) != x.bucket(keyForValue<CONTAINER>(*cIter)));
    ASSERT(x.bucket(key) != x.bucket(keyForValue<CONTAINER>(*next)));
    size_t erasures = 0;
    while (cIter != next) {
        cIter = mX.erase(cIter);  // compile check for return type
        ++erasures;
    }
    ASSERT(x.size() + erasures == size);
    ASSERT(x.end() == x.find(key));
    validateIteration(mX);

    // erase elements straddling a bucket, in reverse order
    // cIter is invalidated, so reset and start next sub-test
    // range-erase all matching values for a given key value
    size = x.size();
    cIter = x.begin();
    key = keyForValue<CONTAINER>(*cIter);
    next = cIter;
    while (key == keyForValue<CONTAINER>(*++next)) {
        cIter = next;
    }
    key = keyForValue<CONTAINER>(*next);
    while (key == keyForValue<CONTAINER>(*++next)) {}
    ++next;
    // cIter/next now point to elements either side of a key-range
    // confirm they are not in the same bucket:
    ASSERT(x.bucket(key) != x.bucket(keyForValue<CONTAINER>(*cIter)));
    ASSERT(x.bucket(key) != x.bucket(keyForValue<CONTAINER>(*next)));
    erasures = 0;
    while (cIter != next) {
        const_iterator cursor = cIter;
        const_iterator testCursor = cursor;
        while(++testCursor != next) {
            cursor = testCursor;
        }
        if (cursor == cIter) {
            cIter = mX.erase(cursor);  // compile check for return type
            ASSERT(cIter == next);
            ++erasures;
        }
        else {
            cursor = mX.erase(cursor);  // compile check for return type
            ASSERT(cursor == next);
            ++erasures;
        }
    }
    ASSERT(x.size() + erasures == size);
    ASSERT(x.end() == x.find(key));
    validateIteration(mX);

    // Finally, test erasing the tail of the container, as 'end' is often
    // invalidated.
    // first truncate with a range-based erase
    cIter = mX.erase(cIter, mX.end());  // mixed const/mutable iterators
    ASSERT(cIter == x.end());
    validateIteration(mX);

    // then erase the rest of the container, one item at a time, from the front
    for(iterator it = mX.begin(); it != x.end(); it = mX.erase(it)) {}
    testEmptyContainer(mX);
}

}  // close namespace BREATHING_TEST

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 40: {
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
            usage();
        }
      } break;
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
        if (verbose)
            printf("\nTEST CASE %d IS DELEGATED TO "
                   "'bslstl_unorderedmultimap_test.t.cpp'"
                   "\n========================================================"
                   "==========\n",
                   test);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTION 'g'
        // --------------------------------------------------------------------

        if (verbose) printf("Testing 'g'\n"
                            "===========\n");
        if (verbose) printf("\nThis test has been disabled.\n");
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // STREAMING FUNCTIONALITY
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Streaming Functionality\n"
                            "===============================\n");

        if (verbose) printf("There is no streaming for this component.\n");
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Assignment Operator\n"
                            "===========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        // 'propagate_on_container_copy_assignment' testing

        RUN_EACH_TYPE(TestDriver,
                      testCase9_propagate_on_container_copy_assignment,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        TestDriver<TestKeyType, TestValueType>::
                            testCase9_propagate_on_container_copy_assignment();
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // MANIPULATOR AND FREE FUNCTION 'swap'
        // --------------------------------------------------------------------

        if (verbose) printf("Testing  Swap\n"
                            "=============\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        // 'propagate_on_container_swap' testing

        RUN_EACH_TYPE(TestDriver,
                      testCase8_propagate_on_container_swap,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        // TBD test 'bsltf::MoveOnlyAllocTestType' here

        TestDriver<TestKeyType, TestValueType>::
                                       testCase8_propagate_on_container_swap();

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
        // Test noexcept
#ifndef BSLMF_ISNOTHROWSWAPPABLE_ALWAYS_FALSE
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false> Alloc;
            typedef bsl::hash<int>     Hash;
            typedef bsl::equal_to<int> Equal;

            ASSERT(!bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT( bsl::is_nothrow_swappable<Hash>::value);
            ASSERT( bsl::is_nothrow_swappable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase8_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false> Alloc;
            typedef ThrowingSwapHash<int> Hash;
            typedef bsl::equal_to<int>    Equal;

            ASSERT(!bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT(!bsl::is_nothrow_swappable<Hash>::value);
            ASSERT( bsl::is_nothrow_swappable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase8_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false> Alloc;
            typedef bsl::hash<int>         Hash;
            typedef ThrowingSwapEqual<int> Equal;

            ASSERT(!bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT( bsl::is_nothrow_swappable<Hash>::value);
            ASSERT(!bsl::is_nothrow_swappable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase8_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true> Alloc;
            typedef bsl::hash<int>     Hash;
            typedef bsl::equal_to<int> Equal;

            ASSERT( bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT( bsl::is_nothrow_swappable<Hash>::value);
            ASSERT( bsl::is_nothrow_swappable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase8_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true> Alloc;
            typedef ThrowingSwapHash<int> Hash;
            typedef bsl::equal_to<int>    Equal;

            ASSERT( bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT(!bsl::is_nothrow_swappable<Hash>::value);
            ASSERT( bsl::is_nothrow_swappable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase8_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true> Alloc;
            typedef bsl::hash<int>         Hash;
            typedef ThrowingSwapEqual<int> Equal;

            ASSERT( bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT( bsl::is_nothrow_swappable<Hash>::value);
            ASSERT(!bsl::is_nothrow_swappable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase8_noexcept();
        }
#endif
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false> Alloc;
            typedef ThrowingSwapHash<int>  Hash;
            typedef ThrowingSwapEqual<int> Equal;

            ASSERT(!bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT(!bsl::is_nothrow_swappable<Hash>::value);
            ASSERT(!bsl::is_nothrow_swappable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase8_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true> Alloc;
            typedef ThrowingSwapHash<int>  Hash;
            typedef ThrowingSwapEqual<int> Equal;

            ASSERT( bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT(!bsl::is_nothrow_swappable<Hash>::value);
            ASSERT(!bsl::is_nothrow_swappable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase8_noexcept();
        }
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Copy Constructors\n"
                            "=========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        // 'select_on_container_copy_construction' testing

        if (verbose) printf("\nCOPY CONSTRUCTOR: ALLOCATOR PROPAGATION"
                            "\n=======================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase7_select_on_container_copy_construction,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        TestDriver<TestKeyType, TestValueType>::
                             testCase7_select_on_container_copy_construction();
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Equality Operators\n"
                            "==========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        TestDriver<TestKeyType, TestValueType>::testCase6();
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Output (<<) Operator\n"
                            "============================\n");

        if (verbose)
                   printf("There is no output operator for this component.\n");
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Basic Accessors\n"
                            "=======================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        TestDriver<TestKeyType, TestValueType>::testCase4();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTIONS 'gg' and 'ggg'
        // --------------------------------------------------------------------

        if (verbose) printf("Testing 'gg' and 'ggg'\n"
                            "======================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        TestDriver<TestKeyType, TestValueType>::testCase3();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Primary Manipulators\n"
                            "============================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   That basic functionality appears to work as advertised before
        //   before beginning testing in earnest:
        //   - default and copy constructors
        //   - assignment operator
        //   - primary manipulators, basic accessors
        //   - 'operator==', 'operator!='
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("BREATHING TEST\n"
                            "==============\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
        {
            bsl::unordered_multimap<bsl::string,
                                    bsl::pair<bsl::string, bsl::string> > mX;

            mX.insert({bsl::string{"banana"},
                      {bsl::string{"apple"}, bsl::string{"cherry"} } });
        }
#endif

        using namespace BREATHING_TEST;

        typedef bsl::unordered_multimap<int, int> TestType;

        if (veryVerbose)
            printf("Default construct an unordered_mutlimap, 'x'\n");

        TestType mX;
        const TestType &x = mX;

        if (veryVerbose) printf("Validate default behavior of 'x'\n");

        ASSERT(1.0f == x.max_load_factor());

        testConstEmptyContainer(x);
        testEmptyContainer(mX);

        swap(mX, mX);

        testConstEmptyContainer(x);
        testEmptyContainer(mX);

        validateIteration(mX);

        if (veryVerbose) printf("Prepare a test array of data samples\n");

        typedef TestType::value_type BaseValue;
        const int MAX_SAMPLE = 1000;
        BaseValue *dataSamples = new BaseValue[MAX_SAMPLE];
        for(int i = 0; i != MAX_SAMPLE; ++i) {
            new(&dataSamples[i]) BaseValue(i, i*i);  // inplace-new needed to
                                                     // supply 'const' key
        }

        if (veryVerbose)  printf(
          "Range-construct an unordered_multimap, 'y', from the test array\n");

        TestType mY(dataSamples, dataSamples + MAX_SAMPLE);
        const TestType &y = mY;

        if (veryVerbose)
            printf("Validate behavior of freshly constructed 'y'\n");

        ASSERT(1.0f == y.max_load_factor());

        testContainerHasData(y, 1, dataSamples, MAX_SAMPLE);
        validateIteration(mY);

        if (veryVerbose)
            printf("Assert equality relationships, noting 'x != y'\n");

        ASSERT(x == x);
        ASSERT(!(x != x));
        ASSERT(y != x);
        ASSERT(!(y == x));
        ASSERT(x != y);
        ASSERT(!(x == y));
        ASSERT(y == y);
        ASSERT(!(y != y));

        if (veryVerbose) printf("Swap 'x' and 'y'\n");

        swap(mX, mY);

        if (veryVerbose) printf("Validate swapped values\n");

        testConstEmptyContainer(y);
        testEmptyContainer(mY);

        validateIteration(mY);

        testContainerHasData(x, 1, dataSamples, MAX_SAMPLE);
        validateIteration(mX);

        if (veryVerbose)
            printf("Assert swapped equality relationships, noting 'x != y'\n");

        ASSERT(x == x);
        ASSERT(!(x != x));
        ASSERT(y != x);
        ASSERT(!(y == x));
        ASSERT(x != y);
        ASSERT(!(x == y));
        ASSERT(y == y);
        ASSERT(!(y != y));

        if (veryVerbose) printf("Try to fill 'x' with duplicate values\n");

        for (int i = 0; i != MAX_SAMPLE; ++i) {
            TestType::iterator it = mX.insert(dataSamples[i]);
            ASSERT(x.end() != it);
            ASSERT(*it == dataSamples[i]);
        }
        validateIteration(mX);

        if (veryVerbose) printf(
          "Confirm the value of 'x' with the successfully inserted values.\n");

        testContainerHasData(x, 2, dataSamples, MAX_SAMPLE);

        if (veryVerbose) printf(
                "Create an unordered_multimap, 'z', that is a copy of 'x'.\n");

        TestType mZ = x;
        const TestType &z = mZ;

        if (veryVerbose)
            printf("Validate behavior of freshly constructed 'z'.\n");

        ASSERT(1.0f == z.max_load_factor());
        ASSERT(x == z);
        ASSERT(!(x != z));
        ASSERT(z == x);
        ASSERT(!(z != x));
        testContainerHasData(z, 2, dataSamples, MAX_SAMPLE);

        if (veryVerbose) printf(
                             "Expand 'z' with additional duplicate values.\n");

        validateIteration(mZ);
        for (int i = 3; i != 6; ++i) {
            fillContainerWithData(mZ, dataSamples, MAX_SAMPLE);
            testContainerHasData(z, i, dataSamples, MAX_SAMPLE);
            validateIteration(mZ);
        }
        ASSERT(x != z);
        ASSERT(!(x == z));
        ASSERT(z != x);
        ASSERT(!(z == x));


        if (veryVerbose)
            printf("Confirm that 'x' is unchanged by making the copy.\n");

        testBuckets(mX);
        validateIteration(mX);
        testContainerHasData(x, 2, dataSamples, MAX_SAMPLE);

        if (veryVerbose)
            printf("Clear 'x' and confirm that it is empty.\n");

        mX.clear();
        testEmptyContainer(mX);
        testBuckets(mX);

        if (veryVerbose)
            printf("Assign the value of 'y' to 'x'.\n");

        mX = y;

        if (veryVerbose) printf("Confirm 'x' has the expected value.\n");

        ASSERT(x == y);

        validateIteration(mX);
        testBuckets(mX);

        testErase(mZ);

        if (veryVerbose) printf(
             "Call any remaining methods to be sure they at least compile.\n");

        mX.insert(bsl::pair<const int, int>(1, 1));

        const bsl::allocator<int> alloc   = x.get_allocator();
        const bsl::hash<int>      hasher  = x.hash_function();
        const bsl::equal_to<int>  compare = x.key_eq();

        (void) alloc;
        (void) hasher;
        (void) compare;

        const size_t maxSize    = x.max_size();
        (void) maxSize;
        const size_t buckets    = x.bucket_count();
        const float  loadFactor = x.load_factor();
        const float  maxLF      = x.max_load_factor();

        ASSERT(loadFactor < maxLF);

        mX.rehash(2 * buckets);
        ASSERTV(x.bucket_count(), 2 * buckets, x.bucket_count() > 2 * buckets);
        ASSERTV(x.load_factor(), loadFactor, x.load_factor() < loadFactor);

        mX.reserve(0);
        ASSERTV(x.bucket_count(), 2 * buckets, x.bucket_count() > 2 * buckets);
        ASSERTV(x.load_factor(), loadFactor, x.load_factor() < loadFactor);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        if (verbose) {
             printf("Test initializer lists.\n");
        }
        {
            ASSERT((0 == []() -> bsl::unordered_multimap<char, int> {
                return {};
            }().size()));
            ASSERT((1 == []() -> bsl::unordered_multimap<char, int> {
                return {{'a', 1}};
            }().size()));
            ASSERT((3 == []() -> bsl::unordered_multimap<char, int> {
                return {{'a', 1}, {'b', 2}, {'a', 3}};
            }().size()));
        }
#endif
        if (veryVerbose)
            printf("Final message to confim the end of the breathing test.\n");
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    ASSERTV(defaultAllocator.numBlocksInUse(),
            0 == defaultAllocator.numBlocksInUse());

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
