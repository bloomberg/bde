// bslstl_unorderedmultimap_test.t.cpp                                -*-C++-*-
#include <bslstl_unorderedmultimap_test.h>

#include <bslstl_iterator.h>
#include <bslstl_string.h>

#include <bslalg_rangecompare.h>
#include <bslalg_swaputil.h>
#include <bslstl_unorderedmultimap.h>

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
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_buildtarget.h>
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
// [38] CONCERN: 'find'        properly handles transparent comparators.
// [38] CONCERN: 'count'       properly handles transparent comparators.
// [38] CONCERN: 'equal_range' properly handles transparent comparators.
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

#if defined(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND)
# define BSL_DO_NOT_TEST_MOVE_FORWARDING 1
// Some compilers produce ambiguities when trying to construct our test types
// for 'emplace'-type functionality with the C++03 move-emulation.  This is a
// compiler bug triggering in lower level components, so we simply disable
// those aspects of testing, and rely on the extensive test coverage on other
// platforms.
#endif

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

size_t numCharInstances(const char *SPEC, const char c)
{
    size_t ret = 0;
    for (const char *pc = SPEC; *pc; ++pc) {
        ret += (c == *pc);
    }
    return ret;
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
                            // struct ThrowingMoveHash
                            // =======================

template <class TYPE>
struct ThrowingMoveHash : public bsl::hash<TYPE> {
    // Hash functor with throwing move operations.

    // CREATORS
    ThrowingMoveHash()
        // Create a 'ThrowingMoveHash' object.
    {
    }

    ThrowingMoveHash(const ThrowingMoveHash &other)
        // Create a 'ThrowingMoveHash' object having the same value as that of
        // the specified 'other'.
    {
        (void)other;
    }

    ThrowingMoveHash(bslmf::MovableRef<ThrowingMoveHash> other)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Create a 'ThrowingMoveHash' object having the same value as that of
        // the specified 'other'.
    {
        (void)other;
    }

    // MANIPULATORS
    ThrowingMoveHash &operator=(const ThrowingMoveHash &other)
        // Assign to this object the value of the specified 'other'.
    {
        (void)other;
        return *this;
    }

    ThrowingMoveHash &operator=(bslmf::MovableRef<ThrowingMoveHash> other)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Assign to this object the value of the specified 'other'.
    {
        (void)other;
        return *this;
    }
};
                            // ========================
                            // struct ThrowingMoveEqual
                            // ========================

template <class TYPE>
struct ThrowingMoveEqual : public bsl::equal_to<TYPE> {
    // Equal functor with throwing move operations.

    // CREATORS
    ThrowingMoveEqual()
        // Create a 'ThrowingMoveEqual' object.
    {
    }

    ThrowingMoveEqual(const ThrowingMoveEqual &other)
        // Create a 'ThrowingMoveEqual' object having the same value as that of
        // the specified 'other'.
    {
        (void)other;
    }

    ThrowingMoveEqual(bslmf::MovableRef<ThrowingMoveEqual> other)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Create a 'ThrowingMoveEqual' object having the same value as that of
        // the specified 'other'.
    {
        (void)other;
    }

    // MANIPULATORS
    ThrowingMoveEqual &operator=(const ThrowingMoveEqual &other)
        // Assign to this object the value of the specified 'other'.
    {
        (void)other;
        return *this;
    }

    ThrowingMoveEqual &operator=(bslmf::MovableRef<ThrowingMoveEqual> other)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Assign to this object the value of the specified 'other'.
    {
        (void)other;
        return *this;
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

                       // =============================
                       // struct EraseAmbiguityTestType
                       // =============================

struct EraseAmbiguityTestType
    // This test type has a template constructor that can accept iterator.
{
    // CREATORS
    template <class T>
    EraseAmbiguityTestType(T&)
        // Construct an object.
    {}
};

bool operator==(const EraseAmbiguityTestType&,
                const EraseAmbiguityTestType&)
    // This operator is no-op and written only to satisfy requirements for
    // 'key_type' class.
{
    return true;
}

template <class HASHALG>
inline
void hashAppend(HASHALG& hashAlg, const EraseAmbiguityTestType& object)
    // This function is no-op and written only to satisfy requirements for
    // 'key_type' class.
{
    using ::BloombergLP::bslh::hashAppend;
    (void) object;  // suppress 'unused variable' warning
    hashAppend(hashAlg, 0);
}

void runErasure(
               bsl::unordered_multimap<EraseAmbiguityTestType, int>& container,
               EraseAmbiguityTestType                                element)
    // Look for the specified 'element' in the specified 'container' and delete
    // it if found.  Code is written in such a way as to reveal the ambiguity
    // of the 'erase' method call.
{
    bsl::unordered_multimap<EraseAmbiguityTestType, int>::iterator it =
                                                       container.find(element);
    if (it != container.end()) {
        container.erase(it);
    }
}

                    // =============================
                    // class TransparentlyComparable
                    // =============================

class TransparentlyComparable {
    // DATA
    int d_conversionCount;  // number of times 'operator int' has been called
    int d_value;            // the value

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

    friend bool operator==(const TransparentlyComparable& lhs, int rhs)
        // Return 'true' if the value of the specified 'lhs' is equal to the
        // specified 'rhs', and 'false' otherwise.
    {
        return lhs.d_value == rhs;
    }

    friend bool operator==(int lhs, const TransparentlyComparable& rhs)
        // Return 'true' if the specified 'lhs' is equal to the value of the
        // specified 'rhs', and 'false' otherwise.
    {
        return lhs == rhs.d_value;
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
        // Return 'true' if the specified 'lhs' is equivalent to the specified
        // 'rhs' and 'false' otherwise.
    {
        return lhs == rhs;
    }
};

                      // ========================
                      // struct TransparentHasher
                      // ========================

struct TransparentHasher
    // This class can be used as a comparator for containers.  It has a nested
    // type 'is_transparent', so it is classified as transparent by the
    // 'bslmf::IsTransparentPredicate' metafunction and can be used for
    // heterogeneous comparison.
 {
    typedef void is_transparent;

    size_t operator () (const TransparentlyComparable &value) const
    {
        return static_cast<size_t>(value.value());
    }

    template <class VALUE>
    size_t operator()(const VALUE &value) const
    {
        return static_cast<size_t>(value);
    }
};

template <class Container>
void testTransparentComparator(Container& container,
                               bool       isTransparent,
                               int        initKeyValue)
    // Search for a key equal to the specified 'initKeyValue' in the specified
    // 'container', and count the number of conversions expected based on the
    // specified 'isTransparent'.  Note that 'Container' may resolve to a
    // 'const'-qualified type, we are using the "reference" here as a sort of
    // universal reference; conceptually, the object remains constant, but we
    // want to test 'const'-qualified and non-'const'-qualified overloads.
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

    const Count EXPECTED_C = initKeyValue ? initKeyValue : 1;
    const Count EXISTING_C = container.count(existingKey);

    if (!isTransparent) {
        ++expectedConversionCount;
    }

    ASSERT(EXPECTED_C              == EXISTING_C);
    ASSERT(expectedConversionCount == existingKey.conversionCount());

    const Count NON_EXISTING_C = container.count(nonExistingKey);
    ASSERT(0                       == NON_EXISTING_C);
    ASSERT(expectedConversionCount == nonExistingKey.conversionCount());

    // Testing 'equal_range'.

    const bsl::pair<Iterator, Iterator> EXISTING_ER =
                                            container.equal_range(existingKey);
    if (!isTransparent) {
        ++expectedConversionCount;
    }

    ASSERT(expectedConversionCount == existingKey.conversionCount());
    ASSERT(EXPECTED_C ==
     static_cast<Count>(bsl::distance(EXISTING_ER.first, EXISTING_ER.second)));

    for (Iterator it = EXISTING_ER.first; it != EXISTING_ER.second; ++it) {
        ASSERT(existingKey.value() == it->first);
    }

    const bsl::pair<Iterator, Iterator> NON_EXISTING_ER =
                                         container.equal_range(nonExistingKey);
    ASSERT(NON_EXISTING_ER.first   == NON_EXISTING_ER.second);
    ASSERT(expectedConversionCount == nonExistingKey.conversionCount());
}

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

    static const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value +
                                  bslma::UsesBslmaAllocator<VALUE>::value;

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

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
    template <int N_ARGS_KEY,
              int NK1,
              int NK2,
              int NK3,
              int N_ARGS_VALUE,
              int NV1,
              int NV2,
              int NV3>
    static void testCase30a_RunTest(Obj *target);
        // Call 'emplace' on the specified 'target' container and verify that a
        // value was inserted.  Forward (template parameters) 'N_ARGS_KEY' and
        // 'N_ARGS_VALUE' arguments to the 'emplace' method and ensure 1) that
        // values are properly passed to the piecewise constructor of
        // 'value_type', 2) that the allocator is correctly configured for each
        // argument in the newly inserted element in 'target', and 3) that the
        // arguments are forwarded using copy or move semantics based on
        // integer template parameters 'NK1' ... 'NK3' and 'NV1' ... 'NV3'.

    template <int N_ARGS_KEY,
              int NK1,
              int NK2,
              int NK3,
              int N_ARGS_VALUE,
              int NV1,
              int NV2,
              int NV3>
    static Iter testCase31a_RunTest(Obj *target, CIter hint);
        // Call 'emplace_hint' on the specified 'target' container and verify
        // that a value was inserted.  Forward (template parameters)
        // 'N_ARGS_KEY' and 'N_ARGS_VALUE' arguments to the 'emplace_hint'
        // method and ensure 1) that values are properly passed to the
        // piecewise constructor of 'value_type', 2) that the allocator is
        // correctly configured for each argument in the newly inserted element
        // in 'target', and 3) that the arguments are forwarded using copy or
        // move semantics based on integer template parameters 'NK1' ... 'NK3'
        // and 'NV1' ... 'NV3'.
#endif
  public:
    // TEST CASES
    static void testCase37();
        // Test absence of 'erase' method ambiguity.

    static void testCase36();
        // Test 'noexcept' specifications

    static void testCase34();
        // Growing functions.

    static void testCase33();
        // Test Typedefs.

    static void testCase32_outOfLine();
    static void testCase32_inline();
        // Test methods taking initializer lists.

    static void testCase31a();
        // Test forwarding of arguments in 'emplace_hint' method.

    static void testCase31();
        // Test 'emplace_hint' method.

    static void testCase30a();
        // Test forwarding of arguments in 'emplace' method.

    static void testCase30();
        // Test 'emplace' method.

    static void testCase29();
        // Test 'insert' on movable value with hint.

    static void testCase28();
        // Test 'insert' on movable value.

    template <bool PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase27_propagate_on_container_move_assignment_dispatch();
    static void testCase27_propagate_on_container_move_assignment();
        // Test 'propagate_on_container_move_assignment'.

    static void testCase27_noexcept();
        // Test move assignment operator noexcept.

    static void testCase27();
        // Test move assignment.

    static void testCase26();
        // Test move constructors.

    static void testCase23();
        // Test type traits.

    static void testCase22();
        // Test STL allocator.

    static void testCase21();
        // Test comparators.

    static void testCase20();
        // Test 'max_size' and 'empty'.

    static void testCase18();
        // Test 'erase'.

    static void testCase17();
        // Test range 'insert'.

    static void testCase16();
        // Test 'insert' with hint.

    static void testCase15();
        // Test 'insert'.

    static void testCase14();
        // Test iterators.

    static void testCase13();
        // Test 'find', 'equal_range' and 'count'.

    static void testCase12();
        // Test value and range constructors.

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
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
template <int N_ARGS_KEY,
          int NK1,
          int NK2,
          int NK3,
          int N_ARGS_VALUE,
          int NV1,
          int NV2,
          int NV3>
void
TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase30a_RunTest(Obj *target)
{
    // In C++17 these become the simpler to name 'bool_constant'
    static const bsl::integral_constant<bool, NK1 == 1> MOVE_K1 = {};
    static const bsl::integral_constant<bool, NK2 == 1> MOVE_K2 = {};
    static const bsl::integral_constant<bool, NK3 == 1> MOVE_K3 = {};
    static const bsl::integral_constant<bool, NV1 == 1> MOVE_V1 = {};
    static const bsl::integral_constant<bool, NV2 == 1> MOVE_V2 = {};
    static const bsl::integral_constant<bool, NV3 == 1> MOVE_V3 = {};

    bslma::TestAllocator *testAlloc = dynamic_cast<bslma::TestAllocator *>(
                                          target->get_allocator().mechanism());
    if (!testAlloc) {
        ASSERT(!"Allocator in test case 30 is not a test allocator!");
        return;
    }

    bslma::TestAllocator& oa = *testAlloc;
    Obj& mX = *target; const Obj& X = mX;

    bslma::TestAllocator aa("args", veryVeryVeryVerbose);
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    Iter result;

    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

        // Construct all arguments inside the exception test loop as the
        // exception thrown after moving only a portion of arguments leave the
        // moved arguments in a valid, but unspecified state.
        bsls::ObjectBuffer<typename KEY::ArgType01> BUFK1;
        ConsUtil::construct(BUFK1.address(), &aa, K01);
        typename KEY::ArgType01& AK1 = BUFK1.object();
        bslma::DestructorGuard<typename KEY::ArgType01> GK1(&AK1);

        bsls::ObjectBuffer<typename KEY::ArgType02> BUFK2;
        ConsUtil::construct(BUFK2.address(), &aa, K02);
        typename KEY::ArgType02& AK2 = BUFK2.object();
        bslma::DestructorGuard<typename KEY::ArgType02> GK2(&AK2);

        bsls::ObjectBuffer<typename KEY::ArgType03> BUFK3;
        ConsUtil::construct(BUFK3.address(), &aa, K03);
        typename KEY::ArgType03& AK3 = BUFK3.object();
        bslma::DestructorGuard<typename KEY::ArgType03> GK3(&AK3);

        bsls::ObjectBuffer<typename VALUE::ArgType01> BUFV1;
        ConsUtil::construct(BUFV1.address(), &aa, V01);
        typename VALUE::ArgType01& AV1 = BUFV1.object();
        bslma::DestructorGuard<typename VALUE::ArgType01> GV1(&AV1);

        bsls::ObjectBuffer<typename VALUE::ArgType02> BUFV2;
        ConsUtil::construct(BUFV2.address(), &aa, V02);
        typename VALUE::ArgType02& AV2 = BUFV2.object();
        bslma::DestructorGuard<typename VALUE::ArgType02> GV2(&AV2);

        bsls::ObjectBuffer<typename VALUE::ArgType03> BUFV3;
        ConsUtil::construct(BUFV3.address(), &aa, V03);
        typename VALUE::ArgType03& AV3 = BUFV3.object();
        bslma::DestructorGuard<typename VALUE::ArgType03> GV3(&AV3);

        ExceptionProctor<Obj, ALLOC> guard(&X, L_, &scratch);

        switch (N_ARGS_KEY) {
          case 0: {
            switch (N_ARGS_VALUE) {
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
                  ASSERTV(!"Invalid # of args!");
              } break;
            }
          } break;
          case 1: {
            switch (N_ARGS_VALUE) {
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
                  ASSERTV(!"Invalid # of args!");
              } break;
            }
          } break;
          case 2: {
            switch (N_ARGS_VALUE) {
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
                  ASSERTV(!"Invalid # of args!");
              } break;
            }
          } break;
          case 3: {
            switch (N_ARGS_VALUE) {
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
                  ASSERTV(!"Invalid # of args!");
              } break;
            }
          } break;
          default: {
            ASSERTV(!"Invalid # of args!");
          } break;
        }
        guard.release();

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

        ASSERTV(V01, V.arg01(), V01 == V.arg01() || 2 == NV1);
        ASSERTV(V02, V.arg02(), V02 == V.arg02() || 2 == NV2);
        ASSERTV(V03, V.arg03(), V03 == V.arg03() || 2 == NV3);

        TestAllocatorUtil::test(K, oa);
        TestAllocatorUtil::test(V, oa);
    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
template <int N_ARGS_KEY,
          int NK1,
          int NK2,
          int NK3,
          int N_ARGS_VALUE,
          int NV1,
          int NV2,
          int NV3>
typename TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::Iter
TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase31a_RunTest(Obj   *target,
                                                                CIter  hint)
{
    // In C++17 these become the simpler to name 'bool_constant'
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
        return target->end();
    }

    bslma::TestAllocator& oa = *testAlloc;
    Obj& mX = *target; const Obj& X = mX;

    bslma::TestAllocator aa("args", veryVeryVeryVerbose);
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    Iter result;

    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

        // Construct all arguments inside the exception test loop as the
        // exception thrown after moving only a portion of arguments leave the
        // moved arguments in a valid, but unspecified state.
        bsls::ObjectBuffer<typename KEY::ArgType01> BUFK1;
        ConsUtil::construct(BUFK1.address(), &aa, K01);
        typename KEY::ArgType01& AK1 = BUFK1.object();
        bslma::DestructorGuard<typename KEY::ArgType01> GK1(&AK1);

        bsls::ObjectBuffer<typename KEY::ArgType02> BUFK2;
        ConsUtil::construct(BUFK2.address(), &aa, K02);
        typename KEY::ArgType02& AK2 = BUFK2.object();
        bslma::DestructorGuard<typename KEY::ArgType02> GK2(&AK2);

        bsls::ObjectBuffer<typename KEY::ArgType03> BUFK3;
        ConsUtil::construct(BUFK3.address(), &aa, K03);
        typename KEY::ArgType03& AK3 = BUFK3.object();
        bslma::DestructorGuard<typename KEY::ArgType03> GK3(&AK3);

        bsls::ObjectBuffer<typename VALUE::ArgType01> BUFV1;
        ConsUtil::construct(BUFV1.address(), &aa, V01);
        typename VALUE::ArgType01& AV1 = BUFV1.object();
        bslma::DestructorGuard<typename VALUE::ArgType01> GV1(&AV1);

        bsls::ObjectBuffer<typename VALUE::ArgType02> BUFV2;
        ConsUtil::construct(BUFV2.address(), &aa, V02);
        typename VALUE::ArgType02& AV2 = BUFV2.object();
        bslma::DestructorGuard<typename VALUE::ArgType02> GV2(&AV2);

        bsls::ObjectBuffer<typename VALUE::ArgType03> BUFV3;
        ConsUtil::construct(BUFV3.address(), &aa, V03);
        typename VALUE::ArgType03& AV3 = BUFV3.object();
        bslma::DestructorGuard<typename VALUE::ArgType03> GV3(&AV3);

        ExceptionProctor<Obj, ALLOC> proctor(&X, L_, &scratch);

        switch (N_ARGS_KEY) {
          case 0: {
            switch (N_ARGS_VALUE) {
              case 0: {
                  result = mX.emplace_hint(hint, std::piecewise_construct,
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
                  ASSERTV(!"Invalid # of args!");
              } break;
            }
          } break;
          case 1: {
            switch (N_ARGS_VALUE) {
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
                  ASSERTV(!"Invalid # of args!");
              } break;
            }
          } break;
          case 2: {
            switch (N_ARGS_VALUE) {
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
                  ASSERTV(!"Invalid # of args!");
              } break;
            }
          } break;
          case 3: {
            switch (N_ARGS_VALUE) {
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
                  ASSERTV(!"Invalid # of args!");
              } break;
            }
          } break;
          default: {
            ASSERTV(!"Invalid # of args!");
          } break;
        }
        proctor.release();

        if (mX.end() != hint) {
            ASSERTV(true == (&(*result) != &(*hint)));
        }

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
        ASSERTV(V01, V.arg01(), V01 == V.arg01() || 2 == NV1);
        ASSERTV(V02, V.arg02(), V02 == V.arg02() || 2 == NV2);
        ASSERTV(V03, V.arg03(), V03 == V.arg03() || 2 == NV3);

        TestAllocatorUtil::test(K, oa);
        TestAllocatorUtil::test(V, oa);
    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

    return result;
}
#endif

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase37()
{
    // ------------------------------------------------------------------------
    // TESTING ABSENCE OF ERASE AMBIGUITY
    //  'std::unordered_multimap::erase' takes an iterator in C++03, but a
    //  const_iterator in C++0x.  This breaks code where the
    //  unordered_multimap's 'key_type' has a constructor which accepts an
    //  iterator (for example a template constructor), as the compiler cannot
    //  choose between 'erase(const key_type&)' and 'erase(const_iterator)'. As
    //  BDE library duplicates 'std' interfaces, it has the same problem. The
    //  solution is to restore the iterator overload in addition to the
    //  const_iterator overload.
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

    VALUE                                      value(5);
    KEY                                        key(value);
    bsl::unordered_multimap<KEY, VALUE>        mX;
    const bsl::unordered_multimap<KEY, VALUE>& X = mX;

    mX.insert(bsl::pair<KEY, VALUE>(key, value));
    ASSERTV(X.size(), 1 == X.size());
    runErasure(mX, key);
    ASSERTV(X.size(), 0 == X.size());
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase36()
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
        P(bsls::NameOf<KEY>())
        P(bsls::NameOf<VALUE>())
        P(bsls::NameOf<HASH>())
        P(bsls::NameOf<EQUAL>())
        P(bsls::NameOf<ALLOC>())
    }

    // N4594: 23.5.5.5: Class template 'unordered_multimap'

    // page 887 - 888
    //..
    //  // 23.5.5.2, construct/copy/destroy:
    //  unordered_multimap& operator=(unordered_multimap&&)
    //   noexcept(allocator_traits<Allocator>::is_always_equal::value &&
    //            is_nothrow_move_assignable<Hash>::value &&
    //            is_nothrow_move_assignable<Pred>::value);
    //  allocator_type get_allocator() const noexcept;
    //..

    {
        Obj mX;  const Obj& X = mX;    (void) X;
        Obj mY;  const Obj& Y = mY;    (void) Y;

        ASSERT(false
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX =
                                             bslmf::MovableRefUtil::move(mY)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.get_allocator()));
    }

    // page 888
    //..
    //  // iterators:
    //  iterator begin() noexcept;
    //  const_iterator begin() const noexcept;
    //  iterator end() noexcept;
    //  const_iterator end() const noexcept;
    //  const_iterator cbegin() const noexcept;
    //  const_iterator cend() const noexcept;
    //..

    {
        Obj mX; const Obj& X = mX;    (void) X;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.begin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.begin()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.end()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.end()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.cbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.cend()));
    }

    // page 888
    //..
    //  // capacity:
    //  bool empty() const noexcept;
    //  size_type size() const noexcept;
    //  size_type max_size() const noexcept;
    //..

    {
        Obj mX; const Obj& X = mX;    (void) X;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.empty()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.size()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.max_size()));
    }

    // page 889
    // 23.5.5.3, modifiers:
    //..
    //  void swap(unordered_multimap&)
    //      noexcept(allocator_traits<Allocator>::is_always_equal::value &&
    //               is_nothrow_swappable_v<Hash> &&
    //               is_nothrow_swappable_v<Pred>);
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

    // page 889
    //..
    //  // bucket interface:
    //  size_type bucket_count() const noexcept;
    //  size_type max_bucket_count() const noexcept;
    //..

    {
        Obj mX; const Obj& X = mX;    (void) X;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.bucket_count()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.max_bucket_count()));
    }

    // page 889
    //..
    //  // hash policy
    //  float load_factor() const noexcept;
    //  float max_load_factor() const noexcept;
    //..

    {
        Obj mX; const Obj& X = mX;    (void) X;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.load_factor()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.max_load_factor()));
    }

    // page 890
    //..
    //  // 23.5.5.4, swap:
    //  template <class Key, class T, class Hash, class Pred, class Alloc>
    //  void swap(unordered_multimap<Key, T, Hash, Pred, Alloc>& x,
    //            unordered_multimap<Key, T, Hash, Pred, Alloc>& y)
    //      noexcept(noexcept(x.swap(y)));
    //..

    {
        Obj mX;    (void) mX;
        Obj mY;    (void) mY;

        ASSERT(false == BSLS_KEYWORD_NOEXCEPT_OPERATOR(swap(mX, mY)));
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase34()
{
    // ------------------------------------------------------------------------
    // BUCKET GROWTH
    // ------------------------------------------------------------------------

    {
        typedef float (Obj::*MP)() const;
        MP mp = &Obj::load_factor;
        (void) mp;
        mp    = &Obj::max_load_factor;
        (void) mp;
    }
    {
        typedef void (Obj::*MP)(float);
        MP mp = &Obj::max_load_factor;
        (void) mp;
    }
    {
        typedef void (Obj::*MP)(size_t);
        MP mp = &Obj::reserve;
        (void) mp;
        mp    = &Obj::rehash;
        (void) mp;
    }

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char       *SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(SPEC);

        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        if (veryVeryVerbose) Q(Test 'rehash');
        {
            Obj mX(&sa); const Obj& X = mX;
            gg(&mX, SPEC);

            const size_t BC = X.bucket_count();

            ASSERTV((LENGTH > 0) == (BC > 1));

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                ASSERTV(BC == X.bucket_count());

                mX.rehash(2 * BC);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV((LENGTH > 0) == (X.bucket_count() > 2 * BC));
        }

        if (veryVeryVerbose) Q(Test 'load_factor' and 'max_load_factor');
        {
            const float loadFactors[] = { 1.0f, 2.0f, 1.0f / 2, 1.0f / 4 };
            enum { NUM_LOAD_FACTORS =
                                    sizeof loadFactors / sizeof *loadFactors };

            for (size_t tj = 0; tj < NUM_LOAD_FACTORS; ++tj) {
                const float MAX_LOAD_FACTOR = loadFactors[tj];

                Obj mX(&sa);    const Obj& X = mX;
                if (0 != tj) {
                    mX.max_load_factor(MAX_LOAD_FACTOR);
                }
                ASSERTV(X.max_load_factor() == MAX_LOAD_FACTOR);
                gg(&mX, SPEC);

                const size_t BC = X.bucket_count();

                ASSERTV(X.load_factor() <= X.max_load_factor());
                ASSERTV(nearlyEqual<double>(X.load_factor(),
                                            static_cast<double>(X.size())
                                            / static_cast<double>(BC)));

                if (LENGTH > 0) {
                    const float newLoadFactor = X.load_factor() / 2;
                    int numPasses = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        ++numPasses;

                        ASSERTV(BC == X.bucket_count());

                        mX.max_load_factor(newLoadFactor);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    ASSERTV(!PLAT_EXC || numPasses > 1);

                    ASSERTV(newLoadFactor == X.max_load_factor());

                    ASSERTV(X.bucket_count() > 2 * BC);
                    ASSERTV(X.load_factor() <= X.max_load_factor());
                }
            }
        }

        if (veryVeryVerbose) Q(Test 'reserve');
        {
            for (size_t len2 = 0; len2 <= LENGTH; ++len2) {
                Obj mX(&sa); const Obj& X = mX;

                TestValues values(SPEC, &sa);

                mX.insert(values.begin(), values.index(len2));

                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    ++numPasses;

                    mX.reserve(LENGTH);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERTV(!PLAT_EXC || len2 || 0 == LENGTH || numPasses > 1);

                const size_t BC = X.bucket_count();
                ASSERTV(X.load_factor() <= X.max_load_factor());
                ASSERTV(0.9999
                        * static_cast<double>(LENGTH)
                          / static_cast<double>(X.bucket_count()) <
                                                          X.max_load_factor());

                mX.insert(values.index(len2), values.end());

                ASSERTV(LINE, SPEC, LENGTH, X.size(), LENGTH == X.size());
                ASSERTV(verifySpec(X, SPEC));

                ASSERTV(BC == X.bucket_count());
            }
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase33()
{
    // ------------------------------------------------------------------------
    // TESTING TYPEDEFs
    // ------------------------------------------------------------------------

    typedef bsl::pair<const KEY, VALUE>        VT;

    BSLMF_ASSERT((bslmf::IsSame<KEY,        typename Obj::key_type>::value));
    BSLMF_ASSERT((bslmf::IsSame<VT,         typename Obj::value_type>::value));
    BSLMF_ASSERT((bslmf::IsSame<VALUE,      typename Obj::mapped_type>::
                                                                       value));
    BSLMF_ASSERT((bslmf::IsSame<HASH,       typename Obj::hasher>::value));
    BSLMF_ASSERT((bslmf::IsSame<EQUAL,      typename Obj::key_equal>::value));
    BSLMF_ASSERT((bslmf::IsSame<ALLOC,      typename Obj::allocator_type>::
                                                                       value));
    BSLMF_ASSERT((bslmf::IsSame<VT *,       typename Obj::pointer>::value));
    BSLMF_ASSERT((bslmf::IsSame<const VT *, typename Obj::const_pointer>::
                                                                       value));
    BSLMF_ASSERT((bslmf::IsSame<VT &,       typename Obj::reference>::value));
    BSLMF_ASSERT((bslmf::IsSame<const VT &, typename Obj::const_reference>::
                                                                       value));

    BSLMF_ASSERT((bslmf::IsSame<typename Obj::pointer,
                                typename ALLOC::pointer>::value));
    BSLMF_ASSERT((bslmf::IsSame<typename Obj::const_pointer,
                                typename ALLOC::const_pointer>::value));
    BSLMF_ASSERT((bslmf::IsSame<typename Obj::reference,
                                typename ALLOC::reference>::value));
    BSLMF_ASSERT((bslmf::IsSame<typename Obj::const_reference,
                                typename ALLOC::const_reference>::value));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase32_outOfLine()
{
    // ------------------------------------------------------------------------
    // TESTING FUNCTIONS TAKING INITIALIZER LISTS
    //
    // Concerns:
    //:  The 6 functions that take an initializer lists (four constructors,
    //:  an assignment operator, and the 'insert' function) simply forward to
    //:  another already tested function.  We are interested here only in
    //:  ensuring that the forwarding is working -- not retesting already
    //:  tested functionality.
    //
    // Plan:
    //:  TBD
    //:
    // Testing:
    //   unordered_multimap(initializer_list, const A& allocator);
    //   unordered_multimap(initializer_list, size_t, const A& allocator);
    //   unordered_multimap(initializer_list, size_t, hasher, const A& alloc);
    //   unordered_multimap(initializer_list, size_t, hasher, equal, const A&);
    //   unordered_multimap& operator=(initializer_list<value_type>);
    //   iterator insert(initializer_list<value_type>);
    // -----------------------------------------------------------------------
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    const TestValues V;

    if (veryVerbose) { P(NameOf<KEY>().name()) P(NameOf<VALUE>().name()); }

    typedef std::initializer_list<ValueType> InitList;

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocatorRaw(&da);

    if (verbose)
        printf("\nTesting constructor with initializer lists.\n");
    {
        const struct {
            int          d_line;   // source line number
            InitList     d_list;   // source list
            const char  *d_result; // expected result
        } DATA[] = {
            //line  list                   result
            //----  ---------------------  ------
            { L_,   {                  },  ""     },
            { L_,   { V[0]             },  "A"    },
            { L_,   { V[0], V[0]       },  "AA"   },
            { L_,   { V[0], V[1]       },  "AB"   },
            { L_,   { V[1], V[0]       },  "AB"   },
            { L_,   { V[0], V[1], V[0] },  "AAB"  },
            { L_,   { V[0], V[1], V[2] },  "ABC"  },
            { L_,   { V[2], V[1], V[1] },  "BBC"  },
        };

        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor dam(&da);

        const HASH  hf(5);
        const EQUAL eq(7);

        for (char cfg = 'a'; cfg <= 'h' ; ++cfg) {
            for (size_t ti = 0; ti < NUM_DATA; ++ti) {
                int              LINE   = DATA[ti].d_line;
                const char      *SPEC   = DATA[ti].d_result;
                const InitList&  LIST   = DATA[ti].d_list;
                const size_t     LENGTH = strlen(SPEC);

                ASSERT(0 == oa.numBytesInUse());

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                bslma::TestAllocator&  oa = strchr("bdfh", cfg) ? sa : da;
                bslma::TestAllocator& noa = &da == &oa          ? sa : da;

                Obj *objPtr = 0;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    switch (cfg) {
                      case 'a': {
                        objPtr = new (fa) Obj(LIST);
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj(LIST, &sa);
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(LIST, 100);
                      } break;
                      case 'd': {
                        objPtr = new (fa) Obj(LIST, 100, &sa);
                      } break;
                      case 'e': {
                        objPtr = new (fa) Obj(LIST, 100, hf);
                      } break;
                      case 'f': {
                        objPtr = new (fa) Obj(LIST, 100, hf, &sa);
                      } break;
                      case 'g': {
                        objPtr = new (fa) Obj(LIST, 100, hf, eq);
                      } break;
                      case 'h': {
                        objPtr = new (fa) Obj(LIST, 100, hf, eq, &sa);
                      } break;
                      default: {
                        ASSERTV(cfg, !"Invalid constructor choice.");
                      } return;                                       // RETURN
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(objPtr);

                Obj& mX = *objPtr; const Obj& X = mX;

                ASSERTV(LENGTH == X.size());
                ASSERTV(LINE, X, verifySpec(X, SPEC));

                ASSERTV(&oa == X.get_allocator());

                const bool hfPassed = strchr("efgh", cfg) ? true : false;
                const int  hfId = X.hash_function().id();
                ASSERTV(hfId, hfId ==  (hfPassed ? hf.id() : HASH().id()));

                const bool eqPassed = strchr("gh", cfg) ? true : false;
                const int  eqId = X.key_eq().id();
                ASSERTV(hfId, eqId ==  (eqPassed ? eq.id() : EQUAL().id()));

                // --------------------------------------------------------
                // Verify no allocation from the non-object allocator (before
                // deletion -- in SAFE2 mode, the d'tor calls 'isWellFormed',
                // which uses the default allocator.
                ASSERTV(cfg, 0 == noa.numBlocksInUse());

                fa.deleteObjectRaw(objPtr);

                ASSERTV(cfg, 0 == fa.numBlocksInUse());
                ASSERTV(cfg, 0 == da.numBlocksInUse());
                ASSERTV(cfg, 0 == sa.numBlocksInUse());
            }
        }
    }

    if (verbose)
        printf("\nTesting constructor with initializer lists.\n");
    {
        const struct {
            int          d_line;   // source line number
            InitList     d_list;   // source list
            const char  *d_result; // expected result
        } DATA[] = {
            //line  list                   result
            //----  ---------------------  ------
            { L_,   {                  },  ""     },
            { L_,   { V[0]             },  "A"    },
            { L_,   { V[0], V[0]       },  "AA"   },
            { L_,   { V[0], V[1]       },  "AB"   },
            { L_,   { V[1], V[0]       },  "AB"   },
            { L_,   { V[0], V[1], V[0] },  "AAB"  },
            { L_,   { V[0], V[1], V[2] },  "ABC"  },
            { L_,   { V[2], V[1], V[1] },  "BBC"  },
        };

        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);
        bslma::TestAllocatorMonitor  dam(&da);

        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            Obj mY(&scratch); const Obj& Y = gg(&mY, DATA[ti].d_result);

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

    if (verbose)
        printf("\nTesting 'operator=' with initializer lists.\n");
    {
        const struct {
            int                               d_line;   // source line number
            const char                       *d_spec;   // target string
            std::initializer_list<ValueType>  d_list;   // source list
            const char                       *d_result; // expected result
        } DATA[] = {
            //line  lhs    list                   result
            //----  -----  ---------------------  ------
            { L_,   "",    {                  },  ""     },
            { L_,   "",    { V[0]             },  "A"    },
            { L_,   "A",   {                  },  ""     },
            { L_,   "A",   { V[1]             },  "B"    },
            { L_,   "A",   { V[0], V[1]       },  "AB"   },
            { L_,   "A",   { V[1], V[2]       },  "BC"   },
            { L_,   "AB",  {                  },  ""     },
            { L_,   "AB",  { V[0], V[1], V[2] },  "ABC"  },
            { L_,   "AB",  { V[2], V[1], V[0] },  "ABC"  },
            { L_,   "AB",  { V[2], V[3], V[4] },  "CDE"  },
        };

        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor dam(&da);

        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            int              LINE   = DATA[ti].d_line;
            const char      *SPEC   = DATA[ti].d_spec;
            const InitList&  LIST   = DATA[ti].d_list;
            const char      *RESULT = DATA[ti].d_result;

            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            Obj mY(&scratch); const Obj& Y = gg(&mY, RESULT);

            Obj mX(&oa); const Obj& X = gg(&mX, SPEC);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj *mR = &(mX = LIST);
                ASSERTV(mR, &mX, mR == &mX);
                ASSERTV(LINE, Y,  X,   Y  == X);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        ASSERT(dam.isTotalSame());
    }

    if (verbose)
        printf("\nTesting 'insert' with initializer lists\n");
    {
        const struct {
            int                               d_line;   // source line number
            const char                       *d_spec;   // target string
            std::initializer_list<ValueType>  d_list;   // source list
            const char                       *d_result; // expected result
        } DATA[] = {
            //line  source  list                   result
            //----  ------  ---------------------  -------
            { L_,   "",     {                  },  ""      },
            { L_,   "",     { V[0]             },  "A"     },
            { L_,   "A",    {                  },  "A"     },
            { L_,   "A",    { V[0]             },  "AA"    },
            { L_,   "A",    { V[1]             },  "AB"    },
            { L_,   "AB",   { V[0], V[1]       },  "AABB"  },
            { L_,   "AB",   { V[1], V[0]       },  "AABB"  },
            { L_,   "AB",   { V[1], V[2]       },  "ABBC"  },
            { L_,   "AB",   { V[2], V[1]       },  "ABBC"  },
            { L_,   "AB",   { V[0], V[1], V[2] },  "AABBC" },
            { L_,   "AB",   { V[2], V[3], V[4] },  "ABCDE" },
        };

        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor dam(&da);

        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            int              LINE   = DATA[ti].d_line;
            const char      *SPEC   = DATA[ti].d_spec;
            const InitList&  LIST   = DATA[ti].d_list;
            const char      *RESULT = DATA[ti].d_result;

            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            Obj mY(&scratch); const Obj& Y = gg(&mY, RESULT);

            Obj mX(&oa); const Obj& X = gg(&mX, SPEC);

            mX.insert(LIST);

            ASSERTV(LINE, X, Y, X == Y);
        }
        ASSERT(dam.isTotalSame());
    }

    {
        typedef bsl::pair<int, Obj> MyPair;

        bslma::TestAllocator         ta("ta", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&ta);

        const MyPair& mp = MyPair(5, { V[0], V[1], V[2] });

        ASSERT(3 == mp.second.size());
    }
#endif  // defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase32_inline()
{
    // ------------------------------------------------------------------------
    // TESTING INITIALIZER LIST C'TORS INLINE
    //
    // Concerns:
    //: 1 That the initializer list constructors can work with an inline
    //:   initializer list.
    //
    // Plan:
    //: 1 Repeat the tests done in 'testCase33_outOfLine', except test only
    //:   with a single, inline constructor list.
    //: 2 There is a bug in the GNU compiler where, if an exception is thrown
    //:   while creating an inline initializer_list, the partially completely
    //:   list is not correctly destroyed and memory can be leaked.  So if
    //:   we're on that compiler, disable the default allocator from throwing
    //:   exceptions (inline initiailizer_list's are always created using the
    //:   default allocator).
    //
    // Testing:
    //   void Obj(initializer_list<Pair>);
    //   void Obj(initializer_list<Pair>, size_t);
    //   void Obj(initializer_list<Pair>, size_t, HASH);
    //   void Obj(initializer_list<Pair>, size_t, HASH, EQUAL);
    //   void Obj(initializer_list<Pair>, const ALLOC&);
    //   void Obj(initializer_list<Pair>, size_t, const ALLOC&);
    //   void Obj(initializer_list<Pair>, size_t, HASH, const ALLOC&);
    //   void Obj(initializer_list<Pair>, size_t, HASH, EQUAL, const ALLOC&);
    //   void insert(initializer_list<Pair>);
    //   Obj& operator=(initializer_list<Pair>);
    // ------------------------------------------------------------------------

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    if (verbose) printf("INITIALIZER_LIST not supported -- no test: %s\n",
                        NameOf<KEY>().name());
#else
    if (verbose) printf(
                "TESTING INITIALIZER LIST C'TORS AND MANIPULATORS INLINE: %s\n"
                "=======================================================\n",
                NameOf<KEY>().name());

    bslma::TestAllocator ta("ta", veryVeryVeryVerbose);    // testValues
    bslma::TestAllocator fa("fa", veryVeryVeryVerbose);    // footprint
    bslma::TestAllocator oa("oa", veryVeryVeryVerbose);    // other
    bslma::TestAllocator da("da", veryVeryVeryVerbose);    // default
    bslma::DefaultAllocatorGuard dag(&da);

    const char   testValuesSpec[] = { "ABC" };
    const size_t testValuesSpecLen = sizeof(testValuesSpec) - 1;

    TestValues  testValues(testValuesSpec, &ta);
    ASSERT(testValues.size() == testValuesSpecLen);

#define u_INIT_LIST                                                           \
              { testValues[1], testValues[0], testValues[2], testValues[0] }

    const TestHashFunctor<KEY> hf(5);
    const TestEqualityComparator<KEY> ec(5);

    bool done = false;
    int totalThrows = 0;
    for (char ctor = 'a'; ctor <= 'h' ; ++ctor) {
        bool ibPassed = false;
        bool hfPassed = false;
        bool ecPassed = false;
        bool oaPassed = false;

        Obj *p = 0;

#ifdef BDE_BUILD_TARGET_EXC
        bslma::TestAllocator& usedAlloc = ctor < 'e' ? da : oa;
#endif
        int numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(usedAlloc) {
            ++numThrows;

#if defined(BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS)
            // There's a bug in 'std::initializer_list' in the GNU g++ compiler
            // which, if a throw occurs while the initializer list is being
            // constructed, it isn't destroyed properly and memory is leaked.
            // To avoid that, do this test without the exceptions if
            // '&usedAlloc == &da'.

            // Therefore, default allocator should never throw on allocations.

            da.setAllocationLimit(-1);
#endif

            switch (ctor) {
              case 'a': {
                  p = new (fa) Obj(u_INIT_LIST);
              } break;
              case 'b': {
                  p = new (fa) Obj(u_INIT_LIST, 1000);
                  ibPassed = true;
              } break;
              case 'c': {
                  p = new (fa) Obj(u_INIT_LIST, 1000, hf);
                  ibPassed = true;
                  hfPassed = true;
              } break;
              case 'd': {
                  p = new (fa) Obj(u_INIT_LIST, 1000, hf, ec);
                  ibPassed = true;
                  hfPassed = true;
                  ecPassed = true;
              } break;
              case 'e': {
                  p = new (fa) Obj(u_INIT_LIST, &oa);
                  oaPassed = true;
              } break;
              case 'f': {
                  p = new (fa) Obj(u_INIT_LIST, 1000, &oa);
                  ibPassed = true;
                  oaPassed = true;
              } break;
              case 'g': {
                  p = new (fa) Obj(u_INIT_LIST, 1000, hf, &oa);
                  ibPassed = true;
                  hfPassed = true;
                  oaPassed = true;
              } break;
              case 'h': {
                  p = new (fa) Obj(u_INIT_LIST, 1000, hf, ec, &oa);
                  ibPassed = true;
                  hfPassed = true;
                  ecPassed = true;
                  oaPassed = true;

                  done = true;
              } break;
              default: {
                  ASSERTV(ctor, 0 && "invalid ctor choice");
              } return;                                               // RETURN
            }

            ASSERT(p);
            ASSERT(!p->empty() && 4 == p->size());
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        const Obj& X = *p;
#ifdef BDE_BUILD_TARGET_EXC
        ASSERTV(NameOf<KEY>(), numThrows, ctor, !oaPassed || 0 < numThrows);
#endif
        totalThrows += numThrows;

        // Make sure parameters either got passed or were default constructed
        // appropriately.

        const size_t bc = X.bucket_count();
        ASSERTV(bc, ibPassed, ibPassed ? (bc > 1000) : (bc < 1000));

        const int hfId = X.hash_function().id();
        ASSERTV(hfId, hfPassed, hfId == (hfPassed ? 5 : 0));

        const int ecId = X.key_eq().id();
        ASSERTV(ecId, ecPassed, ecId == (ecPassed ? 5 : 0));

        ASSERTV(X.get_allocator().mechanism() == (oaPassed ? &oa : &da));
        ASSERTV(oaPassed || 0 == oa.numAllocations());

        // Make sure the container works OK.

        ASSERTV(NameOf<KEY>(), X.size(), 4 == X.size());
        ASSERTV(NameOf<KEY>(), verifySpec(X, "AABC"));

        ASSERTV(!oa.numBlocksInUse() != !da.numBlocksInUse());

        fa.deleteObjectRaw(p);

        ASSERTV(ctor, 0 == fa.numBlocksInUse());
        ASSERTV(ctor, 0 == oa.numBlocksInUse());
        ASSERTV(ctor, 0 == da.numBlocksInUse());
    }

    ASSERTV(fa.numBlocksTotal() == 'h' + 1 - 'a' + totalThrows);

    {
        Obj mX(&oa);    const Obj& X = gg(&mX, "CDE");

        mX.insert(u_INIT_LIST);

        ASSERTV(X, verifySpec(X, "AABCCDE"));
    }

    {
        Obj mX(&oa);    const Obj& X = gg(&mX, "CDE");
        Obj Y(X, &oa);

        int numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
            ++numThrows;

            CompareProctor<Obj> proctor(Y, X, L_);

            Obj *ret = &(mX = u_INIT_LIST);
            ASSERT(&X == ret);

            proctor.release();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        ASSERTV(!TYPE_ALLOC || !PLAT_EXC || 0 < numThrows);
        ASSERTV(X, verifySpec(X, "AABC"));
        ASSERTV(Y, verifySpec(Y, "CDE"));
    }

    {
        typedef bsl::pair<int, Obj> MyPair;

        bslma::DefaultAllocatorGuard dag(&ta);

        const MyPair& mp = MyPair(5, u_INIT_LIST);

        ASSERTV(mp.second, 4 == mp.second.size());
    }

#undef u_INIT_LIST

    ASSERT(done);
#endif
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase31a()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH EMPLACE WITH HINT
    //
    // Concerns:
    //: 1 'emplace_hint' correctly forwards arguments to the constructor of the
    //:   value type, up to 3 arguments, the max number of arguments provided
    //:   for C++03 compatibility.  Note that only the forwarding of arguments
    //:   is tested in this function; all other functionality is tested in
    //:  'testCase31'.
    //:
    //: 2 'emplace_hint' is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 This test makes material use of template method 'testCase31a_RunTest'
    //:   with first integer template parameter indicating the number of
    //:   arguments to use, the next 3 integer template parameters indicating
    //:   '0' for copy, '1' for move, and '2' for not-applicable (i.e., beyond
    //:   the number of arguments), and taking as arguments a pointer to a
    //:   modifiable container and a hint.
    //:   1 Create 10 argument values with their own argument-specific
    //:     allocator.
    //:
    //:   2 Based on the (first) template parameter indicating the number of
    //:     arguments to pass in, call 'emplace_hint' with the hint passed in
    //:     as an argument and the corresponding constructor argument values,
    //:     performing an explicit move of the argument if so indicated by
    //:     the template parameter corresponding to the argument, all of this
    //:     in the presence of injected exceptions.
    //:
    //:   3 Verify that the argument values were passed correctly.
    //:
    //:   4 Verify that the allocator was forwarded correctly.
    //:
    //:   5 Verify that the move-state for each argument is as expected.
    //:
    //:   6 If the object did not contain the emplaced value, verify it now
    //:     exists.  Otherwise, verify the return value is as expected.
    //:
    //:   7 Return the iterator returned by the call to 'emplace_hint'.
    //:
    //: 2 Create a container with it's own object-specific allocator.
    //:
    //: 3 Call 'testCase31a_RunTest' in various configurations:
    //:   1 For 1..3 arguments, call with the move flag set to '1' and then
    //:     with the move flag set to '0'.
    //:
    //:   2 For 1, 2, 3 arguments, call with move flags set to '0',
    //:     '1', and each move flag set independently.
    //
    // Testing:
    //   iterator emplace_hint(const_iterator hint, Args&&... args);
    // ------------------------------------------------------------------------

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
    if (verbose) printf("EMPLACE WITH HINT: not tested\n");
#else
    if (verbose) printf(
                 "TESTING FORWARDING OF ARGUMENTS WITH EMPLACE WITH HINT\n"
                 "------------------------------------------------------\n");
    if (veryVerbose) { P(NameOf<KEY>().name()) P(NameOf<VALUE>().name()) }

    Iter hint;

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
    if (verbose) printf("\nTesting emplace 1..3 args, move=1"
                        "\n----------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa); const Obj& X = mX;

        hint = testCase31a_RunTest< 0,2,2,2,0,2,2,2>(&mX, X.end());
        hint = testCase31a_RunTest< 1,1,2,2,0,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 2,1,1,2,0,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 3,1,1,1,0,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 0,2,2,2,1,1,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 1,1,2,2,1,1,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 2,1,1,2,1,1,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 3,1,1,1,1,1,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 0,2,2,2,2,1,1,2>(&mX, hint);
        hint = testCase31a_RunTest< 1,1,2,2,2,1,1,2>(&mX, hint);
        hint = testCase31a_RunTest< 2,1,1,2,2,1,1,2>(&mX, hint);
        hint = testCase31a_RunTest< 3,1,1,1,2,1,1,2>(&mX, hint);
        hint = testCase31a_RunTest< 0,2,2,2,3,1,1,1>(&mX, hint);
        hint = testCase31a_RunTest< 1,1,2,2,3,1,1,1>(&mX, hint);
        hint = testCase31a_RunTest< 2,1,1,2,3,1,1,1>(&mX, hint);
        hint = testCase31a_RunTest< 3,1,1,1,3,1,1,1>(&mX, hint);
    }

    if (verbose) printf("\nTesting emplace 1..3 args, move=0"
                        "\n----------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa); const Obj& X = mX;

        hint = testCase31a_RunTest< 0,2,2,2,0,2,2,2>(&mX, X.end());
        hint = testCase31a_RunTest< 1,0,2,2,0,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 2,0,0,2,0,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 3,0,0,0,0,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 0,2,2,2,1,0,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 1,0,2,2,1,0,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 2,0,0,2,1,0,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 3,0,0,0,1,0,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 0,2,2,2,2,0,0,2>(&mX, hint);
        hint = testCase31a_RunTest< 1,0,2,2,2,0,0,2>(&mX, hint);
        hint = testCase31a_RunTest< 2,0,0,2,2,0,0,2>(&mX, hint);
        hint = testCase31a_RunTest< 3,0,0,0,2,0,0,2>(&mX, hint);
        hint = testCase31a_RunTest< 0,2,2,2,3,0,0,0>(&mX, hint);
        hint = testCase31a_RunTest< 1,0,2,2,3,0,0,0>(&mX, hint);
        hint = testCase31a_RunTest< 2,0,0,2,3,0,0,0>(&mX, hint);
        hint = testCase31a_RunTest< 3,0,0,0,3,0,0,0>(&mX, hint);
    }

    if (verbose) printf("\nTesting emplace with 0 args"
                        "\n---------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa); const Obj& X = mX;

        hint = testCase31a_RunTest<0,2,2,2,0,2,2,2>(&mX, X.end());
    }

    if (verbose) printf("\nTesting emplace with 1 args"
                        "\n---------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa); const Obj& X = mX;

        hint = testCase31a_RunTest<0,2,2,2,1,0,2,2>(&mX, X.end());
        hint = testCase31a_RunTest<0,2,2,2,1,1,2,2>(&mX, hint);
        hint = testCase31a_RunTest<1,0,2,2,0,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest<1,1,2,2,0,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest<1,0,2,2,1,0,2,2>(&mX, hint);
        hint = testCase31a_RunTest<1,1,2,2,1,0,2,2>(&mX, hint);
        hint = testCase31a_RunTest<1,0,2,2,1,1,2,2>(&mX, hint);
        hint = testCase31a_RunTest<1,1,2,2,1,1,2,2>(&mX, hint);
    }

    if (verbose) printf("\nTesting emplace with 2 args"
                        "\n---------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa); const Obj& X = mX;

        hint = testCase31a_RunTest<0,2,2,2,2,0,0,2>(&mX, X.end());
        hint = testCase31a_RunTest<0,2,2,2,2,0,1,2>(&mX, hint);
        hint = testCase31a_RunTest<0,2,2,2,2,1,0,2>(&mX, hint);
        hint = testCase31a_RunTest<0,2,2,2,2,1,1,2>(&mX, hint);

        hint = testCase31a_RunTest<2,0,0,2,0,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest<2,0,1,2,0,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest<2,1,0,2,0,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest<2,1,1,2,0,2,2,2>(&mX, hint);

        hint = testCase31a_RunTest<2,0,0,2,2,0,0,2>(&mX, hint);
        hint = testCase31a_RunTest<2,0,1,2,2,0,0,2>(&mX, hint);
        hint = testCase31a_RunTest<2,1,0,2,2,0,0,2>(&mX, hint);
        hint = testCase31a_RunTest<2,1,1,2,2,0,0,2>(&mX, hint);

        hint = testCase31a_RunTest<2,0,0,2,2,0,1,2>(&mX, hint);
        hint = testCase31a_RunTest<2,0,1,2,2,0,1,2>(&mX, hint);
        hint = testCase31a_RunTest<2,1,0,2,2,0,1,2>(&mX, hint);
        hint = testCase31a_RunTest<2,1,1,2,2,0,1,2>(&mX, hint);

        hint = testCase31a_RunTest<2,0,0,2,2,1,0,2>(&mX, hint);
        hint = testCase31a_RunTest<2,0,1,2,2,1,0,2>(&mX, hint);
        hint = testCase31a_RunTest<2,1,0,2,2,1,0,2>(&mX, hint);
        hint = testCase31a_RunTest<2,1,1,2,2,1,0,2>(&mX, hint);

        hint = testCase31a_RunTest<2,0,0,2,2,1,1,2>(&mX, hint);
        hint = testCase31a_RunTest<2,0,1,2,2,1,1,2>(&mX, hint);
        hint = testCase31a_RunTest<2,1,0,2,2,1,1,2>(&mX, hint);
        hint = testCase31a_RunTest<2,1,1,2,2,1,1,2>(&mX, hint);
    }

    if (verbose) printf("\nTesting emplace with 3 args"
                        "\n----------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa); const Obj& X = mX;

        hint = testCase31a_RunTest<3,0,0,0,3,0,0,0>(&mX, X.end());
        hint = testCase31a_RunTest<3,1,0,0,3,0,0,0>(&mX, hint);
        hint = testCase31a_RunTest<3,0,1,0,3,0,0,0>(&mX, hint);
        hint = testCase31a_RunTest<3,0,0,1,3,0,0,0>(&mX, hint);
        hint = testCase31a_RunTest<3,0,0,0,3,1,0,0>(&mX, hint);
        hint = testCase31a_RunTest<3,1,0,0,3,1,0,0>(&mX, hint);
        hint = testCase31a_RunTest<3,0,1,0,3,1,0,0>(&mX, hint);
        hint = testCase31a_RunTest<3,0,0,1,3,1,0,0>(&mX, hint);
        hint = testCase31a_RunTest<3,0,0,0,3,0,1,0>(&mX, hint);
        hint = testCase31a_RunTest<3,1,0,0,3,0,1,0>(&mX, hint);
        hint = testCase31a_RunTest<3,0,1,0,3,0,1,0>(&mX, hint);
        hint = testCase31a_RunTest<3,0,0,1,3,0,1,0>(&mX, hint);
        hint = testCase31a_RunTest<3,0,0,0,3,0,0,1>(&mX, hint);
        hint = testCase31a_RunTest<3,1,0,0,3,0,0,1>(&mX, hint);
        hint = testCase31a_RunTest<3,0,1,0,3,0,0,1>(&mX, hint);
        hint = testCase31a_RunTest<3,0,0,1,3,0,0,1>(&mX, hint);
        hint = testCase31a_RunTest<3,1,1,1,3,1,1,1>(&mX, hint);
    }
#else
    if (verbose) printf("\nTesting emplace 0..3 args, move=0"
                        "\n----------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa); const Obj& X = mX;

        hint = testCase31a_RunTest< 0,2,2,2,0,2,2,2>(&mX, X.end());
        hint = testCase31a_RunTest< 1,0,2,2,0,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 2,0,0,2,0,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 3,0,0,0,0,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 0,2,2,2,1,0,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 1,0,2,2,1,0,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 2,0,0,2,1,0,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 3,0,0,0,1,0,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 0,2,2,2,2,0,0,2>(&mX, hint);
        hint = testCase31a_RunTest< 1,0,2,2,2,0,0,2>(&mX, hint);
        hint = testCase31a_RunTest< 2,0,0,2,2,0,0,2>(&mX, hint);
        hint = testCase31a_RunTest< 3,0,0,0,2,0,0,2>(&mX, hint);
        hint = testCase31a_RunTest< 0,2,2,2,3,0,0,0>(&mX, hint);
        hint = testCase31a_RunTest< 1,0,2,2,3,0,0,0>(&mX, hint);
        hint = testCase31a_RunTest< 2,0,0,2,3,0,0,0>(&mX, hint);
        hint = testCase31a_RunTest< 3,0,0,0,3,0,0,0>(&mX, hint);
    }
#endif
#endif
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase31()
{
    // ------------------------------------------------------------------------
    // TESTING EMPLACE WITH HINT
    //
    // Concerns:
    //: 1 'emplace_hint' returns an iterator referring to the newly inserted
    //:   element.
    //:
    //: 2 A new element is added to the container at the end of the range
    //:   containing equivalent elements.
    //:
    //: 3 Inserting with the correct hint places the new element right before
    //:   the hint.
    //:
    //: 4 Incorrect hint will be ignored and 'insert' will proceed as if the
    //:   hint is not supplied.
    //:
    //: 5 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 6 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 We will use 'value' as the single argument to the 'emplace_hint'
    //:   function and will test proper forwarding of constructor arguments in
    //:   test 'testCase31a'.
    //:
    //: 2 For insertion we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 For each set of values, set hint to be 'begin', 'begin' + 1, 'end'
    //:     and 'find(key)'.
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
    //:       5 Verify all allocations are from the object's allocator.  (C-5)
    //:
    //: 3 Repeat P-2 under the presence of exception  (C-6)
    //
    // Testing:
    //   iterator emplace_hint(const_iterator position, Args&&... args);
    // ------------------------------------------------------------------------

    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
        const char *d_unique;  // expected element values
    } DATA[] = {
        //line  spec           isUnique
        //----  -------------- --------------

        { L_,   "A",           "Y"             },
        { L_,   "AAA",         "YNN"           },
        { L_,   "ABCDEFGH",    "YYYYYYYY"      },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"    },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY"   },
    };
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    const size_t MAX_LENGTH = 16;

    if (verbose) printf("\nTesting 'emplace_hint'.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                bsl::allocator<ValueType> soa(&oa);

                Obj mX(soa); const Obj &X = mX;

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                    EXPECTED[SIZE] = SPEC[tj];
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }

                    CIter hint;
                    switch(CONFIG) {
                      case 'a': {
                          hint = X.begin();
                      } break;
                      case 'b': {
                          hint = X.begin();
                          if (hint != X.end()) {
                              ++hint;
                          }
                      } break;
                      case 'c': {
                          hint = X.end();
                      } break;
                      case 'd': {
                          hint = X.find(VALUES[tj].first);
                          ASSERTV(IS_UNIQ == (hint == X.end()));
                      } break;
                      default: {
                          ASSERTV(!"Unexpected configuration");
                      }
                    }

                    Iter RESULT = mX.emplace_hint(hint, VALUES[tj]);

                    if (CONFIG == 'd' && hint != X.end()) {
                        ASSERTV(LINE, tj, hint == ++RESULT);
                    }

                    ASSERTV(LINE, CONFIG, tj, SIZE, VALUES[tj] == *RESULT);
                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());
                    ASSERTV(LINE, tj, verifySpec(X, EXPECTED));
                }
            }
        }
    }
    if (verbose) printf("\nTesting 'emplace_hint' with exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                bsl::allocator<ValueType> soa(&oa);

                Obj mX(soa); const Obj &X = mX;

                for (int tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                    EXPECTED[SIZE] = SPEC[tj];
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }

                    CIter hint;
                    switch(CONFIG) {
                      case 'a': {
                          hint = X.begin();
                      } break;
                      case 'b': {
                          hint = X.begin();
                          if (hint != X.end()) {
                              ++hint;
                          }
                      } break;
                      case 'c': {
                          hint = X.end();
                      } break;
                      case 'd': {
                          hint = X.find(VALUES[tj].first);
                          ASSERTV(IS_UNIQ == (hint == X.end()));
                      } break;
                      default: {
                          ASSERTV(!"Unexpected configuration");
                      }
                    }

                    bslma::TestAllocator scratch("scratch",
                                                 veryVeryVeryVerbose);
                    bsl::allocator<ValueType> sscratch(&scratch);

                    Iter RESULT;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ExceptionProctor<Obj, ALLOC> guard(&X, L_, sscratch);

                        RESULT = mX.emplace_hint(hint, VALUES[tj]);
                        guard.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    if (CONFIG == 'd' && hint != X.end()) {
                        ASSERTV(LINE, tj, hint == ++RESULT);
                    }

                    ASSERTV(LINE, CONFIG, tj, SIZE, VALUES[tj] == *RESULT);
                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());
                    ASSERTV(LINE, tj, verifySpec(X, EXPECTED));
                }
            }
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase30a()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH EMPLACE
    //
    // Concerns:
    //: 1 'emplace' correctly forwards arguments to the piecewise constructor
    //:   of the value type, up to 3 arguments.  Note that only the forwarding
    //:   of arguments is tested in this function; all other functionality is
    //:   tested in 'testCase30'.
    //:
    //: 2 'emplace' is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 This test makes material use of template method 'testCase30a_RunTest'
    //:   with first integer template parameter indicating the number of
    //:   arguments to use, the next 3 integer template parameters indicating
    //:   '0' for copy, '1' for move, and '2' for not-applicable (i.e., beyond
    //:   the number of arguments), and taking as the only argument a pointer
    //:   to a modifiable container.
    //:   1 Create 3 argument values with their own argument-specific
    //:     allocator.
    //:
    //:   2 Based on the (first) template parameter indicating the number of
    //:     arguments to pass in, call 'emplace' with the corresponding
    //:     argument values, performing an explicit move of the argument if so
    //:     indicated by the template parameter corresponding to the argument,
    //:     all in the presence of injected exceptions.
    //:
    //:   3 Verify that the argument values were passed correctly.
    //:
    //:   4 Verify that the allocator was forwarded correctly.
    //:
    //:   5 Verify that the move-state for each argument is as expected.
    //:
    //:   6 If the object did not contain the emplaced value, verify it now
    //:     exists.  Otherwise, verify the return value is as expected.
    //:
    //: 2 Create a container with it's own object-specific allocator.
    //:
    //: 3 Call 'testCase30a_RunTest' in various configurations:
    //:   1 For 1..3 arguments, call with the move flag set to '1' and then
    //:     with the move flag set to '0'.
    //:
    //:   2 For 1..3, call with move flags set to '0', '1', and each move flag
    //:     set independently.
    //
    // Testing:
    //   iterator emplace(Args&&... args);
    // ------------------------------------------------------------------------

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
    if (verbose) printf("Testcase31a -- no test\n");
#else
    if (verbose) printf("TESTING FORWARDING OF ARGUMENTS WITH EMPLACE\n"
                        "--------------------------------------------\n");

    if (veryVerbose) { P(NameOf<KEY>().name()) P(NameOf<VALUE>().name()) }

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
    if (verbose) printf("\nTesting emplace 1..3 args, move=1"
                        "\n----------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);

        testCase30a_RunTest< 0,2,2,2,0,2,2,2>(&mX);
        testCase30a_RunTest< 1,1,2,2,0,2,2,2>(&mX);
        testCase30a_RunTest< 2,1,1,2,0,2,2,2>(&mX);
        testCase30a_RunTest< 3,1,1,1,0,2,2,2>(&mX);
        testCase30a_RunTest< 0,2,2,2,1,1,2,2>(&mX);
        testCase30a_RunTest< 1,1,2,2,1,1,2,2>(&mX);
        testCase30a_RunTest< 2,1,1,2,1,1,2,2>(&mX);
        testCase30a_RunTest< 3,1,1,1,1,1,2,2>(&mX);
        testCase30a_RunTest< 0,2,2,2,2,1,1,2>(&mX);
        testCase30a_RunTest< 1,1,2,2,2,1,1,2>(&mX);
        testCase30a_RunTest< 2,1,1,2,2,1,1,2>(&mX);
        testCase30a_RunTest< 3,1,1,1,2,1,1,2>(&mX);
        testCase30a_RunTest< 0,2,2,2,3,1,1,1>(&mX);
        testCase30a_RunTest< 1,1,2,2,3,1,1,1>(&mX);
        testCase30a_RunTest< 2,1,1,2,3,1,1,1>(&mX);
        testCase30a_RunTest< 3,1,1,1,3,1,1,1>(&mX);
    }

    if (verbose) printf("\nTesting emplace 1..3 args, move=0"
                        "\n----------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);

        testCase30a_RunTest< 0,2,2,2,0,2,2,2>(&mX);
        testCase30a_RunTest< 1,0,2,2,0,2,2,2>(&mX);
        testCase30a_RunTest< 2,0,0,2,0,2,2,2>(&mX);
        testCase30a_RunTest< 3,0,0,0,0,2,2,2>(&mX);
        testCase30a_RunTest< 0,2,2,2,1,0,2,2>(&mX);
        testCase30a_RunTest< 1,0,2,2,1,0,2,2>(&mX);
        testCase30a_RunTest< 2,0,0,2,1,0,2,2>(&mX);
        testCase30a_RunTest< 3,0,0,0,1,0,2,2>(&mX);
        testCase30a_RunTest< 0,2,2,2,2,0,0,2>(&mX);
        testCase30a_RunTest< 1,0,2,2,2,0,0,2>(&mX);
        testCase30a_RunTest< 2,0,0,2,2,0,0,2>(&mX);
        testCase30a_RunTest< 3,0,0,0,2,0,0,2>(&mX);
        testCase30a_RunTest< 0,2,2,2,3,0,0,0>(&mX);
        testCase30a_RunTest< 1,0,2,2,3,0,0,0>(&mX);
        testCase30a_RunTest< 2,0,0,2,3,0,0,0>(&mX);
        testCase30a_RunTest< 3,0,0,0,3,0,0,0>(&mX);
    }

    if (verbose) printf("\nTesting emplace with 0 args"
                        "\n---------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);

        testCase30a_RunTest<0,2,2,2,0,2,2,2>(&mX);
    }

    if (verbose) printf("\nTesting emplace with 1 args"
                        "\n---------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);

        testCase30a_RunTest<0,2,2,2,1,0,2,2>(&mX);
        testCase30a_RunTest<0,2,2,2,1,1,2,2>(&mX);
        testCase30a_RunTest<1,0,2,2,0,2,2,2>(&mX);
        testCase30a_RunTest<1,1,2,2,0,2,2,2>(&mX);
        testCase30a_RunTest<1,0,2,2,1,0,2,2>(&mX);
        testCase30a_RunTest<1,1,2,2,1,0,2,2>(&mX);
        testCase30a_RunTest<1,0,2,2,1,1,2,2>(&mX);
        testCase30a_RunTest<1,1,2,2,1,1,2,2>(&mX);
    }

    if (verbose) printf("\nTesting emplace with 2 args"
                        "\n---------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);

        testCase30a_RunTest<0,2,2,2,2,0,0,2>(&mX);
        testCase30a_RunTest<0,2,2,2,2,0,1,2>(&mX);
        testCase30a_RunTest<0,2,2,2,2,1,0,2>(&mX);
        testCase30a_RunTest<0,2,2,2,2,1,1,2>(&mX);

        testCase30a_RunTest<2,0,0,2,0,2,2,2>(&mX);
        testCase30a_RunTest<2,0,1,2,0,2,2,2>(&mX);
        testCase30a_RunTest<2,1,0,2,0,2,2,2>(&mX);
        testCase30a_RunTest<2,1,1,2,0,2,2,2>(&mX);

        testCase30a_RunTest<2,0,0,2,2,0,0,2>(&mX);
        testCase30a_RunTest<2,0,1,2,2,0,0,2>(&mX);
        testCase30a_RunTest<2,1,0,2,2,0,0,2>(&mX);
        testCase30a_RunTest<2,1,1,2,2,0,0,2>(&mX);

        testCase30a_RunTest<2,0,0,2,2,0,1,2>(&mX);
        testCase30a_RunTest<2,0,1,2,2,0,1,2>(&mX);
        testCase30a_RunTest<2,1,0,2,2,0,1,2>(&mX);
        testCase30a_RunTest<2,1,1,2,2,0,1,2>(&mX);

        testCase30a_RunTest<2,0,0,2,2,1,0,2>(&mX);
        testCase30a_RunTest<2,0,1,2,2,1,0,2>(&mX);
        testCase30a_RunTest<2,1,0,2,2,1,0,2>(&mX);
        testCase30a_RunTest<2,1,1,2,2,1,0,2>(&mX);

        testCase30a_RunTest<2,0,0,2,2,1,1,2>(&mX);
        testCase30a_RunTest<2,0,1,2,2,1,1,2>(&mX);
        testCase30a_RunTest<2,1,0,2,2,1,1,2>(&mX);
        testCase30a_RunTest<2,1,1,2,2,1,1,2>(&mX);
    }

    if (verbose) printf("\nTesting emplace with 3 args"
                        "\n----------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);

        testCase30a_RunTest<3,0,0,0,3,0,0,0>(&mX);
        testCase30a_RunTest<3,1,0,0,3,0,0,0>(&mX);
        testCase30a_RunTest<3,0,1,0,3,0,0,0>(&mX);
        testCase30a_RunTest<3,0,0,1,3,0,0,0>(&mX);
        testCase30a_RunTest<3,0,0,0,3,1,0,0>(&mX);
        testCase30a_RunTest<3,1,0,0,3,1,0,0>(&mX);
        testCase30a_RunTest<3,0,1,0,3,1,0,0>(&mX);
        testCase30a_RunTest<3,0,0,1,3,1,0,0>(&mX);
        testCase30a_RunTest<3,0,0,0,3,0,1,0>(&mX);
        testCase30a_RunTest<3,1,0,0,3,0,1,0>(&mX);
        testCase30a_RunTest<3,0,1,0,3,0,1,0>(&mX);
        testCase30a_RunTest<3,0,0,1,3,0,1,0>(&mX);
        testCase30a_RunTest<3,0,0,0,3,0,0,1>(&mX);
        testCase30a_RunTest<3,1,0,0,3,0,0,1>(&mX);
        testCase30a_RunTest<3,0,1,0,3,0,0,1>(&mX);
        testCase30a_RunTest<3,0,0,1,3,0,0,1>(&mX);
        testCase30a_RunTest<3,1,1,1,3,1,1,1>(&mX);
    }
#else
    if (verbose) printf("\nTesting emplace 0..3 args, move=0"
                        "\n----------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);

        testCase30a_RunTest< 0,2,2,2,0,2,2,2>(&mX);
        testCase30a_RunTest< 1,0,2,2,0,2,2,2>(&mX);
        testCase30a_RunTest< 2,0,0,2,0,2,2,2>(&mX);
        testCase30a_RunTest< 3,0,0,0,0,2,2,2>(&mX);
        testCase30a_RunTest< 0,2,2,2,1,0,2,2>(&mX);
        testCase30a_RunTest< 1,0,2,2,1,0,2,2>(&mX);
        testCase30a_RunTest< 2,0,0,2,1,0,2,2>(&mX);
        testCase30a_RunTest< 3,0,0,0,1,0,2,2>(&mX);
        testCase30a_RunTest< 0,2,2,2,2,0,0,2>(&mX);
        testCase30a_RunTest< 1,0,2,2,2,0,0,2>(&mX);
        testCase30a_RunTest< 2,0,0,2,2,0,0,2>(&mX);
        testCase30a_RunTest< 3,0,0,0,2,0,0,2>(&mX);
        testCase30a_RunTest< 0,2,2,2,3,0,0,0>(&mX);
        testCase30a_RunTest< 1,0,2,2,3,0,0,0>(&mX);
        testCase30a_RunTest< 2,0,0,2,3,0,0,0>(&mX);
        testCase30a_RunTest< 3,0,0,0,3,0,0,0>(&mX);
    }
#endif
#endif
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase30()
{
    // ------------------------------------------------------------------------
    // TESTING EMPLACE
    //
    // Concerns:
    //: 1 The iterator returned refers to the newly inserted element.
    //:
    //: 2 A new element is added to the container and the order of the
    //:   container remains correct.
    //:
    //: 3 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 4 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 We will use 'value' as the single argument to the 'emplace' function
    //:   and will test proper forwarding of constructor arguments in test
    //:   'testCase30a'.
    //:
    //: 2 For emplace, we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 Compute the number of allocations and verify it is as expected.
    //:
    //:   2 If the object did not contain 'value', verify it now exist.
    //:     Otherwise, verify the return value is as expected.  (C-1..2)
    //:
    //:   3 Verify all allocations are from the object's allocator.  (C-3)
    //:
    //: 3 Repeat P-2 under the presence of exception  (C-4)
    //
    // Testing:
    //   iterator emplace(Args&&... arguments);
    // -----------------------------------------------------------------------

    if (veryVerbose) { P(NameOf<KEY>().name()) P(NameOf<VALUE>().name()) }

    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
    } DATA[] = {
        //line  spec
        //----  --------------

        { L_,   "A",            },
        { L_,   "AAA",          },
        { L_,   "AAB",          },
        { L_,   "ABA",          },
        { L_,   "ABB",          },
        { L_,   "BAA",          },
        { L_,   "ABCDEFGH",     },
        { L_,   "EEDDCCBBAA",   },
        { L_,   "ABCDEABCDEF",  },
    };
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    const int MAX_LENGTH = 16;

    if (verbose) printf("\nTesting 'emplace' without exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P(LENGTH); }

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bsl::allocator<ValueType> soa(&oa);

            Obj mX(soa); const Obj &X = mX;

            for (int tj = 0; tj < LENGTH; ++tj) {
                const size_t SIZE    = X.size();

                if (veryVerbose) { P(SIZE); }

                EXPECTED[SIZE] = SPEC[tj];
                EXPECTED[SIZE + 1] = '\0';

                if (veryVeryVerbose) { P(EXPECTED); }

                Iter RESULT = mX.emplace(VALUES[tj]);

                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);

                ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());
                ASSERTV(LINE, tj, verifySpec(X, EXPECTED));
            }
        }
    }
    if (verbose) printf("\nTesting 'emplace' with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P(LENGTH); }

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bsl::allocator<ValueType> soa(&oa);

            Obj mX(soa); const Obj &X = mX;

            for (int tj = 0; tj < LENGTH; ++tj) {
                const size_t SIZE    = X.size();

                if (veryVerbose) { P(SIZE); }

                EXPECTED[SIZE] = SPEC[tj];
                EXPECTED[SIZE + 1] = '\0';

                if (veryVeryVerbose) { P(EXPECTED); }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                bsl::allocator<ValueType> sscratch(&scratch);

                Iter RESULT;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionProctor<Obj, ALLOC> guard(&X, L_, sscratch);

                    RESULT = mX.emplace(VALUES[tj]);

                    guard.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);
                ASSERTV(LINE, tj, verifySpec(X, EXPECTED));
            }
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase29()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION WITH HINT ON MOVABLE VALUES
    //
    // Concerns:
    //: 1 'insert' returns an iterator referring to the newly inserted element.
    //:
    //: 2 Inserting with a correct hint places the new element right before the
    //:   hint.
    //:
    //: 3 An incorrect hint will be ignored and 'insert' will proceed as if the
    //:   hint is not supplied.
    //:
    //: 4 The new element is move-inserted into the container.
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
    //:   1 For each set of values, set hint to be 'begin', 'begin' + 1, 'end'
    //:     and 'find(key)'.
    //:
    //:     1 For each value in the set, 'insert' the value with hint.
    //:
    //:       1 Verify the return value and the resulting data in the container
    //:         is as expected.  (C-1)
    //:
    //:       2 Verify the new element is inserted right before the hint if
    //:         the hint is valid.  (C-2,3)
    //:
    //:       3 Ensure that the move constructor was called on the value type
    //:         (if that type has a mechanism to detect such) where the value
    //:         has the same allocator as that of the container and a different
    //:         allocator than that of the container.  (C-4)
    //:
    //:       4 Verify all allocations are from the object's allocator.  (C-5)
    //:
    //: 2 Repeat P-1 under the presence of exception  (C-6)
    //
    // Testing:
    //   iterator insert(const_iterator hint, value_type&& value);
    // -----------------------------------------------------------------------

    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
        const char *d_unique;  // expected element values
    } DATA[] = {
        //line  spec           unique
        //----  -------------- --------------
        { L_,   "A",           "Y"            },
        { L_,   "AAA",         "YNN"          },
        { L_,   "ABCDEFGH",    "YYYYYYYY"     },
        { L_,   "AABBCCDDEE",  "YNYNYNYNYN"   },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"   },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY"  },
    };
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    const int MAX_LENGTH = 16;

    if (verbose) printf("\nTesting 'insert' with hint.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                for (char cfg2 = 'a'; cfg2 <= 'b'; ++cfg2) {

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bsl::allocator<ValueType> soa(&oa);

                    Obj mX(soa);  const Obj &X = mX;

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
                        ASSERTV(cfg2, !"Bad allocator config.");
                      } break;
                    }
                    bslma::TestAllocator& sa = *valAllocator;
                    bsl::allocator<ValueType> ssa(&sa);

                    for (size_t tj = 0; tj < LENGTH; ++tj) {
                        const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                        const size_t SIZE    = X.size();

                        if (veryVeryVerbose) { P(SIZE); }

                        EXPECTED[SIZE] = SPEC[tj];
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { P(EXPECTED); }

                        CIter hint;
                        switch(CONFIG) {
                          case 'a': {
                              hint = X.begin();
                          } break;
                          case 'b': {
                              hint = X.begin();
                              if (hint != X.end()) {
                                  ++hint;
                              }
                          } break;
                          case 'c': {
                              hint = X.end();
                          } break;
                          case 'd': {
                              hint = X.find(VALUES[tj].first);
                              ASSERTV(IS_UNIQ == (hint == X.end()));
                          } break;
                          default: {
                              ASSERTV(CONFIG, !"Unexpected configuration");
                          }
                        }

                        bsls::ObjectBuffer<TValueType> buffer;
                        TValueType *valptr = buffer.address();

                        IntToPairConverter<KEY, VALUE, ALLOC>::createInplace(
                                  valptr,
                                  TstFacility::getIdentifier(VALUES[tj].first),
                                  ALLOC(ssa));

                        Iter RESULT = mX.insert(hint, MoveUtil::move(*valptr));
                        MoveState::Enum mStateFirst =
                                TstFacility::getMovedFromState(valptr->first);
                        MoveState::Enum mStateSecond =
                                TstFacility::getMovedFromState(valptr->second);
                        bslma::DestructionUtil::destroy(valptr);

                        ASSERTV(LINE, CONFIG, tj, SIZE, VALUES[tj] == *RESULT);

                        ASSERTV(LINE, CONFIG, tj, SIZE,
                                VALUES[tj].first == RESULT->first);
                        ASSERTV(LINE, CONFIG, tj, SIZE,
                                VALUES[tj].second == RESULT->second);

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
                                                    expKeyMove == mStateFirst);
                        ASSERTV(mStateSecond, expValueMove, NameOf<VALUE>(),
                                                 expValueMove == mStateSecond);

                        ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                        if (CONFIG == 'd' && hint != X.end()) {
                            ASSERTV(LINE, tj, hint == ++RESULT);
                        }

                        ASSERTV(LINE, tj, verifySpec(X, EXPECTED));
                    }
                }
            }
        }
    }
    if (verbose) printf("\nTesting 'insert' with exception.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                const char CONFIG = cfg;

                for (char cfg2 = 'a'; cfg2 <= 'b'; ++cfg2) {

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bsl::allocator<ValueType> soa(&oa);

                    Obj mX(soa);  const Obj &X = mX;

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
                        ASSERTV(cfg2, !"Bad allocator config.");
                      } break;
                    }
                    bslma::TestAllocator& sa = *valAllocator;
                    bsl::allocator<ValueType> ssa(&sa);

                    for (size_t tj = 0; tj < LENGTH; ++tj) {
                        const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                        const size_t SIZE    = X.size();

                        if (veryVeryVerbose) { P(SIZE); }

                        EXPECTED[SIZE] = SPEC[tj];
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { P(EXPECTED); }

                        CIter hint;
                        switch(CONFIG) {
                          case 'a': {
                              hint = X.begin();
                          } break;
                          case 'b': {
                              hint = X.begin();
                              if (hint != X.end()) {
                                  ++hint;
                              }
                          } break;
                          case 'c': {
                              hint = X.end();
                          } break;
                          case 'd': {
                              hint = X.find(VALUES[tj].first);
                              ASSERTV(IS_UNIQ == (hint == X.end()));
                          } break;
                          default: {
                              ASSERTV(CONFIG, !"Unexpected configuration");
                          }
                        }

                        bslma::TestAllocator scratch("scratch",
                                                    veryVeryVeryVerbose);
                        bsl::allocator<ValueType> sscratch(&scratch);

                        Iter RESULT;
                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                            Obj mZ(sscratch); const Obj& Z = mZ;
                            for (size_t tk = 0; tk < tj; ++tk) {
                                primaryManipulator(
                                 &mZ,
                                 TstFacility::getIdentifier(VALUES[tk].first),
                                 sscratch);
                            }
                            ASSERTV(Z, X, Z == X);

                            ExceptionProctor<Obj, ALLOC> proctor(&X, L_,
                                                           MoveUtil::move(mZ));

                            bsls::ObjectBuffer<TValueType> buffer;
                            TValueType *valptr = buffer.address();

                            IntToPairConverter<KEY, VALUE, ALLOC>::
                                createInplace(
                                  valptr,
                                  TstFacility::getIdentifier(VALUES[tj].first),
                                  ALLOC(ssa));

                            bslma::DestructorGuard<TValueType> guard(valptr);

                            RESULT = mX.insert(hint, MoveUtil::move(*valptr));

                            proctor.release();

                            ASSERTV(LINE, CONFIG, tj, SIZE,
                                                        VALUES[tj] == *RESULT);

                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                        ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                        if (CONFIG == 'd' && hint != X.end()) {
                            ASSERTV(LINE, tj, hint == ++RESULT);
                        }

                        ASSERTV(LINE, tj, verifySpec(X, EXPECTED));
                    }
                }
            }
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase28()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION ON MOVABLE VALUES
    //
    // Concerns:
    //: 1 The iterator returned refers to the newly inserted element.
    //:
    //: 2 The new element is move-inserted into the container.
    //:
    //: 3 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 4 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 For insertion we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 Compute the number of allocations and verify it is as expected.
    //:
    //:   2 If the object did not contain 'value', verify it now exist.
    //:     Otherwise, verify the return value is as expected.  (C-1..2)
    //:
    //:   3 Ensure that the move constructor was called on the value type (if
    //:     that type has a mechanism to detect such) where the value has the
    //:     same allocator as that of the container and a different allocator
    //:     than that of the container.  (C-2)
    //:
    //:   4 Verify all allocations are from the object's allocator.  (C-3)
    //:
    //: 2 Repeat P-1 under the presence of exception  (C-4)
    //
    // Testing:
    //   iterator insert(value_type&& value);
    // -----------------------------------------------------------------------
    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
        const char *d_unique;  // expected element values
    } DATA[] = {
        //line  spec           unique
        //----  -------------- --------------
        { L_,   "A",           "Y"            },
        { L_,   "AAA",         "YNN"          },
        { L_,   "ABCDEFGH",    "YYYYYYYY"     },
        { L_,   "AABBCCDDEE",  "YNYNYNYNYN"   },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"   },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY"  },
    };
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    const int MAX_LENGTH = 16;

    if (veryVerbose) { P(NameOf<KEY>().name()) P(NameOf<VALUE>().name()) }

    if (verbose) printf("\nTesting 'insert' without exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                bslma::TestAllocator za("different", veryVeryVeryVerbose);
                bsl::allocator<ValueType> soa(&oa);

                Obj mX(soa); const Obj &X = mX;

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
                  } break;
                }
                bslma::TestAllocator& sa = *valAllocator;
                bsl::allocator<ValueType> ssa(&sa);

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                    EXPECTED[SIZE] = SPEC[tj];
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }

                    const bsls::Types::Int64 BB = oa.numBlocksTotal();
                    const bsls::Types::Int64 B  = oa.numBlocksInUse();

                    bsls::ObjectBuffer<TValueType> buffer;
                    TValueType *valptr = buffer.address();
                    IntToPairConverter<KEY, VALUE, ALLOC>::createInplace(
                                  valptr,
                                  TstFacility::getIdentifier(VALUES[tj].first),
                                  ALLOC(ssa));

                    Iter RESULT = mX.insert(MoveUtil::move(*valptr));
                    MoveState::Enum mState =
                                 TstFacility::getMovedFromState(valptr->first);
                    bslma::DestructionUtil::destroy(valptr);

                    ASSERTV(LINE, CONFIG, tj, SIZE, VALUES[tj] == *RESULT);

                    const bsls::Types::Int64 AA = oa.numBlocksTotal();
                    const bsls::Types::Int64 A  = oa.numBlocksInUse();

                    ASSERTV(mState, MoveState::e_UNKNOWN == mState
                                    || MoveState::e_MOVED == mState);

                    ASSERTV(LINE, tj, AA, BB, BB + TYPE_ALLOC <= AA);
                    ASSERTV(LINE, tj,  A,  B,  B + TYPE_ALLOC <=  A);
                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                    ASSERTV(LINE, tj, verifySpec(X, EXPECTED));
                }
            }
        }
    }

    if (verbose) printf("\nTesting 'insert' with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                bsl::allocator<ValueType> soa(&oa);

                Obj mX(soa); const Obj &X = mX;

                bslma::TestAllocator za("different", veryVeryVeryVerbose);
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
                  } break;
                }
                bslma::TestAllocator& sa = *valAllocator;
                bsl::allocator<ValueType> ssa(&sa);

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                    EXPECTED[SIZE] = SPEC[tj];
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }

                    bslma::TestAllocator scratch("scratch",
                                                 veryVeryVeryVerbose);
                    bsl::allocator<ValueType> sscratch(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        Obj mZ(sscratch); const Obj& Z = mZ;
                        for (size_t tk = 0; tk < tj; ++tk) {
                            primaryManipulator(
                                 &mZ,
                                 TstFacility::getIdentifier(VALUES[tk].first),
                                 sscratch);
                        }
                        ASSERTV(Z, X, Z == X);

                        ExceptionProctor<Obj, ALLOC> proctor(&X, L_,
                                                           MoveUtil::move(mZ));

                        bsls::ObjectBuffer<TValueType> buffer;
                        TValueType *valptr = buffer.address();

                        IntToPairConverter<KEY, VALUE, ALLOC>::createInplace(
                                valptr,
                                TstFacility::getIdentifier(VALUES[tj].first),
                                ALLOC(ssa));

                        bslma::DestructorGuard<TValueType> guard(valptr);

                        Iter RESULT = mX.insert(MoveUtil::move(*valptr));

                        ASSERTV(LINE, CONFIG, tj, SIZE, VALUES[tj] == *RESULT);
                        proctor.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE, tj, verifySpec(X, EXPECTED));
                }
            }
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG, bool OTHER_FLAGS>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::
                   testCase27_propagate_on_container_move_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   KEY,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG>
                                                                      StdAlloc;

    typedef bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, StdAlloc> Obj;

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

                Obj *mR = &(mX = MoveUtil::move(mY));

                ASSERTV(ISPEC, JSPEC,  W,   X,  W == X);
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
                    ASSERTV(ISPEC, JSPEC, !oasm.isInUseUp());
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
                            testCase27_propagate_on_container_move_assignment()
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

    testCase27_propagate_on_container_move_assignment_dispatch<false, false>();
    testCase27_propagate_on_container_move_assignment_dispatch<false, true>();

    if (verbose)
        printf("\n'propagate_on_container_move_assignment::value == true'\n");

    testCase27_propagate_on_container_move_assignment_dispatch<true, false>();
    testCase27_propagate_on_container_move_assignment_dispatch<true, true>();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase27_noexcept()
    // Verify that noexcept specification of the move assignment operator is
    // correct.
{
    Obj a;
    Obj b;

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
    const bool isNoexcept =
                        bsl::allocator_traits<ALLOC>::is_always_equal::value &&
                        std::is_nothrow_move_assignable<HASH>::value &&
                        std::is_nothrow_move_assignable<EQUAL>::value;
    ASSERT(isNoexcept ==
           BSLS_KEYWORD_NOEXCEPT_OPERATOR(a = MoveUtil::move(b)));
#endif
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase27()
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
    //:
    // Plan:
    //
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
    //:   that of target and 2) the same allocator as that of the target,
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
    //   unordered_multimap& operator=(unordered_multimap&& rhs);
    // ------------------------------------------------------------------------

    Obj& (Obj::*operatorMAg) (bslmf::MovableRef<Obj>) = &Obj::operator=;
    (void) operatorMAg;  // quash potential compiler warning

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const TestValues VALUES;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    // Create first object
    if (verbose)
        printf("\nTesting move assignment.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec;
            const char *const RESULT1 = DATA[ti].d_results;
            const size_t      LENGTH1 = strlen(RESULT1);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bsl::allocator<ValueType> sscratch(&scratch);

            Obj  mZZ(sscratch); const Obj&  ZZ = gg(&mZZ, SPEC1);

            const bsls::Types::Int64 BEFORE = scratch.numBytesInUse();

            Obj mZZZ(sscratch); const Obj& ZZZ = gg(&mZZZ, SPEC1);

            const bsls::Types::Int64 BIU = scratch.numBytesInUse() - BEFORE;

            if (veryVerbose) { T_ P_(LINE1) P_(ZZZ) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.
            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE1, Obj(), ZZZ, Obj() == ZZZ);
                firstFlag = false;
            }

            // Create second object
            for (size_t tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;
                const char *const RESULT2 = DATA[tj].d_results;
                const size_t      LENGTH2 = strlen(RESULT2);

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
                    bslma::TestAllocator za("different", veryVeryVeryVerbose);

                    bsl::allocator<ValueType> soa(&oa);
                    bsl::allocator<ValueType> sza(&za);

                    Obj *objPtr = new (fa) Obj(soa);
                    Obj& mX = *objPtr; const Obj& X = gg(&mX, SPEC2);

                    Obj *srcPtr = 0;
                    bslma::TestAllocator *srcAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        srcPtr = new (fa) Obj(sza); gg(srcPtr, SPEC1);
                        srcAllocatorPtr = &za;
                      } break;
                      case 'b': {
                        srcPtr = new (fa) Obj(soa); gg(srcPtr, SPEC1);
                        srcAllocatorPtr = &oa;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } return;                                       // RETURN
                    }

                    Obj& mZ = *srcPtr; const Obj& Z = mZ;
                    bslma::TestAllocator& sa = *srcAllocatorPtr;
                    bsl::allocator<ValueType> ssa(&sa);

                    if (veryVerbose) { T_ P_(LINE2) P(Z) }
                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    ASSERTV(SPEC1, SPEC2, Z, X,
                            (Z == X) == (INDEX1 == INDEX2));

                    bool empty = 0 == ZZ.size();

                    typename Obj::const_pointer pointers[2];
                    storeFirstNElemAddr(pointers, Z,
                            sizeof pointers / sizeof *pointers);

                    bslma::TestAllocatorMonitor oam(&oa), zam(&za);

                    Obj *mR = &(mX = bslmf::MovableRefUtil::move(mZ));
                    ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                    // Verify the value of the object.
                    ASSERTV(SPEC1, SPEC2,  X,  ZZ,  X ==  ZZ);

                    if (0 == LENGTH1) {
                        // assigned an empty unordered_multimap
                        ASSERTV(SPEC1, SPEC2, oa.numBytesInUse(),
                                0 == oa.numBytesInUse());
                    }

                    // CONTAINER SPECIFIC NOTE: For 'multimap', the original
                    // object is left in the default state even when the source
                    // and target objects use different allocators because
                    // move-insertion changes the value of the source key and
                    // violates the requirements for keys contained in the
                    // 'multimap'.

                    if (&sa == &oa) {
                        // same allocator
                        ASSERTV(SPEC1, SPEC2, oam.isTotalSame());
                        if (0 == LENGTH2) {
                            // assigning to an empty multimap
                            ASSERTV(SPEC1, SPEC2, oam.isInUseSame());
                            ASSERTV(SPEC1, SPEC2, BIU, oa.numBytesInUse(),
                                    BIU == oa.numBytesInUse());
                        }

                        // 2. unchanged address of contained element(s)
                        ASSERT(0 == checkFirstNElemAddr(pointers,
                                                        X,
                                                        sizeof pointers /
                                                            sizeof *pointers));

                        // 3. original object left empty
                        ASSERTV(SPEC1, SPEC2, &sa == &oa, Z, 0 == Z.size());

                        // 4. nothing from the other allocator
                        ASSERTV(SPEC1, SPEC2, 0 == za.numBlocksTotal());
                    }
                    else {
                        // 1. each element in original move-inserted
                        ASSERTV(SPEC1,
                                SPEC2,
                                X.end() == TstMoveUtil::findFirstNotMovedInto(
                                               X.begin(), X.end()));

                        // 2. CONTAINER SPECIFIC NOTE: original object left
                        // empty
                        ASSERTV(SPEC1, SPEC2, &sa == &oa, Z, 0 == Z.size());

                        // 3. additional memory checks
                        ASSERTV(SPEC1, SPEC2, &sa == &oa,
                                empty || oam.isTotalUp());
                        ASSERTV(SPEC1, SPEC2, 0 == zam.isInUseUp());

                    }
                    // Verify that 'X', 'Z', and 'ZZ' have correct allocator.
                    ASSERTV(SPEC1, SPEC2, &scratch == ZZ.get_allocator());
                    ASSERTV(SPEC1, SPEC2,      &oa ==  X.get_allocator());
                    ASSERTV(SPEC1, SPEC2,      &sa ==  Z.get_allocator());

                    // Manipulate source object 'Z' to ensure it is in a valid
                    // state and is independent of 'X'.
                    Iter RESULT = primaryManipulator(&mZ, 'Z', ssa);
                    ASSERTV(SPEC1, SPEC2, Z, 1 == Z.size());
                    ASSERTV(SPEC1, SPEC2, Z, ZZ, Z != ZZ);
                    ASSERTV(SPEC1, SPEC2, X, ZZ, X == ZZ);

                    fa.deleteObject(srcPtr);

                    ASSERTV(SPEC1, SPEC2, oa.numBlocksInUse(),
                            empty ||
                                ((&sa == &oa) == (0 < sa.numBlocksInUse())));

                    // Verify subsequent manipulation of target object 'X'.
                    RESULT = primaryManipulator(&mX, 'Z', soa);
                    ASSERTV(SPEC1, SPEC2, LENGTH1 + 1, X.size(),
                            LENGTH1 + 1 == X.size());
                    ASSERTV(SPEC1, SPEC2, X, ZZ, X != ZZ);

                    fa.deleteObject(objPtr);

                    ASSERTV(SPEC1, SPEC2, oa.numBlocksInUse(),
                            0 == oa.numBlocksInUse());
                    ASSERTV(SPEC1, SPEC2, sa.numBlocksInUse(),
                            0 == sa.numBlocksInUse());
                }
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bsl::allocator<ValueType> soa(&oa);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                bsl::allocator<ValueType> sscratch(&scratch);

                Obj mX(soa);       const Obj& X  = gg(&mX,  SPEC1);
                Obj mZZ(sscratch); const Obj& ZZ = gg(&mZZ, SPEC1);

                const Obj& Z = mX;

                ASSERTV(SPEC1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = bslmf::MovableRefUtil::move(mX));
                    ASSERTV(SPEC1, ZZ,   Z, ZZ == Z);
                    ASSERTV(SPEC1, mR,  &X, mR == &X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(SPEC1, &oa == Z.get_allocator());

                ASSERTV(SPEC1, sam.isTotalSame());
                ASSERTV(SPEC1, oam.isTotalSame());

                ASSERTV(SPEC1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(SPEC1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }
    }

    if (verbose)
        printf("\nTesting move assignment with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec;
            const char *const RESULT1 = DATA[ti].d_results;
            const size_t      LENGTH1 = strlen(RESULT1);

            if (4 < LENGTH1 && NUM_DATA-1 != ti) {
                continue;
            }

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bsl::allocator<ValueType> sscratch(&scratch);

            Obj mZZ(sscratch); const Obj& ZZ = gg(&mZZ, SPEC1);

            if (veryVerbose) { T_ P_(LINE1) P(ZZ) }

            // Create second object
            for (size_t tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;
                const char *const RESULT2 = DATA[tj].d_results;
                const size_t      LENGTH2 = strlen(RESULT2);

                if (4 < LENGTH2 && NUM_DATA-1 != tj) {
                    continue;
                }

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
                    bslma::TestAllocator za("different", veryVeryVeryVerbose);
                    bsl::allocator<ValueType> soa(&oa);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        bsls::Types::Int64 al = oa.allocationLimit();

                        oa.setAllocationLimit(-1);

                        Obj mX(soa); const Obj& X = gg(&mX, SPEC2);

                        bslma::TestAllocator *srcAllocatorPtr;
                        switch (CONFIG) {
                          case 'a': {
                            srcAllocatorPtr = &za;
                          } break;
                          case 'b': {
                            srcAllocatorPtr = &oa;
                          } break;
                          default: {
                            ASSERTV(CONFIG, !"Bad allocator config.");
                          } return;                                   // RETURN
                        }
                        bslma::TestAllocator& sa = *srcAllocatorPtr;
                        bsl::allocator<ValueType> ssa(&sa);

                        Obj mZ(ssa); const Obj& Z = gg(&mZ, SPEC1);

                        if (veryVerbose) { T_ P_(LINE2) P(Z) }
                        if (veryVerbose) { T_ P_(LINE2) P(X) }

                        ASSERTV(SPEC1, SPEC2, Z, X,
                                (Z == X) == (INDEX1 == INDEX2));

                        typename Obj::const_pointer pointers[2];
                        storeFirstNElemAddr(pointers, Z,
                                            sizeof pointers /sizeof *pointers);

                        Obj mE(sscratch);
                        ExceptionProctor<Obj, ALLOC> guard(
                                                   &Z, L_, MoveUtil::move(mE));

                        oa.setAllocationLimit(al);

                        Obj *mR = &(mX = bslmf::MovableRefUtil::move(mZ));
                        ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                        // Verify the value of the object.
                        ASSERTV(SPEC1, SPEC2,  X,  ZZ,  X ==  ZZ);

                        guard.release();

                        // Manipulate source object 'Z' to ensure it is in a
                        // valid state and is independent of 'X'.
                        Iter RESULT = primaryManipulator(&mZ, 'Z', ssa);
                        ASSERTV(SPEC1, SPEC2, Z, 1 == Z.size());
                        ASSERTV(SPEC1, SPEC2, Z, ZZ, Z != ZZ);
                        ASSERTV(SPEC1, SPEC2, X, ZZ, X == ZZ);

                        // Verify subsequent manipulation of target object 'X'.
                        RESULT = primaryManipulator(&mX, 'Z', soa);
                        ASSERTV(SPEC1, SPEC2, LENGTH1 + 1, X.size(),
                                LENGTH1 + 1 == X.size());
                        ASSERTV(SPEC1, SPEC2, X, ZZ, X != ZZ);

                        oa.setAllocationLimit(al);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }
    }
    ASSERTV(0 == da.numAllocations());
}
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase26()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE CONSTRUCTOR
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
    //   unordered_multimap(unordered_multimap&& original);
    //   unordered_multimap(unordered_multimap&& original, const A& allocator);
    // ------------------------------------------------------------------------

    if (verbose) printf("TESTING MOVE CONSTRUCTOR\n"
                        "========================\n");

    if (veryVerbose) { P(NameOf<KEY>().name()) P(NameOf<VALUE>().name()) }

    const TestValues VALUES;

    static const char *SPECS[] = {
        "",
        "A",
        "AA",
        "BC",
        "CB",
        "BBC",
        "BCB",
        "BCC",
        "CBB",
        "CBC",
        "CCB",
        "CCC",
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

    const size_t NUM_SPECS = sizeof SPECS / sizeof *SPECS;

    if (veryVerbose)
        printf("\nTesting of move constructor.\n");
    {
        const HASH  tstHash(7);
        const EQUAL tstEqual(9);

        for (size_t ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            if (veryVerbose) {
                printf("\nFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            // Create control object, 'ZZ', with the scratch allocator.
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bsl::allocator<ValueType> sscratch(&scratch);

            Obj mZZ(0, tstHash, tstEqual, sscratch);
            const Obj& ZZ = gg(&mZZ, SPEC);

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                if (verbose) P(CONFIG);

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator za("different", veryVeryVeryVerbose);

                bsl::allocator<ValueType> sda(&da);
                bsl::allocator<ValueType> ssa(&sa);
                bsl::allocator<ValueType> sza(&za);

                bslma::DefaultAllocatorGuard dag(&da);

                // Create source object 'Z'.
                Obj *srcPtr = new (fa) Obj(0, tstHash, tstEqual, ssa);
                Obj& mZ = *srcPtr; const Obj& Z = gg(&mZ, SPEC);

                {
                    const HASH& hf   = Z.hash_function();
                    ASSERTV(tstHash.id()  == hf.id()  &&
                                                     HASH().id()  != hf.id());
                    const EQUAL& equ = Z.key_eq();
                    ASSERTV(tstEqual.id() == equ.id() &&
                                                     EQUAL().id() != equ.id());
                }

                typename Obj::const_pointer pointers[2];
                storeFirstNElemAddr(pointers, Z,
                                    sizeof pointers / sizeof *pointers);

                bslma::TestAllocatorMonitor oam(&da), sam(&sa);

                Obj *objPtr;
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
                    bsl::allocator<ValueType> sdc;
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), sdc);
                    objAllocatorPtr = &da;
                    othAllocatorPtr = &za;
                  } break;
                  case 'c': {
                    oam.reset(&sa);
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), ssa);
                    objAllocatorPtr = &sa;
                    othAllocatorPtr = &da;
                  } break;
                  case 'd': {
                    oam.reset(&za);
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), sza);
                    objAllocatorPtr = &za;
                    othAllocatorPtr = &da;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = *othAllocatorPtr;
                bsl::allocator<ValueType> soa(&oa);

                Obj& mX = *objPtr; const Obj& X = mX;

                // Verify the value of the object.
                ASSERTV(SPEC, CONFIG, X == ZZ);

                // Verify that the hash function and key equality predicate was
                // copied.
                ASSERTV(SPEC, CONFIG,
                        X.hash_function().id() == Z.hash_function().id());
                ASSERTV(SPEC, CONFIG, X.key_eq().id() == Z.key_eq().id());

                if (&sa == &oa) {
                    // 1. no memory allocation
                    ASSERTV(SPEC, CONFIG, &sa == &oa, oam.isTotalSame());
                    // 2. unchanged address of contained element(s)
                    ASSERT(0 == checkFirstNElemAddr(pointers, X,
                                          sizeof pointers / sizeof *pointers));
                    // 3. original object left empty
                    ASSERTV(SPEC, CONFIG, &sa == &oa, Z, 0 == Z.size());
                }
                else {
                    // 1. each element in original move-inserted
                    ASSERTV(SPEC, X.end() ==
                       TstMoveUtil::findFirstNotMovedInto(X.begin(), X.end()));
                    // 2. original object left empty
                    ASSERTV(SPEC, CONFIG, &sa == &oa, Z, 0 == Z.size());
                    // 3. additional memory checks
                    ASSERTV(SPEC, CONFIG, &sa == &oa, oam.isTotalUp() ||empty);
                }

                ASSERTV(verifySpec(X, SPEC));

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
                Iter RESULT = primaryManipulator(&mZ, 'Z', ssa);
                ASSERTV(SPEC, CONFIG, Z, 1 == Z.size());
                ASSERTV(SPEC, CONFIG, Z, ZZ, Z != ZZ);
                ASSERTV(SPEC, CONFIG, X, ZZ, X == ZZ);

                fa.deleteObject(srcPtr);

                ASSERTV(SPEC, CONFIG, X, ZZ, X == ZZ);

                // Verify subsequent manipulation of new object 'X'.
                RESULT = primaryManipulator(&mX, 'Z', soa);
                ASSERTV(SPEC, LENGTH + 1 == X.size());
                ASSERTV(SPEC, X != ZZ);

                fa.deleteObject(objPtr);

                ASSERTV(SPEC, 0 == fa.numBlocksInUse());
                ASSERTV(SPEC, 0 == da.numBlocksInUse());
                ASSERTV(SPEC, 0 == sa.numBlocksInUse());
                ASSERTV(SPEC, 0 == za.numBlocksInUse());
            }
        }
    }

    if (veryVerbose)
        printf("\nTesting move constructor with injected exceptions.\n");
#if defined(BDE_BUILD_TARGET_EXC)
    {
        for (size_t ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bsl::allocator<ValueType> sscratch(&scratch);

            Obj mZZ(sscratch); const Obj& ZZ = gg(&mZZ, SPEC);

            if (verbose) {
                printf("\nFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
            bslma::TestAllocator za("different", veryVeryVeryVerbose);

            bsl::allocator<ValueType> soa(&oa);
            bsl::allocator<ValueType> sza(&za);

            const bsls::Types::Int64 BB = oa.numBlocksTotal();
            const bsls::Types::Int64  B = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
            }

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                Obj mE(sscratch);
                Obj mZ(sza); const Obj& Z = gg(&mZ, SPEC);

                ExceptionProctor<Obj, ALLOC> guard(&Z, L_, MoveUtil::move(mE));

                const Obj X(MoveUtil::move(mZ), soa);

                if (veryVerbose) {
                    printf("\t\t\tException Case  :\n");
                    printf("\t\t\t\tObj : "); P(X);
                }
                ASSERTV(SPEC, X, ZZ, ZZ == X);
                ASSERTV(SPEC, 0 == Z.size());
                ASSERTV(SPEC, Z.get_allocator() != X.get_allocator());

                guard.release();

                // Manipulate source object 'Z' to ensure it is in a
                // valid state and is independent of 'X'.
                Iter RESULT = primaryManipulator(&mZ, 'Z', sza);
                ASSERTV(RESULT != Z.end());
                ASSERTV(SPEC, Z, 1 == Z.size());
                ASSERTV(SPEC, Z, ZZ, Z != ZZ);
                ASSERTV(SPEC, X, ZZ, X == ZZ);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            const bsls::Types::Int64 AA = oa.numBlocksTotal();
            const bsls::Types::Int64  A = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
            }

            if (LENGTH == 0) {
                ASSERTV(SPEC, BB == AA);
                ASSERTV(SPEC,  B ==  A);
            }
        }
    }
#endif
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase23()
{
    // ------------------------------------------------------------------------
    // TESTING TYPE TRAITS
    //
    // Concern:
    //: 1 The object has the necessary type traits.
    //
    // Plan:
    //: 1 Use 'BSLMF_ASSERT' to verify all the type traits exists.  (C-1)
    //
    // Testing:
    //   CONCERN: 'unordered_multimap' has the necessary type traits.
    // ------------------------------------------------------------------------

    // Verify unordered_multimap defines the expected traits.

    typedef bsl::unordered_multimap<KEY, VALUE> UMMKV;

    BSLMF_ASSERT((1 == bslalg::HasStlIterators<UMMKV>::value));

    BSLMF_ASSERT((1 == bslma::UsesBslmaAllocator<UMMKV>::value));

    BSLMF_ASSERT((1 == bslmf::IsBitwiseMoveable<UMMKV>::value));

    // Verify the bslma-allocator trait is not defined for non
    // bslma-allocators.

    typedef bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, StlAlloc>
                                                                   ObjStlAlloc;
    BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<ObjStlAlloc>::value));

    // Verify unordered_multimap does not define other common traits.

    BSLMF_ASSERT((0 == bsl::is_trivially_copyable<UMMKV>::value));

    BSLMF_ASSERT((0 == bslmf::IsBitwiseEqualityComparable<UMMKV>::value));

    BSLMF_ASSERT((0 == bslmf::HasPointerSemantics<UMMKV>::value));

    BSLMF_ASSERT((0 == bsl::is_trivially_default_constructible<UMMKV>::value));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase22()
{
    // ------------------------------------------------------------------------
    // TESTING STL ALLOCATOR
    //
    // Concern:
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
    //  CONCERN: 'unordered_multimap' is compatible with standard allocators.
    // ------------------------------------------------------------------------

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    typedef bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, StlAlloc>
                                                                   ObjStlAlloc;

    StlAlloc scratch;

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int     LINE   = DATA[ti].d_line;
        const char   *SPEC   = DATA[ti].d_spec;
        const size_t  LENGTH = strlen(SPEC);

        TestValues CONT(SPEC, scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator da("default",   veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        {
            ObjStlAlloc mX(BEGIN, END); const ObjStlAlloc& X = mX;

            ASSERTV(LINE, verifySpec(X, SPEC));

            ObjStlAlloc mY(X);  const ObjStlAlloc& Y = mY;

            ASSERTV(LINE, verifySpec(Y, SPEC));

            ObjStlAlloc mZ;  const ObjStlAlloc& Z = mZ;

            mZ.swap(mX);

            ASSERTV(LINE, verifySpec(Z, SPEC));
        }

        CONT.resetIterators();

        {
            ObjStlAlloc mX;  const ObjStlAlloc& X = mX;
            mX.insert(BEGIN, END);
            ASSERTV(LINE, verifySpec(X, SPEC));
        }

        CONT.resetIterators();

        {
            ObjStlAlloc mX;  const ObjStlAlloc& X = mX;
            for (size_t tj = 0; tj < CONT.size(); ++tj) {
                Iter RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH,
                        CONT[tj].first == RESULT->first);
                ASSERTV(LINE, tj, LENGTH,
                        CONT[tj].second == RESULT->second);
            }
            ASSERTV(LINE, verifySpec(X, SPEC));
        }

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

    // IBM empty class swap bug test

    {
        typedef bsl::unordered_multimap<int,
                                        int,
                                        TestHashFunctor<int>,
                                        TestEqualityComparator<int>,
                                        StlAlloc> TestObjIntStlAlloc;

        TestObjIntStlAlloc mX;
        mX.insert(bsl::pair<int, int>(1, 1));
        TestObjIntStlAlloc mY;
        mY = mX;
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase21()
{
    // ------------------------------------------------------------------------
    // TESTING 'hash_function' AND 'key_eq'
    //
    // Concern:
    //: 1 The hash function and key equal comparator is set correctly.
    //:
    //: 2 'hash_function' and 'key_eq' return the functions that were passed in
    //:   on construction.
    //:
    //: 3 Functions are properly propagated on copy construction, copy
    //:   assignment, and swap.
    //
    // Plan:
    //:  TBD
    //
    // Testing:
    //   hasher hash_function() const;
    //   key_equal key_eq() const;
    // ------------------------------------------------------------------------

    {
        typedef HASH (Obj::*MP)() const;
        MP mp = &Obj::hash_function;
        (void) mp;
    }

    {
        typedef EQUAL (Obj::*MP)() const;
        MP mp = &Obj::key_eq;
        (void) mp;
    }

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    static const int ID[] = { 0, 1, 2 };
    const int NUM_ID = static_cast<int>(sizeof ID / sizeof *ID);

    for (int ti = 0; ti < NUM_ID; ++ti) {
        const int hId = ti;
        const int eId = ti * 7 + 5;
        const HASH  H(hId);
        const EQUAL E(eId);
        {
            const Obj X(0, H, E);
            ASSERTV(ti, hId == X.hash_function().id());
            ASSERTV(ti, eId == X.key_eq().id());
            ASSERTV(ti, 0   == X.hash_function().count());
            ASSERTV(ti, 0   == X.key_eq().count());
        }
        {
            const Obj X(0, H, E, &scratch);
            ASSERTV(ti, hId == X.hash_function().id());
            ASSERTV(ti, eId == X.key_eq().id());
            ASSERTV(ti, 0   == X.hash_function().count());
            ASSERTV(ti, 0   == X.key_eq().count());
        }
        {
            TestValues CONT("");
            const Obj X(CONT.end(), CONT.end(), 0, H, E);
            ASSERTV(ti, hId == X.hash_function().id());
            ASSERTV(ti, eId == X.key_eq().id());
            ASSERTV(ti, 0   == X.hash_function().count());
            ASSERTV(ti, 0   == X.key_eq().count());
        }
        {
            TestValues CONT("");
            const Obj X(CONT.end(), CONT.end(), 0, H, E, &scratch);
            ASSERTV(ti, hId == X.hash_function().id());
            ASSERTV(ti, eId == X.key_eq().id());
            ASSERTV(ti, 0   == X.hash_function().count());
            ASSERTV(ti, 0   == X.key_eq().count());
        }
    }

    static const struct {
        int         d_line;             // source line number
        const char *d_spec;             // spec
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
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    const HASH DH(0, false);    // delinquent hash, always returns 0
    const HASH H;               // normal hash

    const EQUAL E;

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(DATA[ti].d_spec);

        TestValues CONT(SPEC, &scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            Obj mW(BEGIN, END, 0, DH, E); const Obj& W = mW;

            ASSERTV(LINE, verifySpec(W, SPEC));
            ASSERTV(LINE,  H == W.hash_function());
            ASSERTV(LINE,  E == W.key_eq());

            Obj mX(W); const Obj& X = mX;

            ASSERTV(LINE, verifySpec(X, SPEC));
            ASSERTV(LINE,  H == X.hash_function());
            ASSERTV(LINE,  E == X.key_eq());

            Obj mY;  const Obj& Y = mY;
            mY = mW;
            ASSERTV(LINE, verifySpec(Y, SPEC));
            ASSERTV(LINE,  H == Y.hash_function());
            ASSERTV(LINE,  E == Y.key_eq());

            Obj mZ;  const Obj& Z = mZ;
            mZ.swap(mW);

            ASSERTV(LINE, verifySpec(Z, SPEC));
            ASSERTV(LINE,  H == Z.hash_function());
            ASSERTV(LINE,  E == Z.key_eq());
            ASSERTV(LINE,  H == W.hash_function());
            ASSERTV(LINE,  E == W.key_eq());
        }

        CONT.resetIterators();

        {
            Obj mX(0, DH, E); const Obj& X = mX;
            mX.insert(BEGIN, END);
            ASSERTV(LINE, verifySpec(X, SPEC));
        }

        CONT.resetIterators();

        {
            Obj mX(0, DH, E); const Obj& X = mX;
            for (size_t tj = 0; tj < CONT.size(); ++tj) {
                Iter RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH, CONT[tj] == *RESULT);
            }
            ASSERTV(LINE, verifySpec(X, SPEC));
        }

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

#if 1 // TBD non-'const' comparator
    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(DATA[ti].d_spec);

        TestValues CONT(SPEC, &scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator da("default",   veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        TestNonConstHashFunctor<KEY>        NCH;
        TestNonConstEqualityComparator<KEY> NCE;
        typedef bsl::unordered_multimap<KEY,
                                        VALUE,
                                        TestNonConstHashFunctor<KEY>,
                                        TestNonConstEqualityComparator<KEY>,
                                        ALLOC> ObjNCH;

        {
            ObjNCH mW(BEGIN, END); const ObjNCH& W = mW;

            ASSERTV(LINE, verifySpec(W, SPEC));
            ASSERTV(LINE, NCH == W.hash_function());
            ASSERTV(LINE, NCE == W.key_eq());

            ObjNCH mX(W); const ObjNCH& X = mX;

            ASSERTV(LINE, verifySpec(X, SPEC));
            ASSERTV(LINE, NCH == X.hash_function());
            ASSERTV(LINE, NCE == X.key_eq());

            ObjNCH mY; const ObjNCH& Y = mY;
            mY = mW;
            ASSERTV(LINE, verifySpec(Y, SPEC));
            ASSERTV(LINE, NCH == Y.hash_function());
            ASSERTV(LINE, NCE == Y.key_eq());

            ObjNCH mZ; const ObjNCH& Z = mZ;
            mZ.swap(mW);

            ASSERTV(LINE, verifySpec(Z, SPEC));
            ASSERTV(LINE, NCH == Z.hash_function());
            ASSERTV(LINE, NCH == W.hash_function());
            ASSERTV(LINE, NCE == Z.key_eq());
            ASSERTV(LINE, NCE == W.key_eq());
            ASSERTV(LINE, TestNonConstHashFunctor<KEY>() == W.hash_function());
            ASSERTV(LINE, TestNonConstEqualityComparator<KEY>() == W.key_eq());
        }

        CONT.resetIterators();

        {
            ObjNCH mX; const ObjNCH& X = mX;
            mX.insert(BEGIN, END);
            ASSERTV(LINE, verifySpec(X, SPEC));
        }

        CONT.resetIterators();

        {
            ObjNCH mX; const ObjNCH& X = mX;
            for (size_t tj = 0; tj < CONT.size(); ++tj) {
                Iter RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH, CONT[tj] == *RESULT);
            }
            ASSERTV(LINE, verifySpec(X, SPEC));
        }

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }
#endif
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase20()
{
    // ------------------------------------------------------------------------
    // TESTING 'max_size' AND 'empty'
    //
    // Concern:
    //: 1 'max_size' returns the 'max_size' of the supplied allocator.
    //:
    //: 2 'empty' returns 'true' only when the object is empty.
    //
    // Plan:
    //: 1 Run each function and verify the result.  (C-1..2)
    //
    // Testing:
    //  bool empty() const;
    //  size_type max_size() const;
    // ------------------------------------------------------------------------

    bslma::TestAllocator  oa("object", veryVeryVerbose);

    if (verbose) printf("\tTesting 'max_size'.\n");
    {
        // This is the maximum value.  Any larger value would be cause for
        // potential bugs.

        Obj X;
        ALLOC a;
        ASSERTV(a.max_size(), X.max_size(), a.max_size() ==  X.max_size());
    }

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // initial
    } DATA[] = {
        //line  spec
        //----  -----------
        { L_,   ""          },
        { L_,   "A"         },
        { L_,   "AA"        },
        { L_,   "ABC"       },
        { L_,   "ABCA"      },
        { L_,   "ABCDE"     },
        { L_,   "ABCDEF"    },
        { L_,   "ABCDEFG"   },
        { L_,   "ABCDEFGH"  }
    };
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\tTesting 'empty'.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_lineNum;
            const char *SPEC   = DATA[ti].d_spec;

            Obj mX(&oa); const Obj& X = gg(&mX, SPEC);

            ASSERTV(LINE, SPEC, (0 == ti) == X.empty());
            ASSERTV(LINE, SPEC, (0 == ti) == bsl::empty(X));

            mX.clear();

            ASSERTV(LINE, SPEC, true == X.empty());
            ASSERTV(LINE, SPEC, true == bsl::empty(X));
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase18()
{
    // ------------------------------------------------------------------------
    // TESTING ERASE
    //
    // Concern:
    //: 1 'erase' with iterators returns the iterator right after the erased
    //:   value(s).
    //:
    //: 2 'erase' with 'key' returns the number of elements removed (0 if
    //:    there are no elements equivalent to the 'key'
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
    //: 2 Repeat P-1 with 'erase(iterator pos)'.
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
    //   size_type erase(const key_type& key);
    //   iterator erase(const_iterator pos);
    //   iterator erase(iterator pos);
    //   iterator erase(const_iterator first, const_iterator last);
    // -----------------------------------------------------------------------

    if (veryVerbose) printf("\nTesting '%s'\n", NameOf<KEY>().name());

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da("default",   veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose)
        printf("\nTesting 'erase(pos)' on non-empty unordered_multimap.\n");
    {
        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_spec);

            if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma::TestAllocator oa("object", veryVeryVerbose);

                Obj mXC(&oa);     const Obj& XC = gg(&mXC, SPEC);
                Obj mX(&oa);      const Obj& X  = gg(&mX,  SPEC);
                Obj mYC(XC, &oa); const Obj& YC = mYC;
                Obj mY( X,  &oa); const Obj& Y  = mY;

                CIter posC    = getIterForIndex(XC,  tj);
                Iter  pos     = getIterForIndex(mX,  tj);
                CIter beforeC = 0 == tj ? mXC.end()
                                        : getIterForIndex(XC, tj - 1);
                Iter  before  = 0 == tj ? mX.end()
                                        : getIterForIndex(mX, tj - 1);
                CIter afterC = posC; ++afterC;
                CIter after  = pos;  ++after;

                const KEY kC((*posC).first);
                const KEY k( (*pos).first);

                const size_t COUNTC = XC.count(kC);
                const size_t COUNT  = X.count(k);

                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor dam(&oa);

                {
                    Iter RESULTC;
                    int numPasses = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ++numPasses;

                        RESULTC = mXC.erase(posC);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(RESULTC == afterC);
                    ASSERTV(1       == numPasses);
                }

                ASSERTV(oam.isTotalSame());
                ASSERTV(dam.isTotalSame());

                {
                    Iter RESULT;
                    int numPasses = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ++numPasses;

                        RESULT = mX.erase(pos);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(RESULT == after);
                    ASSERTV(1      == numPasses);
                }

                ASSERTV(oam.isTotalSame());
                ASSERTV(dam.isTotalSame());

                if (TYPE_ALLOC) {
                    ASSERTV(LINE, tj, oam.isInUseDown());
                }

                ASSERTV(COUNTC - 1 == XC.count(kC));
                ASSERTV(COUNT  - 1 == X.count(k));
                ASSERTV(XC.size()  == LENGTH - 1);
                ASSERTV(X.size()   == LENGTH - 1);

                for (CIter it = YC.begin(); YC.end() != it; ++it) {
                    if (kC != (*it).first) {
                        ASSERTV(*it, kC, XC.count((*it).first),
                                XC.count((*it).first) > 0);
                    }
                }

                for (CIter it = Y.begin(); Y.end() != it; ++it) {
                    if (k != (*it).first) {
                        ASSERTV(*it, k, X.count((*it).first),
                                X.count((*it).first) > 0);
                    }
                }

                if (0 == tj) {
                    ASSERTV(XC.begin() == afterC);
                    ASSERTV(X.begin()  == after);
                }
                else {
                    posC = beforeC; ++posC;
                    pos  = before;  ++pos;
                    ASSERT(posC == afterC);
                    ASSERT(pos  == after);
                }
            }
        }
    }

    if (verbose)
        printf("\nTesting 'erase(key)' on non-empty unordered_multimap.\n");
    {
        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_spec);

            if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma::TestAllocator oa("object", veryVeryVerbose);

                Obj mX(&oa);    const Obj& X = gg(&mX, SPEC);
                Obj mY(X, &oa); const Obj& Y = mY;

                CIter pos = getIterForIndex(X, tj);

                const KEY k((*pos).first);

                bsl::pair<CIter, CIter> range = X.equal_range(k);

                CIter before = mX.begin();
                if (mX.begin() == range.first) {
                    before = mX.end();
                }
                else {
                    CIter afterBefore = before;
                    while (mX.end() != afterBefore &&
                           range.first != ++afterBefore) {
                        ++before;
                    }
                }

                CIter after = range.second;

                const size_t COUNT = X.count(k);

                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor dam(&oa);

                size_t RESULT;
                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;

                    RESULT = mX.erase(k);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(COUNT == RESULT);
                ASSERT(1 == numPasses);

                ASSERTV(oam.isTotalSame());
                ASSERTV(dam.isTotalSame());

                if (TYPE_ALLOC) {
                    ASSERTV(LINE, tj, oam.isInUseDown());
                }

                ASSERTV(!X.count(k));
                ASSERTV(X.size() == LENGTH - COUNT);

                for (CIter it = Y.begin(); Y.end() != it; ++it) {
                    ASSERT((k != (*it).first) == !!X.count((*it).first));
                }

                if (before == X.end()) {
                    ASSERTV(X.begin() == after);
                }
                else {
                    pos = before; ++pos;
                    ASSERTV(X, pos == after);
                }
            }
        }
    }

    if (verbose) printf("\nTesting 'erase(first, last)'.\n");
    {
//#warning "TBD: non-trivial testing"
        if (verbose) printf("\nTBD: Testing 'erase(first, last)'.\n");
    }

    if (verbose) printf("\nNegative Testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        {
            const TestValues VALUES;

            Obj mX;
            Iter it = mX.insert(VALUES[0]);

            ASSERT_FAIL(mX.erase(mX.end()));
            ASSERT_SAFE_PASS(mX.erase(it));
        }
    }
}

template <class KEY, class MAPPED, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, MAPPED, HASH, EQUAL, ALLOC>::testCase17()
{
    // ------------------------------------------------------------------------
    // RANGE 'insert'
    //
    // Concerns:
    //: 1 All values within the range [first, last) are inserted.
    //:
    //: 2 Each iterator is dereferenced only once.
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
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value set, 'V') in the
    //:   table described in P-1:
    //:
    //:   1 Use the range constructor to create a object with part of the
    //:     elements in 'V'.
    //:
    //:   2 Insert the rest of 'V' under the presence of exceptions.  (C-7)
    //:
    //:   3 Verify the object's value.  (C-1-3)
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

    if (veryVerbose) printf("Testing '%s'\n", NameOf<KEY>().name());

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator sa("scratch", veryVeryVeryVerbose);

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const char *const EXP    = DATA[ti].d_results;
        const size_t      LENGTH = strlen(EXP);

        TestValues CONT(SPEC);
        for (size_t tj = 0; tj <= CONT.size(); ++tj) {
            CONT.resetIterators();

            typename TestValues::iterator BEGIN = CONT.begin();
            typename TestValues::iterator MID   = CONT.index(tj);
            typename TestValues::iterator END   = CONT.end();

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",  veryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(BEGIN, MID, &oa); const Obj& X = mX;

            const Int64 A = oa.numAllocations();

            mX.insert(MID, END);

            const Int64 AA = oa.numAllocations();

            ASSERTV(LINE, tj < CONT.size() || AA == A);
            ASSERTV(LINE, tj, X, LENGTH, X.size(), verifySpec(X, EXP));
            ASSERTV(LINE, tj, da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase16()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION WITH HINT
    //
    // Concerns:
    //: 1 'insert' returns an iterator referring to the newly inserted element.
    //:
    //: 2 Inserting with the correct hint places the new element right before
    //:   the hint.
    //:
    //: 3 Incorrect hint will be ignored and 'insert' will proceed as if the
    //:   hint is not supplied.
    //:
    //: 4 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 5 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 For insertion we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 For each set of values, set hint to be 'begin', 'begin' + 1,
    //:     'end', find(key).
    //:
    //:     1 For each value in the set, 'insert' the value with hint.
    //:
    //:       1 Verify the return value and the resulting data in the container
    //:         is as expected.  (C-1)
    //:
    //:       2 Verify the new element is inserted right before the hint if
    //:         the hint is valid.  (C-2,3)
    //:
    //:       3 Verify all allocations are from the object's allocator.  (C-4)
    //:
    //: 2 Repeat P-1 under the presence of exception  (C-5)
    //
    // Testing:
    //   iterator insert(const_iterator position, const value_type& value);
    // -----------------------------------------------------------------------

    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
        const char *d_unique;  // expected element values
    } DATA[] = {
        //line  spec           unique
        //----  -------------- --------------
        { L_,   "A",           "Y"            },
        { L_,   "AAA",         "YNN"          },
        { L_,   "ABA",         "YYN"          },
        { L_,   "BAA",         "YYN"          },
        { L_,   "ABCDEFGH",    "YYYYYYYY"     },
        { L_,   "AABBCCDDEE",  "YNYNYNYNYN"   },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"   },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY"  },
    };
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    const int MAX_LENGTH = 16;

    if (verbose) printf("\nTesting 'insert' with hint.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P(LENGTH); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                Obj mX(&oa); const Obj &X = mX;

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { P(SIZE); }

                    EXPECTED[SIZE] = SPEC[tj];
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P_(SIZE); P(EXPECTED); }

                    CIter hint;
                    switch(CONFIG) {
                      case 'a': {
                          hint = X.begin();
                      } break;
                      case 'b': {
                          hint = X.begin();
                          if (hint != X.end()) {
                              ++hint;
                          }
                      } break;
                      case 'c': {
                          hint = X.end();
                      } break;
                      case 'd': {
                          hint = X.find(VALUES[tj].first);
                          ASSERTV(IS_UNIQ == (hint == X.end()));
                      } break;
                      default: {
                          ASSERTV(!"Unexpected configuration");
                      }
                    }

                    Iter RESULT = mX.insert(hint, VALUES[tj]);

                    ASSERTV(LINE, CONFIG, tj, SIZE, VALUES[tj] == *RESULT);

                    if (CONFIG == 'd' && hint != X.end()) {
                        ASSERTV(LINE, tj, hint == ++RESULT);
                    }

                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                    ASSERTV(LINE, tj, verifySpec(X, EXPECTED));
                }
            }
        }
    }

    if (verbose) printf("\nTesting 'insert' with exception.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P(LENGTH); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                Obj mX(&oa);  const Obj &X = mX;

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { P(SIZE); }

                    EXPECTED[SIZE] = SPEC[tj];
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }

                    CIter hint;
                    switch(CONFIG) {
                      case 'a': {
                          hint = X.begin();
                      } break;
                      case 'b': {
                          hint = X.begin();
                          if (hint != X.end()) {
                              ++hint;
                          }
                      } break;
                      case 'c': {
                          hint = X.end();
                      } break;
                      case 'd': {
                          hint = X.find(VALUES[tj].first);
                          ASSERTV(IS_UNIQ == (hint == X.end()));
                      } break;
                      default: {
                          ASSERTV(!"Unexpected configuration");
                      }
                    }

                    bslma::TestAllocator scratch("scratch",
                                                veryVeryVeryVerbose);

                    Iter RESULT;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ExceptionProctor<Obj, ALLOC> guard(&X, L_, &scratch);

                        RESULT = mX.insert(hint, VALUES[tj]);
                        guard.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE, CONFIG, tj, SIZE, VALUES[tj] == *RESULT);

                    if (CONFIG == 'd' && hint != X.end()) {
                        ASSERTV(LINE, tj, hint == ++RESULT);
                    }

                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                    ASSERTV(LINE, tj, verifySpec(X, EXPECTED));
                }
            }
        }
    }

}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION
    //
    // Concerns:
    //: 1 'insert' adds an additional element to the object and return the
    //:   iterator to the newly added element.
    //:
    //: 2 Inserting duplicated value do not overwrite previous ones.
    //:
    //: 3 Duplicated values are inserted at the end of its range.
    //:
    //: 4 A new element is added to the container.
    //:
    //: 5 Insertion is exception neutral w.r.t. memory allocation.
    //:
    //: 6 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
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
    //: 2 Repeat P-1 under the presence of exception  (C-6)
    //
    // Testing:
    //   iterator insert(const value_type& value);
    // -----------------------------------------------------------------------

    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
        const char *d_unique;  // expected element values
    } DATA[] = {
        //line  spec           unique
        //----  -------------- --------------
        { L_,   "A",           "Y"            },
        { L_,   "AAA",         "YNN"          },
        { L_,   "ABCDEFGH",    "YYYYYYYY"     },
        { L_,   "AABBCCDDEE",  "YNYNYNYNYN"   },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"   },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY"  },
    };
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    const int MAX_LENGTH = 16;

    if (verbose) printf("\nTesting 'insert' without exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa); const Obj &X = mX;

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                const size_t SIZE    = X.size();

                if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                EXPECTED[SIZE] = SPEC[tj];
                EXPECTED[SIZE + 1] = '\0';

                if (veryVeryVerbose) { P(EXPECTED); }

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                Iter RESULT = mX.insert(VALUES[tj]);

                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LINE, tj, AA, BB, BB + TYPE_ALLOC <= AA);
                ASSERTV(LINE, tj,  A,  B,  B + TYPE_ALLOC <= A);
                ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                ASSERTV(LINE, tj, verifySpec(X, EXPECTED));
            }
        }
    }

    if (verbose) printf("\nTesting 'insert' with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj &X = mX;

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const size_t SIZE    = X.size();

                if (veryVerbose) { P(SIZE); }

                EXPECTED[SIZE] = SPEC[tj];
                EXPECTED[SIZE + 1] = '\0';

                if (veryVeryVerbose) { P(EXPECTED); }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Iter RESULT;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionProctor<Obj, ALLOC> guard(&X, L_, &scratch);

                    RESULT = mX.insert(VALUES[tj]);
                    guard.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);

                ASSERTV(LINE, tj, verifySpec(X, EXPECTED));
            }
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING ITERATORS
    //
    // Concerns:
    //: 1 'begin' and 'end' return non-mutable iterators.
    //:
    //: 2 The range '[begin(), end())' contains all values inserted into the
    //:   container.
    //:
    //: 3 The range '[rbegin(), rend())' contains all values inserted into the
    //:   container.
    //:
    //: 4 'iterator' is a pointer to 'const KEY'.
    //:
    //: 5 'const_iterator' is a pointer to 'const KEY'.
    //:
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
    //   const_iterator begin() const;
    //   const_iterator end() const;
    //   const_iterator cbegin() const;
    //   const_iterator cend() const;
    //   local_iterator begin(index);
    //   local_iterator end(index);
    //   const_local_iterator cbegin(index) const;
    //   const_local_iterator cend(index) const;
    //   const_local_iterator begin(index) const;
    //   const_local_iterator end(index) const;
    // ------------------------------------------------------------------------
    BSLMF_ASSERT(1 == (bsl::is_same<typename Iter::pointer,
                                        bsl::pair<const KEY, VALUE>*>::value));
    BSLMF_ASSERT(1 == (bsl::is_same<typename Iter::reference,
                                        bsl::pair<const KEY, VALUE>&>::value));
    BSLMF_ASSERT(1 == (bsl::is_same<typename CIter::pointer,
                                  const bsl::pair<const KEY, VALUE>*>::value));
    BSLMF_ASSERT(1 == (bsl::is_same<typename CIter::reference,
                                  const bsl::pair<const KEY, VALUE>&>::value));

    BSLMF_ASSERT(1 == (bsl::is_same<typename LIter::pointer,
                                        bsl::pair<const KEY, VALUE>*>::value));
    BSLMF_ASSERT(1 == (bsl::is_same<typename LIter::reference,
                                        bsl::pair<const KEY, VALUE>&>::value));
    BSLMF_ASSERT(1 == (bsl::is_same<typename CLIter::pointer,
                                  const bsl::pair<const KEY, VALUE>*>::value));
    BSLMF_ASSERT(1 == (bsl::is_same<typename CLIter::reference,
                                  const bsl::pair<const KEY, VALUE>&>::value));

#if !defined(BSLS_PLATFORM_CMP_MSVC)
# warning "TBD: implementation is missing"
#endif
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase13()
{
    // ------------------------------------------------------------------------
    // TESTING FIND, CONTAINS, EQUAL_RANGE, COUNT
    //
    // Concern:
    //: 1 If the key being searched exists in the container, 'find' returns the
    //:   iterator referring to the existing element and 'contains' returns
    //:   'true'.
    //:
    //: 2 If the key being searched does not exists in the container, 'find'
    //:   returns the 'end' iterator and 'contains' returns 'false'.
    //:
    //: 3 'equal_range(key)' returns all elements equivalent to 'key'.
    //:
    //: 2 'count' returns the number of elements with the same value as defined
    //:   by the comparator.
    //:
    //: 3 Both the 'const' and non-'const' versions returns the same value.
    //:
    //: 4 No memory is allocated, from either the object allocator nor from
    //:   the default allocator.
    //
    // Plan:
    //:  TBD
    //
    // Testing:
    //   bool contains(const key_type& key);
    //   bool contains(const LOOKUP_KEY& key);
    //   size_type count(const key_type& key) const;
    //   bsl::pair<iterator, iterator> equal_range(const key_type& key);
    //   bsl::pair<const_iter, const_iter> equal_range(const key_type&) const;
    //   iterator find(const key_type& key);
    //   const_iterator find(const key_type& key) const;
    // ------------------------------------------------------------------------

    typedef bsl::pair<Iter, Iter>   Range;
    typedef bsl::pair<CIter, CIter> CRange;

    if (veryVerbose) printf("Testing Signatures\n");
    {
        typedef Iter (Obj::*MP)(const KEY&);
        MP mp = &Obj::find;
        (void) mp;
    }
    {
        typedef CIter (Obj::*MP)(const KEY&) const;
        MP mp = &Obj::find;
        (void) mp;
    }
    {
        typedef size_t (Obj::*MP)(const KEY&) const;
        MP mp = &Obj::count;
        (void) mp;
    }
    {
        typedef Range (Obj::*MP)(const KEY&);
        MP mp = &Obj::equal_range;
        (void) mp;
    }
    {
        typedef CRange (Obj::*MP)(const KEY&) const;
        MP mp = &Obj::equal_range;
        (void) mp;
    }

    if (veryVerbose) printf("Testing methods\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::TestAllocator         oa("object",  veryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    TestValues VALUES(&oa);

    for (int i = 0; i < DEFAULT_NUM_DATA; ++i) {
        const char *SPEC = DEFAULT_DATA[i].d_spec;

        if (veryVeryVerbose) { T_ P_(SPEC) }

        Obj mX(&oa); const Obj& X = gg(&mX, SPEC);

        for (char c = 'A'; c < 'Z'; ++c) {
            bslma::TestAllocatorMonitor oam(&oa);
            bslma::TestAllocatorMonitor dam(&da);

            {
                Iter it;
                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;

                    it = mX.find(VALUES[c - 'A'].first);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERT(it == X.find(VALUES[c - 'A'].first));
                ASSERT(1 == numPasses);

                bool cShouldBeFound = it != mX.end();
                ASSERTV(c,
                        cShouldBeFound,
                        cShouldBeFound == mX.contains(VALUES[c - 'A'].first));

                const size_t expectedCount = numCharInstances(SPEC, c);

                if (expectedCount) {
                    ASSERT(VALUES[c - 'A'].first == it->first);
                }
                else {
                    ASSERT(X.end() == it);

                }
                ASSERT(expectedCount == mX.count(VALUES[c - 'A'].first));
                ASSERT(expectedCount ==  X.count(VALUES[c - 'A'].first));
            }

            {
                Range  range;
                CRange crange;
                Iter    it = mX.find(VALUES[c - 'A'].first);
                CIter  cit =  X.find(VALUES[c - 'A'].first);
                {
                    int numPasses = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ++numPasses;

                        range = mX.equal_range(VALUES[c - 'A'].first);
                        crange = X.equal_range(VALUES[c - 'A'].first);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    ASSERT(1 == numPasses);
                }

                ASSERTV(it == range.first);
                ASSERTV(cit == crange.first);

                const size_t expectedCount = numCharInstances(SPEC, c);

                if (expectedCount) {
                    Iter after = range.first;
                    for(size_t count = expectedCount; count > 0; --count) {
                        ASSERT(VALUES[c - 'A'].first == after->first);
                        ++after;
                    }
                    ASSERT(after == range.second);

                    CIter cafter = crange.first;
                    for(size_t count = expectedCount; count > 0; --count) {
                        ASSERT(VALUES[c - 'A'].first == cafter->first);
                        ++cafter;
                    }
                    ASSERT(cafter == crange.second);
                }
                else {
                    ASSERT(range.first  == range.second);
                    ASSERT(X.end()      == range.first);
                    ASSERT(crange.first == crange.second);
                    ASSERT(X.cend()     == crange.first);
                }
            }

            ASSERT(oam.isTotalSame());
            ASSERT(dam.isTotalSame());
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase12()
{
    // ------------------------------------------------------------------------
    // TESTING RANGE CONSTRUCTORS
    //
    // Concern:
    //: 1 That all c'tors taking a range of objects of type 'KEY' function
    //:   correctly.
    //
    // Plan:
    //:  TBD
    //
    // Testing:
    //   unordered_multimap(ITER, ITER, allocator);
    //   unordered_multimap(ITER, ITER, size_t, allocator);
    //   unordered_multimap(ITER, ITER, size_t, hash, allocator);
    //   unordered_multimap(ITER, ITER, size_t, hash, equal, allocator);
    // ------------------------------------------------------------------------

    HASH  tstHash(7);
    EQUAL tstEqual(9);

    ASSERTV(!(HASH()  == tstHash));
    ASSERTV(!(EQUAL() == tstEqual));

    for (size_t i = 0; i < DEFAULT_NUM_DATA; ++i) {
        const int     LINE   = DEFAULT_DATA[i].d_line;
        const char   *SPEC   = DEFAULT_DATA[i].d_spec;
        const size_t  LENGTH = strlen(SPEC);

        TestValues values(SPEC);

        for (char config = 'a'; config <= 'n'; ++config) {
            const char CONFIG = config;

            bslma::TestAllocator sa("scratch",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            bslma::TestAllocator&  oa = (1 == ((CONFIG - 'a') & 1)) ? sa : da;
            bslma::TestAllocator& noa = &oa == &da                  ? sa : da;

            Obj *objPtr;
            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;
                values.resetIterators();

                switch (CONFIG) {
                  case 'a': {
                    objPtr = new (fa) Obj(values.begin(), values.end());
                  } break;
                  case 'b': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), &sa);
                  } break;
                  case 'c': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 0);
                  } break;
                  case 'd': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 0,
                                          &sa);
                  } break;
                  case 'e': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 0,
                                          tstHash);
                  } break;
                  case 'f': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 0,
                                          tstHash, &sa);
                  } break;
                  case 'g': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 0,
                                          tstHash, tstEqual);
                  } break;
                  case 'h': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 0,
                                          tstHash, tstEqual, &sa);
                  } break;
                  case 'i': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 100);
                  } break;
                  case 'j': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 100,
                                          &sa);
                  } break;
                  case 'k': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 100,
                                          tstHash);
                  } break;
                  case 'l': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 100,
                                          tstHash, &sa);
                  } break;
                  case 'm': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 100,
                                          tstHash, tstEqual);
                  } break;
                  case 'n': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 100,
                                          tstHash, tstEqual, &sa);
                  } break;
                  default: {
                    ASSERTV(0 && "unrecognized CONFIG");
                    return;                                           // RETURN
                  }
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            const Obj& X = *objPtr;

            ASSERTV(0 == noa.numBlocksTotal());

            const bool EXP0 = CONFIG <= 'h' && 0 == LENGTH;
            ASSERTV(LINE, CONFIG, EXP0 == (0 == oa.numBlocksInUse()));
            ASSERTV(LINE, CONFIG, EXP0 == (0 == oa.numBlocksTotal()));

            ASSERTV(numPasses, CONFIG, EXP0,
                                  (PLAT_EXC && !EXP0)      == (numPasses > 1));
            ASSERTV(numPasses, CONFIG,
                                  (PLAT_EXC && LENGTH > 0) == (numPasses > 2));

            ASSERTV(LENGTH == X.size());
            ASSERTV(verifySpec(X, SPEC));

            Obj mY; const Obj& Y = gg(&mY, SPEC);
            ASSERTV(X == Y);

            ASSERTV((CONFIG >= 'i') == (X.bucket_count() > 100));

            const HASH& EXP_HASH = strchr("efghklmn", CONFIG) ? tstHash
                                                              : HASH();
            ASSERTV(CONFIG, EXP_HASH == X.hash_function());

            const EQUAL& EXP_EQUAL = strchr("ghmn", CONFIG) ? tstEqual
                                                            : EQUAL();
            ASSERTV(CONFIG, EXP_EQUAL == X.key_eq());

            fa.deleteObject(objPtr);
        }
    }
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
struct TestDeductionGuides {
    // This struct provides a namespace for functions testing deduction guides.
    // The tests are compile-time only; it is not necessary that these routines
    // be called at run-time.  Note that the following constructors do not have
    // associated deduction guides because the template parameters for
    // 'bsl::unordered_multimap' cannot be deduced from the constructor
    // parameters.
    //..
    // unordered_multimap()
    // explicit unordered_multimap(size_t, HASH=HASH(), EQUAL=EQUAL(),
    //                                                  ALLOCATOR=ALLOCATOR());
    // unordered_multimap(size_t, HASH, ALLOCATOR);
    // unordered_multimap(size_t, ALLOCATOR);
    // explicit unordered_multimap(ALLOCATOR);
    //..

    template <class KEY_TYPE>
    struct StupidEqual {
        bool operator()(const KEY_TYPE&, const KEY_TYPE&) const
            // Always return true
        {
            return true;
        }
    };

    template <class KEY_TYPE>
    static size_t StupidEqualFn(const KEY_TYPE&, const KEY_TYPE&)
        // Always return true
    {
        return true;
    }

    template <class KEY_TYPE>
    struct StupidHash {
        size_t operator()(const KEY_TYPE&) const
            // Always hash to bucket #0
        {
            return 0U;
        }
    };

    template <class KEY_TYPE>
    static size_t StupidHashFn(const KEY_TYPE&)
        // Always hash to bucket #0
    {
        return 0U;
    }

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

    void TestConstructors ()
        // Test that constructing a 'bsl::unordered_multimap' from various
        // combinations of arguments deduces the correct type.
        //..
        // unordered_multimap(const unordered_multimap&  s)
        //                                                       -> decltype(s)
        // unordered_multimap(const unordered_multimap&  s, ALLOCATOR)
        //                                                       -> decltype(s)
        // unordered_multimap(      unordered_multimap&& s)
        //                                                       -> decltype(s)
        // unordered_multimap(      unordered_multimap&& s, ALLOCATOR)
        //                                                       -> decltype(s)
        //..
    {
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;

        typedef int T1;
        bsl::unordered_multimap<T1, T1> us1;
        bsl::unordered_multimap         us1a(us1);
        ASSERT_SAME_TYPE(decltype(us1a), bsl::unordered_multimap<T1, T1>);

        typedef float T2;
        typedef bsl::allocator<bsl::pair<const T2, T2>> BA2;

        bsl::unordered_multimap<T2, T2> us2;
        bsl::unordered_multimap         us2a(us2, BA2{});
        bsl::unordered_multimap         us2b(us2, a1);
        bsl::unordered_multimap         us2c(us2, a2);
        bsl::unordered_multimap         us2d(us2, bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(us2a), bsl::unordered_multimap<T2, T2>);
        ASSERT_SAME_TYPE(decltype(us2b), bsl::unordered_multimap<T2, T2>);
        ASSERT_SAME_TYPE(decltype(us2c), bsl::unordered_multimap<T2, T2>);
        ASSERT_SAME_TYPE(decltype(us2d), bsl::unordered_multimap<T2, T2>);

        typedef short T3;
        bsl::unordered_multimap<T3, T3> us3;
        bsl::unordered_multimap         us3a(std::move(us3));
        ASSERT_SAME_TYPE(decltype(us3a), bsl::unordered_multimap<T3, T3>);

        typedef long double T4;
        typedef bsl::allocator<bsl::pair<const T4, T4>> BA4;

        bsl::unordered_multimap<T4, T4> us4;
        bsl::unordered_multimap         us4a(std::move(us4), BA4{});
        bsl::unordered_multimap         us4b(std::move(us4), a1);
        bsl::unordered_multimap         us4c(std::move(us4), a2);
        bsl::unordered_multimap         us4d(std::move(us4),
                                             bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(us4a), bsl::unordered_multimap<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4b), bsl::unordered_multimap<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4c), bsl::unordered_multimap<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4d), bsl::unordered_multimap<T4, T4>);
    }

    void TestIteratorConstructors ()
        // Test that constructing a 'bsl::unordered_multimap' from a pair of
        // iterators and various combinations of other arguments deduces the
        // correct type.
        //..
        // unordered_multimap(Iter, Iter, size_type = N, HASH=HASH(),
        //                               EQUAL=EQUAL(), ALLOCATOR=ALLOCATOR());
        // unordered_multimap(Iter, Iter, size_type, HASH, ALLOCATOR);
        // unordered_multimap(Iter, Iter, size_type, ALLOCATOR);
        // unordered_multimap(Iter, Iter, ALLOCATOR)
        //..
    {
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;

        typedef long                                    T1;
        typedef StupidHash<T1>                          HashT1;
        typedef StupidEqual<T1>                         EqualT1;
        typedef decltype(StupidHashFn<T1>)              HashFnT1;
        typedef decltype(StupidEqualFn<T1>)             EqualFnT1;
        typedef bsl::allocator<bsl::pair<const T1, T1>> BA1;
        typedef std::allocator<bsl::pair<const T1, T1>> SA1;

        bsl::pair<T1, T1>                         *p1b = nullptr;
        bsl::pair<T1, T1>                         *p1e = nullptr;
        bsl::unordered_multimap<T1, T1>::iterator  i1b;
        bsl::unordered_multimap<T1, T1>::iterator  i1e;

        bsl::unordered_multimap us1a(p1b, p1e);
        bsl::unordered_multimap us1b(i1b, i1e);
        bsl::unordered_multimap us1c(p1b, p1e, 3);
        bsl::unordered_multimap us1d(i1b, i1e, 3);

        bsl::unordered_multimap us1e(p1b, p1e, 3, HashT1{});
        bsl::unordered_multimap us1f(p1b, p1e, 3, StupidHashFn<T1>);
        bsl::unordered_multimap us1g(i1b, i1e, 3, HashT1{});
        bsl::unordered_multimap us1h(i1b, i1e, 3, StupidHashFn<T1>);

        bsl::unordered_multimap us1i(p1b, p1e, 3, HashT1{}, EqualT1{});
        bsl::unordered_multimap us1j(i1b, i1e, 3, HashT1{}, StupidEqualFn<T1>);
        bsl::unordered_multimap us1k(p1b, p1e, 3, StupidHashFn<T1>, EqualT1{});
        bsl::unordered_multimap us1l(i1b, i1e, 3,
                                          StupidHashFn<T1>, StupidEqualFn<T1>);

        bsl::unordered_multimap us1m(p1b, p1e, 3, HashT1{}, EqualT1{}, BA1{});
        bsl::unordered_multimap us1n(p1b, p1e, 3, HashT1{}, EqualT1{}, a1);
        bsl::unordered_multimap us1o(p1b, p1e, 3, HashT1{}, EqualT1{}, a2);
        bsl::unordered_multimap us1p(p1b, p1e, 3, HashT1{}, EqualT1{}, SA1{});
        bsl::unordered_multimap us1q(i1b, i1e, 3, HashT1{}, EqualT1{}, BA1{});
        bsl::unordered_multimap us1r(i1b, i1e, 3, HashT1{}, EqualT1{}, a1);
        bsl::unordered_multimap us1s(i1b, i1e, 3, HashT1{}, EqualT1{}, a2);
        bsl::unordered_multimap us1t(i1b, i1e, 3, HashT1{}, EqualT1{}, SA1{});

        ASSERT_SAME_TYPE(decltype(us1a), bsl::unordered_multimap<T1, T1>);
        ASSERT_SAME_TYPE(decltype(us1b), bsl::unordered_multimap<T1, T1>);
        ASSERT_SAME_TYPE(decltype(us1c), bsl::unordered_multimap<T1, T1>);
        ASSERT_SAME_TYPE(decltype(us1d), bsl::unordered_multimap<T1, T1>);
        ASSERT_SAME_TYPE(decltype(us1e),
                         bsl::unordered_multimap<T1, T1, HashT1>);
        ASSERT_SAME_TYPE(decltype(us1f),
                         bsl::unordered_multimap<T1, T1, HashFnT1 *>);
        ASSERT_SAME_TYPE(decltype(us1g),
                         bsl::unordered_multimap<T1, T1, HashT1>);
        ASSERT_SAME_TYPE(decltype(us1h),
                         bsl::unordered_multimap<T1, T1, HashFnT1 *>);
        ASSERT_SAME_TYPE(decltype(us1i),
                         bsl::unordered_multimap<T1, T1, HashT1, EqualT1>);
        ASSERT_SAME_TYPE(decltype(us1j),
                         bsl::unordered_multimap<T1, T1, HashT1, EqualFnT1 *>);
        ASSERT_SAME_TYPE(decltype(us1k),
                         bsl::unordered_multimap<T1, T1, HashFnT1 *, EqualT1>);
        ASSERT_SAME_TYPE(
                     decltype(us1l),
                     bsl::unordered_multimap<T1, T1, HashFnT1 *, EqualFnT1 *>);

        ASSERT_SAME_TYPE(
                        decltype(us1m),
                        bsl::unordered_multimap<T1, T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(
                        decltype(us1n),
                        bsl::unordered_multimap<T1, T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(
                        decltype(us1o),
                        bsl::unordered_multimap<T1, T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(
                        decltype(us1p),
                        bsl::unordered_multimap<T1, T1, HashT1, EqualT1, SA1>);
        ASSERT_SAME_TYPE(
                        decltype(us1q),
                        bsl::unordered_multimap<T1, T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(
                        decltype(us1r),
                        bsl::unordered_multimap<T1, T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(
                        decltype(us1s),
                        bsl::unordered_multimap<T1, T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(
                        decltype(us1t),
                        bsl::unordered_multimap<T1, T1, HashT1, EqualT1, SA1>);

        typedef double                      T2;
        typedef StupidHash<T2>              HashT2;
        typedef decltype(StupidHashFn<T2>)  HashFnT2;
        typedef bsl::allocator<bsl::pair<const T2, T2>> BA2;
        typedef std::allocator<bsl::pair<const T2, T2>> SA2;
        bsl::pair<T2, T2>                         *p2b = nullptr;
        bsl::pair<T2, T2>                         *p2e = nullptr;
        bsl::unordered_multimap<T2, T2>::iterator  i2b;
        bsl::unordered_multimap<T2, T2>::iterator  i2e;

        bsl::unordered_multimap us2a(p2b, p2e, 3, HashT2{}, BA2{});
        bsl::unordered_multimap us2b(p2b, p2e, 3, HashT2{}, a1);
        bsl::unordered_multimap us2c(p2b, p2e, 3, HashT2{}, a2);
        bsl::unordered_multimap us2d(p2b, p2e, 3, HashT2{}, SA2{});
        bsl::unordered_multimap us2e(p2b, p2e, 3, StupidHashFn<T2>, BA2{});
        bsl::unordered_multimap us2f(p2b, p2e, 3, StupidHashFn<T2>, a1);
        bsl::unordered_multimap us2g(p2b, p2e, 3, StupidHashFn<T2>, a2);
        bsl::unordered_multimap us2h(p2b, p2e, 3, StupidHashFn<T2>, SA2{});
        bsl::unordered_multimap us2i(i2b, i2e, 3, HashT2{}, BA2{});
        bsl::unordered_multimap us2j(i2b, i2e, 3, HashT2{}, a1);
        bsl::unordered_multimap us2k(i2b, i2e, 3, HashT2{}, a2);
        bsl::unordered_multimap us2l(i2b, i2e, 3, HashT2{}, SA2{});
        bsl::unordered_multimap us2m(i2b, i2e, 3, StupidHashFn<T2>, BA2{});
        bsl::unordered_multimap us2n(i2b, i2e, 3, StupidHashFn<T2>, a1);
        bsl::unordered_multimap us2o(i2b, i2e, 3, StupidHashFn<T2>, a2);
        bsl::unordered_multimap us2p(i2b, i2e, 3, StupidHashFn<T2>, SA2{});

        ASSERT_SAME_TYPE(decltype(us2a),
                         bsl::unordered_multimap<T2, T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(us2b),
                         bsl::unordered_multimap<T2, T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(us2c),
                         bsl::unordered_multimap<T2, T2, HashT2>);
        ASSERT_SAME_TYPE(
              decltype(us2d),
              bsl::unordered_multimap<T2, T2, HashT2, bsl::equal_to<T2>, SA2>);
        ASSERT_SAME_TYPE(decltype(us2e),
                         bsl::unordered_multimap<T2, T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2f),
                         bsl::unordered_multimap<T2, T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2g),
                         bsl::unordered_multimap<T2, T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(
          decltype(us2h),
          bsl::unordered_multimap<T2, T2, HashFnT2 *, bsl::equal_to<T2>, SA2>);
        ASSERT_SAME_TYPE(decltype(us2i),
                         bsl::unordered_multimap<T2, T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(us2j),
                         bsl::unordered_multimap<T2, T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(us2k),
                         bsl::unordered_multimap<T2, T2, HashT2>);
        ASSERT_SAME_TYPE(
              decltype(us2l),
              bsl::unordered_multimap<T2, T2, HashT2, bsl::equal_to<T2>, SA2>);
        ASSERT_SAME_TYPE(decltype(us2m),
                         bsl::unordered_multimap<T2, T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2n),
                         bsl::unordered_multimap<T2, T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2o),
                         bsl::unordered_multimap<T2, T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(
          decltype(us2p),
          bsl::unordered_multimap<T2, T2, HashFnT2 *, bsl::equal_to<T2>, SA2>);

        typedef int                T3;
        typedef bsl::allocator<bsl::pair<const T3, T3>> BA3;
        typedef std::allocator<bsl::pair<const T3, T3>> SA3;
        bsl::pair<T3, T3>                         *p3b = nullptr;
        bsl::pair<T3, T3>                         *p3e = nullptr;
        bsl::unordered_multimap<T3, T3>::iterator  i3b;
        bsl::unordered_multimap<T3, T3>::iterator  i3e;

        bsl::unordered_multimap us3a(p3b, p3e, 3, BA3{});
        bsl::unordered_multimap us3b(p3b, p3e, 3, a1);
        bsl::unordered_multimap us3c(p3b, p3e, 3, a2);
        bsl::unordered_multimap us3d(p3b, p3e, 3, SA3{});
        bsl::unordered_multimap us3e(i3b, i3e, 3, BA3{});
        bsl::unordered_multimap us3f(i3b, i3e, 3, a1);
        bsl::unordered_multimap us3g(i3b, i3e, 3, a2);
        bsl::unordered_multimap us3h(i3b, i3e, 3, SA3{});

        ASSERT_SAME_TYPE(decltype(us3a), bsl::unordered_multimap<T3, T3>);
        ASSERT_SAME_TYPE(decltype(us3b), bsl::unordered_multimap<T3, T3>);
        ASSERT_SAME_TYPE(decltype(us3c), bsl::unordered_multimap<T3, T3>);
        ASSERT_SAME_TYPE(decltype(us3d),
                         bsl::unordered_multimap<T3,
                                                 T3,
                                                 bsl::hash<T3>,
                                                 bsl::equal_to<T3>,
                                                 SA3>);
        ASSERT_SAME_TYPE(decltype(us3e), bsl::unordered_multimap<T3, T3>);
        ASSERT_SAME_TYPE(decltype(us3f), bsl::unordered_multimap<T3, T3>);
        ASSERT_SAME_TYPE(decltype(us3g), bsl::unordered_multimap<T3, T3>);
        ASSERT_SAME_TYPE(decltype(us3h),
                         bsl::unordered_multimap<T3,
                                                 T3,
                                                 bsl::hash<T3>,
                                                 bsl::equal_to<T3>,
                                                 SA3>);

        typedef char               T4;
        typedef bsl::allocator<bsl::pair<const T4, T4>> BA4;
        typedef std::allocator<bsl::pair<const T4, T4>> SA4;
        bsl::pair<T4, T4>                         *p4b = nullptr;
        bsl::pair<T4, T4>                         *p4e = nullptr;
        bsl::unordered_multimap<T4, T4>::iterator  i4b;
        bsl::unordered_multimap<T4, T4>::iterator  i4e;

        bsl::unordered_multimap us4a(p4b, p4e, BA4{});
        bsl::unordered_multimap us4b(p4b, p4e, a1);
        bsl::unordered_multimap us4c(p4b, p4e, a2);
        bsl::unordered_multimap us4d(p4b, p4e, SA4{});
        bsl::unordered_multimap us4e(i4b, i4e, BA4{});
        bsl::unordered_multimap us4f(i4b, i4e, a1);
        bsl::unordered_multimap us4g(i4b, i4e, a2);
        bsl::unordered_multimap us4h(i4b, i4e, SA4{});

        ASSERT_SAME_TYPE(decltype(us4a), bsl::unordered_multimap<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4b), bsl::unordered_multimap<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4c), bsl::unordered_multimap<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4d),
                         bsl::unordered_multimap<T4,
                                                 T4,
                                                 bsl::hash<T4>,
                                                 bsl::equal_to<T4>,
                                                 SA4>);

        ASSERT_SAME_TYPE(decltype(us4e), bsl::unordered_multimap<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4f), bsl::unordered_multimap<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4g), bsl::unordered_multimap<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4h),
                         bsl::unordered_multimap<T4,
                                                 T4,
                                                 bsl::hash<T4>,
                                                 bsl::equal_to<T4>,
                                                 SA4>);
    }

    void TestStdInitializerListConstructors ()
        // Test that constructing a 'bsl::unordered_multimap' from an
        // initializer_list and various combinations of other arguments deduces
        // the correct type.
        //..
        // unordered_multimap(initializer_list, size_type = N, HASH=HASH(),
        //                               EQUAL=EQUAL(), ALLOCATOR=ALLOCATOR());
        // unordered_multimap(initializer_list, size_type, HASH, ALLOCATOR);
        // unordered_multimap(initializer_list, size_type, ALLOCATOR);
        // unordered_multimap(initializer_list, ALLOCATOR)
        //..
    {
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;

        typedef long                                           T1;
        typedef bsl::allocator<bsl::pair<const T1, T1>>        BA1;
        typedef std::allocator<bsl::pair<const T1, T1>>        SA1;
        typedef std::initializer_list<bsl::pair<const T1, T1>> IL1;

        typedef StupidHash<T1>              HashT1;
        typedef StupidEqual<T1>             EqualT1;
        typedef decltype(StupidHashFn<T1>)  HashFnT1;
        typedef decltype(StupidEqualFn<T1>) EqualFnT1;
        IL1 il1 = {{1L, 2L}, {3L, 4L}};

        bsl::unordered_multimap us1a(il1);
        bsl::unordered_multimap us1b(il1, 3);
        bsl::unordered_multimap us1c(il1, 3, HashT1{});
        bsl::unordered_multimap us1d(il1, 3, StupidHashFn<T1>);
        bsl::unordered_multimap us1e(il1, 3, HashT1{}, EqualT1{});
        bsl::unordered_multimap us1f(il1, 3, HashT1{}, StupidEqualFn<T1>);
        bsl::unordered_multimap us1g(il1, 3, StupidHashFn<T1>, EqualT1{});
        bsl::unordered_multimap us1h(il1,
                                     3,
                                     StupidHashFn<T1>,
                                     StupidEqualFn<T1>);

        bsl::unordered_multimap us1i(il1, 3, HashT1{}, EqualT1{}, BA1{});
        bsl::unordered_multimap us1j(il1, 3, HashT1{}, EqualT1{}, a1);
        bsl::unordered_multimap us1k(il1, 3, HashT1{}, EqualT1{}, a2);
        bsl::unordered_multimap us1l(il1, 3, HashT1{}, EqualT1{}, SA1{});

        ASSERT_SAME_TYPE(decltype(us1a), bsl::unordered_multimap<T1, T1>);
        ASSERT_SAME_TYPE(decltype(us1b), bsl::unordered_multimap<T1, T1>);
        ASSERT_SAME_TYPE(decltype(us1c),
                         bsl::unordered_multimap<T1, T1, HashT1>);
        ASSERT_SAME_TYPE(decltype(us1d),
                         bsl::unordered_multimap<T1, T1, HashFnT1 *>);
        ASSERT_SAME_TYPE(decltype(us1e),
                         bsl::unordered_multimap<T1, T1, HashT1, EqualT1>);
        ASSERT_SAME_TYPE(decltype(us1f),
                         bsl::unordered_multimap<T1, T1, HashT1, EqualFnT1 *>);
        ASSERT_SAME_TYPE(decltype(us1g),
                         bsl::unordered_multimap<T1, T1, HashFnT1 *, EqualT1>);
        ASSERT_SAME_TYPE(
                     decltype(us1h),
                     bsl::unordered_multimap<T1, T1, HashFnT1 *, EqualFnT1 *>);

        ASSERT_SAME_TYPE(
                        decltype(us1i),
                        bsl::unordered_multimap<T1, T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(
                        decltype(us1j),
                        bsl::unordered_multimap<T1, T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(
                        decltype(us1k),
                        bsl::unordered_multimap<T1, T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(
                        decltype(us1l),
                        bsl::unordered_multimap<T1, T1, HashT1, EqualT1, SA1>);

        typedef double                                         T2;
        typedef bsl::allocator<bsl::pair<const T2, T2>>        BA2;
        typedef std::allocator<bsl::pair<const T2, T2>>        SA2;
        typedef std::initializer_list<bsl::pair<const T2, T2>> IL2;
        typedef StupidHash<T2>                                 HashT2;
        typedef decltype(StupidHashFn<T2>)                     HashFnT2;
        IL2 il2 = {{1.0, 2.0}, {3.0, 4.0}};

        bsl::unordered_multimap us2a(il2, 3, HashT2{}, BA2{});
        bsl::unordered_multimap us2b(il2, 3, HashT2{}, a1);
        bsl::unordered_multimap us2c(il2, 3, HashT2{}, a2);
        bsl::unordered_multimap us2d(il2, 3, HashT2{}, SA2{});
        bsl::unordered_multimap us2e(il2, 3, StupidHashFn<T2>, BA2{});
        bsl::unordered_multimap us2f(il2, 3, StupidHashFn<T2>, a1);
        bsl::unordered_multimap us2g(il2, 3, StupidHashFn<T2>, a2);
        bsl::unordered_multimap us2h(il2, 3, StupidHashFn<T2>, SA2{});

        ASSERT_SAME_TYPE(decltype(us2a),
                         bsl::unordered_multimap<T2, T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(us2b),
                         bsl::unordered_multimap<T2, T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(us2c),
                         bsl::unordered_multimap<T2, T2, HashT2>);
        ASSERT_SAME_TYPE(
              decltype(us2d),
              bsl::unordered_multimap<T2, T2, HashT2, bsl::equal_to<T2>, SA2>);
        ASSERT_SAME_TYPE(decltype(us2e),
                         bsl::unordered_multimap<T2, T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2f),
                         bsl::unordered_multimap<T2, T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2g),
                         bsl::unordered_multimap<T2, T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2h),
                         bsl::unordered_multimap<T2,
                                                 T2,
                                                 HashFnT2 *,
                                                 bsl::equal_to<T2>,
                                                 SA2>);

        typedef int                                            T3;
        typedef bsl::allocator<bsl::pair<const T3, T3>>        BA3;
        typedef std::allocator<bsl::pair<const T3, T3>>        SA3;
        typedef std::initializer_list<bsl::pair<const T3, T3>> IL3;
        IL3 il3 = {{1, 2}, {3, 4}};

        bsl::unordered_multimap us3a(il3, 3, BA3{});
        bsl::unordered_multimap us3b(il3, 3, a1);
        bsl::unordered_multimap us3c(il3, 3, a2);
        bsl::unordered_multimap us3d(il3, 3, SA3{});

        ASSERT_SAME_TYPE(decltype(us3a), bsl::unordered_multimap<T3, T3>);
        ASSERT_SAME_TYPE(decltype(us3b), bsl::unordered_multimap<T3, T3>);
        ASSERT_SAME_TYPE(decltype(us3c), bsl::unordered_multimap<T3, T3>);
        ASSERT_SAME_TYPE(decltype(us3d),
                         bsl::unordered_multimap<T3,
                                                 T3,
                                                 bsl::hash<T3>,
                                                 bsl::equal_to<T3>,
                                                 SA3>);

        typedef char                                           T4;
        typedef bsl::allocator<bsl::pair<const T4, T4>>        BA4;
        typedef std::allocator<bsl::pair<const T4, T4>>        SA4;
        typedef std::initializer_list<bsl::pair<const T4, T4>> IL4;
        IL4 il4 = {{'1', '2'}, {'3', '4'}};

        bsl::unordered_multimap us4a(il4, BA4{});
        bsl::unordered_multimap us4b(il4, a1);
        bsl::unordered_multimap us4c(il4, a2);
        bsl::unordered_multimap us4d(il4, SA4{});

        ASSERT_SAME_TYPE(decltype(us4a), bsl::unordered_multimap<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4b), bsl::unordered_multimap<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4c), bsl::unordered_multimap<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4d),
                         bsl::unordered_multimap<T4,
                                                 T4,
                                                 bsl::hash<T4>,
                                                 bsl::equal_to<T4>,
                                                 SA4>);
    }

#undef ASSERT_SAME_TYPE
};
#endif

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
      case 39: {
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
        //: 1 Create an unordered_map by invoking the constructor without
        //:   supplying the template arguments explicitly.
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
        (void) test; // This variable only exits for ease of IDE navigation
#endif
      } break;
      case 38: {
        // --------------------------------------------------------------------
        // TESTING TRANSPARENT COMPARATOR
        //
        // Concerns:
        //: 1 'unordered_multimap' has does not have a transparent set of
        //:   lookup functions if the comparator is not transparent.
        //:
        //: 2 'unordered_multimap' has a transparent set of lookup functions if
        //:   the comparator is transparent.
        //
        // Plan:
        //: 1 Construct a non-transparent unordered_multimap and call the
        //:   lookup functions with a type that is convertible to the
        //:   'key_type'.  There should be exactly one conversion per call to a
        //:   lookup function.  (C-1)
        //:
        //: 2 Construct a transparent unordered_multimap and call the lookup
        //:   functions with a type that is convertible to the 'key_type'.
        //:   There should be no conversions.  (C-2)
        //
        // Testing:
        //   CONCERN: 'find'        properly handles transparent comparators.
        //   CONCERN: 'count'       properly handles transparent comparators.
        //   CONCERN: 'equal_range' properly handles transparent comparators.
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "TESTING TRANSPARENT COMPARATOR" "\n"
                                 "==============================" "\n");

        typedef bsl::unordered_multimap<int, int>
                                                        NonTransparentMultimap;
        typedef NonTransparentMultimap::value_type      Value;
        typedef bsl::unordered_multimap<int, int,
                                  TransparentHasher, TransparentComparator>
                                                        TransparentMultimap;

        const int DATA[] = { 0, 1, 2, 3, 4 };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        NonTransparentMultimap        mXNT;
        const NonTransparentMultimap& XNT = mXNT;

        mXNT.insert(Value (0, 0));
        for (int i = 0; i < NUM_DATA; ++i) {
            for (int j = 0; j < i; ++j) {
                if (veryVeryVeryVerbose) {
                    printf("Constructing test data.\n");
                }
                const Value VALUE(DATA[i], DATA[j]);
                mXNT.insert(VALUE);
            }
        }

        TransparentMultimap        mXT(mXNT.begin(), mXNT.end());
        const TransparentMultimap& XT = mXT;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int KEY = DATA[i];
            if (veryVerbose) {
                printf("Testing transparent comparators with a key of %d\n",
                       KEY);
            }

            if (veryVerbose) {
                printf("\tTesting const non-transparent multimap.\n");
            }
            testTransparentComparator( XNT, false, KEY);

            if (veryVerbose) {
                printf("\tTesting mutable non-transparent multimap.\n");
            }
            testTransparentComparator(mXNT, false, KEY);

            if (veryVerbose) {
                printf("\tTesting const transparent multimap.\n");
            }
            testTransparentComparator( XT,  true,  KEY);

            if (veryVerbose) {
                printf("\tTesting mutable transparent multimap.\n");
            }
            testTransparentComparator(mXT,  true,  KEY);
        }
      } break;
      case 37: {
        // --------------------------------------------------------------------
        // 'erase' overload is deduced correctly
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING ABSENCE OF ERASE AMBIGUITY\n"
                            "==================================\n");

        TestDriver<EraseAmbiguityTestType, int>::testCase37();
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
        //   CONCERN: 'unordered_multimap' supports incomplete types.
        // --------------------------------------------------------------------

        TestIncompleteType x;
        (void) x;
      } break;
      case 34: {
        // --------------------------------------------------------------------
        // GROWING FUNCTIONS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Growing Functions\n"
                            "=========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase34,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);
      } break;
      case 33: {
        // --------------------------------------------------------------------
        // TESTING TYPEDEFS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Typedefs\n"
                            "================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase33,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);
      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING METHODS TAKING INITIALIZER LISTS
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase32_outOfLine,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        RUN_EACH_TYPE(TestDriver,
                      testCase32_inline,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // TESTING EMPLACE WITH HINT
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
        // TESTING EMPLACE
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase30,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonDefaultConstructibleTestType,
                      bsltf::NonOptionalAllocTestType);

        RUN_EACH_TYPE(TestDriver,
                      testCase30a,
                      bsltf::EmplacableTestType,
                      bsltf::AllocEmplacableTestType);
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING INSERTION WITH HINT MOVABLE VALUES
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase29,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        TestDriver<int,
                   bsltf::MoveOnlyAllocTestType>::testCase29();
        TestDriver<bsltf::MovableTestType,
                   bsltf::MoveOnlyAllocTestType>::testCase29();

        TestDriver<int,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::testCase29();
        TestDriver<bsltf::MovableTestType,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::testCase29();

        //RUN_EACH_TYPE(StdBslmaTestDriver,
        //              testCase29,
        //              bsltf::StdAllocTestType<bsl::allocator<int> >);
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING INSERTION ON MOVABLE VALUES
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase28,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        TestDriver<int,
                   bsltf::MoveOnlyAllocTestType>::testCase28();
        TestDriver<bsltf::MovableTestType,
                   bsltf::MoveOnlyAllocTestType>::testCase28();

        TestDriver<int,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::testCase28();
        TestDriver<bsltf::MovableTestType,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::testCase28();

        //RUN_EACH_TYPE(StdBslmaTestDriver,
        //              testCase28,
        //              bsltf::StdAllocTestType<bsl::allocator<int> >);
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING MOVE ASSIGNMENT
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase27,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        TestDriver<int,
                   bsltf::MoveOnlyAllocTestType>::testCase27();
        TestDriver<bsltf::MovableTestType,
                   bsltf::MoveOnlyAllocTestType>::testCase27();

        TestDriver<int,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::testCase27();
        TestDriver<bsltf::MovableTestType,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::testCase27();

        //RUN_EACH_TYPE(StdBslmaTestDriver,
        //              testCase27,
        //              bsltf::StdAllocTestType<bsl::allocator<int> >);

        // 'propagate_on_container_move_assignment' testing

        RUN_EACH_TYPE(TestDriver,
                      testCase27_propagate_on_container_move_assignment,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

// TBD get this working?
#if 0
        TestDriver<int, bsltf::MoveOnlyAllocTestType>::
                           testCase27_propagate_on_container_move_assignment();

        TestDriver<bsltf::MovableAllocTestType,
                   bsltf::MoveOnlyAllocTestType>::
                           testCase27_propagate_on_container_move_assignment();

        TestDriver<int,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::
                           testCase27_propagate_on_container_move_assignment();

        TestDriver<bsltf::MovableAllocTestType,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::
                           testCase27_propagate_on_container_move_assignment();
#endif

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
        // Test noexcept
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false> Alloc;
            typedef bsl::hash<int> Hash;
            typedef bsl::equal_to<int> Equal;

            ASSERT(!bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT( std::is_nothrow_move_assignable<Hash>::value);
            ASSERT( std::is_nothrow_move_assignable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase27_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false> Alloc;
            typedef ThrowingMoveHash<int> Hash;
            typedef bsl::equal_to<int> Equal;

            ASSERT(!bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT(!std::is_nothrow_move_assignable<Hash>::value);
            ASSERT( std::is_nothrow_move_assignable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase27_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false> Alloc;
            typedef bsl::hash<int> Hash;
            typedef ThrowingMoveEqual<int> Equal;

            ASSERT(!bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT( std::is_nothrow_move_assignable<Hash>::value);
            ASSERT(!std::is_nothrow_move_assignable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase27_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false> Alloc;
            typedef ThrowingMoveHash<int> Hash;
            typedef ThrowingMoveEqual<int> Equal;

            ASSERT(!bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT(!std::is_nothrow_move_assignable<Hash>::value);
            ASSERT(!std::is_nothrow_move_assignable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase27_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true> Alloc;
            typedef bsl::hash<int> Hash;
            typedef bsl::equal_to<int> Equal;

            ASSERT( bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT( std::is_nothrow_move_assignable<Hash>::value);
            ASSERT( std::is_nothrow_move_assignable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase27_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true> Alloc;
            typedef ThrowingMoveHash<int> Hash;
            typedef bsl::equal_to<int> Equal;

            ASSERT( bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT(!std::is_nothrow_move_assignable<Hash>::value);
            ASSERT( std::is_nothrow_move_assignable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase27_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true> Alloc;
            typedef bsl::hash<int> Hash;
            typedef ThrowingMoveEqual<int> Equal;

            ASSERT( bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT( std::is_nothrow_move_assignable<Hash>::value);
            ASSERT(!std::is_nothrow_move_assignable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase27_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true> Alloc;
            typedef ThrowingMoveHash<int> Hash;
            typedef ThrowingMoveEqual<int> Equal;

            ASSERT( bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT(!std::is_nothrow_move_assignable<Hash>::value);
            ASSERT(!std::is_nothrow_move_assignable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase27_noexcept();
        }
#endif
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING MOVE CONSTRUCTION
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase26,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        // Since 'KEY' is 'const', copy c'tor of 'KEY' must be used to insert
        // elements, so cannot have move-only 'KEY'.

        TestDriver<int,
                   bsltf::MoveOnlyAllocTestType>::testCase26();
        TestDriver<bsltf::MovableTestType,
                   bsltf::MoveOnlyAllocTestType>::testCase26();

        TestDriver<int,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::testCase26();
        TestDriver<bsltf::MovableTestType,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::testCase26();

        //RUN_EACH_TYPE(StdBslmaTestDriver,
        //              testCase26,
        //              bsltf::StdAllocTestType<bsl::allocator<int> >);
      } break;
      case 25: {
        // Placeholder to keep numbering.
      } break;
      case 24: {
        // Placeholder to keep numbering.
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS AND TYPE DEFS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase23,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        //            bsltf::MoveOnlyAllocTestType -- no copy

        TestDriver<TestKeyType, TestValueType>::testCase23();
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // OBJECT WITH STL ALLOCATOR
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Object With STL Allocator\n"
                            "=================================\n");

        RUN_EACH_TYPE(StdAllocTestDriver,
                      testCase22,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        //            bsltf::MoveOnlyAllocTestType -- no copy
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING COMPARATOR
        // --------------------------------------------------------------------

        if (verbose) printf("Testing 'HASH' and 'EQUAL'\n"
                            "==========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase21,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        //            bsltf::MoveOnlyAllocTestType -- no copy

        TestDriver<TestKeyType, TestValueType>::testCase21();
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'max_size' and 'empty'
        // --------------------------------------------------------------------

        if (verbose) printf("Testing 'max_size' and 'empty'\n"
                            "==============================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase20,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        //            bsltf::MoveOnlyAllocTestType -- no copy

        TestDriver<TestKeyType, TestValueType>::testCase20();
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING FREE COMPARISON OPERATORS
        //
        // < <= > >= -- N/A for this container
        // --------------------------------------------------------------------
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'erase'
        // --------------------------------------------------------------------

        if (verbose) printf("Testing 'erase'\n"
                            "===============\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase18,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        //            bsltf::MoveOnlyAllocTestType -- no copy

        TestDriver<TestKeyType, TestValueType>::testCase18();
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING RANGE INSERT
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase17,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        //            bsltf::MoveOnlyAllocTestType -- no copy

        TestDriver<TestKeyType, TestValueType>::testCase17();
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'insert' with hint
        // --------------------------------------------------------------------

        if (verbose) printf("Testing 'insert' with hint\n"
                            "==========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase16,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        TestDriver<TestKeyType, TestValueType>::testCase16();
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'insert' SINGLE VALUE.
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase15,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        //            bsltf::MoveOnlyAllocTestType -- no copy

        TestDriver<TestKeyType, TestValueType>::testCase15();
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ITERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing iterators\n"
                            "=================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'find', 'contains', 'equal_range', 'count'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        TestDriver<TestKeyType, TestValueType>::testCase13();
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING RANGE CONSTRUCTORS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Range Constructors\n"
                            "==========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);
      } break;
      case 11: // falls through
      case 10: // falls through
      case  9: // falls through
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
