// bslstl_iteratorutil.t.cpp                                          -*-C++-*-

#include <bslstl_iteratorutil.h>

#include <bslstl_concepts.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(BSLS_PLATFORM_CMP_SUN)
#pragma error_messages(off, SEC_UNINITIALIZED_MEM_READ)
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)
#pragma warning(disable:4312)
#endif

using namespace BloombergLP;
using namespace bsl;
using namespace bslstl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// `bslstl::IteratorUtil` provides a namespace for a suite of utility functions
// for iterator types.  This test driver verifies that each of the functions
// behaves as documented.
//-----------------------------------------------------------------------------
// [ 3] bool canCalculateInsertDistance
// [ 3] size_t insertDistance(InputIterator, InputIterator)
// [ 3] size_t insertDistance(InputIterator, Sentinel)
// [ 4] ALIAS TEMPLATES FOR DEDUCTION GUIDES
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TEST INFRASTRUCTURE
// [ 5] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

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
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//             GLOBAL TYPEDEFS, FUNCTIONS AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

typedef IteratorUtil Obj;

namespace {

                        // ==================
                        // IteratorUsageCount
                        // ==================

/// This simple aggregate contains a number of counts for potential operations
/// that can be performed on a test iterator, along with some additional
/// telemetry that might be useful if and when the associated family of test
/// iterators is extracted for reuse in more components.
struct IteratorUsageCount
{
    IteratorUsageCount()
    : d_postIncrement(0)
    , d_postDecrement(0)
    , d_preIncrement(0)
    , d_preDecrement(0)
    , d_adds(0)
    , d_subtracts(0)
    , d_creations(0)
    , d_copies(0)
    , d_moves(0)
    , d_copyAssignments(0)
    , d_moveAssignments(0)
    , d_copyFroms(0)
    , d_moveFroms(0)
    , d_difference(0)
    , d_compares(0)
    , d_dereferences(0)
    , d_subscripting(0)
    {
    }

    void reset()
    {
        *this = IteratorUsageCount();
    }

    int d_postIncrement;
    int d_postDecrement;
    int d_preIncrement;
    int d_preDecrement;
    int d_adds;
    int d_subtracts;
    int d_creations;
    int d_copies;
    int d_moves;
    int d_copyAssignments;
    int d_moveAssignments;
    int d_copyFroms;
    int d_moveFroms;
    int d_difference;
    int d_compares;
    int d_dereferences;
    int d_subscripting;

    static IteratorUsageCount s_defaultUsage;
};

void debugprint(const IteratorUsageCount& c)
{
#define PRINTMEMBER(X) \
    if (0 != c.d_##X) { \
        if (first) { first = false; } \
        else { printf(" "); } \
        printf("%s:%d", #X, c.d_##X); \
    }

    bool first = true;
    printf("counts(");
    PRINTMEMBER(postIncrement);
    PRINTMEMBER(postDecrement);
    PRINTMEMBER(preIncrement);
    PRINTMEMBER(preDecrement);
    PRINTMEMBER(adds);
    PRINTMEMBER(subtracts);
    PRINTMEMBER(creations);
    PRINTMEMBER(copies);
    PRINTMEMBER(moves);
    PRINTMEMBER(copyAssignments);
    PRINTMEMBER(moveAssignments);
    PRINTMEMBER(copyFroms);
    PRINTMEMBER(moveFroms);
    PRINTMEMBER(difference);
    PRINTMEMBER(compares);
    PRINTMEMBER(dereferences);
    PRINTMEMBER(subscripting);
    printf(")");
#undef PRINTMEMBER
}

IteratorUsageCount IteratorUsageCount::s_defaultUsage;

                           // ===========
                           // IterConcept
                           // ===========

/// An enumeration to indicate what type of concept adherence a `TestIterator`
/// type should provide.
struct IterConcept {
enum Enum {
    e_NONE = 0,
    e_INPUT_ITERATOR = 1,
    e_ALL = 2
};
};

const char* toString(IterConcept::Enum iterConcept)
{
    switch (iterConcept) {
    case IterConcept::e_NONE:           return "e_NONE";
    case IterConcept::e_INPUT_ITERATOR: return "e_INPUT_ITERATOR";
    case IterConcept::e_ALL:            return "e_ALL";
    default:                            return "UNKNOWN";
    }
}


                          // ============
                          // TestIterator
                          // ============

/// This value semantic type wraps an iterator of type `t_UNDERLYING` over
/// elements of type `t_VALUE`, and is itself an iterator whose category is
/// `t_CATEGORY`, achieved by having an `iterator_category` member.  Counts
/// are updated on an `IteratorUsageCount` object provided at construction.
/// The `t_CONCEPTLVL` template parameter controls what level of C++20 iterator
/// concepts this type will meet:
/// * `e_NONE`           --- none
/// * `e_INPUT_ITERATOR` --- `bsl::input_iterator` but no more
/// * `e_ALL`            --- fully compliant
/// These variations are accomplished by altering the return type of pre-
/// and post- increment operators, which should not have an impact on the
/// usability of the algorithms we will be testing.  When concepts are not
/// available the `t_CONCEPTLVL` template parameter has no effect.
///
/// The (template parameter) `t_UNDERLYING` must be an iterator with the same
/// or stronger iterator category, and a compatible `value_type`,
/// `difference_type`, and `reference` to those of this type.
template<class             t_CATEGORY,
         class             t_VALUE,
         IterConcept::Enum t_CONCEPTLVL = IterConcept::e_ALL,
         class             t_UNDERLYING = t_VALUE*>
struct TestIterator {
    // PUBLIC TYPES
    typedef t_UNDERLYING  UIter;
    typedef t_VALUE       value_type;
    typedef ptrdiff_t     difference_type;
    typedef value_type*   pointer;
    typedef value_type&   reference;
    typedef t_CATEGORY    iterator_category;

    BSLMF_ASSERT((bsl::is_convertible<
                     typename bsl::iterator_traits<t_UNDERLYING>::value_type,
                     value_type>::value));

    BSLMF_ASSERT((bsl::is_convertible<
                      typename bsl::iterator_traits<t_UNDERLYING>::
                                                             iterator_category,
                      t_CATEGORY>::value));

    BSLMF_ASSERT((bsl::is_convertible<
                      typename bsl::iterator_traits<t_UNDERLYING>::
                                                               difference_type,
                      difference_type>::value));

    BSLMF_ASSERT((bsl::is_convertible<
                      typename bsl::iterator_traits<t_UNDERLYING>::reference,
                      reference>::value));

    // DATA
    IteratorUsageCount* d_usage;       // usage tracking
    UIter               d_underlying;  // underlying iterator

    // CREATORS
    TestIterator()
    : d_usage(&IteratorUsageCount::s_defaultUsage)
    , d_underlying()
    {
        BSLS_ASSERT(d_usage);
        ++(d_usage->d_creations);
    }

    TestIterator(const TestIterator& orig)
    : d_usage(orig.d_usage)
    , d_underlying(orig.d_underlying)
    {
        BSLS_ASSERT(d_usage);
        ++(d_usage->d_copies);
        ++(orig.d_usage->d_copyFroms);
    }

    TestIterator(bslmf::MovableRef<TestIterator> orig)
    : d_usage(bslmf::MovableRefUtil::move(
                  bslmf::MovableRefUtil::access(orig).d_usage))
    , d_underlying(bslmf::MovableRefUtil::move(
                       bslmf::MovableRefUtil::access(orig).d_underlying))
    {
        BSLS_ASSERT(d_usage);
        ++(d_usage->d_moves);
        ++(bslmf::MovableRefUtil::access(orig).d_usage->d_moveFroms);
    }

    explicit TestIterator(const UIter&        underlying,
                          IteratorUsageCount* usage =
                                           &IteratorUsageCount::s_defaultUsage)
    : d_usage(usage)
    , d_underlying(underlying)
    {
        BSLS_ASSERT(d_usage);
        ++(d_usage->d_creations);
    }

    explicit TestIterator(IteratorUsageCount* usage)
    : d_usage(usage)
    , d_underlying()
    {
        BSLS_ASSERT(d_usage);
        ++(d_usage->d_creations);
    }

    // ACCESSORS
    t_VALUE& operator*() const
    {
        ++(d_usage->d_dereferences);
        return *d_underlying;
    }

    t_VALUE& operator[](ptrdiff_t n) const
    {
        ++(d_usage->d_subscripting);
        return d_underlying[n];
    }

    // MANIPULATORS
    TestIterator& operator=(const TestIterator& rhs)
    {
        d_underlying = rhs.d_underlying;
        ++(d_usage->d_copyAssignments);
        ++(rhs.d_usage->d_copyFroms);
        return *this;
    }

    TestIterator& operator=(bslmf::MovableRef<TestIterator> rhs)
    {
        d_underlying = bslmf::MovableRefUtil::move(
            bslmf::MovableRefUtil::access(rhs).d_underlying);
        ++(d_usage->d_moveAssignments);
        ++(bslmf::MovableRefUtil::access(rhs).d_usage->d_moveFroms);
        return *this;
    }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS)
    void operator++() requires (t_CONCEPTLVL == IterConcept::e_NONE)
    {
        ++d_underlying;
        ++(d_usage->d_preIncrement);
    }

    TestIterator& operator++() requires (t_CONCEPTLVL > IterConcept::e_NONE)
    {
        ++d_underlying;
        ++(d_usage->d_preIncrement);
        return *this;
    }

    void operator++(int) requires (t_CONCEPTLVL < IterConcept::e_ALL)
    {
        d_underlying++;
        ++(d_usage->d_postIncrement);
    }

    TestIterator operator++(int) requires (t_CONCEPTLVL == IterConcept::e_ALL)
    {
        TestIterator tmp(*this);
        d_underlying++;
        ++(d_usage->d_postIncrement);
        return tmp;
    }
#else  // BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    TestIterator& operator++()
    {
        ++d_underlying;
        ++(d_usage->d_preIncrement);
        return *this;
    }

    TestIterator operator++(int)
    {
        TestIterator tmp(*this);
        d_underlying++;
        ++(d_usage->d_postIncrement);
        return tmp;
    }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
};

// FREE OPERATORS

template <class t_VALUE, IterConcept::Enum t_CONCEPTLVL, class t_UNDERLYING>
TestIterator<bsl::bidirectional_iterator_tag,
             t_VALUE,
             t_CONCEPTLVL,
             t_UNDERLYING>&
operator--(TestIterator<bsl::bidirectional_iterator_tag,
                        t_VALUE,
                        t_CONCEPTLVL,
                        t_UNDERLYING>&                   lhs)
{
    --lhs.d_underlying;
    ++(lhs.d_usage->d_preDecrement);
    return lhs;
}

template <class t_VALUE, IterConcept::Enum t_CONCEPTLVL, class t_UNDERLYING>
TestIterator<bsl::random_access_iterator_tag,
             t_VALUE,
             t_CONCEPTLVL,
             t_UNDERLYING>&
operator--(TestIterator<bsl::random_access_iterator_tag,
                        t_VALUE,
                        t_CONCEPTLVL,
                        t_UNDERLYING>&                   lhs)
{
    --lhs.d_underlying;
    ++(lhs.d_usage->d_preDecrement);
    return lhs;
}

template <class t_VALUE, IterConcept::Enum t_CONCEPTLVL, class t_UNDERLYING>
TestIterator<bsl::bidirectional_iterator_tag,
             t_VALUE,
             t_CONCEPTLVL,
             t_UNDERLYING>
operator--(TestIterator<bsl::bidirectional_iterator_tag,
                        t_VALUE,
                        t_CONCEPTLVL,
                        t_UNDERLYING>&                   lhs,
           int)
{
    TestIterator<bsl::bidirectional_iterator_tag,
                 t_VALUE,
                 t_CONCEPTLVL,
                 t_UNDERLYING> tmp(lhs);
    lhs.d_underlying--;
    ++(lhs.d_usage->d_postDecrement);
    return tmp;
}

template <class t_VALUE, IterConcept::Enum t_CONCEPTLVL, class t_UNDERLYING>
TestIterator<bsl::random_access_iterator_tag,
             t_VALUE,
             t_CONCEPTLVL,
             t_UNDERLYING>
operator--(TestIterator<bsl::random_access_iterator_tag,
                        t_VALUE,
                        t_CONCEPTLVL,
                        t_UNDERLYING>&                   lhs,
           int)
{
    TestIterator<bsl::random_access_iterator_tag,
                 t_VALUE,
                 t_CONCEPTLVL,
                 t_UNDERLYING> tmp(lhs);
    lhs.d_underlying--;
    ++(lhs.d_usage->d_postDecrement);
    return tmp;
}

template<class             t_CATEGORY,
         class             t_VALUE,
         IterConcept::Enum t_CONCEPTLVL,
         class             t_UNDERLYING>
bool operator==(
    const TestIterator<t_CATEGORY, t_VALUE, t_CONCEPTLVL, t_UNDERLYING>& lhs,
    const TestIterator<t_CATEGORY, t_VALUE, t_CONCEPTLVL, t_UNDERLYING>& rhs)
{
    ++(lhs.d_usage->d_compares);
    ++(rhs.d_usage->d_compares);
    return lhs.d_underlying == rhs.d_underlying;
}

template<class             t_CATEGORY,
         class             t_VALUE,
         IterConcept::Enum t_CONCEPTLVL,
         class             t_UNDERLYING>
bool operator!=(
    const TestIterator<t_CATEGORY, t_VALUE, t_CONCEPTLVL, t_UNDERLYING>& lhs,
    const TestIterator<t_CATEGORY, t_VALUE, t_CONCEPTLVL, t_UNDERLYING>& rhs)
{
    return !(lhs == rhs);
}

template<class             t_VALUE,
         IterConcept::Enum t_CONCEPTLVL,
         class             t_UNDERLYING>
bool operator<=(const TestIterator<bsl::random_access_iterator_tag,
                                   t_VALUE,
                                   t_CONCEPTLVL,
                                   t_UNDERLYING>&                   lhs,
                const TestIterator<bsl::random_access_iterator_tag,
                                   t_VALUE,
                                   t_CONCEPTLVL,
                                   t_UNDERLYING>&                   rhs)
{
    ++(lhs.d_usage->d_compares);
    ++(rhs.d_usage->d_compares);
    return lhs.d_underlying <= rhs.d_underlying;
}

template<class             t_VALUE,
         IterConcept::Enum t_CONCEPTLVL,
         class             t_UNDERLYING>
bool operator>=(const TestIterator<bsl::random_access_iterator_tag,
                                   t_VALUE,
                                   t_CONCEPTLVL,
                                   t_UNDERLYING>&                   lhs,
                const TestIterator<bsl::random_access_iterator_tag,
                                   t_VALUE,
                                   t_CONCEPTLVL,
                                   t_UNDERLYING>&                   rhs)
{
    ++(lhs.d_usage->d_compares);
    ++(rhs.d_usage->d_compares);
    return lhs.d_underlying >= rhs.d_underlying;
}

template<class             t_VALUE,
         IterConcept::Enum t_CONCEPTLVL,
         class             t_UNDERLYING>
bool operator<(const TestIterator<bsl::random_access_iterator_tag,
                                  t_VALUE,
                                  t_CONCEPTLVL,
                                  t_UNDERLYING>&                   lhs,
               const TestIterator<bsl::random_access_iterator_tag,
                                  t_VALUE,
                                  t_CONCEPTLVL,
                                  t_UNDERLYING>&                   rhs)
{
    ++(lhs.d_usage->d_compares);
    ++(rhs.d_usage->d_compares);
    return lhs.d_underlying < rhs.d_underlying;
}

template<class             t_VALUE,
         IterConcept::Enum t_CONCEPTLVL,
         class             t_UNDERLYING>
bool operator>(const TestIterator<bsl::random_access_iterator_tag,
                                  t_VALUE,
                                  t_CONCEPTLVL,
                                  t_UNDERLYING>&                   lhs,
               const TestIterator<bsl::random_access_iterator_tag,
                                  t_VALUE,
                                  t_CONCEPTLVL,
                                  t_UNDERLYING>&                   rhs)
{
    ++(lhs.d_usage->d_compares);
    ++(rhs.d_usage->d_compares);
    return lhs.d_underlying > rhs.d_underlying;
}

template<class             t_VALUE,
         IterConcept::Enum t_CONCEPTLVL,
         class             t_UNDERLYING>
ptrdiff_t operator-(const TestIterator<bsl::random_access_iterator_tag,
                                       t_VALUE,
                                       t_CONCEPTLVL,
                                       t_UNDERLYING>&                   lhs,
                    const TestIterator<bsl::random_access_iterator_tag,
                                       t_VALUE,
                                       t_CONCEPTLVL,
                                       t_UNDERLYING>&                   rhs)
{
    ++(lhs.d_usage->d_difference);
    ++(rhs.d_usage->d_difference);
    return lhs.d_underlying - rhs.d_underlying;
}

template<class             t_VALUE,
         IterConcept::Enum t_CONCEPTLVL,
         class             t_UNDERLYING>
TestIterator<bsl::random_access_iterator_tag,
             t_VALUE,
             t_CONCEPTLVL,
             t_UNDERLYING>&
operator+=(TestIterator<bsl::random_access_iterator_tag,
                        t_VALUE,
                        t_CONCEPTLVL,
                        t_UNDERLYING>&                   lhs,
           ptrdiff_t                                     n)
{
    ++(lhs.d_usage->d_adds);
    lhs.d_underlying += n;
    return lhs;
}

template<class             t_VALUE,
         IterConcept::Enum t_CONCEPTLVL,
         class             t_UNDERLYING>
TestIterator<bsl::random_access_iterator_tag,
             t_VALUE,
             t_CONCEPTLVL,
             t_UNDERLYING>&
operator-=(TestIterator<bsl::random_access_iterator_tag,
                        t_VALUE,
                        t_CONCEPTLVL,
                        t_UNDERLYING>&                   lhs,
           ptrdiff_t                                     n)
{
    ++(lhs.d_usage->d_subtracts);
    lhs.d_underlying -= n;
    return lhs;
}

template<class             t_VALUE,
         IterConcept::Enum t_CONCEPTLVL,
         class             t_UNDERLYING>
TestIterator<bsl::random_access_iterator_tag,
             t_VALUE,
             t_CONCEPTLVL,
             t_UNDERLYING>
operator+(const TestIterator<bsl::random_access_iterator_tag,
                             t_VALUE,
                             t_CONCEPTLVL,
                             t_UNDERLYING>&                   lhs,
          ptrdiff_t                                           n)
{
    ++(lhs.d_usage->d_adds);
    return TestIterator<bsl::random_access_iterator_tag,
                        t_VALUE,
                        t_CONCEPTLVL,
                        t_UNDERLYING>(
        lhs.d_underlying + n,
        lhs.d_usage);
}

template<class             t_VALUE,
         IterConcept::Enum t_CONCEPTLVL,
         class             t_UNDERLYING>
TestIterator<bsl::random_access_iterator_tag,
             t_VALUE,
             t_CONCEPTLVL,
             t_UNDERLYING>
operator+(ptrdiff_t                                           n,
          const TestIterator<bsl::random_access_iterator_tag,
                             t_VALUE,
                             t_CONCEPTLVL,
                             t_UNDERLYING>&                   rhs)
{
    ++(rhs.d_usage->d_adds);
    return TestIterator<bsl::random_access_iterator_tag,
                        t_VALUE,
                        t_CONCEPTLVL,
                        t_UNDERLYING>(
        rhs.d_underlying + n,
        rhs.d_usage);
}

template<class             t_VALUE,
         IterConcept::Enum t_CONCEPTLVL,
         class             t_UNDERLYING>
TestIterator<bsl::random_access_iterator_tag,
             t_VALUE,
             t_CONCEPTLVL,
             t_UNDERLYING>
operator-(const TestIterator<bsl::random_access_iterator_tag,
                             t_VALUE,
                             t_CONCEPTLVL,
                             t_UNDERLYING>&                   lhs,
          ptrdiff_t                                           n)
{
    ++(lhs.d_usage->d_subtracts);
    return TestIterator<bsl::random_access_iterator_tag,
                        t_VALUE,
                        t_CONCEPTLVL,
                        t_UNDERLYING>(
        lhs.d_underlying - n,
        lhs.d_usage);
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)

                     // =================
                     // TestRangeIterator
                     // =================

/// This value semantic type wraps an iterator of type `t_UNDERLYING` over
/// elements of type `t_VALUE`, and is itself an iterator whose category is
/// `t_CATEGORY`, achieved by having an `iterator_concept` member and
/// satisfying the appropriate iterator concept.  Counts are updated on an
/// `IteratorUsageCount` object provided at construction.
///
/// The (template parameter) `t_UNDERLYING` must be an iterator with the same
/// or stronger iterator category, and a compatible `value_type`,
/// `difference_type`, and `reference` to those of this type.
template <class t_CATEGORY,
          class t_VALUE,
          class t_UNDERLYING = t_VALUE*>
struct TestRangeIterator
{
    // PUBLIC TYPES
    typedef t_UNDERLYING UIter;
    typedef t_VALUE      value_type;
    typedef t_CATEGORY   iterator_concept;
    typedef ptrdiff_t    difference_type;
    typedef t_VALUE&     reference;

    BSLMF_ASSERT((bsl::is_convertible<
                     typename bsl::iterator_traits<t_UNDERLYING>::value_type,
                     value_type>::value));

    BSLMF_ASSERT((bsl::is_convertible<
                      typename bsl::iterator_traits<t_UNDERLYING>::
                                                             iterator_category,
                      t_CATEGORY>::value));

    BSLMF_ASSERT((bsl::is_convertible<
                      typename bsl::iterator_traits<t_UNDERLYING>::
                                                               difference_type,
                      difference_type>::value));

    BSLMF_ASSERT((bsl::is_convertible<
                      typename bsl::iterator_traits<t_UNDERLYING>::reference,
                      reference>::value));

    // DATA
    IteratorUsageCount* d_usage;       // usage tracker
    UIter               d_underlying;  // real underlying iterator

    // CREATORS
    TestRangeIterator()
    : d_usage(&IteratorUsageCount::s_defaultUsage)
    , d_underlying()
    {
        BSLS_ASSERT(d_usage);
        ++(d_usage->d_creations);
    }

    TestRangeIterator(const TestRangeIterator& orig)
    : d_usage(orig.d_usage)
    , d_underlying(orig.d_underlying)
    {
        BSLS_ASSERT(d_usage);
        ++(d_usage->d_copies);
        ++(orig.d_usage->d_copyFroms);
    }

    TestRangeIterator(bslmf::MovableRef<TestRangeIterator> orig)
    : d_usage(bslmf::MovableRefUtil::move(
                  bslmf::MovableRefUtil::access(orig).d_usage))
    , d_underlying(bslmf::MovableRefUtil::move(
                       bslmf::MovableRefUtil::access(orig).d_underlying))
    {
        BSLS_ASSERT(d_usage);
        ++(d_usage->d_moves);
        ++(bslmf::MovableRefUtil::access(orig).d_usage->d_moveFroms);
    }

    explicit TestRangeIterator(const UIter&        underlying,
                               IteratorUsageCount* usage =
                                           &IteratorUsageCount::s_defaultUsage)
    : d_usage(usage)
    , d_underlying(underlying)
    {
        BSLS_ASSERT(d_usage);
        ++(d_usage->d_creations);
    }

    explicit TestRangeIterator(IteratorUsageCount* usage)
    : d_usage(usage)
    , d_underlying()
    {
        BSLS_ASSERT(d_usage);
        ++(d_usage->d_creations);
    }

    // ACCESSORS
    t_VALUE& operator*() const
    {
        ++(d_usage->d_dereferences);
        return *d_underlying;
    }

    t_VALUE& operator[](ptrdiff_t n) const
        requires bsl::convertible_to<t_CATEGORY,
                                     bsl::random_access_iterator_tag>
    {
        ++(d_usage->d_subscripting);
        return d_underlying[n];
    }

    bool operator==(const TestRangeIterator& rhs) const
    {
        ++(d_usage->d_compares);
        ++(rhs.d_usage->d_compares);
        return d_underlying == rhs.d_underlying;
    }

    auto operator<=>(const TestRangeIterator& rhs) const
        requires bsl::convertible_to<t_CATEGORY,
                                     bsl::random_access_iterator_tag>
    {
        ++(d_usage->d_compares);
        ++(rhs.d_usage->d_compares);
        return d_underlying <=> rhs.d_underlying;
    }

    TestRangeIterator operator+(ptrdiff_t n) const
        requires bsl::convertible_to<t_CATEGORY,
                                     bsl::random_access_iterator_tag>
    {
        ++(d_usage->d_adds);
        return TestRangeIterator(d_underlying + n, d_usage);
    }

    TestRangeIterator operator-(ptrdiff_t n) const
        requires bsl::convertible_to<t_CATEGORY,
                                     bsl::random_access_iterator_tag>
    {
        ++(d_usage->d_subtracts);
        return TestRangeIterator(d_underlying - n, d_usage);
    }

    // MANIPULATORS
    TestRangeIterator& operator=(const TestRangeIterator& rhs)
    {
        d_underlying = rhs.d_underlying;
        ++(d_usage->d_copyAssignments);
        ++(rhs.d_usage->d_copyFroms);
        return *this;
    }

    TestRangeIterator& operator=(bslmf::MovableRef<TestRangeIterator> rhs)
    {
        d_underlying = bslmf::MovableRefUtil::move(
            bslmf::MovableRefUtil::access(rhs).d_underlying);
        ++(d_usage->d_moveAssignments);
        ++(bslmf::MovableRefUtil::access(rhs).d_usage->d_moveFroms);
        return *this;
    }

    TestRangeIterator& operator++()
    {
        ++d_underlying;
        ++(d_usage->d_preIncrement);
        return *this;
    }

    TestRangeIterator operator++(int)
    {
        TestRangeIterator tmp(*this);
        d_underlying++;
        ++(d_usage->d_postIncrement);
        return tmp;
    }

    TestRangeIterator& operator--()
        requires bsl::convertible_to<t_CATEGORY,
                                     bsl::bidirectional_iterator_tag>
    {
        --d_underlying;
        ++(d_usage->d_preDecrement);
        return *this;
    }

    TestRangeIterator operator--(int)
        requires bsl::convertible_to<t_CATEGORY,
                                     bsl::bidirectional_iterator_tag>
    {
        TestRangeIterator tmp(*this);
        d_underlying--;
        ++(d_usage->d_postDecrement);
        return tmp;
    }

    TestRangeIterator& operator+=(ptrdiff_t n)
        requires bsl::convertible_to<t_CATEGORY,
                                     bsl::random_access_iterator_tag>
    {
        ++(d_usage->d_adds);
        d_underlying += n;
        return *this;
    }

    TestRangeIterator& operator-=(ptrdiff_t n)
        requires bsl::convertible_to<t_CATEGORY,
                                     bsl::random_access_iterator_tag>
    {
        ++(d_usage->d_subtracts);
        d_underlying -= n;
        return *this;
    }
};

// FREE OPERATORS

template<class t_CATEGORY,
         class t_VALUE,
         class t_UNDERLYING>
ptrdiff_t operator-(
    const TestRangeIterator<t_CATEGORY, t_VALUE, t_UNDERLYING>& lhs,
    const TestRangeIterator<t_CATEGORY, t_VALUE, t_UNDERLYING>& rhs)
    requires bsl::convertible_to<t_CATEGORY, bsl::random_access_iterator_tag>
{
    ++(lhs.d_usage->d_difference);
    ++(rhs.d_usage->d_difference);
    return lhs.d_underlying - rhs.d_underlying;
}

template<class t_CATEGORY,
         class t_VALUE,
         class t_UNDERLYING>
TestRangeIterator<t_CATEGORY, t_VALUE, t_UNDERLYING> operator+(
    ptrdiff_t n,
    const TestRangeIterator<t_CATEGORY, t_VALUE, t_UNDERLYING>& rhs)
    requires bsl::convertible_to<t_CATEGORY, bsl::random_access_iterator_tag>
{
    ++(rhs.d_usage->d_adds);
    return TestRangeIterator<t_CATEGORY, t_VALUE, t_UNDERLYING>(
        n + rhs.d_underlying,
        rhs.d_usage);
}

                        // =================
                        // TestRangeSentinel
                        // =================

/// This value semantic type wraps a sentinel object of type `t_UNDERLYING`.
/// Counts are updated on an `IteratorUsageCount` object provided at
/// construction.
template<class t_VALUE,
         class t_UNDERLYING = t_VALUE*>
struct TestRangeSentinel
{
    // PUBLIC TYPES
    typedef t_UNDERLYING USentinel;

    // DATA
    IteratorUsageCount* d_usage;       // usage tracker
    USentinel           d_underlying;  // underlying sentinel

    // CREATORS
    TestRangeSentinel()
    : d_usage(&IteratorUsageCount::s_defaultUsage)
    , d_underlying()
    {
        ++(d_usage->d_creations);
    }

    TestRangeSentinel(const TestRangeSentinel& orig)
    : d_usage(orig.d_usage)
    , d_underlying(orig.d_underlying)
    {
        ++(d_usage->d_copies);
        ++(orig.d_usage->d_copyFroms);
    }

    TestRangeSentinel(bslmf::MovableRef<TestRangeSentinel> orig)
    : d_usage(bslmf::MovableRefUtil::move(
                  bslmf::MovableRefUtil::access(orig).d_usage))
    , d_underlying(bslmf::MovableRefUtil::move(
                       bslmf::MovableRefUtil::access(orig).d_underlying))
    {
        ++(d_usage->d_moves);
        ++(bslmf::MovableRefUtil::access(orig).d_usage->d_moveFroms);
    }

    TestRangeSentinel(const USentinel&    underlying,
                      IteratorUsageCount* usage =
                                           &IteratorUsageCount::s_defaultUsage)
    : d_usage(usage)
    , d_underlying(underlying)
    {
        ++(d_usage->d_creations);
    }

    TestRangeSentinel(IteratorUsageCount* usage)
    : d_usage(usage)
    , d_underlying()
    {
        ++(d_usage->d_creations);
    }

    // MANIPULATORS
    TestRangeSentinel& operator=(const TestRangeSentinel& rhs)
    {
        d_underlying = rhs.d_underlying;
        ++(d_usage->d_copyAssignments);
        ++(rhs.d_usage->d_copyFroms);
        return *this;
    }

    TestRangeSentinel& operator=(bslmf::MovableRef<TestRangeSentinel> rhs)
    {
        d_underlying = bslmf::MovableRefUtil::move(
            bslmf::MovableRefUtil::access(rhs).d_underlying);
        ++(d_usage->d_moveAssignments);
        ++(bslmf::MovableRefUtil::access(rhs).d_usage->d_moveFroms);
        return *this;
    }
};

template<class t_CATEGORY,
         class t_VALUE,
         class t_UNDERLYING,
         class t_UNDERLYINGSENTINEL>
bool operator==(
        const TestRangeIterator<t_CATEGORY, t_VALUE, t_UNDERLYING>& lhs,
        const TestRangeSentinel<t_VALUE, t_UNDERLYINGSENTINEL>& rhs)
{
    ++(lhs.d_usage->d_compares);
    ++(rhs.d_usage->d_compares);
    return lhs.d_underlying == rhs.d_underlying;
}

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES

                         // ========
                         // Poisoner
                         // ========

/// This limited-use proxy type proxies the (template parameter) `t_RESULT` and
/// will fail to compile (by having a mutable const member) if the (template
/// parameter) `t_DOPOISON` is `true`.
template <bool t_DOPOISON, typename t_RESULT>
struct Poisoner
{
    // DATA
    t_RESULT d_result;  // the value we are wrapping
    mutable typename bsl::conditional<t_DOPOISON, const bool, bool>::type
             d_poison;  // dummy value to force compilation failure when
                        // `t_DOPOISON` is `true`.

    // CREATORS
    Poisoner(const t_RESULT& result)
    : d_result(result)
    , d_poison(t_DOPOISON)
    {}

    operator t_RESULT() const
    {
        return d_result;
    }
};

                         // ===========
                         // RefPoisoner
                         // ===========

/// This limited-use proxy type proxies a reference type `t_RESULT` and will
/// fail to compile (by having a mutable const member) if `t_DOPOISON` is
/// `true`.
template <bool t_DOPOISON, typename t_RESULT>
struct RefPoisoner
{
    // DATA
    t_RESULT d_result;  // the value we are wrapping
    mutable typename bsl::conditional<t_DOPOISON, const bool, bool>::type
             d_poison;  // dummy value to force compilation failure when
                        // `t_DOPOISON` is `true`.

    // CREATORS
    RefPoisoner(t_RESULT result)
    : d_result(result)
    , d_poison(t_DOPOISON)
    {}

    operator t_RESULT() const
    {
        return d_result;
    }
};


                    // ====================
                    // TestPoisonedIterator
                    // ====================

/// This value semantic type wraps an iterator of type `t_UNDERLYING` over
/// elements of type `t_VALUE`, and is itself a classic C++ forward iterator
/// with a minimal interface where all functions except basic value-type
/// operations, prefix increment, and dereference will fail to compile if
/// `t_VALUE` is a `const` type.  Operations that are not valid for the
/// specified iterator category will also fail to compile (in a non-SFINAE-able
/// fashion).
///
/// The (template parameter) `t_UNDERLYING` must be an iterator with the same
/// or stronger iterator category, and a compatible `value_type`,
/// `difference_type`, and `reference` to those of this type.
template <class t_CATEGORY,
          class t_VALUE,
          class t_UNDERLYING = t_VALUE*>
struct TestPoisonedIterator
{
    // PUBLIC TYPES
    typedef t_UNDERLYING  UIter;
    typedef t_VALUE       value_type;
    typedef ptrdiff_t     difference_type;
    typedef value_type*   pointer;
    typedef value_type&   reference;
    typedef t_CATEGORY    iterator_category;

    BSLMF_ASSERT((bsl::is_convertible<
                     typename bsl::iterator_traits<t_UNDERLYING>::value_type,
                     value_type>::value));

    BSLMF_ASSERT((bsl::is_convertible<
                      typename bsl::iterator_traits<t_UNDERLYING>::
                                                             iterator_category,
                      t_CATEGORY>::value));

    BSLMF_ASSERT((bsl::is_convertible<
                      typename bsl::iterator_traits<t_UNDERLYING>::
                                                               difference_type,
                      difference_type>::value));

    BSLMF_ASSERT((bsl::is_convertible<
                      typename bsl::iterator_traits<t_UNDERLYING>::reference,
                      reference>::value));

    // DATA
    IteratorUsageCount* d_usage;       // usage tracking
    UIter               d_underlying;  // underlying iterator

    // CREATORS
    TestPoisonedIterator()
    : d_usage(&IteratorUsageCount::s_defaultUsage)
    , d_underlying()
    {
        BSLS_ASSERT(d_usage);
        ++(d_usage->d_creations);
    }

    TestPoisonedIterator(const TestPoisonedIterator& orig)
    : d_usage(orig.d_usage)
    , d_underlying(orig.d_underlying)
    {
        BSLS_ASSERT(d_usage);
        ++(d_usage->d_copies);
        ++(orig.d_usage->d_copyFroms);
    }

    TestPoisonedIterator(bslmf::MovableRef<TestPoisonedIterator> orig)
    : d_usage(bslmf::MovableRefUtil::move(
                  bslmf::MovableRefUtil::access(orig).d_usage))
    , d_underlying(bslmf::MovableRefUtil::move(
                       bslmf::MovableRefUtil::access(orig).d_underlying))
    {
        BSLS_ASSERT(d_usage);
        ++(d_usage->d_moves);
        ++(bslmf::MovableRefUtil::access(orig).d_usage->d_moveFroms);
    }

    explicit TestPoisonedIterator(const UIter&        underlying,
                                  IteratorUsageCount* usage =
                                           &IteratorUsageCount::s_defaultUsage)
    : d_usage(usage)
    , d_underlying(underlying)
    {
        BSLS_ASSERT(d_usage);
        ++(d_usage->d_creations);
    }

    explicit TestPoisonedIterator(IteratorUsageCount* usage)
    : d_usage(usage)
    , d_underlying()
    {
        BSLS_ASSERT(d_usage);
        ++(d_usage->d_creations);
    }

    // ACCESSORS
    t_VALUE& operator*() const
    {
        ++(d_usage->d_dereferences);
        return *d_underlying;
    }

    RefPoisoner<bsl::is_const<t_VALUE>::value, t_VALUE&>
    operator[](ptrdiff_t n) const
    {
        ++(d_usage->d_subscripting);
        return d_underlying[n];
    }

    bool operator==(const TestPoisonedIterator& rhs) const
    {
        ++(d_usage->d_compares);
        ++(rhs.d_usage->d_compares);
        return d_underlying == rhs.d_underlying;
    }

    bool operator!=(const TestPoisonedIterator& rhs) const
    {
        ++(d_usage->d_compares);
        ++(rhs.d_usage->d_compares);
        return d_underlying != rhs.d_underlying;
    }

    Poisoner<!bsl::is_convertible<t_CATEGORY,
                                  bsl::random_access_iterator_tag>::value,
             TestPoisonedIterator> operator+(ptrdiff_t n) const
    {
        ++(d_usage->d_adds);
        return TestPoisonedIterator(d_underlying + n, d_usage);
    }

    Poisoner<!bsl::is_convertible<t_CATEGORY,
                                  bsl::random_access_iterator_tag>::value,
        TestPoisonedIterator> operator-(ptrdiff_t n) const
    {
        ++(d_usage->d_subtracts);
        return TestPoisonedIterator(d_underlying - n, d_usage);
    }

    Poisoner<!bsl::is_convertible<t_CATEGORY,
                                  bsl::random_access_iterator_tag>::value,
        bool> operator<(const TestPoisonedIterator& rhs) const
    {
        ++(d_usage->d_compares);
        ++(rhs.d_usage->d_compares);
        return d_underlying < rhs.d_underlying;
    }

    Poisoner<bsl::is_const<t_VALUE>::value
         || !bsl::is_convertible<t_CATEGORY,
                                 bsl::random_access_iterator_tag>::value,
             bool> operator>(const TestPoisonedIterator& rhs) const
    {
        ++(d_usage->d_compares);
        ++(rhs.d_usage->d_compares);
        return d_underlying > rhs.d_underlying;
    }

    Poisoner<bsl::is_const<t_VALUE>::value
         || !bsl::is_convertible<t_CATEGORY,
                                 bsl::random_access_iterator_tag>::value,
        bool> operator<=(const TestPoisonedIterator& rhs) const
    {
        ++(d_usage->d_compares);
        ++(rhs.d_usage->d_compares);
        return d_underlying <= rhs.d_underlying;
    }

    Poisoner<bsl::is_const<t_VALUE>::value
         || !bsl::is_convertible<t_CATEGORY,
                                 bsl::random_access_iterator_tag>::value,
        bool> operator>=(const TestPoisonedIterator& rhs) const
    {
        ++(d_usage->d_compares);
        ++(rhs.d_usage->d_compares);
        return d_underlying >= rhs.d_underlying;
    }

    // MANIPULATORS
    TestPoisonedIterator& operator=(const TestPoisonedIterator& rhs)
    {
        d_underlying = rhs.d_underlying;
        ++(d_usage->d_copyAssignments);
        ++(rhs.d_usage->d_copyFroms);
        return *this;
    }

    TestPoisonedIterator& operator=(
        bslmf::MovableRef<TestPoisonedIterator> rhs)
    {
        d_underlying = bslmf::MovableRefUtil::move(
            bslmf::MovableRefUtil::access(rhs).d_underlying);
        ++(d_usage->d_moveAssignments);
        ++(bslmf::MovableRefUtil::access(rhs).d_usage->d_moveFroms);
        return *this;
    }

    TestPoisonedIterator& operator++()
    {
        ++d_underlying;
        ++(d_usage->d_preIncrement);
        return *this;
    }

    Poisoner<bsl::is_const<t_VALUE>::value, TestPoisonedIterator>
    operator++(int)
    {
        TestPoisonedIterator tmp(*this);
        d_underlying++;
        ++(d_usage->d_postIncrement);
        return tmp;
    }

    RefPoisoner<!bsl::is_convertible<iterator_category,
                                     bsl::bidirectional_iterator_tag>::value,
             TestPoisonedIterator&> operator--()
    {
        --d_underlying;
        ++(d_usage->d_preDecrement);
        return *this;
    }

    Poisoner<!bsl::is_convertible<iterator_category,
                                  bsl::bidirectional_iterator_tag>::value,
                     TestPoisonedIterator> operator--(int)
    {
        TestPoisonedIterator tmp(*this);
        d_underlying--;
        ++(d_usage->d_postDecrement);
        return tmp;
    }

    ptrdiff_t operator-(const TestPoisonedIterator& rhs) const
    {
        ++(d_usage->d_difference);
        ++(rhs.d_usage->d_difference);
        return d_underlying - rhs.d_underlying;
    }

    RefPoisoner<!bsl::is_convertible<t_CATEGORY,
                                     bsl::random_access_iterator_tag>::value,
        TestPoisonedIterator&> operator+=(ptrdiff_t n)
    {
        ++(d_usage->d_adds);
        d_underlying += n;
        return *this;
    }

    RefPoisoner<!bsl::is_convertible<t_CATEGORY,
                                     bsl::random_access_iterator_tag>::value,
        TestPoisonedIterator&> operator-=(ptrdiff_t n)
    {
        ++(d_usage->d_subtracts);
        d_underlying -= n;
        return *this;
    }
};

// FREE OPERATORS

template<class t_CATEGORY,
         class t_VALUE,
         class t_UNDERLYING>
Poisoner<!bsl::is_convertible<t_CATEGORY,
                              bsl::random_access_iterator_tag>::value,
    ptrdiff_t> operator-(
    const TestPoisonedIterator<t_CATEGORY, t_VALUE, t_UNDERLYING>& lhs,
    const TestPoisonedIterator<t_CATEGORY, t_VALUE, t_UNDERLYING>& rhs)
{
    ++(lhs.d_usage->d_difference);
    ++(rhs.d_usage->d_difference);
    return lhs.d_underlying - rhs.d_underlying;
}

}  // close unnamed namespace

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
/// This struct provides a namespace for functions testing alias template.
/// The tests are compile-time only; it is not necessary that these routines
/// be called at run-time.
struct TestAliasTemplates {

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

    /// Test the template aliases in this component "return" the correct
    /// type when instantiated with both iterators and pointers.
    static void AliasTemplates ()
    {
        using TAG = bsl::input_iterator_tag;

        using T1    = int;
        using PTR1  = T1*;
        using ITER1 = TestIterator<TAG, T1>;

        ASSERT_SAME_TYPE(IteratorUtil::IterVal_t<PTR1>,  T1);
        ASSERT_SAME_TYPE(IteratorUtil::IterVal_t<ITER1>, T1);

        using T2     = long;
        using PTR2A  =                   bsl::pair<T2, char>*;
        using ITER2A = TestIterator<TAG, bsl::pair<T2, char>>;
        using PTR2B  =                   bsl::pair<const T2, char>*;
        using ITER2B = TestIterator<TAG, bsl::pair<const T2, char>>;

        ASSERT_SAME_TYPE(IteratorUtil::IterKey_t<PTR2A>,  T2);
        ASSERT_SAME_TYPE(IteratorUtil::IterKey_t<ITER2A>, T2);
        ASSERT_SAME_TYPE(IteratorUtil::IterKey_t<PTR2B>,  T2);
        ASSERT_SAME_TYPE(IteratorUtil::IterKey_t<ITER2B>, T2);

        using T3     = double;
        using PTR3A  =                   bsl::pair<char, T3>*;
        using ITER3A = TestIterator<TAG, bsl::pair<char, T3>>;
        using PTR3B  =                   bsl::pair<const char, T3>*;
        using ITER3B = TestIterator<TAG, bsl::pair<const char, T3>>;

        ASSERT_SAME_TYPE(IteratorUtil::IterMapped_t<PTR3A>,  T3);
        ASSERT_SAME_TYPE(IteratorUtil::IterMapped_t<ITER3A>, T3);
        ASSERT_SAME_TYPE(IteratorUtil::IterMapped_t<PTR3B>,  T3);
        ASSERT_SAME_TYPE(IteratorUtil::IterMapped_t<ITER3B>, T3);

        using T4KEY     = unsigned;
        using T4MAPPED  = float;
        using PTR4A  =                   bsl::pair<T4KEY, T4MAPPED>*;
        using ITER4A = TestIterator<TAG, bsl::pair<T4KEY, T4MAPPED>>;
        using PTR4B  =                   bsl::pair<const T4KEY, T4MAPPED>*;
        using ITER4B = TestIterator<TAG, bsl::pair<const T4KEY, T4MAPPED>>;

        using EXPECTEDT4 = bsl::pair<const T4KEY, T4MAPPED>;
        ASSERT_SAME_TYPE(IteratorUtil::IterToAlloc_t<PTR4A>,  EXPECTEDT4);
        ASSERT_SAME_TYPE(IteratorUtil::IterToAlloc_t<ITER4A>, EXPECTEDT4);
        ASSERT_SAME_TYPE(IteratorUtil::IterToAlloc_t<PTR4B>,  EXPECTEDT4);
        ASSERT_SAME_TYPE(IteratorUtil::IterToAlloc_t<ITER4B>, EXPECTEDT4);

    }

#undef ASSERT_SAME_TYPE
};
#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

template <class t_INPUT_ITERATOR,
          class t_SENTINEL,
          bool t_MATCHING_SENTINEL =
              bsl::is_convertible<t_SENTINEL, t_INPUT_ITERATOR>::value>
struct TestTestIterators_LegacyClassifier
{
    BSLS_KEYWORD_CONSTEXPR static int classify()
    {
        typedef
            typename bsl::iterator_traits<t_INPUT_ITERATOR>::iterator_category
                                                                  IterCategory;
        if BSLS_KEYWORD_CONSTEXPR_CPP17 (
            bsl::is_convertible<IterCategory,
                                bsl::forward_iterator_tag>::value) {
            return 5;                                                 // RETURN
        }
        else {
            return 6;                                                 // RETURN
        }
    }
};

template <class t_INPUT_ITERATOR,
          class t_SENTINEL>
struct TestTestIterators_LegacyClassifier<t_INPUT_ITERATOR, t_SENTINEL, false>
{
    BSLS_KEYWORD_CONSTEXPR static int classify()
    {
        return 7;
    }
};

struct TestTestIterators {

    template <class t_INPUT_ITERATOR, class t_SENTINEL>
    BSLS_KEYWORD_CONSTEXPR static int insertDistanceClassification()
    {
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)
    typedef typename bsl::iterator_traits<t_INPUT_ITERATOR>::iterator_category
                                                                  IterCategory;

    if constexpr (IteratorUtil_ModernIterator<t_INPUT_ITERATOR>
               || !bsl::is_convertible_v<t_SENTINEL, t_INPUT_ITERATOR>)
    {
        if constexpr (bsl::input_iterator<t_INPUT_ITERATOR>
                   && bsl::sentinel_for<t_SENTINEL, t_INPUT_ITERATOR>)
        {
            if constexpr (bsl::forward_iterator<t_INPUT_ITERATOR>
                       && bsl::is_convertible_v<IterCategory,
                                                bsl::forward_iterator_tag>) {
                return 1;                                             // RETURN
            }
            else {
                return 3;                                             // RETURN
            }
        }
        else {
            return 4;                                                 // RETURN
        }
    }
    else {
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES

        return
            TestTestIterators_LegacyClassifier<t_INPUT_ITERATOR, t_SENTINEL>
                ::classify();                                         // RETURN

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)
    }  // close of `if constexpr`
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES

    }


    template <IterConcept::Enum t_CONCEPTLVL>
    static void testCountsLegacy()
    {
        if (verbose) printf("\n\tTesting `TestIterator` (concept level:"
                            "%s).\n", toString(t_CONCEPTLVL));
        {
            typedef TestIterator<bsl::random_access_iterator_tag,
                                 int,
                                 t_CONCEPTLVL>                 IntTestIterator;


            int testData[] = { 1, 17, 289 };

            IteratorUsageCount counts1;
            IntTestIterator    it1(&testData[0], &counts1);
            ASSERT(1 == counts1.d_creations);

            IteratorUsageCount counts2;
            IntTestIterator    it2(&testData[0], &counts2);
            ASSERT(1 == counts2.d_creations);

            IntTestIterator tmp1(it1);
            ASSERT(1 == counts1.d_copies);
            ASSERT(1 == counts1.d_copyFroms);

            IntTestIterator tmp2(bslmf::MovableRefUtil::move(tmp1));
            ASSERT(1 == counts1.d_moves);
            ASSERT(1 == counts1.d_moveFroms);

            tmp2 = tmp1;
            ASSERT(1 == counts1.d_copyAssignments);
            ASSERT(2 == counts1.d_copyFroms);

            tmp2 = bslmf::MovableRefUtil::move(tmp1);
            ASSERT(1 == counts1.d_moveAssignments);
            ASSERT(2 == counts1.d_moveFroms);

            static_cast<void>(++it1);
            ASSERT(1 == counts1.d_preIncrement);

            static_cast<void>(--it1);
            ASSERT(1 == counts1.d_preDecrement);

            static_cast<void>(it1++);
            ASSERT(1 == counts1.d_postIncrement);

            static_cast<void>(it1--);
            ASSERT(1 == counts1.d_postDecrement);

            static_cast<void>(it1 + 1);
            ASSERT(1 == counts1.d_adds);

            static_cast<void>(it1 += 1);
            ASSERT(2 == counts1.d_adds);

            static_cast<void>(it1 - 1);
            ASSERT(1 == counts1.d_subtracts);

            static_cast<void>(it1 -= 1);
            ASSERT(2 == counts1.d_subtracts);

            static_cast<void>(it2 - it1);
            ASSERT(1 == counts1.d_difference);
            ASSERT(1 == counts2.d_difference);

            static_cast<void>(static_cast<bool>(it1 == it2));
            ASSERT(1 == counts1.d_compares);
            ASSERT(1 == counts2.d_compares);

            static_cast<void>(static_cast<bool>(it1 != it2));
            ASSERT(2 == counts1.d_compares);
            ASSERT(2 == counts2.d_compares);

            static_cast<void>(static_cast<bool>(it1 < it2));
            ASSERT(3 == counts1.d_compares);
            ASSERT(3 == counts2.d_compares);

            static_cast<void>(static_cast<bool>(it1 > it2));
            ASSERT(4 == counts1.d_compares);
            ASSERT(4 == counts2.d_compares);

            static_cast<void>(static_cast<bool>(it1 <= it2));
            ASSERT(5 == counts1.d_compares);
            ASSERT(5 == counts2.d_compares);

            static_cast<void>(static_cast<bool>(it1 >= it2));
            ASSERT(6 == counts1.d_compares);
            ASSERT(6 == counts2.d_compares);
        }
    }

    static void testCountsRanges()
    {
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)
        if (verbose) printf("\n\tTesting `TestRangeIterator` and "
                            "`TestRangeSentinel`.\n");
        {
            typedef TestRangeIterator<bsl::random_access_iterator_tag, int>
                IntTestIterator;
            typedef TestRangeSentinel<int>
                IntTestSentinel;

            int testData[] = { 1, 17, 289 };

            IteratorUsageCount counts1;
            IntTestIterator    it1(&testData[0], &counts1);
            ASSERT(1 == counts1.d_creations);

            IteratorUsageCount counts2;
            IntTestIterator    it2(&testData[0], &counts2);
            ASSERT(1 == counts2.d_creations);

            IteratorUsageCount counts3;
            IntTestSentinel    sen3(&testData[0], &counts3);
            ASSERT(1 == counts3.d_creations);

            IntTestIterator tmp1(it1);
            ASSERT(1 == counts1.d_copies);
            ASSERT(1 == counts1.d_copyFroms);

            IntTestIterator tmp2(bslmf::MovableRefUtil::move(tmp1));
            ASSERT(1 == counts1.d_moves);
            ASSERT(1 == counts1.d_moveFroms);

            tmp2 = tmp1;
            ASSERT(1 == counts1.d_copyAssignments);
            ASSERT(2 == counts1.d_copyFroms);

            tmp2 = bslmf::MovableRefUtil::move(tmp1);
            ASSERT(1 == counts1.d_moveAssignments);
            ASSERT(2 == counts1.d_moveFroms);

            IntTestSentinel tmp3(sen3);
            ASSERT(1 == counts3.d_copies);
            ASSERT(1 == counts3.d_copyFroms);

            IntTestSentinel tmp4(bslmf::MovableRefUtil::move(tmp3));
            ASSERT(1 == counts3.d_moves);
            ASSERT(1 == counts3.d_moveFroms);

            tmp4 = tmp3;
            ASSERT(1 == counts3.d_copyAssignments);
            ASSERT(2 == counts3.d_copyFroms);

            tmp4 = bslmf::MovableRefUtil::move(tmp3);
            ASSERT(1 == counts3.d_moveAssignments);
            ASSERT(2 == counts3.d_moveFroms);

            static_cast<void>(++it1);
            ASSERT(1 == counts1.d_preIncrement);

            static_cast<void>(--it1);
            ASSERT(1 == counts1.d_preDecrement);

            static_cast<void>(it1++);
            ASSERT(1 == counts1.d_postIncrement);

            static_cast<void>(it1--);
            ASSERT(1 == counts1.d_postDecrement);

            static_cast<void>(it1 + 1);
            ASSERT(1 == counts1.d_adds);

            static_cast<void>(it1 += 1);
            ASSERT(2 == counts1.d_adds);

            static_cast<void>(it1 - 1);
            ASSERT(1 == counts1.d_subtracts);

            static_cast<void>(it1 -= 1);
            ASSERT(2 == counts1.d_subtracts);

            static_cast<void>(it2 - it1);
            ASSERT(1 == counts1.d_difference);
            ASSERT(1 == counts2.d_difference);

            static_cast<void>(static_cast<bool>(it1 == it2));
            ASSERT(1 == counts1.d_compares);
            ASSERT(1 == counts2.d_compares);

            static_cast<void>(static_cast<bool>(it1 != it2));
            ASSERT(2 == counts1.d_compares);
            ASSERT(2 == counts2.d_compares);

            static_cast<void>(static_cast<bool>(it1 == sen3));
            ASSERT(3 == counts1.d_compares);
            ASSERT(1 == counts3.d_compares);

            static_cast<void>(static_cast<bool>(it1 != sen3));
            ASSERT(4 == counts1.d_compares);
            ASSERT(2 == counts3.d_compares);

            static_cast<void>(static_cast<bool>(it1 < it2));
            ASSERT(5 == counts1.d_compares);
            ASSERT(3 == counts2.d_compares);

            static_cast<void>(static_cast<bool>(it1 > it2));
            ASSERT(6 == counts1.d_compares);
            ASSERT(4 == counts2.d_compares);

            static_cast<void>(static_cast<bool>(it1 <= it2));
            ASSERT(7 == counts1.d_compares);
            ASSERT(5 == counts2.d_compares);

            static_cast<void>(static_cast<bool>(it1 >= it2));
            ASSERT(8 == counts1.d_compares);
            ASSERT(6 == counts2.d_compares);
        }
#else  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
        if (verbose) printf("\n\tNo concepts or ranges, not testing C++20 "
                            "range iterators.\n");
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
    }

    static void testCountsPoisoned()
    {
        if (verbose) printf("\n\tTesting `TestPoisonedIterator`.\n");
        {
            // a random-access poisoned iterator with a non-const value
            // type should support everything and update all counts correctly.
            typedef TestPoisonedIterator<bsl::random_access_iterator_tag,
                                         int>                  IntTestIterator;


            int testData[] = { 1, 17, 289 };

            IteratorUsageCount counts1;
            IntTestIterator    it1(&testData[0], &counts1);
            ASSERT(1 == counts1.d_creations);

            IteratorUsageCount counts2;
            IntTestIterator    it2(&testData[0], &counts2);
            ASSERT(1 == counts2.d_creations);

            IntTestIterator tmp1(it1);
            ASSERT(1 == counts1.d_copies);
            ASSERT(1 == counts1.d_copyFroms);

            IntTestIterator tmp2(bslmf::MovableRefUtil::move(tmp1));
            ASSERT(1 == counts1.d_moves);
            ASSERT(1 == counts1.d_moveFroms);

            tmp2 = tmp1;
            ASSERT(1 == counts1.d_copyAssignments);
            ASSERT(2 == counts1.d_copyFroms);

            tmp2 = bslmf::MovableRefUtil::move(tmp1);
            ASSERT(1 == counts1.d_moveAssignments);
            ASSERT(2 == counts1.d_moveFroms);

            static_cast<void>(++it1);
            ASSERT(1 == counts1.d_preIncrement);

            static_cast<void>(--it1);
            ASSERT(1 == counts1.d_preDecrement);

            static_cast<void>(it1++);
            ASSERT(1 == counts1.d_postIncrement);

            static_cast<void>(it1--);
            ASSERT(1 == counts1.d_postDecrement);

            static_cast<void>(it1 + 1);
            ASSERT(1 == counts1.d_adds);

            static_cast<void>(it1 += 1);
            ASSERT(2 == counts1.d_adds);

            static_cast<void>(it1 - 1);
            ASSERT(1 == counts1.d_subtracts);

            static_cast<void>(it1 -= 1);
            ASSERT(2 == counts1.d_subtracts);

            static_cast<void>(it2 - it1);
            ASSERT(1 == counts1.d_difference);
            ASSERT(1 == counts2.d_difference);

            static_cast<void>(static_cast<bool>(it1 == it2));
            ASSERT(1 == counts1.d_compares);
            ASSERT(1 == counts2.d_compares);

            static_cast<void>(static_cast<bool>(it1 != it2));
            ASSERT(2 == counts1.d_compares);
            ASSERT(2 == counts2.d_compares);

            static_cast<void>(static_cast<bool>(it1 < it2));
            ASSERT(3 == counts1.d_compares);
            ASSERT(3 == counts2.d_compares);

            static_cast<void>(static_cast<bool>(it1 > it2));
            ASSERT(4 == counts1.d_compares);
            ASSERT(4 == counts2.d_compares);

            static_cast<void>(static_cast<bool>(it1 <= it2));
            ASSERT(5 == counts1.d_compares);
            ASSERT(5 == counts2.d_compares);

            static_cast<void>(static_cast<bool>(it1 >= it2));
            ASSERT(6 == counts1.d_compares);
            ASSERT(6 == counts2.d_compares);
        }
    }

    template <IterConcept::Enum t_CONCEPTLVL>
    static void testCategories()
    {
        ASSERT((bsl::is_same<
                     bsl::input_iterator_tag,
                     typename bsl::iterator_traits<
                        TestIterator<bsl::input_iterator_tag,
                                     int,
                                     t_CONCEPTLVL>
                     >::iterator_category
                 >::value ));

        ASSERT((bsl::is_same<
                    bsl::forward_iterator_tag,
                    typename bsl::iterator_traits<
                        TestIterator<bsl::forward_iterator_tag,
                                     int,
                                     t_CONCEPTLVL>
                    >::iterator_category
                >::value ));

        ASSERT((bsl::is_same<
                    bsl::bidirectional_iterator_tag,
                    typename bsl::iterator_traits<
                        TestIterator<bsl::bidirectional_iterator_tag,
                                     int,
                                     t_CONCEPTLVL>
                    >::iterator_category
                >::value ));

        ASSERT((bsl::is_same<
                    bsl::random_access_iterator_tag,
                    typename bsl::iterator_traits<
                        TestIterator<bsl::random_access_iterator_tag,
                                     int,
                                     t_CONCEPTLVL>
                    >::iterator_category
                >::value ));
    }

    static void testRangeCategories()
    {
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)
        // because a range iterator with an `input_iterator_tag` is copyable,
        // it will look like a forward iterator to `iterator_traits` based
        // logic.
        ASSERT((bsl::is_same<
                     bsl::forward_iterator_tag,
                     typename bsl::iterator_traits<
                        TestRangeIterator<bsl::input_iterator_tag,
                                          int>
                     >::iterator_category
                 >::value ));

        ASSERT((bsl::is_same<
                    bsl::forward_iterator_tag,
                    typename bsl::iterator_traits<
                        TestRangeIterator<bsl::forward_iterator_tag,
                                          int>
                    >::iterator_category
                >::value ));

        ASSERT((bsl::is_same<
                    bsl::bidirectional_iterator_tag,
                    typename bsl::iterator_traits<
                        TestRangeIterator<bsl::bidirectional_iterator_tag,
                                          int>
                    >::iterator_category
                >::value ));

        ASSERT((bsl::is_same<
                    bsl::random_access_iterator_tag,
                    typename bsl::iterator_traits<
                        TestRangeIterator<bsl::random_access_iterator_tag,
                                          int>
                    >::iterator_category
                >::value ));
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
    }

    static void testPoisonedCategories()
    {
        ASSERT((bsl::is_same<
                     bsl::input_iterator_tag,
                     typename bsl::iterator_traits<
                        TestPoisonedIterator<bsl::input_iterator_tag,int>
                     >::iterator_category
                 >::value ));

        ASSERT((bsl::is_same<
                    bsl::forward_iterator_tag,
                    typename bsl::iterator_traits<
                        TestPoisonedIterator<bsl::forward_iterator_tag,int>
                    >::iterator_category
                >::value ));

        ASSERT((bsl::is_same<
                    bsl::bidirectional_iterator_tag,
                    typename bsl::iterator_traits<
                        TestPoisonedIterator<bsl::bidirectional_iterator_tag,
                                             int>
                    >::iterator_category
                >::value ));

        ASSERT((bsl::is_same<
                    bsl::random_access_iterator_tag,
                    typename bsl::iterator_traits<
                        TestPoisonedIterator<bsl::random_access_iterator_tag,
                                             int>
                    >::iterator_category
                >::value ));
    }

    static void testConcepts()
    {
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)

        // Input Iterators

        {
            using Iter = TestIterator<bsl::input_iterator_tag,
                                      int,
                                      IterConcept::e_NONE>;
            static_assert((!IteratorUtil_ModernIterator<Iter>));
            static_assert((bsl::same_as<
                               bsl::input_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
            static_assert((!bsl::input_or_output_iterator<Iter>));
            static_assert((!bsl::input_iterator<Iter>));
            static_assert((!bsl::forward_iterator<Iter>));
            static_assert((!bsl::bidirectional_iterator<Iter>));
            static_assert((!bsl::random_access_iterator<Iter>));
            static_assert((!bsl::sentinel_for<Iter,Iter>));
        }

        {
            using Iter = TestIterator<bsl::input_iterator_tag,
                                      int,
                                      IterConcept::e_INPUT_ITERATOR>;
            static_assert((!IteratorUtil_ModernIterator<Iter>));
            static_assert((bsl::same_as<
                               bsl::input_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
            static_assert(( bsl::input_or_output_iterator<Iter>));
            static_assert(( bsl::input_iterator<Iter>));
            static_assert((!bsl::forward_iterator<Iter>));
            static_assert((!bsl::bidirectional_iterator<Iter>));
            static_assert((!bsl::random_access_iterator<Iter>));
            static_assert(( bsl::sentinel_for<Iter,Iter>));
        }

        {
            using Iter = TestPoisonedIterator<bsl::input_iterator_tag, int>;

            static_assert((!IteratorUtil_ModernIterator<Iter>));
            static_assert((bsl::same_as<
                               bsl::input_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
            static_assert(( bsl::input_or_output_iterator<Iter>));
            static_assert(( bsl::input_iterator<Iter>));
            static_assert((!bsl::forward_iterator<Iter>));
            static_assert((!bsl::bidirectional_iterator<Iter>));
            static_assert((!bsl::random_access_iterator<Iter>));
            static_assert(( bsl::sentinel_for<Iter,Iter>));
        }

        {
            // with a const parameter member most concepts will fail
            using Iter = TestPoisonedIterator<bsl::input_iterator_tag,
                                              const int>;

            static_assert((!IteratorUtil_ModernIterator<Iter>));
            static_assert((bsl::same_as<
                               bsl::input_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
        }

        {
            using Iter = TestIterator<bsl::input_iterator_tag,
                                      int,
                                      IterConcept::e_ALL>;
            static_assert((!IteratorUtil_ModernIterator<Iter>));
            static_assert((bsl::same_as<
                               bsl::input_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
            static_assert(( bsl::input_or_output_iterator<Iter>));
            static_assert(( bsl::input_iterator<Iter>));
            static_assert((!bsl::forward_iterator<Iter>));
            static_assert((!bsl::bidirectional_iterator<Iter>));
            static_assert((!bsl::random_access_iterator<Iter>));
            static_assert(( bsl::sentinel_for<Iter,Iter>));
        }

        {
            using Iter = TestRangeIterator<bsl::input_iterator_tag, int>;
            using Sentinel = TestRangeSentinel<int>;

            static_assert(( IteratorUtil_ModernIterator<Iter>));
            // The range iterator with an `input_iterator_tag` will look like a
            // forward iterator to `iterator_traits` based logic because it is
            // copyable.
            static_assert((bsl::same_as<
                               bsl::forward_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
            static_assert(( bsl::input_or_output_iterator<Iter>));
            static_assert(( bsl::input_iterator<Iter>));
            static_assert((!bsl::forward_iterator<Iter>));
            static_assert((!bsl::bidirectional_iterator<Iter>));
            static_assert((!bsl::random_access_iterator<Iter>));

            static_assert(( bsl::sentinel_for<Iter,Iter>));
            static_assert(( bsl::sentinel_for<Sentinel, Iter> ));
        }

        // Forward Iterators

        {
            using Iter = TestIterator<bsl::forward_iterator_tag,
                                      int,
                                      IterConcept::e_NONE>;

            static_assert((!IteratorUtil_ModernIterator<Iter>));
            static_assert((bsl::same_as<
                               bsl::forward_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
            static_assert((!bsl::input_or_output_iterator<Iter>));
            static_assert((!bsl::input_iterator<Iter>));
            static_assert((!bsl::forward_iterator<Iter>));
            static_assert((!bsl::bidirectional_iterator<Iter>));
            static_assert((!bsl::random_access_iterator<Iter>));

            static_assert((!bsl::sentinel_for<Iter,Iter>));
        }

        {
            using Iter = TestIterator<bsl::forward_iterator_tag,
                                      int,
                                      IterConcept::e_INPUT_ITERATOR>;

            static_assert((!IteratorUtil_ModernIterator<Iter>));
            static_assert((bsl::same_as<
                               bsl::forward_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
            static_assert(( bsl::input_or_output_iterator<Iter>));
            static_assert(( bsl::input_iterator<Iter>));
            static_assert((!bsl::forward_iterator<Iter>));
            static_assert((!bsl::bidirectional_iterator<Iter>));
            static_assert((!bsl::random_access_iterator<Iter>));

            static_assert(( bsl::sentinel_for<Iter,Iter>));
        }

        {
            using Iter = TestIterator<bsl::forward_iterator_tag,
                                      int,
                                      IterConcept::e_ALL>;

            static_assert((!IteratorUtil_ModernIterator<Iter>));
            static_assert((bsl::same_as<
                               bsl::forward_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
            static_assert(( bsl::input_or_output_iterator<Iter>));
            static_assert(( bsl::input_iterator<Iter>));
            static_assert(( bsl::forward_iterator<Iter>));
            static_assert((!bsl::bidirectional_iterator<Iter>));
            static_assert((!bsl::random_access_iterator<Iter>));

            static_assert(( bsl::sentinel_for<Iter,Iter>));
        }

        {
            using Iter = TestRangeIterator<bsl::forward_iterator_tag, int>;
            using Sentinel = TestRangeSentinel<int>;

            static_assert(( IteratorUtil_ModernIterator<Iter>));

            static_assert((bsl::same_as<
                               bsl::forward_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
            static_assert(( bsl::input_or_output_iterator<Iter>));
            static_assert(( bsl::input_iterator<Iter>));
            static_assert(( bsl::forward_iterator<Iter>));
            static_assert((!bsl::bidirectional_iterator<Iter>));
            static_assert((!bsl::random_access_iterator<Iter>));

            static_assert(( bsl::sentinel_for<Iter,Iter>));
            static_assert(( bsl::sentinel_for<Sentinel, Iter> ));
        }

        {
            using Iter = TestPoisonedIterator<bsl::forward_iterator_tag, int>;

            static_assert((!IteratorUtil_ModernIterator<Iter>));
            static_assert((bsl::same_as<
                               bsl::forward_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
            static_assert(( bsl::input_or_output_iterator<Iter>));
            static_assert(( bsl::input_iterator<Iter>));
        }

        {
            using Iter = TestPoisonedIterator<bsl::forward_iterator_tag,
                                              const int>;

            static_assert((!IteratorUtil_ModernIterator<Iter>));
            static_assert((bsl::same_as<
                               bsl::forward_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
        }


        // Bidirectional Iterators

        {
            using Iter = TestIterator<bsl::bidirectional_iterator_tag,
                                      int,
                                      IterConcept::e_NONE>;

            static_assert((!IteratorUtil_ModernIterator<Iter>));
            static_assert((bsl::same_as<
                               bsl::bidirectional_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
            static_assert((!bsl::input_or_output_iterator<Iter>));
            static_assert((!bsl::input_iterator<Iter>));
            static_assert((!bsl::forward_iterator<Iter>));
            static_assert((!bsl::bidirectional_iterator<Iter>));
            static_assert((!bsl::random_access_iterator<Iter>));

            static_assert((!bsl::sentinel_for<Iter,Iter>));
        }

        {
            using Iter = TestIterator<bsl::bidirectional_iterator_tag,
                                      int,
                                      IterConcept::e_INPUT_ITERATOR>;

            static_assert((!IteratorUtil_ModernIterator<Iter>));
            static_assert((bsl::same_as<
                               bsl::bidirectional_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
            static_assert(( bsl::input_or_output_iterator<Iter>));
            static_assert(( bsl::input_iterator<Iter>));
            static_assert((!bsl::forward_iterator<Iter>));
            static_assert((!bsl::bidirectional_iterator<Iter>));
            static_assert((!bsl::random_access_iterator<Iter>));

            static_assert(( bsl::sentinel_for<Iter,Iter>));
        }

        {
            using Iter = TestIterator<bsl::bidirectional_iterator_tag,
                                      int,
                                      IterConcept::e_ALL>;

            static_assert((!IteratorUtil_ModernIterator<Iter>));
            static_assert((bsl::same_as<
                               bsl::bidirectional_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
            static_assert(( bsl::input_or_output_iterator<Iter>));
            static_assert(( bsl::input_iterator<Iter>));
            static_assert(( bsl::forward_iterator<Iter>));
            static_assert(( bsl::bidirectional_iterator<Iter>));
            static_assert((!bsl::random_access_iterator<Iter>));

            static_assert(( bsl::sentinel_for<Iter,Iter>));
        }

        {
            using Iter = TestRangeIterator<bsl::bidirectional_iterator_tag,
                                           int>;
            using Sentinel = TestRangeSentinel<int>;


            static_assert(( IteratorUtil_ModernIterator<Iter>));
            static_assert((bsl::same_as<
                               bsl::bidirectional_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
            static_assert(( bsl::input_or_output_iterator<Iter>));
            static_assert(( bsl::input_iterator<Iter>));
            static_assert(( bsl::forward_iterator<Iter>));
            static_assert(( bsl::bidirectional_iterator<Iter>));
            static_assert((!bsl::random_access_iterator<Iter>));

            static_assert(( bsl::sentinel_for<Iter,Iter>));
            static_assert(( bsl::sentinel_for<Sentinel, Iter> ));
        }

        // Random Access Iterators

        {
            using Iter = TestIterator<bsl::random_access_iterator_tag,
                                      int,
                                      IterConcept::e_NONE>;

            static_assert((!IteratorUtil_ModernIterator<Iter>));
            static_assert((bsl::same_as<
                               bsl::random_access_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
            static_assert((!bsl::input_or_output_iterator<Iter>));
            static_assert((!bsl::input_iterator<Iter>));
            static_assert((!bsl::forward_iterator<Iter>));
            static_assert((!bsl::bidirectional_iterator<Iter>));
            static_assert((!bsl::random_access_iterator<Iter>));

            static_assert((!bsl::sentinel_for<Iter,Iter>));
        }

        {
            using Iter = TestIterator<bsl::random_access_iterator_tag,
                                      int,
                                      IterConcept::e_INPUT_ITERATOR>;

            static_assert((!IteratorUtil_ModernIterator<Iter>));
            static_assert((bsl::same_as<
                               bsl::random_access_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
            static_assert(( bsl::input_or_output_iterator<Iter>));
            static_assert(( bsl::input_iterator<Iter>));
            static_assert((!bsl::forward_iterator<Iter>));
            static_assert((!bsl::bidirectional_iterator<Iter>));
            static_assert((!bsl::random_access_iterator<Iter>));

            static_assert(( bsl::sentinel_for<Iter,Iter>));
        }

        {
            using Iter = TestIterator<bsl::random_access_iterator_tag,
                                      int,
                                      IterConcept::e_ALL>;

            static_assert((!IteratorUtil_ModernIterator<Iter>));
            static_assert((bsl::same_as<
                               bsl::random_access_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
            static_assert(( bsl::input_or_output_iterator<Iter>));
            static_assert(( bsl::input_iterator<Iter>));
            static_assert(( bsl::forward_iterator<Iter>));
            static_assert(( bsl::bidirectional_iterator<Iter>));
            static_assert(( bsl::random_access_iterator<Iter>));

            static_assert(( bsl::sentinel_for<Iter,Iter>));
        }

        {
            using Iter = TestRangeIterator<bsl::random_access_iterator_tag,
                                           int>;
            using Sentinel = TestRangeSentinel<int>;

            static_assert(( IteratorUtil_ModernIterator<Iter>));
            static_assert((bsl::same_as<
                               bsl::random_access_iterator_tag,
                               typename bsl::iterator_traits<Iter>
                                   ::iterator_category
                           >));
            static_assert(( bsl::input_or_output_iterator<Iter>));
            static_assert(( bsl::input_iterator<Iter>));
            static_assert(( bsl::forward_iterator<Iter>));
            static_assert(( bsl::bidirectional_iterator<Iter>));
            static_assert(( bsl::random_access_iterator<Iter>));

            static_assert(( bsl::sentinel_for<Iter,Iter>));
            static_assert(( bsl::sentinel_for<Sentinel, Iter> ));
        }

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
    }

    static void testCase3Classifications()
    {
        if (verbose) printf("\n\tVerifying Iterator classification "
                            "coverage.\n");

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)
        ASSERTV(( insertDistanceClassification<
                     TestRangeIterator<bsl::forward_iterator_tag, int>,
                     TestRangeIterator<bsl::forward_iterator_tag, int>
                 >()),
          ( 1 == insertDistanceClassification<
                     TestRangeIterator<bsl::forward_iterator_tag, int>,
                     TestRangeIterator<bsl::forward_iterator_tag, int>
                 >() ));


        ASSERTV(( insertDistanceClassification<
                     TestRangeIterator<bsl::input_iterator_tag, int>,
                     TestRangeIterator<bsl::input_iterator_tag, int>
                 >()),
          ( 3 == insertDistanceClassification<
                     TestRangeIterator<bsl::input_iterator_tag, int>,
                     TestRangeIterator<bsl::input_iterator_tag, int>
                 >() ));

        ASSERTV(( insertDistanceClassification<
                     TestRangeIterator<bsl::input_iterator_tag, int>,
                     int*
                 >()),
          ( 4 == insertDistanceClassification<
                     TestRangeIterator<bsl::input_iterator_tag, int>,
                     int*
                 >() ));

        // With ranges we get closer to using `ranges::distance` but still
        // decide not to because the iterator doesn't satisfy the concepts.
        ASSERTV(( insertDistanceClassification<
                     TestIterator<bsl::input_iterator_tag, int>,
                     int*
                 >()),
          ( 4 == insertDistanceClassification<
                     TestIterator<bsl::input_iterator_tag, int>,
                     int*
                 >() ));
#else  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES

        // Without ranges we simply say no to sufficiently mismatched sentinel
        // types.
        ASSERTV(( insertDistanceClassification<
                     TestIterator<bsl::input_iterator_tag, int>,
                     int*
                 >()),
          ( 7 == insertDistanceClassification<
                     TestIterator<bsl::input_iterator_tag, int>,
                     int*
                 >() ));

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES

        ASSERT(( 5 == insertDistanceClassification<
                     TestIterator<bsl::forward_iterator_tag, int>,
                     TestIterator<bsl::forward_iterator_tag, int>
                 >() ));

        ASSERT(( 6 == insertDistanceClassification<
                     TestIterator<bsl::input_iterator_tag, int>,
                     TestIterator<bsl::input_iterator_tag, int>
                 >() ));
    }

    template <IterConcept::Enum t_CONCEPTLVL>
    static void testCase3Legacy()
    {
        if (verbose) printf("\n\tTesting C++17 iterators (concept level:"
                            "%s).\n", toString(t_CONCEPTLVL));

        int testData[] = { 42, 13, 56, 72, 39 };
        int numElements = sizeof(testData) / sizeof(int);

        if (veryVerbose) printf("\n\t\tTesting input iterator.\n");
        {
            typedef TestIterator<bsl::input_iterator_tag, int, t_CONCEPTLVL>
                                                          IntTestInputIterator;

            // `insertDistance` will always be 0 for this iterator
            BSLS_KEYWORD_CONSTEXPR bool canCalculate =
                IteratorUtil::canCalculateInsertDistance<
                    IntTestInputIterator,
                    IntTestInputIterator>();
            ASSERT(!canCalculate);

            BSLS_KEYWORD_CONSTEXPR int classification =
                insertDistanceClassification<
                    IntTestInputIterator,
                    IntTestInputIterator>();
            ASSERTV(classification, 6 == classification);

            IteratorUsageCount ac, bc;
            IntTestInputIterator a(&ac); const IntTestInputIterator& A = a;
            IntTestInputIterator b(&bc); const IntTestInputIterator& B = b;

            a.d_underlying = testData;

            for (int ti = 0; ti < numElements; ++ti) {
                ac.reset();
                bc.reset();

                b.d_underlying = testData + ti;
                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, 0 == DIST);

                ASSERTV(ac,
                        0 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_subtracts
                     && 0 == ac.d_copies
                     && 0 == ac.d_moves);
                ASSERTV(bc,
                        0 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_subtracts
                     && 0 == bc.d_copies
                     && 0 == bc.d_moves);
            }

            ASSERT((!IteratorUtil::canCalculateInsertDistance<
                        IntTestInputIterator,
                        typename IntTestInputIterator::UIter>()));
            ASSERT(( 0 == Obj::insertDistance(A, B.d_underlying) ));
        }

        if (veryVerbose) printf("\n\t\tTesting forward iterator.\n");
        {
            typedef TestIterator<bsl::forward_iterator_tag, int, t_CONCEPTLVL>
                                                        IntTestForwardIterator;

            BSLS_KEYWORD_CONSTEXPR bool canCalculate =
                IteratorUtil::canCalculateInsertDistance<
                    IntTestForwardIterator,
                    IntTestForwardIterator>();
            ASSERT(canCalculate);

            BSLS_KEYWORD_CONSTEXPR int classification =
                insertDistanceClassification<
                    IntTestForwardIterator,
                    IntTestForwardIterator>();
            ASSERTV(classification, 5 == classification);

            IteratorUsageCount ac, bc;
            IntTestForwardIterator a(&ac); const IntTestForwardIterator& A = a;
            IntTestForwardIterator b(&bc); const IntTestForwardIterator& B = b;

            {
                ac.reset();
                bc.reset();

                a.d_underlying = testData;
                b.d_underlying = testData;

                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, 0 == DIST);

                ASSERTV(ac,
                        1 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_difference);
                ASSERTV(bc,
                        1 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_difference);
            }

            for (int ti = 1; ti < numElements; ++ti) {
                ac.reset();
                bc.reset();

                b.d_underlying = testData + ti;
                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, static_cast<size_t>(ti) == DIST);

                ASSERTV(ti,
                        ac,
                        (ti + 1) == ac.d_compares
                     && ti == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_difference);
                ASSERTV(ti,
                        bc,
                        (ti + 1) == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_difference);
            }

            ASSERT((!IteratorUtil::canCalculateInsertDistance<
                        IntTestForwardIterator,
                        typename IntTestForwardIterator::UIter>()));
            ASSERT(( 0 == Obj::insertDistance(A, B.d_underlying) ));
        }

        if (veryVerbose) printf("\n\t\tTesting bidirectional iterator.\n");
        {
            typedef TestIterator<bsl::bidirectional_iterator_tag,
                                 int,
                                 t_CONCEPTLVL>    IntTestBidirectionalIterator;

            BSLS_KEYWORD_CONSTEXPR bool canCalculate =
                IteratorUtil::canCalculateInsertDistance<
                    IntTestBidirectionalIterator,
                    IntTestBidirectionalIterator>();
            ASSERT(canCalculate);

            BSLS_KEYWORD_CONSTEXPR int classification =
                insertDistanceClassification<
                    IntTestBidirectionalIterator,
                    IntTestBidirectionalIterator>();
            ASSERTV(classification, 5 == classification);

            IteratorUsageCount ac, bc;
            IntTestBidirectionalIterator        a(&ac);
            const IntTestBidirectionalIterator& A = a;
            IntTestBidirectionalIterator        b(&bc);
            const IntTestBidirectionalIterator& B = b;

            {
                ac.reset();
                bc.reset();

                a.d_underlying = testData;
                b.d_underlying = testData;

                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, 0 == DIST);

                ASSERTV(ac,
                        1 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_difference);
                ASSERTV(bc,
                        1 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_difference);
            }

            for (int ti = 1; ti < numElements; ++ti) {
                ac.reset();
                bc.reset();

                b.d_underlying = testData + ti;
                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, static_cast<size_t>(ti) == DIST);

                ASSERTV(ti,
                        ac,
                        (ti + 1) == ac.d_compares
                     && ti == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_difference);
                ASSERTV(ti,
                        bc,
                        (ti + 1) == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_difference);
            }

            ASSERT((!IteratorUtil::canCalculateInsertDistance<
                        IntTestBidirectionalIterator,
                        typename IntTestBidirectionalIterator::UIter>()));
            ASSERT(( 0 == Obj::insertDistance(A, B.d_underlying) ));
        }

        if (veryVerbose) printf("\n\t\tTesting random access iterator.\n");
        {
            typedef TestIterator<bsl::random_access_iterator_tag,
                                 int,
                                 t_CONCEPTLVL>     IntTestRandomAccessIterator;

            BSLS_KEYWORD_CONSTEXPR bool canCalculate =
                IteratorUtil::canCalculateInsertDistance<
                    IntTestRandomAccessIterator,
                    IntTestRandomAccessIterator>();
            ASSERT(canCalculate);

            BSLS_KEYWORD_CONSTEXPR int classification =
                insertDistanceClassification<
                    IntTestRandomAccessIterator,
                    IntTestRandomAccessIterator>();
            ASSERTV(classification, 5 == classification);

            IteratorUsageCount ac, bc;
            IntTestRandomAccessIterator        a(&ac);
            const IntTestRandomAccessIterator& A = a;
            IntTestRandomAccessIterator        b(&bc);
            const IntTestRandomAccessIterator& B = b;

            {
                ac.reset();
                bc.reset();

                a.d_underlying = testData;
                b.d_underlying = testData;

                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, 0 == DIST);

                ASSERTV(ac,
                        0 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 1 == ac.d_difference);
                ASSERTV(bc,
                        0 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 1 == bc.d_difference);
            }

            for (int ti = 1; ti < numElements; ++ti) {
                ac.reset();
                bc.reset();

                b.d_underlying = testData + ti;
                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, static_cast<size_t>(ti) == DIST);

                ASSERTV(ti,
                        ac,
                        0 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 1 == ac.d_difference);
                ASSERTV(ti,
                        bc,
                        0 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 1 == bc.d_difference);
            }

            ASSERT((!IteratorUtil::canCalculateInsertDistance<
                        IntTestRandomAccessIterator,
                        typename IntTestRandomAccessIterator::UIter>()));
            ASSERT(( 0 == Obj::insertDistance(A, B.d_underlying) ));
        }
    }

    static void testCase3Ranges()
    {
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)
        if (verbose) printf("\n\tTesting C++20 range iterators.\n");

        int testData[] = { 42, 13, 56, 72, 39 };
        int numElements = sizeof(testData) / sizeof(int);

        if (veryVerbose) printf("\n\t\tTesting input range iterator.\n");
        {
            typedef TestRangeIterator<bsl::input_iterator_tag, int>
                                                          IntTestInputIterator;

            // `insertDistance` will always be 0 for this iterator
            BSLS_KEYWORD_CONSTEXPR bool canCalculate =
                IteratorUtil::canCalculateInsertDistance<
                    IntTestInputIterator,
                    IntTestInputIterator>();
            ASSERT(!canCalculate);

            BSLS_KEYWORD_CONSTEXPR int classification =
                insertDistanceClassification<
                    IntTestInputIterator,
                    IntTestInputIterator>();
            ASSERTV(classification, 3 == classification);

            IteratorUsageCount ac, bc;
            IntTestInputIterator a(&ac); const IntTestInputIterator& A = a;
            IntTestInputIterator b(&bc); const IntTestInputIterator& B = b;

            a.d_underlying = testData;

            for (int ti = 0; ti < numElements; ++ti) {
                ac.reset();
                bc.reset();

                b.d_underlying = testData + ti;
                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, 0 == DIST);

                ASSERTV(ac,
                        0 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_subtracts
                     && 0 == ac.d_copies
                     && 0 == ac.d_moves);
                ASSERTV(bc,
                        0 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_subtracts
                     && 0 == bc.d_copies
                     && 0 == bc.d_moves);
            }

            ASSERT((!IteratorUtil::canCalculateInsertDistance<
                        IntTestInputIterator,
                        typename IntTestInputIterator::UIter>()));
            ASSERT(( 0 == Obj::insertDistance(A, B.d_underlying) ));
        }

        if (veryVerbose) printf("\n\t\tTesting input range iterator with "
                                "sentinel.\n");
        {
            typedef TestRangeIterator<bsl::input_iterator_tag, int>
                                                          IntTestInputIterator;
            typedef TestRangeSentinel<int> IntTestSentinel;

            // `insertDistance` will always be 0 for this iterator
            BSLS_KEYWORD_CONSTEXPR bool canCalculate =
                IteratorUtil::canCalculateInsertDistance<
                    IntTestInputIterator,
                    IntTestSentinel>();
            ASSERT(!canCalculate);

            BSLS_KEYWORD_CONSTEXPR int classification =
                insertDistanceClassification<
                    IntTestInputIterator,
                    IntTestSentinel>();
            ASSERTV(classification, 3 == classification);

            IteratorUsageCount ac, bc;
            IntTestInputIterator a(&ac); const IntTestInputIterator& A = a;
            IntTestSentinel      b(&bc); const IntTestSentinel&      B = b;

            a.d_underlying = testData;

            for (int ti = 0; ti < numElements; ++ti) {
                ac.reset();
                bc.reset();

                b.d_underlying = testData + ti;
                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, 0 == DIST);

                ASSERTV(ac,
                        0 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_subtracts
                     && 0 == ac.d_copies
                     && 0 == ac.d_moves);
                ASSERTV(bc,
                        0 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_subtracts
                     && 0 == bc.d_copies
                     && 0 == bc.d_moves);
            }
        }

        if (veryVerbose) printf("\n\t\tTesting forward range iterator.\n");
        {
            typedef TestRangeIterator<bsl::forward_iterator_tag, int>
                                                        IntTestForwardIterator;

            BSLS_KEYWORD_CONSTEXPR bool canCalculate =
                IteratorUtil::canCalculateInsertDistance<
                    IntTestForwardIterator,
                    IntTestForwardIterator>();
            ASSERT(canCalculate);

            BSLS_KEYWORD_CONSTEXPR int classification =
                insertDistanceClassification<
                    IntTestForwardIterator,
                    IntTestForwardIterator>();
            ASSERTV(classification, 1 == classification);

            IteratorUsageCount ac, bc;
            IntTestForwardIterator a(&ac); const IntTestForwardIterator& A = a;
            IntTestForwardIterator b(&bc); const IntTestForwardIterator& B = b;

            {
                ac.reset();
                bc.reset();

                a.d_underlying = testData;
                b.d_underlying = testData;

                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, 0 == DIST);

                ASSERTV(ac,
                        1 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_difference);
                ASSERTV(bc,
                        1 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_difference);
            }

            for (int ti = 1; ti < numElements; ++ti) {
                ac.reset();
                bc.reset();

                b.d_underlying = testData + ti;
                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, static_cast<size_t>(ti) == DIST);

                ASSERTV(ti,
                        ac,
                        (ti + 1) == ac.d_compares
                     && ti == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_difference);
                ASSERTV(ti,
                        bc,
                        (ti + 1) == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_difference);
            }

            ASSERT((!IteratorUtil::canCalculateInsertDistance<
                        IntTestForwardIterator,
                        typename IntTestForwardIterator::UIter>()));
            ASSERT(( 0 == Obj::insertDistance(A, B.d_underlying) ));
        }

        if (veryVerbose) printf("\n\t\tTesting forward range iterator with "
                                "sentinel.\n");
        {
            typedef TestRangeIterator<bsl::forward_iterator_tag, int>
                                                        IntTestForwardIterator;
            typedef TestRangeSentinel<int> IntTestSentinel;

            BSLS_KEYWORD_CONSTEXPR bool canCalculate =
                IteratorUtil::canCalculateInsertDistance<
                    IntTestForwardIterator,
                    IntTestSentinel>();
            ASSERT(canCalculate);

            BSLS_KEYWORD_CONSTEXPR int classification =
                insertDistanceClassification<
                    IntTestForwardIterator,
                    IntTestSentinel>();
            ASSERTV(classification, 1 == classification);

            IteratorUsageCount ac, bc;
            IntTestForwardIterator a(&ac); const IntTestForwardIterator& A = a;
            IntTestSentinel        b(&bc); const IntTestSentinel&        B = b;

            {
                ac.reset();
                bc.reset();

                a.d_underlying = testData;
                b.d_underlying = testData;

                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, 0 == DIST);

                ASSERTV(ac,
                        1 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_difference);
                ASSERTV(bc,
                        1 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_difference);
            }

            for (int ti = 1; ti < numElements; ++ti) {
                ac.reset();
                bc.reset();

                b.d_underlying = testData + ti;
                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, static_cast<size_t>(ti) == DIST);

                ASSERTV(ti,
                        ac,
                        (ti + 1) == ac.d_compares
                     && ti == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_difference);
                ASSERTV(ti,
                        bc,
                        (ti + 1) == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_difference);
            }
        }

        if (veryVerbose) printf("\n\t\tTesting bidirectional range "
                                "iterator.\n");
        {
            typedef TestRangeIterator<bsl::bidirectional_iterator_tag, int>
                                                  IntTestBidirectionalIterator;

            BSLS_KEYWORD_CONSTEXPR bool canCalculate =
                IteratorUtil::canCalculateInsertDistance<
                    IntTestBidirectionalIterator,
                    IntTestBidirectionalIterator>();
            ASSERT(canCalculate);

            BSLS_KEYWORD_CONSTEXPR int classification =
                insertDistanceClassification<
                    IntTestBidirectionalIterator,
                    IntTestBidirectionalIterator>();
            ASSERTV(classification, 1 == classification);

            IteratorUsageCount ac, bc;
            IntTestBidirectionalIterator        a(&ac);
            const IntTestBidirectionalIterator& A = a;
            IntTestBidirectionalIterator        b(&bc);
            const IntTestBidirectionalIterator& B = b;

            {
                ac.reset();
                bc.reset();

                a.d_underlying = testData;
                b.d_underlying = testData;

                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, 0 == DIST);

                ASSERTV(ac,
                        1 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_difference);
                ASSERTV(bc,
                        1 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_difference);
            }

            for (int ti = 1; ti < numElements; ++ti) {
                ac.reset();
                bc.reset();

                b.d_underlying = testData + ti;
                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, static_cast<size_t>(ti) == DIST);

                ASSERTV(ti,
                        ac,
                        (ti + 1) == ac.d_compares
                     && ti == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_difference);
                ASSERTV(ti,
                        bc,
                        (ti + 1) == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_difference);
            }

            ASSERT((!IteratorUtil::canCalculateInsertDistance<
                        IntTestBidirectionalIterator,
                        typename IntTestBidirectionalIterator::UIter>()));
            ASSERT(( 0 == Obj::insertDistance(A, B.d_underlying) ));
        }

        if (veryVerbose) printf("\n\t\tTesting bidirectional range iterator "
                                "with sentinel.\n");
        {
            typedef TestRangeIterator<bsl::bidirectional_iterator_tag, int>
                                                  IntTestBidirectionalIterator;
            typedef TestRangeSentinel<int> IntTestSentinel;

            BSLS_KEYWORD_CONSTEXPR bool canCalculate =
                IteratorUtil::canCalculateInsertDistance<
                    IntTestBidirectionalIterator,
                    IntTestSentinel>();
            ASSERT(canCalculate);

            BSLS_KEYWORD_CONSTEXPR int classification =
                insertDistanceClassification<
                    IntTestBidirectionalIterator,
                    IntTestSentinel>();
            ASSERTV(classification, 1 == classification);

            IteratorUsageCount ac, bc;
            IntTestBidirectionalIterator        a(&ac);
            const IntTestBidirectionalIterator& A = a;
            IntTestSentinel                     b(&bc);
            const IntTestSentinel&              B = b;

            {
                ac.reset();
                bc.reset();

                a.d_underlying = testData;
                b.d_underlying = testData;

                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, 0 == DIST);

                ASSERTV(ac,
                        1 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_difference);
                ASSERTV(bc,
                        1 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_difference);
            }

            for (int ti = 1; ti < numElements; ++ti) {
                ac.reset();
                bc.reset();

                b.d_underlying = testData + ti;
                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, static_cast<size_t>(ti) == DIST);

                ASSERTV(ti,
                        ac,
                        (ti + 1) == ac.d_compares
                     && ti == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_difference);
                ASSERTV(ti,
                        bc,
                        (ti + 1) == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_difference);
            }
        }

        if (veryVerbose) printf("\n\t\tTesting random access iterator.\n");
        {
            typedef TestRangeIterator<bsl::random_access_iterator_tag, int>
                                                   IntTestRandomAccessIterator;

            BSLS_KEYWORD_CONSTEXPR bool canCalculate =
                IteratorUtil::canCalculateInsertDistance<
                    IntTestRandomAccessIterator,
                    IntTestRandomAccessIterator>();
            ASSERT(canCalculate);

            BSLS_KEYWORD_CONSTEXPR int classification =
                insertDistanceClassification<
                    IntTestRandomAccessIterator,
                    IntTestRandomAccessIterator>();
            ASSERTV(classification, 1 == classification);

            IteratorUsageCount ac, bc;
            IntTestRandomAccessIterator        a(&ac);
            const IntTestRandomAccessIterator& A = a;
            IntTestRandomAccessIterator        b(&bc);
            const IntTestRandomAccessIterator& B = b;

            {
                ac.reset();
                bc.reset();

                a.d_underlying = testData;
                b.d_underlying = testData;

                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, 0 == DIST);

                ASSERTV(ac,
                        0 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 1 == ac.d_difference);
                ASSERTV(bc,
                        0 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 1 == bc.d_difference);
            }

            for (int ti = 1; ti < numElements; ++ti) {
                ac.reset();
                bc.reset();

                b.d_underlying = testData + ti;
                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, static_cast<size_t>(ti) == DIST);

                ASSERTV(ti,
                        ac,
                        0 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 1 == ac.d_difference);
                ASSERTV(ti,
                        bc,
                        0 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 1 == bc.d_difference);
            }

            ASSERT((!IteratorUtil::canCalculateInsertDistance<
                        IntTestRandomAccessIterator,
                        typename IntTestRandomAccessIterator::UIter>()));
            ASSERT(( 0 == Obj::insertDistance(A, B.d_underlying) ));
        }

        if (veryVerbose) printf("\n\t\tTesting random access iterator with "
                                "sentinel.\n");
        {
            typedef TestRangeIterator<bsl::random_access_iterator_tag, int>
                                                   IntTestRandomAccessIterator;
            typedef TestRangeSentinel<int> IntTestSentinel;

            BSLS_KEYWORD_CONSTEXPR bool canCalculate =
                IteratorUtil::canCalculateInsertDistance<
                    IntTestRandomAccessIterator,
                    IntTestSentinel>();
            ASSERT(canCalculate);

            BSLS_KEYWORD_CONSTEXPR int classification =
                insertDistanceClassification<
                    IntTestRandomAccessIterator,
                    IntTestSentinel>();
            ASSERTV(classification, 1 == classification);

            IteratorUsageCount ac, bc;
            IntTestRandomAccessIterator        a(&ac);
            const IntTestRandomAccessIterator& A = a;
            IntTestSentinel                    b(&bc);
            const IntTestSentinel&             B = b;

            {
                ac.reset();
                bc.reset();

                a.d_underlying = testData;
                b.d_underlying = testData;

                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, 0 == DIST);

                ASSERTV(ac,
                        1 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_difference);
                ASSERTV(bc,
                        1 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_difference);
            }

            for (int ti = 1; ti < numElements; ++ti) {
                ac.reset();
                bc.reset();

                b.d_underlying = testData + ti;
                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, static_cast<size_t>(ti) == DIST);

                ASSERTV(ti,
                        ac,
                        (ti + 1) == ac.d_compares
                     && ti == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_difference);
                ASSERTV(ti,
                        bc,
                        (ti + 1) == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_difference);
            }
        }
#else  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
        if (verbose) printf("\n\tNo concepts or ranges, not testing C++20 "
                            "range iterators.\n");
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
    }

    static void testCase3Poisoned()
    {
        if (verbose) printf("\n\tTesting C++17 poisoned iterators.\n");

        int testData[] = { 42, 13, 56, 72, 39 };
        int numElements = sizeof(testData) / sizeof(int);

        if (veryVerbose) printf("\n\t\tTesting input iterator.\n");
        {
            typedef TestPoisonedIterator<
                bsl::input_iterator_tag,
                int> IntTestInputIterator;

            // `insertDistance` will always be 0 for this iterator
            BSLS_KEYWORD_CONSTEXPR bool canCalculate =
                IteratorUtil::canCalculateInsertDistance<
                    IntTestInputIterator,
                    IntTestInputIterator>();
            ASSERT(!canCalculate);

            BSLS_KEYWORD_CONSTEXPR int classification =
                insertDistanceClassification<
                    IntTestInputIterator,
                    IntTestInputIterator>();
            ASSERTV(classification, 6 == classification);

            IteratorUsageCount ac, bc;
            IntTestInputIterator a(&ac); const IntTestInputIterator& A = a;
            IntTestInputIterator b(&bc); const IntTestInputIterator& B = b;

            a.d_underlying = testData;

            for (int ti = 0; ti < numElements; ++ti) {
                ac.reset();
                bc.reset();

                b.d_underlying = testData + ti;
                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, 0 == DIST);

                ASSERTV(ac,
                        0 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_subtracts
                     && 0 == ac.d_copies
                     && 0 == ac.d_moves);
                ASSERTV(bc,
                        0 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_subtracts
                     && 0 == bc.d_copies
                     && 0 == bc.d_moves);
            }

            ASSERT((!IteratorUtil::canCalculateInsertDistance<
                        IntTestInputIterator,
                        typename IntTestInputIterator::UIter>()));
            ASSERT(( 0 == Obj::insertDistance(A, B.d_underlying) ));
        }

        if (veryVerbose) printf("\n\t\tTesting forward iterator.\n");
        {
            typedef TestPoisonedIterator<
                bsl::forward_iterator_tag,
                int> IntTestForwardIterator;

            BSLS_KEYWORD_CONSTEXPR bool canCalculate =
                IteratorUtil::canCalculateInsertDistance<
                    IntTestForwardIterator,
                    IntTestForwardIterator>();
            ASSERT(canCalculate);

            BSLS_KEYWORD_CONSTEXPR int classification =
                insertDistanceClassification<
                    IntTestForwardIterator,
                    IntTestForwardIterator>();
            ASSERTV(classification, 5 == classification);

            IteratorUsageCount ac, bc;
            IntTestForwardIterator a(&ac); const IntTestForwardIterator& A = a;
            IntTestForwardIterator b(&bc); const IntTestForwardIterator& B = b;

            {
                ac.reset();
                bc.reset();

                a.d_underlying = testData;
                b.d_underlying = testData;

                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, 0 == DIST);

                ASSERTV(ac,
                        1 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_difference);
                ASSERTV(bc,
                        1 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_difference);
            }

            for (int ti = 1; ti < numElements; ++ti) {
                ac.reset();
                bc.reset();

                b.d_underlying = testData + ti;
                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, static_cast<size_t>(ti) == DIST);

                ASSERTV(ti,
                        ac,
                        (ti + 1) == ac.d_compares
                     && ti == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_difference);
                ASSERTV(ti,
                        bc,
                        (ti + 1) == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_difference);
            }

            ASSERT((!IteratorUtil::canCalculateInsertDistance<
                        IntTestForwardIterator,
                        typename IntTestForwardIterator::UIter>()));
            ASSERT(( 0 == Obj::insertDistance(A, B.d_underlying) ));
        }

        if (veryVerbose) printf("\n\t\tTesting bidirectional iterator.\n");
        {
            typedef TestPoisonedIterator<
                bsl::bidirectional_iterator_tag,
                int> IntTestBidirectionalIterator;

            BSLS_KEYWORD_CONSTEXPR bool canCalculate =
                IteratorUtil::canCalculateInsertDistance<
                    IntTestBidirectionalIterator,
                    IntTestBidirectionalIterator>();
            ASSERT(canCalculate);

            BSLS_KEYWORD_CONSTEXPR int classification =
                insertDistanceClassification<
                    IntTestBidirectionalIterator,
                    IntTestBidirectionalIterator>();
            ASSERTV(classification, 5 == classification);

            IteratorUsageCount ac, bc;
            IntTestBidirectionalIterator        a(&ac);
            const IntTestBidirectionalIterator& A = a;
            IntTestBidirectionalIterator        b(&bc);
            const IntTestBidirectionalIterator& B = b;

            {
                ac.reset();
                bc.reset();

                a.d_underlying = testData;
                b.d_underlying = testData;

                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, 0 == DIST);

                ASSERTV(ac,
                        1 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_difference);
                ASSERTV(bc,
                        1 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_difference);
            }

            for (int ti = 1; ti < numElements; ++ti) {
                ac.reset();
                bc.reset();

                b.d_underlying = testData + ti;
                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, static_cast<size_t>(ti) == DIST);

                ASSERTV(ti,
                        ac,
                        (ti + 1) == ac.d_compares
                     && ti == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 0 == ac.d_difference);
                ASSERTV(ti,
                        bc,
                        (ti + 1) == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 0 == bc.d_difference);
            }

            ASSERT((!IteratorUtil::canCalculateInsertDistance<
                        IntTestBidirectionalIterator,
                        typename IntTestBidirectionalIterator::UIter>()));
            ASSERT(( 0 == Obj::insertDistance(A, B.d_underlying) ));
        }

        if (veryVerbose) printf("\n\t\tTesting random access iterator.\n");
        {
            typedef TestPoisonedIterator<
                bsl::random_access_iterator_tag,
                int> IntTestRandomAccessIterator;

            BSLS_KEYWORD_CONSTEXPR bool canCalculate =
                IteratorUtil::canCalculateInsertDistance<
                    IntTestRandomAccessIterator,
                    IntTestRandomAccessIterator>();
            ASSERT(canCalculate);

            BSLS_KEYWORD_CONSTEXPR int classification =
                insertDistanceClassification<
                    IntTestRandomAccessIterator,
                    IntTestRandomAccessIterator>();
            ASSERTV(classification, 5 == classification);

            IteratorUsageCount ac, bc;
            IntTestRandomAccessIterator        a(&ac);
            const IntTestRandomAccessIterator& A = a;
            IntTestRandomAccessIterator        b(&bc);
            const IntTestRandomAccessIterator& B = b;

            {
                ac.reset();
                bc.reset();

                a.d_underlying = testData;
                b.d_underlying = testData;

                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, 0 == DIST);

                ASSERTV(ac,
                        0 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 1 == ac.d_difference);
                ASSERTV(bc,
                        0 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 1 == bc.d_difference);
            }

            for (int ti = 1; ti < numElements; ++ti) {
                ac.reset();
                bc.reset();

                b.d_underlying = testData + ti;
                const size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, static_cast<size_t>(ti) == DIST);

                ASSERTV(ti,
                        ac,
                        0 == ac.d_compares
                     && 0 == ac.d_preIncrement
                     && 0 == ac.d_postIncrement
                     && 1 == ac.d_difference);
                ASSERTV(ti,
                        bc,
                        0 == bc.d_compares
                     && 0 == bc.d_preIncrement
                     && 0 == bc.d_postIncrement
                     && 1 == bc.d_difference);
            }

            ASSERT((!IteratorUtil::canCalculateInsertDistance<
                        IntTestRandomAccessIterator,
                        typename IntTestRandomAccessIterator::UIter>()));
            ASSERT(( 0 == Obj::insertDistance(A, B.d_underlying) ));
        }
    }



};

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int      test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: Neither the test driver nor component should lock the default
    // allocator

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Finding the Distance Between Two Random Access Iterators
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to find the number of elements between two random access
// iterators.
//
// First, we create an array of integer values and two pointers (which are
// considered random access iterators) referring to the beginning and end of a
// range within that array:
// ```
        int values[] = { 1, 2, 3, 4, 5 };
        int *begin = &values[0];
        int *end   = &values[3];
// ```
// Now, we use the `IteratorUtil::insertDistance` class method to calculate the
// distance of the open range [`begin`, `end`):
// ```
        std::size_t distance = IteratorUtil::insertDistance(begin, end);
        ASSERT(3 == distance);

// ```

      } break;
      case 4: {
        //---------------------------------------------------------------------
        // TESTING ALIAS TEMPLATES (AT COMPILE TIME)
        //   Ensure that the deduction guides are properly specified to deduce
        //   the template arguments from the arguments supplied to the
        //   constructors.
        //
        // Concerns:
        // 1. Instantiation with both pointers and iterators "returns" the
        //    correct type.
        //
        // Plan:
        // 1. Instantiate the alias templates with different iterator and
        //    pointer types.
        //
        // 2. Verify that the "returned" type is correct.
        //
        // Testing:
        //   ALIAS TEMPLATES FOR DEDUCTION GUIDES
        //---------------------------------------------------------------------
        if (verbose)
            printf(
              "\nTESTING ALIAS TEMPLATES (AT COMPILE TIME)"
              "\n==========================================\n");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
        // This is a compile-time only test case.
        TestAliasTemplates test;
        (void) test; // This variable only exists for ease of IDE navigation.
#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // `insertDistance` and `canCalculateInsertDistance`
        //
        // Concerns:
        // 1. The function returns 0 for input iterators of each potential
        //    iterator category.
        //
        //   1. C++17 iterators that do not model any range iterator concepts.
        //
        //   2. C++17 iterators that model `input_iterator` but do not model
        //      the full concept for their iterator category.
        //
        //   3. C++17 iterators that model the concept for their iterator
        //      category as well
        //
        //   4. C++20 range iterators (defined with `iterator_concept` where
        //      being copyable will cause `iterator_traits` to identify the
        //      `iterator_category` as `forward_iterator_tag` even when the
        //      `iterator_concept` is `input_iterator_tag`.
        //
        //   5. Legacy iterators where even checking that they satisfy the
        //      modern concepts would be a hard error must also still work
        //      correctly.
        //
        // 2. The function returns the distance between two iterators if the
        //    iterators are either forward, bidirectional, or random-access
        //    iterators.
        //
        // 3. The function returns the distance between an iterator and a
        //    sentinel if the iterators are either forward, bidirectional, or
        //    random-access iterators.
        //
        // 4. The functions should return `false` and 0 when passed a sentinel
        //    type that does not convert to the iterator type in C++03 or a
        //    mismatched sentinel type on any platform.
        //
        // Plan:
        // 1. Create iterators of each category using the C++17 `TestIterator`,
        //    the C++20 `TestRangeIterator`, and the more limited C++17
        //    `TestPoisonedIterator` along with corresponding sentinels.
        //
        // 2. For each category verify the output of the functions and the
        //    number of operations performed when `insertDistance` is invoked.
        //
        //   1. Input iterators should not be able to calculate distance,
        //      distance should return 0, and no operations should be
        //      performed.
        //
        //   2. Forward iterators (that are not random access or are paired
        //      with a sentinel) should return the correct distance and execute
        //      the correct number of preIncrements and compares.
        //
        //   3. Random access iterators should return the correct distance and
        //      execute a single difference operation.
        //
        //   4. In all cases we double check that the code path we expected to
        //      have followed within `canCalculateInsertDistance` and
        //      `insertDistance` was followed by checking the matching return
        //      value from `insertDistanceClassification`.
        //
        //   5. We verify that all paths in `insertDistanceClassification` are
        //      exercised and that the appropriate paths are followed when
        //      mismatched sentinels are used by checking the functions with
        //      each iterator type and its underlying type as a sentinel.
        //
        // Testing:
        //   bool canCalculateInsertDistance()
        //   size_t insertDistance(InputIterator, InputIterator)
        //   size_t insertDistance(InputIterator, Sentinel)
        // --------------------------------------------------------------------

        if (verbose)
            printf("\n'insertDistance' and 'canCalculateInsertDistance'"
                   "\n=================================================\n");

        TestTestIterators::testCase3Classifications();
        TestTestIterators::testCase3Legacy<IterConcept::e_NONE>();
        TestTestIterators::testCase3Legacy<IterConcept::e_INPUT_ITERATOR>();
        TestTestIterators::testCase3Legacy<IterConcept::e_ALL>();
        TestTestIterators::testCase3Ranges();
        TestTestIterators::testCase3Poisoned();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST INFRASTRUCTURE
        //   This case verifies compile-time properties of test iterators
        //
        // Concerns:
        // 1. Usage of `TestIterator`, `TestRangeIterator`,
        //    `TestRangeSentinel`, and `TestPoisonedIterator` should update a
        //    provided `IteratorUsageCount` object correctly.
        //
        // 2. The `TestIterator` instantiations should have the appropriate
        //    iterator category and also satisfy the corresponding concept.
        //
        // 3. The `TestRangeIterator` instantiations satisfy the appropriate
        //    concepts, and have the right iterator category (except for input
        //    iterators, which will have a forward iterator category).
        //
        // 4. The `TestPoisonedIterator` should have a small but usable subset
        //    of the above properties, though many of the poisoned cases cannot
        //    be explicitly negative tested at compile time.
        //
        // Plan:
        // 1. Compile-time checks of the various metafunction results and
        //    concepts.
        //
        // Testing:
        //   TEST INFRASTRUCTURE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST INFRASTRUCTURE"
                            "\n===================\n");

        TestTestIterators::testCountsLegacy<IterConcept::e_NONE>();
        TestTestIterators::testCountsLegacy<IterConcept::e_INPUT_ITERATOR>();
        TestTestIterators::testCountsLegacy<IterConcept::e_ALL>();
        TestTestIterators::testCountsRanges();
        TestTestIterators::testCountsPoisoned();

        TestTestIterators::testCategories<IterConcept::e_NONE>();
        TestTestIterators::testCategories<IterConcept::e_INPUT_ITERATOR>();
        TestTestIterators::testCategories<IterConcept::e_ALL>();
        TestTestIterators::testRangeCategories();
        TestTestIterators::testPoisonedCategories();
        TestTestIterators::testConcepts();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Perform some ad-hoc tests.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef TestIterator<bsl::input_iterator_tag,   int>
                                                          IntTestInputIterator;
        typedef TestIterator<bsl::forward_iterator_tag, int>
                                                        IntTestForwardIterator;

        if (veryVerbose) printf("\n\t\tTest uninitialized input iterators\n");
        {
            IntTestInputIterator a; const IntTestInputIterator& A = a;
            IntTestInputIterator b; const IntTestInputIterator& B = b;

            const size_t DIST = Obj::insertDistance(A, B);
            ASSERTV(DIST, 0 == DIST);
        }

        if (veryVerbose) printf("\n\t\tTest initialized input iterators\n");
        {
            IntTestInputIterator a; const IntTestInputIterator& A = a;
            IntTestInputIterator b; const IntTestInputIterator& B = b;

            a.d_underlying = 0;
            b.d_underlying = 0;

            const size_t DIST = Obj::insertDistance(A, B);
            ASSERTV(DIST, 0 == DIST);
            ASSERTV(A.d_underlying, 0 == A.d_underlying);
            ASSERTV(B.d_underlying, 0 == B.d_underlying);

            IntTestInputIterator::pointer DEADBEEF =
                                    (IntTestInputIterator::pointer) 0xdeadbeef;
            b.d_underlying = DEADBEEF;

            const size_t DIST2 = Obj::insertDistance(A, B);
            ASSERTV(DIST2, 0 == DIST2);
            ASSERTV(A.d_underlying,     0 == A.d_underlying);
            ASSERTV(B.d_underlying, DEADBEEF == B.d_underlying);
        }

        if (veryVerbose) printf("\n\t\tTest initialized forward iterators\n");
        {
            IntTestForwardIterator a; const IntTestForwardIterator& A = a;
            IntTestForwardIterator b; const IntTestForwardIterator& B = b;

            a.d_underlying = 0;
            b.d_underlying = 0;

            size_t DIST = Obj::insertDistance(A, B);
            ASSERTV(DIST, 0 == DIST);
            ASSERTV(A.d_underlying, 0 == A.d_underlying);
            ASSERTV(B.d_underlying, 0 == B.d_underlying);

            b++;

            DIST = Obj::insertDistance(A, B);
            ASSERTV(DIST, 1 == DIST);
            ASSERTV(A.d_underlying, 0 == A.d_underlying);
            ASSERTV(B.d_underlying, A.d_underlying + 1 == B.d_underlying);

            a++;

            DIST = Obj::insertDistance(A, B);
            ASSERTV(DIST, 0 == DIST);
            ASSERTV(A.d_underlying, B.d_underlying,
                    B.d_underlying == A.d_underlying);

            // Choose 0xdeadbeec instead of 0xdeadbeef because of alignment.

            IntTestInputIterator::pointer PTR =
                                    (IntTestInputIterator::pointer) 0x10;
            a.d_underlying = 0;
            b.d_underlying = PTR;

            const size_t EXP_DIST2 = 4;

            const size_t DIST2 = Obj::insertDistance(A, B);
            ASSERTV(DIST2,   EXP_DIST2 == DIST2);
            ASSERTV(A.d_underlying,         0 == A.d_underlying);
            ASSERTV(B.d_underlying,       PTR == B.d_underlying);
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
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
