// bslstl_pair.t.cpp                                                  -*-C++-*-
#include <bslstl_pair.h>

#include <bsltf_allocargumenttype.h>
#include <bsltf_allocbitwisemoveabletesttype.h>
#include <bsltf_allocemplacabletesttype.h>
#include <bsltf_argumenttype.h>
#include <bsltf_emplacabletesttype.h>
#include <bsltf_movablealloctesttype.h>
#include <bsltf_movabletesttype.h>
#include <bsltf_moveonlyalloctesttype.h>
#include <bsltf_movestate.h>
#include <bsltf_simpletesttype.h>
#include <bsltf_templatetestfacility.h>

#include <bslma_allocator.h>
#include <bslma_allocatortraits.h>
#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_managedptr.h>
#include <bslma_stdallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_isbitwiseequalitycomparable.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isintegral.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_usesallocatorargt.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_cpp11.h>
#include <bsls_nameof.h>
#include <bsls_types.h>

#include <bsltf_movablealloctesttype.h>
#include <bsltf_movestate.h>
#include <bsltf_simpletesttype.h>
#include <bsltf_templatetestfacility.h>

#include <stddef.h>
#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>     // 'strcmp'

#include <algorithm>    // 'std::swap'

// Local macros to detect and work around compiler defects.

#if !defined(BSLS_COMPILER_FEATURES_SUPPORT_RVALUE_REFERENCES) \
 &&  defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION == 0x5130
#   define BSL_COMPILER_THINKS_MOVE_AMBIGUOUS_WITH_COPY 1
#endif

#if defined(BSL_COMPILER_THINKS_MOVE_AMBIGUOUS_WITH_COPY)
# define BAD_MOVE_GUARD(IDENTIFIER) int
#else
# define BAD_MOVE_GUARD(IDENTIFIER) IDENTIFIER
#endif

using namespace BloombergLP;
using bsls::NameOf;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// Because 'bsl::pair' is a simple struct containing two data members, there
// are no real boundary conditions to test.  The main issue to be tested is
// that the four variants of the class have working constructors.  The class
// behaves differently when instantiated with types that use
// 'bslma::Allocator'.  Testing consists mostly of testing all constructors
// with every combination of allocator-using/non-allocator-using template
// parameters.  The other combinatorial issue is the propagation of type traits
// from template parameters to 'bsl::pair' specializations.  Since this is a
// compile-time computation, it is only necessary to instantiate a
// representative combination of traits.
//
// The test driver is rounded out by invoking each comparison operator for
// each possible result to ensure that they work correctly.  There is no need
// to do more exhaustive tests on the relationship operators because they are
// simple pass-through operations and the only thing we are protecting against
// are typos.
//
// TBD: we need to add test cases for a bunch of new methods that were added
//      for the cpp11 project -- have added them to the plan below but have
//      not yet created the test cases.
//-----------------------------------------------------------------------------
// [ 2] typedef T1 first_type;
// [ 2] typedef T2 second_type;
// [ 2] T1 first;
// [ 2] T1 second;
// [ 2] pair();
// [ 2] pair(AllocatorPtr basicAllocator);
// [ 2] pair(const T1& a, const T2& b);
// [ 2] pair(const T1& a, const T2& b, AllocatorPtr basicAllocator);
// [10] template <class U1, class U2> pair(U1&& a, U2&& b);
// [10] template <class U1, class U2> pair(U1&& a, U2&& b, Alloc a);
// [10] pair(first_type&& a, second_type&& b);
// [10] pair(first_type&& a, second_type&& b, AllocatorPtr a);
// [11] template <class U1, class U2> pair(U1&& a, const U2& b);
// [11] template <class U1, class U2> pair(const U1& a, U2&& b);
// [11] template <class U1, class U2> pair(const U1& a, const U2& b);
// [11] template <class U1, class U2> pair(const U1& a, U2&& b, Alloc a);
// [11] template <class U1, class U2> pair(U1&& a, const U2& b, Alloc a);
// [11] template <class U1, class U2> pair(const U1& a, const U2& b, A a);
// [11] template <class U1, class U2> pair(const pair<U1, U2>& pr);
// [11] template <class U1, class U2> pair(const pair<U1, U2>& pr, Alloc a);
// [11] pair(first_type&& a, const second_type& b);
// [11] pair(const first_type& a, second_type&& b);
// [11] pair(const first_type& a, const second_type& b);
// [11] pair(const first_type& a, second_type&& b, Alloc a);
// [11] pair(first_type&& a, const second_type& b, Alloc a);
// [11] pair(const first_type& a, const second_type& b, A a);
// [11] pair(const pair<first_type, second_type>& pr);
// [11] pair(const pair<first_type, second_type>& pr, Alloc a);
// [13] pair(piecewise_construct_t, tuple aArgs, tuple bArgs)
// [13] pair(piecewise_construct_t, tuple aArgs, tuple bArgs, basicAllocator)
// [14] pair(native_std::pair<*>, bool>)
// [ 2] pair(const pair& original);
// [ 2] pair(const pair& original, AllocatorPtr basicAllocator);
// [ 4] pair(const pair<U1, U2>& rhs);
// [ 4] pair(const pair<U1, U2>& rhs, AllocatorPtr basicAllocator);
// [ 9] template <class U1, class U2> pair(pair<U1, U2>&& other)
// [ 9] template <class U1, class U2> pair(pair<U1, U2>&& other, AllocatorPtr)
// [ 9] pair(pair&& original)
// [ 9] pair(pair&& original, AllocatorPtr basicAllocator)
// [  ] pair(const native_std::pair<U1, U2>& rhs);
// [  ] pair(const native_std::pair<U1, U2>&, BloombergLP::bslma::Allocator *);
// [ 2] ~pair();
// [12] pair& operator=(const pair& rhs);
// [12] pair& operator=(pair&& rhs);
// [12] pair& operator=(const pair<U1, U2>& rhs)
// [12] pair& operator=(pair<U1, U2>&& rhs)
// [  ] pair& operator=(const native_std::pair<U1, U2>& rhs);
// [ 2] bool operator==(const pair& x, const pair& y);
// [ 2] bool operator!=(const pair& x, const pair& y);
// [ 2] bool operator<(const pair& x, const pair& y);
// [ 2] bool operator>(const pair& x, const pair& y);
// [ 2] bool operator<=(const pair& x, const pair& y);
// [ 2] bool operator>=(const pair& x, const pair& y);
// [ 5] void pair::swap(pair& rhs);
// [ 5] void swap(pair& lhs, pair& rhs);
// [ 6] hashAppend(HASHALG& hashAlg, const pair<T1,T2>&  input);
// [13] bsl::pair(piecewise_construct, tuple, tuple);
// [13] bsl::pair(piecewise_construct, tuple, tuple, alloc);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [15] USAGE EXAMPLE
// [ 3] Type Traits
// [ 7] Concern: Can create a pointer-to-member for 'first' and 'second'
// [ 8] Concern: Can assign to a 'pair' of references
// [16] Concern: Methods qualifed 'noexcept' in standard are so implemented.

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

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

#define EXCEPTION_TEST_BEGIN(CONTAINER)                                       \
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(                             \
          (* (bslma::TestAllocator *) (CONTAINER).get_allocator().mechanism()))

#define EXCEPTION_TEST_END  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsltf::TemplateTestFacility TTF;
typedef bsltf::MoveState            MoveState;
typedef bslmf::MovableRefUtil       MoveUtil;
typedef bsls::Types::Int64          Int64;
typedef bslma::ConstructionUtil     ConstrUtil;

bool             verbose = false;
bool         veryVerbose = false;
bool     veryVeryVerbose = false;
bool veryVeryVeryVerbose = false;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {
namespace u {

enum { k_VALUE_SHIFT = 10 };

class Base {
    // DATA
    int d_data;

  public:
    // CREATORS
    Base() : d_data(0) {}
    explicit
    Base(int data) : d_data(data) {}
    Base(const Base& original) :d_data(original.d_data) {}

    //! ~Base() = default;
    //! Base& operator=(const Base&) = default;

    // ACCESSORS
    operator int() const { return d_data; }
};

class AlBase {
    // Like 'Base', except it thinks it allocates.

    // DATA
    int              *d_data_p;
    bslma::Allocator *d_allocator_p;

  public:
    // CREATORS
    explicit
    AlBase(bslma::Allocator *allocator = 0)
    : d_allocator_p(bslma::Default::allocator(allocator))
    {
        d_data_p = new (*d_allocator_p) int(0);
    }

    explicit
    AlBase(int data, bslma::Allocator *allocator = 0)
    : d_allocator_p(bslma::Default::allocator(allocator))
    {
        d_data_p = new (*d_allocator_p) int(data);
    }

    AlBase(const AlBase& original, bslma::Allocator *allocator = 0)
    : d_allocator_p(bslma::Default::allocator(allocator))
    {
        d_data_p = new (*d_allocator_p) int(*original.d_data_p);
    }

    ~AlBase()
    {
        d_allocator_p->deleteObjectRaw(d_data_p);
    }

    //! ~AlBase() = default;

    // MANIPULATOR
    AlBase& operator=(const AlBase& rhs)
    {
        *d_data_p = *rhs.d_data_p;
        return *this;
    }

    // ACCESSORS
    operator int() const { return *d_data_p; }

    bslma::Allocator *allocator() const { return d_allocator_p; }
};

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

class Node {
    // This class is to test conversions from 'Node' to 'Base', which are
    // implicit, but less implicit than conversions from 'Derived' to 'Base'.
    // It turns out that in a C++03 move, a conversion from 'MovableRef<Node>'
    // to 'Base' takes too many steps to work, while in a C++11 move, the
    // conversion from 'Node&&' to 'Base' works, so everything involving 'Node'
    // has to be conditionally compiled for C++11 only.

    // DATA
    int d_data;

  public:
    // CREATORS
    Node() : d_data(0) {}
    explicit
    Node(int data) : d_data(data) {}
    Node(const Node& original) :d_data(original.d_data) {}

    //! ~Node() = default;
    //! Node& operator=(const Node&) = default;

    // ACCESSORS
    int data() const { return d_data; }

    operator Base() const { return Base(d_data); }
};

#endif

struct Derived : public Base {
    // CREATORS
    Derived() : Base() {}
    explicit
    Derived(int data) : Base(data) {}
    Derived(const Derived& original) : Base(original) {}
};

struct AlDerived : public AlBase {
    // CREATORS
    explicit
    AlDerived(bslma::Allocator *allocator = 0)
    : AlBase(allocator)
    {}

    explicit
    AlDerived(int data, bslma::Allocator *allocator = 0)
    : AlBase(data, allocator)
    {}

    AlDerived(const AlDerived& original, bslma::Allocator *allocator = 0)
    : AlBase(original, allocator)
    {}

    // MANIPULATOR
    AlDerived& operator=(const AlDerived& rhs)
    {
        * static_cast<AlBase *>(this) = rhs;

        return *this;
    }
};

template <class TYPE, int ALLOCATOR_ACCESSOR_CLASS =
                (bsl::is_same<TYPE, bsltf::AllocArgumentType<1> >::value
              || bsl::is_same<TYPE, bsltf::AllocArgumentType<2> >::value
              || bsl::is_same<TYPE, bsltf::AllocArgumentType<3> >::value
              || bsl::is_same<TYPE, bsltf::AllocBitwiseMoveableTestType>::value
              || bsl::is_same<TYPE, bsltf::AllocTestType>::value
              || bsl::is_same<TYPE, bsltf::MovableAllocTestType>::value
              || bsl::is_same<TYPE, bsltf::MoveOnlyAllocTestType>::value
              || bsl::is_same<TYPE, AlBase>::value
              || bsl::is_same<TYPE, AlDerived>::value)
              ? 1    // 'allocator()'
              : 0>   // no accessor
struct AllocatorMatchesImp {
};

template <class TYPE>
struct AllocatorMatchesImp<TYPE, 0> {
    bool operator()(const TYPE&, bslma::Allocator *) const
        // The specified 'TYPE' has no 'allocator' accessor.  Dummy function.
    {
        return true;
    }
};

template <class TYPE>
struct AllocatorMatchesImp<TYPE, 1> {
    bool operator()(const TYPE&       object,
                    bslma::Allocator *alloc) const
        // The specified 'object' has an 'allocator' accessor.  Check that the
        // specified 'alloc' matches the allocator of 'object'.
    {
        bool ret;
        ASSERTV(NameOf<TYPE>(), (ret = alloc == object.allocator()));

        return ret;
    }
};

template <class TYPE>
bool allocatorMatches(const TYPE& object, bslma::Allocator *alloc)
{
    return AllocatorMatchesImp<TYPE>()(object, alloc);
}

template <class U, class V>
bool allocatorMatches(const bsl::pair<U, V>& object, bslma::Allocator *alloc)
{
    return AllocatorMatchesImp<U>()(object.first,  alloc)
        && AllocatorMatchesImp<V>()(object.second, alloc);
}

template <class TYPE>
struct IsMoveAware : bsl::integral_constant<
                      bool,
                      bsl::is_same<TYPE, bsltf::MovableTestType>::value      ||
                      bsl::is_same<TYPE, bsltf::MovableAllocTestType>::value ||
                      bsl::is_same<TYPE, bsltf::MoveOnlyAllocTestType>::value>
{};

template <class TYPE>
inline
bool isMoveAware(const TYPE&)
{
    return IsMoveAware<TYPE>::value;
}

template <class TYPE>
inline
bool isMovedFrom(const TYPE& object)
{
    return !isMoveAware(object) || MoveState::e_MOVED ==
                                                TTF::getMovedFromState(object);
}

template <class TYPE>
inline
bool isNotMovedFrom(const TYPE& object)
{
    return !isMoveAware(object) || MoveState::e_NOT_MOVED ==
                                                TTF::getMovedFromState(object);
}

template <class TYPE>
inline
bool isMovedInto(const TYPE& object)
{
    return !isMoveAware(object) || MoveState::e_MOVED ==
                                                TTF::getMovedIntoState(object);
}

template <class TYPE>
inline
bool isNotMovedInto(const TYPE& object)
{
    return !isMoveAware(object) || MoveState::e_NOT_MOVED ==
                                                TTF::getMovedIntoState(object);
}

template <class U, class V>
inline
bool isNotMovedFrom(const bsl::pair<U, V>& pr)
{
    return (!isMoveAware(pr.first) ||
            MoveState::e_NOT_MOVED ==TTF::getMovedFromState(pr.first))
       && (!isMoveAware(pr.second) ||
            MoveState::e_NOT_MOVED ==TTF::getMovedFromState(pr.second));
}

template <class U, class V>
inline
bool isNotMovedInto(const bsl::pair<U, V>& pr)
{
    return (!isMoveAware(pr.first) ||
            MoveState::e_NOT_MOVED ==TTF::getMovedIntoState(pr.first))
       && (!isMoveAware(pr.second) ||
            MoveState::e_NOT_MOVED ==TTF::getMovedIntoState(pr.second));
}

template <class PAIR>
inline
bool isNotMoved(const PAIR& pr)
{
    return isNotMovedInfo(pr) && isNotMovedFrom(pr);
}

template <class TYPE>
int valueOf(const TYPE& object)
{
    return TTF::getIdentifier(object);
}

template <class U, class V>
int valueOf(const bsl::pair<U, V>& pr)
    // Return the value of the specified pair 'pr'.  A scalar value of a pair
    // only makes sense if both members have correspondingsame values.  If they
    // don't, return -1 (an illegal value -- legal values are in '[ 0, 128 )'.
{
    const int f = valueOf<U>(pr.first);
    const int s = valueOf<V>(pr.second);

    return f + k_VALUE_SHIFT == s ? f : -1;
}

template <class U, class V>
int valueOf(const native_std::pair<U, V>& pr)
    // Return the value of the specified pair 'pr'.  A scalar value of a pair
    // only makes sense if both members have correspondingsame values.  If they
    // don't, return -1 (an illegal value -- legal values are in '[ 0, 128 )'.
{
    const int f = valueOf<U>(pr.first);
    const int s = valueOf<V>(pr.second);

    return f + k_VALUE_SHIFT == s ? f : -1;
}

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

template <>
int valueOf<Node>(const Node& node)
{
    return node.data();
}

#endif

template <class PAIR, class ALLOCATOR>
PAIR& initPair(bsls::ObjectBuffer<PAIR> *buffer,
               int                       value,
               ALLOCATOR                 alloc)
    // Note that the specified 'buffer' must be a pointer to an
    // 'bsls::ObjectBuffer<bsl::pair<U, V> >'.  Construct the 'first' and
    // 'second' fields of the pair in the 'buffer' according to the specified
    // 'value', and pass the specified allocator 'alloc' as appropriate.
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= value);  BSLS_ASSERT_SAFE(value < 128);

    TTF::emplace(bsls::Util::addressOf(buffer->object().first),
                 value,
                 alloc);
    TTF::emplace(bsls::Util::addressOf(buffer->object().second),
                 value + k_VALUE_SHIFT,
                 alloc);

    ASSERT(valueOf(buffer->object()) == value);

    return buffer->object();
}

template <class PAIR>
struct PairGuard {
    // DATA
    PAIR *d_pair_p;

  private:
    // NOT IMPLEMENTED
    PairGuard(const PairGuard&);
    PairGuard& operator=(const PairGuard&);

  public:
    // CREATORS
    explicit
    PairGuard(PAIR *pr)
    : d_pair_p(pr)
    {}

    ~PairGuard()
    {
        if (d_pair_p) {
            destroy();
        }
    }

    // MANIPULATORS
    void destroy()
    {
        d_pair_p->~PAIR();
    }

    void release()
    {
        d_pair_p = 0;
    }
};

template <class ETYPE,
          int   NUM_FIRST_ARGS,
          int   NF1,
          int   NF2,
          int   NF3,
          int   NUM_SECOND_ARGS,
          int   NS1,
          int   NS2,
          int   NS3>
struct DisplayType {
    // This 'struct' is just to be created to be passed to 'NameOf' to display
    // the template args.
};

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace bslma {
template <>
struct UsesBslmaAllocator<u::AlBase> : bsl::true_type {};
template <>
struct UsesBslmaAllocator<u::AlDerived> : bsl::true_type {};
}  // close namespace bslma
}  // close enterprise namespace

namespace bsl {

template <class FIRST, class SECOND>
inline
void debugprint(const bsl::pair<FIRST, SECOND>& p)
{
    printf("(");
    bsls::BslTestUtil::callDebugprint(static_cast<char>(
                                                TTF::getIdentifier(p.first)));
    printf(",");
    bsls::BslTestUtil::callDebugprint(static_cast<char>(
                                                TTF::getIdentifier(p.second)));
    printf(")");
}

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

inline
void debugprint(const u::Node& node)
{
    bsls::BslTestUtil::callDebugprint(static_cast<char>(node.data()));
}

#endif

inline
void debugprint(const u::Base& base)
{
    bsls::BslTestUtil::callDebugprint(static_cast<char>(
                                                    TTF::getIdentifier(base)));
}

}  // close namespace bsl

void testCase16()
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

    // N4594: 20.4: Pairs

    // pages 526-527: Class template pair
    //..
    //  pair& operator=(pair&& p) noexcept (see below);
    //  void swap(pair& p) noexcept (see below);
    //..

    {
        bsl::pair<int, long> x;
        bsl::pair<int, long> p;

        ASSERT(BSLS_CPP11_PROVISIONALLY_FALSE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(
                                          x = bslmf::MovableRefUtil::move(p)));

        ASSERT(BSLS_CPP11_PROVISIONALLY_FALSE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(x.swap(p)));
    }
}

                // ===========================================
                // class my_String (supplied by Usage example)
                // ===========================================

///Usage
///-----
// A 'bsl::pair' is a very simple object when used without allocators.  Our
// usage example concentrates on the use of allocators with 'bsl::pair'.
// First, we create a utility function that copies a null-terminated string
// into memory allocated from a supplied allocator.  The allocator can conform
// to either the 'bslma::Allocator' protocol or the STL allocator concept:
//..
    template <class ALLOC>
    char *myStrDup(const char *s, ALLOC *basicAllocator)
        // Copy the specified null-terminated string 's' into memory allocated
        // from the specified '*basicAllocator'
    {
        char *result = (char*) basicAllocator->allocate(strlen(s) + 1);
        return strcpy(result, s);
    }
//..
// We create a simple string class that holds strings allocated from a
// supplied allocator.  It uses 'myStrDup' (above) in its implementation:
//..
    class my_String {
        // Simple string class that uses a 'bslma::Allocator' allocator.

        bslma::Allocator *d_allocator_p;
        char             *d_data_p;

      public:
        BSLMF_NESTED_TRAIT_DECLARATION(my_String, bslma::UsesBslmaAllocator);

        explicit my_String(bslma::Allocator *alloc = 0);
            // Construct an empty string using the optionally specified
            // allocator 'alloc'.

        my_String(const char* s, bslma::Allocator *alloc = 0);      // IMPLICIT
            // Construct a string with contents specified in 's' using the
            // optionally-specified allocator 'alloc'.

        my_String(const my_String& original, bslma::Allocator *alloc = 0);
            // Construct a copy of the specified 'original' string using the
            // optionally specified allocator 'alloc'.

        ~my_String();
            // Destroy this string.

        my_String& operator=(const my_String& rhs);
            // Copy specified 'rhs' string value to this string.

        const char* c_str() const;
            // Return the null-terminated character array for this string.

        bslma::Allocator *allocator() const;
            // Return the allocator used to construct this string or, if no
            // allocator was specified at construction, the default allocator
            // at the time of construction.
    };

    bool operator==(const my_String& lhs, const my_String& rhs)
    {
        return 0 == strcmp(lhs.c_str(), rhs.c_str());
    }

    bool operator==(const my_String& lhs, const char *rhs)
    {
        return 0 == strcmp(rhs, lhs.c_str());
    }

    bool operator==(const char *lhs, const my_String& rhs)
    {
        return rhs == lhs;
    }

    bool operator!=(const my_String& lhs, const my_String& rhs)
    {
        return ! (lhs == rhs);
    }

    bool operator!=(const my_String& lhs, const char *rhs)
    {
        return ! (lhs == rhs);
    }

    bool operator!=(const char *lhs, const my_String& rhs)
    {
        return ! (lhs == rhs);
    }

    bool operator<(const my_String& lhs, const my_String& rhs)
    {
        return strcmp(lhs.c_str(), rhs.c_str()) < 0;
    }

    my_String::my_String(bslma::Allocator *alloc)
        : d_allocator_p(bslma::Default::allocator(alloc))
        , d_data_p(myStrDup("", d_allocator_p))
    {
    }

    my_String::my_String(const char *s, bslma::Allocator *alloc)
        : d_allocator_p(bslma::Default::allocator(alloc))
        , d_data_p(myStrDup(s, d_allocator_p))
    {
    }

    my_String::my_String(const my_String& original, bslma::Allocator *alloc)
        : d_allocator_p(bslma::Default::allocator(alloc))
        , d_data_p(myStrDup(original.d_data_p, d_allocator_p))
    {
    }

    my_String::~my_String()
    {
        d_allocator_p->deallocate(d_data_p);
    }

    my_String& my_String::operator=(const my_String& rhs)
    {
        if (this != &rhs) {
            d_allocator_p->deallocate(d_data_p);
            d_data_p = myStrDup(rhs.d_data_p, d_allocator_p);
        }
        return *this;
    }

    const char *my_String::c_str() const
    {
        return d_data_p;
    }

    bslma::Allocator *my_String::allocator() const
    {
        return d_allocator_p;
    }
//..
// Our main program creates a mapping from strings to integers.  Each node of
// the mapping consists of a 'bsl::pair<my_String, int>'.  The program
// allocates memory from a test allocator in order to ensure that there are no
// leaks:
//..
    int usageExample()
    {
        typedef bsl::pair<my_String, int> Node;

        Node *mapping[3];
        bslma::TestAllocator alloc;
//..
// When constructing a 'Node', an allocator is supplied in addition to
// parameters for the 'first' and 'second' data members.
//..
        {
            mapping[0] = new(alloc) Node("One", 1, &alloc);
            mapping[1] = new(alloc) Node("Three", 3, &alloc);
            mapping[2] = new(alloc) Node("Two", 2, &alloc);
            // Temporaries get destroyed here, even on broken compilers.
        }

        ASSERT("One" == mapping[0]->first);
        ASSERT(1 == mapping[0]->second);
        ASSERT("Three" == mapping[1]->first);
        ASSERT(3 == mapping[1]->second);
        ASSERT("Two" == mapping[2]->first);
        ASSERT(2 == mapping[2]->second);

        ASSERT(6 == alloc.numBlocksInUse());
//..
// Clean up at end.
//..
        alloc.deleteObjectRaw(mapping[0]);
        alloc.deleteObjectRaw(mapping[1]);
        alloc.deleteObjectRaw(mapping[2]);

        ASSERT(0 == alloc.numBlocksInUse());

        return 0;
    }
//..

                           // =======================
                           // class my_AllocArgString
                           // =======================

template <class ALLOC>
class my_AllocArgString
{
    // Another simple string class that uses a user-supplied STL-style
    // allocator that is provided to the constructor as the second argument,
    // following an 'bsl::allocator_arg_t' tag argument.  The behavior is
    // undefined unless 'ALLOC::value_type' is identical to 'char'.

    ALLOC  d_alloc;
    char  *d_data_p;

  public:
    typedef ALLOC allocator_type;

    my_AllocArgString();
    my_AllocArgString(const char* s);                               // IMPLICIT
    my_AllocArgString(const my_AllocArgString& original);
        // Construct a string without supplying an allocator.

    my_AllocArgString(bsl::allocator_arg_t, const ALLOC& a);
    my_AllocArgString(bsl::allocator_arg_t, const ALLOC& a, const char* s);
    my_AllocArgString(bsl::allocator_arg_t,
                      const ALLOC&             a,
                      const my_AllocArgString& string);
        // Construct an object the specified 'a' allocator, following the
        // 'allocator_arg_t' construction protocol.

    ~my_AllocArgString();
        // Destroy this string.

    my_AllocArgString& operator=(const my_AllocArgString& rhs);
        // Copy the specified 'rhs' to this string.

    size_t length() const;
        // Return the length of this string, excluding the null terminator.

    const char* c_str() const;
        // Return the null-terminated character array for this string. Never
        // returns a null pointer.

    allocator_type get_allocator() const;
        // Return the allocator used to construct this object.

    bslma::Allocator* allocator() const;
        // Return the bslma mechanism within the STL allocator.
};

template <class ALLOC>
bool operator==(const my_AllocArgString<ALLOC>& lhs,
                const my_AllocArgString<ALLOC>& rhs)
{
    return 0 == strcmp(lhs.c_str(), rhs.c_str());
}

template <class ALLOC>
bool operator==(const my_AllocArgString<ALLOC>& lhs, const char *rhs)
{
    return 0 == strcmp(rhs, lhs.c_str());
}

template <class ALLOC>
bool operator==(const char *lhs, const my_AllocArgString<ALLOC>& rhs)
{
    return rhs == lhs;
}

template <class ALLOC>
bool operator!=(const my_AllocArgString<ALLOC>& lhs,
                const my_AllocArgString<ALLOC>& rhs)
{
    return ! (lhs == rhs);
}

template <class ALLOC>
bool operator!=(const my_AllocArgString<ALLOC>& lhs, const char *rhs)
{
    return ! (lhs == rhs);
}

template <class ALLOC>
bool operator!=(const char *lhs, const my_AllocArgString<ALLOC>& rhs)
{
    return ! (rhs == lhs);
}

template <class ALLOC>
bool operator<(const my_AllocArgString<ALLOC>& lhs,
               const my_AllocArgString<ALLOC>& rhs)
{
    return strcmp(lhs.c_str(), rhs.c_str()) < 0;
}

template <class ALLOC>
my_AllocArgString<ALLOC>::my_AllocArgString()
    : d_alloc(), d_data_p(myStrDup("", &d_alloc))
{
    // class invariant: 'd_data_p' is not null
}

template <class ALLOC>
my_AllocArgString<ALLOC>::my_AllocArgString(const char *s)
    : d_alloc(), d_data_p(myStrDup(s, &d_alloc))
{
}

template <class ALLOC>
my_AllocArgString<ALLOC>::my_AllocArgString(
    const my_AllocArgString& original)
    : d_alloc(), d_data_p(myStrDup(original.c_str(), &d_alloc))
{
}

template <class ALLOC>
my_AllocArgString<ALLOC>::my_AllocArgString(bsl::allocator_arg_t,
                                                    const ALLOC& a)
    : d_alloc(a), d_data_p(myStrDup("", &d_alloc))
{
    // class invariant: 'd_data_p' is not null
}

template <class ALLOC>
my_AllocArgString<ALLOC>::my_AllocArgString(bsl::allocator_arg_t,
                                                    const ALLOC&  a,
                                                    const char   *s)
    : d_alloc(a), d_data_p(myStrDup(s, &d_alloc))
{
}

template <class ALLOC>
my_AllocArgString<ALLOC>::my_AllocArgString(
                                    bsl::allocator_arg_t,
                                    const ALLOC&                    a,
                                    const my_AllocArgString<ALLOC>& string)
    : d_alloc(a), d_data_p(myStrDup(string.c_str(), &d_alloc))
{
}

template <class ALLOC>
my_AllocArgString<ALLOC>::~my_AllocArgString()
{
    d_alloc.deallocate(d_data_p, length() + 1);
}

template <class ALLOC>
my_AllocArgString<ALLOC>&
my_AllocArgString<ALLOC>::operator=(const my_AllocArgString<ALLOC>& rhs)
{
    if (this != &rhs) {
        d_alloc.deallocate(d_data_p, length() + 1);
        d_data_p = myStrDup(rhs.c_str(), &d_alloc);
    }
    return *this;
}

template <class ALLOC>
bslma::Allocator *my_AllocArgString<ALLOC>::allocator() const
{
    return d_alloc.mechanism();
}

template <class ALLOC>
const char *my_AllocArgString<ALLOC>::c_str() const
{
    return d_data_p;
}

template <class ALLOC>
ALLOC my_AllocArgString<ALLOC>::get_allocator() const
{
    return d_alloc;
}

template <class ALLOC>
size_t my_AllocArgString<ALLOC>::length() const
{
    return strlen(d_data_p);
}

namespace BloombergLP {
namespace bslmf {
template <class ALLOC>
struct UsesAllocatorArgT<my_AllocArgString<ALLOC> > : bsl::true_type {};
}  // close namespace bslmf

namespace bslma {
template <class ALLOC>
struct UsesBslmaAllocator<my_AllocArgString<ALLOC> > :
        bslmf::IsConvertible<Allocator*, ALLOC>::type {};
}  // close namespace bslma
}  // close enterprise namespace

                           // =====================
                           // class my_STLCharAlloc
                           // =====================

class my_STLCharAlloc
{
    // STL-conforming allocator for 'char' elements.  Cannot be rebound to
    // allocate other types.

    bslma::Allocator *d_bslmaAlloc_p;

  public:
    typedef char    value_type;
    typedef char   *pointer_type;
    typedef size_t  size_type;

    static bslma::TestAllocator *defaultMechanism();

    my_STLCharAlloc();
    explicit my_STLCharAlloc(bslma::Allocator *bslmaAlloc_p);
        // Not convertible from 'bslma::Allocator*'.

    //! my_STLCharAlloc(const my_STLCharAlloc&) = default;
    //! ~my_STLCharAlloc() = default;
    //! my_STLCharAlloc& operator=(const my_STLCharAlloc&) = default;

    char *allocate(size_type n);
    void deallocate(char *p, size_type n);

    bslma::Allocator *mechanism() const;
};

bslma::TestAllocator *my_STLCharAlloc::defaultMechanism()
{
    static bslma::TestAllocator singleton(veryVeryVerbose);
    return &singleton;
}

inline
my_STLCharAlloc::my_STLCharAlloc()
    : d_bslmaAlloc_p(defaultMechanism())
{
}

inline
my_STLCharAlloc::my_STLCharAlloc(bslma::Allocator *bslmaAlloc_p)
    : d_bslmaAlloc_p(bslmaAlloc_p)
{
}

inline
char *my_STLCharAlloc::allocate(size_type n)
{
    return static_cast<char*>(d_bslmaAlloc_p->allocate(n));
}

inline
void my_STLCharAlloc::deallocate(char* p, size_type n)
{
    (void) n;    // silence unused warnings

    d_bslmaAlloc_p->deallocate(p);
}

inline
bslma::Allocator *my_STLCharAlloc::mechanism() const
{
    return d_bslmaAlloc_p;
}

typedef my_AllocArgString<my_STLCharAlloc>       my_STLAllocArgStr;
typedef my_AllocArgString<bsl::allocator<char> > my_BslmaAllocArgStr;

                           // ======================
                           // class my_NoAllocString
                           // ======================

class my_NoAllocString : public my_AllocArgString<bsl::allocator<char> >
{
    // Another simple string class that does not use a user-supplied
    // allocator.  All memory is allocated from a shared test allocator.  Will
    // compile and run if an attempt is made to use a constructor that takes
    // an allocator argument, but will report an assert failure.

    typedef my_AllocArgString<bsl::allocator<char> > Base;

  public:
    my_NoAllocString();
    my_NoAllocString(const char              *s);                   // IMPLICIT
    my_NoAllocString(const my_NoAllocString&  original);
        // Construct a string the normal way.

    explicit
    my_NoAllocString(bslma::Allocator        *alloc);
    my_NoAllocString(const char*              s,
                     bslma::Allocator        *alloc);
    my_NoAllocString(const my_NoAllocString&  original,
                     bslma::Allocator        *alloc);
        // Attempt to construct a string and specify a user-supplied allocator.
        // Reports an assert failure and ignores the specified 'alloc', but
        // otherwise compiles and runs.  These functions would be called if
        // bslstl_pair attempted to construct a 'my_NoAllocString' incorrectly.

    //! ~my_NoAllocString() = default;
    //! my_NoAllocString& operator=(const my_NoAllocString& rhs) = default;
};

my_NoAllocString::my_NoAllocString()
    : Base(bsl::allocator_arg, my_STLCharAlloc::defaultMechanism())
{
}

my_NoAllocString::my_NoAllocString(bslma::Allocator *alloc)
    : Base(bsl::allocator_arg, my_STLCharAlloc::defaultMechanism())
{
    (void) alloc;    // silence unused warnings

    ASSERT("Shouldn't get here" && 0);
}

my_NoAllocString::my_NoAllocString(const char *s)
    : Base(bsl::allocator_arg, my_STLCharAlloc::defaultMechanism(), s)
{
}

my_NoAllocString::my_NoAllocString(const char *s, bslma::Allocator *alloc)
    : Base(bsl::allocator_arg, my_STLCharAlloc::defaultMechanism(), s)
{
    (void) alloc;    // silence unused warnings

    ASSERT("Shouldn't get here" && 0);
}

my_NoAllocString::my_NoAllocString(const my_NoAllocString& original)
    : Base(bsl::allocator_arg, my_STLCharAlloc::defaultMechanism(), original)
{
}

my_NoAllocString::my_NoAllocString(const my_NoAllocString&  original,
                                   bslma::Allocator        *alloc)
    : Base(bsl::allocator_arg, my_STLCharAlloc::defaultMechanism(), original)
{
    (void) alloc;    // silence unused warnings

    ASSERT("Shouldn't get here" && 0);
}

                           // ============
                           // class Values
                           // ============

template <class TYPE, bool = bsl::is_integral<TYPE>::value>
struct Values
{
    // Namespace for sample values of non-integral 'TYPE'

    typedef TYPE Type;

    static Type null() { return ""; }
    static Type first() { return "Hello"; }
    static Type second() { return "World"; };
};

template <class TYPE>
struct Values<TYPE, true>
{
    // Namespace for sample values of integral 'TYPE'

    typedef TYPE Type;

    static Type null() { return 0; }
    static Type first() { return 1; }
    static Type second() { return 2; };
};


                           // =====================
                           // struct my_(Traits...)
                           // =====================

struct my_NonTrivialBaseClass {
    // C++11 compilers will detect trivial classes, including empty classes, by
    // default, so we establish a simple non-trivial class that can be used as
    // the base class for our testing types, which will have trivial traits
    // only if explicitly marked as trivial for the relevant 'bsl' trait.

    my_NonTrivialBaseClass(){}
    my_NonTrivialBaseClass(const my_NonTrivialBaseClass& original)
    {
        (void) original;    // suppress unused warnings
    }
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct my_MoveAbandonBslma : my_NonTrivialBaseClass {
};

namespace BloombergLP {
namespace bslmf {
template <>
struct IsBitwiseMoveable<my_MoveAbandonBslma> : bsl::true_type {};
}  // close namespace bslmf

namespace bslma {
template <>
struct UsesBslmaAllocator<my_MoveAbandonBslma> : bsl::true_type {};
}  // close namespace bslma
}  // close enterprise namespace

struct my_CopyTrivial : my_NonTrivialBaseClass {};

namespace bsl {
template <>
struct is_trivially_copyable<my_CopyTrivial> : bsl::true_type {};
template <>
struct is_trivially_default_constructible<my_CopyTrivial>
     : bsl::true_type {};
}  // close namespace bsl

struct my_EqualityTrivial : my_NonTrivialBaseClass {};

namespace bsl {
template <>
struct is_trivially_copyable<my_EqualityTrivial> : bsl::true_type {};
template <>
struct is_trivially_default_constructible<my_EqualityTrivial>
     : bsl::true_type {};
}  // close namespace bsl

struct my_NoTraits : my_NonTrivialBaseClass {};

namespace BloombergLP {
namespace bslmf {
template <>
struct IsBitwiseEqualityComparable<my_EqualityTrivial> : bsl::true_type {};

// Empty classes are bitwise moveable by default.  Specialize for 'my_NoTraits'
// to make it NOT bitwise moveable.
template<>
struct IsBitwiseMoveable<my_NoTraits> : bsl::false_type {};

}  // close namespace bslmf
}  // close enterprise namespace



//=============================================================================
//          HELPER CLASS TO TEST FORWARDING MANAGEDPTR TO CONSTRUCTORS
//-----------------------------------------------------------------------------

template <class TARGET>
class ManagedWrapper {
  private:
    void *d_ptr_p;

  public:
    ManagedWrapper(bslma::ManagedPtr<TARGET> source)                // IMPLICIT
        : d_ptr_p(source.get())
    {
    }

};

//=============================================================================
//              HELPER CLASSES AND FUNCTIONS FOR TESTING SWAP
//-----------------------------------------------------------------------------

                           // ===================
                           // struct TypeWithSwap
                           // ===================

namespace TypeWithSwapNamespace {

struct TypeWithSwap {
    int  d_data;
    bool d_swapCalled;

    explicit TypeWithSwap(int d)
    : d_data(d)
    , d_swapCalled(false)
    {}

    bool operator==(const TypeWithSwap& rhs) const
    {
        return d_data == rhs.d_data;
    }

    void swap(TypeWithSwap& other)
    {
        std::swap(d_data, other.d_data);

        // set the flag indicating that this function has been called
        other.d_swapCalled = d_swapCalled = true;
    }

    void assertSwapCalled() const
    {
        ASSERT(d_swapCalled);
    }
};

void swap(TypeWithSwap& a, TypeWithSwap& b)
{
    a.swap(b);
}
}  // close namespace TypeWithSwapNamespace

                           // ======================
                           // struct TypeWithoutSwap
                           // ======================

struct TypeWithoutSwap {
    int d_data;

    explicit TypeWithoutSwap(int d)
    : d_data(d)
    {}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
    // Nothrow moves needed so that std::swap doesn't get SFINAEd out.

    TypeWithoutSwap(const TypeWithoutSwap& original) noexcept = default;
    TypeWithoutSwap& operator=(const TypeWithoutSwap&) noexcept = default;
#endif

    bool operator==(const TypeWithoutSwap& rhs) const
    {
        return d_data == rhs.d_data;
    }

    void assertSwapCalled() const
    {
    }
};


template <class T1, class T2>
void swapTestHelper()
    // Test 'swap' method and free function for 'bsl::pair<T1, T2>'.
{
    typedef bsl::pair<T1, T2> test_pair;

    // construct two pairs
    test_pair orig_p1(T1(11), T2(12));
    test_pair orig_p2(T1(21), T2(22));

    // copy pairs so that originals remain unchanged
    test_pair p1(orig_p1);
    test_pair p2(orig_p2);

    // swap copies with the free function
    swap(p1, p2);

    // verify that 'swap' worked
    ASSERT(p1 == orig_p2);
    ASSERT(p2 == orig_p1);

    // verify that 'swap' member function was called
    p1.first.assertSwapCalled();
    p1.second.assertSwapCalled();
    p2.first.assertSwapCalled();
    p2.second.assertSwapCalled();

    // restore the original values
    p1 = orig_p1;
    p2 = orig_p2;

    // now use the 'swap' method
    p1.swap(p2);

    // verify that 'swap' worked
    ASSERT(p1 == orig_p2);
    ASSERT(p2 == orig_p1);

    // verify that 'swap' member function was called
    p1.first.assertSwapCalled();
    p1.second.assertSwapCalled();
    p2.first.assertSwapCalled();
    p2.second.assertSwapCalled();
}


                           // =================================
                           // Hash<my_String> (hashAppend test)
                           // =================================

// HASH SPECIALIZATIONS
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const my_String& input)
{
    using bslh::hashAppend;
    hashAlg(input.c_str(), strlen(input.c_str()));
}

template <class STRING>
void testBslmaStringConversionCtor()
{
    bslma::TestAllocator ta1(veryVeryVerbose);
    bslma::TestAllocator ta2(veryVeryVerbose);
    bslma::TestAllocator& ta3 = *my_STLCharAlloc::defaultMechanism();

    bslma::DefaultAllocatorGuard allocGuard(&ta2);

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<STRING,int>, no explicit allocator\n");
    {
        bsl::pair<const char*, short> p1("Hello", (short) 5), &P1 = p1;
        bsl::pair<STRING, int> p2(P1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT(5 == P2.second);
        ASSERT(&ta2 == P2.first.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(1 <= ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<STRING,int>, explicit allocator\n");
    {
        bsl::pair<const char*, short> p1("Hello", (short) 5), &P1 = p1;
        bsl::pair<STRING, int> p2(P1, &ta1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT(5 == P2.second);
        ASSERT(&ta1 == P2.first.allocator());
        ASSERT(1 <= ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<int,STRING>, no explicit allocator\n");
    {
        bsl::pair<short, const char*> p1((short) 5, "Hello"), &P1 = p1;
        bsl::pair<int, STRING> p2(P1), &P2 = p2;
        ASSERT(5 == P2.first);
        ASSERT("Hello" == P2.second);
        ASSERT(&ta2 == P2.second.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(1 <= ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<int,STRING>, explicit allocator\n");
    {
        bsl::pair<short, const char*> p1((short) 5, "Hello"), &P1 = p1;
        bsl::pair<int, STRING> p2(P1, &ta1), &P2 = p2;
        ASSERT(5 == P2.first);
        ASSERT("Hello" == P2.second);
        ASSERT(&ta1 == P2.second.allocator());
        ASSERT(1 <= ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<STRING,STRING>, no explicit alloc\n");
    {
        bsl::pair<const char*, const char*> p1("Hello", "World"), &P1 = p1;
        bsl::pair<STRING, STRING> p2(P1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT("World" == P2.second);
        ASSERT(&ta2 == P2.first.allocator());
        ASSERT(&ta2 == P2.second.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(2 <= ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<STRING,STRING>, explicit allocator\n");
    {
        bsl::pair<const char*, const char*> p1("Hello", "World"), &P1 = p1;
        bsl::pair<STRING, STRING> p2(P1, &ta1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT("World" == P2.second);
        ASSERT(&ta1 == P2.first.allocator());
        ASSERT(&ta1 == P2.second.allocator());
        ASSERT(2 <= ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVerbose) printf("Conversion from native 'std::pair'\n");

    if (veryVeryVerbose) printf("\tpair<STRING,int>, no explicit allocator\n");
    {
        native_std::pair<const char*, short> p1("Hello", 5), &P1 = p1;
        bsl::pair<STRING, int> p2(P1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT(5 == P2.second);
        ASSERT(&ta2 == P2.first.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(1 <= ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<STRING,int>, explicit allocator\n");
    {
        native_std::pair<const char*, short> p1("Hello", 5), &P1 = p1;
        bsl::pair<STRING, int> p2(P1, &ta1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT(5 == P2.second);
        ASSERT(&ta1 == P2.first.allocator());
        ASSERT(1 <= ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<int,STRING>, no explicit allocator\n");
    {
        bsl::pair<short, const char*> p1((short) 5, "Hello"), &P1 = p1;
        bsl::pair<int, STRING> p2(P1), &P2 = p2;
        ASSERT(5 == P2.first);
        ASSERT("Hello" == P2.second);
        ASSERT(&ta2 == P2.second.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(1 <= ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<int,STRING>, explicit allocator\n");
    {
        bsl::pair<short, const char*> p1((short) 5, "Hello"), &P1 = p1;
        bsl::pair<int, STRING> p2(P1, &ta1), &P2 = p2;
        ASSERT(5 == P2.first);
        ASSERT("Hello" == P2.second);
        ASSERT(&ta1 == P2.second.allocator());
        ASSERT(1 <= ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<STRING,STRING>, no explicit alloc\n");
    {
        native_std::pair<const char*, const char*> p1("Hello", "World"),
            &P1 = p1;
        bsl::pair<STRING, STRING> p2(P1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT("World" == P2.second);
        ASSERT(&ta2 == P2.first.allocator());
        ASSERT(&ta2 == P2.second.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(1 <= ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<STRING,STRING>, explicit allocator\n");
    {
        native_std::pair<const char*, const char*> p1("Hello", "World"),
            &P1 = p1;
        bsl::pair<STRING, STRING> p2(P1, &ta1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT("World" == P2.second);
        ASSERT(&ta1 == P2.first.allocator());
        ASSERT(&ta1 == P2.second.allocator());
        ASSERT(2 <= ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
}

template <class STRING>
void testNonBslmaStringConversionCtor()
{
    bslma::TestAllocator ta1(veryVeryVerbose);  // explicit allocator
    bslma::TestAllocator ta2(veryVeryVerbose);  // global default allocator
    // Default mechanism for 'my_STLCharAlloc'
    bslma::TestAllocator& ta3 = *my_STLCharAlloc::defaultMechanism();

    bslma::DefaultAllocatorGuard allocGuard(&ta2);

    bslma::TestAllocator *ta4_p;
    {
        STRING dummy;  // Default-constructed string with defaulted allocator
        // Default allocator when not supplied:
        ta4_p = dynamic_cast<bslma::TestAllocator *>(dummy.allocator());
    }
    bslma::TestAllocator& ta4 = *ta4_p; // 'STRING's default allocator

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
    ASSERT(0 == ta4.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<STRING,int>, no explicit allocator\n");
    {
        bsl::pair<const char*, short> p1("Hello", (short) 5), &P1 = p1;
        bsl::pair<STRING, int> p2(P1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT(5 == P2.second);
        ASSERT(&ta4 == P2.first.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(&ta4 == &ta2 || 0 == ta2.numBlocksInUse());
        ASSERT(&ta4 == &ta3 || 0 == ta3.numBlocksInUse());
        ASSERT(1 <= ta4.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
    ASSERT(0 == ta4.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<int,STRING>, no explicit allocator\n");
    {
        bsl::pair<short, const char*> p1((short) 5, "Hello"), &P1 = p1;
        bsl::pair<int, STRING> p2(P1), &P2 = p2;
        ASSERT(5 == P2.first);
        ASSERT("Hello" == P2.second);
        ASSERT(&ta4 == P2.second.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(&ta4 == &ta2 || 0 == ta2.numBlocksInUse());
        ASSERT(&ta4 == &ta3 || 0 == ta3.numBlocksInUse());
        ASSERT(1 <= ta4.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
    ASSERT(0 == ta4.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<STRING,STRING>, no explicit alloc\n");
    {
        bsl::pair<const char*, const char*> p1("Hello", "World"), &P1 = p1;
        bsl::pair<STRING, STRING> p2(P1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT("World" == P2.second);
        ASSERT(&ta4 == P2.first.allocator());
        ASSERT(&ta4 == P2.second.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(&ta4 == &ta2 || 0 == ta2.numBlocksInUse());
        ASSERT(&ta4 == &ta3 || 0 == ta3.numBlocksInUse());
        ASSERT(2 <= ta4.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
    ASSERT(0 == ta4.numBlocksInUse());

    if (veryVerbose) printf("Conversion from native 'std::pair'\n");

    if (veryVeryVerbose) printf("\tpair<STRING,int>, no explicit allocator\n");
    {
        native_std::pair<const char*, short> p1("Hello", (short) 5), &P1 = p1;
        bsl::pair<STRING, int> p2(P1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT(5 == P2.second);
        ASSERT(&ta4 == P2.first.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(&ta4 == &ta2 || 0 == ta2.numBlocksInUse());
        ASSERT(&ta4 == &ta3 || 0 == ta3.numBlocksInUse());
        ASSERT(1 <= ta4.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
    ASSERT(0 == ta4.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<int,STRING>, no explicit allocator\n");
    {
        bsl::pair<short, const char*> p1((short) 5, "Hello"), &P1 = p1;
        bsl::pair<int, STRING> p2(P1), &P2 = p2;
        ASSERT(5 == P2.first);
        ASSERT("Hello" == P2.second);
        ASSERT(&ta4 == P2.second.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(&ta4 == &ta2 || 0 == ta2.numBlocksInUse());
        ASSERT(&ta4 == &ta3 || 0 == ta3.numBlocksInUse());
        ASSERT(1 <= ta4.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
    ASSERT(0 == ta4.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<STRING,STRING>, no explicit alloc\n");
    {
        native_std::pair<const char*, const char*> p1("Hello", "World"),
            &P1 = p1;
        bsl::pair<STRING, STRING> p2(P1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT("World" == P2.second);
        ASSERT(&ta4 == P2.first.allocator());
        ASSERT(&ta4 == P2.second.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(&ta4 == &ta2 || 0 == ta2.numBlocksInUse());
        ASSERT(&ta4 == &ta3 || 0 == ta3.numBlocksInUse());
        ASSERT(2 <= ta4.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
    ASSERT(0 == ta4.numBlocksInUse());
}

template <class TYPE, bool = bslma::UsesBslmaAllocator<TYPE>::value>
struct AllocatorMatcher
{
    static bool match(const TYPE&, bslma::Allocator *) { return true; }
        // Return true (for types that don't take a bslma::Allocator).
};

template <class TYPE>
struct AllocatorMatcher<TYPE, true>
{
    static bool match(const TYPE& v, bslma::Allocator *a)
        // Return true if the specified 'v' was constructed with the specified
        // allocator 'a'; otherwise false.
        { return v.allocator() == a; }
};

template <class TYPE>
inline
bool matchAllocator(const TYPE& v, bslma::Allocator *a)
    // Return true if the specified 'v' was constructed with the specified
    // allocator 'a'; otherwise false.  If 'TYPE' is not constructed with a
    // 'bslma::Allocator', then always return true.
{
    return AllocatorMatcher<TYPE>::match(v, a);
}

template <class T1, class T2>
void testFunctionality(bsl::false_type usesBslmaAllocator)
    // Test functionality of 'bsl::pair<T1,T2>', using only constructors that
    // don't taken a 'bslma::Allocator*' argument.
{
    (void) usesBslmaAllocator;    // silence unused warnings

    typedef bsl::pair<T1, T2> Obj;
    ASSERT((bsl::is_same<T1, typename Obj::first_type>::value));
    ASSERT((bsl::is_same<T2, typename Obj::second_type>::value));

    const T1 NULL_FIRST   = Values<T1>::null();
    const T2 NULL_SECOND  = Values<T2>::null();
    const T1 VALUE_FIRST  = Values<T1>::first();
    const T2 VALUE_SECOND = Values<T2>::second();

    // Test traits
    ASSERT(bslmf::IsBitwiseMoveable<Obj>::value ==
           (bslmf::IsBitwiseMoveable<T1>::value &&
            bslmf::IsBitwiseMoveable<T2>::value));
    ASSERT(bsl::is_trivially_copyable<Obj>::value ==
           (bsl::is_trivially_copyable<T1>::value &&
            bsl::is_trivially_copyable<T2>::value));
    ASSERT(bsl::is_trivially_default_constructible<Obj>::value ==
           (bsl::is_trivially_default_constructible<T1>::value &&
            bsl::is_trivially_default_constructible<T2>::value));
    ASSERT(bslma::UsesBslmaAllocator<Obj>::value ==
           (bslma::UsesBslmaAllocator<T1>::value ||
            bslma::UsesBslmaAllocator<T2>::value));

    bslma::TestAllocator ta0(veryVeryVerbose);

    bslma::DefaultAllocatorGuard allocGuard(&ta0);

    ASSERT(0 == ta0.numBlocksInUse());

    {
        // Default construct using default allocator
        Obj p1; const Obj& P1 = p1;
        ASSERT(NULL_FIRST  == P1.first);
        ASSERT(NULL_SECOND == P1.second);
        ASSERT(matchAllocator(P1.first, &ta0));
        ASSERT(matchAllocator(P1.second, &ta0));

        // Construct with two values and default allocator
        Obj p2(VALUE_FIRST, VALUE_SECOND); const Obj& P2 = p2;
        ASSERT(VALUE_FIRST  == P2.first);
        ASSERT(VALUE_SECOND == P2.second);
        ASSERT(matchAllocator(P2.first, &ta0));
        ASSERT(matchAllocator(P2.second, &ta0));

        // Copy construct
        Obj p3(p2); const Obj& P3 = p3;
        ASSERT(VALUE_FIRST == P3.first);
        ASSERT(VALUE_SECOND == P3.second);
        ASSERT(matchAllocator(P3.first, &ta0));
        ASSERT(matchAllocator(P3.second, &ta0));

        // Test use of default allocator
        int minAllocations = 0;  // Allocations from default allocator
        if (bslma::UsesBslmaAllocator<T1>::value)
            minAllocations += 3;
        if (bslma::UsesBslmaAllocator<T2>::value)
            minAllocations += 3;
        ASSERT(minAllocations <= ta0.numBlocksInUse());

        // Test equality and relational operators
        ASSERT(P2 == P3);
        ASSERT(P1 != P2);
        ASSERT(P1 < P2);
        ASSERT(P2 > P1);
        ASSERT(P1 <= P2);
        ASSERT(P2 >= P1);
        ASSERT(P3 <= P2);
        ASSERT(P2 >= P3);

        ASSERT(! (P1 == P2));
        ASSERT(! (P3 != P2));
        ASSERT(! (P2 < P1));
        ASSERT(! (P1 > P2));
        ASSERT(! (P2 < P3));
        ASSERT(! (P3 > P2));
        ASSERT(! (P2 <= P1));
        ASSERT(! (P1 >= P2));

        // Test assignment
        p1 = P2;
        ASSERT(P1 == P2);
        ASSERT(matchAllocator(P1.first, &ta0));
        ASSERT(matchAllocator(P1.second, &ta0));
        ASSERT(minAllocations <= ta0.numBlocksInUse());
    }

    // Test the any memory allocated was deallocated
    ASSERT(0 == ta0.numBlocksInUse());
}

template <class T1, class T2>
void testFunctionality(bsl::true_type usesBslmaAllocator)
    // Test functionality of 'bsl::pair<T1,T2>', with and without
    // explicitly-supplied 'bslma::Allocator*' constructor arguments.
{
    (void) usesBslmaAllocator;    // silence unused warnings

    // Test without explicit allocator
    testFunctionality<T1,T2>(bsl::false_type());

    typedef bsl::pair<T1, T2> Obj;

    const T1 NULL_FIRST   = Values<T1>::null();
    const T2 NULL_SECOND  = Values<T2>::null();
    const T1 VALUE_FIRST  = Values<T1>::first();
    const T2 VALUE_SECOND = Values<T2>::second();

    bslma::TestAllocator ta0(veryVeryVerbose);
    bslma::TestAllocator ta1(veryVeryVerbose);
    bslma::TestAllocator ta2(veryVeryVerbose);
    bslma::TestAllocator ta3(veryVeryVerbose);

    bslma::DefaultAllocatorGuard allocGuard(&ta0);

    ASSERT(0 == ta0.numBlocksInUse());
    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    {
        // Compute number of allocations per constructor from supplied
        // allocator.
        int numAllocations = 0;  // Allocations from default allocator
        if (bslma::UsesBslmaAllocator<T1>::value)
            numAllocations += 1;
        if (bslma::UsesBslmaAllocator<T2>::value)
            numAllocations += 1;

        // Test default construction with allocator
        Obj p4(&ta1); const Obj& P4 = p4;
        ASSERT(NULL_FIRST  == P4.first);
        ASSERT(NULL_SECOND == P4.second);
        ASSERT(matchAllocator(P4.first, &ta1));
        ASSERT(matchAllocator(P4.second, &ta1));
        ASSERT(0 == ta0.numBlocksInUse());
        ASSERT(numAllocations <= ta1.numBlocksInUse());

        Obj p5(VALUE_FIRST, VALUE_SECOND, &ta2); const Obj& P5 = p5;
        ASSERT(VALUE_FIRST  == P5.first);
        ASSERT(VALUE_SECOND == P5.second);
        ASSERT(matchAllocator(P5.first, &ta2));
        ASSERT(matchAllocator(P5.second, &ta2));
        ASSERT(0 == ta0.numBlocksInUse());
        ASSERT(numAllocations <= ta2.numBlocksInUse());

        Obj p6(P5, &ta3); const Obj& P6 = p6;
        ASSERT(VALUE_FIRST == P6.first);
        ASSERT(VALUE_SECOND == P6.second);
        ASSERT(matchAllocator(P6.first, &ta3));
        ASSERT(matchAllocator(P6.second, &ta3));
        ASSERT(0 == ta0.numBlocksInUse());
        ASSERT(numAllocations <= ta3.numBlocksInUse());

        p4 = P5;
        ASSERT(P4 == P5);
        ASSERT(matchAllocator(P4.first, &ta1));
        ASSERT(matchAllocator(P4.second, &ta1));
        ASSERT(0 == ta0.numBlocksInUse());
    }

    ASSERT(0 == ta0.numBlocksInUse());
    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
class TupleTestDriver {
    // This 'class' is used for doing the tests with 'EmplacableTestType' and
    // 'AllocEmplacableTestType'.  For simplicity, we chose to implement them
    // as two functions rather than implementing by one with a template
    // parameter.

    // PRIVATE CLASS METHODS
    template <class T>
    static bslmf::MovableRef<T> testArg(T& t, bsl::true_type)
        // If the second argument is a 'true_type', return the argument moved.
    {
        return MoveUtil::move(t);
    }

    template <class T>
    static const T&             testArg(T& t, bsl::false_type)
        // If the second argument is a 'false_type', return a reference
        // providing modifiable access to the argument.
    {
        return t;
    }

    static void checkArgs(const char *displayName,
                          int         numFirstArgs,
                          int         nf1,
                          int         nf2,
                          int         nf3,
                          int         numSecondArgs,
                          int         ns1,
                          int         ns2,
                          int         ns3);
        // Passed as run time 'int's the template args to a 'runTest*' routine,
        // check for sanity, where the specified 'displayName' is the name of
        // the passed display type.  The thinking here is to shrink code size
        // by handling this in a single, non-inline, non-template routine
        // called by all the different template instantiations.

  public:
    // CLASS METHODS
    template <int NUM_FIRST_ARGS,
              int NF1,
              int NF2,
              int NF3,
              int NUM_SECOND_ARGS,
              int NS1,
              int NS2,
              int NS3>
    static void runTestAlloc();
        // Construct a pair of 'bsltf::AllocEmplacableType' with two tuples,
        // each taking 0-3 args.  The number of args for the first tuple is the
        // specified 'NUM_FIRST_ARGS', the number for the second is the
        // specified 'NUM_SECOND_ARGS'.  Interpret the values of 'NFi' and
        // 'NSi' as follows:
        //..
        //  NFi == 0 => forward the i'th 'first' argument using copy semantics
        //  NSi == 0 => forward the i'th 'second' argument using copy semantics
        //
        //  NFi == 1 => forward the i'th 'first' argument using move semantics
        //  NSi == 1 => forward the i'th 'second' argument using move semantics
        //
        //  NFi == 2 => don't forward a value for the i'th 'first' argument
        //  NSi == 2 => don't forward a value for the i'th 'second' argument
        //..
        // The behavior is undefined unless '0 <= NUM_FIRST_ARGS <= 3',
        // '0 <= NUM_SECOND_ARGS <= 3', all '[NF1 .. NF3]' and '[NS1 .. NS3]'
        // parameters are in the range '[0 .. 2]', '2 == NF2' if '2 == NF1',
        // '2 == NF3' if '2 == NF2', '2 == NS2' if '2 == NS1', and '2 == NS3'
        // if '2 == NS2'.

    template <int NUM_FIRST_ARGS,
              int NF1,
              int NF2,
              int NF3,
              int NUM_SECOND_ARGS,
              int NS1,
              int NS2,
              int NS3>
    static void runTestNoAlloc();
        // Construct one pair of 'bsltf::EmplacableType' with two tuples, each
        // taking 0-3 args.  The number of args for the first tuple is the
        // specified 'NUM_FIRST_ARGS', the number for the second is the
        // specified 'NUM_SECOND_ARGS'.  Interpret the values of 'NFi' and
        // 'NSi' as follows:
        //..
        //  NFi == 0 => forward the i'th 'first' argument using copy semantics
        //  NSi == 0 => forward the i'th 'second' argument using copy semantics
        //
        //  NFi == 1 => forward the i'th 'first' argument using move semantics
        //  NSi == 1 => forward the i'th 'second' argument using move semantics
        //
        //  NFi == 2 => don't forward a value for the i'th 'first' argument
        //  NSi == 2 => don't forward a value for the i'th 'second' argument
        //..
        // The behavior is undefined unless '0 <= NUM_FIRST_ARGS <= 3',
        // '0 <= NUM_SECOND_ARGS <= 3', all '[NF1 .. NF3]' and '[NS1 .. NS3]'
        // parameters are in the range '[0 .. 2]', '2 == NF2' if '2 == NF1',
        // '2 == NF3' if '2 == NF2', '2 == NS2' if '2 == NS1', and '2 == NS3'
        // if '2 == NS2'.
};

void TupleTestDriver::checkArgs(const char *displayName,
                                int         numFirstArgs,
                                int         nf1,
                                int         nf2,
                                int         nf3,
                                int         numSecondArgs,
                                int         ns1,
                                int         ns2,
                                int         ns3)
{
    ASSERTV(displayName, 0 <= numFirstArgs  && numFirstArgs  <= 3);
    ASSERTV(displayName, 0 <= numSecondArgs && numSecondArgs <= 3);

    ASSERTV(displayName, 0 <= nf1 && nf1 <= 2);
    ASSERTV(displayName, 0 <= nf2 && nf2 <= 2);
    ASSERTV(displayName, 0 <= nf3 && nf3 <= 2);

    ASSERTV(displayName, 0 <= ns1 && ns1 <= 2);
    ASSERTV(displayName, 0 <= ns2 && ns2 <= 2);
    ASSERTV(displayName, 0 <= ns3 && ns3 <= 2);

    ASSERTV(displayName, (numFirstArgs  < 3) == (2 == nf3));
    ASSERTV(displayName, (numFirstArgs  < 2) == (2 == nf2));
    ASSERTV(displayName, (numFirstArgs  < 1) == (2 == nf1));

    ASSERTV(displayName, (numSecondArgs < 3) == (2 == ns3));
    ASSERTV(displayName, (numSecondArgs < 2) == (2 == ns2));
    ASSERTV(displayName, (numSecondArgs < 1) == (2 == ns1));
}

template <int NUM_FIRST_ARGS,
          int NF1,
          int NF2,
          int NF3,
          int NUM_SECOND_ARGS,
          int NS1,
          int NS2,
          int NS3>
void TupleTestDriver::runTestAlloc()
{
    typedef bsltf::AllocEmplacableTestType                        EType;
    typedef bsl::pair<EType, EType>                               Pair;
    typedef u::DisplayType<EType, NUM_FIRST_ARGS,  NF1, NF2, NF3,
                                  NUM_SECOND_ARGS, NS1, NS2, NS3> DT;
    typedef typename EType::ArgType01                             Arg1;
    typedef typename EType::ArgType02                             Arg2;
    typedef typename EType::ArgType03                             Arg3;

    if (veryVerbose) printf("runTestAlloc(%d, %d,%d,%d, %d, %d,%d,%d);\n",
                             NUM_FIRST_ARGS,  NF1, NF2, NF3,
                             NUM_SECOND_ARGS, NS1, NS2, NS3);

    const char *name = NameOf<DT>();

    checkArgs(name, NUM_FIRST_ARGS,  NF1, NF2, NF3,
                    NUM_SECOND_ARGS, NS1, NS2, NS3);

    // In C++17, these become the simpler-to-name 'bool_constant'.

    static const bsl::integral_constant<bool, NF1 == 1> MOVE_F1 = {};
    static const bsl::integral_constant<bool, NF2 == 1> MOVE_F2 = {};
    static const bsl::integral_constant<bool, NF3 == 1> MOVE_F3 = {};
    static const bsl::integral_constant<bool, NS1 == 1> MOVE_S1 = {};
    static const bsl::integral_constant<bool, NS2 == 1> MOVE_S2 = {};
    static const bsl::integral_constant<bool, NS3 == 1> MOVE_S3 = {};

    bslma::TestAllocator aa("args",    veryVeryVeryVerbose);
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard daGuard(&da);

    bool silenceVeryVerbose = veryVeryVerbose;
#   define veryVerbose silenceVeryVerbose
    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(aa) {
#   undef  veryVerbose
        Arg1 AF1(1,  &aa);
        Arg2 AF2(20, &aa);
        Arg3 AF3(23, &aa);

        Arg1 AS1(2,  &aa);
        Arg2 AS2(18, &aa);
        Arg3 AS3(31, &aa);

        bsls::ObjectBuffer<Pair> oDst;
        Pair *p = oDst.address();

        switch (NUM_FIRST_ARGS) {
          case 0: {
            switch (NUM_SECOND_ARGS) {
              case 0: {
                new (p) Pair(native_std::piecewise_construct,
                             native_std::forward_as_tuple(),
                             native_std::forward_as_tuple(),
                             &aa);
              } break;
              case 1: {
                new (p) Pair(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(),
                          native_std::forward_as_tuple(testArg(AS1, MOVE_S1)),
                          &aa);
              } break;
              case 2: {
                new (p) Pair(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(),
                          native_std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                       testArg(AS2, MOVE_S2)),
                          &aa);
              } break;
              case 3: {
                new (p) Pair(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(),
                          native_std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                       testArg(AS2, MOVE_S2),
                                                       testArg(AS3, MOVE_S3)),
                          &aa);
              } break;
              default: {
                // Invalid number of arguments ('BSLMF_ASSERT'ed on entry).
              } return;                                               // RETURN
            }
          } break;
          case 1: {
            switch (NUM_SECOND_ARGS) {
              case 0: {
                new (p) Pair(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AF1, MOVE_F1)),
                          native_std::forward_as_tuple(),
                          &aa);
              } break;
              case 1: {
                new (p) Pair(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AF1, MOVE_F1)),
                          native_std::forward_as_tuple(testArg(AS1, MOVE_S1)),
                          &aa);
              } break;
              case 2: {
                new (p) Pair(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AF1, MOVE_F1)),
                          native_std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                       testArg(AS2, MOVE_S2)),
                          &aa);
              } break;
              case 3: {
                new (p) Pair(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AF1, MOVE_F1)),
                          native_std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                       testArg(AS2, MOVE_S2),
                                                       testArg(AS3, MOVE_S3)),
                          &aa);
              } break;
              default: {
                // Invalid number of arguments ('BSLMF_ASSERT'ed on entry).
              } return;                                               // RETURN
            }
          } break;
          case 2: {
            switch (NUM_SECOND_ARGS) {
              case 0: {
                new (p) Pair(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                       testArg(AF2, MOVE_F2)),
                          native_std::forward_as_tuple(),
                          &aa);
              } break;
              case 1: {
                new (p) Pair(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                       testArg(AF2, MOVE_F2)),
                          native_std::forward_as_tuple(testArg(AS1, MOVE_S1)),
                          &aa);
              } break;
              case 2: {
                new (p) Pair(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                       testArg(AF2, MOVE_F2)),
                          native_std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                       testArg(AS2, MOVE_S2)),
                          &aa);
              } break;
              case 3: {
                new (p) Pair(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                       testArg(AF2, MOVE_F2)),
                          native_std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                       testArg(AS2, MOVE_S2),
                                                       testArg(AS3, MOVE_S3)),
                          &aa);
              } break;
              default: {
                // Invalid number of arguments ('BSLMF_ASSERT'ed on entry).
              } return;                                               // RETURN
            }
          } break;
          case 3: {
            switch (NUM_SECOND_ARGS) {
              case 0: {
                new (p) Pair(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                       testArg(AF2, MOVE_F2),
                                                       testArg(AF3, MOVE_F3)),
                          native_std::forward_as_tuple(),
                          &aa);
              } break;
              case 1: {
                new (p) Pair(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                       testArg(AF2, MOVE_F2),
                                                       testArg(AF3, MOVE_F3)),
                          native_std::forward_as_tuple(testArg(AS1, MOVE_S1)),
                          &aa);
              } break;
              case 2: {
                new (p) Pair(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                       testArg(AF2, MOVE_F2),
                                                       testArg(AF3, MOVE_F3)),
                          native_std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                       testArg(AS2, MOVE_S2)),
                          &aa);
              } break;
              case 3: {
                new (p) Pair(
                          native_std::piecewise_construct,
                          native_std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                       testArg(AF2, MOVE_F2),
                                                       testArg(AF3, MOVE_F3)),
                          native_std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                       testArg(AS2, MOVE_S2),
                                                       testArg(AS3, MOVE_S3)),
                          &aa);
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
        u::PairGuard<Pair> pg(p);

        ASSERTV(name, MOVE_F1, AF1.movedFrom(),
               MOVE_F1 == (MoveState::e_MOVED == AF1.movedFrom()) || 2 == NF1);
        ASSERTV(name, MOVE_F2, AF2.movedFrom(),
               MOVE_F2 == (MoveState::e_MOVED == AF2.movedFrom()) || 2 == NF2);
        ASSERTV(name, MOVE_F3, AF3.movedFrom(),
               MOVE_F3 == (MoveState::e_MOVED == AF3.movedFrom()) || 2 == NF3);

        ASSERTV(name, MOVE_S1, AS1.movedFrom(),
               MOVE_S1 == (MoveState::e_MOVED == AS1.movedFrom()) || 2 == NS1);
        ASSERTV(name, MOVE_S2, AS2.movedFrom(),
               MOVE_S2 == (MoveState::e_MOVED == AS2.movedFrom()) || 2 == NS2);
        ASSERTV(name, MOVE_S3, AS3.movedFrom(),
               MOVE_S3 == (MoveState::e_MOVED == AS3.movedFrom()) || 2 == NS3);

        const EType& F = p->first;

        ASSERTV(name, 1  == F.arg01() || 2 == NF1);
        ASSERTV(name, 20 == F.arg02() || 2 == NF2);
        ASSERTV(name, 23 == F.arg03() || 2 == NF3);

        const EType& S = p->second;

        ASSERTV(name, 2  == S.arg01() || 2 == NS1);
        ASSERTV(name, 18 == S.arg02() || 2 == NS2);
        ASSERTV(name, 31 == S.arg03() || 2 == NS3);

        ASSERTV(name, u::allocatorMatches(F.arg01(), &aa));
        ASSERTV(name, u::allocatorMatches(F.arg02(), &aa));
        ASSERTV(name, u::allocatorMatches(F.arg03(), &aa));

        ASSERTV(name, u::allocatorMatches(S.arg01(), &aa));
        ASSERTV(name, u::allocatorMatches(S.arg02(), &aa));
        ASSERTV(name, u::allocatorMatches(S.arg03(), &aa));
    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

    const bool copyHappened = !NF1 || !NF2 || !NF3 || !NS1 || !NS2 || !NS3;

    ASSERTV(name, copyHappened, da.numAllocations(),
                                    copyHappened == (0 < da.numAllocations()));
}

template <int NUM_FIRST_ARGS,
          int NF1,
          int NF2,
          int NF3,
          int NUM_SECOND_ARGS,
          int NS1,
          int NS2,
          int NS3>
void TupleTestDriver::runTestNoAlloc()
{
    typedef bsltf::EmplacableTestType                             EType;
    typedef bsl::pair<EType, EType>                               Pair;
    typedef u::DisplayType<EType, NUM_FIRST_ARGS,  NF1, NF2, NF3,
                                  NUM_SECOND_ARGS, NS1, NS2, NS3> DT;
    typedef typename EType::ArgType01                             Arg1;
    typedef typename EType::ArgType02                             Arg2;
    typedef typename EType::ArgType03                             Arg3;

    if (veryVerbose) printf("runTestNoAlloc(%d, %d,%d,%d, %d, %d,%d,%d);\n",
                             NUM_FIRST_ARGS,  NF1, NF2, NF3,
                             NUM_SECOND_ARGS, NS1, NS2, NS3);

    const char *name = NameOf<DT>();

    checkArgs(name, NUM_FIRST_ARGS,  NF1, NF2, NF3,
                    NUM_SECOND_ARGS, NS1, NS2, NS3);

    // In C++17, these become the simpler-to-name 'bool_constant'.

    static const bsl::integral_constant<bool, NF1 == 1> MOVE_F1 = {};
    static const bsl::integral_constant<bool, NF2 == 1> MOVE_F2 = {};
    static const bsl::integral_constant<bool, NF3 == 1> MOVE_F3 = {};
    static const bsl::integral_constant<bool, NS1 == 1> MOVE_S1 = {};
    static const bsl::integral_constant<bool, NS2 == 1> MOVE_S2 = {};
    static const bsl::integral_constant<bool, NS3 == 1> MOVE_S3 = {};

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard daGuard(&da);

    Arg1 AF1(1);
    Arg2 AF2(20);
    Arg3 AF3(23);

    Arg1 AS1(2);
    Arg2 AS2(18);
    Arg3 AS3(31);

    bsls::ObjectBuffer<Pair> oDst;
    Pair *p = oDst.address();

    switch (NUM_FIRST_ARGS) {
      case 0: {
        switch (NUM_SECOND_ARGS) {
          case 0: {
            new (p) Pair(native_std::piecewise_construct,
                         native_std::forward_as_tuple(),
                         native_std::forward_as_tuple());
          } break;
          case 1: {
            new (p) Pair(native_std::piecewise_construct,
                         native_std::forward_as_tuple(),
                         native_std::forward_as_tuple(testArg(AS1, MOVE_S1)));
          } break;
          case 2: {
            new (p) Pair(native_std::piecewise_construct,
                         native_std::forward_as_tuple(),
                         native_std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                      testArg(AS2, MOVE_S2)));
          } break;
          case 3: {
            new (p) Pair(native_std::piecewise_construct,
                         native_std::forward_as_tuple(),
                         native_std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                      testArg(AS2, MOVE_S2),
                                                      testArg(AS3, MOVE_S3)));
          } break;
          default: {
            // Invalid number of arguments ('BSLMF_ASSERT'ed on entry).
          } return;                                                   // RETURN
        }
      } break;
      case 1: {
        switch (NUM_SECOND_ARGS) {
          case 0: {
            new (p) Pair(native_std::piecewise_construct,
                         native_std::forward_as_tuple(testArg(AF1, MOVE_F1)),
                         native_std::forward_as_tuple());
          } break;
          case 1: {
            new (p) Pair(native_std::piecewise_construct,
                         native_std::forward_as_tuple(testArg(AF1, MOVE_F1)),
                         native_std::forward_as_tuple(testArg(AS1, MOVE_S1)));
          } break;
          case 2: {
            new (p) Pair(native_std::piecewise_construct,
                         native_std::forward_as_tuple(testArg(AF1, MOVE_F1)),
                         native_std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                      testArg(AS2, MOVE_S2)));
          } break;
          case 3: {
            new (p) Pair(native_std::piecewise_construct,
                         native_std::forward_as_tuple(testArg(AF1, MOVE_F1)),
                         native_std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                      testArg(AS2, MOVE_S2),
                                                      testArg(AS3, MOVE_S3)));
          } break;
          default: {
            // Invalid number of arguments ('BSLMF_ASSERT'ed on entry).
          } return;                                                   // RETURN
        }
      } break;
      case 2: {
        switch (NUM_SECOND_ARGS) {
          case 0: {
            new (p) Pair(native_std::piecewise_construct,
                         native_std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                      testArg(AF2, MOVE_F2)),
                         native_std::forward_as_tuple());
          } break;
          case 1: {
            new (p) Pair(native_std::piecewise_construct,
                         native_std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                      testArg(AF2, MOVE_F2)),
                         native_std::forward_as_tuple(testArg(AS1, MOVE_S1)));
          } break;
          case 2: {
            new (p) Pair(native_std::piecewise_construct,
                         native_std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                      testArg(AF2, MOVE_F2)),
                         native_std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                      testArg(AS2, MOVE_S2)));
          } break;
          case 3: {
            new (p) Pair(native_std::piecewise_construct,
                         native_std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                      testArg(AF2, MOVE_F2)),
                         native_std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                      testArg(AS2, MOVE_S2),
                                                      testArg(AS3, MOVE_S3)));
          } break;
          default: {
            // Invalid number of arguments ('BSLMF_ASSERT'ed on entry).
          } return;                                                   // RETURN
        }
      } break;
      case 3: {
        switch (NUM_SECOND_ARGS) {
          case 0: {
            new (p) Pair(native_std::piecewise_construct,
                         native_std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                      testArg(AF2, MOVE_F2),
                                                      testArg(AF3, MOVE_F3)),
                         native_std::forward_as_tuple());
          } break;
          case 1: {
            new (p) Pair(native_std::piecewise_construct,
                         native_std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                      testArg(AF2, MOVE_F2),
                                                      testArg(AF3, MOVE_F3)),
                         native_std::forward_as_tuple(testArg(AS1, MOVE_S1)));
          } break;
          case 2: {
            new (p) Pair(native_std::piecewise_construct,
                         native_std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                      testArg(AF2, MOVE_F2),
                                                      testArg(AF3, MOVE_F3)),
                         native_std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                      testArg(AS2, MOVE_S2)));
          } break;
          case 3: {
            new (p) Pair(native_std::piecewise_construct,
                         native_std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                      testArg(AF2, MOVE_F2),
                                                      testArg(AF3, MOVE_F3)),
                         native_std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                      testArg(AS2, MOVE_S2),
                                                      testArg(AS3, MOVE_S3)));
          } break;
          default: {
            // Invalid number of arguments ('BSLMF_ASSERT'ed on entry).
          } return;                                                   // RETURN
        }
      } break;
      default: {
        // Invalid number of arguments ('BSLMF_ASSERT'ed on entry).
      } return;                                                       // RETURN
    }
    u::PairGuard<Pair> pg(p);

    ASSERTV(name, MOVE_F1, AF1.movedFrom(),
            MOVE_F1 == (MoveState::e_MOVED == AF1.movedFrom()) || 2 == NF1);
    ASSERTV(name, MOVE_F2, AF2.movedFrom(),
            MOVE_F2 == (MoveState::e_MOVED == AF2.movedFrom()) || 2 == NF2);
    ASSERTV(name, MOVE_F3, AF3.movedFrom(),
            MOVE_F3 == (MoveState::e_MOVED == AF3.movedFrom()) || 2 == NF3);

    ASSERTV(name, MOVE_S1, AS1.movedFrom(),
            MOVE_S1 == (MoveState::e_MOVED == AS1.movedFrom()) || 2 == NS1);
    ASSERTV(name, MOVE_S2, AS2.movedFrom(),
            MOVE_S2 == (MoveState::e_MOVED == AS2.movedFrom()) || 2 == NS2);
    ASSERTV(name, MOVE_S3, AS3.movedFrom(),
            MOVE_S3 == (MoveState::e_MOVED == AS3.movedFrom()) || 2 == NS3);

    const EType& F = p->first;

    ASSERTV(name, 1  == F.arg01() || 2 == NF1);
    ASSERTV(name, 20 == F.arg02() || 2 == NF2);
    ASSERTV(name, 23 == F.arg03() || 2 == NF3);

    const EType& S = p->second;

    ASSERTV(name, 2  == S.arg01() || 2 == NS1);
    ASSERTV(name, 18 == S.arg02() || 2 == NS2);
    ASSERTV(name, 31 == S.arg03() || 2 == NS3);

    ASSERTV(name, da.numAllocations(), 0 == da.numAllocations());
}
#endif

template <class TO_FIRST, class TO_SECOND, class FROM_FIRST, class FROM_SECOND>
class TestDriver {
    // PRIVATE TYPES
    typedef TO_FIRST                         ToFirst;
    typedef TO_SECOND                        ToSecond;

    typedef FROM_FIRST                       FromFirst;
    typedef FROM_SECOND                      FromSecond;

    typedef bsl::pair<ToFirst,   ToSecond>   ToPair;
    typedef bsl::pair<FromFirst, FromSecond> FromPair;

    // 'ToPair' and 'FromPair' are two types of 'bsl::pair's, sometimes
    // identical, sometimes different, but always such that the c'tor
    // 'ToPair(FromPair&&)' and the assignment operator
    // 'ToPair::operator=(FromPair&&)' are supported.

    typedef bsl::integral_constant<
                       bool,
                       bslma::UsesBslmaAllocator<ToFirst   >::value ||
                       bslma::UsesBslmaAllocator<ToSecond  >::value ||
                       bslma::UsesBslmaAllocator<FromFirst >::value ||
                       bslma::UsesBslmaAllocator<FromSecond>::value> UsesBslma;

    enum TestDriverBools {
                      k_ALLOC_1 = bslma::UsesBslmaAllocator<ToFirst>::value,
                      k_ALLOC_2 = bslma::UsesBslmaAllocator<ToSecond>::value,

                      k_ALLOC = k_ALLOC_1 || k_ALLOC_2,

                      k_MOVE_1 = u::IsMoveAware<ToFirst>::value,
                      k_MOVE_2 = u::IsMoveAware<ToSecond>::value };

                      // Note that the memory allocation property of 'ToFirst'
                      // matches that of 'FromFirst', and that of 'ToSecond'
                      // matches that of 'FromSecond', and the same goes for
                      // the move awareness property.

    // Organization of functions: It turns out that the c'tor for 'pair' only
    // takes an allocator argument if the members of 'pair' allocate memory,
    // so we have to write 2 versions of every test, one to call the c'tors
    // with an allocator and one to call them without.
    //
    // So we do 3 functions for each test:
    //: 1 one, passed a 'false_type, which expects nothing in the pairs to
    //:   allocate
    //: 2 one, passed a 'true_type', which expects the pairs to
    //:   allocate, and
    //: 3 one, passed no argument, which determines through template logic
    //:   which of the other two functions needs to be called.
    // Note that the types of the 2 pairs are set up so that if one of them
    // allocates, both do.

  public:
    // MANIPULATORS
    static void testCase14();
        // Test constructor from 'native_std::pair' in contexts with nested
        // pairs (reproducing / testing the fix for a known bug).

    static void testCase12_copy();
    static void testCase12_move();
        // Test move and copy assignment.  These must be in separate functions
        // because we want to test move assignment with 'MoveonlyTestType' but
        // copy assignment will fail to compile for that type.

    static void testCase11(bsl::false_type pairAllocates);
    static void testCase11(bsl::true_type  pairAllocates);
    static void testCase11();
        // Testing constructors where one element is moved and the other
        // copied.  Also, testing both elements copied, and testing the whole
        // pair copied as a unit.  Must be separate from TC 10 as this cannot
        // handle the move only type.

    static void testCase10(bsl::false_type pairAllocates);
    static void testCase10(bsl::true_type  pairAllocates);
    static void testCase10();
        // Testing move constructors moving the two elements of the pair into
        // the c'tor separately.

    static void testCase9(bsl::false_type pairAllocates);
    static void testCase9(bsl::true_type  pairAllocates);
    static void testCase9();
        // Testing pair to pair move c'tors.
};

template <class TO_FIRST, class TO_SECOND, class FROM_FIRST, class FROM_SECOND>
void TestDriver<TO_FIRST, TO_SECOND, FROM_FIRST, FROM_SECOND>::testCase14()
{
    if (veryVerbose) printf("TD<%s, %s>::case14, %s\n",
                            NameOf<ToFirst>().name(),
                            NameOf<ToSecond>().name(),
                            k_ALLOC ? "alloc" : "no alloc");

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    bslma::TestAllocator tb(veryVeryVeryVerbose);
    bslma::TestAllocator da(veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard daGuard(&da);

    Int64 taSoFar = 0, tbSoFar = 0, daSoFar = 0;

    for (int bb = 0; bb < 2; ++bb) {
        const bool b = bb;

        // pair in 'first' element

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            bsl::pair<FromPair, bool> tp(native_std::make_pair(fp, b), &tb);
            ASSERT(b == tp.second);
            ASSERT('F'                    == u::valueOf(tp.first.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.first.second));

            ASSERT(u::allocatorMatches(tp.first.first,  &tb));
            ASSERT(u::allocatorMatches(tp.first.second, &tb));
        }

        if (k_ALLOC) {
            ASSERT(taSoFar < ta.numAllocations());
            ASSERT(tbSoFar < tb.numAllocations());
            ASSERT(daSoFar < da.numAllocations());
            taSoFar = ta.numAllocations();
            tbSoFar = tb.numAllocations();
            daSoFar = da.numAllocations();
        }

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            bsl::pair<ToPair, bool> tp(native_std::make_pair(fp, b), &tb);
            ASSERT(b == tp.second);
            ASSERT('F'                    == u::valueOf(tp.first.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.first.second));

            ASSERT(u::allocatorMatches(tp.first.first,  &tb));
            ASSERT(u::allocatorMatches(tp.first.second, &tb));
        }

        if (k_ALLOC) {
            ASSERT(taSoFar < ta.numAllocations());
            ASSERT(tbSoFar < tb.numAllocations());
            ASSERT(daSoFar < da.numAllocations());
            taSoFar = ta.numAllocations();
            tbSoFar = tb.numAllocations();
            daSoFar = da.numAllocations();
        }

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            native_std::pair<FromPair, bool> np(fp, b);

            bsl::pair<FromPair, bool> tp(np, &tb);
            ASSERT(b == tp.second);
            ASSERT('F'                    == u::valueOf(tp.first.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.first.second));

            ASSERT(u::allocatorMatches(tp.first.first,  &tb));
            ASSERT(u::allocatorMatches(tp.first.second, &tb));
        }

        if (k_ALLOC) {
            ASSERT(taSoFar < ta.numAllocations());
            ASSERT(tbSoFar < tb.numAllocations());
            ASSERT(daSoFar < da.numAllocations());
            taSoFar = ta.numAllocations();
            tbSoFar = tb.numAllocations();
            daSoFar = da.numAllocations();
        }

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            native_std::pair<FromPair, bool> np(fp, b);

            bsl::pair<ToPair, bool> tp(np, &tb);
            ASSERT(b == tp.second);
            ASSERT('F'                    == u::valueOf(tp.first.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.first.second));

            ASSERT(u::allocatorMatches(tp.first.first,  &tb));
            ASSERT(u::allocatorMatches(tp.first.second, &tb));
        }

        if (k_ALLOC) {
            ASSERT(taSoFar < ta.numAllocations());
            ASSERT(tbSoFar < tb.numAllocations());
            ASSERT(daSoFar < da.numAllocations());
            taSoFar = ta.numAllocations();
            tbSoFar = tb.numAllocations();
            daSoFar = da.numAllocations();
        }

        // pair in 'second' element

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            bsl::pair<bool, FromPair> tp(native_std::make_pair(b, fp), &tb);
            ASSERT(b == tp.first);
            ASSERT('F'                    == u::valueOf(tp.second.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.second.second));

            ASSERT(u::allocatorMatches(tp.second.first,  &tb));
            ASSERT(u::allocatorMatches(tp.second.second, &tb));
        }

        if (k_ALLOC) {
            ASSERT(taSoFar < ta.numAllocations());
            ASSERT(tbSoFar < tb.numAllocations());
            ASSERT(daSoFar < da.numAllocations());
            taSoFar = ta.numAllocations();
            tbSoFar = tb.numAllocations();
            daSoFar = da.numAllocations();
        }

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            bsl::pair<bool, ToPair> tp(native_std::make_pair(b, fp), &tb);
            ASSERT(b == tp.first);
            ASSERT('F'                    == u::valueOf(tp.second.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.second.second));

            ASSERT(u::allocatorMatches(tp.second.first,  &tb));
            ASSERT(u::allocatorMatches(tp.second.second, &tb));
        }

        if (k_ALLOC) {
            ASSERT(taSoFar < ta.numAllocations());
            ASSERT(tbSoFar < tb.numAllocations());
            ASSERT(daSoFar < da.numAllocations());
            taSoFar = ta.numAllocations();
            tbSoFar = tb.numAllocations();
            daSoFar = da.numAllocations();
        }

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            native_std::pair<bool, FromPair> np(b, fp);

            bsl::pair<bool, FromPair> tp(np, &tb);
            ASSERT(b == tp.first);
            ASSERT('F'                    == u::valueOf(tp.second.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.second.second));

            ASSERT(u::allocatorMatches(tp.second.first,  &tb));
            ASSERT(u::allocatorMatches(tp.second.second, &tb));
        }

        if (k_ALLOC) {
            ASSERT(taSoFar < ta.numAllocations());
            ASSERT(tbSoFar < tb.numAllocations());
            ASSERT(daSoFar < da.numAllocations());
            taSoFar = ta.numAllocations();
            tbSoFar = tb.numAllocations();
            daSoFar = da.numAllocations();
        }

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            native_std::pair<bool, FromPair> np(b, fp);

            bsl::pair<bool, ToPair> tp(np, &tb);
            ASSERT(b == tp.first);
            ASSERT('F'                    == u::valueOf(tp.second.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.second.second));

            ASSERT(u::allocatorMatches(tp.second.first,  &tb));
            ASSERT(u::allocatorMatches(tp.second.second, &tb));
        }

        if (k_ALLOC) {
            ASSERT(taSoFar < ta.numAllocations());
            ASSERT(tbSoFar < tb.numAllocations());
            ASSERT(daSoFar < da.numAllocations());
            taSoFar = ta.numAllocations();
            tbSoFar = tb.numAllocations();
            daSoFar = da.numAllocations();
        }
    }

    if (!k_ALLOC) {
        ASSERT(0 == ta.numAllocations());
        ASSERT(0 == tb.numAllocations());
        ASSERT(0 == da.numAllocations());
    }
}

template <class TO_FIRST, class TO_SECOND, class FROM_FIRST, class FROM_SECOND>
void TestDriver<TO_FIRST, TO_SECOND, FROM_FIRST, FROM_SECOND>::
                                                              testCase12_copy()
{
    if (veryVerbose) printf("TD<%s, %s>::case12_copy, %s\n",
                            NameOf<ToFirst>().name(),
                            NameOf<ToSecond>().name(),
                            k_ALLOC ? "alloc" : "no alloc");

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    bslma::TestAllocator tb(veryVeryVeryVerbose);
    bslma::TestAllocator da(veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard daGuard(&da);

    // copy with matching allocators

    {
        bsls::ObjectBuffer<ToPair> otp;
        ToPair& tp = u::initPair(&otp, 'A', &ta);
        u::PairGuard<ToPair> tpg(&tp);

        ASSERT(u::isNotMovedInto(tp));
        ASSERT(u::isNotMovedFrom(tp));
        ASSERT('A' == u::valueOf(tp));

        ASSERT(u::allocatorMatches(tp.first,  &ta));
        ASSERT(u::allocatorMatches(tp.second, &ta));

        bsls::ObjectBuffer<FromPair> ofp;
        FromPair& fp = u::initPair(&ofp, 'F', &ta);    const FromPair& FP = fp;
        u::PairGuard<FromPair> fpg(&fp);

        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isNotMovedFrom(fp));
        ASSERT('F' == u::valueOf(fp));

        ASSERT(u::allocatorMatches(fp.first,  &ta));
        ASSERT(u::allocatorMatches(fp.second, &ta));

        ToPair *tpp = &(tp = FP);

        ASSERT(&tp == tpp);
        ASSERT('F' == u::valueOf(tp));

        ASSERT(u::isNotMovedInto(tp));
        ASSERT(u::isNotMovedFrom(tp));

        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isNotMovedFrom(fp));

        ASSERT(k_ALLOC == (0 < ta.numAllocations()));
    }

    Int64 taFirst = ta.numAllocations();

    // copy with non-matching allocators

    {
        bsls::ObjectBuffer<ToPair> otp;
        ToPair& tp = u::initPair(&otp, 'B', &ta);
        u::PairGuard<ToPair> tpg(&tp);

        ASSERT(u::isNotMovedInto(tp));
        ASSERT(u::isNotMovedFrom(tp));
        ASSERT('B' == u::valueOf(tp));

        ASSERT(u::allocatorMatches(tp.first,  &ta));
        ASSERT(u::allocatorMatches(tp.second, &ta));

        bsls::ObjectBuffer<FromPair> ofp;
        FromPair& fp = u::initPair(&ofp, 'H', &tb);    const FromPair& FP = fp;
        u::PairGuard<FromPair> fpg(&fp);

        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isNotMovedFrom(fp));
        ASSERT('H' == u::valueOf(fp));

        ASSERT(u::allocatorMatches(fp.first,  &tb));
        ASSERT(u::allocatorMatches(fp.second, &tb));

        ToPair *tpp = &(tp = FP);

        ASSERT(&tp == tpp);
        ASSERT('H' == u::valueOf(tp));

        ASSERT(u::isNotMovedInto(tp));
        ASSERT(u::isNotMovedFrom(tp));

        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isNotMovedFrom(fp));
    }

    ASSERT(k_ALLOC == (0 < ta.numAllocations() - taFirst));
    ASSERT(k_ALLOC == (0 < tb.numAllocations()));

    ASSERT(0 == da.numAllocations());
}

template <class TO_FIRST, class TO_SECOND, class FROM_FIRST, class FROM_SECOND>
void TestDriver<TO_FIRST, TO_SECOND, FROM_FIRST, FROM_SECOND>::
                                                              testCase12_move()
{
    if (veryVerbose) printf("TD<%s, %s>::case12_move, %s\n",
                            NameOf<ToFirst>().name(),
                            NameOf<ToSecond>().name(),
                            k_ALLOC ? "alloc" : "no alloc");

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    bslma::TestAllocator tb(veryVeryVeryVerbose);
    bslma::TestAllocator da(veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard daGuard(&da);

    // matching allocators

    {
        bsls::ObjectBuffer<ToPair> otp;
        ToPair& tp = u::initPair(&otp, 'A', &ta);
        u::PairGuard<ToPair> tpg(&tp);

        ASSERT(u::isNotMovedInto(tp));
        ASSERT(u::isNotMovedFrom(tp));
        ASSERT('A' == u::valueOf(tp));

        ASSERT(u::allocatorMatches(tp.first,  &ta));
        ASSERT(u::allocatorMatches(tp.second, &ta));

        bsls::ObjectBuffer<FromPair> ofp;
        FromPair& fp = u::initPair(&ofp, 'F', &ta);
        u::PairGuard<FromPair> fpg(&fp);

        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isNotMovedFrom(fp));
        ASSERT('F' == u::valueOf(fp));

        ASSERT(u::allocatorMatches(fp.first,  &ta));
        ASSERT(u::allocatorMatches(fp.second, &ta));

        ToPair *tpp = &(tp = MoveUtil::move(fp));

        ASSERT(&tp == tpp);
        ASSERT('F' == u::valueOf(tp));

        ASSERT(u::isMovedInto(tp));
        ASSERT(u::isNotMovedFrom(tp));

        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isMovedFrom(fp));

        ASSERT(k_ALLOC == (0 < ta.numAllocations()));
    }

    const Int64 taFirst = ta.numAllocations();

    // non-matching allocators

    if (k_ALLOC) {
        bsls::ObjectBuffer<ToPair> otp;
        ToPair& tp = u::initPair(&otp, 'B', &ta);
        u::PairGuard<ToPair> tpg(&tp);

        ASSERT(u::isNotMovedInto(tp));
        ASSERT(u::isNotMovedFrom(tp));
        ASSERT('B' == u::valueOf(tp));

        ASSERT(u::allocatorMatches(tp.first,  &ta));
        ASSERT(u::allocatorMatches(tp.second, &ta));

        bsls::ObjectBuffer<FromPair> ofp;
        FromPair& fp = u::initPair(&ofp, 'H', &tb);
        u::PairGuard<FromPair> fpg(&fp);

        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isNotMovedFrom(fp));
        ASSERT('H' == u::valueOf(fp));

        ASSERT(u::allocatorMatches(fp.first,  &tb));
        ASSERT(u::allocatorMatches(fp.second, &tb));

        ToPair *tpp = &(tp = MoveUtil::move(fp));

        ASSERT(&tp == tpp);
        ASSERT('H' == u::valueOf(tp));

        ASSERT(u::isMovedInto(tp));
        ASSERT(u::isNotMovedFrom(tp));

        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isMovedFrom(fp));
    }

    ASSERT(k_ALLOC == (0 < ta.numAllocations() - taFirst));
    ASSERT(k_ALLOC == (0 < tb.numAllocations()));

    ASSERT(0 == da.numAllocations());
}

template <class TO_FIRST, class TO_SECOND, class FROM_FIRST, class FROM_SECOND>
void TestDriver<TO_FIRST, TO_SECOND, FROM_FIRST, FROM_SECOND>::testCase11(
                                                               bsl::false_type)
{
    if (veryVerbose) printf("TD<%s, %s>::case11, no alloc\n",
                            NameOf<ToFirst>().name(),
                            NameOf<ToSecond>().name());

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    bslma::TestAllocator da(veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard daGuard(&da);

    for (int useDa = 0; useDa < 2; ++useDa) {
        bslma::TestAllocator& TA = useDa ? da : ta;

        bsls::ObjectBuffer<FromPair> ofp;
        FromPair& fp = u::initPair(&ofp, 'A', &TA);    const FromPair& FP = fp;
        u::PairGuard<FromPair> fpg(&fp);

        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isNotMovedFrom(fp));
        ASSERT('A' == u::valueOf(fp));

        const ToPair tp(FP.first, MoveUtil::move(fp.second));

        ASSERT(u::isNotMovedFrom(fp.first));
        ASSERT(u::isNotMovedInto(fp.first));
        ASSERT(u::isMovedFrom(   fp.second));
        ASSERT(u::isNotMovedInto(fp.second));
        ASSERT(u::isNotMovedInto(tp.first));

        ASSERT(u::isNotMovedFrom(tp.first));
        ASSERT(u::isMovedInto(   tp.second));
        ASSERT(u::isNotMovedFrom(tp.second));

        ASSERT('A' == u::valueOf(tp));

        fpg.destroy();
        u::initPair(&ofp, 'B', &TA);
        ASSERT('B' == u::valueOf(fp));

        const ToPair tpa(MoveUtil::move(fp.first), FP.second);

        ASSERT(u::isMovedFrom(   fp.first));
        ASSERT(u::isNotMovedInto(fp.first));
        ASSERT(u::isNotMovedFrom(fp.second));
        ASSERT(u::isNotMovedInto(fp.second));
        ASSERT(u::isMovedInto(   tpa.first));
        ASSERT(u::isNotMovedFrom(tpa.first));
        ASSERT(u::isNotMovedInto(tpa.second));
        ASSERT(u::isNotMovedFrom(tpa.second));

        ASSERT('B' == u::valueOf(tpa));

        fpg.destroy();
        u::initPair(&ofp, 'C', &TA);
        ASSERT('C' == u::valueOf(fp));

        const ToPair tpb(FP.first, FP.second);

        ASSERT(u::isNotMovedFrom(fp.first));
        ASSERT(u::isNotMovedInto(fp.first));
        ASSERT(u::isNotMovedFrom(fp.second));
        ASSERT(u::isNotMovedInto(fp.second));
        ASSERT(u::isNotMovedInto(tpb.first));
        ASSERT(u::isNotMovedFrom(tpb.first));
        ASSERT(u::isNotMovedInto(tpb.second));
        ASSERT(u::isNotMovedFrom(tpb.second));

        ASSERT('C' == u::valueOf(tpb));

        fpg.destroy();
        u::initPair(&ofp, 'D', &TA);
        ASSERT('D' == u::valueOf(fp));

        const ToPair tpc(FP);

        ASSERT(u::isNotMovedFrom(fp.first));
        ASSERT(u::isNotMovedInto(fp.first));
        ASSERT(u::isNotMovedFrom(fp.second));
        ASSERT(u::isNotMovedInto(fp.second));
        ASSERT(u::isNotMovedInto(tpc.first));
        ASSERT(u::isNotMovedFrom(tpc.first));
        ASSERT(u::isNotMovedInto(tpc.second));
        ASSERT(u::isNotMovedFrom(tpc.second));

        ASSERT('D' == u::valueOf(tpc));

    }

    ASSERT(0 == ta.numAllocations());
    ASSERT(0 == da.numAllocations());
}

template <class TO_FIRST, class TO_SECOND, class FROM_FIRST, class FROM_SECOND>
void TestDriver<TO_FIRST, TO_SECOND, FROM_FIRST, FROM_SECOND>::testCase11(
                                                                bsl::true_type)
{
    if (veryVerbose) printf("TD<%s, %s>::case11, alloc\n",
                            NameOf<ToFirst>().name(),
                            NameOf<ToSecond>().name());

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    bslma::TestAllocator tb(veryVeryVeryVerbose);
    bslma::TestAllocator da(veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard daGuard(&da);

    Int64 numDeliberateDefaultAllocs = 0;

    for (int useTa = 1; useTa >= 0; --useTa) {
        bslma::TestAllocator& TB = useTa ? ta : tb;

        bsls::ObjectBuffer<FromPair> ofp;
        FromPair& fp = u::initPair(&ofp, 'A', &ta);    const FromPair& FP = fp;
        u::PairGuard<FromPair> fpg(&fp);

        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isNotMovedFrom(fp));

        ASSERT(u::allocatorMatches(fp.first,  &ta));
        ASSERT(u::allocatorMatches(fp.second, &ta));
        ASSERT('A' == u::valueOf(fp));

        {
            const ToPair tp(FP.first, MoveUtil::move(fp.second));

            ASSERT(u::isNotMovedFrom(fp.first));
            ASSERT(u::isNotMovedInto(fp.first));
            ASSERT(u::isMovedFrom(   fp.second));
            ASSERT(u::isNotMovedInto(fp.second));
            ASSERT(u::isNotMovedInto(tp.first));
            ASSERT(u::isNotMovedFrom(tp.first));
            ASSERT(u::isMovedInto(   tp.second));
            ASSERT(u::isNotMovedFrom(tp.second));

            ASSERT(u::allocatorMatches(tp.first,  &da));
            ASSERT(u::allocatorMatches(tp.second, k_MOVE_2 ? &ta : &da));

            ASSERT('A' == u::valueOf(tp));

            const Int64 deltaAllocs = da.numAllocations() -
                                                    numDeliberateDefaultAllocs;
            numDeliberateDefaultAllocs = da.numAllocations();
            ASSERTV(NameOf<ToPair>(), k_ALLOC_1, k_ALLOC_2, k_MOVE_2,
                                                                   deltaAllocs,
                                      (k_ALLOC_1 || (k_ALLOC_2 && !k_MOVE_2) ==
                                                           (0 < deltaAllocs)));
        }

        fpg.destroy();
        u::initPair(&ofp, 'B', &ta);
        ASSERT('B' == u::valueOf(fp));

        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isNotMovedFrom(fp));

        ASSERT(u::allocatorMatches(fp.first,  &ta));
        ASSERT(u::allocatorMatches(fp.second, &ta));
        ASSERT('B' == u::valueOf(fp));

        {
            const ToPair tp(MoveUtil::move(fp.first), FP.second);

            ASSERT(u::isMovedFrom(   fp.first));
            ASSERT(u::isNotMovedInto(fp.first));
            ASSERT(u::isNotMovedFrom(fp.second));
            ASSERT(u::isNotMovedInto(fp.second));
            ASSERT(u::isMovedInto(   tp.first));
            ASSERT(u::isNotMovedFrom(tp.first));
            ASSERT(u::isNotMovedInto(tp.second));
            ASSERT(u::isNotMovedFrom(tp.second));

            ASSERT(u::allocatorMatches(tp.first,  k_MOVE_1 ? &ta : &da));
            ASSERT(u::allocatorMatches(tp.second, &da));

            ASSERT('B' == u::valueOf(tp));

            const Int64 deltaAllocs = da.numAllocations() -
                                                    numDeliberateDefaultAllocs;
            numDeliberateDefaultAllocs = da.numAllocations();
            ASSERTV(NameOf<ToPair>(), k_ALLOC_1, k_MOVE_1, k_ALLOC_2,
                                                                   deltaAllocs,
                 ((k_ALLOC_1 && !k_MOVE_1) || k_ALLOC_2) == (0 < deltaAllocs));
        }

        fpg.destroy();
        u::initPair(&ofp, 'C', &ta);
        ASSERT('C' == u::valueOf(fp));

        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isNotMovedFrom(fp));

        ASSERT(u::allocatorMatches(fp.first,  &ta));
        ASSERT(u::allocatorMatches(fp.second, &ta));
        ASSERT('C' == u::valueOf(fp));

        {
            const ToPair tpa(MoveUtil::move(fp.first),
                             FP.second,
                             &TB);

            ASSERT(u::isMovedFrom(   fp.first));
            ASSERT(u::isNotMovedInto(fp.first));
            ASSERT(u::isNotMovedFrom(fp.second));
            ASSERT(u::isNotMovedInto(fp.second));
            ASSERT(u::isMovedInto(   tpa.first));
            ASSERT(u::isNotMovedFrom(tpa.first));
            ASSERT(u::isNotMovedInto(tpa.second));
            ASSERT(u::isNotMovedFrom(tpa.second));

            ASSERT(u::allocatorMatches(tpa.first,  &TB));
            ASSERT(u::allocatorMatches(tpa.second, &TB));

            ASSERT('C' == u::valueOf(tpa));
        }

        fpg.destroy();
        u::initPair(&ofp, 'D', &ta);
        ASSERT('D' == u::valueOf(fp));

        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isNotMovedFrom(fp));

        ASSERT(u::allocatorMatches(fp.first,  &ta));
        ASSERT(u::allocatorMatches(fp.second, &ta));
        ASSERT('D' == u::valueOf(fp));

        {
            const ToPair tpa(FP.first,
                             MoveUtil::move(fp.second),
                             &TB);

            ASSERT(u::isNotMovedFrom(fp.first));
            ASSERT(u::isNotMovedInto(fp.first));
            ASSERT(u::isMovedFrom(   fp.second));
            ASSERT(u::isNotMovedInto(fp.second));
            ASSERT(u::isNotMovedInto(tpa.first));
            ASSERT(u::isNotMovedFrom(tpa.first));
            ASSERT(u::isMovedInto(   tpa.second));
            ASSERT(u::isNotMovedFrom(tpa.second));

            ASSERT(u::allocatorMatches(tpa.first,  &TB));
            ASSERT(u::allocatorMatches(tpa.second, &TB));

            ASSERT('D' == u::valueOf(tpa));
        }

        fpg.destroy();
        u::initPair(&ofp, 'E', &ta);
        ASSERT('E' == u::valueOf(fp));

        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isNotMovedFrom(fp));

        ASSERT(u::allocatorMatches(fp.first,  &ta));
        ASSERT(u::allocatorMatches(fp.second, &ta));
        ASSERT('E' == u::valueOf(fp));

        {
            const ToPair tpa(FP.first,
                             FP.second,
                             &TB);

            ASSERT(u::isNotMovedFrom(fp.first));
            ASSERT(u::isNotMovedInto(fp.first));
            ASSERT(u::isNotMovedFrom(fp.second));
            ASSERT(u::isNotMovedInto(fp.second));
            ASSERT(u::isNotMovedInto(tpa.first));
            ASSERT(u::isNotMovedFrom(tpa.first));
            ASSERT(u::isNotMovedInto(tpa.second));
            ASSERT(u::isNotMovedFrom(tpa.second));

            ASSERT(u::allocatorMatches(tpa.first,  &TB));
            ASSERT(u::allocatorMatches(tpa.second, &TB));

            ASSERT('E' == u::valueOf(tpa));
        }

        fpg.destroy();
        u::initPair(&ofp, 'F', &ta);
        ASSERT('F' == u::valueOf(fp));

        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isNotMovedFrom(fp));

        ASSERT(u::allocatorMatches(fp.first,  &ta));
        ASSERT(u::allocatorMatches(fp.second, &ta));
        ASSERT('F' == u::valueOf(fp));

        {
            const ToPair tpa(FP, &TB);

            ASSERT(u::isNotMovedFrom(fp.first));
            ASSERT(u::isNotMovedInto(fp.first));
            ASSERT(u::isNotMovedFrom(fp.second));
            ASSERT(u::isNotMovedInto(fp.second));
            ASSERT(u::isNotMovedInto(tpa.first));
            ASSERT(u::isNotMovedFrom(tpa.first));
            ASSERT(u::isNotMovedInto(tpa.second));
            ASSERT(u::isNotMovedFrom(tpa.second));

            ASSERT(u::allocatorMatches(tpa.first,  &TB));
            ASSERT(u::allocatorMatches(tpa.second, &TB));

            ASSERT('F' == u::valueOf(tpa));
        }

        ASSERT(0 < ta.numAllocations());
        ASSERT(!useTa == (0 < tb.numAllocations()));
    }

    ASSERT(da.numAllocations() == numDeliberateDefaultAllocs);
}

template <class TO_FIRST, class TO_SECOND, class FROM_FIRST, class FROM_SECOND>
void TestDriver<TO_FIRST, TO_SECOND, FROM_FIRST, FROM_SECOND>::testCase11()
{
    // Dispatch depending on whether any of the parameter types allocate
    // memory.

    testCase11(UsesBslma());
}

template <class TO_FIRST, class TO_SECOND, class FROM_FIRST, class FROM_SECOND>
void TestDriver<TO_FIRST, TO_SECOND, FROM_FIRST, FROM_SECOND>::testCase10(
                                                               bsl::false_type)
{
    if (veryVerbose) printf("TD<%s, %s>::case10, no alloc\n",
                            NameOf<ToFirst>().name(),
                            NameOf<ToSecond>().name());

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    bslma::TestAllocator da(veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard daGuard(&da);

    {
        bsls::ObjectBuffer<FromPair> ofp;
        FromPair& fp = u::initPair(&ofp, 'A', &ta);
        u::PairGuard<FromPair> fpg(&fp);

        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isNotMovedFrom(fp));
        ASSERT('A' == u::valueOf(fp));

        const ToPair tp(MoveUtil::move(fp.first), MoveUtil::move(fp.second));

        ASSERT(u::isMovedFrom(   fp));
        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isMovedInto(   tp));
        ASSERT(u::isNotMovedFrom(tp));

        ASSERT('A' == u::valueOf(tp));
    }

    ASSERT(0 == ta.numAllocations());
    ASSERT(0 == da.numAllocations());
}

template <class TO_FIRST, class TO_SECOND, class FROM_FIRST, class FROM_SECOND>
void TestDriver<TO_FIRST, TO_SECOND, FROM_FIRST, FROM_SECOND>::testCase10(
                                                                bsl::true_type)
{
    if (veryVerbose) printf("TD<%s, %s>::case10, alloc\n",
                            NameOf<ToFirst>().name(),
                            NameOf<ToSecond>().name());

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    bslma::TestAllocator tb(veryVeryVeryVerbose);
    bslma::TestAllocator da(veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard daGuard(&da);

    Int64 numDeliberateDefaultAllocs = 0;
    {
        bsls::ObjectBuffer<FromPair> ofp;
        FromPair& fp = u::initPair(&ofp, 'A', &ta);
        u::PairGuard<FromPair> fpg(&fp);

        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::allocatorMatches(fp.first,  &ta));
        ASSERT(u::allocatorMatches(fp.second, &ta));
        ASSERT('A' == u::valueOf(fp));

        const ToPair tp(MoveUtil::move(fp.first), MoveUtil::move(fp.second));

        ASSERT(u::isMovedFrom(   fp));
        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isMovedInto(   tp));
        ASSERT(u::isNotMovedFrom(tp));

        ASSERT(u::allocatorMatches(tp.first,  k_MOVE_1 ? &ta : &da));
        ASSERT(u::allocatorMatches(tp.second, k_MOVE_2 ? &ta : &da));

        ASSERT('A' == u::valueOf(tp));

        numDeliberateDefaultAllocs = da.numAllocations();
        ASSERTV(NameOf<ToPair>(), k_MOVE_1, k_MOVE_2,
                                                    numDeliberateDefaultAllocs,
                      ((k_ALLOC_1 && !k_MOVE_1) || (k_ALLOC_2 && !k_MOVE_2)) ==
                                             (0 < numDeliberateDefaultAllocs));

        fpg.destroy();
        u::initPair(&ofp, 'B', &ta);
        ASSERT('B' == u::valueOf(fp));

        const ToPair tpa(MoveUtil::move(fp.first),
                         MoveUtil::move(fp.second),
                         &ta);

        ASSERT(u::isMovedFrom(   fp));
        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isMovedInto(   tpa));
        ASSERT(u::isNotMovedFrom(tpa));

        ASSERT(u::allocatorMatches(tpa.first,  &ta));
        ASSERT(u::allocatorMatches(tpa.second, &ta));

        ASSERT('B' == u::valueOf(tpa));

        fpg.destroy();
        u::initPair(&ofp, 'C', &ta);
        ASSERT('C' == u::valueOf(fp));

        const ToPair tpb(MoveUtil::move(fp.first),
                         MoveUtil::move(fp.second),
                         &tb);

        ASSERT(u::isMovedFrom(   fp));
        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isMovedInto(   tpb));
        ASSERT(u::isNotMovedFrom(tpb));

        ASSERT(u::allocatorMatches(tpb.first,  &tb));
        ASSERT(u::allocatorMatches(tpb.second, &tb));

        ASSERT('C' == u::valueOf(tpb));
    }

    if (bsl::is_same<ToFirst, ToSecond>::value) {
        // TBD: clearly the following is just a simple test to ensure that the
        // lvalue references to const and non-const values are being processed
        // correctly.

        {
            typedef ManagedWrapper<ToFirst>              WrappedType;
            typedef bsl::pair<WrappedType, int>          ManagedType;
            bslma::ManagedPtr<ToFirst> mp;
            ManagedType mt(mp, 0);
        }
        {
            typedef ManagedWrapper<ToFirst>              WrappedType;
            typedef bsl::pair<int, WrappedType>          ManagedType;
            bslma::ManagedPtr<ToFirst> mp;
            ManagedType mt(0, mp);
        }
        {
            typedef ManagedWrapper<ToFirst>              WrappedType;
            typedef bsl::pair<WrappedType, WrappedType>  ManagedType;
            bslma::ManagedPtr<ToFirst> mp1;
            bslma::ManagedPtr<ToFirst> mp2;
            ManagedType mt(mp1, mp2);
        }
    }

    ASSERT(0 <  ta.numAllocations());
    ASSERT(0 <  tb.numAllocations());
    ASSERT(da.numAllocations() == numDeliberateDefaultAllocs);
}

template <class TO_FIRST, class TO_SECOND, class FROM_FIRST, class FROM_SECOND>
void TestDriver<TO_FIRST, TO_SECOND, FROM_FIRST, FROM_SECOND>::testCase10()
{
    // Dispatch depending on whether any of the parameter types allocate
    // memory.

    testCase10(UsesBslma());
}

template <class TO_FIRST, class TO_SECOND, class FROM_FIRST, class FROM_SECOND>
void TestDriver<TO_FIRST, TO_SECOND, FROM_FIRST, FROM_SECOND>::testCase9(
                                                               bsl::false_type)
{
    if (veryVerbose) printf("TD<%s, %s>::case9, no alloc\n",
                            NameOf<ToFirst>().name(),
                            NameOf<ToSecond>().name());

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    bslma::TestAllocator da(veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard daGuard(&da);

    {
        bsls::ObjectBuffer<FromPair> ofp;
        FromPair& fp = u::initPair(&ofp, 'A', &ta);
        u::PairGuard<FromPair> fpg(&fp);

        ASSERT(u::isNotMovedInto(fp));
        ASSERT('A' == u::valueOf(fp));

        const ToPair tp(MoveUtil::move(fp));

        ASSERT(u::isMovedFrom(   fp));
        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isMovedInto(   tp));
        ASSERT(u::isNotMovedFrom(tp));

        ASSERT('A' == u::valueOf(tp));
    }

    ASSERT(0 == ta.numAllocations());
    ASSERT(0 == da.numAllocations());
}

template <class TO_FIRST, class TO_SECOND, class FROM_FIRST, class FROM_SECOND>
void TestDriver<TO_FIRST, TO_SECOND, FROM_FIRST, FROM_SECOND>::testCase9(
bsl::true_type)
{
    if (veryVerbose) printf("TD<%s, %s>::case9, alloc\n",
                            NameOf<ToFirst>().name(),
                            NameOf<ToSecond>().name());

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    bslma::TestAllocator tb(veryVeryVeryVerbose);
    bslma::TestAllocator da(veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard daGuard(&da);

    Int64 numDeliberateDefaultAllocs = 0;
    {
        bsls::ObjectBuffer<FromPair> ofp;
        FromPair& fp = u::initPair(&ofp, 'A', &ta);
        u::PairGuard<FromPair> fpg(&fp);

        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::allocatorMatches(fp.first,  &ta));
        ASSERT(u::allocatorMatches(fp.second, &ta));
        ASSERT('A' == u::valueOf(fp));

        const ToPair tp(MoveUtil::move(fp));

        ASSERT(u::isMovedFrom(   fp));
        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isMovedInto(   tp));
        ASSERT(u::isNotMovedFrom(tp));

        ASSERT(u::allocatorMatches(tp.first,  k_MOVE_1 ? &ta : &da));
        ASSERT(u::allocatorMatches(tp.second, k_MOVE_2 ? &ta : &da));

        ASSERT('A' == u::valueOf(tp));

        numDeliberateDefaultAllocs = da.numAllocations();
        ASSERTV(NameOf<ToPair>(), k_MOVE_1, k_MOVE_2,
                                                    numDeliberateDefaultAllocs,
                      ((k_ALLOC_1 && !k_MOVE_1) || (k_ALLOC_2 && !k_MOVE_2)) ==
                                             (0 < numDeliberateDefaultAllocs));

        fpg.destroy();

        u::initPair(&ofp, 'B', &ta);
        ASSERT('B' == u::valueOf(fp));

        const ToPair tpa(MoveUtil::move(fp), &ta);

        ASSERT(u::isMovedFrom(   fp));
        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isMovedInto(   tpa));
        ASSERT(u::isNotMovedFrom(tpa));

        ASSERT(u::allocatorMatches(tpa.first,  &ta));
        ASSERT(u::allocatorMatches(tpa.second, &ta));

        ASSERT('B' == u::valueOf(tpa));

        fpg.destroy();
        u::initPair(&ofp, 'C', &ta);
        ASSERT('C' == u::valueOf(fp));

        const ToPair tpb(MoveUtil::move(fp), &tb);

        ASSERT(u::isMovedFrom(   fp));
        ASSERT(u::isNotMovedInto(fp));
        ASSERT(u::isMovedInto(   tpb));
        ASSERT(u::isNotMovedFrom(tpb));

        ASSERT(u::allocatorMatches(tpb.first,  &tb));
        ASSERT(u::allocatorMatches(tpb.second, &tb));

        ASSERT('C' == u::valueOf(tpb));
    }

    ASSERT(0 <  ta.numAllocations());
    ASSERT(0 <  tb.numAllocations());
    ASSERT(da.numAllocations() == numDeliberateDefaultAllocs);
}

template <class TO_FIRST, class TO_SECOND, class FROM_FIRST, class FROM_SECOND>
void TestDriver<TO_FIRST, TO_SECOND, FROM_FIRST, FROM_SECOND>::testCase9()
{
    // Dispatch depending on whether any of the parameter types allocate
    // memory.

    testCase9(UsesBslma());
}

template <class TYPE>
struct MetaTestDriver {
    // We will try different combinations of type to be passed to the
    // 'TestDriver' class.
    //
    // In practice, the 'TestDriver' class will take the form
    //
    // 'TestDriver<ToFirst, ToSecond, FromFirst, FromSecond>'
    //
    // where it creates two types of pairs:
    //
    //: o 'ToPair' == 'pair<ToFirst, ToSecond>', and
    //: o 'FromPair' == 'pair<FromFirst, FromSecond>'
    //
    // where 'ToPair' should be implicitly convertable from a 'FromPair'.  We
    // will also be doing move conversions, which in C++03 only work if
    // 'ToPair' is trivially (as opposed to implicitly) convertible from
    // 'FromPair', meaning that the respective 'first_type's and 'second_type's
    // either match, or the form in 'FromPair' is a derived class of the form
    // in 'ToPair'.  In C++11, where we are dealing with genuine rvalue
    // references, we also allow the 'From' types to be 'u::Node', which is
    // implicitly convertible to 'u::Base', but is not derived from it.

    typedef TestDriver<TYPE,      TYPE,      TYPE,         TYPE        > Dr00;

    typedef TestDriver<TYPE,      u::Base,   TYPE,         u::Derived  > Dr01;
    typedef TestDriver<u::Base,   TYPE,      u::Derived,   TYPE        > Dr10;
    typedef TestDriver<u::Base,   u::Base,   u::Derived,   u::Derived  > Dr11;

    typedef TestDriver<TYPE,      u::AlBase, TYPE,         u::AlDerived> Dr0a;
    typedef TestDriver<u::AlBase, TYPE,      u::AlDerived, TYPE        > Dra0;
    typedef TestDriver<u::AlBase, u::AlBase, u::AlDerived, u::AlDerived> Draa;

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

    typedef TestDriver<TYPE,    u::Base, TYPE,       u::Node   > Dr0n;
    typedef TestDriver<u::Base, TYPE,    u::Node,    TYPE      > Drn0;
    typedef TestDriver<u::Base, u::Base, u::Node,    u::Node   > Drnn;

#endif

    static void testCase14();
    static void testCase12_copy();
    static void testCase12_move();
    static void testCase11();
    static void testCase10();
    static void testCase9();
};

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

// Define our function to call the test driver, including using 'u::Node'.

#define u_META_FUNCTION(funcName)                                             \
template <class TYPE>                                                         \
void MetaTestDriver<TYPE>::funcName()                                         \
{                                                                             \
    Dr00::funcName();                                                         \
                                                                              \
    Dr01::funcName();                                                         \
    Dr10::funcName();                                                         \
                                                                              \
    Dr0a::funcName();                                                         \
    Dra0::funcName();                                                         \
                                                                              \
    Dr0n::funcName();                                                         \
    Drn0::funcName();                                                         \
                                                                              \
    if (bsl::is_same<TYPE, signed char>::value) {                             \
        Dr11::funcName();                                                     \
        Draa::funcName();                                                     \
        Drnn::funcName();                                                     \
    }                                                                         \
}

#else

// Define our function to call the test driver, but don't use 'u::Node'.

#define u_META_FUNCTION(funcName)                                             \
template <class TYPE>                                                         \
void MetaTestDriver<TYPE>::funcName()                                         \
{                                                                             \
    Dr00::funcName();                                                         \
                                                                              \
    Dr01::funcName();                                                         \
    Dr10::funcName();                                                         \
                                                                              \
    Dr0a::funcName();                                                         \
    Dra0::funcName();                                                         \
                                                                              \
    if (bsl::is_same<TYPE, signed char>::value) {                             \
        Dr11::funcName();                                                     \
        Draa::funcName();                                                     \
    }                                                                         \
}

#endif

u_META_FUNCTION(testCase14)
u_META_FUNCTION(testCase12_copy)
u_META_FUNCTION(testCase12_move)
u_META_FUNCTION(testCase11)
u_META_FUNCTION(testCase10)
u_META_FUNCTION(testCase9)

#undef u_META_FUNCTION

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 16: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "'noexcept' SPECIFICATION" "\n"
                                 "========================" "\n");

        testCase16();

      } break;
      case 15: {
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

        usageExample();

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // C'TOR TO/FROM NATIVE_STD::PAIR, PAIRS WITHIN PAIRS
        //
        // This test case was written to expose a known problem, and verify its
        // fix, to do with pairs within pairs, and conversions between
        // 'bsl::pair' and 'native_std::pair'.
        //
        // Concerns:
        //: 1 In all cases, we will be dealing with cases where one member of
        //:   the pair is a pair, and the other member is a boolean.  We will
        //:   test everything in the cases
        //:   o 'pair<pair, bool>'
        //:   o 'pair<bool, pair>'
        //:
        //: 2 There is only a conversion from a 'native_std::pair' to a
        //:   'bsl::pair', not the other way around, even in the case where
        //:   'first_type' and 'second_type' match, so conversions from
        //:   'bsl::pair' to 'native_std::pair' are *NOT* tested.
        //:
        //: 3 Test a conversion from a pair where the nested pair type of the
        //:   source and destination match.
        //:
        //: 4 Test a conversation from a 'native_std::pair' to a 'bsl::pair'
        //:   containing a nested 'bsl::pair'.
        //:   o In the case where the destination and source 'bsl::pair' member
        //:     are of matching type.
        //:   o In the case where the destination and source 'bsl::pair' member
        //:     is of type 'ToPair' and the matching source member is
        //:     'FromPair'.
        //:
        //: 5 Test conversions where the inner pair goes from a 'FromPair' to a
        //:   'ToPair'.
        //:
        //: 6 If none of the types under test allocate memory, observe that the
        //:   default allocator is never used, even when no allocator is passed
        //:   to the pair constructor.  Also observe that if an allocator is
        //:   passed to the pair constructor, it isn't used.
        //:
        //: 7 If any of the types under test allocate memory, observe that the
        //:   creation of a 'native_std::pair' makes use of the default
        //:   allocator, and that the creation of 'bsl::pair' that has an
        //:   allocator passed to the c'tor makes use of the passed allocator.
        //
        // TESTING:
        //   pair(native_std::pair<*>, bool>)
        // --------------------------------------------------------------------

        if (verbose) printf(
                       "C'TOR TO/FROM NATIVE_STD::PAIR, PAIRS WITHIN PAIRS\n"
                       "==================================================\n");

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase14,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

                   // BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType) -- test case needs copy
                   // c'tor.
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING TUPLE-BASED CONSTRUCTION
        //
        // Concerns:
        //: 1 That the constructor of 'bsl::pair' can properly propagate
        //:   complex argument types to a template constructors of its member
        //:   types.
        //
        // Plan:
        //: 1 Create a template function 'testArg' that will return an argument
        //:   either copied or moved, depending upon a integral template
        //:   argument.
        //:
        //: 2 Create the function 'runTestNoAlloc' which will take 8 integral
        //:   template arguments to drive how to create 2 tuples to pass to the
        //:   constructor of a 'bsl::pair' of 'bsltf::EmplacableType's.  After
        //:   the pair is created, its value is assessed for accuracy.
        //:
        //: 3 Create the function 'runTestAlloc' which will take 8 integral
        //:   template arguments to drive how to create 2 tuples to pass to the
        //:   constructor of a 'bsl::pair' of 'bsltf::AllocEmplacableType's.
        //:   After the pair is created, its value is assessed for accuracy and
        //:   checks are done to ensure that memory was allocated from the
        //:   passed allocator.
        //:
        //: 4 In 'main' call 'runTestAlloc' and 'runTestNoAlloc' with every
        //:   possible valid combination of template arguments.
        //
        // Testing:
        //   bsl::pair(piecewise_construct, tuple, tuple);
        //   bsl::pair(piecewise_construct, tuple, tuple, alloc);
        // --------------------------------------------------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
        // These series were machine generated to generate all possible
        // combinations of calls with no repetition:

        TupleTestDriver::runTestAlloc<0, 2,2,2, 0, 2,2,2>();
        TupleTestDriver::runTestAlloc<0, 2,2,2, 1, 0,2,2>();
        TupleTestDriver::runTestAlloc<0, 2,2,2, 1, 1,2,2>();
        TupleTestDriver::runTestAlloc<0, 2,2,2, 2, 0,0,2>();
        TupleTestDriver::runTestAlloc<0, 2,2,2, 2, 0,1,2>();
        TupleTestDriver::runTestAlloc<0, 2,2,2, 2, 1,0,2>();
        TupleTestDriver::runTestAlloc<0, 2,2,2, 2, 1,1,2>();
        TupleTestDriver::runTestAlloc<0, 2,2,2, 3, 0,0,0>();
        TupleTestDriver::runTestAlloc<0, 2,2,2, 3, 0,0,1>();
        TupleTestDriver::runTestAlloc<0, 2,2,2, 3, 0,1,0>();
        TupleTestDriver::runTestAlloc<0, 2,2,2, 3, 0,1,1>();
        TupleTestDriver::runTestAlloc<0, 2,2,2, 3, 1,0,0>();
        TupleTestDriver::runTestAlloc<0, 2,2,2, 3, 1,0,1>();
        TupleTestDriver::runTestAlloc<0, 2,2,2, 3, 1,1,0>();
        TupleTestDriver::runTestAlloc<0, 2,2,2, 3, 1,1,1>();
        TupleTestDriver::runTestAlloc<1, 0,2,2, 0, 2,2,2>();
        TupleTestDriver::runTestAlloc<1, 0,2,2, 1, 0,2,2>();
        TupleTestDriver::runTestAlloc<1, 0,2,2, 1, 1,2,2>();
        TupleTestDriver::runTestAlloc<1, 0,2,2, 2, 0,0,2>();
        TupleTestDriver::runTestAlloc<1, 0,2,2, 2, 0,1,2>();
        TupleTestDriver::runTestAlloc<1, 0,2,2, 2, 1,0,2>();
        TupleTestDriver::runTestAlloc<1, 0,2,2, 2, 1,1,2>();
        TupleTestDriver::runTestAlloc<1, 0,2,2, 3, 0,0,0>();
        TupleTestDriver::runTestAlloc<1, 0,2,2, 3, 0,0,1>();
        TupleTestDriver::runTestAlloc<1, 0,2,2, 3, 0,1,0>();
        TupleTestDriver::runTestAlloc<1, 0,2,2, 3, 0,1,1>();
        TupleTestDriver::runTestAlloc<1, 0,2,2, 3, 1,0,0>();
        TupleTestDriver::runTestAlloc<1, 0,2,2, 3, 1,0,1>();
        TupleTestDriver::runTestAlloc<1, 0,2,2, 3, 1,1,0>();
        TupleTestDriver::runTestAlloc<1, 0,2,2, 3, 1,1,1>();
        TupleTestDriver::runTestAlloc<1, 1,2,2, 0, 2,2,2>();
        TupleTestDriver::runTestAlloc<1, 1,2,2, 1, 0,2,2>();
        TupleTestDriver::runTestAlloc<1, 1,2,2, 1, 1,2,2>();
        TupleTestDriver::runTestAlloc<1, 1,2,2, 2, 0,0,2>();
        TupleTestDriver::runTestAlloc<1, 1,2,2, 2, 0,1,2>();
        TupleTestDriver::runTestAlloc<1, 1,2,2, 2, 1,0,2>();
        TupleTestDriver::runTestAlloc<1, 1,2,2, 2, 1,1,2>();
        TupleTestDriver::runTestAlloc<1, 1,2,2, 3, 0,0,0>();
        TupleTestDriver::runTestAlloc<1, 1,2,2, 3, 0,0,1>();
        TupleTestDriver::runTestAlloc<1, 1,2,2, 3, 0,1,0>();
        TupleTestDriver::runTestAlloc<1, 1,2,2, 3, 0,1,1>();
        TupleTestDriver::runTestAlloc<1, 1,2,2, 3, 1,0,0>();
        TupleTestDriver::runTestAlloc<1, 1,2,2, 3, 1,0,1>();
        TupleTestDriver::runTestAlloc<1, 1,2,2, 3, 1,1,0>();
        TupleTestDriver::runTestAlloc<1, 1,2,2, 3, 1,1,1>();
        TupleTestDriver::runTestAlloc<2, 0,0,2, 0, 2,2,2>();
        TupleTestDriver::runTestAlloc<2, 0,0,2, 1, 0,2,2>();
        TupleTestDriver::runTestAlloc<2, 0,0,2, 1, 1,2,2>();
        TupleTestDriver::runTestAlloc<2, 0,0,2, 2, 0,0,2>();
        TupleTestDriver::runTestAlloc<2, 0,0,2, 2, 0,1,2>();
        TupleTestDriver::runTestAlloc<2, 0,0,2, 2, 1,0,2>();
        TupleTestDriver::runTestAlloc<2, 0,0,2, 2, 1,1,2>();
        TupleTestDriver::runTestAlloc<2, 0,0,2, 3, 0,0,0>();
        TupleTestDriver::runTestAlloc<2, 0,0,2, 3, 0,0,1>();
        TupleTestDriver::runTestAlloc<2, 0,0,2, 3, 0,1,0>();
        TupleTestDriver::runTestAlloc<2, 0,0,2, 3, 0,1,1>();
        TupleTestDriver::runTestAlloc<2, 0,0,2, 3, 1,0,0>();
        TupleTestDriver::runTestAlloc<2, 0,0,2, 3, 1,0,1>();
        TupleTestDriver::runTestAlloc<2, 0,0,2, 3, 1,1,0>();
        TupleTestDriver::runTestAlloc<2, 0,0,2, 3, 1,1,1>();
        TupleTestDriver::runTestAlloc<2, 0,1,2, 0, 2,2,2>();
        TupleTestDriver::runTestAlloc<2, 0,1,2, 1, 0,2,2>();
        TupleTestDriver::runTestAlloc<2, 0,1,2, 1, 1,2,2>();
        TupleTestDriver::runTestAlloc<2, 0,1,2, 2, 0,0,2>();
        TupleTestDriver::runTestAlloc<2, 0,1,2, 2, 0,1,2>();
        TupleTestDriver::runTestAlloc<2, 0,1,2, 2, 1,0,2>();
        TupleTestDriver::runTestAlloc<2, 0,1,2, 2, 1,1,2>();
        TupleTestDriver::runTestAlloc<2, 0,1,2, 3, 0,0,0>();
        TupleTestDriver::runTestAlloc<2, 0,1,2, 3, 0,0,1>();
        TupleTestDriver::runTestAlloc<2, 0,1,2, 3, 0,1,0>();
        TupleTestDriver::runTestAlloc<2, 0,1,2, 3, 0,1,1>();
        TupleTestDriver::runTestAlloc<2, 0,1,2, 3, 1,0,0>();
        TupleTestDriver::runTestAlloc<2, 0,1,2, 3, 1,0,1>();
        TupleTestDriver::runTestAlloc<2, 0,1,2, 3, 1,1,0>();
        TupleTestDriver::runTestAlloc<2, 0,1,2, 3, 1,1,1>();
        TupleTestDriver::runTestAlloc<2, 1,0,2, 0, 2,2,2>();
        TupleTestDriver::runTestAlloc<2, 1,0,2, 1, 0,2,2>();
        TupleTestDriver::runTestAlloc<2, 1,0,2, 1, 1,2,2>();
        TupleTestDriver::runTestAlloc<2, 1,0,2, 2, 0,0,2>();
        TupleTestDriver::runTestAlloc<2, 1,0,2, 2, 0,1,2>();
        TupleTestDriver::runTestAlloc<2, 1,0,2, 2, 1,0,2>();
        TupleTestDriver::runTestAlloc<2, 1,0,2, 2, 1,1,2>();
        TupleTestDriver::runTestAlloc<2, 1,0,2, 3, 0,0,0>();
        TupleTestDriver::runTestAlloc<2, 1,0,2, 3, 0,0,1>();
        TupleTestDriver::runTestAlloc<2, 1,0,2, 3, 0,1,0>();
        TupleTestDriver::runTestAlloc<2, 1,0,2, 3, 0,1,1>();
        TupleTestDriver::runTestAlloc<2, 1,0,2, 3, 1,0,0>();
        TupleTestDriver::runTestAlloc<2, 1,0,2, 3, 1,0,1>();
        TupleTestDriver::runTestAlloc<2, 1,0,2, 3, 1,1,0>();
        TupleTestDriver::runTestAlloc<2, 1,0,2, 3, 1,1,1>();
        TupleTestDriver::runTestAlloc<2, 1,1,2, 0, 2,2,2>();
        TupleTestDriver::runTestAlloc<2, 1,1,2, 1, 0,2,2>();
        TupleTestDriver::runTestAlloc<2, 1,1,2, 1, 1,2,2>();
        TupleTestDriver::runTestAlloc<2, 1,1,2, 2, 0,0,2>();
        TupleTestDriver::runTestAlloc<2, 1,1,2, 2, 0,1,2>();
        TupleTestDriver::runTestAlloc<2, 1,1,2, 2, 1,0,2>();
        TupleTestDriver::runTestAlloc<2, 1,1,2, 2, 1,1,2>();
        TupleTestDriver::runTestAlloc<2, 1,1,2, 3, 0,0,0>();
        TupleTestDriver::runTestAlloc<2, 1,1,2, 3, 0,0,1>();
        TupleTestDriver::runTestAlloc<2, 1,1,2, 3, 0,1,0>();
        TupleTestDriver::runTestAlloc<2, 1,1,2, 3, 0,1,1>();
        TupleTestDriver::runTestAlloc<2, 1,1,2, 3, 1,0,0>();
        TupleTestDriver::runTestAlloc<2, 1,1,2, 3, 1,0,1>();
        TupleTestDriver::runTestAlloc<2, 1,1,2, 3, 1,1,0>();
        TupleTestDriver::runTestAlloc<2, 1,1,2, 3, 1,1,1>();
        TupleTestDriver::runTestAlloc<3, 0,0,0, 0, 2,2,2>();
        TupleTestDriver::runTestAlloc<3, 0,0,0, 1, 0,2,2>();
        TupleTestDriver::runTestAlloc<3, 0,0,0, 1, 1,2,2>();
        TupleTestDriver::runTestAlloc<3, 0,0,0, 2, 0,0,2>();
        TupleTestDriver::runTestAlloc<3, 0,0,0, 2, 0,1,2>();
        TupleTestDriver::runTestAlloc<3, 0,0,0, 2, 1,0,2>();
        TupleTestDriver::runTestAlloc<3, 0,0,0, 2, 1,1,2>();
        TupleTestDriver::runTestAlloc<3, 0,0,0, 3, 0,0,0>();
        TupleTestDriver::runTestAlloc<3, 0,0,0, 3, 0,0,1>();
        TupleTestDriver::runTestAlloc<3, 0,0,0, 3, 0,1,0>();
        TupleTestDriver::runTestAlloc<3, 0,0,0, 3, 0,1,1>();
        TupleTestDriver::runTestAlloc<3, 0,0,0, 3, 1,0,0>();
        TupleTestDriver::runTestAlloc<3, 0,0,0, 3, 1,0,1>();
        TupleTestDriver::runTestAlloc<3, 0,0,0, 3, 1,1,0>();
        TupleTestDriver::runTestAlloc<3, 0,0,0, 3, 1,1,1>();
        TupleTestDriver::runTestAlloc<3, 0,0,1, 0, 2,2,2>();
        TupleTestDriver::runTestAlloc<3, 0,0,1, 1, 0,2,2>();
        TupleTestDriver::runTestAlloc<3, 0,0,1, 1, 1,2,2>();
        TupleTestDriver::runTestAlloc<3, 0,0,1, 2, 0,0,2>();
        TupleTestDriver::runTestAlloc<3, 0,0,1, 2, 0,1,2>();
        TupleTestDriver::runTestAlloc<3, 0,0,1, 2, 1,0,2>();
        TupleTestDriver::runTestAlloc<3, 0,0,1, 2, 1,1,2>();
        TupleTestDriver::runTestAlloc<3, 0,0,1, 3, 0,0,0>();
        TupleTestDriver::runTestAlloc<3, 0,0,1, 3, 0,0,1>();
        TupleTestDriver::runTestAlloc<3, 0,0,1, 3, 0,1,0>();
        TupleTestDriver::runTestAlloc<3, 0,0,1, 3, 0,1,1>();
        TupleTestDriver::runTestAlloc<3, 0,0,1, 3, 1,0,0>();
        TupleTestDriver::runTestAlloc<3, 0,0,1, 3, 1,0,1>();
        TupleTestDriver::runTestAlloc<3, 0,0,1, 3, 1,1,0>();
        TupleTestDriver::runTestAlloc<3, 0,0,1, 3, 1,1,1>();
        TupleTestDriver::runTestAlloc<3, 0,1,0, 0, 2,2,2>();
        TupleTestDriver::runTestAlloc<3, 0,1,0, 1, 0,2,2>();
        TupleTestDriver::runTestAlloc<3, 0,1,0, 1, 1,2,2>();
        TupleTestDriver::runTestAlloc<3, 0,1,0, 2, 0,0,2>();
        TupleTestDriver::runTestAlloc<3, 0,1,0, 2, 0,1,2>();
        TupleTestDriver::runTestAlloc<3, 0,1,0, 2, 1,0,2>();
        TupleTestDriver::runTestAlloc<3, 0,1,0, 2, 1,1,2>();
        TupleTestDriver::runTestAlloc<3, 0,1,0, 3, 0,0,0>();
        TupleTestDriver::runTestAlloc<3, 0,1,0, 3, 0,0,1>();
        TupleTestDriver::runTestAlloc<3, 0,1,0, 3, 0,1,0>();
        TupleTestDriver::runTestAlloc<3, 0,1,0, 3, 0,1,1>();
        TupleTestDriver::runTestAlloc<3, 0,1,0, 3, 1,0,0>();
        TupleTestDriver::runTestAlloc<3, 0,1,0, 3, 1,0,1>();
        TupleTestDriver::runTestAlloc<3, 0,1,0, 3, 1,1,0>();
        TupleTestDriver::runTestAlloc<3, 0,1,0, 3, 1,1,1>();
        TupleTestDriver::runTestAlloc<3, 0,1,1, 0, 2,2,2>();
        TupleTestDriver::runTestAlloc<3, 0,1,1, 1, 0,2,2>();
        TupleTestDriver::runTestAlloc<3, 0,1,1, 1, 1,2,2>();
        TupleTestDriver::runTestAlloc<3, 0,1,1, 2, 0,0,2>();
        TupleTestDriver::runTestAlloc<3, 0,1,1, 2, 0,1,2>();
        TupleTestDriver::runTestAlloc<3, 0,1,1, 2, 1,0,2>();
        TupleTestDriver::runTestAlloc<3, 0,1,1, 2, 1,1,2>();
        TupleTestDriver::runTestAlloc<3, 0,1,1, 3, 0,0,0>();
        TupleTestDriver::runTestAlloc<3, 0,1,1, 3, 0,0,1>();
        TupleTestDriver::runTestAlloc<3, 0,1,1, 3, 0,1,0>();
        TupleTestDriver::runTestAlloc<3, 0,1,1, 3, 0,1,1>();
        TupleTestDriver::runTestAlloc<3, 0,1,1, 3, 1,0,0>();
        TupleTestDriver::runTestAlloc<3, 0,1,1, 3, 1,0,1>();
        TupleTestDriver::runTestAlloc<3, 0,1,1, 3, 1,1,0>();
        TupleTestDriver::runTestAlloc<3, 0,1,1, 3, 1,1,1>();
        TupleTestDriver::runTestAlloc<3, 1,0,0, 0, 2,2,2>();
        TupleTestDriver::runTestAlloc<3, 1,0,0, 1, 0,2,2>();
        TupleTestDriver::runTestAlloc<3, 1,0,0, 1, 1,2,2>();
        TupleTestDriver::runTestAlloc<3, 1,0,0, 2, 0,0,2>();
        TupleTestDriver::runTestAlloc<3, 1,0,0, 2, 0,1,2>();
        TupleTestDriver::runTestAlloc<3, 1,0,0, 2, 1,0,2>();
        TupleTestDriver::runTestAlloc<3, 1,0,0, 2, 1,1,2>();
        TupleTestDriver::runTestAlloc<3, 1,0,0, 3, 0,0,0>();
        TupleTestDriver::runTestAlloc<3, 1,0,0, 3, 0,0,1>();
        TupleTestDriver::runTestAlloc<3, 1,0,0, 3, 0,1,0>();
        TupleTestDriver::runTestAlloc<3, 1,0,0, 3, 0,1,1>();
        TupleTestDriver::runTestAlloc<3, 1,0,0, 3, 1,0,0>();
        TupleTestDriver::runTestAlloc<3, 1,0,0, 3, 1,0,1>();
        TupleTestDriver::runTestAlloc<3, 1,0,0, 3, 1,1,0>();
        TupleTestDriver::runTestAlloc<3, 1,0,0, 3, 1,1,1>();
        TupleTestDriver::runTestAlloc<3, 1,0,1, 0, 2,2,2>();
        TupleTestDriver::runTestAlloc<3, 1,0,1, 1, 0,2,2>();
        TupleTestDriver::runTestAlloc<3, 1,0,1, 1, 1,2,2>();
        TupleTestDriver::runTestAlloc<3, 1,0,1, 2, 0,0,2>();
        TupleTestDriver::runTestAlloc<3, 1,0,1, 2, 0,1,2>();
        TupleTestDriver::runTestAlloc<3, 1,0,1, 2, 1,0,2>();
        TupleTestDriver::runTestAlloc<3, 1,0,1, 2, 1,1,2>();
        TupleTestDriver::runTestAlloc<3, 1,0,1, 3, 0,0,0>();
        TupleTestDriver::runTestAlloc<3, 1,0,1, 3, 0,0,1>();
        TupleTestDriver::runTestAlloc<3, 1,0,1, 3, 0,1,0>();
        TupleTestDriver::runTestAlloc<3, 1,0,1, 3, 0,1,1>();
        TupleTestDriver::runTestAlloc<3, 1,0,1, 3, 1,0,0>();
        TupleTestDriver::runTestAlloc<3, 1,0,1, 3, 1,0,1>();
        TupleTestDriver::runTestAlloc<3, 1,0,1, 3, 1,1,0>();
        TupleTestDriver::runTestAlloc<3, 1,0,1, 3, 1,1,1>();
        TupleTestDriver::runTestAlloc<3, 1,1,0, 0, 2,2,2>();
        TupleTestDriver::runTestAlloc<3, 1,1,0, 1, 0,2,2>();
        TupleTestDriver::runTestAlloc<3, 1,1,0, 1, 1,2,2>();
        TupleTestDriver::runTestAlloc<3, 1,1,0, 2, 0,0,2>();
        TupleTestDriver::runTestAlloc<3, 1,1,0, 2, 0,1,2>();
        TupleTestDriver::runTestAlloc<3, 1,1,0, 2, 1,0,2>();
        TupleTestDriver::runTestAlloc<3, 1,1,0, 2, 1,1,2>();
        TupleTestDriver::runTestAlloc<3, 1,1,0, 3, 0,0,0>();
        TupleTestDriver::runTestAlloc<3, 1,1,0, 3, 0,0,1>();
        TupleTestDriver::runTestAlloc<3, 1,1,0, 3, 0,1,0>();
        TupleTestDriver::runTestAlloc<3, 1,1,0, 3, 0,1,1>();
        TupleTestDriver::runTestAlloc<3, 1,1,0, 3, 1,0,0>();
        TupleTestDriver::runTestAlloc<3, 1,1,0, 3, 1,0,1>();
        TupleTestDriver::runTestAlloc<3, 1,1,0, 3, 1,1,0>();
        TupleTestDriver::runTestAlloc<3, 1,1,0, 3, 1,1,1>();
        TupleTestDriver::runTestAlloc<3, 1,1,1, 0, 2,2,2>();
        TupleTestDriver::runTestAlloc<3, 1,1,1, 1, 0,2,2>();
        TupleTestDriver::runTestAlloc<3, 1,1,1, 1, 1,2,2>();
        TupleTestDriver::runTestAlloc<3, 1,1,1, 2, 0,0,2>();
        TupleTestDriver::runTestAlloc<3, 1,1,1, 2, 0,1,2>();
        TupleTestDriver::runTestAlloc<3, 1,1,1, 2, 1,0,2>();
        TupleTestDriver::runTestAlloc<3, 1,1,1, 2, 1,1,2>();
        TupleTestDriver::runTestAlloc<3, 1,1,1, 3, 0,0,0>();
        TupleTestDriver::runTestAlloc<3, 1,1,1, 3, 0,0,1>();
        TupleTestDriver::runTestAlloc<3, 1,1,1, 3, 0,1,0>();
        TupleTestDriver::runTestAlloc<3, 1,1,1, 3, 0,1,1>();
        TupleTestDriver::runTestAlloc<3, 1,1,1, 3, 1,0,0>();
        TupleTestDriver::runTestAlloc<3, 1,1,1, 3, 1,0,1>();
        TupleTestDriver::runTestAlloc<3, 1,1,1, 3, 1,1,0>();
        TupleTestDriver::runTestAlloc<3, 1,1,1, 3, 1,1,1>();

        TupleTestDriver::runTestNoAlloc<0, 2,2,2, 0, 2,2,2>();
        TupleTestDriver::runTestNoAlloc<0, 2,2,2, 1, 0,2,2>();
        TupleTestDriver::runTestNoAlloc<0, 2,2,2, 1, 1,2,2>();
        TupleTestDriver::runTestNoAlloc<0, 2,2,2, 2, 0,0,2>();
        TupleTestDriver::runTestNoAlloc<0, 2,2,2, 2, 0,1,2>();
        TupleTestDriver::runTestNoAlloc<0, 2,2,2, 2, 1,0,2>();
        TupleTestDriver::runTestNoAlloc<0, 2,2,2, 2, 1,1,2>();
        TupleTestDriver::runTestNoAlloc<0, 2,2,2, 3, 0,0,0>();
        TupleTestDriver::runTestNoAlloc<0, 2,2,2, 3, 0,0,1>();
        TupleTestDriver::runTestNoAlloc<0, 2,2,2, 3, 0,1,0>();
        TupleTestDriver::runTestNoAlloc<0, 2,2,2, 3, 0,1,1>();
        TupleTestDriver::runTestNoAlloc<0, 2,2,2, 3, 1,0,0>();
        TupleTestDriver::runTestNoAlloc<0, 2,2,2, 3, 1,0,1>();
        TupleTestDriver::runTestNoAlloc<0, 2,2,2, 3, 1,1,0>();
        TupleTestDriver::runTestNoAlloc<0, 2,2,2, 3, 1,1,1>();
        TupleTestDriver::runTestNoAlloc<1, 0,2,2, 0, 2,2,2>();
        TupleTestDriver::runTestNoAlloc<1, 0,2,2, 1, 0,2,2>();
        TupleTestDriver::runTestNoAlloc<1, 0,2,2, 1, 1,2,2>();
        TupleTestDriver::runTestNoAlloc<1, 0,2,2, 2, 0,0,2>();
        TupleTestDriver::runTestNoAlloc<1, 0,2,2, 2, 0,1,2>();
        TupleTestDriver::runTestNoAlloc<1, 0,2,2, 2, 1,0,2>();
        TupleTestDriver::runTestNoAlloc<1, 0,2,2, 2, 1,1,2>();
        TupleTestDriver::runTestNoAlloc<1, 0,2,2, 3, 0,0,0>();
        TupleTestDriver::runTestNoAlloc<1, 0,2,2, 3, 0,0,1>();
        TupleTestDriver::runTestNoAlloc<1, 0,2,2, 3, 0,1,0>();
        TupleTestDriver::runTestNoAlloc<1, 0,2,2, 3, 0,1,1>();
        TupleTestDriver::runTestNoAlloc<1, 0,2,2, 3, 1,0,0>();
        TupleTestDriver::runTestNoAlloc<1, 0,2,2, 3, 1,0,1>();
        TupleTestDriver::runTestNoAlloc<1, 0,2,2, 3, 1,1,0>();
        TupleTestDriver::runTestNoAlloc<1, 0,2,2, 3, 1,1,1>();
        TupleTestDriver::runTestNoAlloc<1, 1,2,2, 0, 2,2,2>();
        TupleTestDriver::runTestNoAlloc<1, 1,2,2, 1, 0,2,2>();
        TupleTestDriver::runTestNoAlloc<1, 1,2,2, 1, 1,2,2>();
        TupleTestDriver::runTestNoAlloc<1, 1,2,2, 2, 0,0,2>();
        TupleTestDriver::runTestNoAlloc<1, 1,2,2, 2, 0,1,2>();
        TupleTestDriver::runTestNoAlloc<1, 1,2,2, 2, 1,0,2>();
        TupleTestDriver::runTestNoAlloc<1, 1,2,2, 2, 1,1,2>();
        TupleTestDriver::runTestNoAlloc<1, 1,2,2, 3, 0,0,0>();
        TupleTestDriver::runTestNoAlloc<1, 1,2,2, 3, 0,0,1>();
        TupleTestDriver::runTestNoAlloc<1, 1,2,2, 3, 0,1,0>();
        TupleTestDriver::runTestNoAlloc<1, 1,2,2, 3, 0,1,1>();
        TupleTestDriver::runTestNoAlloc<1, 1,2,2, 3, 1,0,0>();
        TupleTestDriver::runTestNoAlloc<1, 1,2,2, 3, 1,0,1>();
        TupleTestDriver::runTestNoAlloc<1, 1,2,2, 3, 1,1,0>();
        TupleTestDriver::runTestNoAlloc<1, 1,2,2, 3, 1,1,1>();
        TupleTestDriver::runTestNoAlloc<2, 0,0,2, 0, 2,2,2>();
        TupleTestDriver::runTestNoAlloc<2, 0,0,2, 1, 0,2,2>();
        TupleTestDriver::runTestNoAlloc<2, 0,0,2, 1, 1,2,2>();
        TupleTestDriver::runTestNoAlloc<2, 0,0,2, 2, 0,0,2>();
        TupleTestDriver::runTestNoAlloc<2, 0,0,2, 2, 0,1,2>();
        TupleTestDriver::runTestNoAlloc<2, 0,0,2, 2, 1,0,2>();
        TupleTestDriver::runTestNoAlloc<2, 0,0,2, 2, 1,1,2>();
        TupleTestDriver::runTestNoAlloc<2, 0,0,2, 3, 0,0,0>();
        TupleTestDriver::runTestNoAlloc<2, 0,0,2, 3, 0,0,1>();
        TupleTestDriver::runTestNoAlloc<2, 0,0,2, 3, 0,1,0>();
        TupleTestDriver::runTestNoAlloc<2, 0,0,2, 3, 0,1,1>();
        TupleTestDriver::runTestNoAlloc<2, 0,0,2, 3, 1,0,0>();
        TupleTestDriver::runTestNoAlloc<2, 0,0,2, 3, 1,0,1>();
        TupleTestDriver::runTestNoAlloc<2, 0,0,2, 3, 1,1,0>();
        TupleTestDriver::runTestNoAlloc<2, 0,0,2, 3, 1,1,1>();
        TupleTestDriver::runTestNoAlloc<2, 0,1,2, 0, 2,2,2>();
        TupleTestDriver::runTestNoAlloc<2, 0,1,2, 1, 0,2,2>();
        TupleTestDriver::runTestNoAlloc<2, 0,1,2, 1, 1,2,2>();
        TupleTestDriver::runTestNoAlloc<2, 0,1,2, 2, 0,0,2>();
        TupleTestDriver::runTestNoAlloc<2, 0,1,2, 2, 0,1,2>();
        TupleTestDriver::runTestNoAlloc<2, 0,1,2, 2, 1,0,2>();
        TupleTestDriver::runTestNoAlloc<2, 0,1,2, 2, 1,1,2>();
        TupleTestDriver::runTestNoAlloc<2, 0,1,2, 3, 0,0,0>();
        TupleTestDriver::runTestNoAlloc<2, 0,1,2, 3, 0,0,1>();
        TupleTestDriver::runTestNoAlloc<2, 0,1,2, 3, 0,1,0>();
        TupleTestDriver::runTestNoAlloc<2, 0,1,2, 3, 0,1,1>();
        TupleTestDriver::runTestNoAlloc<2, 0,1,2, 3, 1,0,0>();
        TupleTestDriver::runTestNoAlloc<2, 0,1,2, 3, 1,0,1>();
        TupleTestDriver::runTestNoAlloc<2, 0,1,2, 3, 1,1,0>();
        TupleTestDriver::runTestNoAlloc<2, 0,1,2, 3, 1,1,1>();
        TupleTestDriver::runTestNoAlloc<2, 1,0,2, 0, 2,2,2>();
        TupleTestDriver::runTestNoAlloc<2, 1,0,2, 1, 0,2,2>();
        TupleTestDriver::runTestNoAlloc<2, 1,0,2, 1, 1,2,2>();
        TupleTestDriver::runTestNoAlloc<2, 1,0,2, 2, 0,0,2>();
        TupleTestDriver::runTestNoAlloc<2, 1,0,2, 2, 0,1,2>();
        TupleTestDriver::runTestNoAlloc<2, 1,0,2, 2, 1,0,2>();
        TupleTestDriver::runTestNoAlloc<2, 1,0,2, 2, 1,1,2>();
        TupleTestDriver::runTestNoAlloc<2, 1,0,2, 3, 0,0,0>();
        TupleTestDriver::runTestNoAlloc<2, 1,0,2, 3, 0,0,1>();
        TupleTestDriver::runTestNoAlloc<2, 1,0,2, 3, 0,1,0>();
        TupleTestDriver::runTestNoAlloc<2, 1,0,2, 3, 0,1,1>();
        TupleTestDriver::runTestNoAlloc<2, 1,0,2, 3, 1,0,0>();
        TupleTestDriver::runTestNoAlloc<2, 1,0,2, 3, 1,0,1>();
        TupleTestDriver::runTestNoAlloc<2, 1,0,2, 3, 1,1,0>();
        TupleTestDriver::runTestNoAlloc<2, 1,0,2, 3, 1,1,1>();
        TupleTestDriver::runTestNoAlloc<2, 1,1,2, 0, 2,2,2>();
        TupleTestDriver::runTestNoAlloc<2, 1,1,2, 1, 0,2,2>();
        TupleTestDriver::runTestNoAlloc<2, 1,1,2, 1, 1,2,2>();
        TupleTestDriver::runTestNoAlloc<2, 1,1,2, 2, 0,0,2>();
        TupleTestDriver::runTestNoAlloc<2, 1,1,2, 2, 0,1,2>();
        TupleTestDriver::runTestNoAlloc<2, 1,1,2, 2, 1,0,2>();
        TupleTestDriver::runTestNoAlloc<2, 1,1,2, 2, 1,1,2>();
        TupleTestDriver::runTestNoAlloc<2, 1,1,2, 3, 0,0,0>();
        TupleTestDriver::runTestNoAlloc<2, 1,1,2, 3, 0,0,1>();
        TupleTestDriver::runTestNoAlloc<2, 1,1,2, 3, 0,1,0>();
        TupleTestDriver::runTestNoAlloc<2, 1,1,2, 3, 0,1,1>();
        TupleTestDriver::runTestNoAlloc<2, 1,1,2, 3, 1,0,0>();
        TupleTestDriver::runTestNoAlloc<2, 1,1,2, 3, 1,0,1>();
        TupleTestDriver::runTestNoAlloc<2, 1,1,2, 3, 1,1,0>();
        TupleTestDriver::runTestNoAlloc<2, 1,1,2, 3, 1,1,1>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,0, 0, 2,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,0, 1, 0,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,0, 1, 1,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,0, 2, 0,0,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,0, 2, 0,1,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,0, 2, 1,0,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,0, 2, 1,1,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,0, 3, 0,0,0>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,0, 3, 0,0,1>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,0, 3, 0,1,0>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,0, 3, 0,1,1>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,0, 3, 1,0,0>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,0, 3, 1,0,1>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,0, 3, 1,1,0>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,0, 3, 1,1,1>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,1, 0, 2,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,1, 1, 0,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,1, 1, 1,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,1, 2, 0,0,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,1, 2, 0,1,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,1, 2, 1,0,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,1, 2, 1,1,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,1, 3, 0,0,0>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,1, 3, 0,0,1>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,1, 3, 0,1,0>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,1, 3, 0,1,1>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,1, 3, 1,0,0>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,1, 3, 1,0,1>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,1, 3, 1,1,0>();
        TupleTestDriver::runTestNoAlloc<3, 0,0,1, 3, 1,1,1>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,0, 0, 2,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,0, 1, 0,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,0, 1, 1,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,0, 2, 0,0,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,0, 2, 0,1,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,0, 2, 1,0,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,0, 2, 1,1,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,0, 3, 0,0,0>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,0, 3, 0,0,1>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,0, 3, 0,1,0>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,0, 3, 0,1,1>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,0, 3, 1,0,0>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,0, 3, 1,0,1>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,0, 3, 1,1,0>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,0, 3, 1,1,1>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,1, 0, 2,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,1, 1, 0,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,1, 1, 1,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,1, 2, 0,0,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,1, 2, 0,1,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,1, 2, 1,0,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,1, 2, 1,1,2>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,1, 3, 0,0,0>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,1, 3, 0,0,1>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,1, 3, 0,1,0>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,1, 3, 0,1,1>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,1, 3, 1,0,0>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,1, 3, 1,0,1>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,1, 3, 1,1,0>();
        TupleTestDriver::runTestNoAlloc<3, 0,1,1, 3, 1,1,1>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,0, 0, 2,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,0, 1, 0,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,0, 1, 1,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,0, 2, 0,0,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,0, 2, 0,1,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,0, 2, 1,0,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,0, 2, 1,1,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,0, 3, 0,0,0>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,0, 3, 0,0,1>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,0, 3, 0,1,0>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,0, 3, 0,1,1>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,0, 3, 1,0,0>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,0, 3, 1,0,1>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,0, 3, 1,1,0>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,0, 3, 1,1,1>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,1, 0, 2,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,1, 1, 0,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,1, 1, 1,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,1, 2, 0,0,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,1, 2, 0,1,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,1, 2, 1,0,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,1, 2, 1,1,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,1, 3, 0,0,0>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,1, 3, 0,0,1>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,1, 3, 0,1,0>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,1, 3, 0,1,1>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,1, 3, 1,0,0>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,1, 3, 1,0,1>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,1, 3, 1,1,0>();
        TupleTestDriver::runTestNoAlloc<3, 1,0,1, 3, 1,1,1>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,0, 0, 2,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,0, 1, 0,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,0, 1, 1,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,0, 2, 0,0,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,0, 2, 0,1,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,0, 2, 1,0,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,0, 2, 1,1,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,0, 3, 0,0,0>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,0, 3, 0,0,1>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,0, 3, 0,1,0>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,0, 3, 0,1,1>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,0, 3, 1,0,0>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,0, 3, 1,0,1>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,0, 3, 1,1,0>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,0, 3, 1,1,1>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,1, 0, 2,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,1, 1, 0,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,1, 1, 1,2,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,1, 2, 0,0,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,1, 2, 0,1,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,1, 2, 1,0,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,1, 2, 1,1,2>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,1, 3, 0,0,0>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,1, 3, 0,0,1>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,1, 3, 0,1,0>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,1, 3, 0,1,1>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,1, 3, 1,0,0>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,1, 3, 1,0,1>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,1, 3, 1,1,0>();
        TupleTestDriver::runTestNoAlloc<3, 1,1,1, 3, 1,1,1>();
#endif
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING MOVE and COPY ASSIGNMENT
        //
        // Concerns:
        //: 1 Construct two pairs, a 'ToPair' and a 'FromPair' (both types
        //:   defined and docced within the 'TestDriver' class) with different
        //:   types.



        // template <class U1, class U2> pair& operator=(pair<U1, U2>&&);
        // template <class U1, class U2> pair& operator=(const pair<U1, U2>&);
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase12_move,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase12_move,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType),
                      BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType));

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase12_copy,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase12_copy,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

                   // BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType) - copy assign is needed
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING MOVE CONSTRUCTOR WITH ONE ELEMENT MOVED, ONE COPIED
        //
        // Concerns:
        //: 1 That the constructors work properly in the case where one element
        //:   is moved and the other copied.
        //:
        //: 2 That the constructors work properly in the case where two
        //:   elements to be copied are passed.
        //:
        //: 3 That the constructors work properly in the case where a pair to
        //:   be copied is passed.
        //
        // Plan:
        //: 1 In each case, first construct the source pair 'fp' using
        //:   'initPair' and attach a proctor to it.  In between every pair of
        //:   c'tor calls, destroy 'fp' through the proctor, then call
        //:   'initPair' to initialize 'fp' again with a new value.
        //:
        //: 2 After each c'tor call, verify the value of the constructed
        //:   object, and which allocator it uses if allocators were passed a
        //:   construction, and very memory consumption by the allocators was
        //:   as it should be.
        //:
        //: 3 Call both of the constructors taking a const ref as one argument
        //:   and an rvalue as the other.
        //:
        //: 4 Call the constructor taking const refs to both members.
        //:
        //: 5 Call the constructor taking a const ref to a pair.
        //
        // Testing:
        //   template <class U1, class U2> pair(U1&& a, const U2& b);
        //   template <class U1, class U2> pair(const U1& a, U2&& b);
        //   template <class U1, class U2> pair(const U1& a, const U2& b);
        //   template <class U1, class U2> pair(const U1& a, U2&& b, Alloc a);
        //   template <class U1, class U2> pair(U1&& a, const U2& b, Alloc a);
        //   template <class U1, class U2> pair(const U1& a, const U2& b, A a);
        //   template <class U1, class U2> pair(const pair<U1, U2>& pr);
        //   template <class U1, class U2> pair(const pair<U1, U2>& pr, A a);
        //   pair(first_type&& a, const second_type& b);
        //   pair(const first_type& a, second_type&& b);
        //   pair(const first_type& a, const second_type& b);
        //   pair(const first_type& a, second_type&& b, Alloc a);
        //   pair(first_type&& a, const second_type& b, Alloc a);
        //   pair(const first_type& a, const second_type& b, A a);
        //   pair(const pair<first_type, second_type>& pr);
        //   pair(const pair<first_type, second_type>& pr, Alloc a);
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase11,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType));

        // Cannot do 'BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType)' -- need copy c'tor.
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING MOVE CONSTRUCTOR WITH 2 INDEPENDENTLY MOVED ELEMENTS
        //
        // Concerns:
        //: 1 That the c'tors taking two arguments by rvalue work properly.
        //
        // Plan:
        //: 1 Call the c'tor.
        //:
        //: 2 Check the state of the sources.
        //:
        //: 3 Check the state of the result.
        //:
        //: 4 Check the memory allocation behavior.
        //:
        //: 5 If either of the members of the pair allocate memory, repeat the
        //:   process:
        //:   o with no allocator passed to the c'tor
        //:   o with an allocator passed to the c'tor that matches that used to
        //:     create the sources
        //:   o with an allocator passed to the c'tor that does not match that
        //:     used to create the sources
        //
        // Testing:
        //   template <class U1, class U2> pair(U1&& a, U2&& b);
        //   template <class U1, class U2> pair(U1&& a, U2&& b, Alloc a);
        //   pair(first_type&& a, second_type&& b);
        //   pair(first_type&& a, second_type&& b, AllocatorPtr a);
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase10,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase10,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType),
                      BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType));
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING MOVE CONSTRUCTOR FOR DIFFERENT TYPE / SAME TYPE PAIR
        //
        // Concerns:
        //: 1 That the c'tors taking a pair by rvalue work properly.
        //
        // Plan:
        //: 1 Call the c'tor.
        //:
        //: 2 Check the state of the sources.
        //:
        //: 3 Check the state of the result.
        //:
        //: 4 Check the memory allocation behavior.
        //:
        //: 5 If either of the members of the pair allocate memory, repeat the
        //:   process:
        //:   o with no allocator passed to the c'tor
        //:   o with an allocator passed to the c'tor that matches that used to
        //:     create the sources
        //:   o with an allocator passed to the c'tor that does not match that
        //:     used to create the sources
        //
        // Testing:
        //   template <class U1, class U2> pair(pair<U1, U2>&&)
        //   template <class U1, class U2> pair(pair<U1, U2>&&, AllocatorPtr)
        //   pair(pair&&)
        //   pair(pair&&, AllocatorPtr)
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase9,
                      BAD_MOVE_GUARD(bsltf::MovableTestType),
                      BAD_MOVE_GUARD(bsltf::MovableAllocTestType),
                      BAD_MOVE_GUARD(bsltf::MoveOnlyAllocTestType));
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT TO 'pair' OF REFERENCES
        //
        // Concerns:
        //: 1 The assignment operator for a 'pair' of references is well-formed
        //:   and assigns to the original objects referred to by the 'pair'.
        //
        // Plan:
        //: 1 Create a target 'pair' holding:
        //:   a) a reference in 'first' and a value for 'second'
        //:   b) a reference in 'second' and a value for 'first'
        //:   c) a reference in both 'first' and 'second'
        //:
        //: 2 Create a value pair of values corresponding to the types used by
        //:   the target pairs above.
        //:
        //: 3 Assign from the value pair to the target pair, and confirm:
        //:   a) the address of any reference members of the target pair does
        //:      not change.
        //:   b) the value of any object referred to by the target pair has
        //:      changed
        //:   c) the values held by the value-pair have not changed
        //:
        //: 4 Repeat step (3) with a reference pair, holding two references
        //:   rather than two values.
        //
        // Testing:
        //   pair& operator=(const pair<U1, U2>& rhs)
        //   Concern: Can assign to a 'pair' of references
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ASSIGNMENT TO 'pair' OF REFERENCES"
                            "\n==========================================\n");

        // data for pairs

        int    i1 = 13;
        int    i2 = 42;
        int    i3 = 91;
        double d1 = 3.14159;
        double d2 = 2.71828;
        double d3 = 1.61803;

        const int    ORIGINAL_I1 = i1;
        const int    ORIGINAL_I2 = i2;
        const int    ORIGINAL_I3 = i3;
        const double ORIGINAL_D1 = d1;
        const double ORIGINAL_D2 = d2;
        const double ORIGINAL_D3 = d3;

        // Construct the pairs to assign-from

        bsl::pair<int, double> value(i1, d1);
        bsl::pair<int, double> reference(i2, d2);

        // test assign from 'value'

        {
            bsl::pair<int&, double > target(i3, d3);

            int    *pi = &target.first;
            double *pd = &target.second;

            ASSERTV(&i3,  pi, &i3 == pi);

            target = value;

            ASSERTV(&target.first,  pi, &target.first  == pi);
            ASSERTV(&target.second, pd, &target.second == pd);

            ASSERTV(ORIGINAL_I1, target.first,  ORIGINAL_I1 == target.first );
            ASSERTV(ORIGINAL_D1, target.second, ORIGINAL_D1 == target.second);

            ASSERTV(ORIGINAL_I1, value.first,  ORIGINAL_I1 == value.first );
            ASSERTV(ORIGINAL_D1, value.second, ORIGINAL_D1 == value.second);

            ASSERTV(ORIGINAL_I1, i1, ORIGINAL_I1 == i1);
            ASSERTV(ORIGINAL_D1, d1, ORIGINAL_D1 == d1);
            ASSERTV(ORIGINAL_I1, i3, ORIGINAL_I1 == i3);
            ASSERTV(ORIGINAL_D3, d3, ORIGINAL_D3 == d3);

            target = target;

            ASSERTV(&target.first,  pi, &target.first  == pi);
            ASSERTV(&target.second, pd, &target.second == pd);

            ASSERTV(ORIGINAL_I1, target.first,  ORIGINAL_I1 == target.first );
            ASSERTV(ORIGINAL_D1, target.second, ORIGINAL_D1 == target.second);

            ASSERTV(ORIGINAL_I1, value.first,  ORIGINAL_I1 == value.first );
            ASSERTV(ORIGINAL_D1, value.second, ORIGINAL_D1 == value.second);
        }

        i1 = ORIGINAL_I1;
        i2 = ORIGINAL_I2;
        i3 = ORIGINAL_I3;
        d1 = ORIGINAL_D1;
        d2 = ORIGINAL_D2;
        d3 = ORIGINAL_D3;

        {
            bsl::pair<int,  double&> target(i3, d3);

            int    *pi = &target.first;
            double *pd = &target.second;

            ASSERTV(&d3,  pd, &d3 == pd);

            target = value;

            ASSERTV(&target.first,  pi, &target.first  == pi);
            ASSERTV(&target.second, pd, &target.second == pd);

            ASSERTV(ORIGINAL_I1, target.first,  ORIGINAL_I1 == target.first );
            ASSERTV(ORIGINAL_D1, target.second, ORIGINAL_D1 == target.second);

            ASSERTV(ORIGINAL_I1, value.first,  ORIGINAL_I1 == value.first );
            ASSERTV(ORIGINAL_D1, value.second, ORIGINAL_D1 == value.second);

            ASSERTV(ORIGINAL_I1, i1, ORIGINAL_I1 == i1);
            ASSERTV(ORIGINAL_D1, d1, ORIGINAL_D1 == d1);
            ASSERTV(ORIGINAL_I3, i3, ORIGINAL_I3 == i3);
            ASSERTV(ORIGINAL_D1, d3, ORIGINAL_D1 == d3);

            target = target;

            ASSERTV(&target.first,  pi, &target.first  == pi);
            ASSERTV(&target.second, pd, &target.second == pd);

            ASSERTV(ORIGINAL_I1, target.first,  ORIGINAL_I1 == target.first );
            ASSERTV(ORIGINAL_D1, target.second, ORIGINAL_D1 == target.second);

            ASSERTV(ORIGINAL_I1, value.first,  ORIGINAL_I1 == value.first );
            ASSERTV(ORIGINAL_D1, value.second, ORIGINAL_D1 == value.second);
        }

        i1 = ORIGINAL_I1;
        i2 = ORIGINAL_I2;
        i3 = ORIGINAL_I3;
        d1 = ORIGINAL_D1;
        d2 = ORIGINAL_D2;
        d3 = ORIGINAL_D3;

        {
            bsl::pair<int&, double&> target(i3, d3);

            int    *pi = &target.first;
            double *pd = &target.second;

            ASSERTV(&i3,  pi, &i3 == pi);
            ASSERTV(&d3,  pd, &d3 == pd);

            target = value;

            ASSERTV(&target.first,  pi, &target.first  == pi);
            ASSERTV(&target.second, pd, &target.second == pd);

            ASSERTV(ORIGINAL_I1, target.first,  ORIGINAL_I1 == target.first );
            ASSERTV(ORIGINAL_D1, target.second, ORIGINAL_D1 == target.second);

            ASSERTV(ORIGINAL_I1, value.first,  ORIGINAL_I1 == value.first );
            ASSERTV(ORIGINAL_D1, value.second, ORIGINAL_D1 == value.second);

            ASSERTV(ORIGINAL_I1, i1, ORIGINAL_I1 == i1);
            ASSERTV(ORIGINAL_D1, d1, ORIGINAL_D1 == d1);
            ASSERTV(ORIGINAL_I1, i3, ORIGINAL_I1 == i3);
            ASSERTV(ORIGINAL_D1, d3, ORIGINAL_D1 == d3);

            target = target;

            ASSERTV(&target.first,  pi, &target.first  == pi);
            ASSERTV(&target.second, pd, &target.second == pd);

            ASSERTV(ORIGINAL_I1, target.first,  ORIGINAL_I1 == target.first );
            ASSERTV(ORIGINAL_D1, target.second, ORIGINAL_D1 == target.second);

            ASSERTV(ORIGINAL_I1, value.first,  ORIGINAL_I1 == value.first );
            ASSERTV(ORIGINAL_D1, value.second, ORIGINAL_D1 == value.second);
        }

        i1 = ORIGINAL_I1;
        i2 = ORIGINAL_I2;
        i3 = ORIGINAL_I3;
        d1 = ORIGINAL_D1;
        d2 = ORIGINAL_D2;
        d3 = ORIGINAL_D3;


        // test assign from 'reference'

        {
            bsl::pair<int&, double > target(i3, d3);

            int    *pi = &target.first;
            double *pd = &target.second;

            ASSERTV(&i3,  pi, &i3 == pi);

            target = reference;

            ASSERTV(&target.first,  pi, &target.first  == pi);
            ASSERTV(&target.second, pd, &target.second == pd);

            ASSERTV(ORIGINAL_I2, target.first,  ORIGINAL_I2 == target.first );
            ASSERTV(ORIGINAL_D2, target.second, ORIGINAL_D2 == target.second);

            ASSERTV(ORIGINAL_I2,   reference.first,
                    ORIGINAL_I2 == reference.first );
            ASSERTV(ORIGINAL_D2,   reference.second,
                    ORIGINAL_D2 == reference.second);

            ASSERTV(ORIGINAL_I2, i2, ORIGINAL_I2 == i2);
            ASSERTV(ORIGINAL_D2, d2, ORIGINAL_D2 == d2);
            ASSERTV(ORIGINAL_I2, i3, ORIGINAL_I2 == i3);
            ASSERTV(ORIGINAL_D3, d3, ORIGINAL_D3 == d3);
        }

        i1 = ORIGINAL_I1;
        i2 = ORIGINAL_I2;
        i3 = ORIGINAL_I3;
        d1 = ORIGINAL_D1;
        d2 = ORIGINAL_D2;
        d3 = ORIGINAL_D3;

        {
            bsl::pair<int,  double&> target(i3, d3);

            int    *pi = &target.first;
            double *pd = &target.second;

            ASSERTV(&d3,  pd, &d3 == pd);

            target = reference;

            ASSERTV(&target.first,  pi, &target.first  == pi);
            ASSERTV(&target.second, pd, &target.second == pd);

            ASSERTV(ORIGINAL_I2, target.first,  ORIGINAL_I2 == target.first );
            ASSERTV(ORIGINAL_D2, target.second, ORIGINAL_D2 == target.second);

            ASSERTV(ORIGINAL_I2,   reference.first,
                    ORIGINAL_I2 == reference.first );
            ASSERTV(ORIGINAL_D2,   reference.second,
                    ORIGINAL_D2 == reference.second);

            ASSERTV(ORIGINAL_I2, i2, ORIGINAL_I2 == i2);
            ASSERTV(ORIGINAL_D2, d2, ORIGINAL_D2 == d2);
            ASSERTV(ORIGINAL_I3, i3, ORIGINAL_I3 == i3);
            ASSERTV(ORIGINAL_D2, d3, ORIGINAL_D2 == d3);
        }

        i1 = ORIGINAL_I1;
        i2 = ORIGINAL_I2;
        i3 = ORIGINAL_I3;
        d1 = ORIGINAL_D1;
        d2 = ORIGINAL_D2;
        d3 = ORIGINAL_D3;

        {
            bsl::pair<int&, double&> target(i3, d3);

            int    *pi = &target.first;
            double *pd = &target.second;

            ASSERTV(&i3,  pi, &i3 == pi);
            ASSERTV(&d3,  pd, &d3 == pd);

            target = reference;

            ASSERTV(&target.first,  pi, &target.first  == pi);
            ASSERTV(&target.second, pd, &target.second == pd);

            ASSERTV(ORIGINAL_I2, target.first,  ORIGINAL_I2 == target.first );
            ASSERTV(ORIGINAL_D2, target.second, ORIGINAL_D2 == target.second);

            ASSERTV(ORIGINAL_I2,   reference.first,
                    ORIGINAL_I2 == reference.first );
            ASSERTV(ORIGINAL_D2,   reference.second,
                    ORIGINAL_D2 == reference.second);

            ASSERTV(ORIGINAL_I2, i2, ORIGINAL_I2 == i2);
            ASSERTV(ORIGINAL_D2, d2, ORIGINAL_D2 == d2);
            ASSERTV(ORIGINAL_I2, i3, ORIGINAL_I2 == i3);
            ASSERTV(ORIGINAL_D2, d3, ORIGINAL_D2 == d3);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TEST FORMING A POINTER-TO-DATA-MEMBER
        //
        // Concerns:
        //: 1 We can use pointer to members to access both 'first' and 'second'
        //
        // Plan:
        //: 1 Create pointer to member to both 'first' and 'second' and check
        //:   that the behavior is as expected
        //
        // Testing:
        //   Concern: Can create a pointer-to-member for 'first' and 'second'
        // --------------------------------------------------------------------
        if (verbose) printf("\nTEST FORMING A POINTER-TO-DATA-MEMBER"
                            "\n=====================================\n");

        {
            typedef bsl::pair<int,const char*> PairType;

            int         PairType::*pfirst  = &PairType::first;
            const char *PairType::*psecond = &PairType::second;

            PairType p(10, "test7");

            ASSERTV(&p.first,  &(p.*pfirst),  p.first   == (p.*pfirst));
            ASSERTV(&p.second, &(p.*psecond), p.second  == (p.*psecond));
        }

        {
            typedef bsl::pair<char, long double> PairType;

            char        PairType::*pfirst  = &PairType::first;
            long double PairType::*psecond = &PairType::second;

            PairType p('c', 3.14l);

            ASSERTV(&p.first,  &(p.*pfirst),  p.first   == (p.*pfirst));
            ASSERTV(&p.second, &(p.*psecond), p.second  == (p.*psecond));
        }

        {
            typedef bsl::pair<long double, char> PairType;

            long double PairType::*pfirst  = &PairType::first;
            char        PairType::*psecond = &PairType::second;

            PairType p(2.78l, 'd');

            ASSERTV(&p.first,  &(p.*pfirst),  p.first   == (p.*pfirst));
            ASSERTV(&p.second, &(p.*psecond), p.second  == (p.*psecond));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'hashAppend'
        //
        // Concerns:
        //: 1 Hashes different inputs differently
        //:
        //: 2 Hashes equal inputs identically
        //:
        //: 3 Works for const and non-const pairs, members
        //:
        //: 4 'hashAppend' for 'pair' correctly uses 'hashAppend' implemented
        //:   for the pair's template parameter types.
        //
        // Plan:
        //: 1 Create pairs, some equal and some not, some const, some not.
        //:   Hash them all. Compare hashes, identifying those that should be
        //:   equal and those that should not.  Call with different mixes of
        //:   constness, to verify that all compile. (C-1..3)
        //:
        //: 2 Create a 'hashAppend' for 'my_String', create a set of pairs
        //:   using 'my_String' values, and verify that pairs having the same
        //:   'my_String' value produce the same hash code. (C-4)
        //
        // Testing:
        //   hashAppend(HASHALG& hashAlg, const pair<T1,T2>&  input);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'hashAppend'"
                            "\n====================\n");

        typedef ::BloombergLP::bslh::Hash<> Hasher;
        typedef Hasher::result_type         HashType;

        {
            const char *ptr1 = "hello";
            const char *ptr2 = "goodbye";

            bsl::pair<int,const char*> p1(1,   ptr1);  // P-1
            bsl::pair<int,const char*> p2(1,   ptr1);
            bsl::pair<int,const char*> p3(100, ptr1);
            bsl::pair<int,const char*> p4(1,   ptr2);
            bsl::pair<int,const char*> p5(1,   ptr2);
            bsl::pair<int,const char*> p6(100, ptr2);

            bsl::pair<const int, const char *>      p7(1, ptr1);  // P-4
            const bsl::pair<int,const char * const> p8(1, ptr1);

            Hasher hasher;  // P-2
            HashType a1 = hasher(p1), a2 = hasher(p2), a3 = hasher(p3),
                     a4 = hasher(p4), a5 = hasher(p5), a6 = hasher(p6),
                     a7 = hasher(p7), a8 = hasher(p8);

            if (veryVerbose) {
                printf("\tHash of (%d,\"%s\") is %llx\n",
                       p1.first, p1.second, (unsigned long long) a1);
                printf("\tHash of (%d,\"%s\") is %llx\n",
                       p2.first, p2.second, (unsigned long long) a2);
                printf("\tHash of (%d,\"%s\") is %llx\n",
                       p3.first, p3.second, (unsigned long long) a3);
                printf("\tHash of (%d,\"%s\") is %llx\n",
                       p4.first, p4.second, (unsigned long long) a4);
                printf("\tHash of (%d,\"%s\") is %llx\n",
                       p5.first, p5.second, (unsigned long long) a5);
                printf("\tHash of (%d,\"%s\") is %llx\n",
                       p6.first, p6.second, (unsigned long long) a6);
                printf("\tHash of (%d,\"%s\") is %llx\n",
                       p7.first, p7.second, (unsigned long long) a7);
                printf("\tHash of (%d,\"%s\") is %llx\n",
                       p8.first, p8.second, (unsigned long long) a8);
            }

            // P-3

            ASSERT(a1 == a2);
            ASSERT(a1 != a3);
            ASSERT(a1 != a4);
            ASSERT(a1 != a5);
            ASSERT(a1 != a6);
            if (veryVerbose) {
                printf(
                  "\tp1=p2: %d, p1/p3: %d, p1/p4: %d, p1/p5: %d, p1/p6: %d\n",
                  int(a1 == a2), int(a1 != a3), int(a1 != a4),
                  int(a1 != a5), int(a1 != a6));
            }
            ASSERT(a2 != a3);
            ASSERT(a2 != a4);
            ASSERT(a2 != a5);
            ASSERT(a2 != a6);
            if (veryVerbose) {
                printf(
                   "\tp2/p3: %d, p2/p4: %d, p2/p5: %d, p2/p6: %d\n",
                   int(a2 != a3), int(a2 != a4), int(a2 != a5), int(a2 != a6));
            }
            ASSERT(a3 != a4);
            ASSERT(a3 != a5);
            ASSERT(a3 != a6);
            if (veryVerbose) {
                printf(
                   "\tp3/p4: %d, p3/p5: %d, p3/p6: %d\n",
                   int(a3 != a4), int(a3 != a5), int(a3 != a6));
            }
            ASSERT(a4 == a5);
            ASSERT(a4 != a6);
            if (veryVerbose) {
                printf(
                   "\tp4=p5: %d, p4/p6: %d\n",
                   int(a4 == a5), int(a4 != a6));
            }
            ASSERT(a5 != a6);
            if (veryVerbose) {
                printf(
                    "\tp5/p6: %d\n",
                    int(a5 != a6));
            }

            ASSERT(a7 == a8);
            ASSERT(a1 == a8);
            if (veryVerbose) {
                printf(
                    "\tp7=p8: %d, p1=p8: %d\n",
                    int(a7 == a8), int(a1 == a8));
            }
        }
        if (verbose) {
            printf("\tTesting hash on a pair with a user-defined type");
        }
        {
            bsl::pair<int, my_String> p1(1,   "stringA");  // P-1
            bsl::pair<int, my_String> p2(1,   "stringA");
            bsl::pair<int, my_String> p3(100, "stringA");
            bsl::pair<int, my_String> p4(1,   "stringB");
            bsl::pair<int, my_String> p5(1,   "stringB");
            bsl::pair<int, my_String> p6(100, "stringB");

            Hasher hasher;  // P-2
            HashType a1 = hasher(p1), a2 = hasher(p2), a3 = hasher(p3),
                     a4 = hasher(p4), a5 = hasher(p5), a6 = hasher(p6);

            ASSERT(a1 == a2);
            ASSERT(a1 != a3);
            ASSERT(a1 != a4);
            ASSERT(a1 != a5);
            ASSERT(a1 != a6);

            ASSERT(a2 != a3);
            ASSERT(a2 != a4);
            ASSERT(a2 != a5);
            ASSERT(a2 != a6);

            ASSERT(a3 != a4);
            ASSERT(a3 != a5);
            ASSERT(a3 != a6);

            ASSERT(a4 == a5);
            ASSERT(a4 != a6);

            ASSERT(a5 != a6);
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'swap'
        //
        // Concerns:
        // - 'swap' free function correctly swaps the values of 'pair::first'
        //   and 'pair::second' data fields.
        // - 'swap' member function correctly swaps the values of 'pair::first'
        //   and 'pair::second' data fields.
        // - If there is a 'swap' free functions defined for either of
        //   'pair::first_type' or 'pair::second_type' types, 'pair::swap'
        //   should use it (if the ADL lookup of 'swap' works correctly).
        // - Otherwise 'pair::swap' should use the default 'std::swap'.
        //
        // Plan:
        // - Create two types: one with 'swap' method and free function
        //   (TypeWithSwap), and another without 'swap' (TypeWithoutSwap).
        // - Instantiate 'pair' with TypeWithSwap as T1 and TypeWithoutSwap as
        //   T2, and the other way around.
        // - Test both 'swap' method and free functions on the two
        //   instantiations of 'pair' described above.
        //
        // Testing:
        //   void pair::swap(pair& rhs);
        //   void swap(pair& lhs, pair& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'swap'"
                            "\n==============\n");

        swapTestHelper<TypeWithSwapNamespace::TypeWithSwap, TypeWithoutSwap>();
        swapTestHelper<TypeWithoutSwap, TypeWithSwapNamespace::TypeWithSwap>();
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION CONSTRUCTORS
        //
        // Concerns:
        // - Can construct a 'bsl::pair' from a different instantiation of
        //   'pair' where each type is convertible.
        // - If either type uses a 'bslma::Allocator', then an allocator can
        //   be passed to the conversion constructor and is used to construct
        //   that member.
        // - If neither type uses a 'bslma::allocator', then any allocator
        //   argument is ignored.
        //
        // Plan:
        // - Construct 'pair<int, double>' from 'pair<char, int>'
        // - For each 'STRING' type in the list, 'my_String',
        //   'my_BslmaAllocArgStr', 'my_STLAllocArgStr', 'my_NoAllocStr':
        //   * Construct 'pair<STRING, int>' from 'pair<const char*, short>',
        //     without supplying an allocator on construction.
        //   * Construct 'pair<STRING, int>' from 'pair<const char*, short>',
        //     supplying a 'bslma::Allocator*' on construction.
        //   * Construct 'pair<int, STRING>' from 'pair<short, const char*>',
        //     without supplying an allocator on construction.
        //   * Construct 'pair<int, STRING>' from 'pair<short, const char*>',
        //     supplying a 'bslma::Allocator*' on construction.
        //   * Construct 'pair<STRING, STRING>' from 'pair<char*,const char*>',
        //     without supplying an allocator on construction.
        //   * Construct 'pair<STRING, STRING>' from 'pair<char*,const char*>',
        //     supplying a 'bslma::Allocator*' on construction.
        //   * Repeat the above except constructing 'bsl::pair from
        //     'native_std::pair'.
        // - When an allocator is not supplied on construction, verify that
        //   the correct default is used by the STRING in the constructed pair.
        // - When 'STRING' does not use a 'bslma::Allocator' and an allocator
        //   is supllied on construction, verify that the 'STRING' value
        //   in the constructed pair uses the appropriate default allocator.
        // - When 'STRING' does use a 'bslma::Allocator' and an allocator
        //   is supllied on construction, verify that the 'STRING' value
        //   in the constructed pair uses the supplied allocator.
        // - Verify that there are no memory leaks.
        //
        // Testing:
        //   pair(const pair<U1, U2>& rhs);
        //   pair(const pair<U1, U2>& rhs, AllocatorPtr basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONVERSION CONSTRUCTORS"
                            "\n===============================\n");

        bslma::TestAllocator ta1(veryVeryVerbose);
        bslma::TestAllocator ta2(veryVeryVerbose);
        bslma::TestAllocator& ta3 = *my_STLCharAlloc::defaultMechanism();

        {
            bslma::DefaultAllocatorGuard allocGuard(&ta2);

            bsl::pair<char, int> p1('A', 8), &P1 = p1;
            bsl::pair<int, double> p2(P1), &P2 = p2;
            ASSERT('A' == P2.first);
            ASSERT(8 == P2.second);

            ASSERT(0 == ta1.numBlocksInUse());
            ASSERT(0 == ta2.numBlocksInUse());
            ASSERT(0 == ta3.numBlocksInUse());
        }

        if (veryVerbose) printf("Convert to 'my_String'\n");
        testBslmaStringConversionCtor<my_String>();
        if (veryVerbose) printf("Convert to 'my_BslmaAllocArgStr'\n");
        testBslmaStringConversionCtor<my_BslmaAllocArgStr>();
        if (veryVerbose) printf("Convert to 'my_STLAllocArgStr'\n");
        testNonBslmaStringConversionCtor<my_STLAllocArgStr>();
        if (veryVerbose) printf("Convert to 'my_NoAllocString'\n");
        testNonBslmaStringConversionCtor<my_NoAllocString>();

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TRAITS TEST
        //
        // Concerns:
        // - Instantiating a 'pair' on a pair of types produce a type
        //   with type traits correctly derived from those types' traits.
        //
        // Plan:
        // - Instantiate 'pair' with a number of types, each having one
        //   or more of the type traits in which we are interested.
        // - Verify that the resulting instantiation has the following traits
        //   only if and only if both types on which it is instantiated have
        //   those traits:
        //   * bslmf::IsBitwiseMoveable
        //   * bsl::is_trivially_copyable (implies IsBitwiseMoveable)
        //   * bsl::is_trivially_default_constructible
        // - Verify that the resulting instantiation has the trait
        //   'bslma::UsesBslmaAllocator' if and only BOTH types have
        //   this trait.
        //
        // Testing:
        //   Type Traits
        // --------------------------------------------------------------------

        if (verbose) printf("\nTRAITS TEST"
                            "\n===========\n");

        if (verbose) {
            printf("Testing traits of "
                        "bsl::pair<my_NoTraits, my_NoTraits>\n");
        }
        typedef bsl::pair<my_NoTraits, my_NoTraits> Pair0;
        ASSERT(! (bslmf::IsBitwiseMoveable<               Pair0>::value));
        ASSERT(! (bsl::is_trivially_copyable<             Pair0>::value));
        ASSERT(! (bsl::is_trivially_default_constructible<Pair0>::value));
        ASSERT(! (bslma::UsesBslmaAllocator<              Pair0>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair0>::value));
        ASSERT(  (bslmf::IsPair<                          Pair0>::value));

        if (verbose) {
            printf("Testing traits of "
                        "bsl::pair<my_MoveAbandonBslma,"
                        "             my_MoveAbandonBslma>\n");
        }
        typedef bsl::pair<my_MoveAbandonBslma, my_MoveAbandonBslma> Pair1;
        ASSERT(  (bslmf::IsBitwiseMoveable<               Pair1>::value));
        ASSERT(! (bsl::is_trivially_copyable<             Pair1>::value));
        ASSERT(! (bsl::is_trivially_default_constructible<Pair1>::value));
        ASSERT(  (bslma::UsesBslmaAllocator<              Pair1>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair1>::value));
        ASSERT(  (bslmf::IsPair<                          Pair1>::value));

        if (verbose) {
            printf("Testing traits of "
                        "bsl::pair<my_CopyTrivial, my_CopyTrivial>\n");
        }
        typedef bsl::pair<my_CopyTrivial, my_CopyTrivial> Pair2;
        ASSERT(  (bslmf::IsBitwiseMoveable<               Pair2>::value));
        ASSERT(  (bsl::is_trivially_copyable<             Pair2>::value));
        ASSERT(  (bsl::is_trivially_default_constructible<Pair2>::value));
        ASSERT(! (bslma::UsesBslmaAllocator<              Pair2>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair2>::value));
        ASSERT(  (bslmf::IsPair<                          Pair2>::value));

        if (verbose) {
            printf("Testing traits of "
                        "bsl::pair<my_CopyTrivial, my_MoveAbandonBslma>\n");
        }
        typedef bsl::pair<my_CopyTrivial, my_MoveAbandonBslma> Pair3;
        ASSERT(  (bslmf::IsBitwiseMoveable<               Pair3>::value));
        ASSERT(! (bsl::is_trivially_copyable<             Pair3>::value));
        ASSERT(! (bsl::is_trivially_default_constructible<Pair3>::value));
        ASSERT(  (bslma::UsesBslmaAllocator<              Pair3>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair3>::value));
        ASSERT(  (bslmf::IsPair<                          Pair3>::value));

        if (verbose) {
            printf("Testing traits of "
                        "bsl::pair<my_MoveAbandonBslma, my_CopyTrivial>\n");
        }
        typedef bsl::pair<my_MoveAbandonBslma, my_CopyTrivial> Pair4;
        ASSERT(  (bslmf::IsBitwiseMoveable<               Pair4>::value));
        ASSERT(! (bsl::is_trivially_copyable<             Pair4>::value));
        ASSERT(! (bsl::is_trivially_default_constructible<Pair4>::value));
        ASSERT(  (bslma::UsesBslmaAllocator<              Pair4>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair4>::value));
        ASSERT(  (bslmf::IsPair<                          Pair4>::value));

        if (verbose) {
            printf("Testing traits of "
                        "bsl::pair<my_MoveAbandonBslma, my_NoTraits>\n");
        }
        typedef bsl::pair<my_MoveAbandonBslma, my_NoTraits> Pair5;
        ASSERT(! (bslmf::IsBitwiseMoveable<               Pair5>::value));
        ASSERT(! (bsl::is_trivially_copyable<             Pair5>::value));
        ASSERT(! (bsl::is_trivially_default_constructible<Pair5>::value));
        ASSERT(  (bslma::UsesBslmaAllocator<              Pair5>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair5>::value));
        ASSERT(  (bslmf::IsPair<                          Pair5>::value));

        if (verbose) {
            printf("Testing traits of "
                        "bsl::pair<my_CopyTrivial, my_NoTraits>\n");
        }
        typedef bsl::pair<my_CopyTrivial, my_NoTraits> Pair6;
        ASSERT(! (bslmf::IsBitwiseMoveable<               Pair6>::value));
        ASSERT(! (bsl::is_trivially_copyable<             Pair6>::value));
        ASSERT(! (bsl::is_trivially_default_constructible<Pair6>::value));
        ASSERT(! (bslma::UsesBslmaAllocator<              Pair6>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair6>::value));
        ASSERT(  (bslmf::IsPair<                          Pair6>::value));

        if (verbose) {
            printf("Testing traits of "
                        "bsl::pair<my_String, my_NoTraits>\n");
        }
        typedef bsl::pair<my_String, my_NoTraits> Pair7;
        ASSERT(! (bslmf::IsBitwiseMoveable<               Pair7>::value));
        ASSERT(! (bsl::is_trivially_copyable<             Pair7>::value));
        ASSERT(! (bsl::is_trivially_default_constructible<Pair7>::value));
        ASSERT(  (bslma::UsesBslmaAllocator<              Pair7>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair7>::value));
        ASSERT(  (bslmf::IsPair<                          Pair7>::value));

        if (verbose) {
            printf("Testing traits of "
                        "bsl::pair<my_MoveAbandonBslma, int>\n");
        }
        typedef bsl::pair<my_MoveAbandonBslma, int> Pair8;
        ASSERT(  (bslmf::IsBitwiseMoveable<               Pair8>::value));
        ASSERT(! (bsl::is_trivially_copyable<             Pair8>::value));
        ASSERT(! (bsl::is_trivially_default_constructible<Pair8>::value));
        ASSERT(  (bslma::UsesBslmaAllocator<              Pair8>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair8>::value));
        ASSERT(  (bslmf::IsPair<                          Pair8>::value));

        if (verbose) {
            printf("Testing traits of "
                        "bsl::pair<int, my_CopyTrivial>\n");
        }
        typedef bsl::pair<int, my_CopyTrivial> Pair9;
        ASSERT(  (bslmf::IsBitwiseMoveable<               Pair9>::value));
        ASSERT(  (bsl::is_trivially_copyable<             Pair9>::value));
        ASSERT(  (bsl::is_trivially_default_constructible<Pair9>::value));
        ASSERT(! (bslma::UsesBslmaAllocator<              Pair9>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair9>::value));
        ASSERT(  (bslmf::IsPair<                          Pair9>::value));

        if (verbose) {
            printf("Testing traits of "
                        "bsl::pair<my_String, my_MoveAbandonBslma>\n");
        }
        typedef bsl::pair<my_String, my_MoveAbandonBslma> Pair10;
        ASSERT(! (bslmf::IsBitwiseMoveable<               Pair10>::value));
        ASSERT(! (bsl::is_trivially_copyable<             Pair10>::value));
        ASSERT(! (bsl::is_trivially_default_constructible<Pair10>::value));
        ASSERT(  (bslma::UsesBslmaAllocator<              Pair10>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair10>::value));
        ASSERT(  (bslmf::IsPair<                          Pair10>::value));

        if (verbose) {
            printf("Testing traits of "
                        "bsl::pair<char, int>\n");
        }
        typedef bsl::pair<char, int> Pair11;
        ASSERT(  (bslmf::IsBitwiseMoveable<               Pair11>::value));
        ASSERT(  (bsl::is_trivially_copyable<             Pair11>::value));
        ASSERT(  (bsl::is_trivially_default_constructible<Pair11>::value));
        ASSERT(! (bslma::UsesBslmaAllocator<              Pair11>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair11>::value));
        ASSERT(  (bslmf::IsPair<                          Pair11>::value));

        if (verbose) {
            printf("Testing traits of "
                        "bsl::pair<int, char>\n");
        }
        typedef bsl::pair<int, char> Pair12;
        ASSERT(  (bslmf::IsBitwiseMoveable<               Pair12>::value));
        ASSERT(  (bsl::is_trivially_copyable<             Pair12>::value));
        ASSERT(  (bsl::is_trivially_default_constructible<Pair12>::value));
        ASSERT(! (bslma::UsesBslmaAllocator<              Pair12>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair12>::value));
        ASSERT(  (bslmf::IsPair<                          Pair12>::value));

        if (verbose) {
            printf("Testing traits of "
                        "bsl::pair<int, int>\n");
        }
        typedef bsl::pair<int, int> Pair13;
        ASSERT(  (bslmf::IsBitwiseMoveable<               Pair13>::value));
        ASSERT(  (bsl::is_trivially_copyable<             Pair13>::value));
        ASSERT(  (bsl::is_trivially_default_constructible<Pair13>::value));
        ASSERT(! (bslma::UsesBslmaAllocator<              Pair13>::value));
        ASSERT(  (bslmf::IsBitwiseEqualityComparable<     Pair13>::value));
        ASSERT(  (bslmf::IsPair<                          Pair13>::value));

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // FUNCTIONALITY TEST
        //
        // Concerns:
        //: 1 Can construct a 'pair' using the default constructor, constructor
        //:   with two arguments and copy constructor with no allocator.
        //: 2 If and only if one or both members of the 'pair' have the
        //:   'bslma::UsesBslmaAllocator' trait, then the 'pair' also has that
        //:   trait.
        //: 3 If 'pair' has the 'bslma::UsesBslmaAllocator' trait, then each
        //:   constructor can be passed a 'bslma::Allocator' pointer and that
        //:   pointer is passed through to the member(s) that take it.
        //: 4 Assignment works as designed.
        //: 5 Operators ==, !=, <, >, <=, and >= work as designed.
        //
        // Plan:
        // - Select a small set of interesting types:
        //   'short'              - Fundamental type
        //   'my_String'          - Uses 'bslma::Allocator' in constructor
        //   'my_BslmaAllocArgStr - Uses 'bslma::Allocator' in constructor by
        //                          means of the 'allocator_arg' idiom.
        //   'my_STLAllocArgStr   - Uses an STL-style allocator in constructor
        //                          by means of the 'allocator_arg' idiom.
        //   'my_NoAllocString'   - Doesn't use 'bslma::Allocator' in
        //                          constructor.
        // - Instantiate 'pair' with each combination (25 total) of the
        //   above types.
        // - For each instantiation, do the following:
        //   * Verify that the 'first_type' and 'second_type' typedefs are the
        //     types specified in the template parameters.
        //   * Set the default allocator to a test allocator
        //   * Construct objects using the three non-allocator constructors.
        //   * Verify that the 'first' and 'second' members are correct values.
        //   * Test the relationship operators
        //   * Test assignment
        //   * Test that the default allocator was used as expected.
        //   * Test that the special 'my_NoAllocString' allocator was used or
        //     not used as expected.
        //   * Verify that all memory is returned to the allocator(s) when the
        //     objects go out of scope.
        // - For instantiations where at least one member is of type
        //   'my_String', also do the following:
        //   * Construct objects using the three constructors with allocators.
        //   * Verify that memory was used from the allocators as expected.
        //   * Verify that no memory was used from the default allocator.
        //   * Test assignment among the new objects
        //
        // Testing:
        //   typedef T1 first_type;
        //   typedef T2 second_type;
        //   T1 first;
        //   T1 second;
        //   pair();
        //   pair(AllocatorPtr basicAllocator);
        //   pair(const T1& a, const T2& b);
        //   pair(const T1& a, const T2& b, AllocatorPtr basicAllocator);
        //   pair(const pair& original);
        //   pair(const pair& original, AllocatorPtr basicAllocator);
        //   ~pair();
        //   pair& operator=(const pair& rhs);
        //   bool operator==(const pair& x, const pair& y);
        //   bool operator!=(const pair& x, const pair& y);
        //   bool operator<(const pair& x, const pair& y);
        //   bool operator>(const pair& x, const pair& y);
        //   bool operator<=(const pair& x, const pair& y);
        //   bool operator>=(const pair& x, const pair& y);
        // --------------------------------------------------------------------

        if (verbose) printf("\nFUNCTIONALITY TEST"
                            "\n==================\n");

        ASSERT(! bslma::UsesBslmaAllocator<short              >::value);
        ASSERT(  bslma::UsesBslmaAllocator<my_String          >::value);
        ASSERT(  bslma::UsesBslmaAllocator<my_BslmaAllocArgStr>::value);
        ASSERT(! bslma::UsesBslmaAllocator<my_STLAllocArgStr  >::value);
        ASSERT(! bslma::UsesBslmaAllocator<my_NoAllocString   >::value);

#define TEST(T1, T2) do {                                               \
            if (veryVerbose) printf("Testing pair<%s,%s>\n", #T1, #T2); \
            testFunctionality<T1, T2>(                                  \
                bslma::UsesBslmaAllocator<bsl::pair<T1,T2> >());        \
        } while (false)

        TEST(short              , short              );
        TEST(short              , my_String          );
        TEST(short              , my_BslmaAllocArgStr);
        TEST(short              , my_STLAllocArgStr  );
        TEST(short              , my_NoAllocString   );

        TEST(my_String          , short              );
        TEST(my_String          , my_String          );
        TEST(my_String          , my_BslmaAllocArgStr);
        TEST(my_String          , my_STLAllocArgStr  );
        TEST(my_String          , my_NoAllocString   );

        TEST(my_BslmaAllocArgStr, short              );
        TEST(my_BslmaAllocArgStr, my_String          );
        TEST(my_BslmaAllocArgStr, my_BslmaAllocArgStr);
        TEST(my_BslmaAllocArgStr, my_STLAllocArgStr  );
        TEST(my_BslmaAllocArgStr, my_NoAllocString   );

        TEST(my_STLAllocArgStr  , short              );
        TEST(my_STLAllocArgStr  , my_String          );
        TEST(my_STLAllocArgStr  , my_BslmaAllocArgStr);
        TEST(my_STLAllocArgStr  , my_STLAllocArgStr  );
        TEST(my_STLAllocArgStr  , my_NoAllocString   );

        TEST(my_NoAllocString   , short              );
        TEST(my_NoAllocString   , my_String          );
        TEST(my_NoAllocString   , my_BslmaAllocArgStr);
        TEST(my_NoAllocString   , my_STLAllocArgStr  );
        TEST(my_NoAllocString   , my_NoAllocString   );

#undef TEST

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
        //: 1 Instantiate 'pair' with a simple string class that uses
        //:    'bslma::Allocator'.
        //: 2 Construct a objects using the test allocator.
        //: 3 Verify that object members have the correct value.
        //: 4 Verify that the correct allocator was used.
        //: 5 Verify that there are no memory leaks.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator ta1(veryVeryVerbose);
        bslma::TestAllocator ta2(veryVeryVerbose);

        bslma::DefaultAllocatorGuard allocGuard(&ta2);

        {
            typedef bsl::pair<my_String, short> Obj;
            ASSERT((bsl::is_same<my_String, Obj::first_type>::value));
            ASSERT((bsl::is_same<short, Obj::second_type>::value));

            ASSERT(! (bslmf::IsBitwiseMoveable<               Obj>::value));
            ASSERT(! (bsl::is_trivially_copyable<             Obj>::value));
            ASSERT(! (bsl::is_trivially_default_constructible<Obj>::value));
            ASSERT(  (bslma::UsesBslmaAllocator<              Obj>::value));

            const my_String NULL_FIRST("");
            const short     NULL_SECOND  = 0;
            const my_String VALUE_FIRST("Hello");
            const short     VALUE_SECOND = 4;

            Obj p1(&ta1); const Obj& P1 = p1;
            ASSERT(NULL_FIRST == P1.first && NULL_SECOND == P1.second);
            Obj p2(VALUE_FIRST, VALUE_SECOND, &ta1); const Obj& P2 = p2;
            ASSERT(VALUE_FIRST == P2.first && VALUE_SECOND == P2.second);
            Obj p3(p2, &ta1); const Obj& P3 = p3;
            ASSERT(VALUE_FIRST == P3.first && VALUE_SECOND == P3.second);

            ASSERT(3 == ta1.numBlocksInUse());

            p1 = p2;
            ASSERT(P1 == P2);

            ASSERT(3 == ta1.numBlocksInUse());
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());

      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n",
                     testStatus);
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
